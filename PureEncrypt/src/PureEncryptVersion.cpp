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

#include "PureEncrypt/PureEncryptVersion.h"

// major version
#define PUREENCRYPT_MAJOR_VERSION 1
// minor version
#define PUREENCRYPT_MINOR_VERSION 0
// micro version
#define PUREENCRYPT_MICRO_VERSION 0
// string version
#define PUREENCRYPT_VERSION_STR PUREENCRYPT_MAJOR_VERSION.PUREENCRYPT_MINOR_VERSION.PUREENCRYPT_MICRO_VERSION
// author
#define PUREENCRYPT_AUTHOR ChenDong
// str arg
#define PUREENCRYPT_STR_(str) #str
#define PUREENCRYPT_STR(str) PUREENCRYPT_STR_(str)

namespace PureEncrypt {

uint32_t get_major_version() { return PUREENCRYPT_MAJOR_VERSION; }
uint32_t get_minor_version() { return PUREENCRYPT_MINOR_VERSION; }
uint32_t get_micro_version() { return PUREENCRYPT_MICRO_VERSION; }
uint32_t get_version_num() { return get_major_version() << 24 | get_minor_version() << 16 | get_micro_version() << 8; }
const char* get_version() { return PUREENCRYPT_STR(PUREENCRYPT_VERSION_STR); }
const char* get_author() { return PUREENCRYPT_STR(PUREENCRYPT_AUTHOR); }

}  // namespace PureEncrypt
