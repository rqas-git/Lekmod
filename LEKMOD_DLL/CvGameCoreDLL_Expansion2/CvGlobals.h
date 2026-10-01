/*	-------------------------------------------------------------------------------------------------------
	? 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// CvGlobals.h

#ifndef CIV5_GLOBALS_H
#define CIV5_GLOBALS_H

//
// 'global' vars for Civ V.  singleton class.
// All globals and global types should be contained in this class
// Author -	Mustafa Thamer
//			Jon Shafer - 03/2005
//
class CvRandom;
class CvGame;
class CvPlayerAI;
class CvAStar;
class CvStepPathFinder;
class CvIgnoreUnitsPathFinder;
class CvTwoLayerPathFinder;
class CvInterface;
class CvEngine;
class FVariableSystem;
class CvMap;
class CvPlayerAI;
class CvTeam;
class CvInterfaceModeInfo;
class CvColorInfo;
class CvPlayerColorInfo;
class CvEntityEventInfo;
class CvLandscapeInfo;
class CvTerrainInfo;
class CvResourceClassInfo;
#if defined(TRADE_REFACTOR)
class CvTradeConnectionInfo;
#endif
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
class CvGreatWorkClassInfo;
#endif
class CvResourceInfo;
class CvFeatureInfo;
class CvCivilizationInfo;
class CvMinorCivInfo;
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
class CvMinorCivPersonalityInfo;
#endif
class CvLeaderHeadInfo;
class CvSpecialUnitInfo;
class CvYieldInfo;
class CvRouteInfo;
class CvImprovementInfo;
class CvBuildInfo;
class CvHandicapInfo;
class CvGameSpeedInfo;
class CvTurnTimerInfo;
class CvProcessInfo;
class CvVoteInfo;
class CvBuildingClassInfo;
class CvUnitClassInfo;
class CvActionInfo;
class CvMissionInfo;
class CvControlInfo;
class CvCommandInfo;
class CvAutomateInfo;
class CvSpecialistInfo;
class CvEraInfo;
class CvHurryInfo;
class CvVictoryInfo;
class CvSmallAwardInfo;
class CvGameOptionInfo;
class CvMPOptionInfo;
class CvPlayerOptionInfo;
class CvEventTriggerInfo;
class CvEventInfo;
class CvVoteSourceInfo;
class CvMultiUnitFormationInfo;
class CvEconomicAIStrategyXMLEntries;
class CvEconomicAIStrategyXMLEntry;
class CvCitySpecializationXMLEntries;
class CvCitySpecializationXMLEntry;
class CvTacticalMoveXMLEntries;
class CvTacticalMoveXMLEntry;
class CvMilitaryAIStrategyXMLEntries;
class CvMilitaryAIStrategyXMLEntry;
class CvAIGrandStrategyXMLEntries;
class CvAIGrandStrategyXMLEntry;
class CvAICityStrategies;
class CvAICityStrategyEntry;
class CvPolicyXMLEntries;
class CvPolicyEntry;
class CvPolicyBranchEntry;
class CvTechXMLEntries;
class CvTechEntry;
class CvBuildingEntry;
class CvBuildingXMLEntries;
class CvUnitEntry;
class CvUnitXMLEntries;
class CvProjectEntry;
class CvProjectXMLEntries;
class CvPromotionEntry;
class CvPromotionXMLEntries;
class CvImprovementEntry;
class CvImprovementXMLEntries;
class CvEmphasisEntry;
class CvEmphasisXMLEntries;
class CvTraitEntry;
class CvTraitXMLEntries;
class CvNotificationEntry;
class CvNotificationXMLEntries;
class CvBaseInfo;
class CvReplayInfo;
class CvReligionEntry;
class CvReligionXMLEntries;
class CvBeliefEntry;
class CvBeliefXMLEntries;
class CvLeagueSpecialSessionEntry;
class CvLeagueSpecialSessionXMLEntries;
class CvLeagueNameEntry;
class CvLeagueNameXMLEntries;
class CvLeagueProjectEntry;
class CvLeagueProjectXMLEntries;
class CvLeagueProjectRewardEntry;
class CvLeagueProjectRewardXMLEntries;
class CvResolutionEntry;
class CvResolutionXMLEntries;
class CvDeal;
class CvNetMessageHandler;

class CvDLLInterfaceIFaceBase;
class ICvDLLDatabaseUtility1;
class ICvEngineUtility1;
class ICvEngineUtility2;
class ICvScriptSystemUtility1;
class ICvCity1;
class ICvDeal1;
class ICvGameContext1;
class ICvPlayer1;
class ICvPlot1;
class ICvRandom1;
class ICvUnit1;


class CvGlobals
{
public:

	typedef stdext::hash_map<std::string /* type string */, int /* info index */> InfosMap;
	typedef std::map<uint /* FString::HashType */, int /* info index */> InfosHashMap;

	// singleton accessor
	static CvGlobals& getInstance();

	CvGlobals();
	virtual ~CvGlobals();

	//DLL Utility Methods
	CvCity* UnwrapCityPointer(ICvCity1* pCity);
	auto_ptr<ICvCity1> WrapCityPointer(CvCity* pCity);

	CvDeal* UnwrapDealPointer(ICvDeal1* pDeal);
	auto_ptr<ICvDeal1> WrapDealPointer(CvDeal* pDeal);

	CvPlot* UnwrapPlotPointer(ICvPlot1* pPlot);
	auto_ptr<ICvPlot1> WrapPlotPointer(CvPlot* pPlot);

	CvRandom* UnwrapRandomPointer(ICvRandom1* pRandom);
	auto_ptr<ICvRandom1> WrapRandomPointer(CvRandom* pRandom);

	auto_ptr<ICvUnit1> WrapUnitPointer(CvUnit* pUnit);
	CvUnit*   UnwrapUnitPointer(ICvUnit1* pUnit);

	void init();
	void uninit();

	CvMap& getMap()
	{
		return *m_map;    // inlined for perf reasons, do not use outside of dll
	}
	CvMap* getMapPointer()
	{
		return m_map;
	}
	CvGame& getGame()
	{
		return *m_game;    // inlined for perf reasons, do not use outside of dll
	}
	CvGame* getGamePointer()
	{
		return m_game;
	}
	int* getPlotDirectionX()
	{
		return m_aiPlotDirectionX;
	}
	int* getPlotDirectionY()
	{
		return m_aiPlotDirectionY;
	}

	CvRandom& getASyncRand();
	CvTwoLayerPathFinder& getPathFinder();
	CvTwoLayerPathFinder& getInterfacePathFinder();
	CvIgnoreUnitsPathFinder& getIgnoreUnitsPathFinder();
	CvStepPathFinder& getStepFinder();
	CvAStar& getRouteFinder();
	CvAStar& GetWaterRouteFinder();
	CvAStar& getAreaFinder();
	CvAStar& getInfluenceFinder();
	CvAStar& GetBuildRouteFinder();
	CvAStar& GetInternationalTradeRouteLandFinder();
	CvAStar& GetInternationalTradeRouteWaterFinder();
	CvTwoLayerPathFinder& GetTacticalAnalysisMapFinder();
#ifdef AUI_DANGER_PLOTS_REMADE
	CvTwoLayerPathFinder& getDangerPathFinder();
#endif
	ICvDLLDatabaseUtility1* getDatabaseLoadUtility();

	std::vector<CvInterfaceModeInfo*>& getInterfaceModeInfo();
	CvInterfaceModeInfo* getInterfaceModeInfo(InterfaceModeTypes e);

	bool getLogging();
	void setLogging(bool bEnable);

	int getRandLogging();
	void setRandLogging(int iRandLoggingFlags);

	bool getAILogging();
	void setAILogging(bool bEnable);

	bool getAIPerfLogging();
	void setAIPerfLogging(bool bEnable);

	bool GetBuilderAILogging();
	void SetBuilderAILogging(bool bEnable);

	bool getPlayerAndCityAILogSplit();
	void setPlayerAndCityAILogSplit(bool bEnable);

	bool GetTutorialLogging();
	void SetTutorialLogging(bool bEnable);

	bool GetTutorialDebugging();
	void SetTutorialDebugging(bool bEnable);

	bool GetAllowRClickMovementWhileScrolling();
	void SetAllowRClickMovementWhileScrolling(bool bAllow);

	bool GetPostTurnAutosaves();
	void SetPostTurnAutosaves(bool bEnable);

	void LogMessage(const char* szMessage);

	inline int* getCityPlotX()
	{
		return m_aiCityPlotX;
	}
	inline int* getCityPlotY()
	{
		return m_aiCityPlotY;
	}

	int* getCityPlotPriority();
	int getXYCityPlot(int i, int j);
	DirectionTypes* getTurnLeftDirection();
	DirectionTypes getTurnLeftDirection(int i);
	DirectionTypes* getTurnRightDirection();
	DirectionTypes getTurnRightDirection(int i);

	DirectionTypes getXYDirection(int i, int j, int stagger);

	//
	// Global Infos
	// All info type strings are upper case and are kept in this hash map for fast lookup
	//
	void SetGameDatabase(Database::Connection* pGameDatabase);
	Database::Connection* GetGameDatabase();
	const Database::Connection* GetGameDatabase() const;

	const InfosMap& GetInfoTypes() const;
	int getInfoTypeForString(const char* szType, bool hideAssert = false) const;			// returns the infos index, use this when searching for an info type string
	int getInfoTypeForHash(uint uiHash, bool hideAssert = false) const;			// returns the infos index, use this when searching for an info type string
	void setInfoTypeFromString(const char* szType, int idx);
	void infoTypeFromStringReset();
	void infosReset();

	int getNumWorldInfos();
	int getNumClimateInfos();
	int getNumSeaLevelInfos();

#ifdef AUI_WARNING_FIXES
	uint GetNumColorInfos() const;
#else
	int GetNumColorInfos();
#endif
	std::vector<CvColorInfo*>& GetColorInfo();
	_Ret_maybenull_ CvColorInfo* GetColorInfo(ColorTypes e);

#ifdef AUI_WARNING_FIXES
	uint GetNumPlayerColorInfos() const;
#else
	int GetNumPlayerColorInfos();
#endif
	std::vector<CvPlayerColorInfo*>& GetPlayerColorInfo();
	_Ret_maybenull_ CvPlayerColorInfo* GetPlayerColorInfo(PlayerColorTypes e);

#ifdef AUI_WARNING_FIXES
	uint getNumEntityEventInfos() const;
#else
	int getNumEntityEventInfos();
#endif
	std::vector<CvEntityEventInfo*>& getEntityEventInfo();
	_Ret_maybenull_ CvEntityEventInfo* getEntityEventInfo(EntityEventTypes e);

#ifdef AUI_WARNING_FIXES
	uint getNumMultiUnitFormationInfos() const;
#else
	int getNumMultiUnitFormationInfos();
#endif
	std::vector<CvMultiUnitFormationInfo*>& getMultiUnitFormationInfo();
	_Ret_maybenull_ CvMultiUnitFormationInfo* getMultiUnitFormationInfo(int i);

#ifdef AUI_WARNING_FIXES
	uint getNumTerrainInfos() const;
#else
	int getNumTerrainInfos();
#endif
	std::vector<CvTerrainInfo*>& getTerrainInfo();
	CvTerrainInfo* getTerrainInfo(TerrainTypes eTerrainNum);
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	int getNumGreatWorkClassInfos();
	std::vector<CvGreatWorkClassInfo*>& getGreatWorkClassInfo();
	CvGreatWorkClassInfo* getGreatWorkClassInfo(GreatWorkClass eGreatWorkClassNum);
#endif
#ifdef AUI_WARNING_FIXES
	uint getNumResourceClassInfos() const;
#else
	int getNumResourceClassInfos();
#endif
	std::vector<CvResourceClassInfo*>& getResourceClassInfo();
	_Ret_maybenull_ CvResourceClassInfo* getResourceClassInfo(ResourceClassTypes eResourceNum);
#if defined(TRADE_REFACTOR)
	int getNumTradeConnectionInfos();
	std::vector<CvTradeConnectionInfo*>& getTradeConnectionInfo();
	_Ret_maybenull_ CvTradeConnectionInfo* getTradeConnectionInfo(TradeConnectionType e);
#endif
#ifdef AUI_WARNING_FIXES
	uint getNumResourceInfos() const;
#else
	int getNumResourceInfos();
#endif
	std::vector<CvResourceInfo*>& getResourceInfo();
	CvResourceInfo* getResourceInfo(ResourceTypes eResourceNum);

#ifdef AUI_WARNING_FIXES
	uint getNumFeatureInfos() const;
#else
	int getNumFeatureInfos();
#endif
	std::vector<CvFeatureInfo*>& getFeatureInfo();
	CvFeatureInfo* getFeatureInfo(FeatureTypes eFeatureNum);

	int& getNumPlayableCivilizationInfos();
	int& getNumAIPlayableCivilizationInfos();
#ifdef AUI_WARNING_FIXES
	uint getNumCivilizationInfos() const;
#else
	int getNumCivilizationInfos();
#endif
	std::vector<CvCivilizationInfo*>& getCivilizationInfo();
	_Ret_maybenull_ CvCivilizationInfo* getCivilizationInfo(CivilizationTypes eCivilizationNum);
	CivilizationTypes getCivilizationInfoIndex(const char* pszType);

#ifdef AUI_WARNING_FIXES
	uint getNumMinorCivInfos() const;
	uint GetNumPlayableMinorCivs() const;
#else
	int getNumMinorCivInfos();
	int GetNumPlayableMinorCivs() const;
#endif
	std::vector<CvMinorCivInfo*>& getMinorCivInfo();
	CvMinorCivInfo* getMinorCivInfo(MinorCivTypes eMinorCivNum);

#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
#ifdef AUI_WARNING_FIXES
	uint getNumMinorCivPersonalityInfos() const;
#else
	int getNumMinorCivPersonalityInfos();
#endif
	std::vector<CvMinorCivPersonalityInfo*>& getMinorCivPersonalityInfo();
	_Ret_maybenull_ CvMinorCivPersonalityInfo* getMinorCivPersonalityInfo(int ePersonality);
#endif

#ifdef AUI_WARNING_FIXES
	uint getNumLeaderHeadInfos() const;
#else
	int getNumLeaderHeadInfos();
#endif
	std::vector<CvLeaderHeadInfo*>& getLeaderHeadInfo();
	_Ret_maybenull_ CvLeaderHeadInfo* getLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum);

#ifdef AUI_WARNING_FIXES
	uint getNumUnitInfos() const;
#else
	int getNumUnitInfos();
#endif
	std::vector<CvUnitEntry*>& getUnitInfo();
#ifdef AUI_CONSTIFY
	_Ret_maybenull_ CvUnitEntry* getUnitInfo(UnitTypes eUnitNum) const;
#else
	_Ret_maybenull_ CvUnitEntry* getUnitInfo(UnitTypes eUnitNum);
#endif
	CvUnitXMLEntries* GetGameUnits() const;

#ifdef AUI_WARNING_FIXES
	uint getNumSpecialUnitInfos() const;
#else
	int getNumSpecialUnitInfos();
#endif
	std::vector<CvSpecialUnitInfo*>& getSpecialUnitInfo();
	_Ret_maybenull_ CvSpecialUnitInfo* getSpecialUnitInfo(SpecialUnitTypes eSpecialUnitNum);

#ifdef AUI_WARNING_FIXES
	uint getNumVoteSourceInfos() const;
#else
	int getNumVoteSourceInfos();
#endif
	std::vector<CvVoteSourceInfo*>& getVoteSourceInfo();
	_Ret_maybenull_ CvVoteSourceInfo* getVoteSourceInfo(VoteSourceTypes e);

#ifdef AUI_WARNING_FIXES
	uint getNumUnitCombatClassInfos() const;
#else
	int getNumUnitCombatClassInfos();
#endif
	std::vector<CvBaseInfo*>& getUnitCombatClassInfo();
	_Ret_maybenull_ CvBaseInfo* getUnitCombatClassInfo(UnitCombatTypes e);

	std::vector<CvBaseInfo*>& getUnitAIInfo();
	_Ret_maybenull_ CvBaseInfo* getUnitAIInfo(UnitAITypes eUnitAINum);

#ifdef AUI_WARNING_FIXES
	uint getNumGameOptionInfos() const;
#else
	int getNumGameOptionInfos();
#endif
	std::vector<CvGameOptionInfo*>& getGameOptionInfo();
	_Ret_maybenull_ CvGameOptionInfo* getGameOptionInfo(GameOptionTypes eGameOptionNum);

#ifdef AUI_WARNING_FIXES
	uint getNumMPOptionInfos() const;
#else
	int getNumMPOptionInfos();
#endif
	std::vector<CvMPOptionInfo*>& getMPOptionInfo();
	_Ret_maybenull_ CvMPOptionInfo* getMPOptionInfo(MultiplayerOptionTypes eMPOptionNum);

	std::vector<CvPlayerOptionInfo*>& getPlayerOptionInfo();
	_Ret_maybenull_ CvPlayerOptionInfo* getPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum);

	std::vector<CvYieldInfo*>& getYieldInfo();
#ifdef AUI_CONSTIFY
	_Ret_maybenull_ CvYieldInfo* getYieldInfo(YieldTypes eYieldNum) const;
#else
	_Ret_maybenull_ CvYieldInfo* getYieldInfo(YieldTypes eYieldNum);
#endif

#ifdef AUI_WARNING_FIXES
	uint getNumRouteInfos() const;
#else
	int getNumRouteInfos();
#endif
	std::vector<CvRouteInfo*>& getRouteInfo();
	_Ret_maybenull_ CvRouteInfo* getRouteInfo(RouteTypes eRouteNum);

#ifdef AUI_WARNING_FIXES
	uint getNumImprovementInfos() const;
#else
	int getNumImprovementInfos();
#endif
	std::vector<CvImprovementEntry*>& getImprovementInfo();
	CvImprovementEntry* getImprovementInfo(ImprovementTypes eImprovementNum);
	CvImprovementXMLEntries* GetGameImprovements() const;

#ifdef AUI_WARNING_FIXES
	uint getNumBuildInfos() const;
#else
	int getNumBuildInfos();
#endif
	std::vector<CvBuildInfo*>& getBuildInfo();
	CvBuildInfo* getBuildInfo(BuildTypes eBuildNum);

#ifdef AUI_WARNING_FIXES
	uint getNumHandicapInfos() const;
#else
	int getNumHandicapInfos();
#endif
	std::vector<CvHandicapInfo*>& getHandicapInfo();
	_Ret_maybenull_ CvHandicapInfo* getHandicapInfo(HandicapTypes eHandicapNum);

#ifdef AUI_WARNING_FIXES
	uint getNumGameSpeedInfos() const;
#else
	int getNumGameSpeedInfos();
#endif
	std::vector<CvGameSpeedInfo*>& getGameSpeedInfo();
	_Ret_maybenull_ CvGameSpeedInfo* getGameSpeedInfo(GameSpeedTypes eGameSpeedNum);

#ifdef AUI_WARNING_FIXES
	uint getNumProcessInfos() const;
#else
	int getNumProcessInfos();
#endif
	std::vector<CvProcessInfo*>& getProcessInfo();
	CvProcessInfo* getProcessInfo(ProcessTypes e);

#ifdef AUI_WARNING_FIXES
	uint getNumVoteInfos() const;
#else
	int getNumVoteInfos();
#endif
	std::vector<CvVoteInfo*>& getVoteInfo();
	CvVoteInfo* getVoteInfo(VoteTypes e);

#ifdef AUI_WARNING_FIXES
	uint getNumProjectInfos() const;
#else
	int getNumProjectInfos();
#endif
	std::vector<CvProjectEntry*>& getProjectInfo();
	CvProjectEntry* getProjectInfo(ProjectTypes e);
	CvProjectXMLEntries* GetGameProjects() const;

#ifdef AUI_WARNING_FIXES
	uint getNumBuildingClassInfos() const;
#else
	int getNumBuildingClassInfos();
#endif
	std::vector<CvBuildingClassInfo*>& getBuildingClassInfo();
	_Ret_maybenull_ CvBuildingClassInfo* getBuildingClassInfo(BuildingClassTypes eBuildingClassNum);

#ifdef AUI_WARNING_FIXES
	uint getNumBuildingInfos() const;
#else
	int getNumBuildingInfos();
#endif
	std::vector<CvBuildingEntry*>& getBuildingInfo();
	_Ret_maybenull_ CvBuildingEntry* getBuildingInfo(BuildingTypes eBuildingNum);
	CvBuildingXMLEntries* GetGameBuildings() const;

#ifdef AUI_WARNING_FIXES
	uint getNumUnitClassInfos() const;
#else
	int getNumUnitClassInfos();
#endif
	std::vector<CvUnitClassInfo*>& getUnitClassInfo();
#ifdef AUI_CONSTIFY
	_Ret_maybenull_ CvUnitClassInfo* getUnitClassInfo(UnitClassTypes eUnitClassNum) const;
#else
	_Ret_maybenull_ CvUnitClassInfo* getUnitClassInfo(UnitClassTypes eUnitClassNum);
#endif

#ifdef AUI_WARNING_FIXES
	uint getNumActionInfos() const;
#else
	int getNumActionInfos();
#endif
	std::vector<CvActionInfo*>& getActionInfo();
#ifdef AUI_WARNING_FIXES
	CvActionInfo* getActionInfo(uint i);
#else
	CvActionInfo* getActionInfo(int i);
#endif
#if defined(LEKMOD_TRAIT_BAN_UNIT_MISSIONS)
	int getNumMissionInfos();
#endif
	std::vector<CvMissionInfo*>& getMissionInfo();
	_Ret_maybenull_ CvMissionInfo* getMissionInfo(MissionTypes eMissionNum);

	std::vector<CvControlInfo*>& getControlInfo();
	_Ret_maybenull_ CvControlInfo* getControlInfo(ControlTypes eControlNum);

	std::vector<CvCommandInfo*>& getCommandInfo();
	_Ret_maybenull_ CvCommandInfo* getCommandInfo(CommandTypes eCommandNum);

#ifdef AUI_WARNING_FIXES
	uint getNumAutomateInfos() const;
#else
	int getNumAutomateInfos();
#endif
	std::vector<CvAutomateInfo*>& getAutomateInfo();
#ifdef AUI_WARNING_FIXES
	_Ret_maybenull_ CvAutomateInfo* getAutomateInfo(uint iAutomateNum);
#else
	_Ret_maybenull_ CvAutomateInfo* getAutomateInfo(int iAutomateNum);
#endif

#ifdef AUI_WARNING_FIXES
	uint getNumPromotionInfos() const;
#else
	int getNumPromotionInfos();
#endif
	std::vector<CvPromotionEntry*>& getPromotionInfo();
	_Ret_maybenull_ CvPromotionEntry* getPromotionInfo(PromotionTypes ePromotionNum);
	CvPromotionXMLEntries* GetGamePromotions() const;

#ifdef AUI_WARNING_FIXES
	uint getNumSpecialistInfos() const;
#else
	int getNumSpecialistInfos();
#endif
	std::vector<CvSpecialistInfo*>& getSpecialistInfo();
	_Ret_maybenull_ CvSpecialistInfo* getSpecialistInfo(SpecialistTypes eSpecialistNum);

#ifdef AUI_WARNING_FIXES
	uint getNumEconomicAIStrategyInfos() const;
#else
	int getNumEconomicAIStrategyInfos();
#endif
	std::vector<CvEconomicAIStrategyXMLEntry*>& getEconomicAIStrategyInfo();
	_Ret_maybenull_ CvEconomicAIStrategyXMLEntry* getEconomicAIStrategyInfo(EconomicAIStrategyTypes eAIStrategyNum);
	CvEconomicAIStrategyXMLEntries* GetGameEconomicAIStrategies() const;

#ifdef AUI_WARNING_FIXES
	uint getNumCitySpecializationInfos() const;
#else
	int getNumCitySpecializationInfos();
#endif
	std::vector<CvCitySpecializationXMLEntry*>& getCitySpecializationInfo();
	_Ret_maybenull_ CvCitySpecializationXMLEntry* getCitySpecializationInfo(CitySpecializationTypes eCitySpecialization);
	CvCitySpecializationXMLEntries* GetGameCitySpecializations() const;

#ifdef AUI_WARNING_FIXES
	uint getNumTacticalMoveInfos() const;
#else
	int getNumTacticalMoveInfos();
#endif
	std::vector<CvTacticalMoveXMLEntry*>& getTacticalMoveInfo();
	_Ret_maybenull_ CvTacticalMoveXMLEntry* getTacticalMoveInfo(TacticalAIMoveTypes eTacticalMove);
	CvTacticalMoveXMLEntries* GetGameTacticalMoves() const;

#ifdef AUI_WARNING_FIXES
	uint getNumMilitaryAIStrategyInfos() const;
#else
	int getNumMilitaryAIStrategyInfos();
#endif
	std::vector<CvMilitaryAIStrategyXMLEntry*>& getMilitaryAIStrategyInfo();
	_Ret_maybenull_ CvMilitaryAIStrategyXMLEntry* getMilitaryAIStrategyInfo(MilitaryAIStrategyTypes eAIStrategyNum);
	CvMilitaryAIStrategyXMLEntries* GetGameMilitaryAIStrategies() const;

#ifdef AUI_WARNING_FIXES
	uint getNumAIGrandStrategyInfos() const;
#else
	int getNumAIGrandStrategyInfos();
#endif
	std::vector<CvAIGrandStrategyXMLEntry*>& getAIGrandStrategyInfo();
	_Ret_maybenull_ CvAIGrandStrategyXMLEntry* getAIGrandStrategyInfo(AIGrandStrategyTypes eAIGrandStrategyNum);
	CvAIGrandStrategyXMLEntries* GetGameAIGrandStrategies() const;

#ifdef AUI_WARNING_FIXES
	uint getNumAICityStrategyInfos() const;
#else
	int getNumAICityStrategyInfos();
#endif
	std::vector<CvAICityStrategyEntry*>& getAICityStrategyInfo();
	_Ret_maybenull_ CvAICityStrategyEntry* getAICityStrategyInfo(AICityStrategyTypes eAICityStrategyNum);
	CvAICityStrategies* GetGameAICityStrategies() const;

#ifdef AUI_WARNING_FIXES
	uint getNumPolicyInfos() const;
#else
	int getNumPolicyInfos();
#endif
	std::vector<CvPolicyEntry*>& getPolicyInfo();
	_Ret_maybenull_ CvPolicyEntry* getPolicyInfo(PolicyTypes ePolicyNum);
	CvPolicyXMLEntries* GetGamePolicies() const;

#ifdef AUI_WARNING_FIXES
	uint getNumPolicyBranchInfos() const;
#else
	int getNumPolicyBranchInfos();
#endif
	std::vector<CvPolicyBranchEntry*>& getPolicyBranchInfo();
	_Ret_maybenull_ CvPolicyBranchEntry* getPolicyBranchInfo(PolicyBranchTypes ePolicyBranchNum);

#ifdef AUI_WARNING_FIXES
	uint getNumEmphasisInfos() const;
#else
	int getNumEmphasisInfos();
#endif
	std::vector<CvEmphasisEntry*>& getEmphasisInfo();
	_Ret_maybenull_ CvEmphasisEntry* getEmphasisInfo(EmphasizeTypes eEmphasisNum);
	CvEmphasisXMLEntries* GetGameEmphases() const;

#ifdef AUI_WARNING_FIXES
	uint getNumTraitInfos() const;
#else
	int getNumTraitInfos();
#endif
	std::vector<CvTraitEntry*>& getTraitInfo();
	_Ret_maybenull_ CvTraitEntry* getTraitInfo(TraitTypes eTraitNum);
	CvTraitXMLEntries* GetGameTraits() const;

#ifdef AUI_WARNING_FIXES
	uint getNumReligionInfos() const;
#else
	int getNumReligionInfos();
#endif
	std::vector<CvReligionEntry*>& getReligionInfo();
	_Ret_maybenull_ CvReligionEntry* getReligionInfo(ReligionTypes eReligionNum);
	CvReligionXMLEntries* GetGameReligions() const;

#ifdef AUI_WARNING_FIXES
	uint getNumBeliefInfos() const;
#else
	int getNumBeliefInfos();
#endif
	std::vector<CvBeliefEntry*>& getBeliefInfo();
	_Ret_maybenull_ CvBeliefEntry* getBeliefInfo(BeliefTypes eBeliefNum);
	CvBeliefXMLEntries* GetGameBeliefs() const;

#ifdef AUI_WARNING_FIXES
	uint getNumLeagueSpecialSessionInfos() const;
#else
	int getNumLeagueSpecialSessionInfos();
#endif
	std::vector<CvLeagueSpecialSessionEntry*>& getLeagueSpecialSessionInfo();
	_Ret_maybenull_ CvLeagueSpecialSessionEntry* getLeagueSpecialSessionInfo(LeagueSpecialSessionTypes eLeagueSpecialSessionNum);
	CvLeagueSpecialSessionXMLEntries* GetGameLeagueSpecialSessions() const;
	
#ifdef AUI_WARNING_FIXES
	uint getNumLeagueNameInfos() const;
#else
	int getNumLeagueNameInfos();
#endif
	std::vector<CvLeagueNameEntry*>& getLeagueNameInfo();
	_Ret_maybenull_ CvLeagueNameEntry* getLeagueNameInfo(LeagueNameTypes eLeagueNameNum);
	CvLeagueNameXMLEntries* GetGameLeagueNames() const;
	
#ifdef AUI_WARNING_FIXES
	uint getNumLeagueProjectInfos() const;
#else
	int getNumLeagueProjectInfos();
#endif
	std::vector<CvLeagueProjectEntry*>& getLeagueProjectInfo();
	_Ret_maybenull_ CvLeagueProjectEntry* getLeagueProjectInfo(LeagueProjectTypes eLeagueProjectNum);
	CvLeagueProjectXMLEntries* GetGameLeagueProjects() const;

#ifdef AUI_WARNING_FIXES
	uint getNumLeagueProjectRewardInfos() const;
#else
	int getNumLeagueProjectRewardInfos();
#endif
	std::vector<CvLeagueProjectRewardEntry*>& getLeagueProjectRewardInfo();
	_Ret_maybenull_ CvLeagueProjectRewardEntry* getLeagueProjectRewardInfo(LeagueProjectRewardTypes eLeagueProjectRewardNum);
	CvLeagueProjectRewardXMLEntries* GetGameLeagueProjectRewards() const;

#ifdef AUI_WARNING_FIXES
	uint getNumResolutionInfos() const;
#else
	int getNumResolutionInfos();
#endif
	std::vector<CvResolutionEntry*>& getResolutionInfo();
	_Ret_maybenull_ CvResolutionEntry* getResolutionInfo(ResolutionTypes eResolutionNum);
	CvResolutionXMLEntries* GetGameResolutions() const;

#ifdef AUI_WARNING_FIXES
	uint getNumTechInfos() const;
#else
	int getNumTechInfos();
#endif
	std::vector<CvTechEntry*>& getTechInfo();
	_Ret_maybenull_ CvTechEntry* getTechInfo(TechTypes eTechNum);
	CvTechXMLEntries* GetGameTechs() const;

#ifdef AUI_WARNING_FIXES
	uint getNumEraInfos() const;
#else
	int getNumEraInfos();
#endif
	std::vector<CvEraInfo*>& getEraInfo();
	CvEraInfo* getEraInfo(EraTypes eEraNum);

#ifdef AUI_WARNING_FIXES
	uint getNumHurryInfos() const;
#else
	int getNumHurryInfos();
#endif
	std::vector<CvHurryInfo*>& getHurryInfo();
	_Ret_maybenull_ CvHurryInfo* getHurryInfo(HurryTypes eHurryNum);

#ifdef AUI_WARNING_FIXES
	uint getNumVictoryInfos() const;
#else
	int getNumVictoryInfos();
#endif
	std::vector<CvVictoryInfo*>& getVictoryInfo();
	_Ret_maybenull_ CvVictoryInfo* getVictoryInfo(VictoryTypes eVictoryNum);

#ifdef AUI_WARNING_FIXES
	uint getNumSmallAwardInfos() const;
#else
	int getNumSmallAwardInfos();
#endif
	std::vector<CvSmallAwardInfo*>& getSmallAwardInfo();
	_Ret_maybenull_ CvSmallAwardInfo* getSmallAwardInfo(SmallAwardTypes eSmallAwardNum);

	CvNotificationXMLEntries* GetNotificationEntries();

	//
	// Global Types
	// All type strings are upper case and are kept in this hash map for fast lookup
	// The other functions are kept for convenience when enumerating, but most are not used
	//
	int getNUM_YIELD_TYPES() const;
	int getNUM_CONTROL_TYPES() const;

	// int& getNumFlavorTypes();
	// CvString*& getFlavorTypes();
	// CvString& getFlavorTypes(FlavorTypes e);
	void       setNumFlavorTypes(const int iTypes)
	{
		m_iNumFlavorTypes = iTypes;
	}
	int        getNumFlavorTypes()
	{
		return m_iNumFlavorTypes;
	}
	CvString*& getFlavorTypes()
	{
		return m_paszFlavorTypes;
	}
	CvString&  getFlavorTypes(FlavorTypes e)
	{
		CvAssert(e > -1); /*CvAssert(e < GC.getNumFlavorTypes())*/;
		return m_paszFlavorTypes[e];
	}

#ifdef AUI_WARNING_FIXES
	uint getNumUnitDomainInfos() const;
#else
	int getNumUnitDomainInfos();
#endif
	std::vector<CvDomainInfo*>& getUnitDomainInfo();
	_Ret_maybenull_ CvDomainInfo* getUnitDomainInfo(DomainTypes eDomainNum);

	CvString*& getFootstepAudioTags();
	CvString& getFootstepAudioTags(int i);

	const char** GetHexDebugLayerNames();
	float GetHexDebugLayerScale(const char* szLayerName);
	bool GetHexDebugLayerString(CvPlot* pkPlot, const char* szLayerName, PlayerTypes ePlayer, char* szBuffer, unsigned int uiBufferLength);

	///////////////// BEGIN global defines
	// THESE ARE READ-ONLY
	//
	void cacheGlobals();

	// use very sparingly - this is costly
	CvString getDefineSTRING(const char* szName, bool bReportErrors = true);

#ifdef AUI_DIPLOMACY_AI_LEADERHEAD_DEALS_IN_MULTIPLAYER
	GD_INT_DEF(MULTIPLAYER_AI_DIPLOMACY_NOTIFICATIONS_ONLY_SIMULTANEOUS);
#endif
#ifdef AUI_UNIT_PRODUCTION_AI_LUA_FLAVOR_WEIGHTS
	GD_INT_DEF(DISABLE_UNIT_AI_FLAVOR_LUA_MODDING);
#endif
#ifdef AUI_BUILDING_PRODUCTION_AI_LUA_FLAVOR_WEIGHTS
	GD_INT_DEF(DISABLE_BUILDING_AI_FLAVOR_LUA_MODDING);
#endif
#ifdef AUI_PROJECT_PRODUCTION_AI_LUA_FLAVOR_WEIGHTS
	GD_INT_DEF(DISABLE_PROJECT_AI_FLAVOR_LUA_MODDING);
#endif
#ifdef AUI_PROCESS_PRODUCTION_AI_LUA_FLAVOR_WEIGHTS
	GD_INT_DEF(DISABLE_PROCESS_AI_FLAVOR_LUA_MODDING);
