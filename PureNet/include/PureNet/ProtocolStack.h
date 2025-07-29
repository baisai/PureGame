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
#include "PureCore/Buffer/ArrayBuffer.h"
#include "PureCore/FixedVector.h"
#include "PureNet/PureNetLib.h"
#include "PureNet/NetErrorDesc.h"
#include "PureNet/PureNetTypes.h"
#include "PureNet/Protocol/Protocol.h"
#include "PureNet/NetMsg.h"

#include <functional>

namespace PureNet {
class Link;
const size_t MaxProtocolStackSize = 6;
class PURENET_API ProtocolStack : public Protocol {
public:
    ProtocolStack() = default;
    virtual ~ProtocolStack();

    int push_protocol(Protocol* proto);
    void clear_protocol();

    template <typename... Args>
    int push_protocol_t(Args*... args) {
        auto errs = std::initializer_list<int>{push_protocol(args)...};
        for (auto iter = errs.begin(); iter != errs.end(); ++iter) {
            if (*iter != Success) {
                return *iter;
            }
        }
        return Success;
    }

    int on_start(Link* l);
    int on_read(Link* l, PureCore::IBuffer& buffer);
    int on_write(Link* l, NetMsg& msg);
    void on_end(Link* l);

    virtual int start(Link* l) override;
    virtual int write(Link* l, PureCore::IBuffer& buffer, int64_t leftSize, int64_t totalSize) override;
    virtual int read_msg(Link* l, NetMsgPtr msg) override;
    virtual int end(Link* l) override;

    uint32_t get_writing_flag() const;

private:
    PureCore::FixedVector<Protocol*, MaxProtocolStackSize> mStatck;
    uint32_t mWritingFlag = 0;

    PURE_DISABLE_COPY(ProtocolStack)
};

}  // namespace PureNet
