/*
 * Copyright (c) 2020, the SerenityOS developers.
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

#include <AK/Concepts.h>
#include <AK/Forward.h>
#include <AK/Span.h>
#include <AK/StdLibExtras.h>

namespace AK::Detail {

class Stream {
public:
    virtual ~Stream()
    {
        ASSERT(!error() && !fatal());
    }

    bool error() const { return m_error; }
    bool fatal() const { return m_fatal; }

    bool handle_error() { return exchange(m_error, false); }

    operator bool() const { return !m_error && !m_fatal; }

protected:
    mutable bool m_error { false };
    mutable bool m_fatal { false };
};

}

namespace AK {

class InputStream : public AK::Detail::Stream {
public:
    virtual size_t read(Bytes) = 0;
    virtual bool read_or_error(Bytes) = 0;
    virtual bool eof() const = 0;
    virtual bool discard_or_error(size_t count) = 0;
};

// clang-format off

template<Concepts::Integral Integral>
InputStream& operator>>(InputStream& stream, Integral& value)
{
    stream.read_or_error({ &value, sizeof(value) });
    return stream;
}

#ifndef KERNEL
template<Concepts::FloatingPoint FloatingPoint>
InputStream& operator>>(InputStream& stream, FloatingPoint& value)
{
    stream.read_or_error({ &value, sizeof(value) });
    return stream;
}
#endif

// clang-format on

inline InputStream& operator>>(InputStream& stream, bool& value)
{
    stream.read_or_error({ &value, sizeof(value) });
    return stream;
}

inline InputStream& operator>>(InputStream& stream, Bytes bytes)
{
    stream.read_or_error(bytes);
    return stream;
}

class InputMemoryStream final : public InputStream {
    friend InputMemoryStream& operator>>(InputMemoryStream& stream, String& string);

public:
    InputMemoryStream(ReadonlyBytes bytes)
        : m_bytes(bytes)
    {
    }

    bool eof() const override { return m_offset >= m_bytes.size(); }

    size_t read(Bytes bytes) override
    {
        const auto count = min(bytes.size(), remaining());
        __builtin_memcpy(bytes.data(), m_bytes.data() + m_offset, count);
        m_offset += count;
        return count;
    }

    bool read_or_error(Bytes bytes) override
    {
        if (remaining() < bytes.size()) {
            m_error = true;
            return false;
        }

        __builtin_memcpy(bytes.data(), m_bytes.data() + m_offset, bytes.size());
        m_offset += bytes.size();
        return true;
    }

    bool discard_or_error(size_t count) override
    {
        if (remaining() < count) {
            m_error = true;
            return false;
        }

        m_offset += count;
        return true;
    }

    void seek(size_t offset)
    {
        ASSERT(offset < m_bytes.size());
        m_offset = offset;
    }

    u8 peek_or_error() const
    {
        if (remaining() == 0) {
            m_error = true;
            return 0;
        }

        return m_bytes[m_offset];
    }

    ReadonlyBytes bytes() const { return m_bytes; }
    size_t offset() const { return m_offset; }
    size_t remaining() const { return m_bytes.size() - m_offset; }

private:
    ReadonlyBytes m_bytes;
    size_t m_offset { 0 };
};

}

using AK::InputMemoryStream;
using AK::InputStream;
