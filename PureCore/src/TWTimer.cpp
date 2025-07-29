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
#include "PureCore/TWTimer.h"

namespace PureCore {
///////////////////////////////////////////////////////////////////////////
// TWTimerNode
//////////////////////////////////////////////////////////////////////////
void TWTimerNode::clear() {
    release();
    leave();
}

///////////////////////////////////////////////////////////////////////////
// TimerWheel
//////////////////////////////////////////////////////////////////////////
TimerWheel::TimerWheel() : mIsLeaf(false), mNow(0), mStepInterval(0), mStepCount(0), mSteps(nullptr) {}

TimerWheel::~TimerWheel() { release(); }

int TimerWheel::init(bool isLeaf, int64_t stepInterval, uint32_t stepCount) {
    if (stepInterval <= 0 || stepCount == 0) {
        return ErrorInvalidArg;
    }
    release();
    mIsLeaf = isLeaf;
    mNow = 0;
    mStepInterval = stepInterval;
    mStepCount = stepCount;
    mSteps = new NodeList[mStepCount];
    return Success;
}

void TimerWheel::release() {
    mIsLeaf = false;
    mNow = 0;
    mStepInterval = 0;
    mStepCount = 0;
    if (mSteps != nullptr) {
        delete[] mSteps;
    }
    mSteps = nullptr;
}

bool TimerWheel::is_leaf() const { return mIsLeaf; }

bool TimerWheel::add_timer(TWTimerNode* timer) {
    if (timer == nullptr || mSteps == nullptr || mStepInterval <= 0) {
        return false;
    }
    uint32_t offset = 0;
    int64_t current = mNow / mStepInterval;
    if (timer->get_next_time() > mNow) {
        offset = static_cast<uint32_t>(timer->get_next_time() / mStepInterval - current);
        if (offset >= mStepCount) {
            return false;
        }
    }

    offset = (offset + current) % mStepCount;
    mSteps[offset].push_back(timer);
    return true;
}

int64_t TimerWheel::get_next_time() const { return mNow + mStepInterval; }

void TimerWheel::next_time() { mNow += mStepInterval; }

int64_t TimerWheel::now() const { return mNow; }

NodeList* TimerWheel::get_current_step() const {
    if (mSteps == nullptr) {
        return nullptr;
    }
    int64_t current = (mNow / mStepInterval) % mStepCount;
    return (mSteps + current);
}

bool TimerWheel::is_next_wheel() const { return (mNow / mStepInterval) % mStepCount == 0; }

///////////////////////////////////////////////////////////////////////////
// TWTimer
//////////////////////////////////////////////////////////////////////////
TWTimer::TWTimer() : mNow(0), mOverflowWheel(), mTimers(), mIDGen(), mPool() {}

TWTimer::~TWTimer() { release(); }

int TWTimer::init(uint32_t stepCount) {
    if (stepCount == 0) {
        return ErrorInvalidArg;
    }
    release();
    mNow = 0;
    int64_t interval = 1;
    for (size_t i = 0; i < PURE_ARRAY_SIZE(mWheels); ++i) {
        int err = mWheels[i].init(i == 0, interval, stepCount);
        if (err != Success) {
            return err;
        }
        interval *= stepCount;
    }
    return Success;
}

void TWTimer::release() {
    mNow = 0;
    for (size_t i = 0; i < PURE_ARRAY_SIZE(mWheels); ++i) {
        mWheels[i].release();
    }
    mOverflowWheel.clear();
    for (auto iter = mTimers.begin(); iter != mTimers.end(); ++iter) {
        mPool.free(iter->second);
    }
    mTimers.clear();
}

void TWTimer::update(int64_t delta) {
    if (delta <= 0) {
        return;
    }

    mNow += delta;
    while (mWheels[0].get_next_time() <= mNow) {
        NodeList* timerStep = mWheels[0].get_current_step();
        mWheels[0].next_time();
        if (mWheels[0].is_next_wheel()) {
            try_turn_wheel(mWheels[0].now());
        }
        try_trigger_timer(timerStep);
    }
    try_trigger_timer(mWheels[0].get_current_step());
}

int64_t TWTimer::add_timer(int32_t timerType, int64_t startTime, int64_t interval, int64_t times, TimerCallback callback, bool nextFromNow) {
    if (callback == nullptr || times == 0 || startTime + interval <= 0) {
        return 0;
    }
    int64_t timerID = mIDGen.gen_id();
    TWTimerNode* timer = mPool.get();
    int err = timer->init(timerID, timerType, mNow + startTime, interval, times, callback, nextFromNow);
    if (err != Success) {
        mPool.free(timer);
        return 0;
    }

    if (!mTimers.insert(std::make_pair(timer->get_timer_id(), timer)).second) {
        mPool.free(timer);
        return 0;
    }

    add_timer_node(timer);
    return timer->get_timer_id();
}

bool TWTimer::remove_timer(int64_t timerID) {
    auto iter = mTimers.find(timerID);
    if (iter == mTimers.end()) {
        return false;
    }
    if (iter->second->remove_if_calling()) {
        return true;
    }
    mPool.free(iter->second);
    mTimers.erase(iter);
    return true;
}

void TWTimer::clear_timer() {
    for (auto iter = mTimers.begin(); iter != mTimers.end();) {
        mPool.free(iter->second);
        iter = mTimers.erase(iter);
    }
}

void TWTimer::try_trigger_timer(NodeList* timerStep) {
    if (timerStep == nullptr) {
        return;
    }
    TWTimerNode* node = timerStep->get_front_t<TWTimerNode>();
    while (node != nullptr && node->get_next_time() <= mNow && node->get_left_times() != 0) {
        node->time_out();
        node->leave();
        node->next_time(mNow);
        if (node->get_left_times() == 0) {
            remove_timer(node->get_timer_id());
        } else {
            add_timer_node(node);
        }
        node = timerStep->get_front_t<TWTimerNode>();
    }
}

void TWTimer::try_turn_wheel(int64_t now) {
    NodeList list;
    for (size_t i = 1; i < PURE_ARRAY_SIZE(mWheels); ++i) {
        while (mWheels[i].get_next_time() <= now) {
            mWheels[i].next_time();
            NodeList* timerStep = mWheels[i].get_current_step();
            if (timerStep == nullptr) {
                break;
            }
            list.push_back_list(*timerStep);
            while (!list.empty()) {
                TWTimerNode* node = list.pop_front_t<TWTimerNode>();
                add_timer_node(node);
            }

            if (i == PURE_ARRAY_SIZE(mWheels) - 1 && mWheels[i].is_next_wheel()) {
                list.push_back_list(mOverflowWheel);
                while (!list.empty()) {
                    TWTimerNode* node = list.pop_front_t<TWTimerNode>();
                    add_timer_node(node);
                }
            }
        }
    }
}

void TWTimer::add_timer_node(TWTimerNode* timer) {
    for (size_t i = 0; i < PURE_ARRAY_SIZE(mWheels); ++i) {
        if (mWheels[i].add_timer(timer)) {
            return;
        }
    }
    mOverflowWheel.push_back(timer);
}

}  // namespace PureCore