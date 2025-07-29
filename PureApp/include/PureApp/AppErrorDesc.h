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

#include "PureApp/PureAppLib.h"

#include <string>

#define PUREAPP_ERROR_MAP(XX)                                \
    XX(ErrorCoreBufferFailed, "Core Buffer Operator Failed") \
    XX(ErrorNullPointer, "The Pointer Is Null")              \
    XX(ErrorMemoryNotEnough, "The Memory Not Enough")        \
    XX(ErrorInvalidArg, "The Arg Is Invalid")                \
    XX(ErrorInvalidData, "The Data Is Invalid")              \
    XX(ErrorExeInitFailed, "The Exe Init Failed")            \
    XX(ErrorInvalidState, "The State Is Invalid")            \
    XX(ErrorAppInitFailed, "The App Init Failed")            \
    XX(ErrorNotFoundFile, "Not Found File")

namespace PureApp {
enum EPureAppErrorCode {
    Success = 0,
#ifdef PUREAPP_ERRORCODE_BEGIN
    __PureAppErrorCodeSeparate = PUREAPP_ERRORCODE_BEGIN - 1,
#endif
#define XX(code, desc) code,
    PUREAPP_ERROR_MAP(XX)
#undef XX
};

PUREAPP_API std::string get_error_desc(int err);
}  // namespace PureApp
