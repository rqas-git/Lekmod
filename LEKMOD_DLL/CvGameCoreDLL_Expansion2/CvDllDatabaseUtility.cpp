














#include "CvGameCoreDLLPCH.h"
#include <algorithm>
#include "CvDLLDatabaseUtility.h"
#include "CvDllContext.h"
#include "CvGlobals.h"
#include "CvGrandStrategyAI.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvCitySpecializationAI.h"
#include "CvMinorCivAI.h"
#include "CvImprovementClasses.h"
#include "CvDatabaseUtility.h"
#include "CvGameTextMgr.h"
#include "CvGameCoreUtils.h"
#include "cvStopWatch.h"
#include "CvTypes.h"

#include <CvLocalization.h>
#include <CvGameDatabase.h>

#include "LintFree.h"

#include <windows.h>

#define GAMEPLAYXML_PATH "Gameplay\\XML\\"

#ifdef LEKMOD_POST_DLC_DATA_LOADING

size_t MinValue(size_t a, size_t b);
#endif


void InsertGameDefine(Database::Results& kInsertDefine, const char* szValue, int iValue)
{
	kInsertDefine.Bind(1, szValue);
	kInsertDefine.Bind(2, iValue);
	kInsertDefine.Execute();
	kInsertDefine.Reset();
}


CvDllDatabaseUtility::CvDllDatabaseUtility(Database::Connection& db)
	: m_kGameplayDatabase(db)
	, m_bGameDatabaseNeedsCaching(true)
	, m_uiRefCount(1)
{
}

CvDllDatabaseUtility::~CvDllDatabaseUtility()
{
}

void* CvDllDatabaseUtility::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvDLLDatabaseUtility1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}

unsigned int CvDllDatabaseUtility::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}

unsigned int CvDllDatabaseUtility::DecrementReference()
{
	if(m_uiRefCount == 1)
	{
		delete this;
		return 0;
	}
	else
	{
		--m_uiRefCount;
		return m_uiRefCount;
	}
}

unsigned int CvDllDatabaseUtility::GetReferenceCount()
{
	return m_uiRefCount;
}

void CvDllDatabaseUtility::Destroy()
{
	DecrementReference();
}

void CvDllDatabaseUtility::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}

void* CvDllDatabaseUtility::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}

bool CvDllDatabaseUtility::CacheGameDatabaseData()
{

	if(m_bGameDatabaseNeedsCaching == false)
		return true;


	bool bSuccess = true;



	ValidateGameDatabase();
	
#ifdef LEKMOD_POST_DLC_DATA_LOADING

	bSuccess &= PerformPostDLCLoading();
#endif





	GC.infoTypeFromStringReset();
	Database::Results kTables("name");
	if(DB.SelectAt(kTables, "sqlite_master", "type", "table"))
	{
		while(kTables.Step())
		{
			Database::Results kTypes;
			if(DB.SelectAll(kTypes, kTables.GetText(0)))
			{
				bool bFirstStep = true;
				while(kTypes.Step())
				{
					if(bFirstStep)
					{
						if(!kTypes.HasColumn("ID") || !kTypes.HasColumn("Type"))
						{
							break;
						}

						bFirstStep = false;
					}

					const int rowid = kTypes.GetInt("ID");
					const char* szType = kTypes.GetText("Type");
					if(szType)
						GC.setInfoTypeFromString(szType, rowid);
				}
			}
		}
	}

	bSuccess &= LoadGlobalDefines();
	bSuccess &= PrefetchGameData();
	bSuccess &= UpdatePlayableCivilizationCounts();

	CvTypes::AcquireTypes(DB);

	bSuccess &= SetGlobalActionInfo();


	DB.ClearCountCache();


	LogMsg(DB.CalculateMemoryStats());

	CvAssertMsg(bSuccess, "Failed to load Gameplay Database Data! Not Good!");

	if(bSuccess)
		m_bGameDatabaseNeedsCaching = false;

	return bSuccess;
}

bool CvDllDatabaseUtility::FlushGameDatabaseData()
{
	m_bGameDatabaseNeedsCaching = true;
	
#ifdef LEKMOD_POST_DLC_DATA_LOADING
	this->LogMsg("[LEKMOD] Database flush requested");
#endif

	return true;
}

