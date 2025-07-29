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

#include "PureCore/Buffer/ArrayBuffer.h"
#include "PureCore/Buffer/DynamicBuffer.h"
#include "PureEncrypt/PureEncryptLib.h"

#define XXTEA_KEY_LENGTH 16

namespace PureEncrypt {
class PUREENCRYPT_API PureXxtea {
public:
    PureXxtea();
    ~PureXxtea();

    int encode(PureCore::DataRef src, PureCore::StringRef key);

    int decode(PureCore::DataRef src, PureCore::StringRef key);

    PureCore::DynamicBuffer& get_output();

private:
    PureCore::DynamicBuffer mOutput;

    PURE_DISABLE_COPY(PureXxtea)
};

class PUREENCRYPT_API PureXxteaBlockEncoder {
public:
    PureXxteaBlockEncoder();
    ~PureXxteaBlockEncoder();

    int set_block_size(uint32_t size);
    uint32_t get_block_size() const;

    int encode(PureCore::DataRef src, PureCore::StringRef key);

    int start_encode(PureCore::StringRef key);
    int update_encode(PureCore::DataRef src);
    int flush_encode();
    int finish_encode();

    PureCore::DynamicBuffer& get_output();

    void clear();

private:
    int encode_data(PureCore::DataRef src);
    void clear_hold_output();
    bool is_running() const;

private:
    uint32_t mBlockSize;
    int32_t mPass[XXTEA_KEY_LENGTH / sizeof(int32_t)];

    PureCore::DynamicBuffer mInputCache;
    PureCore::DynamicBuffer mOutput;

    PURE_DISABLE_COPY(PureXxteaBlockEncoder)
};

class PUREENCRYPT_API PureXxteaBlockDecoder {
public:
    PureXxteaBlockDecoder();
    ~PureXxteaBlockDecoder();

    int decode(PureCore::DataRef src, PureCore::StringRef key);

    int start_decode(PureCore::StringRef key);
    int update_decode(PureCore::DataRef src);
    int finish_decode();

    PureCore::DynamicBuffer& get_output();

    void clear();

private:
    int decode_data(PureCore::DataRef src);
    void clear_hold_output();

private:
    uint32_t mBlockSize;
    uint32_t mNeedSize;
    int mDecodeStep;
    int32_t mPass[XXTEA_KEY_LENGTH / sizeof(int32_t)];

    PureCore::DynamicBuffer mInputCache;
    PureCore::DynamicBuffer mOutput;

    PURE_DISABLE_COPY(PureXxteaBlockDecoder)
};

}  // namespace PureEncrypt
