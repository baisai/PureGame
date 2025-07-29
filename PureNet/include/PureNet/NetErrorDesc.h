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

#include "PureNet/PureNetLib.h"

#include "uv.h"

#include <string>

#define PURENET_ERROR_MAP(XX)                                    \
    XX(ErrorCoreBufferFailed, "Core Buffer Operator Failed")     \
    XX(ErrorNullPointer, "The Pointer Is Null")                  \
    XX(ErrorInvalidArg, "The Arg Is Invalid")                    \
    XX(ErrorStateError, "The State Is Error")                    \
    XX(ErrorInvalidSockType, "The Sock Type Is Invalid")         \
    XX(ErrorInvalidHandle, "The Handle Is Invalid")              \
    XX(ErrrorMemoryNotEnough, "The Memory Is Not Enough")        \
    XX(ErrorLinkIDInvalid, "The LinkID Is Invalid")              \
    XX(ErrorLinkKeepAliveFailed, "The Link Keep Alive Failed")   \
    XX(ErrorLinkKeepAliveTimeout, "The Link Keep Alive Timeout") \
    XX(ErrorSockAddrInvalid, "The Sock Address Is Invalid")      \
    XX(ErrorNotSupport, "The Operator Is Not Support")           \
    XX(ErrorNotFoundLink, "The Link Is Not Exists")              \
    XX(ErrorUdpGetPortFailed, "Udp Get Port Failed")             \
    XX(ErrorUdpAlreadyHandshake, "Udp Is Already Handshake")     \
    XX(ErrorUdpHandshakeFailed, "Udp Handshake failed")          \
    XX(ErrorUdpNotHandshake, "Udp Not Handshake")                \
    XX(ErrorUdpHandshakeTimeout, "Udp Handshake Timeout")        \
    XX(ErrorUdpDataInvalid, "Udp Data Is Invalid")               \
    XX(ErrorUdpRemoteClosed, "Udp Remote Closed")                \
    XX(ErrorLinkRouteExist, "The Link Route Alreay Exist")       \
    XX(ErrorNotFoundLinkRoute, "Not Found The Link Route")       \
    XX(ErrorPackMsgFailed, "Pack Msg Failed")                    \
    XX(ErrorUnpackMsgFailed, "Unpack Msg Failed")                \
    XX(ErrorLinkWriteDataFailed, "Link Write Data Failed")       \
    XX(ErrorLinkProtocolFull, "Link Protocol Is Full")           \
    XX(ErrorLinkNoneProtocol, "Link Have None Protocol")         \
    XX(ErrorProtocolDataInvalid, "The Protocol Data Is Invalid") \
    XX(ErrorMsgBodySizeMax, "The Msg Body Size Max")             \
    XX(ErrorAddNetReqFailed, "Add Net Req Failed")               \
    XX(ErrorHttpParseFailed, "Parse Http failed")                \
    XX(ErrorSSLContextInvalid, "SSL Context Invalid")            \
    XX(ErrorSSLSetupFailed, "SSL Setup Failed")                  \
    XX(ErrorSSLCaFailed, "SSL Use Ca Failed")                    \
    XX(ErrorSSLCertFailed, "SSL Use Cert Failed")                \
    XX(ErrorSSLHandshakeFailed, "SSL Handshake Failed")          \
    XX(ErrorSSLNotHandshake, "SSL Not Handshake")                \
    XX(ErrorSSLWriteFailed, "SSL Write Failed")                  \
    XX(ErrorSSLReadFailed, "SSL Read Failed")                    \
    XX(ErrorWSHandshakeFailed, "Web Socket Handshake Failed")    \
    XX(ErrorWSNotHandshake, "Web Socket Not Handshake")          \
    XX(ErrorInvalidUrl, "Url Is Invalid")

namespace PureNet {
enum EPureNetErrorCode {
    Success = 0,
#ifdef PURENET_ERRORCODE_BEGIN
    __PureNetErrorCodeSeparate = PURENET_ERRORCODE_BEGIN - 1,
#endif
#define XX(code, desc) code,
    PURENET_ERROR_MAP(XX)
#undef XX
#define XX(code, str) ErrorUV##code = UV_##code,
        UV_ERRNO_MAP(XX)
#undef XX
};

PURENET_API std::string get_error_desc(int err);
}  // namespace PureNet
