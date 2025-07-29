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
#include "PureCore/DataRef.h"
#include "PureCore/OsHelper.h"
#include "PureCore/CoreErrorDesc.h"
#include "PureEncrypt/EncryptErrorDesc.h"
#include "PureEncrypt/PureLz4.h"

namespace PureEncrypt {
/////////////////////////////////////////////////////////////////
/// PureLz4 Block
///////////////////////////////////////////////////////////////
static const char sLz4BlockHeader[2]{'l', char(205)};
static const size_t sLz4BlockHeaderSize = sizeof(sLz4BlockHeader) + sizeof(uint32_t);
static const size_t sLz4DefaultBlockSize = 8 * 1024;
static const size_t sLz4MaxBlockSize = 1024 * 1024 * 4;
static const int16_t sLz4EncodeLevel = 1;

/////////////////////////////////////////////////////////////////
/// PureLz4BlockEncoder
///////////////////////////////////////////////////////////////
PureLz4BlockEncoder::PureLz4BlockEncoder() : mContext(nullptr), mBlockSize(sLz4DefaultBlockSize), mLevel(sLz4EncodeLevel), mInputCache(), mOutput() {}

PureLz4BlockEncoder::~PureLz4BlockEncoder() { clear(); }

int PureLz4BlockEncoder::set_block_size(uint32_t size) {
    if (is_running()) {
        return ErrorLz4StepError;
    }
    mBlockSize = size;
    return Success;
}

uint32_t PureLz4BlockEncoder::get_block_size() const { return mBlockSize; }

int PureLz4BlockEncoder::set_encode_level(int16_t level) {
    if (is_running()) {
        return ErrorLz4StepError;
    }
    mLevel = level;
    return Success;
}

int16_t PureLz4BlockEncoder::get_encode_level() const { return mLevel; }

int PureLz4BlockEncoder::encode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorLz4DataError;
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

int PureLz4BlockEncoder::start_encode() {
    if (mBlockSize == 0 || mBlockSize > sLz4MaxBlockSize) {
        return ErrorLz4BlockSizeError;
    }
    clear();
    mContext = new LZ4_stream_t{};
    int err = mInputCache.resize_buffer(mBlockSize * 4);
    if (err != Success) {
        return err;
    }
    err = mOutput.ensure_buffer(sLz4BlockHeaderSize);
    if (err != Success) {
        return err;
    }
    err = mOutput.write(PureCore::DataRef(sLz4BlockHeader, sizeof(sLz4BlockHeader)));
    if (err != Success) {
        return err;
    }
    PureCore::u32_to_four_char(mBlockSize, mOutput.free_buffer().data());
    mOutput.write_pos(mOutput.write_pos() + sizeof(mBlockSize));
    return Success;
}

int PureLz4BlockEncoder::update_encode(PureCore::DataRef src) {
    if (mContext == nullptr) {
        return ErrorLz4StepError;
    }
    if (src.empty()) {
        return ErrorLz4DataError;
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
        if (mInputCache.free_size() < needSize) {
            mInputCache.align_data();
        }
        int err = mInputCache.write(PureCore::DataRef(src.data() + idx, needSize));
        if (err != PureCore::Success) {
            return ErrorCoreBufferFailed;
        }
        err = encode_data(mInputCache.data());
        if (err != Success) {
            return err;
        }
        mInputCache.read_pos(mInputCache.write_pos());
        idx += needSize;
        needSize = mBlockSize - mInputCache.size();
    }

    if (idx < src.size()) {
        if (mInputCache.free_size() < src.size() - idx) {
            mInputCache.align_data();
        }
        int err = mInputCache.write(PureCore::DataRef(src.data() + idx, src.size() - idx));
        if (err != Success) {
            return err;
        }
    }
    return Success;
}

int PureLz4BlockEncoder::flush_encode() {
    if (mInputCache.size() > 0) {
        return encode_data(mInputCache.data());
    }
    return Success;
}

int PureLz4BlockEncoder::finish_encode() {
    if (mContext == nullptr) {
        return ErrorLz4StepError;
    }
    if (mInputCache.size() > 0) {
        int err = encode_data(mInputCache.data());
        if (err != Success) {
            return err;
        }
    }
    clear_hold_output();
    return Success;
}

PureCore::DynamicBuffer& PureLz4BlockEncoder::get_output() { return mOutput; }

void PureLz4BlockEncoder::clear() {
    clear_hold_output();
    mOutput.clear();
}

int PureLz4BlockEncoder::encode_data(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorLz4DataError;
    }
    size_t boundSize = LZ4_COMPRESSBOUND(src.size());
    int err = mOutput.ensure_buffer(mOutput.write_pos() + boundSize + sizeof(uint32_t));
    if (err != Success) {
        return err;
    }
    auto destBuffer = mOutput.free_buffer();
    int realSize = LZ4_compress_fast_continue(mContext, src.data(), destBuffer.data() + sizeof(uint32_t), (int)src.size(),
                                              (int)destBuffer.size() - sizeof(uint32_t), mLevel);
    if (realSize <= 0) {
        return ErrorLz4EncodeFailed;
    }
    PureCore::u32_to_four_char((uint32_t)realSize, destBuffer.data());
    mOutput.write_pos(mOutput.write_pos() + sizeof(uint32_t) + realSize);

