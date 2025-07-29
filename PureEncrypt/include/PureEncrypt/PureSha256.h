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

#include "PureCore/DataRef.h"
#include "PureEncrypt/PureEncryptLib.h"

#include "wjcrypt.hpp"

namespace PureEncrypt {
class PUREENCRYPT_API PureSha256 {
public:
    PureSha256();
    ~PureSha256();

    int encode(PureCore::DataRef src);

    void start_encode();
    int update_encode(PureCore::DataRef src);
    void finish_encode();

    PureCore::DataRef get_output();
    std::string get_output_hex();
    std::string get_output_HEX();

    void clear();

private:
    void clear_hold_output();

private:
    Sha256Context mContext{};
    SHA256_HASH mOutput{};

    PURE_DISABLE_COPY(PureSha256)
};

}  // namespace PureEncrypt
