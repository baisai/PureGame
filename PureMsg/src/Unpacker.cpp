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

#include "PureCore/ByteOrder.h"
#include "PureCore/CoreErrorDesc.h"
#include "PureMsg/MsgHead.h"
#include "PureMsg/Unpacker.h"
#include "PureMsg/MsgErrorDesc.h"

#include <limits.h>

namespace PureMsg {
template <typename T>
inline void load(typename std::enable_if<sizeof(T) == 1, T>::type& dst, const char* n) {
    dst = static_cast<T>(*reinterpret_cast<const uint8_t*>(n));
}

template <typename T>
inline void load(typename std::enable_if<sizeof(T) == 2, T>::type& dst, const char* n) {
    _pure_byte_load16(T, n, &dst);
}

template <typename T>
inline void load(typename std::enable_if<sizeof(T) == 4, T>::type& dst, const char* n) {
    _pure_byte_load32(T, n, &dst);
}

template <typename T>
inline void load(typename std::enable_if<sizeof(T) == 8, T>::type& dst, const char* n) {
    _pure_byte_load64(T, n, &dst);
}

template <typename T>
inline int unpack_imp_int(PureCore::IBuffer& buffer, T& v) {
    PureCore::DataRef data;
    int err = buffer.read(data, sizeof(v));
    if (err != PureCore::Success) {
        return ErrorReadBufferFailed;
    }
    load<T>(v, data.data());
    return Success;
}

template <typename T>
int unpack_imp_check_int(PureCore::IBuffer& buffer, T& v, uint8_t header) {
    switch (header) {
        case _msgpack_head_int32: {
            int32_t t = 0;
            int e = unpack_imp_int<int32_t>(buffer, t);
            if (e == Success) {
                v = static_cast<T>(t);
            }
            return e;
        } break;
        case _msgpack_head_uint32: {
            uint32_t t = 0;
            int e = unpack_imp_int<uint32_t>(buffer, t);
            if (e == Success) {
                v = static_cast<T>(t);
            }
            return e;
        } break;
        case _msgpack_head_int16: {
            int16_t t = 0;
            int e = unpack_imp_int<int16_t>(buffer, t);
            if (e == Success) {
                v = static_cast<T>(t);
            }
            return e;
        } break;
        case _msgpack_head_uint16: {
            uint16_t t = 0;
            int e = unpack_imp_int<uint16_t>(buffer, t);
            if (e == Success) {
                v = static_cast<T>(t);
            }
            return e;
        } break;
        case _msgpack_head_int8: {
            int8_t t = 0;
            int e = unpack_imp_int<int8_t>(buffer, t);
            if (e == Success) {
                v = static_cast<T>(t);
            }
            return e;
        } break;
        case _msgpack_head_uint8: {
            uint8_t t = 0;
            int e = unpack_imp_int<uint8_t>(buffer, t);
            if (e == Success) {
                v = static_cast<T>(t);
            }
            return e;
        } break;
        default:
            if (header >= _msgpack_head_negative_fixint_from || header <= _msgpack_head_positive_fixint_to) {
                int8_t t = 0;
                memcpy(&t, &header, sizeof(header));
                v = static_cast<T>(t);
                return Success;
            }
            break;
    }
    return ErrorMsgTypeError;
}

template <typename T>
int unpack_imp_uint8(PureCore::IBuffer& buffer, typename std::enable_if<sizeof(T) == sizeof(uint8_t), T>::type& d, uint8_t header) {
    if (header == _msgpack_head_uint8) {
        return unpack_imp_int(buffer, d);
    } else if (header <= _msgpack_head_positive_fixint_to) {
        memcpy(&d, &header, sizeof(header));
        return Success;
    }
    return ErrorMsgTypeError;
}

template <typename T>
int unpack_imp_uint16(PureCore::IBuffer& buffer, typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type& d, uint8_t header) {
    if (header == _msgpack_head_uint16) {
        return unpack_imp_int(buffer, d);
    }
    return unpack_imp_check_int(buffer, d, header);
}

template <typename T>
int unpack_imp_uint32(PureCore::IBuffer& buffer, typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type& d, uint8_t header) {
    if (header == _msgpack_head_uint32) {
        return unpack_imp_int(buffer, d);
    }
    return unpack_imp_check_int(buffer, d, header);
}

template <typename T>
int unpack_imp_uint64(PureCore::IBuffer& buffer, typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type& d, uint8_t header) {
    if (header == _msgpack_head_uint64) {
        return unpack_imp_int(buffer, d);
    }
    return unpack_imp_check_int(buffer, d, header);
}

template <typename T>
int unpack_imp_int8(PureCore::IBuffer& buffer, typename std::enable_if<sizeof(T) == sizeof(int8_t), T>::type& d, uint8_t header) {
    if (header == _msgpack_head_int8) {
        return unpack_imp_int(buffer, d);
    } else if (header >= _msgpack_head_negative_fixint_from || header <= _msgpack_head_positive_fixint_to) {
        memcpy(&d, &header, sizeof(header));
        return Success;
    }
    return ErrorMsgTypeError;
}

template <typename T>
int unpack_imp_int16(PureCore::IBuffer& buffer, typename std::enable_if<sizeof(T) == sizeof(int16_t), T>::type& d, uint8_t header) {
    if (header == _msgpack_head_int16 || header == _msgpack_head_uint16) {
        return unpack_imp_int(buffer, d);
    }
    return unpack_imp_check_int(buffer, d, header);
}

template <typename T>
int unpack_imp_int32(PureCore::IBuffer& buffer, typename std::enable_if<sizeof(T) == sizeof(int32_t), T>::type& d, uint8_t header) {
    if (header == _msgpack_head_int32 || header == _msgpack_head_uint32) {
        return unpack_imp_int(buffer, d);
    }
    return unpack_imp_check_int(buffer, d, header);
}

template <typename T>
int unpack_imp_int64(PureCore::IBuffer& buffer, typename std::enable_if<sizeof(T) == sizeof(int64_t), T>::type& d, uint8_t header) {
    if (header == _msgpack_head_int64 || header == _msgpack_head_uint64) {
        return unpack_imp_int(buffer, d);
    }
    return unpack_imp_check_int(buffer, d, header);
}

uint8_t get_header(PureCore::IBuffer& buffer) {
    PureCore::DataRef data;
    int err = buffer.read_silent(data, 1, buffer.read_pos());
    if (err != Success) {
        return _msgpack_head_invalid;
    }
    return (*reinterpret_cast<uint8_t*>(data.data()));
}

uint8_t read_header(PureCore::IBuffer& buffer) {
    PureCore::DataRef data;
    int err = buffer.read(data, 1);
    if (err != Success) {
        return _msgpack_head_invalid;
    }
    return (*reinterpret_cast<uint8_t*>(data.data()));
}

int unpack_uint8(PureCore::IBuffer& buffer, uint8_t& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    return unpack_imp_uint8<uint8_t>(buffer, v, h);
}

int unpack_uint16(PureCore::IBuffer& buffer, uint16_t& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    return unpack_imp_uint16<uint16_t>(buffer, v, h);
}

int unpack_uint32(PureCore::IBuffer& buffer, uint32_t& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    return unpack_imp_uint32<uint32_t>(buffer, v, h);
}

int unpack_uint64(PureCore::IBuffer& buffer, uint64_t& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    return unpack_imp_uint64<uint64_t>(buffer, v, h);
}

int unpack_int8(PureCore::IBuffer& buffer, int8_t& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    return unpack_imp_int8<int8_t>(buffer, v, h);
}

int unpack_int16(PureCore::IBuffer& buffer, int16_t& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    return unpack_imp_int16<int16_t>(buffer, v, h);
}

int unpack_int32(PureCore::IBuffer& buffer, int32_t& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    return unpack_imp_int32<int32_t>(buffer, v, h);
}

int unpack_int64(PureCore::IBuffer& buffer, int64_t& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    return unpack_imp_int64<int64_t>(buffer, v, h);
}

int unpack_char(PureCore::IBuffer& buffer, char& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
#if defined(CHAR_MIN)
#if CHAR_MIN < 0
    return unpack_imp_int8<char>(buffer, v, h);
#else
    return unpack_imp_uint8<char>(buffer, v, h);
#endif
#else
#error CHAR_MIN is not defined
#endif
    return ErrorMsgTypeError;
}

int unpack_signed_char(PureCore::IBuffer& buffer, signed char& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    return unpack_imp_int8<signed char>(buffer, v, h);
}

int unpack_short(PureCore::IBuffer& buffer, short& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }

#if defined(SIZEOF_SHORT)
#if SIZEOF_SHORT == 2
    return unpack_imp_int16<short>(buffer, v, h);
#elif SIZEOF_SHORT == 4
    return unpack_imp_int32<short>(buffer, v, h);
#else
    return unpack_imp_int64<short>(buffer, v, h);
#endif

#elif defined(SHRT_MAX)
#if SHRT_MAX == 0x7fff
    return unpack_imp_int16<short>(buffer, v, h);
#elif SHRT_MAX == 0x7fffffff
    return unpack_imp_int32<short>(buffer, v, h);
#else
    return unpack_imp_int64<short>(buffer, v, h);
#endif

#else
    if (sizeof(short) == 2) {
        return unpack_imp_int16<short>(buffer, v, h);
    } else if (sizeof(short) == 4) {
        return unpack_imp_int32<short>(buffer, v, h);
    } else {
        return unpack_imp_int64<short>(buffer, v, h);
    }
#endif

