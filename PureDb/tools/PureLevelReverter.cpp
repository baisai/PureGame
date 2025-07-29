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
#include "PureDb/DbErrorDesc.h"
#include "PureDb/LevelDb/LevelConnector.h"

#include <iostream>

static void Usage() {
    std::cout << "Usage: PureLevelReverter -s backup -d dest" << std::endl;
    getchar();
}

int main(int argc, char** argv) {
    PureCore::ArgParser parser;
    parser.parser(argc, argv);
    if (parser.size() < 2) {
        Usage();
        return -1;
    }

    PureCore::StringRef src, dest;
    for (size_t i = 0; i < parser.size(); ++i) {
        if (parser[i].mOption == "-s") {
            src = parser[i].mValue;
        } else if (parser[i].mOption == "-d") {
            dest = parser[i].mValue;
        }
    }
    if (src.empty() || dest.empty()) {
        Usage();
        return -2;
    }

    PureDb::LevelConnector con;
    PureDb::LevelConfig cfg;
    cfg.mPath.assign(dest.data(), dest.size());
    int err = con.connect(cfg);
    if (err != 0) {
        std::cerr << PureDb::get_error_desc(err) << std::endl;
        return -3;
    }
    con.revert_from(src.data());
    if (err != 0) {
        std::cerr << PureDb::get_error_desc(err) << std::endl;
        return -4;
    }
    con.close();
    return 0;
}
