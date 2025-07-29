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

#include "PureLua/LuaRef.h"
#include "PureLua/LuaTableRef.h"

namespace PureLua {
LuaTableRef::LuaTableRef() : mLua(nullptr), mTableRef(LUA_REFNIL), mKeyRef(LUA_REFNIL) {}
LuaTableRef::LuaTableRef(const LuaRef& tab) : mLua(nullptr), mTableRef(LUA_REFNIL), mKeyRef(LUA_REFNIL) {
    mLua = tab.state();
    if (mLua != nullptr) {
        mTableRef = tab.create_ref();
        mKeyRef = luaL_ref(mLua, LUA_REGISTRYINDEX);
    }
}
LuaTableRef::LuaTableRef(const LuaRef& tab, const LuaRef& key) : mLua(nullptr), mTableRef(LUA_REFNIL), mKeyRef(LUA_REFNIL) {
    mLua = tab.state();
    mTableRef = tab.create_ref();
    mKeyRef = key.create_ref();
}
LuaTableRef::LuaTableRef(lua_State* L, int tabIdx, int keyIdx) : mLua(L), mTableRef(LUA_REFNIL), mKeyRef(LUA_REFNIL) {
    if (mLua != nullptr) {
        lua_pushvalue(mLua, tabIdx);
        mTableRef = luaL_ref(mLua, LUA_REGISTRYINDEX);
        lua_pushvalue(mLua, keyIdx);
        mKeyRef = luaL_ref(mLua, LUA_REGISTRYINDEX);
    }
}
LuaTableRef::LuaTableRef(const LuaTableRef& cp) : mLua(cp.mLua), mTableRef(cp.create_tab_ref()), mKeyRef(cp.create_key_ref()) {}

LuaTableRef::LuaTableRef(LuaTableRef&& cp) : mLua(cp.mLua), mTableRef(std::move(cp.mTableRef)), mKeyRef(std::move(cp.mKeyRef)) {}

LuaTableRef::~LuaTableRef() {
    if (mLua) {
        luaL_unref(mLua, LUA_REGISTRYINDEX, mTableRef);
        luaL_unref(mLua, LUA_REGISTRYINDEX, mKeyRef);
        clear();
    }
}

void LuaTableRef::clear() {
    mLua = nullptr;
    mTableRef = LUA_REFNIL;
    mKeyRef = LUA_REFNIL;
}

int LuaTableRef::create_ref() const {
    if (mLua == nullptr || mTableRef == LUA_REFNIL || mKeyRef == LUA_REFNIL) {
        return LUA_REFNIL;
    }
    push();
    return luaL_ref(mLua, LUA_REGISTRYINDEX);
}

LuaTableRef& LuaTableRef::operator=(const LuaTableRef& v) {
    LuaStateProtect lsp(mLua);
    lua_rawgeti(mLua, LUA_REGISTRYINDEX, mTableRef);
    lua_rawgeti(mLua, LUA_REGISTRYINDEX, mKeyRef);
    v.push();
    lua_settable(mLua, -3);
    return *this;
}

lua_State* LuaTableRef::state() const { return mLua; }

bool LuaTableRef::push() const {
    if (mLua == nullptr) {
        return false;
    }
    lua_rawgeti(mLua, LUA_REGISTRYINDEX, mTableRef);
    lua_rawgeti(mLua, LUA_REGISTRYINDEX, mKeyRef);
    lua_gettable(mLua, -2);
    lua_remove(mLua, -2);  // remove the table
    return true;
}

int LuaTableRef::create_tab_ref() const {
    if (mLua == nullptr || mTableRef == LUA_REFNIL) {
        return LUA_REFNIL;
    }
    lua_rawgeti(mLua, LUA_REGISTRYINDEX, mTableRef);
    return luaL_ref(mLua, LUA_REGISTRYINDEX);
}

int LuaTableRef::create_key_ref() const {
    if (mLua == nullptr || mKeyRef == LUA_REFNIL) {
        return LUA_REFNIL;
    }
    lua_rawgeti(mLua, LUA_REGISTRYINDEX, mKeyRef);
    return luaL_ref(mLua, LUA_REGISTRYINDEX);
}

LuaRef LuaTableRef::get_ref() const { return LuaRef(*this); }

LuaRef LuaTableRef::get_tab_ref() const {
    if (mLua == nullptr) {
        return LuaRef();
    }
    LuaStateProtect lsp(mLua);
    lua_rawgeti(mLua, LUA_REGISTRYINDEX, mTableRef);
    return LuaRef(mLua, -1);
}

LuaRef LuaTableRef::get_key_ref() const {
    if (mLua == nullptr) {
        return LuaRef();
    }
    LuaStateProtect lsp(mLua);
    lua_rawgeti(mLua, LUA_REGISTRYINDEX, mKeyRef);
    return LuaRef(mLua, -1);
}

}  // namespace PureLua
