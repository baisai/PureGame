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

#include "PureApp/PureExe.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureApp {
void bind_app_pure_exe(lua_State* L) {
    PureLua::LuaModule lm(L, "PureApp");
    lm[PureLua::LuaRegisterClass<PureExe>(L, "PureExe")
           .def(&PureExe::bin_dir, "bin_dir")
           .def(&PureExe::bin_name, "bin_name")
           .def(&PureExe::get_name, "get_name")
           .def(&PureExe::set_name, "set_name")
           .def(&PureExe::get_srcipt, "get_srcipt")
           .def(&PureExe::set_script, "set_script")
           .def(&PureExe::has_opt, "has_opt")
           .def(&PureExe::find_opt, "find_opt")];
}
}  // namespace PureApp