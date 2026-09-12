






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvDealAI.h"
#include "CvDealClasses.h"
#include "CvDiplomacyAI.h"
#include "CvMinorCivAI.h"
#include "CvDllInterfaces.h"
#include "CvGrandStrategyAI.h"


#include "LintFree.h"




CvDealAI::CvDealAI()
{
#ifdef AUI_WARNING_FIXES
	Init(NULL);
#endif
}


CvDealAI::~CvDealAI(void)
{
	Uninit();
}


void CvDealAI::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	Reset();
}


void CvDealAI::Uninit()
{
}


void CvDealAI::Reset()
{
	m_iCachedValueOfPeaceWithHuman = 0;
}


void CvDealAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;
}


void CvDealAI::Write(FDataStream& kStream) const
{

	uint uiVersion = 1;
	kStream << uiVersion;
}


CvPlayer* CvDealAI::GetPlayer()
{
	return m_pPlayer;
}


TeamTypes CvDealAI::GetTeam()
{
	return m_pPlayer->getTeam();
}


int CvDealAI::GetDealPercentLeewayWithAI() const
{
	return 25;
}


int CvDealAI::GetDealPercentLeewayWithHuman() const
{
	return 10;
}


DealOfferResponseTypes CvDealAI::DoHumanOfferDealToThisAI(CvDeal* pDeal)
{
	DealOfferResponseTypes eResponse = NO_DEAL_RESPONSE_TYPE;
	DiploUIStateTypes eUIState = NO_DIPLO_UI_STATE;

	const char* szText = "";
	LeaderheadAnimationTypes eAnimation = NO_LEADERHEAD_ANIM;

	PlayerTypes eFromPlayer = GC.getGame().getActivePlayer();

	bool bFromIsActivePlayer = eFromPlayer == GC.getGame().getActivePlayer();

	int iDealValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer;
	bool bCantMatchOffer;
	bool bDealAcceptable = IsDealWithHumanAcceptable(pDeal, eFromPlayer,                         iDealValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, bCantMatchOffer);


	if(!bDealAcceptable)
	{
		if(!pDeal->IsPeaceTreatyTrade(eFromPlayer) && iValueTheyreOffering > iValueImOffering)
		{
			bDealAcceptable = true;
		}
	}

	if(bDealAcceptable)
	{
		CvDeal kDeal = *pDeal;


		if(GET_PLAYER(eFromPlayer).isHuman())
		{
			GC.GetEngineUserInterface()->SetDealInTransit(true);
			auto_ptr<ICvDeal1> pDllDeal = GC.WrapDealPointer(&kDeal);
			gDLL->sendNetDealAccepted(eFromPlayer, GetPlayer()->GetID(), pDllDeal.get(), iDealValueToMe, iValueImOffering, iValueTheyreOffering);
		}

		else
		{
			DoAcceptedDeal(eFromPlayer, kDeal, iDealValueToMe, iValueImOffering, iValueTheyreOffering);
		}
	}

	else if(iDealValueToMe > -75 &&
	        iValueImOffering < (iValueTheyreOffering * 5))
	{
		eResponse = DEAL_RESPONSE_UNACCEPTABLE;
		eUIState = DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER;

		if(bFromIsActivePlayer)
		{
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_REJECT_UNACCEPTABLE);
			eAnimation = LEADERHEAD_ANIM_NO;
		}
	}

	else
	{
		eResponse = DEAL_RESPONSE_INSULTING;
		eUIState = DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER;

		if(bFromIsActivePlayer)
		{
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_REJECT_INSULTING);
			eAnimation = LEADERHEAD_ANIM_NO;
		}
	}

	if(bFromIsActivePlayer)
	{

		if(eResponse >= DEAL_RESPONSE_UNACCEPTABLE)
		{
			int iTimesDealOffered = GC.GetEngineUserInterface()->GetOfferTradeRepeatCount();
			if(iTimesDealOffered > 4)
			{
				szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_REPEAT_TRADE_TOO_MUCH);
			}
			else if(iTimesDealOffered > 1)
			{
				szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_REPEAT_TRADE);
			}

			GC.GetEngineUserInterface()->ChangeOfferTradeRepeatCount(1);
			gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), eUIState, szText, eAnimation);
		}
	}

	return eResponse;
}


void CvDealAI::DoAcceptedDeal(PlayerTypes eFromPlayer, const CvDeal& kDeal, int iDealValueToMe, int iValueImOffering, int iValueTheyreOffering)
{
	int iDealType = -1;
	if(m_pPlayer->GetDiplomacyAI()->GetDealToRenew(&iDealType))
	{
		if (iDealType != 0)
		{

			CvGameDeals::PrepareRenewDeal(m_pPlayer->GetDiplomacyAI()->GetDealToRenew(), &kDeal);
		}
		m_pPlayer->GetDiplomacyAI()->ClearDealToRenew();
	}

	GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
	GC.getGame().GetGameDeals()->FinalizeDeal(eFromPlayer, GetPlayer()->GetID(), true);

	if(GET_PLAYER(eFromPlayer).isHuman())
	{
		iDealValueToMe -= GetCachedValueOfPeaceWithHuman();


		SetCachedValueOfPeaceWithHuman(0);

		LeaderheadAnimationTypes eAnimation;

		if(GC.getGame().getActivePlayer() == eFromPlayer)
			gDLL->DoClearDiplomacyTradeTable();

		DiploUIStateTypes eUIState = NO_DIPLO_UI_STATE;

		const char* szText = 0;


		if(kDeal.GetDemandingPlayer() == GetPlayer()->GetID())
		{
			if(GC.getGame().getActivePlayer() == eFromPlayer)
			{
				szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_AI_DEMAND);
				gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_POSITIVE);
			}

			return;
		}


		if(kDeal.GetRequestingPlayer() == GetPlayer()->GetID())
		{
			if(GC.getGame().getActivePlayer() == eFromPlayer)
			{
				szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_THANKFUL);
				gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
			}
			GetPlayer()->GetDiplomacyAI()->ChangeRecentAssistValue(eFromPlayer, -iDealValueToMe);
			return;
		}

		eUIState = DIPLO_UI_STATE_BLANK_DISCUSSION;


		if(iDealValueToMe >= 100 ||
		        iValueTheyreOffering > (iValueImOffering * 5))
		{
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_GENEROUS);
			eAnimation = LEADERHEAD_ANIM_YES;
			GetPlayer()->GetDiplomacyAI()->ChangeRecentTradeValue(eFromPlayer, iDealValueToMe);
		}

		else
		{
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_ACCEPTABLE);
			eAnimation = LEADERHEAD_ANIM_YES;
			GetPlayer()->GetDiplomacyAI()->ChangeRecentTradeValue(eFromPlayer, (iDealValueToMe / 2));
		}

		if(GC.getGame().getActivePlayer() == eFromPlayer)
			GC.GetEngineUserInterface()->SetOfferTradeRepeatCount(0);


		if(kDeal.GetPeaceTreatyType() != NO_PEACE_TREATY_TYPE)
		{
			eAnimation = LEADERHEAD_ANIM_PEACEFUL;
		}


		if(GC.getGame().getActivePlayer() == eFromPlayer)
			gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), eUIState, szText, eAnimation);
	}

	if(GC.getGame().getActivePlayer() == eFromPlayer || GC.getGame().getActivePlayer() == GetPlayer()->GetID())
	{
		GC.GetEngineUserInterface()->makeInterfaceDirty();
	}
}


DemandResponseTypes CvDealAI::DoHumanDemand(CvDeal* pDeal)
{
	DemandResponseTypes eResponse = NO_DEMAND_RESPONSE_TYPE;

	PlayerTypes eFromPlayer = GC.getGame().getActivePlayer();
	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	int iValueWillingToGiveUp = 0;

	CvDiplomacyAI* pDiploAI = GET_PLAYER(eMyPlayer).GetDiplomacyAI();

	if(pDiploAI->IsDemandTooSoon(eFromPlayer))
		eResponse = DEMAND_RESPONSE_REFUSE_TOO_SOON;


	else
	{
		MajorCivApproachTypes eApproach = pDiploAI->GetMajorCivApproach(eFromPlayer,                       true);
		StrengthTypes eMilitaryStrength = pDiploAI->GetPlayerMilitaryStrengthComparedToUs(eFromPlayer);
		AggressivePostureTypes eMilitaryPosture = pDiploAI->GetMilitaryAggressivePosture(eFromPlayer);
		PlayerProximityTypes eProximity = GET_PLAYER(eMyPlayer).GetProximityToPlayer(eFromPlayer);


		if(pDiploAI->GetMajorCivOpinion(eFromPlayer) == MAJOR_CIV_OPINION_UNFORGIVABLE)
			eResponse = DEMAND_RESPONSE_REFUSE_HOSTILE;


		else if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
			eResponse = DEMAND_RESPONSE_REFUSE_HOSTILE;


		else if(eMilitaryStrength < STRENGTH_AVERAGE)
			eResponse = DEMAND_RESPONSE_REFUSE_WEAK;


		else if(eProximity <= PLAYER_PROXIMITY_FAR && eMilitaryPosture == AGGRESSIVE_POSTURE_NONE)
			eResponse = DEMAND_RESPONSE_REFUSE_WEAK;


		else
		{

			int iOddsOfGivingIn = (10 - pDiploAI->GetBoldness()) * 10;

			iValueWillingToGiveUp = 0;


			if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
			{
				iOddsOfGivingIn += 50;
				iValueWillingToGiveUp += 200;
			}

			else
			{

				switch(eMilitaryStrength)
				{
				case STRENGTH_PATHETIC:
					iOddsOfGivingIn += -100;
					iValueWillingToGiveUp += 10;
					break;
				case STRENGTH_WEAK:
					iOddsOfGivingIn += -100;
					iValueWillingToGiveUp += 10;
					break;
				case STRENGTH_POOR:
					iOddsOfGivingIn += -100;
					iValueWillingToGiveUp += 20;
					break;
				case STRENGTH_AVERAGE:
					iOddsOfGivingIn += -10;
					iValueWillingToGiveUp += 50;
					break;
				case STRENGTH_STRONG:
					iOddsOfGivingIn += 10;
					iValueWillingToGiveUp += 120;
					break;
				case STRENGTH_POWERFUL:
					iOddsOfGivingIn += 20;
					iValueWillingToGiveUp += 200;
					break;
				case STRENGTH_IMMENSE:
					iOddsOfGivingIn += 35;
					iValueWillingToGiveUp += 200;
					break;
				default:
					break;
				}
			}




			int iAsyncRand = GC.getGame().getAsyncRandNum(100, "Deal AI: ASYNC RAND call to determine if AI will give into a human demand.");


			if(iAsyncRand > iOddsOfGivingIn)
				eResponse = DEMAND_RESPONSE_REFUSE_HOSTILE;
		}
	}


#ifdef AUI_WARNING_FIXES
	if (eResponse == NO_DEMAND_RESPONSE_TYPE)
#else
	if(eResponse == NO_DEAL_RESPONSE_TYPE)
#endif
	{
		int iValueDemanded = 0;

		int iGPT = GetPlayer()->GetTreasury()->CalculateBaseNetGold();
		int iTempGold;
		int iModdedGoldValue;


		TradedItemList::iterator it;
		for(it = pDeal->m_TradedItems.begin(); it != pDeal->m_TradedItems.end(); ++it)
		{

			if(it->m_eFromPlayer == eMyPlayer)
			{
				switch(it->m_eItemType)
				{

				case TRADE_ITEM_GOLD:
				{
					iTempGold = it->m_iData1;
					if (iGPT > 0)
						iModdedGoldValue = iTempGold * 10 / iGPT;
					else
						iModdedGoldValue = 0;

					iValueDemanded += max(iTempGold, iModdedGoldValue);
					break;
				}


				case TRADE_ITEM_GOLD_PER_TURN:
				{
					iValueDemanded += (it->m_iData1 * it->m_iDuration * 80 / 100);
					break;
				}


				case TRADE_ITEM_RESOURCES:
				{
					ResourceTypes eResource = (ResourceTypes) it->m_iData1;
					ResourceUsageTypes eUsage = GC.getResourceInfo(eResource)->getResourceUsage();

					if(eUsage == RESOURCEUSAGE_LUXURY)
						iValueDemanded += 200;
					else if(eUsage == RESOURCEUSAGE_STRATEGIC)
						iValueDemanded += (40 * it->m_iData2);

					break;
				}


				case TRADE_ITEM_OPEN_BORDERS:
				{
					iValueDemanded += 50;
					break;
				}

				case TRADE_ITEM_CITIES:
				case TRADE_ITEM_DEFENSIVE_PACT:
				case TRADE_ITEM_RESEARCH_AGREEMENT:
				case TRADE_ITEM_PERMANENT_ALLIANCE:
				case TRADE_ITEM_THIRD_PARTY_PEACE:
				case TRADE_ITEM_THIRD_PARTY_WAR:
				case TRADE_ITEM_THIRD_PARTY_EMBARGO:
				default:
					eResponse = DEMAND_RESPONSE_REFUSE_TOO_MUCH;
					break;
				}
			}
		}


#ifdef AUI_WARNING_FIXES
		if (eResponse == NO_DEMAND_RESPONSE_TYPE)
#else
		if(eResponse == NO_DEAL_RESPONSE_TYPE)
#endif
		{
			if(iValueDemanded <= iValueWillingToGiveUp)
				eResponse = DEMAND_RESPONSE_ACCEPT;
			else
				eResponse = DEMAND_RESPONSE_REFUSE_TOO_MUCH;
		}
	}


	GC.getGame().DoFromUIDiploEvent(FROM_UI_DIPLO_EVENT_HUMAN_DEMAND, eMyPlayer,            eResponse, -1);


	if(eResponse == DEMAND_RESPONSE_ACCEPT)
	{
		CvDeal kDeal = *pDeal;

		GC.GetEngineUserInterface()->SetDealInTransit(true);

		auto_ptr<ICvDeal1> pDllDeal = GC.WrapDealPointer(&kDeal);
		gDLL->sendNetDemandAccepted(eFromPlayer, GetPlayer()->GetID(), pDllDeal.get());
	}

	return eResponse;
}


