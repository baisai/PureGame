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

#include "PureNet/NetMsg.h"

#include "PureLua/LuaRegisterClass.h"
#include "PureLua/LuaRegisterEnum.h"

namespace PureApp {
void bind_net_net_msg(lua_State* L) {
    using namespace PureNet;
    PureLua::LuaModule lm(L, "PureNet");

    lm[PureLua::LuaRegisterEnum<ENetMsgRouteFlag>(L, "ENetMsgRouteFlag")
           .def(ERouteInvalid, "ERouteInvalid")
           .def(ERoutePack, "ERoutePack")
           .def(ERouteNoPack, "ERouteNoPack") +
       PureLua::LuaRegisterEnum<ENetMsgBodyFlag>(L, "ENetMsgBodyFlag").def(EBodyInvalid, "EBodyInvalid").def(EBodyMsg, "EBodyMsg").def(EBodyText, "EBodyText") +
       PureLua::LuaRegisterEnum<ENetMsgSendFlag>(L, "ENetMsgSendFlag")
           .def(ESendInvlid, "ESendInvlid")
           .def(ESendSingle, "ESendSingle")
           .def(ESendMulti, "ESendMulti") +
       PureLua::LuaRegisterEnum<ENetMsgExtraFlag>(L, "ENetMsgExtraFlag").def(EExtraInvalid, "EExtraInvalid")];

    lm[PureLua::LuaRegisterClass<NetMsg>(L, "NetMsg", PureLua::BaseClassStrategy<PureMsg::MsgDynamicBuffer>())
           .def_ctor(NetMsg::get, NetMsg::free)
           .def(&NetMsg::clear, "clear")
           .def(NetMsg::head_size, "head_size")
           .def(&NetMsg::pack_head, "pack_head")
           .def(&NetMsg::unpack_head, "unpack_head")
           .def(&NetMsg::get_body_size, "get_body_size")
           .def(&NetMsg::set_body_size, "set_body_size")
           .def(&NetMsg::add_body_size, "add_body_size")
           .def(&NetMsg::get_flag, "get_flag")
           .def(NetMsg::calc_route_flag, "calc_route_flag")
           .def(NetMsg::calc_body_flag, "calc_body_flag")
           .def(NetMsg::calc_send_flag, "calc_send_flag")
           .def(NetMsg::calc_extra_flag, "calc_extra_flag")
           .def(&NetMsg::get_route_flag, "get_route_flag")
           .def(&NetMsg::set_route_flag, "set_route_flag")
           .def(&NetMsg::get_body_flag, "get_body_flag")
           .def(&NetMsg::set_body_flag, "set_body_flag")
           .def(&NetMsg::get_send_flag, "get_send_flag")
           .def(&NetMsg::set_send_flag, "set_send_flag")
           .def(&NetMsg::get_extra_flag, "get_extra_flag")
           .def(&NetMsg::set_extra_flag, "set_extra_flag")
           .def(&NetMsg::check_msg_flag, "check_msg_flag")
           .def(&NetMsg::pack_route, "pack_route")
           .def(&NetMsg::unpack_route, "unpack_route")
           .def(&NetMsg::get_src_route, "get_src_route")
           .def(&NetMsg::set_src_route, "set_src_route")
           .def(&NetMsg::get_dest_route, "get_dest_route")
           .def(&NetMsg::set_dest_route, "set_dest_route")
           .def(&NetMsg::get_user_id, "get_user_id")
           .def(&NetMsg::set_user_id, "set_user_id")
           .def(&NetMsg::get_opcode_id, "get_opcode_id")
           .def(&NetMsg::set_opcode_id, "set_opcode_id")
           .def(&NetMsg::get_group_id, "get_group_id")
           .def(&NetMsg::set_group_id, "set_group_id")
           .def(&NetMsg::get_link_id, "get_link_id")
           .def(&NetMsg::set_link_id, "set_link_id")

    ];
}

}  // namespace PureApp