    return Success;
}

void PureLz4BlockEncoder::clear_hold_output() {
    if (mContext != nullptr) {
        delete mContext;
        mContext = nullptr;
    }
    mInputCache.clear();
}

bool PureLz4BlockEncoder::is_running() const { return mContext != nullptr; }

/////////////////////////////////////////////////////////////////
/// PureLz4BlockDecoder
///////////////////////////////////////////////////////////////
enum EPureLz4BlockDecodeStep {
    DecodeStepHeader = 0,
    DecodeStepLen = 1,
    DecodeStepData = 2,
};
PureLz4BlockDecoder::PureLz4BlockDecoder()
    : mContext(nullptr), mBlockSize(0), mNeedSize(0), mDecodeStep(DecodeStepHeader), mInputCache(), mOutputCache(), mOutput() {}

PureLz4BlockDecoder::~PureLz4BlockDecoder() { clear(); }

int PureLz4BlockDecoder::decode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorLz4DataError;
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

int PureLz4BlockDecoder::start_decode() {
    clear();
    mContext = new LZ4_streamDecode_t{{0}};
    mNeedSize = sLz4BlockHeaderSize;
    mDecodeStep = DecodeStepHeader;
    return mInputCache.resize_buffer(sLz4BlockHeaderSize);
}

int PureLz4BlockDecoder::update_decode(PureCore::DataRef src) {
    if (mContext == nullptr) {
        return ErrorLz4StepError;
    }
    if (src.empty()) {
        return ErrorLz4DataError;
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
        mInputCache.align_data();
        int err = mInputCache.write(PureCore::DataRef(src.data() + idx, needSize));
        if (err != Success) {
            return err;
        }
        PureCore::DataRef data(mInputCache.data().data(), mNeedSize);
        err = decode_data(data);
        if (err != Success) {
            return err;
        }
        mInputCache.read_pos(mInputCache.read_pos() + data.size());
        idx += needSize;
        needSize = mNeedSize - mInputCache.size();
    }

    if (idx < src.size()) {
        mInputCache.align_data();
        int err = mInputCache.write(PureCore::DataRef(src.data() + idx, src.size() - idx));
        if (err != Success) {
            return err;
        }
    }
    return Success;
}

int PureLz4BlockDecoder::finish_decode() {
    if (mContext == nullptr || mDecodeStep != DecodeStepLen) {
        return ErrorLz4StepError;
    }
    clear_hold_output();
    return Success;
}

PureCore::DynamicBuffer& PureLz4BlockDecoder::get_output() { return mOutput; }

void PureLz4BlockDecoder::clear() {
    clear_hold_output();
    mOutput.clear();
}

