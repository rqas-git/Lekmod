






#pragma once

#ifndef CIV5_GRAND_STRATEGY_AI_H
#define CIV5_GRAND_STRATEGY_AI_H

#include "CvDiplomacyAI.h"










class CvGrandStrategyAI
{
public:
	CvGrandStrategyAI(void);
	~CvGrandStrategyAI(void);
	void Init(CvAIGrandStrategyXMLEntries* pAIGrandStrategies, CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

#ifdef AUI_CONSTIFY
	CvPlayer* GetPlayer() const;
	CvAIGrandStrategyXMLEntries* GetAIGrandStrategies() const;
#else
	CvPlayer* GetPlayer();
	CvAIGrandStrategyXMLEntries* GetAIGrandStrategies();
#endif

	void DoTurn();

#ifdef AUI_CONSTIFY
	int GetConquestPriority() const;
	int GetCulturePriority() const;
	int GetUnitedNationsPriority() const;
	int GetSpaceshipPriority() const;

	int GetBaseGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy) const;
#else
	int GetConquestPriority();
	int GetCulturePriority();
	int GetUnitedNationsPriority();
	int GetSpaceshipPriority();

	int GetBaseGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy);
#endif

	AIGrandStrategyTypes GetActiveGrandStrategy() const;
	void SetActiveGrandStrategy(AIGrandStrategyTypes eGrandStrategy);
	int GetNumTurnsSinceActiveSet() const;
	void SetNumTurnsSinceActiveSet(int iValue);
	void ChangeNumTurnsSinceActiveSet(int iChange);

	int GetGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy) const;
	void SetGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy, int iValue);
	void ChangeGrandStrategyPriority(AIGrandStrategyTypes eGrandStrategy, int iChange);

#ifdef AUI_CONSTIFY
	int GetPersonalityAndGrandStrategy(FlavorTypes eFlavorType) const;
#else
	int GetPersonalityAndGrandStrategy(FlavorTypes eFlavorType);
#endif





	void DoGuessOtherPlayersActiveGrandStrategy();

	AIGrandStrategyTypes GetGuessOtherPlayerActiveGrandStrategy(PlayerTypes ePlayer) const;
	GuessConfidenceTypes GetGuessOtherPlayerActiveGrandStrategyConfidence(PlayerTypes ePlayer) const;
	void SetGuessOtherPlayerActiveGrandStrategy(PlayerTypes ePlayer, AIGrandStrategyTypes eGrandStrategy, GuessConfidenceTypes eGuessConfidence);

	int GetGuessOtherPlayerConquestPriority(PlayerTypes ePlayer, int iWorldMilitaryAverage);
	int GetGuessOtherPlayerCulturePriority(PlayerTypes ePlayer, int iWorldCultureAverage, int iWorldTourismAverage);
	int GetGuessOtherPlayerUnitedNationsPriority(PlayerTypes ePlayer);
	int GetGuessOtherPlayerSpaceshipPriority(PlayerTypes ePlayer, int iWorldNumTechsAverage);

private:

	void LogGrandStrategies(const FStaticVector< int, 5, true, c_eCiv5GameplayDLL >& vGrandStrategyPriorities);
	void LogGuessOtherPlayerGrandStrategy(const FStaticVector< int, 5, true, c_eCiv5GameplayDLL >& vGrandStrategyPriorities, PlayerTypes ePlayer);

	CvPlayer* m_pPlayer;
	CvAIGrandStrategyXMLEntries* m_pAIGrandStrategies;

	int m_iNumTurnsSinceActiveSet;

	AIGrandStrategyTypes m_eActiveGrandStrategy;

	int* m_paiGrandStrategyPriority;





	int* m_eGuessOtherPlayerActiveGrandStrategy;
	int* m_eGuessOtherPlayerActiveGrandStrategyConfidence;
};









class CvAIGrandStrategyXMLEntry: public CvBaseInfo
{
public:
	CvAIGrandStrategyXMLEntry();
	virtual ~CvAIGrandStrategyXMLEntry();

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);


#ifdef AUI_WARNING_FIXES
	int GetFlavorValue(uint i) const;
	int GetFlavorModValue(uint i) const;
#else
	int GetFlavorValue(int i) const;
	int GetFlavorModValue(int i) const;
#endif
	int GetSpecializationBoost(YieldTypes eYield) const;

private:
	int* m_piFlavorValue;
	int* m_piSpecializationBoost;
	int* m_piFlavorModValue;
};












class CvAIGrandStrategyXMLEntries
{
public:
	CvAIGrandStrategyXMLEntries(void);
	~CvAIGrandStrategyXMLEntries(void);


	std::vector<CvAIGrandStrategyXMLEntry*>& GetAIGrandStrategyEntries();
#ifdef AUI_WARNING_FIXES
	uint GetNumAIGrandStrategies() const;
	CvAIGrandStrategyXMLEntry* GetEntry(uint index) const;
#else
	int GetNumAIGrandStrategies();
	CvAIGrandStrategyXMLEntry* GetEntry(int index);
#endif

	void DeleteArray();

private:
	std::vector<CvAIGrandStrategyXMLEntry*> m_paAIGrandStrategyEntries;
};

#endif
