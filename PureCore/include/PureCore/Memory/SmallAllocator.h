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

#include <stdint.h>

namespace PureCore {
class PURECORE_API SmallAllocator {
public:
    enum ESizeConst {
        BigObjectSize = PURE_BIG_SIZE,
        OffSet = 1 << PURE_ALIGN_BIT,
        OffSetBit = PURE_ALIGN_BIT,
    };

public:
    SmallAllocator();
    ~SmallAllocator();

    void* allocate(size_t size);
    void deallocate(void* p, size_t size);
    void* reallocate(void* p, size_t osize, size_t nsize);

    void gc(bool all);

private:
    FixedAllocator* mPool[(BigObjectSize + OffSet - 1) / OffSet];

    PURE_DISABLE_COPY(SmallAllocator)
};

}  // namespace PureCore