#endif
#ifdef NQM_OPTIONAL_SCORING_TWEAKS
	GD_INT_DEF(NEW_SCORE_CITY_MULTIPLIER);
	GD_INT_DEF(NEW_SCORE_POPULATION_MULTIPLIER);
	GD_INT_DEF(NEW_SCORE_WONDER_MULTIPLIER);
	GD_INT_DEF(NEW_SCORE_TECH_MULTIPLIER);
	GD_INT_DEF(NEW_SCORE_FUTURE_TECH_MULTIPLIER);
	GD_INT_DEF(NEW_SCORE_POLICY_MULTIPLIER);
	GD_INT_DEF(NEW_SCORE_GREAT_WORK_MULTIPLIER);
	GD_INT_DEF(NEW_SCORE_BELIEF_MULTIPLIER);
#endif

	// -- ints --

	inline int getAI_ATTEMPT_RUSH_OVER_X_TURNS_TO_BUILD()
	{
		return m_iAI_ATTEMPT_RUSH_OVER_X_TURNS_TO_BUILD;
	}
	inline int getBARBARIAN_UNIT_GOLD_RANSOM_exp()
	{
		return m_iBARBARIAN_UNIT_GOLD_RANSOM;
	}
	inline int getINITIAL_AI_CITY_PRODUCTION()
	{
		return m_iINITIAL_AI_CITY_PRODUCTION;
	}
	inline int getAI_CAN_DISBAND_UNITS()
	{
		return m_iAI_CAN_DISBAND_UNITS;
	}
	inline int getAI_SHOULDNT_MANAGE_PLOT_ASSIGNMENT()
	{
		return m_iAI_SHOULDNT_MANAGE_PLOT_ASSIGNMENT;
	}
	inline int getPOLICY_WEIGHT_PROPAGATION_PERCENT()
	{
		return m_iPOLICY_WEIGHT_PROPAGATION_PERCENT;
	}
	inline int getPOLICY_WEIGHT_PROPAGATION_LEVELS()
	{
		return m_iPOLICY_WEIGHT_PROPAGATION_LEVELS;
	}
	inline int getPOLICY_WEIGHT_PERCENT_DROP_NEW_BRANCH()
	{
		return m_iPOLICY_WEIGHT_PERCENT_DROP_NEW_BRANCH;
	}
	inline int getTECH_WEIGHT_PROPAGATION_PERCENT()
	{
		return m_iTECH_WEIGHT_PROPAGATION_PERCENT;
	}
	inline int getTECH_WEIGHT_PROPAGATION_LEVELS()
	{
		return m_iTECH_WEIGHT_PROPAGATION_LEVELS;
	}
	inline int getTECH_PRIORITY_UNIQUE_ITEM()
	{
		return m_iTECH_PRIORITY_UNIQUE_ITEM;
	}
	inline int getTECH_PRIORITY_MAYA_CALENDAR_BONUS()
	{
		return m_iTECH_PRIORITY_MAYA_CALENDAR_BONUS;
	}
	inline int getAI_VALUE_OF_YIELD_GOLD()
	{
		return m_iAI_VALUE_OF_YIELD_GOLD;
	}
	inline int getAI_VALUE_OF_YIELD_PRODUCTION()
	{
		return m_iAI_VALUE_OF_YIELD_PRODUCTION;
	}
	inline int getAI_VALUE_OF_YIELD_SCIENCE()
	{
		return m_iAI_VALUE_OF_YIELD_SCIENCE;
	}
	inline int getDEFAULT_FLAVOR_VALUE()
	{
		return m_iDEFAULT_FLAVOR_VALUE;
	}
	inline int getPERSONALITY_FLAVOR_MAX_VALUE()
	{
		return m_iPERSONALITY_FLAVOR_MAX_VALUE;
	}
	inline int getPERSONALITY_FLAVOR_MIN_VALUE()
	{
		return m_iPERSONALITY_FLAVOR_MIN_VALUE;
	}
	inline int getFLAVOR_MIN_VALUE()
	{
		return m_iFLAVOR_MIN_VALUE;
	}
	inline int getFLAVOR_MAX_VALUE()
	{
		return m_iFLAVOR_MAX_VALUE;
	}
	inline int getFLAVOR_RANDOMIZATION_RANGE()
	{
		return m_iFLAVOR_RANDOMIZATION_RANGE;
	}
	inline int getFLAVOR_EXPANDGROW_COEFFICIENT()
	{
		return m_iFLAVOR_EXPANDGROW_COEFFICIENT;
	}
	inline int getAI_GRAND_STRATEGY_NUM_TURNS_STRATEGY_MUST_BE_ACTIVE()
	{
		return m_iAI_GRAND_STRATEGY_NUM_TURNS_STRATEGY_MUST_BE_ACTIVE;
	}
	inline int getAI_GS_RAND_ROLL()
	{
		return m_iAI_GS_RAND_ROLL;
	}
	inline int getAI_GRAND_STRATEGY_CURRENT_STRATEGY_WEIGHT()
	{
		return m_iAI_GRAND_STRATEGY_CURRENT_STRATEGY_WEIGHT;
	}
	inline int getAI_GRAND_STRATEGY_GUESS_NO_CLUE_WEIGHT()
	{
		return m_iAI_GRAND_STRATEGY_GUESS_NO_CLUE_WEIGHT;
	}
	inline int getAI_GRAND_STRATEGY_GUESS_POSITIVE_THRESHOLD()
	{
		return m_iAI_GRAND_STRATEGY_GUESS_POSITIVE_THRESHOLD;
	}
	inline int getAI_GRAND_STRATEGY_GUESS_LIKELY_THRESHOLD()
	{
		return m_iAI_GRAND_STRATEGY_GUESS_LIKELY_THRESHOLD;
	}
	inline int getAI_GRAND_STRATEGY_OTHER_PLAYERS_GS_MULTIPLIER()
	{
		return m_iAI_GRAND_STRATEGY_OTHER_PLAYERS_GS_MULTIPLIER;
	}
	inline int getAI_GS_CONQUEST_NOBODY_MET_FIRST_TURN()
	{
		return m_iAI_GS_CONQUEST_NOBODY_MET_FIRST_TURN;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_NOBODY_MET_WEIGHT()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_NOBODY_MET_WEIGHT;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_AT_WAR_WEIGHT()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_AT_WAR_WEIGHT;
	}
	inline int getAI_GS_CONQUEST_MILITARY_STRENGTH_FIRST_TURN()
	{
		return m_iAI_GS_CONQUEST_MILITARY_STRENGTH_FIRST_TURN;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_POWER_RATIO_MULTIPLIER()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_POWER_RATIO_MULTIPLIER;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_CRAMPED_WEIGHT()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_CRAMPED_WEIGHT;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_ATTACKED()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_ATTACKED;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_CONQUERED()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_CONQUERED;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_ATTACKED()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_ATTACKED;
	}
	inline int getAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_CONQUERED()
	{
		return m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_CONQUERED;
	}
	inline int getAI_GS_CULTURE_RATIO_MULTIPLIER()
	{
		return m_iAI_GS_CULTURE_RATIO_MULTIPLIER;
	}
	inline int getAI_GS_TOURISM_RATIO_MULTIPLIER()
	{
		return m_iAI_GS_TOURISM_RATIO_MULTIPLIER;
	}
	inline int getAI_GS_CULTURE_AHEAD_WEIGHT()
	{
		return m_iAI_GS_CULTURE_AHEAD_WEIGHT;
	}
	inline int getAI_GS_CULTURE_TOURISM_AHEAD_WEIGHT()
	{
		return m_iAI_GS_CULTURE_TOURISM_AHEAD_WEIGHT;
	}
	inline int getAI_GS_CULTURE_INFLUENTIAL_CIV_MOD()
	{
		return m_iAI_GS_CULTURE_INFLUENTIAL_CIV_MOD;
	}
	inline int getAI_GRAND_STRATEGY_UN_EACH_MINOR_ATTACKED_WEIGHT()
	{
		return m_iAI_GRAND_STRATEGY_UN_EACH_MINOR_ATTACKED_WEIGHT;
	}
	inline int getAI_GS_UN_SECURED_VOTE_MOD()
	{
		return m_iAI_GS_UN_SECURED_VOTE_MOD;
	}
	inline int getAI_GS_SS_HAS_APOLLO_PROGRAM()
	{
		return m_iAI_GS_SS_HAS_APOLLO_PROGRAM;
	}
	inline int getAI_GS_SS_TECH_PROGRESS_MOD()
	{
		return m_iAI_GS_SS_TECH_PROGRESS_MOD;
	}
	inline int getAI_STRATEGY_EARLY_EXPLORATION_STARTING_WEIGHT()
	{
		return m_iAI_STRATEGY_EARLY_EXPLORATION_STARTING_WEIGHT;
	}
	inline int getAI_STRATEGY_EARLY_EXPLORATION_EXPLORERS_WEIGHT_DIVISOR()
	{
		return m_iAI_STRATEGY_EARLY_EXPLORATION_EXPLORERS_WEIGHT_DIVISOR;
	}
	inline int getAI_STRATEGY_EARLY_EXPLORATION_WEIGHT_PER_FLAVOR()
	{
		return m_iAI_STRATEGY_EARLY_EXPLORATION_WEIGHT_PER_FLAVOR;
	}
	inline int getAI_STRATEGY_EARLY_EXPANSION_NUM_UNOWNED_TILES_REQUIRED()
	{
		return m_iAI_STRATEGY_EARLY_EXPANSION_NUM_UNOWNED_TILES_REQUIRED;
	}
	inline int getAI_STRATEGY_EARLY_EXPANSION_NUM_CITIES_LIMIT()
	{
		return m_iAI_STRATEGY_EARLY_EXPANSION_NUM_CITIES_LIMIT;
	}
	inline float getAI_STRATEGY_NEED_IMPROVEMENT_CITY_RATIO()
	{
		return m_fAI_STRATEGY_NEED_IMPROVEMENT_CITY_RATIO;
	}
	inline int getAI_STRATEGY_ISLAND_START_COAST_REVEAL_PERCENT()
	{
		return m_iAI_STRATEGY_ISLAND_START_COAST_REVEAL_PERCENT;
	}
	inline int getAI_STRATEGY_AREA_IS_FULL_PERCENT()
	{
		return m_iAI_STRATEGY_AREA_IS_FULL_PERCENT;
	}
	inline int getAI_STRATEGY_MINIMUM_SETTLE_FERTILITY()
	{
		return m_iAI_STRATEGY_MINIMUM_SETTLE_FERTILITY;
	}
	inline int getAI_BUY_PLOT_TEST_PROBES()
	{
		return m_iAI_BUY_PLOT_TEST_PROBES;
	}
	inline int getAI_PLOT_VALUE_STRATEGIC_RESOURCE()
	{
		return m_iAI_PLOT_VALUE_STRATEGIC_RESOURCE;
	}
	inline int getAI_PLOT_VALUE_LUXURY_RESOURCE()
	{
		return m_iAI_PLOT_VALUE_LUXURY_RESOURCE;
	}
	inline int getAI_PLOT_VALUE_SPECIALIZATION_MULTIPLIER()
	{
		return m_iAI_PLOT_VALUE_SPECIALIZATION_MULTIPLIER;
	}
	inline int getAI_PLOT_VALUE_YIELD_MULTIPLIER()
	{
		return m_iAI_PLOT_VALUE_YIELD_MULTIPLIER;
	}
	inline int getAI_PLOT_VALUE_DEFICIENT_YIELD_MULTIPLIER()
	{
		return m_iAI_PLOT_VALUE_DEFICIENT_YIELD_MULTIPLIER;
	}
	inline int getAI_PLOT_VALUE_FIERCE_DISPUTE()
	{
		return m_iAI_PLOT_VALUE_FIERCE_DISPUTE;
	}
	inline int getAI_PLOT_VALUE_STRONG_DISPUTE()
	{
		return m_iAI_PLOT_VALUE_STRONG_DISPUTE;
	}
	inline int getAI_PLOT_VALUE_WEAK_DISPUTE()
	{
		return m_iAI_PLOT_VALUE_WEAK_DISPUTE;
	}
	inline int getAI_GOLD_PRIORITY_MINIMUM_PLOT_BUY_VALUE()
	{
		return m_iAI_GOLD_PRIORITY_MINIMUM_PLOT_BUY_VALUE;
	}
	inline int getAI_GOLD_BALANCE_TO_HALVE_PLOT_BUY_MINIMUM()
	{
		return m_iAI_GOLD_BALANCE_TO_HALVE_PLOT_BUY_MINIMUM;
	}
	inline int getAI_GOLD_PRIORITY_UPGRADE_BASE()
	{
		return m_iAI_GOLD_PRIORITY_UPGRADE_BASE;
	}
	inline int getAI_GOLD_PRIORITY_UPGRADE_PER_FLAVOR_POINT()
	{
		return m_iAI_GOLD_PRIORITY_UPGRADE_PER_FLAVOR_POINT;
	}
	inline int getAI_GOLD_PRIORITY_DIPLOMACY_BASE()
	{
		return m_iAI_GOLD_PRIORITY_DIPLOMACY_BASE;
	}
	inline int getAI_GOLD_PRIORITY_DIPLOMACY_PER_FLAVOR_POINT()
	{
		return m_iAI_GOLD_PRIORITY_DIPLOMACY_PER_FLAVOR_POINT;
	}
	inline int getAI_GOLD_PRIORITY_UNIT()
	{
		return m_iAI_GOLD_PRIORITY_UNIT;
	}
	inline int getAI_GOLD_PRIORITY_DEFENSIVE_BUILDING()
	{
		return m_iAI_GOLD_PRIORITY_DEFENSIVE_BUILDING;
	}
	inline int getAI_GOLD_PRIORITY_BUYOUT_CITY_STATE()
	{
		return m_iAI_GOLD_PRIORITY_BUYOUT_CITY_STATE;
	}
	inline int getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_FOOD()
	{
		return m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_FOOD;
	}
	inline int getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_PRODUCTION()
	{
		return m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_PRODUCTION;
	}
	inline int getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_GOLD()
	{
		return m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_GOLD;
	}
	inline int getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_SCIENCE()
	{
		return m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_SCIENCE;
	}
	inline int getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_CULTURE()
	{
		return m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_CULTURE;
	}
	inline int getBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_LUXURY_RESOURCE()
	{
		return m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_LUXURY_RESOURCE;
	}
	inline int getBUILDER_TASKING_IMPROVE_RESOURCE_TENDENCY()
	{
		return m_iBUILDER_TASKING_IMPROVE_RESOURCE_TENDENCY;
	}
	inline int getBUILDER_TASKING_BUILD_ROUTE_TENDENCY()
	{
		return m_iBUILDER_TASKING_BUILD_ROUTE_TENDENCY;
	}
	inline int getBUILDER_TASKING_DIRECTIVES_TO_EVALUATE()
	{
		return m_iBUILDER_TASKING_DIRECTIVES_TO_EVALUATE;
	}
	inline int getBUILDER_TASKING_BASELINE_BUILD_IMPROVEMENTS()
	{
		return m_iBUILDER_TASKING_BASELINE_BUILD_IMPROVEMENTS;
	}
	inline int getBUILDER_TASKING_BASELINE_BUILD_RESOURCE_IMPROVEMENTS()
	{
		return m_iBUILDER_TASKING_BASELINE_BUILD_RESOURCE_IMPROVEMENTS;
	}
	inline int getBUILDER_TASKING_BASELINE_BUILD_ROUTES()
	{
		return m_iBUILDER_TASKING_BASELINE_BUILD_ROUTES;
	}
	inline int getBUILDER_TASKING_BASELINE_REPAIR()
	{
		return m_iBUILDER_TASKING_BASELINE_REPAIR;
	}
	inline int getBUILDER_TASKING_BASELINE_SCRUB_FALLOUT()
	{
		return m_iBUILDER_TASKING_BASELINE_SCRUB_FALLOUT;
	}
	inline int getBUILDER_TASKING_BASELINE_ADDS_CULTURE()
	{
		return m_iBUILDER_TASKING_BASELINE_ADDS_CULTURE;
	}
	inline int getAI_MILITARY_THREAT_WEIGHT_MINOR()
	{
		return m_iAI_MILITARY_THREAT_WEIGHT_MINOR;
	}
	inline int getAI_MILITARY_THREAT_WEIGHT_MAJOR()
	{
		return m_iAI_MILITARY_THREAT_WEIGHT_MAJOR;
	}
	inline int getAI_MILITARY_THREAT_WEIGHT_SEVERE()
	{
		return m_iAI_MILITARY_THREAT_WEIGHT_SEVERE;
	}
	inline int getAI_MILITARY_THREAT_WEIGHT_CRITICAL()
	{
		return m_iAI_MILITARY_THREAT_WEIGHT_CRITICAL;
	}
	inline int getAI_STRATEGY_DEFEND_MY_LANDS_BASE_UNITS()
	{
		return m_iAI_STRATEGY_DEFEND_MY_LANDS_BASE_UNITS;
	}
	inline int getAI_STRATEGY_MILITARY_RESERVE_PERCENTAGE()
	{
		return m_iAI_STRATEGY_MILITARY_RESERVE_PERCENTAGE;
	}
	inline int getAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL()
	{
		return m_iAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL;
	}
	inline int getAI_MILITARY_BARBARIANS_FOR_MINOR_THREAT()
	{
		return m_iAI_MILITARY_BARBARIANS_FOR_MINOR_THREAT;
	}
	inline int getAI_MILITARY_IGNORE_BAD_ODDS()
	{
		return m_iAI_MILITARY_IGNORE_BAD_ODDS;
	}
	inline int getAI_MILITARY_RECAPTURING_OWN_CITY()
	{
		return m_iAI_MILITARY_RECAPTURING_OWN_CITY;
	}
	inline int getAI_MILITARY_CAPTURING_ORIGINAL_CAPITAL()
	{
		return m_iAI_MILITARY_CAPTURING_ORIGINAL_CAPITAL;
	}
	inline int getAI_CITY_SPECIALIZATION_EARLIEST_TURN()
	{
		return m_iAI_CITY_SPECIALIZATION_EARLIEST_TURN;
	}
	inline int getAI_CITY_SPECIALIZATION_REEVALUATION_INTERVAL()
	{
		return m_iAI_CITY_SPECIALIZATION_REEVALUATION_INTERVAL;
	}
	inline int getAI_CITY_SPECIALIZATION_GENERAL_ECONOMIC_WEIGHT()
	{
		return m_iAI_CITY_SPECIALIZATION_GENERAL_ECONOMIC_WEIGHT;
	}
	inline int getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_FLAVOR_EXPANSION()
	{
		return m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_FLAVOR_EXPANSION;
	}
	inline int getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_PERCENT_CONTINENT_UNOWNED()
	{
		return m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_PERCENT_CONTINENT_UNOWNED;
	}
	inline int getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_CITIES()
	{
		return m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_CITIES;
	}
	inline int getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_SETTLERS()
	{
		return m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_SETTLERS;
	}
	inline int getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_EARLY_EXPANSION()
	{
		return m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_EARLY_EXPANSION;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_OPERATIONAL_UNITS_REQUESTED()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_OPERATIONAL_UNITS_REQUESTED;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CIVS_AT_WAR_WITH()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CIVS_AT_WAR_WITH;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_WAR_MOBILIZATION()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_WAR_MOBILIZATION;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE_CRITICAL()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE_CRITICAL;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CAPITAL_THREAT()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CAPITAL_THREAT;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS_CRITICAL()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS_CRITICAL;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_WONDER()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_WONDER;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_SPACESHIP()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_SPACESHIP;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_OFFENSE()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_OFFENSE;
	}
	inline int getAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_PERSONALITY()
	{
		return m_iAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_PERSONALITY;
	}
	inline int getAI_CITY_SPECIALIZATION_GOLD_WEIGHT_FLAVOR_GOLD()
	{
		return m_iAI_CITY_SPECIALIZATION_GOLD_WEIGHT_FLAVOR_GOLD;
	}
	inline int getAI_CITY_SPECIALIZATION_GOLD_WEIGHT_LAND_DISPUTE()
	{
		return m_iAI_CITY_SPECIALIZATION_GOLD_WEIGHT_LAND_DISPUTE;
	}
	inline int getAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SCIENCE()
	{
		return m_iAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SCIENCE;
	}
	inline int getAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SPACESHIP()
	{
		return m_iAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SPACESHIP;
	}
	inline int getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_FIRST_RING()
	{
		return m_iAI_CITY_SPECIALIZATION_YIELD_WEIGHT_FIRST_RING;
	}
	inline int getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_SECOND_RING()
	{
		return m_iAI_CITY_SPECIALIZATION_YIELD_WEIGHT_SECOND_RING;
	}
	inline int getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_THIRD_RING()
	{
		return m_iAI_CITY_SPECIALIZATION_YIELD_WEIGHT_THIRD_RING;
	}
	inline int getAI_CITY_SPECIALIZATION_YIELD_NUM_TILES_CONSIDERED()
	{
		return m_iAI_CITY_SPECIALIZATION_YIELD_NUM_TILES_CONSIDERED;
	}
	inline int getAI_CITYSTRATEGY_SMALL_CITY_POP_THRESHOLD()
	{
		return m_iAI_CITYSTRATEGY_SMALL_CITY_POP_THRESHOLD;
	}
	inline int getAI_CITYSTRATEGY_MEDIUM_CITY_POP_THRESHOLD()
	{
		return m_iAI_CITYSTRATEGY_MEDIUM_CITY_POP_THRESHOLD;
	}
	inline int getAI_CITYSTRATEGY_LARGE_CITY_POP_THRESHOLD()
	{
		return m_iAI_CITYSTRATEGY_LARGE_CITY_POP_THRESHOLD;
	}
	inline int getAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_CITY_THRESHOLD()
	{
		return m_iAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_CITY_THRESHOLD;
	}
	inline int getAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_WORKER_MOD()
	{
		return m_iAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_WORKER_MOD;
	}
	inline int getAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_DESPERATE_TURN()
	{
		return m_iAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_DESPERATE_TURN;
	}
	inline int getAI_CITYSTRATEGY_WANT_TILE_IMPROVERS_MINIMUM_SIZE()
	{
		return m_iAI_CITYSTRATEGY_WANT_TILE_IMPROVERS_MINIMUM_SIZE;
	}
	inline float getAI_CITYSTRATEGY_YIELD_DEFICIENT_FOOD()
	{
		return m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_FOOD;
	}
	inline float getAI_CITYSTRATEGY_YIELD_DEFICIENT_PRODUCTION()
	{
		return m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_PRODUCTION;
	}
	inline float getAI_CITYSTRATEGY_YIELD_DEFICIENT_GOLD()
	{
		return m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_GOLD;
	}
	inline float getAI_CITYSTRATEGY_YIELD_DEFICIENT_SCIENCE()
	{
		return m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_SCIENCE;
	}
	inline int getAI_CITYSTRATEGY_OPERATION_UNIT_BASE_WEIGHT()
	{
		return m_iAI_CITYSTRATEGY_OPERATION_UNIT_BASE_WEIGHT;
	}
	inline int getAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER()
	{
		return m_iAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER;
	}
	inline int getAI_CITYSTRATEGY_ARMY_UNIT_BASE_WEIGHT()
	{
		return m_iAI_CITYSTRATEGY_ARMY_UNIT_BASE_WEIGHT;
	}
	inline int getAI_CITIZEN_VALUE_FOOD()
	{
		return m_iAI_CITIZEN_VALUE_FOOD;
	}
	inline int getAI_CITIZEN_VALUE_PRODUCTION()
	{
		return m_iAI_CITIZEN_VALUE_PRODUCTION;
	}
	inline int getAI_CITIZEN_VALUE_GOLD()
	{
		return m_iAI_CITIZEN_VALUE_GOLD;
	}
	inline int getAI_CITIZEN_VALUE_SCIENCE()
	{
		return m_iAI_CITIZEN_VALUE_SCIENCE;
	}
	inline int getAI_CITIZEN_VALUE_CULTURE()
	{
		return m_iAI_CITIZEN_VALUE_CULTURE;
	}
	inline int getAI_CITIZEN_VALUE_FAITH()
	{
		return m_iAI_CITIZEN_VALUE_FAITH;
	}
	inline int getAI_CITIZEN_FOOD_MOD_SIZE_CUTOFF()
	{
		return m_iAI_CITIZEN_FOOD_MOD_SIZE_CUTOFF;
	}
	inline int getAI_CITIZEN_FOOD_MOD_SIZE_EXPONENT()
	{
		return m_iAI_CITIZEN_FOOD_MOD_SIZE_EXPONENT;
	}
	inline int getAI_CITIZEN_MOD_FOOD_DEFICIT()
	{
		return m_iAI_CITIZEN_MOD_FOOD_DEFICIT;
	}
	inline int getAI_CITIZEN_MOD_PRODUCTION_DEFICIT()
	{
		return m_iAI_CITIZEN_MOD_PRODUCTION_DEFICIT;
	}
	inline int getAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE()
	{
		return m_iAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE;
	}
	inline int getAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE()
	{
		return m_iAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE;
	}
	inline int getAI_OPERATIONAL_PILLAGE_ENEMY_DEPLOY_RANGE()
	{
		return m_iAI_OPERATIONAL_PILLAGE_ENEMY_DEPLOY_RANGE;
	}
	inline int getAI_OPERATIONAL_NAVAL_BOMBARDMENT_DEPLOY_RANGE()
	{
		return m_iAI_OPERATIONAL_NAVAL_BOMBARDMENT_DEPLOY_RANGE;
	}
	inline int getAI_OPERATIONAL_PERCENT_DANGER_FOR_FORWARD_MUSTER()
	{
		return m_iAI_OPERATIONAL_PERCENT_DANGER_FOR_FORWARD_MUSTER;
	}
	inline int getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION()
	{
		return m_iAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION;
	}
	inline int getAI_TACTICAL_MAP_UNIT_STRENGTH_MULTIPLIER()
	{
		return m_iAI_TACTICAL_MAP_UNIT_STRENGTH_MULTIPLIER;
	}
	inline int getAI_TACTICAL_MAP_DOMINANCE_PERCENTAGE()
	{
		return m_iAI_TACTICAL_MAP_DOMINANCE_PERCENTAGE;
	}
	inline int getAI_TACTICAL_MAP_TEMP_ZONE_RADIUS()
	{
		return m_iAI_TACTICAL_MAP_TEMP_ZONE_RADIUS;
	}
	inline int getAI_TACTICAL_MAP_TEMP_ZONE_TURNS()
	{
		return m_iAI_TACTICAL_MAP_TEMP_ZONE_TURNS;
	}
	inline int getAI_TACTICAL_MAP_BOMBARDMENT_ZONE_TURNS()
	{
		return m_iAI_TACTICAL_MAP_BOMBARDMENT_ZONE_TURNS;
	}
	inline int getAI_TACTICAL_RECRUIT_RANGE()
	{
		return m_iAI_TACTICAL_RECRUIT_RANGE;
	}
	inline int getAI_TACTICAL_REPOSITION_RANGE()
	{
		return m_iAI_TACTICAL_REPOSITION_RANGE;
	}
	inline int getAI_TACTICAL_OVERKILL_PERCENT()
	{
		return m_iAI_TACTICAL_OVERKILL_PERCENT;
	}
	inline int getAI_TACTICAL_BARBARIAN_RELEASE_VARIATION()
	{
		return m_iAI_TACTICAL_BARBARIAN_RELEASE_VARIATION;
	}
	inline int getAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS()
	{
		return m_iAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_CAPTURE_CITY()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_CAPTURE_CITY;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_DAMAGE_CITY()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_DAMAGE_CITY;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_HIGH_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_HIGH_UNIT;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_MEDIUM_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_MEDIUM_UNIT;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_LOW_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_LOW_UNIT;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_TO_SAFETY()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_TO_SAFETY;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_HIGH_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_HIGH_UNIT;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_MEDIUM_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_MEDIUM_UNIT;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_LOW_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_LOW_UNIT;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_DESPERATE_ATTACK()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESPERATE_ATTACK;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_CITADEL()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_CITADEL;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_NEXT_TURN()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_NEXT_TURN;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_BLOCKADE_RESOURCE()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_BLOCKADE_RESOURCE;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_CIVILIAN_ATTACK()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_CIVILIAN_ATTACK;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_PASSIVE_MOVE()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_PASSIVE_MOVE;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_AGGRESSIVE_MOVE()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_AGGRESSIVE_MOVE;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_CAMP_DEFENSE()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_CAMP_DEFENSE;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_ESCORT_CIVILIAN()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_ESCORT_CIVILIAN;
	}
	inline int getAI_TACTICAL_BARBARIAN_PRIORITY_PLUNDER_TRADE_UNIT()
	{
		return m_iAI_TACTICAL_BARBARIAN_PRIORITY_PLUNDER_TRADE_UNIT;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_RANDOMNESS()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_RANDOMNESS;
	}
	inline int getAI_HOMELAND_MAX_UPGRADE_MOVE_TURNS()
	{
		return m_iAI_HOMELAND_MAX_UPGRADE_MOVE_TURNS;
	}
	inline int getAI_HOMELAND_MAX_DEFENSIVE_MOVE_TURNS()
	{
		return m_iAI_HOMELAND_MAX_DEFENSIVE_MOVE_TURNS;
	}
	inline int getAI_HOMELAND_ESTIMATE_TURNS_DISTANCE()
	{
		return m_iAI_HOMELAND_ESTIMATE_TURNS_DISTANCE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_SETTLE()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_SETTLE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_HEAL()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_HEAL;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_TO_SAFETY()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_TO_SAFETY;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_WORKER()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_WORKER;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_WORKER_SEA()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_WORKER_SEA;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_EXPLORE()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_EXPLORE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_EXPLORE_SEA()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_EXPLORE_SEA;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_SENTRY()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_SENTRY;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_MOBILE_RESERVE()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_MOBILE_RESERVE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_GARRISON()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_GARRISON;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_PATROL()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_PATROL;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_UPGRADE()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_UPGRADE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_ANCIENT_RUINS()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_ANCIENT_RUINS;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_GARRISON_CITY_STATE()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_GARRISON_CITY_STATE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_WRITER()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_WRITER;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_ARTIST()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_ARTIST;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_MUSICIAN()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_MUSICIAN;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_SCIENTIST_FREE_TECH()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_SCIENTIST_FREE_TECH;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_ENGINEER_HURRY()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_ENGINEER_HURRY;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_GENERAL_GARRISON()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_GENERAL_GARRISON;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_ADMIRAL_GARRISON()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_ADMIRAL_GARRISON;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_PROPHET_RELIGION()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_PROPHET_RELIGION;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_MISSIONARY()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_MISSIONARY;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_INQUISITOR()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_INQUISITOR;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_SPACESHIP_PART()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_SPACESHIP_PART;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_ADD_SPACESHIP_PART()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_ADD_SPACESHIP_PART;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_AIRCRAFT_TO_THE_FRONT()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_AIRCRAFT_TO_THE_FRONT;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_TREASURE()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_TREASURE;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_TRADE_UNIT()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_TRADE_UNIT;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_ARCHAEOLOGIST()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_ARCHAEOLOGIST;
	}
	inline int getAI_HOMELAND_MOVE_PRIORITY_AIRLIFT()
	{
		return m_iAI_HOMELAND_MOVE_PRIORITY_AIRLIFT;
	}
	inline int getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT()
	{
		return m_iAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT;
	}
	inline int getAI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT()
	{
		return m_iAI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT;
	}
	inline int getAI_DIPLO_LAND_DISPUTE_WEIGHT_WEAK()
	{
		return m_iAI_DIPLO_LAND_DISPUTE_WEIGHT_WEAK;
	}
	inline int getAI_DIPLO_LAND_DISPUTE_WEIGHT_STRONG()
	{
		return m_iAI_DIPLO_LAND_DISPUTE_WEIGHT_STRONG;
	}
	inline int getAI_DIPLO_LAND_DISPUTE_WEIGHT_FIERCE()
	{
		return m_iAI_DIPLO_LAND_DISPUTE_WEIGHT_FIERCE;
	}
	inline int getMINOR_BULLY_GOLD()
	{
		return m_iMINOR_BULLY_GOLD;
	}
#if defined(FULL_YIELD_FROM_KILLS)
	inline int getPOLICY_YIELD_CAP()
	{
		return m_iPOLICY_YIELD_CAP;
	}
	inline int getTRAIT_YIELD_CAP()
	{
		return m_iTRAIT_YIELD_CAP;
	}
	inline int getBELIEF_YIELD_CAP()
	{
		return m_iBELIEF_YIELD_CAP;
	}
	inline int getPROMOTION_YIELD_CAP()
	{
		return m_iPROMOTION_YIELD_CAP;
	}
	inline int getUNIT_YIELD_CAP()
	{
		return m_iUNIT_YIELD_CAP;
	}
#endif
#if defined(MISC_CHANGES) // New Global Values
	inline int getBULLY_GOLD_GROWTH_FACTOR()
	{
		return m_iBULLY_GOLD_GROWTH_FACTOR;
	}
	inline int getINTERNAL_TRADE_FOOD_BASE_TIMES100()
	{
		return m_iINTERNAL_TRADE_FOOD_BASE_TIMES100;
	}
	inline int getINTERNAL_TRADE_PRODUCTION_BASE_TIMES100()
	{
		return m_iINTERNAL_TRADE_PRODUCTION_BASE_TIMES100;
	}
	inline int getFAITH_PURCHASE_VISIBLE_DIVISOR()
	{
		return m_iFAITH_PURCHASE_VISIBLE_DIVISOR;
	}
#endif
#if defined(LEKMOD_v34)
	inline int getAI_CITIZEN_VALUE_GOLDEN_AGE_POINTS()
	{
		return m_iAI_CITIZEN_VALUE_GOLDEN_AGE_POINTS;
	}
