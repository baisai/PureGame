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
#include "PureLua/LuaStateProtect.h"
#include "PureLua/LuaStackPushArgs.h"
#include "PureLua/LuaTableRef.h"
#include "PureLua/LuaHelper.h"
#include "PureLua/LuaResult.h"

namespace PureLua {
class PURELUA_API LuaRef {
public:
    LuaRef();
    LuaRef(lua_State *L);
    LuaRef(lua_State *L, int idx);
    LuaRef(const LuaTableRef &v);
    LuaRef(const LuaRef &cp);
    LuaRef(LuaRef &&cp);
    virtual ~LuaRef();

    template <typename T>
    inline static LuaRef new_ref(lua_State *L, const T &v) {
        if (L != nullptr) {
            LuaStack<typename LuaStack<T>::PushType>::push(L, v);
            return LuaRef(L, true);
        }
        return LuaRef(L, false);
    }

    void clear();

    void reset();
    void reset(lua_State *L);
    void reset(lua_State *L, int idx);
    void reset_pop_top(lua_State *L);

    void set_nil();

    int create_ref() const;

    LuaRef &operator=(const LuaRef &right);
    LuaRef &operator=(LuaRef &&right);
    LuaRef &operator=(const LuaTableRef &right);

    template <typename T>
    inline LuaRef &operator=(const T &right) {
        if (mLua != nullptr) {
            LuaStack<typename LuaStack<T>::PushType>::push(mLua, right);
            reset_pop_top(mLua);
        }
        return *this;
    }

    template <typename T>
    inline LuaRef new_sub_table(const T &key) {
        if (mLua != nullptr) {
            LuaRef sub = new_table(mLua);
            rawset(key, sub);
            return sub;
        }
        return LuaRef();
    }

    static LuaRef new_table(lua_State *L, int narr = 0, int nrec = 0);
    static LuaRef new_index_check_table(lua_State *L, PureCore::StringRef tag, int narr = 0, int nrec = 0);
    static LuaRef new_global_index_check_table(lua_State *L, PureCore::StringRef name, int narr = 0, int nrec = 0);
    template <typename KT>
    inline static LuaRef get_global_var(lua_State *L, const KT &key) {
        if (L != nullptr) {
            LuaStateProtect lsp(L);
            lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
            LuaStack<typename LuaStack<KT>::PushType>::push(L, key);
            lua_rawget(L, -2);
            return LuaRef(L, true);
        }
        return LuaRef();
    }
    template <typename KT, typename VT>
    inline static void set_global_var(lua_State *L, const KT &key, const VT &value) {
        if (L != nullptr) {
            LuaStateProtect lsp(L);
            lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
            LuaStack<typename LuaStack<KT>::PushType>::push(L, key);
            LuaStack<typename LuaStack<VT>::PushType>::push(L, value);
            lua_rawset(L, -3);
        }
    }

    std::string to_string() const;
    void print() const;

    lua_State *state() const;
    bool push() const;
    int type() const;
    const char *type_name() const;

    bool valid() const;
    bool is_nil() const;
    bool is_int() const;
    bool is_number() const;
    bool is_string() const;
    bool is_table() const;
    bool is_function() const;
    bool is_userdata() const;
    bool is_thread() const;
    bool is_lightuserdata() const;
    bool is_bool() const;
    bool is_cfunction() const;
    template <typename T>
    inline bool is_object() const {
        if (!valid()) {
            return LUA_TNIL;
        }
        LuaStateProtect lsp(mLua);
        push();
        return LuaStack<T>::valid(mLua, -1);
    }

    template <typename T>
    inline T cast() const {
        LuaStateProtect lsp(mLua);
        if (!push()) return T();
        return T(LuaStack<typename LuaStack<T>::GetType>::get(mLua, -1));
    }
    template <typename T>
    inline operator T() const {
        return cast<T>();
    }

