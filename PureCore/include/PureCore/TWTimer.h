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
#include "PureCore/NodeList.h"
#include "PureCore/Memory/ObjectPool.h"

#include <unordered_map>
#include <unordered_set>

namespace PureCore {
class TWTimerNode : public TimerNode, public Node {
public:
    TWTimerNode() = default;
    ~TWTimerNode() = default;

private:
    friend class TimerWheel;
    friend class TWTimer;

    void clear();

private:
    PURE_DISABLE_COPY(TWTimerNode)
};

class TimerWheel {
private:
    friend class TWTimer;
    TimerWheel();
    ~TimerWheel();

    int init(bool isLeaf, int64_t stepInterval, uint32_t stepCount);
    void release();

    bool is_leaf() const;

    bool add_timer(TWTimerNode* timer);

    int64_t get_next_time() const;
    void next_time();
    int64_t now() const;
    NodeList* get_current_step() const;
    bool is_next_wheel() const;

private:
    bool mIsLeaf;           // leaf wheel
    int64_t mNow;           // now
    int64_t mStepInterval;  // wheel step
    uint32_t mStepCount;    // node head count
    NodeList* mSteps;

    PURE_DISABLE_COPY(TimerWheel)
};

class PURECORE_API TWTimer {
public:
    TWTimer();
    ~TWTimer();

    int init(uint32_t stepCount = 256);
    void release();

    void update(int64_t delta);

    int64_t add_timer(int32_t timerType, int64_t startTime, int64_t interval, int64_t times, TimerCallback callback, bool nextFromNow = false);
    bool remove_timer(int64_t timerID);
    void clear_timer();

private:
    void try_trigger_timer(NodeList* timerStep);
    void try_turn_wheel(int64_t now);
    void add_timer_node(TWTimerNode* timer);

private:
    int64_t mNow;

    TimerWheel mWheels[4];
    NodeList mOverflowWheel;

    std::unordered_map<int64_t, TWTimerNode*> mTimers;

    IncrIDGen mIDGen;
    ObjectPool<TWTimerNode, 255> mPool;

    PURE_DISABLE_COPY(TWTimer)
};
}  // namespace PureCore
