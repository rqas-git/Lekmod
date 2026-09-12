






#pragma once

#ifndef CIV5_CULTURE_CLASSES_H
#define CIV5_CULTURE_CLASSES_H










class CvGreatWork
{
public:
	CvGreatWork();
	CvGreatWork(CvString szGreatPersonName, GreatWorkType eType, GreatWorkClass eClassType, int iTurn, EraTypes eEra, PlayerTypes ePlayer);


	CvString m_szGreatPersonName;
	GreatWorkType m_eType;
	GreatWorkClass m_eClassType;
	int m_iTurnFounded;
	EraTypes m_eEra;
	PlayerTypes m_ePlayer;
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	std::vector<int> m_viYield;
#endif
};

FDataStream& operator>>(FDataStream&, CvGreatWork&);
FDataStream& operator<<(FDataStream&, const CvGreatWork&);

typedef FStaticVector<CvGreatWork, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL > GreatWorkList;











class CvGameCulture
{
public:
	CvGameCulture(void);
	~CvGameCulture(void);

	void DoTurn();


	int CreateGreatWork(GreatWorkType eType, GreatWorkClass eClassType, PlayerTypes ePlayer, EraTypes eEra, CvString szCreator);
	int GetNumGreatWorks() const
	{
		return m_CurrentGreatWorks.size();
	}

	GreatWorkType GetGreatWorkType(int iIndex) const;
	GreatWorkClass GetGreatWorkClass(int iIndex) const;
	CvString GetGreatWorkTooltip(int iIndex, PlayerTypes eOwner) const;
	CvString GetGreatWorkName(int iIndex) const;
	CvString GetGreatWorkArtist(int iIndex) const;
	CvString GetGreatWorkEra(int iIndex) const;
	CvString GetGreatWorkEraAbbreviation(int iIndex) const;
	CvString GetGreatWorkEraShort(int iIndex) const;
	PlayerTypes GetGreatWorkCreator (int iIndex) const;
	PlayerTypes GetGreatWorkController(int iIndex) const;
	int GetGreatWorkCurrentThemingBonus (int iIndex) const;

	bool SwapGreatWorks (PlayerTypes ePlayer1, int iWork1, PlayerTypes ePlayer2, int iWork2);
	void MoveGreatWorks(PlayerTypes ePlayer, int iCity1, int iBuildingClass1, int iWorkIndex1, int iCity2, int iBuildingClass2, int iWorkIndex2);

	GreatWorkList m_CurrentGreatWorks;


	int GetNumCivsInfluentialForWin() const;
	bool GetReportedSomeoneInfluential() const
	{
		return m_bReportedSomeoneInfluential;
	};
	void SetReportedSomeoneInfluential(bool bValue)
	{
		m_bReportedSomeoneInfluential = bValue;
	};

private:
	bool m_bReportedSomeoneInfluential;
};

FDataStream& operator>>(FDataStream&, CvGameCulture&);
FDataStream& operator<<(FDataStream&, const CvGameCulture&);

enum InfluenceLevelTypes		
{
	NO_INFLUENCE_LEVEL = -1,
	INFLUENCE_LEVEL_UNKNOWN,
	INFLUENCE_LEVEL_EXOTIC,
	INFLUENCE_LEVEL_FAMILIAR,
	INFLUENCE_LEVEL_POPULAR,
	INFLUENCE_LEVEL_INFLUENTIAL,
	INFLUENCE_LEVEL_DOMINANT,
};

enum InfluenceLevelTrend		
{
	INFLUENCE_TREND_FALLING = -1,
	INFLUENCE_TREND_STATIC,
	INFLUENCE_TREND_RISING,
};

enum PublicOpinionTypes		
{
	NO_PUBLIC_OPINION = -1,
	PUBLIC_OPINION_CONTENT,
	PUBLIC_OPINION_DISSIDENTS,
	PUBLIC_OPINION_CIVIL_RESISTANCE,
	PUBLIC_OPINION_REVOLUTIONARY_WAVE,
};

