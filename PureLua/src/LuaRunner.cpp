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

#include "PureLua/LuaRunner.h"
#include "PureLua/LuaStateProtect.h"
#include "PureLua/LuaErrorDesc.h"
#include "PureLua/LuaHelper.h"

namespace PureLua {
LuaRunner::LuaRunner(lua_State *L) : mLua(L) {}

int LuaRunner::set_stack_level(int level) {
    if (level < 0) {
        return ErrorInvalidArg;
    }
    mLevel = level;
    return Success;
}

int LuaRunner::exec(const std::string &code) {
    if (!mLua) {
        return ErrorLuaStateNullptr;
    }
    mResults.clear();
    LuaStateProtect lsp(mLua);
    if (luaL_loadstring(mLua, code.c_str())) {
        const char *errMsg = lua_tostring(mLua, -1);
        mResults.push_back(errMsg);
        return ErrorLuaLoadFailed;
    }

    lua_createtable(mLua, 0, 0);
    lua_rawgeti(mLua, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
    lua_setfield(mLua, -2, "__index");
    lua_pushvalue(mLua, -1);
    lua_setmetatable(mLua, -2);
    lua_pushlightuserdata(mLua, this);
    auto execLog = [](lua_State *L) -> int {
        LuaRunner *self = (LuaRunner *)lua_touserdata(L, lua_upvalueindex(1));
        if (self == nullptr) {
            return 0;
        }
        int n = lua_gettop(L); /* number of arguments */
        lua_getglobal(L, "tostring");
        const char *s = nullptr;
        std::string str;
        for (int i = 1; i <= n; ++i) {
            lua_pushvalue(L, -1); /* function to be called */
            lua_pushvalue(L, i);  /* value to print */
            lua_call(L, 1, 1);

            s = lua_tostring(L, -1); /* get result */
            if (s == nullptr) {
                return luaL_error(L, "\'tostring\' must return a string to \'print\'");
            }
            str.append(s);
            str.push_back('\t');

            lua_pop(L, 1); /* pop result */
        }
        self->mResults.push_back(std::move(str));
        return 0;
    };
    lua_pushcclosure(mLua, execLog, 1);
    lua_pushvalue(mLua, -1);
    lua_setfield(mLua, -3, "log_trace");
    lua_pushvalue(mLua, -1);
    lua_setfield(mLua, -3, "log_info");
    lua_pushvalue(mLua, -1);
    lua_setfield(mLua, -3, "log_debug");
    lua_pushvalue(mLua, -1);
    lua_setfield(mLua, -3, "log_warn");
    lua_setfield(mLua, -2, "log_error");
    lua_Debug ld;
    if (lua_getstack(mLua, mLevel, &ld)) {
        int32_t idx = 0;
        const char *name = nullptr;
        while ((name = lua_getlocal(mLua, &ld, ++idx)) != nullptr) {
            lua_setfield(mLua, -2, name);
        }

        if (!lua_getinfo(mLua, "f", &ld)) {
            mResults.push_back("Get function info failed!!!");
            return ErrorLuaOperatorFailed;
        }
        idx = 0;
        while ((name = lua_getupvalue(mLua, -1, ++idx)) != nullptr) {
            lua_setfield(mLua, -3, name);
        }
        lua_pop(mLua, 1);  // pop f
    }

    lua_setfenv(mLua, -2);

    if (lua_pcall(mLua, 0, LUA_MULTRET, 0)) {
        const char *errMsg = lua_tostring(mLua, -1);
        mResults.push_back(errMsg);
        return ErrorLuaPcallFailed;
    }
    return Success;
}

const std::vector<std::string> &LuaRunner::get_results() const { return mResults; }

std::vector<std::string> LuaRunner::pop_results() { return std::move(mResults); }

}  // namespace PureLua