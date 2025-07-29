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
#include "PureCore/StringRef.h"
#include "PureCore/Memory/ObjectCache.h"
#include "PureCore/Buffer/FixedBuffer.h"
#include "PureNet/PureNetLib.h"
#include "PureNet/PureNetTypes.h"
#include "PureNet/NetMsg.h"

#include <functional>

namespace PureNet {
union SockAddr;
class PureNetReacter;
class LinkMgr;
class ProtocolStack;
struct NetConfig;
class PURENET_API Link {
public:
    Link(ProtocolStack& ps);
    virtual ~Link() = default;

    void free();

    virtual int init(PureNetReacter* reacter);
    virtual void clear();

    void reset_id(GroupID groupID, LinkID linkID, bool isServer);

    LinkID get_link_id() const;
    GroupID get_group_id() const;
    int64_t get_alive_timer() const;
    void set_alive_timer(int64_t timerID);
    PureNetReacter* reacter() const;
    LinkMgr& link_mgr() const;
    const NetConfig& config() const;

    bool is_server() const;
    ELinkState state() const;
    bool valid() const;
    int get_close_reason() const;
    bool is_alive() const;

    virtual int get_remote_ip_port(char* ip, int* port) = 0;
    virtual int get_local_ip_port(char* ip, int* port) = 0;

    virtual int flush_data() = 0;
    virtual int push_data(PureCore::IBuffer& buffer, bool msgEnd) = 0;

    virtual int close(int reason);

    int send_msg(NetMsg& msg);

    void on_open();
    void on_close();

    int read();

    int on_start();
    int on_read(NetMsgPtr msg);
    int on_write(PureCore::IBuffer& buffer, int64_t leftSize);
    int on_end();

    void push_read_msg(NetMsgPtr msg);
    NetMsgPtr pop_read_msg();
    void free_read_msg();

    uint32_t get_writing_flag() const;

    PureCore::FixedBuffer* get_reader();
    PureCore::FixedBuffer* get_writer();
    PureCore::FixedBuffer* swap_reader(PureCore::FixedBuffer* buffer);
    PureCore::FixedBuffer* swap_writer(PureCore::FixedBuffer* buffer);

    int64_t get_writing_size() const;
    int64_t add_writing_size(size_t size);
    int64_t finish_writing_size(size_t size);

protected:
    PureNetReacter* mReacter = nullptr;
    LinkID mLinkID = 0;
    GroupID mGroupID = 0;
    bool mIsServer = false;
    ELinkState mState = ELinkInvalid;
    int mCloseReason = 0;
    int64_t mWritingSize = 0;
    int64_t mLastAlive = 0;
    int64_t mAliveTimerID = 0;
    PureCore::FixedBuffer* mReader = nullptr;
    PureCore::FixedBuffer* mWriter = nullptr;

    ProtocolStack& mProtoStatck;
    NetMsgPtr mReadMsg;

    friend class LinkFactory;
    std::function<void(Link*)> mDeallocator;

    PURE_DISABLE_COPY(Link)
};

}  // namespace PureNet
