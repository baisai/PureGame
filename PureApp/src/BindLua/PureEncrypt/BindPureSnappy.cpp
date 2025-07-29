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

#include "PureEncrypt/PureSnappy.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureApp {
void bind_encrypt_pure_snappy(lua_State* L) {
    using namespace PureEncrypt;
    PureLua::LuaModule lm(L, "PureEncrypt");
    lm[PureLua::LuaRegisterClass<PureSnappy>(L, "PureSnappy")
           .default_ctor()
           .def(&PureSnappy::encode, "encode")
           .def(&PureSnappy::decode, "decode")
           .def(&PureSnappy::get_output, "get_output") +
       PureLua::LuaRegisterClass<PureSnappyBlockEncoder>(L, "PureSnappyBlockEncoder")
           .default_ctor()
           .def(&PureSnappyBlockEncoder::set_block_size, "set_block_size")
           .def(&PureSnappyBlockEncoder::get_block_size, "get_block_size")
           .def(&PureSnappyBlockEncoder::encode, "encode")
           .def(&PureSnappyBlockEncoder::start_encode, "start_encode")
           .def(&PureSnappyBlockEncoder::update_encode, "update_encode")
           .def(&PureSnappyBlockEncoder::flush_encode, "flush_encode")
           .def(&PureSnappyBlockEncoder::finish_encode, "finish_encode")
           .def(&PureSnappyBlockEncoder::get_output, "get_output")
           .def(&PureSnappyBlockEncoder::clear, "clear") +
       PureLua::LuaRegisterClass<PureSnappyBlockDecoder>(L, "PureSnappyBlockDecoder")
           .default_ctor()
           .def(&PureSnappyBlockDecoder::decode, "decode")
           .def(&PureSnappyBlockDecoder::start_decode, "start_decode")
           .def(&PureSnappyBlockDecoder::update_decode, "update_decode")
           .def(&PureSnappyBlockDecoder::finish_decode, "finish_decode")
           .def(&PureSnappyBlockDecoder::get_output, "get_output")
           .def(&PureSnappyBlockDecoder::clear, "clear")];
}

}  // namespace PureApp
