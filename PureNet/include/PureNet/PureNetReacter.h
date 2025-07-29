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
#include "PureCore/PureLog.h"
#include "PureCore/RandomGen.h"
#include "PureCore/Memory/ObjectCache.h"
#include "PureCore/Memory/ObjectPool.h"
#include "PureCore/TWTimer.h"
#include "PureNet/PureNetLib.h"
#include "PureNet/PureNetTypes.h"
#include "PureNet/LinkHandle.h"
#include "PureNet/LinkMgr.h"
#include "PureNet/PureNetReq.h"

#include "uv.h"

#include <functional>

namespace PureNet {
class Link;
class LinkTcp;
class PURENET_API PureNetReacter {
public:
    PureNetReacter();
    ~PureNetReacter() = default;

    const NetConfig& config() const;
    void set_config(const NetConfig& cfg);

    int init();
    void release();

    LinkMgr& link_mgr();
    uv_loop_t& get_uv_handle();
    PureCore::RandomGen& randomer();
    PureCore::TWTimer& timer();

    void update(int64_t delta);

    int listen_tcp(LinkType key, GroupID groupID, const char* ip, int port);
    void stop_listen_tcp(GroupID groupID);
    void connect_tcp(LinkType key, GroupID groupID, const char* host, int port, std::function<ConnectCallback> cb);

    void close_link(LinkID linkID, int reason);

    void get_host_ip(const char* host, std::function<GetHostIpCallback> cb);

    void add_next_frame(const std::function<void()>& cb);

public:
    PureCore::Event<Link*> mEventLinkOpen;
    PureCore::Event<Link*> mEventLinkStart;
    PureCore::Event<Link*> mEventLinkMsg;
    PureCore::Event<Link*> mEventLinkEnd;
    PureCore::Event<Link*> mEventLinkClose;

public:
    void close_tcp(LinkTcp* link, int reason);

private:
    uv_shutdown_t* get_shutdown_t();
    void free_shutdown_t(uv_shutdown_t* obj);
    GetAddrReq* get_getaddr_req();
    void free_getaddr_req(GetAddrReq* obj);
    ConnectTcpReq* get_connect_tcp_req();
    void free_connect_tcp_req(ConnectTcpReq* obj);

public:
    PureCore::FixedBuffer* get_tcp_buffer();
    void free_tcp_buffer(PureCore::FixedBuffer* obj);
    WriteTcpReq* get_tcp_write_req();
    void free_tcp_write_req(WriteTcpReq* obj);

private:
    int connect_tcp_link(LinkTcp* link, const struct sockaddr* addr, std::function<ConnectCallback> cb);
    void get_host_addr(const char* host, std::function<GetHostAddrCallback> cb);
    void stop_listen_tcp_req(ListenTcpReq* req);

private:
    int on_accept_tcp(LinkType key, uv_stream_t* server);
    void on_read_alloc_tcp(LinkTcp* link, uv_buf_t* buf);
    void on_read_tcp(LinkTcp* link, ssize_t nread, const uv_buf_t* buf);
    int open_tcp_link(LinkTcp* link);

    void on_tcp_shutdown(uv_shutdown_t* req, int status);
    void on_close(Link* link);

public:
    void on_link_open(Link* link);
    void on_link_start(Link* link);
    void on_link_msg(Link* link);
    void on_link_end(Link* link);
    void on_link_close(Link* link);

private:
    int mState{};
    uv_loop_t mLoop{};
    LinkMgr mLinks;
    PureCore::NodeList mListenTcp;
    PureCore::TWTimer mTimer;
    std::vector<std::function<void()>> mReadyFrame;
    std::vector<std::function<void()>> mWorkFrame;
    PureCore::RandomGen mRandGen;
    NetConfig mConfig;

private:
    struct NetObjectPools {
        PureCore::ObjectPool<uv_shutdown_t, 128> mShutdownPool;
        PureCore::ObjectCache<GetAddrReq, 128> mGetAddrPool;
        PureCore::ObjectCache<ConnectTcpReq, 128> mTcpConnectPool;

        PureCore::ObjectCache<PureCore::FixedBuffer, 256> mTcpBufferPool;
        PureCore::ObjectCache<WriteTcpReq, 256> mWriteTcpPool;
    } mPools;
    PURE_DISABLE_COPY(PureNetReacter)
};

}  // namespace PureNet
