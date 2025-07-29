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

#include "PureCore/PureLog.h"
#include "PureEncrypt/EncryptErrorDesc.h"
#include "PureEncrypt/PureSha1.h"

namespace PureEncrypt {
static const size_t sSha1ResultSize = 20;
static const size_t sSha1HexResultSize = sSha1ResultSize * 2;

PureSha1::PureSha1() = default;

PureSha1::~PureSha1() = default;

int PureSha1::encode(PureCore::DataRef src) {
    start_encode();
    int err = update_encode(src);
    if (err != Success) {
        return err;
    }
    finish_encode();
    return Success;
}

void PureSha1::start_encode() { clear(); }

int PureSha1::update_encode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorSha1DataError;
    }
    Sha1Update(&mContext, src.data(), uint32_t(src.size()));
    return Success;
}
void PureSha1::finish_encode() { Sha1Finalise(&mContext, &mOutput); }

PureCore::DataRef PureSha1::get_output() { return PureCore::DataRef((char *)mOutput.bytes, sizeof(mOutput.bytes)); }

std::string PureSha1::get_output_hex() {
    std::string hex;
    PureCore::DataRef data = get_output();
    for (size_t i = 0; i < data.size(); ++i) {
        fmt::format_to(std::back_inserter(hex), "{0:02x}", data[i]);
    }
    return hex;
}

std::string PureSha1::get_output_HEX() {
    std::string hex;
    PureCore::DataRef data = get_output();
    for (size_t i = 0; i < data.size(); ++i) {
        fmt::format_to(std::back_inserter(hex), "{0:02X}", data[i]);
    }
    return hex;
}

void PureSha1::clear() {
    clear_hold_output();
    memset(&mOutput, 0, sizeof(mOutput));
}

void PureSha1::clear_hold_output() { Sha1Initialise(&mContext); }
}  // namespace PureEncrypt
