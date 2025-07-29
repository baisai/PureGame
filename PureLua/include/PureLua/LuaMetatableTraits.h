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
#include "PureLua/LuaClassInfo.h"
#include "PureLua/LuaObjectCache.h"
#include "PureLua/LuaUserdataForClass.h"
#include "PureLua/LuaMetatable.h"

namespace PureLua {
////////////////////////////////////////////////////////////////////////////////////////////////////////
// LuaMetatableTraits
////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename ClassType>
struct LuaMetatableTraits {
private:
    static LuaUserdataForClass* userdata_to_imp(lua_State* L, int idx) {
        LuaStateProtect lsp(L);
        LuaUserdataForClass* p = LuaMetatable::get_userdata(L, idx);
        if (p == nullptr) {
            return nullptr;
        }
        int32_t offset = LuaClassInfo<ClassType>::calc_from_offset(L, idx);
        if (!LuaClassMgr::is_valid_offset(offset)) {
            PureError("Error!!!The arg {} is a object, but it is not connection {}!!!", idx, LuaClassInfo<ClassType>::get_class_name());
            return nullptr;
        }
        p->set_class_offset(offset);
        return p;
    }

public:
    static inline bool is_class_type(lua_State* L, int idx) {
        if (lua_istable(L, idx) || lua_isuserdata(L, idx)) {
            return LuaClassMgr::is_valid_offset(LuaClassInfo<ClassType>::calc_from_offset(L, idx));
        }
        return false;
    }

    static ClassType* userdata_to_object(lua_State* L, int idx) {
        LuaUserdataForClass* p = userdata_to_imp(L, idx);
        if (p) {
            return p->get_object_t<ClassType>();
        }
        return nullptr;
    }

    static ClassType* userdata_to_object_move(lua_State* L, int idx) {
        ClassType* obj = nullptr;
        LuaUserdataForClass* p = userdata_to_imp(L, idx);
        if (p && p->is_auto_gc()) {
            obj = p->move_object_t<ClassType>();
        } else {
            PureError("this object is not auto gc by lua");
        }
#ifdef PURELUA_CACHE_OBJECT
        if (obj) {
            lua_remove_cache_object(L, LuaClassInfo<ClassType>::get_class_key(), (void*)obj);
        }
#endif
        return obj;
    }

    static int mt_auto_gc_function(lua_State* L) {
        LuaUserdataForClass* p = LuaMetatable::get_userdata(L, 1);
        if (p) {
            p->auto_gc();
        }
        return 0;
    }

    static int mt_is_auto_gc_funtcion(lua_State* L) {
        int autoGC = 0;
        LuaUserdataForClass* p = LuaMetatable::get_userdata(L, 1);
        if (p) {
            autoGC = p->is_auto_gc() ? 1 : 0;
        }
        lua_pushboolean(L, autoGC);
        return 1;
    }

    static int mt_set_auto_gc_funtcion(lua_State* L) {
        LuaUserdataForClass* p = LuaMetatable::get_userdata(L, 1);
        if (p) {
            bool autoGC = lua_toboolean(L, 2);
            p->set_auto_gc(autoGC);
        }
        return 0;
    }

    static int mt_cast_funtcion(lua_State* L) {
        LuaUserdataForClass* p = LuaMetatable::get_userdata(L, 1);
        if (p == nullptr) {
            PureLuaErrorJump(L, "object ptr is nullptr");
            return 0;
        }
        int32_t offset = LuaClassInfo<ClassType>::calc_to_offset(L, 2);
        if (!LuaClassMgr::is_valid_offset(offset)) {
            PureLuaErrorJump(L, "between object and type is not connection");
            return 0;
        }
        p->cast_object(offset);
        lua_pushvalue(L, 2);
        lua_setmetatable(L, 1);
        lua_pushvalue(L, 1);
        return 1;
    }

    static int mt_tostring_function(lua_State* L) {
        LuaUserdataForClass* p = LuaMetatable::get_userdata(L, 1);
        if (p == nullptr) {
            PureLuaErrorJump(L, "object ptr is nullptr");
            return 0;
        }
        int32_t offset = LuaClassInfo<ClassType>::calc_from_offset(L, 1);

        if (!LuaClassMgr::is_valid_offset(offset)) {
            PureLuaErrorJump(L, "Error!!!The arg 1 is a object, but it is not connection of {}!!!", LuaClassInfo<ClassType>::get_class_name());
            return 0;
        }
        p->set_class_offset(offset);
        auto obj = p->get_object_t<ClassType>();
        std::string info(fmt::format("[{}]({})", lua_tostring(L, lua_upvalueindex(1)), (void*)obj));
        lua_pushlstring(L, info.data(), info.size());
        return 1;
    }

    static int mt_toptr_function(lua_State* L) {
        LuaUserdataForClass* p = LuaMetatable::get_userdata(L, 1);
        if (p == nullptr) {
            PureLuaErrorJump(L, "object ptr is nullptr");
            return 0;
        }
        int32_t offset = LuaClassInfo<ClassType>::calc_from_offset(L, 1);

        if (!LuaClassMgr::is_valid_offset(offset)) {
            PureLuaErrorJump(L, "Error!!!The arg 1 is a object, but it is not connection of {}!!!", LuaClassInfo<ClassType>::get_class_name());
            return 0;
        }
        p->set_class_offset(offset);
        auto obj = p->get_object_t<ClassType>();
        lua_pushlightuserdata(L, (void*)obj);
        return 1;
    }
};
}  // namespace PureLua
