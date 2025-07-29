// Copyright 2023-present ChenDong, email <baisaichen@live.com>, All rights reserved.
// this is a generate file, do't modify

#pragma once

#include "PureLua/LuaErrorDesc.h"
#include "PureLua/LuaUserdata.h"
#include "PureLua/LuaPointer.h"

#include <type_traits>

namespace PureLua {
struct PURELUA_API LuaTypeNil {};

template <typename T>
struct LuaStack;

// char
template <>
struct LuaStack<char> {
    typedef char PushType;
    typedef char GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// signed char
template <>
struct LuaStack<signed char> {
    typedef signed char PushType;
    typedef signed char GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// unsigned char
template <>
struct LuaStack<unsigned char> {
    typedef unsigned char PushType;
    typedef unsigned char GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// short
template <>
struct LuaStack<short> {
    typedef short PushType;
    typedef short GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// unsigned short
template <>
struct LuaStack<unsigned short> {
    typedef unsigned short PushType;
    typedef unsigned short GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// int
template <>
struct LuaStack<int> {
    typedef int PushType;
    typedef int GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// unsigned int
template <>
struct LuaStack<unsigned int> {
    typedef unsigned int PushType;
    typedef unsigned int GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// long
template <>
struct LuaStack<long> {
    typedef long PushType;
    typedef long GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// unsigned long
template <>
struct LuaStack<unsigned long> {
    typedef unsigned long PushType;
    typedef unsigned long GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// long long
template <>
struct LuaStack<long long> {
    typedef long long PushType;
    typedef long long GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// unsigned long long
template <>
struct LuaStack<unsigned long long> {
    typedef unsigned long long PushType;
    typedef unsigned long long GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// float
template <>
struct LuaStack<float> {
    typedef float PushType;
    typedef float GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushnumber(L, static_cast<lua_Number>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checknumber(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isnumber(L, idx); }
};
// double
template <>
struct LuaStack<double> {
    typedef double PushType;
    typedef double GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushnumber(L, static_cast<lua_Number>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checknumber(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isnumber(L, idx); }
};
// bool
template <>
struct LuaStack<bool> {
    typedef bool PushType;
    typedef bool GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushboolean(L, value ? 1 : 0); }

    static inline GetType get(lua_State *L, int idx) { return lua_toboolean(L, idx) ? true : false; }

    static inline bool valid(lua_State *L, int idx) { return lua_isboolean(L, idx); }
};
// lua_CFunction
template <>
struct LuaStack<lua_CFunction> {
    typedef lua_CFunction PushType;
    typedef lua_CFunction GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushcfunction(L, value); }

    static inline GetType get(lua_State *L, int idx) { return lua_tocfunction(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return lua_iscfunction(L, idx); }
};
// LuaTypeNil
template <>
struct LuaStack<LuaTypeNil> {
    typedef LuaTypeNil PushType;
    typedef LuaTypeNil GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushnil(L); }

    static inline GetType get(lua_State *L, int idx) { return LuaTypeNil(); }

    static inline bool valid(lua_State *L, int idx) { return lua_isnoneornil(L, idx); }
};
// const char
template <>
struct LuaStack<const char> {
    typedef char PushType;
    typedef char GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const signed char
template <>
struct LuaStack<const signed char> {
    typedef signed char PushType;
    typedef signed char GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const unsigned char
template <>
struct LuaStack<const unsigned char> {
    typedef unsigned char PushType;
    typedef unsigned char GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const short
template <>
struct LuaStack<const short> {
    typedef short PushType;
    typedef short GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const unsigned short
template <>
struct LuaStack<const unsigned short> {
    typedef unsigned short PushType;
    typedef unsigned short GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const int
template <>
struct LuaStack<const int> {
    typedef int PushType;
    typedef int GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const unsigned int
template <>
struct LuaStack<const unsigned int> {
    typedef unsigned int PushType;
    typedef unsigned int GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const long
template <>
struct LuaStack<const long> {
    typedef long PushType;
    typedef long GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const unsigned long
template <>
struct LuaStack<const unsigned long> {
    typedef unsigned long PushType;
    typedef unsigned long GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const long long
template <>
struct LuaStack<const long long> {
    typedef long long PushType;
    typedef long long GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const unsigned long long
template <>
struct LuaStack<const unsigned long long> {
    typedef unsigned long long PushType;
    typedef unsigned long long GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const float
template <>
struct LuaStack<const float> {
    typedef float PushType;
    typedef float GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushnumber(L, static_cast<lua_Number>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checknumber(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isnumber(L, idx); }
};
// const double
template <>
struct LuaStack<const double> {
    typedef double PushType;
    typedef double GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushnumber(L, static_cast<lua_Number>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checknumber(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isnumber(L, idx); }
};
// const bool
template <>
struct LuaStack<const bool> {
    typedef bool PushType;
    typedef bool GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushboolean(L, value ? 1 : 0); }

