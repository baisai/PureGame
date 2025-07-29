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

#include "PureCore/Buffer/DynamicBuffer.h"
#include "PureCore/Buffer/FixedBuffer.h"
#include "PureCore/Buffer/ArrayBuffer.h"
#include "PureCore/Buffer/ReferBuffer.h"
#include "PureMsg/MsgBuffer.h"
#include "PureMsg/PackerAdapter.h"
#include "PureMsg/UnpackerAdapter.h"

namespace PureMsg {
template <typename TBuffer>
template <typename... Args>
inline MsgBuffer<TBuffer>::MsgBuffer(Args... args) : TBuffer(args...) {}

template <typename TBuffer>
inline MsgBuffer<TBuffer>::MsgBuffer(const MsgBuffer& cp) : TBuffer(cp) {}

template <typename TBuffer>
inline MsgBuffer<TBuffer>& MsgBuffer<TBuffer>::operator=(const MsgBuffer& right) {
    TBuffer::operator=(right);
    return (*this);
}

template <typename TBuffer>
inline MsgBuffer<TBuffer>::~MsgBuffer() {}

template <typename TBuffer>
inline int MsgBuffer<TBuffer>::write_buffer(PureCore::IBuffer& src) {
    int err = this->write(src.data());
    if (err != Success) {
        return err;
    }
    src.read_pos(src.write_pos());
    return Success;
}

template <typename TBuffer>
template <typename T>
inline int MsgBuffer<TBuffer>::pack(const T& data) {
    return PureMsg::pack(*this, data);
}

template <typename TBuffer>
inline int MsgBuffer<TBuffer>::pack(const char* data) {
    return PureMsg::pack(*this, data);
}

template <typename TBuffer>
inline int MsgBuffer<TBuffer>::read_buffer(PureCore::IBuffer& dest) {
    int err = dest.write(this->data());
    if (err != Success) {
        return err;
    }
    this->read_pos(this->write_pos());
    return Success;
}

template <typename TBuffer>
template <typename T>
inline int MsgBuffer<TBuffer>::unpack(T& data) {
    return PureMsg::unpack(*this, data);
}

template <typename TBuffer>
inline int MsgBuffer<TBuffer>::unpack(char* data) {
    return PureMsg::unpack(*this, data);
}

template <typename TBuffer>
template <typename T>
inline MsgBuffer<TBuffer>& MsgBuffer<TBuffer>::operator<<(const T& data) {
    int e = PureMsg::pack(*this, data);
    if (e != Success) {
        throw EPureMsgErrorCode(e);
    }
    return (*this);
}

template <typename TBuffer>
inline MsgBuffer<TBuffer>& MsgBuffer<TBuffer>::operator<<(const char* data) {
    int e = PureMsg::pack(*this, data);
    if (e != Success) {
        throw EPureMsgErrorCode(e);
    }
    return (*this);
}

template <typename TBuffer>
template <typename T>
inline MsgBuffer<TBuffer>& MsgBuffer<TBuffer>::operator>>(T& data) {
    int e = PureMsg::unpack(*this, data);
    if (e != Success) {
        throw EPureMsgErrorCode(e);
    }
    return (*this);
}

template <typename TBuffer>
inline MsgBuffer<TBuffer>& MsgBuffer<TBuffer>::operator>>(char* data) {
    int e = PureMsg::unpack(*this, data);
    if (e != Success) {
        throw EPureMsgErrorCode(e);
    }
    return (*this);
}

using MsgDynamicBuffer = MsgBuffer<PureCore::DynamicBuffer>;
using MsgFixedBuffer = MsgBuffer<PureCore::FixedBuffer>;
template <size_t TSize>
using MsgArrayBuffer = MsgBuffer<PureCore::ArrayBuffer<TSize>>;
using MsgReferBuffer = MsgBuffer<PureCore::ReferBuffer>;

}  // namespace PureMsg
