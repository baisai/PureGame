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

#include "PureLua/LuaRegisterClass.h"
#include "PureLua/LuaClassInfo.h"
#include "PureLua/LuaClassMethodTraits.h"
#include "PureLua/LuaClassMoveReturnTraits.h"
#include "PureLua/LuaClassCtorTraits.h"
#include "PureLua/LuaMetatableTraits.h"
#include "PureLua/LuaMetatable.h"
#include "PureLua/LuaDefTraits.h"
#include "PureLua/LuaStack.h"

namespace PureLua {
template <typename ClassType>
LuaRegisterClass<ClassType>::LuaRegisterClass(lua_State *L, PureCore::StringRef name) : LuaRegisterNode(L) {
    init_class(name);
    LuaClassInfo<ClassType>::template register_base_class<ClassType>(L);
    // pop metatable, class table, parent class table
}

template <typename ClassType>
template <typename... Args>
LuaRegisterClass<ClassType>::LuaRegisterClass(lua_State *L, PureCore::StringRef name, BaseClassStrategy<Args...>) : LuaRegisterNode(L) {
    init_class(name);
    (add_base_class<Args>(), ...);
}

template <typename ClassType>
LuaRegisterClass<ClassType>::LuaRegisterClass(const LuaRegisterClass<ClassType> &cp) : LuaRegisterNode(cp) {}

template <typename ClassType>
template <typename T>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::operator=(const LuaRegisterClass<T> &right) {
    LuaRegisterNode::operator=(static_cast<const LuaRegisterNode &>(right));
    return *this;
}

template <typename ClassType>
template <typename... Args>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::default_ctor() {
    LuaStateProtect lsp(this->mLua);
    if (!this->get_class_table()) {
        PureError(get_error_desc(ErrorClassNotRegister));
        return *this;
    }

    using NewerType = std::function<void(Args...)>;
    using DelerType = std::function<void(ClassType *)>;
    lua_class_set_newer<NewerType>(this->mLua, -1, nullptr);
    lua_class_set_deler<DelerType>(this->mLua, -1, nullptr);

    lua_pushvalue(this->mLua, -1);
    lua_pushcclosure(this->mLua, LuaClassDefaultCtorTraits<ClassType, true, NewerType>::call, 1);
    raw_setfield(this->mLua, -2, "ctor__");

    lua_getmetatable(this->mLua, -1);
    lua_pushvalue(this->mLua, -2);
    lua_pushcclosure(this->mLua, LuaClassDefaultCreatorTraits<ClassType, true, NewerType>::call, 1);
    raw_setfield(this->mLua, -2, "__call");
    return *this;
}

template <typename ClassType>
template <typename R, typename... Args>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_ctor(R (*newer)(Args...), void (*deler)(ClassType *)) {
    LuaStateProtect lsp(this->mLua);
    if (!this->get_class_table()) {
        PureError(get_error_desc(ErrorClassNotRegister));
        return *this;
    }

    using NewerType = std::function<R(Args...)>;
    using DelerType = std::function<void(ClassType *)>;
    lua_class_set_newer<NewerType>(this->mLua, -1, NewerType(newer));
    lua_class_set_deler<DelerType>(this->mLua, -1, DelerType(deler));

    lua_pushvalue(this->mLua, -1);
    lua_pushcclosure(this->mLua, LuaClassCtorTraits<ClassType, true, NewerType>::call, 1);
    raw_setfield(this->mLua, -2, "ctor__");

    lua_getmetatable(this->mLua, -1);
    lua_pushcclosure(this->mLua, LuaClassCreatorTraits<ClassType, true, NewerType>::call, 0);
    raw_setfield(this->mLua, -2, "__call");
    return *this;
}

template <typename ClassType>
template <typename LamadaNewer, typename LamadaDeler>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_ctor(LamadaNewer newer, LamadaDeler deler) {
    LuaStateProtect lsp(this->mLua);
    if (!this->get_class_table()) {
        PureError(get_error_desc(ErrorClassNotRegister));
        return *this;
    }

    using NewerType = typename LamadaTraits<LamadaNewer>::function;
    using DelerType = std::function<void(ClassType *)>;
    lua_class_set_newer<NewerType>(this->mLua, -1, newer);
    lua_class_set_deler<DelerType>(this->mLua, -1, DelerType(deler));

    lua_pushvalue(this->mLua, -1);
    lua_pushcclosure(this->mLua, LuaClassCtorTraits<ClassType, true, NewerType>::call, 1);
    raw_setfield(this->mLua, -2, "ctor__");

    lua_getmetatable(this->mLua, -1);
    lua_pushcclosure(this->mLua, LuaClassCreatorTraits<ClassType, true, NewerType>::call, 0);
    raw_setfield(this->mLua, -2, "__call");
    return *this;
}

template <typename ClassType>
template <typename T>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def(T obj, PureCore::StringRef name) {
    LuaDefTraits<T>::def(this, obj, name);
    return *this;
}

template <typename ClassType>
template <typename T, size_t size>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def(T (ClassType::*obj)[size], PureCore::StringRef name) {
    def_array(obj, name);
    return *this;
}

template <typename ClassType>
template <typename T, size_t size>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def(T (&obj)[size], PureCore::StringRef name) {
    def_array(obj, name);
    return *this;
}

template <typename ClassType>
LuaModuleByClass<ClassType> LuaRegisterClass<ClassType>::def_module(PureCore::StringRef name) {
    return (LuaModuleByClass<ClassType>(mLua, name, mValue, *this));
}

template <typename ClassType>
template <typename T, typename KT>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_const(const T &obj, const KT &name) {
    LuaStateProtect lsp(mLua);
    if (!get_class_table()) {
        return *this;
    }
    LuaStack<typename LuaStack<KT>::PushType>::push(mLua, name);
    LuaStack<typename LuaStack<T>::PushType>::push(mLua, obj);
    lua_rawset(mLua, -3);
    return *this;
}

template <typename ClassType>
template <typename T, typename KT>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_const(T &&obj, const KT &name) {
    LuaStateProtect lsp(mLua);
    if (!get_class_table()) {
        return *this;
    }
    LuaStack<typename LuaStack<KT>::PushType>::push(mLua, name);
    LuaStack<T>::push(mLua, std::move(obj));
    lua_rawset(mLua, -3);
    return *this;
}

template <typename ClassType>
template <typename T>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_move_return(T obj, PureCore::StringRef name) {
    LuaStateProtect lsp(mLua);
    if (!get_class_table()) {
        return *this;
    }
    new (lua_newuserdatauv(mLua, sizeof(T), 0)) T(obj);
    lua_pushcclosure(mLua, LuaClassMoveReturnTraits<ClassType, T>::call, 1);
    raw_setfieldlen(mLua, -2, name.data(), name.size());
    return *this;
}

template <typename ClassType>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_class_func(lua_CFunction obj, PureCore::StringRef name) {
    LuaStateProtect lsp(mLua);
    if (!get_class_table()) {
        return *this;
    }
    LuaDefAutoTool::def(mLua, obj, name);
    return *this;
}

template <typename ClassType>
template <typename T>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_class_func(T obj, PureCore::StringRef name) {
    LuaStateProtect lsp(mLua);
    if (!get_class_table()) {
        return *this;
    }
    new (lua_newuserdatauv(mLua, sizeof(T), 0)) T(obj);
    lua_pushcclosure(mLua, LuaClassMethodTraits<ClassType, T>::call, 1);
    raw_setfieldlen(mLua, -2, name.data(), name.size());
    return *this;
}

template <typename ClassType>
template <typename LamadaType>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_lamada(LamadaType lamada, PureCore::StringRef name) {
    auto func = typename LamadaTraits<LamadaType>::cfunction(lamada);
    return def(func, name);
}

template <typename ClassType>
template <typename T>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_property(const T ClassType::*obj, PureCore::StringRef name) {
    typedef LuaClassPropertyInfoImp<T(ClassType::*)> LCPI;
    LuaStateProtect lsp(mLua);
    if (!get_class_table()) {
        return *this;
    }
    LCPI *p = (LCPI *)lua_newuserdatauv(mLua, sizeof(LCPI), 0);
    new (p) LCPI(name, LuaClassPropertyTraits<T, ClassType>::pt_index_function, p);
    p->set_imp(obj);
    LuaMetatable::push_property_function(mLua, -1, false);
    raw_setfieldlen(mLua, -3, name.data(), name.size());
    return *this;
}

template <typename ClassType>
template <typename T>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_property(T ClassType::*obj, PureCore::StringRef name) {
    typedef LuaClassPropertyInfoImp<T(ClassType::*)> LCPI;
    LuaStateProtect lsp(mLua);
    if (!get_class_table()) {
        return *this;
    }
    LCPI *p = (LCPI *)lua_newuserdatauv(mLua, sizeof(LCPI), 0);
    new (p) LCPI(name, LuaClassPropertyTraits<T, ClassType>::pt_index_function, LuaClassPropertyTraits<T, ClassType>::pt_newindex_function, p);
    p->set_imp(obj);
    LuaMetatable::push_property_function(mLua, -1, false);
    raw_setfieldlen(mLua, -3, name.data(), name.size());
    return *this;
}

template <typename ClassType>
template <typename T>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_property(const T &obj, PureCore::StringRef name) {
    return def_property(&obj, name);
}

template <typename ClassType>
template <typename T>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_property(T &obj, PureCore::StringRef name) {
    return def_property(&obj, name);
}

template <typename ClassType>
template <typename T>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_property(const T *obj, PureCore::StringRef name) {
    LuaStateProtect lsp(mLua);
    if (!get_class_table()) {
        return *this;
    }
    new (lua_newuserdatauv(mLua, sizeof(LuaClassPropertyInfo), 0)) LuaClassPropertyInfo(name, LuaClassPropertyTraits<T, void>::pt_index_function, obj);
    LuaMetatable::push_property_function(mLua, -1, false);
    raw_setfieldlen(mLua, -3, name.data(), name.size());
    return *this;
}

template <typename ClassType>
template <typename T>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_property(T *obj, PureCore::StringRef name) {
    LuaStateProtect lsp(mLua);
    if (!get_class_table()) {
        return *this;
    }
    new (lua_newuserdatauv(mLua, sizeof(LuaClassPropertyInfo), 0))
        LuaClassPropertyInfo(name, LuaClassPropertyTraits<T, void>::pt_index_function, LuaClassPropertyTraits<T, void>::pt_newindex_function, obj);
    LuaMetatable::push_property_function(mLua, -1, false);
    raw_setfieldlen(mLua, -3, name.data(), name.size());
    return *this;
}

template <typename ClassType>
template <typename T, size_t size>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_array(T (ClassType::*obj)[size], PureCore::StringRef name) {
    typedef LuaClassPropertyInfoImp<T(ClassType::*)[size]> LCPI;
    LuaStateProtect lsp(mLua);
    if (!get_class_table()) {
        return *this;
    }
    LCPI *p = (LCPI *)lua_newuserdatauv(mLua, sizeof(LCPI), 0);
    new (p) LCPI(name, LuaClassPropertyArrayTraits<T, size, ClassType>::pt_index_function, p);
    p->set_imp(obj);
    LuaMetatable::push_property_function(mLua, -1, false);
    raw_setfieldlen(mLua, -3, name.data(), name.size());
    return *this;
}

template <typename ClassType>
template <typename T, size_t size>
LuaRegisterClass<ClassType> &LuaRegisterClass<ClassType>::def_array(T (&obj)[size], PureCore::StringRef name) {
    LuaStateProtect lsp(mLua);
    if (!get_class_table()) {
        return *this;
    }
    new (lua_newuserdatauv(mLua, sizeof(LuaClassPropertyInfo), 0))
        LuaClassPropertyInfo(name, LuaClassPropertyArrayTraits<T, size, void>::pt_index_function, obj);
    LuaMetatable::push_property_function(mLua, -1, false);
    raw_setfieldlen(mLua, -3, name.data(), name.size());
    return *this;
}

template <typename ClassType>
bool LuaRegisterClass<ClassType>::get_class_table() {
    push_value();
    if (lua_istable(mLua, -1)) {
        return true;
    }
    PureError("Error!!!This  class {} not have class table!!!", LuaClassInfo<ClassType>::get_class_name());
    return false;
}

template <typename ClassType>
void LuaRegisterClass<ClassType>::init_class(PureCore::StringRef name) {
    LuaStateProtect lsp(mLua);
    set_key(name);
    if (LuaClassInfo<ClassType>::is_registered(mLua))  // have this name
    {
        LuaClassInfo<ClassType>::get_class_table(mLua);
        set_value_from(-1);
    } else {
        lua_createtable(mLua, 0, 12);  // class table
        set_value_from(-1);
        lua_pushlstring(mLua, name.data(), name.size());
        raw_setfield(mLua, -2, "cname__");

        lua_createtable(mLua, 0, 3);  // class metatable
        lua_setmetatable(mLua, -2);

        lua_pushcclosure(mLua, LuaMetatableTraits<ClassType>::mt_auto_gc_function, 0);
        raw_setfield(mLua, -2, "gc__");

        lua_pushcclosure(mLua, LuaMetatableTraits<ClassType>::mt_is_auto_gc_funtcion, 0);
        raw_setfield(mLua, -2, "is_auto_gc__");

        lua_pushcclosure(mLua, LuaMetatableTraits<ClassType>::mt_set_auto_gc_funtcion, 0);
        raw_setfield(mLua, -2, "set_auto_gc__");

        lua_pushcclosure(mLua, LuaMetatableTraits<ClassType>::mt_cast_funtcion, 0);
        raw_setfield(mLua, -2, "cast__");

        lua_pushcclosure(mLua, LuaMetatableTraits<ClassType>::mt_toptr_function, 0);
        raw_setfield(mLua, -2, "toptr__");

        lua_pushvalue(mLua, -1);
        raw_setfield(mLua, -2, "__index");

        lua_pushcclosure(mLua, LuaMetatable::mt_gc_function, 0);
        raw_setfield(mLua, -2, "__gc");

        lua_pushlstring(mLua, name.data(), name.size());
        lua_pushcclosure(mLua, LuaMetatableTraits<ClassType>::mt_tostring_function, 1);
        raw_setfield(mLua, -2, "__tostring");

        LuaClassInfo<ClassType>::set_class_table(mLua, -1);
    }
}

template <typename ClassType>
template <typename BaseType>
void LuaRegisterClass<ClassType>::add_base_class() {
    LuaStateProtect lsp(mLua);
    LuaClassInfo<ClassType>::template register_base_class<BaseType>(mLua);
    if (!LuaClassInfo<ClassType>::get_class_table(mLua)) {
        PureError("class is not registered");
        return;
    }
    int top = lua_gettop(mLua);
    if (!LuaClassInfo<BaseType>::get_class_table(mLua)) {
        PureError("base class is not registered");
        return;
    }

    lua_pushnil(mLua);
    while (lua_next(mLua, -2)) {
        lua_pushvalue(mLua, -2);
        lua_rawget(mLua, top);
        if (lua_isnoneornil(mLua, -1) && !LuaClassMgr::check_keep_field_stack(mLua, -3))  // no this field
        {
            lua_pushvalue(mLua, -3);
            lua_pushvalue(mLua, -3);
            lua_rawset(mLua, top);
        }
        lua_pop(mLua, 2);
    }
}

//////////////////////////////////////////////////////////////////////////////////////
// LuaModuleByClass
//////////////////////////////////////////////////////////////////////////////////////
template <typename ClassType>
LuaModuleByClass<ClassType>::LuaModuleByClass(lua_State *L, const std::string &moduleName, const LuaRef &parent, LuaRegisterClass<ClassType> &c)
    : mModule(L, moduleName, parent), mClass(c) {}

template <typename ClassType>
LuaModuleByClass<ClassType>::~LuaModuleByClass() {}

template <typename ClassType>
inline LuaRegisterClass<ClassType> &LuaModuleByClass<ClassType>::end() {
    mModule.end();
    return mClass;
}

template <typename ClassType>
template <typename T>
inline LuaRegisterClass<ClassType> &LuaModuleByClass<ClassType>::operator[](const T &idx) {
    mModule[idx];
    return mClass;
}

template <typename ClassType>
template <typename T>
inline LuaModuleByClass<ClassType> &LuaModuleByClass<ClassType>::def(T obj, PureCore::StringRef name) {
    mModule.def(obj, name);
    return *this;
}

template <typename ClassType>
template <typename T, typename KT>
inline LuaModuleByClass<ClassType> &LuaModuleByClass<ClassType>::def_const(const T &obj, const KT &key) {
    mModule.def_const(obj, key);
    return *this;
}

template <typename ClassType>
template <typename T, typename KT>
inline LuaModuleByClass<ClassType> &LuaModuleByClass<ClassType>::def_const(T &&obj, const KT &key) {
    mModule.def_const(std::move(obj), key);
    return *this;
}
}  // namespace PureLua