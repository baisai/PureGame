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

#include "PureCore/OsHelper.h"
#include "PureLua/LuaHelper.h"
#include "PureLua/LuaProfiler.h"
#include "PureLua/LuaStateProtect.h"
#include "PureLua/LuaErrorDesc.h"
#include "PureLua/PureLuaLog.h"

#include <string.h>

namespace PureLua {
//////////////////////////////////////////////////////////////////
/// LuaFuncProfiler
//////////////////////////////////////////////////////////////////
size_t lua_func_info_hash(const LuaFuncInfo& info) {
    size_t value = std::hash<std::string>()(info.mSource);
    value ^= std::hash<int>()(info.mLine) + 0x9e3779b9 + (value << 6) + (value >> 2);
    value ^= std::hash<std::string>()(info.mName) + 0x9e3779b9 + (value << 6) + (value >> 2);
    value ^= std::hash<std::string>()(info.mWhat) + 0x9e3779b9 + (value << 6) + (value >> 2);
    value ^= std::hash<bool>()(info.mIsTail) + 0x9e3779b9 + (value << 6) + (value >> 2);
    return value;
}

void lua_func_info_json(const LuaFuncInfo& info, std::string& jsonStr) {
    jsonStr.push_back('{');
    fmt::format_to(std::back_inserter(jsonStr),
                   "\"Source\":\"{}\",\"Line\":\"{}\",\"Name\":\"{}\",\"What\":"
                   "\"{}\",\"IsTail\":\"{}\"",
                   info.mSource, info.mLine, info.mName, info.mWhat, info.mIsTail);
    jsonStr.push_back('}');
}

void lua_call_info_json(const LuaCallInfo& info, std::string& jsonStr) {
    jsonStr.push_back('{');
    jsonStr.append("\"FuncInfo\":");
    lua_func_info_json(info.mFuncInfo, jsonStr);
    fmt::format_to(std::back_inserter(jsonStr), ",\"TotalCost\":\"{}\",\"SelfCost\":\"{}\",\"Times\":\"{}\"", info.mTotalCost, info.mSelfCost, info.mTimes);
    jsonStr.push_back('}');
}

void lua_call_parent_json(const LuaCallNode& parent, std::string& jsonStr) {
    jsonStr.push_back('{');
    jsonStr.append("\"CallInfo\":");
    lua_call_info_json(parent.mCallInfo, jsonStr);
    fmt::format_to(std::back_inserter(jsonStr), ",\"CallSource\":\"{}\",\"CallLine\":\"{}\",", parent.mCallSource, parent.mCallLine);
    jsonStr.append("\"Parent\":");
    if (parent.mParent == nullptr) {
        jsonStr.append("null");
    } else {
        lua_call_parent_json(*parent.mParent, jsonStr);
    }
    jsonStr.push_back('}');
}

void lua_call_node_json(const LuaCallNode& node, std::string& jsonStr) {
    jsonStr.push_back('{');
    jsonStr.append("\"CallInfo\":");
    lua_call_info_json(node.mCallInfo, jsonStr);
    fmt::format_to(std::back_inserter(jsonStr), ",\"CallSource\":\"{}\",\"CallLine\":\"{}\",", node.mCallSource, node.mCallLine);
    jsonStr.append("\"Parent\":");
    if (node.mParent == nullptr) {
        jsonStr.append("null");
    } else {
        lua_call_parent_json(*node.mParent, jsonStr);
    }
    jsonStr.append(",\"SubCall\":");
    if (node.mSubCall.empty()) {
        jsonStr.append("null");
    } else {
        jsonStr.push_back('[');
        for (size_t idx = 0; idx < node.mSubCall.size(); ++idx) {
            lua_call_node_json(*node.mSubCall[idx], jsonStr);
            jsonStr.push_back(',');
        }
        if (jsonStr.back() == ',') {
            jsonStr.back() = ']';
        } else {
            jsonStr.push_back(']');
        }
    }
    jsonStr.push_back('}');
}

LuaFuncProfiler::~LuaFuncProfiler() { close(); }

int LuaFuncProfiler::init(lua_State* L) {
    if (L == nullptr) {
        return ErrorLuaStateNullptr;
    }
    close();
    mLua = L;
    lua_pushlightuserdata(mLua, (void*)LuaFuncProfiler::get_profiler);
    lua_pushlightuserdata(mLua, this);
    lua_rawset(mLua, LUA_REGISTRYINDEX);

    add_interface("attach", profiler_attach);
    add_interface("deattach", profiler_deattach);
    add_interface("valid", profiler_valid);
    add_interface("start", profiler_start);
    add_interface("stop", profiler_stop);
    add_interface("pause", profiler_pause);
    add_interface("resume", profiler_resume);
    add_interface("clear", profiler_clear);
    add_interface("get_detail", profiler_get_detail);
    add_interface("get_collect", profiler_get_collect);
    add_interface("get_total_cost", profiler_get_total_cost);
    return Success;
}

void LuaFuncProfiler::close() {
    deattach();
    if (mLua) {
        lua_pushlightuserdata(mLua, (void*)LuaFuncProfiler::get_profiler);
        lua_pushnil(mLua);
        lua_rawset(mLua, LUA_REGISTRYINDEX);
        mLua = nullptr;
    }
}

bool LuaFuncProfiler::valid() const { return mLua != nullptr && mValid; }

bool LuaFuncProfiler::attach() {
    if (!mLua || lua_gethook(mLua) != nullptr) {
        return false;
    }
    deattach();
    mValid = true;
    lua_sethook(mLua, nullptr, 0, 0);
    return true;
}

void LuaFuncProfiler::deattach() {
    if (valid()) {
        stop();
        clear();
        mValid = false;
        lua_sethook(mLua, nullptr, 0, 0);
    }
}

int LuaFuncProfiler::start() {
    if (!valid()) {
        return ErrorLuaFuncProfilerNotAttach;
    }

    stop();
    clear();
    lua_sethook(mLua, LuaFuncProfiler::hook, LUA_MASKCALL | LUA_MASKRET, 0);
    mProfiling = true;
    return Success;
}

void LuaFuncProfiler::stop() {
    if (valid()) {
        lua_sethook(mLua, nullptr, 0, 0);
        mProfiling = false;
    }
}

int LuaFuncProfiler::pause() {
    if (mLua == nullptr) {
        return ErrorLuaFuncProfilerNotAttach;
    }
    if (!mProfiling) {
        return ErrorLuaFuncProfilerNotRunning;
    }
    mCalling = nullptr;
    while (!mCallingStack.empty()) {
        mCallingStack.pop();
    }
    mProfiling = false;
    return Success;
}

int LuaFuncProfiler::resume() {
    if (mLua == nullptr) {
        return ErrorLuaFuncProfilerNotAttach;
    }
    if (mProfiling) {
        return ErrorLuaFuncProfilerIsRunning;
    }

    mProfiling = true;
    return Success;
}

void LuaFuncProfiler::clear() {
    for (size_t i = 0; i < mCallDetail.size(); i++) {
        delete_call_node(mCallDetail[i]);
    }
    mCallDetail.clear();
    mCalling = nullptr;

    while (!mCallingStack.empty()) {
        mCallingStack.pop();
    }
    mTotalCost = 0;

    for (auto iter = mCallCollect.begin(); iter != mCallCollect.end(); ++iter) {
        delete iter->second;
    }
    mCallCollect.clear();
}

const LuaCallDetail& LuaFuncProfiler::get_detail() const { return mCallDetail; }

std::string LuaFuncProfiler::get_detail_json() const {
    std::string jsonStr("[");
    for (auto iter = mCallDetail.begin(); iter != mCallDetail.end(); ++iter) {
        lua_call_node_json(**iter, jsonStr);
        jsonStr.push_back(',');
    }
    if (jsonStr.back() == ',') {
        jsonStr.back() = ']';
    } else {
        jsonStr.push_back(']');
    }
    return jsonStr;
}

const LuaCallCollect& LuaFuncProfiler::get_collect() const { return mCallCollect; }

std::string LuaFuncProfiler::get_collect_json() {
    std::string jsonStr("[");
    for (auto iter = mCallCollect.begin(); iter != mCallCollect.end(); ++iter) {
        lua_call_info_json(*iter->second, jsonStr);
        jsonStr.push_back(',');
    }
    if (jsonStr.back() == ',') {
        jsonStr.back() = ']';
    } else {
        jsonStr.push_back(']');
    }
    return jsonStr;
}

long long LuaFuncProfiler::get_total_cost() const { return mTotalCost; }

int LuaFuncProfiler::profiler_attach(lua_State* L) {
    LuaFuncProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    bool res = pf->attach();
    lua_pushboolean(L, res ? 1 : 0);
    return 1;
}

int LuaFuncProfiler::profiler_deattach(lua_State* L) {
    LuaFuncProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    pf->deattach();
    lua_pushboolean(L, 1);
    return 1;
}

int LuaFuncProfiler::profiler_valid(lua_State* L) {
    LuaFuncProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_pushboolean(L, pf->valid() ? 1 : 0);
    return 1;
}

int LuaFuncProfiler::profiler_start(lua_State* L) {
    LuaFuncProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    int err = pf->start();
    lua_pushboolean(L, err == Success ? 1 : 0);
    return 1;
}

int LuaFuncProfiler::profiler_stop(lua_State* L) {
    LuaFuncProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    pf->stop();
    lua_pushboolean(L, 1);
    return 1;
}

int LuaFuncProfiler::profiler_pause(lua_State* L) {
    LuaFuncProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    int err = pf->pause();
    lua_pushboolean(L, err == Success ? 1 : 0);
    return 1;
}

int LuaFuncProfiler::profiler_resume(lua_State* L) {
    LuaFuncProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    int err = pf->resume();
    lua_pushboolean(L, err == Success ? 1 : 0);
    return 1;
}

int LuaFuncProfiler::profiler_clear(lua_State* L) {
    LuaFuncProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    pf->clear();
    lua_pushboolean(L, 1);
    return 1;
}

int LuaFuncProfiler::profiler_get_detail(lua_State* L) {
    LuaFuncProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushnil(L);
        return 1;
    }
    std::string jsonStr = pf->get_detail_json();
    lua_pushlstring(L, jsonStr.data(), jsonStr.size());
    return 1;
}

