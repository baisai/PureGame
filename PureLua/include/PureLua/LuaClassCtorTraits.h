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
#include "PureLua/LuaStack.h"
#include "PureLua/LuaObjectCache.h"
#include "PureLua/LuaClassCtorHelper.h"

namespace PureLua {
////////////////////////////////////////////////////////////////////////////////////////////////////////
// LuaClassCtorTraits and LuaClassCreateTraits
////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename ClassType, bool luaGC, typename FunctionType>
class LuaClassDefaultCtorTraits;

template <typename ClassType, bool luaGC, typename FunctionType>
class LuaClassCtorTraits;

template <typename ClassType, bool luaGC, typename FunctionType>
class LuaClassDefaultCreatorTraits;

template <typename ClassType, bool luaGC, typename FunctionType>
class LuaClassCreatorTraits;

template <typename ClassType, bool luaGC, typename... Args>
class LuaClassDefaultCtorTraits<ClassType, luaGC, std::function<void(Args...)>> {
public:
    static int call(lua_State* L) {
        if (!lua_istable(L, 1)) {
            PureLuaErrorJump(L, "Error!!!The NewClass Traits First arg is not object table!!!");
            return 0;
        }
        return lua_ctor_object_userdata(L, 1, ctor(L, std::index_sequence_for<Args...>{}), 0, luaGC);
    }

private:
    template <std::size_t... Index>
    static ClassType* ctor(lua_State* L, std::index_sequence<Index...>) {
        return new ClassType(LuaStack<Args>::get(L, Index + 2)...);
    }
};

template <typename ClassType, bool luaGC>
class LuaClassCtorTraits<ClassType, luaGC, std::function<ClassType*(lua_State*)>> {
public:
    static int call(lua_State* L) {
        if (!lua_istable(L, 1)) {
            PureLuaErrorJump(L, "Error!!!The NewClass Traits First arg is not object table!!!");
            return 0;
        }
        LuaRef cls(L, 1);
        auto obj = ctor(L);
        cls.push();
        return lua_ctor_object_userdata(L, -1, obj, 0, luaGC);
    }

private:
    static ClassType* ctor(lua_State* L) {
        auto newer = lua_class_get_newer<std::function<ClassType*(lua_State*)>>(L, 1);
        if (!newer || !*newer) {
            PureLuaErrorJump(L, "Error!!!not found the class newer!!!");
            return nullptr;
        }
        lua_remove(L, 1);
        return (*newer)(L);
    }
};

template <typename ClassType, bool luaGC, typename... Args>
class LuaClassCtorTraits<ClassType, luaGC, std::function<ClassType*(Args...)>> {
public:
    static int call(lua_State* L) {
        if (!lua_istable(L, 1)) {
            PureLuaErrorJump(L, "Error!!!The NewClass Traits First arg is not object table!!!");
            return 0;
        }
        return lua_ctor_object_userdata(L, 1, ctor(L, std::index_sequence_for<Args...>{}), 0, luaGC);
    }

private:
    template <std::size_t... Index>
    static ClassType* ctor(lua_State* L, std::index_sequence<Index...>) {
        auto newer = lua_class_get_newer<std::function<ClassType*(Args...)>>(L, 1);
        if (!newer || !*newer) {
            PureLuaErrorJump(L, "Error!!!not found the class newer!!!");
            return nullptr;
        }
        return (*newer)(LuaStack<Args>::get(L, Index + 2)...);
    }
};

template <typename ClassType, bool luaGC, typename... Args>
class LuaClassDefaultCreatorTraits<ClassType, luaGC, std::function<void(Args...)>> {
public:
    static int call(lua_State* L) {
        if (!lua_istable(L, 1)) {
            PureError("Error!!!The NewClass Traits First arg is not class table!!!");
            return 0;
        }
        return lua_new_object_userdata(L, 1, ctor(L, std::index_sequence_for<Args...>{}), 0, luaGC);
    }

private:
    template <std::size_t... Index>
    static ClassType* ctor(lua_State* L, std::index_sequence<Index...>) {
        return new ClassType(LuaStack<Args>::get(L, Index + 2)...);
    }
};

template <typename ClassType, bool luaGC>
class LuaClassCreatorTraits<ClassType, luaGC, std::function<ClassType*(lua_State*)>> {
public:
    static int call(lua_State* L) {
        if (!lua_istable(L, 1)) {
            PureError("Error!!!The NewClass Traits First arg is not class table!!!");
            return 0;
        }
        LuaRef cls(L, 1);
        auto obj = ctor(L);
        cls.push();
        return lua_new_object_userdata(L, -1, obj, 0, luaGC);
    }

private:
    static ClassType* ctor(lua_State* L) {
        auto newer = lua_class_get_newer<std::function<ClassType*(lua_State*)>>(L, 1);
        if (!newer || !*newer) {
            PureLuaErrorJump(L, "Error!!!not found the class newer!!!");
            return nullptr;
        }
        lua_remove(L, 1);
        return (*newer)(L);
    }
};

template <typename ClassType, bool luaGC, typename... Args>
class LuaClassCreatorTraits<ClassType, luaGC, std::function<ClassType*(Args...)>> {
public:
    static int call(lua_State* L) {
        if (!lua_istable(L, 1)) {
            PureError("Error!!!The NewClass Traits First arg is not class table!!!");
            return 0;
        }
        return lua_new_object_userdata(L, 1, ctor(L, std::index_sequence_for<Args...>{}), 0, luaGC);
    }

private:
    template <std::size_t... Index>
    static ClassType* ctor(lua_State* L, std::index_sequence<Index...>) {
        auto newer = lua_class_get_newer<std::function<ClassType*(Args...)>>(L, 1);
        if (!newer || !*newer) {
            PureLuaErrorJump(L, "Error!!!not found the class newer!!!");
            return nullptr;
        }
        return (*newer)(LuaStack<Args>::get(L, Index + 2)...);
    }
};

}  // namespace PureLua