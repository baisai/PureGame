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
#include "PureCore/Memory/ObjectCache.h"
#include "PureCore/MovePtr.h"
#include "PureMsg/MsgClass.h"
#include "PureMsg/MsgBuffer.h"
#include "PureNet/PureNetLib.h"
#include "PureNet/PureNetTypes.h"

namespace PureNet {
class PURENET_API NetMsgHead {
private:
    friend class NetMsg;
    NetMsgHead();
    ~NetMsgHead() = default;

    void clear();

private:
    int pack(PureCore::IBuffer& buffer) const;
    int unpack(PureCore::IBuffer& buffer);
    static size_t buffer_size();

    uint32_t mFlag = 0;
    uint32_t mSize = 0;
};

class PURENET_API NetMsgRoute {
private:
    friend class NetMsg;
    NetMsgRoute() = default;
    ~NetMsgRoute() = default;

    void clear();

private:
    PUREMSG_CLASS(mSrcRoute, mDstRoute, mUserID, mOpcodeID)

    RouteID mSrcRoute = 0;
    RouteID mDstRoute = 0;
    UserID mUserID = 0;
    OpcodeID mOpcodeID = 0;
};

class PURENET_API NetMsg : public PureMsg::MsgDynamicBuffer {
public:
    NetMsg() = default;
    virtual ~NetMsg() = default;

    // thread safe
    static NetMsg* get();
    // thread safe
    static void free(NetMsg* obj);

    virtual void clear();

    static size_t head_size();
    int pack_head(PureCore::IBuffer& buffer) const;
    int unpack_head(PureCore::IBuffer& buffer);

    size_t get_body_size() const;
    void set_body_size(size_t s);
    void add_body_size(size_t s);

    uint32_t get_flag() const;
    static ENetMsgRouteFlag calc_route_flag(uint32_t flag);
    static ENetMsgBodyFlag calc_body_flag(uint32_t flag);
    static ENetMsgSendFlag calc_send_flag(uint32_t flag);
    static ENetMsgExtraFlag calc_extra_flag(uint32_t flag);

    ENetMsgRouteFlag get_route_flag() const;
    void set_route_flag(ENetMsgRouteFlag flag);
    ENetMsgBodyFlag get_body_flag() const;
    void set_body_flag(ENetMsgBodyFlag flag);
    ENetMsgSendFlag get_send_flag() const;
    void set_send_flag(ENetMsgSendFlag flag);
    ENetMsgExtraFlag get_extra_flag() const;
    void set_extra_flag(ENetMsgExtraFlag flag);
    bool check_msg_flag() const;

    int pack_route(PureCore::IBuffer& buffer) const;
    int unpack_route(PureCore::IBuffer& buffer);

    RouteID get_src_route() const;
    void set_src_route(RouteID routeID);
    RouteID get_dest_route() const;
    void set_dest_route(RouteID routeID);
    UserID get_user_id() const;
    void set_user_id(UserID userID);
    OpcodeID get_opcode_id() const;
    void set_opcode_id(OpcodeID opcodeID);

    GroupID get_group_id() const;
    void set_group_id(GroupID groupID);
    LinkID get_link_id() const;
    void set_link_id(LinkID linkID);

private:
    GroupID mGroupID = 0;
    LinkID mLinkID = 0;
    NetMsgHead mHead;
    NetMsgRoute mRoute;

    static thread_local PureCore::ObjectCache<NetMsg, 256> tlPool;
};

using NetMsgPtr = PureCore::MovePtr<NetMsg, NetMsg>;

}  // namespace PureNet
