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
#include "PureCore/Buffer/FixedBuffer.h"

#include <random>

namespace PureCore {
class PURECORE_API RandomGen {
public:
    RandomGen();

    void reset(uint32_t seed);

    uint32_t gen_int();
    double gen_float();
    int gen_bytes(size_t size, IBuffer& bytes);

    uint32_t gen_less_int(uint32_t max);
    int32_t gen_between_int(int32_t n1, int32_t n2);
    double gen_less_float(double max);
    double gen_between_float(double n1, double n2);

private:
    std::mt19937 mRandomer;

    PURE_DISABLE_COPY(RandomGen)
};
}  // namespace PureCore
