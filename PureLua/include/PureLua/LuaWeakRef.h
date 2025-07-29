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

#include "PureLua/LuaStateProtect.h"
#include "PureLua/LuaStack.h"
#include "PureLua/LuaRef.h"
#include "PureLua/LuaTableRef.h"
#include "PureLua/LuaHelper.h"

namespace PureLua {
class PURELUA_API LuaWeakRef {
public:
    LuaWeakRef();
    LuaWeakRef(lua_State *L, bool top);
    LuaWeakRef(lua_State *L, int idx, bool pop);
    LuaWeakRef(const LuaTableRef &v);
    LuaWeakRef(const LuaRef &v);
    LuaWeakRef(const LuaWeakRef &cp);
    ~LuaWeakRef();

    void clear();

    LuaWeakRef &operator=(const LuaRef &right);
    LuaWeakRef &operator=(const LuaTableRef &right);
    LuaWeakRef &operator=(const LuaWeakRef &right);

    lua_State *state() const;
    bool push() const;

    bool valid() const;

    void reset();
    void reset(lua_State *L);
    void reset(lua_State *L, int idx);
    void reset_pop_top(lua_State *L);

    void set_nil();

    LuaRef get_ref() const;

private:
    bool push_parent() const;
    int ref(int t) const;
    void unref(int t, int ref) const;

private:
    lua_State *mLua;
    int mRef;

    static const char sWeakKey;
    static const char sRefKey;
};

template <>
struct LuaStack<LuaWeakRef> {
    typedef const LuaWeakRef &PushType;
    typedef LuaWeakRef GetType;
    static inline void push(lua_State *L, PushType v) {
        if (L != v.state()) {
            PureLuaErrorJump(L,
                             "Error!!!The LuaStack<LuaWeakRef>::push vm is not "
                             "LuaWeakRef vm!!!");
        }
        if (!v.push()) {
            PureLuaErrorJump(L, "Error!!!The LuaStack<LuaWeakRef>::push failed!!!");
        }
    }

    static inline GetType get(lua_State *L, int idx) { return LuaWeakRef(L, idx, false); }

    static inline bool valid(lua_State *L, int idx) { return true; }
};

template <>
struct LuaStack<const LuaWeakRef> {
    typedef const LuaWeakRef &PushType;
    typedef LuaWeakRef GetType;
    static inline void push(lua_State *L, PushType v) {
        if (L != v.state()) {
            PureLuaErrorJump(L,
                             "Error!!!The LuaStack<const LuaWeakRef>::push vm "
                             "is not LuaWeakRef vm!!!");
        }
        if (!v.push()) {
            PureLuaErrorJump(L, "Error!!!The LuaStack<LuaWeakRef>::push failed!!!");
        }
    }

    static inline GetType get(lua_State *L, int idx) { return LuaWeakRef(L, idx, false); }

    static inline bool valid(lua_State *L, int idx) { return true; }
};

template <>
struct LuaStack<LuaWeakRef &> {
    typedef const LuaWeakRef &PushType;
    typedef LuaWeakRef GetType;
    static inline void push(lua_State *L, PushType v) {
        if (L != v.state()) {
            PureLuaErrorJump(L,
                             "Error!!!The LuaStack<LuaWeakRef &>::push vm is "
                             "not LuaWeakRef vm!!!");
        }
        if (!v.push()) {
            PureLuaErrorJump(L, "Error!!!The LuaStack<LuaWeakRef>::push failed!!!");
        }
    }

    static inline GetType get(lua_State *L, int idx) { return LuaWeakRef(L, idx, false); }

    static inline bool valid(lua_State *L, int idx) { return true; }
};

template <>
struct LuaStack<const LuaWeakRef &> {
    typedef const LuaWeakRef &PushType;
    typedef LuaWeakRef GetType;
    static inline void push(lua_State *L, PushType v) {
        if (L != v.state()) {
            PureLuaErrorJump(L,
                             "Error!!!The LuaStack<const LuaWeakRef &>::push "
                             "vm is not LuaWeakRef vm!!!");
        }
        if (!v.push()) {
            PureLuaErrorJump(L, "Error!!!The LuaStack<LuaWeakRef>::push failed!!!");
        }
    }

    static inline GetType get(lua_State *L, int idx) { return LuaWeakRef(L, idx, false); }

    static inline bool valid(lua_State *L, int idx) { return true; }
};

}  // namespace PureLua
