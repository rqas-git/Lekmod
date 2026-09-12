













#pragma once
#ifndef CVLUAENUMS_H
#define CVLUAENUMS_H

class CvLuaEnums
{
public:

	static void Register(lua_State* L);

protected:


	static int pRegister(lua_State* L);


	static void pRegisterEnum(lua_State* L, const char* enumName, const int enumVal);
	static void pRegisterEnumUInt(lua_State* L, const char* enumName, const uint enumVal);


	static void RegisterDynamicEnums(lua_State* L, _In_z_ const char* szTableName,
	                                 _In_z_ const char* szIdField = "_ROWID_", _In_z_ const char* szNameField = "Type", _In_opt_z_ const char* szCountName = NULL);


	static void EnumStart(lua_State* L, const char* enumType);


	static void EnumEnd(lua_State* L);

};

#endif
