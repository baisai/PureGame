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
#include "PureNet/PureNetTypes.h"

#include <unordered_map>
#include <unordered_set>

namespace PureNet {
typedef std::unordered_map<RouteID, LinkID> RouteMap;
typedef std::unordered_map<LinkID, std::unordered_set<RouteID>> LinkRouteMap;
typedef std::unordered_map<UserID, LinkID> UserMap;
typedef std::unordered_map<LinkID, std::unordered_set<UserID>> LinkUserMap;
class PURENET_API RouteMgr {
public:
    RouteMgr() = default;
    ~RouteMgr() = default;

    void clear();

    int add_route_by_user(UserID userID, LinkID linkID);
    int add_route_by_server(ServerType serverType, ServerID serverID, ServerIndex serverIndex, LinkID linkID);
    void remove_route(LinkID linkID);
    LinkID find_route_by_user(UserID userID);
    LinkID find_route_by_server(ServerType serverType, ServerID serverID, ServerIndex serverIndex);
    const RouteMap& find_server_type_routes(ServerType serverType);
    const RouteMap& find_server_type_id_routes(ServerType serverType, ServerID serverID);

private:
    LinkRouteMap mRouteLinks;
    RouteMap mRoutes;
    LinkUserMap mUserLinks;
    UserMap mUsers;
    std::unordered_map<ServerType, RouteMap> mServerTypes;
    std::unordered_map<RouteID, RouteMap> mServerTypeIDs;

    PURE_DISABLE_COPY(RouteMgr)
};

}  // namespace PureNet
