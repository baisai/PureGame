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

#include "PureCore/PureCoreLib.h"
#include "spdlog/fmt/fmt.h"

#include <assert.h>
#include <string.h>
#include <array>
#include <vector>
#include <string>

#if PURE_CPP >= 201703L
#include <string_view>
#endif

namespace PureCore {
template <typename T>
class TStringRef {
public:
    TStringRef() : mPtr(nullptr), mSize(0) { ; }
    TStringRef(const T* charPtr) : mPtr(const_cast<T*>(charPtr)), mSize(0) {
        if (charPtr) {
            mSize = std::char_traits<T>::length(charPtr);
        }
    }
    TStringRef(const T* charPtr, size_t len) : mPtr(const_cast<T*>(charPtr)), mSize(len) {}
    TStringRef(const std::basic_string<T>& str) : mPtr(const_cast<T*>(str.c_str())), mSize(str.size()) {}
#if PURE_CPP >= 201703L
    TStringRef(const std::basic_string_view<T>& str) : mPtr(const_cast<T*>(str.data())), mSize(str.size()) {}
#endif
    template <size_t len>
    TStringRef(T (&arr)[len]) : mPtr(arr), mSize(len) {}
    template <size_t len>
    TStringRef(std::array<T, len>& src) : mPtr(src.data()), mSize(src.size()) {}
    TStringRef(std::vector<T>& src) : mPtr(src.data()), mSize(src.size()) {}
    template <size_t len>
    TStringRef(const T (&arr)[len]) : mPtr(const_cast<T*>(arr)), mSize(len) {}
    template <size_t len>
    TStringRef(const std::array<T, len>& src) : mPtr(const_cast<T*>(src.data())), mSize(src.size()) {}
    TStringRef(const std::vector<T>& src) : mPtr(const_cast<T*>(src.data())), mSize(src.size()) {}

    TStringRef(const TStringRef& cp) : mPtr(cp.mPtr), mSize(cp.mSize) {}

#if PURE_CPP >= 201703L
    inline operator const std::basic_string_view<T>() const { return std::basic_string_view<T>(mPtr, mSize); }
    inline operator std::basic_string_view<T>() { return std::basic_string_view<T>(mPtr, mSize); }
#endif
    inline operator const std::basic_string<T>() const { return std::basic_string<T>(mPtr, mSize); }
    inline operator std::basic_string<T>() { return std::basic_string<T>(mPtr, mSize); }

    inline TStringRef& operator=(const TStringRef& right) {
        mPtr = right.mPtr;
        mSize = right.mSize;
        return *this;
    }

    inline void reset(const T* charPtr, size_t len) {
        mPtr = const_cast<T*>(charPtr);
        mSize = len;
    }

    inline bool operator==(const PureCore::TStringRef<T>& right) const { return size() == right.size() && memcmp(data(), right.data(), size()) == 0; }

    inline bool operator!=(const PureCore::TStringRef<T>& right) const { return !(*this == right); }

    inline T& at(size_t index) {
        assert(index < mSize);
        return *(mPtr + index);
    }
    inline const T& at(size_t index) const {
        assert(index < mSize);
        return *(mPtr + index);
    }

    inline T& operator[](size_t index) { return at(index); }
    inline const T& operator[](size_t index) const { return at(index); }

    inline T* data() const { return mPtr; }

    inline bool empty() const { return mPtr == nullptr || mSize == 0; }
    inline size_t size() const { return mSize; }

    inline T* begin() const { return mPtr; }
    inline T* end() const { return mPtr + mSize; }
    inline T* rbegin() const { return mPtr + mSize - 1; }
    inline T* rend() const { return mPtr - 1; }

private:
    T* mPtr;
    size_t mSize;
};

using StringRef = TStringRef<char>;
using String16Ref = TStringRef<char16_t>;
using String32Ref = TStringRef<char32_t>;
using WStringRef = TStringRef<wchar_t>;

}  // namespace PureCore

template <>
struct fmt::formatter<PureCore::StringRef> : fmt::formatter<fmt::string_view> {
    inline fmt::format_context::iterator format(PureCore::StringRef str, fmt::format_context& ctx) const {
        return fmt::formatter<fmt::string_view>::format(fmt::string_view(str.data(), str.size()), ctx);
    }
};

#if PURE_CPP >= 201703L
template <typename T>
inline bool operator==(const std::basic_string_view<T>& left, const PureCore::TStringRef<T>& right) {
    return left.compare(0, left.size(), right.data(), right.size()) == 0;
}
template <typename T>
inline bool operator==(const PureCore::TStringRef<T>& left, const std::basic_string_view<T>& right) {
    return right.compare(0, right.size(), left.data(), left.size()) == 0;
}
#else
template <typename T>
inline bool operator==(const std::basic_string<T>& left, const PureCore::TStringRef<T>& right) {
    return left.compare(0, left.size(), right.data(), right.size()) == 0;
}
template <typename T>
inline bool operator==(const PureCore::TStringRef<T>& left, const std::basic_string<T>& right) {
    return right.compare(0, right.size(), left.data(), left.size()) == 0;
}
#endif

#if PURE_CPP >= 201703L
template <typename T>
inline bool operator!=(const std::basic_string_view<T>& left, const PureCore::TStringRef<T>& right) {
    return !(left == right);
}
template <typename T>
inline bool operator!=(const PureCore::TStringRef<T>& left, const std::basic_string_view<T>& right) {
    return !(left == right);
}
#else
template <typename T>
inline bool operator!=(const std::basic_string<T>& left, const PureCore::TStringRef<T>& right) {
    return !(left == right);
}
template <typename T>
inline bool operator!=(const PureCore::TStringRef<T>& left, const std::basic_string<T>& right) {
    return !(left == right);
}
#endif
