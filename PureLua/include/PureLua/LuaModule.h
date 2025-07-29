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
#include "PureLua/LuaRegisterNode.h"

namespace PureLua {
class PURELUA_API LuaModule : public LuaRegisterNode {
public:
    explicit LuaModule(lua_State* L);
    LuaModule(lua_State* L, PureCore::StringRef moduleName);
    LuaModule(lua_State* L, PureCore::StringRef moduleName, const LuaRef& parent);
    virtual ~LuaModule();

    LuaModule& operator[](const LuaRegisterNode& key);

    LuaModule& end();

    LuaModule child(PureCore::StringRef moduleName);

    template <typename T>
    inline LuaModule& def(T obj, PureCore::StringRef name) {
        LuaStateProtect lsp(state());
        push_value();
        LuaDefAutoTool::def(state(), obj, name);
        return *this;
    }

    template <typename T, typename KT>
    inline LuaModule& def_const(const T& obj, const KT& key) {
        LuaStateProtect lsp(state());
        push_value();
        LuaStack<typename LuaStack<KT>::PushType>::push(state(), key);
        LuaStack<typename LuaStack<T>::PushType>::push(state(), std::move(obj));
        lua_rawset(state(), -3);
        return *this;
    }

    template <typename T, typename KT>
    inline LuaModule& def_const(T&& obj, const KT& key) {
        LuaStateProtect lsp(state());
        push_value();
        LuaStack<typename LuaStack<KT>::PushType>::push(state(), key);
        LuaStack<T>::push(state(), std::move(obj));
        lua_rawset(state(), -3);
        return *this;
    }

    template <typename LamadaType>
    inline LuaModule& def_lamada(LamadaType lamada, PureCore::StringRef name) {
        auto func = typename LamadaTraits<LamadaType>::cfunction(lamada);
        return def(func, name);
    }
};

}  // namespace PureLua
