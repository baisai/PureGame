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

#include "PureCore/PureCoreLib.h"
#include "PureCore/NodeList.h"
#include "PureCore/Buffer/FixedBuffer.h"
#include "PureCore/Buffer/ArrayBuffer.h"
#include "PureNet/PureNetLib.h"
#include "PureNet/PureNetTypes.h"

#include <functional>

namespace PureNet {
class ListenTcpReq : public PureCore::Node {
public:
    ListenTcpReq(GroupID groupID, std::function<TcpAcceptCallback> cb);

    GroupID mGroupID;
    uv_tcp_t mHandle;
    std::function<TcpAcceptCallback> mAccept;
};

class LinkTcp;
class ConnectTcpReq {
public:
    ConnectTcpReq() = default;

    int init(LinkTcp* link, std::function<ConnectCallback> cb);
    void clear();

    LinkTcp* mLink = nullptr;
    uv_connect_t mHandle{};
    std::function<ConnectCallback> mConnected{};
};

class GetAddrReq {
public:
    GetAddrReq() = default;

    int init(std::function<GetHostAddrCallback> cb);
    void clear();

    uv_getaddrinfo_t mHandle{};
    std::function<GetHostAddrCallback> mGetted{};
};

class WriteTcpReq {
public:
    WriteTcpReq() = default;

    int init(LinkTcp* link, PureCore::DataRef data, PureCore::FixedBuffer* lastBuffer);
    void clear();

    LinkTcp* mLink = nullptr;
    PureCore::FixedBuffer* mLastBuffer = nullptr;
    uv_write_t mHandle{};
    uv_buf_t mUvBuf{};
};

}  // namespace PureNet
