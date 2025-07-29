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

#include "PureCore/OsHelper.h"
#include "PureCore/SnowIDGen.h"

namespace PureCore {
SnowIDGen::SnowIDGen(uint16_t nodeID) : mNodeID(nodeID) {}

void SnowIDGen::reset(uint16_t nodeID) {
    mNodeID = nodeID;
    mLastID = 0;
}

int64_t SnowIDGen::gen_id() {
    check_time();
    while (mLastID >= UINT16_MAX) {
        PureCore::sleep(10);
        check_time();
    }
    return (int64_t(mNodeID) << 47) | (int64_t(mLastTime) << 16) | int64_t(++mLastID);
}

bool SnowIDGen::check_time() {
    uint32_t now = uint32_t(system_s() - 1577808000) & INT32_MAX;  // 2020.1.1
    if (now != mLastTime) {
        mLastID = 0;
        mLastTime = now;
        return true;
    }
    return false;
}

}  // namespace PureCore