-- Copyright 2023-2024 ChenDong, email <baisaichen@live.com>, All rights reserved.
local type_str = {
	--------------------------number---------------------------------
	{
		key = "char",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "char",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "char",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "signed char",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "signed char",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "signed char",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "unsigned char",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned char",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned char",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "short",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "short",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "short",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "unsigned short",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned short",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned short",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "int",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "int",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "int",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "unsigned int",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned int",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned int",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "long",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "long",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "long",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "unsigned long",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned long",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned long",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "long long",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "long long",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "long long",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "unsigned long long",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned long long",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned long long",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "float",
		push = "lua_pushnumber(L, static_cast<lua_Number>(value))",
		PushType = "float",
		get = "return static_cast<GetType>(luaL_checknumber(L, idx))",
		GetType = "float",
		valid = "return lua_isnumber(L, idx)"
	},
	{
		key = "double",
		push = "lua_pushnumber(L, static_cast<lua_Number>(value))",
		PushType = "double",
		get = "return static_cast<GetType>(luaL_checknumber(L, idx))",
		GetType = "double",
		valid = "return lua_isnumber(L, idx)"
	},
	{
		key = "bool",
		push = "lua_pushboolean(L, value ? 1 : 0)",
		PushType = "bool",
		get = "return lua_toboolean(L, idx) ? true : false",
		GetType = "bool",
		valid = "return lua_isboolean(L, idx)"
	},
	{
		key = "lua_CFunction",
		push = "lua_pushcfunction(L, value)",
		PushType = "lua_CFunction",
		get = "return lua_tocfunction(L, idx)",
		GetType = "lua_CFunction",
		valid = "return lua_iscfunction(L, idx)"
	},
	{
		key = "LuaTypeNil",
		push = "lua_pushnil(L)",
		PushType = "LuaTypeNil",
		get = "return LuaTypeNil()",
		GetType = "LuaTypeNil",
		valid = "return lua_isnoneornil(L, idx)"
	},

	-----------------------------const number--------------------------------------
	{
		key = "const char",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "char",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "char",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const signed char",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "signed char",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "signed char",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const unsigned char",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned char",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned char",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const short",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "short",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "short",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const unsigned short",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned short",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned short",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const int",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "int",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "int",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const unsigned int",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned int",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned int",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const long",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "long",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "long",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const unsigned long",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned long",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned long",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const long long",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "long long",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "long long",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const unsigned long long",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned long long",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned long long",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const float",
		push = "lua_pushnumber(L, static_cast<lua_Number>(value))",
		PushType = "float",
		get = "return static_cast<GetType>(luaL_checknumber(L, idx))",
		GetType = "float",
		valid = "return lua_isnumber(L, idx)"
	},
	{
		key = "const double",
		push = "lua_pushnumber(L, static_cast<lua_Number>(value))",
		PushType = "double",
		get = "return static_cast<GetType>(luaL_checknumber(L, idx))",
		GetType = "double",
		valid = "return lua_isnumber(L, idx)"
	},
	{
		key = "const bool",
		push = "lua_pushboolean(L, value ? 1 : 0)",
		PushType = "bool",
		get = "return lua_toboolean(L, idx) ? true : false",
		GetType = "bool",
		valid = "return lua_isboolean(L, idx)"
	},
	{
		key = "const lua_CFunction",
		push = "lua_pushcfunction(L, value)",
		PushType = "lua_CFunction",
		get = "return lua_tocfunction(L, idx)",
		GetType = "lua_CFunction",
		valid = "return lua_iscfunction(L, idx)"
	},
	{
		key = "const LuaTypeNil",
		push = "lua_pushnil(L)",
		PushType = "LuaTypeNil",
		get = "return LuaTypeNil()",
		GetType = "LuaTypeNil",
		valid = "return lua_isnoneornil(L, idx)"
	},

	--------------------------number &---------------------------------
	{
		key = "char &",
		push = "LuaUserdata::push<LuaPointer<char> >(L, &value)",
		PushType = "char &",
		get = "return LuaUserdataPtr::get<LuaPointer<char> >(L, idx)->get_ref()",
		GetType = "char &",
		valid = "return LuaMetatableTraits<LuaPointer<char> >::is_class_type(L, idx)"
	},
	{
		key = "signed char &",
		push = "LuaUserdata::push<LuaPointer<signed char> >(L, &value)",
		PushType = "signed char &",
		get = "return LuaUserdataPtr::get<LuaPointer<signed char> >(L, idx)->get_ref()",
		GetType = "signed char &",
		valid = "return LuaMetatableTraits<LuaPointer<signed char> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned char &",
		push = "LuaUserdata::push<LuaPointer<unsigned char> >(L, &value)",
		PushType = "unsigned char &",
		get = "return LuaUserdataPtr::get<LuaPointer<unsigned char> >(L, idx)->get_ref()",
		GetType = "unsigned char &",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned char> >::is_class_type(L, idx)"
	},
	{
		key = "short &",
		push = "LuaUserdata::push<LuaPointer<short> >(L, &value)",
		PushType = "short &",
		get = "return LuaUserdataPtr::get<LuaPointer<short> >(L, idx)->get_ref()",
		GetType = "short &",
		valid = "return LuaMetatableTraits<LuaPointer<short> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned short &",
		push = "LuaUserdata::push<LuaPointer<unsigned short> >(L, &value)",
		PushType = "unsigned short &",
		get = "return LuaUserdataPtr::get<LuaPointer<unsigned short> >(L, idx)->get_ref()",
		GetType = "unsigned short &",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned short> >::is_class_type(L, idx)"
	},
	{
		key = "int &",
		push = "LuaUserdata::push<LuaPointer<int> >(L, &value)",
		PushType = "int &",
		get = "return LuaUserdataPtr::get<LuaPointer<int> >(L, idx)->get_ref()",
		GetType = "int &",
		valid = "return LuaMetatableTraits<LuaPointer<int> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned int &",
		push = "LuaUserdata::push<LuaPointer<unsigned int> >(L, &value)",
		PushType = "unsigned int &",
		get = "return LuaUserdataPtr::get<LuaPointer<unsigned int> >(L, idx)->get_ref()",
		GetType = "unsigned int &",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned int> >::is_class_type(L, idx)"
	},
	{
		key = "long &",
		push = "LuaUserdata::push<LuaPointer<long> >(L, &value)",
		PushType = "long &",
		get = "return LuaUserdataPtr::get<LuaPointer<long> >(L, idx)->get_ref()",
		GetType = "long &",
		valid = "return LuaMetatableTraits<LuaPointer<long> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned long &",
		push = "LuaUserdata::push<LuaPointer<unsigned long> >(L, &value)",
		PushType = "unsigned long &",
		get = "return LuaUserdataPtr::get<LuaPointer<unsigned long> >(L, idx)->get_ref()",
		GetType = "unsigned long &",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned long> >::is_class_type(L, idx)"
	},
	{
		key = "long long &",
		push = "LuaUserdata::push<LuaPointer<long long> >(L, &value)",
		PushType = "long long &",
		get = "return LuaUserdataPtr::get<LuaPointer<long long> >(L, idx)->get_ref()",
		GetType = "long long &",
		valid = "return LuaMetatableTraits<LuaPointer<long long> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned long long &",
		push = "LuaUserdata::push<LuaPointer<unsigned long long> >(L, &value)",
		PushType = "unsigned long long &",
		get = "return LuaUserdataPtr::get<LuaPointer<unsigned long long> >(L, idx)->get_ref()",
		GetType = "unsigned long long &",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned long long> >::is_class_type(L, idx)"
	},
	{
		key = "float &",
		push = "LuaUserdata::push<LuaPointer<float> >(L, &value)",
		PushType = "float &",
		get = "return LuaUserdataPtr::get<LuaPointer<float> >(L, idx)->get_ref()",
		GetType = "float &",
		valid = "return LuaMetatableTraits<LuaPointer<float> >::is_class_type(L, idx)"
	},
	{
		key = "double &",
		push = "LuaUserdata::push<LuaPointer<double> >(L, &value)",
		PushType = "double &",
		get = "return LuaUserdataPtr::get<LuaPointer<double> >(L, idx)->get_ref()",
		GetType = "double &",
		valid = "return LuaMetatableTraits<LuaPointer<double> >::is_class_type(L, idx)"
	},
	{
		key = "bool &",
		push = "LuaUserdata::push<LuaPointer<bool> >(L, &value)",
		PushType = "bool &",
		get = "return LuaUserdataPtr::get<LuaPointer<bool> >(L, idx)->get_ref()",
		GetType = "bool &",
		valid = "return LuaMetatableTraits<LuaPointer<bool> >::is_class_type(L, idx)"
	},

	-----------------------------const number &--------------------------------------
	{
		key = "const char &",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "char",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "char",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const signed char &",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "signed char",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "signed char",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const unsigned char &",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned char",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned char",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const short &",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "short",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "short",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const unsigned short &",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned short",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned short",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const int &",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "int",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "int",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const unsigned int &",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned int",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned int",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const long &",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "long",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "long",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const unsigned long &",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned long",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned long",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const long long &",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "long long",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "long long",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const unsigned long long &",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "unsigned long long",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "unsigned long long",
		valid = "return lua_isinteger(L, idx)"
	},
	{
		key = "const float &",
		push = "lua_pushnumber(L, static_cast<lua_Number>(value))",
		PushType = "float",
		get = "return static_cast<GetType>(luaL_checknumber(L, idx))",
		GetType = "float",
		valid = "return lua_isnumber(L, idx)"
	},
	{
		key = "const double &",
		push = "lua_pushnumber(L, static_cast<lua_Number>(value))",
		PushType = "double",
		get = "return static_cast<GetType>(luaL_checknumber(L, idx))",
		GetType = "double",
		valid = "return lua_isnumber(L, idx)"
	},
	{
		key = "const bool &",
		push = "lua_pushboolean(L, value ? 1 : 0)",
		PushType = "bool",
		get = "return lua_toboolean(L, idx) ? true : false",
		GetType = "bool",
		valid = "return lua_isboolean(L, idx)"
	},
	{
		key = "const lua_CFunction &",
		push = "lua_pushcfunction(L, value)",
		PushType = "lua_CFunction",
		get = "return lua_tocfunction(L, idx)",
		GetType = "lua_CFunction",
		valid = "return lua_iscfunction(L, idx)"
	},
	{
		key = "const LuaTypeNil &",
		push = "lua_pushnil(L)",
		PushType = "LuaTypeNil",
		get = "return LuaTypeNil()",
		GetType = "LuaTypeNil",
		valid = "return lua_isnoneornil(L, idx)"
	},

	--------------------------number *---------------------------------
	{
		key = "signed char *",
		push = "LuaUserdata::push<LuaPointer<signed char> >(L, value)",
		PushType = "signed char *",
		get = "LuaPointer<signed char> * lp = LuaUserdataPtr::get<LuaPointer<signed char> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "signed char *",
		valid = "return LuaMetatableTraits<LuaPointer<signed char> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned char *",
		push = "LuaUserdata::push<LuaPointer<unsigned char> >(L, value)",
		PushType = "unsigned char *",
		get = "LuaPointer<unsigned char> * lp = LuaUserdataPtr::get<LuaPointer<unsigned char> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned char *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned char> >::is_class_type(L, idx)"
	},
	{
		key = "short *",
		push = "LuaUserdata::push<LuaPointer<short> >(L, value)",
		PushType = "short *",
		get = "LuaPointer<short> * lp = LuaUserdataPtr::get<LuaPointer<short> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "short *",
		valid = "return LuaMetatableTraits<LuaPointer<short> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned short *",
		push = "LuaUserdata::push<LuaPointer<unsigned short> >(L, value)",
		PushType = "unsigned short *",
		get = "LuaPointer<unsigned short> * lp = LuaUserdataPtr::get<LuaPointer<unsigned short> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned short *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned short> >::is_class_type(L, idx)"
	},
	{
		key = "int *",
		push = "LuaUserdata::push<LuaPointer<int> >(L, value)",
		PushType = "int *",
		get = "LuaPointer<int> * lp = LuaUserdataPtr::get<LuaPointer<int> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "int *",
		valid = "return LuaMetatableTraits<LuaPointer<int> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned int *",
		push = "LuaUserdata::push<LuaPointer<unsigned int> >(L, value)",
		PushType = "unsigned int *",
		get = "LuaPointer<unsigned int> * lp = LuaUserdataPtr::get<LuaPointer<unsigned int> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned int *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned int> >::is_class_type(L, idx)"
	},
	{
		key = "long *",
		push = "LuaUserdata::push<LuaPointer<long> >(L, value)",
		PushType = "long *",
		get = "LuaPointer<long> * lp = LuaUserdataPtr::get<LuaPointer<long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "long *",
		valid = "return LuaMetatableTraits<LuaPointer<long> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned long *",
		push = "LuaUserdata::push<LuaPointer<unsigned long> >(L, value)",
		PushType = "unsigned long *",
		get = "LuaPointer<unsigned long> * lp = LuaUserdataPtr::get<LuaPointer<unsigned long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned long *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned long> >::is_class_type(L, idx)"
	},
	{
		key = "long long *",
		push = "LuaUserdata::push<LuaPointer<long long> >(L, value)",
		PushType = "long long *",
		get = "LuaPointer<long long> * lp = LuaUserdataPtr::get<LuaPointer<long long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "long long *",
		valid = "return LuaMetatableTraits<LuaPointer<long long> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned long long *",
		push = "LuaUserdata::push<LuaPointer<unsigned long long> >(L, value)",
		PushType = "unsigned long long *",
		get = "LuaPointer<unsigned long long> * lp = LuaUserdataPtr::get<LuaPointer<unsigned long long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned long long *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned long long> >::is_class_type(L, idx)"
	},
	{
		key = "float *",
		push = "LuaUserdata::push<LuaPointer<float> >(L, value)",
		PushType = "float *",
		get = "LuaPointer<float> * lp = LuaUserdataPtr::get<LuaPointer<float> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "float *",
		valid = "return LuaMetatableTraits<LuaPointer<float> >::is_class_type(L, idx)"
	},
	{
		key = "double *",
		push = "LuaUserdata::push<LuaPointer<double> >(L, value)",
		PushType = "double *",
		get = "LuaPointer<double> * lp = LuaUserdataPtr::get<LuaPointer<double> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "double *",
		valid = "return LuaMetatableTraits<LuaPointer<double> >::is_class_type(L, idx)"
	},
	{
		key = "bool *",
		push = "LuaUserdata::push<LuaPointer<bool> >(L, value)",
		PushType = "bool *",
		get = "LuaPointer<bool> * lp = LuaUserdataPtr::get<LuaPointer<bool> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "bool *",
		valid = "return LuaMetatableTraits<LuaPointer<bool> >::is_class_type(L, idx)"
	},

	--------------------------number * &---------------------------------
	{
		key = "signed char * &",
		push = "LuaUserdata::push<LuaPointer<signed char> >(L, value)",
		PushType = "signed char *",
		get = "LuaPointer<signed char> * lp = LuaUserdataPtr::get<LuaPointer<signed char> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "signed char *",
		valid = "return LuaMetatableTraits<LuaPointer<signed char> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned char * &",
		push = "LuaUserdata::push<LuaPointer<unsigned char> >(L, value)",
		PushType = "unsigned char *",
		get = "LuaPointer<unsigned char> * lp = LuaUserdataPtr::get<LuaPointer<unsigned char> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned char *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned char> >::is_class_type(L, idx)"
	},
	{
		key = "short * &",
		push = "LuaUserdata::push<LuaPointer<short> >(L, value)",
		PushType = "short *",
		get = "LuaPointer<short> * lp = LuaUserdataPtr::get<LuaPointer<short> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "short *",
		valid = "return LuaMetatableTraits<LuaPointer<short> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned short * &",
		push = "LuaUserdata::push<LuaPointer<unsigned short> >(L, value)",
		PushType = "unsigned short *",
		get = "LuaPointer<unsigned short> * lp = LuaUserdataPtr::get<LuaPointer<unsigned short> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned short *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned short> >::is_class_type(L, idx)"
	},
	{
		key = "int * &",
		push = "LuaUserdata::push<LuaPointer<int> >(L, value)",
		PushType = "int *",
		get = "LuaPointer<int> * lp = LuaUserdataPtr::get<LuaPointer<int> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "int *",
		valid = "return LuaMetatableTraits<LuaPointer<int> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned int * &",
		push = "LuaUserdata::push<LuaPointer<unsigned int> >(L, value)",
		PushType = "unsigned int *",
		get = "LuaPointer<unsigned int> * lp = LuaUserdataPtr::get<LuaPointer<unsigned int> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned int *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned int> >::is_class_type(L, idx)"
	},
	{
		key = "long * &",
		push = "LuaUserdata::push<LuaPointer<long> >(L, value)",
		PushType = "long *",
		get = "LuaPointer<long> * lp = LuaUserdataPtr::get<LuaPointer<long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "long *",
		valid = "return LuaMetatableTraits<LuaPointer<long> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned long * &",
		push = "LuaUserdata::push<LuaPointer<unsigned long> >(L, value)",
		PushType = "unsigned long *",
		get = "LuaPointer<unsigned long> * lp = LuaUserdataPtr::get<LuaPointer<unsigned long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned long *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned long> >::is_class_type(L, idx)"
	},
	{
		key = "long long * &",
		push = "LuaUserdata::push<LuaPointer<long long> >(L, value)",
		PushType = "long long *",
		get = "LuaPointer<long long> * lp = LuaUserdataPtr::get<LuaPointer<long long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "long long *",
		valid = "return LuaMetatableTraits<LuaPointer<long long> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned long long * &",
		push = "LuaUserdata::push<LuaPointer<unsigned long long> >(L, value)",
		PushType = "unsigned long long *",
		get = "LuaPointer<unsigned long long> * lp = LuaUserdataPtr::get<LuaPointer<unsigned long long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned long long *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned long long> >::is_class_type(L, idx)"
	},
	{
		key = "float * &",
		push = "LuaUserdata::push<LuaPointer<float> >(L, value)",
		PushType = "float *",
		get = "LuaPointer<float> * lp = LuaUserdataPtr::get<LuaPointer<float> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "float *",
		valid = "return LuaMetatableTraits<LuaPointer<float> >::is_class_type(L, idx)"
	},
	{
		key = "double * &",
		push = "LuaUserdata::push<LuaPointer<double> >(L, value)",
		PushType = "double *",
		get = "LuaPointer<double> * lp = LuaUserdataPtr::get<LuaPointer<double> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "double *",
		valid = "return LuaMetatableTraits<LuaPointer<double> >::is_class_type(L, idx)"
	},
	{
		key = "bool * &",
		push = "LuaUserdata::push<LuaPointer<bool> >(L, value)",
		PushType = "bool *",
		get = "LuaPointer<bool> * lp = LuaUserdataPtr::get<LuaPointer<bool> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "bool *",
		valid = "return LuaMetatableTraits<LuaPointer<bool> >::is_class_type(L, idx)"
	},

	--------------------------const number *---------------------------------
	{
		key = "const signed char *",
		push = "LuaUserdata::push<LuaPointer<signed char> >(L, const_cast<signed char *>(value))",
		PushType = "const signed char *",
		get = "LuaPointer<signed char> * lp = LuaUserdataPtr::get<LuaPointer<signed char> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const signed char *",
		valid = "return LuaMetatableTraits<LuaPointer<signed char> >::is_class_type(L, idx)"
	},
	{
		key = "const unsigned char *",
		push = "LuaUserdata::push<LuaPointer<unsigned char> >(L, const_cast<unsigned char *>(value))",
		PushType = "const unsigned char *",
		get = "LuaPointer<unsigned char> * lp = LuaUserdataPtr::get<LuaPointer<unsigned char> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const unsigned char *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned char> >::is_class_type(L, idx)"
	},
	{
		key = "const short *",
		push = "LuaUserdata::push<LuaPointer<short> >(L, const_cast<short *>(value))",
		PushType = "const short *",
		get = "LuaPointer<short> * lp = LuaUserdataPtr::get<LuaPointer<short> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const short *",
		valid = "return LuaMetatableTraits<LuaPointer<short> >::is_class_type(L, idx)"
	},
	{
		key = "const unsigned short *",
		push = "LuaUserdata::push<LuaPointer<unsigned short> >(L, const_cast<unsigned short *>(value))",
		PushType = "const unsigned short *",
		get = "LuaPointer<unsigned short> * lp = LuaUserdataPtr::get<LuaPointer<unsigned short> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const unsigned short *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned short> >::is_class_type(L, idx)"
	},
	{
		key = "const int *",
		push = "LuaUserdata::push<LuaPointer<int> >(L, const_cast<int *>(value))",
		PushType = "const int *",
		get = "LuaPointer<int> * lp = LuaUserdataPtr::get<LuaPointer<int> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const int *",
		valid = "return LuaMetatableTraits<LuaPointer<int> >::is_class_type(L, idx)"
	},
	{
		key = "const unsigned int *",
		push = "LuaUserdata::push<LuaPointer<unsigned int> >(L, const_cast<unsigned int *>(value))",
		PushType = "const unsigned int *",
		get = "LuaPointer<unsigned int> * lp = LuaUserdataPtr::get<LuaPointer<unsigned int> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const unsigned int *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned int> >::is_class_type(L, idx)"
	},
	{
		key = "const long *",
		push = "LuaUserdata::push<LuaPointer<long> >(L, const_cast<long *>(value))",
		PushType = "const long *",
		get = "LuaPointer<long> * lp = LuaUserdataPtr::get<LuaPointer<long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const long *",
		valid = "return LuaMetatableTraits<LuaPointer<long> >::is_class_type(L, idx)"
	},
	{
		key = "const unsigned long *",
		push = "LuaUserdata::push<LuaPointer<unsigned long> >(L, const_cast<unsigned long *>(value))",
		PushType = "const unsigned long *",
		get = "LuaPointer<unsigned long> * lp = LuaUserdataPtr::get<LuaPointer<unsigned long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const unsigned long *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned long> >::is_class_type(L, idx)"
	},
	{
		key = "const long long *",
		push = "LuaUserdata::push<LuaPointer<long long> >(L, const_cast<long long *>(value))",
		PushType = "const long long *",
		get = "LuaPointer<long long> * lp = LuaUserdataPtr::get<LuaPointer<long long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const long long *",
		valid = "return LuaMetatableTraits<LuaPointer<long long> >::is_class_type(L, idx)"
	},
	{
		key = "const unsigned long long *",
		push = "LuaUserdata::push<LuaPointer<unsigned long long> >(L, const_cast<unsigned long long *>(value))",
		PushType = "const unsigned long long *",
		get = "LuaPointer<unsigned long long> * lp = LuaUserdataPtr::get<LuaPointer<unsigned long long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const unsigned long long *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned long long> >::is_class_type(L, idx)"
	},
	{
		key = "const float *",
		push = "LuaUserdata::push<LuaPointer<float> >(L, const_cast<float *>(value))",
		PushType = "const float *",
		get = "LuaPointer<float> * lp = LuaUserdataPtr::get<LuaPointer<float> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const float *",
		valid = "return LuaMetatableTraits<LuaPointer<float> >::is_class_type(L, idx)"
	},
	{
		key = "const double *",
		push = "LuaUserdata::push<LuaPointer<double> >(L, const_cast<double *>(value))",
		PushType = "const double *",
		get = "LuaPointer<double> * lp = LuaUserdataPtr::get<LuaPointer<double> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const double *",
		valid = "return LuaMetatableTraits<LuaPointer<double> >::is_class_type(L, idx)"
	},
	{
		key = "const bool *",
		push = "LuaUserdata::push<LuaPointer<bool> >(L, const_cast<bool *>(value))",
		PushType = "const bool *",
		get = "LuaPointer<bool> * lp = LuaUserdataPtr::get<LuaPointer<bool> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const bool *",
		valid = "return LuaMetatableTraits<LuaPointer<bool> >::is_class_type(L, idx)"
	},

	--------------------------const number * &---------------------------------
	{
		key = "const signed char * &",
		push = "LuaUserdata::push<LuaPointer<signed char> >(L, const_cast<signed char *>(value))",
		PushType = "const signed char *",
		get = "LuaPointer<signed char> * lp = LuaUserdataPtr::get<LuaPointer<signed char> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const signed char *",
		valid = "return LuaMetatableTraits<LuaPointer<signed char> >::is_class_type(L, idx)"
	},
	{
		key = "const unsigned char * &",
		push = "LuaUserdata::push<LuaPointer<unsigned char> >(L, const_cast<unsigned char *>(value))",
		PushType = "const unsigned char *",
		get = "LuaPointer<unsigned char> * lp = LuaUserdataPtr::get<LuaPointer<unsigned char> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const unsigned char *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned char> >::is_class_type(L, idx)"
	},
	{
		key = "const short * &",
		push = "LuaUserdata::push<LuaPointer<short> >(L, const_cast<short *>(value))",
		PushType = "const short *",
		get = "LuaPointer<short> * lp = LuaUserdataPtr::get<LuaPointer<short> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const short *",
		valid = "return LuaMetatableTraits<LuaPointer<short> >::is_class_type(L, idx)"
	},
	{
		key = "const unsigned short * &",
		push = "LuaUserdata::push<LuaPointer<unsigned short> >(L, const_cast<unsigned short *>(value))",
		PushType = "const unsigned short *",
		get = "LuaPointer<unsigned short> * lp = LuaUserdataPtr::get<LuaPointer<unsigned short> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const unsigned short *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned short> >::is_class_type(L, idx)"
	},
	{
		key = "const int * &",
		push = "LuaUserdata::push<LuaPointer<int> >(L, const_cast<int *>(value))",
		PushType = "const int *",
		get = "LuaPointer<int> * lp = LuaUserdataPtr::get<LuaPointer<int> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const int *",
		valid = "return LuaMetatableTraits<LuaPointer<int> >::is_class_type(L, idx)"
	},
	{
		key = "const unsigned int * &",
		push = "LuaUserdata::push<LuaPointer<unsigned int> >(L, const_cast<unsigned int *>(value))",
		PushType = "const unsigned int *",
		get = "LuaPointer<unsigned int> * lp = LuaUserdataPtr::get<LuaPointer<unsigned int> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const unsigned int *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned int> >::is_class_type(L, idx)"
	},
	{
		key = "const long * &",
		push = "LuaUserdata::push<LuaPointer<long> >(L, const_cast<long *>(value))",
		PushType = "const long *",
		get = "LuaPointer<long> * lp = LuaUserdataPtr::get<LuaPointer<long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const long *",
		valid = "return LuaMetatableTraits<LuaPointer<long> >::is_class_type(L, idx)"
	},
	{
		key = "const unsigned long * &",
		push = "LuaUserdata::push<LuaPointer<unsigned long> >(L, const_cast<unsigned long *>(value))",
		PushType = "const unsigned long *",
		get = "LuaPointer<unsigned long> * lp = LuaUserdataPtr::get<LuaPointer<unsigned long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const unsigned long *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned long> >::is_class_type(L, idx)"
	},
	{
		key = "const long long * &",
		push = "LuaUserdata::push<LuaPointer<long long> >(L, const_cast<long long *>(value))",
		PushType = "const long long *",
		get = "LuaPointer<long long> * lp = LuaUserdataPtr::get<LuaPointer<long long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const long long *",
		valid = "return LuaMetatableTraits<LuaPointer<long long> >::is_class_type(L, idx)"
	},
	{
		key = "const unsigned long long * &",
		push = "LuaUserdata::push<LuaPointer<unsigned long long> >(L, const_cast<unsigned long long *>(value))",
		PushType = "const unsigned long long *",
		get = "LuaPointer<unsigned long long> * lp = LuaUserdataPtr::get<LuaPointer<unsigned long long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const unsigned long long *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned long long> >::is_class_type(L, idx)"
	},
	{
		key = "const float * &",
		push = "LuaUserdata::push<LuaPointer<float> >(L, const_cast<float *>(value))",
		PushType = "const float *",
		get = "LuaPointer<float> * lp = LuaUserdataPtr::get<LuaPointer<float> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const float *",
		valid = "return LuaMetatableTraits<LuaPointer<float> >::is_class_type(L, idx)"
	},
	{
		key = "const double * &",
		push = "LuaUserdata::push<LuaPointer<double> >(L, const_cast<double *>(value))",
		PushType = "const double *",
		get = "LuaPointer<double> * lp = LuaUserdataPtr::get<LuaPointer<double> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const double *",
		valid = "return LuaMetatableTraits<LuaPointer<double> >::is_class_type(L, idx)"
	},
	{
		key = "const bool * &",
		push = "LuaUserdata::push<LuaPointer<bool> >(L, const_cast<bool *>(value))",
		PushType = "const bool *",
		get = "LuaPointer<bool> * lp = LuaUserdataPtr::get<LuaPointer<bool> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "const bool *",
		valid = "return LuaMetatableTraits<LuaPointer<bool> >::is_class_type(L, idx)"
	},

	--------------------------number * const &---------------------------------
	{
		key = "signed char * const &",
		push = "LuaUserdata::push<LuaPointer<signed char> >(L, value)",
		PushType = "signed char *",
		get = "LuaPointer<signed char> * lp = LuaUserdataPtr::get<LuaPointer<signed char> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "signed char *",
		valid = "return LuaMetatableTraits<LuaPointer<signed char> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned char * const &",
		push = "LuaUserdata::push<LuaPointer<unsigned char> >(L, value)",
		PushType = "unsigned char *",
		get = "LuaPointer<unsigned char> * lp = LuaUserdataPtr::get<LuaPointer<unsigned char> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned char *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned char> >::is_class_type(L, idx)"
	},
	{
		key = "short * const &",
		push = "LuaUserdata::push<LuaPointer<short> >(L, value)",
		PushType = "short *",
		get = "LuaPointer<short> * lp = LuaUserdataPtr::get<LuaPointer<short> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "short *",
		valid = "return LuaMetatableTraits<LuaPointer<short> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned short * const &",
		push = "LuaUserdata::push<LuaPointer<unsigned short> >(L, value)",
		PushType = "unsigned short *",
		get = "LuaPointer<unsigned short> * lp = LuaUserdataPtr::get<LuaPointer<unsigned short> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned short *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned short> >::is_class_type(L, idx)"
	},
	{
		key = "int * const &",
		push = "LuaUserdata::push<LuaPointer<int> >(L, value)",
		PushType = "int *",
		get = "LuaPointer<int> * lp = LuaUserdataPtr::get<LuaPointer<int> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "int *",
		valid = "return LuaMetatableTraits<LuaPointer<int> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned int * const &",
		push = "LuaUserdata::push<LuaPointer<unsigned int> >(L, value)",
		PushType = "unsigned int *",
		get = "LuaPointer<unsigned int> * lp = LuaUserdataPtr::get<LuaPointer<unsigned int> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned int *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned int> >::is_class_type(L, idx)"
	},
	{
		key = "long * const &",
		push = "LuaUserdata::push<LuaPointer<long> >(L, value)",
		PushType = "long *",
		get = "LuaPointer<long> * lp = LuaUserdataPtr::get<LuaPointer<long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "long *",
		valid = "return LuaMetatableTraits<LuaPointer<long> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned long * const &",
		push = "LuaUserdata::push<LuaPointer<unsigned long> >(L, value)",
		PushType = "unsigned long *",
		get = "LuaPointer<unsigned long> * lp = LuaUserdataPtr::get<LuaPointer<unsigned long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned long *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned long> >::is_class_type(L, idx)"
	},
	{
		key = "long long * const &",
		push = "LuaUserdata::push<LuaPointer<long long> >(L, value)",
		PushType = "long long *",
		get = "LuaPointer<long long> * lp = LuaUserdataPtr::get<LuaPointer<long long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "long long *",
		valid = "return LuaMetatableTraits<LuaPointer<long long> >::is_class_type(L, idx)"
	},
	{
		key = "unsigned long long * const &",
		push = "LuaUserdata::push<LuaPointer<unsigned long long> >(L, value)",
		PushType = "unsigned long long *",
		get = "LuaPointer<unsigned long long> * lp = LuaUserdataPtr::get<LuaPointer<unsigned long long> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "unsigned long long *",
		valid = "return LuaMetatableTraits<LuaPointer<unsigned long long> >::is_class_type(L, idx)"
	},
	{
		key = "float * const &",
		push = "LuaUserdata::push<LuaPointer<float> >(L, value)",
		PushType = "float *",
		get = "LuaPointer<float> * lp = LuaUserdataPtr::get<LuaPointer<float> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "float *",
		valid = "return LuaMetatableTraits<LuaPointer<float> >::is_class_type(L, idx)"
	},
	{
		key = "double * const &",
		push = "LuaUserdata::push<LuaPointer<double> >(L, value)",
		PushType = "double *",
		get = "LuaPointer<double> * lp = LuaUserdataPtr::get<LuaPointer<double> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "double *",
		valid = "return LuaMetatableTraits<LuaPointer<double> >::is_class_type(L, idx)"
	},
	{
		key = "bool * const &",
		push = "LuaUserdata::push<LuaPointer<bool> >(L, value)",
		PushType = "bool *",
		get = "LuaPointer<bool> * lp = LuaUserdataPtr::get<LuaPointer<bool> >(L, idx);\n\t\t\treturn lp ? lp->get_ptr() : nullptr;",
		GetType = "bool *",
		valid = "return LuaMetatableTraits<LuaPointer<bool> >::is_class_type(L, idx)"
	},
	------------------------------enum-------------------------------------------
	--[[{
		template = "typename T",
		key = "T, std::enable_if_t<std::is_enum<T>::type, T>",
		push = "lua_pushinteger(L, static_cast<lua_Integer>(value))",
		PushType = "T",
		get = "return static_cast<GetType>(luaL_checkinteger(L, idx))",
		GetType = "T",
		valid = "return lua_isinteger(L, idx)"
	},--]]
}


