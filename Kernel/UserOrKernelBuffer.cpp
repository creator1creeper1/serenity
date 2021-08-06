/*
 * Copyright (c) 2020, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/UserOrKernelBuffer.h>

namespace Kernel {

bool UserOrKernelBuffer::is_kernel_buffer() const
{
    return !is_user_address(VirtualAddress(m_buffer));
}

String UserOrKernelBuffer::copy_into_string(size_t size) const
{
    if (!m_buffer)
        return {};
    if (is_user_address(VirtualAddress(m_buffer))) {
        char* buffer;
        auto data_copy = StringImpl::create_uninitialized(size, buffer);
        if (!copy_from_user(buffer, m_buffer, size))
            return {};
        return data_copy;
    }

    return String(ReadonlyBytes { m_buffer, size });
}

KResultOr<NonnullOwnPtr<KString>> UserOrKernelBuffer::try_copy_into_kstring(size_t size) const
{
    if (!m_buffer)
        return EINVAL;
    if (is_user_address(VirtualAddress(m_buffer))) {
        char* buffer;
        auto kstring = KString::try_create_uninitialized(size, buffer);
        if (!kstring)
            return ENOMEM;
        if (!copy_from_user(buffer, m_buffer, size))
            return EFAULT;
        return kstring.release_nonnull();
    }

    auto kstring = KString::try_create(ReadonlyBytes { m_buffer, size });
    if (!kstring)
        return ENOMEM;
    return kstring.release_nonnull();
}

bool UserOrKernelBuffer::write(const void* src, size_t offset, size_t len)
{
    if (!m_buffer)
        return false;

    if (is_user_address(VirtualAddress(m_buffer)))
        return copy_to_user(m_buffer + offset, src, len);

    memcpy(m_buffer + offset, src, len);
    return true;
}

bool UserOrKernelBuffer::read(void* dest, size_t offset, size_t len) const
{
    if (!m_buffer)
        return false;

    if (is_user_address(VirtualAddress(m_buffer)))
        return copy_from_user(dest, m_buffer + offset, len);

    memcpy(dest, m_buffer + offset, len);
    return true;
}

bool UserOrKernelBuffer::memset(int value, size_t offset, size_t len)
{
    if (!m_buffer)
        return false;

    if (is_user_address(VirtualAddress(m_buffer)))
        return memset_user(m_buffer + offset, value, len);

    ::memset(m_buffer + offset, value, len);
    return true;
}

}
