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
#include "PureLua/LuaStackExt.h"

namespace PureLua {
////////////////////////////////////////////////////////////////////////////////////////////////////////
// LuaFunctionTraits
////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename FunctionType>
class LuaFunctionTraits;

template <typename... Args>
class LuaFunctionTraits<void (*)(Args...)> {
public:
    typedef void (*FunctionType)(Args...);
    static inline int call(lua_State *L) {
        FunctionType func = *((FunctionType *)lua_touserdata(L, lua_upvalueindex(1)));
        return call_impl(L, func, std::index_sequence_for<Args...>{});
    }

private:
    template <std::size_t... Index>
    static inline int call_impl(lua_State *L, FunctionType func, std::index_sequence<Index...>) {
        func(LuaStack<Args>::get(L, Index + 1)...);
        return 0;
    }
};

template <typename R, typename... Args>
class LuaFunctionTraits<R (*)(Args...)> {
public:
    typedef R (*FunctionType)(Args...);
    static inline int call(lua_State *L) {
        FunctionType func = *((FunctionType *)lua_touserdata(L, lua_upvalueindex(1)));
        return call_impl(L, func, std::index_sequence_for<Args...>{});
    }

private:
    template <std::size_t... Index>
    static inline int call_impl(lua_State *L, FunctionType func, std::index_sequence<Index...>) {
        LuaStack<R>::push(L, std::move(func(LuaStack<Args>::get(L, Index + 1)...)));
        return 1;
    }
};

}  // namespace PureLua