class CvGreatWorkInMyEmpire
{
public:
	CvGreatWorkInMyEmpire();
	CvGreatWorkInMyEmpire(int iIndex, int iCityID, BuildingTypes eBuilding, int iSlot, PlayerTypes ePlayer, EraTypes eEra);


	int m_iGreatWorkIndex;
	int m_iCityID;
	BuildingTypes m_eBuilding;
	int m_iSlot;
	PlayerTypes m_ePlayer;
	EraTypes m_eEra;
};

class CvGreatWorkBuildingInMyEmpire
{
public:
	CvGreatWorkBuildingInMyEmpire();
	CvGreatWorkBuildingInMyEmpire(int iCityID, BuildingTypes eBuilding);


	int m_iCityID;
	BuildingTypes m_eBuilding;
	bool m_bThemed;
	bool m_bEndangered;
};










class CvPlayerCulture
{
public:
	CvPlayerCulture(void);
	~CvPlayerCulture(void);

	void Init(CvPlayer* pPlayer);


	bool HasAvailableGreatWorkSlot(GreatWorkSlotType eGreatWorkSlot);
	int GetNumAvailableGreatWorkSlots(GreatWorkSlotType eGreatWorkSlot) const;
	CvCity *GetClosestAvailableGreatWorkSlot(int iX, int iY, GreatWorkSlotType eGreatWorkSlot, BuildingClassTypes *eBuildingClass, int *iSlot) const;
	int GetNumGreatWorks() const;
	int GetNumGreatWorkSlots() const;
	int GetNumGreatWorkSlots(GreatWorkSlotType eGreatWorkSlot) const;
	bool ControlsGreatWork (int iIndex);
	bool GetGreatWorkLocation(int iGreatWorkIndex, int &iCityID, BuildingTypes &eBuilding, int &iSlot);

	void DoSwapGreatWorks();
	void MoveWorks (GreatWorkSlotType eType, vector<CvGreatWorkBuildingInMyEmpire> &buildings, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2);
	bool ThemeBuilding(vector<CvGreatWorkBuildingInMyEmpire>::const_iterator it, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2, bool bConsiderOtherPlayers);
	bool ThemeEqualArtArtifact(CvGreatWorkBuildingInMyEmpire kBldg, int iThemingBonusIndex, int iNumSlots, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2, bool bConsiderOtherPlayers);
	bool FillBuilding(vector<CvGreatWorkBuildingInMyEmpire>::const_iterator it, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2);
	void MoveWorkIntoSlot (CvGreatWorkInMyEmpire kWork, int iCityID, BuildingTypes eBuilding, int iSlot);
	int GetSwappableWritingIndex() const;
	int GetSwappableArtIndex() const;
	int GetSwappableArtifactIndex() const;
	int GetSwappableMusicIndex() const;
	void SetSwappableWritingIndex (int iIndex);
	void SetSwappableArtIndex (int iIndex);
	void SetSwappableArtifactIndex (int iIndex);
	void SetSwappableMusicIndex (int iIndex);


	void AddDigCompletePlot(CvPlot *pPlot);
	void RemoveDigCompletePlot(CvPlot *pPlot);
	void ResetDigCompletePlots();
	CvPlot *GetNextDigCompletePlot() const;
	CvUnit *GetNextDigCompleteArchaeologist(CvPlot **ppPlot) const;
	bool HasDigCompleteHere(CvPlot *pPlot) const;
	int GetWrittenArtifactCulture() const;

	void DoArchaeologyChoice (ArchaeologyChoiceType eChoice);

	ArchaeologyChoiceType GetArchaeologyChoice(CvPlot *pPlot);
 

