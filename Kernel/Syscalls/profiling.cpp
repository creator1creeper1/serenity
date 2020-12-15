/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <Kernel/CoreDump.h>
#include <Kernel/Process.h>
#include <Kernel/Profiling.h>

namespace Kernel {

int Process::sys$profiling_enable(pid_t pid)
{
    REQUIRE_NO_PROMISES;
    ScopedSpinLock lock(g_processes_lock);
    auto process = Process::from_pid(pid);
    if (!process)
        return -ESRCH;
    if (process->is_dead())
        return -ESRCH;
    if (!is_superuser() && process->uid() != m_uid)
        return -EPERM;
    Profiling::start(*process);
    process->set_profiling(true);
    return 0;
}

int Process::sys$profiling_disable(pid_t pid)
{
    ScopedSpinLock lock(g_processes_lock);
    auto process = Process::from_pid(pid);
    if (!process)
        return -ESRCH;
    if (!is_superuser() && process->uid() != m_uid)
        return -EPERM;
    process->set_profiling(false);
    Profiling::stop();

    // We explicitly unlock here because we can't hold the lock when writing the coredump VFS
    lock.unlock();

    if (auto coredump = CoreDump::create(*process, String::formatted("/tmp/profiler_coredumps/{}", pid)))
        coredump->write();
    else
        dbgln("Unable to create profiler coredump for PID {}", pid);
    return 0;
}

}
