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
#include "PureLua/LuaHelper.h"
#include "PureLua/LuaModule.h"
#include "PureLua/LuaStack.h"

namespace PureLua {
LuaRef::LuaRef() : mLua(nullptr), mRef(LUA_REFNIL) {}

LuaRef::LuaRef(lua_State* L) : mLua(L), mRef(LUA_REFNIL) {}

LuaRef::LuaRef(lua_State* L, int idx) : mLua(nullptr), mRef(LUA_REFNIL) { reset(L, idx); }

LuaRef::LuaRef(const LuaTableRef& v) : mLua(v.state()), mRef(v.create_ref()) {}

LuaRef::LuaRef(const LuaRef& cp) : mLua(cp.mLua), mRef(cp.create_ref()) {}

LuaRef::LuaRef(LuaRef&& cp) : mLua(cp.mLua), mRef(cp.mRef) { cp.mRef = LUA_REFNIL; }

LuaRef::~LuaRef() { set_nil(); }

void LuaRef::clear() {
    mLua = nullptr;
    mRef = LUA_REFNIL;
}

void LuaRef::reset() { set_nil(); }

void LuaRef::reset(lua_State* L) {
    set_nil();
    mLua = L;
}

void LuaRef::reset(lua_State* L, int idx) {
    set_nil();
    if (L != nullptr) {
        mLua = L;
        lua_pushvalue(L, idx);
        mRef = luaL_ref(L, LUA_REGISTRYINDEX);
    }
}

void LuaRef::reset_pop_top(lua_State* L) {
    set_nil();
    if (L != nullptr) {
        mLua = L;
        mRef = luaL_ref(mLua, LUA_REGISTRYINDEX);
    }
}

void LuaRef::set_nil() {
    if (valid()) {
        luaL_unref(mLua, LUA_REGISTRYINDEX, mRef);
        mRef = LUA_REFNIL;
    }
}

int LuaRef::create_ref() const {
    if (!valid()) {
        return LUA_REFNIL;
    }
    push();
    return luaL_ref(mLua, LUA_REGISTRYINDEX);
}

LuaRef& LuaRef::operator=(const LuaRef& right) {
    if (right.push()) {
        reset_pop_top(right.state());
    }
    return *this;
}

LuaRef& LuaRef::operator=(LuaRef&& right) {
    set_nil();
    mLua = right.mLua;
    mRef = right.mRef;
    right.mRef = LUA_REFNIL;
    return *this;
}

LuaRef& LuaRef::operator=(const LuaTableRef& right) {
    if (right.push()) {
        reset_pop_top(right.state());
    }
    return *this;
}

LuaRef LuaRef::new_table(lua_State* L, int narr, int nrec) {
    if (L != nullptr) {
        LuaStateProtect lsp(L);
        lua_createtable(L, narr, nrec);
        return LuaRef(L, -1);
    }
    return LuaRef();
}
LuaRef LuaRef::new_index_check_table(lua_State* L, PureCore::StringRef tag, int narr, int nrec) {
    if (L != nullptr) {
        LuaStateProtect lsp(L);
        lua_createtable(L, narr, nrec);
        lua_createtable(L, 0, 1);
        lua_pushstring(L, "__index");
        LuaStack<PureCore::StringRef>::push(L, tag);
        lua_pushcclosure(L, table_index_check, 1);
        lua_rawset(L, -3);
        lua_setmetatable(L, -2);
        return LuaRef(L, -1);
    }
    return LuaRef();
}

LuaRef LuaRef::new_global_index_check_table(lua_State* L, PureCore::StringRef name, int narr, int nrec) {
    LuaRef newTab = new_index_check_table(L, name);
    set_global_var(L, name, newTab);
    return newTab;
}

std::string LuaRef::to_string() const {
    if (!valid()) {
        return std::string("nil");
    }
    LuaStateProtect lsp(mLua);
    int err = push_lua_error_catcher(mLua);
    lua_getglobal(mLua, "tostring");
    push();
    if (lua_pcall(mLua, 1, 1, err)) {
        const char* errStr = lua_tostring(mLua, -1);
        if (errStr) PureError(errStr);
        return std::string();
    }
    const char* str = lua_tostring(mLua, -1);
    return std::string(str);
}
void LuaRef::print() const {
    switch (type()) {
        case LUA_TNIL:
            PureInfo("nil");
            break;
        case LUA_TNUMBER:
            PureInfo(to_string().c_str());
            break;
        case LUA_TBOOLEAN:
            PureInfo(cast<bool>() ? "true" : "false");
            break;
        case LUA_TSTRING:
            PureInfo("\"{}\"", cast<std::string>());
            break;
        case LUA_TTABLE:
            PureInfo("table : {}", to_string());
            break;
        case LUA_TFUNCTION:
            PureInfo("function : {}", to_string());
            break;
        case LUA_TUSERDATA:
            PureInfo("userdata : {}", to_string());
            break;
        case LUA_TTHREAD:
            PureInfo("thread : {}", to_string());
            break;
        case LUA_TLIGHTUSERDATA:
            PureInfo("lightuserdata : {}", to_string());
            break;
        default:
            PureInfo("unknown");
            break;
    }
}

lua_State* LuaRef::state() const { return mLua; }

bool LuaRef::push() const {
    if (mLua == nullptr) {
        PureError("Error!!! the lua state is nullptr!!!");
        return false;
    }
    lua_rawgeti(mLua, LUA_REGISTRYINDEX, mRef);
    return true;
}
int LuaRef::type() const {
    if (!valid()) {
        return LUA_TNIL;
    }
    LuaStateProtect lsp(mLua);
    push();
    return lua_type(mLua, -1);
}

const char* LuaRef::type_name() const {
    if (mLua == nullptr) return "nil";
    return lua_typename(mLua, type());
}

bool LuaRef::valid() const { return !(mLua == nullptr || mRef == LUA_REFNIL || mRef == LUA_NOREF); }

bool LuaRef::is_nil() const { return (type() == LUA_TNIL); }
bool LuaRef::is_int() const {
    if (!valid()) {
        return false;
    }
    LuaStateProtect lsp(mLua);
    push();
    return lua_isinteger(mLua, -1) != 0;
}
bool LuaRef::is_number() const { return (type() == LUA_TNUMBER); }
bool LuaRef::is_string() const { return (type() == LUA_TSTRING); }
bool LuaRef::is_table() const { return (type() == LUA_TTABLE); }
bool LuaRef::is_function() const { return (type() == LUA_TFUNCTION); }
bool LuaRef::is_userdata() const { return (type() == LUA_TUSERDATA); }
bool LuaRef::is_thread() const { return (type() == LUA_TTHREAD); }
bool LuaRef::is_lightuserdata() const { return (type() == LUA_TLIGHTUSERDATA); }
bool LuaRef::is_bool() const { return (type() == LUA_TBOOLEAN); }

bool LuaRef::is_cfunction() const {
    LuaStateProtect lsp(mLua);
    if (push()) return lua_iscfunction(mLua, -1) == 1;
    return false;
}

size_t LuaRef::length() const {
    LuaStateProtect lsp(mLua);
    if (push() && lua_istable(mLua, -1)) {
        size_t len = lua_rawlen(mLua, -1);
        return len;
    }
    return 0u;
}

int LuaRef::table_index_check(lua_State* L) {
    const char* tag = lua_tostring(L, lua_upvalueindex(1));
    const char* key = lua_tostring(L, 2);
    luaL_error(L, "`%s` Not This Field `%s`", tag, key);
    return 0;
}

}  // namespace PureLua
