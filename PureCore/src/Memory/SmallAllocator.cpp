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

#include "PureCore/CoreErrorDesc.h"
#include "PureCore/PureLog.h"
#include "PureCore/Memory/SmallAllocator.h"

namespace PureCore {
//////////////////////////////////////////////////////////////
// SmallAllocator
/////////////////////////////////////////////////////////////
SmallAllocator::SmallAllocator() { memset(mPool, 0, sizeof(mPool)); }

SmallAllocator::~SmallAllocator() {
    for (size_t i = 0u; i < PURE_ARRAY_SIZE(mPool); ++i) {
        if (mPool[i]) {
            delete mPool[i];
        }
    }
}

void *SmallAllocator::allocate(size_t size) {
    if (size > BigObjectSize) {
        return ::malloc(size);
    } else {
        if (size == 0) {
            size = 1;
        }
        size_t index = (size - 1) >> OffSetBit;
        FixedAllocator *f = *(mPool + index);
        if (!f)  // nullptr
        {
            if (!*(mPool + index)) {
                f = new FixedAllocator((index + 1) << OffSetBit, 255);
                if (!f) {
                    return nullptr;
                }
                *(mPool + index) = f;
            } else {
                f = *(mPool + index);
            }
        }
        return f->allocate();
    }
    return nullptr;
}

void SmallAllocator::deallocate(void *p, size_t size) {
    if (p) {
        if (size > BigObjectSize) {
            ::free(p);
        } else {
            if (size == 0) {
                size = 1;
            }
            FixedAllocator *pdell = *(mPool + ((size - 1) >> OffSetBit));
            if (pdell) {
                pdell->deallocate(p);
                if (pdell->get_empty_count() > 1) {
                    pdell->gc(false);
                }
            } else {
                PureError("SmallAllocator::deallocate `{}` failed, not found FixedAllocator", p);
            }
        }
    }
}

void *SmallAllocator::reallocate(void *p, size_t osize, size_t nsize) {
    if (p == nullptr) {
        return allocate(nsize);
    }
    if (nsize == 0) {
        deallocate(p, osize);
        return nullptr;
    }
    if (((osize - 1) >> OffSetBit) == ((nsize - 1) >> OffSetBit)) {
        return p;
    }
    if (osize > BigObjectSize && nsize > BigObjectSize) {
        return ::realloc(p, nsize);
    }

    void *np = allocate(nsize);
    if (np) {
        if (osize < nsize) {
            memcpy(np, p, osize);
        } else {
            memcpy(np, p, nsize);
        }
    } else {
        return nullptr;
    }
    deallocate(p, osize);
    return np;
}

void SmallAllocator::gc(bool all) {
    for (size_t i = 0u; i < PURE_ARRAY_SIZE(mPool); ++i) {
        if (mPool[i]) {
            mPool[i]->gc(all);
        }
    }
}
}  // namespace PureCore