void CvDealAI::DoAcceptedDemand(PlayerTypes eFromPlayer, const CvDeal& kDeal)
{
	CvGame& kGame = GC.getGame();
	CvGameDeals* pGameDeals = kGame.GetGameDeals();
	const PlayerTypes eActivePlayer = kGame.getActivePlayer();
	const PlayerTypes ePlayer = GetPlayer()->GetID();

	pGameDeals->AddProposedDeal(kDeal);
	pGameDeals->FinalizeDeal(eFromPlayer, ePlayer, true);
	if(eActivePlayer == eFromPlayer || eActivePlayer == ePlayer)
	{
		GC.GetEngineUserInterface()->makeInterfaceDirty();
	}
}


bool CvDealAI::IsDealWithHumanAcceptable(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValueToMe, int& iValueImOffering, int& iValueTheyreOffering, int& iAmountOverWeWillRequest, int& iAmountUnderWeWillOffer, bool& bCantMatchOffer)
{
	CvAssertMsg(GET_PLAYER(eOtherPlayer).isHuman(), "DEAL_AI: Trying to see if AI will accept a deal with human player... but it's not human.  Please show Jon.");

	int iPercentOverWeWillRequest;
	int iPercentUnderWeWillOffer;

	bCantMatchOffer = false;


	iPercentOverWeWillRequest = GetDealPercentLeewayWithHuman();
	iPercentUnderWeWillOffer = 0;


	iTotalValueToMe = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering,                   false);


	if (pDeal->GetGoldTrade(eOtherPlayer) == 0 && pDeal->GetGoldTrade(m_pPlayer->GetID()) == 0)
	{
		int iOneGPT = 25;
		int iDiff = abs(iValueTheyreOffering - iValueImOffering);
		if (iDiff < iOneGPT)
		{
			return true;
		}
	}

	int iDealSumValue = iValueImOffering + iValueTheyreOffering;

	iAmountOverWeWillRequest = iDealSumValue;
	iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
	iAmountOverWeWillRequest /= 100;

	iAmountUnderWeWillOffer = iDealSumValue;
	iAmountUnderWeWillOffer *= iPercentUnderWeWillOffer;
	iAmountUnderWeWillOffer /= 100;


	if(pDeal->GetSurrenderingPlayer() == GetPlayer()->GetID())
	{
		if (iTotalValueToMe >= GetCachedValueOfPeaceWithHuman())
		{
			return true;
		}
	}


	else if (pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
#ifdef NQM_AI_GIMP_ALWAYS_WHITE_PEACE
		int iPeaceValueRequired = GetCachedValueOfPeaceWithHuman();
		if ((GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_ALWAYS_WHITE_PEACE")) && iPeaceValueRequired > 0 && GET_PLAYER(eOtherPlayer).isHuman())
			iPeaceValueRequired = 0;
		if (iTotalValueToMe >= iPeaceValueRequired)
#else
		if (iTotalValueToMe >= GetCachedValueOfPeaceWithHuman())
#endif
		{
			return true;
		}
	}


	else if(iTotalValueToMe <= iAmountOverWeWillRequest && iTotalValueToMe >= iAmountUnderWeWillOffer)
	{
		return true;
	}
	else if (iTotalValueToMe > iAmountOverWeWillRequest)
	{
		bCantMatchOffer = true;
	}

	return false;
}


bool CvDealAI::DoEqualizeDealWithHuman(CvDeal* pDeal, PlayerTypes eOtherPlayer, bool bDontChangeMyExistingItems, bool bDontChangeTheirExistingItems, bool& bDealGoodToBeginWith, bool& bCantMatchOffer)
{
	bool bMakeOffer;
#ifdef CVASSERT_ENABLE
	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	DEBUG_VARIABLE(eMyPlayer);

	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);
	CvAssertMsg(eMyPlayer != eOtherPlayer, "DEAL_AI: Trying to equalize human deal, but both players are the same.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
#endif

	int iDealDuration = GC.getGame().GetDealDuration();
	bCantMatchOffer = false;


	if (pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		pDeal->ClearItems();
		bMakeOffer = IsOfferPeace(eOtherPlayer, pDeal, true                     );
	}
	else
	{
		int iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer;
		bMakeOffer = IsDealWithHumanAcceptable(pDeal, GC.getGame().getActivePlayer(),                         iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, bCantMatchOffer);

		if (iTotalValueToMe < 0 && bDontChangeTheirExistingItems)
		{
			return false;
		}

		if(bMakeOffer)
		{
			bDealGoodToBeginWith = true;
		}
		else
		{
			bDealGoodToBeginWith = false;
		}

		if(!bMakeOffer)
		{




			bool bUseEvenValue = false;





			DoAddVoteCommitmentToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
			DoAddVoteCommitmentToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);

			DoAddEmbassyToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
			DoAddEmbassyToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);

			DoAddResourceToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iDealDuration, bUseEvenValue);
			DoAddResourceToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountUnderWeWillOffer, iDealDuration, bUseEvenValue);

			DoAddOpenBordersToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iDealDuration, bUseEvenValue);
			DoAddOpenBordersToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountUnderWeWillOffer, iDealDuration, bUseEvenValue);

			DoAddGPTToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iDealDuration, bUseEvenValue);
			DoAddGPTToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iDealDuration, bUseEvenValue);

			DoAddGoldToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
			DoAddGoldToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, bUseEvenValue);

			if (!bDontChangeTheirExistingItems)
			{
				DoRemoveGPTFromThem(pDeal, eOtherPlayer, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iDealDuration, bUseEvenValue);
			}
			if (!bDontChangeMyExistingItems)
			{
				DoRemoveGPTFromUs(pDeal, eOtherPlayer, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iDealDuration, bUseEvenValue);
			}

			DoRemoveGoldFromUs(pDeal, eOtherPlayer, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
			DoRemoveGoldFromThem(pDeal, eOtherPlayer, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, bUseEvenValue);

			DoAddCitiesToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);


			if(pDeal->m_TradedItems.size() > 0)
			{
				bMakeOffer = IsDealWithHumanAcceptable(pDeal, GC.getGame().getActivePlayer(),                         iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer,                        bCantMatchOffer);
			}
		}
	}

	return bMakeOffer;
}



bool CvDealAI::DoEqualizeDealWithAI(CvDeal* pDeal, PlayerTypes eOtherPlayer)
{
#ifdef CVASSERT_ENABLE
	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	DEBUG_VARIABLE(eMyPlayer);

	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);
	CvAssertMsg(eMyPlayer != eOtherPlayer, "DEAL_AI: Trying to equalize AI deal, but both players are the same.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
#endif

	int iEvenValueImOffering;
	int iEvenValueTheyreOffering;
	int iTotalValue = GetDealValue(pDeal, iEvenValueImOffering, iEvenValueTheyreOffering,                   true);

	int iDealDuration = GC.getGame().GetDealDuration();

	bool bMakeOffer = false;





	int iPercentOverWeWillRequest = GetDealPercentLeewayWithAI();
	int iPercentUnderWeWillOffer = -GetDealPercentLeewayWithAI();

	int iDealSumValue = iEvenValueImOffering + iEvenValueTheyreOffering;

	int iAmountOverWeWillRequest = iDealSumValue;
	iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
	iAmountOverWeWillRequest /= 100;

	int iAmountUnderWeWillOffer = iDealSumValue;
	iAmountUnderWeWillOffer *= iPercentUnderWeWillOffer;
	iAmountUnderWeWillOffer /= 100;


	if(iTotalValue <= iAmountOverWeWillRequest && iTotalValue >= iAmountUnderWeWillOffer)
	{
		bMakeOffer = true;
	}


	if(pDeal != GC.getGame().GetGameDeals()->GetTempDeal())
	{
		GC.getGame().GetGameDeals()->SetTempDeal(pDeal);
	}

	CvDeal* pCounterDeal = GC.getGame().GetGameDeals()->GetTempDeal();

	if(!bMakeOffer)
	{




		bool bUseEvenValue = true;

		DoAddVoteCommitmentToThem(pCounterDeal, eOtherPlayer,                                   false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
		DoAddVoteCommitmentToUs(pCounterDeal, eOtherPlayer,                                false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);

		DoAddResourceToThem(pCounterDeal, eOtherPlayer,                                   false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, iDealDuration, bUseEvenValue);
		DoAddResourceToUs(pCounterDeal, eOtherPlayer,                                false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, iDealDuration, bUseEvenValue);

		DoAddOpenBordersToThem(pCounterDeal, eOtherPlayer,                                   true, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, iDealDuration, bUseEvenValue);
		DoAddOpenBordersToUs(pCounterDeal, eOtherPlayer,                                true, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, iDealDuration, bUseEvenValue);

		DoAddGPTToThem(pCounterDeal, eOtherPlayer,                                   false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);
		DoAddGPTToUs(pCounterDeal, eOtherPlayer,                                false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);

		DoAddGoldToThem(pCounterDeal, eOtherPlayer,                                   false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, bUseEvenValue);
		DoAddGoldToUs(pCounterDeal, eOtherPlayer,                                false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, bUseEvenValue);

		DoRemoveGPTFromThem(pCounterDeal, eOtherPlayer, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);
		DoRemoveGPTFromUs(pCounterDeal, eOtherPlayer, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);

		DoRemoveGoldFromUs(pCounterDeal, eOtherPlayer, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, bUseEvenValue);
		DoRemoveGoldFromThem(pCounterDeal, eOtherPlayer, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, bUseEvenValue);


		if(pCounterDeal->m_TradedItems.size() > 0)
		{
			int iValueIThinkImOffering, iValueIThinkImGetting;
			GetDealValue(pDeal, iValueIThinkImOffering, iValueIThinkImGetting,                   false);


			int iLowEndOfWhatIWillAccept = iValueIThinkImOffering - (iValueIThinkImOffering * -iPercentUnderWeWillOffer / 100);
			if(iValueIThinkImGetting < iLowEndOfWhatIWillAccept)
			{
				return false;
			}

			int iValueTheyThinkTheyreOffering, iValueTheyThinkTheyreGetting;
			GET_PLAYER(eOtherPlayer).GetDealAI()->GetDealValue(pDeal, iValueTheyThinkTheyreOffering, iValueTheyThinkTheyreGetting,                   false);


			int iLowEndOfWhatTheyWillAccept = iValueTheyThinkTheyreOffering - (iValueTheyThinkTheyreOffering * GET_PLAYER(eOtherPlayer).GetDealAI()->GetDealPercentLeewayWithAI() / 100);
			if(iValueTheyThinkTheyreGetting < iLowEndOfWhatTheyWillAccept)
			{
				return false;
			}

			bMakeOffer = true;
		}
	}

	return bMakeOffer;
}








int CvDealAI::GetDealValue(CvDeal* pDeal, int& iValueImOffering, int& iValueTheyreOffering, bool bUseEvenValue)
{
	int iDealValue = 0;
	iValueImOffering = 0;
	iValueTheyreOffering = 0;

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	int iItemValue;

	bool bFromMe;
	PlayerTypes eOtherPlayer;

	eOtherPlayer = pDeal->m_eFromPlayer == eMyPlayer ? pDeal->m_eToPlayer : pDeal->m_eFromPlayer;

	TradedItemList::iterator it;
	for(it = pDeal->m_TradedItems.begin(); it != pDeal->m_TradedItems.end(); ++it)
	{
		if(eMyPlayer == it->m_eFromPlayer)
		{
			bFromMe = true;
			eOtherPlayer = pDeal->GetOtherPlayer(eMyPlayer);
		}
		else
		{
			bFromMe = false;
			eOtherPlayer = it->m_eFromPlayer;
		}


		int iValueMultiplier = bFromMe ? -1 : 1;

		iItemValue = GetTradeItemValue(it->m_eItemType, bFromMe, eOtherPlayer, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1, it->m_iDuration, bUseEvenValue);

		iItemValue *= iValueMultiplier;
		iDealValue += iItemValue;


		if(iItemValue < 0)
		{
			iValueImOffering -= iItemValue;
		}
		else
		{
			iValueTheyreOffering += iItemValue;
		}
	}

	return iDealValue;
}


int CvDealAI::GetTradeItemValue(TradeableItems eItem, bool bFromMe, PlayerTypes eOtherPlayer, int iData1, int iData2, int iData3, bool bFlag1, int iDuration, bool bUseEvenValue)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to get deal item value for trading to oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eItem != TRADE_ITEM_NONE, "DEAL_AI: Trying to get value of TRADE_ITEM_NONE.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	if(eItem == TRADE_ITEM_GOLD)
		iItemValue = GetGoldForForValueExchange(                iData1,                       false, bFromMe, eOtherPlayer, bUseEvenValue,              false);
	else if(eItem == TRADE_ITEM_GOLD_PER_TURN)
		iItemValue = GetGPTforForValueExchange(                         iData1,                      false, iDuration, bFromMe, eOtherPlayer, bUseEvenValue,              false);
	else if(eItem == TRADE_ITEM_RESOURCES)
		iItemValue = GetResourceValue(                 (ResourceTypes) iData1,              iData2, iDuration, bFromMe, eOtherPlayer);
	else if(eItem == TRADE_ITEM_CITIES)
		iItemValue = GetCityValue(       iData1,        iData2, bFromMe, eOtherPlayer, bUseEvenValue);
	else if(eItem == TRADE_ITEM_ALLOW_EMBASSY)
		iItemValue = GetEmbassyValue(bFromMe, eOtherPlayer, bUseEvenValue);
	else if(eItem == TRADE_ITEM_OPEN_BORDERS)
		iItemValue = GetOpenBordersValue(bFromMe, eOtherPlayer, bUseEvenValue);
	else if(eItem == TRADE_ITEM_DEFENSIVE_PACT)
		iItemValue = GetDefensivePactValue(bFromMe, eOtherPlayer, bUseEvenValue);
	else if(eItem == TRADE_ITEM_RESEARCH_AGREEMENT)
		iItemValue = GetResearchAgreementValue(bFromMe, eOtherPlayer, bUseEvenValue);
	else if(eItem == TRADE_ITEM_TRADE_AGREEMENT)
		iItemValue = GetTradeAgreementValue(bFromMe, eOtherPlayer, bUseEvenValue);
	else if(eItem == TRADE_ITEM_PEACE_TREATY)
		iItemValue = GetPeaceTreatyValue(eOtherPlayer);
	else if(eItem == TRADE_ITEM_THIRD_PARTY_PEACE)
		iItemValue = GetThirdPartyPeaceValue(bFromMe, eOtherPlayer,               (TeamTypes) iData1);
	else if(eItem == TRADE_ITEM_THIRD_PARTY_WAR)
		iItemValue = GetThirdPartyWarValue(bFromMe, eOtherPlayer,               (TeamTypes) iData1);
	else if(eItem == TRADE_ITEM_VOTE_COMMITMENT)
		iItemValue = GetVoteCommitmentValue(bFromMe, eOtherPlayer, iData1, iData2, iData3, bFlag1, bUseEvenValue);

	CvAssertMsg(iItemValue >= 0, "DEAL_AI: Trade Item value is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return iItemValue;
}


int CvDealAI::GetGoldForForValueExchange(int iGoldOrValue, bool bNumGoldFromValue, bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue, bool bRoundUp)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of Gold with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iMultiplier;
	int iDivisor;


	if(bNumGoldFromValue)
	{
		iMultiplier = 100;
		iDivisor =         GC.getEACH_GOLD_VALUE_PERCENT();

		if(iDivisor == 0)
			iDivisor = 1;
	}

	else
	{
		iMultiplier =         GC.getEACH_GOLD_VALUE_PERCENT();
		iDivisor = 100;
	}


	int iReturnValue = iGoldOrValue * iMultiplier;

	int iModifier;


	if(bFromMe)
	{

		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer,                       true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iModifier = 150;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iModifier = 110;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iModifier = 100;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iModifier = 100;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iModifier = 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Gold valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iModifier = 100;
			break;
		}


		if(!bNumGoldFromValue)
		{
			iReturnValue *= iModifier;
			iReturnValue /= 100;
		}
		else
		{
			iReturnValue *= 100;
			iReturnValue /= iModifier;
		}


		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
		{
		case MAJOR_CIV_OPINION_ALLY:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_FRIEND:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_FAVORABLE:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_NEUTRAL:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_COMPETITOR:
			iModifier = 115;
			break;
		case MAJOR_CIV_OPINION_ENEMY:
			iModifier = 140;
			break;
		case MAJOR_CIV_OPINION_UNFORGIVABLE:
			iModifier = 200;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Opinion for Gold valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iReturnValue *= 100;
			break;
		}


		if(!bNumGoldFromValue)
		{
			iReturnValue *= iModifier;
			iReturnValue /= 100;
		}
		else
		{
			iReturnValue *= 100;
			iReturnValue /= iModifier;
		}
	}


	if(bRoundUp)
	{
		iReturnValue += 99;
	}

	iReturnValue /= iDivisor;


	if(bUseEvenValue)
	{
		iReturnValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetGoldForForValueExchange(iGoldOrValue, bNumGoldFromValue, !bFromMe, GetPlayer()->GetID(),                   false, bRoundUp);

		iReturnValue /= 2;
	}

	return iReturnValue;
}


