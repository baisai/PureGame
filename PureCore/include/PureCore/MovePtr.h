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

#include <utility>

namespace PureCore {
template <typename T>
struct MovePtrDeler {
    static void free(T* t) { delete t; }
};

template <typename T, typename Deler = MovePtrDeler<T>>
class MovePtr {
public:
    typedef T ValueType;
    typedef T element_type;

    MovePtr() : mPtr(nullptr) {}
    ~MovePtr() { remove(); }
    MovePtr(const T* p) : mPtr(const_cast<T*>(p)) {}
    MovePtr(const MovePtr& cp) { mPtr = cp.move(); }
    MovePtr(MovePtr&& cp) { mPtr = cp.move(); }
    inline MovePtr& operator=(const MovePtr& cp) {
        reset(cp.move());
        return *this;
    }
    inline MovePtr& operator=(MovePtr&& cp) {
        reset(cp.move());
        return *this;
    }
    inline MovePtr& operator=(const T* t) {
        reset(t);
        return *this;
    }

    inline explicit operator bool() const { return valid(); }

    inline const T* operator->() const { return mPtr; }
    inline T* operator->() { return mPtr; }
    inline const T& operator*() const { return *mPtr; }
    inline T& operator*() { return *mPtr; }

    inline void reset(const T* t) {
        if (mPtr) {
            Deler::free(mPtr);
        }
        mPtr = const_cast<T*>(t);
    }
    inline void remove() { reset(nullptr); }

    inline bool valid() const { return mPtr != nullptr; }
    inline T* move() const {
        T* ptr = mPtr;
        mPtr = nullptr;
        return ptr;
    }
    inline T* release() { return move(); }
    inline T* get() const { return mPtr; }
    inline T& ref() const { return *mPtr; }

private:
    mutable T* mPtr;
};

template <typename T, typename... Args>
inline MovePtr<T, MovePtrDeler<T>> MakeMovePtr(Args&&... args) {
    return MovePtr<T, MovePtrDeler<T>>(new T(std::forward<Args>(args)...));
}

template <typename T>
inline MovePtr<T, MovePtrDeler<T>> CloneMovePtr(const T& t) {
    return MovePtr<T, MovePtrDeler<T>>(new T(t));
}

}  // namespace PureCore
