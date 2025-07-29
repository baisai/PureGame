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
#include "PureMsg/Packer.h"

#include <string>
#include <array>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

namespace PureMsg {
inline int pack(PureCore::IBuffer& buffer, const char& v) { return pack_char(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const signed char& v) { return pack_signed_char(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const short& v) { return pack_short(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const int& v) { return pack_int(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const long& v) { return pack_long(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const long long& v) { return pack_long_long(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const unsigned char& v) { return pack_unsigned_char(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const unsigned short& v) { return pack_unsigned_short(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const unsigned int& v) { return pack_unsigned_int(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const unsigned long& v) { return pack_unsigned_long(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const unsigned long long& v) { return pack_unsigned_long_long(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const float& v) { return pack_float(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const double& v) { return pack_double(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const MsgTypeNil&) { return pack_nil(buffer); }

inline int pack(PureCore::IBuffer& buffer, const MsgTypeEmptyObj&) { return pack_obj(buffer, 0); }

inline int pack(PureCore::IBuffer& buffer, const bool& v) { return pack_bool(buffer, v); }

inline int pack(PureCore::IBuffer& buffer, const char* v) { return pack_string_ref(buffer, PureCore::StringRef(v)); }

inline int pack(PureCore::IBuffer& buffer, const char* v, uint32_t len) { return pack_string_ref(buffer, PureCore::StringRef(v, len)); }

inline int pack(PureCore::IBuffer& buffer, PureCore::StringRef str) { return pack_string_ref(buffer, str); }

inline int pack(PureCore::IBuffer& buffer, PureCore::DataRef bin) { return pack_data_ref(buffer, bin); }

template <size_t len>
inline int pack(PureCore::IBuffer& buffer, const PureCore::ArrayBuffer<len>& bin) {
    return pack_data(buffer, bin);
}

inline int pack(PureCore::IBuffer& buffer, const PureCore::DynamicBuffer& bin) { return pack_data(buffer, bin); }

inline int pack(PureCore::IBuffer& buffer, const PureCore::FixedBuffer& bin) { return pack_data(buffer, bin); }

inline int pack(PureCore::IBuffer& buffer, const PureCore::ReferBuffer& bin) { return pack_data(buffer, bin); }

template <uint32_t len>
inline int pack(PureCore::IBuffer& buffer, const char (&v)[len]) {
    uint32_t size = strnlen(v, len);
    int e = pack_str(buffer, size);
    if (e != Success) {
        return e;
    }
    return pack_str_body(buffer, v, size);
}

#if PURE_CPP >= 201703L
inline int pack(PureCore::IBuffer& buffer, const std::string_view& v) { return pack_string_view(buffer, v); }
#endif

inline int pack(PureCore::IBuffer& buffer, const std::string& v) { return pack_string(buffer, v); }

template <typename T, uint32_t len>
int pack(PureCore::IBuffer& buffer, const T (&v)[len]);
template <typename T>
int pack(PureCore::IBuffer& buffer, const PureCore::ArrayRef<T> arr);
template <typename T>
int pack(PureCore::IBuffer& buffer, const T* v, uint32_t len);
template <typename T, size_t len>
int pack(PureCore::IBuffer& buffer, const std::array<T, len>& arr);
template <typename T>
int pack(PureCore::IBuffer& buffer, const std::vector<T>& v);
template <typename T, typename Lesser = std::less<T>>
int pack(PureCore::IBuffer& buffer, const std::set<T, Lesser>& v);
template <typename T, typename Hasher = std::hash<T>, typename EqualTo = std::equal_to<T>>
int pack(PureCore::IBuffer& buffer, const std::unordered_set<T, Hasher, EqualTo>& v);
template <typename T1, typename T2>
int pack(PureCore::IBuffer& buffer, const std::pair<T1, T2>& v);
template <typename K, typename T, typename Lesser = std::less<K>>
int pack(PureCore::IBuffer& buffer, const std::map<K, T, Lesser>& v);
template <typename K, typename T, typename Hasher = std::hash<K>, typename EqualTo = std::equal_to<K>>
int pack(PureCore::IBuffer& buffer, const std::unordered_map<K, T, Hasher, EqualTo>& v);
template <typename T>
int pack(PureCore::IBuffer& buffer, const T& v);

template <typename T, uint32_t len>
inline int pack(PureCore::IBuffer& buffer, const T (&v)[len]) {
    int e = pack_array(buffer, len);
    if (e != Success) {
        return e;
    }
    for (uint32_t i = 0; i < len; ++i) {
        e = pack(buffer, v[i]);
        if (e != Success) {
            return e;
        }
    }
    return Success;
}

template <typename T>
inline int pack(PureCore::IBuffer& buffer, const PureCore::ArrayRef<T> arr) {
    int e = pack_array(buffer, uint32_t(arr.size()));
    if (e != Success) {
        return e;
    }
    for (uint32_t i = 0; i < arr.size(); ++i) {
        e = pack(buffer, arr[i]);
        if (e != Success) {
            return e;
        }
    }
    return Success;
}

template <typename T>
inline int pack(PureCore::IBuffer& buffer, const T* v, uint32_t len) {
    int e = pack_array(buffer, len);
    if (e != Success) {
        return e;
    }
    for (uint32_t i = 0; i < len; ++i) {
        e = pack(buffer, *(v + i));
        if (e != Success) {
            return e;
        }
    }
    return Success;
}

template <typename T, size_t len>
inline int pack(PureCore::IBuffer& buffer, const std::array<T, len>& arr) {
    int e = pack_array(buffer, uint32_t(arr.size()));
    if (e != Success) {
        return e;
    }
    for (uint32_t i = 0; i < arr.size(); ++i) {
        e = pack(buffer, arr[i]);
        if (e != Success) {
            return e;
        }
    }
    return Success;
}

template <typename T>
inline int pack(PureCore::IBuffer& buffer, const std::vector<T>& v) {
    int e = pack_array(buffer, uint32_t(v.size()));
    if (e != Success) {
        return e;
    }
    for (size_t i = 0; i < v.size(); ++i) {
        e = pack(buffer, v[i]);
        if (e != Success) {
            return e;
        }
    }
    return Success;
}

template <typename T, typename Lesser>
inline int pack(PureCore::IBuffer& buffer, const std::set<T, Lesser>& v) {
    int e = pack_array(buffer, uint32_t(v.size()));
    if (e != Success) {
        return e;
    }
    for (typename std::set<T, Lesser>::const_iterator iter = v.begin(); iter != v.end(); ++iter) {
        e = pack(buffer, *iter);
        if (e != Success) {
            return e;
        }
    }
    return Success;
}

template <typename T, typename Hasher, typename EqualTo>
inline int pack(PureCore::IBuffer& buffer, const std::unordered_set<T, Hasher, EqualTo>& v) {
    int e = pack_array(buffer, uint32_t(v.size()));
    if (e != Success) {
        return e;
    }
    for (typename std::unordered_set<T, Hasher, EqualTo>::const_iterator iter = v.begin(); iter != v.end(); ++iter) {
        e = pack(buffer, *iter);
        if (e != Success) {
            return e;
        }
    }
    return Success;
}

template <typename T1, typename T2>
inline int pack(PureCore::IBuffer& buffer, const std::pair<T1, T2>& v) {
    int e = pack(buffer, v.first);
    if (e != Success) {
        return e;
    }
    e = pack(buffer, v.second);
    if (e != Success) {
        return e;
    }
    return Success;
}

template <typename K, typename T, typename Lesser>
inline int pack(PureCore::IBuffer& buffer, const std::map<K, T, Lesser>& v) {
    int e = pack_map(buffer, uint32_t(v.size()));
    if (e != Success) {
        return e;
    }
    for (typename std::map<K, T, Lesser>::const_iterator iter = v.begin(); iter != v.end(); ++iter) {
        e = pack(buffer, *iter);
        if (e != Success) {
            return e;
        }
    }
    return Success;
}

template <typename K, typename T, typename Hasher, typename EqualTo>
inline int pack(PureCore::IBuffer& buffer, const std::unordered_map<K, T, Hasher, EqualTo>& v) {
    int e = pack_map(buffer, uint32_t(v.size()));
    if (e != Success) {
        return e;
    }
    for (typename std::unordered_map<K, T, Hasher, EqualTo>::const_iterator iter = v.begin(); iter != v.end(); ++iter) {
        e = pack(buffer, *iter);
        if (e != Success) {
            return e;
        }
    }
    return Success;
}

template <typename T>
inline int pack(PureCore::IBuffer& buffer, const T& v) {
    return v.pack(buffer);
}

}  // namespace PureMsg
