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

#include "PureCore/ArrayRef.h"
#include "PureLua/PureLuaLib.h"
#include "PureLua/LuaUserdata.h"

#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <type_traits>
#include <memory>

namespace PureLua {
class LuaModule;
class PURELUA_API LuaSTLRegister {
    PURE_DISABLE_COPY(LuaSTLRegister)
public:
    typedef void (*RegisterType)(LuaModule&);

    LuaSTLRegister() = default;

    static LuaSTLRegister* inst();
    static void cleanup();

    void add_register(RegisterType reg);
    void try_register(LuaModule& lm);

private:
    std::vector<RegisterType> mRegister;

private:
    static LuaSTLRegister* sInst;
};

template <typename ST>
class LuaSharedPtrAdapter {
private:
    template <typename T>
    friend class LuaSTLHelper;

private:
    static void try_register(LuaModule& lm);
    static typename ST::element_type* get(ST* self);
    static bool valid(ST* self);
};

template <typename WT>
class LuaWeakPtrAdapter {
private:
    template <typename T>
    friend class LuaSTLHelper;

private:
    static void try_register(LuaModule& lm);
    static typename WT::element_type* get(WT* self);
    static bool valid(WT* self);
};

template <typename VT>
class LuaVectorAdapter {
private:
    template <typename T>
    friend class LuaSTLHelper;

private:
    static void try_register(LuaModule& lm);
    static const typename VT::value_type& get(VT* self, size_t idx);
    static int set(lua_State* L);
    static const typename VT::value_type& get_back(VT* self);
    static int set_back(lua_State* L);
    static const typename VT::value_type& get_front(VT* self);
    static int set_front(lua_State* L);
    static void insert(VT* self, size_t idx, const typename VT::value_type& value);
    static void erase(VT* self, size_t begin, size_t end);
    static PureCore::ArrayRef<typename VT::value_type> to_array_ref(VT* self);
    static int to_table(lua_State* L);
    static int from_table(lua_State* L);
};

template <typename LT>
class LuaListAdapter {
private:
    template <typename T>
    friend class LuaSTLHelper;

private:
    static void try_register(LuaModule& lm);
    static const typename LT::value_type& get_back(LT* self);
    static int set_back(lua_State* L);
    static const typename LT::value_type& get_front(LT* self);
    static int set_front(lua_State* L);
    static int to_table(lua_State* L);
    static int from_table(lua_State* L);
};

template <typename ST>
class LuaSetAdapter {
private:
    template <typename T>
    friend class LuaSTLHelper;

private:
    static void try_register(LuaModule& lm);
    static bool insert(ST* self, const typename ST::value_type& value);
    static bool exist(ST* self, const typename ST::value_type& value);
    static int to_table(lua_State* L);
    static int from_table(lua_State* L);
};

template <typename PT>
class LuaPairAdapter {
private:
    template <typename T>
    friend class LuaSTLHelper;

private:
    static void try_register(LuaModule& lm);
    static int get(lua_State* L);
    static int set(lua_State* L);
};

template <typename MT>
class LuaMapAdapter {
private:
    template <typename T>
    friend class LuaSTLHelper;

private:
    static void try_register(LuaModule& lm);
    static bool insert(MT* self, const typename MT::key_type& key, const typename MT::mapped_type& value);
    static int set(lua_State* L);
    static typename MT::mapped_type* get(MT* self, const typename MT::key_type& key);
    static bool exist(MT* self, const typename MT::key_type& key);
    static int to_table(lua_State* L);
    static int from_table(lua_State* L);
};

template <typename ART>
class LuaArrayRefAdapter {
private:
    template <typename T>
    friend class LuaSTLHelper;

private:
    static void try_register(LuaModule& lm);
    static const typename ART::ValueType& get(ART* self, size_t idx);
    static int set(lua_State* L);
    static int to_table(lua_State* L);
};

template <typename T>
class LuaSTLHelper {
public:
    LuaSTLHelper();
};
}  // namespace PureLua

#include "PureLua/LuaSTLAdapter.inl.h"