    return ErrorMsgTypeError;
}

int unpack_int(PureCore::IBuffer& buffer, int& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }

#if defined(SIZEOF_INT)
#if SIZEOF_INT == 2
    return unpack_imp_int16<int>(buffer, v, h);
#elif SIZEOF_INT == 4
    return unpack_imp_int32<int>(buffer, v, h);
#else
    return unpack_imp_int64<int>(buffer, v, h);
#endif

#elif defined(INT_MAX)
#if INT_MAX == 0x7fff
    return unpack_imp_int16<int>(buffer, v, h);
#elif INT_MAX == 0x7fffffff
    return unpack_imp_int32<int>(buffer, v, h);
#else
    return unpack_imp_int64<int>(buffer, v, h);
#endif

#else
    if (sizeof(int) == 2) {
        return unpack_imp_int16<int>(buffer, v, h);
    } else if (sizeof(int) == 4) {
        return unpack_imp_int32<int>(buffer, v, h);
    } else {
        return unpack_imp_int64<int>(buffer, v, h);
    }
#endif

    return ErrorMsgTypeError;
}

int unpack_long(PureCore::IBuffer& buffer, long& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }

#if defined(SIZEOF_LONG)
#if SIZEOF_LONG == 2
    return unpack_imp_int16<long>(buffer, v, h);
