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

#include "PureCore/TWTimer.h"
#include "PureCore/Event.h"
#include "PureCore/SleepIdler.h"
#include "PureLua/PureLuaEnv.h"
#include "PureNet/PureNetThread.h"
#include "PureApp/PureAppLib.h"

namespace PureApp {
class PUREAPP_API PureApp {
public:
    PureApp() = default;
    ~PureApp() = default;

    const std::string& name() const;
    void set_hz(uint16_t hz);
    uint16_t get_hz() const;

    PureLua::PureLuaEnv& lua();

    int init(const std::string& name);
    void stop();
    bool is_running() const;

    double time() const;
    bool set_time(double s);

    int64_t time_s() const;
    int64_t time_milli_s() const;
    int64_t time_micro_s() const;

    int add_lua_archive(const char* path, const char* pwd = nullptr);
    void clear_lua_archive();

    void loop();

private:
    void update(int64_t delta);

public:
    PureCore::Event<> mEventStart;
    PureCore::Event<int64_t> mEventFrame;
    PureCore::Event<> mEventEnd;

private:
    bool mRunning = false;
    std::string mName;
    uint16_t mHz = 30;
    PureCore::TWTimer mTimer;
    PureLua::PureLuaEnv mLua;
    std::vector<std::pair<std::string, std::string>> mLuaArchive;
    int64_t mTimeZero = 0;
    int64_t mTimeOffset = 0;
    PureCore::SleepIdler mIdler;
    PureNet::PureNetThread mNet;
};

}  // namespace PureApp
