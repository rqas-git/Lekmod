






#pragma once

#ifndef CV_DEAL_CLASSES_H
#define CV_DEAL_CLASSES_H

#include "CvDiplomacyAIEnums.h"

enum TradeableItems
{
    TRADE_ITEM_NONE = -1,
    TRADE_ITEM_GOLD,
    TRADE_ITEM_GOLD_PER_TURN,
    TRADE_ITEM_MAPS,
    TRADE_ITEM_RESOURCES,
    TRADE_ITEM_CITIES,
    TRADE_ITEM_UNITS,
    TRADE_ITEM_OPEN_BORDERS,
    TRADE_ITEM_DEFENSIVE_PACT,
    TRADE_ITEM_RESEARCH_AGREEMENT,
    TRADE_ITEM_TRADE_AGREEMENT,
    TRADE_ITEM_PERMANENT_ALLIANCE,
    TRADE_ITEM_SURRENDER,
    TRADE_ITEM_TRUCE,
    TRADE_ITEM_PEACE_TREATY,
    TRADE_ITEM_THIRD_PARTY_PEACE,
    TRADE_ITEM_THIRD_PARTY_WAR,
    TRADE_ITEM_THIRD_PARTY_EMBARGO,
    TRADE_ITEM_ALLOW_EMBASSY,
	TRADE_ITEM_DECLARATION_OF_FRIENDSHIP,
	TRADE_ITEM_VOTE_COMMITMENT,
    NUM_TRADEABLE_ITEMS,
};
FDataStream& operator>>(FDataStream&, TradeableItems&);
FDataStream& operator<<(FDataStream&, const TradeableItems&);









struct CvTradedItem
{
	CvTradedItem();
	bool operator==(const CvTradedItem& rhs) const;

	TradeableItems m_eItemType;
	int m_iDuration;
#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	int m_iTurnsRemaining;
#else
	int m_iFinalTurn;
#endif
	int m_iData1;
	int m_iData2;
	int m_iData3;
#ifdef AUI_WARNING_FIXES
	PlayerTypes m_eFromPlayer;
	bool m_bFlag1;
#else
	bool m_bFlag1;
	PlayerTypes m_eFromPlayer;
#endif
	bool m_bFromRenewed;
	bool m_bToRenewed;
};
FDataStream& OldLoad(FDataStream&, CvTradedItem&);
FDataStream& operator>>(FDataStream&, CvTradedItem&);
FDataStream& operator<<(FDataStream&, const CvTradedItem&);
typedef FFastList< CvTradedItem, c_eMPoolTypeGame, 0 > TradedItemList;












class CvDeal
{
public:
	enum DealRenewStatus
	{
	    DEAL_RENEWABLE,
	    DEAL_NONRENEWABLE,
	    DEAL_SUPPLEMENTAL
	};

	CvDeal();
	CvDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer);
	CvDeal(const CvDeal& source);
	virtual ~CvDeal();
	CvDeal& operator=(const CvDeal& source);


	PlayerTypes m_eFromPlayer;
	PlayerTypes m_eToPlayer;
	int m_iStartTurn;
	int m_iFinalTurn;
	int m_iDuration;

	PeaceTreatyTypes m_ePeaceTreatyType;
	PlayerTypes m_eSurrenderingPlayer;
	PlayerTypes m_eDemandingPlayer;
	PlayerTypes m_eRequestingPlayer;

	bool m_bConsideringForRenewal;
	bool m_bCheckedForRenewal;
	bool m_bDealCancelled;

	TradedItemList m_TradedItems;

	void ClearItems();
	int GetNumItems();

	void SetFromPlayer(PlayerTypes ePlayer);
	void SetToPlayer(PlayerTypes ePlayer);

	PlayerTypes GetOtherPlayer(PlayerTypes eFromPlayer) const;
	PlayerTypes GetToPlayer()   const
	{
		return m_eToPlayer;
	};
	PlayerTypes GetFromPlayer() const
	{
		return m_eFromPlayer;
	};
	uint GetStartTurn() const
	{
		return m_iStartTurn;
	};
	uint GetDuration()  const
	{
		return m_iDuration;
	};
	uint GetEndTurn()   const
	{
		return m_iFinalTurn;
	};


	PeaceTreatyTypes GetPeaceTreatyType() const;
	void SetPeaceTreatyType(PeaceTreatyTypes eTreaty);

	PlayerTypes GetSurrenderingPlayer() const;
	void SetSurrenderingPlayer(PlayerTypes ePlayer);


	PlayerTypes GetDemandingPlayer() const;
	void SetDemandingPlayer(PlayerTypes ePlayer);


	PlayerTypes GetRequestingPlayer() const;
	void SetRequestingPlayer(PlayerTypes ePlayer);



	int GetGoldAvailable(PlayerTypes ePlayer, TradeableItems eItemToBeChanged);

	bool IsPossibleToTradeItem(PlayerTypes ePlayer, PlayerTypes eToPlayer, TradeableItems eItem, int iData1 = -1, int iData2 = -1, int iData3 = -1, bool bFlag1 = false, bool bCheckOtherPlayerValidity = true, bool bFinalizing = false);
	int GetNumResource(PlayerTypes ePlayer, ResourceTypes eResource);
