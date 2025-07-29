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

#include "PureCore/PureXml.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureLua {
void bind_core_pure_xml(lua_State* L) {
    using namespace PureCore;
    PureLua::LuaModule lm(L, "PureCore");
    lm[PureLua::LuaRegisterClass<PureXml>(L, "PureXml")
           .default_ctor()
           .def(&PureXml::load, "load")
           .def(&PureXml::save, "save")
           .def(&PureXml::clear, "clear")
           .def(&PureXml::has_root, "has_root")
           .def(&PureXml::has_name_root, "has_name_root")
           .def(&PureXml::get_root, "get_root")
           .def(&PureXml::get_name_root, "get_name_root")
           .def(&PureXml::must_get_root, "must_get_root")
           .def(&PureXml::create_root, "create_root") +
       PureLua::LuaRegisterClass<PureXmlItem>(L, "PureXmlItem")
           .default_ctor()
           .def(&PureXmlItem::is_null, "is_null")
           .def(&PureXmlItem::get_name, "get_name")
           .def(&PureXmlItem::is_node_item, "is_node_item")
           .def(&PureXmlItem::has_child, "has_child")
           .def(&PureXmlItem::has_name_child, "has_name_child")
           .def(&PureXmlItem::get_first_child, "get_first_child")
           .def(&PureXmlItem::get_name_child, "get_name_child")
           .def(&PureXmlItem::must_get_child, "must_get_child")
           .def(&PureXmlItem::get_next_brother, "get_next_brother")
           .def(&PureXmlItem::get_next_name_brother, "get_next_name_brother")
           .def(&PureXmlItem::get_parent, "get_parent")
           .def(&PureXmlItem::get_value, "get_value")
           .def(&PureXmlItem::has_any_attr, "has_any_attr")
           .def(&PureXmlItem::get_attrs_count, "get_attrs_count")
           .def(&PureXmlItem::range_attrs, "range_attrs")
           .def(&PureXmlItem::has_attr, "has_attr")
           .def(&PureXmlItem::get_attr, "get_attr")
           .def(&PureXmlItem::get_attr_as_bool, "get_attr_as_bool")
           .def(&PureXmlItem::get_attr_as_int, "get_attr_as_int")
           .def(&PureXmlItem::get_attr_as_float, "get_attr_as_float")
           .def(&PureXmlItem::set_name, "set_name")
           .def(&PureXmlItem::append_child, "append_child")
           .def(&PureXmlItem::front_child, "front_child")
           .def(&PureXmlItem::insert_after_child, "insert_after_child")
           .def(&PureXmlItem::insert_before_child, "insert_before_child")
           .def(&PureXmlItem::add_to_last_child, "add_to_last_child")
           .def(&PureXmlItem::add_to_first_child, "add_to_first_child")
           .def(&PureXmlItem::leave, "leave")
           .def(&PureXmlItem::remove_child, "remove_child")
           .def(&PureXmlItem::remove_name_child, "remove_name_child")
           .def(&PureXmlItem::remove_name_children, "remove_name_children")
           .def(&PureXmlItem::clear_children, "clear_children")
           .def(&PureXmlItem::remove_attr, "remove_attr")
           .def(&PureXmlItem::clear_attrs, "clear_attrs")
           .def(&PureXmlItem::set_value, "set_value")
           .def(&PureXmlItem::set_attr, "set_attr")
           .def(&PureXmlItem::set_attr_bool, "set_attr_bool")
           .def(&PureXmlItem::set_attr_int, "set_attr_int")
           .def(&PureXmlItem::set_attr_float, "set_attr_float")];
}
}  // namespace PureLua
