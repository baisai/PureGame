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

#include "PureLua/LuaModule.h"

namespace PureLua {
template <typename ClassType>
class LuaModuleByClass;

template <typename... Args>
struct BaseClassStrategy {};

// namespace use ::table::table::classname
template <typename ClassType>
class LuaRegisterClass : public LuaRegisterNode {
public:
    template <typename RT, bool>
    friend class LuaDefFuncHelper;
    template <typename RT, bool>
    friend class LuaDefPropHelper;

    LuaRegisterClass(lua_State* L, PureCore::StringRef name);

    template <typename... Args>
    LuaRegisterClass(lua_State* L, PureCore::StringRef name,
                     BaseClassStrategy<Args...>);  // call once again base every class

    LuaRegisterClass(const LuaRegisterClass<ClassType>& cp);

    virtual ~LuaRegisterClass() {}

    template <typename T>
    LuaRegisterClass& operator=(const LuaRegisterClass<T>& right);

    template <typename... Args>
    LuaRegisterClass& default_ctor();

    template <typename R, typename... Args>
    LuaRegisterClass& def_ctor(R (*newer)(Args...), void (*deler)(ClassType*));

    template <typename LamadaNewer, typename LamadaDeler>
    LuaRegisterClass& def_ctor(LamadaNewer newer, LamadaDeler deler);

    template <typename T>
    LuaRegisterClass& def(T obj, PureCore::StringRef name);

    template <typename T, size_t size>
    LuaRegisterClass& def(T (ClassType::*obj)[size], PureCore::StringRef name);

    template <typename T, size_t size>
    LuaRegisterClass& def(T (&obj)[size], PureCore::StringRef name);

    LuaModuleByClass<ClassType> def_module(PureCore::StringRef name);

    template <typename T, typename KT>
    LuaRegisterClass& def_const(const T& obj, const KT& name);

    template <typename T, typename KT>
    LuaRegisterClass& def_const(T&& obj, const KT& name);

    template <typename T>
    LuaRegisterClass& def_move_return(T obj, PureCore::StringRef name);

private:
    LuaRegisterClass& def_class_func(lua_CFunction obj, PureCore::StringRef name);

    template <typename T>
    LuaRegisterClass& def_class_func(T obj, PureCore::StringRef name);

    template <typename LamadaType>
    LuaRegisterClass& def_lamada(LamadaType lamada, PureCore::StringRef name);

    template <typename T>
    LuaRegisterClass& def_property(const T ClassType::*obj, PureCore::StringRef name);

    template <typename T>
    LuaRegisterClass& def_property(T ClassType::*obj, PureCore::StringRef name);

    template <typename T>
    LuaRegisterClass& def_property(const T& obj, PureCore::StringRef name);

    template <typename T>
    LuaRegisterClass& def_property(T& obj, PureCore::StringRef name);

    template <typename T>
    LuaRegisterClass& def_property(const T* obj, PureCore::StringRef name);

    template <typename T>
    LuaRegisterClass& def_property(T* obj, PureCore::StringRef name);

    template <typename T, size_t size>
    LuaRegisterClass& def_array(T (ClassType::*obj)[size], PureCore::StringRef name);

    template <typename T, size_t size>
    LuaRegisterClass& def_array(T (&obj)[size], PureCore::StringRef name);

    bool get_class_table();

    void init_class(PureCore::StringRef name);

    template <typename BaseType>
    void add_base_class();
};

template <typename ClassType>
class LuaModuleByClass {
public:
    LuaModuleByClass(lua_State* L, const std::string& moduleName, const LuaRef& parent, LuaRegisterClass<ClassType>& c);

    virtual ~LuaModuleByClass();

    LuaRegisterClass<ClassType>& end();

    template <typename T>
    LuaRegisterClass<ClassType>& operator[](const T& idx);

    template <typename T>
    LuaModuleByClass& def(T obj, PureCore::StringRef name);

    template <typename T, typename KT>
    LuaModuleByClass& def_const(const T& obj, const KT& key);

    template <typename T, typename KT>
    LuaModuleByClass& def_const(T&& obj, const KT& key);

protected:
    LuaModule mModule;
    LuaRegisterClass<ClassType>& mClass;
};

}  // namespace PureLua

#include "PureLua/LuaRegisterClass.inl.h"
