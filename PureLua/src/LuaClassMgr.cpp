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

#include "PureLua/PureLuaLog.h"
#include "PureCore/StringRef.h"
#include "PureLua/LuaStateProtect.h"
#include "PureLua/LuaClassMgr.h"
#include "PureLua/LuaHelper.h"

namespace PureLua {
static const int32_t sInvalidOffset = INT32_MAX;
static const char sKey = 1;
static const char sObjectKey = 2;
static const char sClassKey = 3;
static const char sWeakKey = 4;
static const char sNewerKey = 5;
static const char sDelerKey = 6;

LuaClassMgr::LuaClassMgr() {}

LuaClassMgr::~LuaClassMgr() {
    for (std::unordered_map<const void *, LuaClass *>::const_iterator iter(mClassTable.begin()); iter != mClassTable.end(); ++iter) {
        delete iter->second;
    }
    mClassTable.clear();
}

void LuaClassMgr::register_class(const void *key) {
    if (mClassTable.find(key) == mClassTable.end()) {
        LuaClass *c = new LuaClass;
        c->set_class_offset(key, 0, false);
        mClassTable.insert(std::make_pair(key, c));
    }
}

bool LuaClassMgr::is_registered(const void *key) { return mClassTable.find(key) != mClassTable.end(); }

bool LuaClassMgr::add_base_class(const void *key, const void *base, int32_t offset) {
    std::unordered_map<const void *, LuaClass *>::const_iterator iter = mClassTable.find(key);
    if (iter == mClassTable.end()) {
        PureError("the child class {} is not register", key);
        return false;
    }

    if (key == base) return iter->second->set_class_offset(key, 0, false);

    std::unordered_map<const void *, LuaClass *>::const_iterator baseIter = mClassTable.find(base);
    if (baseIter == mClassTable.end()) {
        PureError("the base class {} is not register", base);
        return false;
    }
    iter->second->set_class_offset(base, offset, true);
    baseIter->second->set_class_offset(key, -offset, false);
    const std::vector<LuaClassInfo> &linkClass = baseIter->second->get_link_class();
    for (size_t idx = 0u; idx < linkClass.size(); ++idx) {
        if (linkClass[idx].mIsBase) {
            iter->second->set_class_offset(linkClass[idx].mKey, offset + linkClass[idx].mOffset, true);
            std::unordered_map<const void *, LuaClass *>::const_iterator baseBase = mClassTable.find(linkClass[idx].mKey);
            if (baseBase != mClassTable.end()) {
                baseBase->second->set_class_offset(key, -offset - linkClass[idx].mOffset, false);
            }
        }
    }
    return true;
}

int32_t LuaClassMgr::get_class_offset(const void *fromKey, const void *toKey) {
    if (fromKey == nullptr || toKey == nullptr) {
        return sInvalidOffset;
    }
    std::unordered_map<const void *, LuaClass *>::const_iterator iter = mClassTable.find(fromKey);
    if (iter == mClassTable.end()) {
        PureError("the child class {} is not register", fromKey);
        return sInvalidOffset;
    }
    return iter->second->get_class_offset(toKey);
}

bool LuaClassMgr::is_valid_offset(int32_t offset) { return offset != sInvalidOffset; }

void *LuaClassMgr::get_key() { return (void *)sKey; }

void *LuaClassMgr::get_object_key() { return (void *)sObjectKey; }

void *LuaClassMgr::get_class_key() { return (void *)sClassKey; }

void *LuaClassMgr::get_weak_key() { return (void *)sWeakKey; }

void *LuaClassMgr::get_newer_key() { return (void *)sNewerKey; }

void *LuaClassMgr::get_deler_key() { return (void *)sDelerKey; }

LuaClassMgr *LuaClassMgr::get_class_mgr(lua_State *L) {
    LuaStateProtect lsp(L);
    lua_pushlightuserdata(L, LuaClassMgr::get_key());
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_islightuserdata(L, -1)) {
        return ((LuaClassMgr *)lua_touserdata(L, -1));
    }
    LuaClassMgr *pMgr = new LuaClassMgr;
    lua_pushlightuserdata(L, LuaClassMgr::get_key());
    lua_pushlightuserdata(L, pMgr);
    lua_rawset(L, LUA_REGISTRYINDEX);
    push_keep_field(L);
    return pMgr;
}

