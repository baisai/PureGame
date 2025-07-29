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

#include "PureLua/LuaWeakRef.h"
#include "PureLua/LuaHelper.h"
#include "PureLua/LuaModule.h"
#include "PureLua/LuaStack.h"

namespace PureLua {
LuaWeakRef::LuaWeakRef() : mLua(nullptr), mRef(LUA_REFNIL) {}

LuaWeakRef::LuaWeakRef(lua_State *L, bool top) : mLua(L), mRef(LUA_REFNIL) {
    if (top) {
        reset_pop_top(L);
    }
}

LuaWeakRef::LuaWeakRef(lua_State *L, int idx, bool pop) : mLua(L), mRef(LUA_REFNIL) {
    if (L != nullptr) {
        reset(L, idx);
        if (pop) {
            lua_remove(mLua, idx);
        }
    }
}

LuaWeakRef::LuaWeakRef(const LuaTableRef &v) : mLua(v.state()), mRef(LUA_REFNIL) {
    if (v.push()) {
        reset_pop_top(v.state());
    }
}

LuaWeakRef::LuaWeakRef(const LuaRef &v) : mLua(v.state()), mRef(LUA_REFNIL) {
    if (v.push()) {
        reset_pop_top(v.state());
    }
}

LuaWeakRef::LuaWeakRef(const LuaWeakRef &cp) : mLua(cp.mLua), mRef(LUA_REFNIL) {
    if (cp.push()) {
        reset_pop_top(cp.mLua);
    }
}

LuaWeakRef::~LuaWeakRef() {
    if (valid()) {
        if (push_parent()) {
            unref(-1, mRef);
            lua_pop(mLua, 1);
        }
        clear();
    }
}

void LuaWeakRef::clear() {
    mLua = nullptr;
    mRef = LUA_REFNIL;
}

LuaWeakRef &LuaWeakRef::operator=(const LuaRef &right) {
    if (right.push()) {
        reset_pop_top(right.state());
    }
    return *this;
}

LuaWeakRef &LuaWeakRef::operator=(const LuaTableRef &right) {
    if (right.push()) {
        reset_pop_top(right.state());
    }
    return *this;
}

LuaWeakRef &LuaWeakRef::operator=(const LuaWeakRef &right) {
    if (right.push()) {
        reset_pop_top(right.state());
    }
    return *this;
}

lua_State *LuaWeakRef::state() const { return mLua; }

bool LuaWeakRef::push() const {
    if (!push_parent()) {
        PureError("Error!!! push parent failed!!!");
        return false;
    }
    lua_rawgeti(mLua, -1, mRef);
    lua_remove(mLua, -2);
    return true;
}

bool LuaWeakRef::valid() const { return !(mLua == nullptr || mRef == LUA_REFNIL || mRef == LUA_NOREF); }

void LuaWeakRef::reset() { set_nil(); }

void LuaWeakRef::reset(lua_State *L) {
    set_nil();
    mLua = L;
}

void LuaWeakRef::reset(lua_State *L, int idx) {
    set_nil();
    if (L != nullptr) {
        mLua = L;
        idx = lua_absindex(mLua, idx);
        if (push_parent()) {
            lua_pushvalue(mLua, idx);
            mRef = ref(-2);
            lua_pop(mLua, 1);
        }
    }
}

void LuaWeakRef::reset_pop_top(lua_State *L) {
    set_nil();
    if (L != nullptr) {
        mLua = L;
        if (push_parent()) {
            lua_pushvalue(mLua, -2);
            mRef = ref(-2);
            lua_pop(mLua, 1);
        }
        lua_pop(mLua, 1);
    }
}

void LuaWeakRef::set_nil() {
    if (valid()) {
        if (push_parent()) {
            unref(-1, mRef);
            lua_pop(mLua, 1);
        }
        mRef = LUA_REFNIL;
    }
}

LuaRef LuaWeakRef::get_ref() const {
    if (push()) {
        LuaStateProtect lsp(mLua);
        return LuaRef(mLua, -1);
    }
    return LuaRef();
}

bool LuaWeakRef::push_parent() const {
    if (mLua) {
        lua_pushlightuserdata(mLua, (void *)&sWeakKey);
        lua_rawget(mLua, LUA_REGISTRYINDEX);
        if (!lua_istable(mLua, -1)) {
            lua_pop(mLua, 1);
            lua_createtable(mLua, 0, 128);
            lua_pushstring(mLua, "__mode");
            lua_pushstring(mLua, "v");
            lua_rawset(mLua, -3);
            lua_pushvalue(mLua, -1);
            lua_setmetatable(mLua, -2);
            lua_pushlightuserdata(mLua, (void *)&sWeakKey);
            lua_pushvalue(mLua, -2);
            lua_rawset(mLua, LUA_REGISTRYINDEX);
        }
        return true;
    }
    return false;
}

int LuaWeakRef::ref(int t) const {
    int ref = LUA_NOREF;
    if (mLua) {
        {
            LuaStateProtect lsp(mLua);
            lua_pushlightuserdata(mLua, (void *)&sRefKey);
            lua_rawget(mLua, LUA_REGISTRYINDEX);
            if (!lua_istable(mLua, -1)) {
                lua_pop(mLua, 1);
                lua_createtable(mLua, 0, 128);
                lua_pushlightuserdata(mLua, (void *)&sRefKey);
                lua_pushvalue(mLua, -2);
                lua_rawset(mLua, LUA_REGISTRYINDEX);
            }
            lua_pushboolean(mLua, true);
            ref = luaL_ref(mLua, -2);
        }
        if (ref != LUA_NOREF && ref != LUA_REFNIL) lua_rawseti(mLua, t, ref);
    }
    return ref;
}

void LuaWeakRef::unref(int t, int ref) const {
    if (mLua && ref != LUA_NOREF && ref != LUA_REFNIL) {
        t = lua_absindex(mLua, t);
        LuaStateProtect lsp(mLua);
        lua_pushlightuserdata(mLua, (void *)&sRefKey);
        lua_rawget(mLua, LUA_REGISTRYINDEX);
        if (lua_istable(mLua, -1)) {
            luaL_unref(mLua, -1, ref);
        }
        lua_pushnil(mLua);
        lua_rawseti(mLua, t, ref);
    }
}

const char LuaWeakRef::sWeakKey = 0;
const char LuaWeakRef::sRefKey = 0;
}  // namespace PureLua