    static inline GetType get(lua_State *L, int idx) { return lua_toboolean(L, idx) ? true : false; }

    static inline bool valid(lua_State *L, int idx) { return lua_isboolean(L, idx); }
};
// const lua_CFunction
template <>
struct LuaStack<const lua_CFunction> {
    typedef lua_CFunction PushType;
    typedef lua_CFunction GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushcfunction(L, value); }

    static inline GetType get(lua_State *L, int idx) { return lua_tocfunction(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return lua_iscfunction(L, idx); }
};
// const LuaTypeNil
template <>
struct LuaStack<const LuaTypeNil> {
    typedef LuaTypeNil PushType;
    typedef LuaTypeNil GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushnil(L); }

    static inline GetType get(lua_State *L, int idx) { return LuaTypeNil(); }

    static inline bool valid(lua_State *L, int idx) { return lua_isnoneornil(L, idx); }
};
// char &
template <>
struct LuaStack<char &> {
    typedef char &PushType;
    typedef char &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<char> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<char> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<char> >::is_class_type(L, idx); }
};
// signed char &
template <>
struct LuaStack<signed char &> {
    typedef signed char &PushType;
    typedef signed char &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<signed char> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<signed char> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<signed char> >::is_class_type(L, idx); }
};
// unsigned char &
template <>
struct LuaStack<unsigned char &> {
    typedef unsigned char &PushType;
    typedef unsigned char &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned char> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<unsigned char> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned char> >::is_class_type(L, idx); }
};
// short &
template <>
struct LuaStack<short &> {
    typedef short &PushType;
    typedef short &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<short> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<short> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<short> >::is_class_type(L, idx); }
};
// unsigned short &
template <>
struct LuaStack<unsigned short &> {
    typedef unsigned short &PushType;
    typedef unsigned short &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned short> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<unsigned short> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned short> >::is_class_type(L, idx); }
};
// int &
template <>
struct LuaStack<int &> {
    typedef int &PushType;
    typedef int &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<int> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<int> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<int> >::is_class_type(L, idx); }
};
// unsigned int &
template <>
struct LuaStack<unsigned int &> {
    typedef unsigned int &PushType;
    typedef unsigned int &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned int> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<unsigned int> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned int> >::is_class_type(L, idx); }
};
// long &
template <>
struct LuaStack<long &> {
    typedef long &PushType;
    typedef long &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<long> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<long> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<long> >::is_class_type(L, idx); }
};
// unsigned long &
template <>
struct LuaStack<unsigned long &> {
    typedef unsigned long &PushType;
    typedef unsigned long &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned long> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<unsigned long> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned long> >::is_class_type(L, idx); }
};
// long long &
template <>
struct LuaStack<long long &> {
    typedef long long &PushType;
    typedef long long &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<long long> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<long long> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<long long> >::is_class_type(L, idx); }
};
// unsigned long long &
template <>
struct LuaStack<unsigned long long &> {
    typedef unsigned long long &PushType;
    typedef unsigned long long &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned long long> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<unsigned long long> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned long long> >::is_class_type(L, idx); }
};
// float &
template <>
struct LuaStack<float &> {
    typedef float &PushType;
    typedef float &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<float> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<float> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<float> >::is_class_type(L, idx); }
};
// double &
template <>
struct LuaStack<double &> {
    typedef double &PushType;
    typedef double &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<double> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<double> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<double> >::is_class_type(L, idx); }
};
// bool &
template <>
struct LuaStack<bool &> {
    typedef bool &PushType;
    typedef bool &GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<bool> >(L, &value); }

