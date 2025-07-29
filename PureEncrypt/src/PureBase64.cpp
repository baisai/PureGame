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
#include "PureEncrypt/PureBase64.h"

namespace PureEncrypt {
static const size_t sRawBase64Block = 3;
static const size_t sBase64Block = 4;
static size_t get_base64_size(size_t rawSize) { return (rawSize + sRawBase64Block - 1) / sRawBase64Block * sBase64Block; }
static size_t get_raw_size(size_t base64Size) { return base64Size / sBase64Block * sRawBase64Block; }
static bool is_base64_char(char c) { return (isalnum(c) || (c == '+') || (c == '/')); }
static const char sBase64Pad = '=';
static const char sBase64First = '+';
static const char sBase64Last = 'z';
static const char sEnBase64Code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const unsigned char sDeBase64Code[] = {
    /* nul, soh, stx, etx, eot, enq, ack, bel, */
    255, 255, 255, 255, 255, 255, 255, 255,
    /*  bs,  ht,  nl,  vt,  np,  cr,  so,  si, */
    255, 255, 255, 255, 255, 255, 255, 255,
    /* dle, dc1, dc2, dc3, dc4, nak, syn, etb, */
    255, 255, 255, 255, 255, 255, 255, 255,
    /* can,  em, sub, esc,  fs,  gs,  rs,  us, */
    255, 255, 255, 255, 255, 255, 255, 255,
    /*  sp, '!', '"', '#', '$', '%', '&', ''', */
    255, 255, 255, 255, 255, 255, 255, 255,
    /* '(', ')', '*', '+', ',', '-', '.', '/', */
    255, 255, 255, 62, 255, 255, 255, 63,
    /* '0', '1', '2', '3', '4', '5', '6', '7', */
    52, 53, 54, 55, 56, 57, 58, 59,
    /* '8', '9', ':', ';', '<', '=', '>', '?', */
    60, 61, 255, 255, 255, 255, 255, 255,
    /* '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', */
    255, 0, 1, 2, 3, 4, 5, 6,
    /* 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', */
    7, 8, 9, 10, 11, 12, 13, 14,
    /* 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', */
    15, 16, 17, 18, 19, 20, 21, 22,
    /* 'X', 'Y', 'Z', '[', '\', ']', '^', '_', */
    23, 24, 25, 255, 255, 255, 255, 255,
    /* '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', */
    255, 26, 27, 28, 29, 30, 31, 32,
    /* 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', */
    33, 34, 35, 36, 37, 38, 39, 40,
    /* 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', */
    41, 42, 43, 44, 45, 46, 47, 48,
    /* 'x', 'y', 'z', '{', '|', '}', '~', del, */
    49, 50, 51, 255, 255, 255, 255, 255};
static int base64_encode(PureCore::DataRef &buffer, PureCore::DataRef src, size_t &len) {
    if (get_base64_size(src.size()) > buffer.size()) {
        return ErrorBufferNotEnough;
    }
    int s = 0;
    unsigned int j = 0;
    unsigned char c = 0;
    unsigned char l = 0;
    char *out = buffer.data();
    for (unsigned int i = 0; i < src.size(); ++i) {
        c = src[i];
        switch (s) {
            case 0:
                s = 1;
                out[j++] = sEnBase64Code[(c >> 2) & 0x3F];
                break;
            case 1:
                s = 2;
                out[j++] = sEnBase64Code[((l & 0x3) << 4) | ((c >> 4) & 0xF)];
                break;
            case 2:
                s = 0;
                out[j++] = sEnBase64Code[((l & 0xF) << 2) | ((c >> 6) & 0x3)];
                out[j++] = sEnBase64Code[c & 0x3F];
                break;
        }
        l = c;
    }

    switch (s) {
        case 1:
            out[j++] = sEnBase64Code[(l & 0x3) << 4];
            out[j++] = sBase64Pad;
            out[j++] = sBase64Pad;
            break;
        case 2:
            out[j++] = sEnBase64Code[(l & 0xF) << 2];
            out[j++] = sBase64Pad;
            break;
    }
    out[j] = 0;
    len = j;
    return Success;
}
static int base64_decode(PureCore::DataRef &buffer, PureCore::DataRef src, size_t &len) {
    unsigned int j = 0;
    unsigned char c = 0;

    if (uint32_t(src.size()) & 0x3u) {
        return ErrorBase64DataError;
    }
    const char *in = src.data();
    char *out = buffer.data();
    for (unsigned int i = 0; i < src.size(); ++i) {
        if (in[i] == sBase64Pad) {
            break;
        }
        if (in[i] < sBase64First || in[i] > sBase64Last) {
            return 0;
        }

        c = sDeBase64Code[(unsigned char)in[i]];
        if (c == 255) {
            return 0;
        }

        switch (i & 0x3) {
            case 0:
                out[j] = (c << 2) & 0xFF;
                break;
            case 1:
                out[j++] |= (c >> 4) & 0x3;
                out[j] = (c & 0xF) << 4;
                break;
            case 2:
                out[j++] |= (c >> 2) & 0xF;
                out[j] = (c & 0x3) << 6;
                break;
            case 3:
                out[j++] |= c;
                break;
        }
    }
    len = j;
    return Success;
}

PureBase64::PureBase64() : mInputCache(), mOutput() {}

PureBase64::~PureBase64() { clear(); }

int PureBase64::encode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorBase64DataError;
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

int PureBase64::decode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorBase64DataError;
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

int PureBase64::start_encode() {
    clear();
    return Success;
}

int PureBase64::update_encode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorBase64DataError;
    }
    PureCore::DataRef data(src);
    if (mInputCache.size() > 0) {
        if (mInputCache.size() + src.size() < sRawBase64Block) {
            return mInputCache.write(src);
        }
        PureCore::DataRef input(src.data(), sRawBase64Block - mInputCache.size());
        int err = mInputCache.write(input);
        if (err != Success) {
            return err;
        }
        data.reset(src.data() + input.size(), src.size() - input.size());
        err = encode_data(mInputCache.data());
        if (err != Success) {
            return err;
        }
        mInputCache.clear();
    }
    size_t cacheSize = data.size() % sRawBase64Block;
    if (cacheSize > 0) {
        data.reset(data.data(), data.size() - cacheSize);
        int err = mInputCache.write(PureCore::DataRef(data.data() + data.size(), cacheSize));
        if (err != Success) {
            return err;
        }
    }
    if (!data.empty()) {
        return encode_data(data);
    }
    return Success;
}