int CvDealAI::GetGPTforForValueExchange(int iGPTorValue, bool bNumGPTFromValue, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue, bool bRoundUp)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of GPT with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iPreCalculationDurationMultiplier;
	int iMultiplier;
	int iDivisor;
	int iPostCalculationDurationDivider;


	if(bNumGPTFromValue)
	{
		iPreCalculationDurationMultiplier = 1;
		iMultiplier = 100;
		iDivisor =        GC.getEACH_GOLD_PER_TURN_VALUE_PERCENT();
		iPostCalculationDurationDivider = iNumTurns;





	}

	else
	{
		iPreCalculationDurationMultiplier = iNumTurns;
		iMultiplier =        GC.getEACH_GOLD_PER_TURN_VALUE_PERCENT();
		iDivisor = 100;
		iPostCalculationDurationDivider = 1;





	}


	int iReturnValue = iGPTorValue * iPreCalculationDurationMultiplier;
	iReturnValue *= iMultiplier;


	if(bFromMe)
	{


		if(!bNumGPTFromValue)
		{
			iReturnValue *= 130;
			iReturnValue /= 100;
		}
		else
		{
			iReturnValue *= 100;
			iReturnValue /= 130;
		}

		int iModifier;


		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer,                       true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iModifier = 150;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iModifier = 110;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iModifier = 100;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iModifier = 100;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iModifier = 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Gold valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iModifier = 100;
			break;
		}


		if(!bNumGPTFromValue)
		{
			iReturnValue *= iModifier;
			iReturnValue /= 100;
		}
		else
		{
			iReturnValue *= 100;
			iReturnValue /= iModifier;
		}


		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
		{
		case MAJOR_CIV_OPINION_ALLY:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_FRIEND:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_FAVORABLE:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_NEUTRAL:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_COMPETITOR:
			iModifier = 115;
			break;
		case MAJOR_CIV_OPINION_ENEMY:
			iModifier = 140;
			break;
		case MAJOR_CIV_OPINION_UNFORGIVABLE:
			iModifier = 200;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Opinion for Gold valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iModifier = 100;
			break;
		}


		if(!bNumGPTFromValue)
		{
			iReturnValue *= iModifier;
			iReturnValue /= 100;
		}
		else
		{
			iReturnValue *= 100;
			iReturnValue /= iModifier;
		}
	}


	if(bRoundUp)
	{
		iReturnValue += 99;
	}

	iReturnValue /= iDivisor;

	iReturnValue /= iPostCalculationDurationDivider;


	if(bUseEvenValue)
	{
		iReturnValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetGPTforForValueExchange(iGPTorValue, bNumGPTFromValue, iNumTurns, !bFromMe, GetPlayer()->GetID(),                   false, bRoundUp);

		iReturnValue /= 2;
	}

	return iReturnValue;
}


int CvDealAI::GetResourceValue(ResourceTypes eResource, int iResourceQuantity, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Resource with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
	CvAssert(pkResourceInfo != NULL);
	if (pkResourceInfo == NULL)
		return 0;

	ResourceUsageTypes eUsage = pkResourceInfo->getResourceUsage();


	if(eUsage == RESOURCEUSAGE_LUXURY)
	{
		if (GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(GetPlayer()->GetID(), eResource))
		{
			iItemValue = 0;
		}
		else
		{
			int iHappinessFromResource = pkResourceInfo->getHappiness();
			iItemValue += (iResourceQuantity * iHappinessFromResource * iNumTurns * 2);


			if(bFromMe)
			{
				if(GetPlayer()->getNumResourceAvailable(eResource) == 1)
				{
					iItemValue *= 3;
					if(GetPlayer()->GetPlayerTraits()->GetLuxuryHappinessRetention() > 0)
					{
						iItemValue /= 2;
					}
				}
			}
		}
	}

	else if(eUsage == RESOURCEUSAGE_STRATEGIC)
	{

		if(!bFromMe)
		{

			if(GetPlayer()->getNumResourceAvailable(eResource) > GetPlayer()->getNumCities())
			{
				iResourceQuantity = 0;
			}
			else
			{
				iResourceQuantity = min(max(5,GetPlayer()->getNumCities()), iResourceQuantity);
			}
		}
		if(!GET_TEAM(GetPlayer()->getTeam()).IsResourceObsolete(eResource))
		{
			iItemValue += (iResourceQuantity * iNumTurns * 150 / 100);
		}
		else
		{
			iItemValue = 0;
		}
	}


	if(bFromMe)
	{
		int iModifier = 0;


		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
		{
		case MAJOR_CIV_OPINION_ALLY:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_FRIEND:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_FAVORABLE:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_NEUTRAL:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_COMPETITOR:
			iModifier = 175;
			break;
		case MAJOR_CIV_OPINION_ENEMY:
			iModifier = 400;
			break;
		case MAJOR_CIV_OPINION_UNFORGIVABLE:
			iModifier = 1000;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Opinion for Resource valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iModifier = 100;
			break;
		}


		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer,                       true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iModifier += 300;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iModifier += 150;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iModifier = 200;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iModifier = 200;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iModifier += 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Resource valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iModifier += 100;
			break;
		}

		iItemValue *= iModifier;
		iItemValue /= 200;
	}

	return iItemValue;
}


int CvDealAI::GetCityValue(int iX, int iY, bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of City with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	CvCity* pCity = GC.getMap().plot(iX, iY)->getPlotCity();

	if(pCity != NULL)
	{
		iItemValue = 440 + (pCity->getPopulation() * 200);




		int goldPerPlot = GetPlayer()->GetBuyPlotCost();

		int iGoldValueOfPlots = 0;
		int iGoldValueOfImprovedPlots = 0;
		int iGoldValueOfResourcePlots = 0;
		for(int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			CvPlot* pLoopPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iI);
			if(NULL != pLoopPlot && pCity->GetID() == pLoopPlot->GetCityPurchaseID())
			{
				if(iI > 6)
				{
					iGoldValueOfPlots += goldPerPlot;
				}
				if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
				{
					iGoldValueOfImprovedPlots += goldPerPlot * 25;
				}
				ResourceTypes eResource = pLoopPlot->getNonObsoleteResourceType(GetPlayer()->getTeam());
				if(eResource != NO_RESOURCE)
				{
					const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
					if (pkResourceInfo)
					{
						ResourceUsageTypes eUsage = pkResourceInfo->getResourceUsage();
						int iResourceQuantity = pLoopPlot->getNumResource();

						if(eUsage == RESOURCEUSAGE_LUXURY)
						{
#ifdef AUI_WARNING_FIXES
							int iNumTurns = MIN(1, GC.getGame().getMaxTurns() - GC.getGame().getGameTurn());
							iNumTurns = MAX(120, iNumTurns);
#else
							int iNumTurns = min(1,GC.getGame().getMaxTurns() - GC.getGame().getGameTurn());
							iNumTurns = max(120,iNumTurns);
#endif
							int iHappinessFromResource = pkResourceInfo->getHappiness();
							iGoldValueOfResourcePlots += (iResourceQuantity * iHappinessFromResource * iNumTurns * 2);

							if(bFromMe)
							{
								if(GetPlayer()->getNumResourceAvailable(eResource) == 1)
								{
									iGoldValueOfResourcePlots += (iResourceQuantity * iHappinessFromResource * iNumTurns * 4);
								}
							}
						}

						else if(eUsage == RESOURCEUSAGE_STRATEGIC)
						{
							int iNumTurns = 60;
							iGoldValueOfResourcePlots += (iResourceQuantity * iNumTurns * 150 / 100);
						}
					}
				}
			}
		}
		iGoldValueOfImprovedPlots /= 100;

		iItemValue = iItemValue + iGoldValueOfPlots + iGoldValueOfImprovedPlots + iGoldValueOfResourcePlots;




		if(bFromMe)
		{

			if(pCity->hasActiveWorldWonder())
				iItemValue *= 2;


			switch(GetPlayer()->GetDiplomacyAI()->GetWarProjection(eOtherPlayer))
			{
			case WAR_PROJECTION_DESTRUCTION:
				iItemValue *= 100;
				break;
			case WAR_PROJECTION_DEFEAT:
				iItemValue *= 180;
				break;
			case WAR_PROJECTION_STALEMATE:
				iItemValue *= 220;
				break;
			case WAR_PROJECTION_UNKNOWN:
				iItemValue *= 250;
				break;
			case WAR_PROJECTION_GOOD:
				iItemValue *= 400;
				break;
			case WAR_PROJECTION_VERY_GOOD:
				iItemValue *= 400;
				break;
			default:
				CvAssertMsg(false, "DEAL_AI: AI player has no valid War Projection for City valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iItemValue *= 300;
				break;
			}
			iItemValue /= 100;


			if(!GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam()))
			{
				iItemValue *= 2;
			}

		}
		else
		{
			CvPlayerAI& theOtherPlayer = GET_PLAYER(eOtherPlayer);
			if(!GET_TEAM(GetTeam()).isAtWar(theOtherPlayer.getTeam()))
			{
				if(theOtherPlayer.isHuman())
				{
					CvCity* pLoopCity;
					int iCityLoop;
					int iBestDistance = 99;
					for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
					{
						int iDistFromThisCity = plotDistance(iX, iY, pLoopCity->getX(), pLoopCity->getY());
						if(iDistFromThisCity < iBestDistance)
						{
							iBestDistance = iDistFromThisCity;
						}
					}
					iBestDistance = (iBestDistance > 4) ? iBestDistance : 5;
					iItemValue /= iBestDistance - 4;
					iItemValue = (iItemValue >= 100) ? iItemValue : 100;
				}
			}
		}


		if (pCity->IsOriginalMajorCapital())
		{
			iItemValue *= 2;
		}
	}


	if(bUseEvenValue)
	{
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetCityValue(iX, iY, !bFromMe, GetPlayer()->GetID(),                   false);

		iItemValue /= 2;
	}

	return iItemValue;
}


