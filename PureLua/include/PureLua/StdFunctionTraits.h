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
#include "PureLua/LuaUserdataGcTraits.h"
#include "PureLua/LuaRef.h"

namespace PureLua {
////////////////////////////////////////////////////////////////////////////////////////////////////////
// StdFunctionTraits
////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename FunctionType>
class StdFunctionTraits;

template <typename... Args>
class StdFunctionTraits<std::function<void(Args...)>> {
public:
    typedef std::function<void(Args...)> FunctionType;
    static inline void push_std_func(lua_State* L, const FunctionType& func) {
        void* userdataPtr = lua_newuserdatauv(L, sizeof(FunctionType), 0);
        new (userdataPtr) FunctionType(func);
        lua_createtable(L, 0, 2);
        lua_pushstring(L, "__gc");
        lua_pushcclosure(L, lua_userdata_gc<FunctionType>, 0);
        lua_rawset(L, -3);
        lua_pushstring(L, "__call");
        lua_pushvalue(L, -3);
        lua_pushcclosure(L, call, 1);
        lua_rawset(L, -3);
        lua_setmetatable(L, -2);
    }

    static inline FunctionType get_std_func(lua_State* L, int idx) {
        LuaRef ref = LuaStack<LuaRef>::get(L, idx);
        if (!ref.is_function()) {
            PureLuaErrorJump(L, "this obj is not a function");
            return nullptr;
        }
        return FunctionType([ref](Args&&... args) -> void { ref(std::forward<Args>(args)...); });
    }

private:
    static inline int call(lua_State* L) {
        FunctionType* func = (FunctionType*)lua_touserdata(L, lua_upvalueindex(1));
        if (func == nullptr || !(*func)) {
            PureLuaErrorJump(L, "this obj is not a valid function");
            return 0;
        }
        return call_impl(L, func, std::index_sequence_for<Args...>{});
    }

    template <std::size_t... Index>
    static inline int call_impl(lua_State* L, FunctionType* func, std::index_sequence<Index...>) {
        (*func)(LuaStack<Args>::get(L, Index + 2)...);
        return 0;
    }
};

template <typename R, typename... Args>
class StdFunctionTraits<std::function<R(Args...)>> {
public:
    typedef std::function<R(Args...)> FunctionType;
    static inline void push_std_func(lua_State* L, const FunctionType& func) {
        void* userdataPtr = lua_newuserdatauv(L, sizeof(FunctionType), 0);
        new (userdataPtr) FunctionType(func);
        lua_createtable(L, 0, 2);
        lua_pushstring(L, "__gc");
        lua_pushcclosure(L, lua_userdata_gc<FunctionType>, 0);
        lua_rawset(L, -3);
        lua_pushstring(L, "__call");
        lua_pushvalue(L, -3);
        lua_pushcclosure(L, call, 1);
        lua_rawset(L, -3);
        lua_setmetatable(L, -2);
    }

    static inline FunctionType get_std_func(lua_State* L, int idx) {
        LuaRef ref = LuaStack<LuaRef>::get(L, idx);
        if (!ref.is_function()) {
            PureLuaErrorJump(L, "this obj is not a function");
            return nullptr;
        }
        return FunctionType([ref](Args&&... args) -> R {
            auto r = ref(std::forward<Args>(args)...);
            return r.template cast<R>();
        });
    }

private:
    static inline int call(lua_State* L) {
        FunctionType* func = (FunctionType*)lua_touserdata(L, lua_upvalueindex(1));
        if (func == nullptr || !(*func)) {
            PureLuaErrorJump(L, "this obj is not a valid function");
            return 0;
        }
        return call_impl(L, func, std::index_sequence_for<Args...>{});
    }

    template <std::size_t... Index>
    static inline int call_impl(lua_State* L, FunctionType* func, std::index_sequence<Index...>) {
        LuaStack<R>::push(L, std::move((*func)(LuaStack<Args>::get(L, Index + 2)...)));
        return 1;
    }
};

}  // namespace PureLua