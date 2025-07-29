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

#include "PureCore/StringRef.h"
#include "PureLua/PureLuaLib.h"
#include "lua.hpp"

#include <vector>
#include <stack>
#include <string>
#include <unordered_map>

namespace PureLua {
struct PURELUA_API LuaFuncInfo {
    std::string mSource;
    int mLine = -1;
    std::string mName;
    std::string mWhat;
    bool mIsTail = false;
};

struct PURELUA_API LuaCalling {
    LuaFuncInfo mFuncInfo;
    long long mStartTime = 0;
};

struct PURELUA_API LuaCallInfo {
    LuaFuncInfo mFuncInfo;
    long long mTotalCost = 0;
    long long mSelfCost = 0;
    long long mTimes = 0;
};

struct PURELUA_API LuaCallNode {
    LuaCallInfo mCallInfo;

    std::string mCallSource;
    int mCallLine = -1;

    LuaCallNode* mParent = nullptr;
    std::vector<LuaCallNode*> mSubCall;
};

typedef std::vector<LuaCallNode*> LuaCallDetail;
typedef std::unordered_map<size_t, LuaCallInfo*> LuaCallCollect;

class PURELUA_API LuaFuncProfiler {
public:
    LuaFuncProfiler() = default;
    ~LuaFuncProfiler();

    int init(lua_State* L);
    void close();

    bool valid() const;

    bool attach();
    void deattach();
    int start();
    void stop();
    int pause();
    int resume();
    void clear();

    const LuaCallDetail& get_detail() const;
    std::string get_detail_json() const;
    const LuaCallCollect& get_collect() const;
    std::string get_collect_json();
    long long get_total_cost() const;

private:
    static int profiler_attach(lua_State* L);
    static int profiler_deattach(lua_State* L);
    static int profiler_valid(lua_State* L);
    static int profiler_start(lua_State* L);
    static int profiler_stop(lua_State* L);
    static int profiler_pause(lua_State* L);
    static int profiler_resume(lua_State* L);
    static int profiler_clear(lua_State* L);
    static int profiler_get_detail(lua_State* L);
    static int profiler_get_collect(lua_State* L);
    static int profiler_get_total_cost(lua_State* L);
    static LuaFuncProfiler* get_profiler(lua_State* L);

private:
    int add_interface(PureCore::StringRef name, lua_CFunction fn);

    void enter_call(lua_Debug* ar);
    void out_call(lua_Debug* ar);
    void fill_collect(const LuaCallNode* call);
    void fill_call_node(LuaCallNode* call, lua_Debug* ar);

    static void hook(lua_State* L, lua_Debug* ar);
    static void delete_call_node(LuaCallNode* node);
    static bool compare_func_info(const LuaFuncInfo& info, lua_Debug* ar);

private:
    lua_State* mLua = nullptr;
    bool mValid = false;
    bool mProfiling = false;

    LuaCallDetail mCallDetail;
    LuaCallNode* mCalling = nullptr;
    std::stack<LuaCalling> mCallingStack;
    long long mTotalCost = 0;
    LuaCallCollect mCallCollect;

    PURE_DISABLE_COPY(LuaFuncProfiler)
};

struct LuaMemDetail {
    LuaFuncInfo mFuncInfo;
    void* mPtr = nullptr;
    long long mSize = 0;
};
struct LuaMemCollect {
    LuaFuncInfo mFuncInfo;
    long long mSize = 0;
};

typedef std::unordered_map<void*, LuaMemDetail*> MemoryDetail;
typedef std::unordered_map<size_t, LuaMemCollect*> MemoryCollect;

class PURELUA_API LuaMemProfiler {
public:
    LuaMemProfiler() = default;
    ~LuaMemProfiler();

    int init(lua_State* L);
    void close();

    bool valid() const;

    bool attach();
    void deattach();
    int start();
    void stop();
    int pause();
    int resume();
    void clear();

    const MemoryDetail& get_detail() const;
    std::string get_detail_json() const;
    const MemoryCollect& get_collect() const;
    std::string get_collect_json() const;
    size_t get_total_memory() const;

    static int profiler_attach(lua_State* L);
    static int profiler_deattach(lua_State* L);
    static int profiler_valid(lua_State* L);
    static int profiler_start(lua_State* L);
    static int profiler_stop(lua_State* L);
    static int profiler_pause(lua_State* L);
    static int profiler_resume(lua_State* L);
    static int profiler_clear(lua_State* L);
    static int profiler_get_detail(lua_State* L);
    static int profiler_get_collect(lua_State* L);
    static int profiler_get_total_memory(lua_State* L);
    static LuaMemProfiler* get_profiler(lua_State* L);

private:
    int add_interface(PureCore::StringRef name, lua_CFunction fn);

    void alloc_memory(void* ptr, size_t size);
    void free_memory(void* ptr, size_t osize);
    void fill_func_info(LuaFuncInfo& info);
    void fill_collect(const LuaMemDetail& detail, bool isAlloc);

    static void* alloc(void* ud, void* ptr, size_t osize, size_t nsize);

private:
    lua_State* mLua = nullptr;
    lua_Alloc mPreAlloc = nullptr;
    void* mPreAllocUd = nullptr;
    bool mValid = false;
    bool mProfiling = false;

    size_t mTotalMemory = 0;
    MemoryDetail mMemoryDetail;
    MemoryCollect mMemoryCollect;

    PURE_DISABLE_COPY(LuaMemProfiler)
};
}  // namespace PureLua
