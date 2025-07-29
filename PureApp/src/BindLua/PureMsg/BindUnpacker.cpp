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

#include "PureCore/Buffer/DynamicBuffer.h"
#include "PureMsg/Unpacker.h"
#include "PureMsg/MsgErrorDesc.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureApp {
#define UnpackFunc(name, type)                                                        \
    [](lua_State* L) -> int {                                                         \
        PureCore::IBuffer& buffer = PureLua::LuaStack<PureCore::IBuffer&>::get(L, 1); \
        type val{};                                                                   \
        int err = name(buffer, val);                                                  \
        lua_pushinteger(L, err);                                                      \
        if (err != PureMsg::Success) {                                                \
            return 1;                                                                 \
        }                                                                             \
        PureLua::LuaStack<type>::push(L, val);                                        \
        return 2;                                                                     \
    },                                                                                \
        #name

void bind_msg_unpacker(lua_State* L) {
    using namespace PureMsg;
    PureLua::LuaModule lm(L, "PureMsg");
    lm.def(get_header, "get_header")
        .def(read_header, "read_header")
        .def(UnpackFunc(unpack_uint8, uint8_t))
        .def(UnpackFunc(unpack_uint16, uint16_t))
        .def(UnpackFunc(unpack_uint32, uint32_t))
        .def(UnpackFunc(unpack_uint64, uint64_t))
        .def(UnpackFunc(unpack_int8, int8_t))
        .def(UnpackFunc(unpack_int16, int16_t))
        .def(UnpackFunc(unpack_int32, int32_t))
        .def(UnpackFunc(unpack_int64, int64_t))
        .def(UnpackFunc(unpack_char, char))
        .def(UnpackFunc(unpack_signed_char, signed char))
        .def(UnpackFunc(unpack_short, short))
        .def(UnpackFunc(unpack_int, int))
        .def(UnpackFunc(unpack_long, long))
        .def(UnpackFunc(unpack_long_long, long long))
        .def(UnpackFunc(unpack_unsigned_char, unsigned char))
        .def(UnpackFunc(unpack_unsigned_short, unsigned short))
        .def(UnpackFunc(unpack_unsigned_int, unsigned int))
        .def(UnpackFunc(unpack_unsigned_long, unsigned long))
        .def(UnpackFunc(unpack_unsigned_long_long, unsigned long long))
        .def(UnpackFunc(unpack_float, float))
        .def(UnpackFunc(unpack_double, double))
        .def(unpack_nil, "unpack_nil")
        .def(unpack_skip, "unpack_skip")
        .def(UnpackFunc(unpack_bool, bool))
        .def(UnpackFunc(unpack_array, uint32_t))
        .def(UnpackFunc(unpack_map, uint32_t))
        .def(UnpackFunc(unpack_string, std::string))
        .def(UnpackFunc(unpack_string_ref, PureCore::StringRef))
        .def(UnpackFunc(unpack_data, PureCore::DynamicBuffer))
        .def(UnpackFunc(unpack_data_ref, PureCore::DataRef))
        .def(UnpackFunc(unpack_obj, uint32_t));
}
#undef UnpackFunc

}  // namespace PureApp
