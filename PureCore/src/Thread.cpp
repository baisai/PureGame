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
#include "PureCore/OsHelper.h"
#include "PureCore/Thread.h"

namespace PureCore {
int Thread::run() {
    if (is_running()) {
        return ErrorThreadAlreadyRunning;
    }
    std::thread t([=]() {
        this->mHandle = get_thread_id();
        this->work();
        this->mHandle = 0;
    });
    t.detach();
    return Success;
}

bool Thread::is_running() const { return mHandle.load(std::memory_order_relaxed) != 0; }

int Thread::join(int64_t timeout) {
    if (get_thread_id() == mHandle.load(std::memory_order_relaxed)) {
        return ErrorNotJoinSelfThread;
    }
    int64_t left = timeout > 0 ? timeout : 1;
    while (is_running() && left > 0) {
        PureCore::sleep(10);
        if (timeout > 0) {
            left -= 10;
        }
    }
    return Success;
}

}  // namespace PureCore