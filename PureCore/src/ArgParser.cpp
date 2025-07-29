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

#include "PureCore/ArgParser.h"
#include "PureCore/CoreErrorDesc.h"

#include <algorithm>

namespace PureCore {
Arg& Arg::operator=(const Arg& right) {
    mIndex = right.mIndex;
    mOption = right.mOption;
    mValue = right.mValue;
    return *this;
}

ArgParser::ArgParser() {}
ArgParser::~ArgParser() {}
int ArgParser::parser(int argc, char** argv) {
    if (argc < 1) {
        return ErrorInvalidArg;
    }
    mCommand = argv[0];
    mArgs.clear();
    Arg arg;
    int idx = 1;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (arg.mIndex != 0) {
                mArgs.push_back(arg);
            }
            arg.mIndex = idx++;
            arg.mOption = argv[i];
            arg.mValue = "";
        } else {
            if (arg.mIndex == 0) {
                arg.mIndex = idx++;
            }
            arg.mValue = argv[i];
            mArgs.push_back(arg);
            arg.mIndex = 0;
            arg.mOption.clear();
            arg.mValue = "";
        }
    }
    if (arg.mIndex != 0) {
        mArgs.push_back(arg);
    }
    for (size_t i = 0; i < mArgs.size(); ++i) {
        std::transform(mArgs[i].mOption.begin(), mArgs[i].mOption.end(), mArgs[i].mOption.begin(), ::tolower);
    }
    return Success;
}

const std::string& ArgParser::command() const { return mCommand; }

size_t ArgParser::size() const { return mArgs.size(); }

const Arg& ArgParser::get_arg(size_t index) const { return mArgs.at(index); }
const Arg& ArgParser::operator[](size_t index) const { return mArgs[index]; }

bool ArgParser::has_opt(const char* opt) const {
    for (auto const& arg : mArgs) {
        if (arg.mOption == opt) {
            return true;
        }
    }
    return false;
}

static const std::string sEmptyStr{};
const std::string& ArgParser::find_opt(const char* opt) const {
    for (auto const& arg : mArgs) {
        if (arg.mOption == opt) {
            return arg.mValue;
        }
    }
    return sEmptyStr;
}

void ArgParser::clear() {
    mCommand.clear();
    mArgs.clear();
}

}  // namespace PureCore