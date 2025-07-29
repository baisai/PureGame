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

#include "PureCore/CoreErrorDesc.h"
#include "PureCore/ByteOrder.h"
#include "PureMsg/MsgHead.h"
#include "PureMsg/Packer.h"
#include "PureMsg/MsgErrorDesc.h"

#include <climits>

namespace PureMsg {
#if BYTE_ORDER == LITTLE_ENDIAN
template <typename T>
inline uint8_t take8_8(T d) {
    return reinterpret_cast<uint8_t*>(&d)[0];
}
template <typename T>
inline uint8_t take8_16(T d) {
    return reinterpret_cast<uint8_t*>(&d)[0];
}
template <typename T>
inline uint8_t take8_32(T d) {
    return reinterpret_cast<uint8_t*>(&d)[0];
}
template <typename T>
inline uint8_t take8_64(T d) {
    return reinterpret_cast<uint8_t*>(&d)[0];
}

#elif BYTE_ORDER == BIG_ENDIAN

template <typename T>
inline uint8_t take8_8(T d) {
    return reinterpret_cast<uint8_t*>(&d)[0];
}
template <typename T>
inline uint8_t take8_16(T d) {
    return reinterpret_cast<uint8_t*>(&d)[1];
}
template <typename T>
inline uint8_t take8_32(T d) {
    return reinterpret_cast<uint8_t*>(&d)[3];
}
template <typename T>
inline uint8_t take8_64(T d) {
    return reinterpret_cast<uint8_t*>(&d)[7];
}

#endif

inline int write(PureCore::IBuffer& dest, const uint8_t* src, size_t srcSize) {
    if (dest.write(PureCore::DataRef(src, srcSize)) != PureCore::Success) {
        return ErrorWriteBufferFailed;
    }
    return Success;
}

inline int write(PureCore::IBuffer& dest, PureCore::DataRef src) {
    if (dest.write(src) != PureCore::Success) {
        return ErrorWriteBufferFailed;
    }
    return Success;
}

template <typename T>
int pack_imp_uint8(PureCore::IBuffer& buffer, T d) {
    if (d < (1 << 7)) {
        /* fixnum */
        uint8_t buf = take8_8(d);
        return write(buffer, &buf, 1);
    } else {
        /* unsigned 8 */
        uint8_t buf[2] = {static_cast<uint8_t>(_msgpack_head_uint8), take8_8(d)};
        return write(buffer, buf, 2);
    }
}

template <typename T>
int pack_imp_uint16(PureCore::IBuffer& buffer, T d) {
    if (d < (1 << 7)) {
        /* fixnum */
        uint8_t buf = take8_16(d);
        return write(buffer, &buf, 1);
    } else if (d < (1 << 8)) {
        /* unsigned 8 */
        uint8_t buf[2] = {static_cast<uint8_t>(_msgpack_head_uint8), take8_16(d)};
        return write(buffer, buf, 2);
    } else {
        /* unsigned 16 */
        uint8_t buf[3];
        buf[0] = static_cast<uint8_t>(_msgpack_head_uint16);
        _pure_byte_store16(&buf[1], static_cast<uint16_t>(d));
        return write(buffer, buf, 3);
    }
}

template <typename T>
int pack_imp_uint32(PureCore::IBuffer& buffer, T d) {
    if (d < (1 << 8)) {
        if (d < (1 << 7)) {
            /* fixnum */
            uint8_t buf = take8_32(d);
            return write(buffer, &buf, 1);
        } else {
            /* unsigned 8 */
            uint8_t buf[2] = {static_cast<uint8_t>(_msgpack_head_uint8), take8_32(d)};
            return write(buffer, buf, 2);
        }
    } else {
        if (d < (1 << 16)) {
            /* unsigned 16 */
            uint8_t buf[3];
            buf[0] = static_cast<uint8_t>(_msgpack_head_uint16);
            _pure_byte_store16(&buf[1], static_cast<uint16_t>(d));
            return write(buffer, buf, 3);
        } else {
            /* unsigned 32 */
            uint8_t buf[5];
            buf[0] = static_cast<uint8_t>(_msgpack_head_uint32);
            _pure_byte_store32(&buf[1], static_cast<uint32_t>(d));
            return write(buffer, buf, 5);
        }
    }
}

template <typename T>
int pack_imp_uint64(PureCore::IBuffer& buffer, T d) {
    if (d < (1ULL << 8)) {
        if (d < (1ULL << 7)) {
            /* fixnum */
            uint8_t buf = take8_64(d);
            return write(buffer, &buf, 1);
        } else {
            /* unsigned 8 */
            uint8_t buf[2] = {static_cast<uint8_t>(_msgpack_head_uint8), take8_64(d)};
            return write(buffer, buf, 2);
        }
    } else {
        if (d < (1ULL << 16)) {
            /* unsigned 16 */
            uint8_t buf[3];
            buf[0] = static_cast<uint8_t>(_msgpack_head_uint16);
            _pure_byte_store16(&buf[1], static_cast<uint16_t>(d));
            return write(buffer, buf, 3);
        } else if (d < (1ULL << 32)) {
            /* unsigned 32 */
            uint8_t buf[5];
            buf[0] = static_cast<uint8_t>(_msgpack_head_uint32);
            _pure_byte_store32(&buf[1], static_cast<uint32_t>(d));
            return write(buffer, buf, 5);
        } else {
            /* unsigned 64 */
            uint8_t buf[9];
            buf[0] = static_cast<uint8_t>(_msgpack_head_uint64);
            _pure_byte_store64(&buf[1], d);
            return write(buffer, buf, 9);
        }
    }
}

template <typename T>
int pack_imp_int8(PureCore::IBuffer& buffer, T d) {
    if (d < -(1 << 5)) {
        /* signed 8 */
        uint8_t buf[2] = {static_cast<uint8_t>(_msgpack_head_int8), take8_8(d)};
        return write(buffer, buf, 2);
    } else {
        /* fixnum */
        uint8_t buf = take8_8(d);
        return write(buffer, &buf, 1);
    }
}

template <typename T>
int pack_imp_int16(PureCore::IBuffer& buffer, T d) {
    if (d < -(1 << 5)) {
        if (d < -(1 << 7)) {
            /* signed 16 */
            uint8_t buf[3];
            buf[0] = static_cast<uint8_t>(_msgpack_head_int16);
            _pure_byte_store16(&buf[1], static_cast<int16_t>(d));
            return write(buffer, buf, 3);
        } else {
            /* signed 8 */
            uint8_t buf[2] = {static_cast<uint8_t>(_msgpack_head_int8), take8_16(d)};
            return write(buffer, buf, 2);
        }
    } else if (d < (1 << 7)) {
        /* fixnum */
        uint8_t buf = take8_16(d);
        return write(buffer, &buf, 1);
    } else {
        if (d < (1 << 8)) {
            /* unsigned 8 */
            uint8_t buf[2] = {static_cast<uint8_t>(_msgpack_head_uint8), take8_16(d)};
            return write(buffer, buf, 2);
        } else {
            /* unsigned 16 */
            uint8_t buf[3];
            buf[0] = static_cast<uint8_t>(_msgpack_head_uint16);
            _pure_byte_store16(&buf[1], static_cast<uint16_t>(d));
            return write(buffer, buf, 3);
        }
    }
}

template <typename T>
int pack_imp_int32(PureCore::IBuffer& buffer, T d) {
    if (d < -(1 << 5)) {
        if (d < -(1 << 15)) {
            /* signed 32 */
            uint8_t buf[5];
            buf[0] = static_cast<uint8_t>(_msgpack_head_int32);
            _pure_byte_store32(&buf[1], static_cast<int32_t>(d));
            return write(buffer, buf, 5);
        } else if (d < -(1 << 7)) {
            /* signed 16 */
            uint8_t buf[3];
            buf[0] = static_cast<uint8_t>(_msgpack_head_int16);
            _pure_byte_store16(&buf[1], static_cast<int16_t>(d));
            return write(buffer, buf, 3);
        } else {
            /* signed 8 */
            uint8_t buf[2] = {static_cast<uint8_t>(_msgpack_head_int8), take8_32(d)};
            return write(buffer, buf, 2);
        }
    } else if (d < (1 << 7)) {
        /* fixnum */
        uint8_t buf = take8_32(d);
        return write(buffer, &buf, 1);
    } else {
        if (d < (1 << 8)) {
            /* unsigned 8 */
            uint8_t buf[2] = {static_cast<uint8_t>(_msgpack_head_uint8), take8_32(d)};
            return write(buffer, buf, 2);
        } else if (d < (1 << 16)) {
            /* unsigned 16 */
            uint8_t buf[3];
            buf[0] = static_cast<uint8_t>(_msgpack_head_uint16);
            _pure_byte_store16(&buf[1], static_cast<uint16_t>(d));
            return write(buffer, buf, 3);
        } else {
            /* unsigned 32 */
            uint8_t buf[5];
            buf[0] = static_cast<uint8_t>(_msgpack_head_uint32);
            _pure_byte_store32(&buf[1], static_cast<uint32_t>(d));
            return write(buffer, buf, 5);
        }
    }
}

template <typename T>
int pack_imp_int64(PureCore::IBuffer& buffer, T d) {
    if (d < -(1LL << 5)) {
        if (d < -(1LL << 15)) {
            if (d < -(1LL << 31)) {
                /* signed 64 */
                uint8_t buf[9];
                buf[0] = static_cast<uint8_t>(_msgpack_head_int64);
                _pure_byte_store64(&buf[1], d);
                return write(buffer, buf, 9);
            } else {
                /* signed 32 */
                uint8_t buf[5];
                buf[0] = static_cast<uint8_t>(_msgpack_head_int32);
                _pure_byte_store32(&buf[1], static_cast<int32_t>(d));
                return write(buffer, buf, 5);
            }
        } else {
            if (d < -(1 << 7)) {
                /* signed 16 */
                uint8_t buf[3];
                buf[0] = static_cast<uint8_t>(_msgpack_head_int16);
                _pure_byte_store16(&buf[1], static_cast<int16_t>(d));
                return write(buffer, buf, 3);
            } else {
                /* signed 8 */
                uint8_t buf[2] = {static_cast<uint8_t>(_msgpack_head_int8), take8_64(d)};
                return write(buffer, buf, 2);
            }
        }
    } else if (d < (1 << 7)) {
        /* fixnum */
        uint8_t buf = take8_64(d);
        return write(buffer, &buf, 1);
    } else {
        if (d < (1LL << 16)) {
            if (d < (1 << 8)) {
                /* unsigned 8 */
                uint8_t buf[2] = {static_cast<uint8_t>(_msgpack_head_uint8), take8_64(d)};
                return write(buffer, buf, 2);
            } else {
                /* unsigned 16 */
                uint8_t buf[3];
                buf[0] = static_cast<uint8_t>(_msgpack_head_uint16);
                _pure_byte_store16(&buf[1], static_cast<uint16_t>(d));
                return write(buffer, buf, 3);
            }
        } else {
            if (d < (1LL << 32)) {
                /* unsigned 32 */
                uint8_t buf[5];
                buf[0] = static_cast<uint8_t>(_msgpack_head_uint32);
                _pure_byte_store32(&buf[1], static_cast<uint32_t>(d));
                return write(buffer, buf, 5);
            } else {
                /* unsigned 64 */
                uint8_t buf[9];
                buf[0] = static_cast<uint8_t>(_msgpack_head_uint64);
                _pure_byte_store64(&buf[1], d);
                return write(buffer, buf, 9);
            }
        }
    }
}

int pack_uint8(PureCore::IBuffer& buffer, uint8_t d) { return pack_imp_uint8(buffer, d); }

int pack_uint16(PureCore::IBuffer& buffer, uint16_t d) { return pack_imp_uint16(buffer, d); }

int pack_uint32(PureCore::IBuffer& buffer, uint32_t d) { return pack_imp_uint32(buffer, d); }

int pack_uint64(PureCore::IBuffer& buffer, uint64_t d) { return pack_imp_uint64(buffer, d); }

int pack_int8(PureCore::IBuffer& buffer, int8_t d) { return pack_imp_int8(buffer, d); }

int pack_int16(PureCore::IBuffer& buffer, int16_t d) { return pack_imp_int16(buffer, d); }

int pack_int32(PureCore::IBuffer& buffer, int32_t d) { return pack_imp_int32(buffer, d); }

int pack_int64(PureCore::IBuffer& buffer, int64_t d) { return pack_imp_int64(buffer, d); }

int pack_fixed_uint8(PureCore::IBuffer& buffer, uint8_t d) {
    uint8_t buf[2] = {static_cast<uint8_t>(_msgpack_head_uint8), take8_8(d)};
    return write(buffer, buf, 2);
}

int pack_fixed_uint16(PureCore::IBuffer& buffer, uint16_t d) {
    uint8_t buf[3];
    buf[0] = static_cast<uint8_t>(_msgpack_head_uint16);
    _pure_byte_store16(&buf[1], d);
    return write(buffer, buf, 3);
}

int pack_fixed_uint32(PureCore::IBuffer& buffer, uint32_t d) {
    uint8_t buf[5];
    buf[0] = static_cast<uint8_t>(_msgpack_head_uint32);
    _pure_byte_store32(&buf[1], d);
    return write(buffer, buf, 5);
}

int pack_fixed_uint64(PureCore::IBuffer& buffer, uint64_t d) {
    uint8_t buf[9];
    buf[0] = static_cast<uint8_t>(_msgpack_head_uint64);
    _pure_byte_store64(&buf[1], d);
    return write(buffer, buf, 9);
}

int pack_fixed_int8(PureCore::IBuffer& buffer, int8_t d) {
    uint8_t buf[2] = {static_cast<uint8_t>(_msgpack_head_int8), take8_8(d)};
    return write(buffer, buf, 2);
}

int pack_fixed_int16(PureCore::IBuffer& buffer, int16_t d) {
    uint8_t buf[3];
    buf[0] = static_cast<uint8_t>(_msgpack_head_int16);
    _pure_byte_store16(&buf[1], d);
    return write(buffer, buf, 3);
}

int pack_fixed_int32(PureCore::IBuffer& buffer, int32_t d) {
    uint8_t buf[5];
    buf[0] = static_cast<uint8_t>(_msgpack_head_int32);
    _pure_byte_store32(&buf[1], d);
    return write(buffer, buf, 5);
}

int pack_fixed_int64(PureCore::IBuffer& buffer, int64_t d) {
    uint8_t buf[9];
    buf[0] = static_cast<uint8_t>(_msgpack_head_int64);
    _pure_byte_store64(&buf[1], d);
    return write(buffer, buf, 9);
}

int pack_char(PureCore::IBuffer& buffer, char d) {
#if defined(CHAR_MIN)
#if CHAR_MIN < 0
    return pack_imp_int8(buffer, d);
#else
    return pack_imp_uint8(buffer, d);
#endif
#else
#error CHAR_MIN is not defined
#endif
}

int pack_signed_char(PureCore::IBuffer& buffer, signed char d) { return pack_imp_int8(buffer, d); }

int pack_short(PureCore::IBuffer& buffer, short d) {
#if defined(SIZEOF_SHORT)
#if SIZEOF_SHORT == 2
    return pack_imp_int16(buffer, d);
#elif SIZEOF_SHORT == 4
    return pack_imp_int32(buffer, d);
#else
    return pack_imp_int64(buffer, d);
#endif

#elif defined(SHRT_MAX)
#if SHRT_MAX == 0x7fff
    return pack_imp_int16(buffer, d);
#elif SHRT_MAX == 0x7fffffff
    return pack_imp_int32(buffer, d);
#else
    return pack_imp_int64(buffer, d);
#endif

#else
    if (sizeof(short) == 2) {
        return pack_imp_int16(buffer, d);
    } else if (sizeof(short) == 4) {
        return pack_imp_int32(buffer, d);
    } else {
        return pack_imp_int64(buffer, d);
    }
#endif
}

int pack_int(PureCore::IBuffer& buffer, int d) {
#if defined(SIZEOF_INT)
#if SIZEOF_INT == 2
    return pack_imp_int16(buffer, d);
#elif SIZEOF_INT == 4
    return pack_imp_int32(buffer, d);
#else
    return pack_imp_int64(buffer, d);
#endif

#elif defined(INT_MAX)
#if INT_MAX == 0x7fff
    return pack_imp_int16(buffer, d);
#elif INT_MAX == 0x7fffffff
    return pack_imp_int32(buffer, d);
#else
    return pack_imp_int64(buffer, d);
#endif

#else
    if (sizeof(int) == 2) {
        return pack_imp_int16(buffer, d);
    } else if (sizeof(int) == 4) {
        return pack_imp_int32(buffer, d);
    } else {
        return pack_imp_int64(buffer, d);
    }
#endif
}

int pack_long(PureCore::IBuffer& buffer, long d) {
#if defined(SIZEOF_LONG)
#if SIZEOF_LONG == 2
    return pack_imp_int16(buffer, d);
#elif SIZEOF_LONG == 4
    return pack_imp_int32(buffer, d);
#else
    return pack_imp_int64(buffer, d);
#endif

#elif defined(LONG_MAX)
#if LONG_MAX == 0x7fffL
    return pack_imp_int16(buffer, d);
#elif LONG_MAX == 0x7fffffffL
    return pack_imp_int32(buffer, d);
#else
    return pack_imp_int64(buffer, d);
#endif

#else
    if (sizeof(long) == 2) {
        return pack_imp_int16(buffer, d);
    } else if (sizeof(long) == 4) {
        return pack_imp_int32(buffer, d);
    } else {
        return pack_imp_int64(buffer, d);
    }
#endif
}

int pack_long_long(PureCore::IBuffer& buffer, long long d) {
#if defined(SIZEOF_LONG_LONG)
#if SIZEOF_LONG_LONG == 2
    return pack_imp_int16(buffer, d);
#elif SIZEOF_LONG_LONG == 4
    return pack_imp_int32(buffer, d);
#else
    return pack_imp_int64(buffer, d);
#endif

#elif defined(LLONG_MAX)
#if LLONG_MAX == 0x7fffL
    return pack_imp_int16(buffer, d);
#elif LLONG_MAX == 0x7fffffffL
    return pack_imp_int32(buffer, d);
#else
    return pack_imp_int64(buffer, d);
#endif

#else
    if (sizeof(long long) == 2) {
        return pack_imp_int16(buffer, d);
    } else if (sizeof(long long) == 4) {
        return pack_imp_int32(buffer, d);
    } else {
        return pack_imp_int64(buffer, d);
    }
#endif
}

int pack_unsigned_char(PureCore::IBuffer& buffer, unsigned char d) { return pack_imp_uint8(buffer, d); }

int pack_unsigned_short(PureCore::IBuffer& buffer, unsigned short d) {
#if defined(SIZEOF_SHORT)
#if SIZEOF_SHORT == 2
    return pack_imp_uint16(buffer, d);
#elif SIZEOF_SHORT == 4
    return pack_imp_uint32(buffer, d);
#else
    return pack_imp_uint64(buffer, d);
#endif

#elif defined(USHRT_MAX)
#if USHRT_MAX == 0xffffU
    return pack_imp_uint16(buffer, d);
#elif USHRT_MAX == 0xffffffffU
    return pack_imp_uint32(buffer, d);
#else
    return pack_imp_uint64(buffer, d);
#endif

#else
    if (sizeof(unsigned short) == 2) {
        return pack_imp_uint16(buffer, d);
    } else if (sizeof(unsigned short) == 4) {
        return pack_imp_uint32(buffer, d);
    } else {
        return pack_imp_uint64(buffer, d);
    }
#endif
}

int pack_unsigned_int(PureCore::IBuffer& buffer, unsigned int d) {
#if defined(SIZEOF_INT)
#if SIZEOF_INT == 2
    return pack_imp_uint16(buffer, d);
#elif SIZEOF_INT == 4
    return pack_imp_uint32(buffer, d);
#else
    return pack_imp_uint64(buffer, d);
#endif

#elif defined(UINT_MAX)
#if UINT_MAX == 0xffffU
    return pack_imp_uint16(buffer, d);
#elif UINT_MAX == 0xffffffffU
    return pack_imp_uint32(buffer, d);
#else
    return pack_imp_uint64(buffer, d);
#endif

#else
    if (sizeof(unsigned int) == 2) {
        return pack_imp_uint16(buffer, d);
    } else if (sizeof(unsigned int) == 4) {
        return pack_imp_uint32(buffer, d);
    } else {
        return pack_imp_uint64(buffer, d);
    }
#endif
}

int pack_unsigned_long(PureCore::IBuffer& buffer, unsigned long d) {
#if defined(SIZEOF_LONG)
#if SIZEOF_LONG == 2
    return pack_imp_uint16(buffer, d);
#elif SIZEOF_LONG == 4
    return pack_imp_uint32(buffer, d);
#else
    return pack_imp_uint64(buffer, d);
#endif

#elif defined(ULONG_MAX)
#if ULONG_MAX == 0xffffUL
    return pack_imp_uint16(buffer, d);
#elif ULONG_MAX == 0xffffffffUL
    return pack_imp_uint32(buffer, d);
#else
    return pack_imp_uint64(buffer, d);
#endif

#else
    if (sizeof(unsigned long) == 2) {
        return pack_imp_uint16(buffer, d);
    } else if (sizeof(unsigned long) == 4) {
        return pack_imp_uint32(buffer, d);
    } else {
        return pack_imp_uint64(buffer, d);
    }
#endif
}

int pack_unsigned_long_long(PureCore::IBuffer& buffer, unsigned long long d) {
#if defined(SIZEOF_LONG_LONG)
#if SIZEOF_LONG_LONG == 2
    return pack_imp_uint16(buffer, d);
#elif SIZEOF_LONG_LONG == 4
    return pack_imp_uint32(buffer, d);
#else
    return pack_imp_uint64(buffer, d);
#endif

#elif defined(ULLONG_MAX)
#if ULLONG_MAX == 0xffffUL
    return pack_imp_uint16(buffer, d);
#elif ULLONG_MAX == 0xffffffffUL
    return pack_imp_uint32(buffer, d);
#else
    return pack_imp_uint64(buffer, d);
#endif

#else
    if (sizeof(unsigned long long) == 2) {
        return pack_imp_uint16(buffer, d);
    } else if (sizeof(unsigned long long) == 4) {
        return pack_imp_uint32(buffer, d);
    } else {
        return pack_imp_uint64(buffer, d);
    }
#endif
}

int pack_float(PureCore::IBuffer& buffer, float d) {
    union {
        float f;
        uint32_t i;
    } mem;
    mem.f = d;
    uint8_t buf[5];
    buf[0] = static_cast<uint8_t>(_msgpack_head_float32);
    _pure_byte_store32(&buf[1], mem.i);
    return write(buffer, buf, 5);
}

int pack_double(PureCore::IBuffer& buffer, double d) {
    union {
        double f;
        uint64_t i;
    } mem;
    mem.f = d;
    uint8_t buf[9];
    buf[0] = static_cast<uint8_t>(_msgpack_head_float64);
#if defined(__arm__) && !(__ARM_EABI__)  // arm-oabi
    // https://github.com/msgpack/msgpack-perl/pull/1
    mem.i = (mem.i & 0xFFFFFFFFUL) << 32UL | (mem.i >> 32UL);
#endif
    _pure_byte_store64(&buf[1], mem.i);
    return write(buffer, buf, 9);
}

int pack_nil(PureCore::IBuffer& buffer) {
    const uint8_t d = static_cast<uint8_t>(_msgpack_head_nil);
    return write(buffer, &d, 1);
}

int pack_skip(PureCore::IBuffer& buffer) {
    const uint8_t d = static_cast<uint8_t>(_msgpack_head_nil);
    return write(buffer, &d, 1);
}

int pack_bool(PureCore::IBuffer& buffer, bool d) {
    if (d) {
        const uint8_t t = static_cast<uint8_t>(_msgpack_head_true);
        return write(buffer, &t, 1);
    } else {
        const uint8_t t = static_cast<uint8_t>(_msgpack_head_false);
        return write(buffer, &t, 1);
    }
}

int pack_array(PureCore::IBuffer& buffer, uint32_t n) {
    if (n < 16) {
        uint8_t d = static_cast<uint8_t>(_msgpack_head_fixarray_from | n);
        return write(buffer, &d, 1);
    } else if (n < 65536) {
        uint8_t buf[3];
        buf[0] = static_cast<uint8_t>(_msgpack_head_array16);
        _pure_byte_store16(&buf[1], static_cast<uint16_t>(n));
        return write(buffer, buf, 3);
    } else {
        uint8_t buf[5];
        buf[0] = static_cast<uint8_t>(_msgpack_head_array32);
        _pure_byte_store32(&buf[1], static_cast<uint32_t>(n));
        return write(buffer, buf, 5);
    }
}

int pack_map(PureCore::IBuffer& buffer, uint32_t n) {
    if (n < 16) {
        uint8_t d = static_cast<uint8_t>(_msgpack_head_fixmap_from | n);
        uint8_t buf = take8_8(d);
        return write(buffer, &buf, 1);
    } else if (n < 65536) {
        uint8_t buf[3];
        buf[0] = static_cast<uint8_t>(_msgpack_head_map16);
        _pure_byte_store16(&buf[1], static_cast<uint16_t>(n));
        return write(buffer, buf, 3);
    } else {
        uint8_t buf[5];
        buf[0] = static_cast<uint8_t>(_msgpack_head_map32);
        _pure_byte_store32(&buf[1], static_cast<uint32_t>(n));
        return write(buffer, buf, 5);
    }
}

int pack_str(PureCore::IBuffer& buffer, uint32_t l) {
    if (l < 32) {
        uint8_t d = _msgpack_head_fixstr_from | static_cast<uint8_t>(l);
        uint8_t buf = take8_8(d);
        return write(buffer, &buf, 1);
    } else if (l < 256) {
        uint8_t buf[2];
        buf[0] = static_cast<uint8_t>(_msgpack_head_str8);
        buf[1] = static_cast<uint8_t>(l);
        return write(buffer, buf, 2);
    } else if (l < 65536) {
        uint8_t buf[3];
        buf[0] = static_cast<uint8_t>(_msgpack_head_str16);
        _pure_byte_store16(&buf[1], static_cast<uint16_t>(l));
        return write(buffer, buf, 3);
    } else {
        uint8_t buf[5];
        buf[0] = static_cast<uint8_t>(_msgpack_head_str32);
        _pure_byte_store32(&buf[1], static_cast<uint32_t>(l));
        return write(buffer, buf, 5);
    }
}

int pack_string_ref(PureCore::IBuffer& buffer, PureCore::StringRef s) {
    int err = pack_str(buffer, (uint32_t)s.size());
    if (err != Success) {
        return err;
    }
    return write(buffer, s);
}

#if PURE_CPP >= 201703L
int pack_string_view(PureCore::IBuffer& buffer, std::string_view s) {
    int err = pack_str(buffer, static_cast<uint32_t>(s.size()));
    if (err != Success) {
        return err;
    }
    return write(buffer, PureCore::DataRef(s.data(), s.size()));
}
#endif

int pack_string(PureCore::IBuffer& buffer, const std::string& s) {
    int err = pack_str(buffer, static_cast<uint32_t>(s.size()));
    if (err != Success) {
        return err;
    }
    return write(buffer, PureCore::DataRef(s.data(), s.size()));
}

int pack_bin(PureCore::IBuffer& buffer, uint32_t l) {
    if (l < 256) {
        uint8_t buf[2];
        buf[0] = static_cast<uint8_t>(_msgpack_head_bin8);
        buf[1] = static_cast<uint8_t>(l);
        return write(buffer, buf, 2);
    } else if (l < 65536) {
        uint8_t buf[3];
        buf[0] = static_cast<uint8_t>(_msgpack_head_bin16);
        _pure_byte_store16(&buf[1], static_cast<uint16_t>(l));
        return write(buffer, buf, 3);
    } else {
        uint8_t buf[5];
        buf[0] = static_cast<uint8_t>(_msgpack_head_bin32);
        _pure_byte_store32(&buf[1], static_cast<uint32_t>(l));
        return write(buffer, buf, 5);
    }
    return ErrorMsgTypeError;
}

int pack_data_ref(PureCore::IBuffer& buffer, PureCore::DataRef bin) {
    int err = pack_bin(buffer, (uint32_t)bin.size());
    if (err != Success) {
        return err;
    }
    return write(buffer, bin);
}

int pack_data(PureCore::IBuffer& buffer, const PureCore::IBuffer& bin) {
    int err = pack_bin(buffer, (uint32_t)bin.size());
    if (err != Success) {
        return err;
    }
    return write(buffer, bin.data());
}

int pack_obj(PureCore::IBuffer& buffer, uint32_t count) {
    if (count < 256) {
        uint8_t buf[2];
        buf[0] = static_cast<uint8_t>(_msgpack_head_obj8);
        buf[1] = static_cast<uint8_t>(count);
        return write(buffer, buf, sizeof(buf));
    } else if (count < 65536) {
        uint8_t buf[3];
        buf[0] = static_cast<uint8_t>(_msgpack_head_obj16);
        _pure_byte_store16(&buf[1], static_cast<uint16_t>(count));
        return write(buffer, buf, sizeof(buf));
    } else {
        uint8_t buf[5];
        buf[0] = static_cast<uint8_t>(_msgpack_head_obj32);
        _pure_byte_store32(&buf[1], static_cast<uint32_t>(count));
        return write(buffer, buf, sizeof(buf));
    }
    return ErrorMsgTypeError;
}
}  // namespace PureMsg
