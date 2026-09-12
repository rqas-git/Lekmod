














#pragma once
#ifndef CVLUATEAMTECH_H
#define CVLUATEAMTECH_H

#include "CvLuaScopedInstance.h"

class CvLuaTeamTech : public CvLuaScopedInstance<CvLuaTeamTech, CvTeamTechs>
{
public:

	static void PushMethods(lua_State* L, int t);


	static void HandleMissingInstance(lua_State* L);


	static const char* GetTypeName();

protected:

	static int lSetHasTech(lua_State* L);
	static int lHasTech(lua_State* L);
	static int lGetNumTechsKnown(lua_State* L);
	static int lHasResearchedAllTechs(lua_State* L);

	static int lGetLastTechAcquired(lua_State* L);

	static int lSetNoTradeTech(lua_State* L);
	static int lIsNoTradeTech(lua_State* L);
	static int lIncrementTechCount(lua_State* L);
	static int lGetTechCount(lua_State* L);
	static int lSetResearchProgress(lua_State* L);
	static int lSetResearchProgressTimes100(lua_State* L);
	static int lGetResearchProgress(lua_State* L);
	static int lGetResearchProgressTimes100(lua_State* L);
	static int lChangeResearchProgress(lua_State* L);
	static int lChangeResearchProgressTimes100(lua_State* L);
	static int lChangeResearchProgressPercent(lua_State* L);
	static int lGetResearchCost(lua_State* L);
	static int lGetResearchLeft(lua_State* L);
	static int lGetPlayerResearchCost(lua_State* L);
	static int lGetPlayerResearchLeft(lua_State* L);

};

#endif
