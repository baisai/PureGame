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
#include "PureNet/NetErrorDesc.h"
#include "PureNet/NetMsg.h"

namespace PureNet {
static const uint32_t sMsgCheckFlag = 0xcdcd0000;
///////////////////////////////////////////////////////////////////////////
// NetMsgHead
//////////////////////////////////////////////////////////////////////////
void NetMsgHead::clear() {
    mFlag = 0;
    mSize = 0;
}

NetMsgHead::NetMsgHead() : mFlag(sMsgCheckFlag), mSize(0) {}
int NetMsgHead::pack(PureCore::IBuffer& buffer) const {
    char buf[8]{};
    PureCore::u32_to_four_char(mFlag, buf);
    memcpy(buf + 6, buf + 2, 2);
    PureCore::u32_to_four_char(mSize, buf + 2);
    int err = buffer.write(PureCore::DataRef(buf, 8));
    if (err != PureCore::Success) {
        return PureMsg::ErrorWriteBufferFailed;
    }
    return PureMsg::Success;
}

int NetMsgHead::unpack(PureCore::IBuffer& buffer) {
    char buf[8]{};
    PureCore::DataRef data;
    int err = buffer.read(data, 8);
    if (err != PureCore::Success) {
        return PureMsg::ErrorReadBufferFailed;
    }
    memcpy(buf, data.data(), 2);
    memcpy(buf + 2, data.data() + 6, 2);
    memcpy(buf + 4, data.data() + 2, 4);

    mFlag = PureCore::four_char_to_u32(buf);
    mSize = PureCore::four_char_to_u32(buf + 4);
    return PureMsg::Success;
}

size_t NetMsgHead::buffer_size() { return (sizeof(mFlag) + sizeof(mSize)); }

///////////////////////////////////////////////////////////////////////////
// NetMsgRoute
//////////////////////////////////////////////////////////////////////////
void NetMsgRoute::clear() {
    mSrcRoute = 0;
    mDstRoute = 0;
    mUserID = 0;
    mOpcodeID = 0;
}

///////////////////////////////////////////////////////////////////////////
// NetMsg
//////////////////////////////////////////////////////////////////////////
NetMsg* NetMsg::get() { return tlPool.get(); }

void NetMsg::free(NetMsg* obj) { tlPool.free(obj); }

void NetMsg::clear() {
    PureMsg::MsgDynamicBuffer::clear();
    mGroupID = 0;
    mLinkID = 0;
    mHead.clear();
    mRoute.clear();
}

size_t NetMsg::head_size() { return NetMsgHead::buffer_size(); }

int NetMsg::pack_head(PureCore::IBuffer& buffer) const { return mHead.pack(buffer); }

int NetMsg::unpack_head(PureCore::IBuffer& buffer) { return mHead.unpack(buffer); }

size_t NetMsg::get_body_size() const { return mHead.mSize; }

void NetMsg::set_body_size(size_t s) { mHead.mSize = uint32_t(s); }

void NetMsg::add_body_size(size_t s) { mHead.mSize += uint32_t(s); }

uint32_t NetMsg::get_flag() const { return mHead.mFlag; }

ENetMsgRouteFlag NetMsg::calc_route_flag(uint32_t flag) { return ENetMsgRouteFlag(flag & __ERouteMask); }

ENetMsgBodyFlag NetMsg::calc_body_flag(uint32_t flag) { return ENetMsgBodyFlag(flag & __EBodyMask); }

ENetMsgSendFlag NetMsg::calc_send_flag(uint32_t flag) { return ENetMsgSendFlag(flag & __ESendMask); }

ENetMsgExtraFlag NetMsg::calc_extra_flag(uint32_t flag) { return ENetMsgExtraFlag(flag & __EExtraMask); }

ENetMsgRouteFlag NetMsg::get_route_flag() const { return calc_route_flag(mHead.mFlag); }

void NetMsg::set_route_flag(ENetMsgRouteFlag flag) {
    mHead.mFlag &= (~__ERouteMask);
    mHead.mFlag |= (flag & __ERouteMask);
}

ENetMsgBodyFlag NetMsg::get_body_flag() const { return calc_body_flag(mHead.mFlag); }

void NetMsg::set_body_flag(ENetMsgBodyFlag flag) {
    mHead.mFlag &= (~__EBodyMask);
    mHead.mFlag |= (flag & __EBodyMask);
}

ENetMsgSendFlag NetMsg::get_send_flag() const { return calc_send_flag(mHead.mFlag); }

void NetMsg::set_send_flag(ENetMsgSendFlag flag) {
    mHead.mFlag &= (~__ESendMask);
    mHead.mFlag |= (flag & __ESendMask);
}

ENetMsgExtraFlag NetMsg::get_extra_flag() const { return calc_extra_flag(mHead.mFlag); }

void NetMsg::set_extra_flag(ENetMsgExtraFlag flag) {
    mHead.mFlag &= (~__EExtraMask);
    mHead.mFlag |= (flag & __EExtraMask);
}

bool NetMsg::check_msg_flag() const { return (mHead.mFlag & 0xffff0000u) == sMsgCheckFlag; }

int NetMsg::pack_route(PureCore::IBuffer& buffer) const {
    int err = mRoute.pack(buffer);
    if (err != PureMsg::Success) {
        return ErrorPackMsgFailed;
    }
    return Success;
}

int NetMsg::unpack_route(PureCore::IBuffer& buffer) {
    int err = mRoute.unpack(buffer);
    if (err != PureMsg::Success) {
        return ErrorUnpackMsgFailed;
    }
    return Success;
}

RouteID NetMsg::get_src_route() const { return mRoute.mSrcRoute; }

void NetMsg::set_src_route(RouteID routeID) { mRoute.mSrcRoute = routeID; }

RouteID NetMsg::get_dest_route() const { return mRoute.mDstRoute; }

void NetMsg::set_dest_route(RouteID routeID) { mRoute.mDstRoute = routeID; }

UserID NetMsg::get_user_id() const { return mRoute.mUserID; }

void NetMsg::set_user_id(UserID userID) { mRoute.mUserID = userID; }

OpcodeID NetMsg::get_opcode_id() const { return mRoute.mOpcodeID; }

void NetMsg::set_opcode_id(OpcodeID opcodeID) { mRoute.mOpcodeID = opcodeID; }

GroupID NetMsg::get_group_id() const { return mGroupID; }

void NetMsg::set_group_id(GroupID groupID) { mGroupID = groupID; }

LinkID NetMsg::get_link_id() const { return mLinkID; }

void NetMsg::set_link_id(LinkID linkID) { mLinkID = linkID; }

thread_local PureCore::ObjectCache<NetMsg, 256> NetMsg::tlPool{};

}  // namespace PureNet