int LuaFuncProfiler::profiler_get_collect(lua_State* L) {
    LuaFuncProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushnil(L);
        return 1;
    }
    std::string jsonStr = pf->get_collect_json();
    lua_pushlstring(L, jsonStr.data(), jsonStr.size());
    return 1;
}

int LuaFuncProfiler::profiler_get_total_cost(lua_State* L) {
    LuaFuncProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushnil(L);
        return 1;
    }
    long long cost = pf->get_total_cost();
    lua_pushinteger(L, cost);
    return 1;
}

LuaFuncProfiler* LuaFuncProfiler::get_profiler(lua_State* L) {
    LuaStateProtect lsp(L);
    lua_pushlightuserdata(L, (void*)LuaFuncProfiler::get_profiler);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_islightuserdata(L, -1)) {
        return (LuaFuncProfiler*)lua_touserdata(L, -1);
    }
    return nullptr;
}

int LuaFuncProfiler::add_interface(PureCore::StringRef name, lua_CFunction fn) {
    if (mLua == nullptr) {
        return ErrorLuaStateNullptr;
    }
    LuaStateProtect lsp(mLua);
    lua_getglobal(mLua, "FuncProfiler");
    if (!lua_istable(mLua, -1)) {
        lua_createtable(mLua, 0, 8);
        lua_pushvalue(mLua, -1);
        lua_setglobal(mLua, "FuncProfiler");
    }
    lua_pushlstring(mLua, name.data(), name.size());
    lua_pushcclosure(mLua, fn, 0);
    lua_rawset(mLua, -3);
    return Success;
}

