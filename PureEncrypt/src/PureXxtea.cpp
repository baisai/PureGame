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
#include "PureEncrypt/PureXxtea.h"

#define XXTEA_INTKEY_LENGTH 4
#define XXTEA_DELTA 0x9e3779b9
#define XXTEA_MX ((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (key[p & 3 ^ e] ^ z))

namespace PureEncrypt {
/////////////////////////////////////////////////////////////////
/// tool func
///////////////////////////////////////////////////////////////
static void xxtea_encode(int32_t *data, size_t len, int32_t key[XXTEA_INTKEY_LENGTH]) {
    if (len <= 1) {
        return;
    }
    int32_t n = int32_t(len) - 1;
    int32_t p = 0;
    int32_t z = data[n], y = data[0], q = 6 + 52 / int32_t(len), sum = 0, e;
    while (0 < q--) {
        sum += XXTEA_DELTA;
        e = (sum >> 2) & 3;
        for (p = 0; p < n; p++) {
            y = data[p + 1];
            z = data[p] += XXTEA_MX;
        }
        y = data[0];
        z = data[n] += XXTEA_MX;
    }
}

static void xxtea_decode(int32_t *data, size_t len, int32_t key[XXTEA_INTKEY_LENGTH]) {
    if (len <= 1) {
        return;
    }
    int32_t n = int32_t(len) - 1;
    int32_t p = n;
    int32_t z = data[n], y = data[0], q = 6 + 52 / int32_t(len), sum = q * XXTEA_DELTA, e;
    while (sum != 0) {
        e = (sum >> 2) & 3;
        for (p = n; p > 0; p--) {
            z = data[p - 1];
            y = data[p] -= XXTEA_MX;
        }
        z = data[n];
        y = data[0] -= XXTEA_MX;
        sum -= XXTEA_DELTA;
    }
}

/////////////////////////////////////////////////////////////////
/// PureXxtea
///////////////////////////////////////////////////////////////
PureXxtea::PureXxtea() : mOutput() {}

PureXxtea::~PureXxtea() { mOutput.clear(); }

int PureXxtea::encode(PureCore::DataRef src, PureCore::StringRef key) {
    if (key.size() != XXTEA_KEY_LENGTH) {
        return ErrorXxteaKeyError;
    }

    int32_t *pPass = (int32_t *)key.data();
#if BYTE_ORDER == BIG_ENDIAN
    int32_t pass[XXTEA_INTKEY_LENGTH]{0};
    for (size_t i = 0; i < XXTEA_INTKEY_LENGTH; ++i) {
        _pure_byte_load32(int32_t, &key[i * sizeof(int32_t)], &pass[i]);
    }
    pPass = pass;
#endif

    mOutput.clear();
    if (src.size() == 0) {
        return Success;
    }
    PureCore::DataRef curData(src);
    size_t leftSize = src.size() % sizeof(int32_t);
    char alignData[sizeof(int32_t)] = {0};
    if (leftSize != 0) {
        memcpy(alignData, src.data() + src.size() - leftSize, leftSize);
        int align = int(sizeof(int32_t) - leftSize);
        memset(alignData + leftSize, align, align);
        curData.reset(src.data(), src.size() - leftSize);
    } else {
        memset(alignData, sizeof(int32_t), sizeof(alignData));
    }
    int err = mOutput.write_silent(curData);
    if (err != Success) {
        return err;
    }
    PureCore::DataRef buffer = mOutput.free_buffer();
    size_t count = curData.size() / sizeof(int32_t);
#if BYTE_ORDER == BIG_ENDIAN
    for (size_t i = 0; i < count; ++i) {
        _pure_byte_load32(int32_t, &buffer[i * sizeof(int32_t)], (int32_t *)&buffer[i * sizeof(int32_t)]);
    }
    xxtea_encode((int32_t *)buffer.data(), count, pPass);
    for (size_t i = 0; i < count; ++i) {
        _pure_byte_store32(&buffer[i * sizeof(int32_t)], *(int32_t *)&buffer[i * sizeof(int32_t)]);
    }
#else
    xxtea_encode((int32_t *)buffer.data(), count, pPass);
#endif
    mOutput.write_pos(mOutput.write_pos() + curData.size());
    return mOutput.write(PureCore::DataRef(alignData, sizeof(alignData)));
}

int PureXxtea::decode(PureCore::DataRef src, PureCore::StringRef key) {
    if (key.size() != XXTEA_KEY_LENGTH) {
        return ErrorXxteaKeyError;
    }

    int32_t *pPass = (int32_t *)key.data();
#if BYTE_ORDER == BIG_ENDIAN
    int32_t pass[XXTEA_INTKEY_LENGTH]{0};
    for (size_t i = 0; i < XXTEA_INTKEY_LENGTH; ++i) {
        _pure_byte_load32(int32_t, &key[i * sizeof(int32_t)], &pass[i]);
    }
    pPass = pass;
#endif
    if (src.size() % sizeof(int32_t) != 0) {
        return ErrorXxteaDecodeFailed;
    }
    mOutput.clear();
    if (src.size() <= sizeof(int32_t)) {
        return mOutput.write(src);
    }

    PureCore::DataRef alignData(src.data() + src.size() - sizeof(int32_t), sizeof(int32_t));
    PureCore::DataRef curData(src.data(), src.size() - sizeof(int32_t));
    int err = mOutput.write_silent(curData);
    if (err != Success) {
        return err;
    }
    PureCore::DataRef buffer = mOutput.free_buffer();
    mOutput.write_pos(mOutput.write_pos() + curData.size());
    size_t count = curData.size() / sizeof(int32_t);
#if BYTE_ORDER == BIG_ENDIAN
    for (size_t i = 0; i < count; ++i) {
        _pure_byte_load32(int32_t, &buffer[i * sizeof(int32_t)], (int32_t *)&buffer[i * sizeof(int32_t)]);
    }
    xxtea_decode((int32_t *)buffer.data(), count, pPass);
    for (size_t i = 0; i < count; ++i) {
        _pure_byte_store32(&buffer[i * sizeof(int32_t)], *(((int32_t *)buffer.data()) + i));
    }
#else
    xxtea_decode((int32_t *)buffer.data(), count, pPass);
#endif

    int align = alignData[alignData.size() - 1];
    if (align >= alignData.size()) {
        return Success;
    }
    return mOutput.write(PureCore::DataRef(alignData.data(), alignData.size() - align));
}

PureCore::DynamicBuffer &PureXxtea::get_output() { return mOutput; }

/////////////////////////////////////////////////////////////////
/// PureXxteaBlock
///////////////////////////////////////////////////////////////
static const char sXxteaBlockHeader[2]{'x', char(205)};
static const size_t sXxteaBlockHeaderSize = sizeof(sXxteaBlockHeader) + sizeof(uint32_t);
static const size_t sXxteaDefaultBlockSize = 8 * 1024;
static const size_t sXxteaMaxBlockSize = 1024 * 1024 * 4;

enum EPureXxteaDataDecodeStep {
    DecodeStepHeader = 0,
    DecodeStepLen = 1,
    DecodeStepData = 2,
};
/////////////////////////////////////////////////////////////////
/// PureXxteaBlockEncoder
///////////////////////////////////////////////////////////////
PureXxteaBlockEncoder::PureXxteaBlockEncoder() : mBlockSize(sXxteaDefaultBlockSize), mPass{}, mInputCache(), mOutput() {}

PureXxteaBlockEncoder::~PureXxteaBlockEncoder() { clear(); }

int PureXxteaBlockEncoder::set_block_size(uint32_t size) {
    if (is_running()) {
        return ErrorXxteaStepError;
    }
    mBlockSize = size;
    return Success;
}

uint32_t PureXxteaBlockEncoder::get_block_size() const { return mBlockSize; }

int PureXxteaBlockEncoder::encode(PureCore::DataRef src, PureCore::StringRef key) {
    if (src.empty()) {
        return ErrorXxteaDataError;
    }
    int err = start_encode(key);
    if (err != Success) {
        return err;
    }
    err = update_encode(src);
    if (err != Success) {
        return err;
    }
    return finish_encode();
}

int PureXxteaBlockEncoder::start_encode(PureCore::StringRef key) {
    if (mBlockSize == 0 || mBlockSize > sXxteaMaxBlockSize) {
        return ErrorXxteaBlockSizeError;
    }
    if (key.size() != XXTEA_KEY_LENGTH) {
        return ErrorXxteaKeyError;
    }
    clear();
#if BYTE_ORDER == BIG_ENDIAN
    for (size_t i = 0; i < XXTEA_INTKEY_LENGTH; ++i) {
        _pure_byte_load32(int32_t, &key[i * sizeof(int32_t)], &mPass[i]);
    }
#else
    memcpy(mPass, key.data(), key.size());
#endif
    int err = mInputCache.resize_buffer(mBlockSize);
    if (err != Success) {
        return err;
    }
    err = mOutput.ensure_buffer(sXxteaBlockHeaderSize);
    if (err != Success) {
        return err;
    }
    err = mOutput.write(PureCore::DataRef(sXxteaBlockHeader, sizeof(sXxteaBlockHeader)));
    if (err != Success) {
        return err;
    }
    PureCore::u32_to_four_char(mBlockSize, mOutput.free_buffer().data());
    mOutput.write_pos(mOutput.write_pos() + sizeof(mBlockSize));
    return Success;
}
int PureXxteaBlockEncoder::update_encode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorXxteaDataError;
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

int PureXxteaBlockEncoder::flush_encode() {
    if (mInputCache.size() > 0) {
        return encode_data(mInputCache.data());
    }
    return Success;
}

int PureXxteaBlockEncoder::finish_encode() {
    if (mInputCache.size() > 0) {
        int err = encode_data(mInputCache.data());
        if (err != Success) {
            return err;
        }
    }
    clear_hold_output();
    return Success;
}

PureCore::DynamicBuffer &PureXxteaBlockEncoder::get_output() { return mOutput; }

void PureXxteaBlockEncoder::clear() {
    clear_hold_output();
    mOutput.clear();
}

int PureXxteaBlockEncoder::encode_data(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorXxteaDataError;
    }
    int err = mOutput.ensure_buffer(mOutput.write_pos() + src.size() + sizeof(uint32_t));
    if (err != Success) {
        return err;
    }
    size_t leftSize = src.size() % sizeof(int32_t);
    PureCore::DataRef curData(src.data(), src.size() - leftSize);
    err = mOutput.write_silent(curData);
    if (err != Success) {
        return err;
    }
    PureCore::DataRef buffer = mOutput.free_buffer();
    size_t count = curData.size() / sizeof(int32_t);
#if BYTE_ORDER == BIG_ENDIAN
    for (size_t i = 0; i < count; ++i) {
        _pure_byte_load32(int32_t, &buffer[i * sizeof(int32_t) + sizeof(uint32_t)], (int32_t *)&buffer[i * sizeof(int32_t) + sizeof(uint32_t)]);
    }
    xxtea_encode((int32_t *)(buffer.data() + sizeof(uint32_t)), count, mPass);
    for (size_t i = 0; i < count; ++i) {
        _pure_byte_store32(&buffer[i * sizeof(int32_t) + sizeof(uint32_t)], *(int32_t *)&buffer[i * sizeof(int32_t) + sizeof(uint32_t)]);
    }
#else
    xxtea_encode((int32_t *)(buffer.data() + sizeof(uint32_t)), count, mPass);
#endif
    if (leftSize > 0) {
        memcpy(buffer.data() + curData.size() + sizeof(uint32_t), src.data() + curData.size(), leftSize);
    }

