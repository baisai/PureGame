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

#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
#define PURE_DEBUG 1
#endif  // DEBUG

#if defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64) || defined(__aarch64__) || defined(__ppc64__)
#define PURE_X64
#endif

#ifdef _MSVC_LANG
#define PURE_CPP _MSVC_LANG
#else
#define PURE_CPP __cplusplus
#endif

#ifndef PURECORE_ERRORCODE_BEGIN
#define PURECORE_ERRORCODE_BEGIN 1
#endif

// PURECORE_API
#ifdef PURECORE_SHARED_LIB
#ifdef _WIN32
#ifdef PURECORE_SHARED_EXPORT
#define PURECORE_API __declspec(dllexport)
#else
#define PURECORE_API __declspec(dllimport)
#endif
#else
#define PURECORE_API __attribute__((visibility("default")))
#endif
#else
#define PURECORE_API
#endif

#define PURE_DISABLE_COPY(name)  \
    name(const name &) = delete; \
    name &operator=(const name &) = delete;

#define PURE_ONLE_HEAP_NEW(name) ~name() = delete;

#define PURE_DISABLE_CRAETE(name)           \
    name() = delete;                        \
    name(const name &) = delete;            \
    name &operator=(const name &) = delete; \
    ~name() = delete;

#define PURE_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define PURE_STR_EMPTY(str) ((str) == nullptr || (str)[0] == 0)

#if !defined(PURE_BIG_SIZE)
#define PURE_BIG_SIZE 1024  // big object size
#endif
#if !defined(PURE_ALIGN_BIT)
#define PURE_ALIGN_BIT 3  // memory align
#endif
