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
#include "PureNet/Link.h"
#include "PureNet/ProtocolStack.h"
#include "PureNet/PureNetReacter.h"
#include "PureNet/LinkMgr.h"
#include "PureNet/NetConfig.h"

#include <functional>

namespace PureNet {
Link::Link(ProtocolStack& ps)
    : mReacter(nullptr),
      mLinkID(0),
      mGroupID(0),
      mIsServer(false),
      mState(ELinkInvalid),
      mWritingSize(0),
      mLastAlive(0),
      mAliveTimerID(0),
      mReader(nullptr),
      mWriter(nullptr),
      mProtoStatck(ps) {}

void Link::free() {
    if (!mDeallocator) {
        PureError("not found link({}:{}) deallocator, maybe leak memory", mGroupID, mLinkID);
        return;
    }
    auto gc = mDeallocator;
    mDeallocator = nullptr;
    gc(this);
}

int Link::init(PureNetReacter* reacter) {
    mReacter = reacter;
    mCloseReason = 0;
    return Success;
}

void Link::clear() {
    mReacter = nullptr;
    mLinkID = 0;
    mGroupID = 0;
    mIsServer = false;
    mState = ELinkInvalid;
    mWritingSize = 0;
    mLastAlive = 0;
    mAliveTimerID = 0;
    mReader = nullptr;
    mWriter = nullptr;
    mCloseReason = 0;
    free_read_msg();
}

void Link::reset_id(GroupID groupID, LinkID linkID, bool isServer) {
    mGroupID = groupID;
    mLinkID = linkID;
    mIsServer = isServer;
    mState = ELinkOpening;
}

LinkID Link::get_link_id() const { return mLinkID; }

GroupID Link::get_group_id() const { return mGroupID; }

int64_t Link::get_alive_timer() const { return mAliveTimerID; }

void Link::set_alive_timer(int64_t timerID) { mAliveTimerID = timerID; }

PureNetReacter* Link::reacter() const { return mReacter; }

LinkMgr& Link::link_mgr() const { return mReacter->link_mgr(); }

const NetConfig& Link::config() const { return mReacter->config(); }

bool Link::is_server() const { return mIsServer; }

ELinkState Link::state() const { return mState; }

bool Link::valid() const { return mState == ELinkOpening || mState == ELinkOpen || mState == ELinkStart; }

int Link::get_close_reason() const { return mCloseReason; }

bool Link::is_alive() const { return ((mState == ELinkOpen || mState == ELinkStart) && (PureCore::steady_milli_s() - mLastAlive) < config().mKeepAlive); }

int Link::close(int reason) {
    if (mState == ELinkInvalid || mState == ELinkClose || mState == ELinkEnd || mReacter == nullptr) {
        return ErrorStateError;
    }
    mCloseReason = reason;
    mProtoStatck.on_end(this);
    return Success;
}

int Link::send_msg(NetMsg& msg) {
    if (mState != ELinkStart) {
        return ErrorStateError;
    }
    return mProtoStatck.on_write(this, msg);
}

void Link::on_open() {
    mState = ELinkOpen;
    mLastAlive = PureCore::steady_milli_s();
    if (mReacter != nullptr) {
        mReacter->on_link_open(this);
    }
    int err = mProtoStatck.on_start(this);
    if (err != Success) {
        link_mgr().close_link(this, err);
    }
}

void Link::on_close() {
    if (mReacter != nullptr) {
        mReacter->on_link_close(this);
    }
    mState = ELinkClose;
}

int Link::read() {
    if (!valid() || mReader == nullptr) {
        return ErrorStateError;
    }
    mLastAlive = PureCore::steady_milli_s();
    return mProtoStatck.on_read(this, *mReader);
}

int Link::on_start() {
    if (mState != ELinkOpen || mReacter == nullptr) {
        return ErrorStateError;
    }
    mState = ELinkStart;
    mReacter->on_link_start(this);
    return Success;
}

int Link::on_read(NetMsgPtr msg) {
    if (!valid() || mReacter == nullptr) {
        return ErrorStateError;
    }
    push_read_msg(msg);
    mReacter->on_link_msg(this);
    return Success;
}

int Link::on_write(PureCore::IBuffer& buffer, int64_t leftSize) {
    if (!valid() || mReacter == nullptr) {
        return ErrorStateError;
    }
    int err = push_data(buffer, leftSize <= 0);
    if (err != Success) {
        return err;
    }
    link_mgr().need_flush(this);
    return Success;
}

int Link::on_end() {
    if ((mState != ELinkOpen && mState != ELinkStart) || mReacter == nullptr) {
        return ErrorStateError;
    }

    mState = ELinkEnd;
    if (mState == ELinkStart) {
        mReacter->on_link_end(this);
    }
    return Success;
}

void Link::push_read_msg(NetMsgPtr msg) { mReadMsg = msg; }

NetMsgPtr Link::pop_read_msg() { return mReadMsg; }

void Link::free_read_msg() { mReadMsg.remove(); }

uint32_t Link::get_writing_flag() const { return mProtoStatck.get_writing_flag(); }

PureCore::FixedBuffer* Link::get_reader() { return mReader; }

PureCore::FixedBuffer* Link::get_writer() { return mWriter; }

PureCore::FixedBuffer* Link::swap_reader(PureCore::FixedBuffer* buffer) {
    if (buffer == nullptr) {
        return nullptr;
    }
    auto old = mReader;
    mReader = buffer;
    return old;
}

PureCore::FixedBuffer* Link::swap_writer(PureCore::FixedBuffer* buffer) {
    if (buffer == nullptr) {
        return nullptr;
    }
    auto old = mWriter;
    mWriter = buffer;
    return old;
}

int64_t Link::get_writing_size() const { return mWritingSize; }

int64_t Link::add_writing_size(size_t size) {
    mWritingSize += size;
    return mWritingSize;
}

int64_t Link::finish_writing_size(size_t size) {
    mWritingSize -= size;
    if (mWritingSize < 0) {
        mWritingSize = 0;
    }
    return mWritingSize;
}

}  // namespace PureNet