#endif
	inline int getMINOR_FRIENDSHIP_RATE_MOD_MAXIMUM()
	{
		return m_iMINOR_FRIENDSHIP_RATE_MOD_MAXIMUM;
	}
	inline int getMINOR_FRIENDSHIP_RATE_MOD_SHARED_RELIGION()
	{
		return m_iMINOR_FRIENDSHIP_RATE_MOD_SHARED_RELIGION;
	}
	inline int getMINOR_FRIENDSHIP_DROP_PER_TURN()
	{
		return m_iMINOR_FRIENDSHIP_DROP_PER_TURN;
	}
	inline int getMINOR_FRIENDSHIP_DROP_PER_TURN_HOSTILE()
	{
		return m_iMINOR_FRIENDSHIP_DROP_PER_TURN_HOSTILE;
	}
	inline int getMINOR_FRIENDSHIP_DROP_PER_TURN_AGGRESSOR()
	{
		return m_iMINOR_FRIENDSHIP_DROP_PER_TURN_AGGRESSOR;
	}
	inline int getMINOR_FRIENDSHIP_DROP_DISHONOR_PLEDGE_TO_PROTECT()
	{
		return m_iMINOR_FRIENDSHIP_DROP_DISHONOR_PLEDGE_TO_PROTECT;
	}
	inline int getMINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS()
	{
		return m_iMINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS;
	}
	inline int getMINOR_FRIENDSHIP_DROP_BULLY_GOLD_FAILURE()
	{
		return m_iMINOR_FRIENDSHIP_DROP_BULLY_GOLD_FAILURE;
	}
	inline int getMINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS()
	{
		return m_iMINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS;
	}
	inline int getMINOR_FRIENDSHIP_DROP_BULLY_WORKER_FAILURE()
	{
		return m_iMINOR_FRIENDSHIP_DROP_BULLY_WORKER_FAILURE;
	}
	inline int getMINOR_FRIENDSHIP_NEGATIVE_INCREASE_PER_TURN()
	{
		return m_iMINOR_FRIENDSHIP_NEGATIVE_INCREASE_PER_TURN;
	}
	inline int getMINOR_FRIENDSHIP_AT_WAR()
	{
		return m_iMINOR_FRIENDSHIP_AT_WAR;
	}
	inline int getMINOR_CIV_AGGRESSOR_THRESHOLD()
	{
		return m_iMINOR_CIV_AGGRESSOR_THRESHOLD;
	}
	inline int getMINOR_CIV_WARMONGER_THRESHOLD()
	{
		return m_iMINOR_CIV_WARMONGER_THRESHOLD;
	}
	inline int getPERMANENT_WAR_AGGRESSOR_CHANCE()
	{
		return m_iPERMANENT_WAR_AGGRESSOR_CHANCE;
	}
	inline int getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_DISTANT()
	{
		return m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_DISTANT;
	}
	inline int getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_FAR()
	{
		return m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_FAR;
	}
	inline int getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_CLOSE()
	{
		return m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_CLOSE;
	}
	inline int getPERMANENT_WAR_OTHER_WARMONGER_CHANCE_NEIGHBORS()
	{
		return m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_NEIGHBORS;
	}
	inline int getPERMANENT_WAR_OTHER_CHANCE_DISTANT()
	{
		return m_iPERMANENT_WAR_OTHER_CHANCE_DISTANT;
	}
	inline int getPERMANENT_WAR_OTHER_CHANCE_FAR()
	{
		return m_iPERMANENT_WAR_OTHER_CHANCE_FAR;
	}
	inline int getPERMANENT_WAR_OTHER_CHANCE_CLOSE()
	{
		return m_iPERMANENT_WAR_OTHER_CHANCE_CLOSE;
	}
	inline int getPERMANENT_WAR_OTHER_CHANCE_NEIGHBORS()
	{
		return m_iPERMANENT_WAR_OTHER_CHANCE_NEIGHBORS;
	}
	inline int getPERMANENT_WAR_OTHER_AT_WAR()
	{
		return m_iPERMANENT_WAR_OTHER_AT_WAR;
	}
	inline int getFRIENDSHIP_NEUTRAL_ON_DEATH()
	{
		return m_iFRIENDSHIP_NEUTRAL_ON_DEATH;
	}
	inline int getFRIENDSHIP_THRESHOLD_NEUTRAL()
	{
		return m_iFRIENDSHIP_THRESHOLD_NEUTRAL;
	}
	inline int getFRIENDSHIP_FRIENDS_ON_DEATH()
	{
		return m_iFRIENDSHIP_FRIENDS_ON_DEATH;
	}
	inline int getFRIENDSHIP_THRESHOLD_FRIENDS()
	{
		return m_iFRIENDSHIP_THRESHOLD_FRIENDS;
	}
	inline int getFRIENDSHIP_ALLIES_ON_DEATH()
	{
		return m_iFRIENDSHIP_ALLIES_ON_DEATH;
	}
	inline int getFRIENDSHIP_THRESHOLD_ALLIES()
	{
		return m_iFRIENDSHIP_THRESHOLD_ALLIES;
	}
	inline int getFRIENDSHIP_THRESHOLD_MAX()
	{
		return m_iFRIENDSHIP_THRESHOLD_MAX;
	}
	inline int getFRIENDSHIP_THRESHOLD_CAN_BULLY()
	{
		return m_iFRIENDSHIP_THRESHOLD_CAN_BULLY;
	}
	inline int getFRIENDSHIP_THRESHOLD_CAN_PLEDGE_TO_PROTECT()
	{
		return m_iFRIENDSHIP_THRESHOLD_CAN_PLEDGE_TO_PROTECT;
	}
	inline int getMINOR_FRIENDSHIP_CLOSE_AMOUNT()
	{
		return m_iMINOR_FRIENDSHIP_CLOSE_AMOUNT;
	}
	inline int getMINOR_CIV_SCIENCE_BONUS_MULTIPLIER()
	{
		return m_iMINOR_CIV_SCIENCE_BONUS_MULTIPLIER;
	}
	inline int getFRIENDS_CULTURE_BONUS_AMOUNT_ANCIENT()
	{
		return m_iFRIENDS_CULTURE_BONUS_AMOUNT_ANCIENT;
	}
	inline int getFRIENDS_CULTURE_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iFRIENDS_CULTURE_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getFRIENDS_CULTURE_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iFRIENDS_CULTURE_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getALLIES_CULTURE_BONUS_AMOUNT_ANCIENT()
	{
		return m_iALLIES_CULTURE_BONUS_AMOUNT_ANCIENT;
	}
	inline int getALLIES_CULTURE_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iALLIES_CULTURE_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getALLIES_CULTURE_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iALLIES_CULTURE_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE()
	{
		return m_iFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE;
	}
	inline int getFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_POST_RENAISSANCE()
	{
		return m_iFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_POST_RENAISSANCE;
	}
	inline int getFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE()
	{
		return m_iFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE;
	}
	inline int getFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_POST_RENAISSANCE()
	{
		return m_iFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_POST_RENAISSANCE;
	}
	inline int getALLIES_CAPITAL_FOOD_BONUS_AMOUNT()
	{
		return m_iALLIES_CAPITAL_FOOD_BONUS_AMOUNT;
	}
	inline int getALLIES_OTHER_CITIES_FOOD_BONUS_AMOUNT()
	{
		return m_iALLIES_OTHER_CITIES_FOOD_BONUS_AMOUNT;
	}
	inline int getFRIENDS_BASE_TURNS_UNIT_SPAWN()
	{
		return m_iFRIENDS_BASE_TURNS_UNIT_SPAWN;
	}
	inline int getFRIENDS_RAND_TURNS_UNIT_SPAWN()
	{
		return m_iFRIENDS_RAND_TURNS_UNIT_SPAWN;
	}
	inline int getALLIES_EXTRA_TURNS_UNIT_SPAWN()
	{
		return m_iALLIES_EXTRA_TURNS_UNIT_SPAWN;
	}
	inline int getUNIT_SPAWN_BIAS_MULTIPLIER()
	{
		return m_iUNIT_SPAWN_BIAS_MULTIPLIER;
	}
	inline int getUNIT_SPAWN_NUM_CHOICES()
	{
		return m_iUNIT_SPAWN_NUM_CHOICES;
	}
	inline int getFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT()
	{
		return m_iFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT;
	}
	inline int getFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT()
	{
		return m_iALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT;
	}
	inline int getALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT()
	{
		return m_iFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT;
	}
	inline int getFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT()
	{
		return m_iALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT;
	}
	inline int getALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE()
	{
		return m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE;
	}
	inline int getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL()
	{
		return m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL;
	}
	inline int getFRIENDS_FAITH_FLAT_BONUS_AMOUNT_ANCIENT()
	{
		return m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_ANCIENT;
	}
	inline int getALLIES_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL()
	{
		return m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	}
	inline int getALLIES_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE()
	{
		return m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE;
	}
	inline int getALLIES_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL()
	{
		return m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL;
	}
	inline int getALLIES_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL()
	{
		return m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL;
	}
	inline int getALLIES_FAITH_FLAT_BONUS_AMOUNT_ANCIENT()
	{
		return m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_ANCIENT;
	}
	inline int getMINOR_TURNS_GREAT_PEOPLE_SPAWN_BASE()
	{
		return m_iMINOR_TURNS_GREAT_PEOPLE_SPAWN_BASE;
	}
	inline int getMINOR_TURNS_GREAT_PEOPLE_SPAWN_RAND()
	{
		return m_iMINOR_TURNS_GREAT_PEOPLE_SPAWN_RAND;
	}
	inline int getMINOR_TURNS_GREAT_PEOPLE_SPAWN_BIAS_MULTIPLY()
	{
		return m_iMINOR_TURNS_GREAT_PEOPLE_SPAWN_BIAS_MULTIPLY;
	}
	inline int getMINOR_ADDITIONAL_ALLIES_GP_CHANGE()
	{
		return m_iMINOR_ADDITIONAL_ALLIES_GP_CHANGE;
	}
	inline int getMAX_MINOR_ADDITIONAL_ALLIES_GP_CHANGE()
	{
		return m_iMAX_MINOR_ADDITIONAL_ALLIES_GP_CHANGE;
	}
	inline int getMAX_DISTANCE_MINORS_BARB_QUEST()
	{
		return m_iMAX_DISTANCE_MINORS_BARB_QUEST;
	}
	inline int getTXT_KEY_MINOR_GIFT_UNITS_REMINDER()
	{
		return m_iTXT_KEY_MINOR_GIFT_UNITS_REMINDER;
	}
	inline int getWAR_QUEST_COMPLETE_FRIENDSHIP()
	{
		return m_iWAR_QUEST_COMPLETE_FRIENDSHIP;
	}
	inline int getWAR_QUEST_UNITS_TO_KILL_DIVISOR()
	{
		return m_iWAR_QUEST_UNITS_TO_KILL_DIVISOR;
	}
	inline int getWAR_QUEST_MIN_UNITS_TO_KILL()
	{
		return m_iWAR_QUEST_MIN_UNITS_TO_KILL;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_ROUTE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_ROUTE;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_KILL_CAMP()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_KILL_CAMP;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_CONNECT_RESOURCE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_CONNECT_RESOURCE;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_CONSTRUCT_WONDER()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_CONSTRUCT_WONDER;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_GREAT_PERSON()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_GREAT_PERSON;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_KILL_CITY_STATE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_KILL_CITY_STATE;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_FIND_PLAYER()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_FIND_PLAYER;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_FIND_NATURAL_WONDER()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_FIND_NATURAL_WONDER;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_GIVE_GOLD()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_GIVE_GOLD;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_PLEDGE_TO_PROTECT()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_PLEDGE_TO_PROTECT;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_CONTEST_CULTURE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_CONTEST_CULTURE;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_CONTEST_FAITH()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_CONTEST_FAITH;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_CONTEST_TECHS()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_CONTEST_TECHS;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_INVEST()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_INVEST;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_BULLY_CITY_STATE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_BULLY_CITY_STATE;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_DENOUNCE_MAJOR()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_DENOUNCE_MAJOR;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_SPREAD_RELIGION()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_SPREAD_RELIGION;
	}
	inline int getMINOR_QUEST_FRIENDSHIP_TRADE_ROUTE()
	{
		return m_iMINOR_QUEST_FRIENDSHIP_TRADE_ROUTE;
	}
	inline int getMINOR_QUEST_STANDARD_CONTEST_LENGTH()
	{
		return m_iMINOR_QUEST_STANDARD_CONTEST_LENGTH;
	}
	inline int getMINOR_CIV_GOLD_GIFT_GAME_MULTIPLIER()
	{
		return m_iMINOR_CIV_GOLD_GIFT_GAME_MULTIPLIER;
	}
	inline int getMINOR_CIV_GOLD_GIFT_GAME_DIVISOR()
	{
		return m_iMINOR_CIV_GOLD_GIFT_GAME_DIVISOR;
	}
	inline int getMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR()
	{
		return m_iMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR;
	}
	inline int getMINOR_CIV_GOLD_GIFT_MINIMUM_FRIENDSHIP_REWARD()
	{
		return m_iMINOR_CIV_GOLD_GIFT_MINIMUM_FRIENDSHIP_REWARD;
	}
	inline int getMINOR_CIV_BEST_RELATIONS_HAPPINESS_BONUS()
	{
		return m_iMINOR_CIV_BEST_RELATIONS_HAPPINESS_BONUS;
	}
	inline int getMINOR_CIV_RESOURCE_SEARCH_RADIUS()
	{
		return m_iMINOR_CIV_RESOURCE_SEARCH_RADIUS;
	}
	inline int getFRIENDSHIP_PER_UNIT_INTRUDING()
	{
		return m_iFRIENDSHIP_PER_UNIT_INTRUDING;
	}
	inline int getFRIENDSHIP_PER_BARB_KILLED()
	{
		return m_iFRIENDSHIP_PER_BARB_KILLED;
	}
	inline int getFRIENDSHIP_PER_UNIT_GIFTED()
	{
		return m_iFRIENDSHIP_PER_UNIT_GIFTED;
	}
	inline int getMAX_INFLUENCE_FROM_MINOR_GIFTS()
	{
		return m_iMAX_INFLUENCE_FROM_MINOR_GIFTS;
	}
	inline int getMINOR_LIBERATION_FRIENDSHIP()
	{
		return m_iMINOR_LIBERATION_FRIENDSHIP;
	}
	inline int getRETURN_CIVILIAN_FRIENDSHIP()
	{
		return m_iRETURN_CIVILIAN_FRIENDSHIP;
	}
	inline int getMINOR_CIV_MAX_GLOBAL_QUESTS_FOR_PLAYER()
	{
		return m_iMINOR_CIV_MAX_GLOBAL_QUESTS_FOR_PLAYER;
	}
	inline int getMINOR_CIV_MAX_PERSONAL_QUESTS_FOR_PLAYER()
	{
		return m_iMINOR_CIV_MAX_PERSONAL_QUESTS_FOR_PLAYER;
	}
	inline int getMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN()
	{
		return m_iMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN;
	}
	inline int getMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN_RAND()
	{
		return m_iMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN_RAND;
	}
	inline int getMINOR_CIV_GLOBAL_QUEST_MIN_TURNS_BETWEEN()
	{
		return m_iMINOR_CIV_GLOBAL_QUEST_MIN_TURNS_BETWEEN;
	}
	inline int getMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN()
	{
		return m_iMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN;
	}
	inline int getMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER()
	{
		return m_iMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER;
	}
	inline int getMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN()
	{
		return m_iMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN;
	}
	inline int getMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN_RAND()
	{
		return m_iMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN_RAND;
	}
	inline int getMINOR_CIV_PERSONAL_QUEST_MIN_TURNS_BETWEEN()
	{
		return m_iMINOR_CIV_PERSONAL_QUEST_MIN_TURNS_BETWEEN;
	}
	inline int getMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN()
	{
		return m_iMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN;
	}
	inline int getMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER()
	{
		return m_iMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER;
	}
	inline int getMINOR_CIV_QUEST_KILL_CAMP_RANGE()
	{
		return m_iMINOR_CIV_QUEST_KILL_CAMP_RANGE;
	}
	inline int getMINOR_CIV_QUEST_WONDER_COMPLETION_THRESHOLD()
	{
		return m_iMINOR_CIV_QUEST_WONDER_COMPLETION_THRESHOLD;
	}
	inline int getMINOR_CIV_THREATENED_BY_BARBS_NOTIF_THRESHOLD()
	{
		return m_iMINOR_CIV_THREATENED_BY_BARBS_NOTIF_THRESHOLD;
	}
	inline int getMINOR_CIV_MERCANTILE_RESOURCES_QUANTITY()
	{
		return m_iMINOR_CIV_MERCANTILE_RESOURCES_QUANTITY;
	}
	inline int getMINOR_CIV_MERCANTILE_RESOURCES_KEEP_ON_CAPTURE_DISABLED()
	{
		return m_iMINOR_CIV_MERCANTILE_RESOURCES_KEEP_ON_CAPTURE_DISABLED;
	}
	inline int getQUEST_DISABLED_ROUTE()
	{
		return m_iQUEST_DISABLED_ROUTE;
	}
	inline int getQUEST_DISABLED_KILL_CAMP()
	{
		return m_iQUEST_DISABLED_KILL_CAMP;
	}
	inline int getQUEST_DISABLED_CONNECT_RESOURCE()
	{
		return m_iQUEST_DISABLED_CONNECT_RESOURCE;
	}
	inline int getQUEST_DISABLED_CONSTRUCT_WONDER()
	{
		return m_iQUEST_DISABLED_CONSTRUCT_WONDER;
	}
	inline int getQUEST_DISABLED_GREAT_PERSON()
	{
		return m_iQUEST_DISABLED_GREAT_PERSON;
	}
	inline int getQUEST_DISABLED_KILL_CITY_STATE()
	{
		return m_iQUEST_DISABLED_KILL_CITY_STATE;
	}
	inline int getQUEST_DISABLED_FIND_PLAYER()
	{
		return m_iQUEST_DISABLED_FIND_PLAYER;
	}
	inline int getQUEST_DISABLED_NATURAL_WONDER()
	{
		return m_iQUEST_DISABLED_NATURAL_WONDER;
	}
	inline int getQUEST_DISABLED_GIVE_GOLD()
	{
		return m_iQUEST_DISABLED_GIVE_GOLD;
	}
	inline int getQUEST_DISABLED_PLEDGE_TO_PROTECT()
	{
		return m_iQUEST_DISABLED_PLEDGE_TO_PROTECT;
	}
	inline int getQUEST_DISABLED_CONTEST_CULTURE()
	{
		return m_iQUEST_DISABLED_CONTEST_CULTURE;
	}
	inline int getQUEST_DISABLED_CONTEST_FAITH()
	{
		return m_iQUEST_DISABLED_CONTEST_FAITH;
	}
	inline int getQUEST_DISABLED_CONTEST_TECHS()
	{
		return m_iQUEST_DISABLED_CONTEST_TECHS;
	}
	inline int getQUEST_DISABLED_INVEST()
	{
		return m_iQUEST_DISABLED_INVEST;
	}
	inline int getQUEST_DISABLED_BULLY_CITY_STATE()
	{
		return m_iQUEST_DISABLED_BULLY_CITY_STATE;
	}
	inline int getQUEST_DISABLED_DENOUNCE_MAJOR()
	{
		return m_iQUEST_DISABLED_DENOUNCE_MAJOR;
	}
	inline int getQUEST_DISABLED_SPREAD_RELIGION()
	{
		return m_iQUEST_DISABLED_SPREAD_RELIGION;
	}
	inline int getQUEST_DISABLED_TRADE_ROUTE()
	{
		return m_iQUEST_DISABLED_TRADE_ROUTE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_DEFAULT()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_DEFAULT;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_ROUTE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_ROUTE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_ROUTE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_ROUTE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_ROUTE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_ROUTE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_ROUTE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_ROUTE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CAMP()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CAMP;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_CONNECT_RESOURCE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_CONNECT_RESOURCE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_CONNECT_RESOURCE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_CONNECT_RESOURCE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_CONSTRUCT_WONDER()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_CONSTRUCT_WONDER;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_GREAT_PERSON()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_GREAT_PERSON;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CITY_STATE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CITY_STATE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_KILL_CITY_STATE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_KILL_CITY_STATE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_NEUTRAL_KILL_CITY_STATE()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_NEUTRAL_KILL_CITY_STATE;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_FIND_PLAYER()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_FIND_PLAYER;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_FIND_PLAYER()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_FIND_PLAYER;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_FIND_NATURAL_WONDER()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_FIND_NATURAL_WONDER;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_FIND_NATURAL_WONDER()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_FIND_NATURAL_WONDER;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_GIVE_GOLD()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_GIVE_GOLD;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_GIVE_GOLD()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_GIVE_GOLD;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_GIVE_GOLD()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_GIVE_GOLD;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_PLEDGE_TO_PROTECT()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_PLEDGE_TO_PROTECT;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_PLEDGE_TO_PROTECT()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_PLEDGE_TO_PROTECT;
	}
	inline int getMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_PLEDGE_TO_PROTECT()
	{
		return m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_PLEDGE_TO_PROTECT;
	}
	inline int getRELIGION_BELIEF_SCORE_CITY_MULTIPLIER()
	{
		return m_iRELIGION_BELIEF_SCORE_CITY_MULTIPLIER;
	}
	inline int getRELIGION_BELIEF_SCORE_WORKED_PLOT_MULTIPLIER()
	{
		return m_iRELIGION_BELIEF_SCORE_WORKED_PLOT_MULTIPLIER;
	}
	inline int getRELIGION_BELIEF_SCORE_OWNED_PLOT_MULTIPLIER()
	{
		return m_iRELIGION_BELIEF_SCORE_OWNED_PLOT_MULTIPLIER;
	}
	inline int getRELIGION_BELIEF_SCORE_UNOWNED_PLOT_MULTIPLIER()
	{
		return m_iRELIGION_BELIEF_SCORE_UNOWNED_PLOT_MULTIPLIER;
	}
	inline int getRELIGION_MISSIONARY_RANGE_IN_TURNS()
	{
		return m_iRELIGION_MISSIONARY_RANGE_IN_TURNS;
	}
	inline int getRELIGION_MAX_MISSIONARIES()
	{
		return m_iRELIGION_MAX_MISSIONARIES;
	}
	inline float getNORMAL_ANNEX()
	{
		return m_fNORMAL_ANNEX;
	}
	inline float getAGGRESIVE_ANNEX()
	{
		return m_fAGGRESSIVE_ANNEX;
	}
	inline int getMC_GIFT_WEIGHT_THRESHOLD()
	{
		return m_iMC_GIFT_WEIGHT_THRESHOLD;
	}
	inline int getMC_ALWAYS_GIFT_DIPLO_THRESHOLD()
	{
		return m_iMC_ALWAYS_GIFT_DIPLO_THRESHOLD;
	}
	inline int getMC_SOMETIMES_GIFT_RAND_MULTIPLIER()
	{
		return m_iMC_SOMETIMES_GIFT_RAND_MULTIPLIER;
	}
	inline int getMC_SMALL_GIFT_WEIGHT_PASS_OTHER_PLAYER()
	{
		return m_iMC_SMALL_GIFT_WEIGHT_PASS_OTHER_PLAYER;
	}
	inline int getMC_GIFT_WEIGHT_PASS_OTHER_PLAYER()
	{
		return m_iMC_GIFT_WEIGHT_PASS_OTHER_PLAYER;
	}
	inline int getMC_GIFT_WEIGHT_MARITIME_GROWTH()
	{
		return m_iMC_GIFT_WEIGHT_MARITIME_GROWTH;
	}
	inline int getMC_GIFT_WEIGHT_DIPLO_VICTORY()
	{
		return m_iMC_GIFT_WEIGHT_DIPLO_VICTORY;
	}
	inline int getMC_GIFT_WEIGHT_CULTURE_VICTORY()
	{
		return m_iMC_GIFT_WEIGHT_CULTURE_VICTORY;
	}
	inline int getMC_GIFT_WEIGHT_CONQUEST_VICTORY()
	{
		return m_iMC_GIFT_WEIGHT_CONQUEST_VICTORY;
	}
	inline int getMC_GIFT_WEIGHT_MILITARISTIC()
	{
		return m_iMC_GIFT_WEIGHT_MILITARISTIC;
	}
	inline int getMC_GIFT_WEIGHT_RESOURCE_WE_NEED()
	{
		return m_iMC_GIFT_WEIGHT_RESOURCE_WE_NEED;
	}
	inline int getMC_GIFT_WEIGHT_NEUTRAL_VICTORY_ROLL()
	{
		return m_iMC_GIFT_WEIGHT_NEUTRAL_VICTORY_ROLL;
	}
	inline int getMC_GIFT_WEIGHT_PROTECTIVE()
	{
		return m_iMC_GIFT_WEIGHT_PROTECTIVE;
	}
	inline int getMC_GIFT_WEIGHT_HOSTILE()
	{
		return m_iMC_GIFT_WEIGHT_HOSTILE;
	}
	inline int getMC_GIFT_WEIGHT_NEIGHBORS()
	{
		return m_iMC_GIFT_WEIGHT_NEIGHBORS;
	}
	inline int getMC_GIFT_WEIGHT_CLOSE()
	{
		return m_iMC_GIFT_WEIGHT_CLOSE;
	}
	inline int getMC_GIFT_WEIGHT_FAR()
	{
		return m_iMC_GIFT_WEIGHT_FAR;
	}
	inline int getMC_GIFT_WEIGHT_ALMOST_NOT_ALLIES()
	{
		return m_iMC_GIFT_WEIGHT_ALMOST_NOT_ALLIES;
	}
	inline int getMC_GIFT_WEIGHT_ALMOST_NOT_FRIENDS()
	{
		return m_iMC_GIFT_WEIGHT_ALMOST_NOT_FRIENDS;
	}
	inline int getOPINION_WEIGHT_LAND_FIERCE()
	{
		return m_iOPINION_WEIGHT_LAND_FIERCE;
	}
	inline int getOPINION_WEIGHT_LAND_STRONG()
	{
		return m_iOPINION_WEIGHT_LAND_STRONG;
	}
	inline int getOPINION_WEIGHT_LAND_WEAK()
	{
		return m_iOPINION_WEIGHT_LAND_WEAK;
	}
	inline int getOPINION_WEIGHT_LAND_NONE()
	{
		return m_iOPINION_WEIGHT_LAND_NONE;
	}
	inline int getOPINION_WEIGHT_VICTORY_FIERCE()
	{
		return m_iOPINION_WEIGHT_VICTORY_FIERCE;
	}
	inline int getOPINION_WEIGHT_VICTORY_STRONG()
	{
		return m_iOPINION_WEIGHT_VICTORY_STRONG;
	}
	inline int getOPINION_WEIGHT_VICTORY_WEAK()
	{
		return m_iOPINION_WEIGHT_VICTORY_WEAK;
	}
	inline int getOPINION_WEIGHT_VICTORY_NONE()
	{
		return m_iOPINION_WEIGHT_VICTORY_NONE;
	}
	inline int getOPINION_WEIGHT_WONDER_FIERCE()
	{
		return m_iOPINION_WEIGHT_WONDER_FIERCE;
	}
	inline int getOPINION_WEIGHT_WONDER_STRONG()
	{
		return m_iOPINION_WEIGHT_WONDER_STRONG;
	}
	inline int getOPINION_WEIGHT_WONDER_WEAK()
	{
		return m_iOPINION_WEIGHT_WONDER_WEAK;
	}
	inline int getOPINION_WEIGHT_WONDER_NONE()
	{
		return m_iOPINION_WEIGHT_WONDER_NONE;
	}
	inline int getOPINION_WEIGHT_MINOR_CIV_FIERCE()
	{
		return m_iOPINION_WEIGHT_MINOR_CIV_FIERCE;
	}
	inline int getOPINION_WEIGHT_MINOR_CIV_STRONG()
	{
		return m_iOPINION_WEIGHT_MINOR_CIV_STRONG;
	}
	inline int getOPINION_WEIGHT_MINOR_CIV_WEAK()
	{
		return m_iOPINION_WEIGHT_MINOR_CIV_WEAK;
	}
	inline int getOPINION_WEIGHT_MINOR_CIV_NONE()
	{
		return m_iOPINION_WEIGHT_MINOR_CIV_NONE;
	}
	inline int getOPINION_WEIGHT_WARMONGER_CRITICAL()
	{
		return m_iOPINION_WEIGHT_WARMONGER_CRITICAL;
	}
	inline int getOPINION_WEIGHT_WARMONGER_SEVERE()
	{
		return m_iOPINION_WEIGHT_WARMONGER_SEVERE;
	}
	inline int getOPINION_WEIGHT_WARMONGER_MAJOR()
	{
		return m_iOPINION_WEIGHT_WARMONGER_MAJOR;
	}
	inline int getOPINION_WEIGHT_WARMONGER_MINOR()
	{
		return m_iOPINION_WEIGHT_WARMONGER_MINOR;
	}
	inline int getOPINION_WEIGHT_WARMONGER_NONE()
	{
		return m_iOPINION_WEIGHT_WARMONGER_NONE;
	}
	inline int getOPINION_WEIGHT_ASKED_NO_SETTLE()
	{
		return m_iOPINION_WEIGHT_ASKED_NO_SETTLE;
	}
	inline int getOPINION_WEIGHT_ASKED_STOP_SPYING()
	{
		return m_iOPINION_WEIGHT_ASKED_STOP_SPYING;
	}
	inline int getOPINION_WEIGHT_MADE_DEMAND_OF_US()
	{
		return m_iOPINION_WEIGHT_MADE_DEMAND_OF_US;
	}
	inline int getOPINION_WEIGHT_RETURNED_CIVILIAN()
	{
		return m_iOPINION_WEIGHT_RETURNED_CIVILIAN;
	}
	inline int getOPINION_WEIGHT_BUILT_LANDMARK()
	{
		return m_iOPINION_WEIGHT_BUILT_LANDMARK;
	}
	inline int getOPINION_WEIGHT_RESURRECTED()
	{
		return m_iOPINION_WEIGHT_RESURRECTED;
	}
	inline int getOPINION_WEIGHT_LIBERATED_ONE_CITY()
	{
		return m_iOPINION_WEIGHT_LIBERATED_ONE_CITY;
	}
	inline int getOPINION_WEIGHT_LIBERATED_TWO_CITIES()
	{
		return m_iOPINION_WEIGHT_LIBERATED_TWO_CITIES;
	}
	inline int getOPINION_WEIGHT_LIBERATED_THREE_CITIES()
	{
		return m_iOPINION_WEIGHT_LIBERATED_THREE_CITIES;
	}
	inline int getOPINION_WEIGHT_EMBASSY()
	{
		return m_iOPINION_WEIGHT_EMBASSY;
	}
	inline int getOPINION_WEIGHT_CULTURE_BOMBED()
	{
		return m_iOPINION_WEIGHT_CULTURE_BOMBED;
	}
	inline int getOPINION_WEIGHT_PER_NEGATIVE_CONVERSION()
	{
		return m_iOPINION_WEIGHT_PER_NEGATIVE_CONVERSION;
	}
	inline int getOPINION_WEIGHT_ADOPTING_HIS_RELIGION()
	{
		return m_iOPINION_WEIGHT_ADOPTING_HIS_RELIGION;
	}
	inline int getOPINION_WEIGHT_ADOPTING_MY_RELIGION()
	{
		return m_iOPINION_WEIGHT_ADOPTING_MY_RELIGION;
	}
	inline int getOPINION_WEIGHT_SAME_LATE_POLICIES()
	{
		return m_iOPINION_WEIGHT_SAME_LATE_POLICIES;
	}
	inline int getOPINION_WEIGHT_DIFFERENT_LATE_POLICIES()
	{
		return m_iOPINION_WEIGHT_DIFFERENT_LATE_POLICIES;
	}
	inline int getOPINION_WEIGHT_BROKEN_MILITARY_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_MILITARY_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_MILITARY_PROMISE_WORLD()
	{
		return m_iOPINION_WEIGHT_BROKEN_MILITARY_PROMISE_WORLD;
	}
	inline int getOPINION_WEIGHT_IGNORED_MILITARY_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_MILITARY_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_EXPANSION_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_EXPANSION_PROMISE;
	}
	inline int getOPINION_WEIGHT_IGNORED_EXPANSION_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_EXPANSION_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_BORDER_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_BORDER_PROMISE;
	}
	inline int getOPINION_WEIGHT_IGNORED_BORDER_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_BORDER_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE_WORLD()
	{
		return m_iOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE_WORLD;
	}
	inline int getOPINION_WEIGHT_IGNORED_CITY_STATE_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_CITY_STATE_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_SPY_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_SPY_PROMISE;
	}
	inline int getOPINION_WEIGHT_IGNORED_SPY_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_SPY_PROMISE;
	}
	inline int getOPINION_WEIGHT_FORGAVE_FOR_SPYING()
	{
		return m_iOPINION_WEIGHT_FORGAVE_FOR_SPYING;
	}
	inline int getOPINION_WEIGHT_BROKEN_NO_CONVERT_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_NO_CONVERT_PROMISE;
	}
	inline int getOPINION_WEIGHT_IGNORED_NO_CONVERT_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_NO_CONVERT_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_NO_DIG_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_NO_DIG_PROMISE;
	}
	inline int getOPINION_WEIGHT_IGNORED_NO_DIG_PROMISE()
	{
		return m_iOPINION_WEIGHT_IGNORED_NO_DIG_PROMISE;
	}
	inline int getOPINION_WEIGHT_BROKEN_COOP_WAR_PROMISE()
	{
		return m_iOPINION_WEIGHT_BROKEN_COOP_WAR_PROMISE;
	}
	inline int getOPINION_WEIGHT_KILLED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN()
	{
		return m_iOPINION_WEIGHT_KILLED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	}
	inline int getOPINION_WEIGHT_KILLED_PROTECTED_MINOR()
	{
		return m_iOPINION_WEIGHT_KILLED_PROTECTED_MINOR;
	}
	inline int getOPINION_WEIGHT_KILLED_MANY_PROTECTED_MINORS()
	{
		return m_iOPINION_WEIGHT_KILLED_MANY_PROTECTED_MINORS;
	}
	inline int getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN()
	{
		return m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	}
	inline int getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY_NUM_TURNS()
	{
		return m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY_NUM_TURNS;
	}
	inline int getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY()
	{
		return m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY;
	}
	inline int getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_WHILE_AGO()
	{
		return m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_WHILE_AGO;
	}
	inline int getOPINION_WEIGHT_ATTACKED_MANY_PROTECTED_MINORS()
	{
		return m_iOPINION_WEIGHT_ATTACKED_MANY_PROTECTED_MINORS;
	}
	inline int getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN()
	{
		return m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	}
	inline int getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY_NUM_TURNS()
	{
		return m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY_NUM_TURNS;
	}
	inline int getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY()
	{
		return m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY;
	}
	inline int getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_WHILE_AGO()
	{
		return m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_WHILE_AGO;
	}
	inline int getOPINION_WEIGHT_BULLIED_MANY_PROTECTED_MINORS()
	{
		return m_iOPINION_WEIGHT_BULLIED_MANY_PROTECTED_MINORS;
	}
	inline int getOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_NUM_TURNS_UNTIL_FORGIVEN()
	{
		return m_iOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	}
	inline int getOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR()
	{
		return m_iOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR;
	}
	inline int getOPINION_WEIGHT_DOF()
	{
		return m_iOPINION_WEIGHT_DOF;
	}
	inline int getOPINION_WEIGHT_DOF_WITH_FRIEND()
	{
		return m_iOPINION_WEIGHT_DOF_WITH_FRIEND;
	}
	inline int getOPINION_WEIGHT_DOF_WITH_ENEMY()
	{
		return m_iOPINION_WEIGHT_DOF_WITH_ENEMY;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_BY_FRIEND_EACH()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_BY_FRIEND_EACH;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_BY_FRIEND_DONT_LIKE()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_BY_FRIEND_DONT_LIKE;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_FRIEND_EACH()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_FRIEND_EACH;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_ME_FRIENDS()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_ME_FRIENDS;
	}
	inline int getOPINION_WEIGHT_WAR_FRIEND_EACH()
	{
		return m_iOPINION_WEIGHT_WAR_FRIEND_EACH;
	}
	inline int getOPINION_WEIGHT_WAR_ME_FRIENDS()
	{
		return m_iOPINION_WEIGHT_WAR_ME_FRIENDS;
	}
	inline int getOPINION_WEIGHT_REFUSED_REQUEST_EACH()
	{
		return m_iOPINION_WEIGHT_REFUSED_REQUEST_EACH;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_ME()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_ME;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_THEM()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_THEM;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_FRIEND()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_FRIEND;
	}
	inline int getOPINION_WEIGHT_DENOUNCED_ENEMY()
	{
		return m_iOPINION_WEIGHT_DENOUNCED_ENEMY;
	}
	inline int getOPINION_WEIGHT_RECKLESS_EXPANDER()
	{
		return m_iOPINION_WEIGHT_RECKLESS_EXPANDER;
	}
	inline int getOPINION_WEIGHT_TRADE_MAX()
	{
		return m_iOPINION_WEIGHT_TRADE_MAX;
	}
	inline int getOPINION_WEIGHT_COMMON_FOE_MAX()
	{
		return m_iOPINION_WEIGHT_COMMON_FOE_MAX;
	}
	inline int getOPINION_WEIGHT_ASSIST_MAX()
	{
		return m_iOPINION_WEIGHT_ASSIST_MAX;
	}
	inline int getOPINION_WEIGHT_LIBERATED_CAPITAL()
	{
		return m_iOPINION_WEIGHT_LIBERATED_CAPITAL;
	}
	inline int getOPINION_WEIGHT_LIBERATED_CITY()
	{
		return m_iOPINION_WEIGHT_LIBERATED_CITY;
	}
	inline int getOPINION_WEIGHT_GAVE_ASSISTANCE()
	{
		return m_iOPINION_WEIGHT_GAVE_ASSISTANCE;
	}
	inline int getOPINION_WEIGHT_PAID_TRIBUTE()
	{
		return m_iOPINION_WEIGHT_PAID_TRIBUTE;
	}
	inline int getOPINION_WEIGHT_NUKED_MAX()
	{
		return m_iOPINION_WEIGHT_NUKED_MAX;
	}
	inline int getOPINION_WEIGHT_ROBBED_BY()
	{
		return m_iOPINION_WEIGHT_ROBBED_BY;
	}
	inline int getOPINION_WEIGHT_INTRIGUE_SHARED_BY()
	{
		return m_iOPINION_WEIGHT_INTRIGUE_SHARED_BY;
	}
	inline int getOPINION_WEIGHT_CAPTURED_CAPITAL()
	{
		return m_iOPINION_WEIGHT_CAPTURED_CAPITAL;
	}
