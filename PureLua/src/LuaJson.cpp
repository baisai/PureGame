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
#include "PureCore/MovePtr.h"
#include "PureCore/Buffer/DynamicBuffer.h"
#include "PureLua/PureLuaLog.h"
#include "PureLua/LuaJson.h"
#include "PureLua/LuaErrorDesc.h"
#include "PureLua/LuaHelper.h"
#include "PureLua/LuaStack.h"

#include "yyjson.h"

namespace PureLua {
static const PureCore::StringRef sJsonNull = "null";
static const PureCore::StringRef sJsonTrue = "true";
static const PureCore::StringRef sJsonFalse = "false";

static const char sChar2Escape[256] = {
    'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'b', 't', 'n', 'u', 'f',  'r', 'u', 'u', 'u', 'u', 'u', 'u',  // 0~19
    'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 0,    0,   '"', 0,   0,   0,   0,   0,    // 20~39
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,    // 40~59
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,    // 60~79
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '\\', 0,   0,   0,   0,   0,   0,   0,    // 80~99
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,    // 100~119
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,    // 120~139
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,    // 140~159
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,    // 160~179
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,    // 180~199
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,    // 200~219
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,    // 220~239
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,                        // 240~256
};

static const char sHexDigits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

static long long char_ptr_to_ll(PureCore::StringRef str, size_t& lastIdx) {
    long long num = 0;
    int flag = 0;
    int base = 0;
    for (lastIdx = 0; lastIdx < str.size(); ++lastIdx) {
        if (isspace(str[lastIdx])) {
            continue;
        }
        if (flag == 0) {
            if (str[lastIdx] == '+') {
                flag = 1;
                continue;
            } else if (str[lastIdx] == '-') {
                flag = 2;
                continue;
            }
        }
        break;
    }
    if (lastIdx >= str.size()) {
        return flag == 2 ? -num : num;
    }
    if (str[lastIdx] == '0') {
        base = 8;
        ++lastIdx;
    } else if (lastIdx + 1 < str.size()) {
        if (str[lastIdx] == '0' && (str[lastIdx + 1] == 'x' || str[lastIdx + 1] == 'X')) {
            base = 16;
            lastIdx += 2;
        }
    }
    if (base == 0) {
        base = 10;
    }

    for (; lastIdx < str.size(); ++lastIdx) {
        char ch = str[lastIdx];
        if (isdigit(ch)) {
            num *= base;
            num += (ch - '0');
            continue;
        } else if (base == 16 && ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))) {
            num *= base;
            if (ch <= 'F') {
                num += (ch - 'A' + 10);
            } else {
                num += (ch - 'a' + 10);
            }
            continue;
        }
        break;
    }

    return flag == 2 ? -num : num;
}

struct JsonConfig {
    int mMaxDepth = 64;
    bool mEmptyAsArray = true;
    bool mDecodeIntegerKey = false;
    bool mEncodeFormat = false;
    bool mNullAsNil = true;
};

static int json_config_gc(lua_State* L) {
    JsonConfig* cfg = (JsonConfig*)lua_touserdata(L, 1);
    return 0;
}

static void push_new_json_config(lua_State* L) {
    void* mem = lua_newuserdatauv(L, sizeof(JsonConfig), 0);
    new (mem) JsonConfig{};
    lua_newtable(L);
    lua_pushcfunction(L, json_config_gc);
    lua_setfield(L, -2, "__gc");
    lua_setmetatable(L, -2);
}

static JsonConfig* get_json_config(lua_State* L) { return (JsonConfig*)lua_touserdata(L, lua_upvalueindex(1)); }

static int json_config_max_depth(lua_State* L) {
    JsonConfig* cfg = get_json_config(L);
    if (cfg == nullptr) {
        PureLuaErrorJump(L, "not get json config");
        return 0;
    }
    cfg->mMaxDepth = static_cast<int16_t>(luaL_checkinteger(L, 1));
    return 0;
}

static int json_config_empty_as_array(lua_State* L) {
    JsonConfig* cfg = get_json_config(L);
    if (cfg == nullptr) {
        PureLuaErrorJump(L, "not get json config");
        return 0;
    }
    cfg->mEmptyAsArray = static_cast<bool>(lua_toboolean(L, 1));
    return 0;
}

