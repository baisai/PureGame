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
#include <stdint.h>

namespace PureCore {
class PURECORE_API SleepIdler {
public:
    SleepIdler();

    void set_idle_delay(int64_t ms);
    int64_t get_idle_delay() const;

    bool set_offset(int64_t ms);
    int64_t get_offset() const;

    void frame_begin();
    int64_t frame_check(int64_t needDelta);
    void frame_end(int64_t suggestDelta = 10);

private:
    int64_t now() const;

private:
    int64_t mIdle = 0;
    int64_t mIdleDelay = 10 * 1000;
    int64_t mStart = 0;
    int64_t mLastNow = 0;
    int64_t mOffset = 0;
};
}  // namespace PureCore
