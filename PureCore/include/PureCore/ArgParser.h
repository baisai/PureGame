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

#include <string>
#include <vector>

namespace PureCore {
struct PURECORE_API Arg {
    Arg() : mIndex(0), mValue("") {}
    Arg(const Arg& cp) : mIndex(cp.mIndex), mOption(cp.mOption), mValue(cp.mValue) {}
    Arg& operator=(const Arg& right);
    ~Arg() {}
    uint32_t mIndex;
    std::string mOption;
    std::string mValue;
};

class PURECORE_API ArgParser {
    PURE_DISABLE_COPY(ArgParser)
public:
    ArgParser();
    ~ArgParser();
    int parser(int argc, char** argv);
    const std::string& command() const;
    size_t size() const;
    const Arg& get_arg(size_t index) const;
    const Arg& operator[](size_t index) const;

    bool has_opt(const char* opt) const;
    const std::string& find_opt(const char* opt) const;

    void clear();

private:
    std::string mCommand;
    std::vector<Arg> mArgs;
};
}  // namespace PureCore
