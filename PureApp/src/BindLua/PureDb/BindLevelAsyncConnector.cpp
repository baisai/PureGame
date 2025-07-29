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
#ifdef PURE_LEVELDB
#include "PureDb/LevelDb/LevelReply.h"
#include "PureDb/LevelDb/LevelAsyncConnector.h"

#include "PureLua/LuaRegisterClass.h"
#include "PureLua/LuaIterator.h"

namespace PureApp {
void bind_db_level_async_connector(lua_State* L) {
    using namespace PureDb;
    PureLua::LuaModule lm(L, "PureDb");
    lm[PureLua::LuaRegisterClass<LevelAsyncConnector>(L, "LevelAsyncConnector")
           .default_ctor()
           .def(&LevelAsyncConnector::stop, "stop")
           .def(&LevelAsyncConnector::init, "init")
           .def(&LevelAsyncConnector::update, "update")
           .def(&LevelAsyncConnector::connect, "connect")
           .def(&LevelAsyncConnector::vset, "vset")
           .def(&LevelAsyncConnector::vdel, "vdel")
           .def(&LevelAsyncConnector::vget, "vget")
           .def(
               [](lua_State* L) -> int {
                   LevelAsyncConnector& self = PureLua::LuaStack<LevelAsyncConnector&>::get(L, 1);
                   auto result = PureLua::LuaStack<std::function<void(LevelReplyPtr)>>::get(L, 2);
                   if (!lua_istable(L, 3)) {
                       PureLuaErrorJump(L, "3th arg must is a table");
                       return 0;
                   }
                   std::vector<std::pair<PureCore::StringRef, PureCore::StringRef>> datas;
                   for (PureLua::LuaIterator iter(L, 3); iter.is_over(); ++iter) {
                       datas.emplace_back();
                       datas.back().first = iter.get_key<PureCore::StringRef>();
                       datas.back().second = iter.get_value<PureCore::StringRef>();
                   }
                   int err = self.vmset(result, datas);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "vmset")
           .def(
               [](lua_State* L) -> int {
                   LevelAsyncConnector& self = PureLua::LuaStack<LevelAsyncConnector&>::get(L, 1);
                   auto result = PureLua::LuaStack<std::function<void(LevelReplyPtr)>>::get(L, 2);
                   if (!lua_istable(L, 3)) {
                       PureLuaErrorJump(L, "3th arg must is a array");
                       return 0;
                   }
                   int64_t len = lua_rawlen(L, 3);
                   std::vector<PureCore::StringRef> keys;
                   keys.reserve(len);
                   for (int64_t i = 1; i <= len; ++i) {
                       lua_rawgeti(L, 3, i);
                       auto key = PureLua::LuaStack<PureCore::StringRef>::get(L, -1);
                       keys.push_back(key);
                       lua_pop(L, 1);
                   }
                   int err = self.vmdel(result, keys);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "vmdel")
           .def(
               [](lua_State* L) -> int {
                   LevelAsyncConnector& self = PureLua::LuaStack<LevelAsyncConnector&>::get(L, 1);
                   auto result = PureLua::LuaStack<std::function<void(LevelReplyPtr)>>::get(L, 2);
                   if (!lua_istable(L, 3)) {
                       PureLuaErrorJump(L, "3th arg must is a array");
                       return 0;
                   }
                   int64_t len = lua_rawlen(L, 3);
                   std::vector<PureCore::StringRef> keys;
                   keys.reserve(len);
                   for (int64_t i = 1; i <= len; ++i) {
                       lua_rawgeti(L, 3, i);
                       auto key = PureLua::LuaStack<PureCore::StringRef>::get(L, -1);
                       keys.push_back(key);
                       lua_pop(L, 1);
                   }
                   int err = self.vmget(result, keys);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "vmget")
           .def(&LevelAsyncConnector::v_approximate_size, "v_approximate_size")
           .def(&LevelAsyncConnector::create_v_iterator, "create_v_iterator")
           .def(&LevelAsyncConnector::hset, "hset")
           .def(&LevelAsyncConnector::hdel, "hdel")
           .def(&LevelAsyncConnector::hget, "hget")
           .def(
               [](lua_State* L) -> int {
                   LevelAsyncConnector& self = PureLua::LuaStack<LevelAsyncConnector&>::get(L, 1);
                   auto result = PureLua::LuaStack<std::function<void(LevelReplyPtr)>>::get(L, 2);
                   PureCore::StringRef name = PureLua::LuaStack<PureCore::StringRef>::get(L, 3);
                   if (!lua_istable(L, 4)) {
                       PureLuaErrorJump(L, "4th arg must is a table");
                       return 0;
                   }
                   std::vector<std::pair<PureCore::StringRef, PureCore::StringRef>> datas;
                   for (PureLua::LuaIterator iter(L, 4); iter.is_over(); ++iter) {
                       datas.emplace_back();
                       datas.back().first = iter.get_key<PureCore::StringRef>();
                       datas.back().second = iter.get_value<PureCore::StringRef>();
                   }
                   int err = self.hmset(result, name, datas);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "hmset")
           .def(
               [](lua_State* L) -> int {
                   LevelAsyncConnector& self = PureLua::LuaStack<LevelAsyncConnector&>::get(L, 1);
                   auto result = PureLua::LuaStack<std::function<void(LevelReplyPtr)>>::get(L, 2);
                   PureCore::StringRef name = PureLua::LuaStack<PureCore::StringRef>::get(L, 3);
                   if (!lua_istable(L, 4)) {
                       PureLuaErrorJump(L, "4th arg must is a array");
                       return 0;
                   }
                   int64_t len = lua_rawlen(L, 4);
                   std::vector<PureCore::StringRef> keys;
                   keys.reserve(len);
                   for (int64_t i = 1; i <= len; ++i) {
                       lua_rawgeti(L, 4, i);
                       auto key = PureLua::LuaStack<PureCore::StringRef>::get(L, -1);
                       keys.push_back(key);
                       lua_pop(L, 1);
                   }
                   int err = self.hmdel(result, name, keys);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "hmdel")
           .def(
               [](lua_State* L) -> int {
                   LevelAsyncConnector& self = PureLua::LuaStack<LevelAsyncConnector&>::get(L, 1);
                   auto result = PureLua::LuaStack<std::function<void(LevelReplyPtr)>>::get(L, 2);
                   PureCore::StringRef name = PureLua::LuaStack<PureCore::StringRef>::get(L, 3);
                   if (!lua_istable(L, 4)) {
                       PureLuaErrorJump(L, "4th arg must is a array");
                       return 0;
                   }
                   int64_t len = lua_rawlen(L, 4);
                   std::vector<PureCore::StringRef> keys;
                   keys.reserve(len);
                   for (int64_t i = 1; i <= len; ++i) {
                       lua_rawgeti(L, 4, i);
                       auto key = PureLua::LuaStack<PureCore::StringRef>::get(L, -1);
                       keys.push_back(key);
                       lua_pop(L, 1);
                   }
                   int err = self.hmget(result, name, keys);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "hmget")
           .def(&LevelAsyncConnector::hgetall, "hgetall")
           .def(&LevelAsyncConnector::hdrop, "hdrop")
           .def(&LevelAsyncConnector::h_approximate_size, "h_approximate_size")
           .def(&LevelAsyncConnector::create_h_iterator, "create_h_iterator")
           .def(&LevelAsyncConnector::sset, "sset")
           .def(&LevelAsyncConnector::sdel, "sdel")
           .def(&LevelAsyncConnector::sget, "sget")
           .def(
               [](lua_State* L) -> int {
                   LevelAsyncConnector& self = PureLua::LuaStack<LevelAsyncConnector&>::get(L, 1);
                   auto result = PureLua::LuaStack<std::function<void(LevelReplyPtr)>>::get(L, 2);
                   PureCore::StringRef name = PureLua::LuaStack<PureCore::StringRef>::get(L, 3);
                   if (!lua_istable(L, 4)) {
                       PureLuaErrorJump(L, "4th arg must is a array");
                       return 0;
                   }
                   int64_t len = lua_rawlen(L, 4);
                   std::vector<PureCore::StringRef> keys;
                   keys.reserve(len);
                   for (int64_t i = 1; i <= len; ++i) {
                       lua_rawgeti(L, 4, i);
                       auto key = PureLua::LuaStack<PureCore::StringRef>::get(L, -1);
                       keys.push_back(key);
                       lua_pop(L, 1);
                   }
                   int err = self.smset(result, name, keys);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "smset")
           .def(
               [](lua_State* L) -> int {
                   LevelAsyncConnector& self = PureLua::LuaStack<LevelAsyncConnector&>::get(L, 1);
                   auto result = PureLua::LuaStack<std::function<void(LevelReplyPtr)>>::get(L, 2);
                   PureCore::StringRef name = PureLua::LuaStack<PureCore::StringRef>::get(L, 3);
                   if (!lua_istable(L, 4)) {
                       PureLuaErrorJump(L, "4th arg must is a array");
                       return 0;
                   }
                   int64_t len = lua_rawlen(L, 4);
                   std::vector<PureCore::StringRef> keys;
                   keys.reserve(len);
                   for (int64_t i = 1; i <= len; ++i) {
                       lua_rawgeti(L, 4, i);
                       auto key = PureLua::LuaStack<PureCore::StringRef>::get(L, -1);
                       keys.push_back(key);
                       lua_pop(L, 1);
                   }
                   int err = self.smdel(result, name, keys);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "smdel")
           .def(&LevelAsyncConnector::sgetall, "sgetall")
           .def(&LevelAsyncConnector::sdrop, "sdrop")
           .def(&LevelAsyncConnector::s_approximate_size, "s_approximate_size")
           .def(&LevelAsyncConnector::create_s_iterator, "create_s_iterator")
           .def(&LevelAsyncConnector::lset, "lset")
           .def(&LevelAsyncConnector::llpush, "llpush")
           .def(&LevelAsyncConnector::llpop, "llpop")
           .def(&LevelAsyncConnector::lrpush, "lrpush")
           .def(&LevelAsyncConnector::lrpop, "lrpop")
           .def(&LevelAsyncConnector::lget, "lget")
           .def(&LevelAsyncConnector::lrange, "lrange")
           .def(&LevelAsyncConnector::ldel, "ldel")
           .def(&LevelAsyncConnector::ltrim, "ltrim")
           .def(&LevelAsyncConnector::lgetall, "lgetall")
           .def(&LevelAsyncConnector::ldrop, "ldrop")
           .def(&LevelAsyncConnector::l_approximate_size, "l_approximate_size")
           .def(&LevelAsyncConnector::create_l_iterator, "create_l_iterator")
           .def(&LevelAsyncConnector::approximate_size, "approximate_size")
           .def(&LevelAsyncConnector::property, "property")
           .def(&LevelAsyncConnector::compact, "compact")
           .def([](LevelAsyncConnector& self, std::function<void(LevelReplyPtr)> cb, const char* path, int64_t batch,
                   int8_t compress) { return self.backup_to(cb, path, batch, ELevelBackupCompress(compress)); },
                "backup_to")
           .def(&LevelAsyncConnector::iterator, "iterator")
           .def(&LevelAsyncConnector::remove_iterator, "remove_iterator")];
}
}  // namespace PureApp
#endif