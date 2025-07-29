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

#include "PureLua/StdFunctionTraits.h"
#include "PureLua/LuaStackExt.h"

#include <functional>

namespace PureLua {
// const std::function&
template <typename R, typename... Args>
struct LuaStack<const std::function<R(Args...)> &> {
    typedef const std::function<R(Args...)> &PushType;
    typedef std::function<R(Args...)> GetType;
    static inline void push(lua_State *L, PushType t) { StdFunctionTraits<std::function<R(Args...)>>::push_std_func(L, t); }

    static inline GetType get(lua_State *L, int idx) { return StdFunctionTraits<std::function<R(Args...)>>::get_std_func(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return lua_isfunction(L, idx); }
};

// std::function&
template <typename R, typename... Args>
struct LuaStack<std::function<R(Args...)> &> {
    typedef const std::function<R(Args...)> &PushType;
    typedef std::function<R(Args...)> GetType;
    static inline void push(lua_State *L, PushType t) { StdFunctionTraits<std::function<R(Args...)>>::push_std_func(L, t); }

    static inline GetType get(lua_State *L, int idx) { return StdFunctionTraits<std::function<R(Args...)>>::get_std_func(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return lua_isfunction(L, idx); }
};

// const std::function
template <typename R, typename... Args>
struct LuaStack<const std::function<R(Args...)>> {
    typedef const std::function<R(Args...)> &PushType;
    typedef const std::function<R(Args...)> GetType;
    static inline void push(lua_State *L, PushType t) { StdFunctionTraits<std::function<R(Args...)>>::push_std_func(L, t); }

    static inline GetType get(lua_State *L, int idx) { return StdFunctionTraits<std::function<R(Args...)>>::get_std_func(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return lua_isfunction(L, idx); }
};

// std::function
template <typename R, typename... Args>
struct LuaStack<std::function<R(Args...)>> {
    typedef const std::function<R(Args...)> &PushType;
    typedef const std::function<R(Args...)> GetType;
    static inline void push(lua_State *L, PushType t) { StdFunctionTraits<std::function<R(Args...)>>::push_std_func(L, t); }

    static inline GetType get(lua_State *L, int idx) { return StdFunctionTraits<std::function<R(Args...)>>::get_std_func(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return lua_isfunction(L, idx); }
};

}  // namespace PureLua