void LuaFuncProfiler::enter_call(lua_Debug* ar) {
    if (!mProfiling) {
        return;
    }

    if (mCalling) {
        auto& subCall = mCalling->mSubCall;
        LuaCallNode* parent = mCalling;
        mCalling = nullptr;
        for (size_t i = 0; i < subCall.size(); i++) {
            if (compare_func_info(subCall[i]->mCallInfo.mFuncInfo, ar)) {
                mCalling = subCall[i];
                break;
            }
        }
        if (!mCalling) {
            mCalling = new LuaCallNode;
            fill_call_node(mCalling, ar);
            mCalling->mParent = parent;
            subCall.push_back(mCalling);
        }
    } else {
        for (size_t i = 0; i < mCallDetail.size(); i++) {
            if (compare_func_info(mCallDetail[i]->mCallInfo.mFuncInfo, ar)) {
                mCalling = mCallDetail[i];
            }
        }
        if (!mCalling) {
            mCalling = new LuaCallNode;
            fill_call_node(mCalling, ar);
            mCallDetail.push_back(mCalling);
        }
    }

    mCallingStack.emplace();
    auto& info = mCallingStack.top().mFuncInfo;
    info.mSource = ar->source;
    info.mLine = ar->linedefined;
    info.mName = ar->name ? ar->name : "";
    info.mWhat = ar->what;
    info.mIsTail = ar->event == LUA_HOOKTAILCALL;
    mCallingStack.top().mStartTime = PureCore::steady_micro_s();
}