#elif SIZEOF_LONG == 4
    return unpack_imp_int32<long>(buffer, v, h);
#else
    return unpack_imp_int64<long>(buffer, v, h);
#endif

#elif defined(LONG_MAX)
#if LONG_MAX == 0x7fffL
    return unpack_imp_int16<long>(buffer, v, h);
#elif LONG_MAX == 0x7fffffffL
    return unpack_imp_int32<long>(buffer, v, h);
#else
    return unpack_imp_int64<long>(buffer, v, h);
#endif

#else
    if (sizeof(long) == 2) {
        return unpack_imp_int16<long>(buffer, v, h);
    } else if (sizeof(long) == 4) {
        return unpack_imp_int32<long>(buffer, v, h);
    } else {
        return unpack_imp_int64<long>(buffer, v, h);
    }
#endif

    return ErrorMsgTypeError;
}

int unpack_long_long(PureCore::IBuffer& buffer, long long& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }

#if defined(SIZEOF_LONG_LONG)
#if SIZEOF_LONG_LONG == 2
    return unpack_imp_int16<long long>(buffer, v, h);
#elif SIZEOF_LONG_LONG == 4
    return unpack_imp_int32<long long>(buffer, v, h);
#else
    return unpack_imp_int64<long long>(buffer, v, h);
#endif

#elif defined(LLONG_MAX)
#if LLONG_MAX == 0x7fffL
    return unpack_imp_int16<long long>(buffer, v, h);
#elif LLONG_MAX == 0x7fffffffL
    return unpack_imp_int32<long long>(buffer, v, h);
#else
    return unpack_imp_int64<long long>(buffer, v, h);
#endif