    uint32_t realSize = (uint32_t)src.size();
    PureCore::u32_to_four_char(realSize, buffer.data());
    mOutput.write_pos(mOutput.write_pos() + realSize + sizeof(uint32_t));
    return Success;
}

void PureXxteaBlockEncoder::clear_hold_output() {
    memset(mPass, 0, sizeof(mPass));
    mInputCache.clear();
}

bool PureXxteaBlockEncoder::is_running() const {
    for (size_t i = 0; i < PURE_ARRAY_SIZE(mPass); ++i) {
        if (mPass[i] != 0) {
            return true;
        }
    }
    return false;
}

/////////////////////////////////////////////////////////////////
/// PureXxteaBlockDecoder
///////////////////////////////////////////////////////////////
PureXxteaBlockDecoder::PureXxteaBlockDecoder() : mBlockSize(0), mNeedSize(0), mDecodeStep(DecodeStepHeader), mPass{}, mInputCache(), mOutput() {}

PureXxteaBlockDecoder::~PureXxteaBlockDecoder() { clear(); }

int PureXxteaBlockDecoder::decode(PureCore::DataRef src, PureCore::StringRef key) {
    if (src.empty()) {
        return ErrorXxteaDataError;
    }
    int err = start_decode(key);
    if (err != Success) {
        return err;
    }
    err = update_decode(src);
    if (err != Success) {
        return err;
    }
    return finish_decode();
}

