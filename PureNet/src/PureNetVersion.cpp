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

#include "PureNet/PureNetVersion.h"

// major version
#define PURENET_MAJOR_VERSION 1
// minor version
#define PURENET_MINOR_VERSION 0
// micro version
#define PURENET_MICRO_VERSION 0
// string version
#define PURENET_VERSION_STR PURENET_MAJOR_VERSION.PURENET_MINOR_VERSION.PURENET_MICRO_VERSION
// author
#define PURENET_AUTHOR ChenDong
// str arg
#define PURENET_STR_(str) #str
#define PURENET_STR(str) PURENET_STR_(str)

namespace PureNet {

uint32_t get_major_version() { return PURENET_MAJOR_VERSION; }
uint32_t get_minor_version() { return PURENET_MINOR_VERSION; }
uint32_t get_micro_version() { return PURENET_MICRO_VERSION; }
uint32_t get_version_num() { return get_major_version() << 24 | get_minor_version() << 16 | get_micro_version() << 8; }
const char* get_version() { return PURENET_STR(PURENET_VERSION_STR); }
const char* get_author() { return PURENET_STR(PURENET_AUTHOR); }

}  // namespace PureNet
