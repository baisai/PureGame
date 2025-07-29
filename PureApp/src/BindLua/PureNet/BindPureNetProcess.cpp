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
#include "PureNet/PureNetProcess.h"
#include "PureNet/NetProtocols.h"

#include "PureLua/LuaRegisterClass.h"
#include "PureLua/LuaIterator.h"

namespace PureApp {
void bind_net_pure_net_process(lua_State* L) {
    using namespace PureNet;
    PureLua::LuaModule lm(L, "PureNet");
    lm[PureLua::LuaRegisterClass<PureNetProcess>(L, "PureNetProcess")
           .default_ctor()
#ifdef PURE_OPENSSL
           .def(&PureNetProcess::setup_ssl, "setup_ssl")
#endif
           .def(&PureNetProcess::set_config, "config")
           .def(&PureNetProcess::start, "start")
           .def(&PureNetProcess::stop, "stop")
           .def(&PureNetProcess::update, "update")
           .def(&PureNetProcess::get_host_ip, "get_host_ip")
           .def(&PureNetProcess::close_link, "close_link")
           .def(&PureNetProcess::send_msg, "send_msg")
           .def(
               [](lua_State* L) -> int {
                   PureNetProcess& self = PureLua::LuaStack<PureNetProcess&>::get(L, 1);
                   if (!lua_istable(L, 2)) {
                       PureLuaErrorJump(L, "2th arg must a table");
                       return 0;
                   }
                   BroadcastDest dest;
                   for (PureLua::LuaIterator ls(L, 2); !ls.is_over(); ++ls) {
                       LinkID linkID = ls.get_key<LinkID>();
                       int64_t len = lua_rawlen(L, ls.value_idx());
                       for (int64_t i = 1; i <= len; ++i) {
                           lua_rawgeti(L, ls.value_idx(), i);
                           auto userID = PureLua::LuaStack<UserID>::get(L, -1);
                           if (userID > 0) {
                               dest[linkID].insert(userID);
                           }
                           lua_pop(L, 1);
                       }
                   }
                   auto msg = PureLua::LuaStack<NetMsg*>::get(L, 3);
                   int err = PureNet::Success;
                   if (msg == nullptr) {
                       err = PureNet::ErrorInvalidArg;
                   } else {
                       int err = self.broadcast_msg(dest, msg);
                   }
                   lua_pushinteger(L, err);
                   return 1;
               },
               "broadcast_msg")
           .def([](PureNetProcess& self, std::function<bool(GroupID, LinkID, const char*, int)> cb) { return self.mEventLinkOpen.bind(cb); },
                "listen_event_open")
           .def([](PureNetProcess& self, int64_t id) { self.mEventLinkOpen.unbind(id); }, "stop_event_open")
           .def([](PureNetProcess& self, std::function<bool(GroupID, LinkID)> cb) { return self.mEventLinkStart.bind(cb); }, "listen_event_start")
           .def([](PureNetProcess& self, int64_t id) { self.mEventLinkStart.unbind(id); }, "stop_event_start")
           .def([](PureNetProcess& self, std::function<bool(GroupID, LinkID, NetMsgPtr)> cb) { return self.mEventLinkMsg.bind(cb); }, "listen_event_msg")
           .def([](PureNetProcess& self, int64_t id) { self.mEventLinkMsg.unbind(id); }, "stop_event_msg")
           .def([](PureNetProcess& self, std::function<bool(GroupID, LinkID, int)> cb) { return self.mEventLinkEnd.bind(cb); }, "listen_event_end")
           .def([](PureNetProcess& self, int64_t id) { self.mEventLinkEnd.unbind(id); }, "stop_event_end")
           .def([](PureNetProcess& self, std::function<bool(GroupID, LinkID, int)> cb) { return self.mEventLinkClose.bind(cb); }, "listen_event_close")
           .def([](PureNetProcess& self, int64_t id) { self.mEventLinkClose.unbind(id); }, "stop_event_close")
           .def(&PureNetProcess::stop_listen_tcp, "stop_listen_tcp")
           .def(&PureNetProcess::listen_tcp<WSMsgLink>, "listen_ws_msg")
           .def(&PureNetProcess::connect_tcp<WSMsgLink>, "connect_ws_msg")
           .def(&PureNetProcess::listen_tcp<WSTextLink>, "listen_ws_text")
           .def(&PureNetProcess::connect_tcp<WSTextLink>, "connect_ws_text")];
}

}  // namespace PureApp