    static inline GetType get(lua_State *L, int idx) { return LuaUserdataPtr::get<LuaPointer<bool> >(L, idx)->get_ref(); }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<bool> >::is_class_type(L, idx); }
};
// const char &
template <>
struct LuaStack<const char &> {
    typedef char PushType;
    typedef char GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const signed char &
template <>
struct LuaStack<const signed char &> {
    typedef signed char PushType;
    typedef signed char GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const unsigned char &
template <>
struct LuaStack<const unsigned char &> {
    typedef unsigned char PushType;
    typedef unsigned char GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const short &
template <>
struct LuaStack<const short &> {
    typedef short PushType;
    typedef short GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const unsigned short &
template <>
struct LuaStack<const unsigned short &> {
    typedef unsigned short PushType;
    typedef unsigned short GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const int &
template <>
struct LuaStack<const int &> {
    typedef int PushType;
    typedef int GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const unsigned int &
template <>
struct LuaStack<const unsigned int &> {
    typedef unsigned int PushType;
    typedef unsigned int GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const long &
template <>
struct LuaStack<const long &> {
    typedef long PushType;
    typedef long GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const unsigned long &
template <>
struct LuaStack<const unsigned long &> {
    typedef unsigned long PushType;
    typedef unsigned long GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const long long &
template <>
struct LuaStack<const long long &> {
    typedef long long PushType;
    typedef long long GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const unsigned long long &
template <>
struct LuaStack<const unsigned long long &> {
    typedef unsigned long long PushType;
    typedef unsigned long long GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checkinteger(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isinteger(L, idx); }
};
// const float &
template <>
struct LuaStack<const float &> {
    typedef float PushType;
    typedef float GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushnumber(L, static_cast<lua_Number>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checknumber(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isnumber(L, idx); }
};
// const double &
template <>
struct LuaStack<const double &> {
    typedef double PushType;
    typedef double GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushnumber(L, static_cast<lua_Number>(value)); }

    static inline GetType get(lua_State *L, int idx) { return static_cast<GetType>(luaL_checknumber(L, idx)); }

    static inline bool valid(lua_State *L, int idx) { return lua_isnumber(L, idx); }
};
// const bool &
template <>
struct LuaStack<const bool &> {
    typedef bool PushType;
    typedef bool GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushboolean(L, value ? 1 : 0); }

    static inline GetType get(lua_State *L, int idx) { return lua_toboolean(L, idx) ? true : false; }

    static inline bool valid(lua_State *L, int idx) { return lua_isboolean(L, idx); }
};
// const lua_CFunction &
template <>
struct LuaStack<const lua_CFunction &> {
    typedef lua_CFunction PushType;
    typedef lua_CFunction GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushcfunction(L, value); }

    static inline GetType get(lua_State *L, int idx) { return lua_tocfunction(L, idx); }

    static inline bool valid(lua_State *L, int idx) { return lua_iscfunction(L, idx); }
};
// const LuaTypeNil &
template <>
struct LuaStack<const LuaTypeNil &> {
    typedef LuaTypeNil PushType;
    typedef LuaTypeNil GetType;
    static inline void push(lua_State *L, PushType value) { lua_pushnil(L); }

    static inline GetType get(lua_State *L, int idx) { return LuaTypeNil(); }