void LuaFuncProfiler::out_call(lua_Debug* ar) {
    if (mCalling && !mCallingStack.empty()) {
        auto& info = mCallingStack.top();
        if (ar == nullptr || compare_func_info(info.mFuncInfo, ar)) {
            long long cost = PureCore::steady_micro_s() - info.mStartTime;
            mCalling->mCallInfo.mTotalCost += cost;
            mCalling->mCallInfo.mSelfCost += cost;
            if (mCalling->mParent) {
                mCalling->mParent->mCallInfo.mSelfCost -= cost;
            }
            mCalling->mCallInfo.mTimes++;
            mCallingStack.pop();
            if (mCalling->mParent == nullptr) {
                mTotalCost += mCalling->mCallInfo.mTotalCost;
            }

            fill_collect(mCalling);
            mCalling = mCalling->mParent;
        }
        if (info.mFuncInfo.mIsTail) {
            out_call(nullptr);
        }
    }
}

void LuaFuncProfiler::fill_collect(const LuaCallNode* call) {
    size_t key = lua_func_info_hash(call->mCallInfo.mFuncInfo);
    auto iter = mCallCollect.find(key);
    if (iter == mCallCollect.end()) {
        mCallCollect.insert(std::make_pair(key, new LuaCallInfo(call->mCallInfo)));
    } else {
        iter->second->mTimes = call->mCallInfo.mTimes;
        iter->second->mSelfCost = call->mCallInfo.mSelfCost;
        iter->second->mTotalCost = call->mCallInfo.mTotalCost;
    }
}

void LuaFuncProfiler::fill_call_node(LuaCallNode* call, lua_Debug* ar) {
    call->mCallInfo.mFuncInfo.mSource = ar->source;
    call->mCallInfo.mFuncInfo.mLine = ar->linedefined;
    call->mCallInfo.mFuncInfo.mName = ar->name ? ar->name : "";
    call->mCallInfo.mFuncInfo.mWhat = ar->what;
    call->mCallInfo.mFuncInfo.mIsTail = ar->event == LUA_HOOKTAILCALL;
    lua_Debug preAr;
    if (lua_getstack(mLua, 1, &preAr) && lua_getinfo(mLua, "nSlf", &preAr)) {
        call->mCallSource = preAr.source;
        call->mCallLine = preAr.currentline;
    }
}

void LuaFuncProfiler::hook(lua_State* L, lua_Debug* ar) {
    LuaFuncProfiler* self = get_profiler(L);
    if (self) {
        lua_getinfo(L, "nSl", ar);
        if (ar->event == LUA_HOOKCALL || ar->event == LUA_HOOKTAILCALL) {
            self->enter_call(ar);
        } else if (ar->event == LUA_HOOKRET) {
            self->out_call(ar);
        }
    }
}

void LuaFuncProfiler::delete_call_node(LuaCallNode* node) {
    for (size_t i = 0; i < node->mSubCall.size(); i++) {
        delete_call_node(node->mSubCall[i]);
    }
    node->mSubCall.clear();
    delete node;
}

bool LuaFuncProfiler::compare_func_info(const LuaFuncInfo& info, lua_Debug* ar) {
    return (info.mName == (ar->name ? ar->name : "") && info.mWhat == ar->what && info.mSource == ar->source && info.mLine == ar->linedefined);
}