#else
    if (sizeof(long long) == 2) {
        return unpack_imp_int16<long long>(buffer, v, h);
    } else if (sizeof(long long) == 4) {
        return unpack_imp_int32<long long>(buffer, v, h);
    } else {
        return unpack_imp_int64<long long>(buffer, v, h);
    }
#endif

    return ErrorMsgTypeError;
}

int unpack_unsigned_char(PureCore::IBuffer& buffer, unsigned char& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }

    return unpack_imp_uint8<unsigned char>(buffer, v, h);
}

int unpack_unsigned_short(PureCore::IBuffer& buffer, unsigned short& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }

#if defined(SIZEOF_SHORT)
#if SIZEOF_SHORT == 2
    return unpack_imp_uint16<unsigned short>(buffer, v, h);
#elif SIZEOF_SHORT == 4
    return unpack_imp_uint32<unsigned short>(buffer, v, h);
#else
    return unpack_imp_uint64<unsigned short>(buffer, v, h);
#endif

#elif defined(USHRT_MAX)
#if USHRT_MAX == 0xffffU
    return unpack_imp_uint16<unsigned short>(buffer, v, h);
#elif USHRT_MAX == 0xffffffffU
    return unpack_imp_uint32<unsigned short>(buffer, v, h);
#else
    return unpack_imp_uint64<unsigned short>(buffer, v, h);
#endif

#else
    if (sizeof(unsigned short) == 2) {
        return unpack_imp_uint16<unsigned short>(buffer, v, h);
    } else if (sizeof(unsigned short) == 4) {
        return unpack_imp_uint32<unsigned short>(buffer, v, h);
    } else {
        return unpack_imp_uint64<unsigned short>(buffer, v, h);
    }
#endif

    return ErrorMsgTypeError;
}

int unpack_unsigned_int(PureCore::IBuffer& buffer, unsigned int& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }

#if defined(SIZEOF_INT)
#if SIZEOF_INT == 2
    return unpack_imp_uint16<unsigned int>(buffer, v, h);
#elif SIZEOF_INT == 4
    return unpack_imp_uint32<unsigned int>(buffer, v, h);
#else
    return unpack_imp_uint64<unsigned int>(buffer, v, h);
#endif

#elif defined(UINT_MAX)
#if UINT_MAX == 0xffffU
    return unpack_imp_uint16<unsigned int>(buffer, v, h);
#elif UINT_MAX == 0xffffffffU
    return unpack_imp_uint32<unsigned int>(buffer, v, h);
#else
    return unpack_imp_uint64<unsigned int>(buffer, v, h);
#endif

#else
    if (sizeof(unsigned int) == 2) {
        return unpack_imp_uint16<unsigned int>(buffer, v, h);
    } else if (sizeof(unsigned int) == 4) {
        return unpack_imp_uint32<unsigned int>(buffer, v, h);
    } else {
        return unpack_imp_uint64<unsigned int>(buffer, v, h);
    }
#endif

    return ErrorMsgTypeError;
}

int unpack_unsigned_long(PureCore::IBuffer& buffer, unsigned long& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }

#if defined(SIZEOF_LONG)
#if SIZEOF_LONG == 2
    return unpack_imp_uint16<unsigned long>(buffer, v, h);
#elif SIZEOF_LONG == 4
    return unpack_imp_uint32<unsigned long>(buffer, v, h);
#else
    return unpack_imp_uint64<unsigned long>(buffer, v, h);
#endif

#elif defined(ULONG_MAX)
#if ULONG_MAX == 0xffffUL
    return unpack_imp_uint16<unsigned long>(buffer, v, h);
#elif ULONG_MAX == 0xffffffffUL
    return unpack_imp_uint32<unsigned long>(buffer, v, h);
#else
    return unpack_imp_uint64<unsigned long>(buffer, v, h);
#endif

#else
    if (sizeof(unsigned long) == 2) {
        return unpack_imp_uint16<unsigned long>(buffer, v, h);
    } else if (sizeof(unsigned long) == 4) {
        return unpack_imp_uint32<unsigned long>(buffer, v, h);
    } else {
        return unpack_imp_uint64<unsigned long>(buffer, v, h);
    }
#endif

    return ErrorMsgTypeError;
}

int unpack_unsigned_long_long(PureCore::IBuffer& buffer, unsigned long long& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }

