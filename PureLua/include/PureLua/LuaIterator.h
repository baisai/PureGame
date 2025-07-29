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

#include "PureLua/LuaStack.h"
#include "PureLua/LuaRef.h"

namespace PureLua {
class PURELUA_API LuaIterator {
public:
    LuaIterator(lua_State* L, int idx);
    ~LuaIterator();

    void set_over();
    lua_State* state();
    bool is_over();

    template <typename T>
    inline bool key_valid() {
        return LuaStack<T>::valid(mLua, mKeyIdx);
    }
    template <typename T>
    inline typename LuaStack<T>::GetType get_key() {
        return LuaStack<T>::get(mLua, mKeyIdx);
    }
    template <typename T>
    inline bool value_valid() {
        return LuaStack<T>::valid(mLua, mKeyIdx + 1);
    }
    template <typename T>
    inline typename LuaStack<T>::GetType get_value() {
        return LuaStack<T>::get(mLua, mKeyIdx + 1);
    }

    inline LuaRef operator*() { return LuaRef(mLua, mKeyIdx + 1); }

    inline int key_type() { return lua_type(mLua, mKeyIdx); }
    inline int value_type() { return lua_type(mLua, mKeyIdx + 1); }

    inline int key_idx() { return mKeyIdx; }
    inline int value_idx() { return mKeyIdx + 1; }

    void push_key();
    void push_value();

    LuaIterator& operator++();
    LuaIterator& operator++(int);

private:
    void next();

private:
    lua_State* mLua;
    int mTop;
    bool mOver;
    int mKeyIdx;
};

class PURELUA_API LuaRefIterator {
public:
    explicit LuaRefIterator(const LuaRef& table);

    lua_State* state();
    bool is_over();

    LuaRef& get_key();

    LuaRef& get_value();

    LuaRef& operator*();

    LuaRef& operator->();

    LuaRefIterator& operator++();

    LuaRefIterator& operator++(int);

private:
    void next();

private:
    lua_State* mLua;
    LuaRef mTable;
    LuaRef mKey;
    LuaRef mValue;
};

}  // namespace PureLua
