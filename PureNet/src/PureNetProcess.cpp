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

#include "PureNet/NetErrorDesc.h"
#include "PureNet/Link.h"
#include "PureNet/PureNetProcess.h"

namespace PureNet {
#ifdef PURE_OPENSSL
int PureNetProcess::setup_ssl(const OpenSSLConfig& cfg) { return mReacter.setup_ssl(cfg); }
#endif

void PureNetProcess::set_config(const NetConfig& cfg) { mReacter.set_config(cfg); }

int PureNetProcess::start() {
    int err = mReacter.init();
    if (err != Success) {
        return err;
    }
    mReacter.mEventLinkOpen.bind(this, &PureNetProcess::on_link_open);
    mReacter.mEventLinkStart.bind(this, &PureNetProcess::on_link_start);
    mReacter.mEventLinkMsg.bind(this, &PureNetProcess::on_link_msg);
    mReacter.mEventLinkEnd.bind(this, &PureNetProcess::on_link_end);
    mReacter.mEventLinkClose.bind(this, &PureNetProcess::on_link_close);
    return Success;
}

void PureNetProcess::stop() {
    mReacter.release();
    mEventLinkOpen.clear();
    mEventLinkStart.clear();
    mEventLinkMsg.clear();
    mEventLinkEnd.clear();
    mEventLinkClose.clear();
}

void PureNetProcess::update(int64_t delta) { mReacter.update(delta); }

void PureNetProcess::get_host_ip(const char* host, std::function<GetHostIpCallback> cb) { mReacter.get_host_ip(host, cb); }

void PureNetProcess::close_link(LinkID linkID, int reason) { mReacter.close_link(linkID, reason); }

int PureNetProcess::send_msg(NetMsgPtr msg) {
    if (!msg) {
        return ErrorInvalidArg;
    }
    msg->set_send_flag(ESendSingle);
    return mReacter.link_mgr().send_msg(msg);
}

int PureNetProcess::broadcast_msg(const BroadcastDest& dest, NetMsgPtr msg) {
    if (!msg) {
        return ErrorInvalidArg;
    }
    msg->set_send_flag(ESendMulti);
    return mReacter.link_mgr().broadcast_msg(dest, msg);
}

bool PureNetProcess::on_link_open(Link* link) {
    if (link == nullptr) {
        return true;
    }
    char ip[INET6_ADDRSTRLEN]{};
    int port = 0;
    int err = link->get_remote_ip_port(ip, &port);
    if (err != Success) {
        PureError("on_link_open error {}", get_error_desc(err));
        return true;
    }
    mEventLinkOpen.notify(link->get_group_id(), link->get_link_id(), ip, port);
    return true;
}
bool PureNetProcess::on_link_start(Link* link) {
    if (link == nullptr) {
        return true;
    }
    mEventLinkStart.notify(link->get_group_id(), link->get_link_id());
    return true;
}

bool PureNetProcess::on_link_msg(Link* link) {
    if (link == nullptr) {
        return true;
    }
    NetMsgPtr msg = link->pop_read_msg();
    if (!msg) {
        return true;
    }
    mEventLinkMsg.notify(link->get_group_id(), link->get_link_id(), msg);
    return true;
}

bool PureNetProcess::on_link_end(Link* link) {
    if (link == nullptr) {
        return true;
    }
    mEventLinkEnd.notify(link->get_group_id(), link->get_link_id(), link->get_close_reason());
    return true;
}

bool PureNetProcess::on_link_close(Link* link) {
    if (link == nullptr) {
        return true;
    }
    mEventLinkClose.notify(link->get_group_id(), link->get_link_id(), link->get_close_reason());
    return true;
}
}  // namespace PureNet
