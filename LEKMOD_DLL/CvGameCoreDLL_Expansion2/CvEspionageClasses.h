






#pragma once

#ifndef CIV5_ESPIONAGE_CLASSES_H
#define CIV5_ESPIONAGE_CLASSES_H







enum CvSpyRank
{
    SPY_RANK_RECRUIT,
    SPY_RANK_AGENT,
    SPY_RANK_SPECIAL_AGENT,
    NUM_SPY_RANKS
};

enum CvSpyState
{
    SPY_STATE_UNASSIGNED,
    SPY_STATE_TRAVELLING,
    SPY_STATE_SURVEILLANCE,
    SPY_STATE_GATHERING_INTEL,
    SPY_STATE_RIG_ELECTION,
    SPY_STATE_COUNTER_INTEL,
	SPY_STATE_MAKING_INTRODUCTIONS,
	SPY_STATE_SCHMOOZE,
    SPY_STATE_DEAD,
    NUM_SPY_STATES
};

enum CvSpyResult
{
    SPY_RESULT_UNDETECTED,
    SPY_RESULT_DETECTED,
    SPY_RESULT_IDENTIFIED,
#ifdef ESPIONAGE_SYSTEM_REWORK
	SPY_RESULT_SPOTTED,
#endif
    SPY_RESULT_KILLED,
    NUM_SPY_RESULTS
};

enum CvIntrigueType
{
    INTRIGUE_TYPE_DECEPTION,
    INTRIGUE_TYPE_BUILDING_ARMY,
    INTRIGUE_TYPE_BUILDING_AMPHIBIOUS_ARMY,
    INTRIGUE_TYPE_ARMY_SNEAK_ATTACK,
    INTRIGUE_TYPE_AMPHIBIOUS_SNEAK_ATTACK,
	INTRIGUE_TYPE_CONSTRUCTING_WONDER,
    NUM_INTRIGUE_TYPES
};

class CvEspionageSpy
{
public:
	CvEspionageSpy();


	int m_iName;
	int m_iCityX;
	int m_iCityY;
	CvSpyRank m_eRank;
	CvSpyState m_eSpyState;
	int m_iReviveCounter;
	bool m_bIsDiplomat;
	bool m_bEvaluateReassignment;
#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	int m_iMyPoliciesEspionageModifierCached;
	int m_iInfluenceMajorCivSpyRankBonusCached;
	int m_iInfluenceCityStateSpyRankBonusCached;
	int m_iInfluenceSurveillanceTimeCached;
	CvSpyRank m_eCachedRank;
#endif
};

FDataStream& operator>>(FDataStream&, CvEspionageSpy&);
FDataStream& operator<<(FDataStream&, const CvEspionageSpy&);









struct HeistLocation
{
	int m_iX;
	int m_iY;
};
struct SpyNotificationMessage
{
	int m_iCityX;
	int m_iCityY;
	PlayerTypes m_eAttackingPlayer;
	int m_iSpyResult;
	TechTypes m_eStolenTech;
};
struct IntrigueNotificationMessage
{
	PlayerTypes m_eDiscoveringPlayer;
	PlayerTypes m_eSourcePlayer;
	PlayerTypes m_eTargetPlayer;
	BuildingTypes m_eBuilding;
	ProjectTypes m_eProject;
	int m_iIntrigueType;
	int m_iTurnNum;
	int m_iCityX;
	int m_iCityY;
	CvString m_strSpyName;
	bool m_bShared;
};
typedef FStaticVector<CvEspionageSpy, 8, false, c_eCiv5GameplayDLL > SpyList;
typedef FStaticVector<TechTypes, 1, false, c_eCiv5GameplayDLL> TechList;
typedef FStaticVector<TechList, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> PlayerTechList;
#ifdef ESPIONAGE_SYSTEM_REWORK
typedef FStaticVector<int, 1, false, c_eCiv5GameplayDLL> ScienceToStealList;
typedef FStaticVector<ScienceToStealList, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> PlayerScienceToStealList;
#endif
typedef FStaticVector<int, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> NumTechsToStealList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> MaxTechCost;
typedef Firaxis::Array<std::vector<HeistLocation>, MAX_MAJOR_CIVS> HeistLocationList;

class CvPlayerEspionage
{
public:
	CvPlayerEspionage(void);
	~CvPlayerEspionage(void);
	void Init(CvPlayer* pPlayer);
	void Uninit(void);
	void Reset(void);


	void DoTurn(void);

