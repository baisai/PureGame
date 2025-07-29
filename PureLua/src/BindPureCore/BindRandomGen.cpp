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

#include "PureCore/RandomGen.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureLua {
void bind_core_random_gen(lua_State* L) {
    using namespace PureCore;
    PureLua::LuaModule lm(L, "PureCore");
    lm[PureLua::LuaRegisterClass<RandomGen>(L, "RandomGen")
           .default_ctor()
           .def(&RandomGen::reset, "reset")
           .def(&RandomGen::gen_int, "gen_int")
           .def(&RandomGen::gen_float, "gen_float")
           .def(&RandomGen::gen_bytes, "gen_bytes")
           .def(&RandomGen::gen_less_int, "gen_less_int")
           .def(&RandomGen::gen_between_int, "gen_between_int")
           .def(&RandomGen::gen_less_float, "gen_less_float")
           .def(&RandomGen::gen_between_float, "gen_between_float")];
}
}  // namespace PureLua
