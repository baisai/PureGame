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
#include "PureCore/IncrIDGen.h"
#include "PureCore/TimerNode.h"
#include "PureCore/Memory/ObjectPool.h"

#include <unordered_map>
#include <unordered_set>
#include <map>

namespace PureCore {
class PURECORE_API RBTimer {
public:
    RBTimer();
    ~RBTimer();

    void init();
    void release();

    void update(int64_t delta);

    int64_t add_timer(int32_t timerType, int64_t startTime, int64_t interval, int64_t times, TimerCallback callback, bool nextFromNow = false);
    bool remove_timer(int64_t timerID);
    void clear_timer();

private:
    void add_timer_node(TimerNode* timer);

private:
    int64_t mNow;

    std::multimap<int64_t, TimerNode*> mSortTimers;
    std::unordered_map<int64_t, TimerNode*> mTimers;

    IncrIDGen mIDGen;
    ObjectPool<TimerNode, 255> mPool;

    PURE_DISABLE_COPY(RBTimer)
};
}  // namespace PureCore
