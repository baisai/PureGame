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
#include "PureLua/LuaDebugger.h"
#include "PureLua/LuaStateProtect.h"
#include "PureLua/LuaClassMgr.h"
#include "PureLua/LuaUserdataForClass.h"

namespace PureLua {
template <typename T>
class LuaClassInfo {
public:
    template <class BaseType>
    inline static int32_t get_class_offset() {
        return static_cast<int32_t>(((char *)static_cast<BaseType *>((T *)1)) - (char *)1);
    }

    inline static const char *get_class_name() { return sClassName.c_str(); }

    inline static void *get_class_key() { return (void *)(sClassId); }

    template <class BaseType>
    inline static bool register_base_class(lua_State *L) {
        int32_t classOffset = get_class_offset<BaseType>();
        bool success = LuaClassMgr::get_class_mgr(L)->add_base_class(get_class_key(), LuaClassInfo<BaseType>::get_class_key(), classOffset);
        return success;
    }

    inline static bool is_registered(lua_State *L) {
        LuaStateProtect lsp(L);
        lua_pushlightuserdata(L, get_class_key());
        lua_rawget(L, LUA_REGISTRYINDEX);
        if (lua_istable(L, -1)) {
            return true;
        }
        return false;
    }

    inline static int32_t calc_from_offset(lua_State *L, int fromIdx) {
        LuaStateProtect lsp(L);
        fromIdx = lua_absindex(L, fromIdx);
        lua_pushlightuserdata(L, LuaClassMgr::get_class_key());
        lua_gettable(L, fromIdx);
        return LuaClassMgr::get_class_mgr(L)->get_class_offset(lua_touserdata(L, -1), get_class_key());
    }

    inline static int32_t calc_to_offset(lua_State *L, int toIdx) {
        LuaStateProtect lsp(L);
        toIdx = lua_absindex(L, toIdx);
        lua_pushlightuserdata(L, LuaClassMgr::get_class_key());
        lua_gettable(L, toIdx);
        return LuaClassMgr::get_class_mgr(L)->get_class_offset(get_class_key(), lua_touserdata(L, -1));
    }

    static bool get_class_table(lua_State *L) {
        lua_pushlightuserdata(L, get_class_key());
        lua_rawget(L, LUA_REGISTRYINDEX);
        if (lua_istable(L, -1)) {
            return true;
        }
        PureError("Error!!!This  class {} is not register!!!", sClassName);
        return false;
    }

    static bool set_class_table(lua_State *L, int idx) {
        idx = lua_absindex(L, idx);
        LuaStateProtect lsp(L);
        lua_pushvalue(L, idx);
        lua_rawsetp(L, LUA_REGISTRYINDEX, get_class_key());
        lua_pushlightuserdata(L, get_class_key());
        lua_rawsetp(L, idx, LuaClassMgr::get_class_key());
        LuaClassMgr::get_class_mgr(L)->register_class(get_class_key());
        return true;
    }

private:
    static const std::string sClassName;
    static const size_t sClassId;
};

template <typename T>
const std::string LuaClassInfo<T>::sClassName(typeid(T).name());

#ifdef PURE_DEBUG
template <typename T>
const size_t LuaClassInfo<T>::sClassId(debug_class_info(typeid(T)));
#else
template <typename T>
const size_t LuaClassInfo<T>::sClassId(typeid(T).hash_code());
#endif
}  // namespace PureLua
