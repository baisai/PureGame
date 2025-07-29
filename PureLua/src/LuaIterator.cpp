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

#include "PureLua/LuaIterator.h"

namespace PureLua {
//////////////////////////////////////////////////////////
// LuaIterator
/////////////////////////////////////////////////////////
LuaIterator::LuaIterator(lua_State* L, int idx) : mLua(L), mTop(lua_gettop(L)), mOver(false), mKeyIdx(0) {
    lua_pushvalue(L, idx);
    lua_pushnil(L);
    mKeyIdx = lua_absindex(mLua, -1);
    next();
}

LuaIterator::~LuaIterator() { set_over(); }

void LuaIterator::set_over() {
    if (!mOver) {
        mOver = true;
        mKeyIdx = 0;
        lua_settop(mLua, mTop);
    }
}
lua_State* LuaIterator::state() { return mLua; }

bool LuaIterator::is_over() { return mOver; }

void LuaIterator::push_key() { lua_pushvalue(mLua, key_idx()); }

void LuaIterator::push_value() { lua_pushvalue(mLua, value_idx()); }

LuaIterator& LuaIterator::operator++() {
    if (!mOver) {
        next();
    }
    return *this;
}

LuaIterator& LuaIterator::operator++(int) {
    if (!mOver) {
        next();
    }
    return *this;
}

void LuaIterator::next() {
    lua_settop(mLua, mKeyIdx);
    if (!lua_next(mLua, -2)) {
        set_over();
    }
}

//////////////////////////////////////////////////////////
// LuaRefIterator
/////////////////////////////////////////////////////////
LuaRefIterator::LuaRefIterator(const LuaRef& table) : mLua(table.state()), mTable(table), mKey(table.state(), false), mValue(table.state(), false) { next(); }

lua_State* LuaRefIterator::state() { return mLua; }

bool LuaRefIterator::is_over() { return mKey.is_nil(); }

LuaRef& LuaRefIterator::get_key() { return mKey; }

LuaRef& LuaRefIterator::get_value() { return mValue; }

LuaRef& LuaRefIterator::operator*() { return mValue; }

LuaRef& LuaRefIterator::operator->() { return mValue; }

LuaRefIterator& LuaRefIterator::operator++() {
    if (!mKey.is_nil()) {
        next();
    }
    return *this;
}

LuaRefIterator& LuaRefIterator::operator++(int) {
    if (!mKey.is_nil()) {
        next();
    }
    return *this;
}

void LuaRefIterator::next() {
    mTable.push();
    mKey.push();
    if (lua_next(mLua, -2)) {
        mValue.reset_pop_top(mLua);
        mKey.reset_pop_top(mLua);
    } else {
        mKey.set_nil();
        mValue.set_nil();
    }
    lua_pop(mLua, 1);
}

}  // namespace PureLua