	void DoTurn();
	int GetLastTurnLifetimeCulture() const;
	void SetLastTurnLifetimeCulture(int iValue);
	int GetInfluenceOn(PlayerTypes ePlayer) const;
	void ChangeInfluenceOn(PlayerTypes ePlayer, int iValue);
	int GetLastTurnInfluenceOn(PlayerTypes ePlayer) const;
	int GetInfluencePerTurn(PlayerTypes ePlayer) const;
	InfluenceLevelTypes GetInfluenceLevel(PlayerTypes ePlayer) const;
	InfluenceLevelTrend GetInfluenceTrend(PlayerTypes ePlayer) const;
	int GetTurnsToInfluential(PlayerTypes ePlayer) const;
	int GetNumCivsInfluentialOn() const;
	int GetNumCivsToBeInfluentialOn() const;
	PlayerTypes GetCivLowestInfluence(bool bCheckOpenBorders) const;
	int GetInfluenceTradeRouteScienceBonus(PlayerTypes ePlayer) const;
	int GetInfluenceCityConquestReduction(PlayerTypes ePlayer) const;
	int GetInfluenceSurveillanceTime(PlayerTypes ePlayer) const;
	int GetInfluenceCityStateSpyRankBonus(PlayerTypes eCityStatePlayer) const;
	int GetInfluenceMajorCivSpyRankBonus(PlayerTypes ePlayer) const;
	CvString GetInfluenceSpyRankTooltip (CvString szName, CvString iRank, PlayerTypes ePlayer);

	int GetTourism();
#if defined(LEK_YIELD_TOURISM) && defined(STANDARDIZE_YIELDS)
	int GetTourismTimes100() const;
#endif
	int GetTourismModifierWith(PlayerTypes ePlayer) const;
	CvString GetTourismModifierWithTooltip(PlayerTypes ePlayer) const;
	int GetTourismModifierSharedReligion() const;
	int GetTourismModifierTradeRoute() const;
	int GetTourismModifierOpenBorders() const;
	PublicOpinionTypes GetPublicOpinionType() const;
	PolicyBranchTypes GetPublicOpinionPreferredIdeology() const;
	CvString GetPublicOpinionTooltip() const;
	int GetPublicOpinionUnhappiness() const;
	CvString GetPublicOpinionUnhappinessTooltip() const;
	PlayerTypes GetPublicOpinionBiggestInfluence() const;
	int GetTurnIdeologySwitch() const;
	void SetTurnIdeologySwitch(int iTurn);
	int GetTourismBlastStrength(int iMultiplier);
	void AddTourismAllKnownCivs(int iTourism);
	void DoPublicOpinion();
	int ComputeHypotheticalPublicOpinionUnhappiness(PolicyBranchTypes eBranch);
	bool WantsDiplomatDoingPropaganda(PlayerTypes ePlayer) const;
	int GetMaxPropagandaDiplomatsWanted() const;


	int GetTotalThemingBonuses() const;


	vector<CvPlot *> m_aDigCompletePlots;
	int m_iLastTurnLifetimeCulture;
	int m_aiCulturalInfluence[MAX_MAJOR_CIVS];
	int m_aiLastTurnCulturalInfluence[MAX_MAJOR_CIVS];
	bool m_bReportedTwoCivsAway;
	bool m_bReportedOneCivAway;
	PublicOpinionTypes m_eOpinion;
	PlayerTypes m_eOpinionBiggestInfluence;
	int m_iTurnIdeologySwitch;
	PolicyBranchTypes m_ePreferredIdeology;
	int m_iOpinionUnhappiness;
	CvString m_strOpinionTooltip;
	CvString m_strOpinionUnhappinessTooltip;

	int m_iSwappableWritingIndex;
	int m_iSwappableArtIndex;
	int m_iSwappableArtifactIndex;
	int m_iSwappableMusicIndex;

private:
#ifdef NQ_IDEOLOGY_PRESSURE_UNHAPPINESS_MODIFIER_FROM_POLICIES
	int ComputePublicOpinionUnhappiness(int iDissatisfaction, int &iPerCityUnhappy, int &iUnhappyPerXPop, int iUnhappinessModifier);
#else
	int ComputePublicOpinionUnhappiness(int iDissatisfaction, int &iPerCityUnhappy, int &iUnhappyPerXPop);
#endif