int PureLz4BlockDecoder::decode_data(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorLz4DataError;
    }
    switch (mDecodeStep) {
        case DecodeStepHeader: {
            if (src.size() != sLz4BlockHeaderSize) {
                return ErrorLz4DataError;
            }
            if (memcmp(sLz4BlockHeader, src.data(), sizeof(sLz4BlockHeader)) != 0) {
                return ErrorLz4DataError;
            }
            uint32_t blockSize = PureCore::four_char_to_u32(src.data() + sizeof(sLz4BlockHeader));
            if (blockSize == 0 || blockSize > sLz4MaxBlockSize) {
                return ErrorLz4BlockSizeError;
            }
            mBlockSize = blockSize;
            mNeedSize = sizeof(uint32_t);
            mDecodeStep = DecodeStepLen;
            size_t boundSize = LZ4_COMPRESSBOUND(mBlockSize);
            int err = mInputCache.resize_buffer(boundSize + sLz4BlockHeaderSize);
            if (err != Success) {
                return err;
            }
            err = mOutputCache.ensure_buffer(LZ4_decoderRingBufferSize(mBlockSize));
            if (err != Success) {
                return err;
            }
        } break;
        case DecodeStepLen: {
            if (src.size() != sizeof(uint32_t)) {
                return ErrorLz4DataError;
            }
            uint32_t dataSize = PureCore::four_char_to_u32(src.data());
            if (dataSize == 0) {
                return ErrorLz4DataError;
            }
            mNeedSize = dataSize;
            mDecodeStep = DecodeStepData;
        } break;
        case DecodeStepData: {
            if (src.size() != mNeedSize) {
                return ErrorLz4DataError;
            }
            if (mOutputCache.free_size() < mBlockSize) {
                mOutputCache.align_data();
            }
            auto destBuffer = mOutputCache.free_buffer();
            int realSize = LZ4_decompress_safe_continue(mContext, src.data(), destBuffer.data(), (int)src.size(), (int)destBuffer.size());
            if (realSize <= 0) {
                return ErrorLz4DecodeFailed;
            }
            mOutputCache.write_pos(mOutputCache.write_pos() + realSize);
            int err = mOutput.write(mOutputCache.data());
            if (err != Success) {
                return err;
            }
            mOutputCache.read_pos(mOutputCache.read_pos() + mOutputCache.size());
            mNeedSize = sizeof(uint32_t);
            mDecodeStep = DecodeStepLen;
        } break;
        default:
            return ErrorLz4StepError;
    }
    return Success;
}

void PureLz4BlockDecoder::clear_hold_output() {
    if (mContext != nullptr) {
        delete mContext;
        mContext = nullptr;
    }
    mBlockSize = 0;
    mNeedSize = 0;
    mDecodeStep = DecodeStepHeader;
    mInputCache.clear();
    mOutputCache.clear();
}

/////////////////////////////////////////////////////////////////
/// PureLz4FrameEncoder
///////////////////////////////////////////////////////////////
static const LZ4F_preferences_t sDefaultPreference = {
    {LZ4F_max256KB, LZ4F_blockLinked, LZ4F_noContentChecksum, LZ4F_frame, 0 /* unknown content size */, 0 /* no dictID */, LZ4F_noBlockChecksum},
    0,         /* compression level; 0 == default */
    0,         /* autoflush */
    0,         /* favor decompression speed */
    {0, 0, 0}, /* reserved, must be set to 0 */
};
PureLz4FrameEncoder::PureLz4FrameEncoder() : mContext(nullptr), mPreference(sDefaultPreference), mOutput(), mLz4Err() {}

PureLz4FrameEncoder::~PureLz4FrameEncoder() { clear(); }

int PureLz4FrameEncoder::set_preference(const LZ4F_preferences_t& preference) {
    if (is_running()) {
        return ErrorLz4StepError;
    }
    mPreference = preference;
    return Success;
}

int PureLz4FrameEncoder::encode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorLz4DataError;
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