int PureBase64::finish_encode() {
    if (mInputCache.size() > 0) {
        int err = encode_data(mInputCache.data());
        if (err != Success) {
            return err;
        }
    }
    int err = mOutput.write_char_silent(0, mOutput.write_pos());
    if (err != Success) {
        return err;
    }
    clear_hold_output();
    return Success;
}

int PureBase64::start_decode() {
    clear();
    return Success;
}

int PureBase64::update_decode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorBase64DataError;
    }
    PureCore::DataRef data(src);
    if (mInputCache.size() > 0) {
        if (mInputCache.size() + src.size() < sBase64Block) {
            return mInputCache.write(src);
        }
        PureCore::DataRef input(src.data(), sBase64Block - mInputCache.size());
        int err = mInputCache.write(input);
        if (err != Success) {
            return err;
        }
        data.reset(src.data() + input.size(), src.size() - input.size());
        err = decode_data(mInputCache.data());
        if (err != Success) {
            return err;
        }
        mInputCache.clear();
    }
    size_t cacheSize = data.size() % sBase64Block;
    if (cacheSize > 0) {
        data.reset(data.data(), data.size() - cacheSize);
        int err = mInputCache.write(PureCore::DataRef(data.data() + data.size(), cacheSize));
        if (err != Success) {
            return err;
        }
    }
    if (!data.empty()) {
        return decode_data(data);
    }
    return Success;
}

int PureBase64::finish_decode() {
    if (mInputCache.size() > 0) {
        int err = decode_data(mInputCache.data());
        if (err != Success) {
            return err;
        }
    }
    clear_hold_output();
    return Success;
}

PureCore::DynamicBuffer &PureBase64::get_output() { return mOutput; }

void PureBase64::clear() {
    clear_hold_output();
    mOutput.clear();
}

int PureBase64::encode_data(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorBase64DataError;
    }
    int err = mOutput.ensure_buffer(mOutput.write_pos() + get_base64_size(src.size()));
    if (err != Success) {
        return err;
    }
    PureCore::DataRef buffer = mOutput.free_buffer();
    size_t len = 0;
    if (base64_encode(buffer, src, len) != 0) {
        return ErrorBase64EncodeFailed;
    }
    mOutput.write_pos(mOutput.write_pos() + len);
    return 0;
}

int PureBase64::decode_data(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorBase64DataError;
    }
    int err = mOutput.ensure_buffer(mOutput.write_pos() + get_raw_size(src.size()));
    if (err != Success) {
        return err;
    }
    PureCore::DataRef buffer = mOutput.free_buffer();
    size_t len = 0;
    if (base64_decode(buffer, src, len) != 0) {
        return ErrorBase64DecodeFailed;
    }
    mOutput.write_pos(mOutput.write_pos() + len);
    return 0;
}

void PureBase64::clear_hold_output() { mInputCache.clear(); }
}  // namespace PureEncrypt