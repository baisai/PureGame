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
#include "PureNet/LinkHandle.h"
#include "PureNet/Link.h"
#include "PureNet/PureNetReacter.h"

namespace PureNet {
///////////////////////////////////////////////////////////////////////////
// SockAddr
//////////////////////////////////////////////////////////////////////////
int get_ip_port_from_addr(const SockAddr& addr, char* ip, int* port) {
    if (ip == nullptr || port == nullptr) {
        return ErrorInvalidArg;
    }
    int err = uv_ip_name(&addr.addr, ip, INET6_ADDRSTRLEN);
    *port = ntohs(addr.addr4.sin_port);
    return err;
}

int set_ip_port_to_addr(SockAddr& addr, const char* ip, int port) {
    if (ip == nullptr) {
        return ErrorInvalidArg;
    }
    if (PureCore::is_ip_v4(ip)) {  // ipv4
        return uv_ip4_addr(ip, port, &addr.addr4);
    }
    return uv_ip6_addr(ip, port, &addr.addr6);
}

int copy_addr(SockAddr& to, const struct sockaddr* from) {
    if (from == nullptr) {
        return ErrorInvalidArg;
    }
    if (from->sa_family == AF_INET) {
        to.addr4 = *(const struct sockaddr_in*)from;
        return Success;
    } else if (from->sa_family == AF_INET6) {
        to.addr6 = *(const struct sockaddr_in6*)from;
        return Success;
    }
    return ErrorSockAddrInvalid;
}

int set_addr_port(SockAddr& to, const struct sockaddr* from, int port) {
    if (from == nullptr) {
        return ErrorInvalidArg;
    }
    if (from->sa_family == AF_INET) {
        to.addr4 = *(const struct sockaddr_in*)from;
        to.addr4.sin_port = htons(port);
        return Success;
    } else if (from->sa_family == AF_INET6) {
        to.addr6 = *(const struct sockaddr_in6*)from;
        to.addr6.sin6_port = htons(port);
        return Success;
    }
    return ErrorSockAddrInvalid;
}

///////////////////////////////////////////////////////////////////////////
// LinkTcpHandle
//////////////////////////////////////////////////////////////////////////
LinkTcpHandle::LinkTcpHandle() : mTcp{} {}

LinkTcpHandle::~LinkTcpHandle() {}

int LinkTcpHandle::init(PureNetReacter* reacter, Link* link) {
    if (reacter == nullptr) {
        return ErrorInvalidArg;
    }
    int err = uv_tcp_init(&reacter->get_uv_handle(), &mTcp);
    if (err != 0) {
        return err;
    }
    err = uv_tcp_nodelay(&mTcp, 1);
    if (err != 0) {
        return err;
    }
    mTcp.data = link;
    return Success;
}

void LinkTcpHandle::release() { mTcp.data = nullptr; }

uv_tcp_t& LinkTcpHandle::get_uv_handle() { return mTcp; }

int LinkTcpHandle::get_remote_ip_port(char* ip, int* port) {
    SockAddr addr;
    int nameLen = sizeof(addr);
    int err = 0;
    err = uv_tcp_getpeername(&mTcp, &addr.addr, &nameLen);
    if (err) {
        return err;
    }

    return get_ip_port_from_addr(addr, ip, port);
}

int LinkTcpHandle::get_local_ip_port(char* ip, int* port) {
    SockAddr addr;
    int nameLen = sizeof(addr);
    int err = 0;
    err = uv_tcp_getsockname(&mTcp, &addr.addr, &nameLen);
    if (err) {
        return err;
    }

    return get_ip_port_from_addr(addr, ip, port);
}

}  // namespace PureNet
