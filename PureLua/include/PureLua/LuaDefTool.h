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

#include "PureLua/LuaLamadaTraits.h"
#include "PureLua/PureLuaLog.h"
#include "PureLua/LuaStateProtect.h"
#include "PureLua/LuaFunctionTraits.h"
#include "PureLua/LuaMoveReturnTraits.h"
#include "PureLua/LuaClassPropertyTraits.h"
#include "PureLua/LuaMetatable.h"
#include "PureLua/LuaHelper.h"

namespace PureLua {
struct PURELUA_API LuaDefTool {
    static void def_func(lua_State* L, lua_CFunction obj, PureCore::StringRef name) {
        LuaStateProtect lsp(L);
        if (!lua_istable(L, -1)) {
            // lua_getglobal(L, "_G");
            lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
        } else {
            lua_pushvalue(L, -1);
        }
        lua_pushlstring(L, name.data(), name.size());
        lua_pushcclosure(L, obj, 0);
        lua_rawset(L, -3);
    }

    template <typename T>
    static void def_func(lua_State* L, T obj, PureCore::StringRef name) {
        LuaStateProtect lsp(L);
        if (!lua_istable(L, -1)) {
            // lua_getglobal(L, "_G");
            lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
        } else {
            lua_pushvalue(L, -1);
        }
        new (lua_newuserdatauv(L, sizeof(T), 0)) T(obj);
        lua_pushcclosure(L, LuaFunctionTraits<T>::call, 1);
        raw_setfieldlen(L, -2, name.data(), name.size());
    }

    template <typename T>
    static void def_lamada(lua_State* L, T obj, PureCore::StringRef name) {
        auto func = typename LamadaTraits<T>::cfunction(obj);
        return def_func(L, func, name);
    }

    template <typename T>
    static void def_const(lua_State* L, T obj, PureCore::StringRef name) {
        LuaStateProtect lsp(L);
        if (!lua_istable(L, -1)) {
            // lua_getglobal(L, "_G");
            lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
        } else {
            lua_pushvalue(L, -1);
        }
        lua_pushlstring(L, name.data(), name.size());
        LuaStack<T>::push(L, obj);
        lua_rawset(L, -3);
    }

    template <typename T>
    static void def_const(lua_State* L, T&& obj, PureCore::StringRef name) {
        LuaStateProtect lsp(L);
        if (!lua_istable(L, -1)) {
            // lua_getglobal(L, "_G");
            lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
        } else {
            lua_pushvalue(L, -1);
        }
        lua_pushlstring(L, name.data(), name.size());
        LuaStack<T>::push(L, std::move(obj));
        lua_rawset(L, -3);
    }

    template <typename T>
    static void def_move_return(lua_State* L, T obj, PureCore::StringRef name) {
        LuaStateProtect lsp(L);
        if (!lua_istable(L, -1)) {
            // lua_getglobal(L, "_G");
            lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
        } else {
            lua_pushvalue(L, -1);
        }
        new (lua_newuserdatauv(L, sizeof(T), 0)) T(obj);
        lua_pushcclosure(L, LuaMoveReturnTraits<T>::call, 1);
        raw_setfieldlen(L, -2, name.data(), name.size());
    }

    template <typename T, size_t size>
    static void def_array(lua_State* L, T obj[size], PureCore::StringRef name) {
        LuaStateProtect lsp(L);
        if (!lua_istable(L, -1)) {
            // lua_getglobal(L, "_G");
            lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
        } else {
            lua_pushvalue(L, -1);
        }
        new (lua_newuserdatauv(L, sizeof(LuaClassPropertyInfo), 0))
            LuaClassPropertyInfo(name, LuaClassPropertyArrayTraits<T, size, void>::pt_index_function, obj);
        LuaMetatable::push_property_function(L, -1, true);
        raw_setfieldlen(L, -3, name.data(), name.size());
    }

    template <typename T>
    static void def_property(lua_State* L, const T& obj, PureCore::StringRef name) {
        def_property(L, &obj, name);
    }

    template <typename T>
    static void def_property(lua_State* L, T& obj, PureCore::StringRef name) {
        def_property(L, &obj, name);
    }

    template <typename T>
    static void def_property(lua_State* L, const T* obj, PureCore::StringRef name) {
        LuaStateProtect lsp(L);
        if (!lua_istable(L, -1)) {
            // lua_getglobal(L, "_G");
            lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
        } else {
            lua_pushvalue(L, -1);
        }
        new (lua_newuserdatauv(L, sizeof(LuaClassPropertyInfo), 0)) LuaClassPropertyInfo(name, LuaClassPropertyTraits<T, void>::pt_index_function, obj);
        LuaMetatable::push_property_function(L, -1, true);
        raw_setfieldlen(L, -3, name.data(), name.size());
    }

    template <typename T>
    static void def_property(lua_State* L, T* obj, PureCore::StringRef name) {
        LuaStateProtect lsp(L);
        if (!lua_istable(L, -1)) {
            // lua_getglobal(L, "_G");
            lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
        } else {
            lua_pushvalue(L, -1);
        }
        new (lua_newuserdatauv(L, sizeof(LuaClassPropertyInfo), 0))
            LuaClassPropertyInfo(name, LuaClassPropertyTraits<T, void>::pt_index_function, LuaClassPropertyTraits<T, void>::pt_newindex_function, obj);
        LuaMetatable::push_property_function(L, -1, true);
        raw_setfieldlen(L, -3, name.data(), name.size());
    }
};

}  // namespace PureLua
