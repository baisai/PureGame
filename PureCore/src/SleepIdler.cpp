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

#include "PureCore/SleepIdler.h"
#include "PureCore/OsHelper.h"
#include "PureCore/PureLog.h"

namespace PureCore {
SleepIdler::SleepIdler() {
    mLastNow = this->now();
    mIdle = this->now();
}

void SleepIdler::set_idle_delay(int64_t ms) { mIdleDelay = ms; }

int64_t SleepIdler::get_idle_delay() const { return mIdleDelay; }

bool SleepIdler::set_offset(int64_t ms) {
    if (ms <= mOffset) {
        return false;
    }
    mOffset = ms;
    return true;
}

int64_t SleepIdler::get_offset() const { return mOffset; }

void SleepIdler::frame_begin() { mStart = this->now(); }

int64_t SleepIdler::frame_check(int64_t needDelta) {
    int64_t delta = this->now() - mLastNow;
    if (delta >= needDelta) {
        mLastNow += delta;
        return delta;
    }
    return -1;
}

void SleepIdler::frame_end(int64_t suggestDelta) {
    int64_t now = this->now();
    int64_t delta = now - mStart;
    int64_t checkDelta = now - mLastNow;
    if (delta > 2) {
        mIdle = now;
        PureCore::yeild();
        return;
    }
    if (suggestDelta <= checkDelta) {
        PureCore::yeild();
        return;
    }
    if (mIdleDelay <= 0 || now - mIdle > mIdleDelay) {
        int64_t s = std::min(int64_t(5), suggestDelta - checkDelta);
        PureCore::sleep(s);
        return;
    }
    PureCore::yeild();
}

int64_t SleepIdler::now() const { return steady_milli_s() + mOffset; }

}  // namespace PureCore