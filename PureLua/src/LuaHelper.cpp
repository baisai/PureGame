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

#include "PureLua/LuaHelper.h"
#include "PureLua/LuaStateProtect.h"
#include "PureLua/PureLuaEnv.h"

#include <chrono>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif  // _WIN32

namespace PureLua {
std::string dump_stack(lua_State* L) {
    std::string dumpStr;
    int top = lua_gettop(L);
    for (int i = 1; i <= top; i++) {
        dumpStr.append(fmt::format("[top {}]: ", i));
        int t = lua_type(L, i);
        switch (t) {
            case LUA_TSTRING: {
                dumpStr.append(lua_typename(L, lua_type(L, i)));
                dumpStr.append(" = `");
                dumpStr.append(lua_tostring(L, i));
                dumpStr.push_back('`');
            } break;
            case LUA_TBOOLEAN: {
                dumpStr.append(lua_typename(L, lua_type(L, i)));
                dumpStr.append(" = ");
                dumpStr.append(lua_toboolean(L, i) ? "true" : "false");
            } break;
            case LUA_TNUMBER: {
                dumpStr.append(lua_typename(L, lua_type(L, i)));
                dumpStr.append(" = ");
                dumpStr.append(fmt::format("`{}`", lua_tonumber(L, i)));
            } break;
            case LUA_TTABLE: {
                {
                    LuaStateProtect lsp(L);
                    lua_getglobal(L, "tostring");
                    lua_pushvalue(L, i); /* value to print */
                    const char* s = nullptr;
                    if (!lua_pcall(L, 1, 1, 0)) s = lua_tostring(L, -1); /* get result */
                    if (s == nullptr)
                        dumpStr.append("table: no adress");
                    else
                        dumpStr.append(s);
                }
                dumpStr.append(" begin\n");
                lua_pushnil(L);
                while (lua_next(L, i) != 0) {
                    dumpStr.append("\t");
                    const char* s = lua_typename(L, lua_type(L, -2));
                    dumpStr.append(s);
                    dumpStr.push_back('(');
                    lua_getglobal(L, "tostring");
                    lua_pushvalue(L, -3);
                    s = nullptr;
                    if (!lua_pcall(L, 1, 1, 0)) s = lua_tostring(L, -1);
                    if (s == nullptr)
                        dumpStr.append("unknown");
                    else
                        dumpStr.append(s);
                    dumpStr.append(") = ");
                    lua_pop(L, 1);

                    s = lua_typename(L, lua_type(L, -1));
                    dumpStr.append(s);
                    dumpStr.push_back('(');
                    lua_getglobal(L, "tostring");
                    lua_pushvalue(L, -2);
                    s = nullptr;
                    if (!lua_pcall(L, 1, 1, 0)) s = lua_tostring(L, -1);
                    if (s == nullptr)
                        dumpStr.append("unknown");
                    else
                        dumpStr.append(s);
                    dumpStr.append(")\n");
                    lua_pop(L, 1);

                    lua_pop(L, 1);
                }
                dumpStr.append("table end");
            } break;
            default: {
                dumpStr.append(lua_typename(L, lua_type(L, i)));
                dumpStr.append(" = `");
                {
                    LuaStateProtect lsp(L);
                    lua_getglobal(L, "tostring");
                    lua_pushvalue(L, i); /* value to print */
                    const char* s = nullptr;
                    if (!lua_pcall(L, 1, 1, 0)) s = lua_tostring(L, -1); /* get result */
                    if (s != nullptr) dumpStr.append(s);
                }
                dumpStr.push_back('`');
            } break;
        }
        dumpStr.push_back('\n');
    }
    dumpStr.push_back('\n');
    lua_settop(L, top);
    return dumpStr;
}

int push_lua_error_catcher(lua_State* L) { return PureLuaEnv::push_lua_error_catcher(L); }

const char* string_format(lua_State* L, int idx) {
    if (!lua_isstring(L, idx)) {
        return nullptr;
    }
    idx = lua_absindex(L, idx);
    int top = lua_gettop(L);
    if (idx == top) {
        return lua_tostring(L, idx);
    }
    LuaStateProtect lsp(L);
    int err = push_lua_error_catcher(L);
    lua_getglobal(L, "string");
    lua_getfield(L, -1, "format");
    for (int i = idx; i <= top; ++i) {
        lua_pushvalue(L, i);
    }
    if (lua_pcall(L, top - idx + 1, 1, err)) {
        return nullptr;
    }
    return lua_tostring(L, -1);
}

int raw_getfield(lua_State* L, int index, const char* key) {
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
    return lua_rawget(L, index);
}

void raw_setfield(lua_State* L, int index, const char* key) {
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
    lua_insert(L, -2);
    lua_rawset(L, index);
}

int raw_getfieldlen(lua_State* L, int index, const char* key, size_t l) {
    index = lua_absindex(L, index);
    lua_pushlstring(L, key, l);
    return lua_rawget(L, index);
}

void raw_setfieldlen(lua_State* L, int index, const char* key, size_t l) {
    index = lua_absindex(L, index);
    lua_pushlstring(L, key, l);
    lua_insert(L, -2);
    lua_rawset(L, index);
}

int lua_setfenv(lua_State* L, int idx) {
    idx = lua_absindex(L, idx);
    int n = 0;
    const char* name = nullptr;
    while ((name = lua_getupvalue(L, idx, ++n)) != nullptr) {
        if (strcmp(name, "_ENV") == 0) {
            lua_pop(L, 1);
            if (lua_setupvalue(L, idx, n) != nullptr) {
                return 1;
            }
            return 0;
        }
        lua_pop(L, 1);
    }
    return 0;
}

void lua_getfenv(lua_State* L, int idx) {
    int n = 0;
    const char* name = nullptr;
    while ((name = lua_getupvalue(L, idx, ++n)) != nullptr) {
        if (strcmp(name, "_ENV") == 0) return;
        lua_pop(L, 1);
    }
}

bool isfulluserdata(lua_State* L, int index) { return lua_isuserdata(L, index) && !lua_islightuserdata(L, index); }

void lock_metatable(lua_State* L, int index) {
    index = lua_absindex(L, index);
    lua_pushstring(L, "__metatable");
    lua_pushstring(L, "error!!!metatable is locked!!!");
    lua_rawset(L, index);
}
}  // namespace PureLua