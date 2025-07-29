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

#include "PureApp/PureAppLib.h"

#include "lua.hpp"

namespace PureApp {
PUREAPP_API void bind_net_net_config(lua_State* L);
PUREAPP_API void bind_net_error_desc(lua_State* L);
PUREAPP_API void bind_net_net_msg(lua_State* L);
PUREAPP_API void bind_net_version(lua_State* L);
PUREAPP_API void bind_net_pure_net_process(lua_State* L);
PUREAPP_API void bind_net_pure_net_thread(lua_State* L);
PUREAPP_API void bind_net_route_mgr(lua_State* L);

PUREAPP_API void bind_all_pure_net(lua_State* L);

}  // namespace PureApp
