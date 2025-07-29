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

#include "PureApp/PureAppVersion.h"

// major version
#define PUREAPP_MAJOR_VERSION 1
// minor version
#define PUREAPP_MINOR_VERSION 0
// micro version
#define PUREAPP_MICRO_VERSION 0
// string version
#define PUREAPP_VERSION_STR PUREAPP_MAJOR_VERSION.PUREAPP_MINOR_VERSION.PUREAPP_MICRO_VERSION
// author
#define PUREAPP_AUTHOR ChenDong
// str arg
#define PUREAPP_STR_(str) #str
#define PUREAPP_STR(str) PUREAPP_STR_(str)

namespace PureApp {

uint32_t get_major_version() { return PUREAPP_MAJOR_VERSION; }
uint32_t get_minor_version() { return PUREAPP_MINOR_VERSION; }
uint32_t get_micro_version() { return PUREAPP_MICRO_VERSION; }
uint32_t get_version_num() { return get_major_version() << 24 | get_minor_version() << 16 | get_micro_version() << 8; }
const char* get_version() { return PUREAPP_STR(PUREAPP_VERSION_STR); }
const char* get_author() { return PUREAPP_STR(PUREAPP_AUTHOR); }

}  // namespace PureApp