    template <typename T>
    bool operator==(const T &right) const {
        LuaStateProtect lsp(mLua);
        if (!push()) return false;
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, right);
        bool eq = lua_compare(mLua, -1, -2, LUA_OPEQ) != 0;
        return eq;
    }
    template <typename T>
    inline bool operator!=(const T &right) const {
        return !((*this) == right);
    }
    template <typename T>
    bool operator<(const T &right) const {
        LuaStateProtect lsp(mLua);
        if (!push()) return false;
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, right);
        bool lt = lua_compare(mLua, -2, -1, LUA_OPLT) != 0;
        return lt;
    }
    template <typename T>
    bool operator<=(const T &right) const {
        LuaStateProtect lsp(mLua);
        if (!push()) return false;
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, right);
        bool elt = lua_compare(mLua, -2, -1, LUA_OPLE) != 0;
        return elt;
    }
    template <typename T>
    bool operator>(const T &right) const {
        LuaStateProtect lsp(mLua);
        if (!push()) return false;
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, right);
        bool mt = lua_compare(mLua, -1, -2, LUA_OPLT) != 0;
        return mt;
    }
    template <typename T>
    bool operator>=(const T &right) const {
        LuaStateProtect lsp(mLua);
        if (!push()) return false;
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, right);
        bool emt = lua_compare(mLua, -1, -2, LUA_OPLE) != 0;
        return emt;
    }
    template <typename T>
    bool rawequal(const T &right) const {
        LuaStateProtect lsp(mLua);
        if (!push()) return false;
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, right);
        bool req = lua_rawequal(mLua, -1, -2) == 1;
        return req;
    }

    template <typename T>
    bool append(const T &v) const {
        LuaStateProtect lsp(mLua);
        if (!push()) return false;
        size_t len = lua_rawlen(mLua, -1);
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, v);
        lua_rawseti(mLua, -2, len + 1);
        return true;
    }

    size_t length() const;

    template <typename T, typename V>
    LuaRef &rawset(const T &key, const V &v) {
        LuaStateProtect lsp(mLua);
        if (!push()) return *this;
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, key);
        LuaStack<typename LuaStack<V>::PushType>::push(mLua, v);
        lua_rawset(mLua, -3);
        return *this;
    }

    template <typename T, typename V>
    LuaRef &rawset(const T &key, V &&v) {
        LuaStateProtect lsp(mLua);
        if (!push()) return *this;
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, key);
        LuaStack<V>::push(mLua, std::move(v));
        lua_rawset(mLua, -3);
        return *this;
    }

    template <typename T>
    LuaRef rawget(const T &key) const {
        LuaStateProtect lsp(mLua);
        if (!push()) return LuaRef();
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, key);
        lua_rawget(mLua, -2);
        LuaRef t(mLua, true);
        return t;
    }

    template <typename T>
    bool rawhas(const T &key) const {
        LuaStateProtect lsp(mLua);
        if (!push()) return false;
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, key);
        lua_rawget(mLua, -2);
        return !lua_isnoneornil(mLua, -1);
    }

    template <typename T, typename V>
    LuaRef &setfield(const T &key, const V &v) {
        LuaStateProtect lsp(mLua);
        if (!push()) return *this;
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, key);
        LuaStack<typename LuaStack<V>::PushType>::push(mLua, v);
        lua_settable(mLua, -3);
        return *this;
    }

    template <typename T, typename V>
    LuaRef &setfield(const T &key, V &&v) {
        LuaStateProtect lsp(mLua);
        if (!push()) return *this;
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, key);
        LuaStack<V>::push(mLua, std::move(v));
        lua_settable(mLua, -3);
        return *this;
    }

    template <typename T>
    LuaRef getfield(const T &key) const {
        LuaStateProtect lsp(mLua);
        if (!push()) return LuaRef();
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, key);
        lua_gettable(mLua, -2);
        LuaRef t(mLua, true);
        return t;
    }

    template <typename T>
    bool hasfield(const T &key) const {
        LuaStateProtect lsp(mLua);
        if (!push()) return false;
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, key);
        lua_gettable(mLua, -2);
        return !lua_isnoneornil(mLua, -1);
    }

    template <typename T>
    LuaTableRef operator[](const T &key) const {
        if (mLua == nullptr) {
            PureError("Error!!!the lua state is nullptr!!!");
            return LuaTableRef();
        }
        LuaStack<typename LuaStack<T>::PushType>::push(mLua, key);
        return LuaTableRef(*this);
    }

    template <typename... Args>
    LuaResult<LuaRef> operator()(Args &&...args) const {
        LuaStateProtect lsp(mLua);
        int err = push_lua_error_catcher(mLua);
        if (!push()) {
            return LuaResult<LuaRef>("the ref is not found");
        }
        int s = lua_stack_push_args(mLua, std::forward<Args>(args)...);
        if (lua_pcall(mLua, s, 1, err)) {
            const char *errStr = lua_tostring(mLua, -1);
            if (errStr != nullptr) {
                return LuaResult<LuaRef>(errStr);
            }
            return LuaResult<LuaRef>(get_error_desc(ErrorLuaPcallFailed));
        }
        return LuaResult<LuaRef>(mLua, -1);
    }