void LuaClassMgr::release_class_mgr(lua_State *L) {
    LuaStateProtect lsp(L);
    lua_pushlightuserdata(L, LuaClassMgr::get_key());
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_islightuserdata(L, -1)) {
        LuaClassMgr *pMgr = (LuaClassMgr *)lua_touserdata(L, -1);
        delete pMgr;
        lua_pushlightuserdata(L, LuaClassMgr::get_key());
        lua_pushnil(L);
        lua_rawset(L, LUA_REGISTRYINDEX);
    }
}

int32_t LuaClassMgr::calc_offset(lua_State *L, int fromIdx, int toIdx) {
    LuaStateProtect lsp(L);
    fromIdx = lua_absindex(L, fromIdx);
    toIdx = lua_absindex(L, toIdx);

    if (!lua_istable(L, fromIdx)) {
        return sInvalidOffset;
    }
    lua_pushlightuserdata(L, LuaClassMgr::get_class_key());
    lua_gettable(L, fromIdx);
    void *fromKey = lua_touserdata(L, -1);

    if (!lua_istable(L, toIdx)) {
        return sInvalidOffset;
    }
    lua_pushlightuserdata(L, LuaClassMgr::get_class_key());
    lua_gettable(L, toIdx);
    void *toKey = lua_touserdata(L, -1);

    return get_class_mgr(L)->get_class_offset(fromKey, toKey);
}

static const char *sKeepField[] = {"cname__",    "gc__",   "is_auto_gc__", "set_auto_gc__", "cast__", "toptr__", "ctor__",     "__add",       "__sub",
                                   "__mul",      "__div",  "__mod",        "__pow",         "__unm",  "__idiv",  "__band",     "__bor",       "__bxor",
                                   "__bnot",     "__shl",  "__shr",        "__concat",      "__len",  "__eq",    "__lt",       "__le",        "__index",
                                   "__newindex", "__call", "__gc",         "__close",       "__mode", "__name",  "__tostring", "__metatable", "__pairs"};
void LuaClassMgr::push_keep_field(lua_State *L) {
    lua_pushlightuserdata(L, sKeepField);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_createtable(L, 0, PURE_ARRAY_SIZE(sKeepField));
        lua_pushlightuserdata(L, sKeepField);
        lua_pushvalue(L, -2);
        lua_rawset(L, LUA_REGISTRYINDEX);
        for (size_t idx = 0; idx < PURE_ARRAY_SIZE(sKeepField); ++idx) {
            lua_pushstring(L, sKeepField[idx]);
            lua_pushboolean(L, 1);
            lua_rawset(L, -3);
        }
    }
}

bool LuaClassMgr::check_keep_field_str(lua_State *L, PureCore::StringRef str) {
    LuaStateProtect lsp(L);
    lua_pushlstring(L, str.data(), str.size());
    return check_keep_field_stack(L, -1);
}

bool LuaClassMgr::check_keep_field_stack(lua_State *L, int fieldIdx) {
    fieldIdx = lua_absindex(L, fieldIdx);
    LuaStateProtect lsp(L);
    push_keep_field(L);
    lua_pushvalue(L, fieldIdx);
    lua_rawget(L, -2);
    return lua_toboolean(L, -1) != 0;
}

bool LuaClassMgr::LuaClass::set_class_offset(const void *key, int32_t offset, bool isBase) {
    for (size_t idx = 0u; idx < mLinkClass.size(); ++idx) {
        if (mLinkClass[idx].mKey == key) return false;
    }
    mLinkClass.push_back(LuaClassInfo(key, offset, isBase));
    return true;
}

int32_t LuaClassMgr::LuaClass::get_class_offset(const void *key) {
    for (size_t idx = 0u; idx < mLinkClass.size(); ++idx) {
        if (mLinkClass[idx].mKey == key) {
            return mLinkClass[idx].mOffset;
        }
    }
    return sInvalidOffset;
}

#ifdef PURE_DEBUG
size_t debug_class_info(const std::type_info &info) {
    static std::unordered_map<std::string, size_t> sClassInfo;
    auto classId = info.hash_code();
    auto res = sClassInfo.insert(std::make_pair(info.name(), classId));
    assert(res.first->second == classId);
    return classId;
}
#endif

}  // namespace PureLua
