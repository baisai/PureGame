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

#include "PureCore/StringRef.h"
#include "PureLua/PureLuaLib.h"
#include "lua.hpp"

#include <vector>
#include <unordered_map>
#include <typeinfo>

namespace PureLua {
class PURELUA_API LuaClassMgr {
public:
    LuaClassMgr();
    ~LuaClassMgr();

    void register_class(const void *key);
    bool is_registered(const void *key);

    bool add_base_class(const void *key, const void *base, int32_t offset);

    int32_t get_class_offset(const void *fromKey, const void *toKey);

    static bool is_valid_offset(int32_t offset);

    // LuaClassMgr in lua key
    static void *get_key();
    // Object in lua Class table key
    static void *get_object_key();
    // Class key in lua Class table key
    static void *get_class_key();
    // weak object table key
    static void *get_weak_key();
    // object newer table key
    static void *get_newer_key();
    // object deler table key
    static void *get_deler_key();

    static LuaClassMgr *get_class_mgr(lua_State *L);
    static void release_class_mgr(lua_State *L);
    static int32_t calc_offset(lua_State *L, int fromIdx, int toIdx);

    static void push_keep_field(lua_State *L);
    static bool check_keep_field_str(lua_State *L, PureCore::StringRef str);
    static bool check_keep_field_stack(lua_State *L, int fieldIdx);

private:
    struct LuaClassInfo {
        LuaClassInfo(const void *key, int32_t offset, bool isBase) : mKey(key), mOffset(offset), mIsBase(isBase) {}
        const void *mKey;
        int32_t mOffset;
        bool mIsBase;
    };
    class LuaClass {
    public:
        bool set_class_offset(const void *key, int32_t offset, bool isBase);
        int32_t get_class_offset(const void *key);
        inline const std::vector<LuaClassInfo> &get_link_class() const { return mLinkClass; }

    private:
        std::vector<LuaClassInfo> mLinkClass;
    };

private:
    std::unordered_map<const void *, LuaClass *> mClassTable;
};

#ifdef PURE_DEBUG
size_t debug_class_info(const std::type_info &info);
#endif

}  // namespace PureLua
