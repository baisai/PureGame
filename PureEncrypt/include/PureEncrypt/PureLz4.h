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
#include "PureEncrypt/PureEncryptLib.h"

#include "lz4.h"
#include "lz4frame.h"

namespace PureEncrypt {
class PUREENCRYPT_API PureLz4BlockEncoder {
public:
    PureLz4BlockEncoder();
    ~PureLz4BlockEncoder();

    int set_block_size(uint32_t size);
    uint32_t get_block_size() const;
    int set_encode_level(int16_t level);
    int16_t get_encode_level() const;

    int encode(PureCore::DataRef src);

    int start_encode();
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
    LZ4_stream_t* mContext;
    uint32_t mBlockSize;
    uint16_t mLevel;
    PureCore::DynamicBuffer mInputCache;
    PureCore::DynamicBuffer mOutput;

    PURE_DISABLE_COPY(PureLz4BlockEncoder)
};

class PUREENCRYPT_API PureLz4BlockDecoder {
public:
    PureLz4BlockDecoder();
    ~PureLz4BlockDecoder();

    int decode(PureCore::DataRef src);

    int start_decode();
    int update_decode(PureCore::DataRef src);
    int finish_decode();

    PureCore::DynamicBuffer& get_output();

    void clear();

private:
    int decode_data(PureCore::DataRef src);
    void clear_hold_output();

private:
    LZ4_streamDecode_t* mContext;
    uint32_t mBlockSize;
    uint32_t mNeedSize;
    int mDecodeStep;
    PureCore::DynamicBuffer mInputCache;
    PureCore::DynamicBuffer mOutputCache;
    PureCore::DynamicBuffer mOutput;

    PURE_DISABLE_COPY(PureLz4BlockDecoder)
};

class PUREENCRYPT_API PureLz4FrameEncoder {
public:
    PureLz4FrameEncoder();
    ~PureLz4FrameEncoder();

    int set_preference(const LZ4F_preferences_t& preference);

    int encode(PureCore::DataRef src);

    int start_encode();
    int update_encode(PureCore::DataRef src);
    int finish_encode();

    PureCore::DynamicBuffer& get_output();
    const std::string& get_lz4_error() const;

    void clear();

private:
    void clear_hold_output();
    bool is_running() const;

private:
    LZ4F_compressionContext_t mContext;
    LZ4F_preferences_t mPreference;
    PureCore::DynamicBuffer mOutput;
    std::string mLz4Err;

    PURE_DISABLE_COPY(PureLz4FrameEncoder)
};

class PUREENCRYPT_API PureLz4FrameDecoder {
public:
    PureLz4FrameDecoder();
    ~PureLz4FrameDecoder();

    int decode(PureCore::DataRef src);

    int start_decode();
    int update_decode(PureCore::DataRef src);
    int finish_decode();

    PureCore::DynamicBuffer& get_output();
    const std::string& get_lz4_error() const;

    void clear();

private:
    int decode_data(PureCore::DataRef src, size_t* costSize);
    void clear_hold_output();

private:
    LZ4F_dctx* mContext;
    uint32_t mBlockSize;
    uint32_t mNeedSize;
    PureCore::DynamicBuffer mInputCache;
    PureCore::DynamicBuffer mOutput;
    std::string mLz4Err;

    PURE_DISABLE_COPY(PureLz4FrameDecoder)
};

}  // namespace PureEncrypt
