






#pragma once

#ifndef CIV5_WORLD_BUILDER_MAP_LOADER_H
#define CIV5_WORLD_BUILDER_MAP_LOADER_H

class CvWorldBuilderMap;

class CvWorldBuilderMapLoader
{
public:

	static const CvWorldBuilderMapLoaderMapInfo& GetCurrentMapInfo();


	static bool Preload(_In_z_ const wchar_t* wszFilename, bool bScenario);
	static void SetupGameOptions();
	static void SetupPlayers();
	static void SetInitialItems(bool bFirstCall);
	static bool InitMap();


	static bool Load(_In_z_ const wchar_t* wszFilename);


	static bool Save(_In_z_ const wchar_t* wszFilename, const char* szMapName = NULL);


	static int LoadModData(lua_State* L);
	static int RunPostProcessScript(lua_State* L);
	static void ValidateTerrain();
	static void ValidateCoast();
	static void ClearResources();
	static void ClearGoodies();

	static WorldSizeTypes GetCurrentWorldSizeType();
	static WorldSizeTypes GetWorldSizeType(const CvWorldBuilderMap& kMap);
	

	static void ResetPlayerSlots();
	static void MapPlayerToSlot(uint uiPlayer, PlayerTypes ePlayerSlot);
	static PlayerTypes GetMapPlayerSlot(uint uiPlayer);
	static int GetMapPlayerCount();


	static uint PreviewPlayableCivCount(_In_z_ const wchar_t* wszFilename);
	static int GetMapPreview(lua_State* L);
	static int GetMapPlayers(lua_State* L);


	static int AddRandomItems(lua_State* L);
	static int ScatterResources(lua_State* L);
	static int ScatterGoodies(lua_State* L);

};

#endif
