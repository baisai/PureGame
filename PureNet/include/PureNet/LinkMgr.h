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
#include "PureCore/IncrIDGen.h"
#include "PureCore/Event.h"
#include "PureNet/PureNetLib.h"
#include "PureNet/PureNetTypes.h"
#include "PureNet/NetMsg.h"
#include "PureNet/PureNetReq.h"
#include "PureNet/NetConfig.h"

#include <unordered_map>

namespace PureNet {
class Link;
class PureNetReacter;
class PURENET_API LinkMgr {
public:
    LinkMgr();
    ~LinkMgr();

    int init();
    void release();

    Link* find_link(LinkID linkID);
    void close_link(Link* link, int reason);
    void close_link(LinkID linkID, int reason);
    int add_link(Link* link, GroupID groupID, bool isServer);
    void remove_link(LinkID linkID);

    void need_flush(Link* link);
    void flush_link();

    void close_all_link(int reason);

    int auto_send_msg(NetMsgPtr msg);
    int send_msg(NetMsgPtr msg);
    int broadcast_msg(const BroadcastDest& dest, NetMsgPtr msg);

private:
    int do_send_msg(NetMsg* msg);

private:
    std::unordered_map<int64_t, Link*> mLinks;
    std::unordered_set<int64_t> mNeedFlush;
    PureCore::IncrIDGen mLinkIDGen;

    PURE_DISABLE_COPY(LinkMgr)
};

}  // namespace PureNet
