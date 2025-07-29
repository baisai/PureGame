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

#include "PureEncrypt/PureEncryptLib.h"

#include <string>

#define PUREENCRYPT_ERROR_MAP(XX)                            \
    XX(ErrorNullPointer, "The Pointer Is Null")              \
    XX(ErrorBufferNotEnough, "The Buffer Is Not Enough")     \
    XX(ErrorCoreBufferFailed, "Core Buffer Operator Failed") \
    XX(ErrorNotSupport, "Not Support Operator")              \
    XX(ErrorPathNotExist, "Path Not Exist")                  \
    XX(ErrorFileNotExist, "File Not Exist")                  \
    XX(ErrorDataInvalid, "Data Ivalid")                      \
    XX(ErrorSnappyEncodeFailed, "Snappy Encode Failed")      \
    XX(ErrorSnappyDecodeFailed, "Snappy Decode Failed")      \
    XX(ErrorSnappyStepError, "Snappy Step Error")            \
    XX(ErrorSnappyBlockSizeError, "Snappy Block Size Error") \
    XX(ErrorSnappyDataError, "Snappy Data Error")            \
    XX(ErrorLz4DataError, "Lz4 Data Error")                  \
    XX(ErrorLz4EncodeFailed, "Lz4 Encode Failed")            \
    XX(ErrorLz4DecodeFailed, "Lz4 Decode Failed")            \
    XX(ErrorLz4StepError, "Lz4 Step Error")                  \
    XX(ErrorLz4BlockSizeError, "Lz4 Block Size Error")       \
    XX(ErrorZipContextError, "Zip Context Error")            \
    XX(ErrorZipOpenFailed, "Zip Open Failed")                \
    XX(ErrorZipSrcNotExist, "Zip Src Not Exist")             \
    XX(ErrorZipEntryFailed, "Zip Entry Failed")              \
    XX(ErrorZipDataError, "Zip Data Error")                  \
    XX(ErrorZipEncodeFailed, "Zip Encode Failed")            \
    XX(ErrorZipDecodeFailed, "Zip Decode Failed")            \
    XX(ErrorZipStepError, "Zip Step Error")                  \
    XX(ErrorZipBlockSizeError, "Zip Block Size Error")       \
    XX(ErrorXxteaKeyError, "Xxtea Key Error")                \
    XX(ErrorXxteaEncodeFailed, "Xxtea Encode Failed")        \
    XX(ErrorXxteaDecodeFailed, "Xxtea Decode Failed")        \
    XX(ErrorXxteaStepError, "Xxtea Step Error")              \
    XX(ErrorXxteaBlockSizeError, "Xxtea Block Size Error")   \
    XX(ErrorXxteaDataError, "Xxtea Data Error")              \
    XX(ErrorBase64DataError, "Base64 Data Error")            \
    XX(ErrorBase64EncodeFailed, "Base64 Encode Failed")      \
    XX(ErrorBase64DecodeFailed, "Base64 Decode Failed")      \
    XX(ErrorMD5DataError, "MD5 Data Error")                  \
    XX(ErrorMD5EncodeFailed, "MD5 Encode Failed")            \
    XX(ErrorMD5DecodeFailed, "MD5 Decode Failed")            \
    XX(ErrorSha1DataError, "Sha1 Data Error")                \
    XX(ErrorSha1EncodeFailed, "Sha1 Encode Failed")          \
    XX(ErrorSha1DecodeFailed, "Sha1 Decode Failed")          \
    XX(ErrorSha256DataError, "Sha256 Data Error")            \
    XX(ErrorSha256EncodeFailed, "Sha256 Encode Failed")      \
    XX(ErrorSha256DecodeFailed, "Sha256 Decode Failed")      \
    XX(ErrorSha512DataError, "Sha512 Data Error")            \
    XX(ErrorSha512EncodeFailed, "Sha512 Encode Failed")      \
    XX(ErrorSha512DecodeFailed, "Sha512 Decode Failed")

namespace PureEncrypt {
enum EPureEncryptErrorCode {
    Success = 0,
#ifdef PUREENCRYPT_ERRORCODE_BEGIN
    __PureEncryptErrorCodeSeparate = PUREENCRYPT_ERRORCODE_BEGIN - 1,
#endif
#define XX(code, desc) code,
    PUREENCRYPT_ERROR_MAP(XX)
#undef XX
};

PUREENCRYPT_API std::string get_error_desc(int err);
}  // namespace PureEncrypt
