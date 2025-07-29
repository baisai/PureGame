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
#include "PureMsg/MsgBuffer.h"
#include "PureMsg/PackerAdapter.h"
#include "PureMsg/UnpackerAdapter.h"

#define PUREMSG_CLASS(...)                                                                                  \
    inline int pack(PureCore::IBuffer& buffer) const { return PureMsg::pack_class(buffer, ##__VA_ARGS__); } \
    inline int unpack(PureCore::IBuffer& buffer) { return PureMsg::unpack_class(buffer, ##__VA_ARGS__); }

namespace PureMsg {
namespace __ClassMembersDetail {
inline int pack_members(PureCore::IBuffer& buffer) { return Success; }

template <typename Arg1, typename... Args>
inline int pack_members(PureCore::IBuffer& buffer, const Arg1& arg1, Args&&... args) {
    int err = PureMsg::pack(buffer, arg1);
    if (err != Success) {
        return err;
    }
    return pack_members(buffer, std::forward<Args>(args)...);
}

template <typename T, bool>
struct unpack_member_const;

template <typename T>
struct unpack_member_const<T, true> {
    static int unpack(PureCore::IBuffer& buffer, T& t) { return PureMsg::unpack_skip(buffer); }
};
template <typename T>
struct unpack_member_const<T, false> {
    static int unpack(PureCore::IBuffer& buffer, T& t) { return PureMsg::unpack(buffer, t); }
};

inline int unpack_members(PureCore::IBuffer& buffer, uint32_t count) {
    for (uint32_t i = 0; i != count; ++i) {
        PureMsg::unpack_skip(buffer);
    }
    return Success;
}

template <typename Arg1, typename... Args>
inline int unpack_members(PureCore::IBuffer& buffer, uint32_t count, Arg1& arg1, Args&&... args) {
    if (count == 0) {
        return Success;
    }
    int err = unpack_member_const<Arg1, std::is_const<Arg1>::value>::unpack(buffer, arg1);
    if (err != Success) {
        return err;
    }
    return unpack_members(buffer, count - 1, std::forward<Args>(args)...);
}
}  // namespace __ClassMembersDetail

template <typename... Args>
inline int pack_class(PureCore::IBuffer& buffer, Args&&... args) {
    uint32_t count = static_cast<uint32_t>(sizeof...(args));
    int err = PureMsg::pack_obj(buffer, count);
    if (err != Success) {
        return err;
    }
    return __ClassMembersDetail::pack_members(buffer, std::forward<Args>(args)...);
}

template <typename... Args>
inline int unpack_class(PureCore::IBuffer& buffer, Args&&... args) {
    uint32_t count = 0;
    int err = PureMsg::unpack_obj(buffer, count);
    if (err != Success) {
        return err;
    }
    return __ClassMembersDetail::unpack_members(buffer, count, std::forward<Args>(args)...);
}
}  // namespace PureMsg