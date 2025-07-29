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

#include "PureCore/OsHelper.h"

#include <string>
#include <vector>
#include <functional>
#include <signal.h>

#ifdef _WIN32
#define PURE_MAXSIGNAL NSIG
#else
#define PURE_MAXSIGNAL _NSIG
#endif  // _WIN32

namespace PureCore {
typedef void(SignalHandler)(int);
// only used main thread
class PURECORE_API PureSignal {
public:
    static PureSignal* inst();
    static void cleanup();

    PureSignal() = default;
    ~PureSignal();

    void clear();

#ifdef _WIN32
    int reset_dump_cfg(const std::string& name);
#else
    int reset_core_cfg();
#endif
    int reg_signal(int num, std::function<SignalHandler> handler);
    int ignore_signal(int num);
    int default_signal(int num);
    int raise_signal(int num);

#ifdef _WIN32
    int dump_core_file(void* context = nullptr);
#else
    int dump_core_file();
#endif
private:
    void on_signal(int num);

private:
#ifdef _WIN32
    std::string mDumpFullName;
#endif
    std::function<SignalHandler> mHandles[PURE_MAXSIGNAL];

    static PureSignal* sInst;

    PURE_DISABLE_COPY(PureSignal)
};

}  // namespace PureCore
