






#pragma once

#include "CvDllInterfaces.h"

class CvDllWorldBuilderMapLoader : public ICvWorldBuilderMapLoader2
{
public:
	CvDllWorldBuilderMapLoader();
	~CvDllWorldBuilderMapLoader();

	void* DLLCALL QueryInterface(GUID guidInterface);

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	const CvWorldBuilderMapLoaderMapInfo& DLLCALL GetCurrentMapInfo();


	bool DLLCALL Preload(_In_z_ const wchar_t* wszFilename, bool bScenario);
	void DLLCALL SetupGameOptions();
	void DLLCALL SetupPlayers();
	void DLLCALL SetInitialItems(bool bFirstCall);
	bool DLLCALL InitMap();


	bool DLLCALL Save(_In_z_ const wchar_t* wszFilename, const char* szMapName = NULL);


	int DLLCALL LoadModData(lua_State* L);
	void DLLCALL ValidateTerrain();
	void DLLCALL ValidateCoast();
	void DLLCALL ClearResources();
	void DLLCALL ClearGoodies();


	void DLLCALL ResetPlayerSlots();
	void DLLCALL MapPlayerToSlot(uint uiPlayer, PlayerTypes ePlayerSlot);


	unsigned int DLLCALL PreviewPlayableCivCount(_In_z_ const wchar_t* wszFilename);
	int DLLCALL GetMapPreview(lua_State* L);
	int DLLCALL GetMapPlayers(lua_State* L);


	int DLLCALL AddRandomItems(lua_State* L);
	int DLLCALL ScatterResources(lua_State* L);
	int DLLCALL ScatterGoodies(lua_State* L);

	PlayerTypes DLLCALL GetMapPlayerSlot(uint uiPlayer);
	int DLLCALL GetMapPlayerCount();

	void DLLCALL GenerateRandomMap(const char* szMapScript);

	WorldSizeTypes DLLCALL GetWorldSizeType() const;

private:
	void DLLCALL Destroy();
};
