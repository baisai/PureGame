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
#include "PureCore/Memory/MemoryChunk.h"
#include "PureCore/Memory/FixedAllocator.h"

namespace PureCore {

////////////////////////////////////////////////////////////////
// FixedAllocator
///////////////////////////////////////////////////////////////
FixedAllocator::FixedAllocator(size_t blockSize, uint8_t countBlocks)
    : mBlockSize(blockSize), mCountBlocks(countBlocks), mFullCount(0u), mEmptyCount(0u), mFullList(), mFreeList(), mEmptyList() {}

FixedAllocator::~FixedAllocator() {
    MemoryChunk *pChunk = nullptr;
    while (mFullList.get_front() != nullptr) {
        pChunk = mFullList.pop_front_t<MemoryChunk>();
        delete pChunk;
    }
    while (mFreeList.get_front() != nullptr) {
        pChunk = mFreeList.pop_front_t<MemoryChunk>();
        delete pChunk;
    }
    while (mEmptyList.get_front() != nullptr) {
        pChunk = mEmptyList.pop_front_t<MemoryChunk>();
        delete pChunk;
    }

    mFullList.clear();
    mFreeList.clear();
    mEmptyList.clear();
    mFullCount = 0u;
    mEmptyCount = 0u;
}

void *FixedAllocator::allocate() {
    MemoryChunk *pChunk = nullptr;
    if (mFreeList.empty()) {
        if (mEmptyList.empty()) {
            int err = create_chunk();
            if (err != Success) {
                return nullptr;
            }
        }
        pChunk = mEmptyList.get_front_t<MemoryChunk>();
    } else {
        pChunk = mFreeList.get_front_t<MemoryChunk>();
    }
    if (pChunk == nullptr) {
        return nullptr;
    }

    bool emptyBefore = pChunk->free_count() == mCountBlocks;
    void *p = pChunk->allocate(mBlockSize);
    if (p == nullptr) {
        return p;
    }
    if (emptyBefore) {  // before empty
        pChunk->leave();
        mFreeList.push_back(pChunk);
        --mEmptyCount;
    }
    if (pChunk->free_count() == 0) {  // now full
        pChunk->leave();
        ++mFullCount;
        mFullList.push_back(pChunk);
    }
    return p;
}

int FixedAllocator::create_chunk() {
    MemoryChunk *pChunk = new MemoryChunk();
    if (pChunk == nullptr) {
        return ErrorAllocMemoryFailed;
    }
    int err = pChunk->init(mBlockSize, mCountBlocks);
    if (err != Success) {
        delete pChunk;
        return err;
    }

    ++mEmptyCount;
    mEmptyList.push_back(pChunk);
    return Success;
}

void FixedAllocator::deallocate(void *p) {
    MemoryChunk *pChunk = MemoryChunk::self(p, mBlockSize);
    if (pChunk == nullptr) {
        return;
    }
    bool fullBefore = pChunk->free_count() == 0;
    int err = pChunk->deallocate(p, mBlockSize);
    if (err != Success) {
        PureError("FixedAllocator::deallocate failed {}", get_error_desc(err));
        return;
    }
    if (fullBefore) {  // full before
        pChunk->leave();
        mFreeList.push_back(pChunk);
        --mFullCount;
    }
    if (pChunk->free_count() == mCountBlocks) {  // now empty
        pChunk->leave();
        mEmptyList.push_back(pChunk);
        ++mEmptyCount;
    }
}

size_t FixedAllocator::get_full_count() const { return mFullCount; }

size_t FixedAllocator::get_empty_count() const { return mEmptyCount; }

void FixedAllocator::gc(bool all) {
    if (all) {
        MemoryChunk *pChunk = mEmptyList.get_front_t<MemoryChunk>();
        while (pChunk != nullptr) {
            delete pChunk;
            --mEmptyCount;
            pChunk = mEmptyList.get_front_t<MemoryChunk>();
        }
    } else {
        MemoryChunk *pChunk = mEmptyList.pop_front_t<MemoryChunk>();
        if (pChunk != nullptr) {
            delete pChunk;
            --mEmptyCount;
        }
    }
}

}  // namespace PureCore