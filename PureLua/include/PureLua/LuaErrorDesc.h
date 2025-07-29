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

#include <string>

#define PURELUA_ERROR_MAP(XX)                                                       \
    XX(ErrorInvalidArg, "The Arg Is Invalid")                                       \
    XX(ErrorLuaOperatorFailed, "The Lua Operator Failed")                           \
    XX(ErrorThisStateInited, "Lua State Has Already Inited")                        \
    XX(ErrorNewStateFailed, "New Lua State Failed")                                 \
    XX(ErrorLuaStateNullptr, "Lua State Is Nullptr")                                \
    XX(ErrorLuaFuncProfilerNotAttach, "Lua CallProfiler Is Not Attach")             \
    XX(ErrorLuaMemProfilerNotAttach, "Lua MemProfiler Is Not Attach")               \
    XX(ErrorLuaFuncProfilerNotRunning, "Lua CallProfiler Is Not Running")           \
    XX(ErrorLuaMemProfilerNotRunning, "Lua MemProfiler Is Not Running")             \
    XX(ErrorLuaFuncProfilerIsRunning, "Lua CallProfiler Is Running")                \
    XX(ErrorLuaMemProfilerIsRunning, "Lua MemProfiler Is Running")                  \
    XX(ErrorLuaJsonConfigInvalid, "Lua Json Config Invalid")                        \
    XX(ErrorLuaJsonDepthMax, "Lua Json Depth Max")                                  \
    XX(ErrorLuaJsonUserdataNotNull, "Lua Json Userdata Not Null")                   \
    XX(ErrorLuaJsonKeyTypeInvalid, "Lua Json Key Type Is Invalid")                  \
    XX(ErrorLuaJsonValueTypeInvalid, "Lua Json Value Type Is Invalid")              \
    XX(ErrorLuaXmlEncodeFailed, "Lua Xml Encode Failed")                            \
    XX(ErrorStringEmpty, "Lua Call Arg Is Empty String")                            \
    XX(ErrorLuaPanic, "Catch Lua Panic, The Error Info Look Lua Error Info")        \
    XX(ErrorLuaLoadFailed, "Lua Load Failed, The Error Info Look Lua Error Info")   \
    XX(ErrorLuaPcallFailed, "Lua Pcall Failed, The Error Info Look Lua Error Info") \
    XX(ErrorLuaCancelPcall, "Lua Cancel Pcall")                                     \
    XX(ErrorTheDataIsNil, "Lua The Data Is Nil")                                    \
    XX(ErrorStackTopNotTable, "The Lua Stack Top Is Not A Table")                   \
    XX(ErrorClassNotRegister, "The Class Not Register")                             \
    XX(ErrorTypeError, "Lua Object Type Error")                                     \
    XX(ErrorConfigCommentError, "The Config Comment Error")                         \
    XX(ErrorEnvNotInit, "The PureLuaEnv Is Not Inited")

namespace PureLua {
enum EPureLuaErrorCode {
    Success = 0,
#ifdef PURELUA_ERRORCODE_BEGIN
    __PureLuaErrorCodeSeparate = PURELUA_ERRORCODE_BEGIN - 1,
#endif
#define XX(code, desc) code,
    PURELUA_ERROR_MAP(XX)
#undef XX
};

PURELUA_API std::string get_error_desc(int err);
}  // namespace PureLua
