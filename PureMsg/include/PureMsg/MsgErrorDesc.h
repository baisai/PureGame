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

#include "PureMsg/PureMsgLib.h"

#include <string>

#define PUREMSG_ERROR_MAP(XX)                         \
    XX(ErrorMsgTypeError, "Msg Type Error")           \
    XX(ErrorEmptyData, "Data Is Empty")               \
    XX(ErrorInvalidData, "Data Is Invalid")           \
    XX(ErrorArraySizeError, "Array Size Error")       \
    XX(ErrorNullPointer, "The Pointer Is Null")       \
    XX(ErrorWriteBufferFailed, "Write Buffer Failed") \
    XX(ErrorReadBufferFailed, "Read Buffer Failed")   \
    XX(ErrorInvalidOpcode, "Invalid Opcode")

namespace PureMsg {
enum EPureMsgErrorCode {
    Success = 0,
#ifdef PUREMSG_ERRORCODE_BEGIN
    __PureMsgErrorCodeSeparate = PUREMSG_ERRORCODE_BEGIN - 1,
#endif
#define XX(code, desc) code,
    PUREMSG_ERROR_MAP(XX)
#undef XX
};

PUREMSG_API std::string get_error_desc(int err);
}  // namespace PureMsg
