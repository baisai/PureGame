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
#include "PureNet/PureNetReacter.h"

namespace PureNet {
enum ESockType {
    ESockInvalid = 0,
    ESockTcp = 1,
};
enum EAsyncType {
    EAsyncInvalid = 0,
    EAsyncListen = 1,
    EAsyncStopListen = 2,
    EAsyncConnect = 3,
    EAsyncGetHostIp = 4,
    EAsyncCloseLink = 5,
    EAsyncSendMsg = 6,

    EAsyncLinkOpen = 100,
    EAsyncLinkStart = 101,
    EAsyncLinkMsg = 102,
    EAsyncLinkEnd = 103,
    EAsyncLinkClose = 104,
};

class PURENET_API AsyncItem : public PureCore::Node {
public:
    void clear() {
        mReqID = 0;
        mType = EAsyncInvalid;
        mMsg.remove();
    }

    int64_t mReqID{};
    EAsyncType mType{};
    NetMsgPtr mMsg;
};

struct PURENET_API ReqItem {
    int64_t mReqID{};
    int64_t mReqTime{};
    std::function<void(NetMsgPtr)> mResp{};
};

}  // namespace PureNet
