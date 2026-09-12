






#pragma once

#ifndef CIV5_TECH_CLASSES_H
#define CIV5_TECH_CLASSES_H


class CvTeam;
class CvTechAI;

#include "CvInfos.h"










class CvTechEntry: public CvBaseInfo
{
public:
	CvTechEntry(void);
	~CvTechEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);


	int GetAIWeight() const;
	int GetAITradeModifier() const;
	int GetResearchCost() const;
	int GetAdvancedStartCost() const;
	int GetEra() const;
	int GetFeatureProductionModifier() const;
	int GetUnitFortificationModifier() const;
	int GetUnitBaseHealModifier() const;
	int GetWorkerSpeedModifier() const;
	int GetFirstFreeUnitClass() const;
	int GetFirstFreeTechs() const;
	int GetEmbarkedMoveChange() const;


	int GetEmbarkedSightChange() const;

#if defined(MISC_CHANGES)
	int GetExtraLeagueVotes() const;
#endif

	int GetNumInternationalTradeRoutesChange() const;;
	int GetInfluenceSpreadModifier() const;;
	int GetExtraVotesPerDiplomat() const;

	int GetGridX() const;
	int GetGridY() const;

	bool IsEndsGame() const;
	bool IsAllowsEmbarking() const;

	bool IsAllowsEmbarkingCivilian() const;

	bool IsAllowsDefensiveEmbarking() const;
	bool IsEmbarkedAllWaterPassage() const;
	bool IsAllowsBarbarianBoats() const;
	bool IsRepeat() const;
	bool IsTrade() const;
	bool IsDisable() const;
	bool IsDisableTechSteal() const;
	bool IsGoodyTech() const;
	bool IsExtraWaterSeeFrom() const;
	bool IsMapCentering() const;
	bool IsMapVisible() const;
	bool IsMapTrading() const;
	bool IsTechTrading() const;
	bool IsGoldTrading() const;
	bool IsAllowEmbassyTradingAllowed() const;
	bool IsOpenBordersTradingAllowed() const;
	bool IsDefensivePactTradingAllowed() const;
	bool IsResearchAgreementTradingAllowed() const;
	bool IsTradeAgreementTradingAllowed() const;
	bool IsPermanentAllianceTrading() const;
	bool IsBridgeBuilding() const;
	bool IsWaterWork() const;
	int IsFreePromotion(int i) const;
	bool IsTriggersArchaeologicalSites() const;
	bool IsAllowsWorldCongress() const;

	std::string pyGetQuote()
	{
		return GetQuote();
	}
	const char* GetQuote();
	void SetQuoteKey(const char* szVal);
	const char* GetSound() const;
	void SetSound(const char* szVal);
	const char* GetSoundMP() const;
	void SetSoundMP(const char* szVal);


	int GetDomainExtraMoves(int i) const;
	int GetTradeRouteDomainExtraRange(int i) const;
	int GetFlavorValue(int i) const;
	int GetPrereqOrTechs(int i) const;
	int GetPrereqAndTechs(int i) const;

private:
	int m_iAIWeight;
	int m_iAITradeModifier;
	int m_iResearchCost;
	int m_iAdvancedStartCost;
	int m_iEra;
	int m_iFeatureProductionModifier;
	int m_iUnitFortificationModifier;
	int m_iUnitBaseHealModifier;
	int m_iWorkerSpeedModifier;
	int m_iFirstFreeUnitClass;
	int m_iFirstFreeTechs;
	int m_iEmbarkedMoveChange;

	int m_iEmbarkedSightChange;
	int m_iInternationalTradeRoutesChange;
	int m_iInfluenceSpreadModifier;
	int m_iExtraVotesPerDiplomat;

	int m_iGridX;
	int m_iGridY;

	bool m_bEndsGame;
	bool m_bAllowsEmbarking;

	bool m_bAllowsEmbarkingCivilian;

#if defined(MISC_CHANGES)
	int m_iExtraLeagueVotes;
#endif

	bool m_bAllowsDefensiveEmbarking;
	bool m_bEmbarkedAllWaterPassage;
	bool m_bAllowsBarbarianBoats;
	bool m_bRepeat;
	bool m_bTrade;
	bool m_bDisable;
	bool m_bDisableTechSteal;
	bool m_bGoodyTech;
	bool m_bExtraWaterSeeFrom;
	bool m_bMapCentering;
	bool m_bMapVisible;
	bool m_bMapTrading;
	bool m_bTechTrading;
	bool m_bGoldTrading;
	bool m_bAllowEmbassyTradingAllowed;
	bool m_bOpenBordersTradingAllowed;
	bool m_bDefensivePactTradingAllowed;
	bool m_bResearchAgreementTradingAllowed;
	bool m_bTradeAgreementTradingAllowed;
	bool m_bPermanentAllianceTrading;
	bool m_bBridgeBuilding;
	bool m_bWaterWork;
	bool m_bTriggersArchaeologicalSites;
	bool m_bAllowsWorldCongress;

	CvString m_strQuoteKey;
	CvString m_wstrQuote;
	CvString m_strSound;
	CvString m_strSoundMP;


	int* m_piDomainExtraMoves;
	int* m_piTradeRouteDomainExtraRange;
	int* m_piFlavorValue;
	int* m_piPrereqOrTechs;
	int* m_piPrereqAndTechs;
	bool* m_pabFreePromotion;
};












class CvTechXMLEntries
{
public:
	CvTechXMLEntries(void);
	~CvTechXMLEntries(void);


