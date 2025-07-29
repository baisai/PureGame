// Copyright 2023-present ChenDong, email <baisaichen@live.com>, All rights reserved.
// this is a generate file, do't modify

#pragma once

#include "PureCore/ArrayRef.h"
#include "PureLua/LuaUserdata.h"
#include "PureLua/LuaStackStdFunc.h"
#include "PureLua/LuaSTLAdapter.h"

#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <memory>

namespace PureLua {
// const std::shared_ptr<T> *
template <typename T>
struct LuaStack<const std::shared_ptr<T> *> {
    typedef const std::shared_ptr<T> *PushType;
    typedef std::shared_ptr<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::shared_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::shared_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::shared_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> LuaStack<const std::shared_ptr<T> *>::__;

// std::shared_ptr<T> *
template <typename T>
struct LuaStack<std::shared_ptr<T> *> {
    typedef const std::shared_ptr<T> *PushType;
    typedef std::shared_ptr<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::shared_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::shared_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::shared_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> LuaStack<std::shared_ptr<T> *>::__;

// const std::shared_ptr<T> * &
template <typename T>
struct LuaStack<const std::shared_ptr<T> *&> {
    typedef const std::shared_ptr<T> *PushType;
    typedef std::shared_ptr<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::shared_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::shared_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::shared_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> LuaStack<const std::shared_ptr<T> *&>::__;

// std::shared_ptr<T> * &
template <typename T>
struct LuaStack<std::shared_ptr<T> *&> {
    typedef const std::shared_ptr<T> *PushType;
    typedef std::shared_ptr<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::shared_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::shared_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::shared_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> LuaStack<std::shared_ptr<T> *&>::__;

// std::shared_ptr<T> * const &
template <typename T>
struct LuaStack<std::shared_ptr<T> *const &> {
    typedef const std::shared_ptr<T> *PushType;
    typedef std::shared_ptr<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::shared_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::shared_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::shared_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> LuaStack<std::shared_ptr<T> *const &>::__;

// const std::shared_ptr<T> &
template <typename T>
struct LuaStack<const std::shared_ptr<T> &> {
    typedef const std::shared_ptr<T> &PushType;
    typedef std::shared_ptr<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::shared_ptr<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::shared_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::shared_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> LuaStack<const std::shared_ptr<T> &>::__;

// std::shared_ptr<T> &
template <typename T>
struct LuaStack<std::shared_ptr<T> &> {
    typedef const std::shared_ptr<T> &PushType;
    typedef std::shared_ptr<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::shared_ptr<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::shared_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::shared_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> LuaStack<std::shared_ptr<T> &>::__;

// const std::shared_ptr<T>
template <typename T>
struct LuaStack<const std::shared_ptr<T>> {
    typedef const std::shared_ptr<T> &PushType;
    typedef const std::shared_ptr<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::shared_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::shared_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::shared_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> LuaStack<const std::shared_ptr<T>>::__;

// std::shared_ptr<T>
template <typename T>
struct LuaStack<std::shared_ptr<T>> {
    typedef const std::shared_ptr<T> &PushType;
    typedef const std::shared_ptr<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::shared_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::shared_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::shared_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSharedPtrAdapter<std::shared_ptr<T>>> LuaStack<std::shared_ptr<T>>::__;

// const std::weak_ptr<T> *
template <typename T>
struct LuaStack<const std::weak_ptr<T> *> {
    typedef const std::weak_ptr<T> *PushType;
    typedef std::weak_ptr<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::weak_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::weak_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::weak_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> LuaStack<const std::weak_ptr<T> *>::__;

// std::weak_ptr<T> *
template <typename T>
struct LuaStack<std::weak_ptr<T> *> {
    typedef const std::weak_ptr<T> *PushType;
    typedef std::weak_ptr<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::weak_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::weak_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::weak_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> LuaStack<std::weak_ptr<T> *>::__;

// const std::weak_ptr<T> * &
template <typename T>
struct LuaStack<const std::weak_ptr<T> *&> {
    typedef const std::weak_ptr<T> *PushType;
    typedef std::weak_ptr<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::weak_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::weak_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::weak_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> LuaStack<const std::weak_ptr<T> *&>::__;

// std::weak_ptr<T> * &
template <typename T>
struct LuaStack<std::weak_ptr<T> *&> {
    typedef const std::weak_ptr<T> *PushType;
    typedef std::weak_ptr<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::weak_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::weak_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::weak_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> LuaStack<std::weak_ptr<T> *&>::__;

// std::weak_ptr<T> * const &
template <typename T>
struct LuaStack<std::weak_ptr<T> *const &> {
    typedef const std::weak_ptr<T> *PushType;
    typedef std::weak_ptr<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::weak_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::weak_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::weak_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> LuaStack<std::weak_ptr<T> *const &>::__;

// const std::weak_ptr<T> &
template <typename T>
struct LuaStack<const std::weak_ptr<T> &> {
    typedef const std::weak_ptr<T> &PushType;
    typedef std::weak_ptr<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::weak_ptr<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::weak_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::weak_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> LuaStack<const std::weak_ptr<T> &>::__;

// std::weak_ptr<T> &
template <typename T>
struct LuaStack<std::weak_ptr<T> &> {
    typedef const std::weak_ptr<T> &PushType;
    typedef std::weak_ptr<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::weak_ptr<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::weak_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::weak_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> LuaStack<std::weak_ptr<T> &>::__;

// const std::weak_ptr<T>
template <typename T>
struct LuaStack<const std::weak_ptr<T>> {
    typedef const std::weak_ptr<T> &PushType;
    typedef const std::weak_ptr<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::weak_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::weak_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::weak_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> LuaStack<const std::weak_ptr<T>>::__;

// std::weak_ptr<T>
template <typename T>
struct LuaStack<std::weak_ptr<T>> {
    typedef const std::weak_ptr<T> &PushType;
    typedef const std::weak_ptr<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::weak_ptr<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::weak_ptr<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::weak_ptr<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaWeakPtrAdapter<std::weak_ptr<T>>> LuaStack<std::weak_ptr<T>>::__;

// const std::vector<T> *
template <typename T>
struct LuaStack<const std::vector<T> *> {
    typedef const std::vector<T> *PushType;
    typedef std::vector<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::vector<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::vector<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::vector<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> LuaStack<const std::vector<T> *>::__;

// std::vector<T> *
template <typename T>
struct LuaStack<std::vector<T> *> {
    typedef const std::vector<T> *PushType;
    typedef std::vector<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::vector<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::vector<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::vector<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> LuaStack<std::vector<T> *>::__;

// const std::vector<T> * &
template <typename T>
struct LuaStack<const std::vector<T> *&> {
    typedef const std::vector<T> *PushType;
    typedef std::vector<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::vector<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::vector<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::vector<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> LuaStack<const std::vector<T> *&>::__;

// std::vector<T> * &
template <typename T>
struct LuaStack<std::vector<T> *&> {
    typedef const std::vector<T> *PushType;
    typedef std::vector<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::vector<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::vector<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::vector<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> LuaStack<std::vector<T> *&>::__;

// std::vector<T> * const &
template <typename T>
struct LuaStack<std::vector<T> *const &> {
    typedef const std::vector<T> *PushType;
    typedef std::vector<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::vector<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::vector<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::vector<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> LuaStack<std::vector<T> *const &>::__;

// const std::vector<T> &
template <typename T>
struct LuaStack<const std::vector<T> &> {
    typedef const std::vector<T> &PushType;
    typedef std::vector<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::vector<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::vector<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::vector<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> LuaStack<const std::vector<T> &>::__;

// std::vector<T> &
template <typename T>
struct LuaStack<std::vector<T> &> {
    typedef const std::vector<T> &PushType;
    typedef std::vector<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::vector<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::vector<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::vector<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> LuaStack<std::vector<T> &>::__;

// const std::vector<T>
template <typename T>
struct LuaStack<const std::vector<T>> {
    typedef const std::vector<T> &PushType;
    typedef const std::vector<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::vector<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::vector<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::vector<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> LuaStack<const std::vector<T>>::__;

// std::vector<T>
template <typename T>
struct LuaStack<std::vector<T>> {
    typedef const std::vector<T> &PushType;
    typedef const std::vector<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::vector<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::vector<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::vector<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaVectorAdapter<std::vector<T>>> LuaStack<std::vector<T>>::__;

// const std::list<T> *
template <typename T>
struct LuaStack<const std::list<T> *> {
    typedef const std::list<T> *PushType;
    typedef std::list<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::list<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::list<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::list<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaListAdapter<std::list<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaListAdapter<std::list<T>>> LuaStack<const std::list<T> *>::__;

// std::list<T> *
template <typename T>
struct LuaStack<std::list<T> *> {
    typedef const std::list<T> *PushType;
    typedef std::list<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::list<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::list<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::list<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaListAdapter<std::list<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaListAdapter<std::list<T>>> LuaStack<std::list<T> *>::__;

// const std::list<T> * &
template <typename T>
struct LuaStack<const std::list<T> *&> {
    typedef const std::list<T> *PushType;
    typedef std::list<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::list<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::list<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::list<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaListAdapter<std::list<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaListAdapter<std::list<T>>> LuaStack<const std::list<T> *&>::__;

// std::list<T> * &
template <typename T>
struct LuaStack<std::list<T> *&> {
    typedef const std::list<T> *PushType;
    typedef std::list<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::list<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::list<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::list<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaListAdapter<std::list<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaListAdapter<std::list<T>>> LuaStack<std::list<T> *&>::__;

// std::list<T> * const &
template <typename T>
struct LuaStack<std::list<T> *const &> {
    typedef const std::list<T> *PushType;
    typedef std::list<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::list<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::list<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::list<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaListAdapter<std::list<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaListAdapter<std::list<T>>> LuaStack<std::list<T> *const &>::__;

// const std::list<T> &
template <typename T>
struct LuaStack<const std::list<T> &> {
    typedef const std::list<T> &PushType;
    typedef std::list<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::list<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::list<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::list<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaListAdapter<std::list<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaListAdapter<std::list<T>>> LuaStack<const std::list<T> &>::__;

// std::list<T> &
template <typename T>
struct LuaStack<std::list<T> &> {
    typedef const std::list<T> &PushType;
    typedef std::list<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::list<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::list<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::list<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaListAdapter<std::list<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaListAdapter<std::list<T>>> LuaStack<std::list<T> &>::__;

// const std::list<T>
template <typename T>
struct LuaStack<const std::list<T>> {
    typedef const std::list<T> &PushType;
    typedef const std::list<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::list<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::list<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::list<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaListAdapter<std::list<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaListAdapter<std::list<T>>> LuaStack<const std::list<T>>::__;

// std::list<T>
template <typename T>
struct LuaStack<std::list<T>> {
    typedef const std::list<T> &PushType;
    typedef const std::list<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::list<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::list<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::list<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaListAdapter<std::list<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaListAdapter<std::list<T>>> LuaStack<std::list<T>>::__;

// const std::set<T> *
template <typename T>
struct LuaStack<const std::set<T> *> {
    typedef const std::set<T> *PushType;
    typedef std::set<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::set<T>>> LuaStack<const std::set<T> *>::__;

// std::set<T> *
template <typename T>
struct LuaStack<std::set<T> *> {
    typedef const std::set<T> *PushType;
    typedef std::set<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::set<T>>> LuaStack<std::set<T> *>::__;

// const std::set<T> * &
template <typename T>
struct LuaStack<const std::set<T> *&> {
    typedef const std::set<T> *PushType;
    typedef std::set<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::set<T>>> LuaStack<const std::set<T> *&>::__;

// std::set<T> * &
template <typename T>
struct LuaStack<std::set<T> *&> {
    typedef const std::set<T> *PushType;
    typedef std::set<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::set<T>>> LuaStack<std::set<T> *&>::__;

// std::set<T> * const &
template <typename T>
struct LuaStack<std::set<T> *const &> {
    typedef const std::set<T> *PushType;
    typedef std::set<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::set<T>>> LuaStack<std::set<T> *const &>::__;

// const std::set<T> &
template <typename T>
struct LuaStack<const std::set<T> &> {
    typedef const std::set<T> &PushType;
    typedef std::set<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::set<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::set<T>>> LuaStack<const std::set<T> &>::__;

// std::set<T> &
template <typename T>
struct LuaStack<std::set<T> &> {
    typedef const std::set<T> &PushType;
    typedef std::set<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::set<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::set<T>>> LuaStack<std::set<T> &>::__;

// const std::set<T>
template <typename T>
struct LuaStack<const std::set<T>> {
    typedef const std::set<T> &PushType;
    typedef const std::set<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::set<T>>> LuaStack<const std::set<T>>::__;

// std::set<T>
template <typename T>
struct LuaStack<std::set<T>> {
    typedef const std::set<T> &PushType;
    typedef const std::set<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::set<T>>> LuaStack<std::set<T>>::__;

// const std::unordered_set<T> *
template <typename T>
struct LuaStack<const std::unordered_set<T> *> {
    typedef const std::unordered_set<T> *PushType;
    typedef std::unordered_set<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::unordered_set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> LuaStack<const std::unordered_set<T> *>::__;

// std::unordered_set<T> *
template <typename T>
struct LuaStack<std::unordered_set<T> *> {
    typedef const std::unordered_set<T> *PushType;
    typedef std::unordered_set<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::unordered_set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> LuaStack<std::unordered_set<T> *>::__;

// const std::unordered_set<T> * &
template <typename T>
struct LuaStack<const std::unordered_set<T> *&> {
    typedef const std::unordered_set<T> *PushType;
    typedef std::unordered_set<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::unordered_set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> LuaStack<const std::unordered_set<T> *&>::__;

// std::unordered_set<T> * &
template <typename T>
struct LuaStack<std::unordered_set<T> *&> {
    typedef const std::unordered_set<T> *PushType;
    typedef std::unordered_set<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::unordered_set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> LuaStack<std::unordered_set<T> *&>::__;

// std::unordered_set<T> * const &
template <typename T>
struct LuaStack<std::unordered_set<T> *const &> {
    typedef const std::unordered_set<T> *PushType;
    typedef std::unordered_set<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::unordered_set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> LuaStack<std::unordered_set<T> *const &>::__;

// const std::unordered_set<T> &
template <typename T>
struct LuaStack<const std::unordered_set<T> &> {
    typedef const std::unordered_set<T> &PushType;
    typedef std::unordered_set<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_set<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::unordered_set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> LuaStack<const std::unordered_set<T> &>::__;

// std::unordered_set<T> &
template <typename T>
struct LuaStack<std::unordered_set<T> &> {
    typedef const std::unordered_set<T> &PushType;
    typedef std::unordered_set<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_set<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::unordered_set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> LuaStack<std::unordered_set<T> &>::__;

// const std::unordered_set<T>
template <typename T>
struct LuaStack<const std::unordered_set<T>> {
    typedef const std::unordered_set<T> &PushType;
    typedef const std::unordered_set<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::unordered_set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::unordered_set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> LuaStack<const std::unordered_set<T>>::__;

// std::unordered_set<T>
template <typename T>
struct LuaStack<std::unordered_set<T>> {
    typedef const std::unordered_set<T> &PushType;
    typedef const std::unordered_set<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::unordered_set<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::unordered_set<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_set<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaSetAdapter<std::unordered_set<T>>> LuaStack<std::unordered_set<T>>::__;

// const std::map<K, V> *
template <typename K, typename V>
struct LuaStack<const std::map<K, V> *> {
    typedef const std::map<K, V> *PushType;
    typedef std::map<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> LuaStack<const std::map<K, V> *>::__;

// std::map<K, V> *
template <typename K, typename V>
struct LuaStack<std::map<K, V> *> {
    typedef const std::map<K, V> *PushType;
    typedef std::map<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> LuaStack<std::map<K, V> *>::__;

// const std::map<K, V> * &
template <typename K, typename V>
struct LuaStack<const std::map<K, V> *&> {
    typedef const std::map<K, V> *PushType;
    typedef std::map<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> LuaStack<const std::map<K, V> *&>::__;

// std::map<K, V> * &
template <typename K, typename V>
struct LuaStack<std::map<K, V> *&> {
    typedef const std::map<K, V> *PushType;
    typedef std::map<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> LuaStack<std::map<K, V> *&>::__;

// std::map<K, V> * const &
template <typename K, typename V>
struct LuaStack<std::map<K, V> *const &> {
    typedef const std::map<K, V> *PushType;
    typedef std::map<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> LuaStack<std::map<K, V> *const &>::__;

// const std::map<K, V> &
template <typename K, typename V>
struct LuaStack<const std::map<K, V> &> {
    typedef const std::map<K, V> &PushType;
    typedef std::map<K, V> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::map<K, V>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> LuaStack<const std::map<K, V> &>::__;

// std::map<K, V> &
template <typename K, typename V>
struct LuaStack<std::map<K, V> &> {
    typedef const std::map<K, V> &PushType;
    typedef std::map<K, V> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::map<K, V>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> LuaStack<std::map<K, V> &>::__;

// const std::map<K, V>
template <typename K, typename V>
struct LuaStack<const std::map<K, V>> {
    typedef const std::map<K, V> &PushType;
    typedef const std::map<K, V> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> LuaStack<const std::map<K, V>>::__;

// std::map<K, V>
template <typename K, typename V>
struct LuaStack<std::map<K, V>> {
    typedef const std::map<K, V> &PushType;
    typedef const std::map<K, V> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::map<K, V>>> LuaStack<std::map<K, V>>::__;

// const std::pair<K, V> *
template <typename K, typename V>
struct LuaStack<const std::pair<K, V> *> {
    typedef const std::pair<K, V> *PushType;
    typedef std::pair<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::pair<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::pair<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::pair<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> LuaStack<const std::pair<K, V> *>::__;

// std::pair<K, V> *
template <typename K, typename V>
struct LuaStack<std::pair<K, V> *> {
    typedef const std::pair<K, V> *PushType;
    typedef std::pair<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::pair<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::pair<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::pair<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> LuaStack<std::pair<K, V> *>::__;

// const std::pair<K, V> * &
template <typename K, typename V>
struct LuaStack<const std::pair<K, V> *&> {
    typedef const std::pair<K, V> *PushType;
    typedef std::pair<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::pair<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::pair<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::pair<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> LuaStack<const std::pair<K, V> *&>::__;

// std::pair<K, V> * &
template <typename K, typename V>
struct LuaStack<std::pair<K, V> *&> {
    typedef const std::pair<K, V> *PushType;
    typedef std::pair<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::pair<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::pair<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::pair<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> LuaStack<std::pair<K, V> *&>::__;

// std::pair<K, V> * const &
template <typename K, typename V>
struct LuaStack<std::pair<K, V> *const &> {
    typedef const std::pair<K, V> *PushType;
    typedef std::pair<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::pair<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::pair<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::pair<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> LuaStack<std::pair<K, V> *const &>::__;

// const std::pair<K, V> &
template <typename K, typename V>
struct LuaStack<const std::pair<K, V> &> {
    typedef const std::pair<K, V> &PushType;
    typedef std::pair<K, V> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::pair<K, V>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::pair<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::pair<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> LuaStack<const std::pair<K, V> &>::__;

// std::pair<K, V> &
template <typename K, typename V>
struct LuaStack<std::pair<K, V> &> {
    typedef const std::pair<K, V> &PushType;
    typedef std::pair<K, V> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::pair<K, V>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::pair<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::pair<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> LuaStack<std::pair<K, V> &>::__;

// const std::pair<K, V>
template <typename K, typename V>
struct LuaStack<const std::pair<K, V>> {
    typedef const std::pair<K, V> &PushType;
    typedef const std::pair<K, V> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::pair<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::pair<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::pair<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> LuaStack<const std::pair<K, V>>::__;

// std::pair<K, V>
template <typename K, typename V>
struct LuaStack<std::pair<K, V>> {
    typedef const std::pair<K, V> &PushType;
    typedef const std::pair<K, V> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::pair<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::pair<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::pair<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaPairAdapter<std::pair<K, V>>> LuaStack<std::pair<K, V>>::__;

// const std::unordered_map<K, V> *
template <typename K, typename V>
struct LuaStack<const std::unordered_map<K, V> *> {
    typedef const std::unordered_map<K, V> *PushType;
    typedef std::unordered_map<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::unordered_map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> LuaStack<const std::unordered_map<K, V> *>::__;

// std::unordered_map<K, V> *
template <typename K, typename V>
struct LuaStack<std::unordered_map<K, V> *> {
    typedef const std::unordered_map<K, V> *PushType;
    typedef std::unordered_map<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::unordered_map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> LuaStack<std::unordered_map<K, V> *>::__;

// const std::unordered_map<K, V> * &
template <typename K, typename V>
struct LuaStack<const std::unordered_map<K, V> *&> {
    typedef const std::unordered_map<K, V> *PushType;
    typedef std::unordered_map<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::unordered_map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> LuaStack<const std::unordered_map<K, V> *&>::__;

// std::unordered_map<K, V> * &
template <typename K, typename V>
struct LuaStack<std::unordered_map<K, V> *&> {
    typedef const std::unordered_map<K, V> *PushType;
    typedef std::unordered_map<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::unordered_map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> LuaStack<std::unordered_map<K, V> *&>::__;

// std::unordered_map<K, V> * const &
template <typename K, typename V>
struct LuaStack<std::unordered_map<K, V> *const &> {
    typedef const std::unordered_map<K, V> *PushType;
    typedef std::unordered_map<K, V> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<std::unordered_map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> LuaStack<std::unordered_map<K, V> *const &>::__;

// const std::unordered_map<K, V> &
template <typename K, typename V>
struct LuaStack<const std::unordered_map<K, V> &> {
    typedef const std::unordered_map<K, V> &PushType;
    typedef std::unordered_map<K, V> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_map<K, V>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::unordered_map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> LuaStack<const std::unordered_map<K, V> &>::__;

// std::unordered_map<K, V> &
template <typename K, typename V>
struct LuaStack<std::unordered_map<K, V> &> {
    typedef const std::unordered_map<K, V> &PushType;
    typedef std::unordered_map<K, V> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<std::unordered_map<K, V>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<std::unordered_map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> LuaStack<std::unordered_map<K, V> &>::__;

// const std::unordered_map<K, V>
template <typename K, typename V>
struct LuaStack<const std::unordered_map<K, V>> {
    typedef const std::unordered_map<K, V> &PushType;
    typedef const std::unordered_map<K, V> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::unordered_map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::unordered_map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> LuaStack<const std::unordered_map<K, V>>::__;

// std::unordered_map<K, V>
template <typename K, typename V>
struct LuaStack<std::unordered_map<K, V>> {
    typedef const std::unordered_map<K, V> &PushType;
    typedef const std::unordered_map<K, V> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<std::unordered_map<K, V>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<std::unordered_map<K, V>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<std::unordered_map<K, V>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> __;
};
template <typename K, typename V>
LuaSTLHelper<LuaMapAdapter<std::unordered_map<K, V>>> LuaStack<std::unordered_map<K, V>>::__;

// const PureCore::ArrayRef<T> *
template <typename T>
struct LuaStack<const PureCore::ArrayRef<T> *> {
    typedef const PureCore::ArrayRef<T> *PushType;
    typedef PureCore::ArrayRef<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<PureCore::ArrayRef<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<PureCore::ArrayRef<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<PureCore::ArrayRef<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> LuaStack<const PureCore::ArrayRef<T> *>::__;

// PureCore::ArrayRef<T> *
template <typename T>
struct LuaStack<PureCore::ArrayRef<T> *> {
    typedef const PureCore::ArrayRef<T> *PushType;
    typedef PureCore::ArrayRef<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<PureCore::ArrayRef<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<PureCore::ArrayRef<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<PureCore::ArrayRef<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> LuaStack<PureCore::ArrayRef<T> *>::__;

// const PureCore::ArrayRef<T> * &
template <typename T>
struct LuaStack<const PureCore::ArrayRef<T> *&> {
    typedef const PureCore::ArrayRef<T> *PushType;
    typedef PureCore::ArrayRef<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<PureCore::ArrayRef<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<PureCore::ArrayRef<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<PureCore::ArrayRef<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> LuaStack<const PureCore::ArrayRef<T> *&>::__;

// PureCore::ArrayRef<T> * &
template <typename T>
struct LuaStack<PureCore::ArrayRef<T> *&> {
    typedef const PureCore::ArrayRef<T> *PushType;
    typedef PureCore::ArrayRef<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<PureCore::ArrayRef<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<PureCore::ArrayRef<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<PureCore::ArrayRef<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> LuaStack<PureCore::ArrayRef<T> *&>::__;

// PureCore::ArrayRef<T> * const &
template <typename T>
struct LuaStack<PureCore::ArrayRef<T> *const &> {
    typedef const PureCore::ArrayRef<T> *PushType;
    typedef PureCore::ArrayRef<T> *GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<PureCore::ArrayRef<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdataPtr::get<PureCore::ArrayRef<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<PureCore::ArrayRef<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> LuaStack<PureCore::ArrayRef<T> *const &>::__;

// const PureCore::ArrayRef<T> &
template <typename T>
struct LuaStack<const PureCore::ArrayRef<T> &> {
    typedef const PureCore::ArrayRef<T> &PushType;
    typedef PureCore::ArrayRef<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<PureCore::ArrayRef<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<PureCore::ArrayRef<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<PureCore::ArrayRef<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> LuaStack<const PureCore::ArrayRef<T> &>::__;

// PureCore::ArrayRef<T> &
template <typename T>
struct LuaStack<PureCore::ArrayRef<T> &> {
    typedef const PureCore::ArrayRef<T> &PushType;
    typedef PureCore::ArrayRef<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdataPtr::push<PureCore::ArrayRef<T>>(L, &value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return *LuaUserdataPtr::get<PureCore::ArrayRef<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<PureCore::ArrayRef<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> LuaStack<PureCore::ArrayRef<T> &>::__;

// const PureCore::ArrayRef<T>
template <typename T>
struct LuaStack<const PureCore::ArrayRef<T>> {
    typedef const PureCore::ArrayRef<T> &PushType;
    typedef const PureCore::ArrayRef<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<PureCore::ArrayRef<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<PureCore::ArrayRef<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<PureCore::ArrayRef<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> LuaStack<const PureCore::ArrayRef<T>>::__;

// PureCore::ArrayRef<T>
template <typename T>
struct LuaStack<PureCore::ArrayRef<T>> {
    typedef const PureCore::ArrayRef<T> &PushType;
    typedef const PureCore::ArrayRef<T> &GetType;
    static inline void push(lua_State *L, PushType value) {
        (void)__;
        LuaUserdata::push<PureCore::ArrayRef<T>>(L, value);
    }

    static inline GetType get(lua_State *L, int idx) {
        (void)__;
        return LuaUserdata::get<PureCore::ArrayRef<T>>(L, idx);
    }

    static inline bool valid(lua_State *L, int idx) {
        (void)__;
        return LuaMetatableTraits<PureCore::ArrayRef<T>>::is_class_type(L, idx);
    }

    static LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> __;
};
template <typename T>
LuaSTLHelper<LuaArrayRefAdapter<PureCore::ArrayRef<T>>> LuaStack<PureCore::ArrayRef<T>>::__;

}  // namespace PureLua