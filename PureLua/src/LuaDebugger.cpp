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
#include "PureLua/LuaErrorDesc.h"
#include "PureLua/LuaDebugger.h"
#include "PureLua/LuaStateProtect.h"
#include "PureLua/LuaModule.h"
#include "PureLua/LuaIterator.h"
#include "PureLua/LuaRunner.h"

#include <algorithm>

namespace PureLua {
static bool check_digit(const std::string &str) {
    for (auto c : str) {
        if (!std::isdigit((unsigned char)(c))) {
            return false;
        }
    }
    return !str.empty();
}

static void trim_str(std::string &str) {
    if (str.empty()) return;
    for (size_t i = str.size(); i > 0u;) {
        --i;
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\r' && str[i] != '\n') {
            str.resize(i + 1);
            break;
        }
    }
    if (str.empty()) return;
    char back = str.back();
    if (back == ' ' || back == '\t' || back == '\r' || back == '\n') {
        str.clear();
        return;
    }
    for (size_t i = 0u; i < str.size(); ++i) {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\r' && str[i] != '\n') {
            str.assign(str.substr(i));
            break;
        }
    }
}

static bool split_fields(const std::string &varName, std::vector<std::string> &fields) {
    size_t startIdx = 0u, endIdx = varName.size();
    while ((endIdx = varName.find_first_of(".[", startIdx)) != std::string::npos) {
        if (endIdx - startIdx == 0) {
            ++startIdx;
            continue;
        }
        fields.push_back(varName.substr(startIdx, endIdx - startIdx));
        trim_str(fields.back());
        startIdx = endIdx + 1u;

        if (varName[endIdx] == '[') {
            int subCount = 1;
            size_t subStartIdx = startIdx, subEndIdx = endIdx;
            while (subCount > 0 && (subEndIdx = varName.find_first_of("[]", subStartIdx)) != std::string::npos) {
                subStartIdx = subEndIdx + 1u;
                if (varName[subEndIdx] == '[')
                    ++subCount;
                else
                    --subCount;
            }
            if (subCount > 0) return false;
            fields.push_back(varName.substr(endIdx, subEndIdx - endIdx + 1u));
            trim_str(fields.back());
            startIdx = subEndIdx + 1u;
        }
    }
    if (startIdx < varName.size() && endIdx > startIdx) {
        fields.push_back(varName.substr(startIdx, endIdx - startIdx));
        trim_str(fields.back());
    }
    return true;
}

static bool get_bool(const std::string &str, bool &ret) {
    if (str.size() != 1 && str.size() != 4) {
        return false;
    }
    std::string boolStr;
    boolStr.reserve(str.size());
    for (auto c : str) {
        boolStr.push_back(std::tolower(c));
    }
    if (boolStr == "1" || boolStr == "t" || boolStr == "true") {
        ret = true;
        return true;
    } else if (boolStr == "0" || boolStr == "f" || boolStr == "false") {
        ret = false;
        return true;
    }
    return false;
}

LuaDebugger::LuaDebugger() {
    mCmdFunc["help"] = [this](const std::string &params) { return cmd_help(params); };
    mCmdFunc["h"] = [this](const std::string &params) { return cmd_help(params); };
    mCmdFunc["offldb"] = [this](const std::string &params) { return cmd_offldb(params); };
    mCmdFunc["ol"] = [this](const std::string &params) { return cmd_offldb(params); };
    mCmdFunc["offerr"] = [this](const std::string &params) { return cmd_offerr(params); };
    mCmdFunc["oe"] = [this](const std::string &params) { return cmd_offerr(params); };
    mCmdFunc["frame"] = [this](const std::string &params) { return cmd_frame(params); };
    mCmdFunc["f"] = [this](const std::string &params) { return cmd_frame(params); };
    mCmdFunc["up"] = [this](const std::string &params) { return cmd_up(params); };
    mCmdFunc["u"] = [this](const std::string &params) { return cmd_up(params); };
    mCmdFunc["down"] = [this](const std::string &params) { return cmd_down(params); };
    mCmdFunc["d"] = [this](const std::string &params) { return cmd_down(params); };
    mCmdFunc["backtrace"] = [this](const std::string &params) { return cmd_backtrace(params); };
    mCmdFunc["bt"] = [this](const std::string &params) { return cmd_backtrace(params); };
    mCmdFunc["listbreak"] = [this](const std::string &params) { return cmd_listbreak(params); };
    mCmdFunc["lb"] = [this](const std::string &params) { return cmd_listbreak(params); };
    mCmdFunc["addbreak"] = [this](const std::string &params) { return cmd_addbreak(params); };
    mCmdFunc["ab"] = [this](const std::string &params) { return cmd_addbreak(params); };
    mCmdFunc["delbreak"] = [this](const std::string &params) { return cmd_delbreak(params); };
    mCmdFunc["db"] = [this](const std::string &params) { return cmd_delbreak(params); };
    mCmdFunc["clearbreak"] = [this](const std::string &params) { return cmd_clearbreak(params); };
    mCmdFunc["cb"] = [this](const std::string &params) { return cmd_clearbreak(params); };
    mCmdFunc["next"] = [this](const std::string &params) { return cmd_next(params); };
    mCmdFunc["n"] = [this](const std::string &params) { return cmd_next(params); };
    mCmdFunc["step"] = [this](const std::string &params) { return cmd_step(params); };
    mCmdFunc["s"] = [this](const std::string &params) { return cmd_step(params); };
    mCmdFunc["over"] = [this](const std::string &params) { return cmd_over(params); };
    mCmdFunc["o"] = [this](const std::string &params) { return cmd_over(params); };
    mCmdFunc["continue"] = [this](const std::string &params) { return cmd_continue(params); };
    mCmdFunc["c"] = [this](const std::string &params) { return cmd_continue(params); };
    mCmdFunc["findvar"] = [this](const std::string &params) { return cmd_findvar(params); };
    mCmdFunc["fv"] = [this](const std::string &params) { return cmd_findvar(params); };
    mCmdFunc["varfields"] = [this](const std::string &params) { return cmd_varfields(params); };
    mCmdFunc["vf"] = [this](const std::string &params) { return cmd_varfields(params); };
    mCmdFunc["localvars"] = [this](const std::string &params) { return cmd_localvars(params); };
    mCmdFunc["lv"] = [this](const std::string &params) { return cmd_localvars(params); };
    mCmdFunc["globalvars"] = [this](const std::string &params) { return cmd_globalvars(params); };
    mCmdFunc["gv"] = [this](const std::string &params) { return cmd_globalvars(params); };
    mCmdFunc["allvars"] = [this](const std::string &params) { return cmd_allvars(params); };
    mCmdFunc["av"] = [this](const std::string &params) { return cmd_allvars(params); };
    mCmdFunc["exec"] = [this](const std::string &params) { return cmd_exec(params); };
    mCmdFunc["e"] = [this](const std::string &params) { return cmd_exec(params); };
    mCmdFunc["reload"] = [this](const std::string &params) { return cmd_reload(params); };
    mCmdFunc["rl"] = [this](const std::string &params) { return cmd_reload(params); };
    mCmdFunc["list"] = [this](const std::string &params) { return cmd_list(params); };
    mCmdFunc["l"] = [this](const std::string &params) { return cmd_list(params); };
    mCmdFunc["cancel"] = [this](const std::string &params) { return cmd_cancel(params); };
    mCmdFunc["ca"] = [this](const std::string &params) { return cmd_cancel(params); };
}