private:
    static int table_index_check(lua_State *L);

private:
    lua_State *mLua;
    int mRef;
};

template <>
struct LuaStack<LuaRef> {
    typedef const LuaRef &PushType;
    typedef LuaRef GetType;
    static inline void push(lua_State *L, PushType v) {
        if (L != v.state()) {
            PureLuaErrorJump(L, "Error!!!The LuaStack<LuaRef>::push vm is not LuaRef vm!!!");
        }
        if (!v.push()) {
            PureLuaErrorJump(L, "Error!!!The LuaStack<LuaRef>::push failed!!!");
        }
    }

    static inline GetType get(lua_State *L, int idx) { return LuaRef(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return true; }
};

template <>
struct LuaStack<const LuaRef> {
    typedef const LuaRef &PushType;
    typedef LuaRef GetType;
    static inline void push(lua_State *L, PushType v) {
        if (L != v.state()) {
            PureLuaErrorJump(L,
                             "Error!!!The LuaStack<const LuaRef>::push vm is "
                             "not LuaRef vm!!!");
        }
        if (!v.push()) {
            PureLuaErrorJump(L, "Error!!!The LuaStack<LuaRef>::push failed!!!");
        }
    }

    static inline GetType get(lua_State *L, int idx) { return LuaRef(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return true; }
};

template <>
struct LuaStack<LuaRef &> {
    typedef const LuaRef &PushType;
    typedef LuaRef GetType;
    static inline void push(lua_State *L, PushType v) {
        if (L != v.state()) {
            PureLuaErrorJump(L, "Error!!!The LuaStack<LuaRef &>::push vm is not LuaRef vm!!!");
        }
        if (!v.push()) {
            PureLuaErrorJump(L, "Error!!!The LuaStack<LuaRef>::push failed!!!");
        }
    }

    static inline GetType get(lua_State *L, int idx) { return LuaRef(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return true; }
};

template <>
struct LuaStack<const LuaRef &> {
    typedef const LuaRef &PushType;
    typedef LuaRef GetType;
    static inline void push(lua_State *L, PushType v) {
        if (L != v.state()) {
            PureLuaErrorJump(L,
                             "Error!!!The LuaStack<const LuaRef &>::push vm is "
                             "not LuaRef vm!!!");
        }
        if (!v.push()) {
            PureLuaErrorJump(L, "Error!!!The LuaStack<LuaRef>::push failed!!!");
        }
    }

    static inline GetType get(lua_State *L, int idx) { return LuaRef(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return true; }
};

}  // namespace PureLua