bool CvDllDatabaseUtility::PerformDatabasePostProcessing()
{





	Database::Connection* db = GC.GetGameDatabase();


	db->BeginTransaction();
	Database::Results kPostDefines;


	Database::Results kInsert;
	db->Execute(kInsert, "INSERT OR REPLACE INTO Defines(Name, Value) VALUES(?, ?)");

	db->SelectAll(kPostDefines, "PostDefines");
	while(kPostDefines.Step())
	{
		const char* szName = kPostDefines.GetText("Name");
		const char* szKeyName = kPostDefines.GetText("Key");
		const char* szTableName = kPostDefines.GetText("Table");
		char szSQL[512];

		sprintf_s(szSQL, "select ROWID from %s where Type = '%s' LIMIT 1", szTableName, szKeyName);

		Database::Results kLookup;


		if(db->Execute(kLookup, szSQL))
		{

			if(kLookup.Step())
			{

				kInsert.Bind(1, szName);
				kInsert.Bind(2, kLookup.GetInt(0));
				kInsert.Step();
				kInsert.Reset();
			}
		}
	}





	const char* szStrategicResource
	    = "UPDATE Resources SET ResourceUsage = 1 WHERE EXISTS (SELECT * FROM Unit_ResourceQuantityRequirements WHERE ResourceType = Type)";
	db->Execute(szStrategicResource);


	const char* szLuxoryResource
	    = "UPDATE Resources SET ResourceUsage = 2 where Happiness > 0";
	db->Execute(szLuxoryResource);



	Database::Results kInsertDefine;
	if(db->Execute(kInsertDefine, "INSERT OR REPLACE INTO Defines(Name, Value) VALUES(?, ?);"))
	{
		InsertGameDefine(kInsertDefine, "MAX_CIV_PLAYERS", MAX_CIV_PLAYERS);
		InsertGameDefine(kInsertDefine, "MAX_CIV_TEAMS", MAX_CIV_TEAMS);
		InsertGameDefine(kInsertDefine, "MAX_MAJOR_CIVS", MAX_MAJOR_CIVS);
		InsertGameDefine(kInsertDefine, "MAX_MINOR_CIVS", MAX_MINOR_CIVS);
		InsertGameDefine(kInsertDefine, "MAX_PLAYERS", MAX_PLAYERS);
		InsertGameDefine(kInsertDefine, "MAX_TEAMS", MAX_TEAMS);
		InsertGameDefine(kInsertDefine, "BARBARIAN_PLAYER", BARBARIAN_PLAYER);
		InsertGameDefine(kInsertDefine, "BARBARIAN_TEAM", BARBARIAN_TEAM);

		InsertGameDefine(kInsertDefine, "NUM_CITY_PLOTS", NUM_CITY_PLOTS);
		InsertGameDefine(kInsertDefine, "CITY_HOME_PLOT", CITY_HOME_PLOT);
		InsertGameDefine(kInsertDefine, "CITY_PLOTS_RADIUS", CITY_PLOTS_RADIUS);
		InsertGameDefine(kInsertDefine, "CITY_PLOTS_DIAMETER", CITY_PLOTS_DIAMETER);
	}

	db->EndTransaction();

	return true;
}

