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

#include "PureMsg/MsgBuffer.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureApp {
void bind_msg_msg_buffer(lua_State* L) {
    using namespace PureMsg;
    PureLua::LuaModule lm(L, "PureMsg");
    lm[PureLua::LuaRegisterClass<MsgDynamicBuffer>(L, "MsgDynamicBuffer", PureLua::BaseClassStrategy<PureCore::DynamicBuffer>())
           .default_ctor()
           .def(&MsgDynamicBuffer::write_buffer, "write_buffer")
           .def(&MsgDynamicBuffer::read_buffer, "read_buffer") +
       PureLua::LuaRegisterClass<MsgFixedBuffer>(L, "MsgFixedBuffer", PureLua::BaseClassStrategy<PureCore::FixedBuffer>())
           .default_ctor<size_t>()
           .def(&MsgFixedBuffer::write_buffer, "write_buffer")
           .def(&MsgFixedBuffer::read_buffer, "read_buffer") +
       PureLua::LuaRegisterClass<MsgReferBuffer>(L, "MsgReferBuffer", PureLua::BaseClassStrategy<PureCore::ReferBuffer>())
           .default_ctor()
           .def(&MsgReferBuffer::write_buffer, "write_buffer")
           .def(&MsgReferBuffer::read_buffer, "read_buffer")];
}

}  // namespace PureApp
