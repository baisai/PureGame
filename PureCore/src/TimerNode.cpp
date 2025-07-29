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
#include "PureCore/TimerNode.h"

namespace PureCore {
///////////////////////////////////////////////////////////////////////////
// TimerNode
//////////////////////////////////////////////////////////////////////////
TimerNode::TimerNode() : mTimerID(0), mTimerType(0), mCallback(nullptr), mNextTime(0), mInterval(0), mLeftTimes(0), mNextFromNow(false), mCalling(false) {}

TimerNode::~TimerNode() { release(); }

int TimerNode::init(int64_t timerID, int32_t timerType, int64_t startTime, int64_t interval, int64_t times, TimerCallback callback, bool nextFromNow) {
    if (callback == nullptr || times == 0 || interval <= 0) {
        return ErrorInvalidArg;
    }
    mTimerID = timerID;
    mTimerType = timerType;
    mCallback = callback;
    mNextTime = startTime;
    mInterval = interval;
    mLeftTimes = times;
    mNextFromNow = nextFromNow;
    return Success;
}

void TimerNode::release() {
    mTimerID = 0;
    mTimerType = 0;
    mCallback = nullptr;
    mNextTime = 0;
    mInterval = 0;
    mLeftTimes = 0;
    mNextFromNow = false;
    mCalling = false;
}

void TimerNode::time_out() {
    if (!mCallback || mLeftTimes == 0) {
        return;
    }
    if (mLeftTimes > 0) {
        --mLeftTimes;
    }
    mCalling = true;
    if (!mCallback(mTimerID, mTimerType, mLeftTimes)) {
        mLeftTimes = 0;
    }
    mCalling = false;
}

int64_t TimerNode::get_timer_id() const { return mTimerID; }

int32_t TimerNode::get_timer_type() const { return mTimerType; }

int64_t TimerNode::get_next_time() const { return mNextTime; }

int64_t TimerNode::get_left_times() const { return mLeftTimes; }

TimerCallback TimerNode::get_callback() const { return mCallback; }

bool TimerNode::remove_if_calling() {
    if (mCalling) {
        mLeftTimes = 0;
        return true;
    }
    return false;
}

void TimerNode::next_time(int64_t now) {
    if (mNextFromNow) {
        mNextTime = now + mInterval;
    } else {
        mNextTime += mInterval;
    }
}

}  // namespace PureCore