	void CreateSpy(void);
	void ProcessSpy(uint uiSpyIndex);
	void UncoverIntrigue(uint uiSpyIndex);
	int  GetNextSpyName(void);
	bool IsSpyInCity(uint uiSpyIndex);
	CvCity* GetCityWithSpy(uint uiSpyIndex);
	int  GetSpyIndexInCity(CvCity* pCity);
	bool CanEverMoveSpyTo(CvCity* pCity);
	bool CanMoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat);
	bool MoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat);
	bool ExtractSpyFromCity(uint uiSpyIndex);
	void LevelUpSpy(uint uiSpyIndex);

	void UpdateSpies();
	void UpdateCity(CvCity* pCity);

#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	void CacheSpyStats();
#endif
	int CalcPerTurn(int iSpyState, CvCity* pCity, int iSpyIndex);
	int CalcRequired(int iSpyState, CvCity* pCity, int iSpyIndex);

	const char* GetSpyRankName(int iRank) const;

	bool IsDisableTechSteal() const;
	bool m_bDisableTechSteal;

	bool HasEstablishedSurveillance(uint uiSpyIndex);
	bool HasEstablishedSurveillanceInCity(CvCity* pCity);
	bool IsAnySurveillanceEstablished(PlayerTypes eTargetPlayer);

	bool IsDiplomat (uint uiSpyIndex);
	bool IsSchmoozing (uint uiSpyIndex);
	bool IsAnySchmoozing (CvCity* pCity);

	bool CanStageCoup(uint uiSpyIndex);
	int GetCoupChanceOfSuccess(uint uiSpyIndex);
	bool AttemptCoup(uint uiSpyIndex);

	int GetTurnsUntilStateComplete(uint uiSpyIndex);
	int GetPercentOfStateComplete(uint uiSpyIndex);

	int GetNumSpies(void);
	int GetNumAliveSpies(void);
	int GetNumAssignedSpies(void);
	int GetNumUnassignedSpies(void);

	void BuildStealableTechList(PlayerTypes ePlayer);
	bool IsTechStealable(PlayerTypes ePlayer, TechTypes eTech);

	int GetNumTechsToSteal(PlayerTypes ePlayer);

	bool IsMyDiplomatVisitingThem(PlayerTypes ePlayer, bool bIncludeTravelling = false);
	bool IsOtherDiplomatVisitingMe(PlayerTypes ePlayer);

	void AddSpyMessage(int iCityX, int iCityY, PlayerTypes ePlayer, int iSpyResult, TechTypes eStolenTech);
	void ProcessSpyMessages(void);

	void AddIntrigueMessage(PlayerTypes eDiscoveringPlayer, PlayerTypes eSourcePlayer, PlayerTypes eTargetPlayer, BuildingTypes eBuilding, ProjectTypes eProject, CvIntrigueType eIntrigueType, uint uiSpyIndex, CvCity* pCity, bool bShowNotification);
	Localization::String GetIntrigueMessage(uint uiIndex);
	bool HasRecentIntrigueAbout(PlayerTypes eTargetPlayer);
	IntrigueNotificationMessage* GetRecentIntrigueInfo(PlayerTypes eTargetPlayer);
	bool HasSharedIntrigue(PlayerTypes eTargetPlayer, PlayerTypes eSourcePlayer, CvIntrigueType eIntrigueType);
	bool HasSharedIntrigue(PlayerTypes eTargetPlayer, PlayerTypes eSourcePlayer);
	int MarkRecentIntrigueAsShared(PlayerTypes eTargetPlayer, PlayerTypes eSourcePlayer, CvIntrigueType eIntrigueType);
	bool HasSharedIntrigueAboutMe(PlayerTypes eFromPlayer);

	CvString GetLogFileName(void) const;
	void LogEspionageMsg(CvString& strMsg);

	SpyList m_aSpyList;
	std::vector<int> m_aiSpyListNameOrder;
	int m_iSpyListNameOrderIndex;
	PlayerTechList m_aaPlayerStealableTechList;
#ifdef ESPIONAGE_SYSTEM_REWORK
	PlayerScienceToStealList m_aaPlayerScienceToStealList;
#endif
	NumTechsToStealList m_aiNumTechsToStealList;
	MaxTechCost m_aiMaxTechCost;
	HeistLocationList m_aHeistLocations;
	std::vector<SpyNotificationMessage> m_aSpyNotificationMessages;
	std::vector<IntrigueNotificationMessage> m_aIntrigueNotificationMessages;

