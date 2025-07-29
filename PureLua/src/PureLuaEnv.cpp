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

#include "PureCore/PureLog.h"
#include "PureCore/DataRef.h"
#include "PureLua/PureLuaEnv.h"
#include "PureLua/LuaPointerAdapter.h"
#include "PureLua/LuaStack.h"
#include "PureLua/LuaJson.h"
#include "PureLua/LuaXml.h"
#include "PureLua/LuaIterator.h"
#include "PureLua/LuaRunner.h"
#include "PureLua/PureLuaVersion.h"
#include "PureLua/BindAllPureCore.h"

namespace PureLua {
PureLuaEnv::PureLuaEnv() : mLua(nullptr) {}

PureLuaEnv::~PureLuaEnv() { close(); }

lua_State *PureLuaEnv::state() { return mLua; }

bool PureLuaEnv::valid() { return mLua != nullptr; }

template <typename T>
inline PureCore::MovePtr<T> create_std_move() {
    return PureCore::MovePtr<T>(new T());
}

int PureLuaEnv::init() {
    if (mLua) {
        return ErrorThisStateInited;
    }

    mLua = lua_newstate(PureLuaEnv::lua_allocator, this);
    if (!mLua) {
        return ErrorNewStateFailed;
    }
    LuaStateProtect lsp(mLua);

    lua_pushlightuserdata(mLua, (void *)PureLuaEnv::get_env);
    lua_pushlightuserdata(mLua, this);
    lua_rawset(mLua, LUA_REGISTRYINDEX);

    lua_atpanic(mLua, PureLuaEnv::lua_panic_catcher);
    luaL_openlibs(mLua);
    lua_gc(mLua, LUA_GCGEN);

    load_class_script(mLua);

    LuaClassMgr::get_class_mgr(mLua);

    int err = mDebugger.init(mLua);
    if (err != Success) {
        return err;
    }
    reset_debugger_attach();

    err = mFuncProfiler.init(mLua);
    if (err != Success) {
        return err;
    }
    reset_func_profiler_attach();

    err = mMemProfiler.init(mLua);
    if (err != Success) {
        return err;
    }
    reset_mem_profiler_attach();

    mDebugger.attach();

    replace_require();

    lua_rawgeti(mLua, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
    LuaRef _G(mLua, -1);
    lua_pop(mLua, 1);

    _G["print"] = LuaTypeNil();
    def(&lua_log_trace, "log_trace");
    def(&lua_log_info, "log_info");
    def(&lua_log_debug, "log_debug");
    def(&lua_log_warn, "log_warn");
    def(&lua_log_error, "log_error");
    def(&lua_log_view, "log_view");

#ifdef PURE_DEBUG
    _G["PURE_DEBUG"] = true;
#else
    _G["PURE_DEBUG"] = false;
#endif
#ifdef PURE_X64
    _G["PURE_X64"] = true;
#else
    _G["PURE_X64"] = false;
#endif

    bind_all_pure_core(mLua);

    luaopen_json(mLua);
    lua_setglobal(mLua, "json");

    luaopen_xml(mLua);
    lua_setglobal(mLua, "xml");

    LuaModule core(mLua, "PureCore");
    core.def(PureLua::get_error_desc, "get_error_desc")
        .def(get_major_version, "get_major_version")
        .def(get_minor_version, "get_minor_version")
        .def(get_micro_version, "get_micro_version")
        .def(get_version_num, "get_version_num")
        .def(get_version, "get_version")
        .def(get_author, "get_author")
        .def_const(PureLua::Success, "Success");
#define XX(code, desc) core.def_const(int(PureLua::code), #code) PURELUA_ERROR_MAP(XX)
#undef XX

    LuaModule lmPtr(mLua, "Ptr");
    register_lua_pointer<std::string>(lmPtr, "String");
#if PURE_CPP >= 201703L
    register_lua_pointer<std::string_view>(lmPtr, "StringView");
#endif
    register_lua_pointer<PureCore::StringRef>(lmPtr, "StringRef");
    register_lua_pointer<unsigned short>(lmPtr, "UShort");
    register_lua_pointer<long>(lmPtr, "Long");
    register_lua_pointer<int>(lmPtr, "Int");
    register_lua_pointer<float>(lmPtr, "Float");
    register_lua_pointer<ptrdiff_t>(lmPtr, "PtrDiffT");
    register_lua_pointer<unsigned int>(lmPtr, "UInt");
    register_lua_pointer<bool>(lmPtr, "Bool");
    register_lua_pointer<short>(lmPtr, "Short");
    register_lua_pointer<size_t>(lmPtr, "SizeT");
    register_lua_pointer<char>(lmPtr, "Char");
    register_lua_pointer<unsigned long>(lmPtr, "ULong");
    register_lua_pointer<signed char>(lmPtr, "SChar");
    register_lua_pointer<long long>(lmPtr, "LongLong");
    register_lua_pointer<unsigned long long>(lmPtr, "ULongLong");
    register_lua_pointer<double>(lmPtr, "Double");

    LuaModule lmStd(mLua, "std");
    LuaSTLRegister::inst()->try_register(lmStd);
    def(create_std_move<std::vector<unsigned short> >, "VectorUShort");
    def(create_std_move<std::vector<std::string> >, "VectorString");
    def(create_std_move<std::vector<long> >, "VectorLong");
    def(create_std_move<std::vector<int> >, "VectorInt");
    def(create_std_move<std::vector<float> >, "VectorFloat");
    def(create_std_move<std::vector<ptrdiff_t> >, "VectorPtrDiffT");
    def(create_std_move<std::vector<unsigned int> >, "VectorUInt");
    def(create_std_move<std::vector<bool> >, "VectorBool");
    def(create_std_move<std::vector<short> >, "VectorShort");
    def(create_std_move<std::vector<unsigned char> >, "VectorUChar");
    def(create_std_move<std::vector<size_t> >, "VectorSizeT");
    def(create_std_move<std::vector<char> >, "VectorChar");
    def(create_std_move<std::vector<unsigned long> >, "VectorULong");
    def(create_std_move<std::vector<signed char> >, "VectorSChar");
    def(create_std_move<std::vector<long long> >, "VectorLongLong");
    def(create_std_move<std::vector<unsigned long long> >, "VectorULongLong");
    def(create_std_move<std::vector<double> >, "VectorDouble");

    def(PureLuaEnv::lua_exec, "exec");
    return Success;
}

void PureLuaEnv::close() {
    mDebugger.close();
    mFuncProfiler.close();
    mMemProfiler.close();
    mRequire.clear();
    if (mLua) {
        LuaClassMgr *pMgr = LuaClassMgr::get_class_mgr(mLua);
        lua_close(mLua);
        mLua = nullptr;
        delete pMgr;
    }
}

LuaFuncProfiler &PureLuaEnv::get_func_profiler() { return mFuncProfiler; }

LuaMemProfiler &PureLuaEnv::get_mem_profiler() { return mMemProfiler; }

LuaDebugger &PureLuaEnv::get_debugger() { return mDebugger; }

const std::string &PureLuaEnv::get_last_error() const { return mLastError; }

void PureLuaEnv::allocator_gc(bool all) { mAllocator.gc(all); }

int PureLuaEnv::add_package_path(const std::string &path) {
    std::string new_path =
        "package = package or {};package.path = (package.path or \";./?.lua\") "
        ".. \"";
    if (path.empty()) {
        return ErrorStringEmpty;
    }

    if (path[0] != ';') {
        new_path.push_back(';');
    }

    new_path.append(path);
    new_path.append("\"");

    do_string(new_path.c_str());
    return Success;
}

void PureLuaEnv::clear_package_path() { do_string("package = package or {};package.path = \";./?.lua\""); }

void PureLuaEnv::add_require_loader(RequireLoader loader) { mLoader.push_back(loader); }

void PureLuaEnv::clear_require_loader() { mLoader.clear(); }

int PureLuaEnv::do_string(const char *chunk, int envIdx) {
    LuaStateProtect lsp(mLua);
    if (envIdx != 0) {
        envIdx = lua_absindex(mLua, envIdx);
    }
    int ret = Success;
    const char *errStr = nullptr;
    int errFunc = push_lua_error_catcher(mLua);
    do {
        if (luaL_loadstring(mLua, chunk)) {
            errStr = lua_tostring(mLua, -1);
            ret = ErrorLuaLoadFailed;
            break;
        }
        if (envIdx != 0) {
            lua_pushvalue(mLua, envIdx);
            if (!lua_setfenv(mLua, -2)) {
                errStr = "Set Function Env Failed";
                ret = ErrorLuaLoadFailed;
                break;
            }
        }
        mDebugger.clear_last_catch();
        if (lua_pcall(mLua, 0, LUA_MULTRET, errFunc)) {
            errStr = lua_tostring(mLua, -1);
            ret = mDebugger.get_last_catch();
            if (ret == Success) {
                ret = ErrorLuaPcallFailed;
            }
            break;
        }

    } while (false);

    if (errStr != nullptr) {
        mLastError = errStr;
    }
    return ret;
}

int PureLuaEnv::do_chunk(PureCore::StringRef chunk, const char *name, int envIdx) {
    LuaStateProtect lsp(mLua);
    if (envIdx != 0) {
        envIdx = lua_absindex(mLua, envIdx);
    }
    int ret = Success;
    const char *errStr = nullptr;
    int errFunc = push_lua_error_catcher(mLua);
    do {
        if (luaL_loadbuffer(mLua, chunk.data(), chunk.size(), name)) {
            errStr = lua_tostring(mLua, -1);
            ret = ErrorLuaLoadFailed;
            break;
        }
        if (envIdx != 0) {
            lua_pushvalue(mLua, envIdx);
            if (!lua_setfenv(mLua, -2)) {
                errStr = "Set Function Env Failed";
                ret = ErrorLuaLoadFailed;
                break;
            }
        }
        mDebugger.clear_last_catch();
        if (lua_pcall(mLua, 0, LUA_MULTRET, errFunc)) {
            errStr = lua_tostring(mLua, -1);
            ret = mDebugger.get_last_catch();
            if (ret == Success) {
                ret = ErrorLuaPcallFailed;
            }
            break;
        }
    } while (false);

    if (errStr != nullptr) {
        mLastError = errStr;
    }
    return ret;
}

int PureLuaEnv::do_file(const char *fileName, int envIdx) {
    LuaStateProtect lsp(mLua);
    if (envIdx != 0) {
        envIdx = lua_absindex(mLua, envIdx);
    }
    int ret = Success;
    const char *errStr = nullptr;
    int errFunc = push_lua_error_catcher(mLua);
    do {
        if (luaL_loadfile(mLua, fileName)) {
            errStr = lua_tostring(mLua, -1);
            ret = ErrorLuaLoadFailed;
            break;
        }
        if (envIdx != 0) {
            lua_pushvalue(mLua, envIdx);
            if (!lua_setfenv(mLua, -2)) {
                errStr = "Set Function Env Failed";
                ret = ErrorLuaLoadFailed;
                break;
            }
        }
        mDebugger.clear_last_catch();
        if (lua_pcall(mLua, 0, LUA_MULTRET, errFunc)) {
            errStr = lua_tostring(mLua, -1);
            ret = mDebugger.get_last_catch();
            if (ret == Success) {
                ret = ErrorLuaPcallFailed;
            }
            break;
        }
    } while (false);

    if (errStr != nullptr) {
        mLastError = errStr;
    }
    return ret;
}

int PureLuaEnv::require_file(const char *fileName) {
    LuaStateProtect lsp(mLua);
    int err = push_lua_error_catcher(mLua);
    lua_getglobal(mLua, "require");
    lua_pushstring(mLua, fileName);
    if (lua_pcall(mLua, 1, LUA_MULTRET, err)) {
        const char *errStr = lua_tostring(mLua, -1);
        if (errStr != nullptr) {
            mLastError = errStr;
        }
        return ErrorLuaPcallFailed;
    }
    return Success;
}

LuaRef PureLuaEnv::get_global_ref(const char *name) {
    LuaStateProtect lsp(mLua);
    lua_getglobal(mLua, name);
    return LuaRef(mLua, -1);
}

const char *PureLuaEnv::get_class_name(lua_State *L, int idx) {
    const char *className = "";
    if (lua_isuserdata(L, idx) && lua_getmetatable(L, idx)) {
        raw_getfield(L, -1, "cname__");
        if (lua_isstring(L, -1)) {
            className = lua_tostring(L, -1);
        }
        lua_pop(L, 2);
    }
    return className;
}

bool PureLuaEnv::load_class_script(lua_State *L) {
    LuaStateProtect lsp(L);
    const char *class_script =
        "local newer = function(cls, ...)\n"
        "    local obj = {}\n"
        "    setmetatable(obj, cls)\n"
        "    if cls.ctor__ ~= nil then\n"
        "        cls.ctor__(obj, ...)\n"
        "    end\n"
        "    return obj\n"
        "end\n"
        "function class__(BaseClass)\n"
        "    local LuaClass = {}\n"
        "    if BaseClass then\n"
        "        setmetatable(LuaClass, {__call = newer, __index = BaseClass })\n"
        "    else\n"
        "        setmetatable(LuaClass, {__call = newer })\n"
        "    end\n"
        "    LuaClass.__index = LuaClass\n"
        "    return LuaClass\n"
        "end\n";
    int err = push_lua_error_catcher(L);
    if (luaL_loadbuffer(L, class_script, strlen(class_script), "class__") || lua_pcall(L, 0, LUA_MULTRET, err)) {
        const char *err = lua_tostring(L, -1);
        PureError("Call PureLuaEnv::load_class_script Error!!! error is \"{}\"", err);
        return false;
    }
    return true;
}

int PureLuaEnv::push_lua_error_catcher(lua_State *L) {
    lua_pushcclosure(L, lua_error_catcher, 0);
    return lua_gettop(L);
}

void *PureLuaEnv::do_lua_allocator(void *ptr, size_t osize, size_t nsize) { return mAllocator.reallocate(ptr, osize, nsize); }

void PureLuaEnv::reset_attach(const char *tab, const char *name, lua_CFunction fn) {
    if (!valid()) {
        return;
    }
    LuaStateProtect lsp(mLua);
    lua_getglobal(mLua, tab);
    if (!lua_istable(mLua, -1)) {
        return;
    }
    lua_pushstring(mLua, name);
    lua_rawget(mLua, -2);
    if (!lua_iscfunction(mLua, -1)) {
        return;
    }
    lua_pop(mLua, 1);
    lua_pushstring(mLua, name);
    lua_pushcclosure(mLua, fn, 0);
    lua_rawset(mLua, -3);
}

void PureLuaEnv::reset_debugger_attach() {
    reset_attach("Debugger", "attach", [](lua_State *L) {
        PureLuaEnv *self = PureLuaEnv::get_env(L);
        if (self == nullptr) {
            lua_pushboolean(L, 0);
            return 1;
        }
        self->mFuncProfiler.deattach();
        self->mMemProfiler.deattach();
        bool res = self->mDebugger.attach();
        lua_pushboolean(L, res ? 1 : 0);
        return 1;
    });
}

void PureLuaEnv::reset_func_profiler_attach() {
    reset_attach("FunProfiler", "attach", [](lua_State *L) {
        PureLuaEnv *self = PureLuaEnv::get_env(L);
        if (self == nullptr) {
            lua_pushboolean(L, 0);
            return 1;
        }
        self->mDebugger.deattach();
        self->mMemProfiler.deattach();
        bool res = self->mFuncProfiler.attach();
        lua_pushboolean(L, res ? 1 : 0);
        return 1;
    });
}

void PureLuaEnv::reset_mem_profiler_attach() {
    reset_attach("MemProfiler", "attach", [](lua_State *L) {
        PureLuaEnv *self = PureLuaEnv::get_env(L);
        if (self == nullptr) {
            lua_pushboolean(L, 0);
            return 1;
        }
        self->mFuncProfiler.deattach();
        self->mDebugger.deattach();
        bool res = self->mMemProfiler.attach();
        lua_pushboolean(L, res ? 1 : 0);
        return 1;
    });
}

void PureLuaEnv::replace_require() {
    if (mRequire.is_function()) {
        return;
    }
    LuaStateProtect lsp(mLua);
    lua_getglobal(mLua, "require");
    if (lua_isfunction(mLua, -1)) {
        mRequire.reset(mLua, -1);
    }
    lua_pop(mLua, 1);

    lua_pushlightuserdata(mLua, (void *)this);
    lua_pushcclosure(
        mLua,
        [](lua_State *L) -> int {
            PureLuaEnv *env = (PureLuaEnv *)lua_touserdata(L, lua_upvalueindex(1));
            const char *fileName = LuaStack<const char *>::get(L, 1);
            lua_getglobal(L, "package");
            if (lua_istable(L, -1)) {
                lua_getfield(L, -1, "loaded");
                if (lua_istable(L, -1)) {
                    lua_pushvalue(L, 1);
                    lua_gettable(L, -2);
                    if (!lua_isnoneornil(L, -1)) {
                        return 1;
                    }
                }
                lua_pop(L, 1);
            } else {
                lua_pop(L, 1);
                lua_createtable(L, 0, 0);
                lua_pushvalue(L, -1);
                lua_setglobal(L, "package");
            }
            PureCore::DynamicBuffer buffer;
            for (auto &loader : env->mLoader) {
                buffer.clear();
                if (loader(fileName, buffer)) {
                    auto data = buffer.data();
                    if (luaL_loadbuffer(L, data.data(), data.size(), fileName)) {
                        const char *err = lua_tostring(L, -1);
                        PureLuaErrorJump(L, err);
                        return 0;
                    }
                    lua_call(L, 0, 1);
                    if (lua_isnoneornil(L, -1)) {
                        lua_pop(L, 1);
                        lua_pushboolean(L, 1);
                    }
                    lua_pushvalue(L, -1);
                    lua_setfield(L, -3, fileName);
                    return 1;
                }
            }
            if (env->mRequire.push() && lua_isfunction(L, -1)) {
                lua_pushvalue(L, 1);
                lua_call(L, 1, 1);
                return 1;
            }
            PureLuaErrorJump(L, "require not found `{}`", fileName);
            return 0;
        },
        1);
    lua_setglobal(mLua, "require");
}

PureLuaEnv *PureLuaEnv::get_env(lua_State *L) {
    LuaStateProtect lsp(L);
    lua_pushlightuserdata(L, (void *)PureLuaEnv::get_env);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_islightuserdata(L, -1)) {
        return (PureLuaEnv *)lua_touserdata(L, -1);
    }
    return nullptr;
}

int PureLuaEnv::lua_panic_catcher(lua_State *L) {
    PureLuaEnv *self = PureLuaEnv::get_env(L);
    if (self != nullptr) {
        self->mLastError = lua_tostring(L, -1);
        self->get_debugger().catch_lua_error();
    }
    throw ErrorLuaPanic;
    return 1;
}

int PureLuaEnv::lua_error_catcher(lua_State *L) {
    PureLuaEnv *self = PureLuaEnv::get_env(L);
    if (self != nullptr) {
        self->mLastError = lua_tostring(L, -1);
        self->get_debugger().catch_lua_error();
    }
    return 1;
}

void *PureLuaEnv::lua_allocator(void *ud, void *ptr, size_t osize, size_t nsize) {
    PureLuaEnv *p = (PureLuaEnv *)ud;
    return p->do_lua_allocator(ptr, osize, nsize);
}

int PureLuaEnv::lua_log(lua_State *L, spdlog::level::level_enum level) {
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
    PureCore::PureLogger::inst()->log(level, str);
    return 0;
}

int PureLuaEnv::lua_log_trace(lua_State *L) { return PureLuaEnv::lua_log(L, spdlog::level::trace); }
int PureLuaEnv::lua_log_debug(lua_State *L) { return PureLuaEnv::lua_log(L, spdlog::level::debug); }
int PureLuaEnv::lua_log_info(lua_State *L) { return PureLuaEnv::lua_log(L, spdlog::level::info); }
int PureLuaEnv::lua_log_warn(lua_State *L) { return PureLuaEnv::lua_log(L, spdlog::level::warn); }
int PureLuaEnv::lua_log_error(lua_State *L) { return PureLuaEnv::lua_log(L, spdlog::level::err); }

static std::string get_field(lua_State *L, int objIdx);
static std::string get_obj_fields(lua_State *L, int objIdx, int tabIdx, int32_t indexLevel) {
    objIdx = lua_absindex(L, objIdx);
    tabIdx = lua_absindex(L, tabIdx);
    std::string info;
    if (lua_istable(L, tabIdx)) {
        LuaStateProtect lsp(L);
        for (LuaIterator iter(L, tabIdx); !iter.is_over(); ++iter) {
            if (lua_isuserdata(L, iter.key_idx())) {
                continue;
            }
            bool callFunc = false;
            if (lua_iscfunction(L, iter.value_idx())) {
                const char *t = lua_tostring(L, iter.key_idx());
                auto f = lua_tocfunction(L, iter.value_idx());
                if (f == LuaMetatable::mt_property_getset_function || f == LuaMetatable::mt_static_getset_function) {
                    callFunc = true;
                }
            }
            if (!callFunc && lua_isfunction(L, iter.value_idx())) {
                continue;
            }

            std::string key = get_field(L, iter.key_idx());
            if (key == "__index" || key == "cname__") {
                continue;
            }

            if (callFunc) {
                iter.push_value();
                lua_pushvalue(L, objIdx);
                lua_call(L, 1, 1);  // property
            } else {
                iter.push_value();  // value
            }
            std::string val = get_field(L, -1);
            info.append(key);
            info.push_back('=');
            info.append(val);
            info.push_back(',');
        }
    }

    LuaStateProtect lsp(L);
    if (lua_getmetatable(L, tabIdx)) {
        lua_pushstring(L, "__index");
        lua_rawget(L, -2);
        if (lua_istable(L, -1)) {
            auto obj = get_obj_fields(L, objIdx, -1, indexLevel + 1);
            if (!obj.empty()) {
                fmt::format_to(std::back_inserter(info), "__index[{}]={{{}}},", indexLevel + 1, obj);
            }
        }
    }
    return std::move(info);
}
static std::string get_field(lua_State *L, int objIdx) {
    objIdx = lua_absindex(L, objIdx);
    LuaStateProtect lsp(L);
    std::string info;
    if (lua_istable(L, objIdx) || lua_isuserdata(L, objIdx)) {
        info.push_back('{');
        auto obj = get_obj_fields(L, objIdx, objIdx, 0);
        info.append(obj);
        info.push_back('}');
    } else {
        lua_getglobal(L, "tostring");
        lua_pushvalue(L, objIdx);
        lua_call(L, 1, 1);
        const char *var = lua_tostring(L, -1);
        if (var == nullptr) {
            info.append("nil");
        } else {
            info.append(var);
        }
    }
    return std::move(info);
}
int PureLuaEnv::lua_log_view(lua_State *L) {
    int n = lua_gettop(L); /* number of arguments */
    for (int i = 1; i <= n; ++i) {
        auto str = get_field(L, i);
        lua_pushlstring(L, str.data(), str.size());
    }
    return n;
}

int PureLuaEnv::lua_exec(lua_State *L) {
    PureCore::StringRef code = LuaStack<PureCore::StringRef>::get(L, 1);
    LuaRunner exec(L);
    int err = exec.exec(std::string(code.data(), code.size()));
    if (err != Success) {
        PureLuaErrorJump(L, "exec failed error `{}`", get_error_desc(err));
        return 0;
    }
    auto res = exec.pop_results();
    for (const auto &str : res) {
        PureInfo("{}", str);
    }
    return 0;
}

}  // namespace PureLua