bool CvDllDatabaseUtility::PrefetchGameData()
{
	cvStopWatch kTest("PrefetchGameData", "xml-perf.log");
	CvBuilderTaskingAI::ClearBuildTypeCache();




	PrefetchCollection(GC.GetColorInfo(), "Colors");
	PrefetchCollection(GC.GetPlayerColorInfo(), "PlayerColors");
	PrefetchCollection(GC.getInterfaceModeInfo(), "InterfaceModes");


	PrefetchCollection(GC.getAICityStrategyInfo(), "AICityStrategies");
	PrefetchCollection(GC.getEconomicAIStrategyInfo(), "AIEconomicStrategies");
	PrefetchCollection(GC.getAIGrandStrategyInfo(), "AIGrandStrategies");
	PrefetchCollection(GC.getMilitaryAIStrategyInfo(), "AIMilitaryStrategies");
	PrefetchCollection(GC.getCitySpecializationInfo(), "CitySpecializations");
	PrefetchCollection(GC.getTacticalMoveInfo(), "TacticalMoves");




	PrefetchCollection(GC.getUnitAIInfo(), "UnitAIInfos");
	PrefetchCollection(GC.getUnitCombatClassInfo(), "UnitCombatInfos");


	PrefetchCollection(GC.getBuildingClassInfo(), "BuildingClasses");
	PrefetchCollection(GC.getBuildingInfo(), "Buildings");


	PrefetchCollection(GC.getEmphasisInfo(), "EmphasizeInfos");
	PrefetchCollection(GC.getEraInfo(), "Eras");
	PrefetchCollection(GC.getGameOptionInfo(), "GameOptions");
	PrefetchCollection(GC.getGameSpeedInfo(), "GameSpeeds");
	PrefetchCollection(GC.getHandicapInfo(), "HandicapInfos");
	PrefetchCollection(GC.getHurryInfo(), "HurryInfos");
	PrefetchCollection(GC.getMPOptionInfo(), "MultiplayerOptions");
	PrefetchCollection(GC.getPlayerOptionInfo(), "PlayerOptions");
	PrefetchCollection(GC.getPolicyInfo(), "Policies");
	PrefetchCollection(GC.getPolicyBranchInfo(), "PolicyBranchTypes");
	PrefetchCollection(GC.getProcessInfo(), "Processes");
	PrefetchCollection(GC.getProjectInfo(), "Projects");
	PrefetchCollection(GC.getSmallAwardInfo(), "SmallAwards");
	PrefetchCollection(GC.getSpecialistInfo(), "Specialists");
	PrefetchCollection(GC.getVictoryInfo(), "Victories");
	PrefetchCollection(GC.getVoteInfo(), "Votes");
	PrefetchCollection(GC.getVoteSourceInfo(), "VoteSources");
	PrefetchCollection(GC.getUnitDomainInfo(), "Domains");


	PrefetchCollection(GC.getLeaderHeadInfo(), "Leaders");


	PrefetchCollection(GC.getRouteInfo(), "Routes");
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	PrefetchCollection(GC.getGreatWorkClassInfo(), "GreatWorkClasses");
#endif

	CvNotificationXMLEntries* pkNotificationEntries =  GC.GetNotificationEntries();
	if(pkNotificationEntries != NULL)
	{
		PrefetchCollection(pkNotificationEntries->GetNotificationEntries(), "Notifications");
	}

#if defined(TRADE_REFACTOR)
	PrefetchCollection(GC.getTradeConnectionInfo(), "TradeConnections");
#endif

	PrefetchCollection(GC.getTechInfo(), "Technologies");


	PrefetchCollection(GC.getFeatureInfo(), "Features");
	PrefetchCollection(GC.getImprovementInfo(), "Improvements");
	PrefetchCollection(GC.getResourceClassInfo(), "ResourceClasses");
	PrefetchCollection(GC.getResourceInfo(), "Resources");
	PrefetchCollection(GC.getTerrainInfo(), "Terrains");
	PrefetchCollection(GC.getYieldInfo(), "Yields");


	PrefetchCollection(GC.getAutomateInfo(), "Automates");
	PrefetchCollection(GC.getBuildInfo(), "Builds");
	PrefetchCollection(GC.getCommandInfo(), "Commands");
	PrefetchCollection(GC.getControlInfo(), "Controls");
	PrefetchCollection(GC.getEntityEventInfo(), "EntityEvents");
	PrefetchCollection(GC.getMissionInfo(), "Missions");
	PrefetchCollection(GC.getMultiUnitFormationInfo(), "MultiUnitFormations");
	PrefetchCollection(GC.getSpecialUnitInfo(), "SpecialUnits");
	PrefetchCollection(GC.getUnitClassInfo(), "UnitClasses");
	PrefetchCollection(GC.getPromotionInfo(), "UnitPromotions");
	CvPromotionEntry::InvalidateVisibilityChangeCache();
	PrefetchCollection(GC.getUnitInfo(), "Units");


	PrefetchCollection(GC.getCivilizationInfo(), "Civilizations");
	PrefetchCollection(GC.getMinorCivInfo(), "MinorCivilizations");
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	PrefetchCollection(GC.getMinorCivPersonalityInfo(), "Minor_Civ_Personalities");
#endif
	PrefetchCollection(GC.getTraitInfo(), "Traits");
	PrefetchCollection(GC.getReligionInfo(), "Religions");
	PrefetchCollection(GC.getBeliefInfo(), "Beliefs");
	PrefetchCollection(GC.getLeagueSpecialSessionInfo(), "LeagueSpecialSessions");
	PrefetchCollection(GC.getLeagueNameInfo(), "LeagueNames");
	PrefetchCollection(GC.getLeagueProjectInfo(), "LeagueProjects");
	PrefetchCollection(GC.getLeagueProjectRewardInfo(), "LeagueProjectRewards");
	PrefetchCollection(GC.getResolutionInfo(), "Resolutions");


	{
		CvDatabaseUtility kUtility;
		CvString*& paFlavors = GC.getFlavorTypes();
		const int iNumFlavors = kUtility.MaxRows("Flavors");

		GC.setNumFlavorTypes(iNumFlavors);
		paFlavors = FNEW(CvString[iNumFlavors], c_eCiv5GameplayDLL, 0);

		Database::Results kResults;
		if(DB.SelectWhere(kResults, "Flavors", "ID > -1"))
		{
			while(kResults.Step())
			{
				const int iFlavor = kResults.GetInt("ID");
				CvAssert(iFlavor >= 0 && iFlavor < iNumFlavors);
				if(iFlavor >= 0 && iFlavor < iNumFlavors)
				{
					paFlavors[iFlavor] = kResults.GetText("Type");

				}
			}
		}
		else
		{
			CvAssertMsg(false, DB.ErrorMessage());
		}
	}

#ifdef LEKMOD_ADJACENT_IMPROVEMENT_YIELD

	for(int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); iImprovement++)
	{
		CvImprovementEntry* pImprovement = GC.getImprovementInfo(static_cast<ImprovementTypes>(iImprovement));
		if(pImprovement)
		{
			pImprovement->CacheAdjacencyYieldBonus();
		}
	}
#endif

	CvBuilderTaskingAI::CacheBuildTypes();
	ValidatePrefetchProcess();

	return true;
}

