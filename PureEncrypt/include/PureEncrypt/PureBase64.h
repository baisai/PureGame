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

#include "PureCore/Buffer/DynamicBuffer.h"
#include "PureCore/Buffer/ArrayBuffer.h"
#include "PureEncrypt/PureEncryptLib.h"

namespace PureEncrypt {
class PUREENCRYPT_API PureBase64 {
public:
    PureBase64();
    ~PureBase64();

    int encode(PureCore::DataRef src);
    int decode(PureCore::DataRef src);

    int start_encode();
    int update_encode(PureCore::DataRef src);
    int finish_encode();

    int start_decode();
    int update_decode(PureCore::DataRef src);
    int finish_decode();

    PureCore::DynamicBuffer& get_output();

    void clear();

private:
    int encode_data(PureCore::DataRef src);
    int decode_data(PureCore::DataRef src);
    void clear_hold_output();

private:
    PureCore::ArrayBuffer<4> mInputCache;
    PureCore::DynamicBuffer mOutput;

    PURE_DISABLE_COPY(PureBase64)
};
}  // namespace PureEncrypt