LuaDebugger::~LuaDebugger() { close(); }

int LuaDebugger::init(lua_State *L) {
    if (L == nullptr) {
        return ErrorLuaStateNullptr;
    }
    close();
    mLua = L;
    lua_pushlightuserdata(mLua, (void *)LuaDebugger::get_debugger);
    lua_pushlightuserdata(mLua, this);
    lua_rawset(mLua, LUA_REGISTRYINDEX);

    add_interface("attach", debugger_attach);
    add_interface("deattach", debugger_deattach);
    add_interface("valid", debugger_valid);
    add_interface("ldb", debugger_ldb);
    add_interface("cmd", debugger_cmd);
    add_interface("exec", debugger_exec);
    add_interface("enable", debugger_enable);
    add_interface("enable_catch_error", debugger_enable_catch_error);
    return Success;
}

void LuaDebugger::close() {
    deattach();
    if (mLua) {
        lua_pushlightuserdata(mLua, (void *)LuaDebugger::get_debugger);
        lua_pushnil(mLua);
        lua_rawset(mLua, LUA_REGISTRYINDEX);
        mLua = nullptr;
    }
}

bool LuaDebugger::valid() const { return mLua != nullptr && mValid; }

bool LuaDebugger::attach() {
    if (!mLua || lua_gethook(mLua) != nullptr) {
        return false;
    }
    deattach();
    mValid = true;
    mBreakLevel = -1;
    mCurrentLevel = 0;
    mCoroutine = nullptr;
    mCorBreakLevel = -1;
    mCorCurrentLevel = 0;
    mMaxFrameCount = UINT8_MAX;
    return true;
}

void LuaDebugger::deattach() {
    if (valid()) {
        lua_sethook(mLua, nullptr, 0, 0);
        mValid = false;
    }
}

void LuaDebugger::clear_last_catch() { mLastCatchError = Success; }

int LuaDebugger::get_last_catch() { return mLastCatchError; }

void LuaDebugger::add_fix_path(const std::string &path) {
    auto end = mFixPaths.end();
    for (auto iter = mFixPaths.begin(); iter != end; ++iter) {
        if (*iter == path) {
            return;
        }
    }
    mFixPaths.push_back(path);
}

void LuaDebugger::remove_fix_path(const std::string &path) {
    auto end = mFixPaths.end();
    for (auto iter = mFixPaths.begin(); iter != end; ++iter) {
        if (*iter == path) {
            mFixPaths.erase(iter);
            return;
        }
    }
}

void LuaDebugger::clear_fix_path() { mFixPaths.clear(); }

void LuaDebugger::write_cmd(const std::string &cmd) { mCmds.push_back(cmd); }

std::vector<std::string> LuaDebugger::read_result() { return std::move(mResults); }

bool LuaDebugger::debug(bool wait) {
    if (!valid() || !is_enable() || mDebugging) {
        return false;
    }
    mDebugging = true;
    set_current_level(0);
    bool nextCmd = true;
    while (nextCmd) {
        if (mCmds.empty()) {
            if (wait) {
                block_read_cmd();
            } else {
                break;
            }
        }
        std::string cmd, params;
        if (!pop_cmd(cmd, params)) {
            continue;
        }

        if (cmd.empty()) {
            cmd = mLastCmd;
            params = mLastParams;
        }

        auto iter = mCmdFunc.find(cmd);
        if (iter != mCmdFunc.end()) {
            mLastCmd = cmd;
            mLastParams = params;
            try {
                nextCmd = iter->second(params);
                if (!wait) {
                    nextCmd = true;
                }
            } catch (EPureLuaErrorCode e) {
                mResults.clear();
                mResults.push_back(get_error_desc(e));
            }
        } else {
            mResults.push_back("invalid debug cmd");
        }
        for (auto const &res : mResults) {
            PureInfo("{}", res);
        }
        mResults.clear();
    }

    set_current_level(0);
    mDebugging = false;
    return true;
}

bool LuaDebugger::is_enable() const { return mEnable; }

void LuaDebugger::enable(bool enable) {
    if (valid()) {
        if (enable && has_break_point()) {
            lua_sethook(main_lua(), LuaDebugger::debug_hook, LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE, 0);
        } else {
            lua_sethook(main_lua(), nullptr, 0, 0);
        }
        mEnable = enable;
    }
}

bool LuaDebugger::is_enable_catch_error() const { return mEnableCatchError; }

void LuaDebugger::enable_catch_error(bool enable) {
    mEnableCatchError = enable;
    mLastCatchError = Success;
}

void LuaDebugger::catch_lua_error() {
    if (mSkipStack) {
        mDebugging = false;
        mSkipStack = false;
        mLastCatchError = ErrorLuaCancelPcall;
    } else {
        const char *errStr = lua_tostring(lua(), -1);
        PureError("Error!!!Lua Error Catch \"{}\"", errStr);
        if (valid() && is_enable_catch_error()) {
            mLastCatchError = ErrorLuaPcallFailed;
            debug(true);
        }
    }
}

