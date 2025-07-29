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

#include "PureNet/RouteMgr.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureApp {
void bind_net_route_mgr(lua_State* L) {
    using namespace PureNet;
    PureLua::LuaModule lm(L, "PureNet");
    lm[PureLua::LuaRegisterClass<RouteMgr>(L, "RouteMgr")
           .default_ctor()
           .def(&RouteMgr::clear, "clear")
           .def(&RouteMgr::add_route_by_user, "add_route_by_user")
           .def(&RouteMgr::add_route_by_server, "add_route_by_server")
           .def(&RouteMgr::remove_route, "remove_route")
           .def(&RouteMgr::find_route_by_user, "find_route_by_user")
           .def(&RouteMgr::find_route_by_server, "find_route_by_server")
           .def(&RouteMgr::find_server_type_routes, "find_server_type_routes")
           .def(&RouteMgr::find_server_type_id_routes, "find_server_type_id_routes")];
}

}  // namespace PureApp
