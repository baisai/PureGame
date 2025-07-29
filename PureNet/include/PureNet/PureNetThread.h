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

#include "PureCore/Thread.h"
#include "PureNet/PureNetLib.h"
#include "PureNet/PureNetReacter.h"
#include "PureNet/PureNetAsync.h"
#include "PureNet/LinkFactory.h"

#include <mutex>

namespace PureNet {
class PURENET_API PureNetThread : public PureCore::Thread {
public:
    PureNetThread() = default;
    ~PureNetThread() = default;

#ifdef PURE_OPENSSL
    int setup_ssl(const OpenSSLConfig& cfg);
#endif
    void set_config(const NetConfig& cfg);

    int start(int64_t reqTimeout);
    void stop();

    int64_t get_req_timeout() const;

    void update();

    template <typename T>
    void listen_tcp(GroupID groupID, const char* ip, int port, std::function<ListenCallback> cb) {
        listen(ESockTcp, LinkFactory::key<T>(), groupID, ip, port, cb);
    }
    void stop_listen_tcp(GroupID groupID) { stop_listen(ESockTcp, groupID); }
    template <typename T>
    void connect_tcp(GroupID groupID, const char* host, int port, std::function<ConnectCallback> cb) {
        connect(ESockTcp, LinkFactory::key<T>(), groupID, host, port, cb);
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

protected:
    virtual void work();

private:
    void logic_req();
    void logic_resp();

    void work_req();
    void work_resp();

private:
    void listen(ESockType type, LinkType key, GroupID groupID, const char* ip, int port, std::function<ListenCallback> cb);
    void stop_listen(ESockType type, GroupID groupID);
    void connect(ESockType type, LinkType key, GroupID groupID, const char* host, int port, std::function<ConnectCallback> cb);

    void on_net_listen(AsyncItem* item);
    void on_net_stop_listen(AsyncItem* item);
    void on_net_connect(AsyncItem* item);
    void on_net_get_host_ip(AsyncItem* item);
    void on_net_close_link(AsyncItem* item);
    void on_net_send_msg(AsyncItem* item);

    bool on_link_open(Link* link);
    bool on_link_start(Link* link);
    bool on_link_msg(Link* link);
    bool on_link_end(Link* link);
    bool on_link_close(Link* link);

private:
    std::atomic<bool> mReacterRunning{};
    PureNetReacter mReacter;

    int64_t mReqTimeOut{};
    PureCore::IncrIDGen mReqGen;
    std::map<int64_t, ReqItem*> mReqWaiting{};

    PureCore::NodeList mReqQueue;
    PureCore::NodeList mRespQueue;

    std::mutex mSwapReqMutex;
    PureCore::NodeList mSwapReqQueue;
    std::mutex mSwapRespMutex;
    PureCore::NodeList mSwapRespQueue;

    PureCore::ObjectPool<ReqItem, 255> mReqPool;
    PureCore::ObjectCache<AsyncItem, 255> mAsyncReqPool;
    PureCore::ObjectCache<AsyncItem, 255> mAsyncRespPool;

    PURE_DISABLE_COPY(PureNetThread)
};

}  // namespace PureNet
