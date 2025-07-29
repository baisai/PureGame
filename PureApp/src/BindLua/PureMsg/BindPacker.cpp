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

#include "PureMsg/Packer.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureApp {
void bind_msg_packer(lua_State* L) {
    using namespace PureMsg;
    PureLua::LuaModule lm(L, "PureMsg");
    lm.def(pack_uint8, "pack_uint8")
        .def(pack_uint16, "pack_uint16")
        .def(pack_uint32, "pack_uint32")
        .def(pack_uint64, "pack_uint64")
        .def(pack_int8, "pack_int8")
        .def(pack_int16, "pack_int16")
        .def(pack_int32, "pack_int32")
        .def(pack_int64, "pack_int64")
        .def(pack_fixed_uint8, "pack_fixed_uint8")
        .def(pack_fixed_uint16, "pack_fixed_uint16")
        .def(pack_fixed_uint32, "pack_fixed_uint32")
        .def(pack_fixed_uint64, "pack_fixed_uint64")
        .def(pack_fixed_int8, "pack_fixed_int8")
        .def(pack_fixed_int16, "pack_fixed_int16")
        .def(pack_fixed_int32, "pack_fixed_int32")
        .def(pack_fixed_int64, "pack_fixed_int64")
        .def(pack_char, "pack_char")
        .def(pack_signed_char, "pack_signed_char")
        .def(pack_short, "pack_short")
        .def(pack_int, "pack_int")
        .def(pack_long, "pack_long")
        .def(pack_long_long, "pack_long_long")
        .def(pack_unsigned_char, "pack_unsigned_char")
        .def(pack_unsigned_short, "pack_unsigned_short")
        .def(pack_unsigned_int, "pack_unsigned_int")
        .def(pack_unsigned_long, "pack_unsigned_long")
        .def(pack_unsigned_long_long, "pack_unsigned_long_long")
        .def(pack_float, "pack_float")
        .def(pack_double, "pack_double")
        .def(pack_nil, "pack_nil")
        .def(pack_bool, "pack_bool")
        .def(pack_array, "pack_array")
        .def(pack_map, "pack_map")
        .def(pack_string, "pack_string")
        .def(pack_string_ref, "pack_string_ref")
        .def(pack_data, "pack_data")
        .def(pack_data_ref, "pack_data_ref")
        .def(pack_obj, "pack_obj");
}

}  // namespace PureApp
