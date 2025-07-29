/*
 * Copyright (c) 2023-present ChenDong, email <baisaichen@live.com>. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include "PureCore/Buffer/IBuffer.h"

namespace PureMsg {
template <typename TBuffer>
class MsgBuffer : public TBuffer {
public:
    template <typename... Args>
    MsgBuffer(Args... args);
    MsgBuffer(const MsgBuffer& cp);
    MsgBuffer& operator=(const MsgBuffer& right);
    virtual ~MsgBuffer();

    int write_buffer(PureCore::IBuffer& src);

    template <typename T>
    int pack(const T& data);
    int pack(const char* data);

    int read_buffer(PureCore::IBuffer& dest);

    template <typename T>
    int unpack(T& data);
    int unpack(char* data);

    template <typename T>
    MsgBuffer& operator<<(const T& src);

    MsgBuffer& operator<<(const char* src);

    template <typename T>
    MsgBuffer& operator>>(T& dest);

    MsgBuffer& operator>>(char* dest);
};

}  // namespace PureMsg

#include "PureMsg/MsgBuffer.inl.h"