    static inline bool valid(lua_State *L, int idx) { return lua_isnoneornil(L, idx); }
};
// signed char *
template <>
struct LuaStack<signed char *> {
    typedef signed char *PushType;
    typedef signed char *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<signed char> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<signed char> *lp = LuaUserdataPtr::get<LuaPointer<signed char> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<signed char> >::is_class_type(L, idx); }
};
// unsigned char *
template <>
struct LuaStack<unsigned char *> {
    typedef unsigned char *PushType;
    typedef unsigned char *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned char> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned char> *lp = LuaUserdataPtr::get<LuaPointer<unsigned char> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned char> >::is_class_type(L, idx); }
};
// short *
template <>
struct LuaStack<short *> {
    typedef short *PushType;
    typedef short *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<short> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<short> *lp = LuaUserdataPtr::get<LuaPointer<short> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<short> >::is_class_type(L, idx); }
};
// unsigned short *
template <>
struct LuaStack<unsigned short *> {
    typedef unsigned short *PushType;
    typedef unsigned short *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned short> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned short> *lp = LuaUserdataPtr::get<LuaPointer<unsigned short> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned short> >::is_class_type(L, idx); }
};
// int *
template <>
struct LuaStack<int *> {
    typedef int *PushType;
    typedef int *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<int> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<int> *lp = LuaUserdataPtr::get<LuaPointer<int> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<int> >::is_class_type(L, idx); }
};
// unsigned int *
template <>
struct LuaStack<unsigned int *> {
    typedef unsigned int *PushType;
    typedef unsigned int *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned int> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned int> *lp = LuaUserdataPtr::get<LuaPointer<unsigned int> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned int> >::is_class_type(L, idx); }
};
// long *
template <>
struct LuaStack<long *> {
    typedef long *PushType;
    typedef long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<long> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<long> *lp = LuaUserdataPtr::get<LuaPointer<long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<long> >::is_class_type(L, idx); }
};
// unsigned long *
template <>
struct LuaStack<unsigned long *> {
    typedef unsigned long *PushType;
    typedef unsigned long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned long> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned long> *lp = LuaUserdataPtr::get<LuaPointer<unsigned long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned long> >::is_class_type(L, idx); }
};
// long long *
template <>
struct LuaStack<long long *> {
    typedef long long *PushType;
    typedef long long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<long long> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<long long> *lp = LuaUserdataPtr::get<LuaPointer<long long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<long long> >::is_class_type(L, idx); }
};
// unsigned long long *
template <>
struct LuaStack<unsigned long long *> {
    typedef unsigned long long *PushType;
    typedef unsigned long long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned long long> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned long long> *lp = LuaUserdataPtr::get<LuaPointer<unsigned long long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned long long> >::is_class_type(L, idx); }
};
// float *
template <>
struct LuaStack<float *> {
    typedef float *PushType;
    typedef float *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<float> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<float> *lp = LuaUserdataPtr::get<LuaPointer<float> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<float> >::is_class_type(L, idx); }
};
// double *
template <>
struct LuaStack<double *> {
    typedef double *PushType;
    typedef double *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<double> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<double> *lp = LuaUserdataPtr::get<LuaPointer<double> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<double> >::is_class_type(L, idx); }
};
// bool *
template <>
struct LuaStack<bool *> {
    typedef bool *PushType;
    typedef bool *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<bool> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<bool> *lp = LuaUserdataPtr::get<LuaPointer<bool> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<bool> >::is_class_type(L, idx); }
};
// signed char * &
template <>
struct LuaStack<signed char *&> {
    typedef signed char *PushType;
    typedef signed char *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<signed char> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<signed char> *lp = LuaUserdataPtr::get<LuaPointer<signed char> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<signed char> >::is_class_type(L, idx); }
};
// unsigned char * &
template <>
struct LuaStack<unsigned char *&> {
    typedef unsigned char *PushType;
    typedef unsigned char *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned char> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned char> *lp = LuaUserdataPtr::get<LuaPointer<unsigned char> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned char> >::is_class_type(L, idx); }
};
// short * &
template <>
struct LuaStack<short *&> {
    typedef short *PushType;
    typedef short *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<short> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<short> *lp = LuaUserdataPtr::get<LuaPointer<short> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<short> >::is_class_type(L, idx); }
};
// unsigned short * &
template <>
struct LuaStack<unsigned short *&> {
    typedef unsigned short *PushType;
    typedef unsigned short *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned short> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned short> *lp = LuaUserdataPtr::get<LuaPointer<unsigned short> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned short> >::is_class_type(L, idx); }
};
// int * &
template <>
struct LuaStack<int *&> {
    typedef int *PushType;
    typedef int *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<int> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<int> *lp = LuaUserdataPtr::get<LuaPointer<int> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<int> >::is_class_type(L, idx); }
};
// unsigned int * &
template <>
struct LuaStack<unsigned int *&> {
    typedef unsigned int *PushType;
    typedef unsigned int *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned int> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned int> *lp = LuaUserdataPtr::get<LuaPointer<unsigned int> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned int> >::is_class_type(L, idx); }
};
// long * &
template <>
struct LuaStack<long *&> {
    typedef long *PushType;
    typedef long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<long> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<long> *lp = LuaUserdataPtr::get<LuaPointer<long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<long> >::is_class_type(L, idx); }
};
// unsigned long * &
template <>
struct LuaStack<unsigned long *&> {
    typedef unsigned long *PushType;
    typedef unsigned long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned long> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned long> *lp = LuaUserdataPtr::get<LuaPointer<unsigned long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned long> >::is_class_type(L, idx); }
};
// long long * &
template <>
struct LuaStack<long long *&> {
    typedef long long *PushType;
    typedef long long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<long long> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<long long> *lp = LuaUserdataPtr::get<LuaPointer<long long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<long long> >::is_class_type(L, idx); }
};
// unsigned long long * &
template <>
struct LuaStack<unsigned long long *&> {
    typedef unsigned long long *PushType;
    typedef unsigned long long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned long long> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned long long> *lp = LuaUserdataPtr::get<LuaPointer<unsigned long long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned long long> >::is_class_type(L, idx); }
};
// float * &
template <>
struct LuaStack<float *&> {
    typedef float *PushType;
    typedef float *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<float> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<float> *lp = LuaUserdataPtr::get<LuaPointer<float> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<float> >::is_class_type(L, idx); }
};
// double * &
template <>
struct LuaStack<double *&> {
    typedef double *PushType;
    typedef double *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<double> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<double> *lp = LuaUserdataPtr::get<LuaPointer<double> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<double> >::is_class_type(L, idx); }
};
// bool * &
template <>
struct LuaStack<bool *&> {
    typedef bool *PushType;
    typedef bool *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<bool> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<bool> *lp = LuaUserdataPtr::get<LuaPointer<bool> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<bool> >::is_class_type(L, idx); }
};
// const signed char *
template <>
struct LuaStack<const signed char *> {
    typedef const signed char *PushType;
    typedef const signed char *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<signed char> >(L, const_cast<signed char *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<signed char> *lp = LuaUserdataPtr::get<LuaPointer<signed char> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<signed char> >::is_class_type(L, idx); }
};
// const unsigned char *
template <>
struct LuaStack<const unsigned char *> {
    typedef const unsigned char *PushType;
    typedef const unsigned char *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned char> >(L, const_cast<unsigned char *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned char> *lp = LuaUserdataPtr::get<LuaPointer<unsigned char> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned char> >::is_class_type(L, idx); }
};
// const short *
template <>
struct LuaStack<const short *> {
    typedef const short *PushType;
    typedef const short *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<short> >(L, const_cast<short *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<short> *lp = LuaUserdataPtr::get<LuaPointer<short> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<short> >::is_class_type(L, idx); }
};
// const unsigned short *
template <>
struct LuaStack<const unsigned short *> {
    typedef const unsigned short *PushType;
    typedef const unsigned short *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned short> >(L, const_cast<unsigned short *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned short> *lp = LuaUserdataPtr::get<LuaPointer<unsigned short> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned short> >::is_class_type(L, idx); }
};
// const int *
template <>
struct LuaStack<const int *> {
    typedef const int *PushType;
    typedef const int *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<int> >(L, const_cast<int *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<int> *lp = LuaUserdataPtr::get<LuaPointer<int> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<int> >::is_class_type(L, idx); }
};
// const unsigned int *
template <>
struct LuaStack<const unsigned int *> {
    typedef const unsigned int *PushType;
    typedef const unsigned int *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned int> >(L, const_cast<unsigned int *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned int> *lp = LuaUserdataPtr::get<LuaPointer<unsigned int> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned int> >::is_class_type(L, idx); }
};
// const long *
template <>
struct LuaStack<const long *> {
    typedef const long *PushType;
    typedef const long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<long> >(L, const_cast<long *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<long> *lp = LuaUserdataPtr::get<LuaPointer<long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<long> >::is_class_type(L, idx); }
};
// const unsigned long *
template <>
struct LuaStack<const unsigned long *> {
    typedef const unsigned long *PushType;
    typedef const unsigned long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned long> >(L, const_cast<unsigned long *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned long> *lp = LuaUserdataPtr::get<LuaPointer<unsigned long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned long> >::is_class_type(L, idx); }
};
// const long long *
template <>
struct LuaStack<const long long *> {
    typedef const long long *PushType;
    typedef const long long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<long long> >(L, const_cast<long long *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<long long> *lp = LuaUserdataPtr::get<LuaPointer<long long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<long long> >::is_class_type(L, idx); }
};
// const unsigned long long *
template <>
struct LuaStack<const unsigned long long *> {
    typedef const unsigned long long *PushType;
    typedef const unsigned long long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned long long> >(L, const_cast<unsigned long long *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned long long> *lp = LuaUserdataPtr::get<LuaPointer<unsigned long long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned long long> >::is_class_type(L, idx); }
};
// const float *
template <>
struct LuaStack<const float *> {
    typedef const float *PushType;
    typedef const float *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<float> >(L, const_cast<float *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<float> *lp = LuaUserdataPtr::get<LuaPointer<float> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<float> >::is_class_type(L, idx); }
};
// const double *
template <>
struct LuaStack<const double *> {
    typedef const double *PushType;
    typedef const double *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<double> >(L, const_cast<double *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<double> *lp = LuaUserdataPtr::get<LuaPointer<double> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<double> >::is_class_type(L, idx); }
};
// const bool *
template <>
struct LuaStack<const bool *> {
    typedef const bool *PushType;
    typedef const bool *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<bool> >(L, const_cast<bool *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<bool> *lp = LuaUserdataPtr::get<LuaPointer<bool> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<bool> >::is_class_type(L, idx); }
};
// const signed char * &
template <>
struct LuaStack<const signed char *&> {
    typedef const signed char *PushType;
    typedef const signed char *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<signed char> >(L, const_cast<signed char *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<signed char> *lp = LuaUserdataPtr::get<LuaPointer<signed char> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<signed char> >::is_class_type(L, idx); }
};
// const unsigned char * &
template <>
struct LuaStack<const unsigned char *&> {
    typedef const unsigned char *PushType;
    typedef const unsigned char *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned char> >(L, const_cast<unsigned char *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned char> *lp = LuaUserdataPtr::get<LuaPointer<unsigned char> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned char> >::is_class_type(L, idx); }
};
// const short * &
template <>
struct LuaStack<const short *&> {
    typedef const short *PushType;
    typedef const short *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<short> >(L, const_cast<short *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<short> *lp = LuaUserdataPtr::get<LuaPointer<short> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<short> >::is_class_type(L, idx); }
};
// const unsigned short * &
template <>
struct LuaStack<const unsigned short *&> {
    typedef const unsigned short *PushType;
    typedef const unsigned short *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned short> >(L, const_cast<unsigned short *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned short> *lp = LuaUserdataPtr::get<LuaPointer<unsigned short> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned short> >::is_class_type(L, idx); }
};
// const int * &
template <>
struct LuaStack<const int *&> {
    typedef const int *PushType;
    typedef const int *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<int> >(L, const_cast<int *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<int> *lp = LuaUserdataPtr::get<LuaPointer<int> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<int> >::is_class_type(L, idx); }
};
// const unsigned int * &
template <>
struct LuaStack<const unsigned int *&> {
    typedef const unsigned int *PushType;
    typedef const unsigned int *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned int> >(L, const_cast<unsigned int *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned int> *lp = LuaUserdataPtr::get<LuaPointer<unsigned int> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned int> >::is_class_type(L, idx); }
};
// const long * &
template <>
struct LuaStack<const long *&> {
    typedef const long *PushType;
    typedef const long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<long> >(L, const_cast<long *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<long> *lp = LuaUserdataPtr::get<LuaPointer<long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<long> >::is_class_type(L, idx); }
};
// const unsigned long * &
template <>
struct LuaStack<const unsigned long *&> {
    typedef const unsigned long *PushType;
    typedef const unsigned long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned long> >(L, const_cast<unsigned long *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned long> *lp = LuaUserdataPtr::get<LuaPointer<unsigned long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned long> >::is_class_type(L, idx); }
};
// const long long * &
template <>
struct LuaStack<const long long *&> {
    typedef const long long *PushType;
    typedef const long long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<long long> >(L, const_cast<long long *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<long long> *lp = LuaUserdataPtr::get<LuaPointer<long long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<long long> >::is_class_type(L, idx); }
};
// const unsigned long long * &
template <>
struct LuaStack<const unsigned long long *&> {
    typedef const unsigned long long *PushType;
    typedef const unsigned long long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned long long> >(L, const_cast<unsigned long long *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned long long> *lp = LuaUserdataPtr::get<LuaPointer<unsigned long long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned long long> >::is_class_type(L, idx); }
};
// const float * &
template <>
struct LuaStack<const float *&> {
    typedef const float *PushType;
    typedef const float *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<float> >(L, const_cast<float *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<float> *lp = LuaUserdataPtr::get<LuaPointer<float> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<float> >::is_class_type(L, idx); }
};
// const double * &
template <>
struct LuaStack<const double *&> {
    typedef const double *PushType;
    typedef const double *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<double> >(L, const_cast<double *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<double> *lp = LuaUserdataPtr::get<LuaPointer<double> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<double> >::is_class_type(L, idx); }
};
// const bool * &
template <>
struct LuaStack<const bool *&> {
    typedef const bool *PushType;
    typedef const bool *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<bool> >(L, const_cast<bool *>(value)); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<bool> *lp = LuaUserdataPtr::get<LuaPointer<bool> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<bool> >::is_class_type(L, idx); }
};
// signed char * const &
template <>
struct LuaStack<signed char *const &> {
    typedef signed char *PushType;
    typedef signed char *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<signed char> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<signed char> *lp = LuaUserdataPtr::get<LuaPointer<signed char> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<signed char> >::is_class_type(L, idx); }
};
// unsigned char * const &
template <>
struct LuaStack<unsigned char *const &> {
    typedef unsigned char *PushType;
    typedef unsigned char *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned char> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned char> *lp = LuaUserdataPtr::get<LuaPointer<unsigned char> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned char> >::is_class_type(L, idx); }
};
// short * const &
template <>
struct LuaStack<short *const &> {
    typedef short *PushType;
    typedef short *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<short> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<short> *lp = LuaUserdataPtr::get<LuaPointer<short> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<short> >::is_class_type(L, idx); }
};
// unsigned short * const &
template <>
struct LuaStack<unsigned short *const &> {
    typedef unsigned short *PushType;
    typedef unsigned short *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned short> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned short> *lp = LuaUserdataPtr::get<LuaPointer<unsigned short> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned short> >::is_class_type(L, idx); }
};
// int * const &
template <>
struct LuaStack<int *const &> {
    typedef int *PushType;
    typedef int *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<int> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<int> *lp = LuaUserdataPtr::get<LuaPointer<int> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<int> >::is_class_type(L, idx); }
};
// unsigned int * const &
template <>
struct LuaStack<unsigned int *const &> {
    typedef unsigned int *PushType;
    typedef unsigned int *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned int> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned int> *lp = LuaUserdataPtr::get<LuaPointer<unsigned int> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned int> >::is_class_type(L, idx); }
};
// long * const &
template <>
struct LuaStack<long *const &> {
    typedef long *PushType;
    typedef long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<long> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<long> *lp = LuaUserdataPtr::get<LuaPointer<long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<long> >::is_class_type(L, idx); }
};
// unsigned long * const &
template <>
struct LuaStack<unsigned long *const &> {
    typedef unsigned long *PushType;
    typedef unsigned long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned long> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned long> *lp = LuaUserdataPtr::get<LuaPointer<unsigned long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned long> >::is_class_type(L, idx); }
};
// long long * const &
template <>
struct LuaStack<long long *const &> {
    typedef long long *PushType;
    typedef long long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<long long> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<long long> *lp = LuaUserdataPtr::get<LuaPointer<long long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<long long> >::is_class_type(L, idx); }
};
// unsigned long long * const &
template <>
struct LuaStack<unsigned long long *const &> {
    typedef unsigned long long *PushType;
    typedef unsigned long long *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<unsigned long long> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<unsigned long long> *lp = LuaUserdataPtr::get<LuaPointer<unsigned long long> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<unsigned long long> >::is_class_type(L, idx); }
};
// float * const &
template <>
struct LuaStack<float *const &> {
    typedef float *PushType;
    typedef float *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<float> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<float> *lp = LuaUserdataPtr::get<LuaPointer<float> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<float> >::is_class_type(L, idx); }
};
// double * const &
template <>
struct LuaStack<double *const &> {
    typedef double *PushType;
    typedef double *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<double> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<double> *lp = LuaUserdataPtr::get<LuaPointer<double> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<double> >::is_class_type(L, idx); }
};
// bool * const &
template <>
struct LuaStack<bool *const &> {
    typedef bool *PushType;
    typedef bool *GetType;
    static inline void push(lua_State *L, PushType value) { LuaUserdata::push<LuaPointer<bool> >(L, value); }

    static inline GetType get(lua_State *L, int idx) {
        LuaPointer<bool> *lp = LuaUserdataPtr::get<LuaPointer<bool> >(L, idx);
        return lp ? lp->get_ptr() : nullptr;
        ;
    }

    static inline bool valid(lua_State *L, int idx) { return LuaMetatableTraits<LuaPointer<bool> >::is_class_type(L, idx); }
};
}  // namespace PureLua