//////////////////////////////////////////////////////////////////
/// LuaMemProfiler
//////////////////////////////////////////////////////////////////
void lua_mem_detail_json(const LuaMemDetail& detail, std::string& jsonStr) {
    jsonStr.push_back('{');
    jsonStr.append("\"FuncInfo\":");
    lua_func_info_json(detail.mFuncInfo, jsonStr);
    fmt::format_to(std::back_inserter(jsonStr), ",\"Ptr\":\"{}\",\"Size\":\"{}\"", detail.mPtr, detail.mSize);
    jsonStr.push_back('}');
}

void lua_mem_collect_json(const LuaMemCollect& collect, std::string& jsonStr) {
    jsonStr.push_back('{');
    jsonStr.append("\"FuncInfo\":");
    lua_func_info_json(collect.mFuncInfo, jsonStr);
    fmt::format_to(std::back_inserter(jsonStr), ",\"Size\":\"{}\"", collect.mSize);
    jsonStr.push_back('}');
}

LuaMemProfiler::~LuaMemProfiler() { close(); }

int LuaMemProfiler::init(lua_State* L) {
    if (L == nullptr) {
        return ErrorLuaStateNullptr;
    }
    close();
    mLua = L;
    lua_pushlightuserdata(mLua, (void*)LuaMemProfiler::get_profiler);
    lua_pushlightuserdata(mLua, this);
    lua_rawset(mLua, LUA_REGISTRYINDEX);

    add_interface("attach", LuaMemProfiler::profiler_attach);
    add_interface("deattach", LuaMemProfiler::profiler_deattach);
    add_interface("valid", LuaMemProfiler::profiler_valid);
    add_interface("start", LuaMemProfiler::profiler_start);
    add_interface("stop", LuaMemProfiler::profiler_stop);
    add_interface("pause", LuaMemProfiler::profiler_pause);
    add_interface("resume", LuaMemProfiler::profiler_resume);
    add_interface("clear", LuaMemProfiler::profiler_clear);
    add_interface("get_detail", LuaMemProfiler::profiler_get_detail);
    add_interface("get_collect", LuaMemProfiler::profiler_get_collect);
    add_interface("get_total_memory", LuaMemProfiler::profiler_get_total_memory);
    return Success;
}

void LuaMemProfiler::close() {
    deattach();
    if (mLua) {
        lua_pushlightuserdata(mLua, (void*)LuaMemProfiler::get_profiler);
        lua_pushnil(mLua);
        lua_rawset(mLua, LUA_REGISTRYINDEX);
        mLua = nullptr;
    }
}

bool LuaMemProfiler::valid() const { return mLua != nullptr && mValid; }

bool LuaMemProfiler::attach() {
    if (!mLua) {
        return false;
    }
    deattach();
    mValid = true;
    mPreAlloc = lua_getallocf(mLua, &mPreAllocUd);
    return true;
}

void LuaMemProfiler::deattach() {
    if (mLua) {
        stop();
        clear();

        mValid = false;
        mPreAlloc = nullptr;
        mPreAllocUd = nullptr;
    }
}

int LuaMemProfiler::start() {
    if (mLua == nullptr) {
        return ErrorLuaMemProfilerNotAttach;
    }
    stop();
    clear();
    lua_setallocf(mLua, LuaMemProfiler::alloc, this);
    mProfiling = true;
    return Success;
}

void LuaMemProfiler::stop() {
    if (valid()) {
        void* ud = nullptr;
        auto af = lua_getallocf(mLua, &ud);
        if (af == LuaMemProfiler::alloc && this == ud) {
            lua_setallocf(mLua, mPreAlloc, mPreAllocUd);
        }
    }
    mProfiling = false;
}

int LuaMemProfiler::pause() {
    if (mLua == nullptr) {
        return ErrorLuaMemProfilerNotAttach;
    }
    if (!mProfiling) {
        return ErrorLuaMemProfilerNotRunning;
    }

    mProfiling = false;
    return Success;
}

