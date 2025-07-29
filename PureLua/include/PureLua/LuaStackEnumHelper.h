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

#include "PureLua/LuaUserdata.h"

namespace PureLua {
template <typename T, bool IsEnum>
struct LuaStackEnumHelper;

template <typename T>
struct LuaStackEnumHelper<const T &, true> {
    typedef const T &PushType;
    typedef T GetType;
    static inline void push(lua_State *L, PushType t) { lua_pushinteger(L, lua_Integer(t)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};

template <typename T>
struct LuaStackEnumHelper<T &, true> {
    typedef const T &PushType;
    typedef T GetType;
    static inline void push(lua_State *L, PushType t) { lua_pushinteger(L, lua_Integer(t)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};

template <typename T>
struct LuaStackEnumHelper<const T, true> {
    typedef const T &PushType;
    typedef T GetType;
    static inline void push(lua_State *L, PushType t) { lua_pushinteger(L, lua_Integer(t)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};

template <typename T>
struct LuaStackEnumHelper<T, true> {
    typedef const T &PushType;
    typedef T GetType;
    static inline void push(lua_State *L, PushType t) { lua_pushinteger(L, lua_Integer(t)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};

template <typename T>
struct LuaStackEnumHelper<const T &, false> {
    typedef const T &PushType;
    typedef T &GetType;
    static inline void push(lua_State *L, PushType t) { LuaUserdataPtr::push<T>(L, &t); }

    static inline GetType get(lua_State *L, int idx) { return *LuaUserdataPtr::get<T>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

template <typename T>
struct LuaStackEnumHelper<T &, false> {
    typedef const T &PushType;
    typedef T &GetType;
    static inline void push(lua_State *L, PushType t) { LuaUserdataPtr::push<T>(L, &t); }

    static inline GetType get(lua_State *L, int idx) { return *LuaUserdataPtr::get<T>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// Lua stack conversions for const class objects passed by value.
template <typename T>
struct LuaStackEnumHelper<const T, false> {
    typedef const T &PushType;
    typedef const T &GetType;
    static inline void push(lua_State *L, PushType t) { LuaUserdata::push<T>(L, t); }

    static inline void push(lua_State *L, T &&t) { LuaUserdata::push<T>(L, std::move(t)); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdata::get<T>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// Lua stack conversions for class objects passed by value.
template <typename T>
struct LuaStackEnumHelper<T, false> {
    typedef const T &PushType;
    typedef const T &GetType;
    static inline void push(lua_State *L, PushType t) { LuaUserdata::push<T>(L, t); }

    static inline void push(lua_State *L, T &&t) { LuaUserdata::push<T>(L, std::move(t)); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdata::get<T>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

}  // namespace PureLua
