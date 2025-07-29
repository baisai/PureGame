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

#include <assert.h>

namespace PureCore {
template <typename T, size_t N>
class FixedVector {
public:
    typedef T ValueType;

    FixedVector() : mVector{}, mSize(0) {}

    FixedVector(const FixedVector& cp) : mVector{}, mSize(0) {
        for (size_t i = 0; i < cp.size(); ++i) {
            push_back(cp[i]);
        }
    }

    inline FixedVector<T, N>& operator=(const FixedVector& right) {
        for (size_t i = 0; i < right.size(); ++i) {
            push_back(right[i]);
        }
        return *this;
    }

    inline T& operator[](size_t index) {
        assert(index < mSize);
        T* obj = (T*)mVector;
        return *(obj + index);
    }
    inline const T& operator[](size_t index) const {
        assert(index < mSize);
        const T* obj = (const T*)mVector;
        return *(obj + index);
    }

    inline T& front() {
        assert(!empty());
        return operator[](0);
    }

    inline const T& front() const {
        assert(!empty());
        return operator[](0);
    }

    inline T& back() {
        assert(!empty());
        return operator[](mSize - 1);
    }

    inline const T& back() const {
        assert(!empty());
        return operator[](mSize - 1);
    }

    inline T* at(size_t index) {
        if (index >= mSize) {
            return nullptr;
        }
        T* obj = (T*)mVector;
        return (obj + index);
    }

    inline const T* at(size_t index) const {
        if (index >= mSize) {
            return nullptr;
        }
        const T* obj = (const T*)mVector;
        return (obj + index);
    }

    inline bool empty() const { return mSize == 0; }
    inline bool full() const { return mSize >= N; }
    inline size_t size() const { return mSize; }

    inline T* begin() { return (T*)mVector; }
    inline T* end() { return begin() + mSize; }

    inline const T* begin() const { return (const T*)mVector; }
    inline const T* end() const { return begin() + mSize; }

    inline bool push_back(const T& t) {
        if (full()) {
            return false;
        }
        T* obj = begin() + size();
        new (obj) T(t);
        ++mSize;
        return true;
    }

    inline bool insert(size_t idx, const T& t) {
        if (full()) {
            return false;
        }
        if (idx >= size()) {
            return push_back(t);
        }

        size_t s = idx * sizeof(T);
        size_t e = size() * sizeof(T);
        memmove(mVector + s + sizeof(T), mVector + s, e - s);
        T* obj = begin() + idx;
        new (obj) T(t);
        ++mSize;
        return true;
    }

    inline bool pop_back() {
        if (empty()) {
            return false;
        }
        --mSize;
        T* obj = begin() + size();
        obj->~T();
        return true;
    }

    inline bool erase(size_t idx) {
        if (idx >= size()) {
            return false;
        }

        T* obj = begin() + idx;
        obj->~T();
        --mSize;

        size_t s = idx * sizeof(T);
        size_t e = size() * sizeof(T);
        memmove(mVector + s, mVector + s + sizeof(T), e - s);
        return true;
    }

    inline void clear() {
        for (auto iter = begin(); iter != end(); ++iter) {
            iter->~T();
        }
        mSize = 0;
    }

private:
    char mVector[sizeof(T) * N];
    size_t mSize;
};
}  // namespace PureCore
