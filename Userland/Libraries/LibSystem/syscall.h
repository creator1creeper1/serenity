/*
 * Copyright (c) 2021, Andreas Kling <kling@serenityos.org>
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

#pragma once

#include <Kernel/API/Syscall.h>
#include <sys/types.h>

extern "C" {

uintptr_t syscall0(uintptr_t function);
uintptr_t syscall1(uintptr_t function, uintptr_t arg0);
uintptr_t syscall2(uintptr_t function, uintptr_t arg0, uintptr_t arg1);
uintptr_t syscall3(uintptr_t function, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2);
}

#ifdef __cplusplus

inline uintptr_t syscall(auto function)
{
    return syscall0(function);
}

inline uintptr_t syscall(auto function, auto arg0)
{
    return syscall1((uintptr_t)function, (uintptr_t)arg0);
}

inline uintptr_t syscall(auto function, auto arg0, auto arg1)
{
    return syscall2((uintptr_t)function, (uintptr_t)arg0, (uintptr_t)arg1);
}

inline uintptr_t syscall(auto function, auto arg0, auto arg1, auto arg2)
{
    return syscall3((uintptr_t)function, (uintptr_t)arg0, (uintptr_t)arg1, (uintptr_t)arg2);
}

#endif
