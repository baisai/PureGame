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

#include "PureEncrypt/PureSha512.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureApp {
void bind_encrypt_pure_sha512(lua_State* L) {
    using namespace PureEncrypt;
    PureLua::LuaModule lm(L, "PureEncrypt");
    lm[PureLua::LuaRegisterClass<PureSha512>(L, "PureSha512")
           .default_ctor()
           .def(&PureSha512::encode, "encode")
           .def(&PureSha512::start_encode, "start_encode")
           .def(&PureSha512::update_encode, "update_encode")
           .def(&PureSha512::finish_encode, "finish_encode")
           .def(&PureSha512::get_output, "get_output")
           .def(&PureSha512::get_output_hex, "get_output_hex")
           .def(&PureSha512::get_output_HEX, "get_output_HEX")
           .def(&PureSha512::clear, "clear")];
}

}  // namespace PureApp
