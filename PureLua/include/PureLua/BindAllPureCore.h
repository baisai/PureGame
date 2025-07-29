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

namespace PureLua {
PURELUA_API void bind_core_buffer(lua_State* L);
PURELUA_API void bind_core_error_desc(lua_State* L);
PURELUA_API void bind_core_version(lua_State* L);
PURELUA_API void bind_core_geometry(lua_State* L);
PURELUA_API void bind_core_incr_id_gen(lua_State* L);
PURELUA_API void bind_core_os_helper(lua_State* L);
PURELUA_API void bind_core_pure_json(lua_State* L);
PURELUA_API void bind_core_pure_xml(lua_State* L);
PURELUA_API void bind_core_quad_tree(lua_State* L);
PURELUA_API void bind_core_random_gen(lua_State* L);
PURELUA_API void bind_core_rb_timer(lua_State* L);
PURELUA_API void bind_core_rect_tree(lua_State* L);
PURELUA_API void bind_core_reuse_id_gen(lua_State* L);
PURELUA_API void bind_core_snow_id_gen(lua_State* L);
PURELUA_API void bind_core_sleep_idler(lua_State* L);
PURELUA_API void bind_core_time_counter(lua_State* L);
PURELUA_API void bind_core_tw_timer(lua_State* L);
PURELUA_API void bind_core_utf_helper(lua_State* L);
PURELUA_API void bind_core_data_ref(lua_State* L);

PURELUA_API void bind_all_pure_core(lua_State* L);

}  // namespace PureLua
