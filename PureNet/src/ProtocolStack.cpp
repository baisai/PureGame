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
#include "PureNet/ProtocolStack.h"

namespace PureNet {
ProtocolStack::~ProtocolStack() { clear_protocol(); }

int ProtocolStack::push_protocol(Protocol* proto) {
    if (!mStatck.push_back(proto)) {
        return ErrorLinkProtocolFull;
    }
    return Success;
}

void ProtocolStack::clear_protocol() { mStatck.clear(); }

int ProtocolStack::on_start(Link* l) {
    if (l == nullptr) {
        return ErrorNullPointer;
    }
    if (mStatck.empty()) {
        return ErrorLinkNoneProtocol;
    }

    Protocol* pre = this;
    for (auto iter = mStatck.begin(); iter != mStatck.end(); ++iter) {
        Protocol* curr = *iter;
        curr->pre(pre);
        pre->next(curr);
        pre = curr;
    }
    mStatck.back()->next(this);
    return mStatck.front()->start(l);
}

int ProtocolStack::on_read(Link* l, PureCore::IBuffer& buffer) {
    if (l == nullptr) {
        return ErrorNullPointer;
    }
    if (mStatck.empty()) {
        return ErrorLinkNoneProtocol;
    }
    return mStatck.front()->read(l, buffer);
}

int ProtocolStack::on_write(Link* l, NetMsg& msg) {
    if (l == nullptr) {
        return ErrorNullPointer;
    }
    if (mStatck.empty()) {
        return ErrorLinkNoneProtocol;
    }
    mWritingFlag = msg.get_flag();
    return mStatck.back()->write_msg(l, msg);
}

void ProtocolStack::on_end(Link* l) {
    if (l == nullptr || mStatck.empty()) {
        return;
    }
    mStatck.back()->end(l);
}

int ProtocolStack::start(Link* l) { return l->on_start(); }

int ProtocolStack::read_msg(Link* l, NetMsgPtr msg) { return l->on_read(msg); }

int ProtocolStack::write(Link* l, PureCore::IBuffer& buffer, int64_t leftSize, int64_t totalSize) { return l->on_write(buffer, leftSize); }

int ProtocolStack::end(Link* l) { return l->on_end(); }

uint32_t ProtocolStack::get_writing_flag() const { return mWritingFlag; }

}  // namespace PureNet
