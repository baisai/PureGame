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

#include "PureLua/LuaDefAutoTool.h"
#include "PureLua/LuaRef.h"

namespace PureLua {
class PURELUA_API LuaRegisterNode {
public:
    LuaRegisterNode(lua_State* L);
    LuaRegisterNode(const LuaRef& key, const LuaRef& value, const LuaRef& parent);
    LuaRegisterNode(const LuaRegisterNode& cp);
    virtual ~LuaRegisterNode();

    lua_State* state() const;

    template <typename T>
    inline void set_key(const T& t) {
        mKey = t;
    }

    void set_key_from(int idx);

    template <typename T>
    inline void set_value(const T& t) {
        mValue = t;
    }

    void set_value_from(int idx);

    template <typename T>
    inline void set_parent(const T& t) {
        mParent = t;
    }

    void set_parent_from(int idx);

    void push_parent() const;
    void push() const;
    void push_key() const;
    void push_value() const;

    LuaRegisterNode& operator+(const LuaRegisterNode& right);
    LuaRegisterNode& operator=(const LuaRegisterNode& right);

protected:
    void register_field(const LuaRegisterNode& head);

private:
    void next(LuaRegisterNode* n);
    const LuaRegisterNode* next() const;

protected:
    lua_State* mLua;
    LuaRef mKey;
    LuaRef mValue;
    LuaRef mParent;
    LuaRegisterNode* mNext;
};
}  // namespace PureLua
