







#pragma once

#include "CvLuaMethodWrapper.h"

template<class Derived, class InstanceType, typename EnumType>
class CvLuaScopedIndirectInstance : public CvLuaMethodWrapper<Derived, InstanceType>
{
public:
	static void Push(lua_State* L, InstanceType* pkType);
	static void Push(lua_State* L, FObjectHandle<InstanceType> handle)
	{
		Push(L, handle.pointer());
	}
	static InstanceType* GetInstance(lua_State* L, int idx = 1, bool bErrorOnFail = true);


	static const int GetStartingArgIndex();

protected:
	static void DefaultHandleMissingInstance(lua_State* L);
};






template<class Derived, class InstanceType, typename EnumType>
void CvLuaScopedIndirectInstance<Derived, InstanceType, EnumType>::Push(lua_State* L, EnumType eID)
{










	lua_getglobal(L, Derived::GetTypeName());
	if(lua_isnil(L, -1))
	{

		lua_pop(L, 1);
		lua_newtable(L);


		lua_pushstring(L, "__instances");
		lua_newtable(L);


		lua_newtable(L);
		lua_pushstring(L, "__mode");
		lua_pushstring(L, "v");
		lua_rawset(L, -3);
		lua_setmetatable(L, -2);

		lua_rawset(L, -3);


		lua_pushvalue(L, -1);
		lua_setglobal(L, Derived::GetTypeName());

		Derived::PushMethods(L, lua_gettop(L));
	}
	const int type_index = lua_gettop(L);

	lua_pushstring(L, "__instances");
	lua_rawget(L, -2);

	const int instances_index = lua_gettop(L);

	lua_pushinteger(L, static_cast<int>(eID));
	lua_rawget(L, -2);

	if(lua_isnil(L, -1))
	{
		lua_pop(L, 1);


		lua_createtable(L, 0, 1);
		lua_pushinteger(L, static_cast<int>(eID));
		lua_setfield(L, -2, "__instance");

		lua_createtable(L, 0, 1);
		lua_pushstring(L, "__index");
		lua_pushvalue(L, type_index);
		lua_rawset(L, -3);
		lua_setmetatable(L, -2);


		lua_pushinteger(L, static_cast<int>(eID));
		lua_pushvalue(L, -2);
		lua_rawset(L, instances_index);
	}


	lua_remove(L, instances_index);
	lua_remove(L, type_index);
}

template<class Derived, class InstanceType, typename EnumType>
InstanceType* CvLuaScopedIndirectInstance<Derived, InstanceType, EnumType>::GetInstance(lua_State* L, int idx, bool bErrorOnFail)
{
	const int stack_size = lua_gettop(L);
	bool bFail = true;

	InstanceType* pkInstance = NULL;
	if(lua_type(L, idx) == LUA_TTABLE)
	{
		lua_getfield(L, idx, "__instance");
		if(lua_type(L, -1) == LUA_TNUMBER)
		{
			EnumType eValue = static_cast<EnumType>(lua_tointeger(L, -1));
			pkInstance = static_cast<Derived*>(this)->ResolveInstance(eValue);
			if(pkInstance != NULL)
			{
				bFail = false;
			}
		}
	}

	lua_settop(L, stack_size);

	if(bFail && bErrorOnFail)
	{
		if(idx == 1)
			luaL_error(L, "Not a valid instance.  Either the instance is NULL or you used '.' instead of ':'.");
		Derived::HandleMissingInstance(L);
	}
	return pkInstance;
}

template<class Derived, class InstanceType, typename EnumType>
const int CvLuaScopedIndirectInstance<Derived, InstanceType, EnumType>::GetStartingArgIndex()
{
	return 2;
}

template<class Derived, class InstanceType, typename EnumType>
void CvLuaScopedIndirectInstance<Derived, InstanceType, EnumType>::DefaultHandleMissingInstance(lua_State* L)
{
	luaL_error(L, "Instance does not exist.");
}
