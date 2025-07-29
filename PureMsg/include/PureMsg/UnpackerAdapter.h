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

#include "PureCore/DataRef.h"
#include "PureCore/ArrayRef.h"
#include "PureCore/Buffer/IBuffer.h"
#include "PureMsg/MsgSpecialType.h"
#include "PureMsg/MsgErrorDesc.h"
#include "PureMsg/Unpacker.h"

#include <string>
#include <array>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

namespace PureMsg {
inline int unpack(PureCore::IBuffer& buffer, char& v) { return unpack_char(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, signed char& v) { return unpack_signed_char(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, short& v) { return unpack_short(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, int& v) { return unpack_int(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, long& v) { return unpack_long(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, long long& v) { return unpack_long_long(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, unsigned char& v) { return unpack_unsigned_char(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, unsigned short& v) { return unpack_unsigned_short(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, unsigned int& v) { return unpack_unsigned_int(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, unsigned long& v) { return unpack_unsigned_long(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, unsigned long long& v) { return unpack_unsigned_long_long(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, float& v) { return unpack_float(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, double& v) { return unpack_double(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, MsgTypeNil&) { return unpack_nil(buffer); }

inline int unpack(PureCore::IBuffer& buffer, const MsgTypeEmptyObj&) {
    uint32_t c = 0;
    return unpack_obj(buffer, c);
}

inline int unpack(PureCore::IBuffer& buffer, bool& v) { return unpack_bool(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, PureCore::StringRef& str) { return unpack_string_ref(buffer, str); }

#if PURE_CPP >= 201703L
inline int unpack(PureCore::IBuffer& buffer, std::string_view& sv) { return unpack_string_view(buffer, sv); }
#endif

inline int unpack(PureCore::IBuffer& buffer, std::string& v) { return unpack_string(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, PureCore::DataRef& v) { return unpack_data_ref(buffer, v); }

template <size_t len>
inline int unpack(PureCore::IBuffer& buffer, PureCore::ArrayBuffer<len>& v) {
    return unpack_data(buffer, v);
}

inline int unpack(PureCore::IBuffer& buffer, PureCore::DynamicBuffer& v) { return unpack_data(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, PureCore::FixedBuffer& v) { return unpack_data(buffer, v); }

inline int unpack(PureCore::IBuffer& buffer, PureCore::ReferBuffer& v) { return unpack_data(buffer, v); }

template <typename T, uint32_t len>
int unpack(PureCore::IBuffer& buffer, T (&v)[len]);
template <typename T, size_t len>
int unpack(PureCore::IBuffer& buffer, std::array<T, len>& arr);
template <typename T>
int unpack(PureCore::IBuffer& buffer, T* v, uint32_t& len);
template <typename T>
int unpack(PureCore::IBuffer& buffer, std::vector<T>& v);
template <typename T, typename Lesser = std::less<T>>
int unpack(PureCore::IBuffer& buffer, std::set<T, Lesser>& v);
template <typename T, typename Hasher = std::hash<T>, typename EqualTo = std::equal_to<T>>
int unpack(PureCore::IBuffer& buffer, std::unordered_set<T, Hasher, EqualTo>& v);
template <typename T1, typename T2>
int unpack(PureCore::IBuffer& buffer, std::pair<T1, T2>& v);
template <typename K, typename T, typename Lesser = std::less<K>>
int unpack(PureCore::IBuffer& buffer, std::map<K, T, Lesser>& v);
template <typename K, typename T, typename Hasher = std::hash<K>, typename EqualTo = std::equal_to<K>>
int unpack(PureCore::IBuffer& buffer, std::unordered_map<K, T, Hasher, EqualTo>& v);
template <typename T>
int unpack(PureCore::IBuffer& buffer, T& v);

template <typename T, uint32_t len>
inline int unpack(PureCore::IBuffer& buffer, T (&v)[len]) {
    uint32_t size = 0;
    unpack_array(buffer, size);
    if (size != len) {
        return ErrorArraySizeError;
    }
    int e = Success;
    for (uint32_t i = 0; i < len; ++i) {
        e = unpack(buffer, *(v + i));
        if (e != Success) {
            return e;
        }
    }
    return e;
}

template <typename T, size_t len>
inline int unpack(PureCore::IBuffer& buffer, std::array<T, len>& arr) {
    uint32_t size = 0;
    int e = unpack_array(buffer, size);
    if (e != Success) {
        return e;
    }
    if (arr.size() != size) {
        return ErrorArraySizeError;
    }
    for (uint32_t i = 0; i < size; ++i) {
        e = unpack(buffer, arr[i]);
        if (e != Success) {
            return e;
        }
    }
    return e;
}

template <typename T>
inline int unpack(PureCore::IBuffer& buffer, T* v, uint32_t& len) {
    if (v) {
        int e = Success;
        for (uint32_t i = 0; i < len; ++i) {
            e = unpack(buffer, *(v + i));
            if (e != Success) {
                return e;
            }
        }
        return e;
    }
    return unpack_array(buffer, len);
}

template <typename T>
inline int unpack(PureCore::IBuffer& buffer, std::vector<T>& v) {
    uint32_t len = 0;
    unpack_array(buffer, len);
    int e = Success;
    for (uint32_t i = 0; i < len; ++i) {
        v.push_back(T());
        e = unpack(buffer, v[i]);
        if (e != Success) {
            return e;
        }
    }
    return e;
}

template <typename T, typename Lesser>
inline int unpack(PureCore::IBuffer& buffer, std::set<T, Lesser>& v) {
    uint32_t len = 0;
    unpack_array(buffer, len);
    int e = Success;
    for (uint32_t i = 0; i < len; ++i) {
        T t{};
        e = unpack(buffer, t);
        if (e != Success) {
            return e;
        }
        v.insert(t);
    }
    return e;
}

template <typename T, typename Hasher, typename EqualTo>
inline int unpack(PureCore::IBuffer& buffer, std::unordered_set<T, Hasher, EqualTo>& v) {
    uint32_t len = 0;
    unpack_array(buffer, len);
    int e = Success;
    for (uint32_t i = 0; i < len; ++i) {
        T t{};
        e = unpack(buffer, t);
        if (e != Success) {
            return e;
        }
        v.insert(t);
    }
    return e;
}

template <typename T1, typename T2>
inline int unpack(PureCore::IBuffer& buffer, std::pair<T1, T2>& v) {
    int e = unpack(buffer, v.first);
    if (e != Success) {
        return e;
    }
    e = unpack(buffer, v.second);
    if (e != Success) {
        return e;
    }
    return e;
}

template <typename K, typename T, typename Lesser>
inline int unpack(PureCore::IBuffer& buffer, std::map<K, T, Lesser>& v) {
    uint32_t len = 0;
    unpack_map(buffer, len);
    int e = Success;
    for (uint32_t i = 0; i < len; ++i) {
        std::pair<K, T> elem;
        e = unpack(buffer, elem);
        if (e != Success) {
            return e;
        }
        v.insert(elem);
    }
    return e;
}

template <typename K, typename T, typename Hasher, typename EqualTo>
inline int unpack(PureCore::IBuffer& buffer, std::unordered_map<K, T, Hasher, EqualTo>& v) {
    uint32_t len = 0;
    unpack_map(buffer, len);
    int e = Success;
    for (uint32_t i = 0; i < len; ++i) {
        std::pair<K, T> elem;
        e = unpack(buffer, elem);
        if (e != Success) {
            return e;
        }
        v.insert(elem);
    }
    return e;
}

template <typename T>
inline int unpack(PureCore::IBuffer& buffer, T& v) {
    return v.unpack(buffer);
}

}  // namespace PureMsg
