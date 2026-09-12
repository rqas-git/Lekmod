






#pragma once
#include "CvDllInterfaces.h"

class CvDllNetInitInfo : public ICvNetInitInfo1
{
public:
	CvDllNetInitInfo();
	~CvDllNetInitInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	const char* DLLCALL GetDebugString();
	bool DLLCALL Read(FDataStream& kStream);
	bool DLLCALL Write(FDataStream& kStream);
	bool DLLCALL Commit();

private:
	void DLLCALL Destroy();


	CvString m_szLoadFileName;
	StorageLocation m_eLoadFileStorage;


	CvString m_szMapScriptName;
	bool m_bIsEarthMap;
	bool m_bIsRandomMapScript;
	bool m_bIsRandomWorldSize;
	bool m_bWBMapNoPlayers;

	WorldSizeTypes m_eWorldSize;
	ClimateTypes m_eClimate;
	SeaLevelTypes m_eSeaLevel;
	EraTypes m_eEra;
	CalendarTypes m_eCalendar;
	int m_iGameTurn;
	bool m_bGameStarted;
	GameSpeedTypes m_eGameSpeed;
	TurnTimerTypes m_eTurnTimerEnabled;
	int m_iTurnTimerTime;


	CvString m_szGameName;

	unsigned int m_uiSyncRandSeed;
	unsigned int m_uiMapRandSeed;

	int m_iNumVictories;
	std::vector<bool> m_abVictories;


	std::vector<CvPreGame::CustomOption> m_aGameOptions;
	std::vector<CvPreGame::CustomOption> m_aMapOptions;

	std::vector<bool> m_abMPOptions;


	int m_iMaxTurns;
	int m_iPitbossTurnTime;
	int m_iMaxCityElimination;
	int m_iNumMinorCivs;
	int m_iNumAdvancedStartPoints;
	GameMode m_eMode;
	bool m_bStatReporting;

	char m_szDebugString[512];

	unsigned int m_uiRefCount;
};
