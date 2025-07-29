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

#include "PureCore/ByteOrder.h"
#include "PureCore/PureLog.h"
#include "PureCore/OsHelper.h"
#include "PureEncrypt/EncryptErrorDesc.h"
#include "PureEncrypt/PureSnappy.h"

#include "snappy-c.h"

namespace PureEncrypt {
/////////////////////////////////////////////////////////////////
/// PureSnappy
///////////////////////////////////////////////////////////////
PureSnappy::PureSnappy() : mOutput() {}

PureSnappy::~PureSnappy() { mOutput.clear(); }

int PureSnappy::encode(PureCore::DataRef src) {
    mOutput.clear();
    if (src.size() == 0) {
        return Success;
    }
    size_t maxLen = snappy_max_compressed_length(src.size());
    if (mOutput.ensure_buffer(maxLen) != PureCore::Success) {
        return ErrorCoreBufferFailed;
    }
    size_t len = maxLen;
    if (snappy_compress(src.data(), src.size(), mOutput.free_buffer().data(), &len) != SNAPPY_OK) {
        return ErrorSnappyEncodeFailed;
    }
    mOutput.write_pos(len);
    return Success;
}

int PureSnappy::decode(PureCore::DataRef src) {
    mOutput.clear();
    if (src.size() == 0) {
        return Success;
    }
    size_t needLen = 0;
    if (snappy_uncompressed_length(src.data(), src.size(), &needLen) != SNAPPY_OK) {
        return ErrorSnappyDecodeFailed;
    }
    if (mOutput.ensure_buffer(needLen) != PureCore::Success) {
        return ErrorCoreBufferFailed;
    }
    size_t len = needLen;
    if (snappy_uncompress(src.data(), src.size(), mOutput.free_buffer().data(), &len) != SNAPPY_OK) {
        return ErrorSnappyEncodeFailed;
    }
    mOutput.write_pos(len);
    return Success;
}

PureCore::DynamicBuffer &PureSnappy::get_output() { return mOutput; }

/////////////////////////////////////////////////////////////////
/// PureSnappyBlock
///////////////////////////////////////////////////////////////
static const char sSnappyBlockHeader[2]{'s', char(205)};
static const size_t sSnappyBlockHeaderSize = sizeof(sSnappyBlockHeader) + sizeof(uint32_t);
static const size_t sSnappyDefaultBlockSize = 8 * 1024;
static const size_t sSnappyMaxBlockSize = 1024 * 1024 * 4;

enum EPureSnappyDataDecodeStep {
    DecodeStepHeader = 0,
    DecodeStepLen = 1,
    DecodeStepData = 2,
};
/////////////////////////////////////////////////////////////////
/// PureSnappyBlockEncoder
///////////////////////////////////////////////////////////////
PureSnappyBlockEncoder::PureSnappyBlockEncoder() : mBlockSize(sSnappyDefaultBlockSize), mRunning(false), mInputCache(), mOutput() {}

PureSnappyBlockEncoder::~PureSnappyBlockEncoder() { clear(); }

int PureSnappyBlockEncoder::set_block_size(uint32_t size) {
    if (is_running()) {
        return ErrorSnappyStepError;
    }
    mBlockSize = size;
    return Success;
}

uint32_t PureSnappyBlockEncoder::get_block_size() const { return mBlockSize; }

int PureSnappyBlockEncoder::encode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorSnappyDataError;
    }
    int err = start_encode();
    if (err != Success) {
        return err;
    }
    err = update_encode(src);
    if (err != Success) {
        return err;
    }
    return finish_encode();
}