int LuaDebugger::debugger_attach(lua_State *L) {
    LuaDebugger *ld = get_debugger(L);
    if (ld == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    bool res = ld->attach();
    lua_pushboolean(L, res ? 1 : 0);
    return 1;
}

int LuaDebugger::debugger_deattach(lua_State *L) {
    LuaDebugger *ld = get_debugger(L);
    if (ld == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    ld->deattach();
    lua_pushboolean(L, 1);
    return 1;
}

int LuaDebugger::debugger_valid(lua_State *L) {
    LuaDebugger *ld = get_debugger(L);
    if (ld == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_pushboolean(L, ld->valid() ? 1 : 0);
    return 1;
}

int LuaDebugger::debugger_ldb(lua_State *L) {
    LuaDebugger *ld = get_debugger(L);
    if (ld == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    bool result = ld->debug(true);
    lua_pushboolean(L, result ? 1 : 0);
    return 1;
}

int LuaDebugger::debugger_cmd(lua_State *L) {
    LuaDebugger *ld = get_debugger(L);
    if (ld == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    size_t len = 0;
    const char *cmd = lua_tolstring(L, 1, &len);
    ld->write_cmd(std::string(cmd, len));
    ld->debug(false);
    auto res = ld->read_result();
    for (const auto &str : res) {
        PureInfo("{}", str);
    }
    lua_pushboolean(L, 1);
    return 1;
}

int LuaDebugger::debugger_exec(lua_State *L) {
    LuaDebugger *ld = get_debugger(L);
    if (ld == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    size_t len = 0;
    const char *cmd = lua_tolstring(L, 1, &len);
    ld->cmd_exec(std::string(cmd, len));
    auto res = ld->read_result();
    for (const auto &str : res) {
        PureInfo("{}", str);
    }
    lua_pushboolean(L, 1);
    return 1;
}

int LuaDebugger::debugger_enable(lua_State *L) {
    LuaDebugger *ld = get_debugger(L);
    if (ld == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    bool enable = lua_toboolean(L, 1) != 0;
    ld->enable(enable);
    lua_pushboolean(L, 1);
    return 1;
}

int LuaDebugger::debugger_enable_catch_error(lua_State *L) {
    LuaDebugger *ld = get_debugger(L);
    if (ld == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    bool enable = lua_toboolean(L, 1) != 0;
    ld->enable_catch_error(enable);
    lua_pushboolean(L, 1);
    return 1;
}

LuaDebugger *LuaDebugger::get_debugger(lua_State *L) {
    LuaStateProtect lsp(L);
    lua_pushlightuserdata(L, (void *)LuaDebugger::get_debugger);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_islightuserdata(L, -1)) {
        return (LuaDebugger *)lua_touserdata(L, -1);
    }
    return nullptr;
}

void LuaDebugger::debug_hook(lua_State *L, lua_Debug *ld) {
    LuaDebugger *self = LuaDebugger::get_debugger(L);
    if (self != nullptr) {
        if (self->mLua == L) {
            self->reset_coroutine(nullptr);
        } else {
            self->reset_coroutine(L);
        }
        switch (ld->event) {
            case LUA_HOOKCALL:
                if (self->check_func_point(ld)) {
                    break;
                }
                return;
            case LUA_HOOKLINE:
                if (self->check_line_point(ld) || self->get_break_level() >= self->get_stack_level()) {
                    break;
                }
                return;
            case LUA_HOOKRET:
            case LUA_HOOKCOUNT:
            case LUA_HOOKTAILCALL:
                return;
            default:
                PureError("Invalid lua hook event {}", ld->event);
                return;
        }
        lua_sethook(self->main_lua(), nullptr, 0, 0);
        int32_t level = self->get_current_level();
        std::string info;
        if (self->get_frame_info(level, true, info)) {
            PureInfo(info);
        }
        self->debug(true);
    }
}

int LuaDebugger::add_interface(PureCore::StringRef name, lua_CFunction fn) {
    if (mLua == nullptr) {
        return ErrorLuaStateNullptr;
    }
    LuaStateProtect lsp(mLua);
    lua_getglobal(mLua, "Debugger");
    if (!lua_istable(mLua, -1)) {
        lua_createtable(mLua, 0, 8);
        lua_pushvalue(mLua, -1);
        lua_setglobal(mLua, "Debugger");
    }
    lua_pushlstring(mLua, name.data(), name.size());
    lua_pushcclosure(mLua, fn, 0);
    lua_rawset(mLua, -3);
    return Success;
}

void LuaDebugger::block_read_cmd() {
    PureInfo("ldb : ");
    std::string input;
    char tmp[128]{};
    while (fgets(tmp, sizeof(tmp), stdin)) {
        input.append(tmp);
        if (!input.empty() && input.back() == '\n') {
            mCmds.push_back(std::move(input));
            return;
        }
    }
}

bool LuaDebugger::pop_cmd(std::string &cmd, std::string &params) {
    if (mCmds.empty()) {
        return false;
    }
    std::string data = std::move(mCmds.front());
    mCmds.pop_front();
    size_t idx = data.find_first_of(" \t\n");
    if (idx != std::string::npos) {
        cmd = data.substr(0u, idx);
        params = data.substr(idx);
        trim_str(params);
    } else {
        cmd = data;
    }
    return true;
}

bool LuaDebugger::get_frame_info(int32_t level, bool isCurrent, std::string &info) const {
    lua_Debug ld;
    if (!lua_getstack(lua(), level, &ld)) {
        return false;
    }
    std::string buffer;
    lua_getinfo(lua(), "nSl", &ld);

    if (isCurrent) {
        fmt::format_to(std::back_inserter(buffer), " >> #{} {}", level, ld.source);
    } else {
        fmt::format_to(std::back_inserter(buffer), "    #{} {}", level, ld.source);
    }

    if (ld.currentline >= 0) {
        fmt::format_to(std::back_inserter(buffer), " : {}", ld.currentline);
    }
    if (ld.name != nullptr) {
        fmt::format_to(std::back_inserter(buffer), " | ({} {} `{}`)", ld.namewhat, ld.what, ld.name);
    } else {
        fmt::format_to(std::back_inserter(buffer), " | (trunk)");
    }
    info.append(buffer.data(), buffer.size());
    return true;
}

void LuaDebugger::get_all_frame_info(int32_t currentLevel, std::vector<std::string> &infos) const {
    for (int i = 0; i <= get_max_frame_count(); i++) {
        std::string info;
        if (get_frame_info(i, i == currentLevel, info)) {
            infos.push_back(std::move(info));
        } else {
            break;
        }
    }
}

bool LuaDebugger::get_line(int32_t level, std::string &info) const {
    int32_t line = get_current_line(level);
    if (get_current_source(level, info)) {
        fmt::format_to(std::back_inserter(info), ":{}", line);
        return true;
    }
    return false;
}

void LuaDebugger::get_all_break_points(std::vector<std::string> &infos) const {
    int32_t index = 1;
    for (std::vector<BreakPoint>::const_iterator iter = mBreakPoints.begin(); iter != mBreakPoints.end(); ++iter, ++index) {
        if (iter->is_function()) {
            infos.push_back(fmt::format("    {}\t function : {}", index, iter->mName));
        } else {
            infos.push_back(fmt::format("    {}\t {} : {}", index, iter->mName, iter->mLineNum));
        }
    }
}

bool LuaDebugger::get_var(const std::string &varName, std::string &info) const {
    if (!valid()) {
        return false;
    }
    LuaStateProtect lsp(lua());
    lua_Debug ld;
    if (lua_getstack(lua(), get_current_level(), &ld)) {
        if (!push_var(&ld, varName)) {
            return false;
        }
    } else {
        if (!push_var(nullptr, varName)) {
            return false;
        }
    }

    int idx = lua_absindex(lua(), -1);
    info.append(varName);
    info.push_back('|');
    const char *tName = lua_typename(lua(), lua_type(lua(), idx));
    info.append(tName);
    info.push_back('|');
    lua_getglobal(lua(), "tostring");
    lua_pushvalue(lua(), idx);
    lua_call(lua(), 1, 1);
    const char *var = lua_tostring(lua(), -1);
    if (var == nullptr) {
        return false;
    }
    info.append(var);
    return true;
}

bool LuaDebugger::get_var_fields(const std::string &varName, std::vector<std::string> &infos) const {
    if (!valid()) {
        return false;
    }
    LuaStateProtect lsp(lua());
    lua_Debug ld;
    if (lua_getstack(lua(), get_current_level(), &ld)) {
        if (!push_var(&ld, varName)) {
            return false;
        }
    } else {
        if (!push_var(nullptr, varName)) {
            return false;
        }
    }

    return get_obj_fields(-1, -1, 0, true, infos);
}

bool LuaDebugger::get_obj_fields(int objIdx, int tabIdx, int32_t indexLevel, bool skipFunc, std::vector<std::string> &infos) const {
    LuaStateProtect lsp(lua());
    objIdx = lua_absindex(lua(), objIdx);
    tabIdx = lua_absindex(lua(), tabIdx);
    if (lua_istable(lua(), tabIdx)) {
        LuaStateProtect lsp(lua());
        lua_getglobal(lua(), "tostring");
        int tosIdx = lua_gettop(lua());
        for (LuaIterator iter(lua(), tabIdx); !iter.is_over(); ++iter) {
            if (lua_isuserdata(lua(), iter.key_idx())) {
                continue;
            }
            bool callFunc = false;
            if (lua_iscfunction(lua(), iter.value_idx())) {
                const char *t = lua_tostring(lua(), iter.key_idx());
                auto f = lua_tocfunction(lua(), iter.value_idx());
                if (f == LuaMetatable::mt_property_getset_function || f == LuaMetatable::mt_static_getset_function) {
                    callFunc = true;
                }
            }
            if (!callFunc && skipFunc && lua_isfunction(lua(), iter.value_idx())) {
                continue;
            }
            lua_pushvalue(lua(), tosIdx);  // tostring
            iter.push_key();               // key
            lua_call(lua(), 1, 1);
            const char *s = lua_tostring(lua(), -1); /* get result */
            if (s == nullptr) {
                return false;
            }
            if (strcmp(s, "__index") == 0) {
                continue;
            }
            std::string info = s;
            info.push_back('|');

            lua_pushvalue(lua(), tosIdx);  // tostring
            if (callFunc) {
                iter.push_value();
                lua_pushvalue(lua(), objIdx);
                lua_call(lua(), 1, 1);  // property
            } else {
                iter.push_value();  // value
            }
            const char *tName = lua_typename(lua(), lua_type(lua(), -1));
            info.append(tName);
            info.push_back('|');
            lua_call(lua(), 1, 1);
            s = lua_tostring(lua(), -1); /* get result */
            if (s == nullptr) {
                return false;
            }
            info.append(s);
            infos.push_back(std::move(info));
        }
    }

    if (lua_getmetatable(lua(), tabIdx)) {
        lua_pushstring(lua(), "__index");
        lua_rawget(lua(), -2);
        if (lua_istable(lua(), -1)) {
            std::string info(fmt::format("__index[{}]|", indexLevel + 1));
            const char *tName = lua_typename(lua(), lua_type(lua(), -1));
            info.append(tName);
            info.push_back('|');
            lua_getglobal(lua(), "tostring");
            lua_pushvalue(lua(), -2);
            lua_call(lua(), 1, 1);
            const char *var = lua_tostring(lua(), -1);
            if (var != nullptr) {
                info.append(var);
                infos.push_back(std::move(info));
                lua_pop(lua(), 1);
                get_obj_fields(objIdx, -1, indexLevel + 1, skipFunc, infos);
            }
        }
    }
    return true;
}

bool LuaDebugger::get_local_vars(std::vector<std::string> &infos) const {
    LuaStateProtect lsp(lua());
    lua_Debug ld;
    if (!lua_getstack(lua(), get_current_level(), &ld)) {
        return false;
    }
    int32_t idx = 0;
    const char *name = nullptr;
    while ((name = lua_getlocal(lua(), &ld, ++idx)) != nullptr) {
        if (lua_isfunction(lua(), -1) || strcmp(name, "(temporary)") == 0 || strcmp(name, "(C temporary)") == 0) {
            lua_pop(lua(), 1);
            continue;
        }
        int idx = lua_absindex(lua(), -1);
        std::string info(name);
        info.push_back('|');
        const char *tName = lua_typename(lua(), lua_type(lua(), idx));
        info.append(tName);
        info.push_back('|');
        lua_getglobal(lua(), "tostring");
        lua_pushvalue(lua(), idx);
        lua_call(lua(), 1, 1);
        const char *var = lua_tostring(lua(), -1);
        if (var == nullptr) {
            return false;
        }
        info.append(var);
        infos.push_back(std::move(info));
        lua_pop(lua(), 2);
    }

    if (!lua_getinfo(lua(), "f", &ld)) {
        PureError("Get function info failed!!!");
        return false;
    }

    idx = 0;
    while ((name = lua_getupvalue(lua(), -1, ++idx)) != nullptr) {
        if (lua_isfunction(lua(), -1) || strcmp(name, "_ENV") == 0) {
            lua_pop(lua(), 1);
            continue;
        }
        int idx = lua_absindex(lua(), -1);
        std::string info(name);
        info.push_back('|');
        const char *tName = lua_typename(lua(), lua_type(lua(), idx));
        info.append(tName);
        info.push_back('|');
        lua_getglobal(lua(), "tostring");
        lua_pushvalue(lua(), idx);
        lua_call(lua(), 1, 1);
        const char *var = lua_tostring(lua(), -1);
        if (var == nullptr) {
            return false;
        }
        info.append(var);
        infos.push_back(std::move(info));
        lua_pop(lua(), 2);
    }
    return true;
}

bool LuaDebugger::get_global_vars(std::vector<std::string> &infos) const {
    LuaStateProtect lsp(lua());
    lua_Debug ld;
    if (!lua_getstack(lua(), get_current_level(), &ld)) {
        return false;
    }
    if (!lua_getinfo(lua(), "f", &ld)) {
        PureError("Get function info failed!!!");
        return false;
    }
    int32_t idx = 0;
    const char *name = nullptr;
    while ((name = lua_getupvalue(lua(), -1, ++idx)) != nullptr) {
        if (strcmp(name, "_ENV") == 0) {
            break;
        }
        name = nullptr;
        lua_pop(lua(), 1);
    }
    if (name != nullptr && lua_istable(lua(), -1)) {  // find env
        return get_obj_fields(-1, -1, 0, true, infos);
    }
    return true;
}

int32_t LuaDebugger::get_max_frame_count() const { return mMaxFrameCount; }

void LuaDebugger::set_max_frame_count(int32_t frameCount) { mMaxFrameCount = frameCount; }

bool LuaDebugger::has_break_point() const { return !mBreakPoints.empty(); }

bool LuaDebugger::add_break_point(const std::string &point) {
    BreakPoint bp;
    if (!get_break_point(point, bp)) {
        return false;
    }
    auto iter = std::find(mBreakPoints.begin(), mBreakPoints.end(), bp);
    if (iter != mBreakPoints.end()) {
        return true;
    }
    mBreakPoints.push_back(bp);
    lua_sethook(main_lua(), LuaDebugger::debug_hook, LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE, 0);
    return true;
}

bool LuaDebugger::del_break_point(const std::string &point) {
    if (check_digit(point)) {  // index
        int32_t idx = atoi(point.c_str()) - 1;
        if (idx < 0 || idx >= (int32_t)mBreakPoints.size()) return false;
        mBreakPoints.erase(mBreakPoints.begin() + idx);
    } else {  // bp
        BreakPoint bp;
        if (!get_break_point(point, bp)) {
            return false;
        }
        auto iter = std::find(mBreakPoints.begin(), mBreakPoints.end(), bp);
        if (iter != mBreakPoints.end()) {
            mBreakPoints.erase(iter);
        }
    }

    if (!has_break_point()) {
        lua_sethook(main_lua(), LuaDebugger::debug_hook, 0, 0);
    }
    return true;
}

bool LuaDebugger::list_break_points(std::string &points) {
    if (mBreakPoints.empty()) return false;
    int32_t index = 1;
    for (std::vector<BreakPoint>::const_iterator iter = mBreakPoints.begin(); iter != mBreakPoints.end(); ++iter, ++index) {
        if (iter->is_function())
            fmt::format_to(std::back_inserter(points), "{}\t function : {}", index, iter->mName);
        else
            fmt::format_to(std::back_inserter(points), "{}\t {} : {}", index, iter->mName, iter->mLineNum);
    }
    return true;
}

bool LuaDebugger::check_line_point(lua_Debug *ld) const {
    if (valid() && !mBreakPoints.empty() && lua_getinfo(lua(), "Sl", ld) && ld->source != nullptr && ld->currentline >= 0) {
        BreakPoint bp;
        if (ld->source[0] == '@')
            bp.mName = ld->source + 1;
        else
            bp.mName = ld->source;
        bp.mLineNum = ld->currentline;
        return std::find(mBreakPoints.begin(), mBreakPoints.end(), bp) != mBreakPoints.end();
    }
    return false;
}

bool LuaDebugger::check_func_point(lua_Debug *ld) const {
    if (valid() && !mBreakPoints.empty() && lua_getinfo(lua(), "n", ld) && ld->name != nullptr) {
        BreakPoint bp;
        bp.mName = ld->name;
        return std::find(mBreakPoints.begin(), mBreakPoints.end(), bp) != mBreakPoints.end();
    }
    return false;
}

bool LuaDebugger::get_break_point(const std::string &point, BreakPoint &bp) {
    size_t idx = point.find(':');
    if (idx < point.size())  // file name and line
    {
        std::string fileName = point.substr(0u, idx);
        std::string fileLine = point.substr(idx + 1);
        trim_str(fileName);
        trim_str(fileLine);
        if (fileName.empty() || fileLine.empty()) {
            return false;
        }
        int32_t line = atoi(fileLine.c_str());
        if (!bp.set_line_point(fileName, line)) {
            return false;
        }
    } else {
        if (point.empty())  // current file line
        {
            std::string data = point;
            trim_str(data);
            int32_t line = atoi(data.c_str());
            if (!get_current_source(0, data) || !bp.set_line_point(data, line)) {
                return false;
            }
        } else  // function name
        {
            std::string name = point;
            trim_str(name);
            if (!bp.set_func_point(name)) {
                return false;
            }
        }
    }
    return true;
}

bool LuaDebugger::reload_file(const std::string &path) {
    if (!valid()) {
        return false;
    }
    if (path.empty()) {
        std::string source;
        if (!get_current_source(0, source) || source.empty() || source[0] != '@') {
            return false;
        }
        LuaStateProtect lsp(main_lua());
        int err = push_lua_error_catcher(main_lua());
        if (luaL_loadfile(main_lua(), source.c_str() + 1) || lua_pcall(main_lua(), 0, LUA_MULTRET, err)) {
            return false;
        }
    } else {
        LuaStateProtect lsp(main_lua());
        int err = push_lua_error_catcher(main_lua());
        if (luaL_loadfile(main_lua(), path.c_str()) || lua_pcall(main_lua(), 0, LUA_MULTRET, err)) {
            return false;
        }
    }
    return true;
}

bool LuaDebugger::next_call() {
    if (valid()) {
        lua_sethook(main_lua(), LuaDebugger::debug_hook, LUA_MASKLINE | LUA_MASKCALL | LUA_MASKRET, 0);
        set_break_level(get_stack_level());
        return true;
    }
    return false;
}

bool LuaDebugger::step_call() {
    if (valid()) {
        lua_sethook(main_lua(), LuaDebugger::debug_hook, LUA_MASKLINE | LUA_MASKCALL | LUA_MASKRET, 0);
        set_break_level(INT32_MAX);
        return true;
    }
    return false;
}

bool LuaDebugger::over_call() {
    if (valid()) {
        lua_sethook(main_lua(), LuaDebugger::debug_hook, LUA_MASKLINE | LUA_MASKCALL | LUA_MASKRET, 0);
        set_break_level(get_stack_level() - 1);
        return true;
    }
    return false;
}

bool LuaDebugger::list_code(int32_t level, int32_t currentLine, int32_t lineCount, std::vector<std::string> &codes) const {
    std::string path;
    if (!get_current_source(level, path)) {
        return false;
    }

    if (currentLine <= 0) {
        codes.push_back(" >> 0");
        codes.back().append(path);
        return true;
    }

    std::string code;
    FILE *fp = nullptr;
    std::string absPath = get_abs_source_path(path.c_str());
    if (absPath.empty()) {
        fp = fopen(path.c_str(), "rb");
    } else {
        fp = fopen(absPath.c_str(), "rb");
    }
    if (fp == nullptr) {
        PureError("source `{}` not found!!!", path);
        return false;
    }

    bool result = true;
    int c = fgetc(fp);
    if (c == '#' || c == LUA_SIGNATURE[0]) {
        PureError("not support lua bin code `{}`", path);
        result = false;
    } else {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        PureCore::DynamicBuffer buffer;
        buffer.ensure_buffer(size + 1);
        auto data = buffer.free_buffer();
        std::vector<std::string> lines;
        while (!feof(fp)) {
            char *line = fgets(data.data(), static_cast<int>(data.size()), fp);
            if (line != nullptr) {
                lines.push_back(line);
            }
        }
        if (currentLine > (int32_t)lines.size()) {
            PureError("source line {} is invalid, the file `{}` has {} lines!!!", currentLine, path, lines.size());
            result = false;
        } else {
            --currentLine;
            int startLine = std::max(0, currentLine - (lineCount >> 1));
            int endLine = std::min((int32_t)lines.size(), currentLine + (lineCount >> 1) + 1);
            while (startLine < endLine) {
                code.clear();
                if (startLine == currentLine) {
                    fmt::format_to(std::back_inserter(code), " >> {}\t", startLine + 1);
                } else {
                    fmt::format_to(std::back_inserter(code), "    {}\t", startLine + 1);
                }
                fmt::format_to(std::back_inserter(code), "{}", lines[startLine]);
                ++startLine;
                if (!code.empty() && code.back() == '\n') {
                    code.pop_back();
                }
                if (!code.empty() && code.back() == '\r') {
                    code.pop_back();
                }
                codes.push_back(std::move(code));
            }
        }
    }
    fclose(fp);
    return result;
}

bool LuaDebugger::set_frame(int32_t level) {
    if (!valid() || level < 0) {
        return false;
    }
    lua_Debug ld;
    if (!lua_getstack(lua(), level, &ld)) {
        return false;
    }
    set_current_level(level);
    return true;
}

bool LuaDebugger::push_var(lua_Debug *ld, const std::string &varName) const {
    std::vector<std::string> fields;
    if (!split_fields(varName, fields) || fields.empty()) {
        PureError("Split var failed!!!");
        return false;
    }
    int32_t idx = 0;
    const char *name = nullptr;
    LuaRef env;
    if (ld != nullptr) {
        while ((name = lua_getlocal(lua(), ld, ++idx)) != nullptr) {
            if (fields[0] == name) {
                if (!push_sub_var(ld, 1u, fields)) {
                    return false;
                }
                return true;
            } else {
                lua_pop(lua(), 1);
            }
        }

        if (!lua_getinfo(lua(), "f", ld)) {
            PureError("Get function info failed!!!");
            return false;
        }

        idx = 0;
        while ((name = lua_getupvalue(lua(), -1, ++idx)) != nullptr) {
            if (fields[0] == name) {
                lua_remove(lua(), -2);  // pop f
                if (!push_sub_var(ld, 1u, fields)) {
                    return false;
                }
                return true;
            } else {
                if (strcmp("_ENV", name) == 0) {
                    env.reset(lua(), -1);
                }
                lua_pop(lua(), 1);
            }
        }
        lua_pop(lua(), 1);  // pop f
    }

    if (env.is_table())  // env
    {
        env.push();
        lua_pushlstring(lua(), fields[0].c_str(), fields[0].size());
        lua_gettable(lua(), -2);
        if (!lua_isnoneornil(lua(), -1)) {
            lua_remove(lua(), -2);
            if (!push_sub_var(ld, 1u, fields)) {
                return false;
            }
            return true;
        } else {
            lua_pop(lua(), 2);
        }
    }
    return false;
}

bool LuaDebugger::push_sub_var(lua_Debug *ld, size_t startIdx, const std::vector<std::string> &fields) const {
    for (size_t i = startIdx; i < fields.size(); ++i) {
        if (fields[i].empty()) {
            continue;
        }
        if (fields[i][0] == '[') {
            std::string field(fields[i].substr(1u, fields[i].size() - 2u));
            trim_str(field);
            if (field.empty()) {
                PureError("`{}` field is invalid!!!", fields[i]);
                return false;
            }
            if (field[0] == '\"') {
                if (field.size() <= 2u || field.back() != '\"') {
                    PureError("`{}` field is invalid, the \" is not match!!!", fields[i]);
                    return false;
                }
                field = field.substr(1u, field.size() - 2u);
                lua_pushlstring(lua(), field.c_str(), field.size());
                lua_gettable(lua(), -2);
                try_get_property(-2, -1);
                lua_remove(lua(), -2);
            } else if (std::isdigit((unsigned char)(field[0]))) {
                double num = atof(field.c_str());
                lua_pushnumber(lua(), num);
                lua_gettable(lua(), -2);
                try_get_property(-2, -1);
                lua_remove(lua(), -2);
            } else {
                if (!push_var(ld, field)) {
                    return false;
                }
                lua_gettable(lua(), -2);
                try_get_property(-2, -1);
                lua_remove(lua(), -2);
            }
        } else {
            lua_pushlstring(lua(), fields[i].c_str(), fields[i].size());
            lua_gettable(lua(), -2);
            try_get_property(-2, -1);
            lua_remove(lua(), -2);
        }
    }
    return true;
}

void LuaDebugger::try_get_property(int tabIdx, int varIdx) const {
    if (!lua_iscfunction(lua(), varIdx)) {
        return;
    }
    auto f = lua_tocfunction(lua(), varIdx);
    if (f != LuaMetatable::mt_property_getset_function && f != LuaMetatable::mt_static_getset_function) {
        return;
    }
    tabIdx = lua_absindex(lua(), tabIdx);
    varIdx = lua_absindex(lua(), varIdx);
    LuaStateProtect lsp(lua());
    lua_pushvalue(lua(), varIdx);
    lua_pushvalue(lua(), tabIdx);
    lua_call(lua(), 1, 1);
    lua_replace(lua(), varIdx);
}

int32_t LuaDebugger::get_current_line(int32_t level) const {
    if (valid()) {
        lua_Debug ld;
        if (lua_getstack(lua(), level, &ld) && lua_getinfo(lua(), "l", &ld)) {
            return ld.currentline;
        }
    }
    return -1;
}

int32_t LuaDebugger::get_stack_level() const {
    if (valid()) {
        lua_Debug ld;
        int32_t level = 0;
        while (lua_getstack(lua(), level, &ld)) {
            ++level;
        }
        return level;
    }
    return -1;
}

bool LuaDebugger::get_current_source(int32_t level, std::string &source) const {
    if (valid()) {
        lua_Debug ld;
        if (lua_getstack(lua(), level, &ld) && lua_getinfo(lua(), "S", &ld)) {
            if (ld.source[0] == '@') {
                source.assign(ld.source + 1);
            } else {
                source.assign(ld.source);
            }
            return true;
        }
    }
    return false;
}

void LuaDebugger::reset_coroutine(lua_State *L) {
    if (mCoroutine != L) {
        mCoroutine = L;
        mCorBreakLevel = -1;
        mCorCurrentLevel = 0;
    }
}

lua_State *LuaDebugger::lua() const { return mCoroutine == nullptr ? mLua : mCoroutine; }

lua_State *LuaDebugger::main_lua() const { return mLua; }

int32_t LuaDebugger::get_break_level() const { return mCoroutine == nullptr ? mBreakLevel : mCorBreakLevel; }

void LuaDebugger::set_break_level(int32_t level) {
    if (mCoroutine == nullptr) {
        mBreakLevel = level;
    } else {
        mCorBreakLevel = level;
    }
}

int32_t LuaDebugger::get_current_level() const { return mCoroutine == nullptr ? mCurrentLevel : mCorCurrentLevel; }

void LuaDebugger::set_current_level(int32_t level) {
    if (mCoroutine == nullptr) {
        mCurrentLevel = level;
    } else {
        mCorCurrentLevel = level;
    }
}

std::string LuaDebugger::get_abs_source_path(const char *path) const {
    auto end = mFixPaths.end();
    for (auto iter = mFixPaths.begin(); iter != end; ++iter) {
        std::string absPath(*iter);
        if (!absPath.empty()) {
            size_t idx = absPath.find('?');
            if (idx != std::string::npos) {
                absPath.replace(idx, 1, path);
            } else {
                if (absPath.back() != '/') absPath.push_back('/');
                absPath.append(path);
            }
            if (PureCore::file_exist(absPath.c_str())) {
                return std::move(absPath);
            }
            absPath.append(".lua");
            if (PureCore::file_exist(absPath.c_str())) {
                return std::move(absPath);
            }
        }
    }
    return std::string();
}

bool LuaDebugger::BreakPoint::set_func_point(const std::string &funcName) {
    if (funcName.empty()) return false;
    mName = funcName;
    mLineNum = -1;
    return true;
}

bool LuaDebugger::BreakPoint::set_line_point(const std::string &fileName, int32_t lineNum) {
    if (fileName.empty()) return false;
    mName = fileName;
    for (size_t i = 0u; i < mName.size(); ++i) {
        if (mName[i] == '\\') {
            mName[i] = '/';
        }
    }
    for (size_t idx = mName.find("//"); idx < mName.size(); idx = mName.find("//")) {
        mName.replace(idx, 2, "/");
    }
    if (mName.empty()) return false;
    mLineNum = lineNum;
    return true;
}

bool LuaDebugger::BreakPoint::is_function() const { return mLineNum == -1; }

bool LuaDebugger::BreakPoint::operator==(const BreakPoint &right) const {
    if (mLineNum != right.mLineNum) return false;
    return mName.find(right.mName) != std::string::npos || right.mName.find(mName) != std::string::npos;
}

bool LuaDebugger::cmd_help(const std::string &params) {
    mResults.push_back("help/h; show help");
    mResults.push_back("offldb/ol; disable ldb");
    mResults.push_back("offerr/oe; disable catch error");
    mResults.push_back("frame/f n; goto n stack level");
    mResults.push_back("up/u; up stack level");
    mResults.push_back("down/d; down stack level");
    mResults.push_back("backtrace/bt; show backtrace");
    mResults.push_back("listbreak/lb; show break point list");
    mResults.push_back("addbreak/ab func/file:line; add break point,func means function point，file:line means line point");
    mResults.push_back("delbreak/db n; delete n index break point");
    mResults.push_back("next/n; next call, will skip sub function");
    mResults.push_back("step/s; step call, will into sub function");
    mResults.push_back("over/o;step over, will skip out current function");
    mResults.push_back("continue/c; continue run");
    mResults.push_back("findvar/fv name; find value of name");
    mResults.push_back("varfields/vf name; find table of name all fields value");
    mResults.push_back("localvars/lv; find all local vars");
    mResults.push_back("globalvars/gv; find all global vars");
    mResults.push_back("allvars/av; find all vars");
    mResults.push_back("exec/e code; exec lua code");
    mResults.push_back("reload/rl path ; relod lua file from path");
    mResults.push_back("list/l n; list near n line code, n normal 16;");
    mResults.push_back("cancel/ca; cancel current stack call");
    return true;
}

bool LuaDebugger::cmd_offldb(const std::string &params) {
    bool ret = true;
    if (!get_bool(params, ret)) {
        mResults.push_back("param error");
        return true;
    }
    enable(ret);
    mResults.push_back("success");
    return true;
}

bool LuaDebugger::cmd_offerr(const std::string &params) {
    bool ret = true;
    if (!get_bool(params, ret)) {
        mResults.push_back("param error");
        return true;
    }
    enable_catch_error(ret);
    mResults.push_back("success");
    return true;
}

bool LuaDebugger::cmd_frame(const std::string &params) {
    int32_t index = atoi(params.c_str());
    if (set_frame(index)) {
        get_all_frame_info(get_current_level(), mResults);
    } else {
        mResults.push_back("set frame failed");
    }
    return true;
}

bool LuaDebugger::cmd_up(const std::string &params) {
    if (set_frame(get_current_level() + 1)) {
        get_all_frame_info(get_current_level(), mResults);
    } else {
        mResults.push_back("set frame failed");
    }
    return true;
}

bool LuaDebugger::cmd_down(const std::string &params) {
    if (set_frame(get_current_level() - 1)) {
        get_all_frame_info(get_current_level(), mResults);
    } else {
        mResults.push_back("set frame failed");
    }
    return true;
}

bool LuaDebugger::cmd_backtrace(const std::string &params) {
    get_all_frame_info(get_current_level(), mResults);
    return true;
}

bool LuaDebugger::cmd_listbreak(const std::string &params) {
    get_all_break_points(mResults);
    return true;
}

bool LuaDebugger::cmd_addbreak(const std::string &params) {
    if (add_break_point(params)) {
        get_all_break_points(mResults);
    } else {
        mResults.push_back("add break point failed");
    }
    return true;
}

bool LuaDebugger::cmd_delbreak(const std::string &params) {
    if (del_break_point(params)) {
        get_all_break_points(mResults);
    } else {
        mResults.push_back("delete break point failed");
    }
    return true;
}

bool LuaDebugger::cmd_clearbreak(const std::string &params) {
    mBreakPoints.clear();
    lua_sethook(main_lua(), LuaDebugger::debug_hook, 0, 0);
    return true;
}

bool LuaDebugger::cmd_next(const std::string &params) {
    if (next_call()) {
        return false;
    } else {
        mResults.push_back("next call failed");
    }
    return true;
}

bool LuaDebugger::cmd_step(const std::string &params) {
    if (step_call()) {
        return false;
    } else {
        mResults.push_back("step call failed");
    }
    return true;
}

bool LuaDebugger::cmd_over(const std::string &params) {
    if (over_call()) {
        return false;
    } else {
        mResults.push_back("over call failed");
    }
    return true;
}

bool LuaDebugger::cmd_continue(const std::string &params) {
    if (has_break_point()) {
        lua_sethook(main_lua(), LuaDebugger::debug_hook, LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE, 0);
    }
    // close all next and setp and over
    set_break_level(-1);
    return false;
}

bool LuaDebugger::cmd_findvar(const std::string &params) {
    std::string info;
    if (get_var(params, info)) {
        mResults.push_back(std::move(info));
    } else {
        mResults.push_back("find var failed");
    }
    return true;
}

bool LuaDebugger::cmd_varfields(const std::string &params) {
    if (!get_var_fields(params, mResults)) {
        mResults.push_back("find var children failed");
    }
    return true;
}

bool LuaDebugger::cmd_localvars(const std::string &params) {
    if (!get_local_vars(mResults)) {
        mResults.push_back("find local vars failed");
    }
    return true;
}

bool LuaDebugger::cmd_globalvars(const std::string &params) {
    if (!get_global_vars(mResults)) {
        mResults.push_back("find global vars failed");
    }
    return true;
}

bool LuaDebugger::cmd_allvars(const std::string &params) {
    if (!get_local_vars(mResults) || !get_global_vars(mResults)) {
        mResults.push_back("find all vars failed");
    }
    return true;
}

bool LuaDebugger::cmd_exec(const std::string &params) {
    if (!valid()) {
        mResults.push_back("lua debugger is not valid");
        return true;
    }
    LuaRunner exec(mLua);
    exec.set_stack_level(get_current_level());
    exec.exec(params);
    auto res = exec.pop_results();
    for (auto &str : res) {
        mResults.push_back(std::move(str));
    }
    return true;
}

bool LuaDebugger::cmd_reload(const std::string &params) {
    if (!reload_file(params)) {
        mResults.push_back("reload failed");
    }
    return true;
}

bool LuaDebugger::cmd_list(const std::string &params) {
    int32_t lineCount = atoi(params.c_str());
    if (lineCount <= 0) {
        lineCount = 16;
    }
    int32_t level = get_current_level();
    if (!list_code(level, get_current_line(level), lineCount, mResults)) {
        mResults.push_back("list code failed");
    }
    return true;
}

bool LuaDebugger::cmd_cancel(const std::string &params) {
    mSkipStack = true;
    luaL_error(lua(), "debugger cancel pcall");
    return false;
}

}  // namespace PureLua