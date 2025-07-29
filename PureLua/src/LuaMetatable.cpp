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

#include "PureLua/LuaClassPropertyTraits.h"
#include "PureLua/LuaMetatable.h"
#include "PureLua/LuaUserdataForClass.h"

namespace PureLua {
LuaUserdataForClass *LuaMetatable::get_userdata(lua_State *L, int idx) {
    idx = lua_absindex(L, idx);
    if (lua_istable(L, idx)) {
        lua_pushlightuserdata(L, LuaClassMgr::get_object_key());
        lua_rawget(L, idx);
    } else if (lua_isuserdata(L, idx)) {
        lua_pushvalue(L, idx);
    } else {
        if (!lua_isnoneornil(L, idx)) {
            PureLuaErrorJump(L, "Error!!!The arg {} type is {}, need table or userdata!!!", idx, lua_typename(L, lua_type(L, idx)));
        }
        lua_pushnil(L);
        return nullptr;
    }
    LuaUserdataForClass *p = (LuaUserdataForClass *)lua_touserdata(L, -1);
    return p;
}

int LuaMetatable::mt_property_getset_function(lua_State *L) {
    LuaClassPropertyInfo *p = (LuaClassPropertyInfo *)lua_touserdata(L, lua_upvalueindex(1));
    if (p != nullptr) {
        if (lua_isnoneornil(L, 2)) {
            return p->call_index(L);
        } else {
            if (p->is_const()) {
                PureLuaErrorJump(L, "Error!!!This field is a const field!!!");
                return 0;
            } else {
                return p->call_newindex(L, 2);
            }
        }
    }
    PureLuaErrorJump(L, "Error!!!Not this field!!!");
    return 0;
}

int LuaMetatable::mt_static_getset_function(lua_State *L) {
    LuaClassPropertyInfo *p = (LuaClassPropertyInfo *)lua_touserdata(L, lua_upvalueindex(1));
    if (p != nullptr) {
        if (lua_isnoneornil(L, 1)) {
            return p->call_index(L);
        } else {
            if (p->is_const()) {
                PureLuaErrorJump(L, "Error!!!This static field is a const field!!!");
                return 0;
            } else {
                return p->call_newindex(L, 1);
            }
        }
    }
    PureLuaErrorJump(L, "Error!!!Not this static field!!!");
    return 0;
}

int LuaMetatable::mt_property_gc_function(lua_State *L) {
    if (lua_isuserdata(L, 1)) {
        LuaClassPropertyInfo *p = (LuaClassPropertyInfo *)lua_touserdata(L, 1);
        if (p != nullptr) {
            p->~LuaClassPropertyInfo();
        }
    }
    return 0;
}

bool LuaMetatable::push_property_function(lua_State *L, int propertyIdx, bool isStatic) {
    if (lua_isuserdata(L, propertyIdx)) {
        propertyIdx = lua_absindex(L, propertyIdx);
        lua_pushlightuserdata(L, (void *)push_property_function);
        lua_rawget(L, LUA_REGISTRYINDEX);
        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            lua_createtable(L, 0, 1);
            lua_pushstring(L, "__gc");
            lua_pushcclosure(L, mt_property_gc_function, 0);
            lua_rawset(L, -3);

            lua_pushlightuserdata(L, (void *)push_property_function);
            lua_pushvalue(L, -2);
            lua_rawset(L, LUA_REGISTRYINDEX);
        }
        lua_setmetatable(L, propertyIdx);
        lua_pushvalue(L, propertyIdx);
        if (isStatic) {
            lua_pushcclosure(L, mt_static_getset_function, 1);
        } else {
            lua_pushcclosure(L, mt_property_getset_function, 1);
        }
        return true;
    }
    return false;
}

int LuaMetatable::mt_gc_function(lua_State *L) {
    if (lua_isuserdata(L, 1)) {
        LuaUserdataForClass *p = (LuaUserdataForClass *)lua_touserdata(L, 1);
        if (p != nullptr) {
            p->~LuaUserdataForClass();
        }
    }
    return 0;
}

}  // namespace PureLua