#define CV_GLOBAL_DEFINE(kind, name, initial) GD_##kind##_DEF(name)
#include "CvGlobalDefines.inc"
#undef CV_GLOBAL_DEFINE
#if !defined(LEKMOD_PLAYER_GOLDEN_AGE_YIELD_MOD_INFO)
	inline int getGOLDEN_AGE_CULTURE_MODIFIER()
	{
		return m_iGOLDEN_AGE_CULTURE_MODIFIER;
	}
#endif
	inline int getHILLS_EXTRA_MOVEMENT()
	{
		return m_iHILLS_EXTRA_MOVEMENT;
	}
	inline int getRIVER_EXTRA_MOVEMENT()
	{
		return m_iRIVER_EXTRA_MOVEMENT;
	}
	inline int getFEATURE_GROWTH_MODIFIER()
	{
		return m_iFEATURE_GROWTH_MODIFIER;
	}
	inline int getROUTE_FEATURE_GROWTH_MODIFIER()
	{
		return m_iROUTE_FEATURE_GROWTH_MODIFIER;
	}
	inline int getEXTRA_YIELD()
	{
		return m_iEXTRA_YIELD;
	}
	inline int getFORTIFY_MODIFIER_PER_TURN()
	{
		return m_iFORTIFY_MODIFIER_PER_TURN;
	}
	inline int getMAX_FORTIFY_TURNS()
	{
		return m_iMAX_FORTIFY_TURNS;
	}
	inline int getNUKE_FALLOUT_PROB()
	{
		return m_iNUKE_FALLOUT_PROB;
	}
	inline int getNUKE_UNIT_DAMAGE_BASE()
	{
		return m_iNUKE_UNIT_DAMAGE_BASE;
	}
	inline int getNUKE_UNIT_DAMAGE_RAND_1()
	{
		return m_iNUKE_UNIT_DAMAGE_RAND_1;
	}
	inline int getNUKE_UNIT_DAMAGE_RAND_2()
	{
		return m_iNUKE_UNIT_DAMAGE_RAND_2;
	}
	inline int getNUKE_NON_COMBAT_DEATH_THRESHOLD()
	{
		return m_iNUKE_NON_COMBAT_DEATH_THRESHOLD;
	}
	inline int getNUKE_LEVEL1_POPULATION_DEATH_BASE()
	{
		return m_iNUKE_LEVEL1_POPULATION_DEATH_BASE;
	}
	inline int getNUKE_LEVEL1_POPULATION_DEATH_RAND_1()
	{
		return m_iNUKE_LEVEL1_POPULATION_DEATH_RAND_1;
	}
	inline int getNUKE_LEVEL1_POPULATION_DEATH_RAND_2()
	{
		return m_iNUKE_LEVEL1_POPULATION_DEATH_RAND_2;
	}
	inline int getNUKE_LEVEL2_POPULATION_DEATH_BASE()
	{
		return m_iNUKE_LEVEL2_POPULATION_DEATH_BASE;
	}
	inline int getNUKE_LEVEL2_POPULATION_DEATH_RAND_1()
	{
		return m_iNUKE_LEVEL2_POPULATION_DEATH_RAND_1;
	}
	inline int getNUKE_LEVEL2_POPULATION_DEATH_RAND_2()
	{
		return m_iNUKE_LEVEL2_POPULATION_DEATH_RAND_2;
	}
	inline int getNUKE_LEVEL2_ELIM_POPULATION_THRESHOLD()
	{
		return m_iNUKE_LEVEL2_ELIM_POPULATION_THRESHOLD;
	}
	inline int getNUKE_CITY_HIT_POINT_DAMAGE()
	{
		return m_iNUKE_CITY_HIT_POINT_DAMAGE;
	}
	inline int getNUKE_BLAST_RADIUS()
	{
		return m_iNUKE_BLAST_RADIUS;
	}
	inline int getTECH_COST_EXTRA_TEAM_MEMBER_MODIFIER()
	{
		return m_iTECH_COST_EXTRA_TEAM_MEMBER_MODIFIER;
	}
	inline int getTECH_COST_TOTAL_KNOWN_TEAM_MODIFIER()
	{
		return m_iTECH_COST_TOTAL_KNOWN_TEAM_MODIFIER;
	}
	inline int getTECH_COST_KNOWN_PREREQ_MODIFIER()
	{
		return m_iTECH_COST_KNOWN_PREREQ_MODIFIER;
	}
	inline int getPEACE_TREATY_LENGTH()
	{
		return m_iPEACE_TREATY_LENGTH;
	}
	inline int getCOOP_WAR_LOCKED_LENGTH()
	{
		return m_iCOOP_WAR_LOCKED_LENGTH;
	}
	inline int getBASE_FEATURE_PRODUCTION_PERCENT()
	{
		return m_iBASE_FEATURE_PRODUCTION_PERCENT;
	}
	inline int getFEATURE_PRODUCTION_PERCENT_MULTIPLIER()
	{
		return m_iFEATURE_PRODUCTION_PERCENT_MULTIPLIER;
	}
	inline int getDIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT()
	{
		return m_iDIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT;
	}
	inline int getDEFAULT_WAR_VALUE_FOR_UNIT()
	{
		return m_iDEFAULT_WAR_VALUE_FOR_UNIT;
	}
	inline int getUNIT_PRODUCTION_PERCENT()
	{
		return m_iUNIT_PRODUCTION_PERCENT;
	}
	inline int getMAX_UNIT_SUPPLY_PRODMOD()
	{
		return m_iMAX_UNIT_SUPPLY_PRODMOD;
	}
	inline int getBUILDING_PRODUCTION_PERCENT()
	{
		return m_iBUILDING_PRODUCTION_PERCENT;
	}
	inline int getPROJECT_PRODUCTION_PERCENT()
	{
		return m_iPROJECT_PRODUCTION_PERCENT;
	}
	inline int getMAXED_UNIT_GOLD_PERCENT()
	{
		return m_iMAXED_UNIT_GOLD_PERCENT;
	}
	inline int getMAXED_BUILDING_GOLD_PERCENT()
	{
		return m_iMAXED_BUILDING_GOLD_PERCENT;
	}
	inline int getMAXED_PROJECT_GOLD_PERCENT()
	{
		return m_iMAXED_PROJECT_GOLD_PERCENT;
	}
	inline int getMAX_CITY_DEFENSE_DAMAGE()
	{
		return m_iMAX_CITY_DEFENSE_DAMAGE;
	}
	inline int getCIRCUMNAVIGATE_FREE_MOVES()
	{
		return m_iCIRCUMNAVIGATE_FREE_MOVES;
	}
	inline int getBASE_CAPTURE_GOLD()
	{
		return m_iBASE_CAPTURE_GOLD;
	}
	inline int getCAPTURE_GOLD_PER_POPULATION()
	{
		return m_iCAPTURE_GOLD_PER_POPULATION;
	}
	inline int getCAPTURE_GOLD_RAND1()
	{
		return m_iCAPTURE_GOLD_RAND1;
	}
	inline int getCAPTURE_GOLD_RAND2()
	{
		return m_iCAPTURE_GOLD_RAND2;
	}
	inline int getCAPTURE_GOLD_MAX_TURNS()
	{
		return m_iCAPTURE_GOLD_MAX_TURNS;
	}
	inline int getBARBARIAN_CITY_ATTACK_MODIFIER()
	{
		return m_iBARBARIAN_CITY_ATTACK_MODIFIER;
	}
	inline int getBUILDING_PRODUCTION_DECAY_TIME()
	{
		return m_iBUILDING_PRODUCTION_DECAY_TIME;
	}
	inline int getBUILDING_PRODUCTION_DECAY_PERCENT()
	{
		return m_iBUILDING_PRODUCTION_DECAY_PERCENT;
	}
	inline int getUNIT_PRODUCTION_DECAY_TIME()
	{
		return m_iUNIT_PRODUCTION_DECAY_TIME;
	}
	inline int getUNIT_PRODUCTION_DECAY_PERCENT()
	{
		return m_iUNIT_PRODUCTION_DECAY_PERCENT;
	}
	inline int getBASE_UNIT_UPGRADE_COST()
	{
		return m_iBASE_UNIT_UPGRADE_COST;
	}
	inline int getUNIT_UPGRADE_COST_PER_PRODUCTION()
	{
		return m_iUNIT_UPGRADE_COST_PER_PRODUCTION;
	}
	inline int getUNIT_UPGRADE_COST_VISIBLE_DIVISOR()
	{
		return m_iUNIT_UPGRADE_COST_VISIBLE_DIVISOR;
	}
	inline int getUNIT_UPGRADE_COST_DISCOUNT_MAX()
	{
		return m_iUNIT_UPGRADE_COST_DISCOUNT_MAX;
	}
	inline int getWAR_SUCCESS_UNIT_CAPTURING()
	{
		return m_iWAR_SUCCESS_UNIT_CAPTURING;
	}
	inline int getWAR_SUCCESS_CITY_CAPTURING()
	{
		return m_iWAR_SUCCESS_CITY_CAPTURING;
	}
	inline int getDIPLO_VOTE_SECRETARY_GENERAL_INTERVAL()
	{
		return m_iDIPLO_VOTE_SECRETARY_GENERAL_INTERVAL;
	}
	inline int getTEAM_VOTE_MIN_CANDIDATES()
	{
		return m_iTEAM_VOTE_MIN_CANDIDATES;
	}
	inline int getRESEARCH_AGREEMENT_TIMER()
	{
		return m_iRESEARCH_AGREEMENT_TIMER;
	}
	inline int getRESEARCH_AGREEMENT_BOOST_DIVISOR()
	{
		return m_iRESEARCH_AGREEMENT_BOOST_DIVISOR;
	}
	inline int getSCORE_POPULATION_FACTOR()
	{
		return m_iSCORE_POPULATION_FACTOR;
	}
	inline int getSCORE_LAND_FACTOR()
	{
		return m_iSCORE_LAND_FACTOR;
	}
	inline int getSCORE_WONDER_FACTOR()
	{
		return m_iSCORE_WONDER_FACTOR;
	}
	inline int getSCORE_TECH_FACTOR()
	{
		return m_iSCORE_TECH_FACTOR;
	}
	inline int getSCORE_FREE_PERCENT()
	{
		return m_iSCORE_FREE_PERCENT;
	}
	inline int getSCORE_VICTORY_PERCENT()
	{
		return m_iSCORE_VICTORY_PERCENT;
	}
	inline int getSCORE_HANDICAP_PERCENT_OFFSET()
	{
		return m_iSCORE_HANDICAP_PERCENT_OFFSET;
	}
	inline int getSCORE_HANDICAP_PERCENT_PER()
	{
		return m_iSCORE_HANDICAP_PERCENT_PER;
	}
	inline int getMINIMAP_RENDER_SIZE()
	{
		return m_iMINIMAP_RENDER_SIZE;
	}
	inline int getMAX_INTERCEPTION_PROBABILITY()
	{
		return m_iMAX_INTERCEPTION_PROBABILITY;
	}
	inline int getMAX_EVASION_PROBABILITY()
	{
		return m_iMAX_EVASION_PROBABILITY;
	}
	inline int getPLAYER_ALWAYS_RAZES_CITIES()
	{
		return m_iPLAYER_ALWAYS_RAZES_CITIES;
	}
	inline int getMIN_WATER_SIZE_FOR_OCEAN()
	{
		return m_iMIN_WATER_SIZE_FOR_OCEAN;
	}
	inline int getCITY_SCREEN_CLICK_WILL_EXIT()
	{
		return m_iCITY_SCREEN_CLICK_WILL_EXIT;
	}
	inline int getWATER_POTENTIAL_CITY_WORK_FOR_AREA()
	{
		return m_iWATER_POTENTIAL_CITY_WORK_FOR_AREA;
	}
	inline int getLAND_UNITS_CAN_ATTACK_WATER_CITIES()
	{
		return m_iLAND_UNITS_CAN_ATTACK_WATER_CITIES;
	}
	inline int getCITY_MAX_NUM_BUILDINGS()
	{
		return m_iCITY_MAX_NUM_BUILDINGS;
	}
	inline int getCITY_MIN_SIZE_FOR_SETTLERS()
	{
		return m_iCITY_MIN_SIZE_FOR_SETTLERS;
	}
	inline int getRANGED_ATTACKS_USE_MOVES()
	{
		return m_iRANGED_ATTACKS_USE_MOVES;
	}
	inline int getADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES()
	{
		return m_iADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES;
	}
	inline int getADVANCED_START_MAX_UNITS_PER_CITY()
	{
		return m_iADVANCED_START_MAX_UNITS_PER_CITY;
	}
	inline int getADVANCED_START_CITY_COST()
	{
		return m_iADVANCED_START_CITY_COST;
	}
	inline int getADVANCED_START_CITY_COST_INCREASE()
	{
		return m_iADVANCED_START_CITY_COST_INCREASE;
	}
	inline int getADVANCED_START_POPULATION_COST()
	{
		return m_iADVANCED_START_POPULATION_COST;
	}
	inline int getADVANCED_START_POPULATION_COST_INCREASE()
	{
		return m_iADVANCED_START_POPULATION_COST_INCREASE;
	}
	inline int getADVANCED_START_VISIBILITY_COST()
	{
		return m_iADVANCED_START_VISIBILITY_COST;
	}
	inline int getADVANCED_START_VISIBILITY_COST_INCREASE()
	{
		return m_iADVANCED_START_VISIBILITY_COST_INCREASE;
	}
	inline int getADVANCED_START_CITY_PLACEMENT_MAX_RANGE()
	{
		return m_iADVANCED_START_CITY_PLACEMENT_MAX_RANGE;
	}
	inline int getNEW_CITY_BUILDING_VALUE_MODIFIER()
	{
		return m_iNEW_CITY_BUILDING_VALUE_MODIFIER;
	}
	inline int getPATH_DAMAGE_WEIGHT()
	{
		return m_iPATH_DAMAGE_WEIGHT;
	}
#if !defined(LEKMOD_PUPPET_YIELD_MOD_INFO)
	inline int getPUPPET_SCIENCE_MODIFIER()
	{
		return m_iPUPPET_SCIENCE_MODIFIER;
	}
	inline int getPUPPET_CULTURE_MODIFIER()
	{
		return m_iPUPPET_CULTURE_MODIFIER;
	}
	inline int getPUPPET_GOLD_MODIFIER()
	{
		return m_iPUPPET_GOLD_MODIFIER;
	}
	inline int getPUPPET_FAITH_MODIFIER()
	{
		return m_iPUPPET_FAITH_MODIFIER;
	}
#endif
	inline int getBASE_POLICY_COST()
	{
		return m_iBASE_POLICY_COST;
	}
	inline int getPOLICY_COST_INCREASE_TO_BE_EXPONENTED()
	{
		return m_iPOLICY_COST_INCREASE_TO_BE_EXPONENTED;
	}
	inline int getPOLICY_COST_VISIBLE_DIVISOR()
	{
		return m_iPOLICY_COST_VISIBLE_DIVISOR;
	}
	inline int getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS()
	{
		return m_iSWITCH_POLICY_BRANCHES_ANARCHY_TURNS;
	}
	inline int getSWITCH_POLICY_BRANCHES_TENETS_LOST()
	{
		return m_iSWITCH_POLICY_BRANCHES_TENETS_LOST;
	}
	inline int getPOLICY_COST_DISCOUNT_MAX()
	{
		return m_iPOLICY_COST_DISCOUNT_MAX;
	}
	inline int getGOLD_PURCHASE_GOLD_PER_PRODUCTION()
	{
		return m_iGOLD_PURCHASE_GOLD_PER_PRODUCTION;
	}
	inline int getGOLD_PURCHASE_VISIBLE_DIVISOR()
	{
		return m_iGOLD_PURCHASE_VISIBLE_DIVISOR;
	}
	inline int getPROJECT_PURCHASING_DISABLED()
	{
		return m_iPROJECT_PURCHASING_DISABLED;
	}
	inline int getINFLUENCE_MOUNTAIN_COST()
	{
		return m_iINFLUENCE_MOUNTAIN_COST;
	}
	inline int getINFLUENCE_HILL_COST()
	{
		return m_iINFLUENCE_HILL_COST;
	}
	inline int getINFLUENCE_RIVER_COST()
	{
		return m_iINFLUENCE_RIVER_COST;
	}
	inline int getUSE_FIRST_RING_INFLUENCE_TERRAIN_COST()
	{
		return m_iUSE_FIRST_RING_INFLUENCE_TERRAIN_COST;
	}
	inline int getNUM_RESOURCE_QUANTITY_TYPES()
	{
		return m_iNUM_RESOURCE_QUANTITY_TYPES;
	}
	inline int getSPECIALISTS_DIVERT_POPULATION_ENABLED()
	{
		return m_iSPECIALISTS_DIVERT_POPULATION_ENABLED;
	}
	inline int getSCIENCE_PER_POPULATION()
	{
		return m_iSCIENCE_PER_POPULATION;
	}
	inline int getRESEARCH_AGREEMENT_MOD()
	{
		return m_iRESEARCH_AGREEMENT_MOD;
	}
	inline int getBARBARIAN_CAMP_FIRST_TURN_PERCENT_OF_TARGET_TO_ADD()
	{
		return m_iBARBARIAN_CAMP_FIRST_TURN_PERCENT_OF_TARGET_TO_ADD;
	}
	inline int getBARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING()
	{
		return m_iBARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING;
	}
	inline int getBARBARIAN_CAMP_MINIMUM_DISTANCE_CAPITAL()
	{
		return m_iBARBARIAN_CAMP_MINIMUM_DISTANCE_CAPITAL;
	}
	inline int getBARBARIAN_CAMP_MINIMUM_DISTANCE_ANOTHER_CAMP()
	{
		return m_iBARBARIAN_CAMP_MINIMUM_DISTANCE_ANOTHER_CAMP;
	}
	inline int getBARBARIAN_CAMP_COASTAL_SPAWN_ROLL()
	{
		return m_iBARBARIAN_CAMP_COASTAL_SPAWN_ROLL;
	}
	inline int getBARBARIAN_EXTRA_RAGING_UNIT_SPAWN_CHANCE()
	{
		return m_iBARBARIAN_EXTRA_RAGING_UNIT_SPAWN_CHANCE;
	}
	inline int getBARBARIAN_NAVAL_UNIT_START_TURN_SPAWN()
	{
		return m_iBARBARIAN_NAVAL_UNIT_START_TURN_SPAWN;
	}
	inline int getMAX_BARBARIANS_FROM_CAMP_NEARBY()
	{
		return m_iMAX_BARBARIANS_FROM_CAMP_NEARBY;
	}
	inline int getMAX_BARBARIANS_FROM_CAMP_NEARBY_RANGE()
	{
		return m_iMAX_BARBARIANS_FROM_CAMP_NEARBY_RANGE;
	}
	inline int getGOLD_FROM_BARBARIAN_CONVERSION()
	{
		return m_iGOLD_FROM_BARBARIAN_CONVERSION;
	}
	inline int getBARBARIAN_CITY_GOLD_RANSOM()
	{
		return m_iBARBARIAN_CITY_GOLD_RANSOM;
	}
	inline int getBARBARIAN_UNIT_GOLD_RANSOM()
	{
		return m_iBARBARIAN_UNIT_GOLD_RANSOM;
	}
	inline int getEMBARKED_UNIT_MOVEMENT()
	{
		return m_iEMBARKED_UNIT_MOVEMENT;
	}
	inline int getEMBARKED_VISIBILITY_RANGE()
	{
		return m_iEMBARKED_VISIBILITY_RANGE;
	}
	inline int getDEFAULT_MAX_NUM_BUILDERS()
	{
		return m_iDEFAULT_MAX_NUM_BUILDERS;
	}
	inline int getBARBARIAN_TECH_PERCENT()
	{
		return m_iBARBARIAN_TECH_PERCENT;
	}
	inline int getCITY_RESOURCE_WLTKD_TURNS()
	{
		return m_iCITY_RESOURCE_WLTKD_TURNS;
	}
	inline int getMAX_SPECIALISTS_FROM_BUILDING()
	{
		return m_iMAX_SPECIALISTS_FROM_BUILDING;
	}
	inline int getGREAT_PERSON_THRESHOLD_BASE()
	{
		return m_iGREAT_PERSON_THRESHOLD_BASE;
	}
	inline int getGREAT_PERSON_THRESHOLD_INCREASE()
	{
		return m_iGREAT_PERSON_THRESHOLD_INCREASE;
	}
	inline int getCULTURE_BOMB_COOLDOWN()
	{
		return m_iCULTURE_BOMB_COOLDOWN;
	}
	inline int getCULTURE_BOMB_MINOR_FRIENDSHIP_CHANGE()
	{
		return m_iCULTURE_BOMB_MINOR_FRIENDSHIP_CHANGE;
	}
	inline int getLANDMARK_MINOR_FRIENDSHIP_CHANGE()
	{
		return m_iLANDMARK_MINOR_FRIENDSHIP_CHANGE;
	}
	inline int getUNIT_AUTO_EXPLORE_DISABLED()
	{
		return m_iUNIT_AUTO_EXPLORE_DISABLED;
	}
	inline int getUNIT_AUTO_EXPLORE_FULL_DISABLED()
	{
		return m_iUNIT_AUTO_EXPLORE_FULL_DISABLED;
	}
	inline int getUNIT_WORKER_AUTOMATION_DISABLED()
	{
		return m_iUNIT_WORKER_AUTOMATION_DISABLED;
	}
	inline int getUNIT_DELETE_DISABLED()
	{
		return m_iUNIT_DELETE_DISABLED;
	}
	inline int getMIN_START_AREA_TILES()
	{
		return m_iMIN_START_AREA_TILES;
	}
	inline int getMIN_DISTANCE_OTHER_AREA_PERCENT()
	{
		return m_iMIN_DISTANCE_OTHER_AREA_PERCENT;
	}
	inline int getMINOR_CIV_FOOD_REQUIREMENT()
	{
		return m_iMINOR_CIV_FOOD_REQUIREMENT;
	}
	inline int getMAJOR_CIV_FOOD_REQUIREMENT()
	{
		return m_iMAJOR_CIV_FOOD_REQUIREMENT;
	}
	inline int getMIN_START_FOUND_VALUE_AS_PERCENT_OF_BEST()
	{
		return m_iMIN_START_FOUND_VALUE_AS_PERCENT_OF_BEST;
	}
	inline int getSTART_AREA_FOOD_MULTIPLIER()
	{
		return m_iSTART_AREA_FOOD_MULTIPLIER;
	}
	inline int getSTART_AREA_HAPPINESS_MULTIPLIER()
	{
		return m_iSTART_AREA_HAPPINESS_MULTIPLIER;
	}
	inline int getSTART_AREA_PRODUCTION_MULTIPLIER()
	{
		return m_iSTART_AREA_PRODUCTION_MULTIPLIER;
	}
	inline int getSTART_AREA_GOLD_MULTIPLIER()
	{
		return m_iSTART_AREA_GOLD_MULTIPLIER;
	}
	inline int getSTART_AREA_SCIENCE_MULTIPLIER()
	{
		return m_iSTART_AREA_SCIENCE_MULTIPLIER;
	}
#ifdef AUI_SITE_EVALUATION_PLOT_FOUND_VALUE_CONSIDER_CULTURE
	GD_INT_DEF(START_AREA_CULTURE_MULTIPLIER)
#endif
	inline int getSTART_AREA_FAITH_MULTIPLIER()
	{
		return m_iSTART_AREA_FAITH_MULTIPLIER;
	}
	inline int getSTART_AREA_RESOURCE_MULTIPLIER()
	{
		return m_iSTART_AREA_RESOURCE_MULTIPLIER;
	}
	inline int getSTART_AREA_STRATEGIC_MULTIPLIER()
	{
		return m_iSTART_AREA_STRATEGIC_MULTIPLIER;
	}
	inline int getSTART_AREA_BUILD_ON_COAST_PERCENT()
	{
		return m_iSTART_AREA_BUILD_ON_COAST_PERCENT;
	}
	inline int getSETTLER_FOOD_MULTIPLIER()
	{
		return m_iSETTLER_FOOD_MULTIPLIER;
	}
	inline int getSETTLER_HAPPINESS_MULTIPLIER()
	{
		return m_iSETTLER_HAPPINESS_MULTIPLIER;
	}
	inline int getSETTLER_PRODUCTION_MULTIPLIER()
	{
		return m_iSETTLER_PRODUCTION_MULTIPLIER;
	}
	inline int getSETTLER_GOLD_MULTIPLIER()
	{
		return m_iSETTLER_GOLD_MULTIPLIER;
	}
	inline int getSETTLER_SCIENCE_MULTIPLIER()
	{
		return m_iSETTLER_SCIENCE_MULTIPLIER;
	}
#ifdef AUI_SITE_EVALUATION_PLOT_FOUND_VALUE_CONSIDER_CULTURE
	GD_INT_DEF(SETTLER_CULTURE_MULTIPLIER)
#endif
	inline int getSETTLER_FAITH_MULTIPLIER()
	{
		return m_iSETTLER_FAITH_MULTIPLIER;
	}
	inline int getSETTLER_RESOURCE_MULTIPLIER()
	{
		return m_iSETTLER_RESOURCE_MULTIPLIER;
	}
	inline int getSETTLER_STRATEGIC_MULTIPLIER()
	{
		return m_iSETTLER_STRATEGIC_MULTIPLIER;
	}
	inline int getSETTLER_BUILD_ON_COAST_PERCENT()
	{
		return m_iSETTLER_BUILD_ON_COAST_PERCENT;
	}
#ifdef AUI_SITE_EVALUATION_COMPUTE_YIELD_VALUE_RECOGNIZE_CITY_PLOT
	GD_INT_DEF(CITY_RING_0_MULTIPLIER)
#endif
	inline int getCITY_RING_1_MULTIPLIER()
	{
		return m_iCITY_RING_1_MULTIPLIER;
	}
	inline int getCITY_RING_2_MULTIPLIER()
	{
		return m_iCITY_RING_2_MULTIPLIER;
	}
	inline int getCITY_RING_3_MULTIPLIER()
	{
		return m_iCITY_RING_3_MULTIPLIER;
	}
	inline int getCITY_RING_4_MULTIPLIER()
	{
		return m_iCITY_RING_4_MULTIPLIER;
	}
	inline int getCITY_RING_5_MULTIPLIER()
	{
		return m_iCITY_RING_5_MULTIPLIER;
	}
#ifdef AUI_SITE_EVALUATION_YIELD_MULTIPLIER_DISTANCE_DECAY
	GD_INT_DEF(SETTLER_FOOD_RING_0_DIVIDER);
	GD_INT_DEF(SETTLER_PRODUCTION_RING_0_DIVIDER);
	GD_INT_DEF(SETTLER_GOLD_RING_0_DIVIDER);
	GD_INT_DEF(SETTLER_SCIENCE_RING_0_DIVIDER);
	GD_INT_DEF(SETTLER_CULTURE_RING_0_DIVIDER);
	GD_INT_DEF(SETTLER_FAITH_RING_0_DIVIDER);
	GD_INT_DEF(SETTLER_HAPPINESS_RING_0_DIVIDER);
	GD_INT_DEF(SETTLER_RESOURCES_RING_0_DIVIDER);
	GD_INT_DEF(SETTLER_STRATEGIC_RING_0_DIVIDER);
	GD_INT_DEF(SETTLER_FOOD_RING_1_DIVIDER);
	GD_INT_DEF(SETTLER_PRODUCTION_RING_1_DIVIDER);
	GD_INT_DEF(SETTLER_GOLD_RING_1_DIVIDER);
	GD_INT_DEF(SETTLER_SCIENCE_RING_1_DIVIDER);
	GD_INT_DEF(SETTLER_CULTURE_RING_1_DIVIDER);
	GD_INT_DEF(SETTLER_FAITH_RING_1_DIVIDER);
	GD_INT_DEF(SETTLER_HAPPINESS_RING_1_DIVIDER);
	GD_INT_DEF(SETTLER_RESOURCES_RING_1_DIVIDER);
	GD_INT_DEF(SETTLER_STRATEGIC_RING_1_DIVIDER);
	GD_INT_DEF(SETTLER_FOOD_RING_2_DIVIDER);
	GD_INT_DEF(SETTLER_PRODUCTION_RING_2_DIVIDER);
	GD_INT_DEF(SETTLER_GOLD_RING_2_DIVIDER);
	GD_INT_DEF(SETTLER_SCIENCE_RING_2_DIVIDER);
	GD_INT_DEF(SETTLER_CULTURE_RING_2_DIVIDER);
	GD_INT_DEF(SETTLER_FAITH_RING_2_DIVIDER);
	GD_INT_DEF(SETTLER_HAPPINESS_RING_2_DIVIDER);
	GD_INT_DEF(SETTLER_RESOURCES_RING_2_DIVIDER);
	GD_INT_DEF(SETTLER_STRATEGIC_RING_2_DIVIDER);
	GD_INT_DEF(SETTLER_FOOD_RING_3_DIVIDER);
	GD_INT_DEF(SETTLER_PRODUCTION_RING_3_DIVIDER);
	GD_INT_DEF(SETTLER_GOLD_RING_3_DIVIDER);
	GD_INT_DEF(SETTLER_SCIENCE_RING_3_DIVIDER);
	GD_INT_DEF(SETTLER_CULTURE_RING_3_DIVIDER);
	GD_INT_DEF(SETTLER_FAITH_RING_3_DIVIDER);
	GD_INT_DEF(SETTLER_HAPPINESS_RING_3_DIVIDER);
	GD_INT_DEF(SETTLER_RESOURCES_RING_3_DIVIDER);
	GD_INT_DEF(SETTLER_STRATEGIC_RING_3_DIVIDER);
	GD_INT_DEF(SETTLER_HAPPINESS_RING_4_DIVIDER);
	GD_INT_DEF(SETTLER_RESOURCES_RING_4_DIVIDER);
	GD_INT_DEF(SETTLER_STRATEGIC_RING_4_DIVIDER);
	GD_INT_DEF(SETTLER_HAPPINESS_RING_5_DIVIDER);
	GD_INT_DEF(SETTLER_RESOURCES_RING_5_DIVIDER);
	GD_INT_DEF(SETTLER_STRATEGIC_RING_5_DIVIDER);
