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

namespace PureEncrypt {
class PUREENCRYPT_API PureCrc32 {
public:
    PureCrc32() : mCrc(0) {}
    ~PureCrc32() {}

    static uint32_t encode(PureCore::DataRef src, uint32_t key);

    void start_encode(uint32_t key);
    void update_encode(PureCore::DataRef src);
    uint32_t finish_encode() const;

private:
    uint32_t mCrc;
    static const uint32_t sCrc32Table[256];

    PURE_DISABLE_COPY(PureCrc32)
};

///////////////////////////////////////////////////////////////////////
// inline method
//////////////////////////////////////////////////////////////////////
inline void PureCrc32::start_encode(uint32_t key) { mCrc = key; }

inline uint32_t PureCrc32::finish_encode() const { return mCrc; }
}  // namespace PureEncrypt
