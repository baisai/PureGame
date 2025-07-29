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
#ifdef PURE_REDIS
#include "PureDb/Redis/RedisAsyncConnector.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureApp {
void bind_db_redis_async_connector(lua_State* L) {
    using namespace PureDb;
    PureLua::LuaModule lm(L, "PureDb");
    lm[PureLua::LuaRegisterClass<RedisAsyncConnector>(L, "RedisAsyncConnector")
           .default_ctor()
           .def(&RedisAsyncConnector::stop, "stop")
           .def(&RedisAsyncConnector::init, "init")
           .def(&RedisAsyncConnector::update, "update")
           .def(&RedisAsyncConnector::is_busying, "is_busying")
           .def(&RedisAsyncConnector::connect, "connect")
           .def(
               [](lua_State* L) -> int {
                   RedisAsyncConnector& self = PureLua::LuaStack<RedisAsyncConnector&>::get(L, 1);
                   PureCore::StringRef cmd = PureLua::LuaStack<PureCore::StringRef>::get(L, 2);
                   int count = lua_gettop(L);
                   std::vector<PureCore::StringRef> params;
                   if (count >= 3) {
                       params.resize(count - 2);
                       for (int i = 3; i <= count; ++i) {
                           params.push_back(PureLua::LuaStack<PureCore::StringRef>::get(L, i));
                       }
                   }
                   int err = self.add_pipe(cmd, params);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "add_pipe")
           .def(&RedisAsyncConnector::commit_pipe, "commit_pipe")
           .def(&RedisAsyncConnector::cancel_pipe, "cancel_pipe")
           .def(
               [](lua_State* L) -> int {
                   RedisAsyncConnector& self = PureLua::LuaStack<RedisAsyncConnector&>::get(L, 1);
                   std::function<void(RedisReplyPtr)> cb = PureLua::LuaStack<std::function<void(RedisReplyPtr)>>::get(L, 2);
                   PureCore::StringRef cmd = PureLua::LuaStack<PureCore::StringRef>::get(L, 3);
                   int count = lua_gettop(L);
                   std::vector<PureCore::StringRef> params;
                   if (count >= 4) {
                       params.resize(count - 3);
                       for (int i = 4; i <= count; ++i) {
                           params.push_back(PureLua::LuaStack<PureCore::StringRef>::get(L, i));
                       }
                   }
                   int err = self.command(cb, cmd, params);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "command")];
}
}  // namespace PureApp
#endif