#if defined(SIZEOF_LONG_LONG)
#if SIZEOF_LONG_LONG == 2
    return unpack_imp_uint16<unsigned long long>(buffer, v, h);
#elif SIZEOF_LONG_LONG == 4
    return unpack_imp_uint32<unsigned long long>(buffer, v, h);
#else
    return unpack_imp_uint64<unsigned long long>(buffer, v, h);
#endif

#elif defined(ULLONG_MAX)
#if ULLONG_MAX == 0xffffUL
    return unpack_imp_uint16<unsigned long long>(buffer, v, h);
#elif ULLONG_MAX == 0xffffffffUL
    return unpack_imp_uint32<unsigned long long>(buffer, v, h);
#else
    return unpack_imp_uint64<unsigned long long>(buffer, v, h);
#endif

#else
    if (sizeof(unsigned long long) == 2) {
        return unpack_imp_uint16<unsigned long long>(buffer, v, h);
    } else if (sizeof(unsigned long long) == 4) {
        return unpack_imp_uint32<unsigned long long>(buffer, v, h);
    } else {
        return unpack_imp_uint64<unsigned long long>(buffer, v, h);
    }
#endif

    return ErrorMsgTypeError;
}

int unpack_float(PureCore::IBuffer& buffer, float& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    if (h == _msgpack_head_float32) {
        PureCore::DataRef data;
        int err = buffer.read(data, sizeof(v));
        if (err != PureCore::Success) {
            return ErrorReadBufferFailed;
        }
        union {
            uint32_t i;
            float f;
        } mem;
        load<uint32_t>(mem.i, data.data());
        v = mem.f;
        return Success;
    }
    return ErrorMsgTypeError;
}

int unpack_double(PureCore::IBuffer& buffer, double& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    if (h == _msgpack_head_float64) {
        PureCore::DataRef data;
        int err = buffer.read(data, sizeof(v));
        if (err != PureCore::Success) {
            return ErrorReadBufferFailed;
        }
        union {
            uint64_t i;
            double f;
        } mem;
        load<uint64_t>(mem.i, data.data());

#if defined(__arm__) && !(__ARM_EABI__)  // arm-oabi
        // https://github.com/msgpack/msgpack-perl/pull/1
        mem.i = (mem.i & 0xFFFFFFFFUL) << 32UL | (mem.i >> 32UL);
#endif
        v = mem.f;
        return Success;
    }
    return ErrorMsgTypeError;
}

int unpack_nil(PureCore::IBuffer& buffer) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    if (h == _msgpack_head_nil) {
        return Success;
    }
    return ErrorMsgTypeError;
}