int CvDealAI::GetEmbassyValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Embassy with oneself.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 35;


	CvGameSpeedInfo *pkStdSpeedInfo = GC.getGameSpeedInfo((GameSpeedTypes)GC.getSTANDARD_GAMESPEED());
	if (pkStdSpeedInfo)
	{
		iItemValue *= GC.getGame().getGameSpeedInfo().GetDealDuration();
		iItemValue /= pkStdSpeedInfo->GetDealDuration();
	}

	if(bFromMe)
	{

		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer,                       true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 250;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 130;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue *= 80;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue *= 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Research Agreement valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;
	}


	if(bUseEvenValue)
	{
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetTradeAgreementValue(!bFromMe, GetPlayer()->GetID(),                   false);

		iItemValue /= 2;
	}

	return iItemValue;
}


int CvDealAI::GetOpenBordersValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of Open Borders with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer,                       true);


	if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
		return 50;

	int iItemValue = 0;


	if(bFromMe)
	{

		switch(eApproach)
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue = 1000;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue = 100;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue = 20;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue = 50;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue = 75;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Open Borders valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue = 100;
			break;
		}


		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
		{
		case MAJOR_CIV_OPINION_ALLY:
			iItemValue = 0;
			break;
		case MAJOR_CIV_OPINION_FRIEND:
			iItemValue *= 35;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_OPINION_FAVORABLE:
			iItemValue *= 70;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_OPINION_NEUTRAL:
			break;
		case MAJOR_CIV_OPINION_COMPETITOR:
			iItemValue *= 150;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_OPINION_ENEMY:
			iItemValue *= 400;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_OPINION_UNFORGIVABLE:
			iItemValue = 10000;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Opinion for Open Borders valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			break;
		}


		int iNumEnemiesAtWarWith = GetPlayer()->GetDiplomacyAI()->GetNumOurEnemiesPlayerAtWarWith(eOtherPlayer);
		if(iNumEnemiesAtWarWith >= 2)
		{
			iItemValue *= 10;
			iItemValue /= 100;
		}
		else if(iNumEnemiesAtWarWith == 1)
		{
			iItemValue *= 25;
			iItemValue /= 100;
		}


		AIGrandStrategyTypes eCultureStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
		if (eCultureStrategy != NO_AIGRANDSTRATEGY && GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(eOtherPlayer) == eCultureStrategy)
		{
			CvPlayer &kOtherPlayer = GET_PLAYER(eOtherPlayer);


			if (kOtherPlayer.GetCulture()->GetTourism() > 0 && kOtherPlayer.GetCulture()->GetInfluenceOn(GetPlayer()->GetID()) < INFLUENCE_LEVEL_INFLUENTIAL)
			{
				iItemValue *= 500;
				iItemValue /= 100;
			}
		}
	}

	else
	{

		switch(GetPlayer()->GetProximityToPlayer(eOtherPlayer))
		{
		case PLAYER_PROXIMITY_DISTANT:
			iItemValue = 5;
			break;
		case PLAYER_PROXIMITY_FAR:
			iItemValue = 10;
			break;
		case PLAYER_PROXIMITY_CLOSE:
			iItemValue = 15;
			break;
		case PLAYER_PROXIMITY_NEIGHBORS:
			iItemValue = 30;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Proximity for Open Borders valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue = 0;
			break;
		}


		if(GET_PLAYER(eOtherPlayer).getNumCities() == 1)
		{
			iItemValue *= 50;
			iItemValue /= 100;
		}



		AIGrandStrategyTypes eCultureStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
#ifdef AUI_WARNING_FIXES
		const CvPlayerCulture* pPlayerCulture = GetPlayer()->GetCulture();
		if (eCultureStrategy != NO_AIGRANDSTRATEGY && GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy() == eCultureStrategy && pPlayerCulture->GetTourism() > 0)
		{

			if (pPlayerCulture->GetCivLowestInfluence(false                      ) == eOtherPlayer)
#else
		if (eCultureStrategy != NO_AIGRANDSTRATEGY && GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy() == eCultureStrategy && GetPlayer()->GetCulture()->GetTourism() > 0 )
		{

			if (GetPlayer()->GetCulture()->GetCivLowestInfluence(false                      ) == eOtherPlayer)
#endif
			{
				iItemValue *= 1000;
				iItemValue /= 100;
			}


#ifdef AUI_WARNING_FIXES
			else if (pPlayerCulture->GetNumCivsToBeInfluentialOn() <= pPlayerCulture->GetNumCivsInfluentialOn())
			{
				if (pPlayerCulture->GetInfluenceLevel(eOtherPlayer) < INFLUENCE_LEVEL_INFLUENTIAL)
#else
			else if (GetPlayer()->GetCulture()->GetNumCivsToBeInfluentialOn() <= GetPlayer()->GetCulture()->GetNumCivsInfluentialOn())
			{
				if (GetPlayer()->GetCulture()->GetInfluenceLevel(eOtherPlayer) < INFLUENCE_LEVEL_INFLUENTIAL)
#endif
				{
					iItemValue *= 500;
					iItemValue /= 100;
				}
			}

			else if (GetPlayer()->GetProximityToPlayer(eOtherPlayer) == PLAYER_PROXIMITY_NEIGHBORS)
			{

				if(GetPlayer()->IsCramped())
				{
					iItemValue *= 300;
					iItemValue /= 100;
				}
			}
		}
	}


	if(bUseEvenValue)
	{
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetOpenBordersValue(!bFromMe, GetPlayer()->GetID(),                   false);

		iItemValue /= 2;
	}

	return iItemValue;
}


int CvDealAI::GetDefensivePactValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Defensive Pact with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue;


	if(!bFromMe)
	{
		iItemValue = 100;





























	}

	else
	{

		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
		{
		case MAJOR_CIV_OPINION_ALLY:
			iItemValue = 100;
			break;
		case MAJOR_CIV_OPINION_FRIEND:
			iItemValue = 100;
			break;
		case MAJOR_CIV_OPINION_FAVORABLE:
			iItemValue = 100;
			break;
		case MAJOR_CIV_OPINION_NEUTRAL:
			iItemValue = 100000;
			break;
		case MAJOR_CIV_OPINION_COMPETITOR:
			iItemValue = 100000;
			break;
		case MAJOR_CIV_OPINION_ENEMY:
			iItemValue = 100000;
			break;
		case MAJOR_CIV_OPINION_UNFORGIVABLE:
			iItemValue = 100000;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Opinion for Defensive Pact valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue = 100000;
			break;
		}

























	}


	if(bUseEvenValue)
	{
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetDefensivePactValue(!bFromMe, GetPlayer()->GetID(),                   false);

		iItemValue /= 2;
	}

	return iItemValue;
}


int CvDealAI::GetResearchAgreementValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Research Agreement with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 100;

	if(bFromMe)
	{

		EraTypes eMyEra = GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra();
		EraTypes eTheirEra = GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetCurrentEra();

		int iAdditionalValue = iItemValue * max(0,(int)(eTheirEra-eMyEra));
		iItemValue += iAdditionalValue;


		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer,                       true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 1000;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue *= 80;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue *= 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Research Agreement valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;

	}


	if(bUseEvenValue)
	{
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetResearchAgreementValue(!bFromMe, GetPlayer()->GetID(),                   false);

		iItemValue /= 2;
	}

	return iItemValue;
}


int CvDealAI::GetTradeAgreementValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Trade Agreement with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 100;

	if(bFromMe)
	{

		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer,                       true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 250;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 130;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue *= 80;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue *= 110;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Research Agreement valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;
	}


	if(bUseEvenValue)
	{
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetTradeAgreementValue(!bFromMe, GetPlayer()->GetID(),                   false);

		iItemValue /= 2;
	}

	return iItemValue;
}


#ifdef CVASSERT_ENABLE
int CvDealAI::GetPeaceTreatyValue(PlayerTypes eOtherPlayer)
{
	DEBUG_VARIABLE(eOtherPlayer);

	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Peace Treaty with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
#else
int CvDealAI::GetPeaceTreatyValue(PlayerTypes)
{
#endif
	return 0;































}


int CvDealAI::GetThirdPartyPeaceValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Third Party Peace with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	CvDiplomacyAI* pDiploAI = GetPlayer()->GetDiplomacyAI();

	PlayerTypes eWithPlayer = NO_PLAYER;


	for (uint ui = 0; ui < MAX_CIV_PLAYERS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;
		if (GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).getTeam() == eWithTeam) 
		{
			eWithPlayer = ePlayer;
			break;
		}
	}

	CvAssertMsg(eWithPlayer != NO_PLAYER, "eWithPlayer could not be found");
	if (eWithPlayer == NO_PLAYER)
	{
		return 0;
	}

	WarProjectionTypes eWarProjection = pDiploAI->GetWarProjection(eWithPlayer);

	EraTypes eOurEra = GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra();

	MajorCivOpinionTypes eOpinionTowardsAskingPlayer = pDiploAI->GetMajorCivOpinion(eOtherPlayer);
	MajorCivOpinionTypes eOpinionTowardsWarPlayer = NO_MAJOR_CIV_OPINION_TYPE;

	bool bMinor = false;


	if(GET_PLAYER(eWithPlayer).isMinorCiv())
	{

		if (GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam()))
		{
			PlayerTypes eMinorAlly = GET_PLAYER(eWithPlayer).GetMinorCivAI()->GetAlly();

			if (eMinorAlly == eOtherPlayer || eMinorAlly == m_pPlayer->GetID())
			{
				return 0;
			}
		}

		bMinor = true;
	}

	else
		eOpinionTowardsWarPlayer = pDiploAI->GetMajorCivOpinion(eWithPlayer);


	if(bFromMe)
	{
		if(eWarProjection == WAR_PROJECTION_VERY_GOOD)
			iItemValue = 600;
		else if(eWarProjection == WAR_PROJECTION_GOOD)
			iItemValue = 400;
		else if(eWarProjection == WAR_PROJECTION_UNKNOWN)
			iItemValue = 250;
		else
			iItemValue = 200;


		int iExtraCost = eOurEra * 50;
		iItemValue += iExtraCost;


		if(bMinor)
		{
		}

		else
		{

			if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_UNFORGIVABLE)
			{
				iItemValue *= 300;
				iItemValue /= 100;
			}
			else if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_ENEMY)
			{
				iItemValue *= 200;
				iItemValue /= 100;
			}
		}


		if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_ALLY)
		{
			iItemValue *= 30;
			iItemValue /= 100;
		}
		else if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_FRIEND)
		{
			iItemValue *= 50;
			iItemValue /= 100;
		}
		else if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_FAVORABLE)
		{
			iItemValue *= 75;
			iItemValue /= 100;
		}
	}

	else
	{
		iItemValue = -10000;
	}

	return iItemValue;
}