int LuaMemProfiler::resume() {
    if (mLua == nullptr) {
        return ErrorLuaMemProfilerNotAttach;
    }
    if (mProfiling) {
        return ErrorLuaMemProfilerIsRunning;
    }

    mProfiling = true;
    return Success;
}

void LuaMemProfiler::clear() {
    mTotalMemory = 0;
    for (auto iter = mMemoryDetail.begin(); iter != mMemoryDetail.end(); ++iter) {
        delete iter->second;
    }
    mMemoryDetail.clear();
    for (auto iter = mMemoryCollect.begin(); iter != mMemoryCollect.end(); ++iter) {
        delete iter->second;
    }
    mMemoryCollect.clear();
}

const MemoryDetail& LuaMemProfiler::get_detail() const { return mMemoryDetail; }

std::string LuaMemProfiler::get_detail_json() const {
    std::string jsonStr("[");
    for (auto iter = mMemoryDetail.begin(); iter != mMemoryDetail.end(); ++iter) {
        lua_mem_detail_json(*iter->second, jsonStr);
        jsonStr.push_back(',');
    }
    if (jsonStr.back() == ',') {
        jsonStr.back() = ']';
    } else {
        jsonStr.push_back(']');
    }
    return jsonStr;
}

const MemoryCollect& LuaMemProfiler::get_collect() const { return mMemoryCollect; }

std::string LuaMemProfiler::get_collect_json() const {
    std::string jsonStr("[");
    for (auto iter = mMemoryCollect.begin(); iter != mMemoryCollect.end(); ++iter) {
        lua_mem_collect_json(*iter->second, jsonStr);
        jsonStr.push_back(',');
    }
    if (jsonStr.back() == ',') {
        jsonStr.back() = ']';
    } else {
        jsonStr.push_back(']');
    }
    return jsonStr;
}

size_t LuaMemProfiler::get_total_memory() const { return mTotalMemory; }

int LuaMemProfiler::profiler_attach(lua_State* L) {
    LuaMemProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    bool res = pf->attach();
    lua_pushboolean(L, res ? 1 : 0);
    return 1;
}

int LuaMemProfiler::profiler_deattach(lua_State* L) {
    LuaMemProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    pf->deattach();
    lua_pushboolean(L, 1);
    return 1;
}

int LuaMemProfiler::profiler_valid(lua_State* L) {
    LuaMemProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_pushboolean(L, pf->valid() ? 1 : 0);
    return 1;
}

int LuaMemProfiler::profiler_start(lua_State* L) {
    LuaMemProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    int err = pf->start();
    lua_pushboolean(L, err == Success ? 1 : 0);
    return 1;
}

int LuaMemProfiler::profiler_stop(lua_State* L) {
    LuaMemProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    pf->stop();
    lua_pushboolean(L, 1);
    return 1;
}

int LuaMemProfiler::profiler_pause(lua_State* L) {
    LuaMemProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    int err = pf->pause();
    lua_pushboolean(L, err == Success ? 1 : 0);
    return 1;
}

int LuaMemProfiler::profiler_resume(lua_State* L) {
    LuaMemProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    int err = pf->resume();
    lua_pushboolean(L, err == Success ? 1 : 0);
    return 1;
}

int LuaMemProfiler::profiler_clear(lua_State* L) {
    LuaMemProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    pf->clear();
    lua_pushboolean(L, 1);
    return 1;
}

int LuaMemProfiler::profiler_get_detail(lua_State* L) {
    LuaMemProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushnil(L);
        return 1;
    }
    std::string jsonStr = pf->get_detail_json();
    lua_pushlstring(L, jsonStr.data(), jsonStr.size());
    return 1;
}

int LuaMemProfiler::profiler_get_collect(lua_State* L) {
    LuaMemProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushnil(L);
        return 1;
    }
    std::string jsonStr = pf->get_collect_json();
    lua_pushlstring(L, jsonStr.data(), jsonStr.size());
    return 1;
}

int LuaMemProfiler::profiler_get_total_memory(lua_State* L) {
    LuaMemProfiler* pf = get_profiler(L);
    if (pf == nullptr) {
        lua_pushnil(L);
        return 1;
    }
    long long cost = pf->get_total_memory();
    lua_pushinteger(L, cost);
    return 1;
}