#endif
	inline int getSETTLER_EVALUATION_DISTANCE()
	{
		return m_iSETTLER_EVALUATION_DISTANCE;
	}
	inline int getSETTLER_DISTANCE_DROPOFF_MODIFIER()
	{
		return m_iSETTLER_DISTANCE_DROPOFF_MODIFIER;
	}
	inline int getBUILD_ON_RESOURCE_PERCENT()
	{
		return m_iBUILD_ON_RESOURCE_PERCENT;
	}
	inline int getBUILD_ON_RIVER_PERCENT()
	{
		return m_iBUILD_ON_RIVER_PERCENT;
	}
	inline int getCHOKEPOINT_STRATEGIC_VALUE()
	{
		return m_iCHOKEPOINT_STRATEGIC_VALUE;
	}
	inline int getHILL_STRATEGIC_VALUE()
	{
		return m_iHILL_STRATEGIC_VALUE;
	}
	inline int getALREADY_OWNED_STRATEGIC_VALUE()
	{
		return m_iALREADY_OWNED_STRATEGIC_VALUE;
	}
	inline int getMINOR_CIV_CONTACT_GOLD_FIRST()
	{
		return m_iMINOR_CIV_CONTACT_GOLD_FIRST;
	}
	inline int getMINOR_CIV_CONTACT_GOLD_OTHER()
	{
		return m_iMINOR_CIV_CONTACT_GOLD_OTHER;
	}
	inline int getMINOR_CIV_GROWTH_PERCENT()
	{
		return m_iMINOR_CIV_GROWTH_PERCENT;
	}
	inline int getMINOR_CIV_PRODUCTION_PERCENT()
	{
		return m_iMINOR_CIV_PRODUCTION_PERCENT;
	}
	inline int getMINOR_CIV_GOLD_PERCENT()
	{
		return m_iMINOR_CIV_GOLD_PERCENT;
	}
	inline int getMINOR_CIV_TECH_PERCENT()
	{
		return m_iMINOR_CIV_TECH_PERCENT;
	}
	inline int getMINOR_POLICY_RESOURCE_MULTIPLIER()
	{
		return m_iMINOR_POLICY_RESOURCE_MULTIPLIER;
	}
	inline int getMINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER()
	{
		return m_iMINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER;
	}
	inline int getMINOR_GOLD_GIFT_LARGE()
	{
		return m_iMINOR_GOLD_GIFT_LARGE;
	}
	inline int getMINOR_GOLD_GIFT_MEDIUM()
	{
		return m_iMINOR_GOLD_GIFT_MEDIUM;
	}
	inline int getMINOR_GOLD_GIFT_SMALL()
	{
		return m_iMINOR_GOLD_GIFT_SMALL;
	}
	inline int getMINOR_CIV_TILE_IMPROVEMENT_GIFT_COST()
	{
		return m_iMINOR_CIV_TILE_IMPROVEMENT_GIFT_COST;
	}
	inline int getMINOR_CIV_BUYOUT_COST()
	{
		return m_iMINOR_CIV_BUYOUT_COST;
	}
	inline int getMINOR_CIV_BUYOUT_TURNS()
	{
		return m_iMINOR_CIV_BUYOUT_TURNS;
	}
	inline int getMINOR_FRIENDSHIP_FROM_TRADE_MISSION()
	{
		return m_iMINOR_FRIENDSHIP_FROM_TRADE_MISSION;
	}
	inline int getMINOR_FRIENDSHIP_ANCHOR_DEFAULT()
	{
		return m_iMINOR_FRIENDSHIP_ANCHOR_DEFAULT;
	}
	inline int getMINOR_FRIENDSHIP_ANCHOR_MOD_PROTECTED()
	{
		return m_iMINOR_FRIENDSHIP_ANCHOR_MOD_PROTECTED;
	}
	inline int getMINOR_FRIENDSHIP_ANCHOR_MOD_WARY_OF()
	{
		return m_iMINOR_FRIENDSHIP_ANCHOR_MOD_WARY_OF;
	}
	inline int getMINOR_UNIT_GIFT_TRAVEL_TURNS()
	{
		return m_iMINOR_UNIT_GIFT_TRAVEL_TURNS;
	}
	inline int getPLOT_UNIT_LIMIT()
	{
		return m_iPLOT_UNIT_LIMIT;
	}
	inline int getZONE_OF_CONTROL_ENABLED()
	{
		return m_iZONE_OF_CONTROL_ENABLED;
	}
	inline int getFIRE_SUPPORT_DISABLED()
	{
		return m_iFIRE_SUPPORT_DISABLED;
	}
	inline int getMAX_HIT_POINTS()
	{
		return m_iMAX_HIT_POINTS;
	}
	inline int getMAX_CITY_HIT_POINTS()
	{
		return m_iMAX_CITY_HIT_POINTS;
	}
	inline int getCITY_HIT_POINTS_HEALED_PER_TURN()
	{
		return m_iCITY_HIT_POINTS_HEALED_PER_TURN;
	}
	inline int getFLAT_LAND_EXTRA_DEFENSE()
	{
		return m_iFLAT_LAND_EXTRA_DEFENSE;
	}
	inline int getHILLS_EXTRA_DEFENSE()
	{
		return m_iHILLS_EXTRA_DEFENSE;
	}
	inline int getRIVER_ATTACK_MODIFIER()
	{
		return m_iRIVER_ATTACK_MODIFIER;
	}
	inline int getAMPHIB_ATTACK_MODIFIER()
	{
		return m_iAMPHIB_ATTACK_MODIFIER;
	}
	inline int getENEMY_HEAL_RATE()
	{
		return m_iENEMY_HEAL_RATE;
	}
	inline int getNEUTRAL_HEAL_RATE()
	{
		return m_iNEUTRAL_HEAL_RATE;
	}
	inline int getFRIENDLY_HEAL_RATE()
	{
		return m_iFRIENDLY_HEAL_RATE;
	}
	inline int getINSTA_HEAL_RATE()
	{
		return m_iINSTA_HEAL_RATE;
	}
	inline int getCITY_HEAL_RATE()
	{
		return m_iCITY_HEAL_RATE;
	}
	inline int getATTACK_SAME_STRENGTH_MIN_DAMAGE()
	{
		return m_iATTACK_SAME_STRENGTH_MIN_DAMAGE;
	}
	inline int getRANGE_ATTACK_RANGED_DEFENDER_MOD()
	{
		return m_iRANGE_ATTACK_RANGED_DEFENDER_MOD;
	}
	inline int getATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE()
	{
		return m_iATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE;
	}
	inline int getRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE()
	{
		return m_iRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE;
	}
	inline int getRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE()
	{
		return m_iRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE;
	}
	inline int getAIR_STRIKE_SAME_STRENGTH_MIN_DEFENSE_DAMAGE()
	{
		return m_iAIR_STRIKE_SAME_STRENGTH_MIN_DEFENSE_DAMAGE;
	}
	inline int getAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE()
	{
		return m_iAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE;
	}
	inline int getINTERCEPTION_SAME_STRENGTH_MIN_DAMAGE()
	{
		return m_iINTERCEPTION_SAME_STRENGTH_MIN_DAMAGE;
	}
	inline int getINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE()
	{
		return m_iINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE;
	}
	inline int getAIR_SWEEP_INTERCEPTION_DAMAGE_MOD()
	{
		return m_iAIR_SWEEP_INTERCEPTION_DAMAGE_MOD;
	}
	inline int getWOUNDED_DAMAGE_MULTIPLIER()
	{
		return m_iWOUNDED_DAMAGE_MULTIPLIER;
	}
	inline int getTRAIT_WOUNDED_DAMAGE_MOD()
	{
		return m_iTRAIT_WOUNDED_DAMAGE_MOD;
	}
	inline int getCITY_STRENGTH_DEFAULT()
	{
		return m_iCITY_STRENGTH_DEFAULT;
	}
	inline int getCITY_STRENGTH_POPULATION_CHANGE()
	{
		return m_iCITY_STRENGTH_POPULATION_CHANGE;
	}
	inline int getCITY_STRENGTH_UNIT_DIVISOR()
	{
		return m_iCITY_STRENGTH_UNIT_DIVISOR;
	}
	inline int getCITY_STRENGTH_HILL_CHANGE()
	{
		return m_iCITY_STRENGTH_HILL_CHANGE;
	}
	inline int getCITY_ATTACKING_DAMAGE_MOD()
	{
		return m_iCITY_ATTACKING_DAMAGE_MOD;
	}
	inline int getATTACKING_CITY_MELEE_DAMAGE_MOD()
	{
		return m_iATTACKING_CITY_MELEE_DAMAGE_MOD;
	}
	inline int getCITY_ATTACK_RANGE()
	{
		return m_iCITY_ATTACK_RANGE;
	}
	inline int getCAN_CITY_USE_INDIRECT_FIRE()
	{
		return m_iCAN_CITY_USE_INDIRECT_FIRE;
	}
	inline int getCITY_RANGED_ATTACK_STRENGTH_MULTIPLIER()
	{
		return m_iCITY_RANGED_ATTACK_STRENGTH_MULTIPLIER;
	}
	inline int getMIN_CITY_STRIKE_DAMAGE()
	{
		return m_iMIN_CITY_STRIKE_DAMAGE;
	}
	inline int getCITY_CAPTURE_DAMAGE_PERCENT()
	{
		return m_iCITY_CAPTURE_DAMAGE_PERCENT;
	}
	inline int getEXPERIENCE_PER_LEVEL()
	{
		return m_iEXPERIENCE_PER_LEVEL;
	}
	inline int getEXPERIENCE_ATTACKING_UNIT_MELEE()
	{
		return m_iEXPERIENCE_ATTACKING_UNIT_MELEE;
	}
	inline int getEXPERIENCE_DEFENDING_UNIT_MELEE()
	{
		return m_iEXPERIENCE_DEFENDING_UNIT_MELEE;
	}
	inline int getEXPERIENCE_ATTACKING_UNIT_AIR()
	{
		return m_iEXPERIENCE_ATTACKING_UNIT_AIR;
	}
	inline int getEXPERIENCE_DEFENDING_UNIT_AIR()
	{
		return m_iEXPERIENCE_DEFENDING_UNIT_AIR;
	}
	inline int getEXPERIENCE_ATTACKING_UNIT_RANGED()
	{
		return m_iEXPERIENCE_ATTACKING_UNIT_RANGED;
	}
	inline int getEXPERIENCE_DEFENDING_UNIT_RANGED()
	{
		return m_iEXPERIENCE_DEFENDING_UNIT_RANGED;
	}
	inline int getEXPERIENCE_ATTACKING_AIR_SWEEP()
	{
		return m_iEXPERIENCE_ATTACKING_AIR_SWEEP;
	}
	inline int getEXPERIENCE_DEFENDING_AIR_SWEEP_AIR()
	{
		return m_iEXPERIENCE_DEFENDING_AIR_SWEEP_AIR;
	}
	inline int getEXPERIENCE_DEFENDING_AIR_SWEEP_GROUND()
	{
		return m_iEXPERIENCE_DEFENDING_AIR_SWEEP_GROUND;
	}
	inline int getEXPERIENCE_ATTACKING_CITY_MELEE()
	{
		return m_iEXPERIENCE_ATTACKING_CITY_MELEE;
	}
	inline int getEXPERIENCE_ATTACKING_CITY_RANGED()
	{
		return m_iEXPERIENCE_ATTACKING_CITY_RANGED;
	}
	inline int getEXPERIENCE_ATTACKING_CITY_AIR()
	{
		return m_iEXPERIENCE_ATTACKING_CITY_AIR;
	}
	inline int getBARBARIAN_MAX_XP_VALUE()
	{
		return m_iBARBARIAN_MAX_XP_VALUE;
	}
	inline int getCOMBAT_EXPERIENCE_IN_BORDERS_PERCENT()
	{
		return m_iCOMBAT_EXPERIENCE_IN_BORDERS_PERCENT;
	}
	inline int getGREAT_GENERALS_THRESHOLD_INCREASE()
	{
		return m_iGREAT_GENERALS_THRESHOLD_INCREASE;
	}
	inline int getGREAT_GENERALS_THRESHOLD_INCREASE_TEAM()
	{
		return m_iGREAT_GENERALS_THRESHOLD_INCREASE_TEAM;
	}
	inline int getGREAT_GENERALS_THRESHOLD()
	{
		return m_iGREAT_GENERALS_THRESHOLD;
	}
	inline int getUNIT_DEATH_XP_GREAT_GENERAL_LOSS()
	{
		return m_iUNIT_DEATH_XP_GREAT_GENERAL_LOSS;
	}
	inline int getMIN_EXPERIENCE_PER_COMBAT()
	{
		return m_iMIN_EXPERIENCE_PER_COMBAT;
	}
	inline int getMAX_EXPERIENCE_PER_COMBAT()
	{
		return m_iMAX_EXPERIENCE_PER_COMBAT;
	}
	inline int getCRAMPED_RANGE_FROM_CITY()
	{
		return m_iCRAMPED_RANGE_FROM_CITY;
	}
	inline int getCRAMPED_USABLE_PLOT_PERCENT()
	{
		return m_iCRAMPED_USABLE_PLOT_PERCENT;
	}
	inline int getPROXIMITY_NEIGHBORS_CLOSEST_CITY_REQUIREMENT()
	{
		return m_iPROXIMITY_NEIGHBORS_CLOSEST_CITY_REQUIREMENT;
	}
	inline int getPROXIMITY_CLOSE_CLOSEST_CITY_POSSIBILITY()
	{
		return m_iPROXIMITY_CLOSE_CLOSEST_CITY_POSSIBILITY;
	}
	inline int getPROXIMITY_CLOSE_DISTANCE_MAP_MULTIPLIER()
	{
		return m_iPROXIMITY_CLOSE_DISTANCE_MAP_MULTIPLIER;
	}
	inline int getPROXIMITY_CLOSE_DISTANCE_MAX()
	{
		return m_iPROXIMITY_CLOSE_DISTANCE_MAX;
	}
	inline int getPROXIMITY_CLOSE_DISTANCE_MIN()
	{
		return m_iPROXIMITY_CLOSE_DISTANCE_MIN;
	}
	inline int getPROXIMITY_FAR_DISTANCE_MAP_MULTIPLIER()
	{
		return m_iPROXIMITY_FAR_DISTANCE_MAP_MULTIPLIER;
	}
	inline int getPROXIMITY_FAR_DISTANCE_MAX()
	{
		return m_iPROXIMITY_FAR_DISTANCE_MAX;
	}
	inline int getPROXIMITY_FAR_DISTANCE_MIN()
	{
		return m_iPROXIMITY_FAR_DISTANCE_MIN;
	}
	inline int getPLOT_BASE_COST()
	{
		return m_iPLOT_BASE_COST;
	}
	inline int getPLOT_ADDITIONAL_COST_PER_PLOT()
	{
		return m_iPLOT_ADDITIONAL_COST_PER_PLOT;
	}
	inline int getPLOT_COST_APPEARANCE_DIVISOR()
	{
		return m_iPLOT_COST_APPEARANCE_DIVISOR;
	}
	inline int getCULTURE_COST_FIRST_PLOT()
	{
		return m_iCULTURE_COST_FIRST_PLOT;
	}
	inline int getCULTURE_COST_LATER_PLOT_MULTIPLIER()
	{
		return m_iCULTURE_COST_LATER_PLOT_MULTIPLIER;
	}
	inline int getCULTURE_COST_VISIBLE_DIVISOR()
	{
		return m_iCULTURE_COST_VISIBLE_DIVISOR;
	}
	inline int getCULTURE_PLOT_COST_MOD_MINIMUM()
	{
		return m_iCULTURE_PLOT_COST_MOD_MINIMUM;
	}
	inline int getMINOR_CIV_PLOT_CULTURE_COST_MULTIPLIER()
	{
		return m_iMINOR_CIV_PLOT_CULTURE_COST_MULTIPLIER;
	}
	inline int getMAXIMUM_BUY_PLOT_DISTANCE()
	{
		return m_iMAXIMUM_BUY_PLOT_DISTANCE;
	}
	inline int getMAXIMUM_ACQUIRE_PLOT_DISTANCE()
	{
		return m_iMAXIMUM_ACQUIRE_PLOT_DISTANCE;
	}
	inline int getPLOT_INFLUENCE_BASE_MULTIPLIER()
	{
		return m_iPLOT_INFLUENCE_BASE_MULTIPLIER;
	}
	inline int getPLOT_INFLUENCE_DISTANCE_MULTIPLIER()
	{
		return m_iPLOT_INFLUENCE_DISTANCE_MULTIPLIER;
	}
	inline int getPLOT_INFLUENCE_DISTANCE_DIVISOR()
	{
		return m_iPLOT_INFLUENCE_DISTANCE_DIVISOR;
	}
	inline int getPLOT_INFLUENCE_RING_COST()
	{
		return m_iPLOT_INFLUENCE_RING_COST;
	}
	inline int getPLOT_INFLUENCE_WATER_COST()
	{
		return m_iPLOT_INFLUENCE_WATER_COST;
	}
	inline int getPLOT_INFLUENCE_IMPROVEMENT_COST()
	{
		return m_iPLOT_INFLUENCE_IMPROVEMENT_COST;
	}
	inline int getPLOT_INFLUENCE_ROUTE_COST()
	{
		return m_iPLOT_INFLUENCE_ROUTE_COST;
	}
	inline int getPLOT_INFLUENCE_RESOURCE_COST()
	{
		return m_iPLOT_INFLUENCE_RESOURCE_COST;
	}
	inline int getPLOT_INFLUENCE_NW_COST()
	{
		return m_iPLOT_INFLUENCE_NW_COST;
	}
	inline int getPLOT_BUY_RESOURCE_COST()
	{
		return m_iPLOT_BUY_RESOURCE_COST;
	}
	inline int getPLOT_BUY_YIELD_COST()
	{
		return m_iPLOT_BUY_YIELD_COST;
	}
	inline int getPLOT_INFLUENCE_YIELD_POINT_COST()
	{
		return m_iPLOT_INFLUENCE_YIELD_POINT_COST;
	}
	inline int getPLOT_INFLUENCE_NO_ADJACENT_OWNED_COST()
	{
		return m_iPLOT_INFLUENCE_NO_ADJACENT_OWNED_COST;
	}
	inline int getUNITED_NATIONS_COUNTDOWN_TURNS()
	{
		return m_iUNITED_NATIONS_COUNTDOWN_TURNS;
	}
	inline int getOWN_UNITED_NATIONS_VOTE_BONUS()
	{
		return m_iOWN_UNITED_NATIONS_VOTE_BONUS;
	}
	inline int getDIPLO_VICTORY_ALGORITHM_THRESHOLD()
	{
		return m_iDIPLO_VICTORY_ALGORITHM_THRESHOLD;
	}
	inline int getDIPLO_VICTORY_BEYOND_ALGORITHM_MULTIPLIER()
	{
		return m_iDIPLO_VICTORY_BEYOND_ALGORITHM_MULTIPLIER;
	}
	inline int getDIPLO_VICTORY_DEFAULT_VOTE_PERCENT()
	{
		return m_iDIPLO_VICTORY_DEFAULT_VOTE_PERCENT;
	}
	inline int getSCORE_CITY_MULTIPLIER()
	{
		return m_iSCORE_CITY_MULTIPLIER;
	}
	inline int getSCORE_POPULATION_MULTIPLIER()
	{
		return m_iSCORE_POPULATION_MULTIPLIER;
	}
	inline int getSCORE_LAND_MULTIPLIER()
	{
		return m_iSCORE_LAND_MULTIPLIER;
	}
	inline int getSCORE_WONDER_MULTIPLIER()
	{
		return m_iSCORE_WONDER_MULTIPLIER;
	}
	inline int getSCORE_TECH_MULTIPLIER()
	{
		return m_iSCORE_TECH_MULTIPLIER;
	}
	inline int getSCORE_FUTURE_TECH_MULTIPLIER()
	{
		return m_iSCORE_FUTURE_TECH_MULTIPLIER;
	}
	inline int getSCORE_POLICY_MULTIPLIER()
	{
		return m_iSCORE_POLICY_MULTIPLIER;
	}
	inline int getSCORE_GREAT_WORK_MULTIPLIER()
	{
		return m_iSCORE_GREAT_WORK_MULTIPLIER;
	}
	inline int getSCORE_BELIEF_MULTIPLIER()
	{
		return m_iSCORE_BELIEF_MULTIPLIER;
	}
	inline int getSCORE_RELIGION_CITIES_MULTIPLIER()
	{
		return m_iSCORE_RELIGION_CITIES_MULTIPLIER;
	}
	inline int getVICTORY_POINTS_PER_ERA()
	{
		return m_iVICTORY_POINTS_PER_ERA;
	}
	inline int getMIN_GAME_TURNS_ELAPSED_TO_TEST_VICTORY()
	{
		return m_iMIN_GAME_TURNS_ELAPSED_TO_TEST_VICTORY;
	}
	inline int getZERO_SUM_COMPETITION_WONDERS_VICTORY_POINTS()
	{
		return m_iZERO_SUM_COMPETITION_WONDERS_VICTORY_POINTS;
	}
	inline int getZERO_SUM_COMPETITION_POLICIES_VICTORY_POINTS()
	{
		return m_iZERO_SUM_COMPETITION_POLICIES_VICTORY_POINTS;
	}
	inline int getZERO_SUM_COMPETITION_GREAT_PEOPLE_VICTORY_POINTS()
	{
		return m_iZERO_SUM_COMPETITION_GREAT_PEOPLE_VICTORY_POINTS;
	}
	inline int getMAX_CITY_DIST_HIGHWATER_MARK()
	{
		return m_iMAX_CITY_DIST_HIGHWATER_MARK;
	}
	inline int getHEAVY_RESOURCE_THRESHOLD()
	{
		return m_iHEAVY_RESOURCE_THRESHOLD;
	}
	inline int getPROGRESS_POPUP_TURN_FREQUENCY()
	{
		return m_iPROGRESS_POPUP_TURN_FREQUENCY;
	}
	inline int getSETTLER_PRODUCTION_SPEED()
	{
		return m_iSETTLER_PRODUCTION_SPEED;
	}
	inline int getBUY_PLOTS_DISABLED()
	{
		return m_iBUY_PLOTS_DISABLED;
	}
	inline int getCITY_STRENGTH_TECH_MULTIPLIER()
	{
		return m_iCITY_STRENGTH_TECH_MULTIPLIER;
	}
	inline int getWARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT()
	{
		return m_iWARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT;
	}
	inline int getBUILDING_SALE_DIVISOR()
	{
		return m_iBUILDING_SALE_DIVISOR;
	}
	inline int getDISBAND_UNIT_REFUND_PERCENT()
	{
		return m_iDISBAND_UNIT_REFUND_PERCENT;
	}
	inline int getMINOR_CIV_ROUTE_QUEST_WEIGHT()
	{
		return m_iMINOR_CIV_ROUTE_QUEST_WEIGHT;
	}
	inline int getWITHDRAW_MOD_ENEMY_MOVES()
	{
		return m_iWITHDRAW_MOD_ENEMY_MOVES;
	}
	inline int getWITHDRAW_MOD_BLOCKED_TILE()
	{
		return m_iWITHDRAW_MOD_BLOCKED_TILE;
	}
	inline int getAI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT()
	{
		return m_iAI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT;
	}
	inline int getAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY()
	{
		return m_iAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY;
	}
	inline int getWALLS_BUILDINGCLASS() const
	{
		return m_iWALLS_BUILDINGCLASS;
	}

	// -- floats --

	inline float getDIPLO_VICTORY_CIV_DELEGATES_COEFFICIENT()
	{
		return m_fDIPLO_VICTORY_CIV_DELEGATES_COEFFICIENT;
	}
	inline float getDIPLO_VICTORY_CIV_DELEGATES_CONSTANT()
	{
		return m_fDIPLO_VICTORY_CIV_DELEGATES_CONSTANT;
	}
	inline float getDIPLO_VICTORY_CS_DELEGATES_COEFFICIENT()
	{
		return m_fDIPLO_VICTORY_CS_DELEGATES_COEFFICIENT;
	}
	inline float getDIPLO_VICTORY_CS_DELEGATES_CONSTANT()
	{
		return m_fDIPLO_VICTORY_CS_DELEGATES_CONSTANT;
	}
	inline float getAI_PRODUCTION_WEIGHT_MOD_PER_TURN_LEFT()
	{
		return m_fAI_PRODUCTION_WEIGHT_MOD_PER_TURN_LEFT;
	}
	inline float getAI_PRODUCTION_WEIGHT_BASE_MOD()
	{
		return m_fAI_PRODUCTION_WEIGHT_BASE_MOD;
	}
	inline float getAI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT()
	{
		return m_fAI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT;
	}
	inline float getAI_RESEARCH_WEIGHT_BASE_MOD()
	{
		return m_fAI_RESEARCH_WEIGHT_BASE_MOD;
	}
	inline float getHURRY_GOLD_PRODUCTION_EXPONENT()
	{
		return m_fHURRY_GOLD_PRODUCTION_EXPONENT;
	}
	inline float getCITY_ZOOM_LEVEL_1()
	{
		return m_fCITY_ZOOM_LEVEL_1;
	}
	inline float getCITY_ZOOM_LEVEL_2()
	{
		return m_fCITY_ZOOM_LEVEL_2;
	}
	inline float getCITY_ZOOM_LEVEL_3()
	{
		return m_fCITY_ZOOM_LEVEL_3;
	}
	inline float getCITY_ZOOM_OFFSET()
	{
		return m_fCITY_ZOOM_OFFSET;
	}
	inline float getCULTURE_COST_LATER_PLOT_EXPONENT()
	{
		return m_fCULTURE_COST_LATER_PLOT_EXPONENT;
	}
	inline float getPOST_COMBAT_TEXT_DELAY()
	{
		return m_fPOST_COMBAT_TEXT_DELAY;
	}
	inline float getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEXT_WONDER()
	{
		return m_fAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEXT_WONDER;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_WAR()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_WAR;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_HOSTILE()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_HOSTILE;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_DECEPTIVE()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_DECEPTIVE;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_GUARDED()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_GUARDED;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_AFRAID()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_AFRAID;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_FRIENDLY()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_FRIENDLY;
	}
	inline float getAI_DANGER_MAJOR_APPROACH_NEUTRAL()
	{
		return m_fAI_DANGER_MAJOR_APPROACH_NEUTRAL;
	}
	inline float getAI_DANGER_MINOR_APPROACH_NEUTRAL()
	{
		return m_fAI_DANGER_MINOR_APPROACH_NEUTRAL;
	}
	inline float getAI_DANGER_MINOR_APPROACH_FRIENDLY()
	{
		return m_fAI_DANGER_MINOR_APPROACH_FRIENDLY;
	}
	inline float getAI_DANGER_MINOR_APPROACH_BULLY()
	{
		return m_fAI_DANGER_MINOR_APPROACH_BULLY;
	}
	inline float getAI_DANGER_MINOR_APPROACH_CONQUEST()
	{
		return m_fAI_DANGER_MINOR_APPROACH_CONQUEST;
	}
	inline float getFLAVOR_STANDARD_LOG10_TILES_PER_PLAYER()
	{
		return m_fFLAVOR_STANDARD_LOG10_TILES_PER_PLAYER;
	}
	inline float getDIPLO_VICTORY_TEAM_MULTIPLIER()
	{
		return m_fDIPLO_VICTORY_TEAM_MULTIPLIER;
	}
	inline float getAI_TACTICAL_FLAVOR_DAMPENING_FOR_MOVE_PRIORITIZATION()
	{
		return m_fAI_TACTICAL_FLAVOR_DAMPENING_FOR_MOVE_PRIORITIZATION;
	}
	inline float getAI_STRATEGY_DEFEND_MY_LANDS_UNITS_PER_CITY()
	{
		return m_fAI_STRATEGY_DEFEND_MY_LANDS_UNITS_PER_CITY;
	}
	inline float getAI_STRATEGY_NAVAL_UNITS_PER_CITY()
	{
		return m_fAI_STRATEGY_NAVAL_UNITS_PER_CITY;
	}
	inline float getGOLD_GIFT_FRIENDSHIP_EXPONENT()
	{
		return m_fGOLD_GIFT_FRIENDSHIP_EXPONENT;
	}
	inline float getGOLD_GIFT_FRIENDSHIP_DIVISOR()
	{
		return m_fGOLD_GIFT_FRIENDSHIP_DIVISOR;
	}
	inline float getHURRY_GOLD_TECH_EXPONENT()
	{
		return m_fHURRY_GOLD_TECH_EXPONENT;
	}
	inline float getHURRY_GOLD_CULTURE_EXPONENT()
	{
		return m_fHURRY_GOLD_CULTURE_EXPONENT;
	}
	inline float getCITY_GROWTH_MULTIPLIER()
	{
		return m_fCITY_GROWTH_MULTIPLIER;
	}
	inline float getCITY_GROWTH_EXPONENT()
	{
		return m_fCITY_GROWTH_EXPONENT;
	}
	inline float getPOLICY_COST_EXPONENT()
	{
		return m_fPOLICY_COST_EXPONENT;
	}
	inline float getUNIT_UPGRADE_COST_MULTIPLIER_PER_ERA()
	{
		return m_fUNIT_UPGRADE_COST_MULTIPLIER_PER_ERA;
	}
	inline float getUNIT_UPGRADE_COST_EXPONENT()
	{
		return m_fUNIT_UPGRADE_COST_EXPONENT;
	}
	inline float getCITY_STRENGTH_TECH_BASE()
	{
		return m_fCITY_STRENGTH_TECH_BASE;
	}
	inline float getCITY_STRENGTH_TECH_EXPONENT()
	{
		return m_fCITY_STRENGTH_TECH_EXPONENT;
	}

	// post defines

	inline int getLAND_TERRAIN()
	{
		return m_iLAND_TERRAIN;
	}
	inline int getDEEP_WATER_TERRAIN()
	{
		return m_iDEEP_WATER_TERRAIN;
	}
	inline int getSHALLOW_WATER_TERRAIN()
	{
		return m_iSHALLOW_WATER_TERRAIN;
	}
	inline int getRUINS_IMPROVEMENT()
	{
		return m_iRUINS_IMPROVEMENT;
	}
	inline int getNUKE_FEATURE()
	{
		return m_iNUKE_FEATURE;
	}
	inline int getARTIFACT_RESOURCE()
	{
		return m_iARTIFACT_RESOURCE;
	}
	inline int getHIDDEN_ARTIFACT_RESOURCE()
	{
		return m_iHIDDEN_ARTIFACT_RESOURCE;
	}
	inline int getCAPITAL_BUILDINGCLASS()
	{
		return m_iCAPITAL_BUILDINGCLASS;
	}
	inline int getDEFAULT_SPECIALIST()
	{
		return m_iDEFAULT_SPECIALIST;
	}
	inline int getSPACE_RACE_TRIGGER_PROJECT()
	{
		return m_iSPACE_RACE_TRIGGER_PROJECT;
	}
	inline int getSPACESHIP_CAPSULE()
	{
		return m_iSPACESHIP_CAPSULE;
	}
	inline int getSPACESHIP_BOOSTER()
	{
		return m_iSPACESHIP_BOOSTER;
	}
	inline int getSPACESHIP_STASIS()
	{
		return m_iSPACESHIP_STASIS;
	}
	inline int getSPACESHIP_ENGINE()
	{
		return m_iSPACESHIP_ENGINE;
	}
	inline int getMANHATTAN_PROJECT()
	{
		return m_iMANHATTAN_PROJECT;
	}
	inline int getINITIAL_CITY_ROUTE_TYPE()
	{
		return m_iINITIAL_CITY_ROUTE_TYPE;
	}
	inline int getSTANDARD_HANDICAP()
	{
		return m_iSTANDARD_HANDICAP;
	}
	inline int getMULTIPLAYER_HANDICAP()
	{
		return m_iMULTIPLAYER_HANDICAP;
	}
	inline int getSTANDARD_HANDICAP_QUICK()
	{
		return m_iSTANDARD_HANDICAP_QUICK;
	}
	inline int getSTANDARD_GAMESPEED()
	{
		return m_iSTANDARD_GAMESPEED;
	}
	inline int getSTANDARD_TURNTIMER()
	{
		return m_iSTANDARD_TURNTIMER;
	}
	inline int getSTANDARD_CLIMATE()
	{
		return m_iSTANDARD_CLIMATE;
	}
	inline int getSTANDARD_WORLD_SIZE()
	{
		return m_iSTANDARD_WORLD_SIZE;
	}
	inline int getSTANDARD_SEALEVEL()
	{
		return m_iSTANDARD_SEALEVEL;
	}
	inline int getSTANDARD_ERA()
	{
		return m_iSTANDARD_ERA;
	}
	inline int getLAST_EMBARK_ART_ERA()
	{
		return m_iLAST_EMBARK_ART_ERA;
	}
	inline int getLAST_UNIT_ART_ERA()
	{
		return m_iLAST_UNIT_ART_ERA;
	}
	inline int getLAST_BRIDGE_ART_ERA()
	{
		return m_iLAST_BRIDGE_ART_ERA;
	}
	inline int getSTANDARD_CALENDAR()
	{
		return m_iSTANDARD_CALENDAR;
	}
	inline int getBARBARIAN_HANDICAP()
	{
		return m_iBARBARIAN_HANDICAP;
	}
	inline int getBARBARIAN_CIVILIZATION()
	{
		return m_iBARBARIAN_CIVILIZATION;
	}
	inline int getBARBARIAN_LEADER()
	{
		return m_iBARBARIAN_LEADER;
	}
	inline int getMINOR_CIV_HANDICAP()
	{
		return m_iMINOR_CIV_HANDICAP;
	}
	inline int getMINOR_CIVILIZATION()
	{
		return m_iMINOR_CIVILIZATION;
	}
	inline int getPROMOTION_EMBARKATION()
	{
		return m_iPROMOTION_EMBARKATION;
	}
	inline int getPROMOTION_DEFENSIVE_EMBARKATION()
	{
		return m_iPROMOTION_DEFENSIVE_EMBARKATION;
	}
	inline int getPROMOTION_ALLWATER_EMBARKATION()
	{
		return m_iPROMOTION_ALLWATER_EMBARKATION;
	}
	inline int getPROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY()
	{
		return m_iPROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY;
	}
	inline int getPROMOTION_OCEAN_IMPASSABLE()
	{
		return m_iPROMOTION_OCEAN_IMPASSABLE;
	}
	inline int getPROMOTION_ONLY_DEFENSIVE()
	{
		return m_iPROMOTION_ONLY_DEFENSIVE;
	}
	inline int getPROMOTION_UNWELCOME_EVANGELIST()
	{
		return m_iPROMOTION_UNWELCOME_EVANGELIST;
	}
	inline int getCOMBAT_CAPTURE_HEALTH()
	{
		return m_iCOMBAT_CAPTURE_HEALTH;
	}
	inline int getCOMBAT_CAPTURE_MIN_CHANCE()
	{
		return m_iCOMBAT_CAPTURE_MIN_CHANCE;
	}
	inline int getCOMBAT_CAPTURE_MAX_CHANCE()
	{
		return m_iCOMBAT_CAPTURE_MAX_CHANCE;
	}
	inline int getCOMBAT_CAPTURE_RATIO_MULTIPLIER()
	{
		return m_iCOMBAT_CAPTURE_RATIO_MULTIPLIER;
	}
	inline int getAI_HANDICAP()
	{
		return m_iAI_HANDICAP;
	}
	inline int getBARBARIAN_CAMP_IMPROVEMENT()
	{
		return m_iBARBARIAN_CAMP_IMPROVEMENT;
	}
	inline int getRELIGION_MIN_FAITH_FIRST_PANTHEON()
	{
		return m_iRELIGION_MIN_FAITH_FIRST_PANTHEON;
	}
	inline int getRELIGION_MIN_FAITH_FIRST_PROPHET()
	{
		return m_iRELIGION_MIN_FAITH_FIRST_PROPHET;
	}
	inline int getRELIGION_MIN_FAITH_FIRST_GREAT_PERSON()
	{
		return m_iRELIGION_MIN_FAITH_FIRST_GREAT_PERSON;
	}
	inline int getRELIGION_GAME_FAITH_DELTA_NEXT_PANTHEON()
	{
		return m_iRELIGION_GAME_FAITH_DELTA_NEXT_PANTHEON;
	}
	inline int getRELIGION_FAITH_DELTA_NEXT_PROPHET()
	{
		return m_iRELIGION_FAITH_DELTA_NEXT_PROPHET;
	}
	inline int getRELIGION_FAITH_DELTA_NEXT_GREAT_PERSON()
	{
		return m_iRELIGION_FAITH_DELTA_NEXT_GREAT_PERSON;
	}
	inline int getRELIGION_BASE_CHANCE_PROPHET_SPAWN()
	{
		return m_iRELIGION_BASE_CHANCE_PROPHET_SPAWN;
	}
	inline int getRELIGION_ATHEISM_PRESSURE_PER_POP()
	{
		return m_iRELIGION_ATHEISM_PRESSURE_PER_POP;
	}
	inline int getRELIGION_INITIAL_FOUNDING_CITY_PRESSURE()
	{
		return m_iRELIGION_INITIAL_FOUNDING_CITY_PRESSURE;
	}
	inline int getRELIGION_PER_TURN_FOUNDING_CITY_PRESSURE()
	{
		return m_iRELIGION_PER_TURN_FOUNDING_CITY_PRESSURE;
	}
	inline int getRELIGION_MISSIONARY_PRESSURE_MULTIPLIER()
	{
		return m_iRELIGION_MISSIONARY_PRESSURE_MULTIPLIER;
	}
	inline int getRELIGION_ADJACENT_CITY_DISTANCE()
	{
		return m_iRELIGION_ADJACENT_CITY_DISTANCE;
	}
	inline int getRELIGION_DIPLO_HIT_INITIAL_CONVERT_FRIENDLY_CITY()
	{
		return m_iRELIGION_DIPLO_HIT_INITIAL_CONVERT_FRIENDLY_CITY;
	}
	inline int getRELIGION_DIPLO_HIT_RELIGIOUS_FLIP_FRIENDLY_CITY()
	{
		return m_iRELIGION_DIPLO_HIT_RELIGIOUS_FLIP_FRIENDLY_CITY;
	}
	inline int getRELIGION_DIPLO_HIT_CONVERT_HOLY_CITY()
	{
		return m_iRELIGION_DIPLO_HIT_CONVERT_HOLY_CITY;
	}
	inline int getRELIGION_DIPLO_HIT_THRESHOLD()
	{
		return m_iRELIGION_DIPLO_HIT_THRESHOLD;
	}
	inline int getESPIONAGE_GATHERING_INTEL_COST_PERCENT()
	{
		return m_iESPIONAGE_GATHERING_INTEL_COST_PERCENT;
	}
	inline int getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT()
	{
		return m_iESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT;
	}
	inline int getESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT()
	{
		return m_iESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT;
	}
	inline int getESPIONAGE_TURNS_BETWEEN_CITY_STATE_ELECTIONS()
	{
		return m_iESPIONAGE_TURNS_BETWEEN_CITY_STATE_ELECTIONS;
	}
	inline int getESPIONAGE_INFLUENCE_GAINED_FOR_RIGGED_ELECTION()
	{
		return m_iESPIONAGE_INFLUENCE_GAINED_FOR_RIGGED_ELECTION;
	}
	inline int getESPIONAGE_INFLUENCE_LOST_FOR_RIGGED_ELECTION()
	{
		return m_iESPIONAGE_INFLUENCE_LOST_FOR_RIGGED_ELECTION;
	}
	inline int getESPIONAGE_SURVEILLANCE_SIGHT_RANGE()
	{
		return m_iESPIONAGE_SURVEILLANCE_SIGHT_RANGE;
	}
	inline int getESPIONAGE_COUP_OTHER_PLAYERS_INFLUENCE_DROP()
	{
		return m_iESPIONAGE_COUP_OTHER_PLAYERS_INFLUENCE_DROP;
	}
	inline float getESPIONAGE_COUP_NOBODY_BONUS()
	{
		return m_fESPIONAGE_COUP_NOBODY_BONUS;
	}
	inline float getESPIONAGE_COUP_MULTIPLY_CONSTANT()
	{
		return m_fESPIONAGE_COUP_MULTIPLY_CONSTANT;
	}
	inline float getESPIONAGE_COUP_SPY_LEVEL_DELTA_ZERO()
	{
		return m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_ZERO;
	}
	inline float getESPIONAGE_COUP_SPY_LEVEL_DELTA_ONE()
	{
		return m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_ONE;
	}
	inline float getESPIONAGE_COUP_SPY_LEVEL_DELTA_TWO()
	{
		return m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_TWO;
	}
	inline float getESPIONAGE_COUP_SPY_LEVEL_DELTA_THREE()
	{
		return m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_THREE;
	}
	inline float getESPIONAGE_COUP_SPY_LEVEL_DELTA_FOUR()
	{
		return m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_FOUR;
	}
	inline int getINTERNATIONAL_TRADE_BASE()
	{
		return m_iINTERNATIONAL_TRADE_BASE;
	}
	inline int getINTERNATIONAL_TRADE_EXCLUSIVE_CONNECTION()
	{
		return m_iINTERNATIONAL_TRADE_EXCLUSIVE_CONNECTION;
	}
	inline int getINTERNATIONAL_TRADE_CITY_GPT_DIVISOR()
	{
		return m_iINTERNATIONAL_TRADE_CITY_GPT_DIVISOR;
	}
	inline int getPILLAGE_HEAL_AMOUNT()
	{
		return m_iPILLAGE_HEAL_AMOUNT;
	}
	inline int getCITY_CONNECTIONS_CONNECT_TO_CAPITAL()
	{
		return m_iCITY_CONNECTIONS_CONNECT_TO_CAPITAL;
	}
	inline float getUNRESEARCHED_TECH_BONUS_FROM_KILLS_SLOPE()
	{
		return m_fUNRESEARCHED_TECH_FROM_KILLS_SLOPE;
	}
	inline float getUNRESEARCHED_TECH_BONUS_FROM_KILLS_INTERCEPT()
	{
		return m_fUNRESEARCHED_TECH_FROM_KILLS_INTERCEPT;
	}
	inline int getBASE_CULTURE_PER_GREAT_WORK()
	{
		return m_iBASE_CULTURE_PER_GREAT_WORK;
	}
	inline int getBASE_TOURISM_PER_GREAT_WORK()
	{
		return m_iBASE_TOURISM_PER_GREAT_WORK;
	}
	inline int getTOURISM_MODIFIER_SHARED_RELIGION()
	{
		return m_iTOURISM_MODIFIER_SHARED_RELIGION;
	}
	inline int getTOURISM_MODIFIER_TRADE_ROUTE()
	{
		return m_iTOURISM_MODIFIER_TRADE_ROUTE;
	}
	inline int getTOURISM_MODIFIER_OPEN_BORDERS()
	{
		return m_iTOURISM_MODIFIER_OPEN_BORDERS;
	}
	inline int getTOURISM_MODIFIER_DIFFERENT_IDEOLOGIES()
	{
		return m_iTOURISM_MODIFIER_DIFFERENT_IDEOLOGIES;
	}
	inline int getTOURISM_MODIFIER_DIPLOMAT()
	{
		return m_iTOURISM_MODIFIER_DIPLOMAT;
	}
	inline int getMINIUMUM_TOURISM_BLAST_STRENGTH()
	{
		return m_iMINIUMUM_TOURISM_BLAST_STRENGTH;
	}
	inline int getCULTURE_LEVEL_EXOTIC()
	{
		return m_iCULTURE_LEVEL_EXOTIC;
	}
	inline int getCULTURE_LEVEL_FAMILIAR()
	{
		return m_iCULTURE_LEVEL_FAMILIAR;
	}
	inline int getCULTURE_LEVEL_POPULAR()
	{
		return m_iCULTURE_LEVEL_POPULAR;
	}
	inline int getCULTURE_LEVEL_INFLUENTIAL()
	{
		return m_iCULTURE_LEVEL_INFLUENTIAL;
	}
	inline int getCULTURE_LEVEL_DOMINANT()
	{
		return m_iCULTURE_LEVEL_DOMINANT;
	}
