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

#include "PureCore/PureCoreLib.h"
#include "PureCore/PureXml.h"
#include "PureCore/CoreErrorDesc.h"
#include "PureCore/Buffer/DynamicBuffer.h"
#include "PureCore/MovePtr.h"
#include "PureLua/PureLuaLog.h"
#include "PureLua/LuaXml.h"
#include "PureLua/LuaErrorDesc.h"
#include "PureLua/LuaStack.h"
#include "PureLua/LuaIterator.h"

#include "yyjson.h"

namespace PureLua {
static const PureCore::StringRef sXmlNodes = "nodes";
static const PureCore::StringRef sXmlAttrs = "attrs";
static const PureCore::StringRef sXmlValue = "value";
static const PureCore::StringRef sXmlName = "name";

static int encode_object(lua_State* L, int tabIdx, PureCore::PureXmlItem& item) {
    tabIdx = lua_absindex(L, tabIdx);
    LuaStateProtect lsp(L);

    lua_pushlstring(L, sXmlName.data(), sXmlName.size());
    lua_rawget(L, tabIdx);
    if (LuaStack<PureCore::StringRef>::valid(L, -1)) {
        auto v = LuaStack<PureCore::StringRef>::get(L, -1);
        item.set_name(v);
    }

    lua_pushlstring(L, sXmlValue.data(), sXmlValue.size());
    lua_rawget(L, tabIdx);
    if (LuaStack<PureCore::DataRef>::valid(L, -1)) {
        auto v = LuaStack<PureCore::DataRef>::get(L, -1);
        if (item.set_value(v) != PureCore::Success) {
            return ErrorLuaXmlEncodeFailed;
        }
    }
    lua_pushlstring(L, sXmlAttrs.data(), sXmlAttrs.size());
    lua_rawget(L, tabIdx);
    if (lua_istable(L, -1)) {
        for (LuaIterator iter(L, -1); !iter.is_over(); ++iter) {
            auto key = iter.get_key<const char*>();
            auto val = iter.get_value<const char*>();
            int err = item.set_attr(key, val);
            if (err != PureCore::Success) {
                return ErrorLuaXmlEncodeFailed;
            }
        }
    }
    lua_pushlstring(L, sXmlNodes.data(), sXmlNodes.size());
    lua_rawget(L, tabIdx);
    if (lua_istable(L, -1)) {
        int nodeIdx = lua_absindex(L, -1);
        size_t len = lua_rawlen(L, -1);
        for (size_t i = 0; i < len; ++i) {
            lua_rawgeti(L, nodeIdx, i + 1);
            if (lua_istable(L, -1)) {
                auto subItem = item.append_child("");
                int err = encode_object(L, -1, subItem);
                if (err != Success) {
                    return err;
                }
            }
        }
    }
    return Success;
}

static int encode(lua_State* L) {
    if (!lua_istable(L, 1)) {
        PureLuaErrorJump(L, "1th arg is not table");
        return 0;
    }
    bool format = lua_toboolean(L, 2);
    PureCore::PureXml xml;
    auto root = xml.create_root("");
    int err = encode_object(L, 1, root);
    if (err != Success) {
        PureLuaErrorJump(L, get_error_desc(err));
        return 0;
    }
    auto buffer = PureCore::MakeMovePtr<PureCore::DynamicBuffer>();
    err = xml.save(buffer.ref(), format);
    if (err != PureCore::Success) {
        PureLuaErrorJump(L, PureCore::get_error_desc(err));
        return 0;
    }
    LuaStack<PureCore::MovePtr<PureCore::DynamicBuffer>>::push(L, buffer);
    return 1;
}

static int decode_object(lua_State* L, int tabIdx, PureCore::PureXmlItem& item) {
    tabIdx = lua_absindex(L, tabIdx);
    LuaStateProtect lsp(L);
    lua_pushlstring(L, sXmlName.data(), sXmlName.size());
    lua_pushstring(L, item.get_name());
    lua_rawset(L, tabIdx);
    auto val = item.get_value();
    if (val != nullptr && val[0] != 0) {
        lua_pushlstring(L, sXmlValue.data(), sXmlValue.size());
        lua_pushstring(L, val);
        lua_rawset(L, tabIdx);
    }
    if (item.has_any_attr()) {
        lua_pushlstring(L, sXmlAttrs.data(), sXmlAttrs.size());
        lua_createtable(L, 0, 8);
        item.range_attrs([L](const char* name, const char* attr) {
            lua_pushstring(L, name);
            lua_pushstring(L, attr);
            lua_rawset(L, -3);
        });
        lua_rawset(L, tabIdx);
    }
    auto child = item.get_first_child();
    if (child) {
        lua_createtable(L, 8, 0);
        lua_pushlstring(L, sXmlNodes.data(), sXmlNodes.size());
        lua_pushvalue(L, -2);
        lua_rawset(L, tabIdx);
        int idx = 1;
        while (child) {
            if (child.is_node_item()) {
                lua_createtable(L, 0, 8);
                int err = decode_object(L, -1, child);
                if (err != Success) {
                    return err;
                }
                lua_rawseti(L, -2, idx++);
            }
            child = child.get_next_brother();
        }
    }
    return Success;
}

static int decode(lua_State* L) {
    auto data = LuaStack<PureCore::DataRef>::get(L, 1);
    if (data.empty()) {
        PureLuaErrorJump(L, "1th arg is invalid");
        return 0;
    }

    PureCore::PureXml xml;
    int err = xml.load(data);
    if (err != PureCore::Success) {
        PureLuaErrorJump(L, PureCore::get_error_desc(err));
        return 0;
    }

    auto root = xml.get_root();
    if (root.is_null()) {
        PureLuaErrorJump(L, "xml not found root node");
        return 0;
    }

    lua_createtable(L, 0, 4);
    err = decode_object(L, -1, root);
    if (err != Success) {
        PureLuaErrorJump(L, get_error_desc(err));
        return 0;
    }
    return 1;
}

int luaopen_xml(lua_State* L) {
    luaL_Reg xml[] = {{"encode", encode}, {"decode", decode}, {nullptr, nullptr}};
    // xml
    lua_createtable(L, 0, PURE_ARRAY_SIZE(xml));
    luaL_setfuncs(L, xml, 0);
    return 1;
}
}  // namespace PureLua