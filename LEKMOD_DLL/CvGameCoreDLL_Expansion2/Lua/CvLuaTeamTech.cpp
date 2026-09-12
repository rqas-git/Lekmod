













#include <CvGameCoreDLLPCH.h>
#include "CvLuaSupport.h"
#include "CvLuaTeamTech.h"


#define Method(Name)			\
	lua_pushcclosure(L, l##Name, 0);	\
	lua_setfield(L, t, #Name);


void CvLuaTeamTech::HandleMissingInstance(lua_State* L)
{
	DefaultHandleMissingInstance(L);
}

void CvLuaTeamTech::PushMethods(lua_State* L, int t)
{
	Method(SetHasTech);
	Method(HasTech);

	Method(GetNumTechsKnown);
	Method(HasResearchedAllTechs);

	Method(GetLastTechAcquired);

	Method(SetNoTradeTech);
	Method(IsNoTradeTech);
	Method(IncrementTechCount);
	Method(GetTechCount);
	Method(SetResearchProgress);
	Method(SetResearchProgressTimes100);
	Method(GetResearchProgress);
	Method(GetResearchProgressTimes100);
	Method(ChangeResearchProgress);
	Method(ChangeResearchProgressTimes100);
	Method(ChangeResearchProgressPercent);
	Method(GetResearchCost);
	Method(GetResearchLeft);
}

const char* CvLuaTeamTech::GetTypeName()
{
	return "TeamTech";
}









int CvLuaTeamTech::lSetHasTech(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::SetHasTech);
}



int CvLuaTeamTech::lHasTech(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::HasTech);
}



int CvLuaTeamTech::lGetNumTechsKnown(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetNumTechsKnown);
}



int CvLuaTeamTech::lHasResearchedAllTechs(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::HasResearchedAllTechs);
}



int CvLuaTeamTech::lGetLastTechAcquired(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetLastTechAcquired);
}



int CvLuaTeamTech::lSetNoTradeTech(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::SetNoTradeTech);
}



int CvLuaTeamTech::lIsNoTradeTech(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::IsNoTradeTech);
}



int CvLuaTeamTech::lIncrementTechCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::IncrementTechCount);
}



int CvLuaTeamTech::lGetTechCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetTechCount);
}



int CvLuaTeamTech::lSetResearchProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::SetResearchProgress);
}



int CvLuaTeamTech::lSetResearchProgressTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::SetResearchProgressTimes100);
}



int CvLuaTeamTech::lGetResearchProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetResearchProgress);
}



int CvLuaTeamTech::lGetResearchProgressTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetResearchProgressTimes100);
}



int CvLuaTeamTech::lChangeResearchProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::ChangeResearchProgress);
}



int CvLuaTeamTech::lChangeResearchProgressTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::ChangeResearchProgressTimes100);
}



int CvLuaTeamTech::lChangeResearchProgressPercent(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::ChangeResearchProgressPercent);
}



int CvLuaTeamTech::lGetResearchCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetResearchCost);
}



int CvLuaTeamTech::lGetResearchLeft(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetResearchLeft);
}