static int json_config_decode_integer_key(lua_State* L) {
    JsonConfig* cfg = get_json_config(L);
    if (cfg == nullptr) {
        PureLuaErrorJump(L, "not get json config");
        return 0;
    }
    cfg->mDecodeIntegerKey = static_cast<bool>(lua_toboolean(L, 1));
    return 0;
}

static int json_config_encode_format(lua_State* L) {
    JsonConfig* cfg = get_json_config(L);
    if (cfg == nullptr) {
        PureLuaErrorJump(L, "not get json config");
        return 0;
    }
    cfg->mEncodeFormat = static_cast<bool>(lua_toboolean(L, 1));
    return 0;
}

static int json_config_null_as_nil(lua_State* L) {
    JsonConfig* cfg = get_json_config(L);
    if (cfg == nullptr) {
        PureLuaErrorJump(L, "not get json config");
        return 0;
    }
    cfg->mNullAsNil = static_cast<bool>(lua_toboolean(L, 1));
    return 0;
}

static void lua_format_number(lua_State* L, int idx, PureCore::DynamicBuffer& buffer) {
    buffer.ensure_buffer(buffer.size() + 128);
    auto freeBuffer = buffer.free_buffer();
    if (lua_isinteger(L, idx)) {
        auto r = fmt::format_to_n(freeBuffer.data(), freeBuffer.size(), "{}", lua_tointeger(L, idx));
        buffer.write_pos(buffer.write_pos() + r.size);
    } else {
        auto r = fmt::format_to_n(freeBuffer.data(), freeBuffer.size(), "{}", lua_tonumber(L, idx));
        buffer.write_pos(buffer.write_pos() + r.size);
    }
}

static int encode_table(const JsonConfig* cfg, lua_State* L, int idx, int depth, PureCore::DynamicBuffer& buffer);

static int encode_object(const JsonConfig* cfg, lua_State* L, int idx, int depth, PureCore::DynamicBuffer& buffer) {
    int t = lua_type(L, idx);
    switch (t) {
        case LUA_TBOOLEAN: {
            if (lua_toboolean(L, idx))
                buffer.write_str(sJsonTrue);
            else
                buffer.write_str(sJsonFalse);
            return Success;
        }
        case LUA_TNUMBER: {
            lua_format_number(L, idx, buffer);
            return Success;
        }
        case LUA_TSTRING: {
            size_t len = 0;
            const char* str = lua_tolstring(L, idx, &len);
            buffer.ensure_buffer(buffer.size() + len * 6 + 2);
            buffer.write_char('\"');
            for (size_t i = 0; i < len; ++i) {
                unsigned char ch = (unsigned char)str[i];
                char esc = sChar2Escape[ch];
                if (!esc) {
                    buffer.write_char((char)ch);
                } else {
                    buffer.write_char('\\');
                    buffer.write_char(esc);
                    if (esc == 'u') {
                        buffer.write_str("00");
                        buffer.write_char(sHexDigits[(unsigned char)ch >> 4]);
                        buffer.write_char(sHexDigits[(unsigned char)ch & 0xF]);
                    }
                }
            }
            buffer.write_char('\"');
            return Success;
        }
        case LUA_TTABLE: {
            return encode_table(cfg, L, idx, depth, buffer);
        }
        case LUA_TNIL: {
            buffer.write_str(sJsonNull);
            return Success;
        }
        case LUA_TLIGHTUSERDATA: {
            if (lua_touserdata(L, idx) == nullptr) {
                buffer.write_str(sJsonNull);
                return Success;
            }
            return ErrorLuaJsonUserdataNotNull;
        }
    }
    return ErrorLuaJsonValueTypeInvalid;
}

static size_t array_size(lua_State* L, int idx) {
    idx = lua_absindex(L, idx);
    size_t len = lua_rawlen(L, idx);
    if (len == 0) {
        return 0;
    }
    lua_pushinteger(L, (lua_Integer)len);
    if (lua_next(L, idx) != 0) {
        const char* key = lua_tostring(L, -2);
        const char* val = lua_tostring(L, -1);
        lua_pop(L, 2);
        return 0;
    }
    return len;
}

