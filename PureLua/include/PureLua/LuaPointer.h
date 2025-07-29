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

namespace PureLua {
template <typename T>
class LuaPointer {
public:
    LuaPointer() : mPtr(new T), mRef(false) {}
    LuaPointer(const T* p) : mPtr(const_cast<T*>(p)), mRef(true) {}
    LuaPointer(const T& t) : mPtr(const_cast<T*>(&t)), mRef(true) {}

    LuaPointer(const LuaPointer& cp) : mPtr(cp.mPtr), mRef(false) {}
    LuaPointer(LuaPointer&& cp) : mPtr(cp.mPtr), mRef(cp.mRef) {
        cp.mPtr = nullptr;
        cp.mRef = false;
    }
    LuaPointer& operator=(const LuaPointer& cp) {
        mPtr = cp.mPtr;
        mRef = false;
        return *this;
    }
    LuaPointer& operator=(LuaPointer&& cp) {
        mPtr = cp.mPtr;
        mRef = cp.mRef;
        cp.mPtr = nullptr;
        cp.mRef = false;
        return *this;
    }
    ~LuaPointer() {
        if (mPtr && mRef) {
            delete mPtr;
        }
        mPtr = nullptr;
        mRef = false;
    }

    T* get_ptr() { return mPtr; }
    T& get_ref() { return *mPtr; }

    const T& get_value() const { return *mPtr; }
    void set_value(const T& value) { *mPtr = value; }

private:
    T* mPtr;
    bool mRef;
};

}  // namespace PureLua
