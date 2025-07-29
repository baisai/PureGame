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
#include "PureLua/LuaStackExt.h"

namespace PureLua {
class LuaRef;
class PURELUA_API LuaTableRef {
public:
    LuaTableRef();
    explicit LuaTableRef(const LuaRef &tab);
    LuaTableRef(const LuaRef &tab, const LuaRef &key);
    LuaTableRef(lua_State *L, int tabIdx, int keyIdx);
    LuaTableRef(const LuaTableRef &cp);
    LuaTableRef(LuaTableRef &&cp);
    ~LuaTableRef();

    void clear();

    int create_ref() const;

    LuaTableRef &operator=(const LuaTableRef &v);

    template <typename T>
    inline LuaTableRef &operator=(T v) {
        if (mLua != nullptr) {
            LuaStateProtect lsp(mLua);
            lua_rawgeti(mLua, LUA_REGISTRYINDEX, mTableRef);
            lua_rawgeti(mLua, LUA_REGISTRYINDEX, mKeyRef);
            LuaStack<T>::push(mLua, v);
            lua_settable(mLua, -3);
        }
        return *this;
    }

    template <typename T>
    inline LuaTableRef &set(T v) {
        if (mLua != nullptr) {
            LuaStateProtect lsp(mLua);
            lua_rawgeti(mLua, LUA_REGISTRYINDEX, mTableRef);
            lua_rawgeti(mLua, LUA_REGISTRYINDEX, mKeyRef);
            LuaStack<T>::push(mLua, v);
            lua_settable(mLua, -3);
        }
        return *this;
    }

    template <typename T>
    inline LuaTableRef &rawset(T v) {
        if (mLua != nullptr) {
            LuaStateProtect lsp(mLua);
            lua_rawgeti(mLua, LUA_REGISTRYINDEX, mTableRef);
            lua_rawgeti(mLua, LUA_REGISTRYINDEX, mKeyRef);
            LuaStack<T>::push(mLua, v);
            lua_rawset(mLua, -3);
        }
        return *this;
    }

    lua_State *state() const;

    bool push() const;

    int create_tab_ref() const;
    int create_key_ref() const;

    LuaRef get_ref() const;
    LuaRef get_tab_ref() const;
    LuaRef get_key_ref() const;

private:
    lua_State *mLua;
    int mTableRef;
    int mKeyRef;
};

template <>
struct LuaStack<LuaTableRef> {
    typedef const LuaTableRef &PushType;
    typedef LuaTableRef GetType;
    static inline void push(lua_State *L, PushType v) {
        if (L != v.state()) {
            PureLuaErrorJump(L,
                             "Error!!!The LuaStack<LuaTableRef>::push vm is "
                             "not LuaTableRef vm!!!");
        }
        v.push();
    }

    static inline GetType get(lua_State *L, int tabIdx, int keyIdx) { return LuaTableRef(L, tabIdx, keyIdx); }

    static inline bool valid(lua_State *L, int tabIdx, int keyIdx) { return lua_istable(L, tabIdx) && !lua_isnoneornil(L, keyIdx); }
};

template <>
struct LuaStack<const LuaTableRef> {
    typedef const LuaTableRef &PushType;
    typedef LuaTableRef GetType;
    static inline void push(lua_State *L, PushType v) {
        if (L != v.state()) {
            PureLuaErrorJump(L,
                             "Error!!!The LuaStack<const LuaTableRef>::push vm "
                             "is not LuaTableRef vm!!!");
        }
        v.push();
    }

    static inline GetType get(lua_State *L, int tabIdx, int keyIdx) { return LuaTableRef(L, tabIdx, keyIdx); }

    static inline bool valid(lua_State *L, int tabIdx, int keyIdx) { return lua_istable(L, tabIdx) && !lua_isnoneornil(L, keyIdx); }
};

template <>
struct LuaStack<LuaTableRef &> {
    typedef const LuaTableRef &PushType;
    typedef LuaTableRef GetType;
    static inline void push(lua_State *L, PushType v) {
        if (L != v.state()) {
            PureLuaErrorJump(L,
                             "Error!!!The LuaStack<LuaTableRef &>::push vm is "
                             "not LuaTableRef vm!!!");
        }
        v.push();
    }

    static inline GetType get(lua_State *L, int tabIdx, int keyIdx) { return LuaTableRef(L, tabIdx, keyIdx); }

    static inline bool valid(lua_State *L, int tabIdx, int keyIdx) { return lua_istable(L, tabIdx) && !lua_isnoneornil(L, keyIdx); }
};

template <>
struct LuaStack<const LuaTableRef &> {
    typedef const LuaTableRef &PushType;
    typedef LuaTableRef GetType;
    static inline void push(lua_State *L, PushType v) {
        if (L != v.state()) {
            PureLuaErrorJump(L,
                             "Error!!!The LuaStack<const LuaTableRef &>::push "
                             "vm is not LuaTableRef vm!!!");
        }
        v.push();
    }

    static inline GetType get(lua_State *L, int tabIdx, int keyIdx) { return LuaTableRef(L, tabIdx, keyIdx); }

    static inline bool valid(lua_State *L, int tabIdx, int keyIdx) { return lua_istable(L, tabIdx) && !lua_isnoneornil(L, keyIdx); }
};
}  // namespace PureLua