#ifdef LEKMOD_TOURISM_COMBAT_MOD
	inline int getLekmodTourismCombatMinInfluencePercent()
	{
		return m_iLekmodTourismCombatMinInfluencePercent;
	}
	inline int getLekmodTourismCombatMaxInfluencePercent()
	{
		return m_iLekmodTourismCombatMaxInfluencePercent;
	}
	inline int getLekmodTourismCombatMinBonus()
	{
		return m_iLekmodTourismCombatMinBonus;
	}
	inline int getLekmodTourismCombatMaxBonus()
	{
		return m_iLekmodTourismCombatMaxBonus;
	}
	inline bool isLekmodTourismCombatIdeologyRequired()
	{
		return m_bLekmodTourismCombatIdeologyRequired;
	}
#endif
	inline int getPROMOTION_GOODY_HUT_PICKER()
	{
		return m_iPROMOTION_GOODY_HUT_PICKER;
	}
	inline int getIDEOLOGY_PERCENT_CLEAR_VICTORY_PREF()
	{
		return m_iIDEOLOGY_PERCENT_CLEAR_VICTORY_PREF;
	}
	inline int getIDEOLOGY_SCORE_GRAND_STRATS()
	{
		return m_iIDEOLOGY_SCORE_GRAND_STRATS;
	}
	inline int getIDEOLOGY_SCORE_HAPPINESS()
	{
		return m_iIDEOLOGY_SCORE_HAPPINESS;
	}
	inline int getIDEOLOGY_SCORE_PER_FREE_TENET()
	{
		return m_iIDEOLOGY_SCORE_PER_FREE_TENET;
	}
	inline int getIDEOLOGY_SCORE_HOSTILE()
	{
		return m_iIDEOLOGY_SCORE_HOSTILE;
	}
	inline int getIDEOLOGY_SCORE_GUARDED()
	{
		return m_iIDEOLOGY_SCORE_GUARDED;
	}
	inline int getIDEOLOGY_SCORE_AFRAID()
	{
		return m_iIDEOLOGY_SCORE_AFRAID;
	}
	inline int getIDEOLOGY_SCORE_FRIENDLY()
	{
		return m_iIDEOLOGY_SCORE_FRIENDLY;
	}
	inline int getPOLICY_BRANCH_FREEDOM()
	{
		return m_iPOLICY_BRANCH_FREEDOM;
	}
	inline int getPOLICY_BRANCH_AUTOCRACY()
	{
		return m_iPOLICY_BRANCH_AUTOCRACY;
	}
	inline int getPOLICY_BRANCH_ORDER()
	{
		return m_iPOLICY_BRANCH_ORDER;
	}

	inline int getMIN_DIG_SITES_PER_MAJOR_CIV()
	{
		return m_iMIN_DIG_SITES_PER_MAJOR_CIV;
	}
	inline int getMAX_DIG_SITES_PER_MAJOR_CIV()
	{
		return m_iMAX_DIG_SITES_PER_MAJOR_CIV;
	}
	inline int getPERCENT_SITES_HIDDEN()
	{
		return m_iPERCENT_SITES_HIDDEN;
	}
	inline int getPERCENT_HIDDEN_SITES_WRITING()
	{
		return m_iPERCENT_HIDDEN_SITES_WRITING;
	}
	inline int getSAPPED_CITY_ATTACK_MODIFIER()
	{
		return m_iSAPPED_CITY_ATTACK_MODIFIER;
	}
	inline int getSAPPER_BONUS_RANGE()
	{
		return m_iSAPPER_BONUS_RANGE;
	}

	inline int getEXOTIC_GOODS_GOLD_MIN()
	{
		return m_iEXOTIC_GOODS_GOLD_MIN;
	}
	inline int getEXOTIC_GOODS_GOLD_MAX()
	{
		return m_iEXOTIC_GOODS_GOLD_MAX;
	}
	inline int getEXOTIC_GOODS_XP_MIN()
	{
		return m_iEXOTIC_GOODS_XP_MIN;
	}
	inline int getEXOTIC_GOODS_XP_MAX()
	{
		return m_iEXOTIC_GOODS_XP_MAX;
	}

	inline int getTEMPORARY_CULTURE_BOOST_MOD()
	{
		return m_iTEMPORARY_CULTURE_BOOST_MOD;
	}
	inline int getTEMPORARY_TOURISM_BOOST_MOD()
	{
		return m_iTEMPORARY_TOURISM_BOOST_MOD;
	}

	inline int getLEAGUE_SESSION_INTERVAL_BASE_TURNS()
	{
		return m_iLEAGUE_SESSION_INTERVAL_BASE_TURNS;
	}
	inline int getLEAGUE_SESSION_SOON_WARNING_TURNS()
	{
		return m_iLEAGUE_SESSION_SOON_WARNING_TURNS;
	}
	inline int getLEAGUE_MEMBER_PROPOSALS_BASE()
	{
		return m_iLEAGUE_MEMBER_PROPOSALS_BASE;
	}
	inline int getLEAGUE_MEMBER_VOTES_BASE()
	{
		return m_iLEAGUE_MEMBER_VOTES_BASE;
	}
	inline int getLEAGUE_MEMBER_VOTES_FOR_HOST()
	{
		return m_iLEAGUE_MEMBER_VOTES_FOR_HOST;
	}
	inline int getLEAGUE_MEMBER_VOTES_PER_CITY_STATE_ALLY()
	{
		return m_iLEAGUE_MEMBER_VOTES_PER_CITY_STATE_ALLY;
	}
	inline float getLEAGUE_PROJECT_REWARD_TIER_1_THRESHOLD()
	{
		return m_fLEAGUE_PROJECT_REWARD_TIER_1_THRESHOLD;
	}
	inline float getLEAGUE_PROJECT_REWARD_TIER_2_THRESHOLD()
	{
		return m_fLEAGUE_PROJECT_REWARD_TIER_2_THRESHOLD;
	}

#ifdef NQ_SPOILS_OF_WAR
	inline int getSPOILS_OF_WAR_ANCIENT()
	{
		return m_iSPOILS_OF_WAR_PERCENT_ANCIENT;
	}
	inline int getSPOILS_OF_WAR_CLASSICAL()
	{
		return m_iSPOILS_OF_WAR_PERCENT_CLASSICAL;
	}
	inline int getSPOILS_OF_WAR_MEDIEVAL()
	{
		return m_iSPOILS_OF_WAR_PERCENT_MEDIEVAL;
	}
	inline int getSPOILS_OF_WAR_RENAISSANCE()
	{
		return m_iSPOILS_OF_WAR_PERCENT_RENAISSANCE;
	}
	inline int getSPOILS_OF_WAR_INDUSTRIAL()
	{
		return m_iSPOILS_OF_WAR_PERCENT_INDUSTRIAL;
	}
	inline int getSPOILS_OF_WAR_MODERN()
	{
		return m_iSPOILS_OF_WAR_PERCENT_MODERN;
	}
	inline int getSPOILS_OF_WAR_ATOMIC()
	{
		return m_iSPOILS_OF_WAR_PERCENT_ATOMIC;
	}
	inline int getSPOILS_OF_WAR_FUTURE()
	{
		return m_iSPOILS_OF_WAR_PERCENT_FUTURE;
	}
#endif

	////////////// END DEFINES //////////////////

	void setDLLIFace(ICvEngineUtility4* pDll)
	{
		m_pDLL = pDll;
	}

	ICvEngineUtility4* getDLLIFace()
	{
		return m_pDLL;
	}

	void SetEngineUserInterface(ICvUserInterface2* pUI)
	{
		m_pEngineUI = pUI;
	}

	ICvUserInterface2* GetEngineUserInterface()
	{
		return m_pEngineUI;
	}

	bool IsGraphicsInitialized() const;
	void SetGraphicsInitialized(bool bVal);

	// for caching
	bool readEventInfoArray(FDataStream& kStream);
	void writeEventInfoArray(FDataStream& kStream);

	bool readEventTriggerInfoArray(FDataStream& kStream);
	void writeEventTriggerInfoArray(FDataStream& kStream);

	//
	// additional accessors for initting globals
	//
	void setPathFinder(CvTwoLayerPathFinder* pVal);
	void setInterfacePathFinder(CvTwoLayerPathFinder* pVal);
	void setIgnoreUnitsPathFinder(CvIgnoreUnitsPathFinder* pVal);
	void setStepFinder(CvStepPathFinder* pVal);
	void setRouteFinder(CvAStar* pVal);
	void SetWaterRouteFinder(CvAStar* pVal);
	void setAreaFinder(CvAStar* pVal);
	void setInfluenceFinder(CvAStar* pVal);
	void SetBuildRouteFinder(CvAStar* pVal);
	void SetInternationalTradeRouteLandFinder(CvAStar* pVal);
	void SetInternationalTradeRouteWaterFinder(CvAStar* pVal);
	void SetTacticalAnalysisMapFinder(CvTwoLayerPathFinder* pVal);
#ifdef AUI_DANGER_PLOTS_REMADE
	void setDangerPathFinder(CvTwoLayerPathFinder* pVal);
#endif

	// So that CvEnums are moddable in the DLL
	int getNumDirections() const;
	int getNumGameOptions() const;
	int getNumMPOptions() const;
	int getNumSpecialOptions() const;
	int getNumGraphicOptions() const;
	int getNumCommandInfos() const;
	int getNumControlInfos() const;
	int getNumPlayerOptionInfos() const;
	int getNumGraphicLevels() const;

	bool getOutOfSyncDebuggingEnabled() const;
	void setOutOfSyncDebuggingEnabled(bool isEnabled);

	void deleteInfoArrays();

protected:

	bool m_bGraphicsInitialized;
	bool m_bDLLProfiler;
	bool m_bTutorialDisabled;
	bool m_bLogging;
	int  m_iRandLogging;
	bool m_bSynchLogging;
	bool m_bAILogging;
	bool m_bAIPerfLogging;
	bool m_bBuilderAILogging;
	bool m_bSerializationLogging;
	bool m_bPlayerAndCityAILogSplit;
	bool m_bTutorialLogging;
	bool m_bTutorialDebugging;
	bool m_bAllowRClickMovementWhileCameraScrolling;
	bool m_bPostTurnAutosaves;
	bool m_bOverwriteLogs;
	bool m_bOutOfSyncDebuggingEnabled;
	CvPoint3  m_pt3CameraDir;
	int m_iNewPlayers;

	bool m_bZoomOut;
	bool m_bZoomIn;
	bool m_bLoadGameFromFile;

	CvRandom* m_asyncRand;

	CvGame* m_game;

	CvMap* m_map;

	CvTwoLayerPathFinder* m_pathFinder;
	CvTwoLayerPathFinder* m_interfacePathFinder;
	CvIgnoreUnitsPathFinder* m_ignoreUnitsPathFinder;
	CvStepPathFinder* m_stepFinder;
	CvAStar* m_routeFinder;
	CvAStar* m_waterRouteFinder;
	CvAStar* m_borderFinder;
	CvAStar* m_areaFinder;
	CvAStar* m_influenceFinder;
	CvAStar* m_buildRouteFinder;
	CvAStar* m_internationalTradeRouteLandFinder;
	CvAStar* m_internationalTradeRouteWaterFinder;
	CvTwoLayerPathFinder* m_tacticalAnalysisMapFinder;
#ifdef AUI_DANGER_PLOTS_REMADE
	CvTwoLayerPathFinder* m_pathDangerFinder;
#endif

	ICvDLLDatabaseUtility1* m_pkDatabaseLoadUtility;

	int m_aiPlotDirectionX[NUM_DIRECTION_TYPES];
	int m_aiPlotDirectionY[NUM_DIRECTION_TYPES];
	int m_aiCityPlotX[NUM_CITY_PLOTS];
	int m_aiCityPlotY[NUM_CITY_PLOTS];
	int m_aiCityPlotPriority[NUM_CITY_PLOTS];
	int m_aaiXYCityPlot[CITY_PLOTS_DIAMETER][CITY_PLOTS_DIAMETER];

	DirectionTypes m_aeTurnLeftDirection[NUM_DIRECTION_TYPES];
	DirectionTypes m_aeTurnRightDirection[NUM_DIRECTION_TYPES];

	FlowDirectionTypes* m_aeTurnLeftFlowDirection[NUM_FLOWDIRECTION_TYPES];
	FlowDirectionTypes* m_aeTurnRightFlowDirection[NUM_FLOWDIRECTION_TYPES];

	//InterfaceModeInfo m_aInterfaceModeInfo[NUM_INTERFACEMODE_TYPES] =
	std::vector<CvInterfaceModeInfo*> m_paInterfaceModeInfo;

	/***********************************************************************************************************************
	Globals loaded from Database
	************************************************************************************************************************/
	Database::Connection* m_pGameDatabase;
	Database::Results m_kGlobalDefinesLookup;

	// all type strings are upper case and are kept in this hash map for fast lookup, Moose
	InfosMap		m_infosMap;
	InfosHashMap	m_infosHashMap;		// Hash of the type string, mapped to the index of the info in its array.

	std::vector<CvColorInfo*> m_paColorInfo;
	std::vector<CvPlayerColorInfo*> m_paPlayerColorInfo;

	std::vector<CvTerrainInfo*> m_paTerrainInfo;
	std::vector<CvYieldInfo*> m_paYieldInfo;
	std::vector<CvRouteInfo*> m_paRouteInfo;
	std::vector<CvFeatureInfo*> m_paFeatureInfo;
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	std::vector<CvGreatWorkClassInfo*> m_paGreatWorkClassInfo;
#endif
	std::vector<CvResourceClassInfo*> m_paResourceClassInfo;
#if defined(TRADE_REFACTOR)
	std::vector<CvTradeConnectionInfo*> m_paTradeConnectionInfo;
#endif
	std::vector<CvResourceInfo*> m_paResourceInfo;
	std::vector<CvBuildInfo*> m_paBuildInfo;
	std::vector<CvHandicapInfo*> m_paHandicapInfo;
	std::vector<CvGameSpeedInfo*> m_paGameSpeedInfo;
	std::vector<CvTurnTimerInfo*> m_paTurnTimerInfo;
	std::vector<CvCivilizationInfo*> m_paCivilizationInfo;
	int m_iNumPlayableCivilizationInfos;
	int m_iNumAIPlayableCivilizationInfos;
	std::vector<CvMinorCivInfo*> m_paMinorCivInfo;
#ifdef LEKMOD_MINOR_CIV_PERSONALITIES
	std::vector<CvMinorCivPersonalityInfo*> m_paMinorCivPersonalityInfo;
#endif
	std::vector<CvLeaderHeadInfo*> m_paLeaderHeadInfo;
	std::vector<CvProcessInfo*> m_paProcessInfo;
	std::vector<CvVoteInfo*> m_paVoteInfo;
	std::vector<CvBuildingClassInfo*> m_paBuildingClassInfo;
	std::vector<CvUnitClassInfo*> m_paUnitClassInfo;
	std::vector<CvSpecialUnitInfo*> m_paSpecialUnitInfo;
	std::vector<CvVoteSourceInfo*> m_paVoteSourceInfo;
	std::vector<CvBaseInfo*> m_paUnitCombatClassInfo;
	std::vector<CvBaseInfo*> m_paUnitAIInfos;
	std::vector<CvGameOptionInfo*> m_paGameOptionInfos;
	std::vector<CvMPOptionInfo*> m_paMPOptionInfos;
	std::vector<CvPlayerOptionInfo*> m_paPlayerOptionInfos;
	std::vector<CvSpecialistInfo*> m_paSpecialistInfo;
	std::vector<CvActionInfo*> m_paActionInfo;
	std::vector<CvMissionInfo*> m_paMissionInfo;
	std::vector<CvControlInfo*> m_paControlInfo;
	std::vector<CvCommandInfo*> m_paCommandInfo;
	std::vector<CvAutomateInfo*> m_paAutomateInfo;
	std::vector<CvEraInfo*> m_aEraInfo;	// [NUM_ERA_TYPES];
	std::vector<CvHurryInfo*> m_paHurryInfo;
	std::vector<CvVictoryInfo*> m_paVictoryInfo;
	std::vector<CvSmallAwardInfo*> m_paSmallAwardInfo;
	std::vector<CvEntityEventInfo*> m_paEntityEventInfo;
	std::vector<CvDomainInfo*> m_paUnitDomainInfo;

	std::vector<CvMultiUnitFormationInfo*> m_paMultiUnitFormationInfo;

	CvEconomicAIStrategyXMLEntries* m_pEconomicAIStrategies;
	CvCitySpecializationXMLEntries* m_pCitySpecializations;
	CvTacticalMoveXMLEntries* m_pTacticalMoves;
	CvMilitaryAIStrategyXMLEntries* m_pMilitaryAIStrategies;
	CvAIGrandStrategyXMLEntries* m_pAIGrandStrategies;
	CvAICityStrategies* m_pAICityStrategies;
	CvPolicyXMLEntries* m_pPolicies;
	CvTechXMLEntries* m_pTechs;
	CvBuildingXMLEntries* m_pBuildings;
	CvUnitXMLEntries* m_pUnits;
	CvProjectXMLEntries* m_pProjects;
	CvPromotionXMLEntries* m_pPromotions;
	CvImprovementXMLEntries* m_pImprovements;
	CvEmphasisXMLEntries* m_pEmphases;
	CvTraitXMLEntries* m_pTraits;
	CvReligionXMLEntries* m_pReligions;
	CvBeliefXMLEntries* m_pBeliefs;
	CvLeagueSpecialSessionXMLEntries* m_pLeagueSpecialSessions;
	CvLeagueNameXMLEntries* m_pLeagueNames;
	CvLeagueProjectXMLEntries* m_pLeagueProjects;
	CvLeagueProjectRewardXMLEntries* m_pLeagueProjectRewards;
	CvResolutionXMLEntries* m_pResolutions;
	CvNotificationXMLEntries* m_pNotifications;

	//////////////////////////////////////////////////////////////////////////
	// GLOBAL TYPES
	//////////////////////////////////////////////////////////////////////////

	// XXX These are duplicates and are kept for enumeration convenience - most could be removed, Moose
	CvString* m_paszFlavorTypes;
	int m_iNumFlavorTypes;

	CvString* m_paszFootstepAudioTags;
	int m_iNumFootstepAudioTags;

	//////////////////////////////////////////////////////////////////////////
	// Formerly Global Defines
	//////////////////////////////////////////////////////////////////////////

	int getDefineINT(const char* szName, bool bReportErrors = true);
	float getDefineFLOAT(const char* szName, bool bReportErrors = true);
#ifdef AUI_CACHE_DOUBLE
	double getDefineDOUBLE(const char* szName, bool bReportErrors = true);
#endif

	bool getDefineValue(const char* szName, int& iValue, bool bReportErrors = true);
	bool getDefineValue(const char* szName, float& fValue, bool bReportErrors = true);
#ifdef AUI_CACHE_DOUBLE
	bool getDefineValue(const char* szName, double& dValue, bool bReportErrors = true);
#endif
	bool getDefineValue(const char* szName, CvString& szValue, bool bReportErrors = true);

	// -- Togglable AuI options
#ifdef AUI_DIPLOMACY_AI_LEADERHEAD_DEALS_IN_MULTIPLAYER
	GD_INT_DECL(MULTIPLAYER_AI_DIPLOMACY_NOTIFICATIONS_ONLY_SIMULTANEOUS);
#endif
#ifdef AUI_UNIT_PRODUCTION_AI_LUA_FLAVOR_WEIGHTS
	GD_INT_DECL(DISABLE_UNIT_AI_FLAVOR_LUA_MODDING);
#endif
#ifdef AUI_BUILDING_PRODUCTION_AI_LUA_FLAVOR_WEIGHTS
	GD_INT_DECL(DISABLE_BUILDING_AI_FLAVOR_LUA_MODDING);
#endif
#ifdef AUI_PROJECT_PRODUCTION_AI_LUA_FLAVOR_WEIGHTS
	GD_INT_DECL(DISABLE_PROJECT_AI_FLAVOR_LUA_MODDING);
#endif
#ifdef AUI_PROCESS_PRODUCTION_AI_LUA_FLAVOR_WEIGHTS
	GD_INT_DECL(DISABLE_PROCESS_AI_FLAVOR_LUA_MODDING);
#endif
#ifdef NQM_OPTIONAL_SCORING_TWEAKS
	GD_INT_DECL(NEW_SCORE_CITY_MULTIPLIER);
	GD_INT_DECL(NEW_SCORE_POPULATION_MULTIPLIER);
	GD_INT_DECL(NEW_SCORE_WONDER_MULTIPLIER);
	GD_INT_DECL(NEW_SCORE_TECH_MULTIPLIER);
	GD_INT_DECL(NEW_SCORE_FUTURE_TECH_MULTIPLIER);
	GD_INT_DECL(NEW_SCORE_POLICY_MULTIPLIER);
	GD_INT_DECL(NEW_SCORE_GREAT_WORK_MULTIPLIER);
	GD_INT_DECL(NEW_SCORE_BELIEF_MULTIPLIER);
#endif

	// -- ints --

	int m_iAI_ATTEMPT_RUSH_OVER_X_TURNS_TO_BUILD;
	int m_iINITIAL_AI_CITY_PRODUCTION;
	int m_iAI_CAN_DISBAND_UNITS;
	int m_iAI_SHOULDNT_MANAGE_PLOT_ASSIGNMENT;
	int m_iPOLICY_WEIGHT_PROPAGATION_PERCENT;
	int m_iPOLICY_WEIGHT_PROPAGATION_LEVELS;
	int m_iPOLICY_WEIGHT_PERCENT_DROP_NEW_BRANCH;
	int m_iTECH_WEIGHT_PROPAGATION_PERCENT;
	int m_iTECH_WEIGHT_PROPAGATION_LEVELS;
	int m_iTECH_PRIORITY_UNIQUE_ITEM;
	int m_iTECH_PRIORITY_MAYA_CALENDAR_BONUS;
	int m_iAI_VALUE_OF_YIELD_GOLD;
	int m_iAI_VALUE_OF_YIELD_PRODUCTION;
	int m_iAI_VALUE_OF_YIELD_SCIENCE;
	int m_iDEFAULT_FLAVOR_VALUE;
	int m_iPERSONALITY_FLAVOR_MAX_VALUE;
	int m_iPERSONALITY_FLAVOR_MIN_VALUE;
	int m_iFLAVOR_MIN_VALUE;
	int m_iFLAVOR_MAX_VALUE;
	int m_iFLAVOR_RANDOMIZATION_RANGE;
	int m_iFLAVOR_EXPANDGROW_COEFFICIENT;
	int m_iAI_GRAND_STRATEGY_NUM_TURNS_STRATEGY_MUST_BE_ACTIVE;
	int m_iAI_GS_RAND_ROLL;
	int m_iAI_GRAND_STRATEGY_CURRENT_STRATEGY_WEIGHT;
	int m_iAI_GRAND_STRATEGY_GUESS_NO_CLUE_WEIGHT;
	int m_iAI_GRAND_STRATEGY_GUESS_POSITIVE_THRESHOLD;
	int m_iAI_GRAND_STRATEGY_GUESS_LIKELY_THRESHOLD;
	int m_iAI_GRAND_STRATEGY_OTHER_PLAYERS_GS_MULTIPLIER;
	int m_iAI_GS_CONQUEST_NOBODY_MET_FIRST_TURN;
	int m_iAI_GRAND_STRATEGY_CONQUEST_NOBODY_MET_WEIGHT;
	int m_iAI_GRAND_STRATEGY_CONQUEST_AT_WAR_WEIGHT;
	int m_iAI_GS_CONQUEST_MILITARY_STRENGTH_FIRST_TURN;
	int m_iAI_GRAND_STRATEGY_CONQUEST_POWER_RATIO_MULTIPLIER;
	int m_iAI_GRAND_STRATEGY_CONQUEST_CRAMPED_WEIGHT;
	int m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_ATTACKED;
	int m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MINOR_CONQUERED;
	int m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_ATTACKED;
	int m_iAI_GRAND_STRATEGY_CONQUEST_WEIGHT_PER_MAJOR_CONQUERED;
	int m_iAI_GS_CULTURE_RATIO_MULTIPLIER;
	int m_iAI_GS_TOURISM_RATIO_MULTIPLIER;
	int m_iAI_GS_CULTURE_AHEAD_WEIGHT;
	int m_iAI_GS_CULTURE_TOURISM_AHEAD_WEIGHT;
	int m_iAI_GS_CULTURE_INFLUENTIAL_CIV_MOD;
	int m_iAI_GRAND_STRATEGY_UN_EACH_MINOR_ATTACKED_WEIGHT;
	int m_iAI_GS_UN_SECURED_VOTE_MOD;
	int m_iAI_GS_SS_HAS_APOLLO_PROGRAM;
	int m_iAI_GS_SS_TECH_PROGRESS_MOD;
	int m_iAI_STRATEGY_EARLY_EXPLORATION_STARTING_WEIGHT;
	int m_iAI_STRATEGY_EARLY_EXPLORATION_EXPLORERS_WEIGHT_DIVISOR;
	int m_iAI_STRATEGY_EARLY_EXPLORATION_WEIGHT_PER_FLAVOR;
	int m_iAI_STRATEGY_EARLY_EXPANSION_NUM_UNOWNED_TILES_REQUIRED;
	int m_iAI_STRATEGY_EARLY_EXPANSION_NUM_CITIES_LIMIT;
	float m_fAI_STRATEGY_NEED_IMPROVEMENT_CITY_RATIO;
	int m_iAI_STRATEGY_ISLAND_START_COAST_REVEAL_PERCENT;
	int m_iAI_STRATEGY_AREA_IS_FULL_PERCENT;
	int m_iAI_STRATEGY_MINIMUM_SETTLE_FERTILITY;
	int m_iAI_BUY_PLOT_TEST_PROBES;
	int m_iAI_PLOT_VALUE_STRATEGIC_RESOURCE;
	int m_iAI_PLOT_VALUE_LUXURY_RESOURCE;
	int m_iAI_PLOT_VALUE_SPECIALIZATION_MULTIPLIER;
	int m_iAI_PLOT_VALUE_YIELD_MULTIPLIER;
	int m_iAI_PLOT_VALUE_DEFICIENT_YIELD_MULTIPLIER;
	int m_iAI_PLOT_VALUE_FIERCE_DISPUTE;
	int m_iAI_PLOT_VALUE_STRONG_DISPUTE;
	int m_iAI_PLOT_VALUE_WEAK_DISPUTE;
	int m_iAI_GOLD_PRIORITY_MINIMUM_PLOT_BUY_VALUE;
	int m_iAI_GOLD_BALANCE_TO_HALVE_PLOT_BUY_MINIMUM;
	int m_iAI_GOLD_PRIORITY_UPGRADE_BASE;
	int m_iAI_GOLD_PRIORITY_UPGRADE_PER_FLAVOR_POINT;
	int m_iAI_GOLD_PRIORITY_DIPLOMACY_BASE;
	int m_iAI_GOLD_PRIORITY_DIPLOMACY_PER_FLAVOR_POINT;
	int m_iAI_GOLD_PRIORITY_UNIT;
	int m_iAI_GOLD_PRIORITY_DEFENSIVE_BUILDING;
	int m_iAI_GOLD_PRIORITY_BUYOUT_CITY_STATE;
	int m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_FOOD;
	int m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_PRODUCTION;
	int m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_GOLD;
	int m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_SCIENCE;
	int m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_CULTURE;
	int m_iBUILDER_TASKING_PLOT_EVAL_MULTIPLIER_LUXURY_RESOURCE;
	int m_iBUILDER_TASKING_IMPROVE_RESOURCE_TENDENCY;
	int m_iBUILDER_TASKING_BUILD_ROUTE_TENDENCY;
	int m_iBUILDER_TASKING_DIRECTIVES_TO_EVALUATE;
	int m_iBUILDER_TASKING_BASELINE_BUILD_IMPROVEMENTS;
	int m_iBUILDER_TASKING_BASELINE_BUILD_RESOURCE_IMPROVEMENTS;
	int m_iBUILDER_TASKING_BASELINE_BUILD_ROUTES;
	int m_iBUILDER_TASKING_BASELINE_REPAIR;
	int m_iBUILDER_TASKING_BASELINE_SCRUB_FALLOUT;
	int m_iBUILDER_TASKING_BASELINE_ADDS_CULTURE;
	int m_iAI_MILITARY_THREAT_WEIGHT_MINOR;
	int m_iAI_MILITARY_THREAT_WEIGHT_MAJOR;
	int m_iAI_MILITARY_THREAT_WEIGHT_SEVERE;
	int m_iAI_MILITARY_THREAT_WEIGHT_CRITICAL;
	int m_iAI_STRATEGY_DEFEND_MY_LANDS_BASE_UNITS;
	int m_iAI_STRATEGY_MILITARY_RESERVE_PERCENTAGE;
	int m_iAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL;
	int m_iAI_MILITARY_BARBARIANS_FOR_MINOR_THREAT;
	int m_iAI_MILITARY_IGNORE_BAD_ODDS;
	int m_iAI_MILITARY_RECAPTURING_OWN_CITY;
	int m_iAI_MILITARY_CAPTURING_ORIGINAL_CAPITAL;
	int m_iAI_CITY_SPECIALIZATION_EARLIEST_TURN;
	int m_iAI_CITY_SPECIALIZATION_REEVALUATION_INTERVAL;
	int m_iAI_CITY_SPECIALIZATION_GENERAL_ECONOMIC_WEIGHT;
	int m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_FLAVOR_EXPANSION;
	int m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_PERCENT_CONTINENT_UNOWNED;
	int m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_CITIES;
	int m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_SETTLERS;
	int m_iAI_CITY_SPECIALIZATION_FOOD_WEIGHT_EARLY_EXPANSION;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_OPERATIONAL_UNITS_REQUESTED;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CIVS_AT_WAR_WITH;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_WAR_MOBILIZATION;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE_CRITICAL;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CAPITAL_THREAT;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS_CRITICAL;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_WONDER;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_SPACESHIP;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_OFFENSE;
	int m_iAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_PERSONALITY;
	int m_iAI_CITY_SPECIALIZATION_GOLD_WEIGHT_FLAVOR_GOLD;
	int m_iAI_CITY_SPECIALIZATION_GOLD_WEIGHT_LAND_DISPUTE;
	int m_iAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SCIENCE;
	int m_iAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SPACESHIP;
	int m_iAI_CITY_SPECIALIZATION_YIELD_WEIGHT_FIRST_RING;
	int m_iAI_CITY_SPECIALIZATION_YIELD_WEIGHT_SECOND_RING;
	int m_iAI_CITY_SPECIALIZATION_YIELD_WEIGHT_THIRD_RING;
	int m_iAI_CITY_SPECIALIZATION_YIELD_NUM_TILES_CONSIDERED;
	int m_iAI_CITYSTRATEGY_SMALL_CITY_POP_THRESHOLD;
	int m_iAI_CITYSTRATEGY_MEDIUM_CITY_POP_THRESHOLD;
	int m_iAI_CITYSTRATEGY_LARGE_CITY_POP_THRESHOLD;
	int m_iAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_CITY_THRESHOLD;
	int m_iAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_WORKER_MOD;
	int m_iAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_DESPERATE_TURN;
	int m_iAI_CITYSTRATEGY_WANT_TILE_IMPROVERS_MINIMUM_SIZE;
	float m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_FOOD;
	float m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_PRODUCTION;
	float m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_GOLD;
	float m_fAI_CITYSTRATEGY_YIELD_DEFICIENT_SCIENCE;
	int m_iAI_CITYSTRATEGY_OPERATION_UNIT_BASE_WEIGHT;
	int m_iAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER;
	int m_iAI_CITYSTRATEGY_ARMY_UNIT_BASE_WEIGHT;
	int m_iAI_CITIZEN_VALUE_FOOD;
	int m_iAI_CITIZEN_VALUE_PRODUCTION;
	int m_iAI_CITIZEN_VALUE_GOLD;
	int m_iAI_CITIZEN_VALUE_SCIENCE;
	int m_iAI_CITIZEN_VALUE_CULTURE;
	int m_iAI_CITIZEN_VALUE_FAITH;
	int m_iAI_CITIZEN_FOOD_MOD_SIZE_CUTOFF;
	int m_iAI_CITIZEN_FOOD_MOD_SIZE_EXPONENT;
	int m_iAI_CITIZEN_MOD_FOOD_DEFICIT;
	int m_iAI_CITIZEN_MOD_PRODUCTION_DEFICIT;
	int m_iAI_OPERATIONAL_BARBARIAN_CAMP_DEPLOY_RANGE;
	int m_iAI_OPERATIONAL_CITY_ATTACK_DEPLOY_RANGE;
	int m_iAI_OPERATIONAL_PILLAGE_ENEMY_DEPLOY_RANGE;
	int m_iAI_OPERATIONAL_NAVAL_BOMBARDMENT_DEPLOY_RANGE;
	int m_iAI_OPERATIONAL_PERCENT_DANGER_FOR_FORWARD_MUSTER;
	int m_iAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION;
	int m_iAI_TACTICAL_MAP_UNIT_STRENGTH_MULTIPLIER;
	int m_iAI_TACTICAL_MAP_DOMINANCE_PERCENTAGE;
	int m_iAI_TACTICAL_MAP_TEMP_ZONE_RADIUS;
	int m_iAI_TACTICAL_MAP_TEMP_ZONE_TURNS;
	int m_iAI_TACTICAL_MAP_BOMBARDMENT_ZONE_TURNS;
	int m_iAI_TACTICAL_RECRUIT_RANGE;
	int m_iAI_TACTICAL_REPOSITION_RANGE;
	int m_iAI_TACTICAL_OVERKILL_PERCENT;
	int m_iAI_TACTICAL_BARBARIAN_RELEASE_VARIATION;
	int m_iAI_TACTICAL_MOVE_PRIORITY_RANDOMNESS;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_CAPTURE_CITY;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_DAMAGE_CITY;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_HIGH_UNIT;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_MEDIUM_UNIT;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESTROY_LOW_UNIT;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_TO_SAFETY;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_HIGH_UNIT;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_MEDIUM_UNIT;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_ATTRIT_LOW_UNIT;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_DESPERATE_ATTACK;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_CITADEL;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_PILLAGE_NEXT_TURN;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_BLOCKADE_RESOURCE;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_CIVILIAN_ATTACK;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_PASSIVE_MOVE;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_AGGRESSIVE_MOVE;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_CAMP_DEFENSE;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_ESCORT_CIVILIAN;
	int m_iAI_TACTICAL_BARBARIAN_PRIORITY_PLUNDER_TRADE_UNIT;
	int m_iAI_HOMELAND_MOVE_PRIORITY_RANDOMNESS;
	int m_iAI_HOMELAND_MAX_UPGRADE_MOVE_TURNS;
	int m_iAI_HOMELAND_MAX_DEFENSIVE_MOVE_TURNS;
	int m_iAI_HOMELAND_ESTIMATE_TURNS_DISTANCE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_SETTLE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_HEAL;
	int m_iAI_HOMELAND_MOVE_PRIORITY_TO_SAFETY;
	int m_iAI_HOMELAND_MOVE_PRIORITY_WORKER;
	int m_iAI_HOMELAND_MOVE_PRIORITY_WORKER_SEA;
	int m_iAI_HOMELAND_MOVE_PRIORITY_EXPLORE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_EXPLORE_SEA;
	int m_iAI_HOMELAND_MOVE_PRIORITY_SENTRY;
	int m_iAI_HOMELAND_MOVE_PRIORITY_MOBILE_RESERVE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_GARRISON;
	int m_iAI_HOMELAND_MOVE_PRIORITY_PATROL;
	int m_iAI_HOMELAND_MOVE_PRIORITY_UPGRADE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_ANCIENT_RUINS;
	int m_iAI_HOMELAND_MOVE_PRIORITY_GARRISON_CITY_STATE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_WRITER;
	int m_iAI_HOMELAND_MOVE_PRIORITY_ARTIST;
	int m_iAI_HOMELAND_MOVE_PRIORITY_MUSICIAN;
	int m_iAI_HOMELAND_MOVE_PRIORITY_SCIENTIST_FREE_TECH;
	int m_iAI_HOMELAND_MOVE_PRIORITY_ENGINEER_HURRY;
	int m_iAI_HOMELAND_MOVE_PRIORITY_GENERAL_GARRISON;
	int m_iAI_HOMELAND_MOVE_PRIORITY_ADMIRAL_GARRISON;
	int m_iAI_HOMELAND_MOVE_PRIORITY_PROPHET_RELIGION;
	int m_iAI_HOMELAND_MOVE_PRIORITY_MISSIONARY;
	int m_iAI_HOMELAND_MOVE_PRIORITY_INQUISITOR;
	int m_iAI_HOMELAND_MOVE_PRIORITY_SPACESHIP_PART;
	int m_iAI_HOMELAND_MOVE_PRIORITY_ADD_SPACESHIP_PART;
	int m_iAI_HOMELAND_MOVE_PRIORITY_AIRCRAFT_TO_THE_FRONT;
	int m_iAI_HOMELAND_MOVE_PRIORITY_TREASURE;
	int m_iAI_HOMELAND_MOVE_PRIORITY_TRADE_UNIT;
	int m_iAI_HOMELAND_MOVE_PRIORITY_ARCHAEOLOGIST;
	int m_iAI_HOMELAND_MOVE_PRIORITY_AIRLIFT;
	int m_iAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT;
	int m_iAI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT;
	int m_iAI_DIPLO_LAND_DISPUTE_WEIGHT_WEAK;
	int m_iAI_DIPLO_LAND_DISPUTE_WEIGHT_STRONG;
	int m_iAI_DIPLO_LAND_DISPUTE_WEIGHT_FIERCE;
	int m_iMINOR_BULLY_GOLD;
