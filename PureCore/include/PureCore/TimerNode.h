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

#include <functional>

namespace PureCore {

typedef std::function<bool(int64_t timerID, int32_t timerType, int64_t leftTimes)> TimerCallback;

class PURECORE_API TimerNode {
public:
    TimerNode();
    virtual ~TimerNode();

    int init(int64_t timerID, int32_t timerType, int64_t startTime, int64_t interval, int64_t times, TimerCallback callback, bool nextFromNow);
    void release();

    void time_out();

    int64_t get_timer_id() const;
    int32_t get_timer_type() const;
    int64_t get_next_time() const;
    int64_t get_left_times() const;
    TimerCallback get_callback() const;

    bool remove_if_calling();

    void next_time(int64_t now);

private:
    int64_t mTimerID;
    int32_t mTimerType;
    TimerCallback mCallback;

    int64_t mNextTime;
    int64_t mInterval;
    int64_t mLeftTimes;
    bool mCalling;
    bool mNextFromNow;

    PURE_DISABLE_COPY(TimerNode)
};

}  // namespace PureCore