int CvDealAI::GetThirdPartyWarValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Third Party War with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	CvDiplomacyAI* pDiploAI = GetPlayer()->GetDiplomacyAI();


	int iWarApproachWeight = pDiploAI->GetPersonalityMajorCivApproachBias(MAJOR_CIV_APPROACH_WAR);
	if(bFromMe && iWarApproachWeight < 4)
		return 100000;


	PlayerTypes eWithPlayer = NO_PLAYER;

	for (uint ui = 0; ui < MAX_CIV_PLAYERS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;
		if (GET_PLAYER(ePlayer).getTeam() == eWithTeam) 
		{
			eWithPlayer = ePlayer;
			break;
		}
	}
	WarProjectionTypes eWarProjection = pDiploAI->GetWarProjection(eWithPlayer);

	EraTypes eOurEra = GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra();

	MajorCivOpinionTypes eOpinionTowardsAskingPlayer = pDiploAI->GetMajorCivOpinion(eOtherPlayer);
	MajorCivOpinionTypes eOpinionTowardsWarPlayer = NO_MAJOR_CIV_OPINION_TYPE;
	MinorCivApproachTypes eMinorApproachTowardsWarPlayer = NO_MINOR_CIV_APPROACH;

	bool bMinor = false;


	if(GET_PLAYER(eWithPlayer).isMinorCiv())
	{
		bMinor = true;
		eMinorApproachTowardsWarPlayer = pDiploAI->GetMinorCivApproach(eWithPlayer);
	}

	else
		eOpinionTowardsWarPlayer = pDiploAI->GetMajorCivOpinion(eWithPlayer);


	if(bFromMe)
	{
		if(eWarProjection >= WAR_PROJECTION_GOOD)
			iItemValue = 400;
		else if(eWarProjection == WAR_PROJECTION_UNKNOWN)
			iItemValue = 600;
		else if(eWarProjection == WAR_PROJECTION_STALEMATE)
			iItemValue = 1000;
		else
			iItemValue = 50000;


		int iExtraCost = eOurEra * 50;
		iItemValue += iExtraCost;


		int iWarBias =       GC.getDEFAULT_FLAVOR_VALUE() - iWarApproachWeight;
		int iWarMod = iWarBias * 10;
		iWarMod *= iItemValue;
		iWarMod /= 100;

		iItemValue += iWarMod;


		if(bMinor)
		{
			if(eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_FRIENDLY)
				iItemValue = 100000;
			else if(eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_PROTECTIVE)
				iItemValue = 100000;
		}

		else
		{

			if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_UNFORGIVABLE)
			{
				iItemValue *= 25;
				iItemValue /= 100;
			}
			else if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_ENEMY)
			{
				iItemValue *= 50;
				iItemValue /= 100;
			}
			else if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_COMPETITOR)
			{
				iItemValue *= 75;
				iItemValue /= 100;
			}
		}


		if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_ALLY)
		{
			iItemValue *= 50;
			iItemValue /= 100;
		}
		else if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_FRIEND)
		{
			iItemValue *= 75;
			iItemValue /= 100;
		}
		else if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_FAVORABLE)
		{
			iItemValue *= 85;
			iItemValue /= 100;
		}
		else if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_COMPETITOR)
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
	}


	else
	{

		if(bMinor)
			iItemValue = -100000;


		else
		{

			if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_UNFORGIVABLE)
				iItemValue = 200;
			else if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_ENEMY)
				iItemValue = 100;
			else
				iItemValue = -100000;
		}
	}

	return iItemValue;
}


int CvDealAI::GetVoteCommitmentValue(bool bFromMe, PlayerTypes eOtherPlayer, int iProposalID, int iVoteChoice, int iNumVotes, bool bRepeal, bool bUseEvenValue)
{
	int iValue = 0;


	if (bFromMe)
	{

		CvLeagueAI::DesireLevels eDesire = GetPlayer()->GetLeagueAI()->EvaluateVoteForTrade(iProposalID, iVoteChoice, iNumVotes, bRepeal);
		switch(eDesire)
		{
		case CvLeagueAI::DESIRE_NEVER:
		case CvLeagueAI::DESIRE_STRONG_DISLIKE:
			iValue += 100000;
			break;
		case CvLeagueAI::DESIRE_DISLIKE:
			iValue += 300;
			break;
		case CvLeagueAI::DESIRE_WEAK_DISLIKE:
			iValue += 200;
			break;
		case CvLeagueAI::DESIRE_NEUTRAL:
		case CvLeagueAI::DESIRE_WEAK_LIKE:
		case CvLeagueAI::DESIRE_LIKE:
			iValue += 150;
			break;
		case CvLeagueAI::DESIRE_STRONG_LIKE:
		case CvLeagueAI::DESIRE_ALWAYS:
			iValue += 50;
			break;
		default:
			iValue += 100000;
			break;
		}


		CvAssert(eOtherPlayer != NO_PLAYER);
		if (eOtherPlayer != NO_PLAYER)
		{
			CvLeagueAI::AlignmentLevels eAlignment = GetPlayer()->GetLeagueAI()->EvaluateAlignment(eOtherPlayer);
			switch (eAlignment)
			{
			case CvLeagueAI::ALIGNMENT_LIBERATOR:
			case CvLeagueAI::ALIGNMENT_LEADER:
				iValue += -50;
				break;
			case CvLeagueAI::ALIGNMENT_SELF:
				CvAssertMsg(false, "ALIGNMENT_SELF found when evaluating a trade deal for delegates. Please send Anton your save file and version.");
				break;
			case CvLeagueAI::ALIGNMENT_ALLY:
				iValue += -35;
				break;
			case CvLeagueAI::ALIGNMENT_CONFIDANT:
			case CvLeagueAI::ALIGNMENT_FRIEND:
				iValue += -25;
				break;
			case CvLeagueAI::ALIGNMENT_NEUTRAL:
				break;
			case CvLeagueAI::ALIGNMENT_RIVAL:
				iValue += 25;
				break;
			case CvLeagueAI::ALIGNMENT_HATRED:
				iValue += 50;
				break;
			case CvLeagueAI::ALIGNMENT_ENEMY:
			case CvLeagueAI::ALIGNMENT_WAR:
				iValue += 100000;
				break;
			default:
				break;
			}

			MajorCivApproachTypes eOtherPlayerApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer,                       false);
			if (eOtherPlayerApproach == MAJOR_CIV_APPROACH_HOSTILE || eOtherPlayerApproach == MAJOR_CIV_APPROACH_WAR)
			{
				iValue += 100000;
			}
		}
	}

	else
	{

		CvLeagueAI::DesireLevels eDesire = GetPlayer()->GetLeagueAI()->EvaluateVoteForTrade(iProposalID, iVoteChoice, iNumVotes, bRepeal);
		switch(eDesire)
		{
		case CvLeagueAI::DESIRE_NEVER:
		case CvLeagueAI::DESIRE_STRONG_DISLIKE:
		case CvLeagueAI::DESIRE_WEAK_DISLIKE:
		case CvLeagueAI::DESIRE_NEUTRAL:
			iValue += -100000;
			break;
		case CvLeagueAI::DESIRE_WEAK_LIKE:
			iValue += 50;
			break;
		case CvLeagueAI::DESIRE_LIKE:
			iValue += 100;
			break;
		case CvLeagueAI::DESIRE_STRONG_LIKE:
			iValue += 150;
			break;
		case CvLeagueAI::DESIRE_ALWAYS:
			iValue += 200;
			break;
		default:
			iValue += -100000;
			break;
		}


#ifndef AUI_LEAGUES_FIX_POSSIBLE_DEALLOCATION_CRASH
		if (GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0)
#endif
		{
			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague)
			{
				float fVotesRatio = (float)iNumVotes / (float)pLeague->CalculateStartingVotesForMember(eOtherPlayer);
				if (fVotesRatio > 0.5f)
				{

				}
				else if (fVotesRatio > 0.25f)
				{

					iValue += -20;
				}
				else
				{

					iValue += -40;
				}
			}
		}
	}

	iValue = MAX(iValue, 0);


	iValue *= iNumVotes;


	if (bUseEvenValue)
	{
		iValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetVoteCommitmentValue(!bFromMe, GetPlayer()->GetID(), iProposalID, iVoteChoice, iNumVotes, bRepeal,                   false);

		iValue /= 2;
	}

	return iValue;
}


void CvDealAI::DoAddVoteCommitmentToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Them, but them is us. Please send Anton your save file and version.");

	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();


			if(!pDeal->IsVoteCommitmentTrade(eThem) && !pDeal->IsVoteCommitmentTrade(eMyPlayer))
			{
				CvLeagueAI::VoteCommitmentList vDesiredCommitments = GetPlayer()->GetLeagueAI()->GetDesiredVoteCommitments(eThem);
				for (CvLeagueAI::VoteCommitmentList::iterator it = vDesiredCommitments.begin(); it != vDesiredCommitments.end(); ++it)
				{
					int iProposalID = it->iResolutionID;
					int iVoteChoice = it->iVoteChoice;
					int iNumVotes = it->iNumVotes;
					bool bRepeal = !it->bEnact;

					if (iProposalID != -1 && pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_VOTE_COMMITMENT, iProposalID, iVoteChoice, iNumVotes, bRepeal))
					{
						int iItemValue = GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT,             false, eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1, bUseEvenValue);


						if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)
						{
							pDeal->AddVoteCommitment(eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal);
							iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
						}
					}
				}
			}
		}
	}
}


void CvDealAI::DoAddVoteCommitmentToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS)
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Us, but them is us. Please send Anton your save file and version.");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();


			if(!pDeal->IsVoteCommitmentTrade(eThem) && !pDeal->IsVoteCommitmentTrade(eMyPlayer))
			{
				CvLeagueAI::VoteCommitmentList vDesiredCommitments = GET_PLAYER(eThem).GetLeagueAI()->GetDesiredVoteCommitments(eMyPlayer);
				for (CvLeagueAI::VoteCommitmentList::iterator it = vDesiredCommitments.begin(); it != vDesiredCommitments.end(); ++it)
				{
					int iProposalID = it->iResolutionID;
					int iVoteChoice = it->iVoteChoice;
					int iNumVotes = it->iNumVotes;
					bool bRepeal = !it->bEnact;

					if (iProposalID != -1 && pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_VOTE_COMMITMENT, iProposalID, iVoteChoice, iNumVotes, bRepeal))
					{
						int iItemValue = GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT,             true, eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1, bUseEvenValue);


						if(-iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
						{
							pDeal->AddVoteCommitment(eMyPlayer, iProposalID, iVoteChoice, iNumVotes, bRepeal);
							iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
						}
					}
				}
			}
		}
	}
}


void CvDealAI::DoAddResourceToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Resource to Them, but them is us.  Please show Jon");

	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			int iItemValue;

#ifdef AUI_WARNING_FIXES
			uint iResourceLoop;
#else
			int iResourceLoop;
#endif
			ResourceTypes eResource;
			int iResourceQuantity;


			for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				eResource = (ResourceTypes) iResourceLoop;

				const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY)
					continue;

				iResourceQuantity = GET_PLAYER(eThem).getNumResourceAvailable(eResource, false);


				if(iResourceQuantity <= 0)
					continue;


				if(GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(eMyPlayer, eResource))
					continue;


				iResourceQuantity = 1;


				if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
				{
					iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES,             false, eThem, eResource, iResourceQuantity, -1,           false, iDealDuration, bUseEvenValue);


					if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)
					{

						if(!pDeal->ChangeResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration))
						{
							pDeal->AddResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration);
							iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
						}
					}
				}
			}


			for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				eResource = (ResourceTypes) iResourceLoop;

				const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
					continue;

				iResourceQuantity = GET_PLAYER(eThem).getNumResourceAvailable(eResource, false);


				if(iResourceQuantity <= 0)
					continue;


				iResourceQuantity = min(5, iResourceQuantity);


				if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
				{
					iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES,             false, eThem, eResource, iResourceQuantity, -1,           false, iDealDuration, bUseEvenValue);


					if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)
					{

						if(!pDeal->ChangeResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration))
						{
							pDeal->AddResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration);
							iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
						}
					}
				}
			}
		}
	}
}


void CvDealAI::DoAddResourceToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Resource to Us, but them is us.  Please show Jon");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			int iItemValue;

			ResourceTypes eResource;
			int iResourceQuantity;
#ifdef AUI_WARNING_FIXES
			for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
			{
				eResource = (ResourceTypes) iResourceLoop;
				iResourceQuantity = GET_PLAYER(eMyPlayer).getNumResourceAvailable(eResource, false);


				if(iResourceQuantity == 0)
				{
					continue;
				}

				const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if (pkResourceInfo == NULL)
					continue;


				if(pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{
					iResourceQuantity = 1;


					if(GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(eThem, eResource))
						continue;
				}
				else
				{
					iResourceQuantity = min(5, iResourceQuantity);
				}


				if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
				{
					iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES,             true, eThem, eResource, iResourceQuantity, -1,           false, iDealDuration, bUseEvenValue);


					if(-iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
					{

						if(!pDeal->ChangeResourceTrade(eMyPlayer, eResource, iResourceQuantity, iDealDuration))
						{
							pDeal->AddResourceTrade(eMyPlayer, eResource, iResourceQuantity, iDealDuration);
							iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
						}
					}
				}
			}
		}
	}
}