static int encode_table(const JsonConfig* cfg, lua_State* L, int idx, int depth, PureCore::DynamicBuffer& buffer) {
    if ((++depth) > cfg->mMaxDepth) {
        return ErrorLuaJsonDepthMax;
    }

    idx = lua_absindex(L, idx);
    size_t size = array_size(L, idx);
    buffer.ensure_buffer(buffer.size() + 2 * size + depth);
    if (size > 0) {
        buffer.write_char('[');
        if (cfg->mEncodeFormat) {
            buffer.write_char('\n');
        }
        for (size_t i = 1; i <= size; i++) {
            if (cfg->mEncodeFormat) {
                buffer.write_repeat_char(depth, ' ');
            }
            lua_rawgeti(L, idx, i);
            int err = encode_object(cfg, L, -1, depth, buffer);
            if (err != Success) {
                return err;
            }
            lua_pop(L, 1);
            if (i < size) {
                buffer.write_char(',');
            }
            if (cfg->mEncodeFormat) {
                buffer.write_char('\n');
            }
        }
        if (cfg->mEncodeFormat) {
            buffer.write_repeat_char(depth - 1, ' ');
        }
        buffer.write_char(']');
    } else {
        size_t i = 0;
        buffer.write_char('{');
        lua_pushnil(L);  // [table, nil]
        while (lua_next(L, idx)) {
            if (i++ > 0) {
                buffer.write_char(',');
            }
            if (cfg->mEncodeFormat) {
                buffer.write_char('\n');
            }
            int key_type = lua_type(L, -2);
            switch (key_type) {
                case LUA_TSTRING: {
                    if (cfg->mEncodeFormat) {
                        buffer.write_repeat_char(depth, ' ');
                    }
                    size_t len = 0;
                    const char* key = lua_tolstring(L, -2, &len);
                    buffer.write_char('\"');
                    buffer.write(PureCore::DataRef(key, len));
                    buffer.write_str("\":");
                    if (cfg->mEncodeFormat) {
                        buffer.write_char(' ');
                    }
                    int err = encode_object(cfg, L, -1, depth, buffer);
                    if (err != Success) {
                        return err;
                    }
                    break;
                }
                case LUA_TNUMBER: {
                    if (cfg->mEncodeFormat) {
                        buffer.write_repeat_char(depth, ' ');
                    }
                    buffer.write_char('\"');
                    lua_format_number(L, -2, buffer);
                    buffer.write_str("\":");
                    if (cfg->mEncodeFormat) {
                        buffer.write_char(' ');
                    }
                    int err = encode_object(cfg, L, -1, depth, buffer);
                    if (err != Success) {
                        return err;
                    }
                    break;
                }
                default:
                    return ErrorLuaJsonKeyTypeInvalid;
            }
            lua_pop(L, 1);
        }

        if (i == 0 && cfg->mEmptyAsArray) {
            if (buffer.size() != 0) {
                buffer.write_pos(buffer.write_pos() - 1);
            }
            buffer.write_str("[]");
        } else {
            if (i > 0 && cfg->mEncodeFormat) {
                buffer.write_char('\n');
                buffer.write_repeat_char(depth - 1, ' ');
            }
            buffer.write_char('}');
        }
    }
    return Success;
}

static int encode(lua_State* L) {
    JsonConfig* cfg = get_json_config(L);
    if (cfg == nullptr) {
        PureLuaErrorJump(L, "not get json config");
        return 0;
    }
    luaL_checkany(L, 1);
    auto buffer = PureCore::MakeMovePtr<PureCore::DynamicBuffer>();
    int err = encode_object(cfg, L, 1, 0, *buffer);
    if (err != Success) {
        PureLuaErrorJump(L, get_error_desc(err));
        return 0;
    }
    LuaStack<PureCore::MovePtr<PureCore::DynamicBuffer>>::push(L, buffer);
    return 1;
}