	std::vector<CvTechEntry*>& GetTechEntries();
#ifdef AUI_WARNING_FIXES
	uint GetNumTechs() const;
	_Ret_maybenull_ CvTechEntry* GetEntry(uint index);
#else
	int GetNumTechs();
	_Ret_maybenull_ CvTechEntry* GetEntry(int index);
#endif

	void DeleteArray();

private:
	std::vector<CvTechEntry*> m_paTechEntries;
};










class CvPlayerTechs: public CvFlavorRecipient
{
public:
	CvPlayerTechs(void);
	~CvPlayerTechs(void);
	void Init(CvTechXMLEntries* pTechs, CvPlayer* pPlayer, bool bIsCity);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);


	void FlavorUpdate();

	CvPlayer* GetPlayer();
	CvTechAI* GetTechAI();


	void SetResearchingTech(TechTypes eIndex, bool bNewValue);
	bool IsResearchingTech(TechTypes eIndex) const;
	void SetCivTechPriority(TechTypes eIndex, int iNewValue);
	int GetCivTechPriority(TechTypes eIndex) const;
	void SetLocaleTechPriority(TechTypes eIndex, int iNewValue);
	int GetLocaleTechPriority(TechTypes eIndex) const;
	ResourceTypes GetLocaleTechResource(TechTypes eIndex) const;
	UnitTypes GetCivTechUniqueUnit(TechTypes eIndex) const;
	BuildingTypes GetCivTechUniqueBuilding(TechTypes eIndex) const;
	ImprovementTypes GetCivTechUniqueImprovement(TechTypes eIndex) const;
	void SetLocalePriorities();
	bool IsResearch() const;
	bool CanEverResearch(TechTypes eTech) const;
	bool CanResearch(TechTypes eTech, bool bTrade = false) const;
	bool CanResearchForFree(TechTypes eTech) const;
	TechTypes GetCurrentResearch() const;
	bool IsCurrentResearchRepeat() const;
	bool IsNoResearchAvailable() const;
	void CheckForTechAchievement() const;
	int GetResearchTurnsLeft(TechTypes eTech, bool bOverflow) const;
	int GetResearchTurnsLeftTimes100(TechTypes eTech, bool bOverflow) const;
	int GetNumTechsCanBeResearched() const;
	CvTechXMLEntries* GetTechs() const;
	int GetResearchCost(TechTypes eTech) const;
	int GetResearchProgress(TechTypes eTech) const;
	int GetMedianTechResearch() const;
#ifdef ESPIONAGE_SYSTEM_REWORK
	int GetMedianTechToStealResearch(PlayerTypes eTarget) const;
#endif

private:
	void AddFlavorAsStrategies(int iPropagatePercent);


	void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR);

	bool* m_pabResearchingTech;
	int* m_piCivTechPriority;
	int* m_piLocaleTechPriority;
	ResourceTypes* m_peLocaleTechResources;
	UnitTypes* m_peCivTechUniqueUnits;
	BuildingTypes* m_peCivTechUniqueBuildings;
	ImprovementTypes* m_peCivTechUniqueImprovements;
	CvTechXMLEntries* m_pTechs;
	CvPlayer* m_pPlayer;
	CvTechAI* m_pTechAI;
};










class CvTeamTechs
{
public:
	CvTeamTechs(void);
	~CvTeamTechs(void);
	void Init(CvTechXMLEntries* pTechs, CvTeam* pTeam);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

#ifdef HAS_TECH_BY_HUMAN
	void SetHasTechByHuman(TechTypes eIndex, bool bNewValue);
	bool HasTechByHuman(TechTypes eIndex) const;
#endif


	void SetHasTech(TechTypes eIndex, bool bNewValue);
	bool HasTech(TechTypes eIndex) const;
#ifdef CAN_PROPOSE_ENACT_UPDATES_ONCE_PER_SESSION
	void SetHasTechForLeague(TechTypes eIndex, bool bNewValue);
	bool HasTechForLeague(TechTypes eIndex) const;
#endif

	TechTypes GetLastTechAcquired() const;
	void SetLastTechAcquired(TechTypes eTech);

	int GetNumTechsKnown() const;
	bool HasResearchedAllTechs() const;

	void SetNoTradeTech(TechTypes eIndex, bool bNewValue);
	bool IsNoTradeTech(TechTypes eIndex) const;
	void IncrementTechCount(TechTypes eIndex);
	int GetTechCount(TechTypes eIndex) const;
	void SetResearchProgress(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer);
	void SetResearchProgressTimes100(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer);
	int GetResearchProgress(TechTypes eIndex) const;
	int GetResearchProgressTimes100(TechTypes eIndex) const;
	void ChangeResearchProgress(TechTypes eIndex, int iChange, PlayerTypes ePlayer);
	void ChangeResearchProgressTimes100(TechTypes eIndex, int iChange, PlayerTypes ePlayer);
	int ChangeResearchProgressPercent(TechTypes eIndex, int iPercent, PlayerTypes ePlayer);
	int GetResearchCost(TechTypes eTech) const;
	int GetResearchLeft(TechTypes eTech) const;
	CvTechXMLEntries* GetTechs() const;

private:
	int GetMaxResearchOverflow(TechTypes eTech, PlayerTypes ePlayer) const;

	TechTypes m_eLastTechAcquired;

	bool* m_pabHasTech;
#ifdef HAS_TECH_BY_HUMAN
	bool* m_pabHasTechByHuman;
#endif
#ifdef CAN_PROPOSE_ENACT_UPDATES_ONCE_PER_SESSION
	bool* m_pabHasTechForLeague;
#endif
	bool* m_pabNoTradeTech;
	int* m_paiResearchProgress;
	int* m_paiTechCount;
	CvTechXMLEntries* m_pTechs;
	CvTeam* m_pTeam;
};

#endif
