













#pragma once
#ifndef CVLUAGAMEINFO_H
#define CVLUAGAMEINFO_H

class CvLuaGameInfo
{
public:

	static void Register(lua_State* L);

private:


	static int pRegisterActions(lua_State* L);


	static int pRegisterTypesInfo(lua_State* L);
};


#endif
