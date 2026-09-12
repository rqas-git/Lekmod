






#pragma once

#ifndef CIV5_MAPGENERATOR_H
#define CIV5_MAPGENERATOR_H

class CvMapGenerator
{
public:
	CvMapGenerator(const char* szMapScriptName);
	virtual ~CvMapGenerator();


	bool GetMapInitData(CvMapInitData& kData, WorldSizeTypes eWorldSize);


	void GenerateRandomMap();


	bool GetGameInitialItemsOverrides(CvGameInitialItemsOverrides& kOverrides);

protected:


	static int pGetMapInitData(lua_State* L);


	static int pGenerateRandomMap(lua_State* L);


	static int pGetGameInitialItemsOverrides(lua_State* L);

private:
	CvMapGenerator(const CvMapGenerator&);
	CvMapGenerator& operator=(const CvMapGenerator&);

	lua_State* m_pkLuaState;
};

#endif
