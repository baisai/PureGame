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

#include "PureNet/Protocol/Protocol.h"

namespace PureNet {
class Link;
class NetMsg;
class PURENET_API TextProtocol : public Protocol {
public:
    TextProtocol() = default;
    virtual ~TextProtocol() = default;

    virtual int start(Link* l) override;
    virtual int read(Link* l, PureCore::IBuffer& buffer) override;
    virtual int write_msg(Link* l, NetMsg& msg) override;
    virtual int end(Link* l) override;

private:
    int read_to_msg(Link* l, PureCore::IBuffer& buffer);

protected:
    NetMsgPtr mReading = nullptr;
    uint32_t mNeedSize = 0;

    PURE_DISABLE_COPY(TextProtocol)
};

}  // namespace PureNet
