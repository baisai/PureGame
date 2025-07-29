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
#include "PureNet/PureNetLib.h"

namespace PureNet {
union PURENET_API SockAddr {
    struct sockaddr addr;
    struct sockaddr_in addr4;
    struct sockaddr_in6 addr6;
};

PURENET_API int get_ip_port_from_addr(const SockAddr& addr, char* ip, int* port);
PURENET_API int set_ip_port_to_addr(SockAddr& addr, const char* ip, int port);
PURENET_API int copy_addr(SockAddr& to, const struct sockaddr* from);
PURENET_API int set_addr_port(SockAddr& to, const struct sockaddr* from, int port);

class PureNetReacter;
class Link;
class PURENET_API LinkTcpHandle {
public:
    LinkTcpHandle();
    ~LinkTcpHandle();

    int init(PureNetReacter* reacter, Link* link);
    void release();

    uv_tcp_t& get_uv_handle();

    int get_remote_ip_port(char* ip, int* port);
    int get_local_ip_port(char* ip, int* port);

private:
    uv_tcp_t mTcp;

    PURE_DISABLE_COPY(LinkTcpHandle)
};

}  // namespace PureNet