void CvDealAI::DoAddEmbassyToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Embassy to Them, but them is us.  Please show Jon");

	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			if(!pDeal->IsAllowEmbassyTrade(eThem))
			{
				PlayerTypes eMyPlayer = GetPlayer()->GetID();


				if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_ALLOW_EMBASSY))
				{
					int iItemValue = GetTradeItemValue(TRADE_ITEM_ALLOW_EMBASSY,             false, eThem, -1, -1, -1,           false, -1, bUseEvenValue);


					if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)
					{
						pDeal->AddAllowEmbassy(eThem);
						iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
					}
				}
			}
		}
	}
}


void CvDealAI::DoAddEmbassyToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Embassy to Us, but them is us.  Please show Jon");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			if(!pDeal->IsAllowEmbassyTrade(eMyPlayer))
			{

				if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_ALLOW_EMBASSY))
				{
					int iItemValue = GetTradeItemValue(TRADE_ITEM_ALLOW_EMBASSY,             true, eThem, -1, -1, -1,           false, -1, bUseEvenValue);


					if(-iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
					{
						pDeal->AddAllowEmbassy(eMyPlayer);
						iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
					}
				}
			}
		}
	}
}


void CvDealAI::DoAddOpenBordersToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Them, but them is us.  Please show Jon");

	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			if(!pDeal->IsOpenBordersTrade(eThem))
			{
				PlayerTypes eMyPlayer = GetPlayer()->GetID();


				if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_OPEN_BORDERS))
				{
					int iItemValue = GetTradeItemValue(TRADE_ITEM_OPEN_BORDERS,             false, eThem, -1, -1, -1,           false, iDealDuration, bUseEvenValue);


					if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)
					{
						pDeal->AddOpenBorders(eThem, iDealDuration);
						iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
					}
				}
			}
		}
	}
}


void CvDealAI::DoAddOpenBordersToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Us, but them is us.  Please show Jon");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			if(!pDeal->IsOpenBordersTrade(eMyPlayer))
			{

				if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_OPEN_BORDERS))
				{
					int iItemValue = GetTradeItemValue(TRADE_ITEM_OPEN_BORDERS,             true, eThem, -1, -1, -1,           false, iDealDuration, bUseEvenValue);


					if(-iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
					{
						pDeal->AddOpenBorders(eMyPlayer, iDealDuration);
						iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
					}
				}
			}
		}
	}
}


void CvDealAI::DoAddCitiesToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Us, but them is us.  Please show Jon");

	PlayerTypes eMyPlayer = GetPlayer()->GetID();


	if(pDeal->GetSurrenderingPlayer() != eMyPlayer)
		return;


	if(bDontChangeMyExistingItems)
		return;


	if(iTotalValue <= 0)
		return;

	CvPlayer* pLosingPlayer = GetPlayer();
	CvPlayer* pWinningPlayer = &GET_PLAYER(eThem);


	if(pLosingPlayer->getNumCities() == 1)
		return;



	int iCityDistanceFromWinnersCapital = 0;
	int iWinnerCapitalX = -1, iWinnerCapitalY = -1;


	CvCity* pWinnerCapital = pWinningPlayer->getCapitalCity();
	if(pWinnerCapital != NULL)
	{
		iWinnerCapitalX = pWinningPlayer->getCapitalCity()->getX();
		iWinnerCapitalY = pWinningPlayer->getCapitalCity()->getY();
	}


	CvWeightedVector<int> viCityProximities;


	CvCity* pLoopCity;
	int iCityLoop;
	for(pLoopCity = pLosingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pLosingPlayer->nextCity(&iCityLoop))
	{





		if(pWinnerCapital != NULL)
		{
			iCityDistanceFromWinnersCapital = plotDistance(iWinnerCapitalX, iWinnerCapitalY, pLoopCity->getX(), pLoopCity->getY());
		}


		if(!pLoopCity->isCapital())
		{
			viCityProximities.push_back(pLoopCity->GetID(), iCityDistanceFromWinnersCapital);
		}
	}


	viCityProximities.SortItems();


	int iSortedCityID;

	for(int iSortedCityIndex = viCityProximities.size() - 1; iSortedCityIndex > -1 ; iSortedCityIndex--)
	{
		iSortedCityID = viCityProximities.GetElement(iSortedCityIndex);
		pLoopCity = pLosingPlayer->getCity(iSortedCityID);




		if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))

		{
			int iItemValue = GetCityValue(pLoopCity->getX(), pLoopCity->getY(),             true, eThem, bUseEvenValue);




			if(-iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
			{

				pDeal->AddCityTrade(eMyPlayer, iSortedCityID);
				iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
			}
		}














	}
}


void CvDealAI::DoAddGoldToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Gold to Them, but them is us.  Please show Jon");

	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();


			if(pDeal->GetGoldTrade(eMyPlayer) == 0)
			{
				int iNumGold = GetGoldForForValueExchange(-iTotalValue,                       true,             false, eThem, bUseEvenValue,              false);
				int iNumGoldAlreadyInTrade = pDeal->GetGoldTrade(eThem);
				iNumGold += iNumGoldAlreadyInTrade;
				iNumGold = min(iNumGold, pDeal->GetGoldAvailable(eThem, TRADE_ITEM_GOLD));


				if(iNumGold != iNumGoldAlreadyInTrade && !pDeal->ChangeGoldTrade(eThem, iNumGold))
				{
					pDeal->AddGoldTrade(eThem, iNumGold);
				}

				iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
			}
		}
	}
}


void CvDealAI::DoAddGoldToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Gold to Us, but them is us.  Please show Jon");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{

			if(pDeal->GetGoldTrade(eThem) == 0)
			{
				PlayerTypes eMyPlayer = GetPlayer()->GetID();

				int iNumGold = GetGoldForForValueExchange(iTotalValue,                       true,             true, eThem, bUseEvenValue,              false);
				int iNumGoldAlreadyInTrade = pDeal->GetGoldTrade(eMyPlayer);
				iNumGold += iNumGoldAlreadyInTrade;
				iNumGold = min(iNumGold, pDeal->GetGoldAvailable(eMyPlayer, TRADE_ITEM_GOLD));


				if(iNumGold != iNumGoldAlreadyInTrade && !pDeal->ChangeGoldTrade(eMyPlayer, iNumGold))
				{
					pDeal->AddGoldTrade(eMyPlayer, iNumGold);
				}

				iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
			}
		}
	}
}


void CvDealAI::DoAddGPTToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add GPT to Them, but them is us.  Please show Jon");

	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			if(GET_PLAYER(eThem).calculateGoldRate() > 0)
			{
				PlayerTypes eMyPlayer = GetPlayer()->GetID();


				if(pDeal->GetGoldPerTurnTrade(eMyPlayer) == 0)
				{
					int iNumGPT = GetGPTforForValueExchange(-iTotalValue,                      true, iDealDuration,             false, eThem, bUseEvenValue,              false);
					int iNumGPTAlreadyInTrade = pDeal->GetGoldPerTurnTrade(eThem);
					iNumGPT += iNumGPTAlreadyInTrade;
					iNumGPT = min(iNumGPT, GET_PLAYER(eThem).calculateGoldRate());

					if(iNumGPT != iNumGPTAlreadyInTrade && !pDeal->ChangeGoldPerTurnTrade(eThem, iNumGPT, iDealDuration))
					{
						pDeal->AddGoldPerTurnTrade(eThem, iNumGPT, iDealDuration);
					}

					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
			}
		}
	}
}


void CvDealAI::DoAddGPTToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add GPT to Us, but them is us.  Please show Jon");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			if(GET_PLAYER(eMyPlayer).calculateGoldRate() > 0)
			{

				if(pDeal->GetGoldPerTurnTrade(eThem) == 0)
				{
					int iNumGPT = GetGPTforForValueExchange(iTotalValue,                      true, iDealDuration,             true, eThem, bUseEvenValue,              false);
					int iNumGPTAlreadyInTrade = pDeal->GetGoldPerTurnTrade(eMyPlayer);
					iNumGPT += iNumGPTAlreadyInTrade;
					iNumGPT = min(iNumGPT, GET_PLAYER(eMyPlayer).calculateGoldRate());

					if(iNumGPT != iNumGPTAlreadyInTrade && !pDeal->ChangeGoldPerTurnTrade(eMyPlayer, iNumGPT, iDealDuration))
					{
						pDeal->AddGoldPerTurnTrade(eMyPlayer, iNumGPT, iDealDuration);
					}

					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
			}
		}
	}
}


void CvDealAI::DoRemoveGPTFromThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to remove GPT from Them, but them is us.  Please show Jon");


	{
		if(iTotalValue > 0)
		{

			int iValueToRemove = iTotalValue * 150;
			iValueToRemove /= 100;

			int iNumGoldPerTurnToRemove = GetGPTforForValueExchange(iValueToRemove,                      true, iDealDuration,             false, eThem, bUseEvenValue,              true);

			int iNumGoldPerTurnInThisDeal = pDeal->GetGoldPerTurnTrade(eThem);
			if(iNumGoldPerTurnInThisDeal > 0)
			{

				iNumGoldPerTurnToRemove = min(iNumGoldPerTurnToRemove, iNumGoldPerTurnInThisDeal);
				iNumGoldPerTurnInThisDeal -= iNumGoldPerTurnToRemove;


				if(iNumGoldPerTurnInThisDeal == 0)
				{
					pDeal->RemoveByType(TRADE_ITEM_GOLD_PER_TURN);
					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}

				else
				{
					if(!pDeal->ChangeGoldPerTurnTrade(eThem, iNumGoldPerTurnInThisDeal, iDealDuration))
					{
						CvAssertMsg(false, "DEAL_AI: DealAI is trying to remove GoldPerTurn from a deal but couldn't find the item for some reason.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
					}

					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
			}
		}
	}
}


void CvDealAI::DoRemoveGPTFromUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to remove GPT from Us, but them is us.  Please show Jon");


	{
		if(iTotalValue < 0)
		{

			int iValueToRemove = -iTotalValue * 150;
			iValueToRemove /= 100;

			int iNumGoldPerTurnToRemove = GetGPTforForValueExchange(iValueToRemove,                      true, iDealDuration,             true, eThem, bUseEvenValue,              true);

			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			int iNumGoldPerTurnInThisDeal = pDeal->GetGoldPerTurnTrade(eMyPlayer);
			if(iNumGoldPerTurnInThisDeal > 0)
			{

				iNumGoldPerTurnToRemove = min(iNumGoldPerTurnToRemove, iNumGoldPerTurnInThisDeal);
				iNumGoldPerTurnInThisDeal -= iNumGoldPerTurnToRemove;


				if(iNumGoldPerTurnInThisDeal == 0)
				{
					pDeal->RemoveByType(TRADE_ITEM_GOLD_PER_TURN);
					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}

				else
				{
					if(!pDeal->ChangeGoldPerTurnTrade(eMyPlayer, iNumGoldPerTurnInThisDeal, iDealDuration))
					{
						CvAssertMsg(false, "DEAL_AI: DealAI is trying to remove GoldPerTurn from a deal but couldn't find the item for some reason.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
					}

					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
			}
		}
	}
}


void CvDealAI::DoRemoveGoldFromThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to remove Gold from Them, but them is us.  Please show Jon");


	{
		if(iTotalValue > 0)
		{
			int iNumGoldInThisDeal = pDeal->GetGoldTrade(eThem);
			if(iNumGoldInThisDeal > 0)
			{

				int iNumGoldToRemove = min(iNumGoldInThisDeal, GetGoldForForValueExchange(iTotalValue,                       true,             false, eThem, bUseEvenValue,              true));
				iNumGoldInThisDeal -= iNumGoldToRemove;


				if(iNumGoldInThisDeal == 0)
				{
					pDeal->RemoveByType(TRADE_ITEM_GOLD);
					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}

				else
				{
					if(!pDeal->ChangeGoldTrade(eThem, iNumGoldInThisDeal))
					{
						CvAssertMsg(false, "DEAL_AI: DealAI is trying to remove Gold from a deal but couldn't find the item for some reason.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
					}

					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
			}
		}
	}
}


void CvDealAI::DoRemoveGoldFromUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to remove Gold from Us, but them is us.  Please show Jon");


	{
		if(iTotalValue < 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			int iNumGoldInThisDeal = pDeal->GetGoldTrade(eMyPlayer);
			if(iNumGoldInThisDeal > 0)
			{

				int iNumGoldToRemove = min(iNumGoldInThisDeal, GetGoldForForValueExchange(-iTotalValue,                       true,             true, eThem, bUseEvenValue,              true));
				iNumGoldInThisDeal -= iNumGoldToRemove;


				if(iNumGoldInThisDeal == 0)
				{
					pDeal->RemoveByType(TRADE_ITEM_GOLD);
					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}

				else
				{
					if(!pDeal->ChangeGoldTrade(eMyPlayer, iNumGoldInThisDeal))
					{
						CvAssertMsg(false, "DEAL_AI: DealAI is trying to remove Gold from a deal but couldn't find the item for some reason.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
					}

					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
			}
		}
	}
}


bool CvDealAI::IsOfferPeace(PlayerTypes eOtherPlayer, CvDeal* pDeal, bool bEqualizingDeals)
{
	bool result = false;
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);



	if(!pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), eOtherPlayer, TRADE_ITEM_PEACE_TREATY))
	{
		return false;
	}
	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_PEACE_TREATY))
	{
		return false;
	}

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	PeaceTreatyTypes ePeaceTreatyImWillingToOffer = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToOffer(eOtherPlayer);
	PeaceTreatyTypes ePeaceTreatyImWillingToAccept = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToAccept(eOtherPlayer);


	if(!GET_PLAYER(eOtherPlayer).isHuman())
	{
		PeaceTreatyTypes ePeaceTreatyTheyreWillingToAccept = GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetTreatyWillingToAccept(eMyPlayer);
		PeaceTreatyTypes ePeaceTreatyTheyreWillingToOffer = GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetTreatyWillingToOffer(eMyPlayer);


		if(ePeaceTreatyImWillingToOffer < ePeaceTreatyTheyreWillingToAccept)
		{
			return false;
		}

		if(ePeaceTreatyTheyreWillingToOffer < ePeaceTreatyImWillingToAccept)
		{
			return false;
		}


		if(ePeaceTreatyImWillingToOffer > PEACE_TREATY_WHITE_PEACE && ePeaceTreatyTheyreWillingToOffer > PEACE_TREATY_WHITE_PEACE)
		{
			int iAmountToReduce = min(ePeaceTreatyImWillingToOffer, ePeaceTreatyTheyreWillingToOffer);

			ePeaceTreatyImWillingToOffer = PeaceTreatyTypes(ePeaceTreatyImWillingToOffer - iAmountToReduce);
			ePeaceTreatyTheyreWillingToOffer = PeaceTreatyTypes(ePeaceTreatyTheyreWillingToOffer - iAmountToReduce);
		}


		if(ePeaceTreatyImWillingToOffer > ePeaceTreatyTheyreWillingToAccept)
		{
			ePeaceTreatyImWillingToOffer = PeaceTreatyTypes((ePeaceTreatyImWillingToOffer + ePeaceTreatyTheyreWillingToAccept) / 2);
		}
		if(ePeaceTreatyTheyreWillingToOffer > ePeaceTreatyImWillingToAccept)
		{
			ePeaceTreatyTheyreWillingToOffer = PeaceTreatyTypes((ePeaceTreatyTheyreWillingToOffer + ePeaceTreatyImWillingToAccept) / 2);
		}

		CvAssertMsg(ePeaceTreatyImWillingToOffer >= PEACE_TREATY_WHITE_PEACE, "DEAL_AI: I'm offering a peace treaty with negative ID.  Please show Jon");
		CvAssertMsg(ePeaceTreatyTheyreWillingToOffer >= PEACE_TREATY_WHITE_PEACE, "DEAL_AI: They're offering a peace treaty with negative ID.  Please show Jon");


		if(ePeaceTreatyImWillingToOffer > ePeaceTreatyTheyreWillingToOffer)
		{
			pDeal->SetSurrenderingPlayer(eMyPlayer);
			pDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToOffer);

			DoAddItemsToDealForPeaceTreaty(eOtherPlayer, pDeal, ePeaceTreatyImWillingToOffer,                     true);
		}

		else if(ePeaceTreatyImWillingToOffer < ePeaceTreatyTheyreWillingToOffer)
		{
			pDeal->SetSurrenderingPlayer(eOtherPlayer);
			pDeal->SetPeaceTreatyType(ePeaceTreatyTheyreWillingToOffer);

			DoAddItemsToDealForPeaceTreaty(eOtherPlayer, pDeal, ePeaceTreatyTheyreWillingToOffer,                     false);
		}


