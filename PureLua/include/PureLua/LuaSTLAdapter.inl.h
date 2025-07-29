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

#include "PureLua/LuaSTLAdapter.h"
#include "PureLua/LuaModule.h"
#include "PureLua/LuaRegisterClass.h"
#include "PureLua/LuaStackExt.h"

#include <type_traits>

namespace PureLua {
template <typename ST>
void LuaSharedPtrAdapter<ST>::try_register(LuaModule &lm) {
    if (!LuaClassInfo<ST>::is_registered(lm.state())) {
        lm[LuaRegisterClass<ST>(lm.state(), LuaClassInfo<ST>::get_class_name()).def(get, "get").def(valid, "valid")];
    }
}

template <typename ST>
inline typename ST::element_type *LuaSharedPtrAdapter<ST>::get(ST *self) {
    if (self == nullptr) {
        return nullptr;
    }
    return self->get();
}

template <typename ST>
inline bool LuaSharedPtrAdapter<ST>::valid(ST *self) {
    if (self == nullptr) {
        return false;
    }
    return static_cast<bool>(*self);
}

template <typename WT>
void LuaWeakPtrAdapter<WT>::try_register(LuaModule &lm) {
    if (!LuaClassInfo<WT>::is_registered(lm.state())) {
        lm[LuaRegisterClass<WT>(lm.state(), LuaClassInfo<WT>::get_class_name()).def(get, "get").def(valid, "valid")];
    }
}

template <typename WT>
inline typename WT::element_type *LuaWeakPtrAdapter<WT>::get(WT *self) {
    if (self == nullptr) {
        return nullptr;
    }
    return self->get();
}

template <typename WT>
inline bool LuaWeakPtrAdapter<WT>::valid(WT *self) {
    if (self == nullptr) {
        return false;
    }
    return static_cast<bool>(*self);
}

template <typename VT>
void LuaVectorAdapter<VT>::try_register(LuaModule &lm) {
    if (!LuaClassInfo<VT>::is_registered(lm.state())) {
        lm[LuaRegisterClass<VT>(lm.state(), LuaClassInfo<VT>::get_class_name())
               .def(static_cast<size_t (VT::*)() const>(&VT::size), "size")
               .def(static_cast<size_t (VT::*)() const>(&VT::max_size), "max_size")
               .def(static_cast<void (VT::*)(size_t)>(&VT::resize), "resize")
               .def(static_cast<size_t (VT::*)() const>(&VT::capacity), "capacity")
               .def(static_cast<bool (VT::*)() const>(&VT::empty), "empty")
               .def(static_cast<void (VT::*)(size_t)>(&VT::reserve), "reserve")
               .def(get, "get")
               .def(set, "set")
               .def(get_back, "get_back")
               .def(set_back, "set_back")
               .def(get_front, "get_front")
               .def(set_front, "set_front")
               .def(static_cast<void (VT::*)(const typename VT::value_type &)>(&VT::push_back), "push_back")
               .def(static_cast<void (VT::*)()>(&VT::pop_back), "pop_back")
               .def(insert, "insert")
               .def(erase, "erase")
               .def(static_cast<void (VT::*)(VT &)>(&VT::swap), "swap")
               .def(static_cast<void (VT::*)()>(&VT::clear), "clear")
               .def(to_array_ref, "to_array_ref")
               .def(to_table, "to_table")
               .def(from_table, "from_table")];
    }
}

template <typename VT>
inline const typename VT::value_type &LuaVectorAdapter<VT>::get(VT *self, size_t idx) {
    return self->at(idx);
}

template <typename VT>
int LuaVectorAdapter<VT>::set(lua_State *L) {
    if (std::is_const<typename VT::value_type>::value) {
        PureLuaErrorJump(L, "Error!!! set the const type({}) value!!!", typeid(typename VT::value_type).name());
        return 0;
    }
    VT *self = LuaStack<VT *>::get(L, 1);
    if (!self) {
        PureLuaErrorJump(L, "Error!!!The vector is nullptr!!!");
        return 0;
    }
    size_t idx = LuaStack<size_t>::get(L, 2);
    const typename VT::value_type &value = LuaStack<const typename VT::value_type &>::get(L, 3);
    (*self)[idx] = value;
    return 0;
}

template <typename VT>
inline const typename VT::value_type &LuaVectorAdapter<VT>::get_back(VT *self) {
    return self->back();
}

template <typename VT>
int LuaVectorAdapter<VT>::set_back(lua_State *L) {
    if (std::is_const<typename VT::value_type>::value) {
        PureLuaErrorJump(L, "Error!!! set the const type({}) value!!!", typeid(typename VT::value_type).name());
        return 0;
    }
    VT *self = LuaStack<VT *>::get(L, 1);
    if (!self) {
        PureLuaErrorJump(L, "Error!!!The vector is nullptr!!!");
        return 0;
    }
    const typename VT::value_type &value = LuaStack<const typename VT::value_type &>::get(L, 2);
    self->back() = value;
    return 0;
}

template <typename VT>
inline const typename VT::value_type &LuaVectorAdapter<VT>::get_front(VT *self) {
    return self->front();
}

template <typename VT>
int LuaVectorAdapter<VT>::set_front(lua_State *L) {
    if (std::is_const<typename VT::value_type>::value) {
        PureLuaErrorJump(L, "Error!!! set the const type({}) value!!!", typeid(typename VT::value_type).name());
        return 0;
    }
    VT *self = LuaStack<VT *>::get(L, 1);
    if (!self) {
        PureLuaErrorJump(L, "Error!!!The vector is nullptr!!!");
        return 0;
    }
    const typename VT::value_type &value = LuaStack<const typename VT::value_type &>::get(L, 2);
    self->front() = value;
    return 0;
}

template <typename VT>
inline void LuaVectorAdapter<VT>::insert(VT *self, size_t idx, const typename VT::value_type &value) {
    self->insert(self->begin() + idx, value);
}

template <typename VT>
inline void LuaVectorAdapter<VT>::erase(VT *self, size_t begin, size_t end) {
    self->erase(self->begin() + begin, self->begin() + end);
}

template <typename VT>
inline PureCore::ArrayRef<typename VT::value_type> LuaVectorAdapter<VT>::to_array_ref(VT *self) {
    return PureCore::ArrayRef<typename VT::value_type>(*self);
}

template <typename VT>
int LuaVectorAdapter<VT>::to_table(lua_State *L) {
    VT *self = LuaStack<VT *>::get(L, 1);
    if (!self) {
        return 0;
    }
    int size = static_cast<int>(self->size());
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; ++i) {
        LuaStack<const typename VT::value_type &>::push(L, self->at(i));
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

template <typename VT>
int LuaVectorAdapter<VT>::from_table(lua_State *L) {
    int result = 0;
    do {
        VT *self = LuaStack<VT *>::get(L, 1);
        if (!self) {
            PureLuaErrorJump(L, "Error!!!The vector is nullptr!!!");
            break;
        }
        if (!lua_istable(L, 2)) {
            PureLuaErrorJump(L,
                             "Error!!!The index 2 param is not a table, "
                             "convert vector failed!!!");
            break;
        }
        int len = static_cast<int>(lua_rawlen(L, 2));
        self->reserve(self->size() + len);
        for (int i = 1; i <= len; i++) {
            lua_rawgeti(L, 2, i);
            self->push_back(LuaStack<const typename VT::value_type &>::get(L, -1));
            lua_pop(L, 1);
        }
        result = 1;
    } while (false);

    lua_pushboolean(L, result);
    return 1;
}

template <typename LT>
void LuaListAdapter<LT>::try_register(LuaModule &lm) {
    if (!LuaClassInfo<LT>::is_registered(lm.state())) {
        lm[LuaRegisterClass<LT>(lm.state(), LuaClassInfo<LT>::get_class_name())
               .def(static_cast<bool (LT::*)() const>(&LT::empty), "empty")
               .def(static_cast<size_t (LT::*)() const>(&LT::size), "size")
               .def(static_cast<size_t (LT::*)() const>(&LT::max_size), "max_size")
               .def(get_back, "get_back")
               .def(set_back, "set_back")
               .def(get_front, "get_front")
               .def(set_front, "set_front")
               .def(static_cast<void (LT::*)(const typename LT::value_type &)>(&LT::push_front), "push_front")
               .def(static_cast<void (LT::*)()>(&LT::pop_front), "pop_front")
               .def(static_cast<void (LT::*)(const typename LT::value_type &)>(&LT::push_back), "push_back")
               .def(static_cast<void (LT::*)()>(&LT::pop_back), "pop_back")
               .def(static_cast<void (LT::*)(LT &)>(&LT::swap), "swap")
               .def(static_cast<void (LT::*)(size_t)>(&LT::resize), "resize")
               .def(static_cast<void (LT::*)()>(&LT::clear), "clear")
               .def(static_cast<void (LT::*)(const typename LT::value_type &)>(&LT::remove), "remove")
               .def(static_cast<void (LT::*)()>(&LT::sort), "sort")
               .def(static_cast<void (LT::*)()>(&LT::reverse), "reverse")
               .def(to_table, "to_table")
               .def(from_table, "from_table")];
    }
}

template <typename LT>
inline const typename LT::value_type &LuaListAdapter<LT>::get_back(LT *self) {
    return self->back();
}

template <typename LT>
int LuaListAdapter<LT>::set_back(lua_State *L) {
    if (std::is_const<typename LT::value_type>::value) {
        PureLuaErrorJump(L, "Error!!! set the const type({}) value!!!", typeid(typename LT::value_type).name());
        return 0;
    }
    LT *self = LuaStack<LT *>::get(L, 1);
    if (!self) {
        PureLuaErrorJump(L, "Error!!!The list is nullptr!!!");
        return 0;
    }
    const typename LT::value_type &value = LuaStack<const typename LT::value_type &>::get(L, 2);
    self->back() = value;
    return 0;
}

template <typename LT>
inline const typename LT::value_type &LuaListAdapter<LT>::get_front(LT *self) {
    return self->front();
}

template <typename LT>
int LuaListAdapter<LT>::set_front(lua_State *L) {
    if (std::is_const<typename LT::value_type>::value) {
        PureLuaErrorJump(L, "Error!!! set the const type({}) value!!!", typeid(typename LT::value_type).name());
        return 0;
    }
    LT *self = LuaStack<LT *>::get(L, 1);
    if (!self) {
        PureLuaErrorJump(L, "Error!!!The list is nullptr!!!");
        return 0;
    }
    const typename LT::value_type &value = LuaStack<const typename LT::value_type &>::get(L, 2);
    self->front() = value;
    return 0;
}

template <typename LT>
int LuaListAdapter<LT>::to_table(lua_State *L) {
    LT *self = LuaStack<LT *>::get(L, 1);
    if (!self) {
        return 0;
    }
    lua_createtable(L, 0, 0);
    int i = 1;
    for (typename LT::const_iterator iter = self->begin(); iter != self->end(); ++iter) {
        LuaStack<const typename LT::value_type &>::push(L, *iter);
        lua_rawseti(L, -2, i++);
    }
    return 1;
}

template <typename LT>
int LuaListAdapter<LT>::from_table(lua_State *L) {
    int result = 0;
    do {
        LT *self = LuaStack<LT *>::get(L, 1);
        if (!self) {
            PureLuaErrorJump(L, "Error!!!The list is nullptr!!!");
            break;
        }
        if (!lua_istable(L, 2)) {
            PureLuaErrorJump(L,
                             "Error!!!The index 2 param is not a table, "
                             "convert list failed!!!");
            break;
        }
        size_t len = lua_rawlen(L, 2);
        for (size_t i = 1; i <= len; i++) {
            lua_rawgeti(L, 2, i);
            self->push_back(LuaStack<const typename LT::value_type &>::get(L, -1));
            lua_pop(L, 1);
        }
        result = 1;
    } while (false);

    lua_pushboolean(L, result);
    return 1;
}

template <typename ST>
void LuaSetAdapter<ST>::try_register(LuaModule &lm) {
    if (!LuaClassInfo<ST>::is_registered(lm.state())) {
        lm[LuaRegisterClass<ST>(lm.state(), LuaClassInfo<ST>::get_class_name())
               .def(static_cast<bool (ST::*)() const>(&ST::empty), "empty")
               .def(static_cast<size_t (ST::*)() const>(&ST::size), "size")
               .def(static_cast<size_t (ST::*)() const>(&ST::max_size), "max_size")
               .def(insert, "insert")
               .def(static_cast<size_t (ST::*)(const typename ST::value_type &)>(&ST::erase), "erase")
               .def(static_cast<void (ST::*)(ST &)>(&ST::swap), "swap")
               .def(static_cast<void (ST::*)()>(&ST::clear), "clear")
               .def(exist, "exist")
               .def(to_table, "to_table")
               .def(from_table, "from_table")];
    }
}

template <typename ST>
inline bool LuaSetAdapter<ST>::insert(ST *self, const typename ST::value_type &value) {
    return self->insert(value).second;
}

template <typename ST>
inline bool LuaSetAdapter<ST>::exist(ST *self, const typename ST::value_type &value) {
    return self->find(value) != self->end();
}

template <typename ST>
int LuaSetAdapter<ST>::to_table(lua_State *L) {
    ST *self = LuaStack<ST *>::get(L, 1);
    lua_createtable(L, 0, int(self->size()));
    for (typename ST::const_iterator iter = self->begin(); iter != self->end(); ++iter) {
        LuaStack<const typename ST::value_type &>::push(L, *iter);
        LuaStack<bool>::push(L, true);
        lua_rawset(L, -3);
    }
    return 1;
}

template <typename ST>
int LuaSetAdapter<ST>::from_table(lua_State *L) {
    int result = 0;
    do {
        ST *self = LuaStack<ST *>::get(L, 1);
        if (!self) {
            PureLuaErrorJump(L, "Error!!!The set is nullptr!!!");
            break;
        }
        if (!lua_istable(L, 2)) {
            PureLuaErrorJump(L, "Error!!!The 2 param is not a table, convert set failed!!!");
            break;
        }

        lua_pushnil(L);
        while (lua_next(L, 2)) {
            self->insert(LuaStack<const typename ST::value_type &>::get(L, -2));
            lua_pop(L, 1);
        }
        result = 1;
    } while (false);

    lua_pushboolean(L, result);
    return 1;
}

template <typename PT>
void LuaPairAdapter<PT>::try_register(LuaModule &lm) {
    if (!LuaClassInfo<PT>::is_registered(lm.state())) {
        lm[LuaRegisterClass<PT>(lm.state(), LuaClassInfo<PT>::get_class_name())
               .def(&PT::first, "first")
               .def(&PT::second, "second")
               .def(get, "get")
               .def(set, "set")];
    }
}

template <typename PT>
int LuaPairAdapter<PT>::get(lua_State *L) {
    PT *self = LuaStack<PT *>::get(L, 1);
    LuaStack<const typename PT::first_type &>::push(L, self->first);
    LuaStack<const typename PT::second_type &>::push(L, self->second);
    return 2;
}

template <typename PT>
int LuaPairAdapter<PT>::set(lua_State *L) {
    int result = 0;
    do {
        if (std::is_const<typename PT::first_type>::value || std::is_const<typename PT::second_type>::value) {
            PureLuaErrorJump(L, "Error!!! set the const type({}, {}) value!!!", typeid(typename PT::first_type).name(),
                             typeid(typename PT::second_type).name());
            break;
        }
        PT *self = LuaStack<PT *>::get(L, 1);
        if (!self) {
            PureLuaErrorJump(L, "Error!!!The pair is nullptr!!!");
            break;
        }

        const typename PT::first_type &first = LuaStack<const typename PT::first_type &>::get(L, 2);
        const typename PT::second_type &second = LuaStack<const typename PT::second_type &>::get(L, 3);

        self->first = first;
        self->second = second;

        result = 1;

    } while (false);

    lua_pushboolean(L, result);
    return 1;
}

template <typename MT>
void LuaMapAdapter<MT>::try_register(LuaModule &lm) {
    if (!LuaClassInfo<MT>::is_registered(lm.state())) {
        lm[LuaRegisterClass<MT>(lm.state(), LuaClassInfo<MT>::get_class_name())
               .def(static_cast<bool (MT::*)() const>(&MT::empty), "empty")
               .def(static_cast<size_t (MT::*)() const>(&MT::size), "size")
               .def(static_cast<size_t (MT::*)() const>(&MT::max_size), "max_size")
               .def(insert, "insert")
               .def(static_cast<size_t (MT::*)(const typename MT::key_type &)>(&MT::erase), "erase")
               .def(static_cast<void (MT::*)(MT &)>(&MT::swap), "swap")
               .def(static_cast<void (MT::*)()>(&MT::clear), "clear")
               .def(set, "set")
               .def(get, "get")
               .def(exist, "exist")
               .def(to_table, "to_table")
               .def(from_table, "from_table")];
    }
}

template <typename MT>
inline bool LuaMapAdapter<MT>::insert(MT *self, const typename MT::key_type &key, const typename MT::mapped_type &value) {
    return self->insert(std::make_pair(key, value)).second;
}

template <typename MT>
int LuaMapAdapter<MT>::set(lua_State *L) {
    if (std::is_const<typename MT::mapped_type>::value) {
        PureLuaErrorJump(L, "Error!!! set the const type({}) value!!!", typeid(typename MT::mapped_type).name());
        return 0;
    }
    MT *self = LuaStack<MT *>::get(L, 1);
    if (!self) {
        PureLuaErrorJump(L, "Error!!!The map is nullptr!!!");
        return 0;
    }
    const typename MT::key_type &key = LuaStack<const typename MT::key_type &>::get(L, 2);
    const typename MT::mapped_type &value = LuaStack<const typename MT::mapped_type &>::get(L, 3);
    (*self)[key] = value;
    return 0;
}

template <typename MT>
typename MT::mapped_type *LuaMapAdapter<MT>::get(MT *self, const typename MT::key_type &key) {
    typename MT::iterator iter = self->find(key);
    if (iter != self->end()) {
        return &iter->second;
    }
    return nullptr;
}

template <typename MT>
inline bool LuaMapAdapter<MT>::exist(MT *self, const typename MT::key_type &key) {
    return self->find(key) != self->end();
}

template <typename MT>
int LuaMapAdapter<MT>::to_table(lua_State *L) {
    MT *self = LuaStack<MT *>::get(L, 1);
    lua_createtable(L, 0, static_cast<int>(self->size()));
    for (typename MT::const_iterator iter = self->begin(); iter != self->end(); ++iter) {
        LuaStack<const typename MT::key_type &>::push(L, iter->first);
        LuaStack<const typename MT::mapped_type &>::push(L, iter->second);
        lua_rawset(L, -3);
    }
    return 1;
}

template <typename MT>
int LuaMapAdapter<MT>::from_table(lua_State *L) {
    int result = 0;
    do {
        MT *self = LuaStack<MT *>::get(L, 1);
        if (!self) {
            PureLuaErrorJump(L, "Error!!!The map is nullptr!!!");
            break;
        }
        if (!lua_istable(L, 2)) {
            PureLuaErrorJump(L, "Error!!!The 2 param is not a table, convert map failed!!!");
            break;
        }

        lua_pushnil(L);
        while (lua_next(L, 2)) {
            const typename MT::key_type &key = LuaStack<const typename MT::key_type &>::get(L, -2);
            const typename MT::mapped_type &value = LuaStack<const typename MT::mapped_type &>::get(L, -1);
            self->insert(std::make_pair(key, value));
            lua_pop(L, 1);
        }
        result = 1;
    } while (false);

    lua_pushboolean(L, result);
    return 1;
}

template <typename ART>
void LuaArrayRefAdapter<ART>::try_register(LuaModule &lm) {
    if (!LuaClassInfo<ART>::is_registered(lm.state())) {
        lm[LuaRegisterClass<ART>(lm.state(), LuaClassInfo<ART>::get_class_name())
               .def(get, "get")
               .def(set, "set")
               .def(&ART::empty, "empty")
               .def(&ART::size, "size")
               .def(to_table, "to_table")];
    }
}

template <typename ART>
inline const typename ART::ValueType &LuaArrayRefAdapter<ART>::get(ART *self, size_t idx) {
    return (*self)[idx];
}

template <typename ART>
int LuaArrayRefAdapter<ART>::set(lua_State *L) {
    if (std::is_const<typename ART::ValueType>::value) {
        PureLuaErrorJump(L, "Error!!! set the const type({}) value!!!", typeid(typename ART::ValueType).name());
        return 0;
    }
    ART *self = LuaStack<ART *>::get(L, 1);
    if (!self) {
        PureLuaErrorJump(L, "Error!!!The ArrayRef is nullptr!!!");
        return 0;
    }
    size_t idx = LuaStack<size_t>::get(L, 2);
    const typename ART::ValueType &value = LuaStack<const typename ART::ValueType &>::get(L, 3);
    (*self)[idx] = value;
    return 0;
}

template <typename ART>
int LuaArrayRefAdapter<ART>::to_table(lua_State *L) {
    ART *self = LuaStack<ART *>::get(L, 1);
    if (!self) {
        return 0;
    }
    int size = static_cast<int>(self->size());
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; ++i) {
        LuaStack<const typename ART::ValueType &>::push(L, (*self)[i]);
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

template <typename T>
LuaSTLHelper<T>::LuaSTLHelper() {
    LuaSTLRegister::inst()->add_register(T::try_register);
}

}  // namespace PureLua