int PureLz4FrameEncoder::start_encode() {
    clear();
    size_t lz4Err = LZ4F_createCompressionContext(&mContext, LZ4F_VERSION);
    if (LZ4F_isError(lz4Err)) {
        mLz4Err = LZ4F_getErrorName(lz4Err);
        return ErrorLz4EncodeFailed;
    }
    int err = mOutput.ensure_buffer(mOutput.write_pos() + LZ4F_HEADER_SIZE_MAX);
    if (err != Success) {
        return err;
    }
    size_t headSize = LZ4F_compressBegin(mContext, mOutput.free_buffer().data(), LZ4F_HEADER_SIZE_MAX, &mPreference);
    if (LZ4F_isError(headSize)) {
        mLz4Err = LZ4F_getErrorName(headSize);
        return ErrorLz4EncodeFailed;
    }
    mOutput.write_pos(mOutput.write_pos() + headSize);
    return Success;
}

int PureLz4FrameEncoder::update_encode(PureCore::DataRef src) {
    if (mContext == nullptr) {
        return ErrorLz4StepError;
    }
    if (src.empty()) {
        return ErrorLz4DataError;
    }
    size_t boundSize = LZ4F_compressBound(src.size(), &mPreference);
    int err = mOutput.ensure_buffer(mOutput.write_pos() + boundSize);
    if (err != Success) {
        return err;
    }
    size_t realSize = LZ4F_compressUpdate(mContext, mOutput.free_buffer().data(), boundSize, src.data(), src.size(), nullptr);
    if (LZ4F_isError(realSize)) {
        mLz4Err = LZ4F_getErrorName(realSize);
        return ErrorLz4EncodeFailed;
    }
    mOutput.write_pos(mOutput.write_pos() + realSize);
    return Success;
}

int PureLz4FrameEncoder::finish_encode() {
    if (mContext == nullptr) {
        return ErrorLz4StepError;
    }

    size_t boundSize = LZ4F_compressBound(0, &mPreference);
    int err = mOutput.ensure_buffer(mOutput.write_pos() + boundSize);
    if (err != Success) {
        return err;
    }

    size_t endSize = LZ4F_compressEnd(mContext, mOutput.free_buffer().data(), boundSize, nullptr);
    if (LZ4F_isError(endSize)) {
        mLz4Err = LZ4F_getErrorName(endSize);
        return ErrorLz4EncodeFailed;
    }
    mOutput.write_pos(mOutput.write_pos() + endSize);
    clear_hold_output();
    return Success;
}

PureCore::DynamicBuffer& PureLz4FrameEncoder::get_output() { return mOutput; }

const std::string& PureLz4FrameEncoder::get_lz4_error() const { return mLz4Err; }

void PureLz4FrameEncoder::clear() {
    clear_hold_output();
    mOutput.clear();
    mLz4Err.clear();
}

void PureLz4FrameEncoder::clear_hold_output() {
    if (mContext != nullptr) {
        LZ4F_freeCompressionContext(mContext);
        mContext = nullptr;
    }
}

bool PureLz4FrameEncoder::is_running() const { return mContext != nullptr; }

/////////////////////////////////////////////////////////////////
/// PureLz4FrameDecoder
///////////////////////////////////////////////////////////////
PureLz4FrameDecoder::PureLz4FrameDecoder() : mContext(nullptr), mBlockSize(0), mNeedSize(0), mInputCache(), mOutput(), mLz4Err() {}

PureLz4FrameDecoder::~PureLz4FrameDecoder() { clear(); }

int PureLz4FrameDecoder::decode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorLz4DataError;
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

int PureLz4FrameDecoder::start_decode() {
    clear();
    size_t lz4Err = LZ4F_createDecompressionContext(&mContext, LZ4F_VERSION);
    if (LZ4F_isError(lz4Err)) {
        mLz4Err = LZ4F_getErrorName(lz4Err);
        return ErrorLz4DecodeFailed;
    }
    mNeedSize = LZ4F_HEADER_SIZE_MAX;
    return Success;
}

