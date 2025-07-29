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

#include "PureLua/LuaModule.h"
#include "PureLua/LuaStack.h"

namespace PureLua {
////////////////////////////////////////////////////////////////////////
// LuaModule
///////////////////////////////////////////////////////////////////////
LuaModule::LuaModule(lua_State* L) : LuaRegisterNode(L) {
    LuaStateProtect lsp(L);
    set_key("_G");
    lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
    set_parent_from(-1);
    lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
    set_value_from(-1);
}
LuaModule::LuaModule(lua_State* L, PureCore::StringRef moduleName) : LuaRegisterNode(L) {
    LuaStateProtect lsp(L);
    set_key(moduleName);
    lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
    set_parent_from(-1);
    LuaRef v = mParent.rawget(mKey);
    if (v.is_table()) {
        set_value(v);
    } else {
        set_value(LuaRef::new_table(L));
    }
}
LuaModule::LuaModule(lua_State* L, PureCore::StringRef moduleName, const LuaRef& parent) : LuaRegisterNode(LuaRef::new_ref(L, moduleName), LuaRef(L), parent) {
    LuaStateProtect lsp(L);
    if (!parent.is_table()) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
        set_parent_from(-1);
    }
    LuaRef v = mParent.rawget(mKey);
    if (v.is_table()) {
        set_value(v);
    } else {
        set_value(LuaRef::new_table(L));
    }
}
LuaModule::~LuaModule() {}

LuaModule& LuaModule::operator[](const LuaRegisterNode& key) {
    register_field(key);
    return end();
}

LuaModule& LuaModule::end() {
    if (mParent.is_table()) {
        mParent.rawset(mKey, mValue);
    }
    return *this;
}

LuaModule LuaModule::child(PureCore::StringRef moduleName) {
    push_value();
    LuaRef parent(mLua, -1);
    lua_pop(mLua, 1);
    return LuaModule(mLua, moduleName, parent);
}

}  // namespace PureLua