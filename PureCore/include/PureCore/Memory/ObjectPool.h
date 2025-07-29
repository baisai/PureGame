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
#include "PureCore/Memory/FixedAllocator.h"

#include <utility>

namespace PureCore {
template <typename T, uint8_t BlockCount>
class ObjectPool {
public:
    ObjectPool() : mAllocator(sizeof(T), BlockCount) {}

    ~ObjectPool() {}

    template <typename... Args>
    T* get(Args&&... args) {
        void* p = mAllocator.allocate();
        return new (p) T(std::forward<Args>(args)...);
    }

    void free(T* p) {
        if (p == nullptr) {
            return;
        }
        p->~T();
        mAllocator.deallocate(p);
        if (mAllocator.get_empty_count() > 1) {
            mAllocator.gc(false);
        }
    }

private:
    FixedAllocator mAllocator;

    PURE_DISABLE_COPY(ObjectPool)
};

}  // namespace PureCore
