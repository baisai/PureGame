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
#include "PureApp/PureExe.h"
#include "PureApp/AppErrorDesc.h"

namespace PureApp {

PureExe::~PureExe() {
    PureCore::PureSignal::cleanup();
    PureCore::PureLogger::cleanup();
}

int PureExe::init(int argc, char** argv) {
    int err = mArgs.parser(argc, argv);
    if (err != PureCore::Success) {
        return ErrorExeInitFailed;
    }
    mBinDir.clear();
    mBinName.clear();
    std::string binPath(PureCore::get_bin_path());
    size_t pos = binPath.find_last_of('/');
    if (pos == std::string::npos) {
        mBinName = binPath;
    } else {
        mBinDir = binPath.substr(0, pos);
        mBinDir.push_back('/');
        mBinName = binPath.substr(pos + 1);
    }

    mName = mArgs.find_opt("-name");
    if (mName.empty()) {
        mName = mBinName;
    }
#ifdef _WIN32
    {
        std::string title(fmt::format("{} --> [{}{}]", mName, mBinDir, mBinName));
        SetConsoleTitle(title.c_str());
    }
#endif
    auto& logFile = mArgs.find_opt("-log");
    if (!logFile.empty()) {
        if (logger().add_hourly_file(logFile.c_str()) != PureCore::Success) {
            return ErrorExeInitFailed;
        }
        logger().start(mName, 16 * 1024);
    }

    mScript = mArgs.find_opt("-script");

#ifdef _WIN32
    signals().reset_dump_cfg(mBinDir + mName);
#else
    signals().reset_core_cfg();
    signals().ignore_signal(SIGPIPE);
#endif

    return Success;
}
const std::string& PureExe::bin_dir() const { return mBinDir; }

const std::string& PureExe::bin_name() const { return mBinName; }

const std::string& PureExe::get_name() const { return mName; }

void PureExe::set_name(const std::string& name) { mName = name; }

const std::string& PureExe::get_srcipt() const { return mScript; }

void PureExe::set_script(const std::string& script) { mScript = script; }

bool PureExe::has_opt(const char* opt) const { return mArgs.has_opt(opt); }

const std::string& PureExe::find_opt(const char* opt) const { return mArgs.find_opt(opt); }

PureCore::PureSignal& PureExe::signals() const { return *PureCore::PureSignal::inst(); }

PureCore::PureLogger& PureExe::logger() const { return *PureCore::PureLogger::inst(); }

}  // namespace PureApp