int PureXxteaBlockDecoder::start_decode(PureCore::StringRef key) {
    if (key.size() != XXTEA_KEY_LENGTH) {
        return ErrorXxteaKeyError;
    }
    clear();
#if BYTE_ORDER == BIG_ENDIAN
    for (size_t i = 0; i < XXTEA_INTKEY_LENGTH; ++i) {
        _pure_byte_load32(int32_t, &key[i * sizeof(int32_t)], &mPass[i]);
    }
#else
    memcpy(mPass, key.data(), key.size());
#endif
    mNeedSize = sXxteaBlockHeaderSize;
    mDecodeStep = DecodeStepHeader;
    return mInputCache.resize_buffer(sXxteaBlockHeaderSize);
}

int PureXxteaBlockDecoder::update_decode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorXxteaDataError;
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

int PureXxteaBlockDecoder::finish_decode() {
    if (mDecodeStep != DecodeStepLen) {
        return ErrorXxteaStepError;
    }
    clear_hold_output();
    return Success;
}

PureCore::DynamicBuffer &PureXxteaBlockDecoder::get_output() { return mOutput; }

void PureXxteaBlockDecoder::clear() {
    clear_hold_output();
    mOutput.clear();
}

int PureXxteaBlockDecoder::decode_data(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorXxteaDataError;
    }
    switch (mDecodeStep) {
        case DecodeStepHeader: {
            if (src.size() != sXxteaBlockHeaderSize) {
                return ErrorXxteaDataError;
            }
            if (memcmp(sXxteaBlockHeader, src.data(), sizeof(sXxteaBlockHeader)) != 0) {
                return ErrorXxteaDataError;
            }
            uint32_t blockSize = PureCore::four_char_to_u32(src.data() + sizeof(sXxteaBlockHeader));
            if (blockSize == 0 || blockSize > sXxteaMaxBlockSize) {
                return ErrorXxteaBlockSizeError;
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
                return ErrorXxteaDataError;
            }
            uint32_t dataSize = PureCore::four_char_to_u32(src.data());
            if (dataSize == 0) {
                return ErrorXxteaDataError;
            }
            mNeedSize = dataSize;
            mDecodeStep = DecodeStepData;
        } break;
        case DecodeStepData: {
            if (src.size() != mNeedSize) {
                return ErrorXxteaDataError;
            }
            int err = mOutput.write_silent(src);
            if (err != Success) {
                return err;
            }
            size_t leftSize = src.size() % sizeof(int32_t);
            PureCore::DataRef buffer(mOutput.free_buffer().data(), src.size() - leftSize);

            size_t count = src.size() / sizeof(int32_t);
#if BYTE_ORDER == BIG_ENDIAN
            for (size_t i = 0; i < count; ++i) {
                _pure_byte_load32(int32_t, &buffer[i * sizeof(int32_t)], (int32_t *)&buffer[i * sizeof(int32_t)]);
            }
            xxtea_decode((int32_t *)buffer.data(), count, mPass);
            for (size_t i = 0; i < count; ++i) {
                _pure_byte_store32(&buffer[i * sizeof(int32_t)], *(((int32_t *)buffer.data()) + i));
            }
#else
            xxtea_decode((int32_t *)buffer.data(), count, mPass);
#endif

            mOutput.write_pos(mOutput.write_pos() + src.size());
            mNeedSize = sizeof(uint32_t);
            mDecodeStep = DecodeStepLen;
        } break;
        default:
            return ErrorXxteaStepError;
    }
    return Success;
}

void PureXxteaBlockDecoder::clear_hold_output() {
    mBlockSize = 0;
    mNeedSize = 0;
    mDecodeStep = DecodeStepHeader;
    memset(mPass, 0, sizeof(mPass));
    mInputCache.clear();
}

}  // namespace PureEncrypt

#undef XXTEA_DELTA
#undef XXTEA_MX
#undef XXTEA_INTKEY_LENGTH
