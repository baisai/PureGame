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
#include "PureNet/LinkFactory.h"

namespace PureNet {
// no thread safe
class PURENET_API PureNetProcess {
public:
    PureNetProcess() = default;
    ~PureNetProcess() = default;

#ifdef PURE_OPENSSL
    int setup_ssl(const OpenSSLConfig& cfg);
#endif
    void set_config(const NetConfig& cfg);

    int start();
    void stop();

    void update(int64_t delta);

    template <typename T>
    int listen_tcp(GroupID groupID, const char* ip, int port) {
        return mReacter.listen_tcp(LinkFactory::key<T>(), groupID, ip, port);
    }
    void stop_listen_tcp(GroupID groupID) { mReacter.stop_listen_tcp(groupID); }
    template <typename T>
    void connect_tcp(GroupID groupID, const char* host, int port, std::function<ConnectCallback> cb) {
        mReacter.connect_tcp(LinkFactory::key<T>(), groupID, host, port, cb);
    }

    void get_host_ip(const char* host, std::function<GetHostIpCallback> cb);
    void close_link(LinkID linkID, int reason);

    int send_msg(NetMsgPtr msg);
    int broadcast_msg(const BroadcastDest& dest, NetMsgPtr msg);

public:
    PureCore::Event<GroupID, LinkID, const char*, int> mEventLinkOpen;
    PureCore::Event<GroupID, LinkID> mEventLinkStart;
    PureCore::Event<GroupID, LinkID, NetMsgPtr> mEventLinkMsg;
    PureCore::Event<GroupID, LinkID, int> mEventLinkEnd;
    PureCore::Event<GroupID, LinkID, int> mEventLinkClose;

private:
    bool on_link_open(Link* link);
    bool on_link_start(Link* link);
    bool on_link_msg(Link* link);
    bool on_link_end(Link* link);
    bool on_link_close(Link* link);

private:
    PureNetReacter mReacter;

    PURE_DISABLE_COPY(PureNetProcess)
};

}  // namespace PureNet
