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
#include "PureCore/Buffer/IBuffer.h"
#include "PureNet/PureNetLib.h"
#include "PureNet/NetErrorDesc.h"
#include "PureNet/NetMsg.h"

namespace PureNet {
class Link;
class PURENET_API Protocol {
public:
    Protocol() = default;
    virtual ~Protocol() = default;

    Protocol* next() const { return mNext; }
    void next(Protocol* p) { mNext = p; }
    Protocol* pre() const { return mPre; }
    void pre(Protocol* p) { mPre = p; }

    virtual int start(Link* l) { return ErrorNotSupport; }
    virtual int read(Link* l, PureCore::IBuffer& buffer) { return ErrorNotSupport; }
    virtual int write(Link* l, PureCore::IBuffer& buffer, int64_t leftSize, int64_t totalSize) { return ErrorNotSupport; }
    virtual int read_msg(Link* l, NetMsgPtr msg) { return ErrorNotSupport; }
    virtual int write_msg(Link* l, NetMsg& msg) { return ErrorNotSupport; }
    virtual int end(Link* l) { return ErrorNotSupport; }

private:
    Protocol* mNext = nullptr;
    Protocol* mPre = nullptr;

    PURE_DISABLE_COPY(Protocol)
};

}  // namespace PureNet
