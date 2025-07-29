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
#include "PureCore/RBTimer.h"

namespace PureCore {
RBTimer::RBTimer() : mNow(0), mSortTimers(), mTimers(), mIDGen() {}

RBTimer::~RBTimer() { release(); }

void RBTimer::init() {
    release();
    mNow = 0;
}

void RBTimer::release() {
    mNow = 0;
    mSortTimers.clear();
    for (auto iter = mTimers.begin(); iter != mTimers.end(); ++iter) {
        mPool.free(iter->second);
    }
    mTimers.clear();
}

void RBTimer::update(int64_t delta) {
    if (delta <= 0) {
        return;
    }

    mNow += delta;
    auto iter = mSortTimers.begin();
    while (iter != mSortTimers.end() && iter->second != nullptr && iter->second->get_next_time() <= mNow && iter->second->get_left_times() != 0) {
        TimerNode* node = iter->second;
        mSortTimers.erase(iter);
        node->time_out();
        if (node->get_left_times() == 0) {
            remove_timer(node->get_timer_id());
        } else {
            node->next_time(mNow);
            add_timer_node(node);
        }
        iter = mSortTimers.begin();
    }
}

int64_t RBTimer::add_timer(int32_t timerType, int64_t startTime, int64_t interval, int64_t times, TimerCallback callback, bool nextFromNow) {
    if (callback == nullptr || times == 0 || startTime + interval <= 0) {
        return 0;
    }
    int64_t timerID = mIDGen.gen_id();
    TimerNode* timer = mPool.get();
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

bool RBTimer::remove_timer(int64_t timerID) {
    auto iter = mTimers.find(timerID);
    if (iter == mTimers.end() || iter->second == nullptr) {
        return false;
    }
    TimerNode* timer = iter->second;
    if (timer->remove_if_calling()) {
        return true;
    }
    mTimers.erase(iter);

    for (auto sortIter = mSortTimers.find(timer->get_next_time());
         sortIter != mSortTimers.end() && sortIter->second != nullptr && sortIter->second->get_next_time() <= timer->get_next_time(); ++sortIter) {
        if (sortIter->second->get_timer_id() == timerID) {
            mSortTimers.erase(sortIter);
            break;
        }
    }
    mPool.free(timer);
    return true;
}

void RBTimer::clear_timer() {
    for (auto iter = mTimers.begin(); iter != mTimers.end();) {
        mPool.free(iter->second);
        iter = mTimers.erase(iter);
    }
    mSortTimers.clear();
}

void RBTimer::add_timer_node(TimerNode* timer) {
    if (timer == nullptr) {
        return;
    }
    mSortTimers.insert(std::make_pair(timer->get_next_time(), timer));
}

}  // namespace PureCore