#if defined(FULL_YIELD_FROM_KILLS)
	int m_iPOLICY_YIELD_CAP;
	int m_iTRAIT_YIELD_CAP;
	int m_iBELIEF_YIELD_CAP;
	int m_iPROMOTION_YIELD_CAP;
	int m_iUNIT_YIELD_CAP;
#endif
#if defined(MISC_CHANGES) // New Global Values
	int m_iBULLY_GOLD_GROWTH_FACTOR;
	int m_iINTERNAL_TRADE_FOOD_BASE_TIMES100;
	int m_iINTERNAL_TRADE_PRODUCTION_BASE_TIMES100;
	int m_iFAITH_PURCHASE_VISIBLE_DIVISOR;
#endif
#if defined(LEKMOD_v34) // Support for the New Golden Age Points Yield
	int m_iAI_CITIZEN_VALUE_GOLDEN_AGE_POINTS;
#endif
	int m_iMINOR_FRIENDSHIP_RATE_MOD_MAXIMUM;
	int m_iMINOR_FRIENDSHIP_RATE_MOD_SHARED_RELIGION;
	int m_iMINOR_FRIENDSHIP_DROP_PER_TURN;
	int m_iMINOR_FRIENDSHIP_DROP_PER_TURN_HOSTILE;
	int m_iMINOR_FRIENDSHIP_DROP_PER_TURN_AGGRESSOR;
	int m_iMINOR_FRIENDSHIP_DROP_DISHONOR_PLEDGE_TO_PROTECT;
	int m_iMINOR_FRIENDSHIP_DROP_BULLY_GOLD_SUCCESS;
	int m_iMINOR_FRIENDSHIP_DROP_BULLY_GOLD_FAILURE;
	int m_iMINOR_FRIENDSHIP_DROP_BULLY_WORKER_SUCCESS;
	int m_iMINOR_FRIENDSHIP_DROP_BULLY_WORKER_FAILURE;
	int m_iMINOR_FRIENDSHIP_NEGATIVE_INCREASE_PER_TURN;
	int m_iMINOR_FRIENDSHIP_AT_WAR;
	int m_iMINOR_CIV_AGGRESSOR_THRESHOLD;
	int m_iMINOR_CIV_WARMONGER_THRESHOLD;
	int m_iPERMANENT_WAR_AGGRESSOR_CHANCE;
	int m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_DISTANT;
	int m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_FAR;
	int m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_CLOSE;
	int m_iPERMANENT_WAR_OTHER_WARMONGER_CHANCE_NEIGHBORS;
	int m_iPERMANENT_WAR_OTHER_CHANCE_DISTANT;
	int m_iPERMANENT_WAR_OTHER_CHANCE_FAR;
	int m_iPERMANENT_WAR_OTHER_CHANCE_CLOSE;
	int m_iPERMANENT_WAR_OTHER_CHANCE_NEIGHBORS;
	int m_iPERMANENT_WAR_OTHER_AT_WAR;
	int m_iFRIENDSHIP_NEUTRAL_ON_DEATH;
	int m_iFRIENDSHIP_THRESHOLD_NEUTRAL;
	int m_iFRIENDSHIP_FRIENDS_ON_DEATH;
	int m_iFRIENDSHIP_THRESHOLD_FRIENDS;
	int m_iFRIENDSHIP_ALLIES_ON_DEATH;
	int m_iFRIENDSHIP_THRESHOLD_ALLIES;
	int m_iFRIENDSHIP_THRESHOLD_MAX;
	int m_iFRIENDSHIP_THRESHOLD_CAN_BULLY;
	int m_iFRIENDSHIP_THRESHOLD_CAN_PLEDGE_TO_PROTECT;
	int m_iMINOR_FRIENDSHIP_CLOSE_AMOUNT;
	int m_iMINOR_CIV_SCIENCE_BONUS_MULTIPLIER;
	int m_iFRIENDS_CULTURE_BONUS_AMOUNT_ANCIENT;
	int m_iFRIENDS_CULTURE_BONUS_AMOUNT_MEDIEVAL;
	int m_iFRIENDS_CULTURE_BONUS_AMOUNT_INDUSTRIAL;
	int m_iALLIES_CULTURE_BONUS_AMOUNT_ANCIENT;
	int m_iALLIES_CULTURE_BONUS_AMOUNT_MEDIEVAL;
	int m_iALLIES_CULTURE_BONUS_AMOUNT_INDUSTRIAL;
	int m_iFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE;
	int m_iFRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_POST_RENAISSANCE;
	int m_iFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE;
	int m_iFRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_POST_RENAISSANCE;
	int m_iALLIES_CAPITAL_FOOD_BONUS_AMOUNT;
	int m_iALLIES_OTHER_CITIES_FOOD_BONUS_AMOUNT;
	int m_iFRIENDS_BASE_TURNS_UNIT_SPAWN;
	int m_iFRIENDS_RAND_TURNS_UNIT_SPAWN;
	int m_iALLIES_EXTRA_TURNS_UNIT_SPAWN;
	int m_iUNIT_SPAWN_BIAS_MULTIPLIER;
	int m_iUNIT_SPAWN_NUM_CHOICES;
	int m_iFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT;
	int m_iFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL;
	int m_iFRIENDS_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	int m_iALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_ANCIENT;
	int m_iALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_MEDIEVAL;
	int m_iALLIES_HAPPINESS_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	int m_iFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT;
	int m_iFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL;
	int m_iFRIENDS_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL;
	int m_iALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_ANCIENT;
	int m_iALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_MEDIEVAL;
	int m_iALLIES_HAPPINESS_PER_LUXURY_BONUS_AMOUNT_INDUSTRIAL;
	int m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	int m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE;
	int m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL;
	int m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL;
	int m_iFRIENDS_FAITH_FLAT_BONUS_AMOUNT_ANCIENT;
	int m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL;
	int m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE;
	int m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL;
	int m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL;
	int m_iALLIES_FAITH_FLAT_BONUS_AMOUNT_ANCIENT;
	int m_iMINOR_TURNS_GREAT_PEOPLE_SPAWN_BASE;
	int m_iMINOR_TURNS_GREAT_PEOPLE_SPAWN_RAND;
	int m_iMINOR_TURNS_GREAT_PEOPLE_SPAWN_BIAS_MULTIPLY;
	int m_iMINOR_ADDITIONAL_ALLIES_GP_CHANGE;
	int m_iMAX_MINOR_ADDITIONAL_ALLIES_GP_CHANGE;
	int m_iMAX_DISTANCE_MINORS_BARB_QUEST;
	int m_iTXT_KEY_MINOR_GIFT_UNITS_REMINDER;
	int m_iWAR_QUEST_COMPLETE_FRIENDSHIP;
	int m_iWAR_QUEST_UNITS_TO_KILL_DIVISOR;
	int m_iWAR_QUEST_MIN_UNITS_TO_KILL;
	int m_iMINOR_QUEST_FRIENDSHIP_ROUTE;
	int m_iMINOR_QUEST_FRIENDSHIP_KILL_CAMP;
	int m_iMINOR_QUEST_FRIENDSHIP_CONNECT_RESOURCE;
	int m_iMINOR_QUEST_FRIENDSHIP_CONSTRUCT_WONDER;
	int m_iMINOR_QUEST_FRIENDSHIP_GREAT_PERSON;
	int m_iMINOR_QUEST_FRIENDSHIP_KILL_CITY_STATE;
	int m_iMINOR_QUEST_FRIENDSHIP_FIND_PLAYER;
	int m_iMINOR_QUEST_FRIENDSHIP_FIND_NATURAL_WONDER;
	int m_iMINOR_QUEST_FRIENDSHIP_GIVE_GOLD;
	int m_iMINOR_QUEST_FRIENDSHIP_PLEDGE_TO_PROTECT;
	int m_iMINOR_QUEST_FRIENDSHIP_CONTEST_CULTURE;
	int m_iMINOR_QUEST_FRIENDSHIP_CONTEST_FAITH;
	int m_iMINOR_QUEST_FRIENDSHIP_CONTEST_TECHS;
	int m_iMINOR_QUEST_FRIENDSHIP_INVEST;
	int m_iMINOR_QUEST_FRIENDSHIP_BULLY_CITY_STATE;
	int m_iMINOR_QUEST_FRIENDSHIP_DENOUNCE_MAJOR;
	int m_iMINOR_QUEST_FRIENDSHIP_SPREAD_RELIGION;
	int m_iMINOR_QUEST_FRIENDSHIP_TRADE_ROUTE;
	int m_iMINOR_QUEST_STANDARD_CONTEST_LENGTH;
	int m_iMINOR_CIV_GOLD_GIFT_GAME_MULTIPLIER;
	int m_iMINOR_CIV_GOLD_GIFT_GAME_DIVISOR;
	int m_iMINOR_CIV_GOLD_GIFT_VISIBLE_DIVISOR;
	int m_iMINOR_CIV_GOLD_GIFT_MINIMUM_FRIENDSHIP_REWARD;
	int m_iMINOR_CIV_BEST_RELATIONS_HAPPINESS_BONUS;
	int m_iMINOR_CIV_RESOURCE_SEARCH_RADIUS;
	int m_iFRIENDSHIP_PER_UNIT_INTRUDING;
	int m_iFRIENDSHIP_PER_BARB_KILLED;
	int m_iFRIENDSHIP_PER_UNIT_GIFTED;
	int m_iMAX_INFLUENCE_FROM_MINOR_GIFTS;
	int m_iMINOR_LIBERATION_FRIENDSHIP;
	int m_iRETURN_CIVILIAN_FRIENDSHIP;
	int m_iMINOR_CIV_MAX_GLOBAL_QUESTS_FOR_PLAYER;
	int m_iMINOR_CIV_MAX_PERSONAL_QUESTS_FOR_PLAYER;
	int m_iMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN;
	int m_iMINOR_CIV_GLOBAL_QUEST_FIRST_POSSIBLE_TURN_RAND;
	int m_iMINOR_CIV_GLOBAL_QUEST_MIN_TURNS_BETWEEN;
	int m_iMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN;
	int m_iMINOR_CIV_GLOBAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER;
	int m_iMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN;
	int m_iMINOR_CIV_PERSONAL_QUEST_FIRST_POSSIBLE_TURN_RAND;
	int m_iMINOR_CIV_PERSONAL_QUEST_MIN_TURNS_BETWEEN;
	int m_iMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN;
	int m_iMINOR_CIV_PERSONAL_QUEST_RAND_TURNS_BETWEEN_HOSTILE_MULTIPLIER;
	int m_iMINOR_CIV_QUEST_KILL_CAMP_RANGE;
	int m_iMINOR_CIV_QUEST_WONDER_COMPLETION_THRESHOLD;
	int m_iMINOR_CIV_THREATENED_BY_BARBS_NOTIF_THRESHOLD;
	int m_iMINOR_CIV_MERCANTILE_RESOURCES_QUANTITY;
	int m_iMINOR_CIV_MERCANTILE_RESOURCES_KEEP_ON_CAPTURE_DISABLED;
	int m_iQUEST_DISABLED_ROUTE;
	int m_iQUEST_DISABLED_KILL_CAMP;
	int m_iQUEST_DISABLED_CONNECT_RESOURCE;
	int m_iQUEST_DISABLED_CONSTRUCT_WONDER;
	int m_iQUEST_DISABLED_GREAT_PERSON;
	int m_iQUEST_DISABLED_KILL_CITY_STATE;
	int m_iQUEST_DISABLED_FIND_PLAYER;
	int m_iQUEST_DISABLED_NATURAL_WONDER;
	int m_iQUEST_DISABLED_GIVE_GOLD;
	int m_iQUEST_DISABLED_PLEDGE_TO_PROTECT;
	int m_iQUEST_DISABLED_CONTEST_CULTURE;
	int m_iQUEST_DISABLED_CONTEST_FAITH;
	int m_iQUEST_DISABLED_CONTEST_TECHS;
	int m_iQUEST_DISABLED_INVEST;
	int m_iQUEST_DISABLED_BULLY_CITY_STATE;
	int m_iQUEST_DISABLED_DENOUNCE_MAJOR;
	int m_iQUEST_DISABLED_SPREAD_RELIGION;
	int m_iQUEST_DISABLED_TRADE_ROUTE;
	int m_iMINOR_CIV_QUEST_WEIGHT_DEFAULT;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_FRIENDLY_ROUTE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_ROUTE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_ROUTE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_ROUTE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CAMP;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_CONNECT_RESOURCE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_CONNECT_RESOURCE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_CONSTRUCT_WONDER;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_GREAT_PERSON;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_KILL_CITY_STATE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_KILL_CITY_STATE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_NEUTRAL_KILL_CITY_STATE;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MARITIME_FIND_PLAYER;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_FIND_PLAYER;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_FIND_NATURAL_WONDER;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_HOSTILE_FIND_NATURAL_WONDER;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_GIVE_GOLD;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MERCANTILE_GIVE_GOLD;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_GIVE_GOLD;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_MILITARISTIC_PLEDGE_TO_PROTECT;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_CULTURED_PLEDGE_TO_PROTECT;
	int m_iMINOR_CIV_QUEST_WEIGHT_MULTIPLIER_OTHER_PLEDGE_TO_PROTECT;
	int m_iRELIGION_BELIEF_SCORE_CITY_MULTIPLIER;
	int m_iRELIGION_BELIEF_SCORE_WORKED_PLOT_MULTIPLIER;
	int m_iRELIGION_BELIEF_SCORE_OWNED_PLOT_MULTIPLIER;
	int m_iRELIGION_BELIEF_SCORE_UNOWNED_PLOT_MULTIPLIER;
	int m_iRELIGION_MISSIONARY_RANGE_IN_TURNS;
	int m_iRELIGION_MAX_MISSIONARIES;
	int m_iMC_GIFT_WEIGHT_THRESHOLD;
	int m_iMC_ALWAYS_GIFT_DIPLO_THRESHOLD;
	int m_iMC_SOMETIMES_GIFT_RAND_MULTIPLIER;
	int m_iMC_SMALL_GIFT_WEIGHT_PASS_OTHER_PLAYER;
	int m_iMC_GIFT_WEIGHT_PASS_OTHER_PLAYER;
	int m_iMC_GIFT_WEIGHT_MARITIME_GROWTH;
	int m_iMC_GIFT_WEIGHT_DIPLO_VICTORY;
	int m_iMC_GIFT_WEIGHT_CULTURE_VICTORY;
	int m_iMC_GIFT_WEIGHT_CONQUEST_VICTORY;
	int m_iMC_GIFT_WEIGHT_MILITARISTIC;
	int m_iMC_GIFT_WEIGHT_RESOURCE_WE_NEED;
	int m_iMC_GIFT_WEIGHT_NEUTRAL_VICTORY_ROLL;
	int m_iMC_GIFT_WEIGHT_PROTECTIVE;
	int m_iMC_GIFT_WEIGHT_HOSTILE;
	int m_iMC_GIFT_WEIGHT_NEIGHBORS;
	int m_iMC_GIFT_WEIGHT_CLOSE;
	int m_iMC_GIFT_WEIGHT_FAR;
	int m_iMC_GIFT_WEIGHT_ALMOST_NOT_ALLIES;
	int m_iMC_GIFT_WEIGHT_ALMOST_NOT_FRIENDS;
	int m_iOPINION_WEIGHT_LAND_FIERCE;
	int m_iOPINION_WEIGHT_LAND_STRONG;
	int m_iOPINION_WEIGHT_LAND_WEAK;
	int m_iOPINION_WEIGHT_LAND_NONE;
	int m_iOPINION_WEIGHT_VICTORY_FIERCE;
	int m_iOPINION_WEIGHT_VICTORY_STRONG;
	int m_iOPINION_WEIGHT_VICTORY_WEAK;
	int m_iOPINION_WEIGHT_VICTORY_NONE;
	int m_iOPINION_WEIGHT_WONDER_FIERCE;
	int m_iOPINION_WEIGHT_WONDER_STRONG;
	int m_iOPINION_WEIGHT_WONDER_WEAK;
	int m_iOPINION_WEIGHT_WONDER_NONE;
	int m_iOPINION_WEIGHT_MINOR_CIV_FIERCE;
	int m_iOPINION_WEIGHT_MINOR_CIV_STRONG;
	int m_iOPINION_WEIGHT_MINOR_CIV_WEAK;
	int m_iOPINION_WEIGHT_MINOR_CIV_NONE;
	int m_iOPINION_WEIGHT_WARMONGER_CRITICAL;
	int m_iOPINION_WEIGHT_WARMONGER_SEVERE;
	int m_iOPINION_WEIGHT_WARMONGER_MAJOR;
	int m_iOPINION_WEIGHT_WARMONGER_MINOR;
	int m_iOPINION_WEIGHT_WARMONGER_NONE;
	int m_iOPINION_WEIGHT_ASKED_NO_SETTLE;
	int m_iOPINION_WEIGHT_ASKED_STOP_SPYING;
	int m_iOPINION_WEIGHT_MADE_DEMAND_OF_US;
	int m_iOPINION_WEIGHT_RETURNED_CIVILIAN;
	int m_iOPINION_WEIGHT_BUILT_LANDMARK;
	int m_iOPINION_WEIGHT_RESURRECTED;
	int m_iOPINION_WEIGHT_LIBERATED_ONE_CITY;
	int m_iOPINION_WEIGHT_LIBERATED_TWO_CITIES;
	int m_iOPINION_WEIGHT_LIBERATED_THREE_CITIES;
	int m_iOPINION_WEIGHT_EMBASSY;
	int m_iOPINION_WEIGHT_CULTURE_BOMBED;
	int m_iOPINION_WEIGHT_PER_NEGATIVE_CONVERSION;
	int m_iOPINION_WEIGHT_ADOPTING_HIS_RELIGION;
	int m_iOPINION_WEIGHT_ADOPTING_MY_RELIGION;
	int m_iOPINION_WEIGHT_SAME_LATE_POLICIES;
	int m_iOPINION_WEIGHT_DIFFERENT_LATE_POLICIES;
	int m_iOPINION_WEIGHT_BROKEN_MILITARY_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_MILITARY_PROMISE_WORLD;
	int m_iOPINION_WEIGHT_IGNORED_MILITARY_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_EXPANSION_PROMISE;
	int m_iOPINION_WEIGHT_IGNORED_EXPANSION_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_BORDER_PROMISE;
	int m_iOPINION_WEIGHT_IGNORED_BORDER_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE_WORLD;
	int m_iOPINION_WEIGHT_IGNORED_CITY_STATE_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_SPY_PROMISE;
	int m_iOPINION_WEIGHT_IGNORED_SPY_PROMISE;
	int m_iOPINION_WEIGHT_FORGAVE_FOR_SPYING;
	int m_iOPINION_WEIGHT_BROKEN_NO_CONVERT_PROMISE;
	int m_iOPINION_WEIGHT_IGNORED_NO_CONVERT_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_NO_DIG_PROMISE;
	int m_iOPINION_WEIGHT_IGNORED_NO_DIG_PROMISE;
	int m_iOPINION_WEIGHT_BROKEN_COOP_WAR_PROMISE;
	int m_iOPINION_WEIGHT_KILLED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	int m_iOPINION_WEIGHT_KILLED_PROTECTED_MINOR;
	int m_iOPINION_WEIGHT_KILLED_MANY_PROTECTED_MINORS;
	int m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	int m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY_NUM_TURNS;
	int m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY;
	int m_iOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_WHILE_AGO;
	int m_iOPINION_WEIGHT_ATTACKED_MANY_PROTECTED_MINORS;
	int m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	int m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY_NUM_TURNS;
	int m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY;
	int m_iOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_WHILE_AGO;
	int m_iOPINION_WEIGHT_BULLIED_MANY_PROTECTED_MINORS;
	int m_iOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_NUM_TURNS_UNTIL_FORGIVEN;
	int m_iOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR;
	int m_iOPINION_WEIGHT_DOF;
	int m_iOPINION_WEIGHT_DOF_WITH_FRIEND;
	int m_iOPINION_WEIGHT_DOF_WITH_ENEMY;
	int m_iOPINION_WEIGHT_DENOUNCED_BY_FRIEND_EACH;
	int m_iOPINION_WEIGHT_DENOUNCED_BY_FRIEND_DONT_LIKE;
	int m_iOPINION_WEIGHT_DENOUNCED_FRIEND_EACH;
	int m_iOPINION_WEIGHT_DENOUNCED_ME_FRIENDS;
	int m_iOPINION_WEIGHT_WAR_FRIEND_EACH;
	int m_iOPINION_WEIGHT_WAR_ME_FRIENDS;
	int m_iOPINION_WEIGHT_REFUSED_REQUEST_EACH;
	int m_iOPINION_WEIGHT_DENOUNCED_ME;
	int m_iOPINION_WEIGHT_DENOUNCED_THEM;
	int m_iOPINION_WEIGHT_DENOUNCED_FRIEND;
	int m_iOPINION_WEIGHT_DENOUNCED_ENEMY;
	int m_iOPINION_WEIGHT_RECKLESS_EXPANDER;
	int m_iOPINION_WEIGHT_TRADE_MAX;
	int m_iOPINION_WEIGHT_COMMON_FOE_MAX;
	int m_iOPINION_WEIGHT_ASSIST_MAX;
	int m_iOPINION_WEIGHT_LIBERATED_CAPITAL;
	int m_iOPINION_WEIGHT_LIBERATED_CITY;
	int m_iOPINION_WEIGHT_GAVE_ASSISTANCE;
	int m_iOPINION_WEIGHT_PAID_TRIBUTE;
	int m_iOPINION_WEIGHT_NUKED_MAX;
	int m_iOPINION_WEIGHT_ROBBED_BY;
	int m_iOPINION_WEIGHT_INTRIGUE_SHARED_BY;
	int m_iOPINION_WEIGHT_CAPTURED_CAPITAL;
#define CV_GLOBAL_DEFINE(kind, name, initial) GD_##kind##_DECL(name);
#include "CvGlobalDefines.inc"
#undef CV_GLOBAL_DEFINE
#if !defined(LEKMOD_PLAYER_GOLDEN_AGE_YIELD_MOD_INFO)
	int m_iGOLDEN_AGE_CULTURE_MODIFIER;
#endif
	int m_iHILLS_EXTRA_MOVEMENT;
	int m_iRIVER_EXTRA_MOVEMENT;
	int m_iFEATURE_GROWTH_MODIFIER;
	int m_iROUTE_FEATURE_GROWTH_MODIFIER;
	int m_iEXTRA_YIELD;
	int m_iFORTIFY_MODIFIER_PER_TURN;
	int m_iMAX_FORTIFY_TURNS;
	int m_iNUKE_FALLOUT_PROB;
	int m_iNUKE_UNIT_DAMAGE_BASE;
	int m_iNUKE_UNIT_DAMAGE_RAND_1;
	int m_iNUKE_UNIT_DAMAGE_RAND_2;
	int m_iNUKE_NON_COMBAT_DEATH_THRESHOLD;
	int m_iNUKE_LEVEL1_POPULATION_DEATH_BASE;
	int m_iNUKE_LEVEL1_POPULATION_DEATH_RAND_1;
	int m_iNUKE_LEVEL1_POPULATION_DEATH_RAND_2;
	int m_iNUKE_LEVEL2_POPULATION_DEATH_BASE;
	int m_iNUKE_LEVEL2_POPULATION_DEATH_RAND_1;
	int m_iNUKE_LEVEL2_POPULATION_DEATH_RAND_2;
	int m_iNUKE_LEVEL2_ELIM_POPULATION_THRESHOLD;
	int m_iNUKE_CITY_HIT_POINT_DAMAGE;
	int m_iNUKE_BLAST_RADIUS;
	int m_iTECH_COST_EXTRA_TEAM_MEMBER_MODIFIER;
	int m_iTECH_COST_TOTAL_KNOWN_TEAM_MODIFIER;
	int m_iTECH_COST_KNOWN_PREREQ_MODIFIER;
	int m_iPEACE_TREATY_LENGTH;
	int m_iCOOP_WAR_LOCKED_LENGTH;
	int m_iBASE_FEATURE_PRODUCTION_PERCENT;
	int m_iFEATURE_PRODUCTION_PERCENT_MULTIPLIER;
	int m_iDIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT;
	int m_iDEFAULT_WAR_VALUE_FOR_UNIT;
	int m_iUNIT_PRODUCTION_PERCENT;
	int m_iMAX_UNIT_SUPPLY_PRODMOD;
	int m_iBUILDING_PRODUCTION_PERCENT;
	int m_iPROJECT_PRODUCTION_PERCENT;
	int m_iMAXED_UNIT_GOLD_PERCENT;
	int m_iMAXED_BUILDING_GOLD_PERCENT;
	int m_iMAXED_PROJECT_GOLD_PERCENT;
	int m_iMAX_CITY_DEFENSE_DAMAGE;
	int m_iCIRCUMNAVIGATE_FREE_MOVES;
	int m_iBASE_CAPTURE_GOLD;
	int m_iCAPTURE_GOLD_PER_POPULATION;
	int m_iCAPTURE_GOLD_RAND1;
	int m_iCAPTURE_GOLD_RAND2;
	int m_iCAPTURE_GOLD_MAX_TURNS;
	int m_iBARBARIAN_CITY_ATTACK_MODIFIER;
	int m_iBUILDING_PRODUCTION_DECAY_TIME;
	int m_iBUILDING_PRODUCTION_DECAY_PERCENT;
	int m_iUNIT_PRODUCTION_DECAY_TIME;
	int m_iUNIT_PRODUCTION_DECAY_PERCENT;
	int m_iBASE_UNIT_UPGRADE_COST;
	int m_iUNIT_UPGRADE_COST_PER_PRODUCTION;
	int m_iUNIT_UPGRADE_COST_VISIBLE_DIVISOR;
	int m_iUNIT_UPGRADE_COST_DISCOUNT_MAX;
	int m_iWAR_SUCCESS_UNIT_CAPTURING;
	int m_iWAR_SUCCESS_CITY_CAPTURING;
	int m_iDIPLO_VOTE_SECRETARY_GENERAL_INTERVAL;
	int m_iTEAM_VOTE_MIN_CANDIDATES;
	int m_iRESEARCH_AGREEMENT_TIMER;
	int m_iRESEARCH_AGREEMENT_BOOST_DIVISOR;
	int m_iSCORE_POPULATION_FACTOR;
	int m_iSCORE_LAND_FACTOR;
	int m_iSCORE_WONDER_FACTOR;
	int m_iSCORE_TECH_FACTOR;
	int m_iSCORE_FREE_PERCENT;
	int m_iSCORE_VICTORY_PERCENT;
	int m_iSCORE_HANDICAP_PERCENT_OFFSET;
	int m_iSCORE_HANDICAP_PERCENT_PER;
	int m_iMINIMAP_RENDER_SIZE;
	int m_iMAX_INTERCEPTION_PROBABILITY;
	int m_iMAX_EVASION_PROBABILITY;
	int m_iPLAYER_ALWAYS_RAZES_CITIES;
	int m_iMIN_WATER_SIZE_FOR_OCEAN;
	int m_iCITY_SCREEN_CLICK_WILL_EXIT;
	int m_iWATER_POTENTIAL_CITY_WORK_FOR_AREA;
	int m_iLAND_UNITS_CAN_ATTACK_WATER_CITIES;
	int m_iCITY_MAX_NUM_BUILDINGS;
	int m_iCITY_MIN_SIZE_FOR_SETTLERS;
	int m_iRANGED_ATTACKS_USE_MOVES;
	int m_iADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES;
	int m_iADVANCED_START_MAX_UNITS_PER_CITY;
	int m_iADVANCED_START_CITY_COST;
	int m_iADVANCED_START_CITY_COST_INCREASE;
	int m_iADVANCED_START_POPULATION_COST;
	int m_iADVANCED_START_POPULATION_COST_INCREASE;
	int m_iADVANCED_START_VISIBILITY_COST;
	int m_iADVANCED_START_VISIBILITY_COST_INCREASE;
	int m_iADVANCED_START_CITY_PLACEMENT_MAX_RANGE;
	int m_iNEW_CITY_BUILDING_VALUE_MODIFIER;
	int m_iPATH_DAMAGE_WEIGHT;
