
#pragma once 
#ifndef CVLUASTATICINSTANCE_H

#include "CvLuaMethodWrapper.h"

template<class Derived, class InstanceType>
class CvLuaStaticInstance : public CvLuaMethodWrapper<Derived, InstanceType>
{
public:	
	static void Register(lua_State* L);


	static const int GetStartingArgIndex();

protected:
	static int pRegister(lua_State* L);


	static void RegisterMethod(lua_State* L, lua_CFunction func, const char* funcName);
};




template<class Derived, class InstanceType>
void CvLuaStaticInstance<Derived, InstanceType>::Register(lua_State* L)
{
	FLua::Details::CCallWithErrorHandling(L, pRegister);
}

template<class Derived, class InstanceType>
const int CvLuaStaticInstance<Derived, InstanceType>::GetStartingArgIndex()
{
	return 1;
}

template<class Derived, class InstanceType>
int CvLuaStaticInstance<Derived, InstanceType>::pRegister(lua_State* L)
{
	const char* szInstanceName = Derived::GetInstanceName();
	lua_getglobal(L, szInstanceName);
	if(lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setglobal(L, szInstanceName);
	}

	Derived::RegisterMembers(L);

	return 0;
}

template<class Derived, class InstanceType>
void CvLuaStaticInstance<Derived, InstanceType>::RegisterMethod(lua_State *L, lua_CFunction func, const char* funcName)
{
	lua_pushcclosure(L, func, 0);
	lua_setfield(L, -2, funcName);
}



#endif