int unpack_skip(PureCore::IBuffer& buffer) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    if (h == _msgpack_head_nil || h == _msgpack_head_true || h == _msgpack_head_false || h <= _msgpack_head_positive_fixint_to ||
        h >= _msgpack_head_negative_fixint_from) {
        return Success;
    } else if (h == _msgpack_head_uint8 || h == _msgpack_head_int8) {
        buffer.read_pos(buffer.read_pos() + sizeof(uint8_t));
        return Success;
    } else if (h == _msgpack_head_uint16 || h == _msgpack_head_int16) {
        buffer.read_pos(buffer.read_pos() + sizeof(uint16_t));
        return Success;
    } else if (h == _msgpack_head_uint32 || h == _msgpack_head_int32) {
        buffer.read_pos(buffer.read_pos() + sizeof(uint32_t));
        return Success;
    } else if (h == _msgpack_head_uint64 || h == _msgpack_head_int64) {
        buffer.read_pos(buffer.read_pos() + sizeof(uint64_t));
        return Success;
    } else if (h == _msgpack_head_float32) {
        buffer.read_pos(buffer.read_pos() + sizeof(float));
        return Success;
    } else if (h == _msgpack_head_float64) {
        buffer.read_pos(buffer.read_pos() + sizeof(double));
        return Success;
    } else if (h >= _msgpack_head_fixstr_from && h <= _msgpack_head_fixstr_to) {
        uint32_t len = h & 0x1fu;
        buffer.read_pos(buffer.read_pos() + len);
        return Success;
    } else if (h == _msgpack_head_str8 || h == _msgpack_head_bin8) {
        uint8_t len = 0;
        int e = unpack_imp_int(buffer, len);
        if (e != Success) {
            return e;
        }
        buffer.read_pos(buffer.read_pos() + len);
        return Success;
    } else if (h == _msgpack_head_str16 || h == _msgpack_head_bin16) {
        uint16_t len = 0;
        int e = unpack_imp_int(buffer, len);
        if (e != Success) {
            return e;
        }
        buffer.read_pos(buffer.read_pos() + len);
        return Success;
    } else if (h == _msgpack_head_str32 || h == _msgpack_head_bin32) {
        uint32_t len = 0;
        int e = unpack_imp_int(buffer, len);
        if (e != Success) {
            return e;
        }
        buffer.read_pos(buffer.read_pos() + len);
        return Success;
    }

    uint32_t skipSub = 0;
    if (h >= _msgpack_head_fixarray_from && h <= _msgpack_head_fixarray_to) {
        skipSub = h & 0xfu;
    } else if (h == _msgpack_head_array16) {
        uint16_t len = 0;
        int err = unpack_imp_int(buffer, len);
        if (err != Success) {
            return err;
        }
        skipSub = len;
    } else if (h == _msgpack_head_array32) {
        int err = unpack_imp_int(buffer, skipSub);
        if (err != Success) {
            return err;
        }
    } else if (h >= _msgpack_head_fixmap_from && h <= _msgpack_head_fixmap_to) {
        skipSub = h & 0xfu;
        skipSub *= 2;
    } else if (h == _msgpack_head_map16) {
        uint16_t len = 0;
        int err = unpack_imp_int(buffer, len);
        if (err != Success) {
            return err;
        }
        skipSub = len * 2;
    } else if (h == _msgpack_head_map32) {
        int err = unpack_imp_int(buffer, skipSub);
        if (err != Success) {
            return err;
        }
        skipSub *= 2;
    } else if (h == _msgpack_head_obj8) {
        uint8_t len = 0;
        int err = unpack_imp_int(buffer, len);
        if (err != Success) {
            return err;
        }
        skipSub = len;
    } else if (h == _msgpack_head_obj16) {
        uint16_t len = 0;
        int err = unpack_imp_int(buffer, len);
        if (err != Success) {
            return err;
        }
        skipSub = len;
    } else if (h == _msgpack_head_obj32) {
        int err = unpack_imp_int(buffer, skipSub);
        if (err != Success) {
            return err;
        }
    } else {
        return ErrorMsgTypeError;
    }
    for (uint32_t i = 0; i < skipSub; ++i) {
        int err = unpack_skip(buffer);
        if (err != Success) {
            return err;
        }
    }
    return Success;
}

int unpack_bool(PureCore::IBuffer& buffer, bool& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    if (h == _msgpack_head_true) {
        v = true;
        return Success;
    } else if (h == _msgpack_head_false) {
        v = false;
        return Success;
    }
    return ErrorMsgTypeError;
}

int unpack_array(PureCore::IBuffer& buffer, uint32_t& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    if (h >= _msgpack_head_fixarray_from && h <= _msgpack_head_fixarray_to)  // fixarray
    {
        v = h & 0xfu;
        return Success;
    } else if (h == _msgpack_head_array16) {
        uint16_t len = 0;
        int err = unpack_imp_int(buffer, len);
        if (err != Success) {
            return err;
        }
        v = len;
        return Success;
    } else if (h == _msgpack_head_array32) {
        return unpack_imp_int(buffer, v);
    }
    return ErrorMsgTypeError;
}

int unpack_map(PureCore::IBuffer& buffer, uint32_t& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    if (h >= _msgpack_head_fixmap_from && h <= _msgpack_head_fixmap_to)  // fixmap
    {
        v = h & 0xfu;
        return Success;
    } else if (h == _msgpack_head_map16) {
        uint16_t len = 0;
        int err = unpack_imp_int(buffer, len);
        if (err != Success) {
            return err;
        }
        v = len;
        return Success;
    } else if (h == _msgpack_head_map32) {
        return unpack_imp_int(buffer, v);
    }
    return ErrorMsgTypeError;
}

