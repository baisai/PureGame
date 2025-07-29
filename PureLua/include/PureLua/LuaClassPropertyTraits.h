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

#include "PureLua/LuaMetatableTraits.h"
#include "PureLua/LuaStackExt.h"

namespace PureLua {
////////////////////////////////////////////////////////////////////////////////////////////////////////
// LuaClassPropertyTraits
////////////////////////////////////////////////////////////////////////////////////////////////////////
class LuaClassPropertyInfo {
public:
    typedef int (*IndexFunction)(lua_State *, void *, const std::string &);
    typedef int (*NewindexFunction)(lua_State *, void *, const std::string &, int);
    LuaClassPropertyInfo(PureCore::StringRef name, IndexFunction i, NewindexFunction ni, void *p)
        : mName(name.data(), name.size()), mIndexFunction(i), mNewindexFunction(ni), mProperty(p) {}

    LuaClassPropertyInfo(PureCore::StringRef name, IndexFunction i, const void *p)
        : mName(name.data(), name.size()), mIndexFunction(i), mNewindexFunction(nullptr), mProperty(const_cast<void *>(p)) {}

    virtual ~LuaClassPropertyInfo() {}

    inline bool is_const() const { return mNewindexFunction == nullptr; }

    inline int call_index(lua_State *L) { return mIndexFunction(L, mProperty, mName); }
    inline int call_newindex(lua_State *L, int idx) { return mNewindexFunction(L, mProperty, mName, idx); }

private:
    std::string mName;
    IndexFunction mIndexFunction;
    NewindexFunction mNewindexFunction;
    void *mProperty;
};
template <typename T>
class LuaClassPropertyInfoImp : public LuaClassPropertyInfo {
public:
    LuaClassPropertyInfoImp(PureCore::StringRef name, IndexFunction i, NewindexFunction ni, void *p) : LuaClassPropertyInfo(name, i, ni, p), mImp(nullptr) {}

    LuaClassPropertyInfoImp(PureCore::StringRef name, IndexFunction i, void *p) : LuaClassPropertyInfo(name, i, p), mImp(nullptr) {}

    virtual ~LuaClassPropertyInfoImp() {}

    inline const T &get_imp() const { return mImp; }
    inline void set_imp(const T &imp) { mImp = imp; }

private:
    T mImp;
};

template <typename T, typename ClassType>
struct LuaClassPropertyTraits;

template <typename T, typename ClassType>
struct LuaClassPropertyTraits {
    typedef T PropertyType;
    typedef T(ClassType::*PropertyTypePtr);

    static int pt_index_function(lua_State *L, void *field, const std::string &key) {
        ClassType *objPtr = LuaMetatableTraits<ClassType>::userdata_to_object(L, 1);
        if (objPtr == nullptr) {
            PureError("Error!!!this field \"{}\" first arg is not a object!!!", key);
            return 0;
        }
        LuaClassPropertyInfoImp<PropertyTypePtr> *lcpii = (LuaClassPropertyInfoImp<PropertyTypePtr> *)field;
        PropertyTypePtr ptr = lcpii->get_imp();
        if (ptr) {
            LuaStack<typename LuaStack<PropertyType>::PushType>::push(L, (objPtr->*ptr));
            return 1;
        } else {
            PureError("Error!!!Not this field!!!\"{}\"", key);
        }
        return 0;
    }

    static int pt_newindex_function(lua_State *L, void *field, const std::string &key, int idx) {
        ClassType *objPtr = LuaMetatableTraits<ClassType>::userdata_to_object(L, 1);
        if (objPtr == nullptr) {
            PureError("Error!!!this field \"{}\" first arg is not a object!!!", key);
            return 0;
        }
        LuaClassPropertyInfoImp<PropertyTypePtr> *lcpii = (LuaClassPropertyInfoImp<PropertyTypePtr> *)field;
        PropertyTypePtr ptr = lcpii->get_imp();
        if (ptr) {
            (objPtr->*ptr) = LuaStack<PropertyType>::get(L, idx);
        } else {
            PureLuaErrorJump(L, "Error!!!Not this field!!!\"{}\"", key);
        }
        return 0;
    }
};

template <typename T>
struct LuaClassPropertyTraits<T, void> {
    typedef T PropertyType;

    static int pt_index_function(lua_State *L, void *field, const std::string &key) {
        PropertyType *ptr = (PropertyType *)field;
        if (ptr) {
            LuaStack<typename LuaStack<PropertyType>::PushType>::push(L, *ptr);
            return 1;
        } else {
            PureError("Error!!!Not this field!!!\"{}\"", key);
        }
        return 0;
    }

    static int pt_newindex_function(lua_State *L, void *field, const std::string &key, int idx) {
        PropertyType *ptr = (PropertyType *)field;
        if (ptr) {
            *ptr = LuaStack<PropertyType>::get(L, idx);
        } else {
            PureLuaErrorJump(L, "Error!!!Not this field!!!\"{}\"", key);
        }
        return 0;
    }
};

template <typename T, size_t size, typename ClassType>
struct LuaClassPropertyArrayTraits;

template <typename T, size_t size, typename ClassType>
struct LuaClassPropertyArrayTraits {
    typedef T PropertyType;
    typedef T(ClassType::*PropertyTypePtr);

    static int pt_index_function(lua_State *L, void *field, const std::string &key) {
        typedef T(ClassType::*PropertyTypeArrayPtr)[size];
        ClassType *objPtr = LuaMetatableTraits<ClassType>::userdata_to_object(L, 1);
        if (objPtr == nullptr) {
            PureError("Error!!!this field \"{}\" first arg is not a object!!!", key);
            return 0;
        }
        LuaClassPropertyInfoImp<PropertyTypeArrayPtr> *lcpii = (LuaClassPropertyInfoImp<PropertyTypeArrayPtr> *)field;
        PropertyTypeArrayPtr ptr = lcpii->get_imp();
        if (ptr) {
            LuaStack<typename LuaStack<PropertyType[size]>::PushType>::push(L, (objPtr->*ptr));
            return 1;
        } else {
            PureError("Error!!!Not this field!!!\"{}\"", key);
        }
        return 0;
    }
};

template <typename T, size_t size>
struct LuaClassPropertyArrayTraits<T, size, void> {
    typedef T PropertyType;
    static int pt_index_function(lua_State *L, void *field, const std::string &key) {
        PropertyType *ptr = (PropertyType *)field;
        if (ptr) {
            LuaStack<typename LuaStack<PropertyType[size]>::PushType>::push(L, ptr);
            return 1;
        } else {
            PureError("Error!!!Not this field!!!\"{}\"", key);
        }
        return 0;
    }
};

}  // namespace PureLua
