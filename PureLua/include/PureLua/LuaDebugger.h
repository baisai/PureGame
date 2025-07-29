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

#include "PureLua/PureLuaLib.h"
#include "PureLua/LuaErrorDesc.h"
#include "lua.hpp"

#include <vector>
#include <list>
#include <string>
#include <functional>

namespace PureLua {
class PURELUA_API LuaDebugger {
public:
    LuaDebugger();
    ~LuaDebugger();

    int init(lua_State* L);
    void close();

    bool valid() const;

    bool attach();
    void deattach();

    void clear_last_catch();
    int get_last_catch();

    void add_fix_path(const std::string& path);
    void remove_fix_path(const std::string& path);
    void clear_fix_path();

    void write_cmd(const std::string& cmd);
    std::vector<std::string> read_result();
    bool debug(bool wait);

    bool is_enable() const;
    void enable(bool enable);

    bool is_enable_catch_error() const;
    void enable_catch_error(bool enable);

    void catch_lua_error();

private:
    static int debugger_attach(lua_State* L);
    static int debugger_deattach(lua_State* L);
    static int debugger_valid(lua_State* L);
    static int debugger_ldb(lua_State* L);
    static int debugger_cmd(lua_State* L);
    static int debugger_exec(lua_State* L);
    static int debugger_enable(lua_State* L);
    static int debugger_enable_catch_error(lua_State* L);
    static LuaDebugger* get_debugger(lua_State* L);

private:
    static void debug_hook(lua_State* L, lua_Debug* ld);

    int add_interface(PureCore::StringRef name, lua_CFunction fn);

    void block_read_cmd();
    bool pop_cmd(std::string& cmd, std::string& params);

    bool get_frame_info(int32_t level, bool isCurrent, std::string& info) const;
    void get_all_frame_info(int32_t currentLevel, std::vector<std::string>& infos) const;
    bool get_line(int32_t level, std::string& info) const;
    void get_all_break_points(std::vector<std::string>& infos) const;
    bool get_var(const std::string& varName, std::string& info) const;
    bool get_var_fields(const std::string& varName, std::vector<std::string>& infos) const;
    bool get_obj_fields(int objIdx, int tabIdx, int32_t indexLevel, bool skipFunc, std::vector<std::string>& infos) const;
    bool get_local_vars(std::vector<std::string>& infos) const;
    bool get_global_vars(std::vector<std::string>& infos) const;

    int32_t get_max_frame_count() const;
    void set_max_frame_count(int32_t frameCount);

    bool has_break_point() const;
    bool add_break_point(const std::string& point);
    bool del_break_point(const std::string& point);
    bool list_break_points(std::string& points);
    bool check_line_point(lua_Debug* ld) const;
    bool check_func_point(lua_Debug* ld) const;
    struct BreakPoint;
    bool get_break_point(const std::string& point, BreakPoint& bp);

    bool reload_file(const std::string& path);

    bool next_call();
    bool step_call();
    bool over_call();

    bool list_code(int32_t level, int32_t currentLine, int32_t lineCount, std::vector<std::string>& codes) const;

    bool set_frame(int32_t level);

    bool push_var(lua_Debug* ld, const std::string& varName) const;
    bool push_sub_var(lua_Debug* ld, size_t startIdx, const std::vector<std::string>& fields) const;
    void try_get_property(int tabIdx, int varIdx) const;

    int32_t get_current_line(int32_t level) const;
    int32_t get_stack_level() const;
    bool get_current_source(int32_t level, std::string& source) const;

    void reset_coroutine(lua_State* L);

    lua_State* lua() const;
    lua_State* main_lua() const;

    int32_t get_break_level() const;
    void set_break_level(int32_t level);

    int32_t get_current_level() const;
    void set_current_level(int32_t level);

    std::string get_abs_source_path(const char* path) const;

private:
    struct BreakPoint {
        std::string mName;
        int32_t mLineNum = -1;

        bool set_func_point(const std::string& funcName);
        bool set_line_point(const std::string& fileName, int32_t lineNum);
        bool is_function() const;
        bool operator==(const BreakPoint& right) const;
    };

private:
    // cmd func
    //  help/h; show help
    bool cmd_help(const std::string& params);
    // offldb/ol; disable ldb
    bool cmd_offldb(const std::string& params);
    // offerr/oe; disable catch error
    bool cmd_offerr(const std::string& params);
    // frame/f n; goto n stack level
    bool cmd_frame(const std::string& params);
    // up/u; up stack level
    bool cmd_up(const std::string& params);
    // down/d; down stack level
    bool cmd_down(const std::string& params);
    // backtrace/bt; show backtrace
    bool cmd_backtrace(const std::string& params);
    // listbreak/lb; show break point list
    bool cmd_listbreak(const std::string& params);
    // addbreak/ab func/file:line; add break point,func means function point��file:line means line point
    bool cmd_addbreak(const std::string& params);
    // delbreak/db n; delete n index break point
    bool cmd_delbreak(const std::string& params);
    // clearbreak/cb n; clear all break point
    bool cmd_clearbreak(const std::string& params);
    // next/n; next call, will skip sub function
    bool cmd_next(const std::string& params);
    // step/s; step call, will into sub function
    bool cmd_step(const std::string& params);
    // over/o;step over, will skip out current function
    bool cmd_over(const std::string& params);
    // continue/c; continue run
    bool cmd_continue(const std::string& params);
    // findvar/fv name; find value of name
    bool cmd_findvar(const std::string& params);
    // varfields/vf name; find table of name all fields value
    bool cmd_varfields(const std::string& params);
    // localvars/lv; find all local vars
    bool cmd_localvars(const std::string& params);
    // globalvars/gv; find all global vars
    bool cmd_globalvars(const std::string& params);
    // allvars/av; find all vars
    bool cmd_allvars(const std::string& params);
    // exec/e code; exec lua code
    bool cmd_exec(const std::string& params);
    // reload/rl path ; relod lua file from path
    bool cmd_reload(const std::string& params);
    // list/l n; list near n line code, n normal 16;
    bool cmd_list(const std::string& params);
    // cancel/ca; cancel current stack call
    bool cmd_cancel(const std::string& params);

private:
    bool mValid = false;
    bool mEnable = true;
    bool mEnableCatchError = true;
    int mLastCatchError = Success;
    bool mDebugging = false;
    bool mSkipStack = false;

    lua_State* mLua = nullptr;
    int32_t mBreakLevel = -1;
    int32_t mCurrentLevel = 0;

    lua_State* mCoroutine = nullptr;
    int32_t mCorBreakLevel = -1;
    int32_t mCorCurrentLevel = 0;
    int32_t mMaxFrameCount = UINT8_MAX;

    std::list<std::string> mCmds;
    std::string mLastCmd;
    std::string mLastParams;
    std::vector<std::string> mResults;
    std::vector<BreakPoint> mBreakPoints;
    std::vector<std::string> mFixPaths;
    std::unordered_map<std::string, std::function<bool(const std::string&)>> mCmdFunc;

    PURE_DISABLE_COPY(LuaDebugger)
};
}  // namespace PureLua
