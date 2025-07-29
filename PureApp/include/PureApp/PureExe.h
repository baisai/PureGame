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

#include "PureCore/ArgParser.h"
#include "PureCore/PureSignal.h"
#include "PureCore/PureLogger.h"
#include "PureApp/PureAppLib.h"

#include <string>

namespace PureApp {
class PUREAPP_API PureExe {
public:
    PureExe() = default;
    ~PureExe();

    int init(int argc, char** argv);
    const std::string& bin_dir() const;
    const std::string& bin_name() const;

    const std::string& get_name() const;
    void set_name(const std::string& name);
    const std::string& get_srcipt() const;
    void set_script(const std::string& script);

    bool has_opt(const char* opt) const;
    const std::string& find_opt(const char* opt) const;

    PureCore::PureSignal& signals() const;
    PureCore::PureLogger& logger() const;

private:
    std::string mBinDir;
    std::string mBinName;

    std::string mName;
    std::string mScript;

    PureCore::ArgParser mArgs;
};

}  // namespace PureApp
