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
#include "PureNet/PureNetLib.h"
#include "PureNet/PureNetTypes.h"
#include "PureNet/LinkHandle.h"
#include "PureNet/Link.h"

namespace PureNet {
class PURENET_API LinkTcp : public Link {
public:
    LinkTcp(ProtocolStack& ps);
    virtual ~LinkTcp();

    virtual int init(PureNetReacter* reacter);
    virtual void clear();

    uv_tcp_t& get_uv_handle();

    virtual int get_remote_ip_port(char* ip, int* port);
    virtual int get_local_ip_port(char* ip, int* port);

    virtual int flush_data();
    virtual int push_data(PureCore::IBuffer& buffer, bool msgEnd);

    virtual int close(int reason) override;

protected:
    LinkTcpHandle mHandle;

    PURE_DISABLE_COPY(LinkTcp)
};

}  // namespace PureNet
