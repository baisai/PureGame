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

#include "PureCore/MovePtr.h"
#include "PureLua/LuaClassInfo.h"
#include "PureLua/LuaMetatableTraits.h"
#include "PureLua/LuaObjectPushTraits.h"

namespace PureLua {
struct LuaUserdata {
    template <typename T>
    static inline void push(lua_State* L, const T& t) {
        if (!LuaClassInfo<T>::get_class_table(L)) {
            PureLuaErrorJump(L, "Error!!!LuaUserdata<T>::push failed!!!!");
        }
        const T* obj = new T(t);
        LuaObjectPushTraits<T, true>::push(L, -1, obj, 0);
        lua_remove(L, -2);
    }

    template <typename T>
    static inline void push(lua_State* L, T&& t) {
        if (!LuaClassInfo<T>::get_class_table(L)) {
            PureLuaErrorJump(L, "Error!!!LuaUserdata<T>::push failed!!!!");
        }
        const T* obj = new T(std::move(t));
        LuaObjectPushTraits<T, true>::push(L, -1, obj, 0);
        lua_remove(L, -2);
    }

    template <typename T>
    static inline const T& get(lua_State* L, int idx) {
        T* p = LuaMetatableTraits<T>::userdata_to_object(L, idx);
        if (p == nullptr) {
            PureLuaErrorJump(L, "Error!!!LuaUserdata<T>::get get pointer is null!!!!");
        }
        return *p;
    }
};

struct LuaUserdataMovePtr {
    template <typename T>
    static void push(lua_State* L, T obj) {
        if (obj) {
            if (!LuaClassInfo<typename T::element_type>::get_class_table(L)) {
                PureLuaErrorJump(L, "Error!!!LuaUserdataMovePtr<T>::push failed!!!!");
            }
            typename T::element_type* ptr = obj.release();
            LuaObjectPushTraits<typename T::element_type, true>::push(L, -1, ptr, 0);
            lua_remove(L, -2);
        } else {
            lua_pushnil(L);
        }
    }

    template <typename T>
    static T get(lua_State* L, int idx) {
        return T(LuaMetatableTraits<typename T::element_type>::userdata_to_object_move(L, idx));
    }
};

struct LuaUserdataPtr {
    template <typename T>
    static void push(lua_State* L, const T* t) {
        if (t) {
            if (!LuaClassInfo<T>::get_class_table(L)) {
                PureLuaErrorJump(L, "Error!!!LuaUserdataPtr<T>::push failed!!!!");
            }
            LuaObjectPushTraits<T, false>::push(L, -1, t, 0);
            lua_remove(L, -2);
        } else {
            lua_pushnil(L);
        }
    }

    template <typename T>
    static T* get(lua_State* L, int idx) {
        return LuaMetatableTraits<T>::userdata_to_object(L, idx);
    }
};

}  // namespace PureLua
