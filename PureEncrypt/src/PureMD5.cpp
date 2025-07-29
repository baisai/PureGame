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
#include "PureEncrypt/PureMD5.h"

namespace PureEncrypt {
PureMD5::PureMD5() = default;

PureMD5::~PureMD5() = default;

int PureMD5::encode(PureCore::DataRef src) {
    start_encode();
    int err = update_encode(src);
    if (err != Success) {
        return err;
    }
    finish_encode();
    return Success;
}

void PureMD5::start_encode() { clear(); }

int PureMD5::update_encode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorMD5DataError;
    }
    Md5Update(&mContext, src.data(), uint32_t(src.size()));
    return Success;
}
void PureMD5::finish_encode() { Md5Finalise(&mContext, &mOutput); }

PureCore::DataRef PureMD5::get_output() { return PureCore::DataRef((char*)mOutput.bytes, sizeof(mOutput.bytes)); }

std::string PureMD5::get_output_hex() {
    std::string hex;
    PureCore::DataRef data = get_output();
    for (size_t i = 0; i < data.size(); ++i) {
        fmt::format_to(std::back_inserter(hex), "{0:02x}", data[i]);
    }
    return hex;
}

std::string PureMD5::get_output_HEX() {
    std::string hex;
    PureCore::DataRef data = get_output();
    for (size_t i = 0; i < data.size(); ++i) {
        fmt::format_to(std::back_inserter(hex), "{0:02X}", data[i]);
    }
    return hex;
}

void PureMD5::clear() {
    clear_hold_output();
    memset(&mOutput, 0, sizeof(mOutput));
}

void PureMD5::clear_hold_output() { Md5Initialise(&mContext); }
}  // namespace PureEncrypt