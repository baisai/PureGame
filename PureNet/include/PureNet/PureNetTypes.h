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

#include "uv.h"

#include <unordered_map>
#include <unordered_set>
#include <cstdint>

namespace PureNet {
typedef uint32_t GroupID;
typedef int64_t LinkID;
typedef uint64_t LinkType;
typedef int64_t UserID;
typedef uint32_t OpcodeID;
typedef int16_t ServerType;
typedef uint32_t ServerID;
typedef uint16_t ServerIndex;
typedef int64_t RouteID;

typedef std::unordered_map<LinkID, std::unordered_set<UserID>> BroadcastDest;

enum ELinkState : uint8_t {
    ELinkInvalid = 0,
    ELinkOpening = 1,
    ELinkOpen = 2,
    ELinkStart = 3,
    ELinkEnd = 4,
    ELinkClose = 5,
};

enum ENetMsgRouteFlag : uint32_t {
    ERouteInvalid = 0x0,
    ERoutePack = 0x1,
    ERouteNoPack = 0x2,

    __ERouteMask = 0xf,
};

enum ENetMsgBodyFlag : uint32_t {
    EBodyInvalid = 0x0,
    EBodyMsg = 0x10,
    EBodyText = 0x20,

    __EBodyMask = 0xf0,
};

enum ENetMsgSendFlag : uint32_t {
    ESendInvlid = 0x0,
    ESendSingle = 0x100,
    ESendMulti = 0x200,

    __ESendMask = 0xf00,
};

enum ENetMsgExtraFlag : uint32_t {
    EExtraInvalid = 0x0,

    __EExtraMask = 0xf000,
};

using TcpAcceptCallback = void(uv_stream_t*, int);
using ConnectCallback = void(int, GroupID, LinkID);
using GetHostIpCallback = void(int, const char*);
using GetHostAddrCallback = void(int, const sockaddr*);

using ListenCallback = void(int, GroupID);

}  // namespace PureNet
