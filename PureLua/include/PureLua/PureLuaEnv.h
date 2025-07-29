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

#include "PureCore/Memory/SmallAllocator.h"
#include "PureCore/Buffer/DynamicBuffer.h"
#include "PureLua/PureLuaLib.h"
#include "PureLua/LuaErrorDesc.h"
#include "PureLua/LuaProfiler.h"
#include "PureLua/LuaResult.h"
#include "PureLua/LuaStackPushArgs.h"
#include "PureLua/LuaRegisterClass.h"
#include "PureLua/LuaRegisterEnum.h"
#include "PureLua/LuaModule.h"
#include "PureLua/LuaDefTool.h"

namespace PureLua {
typedef std::function<bool(const char*, PureCore::DynamicBuffer&)> RequireLoader;
class PURELUA_API PureLuaEnv {
public:
    PureLuaEnv();
    ~PureLuaEnv();
    lua_State* state();
    bool valid();
    int init();
    void close();

    LuaFuncProfiler& get_func_profiler();
    LuaMemProfiler& get_mem_profiler();
    LuaDebugger& get_debugger();
    const std::string& get_last_error() const;

    void allocator_gc(bool all);
    int add_package_path(const std::string& path);
    void clear_package_path();
    void add_require_loader(RequireLoader loader);
    void clear_require_loader();
    int do_string(const char* chunk, int envIdx = 0);
    int do_chunk(PureCore::StringRef chunk, const char* name, int envIdx = 0);
    int do_file(const char* fileName, int envIdx = 0);
    int require_file(const char* fileName);

    template <typename T>
    inline void def(T obj, const char* name) {
        LuaDefAutoTool::def(mLua, obj, name);
    }

    LuaRef get_global_ref(const char* name);

    static const char* get_class_name(lua_State* L, int idx);

    static bool load_class_script(lua_State* L);

    static int push_lua_error_catcher(lua_State* L);

private:
    void* do_lua_allocator(void* ptr, size_t osize, size_t nsize);

    void reset_attach(const char* tab, const char* name, lua_CFunction fn);
    void reset_debugger_attach();
    void reset_func_profiler_attach();
    void reset_mem_profiler_attach();

    void replace_require();

private:
    static PureLuaEnv* get_env(lua_State* L);
    static int lua_panic_catcher(lua_State* L);
    static int lua_error_catcher(lua_State* L);
    static void* lua_allocator(void* ud, void* ptr, size_t osize, size_t nsize);
    static int lua_log(lua_State* L, spdlog::level::level_enum level);
    static int lua_log_trace(lua_State* L);
    static int lua_log_debug(lua_State* L);
    static int lua_log_info(lua_State* L);
    static int lua_log_warn(lua_State* L);
    static int lua_log_error(lua_State* L);
    static int lua_log_view(lua_State* L);
    static int lua_exec(lua_State* L);

private:
    lua_State* mLua;
    std::string mLastError;
    LuaDebugger mDebugger;
    PureCore::SmallAllocator mAllocator;
    std::vector<RequireLoader> mLoader;
    LuaRef mRequire;
    LuaFuncProfiler mFuncProfiler;
    LuaMemProfiler mMemProfiler;

public:
    template <typename T>
    inline int open_module(T m) {
        m(mLua);
        return Success;
    }

    template <typename T>
    inline typename LuaStack<T>::GetType get_global_var(const char* name) {
        LuaStateProtect lsp(mLua);
        lua_getglobal(mLua, name);
        return LuaStack<T>::get(mLua, -1);
    }

    template <typename T>
    inline void set_global_var(const char* name, T var) {
        LuaStateProtect lsp(mLua);
        LuaStack<T>::push(mLua, var);
        lua_setglobal(mLua, name);
    }

    template <typename... Args>
    LuaResult<LuaRef> call(const char* name, Args&&... args) const {
        LuaStateProtect lsp(mLua);
        int err = push_lua_error_catcher(mLua);
        lua_getglobal(mLua, name);
        int s = lua_stack_push_args(mLua, std::forward<Args>(args)...);
        if (lua_pcall(mLua, s, 1, err)) {
            const char* errStr = lua_tostring(mLua, -1);
            if (errStr != nullptr) {
                return LuaResult<LuaRef>(errStr);
            }
            return LuaResult<LuaRef>(get_error_desc(ErrorLuaPcallFailed));
        }
        return LuaResult<LuaRef>(mLua, -1);
    }
};
}  // namespace PureLua
