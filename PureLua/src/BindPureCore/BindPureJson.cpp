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

#include "PureCore/PureJson.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureLua {
void bind_core_pure_json(lua_State* L) {
    using namespace PureCore;
    PureLua::LuaModule lm(L, "PureCore");
    lm[PureLua::LuaRegisterClass<PureJson>(L, "PureJson")
           .default_ctor()
           .def(&PureJson::load, "load")
           .def(&PureJson::save, "save")
           .def(&PureJson::clear, "clear")
           .def(&PureJson::has_root, "has_root")
           .def(&PureJson::get_root, "get_root")
           .def(&PureJson::must_get_root, "must_get_root")
           .def(&PureJson::create_root, "create_root") +
       PureLua::LuaRegisterClass<PureJsonItem>(L, "PureJsonItem")
           .default_ctor()
           .def(&PureJsonItem::is_null, "is_null")
           .def(&PureJsonItem::is_bool, "is_bool")
           .def(&PureJsonItem::is_int, "is_int")
           .def(&PureJsonItem::is_float, "is_float")
           .def(&PureJsonItem::is_str, "is_str")
           .def(&PureJsonItem::is_obj, "is_obj")
           .def(&PureJsonItem::is_arr, "is_arr")
           .def(&PureJsonItem::is_obj_or_arr, "is_obj_or_arr")
           .def(&PureJsonItem::get_as_bool, "get_as_bool")
           .def(&PureJsonItem::get_as_int, "get_as_int")
           .def(&PureJsonItem::get_as_float, "get_as_float")
           .def(&PureJsonItem::get_as_str, "get_as_str")
           .def(&PureJsonItem::size, "size")
           .def(&PureJsonItem::get_arr_child, "get_arr_child")
           .def(&PureJsonItem::get_child, "get_child")
           .def(&PureJsonItem::set_null, "set_null")
           .def(&PureJsonItem::set_bool, "set_bool")
           .def(&PureJsonItem::set_int, "set_int")
           .def(&PureJsonItem::set_float, "set_float")
           .def(&PureJsonItem::set_str, "set_str")
           .def(&PureJsonItem::append_arr_bool, "append_arr_bool")
           .def(&PureJsonItem::insert_arr_bool, "insert_arr_bool")
           .def(&PureJsonItem::append_arr_int, "append_arr_int")
           .def(&PureJsonItem::insert_arr_int, "insert_arr_int")
           .def(&PureJsonItem::append_arr_float, "append_arr_float")
           .def(&PureJsonItem::insert_arr_float, "insert_arr_float")
           .def(&PureJsonItem::append_arr_str, "append_arr_str")
           .def(&PureJsonItem::insert_arr_str, "insert_arr_str")
           .def(&PureJsonItem::append_arr_arr, "append_arr_arr")
           .def(&PureJsonItem::insert_arr_arr, "insert_arr_arr")
           .def(&PureJsonItem::append_arr_obj, "append_arr_obj")
           .def(&PureJsonItem::insert_arr_obj, "insert_arr_obj")
           .def(&PureJsonItem::remove_arr_child, "remove_arr_child")
           .def(&PureJsonItem::add_bool, "add_bool")
           .def(&PureJsonItem::add_int, "add_int")
           .def(&PureJsonItem::add_float, "add_float")
           .def(&PureJsonItem::add_str, "add_str")
           .def(&PureJsonItem::add_arr, "add_arr")
           .def(&PureJsonItem::add_obj, "add_obj")
           .def(&PureJsonItem::remove_child, "remove_child")
           .def(&PureJsonItem::clear_children, "clear_children")
           .def(&PureJsonItem::range_arr, "range_arr")
           .def(&PureJsonItem::range_obj, "range_obj")];
}
}  // namespace PureLua
