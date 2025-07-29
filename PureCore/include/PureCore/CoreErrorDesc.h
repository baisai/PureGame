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

#include "PureCore/PureCoreLib.h"

#include <string>

#define PURECORE_ERROR_MAP(XX)                                     \
    XX(ErrorLogNotStart, "The Log Not Start")                      \
    XX(ErrorLogAddSinkFailed, "The Log Add Sink Failed")           \
    XX(ErrorNotSupport, "Not Support This Operator")               \
    XX(ErrorNullPointer, "The Pointer Is Null")                    \
    XX(ErrorAllocMemoryFailed, "Alloc Memory Failed")              \
    XX(ErrorInvalidMemory, "Invalid Memory")                       \
    XX(ErrorInvalidData, "Invalid Data")                           \
    XX(ErrorOpenFileFailed, "Open File Failed")                    \
    XX(ErrorReadFileFailed, "Read File Failed")                    \
    XX(ErrorWriteFileFailed, "Write File Failed")                  \
    XX(ErrorLoggerInvalid, "The Logger Is Invalid")                \
    XX(ErrorInvalidUtf8, "The Char Is Invalid Utf8")               \
    XX(ErrorInvalidUtf16, "The Char Is Invalid Utf16")             \
    XX(ErrorInvalidUtf32, "The Char Is Invalid Utf32")             \
    XX(ErrorInvalidWstring, "The Char Is Invalid std::wstring")    \
    XX(ErrorMemoryNotEnough, "The Memory Not Enough")              \
    XX(ErrorDataOutOfRange, "The Data Is Out of Range")            \
    XX(ErrorBufferNotEnough, "The Buffer Is Not Enough")           \
    XX(ErrorInvalidArg, "The Arg Is Invalid")                      \
    XX(ErrorThreadAlreadyRunning, "The Thread Is Already Running") \
    XX(ErrorNotJoinSelfThread, "Can't Join Self Thread")           \
    XX(ErrorTaskAlreadyRunning, "Task Is Already Running")         \
    XX(ErrorTaskIsStoped, "Task Is Stoped")

namespace PureCore {
enum EPureCoreErrorCode {
    Success = 0,
#ifdef PURECORE_ERRORCODE_BEGIN
    __PureCoreErrorCodeSeparate = PURECORE_ERRORCODE_BEGIN - 1,
#endif
#define XX(code, desc) code,
    PURECORE_ERROR_MAP(XX)
#undef XX
};

PURECORE_API std::string get_error_desc(int err);
}  // namespace PureCore
