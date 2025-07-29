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

#include "PureCore/OsHelper.h"
#include "PureMsg/MsgArgs.h"
#include "PureNet/NetErrorDesc.h"
#include "PureNet/PureNetReq.h"
#include "PureNet/LinkTcp.h"

#include <functional>

namespace PureNet {
///////////////////////////////////////////////////////////////////////////
// ListenTcpReq
//////////////////////////////////////////////////////////////////////////
ListenTcpReq::ListenTcpReq(GroupID groupID, std::function<TcpAcceptCallback> cb) : mGroupID(groupID), mAccept(cb), mHandle{} { mHandle.data = this; }

///////////////////////////////////////////////////////////////////////////
// ConnectTcpReq
//////////////////////////////////////////////////////////////////////////
int ConnectTcpReq::init(LinkTcp* link, std::function<ConnectCallback> cb) {
    if (link == nullptr || !cb) {
        return ErrorInvalidArg;
    }
    mLink = link;
    mConnected = cb;
    mHandle.data = this;
    return Success;
}

void ConnectTcpReq::clear() {
    mLink = nullptr;
    mConnected = nullptr;
    mHandle.data = nullptr;
}

///////////////////////////////////////////////////////////////////////////
// GetAddrReq
//////////////////////////////////////////////////////////////////////////
int GetAddrReq::init(std::function<GetHostAddrCallback> cb) {
    if (!cb) {
        return ErrorInvalidArg;
    }
    mGetted = cb;
    mHandle.data = this;
    return Success;
}

void GetAddrReq::clear() {
    mGetted = nullptr;
    mHandle.data = nullptr;
}

///////////////////////////////////////////////////////////////////////////
// WriteTcpReq
//////////////////////////////////////////////////////////////////////////
int WriteTcpReq::init(LinkTcp* link, PureCore::DataRef data, PureCore::FixedBuffer* lastBuffer) {
    if (link == nullptr || data.empty()) {
        return ErrorInvalidArg;
    }
    mLink = link;
    mLastBuffer = lastBuffer;
    mHandle.data = this;
    mUvBuf.base = data.data();
    mUvBuf.len = (uint32_t)data.size();
    return Success;
}

void WriteTcpReq::clear() {
    mLink = nullptr;
    mLastBuffer = nullptr;
    mHandle.data = nullptr;
    mUvBuf.base = nullptr;
    mUvBuf.len = 0;
}

}  // namespace PureNet