#ifdef AI_PEACE_TURNS
		int iPeaceTreatyLength;
		if (GET_PLAYER(eMyPlayer).isHuman() && GET_PLAYER(eOtherPlayer).isHuman())
		{
			iPeaceTreatyLength = GC.getGame().getGameSpeedInfo().getPeaceDealDuration();
		}
		else
		{
			iPeaceTreatyLength = 5                                                           ;
		}
#else
		int iPeaceTreatyLength = GC.getGame().getGameSpeedInfo().getPeaceDealDuration();
#endif
		pDeal->AddPeaceTreaty(eMyPlayer, iPeaceTreatyLength);
		pDeal->AddPeaceTreaty(eOtherPlayer, iPeaceTreatyLength);

		result = true;
	}


	else
	{

		if(ePeaceTreatyImWillingToOffer > PEACE_TREATY_WHITE_PEACE)
		{
			pDeal->SetSurrenderingPlayer(eMyPlayer);
			pDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToOffer);

			DoAddItemsToDealForPeaceTreaty(eOtherPlayer, pDeal, ePeaceTreatyImWillingToOffer,                     true);


			int iValueImOffering, iValueTheyreOffering;
			GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering,                   false);
			if (!bEqualizingDeals)
			{
				SetCachedValueOfPeaceWithHuman(-iValueImOffering);
			}
		}

		else if(ePeaceTreatyImWillingToAccept > PEACE_TREATY_WHITE_PEACE)
		{
			pDeal->SetSurrenderingPlayer(eOtherPlayer);
			pDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToAccept);

			DoAddItemsToDealForPeaceTreaty(eOtherPlayer, pDeal, ePeaceTreatyImWillingToAccept,                     false);


			int iValueImOffering, iValueTheyreOffering;
			GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering,                   false);
			if (!bEqualizingDeals)
			{
				SetCachedValueOfPeaceWithHuman(iValueTheyreOffering);
			}
		}
		else
		{

			DoAddPlayersAlliesToTreaty(eOtherPlayer, pDeal);
		}

#ifdef AI_PEACE_TURNS
		int iPeaceTreatyLength;
		if (GET_PLAYER(eMyPlayer).isHuman() && GET_PLAYER(eOtherPlayer).isHuman())
		{
			iPeaceTreatyLength = GC.getGame().getGameSpeedInfo().getPeaceDealDuration();
		}
		else
		{
			iPeaceTreatyLength = 5                                                           ;
		}
#else
		int iPeaceTreatyLength = GC.getGame().getGameSpeedInfo().getPeaceDealDuration();
#endif
		pDeal->AddPeaceTreaty(eMyPlayer, iPeaceTreatyLength);
		pDeal->AddPeaceTreaty(eOtherPlayer, iPeaceTreatyLength);

		result = true;
	}

	return result;
}


void CvDealAI::DoAddItemsToDealForPeaceTreaty(PlayerTypes eOtherPlayer, CvDeal* pDeal, PeaceTreatyTypes eTreaty, bool bMeSurrendering)
{
	int iPercentGoldToGive = 0;
	int iPercentGPTToGive = 0;
	bool bGiveOpenBorders = false;
	bool bGiveOnlyOneCity = false;
	int iPercentCitiesGiveUp = 0;
	bool bGiveUpStratResources = false;
	bool bGiveUpLuxuryResources = false;


	switch (eTreaty)
	{
	case PEACE_TREATY_WHITE_PEACE:

		break;

	case PEACE_TREATY_ARMISTICE:
		iPercentGoldToGive = 50;
		iPercentGPTToGive = 50;
		break;

	case PEACE_TREATY_SETTLEMENT:
		iPercentGoldToGive = 100;
		iPercentGPTToGive = 100;
		break;

	case PEACE_TREATY_BACKDOWN:
		iPercentGoldToGive = 100;
		iPercentGPTToGive = 100;
		bGiveOpenBorders = true;
		bGiveUpStratResources = true;
		break;

	case PEACE_TREATY_SUBMISSION:
		iPercentGoldToGive = 100;
		iPercentGPTToGive = 100;
		bGiveOpenBorders = true;
		bGiveUpStratResources = true;
		bGiveUpLuxuryResources = true;
		break;

	case PEACE_TREATY_SURRENDER:
		bGiveOnlyOneCity = true;
		break;

	case PEACE_TREATY_CESSION:
		iPercentCitiesGiveUp = 25;
		iPercentGoldToGive = 50;
		break;

	case PEACE_TREATY_CAPITULATION:
		iPercentCitiesGiveUp = 33;
		iPercentGoldToGive = 100;
		break;

	case PEACE_TREATY_UNCONDITIONAL_SURRENDER:
		iPercentCitiesGiveUp = 100;
		iPercentGoldToGive = 100;
		break;
	}

	int iDuration = GC.getGame().GetDealDuration();

	PlayerTypes eLosingPlayer = bMeSurrendering ? GetPlayer()->GetID() : eOtherPlayer;
	CvPlayer* pLosingPlayer = &GET_PLAYER(eLosingPlayer);
	PlayerTypes eWinningPlayer = bMeSurrendering ? eOtherPlayer : GetPlayer()->GetID();
	CvPlayer* pWinningPlayer = &GET_PLAYER(eWinningPlayer);

	DoAddPlayersAlliesToTreaty(eOtherPlayer, pDeal);

	CvCity* pLoopCity;
	int iCityLoop;


	int iGold = 0;
	if (iPercentGoldToGive > 0)
	{
		iGold = pDeal->GetGoldAvailable(eLosingPlayer, TRADE_ITEM_GOLD);
		if(iGold > 0)
		{
			iGold = iGold * iPercentGoldToGive / 100;

			if(pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_GOLD, iGold))
			{
				pDeal->AddGoldTrade(eLosingPlayer, iGold);
			}
		}
	}


	int iGPT = 0;
	if (iPercentGPTToGive > 0)
	{
		iGPT = min(pLosingPlayer->calculateGoldRate(), pWinningPlayer->calculateGoldRate() /       GC.getARMISTICE_GPT_DIVISOR());
		if (iGPT > 0)
		{
			iGPT = iGPT * iPercentGPTToGive / 100;

			if(iGPT > 0 && pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_GOLD_PER_TURN, iGPT, iDuration))
			{
				pDeal->AddGoldPerTurnTrade(eLosingPlayer, iGPT, iDuration);
			}
		}
	}


	if (bGiveOpenBorders)
	{
		if(pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_OPEN_BORDERS))
		{
			pDeal->AddOpenBorders(eLosingPlayer, iDuration);
		}
	}


	ResourceUsageTypes eUsage;
	ResourceTypes eResource;
	int iResourceQuantity;
#ifdef AUI_WARNING_FIXES
	for (uint iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#else
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
#endif
	{
		eResource = (ResourceTypes) iResourceLoop;

		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if (pkResourceInfo == NULL)
			continue;

		eUsage = pkResourceInfo->getResourceUsage();


		if(eUsage == RESOURCEUSAGE_BONUS)
		{
			continue;
		}

		iResourceQuantity = pLosingPlayer->getNumResourceAvailable(eResource, false);


		if (iResourceQuantity == 0)
		{
			continue;
		}


		if (eUsage == RESOURCEUSAGE_LUXURY && !bGiveUpLuxuryResources)
		{
			continue;
		}

		if (eUsage == RESOURCEUSAGE_STRATEGIC && !bGiveUpStratResources)
		{
			continue;
		}


		if (eUsage == RESOURCEUSAGE_LUXURY)
		{
			iResourceQuantity = 1;
		}

		if(pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
		{
			pDeal->AddResourceTrade(eLosingPlayer, eResource, iResourceQuantity, iDuration);
		}
	}


	if (iPercentCitiesGiveUp == 100)
	{

		for(pLoopCity = pLosingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pLosingPlayer->nextCity(&iCityLoop))
		{
#ifndef AUI_DEAL_ALLOW_CAPITOL_GIFTING
			if(pLoopCity->isCapital())
			{
				continue;
			}
#endif

			if(pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))
			{
				pDeal->AddCityTrade(eLosingPlayer, pLoopCity->GetID());
			}
		}
	}


#ifdef AUI_WARNING_FIXES
	else if (iPercentCitiesGiveUp > 0 || (bGiveOnlyOneCity && pLosingPlayer->getNumCities() > 1))
#else
	else if (iPercentCitiesGiveUp > 0 || bGiveOnlyOneCity && pLosingPlayer->getNumCities() > 1)
#endif
	{
		int iCityValue = 0;
		int iCityDistanceFromWinnersCapital = 0;
		int iWinnerCapitalX = -1, iWinnerCapitalY = -1;


		CvCity* pWinnerCapital = pWinningPlayer->getCapitalCity();
		if(pWinnerCapital != NULL)
		{
			iWinnerCapitalX = pWinnerCapital->getX();
			iWinnerCapitalY = pWinnerCapital->getY();
		}


		CvWeightedVector<int> viCityProximities;


		for(pLoopCity = pLosingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pLosingPlayer->nextCity(&iCityLoop))
		{

			iCityValue += GetCityValue(pLoopCity->getX(), pLoopCity->getY(), bMeSurrendering, eOtherPlayer,                   true);


			if(pWinnerCapital != NULL)
			{
				iCityDistanceFromWinnersCapital = plotDistance(iWinnerCapitalX, iWinnerCapitalY, pLoopCity->getX(), pLoopCity->getY());
			}


			if (pLoopCity->getOriginalOwner() == eWinningPlayer)
			{
				iCityDistanceFromWinnersCapital /= 3;
			}


			if(!pLoopCity->isCapital())
			{
				viCityProximities.push_back(pLoopCity->GetID(), iCityDistanceFromWinnersCapital);
			}
		}


		viCityProximities.SortItems();
		int iSortedCityID;


		if (bGiveOnlyOneCity)
		{
			iSortedCityID = viCityProximities.GetElement(viCityProximities.size() - 1);
			pDeal->AddCityTrade(eLosingPlayer, iSortedCityID);
		}

		else
		{

			int iCityValueToSurrender = iCityValue * iPercentCitiesGiveUp / 100;


			for(int iSortedCityIndex = viCityProximities.size() - 1; iSortedCityIndex > -1 ; iSortedCityIndex--)
			{
				iSortedCityID = viCityProximities.GetElement(iSortedCityIndex);
				pLoopCity = pLosingPlayer->getCity(iSortedCityID);

				iCityValue = GetCityValue(pLoopCity->getX(), pLoopCity->getY(), bMeSurrendering, eOtherPlayer,                   true);


				if(iCityValue < iCityValueToSurrender)
				{
					if(pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))
					{
						pDeal->AddCityTrade(eLosingPlayer, iSortedCityID);
						iCityValueToSurrender -= iCityValue;
					}
				}
			}
		}
	}
}