#if !defined(LEKMOD_PUPPET_YIELD_MOD_INFO)
	int m_iPUPPET_SCIENCE_MODIFIER;
	int m_iPUPPET_CULTURE_MODIFIER;
	int m_iPUPPET_GOLD_MODIFIER;
	int m_iPUPPET_FAITH_MODIFIER;
#endif
	int m_iBASE_POLICY_COST;
	int m_iPOLICY_COST_INCREASE_TO_BE_EXPONENTED;
	int m_iPOLICY_COST_VISIBLE_DIVISOR;
	int m_iSWITCH_POLICY_BRANCHES_ANARCHY_TURNS;
	int m_iSWITCH_POLICY_BRANCHES_TENETS_LOST;
	int m_iPOLICY_COST_DISCOUNT_MAX;
	int m_iGOLD_PURCHASE_GOLD_PER_PRODUCTION;
	int m_iGOLD_PURCHASE_VISIBLE_DIVISOR;
	int m_iPROJECT_PURCHASING_DISABLED;
	int m_iINFLUENCE_MOUNTAIN_COST;
	int m_iINFLUENCE_HILL_COST;
	int m_iINFLUENCE_RIVER_COST;
	int m_iUSE_FIRST_RING_INFLUENCE_TERRAIN_COST;
	int m_iNUM_RESOURCE_QUANTITY_TYPES;
	int m_iSPECIALISTS_DIVERT_POPULATION_ENABLED;
	int m_iSCIENCE_PER_POPULATION;
	int m_iRESEARCH_AGREEMENT_MOD;
	int m_iBARBARIAN_CAMP_FIRST_TURN_PERCENT_OF_TARGET_TO_ADD;
	int m_iBARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING;
	int m_iBARBARIAN_CAMP_MINIMUM_DISTANCE_CAPITAL;
	int m_iBARBARIAN_CAMP_MINIMUM_DISTANCE_ANOTHER_CAMP;
	int m_iBARBARIAN_CAMP_COASTAL_SPAWN_ROLL;
	int m_iBARBARIAN_EXTRA_RAGING_UNIT_SPAWN_CHANCE;
	int m_iBARBARIAN_NAVAL_UNIT_START_TURN_SPAWN;
	int m_iMAX_BARBARIANS_FROM_CAMP_NEARBY;
	int m_iMAX_BARBARIANS_FROM_CAMP_NEARBY_RANGE;
	int m_iGOLD_FROM_BARBARIAN_CONVERSION;
	int m_iBARBARIAN_CITY_GOLD_RANSOM;
	int m_iBARBARIAN_UNIT_GOLD_RANSOM;
	int m_iEMBARKED_UNIT_MOVEMENT;
	int m_iEMBARKED_VISIBILITY_RANGE;
	int m_iDEFAULT_MAX_NUM_BUILDERS;
	int m_iBARBARIAN_TECH_PERCENT;
	int m_iCITY_RESOURCE_WLTKD_TURNS;
	int m_iMAX_SPECIALISTS_FROM_BUILDING;
	int m_iGREAT_PERSON_THRESHOLD_BASE;
	int m_iGREAT_PERSON_THRESHOLD_INCREASE;
	int m_iCULTURE_BOMB_COOLDOWN;
	int m_iCULTURE_BOMB_MINOR_FRIENDSHIP_CHANGE;
	int m_iLANDMARK_MINOR_FRIENDSHIP_CHANGE;
	int m_iUNIT_AUTO_EXPLORE_DISABLED;
	int m_iUNIT_AUTO_EXPLORE_FULL_DISABLED;
	int m_iUNIT_WORKER_AUTOMATION_DISABLED;
	int m_iUNIT_DELETE_DISABLED;
	int m_iMIN_START_AREA_TILES;
	int m_iMIN_DISTANCE_OTHER_AREA_PERCENT;
	int m_iMINOR_CIV_FOOD_REQUIREMENT;
	int m_iMAJOR_CIV_FOOD_REQUIREMENT;
	int m_iMIN_START_FOUND_VALUE_AS_PERCENT_OF_BEST;
	int m_iSTART_AREA_FOOD_MULTIPLIER;
	int m_iSTART_AREA_HAPPINESS_MULTIPLIER;
	int m_iSTART_AREA_PRODUCTION_MULTIPLIER;
	int m_iSTART_AREA_GOLD_MULTIPLIER;
	int m_iSTART_AREA_SCIENCE_MULTIPLIER;
#ifdef AUI_SITE_EVALUATION_PLOT_FOUND_VALUE_CONSIDER_CULTURE
	GD_INT_DECL(START_AREA_CULTURE_MULTIPLIER);
#endif
	int m_iSTART_AREA_FAITH_MULTIPLIER;
	int m_iSTART_AREA_RESOURCE_MULTIPLIER;
	int m_iSTART_AREA_STRATEGIC_MULTIPLIER;
	int m_iSTART_AREA_BUILD_ON_COAST_PERCENT;
	int m_iSETTLER_FOOD_MULTIPLIER;
	int m_iSETTLER_HAPPINESS_MULTIPLIER;
	int m_iSETTLER_PRODUCTION_MULTIPLIER;
	int m_iSETTLER_GOLD_MULTIPLIER;
	int m_iSETTLER_SCIENCE_MULTIPLIER;
#ifdef AUI_SITE_EVALUATION_PLOT_FOUND_VALUE_CONSIDER_CULTURE
	GD_INT_DECL(SETTLER_CULTURE_MULTIPLIER);
#endif
	int m_iSETTLER_FAITH_MULTIPLIER;
	int m_iSETTLER_RESOURCE_MULTIPLIER;
	int m_iSETTLER_STRATEGIC_MULTIPLIER;
	int m_iSETTLER_BUILD_ON_COAST_PERCENT;
#ifdef AUI_SITE_EVALUATION_COMPUTE_YIELD_VALUE_RECOGNIZE_CITY_PLOT
	GD_INT_DECL(CITY_RING_0_MULTIPLIER);
#endif
	int m_iCITY_RING_1_MULTIPLIER;
	int m_iCITY_RING_2_MULTIPLIER;
	int m_iCITY_RING_3_MULTIPLIER;
	int m_iCITY_RING_4_MULTIPLIER;
	int m_iCITY_RING_5_MULTIPLIER;
#ifdef AUI_SITE_EVALUATION_YIELD_MULTIPLIER_DISTANCE_DECAY
	GD_INT_DECL(SETTLER_FOOD_RING_0_DIVIDER);
	GD_INT_DECL(SETTLER_PRODUCTION_RING_0_DIVIDER);
	GD_INT_DECL(SETTLER_GOLD_RING_0_DIVIDER);
	GD_INT_DECL(SETTLER_SCIENCE_RING_0_DIVIDER);
	GD_INT_DECL(SETTLER_CULTURE_RING_0_DIVIDER);
	GD_INT_DECL(SETTLER_FAITH_RING_0_DIVIDER);
	GD_INT_DECL(SETTLER_HAPPINESS_RING_0_DIVIDER);
	GD_INT_DECL(SETTLER_RESOURCES_RING_0_DIVIDER);
	GD_INT_DECL(SETTLER_STRATEGIC_RING_0_DIVIDER);
	GD_INT_DECL(SETTLER_FOOD_RING_1_DIVIDER);
	GD_INT_DECL(SETTLER_PRODUCTION_RING_1_DIVIDER);
	GD_INT_DECL(SETTLER_GOLD_RING_1_DIVIDER);
	GD_INT_DECL(SETTLER_SCIENCE_RING_1_DIVIDER);
	GD_INT_DECL(SETTLER_CULTURE_RING_1_DIVIDER);
	GD_INT_DECL(SETTLER_FAITH_RING_1_DIVIDER);
	GD_INT_DECL(SETTLER_HAPPINESS_RING_1_DIVIDER);
	GD_INT_DECL(SETTLER_RESOURCES_RING_1_DIVIDER);
	GD_INT_DECL(SETTLER_STRATEGIC_RING_1_DIVIDER);
	GD_INT_DECL(SETTLER_FOOD_RING_2_DIVIDER);
	GD_INT_DECL(SETTLER_PRODUCTION_RING_2_DIVIDER);
	GD_INT_DECL(SETTLER_GOLD_RING_2_DIVIDER);
	GD_INT_DECL(SETTLER_SCIENCE_RING_2_DIVIDER);
	GD_INT_DECL(SETTLER_CULTURE_RING_2_DIVIDER);
	GD_INT_DECL(SETTLER_FAITH_RING_2_DIVIDER);
	GD_INT_DECL(SETTLER_HAPPINESS_RING_2_DIVIDER);
	GD_INT_DECL(SETTLER_RESOURCES_RING_2_DIVIDER);
	GD_INT_DECL(SETTLER_STRATEGIC_RING_2_DIVIDER);
	GD_INT_DECL(SETTLER_FOOD_RING_3_DIVIDER);
	GD_INT_DECL(SETTLER_PRODUCTION_RING_3_DIVIDER);
	GD_INT_DECL(SETTLER_GOLD_RING_3_DIVIDER);
	GD_INT_DECL(SETTLER_SCIENCE_RING_3_DIVIDER);
	GD_INT_DECL(SETTLER_CULTURE_RING_3_DIVIDER);
	GD_INT_DECL(SETTLER_FAITH_RING_3_DIVIDER);
	GD_INT_DECL(SETTLER_HAPPINESS_RING_3_DIVIDER);
	GD_INT_DECL(SETTLER_RESOURCES_RING_3_DIVIDER);
	GD_INT_DECL(SETTLER_STRATEGIC_RING_3_DIVIDER);
	GD_INT_DECL(SETTLER_HAPPINESS_RING_4_DIVIDER);
	GD_INT_DECL(SETTLER_RESOURCES_RING_4_DIVIDER);
	GD_INT_DECL(SETTLER_STRATEGIC_RING_4_DIVIDER);
	GD_INT_DECL(SETTLER_HAPPINESS_RING_5_DIVIDER);
	GD_INT_DECL(SETTLER_RESOURCES_RING_5_DIVIDER);
	GD_INT_DECL(SETTLER_STRATEGIC_RING_5_DIVIDER);
#endif
	int m_iSETTLER_EVALUATION_DISTANCE;
	int m_iSETTLER_DISTANCE_DROPOFF_MODIFIER;
	int m_iBUILD_ON_RESOURCE_PERCENT;
	int m_iBUILD_ON_RIVER_PERCENT;
	int m_iCHOKEPOINT_STRATEGIC_VALUE;
	int m_iHILL_STRATEGIC_VALUE;
	int m_iALREADY_OWNED_STRATEGIC_VALUE;
	int m_iMINOR_CIV_CONTACT_GOLD_FIRST;
	int m_iMINOR_CIV_CONTACT_GOLD_OTHER;
	int m_iMINOR_CIV_GROWTH_PERCENT;
	int m_iMINOR_CIV_PRODUCTION_PERCENT;
	int m_iMINOR_CIV_GOLD_PERCENT;
	int m_iMINOR_CIV_TECH_PERCENT;
	int m_iMINOR_POLICY_RESOURCE_MULTIPLIER;
	int m_iMINOR_POLICY_RESOURCE_HAPPINESS_MULTIPLIER;
	int m_iMINOR_GOLD_GIFT_LARGE;
	int m_iMINOR_GOLD_GIFT_MEDIUM;
	int m_iMINOR_GOLD_GIFT_SMALL;
	int m_iMINOR_CIV_TILE_IMPROVEMENT_GIFT_COST;
	int m_iMINOR_CIV_BUYOUT_COST;
	int m_iMINOR_CIV_BUYOUT_TURNS;
	int m_iMINOR_FRIENDSHIP_FROM_TRADE_MISSION;
	int m_iMINOR_FRIENDSHIP_ANCHOR_DEFAULT;
	int m_iMINOR_FRIENDSHIP_ANCHOR_MOD_PROTECTED;
	int m_iMINOR_FRIENDSHIP_ANCHOR_MOD_WARY_OF;
	int m_iMINOR_UNIT_GIFT_TRAVEL_TURNS;
	int m_iPLOT_UNIT_LIMIT;
	int m_iZONE_OF_CONTROL_ENABLED;
	int m_iFIRE_SUPPORT_DISABLED;
	int m_iMAX_HIT_POINTS;
	int m_iMAX_CITY_HIT_POINTS;
	int m_iCITY_HIT_POINTS_HEALED_PER_TURN;
	int m_iFLAT_LAND_EXTRA_DEFENSE;
	int m_iHILLS_EXTRA_DEFENSE;
	int m_iRIVER_ATTACK_MODIFIER;
	int m_iAMPHIB_ATTACK_MODIFIER;
	int m_iENEMY_HEAL_RATE;
	int m_iNEUTRAL_HEAL_RATE;
	int m_iFRIENDLY_HEAL_RATE;
	int m_iINSTA_HEAL_RATE;
	int m_iCITY_HEAL_RATE;
	int m_iATTACK_SAME_STRENGTH_MIN_DAMAGE;
	int m_iRANGE_ATTACK_RANGED_DEFENDER_MOD;
	int m_iATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE;
	int m_iRANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE;
	int m_iRANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE;
	int m_iAIR_STRIKE_SAME_STRENGTH_MIN_DEFENSE_DAMAGE;
	int m_iAIR_STRIKE_SAME_STRENGTH_POSSIBLE_EXTRA_DEFENSE_DAMAGE;
	int m_iINTERCEPTION_SAME_STRENGTH_MIN_DAMAGE;
	int m_iINTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE;
	int m_iAIR_SWEEP_INTERCEPTION_DAMAGE_MOD;
	int m_iWOUNDED_DAMAGE_MULTIPLIER;
	int m_iTRAIT_WOUNDED_DAMAGE_MOD;
	int m_iCITY_STRENGTH_DEFAULT;
	int m_iCITY_STRENGTH_POPULATION_CHANGE;
	int m_iCITY_STRENGTH_UNIT_DIVISOR;
	int m_iCITY_STRENGTH_HILL_CHANGE;
	int m_iCITY_ATTACKING_DAMAGE_MOD;
	int m_iATTACKING_CITY_MELEE_DAMAGE_MOD;
	int m_iCITY_ATTACK_RANGE;
	int m_iCAN_CITY_USE_INDIRECT_FIRE;
	int m_iCITY_RANGED_ATTACK_STRENGTH_MULTIPLIER;
	int m_iMIN_CITY_STRIKE_DAMAGE;
	int m_iCITY_CAPTURE_DAMAGE_PERCENT;
	int m_iEXPERIENCE_PER_LEVEL;
	int m_iEXPERIENCE_ATTACKING_UNIT_MELEE;
	int m_iEXPERIENCE_DEFENDING_UNIT_MELEE;
	int m_iEXPERIENCE_ATTACKING_UNIT_AIR;
	int m_iEXPERIENCE_DEFENDING_UNIT_AIR;
	int m_iEXPERIENCE_ATTACKING_UNIT_RANGED;
	int m_iEXPERIENCE_DEFENDING_UNIT_RANGED;
	int m_iEXPERIENCE_ATTACKING_AIR_SWEEP;
	int m_iEXPERIENCE_DEFENDING_AIR_SWEEP_AIR;
	int m_iEXPERIENCE_DEFENDING_AIR_SWEEP_GROUND;
	int m_iEXPERIENCE_ATTACKING_CITY_MELEE;
	int m_iEXPERIENCE_ATTACKING_CITY_RANGED;
	int m_iEXPERIENCE_ATTACKING_CITY_AIR;
	int m_iBARBARIAN_MAX_XP_VALUE;
	int m_iCOMBAT_EXPERIENCE_IN_BORDERS_PERCENT;
	int m_iGREAT_GENERALS_THRESHOLD_INCREASE;
	int m_iGREAT_GENERALS_THRESHOLD_INCREASE_TEAM;
	int m_iGREAT_GENERALS_THRESHOLD;
	int m_iUNIT_DEATH_XP_GREAT_GENERAL_LOSS;
	int m_iMIN_EXPERIENCE_PER_COMBAT;
	int m_iMAX_EXPERIENCE_PER_COMBAT;
	int m_iCRAMPED_RANGE_FROM_CITY;
	int m_iCRAMPED_USABLE_PLOT_PERCENT;
	int m_iPROXIMITY_NEIGHBORS_CLOSEST_CITY_REQUIREMENT;
	int m_iPROXIMITY_CLOSE_CLOSEST_CITY_POSSIBILITY;
	int m_iPROXIMITY_CLOSE_DISTANCE_MAP_MULTIPLIER;
	int m_iPROXIMITY_CLOSE_DISTANCE_MAX;
	int m_iPROXIMITY_CLOSE_DISTANCE_MIN;
	int m_iPROXIMITY_FAR_DISTANCE_MAP_MULTIPLIER;
	int m_iPROXIMITY_FAR_DISTANCE_MAX;
	int m_iPROXIMITY_FAR_DISTANCE_MIN;
	int m_iPLOT_BASE_COST;
	int m_iPLOT_ADDITIONAL_COST_PER_PLOT;
	int m_iPLOT_COST_APPEARANCE_DIVISOR;
	int m_iCULTURE_COST_FIRST_PLOT;
	int m_iCULTURE_COST_LATER_PLOT_MULTIPLIER;
	int m_iCULTURE_COST_VISIBLE_DIVISOR;
	int m_iCULTURE_PLOT_COST_MOD_MINIMUM;
	int m_iMINOR_CIV_PLOT_CULTURE_COST_MULTIPLIER;
	int m_iMAXIMUM_BUY_PLOT_DISTANCE;
	int m_iMAXIMUM_ACQUIRE_PLOT_DISTANCE;
	int m_iPLOT_INFLUENCE_BASE_MULTIPLIER;
	int m_iPLOT_INFLUENCE_DISTANCE_MULTIPLIER;
	int m_iPLOT_INFLUENCE_DISTANCE_DIVISOR;
	int m_iPLOT_INFLUENCE_RING_COST;
	int m_iPLOT_INFLUENCE_WATER_COST;
	int m_iPLOT_INFLUENCE_IMPROVEMENT_COST;
	int m_iPLOT_INFLUENCE_ROUTE_COST;
	int m_iPLOT_INFLUENCE_RESOURCE_COST;
	int m_iPLOT_INFLUENCE_NW_COST;
	int m_iPLOT_BUY_RESOURCE_COST;
	int m_iPLOT_BUY_YIELD_COST;
	int m_iPLOT_INFLUENCE_YIELD_POINT_COST;
	int m_iPLOT_INFLUENCE_NO_ADJACENT_OWNED_COST;
	int m_iUNITED_NATIONS_COUNTDOWN_TURNS;
	int m_iOWN_UNITED_NATIONS_VOTE_BONUS;
	int m_iDIPLO_VICTORY_ALGORITHM_THRESHOLD;
	int m_iDIPLO_VICTORY_BEYOND_ALGORITHM_MULTIPLIER;
	int m_iDIPLO_VICTORY_DEFAULT_VOTE_PERCENT;
	int m_iSCORE_CITY_MULTIPLIER;
	int m_iSCORE_POPULATION_MULTIPLIER;
	int m_iSCORE_LAND_MULTIPLIER;
	int m_iSCORE_WONDER_MULTIPLIER;
	int m_iSCORE_TECH_MULTIPLIER;
	int m_iSCORE_FUTURE_TECH_MULTIPLIER;
	int m_iSCORE_POLICY_MULTIPLIER;
	int m_iSCORE_GREAT_WORK_MULTIPLIER;
	int m_iSCORE_BELIEF_MULTIPLIER;
	int m_iSCORE_RELIGION_CITIES_MULTIPLIER;
	int m_iVICTORY_POINTS_PER_ERA;
	int m_iMIN_GAME_TURNS_ELAPSED_TO_TEST_VICTORY;
	int m_iZERO_SUM_COMPETITION_WONDERS_VICTORY_POINTS;
	int m_iZERO_SUM_COMPETITION_POLICIES_VICTORY_POINTS;
	int m_iZERO_SUM_COMPETITION_GREAT_PEOPLE_VICTORY_POINTS;
	int m_iMAX_CITY_DIST_HIGHWATER_MARK;
	int m_iHEAVY_RESOURCE_THRESHOLD;
	int m_iPROGRESS_POPUP_TURN_FREQUENCY;
	int m_iSETTLER_PRODUCTION_SPEED;
	int m_iBUY_PLOTS_DISABLED;
	int m_iCITY_STRENGTH_TECH_MULTIPLIER;
	int m_iWARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT;
	int m_iBUILDING_SALE_DIVISOR;
	int m_iDISBAND_UNIT_REFUND_PERCENT;
	int m_iMINOR_CIV_ROUTE_QUEST_WEIGHT;
	int m_iWITHDRAW_MOD_ENEMY_MOVES;
	int m_iWITHDRAW_MOD_BLOCKED_TILE;
	int m_iAI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT;
	int m_iAI_OPERATIONAL_MAX_RECRUIT_TURNS_ENEMY_TERRITORY;
	int m_iRELIGION_MIN_FAITH_FIRST_PANTHEON;
	int m_iRELIGION_MIN_FAITH_FIRST_PROPHET;
	int m_iRELIGION_MIN_FAITH_FIRST_GREAT_PERSON;
	int m_iRELIGION_GAME_FAITH_DELTA_NEXT_PANTHEON;
	int m_iRELIGION_FAITH_DELTA_NEXT_PROPHET;
	int m_iRELIGION_FAITH_DELTA_NEXT_GREAT_PERSON;
	int m_iRELIGION_BASE_CHANCE_PROPHET_SPAWN;
	int m_iRELIGION_ATHEISM_PRESSURE_PER_POP;
	int m_iRELIGION_INITIAL_FOUNDING_CITY_PRESSURE;
	int m_iRELIGION_PER_TURN_FOUNDING_CITY_PRESSURE;
	int m_iRELIGION_MISSIONARY_PRESSURE_MULTIPLIER;
	int m_iRELIGION_ADJACENT_CITY_DISTANCE;
	int m_iRELIGION_DIPLO_HIT_INITIAL_CONVERT_FRIENDLY_CITY;
	int m_iRELIGION_DIPLO_HIT_RELIGIOUS_FLIP_FRIENDLY_CITY;
	int m_iRELIGION_DIPLO_HIT_CONVERT_HOLY_CITY;
	int m_iRELIGION_DIPLO_HIT_THRESHOLD;
	int m_iESPIONAGE_GATHERING_INTEL_COST_PERCENT;
	int m_iESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT;
	int m_iESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT;
	int m_iESPIONAGE_TURNS_BETWEEN_CITY_STATE_ELECTIONS;
	int m_iESPIONAGE_INFLUENCE_GAINED_FOR_RIGGED_ELECTION;
	int m_iESPIONAGE_INFLUENCE_LOST_FOR_RIGGED_ELECTION;
	int m_iESPIONAGE_SURVEILLANCE_SIGHT_RANGE;
	int m_iESPIONAGE_COUP_OTHER_PLAYERS_INFLUENCE_DROP;
	int m_iINTERNATIONAL_TRADE_BASE;
	int m_iINTERNATIONAL_TRADE_EXCLUSIVE_CONNECTION;
	int m_iINTERNATIONAL_TRADE_CITY_GPT_DIVISOR;
	int m_iCOMBAT_CAPTURE_HEALTH;
	int m_iCOMBAT_CAPTURE_MIN_CHANCE;
	int m_iCOMBAT_CAPTURE_MAX_CHANCE;
	int m_iCOMBAT_CAPTURE_RATIO_MULTIPLIER;

	// -- floats --

	float m_fDIPLO_VICTORY_CIV_DELEGATES_COEFFICIENT;
	float m_fDIPLO_VICTORY_CIV_DELEGATES_CONSTANT;
	float m_fDIPLO_VICTORY_CS_DELEGATES_COEFFICIENT;
	float m_fDIPLO_VICTORY_CS_DELEGATES_CONSTANT;
	float m_fAI_PRODUCTION_WEIGHT_MOD_PER_TURN_LEFT;
	float m_fAI_PRODUCTION_WEIGHT_BASE_MOD;
	float m_fAI_RESEARCH_WEIGHT_MOD_PER_TURN_LEFT;
	float m_fAI_RESEARCH_WEIGHT_BASE_MOD;
	float m_fHURRY_GOLD_PRODUCTION_EXPONENT;
	float m_fCITY_ZOOM_LEVEL_1;
	float m_fCITY_ZOOM_LEVEL_2;
	float m_fCITY_ZOOM_LEVEL_3;
	float m_fCITY_ZOOM_OFFSET;
	float m_fCULTURE_COST_LATER_PLOT_EXPONENT;
	float m_fPOST_COMBAT_TEXT_DELAY;
	float m_fAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEXT_WONDER;
	float m_fAI_DANGER_MAJOR_APPROACH_WAR;
	float m_fAI_DANGER_MAJOR_APPROACH_HOSTILE;
	float m_fAI_DANGER_MAJOR_APPROACH_DECEPTIVE;
	float m_fAI_DANGER_MAJOR_APPROACH_GUARDED;
	float m_fAI_DANGER_MAJOR_APPROACH_AFRAID;
	float m_fAI_DANGER_MAJOR_APPROACH_FRIENDLY;
	float m_fAI_DANGER_MAJOR_APPROACH_NEUTRAL;
	float m_fAI_DANGER_MINOR_APPROACH_NEUTRAL;
	float m_fAI_DANGER_MINOR_APPROACH_FRIENDLY;
	float m_fAI_DANGER_MINOR_APPROACH_BULLY;
	float m_fAI_DANGER_MINOR_APPROACH_CONQUEST;
	float m_fFLAVOR_STANDARD_LOG10_TILES_PER_PLAYER;
	float m_fDIPLO_VICTORY_TEAM_MULTIPLIER;
	float m_fAI_TACTICAL_FLAVOR_DAMPENING_FOR_MOVE_PRIORITIZATION;
	float m_fAI_STRATEGY_DEFEND_MY_LANDS_UNITS_PER_CITY;
	float m_fAI_STRATEGY_NAVAL_UNITS_PER_CITY;
	float m_fGOLD_GIFT_FRIENDSHIP_EXPONENT;
	float m_fGOLD_GIFT_FRIENDSHIP_DIVISOR;
	float m_fHURRY_GOLD_TECH_EXPONENT;
	float m_fHURRY_GOLD_CULTURE_EXPONENT;
	float m_fCITY_GROWTH_MULTIPLIER;
	float m_fCITY_GROWTH_EXPONENT;
	float m_fPOLICY_COST_EXPONENT;
	float m_fUNIT_UPGRADE_COST_MULTIPLIER_PER_ERA;
	float m_fUNIT_UPGRADE_COST_EXPONENT;
	float m_fCITY_STRENGTH_TECH_BASE;
	float m_fCITY_STRENGTH_TECH_EXPONENT;
	float m_fESPIONAGE_COUP_NOBODY_BONUS;
	float m_fESPIONAGE_COUP_MULTIPLY_CONSTANT;
	float m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_ZERO;
	float m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_ONE;
	float m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_TWO;
	float m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_THREE;
	float m_fESPIONAGE_COUP_SPY_LEVEL_DELTA_FOUR;
	int   m_iPILLAGE_HEAL_AMOUNT;
	int   m_iCITY_CONNECTIONS_CONNECT_TO_CAPITAL;
	float m_fNORMAL_ANNEX;
	float m_fAGGRESSIVE_ANNEX;
	float m_fUNRESEARCHED_TECH_FROM_KILLS_SLOPE;
	float m_fUNRESEARCHED_TECH_FROM_KILLS_INTERCEPT;
	int   m_iBASE_CULTURE_PER_GREAT_WORK;
	int   m_iBASE_TOURISM_PER_GREAT_WORK;
	int   m_iTOURISM_MODIFIER_SHARED_RELIGION;
	int   m_iTOURISM_MODIFIER_TRADE_ROUTE;
	int   m_iTOURISM_MODIFIER_OPEN_BORDERS;
	int   m_iTOURISM_MODIFIER_DIFFERENT_IDEOLOGIES;
	int   m_iTOURISM_MODIFIER_DIPLOMAT;
	int   m_iMINIUMUM_TOURISM_BLAST_STRENGTH;
	int   m_iCULTURE_LEVEL_EXOTIC;
	int   m_iCULTURE_LEVEL_FAMILIAR;
	int   m_iCULTURE_LEVEL_POPULAR;
	int   m_iCULTURE_LEVEL_INFLUENTIAL;
	int   m_iCULTURE_LEVEL_DOMINANT;
#ifdef LEKMOD_TOURISM_COMBAT_MOD
	int   m_iLekmodTourismCombatMinInfluencePercent;
	int   m_iLekmodTourismCombatMaxInfluencePercent;
	int   m_iLekmodTourismCombatMinBonus;
	int   m_iLekmodTourismCombatMaxBonus;
	bool  m_bLekmodTourismCombatIdeologyRequired;
#endif
	int   m_iMIN_DIG_SITES_PER_MAJOR_CIV;
	int   m_iMAX_DIG_SITES_PER_MAJOR_CIV;
	int   m_iPERCENT_SITES_HIDDEN;
	int   m_iPERCENT_HIDDEN_SITES_WRITING;
	int   m_iSAPPED_CITY_ATTACK_MODIFIER;
	int   m_iSAPPER_BONUS_RANGE;
	int   m_iEXOTIC_GOODS_GOLD_MIN;
	int   m_iEXOTIC_GOODS_GOLD_MAX;
	int   m_iEXOTIC_GOODS_XP_MIN;
	int   m_iEXOTIC_GOODS_XP_MAX;
	int   m_iTEMPORARY_CULTURE_BOOST_MOD;
	int   m_iTEMPORARY_TOURISM_BOOST_MOD;
	int   m_iLEAGUE_SESSION_INTERVAL_BASE_TURNS;
	int   m_iLEAGUE_SESSION_SOON_WARNING_TURNS;
	int   m_iLEAGUE_MEMBER_PROPOSALS_BASE;
	int   m_iLEAGUE_MEMBER_VOTES_BASE;
	int   m_iLEAGUE_MEMBER_VOTES_FOR_HOST;
	int   m_iLEAGUE_MEMBER_VOTES_PER_CITY_STATE_ALLY;
	float m_fLEAGUE_PROJECT_REWARD_TIER_1_THRESHOLD;
	float m_fLEAGUE_PROJECT_REWARD_TIER_2_THRESHOLD;
	int   m_iPROMOTION_GOODY_HUT_PICKER;
	int   m_iIDEOLOGY_PERCENT_CLEAR_VICTORY_PREF;
	int   m_iIDEOLOGY_SCORE_GRAND_STRATS;
	int   m_iIDEOLOGY_SCORE_HAPPINESS;
	int   m_iIDEOLOGY_SCORE_PER_FREE_TENET;
	int   m_iIDEOLOGY_SCORE_HOSTILE;
	int   m_iIDEOLOGY_SCORE_GUARDED;
	int   m_iIDEOLOGY_SCORE_AFRAID;
	int   m_iIDEOLOGY_SCORE_FRIENDLY;
	int   m_iPOLICY_BRANCH_FREEDOM;
	int   m_iPOLICY_BRANCH_AUTOCRACY;
	int   m_iPOLICY_BRANCH_ORDER;

	// -- post defines --

	int m_iLAND_TERRAIN;
	int m_iDEEP_WATER_TERRAIN;
	int m_iSHALLOW_WATER_TERRAIN;
	int m_iRUINS_IMPROVEMENT;
	int m_iNUKE_FEATURE;
	int m_iARTIFACT_RESOURCE;
	int m_iHIDDEN_ARTIFACT_RESOURCE;
	int m_iCAPITAL_BUILDINGCLASS;
	int m_iDEFAULT_SPECIALIST;
	int m_iSPACE_RACE_TRIGGER_PROJECT;
	int m_iSPACESHIP_CAPSULE;
	int m_iSPACESHIP_BOOSTER;
	int m_iSPACESHIP_STASIS;
	int m_iSPACESHIP_ENGINE;
	int m_iMANHATTAN_PROJECT;
	int m_iINITIAL_CITY_ROUTE_TYPE;
	int m_iSTANDARD_HANDICAP;
	int m_iMULTIPLAYER_HANDICAP;
	int m_iSTANDARD_HANDICAP_QUICK;
	int m_iSTANDARD_GAMESPEED;
	int m_iSTANDARD_TURNTIMER;
	int m_iSTANDARD_CLIMATE;
	int m_iSTANDARD_WORLD_SIZE;
	int m_iSTANDARD_SEALEVEL;
	int m_iSTANDARD_ERA;
	int m_iLAST_EMBARK_ART_ERA;
	int m_iLAST_UNIT_ART_ERA;
	int m_iLAST_BRIDGE_ART_ERA;
	int m_iSTANDARD_CALENDAR;
	int m_iBARBARIAN_HANDICAP;
	int m_iBARBARIAN_CIVILIZATION;
	int m_iBARBARIAN_LEADER;
	int m_iMINOR_CIV_HANDICAP;
	int m_iMINOR_CIVILIZATION;
	int m_iPROMOTION_EMBARKATION;
	int m_iPROMOTION_DEFENSIVE_EMBARKATION;
	int m_iPROMOTION_ALLWATER_EMBARKATION;
	int m_iPROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY;
	int m_iPROMOTION_OCEAN_IMPASSABLE;
	int m_iAI_HANDICAP;
	int m_iBARBARIAN_CAMP_IMPROVEMENT;
	int m_iWALLS_BUILDINGCLASS;
	int m_iPROMOTION_ONLY_DEFENSIVE;
	int m_iPROMOTION_UNWELCOME_EVANGELIST;

#ifdef NQ_SPOILS_OF_WAR

	int	m_iSPOILS_OF_WAR_PERCENT_ANCIENT;
	int	m_iSPOILS_OF_WAR_PERCENT_CLASSICAL;
	int	m_iSPOILS_OF_WAR_PERCENT_MEDIEVAL;
	int	m_iSPOILS_OF_WAR_PERCENT_RENAISSANCE;
	int m_iSPOILS_OF_WAR_PERCENT_INDUSTRIAL;
	int m_iSPOILS_OF_WAR_PERCENT_MODERN;
	int m_iSPOILS_OF_WAR_PERCENT_ATOMIC;
	int m_iSPOILS_OF_WAR_PERCENT_FUTURE;
#endif

	// DLL interface
	ICvEngineUtility4* m_pDLL;

	// Engine UI interface
	ICvUserInterface2* m_pEngineUI;
};

//extern CvGlobals gGlobals;	// for debugging

//
// inlines
//
//
// helpers
//
extern CvGlobals gGlobals;
#define GC gGlobals
#define gDLL GC.getDLLIFace()
#define DB (*GC.GetGameDatabase())
#define DLLUI (GC.GetEngineUserInterface())
#define DLLUIClass CvDLLInterfaceIFaceBase

#ifdef AUI_MINIDUMPS
void CreateMiniDump(EXCEPTION_POINTERS *pep = NULL);
LONG WINAPI CustomFilter(EXCEPTION_POINTERS *ExceptionInfo = NULL);
#endif

inline Database::Connection* CvGlobals::GetGameDatabase()
{
	return m_pGameDatabase;
}

inline const Database::Connection* CvGlobals::GetGameDatabase() const
{
	return m_pGameDatabase;
}

#endif
