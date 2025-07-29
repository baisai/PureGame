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
#include <stddef.h>
#include <array>
#include <vector>
#include <iterator>

namespace PureCore {

template <typename T>
class ArrayIter {
public:
    typedef T &reference;
    typedef T *pointer;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;

    ArrayIter() = default;
    explicit ArrayIter(T *elem) : mElem(elem) {}
    ~ArrayIter() = default;

    inline ArrayIter &operator++() {
        ++mElem;
        return *this;
    }
    inline ArrayIter operator++(int) {
        ArrayIter old(mElem);
        ++mElem;
        return old;
    }

    inline ArrayIter &operator--() {
        --mElem;
        return *this;
    }
    inline ArrayIter operator--(int) {
        ArrayIter old(mElem);
        --mElem;
        return old;
    }

    inline T &operator*() const { return *mElem; }
    inline T *operator->() const { return mElem; }

    inline bool operator==(const ArrayIter &right) const { return mElem == right.mElem; }
    inline bool operator!=(const ArrayIter &right) const { return mElem != right.mElem; }
    inline bool operator<(const ArrayIter &right) const { return mElem < right.mElem; }
    inline bool operator<=(const ArrayIter &right) const { return mElem <= right.mElem; }
    inline bool operator>(const ArrayIter &right) const { return mElem > right.mElem; }
    inline bool operator>=(const ArrayIter &right) const { return mElem >= right.mElem; }

    inline ptrdiff_t operator-(const ArrayIter &right) const { return mElem - right.mElem; }
    inline ArrayIter operator+(ptrdiff_t right) const { return ArrayIter(mElem + right); }
    inline ArrayIter &operator+=(ptrdiff_t right) {
        mElem += right;
        return *this;
    }
    inline ArrayIter operator-(ptrdiff_t right) const { return ArrayIter(mElem - right); }
    inline ArrayIter &operator-=(ptrdiff_t right) {
        mElem -= right;
        return *this;
    }

private:
    T *mElem = nullptr;
};

template <typename T>
class ArrayReverseIter {
public:
    typedef T &reference;
    typedef T *pointer;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;

    ArrayReverseIter() = default;
    explicit ArrayReverseIter(T *elem) : mElem(elem) {}
    ~ArrayReverseIter() = default;

    inline ArrayReverseIter &operator++() {
        --mElem;
        return *this;
    }
    inline ArrayReverseIter operator++(int) {
        ArrayReverseIter old(mElem);
        --mElem;
        return old;
    }

    inline ArrayReverseIter &operator--() {
        ++mElem;
        return *this;
    }
    inline ArrayReverseIter operator--(int) {
        ArrayReverseIter old(mElem);
        ++mElem;
        return old;
    }

    inline T &operator*() const { return *mElem; }
    inline T *operator->() const { return mElem; }

    inline bool operator==(const ArrayReverseIter &right) const { return mElem == right.mElem; }
    inline bool operator!=(const ArrayReverseIter &right) const { return mElem != right.mElem; }
    inline bool operator<(const ArrayReverseIter &right) const { return mElem > right.mElem; }
    inline bool operator<=(const ArrayReverseIter &right) const { return mElem >= right.mElem; }
    inline bool operator>(const ArrayReverseIter &right) const { return mElem < right.mElem; }
    inline bool operator>=(const ArrayReverseIter &right) const { return mElem <= right.mElem; }

    inline ptrdiff_t operator-(const ArrayReverseIter &right) const { return right.mElem - mElem; }
    inline ArrayReverseIter operator+(ptrdiff_t right) const { return ArrayReverseIter(mElem - right); }
    inline ArrayReverseIter &operator+=(ptrdiff_t right) const {
        mElem -= right;
        return *this;
    }
    inline ArrayReverseIter operator-(ptrdiff_t right) const { return ArrayReverseIter(mElem + right); }
    inline ArrayReverseIter &operator-=(ptrdiff_t right) const {
        mElem += right;
        return *this;
    }

private:
    T *mElem = nullptr;
};

template <typename T>
class ArrayRef {
public:
    typedef T ValueType;
    typedef T element_type;
    typedef T *iterator;
    typedef const T *const_iterator;

    ArrayRef() : mPtr(nullptr), mSize(0) {}
    ArrayRef(T *ptr, size_t len) : mPtr(ptr), mSize(len) {}
    template <size_t len>
    ArrayRef(T (&arr)[len]) : mPtr(arr), mSize(len) {}
    template <size_t len>
    ArrayRef(std::array<T, len> &src) : mPtr(src.data()), mSize(src.size()) {}
    ArrayRef(std::vector<T> &src) : mPtr(src.data()), mSize(src.size()) {}
    ArrayRef(std::initializer_list<T> src) : mPtr(const_cast<T *>(src.begin())), mSize(src.size()) {}
    template <size_t len>
    ArrayRef(const std::array<T, len> &src) : mPtr(const_cast<T *>(src.data())), mSize(src.size()) {}
    ArrayRef(const std::vector<T> &src) : mPtr(const_cast<T *>(src.data())), mSize(src.size()) {}

    ArrayRef(const ArrayRef &cp) : mPtr(cp.mPtr), mSize(cp.mSize) {}

    inline ArrayRef<T> &operator=(const ArrayRef &right) {
        mPtr = right.mPtr;
        mSize = right.mSize;
        return *this;
    }

    inline void reset(T *ptr, size_t len) {
        mPtr = ptr;
        mSize = len;
    }

    inline T &at(size_t index) {
        assert(index < mSize);
        return mPtr[index];
    }

    inline const T &at(size_t index) const {
        assert(index < mSize);
        return mPtr[index];
    }

    inline T &operator[](size_t index) { return at(index); }

    inline const T &operator[](size_t index) const { return at(index); }

    inline bool operator==(const ArrayRef<T> &right) const {
        if (mSize != right.mSize) {
            return false;
        }
        for (size_t i = 0; i < mSize; ++i) {
            if (at(i) != right.at(i)) {
                return false;
            }
        }
        return true;
    }

    inline T *data() const { return mPtr; }

    inline bool empty() const { return mPtr == nullptr || mSize == 0; }
    inline size_t size() const { return mSize; }

    inline ArrayIter<T> begin() const { return ArrayIter<T>(mPtr); }
    inline ArrayIter<T> end() const { return ArrayIter<T>(mPtr + mSize); }
    inline ArrayReverseIter<T> rbegin() const { return ArrayReverseIter<T>(mPtr + mSize - 1); }
    inline ArrayReverseIter<T> rend() const { return ArrayReverseIter<T>(mPtr - 1); }

    inline ArrayRef<T> sub(size_t offset, size_t count) const {
        if (offset >= size()) {
            return ArrayRef<T>();
        }
        return ArrayRef<T>(data() + offset, std::min(size() - offset, count));
    }

private:
    T *mPtr;
    size_t mSize;
};
}  // namespace PureCore
