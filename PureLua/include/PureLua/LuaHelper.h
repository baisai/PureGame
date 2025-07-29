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

#include "PureLua/PureLuaLib.h"
#include "lua.hpp"

#include <string>

namespace PureLua {
PURELUA_API std::string dump_stack(lua_State* L);
PURELUA_API int push_lua_error_catcher(lua_State* L);
PURELUA_API const char* string_format(lua_State* L, int idx);

PURELUA_API int raw_getfield(lua_State* L, int index, const char* key);
PURELUA_API void raw_setfield(lua_State* L, int index, const char* key);
PURELUA_API int raw_getfieldlen(lua_State* L, int index, const char* key, size_t l);
PURELUA_API void raw_setfieldlen(lua_State* L, int index, const char* key, size_t l);

PURELUA_API int lua_setfenv(lua_State* L, int idx);
PURELUA_API void lua_getfenv(lua_State* L, int idx);

PURELUA_API bool isfulluserdata(lua_State* L, int index);
PURELUA_API void lock_metatable(lua_State* L, int index);

}  // namespace PureLua