int unpack_str(PureCore::IBuffer& buffer, uint32_t& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    if (h >= _msgpack_head_fixstr_from && h <= _msgpack_head_fixstr_to)  // fixstr
    {
        v = h & 0x1fu;
        return Success;
    }
    switch (h) {
        case _msgpack_head_str8: {
            uint8_t len = 0;
            int e = unpack_imp_int(buffer, len);
            if (e != Success) {
                return e;
            }
            v = len;
            return Success;
        }
        case _msgpack_head_str16: {
            uint16_t len = 0;
            int e = unpack_imp_int(buffer, len);
            if (e != Success) {
                return e;
            }
            v = len;
            return Success;
        }
        case _msgpack_head_str32: {
            return unpack_imp_int(buffer, v);
        }
        default:
            break;
    }
    return ErrorMsgTypeError;
}

int unpack_string(PureCore::IBuffer& buffer, std::string& v) {
    uint32_t len = 0;
    int err = unpack_str(buffer, len);
    if (err != Success) {
        return err;
    }
    PureCore::DataRef data;
    err = buffer.read(data, len);
    if (err != PureCore::Success) {
        return ErrorReadBufferFailed;
    }
    v.append(data.data(), data.size());
    return Success;
}

#if PURE_CPP >= 201703L
int unpack_string_view(PureCore::IBuffer& buffer, std::string_view& s) {
    PureCore::StringRef v;
    int err = unpack_string_ref(buffer, v);
    if (err != Success) {
        return err;
    }
    s = std::string_view(v.data(), v.size());
    return Success;
}
#endif

int unpack_string_ref(PureCore::IBuffer& buffer, PureCore::StringRef& v) {
    uint32_t len = 0;
    int err = unpack_str(buffer, len);
    if (err != Success) {
        return err;
    }
    PureCore::DataRef data;
    err = buffer.read(data, len);
    if (err != PureCore::Success) {
        return ErrorReadBufferFailed;
    }
    v = data.bytes();
    return Success;
}

int unpack_bin(PureCore::IBuffer& buffer, uint32_t& v) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    switch (h) {
        case _msgpack_head_bin8: {
            uint8_t len = 0;
            int e = unpack_imp_int(buffer, len);
            if (e != Success) {
                return e;
            }
            v = len;
            return Success;
        }
        case _msgpack_head_bin16: {
            uint16_t len = 0;
            int e = unpack_imp_int(buffer, len);
            if (e != Success) {
                return e;
            }
            v = len;
            return Success;
        }
        case _msgpack_head_bin32: {
            return unpack_imp_int(buffer, v);
        }
        default:
            break;
    }
    return ErrorMsgTypeError;
}

int unpack_data(PureCore::IBuffer& buffer, PureCore::IBuffer& v) {
    uint32_t len = 0;
    int err = unpack_bin(buffer, len);
    if (err != Success) {
        return err;
    }
    PureCore::DataRef data;
    err = buffer.read(data, len);
    if (err != PureCore::Success) {
        return ErrorReadBufferFailed;
    }
    err = v.write(data);
    if (err != PureCore::Success) {
        return ErrorWriteBufferFailed;
    }
    return Success;
}

int unpack_data_ref(PureCore::IBuffer& buffer, PureCore::DataRef& v) {
    uint32_t len = 0;
    int err = unpack_bin(buffer, len);
    if (err != Success) {
        return err;
    }
    err = buffer.read(v, len);
    if (err != PureCore::Success) {
        return ErrorReadBufferFailed;
    }
    return Success;
}

int unpack_obj(PureCore::IBuffer& buffer, uint32_t& count) {
    uint8_t h = read_header(buffer);
    if (h == _msgpack_head_invalid) {
        return ErrorMsgTypeError;
    }
    int err = Success;
    switch (h) {
        case _msgpack_head_obj8: {
            uint8_t len = 0;
            err = unpack_imp_int(buffer, len);
            if (err != Success) {
                return err;
            }
            count = len;
        } break;
        case _msgpack_head_obj16: {
            uint16_t len = 0;
            err = unpack_imp_int(buffer, len);
            if (err != Success) {
                return err;
            }
            count = len;
        } break;
        case _msgpack_head_obj32: {
            err = unpack_imp_int(buffer, count);
            if (err != Success) {
                return err;
            }
        } break;
        default:
            err = ErrorMsgTypeError;
            break;
    }
    return err;
}
}  // namespace PureMsg
