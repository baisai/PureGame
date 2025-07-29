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

#include "PureCore/StringRef.h"

#include <assert.h>
#include <stddef.h>
#include <array>
#include <vector>

namespace PureCore {
class DataRef {
public:
    DataRef() : mPtr(nullptr), mSize(0) {}
    DataRef(void *ptr, size_t len) : mPtr(ptr), mSize(len) {}
    DataRef(const void *ptr, size_t len) : mPtr(const_cast<void *>(ptr)), mSize(len) {}
    DataRef(StringRef v) : mPtr(v.data()), mSize(v.size()) {}
    DataRef(const char *str) : mPtr(const_cast<char *>(str)), mSize(0) {
        if (str) {
            mSize = std::char_traits<char>::length(str);
        }
    }
    DataRef(const char *str, size_t len) : mPtr(const_cast<char *>(str)), mSize(len) {}
    DataRef(const std::string &str) : mPtr(const_cast<char *>(str.data())), mSize(str.size()) {}
#if PURE_CPP >= 201703L
    DataRef(std::string_view str) : mPtr(const_cast<char *>(str.data())), mSize(str.size()) {}
#endif
    DataRef(const DataRef &cp) : mPtr(cp.mPtr), mSize(cp.mSize) {}

    inline DataRef &operator=(const DataRef &right) {
        mPtr = right.mPtr;
        mSize = right.mSize;
        return *this;
    }

    inline void reset(const void *ptr, size_t len) {
        mPtr = const_cast<void *>(ptr);
        mSize = len;
    }

    inline bool operator==(const PureCore::DataRef &right) const { return size() == right.size() && memcmp(data(), right.data(), size()) == 0; }

    inline bool operator!=(const PureCore::DataRef &right) const { return !(*this == right); }

    inline char &at(size_t index) {
        assert(index < mSize);
        return data()[index];
    }
    inline const char &at(size_t index) const {
        assert(index < mSize);
        return data()[index];
    }

    inline char &operator[](size_t index) { return at(index); }

    inline const char &operator[](size_t index) const { return at(index); }

    inline char *data() const { return (char *)mPtr; }
    inline StringRef bytes() const { return StringRef(data(), mSize); }

    inline bool empty() const { return mPtr == nullptr || mSize == 0; }
    inline size_t size() const { return mSize; }

    inline char *begin() const { return data(); }
    inline char *end() const { return data() + mSize; }

private:
    void *mPtr;
    size_t mSize;
};
}  // namespace PureCore
