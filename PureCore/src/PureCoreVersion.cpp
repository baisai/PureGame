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

#include "PureCore/PureCoreVersion.h"

// major version
#define PURECORE_MAJOR_VERSION 1
// minor version
#define PURECORE_MINOR_VERSION 0
// micro version
#define PURECORE_MICRO_VERSION 0
// string version
#define PURECORE_VERSION_STR PURECORE_MAJOR_VERSION.PURECORE_MINOR_VERSION.PURECORE_MICRO_VERSION
// author
#define PURECORE_AUTHOR ChenDong
// str arg
#define PURECORE_STR_(str) #str
#define PURECORE_STR(str) PURECORE_STR_(str)

namespace PureCore {

uint32_t get_major_version() { return PURECORE_MAJOR_VERSION; }
uint32_t get_minor_version() { return PURECORE_MINOR_VERSION; }
uint32_t get_micro_version() { return PURECORE_MICRO_VERSION; }
uint32_t get_version_num() { return get_major_version() << 24 | get_minor_version() << 16 | get_micro_version() << 8; }
const char* get_version() { return PURECORE_STR(PURECORE_VERSION_STR); }
const char* get_author() { return PURECORE_STR(PURECORE_AUTHOR); }

}  // namespace PureCore