int CvDealAI::GetCachedValueOfPeaceWithHuman()
{
	return m_iCachedValueOfPeaceWithHuman;
}


void CvDealAI::SetCachedValueOfPeaceWithHuman(int iValue)
{
	m_iCachedValueOfPeaceWithHuman = iValue;
}


void CvDealAI::DoAddPlayersAlliesToTreaty(PlayerTypes eToPlayer, CvDeal* pDeal)
{
	int iPeaceDuration = GC.getGame().getGameSpeedInfo().getPeaceDealDuration();
	PlayerTypes eMinor;
	CvPlayer* pMinor;
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		eMinor = (PlayerTypes) iMinorLoop;
		pMinor = &GET_PLAYER(eMinor);


		if(!pMinor->isAlive())
			continue;

		PlayerTypes eAlly = pMinor->GetMinorCivAI()->GetAlly();

		if (eAlly == eToPlayer)
		{

			if (!GET_TEAM(GetTeam()).isAtWar(pMinor->getTeam()))
			{
				continue;
			}


			if (pMinor->GetMinorCivAI()->IsPermanentWar(GetTeam()))
			{
				continue;
			}



			if(!pDeal->IsThirdPartyPeaceTrade(GetPlayer()->GetID(), pMinor->getTeam()) && pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), eToPlayer, TRADE_ITEM_THIRD_PARTY_PEACE, pMinor->getTeam()))
			{
				pDeal->AddThirdPartyPeace(GetPlayer()->GetID(), pMinor->getTeam(), iPeaceDuration);
			}
		}

		else if (eAlly == GetPlayer()->GetID())
		{

			if (!GET_TEAM(GET_PLAYER(eToPlayer).getTeam()).isAtWar(pMinor->getTeam()))
			{
				continue;
			}


			if (pMinor->GetMinorCivAI()->IsPermanentWar(GET_PLAYER(eToPlayer).getTeam()))
			{
				continue;
			}



			if(!pDeal->IsThirdPartyPeaceTrade(eToPlayer, pMinor->getTeam()) && pDeal->IsPossibleToTradeItem(eToPlayer, GetPlayer()->GetID(), TRADE_ITEM_THIRD_PARTY_PEACE, pMinor->getTeam()))
			{
				pDeal->AddThirdPartyPeace(eToPlayer, pMinor->getTeam(), iPeaceDuration);
			}
		}
	}
}


bool CvDealAI::IsMakeDemand(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);


	pDeal->SetDemandingPlayer(GetPlayer()->GetID());

	int iGold = pDeal->GetGoldAvailable(eOtherPlayer, TRADE_ITEM_GOLD);


	int iMaxGold = 200 + (GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetCurrentEra() * 150);
	iGold = min(iMaxGold, iGold);

	if(pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_GOLD, iGold))
	{
		pDeal->AddGoldTrade(eOtherPlayer, iGold);

		return true;
	}

	return false;
}


bool CvDealAI::IsMakeOfferForLuxuryResource(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	ResourceTypes eLuxuryFromThem = NO_RESOURCE;


	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer,                       false);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE ||
	        eApproach == MAJOR_CIV_APPROACH_WAR)
	{
		return false;
	}

#ifdef AUI_WARNING_FIXES
	uint iResourceLoop;
#else
	int iResourceLoop;
#endif
	ResourceTypes eResource;


	for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;


		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY)
		{
			continue;
		}


		if (GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(GetPlayer()->GetID(), eResource))
		{
			continue;
		}


		if(GET_PLAYER(eOtherPlayer).getNumResourceAvailable(eResource, false) > 1)
		{
			eLuxuryFromThem = eResource;
			break;
		}
	}


	if(eLuxuryFromThem != NO_RESOURCE)
	{

		if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_RESOURCES, eLuxuryFromThem, 1))
		{
			return false;
		}


		pDeal->AddResourceTrade(eOtherPlayer, eLuxuryFromThem, 1, GC.getGame().GetDealDuration());

		bool bDealAcceptable = false;


		if(!GET_PLAYER(eOtherPlayer).isHuman())
		{
			bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);
		}
		else
		{
			bool bUselessReferenceVariable;
			bool bCantMatchOffer;
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer,                                false,                                   true, bUselessReferenceVariable, bCantMatchOffer);
		}

		return bDealAcceptable;
	}

	return false;
}


bool CvDealAI::MakeOfferForEmbassy(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);


	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer,                       false);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE ||
	        eApproach == MAJOR_CIV_APPROACH_WAR		||
	        eApproach == MAJOR_CIV_APPROACH_GUARDED)
	{
		return false;
	}


	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_ALLOW_EMBASSY))
	{
		return false;
	}


	if(GetPlayer()->GetDiplomacyAI()->WantsEmbassyAtPlayer(eOtherPlayer))
	{

		pDeal->AddAllowEmbassy(eOtherPlayer);
		bool bDealAcceptable = false;


		if(!GET_PLAYER(eOtherPlayer).isHuman())
		{
			bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);
		}
		else
		{
			bool bUselessReferenceVariable;
			bool bCantMatchOffer;
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, false, true, bUselessReferenceVariable, bCantMatchOffer);
		}

		return bDealAcceptable;
	}

	return false;
}


bool CvDealAI::IsMakeOfferForOpenBorders(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);


	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer,                       false);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE ||
	        eApproach == MAJOR_CIV_APPROACH_WAR)
	{
		return false;
	}


	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_OPEN_BORDERS))
	{
		return false;
	}


	if(GetPlayer()->GetDiplomacyAI()->IsWantsOpenBordersWithPlayer(eOtherPlayer))
	{

		pDeal->AddOpenBorders(eOtherPlayer, GC.getGame().GetDealDuration());

		bool bDealAcceptable = false;


		if(!GET_PLAYER(eOtherPlayer).isHuman())
		{
			bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);
		}
		else
		{
			bool bUselessReferenceVariable;
			bool bCantMatchOffer;
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, false, true, bUselessReferenceVariable, bCantMatchOffer);
		}

		return bDealAcceptable;
	}

	return false;
}


bool CvDealAI::IsMakeOfferForResearchAgreement(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);




	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_RESEARCH_AGREEMENT))
	{
		return false;
	}


	pDeal->AddResearchAgreement(GetPlayer()->GetID(), GC.getGame().GetDealDuration());
	pDeal->AddResearchAgreement(eOtherPlayer, GC.getGame().GetDealDuration());

	bool bDealAcceptable = false;


	if(!GET_PLAYER(eOtherPlayer).isHuman())
	{
		bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);
	}
	else
	{
		bool bUselessReferenceVariable;
		bool bCantMatchOffer;
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, true, false, bUselessReferenceVariable, bCantMatchOffer);
	}

	return bDealAcceptable;
}


void CvDealAI::DoTradeScreenOpened()
{
	TeamTypes eActiveTeam = GC.getGame().getActiveTeam();
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();

	if(GET_TEAM(GetTeam()).isAtWar(eActiveTeam))
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();

		PeaceTreatyTypes ePeaceTreatyImWillingToOffer = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToOffer(eActivePlayer);
		PeaceTreatyTypes ePeaceTreatyImWillingToAccept = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToAccept(eActivePlayer);


		if(ePeaceTreatyImWillingToOffer >= PEACE_TREATY_WHITE_PEACE && ePeaceTreatyImWillingToAccept >= PEACE_TREATY_WHITE_PEACE)
		{

			auto_ptr<ICvDeal1> pUIDeal(GC.GetEngineUserInterface()->GetScratchDeal());
			CvDeal* pkUIDeal = GC.UnwrapDealPointer(pUIDeal.get());
			pkUIDeal->ClearItems();

			CvDeal* pDeal = GC.getGame().GetGameDeals()->GetTempDeal();
			pDeal->ClearItems();
			pDeal->SetFromPlayer(eActivePlayer);
			pDeal->SetToPlayer(eMyPlayer);


			if(ePeaceTreatyImWillingToOffer > PEACE_TREATY_WHITE_PEACE)
			{
				pkUIDeal->SetSurrenderingPlayer(eMyPlayer);
				pkUIDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToOffer);

				DoAddItemsToDealForPeaceTreaty(eActivePlayer, pDeal, ePeaceTreatyImWillingToOffer,                     true);


				int iValueImOffering, iValueTheyreOffering;
				GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering,                   false);
				SetCachedValueOfPeaceWithHuman(-iValueImOffering);
			}

			else if(ePeaceTreatyImWillingToAccept > PEACE_TREATY_WHITE_PEACE)
			{
				pkUIDeal->SetSurrenderingPlayer(eActivePlayer);
				pkUIDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToAccept);

				DoAddItemsToDealForPeaceTreaty(eActivePlayer, pDeal, ePeaceTreatyImWillingToAccept,                     false);


				int iValueImOffering, iValueTheyreOffering;
				GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering,                   false);
				SetCachedValueOfPeaceWithHuman(iValueTheyreOffering);
			}

			pDeal->ClearItems();


			pkUIDeal->SetFromPlayer(eActivePlayer);
			pkUIDeal->SetToPlayer(eMyPlayer);
#ifdef AI_PEACE_TURNS
			int iPeaceTreatyLength;
			if (GET_PLAYER(eMyPlayer).isHuman() && GET_PLAYER(eActivePlayer).isHuman())
			{
				iPeaceTreatyLength = GC.getGame().getGameSpeedInfo().getPeaceDealDuration();
			}
			else
			{
				iPeaceTreatyLength = 5                                                           ;
			}
			pkUIDeal->AddPeaceTreaty(eMyPlayer, iPeaceTreatyLength);
			pkUIDeal->AddPeaceTreaty(eActivePlayer, iPeaceTreatyLength);
#else
			pkUIDeal->AddPeaceTreaty(eMyPlayer, GC.getGame().getGameSpeedInfo().getPeaceDealDuration());
			pkUIDeal->AddPeaceTreaty(eActivePlayer, GC.getGame().getGameSpeedInfo().getPeaceDealDuration());
#endif
			

			DoAddPlayersAlliesToTreaty(eActivePlayer, pkUIDeal);


			pkUIDeal->SetPeaceTreatyType(PEACE_TREATY_WHITE_PEACE);
		}
	}
}


void CvDealAI::DoTradeScreenClosed(bool bAIWasMakingOffer)
{
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();


	SetCachedValueOfPeaceWithHuman(0);

	GC.GetEngineUserInterface()->SetAIRequestingConcessions(false);
	GC.GetEngineUserInterface()->SetHumanMakingDemand(false);

#ifdef AUI_WARNING_FIXES
	CvDiplomacyAI* pDiplomacyAI = GetPlayer()->GetDiplomacyAI();
	pDiplomacyAI->ClearDealToRenew();

	if (bAIWasMakingOffer)
	{


		if (pDiplomacyAI->IsWantsResearchAgreementWithPlayer(eActivePlayer))
		{
			if (pDiplomacyAI->IsCanMakeResearchAgreementRightNow(eActivePlayer))
			{
				pDiplomacyAI->DoCancelWantsResearchAgreementWithPlayer(eActivePlayer);
#else
	GetPlayer()->GetDiplomacyAI()->ClearDealToRenew();

	if(bAIWasMakingOffer)
	{


		if(GetPlayer()->GetDiplomacyAI()->IsWantsResearchAgreementWithPlayer(eActivePlayer))
		{
			if(GetPlayer()->GetDiplomacyAI()->IsCanMakeResearchAgreementRightNow(eActivePlayer))
			{
				GetPlayer()->GetDiplomacyAI()->DoCancelWantsResearchAgreementWithPlayer(eActivePlayer);
#endif
			}
		}
	}
}