private:
	CvPlayer* m_pPlayer;
};

FDataStream& operator>>(FDataStream&, CvPlayerEspionage&);
FDataStream& operator<<(FDataStream&, const CvPlayerEspionage&);












typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyAssignmentList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyAmountProgressList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyRateProgressList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyGoalProgressList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> LastProgressList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyResultList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> LastPotentialList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> NumTimesCityRobbedList;

class CvCityEspionage
{
public:
#ifdef AUI_CITY_FIX_COMPONENT_CONSTRUCTORS_CONTAIN_POINTERS
	CvCityEspionage(CvCity* pCity);
#else
	CvCityEspionage(void);
#endif
	~CvCityEspionage(void);
	void Init(CvCity* pCity);
	void Uninit(void);
	void Reset(void);

	void SetActivity(PlayerTypes ePlayer, int iAmount, int iRate, int iGoal);
	void Process(PlayerTypes ePlayer);
	bool HasReachedGoal(PlayerTypes ePlayer);
	void ResetProgress(PlayerTypes ePlayer);
	void SetLastProgress(PlayerTypes ePlayer, int iProgress);
	void SetLastPotential(PlayerTypes ePlayer, int iPotential);
	void SetLastBasePotential(PlayerTypes ePlayer, int iPotential);
	void SetSpyResult(PlayerTypes ePlayer, int iResult);

	bool HasCounterSpy();

	CvCity* m_pCity;
	SpyAssignmentList m_aiSpyAssignment;
	SpyAmountProgressList m_aiAmount;
	SpyRateProgressList m_aiRate;
	SpyGoalProgressList m_aiGoal;
	LastProgressList m_aiLastProgress;
	LastPotentialList m_aiLastPotential;
	LastPotentialList m_aiLastBasePotential;
	SpyResultList m_aiResult;
	NumTimesCityRobbedList m_aiNumTimesCityRobbed;
};

FDataStream& operator>>(FDataStream&, CvCityEspionage&);
FDataStream& operator<<(FDataStream&, const CvCityEspionage&);








typedef FStaticVector<int, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> EspionageAIOutOfTechTurnList;
typedef FStaticVector<int, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> EspionageAILastTurns;
typedef FStaticVector<int, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> EspionageAICount;
typedef std::vector<CvCity*> EspionageCityList;

class CvEspionageAI
{
public:

	enum {
		PLAN_DEFEND_CS_FOR_WIN,
		PLAN_ATTACK_CS_TO_PREVENT_DEFEAT,
		PLAN_COLLECT_VOTES,
		PLAN_PLAY_NORMAL
	};

	CvEspionageAI(void);
	~CvEspionageAI(void);
	void Init(CvPlayer* pPlayer);
	void Uninit(void);
	void Reset(void);

	void DoTurn(void);
	void StealTechnology(void);
	void UpdateCivOutOfTechTurn(void);
	void AttemptCoups(void);
	void FindTargetSpyNumbers(int* piTargetOffensiveSpies, int* piTargetDefensiveSpies, int* piTargetCityStateSpies, int* piTargetDiplomatSpies);
	void BuildDiplomatCityList(EspionageCityList& aEspionageCityList);
	void BuildOffenseCityList(EspionageCityList& aOffenseCityList);
	void BuildDefenseCityList(EspionageCityList& aDefenseCityList);
	void BuildMinorCityList(EspionageCityList& aMinorCityList);

	int GetCityStatePlan(PlayerTypes* peThreatPlayer = NULL);

	void EvaluateSpiesAssignedToTargetPlayer(PlayerTypes ePlayer);
	void EvaluateUnassignedSpies(void);
	void EvaluateDefensiveSpies(void);
	void EvaluateDiplomatSpies(void);

	CvPlayer* m_pPlayer;
	EspionageAIOutOfTechTurnList m_aiCivOutOfTechTurn;

	EspionageAICount m_aiNumSpiesCaught;
	EspionageAICount m_aiNumSpiesKilled;
	EspionageAICount m_aiNumSpiesDied;

	EspionageAILastTurns m_aiTurnLastSpyCaught;
	EspionageAILastTurns m_aiTurnLastSpyKilled;
	EspionageAILastTurns m_aiTurnLastSpyDied;

	bool m_bUNCountdownStarted;
	int m_iTurnEspionageStarted;
};

FDataStream& operator>>(FDataStream&, CvEspionageAI&);
FDataStream& operator<<(FDataStream&, const CvEspionageAI&);

#endif
