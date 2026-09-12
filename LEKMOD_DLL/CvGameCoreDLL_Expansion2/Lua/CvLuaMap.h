
















#pragma once
#ifndef CVLUAMAP_H
#define CVLUAMAP_H

#include "CvLuaStaticInstance.h"

class CvLuaMap : public CvLuaStaticInstance<CvLuaMap, CvMap>
{
public:

	static const char* GetInstanceName();


	static CvMap* GetInstance(lua_State* L, int idx);


	static void RegisterMembers(lua_State* L);

protected:


	static int lAreas(lua_State* L);
	static int lAreasAux(lua_State* L);

	static int lFindBiggestArea(lua_State* L);


	static int lFindWater(lua_State* L);


	static int lGetClimate(lua_State* L);


	static int lGetFractalFlags(lua_State* L);

	static int lGetNumPlots(lua_State* L);


	static int lGetNumResources(lua_State* L);


	static int lGetNumResourcesOnLand(lua_State* L);


	static int lGetPlot(lua_State* L);
	static int lGetPlotByIndex(lua_State* L);

	static int lGetPlotXY(lua_State* L);

	static int lGetArea(lua_State* L);


	static int lGetGridSize(lua_State* L);

	static int lGetIndexAfterLastArea(lua_State* L);

	static int lGetLandPlots(lua_State* L);

	static int lGetNumAreas(lua_State* L);

	static int lGetNumLandAreas(lua_State* L);


	static int lGetRandomResourceQuantity(lua_State* L);


	static int lGetSeaLevel(lua_State* L);


	static int lGetWorldSize(lua_State* L);

	static int lIsPlot(lua_State* L);


	static int lIsWrapX(lua_State* L);


	static int lIsWrapY(lua_State* L);

	static int lMaxPlotDistance(lua_State* L);


	static int lRand(lua_State* L);


	static int lCalculateAreas(lua_State* L);


	static int lRecalculateAreas(lua_State* L);


	static int lPlotDistance(lua_State* L);


	static int lPlotXYWithRangeCheck(lua_State* L);


	static int lPlotDirection(lua_State* L);


	static int lDefaultContinentStamper(lua_State* L);


	static int lDoPlaceNaturalWonders(lua_State* L);


	static int lGetCustomOption(lua_State* L);

	static int lUpdateDeferredFog(lua_State* L);

	static int lChangeAIMapHint(lua_State* L);
	static int lGetAIMapHint(lua_State* L);

};

#endif
