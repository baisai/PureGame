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
#include "PureLua/LuaObjectCache.h"

namespace PureLua {
template <typename T>
int lua_ctor_object_userdata(lua_State *L, int objectIdx, const T *obj, int32_t offset, bool luaGC) {
    if (obj == nullptr) {
        return 0;
    }

    objectIdx = lua_absindex(L, objectIdx);
    lua_pushlightuserdata(L, LuaClassMgr::get_object_key());
    void *userdataPtr = lua_newuserdatauv(L, sizeof(LuaUserdataForClass), 0);
    lua_pushvalue(L, lua_upvalueindex(1));
    auto deler = lua_class_get_deler<std::function<void(T *)>>(L, -1);
    lua_setmetatable(L, -2);
    lua_rawset(L, objectIdx);
    new (userdataPtr) LuaUserdataForClass(obj, offset, deler, luaGC);
#ifdef PURELUA_CACHE_OBJECT
    lua_set_cache_object(L, LuaClassInfo<T>::get_class_key(), (void *)obj, objectIdx);
#endif
    return 0;
}

template <typename T>
int lua_new_object_userdata(lua_State *L, int classIdx, const T *obj, int32_t offset, bool luaGC) {
    if (obj == nullptr) {
        lua_pushnil(L);
        return 1;
    }
    classIdx = lua_absindex(L, classIdx);
    auto deler = lua_class_get_deler<std::function<void(T *)>>(L, classIdx);
    void *userdataPtr = lua_newuserdatauv(L, sizeof(LuaUserdataForClass), 0);
    lua_pushvalue(L, classIdx);
    lua_setmetatable(L, -2);
    new (userdataPtr) LuaUserdataForClass(obj, offset, deler, luaGC);
#ifdef PURELUA_CACHE_OBJECT
    lua_set_cache_object(L, LuaClassInfo<T>::get_class_key(), (void *)obj, -1);
#endif
    return 1;
}

}  // namespace PureLua
