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

#define _msgpack_head_positive_fixint_from 0x00u
#define _msgpack_head_positive_fixint_to 0x7fu

#define _msgpack_head_fixmap_from 0x80u
#define _msgpack_head_fixmap_to 0x8fu

#define _msgpack_head_fixarray_from 0x90u
#define _msgpack_head_fixarray_to 0x9fu

#define _msgpack_head_fixstr_from 0xa0u
#define _msgpack_head_fixstr_to 0xbfu

#define _msgpack_head_nil 0xc0u

#define _msgpack_head_invalid 0xc1u

#define _msgpack_head_false 0xc2u
#define _msgpack_head_true 0xc3u

#define _msgpack_head_bin8 0xc4u
#define _msgpack_head_bin16 0xc5u
#define _msgpack_head_bin32 0xc6u

#define _msgpack_head_obj8 0xc7u
#define _msgpack_head_obj16 0xc8u
#define _msgpack_head_obj32 0xc9u

#define _msgpack_head_float32 0xcau
#define _msgpack_head_float64 0xcbu

#define _msgpack_head_uint8 0xccu
#define _msgpack_head_uint16 0xcdu
#define _msgpack_head_uint32 0xceu
#define _msgpack_head_uint64 0xcfu

#define _msgpack_head_int8 0xd0u
#define _msgpack_head_int16 0xd1u
#define _msgpack_head_int32 0xd2u
#define _msgpack_head_int64 0xd3u

// #define _msgpack_head_fixext1 0xd4u
// #define _msgpack_head_fixext2 0xd5u
// #define _msgpack_head_fixext4 0xd6u
// #define _msgpack_head_fixext8 0xd7u
// #define _msgpack_head_fixext16 0xd8u

#define _msgpack_head_str8 0xd9u
#define _msgpack_head_str16 0xdau
#define _msgpack_head_str32 0xdbu

#define _msgpack_head_array16 0xdcu
#define _msgpack_head_array32 0xddu

#define _msgpack_head_map16 0xdeu
#define _msgpack_head_map32 0xdfu

#define _msgpack_head_negative_fixint_from 0xe0u
#define _msgpack_head_negative_fixint_to 0xffu