static void decode_object(lua_State* L, yyjson_val* value, JsonConfig* cfg) {
    yyjson_type type = yyjson_get_type(value);
    switch (type) {
        case YYJSON_TYPE_ARR: {
            lua_createtable(L, (int)yyjson_arr_size(value), 0);
            lua_Integer pos = 1;
            yyjson_arr_iter iter;
            yyjson_arr_iter_init(value, &iter);
            while (nullptr != (value = yyjson_arr_iter_next(&iter))) {
                decode_object(L, value, cfg);
                lua_rawseti(L, -2, pos++);
            }
            break;
        }
        case YYJSON_TYPE_OBJ: {
            lua_createtable(L, 0, (int)yyjson_obj_size(value));

            yyjson_val *key, *val;
            yyjson_obj_iter iter;
            yyjson_obj_iter_init(value, &iter);
            while (nullptr != (key = yyjson_obj_iter_next(&iter))) {
                val = yyjson_obj_iter_get_val(key);
                PureCore::StringRef str(unsafe_yyjson_get_str(key), unsafe_yyjson_get_len(key));
                if (str.size() > 0) {
                    if (cfg->mDecodeIntegerKey) {
                        size_t lastIdx = 0;
                        long long num = char_ptr_to_ll(str, lastIdx);
                        if (lastIdx >= str.size()) {
                            lua_pushinteger(L, num);
                        } else {
                            lua_pushlstring(L, str.data(), str.size());
                        }
                    } else {
                        lua_pushlstring(L, str.data(), str.size());
                    }
                    decode_object(L, val, cfg);
                    lua_rawset(L, -3);
                }
            }
            break;
        }
        case YYJSON_TYPE_NUM: {
            yyjson_subtype subtype = yyjson_get_subtype(value);
            switch (subtype) {
                case YYJSON_SUBTYPE_UINT: {
                    lua_pushinteger(L, (int64_t)unsafe_yyjson_get_uint(value));
                    break;
                }
                case YYJSON_SUBTYPE_SINT: {
                    lua_pushinteger(L, unsafe_yyjson_get_sint(value));
                    break;
                }
                case YYJSON_SUBTYPE_REAL: {
                    lua_pushnumber(L, unsafe_yyjson_get_real(value));
                    break;
                }
            }
            break;
        }
        case YYJSON_TYPE_STR: {
            lua_pushlstring(L, unsafe_yyjson_get_str(value), unsafe_yyjson_get_len(value));
            break;
        }
        case YYJSON_TYPE_BOOL:
            lua_pushboolean(L, (yyjson_get_subtype(value) == YYJSON_SUBTYPE_TRUE) ? 1 : 0);
            break;
        case YYJSON_TYPE_NULL: {
            if (cfg->mNullAsNil) {
                lua_pushnil(L);
            } else {
                lua_pushlightuserdata(L, nullptr);
            }
            break;
        }
        default:
            break;
    }
}

static int decode(lua_State* L) {
    auto data = LuaStack<PureCore::DataRef>::get(L, 1);
    if (data.empty()) {
        PureLuaErrorJump(L, "1th arg is invalid");
        return 0;
    }

    JsonConfig* cfg = get_json_config(L);

    yyjson_read_err err;
    yyjson_doc* doc = yyjson_read_opts(data.data(), data.size(), 0, nullptr, &err);
    if (nullptr == doc) {
        PureLuaErrorJump(L, "decode error: {} code: {} at position: {}\n", err.msg, (int)err.code, err.pos);
    }
    decode_object(L, yyjson_doc_get_root(doc), cfg);
    yyjson_doc_free(doc);
    return 1;
}

static int new_json_env(lua_State* L) {
    luaL_Reg config[] = {
        {"max_depth", json_config_max_depth},         {"empty_as_array", json_config_empty_as_array}, {"decode_integer_key", json_config_decode_integer_key},
        {"encode_format", json_config_encode_format}, {"null_as_nil", json_config_null_as_nil},       {nullptr, nullptr}};
    luaL_Reg json[] = {{"encode", encode}, {"decode", decode}, {nullptr, nullptr}};

    // json config
    push_new_json_config(L);
    // json
    lua_createtable(L, 0, PURE_ARRAY_SIZE(json) + 2);
    lua_pushvalue(L, -2);
    luaL_setfuncs(L, json, 1);
    lua_pushlightuserdata(L, nullptr);
    raw_setfield(L, -2, "null");

    // config
    lua_createtable(L, 0, PURE_ARRAY_SIZE(config));
    lua_pushvalue(L, -3);
    luaL_setfuncs(L, config, 1);
    raw_setfield(L, -2, "config");

    lua_remove(L, -2);  // remove json config
    return 1;
}

int luaopen_json(lua_State* L) {
    new_json_env(L);
    lua_pushcclosure(L, new_json_env, 0);
    raw_setfield(L, -2, "new_json_env");
    return 1;
}
}  // namespace PureLua