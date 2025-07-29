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

#include <stdint.h>

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif
#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif

#ifndef BYTE_ORDER
#if defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN) || defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__) || \
    defined(_STLP_LITTLE_ENDIAN) && !defined(_STLP_BIG_ENDIAN)

#define BYTE_ORDER LITTLE_ENDIAN

#elif defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN) || defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__) || \
    defined(_STLP_BIG_ENDIAN) && !defined(_STLP_LITTLE_ENDIAN)

#define BYTE_ORDER BIG_ENDIAN

#elif defined(__sparc) || defined(__sparc__) || defined(_POWER) || defined(__powerpc__) || defined(__ppc__) || defined(__hpux) || defined(__hppa) || \
    defined(_MIPSEB) || defined(__s390__)

#define BYTE_ORDER BIG_ENDIAN

#elif defined(__i386__) || defined(__alpha__) || defined(__ia64) || defined(__ia64__) || defined(_M_IX86) || defined(_M_IA64) || defined(_M_ALPHA) || \
    defined(__amd64) || defined(__amd64__) || defined(_M_AMD64) || defined(__x86_64) || defined(__x86_64__) || defined(_M_X64) || defined(__bfin__)

#define BYTE_ORDER LITTLE_ENDIAN

#endif
#endif

#ifndef BYTE_ORDER
#error Need Define BYTE_ORDER
#endif  // !BYTE_ORDER

#if defined(linux) || defined(__linux__) || defined(__GLIBC__) || defined(__GNU__)

#include <byteswap.h>
#define bswap16(x) __bswap_16(x)
#define bswap32(x) __bswap_32(x)
#define bswap64(x) __bswap_64(x)

#elif defined(__FreeBSD__) || defined(__DragonFlyBSD__) || defined(__NetBSD__)

#include <sys/endian.h>
// native bswapNN funcs/macros definied

#elif defined(__OpenBSD__)

#include <sys/endian.h>
#define bswap16(x) __swap16(x)
#define bswap32(x) __swap32(x)
#define bswap64(x) __swap64(x)

#elif defined(__APPLE__)

#include <libkern/OSByteOrder.h>
#define bswap16(x) OSSwapInt16(x)
#define bswap32(x) OSSwapInt32(x)
#define bswap64(x) OSSwapInt64(x)

#elif defined(sun) || defined(__sun)

#include <sys/byteorder.h>
#define bswap16(x) BSWAP_16(x)
#define bswap32(x) BSWAP_32(x)
#define bswap64(x) BSWAP_64(x)

#elif defined(_MSC_VER)

#include <intrin.h>
#define bswap16(x) _byteswap_ushort(x)
#define bswap32(x) _byteswap_ulong(x)
#define bswap64(x) _byteswap_uint64(x)

#else

#define bswap16(x) ((((uint16_t)(x) & 0xff00) >> 8) | (((uint16_t)(x) & 0x00ff) << 8))

#define bswap32(x) \
    ((((uint32_t)(x) & 0xff000000) >> 24) | (((uint32_t)(x) & 0x00ff0000) >> 8) | (((uint32_t)(x) & 0x0000ff00) << 8) | (((uint32_t)(x) & 0x000000ff) << 24))

#define bswap64(x)                                                                                                                                         \
    ((((uint64_t)(x) & 0xff00000000000000ull) >> 56) | (((uint64_t)(x) & 0x00ff000000000000ull) >> 40) | (((uint64_t)(x) & 0x0000ff0000000000ull) >> 24) | \
     (((uint64_t)(x) & 0x000000ff00000000ull) >> 8) | (((uint64_t)(x) & 0x00000000ff000000ull) << 8) | (((uint64_t)(x) & 0x0000000000ff0000ull) << 24) |   \
     (((uint64_t)(x) & 0x000000000000ff00ull) << 40) | (((uint64_t)(x) & 0x00000000000000ffull) << 56))

#endif

#if BYTE_ORDER == LITTLE_ENDIAN

#define _pure_byte_be16(x) bswap16(x)
#define _pure_byte_be32(x) bswap32(x)
#define _pure_byte_be64(x) bswap64(x)

#else

#define _pure_byte_be16(x) (x)
#define _pure_byte_be32(x) (x)
#define _pure_byte_be64(x) (x)

#endif

#define _pure_byte_load16(cast, from, to)          \
    do {                                           \
        memcpy((cast*)(to), (from), sizeof(cast)); \
        *(to) = _pure_byte_be16(*(to));            \
    } while (0);

#define _pure_byte_load32(cast, from, to)          \
    do {                                           \
        memcpy((cast*)(to), (from), sizeof(cast)); \
        *(to) = _pure_byte_be32(*(to));            \
    } while (0);
#define _pure_byte_load64(cast, from, to)          \
    do {                                           \
        memcpy((cast*)(to), (from), sizeof(cast)); \
        *(to) = _pure_byte_be64(*(to));            \
    } while (0);

#define _pure_byte_store16(to, num)          \
    do {                                     \
        uint16_t val = _pure_byte_be16(num); \
        memcpy(to, &val, 2);                 \
    } while (0)
#define _pure_byte_store32(to, num)          \
    do {                                     \
        uint32_t val = _pure_byte_be32(num); \
        memcpy(to, &val, 4);                 \
    } while (0)
#define _pure_byte_store64(to, num)          \
    do {                                     \
        uint64_t val = _pure_byte_be64(num); \
        memcpy(to, &val, 8);                 \
    } while (0)
