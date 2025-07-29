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
#include "PureNet/RouteMgr.h"

namespace PureNet {
void RouteMgr::clear() {
    mRouteLinks.clear();
    mRoutes.clear();
    mUserLinks.clear();
    mUsers.clear();
    mServerTypes.clear();
    mServerTypeIDs.clear();
}

int RouteMgr::add_route_by_user(UserID userID, LinkID linkID) {
    auto iter = mUsers.find(userID);
    if (iter != mUsers.end()) {
        return ErrorLinkRouteExist;
    }
    mUserLinks[linkID].insert(userID);
    mUsers[userID] = linkID;
    return Success;
}

int RouteMgr::add_route_by_server(ServerType serverType, ServerID serverID, ServerIndex serverIndex, LinkID linkID) {
    RouteID routeID = serverType;
    routeID <<= (sizeof(ServerID) * 8);
    routeID |= serverID;
    RouteID serverTypeID = routeID;
    routeID <<= (sizeof(ServerIndex) * 8);
    routeID |= serverIndex;
    auto iter = mRoutes.find(routeID);
    if (iter != mRoutes.end()) {
        return ErrorLinkRouteExist;
    }
    mRouteLinks[linkID].insert(routeID);
    mRoutes[routeID] = linkID;
    mServerTypes[serverType][routeID] = linkID;
    mServerTypeIDs[serverTypeID][routeID] = linkID;
    return Success;
}

void RouteMgr::remove_route(LinkID linkID) {
    auto routeIter = mRouteLinks.find(linkID);
    auto userIter = mUserLinks.find(linkID);
    if (routeIter != mRouteLinks.end()) {
        for (auto routeID : routeIter->second) {
            ServerType serverType = routeID >> ((sizeof(ServerID) + sizeof(ServerIndex)) * 8);
            RouteID serverTypeID = routeID >> (sizeof(ServerIndex) * 8);
            mRoutes.erase(routeID);
            mServerTypes[serverType].erase(routeID);
            mServerTypeIDs[serverTypeID].erase(routeID);
        }
        mRouteLinks.erase(routeIter);
    }
    if (userIter != mUserLinks.end()) {
        for (auto userID : userIter->second) {
            mUsers.erase(userID);
        }
        mUserLinks.erase(userIter);
    }
}

LinkID RouteMgr::find_route_by_user(UserID userID) {
    auto iter = mUsers.find(userID);
    if (iter == mUsers.end()) {
        return 0;
    }
    return iter->second;
}

LinkID RouteMgr::find_route_by_server(ServerType serverType, ServerID serverID, ServerIndex serverIndex) {
    RouteID routeID = serverType;
    routeID <<= (sizeof(ServerID) * 8);
    routeID |= serverID;
    routeID <<= (sizeof(ServerIndex) * 8);
    routeID |= serverIndex;
    auto iter = mRoutes.find(routeID);
    if (iter == mRoutes.end()) {
        return 0;
    }
    return iter->second;
}

const RouteMap& RouteMgr::find_server_type_routes(ServerType serverType) {
    static RouteMap empty;
    auto iter = mServerTypes.find(serverType);
    if (iter == mServerTypes.end()) {
        return empty;
    }
    return iter->second;
}

const RouteMap& RouteMgr::find_server_type_id_routes(ServerType serverType, ServerID serverID) {
    static RouteMap empty;
    RouteID routeID = serverType;
    routeID <<= (sizeof(ServerID) * 8);
    routeID |= serverID;
    auto iter = mServerTypeIDs.find(routeID);
    if (iter == mServerTypeIDs.end()) {
        return empty;
    }
    return iter->second;
}

}  // namespace PureNet
