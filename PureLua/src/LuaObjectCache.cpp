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

#include "PureLua/LuaStateProtect.h"
#include "PureLua/LuaClassMgr.h"
#include "PureLua/LuaObjectCache.h"

namespace PureLua {
#ifdef PURELUA_CACHE_OBJECT
void lua_set_cache_object(lua_State *L, void *key, void *obj, int objectIdx) {
    LuaStateProtect lsp(L);
    objectIdx = lua_absindex(L, objectIdx);
    lua_pushlightuserdata(L, LuaClassMgr::get_weak_key());
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_createtable(L, 0, 128);
        lua_pushstring(L, "__mode");
        lua_pushstring(L, "v");
        lua_rawset(L, -3);
        lua_pushvalue(L, -1);
        lua_setmetatable(L, -2);
        lua_pushlightuserdata(L, LuaClassMgr::get_weak_key());
        lua_pushvalue(L, -2);
        lua_rawset(L, LUA_REGISTRYINDEX);
    }
    void *tmp[] = {key, obj};
    lua_pushlstring(L, (const char *)tmp, sizeof(tmp));
    lua_pushvalue(L, objectIdx);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

bool lua_get_cache_object(lua_State *L, void *key, void *obj) {
    lua_pushlightuserdata(L, LuaClassMgr::get_weak_key());
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1)) {
        void *tmp[] = {key, obj};
        lua_pushlstring(L, (const char *)tmp, sizeof(tmp));
        lua_rawget(L, -2);
        lua_remove(L, -2);
        if (!lua_isnoneornil(L, -1)) {
            return true;
        }
    }
    lua_pop(L, 1);
    return false;
}

void lua_remove_cache_object(lua_State *L, void *key, void *obj) {
    lua_pushlightuserdata(L, LuaClassMgr::get_weak_key());
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1)) {
        void *tmp[] = {key, obj};
        lua_pushlstring(L, (const char *)tmp, sizeof(tmp));
        lua_pushnil(L);
        lua_rawset(L, -3);
    }
    lua_pop(L, 1);
}
#endif
}  // namespace PureLua
