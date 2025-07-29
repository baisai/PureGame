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

#include "PureCore/ReuseIDGen.h"

namespace PureCore {

ReuseIDGen::ReuseIDGen() : mLastID(0) {}

void ReuseIDGen::reset() {
    mLastID = 0;
    mUsedIDs.clear();
}

uint32_t ReuseIDGen::gen_id() {
    if (mUsedIDs.size() >= UINT32_MAX - 1) {
        return 0;
    }

    auto endIter = mUsedIDs.end();
    while (true) {
        if (mLastID >= UINT32_MAX) {
            mLastID = 0;
        }
        ++mLastID;
        if (mUsedIDs.find(mLastID) == endIter) {
            mUsedIDs.emplace(mLastID);
            return mLastID;
        }
    }
    return 0;
}

void ReuseIDGen::use_id(uint32_t id) { mUsedIDs.emplace(id); }

void ReuseIDGen::free_id(uint32_t id) {
    mUsedIDs.erase(id);
    auto endIter = mUsedIDs.end();
    while (mUsedIDs.find(mLastID) == endIter) {
        --mLastID;
    }
}

}  // namespace PureCore