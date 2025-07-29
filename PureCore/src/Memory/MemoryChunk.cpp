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

namespace PureCore {
///////////////////////////////////////////////////////////////////
// MemoryChunk
//////////////////////////////////////////////////////////////////
MemoryChunk::MemoryChunk() : Node(), mData(nullptr), mFirstAvBlock(0), mCountAvBlocks(0) {}

MemoryChunk::~MemoryChunk() { release(); }

int MemoryChunk::init(size_t blockSize, uint8_t countBlocks) {
    if (blockSize == 0 || countBlocks == 0) {
        return ErrorInvalidArg;
    }
    uint8_t *p = static_cast<uint8_t *>(::malloc((blockSize + sizeof(uint8_t)) * countBlocks + sizeof(void *)));
    if (!p) {
        return ErrorAllocMemoryFailed;
    }
    release();
    mData = p + sizeof(void *) + sizeof(uint8_t);
    mFirstAvBlock = 0;
    mCountAvBlocks = countBlocks;
    *reinterpret_cast<MemoryChunk **>(p) = this;
    p += sizeof(void *);
    for (uint8_t i = 0; i < countBlocks; p += (blockSize + sizeof(uint8_t))) {
        *p = i;
        *(p + sizeof(uint8_t)) = ++i;
    }
    return Success;
}
void MemoryChunk::release() {
    if (mData) {
        ::free(mData - sizeof(void *) - sizeof(uint8_t));
        mData = nullptr;
        mFirstAvBlock = 0;
        mCountAvBlocks = 0;
    }
    leave();
}
void *MemoryChunk::allocate(size_t blockSize) {
    if (!mCountAvBlocks || mData == nullptr) {  // not free block
        return nullptr;
    }
    uint8_t *p = mData + (mFirstAvBlock * (blockSize + sizeof(uint8_t)));  // calc allocate address
    mFirstAvBlock = *p;
    --mCountAvBlocks;
    return p;
}
int MemoryChunk::deallocate(void *p, size_t blockSize) {
    uint8_t *real = static_cast<uint8_t *>(p);
    if (real < mData || (real - mData) % (blockSize + sizeof(uint8_t)) != 0) {
        return ErrorInvalidMemory;
    }
    *real = mFirstAvBlock;  // record next free block
    mFirstAvBlock = *(real - sizeof(uint8_t));
    ++mCountAvBlocks;
    return Success;
}

uint8_t MemoryChunk::free_count() const { return mCountAvBlocks; }

MemoryChunk *MemoryChunk::self(void *p, size_t blockSize) {
    if (p == nullptr) {
        return nullptr;
    }
    uint8_t *real = static_cast<uint8_t *>(p) - sizeof(uint8_t);
    size_t offset = (*real) * (blockSize + sizeof(uint8_t)) + sizeof(void *);
    return *reinterpret_cast<MemoryChunk **>(real - offset);
}

}  // namespace PureCore