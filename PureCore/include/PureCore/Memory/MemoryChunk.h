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
#include "PureCore/NodeList.h"

#include <stdint.h>
#include <stddef.h>

namespace PureCore {

class PURECORE_API MemoryChunk : public Node {
public:
    MemoryChunk();
    ~MemoryChunk();
    int init(size_t blockSize, uint8_t countBlocks);
    void release();
    void* allocate(size_t blockSize);
    int deallocate(void* p, size_t blockSize);

    uint8_t free_count() const;

    static MemoryChunk* self(void* p, size_t blockSize);

private:
    uint8_t* mData;          // real memory
    uint8_t mFirstAvBlock;   // first block
    uint8_t mCountAvBlocks;  // count block

    PURE_DISABLE_COPY(MemoryChunk)
};

}  // namespace PureCore