int PureSnappyBlockEncoder::start_encode() {
    if (mBlockSize == 0 || mBlockSize > sSnappyMaxBlockSize) {
        return ErrorSnappyBlockSizeError;
    }
    clear();
    mRunning = true;
    int err = mInputCache.resize_buffer(mBlockSize);
    if (err != Success) {
        return err;
    }
    err = mOutput.ensure_buffer(sSnappyBlockHeaderSize);
    if (err != Success) {
        return err;
    }
    err = mOutput.write(PureCore::DataRef(sSnappyBlockHeader, sizeof(sSnappyBlockHeader)));
    if (err != Success) {
        return err;
    }
    PureCore::u32_to_four_char(mBlockSize, mOutput.free_buffer().data());
    mOutput.write_pos(mOutput.write_pos() + sizeof(mBlockSize));
    return Success;
}
int PureSnappyBlockEncoder::update_encode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorSnappyDataError;
    }

    while (mInputCache.size() >= mBlockSize) {
        int err = encode_data(PureCore::DataRef(mInputCache.data().data(), mBlockSize));
        if (err != Success) {
            return err;
        }
        mInputCache.read_pos(mInputCache.read_pos() + mBlockSize);
    }

    size_t idx = 0;
    size_t needSize = mBlockSize - mInputCache.size();
    while (idx + needSize < src.size()) {
        int err = mInputCache.write(PureCore::DataRef(src.data() + idx, needSize));
        if (err != Success) {
            return err;
        }
        err = encode_data(mInputCache.data());
        if (err != Success) {
            return err;
        }
        mInputCache.clear();
        idx += needSize;
        needSize = mBlockSize - mInputCache.size();
    }

    if (idx < src.size()) {
        int err = mInputCache.write(PureCore::DataRef(src.data() + idx, src.size() - idx));
        if (err != Success) {
            return err;
        }
    }
    return Success;
}

int PureSnappyBlockEncoder::flush_encode() {
    if (mInputCache.size() > 0) {
        return encode_data(mInputCache.data());
    }
    mRunning = false;
    return Success;
}

int PureSnappyBlockEncoder::finish_encode() {
    if (mInputCache.size() > 0) {
        int err = encode_data(mInputCache.data());
        if (err != Success) {
            return err;
        }
    }
    clear_hold_output();
    return Success;
}

PureCore::DynamicBuffer &PureSnappyBlockEncoder::get_output() { return mOutput; }

void PureSnappyBlockEncoder::clear() {
    clear_hold_output();
    mOutput.clear();
}

int PureSnappyBlockEncoder::encode_data(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorSnappyDataError;
    }

    size_t maxLen = snappy_max_compressed_length(src.size());
    int err = mOutput.ensure_buffer(mOutput.write_pos() + maxLen + sizeof(uint32_t));
    if (err != Success) {
        return err;
    }
    PureCore::DataRef buffer = mOutput.free_buffer();
    size_t len = maxLen;
    if (snappy_compress(src.data(), src.size(), buffer.data() + sizeof(uint32_t), &len) != SNAPPY_OK) {
        return ErrorSnappyEncodeFailed;
    }
    PureCore::u32_to_four_char(uint32_t(len), buffer.data());
    mOutput.write_pos(mOutput.write_pos() + len + sizeof(uint32_t));
    return Success;
}

void PureSnappyBlockEncoder::clear_hold_output() { mInputCache.clear(); }

bool PureSnappyBlockEncoder::is_running() const { return mRunning; }

/////////////////////////////////////////////////////////////////
/// PureSnappyBlockDecoder
///////////////////////////////////////////////////////////////
PureSnappyBlockDecoder::PureSnappyBlockDecoder() : mBlockSize(0), mNeedSize(0), mDecodeStep(DecodeStepHeader), mInputCache(), mOutput() {}

PureSnappyBlockDecoder::~PureSnappyBlockDecoder() { clear(); }

int PureSnappyBlockDecoder::decode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorSnappyDataError;
    }
    int err = start_decode();
    if (err != Success) {
        return err;
    }
    err = update_decode(src);
    if (err != Success) {
        return err;
    }
    return finish_decode();
}

int PureSnappyBlockDecoder::start_decode() {
    clear();
    mNeedSize = sSnappyBlockHeaderSize;
    mDecodeStep = DecodeStepHeader;
    return mInputCache.resize_buffer(sSnappyBlockHeaderSize);
}

