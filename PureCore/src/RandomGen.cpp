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
#include "PureCore/RandomGen.h"
#include "PureCore/OsHelper.h"

namespace PureCore {

RandomGen::RandomGen() : mRandomer((uint32_t)system_s()) {}

void RandomGen::reset(uint32_t seed) { mRandomer.seed(seed); }

uint32_t RandomGen::gen_int() { return mRandomer(); }

double RandomGen::gen_float() { return gen_int() / static_cast<double>(std::numeric_limits<uint32_t>::max()); }

int RandomGen::gen_bytes(size_t size, IBuffer& bytes) {
    bytes.clear();
    for (size_t i = 0; i < size; ++i) {
        int err = bytes.write_char(static_cast<char>(gen_less_int(128)));
        if (err != Success) {
            return err;
        }
    }
    return Success;
}

uint32_t RandomGen::gen_less_int(uint32_t max) {
    if (max == 0) {
        return 0;
    }
    double num = gen_float() * max;
    return static_cast<uint32_t>(std::floor(num));
}

int32_t RandomGen::gen_between_int(int32_t n1, int32_t n2) {
    int32_t t = n2 - n1;
    if (t < 0) {
        t = -t;
        n1 = n2;  // min
    }
    return int32_t(gen_less_int(uint32_t(t))) + n1;
}

double RandomGen::gen_less_float(double max) {
    if (max <= 0) {
        return 0;
    }
    return gen_float() * max;
}

double RandomGen::gen_between_float(double n1, double n2) {
    double t = n2 - n1;
    if (t < 0) {
        t = -t;
        n1 = n2;  // min
    }
    return gen_less_float(t) + n1;
}

}  // namespace PureCore