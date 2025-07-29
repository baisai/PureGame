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
#include "PureCore/PureSignal.h"
#include "PureCore/PureLog.h"

#ifdef _WIN32
#include <DbgHelp.h>
#define DOT_CORE ".dmp"
#else
#include <sys/resource.h>
#define DOT_CORE ".core"
#endif

#ifdef _WIN32
static bool set_win_exception() {
    struct __ {
        static LONG __stdcall __UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) {
            PureCore::PureSignal::inst()->dump_core_file(pExceptionInfo);
            return EXCEPTION_EXECUTE_HANDLER;
        }
    };
    SetUnhandledExceptionFilter(__::__UnhandledExceptionFilter);
    return true;
}
#endif

namespace PureCore {
PureSignal* PureSignal::inst() {
    if (sInst == nullptr) {
        sInst = new PureSignal();
    }
    return sInst;
}

void PureSignal::cleanup() {
    if (sInst != nullptr) {
        delete sInst;
        sInst = nullptr;
    }
}

PureSignal::~PureSignal() { clear(); }

void PureSignal::clear() {
    for (int i = 0; i < PURE_ARRAY_SIZE(mHandles); ++i) {
        default_signal(i);
        mHandles[i] = nullptr;
    }
}

#ifdef _WIN32
int PureSignal::reset_dump_cfg(const std::string& name) {
    auto dir(PureCore::get_parent_path(name.c_str()));
    if (!PureCore::dir_exist(dir.c_str())) {
        return ErrorInvalidArg;
    }
    mDumpFullName = name;
    if (!set_win_exception()) {
        return ErrorNotSupport;
    }
    return Success;
}
#else
int PureSignal::reset_core_cfg() {
    struct rlimit rt;
    if (getrlimit(RLIMIT_CORE, &rt) != 0 || rt.rlim_max != RLIM_INFINITY) {
        return ErrorNotSupport;
    }
    if (rt.rlim_cur != RLIM_INFINITY) {
        rt.rlim_cur = RLIM_INFINITY;
        if (setrlimit(RLIMIT_CORE, &rt) != 0) {
            return ErrorNotSupport;
        }
    }
    return Success;
}
#endif

int PureSignal::reg_signal(int num, std::function<SignalHandler> handler) {
    if (num >= PURE_ARRAY_SIZE(mHandles)) {
        return ErrorInvalidArg;
    }
#ifdef _WIN32
    signal(num, [](int sn) { PureSignal::inst()->on_signal(sn); });
#else
    struct sigaction sat;
    memset(&sat, 0, sizeof(sat));
    sat.sa_sigaction = [](int sn, siginfo_t* pInfo, void* pContext) { PureSignal::inst()->on_signal(sn); };
    sat.sa_flags = SA_RESETHAND | SA_RESTART | SA_SIGINFO;
    if (sigaction(num, &sat, nullptr) != 0) {
        return ErrorNotSupport;
    }
#endif
    mHandles[num] = handler;
    return Success;
}

int PureSignal::ignore_signal(int num) {
    if (num >= PURE_ARRAY_SIZE(mHandles)) {
        return ErrorInvalidArg;
    }
    mHandles[num] = nullptr;
#ifdef _WIN32
    signal(num, SIG_IGN);
    return Success;
#else
    struct sigaction sat;
    memset(&sat, 0, sizeof(sat));
    sat.sa_handler = SIG_IGN;
    if (sigaction(num, &sat, nullptr) != 0) {
        return ErrorNotSupport;
    }
    return Success;
#endif
}

int PureSignal::default_signal(int num) {
    if (num >= PURE_ARRAY_SIZE(mHandles)) {
        return ErrorInvalidArg;
    }
    mHandles[num] = nullptr;
#ifdef _WIN32
    signal(num, SIG_DFL);
    return Success;
#else
    struct sigaction sat;
    memset(&sat, 0, sizeof(sat));
    sat.sa_handler = SIG_DFL;
    if (sigaction(num, &sat, nullptr) != 0) {
        return ErrorNotSupport;
    }
    return Success;
#endif
}

int PureSignal::raise_signal(int num) {
#ifdef _WIN32
    if (num == 9) {
        exit(-1);
        return Success;
    }
#endif
    if (raise(num) != 0) {
        return ErrorNotSupport;
    }
    return Success;
}

#ifdef _WIN32
int PureSignal::dump_core_file(void* context) {
    if (mDumpFullName.empty()) {
        return ErrorNotSupport;
    }
    timeval tv;
    gettimeofday(&tv);
    tm t = localtime(tv.tv_sec);
    std::string dst(fmt::format("{}.{:04}{:02}{:02}-{:02}{:02}{:02}.{}" DOT_CORE, mDumpFullName, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min,
                                t.tm_sec, get_process_id()));
    MINIDUMP_EXCEPTION_INFORMATION mdei;
    memset(&mdei, 0, sizeof(MINIDUMP_EXCEPTION_INFORMATION));
    mdei.ThreadId = (DWORD)get_thread_id();
    mdei.ExceptionPointers = (PEXCEPTION_POINTERS)context;
    mdei.ClientPointers = TRUE;
    MINIDUMP_EXCEPTION_INFORMATION* pmdei = context ? &mdei : nullptr;
    HANDLE dumpFileHandle =
        CreateFile(dst.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (dumpFileHandle == nullptr || dumpFileHandle == INVALID_HANDLE_VALUE) {
        return ErrorNotSupport;
    }
    _MINIDUMP_TYPE dmpType = (_MINIDUMP_TYPE)(MiniDumpWithPrivateReadWriteMemory | MiniDumpWithFullMemoryInfo | MiniDumpWithDataSegs | MiniDumpWithHandleData |
                                              MiniDumpWithThreadInfo | MiniDumpWithProcessThreadData | MiniDumpWithUnloadedModules);
    BOOL retCode = MiniDumpWriteDump(GetCurrentProcess(), (DWORD)get_process_id(), dumpFileHandle, dmpType, pmdei, nullptr, nullptr);
    CloseHandle(dumpFileHandle);
    if (retCode != TRUE) {
        return ErrorNotSupport;
    }
    return Success;
}
#else
int PureSignal::dump_core_file() { return raise_signal(SIGSEGV); }
#endif

void PureSignal::on_signal(int num) {
    if (num >= PURE_ARRAY_SIZE(mHandles)) {
        return;
    }
    if (mHandles[num]) {
        mHandles[num](num);
    }
}

PureSignal* PureSignal::sInst = nullptr;

}  // namespace PureCore