#ifdef LEKMOD_LUXURY_DUPLICATE_TRADE_TOOLTIP
	bool IsLuxuryTradeTargetAlreadyHasResource(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, ResourceTypes eResource);
#endif


	void AddGoldTrade(PlayerTypes eFrom, int iAmount);
	void AddGoldPerTurnTrade(PlayerTypes eFrom, int iAmount, int iDuration);
	void AddMapTrade(PlayerTypes eFrom);
	void AddResourceTrade(PlayerTypes eFrom, ResourceTypes eResource, int iAmount, int iDuration);
	void AddCityTrade(PlayerTypes eFrom, int iCityID);
	void AddUnitTrade(PlayerTypes eFrom, int iUnitID);
	void AddAllowEmbassy(PlayerTypes eFrom);
	void AddOpenBorders(PlayerTypes eFrom, int iDuration);
	void AddDefensivePact(PlayerTypes eFrom, int iDuration);
	void AddResearchAgreement(PlayerTypes eFrom, int iDuration);
	void AddTradeAgreement(PlayerTypes eFrom, int iDuration);
	void AddPermamentAlliance();
	void AddSurrender(PlayerTypes eFrom);
	void AddTruce();
	void AddPeaceTreaty(PlayerTypes eFrom, int iDuration);
	void AddThirdPartyPeace(PlayerTypes eFrom, TeamTypes eThirdPartyTeam, int iDuration);
	void AddThirdPartyWar(PlayerTypes eFrom, TeamTypes eThirdPartyTeam);
	void AddThirdPartyEmbargo(PlayerTypes eFrom, PlayerTypes eThirdParty, int iDuration);
	void AddDeclarationOfFriendship(PlayerTypes eFrom);
	void AddVoteCommitment(PlayerTypes eFrom, int iResolutionID, int iVoteChoice, int iNumVotes, bool bRepeal);

	int GetGoldTrade(PlayerTypes eFrom);
	bool ChangeGoldTrade(PlayerTypes eFrom, int iNewAmount);

	int GetGoldPerTurnTrade(PlayerTypes eFrom);
	bool ChangeGoldPerTurnTrade(PlayerTypes eFrom, int iNewAmount, int iDuration);

	bool IsResourceTrade(PlayerTypes eFrom, ResourceTypes eResource);
	bool ChangeResourceTrade(PlayerTypes eFrom, ResourceTypes eResource, int iAmount, int iDuration);
	bool IsCityTrade(PlayerTypes eFrom, int x, int y);
	void ChangeThirdPartyWarDuration(PlayerTypes eFrom, TeamTypes eThirdPartyTeam, int iNewDuration);
	void ChangeThirdPartyPeaceDuration(PlayerTypes eFrom, TeamTypes eThirdPartyTeam, int iNewDuration);
	void ChangeThirdPartyEmbargoDuration(PlayerTypes eFrom, PlayerTypes eThirdParty, int iNewDuration);

	bool IsAllowEmbassyTrade(PlayerTypes eFrom);
	bool IsOpenBordersTrade(PlayerTypes eFrom);
	bool IsDefensivePactTrade(PlayerTypes eFrom);
	bool IsResearchAgreementTrade(PlayerTypes eFrom);
	bool IsTradeAgreementTrade(PlayerTypes eFrom);
	bool IsPeaceTreatyTrade(PlayerTypes eFrom);
	bool IsThirdPartyPeaceTrade(PlayerTypes eFrom, TeamTypes eThirdPartyTeam);
	bool IsThirdPartyWarTrade(PlayerTypes eFrom, TeamTypes eThirdPartyTeam);
	bool IsVoteCommitmentTrade(PlayerTypes eFrom);
	static DealRenewStatus GetItemTradeableState(TradeableItems eItem);
	bool IsPotentiallyRenewable();

	void RemoveByType(TradeableItems eType, PlayerTypes eFrom = NO_PLAYER);
	void RemoveResourceTrade(ResourceTypes eResource);
	void RemoveCityTrade(PlayerTypes eFrom, int iCityID);
	void RemoveUnitTrade(int iUnitID);
	void RemoveThirdPartyPeace(PlayerTypes eFrom, TeamTypes eThirdPartyTeam);
	void RemoveThirdPartyWar(PlayerTypes eFrom, TeamTypes eThirdPartyTeam);
	void RemoveThirdPartyEmbargo(PlayerTypes eFrom, PlayerTypes eThirdParty);
	void RemoveVoteCommitment(PlayerTypes eFrom, int iResolutionID, int iVoteChoice, int iNumVotes, bool bRepeal);

	bool ContainsItemType(TradeableItems eItemType, PlayerTypes eFrom = NO_PLAYER);
};

