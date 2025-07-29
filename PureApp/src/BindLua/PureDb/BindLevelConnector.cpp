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
#include "PureDb/LevelDb/LevelConnector.h"

#include "PureLua/LuaRegisterClass.h"
#include "PureLua/LuaIterator.h"

namespace PureApp {
void bind_db_level_connector(lua_State* L) {
    using namespace PureDb;
    PureLua::LuaModule lm(L, "PureDb");
    lm.def_const(PureDb::ELevelCompressLz4, "ELevelCompressLz4").def_const(PureDb::ELevelCompressZip, "ELevelCompressZip");
    lm[PureLua::LuaRegisterClass<LevelReply>(L, "LevelReply")
           .def_ctor(LevelReply::get, LevelReply::free)
           .def(&LevelReply::clear, "clear")
           .def(&LevelReply::is_error, "is_error")
           .def(&LevelReply::get_error, "get_error")
           .def(&LevelReply::set_error, "set_error")
           .def(&LevelReply::has_int, "has_int")
           .def(&LevelReply::get_int, "get_int")
           .def(&LevelReply::set_int, "set_int")
           .def(&LevelReply::has_bool, "has_bool")
           .def(&LevelReply::get_bool, "get_bool")
           .def(&LevelReply::set_bool, "set_bool")
           .def(&LevelReply::has_key, "has_key")
           .def(&LevelReply::get_key, "get_key")
           .def(&LevelReply::has_value, "has_value")
           .def(&LevelReply::get_value, "get_value")
           .def(&LevelReply::is_array, "is_array")
           .def(
               [](lua_State* L) -> int {
                   LevelReply& self = PureLua::LuaStack<LevelReply&>::get(L, 1);
                   auto arr = self.get_array();
                   lua_createtable(L, int(arr.size()), 0);
                   for (int i = 0; i < arr.size(); ++i) {
                       PureLua::LuaStack<LevelReply*>::push(L, arr[i].get());
                       lua_rawseti(L, -2, i + 1);
                   }
                   return 1;
               },
               "get_array")
           .def(&LevelReply::get_array_item, "get_array_item")
           .def(&LevelReply::get_array_size, "get_array_size") +
       PureLua::LuaRegisterClass<LevelConfig>(L, "LevelConfig")
           .default_ctor()
           .def(&LevelConfig::mPath, "path")
           .def(&LevelConfig::mEnableLog, "enable_log")
           .def(&LevelConfig::mCreateIfMiss, "create_if_miss")
           .def(&LevelConfig::mErrorIfExists, "error_if_exists")
           .def(&LevelConfig::mParanoidCheck, "paranoid_check")
           .def(&LevelConfig::mCacheSize, "cache_size")
           .def(&LevelConfig::mBloomFilter, "bloom_filter")
           .def(&LevelConfig::mWriteBufferSize, "write_buffer_size")
           .def(&LevelConfig::mMaxOpenFiles, "max_open_files")
           .def(&LevelConfig::mBlockSize, "block_size")
           .def(&LevelConfig::mMaxFileSize, "max_file_size")
           .def(&LevelConfig::mReadChecksums, "read_check_sums")
           .def(&LevelConfig::mReadFillCache, "read_fill_cache")
           .def(&LevelConfig::mWriteSync, "write_sync")
           .def(&LevelConfig::mIterKeepTime, "iter_keep_time")
           .def(&LevelConfig::pack, "pack")
           .def(&LevelConfig::unpack, "unpack") +
       PureLua::LuaRegisterClass<LevelConnector>(L, "LevelConnector")
           .default_ctor()
           .def(&LevelConnector::connect, "connect")
           .def(&LevelConnector::close, "close")
           .def(&LevelConnector::update, "update")
           .def(&LevelConnector::vset, "vset")
           .def(&LevelConnector::vdel, "vdel")
           .def(&LevelConnector::vget, "vget")
           .def(
               [](lua_State* L) -> int {
                   LevelConnector& self = PureLua::LuaStack<LevelConnector&>::get(L, 1);
                   if (!lua_istable(L, 2)) {
                       PureLuaErrorJump(L, "2th arg must is a table");
                       return 0;
                   }
                   std::vector<std::pair<PureCore::StringRef, PureCore::StringRef>> datas;
                   for (PureLua::LuaIterator iter(L, 2); iter.is_over(); ++iter) {
                       datas.emplace_back();
                       datas.back().first = iter.get_key<PureCore::StringRef>();
                       datas.back().second = iter.get_value<PureCore::StringRef>();
                   }
                   int err = self.vmset(datas);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "vmset")
           .def(
               [](lua_State* L) -> int {
                   LevelConnector& self = PureLua::LuaStack<LevelConnector&>::get(L, 1);
                   if (!lua_istable(L, 2)) {
                       PureLuaErrorJump(L, "2th arg must is a array");
                       return 0;
                   }
                   int64_t len = lua_rawlen(L, 2);
                   std::vector<PureCore::StringRef> keys;
                   keys.reserve(len);
                   for (int64_t i = 1; i <= len; ++i) {
                       lua_rawgeti(L, 2, i);
                       auto key = PureLua::LuaStack<PureCore::StringRef>::get(L, -1);
                       keys.push_back(key);
                       lua_pop(L, 1);
                   }
                   int err = self.vmdel(keys);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "vmdel")
           .def(
               [](lua_State* L) -> int {
                   LevelConnector& self = PureLua::LuaStack<LevelConnector&>::get(L, 1);
                   if (!lua_istable(L, 2)) {
                       PureLuaErrorJump(L, "2th arg must is a array");
                       return 0;
                   }
                   int64_t len = lua_rawlen(L, 2);
                   std::vector<PureCore::StringRef> keys;
                   keys.reserve(len);
                   for (int64_t i = 1; i <= len; ++i) {
                       lua_rawgeti(L, 2, i);
                       auto key = PureLua::LuaStack<PureCore::StringRef>::get(L, -1);
                       keys.push_back(key);
                       lua_pop(L, 1);
                   }
                   int err = self.vmget(keys);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "vmget")
           .def(&LevelConnector::v_approximate_size, "v_approximate_size")
           .def(&LevelConnector::create_v_iterator, "create_v_iterator")
           .def(&LevelConnector::hset, "hset")
           .def(&LevelConnector::hdel, "hdel")
           .def(&LevelConnector::hget, "hget")
           .def(
               [](lua_State* L) -> int {
                   LevelConnector& self = PureLua::LuaStack<LevelConnector&>::get(L, 1);
                   PureCore::StringRef name = PureLua::LuaStack<PureCore::StringRef>::get(L, 2);
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
                   int err = self.hmset(name, datas);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "hmset")
           .def(
               [](lua_State* L) -> int {
                   LevelConnector& self = PureLua::LuaStack<LevelConnector&>::get(L, 1);
                   PureCore::StringRef name = PureLua::LuaStack<PureCore::StringRef>::get(L, 2);
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
                   int err = self.hmdel(name, keys);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "hmdel")
           .def(
               [](lua_State* L) -> int {
                   LevelConnector& self = PureLua::LuaStack<LevelConnector&>::get(L, 1);
                   PureCore::StringRef name = PureLua::LuaStack<PureCore::StringRef>::get(L, 2);
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
                   int err = self.hmget(name, keys);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "hmget")
           .def(&LevelConnector::hgetall, "hgetall")
           .def(&LevelConnector::hdrop, "hdrop")
           .def(&LevelConnector::h_approximate_size, "h_approximate_size")
           .def(&LevelConnector::create_h_iterator, "create_h_iterator")
           .def(&LevelConnector::sset, "sset")
           .def(&LevelConnector::sdel, "sdel")
           .def(&LevelConnector::sget, "sget")
           .def(
               [](lua_State* L) -> int {
                   LevelConnector& self = PureLua::LuaStack<LevelConnector&>::get(L, 1);
                   PureCore::StringRef name = PureLua::LuaStack<PureCore::StringRef>::get(L, 2);
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
                   int err = self.smset(name, keys);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "smset")
           .def(
               [](lua_State* L) -> int {
                   LevelConnector& self = PureLua::LuaStack<LevelConnector&>::get(L, 1);
                   PureCore::StringRef name = PureLua::LuaStack<PureCore::StringRef>::get(L, 2);
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
                   int err = self.smdel(name, keys);
                   lua_pushinteger(L, err);
                   return 1;
               },
               "smdel")
           .def(&LevelConnector::sgetall, "sgetall")
           .def(&LevelConnector::sdrop, "sdrop")
           .def(&LevelConnector::s_approximate_size, "s_approximate_size")
           .def(&LevelConnector::create_s_iterator, "create_s_iterator")
           .def(&LevelConnector::lset, "lset")
           .def(&LevelConnector::llpush, "llpush")
           .def(&LevelConnector::llpop, "llpop")
           .def(&LevelConnector::lrpush, "lrpush")
           .def(&LevelConnector::lrpop, "lrpop")
           .def(&LevelConnector::lget, "lget")
           .def(&LevelConnector::lrange, "lrange")
           .def(&LevelConnector::ldel, "ldel")
           .def(&LevelConnector::ltrim, "ltrim")
           .def(&LevelConnector::lgetall, "lgetall")
           .def(&LevelConnector::ldrop, "ldrop")
           .def(&LevelConnector::l_approximate_size, "l_approximate_size")
           .def(&LevelConnector::create_l_iterator, "create_l_iterator")
           .def(&LevelConnector::approximate_size, "approximate_size")
           .def(&LevelConnector::property, "property")
           .def(&LevelConnector::compact, "compact")
           .def([](LevelConnector& self, const char* path, int64_t batch,
                   int8_t compress) { return self.backup_to(path, batch, ELevelBackupCompress(compress)); },
                "backup_to")
           .def(&LevelConnector::is_backuping, "is_backuping")
           .def(&LevelConnector::revert_from, "revert_from")
           .def(&LevelConnector::iterator, "iterator")
           .def(&LevelConnector::remove_iterator, "remove_iterator")
           .def(&LevelConnector::pop_reply, "pop_reply")
           .def(&LevelConnector::get_reply, "get_reply")
           .def(&LevelConnector::get_last_error, "get_last_error")];
}
}  // namespace PureApp
#endif