function create_traits(typeStr)
	local temp = {}
	table.insert(temp, "\t//")
	table.insert(temp, typeStr.key)
	table.insert(temp, "\n\ttemplate <")
	if typeStr.template then
		table.insert(temp, typeStr.template)
	end
	table.insert(temp, ">\n\tstruct LuaStack <")
	table.insert(temp, typeStr.key)
	table.insert(temp, ">\n\t{\n\t\ttypedef ")
	table.insert(temp, typeStr.PushType)
	table.insert(temp, " PushType;\n\t\ttypedef ");
	table.insert(temp, typeStr.GetType)
	table.insert(temp, " GetType;\n\t\tstatic inline void push(lua_State * L, PushType value)\n\t\t{\n\t\t\t")
	table.insert(temp, typeStr.push)
	table.insert(temp, ";\n\t\t}\n\n")

	table.insert(temp, "\t\tstatic inline GetType get(lua_State * L, int idx)\n\t\t{\n\t\t\t")
	table.insert(temp, typeStr.get)
	table.insert(temp, ";\n\t\t}\n\n")

	table.insert(temp, "\t\tstatic inline bool valid(lua_State * L, int idx)\n\t\t{\n\t\t\t")
	table.insert(temp, typeStr.valid)
	table.insert(temp, ";\n\t\t}\n\t};\n")

	return table.concat(temp)
end

local data = [[// Copyright 2023-present ChenDong, email <baisaichen@live.com>, All rights reserved.
// this is a generate file, do't modify

#pragma once

#include "PureLua/LuaErrorDesc.h"
#include "PureLua/LuaUserdata.h"
#include "PureLua/LuaPointer.h"

#include <type_traits>

namespace PureLua {
	struct PURELUA_API LuaTypeNil
	{  };

	template <typename T>
	struct LuaStack;

]]
for k, v in ipairs(type_str) do
	data = data .. create_traits(v)
end
data = data .. "}"
local fileName = "../include/PureLua/LuaStackBase.h"
local fp = io.open(fileName, "r")
if not fp then
	error(strint.format("not found file `%s`", fileName))
end
fp:close()
fp = io.open(fileName, "wb")
fp:write(data)
fp:close()