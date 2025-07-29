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
#include "PureDb/Redis/RedisConnector.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureApp {
void bind_db_redis_connector(lua_State* L) {
    using namespace PureDb;
    PureLua::LuaModule lm(L, "PureDb");
    lm[PureLua::LuaRegisterClass<RedisReply>(L, "RedisReply")
           .def_ctor(RedisReply::get, RedisReply::free)
           .def(&RedisReply::clear, "clear")
           .def(&RedisReply::is_bool, "is_bool")
           .def(&RedisReply::get_bool, "get_bool")
           .def(&RedisReply::is_nil, "is_nil")
           .def(&RedisReply::get_nil, "get_nil")
           .def(&RedisReply::is_int, "is_int")
           .def(&RedisReply::get_int, "get_int")
           .def(&RedisReply::is_double, "is_double")
           .def(&RedisReply::get_double, "get_double")
           .def(&RedisReply::is_error, "is_error")
           .def(&RedisReply::get_error, "get_error")
           .def(&RedisReply::is_str, "is_str")
           .def(&RedisReply::get_str, "get_str")
           .def(&RedisReply::is_array, "is_array")
           .def(
               [](lua_State* L) -> int {
                   RedisReply& self = PureLua::LuaStack<RedisReply&>::get(L, 1);
                   auto arr = self.get_array();
                   lua_createtable(L, int(arr.size()), 0);
                   for (int i = 0; i < arr.size(); ++i) {
                       PureLua::LuaStack<RedisReply*>::push(L, arr[i].get());
                       lua_rawseti(L, -2, i + 1);
                   }
                   return 1;
               },
               "get_array")
           .def(&RedisReply::get_array_item, "get_array_item")
           .def(&RedisReply::set_bool, "set_bool")
           .def(&RedisReply::set_nil, "set_nil")
           .def(&RedisReply::set_int, "set_int")
           .def(&RedisReply::set_double, "set_double")
           .def(&RedisReply::set_error, "set_error")
           .def(&RedisReply::set_str, "set_str")
           .def(&RedisReply::capacity_array, "capacity_array")
           .def(&RedisReply::append_array, "append_array") +
       PureLua::LuaRegisterClass<RedisConfig>(L, "RedisConfig")
           .default_ctor()
           .def(&RedisConfig::mIp, "ip")
           .def(&RedisConfig::mPort, "port")
           .def(&RedisConfig::mPass, "pass")
           .def(&RedisConfig::mDbIndex, "db_index")
           .def(&RedisConfig::mConnectTimeout, "connect_timeout")
           .def(&RedisConfig::mCommandTimeout, "command_timeout")
           .def(&RedisConfig::mKeepAliveInterval, "keep_alive_interval")
           .def(&RedisConfig::pack, "pack")
           .def(&RedisConfig::unpack, "unpack") +
       PureLua::LuaRegisterClass<RedisConnector>(L, "RedisConnector")
           .default_ctor()
           .def(&RedisConnector::is_alive, "is_alive")
           .def(&RedisConnector::connect, "connect")
           .def(&RedisConnector::close, "close")
           .def(&RedisConnector::update, "update")
           .def(
               [](lua_State* L) -> int {
                   RedisConnector& self = PureLua::LuaStack<RedisConnector&>::get(L, 1);
                   PureCore::StringRef cmd = PureLua::LuaStack<PureCore::StringRef>::get(L, 2);
                   int count = lua_gettop(L);
                   std::vector<PureCore::StringRef> params;
                   if (count >= 3) {
                       params.resize(count - 2);
                       for (int i = 3; i <= count; ++i) {
                           params.push_back(PureLua::LuaStack<PureCore::StringRef>::get(L, i));
                       }
                   }
                   int err = self.pipe(cmd, params);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "pipe")
           .def(
               [](lua_State* L) -> int {
                   RedisConnector& self = PureLua::LuaStack<RedisConnector&>::get(L, 1);
                   PureCore::StringRef cmd = PureLua::LuaStack<PureCore::StringRef>::get(L, 2);
                   int count = lua_gettop(L);
                   std::vector<PureCore::StringRef> params;
                   if (count >= 3) {
                       params.resize(count - 2);
                       for (int i = 3; i <= count; ++i) {
                           params.push_back(PureLua::LuaStack<PureCore::StringRef>::get(L, i));
                       }
                   }
                   int err = self.command(cmd, params);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "command")
           .def(&RedisConnector::pop_reply, "pop_reply")];
}
}  // namespace PureApp
#endif