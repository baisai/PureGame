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

#include "PureLua/PureLuaLog.h"
#include "PureLua/LuaClassInfo.h"
#include "PureLua/LuaUserdataGcTraits.h"

namespace PureLua {
template <typename NewerType>
void lua_class_set_newer(lua_State *L, int classIdx, NewerType newer) {
    classIdx = lua_absindex(L, classIdx);
    lua_pushlightuserdata(L, LuaClassMgr::get_newer_key());
    if (newer) {
        void *userdataPtr = lua_newuserdatauv(L, sizeof(newer), 0);
        lua_createtable(L, 0, 1);
        lua_pushstring(L, "__gc");
        lua_pushcclosure(L, lua_userdata_gc<NewerType>, 0);
        lua_rawset(L, -3);
        lua_setmetatable(L, -2);
        new (userdataPtr) NewerType(newer);
    } else {
        lua_pushnil(L);
    }
    lua_rawset(L, classIdx);
}

template <typename NewerType>
NewerType *lua_class_get_newer(lua_State *L, int classIdx) {
    classIdx = lua_absindex(L, classIdx);
    lua_pushlightuserdata(L, LuaClassMgr::get_newer_key());
    lua_rawget(L, classIdx);
    if (!lua_isuserdata(L, -1)) {
        lua_pop(L, 1);
        return nullptr;
    }
    auto newer = (NewerType *)lua_touserdata(L, -1);
    lua_pop(L, 1);
    return newer;
}

template <typename DelerType>
void lua_class_set_deler(lua_State *L, int classIdx, DelerType deler) {
    classIdx = lua_absindex(L, classIdx);
    lua_pushlightuserdata(L, LuaClassMgr::get_deler_key());
    if (deler) {
        void *userdataPtr = lua_newuserdatauv(L, sizeof(deler), 0);
        lua_createtable(L, 0, 1);
        lua_pushstring(L, "__gc");
        lua_pushcclosure(L, lua_userdata_gc<DelerType>, 0);
        lua_rawset(L, -3);
        lua_setmetatable(L, -2);
        new (userdataPtr) DelerType(deler);
    } else {
        lua_pushnil(L);
    }
    lua_rawset(L, classIdx);
}

template <typename DelerType>
DelerType *lua_class_get_deler(lua_State *L, int classIdx) {
    classIdx = lua_absindex(L, classIdx);
    lua_pushlightuserdata(L, LuaClassMgr::get_deler_key());
    lua_rawget(L, classIdx);
    if (!lua_isuserdata(L, -1)) {
        lua_pop(L, 1);
        return nullptr;
    }
    auto deler = (DelerType *)lua_touserdata(L, -1);
    lua_pop(L, 1);
    return deler;
}

}  // namespace PureLua