int PureSnappyBlockDecoder::update_decode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorSnappyDataError;
    }

    while (mInputCache.size() >= mNeedSize) {
        PureCore::DataRef data(mInputCache.data().data(), mNeedSize);
        int err = decode_data(data);
        if (err != Success) {
            return err;
        }
        mInputCache.read_pos(mInputCache.read_pos() + data.size());
    }

    size_t idx = 0;
    size_t needSize = mNeedSize - mInputCache.size();
    while (idx + needSize <= src.size()) {
        int err = mInputCache.write(PureCore::DataRef(src.data() + idx, needSize));
        if (err != Success) {
            return err;
        }
        err = decode_data(mInputCache.data());
        if (err != Success) {
            return err;
        }
        mInputCache.clear();
        idx += needSize;
        needSize = mNeedSize - mInputCache.size();
    }

    if (idx < src.size()) {
        int err = mInputCache.write(PureCore::DataRef(src.data() + idx, src.size() - idx));
        if (err != Success) {
            return err;
        }
    }
    return Success;
}

int PureSnappyBlockDecoder::finish_decode() {
    if (mDecodeStep != DecodeStepLen) {
        return ErrorSnappyStepError;
    }
    clear_hold_output();
    return Success;
}

PureCore::DynamicBuffer &PureSnappyBlockDecoder::get_output() { return mOutput; }

void PureSnappyBlockDecoder::clear() {
    clear_hold_output();
    mOutput.clear();
}

int PureSnappyBlockDecoder::decode_data(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorSnappyDataError;
    }
    switch (mDecodeStep) {
        case DecodeStepHeader: {
            if (src.size() != sSnappyBlockHeaderSize) {
                return ErrorSnappyDataError;
            }
            if (memcmp(sSnappyBlockHeader, src.data(), sizeof(sSnappyBlockHeader)) != 0) {
                return ErrorSnappyDataError;
            }
            uint32_t blockSize = PureCore::four_char_to_u32(src.data() + sizeof(sSnappyBlockHeader));
            if (blockSize == 0 || blockSize > sSnappyMaxBlockSize) {
                return ErrorSnappyBlockSizeError;
            }
            mBlockSize = blockSize;
            mNeedSize = sizeof(uint32_t);
            mDecodeStep = DecodeStepLen;
            int err = mInputCache.resize_buffer(mBlockSize);
            if (err != Success) {
                return err;
            }
        } break;
        case DecodeStepLen: {
            if (src.size() != sizeof(uint32_t)) {
                return ErrorSnappyDataError;
            }
            uint32_t dataSize = PureCore::four_char_to_u32(src.data());
            if (dataSize == 0) {
                return ErrorSnappyDataError;
            }
            mNeedSize = dataSize;
            mDecodeStep = DecodeStepData;
        } break;
        case DecodeStepData: {
            if (src.size() != mNeedSize) {
                return ErrorSnappyDataError;
            }
            size_t needLen = 0;
            if (snappy_uncompressed_length(src.data(), src.size(), &needLen) != SNAPPY_OK) {
                return ErrorSnappyDecodeFailed;
            }
            if (mOutput.ensure_buffer(mOutput.write_pos() + needLen) != PureCore::Success) {
                return ErrorCoreBufferFailed;
            }
            size_t len = needLen;
            if (snappy_uncompress(src.data(), src.size(), mOutput.free_buffer().data(), &len) != SNAPPY_OK) {
                return ErrorSnappyEncodeFailed;
            }
            mOutput.write_pos(mOutput.write_pos() + len);
            mNeedSize = sizeof(uint32_t);
            mDecodeStep = DecodeStepLen;
        } break;
        default:
            return ErrorSnappyStepError;
    }
    return Success;
}

void PureSnappyBlockDecoder::clear_hold_output() {
    mBlockSize = 0;
    mNeedSize = 0;
    mDecodeStep = DecodeStepHeader;
    mInputCache.clear();
}

}  // namespace PureEncrypt