	void LogCultureData();
	void LogThemedBuilding(int iCityID, BuildingTypes eBuilding, int iBonus);
	void LogSwapWorks(PlayerTypes eOtherPlayer, int iWorkDiscarded, int iWorkAcquired);
	void AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, CvString strValue);
	void AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, int iValue);
	void AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, float fValue);
	CvString GetLogFileName(CvString& playerName) const;

	CvPlayer *m_pPlayer;
};

FDataStream& operator>>(FDataStream&, CvPlayerCulture&);
FDataStream& operator<<(FDataStream&, const CvPlayerCulture&);










class CvCityCulture
{
public:
#ifdef AUI_CITY_FIX_COMPONENT_CONSTRUCTORS_CONTAIN_POINTERS
	CvCityCulture(CvCity* pCity);
#else
	CvCityCulture(void);
#endif
	~CvCityCulture(void);

	void Init(CvCity* m_pCity);

	int GetNumGreatWorks() const;
	int GetNumGreatWorkSlots() const;
	int GetNumAvailableGreatWorkSlots(GreatWorkSlotType eSlotType) const;
	void ClearGreatWorks();
	GreatWorkSlotType GetSlotTypeFirstAvailableCultureBuilding() const;

	int GetBaseTourismBeforeModifiers();
	int GetBaseTourism();
	int GetTourismMultiplier(PlayerTypes ePlayer, bool bIgnoreReligion, bool bIgnoreOpenBorders, bool bIgnoreTrade, bool bIgnorePolicies, bool bIgnoreIdeologies) const;

	CvString GetTourismTooltip();
	CvString GetFilledSlotsTooltip();
	CvString GetTotalSlotsTooltip();

	bool IsThemingBonusPossible(BuildingClassTypes eBuildingClass) const;
	int GetThemingBonus(BuildingClassTypes eBuildingClass) const;
	CvString GetThemingTooltip(BuildingClassTypes eBuildingClass) const;

	int GetCultureFromWonders() const;
	int GetCultureFromNaturalWonders() const;
	int GetCultureFromImprovements() const;
#if defined(LEKMOD_GREAT_WORK_YIELD_EFFECTS)
	int GetPublicThemingBonusIndex(BuildingClassTypes eBuildingClass) const { return GetThemingBonusIndex(eBuildingClass); }
#endif

	void LogGreatWorks (FILogFile* pLog);

private:
	int GetThemingBonusIndex(BuildingClassTypes eBuildingClass) const;

	CvCity *m_pCity;
};

namespace CultureHelpers
{
	GreatWorkType GetArtifact(CvPlot *pPlot);
	GreatWorkClass GetGreatWorkClass(GreatWorkType eGreatWork);
	CvString GetGreatWorkName(GreatWorkType eGreatWorkType);
	GreatWorkSlotType GetGreatWorkSlot(GreatWorkType eType);
	CvString GetGreatWorkAudio(GreatWorkType eGreatWorkType);
	int GetThemingBonusIndex(PlayerTypes eOwner, CvBuildingEntry *pkEntry, vector<int> &aGreatWorkIndices);
	bool IsValidForThemingBonus(CvThemingBonusInfo *pBonusInfo, EraTypes eEra, vector<EraTypes> &aErasSeen, PlayerTypes ePlayer, vector<PlayerTypes> &aPlayersSeen, PlayerTypes eOwner);
	int FindWorkNotChosen(vector<CvGreatWorkInMyEmpire> &aWorks, vector<int> &aWorksChosen);
	void SendArtSwapNotification(GreatWorkSlotType eType, bool bArt, PlayerTypes eOriginator, PlayerTypes eReceipient, int iWorkFromOriginator, int iWorkFromRecipient);
}

#endif
