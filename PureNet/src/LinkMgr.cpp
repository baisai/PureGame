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
#include "PureNet/LinkMgr.h"
#include "PureNet/PureNetReacter.h"

namespace PureNet {
LinkMgr::LinkMgr() = default;

LinkMgr::~LinkMgr() = default;

int LinkMgr::init() { return Success; }

void LinkMgr::release() {
    for (auto iter = mLinks.begin(); iter != mLinks.end(); ++iter) {
        iter->second->free();
    }
    mLinks.clear();
    mNeedFlush.clear();
}

Link* LinkMgr::find_link(LinkID linkID) {
    auto iter = mLinks.find(linkID);
    if (iter != mLinks.end()) {
        return iter->second;
    }
    return nullptr;
}

void LinkMgr::close_link(Link* link, int reason) {
    if (link == nullptr) {
        return;
    }
    int err = link->close(reason);
    if (err != Success) {
        PureWarn("link({}:{}) close failed desc `{}`", link->get_group_id(), link->get_link_id(), get_error_desc(err));
    }
}

void LinkMgr::close_link(LinkID linkID, int reason) { close_link(find_link(linkID), reason); }

int LinkMgr::add_link(Link* link, GroupID groupID, bool isServer) {
    if (link == nullptr) {
        return ErrorNullPointer;
    }
    LinkID linkID = mLinkIDGen.gen_id();
    auto result = mLinks.insert(std::make_pair(linkID, link));
    if (!result.second) {
        return ErrorLinkIDInvalid;
    }
    link->reset_id(groupID, linkID, isServer);
    return Success;
}

void LinkMgr::remove_link(LinkID linkID) { mLinks.erase(linkID); }

void LinkMgr::need_flush(Link* link) {
    if (link == nullptr || link->get_writer() == nullptr || link->get_writer()->size() == 0) {
        return;
    }
    mNeedFlush.insert(link->get_link_id());
}

void LinkMgr::flush_link() {
    for (auto linkID : mNeedFlush) {
        auto link = find_link(linkID);
        if (link == nullptr) {
            continue;
        }
        link->flush_data();
    }
    mNeedFlush.clear();
}

void LinkMgr::close_all_link(int reason) {
    for (auto iter = mLinks.begin(); iter != mLinks.end(); ++iter) {
        close_link(iter->second, reason);
    }
}

int LinkMgr::auto_send_msg(NetMsgPtr msg) {
    if (!msg) {
        return ErrorInvalidArg;
    }
    ENetMsgSendFlag flag = msg->get_send_flag();
    switch (flag) {
        case PureNet::ESendSingle:
            return send_msg(msg);
            break;
        case PureNet::ESendMulti: {
            BroadcastDest dest;
            int err = PureMsg::unpack(*msg, dest);
            if (err != PureMsg::Success) {
                PureError("send_msg failed, unpack send dest error `{}`", PureMsg::get_error_desc(err));
                return ErrorUnpackMsgFailed;
            }
            return broadcast_msg(dest, msg);
        } break;
        default:
            break;
    }
    return ErrorInvalidArg;
}

int LinkMgr::send_msg(NetMsgPtr msg) { return do_send_msg(msg.get()); }

int LinkMgr::broadcast_msg(const BroadcastDest& dest, NetMsgPtr msg) {
    for (auto& iter : dest) {
        for (auto userID : iter.second) {
            msg->set_link_id(iter.first);
            msg->set_user_id(userID);
            int err = do_send_msg(msg.get());
            if (err != Success) {
                PureError("broadcast_msg link failed, linkID {}, userID {}, error `{}`", iter.first, userID, get_error_desc(err));
            }
        }
    }
    return Success;
}

int LinkMgr::do_send_msg(NetMsg* msg) {
    if (msg == nullptr) {
        return ErrorNullPointer;
    }
    auto link = find_link(msg->get_link_id());
    if (link == nullptr) {
        return ErrorNotFoundLink;
    }
    return link->send_msg(*msg);
}

}  // namespace PureNet
