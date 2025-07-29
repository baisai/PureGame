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
#include "PureLua/LuaStateProtect.h"
#include "PureLua/LuaDefTraits.h"

namespace PureLua {
struct PURELUA_API LuaDefAutoTool {
    template <typename T>
    inline static void def(lua_State* L, T obj, PureCore::StringRef name) {
        LuaDefTraits<T>::def(L, obj, name);
    }

    template <typename T, size_t size>
    inline static void def(lua_State* L, T (&obj)[size], PureCore::StringRef name) {
        LuaDefTool::def_array(L, obj, name);
    }

    template <typename T>
    inline static void def_move_return(lua_State* L, T obj, PureCore::StringRef name) {
        LuaDefTool::def_move_return(L, obj, name);
    }
};

}  // namespace PureLua
