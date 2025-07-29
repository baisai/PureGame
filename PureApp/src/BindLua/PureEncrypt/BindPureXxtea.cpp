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

#include "PureEncrypt/PureXxtea.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureApp {
void bind_encrypt_pure_xxtea(lua_State* L) {
    using namespace PureEncrypt;
    PureLua::LuaModule lm(L, "PureEncrypt");
    lm[PureLua::LuaRegisterClass<PureXxtea>(L, "PureXxtea")
           .default_ctor()
           .def(&PureXxtea::encode, "encode")
           .def(&PureXxtea::decode, "decode")
           .def(&PureXxtea::get_output, "get_output") +
       PureLua::LuaRegisterClass<PureXxteaBlockEncoder>(L, "PureXxteaBlockEncoder")
           .default_ctor()
           .def(&PureXxteaBlockEncoder::set_block_size, "set_block_size")
           .def(&PureXxteaBlockEncoder::get_block_size, "get_block_size")
           .def(&PureXxteaBlockEncoder::encode, "encode")
           .def(&PureXxteaBlockEncoder::start_encode, "start_encode")
           .def(&PureXxteaBlockEncoder::update_encode, "update_encode")
           .def(&PureXxteaBlockEncoder::flush_encode, "flush_encode")
           .def(&PureXxteaBlockEncoder::finish_encode, "finish_encode")
           .def(&PureXxteaBlockEncoder::get_output, "get_output")
           .def(&PureXxteaBlockEncoder::clear, "clear") +
       PureLua::LuaRegisterClass<PureXxteaBlockDecoder>(L, "PureXxteaBlockDecoder")
           .default_ctor()
           .def(&PureXxteaBlockDecoder::decode, "decode")
           .def(&PureXxteaBlockDecoder::start_decode, "start_decode")
           .def(&PureXxteaBlockDecoder::update_decode, "update_decode")
           .def(&PureXxteaBlockDecoder::finish_decode, "finish_decode")
           .def(&PureXxteaBlockDecoder::get_output, "get_output")
           .def(&PureXxteaBlockDecoder::clear, "clear")];
}

}  // namespace PureApp