bool CvDllDatabaseUtility::ValidateGameDatabase()
{


	if(!gDLL->ShouldValidateGameDatabase())
	{
		LogMsg("*** SKIPPING Game Database Validation. ****");
		LogMsg("You can enable it by setting 'ValidateGameDatabase = 1' in config.ini");
		return true;
	}

	cvStopWatch kPerfTest("Validating Game Database", "xml-perf.log");
	bool bError = false;

	LogMsg("**** Validating Game Database *****");


	{
		cvStopWatch watch("Ensure All Tables with 'ID' column start at 0");
		Database::Results kTables("name");
		if(DB.SelectAt(kTables, "sqlite_master", "type", "table"))
		{
			while(kTables.Step())
			{
				const char* szTableName = kTables.GetText(0);
				if(DB.Count(szTableName) > 0)
				{

					bool bHasIDColumn = false;
					{


						char szSQL[512];
						sprintf_s(szSQL, "pragma table_info(%s)", szTableName);
						Database::Results kResults;
						DB.Execute(kResults, szSQL);
						while(kResults.Step())
						{
							const char* szName = kResults.GetText("name");
							if(strcmp(szTableName, szName) == 0)
							{
								bHasIDColumn = true;
								break;
							}

						}
					}

					if(bHasIDColumn)
					{
						Database::SingleResult kTest;
						if(!DB.SelectAt(kTest, szTableName, "ID", 0))
						{

							char szError[512];
							sprintf_s(szError, "Table '%s' contains 'ID' column that starts at 1 instead of 0.", szTableName);
							LogMsg(szError);
							bError = true;
						}
					}

				}
			}
		}
	}


	{
		cvStopWatch watch("Validate FK Constraints");
		DB.ValidateFKConstraints();
	}

	LogMsg("Performing Localization Checks");
	{
		cvStopWatch watch("Localization Checks");
		LogMsg("Checking Tag Format...");
		LogMsg("Note: Tags must only use [A-Z_] characters, start with 'TXT_KEY_', and be under 128 characters long.");

		Database::Results kLanguages;
		if(DB.SelectAll(kLanguages, "Languages"))
		{
			while(kLanguages.Step())
			{
				const char* szTagColumnName = kLanguages.GetText("TagColumnName");
				const char* szTableName = kLanguages.GetText("TableName");

				Database::Results kResults;
				char szSQL[512];
				sprintf_s(szSQL, "SELECT DISTINCT %s from %s", szTagColumnName, szTableName);
				if(DB.Execute(kResults, szSQL))
				{
					bool bFirst = true;
					while(kResults.Step())
					{
						const char* szTag = kResults.GetText(0);
						const size_t len = strlen(szTag);
						if(len > 128)
						{
							if(bFirst)
							{
								LogMsg("In table %s...", szTableName);
								bFirst = false;
							}

							bError =  true;
							LogMsg("Tag (%s) has size greater than 127 characters.", szTag);
						}
						else
						{
							if(strncmp("TXT_KEY_", szTag, 8) != 0)
							{
								if(bFirst)
								{
									LogMsg("In table %s...", szTableName);
									bFirst = false;
								}

								bError = true;
								LogMsg("Tag (%s) does not start with 'TXT_KEY_'", szTag);
							}

							for(size_t i = 0; i < len; ++i)
							{
								char ch = szTag[i];
								if(ch != '_' && !((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')))
								{
									if(bFirst)
									{
										LogMsg("In table %s...", szTableName);
										bFirst = false;
									}

									bError = true;
									LogMsg("Tag (%s) contains invalid character (%.1s)", szTag, &ch);
								}
							}
						}
					}
				}
			}
		}
	}

	LogMsg("Validating UnitGameplay2DScripts");
	{
		cvStopWatch watch("Validating UnitGameplay2DScripts");
		Database::Results kResults;
		if(DB.Execute(kResults, "Select Type from Units where not exists (select 1 from UnitGameplay2DScripts where UnitType = Units.Type limit 1)"))
		{
			while(kResults.Step())
			{
				const char* szUnitType = kResults.GetText(0);
				LogMsg("Missing Entry for %s", szUnitType);
				bError = true;
			}
		}
	}

	if(bError)
	{
		LogMsg("**** VALIDATION FAILED *****");
	}
	else
	{
		LogMsg("**** Validation Success *****");
	}

	kPerfTest.EndPerfTest();
	LogMsg("Validation Took %f seconds", kPerfTest.GetDeltaInSeconds());

	return !bError;
}


#ifdef AUI_WARNING_FIXES
#define ValidateCount(func) if(func() <= 0){bError = true; LogMsg("ERROR: %s <= 0", #func);}
#else
#define ValidateCount(func) if(func() <= 0){bError = true, LogMsg("ERROR: %s <= 0", #func);}
#endif
#if defined(LEKMOD_MACOS)
#define ValidateVectorSize(vec) ValidateCount(gc.vec);
#else
#define ValidateVectorSize(vec) ValidateCount(gc.##vec);
#endif
bool CvDllDatabaseUtility::ValidatePrefetchProcess()
{
	bool bError = false;
	CvGlobals& gc = GC;

	LogMsg("**** Validating Prefetch Process *****");
	ValidateVectorSize(getNumWorldInfos);
	ValidateVectorSize(getNumClimateInfos);
	ValidateVectorSize(getNumSeaLevelInfos);
	ValidateVectorSize(GetNumColorInfos);
	ValidateVectorSize(GetNumPlayerColorInfos);
	ValidateVectorSize(getNumEntityEventInfos);
	ValidateVectorSize(getNumMultiUnitFormationInfos);
	ValidateVectorSize(getNumTerrainInfos);
	ValidateVectorSize(getNumResourceClassInfos);
	ValidateVectorSize(getNumResourceInfos);
	ValidateVectorSize(getNumFeatureInfos);
	ValidateVectorSize(getNumCivilizationInfos);
	ValidateVectorSize(getNumMinorCivInfos);
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	ValidateVectorSize(getNumMinorCivPersonalityInfos);
#endif
	ValidateVectorSize(getNumLeaderHeadInfos);
	ValidateVectorSize(getNumTraitInfos);
	ValidateVectorSize(getNumUnitInfos);
	ValidateVectorSize(getNumSpecialUnitInfos);
	ValidateVectorSize(getNumVoteSourceInfos);
	ValidateVectorSize(getNumUnitCombatClassInfos);
#if defined(TRADE_REFACTOR)
	ValidateVectorSize(getNumTradeConnectionInfos);
#endif

	ValidateCount(gc.getUnitAIInfo().size);


	ValidateVectorSize(getNumGameOptionInfos);
	ValidateVectorSize(getNumMPOptionInfos);

	ValidateCount(gc.getPlayerOptionInfo().size);
	ValidateCount(gc.getYieldInfo().size);

	ValidateVectorSize(getNumRouteInfos);
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	ValidateVectorSize(getNumGreatWorkClassInfos);
#endif
	ValidateVectorSize(getNumImprovementInfos);
	ValidateVectorSize(getNumBuildInfos);
	ValidateVectorSize(getNumHandicapInfos);
	ValidateVectorSize(getNumGameSpeedInfos);
	ValidateVectorSize(getNumProcessInfos);
	ValidateVectorSize(getNumVoteInfos);
	ValidateVectorSize(getNumProjectInfos);
	ValidateVectorSize(getNumBuildingClassInfos);
	ValidateVectorSize(getNumBuildingInfos);
	ValidateVectorSize(getNumUnitClassInfos);


	ValidateCount(gc.getMissionInfo().size);
	ValidateCount(gc.getControlInfo().size);
	ValidateCount(gc.getCommandInfo().size);

	ValidateVectorSize(getNumAutomateInfos);
	ValidateVectorSize(getNumPromotionInfos);
	ValidateVectorSize(getNumSpecialistInfos);
	ValidateVectorSize(getNumEconomicAIStrategyInfos);
	ValidateVectorSize(getNumCitySpecializationInfos);
	ValidateVectorSize(getNumTacticalMoveInfos);
	ValidateVectorSize(getNumMilitaryAIStrategyInfos);
	ValidateVectorSize(getNumAIGrandStrategyInfos);
	ValidateVectorSize(getNumAICityStrategyInfos);
	ValidateVectorSize(getNumPolicyInfos);
	ValidateVectorSize(getNumPolicyBranchInfos);
	ValidateVectorSize(getNumTechInfos);
	ValidateVectorSize(getNumEraInfos);
	ValidateVectorSize(getNumHurryInfos);
	ValidateVectorSize(getNumEmphasisInfos);
	ValidateVectorSize(getNumVictoryInfos);


#define ValidateDomain(domain) { CvDomainInfo* pkDomainInfo; if (GC.getNumUnitDomainInfos() <= (int)domain || (pkDomainInfo = GC.getUnitDomainInfo(domain)) == NULL || strcmp(pkDomainInfo->GetType(), #domain) != 0) bError = true; }

	ValidateDomain(DOMAIN_SEA);
	ValidateDomain(DOMAIN_AIR);
	ValidateDomain(DOMAIN_LAND);
	ValidateDomain(DOMAIN_IMMOBILE);
	ValidateDomain(DOMAIN_HOVER);

	if(bError)
	{
		LogMsg("**** VALIDATION FAILED *****");
	}
	else
	{
		LogMsg("**** Validation Success *****");
	}

	return !bError;
}










bool CvDllDatabaseUtility::LoadGlobalDefines()
{
	GC.cacheGlobals();
	return true;
}






bool CvDllDatabaseUtility::UpdatePlayableCivilizationCounts()
{
	cvStopWatch kPerfTest("UpdatePlayableCivilizationCounts", "xml-perf.log");

	int numPlayableCivilizations = 0;
	int numAIPlayableCivilizations = 0;


#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.getNumCivilizationInfos(); ++i)
#else
	for(int i = 0; i < GC.getNumCivilizationInfos(); ++i)
#endif
	{
		CvCivilizationInfo* pkCivilization = GC.getCivilizationInfo((CivilizationTypes) i);
		if(pkCivilization)
		{
			if(pkCivilization->isPlayable())
				numPlayableCivilizations++;

			if(pkCivilization->isAIPlayable())
				numAIPlayableCivilizations++;
		}

		GC.getNumPlayableCivilizationInfos() = numPlayableCivilizations;
		GC.getNumAIPlayableCivilizationInfos() = numAIPlayableCivilizations;
	}

	return true;
}










bool CvDllDatabaseUtility::SetGlobalActionInfo()
{
	cvStopWatch kPerfTest("SetGlobalActionInfo", "xml-perf.log");
	LogMsg("SetGlobalActionInfo\n");

	typedef std::vector<CvActionInfo*> ActionInfoVector;
	ActionInfoVector& actionInfos = GC.getActionInfo();


	for(ActionInfoVector::iterator it = actionInfos.begin(); it != actionInfos.end(); ++it)
	{
		CvActionInfo* pActionInfo = (*it);
		delete pActionInfo;
	}
	actionInfos.clear();

#ifdef AUI_WARNING_FIXES
	const uint iNumMissionTypes = CvTypes::getNUM_MISSION_TYPES();
#else
	const int iNumMissionTypes = CvTypes::getNUM_MISSION_TYPES();
#endif


	if(!(NUM_INTERFACEMODE_TYPES > 0))
	{
		LogMsg("NUM_INTERFACE_TYPES is not greater than zero in CvDllDatabaseUtility::SetGlobalActionInfo.");
	}
	if(!(GC.getNumBuildInfos() > 0))
	{
		LogMsg("GC.getNumBuildInfos() is not greater than zero in CvDllDatabaseUtility::SetGlobalActionInfo.");
	}
	if(!(GC.getNumPromotionInfos() > 0))
	{
		LogMsg("GC.getNumPromotionInfos() is not greater than zero in CvDllDatabaseUtility::SetGlobalActionInfo.");
	}
	if(!(GC.getNumSpecialistInfos() > 0))
	{
		LogMsg("GC.getNumSpecialistInfos() is not greater than zero in CvDllDatabaseUtility::SetGlobalActionInfo.");
	}
	if(!(NUM_CONTROL_TYPES > 0))
	{
		LogMsg("NUM_CONTROL_TYPES is not greater than zero in CvDllDatabaseUtility::SetGlobalActionInfo.");
	}
	if(!(GC.getNumAutomateInfos() > 0))
	{
		LogMsg("GC.getNumAutomateInfos() is not greater than zero in CvDllDatabaseUtility::SetGlobalActionInfo.");
	}
	if(!(NUM_COMMAND_TYPES > 0))
	{
		LogMsg("NUM_COMMAND_TYPES is not greater than zero in CvDllDatabaseUtility::SetGlobalActionInfo.");
	}
	if(!(iNumMissionTypes > 0))
	{
		LogMsg("NUM_MISSION_TYPES is not greater than zero in CvDllDatabaseUtility::SetGlobalActionInfo.");
	}

	int iEstimatedNumActionInfos =
	    NUM_INTERFACEMODE_TYPES +
	    GC.getNumBuildInfos() +
	    GC.getNumPromotionInfos() +
	    GC.getNumSpecialistInfos() +
	    NUM_CONTROL_TYPES +
	    NUM_COMMAND_TYPES +
	    GC.getNumAutomateInfos() +
	    iNumMissionTypes;

	int* piBuffer = FNEW(int[iEstimatedNumActionInfos * 4], c_eCiv5GameplayDLL, 0);
	memset(piBuffer,0, sizeof(int) * iEstimatedNumActionInfos * 4);

	int* piIndexList = piBuffer + iEstimatedNumActionInfos * 0;
	int* piPriorityList = piBuffer + iEstimatedNumActionInfos * 1;
	int* piActionInfoTypeList = piBuffer + iEstimatedNumActionInfos * 2;
	int* piOrderedIndex = piBuffer + iEstimatedNumActionInfos * 3;



#ifdef AUI_WARNING_FIXES
	uint iTotalActionInfoCount = 0;
	uint i = 0;
#else
	int iTotalActionInfoCount = 0;
	int i = 0;
#endif
	for(i=0; i<NUM_COMMAND_TYPES; i++)
	{
		CvCommandInfo* commandInfo = GC.getCommandInfo((CommandTypes)i);
		if(commandInfo)
		{
			piIndexList[iTotalActionInfoCount] = i;
			piPriorityList[iTotalActionInfoCount] = commandInfo->getOrderPriority();
			piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_COMMAND;
			iTotalActionInfoCount++;
		}
	}

	for(i=0; i<NUM_INTERFACEMODE_TYPES; i++)
	{
		CvInterfaceModeInfo* interfaceInfo = GC.getInterfaceModeInfo((InterfaceModeTypes)i);
		if(interfaceInfo)
		{
			piIndexList[iTotalActionInfoCount] = i;
			piPriorityList[iTotalActionInfoCount] = interfaceInfo->getOrderPriority();
			piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_INTERFACEMODE;
			iTotalActionInfoCount++;
		}
	}

	for(i=0; i<GC.getNumBuildInfos(); i++)
	{
		CvBuildInfo* buildInfo = GC.getBuildInfo((BuildTypes)i);
		if(buildInfo)
		{
			piIndexList[iTotalActionInfoCount] = i;
			piPriorityList[iTotalActionInfoCount] = buildInfo->getOrderPriority();
			piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_BUILD;
			iTotalActionInfoCount++;
		}
	}

	for(i=0; i<GC.getNumPromotionInfos(); i++)
	{
		CvPromotionEntry* promotionEntry = GC.getPromotionInfo((PromotionTypes)i);
		if(promotionEntry)
		{
			piIndexList[iTotalActionInfoCount] = i;
			piPriorityList[iTotalActionInfoCount] = promotionEntry->getOrderPriority();
			piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_PROMOTION;
			iTotalActionInfoCount++;
		}
	}

	for(i=0; i<GC.getNumSpecialistInfos(); i++)
	{
		CvSpecialistInfo* specialistInfo = GC.getSpecialistInfo((SpecialistTypes)i);
		if(specialistInfo)
		{
			piIndexList[iTotalActionInfoCount] = i;
			piPriorityList[iTotalActionInfoCount] = specialistInfo->getOrderPriority();
			piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_SPECIALIST;
			iTotalActionInfoCount++;
		}
	}

	for(i=0; i<NUM_CONTROL_TYPES; i++)
	{
		CvControlInfo* controlInfo = GC.getControlInfo((ControlTypes)i);
		if(controlInfo)
		{
			piIndexList[iTotalActionInfoCount] = i;
			piPriorityList[iTotalActionInfoCount] = controlInfo->getOrderPriority();
			piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_CONTROL;
			iTotalActionInfoCount++;
		}
	}

	for(i=0; i<GC.getNumAutomateInfos(); i++)
	{
		CvAutomateInfo* automateInfo = GC.getAutomateInfo(i);
		if(automateInfo)
		{
			piIndexList[iTotalActionInfoCount] = i;
			piPriorityList[iTotalActionInfoCount] = automateInfo->getOrderPriority();
			piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_AUTOMATE;
			iTotalActionInfoCount++;
		}
	}

	for(i=0; i<iNumMissionTypes; i++)
	{
		CvMissionInfo* missionInfo = GC.getMissionInfo((MissionTypes)i);
		if(missionInfo)
		{
			piIndexList[iTotalActionInfoCount] = i;
			piPriorityList[iTotalActionInfoCount] = missionInfo->getOrderPriority();
			piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_MISSION;
			iTotalActionInfoCount++;
		}
	}


	actionInfos.reserve(iTotalActionInfoCount);


	orderHotkeyInfo(&piOrderedIndex, piPriorityList, iTotalActionInfoCount);
	for(i = 0; i < iTotalActionInfoCount; i++)
	{
		CvActionInfo* pActionInfo = FNEW(CvActionInfo, c_eCiv5GameplayDLL, 0);
		CvAssert(piIndexList[piOrderedIndex[i]] != -1);

		pActionInfo->setOriginalIndex(piIndexList[piOrderedIndex[i]]);
		pActionInfo->setSubType((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]]);
		if((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_COMMAND)
		{
			GC.getCommandInfo((CommandTypes)piIndexList[piOrderedIndex[i]])->setActionInfoIndex(i);
		}
		else if((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_INTERFACEMODE)
		{
			GC.getInterfaceModeInfo((InterfaceModeTypes)piIndexList[piOrderedIndex[i]])->setActionInfoIndex(i);
		}
		else if((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_BUILD)
		{
			GC.getBuildInfo((BuildTypes)piIndexList[piOrderedIndex[i]])->setMissionType(GC.getInfoTypeForString("MISSION_BUILD"));
			GC.getBuildInfo((BuildTypes)piIndexList[piOrderedIndex[i]])->setActionInfoIndex(i);
		}
		else if((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_PROMOTION)
		{
			CvPromotionEntry* pkEntry = GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]);

			pkEntry->SetCommandType(GC.getInfoTypeForString("COMMAND_PROMOTION"));
			pkEntry->setActionInfoIndex(i);
			CvString strHotKey = pkEntry->CreateHotKeyFromDescription(pkEntry->getHotKey(), pkEntry->isShiftDown(), pkEntry->isAltDown(), pkEntry->isCtrlDown());
			pkEntry->setHotKeyDescription(pkEntry->GetDescription(), GC.getCommandInfo((CommandTypes)(pkEntry->GetCommandType()))->GetDescription(), strHotKey);
		}
		else if((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_SPECIALIST)
		{
			CvSpecialistInfo* pkSpecialist = GC.getSpecialistInfo((SpecialistTypes)piIndexList[piOrderedIndex[i]]);

			pkSpecialist->setMissionType(GC.getInfoTypeForString("MISSION_JOIN"));
			pkSpecialist->setActionInfoIndex(i);
			CvString strHotKey = pkSpecialist->CreateHotKeyFromDescription(pkSpecialist->getHotKey(), pkSpecialist->isShiftDown(), pkSpecialist->isAltDown(), pkSpecialist->isCtrlDown());
			pkSpecialist->setHotKeyDescription(pkSpecialist->GetDescription(), GC.getMissionInfo((MissionTypes)(pkSpecialist->getMissionType()))->GetDescription(), strHotKey);
		}
		else if((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_CONTROL)
		{
			GC.getControlInfo((ControlTypes)piIndexList[piOrderedIndex[i]])->setActionInfoIndex(i);
		}
		else if((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_AUTOMATE)
		{
			GC.getAutomateInfo(piIndexList[piOrderedIndex[i]])->setActionInfoIndex(i);
		}
		else if((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_MISSION)
		{
			GC.getMissionInfo((MissionTypes)piIndexList[piOrderedIndex[i]])->setActionInfoIndex(i);
		}

		GC.getActionInfo().push_back(pActionInfo);
	}

	SAFE_DELETE_ARRAY(piBuffer);





	return true;
}





struct OrderIndex
{
	int m_iPriority;
	int m_iIndex;
};
bool sortHotkeyPriority(const OrderIndex& orderIndex1, const OrderIndex& orderIndex2)
{
	return (orderIndex1.m_iPriority > orderIndex2.m_iPriority);
}

template <class T>
void CvDllDatabaseUtility::orderHotkeyInfo(int** ppiSortedIndex, T* pHotkeyInfos, int iLength)
{
	int iI;
	int* piSortedIndex;
	std::vector<OrderIndex> viOrderPriority;

	viOrderPriority.resize(iLength);
	piSortedIndex = *ppiSortedIndex;


	for(iI=0; iI<iLength; iI++)
	{
		viOrderPriority[iI].m_iPriority = pHotkeyInfos[iI].getOrderPriority();
		viOrderPriority[iI].m_iIndex = iI;
	}


	std::sort(viOrderPriority.begin(), viOrderPriority.end(), sortHotkeyPriority);


	for(iI=0; iI<iLength; iI++)
	{
		piSortedIndex[iI] = viOrderPriority[iI].m_iIndex;
	}
}

void CvDllDatabaseUtility::orderHotkeyInfo(int** ppiSortedIndex, int* pHotkeyIndex, int iLength)
{
	int iI;
	int* piSortedIndex;
	std::vector<OrderIndex> viOrderPriority;

	viOrderPriority.resize(iLength);
	piSortedIndex = *ppiSortedIndex;


	for(iI=0; iI<iLength; iI++)
	{
		viOrderPriority[iI].m_iPriority = pHotkeyIndex[iI];
		viOrderPriority[iI].m_iIndex = iI;
	}


	std::sort(viOrderPriority.begin(), viOrderPriority.end(), sortHotkeyPriority);


	for(iI=0; iI<iLength; iI++)
	{
		piSortedIndex[iI] = viOrderPriority[iI].m_iIndex;
	}
}

#ifdef LEKMOD_POST_DLC_DATA_LOADING
bool CvDllDatabaseUtility::PerformPostDLCLoading()
{
	Database::Connection* db = GC.GetGameDatabase();
	if(!db)
		return false;


	db->BeginTransaction();
	this->LogMsg("[LEKMOD] Starting XML loading process");


	const wchar_t* wszDLCPath = L"Assets\\DLC\\";
	

	WIN32_FIND_DATAW ffd;
	HANDLE hFind = FindFirstFileW((std::wstring(wszDLCPath) + L"*LEKMOD*").c_str(), &ffd);
	
	int foldersProcessed = 0;
	bool bSuccess = false;
	
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do {

			if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				continue;


			std::wstring wszXMLPath = std::wstring(wszDLCPath) + ffd.cFileName + L"\\XML\\";
			

			char szFolderName[256];
			WideCharToMultiByte(CP_UTF8, 0, ffd.cFileName, -1, szFolderName, sizeof(szFolderName), NULL, NULL);
			
			char szXMLPath[256];
			WideCharToMultiByte(CP_UTF8, 0, wszXMLPath.c_str(), -1, szXMLPath, sizeof(szXMLPath), NULL, NULL);
			this->LogMsg("[LEKMOD] Found folder: %s, checking for XML at: %s", szFolderName, szXMLPath);
			

			WIN32_FIND_DATAW xmlFolderData;
			HANDLE hXmlFind = FindFirstFileW((wszXMLPath + L"*").c_str(), &xmlFolderData);
			
			if(hXmlFind != INVALID_HANDLE_VALUE) 
			{
				FindClose(hXmlFind);
				

				this->LogMsg("[LEKMOD] Processing XML files in: %s", szXMLPath);
				Database::XMLSerializer serializer(*db);
				ProcessXMLFiles(wszXMLPath, serializer);
				
				foldersProcessed++;
				bSuccess = true;
			}
			else 
			{
				this->LogMsg("[LEKMOD] No XML subfolder found in: %s", szFolderName);
			}

		} while(FindNextFileW(hFind, &ffd) != 0);
		
		FindClose(hFind);
	}
	else
	{
		this->LogMsg("[LEKMOD] No LEKMOD folders found in DLC directory");
		db->EndTransaction();
		return false;
	}


	db->EndTransaction();
	
	if(foldersProcessed > 0) 
	{
		this->LogMsg("[LEKMOD] XML loading complete - processed %d folders", foldersProcessed);
		return bSuccess;
	}
	else 
	{
		this->LogMsg("[LEKMOD] No valid LEKMOD/XML folders were found");
		return false;
	}
}


void CvDllDatabaseUtility::ProcessXMLFiles(const std::wstring& wszPath, Database::XMLSerializer& serializer)
{

    WIN32_FIND_DATAW ffd;
    HANDLE hFind = FindFirstFileW((wszPath + L"*.xml").c_str(), &ffd);
    
    if(hFind != INVALID_HANDLE_VALUE)
    {
        char szLogBuffer[256];
        
        do
        {
            std::wstring wszFile = wszPath + ffd.cFileName;
            

            WideCharToMultiByte(CP_UTF8, 0, wszFile.c_str(), -1, szLogBuffer, sizeof(szLogBuffer), NULL, NULL);
            this->LogMsg("[LEKMOD] Loading XML: %s", szLogBuffer);
            

            if(serializer.Load(wszFile.c_str()))
            {
                this->LogMsg("[LEKMOD] Successfully loaded XML file");
            }
            else
            {
                this->LogMsg("[LEKMOD] Failed to load XML file - %s", serializer.ErrorMessage());
            }
            
        } while(FindNextFileW(hFind, &ffd) != 0);
        
        FindClose(hFind);
    }
    

    hFind = FindFirstFileW((wszPath + L"*").c_str(), &ffd);
    
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {

            if(wcscmp(ffd.cFileName, L".") == 0 || wcscmp(ffd.cFileName, L"..") == 0 || 
              !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                continue;
                

            ProcessXMLFiles(wszPath + ffd.cFileName + L"\\", serializer);
            
        } while(FindNextFileW(hFind, &ffd) != 0);
        
        FindClose(hFind);
    }
}


size_t MinValue(size_t a, size_t b)
{
    return (a < b) ? a : b;
}
#endif








void CvDllDatabaseUtility::LogMsg(const char* format, ...) const
{
	const size_t kBuffSize = 1024;
	static char buf[kBuffSize];
	const uint uiFlags = 0;

	va_list vl;
	va_start(vl,format);
	vsprintf_s(buf, format, vl);
	va_end(vl);

	LOGFILEMGR.GetLog("xml.log", uiFlags)->Msg(buf);
}