int PureLz4FrameDecoder::update_decode(PureCore::DataRef src) {
    if (mContext == nullptr) {
        return ErrorLz4StepError;
    }
    if (src.empty()) {
        return ErrorLz4DataError;
    }

    size_t costSize = 0;
    size_t idx = 0;
    do {
        size_t realNeedSize = 0;
        if (mInputCache.size() < mNeedSize) {
            realNeedSize = mNeedSize - mInputCache.size();
        }
        if (mNeedSize == 0 || realNeedSize >= src.size() - idx) {
            int err = mInputCache.write(PureCore::DataRef(src.data() + idx, src.size() - idx));
            if (err != Success) {
                return err;
            }
            idx = src.size();
        } else if (realNeedSize > 0) {
            int err = mInputCache.write(PureCore::DataRef(src.data() + idx, realNeedSize));
            if (err != Success) {
                return err;
            }
            idx += realNeedSize;
        }

        costSize = 0;
        int err = decode_data(mInputCache.data(), &costSize);
        if (err != Success) {
            return err;
        }
        if (mInputCache.size() == costSize) {
            mInputCache.clear();
        } else {
            mInputCache.read_pos(mInputCache.read_pos() + costSize);
        }
    } while (idx < src.size());
    return Success;
}

int PureLz4FrameDecoder::finish_decode() {
    if (mContext == nullptr) {
        return ErrorLz4StepError;
    }
    if (mInputCache.size() > 0) {
        size_t costSize = 0;
        int err = decode_data(mInputCache.data(), &costSize);
        if (err != Success) {
            return err;
        }
        if (costSize != mInputCache.size()) {
            return ErrorLz4DecodeFailed;
        }
    }

    clear_hold_output();
    return Success;
}

PureCore::DynamicBuffer& PureLz4FrameDecoder::get_output() { return mOutput; }

const std::string& PureLz4FrameDecoder::get_lz4_error() const { return mLz4Err; }

void PureLz4FrameDecoder::clear() {
    clear_hold_output();
    mOutput.clear();
    mLz4Err.clear();
}

int PureLz4FrameDecoder::decode_data(PureCore::DataRef src, size_t* costSize) {
    *costSize = src.size();
    if (mBlockSize == 0) {
        LZ4F_frameInfo_t info;
        size_t needSize = LZ4F_getFrameInfo(mContext, &info, src.data(), costSize);
        if (LZ4F_isError(needSize)) {
            mLz4Err = LZ4F_getErrorName(needSize);
            return ErrorLz4DecodeFailed;
        }
        switch (info.blockSizeID) {
            case LZ4F_default:
            case LZ4F_max64KB:
                mBlockSize = 1 << 16;
                break;
            case LZ4F_max256KB:
                mBlockSize = 1 << 18;
                break;
            case LZ4F_max1MB:
                mBlockSize = 1 << 20;
                break;
            case LZ4F_max4MB:
                mBlockSize = 1 << 22;
                break;
            default:
                return ErrorLz4DataError;
        }
        mNeedSize = (uint32_t)needSize;
    } else {
        int err = mOutput.ensure_buffer(mOutput.write_pos() + mBlockSize);
        if (err != Success) {
            return err;
        }
        size_t realSize = mBlockSize;
        size_t needSize = LZ4F_decompress(mContext, mOutput.free_buffer().data(), &realSize, src.data(), costSize, nullptr);
        if (LZ4F_isError(needSize)) {
            mLz4Err = LZ4F_getErrorName(needSize);
            return ErrorLz4DecodeFailed;
        }
        mOutput.write_pos(mOutput.write_pos() + realSize);
        mNeedSize = (uint32_t)needSize;
    }
    return Success;
}

void PureLz4FrameDecoder::clear_hold_output() {
    if (mContext != nullptr) {
        LZ4F_freeDecompressionContext(mContext);
        mContext = nullptr;
    }
    mBlockSize = 0;
    mNeedSize = 0;
    mInputCache.clear();
}

}  // namespace PureEncrypt