FDataStream& OldLoad(FDataStream&, CvDeal&);
FDataStream& operator>>(FDataStream&, CvDeal&);
FDataStream& operator<<(FDataStream&, const CvDeal&);

typedef FStaticVector<CvDeal, 20, false, c_eCiv5GameplayDLL > DealList;










class CvGameDeals
{
public:
	CvGameDeals();
	virtual ~CvGameDeals();
	void Init();

#ifdef AUI_WARNING_FIXES
	void AddProposedDeal(const CvDeal& kDeal);
#else
	void AddProposedDeal(CvDeal kDeal);
#endif
	bool FinalizeDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, bool bAccepted);
#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	void DoTurn(PlayerTypes eForPlayer);

	void DoUpdateCurrentDealsList(PlayerTypes eForPlayer);
#else
	void DoTurn();	

	void DoUpdateCurrentDealsList();
#endif

	CvDeal* GetTempDeal();
	void SetTempDeal(CvDeal* pDeal);

	PlayerTypes HasMadeProposal(PlayerTypes eFromPlayer);
	bool ProposedDealExists(PlayerTypes eFromPlayer, PlayerTypes eToPlayer);
	CvDeal* GetProposedDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer);
#ifdef LEKMOD_PENDING_DEAL_TURN_PROMPT
	void GetIncomingDealSenders(PlayerTypes eToPlayer, std::vector<PlayerTypes>& vSenders, bool bHumanOnly) const;
#endif

	CvDeal* GetCurrentDeal(PlayerTypes ePlayer, uint index);
	CvDeal* GetHistoricDeal(PlayerTypes ePlayer, uint indx);
	uint GetNumCurrentDeals(PlayerTypes ePlayer);
	uint GetNumHistoricDeals(PlayerTypes ePlayer);
#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	CvDeal* GetCurrentlyEndingDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, uint indx);
	uint GetNumCurrentlyEndingDeals(PlayerTypes eFromPlayer, PlayerTypes eToPlayer) const;
#endif

	uint CreateDeal();
	CvDeal* GetDeal(uint index);
	void DestroyDeal(uint index);

	void DoCancelDealsBetweenTeams(TeamTypes eTeam1, TeamTypes eTeam2);
	void DoCancelDealsBetweenPlayers(PlayerTypes eFromPlayer, PlayerTypes eToPlayer);
	void DoCancelAllDealsWithPlayer(PlayerTypes eCancelPlayer);
	void DoCancelAllProposedDealsWithPlayer(PlayerTypes eCancelPlayer);
#ifdef LEKMOD_LUXURY_BAN_CANCEL_DEALS
	void DoCancelAllDealsWithResource(ResourceTypes eResource);
#endif
	void DoEndTradedItem(CvTradedItem* pItem, PlayerTypes eToPlayer, bool bCancelled);

	int GetTradeItemGoldCost(TradeableItems eItem, PlayerTypes ePlayer1, PlayerTypes ePlayer2) const;

	static void PrepareRenewDeal(CvDeal* pOldDeal, const CvDeal* pNewDeal);


	DealList m_ProposedDeals;
	DealList m_CurrentDeals;
	DealList m_HistoricalDeals;
#ifdef AUI_YIELDS_APPLIED_AFTER_TURN_NOT_BEFORE
	DealList m_CurrentlyEndingDeals;
#endif

protected:
	void LogDealComplete(CvDeal* pDeal);

	CvDeal m_TempDeal;





	std::vector<std::pair<uint, CvDeal*> > m_Deals;
	unsigned int m_uiDealCounter;
};

FDataStream& OldLoad(FDataStream& loadFrom, CvGameDeals& writeTo);
FDataStream& operator>>(FDataStream&, CvGameDeals&);
FDataStream& operator<<(FDataStream&, const CvGameDeals&);

#endif
