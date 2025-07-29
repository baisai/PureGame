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

#include "PureCore/RectTree.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureLua {
void bind_core_rect_tree(lua_State* L) {
    using namespace PureCore;
    PureLua::LuaModule lm(L, "PureCore");
    lm[PureLua::LuaRegisterClass<RectNode>(L, "RectNode").def(&RectNode::obj_id, "obj_id").def(&RectNode::abox, "abox") +
       PureLua::LuaRegisterClass<RectTree>(L, "RectTree")
           .default_ctor<uint32_t>()
           .def(&RectTree::clear, "clear")
           .def(
               [](lua_State* L) -> int {
                   RectTree& self = PureLua::LuaStack<RectTree&>::get(L, 1);
                   auto& r = self.all_objs();
                   lua_createtable(L, int(r.size()), 0);
                   for (int i = 0; i < r.size(); ++i) {
                       PureLua::LuaStack<RectNode*>::push(L, r[i]);
                       lua_rawseti(L, -2, i + 1);
                   }
                   return 1;
               },
               "all_objs")
           .def(
               [](lua_State* L) -> int {
                   RectTree& self = PureLua::LuaStack<RectTree&>::get(L, 1);
                   IntABox& box = PureLua::LuaStack<IntABox&>::get(L, 2);
                   auto& r = self.collide_abox(box);
                   lua_createtable(L, int(r.size()), 0);
                   for (int i = 0; i < r.size(); ++i) {
                       PureLua::LuaStack<RectNode*>::push(L, r[i]);
                       lua_rawseti(L, -2, i + 1);
                   }
                   return 1;
               },
               "collide_abox")
           .def(
               [](lua_State* L) -> int {
                   RectTree& self = PureLua::LuaStack<RectTree&>::get(L, 1);
                   IntCircle& cir = PureLua::LuaStack<IntCircle&>::get(L, 2);
                   auto& r = self.collide_circle(cir);
                   lua_createtable(L, int(r.size()), 0);
                   for (int i = 0; i < r.size(); ++i) {
                       PureLua::LuaStack<RectNode*>::push(L, r[i]);
                       lua_rawseti(L, -2, i + 1);
                   }
                   return 1;
               },
               "collide_circle")
           .def(
               [](lua_State* L) -> int {
                   RectTree& self = PureLua::LuaStack<RectTree&>::get(L, 1);
                   IntSector& sec = PureLua::LuaStack<IntSector&>::get(L, 2);
                   auto& r = self.collide_sector(sec);
                   lua_createtable(L, int(r.size()), 0);
                   for (int i = 0; i < r.size(); ++i) {
                       PureLua::LuaStack<RectNode*>::push(L, r[i]);
                       lua_rawseti(L, -2, i + 1);
                   }
                   return 1;
               },
               "collide_sector")
           .def(
               [](lua_State* L) -> int {
                   RectTree& self = PureLua::LuaStack<RectTree&>::get(L, 1);
                   IntOBox& obox = PureLua::LuaStack<IntOBox&>::get(L, 2);
                   auto& r = self.collide_obox(obox);
                   lua_createtable(L, int(r.size()), 0);
                   for (int i = 0; i < r.size(); ++i) {
                       PureLua::LuaStack<RectNode*>::push(L, r[i]);
                       lua_rawseti(L, -2, i + 1);
                   }
                   return 1;
               },
               "collide_obox")
           .def(&RectTree::is_collide_abox, "is_collide_abox")
           .def(&RectTree::is_collide_circle, "is_collide_circle")
           .def(&RectTree::is_collide_sector, "is_collide_sector")
           .def(&RectTree::is_collide_obox, "is_collide_obox")
           .def(&RectTree::insert, "insert")
           .def(&RectTree::remove, "remove")];
}
}  // namespace PureLua
