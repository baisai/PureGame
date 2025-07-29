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

#include "PureLua/LuaRegisterEnum.h"

namespace PureLua {

template <typename EnumType>
LuaRegisterEnum<EnumType>::LuaRegisterEnum(lua_State *L, PureCore::StringRef name) : LuaRegisterNode(L) {
    LuaStateProtect lsp(L);
    set_key(name);
    lua_createtable(L, 0, 0);
    set_value_from(-1);
}

template <typename EnumType>
LuaRegisterEnum<EnumType>::LuaRegisterEnum(const LuaRegisterEnum<EnumType> &cp) : LuaRegisterNode(cp) {}

template <typename EnumType>
LuaRegisterEnum<EnumType> &LuaRegisterEnum<EnumType>::operator=(const LuaRegisterEnum<EnumType> &right) {
    LuaRegisterNode::operator=(static_cast<const LuaRegisterNode &>(right));
    return *this;
}

template <typename EnumType>
LuaRegisterEnum<EnumType> &LuaRegisterEnum<EnumType>::def(EnumType et, PureCore::StringRef name) {
    LuaStateProtect lsp(state());
    push_value();
    LuaStack<typename LuaStack<PureCore::StringRef>::PushType>::push(state(), name);
    lua_pushinteger(mLua, static_cast<lua_Integer>(et));
    lua_rawset(state(), -3);
    return *this;
}
}  // namespace PureLua
