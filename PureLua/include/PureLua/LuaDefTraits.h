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

#include "PureLua/LuaDefTool.h"

#include <type_traits>

namespace PureLua {
/////////////////////////////////////////////////////////////////////////////////
// LuaDefFuncHelper
/////////////////////////////////////////////////////////////////////////////////
template <typename T, bool IsLamada>
class LuaDefFuncHelper;

template <typename T>
class LuaDefFuncHelper<T, true> {
private:
    template <typename RT, bool>
    friend class LuaDefPropHelper;
    template <typename RegisterType>
    static inline void def(RegisterType* reg, T arg, const PureCore::StringRef name) {
        reg->def_lamada(arg, name);
    }

    static inline void def(lua_State* L, T arg, PureCore::StringRef name) { LuaDefTool::def_lamada(L, arg, name); }
};

template <typename T>
class LuaDefFuncHelper<T, false> {
private:
    template <typename RT, bool>
    friend class LuaDefPropHelper;
    template <typename RegisterType>
    static inline void def(RegisterType* reg, T arg, PureCore::StringRef name) {
        reg->def_class_func(arg, name);
    }

    static inline void def(lua_State* L, T arg, PureCore::StringRef name) { LuaDefTool::def_func(L, arg, name); }
};
/////////////////////////////////////////////////////////////////////////////////
// LuaDefPropHelper
/////////////////////////////////////////////////////////////////////////////////
template <typename T, bool IsProp>
class LuaDefPropHelper;

template <typename T>
class LuaDefPropHelper<T, true> {
private:
    template <typename RT>
    friend struct LuaDefTraits;
    template <typename RegisterType>
    static inline void def(RegisterType* reg, T arg, PureCore::StringRef name) {
        reg->def_property(arg, name);
    }

    static inline void def(lua_State* L, T arg, PureCore::StringRef name) { LuaDefTool::def_property(L, arg, name); }
};

template <typename T>
class LuaDefPropHelper<T, false> {
private:
    template <typename RT>
    friend struct LuaDefTraits;
    template <typename RegisterType>
    static inline void def(RegisterType* reg, T arg, PureCore::StringRef name) {
        LuaDefFuncHelper<T, !std::is_function<typename std::remove_pointer<T>::type>::value && !std::is_member_function_pointer<T>::value>::def(reg, arg, name);
    }

    static inline void def(lua_State* L, T arg, PureCore::StringRef name) {
        LuaDefFuncHelper<T, !std::is_function<typename std::remove_pointer<T>::type>::value>::def(L, arg, name);
    }
};

/////////////////////////////////////////////////////////////////////////////////
// LuaDefTraits
/////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct LuaDefTraits {
    template <typename RegisterType>
    static inline void def(RegisterType* reg, T arg, PureCore::StringRef name) {
        LuaDefPropHelper<T, std::is_member_object_pointer<T>::value ||
                                (std::is_pointer<T>::value && !std::is_function<typename std::remove_pointer<T>::type>::value)>::def(reg, arg, name);
    }

    static inline void def(lua_State* L, T arg, PureCore::StringRef name) {
        LuaDefPropHelper<T, std::is_pointer<T>::value && !std::is_function<typename std::remove_pointer<T>::type>::value>::def(L, arg, name);
    }
};

}  // namespace PureLua
