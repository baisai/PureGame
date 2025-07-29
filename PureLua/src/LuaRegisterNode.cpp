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

#include "PureLua/LuaRegisterNode.h"

namespace PureLua {
////////////////////////////////////////////////////////////////////////
// LuaRegisterNode
///////////////////////////////////////////////////////////////////////
LuaRegisterNode::LuaRegisterNode(lua_State *L) : mLua(L), mKey(L, false), mValue(L, false), mParent(L, false), mNext(nullptr) {}
LuaRegisterNode::LuaRegisterNode(const LuaRef &key, const LuaRef &value, const LuaRef &parent)
    : mLua(key.state()), mKey(key), mValue(value), mParent(parent), mNext(nullptr) {}
LuaRegisterNode::LuaRegisterNode(const LuaRegisterNode &cp) : mLua(cp.mLua), mKey(cp.mKey), mValue(cp.mValue), mParent(cp.mParent), mNext(nullptr) {}
LuaRegisterNode::~LuaRegisterNode() {}

lua_State *LuaRegisterNode::state() const { return mLua; }

void LuaRegisterNode::set_key_from(int idx) {
    lua_pushvalue(mLua, idx);
    mKey.reset_pop_top(mLua);
}

void LuaRegisterNode::set_value_from(int idx) {
    lua_pushvalue(mLua, idx);
    mValue.reset_pop_top(mLua);
}

void LuaRegisterNode::set_parent_from(int idx) {
    lua_pushvalue(mLua, idx);
    mParent.reset_pop_top(mLua);
}

void LuaRegisterNode::push_parent() const { mParent.push(); }

void LuaRegisterNode::push() const {
    mKey.push();
    mValue.push();
}

void LuaRegisterNode::push_key() const { mKey.push(); }

void LuaRegisterNode::push_value() const { mValue.push(); }

LuaRegisterNode &LuaRegisterNode::operator+(const LuaRegisterNode &right) {
    LuaRegisterNode &noneConst = const_cast<LuaRegisterNode &>(right);
    noneConst.mNext = this;
    return noneConst;
}

LuaRegisterNode &LuaRegisterNode::operator=(const LuaRegisterNode &right) {
    mLua = right.mLua;
    mKey = right.mKey;
    mValue = right.mValue;
    mNext = nullptr;
    return *this;
}

void LuaRegisterNode::register_field(const LuaRegisterNode &head) {
    lua_State *lua = mLua;
    LuaStateProtect lsp(lua);
    const LuaRegisterNode *p = &head;
    push_value();
    while (p) {
        p->push();
        lua_rawset(lua, -3);
        p = p->next();
    }
}

void LuaRegisterNode::next(LuaRegisterNode *n) { mNext = n; }
const LuaRegisterNode *LuaRegisterNode::next() const { return mNext; }
}  // namespace PureLua