LuaMemProfiler* LuaMemProfiler::get_profiler(lua_State* L) {
    LuaStateProtect lsp(L);
    lua_pushlightuserdata(L, (void*)LuaMemProfiler::get_profiler);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_islightuserdata(L, -1)) {
        return (LuaMemProfiler*)lua_touserdata(L, -1);
    }
    return nullptr;
}

int LuaMemProfiler::add_interface(PureCore::StringRef name, lua_CFunction fn) {
    if (mLua == nullptr) {
        return ErrorLuaStateNullptr;
    }
    LuaStateProtect lsp(mLua);
    lua_getglobal(mLua, "MemProfiler");
    if (!lua_istable(mLua, -1)) {
        lua_createtable(mLua, 0, 8);
        lua_pushvalue(mLua, -1);
        lua_setglobal(mLua, "MemProfiler");
    }
    lua_pushlstring(mLua, name.data(), name.size());
    lua_pushcclosure(mLua, fn, 0);
    lua_rawset(mLua, -3);
    return Success;
}

void LuaMemProfiler::alloc_memory(void* ptr, size_t size) {
    if (!mLua || !mProfiling || !ptr) {
        return;
    }

    auto memInfo = new LuaMemDetail;
    memInfo->mPtr = ptr;
    memInfo->mSize = size;
    fill_func_info(memInfo->mFuncInfo);
    fill_collect(*memInfo, true);
    mMemoryDetail.insert(std::make_pair(ptr, memInfo));
    mTotalMemory += size;
}

void LuaMemProfiler::free_memory(void* ptr, size_t osize) {
    if (!mLua || !ptr) {
        return;
    }

    auto iter = mMemoryDetail.find(ptr);
    if (iter != mMemoryDetail.end()) {
        fill_collect(*iter->second, false);
        mTotalMemory -= iter->second->mSize;
        delete iter->second;
        mMemoryDetail.erase(iter);
    }
}

void LuaMemProfiler::fill_func_info(LuaFuncInfo& info) {
    if (!mLua) {
        return;
    }

    std::string firstCName = "[C]";
    for (int i = 0;; i++) {
        lua_Debug ar;
        LuaStateProtect lsp(mLua);
        if (lua_getstack(mLua, i, &ar) && lua_getinfo(mLua, "nSlft", &ar)) {
            if (strcmp(ar.what, "C") == 0) {
                if (ar.name) {
                    firstCName += ":";
                    firstCName += ar.name;
                }
            }

            if (strcmp(ar.what, "Lua") == 0 || strcmp(ar.what, "main") == 0) {
                info.mSource = ar.source;
                info.mLine = ar.currentline;
                info.mName = ar.name ? ar.name : "";
                info.mWhat = ar.what;
                info.mIsTail = ar.istailcall != 0;
                return;
            }
        } else {
            break;
        }
    }

    info.mSource = firstCName;
    info.mLine = -1;
    info.mName = firstCName;
    info.mWhat = "C";
    info.mIsTail = false;
}

void LuaMemProfiler::fill_collect(const LuaMemDetail& detail, bool isAlloc) {
    size_t key = lua_func_info_hash(detail.mFuncInfo);
    auto iter = mMemoryCollect.find(key);
    if (iter == mMemoryCollect.end()) {
        auto collect = new LuaMemCollect;
        collect->mFuncInfo = detail.mFuncInfo;
        auto result = mMemoryCollect.insert(std::make_pair(key, collect));
        if (!result.second) {
            return;
        }
        iter = result.first;
    }
    if (isAlloc) {
        iter->second->mSize += detail.mSize;
    } else {
        iter->second->mSize -= detail.mSize;
    }
}

void* LuaMemProfiler::alloc(void* ud, void* ptr, size_t osize, size_t nsize) {
    LuaMemProfiler* self = (LuaMemProfiler*)ud;
    if (self) {
        self->free_memory(ptr, osize);
        ptr = self->mPreAlloc ? (*self->mPreAlloc)(self->mPreAllocUd, ptr, osize, nsize) : realloc(ptr, nsize);
        self->alloc_memory(ptr, nsize);
        return ptr;
    }
    return realloc(ptr, nsize);
}

}  // namespace PureLua
