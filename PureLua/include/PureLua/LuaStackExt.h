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

#include "PureCore/DataRef.h"
#include "PureCore/ArrayRef.h"
#include "PureLua/LuaUserdata.h"
#include "PureLua/LuaStackBase.h"
#include "PureLua/LuaStackEnumHelper.h"

#if PURE_CPP >= 201703L
#include <string_view>
#endif

namespace PureLua {
template <typename T>
inline bool Equals(const T &left, const T &right) {
    return left == right;
}

// const char *
template <>
struct LuaStack<const char *> {
    typedef const char *PushType;
    typedef const char *GetType;
    static inline void push(lua_State *L, PushType str) {
        if (str) {
            lua_pushstring(L, str);
        } else {
            lua_pushnil(L);
        }
    }

    static inline GetType get(lua_State *L, int idx) { return (lua_isnoneornil(L, idx) ? nullptr : luaL_checkstring(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// char *
template <>
struct LuaStack<char *> {
    typedef const char *PushType;
    typedef const char *GetType;
    static inline void push(lua_State *L, PushType str) {
        if (str) {
            lua_pushstring(L, str);
        } else {
            lua_pushnil(L);
        }
    }

    static inline GetType get(lua_State *L, int idx) { return (lua_isnoneornil(L, idx) ? nullptr : luaL_checkstring(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// const char * &
template <>
struct LuaStack<const char *&> {
    typedef const char *PushType;
    typedef const char *GetType;
    static inline void push(lua_State *L, PushType str) {
        if (str) {
            lua_pushstring(L, str);
        } else {
            lua_pushnil(L);
        }
    }

    static inline GetType get(lua_State *L, int idx) { return (lua_isnoneornil(L, idx) ? nullptr : luaL_checkstring(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// char * &
template <>
struct LuaStack<char *&> {
    typedef const char *PushType;
    typedef const char *GetType;
    static inline void push(lua_State *L, PushType str) {
        if (str) {
            lua_pushstring(L, str);
        } else {
            lua_pushnil(L);
        }
    }

    static inline GetType get(lua_State *L, int idx) { return (lua_isnoneornil(L, idx) ? nullptr : luaL_checkstring(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// char * const &
template <>
struct LuaStack<char *const &> {
    typedef const char *PushType;
    typedef const char *GetType;
    static inline void push(lua_State *L, PushType str) {
        if (str) {
            lua_pushstring(L, str);
        } else {
            lua_pushnil(L);
        }
    }

    static inline GetType get(lua_State *L, int idx) { return (lua_isnoneornil(L, idx) ? nullptr : luaL_checkstring(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// const char[size]
template <size_t size>
struct LuaStack<const char[size]> {
    typedef const char *PushType;
    typedef const char *GetType;
    static inline void push(lua_State *L, PushType str) {
        if (str) {
            lua_pushlstring(L, str, strnlen(str, size));
        } else {
            lua_pushnil(L);
        }
    }

    static inline GetType get(lua_State *L, int idx) { return (lua_isnoneornil(L, idx) ? nullptr : luaL_checkstring(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// char[size]
template <size_t size>
struct LuaStack<char[size]> {
    typedef const char *PushType;
    typedef const char *GetType;
    static inline void push(lua_State *L, PushType str) {
        if (str) {
            lua_pushlstring(L, str, strnlen(str, size));
        } else {
            lua_pushnil(L);
        }
    }

    static inline GetType get(lua_State *L, int idx) { return (lua_isnoneornil(L, idx) ? nullptr : luaL_checkstring(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// const char[size]&
template <size_t size>
struct LuaStack<const char (&)[size]> {
    typedef const char *PushType;
    typedef const char *GetType;
    static inline void push(lua_State *L, PushType str) {
        if (str) {
            lua_pushlstring(L, str, strnlen(str, size));
        } else {
            lua_pushnil(L);
        }
    }

    static inline GetType get(lua_State *L, int idx) { return (lua_isnoneornil(L, idx) ? nullptr : luaL_checkstring(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// char[size]&
template <size_t size>
struct LuaStack<char (&)[size]> {
    typedef const char *PushType;
    typedef const char *GetType;
    static inline void push(lua_State *L, PushType str) {
        if (str) {
            lua_pushlstring(L, str, strnlen(str, size));
        } else {
            lua_pushnil(L);
        }
    }

    static inline GetType get(lua_State *L, int idx) { return (lua_isnoneornil(L, idx) ? nullptr : luaL_checkstring(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// PureCore::StringRef
template <>
struct LuaStack<PureCore::StringRef> {
    typedef PureCore::StringRef PushType;
    typedef PureCore::StringRef GetType;
    static inline void push(lua_State *L, PushType str) { lua_pushlstring(L, str.data(), str.size()); }

    static inline GetType get(lua_State *L, int idx) {
        size_t len = 0;
        const char *str = luaL_checklstring(L, idx, &len);
        return PureCore::StringRef(str, len);
    }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// const PureCore::StringRef
template <>
struct LuaStack<const PureCore::StringRef> {
    typedef PureCore::StringRef PushType;
    typedef PureCore::StringRef GetType;
    static inline void push(lua_State *L, PushType str) { lua_pushlstring(L, str.data(), str.size()); }

    static inline GetType get(lua_State *L, int idx) {
        size_t len = 0;
        const char *str = luaL_checklstring(L, idx, &len);
        return PureCore::StringRef(str, len);
    }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// PureCore::StringRef &
template <>
struct LuaStack<PureCore::StringRef &> {
    typedef PureCore::StringRef &PushType;
    typedef PureCore::StringRef &GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<PureCore::StringRef>>(L, &str); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<PureCore::StringRef>>(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<PureCore::StringRef>>::is_class_type(L, idx); }
};

// const PureCore::StringRef &
template <>
struct LuaStack<const PureCore::StringRef &> {
    typedef const PureCore::StringRef &PushType;
    typedef const PureCore::StringRef GetType;
    static inline void push(lua_State *L, PushType str) { lua_pushlstring(L, str.data(), str.size()); }

    static inline GetType get(lua_State *L, int idx) {
        size_t len = 0;
        const char *str = luaL_checklstring(L, idx, &len);
        return PureCore::StringRef(str, len);
    }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// PureCore::StringRef *
template <>
struct LuaStack<PureCore::StringRef *> {
    typedef PureCore::StringRef *PushType;
    typedef PureCore::StringRef *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<PureCore::StringRef>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<PureCore::StringRef> *lp = LuaUserdataPtr::get<LuaPointer<PureCore::StringRef>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<PureCore::StringRef>>::is_class_type(L, idx); }
};

// const PureCore::StringRef *
template <>
struct LuaStack<const PureCore::StringRef *> {
    typedef const PureCore::StringRef *PushType;
    typedef const PureCore::StringRef *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<PureCore::StringRef>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<PureCore::StringRef> *lp = LuaUserdataPtr::get<LuaPointer<PureCore::StringRef>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<PureCore::StringRef>>::is_class_type(L, idx); }
};

// PureCore::StringRef * &
template <>
struct LuaStack<PureCore::StringRef *&> {
    typedef PureCore::StringRef *PushType;
    typedef PureCore::StringRef *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<PureCore::StringRef>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<PureCore::StringRef> *lp = LuaUserdataPtr::get<LuaPointer<PureCore::StringRef>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<PureCore::StringRef>>::is_class_type(L, idx); }
};

// PureCore::StringRef * const &
template <>
struct LuaStack<PureCore::StringRef *const &> {
    typedef PureCore::StringRef *PushType;
    typedef PureCore::StringRef *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<PureCore::StringRef>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<PureCore::StringRef> *lp = LuaUserdataPtr::get<LuaPointer<PureCore::StringRef>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<PureCore::StringRef>>::is_class_type(L, idx); }
};

// const PureCore::StringRef * &
template <>
struct LuaStack<const PureCore::StringRef *&> {
    typedef const PureCore::StringRef *PushType;
    typedef const PureCore::StringRef *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<PureCore::StringRef>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<PureCore::StringRef> *lp = LuaUserdataPtr::get<LuaPointer<PureCore::StringRef>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<PureCore::StringRef>>::is_class_type(L, idx); }
};

#if PURE_CPP >= 201703L
// std::string_view
template <>
struct LuaStack<std::string_view> {
    typedef std::string_view PushType;
    typedef std::string_view GetType;
    static inline void push(lua_State *L, PushType str) { lua_pushlstring(L, str.data(), str.size()); }

    static inline GetType get(lua_State *L, int idx) {
        size_t len = 0;
        const char *str = luaL_checklstring(L, idx, &len);
        return std::string_view(str, len);
    }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// const std::string_view
template <>
struct LuaStack<const std::string_view> {
    typedef std::string_view PushType;
    typedef std::string_view GetType;
    static inline void push(lua_State *L, PushType str) { lua_pushlstring(L, str.data(), str.size()); }

    static inline GetType get(lua_State *L, int idx) {
        size_t len = 0;
        const char *str = luaL_checklstring(L, idx, &len);
        return std::string_view(str, len);
    }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// std::string_view &
template <>
struct LuaStack<std::string_view &> {
    typedef std::string_view &PushType;
    typedef std::string_view &GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<std::string_view>>(L, &str); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<std::string_view>>(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<std::string_view>>::is_class_type(L, idx); }
};

// const std::string_view &
template <>
struct LuaStack<const std::string_view &> {
    typedef const std::string_view &PushType;
    typedef const std::string_view GetType;
    static inline void push(lua_State *L, PushType str) { lua_pushlstring(L, str.data(), str.size()); }

    static inline GetType get(lua_State *L, int idx) {
        size_t len = 0;
        const char *str = luaL_checklstring(L, idx, &len);
        return std::string_view(str, len);
    }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// std::string_view *
template <>
struct LuaStack<std::string_view *> {
    typedef std::string_view *PushType;
    typedef std::string_view *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<std::string_view>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<std::string_view> *lp = LuaUserdataPtr::get<LuaPointer<std::string_view>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<std::string_view>>::is_class_type(L, idx); }
};

// const std::string_view *
template <>
struct LuaStack<const std::string_view *> {
    typedef const std::string_view *PushType;
    typedef const std::string_view *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<std::string_view>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<std::string_view> *lp = LuaUserdataPtr::get<LuaPointer<std::string_view>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<std::string_view>>::is_class_type(L, idx); }
};

// std::string_view * &
template <>
struct LuaStack<std::string_view *&> {
    typedef std::string_view *PushType;
    typedef std::string_view *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<std::string_view>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<std::string_view> *lp = LuaUserdataPtr::get<LuaPointer<std::string_view>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<std::string_view>>::is_class_type(L, idx); }
};

// std::string_view * const &
template <>
struct LuaStack<std::string_view *const &> {
    typedef std::string_view *PushType;
    typedef std::string_view *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<std::string_view>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<std::string_view> *lp = LuaUserdataPtr::get<LuaPointer<std::string_view>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<std::string_view>>::is_class_type(L, idx); }
};

// const std::string_view * &
template <>
struct LuaStack<const std::string_view *&> {
    typedef const std::string_view *PushType;
    typedef const std::string_view *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<std::string_view>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<std::string_view> *lp = LuaUserdataPtr::get<LuaPointer<std::string_view>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<std::string_view>>::is_class_type(L, idx); }
};
#endif

// std::string
template <>
struct LuaStack<std::string> {
    typedef const std::string &PushType;
    typedef std::string GetType;
    static inline void push(lua_State *L, PushType str) { lua_pushlstring(L, str.c_str(), str.size()); }

    static inline GetType get(lua_State *L, int idx) {
        size_t len = 0;
        const char *str = luaL_checklstring(L, idx, &len);
        return std::string(str, len);
    }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// const std::string
template <>
struct LuaStack<const std::string> {
    typedef const std::string &PushType;
    typedef const std::string GetType;
    static inline void push(lua_State *L, PushType str) { lua_pushlstring(L, str.c_str(), str.size()); }

    static inline GetType get(lua_State *L, int idx) {
        size_t len = 0;
        const char *str = luaL_checklstring(L, idx, &len);
        return std::string(str, len);
    }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// std::string &
template <>
struct LuaStack<std::string &> {
    typedef std::string &PushType;
    typedef std::string &GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<std::string>>(L, &str); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<std::string>>(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<std::string>>::is_class_type(L, idx); }
};

// const std::string &
template <>
struct LuaStack<const std::string &> {
    typedef const std::string &PushType;
    typedef const std::string GetType;
    static inline void push(lua_State *L, PushType str) { lua_pushlstring(L, str.c_str(), str.size()); }

    static inline GetType get(lua_State *L, int idx) {
        size_t len = 0;
        const char *str = luaL_checklstring(L, idx, &len);
        return std::string(str, len);
    }

    static inline bool valid(lua_State *L, int idx) { return lua_isstring(L, idx); }
};

// std::string *
template <>
struct LuaStack<std::string *> {
    typedef std::string *PushType;
    typedef std::string *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<std::string>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<std::string> *lp = LuaUserdataPtr::get<LuaPointer<std::string>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<std::string>>::is_class_type(L, idx); }
};

// const std::string *
template <>
struct LuaStack<const std::string *> {
    typedef const std::string *PushType;
    typedef const std::string *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<std::string>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<std::string> *lp = LuaUserdataPtr::get<LuaPointer<std::string>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<std::string>>::is_class_type(L, idx); }
};

// std::string * &
template <>
struct LuaStack<std::string *&> {
    typedef std::string *PushType;
    typedef std::string *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<std::string>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<std::string> *lp = LuaUserdataPtr::get<LuaPointer<std::string>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<std::string>>::is_class_type(L, idx); }
};

// std::string * const &
template <>
struct LuaStack<std::string *const &> {
    typedef std::string *PushType;
    typedef std::string *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<std::string>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<std::string> *lp = LuaUserdataPtr::get<LuaPointer<std::string>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<std::string>>::is_class_type(L, idx); }
};

// const std::string * &
template <>
struct LuaStack<const std::string *&> {
    typedef const std::string *PushType;
    typedef const std::string *GetType;
    static inline void push(lua_State *L, PushType str) { LuaUserdata::push<LuaPointer<std::string>>(L, str); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<std::string> *lp = LuaUserdataPtr::get<LuaPointer<std::string>>(L, idx);
        return lp ? lp->get_ptr() : nullptr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<std::string>>::is_class_type(L, idx); }
};

// PureCore::MovePtr
template <typename T, typename Deler>
struct LuaStack<PureCore::MovePtr<T, Deler>> {
    typedef PureCore::MovePtr<T, Deler> PushType;
    typedef PureCore::MovePtr<T, Deler> GetType;
    static inline void push(lua_State *L, PushType ptr) { LuaUserdataMovePtr::push<PushType>(L, ptr); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataMovePtr::get<GetType>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// const PureCore::MovePtr
template <typename T, typename Deler>
struct LuaStack<const PureCore::MovePtr<T, Deler>> {
    typedef PureCore::MovePtr<T, Deler> PushType;
    typedef PureCore::MovePtr<T, Deler> GetType;
    static inline void push(lua_State *L, PushType ptr) { LuaUserdataMovePtr::push<PushType>(L, ptr); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataMovePtr::get<GetType>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// PureCore::MovePtr &
template <typename T, typename Deler>
struct LuaStack<PureCore::MovePtr<T, Deler> &> {
    typedef PureCore::MovePtr<T, Deler> PushType;
    typedef PureCore::MovePtr<T, Deler> GetType;
    static inline void push(lua_State *L, PushType ptr) { LuaUserdataMovePtr::push<PushType>(L, ptr); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataMovePtr::get<GetType>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// const PureCore::MovePtr &
template <typename T, typename Deler>
struct LuaStack<const PureCore::MovePtr<T, Deler> &> {
    typedef PureCore::MovePtr<T, Deler> PushType;
    typedef PureCore::MovePtr<T, Deler> GetType;
    static inline void push(lua_State *L, PushType ptr) { LuaUserdataMovePtr::push<PushType>(L, ptr); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataMovePtr::get<GetType>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// std::unique_ptr
template <typename T, typename Deler>
struct LuaStack<std::unique_ptr<T, Deler>> {
    typedef std::unique_ptr<T, Deler> PushType;
    typedef std::unique_ptr<T, Deler> GetType;
    static inline void push(lua_State *L, PushType ptr) { LuaUserdataMovePtr::push<PushType>(L, std::move(ptr)); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataMovePtr::get<GetType>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// const std::unique_ptr
template <typename T, typename Deler>
struct LuaStack<const std::unique_ptr<T, Deler>> {
    typedef std::unique_ptr<T, Deler> PushType;
    typedef std::unique_ptr<T, Deler> GetType;
    static inline void push(lua_State *L, PushType ptr) { LuaUserdataMovePtr::push<PushType>(L, ptr); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataMovePtr::get<GetType>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// std::unique_ptr &
template <typename T, typename Deler>
struct LuaStack<std::unique_ptr<T, Deler> &> {
    typedef std::unique_ptr<T, Deler> PushType;
    typedef std::unique_ptr<T, Deler> GetType;
    static inline void push(lua_State *L, PushType ptr) { LuaUserdataMovePtr::push<PushType>(L, ptr); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataMovePtr::get<GetType>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// const std::unique_ptr &
template <typename T, typename Deler>
struct LuaStack<const std::unique_ptr<T, Deler> &> {
    typedef std::unique_ptr<T, Deler> PushType;
    typedef std::unique_ptr<T, Deler> GetType;
    static inline void push(lua_State *L, PushType ptr) { LuaUserdataMovePtr::push<PushType>(L, ptr); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataMovePtr::get<GetType>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// void *
template <>
struct LuaStack<void *> {
    typedef const void *PushType;
    typedef void *GetType;
    static inline void push(lua_State *L, PushType p) { lua_pushlightuserdata(L, (void *)p); }

    static inline GetType get(lua_State *L, int idx) { return lua_touserdata(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return lua_islightuserdata(L, idx); }
};

// const void *
template <>
struct LuaStack<const void *> {
    typedef const void *PushType;
    typedef void *GetType;
    static inline void push(lua_State *L, PushType p) { lua_pushlightuserdata(L, (void *)p); }

    static inline GetType get(lua_State *L, int idx) { return lua_touserdata(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return lua_islightuserdata(L, idx); }
};

// void * &
template <>
struct LuaStack<void *&> {
    typedef const void *PushType;
    typedef void *GetType;
    static inline void push(lua_State *L, PushType p) { lua_pushlightuserdata(L, (void *)p); }

    static inline GetType get(lua_State *L, int idx) { return lua_touserdata(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return lua_islightuserdata(L, idx); }
};

// void * const &
template <>
struct LuaStack<void *const &> {
    typedef const void *PushType;
    typedef void *GetType;
    static inline void push(lua_State *L, PushType p) { lua_pushlightuserdata(L, (void *)p); }

    static inline GetType get(lua_State *L, int idx) { return lua_touserdata(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return lua_islightuserdata(L, idx); }
};

// const void * &
template <>
struct LuaStack<const void *&> {
    typedef const void *PushType;
    typedef void *GetType;
    static inline void push(lua_State *L, PushType p) { lua_pushlightuserdata(L, (void *)p); }

    static inline GetType get(lua_State *L, int idx) { return lua_touserdata(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return lua_islightuserdata(L, idx); }
};

// pointer
template <typename T>
struct LuaStack<T *> {
    typedef const T *PushType;
    typedef T *GetType;
    static inline void push(lua_State *L, PushType p) { LuaUserdataPtr::push<T>(L, p); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<T>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// const pointer
template <typename T>
struct LuaStack<const T *> {
    typedef const T *PushType;
    typedef T *GetType;
    static inline void push(lua_State *L, PushType p) { LuaUserdataPtr::push<T>(L, p); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<T>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// pointer &
template <typename T>
struct LuaStack<T *&> {
    typedef const T *PushType;
    typedef T *GetType;
    static inline void push(lua_State *L, PushType p) { LuaUserdataPtr::push<T>(L, p); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<T>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// pointer const &
template <typename T>
struct LuaStack<T *const &> {
    typedef const T *PushType;
    typedef T *GetType;
    static inline void push(lua_State *L, PushType p) { LuaUserdataPtr::push<T>(L, p); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<T>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

// const pointer &
template <typename T>
struct LuaStack<const T *&> {
    typedef const T *PushType;
    typedef T *GetType;
    static inline void push(lua_State *L, PushType p) { LuaUserdataPtr::push<T>(L, p); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<T>(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<T>::is_class_type(L, idx); }
};

template <typename T>
struct LuaStack<const T &> {
    typedef LuaStackEnumHelper<const T &, std::is_enum<T>::value> HelperTraits;
    typedef typename HelperTraits::PushType PushType;
    typedef typename HelperTraits::GetType GetType;
    static inline void push(lua_State *L, PushType t) { HelperTraits::push(L, t); }

    static inline GetType get(lua_State *L, int idx) { return HelperTraits::get(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return HelperTraits::valid(L, idx); }
};

template <typename T>
struct LuaStack<T &> {
    typedef LuaStackEnumHelper<T &, std::is_enum<T>::value> HelperTraits;
    typedef typename HelperTraits::PushType PushType;
    typedef typename HelperTraits::GetType GetType;
    static inline void push(lua_State *L, PushType t) { HelperTraits::push(L, t); }

    static inline GetType get(lua_State *L, int idx) { return HelperTraits::get(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return HelperTraits::valid(L, idx); }
};

template <typename T>
struct LuaStack<const T> {
    typedef LuaStackEnumHelper<const T, std::is_enum<T>::value> HelperTraits;
    typedef typename HelperTraits::PushType PushType;
    typedef typename HelperTraits::GetType GetType;
    static inline void push(lua_State *L, PushType t) { HelperTraits::push(L, t); }

    static inline void push(lua_State *L, T &&t) { HelperTraits::push(L, std::move(t)); }

    static inline GetType get(lua_State *L, int idx) { return HelperTraits::get(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return HelperTraits::valid(L, idx); }
};

template <typename T>
struct LuaStack {
    typedef LuaStackEnumHelper<T, std::is_enum<T>::value> HelperTraits;
    typedef typename HelperTraits::PushType PushType;
    typedef typename HelperTraits::GetType GetType;
    static inline void push(lua_State *L, PushType t) { HelperTraits::push(L, t); }

    static inline void push(lua_State *L, T &&t) { HelperTraits::push(L, std::move(t)); }

    static inline GetType get(lua_State *L, int idx) { return HelperTraits::get(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return HelperTraits::valid(L, idx); }
};

// const T[size]
template <typename T, size_t size>
struct LuaStack<const T[size]> {
    typedef const T PushType[size];
    typedef const T *GetType;
    static inline void push(lua_State *L, PushType value) {
        PureCore::ArrayRef<T> av(const_cast<T *>(value), size);
        LuaStack<PureCore::ArrayRef<T>>::push(L, av);
    }

    static inline GetType get(lua_State *L, int idx) {
        PureCore::ArrayRef<T> av = LuaStack<PureCore::ArrayRef<T>>::get(L, idx);
        assert(av.size() == size);
        return av.mPtr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<PureCore::ArrayRef<T>>::is_class_type(L, idx); }
};

// T[size]
template <typename T, size_t size>
struct LuaStack<T[size]> {
    typedef T PushType[size];
    typedef T *GetType;
    static inline void push(lua_State *L, PushType value) {
        PureCore::ArrayRef<T> av(value, size);
        LuaStack<PureCore::ArrayRef<T>>::push(L, av);
    }

    static inline GetType get(lua_State *L, int idx) {
        PureCore::ArrayRef<T> av = LuaStack<PureCore::ArrayRef<T>>::get(L, idx);
        assert(av.size() == size);
        return av.mPtr;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<PureCore::ArrayRef<T>>::is_class_type(L, idx); }
};

}  // namespace PureLua
