













#pragma once

class CvFractal;

class CvLuaFractal
{
public:

	static void Register(lua_State* L);

protected:


	static int pRegister(lua_State* L);


	static CvFractal* CreateFractal(lua_State* L);


	static int lCreate(lua_State* L);


	static int lCreateRifts(lua_State* L);


	static int lBuildRidges(lua_State* L);


	static int lDestroy(lua_State* L);


	static int lGetHeight(lua_State* L);
};
