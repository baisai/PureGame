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

#include "PureMsg/PureMsgVersion.h"

// major version
#define PUREMSG_MAJOR_VERSION 1
// minor version
#define PUREMSG_MINOR_VERSION 0
// micro version
#define PUREMSG_MICRO_VERSION 0
// string version
#define PUREMSG_VERSION_STR PUREMSG_MAJOR_VERSION.PUREMSG_MINOR_VERSION.PUREMSG_MICRO_VERSION
// author
#define PUREMSG_AUTHOR ChenDong
// str arg
#define PUREMSG_STR_(str) #str
#define PUREMSG_STR(str) PUREMSG_STR_(str)

namespace PureMsg {

uint32_t get_major_version() { return PUREMSG_MAJOR_VERSION; }
uint32_t get_minor_version() { return PUREMSG_MINOR_VERSION; }
uint32_t get_micro_version() { return PUREMSG_MICRO_VERSION; }
uint32_t get_version_num() { return get_major_version() << 24 | get_minor_version() << 16 | get_micro_version() << 8; }
const char* get_version() { return PUREMSG_STR(PUREMSG_VERSION_STR); }
const char* get_author() { return PUREMSG_STR(PUREMSG_AUTHOR); }

}  // namespace PureMsg
