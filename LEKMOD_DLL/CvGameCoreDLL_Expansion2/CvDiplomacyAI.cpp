







#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvDiplomacyAI.h"
#include "CvGrandStrategyAI.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvCitySpecializationAI.h"
#include "CvDealAI.h"
#include "CvGameCoreUtils.h"
#include "CvNotifications.h"
#include "CvDiplomacyRequests.h"


#include "LintFree.h"

#ifdef _MSC_VER
#	pragma warning ( disable : 4351 )
#endif

CvDiplomacyAI::DiplomacyAIData::DiplomacyAIData() :
	m_aDiploLogStatementTurnCountScratchPad()
	, m_aiMajorCivOpinion()
	, m_aeMajorCivApproach()
	, m_aeApproachScratchPad()
	, m_aeMinorCivApproach()
	, m_aeApproachTowardsUsGuess()
	, m_aeApproachTowardsUsGuessCounter()
	, m_aeWantPeaceCounter()
	, m_aePeaceTreatyWillingToOffer()
	, m_aePeaceTreatyWillingToAccept()
	, m_aiNumWondersBeatenTo()
	, m_abMusteringForAttack()
	, m_abWantsResearchAgreementWithPlayer()
	, m_abWantToRouteToMinor()
	, m_aeWarFace()
	, m_aeWarState()
	, m_aeWarProjection()
	, m_aeWarGoal()
	, m_aiPlayerNumTurnsAtWar()
	, m_aiNumWarsFought()
	, m_aePlayerMilitaryStrengthComparedToUs()
	, m_aePlayerEconomicStrengthComparedToUs()
	, m_aePlayerTargetValue()
	, m_aePlayerLandDisputeLevel()
	, m_aePlayerLastTurnLandDisputeLevel()
	, m_aePlayerVictoryDisputeLevel()
	, m_aePlayerWonderDisputeLevel()
	, m_aePlayerMinorCivDisputeLevel()
	, m_aeMilitaryAggressivePosture()
	, m_aeLastTurnMilitaryAggressivePosture()
	, m_aeExpansionAggressivePosture()
	, m_aePlotBuyingAggressivePosture()
	, m_aeMilitaryThreat()
	, m_aeWarDamageLevel()
	, m_aiWarValueLost()
	, m_aeWarmongerThreat()
	, m_aiPersonalityMajorCivApproachBiases()
	, m_aiPersonalityMinorCivApproachBiases()
	, m_aDeclarationsLog()
	, m_abPlayerNoSettleRequest()
	, m_aiPlayerNoSettleRequestCounter()
	, m_abPlayerStopSpyingRequest()
	, m_aiPlayerStopSpyingRequestCounter()
	, m_aiDemandCounter()
	, m_aiDemandTooSoonNumTurns()
	, m_abDoFAccepted()
	, m_aiDoFCounter()
	, m_abDenouncedPlayer()
	, m_abFriendDenouncedUs()
	, m_abFriendDeclaredWarOnUs()
	, m_aiDenouncedPlayerCounter()
	, m_aiNumRequestsRefused()
	, m_aiNumCiviliansReturnedToMe()
	, m_aiNumLandmarksBuiltForMe()
	, m_aiResurrectedOnTurn()
	, m_aiNumTimesCultureBombed()
	, m_paiNegativeReligiousConversionPoints()
	, m_paiNegativeArchaeologyPoints()
	, m_aiNumTimesNuked()
	, m_aiNumTimesRobbedBy()
	, m_aiNumTimesIntrigueSharedBy()
	, m_abPlayerMadeMilitaryPromise()
	, m_abPlayerBrokenMilitaryPromise()
	, m_abPlayerIgnoredMilitaryPromise()
	, m_aiPlayerMilitaryPromiseCounter()
	, m_aiPlayerMadeExpansionPromiseTurn()
	, m_abPlayerBrokenExpansionPromise()
	, m_abPlayerIgnoredExpansionPromise()
	, m_aePlayerExpansionPromiseData()
	, m_aiPlayerMadeBorderPromiseTurn()
	, m_abPlayerBrokenBorderPromise()
	, m_abPlayerIgnoredBorderPromise()
	, m_aePlayerBorderPromiseData()
	, m_abPlayerMadeAttackCityStatePromise()
	, m_abPlayerBrokenAttackCityStatePromise()
	, m_abPlayerIgnoredAttackCityStatePromise()
	, m_abPlayerMadeBullyCityStatePromise()
	, m_abPlayerBrokenBullyCityStatePromise()
	, m_abPlayerIgnoredBullyCityStatePromise()
	, m_abPlayerMadeSpyPromise()
	, m_abPlayerBrokenSpyPromise()
	, m_abPlayerIgnoredSpyPromise()
	, m_abPlayerForgaveForSpying()
	, m_abPlayerMadeNoConvertPromise()
	, m_abPlayerBrokenNoConvertPromise()
	, m_abPlayerIgnoredNoConvertPromise()
	, m_abPlayerAskedNotToConvert()
	, m_abPlayerAgreedNotToConvert()
	, m_abPlayerMadeNoDiggingPromise()
	, m_abPlayerBrokenNoDiggingPromise()
	, m_abPlayerIgnoredNoDiggingPromise()
	, m_abPlayerAskedNotToDig()
	, m_abPlayerAgreedNotToDig()
	, m_abPlayerBrokenCoopWarPromise()
	, m_aiOtherPlayerTurnsSinceAttackedProtectedMinor()
	, m_aiOtherPlayerProtectedMinorAttacked()
	, m_aiOtherPlayerNumProtectedMinorsAttacked()
	, m_aiOtherPlayerTurnsSinceKilledProtectedMinor()
	, m_aiOtherPlayerProtectedMinorKilled()
	, m_aiOtherPlayerNumProtectedMinorsKilled()
	, m_aiOtherPlayerNumProtectedMinorsBullied()
	, m_aiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor()
	, m_aiOtherPlayerNumMinorsAttacked()
	, m_aiOtherPlayerNumMinorsConquered()
	, m_aiOtherPlayerNumMajorsAttacked()
	, m_aiOtherPlayerNumMajorsConquered()
	, m_apaeOtherPlayerMajorCivOpinion()
	, m_apaeOtherPlayerLandDisputeLevel()
	, m_apaeOtherPlayerVictoryDisputeLevel()
	, m_apaeOtherPlayerWonderDisputeLevel()
	, m_apaeOtherPlayerMinorCivDisputeLevel()
	, m_apaeOtherPlayerWarDamageLevel()
	, m_apaiOtherPlayerWarValueLost()
	, m_apaiOtherPlayerLastRoundWarValueLost()
	, m_apabSentAttackMessageToMinorCivProtector()
	, m_apaeOtherPlayerMilitaryThreat()
	, m_apaDiploStatementsLog()
	, m_apabWorkingAgainstPlayerAccepted()
	, m_apaiWorkingAgainstPlayerCounter()
	, m_apacCoopWarAcceptedState()
	, m_apaiCoopWarCounter()
	, m_aaeOtherPlayerMajorCivOpinion()
	, m_aaeOtherPlayerLandDisputeLevel()
	, m_aaeOtherPlayerVictoryDisputeLevel()
	, m_aaeOtherPlayerWarDamageLevel()
	, m_aaiOtherPlayerWarValueLost()
	, m_aaiOtherPlayerLastRoundWarValueLost()
	, m_aabSentAttackMessageToMinorCivProtector()
	, m_aaeOtherPlayerMilitaryThreat()
	, m_aaDiploStatementsLog()
	, m_aabWorkingAgainstPlayerAccepted()
	, m_aaiWorkingAgainstPlayerCounter()
	, m_aacCoopWarAcceptedState()
	, m_aaiCoopWarCounter()
{
}





CvDiplomacyAI::CvDiplomacyAI():
#ifdef AUI_WARNING_FIXES
	m_pPlayer(NULL),
	m_eDemandTargetPlayer(0),
	m_bDemandReady(false),
	m_iVictoryCompetitiveness(0),
	m_iWonderCompetitiveness(0),
	m_iMinorCivCompetitiveness(0),
	m_iBoldness(0),
	m_iDiploBalance(0),
	m_iWarmongerHate(0),
	m_iDenounceWillingness(0),
	m_iDoFWillingness(0),
	m_iLoyalty(0),
	m_iNeediness(0),
	m_iForgiveness(0),
	m_iChattiness(0),
	m_iMeanness(0),
#endif
	m_pDiploData(NULL),
	m_paDiploLogStatementTurnCountScratchPad(NULL),
	m_paeMajorCivOpinion(NULL),
	m_ppaaeOtherPlayerMajorCivOpinion(NULL),
	m_paeMajorCivApproach(NULL),
	m_paeApproachScratchPad(NULL),
	m_paeMinorCivApproach(NULL),
	m_paeApproachTowardsUsGuess(NULL),
	m_paeApproachTowardsUsGuessCounter(NULL),
	m_paeWantPeaceCounter(NULL),
	m_paePeaceTreatyWillingToOffer(NULL),
	m_paePeaceTreatyWillingToAccept(NULL),
	m_paiNumWondersBeatenTo(NULL),
	m_pabMusteringForAttack(NULL),
	m_pabWantsResearchAgreementWithPlayer(NULL),
	m_pabWantToRouteToMinor(NULL),
	m_paeWarFace(NULL),
	m_paeWarState(NULL),
	m_paeWarProjection(NULL),
	m_paeLastWarProjection(NULL),
	m_paeWarGoal(NULL),
	m_paiPlayerNumTurnsAtWar(NULL),
	m_paiNumWarsFought(NULL),
	m_paePlayerMilitaryStrengthComparedToUs(NULL),
	m_paePlayerEconomicStrengthComparedToUs(NULL),
	m_paePlayerTargetValue(NULL),
	m_paePlayerLandDisputeLevel(NULL),
	m_paePlayerLastTurnLandDisputeLevel(NULL),
	m_paePlayerVictoryDisputeLevel(NULL),
	m_paePlayerWonderDisputeLevel(NULL),
	m_paePlayerMinorCivDisputeLevel(NULL),
	m_ppaaeOtherPlayerLandDisputeLevel(NULL),
	m_ppaaeOtherPlayerVictoryDisputeLevel(NULL),

	m_paeWarDamageLevel(NULL),
	m_paiWarValueLost(NULL),
	m_ppaaeOtherPlayerWarDamageLevel(NULL),
	m_ppaaiOtherPlayerWarValueLost(NULL),
	m_ppaaiOtherPlayerLastRoundWarValueLost(NULL),
	m_ppaabSentAttackMessageToMinorCivProtector(NULL),

	m_paeMilitaryAggressivePosture(NULL),
	m_paeLastTurnMilitaryAggressivePosture(NULL),
	m_paeExpansionAggressivePosture(NULL),
	m_paePlotBuyingAggressivePosture(NULL),

	m_paeMilitaryThreat(NULL),
	m_ppaaeOtherPlayerMilitaryThreat(NULL),

	m_paeWarmongerThreat(NULL),

	m_paiPersonalityMajorCivApproachBiases(NULL),
	m_paiPersonalityMinorCivApproachBiases(NULL),

	m_pabPlayerNoSettleRequestAccepted(NULL),
	m_paiPlayerNoSettleRequestCounter(NULL),

	m_pabPlayerStopSpyingRequestAccepted(NULL),
	m_paiPlayerStopSpyingRequestCounter(NULL),

	m_paiDemandCounter(NULL),
	m_paiDemandTooSoonNumTurns(NULL),

	m_pabDoFAccepted(NULL),
	m_paiDoFCounter(NULL),

	m_pabDenouncedPlayer(NULL),
	m_pabFriendDenouncedUs(NULL),
	m_pabFriendDeclaredWarOnUs(NULL),
	m_paiDenouncedPlayerCounter(NULL),

	m_paiNumRequestsRefused(NULL),

	m_paiNumCiviliansReturnedToMe(NULL),
	m_paiNumLandmarksBuiltForMe(NULL),
	m_paiResurrectedOnTurn(NULL),
	m_paiNumTimesCultureBombed(NULL),

	m_paiNegativeReligiousConversionPoints(NULL),

	m_paiNegativeArchaeologyPoints(NULL),

	m_paiNumTimesNuked(NULL),
	m_paiNumTimesRobbedBy(NULL),
	m_paiNumTimesIntrigueSharedBy(NULL),

	m_paiBrokenExpansionPromiseValue(NULL),
	m_paiIgnoredExpansionPromiseValue(NULL),
	m_paiBrokenBorderPromiseValue(NULL),
	m_paiIgnoredBorderPromiseValue(NULL),
	m_paiDeclaredWarOnFriendValue(NULL),
	m_paiNumCitiesLiberated(NULL),
	m_paiTradeValue(NULL),
	m_paiCommonFoeValue(NULL),
	m_paiAssistValue(NULL),

	m_ppaabWorkingAgainstPlayerAccepted(NULL),
	m_ppaaiWorkingAgainstPlayerCounter(NULL),

	m_ppaacCoopWarAcceptedState(NULL),
	m_ppaaiCoopWarCounter(NULL),

	m_pabPlayerMadeMilitaryPromise(NULL),
	m_pabPlayerBrokenMilitaryPromise(NULL),
	m_pabPlayerIgnoredMilitaryPromise(NULL),
	m_paiPlayerMilitaryPromiseCounter(NULL),

	m_paiPlayerMadeExpansionPromiseTurn(NULL),
	m_pabPlayerBrokenExpansionPromise(NULL),
	m_pabPlayerIgnoredExpansionPromise(NULL),
	m_paePlayerExpansionPromiseData(NULL),

	m_paiPlayerMadeBorderPromiseTurn(NULL),
	m_pabPlayerBrokenBorderPromise(NULL),
	m_pabPlayerIgnoredBorderPromise(NULL),
	m_paePlayerBorderPromiseData(NULL),

	m_pabPlayerMadeAttackCityStatePromise(NULL),
	m_pabPlayerBrokenAttackCityStatePromise(NULL),
	m_pabPlayerIgnoredAttackCityStatePromise(NULL),

	m_pabPlayerMadeBullyCityStatePromise(NULL),
	m_pabPlayerBrokenBullyCityStatePromise(NULL),
	m_pabPlayerIgnoredBullyCityStatePromise(NULL),

	m_pabPlayerMadeSpyPromise(NULL),
	m_pabPlayerBrokenSpyPromise(NULL),
	m_pabPlayerIgnoredSpyPromise(NULL),

	m_pabPlayerForgaveForSpying(NULL),

	m_pabPlayerMadeNoConvertPromise(NULL),
	m_pabPlayerBrokenNoConvertPromise(NULL),
	m_pabPlayerIgnoredNoConvertPromise(NULL),
	m_pabPlayerAskedNotToConvert(NULL),
	m_pabPlayerAgreedNotToConvert(NULL),

	m_pabPlayerMadeNoDiggingPromise(NULL),
	m_pabPlayerBrokenNoDiggingPromise(NULL),
	m_pabPlayerIgnoredNoDiggingPromise(NULL),
	m_pabPlayerAskedNotToDig(NULL),
	m_pabPlayerAgreedNotToDig(NULL),

	m_pabPlayerBrokenCoopWarPromise(NULL),

	m_paiOtherPlayerTurnsSinceAttackedProtectedMinor(NULL),
	m_paiOtherPlayerProtectedMinorAttacked(NULL),
	m_paiOtherPlayerNumProtectedMinorsAttacked(NULL),

	m_paiOtherPlayerTurnsSinceKilledProtectedMinor(NULL),
	m_paiOtherPlayerProtectedMinorKilled(NULL),
	m_paiOtherPlayerNumProtectedMinorsKilled(NULL),

	m_paiOtherPlayerNumProtectedMinorsBullied(NULL),

	m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor(NULL),

	m_paiOtherPlayerNumMinorsAttacked(NULL),
	m_paiOtherPlayerNumMinorsConquered(NULL),
	m_paiOtherPlayerNumMajorsAttacked(NULL),
	m_paiOtherPlayerNumMajorsConquered(NULL),

	m_paiOtherPlayerWarmongerAmount(NULL),

	m_paiOtherPlayerTurnsSinceWeLikedTheirProposal(NULL),
	m_paiOtherPlayerTurnsSinceWeDislikedTheirProposal(NULL),
	m_paiOtherPlayerTurnsSinceTheySupportedOurProposal(NULL),
	m_paiOtherPlayerTurnsSinceTheyFoiledOurProposal(NULL),
	m_paiOtherPlayerTurnsSinceTheySupportedOurHosting(NULL),

	m_ppaDiploStatementsLog(NULL),

	m_paDeclarationsLog(NULL),

	m_eStateAllWars(STATE_ALL_WARS_NEUTRAL),

	m_eTargetPlayer(NO_PLAYER),
	m_eTestToPlayer(NO_PLAYER),
	m_eTestStatement(NO_DIPLO_STATEMENT_TYPE),
	m_iTestStatementArg1(-1)
{
}


CvDiplomacyAI::~CvDiplomacyAI(void)
{
}


void CvDiplomacyAI::Init(CvPlayer* pPlayer)
{

	m_pPlayer = pPlayer;

	CvAssertMsg(!m_pDiploData, "MEMORY LEAK, CvDiplomacyAI::m_pDiploData");
	m_pDiploData = FNEW(DiplomacyAIData, c_eCiv5GameplayDLL, 0);


	m_paDiploLogStatementTurnCountScratchPad = &m_pDiploData->m_aDiploLogStatementTurnCountScratchPad[0];
	m_paeMajorCivOpinion = &m_pDiploData->m_aiMajorCivOpinion[0];
	m_paeMajorCivApproach = &m_pDiploData->m_aeMajorCivApproach[0];
	m_paeApproachScratchPad = &m_pDiploData->m_aeApproachScratchPad[0];
	m_paeMinorCivApproach = &m_pDiploData->m_aeMinorCivApproach[0];
	m_paeApproachTowardsUsGuess = &m_pDiploData->m_aeApproachTowardsUsGuess[0];
	m_paeApproachTowardsUsGuessCounter = &m_pDiploData->m_aeApproachTowardsUsGuessCounter[0];
	m_paeWantPeaceCounter = &m_pDiploData->m_aeWantPeaceCounter[0];
	m_paePeaceTreatyWillingToOffer = &m_pDiploData->m_aePeaceTreatyWillingToOffer[0];
	m_paePeaceTreatyWillingToAccept = &m_pDiploData->m_aePeaceTreatyWillingToAccept[0];
	m_paiNumWondersBeatenTo = &m_pDiploData->m_aiNumWondersBeatenTo[0];
	m_pabMusteringForAttack = &m_pDiploData->m_abMusteringForAttack[0];
	m_pabWantsResearchAgreementWithPlayer = &m_pDiploData->m_abWantsResearchAgreementWithPlayer[0];
	m_pabWantToRouteToMinor = &m_pDiploData->m_abWantToRouteToMinor[0];
	m_paeWarFace = &m_pDiploData->m_aeWarFace[0];
	m_paeWarState = &m_pDiploData->m_aeWarState[0];
	m_paeWarProjection = &m_pDiploData->m_aeWarProjection[0];
	m_paeLastWarProjection = &m_pDiploData->m_aeLastWarProjection[0];
	m_paeWarGoal = &m_pDiploData->m_aeWarGoal[0];
	m_paiPlayerNumTurnsAtWar = &m_pDiploData->m_aiPlayerNumTurnsAtWar[0];
	m_paiNumWarsFought = &m_pDiploData->m_aiNumWarsFought[0];
	m_paePlayerMilitaryStrengthComparedToUs = &m_pDiploData->m_aePlayerMilitaryStrengthComparedToUs[0];
	m_paePlayerEconomicStrengthComparedToUs = &m_pDiploData->m_aePlayerEconomicStrengthComparedToUs[0];
	m_paePlayerTargetValue = &m_pDiploData->m_aePlayerTargetValue[0];
	m_paePlayerLandDisputeLevel = &m_pDiploData->m_aePlayerLandDisputeLevel[0];
	m_paePlayerLastTurnLandDisputeLevel = &m_pDiploData->m_aePlayerLastTurnLandDisputeLevel[0];
	m_paePlayerVictoryDisputeLevel = &m_pDiploData->m_aePlayerVictoryDisputeLevel[0];
	m_paePlayerWonderDisputeLevel = &m_pDiploData->m_aePlayerWonderDisputeLevel[0];
	m_paePlayerMinorCivDisputeLevel = &m_pDiploData->m_aePlayerMinorCivDisputeLevel[0];
	m_paeMilitaryAggressivePosture = &m_pDiploData->m_aeMilitaryAggressivePosture[0];
	m_paeLastTurnMilitaryAggressivePosture = &m_pDiploData->m_aeLastTurnMilitaryAggressivePosture[0];
	m_paeExpansionAggressivePosture = &m_pDiploData->m_aeExpansionAggressivePosture[0];
	m_paePlotBuyingAggressivePosture = &m_pDiploData->m_aePlotBuyingAggressivePosture[0];
	m_paeMilitaryThreat = &m_pDiploData->m_aeMilitaryThreat[0];
	m_paeWarDamageLevel = &m_pDiploData->m_aeWarDamageLevel[0];
	m_paiWarValueLost = &m_pDiploData->m_aiWarValueLost[0];
	m_paeWarmongerThreat = &m_pDiploData->m_aeWarmongerThreat[0];
	m_paiPersonalityMajorCivApproachBiases = &m_pDiploData->m_aiPersonalityMajorCivApproachBiases[0];
	m_paiPersonalityMinorCivApproachBiases = &m_pDiploData->m_aiPersonalityMinorCivApproachBiases[0];
	m_paDeclarationsLog = &m_pDiploData->m_aDeclarationsLog[0];



	m_pabPlayerNoSettleRequestAccepted = &m_pDiploData->m_abPlayerNoSettleRequest[0];
	m_paiPlayerNoSettleRequestCounter = &m_pDiploData->m_aiPlayerNoSettleRequestCounter[0];

	m_pabPlayerStopSpyingRequestAccepted = &m_pDiploData->m_abPlayerStopSpyingRequest[0];
	m_paiPlayerStopSpyingRequestCounter = &m_pDiploData->m_aiPlayerStopSpyingRequestCounter[0];

	m_paiDemandCounter = &m_pDiploData->m_aiDemandCounter[0];
	m_paiDemandTooSoonNumTurns = &m_pDiploData->m_aiDemandTooSoonNumTurns[0];

	m_pabDoFAccepted = &m_pDiploData->m_abDoFAccepted[0];
	m_paiDoFCounter = &m_pDiploData->m_aiDoFCounter[0];

	m_pabDenouncedPlayer = &m_pDiploData->m_abDenouncedPlayer[0];
	m_pabFriendDenouncedUs = &m_pDiploData->m_abFriendDenouncedUs[0];
	m_pabFriendDeclaredWarOnUs = &m_pDiploData->m_abFriendDeclaredWarOnUs[0];
	m_paiDenouncedPlayerCounter = &m_pDiploData->m_aiDenouncedPlayerCounter[0];

	m_paiNumRequestsRefused = &m_pDiploData->m_aiNumRequestsRefused[0];

	m_paiNumCiviliansReturnedToMe = &m_pDiploData->m_aiNumCiviliansReturnedToMe[0];
	m_paiNumLandmarksBuiltForMe = &m_pDiploData->m_aiNumLandmarksBuiltForMe[0];
	m_paiResurrectedOnTurn = &m_pDiploData->m_aiResurrectedOnTurn[0];
	m_paiNumTimesCultureBombed = &m_pDiploData->m_aiNumTimesCultureBombed[0];

	m_paiNegativeReligiousConversionPoints = &m_pDiploData->m_paiNegativeReligiousConversionPoints[0];

	m_paiNegativeArchaeologyPoints = &m_pDiploData->m_paiNegativeArchaeologyPoints[0];

	m_paiNumTimesNuked = &m_pDiploData->m_aiNumTimesNuked[0];
	m_paiNumTimesRobbedBy = &m_pDiploData->m_aiNumTimesRobbedBy[0];
	m_paiNumTimesIntrigueSharedBy = &m_pDiploData->m_aiNumTimesIntrigueSharedBy[0];

	m_paiBrokenExpansionPromiseValue = &m_pDiploData->m_aiBrokenExpansionPromiseValue[0];
	m_paiIgnoredExpansionPromiseValue = &m_pDiploData->m_aiIgnoredExpansionPromiseValue[0];
	m_paiBrokenBorderPromiseValue = &m_pDiploData->m_aiBrokenBorderPromiseValue[0];
	m_paiIgnoredBorderPromiseValue = &m_pDiploData->m_aiIgnoredBorderPromiseValue[0];
	m_paiDeclaredWarOnFriendValue = &m_pDiploData->m_aiDeclaredWarOnFriendValue[0];
	m_paiNumCitiesLiberated = &m_pDiploData->m_aiNumCitiesLiberated[0];
	m_paiTradeValue = &m_pDiploData->m_aiTradeValue[0];
	m_paiCommonFoeValue = &m_pDiploData->m_aiCommonFoeValue[0];
	m_paiAssistValue = &m_pDiploData->m_aiAssistValue[0];



	m_pabPlayerMadeMilitaryPromise = &m_pDiploData->m_abPlayerMadeMilitaryPromise[0];
	m_pabPlayerBrokenMilitaryPromise = &m_pDiploData->m_abPlayerBrokenMilitaryPromise[0];
	m_pabPlayerIgnoredMilitaryPromise = &m_pDiploData->m_abPlayerIgnoredMilitaryPromise[0];
	m_paiPlayerMilitaryPromiseCounter = &m_pDiploData->m_aiPlayerMilitaryPromiseCounter[0];

	m_paiPlayerMadeExpansionPromiseTurn = &m_pDiploData->m_aiPlayerMadeExpansionPromiseTurn[0];
	m_pabPlayerBrokenExpansionPromise = &m_pDiploData->m_abPlayerBrokenExpansionPromise[0];
	m_pabPlayerIgnoredExpansionPromise = &m_pDiploData->m_abPlayerIgnoredExpansionPromise[0];
	m_paePlayerExpansionPromiseData = &m_pDiploData->m_aePlayerExpansionPromiseData[0];

	m_paiPlayerMadeBorderPromiseTurn = &m_pDiploData->m_aiPlayerMadeBorderPromiseTurn[0];
	m_pabPlayerBrokenBorderPromise = &m_pDiploData->m_abPlayerBrokenBorderPromise[0];
	m_pabPlayerIgnoredBorderPromise = &m_pDiploData->m_abPlayerIgnoredBorderPromise[0];
	m_paePlayerBorderPromiseData = &m_pDiploData->m_aePlayerBorderPromiseData[0];

	m_pabPlayerMadeAttackCityStatePromise = &m_pDiploData->m_abPlayerMadeAttackCityStatePromise[0];
	m_pabPlayerBrokenAttackCityStatePromise = &m_pDiploData->m_abPlayerBrokenAttackCityStatePromise[0];
	m_pabPlayerIgnoredAttackCityStatePromise = &m_pDiploData->m_abPlayerIgnoredAttackCityStatePromise[0];

	m_pabPlayerMadeBullyCityStatePromise = &m_pDiploData->m_abPlayerMadeBullyCityStatePromise[0];
	m_pabPlayerBrokenBullyCityStatePromise = &m_pDiploData->m_abPlayerBrokenBullyCityStatePromise[0];
	m_pabPlayerIgnoredBullyCityStatePromise = &m_pDiploData->m_abPlayerIgnoredBullyCityStatePromise[0];

	m_pabPlayerMadeSpyPromise = &m_pDiploData->m_abPlayerMadeSpyPromise[0];
	m_pabPlayerBrokenSpyPromise = &m_pDiploData->m_abPlayerBrokenSpyPromise[0];
	m_pabPlayerIgnoredSpyPromise = &m_pDiploData->m_abPlayerIgnoredSpyPromise[0];

	m_pabPlayerForgaveForSpying = &m_pDiploData->m_abPlayerForgaveForSpying[0];

	m_pabPlayerMadeNoConvertPromise = &m_pDiploData->m_abPlayerMadeNoConvertPromise[0];
	m_pabPlayerBrokenNoConvertPromise = &m_pDiploData->m_abPlayerBrokenNoConvertPromise[0];
	m_pabPlayerIgnoredNoConvertPromise = &m_pDiploData->m_abPlayerIgnoredNoConvertPromise[0];

	m_pabPlayerAskedNotToConvert = &m_pDiploData->m_abPlayerAskedNotToConvert[0];
	m_pabPlayerAgreedNotToConvert = &m_pDiploData->m_abPlayerAgreedNotToConvert[0];

	m_pabPlayerMadeNoDiggingPromise = &m_pDiploData->m_abPlayerMadeNoDiggingPromise[0];
	m_pabPlayerBrokenNoDiggingPromise = &m_pDiploData->m_abPlayerBrokenNoDiggingPromise[0];
	m_pabPlayerIgnoredNoDiggingPromise = &m_pDiploData->m_abPlayerIgnoredNoDiggingPromise[0];

	m_pabPlayerAskedNotToDig = &m_pDiploData->m_abPlayerAskedNotToDig[0];
	m_pabPlayerAgreedNotToDig = &m_pDiploData->m_abPlayerAgreedNotToDig[0];

	m_pabPlayerBrokenCoopWarPromise = &m_pDiploData->m_abPlayerBrokenCoopWarPromise[0];


	m_paiOtherPlayerTurnsSinceAttackedProtectedMinor = &m_pDiploData->m_aiOtherPlayerTurnsSinceAttackedProtectedMinor[0];
	m_paiOtherPlayerProtectedMinorAttacked = &m_pDiploData->m_aiOtherPlayerProtectedMinorAttacked[0];
	m_paiOtherPlayerNumProtectedMinorsAttacked = &m_pDiploData->m_aiOtherPlayerNumProtectedMinorsAttacked[0];
	m_paiOtherPlayerTurnsSinceKilledProtectedMinor = &m_pDiploData->m_aiOtherPlayerTurnsSinceKilledProtectedMinor[0];
	m_paiOtherPlayerProtectedMinorKilled = &m_pDiploData->m_aiOtherPlayerProtectedMinorKilled[0];
	m_paiOtherPlayerNumProtectedMinorsKilled = &m_pDiploData->m_aiOtherPlayerNumProtectedMinorsKilled[0];
	m_paiOtherPlayerNumProtectedMinorsBullied = &m_pDiploData->m_aiOtherPlayerNumProtectedMinorsBullied[0];
	m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor = &m_pDiploData->m_aiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor[0];
	m_paiOtherPlayerNumMinorsAttacked = &m_pDiploData->m_aiOtherPlayerNumMinorsAttacked[0];
	m_paiOtherPlayerNumMinorsConquered = &m_pDiploData->m_aiOtherPlayerNumMinorsConquered[0];
	m_paiOtherPlayerNumMajorsAttacked = &m_pDiploData->m_aiOtherPlayerNumMajorsAttacked[0];
	m_paiOtherPlayerNumMajorsConquered = &m_pDiploData->m_aiOtherPlayerNumMajorsConquered[0];
	m_paiOtherPlayerWarmongerAmount = &m_pDiploData->m_aiOtherPlayerWarmongerAmount[0];

	m_paiOtherPlayerTurnsSinceWeLikedTheirProposal = &m_pDiploData->m_aiOtherPlayerTurnsSinceWeLikedTheirProposal[0];
	m_paiOtherPlayerTurnsSinceWeDislikedTheirProposal = &m_pDiploData->m_aiOtherPlayerTurnsSinceWeDislikedTheirProposal[0];
	m_paiOtherPlayerTurnsSinceTheySupportedOurProposal = &m_pDiploData->m_aiOtherPlayerTurnsSinceTheySupportedOurProposal[0];
	m_paiOtherPlayerTurnsSinceTheyFoiledOurProposal = &m_pDiploData->m_aiOtherPlayerTurnsSinceTheyFoiledOurProposal[0];
	m_paiOtherPlayerTurnsSinceTheySupportedOurHosting = &m_pDiploData->m_aiOtherPlayerTurnsSinceTheySupportedOurHosting[0];


	int iI;
	m_ppaaeOtherPlayerMajorCivOpinion = &m_pDiploData->m_apaeOtherPlayerMajorCivOpinion[0];
	for(iI = 0; iI <MAX_MAJOR_CIVS; iI++)
	{
		m_ppaaeOtherPlayerMajorCivOpinion[iI] = &m_pDiploData->m_aaeOtherPlayerMajorCivOpinion[ MAX_MAJOR_CIVS * iI ];
	}

	m_ppaaeOtherPlayerLandDisputeLevel = &m_pDiploData->m_apaeOtherPlayerLandDisputeLevel[0];
	for(iI = 0; iI <MAX_CIV_PLAYERS; iI++)
	{
		m_ppaaeOtherPlayerLandDisputeLevel[iI] = &m_pDiploData->m_aaeOtherPlayerLandDisputeLevel[ REALLY_MAX_PLAYERS * iI ];
	}

	m_ppaaeOtherPlayerVictoryDisputeLevel = &m_pDiploData->m_apaeOtherPlayerVictoryDisputeLevel[0];
	for(iI = 0; iI <MAX_CIV_PLAYERS; iI++)
	{
		m_ppaaeOtherPlayerVictoryDisputeLevel[iI] = &m_pDiploData->m_aaeOtherPlayerVictoryDisputeLevel[ REALLY_MAX_PLAYERS * iI ];
	}

	m_ppaaeOtherPlayerWarDamageLevel = &m_pDiploData->m_apaeOtherPlayerWarDamageLevel[0];
	for(iI = 0; iI <MAX_CIV_PLAYERS; iI++)
	{
		m_ppaaeOtherPlayerWarDamageLevel[iI] = &m_pDiploData->m_aaeOtherPlayerWarDamageLevel[REALLY_MAX_PLAYERS * iI];
	}

	m_ppaaiOtherPlayerWarValueLost = &m_pDiploData->m_apaiOtherPlayerWarValueLost[0];
	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		m_ppaaiOtherPlayerWarValueLost[iI] = &m_pDiploData->m_aaiOtherPlayerWarValueLost[REALLY_MAX_PLAYERS*iI];
	}

	m_ppaaiOtherPlayerLastRoundWarValueLost = &m_pDiploData->m_apaiOtherPlayerLastRoundWarValueLost[0];
	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		m_ppaaiOtherPlayerLastRoundWarValueLost[iI] = &m_pDiploData->m_aaiOtherPlayerLastRoundWarValueLost[REALLY_MAX_PLAYERS*iI];
	}

	m_ppaabSentAttackMessageToMinorCivProtector = &m_pDiploData->m_apabSentAttackMessageToMinorCivProtector[0];
	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		m_ppaabSentAttackMessageToMinorCivProtector[iI] = &m_pDiploData->m_aabSentAttackMessageToMinorCivProtector[REALLY_MAX_PLAYERS* iI];
	}

	m_ppaaeOtherPlayerMilitaryThreat = &m_pDiploData->m_apaeOtherPlayerMilitaryThreat[0];
	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		m_ppaaeOtherPlayerMilitaryThreat[iI] = &m_pDiploData->m_aaeOtherPlayerMilitaryThreat[REALLY_MAX_PLAYERS*iI];
	}

	m_ppaDiploStatementsLog = &m_pDiploData->m_apaDiploStatementsLog[0];
	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_ppaDiploStatementsLog[iI] = &m_pDiploData->m_aaDiploStatementsLog[iI * MAX_DIPLO_LOG_STATEMENTS];
	}

	m_ppaabWorkingAgainstPlayerAccepted = &m_pDiploData->m_apabWorkingAgainstPlayerAccepted[0];
	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_ppaabWorkingAgainstPlayerAccepted[iI] = &m_pDiploData->m_aabWorkingAgainstPlayerAccepted[iI * MAX_MAJOR_CIVS];
	}

	m_ppaaiWorkingAgainstPlayerCounter = &m_pDiploData->m_apaiWorkingAgainstPlayerCounter[0];
	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_ppaaiWorkingAgainstPlayerCounter[iI] = &m_pDiploData->m_aaiWorkingAgainstPlayerCounter[iI * MAX_MAJOR_CIVS];
	}

	m_ppaacCoopWarAcceptedState = &m_pDiploData->m_apacCoopWarAcceptedState[0];
	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_ppaacCoopWarAcceptedState[iI] = &m_pDiploData->m_aacCoopWarAcceptedState[iI * MAX_MAJOR_CIVS];
	}

	m_ppaaiCoopWarCounter = &m_pDiploData->m_apaiCoopWarCounter[0];
	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_ppaaiCoopWarCounter[iI] = &m_pDiploData->m_aaiCoopWarCounter[iI * MAX_MAJOR_CIVS];
	}

	Reset();
}


void CvDiplomacyAI::Uninit()
{


	m_paDiploLogStatementTurnCountScratchPad = NULL;
	m_paeMajorCivOpinion = NULL;
	m_paeMajorCivApproach = NULL;
	m_paeApproachScratchPad = NULL;
	m_paeMinorCivApproach = NULL;
	m_paeApproachTowardsUsGuess = NULL;
	m_paeApproachTowardsUsGuessCounter = NULL;
	m_paeWantPeaceCounter = NULL;
	m_paePeaceTreatyWillingToOffer = NULL;
	m_paePeaceTreatyWillingToAccept = NULL;
	m_paiNumWondersBeatenTo = NULL;
	m_pabMusteringForAttack = NULL;
	m_pabWantsResearchAgreementWithPlayer = NULL;
	m_pabWantToRouteToMinor = NULL;
	m_paeWarFace = NULL;
	m_paeWarState = NULL;
	m_paeWarProjection = NULL;
	m_paeLastWarProjection = NULL;
	m_paeWarGoal = NULL;
	m_paiPlayerNumTurnsAtWar = NULL;
	m_paiNumWarsFought = NULL;
	m_paePlayerMilitaryStrengthComparedToUs = NULL;
	m_paePlayerEconomicStrengthComparedToUs = NULL;
	m_paePlayerTargetValue = NULL;
	m_paePlayerLandDisputeLevel = NULL;
	m_paePlayerLastTurnLandDisputeLevel = NULL;
	m_paePlayerVictoryDisputeLevel = NULL;
	m_paePlayerWonderDisputeLevel = NULL;
	m_paePlayerMinorCivDisputeLevel = NULL;
	m_paeMilitaryAggressivePosture = NULL;
	m_paeLastTurnMilitaryAggressivePosture = NULL;
	m_paeExpansionAggressivePosture = NULL;
	m_paePlotBuyingAggressivePosture = NULL;
	m_paeMilitaryThreat = NULL;
	m_paeWarDamageLevel = NULL;
	m_paiWarValueLost = NULL;
	m_paeWarmongerThreat = NULL;
	m_paiPersonalityMajorCivApproachBiases = NULL;
	m_paiPersonalityMinorCivApproachBiases = NULL;
	m_paDeclarationsLog = NULL;



	m_pabPlayerNoSettleRequestAccepted = NULL;
	m_paiPlayerNoSettleRequestCounter = NULL;

	m_pabPlayerStopSpyingRequestAccepted = NULL;
	m_paiPlayerStopSpyingRequestCounter = NULL;

	m_paiDemandCounter = NULL;
	m_paiDemandTooSoonNumTurns = NULL;

	m_pabDoFAccepted = NULL;
	m_paiDoFCounter = NULL;

	m_pabDenouncedPlayer = NULL;
	m_pabFriendDenouncedUs = NULL;
	m_pabFriendDeclaredWarOnUs = NULL;
	m_paiDenouncedPlayerCounter = NULL;

	m_paiNumRequestsRefused = NULL;

	m_paiNumCiviliansReturnedToMe = NULL;
	m_paiNumLandmarksBuiltForMe = NULL;
	m_paiResurrectedOnTurn = NULL;
	m_paiNumTimesCultureBombed = NULL;

	m_paiNegativeReligiousConversionPoints = NULL;

	m_paiNegativeArchaeologyPoints = NULL;

	m_paiNumTimesNuked = NULL;
	m_paiNumTimesRobbedBy = NULL;
	m_paiNumTimesIntrigueSharedBy = NULL;

	m_paiBrokenExpansionPromiseValue = NULL;
	m_paiIgnoredExpansionPromiseValue = NULL;
	m_paiBrokenBorderPromiseValue = NULL;
	m_paiIgnoredBorderPromiseValue = NULL;
	m_paiDeclaredWarOnFriendValue = NULL;
	m_paiNumCitiesLiberated = NULL;
	m_paiTradeValue = NULL;
	m_paiCommonFoeValue = NULL;
	m_paiAssistValue = NULL;

	m_ppaabWorkingAgainstPlayerAccepted = NULL;
	m_ppaaiWorkingAgainstPlayerCounter = NULL;

	m_ppaacCoopWarAcceptedState = NULL;
	m_ppaaiCoopWarCounter = NULL;



	m_pabPlayerMadeMilitaryPromise = NULL;
	m_pabPlayerBrokenMilitaryPromise = NULL;
	m_pabPlayerIgnoredMilitaryPromise = NULL;
	m_paiPlayerMilitaryPromiseCounter = NULL;

	m_paiPlayerMadeExpansionPromiseTurn = NULL;
	m_pabPlayerBrokenExpansionPromise = NULL;
	m_pabPlayerIgnoredExpansionPromise = NULL;
	m_paePlayerExpansionPromiseData = NULL;

	m_paiPlayerMadeBorderPromiseTurn = NULL;
	m_pabPlayerBrokenBorderPromise = NULL;
	m_pabPlayerIgnoredBorderPromise = NULL;
	m_paePlayerBorderPromiseData = NULL;

	m_pabPlayerMadeAttackCityStatePromise = NULL;
	m_pabPlayerBrokenAttackCityStatePromise = NULL;
	m_pabPlayerIgnoredAttackCityStatePromise = NULL;

	m_pabPlayerMadeBullyCityStatePromise = NULL;
	m_pabPlayerBrokenBullyCityStatePromise = NULL;
	m_pabPlayerIgnoredBullyCityStatePromise = NULL;

	m_pabPlayerMadeSpyPromise = NULL;
	m_pabPlayerBrokenSpyPromise = NULL;
	m_pabPlayerIgnoredSpyPromise = NULL;

	m_pabPlayerForgaveForSpying = NULL;

	m_pabPlayerMadeNoConvertPromise = NULL;
	m_pabPlayerBrokenNoConvertPromise = NULL;
	m_pabPlayerIgnoredNoConvertPromise = NULL;
	m_pabPlayerAskedNotToConvert = NULL;
	m_pabPlayerAgreedNotToConvert = NULL;

	m_pabPlayerMadeNoDiggingPromise = NULL;
	m_pabPlayerBrokenNoDiggingPromise = NULL;
	m_pabPlayerIgnoredNoDiggingPromise = NULL;
	m_pabPlayerAskedNotToDig = NULL;
	m_pabPlayerAgreedNotToDig = NULL;

	m_pabPlayerBrokenCoopWarPromise = NULL;


	m_paiOtherPlayerTurnsSinceAttackedProtectedMinor = NULL;
	m_paiOtherPlayerProtectedMinorAttacked = NULL;
	m_paiOtherPlayerNumProtectedMinorsAttacked = NULL;
	m_paiOtherPlayerTurnsSinceKilledProtectedMinor = NULL;
	m_paiOtherPlayerProtectedMinorKilled = NULL;
	m_paiOtherPlayerNumProtectedMinorsKilled = NULL;
	m_paiOtherPlayerNumProtectedMinorsBullied = NULL;
	m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor = NULL;
	m_paiOtherPlayerNumMinorsAttacked = NULL;
	m_paiOtherPlayerNumMinorsConquered = NULL;
	m_paiOtherPlayerNumMajorsAttacked = NULL;
	m_paiOtherPlayerNumMajorsConquered = NULL;
	m_paiOtherPlayerWarmongerAmount = NULL;

	m_paiOtherPlayerTurnsSinceWeLikedTheirProposal = NULL;
	m_paiOtherPlayerTurnsSinceWeDislikedTheirProposal = NULL;
	m_paiOtherPlayerTurnsSinceTheySupportedOurProposal = NULL;
	m_paiOtherPlayerTurnsSinceTheyFoiledOurProposal = NULL;
	m_paiOtherPlayerTurnsSinceTheySupportedOurHosting = NULL;

	m_ppaaeOtherPlayerLandDisputeLevel = NULL;
	m_ppaaeOtherPlayerVictoryDisputeLevel = NULL;
	m_ppaaeOtherPlayerWarDamageLevel = NULL;
	m_ppaaiOtherPlayerWarValueLost = NULL;
	m_ppaaiOtherPlayerLastRoundWarValueLost = NULL;
	m_ppaabSentAttackMessageToMinorCivProtector = NULL;
	m_ppaaeOtherPlayerMilitaryThreat = NULL;
	m_ppaDiploStatementsLog = NULL;

	delete m_pDiploData;
	m_pDiploData = NULL;
}


void CvDiplomacyAI::Reset()
{
	int iI, iJ;

	for(iI = 0; iI < NUM_DIPLO_LOG_STATEMENT_TYPES; iI++)
	{
		m_paDiploLogStatementTurnCountScratchPad[iI] = NO_DIPLO_STATEMENT_TYPE;
	}

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_paeMajorCivOpinion[iI] = NO_MAJOR_CIV_OPINION_TYPE;

		for(iJ = 0; iJ < MAX_MAJOR_CIVS; iJ++)
		{
			m_ppaaeOtherPlayerMajorCivOpinion[iI][iJ] = NO_MAJOR_CIV_OPINION_TYPE;
			m_ppaabWorkingAgainstPlayerAccepted[iI][iJ] = false;
			m_ppaaiWorkingAgainstPlayerCounter[iI][iJ] = -1;
			m_ppaacCoopWarAcceptedState[iI][iJ] = NO_COOP_WAR_STATE;
			m_ppaaiCoopWarCounter[iI][iJ] = -1;
		}

		m_paeMajorCivApproach[iI] = NO_MAJOR_CIV_APPROACH;
		m_paeApproachTowardsUsGuess[iI] = NO_MAJOR_CIV_APPROACH;
		m_paeApproachTowardsUsGuessCounter[iI] = 0;

		m_paePeaceTreatyWillingToOffer[iI] = NO_PEACE_TREATY_TYPE;
		m_paePeaceTreatyWillingToAccept[iI] = NO_PEACE_TREATY_TYPE;

		m_paiOtherPlayerTurnsSinceAttackedProtectedMinor[iI] = -1;
		m_paiOtherPlayerProtectedMinorAttacked[iI] = NO_PLAYER;
		m_paiOtherPlayerNumProtectedMinorsAttacked[iI] = 0;

		m_paiOtherPlayerTurnsSinceKilledProtectedMinor[iI] = -1;
		m_paiOtherPlayerProtectedMinorKilled[iI] = NO_PLAYER;
		m_paiOtherPlayerNumProtectedMinorsKilled[iI] = 0;

		m_paiOtherPlayerNumProtectedMinorsBullied[iI] = 0;

		m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor[iI] = -1;

		m_paiOtherPlayerNumMinorsAttacked[iI] = 0;
		m_paiOtherPlayerNumMinorsConquered[iI] = 0;
		m_paiOtherPlayerNumMajorsAttacked[iI] = 0;
		m_paiOtherPlayerNumMajorsConquered[iI] = 0;
		
		m_paiOtherPlayerWarmongerAmount[iI] = 0;

		m_paiOtherPlayerTurnsSinceWeLikedTheirProposal[iI] = -1;
		m_paiOtherPlayerTurnsSinceWeDislikedTheirProposal[iI] = -1;
		m_paiOtherPlayerTurnsSinceTheySupportedOurProposal[iI] = -1;
		m_paiOtherPlayerTurnsSinceTheyFoiledOurProposal[iI] = -1;
		m_paiOtherPlayerTurnsSinceTheySupportedOurHosting[iI] = -1;

		m_pabWantsResearchAgreementWithPlayer[iI] = false;



		m_pabPlayerNoSettleRequestAccepted[iI] = false;
		m_paiPlayerNoSettleRequestCounter[iI] = -1;

		m_pabPlayerStopSpyingRequestAccepted[iI] = false;
		m_paiPlayerStopSpyingRequestCounter[iI] = -1;

		m_paiDemandCounter[iI] = -1;
		m_paiDemandTooSoonNumTurns[iI] = -1;

		m_pabDoFAccepted[iI] = false;
		m_paiDoFCounter[iI] = -1;

		m_pabDenouncedPlayer[iI] = false;
		m_pabFriendDenouncedUs[iI] = false;
		m_pabFriendDeclaredWarOnUs[iI] = false;
		m_paiDenouncedPlayerCounter[iI] = -1;

		m_paiNumRequestsRefused[iI] = 0;

		m_paiNumCiviliansReturnedToMe[iI] = 0;
		m_paiNumLandmarksBuiltForMe[iI] = 0;
		m_paiResurrectedOnTurn[iI] = -1;
		m_paiNumTimesCultureBombed[iI] = 0;

		m_paiNegativeReligiousConversionPoints[iI] = 0;

		m_paiNegativeArchaeologyPoints[iI] = 0;

		m_paiNumTimesNuked[iI] = 0;
		m_paiNumTimesRobbedBy[iI] = 0;
		m_paiNumTimesIntrigueSharedBy[iI] = 0;

		m_paiBrokenExpansionPromiseValue[iI] = 0;
		m_paiIgnoredExpansionPromiseValue[iI] = 0;
		m_paiBrokenBorderPromiseValue[iI] = 0;
		m_paiIgnoredBorderPromiseValue[iI] = 0;
		m_paiDeclaredWarOnFriendValue[iI] = 0;
		m_paiNumCitiesLiberated[iI] = 0;
		m_paiTradeValue[iI] = 0;
		m_paiCommonFoeValue[iI] = 0;
		m_paiAssistValue[iI] = 0;



		m_pabPlayerMadeMilitaryPromise[iI] = false;
		m_pabPlayerBrokenMilitaryPromise[iI] = false;
		m_pabPlayerIgnoredMilitaryPromise[iI] = false;
		m_paiPlayerMilitaryPromiseCounter[iI] = -1;

		m_paiPlayerMadeExpansionPromiseTurn[iI] = -1;
		m_pabPlayerBrokenExpansionPromise[iI] = false;
		m_pabPlayerIgnoredExpansionPromise[iI] = false;
		m_paePlayerExpansionPromiseData[iI] = NO_AGGRESSIVE_POSTURE_TYPE;

		m_paiPlayerMadeBorderPromiseTurn[iI] = -1;
		m_pabPlayerBrokenBorderPromise[iI] = false;
		m_pabPlayerIgnoredBorderPromise[iI] = false;
		m_paePlayerBorderPromiseData[iI] = NO_AGGRESSIVE_POSTURE_TYPE;

		m_pabPlayerMadeAttackCityStatePromise[iI] = false;
		m_pabPlayerBrokenAttackCityStatePromise[iI] = false;
		m_pabPlayerIgnoredAttackCityStatePromise[iI] = false;

		m_pabPlayerMadeBullyCityStatePromise[iI] = false;
		m_pabPlayerBrokenBullyCityStatePromise[iI] = false;
		m_pabPlayerIgnoredBullyCityStatePromise[iI] = false;

		m_pabPlayerMadeSpyPromise[iI] = false;
		m_pabPlayerBrokenSpyPromise[iI] = false;
		m_pabPlayerIgnoredSpyPromise[iI] = false;

		m_pabPlayerForgaveForSpying[iI] = false;

		m_pabPlayerMadeNoConvertPromise[iI] = false;
		m_pabPlayerBrokenNoConvertPromise[iI] = false;
		m_pabPlayerIgnoredNoConvertPromise[iI] = false;
		m_pabPlayerAskedNotToConvert[iI] = false;
		m_pabPlayerAgreedNotToConvert[iI] = false;

		m_pabPlayerMadeNoDiggingPromise[iI] = false;
		m_pabPlayerBrokenNoDiggingPromise[iI] = false;
		m_pabPlayerIgnoredNoDiggingPromise[iI] = false;
		m_pabPlayerAskedNotToDig[iI] = false;
		m_pabPlayerAgreedNotToDig[iI] = false;

		m_pabPlayerBrokenCoopWarPromise[iI] = false;
	}

	for(iI = 0; iI < MAX_MINOR_CIVS; iI++)
	{
		m_paeMinorCivApproach[iI] = NO_MINOR_CIV_APPROACH;
		m_pabWantToRouteToMinor[iI] = true;
	}

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		m_paeApproachScratchPad[iI] = -1;

		m_paiNumWondersBeatenTo[iI] = 0;
		m_pabMusteringForAttack[iI] = false;

		m_paeWantPeaceCounter[iI] = 0;

		m_paeWarFace[iI] = NO_WAR_FACE_TYPE;
		m_paeWarState[iI] = NO_WAR_STATE_TYPE;
		m_paeWarProjection[iI] = NO_WAR_PROJECTION_TYPE;
		m_paeLastWarProjection[iI] = NO_WAR_PROJECTION_TYPE;
		m_paeWarGoal[iI] = NO_WAR_GOAL_TYPE;
		m_paiPlayerNumTurnsAtWar[iI] = 0;
		m_paiNumWarsFought[iI] = 0;

		m_paePlayerMilitaryStrengthComparedToUs[iI] = NO_STRENGTH_VALUE;
		m_paePlayerEconomicStrengthComparedToUs[iI] = NO_STRENGTH_VALUE;
		m_paePlayerTargetValue[iI] = NO_TARGET_VALUE;

		m_paePlayerLandDisputeLevel[iI] = NO_DISPUTE_LEVEL;
		m_paePlayerLastTurnLandDisputeLevel[iI] = NO_DISPUTE_LEVEL;
		m_paePlayerVictoryDisputeLevel[iI] = NO_DISPUTE_LEVEL;
		m_paePlayerWonderDisputeLevel[iI] = NO_DISPUTE_LEVEL;
		m_paePlayerMinorCivDisputeLevel[iI] = NO_DISPUTE_LEVEL;

		for(iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
		{
			m_ppaaeOtherPlayerLandDisputeLevel[iI][iJ] = NO_DISPUTE_LEVEL;
			m_ppaaeOtherPlayerVictoryDisputeLevel[iI][iJ] = NO_DISPUTE_LEVEL;

			m_ppaaeOtherPlayerWarDamageLevel[iI][iJ] = NO_WAR_DAMAGE_LEVEL_VALUE;
			m_ppaaiOtherPlayerWarValueLost[iI][iJ] = 0;
			m_ppaaiOtherPlayerLastRoundWarValueLost[iI][iJ] = 0;
			m_ppaabSentAttackMessageToMinorCivProtector[iI][iJ] = false;

			m_ppaaeOtherPlayerMilitaryThreat[iI][iJ] = NO_THREAT_VALUE;
		}

		m_paeWarDamageLevel[iI] = NO_WAR_DAMAGE_LEVEL_VALUE;
		m_paiWarValueLost[iI] = 0;

		m_paeMilitaryAggressivePosture[iI] = NO_AGGRESSIVE_POSTURE_TYPE;
		m_paeLastTurnMilitaryAggressivePosture[iI] = NO_AGGRESSIVE_POSTURE_TYPE;
		m_paeExpansionAggressivePosture[iI] = NO_AGGRESSIVE_POSTURE_TYPE;
		m_paePlotBuyingAggressivePosture[iI] = NO_AGGRESSIVE_POSTURE_TYPE;

		m_paeMilitaryThreat[iI] = NO_THREAT_VALUE;

		m_paeWarmongerThreat[iI] = NO_THREAT_VALUE;
	}

	m_eDemandTargetPlayer = NO_PLAYER;
	m_bDemandReady = false;

	m_iVictoryCompetitiveness = -1;
	m_iWonderCompetitiveness = -1;
	m_iMinorCivCompetitiveness = -1;
	m_iBoldness = -1;
	m_iDiploBalance = -1;
	m_iWarmongerHate = -1;
	m_iDenounceWillingness = -1;
	m_iDoFWillingness = -1;
	m_iLoyalty = -1;
	m_iNeediness = -1;
	m_iForgiveness = -1;
	m_iChattiness = -1;
	m_iMeanness = -1;

	for(iI = 0; iI < NUM_MAJOR_CIV_APPROACHES; iI++)
	{
		m_paiPersonalityMajorCivApproachBiases[iI] = 0;
	}

	for(iI = 0; iI < NUM_MINOR_CIV_APPROACHES; iI++)
	{
		m_paiPersonalityMinorCivApproachBiases[iI] = 0;
	}

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		for(iJ = 0; iJ < MAX_DIPLO_LOG_STATEMENTS; iJ++)
		{
			m_ppaDiploStatementsLog[iI][iJ].m_eDiploLogStatement = NO_DIPLO_STATEMENT_TYPE;
			m_ppaDiploStatementsLog[iI][iJ].m_iTurn = -1;
		}
	}

	for(iI = 0; iI < MAX_DIPLO_LOG_STATEMENTS; iI++)
	{
		m_paDeclarationsLog[iI].m_eDeclaration = NO_PUBLIC_DECLARATION_TYPE;
		m_paDeclarationsLog[iI].m_iTurn = -1;
	}

	m_eStateAllWars = STATE_ALL_WARS_NEUTRAL;

	m_aGreetPlayers.clear();
}


void CvDiplomacyAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	int iI;

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapMajorOpinion(MAX_MAJOR_CIVS, m_paeMajorCivOpinion);
#else
	ArrayWrapper<char> wrapMajorOpinion(MAX_MAJOR_CIVS, m_paeMajorCivOpinion);
#endif
	kStream >> wrapMajorOpinion;

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
#ifdef AUI_WARNING_FIXES
		ArrayWrapper<int> wrapOtherMajorOpinion(MAX_MAJOR_CIVS, m_ppaaeOtherPlayerMajorCivOpinion[iI]);
#else
		ArrayWrapper<char> wrapOtherMajorOpinion(MAX_MAJOR_CIVS, m_ppaaeOtherPlayerMajorCivOpinion[iI]);
#endif
		kStream >> wrapOtherMajorOpinion;

		ArrayWrapper<bool> wrapWorkingAgainstPlayerAccepted(MAX_MAJOR_CIVS, m_ppaabWorkingAgainstPlayerAccepted[iI]);
		kStream >> wrapWorkingAgainstPlayerAccepted;

#ifdef AUI_WARNING_FIXES
		ArrayWrapper<int> workingAgainstPlayerCounter(MAX_MAJOR_CIVS, m_ppaaiWorkingAgainstPlayerCounter[iI]);
		kStream >> workingAgainstPlayerCounter;

		ArrayWrapper<int> wrapCoopWarAcceptedState(MAX_MAJOR_CIVS, m_ppaacCoopWarAcceptedState[iI]);
		kStream >> wrapCoopWarAcceptedState;

		ArrayWrapper<int> wrapCoopWarCounter(MAX_MAJOR_CIVS, m_ppaaiCoopWarCounter[iI]);
		kStream >> wrapCoopWarCounter;
	}

	ArrayWrapper<int> wrapm_paeMajorCivApproach(MAX_MAJOR_CIVS, m_paeMajorCivApproach);
	kStream >> wrapm_paeMajorCivApproach;

	ArrayWrapper<int> wrapm_paeMinorCivApproach(MAX_MINOR_CIVS, m_paeMinorCivApproach);
	kStream >> wrapm_paeMinorCivApproach;

	ArrayWrapper<int> wrapm_paeApproachTowardsUsGuess(MAX_MAJOR_CIVS, m_paeApproachTowardsUsGuess);
	kStream >> wrapm_paeApproachTowardsUsGuess;

	ArrayWrapper<int> wrapm_paeApproachTowardsUsGuessCounter(MAX_MAJOR_CIVS, m_paeApproachTowardsUsGuessCounter);
	kStream >> wrapm_paeApproachTowardsUsGuessCounter;


	ArrayWrapper<int> wrapm_paiNumWondersBeatenTo(MAX_CIV_PLAYERS, m_paiNumWondersBeatenTo);
#else
		ArrayWrapper<short> workingAgainstPlayerCounter(MAX_MAJOR_CIVS, m_ppaaiWorkingAgainstPlayerCounter[iI]);
		kStream >> workingAgainstPlayerCounter;

		ArrayWrapper<char> wrapCoopWarAcceptedState(MAX_MAJOR_CIVS, m_ppaacCoopWarAcceptedState[iI]);
		kStream >> wrapCoopWarAcceptedState;

		ArrayWrapper<short> wrapCoopWarCounter(MAX_MAJOR_CIVS, m_ppaaiCoopWarCounter[iI]);
		kStream >> wrapCoopWarCounter;
	}

	ArrayWrapper<char> wrapm_paeMajorCivApproach(MAX_MAJOR_CIVS, m_paeMajorCivApproach);
	kStream >> wrapm_paeMajorCivApproach;

	ArrayWrapper<char> wrapm_paeMinorCivApproach(MAX_MINOR_CIVS, m_paeMinorCivApproach);
	kStream >> wrapm_paeMinorCivApproach;

	ArrayWrapper<char> wrapm_paeApproachTowardsUsGuess(MAX_MAJOR_CIVS, m_paeApproachTowardsUsGuess);
	kStream >> wrapm_paeApproachTowardsUsGuess;

	ArrayWrapper<char> wrapm_paeApproachTowardsUsGuessCounter(MAX_MAJOR_CIVS, m_paeApproachTowardsUsGuessCounter);
	kStream >> wrapm_paeApproachTowardsUsGuessCounter;


	ArrayWrapper<short> wrapm_paiNumWondersBeatenTo(MAX_CIV_PLAYERS, m_paiNumWondersBeatenTo);
#endif
	kStream >> wrapm_paiNumWondersBeatenTo;

	ArrayWrapper<bool> wrapm_pabMusteringForAttack(MAX_CIV_PLAYERS, m_pabMusteringForAttack);
	kStream >> wrapm_pabMusteringForAttack;


	ArrayWrapper<bool> wrapm_pabWantsResearchAgreementWithPlayer(MAX_MAJOR_CIVS, m_pabWantsResearchAgreementWithPlayer);
	kStream >> wrapm_pabWantsResearchAgreementWithPlayer;

	ArrayWrapper<bool> wrapm_pabWantToRouteToMinor(MAX_MINOR_CIVS, m_pabWantToRouteToMinor);
	kStream >> wrapm_pabWantToRouteToMinor;


#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapm_paeWantPeaceCounter(MAX_CIV_PLAYERS, m_paeWantPeaceCounter);
	kStream >> wrapm_paeWantPeaceCounter;

	ArrayWrapper<int> wrapm_paePeaceTreatyWillingToOffer(MAX_MAJOR_CIVS, m_paePeaceTreatyWillingToOffer);
	kStream >> wrapm_paePeaceTreatyWillingToOffer;

	ArrayWrapper<int> wrapm_paePeaceTreatyWillingToAccept(MAX_MAJOR_CIVS, m_paePeaceTreatyWillingToAccept);
	kStream >> wrapm_paePeaceTreatyWillingToAccept;


	ArrayWrapper<int> wrapm_paeWarFace(MAX_CIV_PLAYERS, m_paeWarFace);
	kStream >> wrapm_paeWarFace;

	ArrayWrapper<int> wrapm_paeWarState(MAX_CIV_PLAYERS, m_paeWarState);
	kStream >> wrapm_paeWarState;

	ArrayWrapper<int> wrapm_paeWarProjection(MAX_CIV_PLAYERS, m_paeWarProjection);
	kStream >> wrapm_paeWarProjection;

	ArrayWrapper<int> wrapm_paeLastWarProjection(MAX_CIV_PLAYERS, m_paeLastWarProjection);
	kStream >> wrapm_paeLastWarProjection;

	ArrayWrapper<int> wrapm_paeWarGoal(MAX_CIV_PLAYERS, m_paeWarGoal);
	kStream >> wrapm_paeWarGoal;

	ArrayWrapper<int> wrapm_paiPlayerNumTurnsAtWar(MAX_CIV_PLAYERS, m_paiPlayerNumTurnsAtWar);
	kStream >> wrapm_paiPlayerNumTurnsAtWar;

	ArrayWrapper<int> wrapm_paiNumWarsFought(MAX_CIV_PLAYERS, m_paiNumWarsFought);
	kStream >> wrapm_paiNumWarsFought;

	ArrayWrapper<int> wrapm_paePlayerMilitaryStrengthComparedToUs(MAX_CIV_PLAYERS, m_paePlayerMilitaryStrengthComparedToUs);
	kStream >> wrapm_paePlayerMilitaryStrengthComparedToUs;

	ArrayWrapper<int> wrapm_paePlayerEconomicStrengthComparedToUs(MAX_CIV_PLAYERS, m_paePlayerEconomicStrengthComparedToUs);
	kStream >> wrapm_paePlayerEconomicStrengthComparedToUs;

	ArrayWrapper<int> wrapm_paePlayerTargetValue(MAX_CIV_PLAYERS, m_paePlayerTargetValue);
	kStream >> wrapm_paePlayerTargetValue;


	ArrayWrapper<int> wrapm_paePlayerLandDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerLandDisputeLevel);
	kStream >> wrapm_paePlayerLandDisputeLevel;

	ArrayWrapper<int> wrapm_paePlayerLastTurnLandDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerLastTurnLandDisputeLevel);
	kStream >> wrapm_paePlayerLastTurnLandDisputeLevel;

	ArrayWrapper<int> wrapm_paePlayerVictoryDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerVictoryDisputeLevel);
	kStream >> wrapm_paePlayerVictoryDisputeLevel;

	ArrayWrapper<int> wrapm_paePlayerWonderDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerWonderDisputeLevel);
	kStream >> wrapm_paePlayerWonderDisputeLevel;

	ArrayWrapper<int> wrapm_paePlayerMinorCivDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerMinorCivDisputeLevel);
	kStream >> wrapm_paePlayerMinorCivDisputeLevel;


	for (iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		ArrayWrapper<int> wrapm_ppaaeOtherPlayerLandDisputeLevel(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerLandDisputeLevel[iI]);
		kStream >> wrapm_ppaaeOtherPlayerLandDisputeLevel;

		ArrayWrapper<int> wrapm_ppaaeOtherPlayerVictoryDisputeLevel(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerVictoryDisputeLevel[iI]);
		kStream >> wrapm_ppaaeOtherPlayerVictoryDisputeLevel;

		ArrayWrapper<int> wrapm_ppaaeOtherPlayerWarDamageLevel(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerWarDamageLevel[iI]);
#else
	ArrayWrapper<short> wrapm_paeWantPeaceCounter(MAX_CIV_PLAYERS, m_paeWantPeaceCounter);
	kStream >> wrapm_paeWantPeaceCounter;

	ArrayWrapper<char> wrapm_paePeaceTreatyWillingToOffer(MAX_MAJOR_CIVS, m_paePeaceTreatyWillingToOffer);
	kStream >> wrapm_paePeaceTreatyWillingToOffer;

	ArrayWrapper<char> wrapm_paePeaceTreatyWillingToAccept(MAX_MAJOR_CIVS, m_paePeaceTreatyWillingToAccept);
	kStream >> wrapm_paePeaceTreatyWillingToAccept;


	ArrayWrapper<char> wrapm_paeWarFace(MAX_CIV_PLAYERS, m_paeWarFace);
	kStream >> wrapm_paeWarFace;

	ArrayWrapper<char> wrapm_paeWarState(MAX_CIV_PLAYERS, m_paeWarState);
	kStream >> wrapm_paeWarState;

	ArrayWrapper<char> wrapm_paeWarProjection(MAX_CIV_PLAYERS, m_paeWarProjection);
	kStream >> wrapm_paeWarProjection;

	ArrayWrapper<char> wrapm_paeLastWarProjection(MAX_CIV_PLAYERS, m_paeLastWarProjection);
	kStream >> wrapm_paeLastWarProjection;

	ArrayWrapper<char> wrapm_paeWarGoal(MAX_CIV_PLAYERS, m_paeWarGoal);
	kStream >> wrapm_paeWarGoal;

	ArrayWrapper<short> wrapm_paiPlayerNumTurnsAtWar(MAX_CIV_PLAYERS, m_paiPlayerNumTurnsAtWar);
	kStream >> wrapm_paiPlayerNumTurnsAtWar;

	ArrayWrapper<short> wrapm_paiNumWarsFought(MAX_CIV_PLAYERS, m_paiNumWarsFought);
	kStream >> wrapm_paiNumWarsFought;

	ArrayWrapper<char> wrapm_paePlayerMilitaryStrengthComparedToUs(MAX_CIV_PLAYERS, m_paePlayerMilitaryStrengthComparedToUs);
	kStream >> wrapm_paePlayerMilitaryStrengthComparedToUs;

	ArrayWrapper<char> wrapm_paePlayerEconomicStrengthComparedToUs(MAX_CIV_PLAYERS, m_paePlayerEconomicStrengthComparedToUs);
	kStream >> wrapm_paePlayerEconomicStrengthComparedToUs;

	ArrayWrapper<char> wrapm_paePlayerTargetValue(MAX_CIV_PLAYERS, m_paePlayerTargetValue);
	kStream >> wrapm_paePlayerTargetValue;


	ArrayWrapper<char> wrapm_paePlayerLandDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerLandDisputeLevel);
	kStream >> wrapm_paePlayerLandDisputeLevel;

	ArrayWrapper<char> wrapm_paePlayerLastTurnLandDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerLastTurnLandDisputeLevel);
	kStream >> wrapm_paePlayerLastTurnLandDisputeLevel;

	ArrayWrapper<char> wrapm_paePlayerVictoryDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerVictoryDisputeLevel);
	kStream >> wrapm_paePlayerVictoryDisputeLevel;

	ArrayWrapper<char> wrapm_paePlayerWonderDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerWonderDisputeLevel);
	kStream >> wrapm_paePlayerWonderDisputeLevel;

	ArrayWrapper<char> wrapm_paePlayerMinorCivDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerMinorCivDisputeLevel);
	kStream >> wrapm_paePlayerMinorCivDisputeLevel;


	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		ArrayWrapper<char> wrapm_ppaaeOtherPlayerLandDisputeLevel(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerLandDisputeLevel[iI]);
		kStream >> wrapm_ppaaeOtherPlayerLandDisputeLevel;

		ArrayWrapper<char> wrapm_ppaaeOtherPlayerVictoryDisputeLevel(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerVictoryDisputeLevel[iI]);
		kStream >> wrapm_ppaaeOtherPlayerVictoryDisputeLevel;

		ArrayWrapper<char> wrapm_ppaaeOtherPlayerWarDamageLevel(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerWarDamageLevel[iI]);
#endif
		kStream >> wrapm_ppaaeOtherPlayerWarDamageLevel;

		ArrayWrapper<int> wrapm_ppaaiOtherPlayerWarValueLost(MAX_CIV_PLAYERS, m_ppaaiOtherPlayerWarValueLost[iI]);
		kStream >> wrapm_ppaaiOtherPlayerWarValueLost;

#ifdef AUI_WARNING_FIXES
		ArrayWrapper<int> wrapm_ppaaeOtherPlayerMilitaryThreat(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerMilitaryThreat[iI]);
#else
		ArrayWrapper<char> wrapm_ppaaeOtherPlayerMilitaryThreat(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerMilitaryThreat[iI]);
#endif
		kStream >> wrapm_ppaaeOtherPlayerMilitaryThreat;
	}

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		ArrayWrapper<int> wrapm_ppaaiOtherPlayerLastRoundWarValueLost(MAX_CIV_PLAYERS, m_ppaaiOtherPlayerLastRoundWarValueLost[iI]);
		kStream >> wrapm_ppaaiOtherPlayerLastRoundWarValueLost;

		ArrayWrapper<bool> wrapm_ppaabSentAttackMessageToMinorCivProtector(MAX_CIV_PLAYERS, m_ppaabSentAttackMessageToMinorCivProtector[iI]);
		kStream >> wrapm_ppaabSentAttackMessageToMinorCivProtector;
	}

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapm_paeWarDamageLevel(MAX_CIV_PLAYERS, m_paeWarDamageLevel);
#else
	ArrayWrapper<char> wrapm_paeWarDamageLevel(MAX_CIV_PLAYERS, m_paeWarDamageLevel);
#endif
	kStream >> wrapm_paeWarDamageLevel;

	ArrayWrapper<int> wrapm_paiWarValueLost(MAX_CIV_PLAYERS, m_paiWarValueLost);
	kStream >> wrapm_paiWarValueLost;

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapm_paeMilitaryAggressivePosture(MAX_CIV_PLAYERS, m_paeMilitaryAggressivePosture);
	kStream >> wrapm_paeMilitaryAggressivePosture;

	ArrayWrapper<int> wrapm_paeLastTurnMilitaryAggressivePosture(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryAggressivePosture);
	kStream >> wrapm_paeLastTurnMilitaryAggressivePosture;

	ArrayWrapper<int> wrapm_paeExpansionAggressivePosture(MAX_CIV_PLAYERS, m_paeExpansionAggressivePosture);
	kStream >> wrapm_paeExpansionAggressivePosture;

	ArrayWrapper<int> wrapm_paePlotBuyingAggressivePosture(MAX_CIV_PLAYERS, m_paePlotBuyingAggressivePosture);
	kStream >> wrapm_paePlotBuyingAggressivePosture;

	ArrayWrapper<int> wrapm_paeMilitaryThreat(MAX_CIV_PLAYERS, m_paeMilitaryThreat);
	kStream >> wrapm_paeMilitaryThreat;

	ArrayWrapper<int> wrapm_paeWarmongerThreat(MAX_CIV_PLAYERS, m_paeWarmongerThreat);
#else
	ArrayWrapper<char> wrapm_paeMilitaryAggressivePosture(MAX_CIV_PLAYERS, m_paeMilitaryAggressivePosture);
	kStream >> wrapm_paeMilitaryAggressivePosture;

	ArrayWrapper<char> wrapm_paeLastTurnMilitaryAggressivePosture(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryAggressivePosture);
	kStream >> wrapm_paeLastTurnMilitaryAggressivePosture;

	ArrayWrapper<char> wrapm_paeExpansionAggressivePosture(MAX_CIV_PLAYERS, m_paeExpansionAggressivePosture);
	kStream >> wrapm_paeExpansionAggressivePosture;

	ArrayWrapper<char> wrapm_paePlotBuyingAggressivePosture(MAX_CIV_PLAYERS, m_paePlotBuyingAggressivePosture);
	kStream >> wrapm_paePlotBuyingAggressivePosture;

	ArrayWrapper<char> wrapm_paeMilitaryThreat(MAX_CIV_PLAYERS, m_paeMilitaryThreat);
	kStream >> wrapm_paeMilitaryThreat;

	ArrayWrapper<char> wrapm_paeWarmongerThreat(MAX_CIV_PLAYERS, m_paeWarmongerThreat);
#endif
	kStream >> wrapm_paeWarmongerThreat;

	ArrayWrapper<bool> wrapm_pabPlayerNoSettleRequestAccepted(MAX_MAJOR_CIVS, m_pabPlayerNoSettleRequestAccepted);
	kStream >> wrapm_pabPlayerNoSettleRequestAccepted;

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapm_paiPlayerNoSettleRequestCounter(MAX_MAJOR_CIVS, m_paiPlayerNoSettleRequestCounter);
#else
	ArrayWrapper<short> wrapm_paiPlayerNoSettleRequestCounter(MAX_MAJOR_CIVS, m_paiPlayerNoSettleRequestCounter);
#endif
	kStream >> wrapm_paiPlayerNoSettleRequestCounter;

	ArrayWrapper<bool> wrapm_pabPlayerStopSpyingRequestAccepted(MAX_MAJOR_CIVS, m_pabPlayerStopSpyingRequestAccepted);
	kStream >> wrapm_pabPlayerStopSpyingRequestAccepted;

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapm_paiPlayerStopSpyingRequestCounter(MAX_MAJOR_CIVS, m_paiPlayerStopSpyingRequestCounter);
	kStream >> wrapm_paiPlayerStopSpyingRequestCounter;

	ArrayWrapper<int> wrapm_paiDemandCounter(MAX_MAJOR_CIVS, m_paiDemandCounter);
	kStream >> wrapm_paiDemandCounter;

	ArrayWrapper<int> wrapm_paiDemandTooSoonNumTurns(MAX_MAJOR_CIVS, m_paiDemandTooSoonNumTurns);
#else
	ArrayWrapper<short> wrapm_paiPlayerStopSpyingRequestCounter(MAX_MAJOR_CIVS, m_paiPlayerStopSpyingRequestCounter);
	kStream >> wrapm_paiPlayerStopSpyingRequestCounter;

	ArrayWrapper<short> wrapm_paiDemandCounter(MAX_MAJOR_CIVS, m_paiDemandCounter);
	kStream >> wrapm_paiDemandCounter;

	ArrayWrapper<short> wrapm_paiDemandTooSoonNumTurns(MAX_MAJOR_CIVS, m_paiDemandTooSoonNumTurns);
#endif
	kStream >> wrapm_paiDemandTooSoonNumTurns;

	ArrayWrapper<bool> wrapm_pabDoFAccepted(MAX_MAJOR_CIVS, m_pabDoFAccepted);
	kStream >> wrapm_pabDoFAccepted;

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapm_paiDoFCounter(MAX_MAJOR_CIVS, m_paiDoFCounter);
#else
	ArrayWrapper<short> wrapm_paiDoFCounter(MAX_MAJOR_CIVS, m_paiDoFCounter);
#endif
	kStream >> wrapm_paiDoFCounter;

	ArrayWrapper<bool> wrapm_pabDenouncedPlayer(MAX_MAJOR_CIVS, m_pabDenouncedPlayer);
	kStream >> wrapm_pabDenouncedPlayer;

	ArrayWrapper<bool> wrapm_pabFriendDenouncedUs(MAX_MAJOR_CIVS, m_pabFriendDenouncedUs);
	kStream >> wrapm_pabFriendDenouncedUs;

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapm_paiDenouncedPlayerCounter(MAX_MAJOR_CIVS, m_paiDenouncedPlayerCounter);
#else
	ArrayWrapper<short> wrapm_paiDenouncedPlayerCounter(MAX_MAJOR_CIVS, m_paiDenouncedPlayerCounter);
#endif
	kStream >> wrapm_paiDenouncedPlayerCounter;

	ArrayWrapper<bool> wrapm_pabFriendDeclaredWarOnUs(MAX_MAJOR_CIVS, m_pabFriendDeclaredWarOnUs);
	kStream >> wrapm_pabFriendDeclaredWarOnUs;

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapm_paiNumRequestsRefused(MAX_MAJOR_CIVS, m_paiNumRequestsRefused);
	kStream >> wrapm_paiNumRequestsRefused;

	ArrayWrapper<int> wrapm_paiNumCiviliansReturnedToMe(MAX_MAJOR_CIVS, m_paiNumCiviliansReturnedToMe);
	kStream >> wrapm_paiNumCiviliansReturnedToMe;

	if (uiVersion >= 4)
	{
		ArrayWrapper<int> wrapm_paiNumLandmarksBuiltForMe(MAX_MAJOR_CIVS, m_paiNumLandmarksBuiltForMe);
		kStream >> wrapm_paiNumLandmarksBuiltForMe;
	}
	else
	{
		for (int i = 0; i<MAX_MAJOR_CIVS; i++)
		{
			m_paiNumLandmarksBuiltForMe[i] = 0;
		}
	}

	ArrayWrapper<int> wrapm_paiResurrected(MAX_MAJOR_CIVS, m_paiResurrectedOnTurn);
	kStream >> wrapm_paiResurrected;

	ArrayWrapper<int> wrapm_paiNumTimesCultureBombed(MAX_MAJOR_CIVS, m_paiNumTimesCultureBombed);
	kStream >> wrapm_paiNumTimesCultureBombed;

	ArrayWrapper<int> wrapm_paiNegativeReligiousConversionPoints(MAX_MAJOR_CIVS, m_paiNegativeReligiousConversionPoints);
	kStream >> wrapm_paiNegativeReligiousConversionPoints;

	if (uiVersion > 1)
	{
		ArrayWrapper<int> wrapm_paiNegativeArchaeologyPoints(MAX_MAJOR_CIVS, m_paiNegativeArchaeologyPoints);
		kStream >> wrapm_paiNegativeArchaeologyPoints;
	}
	else
	{
		for (int i = 0; i<MAX_MAJOR_CIVS; i++)
		{
			m_paiNegativeArchaeologyPoints[i] = 0;
		}
	}

	ArrayWrapper<int> wrapm_paiNumTimesNuked(MAX_MAJOR_CIVS, m_paiNumTimesNuked);
	kStream >> wrapm_paiNumTimesNuked;

	ArrayWrapper<int> wrapm_paiNumTimesRobbedBy(MAX_MAJOR_CIVS, m_paiNumTimesRobbedBy);
	kStream >> wrapm_paiNumTimesRobbedBy;

	ArrayWrapper<int> wrapm_paiNumTimesIntrigueSharedBy(MAX_MAJOR_CIVS, m_paiNumTimesIntrigueSharedBy);
	kStream >> wrapm_paiNumTimesIntrigueSharedBy;

	ArrayWrapper<int> wrapm_paiBrokenExpansionPromiseValue(MAX_MAJOR_CIVS, m_paiBrokenExpansionPromiseValue);
	kStream >> wrapm_paiBrokenExpansionPromiseValue;
	ArrayWrapper<int> wrapm_paiIgnoredExpansionPromiseValue(MAX_MAJOR_CIVS, m_paiIgnoredExpansionPromiseValue);
	kStream >> wrapm_paiIgnoredExpansionPromiseValue;

	ArrayWrapper<int> wrapm_paiBrokenBorderPromiseValue(MAX_MAJOR_CIVS, m_paiBrokenBorderPromiseValue);
	kStream >> wrapm_paiBrokenBorderPromiseValue;
	ArrayWrapper<int> wrapm_paiIgnoredBorderPromiseValue(MAX_MAJOR_CIVS, m_paiIgnoredBorderPromiseValue);
	kStream >> wrapm_paiIgnoredBorderPromiseValue;

	ArrayWrapper<int> wrapm_aiDeclaredWarOnFriendValue(MAX_MAJOR_CIVS, m_paiDeclaredWarOnFriendValue);
	kStream >> wrapm_aiDeclaredWarOnFriendValue;

	ArrayWrapper<int> wrapm_paiTradeValue(MAX_MAJOR_CIVS, m_paiTradeValue);
	kStream >> wrapm_paiTradeValue;

	ArrayWrapper<int> wrapm_paiCommonFoeValue(MAX_MAJOR_CIVS, m_paiCommonFoeValue);
	kStream >> wrapm_paiCommonFoeValue;
	ArrayWrapper<int> wrapm_paiAssistValue(MAX_MAJOR_CIVS, m_paiAssistValue);
#else
	ArrayWrapper<short> wrapm_paiNumRequestsRefused(MAX_MAJOR_CIVS, m_paiNumRequestsRefused);
	kStream >> wrapm_paiNumRequestsRefused;

	ArrayWrapper<short> wrapm_paiNumCiviliansReturnedToMe(MAX_MAJOR_CIVS, m_paiNumCiviliansReturnedToMe);
	kStream >> wrapm_paiNumCiviliansReturnedToMe;

	if (uiVersion >= 4)
	{
		ArrayWrapper<short> wrapm_paiNumLandmarksBuiltForMe(MAX_MAJOR_CIVS, m_paiNumLandmarksBuiltForMe);
		kStream >> wrapm_paiNumLandmarksBuiltForMe;
	}
	else
	{
		for (int i=0;i<MAX_MAJOR_CIVS;i++)
		{
			m_paiNumLandmarksBuiltForMe[i] = 0;
		}
	}

	ArrayWrapper<short> wrapm_paiResurrected(MAX_MAJOR_CIVS, m_paiResurrectedOnTurn);
	kStream >> wrapm_paiResurrected;

	ArrayWrapper<short> wrapm_paiNumTimesCultureBombed(MAX_MAJOR_CIVS, m_paiNumTimesCultureBombed);
	kStream >> wrapm_paiNumTimesCultureBombed;

	ArrayWrapper<short> wrapm_paiNegativeReligiousConversionPoints(MAX_MAJOR_CIVS, m_paiNegativeReligiousConversionPoints);
	kStream >> wrapm_paiNegativeReligiousConversionPoints;

	if (uiVersion > 1)
	{
		ArrayWrapper<short> wrapm_paiNegativeArchaeologyPoints(MAX_MAJOR_CIVS, m_paiNegativeArchaeologyPoints);
		kStream >> wrapm_paiNegativeArchaeologyPoints;
	}
	else
	{
		for (int i=0;i<MAX_MAJOR_CIVS;i++)
		{
			m_paiNegativeArchaeologyPoints[i] = 0;
		}
	}

	ArrayWrapper<short> wrapm_paiNumTimesNuked(MAX_MAJOR_CIVS, m_paiNumTimesNuked);
	kStream >> wrapm_paiNumTimesNuked;

	ArrayWrapper<short> wrapm_paiNumTimesRobbedBy(MAX_MAJOR_CIVS, m_paiNumTimesRobbedBy);
	kStream >> wrapm_paiNumTimesRobbedBy;

	ArrayWrapper<short> wrapm_paiNumTimesIntrigueSharedBy(MAX_MAJOR_CIVS, m_paiNumTimesIntrigueSharedBy);
	kStream >> wrapm_paiNumTimesIntrigueSharedBy;

	ArrayWrapper<short> wrapm_paiBrokenExpansionPromiseValue(MAX_MAJOR_CIVS, m_paiBrokenExpansionPromiseValue);
	kStream >> wrapm_paiBrokenExpansionPromiseValue;
	ArrayWrapper<short> wrapm_paiIgnoredExpansionPromiseValue(MAX_MAJOR_CIVS, m_paiIgnoredExpansionPromiseValue);
	kStream >> wrapm_paiIgnoredExpansionPromiseValue;		

	ArrayWrapper<short> wrapm_paiBrokenBorderPromiseValue(MAX_MAJOR_CIVS, m_paiBrokenBorderPromiseValue);
	kStream >> wrapm_paiBrokenBorderPromiseValue;
	ArrayWrapper<short> wrapm_paiIgnoredBorderPromiseValue(MAX_MAJOR_CIVS, m_paiIgnoredBorderPromiseValue);
	kStream >> wrapm_paiIgnoredBorderPromiseValue;		

	ArrayWrapper<short> wrapm_aiDeclaredWarOnFriendValue(MAX_MAJOR_CIVS, m_paiDeclaredWarOnFriendValue);
	kStream >> wrapm_aiDeclaredWarOnFriendValue;

	ArrayWrapper<short> wrapm_paiTradeValue(MAX_MAJOR_CIVS, m_paiTradeValue);
	kStream >> wrapm_paiTradeValue;

	ArrayWrapper<short> wrapm_paiCommonFoeValue(MAX_MAJOR_CIVS, m_paiCommonFoeValue);
	kStream >> wrapm_paiCommonFoeValue;
	ArrayWrapper<short> wrapm_paiAssistValue(MAX_MAJOR_CIVS, m_paiAssistValue);
#endif
	kStream >> wrapm_paiAssistValue;

	ArrayWrapper<bool> wrapm_pabPlayerMadeMilitaryPromise(MAX_MAJOR_CIVS, m_pabPlayerMadeMilitaryPromise);
	kStream >> wrapm_pabPlayerMadeMilitaryPromise;

	ArrayWrapper<bool> wrapm_pabPlayerBrokenMilitaryPromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenMilitaryPromise);
	kStream >> wrapm_pabPlayerBrokenMilitaryPromise;

	ArrayWrapper<bool> wrapm_pabPlayerIgnoredMilitaryPromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredMilitaryPromise);
	kStream >> wrapm_pabPlayerIgnoredMilitaryPromise;

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapm_paiPlayerMilitaryPromiseCounter(MAX_MAJOR_CIVS, m_paiPlayerMilitaryPromiseCounter);
	kStream >> wrapm_paiPlayerMilitaryPromiseCounter;

	ArrayWrapper<int> wrapm_paiPlayerMadeExpansionPromiseTurn(MAX_MAJOR_CIVS, m_paiPlayerMadeExpansionPromiseTurn);
#else
	ArrayWrapper<short> wrapm_paiPlayerMilitaryPromiseCounter(MAX_MAJOR_CIVS, m_paiPlayerMilitaryPromiseCounter);
	kStream >> wrapm_paiPlayerMilitaryPromiseCounter;

	ArrayWrapper<short> wrapm_paiPlayerMadeExpansionPromiseTurn(MAX_MAJOR_CIVS, m_paiPlayerMadeExpansionPromiseTurn);
#endif
	kStream >> wrapm_paiPlayerMadeExpansionPromiseTurn;		

	ArrayWrapper<bool> wrapm_pabPlayerBrokenExpansionPromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenExpansionPromise);
	kStream >> wrapm_pabPlayerBrokenExpansionPromise;

	ArrayWrapper<bool> wrapm_pabPlayerIgnoredExpansionPromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredExpansionPromise);
	kStream >> wrapm_pabPlayerIgnoredExpansionPromise;

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapm_paePlayerExpansionPromiseData(MAX_MAJOR_CIVS, m_paePlayerExpansionPromiseData);
	kStream >> wrapm_paePlayerExpansionPromiseData;

	ArrayWrapper<int> wrapm_paiPlayerMadeBorderPromiseTurn(MAX_MAJOR_CIVS, m_paiPlayerMadeBorderPromiseTurn);
#else
	ArrayWrapper<char> wrapm_paePlayerExpansionPromiseData(MAX_MAJOR_CIVS, m_paePlayerExpansionPromiseData);
	kStream >> wrapm_paePlayerExpansionPromiseData;

	ArrayWrapper<short> wrapm_paiPlayerMadeBorderPromiseTurn(MAX_MAJOR_CIVS, m_paiPlayerMadeBorderPromiseTurn);
#endif
	kStream >> wrapm_paiPlayerMadeBorderPromiseTurn;

	ArrayWrapper<bool> wrapm_pabPlayerBrokenBorderPromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenBorderPromise);
	kStream >> wrapm_pabPlayerBrokenBorderPromise;

	ArrayWrapper<bool> wrapm_pabPlayerIgnoredBorderPromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredBorderPromise);
	kStream >> wrapm_pabPlayerIgnoredBorderPromise;

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapm_paePlayerBorderPromiseData(MAX_MAJOR_CIVS, m_paePlayerBorderPromiseData);
#else
	ArrayWrapper<char> wrapm_paePlayerBorderPromiseData(MAX_MAJOR_CIVS, m_paePlayerBorderPromiseData);
#endif
	kStream >> wrapm_paePlayerBorderPromiseData;

	ArrayWrapper<bool> wrapm_pabPlayerMadeAttackCityStatePromise(MAX_MAJOR_CIVS, m_pabPlayerMadeAttackCityStatePromise);
	kStream >> wrapm_pabPlayerMadeAttackCityStatePromise;

	ArrayWrapper<bool> wrapm_pabPlayerBrokenAttackCityStatePromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenAttackCityStatePromise);
	kStream >> wrapm_pabPlayerBrokenAttackCityStatePromise;

	ArrayWrapper<bool> wrapm_pabPlayerIgnoredAttackCityStatePromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredAttackCityStatePromise);
	kStream >> wrapm_pabPlayerIgnoredAttackCityStatePromise;

	ArrayWrapper<bool> wrapm_pabPlayerMadeBullyCityStatePromise(MAX_MAJOR_CIVS, m_pabPlayerMadeBullyCityStatePromise);
	kStream >> wrapm_pabPlayerMadeBullyCityStatePromise;

	ArrayWrapper<bool> wrapm_pabPlayerBrokenBullyCityStatePromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenBullyCityStatePromise);
	kStream >> wrapm_pabPlayerBrokenBullyCityStatePromise;

	ArrayWrapper<bool> wrapm_pabPlayerIgnoredBullyCityStatePromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredBullyCityStatePromise);
	kStream >> wrapm_pabPlayerIgnoredBullyCityStatePromise;

	ArrayWrapper<bool> wrapm_pabPlayerMadeSpyPromise(MAX_MAJOR_CIVS, m_pabPlayerMadeSpyPromise);
	kStream >> wrapm_pabPlayerMadeSpyPromise;

	ArrayWrapper<bool> wrapm_pabPlayerBrokenSpyPromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenSpyPromise);
	kStream >> wrapm_pabPlayerBrokenSpyPromise;

	ArrayWrapper<bool> wrapm_pabPlayerIgnoredSpyPromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredSpyPromise);
	kStream >> wrapm_pabPlayerIgnoredSpyPromise;

	ArrayWrapper<bool> wrapm_pabPlayerForgaveForSpying(MAX_MAJOR_CIVS, m_pabPlayerForgaveForSpying);
	kStream >> wrapm_pabPlayerForgaveForSpying;

	ArrayWrapper<bool> wrapm_pabPlayerMadeNoConvertPromise(MAX_MAJOR_CIVS, m_pabPlayerMadeNoConvertPromise);
	kStream >> wrapm_pabPlayerMadeNoConvertPromise;

	ArrayWrapper<bool> wrapm_pabPlayerBrokenNoConvertPromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenNoConvertPromise);
	kStream >> wrapm_pabPlayerBrokenNoConvertPromise;

	ArrayWrapper<bool> wrapm_pabPlayerIgnoredNoConvertPromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredNoConvertPromise);
	kStream >> wrapm_pabPlayerIgnoredNoConvertPromise;

	ArrayWrapper<bool> wrapm_pabPlayerAskedNotToConvert(MAX_MAJOR_CIVS, m_pabPlayerAskedNotToConvert);
	kStream >> wrapm_pabPlayerAskedNotToConvert;

	ArrayWrapper<bool> wrapm_pabPlayerAgreedNotToConvert(MAX_MAJOR_CIVS, m_pabPlayerAgreedNotToConvert);
	kStream >> wrapm_pabPlayerAgreedNotToConvert;

	if (uiVersion > 1)
	{
		ArrayWrapper<bool> wrapm_pabPlayerMadeNoDiggingPromise(MAX_MAJOR_CIVS, m_pabPlayerMadeNoDiggingPromise);
		kStream >> wrapm_pabPlayerMadeNoDiggingPromise;

		ArrayWrapper<bool> wrapm_pabPlayerBrokenNoDiggingPromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenNoDiggingPromise);
		kStream >> wrapm_pabPlayerBrokenNoDiggingPromise;

		ArrayWrapper<bool> wrapm_pabPlayerIgnoredNoDiggingPromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredNoDiggingPromise);
		kStream >> wrapm_pabPlayerIgnoredNoDiggingPromise;

		ArrayWrapper<bool> wrapm_pabPlayerAskedNotToDig(MAX_MAJOR_CIVS, m_pabPlayerAskedNotToDig);
		kStream >> wrapm_pabPlayerAskedNotToDig;

		ArrayWrapper<bool> wrapm_pabPlayerAgreedNotToDig(MAX_MAJOR_CIVS, m_pabPlayerAgreedNotToDig);
		kStream >> wrapm_pabPlayerAgreedNotToDig;
	}
	else
	{

		for (int i=0;i<MAX_MAJOR_CIVS;i++)
		{
			m_pabPlayerMadeNoDiggingPromise[i] = false;
			m_pabPlayerBrokenNoDiggingPromise[i] = false;
			m_pabPlayerIgnoredNoDiggingPromise[i] = false;
			m_pabPlayerAskedNotToDig[i] = false;
			m_pabPlayerAgreedNotToDig[i] = false;
		}
	}

	ArrayWrapper<bool> wrapm_pabPlayerBrokenCoopWarPromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenCoopWarPromise);
	kStream >> wrapm_pabPlayerBrokenCoopWarPromise;


	kStream >> m_eDemandTargetPlayer;
	kStream >> m_bDemandReady;

	kStream >> m_iVictoryCompetitiveness;
	kStream >> m_iWonderCompetitiveness;
	kStream >> m_iMinorCivCompetitiveness;
	kStream >> m_iBoldness;
	kStream >> m_iDiploBalance;
	kStream >> m_iWarmongerHate;
	kStream >> m_iDenounceWillingness;
	kStream >> m_iDoFWillingness;

	kStream >> m_iLoyalty;
	kStream >> m_iNeediness;
	kStream >> m_iForgiveness;
	kStream >> m_iChattiness;
	kStream >> m_iMeanness;

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapm_paiPersonalityMajorCivApproachBiases(NUM_MAJOR_CIV_APPROACHES, m_paiPersonalityMajorCivApproachBiases);
	kStream >> wrapm_paiPersonalityMajorCivApproachBiases;

	ArrayWrapper<int> wrapm_paiPersonalityMinorCivApproachBiases(NUM_MINOR_CIV_APPROACHES, m_paiPersonalityMinorCivApproachBiases);
	kStream >> wrapm_paiPersonalityMinorCivApproachBiases;

	ArrayWrapper<int> wrapm_paiOtherPlayerTurnsSinceAttackedProtectedMinor(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceAttackedProtectedMinor);
	kStream >> wrapm_paiOtherPlayerTurnsSinceAttackedProtectedMinor;

	ArrayWrapper<int> wrapm_paiOtherPlayerProtectedMinorAttacked(MAX_MAJOR_CIVS, m_paiOtherPlayerProtectedMinorAttacked);
	kStream >> wrapm_paiOtherPlayerProtectedMinorAttacked;

	ArrayWrapper<int> wrapm_paiOtherPlayerNumProtectedMinorsAttacked(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsAttacked);
	kStream >> wrapm_paiOtherPlayerNumProtectedMinorsAttacked;

	ArrayWrapper<int> wrapm_paiOtherPlayerTurnsSinceKilledProtectedMinor(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceKilledProtectedMinor);
	kStream >> wrapm_paiOtherPlayerTurnsSinceKilledProtectedMinor;

	ArrayWrapper<int> wrapm_paiOtherPlayerProtectedMinorKilled(MAX_MAJOR_CIVS, m_paiOtherPlayerProtectedMinorKilled);
	kStream >> wrapm_paiOtherPlayerProtectedMinorKilled;

	ArrayWrapper<int> wrapm_paiOtherPlayerNumProtectedMinorsKilled(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsKilled);
	kStream >> wrapm_paiOtherPlayerNumProtectedMinorsKilled;

	ArrayWrapper<int> wrapm_paiOtherPlayerNumProtectedMinorsBullied(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsBullied);
	kStream >> wrapm_paiOtherPlayerNumProtectedMinorsBullied;

	ArrayWrapper<int> wrapm_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor);
	kStream >> wrapm_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor;

	ArrayWrapper<int> wrapm_paiOtherPlayerNumMinorsAttacked(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMinorsAttacked);
	kStream >> wrapm_paiOtherPlayerNumMinorsAttacked;

	ArrayWrapper<int> wrapm_paiOtherPlayerNumMinorsConquered(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMinorsConquered);
	kStream >> wrapm_paiOtherPlayerNumMinorsConquered;

	ArrayWrapper<int> wrapm_paiOtherPlayerNumMajorsAttacked(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMajorsAttacked);
	kStream >> wrapm_paiOtherPlayerNumMajorsAttacked;

	ArrayWrapper<int> wrapm_paiOtherPlayerNumMajorsConquered(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMajorsConquered);
#else
	ArrayWrapper<char> wrapm_paiPersonalityMajorCivApproachBiases(NUM_MAJOR_CIV_APPROACHES, m_paiPersonalityMajorCivApproachBiases);
	kStream >> wrapm_paiPersonalityMajorCivApproachBiases;

	ArrayWrapper<char> wrapm_paiPersonalityMinorCivApproachBiases(NUM_MINOR_CIV_APPROACHES, m_paiPersonalityMinorCivApproachBiases);
	kStream >> wrapm_paiPersonalityMinorCivApproachBiases;

	ArrayWrapper<short> wrapm_paiOtherPlayerTurnsSinceAttackedProtectedMinor(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceAttackedProtectedMinor);
	kStream >> wrapm_paiOtherPlayerTurnsSinceAttackedProtectedMinor;

	ArrayWrapper<char> wrapm_paiOtherPlayerProtectedMinorAttacked(MAX_MAJOR_CIVS, m_paiOtherPlayerProtectedMinorAttacked);
	kStream >> wrapm_paiOtherPlayerProtectedMinorAttacked;

	ArrayWrapper<char> wrapm_paiOtherPlayerNumProtectedMinorsAttacked(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsAttacked);
	kStream >> wrapm_paiOtherPlayerNumProtectedMinorsAttacked;

	ArrayWrapper<short> wrapm_paiOtherPlayerTurnsSinceKilledProtectedMinor(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceKilledProtectedMinor);
	kStream >> wrapm_paiOtherPlayerTurnsSinceKilledProtectedMinor;

	ArrayWrapper<char> wrapm_paiOtherPlayerProtectedMinorKilled(MAX_MAJOR_CIVS, m_paiOtherPlayerProtectedMinorKilled);
	kStream >> wrapm_paiOtherPlayerProtectedMinorKilled;

	ArrayWrapper<char> wrapm_paiOtherPlayerNumProtectedMinorsKilled(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsKilled);
	kStream >> wrapm_paiOtherPlayerNumProtectedMinorsKilled;

	ArrayWrapper<char> wrapm_paiOtherPlayerNumProtectedMinorsBullied(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsBullied);
	kStream >> wrapm_paiOtherPlayerNumProtectedMinorsBullied;

	ArrayWrapper<short> wrapm_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor);
	kStream >> wrapm_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor;

	ArrayWrapper<char> wrapm_paiOtherPlayerNumMinorsAttacked(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMinorsAttacked);
	kStream >> wrapm_paiOtherPlayerNumMinorsAttacked;

	ArrayWrapper<char> wrapm_paiOtherPlayerNumMinorsConquered(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMinorsConquered);
	kStream >> wrapm_paiOtherPlayerNumMinorsConquered;

	ArrayWrapper<char> wrapm_paiOtherPlayerNumMajorsAttacked(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMajorsAttacked);
	kStream >> wrapm_paiOtherPlayerNumMajorsAttacked;

	ArrayWrapper<char> wrapm_paiOtherPlayerNumMajorsConquered(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMajorsConquered);
#endif
	kStream >> wrapm_paiOtherPlayerNumMajorsConquered;

	ArrayWrapper<int> wrapm_paiOtherPlayerWarmongerAmount(MAX_MAJOR_CIVS, m_paiOtherPlayerWarmongerAmount);
	kStream >> wrapm_paiOtherPlayerWarmongerAmount;

	if (uiVersion >= 3)
	{
#ifdef AUI_WARNING_FIXES
		ArrayWrapper<int> wrapm_paiOtherPlayerTurnsSinceWeLikedTheirProposal(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceWeLikedTheirProposal);
		kStream >> wrapm_paiOtherPlayerTurnsSinceWeLikedTheirProposal;

		ArrayWrapper<int> wrapm_paiOtherPlayerTurnsSinceWeDislikedTheirProposal(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceWeDislikedTheirProposal);
		kStream >> wrapm_paiOtherPlayerTurnsSinceWeDislikedTheirProposal;

		ArrayWrapper<int> wrapm_paiOtherPlayerTurnsSinceTheySupportedOurProposal(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceTheySupportedOurProposal);
		kStream >> wrapm_paiOtherPlayerTurnsSinceTheySupportedOurProposal;

		ArrayWrapper<int> wrapm_paiOtherPlayerTurnsSinceTheyFoiledOurProposal(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceTheyFoiledOurProposal);
		kStream >> wrapm_paiOtherPlayerTurnsSinceTheyFoiledOurProposal;

		ArrayWrapper<int> wrapm_paiOtherPlayerTurnsSinceTheySupportedOurHosting(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceTheySupportedOurHosting);
#else
		ArrayWrapper<short> wrapm_paiOtherPlayerTurnsSinceWeLikedTheirProposal(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceWeLikedTheirProposal);
		kStream >> wrapm_paiOtherPlayerTurnsSinceWeLikedTheirProposal;

		ArrayWrapper<short> wrapm_paiOtherPlayerTurnsSinceWeDislikedTheirProposal(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceWeDislikedTheirProposal);
		kStream >> wrapm_paiOtherPlayerTurnsSinceWeDislikedTheirProposal;

		ArrayWrapper<short> wrapm_paiOtherPlayerTurnsSinceTheySupportedOurProposal(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceTheySupportedOurProposal);
		kStream >> wrapm_paiOtherPlayerTurnsSinceTheySupportedOurProposal;

		ArrayWrapper<short> wrapm_paiOtherPlayerTurnsSinceTheyFoiledOurProposal(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceTheyFoiledOurProposal);
		kStream >> wrapm_paiOtherPlayerTurnsSinceTheyFoiledOurProposal;

		ArrayWrapper<short> wrapm_paiOtherPlayerTurnsSinceTheySupportedOurHosting(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceTheySupportedOurHosting);
#endif
		kStream >> wrapm_paiOtherPlayerTurnsSinceTheySupportedOurHosting;
	}
	else
	{
		for (uint i = 0; i < MAX_MAJOR_CIVS; i++)
		{
			m_paiOtherPlayerTurnsSinceWeLikedTheirProposal[i] = -1;
			m_paiOtherPlayerTurnsSinceWeDislikedTheirProposal[i] = -1;
			m_paiOtherPlayerTurnsSinceTheySupportedOurProposal[i] = -1;
			m_paiOtherPlayerTurnsSinceTheyFoiledOurProposal[i] = -1;
			m_paiOtherPlayerTurnsSinceTheySupportedOurHosting[i] = -1;
		}
	}

#ifdef AUI_WARNING_FIXES
	ArrayWrapper<int> wrapm_paiNumCitiesLiberated(MAX_MAJOR_CIVS, m_paiNumCitiesLiberated);
#else
	ArrayWrapper<short> wrapm_paiNumCitiesLiberated(MAX_MAJOR_CIVS, m_paiNumCitiesLiberated);
#endif
	kStream >> wrapm_paiNumCitiesLiberated;

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		ArrayWrapper<DiploLogData> wrapm_ppaDiploStatementsLog(MAX_DIPLO_LOG_STATEMENTS, m_ppaDiploStatementsLog[iI]);
		kStream >> wrapm_ppaDiploStatementsLog;
	}

	ArrayWrapper<DeclarationLogData> wrapm_paDeclarationsLog(MAX_DIPLO_LOG_STATEMENTS, m_paDeclarationsLog);
	kStream >> wrapm_paDeclarationsLog;
	kStream >> m_eStateAllWars;
}


void CvDiplomacyAI::Write(FDataStream& kStream) const
{

	uint uiVersion = 4;
	kStream << uiVersion;

	int iI;

#ifdef AUI_WARNING_FIXES
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paeMajorCivOpinion);
#else
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paeMajorCivOpinion);
#endif

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
#ifdef AUI_WARNING_FIXES
		kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_ppaaeOtherPlayerMajorCivOpinion[iI]);
		kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_ppaabWorkingAgainstPlayerAccepted[iI]);
		kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_ppaaiWorkingAgainstPlayerCounter[iI]);
		kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_ppaacCoopWarAcceptedState[iI]);
		kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_ppaaiCoopWarCounter[iI]);
	}

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paeMajorCivApproach);
	kStream << ArrayWrapper<int>(MAX_MINOR_CIVS, m_paeMinorCivApproach);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paeApproachTowardsUsGuess);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paeApproachTowardsUsGuessCounter);

	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paiNumWondersBeatenTo);
#else
		kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_ppaaeOtherPlayerMajorCivOpinion[iI]);
		kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_ppaabWorkingAgainstPlayerAccepted[iI]);
		kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_ppaaiWorkingAgainstPlayerCounter[iI]);
		kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_ppaacCoopWarAcceptedState[iI]);
		kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_ppaaiCoopWarCounter[iI]);
	}

	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paeMajorCivApproach);
	kStream << ArrayWrapper<char>(MAX_MINOR_CIVS, m_paeMinorCivApproach);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paeApproachTowardsUsGuess);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paeApproachTowardsUsGuessCounter);

	kStream << ArrayWrapper<short>(MAX_CIV_PLAYERS, m_paiNumWondersBeatenTo);
#endif
	kStream << ArrayWrapper<bool>(MAX_CIV_PLAYERS, m_pabMusteringForAttack);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabWantsResearchAgreementWithPlayer);
	kStream << ArrayWrapper<bool>(MAX_MINOR_CIVS, m_pabWantToRouteToMinor);

#ifdef AUI_WARNING_FIXES
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeWantPeaceCounter);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paePeaceTreatyWillingToOffer);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paePeaceTreatyWillingToAccept);

	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeWarFace);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeWarState);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeWarProjection);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastWarProjection);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeWarGoal);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paiPlayerNumTurnsAtWar);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paiNumWarsFought);

	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paePlayerMilitaryStrengthComparedToUs);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paePlayerEconomicStrengthComparedToUs);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paePlayerTargetValue);

	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paePlayerLandDisputeLevel);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paePlayerLastTurnLandDisputeLevel);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paePlayerVictoryDisputeLevel);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paePlayerWonderDisputeLevel);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paePlayerMinorCivDisputeLevel);

	for (iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerLandDisputeLevel[iI]);
		kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerVictoryDisputeLevel[iI]);

		kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerWarDamageLevel[iI]);
		kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_ppaaiOtherPlayerWarValueLost[iI]);

		kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerMilitaryThreat[iI]);
#else
	kStream << ArrayWrapper<short>(MAX_CIV_PLAYERS, m_paeWantPeaceCounter);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paePeaceTreatyWillingToOffer);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paePeaceTreatyWillingToAccept);

	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeWarFace);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeWarState);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeWarProjection);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeLastWarProjection);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeWarGoal);
	kStream << ArrayWrapper<short>(MAX_CIV_PLAYERS, m_paiPlayerNumTurnsAtWar);
	kStream << ArrayWrapper<short>(MAX_CIV_PLAYERS, m_paiNumWarsFought);

	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerMilitaryStrengthComparedToUs);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerEconomicStrengthComparedToUs);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerTargetValue);

	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerLandDisputeLevel);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerLastTurnLandDisputeLevel);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerVictoryDisputeLevel);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerWonderDisputeLevel);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerMinorCivDisputeLevel);

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerLandDisputeLevel[iI]);
		kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerVictoryDisputeLevel[iI]);

		kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerWarDamageLevel[iI]);
		kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_ppaaiOtherPlayerWarValueLost[iI]);

		kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerMilitaryThreat[iI]);
#endif
	}


	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_ppaaiOtherPlayerLastRoundWarValueLost[iI]);
		kStream << ArrayWrapper<bool>(MAX_CIV_PLAYERS, m_ppaabSentAttackMessageToMinorCivProtector[iI]);
	}

#ifdef AUI_WARNING_FIXES
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeWarDamageLevel);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paiWarValueLost);

	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeMilitaryAggressivePosture);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryAggressivePosture);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeExpansionAggressivePosture);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paePlotBuyingAggressivePosture);

	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeMilitaryThreat);

	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeWarmongerThreat);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerNoSettleRequestAccepted);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiPlayerNoSettleRequestCounter);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerStopSpyingRequestAccepted);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiPlayerStopSpyingRequestCounter);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiDemandCounter);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiDemandTooSoonNumTurns);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabDoFAccepted);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiDoFCounter);
#else
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeWarDamageLevel);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paiWarValueLost);

	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeMilitaryAggressivePosture);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryAggressivePosture);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeExpansionAggressivePosture);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlotBuyingAggressivePosture);

	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeMilitaryThreat);

	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeWarmongerThreat);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerNoSettleRequestAccepted);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiPlayerNoSettleRequestCounter);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerStopSpyingRequestAccepted);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiPlayerStopSpyingRequestCounter);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiDemandCounter);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiDemandTooSoonNumTurns);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabDoFAccepted);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiDoFCounter);
#endif

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabDenouncedPlayer);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabFriendDenouncedUs);
#ifdef AUI_WARNING_FIXES
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiDenouncedPlayerCounter);
#else
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiDenouncedPlayerCounter);
#endif
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabFriendDeclaredWarOnUs);

#ifdef AUI_WARNING_FIXES
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiNumRequestsRefused);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiNumCiviliansReturnedToMe);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiNumLandmarksBuiltForMe);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiResurrectedOnTurn);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiNumTimesCultureBombed);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiNegativeReligiousConversionPoints);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiNegativeArchaeologyPoints);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiNumTimesNuked);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiNumTimesRobbedBy);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiNumTimesIntrigueSharedBy);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiBrokenExpansionPromiseValue);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiIgnoredExpansionPromiseValue);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiBrokenBorderPromiseValue);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiIgnoredBorderPromiseValue);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiDeclaredWarOnFriendValue);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiTradeValue);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiCommonFoeValue);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiAssistValue);
#else
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumRequestsRefused);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumCiviliansReturnedToMe);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumLandmarksBuiltForMe);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiResurrectedOnTurn);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumTimesCultureBombed);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNegativeReligiousConversionPoints);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNegativeArchaeologyPoints);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumTimesNuked);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumTimesRobbedBy);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumTimesIntrigueSharedBy);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiBrokenExpansionPromiseValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiIgnoredExpansionPromiseValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiBrokenBorderPromiseValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiIgnoredBorderPromiseValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiDeclaredWarOnFriendValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiTradeValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiCommonFoeValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiAssistValue);
#endif

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerMadeMilitaryPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenMilitaryPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredMilitaryPromise);
#ifdef AUI_WARNING_FIXES
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiPlayerMilitaryPromiseCounter);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiPlayerMadeExpansionPromiseTurn);
#else
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiPlayerMilitaryPromiseCounter);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiPlayerMadeExpansionPromiseTurn);
#endif
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenExpansionPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredExpansionPromise);
#ifdef AUI_WARNING_FIXES
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paePlayerExpansionPromiseData);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiPlayerMadeBorderPromiseTurn);
#else
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paePlayerExpansionPromiseData);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiPlayerMadeBorderPromiseTurn);
#endif
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenBorderPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredBorderPromise);
#ifdef AUI_WARNING_FIXES
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paePlayerBorderPromiseData);
#else
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paePlayerBorderPromiseData);
#endif

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerMadeAttackCityStatePromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenAttackCityStatePromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredAttackCityStatePromise);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerMadeBullyCityStatePromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenBullyCityStatePromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredBullyCityStatePromise);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerMadeSpyPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenSpyPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredSpyPromise);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerForgaveForSpying);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerMadeNoConvertPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenNoConvertPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredNoConvertPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerAskedNotToConvert);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerAgreedNotToConvert);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerMadeNoDiggingPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenNoDiggingPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredNoDiggingPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerAskedNotToDig);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerAgreedNotToDig);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenCoopWarPromise);

	kStream << m_eDemandTargetPlayer;
	kStream << m_bDemandReady;

	kStream << m_iVictoryCompetitiveness;
	kStream << m_iWonderCompetitiveness;
	kStream << m_iMinorCivCompetitiveness;
	kStream << m_iBoldness;
	kStream << m_iDiploBalance;
	kStream << m_iWarmongerHate;
	kStream << m_iDenounceWillingness;
	kStream << m_iDoFWillingness;
	kStream << m_iLoyalty;
	kStream << m_iNeediness;
	kStream << m_iForgiveness;
	kStream << m_iChattiness;
	kStream << m_iMeanness;

#ifdef AUI_WARNING_FIXES
	kStream << ArrayWrapper<int>(NUM_MAJOR_CIV_APPROACHES, m_paiPersonalityMajorCivApproachBiases);
	kStream << ArrayWrapper<int>(NUM_MINOR_CIV_APPROACHES, m_paiPersonalityMinorCivApproachBiases);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceAttackedProtectedMinor);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerProtectedMinorAttacked);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsAttacked);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceKilledProtectedMinor);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerProtectedMinorKilled);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsKilled);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsBullied);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMinorsAttacked);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMinorsConquered);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMajorsAttacked);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMajorsConquered);
#else
	kStream << ArrayWrapper<char>(NUM_MAJOR_CIV_APPROACHES, m_paiPersonalityMajorCivApproachBiases);
	kStream << ArrayWrapper<char>(NUM_MINOR_CIV_APPROACHES, m_paiPersonalityMinorCivApproachBiases);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceAttackedProtectedMinor);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerProtectedMinorAttacked);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsAttacked);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceKilledProtectedMinor);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerProtectedMinorKilled);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsKilled);

	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsBullied);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor);

	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMinorsAttacked);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMinorsConquered);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMajorsAttacked);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMajorsConquered);
#endif

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerWarmongerAmount);

#ifdef AUI_WARNING_FIXES
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceWeLikedTheirProposal);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceWeDislikedTheirProposal);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceTheySupportedOurProposal);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceTheyFoiledOurProposal);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceTheySupportedOurHosting);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiNumCitiesLiberated);
#else
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceWeLikedTheirProposal);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceWeDislikedTheirProposal);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceTheySupportedOurProposal);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceTheyFoiledOurProposal);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceTheySupportedOurHosting);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumCitiesLiberated);
#endif

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		kStream << ArrayWrapper<DiploLogData>(MAX_DIPLO_LOG_STATEMENTS, m_ppaDiploStatementsLog[iI]);
	}

	kStream << ArrayWrapper<DeclarationLogData>(MAX_DIPLO_LOG_STATEMENTS, m_paDeclarationsLog);
	kStream << m_eStateAllWars;
}


void CvDiplomacyAI::update()
{
	if(!m_aGreetPlayers.empty())
	{
		PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();

		PlayerTypesArray::iterator itr = std::find(m_aGreetPlayers.begin(), m_aGreetPlayers.end(), eActivePlayer);
		if(itr != m_aGreetPlayers.end())
		{
			m_aGreetPlayers.erase(itr);

			const char* szText = GetDiploStringForMessage(DIPLO_MESSAGE_INTRO);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), eActivePlayer, DIPLO_UI_STATE_DEFAULT_ROOT, szText, LEADERHEAD_ANIM_INTRO);
		}
	}
}


CvPlayer* CvDiplomacyAI::GetPlayer()
{
	return m_pPlayer;
}


TeamTypes CvDiplomacyAI::GetTeam()
{
	return m_pPlayer->getTeam();
}



void CvDiplomacyAI::DoInitializePersonality()
{

	if(!GetPlayer()->isHuman())
	{
		CvLeaderHeadInfo& playerLeaderInfo = GetPlayer()->getLeaderInfo();

		m_iVictoryCompetitiveness = GetRandomPersonalityWeight(playerLeaderInfo.GetVictoryCompetitiveness());
		CvAssertMsg(m_iVictoryCompetitiveness >= 0, "DIPLOMACY_AI: Victory Competitiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iWonderCompetitiveness = GetRandomPersonalityWeight(playerLeaderInfo.GetWonderCompetitiveness());
		CvAssertMsg(m_iWonderCompetitiveness >= 0, "DIPLOMACY_AI: Victory Competitiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iMinorCivCompetitiveness = GetRandomPersonalityWeight(playerLeaderInfo.GetMinorCivCompetitiveness());
		CvAssertMsg(m_iMinorCivCompetitiveness >= 0, "DIPLOMACY_AI: Minor Civ Competitiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iBoldness = GetRandomPersonalityWeight(playerLeaderInfo.GetBoldness());
		CvAssertMsg(m_iBoldness >= 0, "DIPLOMACY_AI: Boldness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iDiploBalance = GetRandomPersonalityWeight(playerLeaderInfo.GetDiploBalance());
		CvAssertMsg(m_iDiploBalance >= 0, "DIPLOMACY_AI: DiploBalance Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iWarmongerHate = GetRandomPersonalityWeight(playerLeaderInfo.GetWarmongerHate());
		CvAssertMsg(m_iWarmongerHate >= 0, "DIPLOMACY_AI: WarmongerHate Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iDenounceWillingness = GetRandomPersonalityWeight(playerLeaderInfo.GetDenounceWillingness());
		CvAssertMsg(m_iDenounceWillingness >= 0, "DIPLOMACY_AI: DenounceWillingness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iDoFWillingness = GetRandomPersonalityWeight(playerLeaderInfo.GetDoFWillingness());
		CvAssertMsg(m_iDoFWillingness >= 0, "DIPLOMACY_AI: DoFWillingness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iLoyalty = GetRandomPersonalityWeight(playerLeaderInfo.GetLoyalty());
		CvAssertMsg(m_iLoyalty >= 0, "DIPLOMACY_AI: Loyalty Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iNeediness = GetRandomPersonalityWeight(playerLeaderInfo.GetNeediness());
		CvAssertMsg(m_iNeediness >= 0, "DIPLOMACY_AI: Neediness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iForgiveness = GetRandomPersonalityWeight(playerLeaderInfo.GetForgiveness());
		CvAssertMsg(m_iForgiveness >= 0, "DIPLOMACY_AI: Forgiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iChattiness = GetRandomPersonalityWeight(playerLeaderInfo.GetChattiness());
		CvAssertMsg(m_iChattiness >= 0, "DIPLOMACY_AI: Chattiness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iMeanness = GetRandomPersonalityWeight(playerLeaderInfo.GetMeanness());
		CvAssertMsg(m_iMeanness >= 0, "DIPLOMACY_AI: Meanness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		for(int iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
		{
			m_paiPersonalityMajorCivApproachBiases[iApproachLoop] = GetRandomPersonalityWeight(playerLeaderInfo.GetMajorCivApproachBias(iApproachLoop));
			CvAssertMsg(m_paiPersonalityMajorCivApproachBiases[iApproachLoop] >= 0, "DIPLOMACY_AI: Personality Approach weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
		}

		for(int iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
		{
			m_paiPersonalityMinorCivApproachBiases[iApproachLoop] = GetRandomPersonalityWeight(playerLeaderInfo.GetMinorCivApproachBias(iApproachLoop));
			CvAssertMsg(m_paiPersonalityMinorCivApproachBiases[iApproachLoop] >= 0, "DIPLOMACY_AI: Personality Approach weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
		}
	}

	else
	{
		int iDefaultFlavorValue = GC.getDEFAULT_FLAVOR_VALUE();

		m_iVictoryCompetitiveness = iDefaultFlavorValue;
		CvAssertMsg(m_iVictoryCompetitiveness >= 0, "DIPLOMACY_AI: Victory Competitiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iWonderCompetitiveness = iDefaultFlavorValue;
		CvAssertMsg(m_iWonderCompetitiveness >= 0, "DIPLOMACY_AI: Wonder Competitiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iMinorCivCompetitiveness = iDefaultFlavorValue;
		CvAssertMsg(m_iMinorCivCompetitiveness >= 0, "DIPLOMACY_AI: Minor Civ Competitiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iBoldness = iDefaultFlavorValue;
		CvAssertMsg(m_iBoldness >= 0, "DIPLOMACY_AI: Boldness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iDiploBalance = iDefaultFlavorValue;
		CvAssertMsg(m_iDiploBalance >= 0, "DIPLOMACY_AI: DiploBalance Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iWarmongerHate = iDefaultFlavorValue;
		CvAssertMsg(m_iWarmongerHate >= 0, "DIPLOMACY_AI: WarmongerHate Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iDenounceWillingness = iDefaultFlavorValue;
		CvAssertMsg(m_iDenounceWillingness >= 0, "DIPLOMACY_AI: DenounceWillingness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iDoFWillingness = iDefaultFlavorValue;
		CvAssertMsg(m_iDoFWillingness >= 0, "DIPLOMACY_AI: DoFWillingness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iLoyalty = iDefaultFlavorValue;
		CvAssertMsg(m_iLoyalty >= 0, "DIPLOMACY_AI: Loyalty Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iNeediness = iDefaultFlavorValue;
		CvAssertMsg(m_iNeediness >= 0, "DIPLOMACY_AI: Neediness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iForgiveness = iDefaultFlavorValue;
		CvAssertMsg(m_iForgiveness >= 0, "DIPLOMACY_AI: Forgiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iChattiness = iDefaultFlavorValue;
		CvAssertMsg(m_iChattiness >= 0, "DIPLOMACY_AI: Chattiness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iMeanness = iDefaultFlavorValue;
		CvAssertMsg(m_iMeanness >= 0, "DIPLOMACY_AI: Meanness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		for(int iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
		{
			m_paiPersonalityMajorCivApproachBiases[iApproachLoop] = iDefaultFlavorValue;
			CvAssertMsg(m_paiPersonalityMajorCivApproachBiases[iApproachLoop] >= 0, "DIPLOMACY_AI: Personality Approach weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
		}

		for(int iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
		{
			m_paiPersonalityMinorCivApproachBiases[iApproachLoop] = iDefaultFlavorValue;
			CvAssertMsg(m_paiPersonalityMinorCivApproachBiases[iApproachLoop] >= 0, "DIPLOMACY_AI: Personality Approach weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
		}
	}

	LogPersonality();
}


int CvDiplomacyAI::GetRandomPersonalityWeight(int iOriginalValue) const
{
	int iMin =       GC.getPERSONALITY_FLAVOR_MIN_VALUE();
	int iMax =        GC.getPERSONALITY_FLAVOR_MAX_VALUE();
	int iPlusMinus =       GC.getFLAVOR_RANDOMIZATION_RANGE();

#ifdef AUI_DIPLOMACY_GET_RANDOM_PERSONALITY_WEIGHT_USES_BINOM_RNG
	int iAdjust = GC.getGame().getJonRandNumBinom(iPlusMinus * 2 + 1, "Diplomacy AI Random Weight");
#else
	int iAdjust = GC.getGame().getJonRandNum((iPlusMinus * 2 + 1), "Diplomacy AI Random Weight");
#endif
	int iRtnValue = iOriginalValue + iAdjust - iPlusMinus;

#ifdef AUI_DIPLOMACY_GET_RANDOM_PERSONALITY_WEIGHT_USE_REROLLS
	int iRerolls = 0;
	if (iRtnValue < iMin)
	{
		iRerolls = iMin - iRtnValue;
		iRtnValue = iMin;
	}
	else if (iRtnValue > iMax)
	{
		iRerolls = iRtnValue - iMax;
		iRtnValue = iMax;
	}

	while (iRerolls != 0)
	{
#ifdef AUI_DIPLOMACY_GET_RANDOM_PERSONALITY_WEIGHT_USES_BINOM_RNG
		iAdjust = GC.getGame().getJonRandNumBinom(2 * iRerolls + 1, "Adjusting Personality Flavor") - iRerolls;
#else
		iAdjust = GC.getGame().getJonRandNum(2 * iRerolls + 1, "Adjusting Personality Flavor") - iRerolls;
#endif
		iRtnValue += iAdjust;

		iRerolls = 0;
		if (iRtnValue < iMin)
		{
			iRerolls = iMin - iRtnValue;
			iRtnValue = iMin;
		}
		else if (iRtnValue > iMax)
		{
			iRerolls = iRtnValue - iMax;
			iRtnValue = iMax;
		}
	}
#else
	if(iRtnValue < iMin)
		iRtnValue = iMin;
	else if(iRtnValue > iMax)
		iRtnValue = iMax;
#endif

	return iRtnValue;
}










void CvDiplomacyAI::DoTurn(PlayerTypes eTargetPlayer)
{
	m_eTargetPlayer = eTargetPlayer;

	DoWarDamageDecay();
	DoUpdateWarDamageLevel();
	DoUpdatePlayerMilitaryStrengths();
	DoUpdatePlayerEconomicStrengths();

	DoUpdateWarmongerThreats();
	DoUpdateMilitaryThreats();
	DoUpdatePlayerTargetValues();
	DoUpdateWarStates();
	DoUpdateWarProjections();
	DoUpdateWarGoals();

	DoUpdatePeaceTreatyWillingness();


	DoUpdateLandDisputeLevels();
	DoUpdateVictoryDisputeLevels();
	DoUpdateWonderDisputeLevels();
	DoUpdateMinorCivDisputeLevels();


	DoTestPromises();


	DoUpdateOtherPlayerWarDamageLevel();
	DoUpdateEstimateOtherPlayerLandDisputeLevels();
	DoUpdateEstimateOtherPlayerVictoryDisputeLevels();
	DoUpdateEstimateOtherPlayerMilitaryThreats();
	DoEstimateOtherPlayerOpinions();
	LogOtherPlayerGuessStatus();


	DoUpdateMilitaryAggressivePostures();
	DoUpdateExpansionAggressivePostures();
	DoUpdatePlotBuyingAggressivePosture();


	DoUpdateApproachTowardsUsGuesses();

	DoUpdateOpinions();
	DoUpdateMajorCivApproaches();
	DoUpdateMinorCivApproaches();


	if(!GetPlayer()->isHuman())
	{
		MakeWar();
		DoMakePeaceWithMinors();

		DoUpdateDemands();

		DoUpdatePlanningExchanges();
		DoContactMinorCivs();
		DoContactMajorCivs();
		GC.getGame().GetGameDeals()->DoCancelAllProposedDealsWithPlayer(GetPlayer()->GetID());




	}


	DoCounters();


	CacheOtherPlayerWarValuesLost();

	LogStatus();
	LogWarStatus();
	LogStatements();

	m_eTargetPlayer = NO_PLAYER;
}


void CvDiplomacyAI::DoCounters()
{
	int iItem;
	DiploStatementTypes eStatement;
	PublicDeclarationTypes eDeclaration;

	PlayerTypes eThirdPlayer;
	int iThirdPlayerLoop;


	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{

			if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				ChangePlayerNumTurnsAtWar(eLoopPlayer, 1);

			else if(GetPlayerNumTurnsAtWar(eLoopPlayer) > 0)
				SetPlayerNumTurnsAtWar(eLoopPlayer, 0);




			if(!GET_PLAYER(eLoopPlayer).isMinorCiv())
			{

				ChangeRecentTradeValue(eLoopPlayer, -GC.getDEAL_VALUE_PER_TURN_DECAY());
				ChangeCommonFoeValue(eLoopPlayer, -GC.getCOMMON_FOE_VALUE_PER_TURN_DECAY());
				if (GetRecentAssistValue(eLoopPlayer) > 0)
				{
					int iMin = MIN(GetRecentAssistValue(eLoopPlayer), GC.getDEAL_VALUE_PER_TURN_DECAY());
					ChangeRecentAssistValue(eLoopPlayer, -iMin);
				}
				else if (GetRecentAssistValue(eLoopPlayer) < 0)
				{
					int iMin = MIN(-GetRecentAssistValue(eLoopPlayer), GC.getDEAL_VALUE_PER_TURN_DECAY());
					ChangeRecentAssistValue(eLoopPlayer, iMin);
				}
				
				int iBrokenPromisePreValue = GetBrokenExpansionPromiseValue(eLoopPlayer);
				ChangeBrokenExpansionPromiseValue(eLoopPlayer, -GC.getEXPANSION_PROMISE_BROKEN_PER_TURN_DECAY());
				int iIgnoredPromisePreValue = GetIgnoredExpansionPromiseValue(eLoopPlayer);
				ChangeIgnoredExpansionPromiseValue(eLoopPlayer, -GC.getEXPANSION_PROMISE_IGNORED_PER_TURN_DECAY());


				if ((iBrokenPromisePreValue != 0 && GetBrokenExpansionPromiseValue(eLoopPlayer) == 0 && GetIgnoredExpansionPromiseValue(eLoopPlayer) == 0) || 
					(iIgnoredPromisePreValue != 0 && GetIgnoredExpansionPromiseValue(eLoopPlayer) == 0 && GetBrokenExpansionPromiseValue(eLoopPlayer) == 0))
				{
					SetPlayerMadeExpansionPromise(eLoopPlayer, false);
					SetPlayerBrokenExpansionPromise(eLoopPlayer, false);
					SetPlayerIgnoredExpansionPromise(eLoopPlayer, false);
				}

				iBrokenPromisePreValue = GetBrokenBorderPromiseValue(eLoopPlayer);
				ChangeBrokenBorderPromiseValue(eLoopPlayer, -GC.getBORDER_PROMISE_BROKEN_PER_TURN_DECAY());
				iIgnoredPromisePreValue = GetIgnoredBorderPromiseValue(eLoopPlayer);
				ChangeIgnoredBorderPromiseValue(eLoopPlayer, -GC.getBORDER_PROMISE_IGNORED_PER_TURN_DECAY());


				if ((iBrokenPromisePreValue != 0 && GetBrokenBorderPromiseValue(eLoopPlayer) == 0 && GetIgnoredBorderPromiseValue(eLoopPlayer) == 0) || 
					(iIgnoredPromisePreValue != 0 && GetIgnoredBorderPromiseValue(eLoopPlayer) == 0 && GetBrokenBorderPromiseValue(eLoopPlayer) == 0))
				{
					SetPlayerMadeBorderPromise(eLoopPlayer, false);
					SetPlayerBrokenBorderPromise(eLoopPlayer, false);
					SetPlayerIgnoredBorderPromise(eLoopPlayer, false);
				}

				ChangeDeclaredWarOnFriendValue(eLoopPlayer, -GC.getDECLARED_WAR_ON_FRIEND_PER_TURN_DECAY());


				for(iItem = 0; iItem < MAX_DIPLO_LOG_STATEMENTS; iItem++)
				{
					eStatement = GetDiploLogStatementTypeForIndex(eLoopPlayer, iItem);

					if(eStatement != NO_DIPLO_STATEMENT_TYPE)
						ChangeDiploLogStatementTurnForIndex(eLoopPlayer, iItem, 1);
					else
						SetDiploLogStatementTurnForIndex(eLoopPlayer, iItem, 0);
				}


				if(GetOtherPlayerProtectedMinorAttacked(eLoopPlayer) != NO_PLAYER)
					ChangeOtherPlayerTurnsSinceAttackedProtectedMinor(eLoopPlayer, 1);


				if(GetOtherPlayerProtectedMinorKilled(eLoopPlayer) != NO_PLAYER)
					ChangeOtherPlayerTurnsSinceKilledProtectedMinor(eLoopPlayer, 1);


				if(IsOtherPlayerSidedWithProtectedMinor(eLoopPlayer))
					ChangeOtherPlayerTurnsSinceSidedWithProtectedMinor(eLoopPlayer, 1);


				if(GetPlayerNoSettleRequestCounter(eLoopPlayer) > -1)
				{
					ChangePlayerNoSettleRequestCounter(eLoopPlayer, 1);

					if(GetPlayerNoSettleRequestCounter(eLoopPlayer) >=        GC.getIC_MEMORY_TURN_EXPIRATION())
					{
						SetPlayerNoSettleRequestAccepted(eLoopPlayer, false);
						SetPlayerNoSettleRequestCounter(eLoopPlayer, -666);
					}
				}


				if(GetPlayerStopSpyingRequestCounter(eLoopPlayer) > -1)
				{
					ChangePlayerStopSpyingRequestCounter(eLoopPlayer, 1);

					if(GetPlayerStopSpyingRequestCounter(eLoopPlayer) >=        GC.getSTOP_SPYING_MEMORY_TURN_EXPIRATION())
					{
						SetPlayerStopSpyingRequestAccepted(eLoopPlayer, false);
						SetPlayerStopSpyingRequestCounter(eLoopPlayer, -666);
					}
				}


				if (GetTurnsSinceWeLikedTheirProposal(eLoopPlayer) > -1)
				{
					ChangeTurnsSinceWeLikedTheirProposal(eLoopPlayer, 1);
				}
				if (GetTurnsSinceWeDislikedTheirProposal(eLoopPlayer) > -1)
				{
					ChangeTurnsSinceWeDislikedTheirProposal(eLoopPlayer, 1);
				}
				if (GetTurnsSinceTheySupportedOurProposal(eLoopPlayer) > -1)
				{
					ChangeTurnsSinceTheySupportedOurProposal(eLoopPlayer, 1);
				}
				if (GetTurnsSinceTheyFoiledOurProposal(eLoopPlayer) > -1)
				{
					ChangeTurnsSinceTheyFoiledOurProposal(eLoopPlayer, 1);
				}
				if (GetTurnsSinceTheySupportedOurHosting(eLoopPlayer) > -1)
				{
					ChangeTurnsSinceTheySupportedOurHosting(eLoopPlayer, 1);
				}


				if(GetDemandCounter(eLoopPlayer) > -1)
					ChangeDemandCounter(eLoopPlayer, 1);


				if(GetDoFCounter(eLoopPlayer) > -1)
					ChangeDoFCounter(eLoopPlayer, 1);


				if(GetDenouncedPlayerCounter(eLoopPlayer) > -1)
					ChangeDenouncedPlayerCounter(eLoopPlayer, 1);


				if(IsDenouncedPlayer(eLoopPlayer) && GetDenouncedPlayerCounter(eLoopPlayer) >= GC.getGame().getGameSpeedInfo().getRelationshipDuration())
				{
					SetDenouncedPlayer(eLoopPlayer, false);
					SetDenouncedPlayerCounter(eLoopPlayer, -1);

					SetDoFCounter(eLoopPlayer, -1);


					GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetDoFCounter(GetPlayer()->GetID(), -1);
					GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetFriendDenouncedUs(GetPlayer()->GetID(), false);

					for(iThirdPlayerLoop = 0; iThirdPlayerLoop < MAX_MAJOR_CIVS; iThirdPlayerLoop++){
						eThirdPlayer = (PlayerTypes) iThirdPlayerLoop;


						if(GetCoopWarCounter(eLoopPlayer, eThirdPlayer) < -1)
							SetCoopWarCounter(eLoopPlayer, eThirdPlayer, -1);
						GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetCoopWarCounter(GetPlayer()->GetID(), eThirdPlayer, -1);
					}


					CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
					if(pNotifications){
						CvString							strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_THEIR_DENUNCIATION_EXPIRED_S");
						Localization::String	strInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_THEIR_DENUNCIATION_EXPIRED");
						Localization::String strTemp = strInfo;
						strTemp << GET_PLAYER(GetPlayer()->GetID()).getCivilizationShortDescriptionKey();
						pNotifications->Add(NOTIFICATION_DENUNCIATION_EXPIRED, strTemp.toUTF8(), strSummary, -1, -1, GetPlayer()->GetID(), eLoopPlayer);
					}
				}


				if(IsDoFAccepted(eLoopPlayer) && GetDoFCounter(eLoopPlayer) >= GC.getGame().getGameSpeedInfo().getRelationshipDuration())
				{
					SetDoFCounter(eLoopPlayer, -1);
					SetDoFAccepted(eLoopPlayer, false);

					GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetDoFCounter(GetPlayer()->GetID(), -1);
					GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetDoFAccepted(GetPlayer()->GetID(), false);


					CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
					if (pNotifications){
						CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FRIENDSHIP_EXPIRED", GET_PLAYER(GetPlayer()->GetID()).getCivilizationShortDescriptionKey());
						CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_FRIENDSHIP_EXPIRED_S");
						pNotifications->Add(NOTIFICATION_FRIENDSHIP_EXPIRED, strBuffer, strSummary, -1, -1, GetPlayer()->GetID(), eLoopPlayer);				
					}

					pNotifications = GET_PLAYER(GetPlayer()->GetID()).GetNotifications();
					if (pNotifications){
						CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FRIENDSHIP_EXPIRED", GET_PLAYER(eLoopPlayer).getCivilizationShortDescriptionKey());
						CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_FRIENDSHIP_EXPIRED_S");
						pNotifications->Add(NOTIFICATION_FRIENDSHIP_EXPIRED, strBuffer, strSummary, -1, -1, eLoopPlayer, GetPlayer()->GetID());				
					}
				}
			}
		}
	}





	for(iItem = 0; iItem < MAX_DIPLO_LOG_STATEMENTS; iItem++)
	{
		eDeclaration = GetDeclarationLogTypeForIndex(iItem);

		if(eDeclaration != NO_PUBLIC_DECLARATION_TYPE)
		{
			ChangeDeclarationLogTurnForIndex(iItem, 1);
		}
		else
		{
			SetDeclarationLogTurnForIndex(iItem, 0);
		}
	}

}










void CvDiplomacyAI::DoUpdateOpinions()
{

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		DoUpdateOnePlayerOpinion(eLoopPlayer);
	}
}


void CvDiplomacyAI::DoUpdateOnePlayerOpinion(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(!IsPlayerValid(ePlayer))
		return;

	MajorCivOpinionTypes eOpinion;
	CvPlayer& kPlayerEvaluating = GET_PLAYER(ePlayer);


	if(GetPlayer()->getTeam() == kPlayerEvaluating.getTeam())
	{
		eOpinion = MAJOR_CIV_OPINION_ALLY;
	}

	else
	{
		int iOpinionWeight = GetMajorCivOpinionWeight(ePlayer);

		if(iOpinionWeight >=        GC.getOPINION_THRESHOLD_UNFORGIVABLE())
			eOpinion = MAJOR_CIV_OPINION_UNFORGIVABLE;
		else if(iOpinionWeight >=        GC.getOPINION_THRESHOLD_ENEMY())
			eOpinion = MAJOR_CIV_OPINION_ENEMY;
		else if(iOpinionWeight >=        GC.getOPINION_THRESHOLD_COMPETITOR())
			eOpinion = MAJOR_CIV_OPINION_COMPETITOR;
		else if(iOpinionWeight >=         GC.getOPINION_THRESHOLD_FAVORABLE())
			eOpinion = MAJOR_CIV_OPINION_NEUTRAL;
		else if(iOpinionWeight >=         GC.getOPINION_THRESHOLD_FRIEND())
			eOpinion = MAJOR_CIV_OPINION_FAVORABLE;
		else if(iOpinionWeight >=         GC.getOPINION_THRESHOLD_ALLY())
			eOpinion = MAJOR_CIV_OPINION_FRIEND;
		else
			eOpinion = MAJOR_CIV_OPINION_ALLY;





	}


	SetMajorCivOpinion(ePlayer, eOpinion);


}


int CvDiplomacyAI::GetMajorCivOpinionWeight(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;




	int iHostility = GetPersonalityMajorCivApproachBias(MAJOR_CIV_APPROACH_HOSTILE);
	int iFriendliness = GetPersonalityMajorCivApproachBias(MAJOR_CIV_APPROACH_FRIENDLY);
	iOpinionWeight += (iHostility - iFriendliness) * 2;

	iOpinionWeight += GetLandDisputeLevelScore(ePlayer);
	iOpinionWeight += GetWonderDisputeLevelScore(ePlayer);
	iOpinionWeight += GetMinorCivDisputeLevelScore(ePlayer);
	iOpinionWeight += GetWarmongerThreatScore(ePlayer);




	iOpinionWeight += GetCiviliansReturnedToMeScore(ePlayer);
	iOpinionWeight += GetLandmarksBuiltForMeScore(ePlayer);
	iOpinionWeight += GetResurrectedScore(ePlayer);
	iOpinionWeight += GetLiberatedCitiesScore(ePlayer);
	iOpinionWeight += GetEmbassyScore(ePlayer);
	iOpinionWeight += GetForgaveForSpyingScore(ePlayer);
	iOpinionWeight += GetTimesIntrigueSharedScore(ePlayer);




	iOpinionWeight += GetNoSetterRequestScore(ePlayer);
	iOpinionWeight += GetStopSpyingRequestScore(ePlayer);
	iOpinionWeight += GetDemandEverMadeScore(ePlayer);
	iOpinionWeight += GetTimesCultureBombedScore(ePlayer);
	iOpinionWeight += GetReligiousConversionPointsScore(ePlayer);

	iOpinionWeight += GetHasAdoptedHisReligionScore(ePlayer);
	iOpinionWeight += GetHasAdoptedMyReligionScore(ePlayer);
	
	iOpinionWeight += GetSameLatePoliciesScore(ePlayer);
	iOpinionWeight += GetDifferentLatePoliciesScore(ePlayer);
	

	iOpinionWeight += GetTimesRobbedScore(ePlayer);




	iOpinionWeight += GetBrokenMilitaryPromiseScore(ePlayer);
	iOpinionWeight += GetBrokenMilitaryPromiseWithAnybodyScore(ePlayer);
	iOpinionWeight += GetIgnoredMilitaryPromiseScore(ePlayer);
	iOpinionWeight += GetBrokenExpansionPromiseScore(ePlayer);
	iOpinionWeight += GetIgnoredExpansionPromiseScore(ePlayer);
	iOpinionWeight += GetBrokenBorderPromiseScore(ePlayer);
	iOpinionWeight += GetIgnoredBorderPromiseScore(ePlayer);

	iOpinionWeight += GetBrokenAttackCityStatePromiseScore(ePlayer);
	iOpinionWeight += GetBrokenAttackCityStatePromiseWithAnybodyScore(ePlayer);
	iOpinionWeight += GetIgnoredAttackCityStatePromiseScore(ePlayer);

	iOpinionWeight += GetBrokenBullyCityStatePromiseScore(ePlayer);
	iOpinionWeight += GetIgnoredBullyCityStatePromiseScore(ePlayer);

	iOpinionWeight += GetBrokenNoConvertPromiseScore(ePlayer);
	iOpinionWeight += GetIgnoredNoConvertPromiseScore(ePlayer);

	iOpinionWeight += GetBrokenNoDiggingPromiseScore(ePlayer);
	iOpinionWeight += GetIgnoredNoDiggingPromiseScore(ePlayer);

	iOpinionWeight += GetBrokenSpyPromiseScore(ePlayer);
	iOpinionWeight += GetIgnoredSpyPromiseScore(ePlayer);

	iOpinionWeight += GetBrokenCoopWarPromiseScore(ePlayer);




	iOpinionWeight += GetAngryAboutProtectedMinorKilledScore(ePlayer);
	iOpinionWeight += GetAngryAboutProtectedMinorAttackedScore(ePlayer);
	iOpinionWeight += GetAngryAboutProtectedMinorBulliedScore(ePlayer);




	iOpinionWeight += GetAngryAboutSidedWithProtectedMinorScore(ePlayer);




	iOpinionWeight += GetDOFAcceptedScore(ePlayer);
	iOpinionWeight += GetDOFWithAnyFriendScore(ePlayer);
	iOpinionWeight += GetDOFWithAnyEnemyScore(ePlayer);






	int iTraitorOpinion = GetFriendDenouncementScore(ePlayer);
	iTraitorOpinion = max(iTraitorOpinion, GetWeDenouncedFriendScore(ePlayer));
	iTraitorOpinion = max(iTraitorOpinion, GetFriendDenouncedUsScore(ePlayer));
	iTraitorOpinion = max(iTraitorOpinion, GetWeDeclaredWarOnFriendScore(ePlayer));
	iTraitorOpinion = max(iTraitorOpinion, GetFriendDeclaredWarOnUsScore(ePlayer));
	iOpinionWeight += iTraitorOpinion;








	iOpinionWeight += GetDenouncedUsScore(ePlayer);
	iOpinionWeight += GetDenouncedThemScore(ePlayer);
	iOpinionWeight += GetDenouncedFriendScore(ePlayer);
	iOpinionWeight += GetDenouncedEnemyScore(ePlayer);




	iOpinionWeight += GetRecklessExpanderScore(ePlayer);




	iOpinionWeight += GetRecentTradeScore(ePlayer);
	iOpinionWeight += GetCommonFoeScore(ePlayer);
	iOpinionWeight += GetRecentAssistScore(ePlayer);

	iOpinionWeight += GetNukedByScore(ePlayer);
	iOpinionWeight += GetCapitalCapturedByScore(ePlayer);

	iOpinionWeight += GetGaveAssistanceToScore(ePlayer);
	iOpinionWeight += GetPaidTributeToScore(ePlayer);





	iOpinionWeight += GetLikedTheirProposalScore(ePlayer);
	iOpinionWeight += GetDislikedTheirProposalScore(ePlayer);
	iOpinionWeight += GetSupportedMyProposalScore(ePlayer);
	iOpinionWeight += GetFoiledMyProposalScore(ePlayer);
	iOpinionWeight += GetSupportedMyHostingScore(ePlayer);




	iOpinionWeight += GetScenarioModifier1(ePlayer);
	iOpinionWeight += GetScenarioModifier2(ePlayer);
	iOpinionWeight += GetScenarioModifier3(ePlayer);

	return iOpinionWeight;
}


MajorCivOpinionTypes CvDiplomacyAI::GetMajorCivOpinion(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (MajorCivOpinionTypes) m_paeMajorCivOpinion[ePlayer];
}


void CvDiplomacyAI::SetMajorCivOpinion(PlayerTypes ePlayer, MajorCivOpinionTypes eOpinion)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOpinion >= 0, "DIPLOMACY_AI: Invalid MajorCivOpinionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOpinion < NUM_MAJOR_CIV_OPINION_TYPES, "DIPLOMACY_AI: Invalid MajorCivOpinionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeMajorCivOpinion[ePlayer] = eOpinion;
}


int CvDiplomacyAI::GetNumMajorCivOpinion(MajorCivOpinionTypes eOpinion) const
{
	CvAssertMsg(eOpinion >= 0, "DIPLOMACY_AI: Invalid MajorCivOpinionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOpinion < NUM_MAJOR_CIV_OPINION_TYPES, "DIPLOMACY_AI: Invalid MajorCivOpinionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iCount = 0;

	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		if(GetMajorCivOpinion((PlayerTypes) iMajorLoop) == eOpinion)
		{
			iCount++;
		}
	}

	return iCount;
}



void CvDiplomacyAI::DoEstimateOtherPlayerOpinions()
{
	MajorCivOpinionTypes eOpinion;

	int iOpinionWeight;

	PlayerTypes eLoopOtherPlayer;
	int iOtherPlayerLoop;


	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{

			for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_MAJOR_CIVS; iOtherPlayerLoop++)
			{
				eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;


				if(eLoopPlayer != eLoopOtherPlayer)
				{

					if(IsPlayerValid(eLoopOtherPlayer, true) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopOtherPlayer))
					{
						iOpinionWeight = 0;


						switch(GetEstimateOtherPlayerLandDisputeLevel(eLoopPlayer, eLoopOtherPlayer))
						{
						case DISPUTE_LEVEL_FIERCE:
							iOpinionWeight +=        GC.getOPINION_WEIGHT_LAND_FIERCE();
							break;
						case DISPUTE_LEVEL_STRONG:
							iOpinionWeight +=        GC.getOPINION_WEIGHT_LAND_STRONG();
							break;
						case DISPUTE_LEVEL_WEAK:
							iOpinionWeight +=        GC.getOPINION_WEIGHT_LAND_WEAK();
							break;
						case DISPUTE_LEVEL_NONE:
							iOpinionWeight +=       GC.getOPINION_WEIGHT_LAND_NONE();
							break;
						}


						switch(GetEstimateOtherPlayerVictoryDisputeLevel(eLoopPlayer, eLoopOtherPlayer))
						{
						case DISPUTE_LEVEL_FIERCE:
							iOpinionWeight +=        GC.getOPINION_WEIGHT_VICTORY_FIERCE();
							break;
						case DISPUTE_LEVEL_STRONG:
							iOpinionWeight +=        GC.getOPINION_WEIGHT_VICTORY_STRONG();
							break;
						case DISPUTE_LEVEL_WEAK:
							iOpinionWeight +=        GC.getOPINION_WEIGHT_VICTORY_WEAK();
							break;
						case DISPUTE_LEVEL_NONE:
							iOpinionWeight +=       GC.getOPINION_WEIGHT_VICTORY_NONE();
							break;
						}


						if(iOpinionWeight >=        GC.getOPINION_THRESHOLD_UNFORGIVABLE())
							eOpinion = MAJOR_CIV_OPINION_UNFORGIVABLE;
						else if(iOpinionWeight >=        GC.getOPINION_THRESHOLD_ENEMY())
							eOpinion = MAJOR_CIV_OPINION_ENEMY;
						else if(iOpinionWeight >=        GC.getOPINION_THRESHOLD_COMPETITOR())
							eOpinion = MAJOR_CIV_OPINION_COMPETITOR;
						else if(iOpinionWeight <         GC.getOPINION_THRESHOLD_FAVORABLE())
							eOpinion = MAJOR_CIV_OPINION_FAVORABLE;
						else if(iOpinionWeight <         GC.getOPINION_THRESHOLD_FRIEND())
							eOpinion = MAJOR_CIV_OPINION_FRIEND;
						else if(iOpinionWeight <         GC.getOPINION_THRESHOLD_ALLY())
							eOpinion = MAJOR_CIV_OPINION_ALLY;
						else
							eOpinion = MAJOR_CIV_OPINION_NEUTRAL;


						SetMajorCivOtherPlayerOpinion(eLoopPlayer, eLoopOtherPlayer, eOpinion);
					}
				}
			}
		}
	}
}


MajorCivOpinionTypes CvDiplomacyAI::GetMajorCivOtherPlayerOpinion(PlayerTypes ePlayer, PlayerTypes eWithPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (MajorCivOpinionTypes) m_ppaaeOtherPlayerMajorCivOpinion[ePlayer][eWithPlayer];
}


void CvDiplomacyAI::SetMajorCivOtherPlayerOpinion(PlayerTypes ePlayer, PlayerTypes eWithPlayer, MajorCivOpinionTypes ePlayerOpinion)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayerOpinion >= 0, "DIPLOMACY_AI: Invalid MajorCivOpinionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayerOpinion < NUM_MAJOR_CIV_OPINION_TYPES, "DIPLOMACY_AI: Invalid MajorCivOpinionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaeOtherPlayerMajorCivOpinion[ePlayer][eWithPlayer] = ePlayerOpinion;
}



void CvDiplomacyAI::DoUpdateMajorCivApproaches()
{
	int iPlayerLoop;
	PlayerTypes eLoopPlayer;


	for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		m_paeApproachScratchPad[iPlayerLoop] = -1;

		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			m_paeApproachScratchPad[iPlayerLoop] = GetMajorCivApproach(eLoopPlayer,                       false);
			SetMajorCivApproach(eLoopPlayer, NO_MAJOR_CIV_APPROACH);
		}
	}

	int iHighestWeight;

	MajorCivOpinionTypes eOpinion;
	int iNumCivsWeHaveThisOpinionTowards;

	FStaticVector<PlayerTypes, NUM_MAJOR_CIV_OPINION_TYPES* MAX_MAJOR_CIVS, true, c_eCiv5GameplayDLL, 0> vePlayerApproachOrder;
	CvWeightedVector<PlayerTypes, MAX_MAJOR_CIVS, true> veTiedPlayerApproachWeights;
	int iTiedPlayerVectorIndex;

	int iNumCivsWereLookingAt = 0;

	bool bAssert;


	for(int iOpinionLoop = 0; iOpinionLoop < NUM_MAJOR_CIV_OPINION_TYPES; iOpinionLoop++)
	{
		eOpinion = (MajorCivOpinionTypes) iOpinionLoop;

		iNumCivsWeHaveThisOpinionTowards = GetNumMajorCivOpinion(eOpinion);

		if(iNumCivsWeHaveThisOpinionTowards > 0)
		{
			veTiedPlayerApproachWeights.clear();

			WarFaceTypes eWarFace;


			for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(IsPlayerValid(eLoopPlayer))
				{
					if(GetMajorCivOpinion(eLoopPlayer) == eOpinion)
					{

						if(iNumCivsWeHaveThisOpinionTowards == 1)
						{
							iNumCivsWereLookingAt++;
							vePlayerApproachOrder.push_back(eLoopPlayer);
							break;
						}

						else
						{

							GetBestApproachTowardsMajorCiv(eLoopPlayer,                   iHighestWeight,                         false,          false,                                        eWarFace);
							veTiedPlayerApproachWeights.push_back(eLoopPlayer, iHighestWeight);
						}
					}
				}
			}


			if(iNumCivsWeHaveThisOpinionTowards > 1)
			{
				veTiedPlayerApproachWeights.SortItems();


				for(iTiedPlayerVectorIndex = 0; iTiedPlayerVectorIndex < (int) veTiedPlayerApproachWeights.size(); iTiedPlayerVectorIndex++)
				{
					eLoopPlayer = (PlayerTypes) veTiedPlayerApproachWeights.GetElement(iTiedPlayerVectorIndex);


					if(eLoopPlayer < 0 || eLoopPlayer >= MAX_CIV_PLAYERS)
					{
						bAssert = true;
						CvAssert(false);
					}

					iNumCivsWereLookingAt++;
					vePlayerApproachOrder.push_back(eLoopPlayer);
				}
			}
		}
	}


	if(iNumCivsWereLookingAt != (int) vePlayerApproachOrder.size())
	{
		bAssert = true;
		CvAssert(false);
	}

	MajorCivApproachTypes eApproach;

	WarFaceTypes eWarFace;


	for(int iPlayerVectorIndex = 0; iPlayerVectorIndex < (int) vePlayerApproachOrder.size(); iPlayerVectorIndex++)
	{
		eLoopPlayer = vePlayerApproachOrder[iPlayerVectorIndex];


		eApproach = GetBestApproachTowardsMajorCiv(eLoopPlayer,                                        iHighestWeight,                         true,          true,                               eWarFace);


		if(eApproach == MAJOR_CIV_APPROACH_WAR && GetWarFaceWithPlayer(eLoopPlayer) == NO_WAR_FACE_TYPE)
			SetWarFaceWithPlayer(eLoopPlayer, eWarFace);

		CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


		SetMajorCivApproach(eLoopPlayer, eApproach);
	}
}


MajorCivApproachTypes CvDiplomacyAI::GetBestApproachTowardsMajorCiv(PlayerTypes ePlayer, int& iHighestWeight, bool bLookAtOtherPlayers, bool bLog, WarFaceTypes& eWarFace)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	CvTeam* pTeam = &GET_TEAM(eTeam);

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	MajorCivApproachTypes eApproach;

	FStaticVector< int, 128, true, c_eCiv5GameplayDLL > viApproachWeights;

	int iApproachLoop;

	viApproachWeights.clear();

	for(iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
	{
		viApproachWeights.push_back(0);
	}





	viApproachWeights[MAJOR_CIV_APPROACH_NEUTRAL] +=       GC.getAPPROACH_NEUTRAL_DEFAULT();







	MajorCivApproachTypes eOldApproach = (MajorCivApproachTypes) m_paeApproachScratchPad[ePlayer];
	if(eOldApproach == NO_MAJOR_CIV_APPROACH)
		eOldApproach = MAJOR_CIV_APPROACH_NEUTRAL;

	viApproachWeights[eOldApproach] +=       GC.getAPPROACH_BIAS_FOR_CURRENT();


	if(eOldApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_CURRENTLY_DECEPTIVE();


	if(eOldApproach == MAJOR_CIV_APPROACH_HOSTILE)
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=       GC.getAPPROACH_HOSTILE_CURRENTLY_HOSTILE();


	WarStateTypes eWarState = GetWarState(ePlayer);
	if(eWarState > WAR_STATE_STALEMATE)
	{

		if(eOldApproach == MAJOR_CIV_APPROACH_WAR)
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_CURRENTLY_WAR();
	}


	if(eWarState == NO_WAR_STATE_TYPE || eWarState > WAR_STATE_DEFENSIVE)
	{
		if(IsGoingForWorldConquest())
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_CONQUEST_GRAND_STRATEGY();
	}





	for(iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
	{
		viApproachWeights[iApproachLoop] += GetPersonalityMajorCivApproachBias((MajorCivApproachTypes) iApproachLoop);
	}





	switch(GetMajorCivOpinion(ePlayer))
	{
	case MAJOR_CIV_OPINION_UNFORGIVABLE:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=        GC.getAPPROACH_OPINION_UNFORGIVABLE_WAR();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=       GC.getAPPROACH_OPINION_UNFORGIVABLE_HOSTILE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_OPINION_UNFORGIVABLE_DECEPTIVE();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_OPINION_UNFORGIVABLE_GUARDED();
		break;
	case MAJOR_CIV_OPINION_ENEMY:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_OPINION_ENEMY_WAR();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=       GC.getAPPROACH_OPINION_ENEMY_HOSTILE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_OPINION_ENEMY_DECEPTIVE();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_OPINION_ENEMY_GUARDED();
		break;
	case MAJOR_CIV_OPINION_COMPETITOR:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_OPINION_COMPETITOR_WAR();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=       GC.getAPPROACH_OPINION_COMPETITOR_HOSTILE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_OPINION_COMPETITOR_DECEPTIVE();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_OPINION_COMPETITOR_GUARDED();
		break;
	case MAJOR_CIV_OPINION_NEUTRAL:
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_OPINION_NEUTRAL_DECEPTIVE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY]  +=       GC.getAPPROACH_OPINION_NEUTRAL_FRIENDLY();
		break;
	case MAJOR_CIV_OPINION_FAVORABLE:
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=        GC.getAPPROACH_OPINION_FAVORABLE_HOSTILE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_OPINION_FAVORABLE_DECEPTIVE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=       GC.getAPPROACH_OPINION_FAVORABLE_FRIENDLY();
		break;
	case MAJOR_CIV_OPINION_FRIEND:
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=        GC.getAPPROACH_OPINION_FRIEND_HOSTILE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=        GC.getAPPROACH_OPINION_FRIEND_FRIENDLY();
		break;
	case MAJOR_CIV_OPINION_ALLY:
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=        GC.getAPPROACH_OPINION_ALLY_FRIENDLY();
		break;
	}





	if(IsDoFAccepted(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_DECEPTIVE_WORKING_WITH_PLAYER();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=        GC.getAPPROACH_FRIENDLY_WORKING_WITH_PLAYER();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=          GC.getAPPROACH_HOSTILE_WORKING_WITH_PLAYER();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=          GC.getAPPROACH_GUARDED_WORKING_WITH_PLAYER();
	}






	if(IsDenouncedPlayer(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=        GC.getAPPROACH_WAR_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=        GC.getAPPROACH_HOSTILE_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_GUARDED_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=          GC.getAPPROACH_FRIENDLY_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=          GC.getAPPROACH_DECEPTIVE_DENOUNCED();
	}


	if(IsFriendDenouncedUs(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=        GC.getAPPROACH_WAR_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=        GC.getAPPROACH_HOSTILE_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_GUARDED_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=          GC.getAPPROACH_FRIENDLY_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=          GC.getAPPROACH_DECEPTIVE_DENOUNCED();
	}


	if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDenouncedPlayer(eMyPlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=        GC.getAPPROACH_WAR_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=        GC.getAPPROACH_HOSTILE_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_GUARDED_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=          GC.getAPPROACH_FRIENDLY_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=          GC.getAPPROACH_DECEPTIVE_DENOUNCED();
	}


















	if(IsAngryAboutProtectedMinorKilled(ePlayer) || IsAngryAboutProtectedMinorAttacked(ePlayer))
	{
		int iTurnsSinceMinorAttack = GetTurnsSincePlayerAttackedProtectedMinor(ePlayer);
		if (IsAngryAboutProtectedMinorKilled(ePlayer) || iTurnsSinceMinorAttack <        GC.getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY_NUM_TURNS())
		{
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_ATTACKED_PROTECTED_MINOR_WAR();
			viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=       GC.getAPPROACH_ATTACKED_PROTECTED_MINOR_HOSTILE();
			viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_ATTACKED_PROTECTED_MINOR_GUARDED();
		}
		else
		{
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_WAR();
			viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=       GC.getAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_HOSTILE();
			viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_GUARDED();
		}
	}





	if(IsDemandEverMade(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=        GC.getAPPROACH_DECEPTIVE_DEMAND();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=        GC.getAPPROACH_FRIENDLY_DEMAND();
	}






	if(IsPlayerBrokenMilitaryPromise(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_BROKEN_MILITARY_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=         GC.getAPPROACH_DECEPTIVE_BROKEN_MILITARY_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=         GC.getAPPROACH_FRIENDLY_BROKEN_MILITARY_PROMISE();
	}

	else if(pTeam->IsBrokenMilitaryPromise())
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_BROKEN_MILITARY_PROMISE_WORLD();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=        GC.getAPPROACH_DECEPTIVE_BROKEN_MILITARY_PROMISE_WORLD();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=        GC.getAPPROACH_FRIENDLY_BROKEN_MILITARY_PROMISE_WORLD();
	}

	else if(IsPlayerIgnoredMilitaryPromise(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=        GC.getAPPROACH_DECEPTIVE_IGNORED_MILITARY_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=        GC.getAPPROACH_FRIENDLY_IGNORED_MILITARY_PROMISE();
	}


	if(IsPlayerBrokenExpansionPromise(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_BROKEN_EXPANSION_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=        GC.getAPPROACH_DECEPTIVE_BROKEN_EXPANSION_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=        GC.getAPPROACH_FRIENDLY_BROKEN_EXPANSION_PROMISE();
	}

	else if(IsPlayerIgnoredExpansionPromise(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_IGNORED_EXPANSION_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=        GC.getAPPROACH_DECEPTIVE_IGNORED_EXPANSION_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=        GC.getAPPROACH_FRIENDLY_IGNORED_EXPANSION_PROMISE();
	}


	if(IsPlayerBrokenBorderPromise(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_BROKEN_BORDER_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=        GC.getAPPROACH_DECEPTIVE_BROKEN_BORDER_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=        GC.getAPPROACH_FRIENDLY_BROKEN_BORDER_PROMISE();
	}

	else if(IsPlayerIgnoredBorderPromise(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_IGNORED_BORDER_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=        GC.getAPPROACH_DECEPTIVE_IGNORED_BORDER_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=        GC.getAPPROACH_FRIENDLY_IGNORED_BORDER_PROMISE();
	}


	if(IsPlayerBrokenAttackCityStatePromise(ePlayer))
	{

		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_BROKEN_CITY_STATE_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=         GC.getAPPROACH_DECEPTIVE_BROKEN_CITY_STATE_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=         GC.getAPPROACH_FRIENDLY_BROKEN_CITY_STATE_PROMISE();
	}

	else if(pTeam->IsBrokenCityStatePromise())
	{

		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_BROKEN_CITY_STATE_PROMISE_WORLD();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=        GC.getAPPROACH_DECEPTIVE_BROKEN_CITY_STATE_PROMISE_WORLD();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=        GC.getAPPROACH_FRIENDLY_BROKEN_CITY_STATE_PROMISE_WORLD();
	}

	else if(IsPlayerIgnoredAttackCityStatePromise(ePlayer))
	{

		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_IGNORED_CITY_STATE_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=        GC.getAPPROACH_DECEPTIVE_IGNORED_CITY_STATE_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=        GC.getAPPROACH_FRIENDLY_IGNORED_CITY_STATE_PROMISE();
	}


	if(IsPlayerBrokenBullyCityStatePromise(ePlayer))
	{

		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_BROKEN_CITY_STATE_PROMISE() / 2;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=         GC.getAPPROACH_DECEPTIVE_BROKEN_CITY_STATE_PROMISE() / 2;
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=         GC.getAPPROACH_FRIENDLY_BROKEN_CITY_STATE_PROMISE() / 2;
	}
	else if(IsPlayerIgnoredBullyCityStatePromise(ePlayer))
	{

		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=       GC.getAPPROACH_WAR_IGNORED_CITY_STATE_PROMISE() / 2;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=        GC.getAPPROACH_DECEPTIVE_IGNORED_CITY_STATE_PROMISE() / 2;
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=        GC.getAPPROACH_FRIENDLY_IGNORED_CITY_STATE_PROMISE() / 2;
	}






	switch(GetMilitaryThreat(ePlayer))
	{
	case THREAT_CRITICAL:
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_DECEPTIVE_MILITARY_THREAT_CRITICAL();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_GUARDED_MILITARY_THREAT_CRITICAL();
		viApproachWeights[MAJOR_CIV_APPROACH_AFRAID] +=       GC.getAPPROACH_AFRAID_MILITARY_THREAT_CRITICAL();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=       GC.getAPPROACH_FRIENDLY_MILITARY_THREAT_CRITICAL();
		break;
	case THREAT_SEVERE:
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_DECEPTIVE_MILITARY_THREAT_SEVERE();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_GUARDED_MILITARY_THREAT_SEVERE();
		viApproachWeights[MAJOR_CIV_APPROACH_AFRAID] +=       GC.getAPPROACH_AFRAID_MILITARY_THREAT_SEVERE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=       GC.getAPPROACH_FRIENDLY_MILITARY_THREAT_SEVERE();
		break;
	case THREAT_MAJOR:
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_DECEPTIVE_MILITARY_THREAT_MAJOR();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_GUARDED_MILITARY_THREAT_MAJOR();
		viApproachWeights[MAJOR_CIV_APPROACH_AFRAID] +=       GC.getAPPROACH_AFRAID_MILITARY_THREAT_MAJOR();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=       GC.getAPPROACH_FRIENDLY_MILITARY_THREAT_MAJOR();
		break;
	case THREAT_MINOR:
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_DECEPTIVE_MILITARY_THREAT_MINOR();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_GUARDED_MILITARY_THREAT_MINOR();
		viApproachWeights[MAJOR_CIV_APPROACH_AFRAID] +=       GC.getAPPROACH_AFRAID_MILITARY_THREAT_MINOR();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=       GC.getAPPROACH_FRIENDLY_MILITARY_THREAT_MINOR();
		break;
	case THREAT_NONE:
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=       GC.getAPPROACH_HOSTILE_MILITARY_THREAT_NONE();
		break;
	}





	int iTheirNukes = GET_PLAYER(ePlayer).getNumNukeUnits();
	int iOurNukes = m_pPlayer->getNumNukeUnits();
	int iHowLikelyAreTheyToNukeUs = GET_PLAYER(ePlayer).isHuman() ? 100 : 0;
	if (iHowLikelyAreTheyToNukeUs == 0)
	{
		if (IsNukedBy(ePlayer) || GET_PLAYER(ePlayer).GetDiplomacyAI()->IsNukedBy(eMyPlayer))
		{
			iHowLikelyAreTheyToNukeUs = 100;
		}




		else
		{
			int iFlavorNuke = GET_PLAYER(ePlayer).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_USE_NUKE")) + 1;
			iHowLikelyAreTheyToNukeUs = iFlavorNuke * iFlavorNuke;
		}
	}

	if(iTheirNukes == 0 && iOurNukes > 0)
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += 25;
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += 25;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += 25;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] -= 25;
		viApproachWeights[MAJOR_CIV_APPROACH_AFRAID] -= 50;
	}

	else if (iTheirNukes > 0 && iOurNukes == 0 && iHowLikelyAreTheyToNukeUs > 50)
	{
		viApproachWeights[MAJOR_CIV_APPROACH_AFRAID] += 50;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += 50;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] -= 25;
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] -= 50;
	}

	else if (iTheirNukes > 0 && iOurNukes > 0 && iHowLikelyAreTheyToNukeUs > 25)
	{
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += 25;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] -= 25;
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] -= 25;
		viApproachWeights[MAJOR_CIV_APPROACH_NEUTRAL] += 25;
	}

	int iPlayerLoop;
	PlayerTypes eLoopPlayer;





	for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;


		if(ePlayer != eLoopPlayer && GET_PLAYER(ePlayer).getTeam() != GET_PLAYER(eLoopPlayer).getTeam())
		{
			if(IsPlayerValid(eLoopPlayer))
			{
				if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				{
					if(GetStateAllWars() == STATE_ALL_WARS_NEUTRAL)
						viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=        GC.getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_NEUTRAL();
					else if(GetStateAllWars() == STATE_ALL_WARS_WINNING)
						viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=        GC.getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_WINNING();
					else
						viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=         GC.getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_LOSING();

					viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=        GC.getAPPROACH_HOSTILE_AT_WAR_WITH_PLAYER();
					viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_DECEPTIVE_AT_WAR_WITH_PLAYER();
					viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_GUARDED_AT_WAR_WITH_PLAYER();
					viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=       GC.getAPPROACH_FRIENDLY_AT_WAR_WITH_PLAYER();
				}
			}
		}
	}






	if(bLookAtOtherPlayers)
	{
		bool bMinorCiv;

		for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;


			if(eLoopPlayer != ePlayer)
			{
				if(IsPlayerValid(eLoopPlayer))
				{
					bMinorCiv = GET_PLAYER(eLoopPlayer).isMinorCiv();


					if(GetWarGoal(eLoopPlayer) == WAR_GOAL_PREPARE ||
					        (!bMinorCiv && GetMajorCivApproach(eLoopPlayer,                       false) == MAJOR_CIV_APPROACH_WAR))
					{
						viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=         GC.getAPPROACH_WAR_PLANNING_WAR_WITH_ANOTHER_PLAYER();
						viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=        GC.getAPPROACH_HOSTILE_PLANNING_WAR_WITH_ANOTHER_PLAYER();
						viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_DECEPTIVE_PLANNING_WAR_WITH_ANOTHER_PLAYER();
						viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] +=       GC.getAPPROACH_GUARDED_PLANNING_WAR_WITH_ANOTHER_PLAYER();
						viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=       GC.getAPPROACH_FRIENDLY_PLANNING_WAR_WITH_ANOTHER_PLAYER();
					}


					if(!bMinorCiv)
					{
						switch(GetMajorCivApproach(eLoopPlayer,                       false))
						{
						case MAJOR_CIV_APPROACH_HOSTILE:
							viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=        GC.getAPPROACH_WAR_HOSTILE_WITH_ANOTHER_PLAYER();
							viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=        GC.getAPPROACH_HOSTILE_HOSTILE_WITH_ANOTHER_PLAYER();
							viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_DECEPTIVE_HOSTILE_WITH_ANOTHER_PLAYER();
							viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=       GC.getAPPROACH_FRIENDLY_HOSTILE_WITH_ANOTHER_PLAYER();
							break;
						case MAJOR_CIV_APPROACH_AFRAID:
							viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=        GC.getAPPROACH_WAR_AFRAID_WITH_ANOTHER_PLAYER();
							viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] +=        GC.getAPPROACH_HOSTILE_AFRAID_WITH_ANOTHER_PLAYER();
							viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] +=       GC.getAPPROACH_DECEPTIVE_AFRAID_WITH_ANOTHER_PLAYER();
							viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] +=       GC.getAPPROACH_FRIENDLY_AFRAID_WITH_ANOTHER_PLAYER();
							break;
						}
					}
				}
			}
		}
	}




	int iCurrentTradeValue = GetPlayer()->GetTrade()->GetAllTradeValueFromPlayerTimes100(YIELD_GOLD, ePlayer) / 100;
	if(iCurrentTradeValue > 0)
	{

		viApproachWeights[MAJOR_CIV_APPROACH_WAR] -= 5;
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] -= 5;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += 5;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] -= 1;
		viApproachWeights[MAJOR_CIV_APPROACH_AFRAID] -= 1;
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += 5;
		viApproachWeights[MAJOR_CIV_APPROACH_NEUTRAL] -= 2;


		int iGPT = GetPlayer()->calculateGoldRate();
		int iDeltaGPT = iGPT - iCurrentTradeValue;
		if (iGPT >= 0 && (iDeltaGPT < 0))
		{
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] += iDeltaGPT;
		}
	}





	switch(GetWarProjection(ePlayer))
	{
	case WAR_PROJECTION_DESTRUCTION:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *=        GC.getAPPROACH_WAR_PROJECTION_DESTRUCTION_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *=        GC.getAPPROACH_WAR_PROJECTION_DESTRUCTION_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] *=         GC.getAPPROACH_GUARDED_PROJECTION_DESTRUCTION_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] /= 100;
		break;
	case WAR_PROJECTION_DEFEAT:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *=        GC.getAPPROACH_WAR_PROJECTION_DEFEAT_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *=        GC.getAPPROACH_WAR_PROJECTION_DEFEAT_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] *=         GC.getAPPROACH_GUARDED_PROJECTION_DEFEAT_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] /= 100;
		break;
	case WAR_PROJECTION_STALEMATE:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *=        GC.getAPPROACH_WAR_PROJECTION_STALEMATE_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] *=         GC.getAPPROACH_GUARDED_PROJECTION_STALEMATE_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] /= 100;
		break;
	case WAR_PROJECTION_UNKNOWN:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *=         GC.getAPPROACH_WAR_PROJECTION_UNKNOWN_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] *=         GC.getAPPROACH_GUARDED_PROJECTION_UNKNOWN_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] /= 100;
		break;
	case WAR_PROJECTION_GOOD:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *=         GC.getAPPROACH_WAR_PROJECTION_GOOD_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *=         GC.getAPPROACH_WAR_PROJECTION_GOOD_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] *=        GC.getAPPROACH_GUARDED_PROJECTION_GOOD_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] /= 100;
		break;
	case WAR_PROJECTION_VERY_GOOD:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *=         GC.getAPPROACH_WAR_PROJECTION_VERY_GOOD_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *=         GC.getAPPROACH_WAR_PROJECTION_VERY_GOOD_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] *=        GC.getAPPROACH_GUARDED_PROJECTION_VERY_GOOD_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] /= 100;
		break;
	}





	if(IsPlayerRecklessExpander(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *=         GC.getAPPROACH_WAR_RECKLESS_EXPANDER();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *=         GC.getAPPROACH_WAR_RECKLESS_EXPANDER();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
	}





	bool bThinkingAboutDogpiling = false;
	for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;


		if(eLoopPlayer != GET_PLAYER(ePlayer).GetID() && eLoopPlayer != m_pPlayer->GetID() && GET_PLAYER(ePlayer).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			eWarState = GET_PLAYER(ePlayer).GetDiplomacyAI()->GetWarState(eLoopPlayer);
			if(eWarState != NO_WAR_STATE_TYPE)
			{
				bThinkingAboutDogpiling = true;
			}
		}
	}
	if (bThinkingAboutDogpiling)
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= 200;
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *= 150;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
	}






	switch(GetPlayer()->GetProximityToPlayer(ePlayer))
	{
	case PLAYER_PROXIMITY_NEIGHBORS:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *=         GC.getAPPROACH_WAR_PROXIMITY_NEIGHBORS();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *=         GC.getAPPROACH_WAR_PROXIMITY_NEIGHBORS();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		break;
	case PLAYER_PROXIMITY_CLOSE:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *=         GC.getAPPROACH_WAR_PROXIMITY_CLOSE();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *=         GC.getAPPROACH_WAR_PROXIMITY_CLOSE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		break;
	case PLAYER_PROXIMITY_FAR:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *=        GC.getAPPROACH_WAR_PROXIMITY_FAR();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *=        GC.getAPPROACH_WAR_PROXIMITY_FAR();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		break;
	case PLAYER_PROXIMITY_DISTANT:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *=        GC.getAPPROACH_WAR_PROXIMITY_DISTANT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *=        GC.getAPPROACH_WAR_PROXIMITY_DISTANT();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		break;
	}





	int iPeaceTreatyTurn = GET_TEAM(GetTeam()).GetTurnMadePeaceTreatyWithTeam(eTeam);
	if(iPeaceTreatyTurn > -1)
	{
		int iTurnsSincePeace = GC.getGame().getElapsedGameTurns() - iPeaceTreatyTurn;
		if(iTurnsSincePeace <        GC.getTURNS_SINCE_PEACE_WEIGHT_DAMPENER())
		{
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] *=        GC.getAPPROACH_WAR_HAS_MADE_PEACE_BEFORE_PERCENT();
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		}
	}





	int iNumMajorsLeft = GC.getGame().countMajorCivsAlive();
	if(iNumMajorsLeft == 2)
	{
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] = 0;
	}





	if(IsLockedIntoCoopWar(ePlayer))
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] +=          GC.getCOOP_WAR_LOCKED_TURNS_WAR_WEIGHT();





	int iRandAmount;
	for(iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
	{

		viApproachWeights[iApproachLoop] *= 100;

		iRandAmount = viApproachWeights[iApproachLoop] *        GC.getAPPROACH_RANDOM_PERCENT();


		if(iRandAmount > 0)
		{
			iRandAmount /= 100;
			viApproachWeights[iApproachLoop] += GC.getGame().getJonRandNumVA(iRandAmount, "Randomizing Diplo AI Major Civ Approach Weight (%d; %d)", (int)GetPlayer()->GetID(), (int)ePlayer);
		}
	}





	if(!GET_TEAM(GetTeam()).canDeclareWar(GET_PLAYER(ePlayer).getTeam()))
	{

		if(!GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
		{
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] = 0;
		}
	}




	if(GetTeam() == GET_PLAYER(ePlayer).getTeam())
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_AFRAID] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_NEUTRAL] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] = 100;
	}





	if(GET_PLAYER(ePlayer).isHuman())
	{
		HandicapTypes eHandicap = GET_PLAYER(ePlayer).getHandicapType();
		int iWarMod = 100;

		CvHandicapInfo* pHandicap = GC.getHandicapInfo(eHandicap);
		if(pHandicap)
			iWarMod = pHandicap->getAIDeclareWarProb();

		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= iWarMod;
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *= iWarMod;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
	}


	CvWeightedVector< MajorCivApproachTypes, 128 > vApproachWeightsForSorting;
	vApproachWeightsForSorting.clear();


	for(iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
	{
		vApproachWeightsForSorting.push_back((MajorCivApproachTypes) iApproachLoop, viApproachWeights[iApproachLoop]);
	}

	vApproachWeightsForSorting.SortItems();

	eApproach = vApproachWeightsForSorting.GetElement(0);
	iHighestWeight = vApproachWeightsForSorting.GetWeight(0);


	if(eApproach == MAJOR_CIV_APPROACH_WAR)
	{
		eWarFace = GetWarFaceWithPlayer(ePlayer);


		if(eWarFace == NO_WAR_FACE_TYPE)
		{
			MajorCivApproachTypes eTempApproach;


			eTempApproach = vApproachWeightsForSorting.GetElement(1);


			switch(eTempApproach)
			{

			case MAJOR_CIV_APPROACH_HOSTILE:
				eWarFace = WAR_FACE_HOSTILE;
				break;

			case MAJOR_CIV_APPROACH_DECEPTIVE:
			case MAJOR_CIV_APPROACH_AFRAID:
			case MAJOR_CIV_APPROACH_FRIENDLY:


				if(IsDenouncedPlayer(ePlayer))
				{
					eWarFace = WAR_FACE_NEUTRAL;
				}
				else
				{
					eWarFace = WAR_FACE_FRIENDLY;
				}
				break;

			default:
				eWarFace = WAR_FACE_NEUTRAL;
				break;
			}
		}
	}
	else
	{
		eWarFace = NO_WAR_FACE_TYPE;
	}


	if(bLog)
	{
		LogMajorCivApproachUpdate(ePlayer, viApproachWeights.begin(), eApproach, eOldApproach, eWarFace);
	}

	return eApproach;
}



MajorCivApproachTypes CvDiplomacyAI::GetMajorCivApproach(PlayerTypes ePlayer, bool bHideTrueFeelings) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproach = (MajorCivApproachTypes) m_paeMajorCivApproach[ePlayer];


	if(bHideTrueFeelings)
	{

		if(eApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
		{
			eApproach = MAJOR_CIV_APPROACH_FRIENDLY;
		}


		else if(eApproach == MAJOR_CIV_APPROACH_WAR)
		{
			switch(GetWarFaceWithPlayer(ePlayer))
			{
			case WAR_FACE_HOSTILE:
				eApproach = MAJOR_CIV_APPROACH_HOSTILE;
				break;
			case WAR_FACE_FRIENDLY:
				eApproach = MAJOR_CIV_APPROACH_FRIENDLY;
				break;
			case WAR_FACE_NEUTRAL:
				eApproach = MAJOR_CIV_APPROACH_NEUTRAL;
				break;
			default:
				CvAssert(false);
				break;
			}
		}
	}

	return eApproach;
}


void CvDiplomacyAI::SetMajorCivApproach(PlayerTypes ePlayer, MajorCivApproachTypes eApproach)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach >= NO_MAJOR_CIV_APPROACH, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach < NUM_MAJOR_CIV_APPROACHES, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeMajorCivApproach[ePlayer] = eApproach;
}


int CvDiplomacyAI::GetNumMajorCivApproach(MajorCivApproachTypes eApproach) const
{
	CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach < NUM_MAJOR_CIV_APPROACHES, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iCount = 0;

	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		if(GetMajorCivApproach((PlayerTypes) iMajorLoop,                       false) == eApproach)
		{
			iCount++;
		}
	}

	return iCount;
}



void CvDiplomacyAI::DoUpdateMinorCivApproaches()
{
	int iPlayerLoop;
	PlayerTypes eLoopPlayer;


	for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		m_paeApproachScratchPad[iPlayerLoop] = -1;

		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(eLoopPlayer).isMinorCiv())
		{
			if(IsPlayerValid(eLoopPlayer))
			{
				m_paeApproachScratchPad[eLoopPlayer] = GetMinorCivApproach(eLoopPlayer);
				SetMinorCivApproach(eLoopPlayer, NO_MINOR_CIV_APPROACH);
			}
		}
	}

	int iHighestWeight;

	CvWeightedVector<PlayerTypes, MAX_MINOR_CIVS, true> vePlayerApproachWeights;
	int iPlayerVectorIndex;


	for(iPlayerLoop = MAX_MAJOR_CIVS; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			iHighestWeight = GetPlayer()->GetProximityToPlayer(eLoopPlayer);
			vePlayerApproachWeights.push_back(eLoopPlayer, iHighestWeight);
		}
	}

	MinorCivApproachTypes eApproach;
	MinorCivApproachTypes eLastTurnApproach;


	if(vePlayerApproachWeights.size() > 0)
	{
		vePlayerApproachWeights.SortItems();


		for(iPlayerVectorIndex = 0; iPlayerVectorIndex < (int) vePlayerApproachWeights.size(); iPlayerVectorIndex++)
		{
			eLoopPlayer = (PlayerTypes) vePlayerApproachWeights.GetElement(iPlayerVectorIndex);


			eApproach = GetBestApproachTowardsMinorCiv(eLoopPlayer,                                        iHighestWeight,                         true,          true);

			eLastTurnApproach = (MinorCivApproachTypes) m_paeApproachScratchPad[eLoopPlayer];

			CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Invalid MinorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

			SetMinorCivApproach(eLoopPlayer, eApproach);
		}
	}



#ifndef FINAL_RELEASE
	for(iPlayerLoop = 0; iPlayerLoop < MAX_MINOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(eLoopPlayer).isMinorCiv())
		{
			if(IsPlayerValid(eLoopPlayer))
			{
				if(GetMinorCivApproach(eLoopPlayer) == NO_MINOR_CIV_APPROACH)
					CvAssertMsg(false, "DIPLOMACY_AI: Invalid MinorCivApproachType. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
			}
		}
	}
#endif
}



MinorCivApproachTypes CvDiplomacyAI::GetBestApproachTowardsMinorCiv(PlayerTypes ePlayer, int& iHighestWeight, bool bLookAtOtherPlayers, bool bLog)
{
	CvAssertMsg(ePlayer >= MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	MinorCivApproachTypes eApproach;

	FStaticVector< int, 128, true, c_eCiv5GameplayDLL > viApproachWeights;

	int iApproachLoop;

	viApproachWeights.clear();

	for(iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
	{
		viApproachWeights.push_back(0);
	}





	viApproachWeights[MINOR_CIV_APPROACH_IGNORE] +=       GC.getMINOR_APPROACH_IGNORE_DEFAULT();







	CvAssertMsg(m_paeApproachScratchPad[ePlayer] >= NO_MINOR_CIV_APPROACH && m_paeApproachScratchPad[ePlayer] < NUM_MINOR_CIV_APPROACHES, "m_paeApproachScratchPad[iScratchPadID] is out of bounds for minor civ approaches. Show Jon!");
	if(m_paeApproachScratchPad[ePlayer] < NO_MINOR_CIV_APPROACH || m_paeApproachScratchPad[ePlayer] >= NUM_MINOR_CIV_APPROACHES)
	{
		return NO_MINOR_CIV_APPROACH;
	}

	MinorCivApproachTypes eOldApproach = (MinorCivApproachTypes) m_paeApproachScratchPad[ePlayer];
	viApproachWeights[eOldApproach] +=       GC.getMINOR_APPROACH_BIAS_FOR_CURRENT();


	if(eOldApproach == MINOR_CIV_APPROACH_CONQUEST)
	{

		viApproachWeights[MINOR_CIV_APPROACH_IGNORE] +=       GC.getMINOR_APPROACH_IGNORE_CURRENTLY_WAR();


		WarStateTypes eWarState = GetWarState(ePlayer);
		if(eWarState > WAR_STATE_STALEMATE)
		{
			viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=       GC.getAPPROACH_WAR_CURRENTLY_WAR();
		}
	}

	else if(eOldApproach == MINOR_CIV_APPROACH_PROTECTIVE)
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=         GC.getMINOR_APPROACH_WAR_CURRENTLY_PROTECTIVE();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] +=        GC.getMINOR_APPROACH_PROTECTIVE_CURRENTLY_PROTECTIVE();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -10;
	}


	if(IsAtWar(ePlayer))
	{
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] +=         GC.getMINOR_APPROACH_PROTECTIVE_CURRENTLY_WAR();
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] +=        GC.getMINOR_APPROACH_FRIENDLY_CURRENTLY_WAR();
	}





	int iNumWeLack = GET_PLAYER(ePlayer).GetMinorCivAI()->GetNumResourcesMajorLacks(GetPlayer()->GetID());
	if(iNumWeLack > 0)
	{
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += (iNumWeLack*       GC.getMINOR_APPROACH_FRIENDLY_RESOURCES());
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += (iNumWeLack*       GC.getMINOR_APPROACH_PROTECTIVE_RESOURCES());
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += (iNumWeLack                                                           );
	}





	if(GET_PLAYER(ePlayer).GetMinorCivAI()->IsFriends(GetPlayer()->GetID()))
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=          GC.getMINOR_APPROACH_WAR_FRIENDS();
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] +=       GC.getMINOR_APPROACH_FRIENDLY_FRIENDS();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] +=       GC.getMINOR_APPROACH_PROTECTIVE_FRIENDS();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] -= 10;
	}





	if(GET_PLAYER(ePlayer).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
	{
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += 2;
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -1;
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += -2;
	}





	for(iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
	{
		viApproachWeights[iApproachLoop] += GetPersonalityMinorCivApproachBias((MinorCivApproachTypes) iApproachLoop);
	}

	bool bIsGoodWarTarget = false;
	bool bCheckIfGoodWarTarget = false;





	if(IsGoingForWorldConquest())
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=        GC.getMINOR_APPROACH_WAR_CONQUEST_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] +=         GC.getMINOR_APPROACH_PROTECTIVE_CONQUEST_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] +=        GC.getMINOR_APPROACH_FRIENDLY_CONQUEST_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 8;


		if(GetPlayer()->GetProximityToPlayer(ePlayer) == PLAYER_PROXIMITY_NEIGHBORS)
		{
			bIsGoodWarTarget = true;
		}
	}





	else if(IsGoingForDiploVictory())
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=         GC.getMINOR_APPROACH_WAR_DIPLO_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_IGNORE] +=         GC.getMINOR_APPROACH_IGNORE_DIPLO_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -15;


		if(GetPlayer()->GetProximityToPlayer(ePlayer) >= PLAYER_PROXIMITY_CLOSE)
			viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] +=       GC.getMINOR_APPROACH_PROTECTIVE_DIPLO_GRAND_STRATEGY_NEIGHBORS();
	}





	else if(IsGoingForCultureVictory())
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=         GC.getMINOR_APPROACH_WAR_CULTURE_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_IGNORE] +=         GC.getMINOR_APPROACH_IGNORE_CULTURE_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -10;


		if(GET_PLAYER(ePlayer).GetMinorCivAI()->GetTrait() == MINOR_CIV_TRAIT_CULTURED) 
		{
			viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] +=       GC.getMINOR_APPROACH_PROTECTIVE_CULTURE_GRAND_STRATEGY_CST();
		}
		else
		{
			bCheckIfGoodWarTarget = true;
		}
	}




	else
	{
		bCheckIfGoodWarTarget = true;
	}


	if (bCheckIfGoodWarTarget)
	{
		CvCity *pkMinorCapital = GET_PLAYER(ePlayer).getCapitalCity();
		if (pkMinorCapital)
		{
			int iMinorAreaID = pkMinorCapital->getArea();

			if (m_pPlayer->getCapitalCity())
			{
				int iMyAreaID = m_pPlayer->getCapitalCity()->getArea();
				int iPlayerLoop;
				CvTeam& kMyTeam = GET_TEAM(GetTeam());
				for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
					CvPlayer &kLoopPlayer = GET_PLAYER(eLoopPlayer);


					if (m_pPlayer->GetID() != eLoopPlayer)
					{
						if(IsPlayerValid(eLoopPlayer))
						{
							if (!kMyTeam.isAtWar(kLoopPlayer.getTeam()))
							{

								if (GetMajorCivApproach(eLoopPlayer, false) == MAJOR_CIV_APPROACH_WAR)
								{
									CvCity* pkLoopPlayerCity = kLoopPlayer.getCapitalCity();
									if (pkLoopPlayerCity)
									{
										int iLoopPlayerAreaID = pkLoopPlayerCity->getArea();

										if (iLoopPlayerAreaID == iMinorAreaID &&
											iMyAreaID == iLoopPlayerAreaID)
										{
											if (kLoopPlayer.GetProximityToPlayer(ePlayer) >= PLAYER_PROXIMITY_NEIGHBORS &&
												m_pPlayer->GetProximityToPlayer(ePlayer) >= PLAYER_PROXIMITY_NEIGHBORS)
											{
												bIsGoodWarTarget = true;
												break;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (bIsGoodWarTarget)
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=        GC.getMINOR_APPROACH_WAR_CONQUEST_GRAND_STRATEGY_NEIGHBORS();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 8;
	}




	int iBonusTraitMod = 0;
	iBonusTraitMod += (m_pPlayer->GetPlayerTraits()->GetCityStateFriendshipModifier() > 0) ? 100 : 0;
	iBonusTraitMod += (m_pPlayer->GetPlayerTraits()->GetCityStateBonusModifier() > 0) ? 100 : 0;
	iBonusTraitMod += (m_pPlayer->GetPlayerTraits()->GetCityStateCombatModifier() > 0) ? -100 : 0;
	if (iBonusTraitMod)
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] -= iBonusTraitMod;
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] -= iBonusTraitMod / 5;
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += iBonusTraitMod / 5;
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += iBonusTraitMod;
	}
	if (iBonusTraitMod >= 0 && m_pPlayer->getNumCities() <= 1)
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] -= 200;
	}



























	int iPlayerLoop;
	PlayerTypes eLoopPlayer;





	for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;


		if(ePlayer != eLoopPlayer && GET_PLAYER(ePlayer).getTeam() != GET_PLAYER(eLoopPlayer).getTeam())
		{
			if(IsPlayerValid(eLoopPlayer))
			{
				if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				{
					if(GetStateAllWars() == STATE_ALL_WARS_NEUTRAL)
						viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=        GC.getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_NEUTRAL();
					else if(GetStateAllWars() == STATE_ALL_WARS_WINNING)
						viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=        GC.getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_WINNING();
					else
						viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=         GC.getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_LOSING();
				}
			}
		}
	}






	if(bLookAtOtherPlayers)
	{
		bool bMinorCiv;


		for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;


			if(eLoopPlayer != ePlayer)
			{
				if(IsPlayerValid(eLoopPlayer))
				{
					bMinorCiv = GET_PLAYER(eLoopPlayer).isMinorCiv();


					if(GetWarGoal(eLoopPlayer) == WAR_GOAL_PREPARE ||
					        (!bMinorCiv && GetMajorCivApproach(eLoopPlayer,                       false) == MAJOR_CIV_APPROACH_WAR))
					{
						viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=         GC.getMINOR_APPROACH_WAR_PLANNING_WAR_WITH_ANOTHER_PLAYER();
					}


					if(bMinorCiv)
					{
						switch(GetMinorCivApproach(eLoopPlayer))
						{
						case MINOR_CIV_APPROACH_CONQUEST:
							viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=         GC.getMINOR_APPROACH_WAR_PLANNING_WAR_WITH_ANOTHER_PLAYER();
							break;
						case MINOR_CIV_APPROACH_PROTECTIVE:

							viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] +=        GC.getMINOR_APPROACH_PROTECTIVE_WITH_ANOTHER_PLAYER();
							break;
						case MINOR_CIV_APPROACH_BULLY:

							viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -2;
							break;
						}
					}
				}
			}
		}
	}





	switch(GetPlayer()->GetProximityToPlayer(ePlayer))
	{
	case PLAYER_PROXIMITY_NEIGHBORS:
		viApproachWeights[MINOR_CIV_APPROACH_IGNORE] +=        GC.getMINOR_APPROACH_IGNORE_PROXIMITY_NEIGHBORS();
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] +=        GC.getMINOR_APPROACH_FRIENDLY_PROXIMITY_NEIGHBORS();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] +=       GC.getMINOR_APPROACH_PROTECTIVE_PROXIMITY_NEIGHBORS();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=       GC.getMINOR_APPROACH_CONQUEST_PROXIMITY_NEIGHBORS();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 1;
		break;
	case PLAYER_PROXIMITY_CLOSE:
		viApproachWeights[MINOR_CIV_APPROACH_IGNORE] +=        GC.getMINOR_APPROACH_IGNORE_PROXIMITY_CLOSE();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] +=       GC.getMINOR_APPROACH_PROTECTIVE_PROXIMITY_CLOSE();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=        GC.getMINOR_APPROACH_CONQUEST_PROXIMITY_CLOSE();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 1;
		break;
	case PLAYER_PROXIMITY_FAR:
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] +=       GC.getMINOR_APPROACH_FRIENDLY_PROXIMITY_FAR();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=        GC.getMINOR_APPROACH_CONQUEST_PROXIMITY_FAR();
		break;
	case PLAYER_PROXIMITY_DISTANT:
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] +=       GC.getMINOR_APPROACH_FRIENDLY_PROXIMITY_DISTANT();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=         GC.getMINOR_APPROACH_CONQUEST_PROXIMITY_DISTANT();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -2;
		break;
	}





	switch(GET_PLAYER(ePlayer).GetMinorCivAI()->GetPersonality())
	{
	case MINOR_CIV_PERSONALITY_FRIENDLY:
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] +=       GC.getMINOR_APPROACH_FRIENDLY_PERSONALITY_FRIENDLY();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] +=       GC.getMINOR_APPROACH_PROTECTIVE_PERSONALITY_PROTECTIVE();
		break;
	case MINOR_CIV_PERSONALITY_NEUTRAL:
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] +=       GC.getMINOR_APPROACH_FRIENDLY_PERSONALITY_NEUTRAL();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] +=       GC.getMINOR_APPROACH_PROTECTIVE_PERSONALITY_NEUTRAL();
		break;
	case MINOR_CIV_PERSONALITY_HOSTILE:
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] +=        GC.getMINOR_APPROACH_FRIENDLY_PERSONALITY_HOSTILE();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] +=        GC.getMINOR_APPROACH_PROTECTIVE_PERSONALITY_HOSTILE();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] +=       GC.getMINOR_APPROACH_CONQUEST_PERSONALITY_HOSTILE();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 1;
		break;
	case MINOR_CIV_PERSONALITY_IRRATIONAL:
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] +=       GC.getMINOR_APPROACH_FRIENDLY_PERSONALITY_IRRATIONAL();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] +=       GC.getMINOR_APPROACH_PROTECTIVE_PERSONALITY_IRRATIONAL();
		break;
	}






	if(GET_PLAYER(ePlayer).GetMinorCivAI()->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
	{
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -2;
	}




	if(GET_PLAYER(ePlayer).GetMinorCivAI()->IsEverBulliedByMajor(GetPlayer()->GetID()))
	{
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 2;
	}




	if(GET_PLAYER(ePlayer).GetMinorCivAI()->IsActiveQuestForPlayer(GetPlayer()->GetID(), MINOR_CIV_QUEST_PLEDGE_TO_PROTECT))
	{
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += 3;
	}


	int iCurrentTradeValue = GetPlayer()->GetTrade()->GetAllTradeValueFromPlayerTimes100(YIELD_GOLD, ePlayer) / 100;
	if(iCurrentTradeValue > 0)
	{

		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += 3;
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] -= 3;
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += 5;
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] -= 5;


		int iGPT = GetPlayer()->calculateGoldRate();
		int iDeltaGPT = iGPT - iCurrentTradeValue;
		if (iGPT >= 0 && (iDeltaGPT < 0))
		{
			viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += iDeltaGPT;
		}
	}





	int iPTV = GetPlayerTargetValue(ePlayer);
	if (GetPlayer()->GetPlayerTraits()->GetCityStateCombatModifier() > 0)
	{
		iPTV++;
		iPTV = iPTV > TARGET_VALUE_SOFT ? TARGET_VALUE_SOFT : iPTV;
	}
	switch(iPTV)
	{
	case TARGET_VALUE_IMPOSSIBLE:
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] *=        GC.getMINOR_APPROACH_WAR_TARGET_IMPOSSIBLE();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] /= 100;
		break;
	case TARGET_VALUE_BAD:
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] *=        GC.getMINOR_APPROACH_WAR_TARGET_BAD();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] /= 100;
		break;
	case TARGET_VALUE_AVERAGE:
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] *=        GC.getMINOR_APPROACH_WAR_TARGET_AVERAGE();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] /= 100;
		break;
	case TARGET_VALUE_FAVORABLE:
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] *=         GC.getMINOR_APPROACH_WAR_TARGET_FAVORABLE();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] /= 100;
		break;
	case TARGET_VALUE_SOFT:
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] *=         GC.getMINOR_APPROACH_WAR_TARGET_SOFT();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] /= 100;
		break;
	default:
		break;
	}





































	int iPeaceTreatyTurn = GET_TEAM(GetTeam()).GetTurnMadePeaceTreatyWithTeam(eTeam);
	if(iPeaceTreatyTurn > -1)
	{
		int iTurnsSincePeace = GC.getGame().getElapsedGameTurns() - iPeaceTreatyTurn;
		if(iTurnsSincePeace <        GC.getTURNS_SINCE_PEACE_WEIGHT_DAMPENER())
		{
			viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] *=        GC.getAPPROACH_WAR_HAS_MADE_PEACE_BEFORE_PERCENT();
			viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] /= 100;
		}
	}




	if(BULLY_DEBUGGING)
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += 50;
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 0;
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += 0;
	}





	int iRandAmount;
	for(iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
	{

		viApproachWeights[iApproachLoop] *= 100;

		iRandAmount = viApproachWeights[iApproachLoop] *        GC.getAPPROACH_RANDOM_PERCENT();


		if(iRandAmount > 0)
		{
			iRandAmount /= 100;
			viApproachWeights[iApproachLoop] += GC.getGame().getJonRandNumVA(iRandAmount, "Randomizing Diplo AI Minor Civ Approach Weight (%d; %d)", (int)GetPlayer()->GetID(), (int)ePlayer);
		}
	}




	if(!GET_PLAYER(ePlayer).GetMinorCivAI()->CanMajorProtect(GetPlayer()->GetID()))
	{

		if (viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] > 0)
			viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] = 0;
	}




	if(!GET_TEAM(GetTeam()).canDeclareWar(GET_PLAYER(ePlayer).getTeam()))
	{
		if(!GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
		{

			if (viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] > 0)
				viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] = 0;
		}
	}
	



	if(GET_PLAYER(ePlayer).GetMinorCivAI()->GetAlly() == GetPlayer()->GetID())
	{


		if (viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] > 0)
			viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] = 0;

		if (viApproachWeights[MINOR_CIV_APPROACH_BULLY] > 0)
			viApproachWeights[MINOR_CIV_APPROACH_BULLY] = 0;
	}


	CvWeightedVector< MinorCivApproachTypes, 128 > vApproachWeightsForSorting;
	vApproachWeightsForSorting.clear();


	for(iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
	{
		vApproachWeightsForSorting.push_back((MinorCivApproachTypes) iApproachLoop, viApproachWeights[iApproachLoop]);
	}

	vApproachWeightsForSorting.SortItems();

	eApproach = vApproachWeightsForSorting.GetElement(0);
	iHighestWeight = vApproachWeightsForSorting.GetWeight(0);


	if(bLog)
	{
		LogMinorCivApproachUpdate(ePlayer, viApproachWeights.begin(), eApproach, eOldApproach);
	}

	return eApproach;
}


MinorCivApproachTypes CvDiplomacyAI::GetMinorCivApproach(PlayerTypes ePlayer) const
{

	PlayerTypes eMinor = (PlayerTypes)((int) ePlayer - MAX_MAJOR_CIVS);

	CvAssertMsg(eMinor >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eMinor < MAX_MINOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (MinorCivApproachTypes) m_paeMinorCivApproach[eMinor];
}


void CvDiplomacyAI::SetMinorCivApproach(PlayerTypes ePlayer, MinorCivApproachTypes eApproach)
{

	PlayerTypes eMinor = (PlayerTypes)((int) ePlayer - MAX_MAJOR_CIVS);

	CvAssertMsg(eMinor >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eMinor < MAX_MINOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach >= NO_MINOR_CIV_APPROACH, "DIPLOMACY_AI: Invalid MinorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach < NUM_MINOR_CIV_APPROACHES, "DIPLOMACY_AI: Invalid MinorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeMinorCivApproach[eMinor] = eApproach;
}


int CvDiplomacyAI::GetNumMinorCivApproach(MinorCivApproachTypes eApproach) const
{
	CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Invalid MinorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach < NUM_MINOR_CIV_APPROACHES, "DIPLOMACY_AI: Invalid MinorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iCount = 0;

	for(int iMinorLoop = 0; iMinorLoop < MAX_MINOR_CIVS; iMinorLoop++)
	{
		if(GetMinorCivApproach((PlayerTypes) iMinorLoop) == eApproach)
		{
			iCount++;
		}
	}

	return iCount;
}


void CvDiplomacyAI::SetWantToRouteConnectToMinor(PlayerTypes eMinor, bool bWant)
{
	int iArrayIndex = eMinor - MAX_MAJOR_CIVS;
	CvAssertMsg(iArrayIndex >= 0 && iArrayIndex < MAX_MINOR_CIVS, "DIPLOMACY_AI: Index into array is out of bounds");

	if(IsWantToRouteConnectToMinor(eMinor) != bWant)
		m_pabWantToRouteToMinor[iArrayIndex] = bWant;
}


bool CvDiplomacyAI::IsWantToRouteConnectToMinor(PlayerTypes eMinor)
{
	int iArrayIndex = eMinor - MAX_MAJOR_CIVS;
	CvAssertMsg(iArrayIndex >= 0 && iArrayIndex < MAX_MINOR_CIVS, "DIPLOMACY_AI: Index into array is out of bounds");
	return m_pabWantToRouteToMinor[iArrayIndex];
}


bool CvDiplomacyAI::IsHasActiveGoldQuest()
{

	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinor = (PlayerTypes)iMinorLoop;
		if(GET_PLAYER(eMinor).GetMinorCivAI()->IsActiveQuestForPlayer(GetPlayer()->GetID(), MINOR_CIV_QUEST_GIVE_GOLD))
			return true;
		if(GET_PLAYER(eMinor).GetMinorCivAI()->IsActiveQuestForPlayer(GetPlayer()->GetID(), MINOR_CIV_QUEST_INVEST))
			return true;
	}
	return false;
}



MajorCivApproachTypes CvDiplomacyAI::GetApproachTowardsUsGuess(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return GET_PLAYER(ePlayer).GetDiplomacyAI()->GetMajorCivApproach(GetPlayer()->GetID(),                       true);


}


void CvDiplomacyAI::SetApproachTowardsUsGuess(PlayerTypes ePlayer, MajorCivApproachTypes eApproach)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach < NUM_MAJOR_CIV_APPROACHES, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeApproachTowardsUsGuess[ePlayer] = eApproach;
}


int CvDiplomacyAI::GetApproachTowardsUsGuessCounter(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paeApproachTowardsUsGuessCounter[ePlayer];
}


void CvDiplomacyAI::SetApproachTowardsUsGuessCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting ApproachTowardsUsGuessCounter to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeApproachTowardsUsGuessCounter[ePlayer] = iValue;
}


void CvDiplomacyAI::ChangeApproachTowardsUsGuessCounter(PlayerTypes ePlayer, int iChange)
{
	SetApproachTowardsUsGuessCounter(ePlayer, GetApproachTowardsUsGuessCounter(ePlayer) + iChange);
}


void CvDiplomacyAI::DoUpdateApproachTowardsUsGuesses()
{
	PlayerTypes eLoopPlayer;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{

			if(GetApproachTowardsUsGuess(eLoopPlayer) != MAJOR_CIV_APPROACH_NEUTRAL)
			{
				ChangeApproachTowardsUsGuessCounter(eLoopPlayer, 1);

				if(GetApproachTowardsUsGuessCounter(eLoopPlayer) > 30)
				{
					SetApproachTowardsUsGuess(eLoopPlayer, MAJOR_CIV_APPROACH_NEUTRAL);
					SetApproachTowardsUsGuessCounter(eLoopPlayer, 0);
				}
			}
		}
	}
}










PlayerTypes CvDiplomacyAI::GetDemandTargetPlayer() const
{
	return (PlayerTypes) m_eDemandTargetPlayer;
}


void CvDiplomacyAI::SetDemandTargetPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= NO_PLAYER, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_eDemandTargetPlayer = ePlayer;
}


void CvDiplomacyAI::DoUpdateDemands()
{
	CvWeightedVector<PlayerTypes, MAX_MAJOR_CIVS, true> veDemandTargetPlayers;

	int iWeight;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{

			if(IsPlayerDemandAttractive(eLoopPlayer))
			{
				iWeight = GC.getGame().getJonRandNumVA(100, "DIPLOMACY_AI: Random weight for player to make demand of. (%d; %d)", (int)GetPlayer()->GetID(), (int)eLoopPlayer);
				veDemandTargetPlayers.push_back(eLoopPlayer, iWeight);
			}
		}
	}

	bool bCancelDemand;


	if(veDemandTargetPlayers.size() > 0)
	{
		bCancelDemand = false;


		if(GetDemandTargetPlayer() == NO_PLAYER)
		{
			veDemandTargetPlayers.SortItems();

			int iChanceOfDemand =         GC.getDEMAND_RAND();

			if(iChanceOfDemand > GC.getGame().getJonRandNum(100, "DIPLOMACY_AI: Should AI make demand of player it's hostile towards?"))
			{
				DoStartDemandProcess(veDemandTargetPlayers.GetElement(0));
			}
		}
	}

	else
	{
		bCancelDemand = true;
	}


	if(GetDemandTargetPlayer() != NO_PLAYER)
	{
		if(!IsPlayerDemandAttractive(GetDemandTargetPlayer()))
		{
			bCancelDemand = true;
		}
	}


	if(bCancelDemand)
	{
		DoCancelHaltDemandProcess();
	}


	DoTestDemandReady();
}


void CvDiplomacyAI::DoStartDemandProcess(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	SetDemandTargetPlayer(ePlayer);

	CvAIOperation* pOperation = GetPlayer()->GetMilitaryAI()->GetShowOfForceOperation((ePlayer));


	if(pOperation == NULL && !IsMusteringForAttack(ePlayer))
	{
		if(!GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
		{
			if(GET_TEAM(GetTeam()).canDeclareWar(GET_PLAYER(ePlayer).getTeam()))
			{
				GetPlayer()->GetMilitaryAI()->RequestShowOfForce(ePlayer);
				SetWarGoal(ePlayer, WAR_GOAL_DEMAND);
			}
		}
	}
}


void CvDiplomacyAI::DoCancelHaltDemandProcess()
{
	PlayerTypes eDemandTarget = GetDemandTargetPlayer();


	if(eDemandTarget != NO_PLAYER)
	{


		if(GetWarGoal(eDemandTarget) == WAR_GOAL_DEMAND)
		{
			SetWarGoal(eDemandTarget, NO_WAR_GOAL_TYPE);


			CvAIOperation* pOperation = GetPlayer()->GetMilitaryAI()->GetShowOfForceOperation(eDemandTarget);
			if(pOperation != NULL)
			{
				pOperation->Kill(AI_ABORT_DIPLO_OPINION_CHANGE);
				SetMusteringForAttack(eDemandTarget, false);
			}
		}

		SetDemandTargetPlayer(NO_PLAYER);
		SetDemandReady(false);
	}
}


void CvDiplomacyAI::DoTestDemandReady()
{
	PlayerTypes eDemandTarget = GetDemandTargetPlayer();


	if(eDemandTarget != NO_PLAYER)
	{
		if(GetWarGoal(eDemandTarget) == WAR_GOAL_DEMAND)
		{
			CvAIOperation* pOperation = GetPlayer()->GetMilitaryAI()->GetShowOfForceOperation(eDemandTarget);

			if(pOperation)
			{
				if(!IsAtWar(eDemandTarget))
				{

					if(IsMusteringForAttack(eDemandTarget) || (pOperation != NULL && pOperation->PercentFromMusterPointToTarget() >= 85))
					{
						SetMusteringForAttack(eDemandTarget, false);

						SetDemandReady(true);
					}
				}
			}
		}
	}
}


bool CvDiplomacyAI::IsDemandReady()
{
	return m_bDemandReady;
}


void CvDiplomacyAI::SetDemandReady(bool bValue)
{
	m_bDemandReady = bValue;
}


bool CvDiplomacyAI::IsPlayerDemandAttractive(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(GetMajorCivApproach(ePlayer,                       false) != MAJOR_CIV_APPROACH_HOSTILE)
	{
		return false;
	}


	if(GetPlayer()->GetProximityToPlayer(ePlayer) < PLAYER_PROXIMITY_FAR)
	{
		return false;
	}


	if(GetPlayerTargetValue(ePlayer) < TARGET_VALUE_AVERAGE)
	{
		return false;
	}


	if(GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_DEMAND) < 40)
	{
		return false;
	}


	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(GetWarGoal(eLoopPlayer) == WAR_GOAL_PREPARE)
		{
			return false;
		}

		if(IsAtWar(eLoopPlayer))
		{
			return false;
		}
	}


	if(GetPlayer()->getCapitalCity() != NULL)
	{
		if(GET_PLAYER(ePlayer).getCapitalCity() != NULL)
		{
			if(GetPlayer()->getCapitalCity()->getArea() == GET_PLAYER(ePlayer).getCapitalCity()->getArea())
			{
				return true;
			}
		}
	}

	return false;
}











bool CvDiplomacyAI::IsMakeRequest(PlayerTypes ePlayer, CvDeal* pDeal, bool& bRandPassed)
{
	bool bFriendly = GetMajorCivApproach(ePlayer,                       true) == MAJOR_CIV_APPROACH_FRIENDLY;

	if(bFriendly && IsDoFAccepted(ePlayer))
	{

		bool bWantsSomething = false;

		int iWeightBias = 0;


		if(!bWantsSomething)
			bWantsSomething = IsLuxuryRequest(ePlayer, pDeal, iWeightBias);

		if(!bWantsSomething)
			bWantsSomething = IsGoldRequest(ePlayer, pDeal, iWeightBias);

		if(bWantsSomething)
		{

			int iRand = GC.getGame().getJonRandNumVA(100, "Diplomacy AI: Friendly civ request roll. (%d; %d)", (int)GetPlayer()->GetID(), (int)ePlayer);

			iRand += iWeightBias;

			if(iRand >= 67)
			{
				bRandPassed = true;
				return true;
			}
			else
			{
				bRandPassed = false;
				return false;
			}
		}
	}

	return false;
}


bool CvDiplomacyAI::IsLuxuryRequest(PlayerTypes ePlayer, CvDeal* pDeal, int& iWeightBias)
{
	iWeightBias = 0;

	ResourceTypes eLuxuryToAskFor = NO_RESOURCE;

#ifdef AUI_WARNING_FIXES
	uint iResourceLoop;
#else
	int iResourceLoop;
#endif


	for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);

		CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if(pkResourceInfo)
		{

			if(pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY)
				continue;


			if(GET_PLAYER(ePlayer).getNumResourceAvailable(eResource,                    false) < 2)
				continue;


			if(!pDeal->IsPossibleToTradeItem(ePlayer, GetPlayer()->GetID(), TRADE_ITEM_RESOURCES, eResource, 1))
				continue;

			eLuxuryToAskFor = eResource;
			break;
		}
	}


	if(eLuxuryToAskFor == NO_RESOURCE)
		return false;


	for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);

		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if(pkResource)
		{

			if(pkResource->getResourceUsage() != RESOURCEUSAGE_LUXURY)
				continue;


			if(GetPlayer()->getNumResourceAvailable(eResource,                    false) < 2)
				continue;


			if(!pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), ePlayer, TRADE_ITEM_RESOURCES, eResource, 1))
				continue;


			return false;
		}
	}


	if(GetPlayer()->IsEmpireUnhappy())
		iWeightBias += 25;


	pDeal->AddResourceTrade(ePlayer, eLuxuryToAskFor, 1, GC.getGame().GetDealDuration());

	return true;
}


bool CvDiplomacyAI::IsGoldRequest(PlayerTypes ePlayer, CvDeal* pDeal, int& iWeightBias)
{
	iWeightBias = 0;

	int iOurGold = GetPlayer()->GetTreasury()->GetGold();
	int iOurGPT = GetPlayer()->calculateGoldRate();
	int iOurExpenses = GetPlayer()->GetTreasury()->CalculateInflatedCosts();
	int iOurGrossIncome = iOurGPT + iOurExpenses;


	if(iOurExpenses == 0)
		return false;


	if(iOurGold > 100)
		return false;


	if(iOurGrossIncome * 100 / iOurExpenses > 135)
		return false;

	int iTheirGold = GET_PLAYER(ePlayer).GetTreasury()->GetGold();
	int iTheirGPT = GET_PLAYER(ePlayer).calculateGoldRate();
	int iTheirExpenses = GET_PLAYER(ePlayer).GetTreasury()->CalculateInflatedCosts();
	int iTheirGrossIncome = iTheirGPT + iTheirExpenses;


	if(iTheirExpenses != 0)
	{

		if(iTheirGrossIncome * 100 / iTheirExpenses < 135)
			return false;
	}
	else if(iTheirGPT <= iOurGPT)
	{
		return false;
	}


	if(iOurGPT < 0)
		iWeightBias += 25;


	int iGoldToAskFor = iTheirGPT * GC.getGame().GetDealDuration() / 5;
	int iGPTToAskFor = 0;

	if(iGoldToAskFor > iTheirGold)
	{
		iGoldToAskFor = 0;
		iGPTToAskFor = max(1, iTheirGPT / 6);
	}


	if(iGoldToAskFor > 0)
		pDeal->AddGoldTrade(ePlayer, iGoldToAskFor);
	else if(iGPTToAskFor > 0)
		pDeal->AddGoldPerTurnTrade(ePlayer, iGPTToAskFor, GC.getGame().GetDealDuration());

	return true;
}


bool CvDiplomacyAI::IsEmbassyExchangeAcceptable(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);

	switch(eApproach)
	{
	case MAJOR_CIV_APPROACH_WAR:
	case MAJOR_CIV_APPROACH_HOSTILE:
	case MAJOR_CIV_APPROACH_GUARDED:
		return false;
	case MAJOR_CIV_APPROACH_DECEPTIVE:
	case MAJOR_CIV_APPROACH_AFRAID:
	case MAJOR_CIV_APPROACH_FRIENDLY:
	case MAJOR_CIV_APPROACH_NEUTRAL:
		return true;
	}

	return false;
}


bool CvDiplomacyAI::WantsEmbassyAtPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");



	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE ||
	        eApproach == MAJOR_CIV_APPROACH_WAR)
	{
		return false;
	}

	return true;
}


bool CvDiplomacyAI::IsWantsOpenBordersWithPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	AIGrandStrategyTypes eCultureStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
#ifdef AUI_WARNING_FIXES
	const CvPlayerCulture* pPlayerCulture = m_pPlayer->GetCulture();
	if (eCultureStrategy != NO_AIGRANDSTRATEGY && m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eCultureStrategy && pPlayerCulture->GetTourism() > 0)
	{

		if (pPlayerCulture->GetCivLowestInfluence(false                      ) == ePlayer)
		{
			return true;
		}


		if (pPlayerCulture->GetNumCivsToBeInfluentialOn() <= m_pPlayer->GetCulture()->GetNumCivsInfluentialOn())
		{
			if (pPlayerCulture->GetInfluenceLevel(ePlayer) < INFLUENCE_LEVEL_INFLUENTIAL)
#else
	if (eCultureStrategy != NO_AIGRANDSTRATEGY && m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eCultureStrategy && m_pPlayer->GetCulture()->GetTourism() > 0 )
	{

		if (m_pPlayer->GetCulture()->GetCivLowestInfluence(false                      ) == ePlayer)
		{
			return true;
		}


		if (m_pPlayer->GetCulture()->GetNumCivsToBeInfluentialOn() <= m_pPlayer->GetCulture()->GetNumCivsInfluentialOn())
		{
			if (m_pPlayer->GetCulture()->GetInfluenceLevel(ePlayer) < INFLUENCE_LEVEL_INFLUENTIAL)
#endif
			{
				return true;
			}
		}
	}

	if(GetPlayer()->GetProximityToPlayer(ePlayer) != PLAYER_PROXIMITY_NEIGHBORS)
	{
		return false;
	}

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE || eApproach == MAJOR_CIV_APPROACH_GUARDED || eApproach == MAJOR_CIV_APPROACH_AFRAID)
	{
		return false;
	}

	switch(GetPlayerMilitaryStrengthComparedToUs(ePlayer))
	{
	case STRENGTH_IMMENSE:
	case STRENGTH_POWERFUL:
	case STRENGTH_STRONG:
		return false;
	}

	return true;
}


bool CvDiplomacyAI::IsOpenBordersExchangeAcceptable(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);

	if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
		return true;
	else if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
		return true;

	return false;
}








void CvDiplomacyAI::DoMakePeaceWithMinors()
{
	if((int)m_eTargetPlayer >= (int)DIPLO_FIRST_PLAYER)
		return;

	PlayerTypes eLoopPlayer;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			if(GET_PLAYER(eLoopPlayer).isMinorCiv())
			{
				if(IsAtWar(eLoopPlayer))
				{
					if(IsWantsPeaceWithPlayer(eLoopPlayer) && GET_TEAM(GetPlayer()->getTeam()).GetNumTurnsLockedIntoWar(GET_PLAYER(eLoopPlayer).getTeam()) == 0)
					{
						if(!GET_PLAYER(eLoopPlayer).GetMinorCivAI()->IsPeaceBlocked(GetPlayer()->getTeam()))
						{
							GET_TEAM(GetTeam()).makePeace(GET_PLAYER(eLoopPlayer).getTeam());

							LogPeaceMade(eLoopPlayer);
						}
					}
				}
			}
		}
	}
}


void CvDiplomacyAI::DoUpdatePeaceTreatyWillingness()
{
	PeaceTreatyTypes eTreatyWillingToOffer, eTreatyWillingToAccept;
	int iWillingToOfferScore, iWillingToAcceptScore;
	WarProjectionTypes eWarProjection;


	PlayerTypes eLoopPlayer;
	TeamTypes eLoopTeam;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

			eTreatyWillingToOffer = NO_PEACE_TREATY_TYPE;
			eTreatyWillingToAccept = NO_PEACE_TREATY_TYPE;
			iWillingToOfferScore = 0;
			iWillingToAcceptScore = 0;

			if(IsAtWar(eLoopPlayer))
			{

				if(GET_PLAYER(eLoopPlayer).isHuman())
				{
					if(!IsWillingToMakePeaceWithHuman(eLoopPlayer))
					{
						SetTreatyWillingToOffer(eLoopPlayer, NO_PEACE_TREATY_TYPE);
						SetTreatyWillingToAccept(eLoopPlayer, NO_PEACE_TREATY_TYPE);

						continue;
					}
				}


				if(GetWarGoal(eLoopPlayer) != WAR_GOAL_CONQUEST)
				{
					eWarProjection = GetWarProjection(eLoopPlayer);




					{

						switch(eWarProjection)
						{
						case WAR_PROJECTION_DESTRUCTION:
							iWillingToOfferScore +=         GC.getPEACE_WILLINGNESS_OFFER_PROJECTION_DESTRUCTION();
							break;
						case WAR_PROJECTION_DEFEAT:
							iWillingToOfferScore +=        GC.getPEACE_WILLINGNESS_OFFER_PROJECTION_DEFEAT();
							break;
						case WAR_PROJECTION_STALEMATE:
							iWillingToOfferScore +=        GC.getPEACE_WILLINGNESS_OFFER_PROJECTION_STALEMATE();
							break;
						case WAR_PROJECTION_UNKNOWN:
							iWillingToOfferScore +=       GC.getPEACE_WILLINGNESS_OFFER_PROJECTION_UNKNOWN();
							break;
						case WAR_PROJECTION_GOOD:
							iWillingToOfferScore +=         GC.getPEACE_WILLINGNESS_OFFER_PROJECTION_GOOD();
							break;
						case WAR_PROJECTION_VERY_GOOD:
							iWillingToOfferScore +=         GC.getPEACE_WILLINGNESS_OFFER_PROJECTION_VERY_GOOD();
							break;
						default:
							break;
						}


						switch(GetWarDamageLevel(eLoopPlayer))
						{
						case WAR_DAMAGE_LEVEL_NONE:
							iWillingToOfferScore +=       GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_NONE();
							break;
						case WAR_DAMAGE_LEVEL_MINOR:
							iWillingToOfferScore +=        GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MINOR();
							break;
						case WAR_DAMAGE_LEVEL_MAJOR:
							iWillingToOfferScore +=        GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MAJOR();
							break;
						case WAR_DAMAGE_LEVEL_SERIOUS:
							iWillingToOfferScore +=        GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_SERIOUS();
							break;
						case WAR_DAMAGE_LEVEL_CRIPPLED:
							iWillingToOfferScore +=        GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_CRIPPLED();
							break;
						default:
							break;
						}


						switch(GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->GetWarDamageLevel(GetPlayer()->GetID()))
						{
						case WAR_DAMAGE_LEVEL_NONE:
							iWillingToOfferScore -=       GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_NONE();
							break;
						case WAR_DAMAGE_LEVEL_MINOR:
							iWillingToOfferScore -=        GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MINOR();
							break;
						case WAR_DAMAGE_LEVEL_MAJOR:
							iWillingToOfferScore -=        GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MAJOR();
							break;
						case WAR_DAMAGE_LEVEL_SERIOUS:
							iWillingToOfferScore -=        GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_SERIOUS();
							break;
						case WAR_DAMAGE_LEVEL_CRIPPLED:
							iWillingToOfferScore -=        GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_CRIPPLED();
							break;
						default:
							break;
						}


						if(iWillingToOfferScore >=         GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_UN_SURRENDER())
							eTreatyWillingToOffer = PEACE_TREATY_UNCONDITIONAL_SURRENDER;
						else if(iWillingToOfferScore >=         GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_CAPITULATION())
							eTreatyWillingToOffer = PEACE_TREATY_CAPITULATION;
						else if(iWillingToOfferScore >=         GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_CESSION())
							eTreatyWillingToOffer = PEACE_TREATY_CESSION;
						else if(iWillingToOfferScore >=        GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_SURRENDER())
							eTreatyWillingToOffer = PEACE_TREATY_SURRENDER;
						else if(iWillingToOfferScore >=        GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_SUBMISSION())
							eTreatyWillingToOffer = PEACE_TREATY_SUBMISSION;
						else if(iWillingToOfferScore >=        GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_BACKDOWN())
							eTreatyWillingToOffer = PEACE_TREATY_BACKDOWN;
						else if(iWillingToOfferScore >=        GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_SETTLEMENT())
							eTreatyWillingToOffer = PEACE_TREATY_SETTLEMENT;
						else if(iWillingToOfferScore >=        GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_ARMISTICE())
							eTreatyWillingToOffer = PEACE_TREATY_ARMISTICE;
						else
							eTreatyWillingToOffer = PEACE_TREATY_WHITE_PEACE;


						if(GET_TEAM(eLoopTeam).IsHasBrokenPeaceTreaty())
						{
							if(eTreatyWillingToOffer > PEACE_TREATY_WHITE_PEACE)
								eTreatyWillingToOffer = PEACE_TREATY_WHITE_PEACE;
						}
					}




					switch(eWarProjection)
					{
					case WAR_PROJECTION_DESTRUCTION:
						iWillingToAcceptScore +=         GC.getPEACE_WILLINGNESS_ACCEPT_PROJECTION_DESTRUCTION();
						break;
					case WAR_PROJECTION_DEFEAT:
						iWillingToAcceptScore +=         GC.getPEACE_WILLINGNESS_ACCEPT_PROJECTION_DEFEAT();
						break;
					case WAR_PROJECTION_STALEMATE:
						iWillingToAcceptScore +=         GC.getPEACE_WILLINGNESS_ACCEPT_PROJECTION_STALEMATE();
						break;
					case WAR_PROJECTION_UNKNOWN:
						iWillingToAcceptScore +=       GC.getPEACE_WILLINGNESS_ACCEPT_PROJECTION_UNKNOWN();
						break;
					case WAR_PROJECTION_GOOD:
						iWillingToAcceptScore +=        GC.getPEACE_WILLINGNESS_ACCEPT_PROJECTION_GOOD();
						break;
					case WAR_PROJECTION_VERY_GOOD:
						iWillingToAcceptScore +=         GC.getPEACE_WILLINGNESS_ACCEPT_PROJECTION_VERY_GOOD();
						break;
					default:
						break;
					}


					switch(GetPlayerTargetValue(eLoopPlayer))
					{
					case TARGET_VALUE_IMPOSSIBLE:
						iWillingToAcceptScore +=         GC.getPEACE_WILLINGNESS_ACCEPT_TARGET_IMPOSSIBLE();
						break;
					case TARGET_VALUE_BAD:
						iWillingToAcceptScore +=         GC.getPEACE_WILLINGNESS_ACCEPT_TARGET_BAD();
						break;
					case TARGET_VALUE_AVERAGE:
						iWillingToAcceptScore +=       GC.getPEACE_WILLINGNESS_ACCEPT_TARGET_AVERAGE();
						break;
					case TARGET_VALUE_FAVORABLE:
						iWillingToAcceptScore +=        GC.getPEACE_WILLINGNESS_ACCEPT_TARGET_FAVORABLE();
						break;
					case TARGET_VALUE_SOFT:
						iWillingToAcceptScore +=        GC.getPEACE_WILLINGNESS_ACCEPT_TARGET_SOFT();
						break;
					default:
						break;
					}


					if(iWillingToAcceptScore >=         GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_UN_SURRENDER())
						eTreatyWillingToAccept = PEACE_TREATY_UNCONDITIONAL_SURRENDER;
					else if(iWillingToAcceptScore >=         GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_CAPITULATION())
						eTreatyWillingToAccept = PEACE_TREATY_CAPITULATION;
					else if(iWillingToAcceptScore >=        GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_CESSION())
						eTreatyWillingToAccept = PEACE_TREATY_CESSION;
					else if(iWillingToAcceptScore >=        GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SURRENDER())
						eTreatyWillingToAccept = PEACE_TREATY_SURRENDER;
					else if(iWillingToAcceptScore >=        GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SUBMISSION())
						eTreatyWillingToAccept = PEACE_TREATY_SUBMISSION;
					else if(iWillingToAcceptScore >=        GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_BACKDOWN())
						eTreatyWillingToAccept = PEACE_TREATY_BACKDOWN;
					else if(iWillingToAcceptScore >=        GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SETTLEMENT())
						eTreatyWillingToAccept = PEACE_TREATY_SETTLEMENT;
					else if(iWillingToAcceptScore >=        GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_ARMISTICE())
						eTreatyWillingToAccept = PEACE_TREATY_ARMISTICE;
					else
						eTreatyWillingToAccept = PEACE_TREATY_WHITE_PEACE;


					if (GetStateAllWars() == STATE_ALL_WARS_LOSING)
					{
						eTreatyWillingToAccept = PEACE_TREATY_WHITE_PEACE;
					}
				}
#ifdef NQM_AI_GIMP_ALWAYS_WHITE_PEACE
				if ((GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_ALWAYS_WHITE_PEACE")) && GET_PLAYER(eLoopPlayer).isHuman())
				{
					if (eTreatyWillingToOffer < PEACE_TREATY_WHITE_PEACE)
						eTreatyWillingToOffer = PEACE_TREATY_WHITE_PEACE;
					if (eTreatyWillingToAccept != PEACE_TREATY_WHITE_PEACE)
						eTreatyWillingToAccept = PEACE_TREATY_WHITE_PEACE;
				}
#endif
			}

			SetTreatyWillingToOffer(eLoopPlayer, eTreatyWillingToOffer);
			SetTreatyWillingToAccept(eLoopPlayer, eTreatyWillingToAccept);
		}
	}
}


bool CvDiplomacyAI::IsWillingToMakePeaceWithHuman(PlayerTypes ePlayer)
{
	CvPlayer& kHumanPlayer = GET_PLAYER(ePlayer);
	if (kHumanPlayer.isHuman())
	{
#ifdef NQM_AI_GIMP_ALWAYS_WHITE_PEACE
		if ((GC.getGame().isOption("GAMEOPTION_AI_TWEAKS") || GC.getGame().isOption("GAMEOPTION_AI_GIMP_ALWAYS_WHITE_PEACE")))
			return true;
#endif
		bool bWillMakePeace = GetPlayerNumTurnsAtWar(ePlayer) >= 5;


		if(!GET_TEAM(m_pPlayer->getTeam()).canChangeWarPeace(kHumanPlayer.getTeam()))
		{
			return false;
		}


		if(GET_TEAM(GetPlayer()->getTeam()).GetNumTurnsLockedIntoWar(kHumanPlayer.getTeam()) > 1)
			return false;
		if(GET_TEAM(kHumanPlayer.getTeam()).GetNumTurnsLockedIntoWar(GetPlayer()->getTeam()) > 1)
			return false;

		return bWillMakePeace;
	}
	return true;
}


PeaceTreatyTypes CvDiplomacyAI::GetTreatyWillingToOffer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (PeaceTreatyTypes) m_paePeaceTreatyWillingToOffer[ePlayer];
}


void CvDiplomacyAI::SetTreatyWillingToOffer(PlayerTypes ePlayer, PeaceTreatyTypes eTreaty)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTreaty >= NO_PEACE_TREATY_TYPE, "DIPLOMACY_AI: Invalid PeaceTreatyType Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTreaty < NUM_PEACE_TREATY_TYPES, "DIPLOMACY_AI: Invalid PeaceTreatyType Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePeaceTreatyWillingToOffer[ePlayer] = eTreaty;
}


PeaceTreatyTypes CvDiplomacyAI::GetTreatyWillingToAccept(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (PeaceTreatyTypes) m_paePeaceTreatyWillingToAccept[ePlayer];
}


void CvDiplomacyAI::SetTreatyWillingToAccept(PlayerTypes ePlayer, PeaceTreatyTypes eTreaty)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTreaty >= NO_PEACE_TREATY_TYPE, "DIPLOMACY_AI: Invalid PeaceTreatyType Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTreaty < NUM_PEACE_TREATY_TYPES, "DIPLOMACY_AI: Invalid PeaceTreatyType Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePeaceTreatyWillingToAccept[ePlayer] = eTreaty;
}


bool CvDiplomacyAI::IsWantsPeaceWithPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iRequestPeaceTurnThreshold =       GC.getREQUEST_PEACE_TURN_THRESHOLD();

	if(!GET_TEAM(m_pPlayer->getTeam()).canChangeWarPeace(GET_PLAYER(ePlayer).getTeam()))
	{
		return false;
	}

	if(GetWantPeaceCounter(ePlayer) >= iRequestPeaceTurnThreshold)
	{
		return true;
	}

	return false;
}



int CvDiplomacyAI::GetWantPeaceCounter(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarGoalTypes) m_paeWantPeaceCounter[ePlayer];
}


void CvDiplomacyAI::SetWantPeaceCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting WantPeaceCounter to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeWantPeaceCounter[ePlayer] = iValue;
}


void CvDiplomacyAI::ChangeWantPeaceCounter(PlayerTypes ePlayer, int iChange)
{
	SetWantPeaceCounter(ePlayer, GetWantPeaceCounter(ePlayer) + iChange);
}










void CvDiplomacyAI::DoMakeWarOnPlayer(PlayerTypes eTargetPlayer)
{
	CvAIOperation* pOperation;
	bool bWantToAttack = false;
	bool bDeclareWar = false;

	if(!IsPlayerValid(eTargetPlayer))
		return;

	if(GetWarGoal(eTargetPlayer) == WAR_GOAL_DEMAND)
		return;

	if(IsAtWar(eTargetPlayer))
		return;

	bool bAtWarWithAtLeastOneMajor = MilitaryAIHelpers::IsTestStrategy_AtWar(m_pPlayer);


	if(GET_PLAYER(eTargetPlayer).isMinorCiv())
	{
		bWantToAttack = !bAtWarWithAtLeastOneMajor && (GetMinorCivApproach(eTargetPlayer) == MINOR_CIV_APPROACH_CONQUEST);
		pOperation = GetPlayer()->GetMilitaryAI()->GetCityStateAttackOperation(eTargetPlayer);
	}

	else
	{
		MajorCivApproachTypes eApproach = GetMajorCivApproach(eTargetPlayer,                       false);
		bWantToAttack = (eApproach == MAJOR_CIV_APPROACH_WAR || (eApproach == MAJOR_CIV_APPROACH_DECEPTIVE && IsGoingForWorldConquest()));
		bWantToAttack = bWantToAttack && !bAtWarWithAtLeastOneMajor;
		pOperation = GetPlayer()->GetMilitaryAI()->GetSneakAttackOperation(eTargetPlayer);
	}


	if(pOperation == NULL && !IsMusteringForAttack(eTargetPlayer))
	{
		if(!GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eTargetPlayer).getTeam()))
		{
			if(GET_TEAM(GetTeam()).canDeclareWar(GET_PLAYER(eTargetPlayer).getTeam()))
			{

				if(bWantToAttack)
				{
					SetWarGoal(eTargetPlayer, WAR_GOAL_PREPARE);


					if(GET_PLAYER(eTargetPlayer).isMinorCiv())
						GetPlayer()->GetMilitaryAI()->RequestCityStateAttack(eTargetPlayer);

					else
						GetPlayer()->GetMilitaryAI()->RequestSneakAttack(eTargetPlayer);
				}
			}
		}
	}

	else
	{

		if(bWantToAttack)
		{
			if(!IsAtWar(eTargetPlayer))
			{
				if(GET_TEAM(GetTeam()).canDeclareWar(GET_PLAYER(eTargetPlayer).getTeam()))
				{

					if(IsMusteringForAttack(eTargetPlayer) || (pOperation != NULL && pOperation->PercentFromMusterPointToTarget() >= 85))
					{
						bDeclareWar = true;
						SetMusteringForAttack(eTargetPlayer, false);
					}
				}
			}
		}


		else
		{
			if(pOperation != NULL)
			{
				pOperation->Kill(AI_ABORT_DIPLO_OPINION_CHANGE);
				SetWarGoal(eTargetPlayer, NO_WAR_GOAL_TYPE);
				SetMusteringForAttack(eTargetPlayer, false);
			}
		}


		if(bDeclareWar)
		{
			DeclareWar(eTargetPlayer);
		}
	}
}


void CvDiplomacyAI::MakeWar()
{
	CvWeightedVector<int> playerList;
	int iWeight;

	if((int)m_eTargetPlayer >= (int)DIPLO_FIRST_PLAYER)
	{
		DoMakeWarOnPlayer(m_eTargetPlayer);
	}
	else
	{
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			PlayerTypes eTarget = (PlayerTypes)iPlayerLoop;
#ifdef AUI_DIPLOMACY_AI_FIX_WAR_DECLARATION_IN_MULTIPLAYER
			if (IsPlayerValid(eTarget))
#else
			if(IsValidUIDiplomacyTarget(eTarget) && IsPlayerValid(eTarget))
#endif
			{
				iWeight = (int)GetWarProjection(eTarget) + 1;


				iWeight *= (1 + (int)GetPlayer()->GetProximityToPlayer(eTarget));
				iWeight *= (1 + (int)GetPlayer()->GetProximityToPlayer(eTarget));

				if(iPlayerLoop < MAX_MAJOR_CIVS)
				{
					if(GetMajorCivOpinion(eTarget) == MAJOR_CIV_OPINION_UNFORGIVABLE)
					{
						iWeight *= 2;
					}

					iWeight *= 10;
				}

				playerList.push_back(iPlayerLoop, iWeight);
			}
		}

		playerList.SortItems();

		for(int iI = 0; iI < playerList.size(); iI++)
		{
			DoMakeWarOnPlayer((PlayerTypes)playerList.GetElement(iI));
		}
	}


	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			if(!GET_PLAYER(eLoopPlayer).isMinorCiv())
			{
				for(int iThirdPlayerLoop = 0; iThirdPlayerLoop < MAX_MAJOR_CIVS; iThirdPlayerLoop++)
				{
					PlayerTypes eThirdPlayer = (PlayerTypes) iThirdPlayerLoop;
					if (IsPlayerValid(eThirdPlayer))
					{
						if(GetCoopWarCounter(eLoopPlayer, eThirdPlayer) >= 0)
						{
							ChangeCoopWarCounter(eLoopPlayer, eThirdPlayer, 1);


							if(!GET_PLAYER(eLoopPlayer).isHuman())
							{
								if(GetCoopWarAcceptedState(eLoopPlayer, eThirdPlayer) == COOP_WAR_STATE_SOON &&
									GetCoopWarCounter(eLoopPlayer, eThirdPlayer) ==        GC.getCOOP_WAR_SOON_COUNTER())
								{

									SetCoopWarAcceptedState(eLoopPlayer, eThirdPlayer, COOP_WAR_STATE_ACCEPTED);
									SetCoopWarCounter(eLoopPlayer, eThirdPlayer, 0);
									DeclareWar(eThirdPlayer);
									GetPlayer()->GetMilitaryAI()->RequestShowOfForce(eThirdPlayer);

									int iLockedTurns =        GC.getCOOP_WAR_LOCKED_LENGTH();
									GET_TEAM(GetPlayer()->getTeam()).ChangeNumTurnsLockedIntoWar(GET_PLAYER(eThirdPlayer).getTeam(), iLockedTurns);
								}
							}
						}
					}
				}
			}
		}
	}
}


void CvDiplomacyAI::DeclareWar(PlayerTypes ePlayer)
{
	CvAssertMsg(!GET_PLAYER(ePlayer).isMinorCiv() || GET_PLAYER(ePlayer).GetMinorCivAI()->GetAlly() != GetPlayer()->GetID(), "Major is declaring war on a city-state it is allied with! Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	CvTeam& kMyTeam = GET_TEAM(GetTeam());
	TeamTypes eTheirTeam = GET_PLAYER(ePlayer).getTeam();


	if(!kMyTeam.isAtWar(eTheirTeam))
	{
		kMyTeam.declareWar(eTheirTeam);

		m_pPlayer->GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_NOW_AT_WAR);


		if(!CvPreGame::isNetworkMultiplayerGame() && GC.getGame().getActivePlayer() == ePlayer)
		{
			const char* strText = GetDiploStringForMessage(DIPLO_MESSAGE_DOW_ROOT, ePlayer);
			gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_AI_DECLARED_WAR, strText, LEADERHEAD_ANIM_DECLARE_WAR);
		}

		LogWarDeclaration(ePlayer);
	}
}



WarFaceTypes CvDiplomacyAI::GetWarFaceWithPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarFaceTypes) m_paeWarFace[ePlayer];
}

void CvDiplomacyAI::SetWarFaceWithPlayer(PlayerTypes ePlayer, WarFaceTypes eWarFace)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarFace >= 0, "DIPLOMACY_AI: Invalid WarFaceType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarFace < NUM_WAR_FACES, "DIPLOMACY_AI: Invalid WarFaceType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeWarFace[ePlayer] = eWarFace;
}


bool CvDiplomacyAI::IsMusteringForAttack(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabMusteringForAttack[ePlayer];
}


void CvDiplomacyAI::SetMusteringForAttack(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabMusteringForAttack[ePlayer] = bValue;
}


void CvDiplomacyAI::DoSomeoneDeclaredWarOnMe(TeamTypes eTeam)
{
	PlayerTypes eLoopPlayer;


	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			if(GET_PLAYER(eLoopPlayer).getTeam() == eTeam)
			{

				if(!GET_PLAYER(eLoopPlayer).isMinorCiv())
				{

					SetWarGoal(eLoopPlayer, WAR_GOAL_PEACE);

				}

				else
				{

				}

				m_pPlayer->GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_NOW_AT_WAR);
			}
		}
	}
}



WarStateTypes CvDiplomacyAI::GetWarState(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarStateTypes) m_paeWarState[ePlayer];
}

void CvDiplomacyAI::SetWarState(PlayerTypes ePlayer, WarStateTypes eWarState)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarState >= NO_WAR_STATE_TYPE, "DIPLOMACY_AI: Invalid WarStateType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarState < NUM_WAR_STATES, "DIPLOMACY_AI: Invalid WarStateType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeWarState[ePlayer] = eWarState;
}


void CvDiplomacyAI::DoUpdateWarStates()
{
	WarStateTypes eWarState;

	int iMyLocalMilitaryStrength;
	int iEnemyInHisLandsMilitaryStrength;

	int iMyForeignMilitaryStrength;
	int iEnemyInMyLandsMilitaryStrength;

	int iMyPercentForeign;
	int iEnemyPercentInHisLands;

	int iMyPercentLocal;
	int iEnemyPercentInMyLands;

	int iLocalRatio;
	int iForeignRatio;

	int iWarStateValue;

	CvUnit* pLoopUnit;
	CvCity* pLoopCity;
	int iUnitOrCityLoop;
	int iPercentHealthLeft;


	int iStateAllWars = 0;
	SetStateAllWars(STATE_ALL_WARS_NEUTRAL);


	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{

			if(IsAtWar(eLoopPlayer))
			{
				iMyLocalMilitaryStrength = 0;
				iEnemyInHisLandsMilitaryStrength = 0;

				iMyForeignMilitaryStrength = 0;
				iEnemyInMyLandsMilitaryStrength = 0;


				for(pLoopUnit = GetPlayer()->firstUnit(&iUnitOrCityLoop); pLoopUnit != NULL; pLoopUnit = GetPlayer()->nextUnit(&iUnitOrCityLoop))
				{

					if(pLoopUnit->plot()->IsHomeFrontForPlayer(GetPlayer()->GetID()))
						iMyLocalMilitaryStrength += pLoopUnit->GetPower();


					if(pLoopUnit->plot()->IsHomeFrontForPlayer(eLoopPlayer))
						iMyForeignMilitaryStrength += pLoopUnit->GetPower();
				}


				for(pLoopUnit = GET_PLAYER(eLoopPlayer).firstUnit(&iUnitOrCityLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(eLoopPlayer).nextUnit(&iUnitOrCityLoop))
				{

					if(pLoopUnit->plot()->IsHomeFrontForPlayer(GetPlayer()->GetID()))
					{
						iEnemyInMyLandsMilitaryStrength += pLoopUnit->GetPower();
					}

					if(pLoopUnit->plot()->IsHomeFrontForPlayer(eLoopPlayer))
					{
						iEnemyInHisLandsMilitaryStrength += pLoopUnit->GetPower();
					}
				}


				for(pLoopCity = GetPlayer()->firstCity(&iUnitOrCityLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iUnitOrCityLoop))
				{
					iPercentHealthLeft = (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage()) * 100 / pLoopCity->GetMaxHitPoints();
					iMyLocalMilitaryStrength += (pLoopCity->GetPower() * iPercentHealthLeft / 100 / 100);
				}


				for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iUnitOrCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iUnitOrCityLoop))
				{
					iPercentHealthLeft = (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage()) * 100 / pLoopCity->GetMaxHitPoints();
					iEnemyInHisLandsMilitaryStrength += (pLoopCity->GetPower() * iPercentHealthLeft / 100 / 100);
				}




				if(iMyLocalMilitaryStrength + iMyForeignMilitaryStrength == 0)
				{
					iMyPercentLocal = 100;
					iMyPercentForeign = 0;
				}
				else
				{
					iMyPercentLocal = iMyLocalMilitaryStrength * 100 / (iMyLocalMilitaryStrength + iMyForeignMilitaryStrength);
					iMyPercentForeign = 100 - iMyPercentLocal;
				}


				if(iEnemyInHisLandsMilitaryStrength + iEnemyInMyLandsMilitaryStrength == 0)
				{
					iEnemyPercentInHisLands = 100;
					iEnemyPercentInMyLands = 0;
				}
				else
				{
					iEnemyPercentInHisLands = iEnemyInHisLandsMilitaryStrength * 100 / (iEnemyInHisLandsMilitaryStrength + iEnemyInMyLandsMilitaryStrength);
					iEnemyPercentInMyLands = 100 - iEnemyPercentInHisLands;
				}


				if(iMyLocalMilitaryStrength == 0)
				{
					iMyLocalMilitaryStrength = 1;
				}
				iLocalRatio = iMyLocalMilitaryStrength * 100 / (iMyLocalMilitaryStrength + iEnemyInMyLandsMilitaryStrength);

				if(iEnemyInHisLandsMilitaryStrength == 0)
				{
					iEnemyInHisLandsMilitaryStrength = 1;
				}
				iForeignRatio = iMyForeignMilitaryStrength * 100 / (iMyForeignMilitaryStrength + iEnemyInHisLandsMilitaryStrength);


				if(iForeignRatio <        GC.getWAR_STATE_CALM_THRESHOLD_FOREIGN_FORCES() && iLocalRatio > 100 -        GC.getWAR_STATE_CALM_THRESHOLD_FOREIGN_FORCES())
				{
					eWarState = WAR_STATE_CALM;
				}

				else
				{
					iWarStateValue = 0;

					iWarStateValue += iLocalRatio;
					iWarStateValue += iForeignRatio;

					iWarStateValue /= 2;











					if(iWarStateValue >=        GC.getWAR_STATE_THRESHOLD_NEARLY_WON())
					{
						eWarState = WAR_STATE_NEARLY_WON;
					}
					else if(iWarStateValue >=        GC.getWAR_STATE_THRESHOLD_OFFENSIVE())
					{
						eWarState = WAR_STATE_OFFENSIVE;
					}
					else if(iWarStateValue >=        GC.getWAR_STATE_THRESHOLD_STALEMATE())
					{
						eWarState = WAR_STATE_STALEMATE;
					}
					else if(iWarStateValue >=        GC.getWAR_STATE_THRESHOLD_DEFENSIVE())
					{
						eWarState = WAR_STATE_DEFENSIVE;
					}
					else
					{
						eWarState = WAR_STATE_NEARLY_DEFEATED;
					}
				}






				if(eWarState == WAR_STATE_CALM)
				{
					if(GetPlayerTargetValue(eLoopPlayer) >= TARGET_VALUE_FAVORABLE)
					{
						eWarState = WAR_STATE_OFFENSIVE;
					}
				}


				if(eWarState <= WAR_STATE_DEFENSIVE)
				{
					if(iMyLocalMilitaryStrength >= iEnemyInMyLandsMilitaryStrength + iEnemyInMyLandsMilitaryStrength)
					{
						eWarState = WAR_STATE_STALEMATE;
					}
				}


				if (!GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					if(eWarState == WAR_STATE_NEARLY_WON)
					{
						iStateAllWars += 2;
					}
					else if(eWarState == WAR_STATE_OFFENSIVE)
					{
						iStateAllWars += 1;
					}
					else if(eWarState == WAR_STATE_DEFENSIVE)
					{
						iStateAllWars -= 1;


						CvCity *pCapital = m_pPlayer->getCapitalCity();
						if (pCapital)
						{
							if (pCapital->getDamage() > 0)
							{
								SetStateAllWars(STATE_ALL_WARS_LOSING);
							}
						}
					}
					else if(eWarState == WAR_STATE_NEARLY_DEFEATED)
					{

						SetStateAllWars(STATE_ALL_WARS_LOSING);
					}
				}
			}

			else
				eWarState = NO_WAR_STATE_TYPE;

			SetWarState(eLoopPlayer, eWarState);
		}
	}


	if(iStateAllWars < 0 || GetStateAllWars() == STATE_ALL_WARS_LOSING)
		SetStateAllWars(STATE_ALL_WARS_LOSING);
	else if(iStateAllWars > 0)
		SetStateAllWars(STATE_ALL_WARS_WINNING);
}


StateAllWars CvDiplomacyAI::GetStateAllWars() const
{
	return m_eStateAllWars;
}

void CvDiplomacyAI::SetStateAllWars(StateAllWars eState)
{
	CvAssertMsg(eState >= 0, "DIPLOMACY_AI: Invalid StateAllWar.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eState < NUM_STATES_ALL_WARS, "DIPLOMACY_AI: Invalid StateAllWar.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_eStateAllWars = eState;
}



WarProjectionTypes CvDiplomacyAI::GetWarProjection(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarProjectionTypes) m_paeWarProjection[ePlayer];
}


void CvDiplomacyAI::SetWarProjection(PlayerTypes ePlayer, WarProjectionTypes eWarProjection)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarProjection >= 0, "DIPLOMACY_AI: Invalid WarProjectionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarProjection < NUM_WAR_PROJECTION_TYPES, "DIPLOMACY_AI: Invalid WarProjectionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeWarProjection[ePlayer] = eWarProjection;
}


WarProjectionTypes CvDiplomacyAI::GetLastWarProjection(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	return (WarProjectionTypes) m_paeLastWarProjection[ePlayer];

}

void CvDiplomacyAI::SetLastWarProjection(PlayerTypes ePlayer, WarProjectionTypes eWarProjection)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarProjection >= 0, "DIPLOMACY_AI: Invalid WarProjectionType.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarProjection < NUM_WAR_PROJECTION_TYPES, "DIPLOMACY_AI: Invalid WarProjectionType.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	m_paeLastWarProjection[ePlayer] = eWarProjection;

}



void CvDiplomacyAI::DoUpdateWarProjections()
{
	WarProjectionTypes eWarProjection;

	int iWarScore;


	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			eWarProjection = NO_WAR_PROJECTION_TYPE;



			{
				iWarScore = GetWarScore(eLoopPlayer);


				if(iWarScore >=         GC.getWAR_PROJECTION_THRESHOLD_VERY_GOOD())
					eWarProjection = WAR_PROJECTION_VERY_GOOD;
				else if(iWarScore >=        GC.getWAR_PROJECTION_THRESHOLD_GOOD())
					eWarProjection = WAR_PROJECTION_GOOD;
				else if(iWarScore <=          GC.getWAR_PROJECTION_THRESHOLD_DESTRUCTION())
					eWarProjection = WAR_PROJECTION_DESTRUCTION;
				else if(iWarScore <=         GC.getWAR_PROJECTION_THRESHOLD_DEFEAT())
					eWarProjection = WAR_PROJECTION_DEFEAT;
				else if(iWarScore <=       GC.getWAR_PROJECTION_THRESHOLD_STALEMATE())
					eWarProjection = WAR_PROJECTION_STALEMATE;
				else
					eWarProjection = WAR_PROJECTION_UNKNOWN;


				if(GetPlayerTargetValue(eLoopPlayer) <= TARGET_VALUE_BAD)
				{
					if(eWarProjection >= WAR_PROJECTION_GOOD)
						eWarProjection = WAR_PROJECTION_STALEMATE;
				}
			}

			if (GetWarProjection(eLoopPlayer) != NO_WAR_PROJECTION_TYPE)
			{
				SetLastWarProjection(eLoopPlayer, GetWarProjection(eLoopPlayer));
			}
			else
			{

				SetLastWarProjection(eLoopPlayer, WAR_PROJECTION_UNKNOWN);
			}

			SetWarProjection(eLoopPlayer, eWarProjection);
		}
	}
}


int CvDiplomacyAI::GetWarScore(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iWarScore = 0;


	switch(GetPlayerMilitaryStrengthComparedToUs(ePlayer))
	{
	case STRENGTH_PATHETIC:
		iWarScore +=         GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_PATHETIC();
		break;
	case STRENGTH_WEAK:
		iWarScore +=        GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_WEAK();
		break;
	case STRENGTH_POOR:
		iWarScore +=        GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_POOR();
		break;
	case STRENGTH_AVERAGE:
		iWarScore +=       GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_AVERAGE();
		break;
	case STRENGTH_STRONG:
		iWarScore +=         GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_STRONG();
		break;
	case STRENGTH_POWERFUL:
		iWarScore +=         GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_POWERFUL();
		break;
	case STRENGTH_IMMENSE:
		iWarScore +=          GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_IMMENSE();
		break;
	}


	switch(GetPlayerEconomicStrengthComparedToUs(ePlayer))
	{
	case STRENGTH_PATHETIC:
		iWarScore +=        GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_PATHETIC();
		break;
	case STRENGTH_WEAK:
		iWarScore +=        GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_WEAK();
		break;
	case STRENGTH_POOR:
		iWarScore +=        GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_POOR();
		break;
	case STRENGTH_AVERAGE:
		iWarScore +=       GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_AVERAGE();
		break;
	case STRENGTH_STRONG:
		iWarScore +=         GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_STRONG();
		break;
	case STRENGTH_POWERFUL:
		iWarScore +=         GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_POWERFUL();
		break;
	case STRENGTH_IMMENSE:
		iWarScore +=         GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_IMMENSE();
		break;
	}


	switch(GetWarDamageLevel(ePlayer))
	{
	case WAR_DAMAGE_LEVEL_NONE:
		iWarScore +=       GC.getWAR_PROJECTION_WAR_DAMAGE_US_NONE();

		if(IsPlayerRecklessExpander(ePlayer))
		{
			iWarScore +=        GC.getWAR_PROJECTION_RECKLESS_EXPANDER();
		}
		break;
	case WAR_DAMAGE_LEVEL_MINOR:
		iWarScore +=         GC.getWAR_PROJECTION_WAR_DAMAGE_US_MINOR();
		break;
	case WAR_DAMAGE_LEVEL_MAJOR:
		iWarScore +=         GC.getWAR_PROJECTION_WAR_DAMAGE_US_MAJOR();
		break;
	case WAR_DAMAGE_LEVEL_SERIOUS:
		iWarScore +=         GC.getWAR_PROJECTION_WAR_DAMAGE_US_SERIOUS();
		break;
	case WAR_DAMAGE_LEVEL_CRIPPLED:
		iWarScore +=         GC.getWAR_PROJECTION_WAR_DAMAGE_US_CRIPPLED();
		break;
	}


	switch(GetOtherPlayerWarDamageLevel(ePlayer, GetPlayer()->GetID()))
	{
	case WAR_DAMAGE_LEVEL_NONE:
		iWarScore +=       GC.getWAR_PROJECTION_WAR_DAMAGE_THEM_NONE();
		break;
	case WAR_DAMAGE_LEVEL_MINOR:
		iWarScore +=       GC.getWAR_PROJECTION_WAR_DAMAGE_THEM_MINOR();
		break;
	case WAR_DAMAGE_LEVEL_MAJOR:
		iWarScore +=        GC.getWAR_PROJECTION_WAR_DAMAGE_THEM_MAJOR();
		break;
	case WAR_DAMAGE_LEVEL_SERIOUS:
		iWarScore +=        GC.getWAR_PROJECTION_WAR_DAMAGE_THEM_SERIOUS();
		break;
	case WAR_DAMAGE_LEVEL_CRIPPLED:
		iWarScore +=        GC.getWAR_PROJECTION_WAR_DAMAGE_THEM_CRIPPLED();
		break;
	}


	int iOurScore = GetPlayer()->GetScore();
	iOurScore = iOurScore > 100 ? iOurScore : 100;
	int iTheirScore = GET_PLAYER(ePlayer).GetScore();
	iTheirScore = iTheirScore > 100 ? iTheirScore : 100;
	int iRatio = ((iOurScore-iTheirScore) * 100) / (iOurScore>iTheirScore?iTheirScore:iOurScore);
	iRatio = iRatio >= -50 ? (iRatio <= 50 ? iRatio : 50) : -50;
	iWarScore += iRatio;


	int iTurnsAtWar = GetPlayerNumTurnsAtWar(ePlayer);
	iTurnsAtWar /= 3;
	iWarScore -= min(iTurnsAtWar,        GC.getWAR_PROJECTION_WAR_DURATION_SCORE_CAP());

	return iWarScore;
}



WarGoalTypes CvDiplomacyAI::GetWarGoal(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarGoalTypes) m_paeWarGoal[ePlayer];
}


void CvDiplomacyAI::SetWarGoal(PlayerTypes ePlayer, WarGoalTypes eWarGoal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarGoal >= NO_WAR_GOAL_TYPE, "DIPLOMACY_AI: Invalid WarGoalType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarGoal < NUM_WAR_GOALS, "DIPLOMACY_AI: Invalid WarGoalType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");






	m_paeWarGoal[ePlayer] = eWarGoal;
}


void CvDiplomacyAI::DoUpdateWarGoals()
{

	bool bWorldConquest = false;

	if(IsGoingForWorldConquest())
	{
		bWorldConquest = true;
	}

	int iWarGoalValue;
	WarGoalTypes eWarGoal;



	bool bHigherUpsWantWar;

	bool bIsMinor;

	WarProjectionTypes eProjection;


	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			eWarGoal = NO_WAR_GOAL_TYPE;

			if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
			{
				bIsMinor = (GET_PLAYER(eLoopPlayer).isMinorCiv());

				iWarGoalValue = 0;
				bHigherUpsWantWar = false;


				if(bIsMinor)
					bHigherUpsWantWar = GetMinorCivApproach(eLoopPlayer) == MINOR_CIV_APPROACH_CONQUEST;

				else
					bHigherUpsWantWar = GetMajorCivApproach(eLoopPlayer,                       false) == MAJOR_CIV_APPROACH_WAR;

				eProjection = GetWarProjection(eLoopPlayer);



				if(bHigherUpsWantWar)
				{

					if(bIsMinor)


						if(bWorldConquest)
							eWarGoal = WAR_GOAL_CONQUEST;
						else
							eWarGoal = WAR_GOAL_DAMAGE;


					else
					{

						eWarGoal = WAR_GOAL_DAMAGE;


						if(IsLockedIntoCoopWar(eLoopPlayer))
							eWarGoal = WAR_GOAL_CONQUEST;


						if(eProjection >= WAR_PROJECTION_UNKNOWN)
						{

							if(GetMajorCivOpinion(eLoopPlayer) == MAJOR_CIV_OPINION_UNFORGIVABLE)
								eWarGoal = WAR_GOAL_CONQUEST;


							else if(bWorldConquest)
								eWarGoal = WAR_GOAL_CONQUEST;
						}
					}
				}



				else
				{

					if(!bIsMinor && GetWarState(eLoopPlayer) == WAR_STATE_NEARLY_WON && GetStateAllWars() != STATE_ALL_WARS_LOSING)\
					{
						eWarGoal = WAR_GOAL_DAMAGE;
					}

					else
					{
						eWarGoal = WAR_GOAL_PEACE;
					}
				}
			}

			else if(GetWarGoal(eLoopPlayer) == WAR_GOAL_PREPARE)
				eWarGoal = WAR_GOAL_PREPARE;

			else if(GetWarGoal(eLoopPlayer) == WAR_GOAL_DEMAND)
				eWarGoal = WAR_GOAL_DEMAND;



			if(eWarGoal == WAR_GOAL_PEACE)
				ChangeWantPeaceCounter(eLoopPlayer, 1);
			else
				SetWantPeaceCounter(eLoopPlayer, 0);


			SetWarGoal(eLoopPlayer, eWarGoal);
		}
	}
}


int CvDiplomacyAI::GetPlayerNumTurnsAtWar(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiPlayerNumTurnsAtWar[ePlayer];
}


void CvDiplomacyAI::SetPlayerNumTurnsAtWar(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting PlayerNumTurnsAtWar to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paiPlayerNumTurnsAtWar[ePlayer] = iValue;
}


void CvDiplomacyAI::ChangePlayerNumTurnsAtWar(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		SetPlayerNumTurnsAtWar(ePlayer, GetPlayerNumTurnsAtWar(ePlayer) + iChange);
	}
}


int CvDiplomacyAI::GetNumWarsFought(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiNumWarsFought[ePlayer];
}


void CvDiplomacyAI::SetNumWarsFought(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting NumWarsFought to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paiNumWarsFought[ePlayer] = iValue;
}


void CvDiplomacyAI::ChangeNumWarsFought(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		SetNumWarsFought(ePlayer, GetNumWarsFought(ePlayer) + iChange);
	}
}


StrengthTypes CvDiplomacyAI::GetPlayerMilitaryStrengthComparedToUs(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (StrengthTypes) m_paePlayerMilitaryStrengthComparedToUs[ePlayer];
}


void CvDiplomacyAI::SetPlayerMilitaryStrengthComparedToUs(PlayerTypes ePlayer, StrengthTypes eMilitaryStrength)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eMilitaryStrength >= 0, "DIPLOMACY_AI: Invalid StrengthType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eMilitaryStrength < NUM_STRENGTH_VALUES, "DIPLOMACY_AI: Invalid StrengthType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerMilitaryStrengthComparedToUs[ePlayer] = eMilitaryStrength;
}


void CvDiplomacyAI::DoUpdatePlayerMilitaryStrengths()
{

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		DoUpdateOnePlayerMilitaryStrength(eLoopPlayer);
	}
}


void CvDiplomacyAI::DoUpdateOnePlayerMilitaryStrength(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	StrengthTypes eMilitaryStrength;

	int iBase =        GC.getMILITARY_STRENGTH_BASE();
	int iMilitaryStrength = iBase + GetPlayer()->GetMilitaryMight();

	int iOtherPlayerMilitary;
	int iMilitaryRatio;

	if(IsPlayerValid(ePlayer,                    true))
	{


		{
			iOtherPlayerMilitary = GET_PLAYER(ePlayer).GetMilitaryMight() + iBase;

			iMilitaryRatio = iOtherPlayerMilitary*         GC.getMILITARY_STRENGTH_RATIO_MULTIPLIER() / iMilitaryStrength;
		}








		if(iMilitaryRatio >=         GC.getMILITARY_STRENGTH_IMMENSE_THRESHOLD())
			eMilitaryStrength = STRENGTH_IMMENSE;
		else if(iMilitaryRatio >=         GC.getMILITARY_STRENGTH_POWERFUL_THRESHOLD())
			eMilitaryStrength = STRENGTH_POWERFUL;
		else if(iMilitaryRatio >=         GC.getMILITARY_STRENGTH_STRONG_THRESHOLD())
			eMilitaryStrength = STRENGTH_STRONG;
		else if(iMilitaryRatio >=        GC.getMILITARY_STRENGTH_AVERAGE_THRESHOLD())
			eMilitaryStrength = STRENGTH_AVERAGE;
		else if(iMilitaryRatio >=        GC.getMILITARY_STRENGTH_POOR_THRESHOLD())
			eMilitaryStrength = STRENGTH_POOR;
		else if(iMilitaryRatio >=        GC.getMILITARY_STRENGTH_WEAK_THRESHOLD())
			eMilitaryStrength = STRENGTH_WEAK;
		else
			eMilitaryStrength = STRENGTH_PATHETIC;


		SetPlayerMilitaryStrengthComparedToUs(ePlayer, eMilitaryStrength);
	}
}



StrengthTypes CvDiplomacyAI::GetPlayerEconomicStrengthComparedToUs(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (StrengthTypes) m_paePlayerEconomicStrengthComparedToUs[ePlayer];
}


void CvDiplomacyAI::SetPlayerEconomicStrengthComparedToUs(PlayerTypes ePlayer, StrengthTypes eEconomicStrength)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eEconomicStrength >= 0, "DIPLOMACY_AI: Invalid StrengthType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eEconomicStrength < NUM_STRENGTH_VALUES, "DIPLOMACY_AI: Invalid StrengthType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerEconomicStrengthComparedToUs[ePlayer] = eEconomicStrength;
}


void CvDiplomacyAI::DoUpdatePlayerEconomicStrengths()
{

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		DoUpdateOnePlayerEconomicStrength(eLoopPlayer);
	}
}

void CvDiplomacyAI::DoUpdateOnePlayerEconomicStrength(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iEconomicStrength;
	StrengthTypes eEconomicStrength;

	int iOtherPlayerEconomy;
	int iEconomicRatio;

	if(IsPlayerValid(ePlayer,                    true))
	{
		iEconomicStrength = 0;


		if(GetPlayer()->GetEconomicMight() > 0)
		{
			iOtherPlayerEconomy = GET_PLAYER(ePlayer).GetEconomicMight();

			iEconomicRatio = iOtherPlayerEconomy*         GC.getECONOMIC_STRENGTH_RATIO_MULTIPLIER() / GetPlayer()->GetEconomicMight();
		}
		else
		{
			iEconomicRatio =         GC.getECONOMIC_STRENGTH_RATIO_MULTIPLIER() * 5;
		}

		iEconomicStrength += iEconomicRatio;


		if(iEconomicStrength >=         GC.getECONOMIC_STRENGTH_IMMENSE_THRESHOLD())
			eEconomicStrength = STRENGTH_IMMENSE;
		else if(iEconomicStrength >=         GC.getECONOMIC_STRENGTH_POWERFUL_THRESHOLD())
			eEconomicStrength = STRENGTH_POWERFUL;
		else if(iEconomicStrength >=         GC.getECONOMIC_STRENGTH_STRONG_THRESHOLD())
			eEconomicStrength = STRENGTH_STRONG;
		else if(iEconomicStrength >=        GC.getECONOMIC_STRENGTH_AVERAGE_THRESHOLD())
			eEconomicStrength = STRENGTH_AVERAGE;
		else if(iEconomicStrength >=        GC.getECONOMIC_STRENGTH_POOR_THRESHOLD())
			eEconomicStrength = STRENGTH_POOR;
		else if(iEconomicStrength >=        GC.getECONOMIC_STRENGTH_WEAK_THRESHOLD())
			eEconomicStrength = STRENGTH_WEAK;
		else
			eEconomicStrength = STRENGTH_PATHETIC;


		SetPlayerEconomicStrengthComparedToUs(ePlayer, eEconomicStrength);
	}
}



TargetValueTypes CvDiplomacyAI::GetPlayerTargetValue(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (TargetValueTypes) m_paePlayerTargetValue[ePlayer];
}


void CvDiplomacyAI::SetPlayerTargetValue(PlayerTypes ePlayer, TargetValueTypes eTargetValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetValue >= 0, "DIPLOMACY_AI: Invalid TargetValueType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetValue < NUM_TARGET_VALUES, "DIPLOMACY_AI: Invalid TargetValueType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerTargetValue[ePlayer] = eTargetValue;
}


void CvDiplomacyAI::DoUpdatePlayerTargetValues()
{
	PlayerTypes eLoopPlayer;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		DoUpdateOnePlayerTargetValue(eLoopPlayer);
	}
}


void CvDiplomacyAI::DoUpdateOnePlayerTargetValue(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(!IsPlayerValid(ePlayer))
		return;

	int iTargetValue;
	TargetValueTypes eTargetValue;

	int iWarCount;

	CvCity* pLoopCity;
	int iCityLoop;

	int iOtherPlayerMilitaryStrength;
	int iCityStrengthMod;
	int iMilitaryRatio;

	int iMyMilitaryStrength = GetPlayer()->getPower();

	if(iMyMilitaryStrength == 0)
	{
		iMyMilitaryStrength = 1;
	}

	int iCityDamage;
	int iNumCities;

	int iThirdPartyLoop;
	PlayerTypes eThirdPartyPlayer;
	int iThirdPartyValue;
	StrengthTypes eThirdPartyStrength;
	PlayerProximityTypes eThirdPartyProximity;
	iTargetValue = 0;

	iCityDamage = 0;
	iNumCities = 0;

	iOtherPlayerMilitaryStrength = GET_PLAYER(ePlayer).getPower();


	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
	{
		iCityStrengthMod = pLoopCity->GetPower();

		iCityStrengthMod *=        GC.getMILITARY_STRENGTH_CITY_MOD();
		iCityStrengthMod /= 100;

		iOtherPlayerMilitaryStrength += iCityStrengthMod;


		iCityDamage += pLoopCity->getDamage();
		iNumCities++;
	}


	if(iNumCities > 0)
	{
		iCityDamage /= iNumCities;
		iCityDamage *= 100;
		iCityDamage /= GC.getMAX_CITY_HIT_POINTS();

		iCityDamage *= iOtherPlayerMilitaryStrength;
		iCityDamage /= 200;

		iOtherPlayerMilitaryStrength -= iCityDamage;
	}

	iMilitaryRatio = iOtherPlayerMilitaryStrength*         GC.getMILITARY_STRENGTH_RATIO_MULTIPLIER() / iMyMilitaryStrength;


	iTargetValue += iMilitaryRatio;


	iWarCount = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getAtWarCount(true);

	if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
	{
		iWarCount--;
	}

	iTargetValue += (iWarCount*        GC.getTARGET_ALREADY_WAR_EACH_PLAYER());


	switch(GetPlayer()->GetProximityToPlayer(ePlayer))
	{
	case PLAYER_PROXIMITY_NEIGHBORS:
		iTargetValue +=         GC.getTARGET_NEIGHBORS();
		break;
	case PLAYER_PROXIMITY_CLOSE:
		iTargetValue +=       GC.getTARGET_CLOSE();
		break;
	case PLAYER_PROXIMITY_FAR:
		iTargetValue +=        GC.getTARGET_FAR();
		break;
	case PLAYER_PROXIMITY_DISTANT:
		iTargetValue +=        GC.getTARGET_DISTANT();
		break;
	}


	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		for(iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
		{
			eThirdPartyPlayer = (PlayerTypes) iThirdPartyLoop;

			if(IsPlayerValid(eThirdPartyPlayer))
			{

				if(GET_PLAYER(ePlayer).GetMinorCivAI()->GetAlly() == eThirdPartyPlayer ||
				        GET_PLAYER(ePlayer).GetMinorCivAI()->IsProtectedByMajor(eThirdPartyPlayer))
				{
					iThirdPartyValue = 0;
					eThirdPartyStrength = GetPlayerMilitaryStrengthComparedToUs(eThirdPartyPlayer);
					eThirdPartyProximity	= GetPlayer()->GetProximityToPlayer(eThirdPartyPlayer);


					if(eThirdPartyStrength == STRENGTH_PATHETIC)
						iThirdPartyValue +=        GC.getTARGET_MINOR_BACKUP_PATHETIC();
					else if(eThirdPartyStrength == STRENGTH_WEAK)
						iThirdPartyValue +=        GC.getTARGET_MINOR_BACKUP_WEAK();
					else if(eThirdPartyStrength == STRENGTH_POOR)
						iThirdPartyValue +=        GC.getTARGET_MINOR_BACKUP_POOR();
					else if(eThirdPartyStrength == STRENGTH_AVERAGE)
						iThirdPartyValue +=         GC.getTARGET_MINOR_BACKUP_AVERAGE();
					else if(eThirdPartyStrength == STRENGTH_STRONG)
						iThirdPartyValue +=         GC.getTARGET_MINOR_BACKUP_STRONG();
					else if(eThirdPartyStrength == STRENGTH_POWERFUL)
						iThirdPartyValue +=         GC.getTARGET_MINOR_BACKUP_POWERFUL();
					else if(eThirdPartyStrength == STRENGTH_IMMENSE)
						iThirdPartyValue +=         GC.getTARGET_MINOR_BACKUP_IMMENSE();


					if(eThirdPartyProximity == PLAYER_PROXIMITY_DISTANT)
						iThirdPartyValue *=          GC.getTARGET_MINOR_BACKUP_DISTANT();
					else if(eThirdPartyProximity == PLAYER_PROXIMITY_FAR)
						iThirdPartyValue *=          GC.getTARGET_MINOR_BACKUP_FAR();
					else if(eThirdPartyProximity == PLAYER_PROXIMITY_CLOSE)
						iThirdPartyValue *=          GC.getTARGET_MINOR_BACKUP_CLOSE();
					else if(eThirdPartyProximity == PLAYER_PROXIMITY_NEIGHBORS)
						iThirdPartyValue *=          GC.getTARGET_MINOR_BACKUP_NEIGHBORS();

					iThirdPartyValue /= 100;


					iTargetValue += iThirdPartyValue;
				}
			}
		}
	}


	if(iTargetValue >=         GC.getTARGET_IMPOSSIBLE_THRESHOLD())
		eTargetValue = TARGET_VALUE_IMPOSSIBLE;
	else if(iTargetValue >=         GC.getTARGET_BAD_THRESHOLD())
		eTargetValue = TARGET_VALUE_BAD;
	else if(iTargetValue >=        GC.getTARGET_AVERAGE_THRESHOLD())
		eTargetValue = TARGET_VALUE_AVERAGE;
	else if(iTargetValue >=        GC.getTARGET_FAVORABLE_THRESHOLD())
		eTargetValue = TARGET_VALUE_FAVORABLE;
	else
		eTargetValue = TARGET_VALUE_SOFT;


	if(eTargetValue < TARGET_VALUE_SOFT && IsPlayerRecklessExpander(ePlayer))
		eTargetValue = TargetValueTypes(eTargetValue + 1);


	if(eTargetValue > TARGET_VALUE_IMPOSSIBLE && GetPlayerNumTurnsAtWar(ePlayer) >        GC.getTARGET_INCREASE_WAR_TURNS())
		eTargetValue = TargetValueTypes(eTargetValue - 1);


	if(eTargetValue == TARGET_VALUE_SOFT)
	{
		if(GetPlayer()->GetProximityToPlayer(ePlayer) <= PLAYER_PROXIMITY_FAR)
		{
			eTargetValue = TARGET_VALUE_FAVORABLE;
		}
	}


	SetPlayerTargetValue(ePlayer, eTargetValue);
}








ThreatTypes CvDiplomacyAI::GetMilitaryThreat(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (ThreatTypes) m_paeMilitaryThreat[ePlayer];
}


void CvDiplomacyAI::SetMilitaryThreat(PlayerTypes ePlayer, ThreatTypes eMilitaryThreat)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eMilitaryThreat >= 0, "DIPLOMACY_AI: Invalid ThreatType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eMilitaryThreat < NUM_THREAT_VALUES, "DIPLOMACY_AI: Invalid v.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeMilitaryThreat[ePlayer] = eMilitaryThreat;
}


void CvDiplomacyAI::DoUpdateMilitaryThreats()
{
	int iMilitaryThreat;
	ThreatTypes eMilitaryThreatType;

	int iWarCount;

	int iMyMilitaryStrength = GetPlayer()->getPower();

	if(iMyMilitaryStrength == 0)
	{
		iMyMilitaryStrength = 1;
	}


	CvCity* pLoopCity;
	int iCityLoop;
	int iCityStrengthMod;
	for(pLoopCity = GetPlayer()->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iCityLoop))
	{
		iCityStrengthMod = pLoopCity->GetPower();
		iCityStrengthMod *= (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage());
		iCityStrengthMod /= pLoopCity->GetMaxHitPoints();
		iCityStrengthMod /= 100;
		iCityStrengthMod *=        GC.getMILITARY_STRENGTH_CITY_MOD();
		iCityStrengthMod /= 100;
		iMyMilitaryStrength += iCityStrengthMod;
	}

	int iOtherPlayerMilitaryStrength;
	int iMilitaryRatio;


	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			iMilitaryThreat = 0;

			iOtherPlayerMilitaryStrength = GET_PLAYER(eLoopPlayer).getPower();


			iMilitaryRatio = iOtherPlayerMilitaryStrength*         GC.getMILITARY_STRENGTH_RATIO_MULTIPLIER() / iMyMilitaryStrength;

			iMilitaryThreat += iMilitaryRatio;


			if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
			{

				switch(GetWarState(eLoopPlayer))
				{
				case WAR_STATE_CALM:
					iMilitaryThreat +=       GC.getMILITARY_THREAT_WAR_STATE_CALM();
					break;
				case WAR_STATE_NEARLY_WON:
					iMilitaryThreat +=          GC.getMILITARY_THREAT_WAR_STATE_NEARLY_WON();
					break;
				case WAR_STATE_OFFENSIVE:
					iMilitaryThreat +=         GC.getMILITARY_THREAT_WAR_STATE_OFFENSIVE();
					break;
				case WAR_STATE_STALEMATE:
					iMilitaryThreat +=        GC.getMILITARY_THREAT_WAR_STATE_STALEMATE();
					break;
				case WAR_STATE_DEFENSIVE:
					iMilitaryThreat +=        GC.getMILITARY_THREAT_WAR_STATE_DEFENSIVE();
					break;
				case WAR_STATE_NEARLY_DEFEATED:
					iMilitaryThreat +=         GC.getMILITARY_THREAT_WAR_STATE_NEARLY_DEFEATED();
					break;
				}
			}






			switch(GetPlayer()->GetProximityToPlayer(eLoopPlayer))
			{
			case PLAYER_PROXIMITY_NEIGHBORS:
				iMilitaryThreat +=         GC.getMILITARY_THREAT_NEIGHBORS();
				break;
			case PLAYER_PROXIMITY_CLOSE:
				iMilitaryThreat +=        GC.getMILITARY_THREAT_CLOSE();
				break;
			case PLAYER_PROXIMITY_FAR:
				iMilitaryThreat +=         GC.getMILITARY_THREAT_FAR();
				break;
			case PLAYER_PROXIMITY_DISTANT:
				iMilitaryThreat +=          GC.getMILITARY_THREAT_DISTANT();
				break;
			}


			if(!GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
			{

				iMilitaryThreat += (GetOtherPlayerNumMinorsAttacked(eLoopPlayer) *        GC.getMILITARY_THREAT_PER_MINOR_ATTACKED());
				iMilitaryThreat += (GetOtherPlayerNumMinorsConquered(eLoopPlayer) *        GC.getMILITARY_THREAT_PER_MINOR_CONQUERED());
				iMilitaryThreat += (GetOtherPlayerNumMajorsAttacked(eLoopPlayer) *        GC.getMILITARY_THREAT_PER_MAJOR_ATTACKED());
				iMilitaryThreat += (GetOtherPlayerNumMajorsConquered(eLoopPlayer) *        GC.getMILITARY_THREAT_PER_MAJOR_CONQUERED());


				iWarCount = GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).getAtWarCount(true);
				if(iWarCount > 0)
				{
					iMilitaryThreat += (        GC.getMILITARY_THREAT_ALREADY_WAR_EACH_PLAYER_MULTIPLIER() * iWarCount * iMilitaryThreat / 100);
				}
			}


			if(iMilitaryThreat >=         GC.getMILITARY_THREAT_CRITICAL_THRESHOLD())
				eMilitaryThreatType = THREAT_CRITICAL;
			else if(iMilitaryThreat >=         GC.getMILITARY_THREAT_SEVERE_THRESHOLD())
				eMilitaryThreatType = THREAT_SEVERE;
			else if(iMilitaryThreat >=         GC.getMILITARY_THREAT_MAJOR_THRESHOLD())
				eMilitaryThreatType = THREAT_MAJOR;
			else if(iMilitaryThreat >=         GC.getMILITARY_THREAT_MINOR_THRESHOLD())
				eMilitaryThreatType = THREAT_MINOR;
			else
				eMilitaryThreatType = THREAT_NONE;


			SetMilitaryThreat(eLoopPlayer, eMilitaryThreatType);
		}
	}
}



ThreatTypes CvDiplomacyAI::GetEstimateOtherPlayerMilitaryThreat(PlayerTypes ePlayer, PlayerTypes eWithPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (ThreatTypes) m_ppaaeOtherPlayerMilitaryThreat[ePlayer][eWithPlayer];
}

void CvDiplomacyAI::SetEstimateOtherPlayerMilitaryThreat(PlayerTypes ePlayer, PlayerTypes eWithPlayer, ThreatTypes eThreatType)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eThreatType >= 0, "DIPLOMACY_AI: Invalid ThreatType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eThreatType < NUM_THREAT_VALUES, "DIPLOMACY_AI: Invalid ThreatType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaeOtherPlayerMilitaryThreat[ePlayer][eWithPlayer] = eThreatType;
}


void CvDiplomacyAI::DoUpdateEstimateOtherPlayerMilitaryThreats()
{
	int iMilitaryThreat;
	ThreatTypes eMilitaryThreatType;

	int iWarCount;

	int iPlayerMilitaryStrength;

	CvCity* pLoopCity;
	int iCityLoop;
	int iCityStrengthMod;

	int iThirdPlayerMilitaryStrength;
	int iMilitaryRatio;

	PlayerTypes eLoopOtherPlayer;
	int iOtherPlayerLoop;


	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			iPlayerMilitaryStrength = GET_PLAYER(eLoopPlayer).GetMilitaryMight();


			for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iCityLoop))
			{
				iCityStrengthMod = pLoopCity->GetPower();
				iCityStrengthMod *= (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage());
				iCityStrengthMod /= pLoopCity->GetMaxHitPoints();
				iCityStrengthMod /= 100;
				iCityStrengthMod *=        GC.getMILITARY_STRENGTH_CITY_MOD();
				iCityStrengthMod /= 100;
				iPlayerMilitaryStrength += (MAX(iCityStrengthMod, 0));
			}


			if(iPlayerMilitaryStrength == 0)
				iPlayerMilitaryStrength = 1;


			for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_MAJOR_CIVS; iOtherPlayerLoop++)
			{
				eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;


				if(eLoopPlayer != eLoopOtherPlayer)
				{

					if(IsPlayerValid(eLoopOtherPlayer) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopOtherPlayer))
					{
						eMilitaryThreatType = THREAT_NONE;
						iMilitaryThreat = 0;

						iThirdPlayerMilitaryStrength = GET_PLAYER(eLoopOtherPlayer).GetMilitaryMight();


						iMilitaryRatio = iThirdPlayerMilitaryStrength*         GC.getMILITARY_STRENGTH_RATIO_MULTIPLIER() / iPlayerMilitaryStrength;

						iMilitaryThreat += iMilitaryRatio;

































						switch(GET_PLAYER(eLoopPlayer).GetProximityToPlayer(eLoopOtherPlayer))
						{
						case PLAYER_PROXIMITY_NEIGHBORS:
							iMilitaryThreat +=         GC.getMILITARY_THREAT_NEIGHBORS();
							break;
						case PLAYER_PROXIMITY_CLOSE:
							iMilitaryThreat +=        GC.getMILITARY_THREAT_CLOSE();
							break;
						case PLAYER_PROXIMITY_FAR:
							iMilitaryThreat +=         GC.getMILITARY_THREAT_FAR();
							break;
						case PLAYER_PROXIMITY_DISTANT:
							iMilitaryThreat +=          GC.getMILITARY_THREAT_DISTANT();
							break;
						}



						{

							iMilitaryThreat += (GetOtherPlayerNumMinorsAttacked(eLoopOtherPlayer)  *        GC.getMILITARY_THREAT_PER_MINOR_ATTACKED());
							iMilitaryThreat += (GetOtherPlayerNumMinorsConquered(eLoopOtherPlayer) *        GC.getMILITARY_THREAT_PER_MINOR_CONQUERED());
							iMilitaryThreat += (GetOtherPlayerNumMajorsAttacked(eLoopOtherPlayer)  *        GC.getMILITARY_THREAT_PER_MAJOR_ATTACKED());
							iMilitaryThreat += (GetOtherPlayerNumMajorsConquered(eLoopOtherPlayer) *        GC.getMILITARY_THREAT_PER_MAJOR_CONQUERED());


							iWarCount = GET_TEAM(GET_PLAYER(eLoopOtherPlayer).getTeam()).getAtWarCount(true);
							if(iWarCount > 0)
								iMilitaryThreat += (        GC.getMILITARY_THREAT_ALREADY_WAR_EACH_PLAYER_MULTIPLIER() * iWarCount * iMilitaryThreat / 100);
						}


						if(iMilitaryThreat >=         GC.getMILITARY_THREAT_CRITICAL_THRESHOLD())
							eMilitaryThreatType = THREAT_CRITICAL;
						else if(iMilitaryThreat >=         GC.getMILITARY_THREAT_SEVERE_THRESHOLD())
							eMilitaryThreatType = THREAT_SEVERE;
						else if(iMilitaryThreat >=         GC.getMILITARY_THREAT_MAJOR_THRESHOLD())
							eMilitaryThreatType = THREAT_MAJOR;
						else if(iMilitaryThreat >=         GC.getMILITARY_THREAT_MINOR_THRESHOLD())
							eMilitaryThreatType = THREAT_MINOR;


						SetEstimateOtherPlayerMilitaryThreat(eLoopPlayer, eLoopOtherPlayer, eMilitaryThreatType);
					}
				}
			}
		}
	}
}


ThreatTypes CvDiplomacyAI::GetWarmongerThreat(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (ThreatTypes) m_paeWarmongerThreat[ePlayer];
}


void CvDiplomacyAI::SetWarmongerThreat(PlayerTypes ePlayer, ThreatTypes eWarmongerThreat)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarmongerThreat >= 0, "DIPLOMACY_AI: Invalid ThreatType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarmongerThreat < NUM_THREAT_VALUES, "DIPLOMACY_AI: Invalid ThreatType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeWarmongerThreat[ePlayer] = eWarmongerThreat;
}


void CvDiplomacyAI::DoUpdateWarmongerThreats()
{
	ThreatTypes eThreatType;

	int iThreatValue;

	int iNumPlayersKilled;
	int iNumPlayersEver = GC.getGame().countCivPlayersEverAlive();
	int iPlayersKilledPercent;
	int iWarmongerMod;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			eThreatType = THREAT_NONE;
			iThreatValue = GetOtherPlayerWarmongerScore(eLoopPlayer);


			if(iThreatValue >=         GC.getWARMONGER_THREAT_CRITICAL_THRESHOLD())
				eThreatType = THREAT_CRITICAL;
			else if(iThreatValue >=         GC.getWARMONGER_THREAT_SEVERE_THRESHOLD())
				eThreatType = THREAT_SEVERE;
			else if(iThreatValue >=        GC.getWARMONGER_THREAT_MAJOR_THRESHOLD())
				eThreatType = THREAT_MAJOR;
			else if(iThreatValue >=        GC.getWARMONGER_THREAT_MINOR_THRESHOLD())
				eThreatType = THREAT_MINOR;


			iNumPlayersKilled = GetOtherPlayerNumMinorsConquered(eLoopPlayer) + GetOtherPlayerNumMajorsConquered(eLoopPlayer);
			if(iNumPlayersKilled > 0)
			{
				iPlayersKilledPercent = iNumPlayersKilled * 100 / iNumPlayersEver;

				iWarmongerMod = GetWarmongerHate() -       GC.getDEFAULT_FLAVOR_VALUE();
				iWarmongerMod *=        GC.getWARMONGER_THREAT_PERSONALITY_MOD();
				iPlayersKilledPercent += (iPlayersKilledPercent * iWarmongerMod / 100);

				if(iPlayersKilledPercent >=        GC.getWARMONGER_THREAT_CRITICAL_PERCENT_THRESHOLD())
					eThreatType = THREAT_CRITICAL;
				if(iPlayersKilledPercent >=        GC.getWARMONGER_THREAT_SEVERE_PERCENT_THRESHOLD())
					eThreatType = THREAT_SEVERE;
			}


			SetWarmongerThreat(eLoopPlayer, eThreatType);


			ChangeOtherPlayerWarmongerAmount(eLoopPlayer,       GC.getWARMONGER_THREAT_PER_TURN_DECAY());
		}
	}
}









void CvDiplomacyAI::DoUpdatePlanningExchanges()
{



	int iNumRAsWanted = GetNumResearchAgreementsWanted();


	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{


			if(!GET_TEAM(GetPlayer()->getTeam()).IsHasResearchAgreement(GET_PLAYER(eLoopPlayer).getTeam()))
			{

				if(!IsWantsResearchAgreementWithPlayer(eLoopPlayer))
				{


						if(GET_TEAM(GetPlayer()->getTeam()).IsResearchAgreementTradingAllowedWithTeam(GET_PLAYER(eLoopPlayer).getTeam()) ||
						        GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).IsResearchAgreementTradingAllowedWithTeam(GetPlayer()->getTeam()))
						{








								DoAddWantsResearchAgreementWithPlayer(eLoopPlayer);
								iNumRAsWanted++;

						}

				}
			}


			if(IsWantsResearchAgreementWithPlayer(eLoopPlayer))
			{
				bool bCancel = false;
				if(GetMajorCivApproach(eLoopPlayer,                       true) == MAJOR_CIV_APPROACH_HOSTILE)
				{
					bCancel = true;
				}
				else if(!(GET_TEAM(GetPlayer()->getTeam()).IsResearchAgreementTradingAllowedWithTeam(GET_PLAYER(eLoopPlayer).getTeam()) ||
					      GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).IsResearchAgreementTradingAllowedWithTeam(GetPlayer()->getTeam())))
				{
					bCancel = true;
				}
	
				if (bCancel)
				{
					DoCancelWantsResearchAgreementWithPlayer(eLoopPlayer);
				}
			}
		}
	}
}


bool CvDiplomacyAI::IsWantsResearchAgreementWithPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabWantsResearchAgreementWithPlayer[ePlayer];
}


void CvDiplomacyAI::SetWantsResearchAgreementWithPlayer(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabWantsResearchAgreementWithPlayer[ePlayer] = bValue;
}


int CvDiplomacyAI::GetNumResearchAgreementsWanted() const
{
	int iNum = 0;


	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsWantsResearchAgreementWithPlayer(eLoopPlayer))
			iNum++;
	}

	return iNum;
}


void CvDiplomacyAI::DoAddWantsResearchAgreementWithPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	CvAssertMsg(!IsWantsResearchAgreementWithPlayer(ePlayer), "DIPLOMACY_AI: AI trying to save for multiple Research Agreements with a player.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

	SetWantsResearchAgreementWithPlayer(ePlayer, true);


	int iGoldAmount = GC.getGame().GetGameDeals()->GetTradeItemGoldCost(TRADE_ITEM_RESEARCH_AGREEMENT, GetPlayer()->GetID(), ePlayer);
	GetPlayer()->GetEconomicAI()->StartSaveForPurchase(PURCHASE_TYPE_MAJOR_CIV_TRADE, iGoldAmount,              1);

	LogWantRA(ePlayer);
}


void CvDiplomacyAI::DoCancelWantsResearchAgreementWithPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	CvAssertMsg(IsWantsResearchAgreementWithPlayer(ePlayer), "DIPLOMACY_AI: AI trying to cancel saving for a Research Agreements he doesn't have.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

	SetWantsResearchAgreementWithPlayer(ePlayer, false);

	GetPlayer()->GetEconomicAI()->CancelSaveForPurchase(PURCHASE_TYPE_MAJOR_CIV_TRADE);
}


bool CvDiplomacyAI::IsCanMakeResearchAgreementRightNow(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasResearchedAllTechs() || GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetTeamTechs()->HasResearchedAllTechs())
	{
		return false;
	}


	if(!IsWantsResearchAgreementWithPlayer(ePlayer))
	{
		return false;
	}


	if(GET_TEAM(GetPlayer()->getTeam()).IsHasResearchAgreement(GET_PLAYER(ePlayer).getTeam()))
	{
		return false;
	}


	if(!(GET_TEAM(GetPlayer()->getTeam()).IsResearchAgreementTradingAllowedWithTeam(GET_PLAYER(ePlayer).getTeam())   ||
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsResearchAgreementTradingAllowedWithTeam(GetPlayer()->getTeam())))
	{
		return false;
	}

	int iGoldAmount = GC.getGame().GetGameDeals()->GetTradeItemGoldCost(TRADE_ITEM_RESEARCH_AGREEMENT, GetPlayer()->GetID(), ePlayer);


	if(GetPlayer()->GetTreasury()->GetGold() < iGoldAmount)
	{
		return false;
	}


	if(GET_PLAYER(ePlayer).GetTreasury()->GetGold() < iGoldAmount)
	{
		return false;
	}

	return true;
}









DisputeLevelTypes CvDiplomacyAI::GetLandDisputeLevel(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_paePlayerLandDisputeLevel[ePlayer];
}


void CvDiplomacyAI::SetLandDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel >= 0, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel < NUM_DISPUTE_LEVELS, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerLandDisputeLevel[ePlayer] = eDisputeLevel;
}


void CvDiplomacyAI::DoUpdateLandDisputeLevels()
{
	PlayerTypes ePlayer;

	DisputeLevelTypes eDisputeLevel;

	int iLandDisputeWeight;
	int iExpansionFlavor;

	AggressivePostureTypes eAggression;
	PlayerProximityTypes eProximity;


	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;


		m_paePlayerLastTurnLandDisputeLevel[ePlayer] = GetLandDisputeLevel(ePlayer);

		if(IsPlayerValid(ePlayer))
		{
			eDisputeLevel = DISPUTE_LEVEL_NONE;

			iLandDisputeWeight = 0;


			eAggression = GetExpansionAggressivePosture(ePlayer);

			if(eAggression == AGGRESSIVE_POSTURE_NONE)
				iLandDisputeWeight +=       GC.getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_NONE();
			else if(eAggression == AGGRESSIVE_POSTURE_LOW)
				iLandDisputeWeight +=        GC.getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_LOW();
			else if(eAggression == AGGRESSIVE_POSTURE_MEDIUM)
				iLandDisputeWeight +=        GC.getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_MEDIUM();
			else if(eAggression == AGGRESSIVE_POSTURE_HIGH)
				iLandDisputeWeight +=        GC.getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_HIGH();
			else if(eAggression == AGGRESSIVE_POSTURE_INCREDIBLE)
				iLandDisputeWeight +=        GC.getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_INCREDIBLE();


			eAggression = GetPlotBuyingAggressivePosture(ePlayer);

			if(eAggression == AGGRESSIVE_POSTURE_NONE)
				iLandDisputeWeight +=       GC.getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_NONE();
			else if(eAggression == AGGRESSIVE_POSTURE_LOW)
				iLandDisputeWeight +=       GC.getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_LOW();
			else if(eAggression == AGGRESSIVE_POSTURE_MEDIUM)
				iLandDisputeWeight +=        GC.getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_MEDIUM();
			else if(eAggression == AGGRESSIVE_POSTURE_HIGH)
				iLandDisputeWeight +=        GC.getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_HIGH();
			else if(eAggression == AGGRESSIVE_POSTURE_INCREDIBLE)
				iLandDisputeWeight +=        GC.getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_INCREDIBLE();


			eProximity = GetPlayer()->GetProximityToPlayer(ePlayer);

			if(eProximity == PLAYER_PROXIMITY_DISTANT)
				iLandDisputeWeight +=       GC.getLAND_DISPUTE_DISTANT();
			else if(eProximity == PLAYER_PROXIMITY_FAR)
				iLandDisputeWeight +=        GC.getLAND_DISPUTE_FAR();
			else if(eProximity == PLAYER_PROXIMITY_CLOSE)
				iLandDisputeWeight +=        GC.getLAND_DISPUTE_CLOSE();
			else if(eProximity == PLAYER_PROXIMITY_NEIGHBORS)
				iLandDisputeWeight +=        GC.getLAND_DISPUTE_NEIGHBORS();



			if(GetPlayer()->IsCramped())
			{
				iLandDisputeWeight +=       GC.getLAND_DISPUTE_CRAMPED_MULTIPLIER();
			}















			iExpansionFlavor =       GC.getDEFAULT_FLAVOR_VALUE();

			for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
			{
				if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_EXPANSION")
				{
					iExpansionFlavor = GetPlayer()->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
					break;
				}
			}


			iLandDisputeWeight *= iExpansionFlavor;


			if(iLandDisputeWeight >=         GC.getLAND_DISPUTE_FIERCE_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_FIERCE;
			else if(iLandDisputeWeight >=         GC.getLAND_DISPUTE_STRONG_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_STRONG;
			else if(iLandDisputeWeight >=         GC.getLAND_DISPUTE_WEAK_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_WEAK;


			SetLandDisputeLevel(ePlayer, eDisputeLevel);
		}
	}
}


int CvDiplomacyAI::GetTotalLandDisputeLevel()
{
	int iRtnValue = 0;
	PlayerTypes ePlayer;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(ePlayer))
		{
			switch(GetLandDisputeLevel(ePlayer))
			{
			case DISPUTE_LEVEL_FIERCE:
				iRtnValue += GC.getAI_DIPLO_LAND_DISPUTE_WEIGHT_FIERCE();
				break;
			case DISPUTE_LEVEL_STRONG:
				iRtnValue += GC.getAI_DIPLO_LAND_DISPUTE_WEIGHT_STRONG();
				break;
			case DISPUTE_LEVEL_WEAK:
				iRtnValue += GC.getAI_DIPLO_LAND_DISPUTE_WEIGHT_WEAK();
				break;
			}
		}
	}
	return iRtnValue;
}


DisputeLevelTypes CvDiplomacyAI::GetLastTurnLandDisputeLevel(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_paePlayerLastTurnLandDisputeLevel[ePlayer];
}


DisputeLevelTypes CvDiplomacyAI::GetEstimateOtherPlayerLandDisputeLevel(PlayerTypes ePlayer, PlayerTypes eWithPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_ppaaeOtherPlayerLandDisputeLevel[ePlayer][eWithPlayer];
}


void CvDiplomacyAI::SetEstimateOtherPlayerLandDisputeLevel(PlayerTypes ePlayer, PlayerTypes eWithPlayer, DisputeLevelTypes eDisputeLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel >= 0, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel < NUM_DISPUTE_LEVELS, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaeOtherPlayerLandDisputeLevel[ePlayer][eWithPlayer] = eDisputeLevel;
}


bool CvDiplomacyAI::IsPlayerRecklessExpander(PlayerTypes ePlayer)
{
#ifdef NQ_GAME_OPTION_DISABLE_RECKLESS_EXPANDER
	if (GC.getGame().isOption("GAMEOPTION_DISABLE_RECKLESS_EXPANDER"))
		return false;
#endif


	if(GetPlayer()->GetProximityToPlayer(ePlayer) < PLAYER_PROXIMITY_CLOSE)
		return false;


	int iNumCities = GET_PLAYER(ePlayer).getNumCities();
	if(iNumCities < 4)
		return false;

	double fAverageNumCities = 0;
	int iNumPlayers = 0;


	PlayerTypes eLoopPlayer;
	CvPlayer* pPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		pPlayer = &GET_PLAYER(eLoopPlayer);


		if(!pPlayer->isAlive())
			continue;


		if(eLoopPlayer == ePlayer)
			continue;

		iNumPlayers++;
		fAverageNumCities += pPlayer->getNumCities();
	}


	if(iNumPlayers == 0)
	{
		CvAssertMsg(false, "0 players to evaluate when trying to identify if someone is a reckless expander. Not sure how this would happen without the game being over yet.");
		return false;
	}

	fAverageNumCities /= iNumPlayers;


	if(iNumCities < fAverageNumCities * 1.5)
		return false;


	if(GetPlayerMilitaryStrengthComparedToUs(ePlayer) >= STRENGTH_AVERAGE)
		return false;

	return true;
}

int CvDiplomacyAI::GetBrokenExpansionPromiseValue(PlayerTypes ePlayer)
{
	return m_paiBrokenExpansionPromiseValue[(int)ePlayer];
}

void CvDiplomacyAI::SetBrokenExpansionPromiseValue(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	m_paiBrokenExpansionPromiseValue[(int)ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeBrokenExpansionPromiseValue(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	if (iChange != 0)
	{
		m_paiBrokenExpansionPromiseValue[(int)ePlayer] += iChange;
		if (m_paiBrokenExpansionPromiseValue[(int)ePlayer] < 0)
		{
			m_paiBrokenExpansionPromiseValue[(int)ePlayer] = 0;
		}
	}
}

int CvDiplomacyAI::GetIgnoredExpansionPromiseValue(PlayerTypes ePlayer)
{
	return m_paiIgnoredExpansionPromiseValue[(int)ePlayer];	
}

void CvDiplomacyAI::SetIgnoredExpansionPromiseValue(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	m_paiIgnoredExpansionPromiseValue[(int)ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeIgnoredExpansionPromiseValue(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	if (iChange != 0)
	{
		m_paiIgnoredExpansionPromiseValue[(int)ePlayer] += iChange;
		if (m_paiIgnoredExpansionPromiseValue[(int)ePlayer] < 0)
		{
			m_paiIgnoredExpansionPromiseValue[(int)ePlayer] = 0;
		}
	}
}

int CvDiplomacyAI::GetBrokenBorderPromiseValue(PlayerTypes ePlayer)
{
	return m_paiBrokenBorderPromiseValue[(int)ePlayer];
}

void CvDiplomacyAI::SetBrokenBorderPromiseValue(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	m_paiBrokenBorderPromiseValue[(int)ePlayer] = iValue;

}

void CvDiplomacyAI::ChangeBrokenBorderPromiseValue(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	if (iChange != 0)
	{
		m_paiBrokenBorderPromiseValue[(int)ePlayer] += iChange;
		if (m_paiBrokenBorderPromiseValue[(int)ePlayer] < 0)
		{
			m_paiBrokenBorderPromiseValue[(int)ePlayer] = 0;
		}
	}
}

int CvDiplomacyAI::GetIgnoredBorderPromiseValue(PlayerTypes ePlayer)
{
	return m_paiIgnoredBorderPromiseValue[(int)ePlayer];	
}

void CvDiplomacyAI::SetIgnoredBorderPromiseValue(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	m_paiIgnoredBorderPromiseValue[(int)ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeIgnoredBorderPromiseValue(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	if (iChange != 0)
	{
		m_paiIgnoredBorderPromiseValue[(int)ePlayer] += iChange;
		if (m_paiIgnoredBorderPromiseValue[(int)ePlayer] < 0)
		{
			m_paiIgnoredBorderPromiseValue[(int)ePlayer] = 0;
		}
	}
}

int CvDiplomacyAI::GetDeclaredWarOnFriendValue(PlayerTypes ePlayer)
{
	return m_paiDeclaredWarOnFriendValue[(int)ePlayer];
}

void CvDiplomacyAI::ChangeDeclaredWarOnFriendValue(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	if (iChange != 0)
	{
		m_paiDeclaredWarOnFriendValue[(int)ePlayer] += iChange;
		if (m_paiDeclaredWarOnFriendValue[(int)ePlayer] < 0)
		{
			m_paiDeclaredWarOnFriendValue[(int)ePlayer] = 0;
		}
	}
}

int CvDiplomacyAI::GetNumCitiesLiberated(PlayerTypes ePlayer)
{
	return m_paiNumCitiesLiberated[(int)ePlayer];
}

void CvDiplomacyAI::ChangeNumCitiesLiberated(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	if (iChange != 0)
	{
		m_paiNumCitiesLiberated[(int)ePlayer] += iChange;
		CvAssertMsg(m_paiNumCitiesLiberated[(int)ePlayer] >= 0, "Num cities liberated is less than zero");
	}
}



int CvDiplomacyAI::GetRecentTradeValue(PlayerTypes ePlayer)
{
	return m_paiTradeValue[(int)ePlayer];
}


void CvDiplomacyAI::ChangeRecentTradeValue(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiTradeValue[ePlayer] += iChange;
#ifdef AUI_WARNING_FIXES
		int iMaxOpinionValue = GC.getDEAL_VALUE_PER_OPINION_WEIGHT() * -(GC.getOPINION_WEIGHT_TRADE_MAX());
#else
		short iMaxOpinionValue = GC.getDEAL_VALUE_PER_OPINION_WEIGHT() * -(GC.getOPINION_WEIGHT_TRADE_MAX());
#endif


		if(m_paiTradeValue[ePlayer] < 0)
		{
			m_paiTradeValue[ePlayer] = 0;
		}
		else if(m_paiTradeValue[ePlayer] > iMaxOpinionValue)
		{
			m_paiTradeValue[ePlayer] = iMaxOpinionValue;
		}
	}
}


int CvDiplomacyAI::GetCommonFoeValue(PlayerTypes ePlayer)
{
	return m_paiCommonFoeValue[(int)ePlayer];
}


void CvDiplomacyAI::ChangeCommonFoeValue(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0 && ePlayer < MAX_MAJOR_CIVS)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiCommonFoeValue[ePlayer] += iChange;
#ifdef AUI_WARNING_FIXES
		int iMaxOpinionValue = GC.getCOMMON_FOE_VALUE_PER_OPINION_WEIGHT() * -(GC.getOPINION_WEIGHT_COMMON_FOE_MAX());
#else
		short iMaxOpinionValue = GC.getCOMMON_FOE_VALUE_PER_OPINION_WEIGHT() * -(GC.getOPINION_WEIGHT_COMMON_FOE_MAX());
#endif


		if(m_paiCommonFoeValue[ePlayer] < 0)
		{
			m_paiCommonFoeValue[ePlayer] = 0;
		}
		else if(m_paiCommonFoeValue[ePlayer] > iMaxOpinionValue)
		{
			m_paiCommonFoeValue[ePlayer] = iMaxOpinionValue;
		}
	}
}


int CvDiplomacyAI::GetRecentAssistValue(PlayerTypes ePlayer)
{
	return m_paiAssistValue[(int)ePlayer];
}


void CvDiplomacyAI::ChangeRecentAssistValue(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiAssistValue[ePlayer] += iChange;
#ifdef AUI_WARNING_FIXES
		int iMaxOpinionValue = GC.getDEAL_VALUE_PER_OPINION_WEIGHT() * GC.getOPINION_WEIGHT_ASSIST_MAX();
#else
		short iMaxOpinionValue = GC.getDEAL_VALUE_PER_OPINION_WEIGHT() * GC.getOPINION_WEIGHT_ASSIST_MAX();
#endif

		if (m_paiAssistValue[ePlayer] < -iMaxOpinionValue)
		{
			m_paiAssistValue[ePlayer] = -iMaxOpinionValue;
		}
		else if (m_paiAssistValue[ePlayer] > iMaxOpinionValue)
		{
			m_paiAssistValue[ePlayer] = iMaxOpinionValue;
		}
	}
}

bool CvDiplomacyAI::IsGaveAssistanceTo(PlayerTypes            )
{
	return false;
}

bool CvDiplomacyAI::IsHasPaidTributeTo(PlayerTypes            )
{
	return false;
}

bool CvDiplomacyAI::IsNukedBy(PlayerTypes ePlayer)
{
	return (GetNumTimesNuked(ePlayer) > 0);
}

bool CvDiplomacyAI::IsCapitalCapturedBy(PlayerTypes ePlayer)
{
	if(m_pPlayer->IsHasLostCapital() && m_pPlayer->GetCapitalConqueror() == ePlayer)
	{
		return true;
	}
	return false;
}

bool CvDiplomacyAI::IsAngryAboutProtectedMinorKilled(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	int iNumProtectedMinorsKilled = GetOtherPlayerNumProtectedMinorsKilled(ePlayer);
	if (iNumProtectedMinorsKilled > 0)
	{
		int iTurnsSinceMinorKilled = GetTurnsSincePlayerKilledProtectedMinor(ePlayer);
		if (iTurnsSinceMinorKilled > -1 && iTurnsSinceMinorKilled <         GC.getOPINION_WEIGHT_KILLED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN())
		{
			return true;
		}
	}
	return false;
}

bool CvDiplomacyAI::IsAngryAboutProtectedMinorAttacked(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;


	if (IsAngryAboutProtectedMinorKilled(ePlayer))
		return false;
	
	int iNumProtectedMinorsAttacked = GetOtherPlayerNumProtectedMinorsAttacked(ePlayer);
	if(iNumProtectedMinorsAttacked > 0)
	{
		int iTurnsSinceMinorAttack = GetTurnsSincePlayerAttackedProtectedMinor(ePlayer);
		if (iTurnsSinceMinorAttack > -1 && iTurnsSinceMinorAttack <        GC.getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN())
		{
			return true;
		}
	}
	return false;
}

bool CvDiplomacyAI::IsAngryAboutProtectedMinorBullied(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	int iNumProtectedMinorsBullied = GetOtherPlayerNumProtectedMinorsBullied(ePlayer);
	if (iNumProtectedMinorsBullied > 0)
	{
		int iTurnsSinceBully = GetTurnsSincePlayerBulliedProtectedMinor(ePlayer);
		if (iTurnsSinceBully > -1 && iTurnsSinceBully <        GC.getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN())
		{
			return true;
		}
	}
	return false;
}


bool CvDiplomacyAI::IsAngryAboutSidedWithTheirProtectedMinor(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;


	if (IsOtherPlayerSidedWithProtectedMinor(ePlayer))
	{

		int iTurnsSinceDispute = GetTurnsSinceOtherPlayerSidedWithProtectedMinor(ePlayer);
		if (iTurnsSinceDispute > -1 && iTurnsSinceDispute <        GC.getOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_NUM_TURNS_UNTIL_FORGIVEN())
		{
			return true;
		}
	}
	return false;
}


int CvDiplomacyAI::GetTurnsSinceWeLikedTheirProposal(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;

	return m_paiOtherPlayerTurnsSinceWeLikedTheirProposal[ePlayer];
}

void CvDiplomacyAI::SetTurnsSinceWeLikedTheirProposal(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_paiOtherPlayerTurnsSinceWeLikedTheirProposal[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeTurnsSinceWeLikedTheirProposal(PlayerTypes ePlayer, int iChange)
{
	SetTurnsSinceWeLikedTheirProposal(ePlayer, GetTurnsSinceWeLikedTheirProposal(ePlayer) + iChange);
}


int CvDiplomacyAI::GetTurnsSinceWeDislikedTheirProposal(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;

	return m_paiOtherPlayerTurnsSinceWeDislikedTheirProposal[ePlayer];
}

void CvDiplomacyAI::SetTurnsSinceWeDislikedTheirProposal(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_paiOtherPlayerTurnsSinceWeDislikedTheirProposal[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeTurnsSinceWeDislikedTheirProposal(PlayerTypes ePlayer, int iChange)
{
	SetTurnsSinceWeDislikedTheirProposal(ePlayer, GetTurnsSinceWeDislikedTheirProposal(ePlayer) + iChange);
}


int CvDiplomacyAI::GetTurnsSinceTheySupportedOurProposal(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;

	return m_paiOtherPlayerTurnsSinceTheySupportedOurProposal[ePlayer];
}

void CvDiplomacyAI::SetTurnsSinceTheySupportedOurProposal(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_paiOtherPlayerTurnsSinceTheySupportedOurProposal[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeTurnsSinceTheySupportedOurProposal(PlayerTypes ePlayer, int iChange)
{
	SetTurnsSinceTheySupportedOurProposal(ePlayer, GetTurnsSinceTheySupportedOurProposal(ePlayer) + iChange);
}


int CvDiplomacyAI::GetTurnsSinceTheyFoiledOurProposal(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;

	return m_paiOtherPlayerTurnsSinceTheyFoiledOurProposal[ePlayer];
}

void CvDiplomacyAI::SetTurnsSinceTheyFoiledOurProposal(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_paiOtherPlayerTurnsSinceTheyFoiledOurProposal[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeTurnsSinceTheyFoiledOurProposal(PlayerTypes ePlayer, int iChange)
{
	SetTurnsSinceTheyFoiledOurProposal(ePlayer, GetTurnsSinceTheyFoiledOurProposal(ePlayer) + iChange);
}


int CvDiplomacyAI::GetTurnsSinceTheySupportedOurHosting(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;

	return m_paiOtherPlayerTurnsSinceTheySupportedOurHosting[ePlayer];
}

void CvDiplomacyAI::SetTurnsSinceTheySupportedOurHosting(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_paiOtherPlayerTurnsSinceTheySupportedOurHosting[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeTurnsSinceTheySupportedOurHosting(PlayerTypes ePlayer, int iChange)
{
	SetTurnsSinceTheySupportedOurHosting(ePlayer, GetTurnsSinceTheySupportedOurHosting(ePlayer) + iChange);
}



void CvDiplomacyAI::DoUpdateEstimateOtherPlayerLandDisputeLevels()
{
	PlayerTypes eLoopOtherPlayer;
	int iOtherPlayerLoop;

	PlayerTypes eLoopPlayer;
	int iPlayerLoop;


	int iWorldAverageNumCities = 0;
	int iNumPlayers = 0;
	for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(eLoopPlayer).isAlive())
		{
			iWorldAverageNumCities += GET_PLAYER(eLoopPlayer).getNumCities();
			iNumPlayers++;
		}
	}

	iWorldAverageNumCities *= 100;
	iWorldAverageNumCities /= iNumPlayers;

	int iCityRatio;
	int iExpansionFlavorGuess;

	int iLandDisputeWeight;

	DisputeLevelTypes eDisputeLevel;

	PlayerProximityTypes eProximity;


	for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{


			iCityRatio = GET_PLAYER(eLoopPlayer).getNumCities() * 10000;
			iCityRatio /= iWorldAverageNumCities;

			if(iCityRatio >=         GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_10())
				iExpansionFlavorGuess = 10;
			else if(iCityRatio >=         GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_9())
				iExpansionFlavorGuess = 9;
			else if(iCityRatio >=         GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_8())
				iExpansionFlavorGuess = 8;
			else if(iCityRatio >=         GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_7())
				iExpansionFlavorGuess = 7;
			else if(iCityRatio >=         GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_6())
				iExpansionFlavorGuess = 6;
			else if(iCityRatio >=        GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_5())
				iExpansionFlavorGuess = 5;
			else if(iCityRatio >=        GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_4())
				iExpansionFlavorGuess = 4;
			else if(iCityRatio >=        GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_3())
				iExpansionFlavorGuess = 3;
			else if(iCityRatio >=        GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_2())
				iExpansionFlavorGuess = 2;
			else
				iExpansionFlavorGuess = 1;

			LogOtherPlayerExpansionGuess(eLoopPlayer, iExpansionFlavorGuess);


			for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_MAJOR_CIVS; iOtherPlayerLoop++)
			{
				eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;


				if(eLoopPlayer != eLoopOtherPlayer)
				{

					if(IsPlayerValid(eLoopOtherPlayer) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopOtherPlayer))
					{
						eDisputeLevel = DISPUTE_LEVEL_NONE;
						iLandDisputeWeight = 0;


						eProximity = GET_PLAYER(eLoopPlayer).GetProximityToPlayer(eLoopOtherPlayer);

						if(eProximity == PLAYER_PROXIMITY_DISTANT)
							iLandDisputeWeight +=       GC.getLAND_DISPUTE_DISTANT();
						else if(eProximity == PLAYER_PROXIMITY_FAR)
							iLandDisputeWeight +=       GC.getLAND_DISPUTE_FAR();
						else if(eProximity == PLAYER_PROXIMITY_CLOSE)
							iLandDisputeWeight +=        GC.getLAND_DISPUTE_CLOSE();
						else if(eProximity == PLAYER_PROXIMITY_NEIGHBORS)
							iLandDisputeWeight +=        GC.getLAND_DISPUTE_NEIGHBORS();


						if(GET_PLAYER(eLoopPlayer).IsCramped())
						{
							iLandDisputeWeight *=       GC.getLAND_DISPUTE_CRAMPED_MULTIPLIER();
							iLandDisputeWeight /= 100;
						}


						iLandDisputeWeight *= iExpansionFlavorGuess;












						if(iLandDisputeWeight >=         GC.getLAND_DISPUTE_FIERCE_THRESHOLD())
							eDisputeLevel = DISPUTE_LEVEL_FIERCE;
						else if(iLandDisputeWeight >=         GC.getLAND_DISPUTE_STRONG_THRESHOLD())
							eDisputeLevel = DISPUTE_LEVEL_STRONG;
						else if(iLandDisputeWeight >=         GC.getLAND_DISPUTE_WEAK_THRESHOLD())
							eDisputeLevel = DISPUTE_LEVEL_WEAK;


						SetEstimateOtherPlayerLandDisputeLevel(eLoopPlayer, eLoopOtherPlayer, eDisputeLevel);
					}
				}
			}
		}
	}
}



AggressivePostureTypes CvDiplomacyAI::GetExpansionAggressivePosture(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (AggressivePostureTypes) m_paeExpansionAggressivePosture[ePlayer];
}


void CvDiplomacyAI::SetExpansionAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture >= 0, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture < NUM_AGGRESSIVE_POSTURE_TYPES, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeExpansionAggressivePosture[ePlayer] = ePosture;
}


void CvDiplomacyAI::DoUpdateExpansionAggressivePostures()
{
	if(GetPlayer()->getCapitalCity() == NULL)
		return;


	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		DoUpdateOnePlayerExpansionAggressivePosture(eLoopPlayer);
	}
}


void CvDiplomacyAI::DoUpdateOnePlayerExpansionAggressivePosture(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(GetPlayer()->getCapitalCity() == NULL)
		return;

	if(!IsPlayerValid(ePlayer))
		return;


	CvTeam &kMyTeam = GET_TEAM(GetPlayer()->getTeam());
	if (!kMyTeam.isAtWar(GET_PLAYER(ePlayer).getTeam()) && !kMyTeam.canDeclareWar(GET_PLAYER(ePlayer).getTeam()))
	{
		return;
	}

	int iMyCapitalX = GetPlayer()->getCapitalCity()->getX();
	int iMyCapitalY = GetPlayer()->getCapitalCity()->getY();

	AggressivePostureTypes eAggressivePosture;

	CvCity* pLoopCity;
	int iCityLoop;

	AggressivePostureTypes eMostAggressiveCityPosture;
	int iNumMostAggressiveCities;

	int iDistanceUsToThem, iDistanceThemToTheirCapital, iDistanceCapitals;
	int iTheirCapitalX, iTheirCapitalY;
	int iCityX, iCityY;


	if(GET_PLAYER(ePlayer).getCapitalCity() == NULL)
		return;

	iTheirCapitalX = GET_PLAYER(ePlayer).getCapitalCity()->getX();
	iTheirCapitalY = GET_PLAYER(ePlayer).getCapitalCity()->getY();

	iDistanceCapitals = plotDistance(iTheirCapitalX, iTheirCapitalY, iMyCapitalX, iMyCapitalY);

	eMostAggressiveCityPosture = AGGRESSIVE_POSTURE_NONE;
	iNumMostAggressiveCities = 0;


	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
	{

		if(pLoopCity->isCapital())
			continue;


		if(pLoopCity->getOriginalOwner() != pLoopCity->getOwner())
			continue;

		eAggressivePosture = AGGRESSIVE_POSTURE_NONE;

		iCityX = pLoopCity->getX();
		iCityY = pLoopCity->getY();


		iDistanceUsToThem = plotDistance(iMyCapitalX, iMyCapitalY, iCityX, iCityY);
		iDistanceThemToTheirCapital = plotDistance(iTheirCapitalX, iTheirCapitalY, iCityX, iCityY);

		if(iDistanceUsToThem <=       GC.getEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_HIGH())
			eAggressivePosture = AGGRESSIVE_POSTURE_HIGH;
		else if(iDistanceUsToThem <=       GC.getEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_MEDIUM())
			eAggressivePosture = AGGRESSIVE_POSTURE_MEDIUM;
		else if(iDistanceUsToThem <=       GC.getEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_LOW())
			eAggressivePosture = AGGRESSIVE_POSTURE_LOW;


		if(eAggressivePosture == AGGRESSIVE_POSTURE_LOW)
		{
			if(iDistanceUsToThem < iDistanceThemToTheirCapital)
				eAggressivePosture = AGGRESSIVE_POSTURE_MEDIUM;
		}


		if(eAggressivePosture >= AGGRESSIVE_POSTURE_MEDIUM)
		{
			if(iDistanceCapitals < iDistanceThemToTheirCapital)
				eAggressivePosture = AGGRESSIVE_POSTURE_INCREDIBLE;
		}


		if(eAggressivePosture == eMostAggressiveCityPosture)
			iNumMostAggressiveCities++;


		else if(eAggressivePosture > eMostAggressiveCityPosture)
		{
			eMostAggressiveCityPosture = eAggressivePosture;
			iNumMostAggressiveCities = 0;
		}


		if(eMostAggressiveCityPosture == AGGRESSIVE_POSTURE_INCREDIBLE)
			break;
	}


	if(iNumMostAggressiveCities > 1)
	{

		if(eMostAggressiveCityPosture > AGGRESSIVE_POSTURE_LOW && eMostAggressiveCityPosture < AGGRESSIVE_POSTURE_INCREDIBLE)
		{
			eMostAggressiveCityPosture = AggressivePostureTypes(eMostAggressiveCityPosture + 1);
		}
	}

	SetExpansionAggressivePosture(ePlayer, eMostAggressiveCityPosture);
}



AggressivePostureTypes CvDiplomacyAI::GetPlotBuyingAggressivePosture(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (AggressivePostureTypes) m_paePlotBuyingAggressivePosture[ePlayer];
}


void CvDiplomacyAI::SetPlotBuyingAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture >= 0, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture < NUM_AGGRESSIVE_POSTURE_TYPES, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paePlotBuyingAggressivePosture[ePlayer] = ePosture;
}


void CvDiplomacyAI::DoUpdatePlotBuyingAggressivePosture()
{
	PlayerTypes eLoopPlayer;

	AggressivePostureTypes ePosture;

	int iCityLoop;
	CvCity* pLoopCity;

	int iAggressionScore;


	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			iAggressionScore = 0;


			for(pLoopCity = GetPlayer()->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iCityLoop))
			{
				iAggressionScore += pLoopCity->AI_GetNumPlotsAcquiredByOtherPlayer(eLoopPlayer);
			}


			if(iAggressionScore >=        GC.getPLOT_BUYING_POSTURE_INCREDIBLE_THRESHOLD())
				ePosture = AGGRESSIVE_POSTURE_INCREDIBLE;
			else if(iAggressionScore >=       GC.getPLOT_BUYING_POSTURE_HIGH_THRESHOLD())
				ePosture = AGGRESSIVE_POSTURE_HIGH;
			else if(iAggressionScore >=       GC.getPLOT_BUYING_POSTURE_MEDIUM_THRESHOLD())
				ePosture = AGGRESSIVE_POSTURE_MEDIUM;
			else if(iAggressionScore >=       GC.getPLOT_BUYING_POSTURE_LOW_THRESHOLD())
				ePosture = AGGRESSIVE_POSTURE_LOW;
			else
				ePosture = AGGRESSIVE_POSTURE_NONE;

			SetPlotBuyingAggressivePosture(eLoopPlayer, ePosture);
		}
	}
}



DisputeLevelTypes CvDiplomacyAI::GetVictoryDisputeLevel(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_paePlayerVictoryDisputeLevel[ePlayer];
}


void CvDiplomacyAI::SetVictoryDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel >= 0, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel < NUM_DISPUTE_LEVELS, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerVictoryDisputeLevel[ePlayer] = eDisputeLevel;
}


void CvDiplomacyAI::DoUpdateVictoryDisputeLevels()
{
	AIGrandStrategyTypes eMyGrandStrategy = GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy();

	PlayerTypes ePlayer;

	DisputeLevelTypes eDisputeLevel;

	int iVictoryDisputeWeight;


	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(ePlayer))
		{
			eDisputeLevel = DISPUTE_LEVEL_NONE;


			if(!GET_PLAYER(ePlayer).isMinorCiv())
			{
				iVictoryDisputeWeight = 0;


				if(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(ePlayer) == eMyGrandStrategy)
				{
					switch(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategyConfidence(ePlayer))
					{
					case GUESS_CONFIDENCE_POSITIVE:
						iVictoryDisputeWeight +=        GC.getVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_POSITIVE();
						break;
					case GUESS_CONFIDENCE_LIKELY:
						iVictoryDisputeWeight +=        GC.getVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_LIKELY();
						break;
					case GUESS_CONFIDENCE_UNSURE:
						iVictoryDisputeWeight +=       GC.getVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_UNSURE();
						break;
					}
				}


				iVictoryDisputeWeight *= GetVictoryCompetitiveness();







				if(iVictoryDisputeWeight >=        GC.getVICTORY_DISPUTE_FIERCE_THRESHOLD())
					eDisputeLevel = DISPUTE_LEVEL_FIERCE;
				else if(iVictoryDisputeWeight >=        GC.getVICTORY_DISPUTE_STRONG_THRESHOLD())
					eDisputeLevel = DISPUTE_LEVEL_STRONG;
				else if(iVictoryDisputeWeight >=        GC.getVICTORY_DISPUTE_WEAK_THRESHOLD())
					eDisputeLevel = DISPUTE_LEVEL_WEAK;
			}


			SetVictoryDisputeLevel(ePlayer, eDisputeLevel);
		}
	}
}



DisputeLevelTypes CvDiplomacyAI::GetEstimateOtherPlayerVictoryDisputeLevel(PlayerTypes ePlayer, PlayerTypes eWithPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_ppaaeOtherPlayerVictoryDisputeLevel[ePlayer][eWithPlayer];
}


void CvDiplomacyAI::SetEstimateOtherPlayerVictoryDisputeLevel(PlayerTypes ePlayer, PlayerTypes eWithPlayer, DisputeLevelTypes eDisputeLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel >= 0, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel < NUM_DISPUTE_LEVELS, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaeOtherPlayerVictoryDisputeLevel[ePlayer][eWithPlayer] = eDisputeLevel;
}


void CvDiplomacyAI::DoUpdateEstimateOtherPlayerVictoryDisputeLevels()
{
	PlayerTypes eLoopOtherPlayer;
	int iOtherPlayerLoop;

	int iVictoryDisputeWeight;

	DisputeLevelTypes eDisputeLevel;


	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{

			for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_MAJOR_CIVS; iOtherPlayerLoop++)
			{
				eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;


				if(eLoopPlayer != eLoopOtherPlayer)
				{

					if(IsPlayerValid(eLoopOtherPlayer) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopOtherPlayer))
					{
						eDisputeLevel = DISPUTE_LEVEL_NONE;
						iVictoryDisputeWeight = 0;


#ifdef AUI_WARNING_FIXES
						const CvGrandStrategyAI* pGrandStrategyAI = GetPlayer()->GetGrandStrategyAI();
						if (pGrandStrategyAI->GetGuessOtherPlayerActiveGrandStrategy(eLoopPlayer) == pGrandStrategyAI->GetGuessOtherPlayerActiveGrandStrategy(eLoopOtherPlayer))
						{
							switch (pGrandStrategyAI->GetGuessOtherPlayerActiveGrandStrategyConfidence(eLoopPlayer))
#else
						if(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(eLoopPlayer) == GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(eLoopOtherPlayer))
						{
							switch(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategyConfidence(eLoopPlayer))
#endif
							{
							case GUESS_CONFIDENCE_POSITIVE:
								iVictoryDisputeWeight +=       GC.getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_POSITIVE();
								break;
							case GUESS_CONFIDENCE_LIKELY:
								iVictoryDisputeWeight +=       GC.getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_LIKELY();
								break;
							case GUESS_CONFIDENCE_UNSURE:
								iVictoryDisputeWeight +=       GC.getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_UNSURE();
								break;
							}
#ifdef AUI_WARNING_FIXES
							switch (pGrandStrategyAI->GetGuessOtherPlayerActiveGrandStrategyConfidence(eLoopOtherPlayer))
#else
							switch(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategyConfidence(eLoopOtherPlayer))
#endif
							{
							case GUESS_CONFIDENCE_POSITIVE:
								iVictoryDisputeWeight +=       GC.getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_POSITIVE();
								break;
							case GUESS_CONFIDENCE_LIKELY:
								iVictoryDisputeWeight +=       GC.getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_LIKELY();
								break;
							case GUESS_CONFIDENCE_UNSURE:
								iVictoryDisputeWeight +=       GC.getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_UNSURE();
								break;
							}


							iVictoryDisputeWeight *=       GC.getDEFAULT_FLAVOR_VALUE();







							if(iVictoryDisputeWeight >=        GC.getVICTORY_DISPUTE_OTHER_PLAYER_FIERCE_THRESHOLD())
								eDisputeLevel = DISPUTE_LEVEL_FIERCE;
							else if(iVictoryDisputeWeight >=        GC.getVICTORY_DISPUTE_OTHER_PLAYER_STRONG_THRESHOLD())
								eDisputeLevel = DISPUTE_LEVEL_STRONG;
							else if(iVictoryDisputeWeight >=        GC.getVICTORY_DISPUTE_OTHER_PLAYER_WEAK_THRESHOLD())
								eDisputeLevel = DISPUTE_LEVEL_WEAK;
						}


						SetEstimateOtherPlayerVictoryDisputeLevel(eLoopPlayer, eLoopOtherPlayer, eDisputeLevel);
					}
				}
			}
		}
	}
}



DisputeLevelTypes CvDiplomacyAI::GetWonderDisputeLevel(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_paePlayerWonderDisputeLevel[ePlayer];
}


void CvDiplomacyAI::SetWonderDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel >= 0, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel < NUM_DISPUTE_LEVELS, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerWonderDisputeLevel[ePlayer] = eDisputeLevel;
}


void CvDiplomacyAI::DoUpdateWonderDisputeLevels()
{
	PlayerTypes ePlayer;

	DisputeLevelTypes eDisputeLevel;

	int iWonderDisputeWeight;


	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(ePlayer))
		{
			eDisputeLevel = DISPUTE_LEVEL_NONE;

			iWonderDisputeWeight = GetNumWondersBeatenTo(ePlayer);


			iWonderDisputeWeight *= GetWonderCompetitiveness();


			if(iWonderDisputeWeight >=        GC.getWONDER_DISPUTE_FIERCE_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_FIERCE;
			else if(iWonderDisputeWeight >=       GC.getWONDER_DISPUTE_STRONG_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_STRONG;
			else if(iWonderDisputeWeight >=       GC.getWONDER_DISPUTE_WEAK_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_WEAK;


			SetWonderDisputeLevel(ePlayer, eDisputeLevel);
		}
	}
}



DisputeLevelTypes CvDiplomacyAI::GetMinorCivDisputeLevel(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_paePlayerMinorCivDisputeLevel[ePlayer];
}


void CvDiplomacyAI::SetMinorCivDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel >= 0, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel < NUM_DISPUTE_LEVELS, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerMinorCivDisputeLevel[ePlayer] = eDisputeLevel;
}


void CvDiplomacyAI::DoUpdateMinorCivDisputeLevels()
{
	PlayerTypes ePlayer;

	DisputeLevelTypes eDisputeLevel;

	int iMinorCivDisputeWeight;


	int iPersonalityMod = GetMinorCivCompetitiveness() * GetMinorCivCompetitiveness();

	int iMinorCivLoop;
	PlayerTypes eMinor;


	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(ePlayer))
		{
			eDisputeLevel = DISPUTE_LEVEL_NONE;
			iMinorCivDisputeWeight = 0;


			for(iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
			{
				eMinor = (PlayerTypes) iMinorCivLoop;


				if(GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
				{

					if(GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(ePlayer))
						iMinorCivDisputeWeight += iPersonalityMod*        GC.getMINOR_CIV_DISPUTE_ALLIES_WEIGHT();

					else if(GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(ePlayer))
						iMinorCivDisputeWeight += iPersonalityMod*       GC.getMINOR_CIV_DISPUTE_FRIENDS_WEIGHT();
				}
			}


			if(iMinorCivDisputeWeight >=         GC.getMINOR_CIV_DISPUTE_FIERCE_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_FIERCE;
			else if(iMinorCivDisputeWeight >=         GC.getMINOR_CIV_DISPUTE_STRONG_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_STRONG;
			else if(iMinorCivDisputeWeight >=         GC.getMINOR_CIV_DISPUTE_WEAK_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_WEAK;


			SetMinorCivDisputeLevel(ePlayer, eDisputeLevel);
		}
	}
}


int CvDiplomacyAI::GetNumWondersBeatenTo(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiNumWondersBeatenTo[ePlayer];
}


void CvDiplomacyAI::SetNumWondersBeatenTo(PlayerTypes ePlayer, int iNewValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iNewValue >= 0, "DIPLOMACY_AI: Setting NumWondersBeatenTo to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiNumWondersBeatenTo[ePlayer] = iNewValue;
}


void CvDiplomacyAI::ChangeNumWondersBeatenTo(PlayerTypes ePlayer, int iChange)
{
	SetNumWondersBeatenTo(ePlayer, GetNumWondersBeatenTo(ePlayer) + iChange);
}


WarDamageLevelTypes CvDiplomacyAI::GetWarDamageLevel(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarDamageLevelTypes) m_paeWarDamageLevel[ePlayer];
}

void CvDiplomacyAI::SetWarDamageLevel(PlayerTypes ePlayer, WarDamageLevelTypes eDamageLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDamageLevel >= 0, "DIPLOMACY_AI: Invalid WarDamageLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDamageLevel < NUM_WAR_DAMAGE_LEVEL_TYPES, "DIPLOMACY_AI: Invalid WarDamageLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeWarDamageLevel[ePlayer] = eDamageLevel;
}


void CvDiplomacyAI::DoUpdateWarDamageLevel()
{
	PlayerTypes eLoopPlayer;

	WarDamageLevelTypes eWarDamageLevel;

	int iValueLost;
	int iCurrentValue;
	int iValueLostRatio;

	CvCity* pLoopCity;
	CvUnit* pLoopUnit;
	int iValueLoop;



	iCurrentValue = 0;

	int iTypicalPower = m_pPlayer->GetMilitaryAI()->GetPowerOfStrongestBuildableUnit(DOMAIN_LAND);

	for(pLoopCity = GetPlayer()->firstCity(&iValueLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iValueLoop))
	{
		iCurrentValue += (pLoopCity->getPopulation() *         GC.getWAR_DAMAGE_LEVEL_INVOLVED_CITY_POP_MULTIPLIER());
		if (pLoopCity->IsOriginalCapital())
		{
			iCurrentValue *= 3;
			iCurrentValue /= 2;
		}
	}


	for(pLoopUnit = GetPlayer()->firstUnit(&iValueLoop); pLoopUnit != NULL; pLoopUnit = GetPlayer()->nextUnit(&iValueLoop))
	{
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
		if(pkUnitInfo)
		{
			int iUnitValue = pkUnitInfo->GetPower();
			if(iTypicalPower > 0)
			{
				iUnitValue = iUnitValue*         GC.getDEFAULT_WAR_VALUE_FOR_UNIT() / iTypicalPower;
			}
			else
			{
				iUnitValue =         GC.getDEFAULT_WAR_VALUE_FOR_UNIT();
			}
			iCurrentValue += iUnitValue;
		}
	}


	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			eWarDamageLevel = WAR_DAMAGE_LEVEL_NONE;

			iValueLost = GetWarValueLost(eLoopPlayer);

			if(iValueLost > 0)
			{

				if(iCurrentValue > 0)
					iValueLostRatio = iValueLost * 100 / (iCurrentValue + iValueLost);
				else
					iValueLostRatio = iValueLost;

				if(iValueLostRatio >=        GC.getWAR_DAMAGE_LEVEL_THRESHOLD_CRIPPLED())
					eWarDamageLevel = WAR_DAMAGE_LEVEL_CRIPPLED;
				else if(iValueLostRatio >=        GC.getWAR_DAMAGE_LEVEL_THRESHOLD_SERIOUS())
					eWarDamageLevel = WAR_DAMAGE_LEVEL_SERIOUS;
				else if(iValueLostRatio >=        GC.getWAR_DAMAGE_LEVEL_THRESHOLD_MAJOR())
					eWarDamageLevel = WAR_DAMAGE_LEVEL_MAJOR;
				else if(iValueLostRatio >=        GC.getWAR_DAMAGE_LEVEL_THRESHOLD_MINOR())
					eWarDamageLevel = WAR_DAMAGE_LEVEL_MINOR;
			}

			SetWarDamageLevel(eLoopPlayer, eWarDamageLevel);
		}
	}
}


void CvDiplomacyAI::DoWarDamageDecay()
{
	if((int)m_eTargetPlayer >= (int)DIPLO_FIRST_PLAYER)
		return;

	int iValue;

	TeamTypes eLoopThirdTeam;
	PlayerTypes eLoopThirdPlayer;
	int iThirdPlayerLoop;


	TeamTypes eLoopTeam;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

		if(IsPlayerValid(eLoopPlayer,                    true))
		{

			if(!IsAtWar(eLoopPlayer))
			{
				iValue = GetWarValueLost(eLoopPlayer);

				if(iValue > 0)
				{

					iValue /= 20;


					iValue = max(1, iValue);

					ChangeWarValueLost(eLoopPlayer, -iValue);
				}
			}


			for(iThirdPlayerLoop = 0; iThirdPlayerLoop < MAX_CIV_PLAYERS; iThirdPlayerLoop++)
			{
				eLoopThirdPlayer = (PlayerTypes) iThirdPlayerLoop;
				eLoopThirdTeam = GET_PLAYER(eLoopThirdPlayer).getTeam();


				if(!GET_TEAM(eLoopThirdTeam).isAtWar(eLoopTeam))
				{
					iValue = GetOtherPlayerWarValueLost(eLoopPlayer, eLoopThirdPlayer);

					if(iValue > 0)
					{

						iValue /= 20;


						iValue = max(1, iValue);

						ChangeOtherPlayerWarValueLost(eLoopPlayer, eLoopThirdPlayer, -iValue);
					}
				}
			}
		}
	}
}


int CvDiplomacyAI::GetWarValueLost(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiWarValueLost[ePlayer];
}


void CvDiplomacyAI::SetWarValueLost(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting WarValueLost to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiWarValueLost[ePlayer] = iValue;


	if(iValue == 0)
		m_paeWarDamageLevel[ePlayer] = WAR_DAMAGE_LEVEL_NONE;
}


void CvDiplomacyAI::ChangeWarValueLost(PlayerTypes ePlayer, int iChange)
{
	SetWarValueLost(ePlayer, GetWarValueLost(ePlayer) + iChange);

	if(iChange > 0)
	{

		PlayerTypes eLoopPlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(eLoopPlayer != ePlayer && eLoopPlayer != m_pPlayer->GetID() && IsPlayerValid(eLoopPlayer))
			{

				CvPlayer& kOtherPlayer = GET_PLAYER(eLoopPlayer);
				if(IsAtWar(eLoopPlayer))
				{
					kOtherPlayer.GetDiplomacyAI()->ChangeCommonFoeValue(ePlayer, iChange);
				}
			}
		}
	}
}



WarDamageLevelTypes CvDiplomacyAI::GetOtherPlayerWarDamageLevel(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarDamageLevelTypes) m_ppaaeOtherPlayerWarDamageLevel[ePlayer][eLostToPlayer];
}


void CvDiplomacyAI::SetOtherPlayerWarDamageLevel(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, WarDamageLevelTypes eDamageLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDamageLevel >= 0, "DIPLOMACY_AI: Invalid WarDamageLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDamageLevel < NUM_WAR_DAMAGE_LEVEL_TYPES, "DIPLOMACY_AI: Invalid WarDamageLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaeOtherPlayerWarDamageLevel[ePlayer][eLostToPlayer] = eDamageLevel;
}


void CvDiplomacyAI::DoUpdateOtherPlayerWarDamageLevel()
{
	PlayerTypes eLoopOtherPlayer;
	int iOtherPlayerLoop;

	int iValueLost;
	int iCurrentValue;
	int iValueLostRatio;

	CvCity* pLoopCity;
	int iValueLoop;

	WarDamageLevelTypes eWarDamageLevel;


	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{

			for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_MAJOR_CIVS; iOtherPlayerLoop++)
			{
				eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;


				if(eLoopPlayer != eLoopOtherPlayer)
				{

					if(IsPlayerValid(eLoopOtherPlayer) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopOtherPlayer))
					{

						if(GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).isAtWar(GET_PLAYER(eLoopOtherPlayer).getTeam()))
						{
							iValueLost = GetOtherPlayerWarValueLost(eLoopPlayer, eLoopOtherPlayer);


							iCurrentValue = 0;


							if(GET_PLAYER(eLoopPlayer).getNumCities() > 0)
							{
								for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iValueLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iValueLoop))
								{
									iCurrentValue += (pLoopCity->getPopulation() *         GC.getWAR_DAMAGE_LEVEL_INVOLVED_CITY_POP_MULTIPLIER());
									if (pLoopCity->IsOriginalCapital())
									{
										iCurrentValue *= 3;
										iCurrentValue /= 2;
									}
								}
							}


							iCurrentValue = max(1,iCurrentValue);

							iValueLostRatio = iValueLost * 100 / iCurrentValue;

							if(iValueLostRatio >=        GC.getWAR_DAMAGE_LEVEL_THRESHOLD_CRIPPLED())
								eWarDamageLevel = WAR_DAMAGE_LEVEL_CRIPPLED;
							else if(iValueLostRatio >=        GC.getWAR_DAMAGE_LEVEL_THRESHOLD_SERIOUS())
								eWarDamageLevel = WAR_DAMAGE_LEVEL_SERIOUS;
							else if(iValueLostRatio >=        GC.getWAR_DAMAGE_LEVEL_THRESHOLD_MAJOR())
								eWarDamageLevel = WAR_DAMAGE_LEVEL_MAJOR;
							else if(iValueLostRatio >=        GC.getWAR_DAMAGE_LEVEL_THRESHOLD_MINOR())
								eWarDamageLevel = WAR_DAMAGE_LEVEL_MINOR;
							else
								eWarDamageLevel = WAR_DAMAGE_LEVEL_NONE;
						}

						else
							eWarDamageLevel = WAR_DAMAGE_LEVEL_NONE;

						SetOtherPlayerWarDamageLevel(eLoopPlayer, eLoopOtherPlayer, eWarDamageLevel);
					}
				}
			}
		}
	}
}


int CvDiplomacyAI::GetOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_ppaaiOtherPlayerWarValueLost[ePlayer][eLostToPlayer];
}


void CvDiplomacyAI::SetOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting OtherPlayerWarValueLost to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaiOtherPlayerWarValueLost[ePlayer][eLostToPlayer] = iValue;


	if(iValue == 0)
		m_ppaaeOtherPlayerWarDamageLevel[ePlayer][eLostToPlayer] = WAR_DAMAGE_LEVEL_NONE;
}


void CvDiplomacyAI::ChangeOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iChange)
{
	SetOtherPlayerWarValueLost(ePlayer, eLostToPlayer, GetOtherPlayerWarValueLost(ePlayer, eLostToPlayer) + iChange);
}


void CvDiplomacyAI::CacheOtherPlayerWarValuesLost()
{
	for(int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayer;
		for(int iLostToPlayer = 0; iLostToPlayer < MAX_CIV_PLAYERS; iLostToPlayer++)
		{
			PlayerTypes eLostToPlayer = (PlayerTypes) iLostToPlayer;
			SetOtherPlayerLastRoundWarValueLost(ePlayer, eLostToPlayer, GetOtherPlayerWarValueLost(ePlayer, eLostToPlayer));
		}
	}
}


int CvDiplomacyAI::GetOtherPlayerLastRoundWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eLostToPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eLostToPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	return m_ppaaiOtherPlayerLastRoundWarValueLost[ePlayer][eLostToPlayer];
}


void CvDiplomacyAI::SetOtherPlayerLastRoundWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eLostToPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eLostToPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting OtherPlayerLastRoundWarValueLost to a negative value.  Please send Anton your save file and version.");
	m_ppaaiOtherPlayerLastRoundWarValueLost[ePlayer][eLostToPlayer] = iValue;
}


AggressivePostureTypes CvDiplomacyAI::GetMilitaryAggressivePosture(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (AggressivePostureTypes) m_paeMilitaryAggressivePosture[ePlayer];
}


void CvDiplomacyAI::SetMilitaryAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture >= 0, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture < NUM_AGGRESSIVE_POSTURE_TYPES, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeMilitaryAggressivePosture[ePlayer] = ePosture;
}



AggressivePostureTypes CvDiplomacyAI::GetLastTurnMilitaryAggressivePosture(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (AggressivePostureTypes) m_paeLastTurnMilitaryAggressivePosture[ePlayer];
}


void CvDiplomacyAI::SetLastTurnMilitaryAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture >= 0, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture < NUM_AGGRESSIVE_POSTURE_TYPES, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeLastTurnMilitaryAggressivePosture[ePlayer] = ePosture;
}


void CvDiplomacyAI::DoUpdateMilitaryAggressivePostures()
{

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		DoUpdateOnePlayerMilitaryAggressivePosture(eLoopPlayer);
	}
}


void CvDiplomacyAI::DoUpdateOnePlayerMilitaryAggressivePosture(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(!IsPlayerValid(ePlayer))
		return;

	AggressivePostureTypes eAggressivePosture;
	AggressivePostureTypes eLastTurnAggressivePosture;

	int iUnitValueOnMyHomeFront;
	int iValueToAdd;
	bool bIsAtWarWithSomeone;

	CvUnit* pLoopUnit;
	int iUnitLoop;

	int iOtherPlayerLoop;
	PlayerTypes eLoopOtherPlayer;


	eLastTurnAggressivePosture = GetMilitaryAggressivePosture(ePlayer);
	if(eLastTurnAggressivePosture != NO_AGGRESSIVE_POSTURE_TYPE)
		SetLastTurnMilitaryAggressivePosture(ePlayer, eLastTurnAggressivePosture);

	iUnitValueOnMyHomeFront = 0;
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());
	bIsAtWarWithSomeone = (kTeam.getAtWarCount(false) > 0);

	TeamTypes eOurTeam = GetTeam();
	PlayerTypes eOurPlayerID = GetPlayer()->GetID();


	for(pLoopUnit = kPlayer.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iUnitLoop))
	{

		if(pLoopUnit->IsCombatUnit())
		{
			CvPlot* pUnitPlot = pLoopUnit->plot();

			if(pUnitPlot->isVisible(eOurTeam))
			{

				if(pUnitPlot->IsHomeFrontForPlayer(eOurPlayerID))
				{

					if(bIsAtWarWithSomeone)
					{

						for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_CIV_PLAYERS; iOtherPlayerLoop++)
						{
							eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;


							if(eLoopOtherPlayer != ePlayer && eLoopOtherPlayer != eOurPlayerID)
							{

								if(kTeam.isAtWar(GET_PLAYER(eLoopOtherPlayer).getTeam()))
								{
									if(GET_PLAYER(eLoopOtherPlayer).isAlive())
									{
										if(pUnitPlot->IsHomeFrontForPlayer(eLoopOtherPlayer))
										{
											continue;
										}
									}
								}
							}
						}
					}

					iValueToAdd = 10;


					if(pLoopUnit->plot()->isOwned())
					{
						if(pLoopUnit->plot()->getOwner() == ePlayer)
							iValueToAdd /= 2;
					}


					iUnitValueOnMyHomeFront += iValueToAdd;
				}
			}
		}
	}


	if(iUnitValueOnMyHomeFront >=        GC.getMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_INCREDIBLE())
		eAggressivePosture = AGGRESSIVE_POSTURE_INCREDIBLE;
	else if(iUnitValueOnMyHomeFront >=        GC.getMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_HIGH())
		eAggressivePosture = AGGRESSIVE_POSTURE_HIGH;
	else if(iUnitValueOnMyHomeFront >=        GC.getMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_MEDIUM())
		eAggressivePosture = AGGRESSIVE_POSTURE_MEDIUM;
	else if(iUnitValueOnMyHomeFront >=        GC.getMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_LOW())
		eAggressivePosture = AGGRESSIVE_POSTURE_LOW;
	else
		eAggressivePosture = AGGRESSIVE_POSTURE_NONE;

	SetMilitaryAggressivePosture(ePlayer, eAggressivePosture);
}










int CvDiplomacyAI::GetVictoryCompetitiveness() const
{
	return m_iVictoryCompetitiveness;
}


int CvDiplomacyAI::GetWonderCompetitiveness() const
{
	return m_iWonderCompetitiveness;
}


int CvDiplomacyAI::GetMinorCivCompetitiveness() const
{
	return m_iMinorCivCompetitiveness;
}


int CvDiplomacyAI::GetBoldness() const
{
	return m_iBoldness;
}


int CvDiplomacyAI::GetDiploBalance() const
{
	return m_iDiploBalance;
}


int CvDiplomacyAI::GetWarmongerHate() const
{
	return m_iWarmongerHate;
}


int CvDiplomacyAI::GetDenounceWillingness() const
{
	return m_iDenounceWillingness;
}


int CvDiplomacyAI::GetDoFWillingness() const
{
	return m_iDoFWillingness;
}


int CvDiplomacyAI::GetLoyalty() const
{
	return m_iLoyalty;
}


int CvDiplomacyAI::GetNeediness() const
{
	return m_iNeediness;
}


int CvDiplomacyAI::GetForgiveness() const
{
	return m_iForgiveness;
}


int CvDiplomacyAI::GetChattiness() const
{
	return m_iChattiness;
}


int CvDiplomacyAI::GetMeanness() const
{
	return m_iMeanness;
}


int CvDiplomacyAI::GetPersonalityMajorCivApproachBias(MajorCivApproachTypes eApproach) const
{
	CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Trying to queury invalid Major Approach for personality bias.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach < NUM_MAJOR_CIV_APPROACHES, "DIPLOMACY_AI: Trying to queury invalid Major Approach for personality bias.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiPersonalityMajorCivApproachBiases[eApproach];
}


int CvDiplomacyAI::GetPersonalityMinorCivApproachBias(MinorCivApproachTypes eApproach) const
{
	CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Trying to queury invalid Minor Approach for personality bias.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach < NUM_MINOR_CIV_APPROACHES, "DIPLOMACY_AI: Trying to queury invalid Minor Approach for personality bias.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiPersonalityMinorCivApproachBiases[eApproach];
}










void CvDiplomacyAI::DoWeMadePeaceWithSomeone(TeamTypes eOtherTeam)
{
	CvAssertMsg(eOtherTeam >= 0, "DIPLOMACY_AI: Invalid Team Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherTeam < MAX_CIV_TEAMS, "DIPLOMACY_AI: Invalid Team Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	PlayerTypes eThirdPlayer;
	int iThirdPlayerLoop;

	PlayerTypes ePeacePlayer;
	for(int iPeacePlayerLoop = 0; iPeacePlayerLoop < MAX_CIV_PLAYERS; iPeacePlayerLoop++)
	{
		ePeacePlayer = (PlayerTypes) iPeacePlayerLoop;

		if(GET_PLAYER(ePeacePlayer).getTeam() == eOtherTeam)
		{

			SetMusteringForAttack(ePeacePlayer, false);

			if(!GET_PLAYER(ePeacePlayer).isMinorCiv())
			{

				for(iThirdPlayerLoop = 0; iThirdPlayerLoop < MAX_MAJOR_CIVS; iThirdPlayerLoop++)
				{
					eThirdPlayer = (PlayerTypes) iThirdPlayerLoop;

					SetCoopWarAcceptedState(eThirdPlayer, ePeacePlayer, NO_COOP_WAR_STATE);


				}


				if(GetMajorCivApproach(ePeacePlayer,                       false) == MAJOR_CIV_APPROACH_WAR)
					SetMajorCivApproach(ePeacePlayer, MAJOR_CIV_APPROACH_NEUTRAL);
			}
			else
			{

				ResetSentAttackProtectedMinorTaunts(ePeacePlayer);
			}
		}
	}
}


void CvDiplomacyAI::DoPlayerDeclaredWarOnSomeone(PlayerTypes ePlayer, TeamTypes eOtherTeam)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherTeam >= 0, "DIPLOMACY_AI: Invalid Team Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherTeam <= MAX_CIV_TEAMS, "DIPLOMACY_AI: Invalid Team Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(GET_TEAM(eOtherTeam).isBarbarian())
		return;

	CvAssertMsg(eOtherTeam < MAX_CIV_TEAMS, "DIPLOMACY_AI: Invalid Team Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iThirdPartyLoop;
	PlayerTypes eThirdParty;
	PlayerTypes eThem;

	bool bWasFriends = false;

	PlayerTypes eAttackedPlayer;
	for(int iAttackedPlayerLoop = 0; iAttackedPlayerLoop < MAX_CIV_PLAYERS; iAttackedPlayerLoop++)
	{
		eAttackedPlayer = (PlayerTypes) iAttackedPlayerLoop;

		if(GET_PLAYER(eAttackedPlayer).getTeam() == eOtherTeam)
		{
			bWasFriends = false;


			if(ePlayer == GetPlayer()->GetID() || eAttackedPlayer == GetPlayer()->GetID())
			{
				eThem = GetPlayer()->GetID() != ePlayer ? ePlayer : eAttackedPlayer;

				ChangeNumWarsFought(eThem, 1);


				if(!GET_PLAYER(eThem).isMinorCiv())
				{

					if(IsDoFAccepted(eThem))
					{
						bWasFriends = true;

						SetDoFAccepted(eThem, false);
						SetDoFCounter(eThem, -666);
					}


					if(IsPlayerNoSettleRequestAccepted(eThem))
					{
						SetPlayerNoSettleRequestAccepted(eThem, false);
						SetPlayerNoSettleRequestCounter(eThem, -666);
					}


					if(IsPlayerStopSpyingRequestAccepted(eThem))
					{
						SetPlayerStopSpyingRequestAccepted(eThem, false);
						SetPlayerStopSpyingRequestCounter(eThem, -666);
					}


					for(iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
					{
						eThirdParty = (PlayerTypes) iThirdPartyLoop;









						if(GetCoopWarAcceptedState(eThem, eThirdParty) >= COOP_WAR_STATE_SOON)
						{
							SetCoopWarAcceptedState(eThem, eThirdParty, NO_COOP_WAR_STATE);
							SetCoopWarCounter(eThem, eThirdParty, -666);
						}
					}
				}


				if(eAttackedPlayer == GetPlayer()->GetID())
				{

					if(bWasFriends)
					{
						SetFriendDeclaredWarOnUs(eThem, true);
						ChangeDeclaredWarOnFriendValue(eThem, GC.getOPINION_WEIGHT_WAR_FRIEND_EACH() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
					}

					if(!GET_PLAYER(ePlayer).isMinorCiv())
					{
						SetMajorCivApproach(ePlayer, MAJOR_CIV_APPROACH_WAR);
						SetWarFaceWithPlayer(ePlayer, WAR_FACE_NEUTRAL);
					}


					if(IsPlayerMadeMilitaryPromise(ePlayer))
						SetPlayerBrokenMilitaryPromise(ePlayer, true);


					if(GetNumCiviliansReturnedToMe(ePlayer) > 0)
						ChangeNumCiviliansReturnedToMe(ePlayer, -GetNumCiviliansReturnedToMe(ePlayer));
					if(GetNumLandmarksBuiltForMe(ePlayer) > 0)
						ChangeNumLandmarksBuiltForMe(ePlayer, -GetNumLandmarksBuiltForMe(ePlayer));


					ChangeRecentTradeValue(ePlayer, -GetRecentTradeValue(ePlayer));
					ChangeCommonFoeValue(ePlayer, -GetCommonFoeValue(ePlayer));
					if (GetRecentAssistValue(ePlayer) < 0)
					{
						ChangeRecentAssistValue(ePlayer, -GetRecentAssistValue(ePlayer));
					}


					GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFCounter(GetPlayer()->GetID(), -1);
					GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFAccepted(GetPlayer()->GetID(), false);
				}
			}


			if(ePlayer == GetPlayer()->GetID())
			{
			}

			else if(IsPlayerValid(eAttackedPlayer))
			{
				if(GET_PLAYER(eAttackedPlayer).isMinorCiv())
				{

					if(GET_PLAYER(eAttackedPlayer).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
					{
						SetOtherPlayerTurnsSinceAttackedProtectedMinor(ePlayer, 0);
						SetOtherPlayerProtectedMinorAttacked(ePlayer, eAttackedPlayer);
						ChangeOtherPlayerNumProtectedMinorsAttacked(ePlayer, 1);
					}
				}
			}
		}
	}
}


void CvDiplomacyAI::DoPlayerKilledSomeone(PlayerTypes ePlayer, PlayerTypes eDeadPlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDeadPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDeadPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPlayerValid(ePlayer))
	{

		if(GET_PLAYER(eDeadPlayer).isMinorCiv())
		{
			ChangeOtherPlayerNumMinorsConquered(ePlayer, 1);


			if(GET_PLAYER(eDeadPlayer).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
			{
				SetOtherPlayerTurnsSinceKilledProtectedMinor(ePlayer, 0);
				SetOtherPlayerProtectedMinorKilled(ePlayer, eDeadPlayer);
				ChangeOtherPlayerNumProtectedMinorsKilled(ePlayer, 1);


				if(IsPlayerMadeAttackCityStatePromise(ePlayer))
					SetPlayerBrokenAttackCityStatePromise(ePlayer, true);
			}
		}

		else
			ChangeOtherPlayerNumMajorsConquered(ePlayer, 1);
	}
}


void CvDiplomacyAI::DoPlayerBulliedSomeone(PlayerTypes ePlayer, PlayerTypes eOtherPlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(IsPlayerValid(ePlayer) && ePlayer != GetPlayer()->GetID())
	{
		if(GET_PLAYER(eOtherPlayer).isMinorCiv())
		{

			if(GET_PLAYER(eOtherPlayer).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
			{				
				ChangeOtherPlayerNumProtectedMinorsBullied(ePlayer, 1);
			}
		}
	}
}


void CvDiplomacyAI::DoPlayerMetSomeone(PlayerTypes ePlayer, PlayerTypes eOtherPlayer)
{

	if(IsPlayerValid(ePlayer) && IsPlayerValid(eOtherPlayer))
	{
		if(!GET_PLAYER(ePlayer).isMinorCiv())
		{

			PublicDeclarationTypes eDeclaration;
			int iData1;
			int iData2;
			bool bActive;

			for(int iLoop = 0; iLoop < MAX_DIPLO_LOG_STATEMENTS; iLoop++)
			{

				if(GetDeclarationLogMustHaveMetPlayerForIndex(iLoop) == eOtherPlayer)
				{
					eDeclaration = GetDeclarationLogTypeForIndex(iLoop);


					if(eDeclaration != NO_PUBLIC_DECLARATION_TYPE)
					{
						bActive = IsDeclarationLogForIndexActive(iLoop);


						if(bActive)
						{
							iData1 = GetDeclarationLogData1ForIndex(iLoop);
							iData2 = GetDeclarationLogData2ForIndex(iLoop);

							DoMakePublicDeclaration(eDeclaration, iData1, iData2, eOtherPlayer, ePlayer);
						}
					}
				}
			}
		}
	}
}


int CvDiplomacyAI::GetTurnsSincePlayerAttackedProtectedMinor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(GetOtherPlayerProtectedMinorAttacked(ePlayer) == NO_PLAYER)
		return -1;

	return m_paiOtherPlayerTurnsSinceAttackedProtectedMinor[ePlayer];
}


void CvDiplomacyAI::SetOtherPlayerTurnsSinceAttackedProtectedMinor(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting OtherPlayerTurnsSinceAttackedProtectedMinor to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerTurnsSinceAttackedProtectedMinor[ePlayer] = iValue;
}


void CvDiplomacyAI::ChangeOtherPlayerTurnsSinceAttackedProtectedMinor(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerTurnsSinceAttackedProtectedMinor(ePlayer, GetTurnsSincePlayerAttackedProtectedMinor(ePlayer) + iChange);
}


PlayerTypes CvDiplomacyAI::GetOtherPlayerProtectedMinorAttacked(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (PlayerTypes) m_paiOtherPlayerProtectedMinorAttacked[ePlayer];
}


void CvDiplomacyAI::SetOtherPlayerProtectedMinorAttacked(PlayerTypes ePlayer, PlayerTypes eAttackedPlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eAttackedPlayer >= MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eAttackedPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerProtectedMinorAttacked[ePlayer] = eAttackedPlayer;
}


int CvDiplomacyAI::GetOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiOtherPlayerNumProtectedMinorsAttacked[ePlayer];
}


void CvDiplomacyAI::SetOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of protected Minors to be negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerNumProtectedMinorsAttacked[ePlayer] = iValue;
}


void CvDiplomacyAI::ChangeOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerNumProtectedMinorsAttacked(ePlayer, GetOtherPlayerNumProtectedMinorsAttacked(ePlayer) + iChange);
}


int CvDiplomacyAI::GetTurnsSincePlayerKilledProtectedMinor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(GetOtherPlayerProtectedMinorKilled(ePlayer) == NO_PLAYER)
		return -1;

	return m_paiOtherPlayerTurnsSinceKilledProtectedMinor[ePlayer];
}


void CvDiplomacyAI::SetOtherPlayerTurnsSinceKilledProtectedMinor(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of turns since protected Minor was killed to be negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerTurnsSinceKilledProtectedMinor[ePlayer] = iValue;
}


void CvDiplomacyAI::ChangeOtherPlayerTurnsSinceKilledProtectedMinor(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerTurnsSinceKilledProtectedMinor(ePlayer, GetTurnsSincePlayerKilledProtectedMinor(ePlayer) + iChange);
}


PlayerTypes CvDiplomacyAI::GetOtherPlayerProtectedMinorKilled(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (PlayerTypes) m_paiOtherPlayerProtectedMinorKilled[ePlayer];
}


void CvDiplomacyAI::SetOtherPlayerProtectedMinorKilled(PlayerTypes ePlayer, PlayerTypes eKilledPlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eKilledPlayer >= MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eKilledPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerProtectedMinorKilled[ePlayer] = eKilledPlayer;
}


int CvDiplomacyAI::GetOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiOtherPlayerNumProtectedMinorsKilled[ePlayer];
}


void CvDiplomacyAI::SetOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerNumProtectedMinorsKilled[ePlayer] = iValue;
}


void CvDiplomacyAI::ChangeOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerNumProtectedMinorsKilled(ePlayer, GetOtherPlayerNumProtectedMinorsKilled(ePlayer) + iChange);
}


int CvDiplomacyAI::GetTurnsSincePlayerBulliedProtectedMinor(PlayerTypes eBullyPlayer)
{
	CvAssertMsg(eBullyPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eBullyPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");

	PlayerTypes eMostRecentBulliedMinor = GetOtherPlayerProtectedMinorBullied(eBullyPlayer);
	if(eMostRecentBulliedMinor == NO_PLAYER)
		return -1;

	int iMostRecentBullyTurn = GET_PLAYER(eMostRecentBulliedMinor).GetMinorCivAI()->GetTurnLastBulliedByMajor(eBullyPlayer);
	return (GC.getGame().getGameTurn() - iMostRecentBullyTurn);
}


PlayerTypes CvDiplomacyAI::GetOtherPlayerProtectedMinorBullied(PlayerTypes eBullyPlayer)
{
	CvAssertMsg(eBullyPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eBullyPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");

	int iMostRecentBullyTurn = -1;
	PlayerTypes eMostRecentBulliedMinor = NO_PLAYER;

	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinor = (PlayerTypes) iMinorLoop;
		if(IsPlayerValid(eMinor))
		{
			if(GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
			{
				if(GET_PLAYER(eMinor).GetMinorCivAI()->IsEverBulliedByMajor(eBullyPlayer))
				{
					int iBullyTurn = GET_PLAYER(eMinor).GetMinorCivAI()->GetTurnLastBulliedByMajor(eBullyPlayer);
					if(iBullyTurn > iMostRecentBullyTurn)
					{
						iMostRecentBullyTurn = iBullyTurn;
						eMostRecentBulliedMinor = eMinor;
					}
				}
			}
		}
	}

	return eMostRecentBulliedMinor;
}


int CvDiplomacyAI::GetOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	return m_paiOtherPlayerNumProtectedMinorsBullied[ePlayer];
}


void CvDiplomacyAI::SetOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of protected Minors to be negative.  Please send Anton your save file and version.");
	m_paiOtherPlayerNumProtectedMinorsBullied[ePlayer] = iValue;
}


void CvDiplomacyAI::ChangeOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerNumProtectedMinorsBullied(ePlayer, GetOtherPlayerNumProtectedMinorsBullied(ePlayer) + iChange);
}


bool CvDiplomacyAI::IsOtherPlayerSidedWithProtectedMinor(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	return (GetTurnsSinceOtherPlayerSidedWithProtectedMinor(ePlayer) >= 0);
}

int CvDiplomacyAI::GetTurnsSinceOtherPlayerSidedWithProtectedMinor(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1;

	return m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor[ePlayer];
}

void CvDiplomacyAI::SetOtherPlayerTurnsSinceSidedWithProtectedMinor(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeOtherPlayerTurnsSinceSidedWithProtectedMinor(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	SetOtherPlayerTurnsSinceSidedWithProtectedMinor(ePlayer, GetTurnsSinceOtherPlayerSidedWithProtectedMinor(ePlayer) + iChange);
}


int CvDiplomacyAI::GetOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiOtherPlayerNumMinorsAttacked[ePlayer];
}


void CvDiplomacyAI::SetOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of minors attacked to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerNumMinorsAttacked[ePlayer] = iValue;
}


void CvDiplomacyAI::ChangeOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerNumMinorsAttacked(ePlayer, GetOtherPlayerNumMinorsAttacked(ePlayer) + iChange);
	ChangeOtherPlayerWarmongerAmount(ePlayer, iChange *          GC.getWARMONGER_THREAT_MINOR_ATTACKED_WEIGHT());
}


int CvDiplomacyAI::GetOtherPlayerNumMinorsConquered(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiOtherPlayerNumMinorsConquered[ePlayer];
}


void CvDiplomacyAI::SetOtherPlayerNumMinorsConquered(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of minors conquered to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerNumMinorsConquered[ePlayer] = iValue;
}


void CvDiplomacyAI::ChangeOtherPlayerNumMinorsConquered(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerNumMinorsConquered(ePlayer, GetOtherPlayerNumMinorsConquered(ePlayer) + iChange);

}


int CvDiplomacyAI::GetOtherPlayerNumMajorsAttacked(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiOtherPlayerNumMajorsAttacked[ePlayer];
}


void CvDiplomacyAI::SetOtherPlayerNumMajorsAttacked(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of Majors attacked to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerNumMajorsAttacked[ePlayer] = iValue;
}


void CvDiplomacyAI::ChangeOtherPlayerNumMajorsAttacked(PlayerTypes ePlayer, int iChange, TeamTypes eAttackedTeam)
{
	if(iChange > 0)
	{
		PlayerTypes eAttackedPlayer;
		for(int iAttackedPlayerLoop = 0; iAttackedPlayerLoop < MAX_MAJOR_CIVS; iAttackedPlayerLoop++)
		{
			eAttackedPlayer = (PlayerTypes) iAttackedPlayerLoop;


			if(GET_PLAYER(eAttackedPlayer).getTeam() != eAttackedTeam)
				continue;


			if(IsAtWar(eAttackedPlayer))
				return;
		}
	}

	SetOtherPlayerNumMajorsAttacked(ePlayer, GetOtherPlayerNumMajorsAttacked(ePlayer) + iChange);
	ChangeOtherPlayerWarmongerAmount(ePlayer, iChange *          GC.getWARMONGER_THREAT_MAJOR_ATTACKED_WEIGHT());
}


int CvDiplomacyAI::GetOtherPlayerNumMajorsConquered(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiOtherPlayerNumMajorsConquered[ePlayer];
}


void CvDiplomacyAI::SetOtherPlayerNumMajorsConquered(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of Majors conquered to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerNumMajorsConquered[ePlayer] = iValue;
}


void CvDiplomacyAI::ChangeOtherPlayerNumMajorsConquered(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerNumMajorsConquered(ePlayer, GetOtherPlayerNumMajorsConquered(ePlayer) + iChange);

}


int CvDiplomacyAI::GetOtherPlayerWarmongerAmount(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");	
	return m_paiOtherPlayerWarmongerAmount[ePlayer];
}


void CvDiplomacyAI::ChangeOtherPlayerWarmongerAmount(PlayerTypes ePlayer, int iChangeAmount)
{
	int iNewValue = m_paiOtherPlayerWarmongerAmount[ePlayer] + (iChangeAmount * GC.getEraInfo(GC.getGame().getCurrentEra())->getWarmongerPercent()) / 100;
	iNewValue = max(0, iNewValue);
	m_paiOtherPlayerWarmongerAmount[ePlayer] = iNewValue;
}


int CvDiplomacyAI::GetOtherPlayerWarmongerScore(PlayerTypes ePlayer)
{
	int iReturnValue = GetOtherPlayerWarmongerAmount(ePlayer);




	iReturnValue *= GetWarmongerHate();
	iReturnValue /= 100;
	return iReturnValue;
}






void CvDiplomacyAI::DoFirstContact(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(ePlayer != GetPlayer()->GetID())
	{
		DoFirstContactInitRelationship(ePlayer);
















		if(!GC.getGame().isNetworkMultiPlayer())
		{
			if(!GetPlayer()->isHuman())
			{

				if(GET_PLAYER(ePlayer).isHuman())
				{
					if(!IsAtWar(ePlayer))
					{
						if(GC.getGame().isFinalInitialized())
						{
							if(std::find(m_aGreetPlayers.begin(), m_aGreetPlayers.end(), ePlayer) == m_aGreetPlayers.end())
							{

								m_aGreetPlayers.push_back(ePlayer);
							}
						}
					}
				}
			}
			else
			{

				CvPlayer& kTargetPlayer = GET_PLAYER(ePlayer);
				if(kTargetPlayer.isHuman())
				{
					if(!IsAtWar(ePlayer))
					{
						if(GC.getGame().isFinalInitialized())
						{
							CvNotifications* pNotifications = kTargetPlayer.GetNotifications();
							if(pNotifications)
							{
								CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_MET_MINOR_CIV", GetPlayer()->getNameKey());
								pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strBuffer, -1, -1, GetPlayer()->GetID());
							}
						}
					}
				}
			}
		}



		if(!GET_PLAYER(ePlayer).isMinorCiv())
		{
			PublicDeclarationTypes eDeclaration;
			int iData1;
			int iData2;
			PlayerTypes eMustHaveMetPlayer;
			bool bActive;

			for(int iLoop = 0; iLoop < MAX_DIPLO_LOG_STATEMENTS; iLoop++)
			{
				eDeclaration = GetDeclarationLogTypeForIndex(iLoop);


				if(eDeclaration != NO_PUBLIC_DECLARATION_TYPE)
				{
					bActive = IsDeclarationLogForIndexActive(iLoop);

					iData1 = GetDeclarationLogData1ForIndex(iLoop);
					iData2 = GetDeclarationLogData2ForIndex(iLoop);


					if(eDeclaration == PUBLIC_DECLARATION_PROTECT_MINOR)
					{
						CvAssert(((PlayerTypes) iData1) != NO_PLAYER);
						if(((PlayerTypes) iData1) != NO_PLAYER)
						{
							if(!GET_PLAYER((PlayerTypes) iData1).isAlive())
								bActive = false;
						}
					}
					else if(eDeclaration == PUBLIC_DECLARATION_ABANDON_MINOR)
					{
						CvAssert(((PlayerTypes) iData1) != NO_PLAYER);
						if(((PlayerTypes) iData1) != NO_PLAYER)
						{
							if(!GET_PLAYER((PlayerTypes) iData1).isAlive())
								bActive = false;
						}
					}


					if(bActive)
					{
						eMustHaveMetPlayer = GetDeclarationLogMustHaveMetPlayerForIndex(iLoop);

						DoMakePublicDeclaration(eDeclaration, iData1, iData2, eMustHaveMetPlayer, ePlayer);
					}
				}
			}
		}
	}
}

void CvDiplomacyAI::DoFirstContactInitRelationship(PlayerTypes ePlayer)
{
	DoUpdateOnePlayerMilitaryStrength(ePlayer);
	DoUpdateOnePlayerEconomicStrength(ePlayer);

	DoUpdateOnePlayerMilitaryAggressivePosture(ePlayer);
	DoUpdateOnePlayerExpansionAggressivePosture(ePlayer);

	DoUpdateOnePlayerTargetValue(ePlayer);


	if(!GET_PLAYER(ePlayer).isMinorCiv())
	{
		DoUpdateOnePlayerOpinion(ePlayer);





		int iHighestWeight;
		WarFaceTypes eWarFace;


		MajorCivApproachTypes eApproach = GetBestApproachTowardsMajorCiv(ePlayer,                                        iHighestWeight,                         true,          true,                               eWarFace);


		if(eApproach == MAJOR_CIV_APPROACH_WAR && GetWarFaceWithPlayer(ePlayer) == NO_WAR_FACE_TYPE)
			SetWarFaceWithPlayer(ePlayer, eWarFace);

		CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


		SetMajorCivApproach(ePlayer, eApproach);
	}


	else
	{
		SetMinorCivApproach(ePlayer, MINOR_CIV_APPROACH_IGNORE);
	}
}


void CvDiplomacyAI::DoKilledByPlayer(PlayerTypes ePlayer)
{
	if(ePlayer == GC.getGame().getActivePlayer() && !GC.getGame().isNetworkMultiPlayer())
	{
		const char* szText = GetDiploStringForMessage(DIPLO_MESSAGE_DEFEATED);
		gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_DEFEATED);

		if(!GC.getGame().isGameMultiPlayer())
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_DESTROY_CIV);

			CvAchievementUnlocker::AlexanderConquest(ePlayer);
		}
	}
}


void CvDiplomacyAI::DoSendStatementToPlayer(PlayerTypes ePlayer, DiploStatementTypes eStatement, int iData1, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eStatement >= 0, "DIPLOMACY_AI: Invalid DiploStatementType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eStatement < NUM_DIPLO_LOG_STATEMENT_TYPES, "DIPLOMACY_AI: Invalid DiploStatementType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* szText;

	bool bHuman = GET_PLAYER(ePlayer).isHuman();
	bool bShouldShowLeaderScene = bHuman;


	if(eStatement == DIPLO_STATEMENT_AGGRESSIVE_MILITARY_WARNING)
	{
		if(bShouldShowLeaderScene)
		{
			if(IsActHostileTowardsHuman(ePlayer))
				szText = GetDiploStringForMessage(DIPLO_MESSAGE_HOSTILE_AGGRESSIVE_MILITARY_WARNING);
			else
				szText = GetDiploStringForMessage(DIPLO_MESSAGE_AGGRESSIVE_MILITARY_WARNING);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_AGGRESSIVE_MILITARY_WARNING, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_KILLED_PROTECTED_CITY_STATE)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eMinorCiv = (PlayerTypes) iData1;
			CvAssert(eMinorCiv != NO_PLAYER);
			if(eMinorCiv != NO_PLAYER)
			{
				const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();

				szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_KILLED_PROTECTED_CITY_STATE, NO_PLAYER, strMinorCivKey);
				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}
	}


	else if(eStatement == DIPLO_STATEMENT_ATTACKED_PROTECTED_CITY_STATE)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eMinorCiv = (PlayerTypes) iData1;
			CvAssert(eMinorCiv != NO_PLAYER);
			if(eMinorCiv != NO_PLAYER)
			{
				const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();

				szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_ATTACKED_PROTECTED_CITY_STATE, NO_PLAYER, strMinorCivKey);
				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_YOU_ATTACKED_MINOR_CIV, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}
	}


	else if(eStatement == DIPLO_STATEMENT_BULLIED_PROTECTED_CITY_STATE)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eMinorCiv = (PlayerTypes) iData1;
			CvAssert(eMinorCiv != NO_PLAYER);
			if(eMinorCiv != NO_PLAYER)
			{
				const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();

				szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_BULLIED_PROTECTED_CITY_STATE, NO_PLAYER, strMinorCivKey);
				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_YOU_BULLIED_MINOR_CIV, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}
	}


	else if(eStatement == DIPLO_STATEMENT_EXPANSION_SERIOUS_WARNING)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_EXPANSION_SERIOUS_WARNING);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_SERIOUS_WARNING, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_EXPANSION_WARNING)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_EXPANSION_WARNING);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_WARNING, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_EXPANSION_BROKEN_PROMISE)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_EXPANSION_BROKEN_PROMISE);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_PLOT_BUYING_SERIOUS_WARNING)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_PLOT_BUYING_SERIOUS_WARNING);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_SERIOUS_WARNING, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_PLOT_BUYING_WARNING)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_PLOT_BUYING_WARNING);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_WARNING, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_PLOT_BUYING_BROKEN_PROMISE)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_PLOT_BUYING_BROKEN_PROMISE);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_WE_ATTACKED_YOUR_MINOR)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eMinorCiv = (PlayerTypes) iData1;
			CvAssert(eMinorCiv != NO_PLAYER);
			if(eMinorCiv != NO_PLAYER)
			{
				const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();
				if(IsActHostileTowardsHuman(ePlayer))
					szText = GetDiploStringForMessage(DIPLO_MESSAGE_HOSTILE_WE_ATTACKED_YOUR_MINOR, NO_PLAYER, strMinorCivKey);
				else
					szText = GetDiploStringForMessage(DIPLO_MESSAGE_WE_ATTACKED_YOUR_MINOR, NO_PLAYER, strMinorCivKey);

				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_I_ATTACKED_YOUR_MINOR_CIV, szText, LEADERHEAD_ANIM_POSITIVE, eMinorCiv);


				SetSentAttackProtectedMinorTaunt(ePlayer, eMinorCiv, true);
			}
		}
	}


	else if(eStatement == 	DIPLO_STATEMENT_WE_BULLIED_YOUR_MINOR)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eMinorCiv = (PlayerTypes) iData1;
			CvAssert(eMinorCiv != NO_PLAYER);
			if (eMinorCiv != NO_PLAYER)
			{
				const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();
				if(IsActHostileTowardsHuman(ePlayer))
					szText = GetDiploStringForMessage(DIPLO_MESSAGE_HOSTILE_WE_BULLIED_YOUR_MINOR, NO_PLAYER, strMinorCivKey);
				else
					szText = GetDiploStringForMessage(DIPLO_MESSAGE_WE_BULLIED_YOUR_MINOR, NO_PLAYER, strMinorCivKey);

				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_I_BULLIED_YOUR_MINOR_CIV, szText, LEADERHEAD_ANIM_POSITIVE, eMinorCiv);
			}
		}
	}


	else if(eStatement == DIPLO_STATEMENT_WORK_WITH_US)
	{

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_WORK_WITH_US);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_WORK_WITH_US, szText, LEADERHEAD_ANIM_REQUEST);
		}

		else if(!bHuman)
		{
			SetDoFCounter(ePlayer, 0);
			GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFCounter(GetPlayer()->GetID(), 0);


			if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDoFAcceptable(GetPlayer()->GetID()))
			{
				SetDoFAccepted(ePlayer, true);
				GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFAccepted(GetPlayer()->GetID(), true);

				LogDoF(ePlayer);
			}
		}
	}


	else if(eStatement == DIPLO_STATEMENT_END_WORK_WITH_US)
	{
		SetDoFAccepted(ePlayer, false);
		SetDoFCounter(ePlayer, -666);


		SetPlayerNoSettleRequestAccepted(ePlayer, false);
		SetPlayerNoSettleRequestCounter(ePlayer, -666);


		SetPlayerStopSpyingRequestAccepted(ePlayer, false);
		SetPlayerStopSpyingRequestCounter(ePlayer, -666);


		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_END_WORK_WITH_US, ePlayer);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
		else if(!bHuman)
		{
			GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFAccepted(GetPlayer()->GetID(), false);
			GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFCounter(GetPlayer()->GetID(), -666);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_DENOUNCE)
	{
		DoDenouncePlayer(ePlayer);
		LogDenounce(ePlayer);


		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_WORK_AGAINST_SOMEONE, ePlayer);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_DENOUNCE_FRIEND)
	{
		DoDenouncePlayer(ePlayer);
		LogDenounce(ePlayer,               true);


		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_AI_DOF_BACKSTAB, ePlayer);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_REQUEST_FRIEND_DENOUNCE)
	{
		PlayerTypes eTarget = (PlayerTypes) iData1;
		CvAssert(eTarget != NO_PLAYER);
		if(eTarget != NO_PLAYER)
		{
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();


			if(bShouldShowLeaderScene)
			{
				szText = GetDiploStringForMessage(DIPLO_MESSAGE_DOF_AI_DENOUNCE_REQUEST, ePlayer, strTargetCivKey);

				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_AI_REQUEST_DENOUNCE, szText, LEADERHEAD_ANIM_POSITIVE, eTarget);
			}
			else if(!bHuman)
			{
				bool bAgree = IsDenounceAcceptable(eTarget,           true);

				LogFriendRequestDenounce(ePlayer, eTarget, bAgree);

				if(bAgree)
				{
					GET_PLAYER(ePlayer).GetDiplomacyAI()->DoDenouncePlayer(eTarget);
					GET_PLAYER(ePlayer).GetDiplomacyAI()->LogDenounce(eTarget);


					if(eTarget == GC.getGame().getActivePlayer())
					{
						szText = GetDiploStringForMessage(DIPLO_MESSAGE_WORK_AGAINST_SOMEONE, eTarget);
						CvDiplomacyRequests::SendRequest(ePlayer, eTarget, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
					}
				}
				else
				{

					if(IsFriendDenounceRefusalUnacceptable(ePlayer, eTarget))
					{
						DoDenouncePlayer(ePlayer);
						LogDenounce(ePlayer,               false,              true);
					}
				}
			}
		}
	}

























	else if(eStatement == DIPLO_STATEMENT_COOP_WAR_REQUEST)
	{
		PlayerTypes eAgainstPlayer = (PlayerTypes) iData1;
		CvAssert(eAgainstPlayer != NO_PLAYER);
		if(eAgainstPlayer != NO_PLAYER)
		{

			if(bShouldShowLeaderScene)
			{
				const char* strAgainstPlayerKey = GET_PLAYER(eAgainstPlayer).getNameKey();
				szText = GetDiploStringForMessage(DIPLO_MESSAGE_COOP_WAR_REQUEST, ePlayer, strAgainstPlayerKey);

				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_COOP_WAR, szText, LEADERHEAD_ANIM_POSITIVE, eAgainstPlayer);
			}

			else if(!bHuman)
			{
				SetCoopWarCounter(ePlayer, eAgainstPlayer, 0);
				GET_PLAYER(ePlayer).GetDiplomacyAI()->SetCoopWarCounter(GetPlayer()->GetID(), eAgainstPlayer, 0);


				CoopWarStates eAcceptedState = GET_PLAYER(ePlayer).GetDiplomacyAI()->GetWillingToAgreeToCoopWarState(GetPlayer()->GetID(), eAgainstPlayer);
				GET_PLAYER(ePlayer).GetDiplomacyAI()->SetCoopWarAcceptedState(GetPlayer()->GetID(), eAgainstPlayer, eAcceptedState);

				if(eAcceptedState == COOP_WAR_STATE_ACCEPTED)
				{
					DeclareWar(eAgainstPlayer);
					GetPlayer()->GetMilitaryAI()->RequestBasicAttack(eAgainstPlayer, 1);

					GET_PLAYER(ePlayer).GetDiplomacyAI()->DeclareWar(eAgainstPlayer);
					GET_PLAYER(ePlayer).GetMilitaryAI()->RequestBasicAttack(eAgainstPlayer, 1);

					int iLockedTurns =        GC.getCOOP_WAR_LOCKED_LENGTH();
					GET_TEAM(GetPlayer()->getTeam()).ChangeNumTurnsLockedIntoWar(GET_PLAYER(eAgainstPlayer).getTeam(), iLockedTurns);
					GET_TEAM(GET_PLAYER(ePlayer).getTeam()).ChangeNumTurnsLockedIntoWar(GET_PLAYER(eAgainstPlayer).getTeam(), iLockedTurns);
				}

				LogCoopWar(ePlayer, eAgainstPlayer, eAcceptedState);


				if(eAcceptedState != COOP_WAR_STATE_REJECTED)
					SetCoopWarAcceptedState(ePlayer, eAgainstPlayer, eAcceptedState);
			}
		}
	}


	else if(eStatement == DIPLO_STATEMENT_COOP_WAR_TIME)
	{
		PlayerTypes eAgainstPlayer = (PlayerTypes) iData1;
		CvAssert(eAgainstPlayer != NO_PLAYER);
		if(eAgainstPlayer != NO_PLAYER)
		{

			if(bShouldShowLeaderScene)
			{
				const char* strAgainstPlayerKey = GET_PLAYER(eAgainstPlayer).getNameKey();
				szText = GetDiploStringForMessage(DIPLO_MESSAGE_COOP_WAR_TIME, ePlayer, strAgainstPlayerKey);

				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_COOP_WAR_TIME, szText, LEADERHEAD_ANIM_POSITIVE, eAgainstPlayer);
			}
		}


	}


	else if(eStatement == DIPLO_STATEMENT_DEMAND)
	{

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_DEMAND);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND, szText, LEADERHEAD_ANIM_DEMAND);
		}

		else if(!bHuman)
		{


			CvDeal kDeal = *pDeal;

			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_REQUEST)
	{

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_REQUEST);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST, szText, LEADERHEAD_ANIM_REQUEST);
		}

		else if(!bHuman)
		{


			CvDeal kDeal = *pDeal;

			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_LUXURY_TRADE)
	{

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_LUXURY_TRADE);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
		}

		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;


			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_EMBASSY_EXCHANGE)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_EMBASSY_EXCHANGE);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
		}
		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;


			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_EMBASSY_OFFER)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_EMBASSY_OFFER);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
		}
		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;


			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_OPEN_BORDERS_EXCHANGE)
	{

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_OPEN_BORDERS_EXCHANGE);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
		}

		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;


			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_OPEN_BORDERS_OFFER)
	{

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_OPEN_BORDERS_OFFER);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
		}

		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;


			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_PLAN_RESEARCH_AGREEMENT)
	{

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_PLAN_RESEARCH_AGREEMENT);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_PLAN_RESEARCH_AGREEMENT, szText, LEADERHEAD_ANIM_REQUEST);
		}

		else if(!bHuman)
		{
			if(!GET_PLAYER(ePlayer).GetDiplomacyAI()->IsWantsResearchAgreementWithPlayer(GetPlayer()->GetID()))
				GET_PLAYER(ePlayer).GetDiplomacyAI()->DoAddWantsResearchAgreementWithPlayer(GetPlayer()->GetID());
		}
	}


	else if(eStatement == DIPLO_STATEMENT_RESEARCH_AGREEMENT_OFFER)
	{

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_RESEARCH_AGREEMENT_OFFER);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
		}

		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;


			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_RENEW_DEAL)
	{

		if(bShouldShowLeaderScene)
		{
			int iDealValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer;
			DiploMessageTypes eMessageType = NUM_DIPLO_MESSAGE_TYPES;
			bool bCantMatchOffer;
			bool bDealAcceptable = m_pPlayer->GetDealAI()->IsDealWithHumanAcceptable(pDeal, ePlayer, iDealValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, bCantMatchOffer);

			if(!bDealAcceptable)
			{
				if(iValueTheyreOffering > iValueImOffering)
				{
					bDealAcceptable = true;
				}
			}

			if(bDealAcceptable)
			{
				eMessageType = DIPLO_MESSAGE_RENEW_DEAL;
			}

			else if(iDealValueToMe > -75 &&
			        iValueImOffering < (iValueTheyreOffering * 5))
			{
				eMessageType = DIPLO_MESSAGE_WANT_MORE_RENEW_DEAL;
			}
			else
			{
				CvDeal* pRenewDeal = GetDealToRenew();
				if (pRenewDeal)
				{
					pRenewDeal->m_bCheckedForRenewal = true;
				}
				ClearDealToRenew();
			}

			if(eMessageType != NUM_DIPLO_MESSAGE_TYPES)
			{
				CvDeal* pRenewDeal = GetDealToRenew();
				if (pRenewDeal)
				{
					pRenewDeal->m_bCheckedForRenewal = true;
				}
				szText = GetDiploStringForMessage(eMessageType);
				CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
			}
		}

		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;
			int iDealType = -1;
			CvDeal* pRenewedDeal = m_pPlayer->GetDiplomacyAI()->GetDealToRenew(&iDealType);
			if(pRenewedDeal)
			{	
				if (iDealType != 0)
				{
					CvGameDeals::PrepareRenewDeal(m_pPlayer->GetDiplomacyAI()->GetDealToRenew(), &kDeal);
				}
				pRenewedDeal->m_bCheckedForRenewal = true;
				m_pPlayer->GetDiplomacyAI()->ClearDealToRenew();
			}


			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_NOW_UNFORGIVABLE)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_NOW_UNFORGIVABLE);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_NOW_ENEMY)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_NOW_ENEMY);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_CAUGHT_YOUR_SPY)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_CAUGHT_YOUR_SPY);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_CAUGHT_YOUR_SPY, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_KILLED_YOUR_SPY)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_KILLED_YOUR_SPY);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_KILLED_YOUR_SPY, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_KILLED_MY_SPY)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_KILLED_MY_SPY);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_KILLED_MY_SPY, szText, LEADERHEAD_ANIM_DEFEATED);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_SHARE_INTRIGUE)
	{
		IntrigueNotificationMessage* pNotificationMessage = GetPlayer()->GetEspionage()->GetRecentIntrigueInfo(ePlayer);
		CvAssertMsg(pNotificationMessage, "pNotificationMessage is null. Whut?");
		if (pNotificationMessage)
		{
			CvAssertMsg(pNotificationMessage->m_eSourcePlayer != NO_PLAYER, "There is no plotter! What's going on");
			PlayerTypes ePlotterPlayer = pNotificationMessage->m_eSourcePlayer;
			CvIntrigueType eIntrigueType = (CvIntrigueType)pNotificationMessage->m_iIntrigueType;

			if (!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(GET_PLAYER(ePlotterPlayer).getTeam()))
			{
				CvCity* pCity = NULL;
				if(pNotificationMessage->m_iCityX != -1 && pNotificationMessage->m_iCityY != -1)
				{
					CvPlot* pPlot = GC.getMap().plot(pNotificationMessage->m_iCityX, pNotificationMessage->m_iCityY);
					if(pPlot)
					{
						pCity = pPlot->getPlotCity();
					}
				}


				GET_PLAYER(ePlayer).GetEspionage()->AddIntrigueMessage(m_pPlayer->GetID(), ePlotterPlayer, ePlayer, NO_BUILDING, NO_PROJECT, eIntrigueType, 0, pCity, false);

				if(bShouldShowLeaderScene)
				{
					const char* szPlayerName;
					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(ePlotterPlayer).isHuman())
					{
						szPlayerName = GET_PLAYER(ePlotterPlayer).getNickName();
					}
					else
					{
						szPlayerName = GET_PLAYER(ePlotterPlayer).getNameKey();
					}

					szText = "";

					switch(eIntrigueType)
					{
					case INTRIGUE_TYPE_ARMY_SNEAK_ATTACK:
						if(pCity)
						{
							szText = GetDiploStringForMessage(DIPLO_MESSAGE_SHARE_INTRIGUE_ARMY_SNEAK_ATTACK_KNOWN_CITY, NO_PLAYER, szPlayerName, pCity->getNameKey());
						}
						else
						{
							szText = GetDiploStringForMessage(DIPLO_MESSAGE_SHARE_INTRIGUE_ARMY_SNEAK_ATTACK_UNKNOWN_CITY, NO_PLAYER, szPlayerName);
						}
						break;
					case INTRIGUE_TYPE_AMPHIBIOUS_SNEAK_ATTACK:
						if(pCity)
						{
							szText = GetDiploStringForMessage(DIPLO_MESSAGE_SHARE_INTRIGUE_AMPHIBIOUS_SNEAK_ATTACK_KNOWN_CITY, NO_PLAYER, szPlayerName, pCity->getNameKey());
						}
						else
						{
							szText = GetDiploStringForMessage(DIPLO_MESSAGE_SHARE_INTRIGUE_AMPHIBIOUS_SNEAK_ATTACK_UNKNOWN_CITY, NO_PLAYER, szPlayerName);
						}
						break;
					case INTRIGUE_TYPE_DECEPTION:
						szText = GetDiploStringForMessage(DIPLO_MESSAGE_SHARE_INTRIGUE, NO_PLAYER, szPlayerName);
						break;
					default:
						CvAssertMsg(false, "Unknown intrigue type");
						break;
					}

					CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
				}

			}


			for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)ui;
				GET_PLAYER(eLoopPlayer).GetEspionage()->MarkRecentIntrigueAsShared(ePlayer, ePlotterPlayer, eIntrigueType);
			}
		}
	}


	else if(eStatement == DIPLO_STATEMENT_STOP_CONVERSIONS)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_STOP_CONVERSIONS);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_STOP_CONVERSIONS, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_STOP_DIGGING)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_STOP_DIGGING);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_STOP_DIGGING, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_INSULT)
	{

		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetApproachTowardsUsGuess(GetPlayer()->GetID(), MAJOR_CIV_APPROACH_HOSTILE);
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetApproachTowardsUsGuessCounter(GetPlayer()->GetID(), 0);

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_INSULT_ROOT);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_COMPLIMENT)
	{

		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetApproachTowardsUsGuess(GetPlayer()->GetID(), MAJOR_CIV_APPROACH_FRIENDLY);
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetApproachTowardsUsGuessCounter(GetPlayer()->GetID(), 0);

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_COMPLIMENT);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_BOOT_KISSING)
	{

		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetApproachTowardsUsGuess(GetPlayer()->GetID(), MAJOR_CIV_APPROACH_AFRAID);
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetApproachTowardsUsGuessCounter(GetPlayer()->GetID(), 0);

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_BOOT_KISSING);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_WARMONGER)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_WARMONGER);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_MINOR_CIV_COMPETITION)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eMinorCiv = (PlayerTypes) iData1;
			const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();

			szText = GetDiploStringForMessage(DIPLO_MESSAGE_MINOR_CIV_COMPETITION, NO_PLAYER, strMinorCivKey);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_ANGRY_BEFRIEND_ENEMY)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DOFED_ENEMY, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_ANGRY_DENOUNCED_FRIEND)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DENOUNCED_FRIEND, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_HAPPY_DENOUNCED_ENEMY)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DENOUNCED_ENEMY, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_HAPPY_BEFRIENDED_FRIEND)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DOFED_FRIEND, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_ENEMY)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DENOUNCE_SO_AI_DOF, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_FRIEND)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DOF_SO_AI_DENOUNCE, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_ENEMY)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DENOUNCE_SO_AI_DENOUNCE, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_FRIEND)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DOF_SO_AI_DOF, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_SAME_POLICIES_FREEDOM)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_SAME_POLICIES_FREEDOM);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	else if(eStatement == DIPLO_STATEMENT_SAME_POLICIES_ORDER)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_SAME_POLICIES_ORDER);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	else if(eStatement == DIPLO_STATEMENT_SAME_POLICIES_AUTOCRACY)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_SAME_POLICIES_AUTOCRACY);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	else if(eStatement == DIPLO_STATEMENT_WE_LIKED_THEIR_PROPOSAL)
	{
		if(bShouldShowLeaderScene)
		{
			Localization::String sLeagueName = Localization::Lookup("TXT_KEY_LEAGUE_WORLD_CONGRESS_GENERIC");
			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague != NULL)
			{
				sLeagueName = pLeague->GetName();
			}
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_WE_LIKED_THEIR_PROPOSAL, ePlayer, sLeagueName);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	else if(eStatement == DIPLO_STATEMENT_WE_DISLIKED_THEIR_PROPOSAL)
	{
		if(bShouldShowLeaderScene)
		{
			Localization::String sLeagueName = Localization::Lookup("TXT_KEY_LEAGUE_WORLD_CONGRESS_GENERIC");
			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague != NULL)
			{
				sLeagueName = pLeague->GetName();
			}
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_WE_DISLIKED_THEIR_PROPOSAL, ePlayer, sLeagueName);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}

	else if(eStatement == DIPLO_STATEMENT_THEY_SUPPORTED_OUR_PROPOSAL)
	{
		if(bShouldShowLeaderScene)
		{
			Localization::String sLeagueName = Localization::Lookup("TXT_KEY_LEAGUE_WORLD_CONGRESS_GENERIC");
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_THEY_SUPPORTED_OUR_PROPOSAL, ePlayer, sLeagueName);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	else if(eStatement == DIPLO_STATEMENT_THEY_FOILED_OUR_PROPOSAL)
	{
		if(bShouldShowLeaderScene)
		{
			Localization::String sLeagueName = Localization::Lookup("TXT_KEY_LEAGUE_WORLD_CONGRESS_GENERIC");
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_THEY_FOILED_OUR_PROPOSAL, ePlayer, sLeagueName);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}

	else if(eStatement == DIPLO_STATEMENT_THEY_SUPPORTED_OUR_HOSTING)
	{
		if(bShouldShowLeaderScene)
		{
			Localization::String sLeagueName = Localization::Lookup("TXT_KEY_LEAGUE_WORLD_CONGRESS_GENERIC");
			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague != NULL)
			{
				sLeagueName = pLeague->GetName();
			}
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_THEY_SUPPORTED_OUR_HOSTING, ePlayer, sLeagueName);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_FREEDOM)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_FREEDOM);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}	
	else if(eStatement == DIPLO_STATEMENT_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_ORDER)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_ORDER);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}
	else if(eStatement == DIPLO_STATEMENT_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_AUTOCRACY)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_AUTOCRACY);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}
	else if(eStatement == DIPLO_STATEMENT_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_FREEDOM)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_FREEDOM);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}	
	else if(eStatement == DIPLO_STATEMENT_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_ORDER)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_ORDER);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}	
	else if(eStatement == DIPLO_STATEMENT_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_AUTOCRACY)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_AUTOCRACY);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}
	else if(eStatement == DIPLO_STATEMENT_SWITCH_OUR_IDEOLOGY_FREEDOM)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_SWITCH_OUR_IDEOLOGY_FREEDOM);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}
	else if(eStatement == DIPLO_STATEMENT_SWITCH_OUR_IDEOLOGY_ORDER)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_SWITCH_OUR_IDEOLOGY_ORDER);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}
	else if(eStatement == DIPLO_STATEMENT_SWITCH_OUR_IDEOLOGY_AUTOCRACY)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_SWITCH_OUR_IDEOLOGY_AUTOCRACY);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}
	else if(eStatement == DIPLO_STATEMENT_YOUR_CULTURE_INFLUENTIAL)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_YOUR_CULTURE_INFLUENTIAL);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}
	else if(eStatement == DIPLO_STATEMENT_OUR_CULTURE_INFLUENTIAL)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_OUR_CULTURE_INFLUENTIAL);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}


	else if(eStatement == DIPLO_STATEMENT_REQUEST_PEACE)
	{

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_PEACE_OFFER);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_POSITIVE);
		}

		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;


			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);

			LogPeaceMade(ePlayer);
		}
	}
}


void CvDiplomacyAI::DoMakePublicDeclaration(PublicDeclarationTypes eDeclaration, int iData1, int iData2, PlayerTypes eMustHaveMetPlayer, PlayerTypes eForSpecificPlayer)
{

	if(GetPlayer()->isHuman())
	{
		return;
	}

	CvAssertMsg(eDeclaration >= 0, "DIPLOMACY_AI: Invalid PublicDeclarationType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDeclaration < NUM_PUBLIC_DECLARATION_TYPES, "DIPLOMACY_AI: Invalid PublicDeclarationType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eForSpecificPlayer >= NO_PLAYER, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eForSpecificPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* strText = "";



	bool bActive = true;


	if(eDeclaration == PUBLIC_DECLARATION_PROTECT_MINOR)
	{
		PlayerTypes eMinorCiv = (PlayerTypes) iData1;
		const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();

		strText = GetDiploStringForMessage(DIPLO_MESSAGE_DECLARATION_PROTECT_CITY_STATE, NO_PLAYER, strMinorCivKey);


	}


	if(eDeclaration == PUBLIC_DECLARATION_ABANDON_MINOR)
	{

		DoMakeDeclarationInactive(PUBLIC_DECLARATION_PROTECT_MINOR, iData1, iData2);


		bActive = false;

		PlayerTypes eMinorCiv = (PlayerTypes) iData1;
		const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();

		strText = GetDiploStringForMessage(DIPLO_MESSAGE_DECLARATION_ABANDON_CITY_STATE, NO_PLAYER, strMinorCivKey);

	}








	if(eForSpecificPlayer != NO_PLAYER)
	{
	}


	if(eForSpecificPlayer == NO_PLAYER)
	{
		DoAddNewDeclarationToLog(eDeclaration, iData1, iData2, eMustHaveMetPlayer, bActive);


		for(int iCurPlayer = 0; iCurPlayer < MAX_MAJOR_CIVS; ++iCurPlayer){
			PlayerTypes eCurPlayer = (PlayerTypes) iCurPlayer;
			CvPlayerAI& kCurPlayer = GET_PLAYER(eCurPlayer);
			if(IsPlayerValid(eCurPlayer) 
				&& (eMustHaveMetPlayer == NO_PLAYER || GET_TEAM(kCurPlayer.getTeam()).isHasMet(GET_PLAYER(eMustHaveMetPlayer).getTeam()))){
				CvNotifications* pNotifications = GET_PLAYER(eCurPlayer).GetNotifications();
				if(pNotifications){
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_DECLARATION");
					strSummary << GetPlayer()->getCivilizationShortDescriptionKey();
					pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText, strSummary.toUTF8(), -1, -1, -1);
				}
			}
		}
	}
	else
	{

		CvPlayerAI& kSpecificPlayer = GET_PLAYER(eForSpecificPlayer);
		if(IsPlayerValid(eForSpecificPlayer) 
			&& (eMustHaveMetPlayer == NO_PLAYER || GET_TEAM(kSpecificPlayer.getTeam()).isHasMet(GET_PLAYER(eMustHaveMetPlayer).getTeam()))){	
			CvNotifications* pNotifications = kSpecificPlayer.GetNotifications();
			if(pNotifications){
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_DECLARATION");
				strSummary << GetPlayer()->getCivilizationShortDescriptionKey();
				pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText, strSummary.toUTF8(), -1, -1, -1);
			}
		}
	}

	LogPublicDeclaration(eDeclaration, iData1, eForSpecificPlayer);
}


void CvDiplomacyAI::DoContactMajorCivs()
{





	PlayerTypes eLoopPlayer;
	int iPlayerLoop;

	for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(!IsPlayerValid(eLoopPlayer))
			continue;


		if(GET_PLAYER(eLoopPlayer).isHuman())
			continue;

		DoContactPlayer(eLoopPlayer);
	}


	if(!CvPreGame::isNetworkMultiplayerGame())
	{
		for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(!IsPlayerValid(eLoopPlayer))
				continue;


			if(!GET_PLAYER(eLoopPlayer).isHuman())
				continue;

			DoContactPlayer(eLoopPlayer);
		}
	}
}


void CvDiplomacyAI::DoContactPlayer(PlayerTypes ePlayer)
{
	if(!IsValidUIDiplomacyTarget(ePlayer))
		return;

	int iDiploLogStatement;
	DiploStatementTypes eStatement;


	CvDeal* pDeal = GC.getGame().GetGameDeals()->GetTempDeal();


	int iData1;
	int iData2;


	if(GET_TEAM(GetTeam()).GetTurnsSinceMeetingTeam(GET_PLAYER(ePlayer).getTeam()) == 0)
		return;


	for(int iLoop = 0; iLoop < NUM_DIPLO_LOG_STATEMENT_TYPES; iLoop++)
	{
		m_paDiploLogStatementTurnCountScratchPad[iLoop] = MAX_TURNS_SAFE_ESTIMATE;
	}


	for(iDiploLogStatement = 0; iDiploLogStatement < MAX_DIPLO_LOG_STATEMENTS; iDiploLogStatement++)
	{
		eStatement = GetDiploLogStatementTypeForIndex(ePlayer, iDiploLogStatement);

		if(eStatement != NO_DIPLO_STATEMENT_TYPE)
		{
			CvAssert(eStatement < NUM_DIPLO_LOG_STATEMENT_TYPES);

			m_paDiploLogStatementTurnCountScratchPad[eStatement] = GetDiploLogStatementTurnForIndex(ePlayer, iDiploLogStatement);
		}
	}

	eStatement = NO_DIPLO_STATEMENT_TYPE;

	iData1 = -1;
	iData2 = -1;

	pDeal->ClearItems();
	pDeal->SetFromPlayer(GetPlayer()->GetID());
	pDeal->SetToPlayer(ePlayer);







	if(!IsAtWar(ePlayer))
	{
		DoCoopWarTimeStatement(ePlayer, eStatement, iData1);
		DoCoopWarStatement(ePlayer, eStatement, iData1);


		if(GetPlayer()->getTeam() != GET_PLAYER(ePlayer).getTeam())
		{
			DoMakeDemand(ePlayer, eStatement, pDeal);


			DoAggressiveMilitaryStatement(ePlayer, eStatement);
			DoKilledCityStateStatement(ePlayer, eStatement, iData1);
			DoAttackedCityStateStatement(ePlayer, eStatement, iData1);
			DoBulliedCityStateStatement(ePlayer, eStatement, iData1);

			DoExpansionWarningStatement(ePlayer, eStatement);
			DoExpansionBrokenPromiseStatement(ePlayer, eStatement);

			DoPlotBuyingWarningStatement(ePlayer, eStatement);
			DoPlotBuyingBrokenPromiseStatement(ePlayer, eStatement);

			DoWeAttackedYourMinorStatement(ePlayer, eStatement, iData1);
			DoWeBulliedYourMinorStatement(ePlayer, eStatement, iData1);

			DoKilledYourSpyStatement(ePlayer, eStatement);
			DoKilledMySpyStatement(ePlayer, eStatement);
			DoCaughtYourSpyStatement(ePlayer, eStatement);

			DoTheySupportedOurHosting(ePlayer, eStatement);
			DoWeLikedTheirProposal(ePlayer, eStatement);
			DoWeDislikedTheirProposal(ePlayer, eStatement);
			DoTheySupportedOurProposal(ePlayer, eStatement);
			DoTheyFoiledOurProposal(ePlayer, eStatement);

			DoConvertedMyCityStatement(ePlayer, eStatement);

			DoDugUpMyYardStatement(ePlayer, eStatement);

			DoDoFStatement(ePlayer, eStatement);
			DoDenounceFriendStatement(ePlayer, eStatement);
			DoDenounceStatement(ePlayer, eStatement);
			DoRequestFriendDenounceStatement(ePlayer, eStatement, iData1);


		}


		DoLuxuryTrade(ePlayer, eStatement, pDeal);
		DoEmbassyExchange(ePlayer, eStatement, pDeal);
		DoEmbassyOffer(ePlayer, eStatement, pDeal);
		DoOpenBordersExchange(ePlayer, eStatement, pDeal);
		DoOpenBordersOffer(ePlayer, eStatement, pDeal);
		DoResearchAgreementOffer(ePlayer, eStatement, pDeal);
		DoRenewExpiredDeal(ePlayer, eStatement, pDeal);
		DoShareIntrigueStatement(ePlayer, eStatement);


		DoRequest(ePlayer, eStatement, pDeal);


		if(GetPlayer()->getTeam() != GET_PLAYER(ePlayer).getTeam())
		{



			DoHostileStatement(ePlayer, eStatement);

			DoAfraidStatement(ePlayer, eStatement);
			DoWarmongerStatement(ePlayer, eStatement);
			DoMinorCivCompetitionStatement(ePlayer, eStatement, iData1);


			if(GET_PLAYER(ePlayer).isHuman())
			{
				DoAngryBefriendedEnemy(ePlayer, eStatement, iData1);
				DoAngryDenouncedFriend(ePlayer, eStatement, iData1);
				DoHappyDenouncedEnemy(ePlayer, eStatement, iData1);
				DoHappyBefriendedFriend(ePlayer, eStatement, iData1);
				DoFYIBefriendedHumanEnemy(ePlayer, eStatement, iData1);
				DoFYIDenouncedHumanFriend(ePlayer, eStatement, iData1);
				DoFYIDenouncedHumanEnemy(ePlayer, eStatement, iData1);
				DoFYIBefriendedHumanFriend(ePlayer, eStatement, iData1);
				DoHappySamePolicyTree(ePlayer, eStatement);
				DoIdeologicalStatement(ePlayer, eStatement);
			}
		}
	}


	else if(!GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
	{

		DoPeaceOffer(ePlayer, eStatement, pDeal);
	}

#if !defined(FINAL_RELEASE)

	if(eStatement == NO_DIPLO_STATEMENT_TYPE && m_eTestStatement != NO_DIPLO_STATEMENT_TYPE && ePlayer == m_eTestToPlayer)
	{
		eStatement = m_eTestStatement;
		iData1 = m_iTestStatementArg1;

		m_eTestStatement = NO_DIPLO_STATEMENT_TYPE;
	}
#endif


	if(eStatement != NO_DIPLO_STATEMENT_TYPE)
	{

		{
			LogStatementToPlayer(ePlayer, eStatement);

			DoSendStatementToPlayer(ePlayer, eStatement, iData1, pDeal);
			DoAddNewStatementToDiploLog(ePlayer, eStatement);
		}
	}
}


void CvDiplomacyAI::DoContactMinorCivs()
{

	int iDiplomacyFlavor =       GC.getDEFAULT_FLAVOR_VALUE();
	int iGoldFlavor =       GC.getDEFAULT_FLAVOR_VALUE();
	int iTileImprovementFlavor =       GC.getDEFAULT_FLAVOR_VALUE();
	int iExpansionFlavor =       GC.getDEFAULT_FLAVOR_VALUE();

	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_DIPLOMACY")
		{
			iDiplomacyFlavor = GetPlayer()->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_GOLD")
		{
			iGoldFlavor = GetPlayer()->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_TILE_IMPROVEMENT")
		{
			iTileImprovementFlavor = GetPlayer()->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_EXPANSION")
		{
			iExpansionFlavor = GetPlayer()->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
		}
	}
	EconomicAIStrategyTypes eFoundCity = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_FOUND_CITY");
	EconomicAIStrategyTypes eExpandLikeCrazy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_LIKE_CRAZY");
	EconomicAIStrategyTypes eExpandToOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
	EconomicAIStrategyTypes eNeedHappiness = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_HAPPINESS");
	EconomicAIStrategyTypes eNeedHappinessCritical = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_HAPPINESS_CRITICAL");
	EconomicAIStrategyTypes eLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
#ifdef AUI_WARNING_FIXES
	CvEconomicAI* pEconomicAI = GetPlayer()->GetEconomicAI();
	bool bFoundCity = (eFoundCity != NO_ECONOMICAISTRATEGY) ? pEconomicAI->IsUsingStrategy(eFoundCity) : false;
	bool bExpandLikeCrazy = (eExpandLikeCrazy != NO_ECONOMICAISTRATEGY) ? pEconomicAI->IsUsingStrategy(eExpandLikeCrazy) : false;
	bool bExpandToOtherContinents = (eExpandToOtherContinents != NO_ECONOMICAISTRATEGY) ? pEconomicAI->IsUsingStrategy(eExpandToOtherContinents) : false;
	bool bNeedHappiness = (eNeedHappiness != NO_ECONOMICAISTRATEGY) ? pEconomicAI->IsUsingStrategy(eNeedHappiness) : false;
	bool bNeedHappinessCritical = (eNeedHappinessCritical != NO_ECONOMICAISTRATEGY) ? pEconomicAI->IsUsingStrategy(eNeedHappinessCritical) : false;
	bool bLosingMoney = (eLosingMoney != NO_ECONOMICAISTRATEGY) ? pEconomicAI->IsUsingStrategy(eLosingMoney) : false;
#else
	bool bFoundCity = (eFoundCity != NO_ECONOMICAISTRATEGY) ? GetPlayer()->GetEconomicAI()->IsUsingStrategy(eFoundCity) : false;
	bool bExpandLikeCrazy = (eExpandLikeCrazy != NO_ECONOMICAISTRATEGY) ? GetPlayer()->GetEconomicAI()->IsUsingStrategy(eExpandLikeCrazy) : false;
	bool bExpandToOtherContinents = (eExpandToOtherContinents != NO_ECONOMICAISTRATEGY) ? GetPlayer()->GetEconomicAI()->IsUsingStrategy(eExpandToOtherContinents) : false;
	bool bNeedHappiness = (eNeedHappiness != NO_ECONOMICAISTRATEGY) ? GetPlayer()->GetEconomicAI()->IsUsingStrategy(eNeedHappiness) : false;
	bool bNeedHappinessCritical = (eNeedHappinessCritical != NO_ECONOMICAISTRATEGY) ? GetPlayer()->GetEconomicAI()->IsUsingStrategy(eNeedHappinessCritical) : false;
	bool bLosingMoney = (eLosingMoney != NO_ECONOMICAISTRATEGY) ? GetPlayer()->GetEconomicAI()->IsUsingStrategy(eLosingMoney) : false;
#endif




	bool bWantsToBuyout = false;
	if(GetPlayer()->IsAbleToAnnexCityStates())
	{
		if(bFoundCity || bExpandLikeCrazy || bExpandToOtherContinents ||
		        GetStateAllWars() == STATE_ALL_WARS_LOSING ||
		        IsGoingForWorldConquest() ||
		        m_pPlayer->calculateGoldRate() > 100)
		{
			bWantsToBuyout = true;
		}
		else
		{
			int iThreshold = iExpansionFlavor * 5;
			int iRandRoll = GC.getGame().getJonRandNum(100, "Diplomacy AI: good turn to buyout a minor?");

			if(iRandRoll < iThreshold)
				bWantsToBuyout = true;
		}
	}




	bool bWantsToMakeGoldGift = false;


	if(iDiplomacyFlavor >=       GC.getMC_ALWAYS_GIFT_DIPLO_THRESHOLD() ||
	        IsGoingForDiploVictory() ||
	        IsGoingForCultureVictory() ||
#ifdef AUI_WARNING_FIXES
			pEconomicAI->IsSavingForThisPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT) ||
#else
	        GetPlayer()->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT) ||
#endif
	        IsHasActiveGoldQuest() ||
	        m_pPlayer->calculateGoldRate() > 100)
	{
		bWantsToMakeGoldGift = true;
	}

	else
	{
		int iThreshold = iDiplomacyFlavor*       GC.getMC_SOMETIMES_GIFT_RAND_MULTIPLIER();
		int iRandRoll = GC.getGame().getJonRandNum(100, "Diplomacy AI: good turn to make a gold gift to a minor?");




		if(iRandRoll < iThreshold)
			bWantsToMakeGoldGift = true;
	}




	bool bWantsToBullyUnit = false;

#ifdef AUI_WARNING_FIXES
	if (pEconomicAI->GetWorkersToCitiesRatio() < 0.25 &&
		pEconomicAI->GetImprovedToImprovablePlotsRatio() < 0.50)
#else
	if(GetPlayer()->GetEconomicAI()->GetWorkersToCitiesRatio() < 0.25 &&
	        GetPlayer()->GetEconomicAI()->GetImprovedToImprovablePlotsRatio() < 0.50)
#endif
	{
		bWantsToBullyUnit = true;
	}

	else
	{
		int iThreshold = iTileImprovementFlavor * 3;
		int iRandRoll = GC.getGame().getJonRandNum(100, "Diplomacy AI: good turn to bully a unit (worker) from a minor?");

		if(iRandRoll < iThreshold)
			bWantsToBullyUnit = true;
	}





	bool bWantsToBullyGold = false;

	if(iGoldFlavor >= 6 ||
	        IsGoingForWorldConquest() ||
#ifdef AUI_WARNING_FIXES
		pEconomicAI->IsSavingForThisPurchase(PURCHASE_TYPE_UNIT) ||
		pEconomicAI->IsSavingForThisPurchase(PURCHASE_TYPE_BUILDING) ||
#else
	        GetPlayer()->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_UNIT) ||
	        GetPlayer()->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_BUILDING) ||
#endif
	        bLosingMoney ||
	        m_pPlayer->calculateGoldRate() < 0)
	{
		bWantsToBullyGold = true;
	}

	else
	{
		int iThreshold = iGoldFlavor * 4;
		int iRandRoll = GC.getGame().getJonRandNum(100, "Diplomacy AI: good turn to bully gold from a minor?");

		if(iRandRoll < iThreshold)
			bWantsToBullyGold = true;
	}

	if(BULLY_DEBUGGING)
	{



	}

	CvWeightedVector<PlayerTypes, MAX_PLAYERS, true> veMinorsToBuyout;
	CvWeightedVector<MinorGoldGiftInfo, MAX_PLAYERS, true> veMinorsToGiveGold;
	CvWeightedVector<PlayerTypes, MAX_PLAYERS, true> veMinorsToBullyGold;
	CvWeightedVector<PlayerTypes, MAX_PLAYERS, true> veMinorsToBullyUnit;

	int iLargeGift =          GC.getMINOR_GOLD_GIFT_LARGE();
	int iMediumGift =         GC.getMINOR_GOLD_GIFT_MEDIUM();
	int iSmallGift =         GC.getMINOR_GOLD_GIFT_SMALL();
	int iLargeGiftFriendship;
	int iMediumGiftFriendship;
	int iSmallGiftFriendship;
	bool bMediumGiftAllies;
	bool bSmallGiftAllies;

	PlayerTypes eID = GetPlayer()->GetID();

	CvMinorCivInfo* pMinorInfo;
	CvPlayer* pMinor;
	CvMinorCivAI* pMinorCivAI;

	bool bIntruding;

	int iOtherMajorLoop;
	PlayerTypes eOtherMajor;
	int iFriendshipWithMinor;
	int iOtherPlayerFriendshipWithMinor;

	bool bWantsToConnect;

	MinorCivApproachTypes eApproach;

	int iGrowthFlavor = GetPlayer()->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) GC.getInfoTypeForString("FLAVOR_GROWTH"));


	PlayerTypes eMinor;
	TeamTypes eMinorTeam;
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		eMinor = (PlayerTypes) iMinorLoop;

		pMinor = &GET_PLAYER(eMinor);
		pMinorCivAI = pMinor->GetMinorCivAI();
		eMinorTeam = pMinor->getTeam();

		bWantsToConnect = false;
		bool bWantsToGiveGoldToThisMinor = false;
		bool bWantsToBullyUnitFromThisMinor = false;
		bool bWantsToBullyGoldFromThisMinor = false;
		bool bWantsToBuyoutThisMinor = false;

		if(IsPlayerValid(eMinor))
		{

			if(IsAtWar(eMinor))
				continue;

			eApproach = GetMinorCivApproach(eMinor);


			DoUpdateMinorCivProtection(eMinor, eApproach);


			if(pMinorCivAI->IsActiveQuestForPlayer(eID, MINOR_CIV_QUEST_ROUTE))
			{
				if(eApproach == MINOR_CIV_APPROACH_PROTECTIVE ||
				        eApproach == MINOR_CIV_APPROACH_FRIENDLY)
				{
					if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_NEIGHBORS)
						bWantsToConnect = true;
				}
			}

			bIntruding = true;


			if(pMinorCivAI->IsPlayerHasOpenBorders(eID))
				bIntruding = false;

			else
			{

				if(eApproach == MINOR_CIV_APPROACH_PROTECTIVE || eApproach == MINOR_CIV_APPROACH_FRIENDLY)
					bIntruding = false;
			}

			pMinorCivAI->SetMajorIntruding(eID, bIntruding);


			if(bWantsToBuyout)
			{
				int iValue = 100;

				CvCity* pMinorCapital = pMinor->getCapitalCity();
				if(GetPlayer()->IsAbleToAnnexCityStates() && pMinorCivAI->CanMajorBuyout(eID) && pMinorCapital != NULL)
				{

					int iMinorArea = pMinorCapital->getArea();
					CvArea* pMinorArea = GC.getMap().getArea(iMinorArea);
					bool bPresenceInArea = false;
					int iMajorCapitalsInArea = 0;
					if(pMinorArea)
					{

						if(pMinorArea->getCitiesPerPlayer(eID) > 0)
							bPresenceInArea = true;


						for(int iMajorRivalLoop = 0; iMajorRivalLoop < MAX_MAJOR_CIVS; iMajorRivalLoop++)
						{
							PlayerTypes eMajorRivalLoop = (PlayerTypes) iMajorRivalLoop;
							if(eMajorRivalLoop == eID)
								continue;

							if(GET_PLAYER(eMajorRivalLoop).isAlive())
							{
								CvCity* pCapital = GET_PLAYER(eMajorRivalLoop).getCapitalCity();
								if(pCapital && pCapital->plot())
								{
									CvPlot* pPlot = pCapital->plot();
									if(pPlot->isVisible(GetPlayer()->getTeam()))
										iMajorCapitalsInArea++;
								}
							}
						}
					}
					else
					{
						CvAssertMsg(false, "Could not lookup minor civ's area! Please send Anton your save file and version.");
					}


					int iMinorMilitaryUnits = 0;
					int iMinorUnits = 0;
					CvUnit* pLoopUnit;
					int iLoop;
					for(pLoopUnit = pMinor->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pMinor->nextUnit(&iLoop))
					{
						if(pLoopUnit->IsCanAttack() && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
						{
							iMinorMilitaryUnits++;
						}
						iMinorUnits++;
					}


					if(!bPresenceInArea)
					{

						if(IsGoingForWorldConquest() && iMajorCapitalsInArea > 0)
						{
							iValue += 100;
						}

						else if(bExpandToOtherContinents)
						{
							iValue += 60;
						}
						else
						{
							iValue += -50;
						}
					}

					else
					{

						if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_NEIGHBORS)
						{
							iValue += 100;

							if(GetStateAllWars() == STATE_ALL_WARS_LOSING)
							{
								if(iMinorMilitaryUnits > 0)
								{
									iValue += (iMinorMilitaryUnits) * 10;
								}
								else
								{
									iValue -= 50;
								}
							}
						}
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_CLOSE)
						{
							iValue += 10;
						}
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_FAR)
						{
							iValue += -50;
						}
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_DISTANT)
						{
							iValue += -100;
						}
					}


					if(GetPlayer()->GetNumUnitsSupplied() >= GetPlayer()->getNumUnits() + iMinorUnits)
					{
						iValue += (iMinorMilitaryUnits) * 5;
					}


					if(bNeedHappiness)
						iValue += -50;
					if(bNeedHappinessCritical)
						iValue += -150;


					MinorCivTraitTypes eTrait = pMinorCivAI->GetTrait();
					if(eTrait == MINOR_CIV_TRAIT_CULTURED && IsGoingForCultureVictory())
					{
						iValue += -70;
					}
					else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
					{
						if(bNeedHappiness)
							iValue += -100;
						if(bNeedHappinessCritical)
							iValue += -150;
					}


					if(iValue > 100)
					{
						veMinorsToBuyout.push_back(eMinor, iValue);
						bWantsToBuyoutThisMinor = true;
					}
				}
			}


			if(bWantsToMakeGoldGift && !bWantsToBuyoutThisMinor)
			{
				int iValue =         GC.getMC_GIFT_WEIGHT_THRESHOLD();

				if(eApproach == MINOR_CIV_APPROACH_PROTECTIVE || eApproach == MINOR_CIV_APPROACH_FRIENDLY)
				{
					MinorGoldGiftInfo sGiftInfo;
					sGiftInfo.eMinor = eMinor;
					sGiftInfo.eMajorRival = NO_PLAYER;
					sGiftInfo.bQuickBoost = false;
					sGiftInfo.iGoldAmount = 0;


					iValue += min(max(0, m_pPlayer->calculateGoldRate() - 50),100);

					pMinorInfo = GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType());


					if(IsGoingForDiploVictory())
						iValue +=         GC.getMC_GIFT_WEIGHT_DIPLO_VICTORY();

					if(GC.getGame().IsUnitedNationsActive())
					{
						iValue +=         GC.getMC_GIFT_WEIGHT_DIPLO_VICTORY();
					}

					else if(IsGoingForCultureVictory())
					{
						if(pMinorInfo->GetMinorCivTrait() == MINOR_CIV_TRAIT_CULTURED)
							iValue +=         GC.getMC_GIFT_WEIGHT_CULTURE_VICTORY();
					}

					else if(IsGoingForWorldConquest())
					{
						if(pMinorInfo->GetMinorCivTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
							iValue +=         GC.getMC_GIFT_WEIGHT_CONQUEST_VICTORY();
					}



					if(pMinorCivAI->IsActiveQuestForPlayer(eID, MINOR_CIV_QUEST_GIVE_GOLD))
					{
						iValue += 150;
					}


					if(pMinorCivAI->IsActiveQuestForPlayer(eID, MINOR_CIV_QUEST_INVEST))
					{
						iValue += 100;
					}


					if(m_pPlayer->GetPlayerTraits()->GetCityStateFriendshipModifier() > 0 || m_pPlayer->GetPlayerTraits()->GetCityStateBonusModifier())
					{
						iValue +=         GC.getMC_GIFT_WEIGHT_DIPLO_VICTORY();
					}


					if(pMinorInfo->GetMinorCivTrait() == MINOR_CIV_TRAIT_MARITIME && !GetPlayer()->IsEmpireUnhappy())
					{
						iValue +=        GC.getMC_GIFT_WEIGHT_MARITIME_GROWTH() * iGrowthFlavor * max(1, GetPlayer()->getNumCities() / 3);
					}


					if(pMinorInfo->GetMinorCivTrait() == MINOR_CIV_TRAIT_MILITARISTIC && !IsGoingForWorldConquest())
						iValue +=         GC.getMC_GIFT_WEIGHT_MILITARISTIC();


					int iResourcesWeLack = pMinorCivAI->GetNumResourcesMajorLacks(eID);
					if(iResourcesWeLack > 0)
						iValue += (iResourcesWeLack*        GC.getMC_GIFT_WEIGHT_RESOURCE_WE_NEED());


					if(eApproach == MINOR_CIV_APPROACH_PROTECTIVE)
						iValue +=        GC.getMC_GIFT_WEIGHT_PROTECTIVE();


					if(pMinorCivAI->GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
						iValue +=         GC.getMC_GIFT_WEIGHT_HOSTILE();


					if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_NEIGHBORS)
						iValue +=       GC.getMC_GIFT_WEIGHT_NEIGHBORS();
					else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_CLOSE)
						iValue +=       GC.getMC_GIFT_WEIGHT_CLOSE();
					else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_FAR)
						iValue +=       GC.getMC_GIFT_WEIGHT_FAR();

					iLargeGiftFriendship = pMinorCivAI->GetFriendshipFromGoldGift(eID, iLargeGift);
					iMediumGiftFriendship = pMinorCivAI->GetFriendshipFromGoldGift(eID, iMediumGift);
					iSmallGiftFriendship = pMinorCivAI->GetFriendshipFromGoldGift(eID, iSmallGift);

					iFriendshipWithMinor = pMinorCivAI->GetEffectiveFriendshipWithMajor(eID);


					for(iOtherMajorLoop = 0; iOtherMajorLoop < MAX_MAJOR_CIVS; iOtherMajorLoop++)
					{
						eOtherMajor = (PlayerTypes) iOtherMajorLoop;


						if(!GET_PLAYER(eOtherMajor).isAlive())
							continue;

						iOtherPlayerFriendshipWithMinor = pMinorCivAI->GetEffectiveFriendshipWithMajor(eOtherMajor);


						if(iOtherPlayerFriendshipWithMinor <= 0)
							continue;


						if(iFriendshipWithMinor <= iOtherPlayerFriendshipWithMinor)
							continue;


						bMediumGiftAllies = iFriendshipWithMinor + iMediumGiftFriendship >= pMinorCivAI->GetAlliesThreshold();
						bSmallGiftAllies = iFriendshipWithMinor + iSmallGiftFriendship >= pMinorCivAI->GetAlliesThreshold();


						if(bSmallGiftAllies && iOtherPlayerFriendshipWithMinor - iFriendshipWithMinor < iSmallGiftFriendship)
						{
							iValue +=        GC.getMC_SMALL_GIFT_WEIGHT_PASS_OTHER_PLAYER();
							sGiftInfo.bQuickBoost = true;
							sGiftInfo.eMajorRival = eOtherMajor;
						}

						else if(bMediumGiftAllies && iOtherPlayerFriendshipWithMinor - iFriendshipWithMinor < iMediumGiftFriendship)
						{
							iValue +=        GC.getMC_GIFT_WEIGHT_PASS_OTHER_PLAYER();
							sGiftInfo.eMajorRival = eOtherMajor;
						}

						else
							iValue = 0;
					}


					if(pMinorCivAI->IsAllies(eID))
					{

						if(pMinorCivAI->IsCloseToNotBeingAllies(eID))
						{
							iValue +=         GC.getMC_GIFT_WEIGHT_ALMOST_NOT_ALLIES();
							sGiftInfo.bQuickBoost = true;
						}

						else
							iValue = 0;
					}

					else if(pMinorCivAI->IsFriends(eID))
					{

						if(pMinorCivAI->IsCloseToNotBeingFriends(eID))
						{
							iValue +=         GC.getMC_GIFT_WEIGHT_ALMOST_NOT_FRIENDS();
							sGiftInfo.bQuickBoost = true;
						}

						else if(!IsGoingForDiploVictory() || !GC.getGame().IsUnitedNationsActive())
							iValue = 0;
					}


					if(pMinorCivAI->IsRecentlyBulliedByMajor(eID))
					{
						iValue -= 100;
					}




#ifdef NQ_NUM_TURNS_BEFORE_MINOR_ALLIES_REFUSE_BRIBES_FROM_TRAIT
					bool bAcceptsBribes = true;
					PlayerTypes iAlly = pMinorCivAI->GetAlly();
					if (iAlly != NO_PLAYER && iAlly != eID)
					{
						int iNumTurns = GET_PLAYER(iAlly).GetNumTurnsBeforeMinorAlliesRefuseBribes();
						if (iNumTurns > 0 && pMinorCivAI->GetAlliedTurns() >= iNumTurns)
						{
							bAcceptsBribes = false;
						}
					}
					if(iValue > GC.getMC_GIFT_WEIGHT_THRESHOLD() && bAcceptsBribes)
#else
					if(iValue > GC.getMC_GIFT_WEIGHT_THRESHOLD())
#endif
					{
						veMinorsToGiveGold.push_back(sGiftInfo, iValue);
						bWantsToGiveGoldToThisMinor = true;
					}
				}
			}


			if(bWantsToBullyUnit && !bWantsToBuyoutThisMinor && !bWantsToGiveGoldToThisMinor)
			{
				int iValue = 100;
				if(eApproach == MINOR_CIV_APPROACH_BULLY)
				{

#ifdef AUI_WARNING_FIXES
					if (pMinorCivAI->CanMajorBullyUnit(eID))
#else
					if(pMinor->GetMinorCivAI()->CanMajorBullyUnit(eID))
#endif
					{

						if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_NEIGHBORS)
							iValue += 50;
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_CLOSE)
							iValue += 30;
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_FAR)
							iValue += -30;
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_DISTANT)
							iValue += -50;






#ifdef AUI_WARNING_FIXES
						if (pMinorCivAI->IsProtectedByAnyMajor())
#else
						if(pMinor->GetMinorCivAI()->IsProtectedByAnyMajor())
#endif
						{
							iValue += -20;

						}
						else
						{
							iValue += 20;
						}





#ifdef AUI_WARNING_FIXES
						if (pMinorCivAI->IsAllies(eID) || pMinorCivAI->IsFriends(eID))
#else
						if(pMinor->GetMinorCivAI()->IsAllies(eID) || pMinor->GetMinorCivAI()->IsFriends(eID))
#endif
						{
							iValue = 0;
						}


						if(iValue > 100)
						{
							veMinorsToBullyUnit.push_back(eMinor, iValue);
							bWantsToBullyUnitFromThisMinor = true;
						}
					}
				}
			}


			if(bWantsToBullyGold && !bWantsToBuyoutThisMinor && !bWantsToGiveGoldToThisMinor && !bWantsToBullyUnitFromThisMinor)
			{
				int iValue = 100;
				if(eApproach == MINOR_CIV_APPROACH_BULLY)
				{

#ifdef AUI_WARNING_FIXES
					if (pMinorCivAI->CanMajorBullyGold(eID))
#else
					if(pMinor->GetMinorCivAI()->CanMajorBullyGold(eID))
#endif
					{

						if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_NEIGHBORS)
							iValue += 40;
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_CLOSE)
							iValue += 20;


#ifdef AUI_WARNING_FIXES
						if (pMinorCivAI->IsEverBulliedByMajor(eID))
#else
						if(pMinor->GetMinorCivAI()->IsEverBulliedByMajor(eID))
#endif
						{
							iValue += 20;
						}


#ifdef AUI_WARNING_FIXES
						if (!pMinorCivAI->IsRecentlyBulliedByMajor(eID) && pMinorCivAI->IsRecentlyBulliedByAnyMajor())
#else
						if(!pMinor->GetMinorCivAI()->IsRecentlyBulliedByMajor(eID) && pMinor->GetMinorCivAI()->IsRecentlyBulliedByAnyMajor())
#endif
						{
							iValue += -10;

						}


#ifdef AUI_WARNING_FIXES
						if (pMinorCivAI->IsProtectedByAnyMajor())
#else
						if(pMinor->GetMinorCivAI()->IsProtectedByAnyMajor())
#endif
						{
							iValue += -10;

						}
						else
						{
							iValue += 10;
						}





#ifdef AUI_WARNING_FIXES
						if (pMinorCivAI->IsAllies(eID) || pMinorCivAI->IsFriends(eID))
#else
						if(pMinor->GetMinorCivAI()->IsAllies(eID) || pMinor->GetMinorCivAI()->IsFriends(eID))
#endif
						{
							iValue = 0;
						}


						if(iValue > 100)
						{
							veMinorsToBullyGold.push_back(eMinor, iValue);
							bWantsToBullyGoldFromThisMinor = true;
						}
					}
				}
			}

		}

		SetWantToRouteConnectToMinor(eMinor, bWantsToConnect);
	}

	int iGoldReserve = GetPlayer()->GetTreasury()->GetGold();


	if(veMinorsToBuyout.size() > 0)
	{
		veMinorsToBuyout.SortItems();
		int iGoldLeft = GetPlayer()->GetTreasury()->GetGold();
		PlayerTypes eLoopMinor = NO_PLAYER;
		for(int i = 0; i < veMinorsToBuyout.size(); i++)
		{
			eLoopMinor = veMinorsToBuyout.GetElement(i);
			int iBuyoutCost = GET_PLAYER(eLoopMinor).GetMinorCivAI()->GetBuyoutCost(eID);
			if(iGoldLeft >= iBuyoutCost)
			{
				if(GET_PLAYER(eLoopMinor).GetMinorCivAI()->CanMajorBuyout(eID))
				{
					GC.getGame().DoMinorBuyout(eID, eLoopMinor);
					iGoldLeft -= iBuyoutCost;
					break;
				}
				else
				{
					CvAssertMsg(false, "Chose a minor to buyout that cannot actually be bought! Please send Anton your save file and version.");
				}
			}
			else
			{
#ifdef AUI_WARNING_FIXES
				if (!pEconomicAI->IsSavingForThisPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT))
				{
					LogMinorCivBuyout(eLoopMinor, iBuyoutCost,             true);

					int iPriority = GC.getAI_GOLD_PRIORITY_BUYOUT_CITY_STATE();
					pEconomicAI->StartSaveForPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT, iBuyoutCost, iPriority);
#else
				if(!GetPlayer()->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT))
				{
					LogMinorCivBuyout(eLoopMinor, iBuyoutCost,             true);

					int iPriority = GC.getAI_GOLD_PRIORITY_BUYOUT_CITY_STATE();
					GetPlayer()->GetEconomicAI()->StartSaveForPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT, iBuyoutCost, iPriority);
#endif
				}
			}
		}
	}


	if(veMinorsToGiveGold.size() > 0)
	{
		veMinorsToGiveGold.SortItems();
		for(int i = 0; i < veMinorsToGiveGold.size(); i++)
		{
			int iGoldLeft = GetPlayer()->GetTreasury()->GetGold();
			MinorGoldGiftInfo sGift = veMinorsToGiveGold.GetElement(i);
			sGift.iGoldAmount = 0;

			if(iGoldLeft >= iSmallGift && sGift.bQuickBoost)
				sGift.iGoldAmount = iSmallGift;
			else if(iGoldLeft >= iLargeGift)
				sGift.iGoldAmount = iLargeGift;
			else if(iGoldLeft >= iMediumGift)
				sGift.iGoldAmount = iMediumGift;

			int iOldFriendship = GET_PLAYER(sGift.eMinor).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(eID);


			if(sGift.iGoldAmount > 0 && iGoldLeft >= (iGoldReserve / 2))
			{
				GET_PLAYER(sGift.eMinor).GetMinorCivAI()->DoGoldGiftFromMajor(GetPlayer()->GetID(), sGift.iGoldAmount);

				LogMinorCivGiftGold(sGift.eMinor, iOldFriendship, sGift.iGoldAmount,             false, sGift.bQuickBoost, sGift.eMajorRival);

				if(GetPlayer()->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT))
					GetPlayer()->GetEconomicAI()->CancelSaveForPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT);
			}

			else
			{
				if(!GetPlayer()->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT))
				{
					int iAmountToSaveFor = iMediumGift;

					if(sGift.bQuickBoost)
						iAmountToSaveFor = iSmallGift;

					LogMinorCivGiftGold(sGift.eMinor, iOldFriendship, iAmountToSaveFor,             true, sGift.bQuickBoost, sGift.eMajorRival);

					int iPriority = GC.getAI_GOLD_PRIORITY_DIPLOMACY_BASE();
					iPriority += GC.getAI_GOLD_PRIORITY_DIPLOMACY_PER_FLAVOR_POINT() * iDiplomacyFlavor;
					GetPlayer()->GetEconomicAI()->StartSaveForPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT, iAmountToSaveFor, iPriority);
				}
			}
		}
	}


	if(veMinorsToBullyUnit.size() > 0)
	{
		veMinorsToBullyUnit.SortItems();
		PlayerTypes eLoopMinor = NO_PLAYER;
		for(int i = 0; i < veMinorsToBullyUnit.size(); i++)
		{
			eLoopMinor = veMinorsToBullyUnit.GetElement(i);
			CvAssertMsg(eLoopMinor != NO_PLAYER, "Trying to bully a unit from NO_PLAYER! Please send Anton your save file and version.");
			if(GET_PLAYER(eLoopMinor).GetMinorCivAI()->CanMajorBullyUnit(eID))
			{
				GC.getGame().DoMinorBullyUnit(eID, eLoopMinor);
				break;
			}
			else
			{
				CvAssertMsg(false, "Chose a minor to bully unit from that cannot actually be bullied! Please send Anton your save file and version.");
			}
		}
	}


	if(veMinorsToBullyGold.size() > 0)
	{
		veMinorsToBullyGold.SortItems();
		PlayerTypes eLoopMinor = NO_PLAYER;
		for(int i = 0; i < veMinorsToBullyGold.size(); i++)
		{
			eLoopMinor = veMinorsToBullyGold.GetElement(i);
			CvAssertMsg(eLoopMinor != NO_PLAYER, "Trying to bully gold from NO_PLAYER! Please send Anton your save file and version.");
			if(GET_PLAYER(eLoopMinor).GetMinorCivAI()->CanMajorBullyGold(eID))
			{
				GC.getGame().DoMinorBullyGold(eID, eLoopMinor);
			}
			else
			{
				CvAssertMsg(false, "Chose a minor to bully gold from that cannot actually be bullied! Please send Anton your save file and version.");
			}
		}
	}
}

void CvDiplomacyAI::DoUpdateMinorCivProtection(PlayerTypes eMinor, MinorCivApproachTypes eApproach)
{

	if(!GetPlayer()->isHuman())
	{
		if(eApproach == MINOR_CIV_APPROACH_PROTECTIVE)
		{

			if(GET_PLAYER(eMinor).GetMinorCivAI()->CanMajorStartProtection(GetPlayer()->GetID()))
			{
				GC.getGame().DoMinorPledgeProtection(GetPlayer()->GetID(), eMinor, true);
				DoMakePublicDeclaration(PUBLIC_DECLARATION_PROTECT_MINOR, eMinor, -1, eMinor);
			}
		}
		else
		{

			if(GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()) && GET_PLAYER(eMinor).GetMinorCivAI()->CanMajorWithdrawProtection(GetPlayer()->GetID()))
			{
				GC.getGame().DoMinorPledgeProtection(GetPlayer()->GetID(), eMinor, false);
				DoMakePublicDeclaration(PUBLIC_DECLARATION_ABANDON_MINOR, eMinor, -1, eMinor);
			}
		}
	}
}


void CvDiplomacyAI::DoCoopWarTimeStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{

		if(!GET_PLAYER(ePlayer).isHuman())
			return;

		CvTeam* pTeam = &GET_TEAM(GET_PLAYER(ePlayer).getTeam());

		PlayerTypes eTargetPlayer;
		TeamTypes eTargetTeam;

		for(int iTargetLoop = 0; iTargetLoop < MAX_MAJOR_CIVS; iTargetLoop++)
		{
			eTargetPlayer = (PlayerTypes) iTargetLoop;


			if(GetCoopWarAcceptedState(ePlayer, eTargetPlayer) == COOP_WAR_STATE_SOON)
			{
				if(GetCoopWarCounter(ePlayer, eTargetPlayer) ==        GC.getCOOP_WAR_SOON_COUNTER())
				{
					eTargetTeam = GET_PLAYER(eTargetPlayer).getTeam();


					if(!pTeam->isAtWar(eTargetTeam) && GET_PLAYER(eTargetPlayer).isAlive())
					{
						eStatement = DIPLO_STATEMENT_COOP_WAR_TIME;
						iData1 = eTargetPlayer;


						break;
					}

					else
					{
						SetCoopWarAcceptedState(ePlayer, eTargetPlayer, COOP_WAR_STATE_ACCEPTED);


						if(!IsAtWar(eTargetPlayer) && GET_PLAYER(eTargetPlayer).isAlive())
						{
							DeclareWar(eTargetPlayer);
							GetPlayer()->GetMilitaryAI()->RequestBasicAttack(eTargetPlayer, 1);
						}
					}
				}
			}
		}
	}
}


void CvDiplomacyAI::DoCoopWarStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{

		if (GET_TEAM(GetTeam()).getAtWarCount(true) == 0)
		{
			PlayerTypes eTargetPlayer;
			if(DoTestCoopWarDesire(ePlayer,                       eTargetPlayer))
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_COOP_WAR_REQUEST;
				int iTurnsBetweenStatements = 10;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					bool bSendStatement = true;





					if(bSendStatement)
					{
						eStatement = eTempStatement;
						iData1 = eTargetPlayer;
					}


					else
						DoAddNewStatementToDiploLog(ePlayer, eTempStatement);
				}
			}	
		}
	}
}


void CvDiplomacyAI::DoMakeDemand(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetDemandTargetPlayer() == ePlayer)
		{
			if(IsDemandReady())
			{
				if(GetPlayer()->GetDealAI()->IsMakeDemand(ePlayer,                                            pDeal))
				{
					DiploStatementTypes eTempStatement = DIPLO_STATEMENT_DEMAND;
					int iTurnsBetweenStatements = 40;

					if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
						eStatement = eTempStatement;
				}
				else
				{

					pDeal->ClearItems();
				}
			}
		}
	}
}


void CvDiplomacyAI::DoAggressiveMilitaryStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		bool bSendStatement = false;


		if(!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canDeclareWar(GetPlayer()->getTeam()))
			return;


		if (WasResurrectedBy(ePlayer))
		{
			return;
		}


		if(GetMilitaryAggressivePosture(ePlayer) >= AGGRESSIVE_POSTURE_HIGH && GetLastTurnMilitaryAggressivePosture(ePlayer) < AGGRESSIVE_POSTURE_HIGH)
			bSendStatement = true;


		else if(GetMilitaryAggressivePosture(ePlayer) >= AGGRESSIVE_POSTURE_MEDIUM && GetLastTurnMilitaryAggressivePosture(ePlayer) <= AGGRESSIVE_POSTURE_NONE)
			bSendStatement = true;


		if(IsDoFAccepted(ePlayer))
			return;


		PlayerTypes eThirdParty;
		for(int iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
		{
			eThirdParty = (PlayerTypes) iThirdPartyLoop;


			if(IsAtWar(eThirdParty) && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(GET_PLAYER(eThirdParty).getTeam()))
				return;


			if(GetPlayer()->GetProximityToPlayer(eThirdParty) == PLAYER_PROXIMITY_NEIGHBORS && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(GET_PLAYER(eThirdParty).getTeam()))
				return;
		}

		if(bSendStatement)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_AGGRESSIVE_MILITARY_WARNING;
			int iTurnsBetweenStatements = 40;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
	}
}


void CvDiplomacyAI::DoKilledCityStateStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(IsPlayerMadeAttackCityStatePromise(ePlayer))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_KILLED_PROTECTED_CITY_STATE;
			int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
			{
				PlayerTypes eMinorCiv = GetOtherPlayerProtectedMinorKilled(ePlayer);
				if(eMinorCiv != NO_PLAYER)
				{
					CvAssert(eMinorCiv >= MAX_MAJOR_CIVS);
					CvAssert(eMinorCiv < MAX_CIV_PLAYERS);

					eStatement = eTempStatement;

					iData1 = eMinorCiv;
				}
			}
		}
	}
}


void CvDiplomacyAI::DoAttackedCityStateStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetTurnsSincePlayerAttackedProtectedMinor(ePlayer) == 0)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_ATTACKED_PROTECTED_CITY_STATE;
			int iTurnsBetweenStatements = 1;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
			{
				PlayerTypes eMinorCiv = GetOtherPlayerProtectedMinorAttacked(ePlayer);
				if(eMinorCiv != NO_PLAYER)
				{
					CvAssert(eMinorCiv >= MAX_MAJOR_CIVS);
					CvAssert(eMinorCiv < MAX_CIV_PLAYERS);

					if(GET_PLAYER(eMinorCiv).isAlive())
					{	
						eStatement = eTempStatement;
						iData1 = eMinorCiv;
					}
				}
			}
		}
	}
}


void CvDiplomacyAI::DoBulliedCityStateStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{

		if(GetTurnsSincePlayerBulliedProtectedMinor(ePlayer) == 0)
		{

			if(IsPlayerBrokenBullyCityStatePromise(ePlayer) ||
			        IsPlayerIgnoredBullyCityStatePromise(ePlayer))
			{

			}
			else if(IsPlayerMadeBullyCityStatePromise(ePlayer))
			{

				SetPlayerBrokenBullyCityStatePromise(ePlayer, true);
				SetPlayerMadeBullyCityStatePromise(ePlayer, false);
			}

			else
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_BULLIED_PROTECTED_CITY_STATE;
				int iTurnsBetweenStatements = 1;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					PlayerTypes eMinorCiv = GetOtherPlayerProtectedMinorBullied(ePlayer);
					if(eMinorCiv != NO_PLAYER)
					{
						CvAssert(eMinorCiv >= MAX_MAJOR_CIVS);
						CvAssert(eMinorCiv < MAX_CIV_PLAYERS);

						if(GET_PLAYER(eMinorCiv).isAlive())
						{
							eStatement = eTempStatement;
							iData1 = eMinorCiv;
						}
					}
				}
			}
		}
	}
}


void CvDiplomacyAI::DoExpansionWarningStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		bool bSendStatement = false;
		if (GET_PLAYER(ePlayer).GetTurnsSinceSettledLastCity() < GC.getEXPANSION_BICKER_TIMEOUT() && !EverMadeExpansionPromise(ePlayer) && !IsPlayerMadeExpansionPromise(ePlayer) && !IsPlayerIgnoredExpansionPromise(ePlayer) && !IsPlayerBrokenExpansionPromise(ePlayer))
		{

			if(GetLandDisputeLevel(ePlayer) >= DISPUTE_LEVEL_FIERCE)
				bSendStatement = true;


			else if(GetLandDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG && GetLastTurnLandDisputeLevel(ePlayer) < DISPUTE_LEVEL_STRONG)
			{
				if(GetExpansionAggressivePosture(ePlayer) >= AGGRESSIVE_POSTURE_MEDIUM)
					bSendStatement = true;
			}
		}

		if(bSendStatement)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_EXPANSION_WARNING;
			int iTurnsBetweenStatements = (GC.getEXPANSION_PROMISE_TURNS_EFFECTIVE() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent()) / 100;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
	}
}


void CvDiplomacyAI::DoExpansionBrokenPromiseStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(IsPlayerBrokenExpansionPromise(ePlayer))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_EXPANSION_BROKEN_PROMISE;
			int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
	}
}


void CvDiplomacyAI::DoPlotBuyingWarningStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		bool bSendStatement = false;
		if (!EverMadeBorderPromise(ePlayer) && !IsPlayerMadeBorderPromise(ePlayer) && !IsPlayerBrokenBorderPromise(ePlayer) && !IsPlayerIgnoredBorderPromise(ePlayer))
		{
			if(GetLandDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
			{

				if(GetPlotBuyingAggressivePosture(ePlayer) >= AGGRESSIVE_POSTURE_LOW)
					bSendStatement = true;
			}
		}

		if(bSendStatement)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_PLOT_BUYING_WARNING;
			int iTurnsBetweenStatements = (GC.getBORDER_PROMISE_TURNS_EFFECTIVE() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent()) / 100;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}

	}
}


void CvDiplomacyAI::DoPlotBuyingBrokenPromiseStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(IsPlayerBrokenBorderPromise(ePlayer))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_PLOT_BUYING_BROKEN_PROMISE;
			int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
	}
}


void CvDiplomacyAI::DoWeAttackedYourMinorStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes) iMinorCivLoop;
			if(GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(ePlayer))
			{
				if(IsAtWar(eMinor))
				{

					if(GetOtherPlayerWarValueLost(eMinor, GetPlayer()->GetID()) > GetOtherPlayerLastRoundWarValueLost(eMinor, GetPlayer()->GetID()))
					{

						if(!HasSentAttackProtectedMinorTaunt(ePlayer, eMinor))
						{
							DiploStatementTypes eTempStatement = DIPLO_STATEMENT_WE_ATTACKED_YOUR_MINOR;
							int iTurnsBetweenStatements = 1;
							if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
							{
								CvAssert(eMinor >= MAX_MAJOR_CIVS);
								CvAssert(eMinor < MAX_CIV_PLAYERS);

								eStatement = eTempStatement;
								iData1 = eMinor;
							}
						}
					}
				}
			}
		}
	}
}


void CvDiplomacyAI::DoWeBulliedYourMinorStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		PlayerTypes eMinor;
		for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			eMinor = (PlayerTypes) iMinorCivLoop;


			if(!GET_PLAYER(eMinor).isAlive())
				continue;


			if(GET_PLAYER(eMinor).GetMinorCivAI()->IsEverBulliedByMajor(GetPlayer()->GetID()))
			{
				if(GET_PLAYER(eMinor).GetMinorCivAI()->GetTurnLastBulliedByMajor(GetPlayer()->GetID()) == (GC.getGame().getGameTurn() - 1))
				{

					if(GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(ePlayer))
					{
						DiploStatementTypes eTempStatement = DIPLO_STATEMENT_WE_BULLIED_YOUR_MINOR;
						int iTurnsBetweenStatements = 1;

						if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
						{
							CvAssert(eMinor >= MAX_MAJOR_CIVS);
							CvAssert(eMinor < MAX_CIV_PLAYERS);

							eStatement = eTempStatement;
							iData1 = eMinor;
						}
					}
				}
			}
		}
	}
}

void CvDiplomacyAI::DoCaughtYourSpyStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(m_pPlayer->GetEspionageAI()->m_aiTurnLastSpyCaught[ePlayer] == GC.getGame().getGameTurn())
		{

			if(IsPlayerBrokenSpyPromise(ePlayer) || IsPlayerIgnoredSpyPromise(ePlayer))
			{

			}
			else if(IsPlayerMadeSpyPromise(ePlayer))
			{

				SetPlayerBrokenSpyPromise(ePlayer, true);
				SetPlayerMadeSpyPromise(ePlayer, false);
			}

			else
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_CAUGHT_YOUR_SPY;
				int iTurnsBetweenStatements = 1;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					eStatement = eTempStatement;
				}
			}
		}
	}
}

void CvDiplomacyAI::DoKilledYourSpyStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(m_pPlayer->GetEspionageAI()->m_aiNumSpiesKilled[ePlayer] > 0)
		{
			if(m_pPlayer->GetEspionageAI()->m_aiTurnLastSpyKilled[ePlayer] == GC.getGame().getGameTurn())
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_KILLED_YOUR_SPY;
				int iTurnsBetweenStatements = 1;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					eStatement = eTempStatement;
				}
			}
		}
	}
}

void CvDiplomacyAI::DoKilledMySpyStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(m_pPlayer->GetEspionageAI()->m_aiTurnLastSpyDied[ePlayer] == GC.getGame().getGameTurn() - 1)
		{

			if(IsPlayerBrokenSpyPromise(ePlayer) || IsPlayerIgnoredSpyPromise(ePlayer))
			{

			}
			else if(IsPlayerMadeSpyPromise(ePlayer))
			{

				SetPlayerBrokenSpyPromise(ePlayer, true);
				SetPlayerMadeSpyPromise(ePlayer, false);
			}

			else
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_KILLED_MY_SPY;
				int iTurnsBetweenStatements = 1;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					eStatement = eTempStatement;
				}
			}
		}
	}
}

void CvDiplomacyAI::DoShareIntrigueStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		IntrigueNotificationMessage* pNotificationMessage = m_pPlayer->GetEspionage()->GetRecentIntrigueInfo(ePlayer);
		if (pNotificationMessage)
		{

			if(pNotificationMessage->m_eSourcePlayer != NO_PLAYER)
			{
				bool bIsNewIntrigue = true;

				for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
				{
					PlayerTypes eOtherPlayer = (PlayerTypes)ui;

					if(eOtherPlayer == pNotificationMessage->m_eSourcePlayer)
					{
						continue;
					}

					if(GET_PLAYER(eOtherPlayer).GetEspionage()->HasSharedIntrigue(ePlayer, pNotificationMessage->m_eSourcePlayer, (CvIntrigueType)(pNotificationMessage->m_iIntrigueType)))
					{
						bIsNewIntrigue = false;
						break;
					}
				}

				if(bIsNewIntrigue)
				{
					MajorCivApproachTypes eApproachType = m_pPlayer->GetDiplomacyAI()->GetMajorCivApproach(ePlayer, false);
					if (eApproachType != MAJOR_CIV_APPROACH_GUARDED && eApproachType != MAJOR_CIV_APPROACH_HOSTILE && eApproachType != MAJOR_CIV_APPROACH_WAR)
					{
						DiploStatementTypes eTempStatement = DIPLO_STATEMENT_SHARE_INTRIGUE;
						int iTurnsBetweenStatements = 1;
						if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) > iTurnsBetweenStatements)
						{
							eStatement = eTempStatement;
						}
					}
				}
				else
				{

					m_pPlayer->GetEspionage()->MarkRecentIntrigueAsShared(ePlayer, pNotificationMessage->m_eSourcePlayer, (CvIntrigueType)(pNotificationMessage->m_iIntrigueType));
				}
			}
		}
	}
}

void CvDiplomacyAI::DoConvertedMyCityStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetNegativeReligiousConversionPoints(ePlayer) >= GC.getRELIGION_DIPLO_HIT_THRESHOLD())
		{

			if(IsPlayerBrokenNoConvertPromise(ePlayer) ||
			        IsPlayerIgnoredNoConvertPromise(ePlayer))
			{

			}
			else if(IsPlayerMadeNoConvertPromise(ePlayer))
			{

				SetPlayerBrokenNoConvertPromise(ePlayer, true);
				SetPlayerMadeNoConvertPromise(ePlayer, false);
			}

			else
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_STOP_CONVERSIONS;
				int iTurnsBetweenStatements = 1;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					eStatement = eTempStatement;
				}
			}
		}
	}
}


void CvDiplomacyAI::DoDugUpMyYardStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetNegativeArchaeologyPoints(ePlayer) >= 10)
		{

			if(IsPlayerBrokenNoDiggingPromise(ePlayer) ||
				IsPlayerIgnoredNoDiggingPromise(ePlayer))
			{

			}
			else if(IsPlayerMadeNoDiggingPromise(ePlayer))
			{

				SetPlayerBrokenNoDiggingPromise(ePlayer, true);
				SetPlayerMadeNoDiggingPromise(ePlayer, false);
			}

			else
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_STOP_DIGGING;
				int iTurnsBetweenStatements = 1;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					eStatement = eTempStatement;
				}
			}
		}
	}
}


void CvDiplomacyAI::DoDoFStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{

		if(!IsDoFAccepted(ePlayer))
		{
			if(IsDoFAcceptable(ePlayer))
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_WORK_WITH_US;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 60 &&
				        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_WORK_WITH_US_RANDFAILED) >= 10)
				{
					bool bSendStatement = true;





					if(bSendStatement)
						eStatement = eTempStatement;


					else
						DoAddNewStatementToDiploLog(ePlayer, DIPLO_STATEMENT_WORK_WITH_US_RANDFAILED);
				}
			}
		}
	}
}


void CvDiplomacyAI::DoDenounceFriendStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{

		if(IsDoFAccepted(ePlayer))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_DENOUNCE_FRIEND;
			int iTurnsBetweenStatements = 1;


			if(IsDenounceFriendAcceptable(ePlayer))
			{
				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					eStatement = eTempStatement;
			}




		}
	}
}


void CvDiplomacyAI::DoDenounceStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(IsDenounceAcceptable(ePlayer,           false))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_DENOUNCE;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 60 &&
			        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_DENOUNCE_RANDFAILED) >= 10)
			{
				bool bSendStatement = true;





				if(bSendStatement)
				{
					eStatement = eTempStatement;
				}


				else
					DoAddNewStatementToDiploLog(ePlayer, DIPLO_STATEMENT_DENOUNCE_RANDFAILED);
			}
		}
	}
}


void CvDiplomacyAI::DoRequestFriendDenounceStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		bool bRandFailed;

		PlayerTypes eTarget = GetRequestFriendToDenounce(ePlayer, bRandFailed);
		if(eTarget != NO_PLAYER)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_REQUEST_FRIEND_DENOUNCE;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
			        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_REQUEST_FRIEND_DENOUNCE_RANDFAILED) >= 10)
			{
				if(!bRandFailed)
				{
					eStatement = eTempStatement;
					iData1 = eTarget;
				}

				else
				{
					DoAddNewStatementToDiploLog(ePlayer, DIPLO_STATEMENT_DENOUNCE_RANDFAILED);
				}
			}
		}
	}
}



































































void CvDiplomacyAI::DoLuxuryTrade(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetPlayer()->GetDealAI()->IsMakeOfferForLuxuryResource(ePlayer,                                            pDeal))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_LUXURY_TRADE;
			int iTurnsBetweenStatements = 20;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
		else
		{

			pDeal->ClearItems();
		}
	}
}


void CvDiplomacyAI::DoEmbassyExchange(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{

		if(pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), ePlayer, TRADE_ITEM_ALLOW_EMBASSY) &&
		        pDeal->IsPossibleToTradeItem(ePlayer, GetPlayer()->GetID(), TRADE_ITEM_ALLOW_EMBASSY))
		{

			if(IsEmbassyExchangeAcceptable(ePlayer))
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_EMBASSY_EXCHANGE;
				int iTurnsBetweenStatements = 20;

				if ((GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements) && GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_EMBASSY_OFFER) >= 10)
				{
					bool bSendStatement = false;


					if(!GET_PLAYER(ePlayer).isHuman())
					{
						if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsEmbassyExchangeAcceptable(GetPlayer()->GetID()))
							bSendStatement = true;
					}

					else
						bSendStatement = true;


					if(50 < GC.getGame().getJonRandNum(100, "Diplomacy AI: rand rool to see if we ask to exchange embassies"))
						bSendStatement = false;

					if(bSendStatement)
					{
						pDeal->AddAllowEmbassy(GetPlayer()->GetID());
						pDeal->AddAllowEmbassy(ePlayer);

						eStatement = eTempStatement;
					}
					else
						DoAddNewStatementToDiploLog(ePlayer, eTempStatement);
				}
			}
		}
	}
}


void CvDiplomacyAI::DoEmbassyOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetPlayer()->GetDealAI()->MakeOfferForEmbassy(ePlayer,                                            pDeal))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_EMBASSY_OFFER;
			int iTurnsBetweenStatements = 20;

			if ((GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements) && GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_EMBASSY_EXCHANGE) >= 10)
				eStatement = eTempStatement;
		}
		else
		{

			pDeal->ClearItems();
		}
	}
}


void CvDiplomacyAI::DoOpenBordersExchange(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		int iDuration = GC.getGame().GetDealDuration();


		if(pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), ePlayer, TRADE_ITEM_OPEN_BORDERS, iDuration) &&
		        pDeal->IsPossibleToTradeItem(ePlayer, GetPlayer()->GetID(), TRADE_ITEM_OPEN_BORDERS, iDuration))
		{

			if(IsOpenBordersExchangeAcceptable(ePlayer))
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_OPEN_BORDERS_EXCHANGE;
				int iTurnsBetweenStatements = 20;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					bool bSendStatement = false;


					if(!GET_PLAYER(ePlayer).isHuman())
					{
						if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsOpenBordersExchangeAcceptable(GetPlayer()->GetID()))
							bSendStatement = true;
					}

					else
						bSendStatement = true;


					if(50 < GC.getGame().getJonRandNum(100, "Diplomacy AI: rand roll to see if we ask to exchange open borders"))
						bSendStatement = false;

					if(bSendStatement)
					{

						pDeal->AddOpenBorders(GetPlayer()->GetID(), iDuration);
						pDeal->AddOpenBorders(ePlayer, iDuration);

						eStatement = eTempStatement;
					}

					else
						DoAddNewStatementToDiploLog(ePlayer, eTempStatement);
				}
			}
		}
	}
}


void CvDiplomacyAI::DoOpenBordersOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetPlayer()->GetDealAI()->IsMakeOfferForOpenBorders(ePlayer,                                            pDeal))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_OPEN_BORDERS_OFFER;
			int iTurnsBetweenStatements = 20;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
		else
		{

			pDeal->ClearItems();
		}
	}
}


void CvDiplomacyAI::DoResearchAgreementOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(IsCanMakeResearchAgreementRightNow(ePlayer))
		{
			if(GetPlayer()->GetDealAI()->IsMakeOfferForResearchAgreement(ePlayer,                                            pDeal))
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_RESEARCH_AGREEMENT_OFFER;
				int iTurnsBetweenStatements = 20;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					eStatement = eTempStatement;
			}
			else
			{

				pDeal->ClearItems();
			}
		}
	}
}


void CvDiplomacyAI::DoRenewExpiredDeal(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if (GetGlobalCoopWarAcceptedState(ePlayer) >= COOP_WAR_STATE_SOON)
		{
			return;
		}

		CvDeal* pTargetDeal = NULL;
		CvGameDeals* pGameDeals = GC.getGame().GetGameDeals();

		for(int iDealTypes = 0; iDealTypes < 2; iDealTypes++)
		{
			int iNumDeals = 0;
			if(iDealTypes == 0)
			{
				iNumDeals = pGameDeals->GetNumHistoricDeals(ePlayer);
			}
			else
			{
				iNumDeals = pGameDeals->GetNumCurrentDeals(ePlayer);
			}

			for(int iDeal = 0; iDeal < iNumDeals; iDeal++)
			{
				CvDeal* pCurrentDeal = NULL;
				if(iDealTypes == 0)
				{
					pCurrentDeal = pGameDeals->GetHistoricDeal(ePlayer, iDeal);
				}
				else
				{
					pCurrentDeal = pGameDeals->GetCurrentDeal(ePlayer, iDeal);
				}


				if(pCurrentDeal->m_bConsideringForRenewal || pCurrentDeal->m_bDealCancelled || pCurrentDeal->m_bCheckedForRenewal)
				{
					continue;
				}


				if(!(pCurrentDeal->m_eFromPlayer == m_pPlayer->GetID() || pCurrentDeal->m_eToPlayer == m_pPlayer->GetID()))
				{
					continue;
				}


				if(!pCurrentDeal->IsPotentiallyRenewable())
				{
					continue;
				}


				if(pCurrentDeal->m_iFinalTurn > GC.getGame().getElapsedGameTurns())
				{
					continue;
				}

				bool bCanTradeItems = true;


				TradedItemList::iterator it;
				CvDeal kTempDeal;

				if (iDealTypes == 1) 
				{
					pCurrentDeal->m_bConsideringForRenewal = true;
				}
				for(it = pCurrentDeal->m_TradedItems.begin(); it != pCurrentDeal->m_TradedItems.end(); ++it)
				{
					PlayerTypes eOtherPlayer;
					if (it->m_eFromPlayer == pCurrentDeal->m_eFromPlayer)
					{
						eOtherPlayer = pCurrentDeal->m_eToPlayer;
					}
					else
					{
						eOtherPlayer = pCurrentDeal->m_eFromPlayer;
					}

					if(!kTempDeal.IsPossibleToTradeItem(it->m_eFromPlayer, eOtherPlayer, it->m_eItemType, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1))
					{
						bCanTradeItems = false;
						break;
					}
				}
				if (iDealTypes == 1)
				{
					pCurrentDeal->m_bConsideringForRenewal = false;
				}

				if(!bCanTradeItems)
				{
					continue;
				}

				pTargetDeal = pCurrentDeal;
				break;
			}

			if (pTargetDeal)
			{
				break;
			}
		}

		if (pTargetDeal)
		{

			*pDeal = *pTargetDeal;
			pTargetDeal->m_bConsideringForRenewal = true;
			pDeal->m_iFinalTurn = -1;

			if(!GET_PLAYER(ePlayer).isHuman())
			{
				bool bAbleToEqualize = m_pPlayer->GetDealAI()->DoEqualizeDealWithAI(pDeal, ePlayer);
				if(!bAbleToEqualize)
				{
					pDeal->ClearItems();
					ClearDealToRenew();
					return;
				}
			}

			eStatement = DIPLO_STATEMENT_RENEW_DEAL;
		}
		else
		{
			pDeal->ClearItems();
		}
	}
}
































void CvDiplomacyAI::DoRequest(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_REQUEST;


		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 60 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_REQUEST_RANDFAILED) >= 15)
		{
			bool bRandPassed;
			bool bMakeRequest = IsMakeRequest(ePlayer, pDeal, bRandPassed);


			if(bMakeRequest)
			{
				eStatement = eTempStatement;
				pDeal->SetRequestingPlayer(GetPlayer()->GetID());
			}


			else
				pDeal->ClearItems();


			if(!bRandPassed)
				DoAddNewStatementToDiploLog(ePlayer, DIPLO_STATEMENT_REQUEST_RANDFAILED);
		}
	}
}


void CvDiplomacyAI::DoGift(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_GIFT;


		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 60 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_GIFT_RANDFAILED) >= 15)
		{
			bool bRandPassed = false;
			bool bMakeGift = false;


			if(bMakeGift)
			{
				eStatement = eTempStatement;
				pDeal->SetRequestingPlayer(GetPlayer()->GetID());
			}


			else
				pDeal->ClearItems();


			if(!bRandPassed)
				DoAddNewStatementToDiploLog(ePlayer, DIPLO_STATEMENT_GIFT_RANDFAILED);
		}
	}
}








































































void CvDiplomacyAI::DoHostileStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
		{
			TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();


			int iPeaceTreatyTurn = GET_TEAM(GetTeam()).GetTurnMadePeaceTreatyWithTeam(eTeam);
			if(iPeaceTreatyTurn != -1)
			{
				int iTurnsSincePeace = GC.getGame().getElapsedGameTurns() - iPeaceTreatyTurn;
				if(iTurnsSincePeace <        GC.getTURNS_SINCE_PEACE_WEIGHT_DAMPENER())
					return;
			}

			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_INSULT;
			int iTurnsBetweenStatements = 35;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
	}
}

























void CvDiplomacyAI::DoAfraidStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_BOOT_KISSING;
			int iTurnsBetweenStatements = 35;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
	}
}


void CvDiplomacyAI::DoWarmongerStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetWarmongerThreat(ePlayer) >= THREAT_SEVERE)
		{
			bool bSendStatement = true;


			if(IsGoingForWorldConquest())
				bSendStatement = false;


			if(33 < GC.getGame().getJonRandNum(100, "Diplomacy AI: rand roll to see if we warn a player about being a warmonger"))
				bSendStatement = false;

			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_WARMONGER;
			int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;

			if(bSendStatement)
			{
				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					eStatement = eTempStatement;
			}

			else
				DoAddNewStatementToDiploLog(ePlayer, eTempStatement);
		}
	}
}


void CvDiplomacyAI::DoMinorCivCompetitionStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1, bool bIgnoreTurnsBetweenLimit)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetMinorCivDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_MINOR_CIV_COMPETITION;
			int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements || bIgnoreTurnsBetweenLimit)
			{

				PlayerTypes eMinor;
				for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
				{
					eMinor = (PlayerTypes) iMinorLoop;


					if(GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
					{
						if(GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(ePlayer))
						{
							iData1 = eMinor;
							break;
						}
						else if(GET_PLAYER(eMinor).GetMinorCivAI()->IsFriends(ePlayer))
						{
							iData1 = eMinor;
							break;
						}
					}
				}


				if(iData1 != NO_PLAYER)
				{
					eStatement = eTempStatement;
				}
			}
		}
	}
}


void CvDiplomacyAI::DoAngryBefriendedEnemy(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(IsDenouncedPlayer(ePlayer) || IsAtWar(ePlayer) || GetMajorCivApproach(ePlayer,                       true) == MAJOR_CIV_APPROACH_HOSTILE)
		return;

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_ANGRY_BEFRIEND_ENEMY;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_ANGRY_BEFRIEND_ENEMY_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();


			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;


				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;


				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;


				if(!IsDenouncedPlayer(eLoopPlayer))
					continue;


				if(!pTheirDiploAI->IsDoFAccepted(eLoopPlayer))
					continue;


				if(pTheirDiploAI->GetDoFCounter(eLoopPlayer) > 1)
					continue;



				int iWeight = GetMeanness();
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");


				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}

				else
				{
					eStatement = DIPLO_STATEMENT_ANGRY_BEFRIEND_ENEMY_RANDFAILED;
				}


				break;
			}
		}
	}
}

void CvDiplomacyAI::DoAngryDenouncedFriend(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(IsDenouncedPlayer(ePlayer) || IsAtWar(ePlayer) || GetMajorCivApproach(ePlayer,                       true) == MAJOR_CIV_APPROACH_HOSTILE)
		return;

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_ANGRY_DENOUNCED_FRIEND;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_ANGRY_DENOUNCED_FRIEND_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();


			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;


				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;


				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;


				if(!IsDoFAccepted(eLoopPlayer))
					continue;


				if(!pTheirDiploAI->IsDenouncedPlayer(eLoopPlayer))
					continue;


				if(pTheirDiploAI->GetDenouncedPlayerCounter(eLoopPlayer) > 1)
					continue;



				int iWeight = GetMeanness();
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");


				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}

				else
				{
					eStatement = DIPLO_STATEMENT_ANGRY_DENOUNCED_FRIEND_RANDFAILED;
				}


				break;
			}
		}
	}
}

void CvDiplomacyAI::DoHappyDenouncedEnemy(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(IsDenouncedPlayer(ePlayer) || IsAtWar(ePlayer) || GetMajorCivApproach(ePlayer,                       true) == MAJOR_CIV_APPROACH_HOSTILE)
		return;

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_HAPPY_DENOUNCED_ENEMY;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_HAPPY_DENOUNCED_ENEMY_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();


			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;


				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;


				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;


				if(!IsDenouncedPlayer(eLoopPlayer))
					continue;


				if(!pTheirDiploAI->IsDenouncedPlayer(eLoopPlayer))
					continue;


				if(pTheirDiploAI->GetDenouncedPlayerCounter(eLoopPlayer) > 1)
					continue;



				int iWeight = GetChattiness();
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");


				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}

				else
				{
					eStatement = DIPLO_STATEMENT_HAPPY_DENOUNCED_ENEMY_RANDFAILED;
				}


				break;
			}
		}
	}
}

void CvDiplomacyAI::DoHappyBefriendedFriend(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(IsDenouncedPlayer(ePlayer) || IsAtWar(ePlayer) || GetMajorCivApproach(ePlayer,                       true) == MAJOR_CIV_APPROACH_HOSTILE)
		return;

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_HAPPY_BEFRIENDED_FRIEND;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_HAPPY_BEFRIENDED_FRIEND_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();


			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;


				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;


				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;


				if(!IsDoFAccepted(eLoopPlayer))
					continue;


				if(!pTheirDiploAI->IsDoFAccepted(eLoopPlayer))
					continue;


				if(pTheirDiploAI->GetDoFCounter(eLoopPlayer) > 1)
					continue;



				int iWeight = GetChattiness();
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");


				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}

				else
				{
					eStatement = DIPLO_STATEMENT_HAPPY_BEFRIENDED_FRIEND_RANDFAILED;
				}


				break;
			}
		}
	}
}


void CvDiplomacyAI::DoPeaceOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		CvTeam* pOurTeam = &GET_TEAM(GetPlayer()->getTeam());
		TeamTypes eTheirTeam = GET_PLAYER(ePlayer).getTeam();


		if(GetPlayerNumTurnsAtWar(ePlayer) > 5)
		{

			if(pOurTeam->GetNumTurnsLockedIntoWar(eTheirTeam) == 0 &&
			        GET_TEAM(eTheirTeam).GetNumTurnsLockedIntoWar(GetPlayer()->getTeam()) == 0)
			{
				if(IsWantsPeaceWithPlayer(ePlayer))
				{
					DiploStatementTypes eTempStatement = DIPLO_STATEMENT_REQUEST_PEACE;
					int iTurnsBetweenStatements = 10;

					if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					{
						if(GetPlayer()->GetDealAI()->IsOfferPeace(ePlayer,                                            pDeal, false                     ))
						{
							eStatement = eTempStatement;
						}
						else
						{

							pDeal->ClearItems();
						}
					}
				}
			}
		}
	}
}


void CvDiplomacyAI::DoFYIBefriendedHumanEnemy(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_ENEMY;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_ENEMY_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();

			int iWeight;
			MajorCivApproachTypes eApproach;
			MajorCivOpinionTypes eOpinion;


			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;


				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;


				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;


				if(!IsDoFAccepted(eLoopPlayer))
					continue;


				if(!pTheirDiploAI->IsDenouncedPlayer(eLoopPlayer))
					continue;


				if(GetDoFCounter(eLoopPlayer) > 1)
					continue;

				eOpinion = GetMajorCivOpinion(ePlayer);
				eApproach = GetMajorCivApproach(ePlayer,                       true);


				if(eOpinion >= MAJOR_CIV_OPINION_FAVORABLE)
					continue;
				if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
					continue;

				iWeight = 0;

				if(eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
					iWeight += 2;
				else if(eOpinion == MAJOR_CIV_OPINION_ENEMY)
					iWeight += 5;
				else if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
					iWeight += 10;

				iWeight += GetMeanness();
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");


				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}

				else
				{
					eStatement = DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_ENEMY_RANDFAILED;
				}


				break;
			}
		}
	}
}


void CvDiplomacyAI::DoFYIDenouncedHumanFriend(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_FRIEND;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_FRIEND_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();

			int iWeight;
			MajorCivApproachTypes eApproach;
			MajorCivOpinionTypes eOpinion;


			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;


				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;


				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;


				if(!IsDenouncedPlayer(eLoopPlayer))
					continue;


				if(!pTheirDiploAI->IsDoFAccepted(eLoopPlayer))
					continue;


				if(GetDenouncedPlayerCounter(eLoopPlayer) > 1)
					continue;

				eOpinion = GetMajorCivOpinion(ePlayer);
				eApproach = GetMajorCivApproach(ePlayer,                       true);


				if(eOpinion >= MAJOR_CIV_OPINION_FAVORABLE)
					continue;
				if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
					continue;

				iWeight = 0;

				if(eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
					iWeight += 2;
				else if(eOpinion == MAJOR_CIV_OPINION_ENEMY)
					iWeight += 5;
				else if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
					iWeight += 10;

				iWeight += GetMeanness();
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");


				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}

				else
				{
					eStatement = DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_FRIEND_RANDFAILED;
				}


				break;
			}
		}
	}
}


void CvDiplomacyAI::DoFYIDenouncedHumanEnemy(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_ENEMY;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_ENEMY_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();

			int iWeight;
			MajorCivApproachTypes eApproach;
			MajorCivOpinionTypes eOpinion;


			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;


				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;


				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;


				if(!IsDenouncedPlayer(eLoopPlayer))
					continue;


				if(!pTheirDiploAI->IsDenouncedPlayer(eLoopPlayer))
					continue;


				if(GetDenouncedPlayerCounter(eLoopPlayer) > 1)
					continue;

				eOpinion = GetMajorCivOpinion(ePlayer);
				eApproach = GetMajorCivApproach(ePlayer,                       true);


				if(eOpinion <= MAJOR_CIV_OPINION_COMPETITOR)
					continue;
				if(eApproach == MAJOR_CIV_APPROACH_WAR || eApproach == MAJOR_CIV_APPROACH_HOSTILE)
					continue;

				iWeight = 0;

				if(eOpinion == MAJOR_CIV_OPINION_FAVORABLE)
					iWeight += 2;
				else if(eOpinion == MAJOR_CIV_OPINION_FRIEND)
					iWeight += 5;
				else if(eOpinion == MAJOR_CIV_OPINION_ALLY)
					iWeight += 10;

#ifdef AUI_WARNING_FIXES
				if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
#else
				if(eOpinion == MAJOR_CIV_APPROACH_FRIENDLY)
#endif
					iWeight += 2;


				if(GetPlayerMilitaryStrengthComparedToUs(ePlayer) > STRENGTH_AVERAGE)
					iWeight += 3;

				iWeight += GetChattiness();
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");


				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}

				else
				{
					eStatement = DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_ENEMY_RANDFAILED;
				}


				break;
			}
		}
	}
}


void CvDiplomacyAI::DoFYIBefriendedHumanFriend(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_FRIEND;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_FRIEND_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();

			int iWeight;
			MajorCivApproachTypes eApproach;
			MajorCivOpinionTypes eOpinion;


			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;


				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;


				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;


				if(!IsDoFAccepted(eLoopPlayer))
					continue;


				if(!pTheirDiploAI->IsDoFAccepted(eLoopPlayer))
					continue;


				if(GetDoFCounter(eLoopPlayer) > 1)
					continue;

				eOpinion = GetMajorCivOpinion(ePlayer);
				eApproach = GetMajorCivApproach(ePlayer,                       true);


				if(eOpinion <= MAJOR_CIV_OPINION_COMPETITOR)
					continue;
				if(eApproach == MAJOR_CIV_APPROACH_WAR || eApproach == MAJOR_CIV_APPROACH_HOSTILE)
					continue;

				iWeight = 0;

				if(eOpinion == MAJOR_CIV_OPINION_FAVORABLE)
					iWeight += 2;
				else if(eOpinion == MAJOR_CIV_OPINION_FRIEND)
					iWeight += 5;
				else if(eOpinion == MAJOR_CIV_OPINION_ALLY)
					iWeight += 10;

#ifdef AUI_WARNING_FIXES
				if (eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
#else
				if(eOpinion == MAJOR_CIV_APPROACH_FRIENDLY)
#endif
					iWeight += 2;

				iWeight += GetChattiness();
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");


				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}

				else
				{
					eStatement = DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_FRIEND_RANDFAILED;
				}


				break;
			}
		}
	}
}

void CvDiplomacyAI::DoHappySamePolicyTree(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = NO_DIPLO_STATEMENT_TYPE;

		PolicyBranchTypes eMyBranch = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
		PolicyBranchTypes eTheirBranch = GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
		if(eMyBranch != NO_POLICY_BRANCH_TYPE && eTheirBranch != NO_POLICY_BRANCH_TYPE && eMyBranch == eTheirBranch && GET_PLAYER(ePlayer).GetCulture()->GetTurnIdeologySwitch() < 0)
		{

			bool bSkip = false;
			MajorCivApproachTypes eApproach;
			MajorCivOpinionTypes eOpinion;
			eOpinion = GetMajorCivOpinion(ePlayer);
			eApproach = GetMajorCivApproach(ePlayer,                       true);
			if(eOpinion <= MAJOR_CIV_OPINION_COMPETITOR)
				bSkip = true;
			if(eApproach == MAJOR_CIV_APPROACH_WAR || eApproach == MAJOR_CIV_APPROACH_HOSTILE)
				bSkip = true;


			if(!bSkip && GetChattiness() > GC.getGame().getJonRandNum(10, "Diplomacy AI: rand roll to see if we congratulate a player on the same policy tree choice"))
			{
				DiploStatementTypes eOtherStatementToCheck = NO_DIPLO_STATEMENT_TYPE;

				if(eMyBranch == GC.getPOLICY_BRANCH_FREEDOM())
				{
					eTempStatement = DIPLO_STATEMENT_SAME_POLICIES_FREEDOM;
					eOtherStatementToCheck = DIPLO_STATEMENT_SWITCH_OUR_IDEOLOGY_FREEDOM;
				}
				else if(eMyBranch == GC.getPOLICY_BRANCH_ORDER())
				{
					eTempStatement = DIPLO_STATEMENT_SAME_POLICIES_ORDER;
					eOtherStatementToCheck = DIPLO_STATEMENT_SWITCH_OUR_IDEOLOGY_ORDER;
				}
				else if(eMyBranch == GC.getPOLICY_BRANCH_AUTOCRACY())
				{
					eTempStatement = DIPLO_STATEMENT_SAME_POLICIES_AUTOCRACY;
					eOtherStatementToCheck = DIPLO_STATEMENT_SWITCH_OUR_IDEOLOGY_AUTOCRACY;
				}

				if(eTempStatement != NO_DIPLO_STATEMENT_TYPE)
				{
					int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;

					if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					{

						if(GetNumTurnsSinceStatementSent(ePlayer, eOtherStatementToCheck) >= iTurnsBetweenStatements)
						{
							eStatement = eTempStatement;
						}
					}
				}
			}
		}
	}
}

void CvDiplomacyAI::DoIdeologicalStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	PolicyBranchTypes eFreedom = (PolicyBranchTypes)GC.getPOLICY_BRANCH_FREEDOM();
	PolicyBranchTypes eOrder = (PolicyBranchTypes)GC.getPOLICY_BRANCH_ORDER();
	int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;

	CvPlayer &kTheirPlayer = GET_PLAYER(ePlayer);
#ifdef AUI_WARNING_FIXES
	CvPlayerCulture& kTheirCulture = *kTheirPlayer.GetCulture();
#endif

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement;

		if (m_pPlayer->GetCulture()->GetInfluenceLevel(ePlayer) >= INFLUENCE_LEVEL_INFLUENTIAL)
		{
			eTempStatement = DIPLO_STATEMENT_OUR_CULTURE_INFLUENTIAL;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
			{
				eStatement = eTempStatement;
				return;
			}
		}

#ifdef AUI_WARNING_FIXES
		if (kTheirCulture.GetInfluenceLevel(m_pPlayer->GetID()) >= INFLUENCE_LEVEL_INFLUENTIAL)
#else
		if (kTheirPlayer.GetCulture()->GetInfluenceLevel(m_pPlayer->GetID()) >= INFLUENCE_LEVEL_INFLUENTIAL)
#endif
		{
			eTempStatement = DIPLO_STATEMENT_YOUR_CULTURE_INFLUENTIAL;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
			{
				eStatement = eTempStatement;
				return;
			}
		}

		PolicyBranchTypes eMyBranch = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
		PolicyBranchTypes eTheirBranch = kTheirPlayer.GetPlayerPolicies()->GetLateGamePolicyTree();

		if(eMyBranch != NO_POLICY_BRANCH_TYPE && eTheirBranch != NO_POLICY_BRANCH_TYPE && eMyBranch == eTheirBranch)
		{
			PublicOpinionTypes eOpinionInMyCiv = m_pPlayer->GetCulture()->GetPublicOpinionType();
			PlayerTypes eMyGreatestInfluence = m_pPlayer->GetCulture()->GetPublicOpinionBiggestInfluence();
#ifdef AUI_WARNING_FIXES
			PublicOpinionTypes eOpinionInTheirCiv = kTheirCulture.GetPublicOpinionType();
			PlayerTypes eTheirGreatestInfluence = kTheirCulture.GetPublicOpinionBiggestInfluence();


			int iIdeologySwitchTurn = kTheirCulture.GetTurnIdeologySwitch();
#else
			PublicOpinionTypes eOpinionInTheirCiv = kTheirPlayer.GetCulture()->GetPublicOpinionType();
			PlayerTypes eTheirGreatestInfluence = kTheirPlayer.GetCulture()->GetPublicOpinionBiggestInfluence();


			int iIdeologySwitchTurn = kTheirPlayer.GetCulture()->GetTurnIdeologySwitch();
#endif
			if (iIdeologySwitchTurn > 0 && iIdeologySwitchTurn + 10 > GC.getGame().getGameTurn())
			{
#ifdef AUI_WARNING_FIXES
				kTheirCulture.SetTurnIdeologySwitch(-1);
#else
				kTheirPlayer.GetCulture()->SetTurnIdeologySwitch(-1);
#endif
				if (eTheirBranch == eFreedom)
				{
					eStatement = DIPLO_STATEMENT_SWITCH_OUR_IDEOLOGY_FREEDOM;
				}
				else if (eTheirBranch == eOrder)
				{
					eStatement = DIPLO_STATEMENT_SWITCH_OUR_IDEOLOGY_ORDER;
				}
				else
				{
					eStatement = DIPLO_STATEMENT_SWITCH_OUR_IDEOLOGY_AUTOCRACY;
				}
				return;
			}

			if (eOpinionInMyCiv >= PUBLIC_OPINION_CIVIL_RESISTANCE && eMyGreatestInfluence == ePlayer)
			{
				if (eTheirBranch == eFreedom)
				{
					eTempStatement = DIPLO_STATEMENT_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_FREEDOM;
				}
				else if (eTheirBranch == eOrder)
				{
					eTempStatement = DIPLO_STATEMENT_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_ORDER;
				}
				else
				{
					eTempStatement = DIPLO_STATEMENT_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_AUTOCRACY;
				}
				if (eTempStatement != NO_DIPLO_STATEMENT_TYPE)
				{
					if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					{
						eStatement = eTempStatement;
						return;
					}
				}
			}
			if (eOpinionInTheirCiv >= PUBLIC_OPINION_CIVIL_RESISTANCE && eTheirGreatestInfluence == m_pPlayer->GetID())
			{
				if (eMyBranch == eFreedom)
				{
					eTempStatement = DIPLO_STATEMENT_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_FREEDOM;
				}
				else if (eMyBranch == eOrder)
				{
					eTempStatement = DIPLO_STATEMENT_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_ORDER;
				}
				else
				{
					eTempStatement = DIPLO_STATEMENT_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_AUTOCRACY;
				}
				if (eTempStatement != NO_DIPLO_STATEMENT_TYPE)
				{
					if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					{
						eStatement = eTempStatement;
						return;
					}
				}
			}
		}
	}
}

void CvDiplomacyAI::DoWeLikedTheirProposal(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = NO_DIPLO_STATEMENT_TYPE;
		bool bEffect = GetLikedTheirProposalScore(ePlayer) != 0;
		if (bEffect)
		{
			bool bSkip = false;
			MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);
			MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);
			if(eOpinion <= MAJOR_CIV_OPINION_COMPETITOR)
				bSkip = true;
			if(eApproach == MAJOR_CIV_APPROACH_WAR || eApproach == MAJOR_CIV_APPROACH_HOSTILE)
				bSkip = true;
			if (!bSkip && GetChattiness() > GC.getGame().getJonRandNum(10, "Diplomacy AI: rand roll to see if we tell a player we liked their World Congress proposal"))
			{
				eTempStatement = DIPLO_STATEMENT_WE_LIKED_THEIR_PROPOSAL;
				int iTurnsBetweenStatements = GC.getOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_NUM_TURNS();

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					eStatement = eTempStatement;
			}
		}
	}
}

void CvDiplomacyAI::DoWeDislikedTheirProposal(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = NO_DIPLO_STATEMENT_TYPE;
		bool bEffect = GetDislikedTheirProposalScore(ePlayer) != 0;
		if (bEffect)
		{
			bool bSkip = false;
			MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);
			MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);
			if(eOpinion >= MAJOR_CIV_OPINION_FAVORABLE)
				bSkip = true;
			if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
				bSkip = true;
			if (!bSkip && GetChattiness() > GC.getGame().getJonRandNum(10, "Diplomacy AI: rand roll to see if we tell a player we disliked their World Congress proposal"))
			{
				eTempStatement = DIPLO_STATEMENT_WE_DISLIKED_THEIR_PROPOSAL;
				int iTurnsBetweenStatements = GC.getOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_NUM_TURNS();

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					eStatement = eTempStatement;
			}
		}
	}
}

void CvDiplomacyAI::DoTheySupportedOurProposal(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = NO_DIPLO_STATEMENT_TYPE;
		bool bEffect = GetSupportedMyProposalScore(ePlayer) != 0;
		if (bEffect)
		{
			bool bSkip = false;
			MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);
			MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);
			if(eOpinion <= MAJOR_CIV_OPINION_COMPETITOR)
				bSkip = true;
			if(eApproach == MAJOR_CIV_APPROACH_WAR || eApproach == MAJOR_CIV_APPROACH_HOSTILE)
				bSkip = true;
			if (!bSkip)
			{
				eTempStatement = DIPLO_STATEMENT_THEY_SUPPORTED_OUR_PROPOSAL;
				int iTurnsBetweenStatements = GC.getOPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_NUM_TURNS();

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					eStatement = eTempStatement;
			}
		}
	}
}

void CvDiplomacyAI::DoTheyFoiledOurProposal(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = NO_DIPLO_STATEMENT_TYPE;
		bool bEffect = GetFoiledMyProposalScore(ePlayer) != 0;
		if (bEffect)
		{
			bool bSkip = false;
			MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);
			MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);
			if(eOpinion >= MAJOR_CIV_OPINION_FAVORABLE)
				bSkip = true;
			if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
				bSkip = true;
			if (!bSkip)
			{
				eTempStatement = DIPLO_STATEMENT_THEY_FOILED_OUR_PROPOSAL;
				int iTurnsBetweenStatements = GC.getOPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_NUM_TURNS();

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					eStatement = eTempStatement;
			}
		}
	}
}

void CvDiplomacyAI::DoTheySupportedOurHosting(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = NO_DIPLO_STATEMENT_TYPE;
		bool bEffect = GetSupportedMyHostingScore(ePlayer) != 0;
		if (bEffect)
		{
			bool bSkip = false;
			MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);
			MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);
			if(eOpinion <= MAJOR_CIV_OPINION_COMPETITOR)
				bSkip = true;
			if(eApproach == MAJOR_CIV_APPROACH_WAR || eApproach == MAJOR_CIV_APPROACH_HOSTILE)
				bSkip = true;
			if (!bSkip)
			{
				eTempStatement = DIPLO_STATEMENT_THEY_SUPPORTED_OUR_HOSTING;
				int iTurnsBetweenStatements = GC.getOPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_NUM_TURNS();

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					eStatement = eTempStatement;
			}
		}
	}
}







void CvDiplomacyAI::DoBeginDiploWithHuman()
{
	if(!GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
	{
#ifdef NO_LEADER_SCREEN
		CvPreGame::pushGameType(GAME_NETWORK_MULTIPLAYER);
#endif
		LeaderheadAnimationTypes eAnimation = LEADERHEAD_ANIM_NEUTRAL_HELLO;
		const char* szText = GetGreetHumanMessage(eAnimation);
#ifdef NO_LEADER_SCREEN
		CvPreGame::pushGameType(GAME_NETWORK_MULTIPLAYER);
#endif
		gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_DEFAULT_ROOT, szText, eAnimation);
	}
}


void CvDiplomacyAI::DoBeginDiploWithHumanEspionageResult()
{
	if(!GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR) && !IsAtWar(GC.getGame().getActivePlayer()))
	{
		LeaderheadAnimationTypes eAnimation = LEADERHEAD_ANIM_NEUTRAL_HELLO;
		const char* szText = GetDiploStringForMessage(DIPLO_MESSAGE_CONFRONT_YOU_KILLED_MY_SPY, GC.getGame().getActivePlayer());
		gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_CONFRONT_YOU_KILLED_MY_SPY, szText, eAnimation);
	}
}


void CvDiplomacyAI::DoBeginDiploWithHumanInDiscuss()
{
	if(!GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
	{
		LeaderheadAnimationTypes eAnimation = LEADERHEAD_ANIM_NEUTRAL_HELLO;
		const char* szText = GetGreetHumanMessage(eAnimation);
#ifdef NO_LEADER_SCREEN
		CvPreGame::pushGameType(GAME_NETWORK_MULTIPLAYER);
#endif
		gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, szText, eAnimation);
	}
}



const char* CvDiplomacyAI::GetDiploStringForMessage(DiploMessageTypes eDiploMessage, PlayerTypes eForPlayer)
{
	return GetDiploStringForMessage(eDiploMessage, eForPlayer, Localization::String::Empty, Localization::String::Empty);
}


const char* CvDiplomacyAI::GetDiploStringForMessage(DiploMessageTypes eDiploMessage, PlayerTypes eForPlayer, const Localization::String& strOptionalKey1)
{
	return GetDiploStringForMessage(eDiploMessage, eForPlayer, strOptionalKey1, Localization::String::Empty);
}

const char* CvDiplomacyAI::GetDiploStringForMessage(DiploMessageTypes eDiploMessage, PlayerTypes eForPlayer, const Localization::String& strOptionalKey1, const Localization::String& strOptionalKey2)
{
	CvAssertMsg(eDiploMessage >= 0, "DIPLOMACY_AI: Invalid DiploMessageType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDiploMessage < NUM_DIPLO_MESSAGE_TYPES, "DIPLOMACY_AI: Invalid DiploMessageType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eForPlayer >= NO_PLAYER, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eForPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* strText;

	switch(eDiploMessage)
	{





	case DIPLO_MESSAGE_INTRO:
		strText = GetDiploTextFromTag("RESPONSE_FIRST_GREETING");
		break;


	case DIPLO_MESSAGE_DEFEATED:
		strText = GetDiploTextFromTag("RESPONSE_DEFEATED");
		break;


	case DIPLO_MESSAGE_GREETING_REPEAT_TOO_MUCH:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_REPEAT_TOO_MUCH");
		break;


	case DIPLO_MESSAGE_GREETING_REPEAT:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_REPEAT");
		break;


	case DIPLO_MESSAGE_GREETING_HOSTILE_REPEAT:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_REPEAT");
		break;


	case DIPLO_MESSAGE_GREETING_FRIENDLY_HELLO:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_POLITE_HELLO");
		break;


	case DIPLO_MESSAGE_GREETING_NEUTRAL_HELLO:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_NEUTRAL_HELLO");
		break;


	case DIPLO_MESSAGE_GREETING_HOSTILE_HELLO:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_HELLO");
		break;


	case DIPLO_MESSAGE_GREETING_DESTRUCTION_LOOMS:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_DESTRUCTION_LOOMS");
		break;


	case DIPLO_MESSAGE_GREETING_AT_WAR_WANTS_PEACE:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_AT_WAR_WANTS_PEACE");
		break;


	case DIPLO_MESSAGE_GREETING_AT_WAR_HOSTILE:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_AT_WAR_HOSTILE");
		break;


	case DIPLO_MESSAGE_GREETING_WILL_ACCEPT_SURRENDER:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_WILL_ACCEPT_SURRENDER");
		break;


	case DIPLO_MESSAGE_GREETING_RESEARCH_AGREEMENT:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_RESEARCH_AGREEMENT");
		break;


	case DIPLO_MESSAGE_GREETING_BROKEN_MILITARY_PROMISE:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_BROKEN_MILITARY_PROMISE");
		break;


	case DIPLO_MESSAGE_GREETING_WORKING_WITH:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_WORKING_WITH");
		break;


	case DIPLO_MESSAGE_GREETING_WORKING_AGAINST:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_WORKING_AGAINST", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_GREETING_COOP_WAR:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_COOP_WAR", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_AT_WAR:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_HUMAN_AT_WAR");
		break;


	case DIPLO_MESSAGE_GREETING_HUMAN_AT_WAR:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HUMAN_AT_WAR");
		break;


	case DIPLO_MESSAGE_GREETING_HOSTILE_AGGRESSIVE_MILITARY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_AGGRESSIVE_MILITARY");
		break;


	case DIPLO_MESSAGE_GREETING_AGGRESSIVE_MILITARY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_AGGRESSIVE_MILITARY");
		break;


	case DIPLO_MESSAGE_GREETING_HOSTILE_AGGRESSIVE_EXPANSION:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_AGGRESSIVE_EXPANSION");
		break;


	case DIPLO_MESSAGE_GREETING_AGGRESSIVE_EXPANSION:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_AGGRESSIVE_EXPANSION");
		break;


	case DIPLO_MESSAGE_GREETING_HOSTILE_AGGRESSIVE_PLOT_BUYING:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_AGGRESSIVE_PLOT_BUYING");
		break;


	case DIPLO_MESSAGE_GREETING_AGGRESSIVE_PLOT_BUYING:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_AGGRESSIVE_PLOT_BUYING");
		break;


	case DIPLO_MESSAGE_GREETING_FRIENDLY_STRONG_MILITARY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_FRIENDLY_STRONG_MILITARY");
		break;


	case DIPLO_MESSAGE_GREETING_FRIENDLY_STRONG_ECONOMY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_FRIENDLY_STRONG_ECONOMY");
		break;


	case DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_FEW_CITIES:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_HUMAN_FEW_CITIES");
		break;


	case DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_SMALL_ARMY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_HUMAN_SMALL_ARMY");
		break;


	case DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_IS_WARMONGER:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_HUMAN_IS_WARMONGER");
		break;






	case DIPLO_MESSAGE_DOT_DOT_DOT:
		strText = GetDiploTextFromTag("RESPONSE_DOT_DOT_DOT");
		break;


	case DIPLO_MESSAGE_LETS_HEAR_IT:
		strText = GetDiploTextFromTag("RESPONSE_LETS_HEAR_IT");
		break;


	case DIPLO_MESSAGE_PEACE_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_PEACE_OFFER");
		break;


	case DIPLO_MESSAGE_DEMAND:
		strText = GetDiploTextFromTag("RESPONSE_DEMAND");
		break;


	case DIPLO_MESSAGE_REQUEST:
		strText = GetDiploTextFromTag("RESPONSE_REQUEST");
		break;


	case DIPLO_MESSAGE_LUXURY_TRADE:
		strText = GetDiploTextFromTag("RESPONSE_LUXURY_TRADE");
		break;


	case DIPLO_MESSAGE_EMBASSY_EXCHANGE:
		strText = GetDiploTextFromTag("RESPONSE_EMBASSY_EXCHANGE");
		break;

	case DIPLO_MESSAGE_EMBASSY_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_EMBASSY_OFFER");
		break;


	case DIPLO_MESSAGE_OPEN_BORDERS_EXCHANGE:
		strText = GetDiploTextFromTag("RESPONSE_OPEN_BORDERS_EXCHANGE");
		break;


	case DIPLO_MESSAGE_OPEN_BORDERS_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_OPEN_BORDERS_OFFER");
		break;


	case DIPLO_MESSAGE_PLAN_RESEARCH_AGREEMENT:
		strText = GetDiploTextFromTag("RESPONSE_PLAN_RESEARCH_AGREEMENT");
		break;


	case DIPLO_MESSAGE_RESEARCH_AGREEMENT_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_RESEARCH_AGREEMENT_OFFER");
		break;

	case DIPLO_MESSAGE_RENEW_DEAL:
		strText = GetDiploTextFromTag("RESPONSE_RENEW_DEAL");
		break;

	case DIPLO_MESSAGE_WANT_MORE_RENEW_DEAL:
		strText = GetDiploTextFromTag("RESPONSE_WANT_MORE_RENEW_DEAL");
		break;






	case DIPLO_MESSAGE_HOSTILE_AGGRESSIVE_MILITARY_WARNING:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_AGGRESSIVE_MILITARY_WARNING");
		break;


	case DIPLO_MESSAGE_AGGRESSIVE_MILITARY_WARNING:
		strText = GetDiploTextFromTag("RESPONSE_AGGRESSIVE_MILITARY_WARNING");
		break;


	case DIPLO_MESSAGE_EXPANSION_SERIOUS_WARNING:
		strText = GetDiploTextFromTag("RESPONSE_EXPANSION_SERIOUS_WARNING");
		break;


	case DIPLO_MESSAGE_EXPANSION_WARNING:
		strText = GetDiploTextFromTag("RESPONSE_EXPANSION_WARNING");
		break;


	case DIPLO_MESSAGE_EXPANSION_BROKEN_PROMISE:
		strText = GetDiploTextFromTag("RESPONSE_EXPANSION_BROKEN_PROMISE");
		break;


	case DIPLO_MESSAGE_PLOT_BUYING_SERIOUS_WARNING:
		strText = GetDiploTextFromTag("RESPONSE_PLOT_BUYING_SERIOUS_WARNING");
		break;


	case DIPLO_MESSAGE_PLOT_BUYING_WARNING:
		strText = GetDiploTextFromTag("RESPONSE_PLOT_BUYING_WARNING");
		break;


	case DIPLO_MESSAGE_PLOT_BUYING_BROKEN_PROMISE:
		strText = GetDiploTextFromTag("RESPONSE_PLOT_BUYING_BROKEN_PROMISE");
		break;


	case DIPLO_MESSAGE_HOSTILE_WE_ATTACKED_YOUR_MINOR:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_WE_ATTACKED_YOUR_MINOR", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_WE_ATTACKED_YOUR_MINOR:
		strText = GetDiploTextFromTag("RESPONSE_WE_ATTACKED_YOUR_MINOR", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_HOSTILE_WE_BULLIED_YOUR_MINOR:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_WE_BULLIED_YOUR_MINOR", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_WE_BULLIED_YOUR_MINOR:
		strText = GetDiploTextFromTag("RESPONSE_WE_BULLIED_YOUR_MINOR", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_WORK_WITH_US:
		strText = GetDiploTextFromTag("RESPONSE_WORK_WITH_US");
		break;


	case DIPLO_MESSAGE_END_WORK_WITH_US:
		strText = GetEndDoFMessage(eForPlayer);
		break;


	case DIPLO_MESSAGE_WORK_AGAINST_SOMEONE:
		strText = GetDiploTextFromTag("RESPONSE_WORK_AGAINST_SOMEONE", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_END_WORK_AGAINST_SOMEONE:
		strText = GetEndWorkAgainstSomeoneMessage(eForPlayer, strOptionalKey1);
		break;


	case DIPLO_MESSAGE_COOP_WAR_REQUEST:
		strText = GetDiploTextFromTag("RESPONSE_COOP_WAR_REQUEST", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_COOP_WAR_TIME:
		strText = GetDiploTextFromTag("RESPONSE_COOP_WAR_TIME", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_NOW_UNFORGIVABLE:
		strText = GetDiploTextFromTag("RESPONSE_NOW_UNFORGIVABLE");
		break;


	case DIPLO_MESSAGE_NOW_ENEMY:
		strText = GetDiploTextFromTag("RESPONSE_NOW_ENEMY");
		break;


	case DIPLO_MESSAGE_INSULT_ROOT:
		strText = GetInsultHumanMessage();
		break;


	case DIPLO_MESSAGE_INSULT_GENERIC:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_GENERIC");
		break;


	case DIPLO_MESSAGE_INSULT_MILITARY:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_MILITARY");
		break;


	case DIPLO_MESSAGE_INSULT_NUKE:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_NUKE");
		break;


	case DIPLO_MESSAGE_INSULT_BULLY:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_BULLY");
		break;


	case DIPLO_MESSAGE_INSULT_UNHAPPINESS:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_UNHAPPINESS");
		break;


	case DIPLO_MESSAGE_INSULT_CITIES:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_CITIES");
		break;


	case DIPLO_MESSAGE_INSULT_POPULATION:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_POPULATION");
		break;


	case DIPLO_MESSAGE_INSULT_CULTURE:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_CULTURE");
		break;


	case DIPLO_MESSAGE_COMPLIMENT:
		strText = GetDiploTextFromTag("RESPONSE_COMPLIMENT");
		break;


	case DIPLO_MESSAGE_BOOT_KISSING:
		strText = GetDiploTextFromTag("RESPONSE_BOOT_KISSING");
		break;


	case DIPLO_MESSAGE_WARMONGER:
		strText = GetDiploTextFromTag("RESPONSE_WARMONGER");
		break;


	case DIPLO_MESSAGE_MINOR_CIV_COMPETITION:
		strText = GetDiploTextFromTag("RESPONSE_MINOR_CIV_COMPETITION", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_PLEASED:
		strText = GetDiploTextFromTag("RESPONSE_PLEASED");
		break;


	case DIPLO_MESSAGE_THANKFUL:
		strText = GetDiploTextFromTag("RESPONSE_THANKFUL");
		break;


	case DIPLO_MESSAGE_DISAPPOINTED:
		strText = GetDiploTextFromTag("RESPONSE_DISAPPOINTED");
		break;


	case DIPLO_MESSAGE_SO_BE_IT:
		strText = GetDiploTextFromTag("RESPONSE_SO_BE_IT");
		break;


	case DIPLO_MESSAGE_RETURNED_CIVILIAN:
		strText = GetDiploTextFromTag("RESPONSE_RETURNED_CIVILIAN");
		break;


	case DIPLO_MESSAGE_CULTURE_BOMBED:
		strText = GetDiploTextFromTag("RESPONSE_CULTURE_BOMBED");
		break;






	case DIPLO_MESSAGE_DECLARATION_PROTECT_CITY_STATE:
		strText = GetDiploTextFromTag("RESPONSE_DECLARATION_PROTECT_CITY_STATE", strOptionalKey1, GetPlayer()->getCivilizationShortDescriptionKey());
		break;


	case DIPLO_MESSAGE_DECLARATION_ABANDON_CITY_STATE:
		strText = GetDiploTextFromTag("RESPONSE_DECLARATION_ABANDON_CITY_STATE", strOptionalKey1, GetPlayer()->getCivilizationShortDescriptionKey());
		break;






	case DIPLO_MESSAGE_REPEAT_NO:
		strText = GetDiploTextFromTag("RESPONSE_REPEAT_NO");
		break;


	case DIPLO_MESSAGE_DONT_SETTLE_YES:
		strText = GetDiploTextFromTag("RESPONSE_DONT_SETTLE_YES");
		break;


	case DIPLO_MESSAGE_DONT_SETTLE_NO:
		strText = GetDiploTextFromTag("RESPONSE_DONT_SETTLE_NO");
		break;


	case DIPLO_MESSAGE_WORK_WITH_US_YES:
		strText = GetDiploTextFromTag("RESPONSE_WORK_WITH_US_YES");
		break;


	case DIPLO_MESSAGE_WORK_WITH_US_NO:
		strText = GetDiploTextFromTag("RESPONSE_WORK_WITH_US_NO");
		break;


	case DIPLO_MESSAGE_WORK_AGAINST_SOMEONE_YES:
		strText = GetDiploTextFromTag("RESPONSE_WORK_AGAINST_SOMEONE_YES");
		break;


	case DIPLO_MESSAGE_WORK_AGAINST_SOMEONE_NO:
		strText = GetDiploTextFromTag("RESPONSE_WORK_AGAINST_SOMEONE_NO");
		break;


	case DIPLO_MESSAGE_COOP_WAR_YES:
		strText = GetDiploTextFromTag("RESPONSE_COOP_WAR_YES");
		break;


	case DIPLO_MESSAGE_COOP_WAR_NO:
		strText = GetDiploTextFromTag("RESPONSE_COOP_WAR_NO");
		break;


	case DIPLO_MESSAGE_COOP_WAR_SOON:
		strText = GetDiploTextFromTag("RESPONSE_COOP_WAR_SOON");
		break;


	case DIPLO_MESSAGE_HUMAN_DEMAND_YES:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DEMAND_YES");
		break;


	case DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_WEAK:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DEMAND_REFUSE_WEAK");
		break;


	case DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_HOSTILE:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DEMAND_REFUSE_HOSTILE");
		break;


	case DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_TOO_MUCH:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DEMAND_REFUSE_TOO_MUCH");
		break;


	case DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_TOO_SOON:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DEMAND_REFUSE_TOO_SOON");
		break;






	case DIPLO_MESSAGE_HUMAN_HOSTILE_AGGRESSIVE_MILITARY_WARNING_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_AGGRESSIVE_MILITARY_WARNING_BAD");
		break;


	case DIPLO_MESSAGE_HUMAN_HOSTILE_AGGRESSIVE_MILITARY_WARNING_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_AGGRESSIVE_MILITARY_WARNING_GOOD");
		break;


	case DIPLO_MESSAGE_HUMAN_AGGRESSIVE_MILITARY_WARNING_BAD:
		strText = GetDiploTextFromTag("RESPONSE_AGGRESSIVE_MILITARY_WARNING_BAD");
		break;


	case DIPLO_MESSAGE_HUMAN_AGGRESSIVE_MILITARY_WARNING_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_AGGRESSIVE_MILITARY_WARNING_GOOD");
		break;


	case DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_WE_ATTACKED_MINOR_BAD");
		break;


	case DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_WE_ATTACKED_MINOR_GOOD");
		break;


	case DIPLO_MESSAGE_HUMAN_WE_ATTACKED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_WE_ATTACKED_MINOR_BAD");
		break;


	case DIPLO_MESSAGE_HUMAN_WE_ATTACKED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_WE_ATTACKED_MINOR_GOOD");
		break;


	case DIPLO_MESSAGE_HUMAN_HOSTILE_WE_BULLIED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_WE_BULLIED_MINOR_BAD");
		break;


	case DIPLO_MESSAGE_HUMAN_HOSTILE_WE_BULLIED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_WE_BULLIED_MINOR_GOOD");
		break;


	case DIPLO_MESSAGE_HUMAN_WE_BULLIED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_WE_BULLIED_MINOR_BAD");
		break;


	case DIPLO_MESSAGE_HUMAN_WE_BULLIED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_WE_BULLIED_MINOR_GOOD");
		break;


	case DIPLO_MESSAGE_HUMAN_ATTACKED_PROTECTED_CITY_STATE:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_PROTECTED_CITY_STATE", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_HUMAN_ATTACKED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_ATTACKED_MINOR_BAD");
		break;


	case DIPLO_MESSAGE_HUMAN_ATTACKED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_ATTACKED_MINOR_GOOD");
		break;


	case DIPLO_MESSAGE_HUMAN_KILLED_PROTECTED_CITY_STATE:
		strText = GetDiploTextFromTag("RESPONSE_KILLED_PROTECTED_CITY_STATE", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_HUMAN_KILLED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_KILLED_MINOR_BAD");
		break;


	case DIPLO_MESSAGE_HUMAN_KILLED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_KILLED_MINOR_GOOD");
		break;


	case DIPLO_MESSAGE_HUMAN_BULLIED_PROTECTED_CITY_STATE:
		strText = GetDiploTextFromTag("RESPONSE_BULLIED_PROTECTED_CITY_STATE", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_HUMAN_BULLIED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_BULLIED_MINOR_BAD");
		break;


	case DIPLO_MESSAGE_HUMAN_BULLIED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_BULLIED_MINOR_GOOD");
		break;


	case DIPLO_MESSAGE_HUMAN_SERIOUS_EXPANSION_WARNING_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_SERIOUS_EXPANSION_WARNING_BAD");
		break;


	case DIPLO_MESSAGE_HUMAN_SERIOUS_EXPANSION_WARNING_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_SERIOUS_EXPANSION_WARNING_GOOD");
		break;


	case DIPLO_MESSAGE_HUMAN_EXPANSION_WARNING_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_EXPANSION_WARNING_BAD");
		break;


	case DIPLO_MESSAGE_HUMAN_EXPANSION_WARNING_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_EXPANSION_WARNING_GOOD");
		break;


	case DIPLO_MESSAGE_HUMAN_SERIOUS_PLOT_BUYING_WARNING_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_SERIOUS_PLOT_BUYING_WARNING_BAD");
		break;


	case DIPLO_MESSAGE_HUMAN_SERIOUS_PLOT_BUYING_WARNING_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_SERIOUS_PLOT_BUYING_WARNING_GOOD");
		break;


	case DIPLO_MESSAGE_HUMAN_PLOT_BUYING_WARNING_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_PLOT_BUYING_WARNING_BAD");
		break;


	case DIPLO_MESSAGE_HUMAN_PLOT_BUYING_WARNING_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_PLOT_BUYING_WARNING_GOOD");
		break;






	case DIPLO_MESSAGE_PEACE_WHAT_WILL_HUMAN_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_PEACE_WHAT_WILL_HUMAN_OFFER");
		break;


	case DIPLO_MESSAGE_PEACE_MADE_BY_HUMAN_GRACIOUS:
		strText = GetDiploTextFromTag("RESPONSE_PEACE_MADE_BY_HUMAN_GRACIOUS");
		break;


	case DIPLO_MESSAGE_NO_PEACE:
		strText = GetDiploTextFromTag("RESPONSE_NO_PEACE");
		break;


	case DIPLO_MESSAGE_TOO_SOON_NO_PEACE:
		strText = GetDiploTextFromTag("RESPONSE_TOO_SOON_NO_PEACE");
		break;






	case DIPLO_MESSAGE_REPEAT_TRADE_TOO_MUCH:
		strText = GetDiploTextFromTag("RESPONSE_REPEAT_TRADE_TOO_MUCH");
		break;


	case DIPLO_MESSAGE_REPEAT_TRADE:
		strText = GetDiploTextFromTag("RESPONSE_REPEAT_TRADE");
		break;


	case DIPLO_MESSAGE_TRADE_ACCEPT_GENEROUS:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_ACCEPT_GENEROUS");
		break;


	case DIPLO_MESSAGE_TRADE_ACCEPT_ACCEPTABLE:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_ACCEPT_ACCEPTABLE");
		break;


	case DIPLO_MESSAGE_TRADE_ACCEPT_AI_DEMAND:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_ACCEPT_AI_DEMAND");
		break;


	case DIPLO_MESSAGE_TRADE_ACCEPT_HUMAN_CONCESSIONS:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_ACCEPT_HUMAN_CONCESSIONS");
		break;


	case DIPLO_MESSAGE_TRADE_REJECT_UNACCEPTABLE:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_REJECT_UNACCEPTABLE");
		break;


	case DIPLO_MESSAGE_TRADE_REJECT_INSULTING:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_REJECT_INSULTING");
		break;


	case DIPLO_MESSAGE_TRADE_DEAL_UNCHANGED:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_DEAL_UNCHANGED");
		break;


	case DIPLO_MESSAGE_TRADE_AI_MAKES_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_AI_MAKES_OFFER");
		break;


	case DIPLO_MESSAGE_TRADE_NO_DEAL_POSSIBLE:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_NO_DEAL_POSSIBLE");
		break;

	case DIPLO_MESSAGE_TRADE_CANT_MATCH_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_CANT_MATCH_OFFER");
		break;






	case DIPLO_MESSAGE_ATTACKED_ROOT:
		strText = GetAttackedByHumanMessage();
		break;


	case DIPLO_MESSAGE_ATTACKED_HOSTILE:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_HOSTILE");
		break;


	case DIPLO_MESSAGE_ATTACKED_WEAK_HOSTILE:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_WEAK_HOSTILE");
		break;


	case DIPLO_MESSAGE_ATTACKED_STRONG_HOSTILE:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_STRONG_HOSTILE");
		break;


	case DIPLO_MESSAGE_ATTACKED_EXCITED:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_EXCITED");
		break;


	case DIPLO_MESSAGE_ATTACKED_WEAK_EXCITED:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_WEAK_EXCITED");
		break;


	case DIPLO_MESSAGE_ATTACKED_STRONG_EXCITED:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_STRONG_EXCITED");
		break;


	case DIPLO_MESSAGE_ATTACKED_SAD:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_SAD");
		break;


	case DIPLO_MESSAGE_ATTACKED_BETRAYED:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_BETRAYED");
		break;


	case DIPLO_MESSAGE_ATTACKED_MILITARY_PROMISE_BROKEN:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_MILITARY_PROMISE_BROKEN");
		break;






	case DIPLO_MESSAGE_DOW_ROOT:
		strText = GetWarMessage(eForPlayer);
		break;


	case DIPLO_MESSAGE_DOW_GENERIC:
		strText = GetDiploTextFromTag("RESPONSE_DOW_GENERIC");
		break;


	case DIPLO_MESSAGE_DOW_LAND:
		strText = GetDiploTextFromTag("RESPONSE_DOW_LAND");
		break;


	case DIPLO_MESSAGE_DOW_WORLD_CONQUEST:
		strText = GetDiploTextFromTag("RESPONSE_DOW_WORLD_CONQUEST");
		break;


	case DIPLO_MESSAGE_DOW_OPPORTUNITY:
		strText = GetDiploTextFromTag("RESPONSE_DOW_OPPORTUNITY");
		break;


	case DIPLO_MESSAGE_DOW_DESPERATE:
		strText = GetDiploTextFromTag("RESPONSE_DOW_DESPERATE");
		break;


	case DIPLO_MESSAGE_DOW_BETRAYAL:
		strText = GetDiploTextFromTag("RESPONSE_DOW_BETRAYAL");
		break;


	case DIPLO_MESSAGE_DOW_WEAK_BETRAYAL:
		strText = GetDiploTextFromTag("RESPONSE_DOW_WEAK_BETRAYAL");
		break;


	case DIPLO_MESSAGE_DOW_REGRET:
		strText = GetDiploTextFromTag("RESPONSE_DOW_REGRET");
		break;


	case DIPLO_MESSAGE_WAR_DEMAND_REFUSED:
		strText = GetDiploTextFromTag("RESPONSE_WAR_DEMAND_REFUSED");
		break;






	case DIPLO_MESSAGE_DOF_AI_DENOUNCE_REQUEST:
		strText = GetDiploTextFromTag("RESPONSE_DOF_AI_DENOUNCE_REQUEST", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_DOF_AI_WAR_REQUEST:
		strText = GetDiploTextFromTag("RESPONSE_DOF_AI_WAR_REQUEST", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_DOF_NOT_HONORED:
		strText = GetDiploTextFromTag("RESPONSE_DOF_NOT_HONORED");
		break;


	case DIPLO_MESSAGE_AI_DOF_BACKSTAB:
		strText = GetDiploTextFromTag("RESPONSE_AI_DOF_BACKSTAB");
		break;


	case DIPLO_MESSAGE_RESPONSE_TO_BEING_DENOUNCED:
		strText = GetDiploTextFromTag("RESPONSE_RESPONSE_TO_BEING_DENOUNCED");
		break;


	case DIPLO_MESSAGE_HUMAN_DOFED_FRIEND:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DOFED_FRIEND", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_HUMAN_DOFED_ENEMY:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DOFED_ENEMY", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_HUMAN_DENOUNCED_FRIEND:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DENOUNCED_FRIEND", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_HUMAN_DENOUNCED_ENEMY:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DENOUNCED_ENEMY", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_HUMAN_DOF_SO_AI_DOF:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DOF_SO_AI_DOF", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_HUMAN_DENOUNCE_SO_AI_DENOUNCE:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DENOUNCE_SO_AI_DENOUNCE", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_HUMAN_DOF_SO_AI_DENOUNCE:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DOF_SO_AI_DENOUNCE", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_HUMAN_DENOUNCE_SO_AI_DOF:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DENOUNCE_SO_AI_DOF", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_GREETING_DENOUNCED_BY_AI:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_DENOUNCED_BY_AI");
		break;


	case DIPLO_MESSAGE_GREETING_DENOUNCED_AI:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_DENOUNCED_AI");
		break;


	case DIPLO_MESSAGE_GREETING_OUR_DOF_WITH_AI_ENEMY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_OUR_DOF_WITH_ENEMY_OF_AI", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_GREETING_OUR_DOF_WITH_AI_FRIEND:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_OUR_DOF_WITH_FRIEND_OF_AI", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_GREETING_DENOUNCED_AI_FRIEND:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_DENOUNCED_FRIEND_OF_AI", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_GREETING_DENOUNCED_AI_ENEMY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_DENOUNCED_ENEMY_OF_AI", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_TOO_SOON_FOR_DOF:
		strText = GetDiploTextFromTag("RESPONSE_TOO_SOON_FOR_DOF");
		break;


	case DIPLO_MESSAGE_SAME_POLICIES_FREEDOM:
		strText = GetDiploTextFromTag("RESPONSE_SAME_POLICIES_FREEDOM");
		break;
	case DIPLO_MESSAGE_SAME_POLICIES_ORDER:
		strText = GetDiploTextFromTag("RESPONSE_SAME_POLICIES_ORDER");
		break;
	case DIPLO_MESSAGE_SAME_POLICIES_AUTOCRACY:
		strText = GetDiploTextFromTag("RESPONSE_SAME_POLICIES_AUTOCRACY");
		break;






	case DIPLO_MESSAGE_CAUGHT_YOUR_SPY:
		strText = GetDiploTextFromTag("RESPONSE_CAUGHT_YOUR_SPY");
		break;


	case DIPLO_MESSAGE_KILLED_YOUR_SPY:
		strText = GetDiploTextFromTag("RESPONSE_KILLED_YOUR_SPY");
		break;


	case DIPLO_MESSAGE_KILLED_MY_SPY:
		strText = GetDiploTextFromTag("RESPONSE_KILLED_MY_SPY");
		break;

	case DIPLO_MESSAGE_CONFRONT_YOU_KILLED_MY_SPY:
		strText = GetDiploTextFromTag("RESPONSE_CONFRONT_YOU_KILLED_MY_SPY");
		break;


	case DIPLO_MESSAGE_STOP_CONVERSIONS:
		strText = GetDiploTextFromTag("RESPONSE_STOP_CONVERSIONS");
		break;

	case DIPLO_MESSAGE_HUMAN_CAUGHT_YOUR_SPY_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_CAUGHT_YOUR_SPY_GOOD");
		break;

	case DIPLO_MESSAGE_HUMAN_CAUGHT_YOUR_SPY_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_CAUGHT_YOUR_SPY_BAD");
		break;


	case DIPLO_MESSAGE_STOP_SPYING_YES:
		strText = GetDiploTextFromTag("RESPONSE_STOP_SPYING_YES");
		break;


	case DIPLO_MESSAGE_STOP_SPYING_NO:
		strText = GetDiploTextFromTag("RESPONSE_STOP_SPYING_NO");
		break;

	case DIPLO_MESSAGE_WARNED_ABOUT_INTRIGUE:
		strText = GetDiploTextFromTag("RESPONSE_WARNED_ABOUT_INTRIGUE", strOptionalKey1);
		break;

	case DIPLO_MESSAGE_SHARE_INTRIGUE:
		strText = GetDiploTextFromTag("RESPONSE_SHARE_INTRIGUE_DECEPTION", strOptionalKey1);
		break;

	case DIPLO_MESSAGE_SHARE_INTRIGUE_ARMY_SNEAK_ATTACK_KNOWN_CITY:
		strText = GetDiploTextFromTag("RESPONSE_SHARE_INTRIGUE_ARMY_SNEAK_ATTACK_KNOWN_CITY", strOptionalKey1, strOptionalKey2);
		break;

	case DIPLO_MESSAGE_SHARE_INTRIGUE_ARMY_SNEAK_ATTACK_UNKNOWN_CITY:
		strText = GetDiploTextFromTag("RESPONSE_SHARE_INTRIGUE_ARMY_SNEAK_ATTACK_UNKNOWN_CITY", strOptionalKey1);
		break;

	case DIPLO_MESSAGE_SHARE_INTRIGUE_AMPHIBIOUS_SNEAK_ATTACK_KNOWN_CITY:
		strText = GetDiploTextFromTag("RESPONSE_SHARE_INTRIGUE_AMPHIBIOUS_SNEAK_ATTACK_KNOWN_CITY", strOptionalKey1, strOptionalKey2);
		break;

	case DIPLO_MESSAGE_SHARE_INTRIGUE_AMPHIBIOUS_SNEAK_ATTACK_UNKNOWN_CITY:
		strText = GetDiploTextFromTag("RESPONSE_SHARE_INTRIGUE_AMPHIBIOUS_SNEAK_ATTACK_UNKNOWN_CITY", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_HUMAN_KILLED_MY_SPY_UNFORGIVEN:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_KILLED_MY_SPY_UNFORGIVEN");
		break;


	case DIPLO_MESSAGE_HUMAN_KILLED_MY_SPY_FORGIVEN:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_KILLED_MY_SPY_FORGIVEN");
		break;

	case DIPLO_MESSAGE_HUMAN_STOP_CONVERSIONS_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_STOP_CONVERSIONS_GOOD");
		break;

	case DIPLO_MESSAGE_HUMAN_STOP_CONVERSIONS_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_STOP_CONVERSIONS_BAD");
		break;


	case DIPLO_MESSAGE_STOP_CONVERSIONS_AGREE:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_STOP_CONVERSIONS_AGREE");
		break;


	case DIPLO_MESSAGE_STOP_CONVERSIONS_DISAGREE:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_STOP_CONVERSIONS_DISAGREE");
		break;

	case DIPLO_MESSAGE_STOP_DIGGING:
		strText = GetDiploTextFromTag("RESPONSE_STOP_DIGGING");
		break;

	case DIPLO_MESSAGE_STOP_DIGGING_AGREE:
		strText = GetDiploTextFromTag("RESPONSE_STOP_DIGGING_AGREE");
		break;

	case DIPLO_MESSAGE_STOP_DIGGING_DISAGREE:
		strText = GetDiploTextFromTag("RESPONSE_STOP_DIGGING_DISAGREE");
		break;

	case DIPLO_MESSAGE_HUMAN_STOP_DIGGING_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_STOP_DIGGING_BAD");
		break;

	case DIPLO_MESSAGE_HUMAN_STOP_DIGGING_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_STOP_DIGGING_GOOD");
		break;





	case DIPLO_MESSAGE_WE_LIKED_THEIR_PROPOSAL:
		strText = GetDiploTextFromTag("RESPONSE_WE_LIKE_HUMAN_PROPOSAL", strOptionalKey1);
		break;

	case DIPLO_MESSAGE_WE_DISLIKED_THEIR_PROPOSAL:
		strText = GetDiploTextFromTag("RESPONSE_WE_DISLIKE_HUMAN_PROPOSAL", strOptionalKey1);
		break;

	case DIPLO_MESSAGE_THEY_SUPPORTED_OUR_PROPOSAL:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_SUPPORTED_OUR_PROPOSAL", strOptionalKey1);
		break;

	case DIPLO_MESSAGE_THEY_FOILED_OUR_PROPOSAL:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_FOILED_OUR_PROPOSAL", strOptionalKey1);
		break;

	case DIPLO_MESSAGE_THEY_SUPPORTED_OUR_HOSTING:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_SUPPORTED_OUR_HOSTING", strOptionalKey1);
		break;


	case DIPLO_MESSAGE_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_FREEDOM:
		strText = GetDiploTextFromTag("RESPONSE_CIVIL_RESISTANCE_ON_AI_BY_FREEDOM");
		break;
	case DIPLO_MESSAGE_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_ORDER:
		strText = GetDiploTextFromTag("RESPONSE_CIVIL_RESISTANCE_ON_AI_BY_ORDER");
		break;
	case DIPLO_MESSAGE_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_AUTOCRACY:
		strText = GetDiploTextFromTag("RESPONSE_CIVIL_RESISTANCE_ON_AI_BY_AUTOCRACY");
		break;
	case DIPLO_MESSAGE_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_FREEDOM:
		strText = GetDiploTextFromTag("RESPONSE_CIVIL_RESISTANCE_ON_HUMAN_BY_FREEDOM");
		break;
	case DIPLO_MESSAGE_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_ORDER:
		strText = GetDiploTextFromTag("RESPONSE_CIVIL_RESISTANCE_ON_HUMAN_BY_ORDER");
		break;
	case DIPLO_MESSAGE_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_AUTOCRACY:
		strText = GetDiploTextFromTag("RESPONSE_CIVIL_RESISTANCE_ON_HUMAN_BY_AUTOCRACY");
		break;
	case DIPLO_MESSAGE_SWITCH_OUR_IDEOLOGY_FREEDOM:
		strText = GetDiploTextFromTag("RESPONSE_SWITCHED_TO_FREEDOM");
		break;
	case DIPLO_MESSAGE_SWITCH_OUR_IDEOLOGY_ORDER:
		strText = GetDiploTextFromTag("RESPONSE_SWITCHED_TO_ORDER");
		break;
	case DIPLO_MESSAGE_SWITCH_OUR_IDEOLOGY_AUTOCRACY:
		strText = GetDiploTextFromTag("RESPONSE_SWITCHED_TO_AUTOCRACY");
		break;
	case DIPLO_MESSAGE_YOUR_CULTURE_INFLUENTIAL:
		strText = GetDiploTextFromTag("RESPONSE_INFLUENTIAL_ON_AI");
		break;
	case DIPLO_MESSAGE_OUR_CULTURE_INFLUENTIAL:
		strText = GetDiploTextFromTag("RESPONSE_INFLUENTIAL_ON_HUMAN");
		break;






	default:
		strText = "NO MESSAGE. Trying to get Diplo string.  Something has gone wrong, somehow.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.";
		CvAssert(false);
		break;
	}

	return strText;
}



void CvDiplomacyAI::DoFromUIDiploEvent(PlayerTypes eFromPlayer, FromUIDiploEventTypes eEvent, int iArg1, int iArg2)
{
	CvAssertMsg(eEvent >= 0, "DIPLOMACY_AI: Invalid FromUIDiploEventType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eEvent < NUM_FROM_UI_DIPLO_EVENTS, "DIPLOMACY_AI: Invalid FromUIDiploEventType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* strText = "";

	TeamTypes eFromTeam = GET_PLAYER(eFromPlayer).getTeam();


	bool bActivePlayer = GC.getGame().getActivePlayer() == eFromPlayer;

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	switch(eEvent)
	{



	case FROM_UI_DIPLO_EVENT_HUMAN_DECLARES_WAR:
	{
		GET_TEAM(eFromTeam).declareWar(GetTeam());

		if(bActivePlayer)
		{
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_ROOT);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN, strText, LEADERHEAD_ANIM_ATTACKED, iArg1);
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_HUMAN_NEGOTIATE_PEACE:
	{
		if(bActivePlayer)
		{
			PeaceTreatyTypes ePeaceTreatyImWillingToOffer = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToOffer(eFromPlayer);
			PeaceTreatyTypes ePeaceTreatyImWillingToAccept = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToAccept(eFromPlayer);


			if(IsWillingToMakePeaceWithHuman(eFromPlayer) &&
			        ePeaceTreatyImWillingToOffer >= PEACE_TREATY_WHITE_PEACE && ePeaceTreatyImWillingToAccept >= PEACE_TREATY_WHITE_PEACE)
			{












				GetPlayer()->GetDealAI()->DoTradeScreenOpened();

				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PEACE_WHAT_WILL_HUMAN_OFFER);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_TRADE, strText, LEADERHEAD_ANIM_LETS_HEAR_IT);
			}
			else
			{

				if(GetPlayerNumTurnsAtWar(eFromPlayer) < 1)
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_TOO_SOON_NO_PEACE);

				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_NO_PEACE);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN, strText, LEADERHEAD_ANIM_NO);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_HUMAN_WANTS_DISCUSSION:
	{
		if(bActivePlayer)
		{
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_LETS_HEAR_IT);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_LETS_HEAR_IT);
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_DONT_SETTLE:
	{

		if(IsDontSettleMessageTooSoon(eFromPlayer))
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_REPEAT_NO);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NO);
			}
		}

		else
		{
			SetPlayerNoSettleRequestCounter(eFromPlayer, 0);

			bool bAcceptable;


			if(GetPlayer()->getTeam() == GET_PLAYER(eFromPlayer).getTeam())
				bAcceptable = true;

			else
				bAcceptable = IsDontSettleAcceptable(eFromPlayer);

			if(bAcceptable)
				SetPlayerNoSettleRequestAccepted(eFromPlayer, true);

			if(bActivePlayer)
			{
				if(bAcceptable)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_DONT_SETTLE_YES);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NEGATIVE);
				}
				else
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_DONT_SETTLE_NO);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NO);
				}
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_SPYING:
	{

		if(IsStopSpyingMessageTooSoon(eFromPlayer))
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_REPEAT_NO);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NO);
			}
		}

		else
		{
			SetPlayerStopSpyingRequestCounter(eFromPlayer, 0);

			bool bAcceptable;


			if(GetPlayer()->getTeam() == GET_PLAYER(eFromPlayer).getTeam())
			{
				bAcceptable = true;
			}

			else
			{
				bAcceptable = IsStopSpyingAcceptable(eFromPlayer);
			}

			if(bAcceptable)
			{
				SetPlayerStopSpyingRequestAccepted(eFromPlayer, true);
				m_pPlayer->GetEspionageAI()->EvaluateSpiesAssignedToTargetPlayer(eFromPlayer);
			}

			if(bActivePlayer)
			{
				DiploUIStateTypes eStateType = DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED;
				if(iArg1 == 1)
				{
					eStateType = DIPLO_UI_STATE_BLANK_DISCUSSION_RETURN_TO_ROOT;
				}

				LeaderheadAnimationTypes eAnimType;

				if(bAcceptable)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_STOP_SPYING_YES);
					eAnimType = LEADERHEAD_ANIM_NEGATIVE;
				}
				else
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_STOP_SPYING_NO);
					eAnimType = LEADERHEAD_ANIM_NO;
				}

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, eStateType, strText, eAnimType);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_SPREADING_RELIGION:
		{
			if (bActivePlayer)
			{	
				SetPlayerAskedNotToConvert(eFromPlayer, true);
				DiploUIStateTypes eStateType = DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED;
				if(iArg1 == 1)
				{
					eStateType = DIPLO_UI_STATE_BLANK_DISCUSSION_RETURN_TO_ROOT;
				}

				LeaderheadAnimationTypes eAnimType;

				bool bIsYes = IsStopSpreadingReligionAcceptable(eFromPlayer);
				if (bIsYes)
				{					
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_STOP_CONVERSIONS_AGREE);
					eAnimType = LEADERHEAD_ANIM_NEGATIVE;
				}
				else
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_STOP_CONVERSIONS_DISAGREE);
					eAnimType = LEADERHEAD_ANIM_NO;
				}

				SetPlayerAgreeNotToConvert(eFromPlayer, bIsYes);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, eStateType, strText, eAnimType);
			}
			break;
		}




	case FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_DIGGING:
		{
			if (bActivePlayer)
			{	
				SetPlayerAskedNotToDig(eFromPlayer, true);
				DiploUIStateTypes eStateType = DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED;
				if(iArg1 == 1)
				{
					eStateType = DIPLO_UI_STATE_BLANK_DISCUSSION_RETURN_TO_ROOT;
				}

				LeaderheadAnimationTypes eAnimType;

				bool bIsYes = IsStopDiggingAcceptable(eFromPlayer);
				if (bIsYes)
				{					
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_STOP_DIGGING_AGREE);
					eAnimType = LEADERHEAD_ANIM_NEGATIVE;
				}
				else
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_STOP_DIGGING_DISAGREE);
					eAnimType = LEADERHEAD_ANIM_NO;
				}

				SetPlayerAgreeNotToDig(eFromPlayer, bIsYes);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, eStateType, strText, eAnimType);
			}
			break;
		}




	case FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_WORK_WITH_US:
	{

		if(IsTooEarlyForDoF(eFromPlayer))
		{
			SetDoFCounter(eFromPlayer, 0);
			GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFCounter(eMyPlayer, 0);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_TOO_SOON_FOR_DOF);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NO);
			}
		}

		else if(IsDoFMessageTooSoon(eFromPlayer))
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_REPEAT_NO);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NO);
			}
		}

		else
		{
			SetDoFCounter(eFromPlayer, 0);
			GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFCounter(eMyPlayer, 0);

			bool bAcceptable = IsDoFAcceptable(eFromPlayer);
			if(bAcceptable)
			{
				SetDoFAccepted(eFromPlayer, true);
				GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFAccepted(eMyPlayer, true);
			}

			if(bActivePlayer)
			{
				if(bAcceptable)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_WORK_WITH_US_YES);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_POSITIVE);
				}
				else
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_WORK_WITH_US_NO);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NO);
				}
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_END_WORK_WITH_US:
	{
		SetDoFAccepted(eFromPlayer, false);
		GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFAccepted(eMyPlayer, false);
		SetDoFCounter(eFromPlayer, -666);
		GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFCounter(eMyPlayer, -666);

		if(bActivePlayer)
		{
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_DEMAND_HUMAN_REFUSAL:
	{

		bool bDeclareWar = false;

		if(GC.getGame().getJonRandNum(100, "Human demand refusal war rand.") < 50)
		{
			bDeclareWar = true;

			GET_TEAM(GetTeam()).declareWar(GET_PLAYER(eFromPlayer).getTeam());
			m_pPlayer->GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_NOW_AT_WAR);
			LogWarDeclaration(eFromPlayer);

			GetPlayer()->GetMilitaryAI()->RequestBasicAttack(eFromPlayer, 3);
		}

		if(bActivePlayer)
		{
			if(bDeclareWar)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_WAR_DEMAND_REFUSED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
			else
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_SO_BE_IT);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_REQUEST_HUMAN_REFUSAL:
	{
		GetPlayer()->GetDiplomacyAI()->ChangeRecentAssistValue(eFromPlayer, iArg1);

		if(bActivePlayer)
		{
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, strText, LEADERHEAD_ANIM_NEGATIVE);
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_AGGRESSIVE_MILITARY_WARNING_RESPONSE:
	{



		if(iArg1 == 1)
		{
			SetPlayerMadeMilitaryPromise(eFromPlayer, true);
			SetPlayerMilitaryPromiseCounter(eFromPlayer, 0);

			if(bActivePlayer)
			{
				if(IsActHostileTowardsHuman(eFromPlayer))
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_AGGRESSIVE_MILITARY_WARNING_GOOD);
				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_AGGRESSIVE_MILITARY_WARNING_GOOD);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		else if(iArg1 == 2)
		{
			GET_TEAM(eFromTeam).declareWar(GetTeam());

			SetPlayerIgnoredMilitaryPromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				if(IsActHostileTowardsHuman(eFromPlayer))
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_AGGRESSIVE_MILITARY_WARNING_BAD);
				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_AGGRESSIVE_MILITARY_WARNING_BAD);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_I_ATTACKED_YOUR_MINOR_CIV_RESPONSE:
	{

		PlayerTypes eMinor = (PlayerTypes) iArg2;
		CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
		CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
		if (!(eMinor >= MAX_MAJOR_CIVS && eMinor < MAX_CIV_PLAYERS))
		{

			strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_BAD);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			break;
		}

		CvPlayer* pMinor = &GET_PLAYER(eMinor);
		CvAssertMsg(pMinor, "Error triggering gameplay effects for diplomacy event involving a Protected city-state. Please send Anton your save file and version.");
		if (!pMinor)
		{

			strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_BAD);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			break;
		}
		
		CvAssertMsg(pMinor->GetMinorCivAI()->IsProtectedByMajor(eFromPlayer), "Diplomacy event involving a Protected city-state when there was no Pledge active. Please send Anton your save file and Version.");


		if(iArg1 == 1)
		{
			GC.getGame().DoMinorPledgeProtection(eFromPlayer, eMinor, false, true);
			
			if (bActivePlayer)
			{
				if(IsActHostileTowardsHuman(eFromPlayer))
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_GOOD);
				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_WE_ATTACKED_MINOR_GOOD);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		else if(iArg1 == 2)
		{
			SetOtherPlayerTurnsSinceSidedWithProtectedMinor(eFromPlayer, 0);

			if (bActivePlayer)
			{
				if(IsActHostileTowardsHuman(eFromPlayer))
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_BAD);
				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_WE_ATTACKED_MINOR_BAD);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_I_BULLIED_YOUR_MINOR_CIV_RESPONSE:
	{

		PlayerTypes eMinor = (PlayerTypes) iArg2;
		CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
		CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
		if (!(eMinor >= MAX_MAJOR_CIVS && eMinor < MAX_CIV_PLAYERS))
		{

			strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_BAD);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			break;
		}
		
		CvPlayer* pMinor = &GET_PLAYER(eMinor);
		CvAssertMsg(pMinor, "Error triggering gameplay effects for breaking a Pledge to Protect a city-state. Please send Anton your save file and version.");
		if (!pMinor)
		{

			strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_BAD);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			break;
		}
		
		CvAssertMsg(pMinor->GetMinorCivAI()->IsProtectedByMajor(eFromPlayer), "Diplomacy event involving a Protected city-state when there was no Pledge active. Please send Anton your save file and Version.");


		if(iArg1 == 1)
		{
			GC.getGame().DoMinorPledgeProtection(eFromPlayer, eMinor, false, true);
		
			if (bActivePlayer)
			{
				if(IsActHostileTowardsHuman(eFromPlayer))
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_BULLIED_MINOR_GOOD);
				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_WE_BULLIED_MINOR_GOOD);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		else if(iArg1 == 2)
		{
			SetOtherPlayerTurnsSinceSidedWithProtectedMinor(eFromPlayer, 0);

			if (bActivePlayer)
			{
				if(IsActHostileTowardsHuman(eFromPlayer))
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_BULLIED_MINOR_BAD);
				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_WE_BULLIED_MINOR_BAD);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_ATTACKED_MINOR_RESPONSE:
	{



		if(iArg1 == 1)
		{
			SetPlayerIgnoredAttackCityStatePromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_ATTACKED_MINOR_BAD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}

		else if(iArg1 == 2)
		{
			SetPlayerMadeAttackCityStatePromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_ATTACKED_MINOR_GOOD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_KILLED_MINOR_RESPONSE:
	{
		break;
	}




	case FROM_UI_DIPLO_EVENT_BULLIED_MINOR_RESPONSE:
	{



		if(iArg1 == 1)
		{
			SetPlayerIgnoredBullyCityStatePromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_BULLIED_MINOR_BAD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}

		else if(iArg1 == 2)
		{
			SetPlayerMadeBullyCityStatePromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_BULLIED_MINOR_GOOD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_EXPANSION_SERIOUS_WARNING_RESPONSE:
	{
		break;
	}




	case FROM_UI_DIPLO_EVENT_EXPANSION_WARNING_RESPONSE:
	{



		if(iArg1 == 1)
		{
			if (!IsPlayerIgnoredExpansionPromise(eFromPlayer))
			{
				SetIgnoredExpansionPromiseValue(eFromPlayer, GC.getOPINION_WEIGHT_EXPANSION_PROMISE_IGNORED_MAX() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
			}
			SetPlayerIgnoredExpansionPromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_EXPANSION_WARNING_BAD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}

		else if(iArg1 == 2)
		{
			SetPlayerMadeExpansionPromise(eFromPlayer, true);
			SetPlayerExpansionPromiseData(eFromPlayer, GetExpansionAggressivePosture(eFromPlayer));

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_EXPANSION_WARNING_GOOD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_PLOT_BUYING_SERIOUS_WARNING_RESPONSE:
	{
		break;
	}




	case FROM_UI_DIPLO_EVENT_PLOT_BUYING_WARNING_RESPONSE:
	{



		if(iArg1 == 1)
		{
			if (!IsPlayerIgnoredBorderPromise(eFromPlayer))
			{
				SetIgnoredBorderPromiseValue(eFromPlayer, GC.getOPINION_WEIGHT_BORDER_PROMISE_IGNORED_MAX() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
			}
			SetPlayerIgnoredBorderPromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_PLOT_BUYING_WARNING_BAD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}

		else if(iArg1 == 2)
		{
			SetPlayerMadeBorderPromise(eFromPlayer, true);
			SetPlayerBorderPromiseData(eFromPlayer, GetPlotBuyingAggressivePosture(eFromPlayer));

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_PLOT_BUYING_WARNING_GOOD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_WORK_WITH_US_RESPONSE:
	{


		SetDoFCounter(eFromPlayer, 0);
		GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFCounter(eMyPlayer, 0);


		if(iArg1 == 1)
		{
			SetDoFAccepted(eFromPlayer, true);
			GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFAccepted(eMyPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		else if(iArg1 == 2)
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}

		break;
	}









































	case FROM_UI_DIPLO_EVENT_DENOUNCE:
	{


		GET_PLAYER(eFromPlayer).GetDiplomacyAI()->DoDenouncePlayer(eMyPlayer);

		if(bActivePlayer)
		{
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_RESPONSE_TO_BEING_DENOUNCED);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_COOP_WAR_OFFER:
	{


		PlayerTypes eAgainstPlayer = (PlayerTypes) iArg1;


		if(IsCoopWarMessageTooSoon(eFromPlayer, eAgainstPlayer))
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_REPEAT_NO);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}
		else
		{

			SetCoopWarCounter(eFromPlayer, eAgainstPlayer, 0);

			CoopWarStates eAcceptableState;


			if(GetPlayer()->getTeam() == GET_PLAYER(eFromPlayer).getTeam())
				eAcceptableState = COOP_WAR_STATE_SOON;

			else
				eAcceptableState = GetWillingToAgreeToCoopWarState(eFromPlayer, eAgainstPlayer);

			SetCoopWarAcceptedState(eFromPlayer, eAgainstPlayer, eAcceptableState);


			if(eAcceptableState == COOP_WAR_STATE_ACCEPTED)
			{

				DeclareWar(eAgainstPlayer);
				GetPlayer()->GetMilitaryAI()->RequestBasicAttack(eAgainstPlayer, 1);


				TeamTypes eAgainstTeam = GET_PLAYER(eAgainstPlayer).getTeam();
				GET_TEAM(eFromTeam).declareWar(eAgainstTeam);

				int iLockedTurns =        GC.getCOOP_WAR_LOCKED_LENGTH();
				GET_TEAM(GetPlayer()->getTeam()).ChangeNumTurnsLockedIntoWar(eAgainstTeam, iLockedTurns);
				GET_TEAM(eFromTeam).ChangeNumTurnsLockedIntoWar(eAgainstTeam, iLockedTurns);

				if(bActivePlayer)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_COOP_WAR_YES);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
				}
			}

			else if(eAcceptableState == COOP_WAR_STATE_SOON)
			{
				if(bActivePlayer)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_COOP_WAR_SOON);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
				}
			}

			else
			{
				if(bActivePlayer)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_COOP_WAR_NO);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
				}
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_COOP_WAR_RESPONSE:
	{


		PlayerTypes eAgainstPlayer = (PlayerTypes) iArg2;

		SetCoopWarCounter(eFromPlayer, eAgainstPlayer, 0);


		if(iArg1 == 1 || iArg1 == 2)
		{

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}

		if(iArg1 == 3)
		{
			SetCoopWarAcceptedState(eFromPlayer, eAgainstPlayer, COOP_WAR_STATE_SOON);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		else if(iArg1 == 4)
		{
			SetCoopWarAcceptedState(eFromPlayer, eAgainstPlayer, COOP_WAR_STATE_ACCEPTED);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}


			DeclareWar(eAgainstPlayer);
			GetPlayer()->GetMilitaryAI()->RequestBasicAttack(eAgainstPlayer, 1);


			TeamTypes eAgainstTeam = GET_PLAYER(eAgainstPlayer).getTeam();
			GET_TEAM(eFromTeam).declareWar(eAgainstTeam);

			int iLockedTurns =        GC.getCOOP_WAR_LOCKED_LENGTH();
			GET_TEAM(GetPlayer()->getTeam()).ChangeNumTurnsLockedIntoWar(eAgainstTeam, iLockedTurns);
			GET_TEAM(eFromTeam).ChangeNumTurnsLockedIntoWar(eAgainstTeam, iLockedTurns);
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_COOP_WAR_NOW_RESPONSE:
	{


		PlayerTypes eAgainstPlayer = (PlayerTypes) iArg2;

		SetCoopWarCounter(eFromPlayer, eAgainstPlayer, 0);


		if(iArg1 == 1)
		{
			SetCoopWarAcceptedState(eFromPlayer, eAgainstPlayer, COOP_WAR_STATE_ACCEPTED);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}


			DeclareWar(eAgainstPlayer);
			GetPlayer()->GetMilitaryAI()->RequestBasicAttack(eAgainstPlayer, 1);


			TeamTypes eAgainstTeam = GET_PLAYER(eAgainstPlayer).getTeam();
			GET_TEAM(eFromTeam).declareWar(eAgainstTeam);

			int iLockedTurns =        GC.getCOOP_WAR_LOCKED_LENGTH();
			GET_TEAM(GetPlayer()->getTeam()).ChangeNumTurnsLockedIntoWar(eAgainstTeam, iLockedTurns);
			GET_TEAM(eFromTeam).ChangeNumTurnsLockedIntoWar(eAgainstTeam, iLockedTurns);
		}

		if(iArg1 == 2)
		{
			SetCoopWarAcceptedState(eFromPlayer, eAgainstPlayer, NO_COOP_WAR_STATE);
			SetPlayerBrokenCoopWarPromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_HUMAN_DEMAND:
	{

		DemandResponseTypes eResponse = (DemandResponseTypes) iArg1;


		DoDemandMade(eFromPlayer);

		if(bActivePlayer)
		{

			if(eResponse == DEMAND_RESPONSE_ACCEPT)
			{
				strText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DEMAND_YES);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_RETURN_TO_ROOT, strText, LEADERHEAD_ANIM_YES);
			}

			else
			{
				if(eResponse == DEMAND_RESPONSE_REFUSE_WEAK)
					strText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_WEAK);

				else if(eResponse == DEMAND_RESPONSE_REFUSE_HOSTILE)
					strText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_HOSTILE);

				else if(eResponse == DEMAND_RESPONSE_REFUSE_TOO_MUCH)
					strText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_TOO_MUCH);

				else if(eResponse == DEMAND_RESPONSE_REFUSE_TOO_SOON)
					strText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_TOO_SOON);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_RETURN_TO_ROOT, strText, LEADERHEAD_ANIM_NO);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_PLAN_RA_RESPONSE:
	{



		if(iArg1 == 1)
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		else if(iArg1 == 2)
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}

			DoCancelWantsResearchAgreementWithPlayer(eFromPlayer);
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_AI_REQUEST_DENOUNCE_RESPONSE:
	{

		PlayerTypes eAgainstPlayer = (PlayerTypes) iArg2;


		if(iArg1 == 1)
		{
			GET_PLAYER(eFromPlayer).GetDiplomacyAI()->DoDenouncePlayer(eAgainstPlayer);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		else if(iArg1 == 2)
		{

			if(IsFriendDenounceRefusalUnacceptable(eFromPlayer, eAgainstPlayer))
			{
				DoDenouncePlayer(eFromPlayer);
				LogDenounce(eFromPlayer,               false,              true);

				strText = GetDiploStringForMessage(DIPLO_MESSAGE_DOF_NOT_HONORED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
			else if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_CAUGHT_YOUR_SPY_RESPONSE:
	{



		if(iArg1 == 1)
		{
			SetPlayerIgnoredSpyPromise(eFromPlayer, true);
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_CAUGHT_YOUR_SPY_BAD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}

		else if(iArg1 == 2)
		{
			SetPlayerMadeSpyPromise(eFromPlayer, true);
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_CAUGHT_YOUR_SPY_GOOD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}




	case FROM_UI_DIPLO_EVENT_KILLED_MY_SPY_RESPONSE:
	{



		if(iArg1 == 1)
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_KILLED_MY_SPY_UNFORGIVEN);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}

		else if(iArg1 == 2)
		{
			SetPlayerForgaveForSpying(eFromPlayer, true);
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_KILLED_MY_SPY_FORGIVEN);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}

	case FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_SHARE_INTRIGUE:
	{
		PlayerTypes ePlottingPlayer = (PlayerTypes)iArg1;
		CvIntrigueType eIntrigueType = (CvIntrigueType)iArg2;
		CvAssert(CvPlayerAI::IsValid(ePlottingPlayer));
		if (CvPlayerAI::IsValid(ePlottingPlayer))
		{
			ChangeNumTimesIntrigueSharedBy(eFromPlayer, 1);
			GET_PLAYER(eFromPlayer).GetEspionage()->MarkRecentIntrigueAsShared(eMyPlayer, ePlottingPlayer, eIntrigueType);
			if (bActivePlayer)
			{
				if(GET_PLAYER(eFromPlayer).GetEspionage()->HasSharedIntrigueAboutMe(eMyPlayer))
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_37);
				}

				bool bUsingXP1Scenario3 = gDLL->IsModActivated(CIV5_XP1_SCENARIO3_MODID);
				if(bUsingXP1Scenario3)
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_45);
				}

				strText = GetDiploStringForMessage(DIPLO_MESSAGE_WARNED_ABOUT_INTRIGUE, NO_PLAYER, GET_PLAYER(ePlottingPlayer).getCivilizationAdjectiveKey());
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}
		break;
	}




	case FROM_UI_DIPLO_EVENT_STOP_CONVERSIONS:
	{



		if(iArg1 == 1)
		{
			SetPlayerIgnoredNoConvertPromise(eFromPlayer, true);
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_STOP_CONVERSIONS_BAD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}

		else if(iArg1 == 2)
		{
			SetPlayerMadeNoConvertPromise(eFromPlayer, true);
			int iAdjustmentToJustBelowThreshold = GC.getRELIGION_DIPLO_HIT_THRESHOLD() - GetNegativeReligiousConversionPoints(eFromPlayer) - 1;
			ChangeNegativeReligiousConversionPoints(eFromPlayer, iAdjustmentToJustBelowThreshold);
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_STOP_CONVERSIONS_GOOD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}





	case FROM_UI_DIPLO_EVENT_STOP_DIGGING:
		{



			if(iArg1 == 1)
			{
				SetPlayerIgnoredNoDiggingPromise(eFromPlayer, true);
				if(bActivePlayer)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_STOP_DIGGING_BAD);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
				}
			}

			else if(iArg1 == 2)
			{
				SetPlayerMadeNoDiggingPromise(eFromPlayer, true);

				ChangeNegativeArchaeologyPoints(eFromPlayer, -10);
				if(bActivePlayer)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_STOP_DIGGING_GOOD);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
				}
			}

			break;
		}




	default:
		CvAssert(false);
		break;
	}
}


bool CvDiplomacyAI::IsActHostileTowardsHuman(PlayerTypes eHuman)
{
	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(eHuman);
	MajorCivApproachTypes eVisibleApproach = GetMajorCivApproach(eHuman,                       true);

	bool bAtWar = IsAtWar(eHuman);
	bool bAtWarButWantsPeace = bAtWar &&
	                           GetTreatyWillingToOffer(eHuman) >= PEACE_TREATY_WHITE_PEACE && GetTreatyWillingToAccept(eHuman) >= PEACE_TREATY_WHITE_PEACE &&
	                           IsWillingToMakePeaceWithHuman(eHuman);

	if(eVisibleApproach == MAJOR_CIV_APPROACH_HOSTILE)
		return true;
	else if(bAtWar && !bAtWarButWantsPeace)
		return true;
	else if(eOpinion <= MAJOR_CIV_OPINION_ENEMY && eVisibleApproach != MAJOR_CIV_APPROACH_FRIENDLY)
		return true;

	return false;
}


const char* CvDiplomacyAI::GetGreetHumanMessage(LeaderheadAnimationTypes& eAnimation)
{
	PlayerTypes eHuman = GC.getGame().getActivePlayer();
	CvPlayer* pHuman = &GET_PLAYER(eHuman);
	TeamTypes eHumanTeam = pHuman->getTeam();
	CvTeam* pHumanTeam = &GET_TEAM(eHumanTeam);

	MajorCivApproachTypes eVisibleApproach = GetMajorCivApproach(eHuman,                       true);
	WarProjectionTypes eWarProjection = GetWarProjection(eHuman);
	DisputeLevelTypes eLandDispute = GetLandDisputeLevel(eHuman);
	AggressivePostureTypes eMilitaryPosture = GetMilitaryAggressivePosture(eHuman);
	AggressivePostureTypes eExpansionPosture = GetExpansionAggressivePosture(eHuman);
	AggressivePostureTypes ePlotBuyingPosture = GetPlotBuyingAggressivePosture(eHuman);
	StrengthTypes eMilitaryStrength = GetPlayerMilitaryStrengthComparedToUs(eHuman);
	StrengthTypes eEconomicStrength = GetPlayerEconomicStrengthComparedToUs(eHuman);
	int iNumPlayersAttacked = GetOtherPlayerNumMinorsAttacked(eHuman) + GetOtherPlayerNumMajorsAttacked(eHuman);
	int iNumPlayersKilled = GetOtherPlayerNumMinorsConquered(eHuman) + GetOtherPlayerNumMajorsConquered(eHuman);

	bool bAtWar = IsAtWar(eHuman);
	bool bAtWarButWantsPeace = bAtWar &&
	                           GetTreatyWillingToOffer(eHuman) >= PEACE_TREATY_WHITE_PEACE && GetTreatyWillingToAccept(eHuman) >= PEACE_TREATY_WHITE_PEACE &&
	                           IsWillingToMakePeaceWithHuman(eHuman);



	FStaticVector<DiploMessageTypes, NUM_DIPLO_MESSAGE_TYPES, true, c_eCiv5GameplayDLL, 0> veValidGreetings;



	bool bHostile = IsActHostileTowardsHuman(eHuman);





	if(bHostile)
		eAnimation = LEADERHEAD_ANIM_HATE_HELLO;
	else
		eAnimation = LEADERHEAD_ANIM_NEUTRAL_HELLO;





	int iTimesScreenOpened = GC.GetEngineUserInterface()->GetStartDiploRepeatCount();

	if(iTimesScreenOpened > 4)
		return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_REPEAT_TOO_MUCH);

	else if(iTimesScreenOpened > 2)
	{
		if(bHostile)
			return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_HOSTILE_REPEAT);
		else
			return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_REPEAT);
	}






	if(bAtWarButWantsPeace)
	{
		if (eWarProjection == WAR_PROJECTION_DESTRUCTION)
		{
			return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_DESTRUCTION_LOOMS);
		}
		else if (GetTreatyWillingToAccept(eHuman) >= PEACE_TREATY_CAPITULATION)
		{
			return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_WILL_ACCEPT_SURRENDER);
		}
		else
		{
			return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_AT_WAR_WANTS_PEACE);
		}
	}
	else if(bAtWar)
		return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_AT_WAR_HOSTILE);










	if(eVisibleApproach != MAJOR_CIV_APPROACH_FRIENDLY)
	{
		if(pHumanTeam->IsBrokenMilitaryPromise())
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_BROKEN_MILITARY_PROMISE);
	}


	if(IsDoFAccepted(eHuman))
		veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_WORKING_WITH);













	for(int iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
	{

		if(!GET_PLAYER((PlayerTypes) iThirdPartyLoop).isAlive())
			continue;

		if(GetCoopWarAcceptedState(eHuman, (PlayerTypes) iThirdPartyLoop) >= COOP_WAR_STATE_SOON)
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_COOP_WAR);
	}


	if(pHumanTeam->getAtWarCount(true) > 0)
	{
		if(bHostile)
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_AT_WAR);
		else
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HUMAN_AT_WAR);
	}


	if(eMilitaryPosture >= AGGRESSIVE_POSTURE_MEDIUM)
		if(bHostile)
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_AGGRESSIVE_MILITARY);
		else
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_AGGRESSIVE_MILITARY);


	if(eLandDispute >= DISPUTE_LEVEL_STRONG)
	{

		if(eExpansionPosture >= AGGRESSIVE_POSTURE_MEDIUM)
			if(bHostile)
				veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_AGGRESSIVE_EXPANSION);
			else
				veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_AGGRESSIVE_EXPANSION);


		if(ePlotBuyingPosture >= AGGRESSIVE_POSTURE_MEDIUM)
			if(bHostile)
				veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_AGGRESSIVE_PLOT_BUYING);
			else
				veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_AGGRESSIVE_PLOT_BUYING);
	}





	if(!bHostile)
	{
		if(eVisibleApproach == MAJOR_CIV_APPROACH_FRIENDLY)
		{
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_FRIENDLY_HELLO);


			if(eMilitaryStrength >= STRENGTH_STRONG)
				veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_FRIENDLY_STRONG_MILITARY);


			if(eEconomicStrength >= STRENGTH_STRONG)
				veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_FRIENDLY_STRONG_ECONOMY);
		}
		else
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_NEUTRAL_HELLO);
	}





	else
	{
		veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_HELLO);


		if(pHuman->getNumCities() < GetPlayer()->getNumCities())
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_FEW_CITIES);


		if(eMilitaryStrength <= STRENGTH_POOR)
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_SMALL_ARMY);


		if(iNumPlayersAttacked >= 2 || iNumPlayersKilled >= 1)
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_IS_WARMONGER);
	}


	if(veValidGreetings.size() > 0)
	{
		int iIndex = GC.getGame().getAsyncRandNum(veValidGreetings.size(), "Diplomacy AI: Picking random Greeting for AI to give to human.");

		DiploMessageTypes eGreetingType = veValidGreetings[iIndex];
		const char* strOptionalKey = "";
		bool bFoundPlayer = false;





























		if(eGreetingType == DIPLO_MESSAGE_GREETING_COOP_WAR)
		{
			int iThirdPartyLoop = 0;


			do
			{

				if(iThirdPartyLoop >= MAX_MAJOR_CIVS)
					iThirdPartyLoop = 0;

				if(GetCoopWarAcceptedState(eHuman, (PlayerTypes) iThirdPartyLoop) >= COOP_WAR_STATE_SOON)
				{

					if(GC.getGame().getAsyncRandNum(100, "Diplomacy AI: Random coop war greeting.") < 33)
					{
						strOptionalKey = GET_PLAYER((PlayerTypes) iThirdPartyLoop).getNameKey();
						bFoundPlayer = true;
					}
				}

				iThirdPartyLoop++;

			}
			while(!bFoundPlayer);
		}

		return GetDiploStringForMessage(eGreetingType, NO_PLAYER, strOptionalKey);
	}


	return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_NEUTRAL_HELLO);
}


const char* CvDiplomacyAI::GetInsultHumanMessage()
{
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

	StrengthTypes eMilitaryStrengthComparedToUs = GetPlayerMilitaryStrengthComparedToUs(ePlayer);

	FStaticVector<DiploMessageTypes, NUM_DIPLO_MESSAGE_TYPES, true, c_eCiv5GameplayDLL, 0> veValidInsults;


	if(eMilitaryStrengthComparedToUs < STRENGTH_AVERAGE)
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_MILITARY);


	if(kPlayer.getNumNukeUnits() == 0 && m_pPlayer->getNumNukeUnits() > 0)
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_NUKE);


	else if(GetOtherPlayerNumMinorsAttacked(ePlayer) > 0)
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_BULLY);


	else if(kPlayer.IsEmpireUnhappy())
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_UNHAPPINESS);


	else if(kPlayer.getNumCities() < m_pPlayer->getNumCities())
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_CITIES);


	else if(kPlayer.getTotalPopulation() < m_pPlayer->getTotalPopulation())
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_POPULATION);


	else if(kPlayer.GetJONSCultureEverGenerated() < m_pPlayer->GetJONSCultureEverGenerated())
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_CULTURE);


	if(veValidInsults.size() > 0)
	{
		int iIndex = GC.getGame().getAsyncRandNum(veValidInsults.size(), "Picking random insult for AI to give to human.");

		return GetDiploStringForMessage(veValidInsults[iIndex]);
	}


	return GetDiploStringForMessage(DIPLO_MESSAGE_INSULT_GENERIC);
}


const char* CvDiplomacyAI::GetAttackedByHumanMessage()
{
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();

	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);
	StrengthTypes eMilitaryStrengthComparedToUs = GetPlayerMilitaryStrengthComparedToUs(ePlayer);



	if(IsPlayerBrokenMilitaryPromise(ePlayer))
		return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_MILITARY_PROMISE_BROKEN);



	if(eOpinion == MAJOR_CIV_OPINION_ALLY ||
	        eOpinion == MAJOR_CIV_OPINION_FRIEND)
	{
		return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_SAD);
	}



	if(false)
		return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_BETRAYED);




	{
		if(IsGoingForWorldConquest())
		{

			if(eMilitaryStrengthComparedToUs <= STRENGTH_WEAK)
				return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_STRONG_EXCITED);


			else if(eMilitaryStrengthComparedToUs >= STRENGTH_POWERFUL)
				return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_WEAK_EXCITED);


			return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_EXCITED);
		}
	}




	if(eMilitaryStrengthComparedToUs <= STRENGTH_WEAK)
		return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_STRONG_HOSTILE);


	else if(eMilitaryStrengthComparedToUs >= STRENGTH_POWERFUL)
		return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_WEAK_HOSTILE);


	return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_HOSTILE);
}


const char* CvDiplomacyAI::GetWarMessage(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* strText = "OMG I HAVE NO DATA (DECLARING WAR) - Please send Jon this with your last 5 autosaves and what changelist # you're playing.";


	if(GC.getGame().getActivePlayer() == ePlayer)
	{
		MajorCivApproachTypes eApproachHidingTrueFeelings = GetMajorCivApproach(ePlayer,                       true);
		StrengthTypes eMilitaryStrengthComparedToUs = GetPlayerMilitaryStrengthComparedToUs(ePlayer);



		if(eApproachHidingTrueFeelings == MAJOR_CIV_APPROACH_FRIENDLY)
		{

			if(eMilitaryStrengthComparedToUs >= STRENGTH_STRONG)
				return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_WEAK_BETRAYAL);


			else
				return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_BETRAYAL);
		}



		if(GetLandDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
			return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_LAND);



		if(eMilitaryStrengthComparedToUs >= STRENGTH_STRONG)
			return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_DESPERATE);



		if(IsGoingForWorldConquest())
			return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_WORLD_CONQUEST);



		if(eMilitaryStrengthComparedToUs <= STRENGTH_POOR)
			return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_OPPORTUNITY);


		return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_GENERIC);
	}

	return strText;
}


const char* CvDiplomacyAI::GetEndDoFMessage(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* strText = "OMG I HAVE NO DATA (DECLARING WAR) - Please send Jon this with your last 5 autosaves and what changelist # you're playing.";


	if(GetWarmongerThreat(ePlayer) >= THREAT_MAJOR)
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_WITH_US_WARMONGER");

	else if(GetMinorCivDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_WITH_US_MINORS");

	else if(GetLandDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_WITH_US_LAND");

	else if(GetWonderDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_WITH_US_WONDERS");

	else if(GetVictoryDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_WITH_US_VICTORY");
	else
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_WITH_US_DEFAULT");

	return strText;
}


const char* CvDiplomacyAI::GetEndWorkAgainstSomeoneMessage(PlayerTypes ePlayer, const Localization::String& strAgainstPlayerKey)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* strText = "OMG I HAVE NO DATA (DECLARING WAR) - Please send Jon this with your last 5 autosaves and what changelist # you're playing.";

	if(ePlayer >=0 && ePlayer < MAX_MAJOR_CIVS)
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_AGAINST_SOMEONE_DEFAULT", strAgainstPlayerKey);

	return strText;
}


const char* CvDiplomacyAI::GetDiploTextFromTag(const char* strTag)
{
	return GetDiploTextFromTag(strTag, Localization::String::Empty, Localization::String::Empty);
}
const char* CvDiplomacyAI::GetDiploTextFromTag(const char* strTag, const Localization::String& strOptionalKey1)
{
	return GetDiploTextFromTag(strTag, strOptionalKey1, Localization::String::Empty);
}

const char* CvDiplomacyAI::GetDiploTextFromTag(const char* strTag, const Localization::String& strOptionalKey1, const Localization::String& strOptionalKey2)
{

	const char* strLeaderTag = GetPlayer()->getLeaderInfo().GetType();


	m_strDiploText = GC.getGame().GetDiploResponse(strLeaderTag, strTag, strOptionalKey1, strOptionalKey2);

	return m_strDiploText.toUTF8();
}










bool CvDiplomacyAI::DoTestCoopWarDesire(PlayerTypes ePlayer, PlayerTypes& eChosenTargetPlayer)
{
	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       false);


	if(eApproach == MAJOR_CIV_APPROACH_WAR)
		return false;

	else if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
		return false;

	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);


	if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
		return false;




	PlayerTypes eBestPlayer = NO_PLAYER;
	int iBestPlayerScore = 0;

	int iTempScore;


	PlayerTypes eTargetPlayerLoop;
	for(int iTargetPlayerLoop = 0; iTargetPlayerLoop < MAX_MAJOR_CIVS; iTargetPlayerLoop++)
	{
		eTargetPlayerLoop = (PlayerTypes) iTargetPlayerLoop;


		if(!IsPlayerValid(eTargetPlayerLoop))
			continue;


		if(eTargetPlayerLoop == ePlayer)
			continue;


		if(GetCoopWarAcceptedState(ePlayer, eTargetPlayerLoop) != NO_COOP_WAR_STATE)
			continue;


		if(GetCoopWarCounter(ePlayer, eTargetPlayerLoop) >= 0 && GetCoopWarCounter(ePlayer, eTargetPlayerLoop) < 30)
			continue;

		iTempScore = GetCoopWarScore(ePlayer, eTargetPlayerLoop,                    false);

		if(iTempScore > iBestPlayerScore)
		{
			iBestPlayerScore = iTempScore;
			eBestPlayer = eTargetPlayerLoop;
		}
	}


	if(eBestPlayer != NO_PLAYER)
	{
		eChosenTargetPlayer = eBestPlayer;
		return true;
	}

	return false;
}


CoopWarStates CvDiplomacyAI::GetWillingToAgreeToCoopWarState(PlayerTypes ePlayer, PlayerTypes eTargetPlayer)
{

	if(GetCoopWarScore(ePlayer, eTargetPlayer,                    true) >= 15)
	{
		if(GetPlayerTargetValue(eTargetPlayer) >= TARGET_VALUE_FAVORABLE)
			return COOP_WAR_STATE_ACCEPTED;
		else
			return COOP_WAR_STATE_SOON;
	}

	return COOP_WAR_STATE_REJECTED;
}


int CvDiplomacyAI::GetCoopWarScore(PlayerTypes ePlayer, PlayerTypes eTargetPlayer, bool bAskedByPlayer)
{
	MajorCivApproachTypes eApproachTowardsPlayer = GetMajorCivApproach(ePlayer,                       false);
	MajorCivOpinionTypes eOpinionTowardsPlayer = GetMajorCivOpinion(ePlayer);
	MajorCivApproachTypes eApproachTowardsTarget = GetMajorCivApproach(eTargetPlayer,                       false);
	MajorCivOpinionTypes eOpinionTowardsTarget = GetMajorCivOpinion(eTargetPlayer);


	TeamTypes eTargetTeam = GET_PLAYER(eTargetPlayer).getTeam();
	if(!GET_TEAM(GetPlayer()->getTeam()).canDeclareWar(eTargetTeam))
		return 0;
	if(!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canDeclareWar(eTargetTeam))
		return 0;


	if(!bAskedByPlayer)
	{
		if((eApproachTowardsTarget != MAJOR_CIV_APPROACH_WAR) && (eApproachTowardsTarget != MAJOR_CIV_APPROACH_DECEPTIVE))
			return 0;
	}


	if(eApproachTowardsPlayer == MAJOR_CIV_APPROACH_WAR)
		return 0;

	else if(eApproachTowardsPlayer == MAJOR_CIV_APPROACH_HOSTILE)
		return 0;


	if(eOpinionTowardsPlayer == MAJOR_CIV_OPINION_UNFORGIVABLE)
		return 0;





	if(!IsPlayerValid(eTargetPlayer))
		return 0;


	if(IsDoFAccepted(eTargetPlayer))
		return 0;

	int iWeight = 0;


	if(bAskedByPlayer)
	{
		if(eApproachTowardsPlayer == MAJOR_CIV_APPROACH_FRIENDLY || eApproachTowardsPlayer == MAJOR_CIV_APPROACH_DECEPTIVE)
			iWeight += 2;
		else if(eOpinionTowardsPlayer >= MAJOR_CIV_OPINION_FAVORABLE)
			iWeight += 2;
	}


	if(eApproachTowardsTarget == MAJOR_CIV_APPROACH_WAR)
		iWeight += 5;
	if(eApproachTowardsTarget == MAJOR_CIV_APPROACH_HOSTILE)
		iWeight += 2;

	if (IsGoingForWorldConquest())
	{
		iWeight += 3;
		if(eApproachTowardsTarget == MAJOR_CIV_APPROACH_DECEPTIVE)
			iWeight += 2;
	}
	else if (IsGoingForDiploVictory())
	{
		iWeight -= 2;
	}


	if(eOpinionTowardsTarget == MAJOR_CIV_OPINION_UNFORGIVABLE)
		iWeight += 12;
	else if(eOpinionTowardsTarget == MAJOR_CIV_OPINION_ENEMY)
		iWeight += 8;
	else if(eOpinionTowardsTarget == MAJOR_CIV_OPINION_COMPETITOR)
		iWeight += 4;
	else if(eOpinionTowardsTarget == MAJOR_CIV_OPINION_FAVORABLE)
		iWeight += -1;
	else if(eOpinionTowardsTarget == MAJOR_CIV_OPINION_FRIEND)
		iWeight += -5;
	else if(eOpinionTowardsTarget == MAJOR_CIV_OPINION_ALLY)
		iWeight += -10;


	int iCurrentTradeValue = GetPlayer()->GetTrade()->GetAllTradeValueFromPlayerTimes100(YIELD_GOLD, ePlayer) / 100;
	if(iCurrentTradeValue > 0)
	{
		iWeight -= 2;


		int iGPT = GetPlayer()->calculateGoldRate();
		if (iGPT >= 0 && (iGPT-iCurrentTradeValue < 0))
		{
			iWeight -= 5;
		}
	}


	if(IsPlayerRecklessExpander(eTargetPlayer))
	{
		iWeight += 4;
	}


	switch(GetWarmongerThreat(eTargetPlayer))
	{
	case THREAT_MINOR:
		iWeight += 1;
		break;
	case THREAT_MAJOR:
		iWeight += 3;
		break;
	case THREAT_SEVERE:
		iWeight += 5;
		break;
	case THREAT_CRITICAL:
		iWeight += 7;
		break;
	}


	if(iWeight > 0 && IsDoFAccepted(ePlayer))
		iWeight += 5;









	switch(GetPlayerTargetValue(eTargetPlayer))
	{
	case TARGET_VALUE_IMPOSSIBLE:
		iWeight *= 66;
		iWeight /= 100;
		break;
	case TARGET_VALUE_BAD:
		iWeight *= 75;
		iWeight /= 100;
		break;
	case TARGET_VALUE_AVERAGE:
		iWeight *= 100;
		iWeight /= 100;
		break;
	case TARGET_VALUE_FAVORABLE:
		iWeight *= 110;
		iWeight /= 100;
		break;
	case TARGET_VALUE_SOFT:
		iWeight *= 120;
		iWeight /= 100;
		break;
	}


	iWeight += GC.getGame().getJonRandNum(5, "Diplomacy AI: Rand for whether AI wants to enter a coop war.");


	if(iWeight >= 15)
		return iWeight;

	return 0;
}


bool CvDiplomacyAI::IsCoopWarMessageTooSoon(PlayerTypes ePlayer, PlayerTypes eTargetPlayer) const
{
	if(GetCoopWarCounter(ePlayer, eTargetPlayer) >= 0 &&
	        GetCoopWarCounter(ePlayer, eTargetPlayer) < 30)
		return true;

	return false;
}


bool CvDiplomacyAI::IsCoopWarEverAsked(PlayerTypes ePlayer, PlayerTypes eTargetPlayer) const
{

	if(GetCoopWarCounter(ePlayer, eTargetPlayer) == -1)
		return false;

	return true;
}

CoopWarStates CvDiplomacyAI::GetCoopWarAcceptedState(PlayerTypes ePlayer, PlayerTypes eTargetPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (CoopWarStates) m_ppaacCoopWarAcceptedState[ePlayer][eTargetPlayer];
}

void CvDiplomacyAI::SetCoopWarAcceptedState(PlayerTypes ePlayer, PlayerTypes eTargetPlayer, CoopWarStates eValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eValue != GetCoopWarAcceptedState(ePlayer, eTargetPlayer))
	{
		m_ppaacCoopWarAcceptedState[ePlayer][eTargetPlayer] = eValue;
	}
}

#ifdef AUI_WARNING_FIXES
int CvDiplomacyAI::GetCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eTargetPlayer) const
#else
short CvDiplomacyAI::GetCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eTargetPlayer) const
#endif
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_ppaaiCoopWarCounter[ePlayer][eTargetPlayer];
}

void CvDiplomacyAI::SetCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eTargetPlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaiCoopWarCounter[ePlayer][eTargetPlayer] = iValue;
}

void CvDiplomacyAI::ChangeCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eTargetPlayer, int iChange)
{
	SetCoopWarCounter(ePlayer, eTargetPlayer, GetCoopWarCounter(ePlayer, eTargetPlayer) + iChange);
}


bool CvDiplomacyAI::DoTestContinueCoopWarsDesire(PlayerTypes ePlayer, PlayerTypes& eTargetPlayer)
{

	PlayerTypes eTargetPlayerLoop;
	for(int iTargetPlayerLoop = 0; iTargetPlayerLoop < MAX_MAJOR_CIVS; iTargetPlayerLoop++)
	{
		eTargetPlayerLoop = (PlayerTypes) iTargetPlayerLoop;

		if(GetCoopWarAcceptedState(ePlayer, eTargetPlayerLoop) == COOP_WAR_STATE_ACCEPTED)
		{
			if(!IsContinueCoopWar(ePlayer, eTargetPlayerLoop))
			{
				eTargetPlayer = eTargetPlayerLoop;
				return false;
			}
		}
	}

	return true;
}


bool CvDiplomacyAI::IsContinueCoopWar(PlayerTypes ePlayer, PlayerTypes eTargetPlayer)
{
	CvAssertMsg(GetCoopWarAcceptedState(ePlayer, eTargetPlayer) == COOP_WAR_STATE_ACCEPTED, "Diplomacy AI: Testing whether we should continue working Target a player, but we aren't right now. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproachTowardsPlayer = GetMajorCivApproach(ePlayer,                       false);
	MajorCivOpinionTypes eOpinionTowardsPlayer = GetMajorCivOpinion(ePlayer);
	MajorCivApproachTypes eApproachTowardsTarget = GetMajorCivApproach(eTargetPlayer,                       false);
	MajorCivOpinionTypes eOpinionTowardsTarget = GetMajorCivOpinion(eTargetPlayer);


	if(eApproachTowardsPlayer == MAJOR_CIV_APPROACH_HOSTILE ||
	        GetMajorCivApproach(ePlayer,                       true) == MAJOR_CIV_APPROACH_HOSTILE)
		return false;


	if(eOpinionTowardsPlayer <= MAJOR_CIV_OPINION_ENEMY)
		return false;


	if(eApproachTowardsTarget == MAJOR_CIV_APPROACH_FRIENDLY)
		return false;

	if(eOpinionTowardsTarget >= MAJOR_CIV_OPINION_FRIEND)
		return false;














	return true;
}


CoopWarStates CvDiplomacyAI::GetGlobalCoopWarAcceptedState(PlayerTypes ePlayer)
{
	CoopWarStates eBestState = NO_COOP_WAR_STATE;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			if(GetCoopWarAcceptedState(eLoopPlayer, ePlayer) > eBestState)
				eBestState = GetCoopWarAcceptedState(eLoopPlayer, ePlayer);
		}
	}

	return eBestState;
}


int CvDiplomacyAI::GetGlobalCoopWarCounter(PlayerTypes ePlayer)
{
	int iBestCount = MAX_TURNS_SAFE_ESTIMATE;

	int iTempCount;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			iTempCount = GetCoopWarCounter(eLoopPlayer, ePlayer);


			if(iTempCount < 0)
				continue;

			if(iTempCount < iBestCount)
				iBestCount = iTempCount;
		}
	}

	return iBestCount;
}


bool CvDiplomacyAI::IsLockedIntoCoopWar(PlayerTypes ePlayer)
{
	CoopWarStates eCoopWarState = GetGlobalCoopWarAcceptedState(ePlayer);

	if(eCoopWarState == COOP_WAR_STATE_ACCEPTED || eCoopWarState == COOP_WAR_STATE_SOON)
	{
		if(GetGlobalCoopWarCounter(ePlayer) <=        GC.getCOOP_WAR_LOCKED_TURNS())
			return true;
	}

	return false;
}









void CvDiplomacyAI::DoDemandMade(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	SetDemandCounter(ePlayer, 0);



	int iNumTurns =        GC.getDEMAND_TURN_LIMIT_MIN();
	int iRand = GC.getGame().getJonRandNum(       GC.getDEMAND_TURN_LIMIT_RAND(), "Diplomacy AI: Number of turns before demand can be accepted.");
	iNumTurns += iRand;

	m_paiDemandTooSoonNumTurns[ePlayer] = iNumTurns;
}

bool CvDiplomacyAI::IsDemandTooSoon(PlayerTypes ePlayer) const
{
	int iDemandTooSoonNumTurns = GetDemandTooSoonNumTurns(ePlayer);


	if(iDemandTooSoonNumTurns == -1)
		return false;

	int iCounter = GetDemandCounter(ePlayer);

	if(iCounter > -1 && iCounter < iDemandTooSoonNumTurns)
		return true;

	return false;
}

#ifdef AUI_WARNING_FIXES
int CvDiplomacyAI::GetDemandTooSoonNumTurns(PlayerTypes ePlayer) const
#else
short CvDiplomacyAI::GetDemandTooSoonNumTurns(PlayerTypes ePlayer) const
#endif
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiDemandTooSoonNumTurns[ePlayer];
}

bool CvDiplomacyAI::IsDemandEverMade(PlayerTypes ePlayer) const
{
	if(GetDemandCounter(ePlayer) == -1)
		return false;

	return true;
}

#ifdef AUI_WARNING_FIXES
int CvDiplomacyAI::GetDemandCounter(PlayerTypes ePlayer) const
#else
short CvDiplomacyAI::GetDemandCounter(PlayerTypes ePlayer) const
#endif
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiDemandCounter[ePlayer];
}

void CvDiplomacyAI::SetDemandCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiDemandCounter[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeDemandCounter(PlayerTypes ePlayer, int iChange)
{
	SetDemandCounter(ePlayer, GetDemandCounter(ePlayer) + iChange);
}










bool CvDiplomacyAI::IsDontSettleAcceptable(PlayerTypes ePlayer) const
{
	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);


	if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
		return true;


	else if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
		return false;

	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);


	if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
		return false;


	int iExpansionFlavor =       GC.getDEFAULT_FLAVOR_VALUE();

	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_EXPANSION")
		{
			iExpansionFlavor = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
			break;
		}
	}






	int iThreshold = (      GC.getDONT_SETTLE_FLAVOR_BASE() - iExpansionFlavor) *       GC.getDONT_SETTLE_FLAVOR_MULTIPLIER();

	bool bFriendly = eApproach == MAJOR_CIV_APPROACH_FRIENDLY;


	if(bFriendly)
		iThreshold +=        GC.getDONT_SETTLE_FRIENDLY();

	int iMilitaryMod = 0;


	switch(GetPlayerMilitaryStrengthComparedToUs(ePlayer))
	{
	case STRENGTH_PATHETIC:
		if(!bFriendly)
			iMilitaryMod +=         GC.getDONT_SETTLE_STRENGTH_PATHETIC();
		break;
	case STRENGTH_WEAK:
		if(!bFriendly)
			iMilitaryMod +=         GC.getDONT_SETTLE_STRENGTH_WEAK();
		break;
	case STRENGTH_POOR:
		if(!bFriendly)
			iMilitaryMod +=         GC.getDONT_SETTLE_STRENGTH_POOR();
		break;
	case STRENGTH_AVERAGE:
		iMilitaryMod +=       GC.getDONT_SETTLE_STRENGTH_AVERAGE();
		break;
	case STRENGTH_STRONG:
		iMilitaryMod +=        GC.getDONT_SETTLE_STRENGTH_STRONG();
		break;
	case STRENGTH_POWERFUL:
		iMilitaryMod +=        GC.getDONT_SETTLE_STRENGTH_POWERFUL();
		break;
	case STRENGTH_IMMENSE:
		iMilitaryMod +=        GC.getDONT_SETTLE_STRENGTH_IMMENSE();
		break;
	}


	switch(GetMilitaryAggressivePosture(ePlayer))
	{
	case AGGRESSIVE_POSTURE_NONE:
		iMilitaryMod *=         GC.getDONT_SETTLE_MOD_MILITARY_POSTURE_NONE();
		break;
	case AGGRESSIVE_POSTURE_LOW:
		iMilitaryMod *=         GC.getDONT_SETTLE_MOD_MILITARY_POSTURE_LOW();
		break;
	case AGGRESSIVE_POSTURE_MEDIUM:
		iMilitaryMod *=         GC.getDONT_SETTLE_MOD_MILITARY_POSTURE_MEDIUM();
		break;
	case AGGRESSIVE_POSTURE_HIGH:
		iMilitaryMod *=         GC.getDONT_SETTLE_MOD_MILITARY_POSTURE_HIGH();
		break;
	case AGGRESSIVE_POSTURE_INCREDIBLE:
		iMilitaryMod *=         GC.getDONT_SETTLE_MOD_MILITARY_POSTURE_INCREDIBLE();
		break;
	}


	iMilitaryMod /= 100;

	iThreshold += iMilitaryMod;

	int iRand = GC.getGame().getJonRandNum(        GC.getDONT_SETTLE_RAND(), "Diplomacy AI: will AI agree to not settle near a player?");


	if(iRand < iThreshold)
		return true;

	return false;
}


bool CvDiplomacyAI::IsDontSettleMessageTooSoon(PlayerTypes ePlayer) const
{
	if(GetPlayerNoSettleRequestCounter(ePlayer) >= 0)
		return true;

	return false;
}


bool CvDiplomacyAI::IsPlayerNoSettleRequestEverAsked(PlayerTypes ePlayer) const
{

	if(GetPlayerNoSettleRequestCounter(ePlayer) == -1)
		return false;

	return true;
}

bool CvDiplomacyAI::IsPlayerNoSettleRequestRejected(PlayerTypes ePlayer) const
{

	if(GetPlayerNoSettleRequestCounter(ePlayer) < 0)
		return false;


	if(IsPlayerNoSettleRequestAccepted(ePlayer))
		return false;

	return true;
}

bool CvDiplomacyAI::IsPlayerNoSettleRequestAccepted(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerNoSettleRequestAccepted[ePlayer];
}

void CvDiplomacyAI::SetPlayerNoSettleRequestAccepted(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(bValue != IsPlayerNoSettleRequestAccepted(ePlayer))
	{
		m_pabPlayerNoSettleRequestAccepted[ePlayer] = bValue;

		PlayerTypes eID = GetPlayer()->GetID();

		int iPlotLoop;


		if(bValue)
		{
			int iLoopX, iLoopY;
			CvPlot* pNearbyPlot;
			int iRange = 6;


			CvCity* pLoopCity;
			int iCityLoop;
			for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
			{
#ifdef AUI_HEXSPACE_DX_LOOPS
				int iMaxDX;
				for (iLoopY = -iRange; iLoopY <= iRange; iLoopY++)
				{
					iMaxDX = iRange - MAX(0, iLoopY);
					for (iLoopX = -iRange - MIN(0, iLoopY); iLoopX <= iMaxDX; iLoopX++)
#else
				for(iLoopX = -(iRange); iLoopX <= iRange; iLoopX++)
				{
					for(iLoopY = -(iRange); iLoopY <= iRange; iLoopY++)
#endif
					{
						pNearbyPlot = plotXY(pLoopCity->getX(), pLoopCity->getY(), iLoopX, iLoopY);

						if(pNearbyPlot != NULL)
						{
#ifndef AUI_HEXSPACE_DX_LOOPS
#ifdef AUI_FIX_HEX_DISTANCE_INSTEAD_OF_PLOT_DISTANCE
							if (hexDistance(iLoopX, iLoopY) <= iRange)
#else
							if(plotDistance(pNearbyPlot->getX(), pNearbyPlot->getY(), pLoopCity->getX(), pLoopCity->getY()) <= iRange)
#endif
#endif
							{
								pNearbyPlot->SetNoSettling(eID, true);
							}
						}
					}
				}
			}
		}

		else
		{
			int iNumPlots = GC.getMap().numPlots();
			for(iPlotLoop = 0; iPlotLoop < iNumPlots; iPlotLoop++)
			{
				GC.getMap().plotByIndexUnchecked(iPlotLoop)->SetNoSettling(eID, false);
			}
		}
	}
}

#ifdef AUI_WARNING_FIXES
int CvDiplomacyAI::GetPlayerNoSettleRequestCounter(PlayerTypes ePlayer) const
#else
short CvDiplomacyAI::GetPlayerNoSettleRequestCounter(PlayerTypes ePlayer) const
#endif
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiPlayerNoSettleRequestCounter[ePlayer];
}

void CvDiplomacyAI::SetPlayerNoSettleRequestCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiPlayerNoSettleRequestCounter[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangePlayerNoSettleRequestCounter(PlayerTypes ePlayer, int iChange)
{
	SetPlayerNoSettleRequestCounter(ePlayer, GetPlayerNoSettleRequestCounter(ePlayer) + iChange);
}






bool CvDiplomacyAI::IsStopSpyingAcceptable(PlayerTypes ePlayer) const
{
	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);


	if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
		return true;


	else if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
		return false;

	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);


	if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
		return false;

	return true;
}


bool CvDiplomacyAI::IsStopSpyingMessageTooSoon(PlayerTypes ePlayer) const
{
	if(GetPlayerStopSpyingRequestCounter(ePlayer) >= 0)
	{
		return true;
	}

	return false;
}


bool CvDiplomacyAI::IsPlayerStopSpyingRequestEverAsked(PlayerTypes ePlayer) const
{

	if(GetPlayerStopSpyingRequestCounter(ePlayer) == -1)
	{
		return false;
	}

	return true;
}

bool CvDiplomacyAI::IsPlayerStopSpyingRequestRejected(PlayerTypes ePlayer) const
{

	if(GetPlayerStopSpyingRequestCounter(ePlayer) < 0)
		return false;


	if(IsPlayerStopSpyingRequestAccepted(ePlayer))
		return false;

	return true;
}

bool CvDiplomacyAI::IsPlayerStopSpyingRequestAccepted(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerStopSpyingRequestAccepted[ePlayer];
}

void CvDiplomacyAI::SetPlayerStopSpyingRequestAccepted(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	if(bValue != IsPlayerStopSpyingRequestAccepted(ePlayer))
	{
		m_pabPlayerStopSpyingRequestAccepted[ePlayer] = bValue;
	}
}

#ifdef AUI_WARNING_FIXES
int CvDiplomacyAI::GetPlayerStopSpyingRequestCounter(PlayerTypes ePlayer) const
#else
short CvDiplomacyAI::GetPlayerStopSpyingRequestCounter(PlayerTypes ePlayer) const
#endif
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiPlayerStopSpyingRequestCounter[ePlayer];
}

void CvDiplomacyAI::SetPlayerStopSpyingRequestCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	m_paiPlayerStopSpyingRequestCounter[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangePlayerStopSpyingRequestCounter(PlayerTypes ePlayer, int iChange)
{
	SetPlayerStopSpyingRequestCounter(ePlayer, GetPlayerStopSpyingRequestCounter(ePlayer) + iChange);
}









bool CvDiplomacyAI::IsDoFAcceptable(PlayerTypes ePlayer)
{

	if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()) || GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
	{
		return false;
	}


	if(IsTooEarlyForDoF(ePlayer))
		return false;

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       false);


	if(eApproach == MAJOR_CIV_APPROACH_WAR)
		return false;

	else if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
		return false;

	else if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
		return true;

	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);


	if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
		return false;

	else if(eOpinion == MAJOR_CIV_OPINION_ENEMY)
		return false;


	if(IsDenouncedPlayer(ePlayer))
		return false;
	if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDenouncedPlayer(GetPlayer()->GetID()))
		return false;





	int iWeight = 0;


	iWeight += GetDoFWillingness();


	if(eApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
		iWeight += 3;
	else if(eApproach == MAJOR_CIV_APPROACH_GUARDED)
		iWeight += -1;
	else if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
		iWeight += 3;


	if(eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
		iWeight += -3;
	else if(eOpinion == MAJOR_CIV_OPINION_FAVORABLE)
		iWeight += 2;
	else if(eOpinion == MAJOR_CIV_OPINION_FRIEND)
		iWeight += 5;
	else if(eOpinion == MAJOR_CIV_OPINION_ALLY)
		iWeight += 10;


	iWeight += GC.getGame().getJonRandNum(5, "Diplomacy AI: Rand for whether AI wants to work with player");

	if(iWeight >=        GC.getDOF_THRESHOLD())
		return true;

	return false;
}


bool CvDiplomacyAI::IsTooEarlyForDoF(PlayerTypes ePlayer)
{
	int iDoFBuffer =        GC.getDOF_TURN_BUFFER();

	if(GET_TEAM(GetTeam()).GetTurnsSinceMeetingTeam(GET_PLAYER(ePlayer).getTeam()) < iDoFBuffer)
		return true;

	return false;
}


bool CvDiplomacyAI::IsDoFMessageTooSoon(PlayerTypes ePlayer) const
{
	if(GetDoFCounter(ePlayer) >= 0 &&
	        GetDoFCounter(ePlayer) <        GC.getDOF_TURN_BUFFER())
		return true;

	return false;
}


bool CvDiplomacyAI::IsDoFEverAsked(PlayerTypes ePlayer) const
{

	if(GetDoFCounter(ePlayer) == -1)
		return false;

	return true;
}

bool CvDiplomacyAI::IsDoFRejected(PlayerTypes ePlayer) const
{

	if(GetDoFCounter(ePlayer) == -1)
		return false;


	if(IsDoFAccepted(ePlayer))
		return false;

	return true;
}

bool CvDiplomacyAI::IsDoFAccepted(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabDoFAccepted[ePlayer];
}

void CvDiplomacyAI::SetDoFAccepted(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(bValue != IsDoFAccepted(ePlayer))
	{
		m_pabDoFAccepted[ePlayer] = bValue;

		m_pPlayer->recomputeGreatPeopleModifiers();

		if(bValue){

			Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DOF");
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DOF_S");
			for(int iCurPlayer = 0; iCurPlayer < MAX_MAJOR_CIVS; ++iCurPlayer){
				PlayerTypes eCurPlayer = (PlayerTypes) iCurPlayer;
				CvPlayerAI& kCurPlayer = GET_PLAYER(eCurPlayer);
				CvNotifications* pNotifications = GET_PLAYER(eCurPlayer).GetNotifications();
				if(iCurPlayer != ePlayer && iCurPlayer !=GetPlayer()->GetID() && pNotifications){
					const char* strThisPlayerName;
					const char* strOtherPlayerName;

					CvTeam* pCurTeam = &GET_TEAM(kCurPlayer.getTeam());


					bool bHasMetThisTeam = pCurTeam->isHasMet(GetPlayer()->getTeam());
					if(bHasMetThisTeam)
						strThisPlayerName = GetPlayer()->getCivilizationShortDescriptionKey();
					else
						strThisPlayerName = "TXT_KEY_UNMET_PLAYER";

					bool bHasMetOtherTeam = pCurTeam->isHasMet(GET_PLAYER(ePlayer).getTeam());
					if(bHasMetOtherTeam)
						strOtherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey();
					else
						strOtherPlayerName = "TXT_KEY_UNMET_PLAYER";


					if(bHasMetThisTeam || bHasMetOtherTeam){
						Localization::String tempInfoStr = strText;
						tempInfoStr << strThisPlayerName << strOtherPlayerName;
						Localization::String tempSummaryStr = strSummary;
						tempSummaryStr << strThisPlayerName << strOtherPlayerName;
						pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, tempInfoStr.toUTF8(), tempSummaryStr.toUTF8(), -1, -1, GetPlayer()->GetID(), ePlayer);
					}
				}
			}
		}
	}
}



#ifdef AUI_WARNING_FIXES
int CvDiplomacyAI::GetDoFCounter(PlayerTypes ePlayer) const
#else
short CvDiplomacyAI::GetDoFCounter(PlayerTypes ePlayer) const
#endif
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiDoFCounter[ePlayer];
}

void CvDiplomacyAI::SetDoFCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiDoFCounter[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeDoFCounter(PlayerTypes ePlayer, int iChange)
{
	SetDoFCounter(ePlayer, GetDoFCounter(ePlayer) + iChange);
}

int CvDiplomacyAI::GetNumDoF()
{
	int iRtnValue = 0;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			if(IsDoFAccepted(eLoopPlayer))
			{
				iRtnValue++;
			}
		}
	}

	return iRtnValue;
}


bool CvDiplomacyAI::IsDenounceFriendAcceptable(PlayerTypes ePlayer)
{
	CvAssertMsg(IsDoFAccepted(ePlayer), "Diplomacy AI: Testing whether we should Denounce a Friend, but we aren't right now. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	MajorCivApproachTypes eTrueApproach = GetMajorCivApproach(ePlayer,                       false);
	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       true);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE || eTrueApproach == MAJOR_CIV_APPROACH_HOSTILE)
		return true;


	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);
	if(eOpinion <= MAJOR_CIV_OPINION_ENEMY)
		return true;
	if(eOpinion >= MAJOR_CIV_OPINION_FAVORABLE)
		return false;

	int iChance = 0;


	if(GetDoFCounter(ePlayer) > 30)
	{
		if(eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
		{



			iChance = 10 - GetLoyalty();
			iChance /= 2;
		}
	}


	if(IsGoingForWorldConquest())
	{
		if(GetPlayerMilitaryStrengthComparedToUs(ePlayer) <= STRENGTH_POOR)
		{
			if(GetPlayer()->GetProximityToPlayer(ePlayer) >= PLAYER_PROXIMITY_CLOSE)
				iChance += 5;
		}
	}

	int iRand = GC.getGame().getJonRandNum(100, "Diplomacy AI: Rand for whether AI wants to Denounce a Friend");
	if(iRand < iChance)
		return true;

	return false;
}


bool CvDiplomacyAI::IsPlayerDoFwithAnyFriend(PlayerTypes ePlayer) const
{
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsDoFAccepted(eLoopPlayer) && GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDoFAccepted(eLoopPlayer))
			return true;
	}

	return false;
}


bool CvDiplomacyAI::IsPlayerDoFwithAnyEnemy(PlayerTypes ePlayer) const
{
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsDenouncedPlayer(eLoopPlayer) && GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDoFAccepted(eLoopPlayer))
			return true;
	}

	return false;
}







void CvDiplomacyAI::DoDenouncePlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	TeamTypes eMyTeam = GetPlayer()->getTeam();
	TeamTypes eTheirTeam = GET_PLAYER(ePlayer).getTeam();

	SetDenouncedPlayer(ePlayer, true);

	SetDenouncedPlayerCounter(ePlayer, 0);


	GET_TEAM(eMyTeam).CloseEmbassyAtTeam(eTheirTeam);
	GET_TEAM(eTheirTeam).CloseEmbassyAtTeam(eMyTeam);


	if(IsDoFAccepted(ePlayer))
	{
		SetDoFAccepted(ePlayer, false);
		SetDoFCounter(ePlayer, -666);
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFAccepted(eMyPlayer, false);
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFCounter(eMyPlayer, -666);


		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetFriendDenouncedUs(eMyPlayer, true);
	}


	if(GetWarFaceWithPlayer(ePlayer) == WAR_FACE_FRIENDLY)
	{
		SetWarFaceWithPlayer(ePlayer, WAR_FACE_NEUTRAL);
	}


	PlayerTypes eThirdParty;
	for(int iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
	{
		eThirdParty = (PlayerTypes) iThirdPartyLoop;


		if(GetCoopWarAcceptedState(ePlayer, eThirdParty) >= COOP_WAR_STATE_SOON)
		{
			SetCoopWarAcceptedState(ePlayer, eThirdParty, NO_COOP_WAR_STATE);
			SetCoopWarCounter(ePlayer, eThirdParty, -666);
			GET_PLAYER(ePlayer).GetDiplomacyAI()->SetCoopWarAcceptedState(eMyPlayer, eThirdParty, NO_COOP_WAR_STATE);
			GET_PLAYER(ePlayer).GetDiplomacyAI()->SetCoopWarCounter(eMyPlayer, eThirdParty, -666);
		}
	}

	Localization::String someoneDenounceInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_DENOUNCE");
	Localization::String someoneDenounceSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DENOUNCE_S");
	Localization::String youDenounceInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_DENOUNCE");
	Localization::String youDenounceSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_DENOUNCE_S");
	Localization::String denounceYouInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_DENOUNCE_YOU");
	Localization::String denounceYouSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DENOUNCED_YOU_S");
	for(int iCurPlayer = 0; iCurPlayer < MAX_MAJOR_CIVS; ++iCurPlayer){
		PlayerTypes eCurPlayer = (PlayerTypes) iCurPlayer;
		CvPlayerAI& kCurPlayer = GET_PLAYER(eCurPlayer);
		CvNotifications* pNotifications = GET_PLAYER(eCurPlayer).GetNotifications();
		if(pNotifications)
		{
			const char* strThisPlayerName;
			const char* strOtherPlayerName;

			CvTeam* pNotifyTeam = &GET_TEAM(kCurPlayer.getTeam());

			bool bFromMe = GetPlayer()->getTeam() == kCurPlayer.getTeam();
			bool bAtMe = GET_PLAYER(ePlayer).getTeam() == kCurPlayer.getTeam();


			if(!bFromMe)
			{
				if(pNotifyTeam->isHasMet(GetPlayer()->getTeam()))
					strThisPlayerName = GetPlayer()->getCivilizationShortDescriptionKey();
				else
					strThisPlayerName = "TXT_KEY_UNMET_PLAYER";
			}

			if(!bAtMe)
			{
				if(pNotifyTeam->isHasMet(GET_PLAYER(ePlayer).getTeam()))
					strOtherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey();
				else
					strOtherPlayerName = "TXT_KEY_UNMET_PLAYER";
			}

			Localization::String strText;
			Localization::String strSummary;

			if(bFromMe)
			{
				strText = youDenounceInfo;
				strText << strOtherPlayerName;
				strSummary = youDenounceSummary;
				strSummary << strOtherPlayerName;
			}
			else if(bAtMe)
			{
				strText = denounceYouInfo;
				strText << strThisPlayerName;
				strSummary = denounceYouSummary;
				strSummary << strThisPlayerName;
			}
			else
			{
				bool bHasMetThisTeam = pNotifyTeam->isHasMet(GetPlayer()->getTeam());
				bool bHasMetOtherTeam = pNotifyTeam->isHasMet(GET_PLAYER(ePlayer).getTeam());


				if(bHasMetThisTeam || bHasMetOtherTeam)
				{
					strText = someoneDenounceInfo;
					strText << strThisPlayerName << strOtherPlayerName;
					strSummary = someoneDenounceSummary;
					strSummary << strThisPlayerName << strOtherPlayerName;
				}
				else
				{
					continue;
				}
			}

			pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, GetPlayer()->GetID(), ePlayer);
		}
	}
}


bool CvDiplomacyAI::IsDenounceAcceptable(PlayerTypes ePlayer, bool bBias)
{

	if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()) || GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
	{
		return false;
	}


	if(IsDenouncedPlayer(ePlayer))
		return false;


	if(IsDoFAccepted(ePlayer))
		return false;

	int iWeight = GetDenounceWeight(ePlayer, bBias);
	if(iWeight >= 18)
		return true;

	return false;
}

int CvDiplomacyAI::GetDenounceWeight(PlayerTypes ePlayer, bool bBias)
{
	int iWeight = 0;


	iWeight += GetDenounceWillingness();

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       false);


	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
		iWeight += 6;

	else if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
		iWeight += -10;

	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);


	if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
		iWeight += 10;

	else if(eOpinion == MAJOR_CIV_OPINION_ENEMY)
		iWeight += 5;

	else if(eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
		iWeight += 2;

	else if(eOpinion == MAJOR_CIV_OPINION_FAVORABLE)
		iWeight += -10;
	else if(eOpinion == MAJOR_CIV_OPINION_FRIEND)
		iWeight += -25;
	else if(eOpinion == MAJOR_CIV_OPINION_ALLY)
		iWeight += -50;


	if(IsAtWar(ePlayer))
		iWeight += 2;

	MajorCivApproachTypes eThirdPartyApproach;


	PlayerTypes eThirdParty;
	CvDiplomacyAI* pThirdPartyDiplo;
	for(int iThirdParty = 0; iThirdParty < MAX_MAJOR_CIVS; iThirdParty++)
	{
		eThirdParty = (PlayerTypes) iThirdParty;
		pThirdPartyDiplo = GET_PLAYER(eThirdParty).GetDiplomacyAI();


		if(!pThirdPartyDiplo->IsDenouncedPlayer(ePlayer))
			continue;


		if(GetMajorCivOpinion(eThirdParty) <= MAJOR_CIV_OPINION_COMPETITOR)
			continue;


		eThirdPartyApproach = GetMajorCivApproach(eThirdParty,                       false);
		if(eThirdPartyApproach == MAJOR_CIV_APPROACH_HOSTILE || eThirdPartyApproach == MAJOR_CIV_APPROACH_WAR)
			continue;


		if(GetPlayer()->GetProximityToPlayer(eThirdParty) == PLAYER_PROXIMITY_NEIGHBORS)
			iWeight += 1;


		if(GetPlayerMilitaryStrengthComparedToUs(eThirdParty) > STRENGTH_AVERAGE)
			iWeight += (GetPlayerMilitaryStrengthComparedToUs(eThirdParty) - STRENGTH_AVERAGE);


		if(IsDoFAccepted(eThirdParty))
			iWeight += 4;
	}


	iWeight += GC.getGame().getJonRandNum(5, "Diplomacy AI: Rand for whether AI wants to work with player");


	if(bBias)
		iWeight += 3;

	return iWeight;
}

bool CvDiplomacyAI::IsDenouncedPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabDenouncedPlayer[ePlayer];
}

void CvDiplomacyAI::SetDenouncedPlayer(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(bValue != IsDenouncedPlayer(ePlayer))
	{
		m_pabDenouncedPlayer[ePlayer] = bValue;
	}
}


bool CvDiplomacyAI::IsDenouncingPlayer(PlayerTypes ePlayer) const
{
	return (IsDenouncedPlayer(ePlayer) && GetDenouncedPlayerCounter(ePlayer) == 1);
}

#ifdef AUI_WARNING_FIXES
int CvDiplomacyAI::GetDenouncedPlayerCounter(PlayerTypes ePlayer) const
#else
short CvDiplomacyAI::GetDenouncedPlayerCounter(PlayerTypes ePlayer) const
#endif
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiDenouncedPlayerCounter[ePlayer];
}

void CvDiplomacyAI::SetDenouncedPlayerCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiDenouncedPlayerCounter[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeDenouncedPlayerCounter(PlayerTypes ePlayer, int iChange)
{
	SetDenouncedPlayerCounter(ePlayer, GetDenouncedPlayerCounter(ePlayer) + iChange);
}


bool CvDiplomacyAI::IsPlayerDenouncedFriend(PlayerTypes ePlayer) const
{
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsDoFAccepted(eLoopPlayer) && GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDenouncedPlayer(eLoopPlayer))
			return true;
	}

	return false;
}


bool CvDiplomacyAI::IsPlayerDenouncedEnemy(PlayerTypes ePlayer) const
{
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsDenouncedPlayer(eLoopPlayer) && GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDenouncedPlayer(eLoopPlayer))
			return true;
	}

	return false;
}









PlayerTypes CvDiplomacyAI::GetRequestFriendToDenounce(PlayerTypes ePlayer, bool& bRandFailed)
{
	bRandFailed = false;


	if(!IsDoFAccepted(ePlayer))
		return NO_PLAYER;

	PlayerTypes eTarget = NO_PLAYER;

	int iWeight;

	int iBestWeight = -1;

	CvDiplomacyAI* pTheirAI = GET_PLAYER(ePlayer).GetDiplomacyAI();
	CvTeam* pTheirTeam = &GET_TEAM(GET_PLAYER(ePlayer).getTeam());

	PlayerTypes eLoopPlayer;
	CvPlayer* pLoopPlayer;
	TeamTypes eLoopTeam;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		pLoopPlayer = &GET_PLAYER(eLoopPlayer);
		eLoopTeam = pLoopPlayer->getTeam();


		if(!pLoopPlayer->isAlive())
			continue;


		if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
			continue;


		if(!pTheirTeam->isHasMet(eLoopTeam))
			continue;


		if(pTheirTeam->isAtWar(eLoopTeam))
			continue;


		if(pTheirAI->IsDenouncedPlayer(eLoopPlayer))
			continue;


		if(!IsAtWar(eLoopPlayer) && !IsDenouncedPlayer(eLoopPlayer))
			continue;


		iWeight = GetDenounceWeight(ePlayer,           false);

		iWeight += GetNeediness();

		if(iWeight >= 23 && iWeight > iBestWeight)
		{
			iBestWeight = iWeight;

			eTarget = eLoopPlayer;
			bRandFailed = false;
		}
		else
			bRandFailed = true;
	}

	return eTarget;
}

bool CvDiplomacyAI::IsFriendDenounceRefusalUnacceptable(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer)
{
	ePlayer;
	eAgainstPlayer;


	int iDesire = GetDenounceWeight(eAgainstPlayer,           false);
	iDesire += GetNeediness();
	iDesire -= GetForgiveness();

	if(iDesire >= 23)
		return true;

	return false;
}










bool CvDiplomacyAI::IsUntrustworthyFriend()
{

	if(GetWeDeclaredWarOnFriendCount() > 0)
		return true;


	if(GetWeDenouncedFriendCount() >= 2)
		return true;

	return false;
}


int CvDiplomacyAI::GetNumFriendsDenouncedBy()
{
	int iNum = 0;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsFriendDenouncedUs(eLoopPlayer))
		{

			if(!GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsUntrustworthyFriend())
				iNum++;
		}
	}

	return iNum;
}

bool CvDiplomacyAI::IsFriendDenouncedUs(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabFriendDenouncedUs[ePlayer];
}

void CvDiplomacyAI::SetFriendDenouncedUs(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(bValue != IsFriendDenouncedUs(ePlayer))
	{
		m_pabFriendDenouncedUs[ePlayer] = bValue;
	}
}


int CvDiplomacyAI::GetWeDenouncedFriendCount()
{
	int iNum = 0;

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsFriendDenouncedUs(eMyPlayer))
			iNum++;
	}

	return iNum;
}

bool CvDiplomacyAI::IsFriendDeclaredWarOnUs(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabFriendDeclaredWarOnUs[ePlayer];
}

void CvDiplomacyAI::SetFriendDeclaredWarOnUs(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(bValue != IsFriendDeclaredWarOnUs(ePlayer))
	{
		m_pabFriendDeclaredWarOnUs[ePlayer] = bValue;
	}
}


int CvDiplomacyAI::GetWeDeclaredWarOnFriendCount()
{
	int iNum = 0;

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsFriendDeclaredWarOnUs(eMyPlayer))
			iNum++;
	}

	return iNum;
}



















































































































































































































































































































































void CvDiplomacyAI::DoTestPromises()
{
	AggressivePostureTypes eOldPosture;


	PlayerTypes eLoopPlayer;
	TeamTypes eLoopTeam;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

		if(IsPlayerValid(eLoopPlayer))
		{

			if(IsPlayerMadeMilitaryPromise(eLoopPlayer))
			{
				ChangePlayerMilitaryPromiseCounter(eLoopPlayer, 1);


				if(GetPlayerMilitaryPromiseCounter(eLoopPlayer) > 20)
				{
					SetPlayerMadeMilitaryPromise(eLoopPlayer, false);
					SetPlayerMilitaryPromiseCounter(eLoopPlayer, -1);
				}
			}


			if(IsPlayerMadeExpansionPromise(eLoopPlayer))
			{
				eOldPosture = GetPlayerExpansionPromiseData(eLoopPlayer);


				if(GetExpansionAggressivePosture(eLoopPlayer) > eOldPosture)
				{
					if (!IsPlayerBrokenExpansionPromise(eLoopPlayer))
					{
						SetBrokenExpansionPromiseValue(eLoopPlayer, GC.getOPINION_WEIGHT_EXPANSION_PROMISE_BROKE_MAX() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
						SetPlayerBrokenExpansionPromise(eLoopPlayer, true);
					}
				}
			}
			else if (IsPlayerMadeExpansionPromise(eLoopPlayer, GC.getGame().getGameTurn() - 1))
			{

				CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_EXPANSION_PROMISE_EXPIRED", GET_PLAYER(GetPlayer()->GetID()).getCivilizationShortDescriptionKey());
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_EXPANSION_PROMISE_EXPIRED_S");
					pNotifications->Add(NOTIFICATION_EXPANSION_PROMISE_EXPIRED, strBuffer, strSummary, -1, -1, GetPlayer()->GetID(), eLoopPlayer);
				}

			}


			if(IsPlayerMadeBorderPromise(eLoopPlayer))
			{
				eOldPosture = GetPlayerBorderPromiseData(eLoopPlayer);


				if(GetPlotBuyingAggressivePosture(eLoopPlayer) > eOldPosture)
				{
					if (!IsPlayerBrokenBorderPromise(eLoopPlayer))
					{
						SetBrokenBorderPromiseValue(eLoopPlayer, GC.getOPINION_WEIGHT_BORDER_PROMISE_BROKE_MAX() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
						SetPlayerBrokenBorderPromise(eLoopPlayer, true);
					}
				}
			}
			else if (IsPlayerMadeBorderPromise(eLoopPlayer, GC.getGame().getGameTurn() - 1))
			{

				CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BORDER_PROMISE_EXPIRED", GET_PLAYER(GetPlayer()->GetID()).getCivilizationShortDescriptionKey());
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_BORDER_PROMISE_EXPIRED_S");
					pNotifications->Add(NOTIFICATION_BORDER_PROMISE_EXPIRED, strBuffer, strSummary, -1, -1, GetPlayer()->GetID(), eLoopPlayer);
				}

			}
		}
	}
}

bool CvDiplomacyAI::IsPlayerMadeMilitaryPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerMadeMilitaryPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerMadeMilitaryPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerMadeMilitaryPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerBrokenMilitaryPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerBrokenMilitaryPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenMilitaryPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerBrokenMilitaryPromise[ePlayer] = bValue;

	if(bValue)
	{
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).SetBrokenMilitaryPromise(true);
	}
}

bool CvDiplomacyAI::IsPlayerIgnoredMilitaryPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerIgnoredMilitaryPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredMilitaryPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerIgnoredMilitaryPromise[ePlayer] = bValue;
}

#ifdef AUI_WARNING_FIXES
int CvDiplomacyAI::GetPlayerMilitaryPromiseCounter(PlayerTypes ePlayer)
#else
short CvDiplomacyAI::GetPlayerMilitaryPromiseCounter(PlayerTypes ePlayer)
#endif
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiPlayerMilitaryPromiseCounter[ePlayer];
}

void CvDiplomacyAI::SetPlayerMilitaryPromiseCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiPlayerMilitaryPromiseCounter[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangePlayerMilitaryPromiseCounter(PlayerTypes ePlayer, int iChange)
{
	SetPlayerMilitaryPromiseCounter(ePlayer, GetPlayerMilitaryPromiseCounter(ePlayer) + iChange);
}

bool CvDiplomacyAI::IsPlayerMadeExpansionPromise(PlayerTypes ePlayer, int iTestGameTurn)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if (m_paiPlayerMadeExpansionPromiseTurn[ePlayer] < 0)
	{
		return false;
	}

	int iGameTurn;
	if (iTestGameTurn != -1)
	{
		iGameTurn = iTestGameTurn;
	}
	else
	{
		iGameTurn = GC.getGame().getGameTurn();
	}

	int iTurnDifference = iGameTurn - m_paiPlayerMadeExpansionPromiseTurn[ePlayer];
	int iTimeOutTurns = (GC.getEXPANSION_PROMISE_TURNS_EFFECTIVE() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent()) / 100;
	if (iTurnDifference < iTimeOutTurns)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CvDiplomacyAI::SetPlayerMadeExpansionPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if (bValue)
	{
		m_paiPlayerMadeExpansionPromiseTurn[ePlayer] = GC.getGame().getGameTurn();
	}
	else
	{
		m_paiPlayerMadeExpansionPromiseTurn[ePlayer] = -1;
		SetPlayerExpansionPromiseData(ePlayer, NO_AGGRESSIVE_POSTURE_TYPE);
	}
}

bool CvDiplomacyAI::IsPlayerBrokenExpansionPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerBrokenExpansionPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenExpansionPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerBrokenExpansionPromise[ePlayer] = bValue;

	if(bValue)
	{
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).SetBrokenExpansionPromise(true);
	}
}

bool CvDiplomacyAI::IsPlayerIgnoredExpansionPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerIgnoredExpansionPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredExpansionPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerIgnoredExpansionPromise[ePlayer] = bValue;
}

AggressivePostureTypes CvDiplomacyAI::GetPlayerExpansionPromiseData(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (AggressivePostureTypes) m_paePlayerExpansionPromiseData[ePlayer];
}

void CvDiplomacyAI::SetPlayerExpansionPromiseData(PlayerTypes ePlayer, AggressivePostureTypes eValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerExpansionPromiseData[ePlayer] = eValue;
}

bool CvDiplomacyAI::EverMadeExpansionPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if (m_paiPlayerMadeExpansionPromiseTurn[ePlayer] < 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}


bool CvDiplomacyAI::IsPlayerMadeBorderPromise(PlayerTypes ePlayer, int iTestGameTurn)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if (m_paiPlayerMadeBorderPromiseTurn[ePlayer] < 0)
	{
		return false;
	}

	int iGameTurn;
	if (iTestGameTurn != -1)
	{
		iGameTurn = iTestGameTurn;
	}
	else
	{
		iGameTurn = GC.getGame().getGameTurn();
	}

	int iTurnDifference = iGameTurn - m_paiPlayerMadeBorderPromiseTurn[ePlayer];
	int iTimeOutTurns = (GC.getBORDER_PROMISE_TURNS_EFFECTIVE() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent()) / 100;
	if (iTurnDifference < iTimeOutTurns)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CvDiplomacyAI::SetPlayerMadeBorderPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if (bValue)
	{
		m_paiPlayerMadeBorderPromiseTurn[ePlayer] = GC.getGame().getGameTurn();
	}
	else
	{
		m_paiPlayerMadeBorderPromiseTurn[ePlayer] = GC.getGame().getGameTurn();
		SetPlayerBorderPromiseData(ePlayer, NO_AGGRESSIVE_POSTURE_TYPE);
	}
}

bool CvDiplomacyAI::IsPlayerBrokenBorderPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerBrokenBorderPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenBorderPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerBrokenBorderPromise[ePlayer] = bValue;

	if(bValue)
	{
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).SetBrokenBorderPromise(true);
	}
}

bool CvDiplomacyAI::IsPlayerIgnoredBorderPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerIgnoredBorderPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredBorderPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerIgnoredBorderPromise[ePlayer] = bValue;
}

AggressivePostureTypes CvDiplomacyAI::GetPlayerBorderPromiseData(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (AggressivePostureTypes) m_paePlayerBorderPromiseData[ePlayer];
}

void CvDiplomacyAI::SetPlayerBorderPromiseData(PlayerTypes ePlayer, AggressivePostureTypes eValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerBorderPromiseData[ePlayer] = eValue;
}

bool CvDiplomacyAI::EverMadeBorderPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if (m_paiPlayerMadeBorderPromiseTurn[ePlayer] < 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CvDiplomacyAI::IsPlayerMadeAttackCityStatePromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerMadeAttackCityStatePromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerMadeAttackCityStatePromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerMadeAttackCityStatePromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerBrokenAttackCityStatePromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerBrokenAttackCityStatePromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenAttackCityStatePromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerBrokenAttackCityStatePromise[ePlayer] = bValue;

	if(bValue)
	{
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).SetBrokenCityStatePromise(true);
	}
}

bool CvDiplomacyAI::IsPlayerIgnoredAttackCityStatePromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerIgnoredAttackCityStatePromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredAttackCityStatePromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerIgnoredAttackCityStatePromise[ePlayer] = bValue;
}


bool CvDiplomacyAI::IsPlayerMadeBullyCityStatePromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	return m_pabPlayerMadeBullyCityStatePromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerMadeBullyCityStatePromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	m_pabPlayerMadeBullyCityStatePromise[ePlayer] = bValue;
}


bool CvDiplomacyAI::IsPlayerBrokenBullyCityStatePromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	return m_pabPlayerBrokenBullyCityStatePromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenBullyCityStatePromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	m_pabPlayerBrokenBullyCityStatePromise[ePlayer] = bValue;
}


bool CvDiplomacyAI::IsPlayerIgnoredBullyCityStatePromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	return m_pabPlayerIgnoredBullyCityStatePromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredBullyCityStatePromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	m_pabPlayerIgnoredBullyCityStatePromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerMadeSpyPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerMadeSpyPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerMadeSpyPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerMadeSpyPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerBrokenSpyPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerBrokenSpyPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenSpyPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerBrokenSpyPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerIgnoredSpyPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerIgnoredSpyPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredSpyPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerIgnoredSpyPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerForgaveForSpying(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerForgaveForSpying[ePlayer];
}

void CvDiplomacyAI::SetPlayerForgaveForSpying(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerForgaveForSpying[ePlayer] = bValue;
}


bool CvDiplomacyAI::IsPlayerMadeNoConvertPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerMadeNoConvertPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerMadeNoConvertPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerMadeNoConvertPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerBrokenNoConvertPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerBrokenNoConvertPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenNoConvertPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerBrokenNoConvertPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerIgnoredNoConvertPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerIgnoredNoConvertPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredNoConvertPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerIgnoredNoConvertPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerAskedNotToConvert(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerAskedNotToConvert[ePlayer];
}

void CvDiplomacyAI::SetPlayerAskedNotToConvert(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerAskedNotToConvert[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerAgreeNotToConvert(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerAgreedNotToConvert[ePlayer];
}

void CvDiplomacyAI::SetPlayerAgreeNotToConvert(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerAgreedNotToConvert[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsStopSpreadingReligionAcceptable(PlayerTypes ePlayer)
{
	FlavorTypes eFlavor = (FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION");
	if (eFlavor == NO_FLAVOR)
	{
		return false;
	}
	int iFlavorReligionValue = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);

	if (iFlavorReligionValue <= 5)
	{
		return true;
	}
	else if (iFlavorReligionValue >= 8)
	{
		return false;
	}
	else
	{
		MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, true);
		switch (eApproach)
		{
		case MAJOR_CIV_APPROACH_DECEPTIVE:
		case MAJOR_CIV_APPROACH_AFRAID:
		case MAJOR_CIV_APPROACH_FRIENDLY:
		case MAJOR_CIV_APPROACH_NEUTRAL:
			return true;
			break;
		case MAJOR_CIV_APPROACH_WAR:
		case MAJOR_CIV_APPROACH_HOSTILE:
		case MAJOR_CIV_APPROACH_GUARDED:
			return false;
			break;
		}
	}

	return false;
}


bool CvDiplomacyAI::IsPlayerMadeNoDiggingPromise(PlayerTypes ePlayer)
{
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS)
	{
		return false;
	}
	return m_pabPlayerMadeNoDiggingPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerMadeNoDiggingPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerMadeNoDiggingPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerBrokenNoDiggingPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerBrokenNoDiggingPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenNoDiggingPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerBrokenNoDiggingPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerIgnoredNoDiggingPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerIgnoredNoDiggingPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredNoDiggingPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerIgnoredNoDiggingPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerAskedNotToDig(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerAskedNotToDig[ePlayer];
}

void CvDiplomacyAI::SetPlayerAskedNotToDig(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerAskedNotToDig[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerAgreeNotToDig(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerAgreedNotToDig[ePlayer];
}

void CvDiplomacyAI::SetPlayerAgreeNotToDig(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerAgreedNotToDig[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsStopDiggingAcceptable(PlayerTypes ePlayer)
{
	FlavorTypes eFlavor = (FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE");
	if (eFlavor == NO_FLAVOR)
	{
		return false;
	}
	int iFlavorCultureValue = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);

	if (iFlavorCultureValue < 5)
	{
		return true;
	}
	else if (iFlavorCultureValue >= 8 || IsGoingForCultureVictory())
	{
		return false;
	}
	else
	{
		MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, true);
		switch (eApproach)
		{
		case MAJOR_CIV_APPROACH_DECEPTIVE:
		case MAJOR_CIV_APPROACH_AFRAID:
		case MAJOR_CIV_APPROACH_FRIENDLY:
		case MAJOR_CIV_APPROACH_NEUTRAL:
			return true;
			break;
		case MAJOR_CIV_APPROACH_WAR:
		case MAJOR_CIV_APPROACH_HOSTILE:
		case MAJOR_CIV_APPROACH_GUARDED:
			return false;
			break;
		}
	}

	return false;
}


bool CvDiplomacyAI::IsPlayerBrokenCoopWarPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerBrokenCoopWarPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenCoopWarPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerBrokenCoopWarPromise[ePlayer] = bValue;
}


int CvDiplomacyAI::GetNumCiviliansReturnedToMe(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiNumCiviliansReturnedToMe[ePlayer];
}


void CvDiplomacyAI::ChangeNumCiviliansReturnedToMe(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNumCiviliansReturnedToMe[ePlayer] += iChange;
		CvAssertMsg(m_paiNumCiviliansReturnedToMe[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Civilians returned. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


		if(iChange > 0)
		{
			if(!GC.getGame().isNetworkMultiPlayer())
			{
				if(GC.getGame().getActivePlayer() == ePlayer)
				{
					GC.GetEngineUserInterface()->SetForceDiscussionModeQuitOnBack(true);
					const char* strText = GetDiploStringForMessage(DIPLO_MESSAGE_RETURNED_CIVILIAN);
					gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
				}
			}
		}
	}
}


int CvDiplomacyAI::GetNumLandmarksBuiltForMe(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiNumLandmarksBuiltForMe[ePlayer];
}


void CvDiplomacyAI::ChangeNumLandmarksBuiltForMe(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNumLandmarksBuiltForMe[ePlayer] += iChange;
		CvAssertMsg(m_paiNumLandmarksBuiltForMe[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Landmarks built. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	}
}


bool CvDiplomacyAI::WasResurrectedBy(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiResurrectedOnTurn[ePlayer] != -1;
}

bool CvDiplomacyAI::WasResurrectedThisTurnBy(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiResurrectedOnTurn[ePlayer] == GC.getGame().getGameTurn();
}

void CvDiplomacyAI::SetResurrectedBy(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if(bValue)
	{
		m_paiResurrectedOnTurn[ePlayer] = GC.getGame().getGameTurn();
	}
	else
	{
		m_paiResurrectedOnTurn[ePlayer] = -1;
	}
}


int CvDiplomacyAI::GetNumTimesCultureBombed(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiNumTimesCultureBombed[ePlayer];
}


void CvDiplomacyAI::ChangeNumTimesCultureBombed(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNumTimesCultureBombed[ePlayer] += iChange;
		CvAssertMsg(m_paiNumTimesCultureBombed[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Culture Bombs returned. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	}
}


int CvDiplomacyAI::GetNegativeArchaeologyPoints(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Ed this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Ed this with your last 5 autosaves and what changelist # you're playing.");

	int iRtnValue = m_paiNegativeArchaeologyPoints[ePlayer];
	return iRtnValue;
}

void CvDiplomacyAI::ChangeNegativeArchaeologyPoints(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Ed this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Ed this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNegativeArchaeologyPoints[ePlayer] += iChange;
		CvAssertMsg(m_paiNegativeArchaeologyPoints[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Digs in Other Player's Lands returned. Please send Ed this with your last 5 autosaves and what changelist # you're playing.");

		if(!GC.getGame().isGameMultiPlayer() && GET_PLAYER(ePlayer).isHuman() && ePlayer == GC.getGame().getActivePlayer())
		{
			if (m_paiNegativeArchaeologyPoints[ePlayer] >= 50 ||
#ifdef AUI_WARNING_FIXES
				(m_paiNegativeArchaeologyPoints[ePlayer] >= 40 && IsPlayerBrokenNoDiggingPromise(ePlayer)))
#else
				m_paiNegativeArchaeologyPoints[ePlayer] >= 40 && IsPlayerBrokenNoDiggingPromise(ePlayer))
#endif
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP2_34);
			}
		}
	}
}




int CvDiplomacyAI::GetNegativeReligiousConversionPoints(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiNegativeReligiousConversionPoints[ePlayer];
}


void CvDiplomacyAI::ChangeNegativeReligiousConversionPoints(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNegativeReligiousConversionPoints[ePlayer] += iChange;
		CvAssertMsg(m_paiNegativeReligiousConversionPoints[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Religious Conversions returned. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	}
}


int CvDiplomacyAI::GetNumTimesNuked(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiNumTimesNuked[ePlayer];
}


void CvDiplomacyAI::ChangeNumTimesNuked(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNumTimesNuked[ePlayer] += iChange;
		CvAssertMsg(m_paiNumTimesNuked[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Culture Bombs returned. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	}
}


int CvDiplomacyAI::GetNumTimesRobbedBy(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiNumTimesRobbedBy[ePlayer];
}


void CvDiplomacyAI::ChangeNumTimesRobbedBy(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNumTimesRobbedBy[ePlayer] += iChange;
		CvAssertMsg(m_paiNumTimesRobbedBy[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Robbed By returned. Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	}
}


int CvDiplomacyAI::GetNumTimesIntrigueSharedBy(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiNumTimesIntrigueSharedBy[ePlayer];
}


void CvDiplomacyAI::ChangeNumTimesIntrigueSharedBy(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNumTimesIntrigueSharedBy[ePlayer] += iChange;
		CvAssertMsg(m_paiNumTimesIntrigueSharedBy[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Robbed By returned. Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	}
}




int CvDiplomacyAI::GetLandDisputeLevelScore(PlayerTypes ePlayer)
{

	int iOpinionWeight = 0;
	switch(GetLandDisputeLevel(ePlayer))
	{
	case DISPUTE_LEVEL_FIERCE:
		iOpinionWeight +=        GC.getOPINION_WEIGHT_LAND_FIERCE();
		break;
	case DISPUTE_LEVEL_STRONG:
		iOpinionWeight +=        GC.getOPINION_WEIGHT_LAND_STRONG();
		break;
	case DISPUTE_LEVEL_WEAK:
		iOpinionWeight +=        GC.getOPINION_WEIGHT_LAND_WEAK();
		break;
	case DISPUTE_LEVEL_NONE:
		iOpinionWeight +=        GC.getOPINION_WEIGHT_LAND_NONE();
		break;
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetWonderDisputeLevelScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	switch(GetWonderDisputeLevel(ePlayer))
	{
	case DISPUTE_LEVEL_FIERCE:
		iOpinionWeight +=        GC.getOPINION_WEIGHT_WONDER_FIERCE();
		break;
	case DISPUTE_LEVEL_STRONG:
		iOpinionWeight +=        GC.getOPINION_WEIGHT_WONDER_STRONG();
		break;
	case DISPUTE_LEVEL_WEAK:
		iOpinionWeight +=        GC.getOPINION_WEIGHT_WONDER_WEAK();
		break;
	case DISPUTE_LEVEL_NONE:
		iOpinionWeight +=       GC.getOPINION_WEIGHT_WONDER_NONE();
		break;
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetMinorCivDisputeLevelScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	switch(GetMinorCivDisputeLevel(ePlayer))
	{
	case DISPUTE_LEVEL_FIERCE:
		iOpinionWeight +=        GC.getOPINION_WEIGHT_MINOR_CIV_FIERCE();
		break;
	case DISPUTE_LEVEL_STRONG:
		iOpinionWeight +=        GC.getOPINION_WEIGHT_MINOR_CIV_STRONG();
		break;
	case DISPUTE_LEVEL_WEAK:
		iOpinionWeight +=        GC.getOPINION_WEIGHT_MINOR_CIV_WEAK();
		break;
	case DISPUTE_LEVEL_NONE:
		iOpinionWeight +=       GC.getOPINION_WEIGHT_MINOR_CIV_NONE();
		break;
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetWarmongerThreatScore(PlayerTypes ePlayer)
{

	int iOpinionWeight = GetOtherPlayerWarmongerScore(ePlayer);
	if (iOpinionWeight < 5)
	{
		iOpinionWeight = 0;
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetCiviliansReturnedToMeScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetNumCiviliansReturnedToMe(ePlayer) > 0)
		iOpinionWeight += (        GC.getOPINION_WEIGHT_RETURNED_CIVILIAN() * GetNumCiviliansReturnedToMe(ePlayer));
	return iOpinionWeight;
}

int CvDiplomacyAI::GetLandmarksBuiltForMeScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	int iNumLandmarks = GetNumLandmarksBuiltForMe(ePlayer);
	if (iNumLandmarks> 0)
	{

		iOpinionWeight +=         GC.getOPINION_WEIGHT_BUILT_LANDMARK();


		if (iNumLandmarks > 1)
		{
			iOpinionWeight += ((GC.getOPINION_WEIGHT_BUILT_LANDMARK() / 3) * (iNumLandmarks - 1));
		}
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetResurrectedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(WasResurrectedBy(ePlayer))
		iOpinionWeight += (         GC.getOPINION_WEIGHT_RESURRECTED());
	return iOpinionWeight;
}

int CvDiplomacyAI::GetLiberatedCitiesScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	int iNumCitiesLiberated = GetNumCitiesLiberated(ePlayer);
	if (iNumCitiesLiberated >= 3)
	{
		iOpinionWeight =         GC.getOPINION_WEIGHT_LIBERATED_THREE_CITIES();
	}
	else if (iNumCitiesLiberated >= 2)
	{
		iOpinionWeight =         GC.getOPINION_WEIGHT_LIBERATED_TWO_CITIES();
	}
	else if (iNumCitiesLiberated >= 1)
	{
		iOpinionWeight =         GC.getOPINION_WEIGHT_LIBERATED_ONE_CITY();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetEmbassyScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GET_TEAM(GET_PLAYER(GetPlayer()->GetID()).getTeam()).HasEmbassyAtTeam(GET_PLAYER(ePlayer).getTeam()))
		iOpinionWeight += (       GC.getOPINION_WEIGHT_EMBASSY());
	return iOpinionWeight;
}

int CvDiplomacyAI::GetForgaveForSpyingScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsPlayerForgaveForSpying(ePlayer))
		iOpinionWeight +=         GC.getOPINION_WEIGHT_FORGAVE_FOR_SPYING();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetNoSetterRequestScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsPlayerNoSettleRequestEverAsked(ePlayer))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_ASKED_NO_SETTLE();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetStopSpyingRequestScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsPlayerStopSpyingRequestEverAsked(ePlayer))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_ASKED_STOP_SPYING();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDemandEverMadeScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsDemandEverMade(ePlayer))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_MADE_DEMAND_OF_US();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetTimesCultureBombedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetNumTimesCultureBombed(ePlayer) > 0)
		iOpinionWeight += (GetNumTimesCultureBombed(ePlayer) *        GC.getOPINION_WEIGHT_CULTURE_BOMBED());
	return iOpinionWeight;
}

int CvDiplomacyAI::GetReligiousConversionPointsScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetNegativeReligiousConversionPoints(ePlayer) > 0)
	{
		iOpinionWeight += (GetNegativeReligiousConversionPoints(ePlayer) * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiploEmphasisReligion() *       GC.getOPINION_WEIGHT_PER_NEGATIVE_CONVERSION());
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetHasAdoptedHisReligionScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(m_pPlayer->GetReligions()->HasOthersReligionInMostCities(ePlayer))
	{
		iOpinionWeight +=        GC.getOPINION_WEIGHT_ADOPTING_HIS_RELIGION() * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiploEmphasisReligion();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetHasAdoptedMyReligionScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(m_pPlayer->GetReligions()->HasOthersReligionInMostCities(ePlayer))
	{


	}
	else if(GET_PLAYER(ePlayer).GetReligions()->HasOthersReligionInMostCities(m_pPlayer->GetID()))
	{
		iOpinionWeight +=        GC.getOPINION_WEIGHT_ADOPTING_MY_RELIGION() * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiploEmphasisReligion();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetSameLatePoliciesScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	PolicyBranchTypes eMyBranch = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
	PolicyBranchTypes eTheirBranch = GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
	if(eMyBranch != NO_POLICY_BRANCH_TYPE && eTheirBranch != NO_POLICY_BRANCH_TYPE)
	{
		if(eMyBranch == eTheirBranch)
		{
			iOpinionWeight +=        GC.getOPINION_WEIGHT_SAME_LATE_POLICIES() * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiplpEmphasisLatePolicies();
		}
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDifferentLatePoliciesScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	PolicyBranchTypes eMyBranch = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
	PolicyBranchTypes eTheirBranch = GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
	if(eMyBranch != NO_POLICY_BRANCH_TYPE && eTheirBranch != NO_POLICY_BRANCH_TYPE)
	{
		if(eMyBranch != eTheirBranch)
		{
			iOpinionWeight +=       GC.getOPINION_WEIGHT_DIFFERENT_LATE_POLICIES() * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiplpEmphasisLatePolicies();
		}
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetTimesRobbedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetNumTimesRobbedBy(ePlayer) > 0)
		iOpinionWeight += (GetNumTimesRobbedBy(ePlayer) *        GC.getOPINION_WEIGHT_ROBBED_BY());
	return iOpinionWeight;
}

int CvDiplomacyAI::GetTimesIntrigueSharedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetNumTimesIntrigueSharedBy(ePlayer) > 0)
		iOpinionWeight += (GetNumTimesIntrigueSharedBy(ePlayer) *         GC.getOPINION_WEIGHT_INTRIGUE_SHARED_BY());
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenMilitaryPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerBrokenMilitaryPromise(ePlayer))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_BROKEN_MILITARY_PROMISE();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenMilitaryPromiseWithAnybodyScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerBrokenMilitaryPromise(ePlayer))
	{
	}
	else if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsBrokenMilitaryPromise())
	{
		iOpinionWeight +=        GC.getOPINION_WEIGHT_BROKEN_MILITARY_PROMISE_WORLD();
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredMilitaryPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerBrokenMilitaryPromise(ePlayer))
	{

	}
	else if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsBrokenMilitaryPromise())
	{

	}
	else if(IsPlayerIgnoredMilitaryPromise(ePlayer))
	{
		iOpinionWeight +=        GC.getOPINION_WEIGHT_IGNORED_MILITARY_PROMISE();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenExpansionPromiseScore (PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (GetBrokenExpansionPromiseValue(ePlayer) > 0)
	{
		int iWeightChange = GetBrokenExpansionPromiseValue(ePlayer) / (GC.getBROKEN_EXPANSION_PROMISE_PER_OPINION_WEIGHT() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
		if(iWeightChange >        GC.getOPINION_WEIGHT_EXPANSION_PROMISE_BROKE_MAX())
		{
			iWeightChange = GC.getOPINION_WEIGHT_EXPANSION_PROMISE_BROKE_MAX();
		}
		iOpinionWeight +=  iWeightChange;
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredExpansionPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (GetBrokenExpansionPromiseValue(ePlayer) > 0)
	{
	}
	else if (GetIgnoredExpansionPromiseValue(ePlayer) > 0)
	{
		int iWeightChange = GetIgnoredExpansionPromiseValue(ePlayer) / (GC.getIGNORED_EXPANSION_PROMISE_PER_OPINION_WEIGHT() *  GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
		if(iWeightChange >        GC.getOPINION_WEIGHT_EXPANSION_PROMISE_IGNORED_MAX())
		{
			iWeightChange = GC.getOPINION_WEIGHT_EXPANSION_PROMISE_IGNORED_MAX();
		}
		iOpinionWeight +=  iWeightChange;
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenBorderPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;




	if (GetBrokenBorderPromiseValue(ePlayer) > 0)
	{
		int iWeightChange = GetBrokenBorderPromiseValue(ePlayer) / (GC.getBROKEN_BORDER_PROMISE_PER_OPINION_WEIGHT() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
		if (iWeightChange >        GC.getOPINION_WEIGHT_BORDER_PROMISE_BROKE_MAX())
		{
			iWeightChange += GC.getOPINION_WEIGHT_BORDER_PROMISE_BROKE_MAX();
		}
		iOpinionWeight += iWeightChange;
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredBorderPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;









	if (GetBrokenBorderPromiseValue(ePlayer) > 0)
	{
	}
	else if (GetIgnoredBorderPromiseValue(ePlayer) > 0)
	{
		int iWeightChange = GetIgnoredBorderPromiseValue(ePlayer) / (GC.getIGNORED_BORDER_PROMISE_PER_OPINION_WEIGHT() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
		if (iWeightChange >        GC.getOPINION_WEIGHT_BORDER_PROMISE_IGNORED_MAX())
		{
			iWeightChange = GC.getOPINION_WEIGHT_BORDER_PROMISE_IGNORED_MAX();
		}
		iOpinionWeight += iWeightChange;
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenAttackCityStatePromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerBrokenAttackCityStatePromise(ePlayer))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenAttackCityStatePromiseWithAnybodyScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerBrokenAttackCityStatePromise(ePlayer))
	{
	}

	else if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsBrokenCityStatePromise())
	{
		iOpinionWeight +=       GC.getOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE_WORLD();
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredAttackCityStatePromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerBrokenAttackCityStatePromise(ePlayer))
	{

	}
	else if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsBrokenCityStatePromise())
	{

	}
	else if(IsPlayerIgnoredAttackCityStatePromise(ePlayer))
	{
		iOpinionWeight +=        GC.getOPINION_WEIGHT_IGNORED_CITY_STATE_PROMISE();
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenBullyCityStatePromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerBrokenBullyCityStatePromise(ePlayer))
		iOpinionWeight += 20;
	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredBullyCityStatePromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerBrokenBullyCityStatePromise(ePlayer))
	{

	}
	else if(IsPlayerIgnoredBullyCityStatePromise(ePlayer))
	{
		iOpinionWeight += 10;
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenNoConvertPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerBrokenNoConvertPromise(ePlayer))
	{
		iOpinionWeight +=       GC.getOPINION_WEIGHT_BROKEN_NO_CONVERT_PROMISE() * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiploEmphasisReligion();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredNoConvertPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerBrokenNoConvertPromise(ePlayer))
	{
	}
	else if(IsPlayerIgnoredNoConvertPromise(ePlayer))
	{
		iOpinionWeight +=       GC.getOPINION_WEIGHT_IGNORED_NO_CONVERT_PROMISE() * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiploEmphasisReligion();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenNoDiggingPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if (IsPlayerBrokenNoDiggingPromise(ePlayer))
	{
		iOpinionWeight +=        GC.getOPINION_WEIGHT_BROKEN_NO_DIG_PROMISE();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredNoDiggingPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;


	if (IsPlayerBrokenNoDiggingPromise(ePlayer))
	{
	}
	else if(IsPlayerIgnoredNoDiggingPromise(ePlayer))
	{
		iOpinionWeight +=        GC.getOPINION_WEIGHT_IGNORED_NO_DIG_PROMISE();
	}
	return iOpinionWeight;
}


int CvDiplomacyAI::GetBrokenSpyPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsPlayerBrokenSpyPromise(ePlayer))
	{
		iOpinionWeight += GC.getOPINION_WEIGHT_BROKEN_SPY_PROMISE();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredSpyPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsPlayerBrokenSpyPromise(ePlayer))
	{

	}
	else if (IsPlayerIgnoredSpyPromise(ePlayer))
	{
		iOpinionWeight += GC.getOPINION_WEIGHT_IGNORED_SPY_PROMISE();
	}
	return iOpinionWeight;
}


int CvDiplomacyAI::GetBrokenCoopWarPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerBrokenCoopWarPromise(ePlayer))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_BROKEN_COOP_WAR_PROMISE();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetAngryAboutProtectedMinorKilledScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsAngryAboutProtectedMinorKilled(ePlayer))
	{
		iOpinionWeight +=        GC.getOPINION_WEIGHT_KILLED_PROTECTED_MINOR();
		if (GetOtherPlayerNumProtectedMinorsKilled(ePlayer) > 1)
			iOpinionWeight +=        GC.getOPINION_WEIGHT_KILLED_MANY_PROTECTED_MINORS();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetAngryAboutProtectedMinorAttackedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsAngryAboutProtectedMinorAttacked(ePlayer))
	{
		if (GetTurnsSincePlayerAttackedProtectedMinor(ePlayer) <        GC.getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY_NUM_TURNS())
			iOpinionWeight +=        GC.getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY();
		else
			iOpinionWeight +=        GC.getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_WHILE_AGO();

		if (GetOtherPlayerNumProtectedMinorsAttacked(ePlayer) > 1)
			iOpinionWeight +=        GC.getOPINION_WEIGHT_ATTACKED_MANY_PROTECTED_MINORS();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetAngryAboutProtectedMinorBulliedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsAngryAboutProtectedMinorBullied(ePlayer))
	{
		if (GetTurnsSincePlayerBulliedProtectedMinor(ePlayer) <        GC.getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY_NUM_TURNS())
			iOpinionWeight +=        GC.getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY();
		else
			iOpinionWeight +=        GC.getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_WHILE_AGO();

		if (GetOtherPlayerNumProtectedMinorsBullied(ePlayer) > 1)					
			iOpinionWeight +=        GC.getOPINION_WEIGHT_BULLIED_MANY_PROTECTED_MINORS();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetAngryAboutSidedWithProtectedMinorScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsAngryAboutSidedWithTheirProtectedMinor(ePlayer))
	{
		iOpinionWeight +=       GC.getOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDOFAcceptedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsDoFAccepted(ePlayer))
		iOpinionWeight +=         GC.getOPINION_WEIGHT_DOF();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDOFWithAnyFriendScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerDoFwithAnyFriend(ePlayer))
		iOpinionWeight +=         GC.getOPINION_WEIGHT_DOF_WITH_FRIEND();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDOFWithAnyEnemyScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerDoFwithAnyEnemy(ePlayer))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_DOF_WITH_ENEMY();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetFriendDenouncementScore(PlayerTypes ePlayer)
{
	int iTraitorOpinion = 0;

	PlayerTypes eLoopPlayer;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(eLoopPlayer != GetPlayer()->GetID() && GET_PLAYER(ePlayer).GetDiplomacyAI()->IsFriendDenouncedUs(eLoopPlayer))
		{

			if(!GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsUntrustworthyFriend())
			{

				if(GetMajorCivOpinion(eLoopPlayer) > GetMajorCivOpinion(ePlayer))
				{
					iTraitorOpinion +=        GC.getOPINION_WEIGHT_DENOUNCED_BY_FRIEND_EACH();
				}


				else if(GetMajorCivOpinion(eLoopPlayer) < GetMajorCivOpinion(ePlayer))
				{
					iTraitorOpinion +=         GC.getOPINION_WEIGHT_DENOUNCED_BY_FRIEND_DONT_LIKE();
				}
			}
		}
	}

	return iTraitorOpinion;
}

int CvDiplomacyAI::GetWeDenouncedFriendScore(PlayerTypes ePlayer)
{
	int iTraitorOpinion = 0;
	int iDenouncedFriends = GET_PLAYER(ePlayer).GetDiplomacyAI()->GetWeDenouncedFriendCount();
	if(iDenouncedFriends > 0)
		iTraitorOpinion = iDenouncedFriends*        GC.getOPINION_WEIGHT_DENOUNCED_FRIEND_EACH();
	return iTraitorOpinion;
}

int CvDiplomacyAI::GetFriendDenouncedUsScore(PlayerTypes ePlayer)
{
	int iTraitorOpinion = 0;

	if(IsFriendDenouncedUs(ePlayer))
		iTraitorOpinion =        GC.getOPINION_WEIGHT_DENOUNCED_ME_FRIENDS();
	return iTraitorOpinion;
}

int CvDiplomacyAI::GetWeDeclaredWarOnFriendScore(PlayerTypes ePlayer)
{
	int iTraitorOpinion = 0;
	int iDeclaredWarOnFriendSum = 0;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		iDeclaredWarOnFriendSum += GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->GetDeclaredWarOnFriendValue(ePlayer);
	}

	iTraitorOpinion = iDeclaredWarOnFriendSum / (GC.getDECLARED_WAR_ON_FRIEND_PER_OPINION_WEIGHT() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
	return iTraitorOpinion;
}

int CvDiplomacyAI::GetFriendDeclaredWarOnUsScore(PlayerTypes ePlayer)
{
	int iTraitorOpinion = 0;

	if(IsFriendDeclaredWarOnUs(ePlayer))
		iTraitorOpinion =        GC.getOPINION_WEIGHT_WAR_ME_FRIENDS();
	return iTraitorOpinion;
}

int CvDiplomacyAI::GetDenouncedUsScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDenouncedPlayer(GetPlayer()->GetID()))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_DENOUNCED_ME();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDenouncedThemScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsDenouncedPlayer(ePlayer))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_DENOUNCED_THEM();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDenouncedFriendScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerDenouncedFriend(ePlayer))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_DENOUNCED_FRIEND();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDenouncedEnemyScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	if(IsPlayerDenouncedEnemy(ePlayer))
		iOpinionWeight +=         GC.getOPINION_WEIGHT_DENOUNCED_ENEMY();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetRecklessExpanderScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsPlayerRecklessExpander(ePlayer))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_RECKLESS_EXPANDER();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetRecentTradeScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetRecentTradeValue(ePlayer) > 0)
	{
		int iWeightChange = -1 * GetRecentTradeValue(ePlayer) / GC.getDEAL_VALUE_PER_OPINION_WEIGHT();
		if(iWeightChange <         GC.getOPINION_WEIGHT_TRADE_MAX())
		{
			iWeightChange = GC.getOPINION_WEIGHT_TRADE_MAX();
		}
		iOpinionWeight +=  iWeightChange;
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetCommonFoeScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetCommonFoeValue(ePlayer) > 0)
	{
		int iWeightChange = -1 * GetCommonFoeValue(ePlayer) / GC.getCOMMON_FOE_VALUE_PER_OPINION_WEIGHT();
		if(iWeightChange <         GC.getOPINION_WEIGHT_COMMON_FOE_MAX())
		{
			iWeightChange = GC.getOPINION_WEIGHT_COMMON_FOE_MAX();
		}
		iOpinionWeight +=  iWeightChange;
	}	
	return iOpinionWeight;
}

int CvDiplomacyAI::GetRecentAssistScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetRecentAssistValue(ePlayer) != 0)
	{
		int iWeightChange = GetRecentAssistValue(ePlayer) / GC.getDEAL_VALUE_PER_OPINION_WEIGHT();
		if (iWeightChange < -GC.getOPINION_WEIGHT_ASSIST_MAX())
		{
			iWeightChange = -GC.getOPINION_WEIGHT_ASSIST_MAX();
		}
		else if (iWeightChange > GC.getOPINION_WEIGHT_ASSIST_MAX())
		{
			iWeightChange = GC.getOPINION_WEIGHT_ASSIST_MAX();
		}
		iOpinionWeight +=  iWeightChange;
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetNukedByScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsNukedBy(ePlayer))
		iOpinionWeight +=         GC.getOPINION_WEIGHT_NUKED_MAX();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetCapitalCapturedByScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsCapitalCapturedBy(ePlayer))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_CAPTURED_CAPITAL();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetGaveAssistanceToScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsGaveAssistanceTo(ePlayer))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_GAVE_ASSISTANCE();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetPaidTributeToScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsHasPaidTributeTo(ePlayer))
		iOpinionWeight +=        GC.getOPINION_WEIGHT_PAID_TRIBUTE();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetLikedTheirProposalScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	int iTurnsSince = GetTurnsSinceWeLikedTheirProposal(ePlayer);
	if (iTurnsSince > -1 && iTurnsSince < GC.getOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_NUM_TURNS())
	{
		int iOpposingTurnsSince = GetTurnsSinceWeDislikedTheirProposal(ePlayer);
		if (iOpposingTurnsSince == -1 || iTurnsSince < iOpposingTurnsSince)
		{
			iOpinionWeight += GC.getOPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL();
		}
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDislikedTheirProposalScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	int iTurnsSince = GetTurnsSinceWeDislikedTheirProposal(ePlayer);
	if (iTurnsSince > -1 && iTurnsSince < GC.getOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_NUM_TURNS())
	{
		int iOpposingTurnsSince = GetTurnsSinceWeLikedTheirProposal(ePlayer);
		if (iOpposingTurnsSince == -1 || iTurnsSince < iOpposingTurnsSince)
		{
			iOpinionWeight += GC.getOPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL();
		}
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetSupportedMyProposalScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	int iTurnsSince = GetTurnsSinceTheySupportedOurProposal(ePlayer);
	if (iTurnsSince > -1 && iTurnsSince < GC.getOPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_NUM_TURNS())
	{
		int iOpposingTurnsSince = GetTurnsSinceTheyFoiledOurProposal(ePlayer);
		if (iOpposingTurnsSince == -1 || iTurnsSince < iOpposingTurnsSince)
		{
			iOpinionWeight += GC.getOPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL();
		}
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetFoiledMyProposalScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	int iTurnsSince = GetTurnsSinceTheyFoiledOurProposal(ePlayer);
	if (iTurnsSince > -1 && iTurnsSince < GC.getOPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_NUM_TURNS())
	{
		int iOpposingTurnsSince = GetTurnsSinceTheySupportedOurProposal(ePlayer);
		if (iOpposingTurnsSince == -1 || iTurnsSince < iOpposingTurnsSince)
		{
			iOpinionWeight += GC.getOPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL();
		}
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetSupportedMyHostingScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	int iTurnsSince = GetTurnsSinceTheySupportedOurHosting(ePlayer);
	if (iTurnsSince > -1 && iTurnsSince < GC.getOPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_NUM_TURNS())
	{
		iOpinionWeight += GC.getOPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING();	
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetScenarioModifier1(PlayerTypes ePlayer)
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(ePlayer);

		int iValue = 0;
		if (LuaSupport::CallAccumulator(pkScriptSystem, "GetScenarioDiploModifier1", args.get(), iValue))
		{
			return iValue;
		}
	}

	return 0;
}

int CvDiplomacyAI::GetScenarioModifier2(PlayerTypes ePlayer)
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(ePlayer);

		int iValue = 0;
		if (LuaSupport::CallAccumulator(pkScriptSystem, "GetScenarioDiploModifier2", args.get(), iValue))
		{
			return iValue;
		}
	}

	return 0;
}

int CvDiplomacyAI::GetScenarioModifier3(PlayerTypes ePlayer)
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(ePlayer);

		int iValue = 0;
		if (LuaSupport::CallAccumulator(pkScriptSystem, "GetScenarioDiploModifier3", args.get(), iValue))
		{
			return iValue;
		}
	}

	return 0;

}






bool CvDiplomacyAI::DoPossibleMinorLiberation(PlayerTypes eMinor, int iCityID)
{
	bool bLiberate = false;

	if(GetMinorCivApproach(eMinor) == MINOR_CIV_APPROACH_FRIENDLY || GetMinorCivApproach(eMinor) == MINOR_CIV_APPROACH_PROTECTIVE || IsGoingForDiploVictory())
	{
		bLiberate = true;
	}

	if(bLiberate)
	{
		GetPlayer()->DoLiberatePlayer(eMinor, iCityID);
	}

	return bLiberate;
}


int CvDiplomacyAI::GetNumOurEnemiesPlayerAtWarWith(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iAtWarCount = 0;

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	TeamTypes eLoopTeam;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			if(IsAtWar(eLoopPlayer))
			{
				eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

				if(GET_TEAM(eTeam).isAtWar(eLoopTeam))
				{
					iAtWarCount++;
				}
			}
		}
	}

	return iAtWarCount;
}


bool CvDiplomacyAI::IsGoingForWorldConquest()
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");

	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}


bool CvDiplomacyAI::IsGoingForDiploVictory()
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS");

	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}


bool CvDiplomacyAI::IsGoingForCultureVictory()
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");

	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}

bool CvDiplomacyAI::IsGoingForSpaceshipVictory()
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP");

	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}



bool CvDiplomacyAI::IsPlayerValid(PlayerTypes eOtherPlayer, bool bMyTeamIsValid)
{
	CvAssertMsg(eOtherPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	if(!GET_PLAYER(eOtherPlayer).isAlive())
	{
		return false;
	}


	if(GET_PLAYER(eOtherPlayer).getNumCities() == 0)
	{
		return false;
	}


	if(!GET_TEAM(GetTeam()).isHasMet(GET_PLAYER(eOtherPlayer).getTeam()))
	{
		return false;
	}


	if(!bMyTeamIsValid)
	{
		if(GetTeam() == GET_PLAYER(eOtherPlayer).getTeam())
		{
			return false;
		}
	}

	return true;
}


bool CvDiplomacyAI::HasSentAttackProtectedMinorTaunt(PlayerTypes ePlayer, PlayerTypes eMinor)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eMinor >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	return m_ppaabSentAttackMessageToMinorCivProtector[ePlayer][eMinor];
}


void CvDiplomacyAI::SetSentAttackProtectedMinorTaunt(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eMinor >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	m_ppaabSentAttackMessageToMinorCivProtector[ePlayer][eMinor] = bValue;
}


void CvDiplomacyAI::ResetSentAttackProtectedMinorTaunts(PlayerTypes eMinor)
{
	CvAssertMsg(eMinor >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	for(int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		SetSentAttackProtectedMinorTaunt((PlayerTypes)iI, eMinor, false);
	}
}


void CvDiplomacyAI::DoAddNewStatementToDiploLog(PlayerTypes ePlayer, DiploStatementTypes eNewDiploLogStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eNewDiploLogStatement >= 0, "DIPLOMACY_AI: Invalid Diplo Log Statement Type.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eNewDiploLogStatement < NUM_DIPLO_LOG_STATEMENT_TYPES, "DIPLOMACY_AI: Invalid Diplo Log Statement Type.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	for(int iI = MAX_DIPLO_LOG_STATEMENTS - 1; iI > 0; iI--)
	{

		if(m_ppaDiploStatementsLog[ePlayer][iI-1].m_eDiploLogStatement != NO_DIPLO_STATEMENT_TYPE)
		{
			m_ppaDiploStatementsLog[ePlayer][iI].m_eDiploLogStatement = m_ppaDiploStatementsLog[ePlayer][iI-1].m_eDiploLogStatement;
			m_ppaDiploStatementsLog[ePlayer][iI].m_iTurn = m_ppaDiploStatementsLog[ePlayer][iI-1].m_iTurn;
		}
	}

	m_ppaDiploStatementsLog[ePlayer][0].m_eDiploLogStatement = eNewDiploLogStatement;
	m_ppaDiploStatementsLog[ePlayer][0].m_iTurn = 0;
}


DiploStatementTypes CvDiplomacyAI::GetDiploLogStatementTypeForIndex(PlayerTypes ePlayer, int iIndex)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_ppaDiploStatementsLog[ePlayer][iIndex].m_eDiploLogStatement;
}


int CvDiplomacyAI::GetDiploLogStatementTurnForIndex(PlayerTypes ePlayer, int iIndex)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_ppaDiploStatementsLog[ePlayer][iIndex].m_iTurn;
}


void CvDiplomacyAI::SetDiploLogStatementTurnForIndex(PlayerTypes ePlayer, int iIndex, int iNewValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iNewValue >= 0, "DIPLOMACY_AI: Setting DiploLogStatementTurnForIndex to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaDiploStatementsLog[ePlayer][iIndex].m_iTurn = iNewValue;
}


void CvDiplomacyAI::ChangeDiploLogStatementTurnForIndex(PlayerTypes ePlayer, int iIndex, int iChange)
{
	SetDiploLogStatementTurnForIndex(ePlayer, iIndex, GetDiploLogStatementTurnForIndex(ePlayer, iIndex) + iChange);
}


int CvDiplomacyAI::GetNumTurnsSinceStatementSent(PlayerTypes ePlayer, DiploStatementTypes eDiploLogStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDiploLogStatement >= 0, "DIPLOMACY_AI: Invalid Diplo Log Statement Type.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDiploLogStatement < NUM_DIPLO_LOG_STATEMENT_TYPES, "DIPLOMACY_AI: Invalid Diplo Log Statement Type.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iMostRecentTurn = MAX_TURNS_SAFE_ESTIMATE;

	int iLoopTurnNum;
	DiploStatementTypes eLoopStatement;

	for(int iI = 0; iI < MAX_DIPLO_LOG_STATEMENTS; iI++)
	{
		eLoopStatement = GetDiploLogStatementTypeForIndex(ePlayer, iI);

		if(eLoopStatement != NO_DIPLO_STATEMENT_TYPE)
		{
			if(eLoopStatement == eDiploLogStatement)
			{
				iLoopTurnNum = GetDiploLogStatementTurnForIndex(ePlayer, iI);

				if(iMostRecentTurn == MAX_TURNS_SAFE_ESTIMATE || iLoopTurnNum > iMostRecentTurn)
				{
					iMostRecentTurn = iLoopTurnNum;


					break;
				}
			}
		}
	}

	return iMostRecentTurn;
}


void CvDiplomacyAI::LogMinorCivGiftGold(PlayerTypes eMinor, int iOldFriendship, int iGold, bool bSaving, bool bWantQuickBoost, PlayerTypes ePlayerTryingToPass)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());


		strBaseString += playerName;


		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;


		strTemp.Format("Gold Gift: %d", iGold);
		strOutBuf += ", " + strTemp;

		strTemp.Format("Friendship: %d to %d", iOldFriendship, GET_PLAYER(eMinor).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(GetPlayer()->GetID()));
		strOutBuf += ", " + strTemp;

		LogGrandStrategy(strOutBuf);


		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();


		if(bSaving)
			strOutBuf += ", (SAVING) ";


		if(bWantQuickBoost)
			strOutBuf += ", Boost! ";


		if(ePlayerTryingToPass != NO_PLAYER)
		{
			strOutBuf += ", Passing ";
			strOutBuf += GET_PLAYER(ePlayerTryingToPass).getCivilizationShortDescription();
		}

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


void CvDiplomacyAI::LogMinorCivBullyGold(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, int iGold, bool bSuccess, int iBullyMetricScore)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());


		strBaseString += playerName;


		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;


		strTemp.Format("Gold Bully: %d", iGold);
		strOutBuf += ", " + strTemp;

		strTemp.Format("Friendship: %d to %d", iOldFriendshipTimes100 / 100, iNewFriendshipTimes100 / 100);
		strOutBuf += ", " + strTemp;

		LogGrandStrategy(strOutBuf);


		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();


		if(bSuccess)
		{
			strOutBuf += ", ";
			strOutBuf += "Success!";
		}
		else
		{
			strOutBuf += ", ";
			strOutBuf += "Failure";
		}


		strTemp.Format("Bully Score: %d", iBullyMetricScore);
		strOutBuf += ", " + strTemp;

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

void CvDiplomacyAI::LogMinorCivBullyUnit(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, UnitTypes eUnit, bool bSuccess, int iBullyMetricScore)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());


		strBaseString += playerName;


		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;


		strTemp.Format("Unit Bully: %s", eUnit == (UnitTypes) GC.getInfoTypeForString("UNIT_WORKER") ? "Worker" : "Other unit");
		strOutBuf += ", " + strTemp;

		strTemp.Format("Friendship: %d to %d", iOldFriendshipTimes100 / 100, iNewFriendshipTimes100 / 100);
		strOutBuf += ", " + strTemp;

		LogGrandStrategy(strOutBuf);


		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();


		if(bSuccess)
		{
			strOutBuf += ", ";
			strOutBuf += "Success!";
		}
		else
		{
			strOutBuf += ", ";
			strOutBuf += "Failure";
		}


		strTemp.Format("Bully Score: %d", iBullyMetricScore);
		strOutBuf += ", " + strTemp;

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


void CvDiplomacyAI::LogMinorCivQuestReceived(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());


		strBaseString += playerName;


		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;


		strTemp = "RECEIVED QUEST";
		strOutBuf += ", " + strTemp;

		LogMinorCivQuestType(strOutBuf, eType);


		strTemp.Format("Friendship: %d to %d", iOldFriendshipTimes100 / 100, iNewFriendshipTimes100 / 100);
		strOutBuf += ", " + strTemp;


		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


void CvDiplomacyAI::LogMinorCivQuestFinished(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());


		strBaseString += playerName;


		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;


		strTemp = "FINISHED QUEST";
		strOutBuf += ", " + strTemp;

		LogMinorCivQuestType(strOutBuf, eType);


		strTemp.Format("Friendship: %d to %d", iOldFriendshipTimes100 / 100, iNewFriendshipTimes100 / 100);
		strOutBuf += ", " + strTemp;


		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


void CvDiplomacyAI::LogMinorCivQuestCancelled(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());


		strBaseString += playerName;


		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;


		strTemp = "cancelled";
		strOutBuf += ", " + strTemp;

		LogMinorCivQuestType(strOutBuf, eType);


		strTemp.Format("Friendship: %d to %d", iOldFriendshipTimes100 / 100, iNewFriendshipTimes100 / 100);
		strOutBuf += ", " + strTemp;


		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


void CvDiplomacyAI::LogMinorCivBuyout(PlayerTypes eMinor, int iGoldPaid, bool bSaving)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());


		strBaseString += playerName;


		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;

		strTemp.Format("BUYOUT: %d", iGoldPaid);
		strOutBuf += ", " + strTemp;


		strOutBuf += ", , ";


		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();


		if(bSaving)
			strOutBuf += ", (SAVING) ";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


void CvDiplomacyAI::DoAddNewDeclarationToLog(PublicDeclarationTypes eDeclaration, int iData1, int iData2, PlayerTypes eMustHaveMetPlayer, bool bActive)
{
	CvAssertMsg(eDeclaration >= 0, "DIPLOMACY_AI: Invalid PublicDeclarationType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDeclaration < NUM_PUBLIC_DECLARATION_TYPES, "DIPLOMACY_AI: Invalid PublicDeclarationType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");


	for(int iI = MAX_DIPLO_LOG_STATEMENTS - 1; iI > 0; iI--)
	{

		if(m_paDeclarationsLog[iI-1].m_eDeclaration != NO_PUBLIC_DECLARATION_TYPE)
		{
			m_paDeclarationsLog[iI].m_eDeclaration = m_paDeclarationsLog[iI-1].m_eDeclaration;
			m_paDeclarationsLog[iI].m_iData1 = m_paDeclarationsLog[iI-1].m_iData1;
			m_paDeclarationsLog[iI].m_iData2 = m_paDeclarationsLog[iI-1].m_iData2;
			m_paDeclarationsLog[iI].m_eMustHaveMetPlayer = m_paDeclarationsLog[iI-1].m_eMustHaveMetPlayer;
			m_paDeclarationsLog[iI].m_bActive = m_paDeclarationsLog[iI-1].m_bActive;
			m_paDeclarationsLog[iI].m_iTurn = m_paDeclarationsLog[iI-1].m_iTurn;
		}
	}

	m_paDeclarationsLog[0].m_eDeclaration = eDeclaration;
	m_paDeclarationsLog[0].m_iData1 = iData1;
	m_paDeclarationsLog[0].m_iData2 = iData2;
	m_paDeclarationsLog[0].m_eMustHaveMetPlayer = eMustHaveMetPlayer;
	m_paDeclarationsLog[0].m_bActive = bActive;
	m_paDeclarationsLog[0].m_iTurn = 0;
}


PublicDeclarationTypes CvDiplomacyAI::GetDeclarationLogTypeForIndex(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paDeclarationsLog[iIndex].m_eDeclaration;
}


int CvDiplomacyAI::GetDeclarationLogData1ForIndex(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paDeclarationsLog[iIndex].m_iData1;
}


int CvDiplomacyAI::GetDeclarationLogData2ForIndex(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paDeclarationsLog[iIndex].m_iData2;
}


PlayerTypes CvDiplomacyAI::GetDeclarationLogMustHaveMetPlayerForIndex(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paDeclarationsLog[iIndex].m_eMustHaveMetPlayer;
}


bool CvDiplomacyAI::IsDeclarationLogForIndexActive(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paDeclarationsLog[iIndex].m_bActive;
}


void CvDiplomacyAI::DoMakeDeclarationInactive(PublicDeclarationTypes eDeclaration, int iData1, int iData2)
{
	CvAssertMsg(eDeclaration >= 0, "DIPLOMACY_AI: Invalid PublicDeclarationType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDeclaration < NUM_DIPLO_LOG_STATEMENT_TYPES, "DIPLOMACY_AI: Invalid PublicDeclarationType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	for(int iLoop = 0; iLoop < MAX_DIPLO_LOG_STATEMENTS; iLoop++)
	{

		if(m_paDeclarationsLog[iLoop].m_eDeclaration == eDeclaration)
		{

			if(m_paDeclarationsLog[iLoop].m_iData1 == iData1)
			{

				if(m_paDeclarationsLog[iLoop].m_iData2 == iData2)
				{
					m_paDeclarationsLog[iLoop].m_bActive = false;
				}
			}
		}
	}
}


int CvDiplomacyAI::GetDeclarationLogTurnForIndex(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paDeclarationsLog[iIndex].m_iTurn;
}


void CvDiplomacyAI::SetDeclarationLogTurnForIndex(int iIndex, int iNewValue)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paDeclarationsLog[iIndex].m_iTurn = iNewValue;
}


void CvDiplomacyAI::ChangeDeclarationLogTurnForIndex(int iIndex, int iChange)
{
	SetDeclarationLogTurnForIndex(iIndex, GetDeclarationLogTurnForIndex(iIndex) + iChange);
}


CvDeal* CvDiplomacyAI::GetDealToRenew(int* piDealType)
{
	CvDeal* pTargetDeal = NULL;
	CvGameDeals* pGameDeals = GC.getGame().GetGameDeals();

	if (piDealType)
	{
		*piDealType = -1;
	}

	for(int iDealTypes = 0; iDealTypes < 2; iDealTypes++)
	{
		int iNumDeals = 0;
		if(iDealTypes == 0)
		{
			iNumDeals = pGameDeals->GetNumHistoricDeals(m_pPlayer->GetID());
		}
		else
		{
			iNumDeals = pGameDeals->GetNumCurrentDeals(m_pPlayer->GetID());
		}

		for(int iDeal = 0; iDeal < iNumDeals; iDeal++)
		{
			CvDeal* pCurrentDeal = NULL;
			if(iDealTypes == 0)
			{
				pCurrentDeal = pGameDeals->GetHistoricDeal(m_pPlayer->GetID(), iDeal);
			}
			else
			{
				pCurrentDeal = pGameDeals->GetCurrentDeal(m_pPlayer->GetID(), iDeal);
			}

			if (pCurrentDeal->m_bConsideringForRenewal)
			{
				CvAssertMsg(pTargetDeal == NULL, "pTargetDeal should be null");
				if (!pTargetDeal)
				{
					pTargetDeal = pCurrentDeal;
					if (piDealType)
					{
						*piDealType = iDealTypes;
					}
				}
			}
		}
	}

	return pTargetDeal;
}

void CvDiplomacyAI::ClearDealToRenew()
{
	CvGameDeals* pGameDeals = GC.getGame().GetGameDeals();
	int iNumMarkedToRenew = 0;

	for(int iDealTypes = 0; iDealTypes < 2; iDealTypes++)
	{
		int iNumDeals = 0;
		if(iDealTypes == 0)
		{
			iNumDeals = pGameDeals->GetNumHistoricDeals(m_pPlayer->GetID());
		}
		else
		{
			iNumDeals = pGameDeals->GetNumCurrentDeals(m_pPlayer->GetID());
		}

		for(int iDeal = 0; iDeal < iNumDeals; iDeal++)
		{
			CvDeal* pCurrentDeal = NULL;
			if(iDealTypes == 0)
			{
				pCurrentDeal = pGameDeals->GetHistoricDeal(m_pPlayer->GetID(), iDeal);
			}
			else
			{
				pCurrentDeal = pGameDeals->GetCurrentDeal(m_pPlayer->GetID(), iDeal);
			}

			if (pCurrentDeal->m_bConsideringForRenewal)
			{
				iNumMarkedToRenew++;
				CvAssertMsg(iNumMarkedToRenew <= 1, "iNumMarkedToRenew is greater than one. Too many deals are being renewed at the same time.");
				pCurrentDeal->m_bConsideringForRenewal = false;
			}
		}
	}
}



void CvDiplomacyAI::KilledPlayerCleanup (PlayerTypes eKilledPlayer)
{

	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;
		SetCoopWarAcceptedState(ePlayer, eKilledPlayer, NO_COOP_WAR_STATE);
		SetCoopWarCounter(ePlayer, eKilledPlayer, 0);
	}
}








bool CvDiplomacyAI::IsAtWar(PlayerTypes eOtherPlayer)
{
	CvAssertMsg(eOtherPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam());
}










void CvDiplomacyAI::LogPublicDeclaration(PublicDeclarationTypes eDeclaration, int iData1, PlayerTypes eForSpecificPlayer)
{
	if(GC.getLogging() && GC.getAILogging())
	{

		if(eForSpecificPlayer != NO_PLAYER)
			return;

		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());


		strBaseString += playerName;

		strOutBuf = strBaseString;


		if(eForSpecificPlayer != NO_PLAYER)
		{
			otherPlayerName = GET_PLAYER(eForSpecificPlayer).getCivilizationShortDescription();
			strTemp = "To " + otherPlayerName;
			strOutBuf += ", " + strTemp;
		}

		PlayerTypes eMinorCiv;

		bool bMinorMessage = false;

		switch(eDeclaration)
		{
		case PUBLIC_DECLARATION_PROTECT_MINOR:
			eMinorCiv = (PlayerTypes) iData1;
			otherPlayerName = GET_PLAYER(eMinorCiv).getCivilizationShortDescription();
			strTemp = "We're now protecting " + otherPlayerName + "!";
			bMinorMessage = true;
			break;
		case PUBLIC_DECLARATION_ABANDON_MINOR:
			eMinorCiv = (PlayerTypes) iData1;
			otherPlayerName = GET_PLAYER(eMinorCiv).getCivilizationShortDescription();
			strTemp = "We've abandoned " + otherPlayerName + "!";
			bMinorMessage = true;
			break;
		default:
			strTemp.Format("Unknown Declaration!!!");
			break;
		}

		strOutBuf += ", " + strTemp;

		pLog->Msg(strOutBuf);


		if(bMinorMessage)
		{

			if(GC.getPlayerAndCityAILogSplit())
				strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
			else
				strLogName = "DiplomacyAI_MinorCiv_Log.csv";

			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
		}

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


void CvDiplomacyAI::LogWarDeclaration(PlayerTypes ePlayer, int iTotalWarWeight)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + ", ***** WAR DECLARATION! *****, " + otherPlayerName;

		if(iTotalWarWeight >= 0)
		{
			CvString strWarWeight;
			strWarWeight.Format(", %d, ", iTotalWarWeight);
			strOutBuf += strWarWeight;
		}

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");



		{
			if(GC.getPlayerAndCityAILogSplit())
			{
				strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
			}
			else
			{
				strLogName = "DiplomacyAI_Messages_Log.csv";
			}
			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
		}


		m_pPlayer->GetMilitaryAI()->LogDeclarationOfWar(ePlayer);
	}
}



void CvDiplomacyAI::LogPeaceMade(PlayerTypes ePlayer)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + ", ***** PEACE MADE! *****, " + otherPlayerName;

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");



		{
			if(GC.getPlayerAndCityAILogSplit())
			{
				strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
			}
			else
			{
				strLogName = "DiplomacyAI_Messages_Log.csv";
			}
			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
		}


		m_pPlayer->GetMilitaryAI()->LogPeace(ePlayer);
	}
}



void CvDiplomacyAI::LogDoF(PlayerTypes ePlayer)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + ",***** NOW FRIENDS " + otherPlayerName + "! *****";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}



void CvDiplomacyAI::LogDenounce(PlayerTypes ePlayer, bool bBackstab, bool bRefusal)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvString otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();


		if(bBackstab)
			strOutBuf = strBaseString + ",***** BACKSTABBED " + otherPlayerName + "! *****";
		else if(bRefusal)
			strOutBuf = strBaseString + ",***** REFUSED TO FORGIVE " + otherPlayerName + "! *****";
		else
			strOutBuf = strBaseString + ",***** DENOUNCED " + otherPlayerName + "! *****";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


void CvDiplomacyAI::LogFriendRequestDenounce(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, bool bAgreed)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvString otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		CvString againstPlayerName = GET_PLAYER(eAgainstPlayer).getCivilizationShortDescription();


		if(bAgreed)
			strOutBuf = strBaseString + ",***** CONVINCED " + otherPlayerName + " TO DENOUNCE " + againstPlayerName + "! *****";
		else
			strOutBuf = strBaseString + ", ASKED " + otherPlayerName + " TO DENOUNCE " + againstPlayerName + "!";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}



void CvDiplomacyAI::LogCoopWar(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, CoopWarStates eAcceptedState)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvString otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		CvString againstPlayerName = GET_PLAYER(eAgainstPlayer).getCivilizationShortDescription();


		if(eAcceptedState == COOP_WAR_STATE_ACCEPTED)
			strOutBuf = strBaseString + ",***** NOW coop war with " + otherPlayerName + " against " + againstPlayerName + "! *****";

		else if(eAcceptedState == COOP_WAR_STATE_SOON)
			strOutBuf = strBaseString + ",***** SOON coop war with " + otherPlayerName + " against " + againstPlayerName + "! *****";

		else if(eAcceptedState == COOP_WAR_STATE_REJECTED)
			strOutBuf = strBaseString + ", wanted coop war with " + otherPlayerName + " against " + againstPlayerName + "!";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


void CvDiplomacyAI::LogWantRA(PlayerTypes ePlayer)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + otherPlayerName + ", Wants Research Agreement!";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}



void CvDiplomacyAI::LogOpinionUpdate(PlayerTypes ePlayer, std::vector<int>& viOpinionValues)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Approach_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Approach_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strTemp.Format("OPINION");
		strOutBuf += ", " + strTemp;

		MajorCivOpinionTypes eMajorCivOpinion;



		if(!GET_PLAYER(ePlayer).isMinorCiv())
		{

			switch(GetMajorCivOpinion(ePlayer))
			{
			case MAJOR_CIV_OPINION_ALLY:
				strTemp.Format("** ALLY **");
				break;
			case MAJOR_CIV_OPINION_FRIEND:
				strTemp.Format("FRIEND");
				break;
			case MAJOR_CIV_OPINION_FAVORABLE:
				strTemp.Format("Favorable");
				break;
			case MAJOR_CIV_OPINION_NEUTRAL:
				strTemp.Format("N");
				break;
			case MAJOR_CIV_OPINION_COMPETITOR:
				strTemp.Format("Competitor");
				break;
			case MAJOR_CIV_OPINION_ENEMY:
				strTemp.Format("ENEMY");
				break;
			case MAJOR_CIV_OPINION_UNFORGIVABLE:
				strTemp.Format("**UNFORGIVABLE**");
				break;
			default:
				strTemp.Format("XXX");
				break;
			}
			strOutBuf += ", " + strTemp;

			for(int iMajorCivOpinionLoop = 0; iMajorCivOpinionLoop < NUM_MAJOR_CIV_OPINION_TYPES; iMajorCivOpinionLoop++)
			{
				eMajorCivOpinion = (MajorCivOpinionTypes) iMajorCivOpinionLoop;

				switch(eMajorCivOpinion)
				{
				case MAJOR_CIV_OPINION_ALLY:
					strTemp.Format("Ally");
					break;
				case MAJOR_CIV_OPINION_FRIEND:
					strTemp.Format("Friend");
					break;
				case MAJOR_CIV_OPINION_NEUTRAL:
					strTemp.Format("Neutral");
					break;
				case MAJOR_CIV_OPINION_COMPETITOR:
					strTemp.Format("Competitor");
					break;
				case MAJOR_CIV_OPINION_ENEMY:
					strTemp.Format("Enemy");
					break;
				default:
					strTemp.Format("XXX");
					break;
				}
				strOutBuf += ", " + strTemp;

				strTemp.Format("%d", viOpinionValues[eMajorCivOpinion]);
				strOutBuf += ", " + strTemp;
			}

			strOutBuf = strBaseString + strOutBuf;
			pLog->Msg(strOutBuf);
		}
	}
}


void CvDiplomacyAI::LogMajorCivApproachUpdate(PlayerTypes ePlayer, const int* aiApproachValues, MajorCivApproachTypes eNewMajorCivApproach, MajorCivApproachTypes eOldApproach, WarFaceTypes eNewWarFace)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Approach_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Approach_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		MajorCivApproachTypes eMajorCivApproach;


		if(!GET_PLAYER(ePlayer).isMinorCiv())
		{
			LogMajorCivApproach(strOutBuf, eNewMajorCivApproach, eNewWarFace);

			if(eNewMajorCivApproach != eOldApproach)
			{
				LogMajorCivApproach(strOutBuf, eOldApproach, eNewWarFace);
			}
			else
			{
				strTemp.Format("---");
				strOutBuf += ", " + strTemp;
			}

			LogOpinion(strOutBuf, ePlayer);

			LogWarmongerThreat(strOutBuf, ePlayer);

			LogMilitaryThreat(strOutBuf, ePlayer);

			LogTargetValue(strOutBuf, ePlayer);

			LogWarGoal(strOutBuf, ePlayer);

			LogMilitaryAggressivePosture(strOutBuf, ePlayer);

			LogProximity(strOutBuf, ePlayer);

			LogLandDispute(strOutBuf, ePlayer);

			LogVictoryDispute(strOutBuf, ePlayer);

			LogWonderDispute(strOutBuf, ePlayer);

			LogMinorCivDispute(strOutBuf, ePlayer);

			strTemp.Format("---");
			strOutBuf += ", " + strTemp;


			for(int iMajorCivApproachLoop = 0; iMajorCivApproachLoop < NUM_MAJOR_CIV_APPROACHES; iMajorCivApproachLoop++)
			{
				eMajorCivApproach = (MajorCivApproachTypes) iMajorCivApproachLoop;

				switch(eMajorCivApproach)
				{
				case MAJOR_CIV_APPROACH_WAR:
					strTemp.Format("War");
					break;
				case MAJOR_CIV_APPROACH_HOSTILE:
					strTemp.Format("Hostile");
					break;
				case MAJOR_CIV_APPROACH_DECEPTIVE:
					strTemp.Format("Deceptive");
					break;
				case MAJOR_CIV_APPROACH_GUARDED:
					strTemp.Format("Guarded");
					break;
				case MAJOR_CIV_APPROACH_AFRAID:
					strTemp.Format("Afraid");
					break;
				case MAJOR_CIV_APPROACH_FRIENDLY:
					strTemp.Format("Friendly");
					break;
				case MAJOR_CIV_APPROACH_NEUTRAL:
					strTemp.Format("Neutral");
					break;
				default:
					strTemp.Format("XXX");
					break;
				}
				strOutBuf += ", " + strTemp;

				strTemp.Format("%d", aiApproachValues[eMajorCivApproach] / 100);
				strOutBuf += ", " + strTemp;
			}

			strOutBuf = strBaseString + strOutBuf;
			pLog->Msg(strOutBuf);
		}
	}
}



void CvDiplomacyAI::LogMinorCivApproachUpdate(PlayerTypes ePlayer, const int* aiApproachValues, MinorCivApproachTypes eNewMinorCivApproach, MinorCivApproachTypes eOldApproach)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Approach_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Approach_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());


		strBaseString += playerName;

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strBaseString += ", --- " + otherPlayerName;

		MinorCivApproachTypes eMinorCivApproach;


		if(GET_PLAYER(ePlayer).isMinorCiv())
		{
			LogMinorCivApproach(strOutBuf, eNewMinorCivApproach);

			if(eNewMinorCivApproach != eOldApproach)
			{
				LogMinorCivApproach(strOutBuf, eOldApproach);
			}
			else
			{
				strTemp.Format("---");
				strOutBuf += ", " + strTemp;
			}


			strOutBuf += ", ";


			strOutBuf += ", ";


			strOutBuf += ", ";

			LogTargetValue(strOutBuf, ePlayer);

			LogWarGoal(strOutBuf, ePlayer);

			LogMilitaryAggressivePosture(strOutBuf, ePlayer);

			LogProximity(strOutBuf, ePlayer);

			LogLandDispute(strOutBuf, ePlayer);


			strOutBuf += ", ";

			strOutBuf += ", ---";


			for(int iMinorCivApproachLoop = 0; iMinorCivApproachLoop < NUM_MINOR_CIV_APPROACHES; iMinorCivApproachLoop++)
			{
				eMinorCivApproach = (MinorCivApproachTypes) iMinorCivApproachLoop;

				switch(eMinorCivApproach)
				{
				case MINOR_CIV_APPROACH_IGNORE:
					strTemp.Format("Ignore");
					break;
				case MINOR_CIV_APPROACH_FRIENDLY:
					strTemp.Format("Friendly");
					break;
				case MINOR_CIV_APPROACH_PROTECTIVE:
					strTemp.Format("Protective");
					break;
				case MINOR_CIV_APPROACH_CONQUEST:
					strTemp.Format("Conquest");
					break;
				case MINOR_CIV_APPROACH_BULLY:
					strTemp.Format("Bully");
					break;
				default:
					strTemp.Format("XXX");
					break;
				}
				strOutBuf += ", " + strTemp;

				strTemp.Format("%d", aiApproachValues[eMinorCivApproach] / 100);
				strOutBuf += ", " + strTemp;
			}

			strOutBuf = strBaseString + strOutBuf;
			pLog->Msg(strOutBuf);
		}
	}
}


void CvDiplomacyAI::LogPersonality()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString strDesc;
		CvString strLogName;
		CvString strTemp;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "AI_Personality_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "AI_Personality_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, PERSONALITY, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;


		for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
		{
			strTemp.Format("%s, %d", GC.getFlavorTypes((FlavorTypes) iFlavorLoop).GetCString(), GetPlayer()->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes) iFlavorLoop));
			strOutBuf = strBaseString + ", " + strTemp;
			pLog->Msg(strOutBuf);
		}

		MajorCivApproachTypes eMajorCivApproach;


		for(int iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
		{
			eMajorCivApproach = (MajorCivApproachTypes) iApproachLoop;

			switch(eMajorCivApproach)
			{
			case MAJOR_CIV_APPROACH_WAR:
				strTemp.Format("War");
				break;
			case MAJOR_CIV_APPROACH_HOSTILE:
				strTemp.Format("Hostile");
				break;
			case MAJOR_CIV_APPROACH_DECEPTIVE:
				strTemp.Format("Deceptive");
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				strTemp.Format("Guarded");
				break;
			case MAJOR_CIV_APPROACH_AFRAID:
				strTemp.Format("Afraid");
				break;
			case MAJOR_CIV_APPROACH_FRIENDLY:
				strTemp.Format("Friendly");
				break;
			case MAJOR_CIV_APPROACH_NEUTRAL:
				strTemp.Format("Neutral");
				break;
			default:
				strTemp.Format("XXX");
				break;
			}
			strOutBuf = strBaseString + ", " + strTemp;

			strTemp.Format("%d", GetPersonalityMajorCivApproachBias(eMajorCivApproach));
			strOutBuf += ", " + strTemp;
			pLog->Msg(strOutBuf);
		}

		MinorCivApproachTypes eMinorCivApproach;


		for(int iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
		{
			eMinorCivApproach = (MinorCivApproachTypes) iApproachLoop;

			switch(eMinorCivApproach)
			{
			case MINOR_CIV_APPROACH_IGNORE:
				strTemp.Format("Ignore");
				break;
			case MINOR_CIV_APPROACH_FRIENDLY:
				strTemp.Format("Friendly");
				break;
			case MINOR_CIV_APPROACH_PROTECTIVE:
				strTemp.Format("Protective");
				break;
			case MINOR_CIV_APPROACH_CONQUEST:
				strTemp.Format("Conquest");
				break;
			case MINOR_CIV_APPROACH_BULLY:
				strTemp.Format("Bully");
				break;
			default:
				strTemp.Format("XXX");
				break;
			}
			strOutBuf = strBaseString + ", " + strTemp;

			strTemp.Format("%d", GetPersonalityMinorCivApproachBias(eMinorCivApproach));
			strOutBuf += ", " + strTemp;
			pLog->Msg(strOutBuf);
		}

		strTemp.Format("VICTORY COMPETITIVENESS, %d", GetVictoryCompetitiveness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("WONDER COMPETITIVENESS, %d", GetWonderCompetitiveness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("MINOR CIV COMPETITIVENESS, %d", GetMinorCivCompetitiveness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("BOLDNESS, %d", GetBoldness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("DIPLO BALANCE, %d", GetDiploBalance());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("WARMONGER HATE, %d", GetWarmongerHate());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("DENOUNCE, %d", GetDenounceWillingness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("DoF, %d", GetDoFWillingness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("LOYALTY, %d", GetLoyalty());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("NEEDINESS, %d", GetNeediness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("FORGIVENESS, %d", GetForgiveness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("CHATTINESS, %d", GetChattiness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("MEANNESS, %d", GetMeanness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);
	}
}


void CvDiplomacyAI::LogStatus()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strMinorString;
		CvString strDesc;
		CvString strLogName;
		CvString strTemp;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;

		int iPlayerLoop;
		PlayerTypes eLoopPlayer;


		for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(IsPlayerValid(eLoopPlayer))
			{
				strOutBuf = strBaseString;

				LogGrandStrategy(strOutBuf);


				if(GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					strMinorString = "--- ";
				}

				otherPlayerName = GET_PLAYER(eLoopPlayer).getCivilizationShortDescription();
				strOutBuf += ", " + strMinorString + otherPlayerName;


				if(!GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					if(IsDoFAccepted(eLoopPlayer))
						strOutBuf += ", WW";
					else
						strOutBuf += ", ";

					if(IsDenouncedPlayer(eLoopPlayer))
						strOutBuf += ", DEN";
					else
						strOutBuf += ", ";






					if(GetGlobalCoopWarAcceptedState(eLoopPlayer) == COOP_WAR_STATE_ACCEPTED)
						strOutBuf += ", CW";
					else if(GetGlobalCoopWarAcceptedState(eLoopPlayer) == COOP_WAR_STATE_SOON)
						strOutBuf += ", CWS";
					else
						strOutBuf += ", ";

					LogMajorCivApproach(strOutBuf, GetMajorCivApproach(eLoopPlayer,                       false), GetWarFaceWithPlayer(eLoopPlayer));
					LogOpinion(strOutBuf, eLoopPlayer);
				}

				else
				{
					strOutBuf += ", ";
					strOutBuf += ", ";
					strOutBuf += ", ";

					LogMinorCivApproach(strOutBuf, GetMinorCivApproach(eLoopPlayer));
					strOutBuf += ", ";
				}

				LogProximity(strOutBuf, eLoopPlayer);

				LogWarState(strOutBuf, eLoopPlayer);

				LogEconomicStrength(strOutBuf, eLoopPlayer);

				LogMilitaryStrength(strOutBuf, eLoopPlayer);
				LogTargetValue(strOutBuf, eLoopPlayer);
				LogMilitaryAggressivePosture(strOutBuf, eLoopPlayer);
				LogWarmongerThreat(strOutBuf, eLoopPlayer);
				LogMilitaryThreat(strOutBuf, eLoopPlayer);

				LogExpansionAggressivePosture(strOutBuf, eLoopPlayer);
				LogPlotBuyingAggressivePosture(strOutBuf, eLoopPlayer);
				LogLandDispute(strOutBuf, eLoopPlayer);
				LogVictoryDispute(strOutBuf, eLoopPlayer);
				LogWonderDispute(strOutBuf, eLoopPlayer);
				LogMinorCivDispute(strOutBuf, eLoopPlayer);


				if(!GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					AIGrandStrategyTypes eGrandStrategy = GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(eLoopPlayer);

					CvAIGrandStrategyXMLEntry* pEntry = (eGrandStrategy != NO_AIGRANDSTRATEGY)? GC.getAIGrandStrategyInfo(eGrandStrategy) : NULL;
					if(pEntry != NULL)
					{
						strTemp.Format("GSS %s", pEntry->GetType());
						strOutBuf += ", " + strTemp;


						switch(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategyConfidence(eLoopPlayer))
						{
						case GUESS_CONFIDENCE_POSITIVE:
							strTemp.Format("POSITIVE");
							break;
						case GUESS_CONFIDENCE_LIKELY:
							strTemp.Format("Likely");
							break;
						case GUESS_CONFIDENCE_UNSURE:
							strTemp.Format("Unsure");
							break;
						default:
							strTemp.Format("XXX");
							break;
						}
					}
					else
					{
						strTemp.Format("No GS Guess, ");
					}
				}
				else
				{
					strTemp.Format(", ");
				}
				strOutBuf += ", " + strTemp;

				pLog->Msg(strOutBuf);
			}
		}
	}
}



void CvDiplomacyAI::LogWarStatus()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strMinorString;
		CvString strDesc;
		CvString strLogName;
		CvString strTemp;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_War_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_War_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;

		int iPlayerLoop;
		PlayerTypes eLoopPlayer;

		bool bLogPlayer;


		for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(IsPlayerValid(eLoopPlayer))
			{
				if(IsAtWar(eLoopPlayer))
				{
					bLogPlayer = true;
				}
				else if(GetWarState(eLoopPlayer) != NO_WAR_STATE_TYPE)
				{
					bLogPlayer = true;
				}
				else if(GetWarGoal(eLoopPlayer) != NO_WAR_GOAL_TYPE)
				{
					bLogPlayer = true;
				}
				else
				{
					bLogPlayer = false;
				}


				if(bLogPlayer)
				{
					strOutBuf = strBaseString;


					if(GET_PLAYER(eLoopPlayer).isMinorCiv())
					{
						strMinorString = "--- ";
					}

					otherPlayerName = GET_PLAYER(eLoopPlayer).getCivilizationShortDescription();
					strOutBuf += ", " + strMinorString + otherPlayerName;


					if(GET_PLAYER(eLoopPlayer).isMinorCiv())
					{
						if(GetMinorCivApproach(eLoopPlayer) == MINOR_CIV_APPROACH_CONQUEST)
						{
							strOutBuf += ", APP: CONQUEST";
						}
						else
						{
							strOutBuf += ", APP: o";
						}
					}
					else
					{
						if(GetMajorCivApproach(eLoopPlayer, false) == MAJOR_CIV_APPROACH_WAR)
						{
							strOutBuf += ", APP: WAR";
						}
						else
						{
							strOutBuf += ", APP: o";
						}
					}


					if(IsGoingForWorldConquest())
					{
						strOutBuf += ", WC";
					}
					else
					{
						strOutBuf += ", ";
					}


					if(IsWantsPeaceWithPlayer(eLoopPlayer))
					{
						strOutBuf += ", Wants Peace!!!";
					}
					else
					{
						strOutBuf += ", ";
					}

					LogWarGoal(strOutBuf, eLoopPlayer);
					LogWarState(strOutBuf, eLoopPlayer);
					LogWarProjection(strOutBuf, eLoopPlayer);

					LogWarPeaceWillingToOffer(strOutBuf, eLoopPlayer);
					LogWarPeaceWillingToAccept(strOutBuf, eLoopPlayer);


					if(GetPlayerNumTurnsAtWar(eLoopPlayer) > 0)
					{
						strTemp.Format("%d", GetPlayerNumTurnsAtWar(eLoopPlayer));
						strOutBuf += ", " + strTemp;
					}
					else
					{
						strOutBuf += ", ";
					}

					LogWarDamage(strOutBuf, eLoopPlayer);

					LogMilitaryThreat(strOutBuf, eLoopPlayer);

					LogProximity(strOutBuf, eLoopPlayer);
					LogTargetValue(strOutBuf, eLoopPlayer);

					LogMilitaryStrength(strOutBuf, eLoopPlayer);
					LogEconomicStrength(strOutBuf, eLoopPlayer);

					pLog->Msg(strOutBuf);
				}
			}
		}
	}
}



void CvDiplomacyAI::LogStatements()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strMinorString;
		CvString strDesc;
		CvString strLogName;
		CvString strTemp;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Statement_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Statement_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;

		int iPlayerLoop;
		PlayerTypes eLoopPlayer;

		CvString strStatementLine;

		int iItem;
		DiploStatementTypes eStatement;
		int iTurn;


		for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(IsPlayerValid(eLoopPlayer))
			{
				otherPlayerName = GET_PLAYER(eLoopPlayer).getCivilizationShortDescription();


				for(iItem = 0; iItem < MAX_DIPLO_LOG_STATEMENTS; iItem++)
				{
					eStatement = GetDiploLogStatementTypeForIndex(eLoopPlayer, iItem);

					if(eStatement != NO_DIPLO_STATEMENT_TYPE)
					{
						iTurn = GetDiploLogStatementTurnForIndex(eLoopPlayer, iItem);

						strStatementLine.Format(", Statement: %d, Index: %d, Turn %d", eStatement, iItem, iTurn);

						strOutBuf = strBaseString;

						strOutBuf += ", " + otherPlayerName;
						strOutBuf += strStatementLine;

						pLog->Msg(strOutBuf);
					}
				}
			}
		}
	}
}


void CvDiplomacyAI::LogOtherPlayerExpansionGuess(PlayerTypes ePlayer, int iGuess)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Guess_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Guess_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + "EXPANSION, " + otherPlayerName;

		CvString strExpansionGuess;
		strExpansionGuess.Format(", %d", iGuess);
		strOutBuf += strExpansionGuess;

		pLog->Msg(strOutBuf);
	}
}


void CvDiplomacyAI::LogOtherPlayerGuessStatus()
{
	if((int)m_eTargetPlayer >= (int)DIPLO_FIRST_PLAYER)
		return;

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString thirdPlayerName;
		CvString strOtherPlayerGrandStrategy;
		CvString strThirdPlayerGrandStrategy;
		CvString strDesc;
		CvString strLogName;
		CvString strTemp;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Guess_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Guess_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;

		AIGrandStrategyTypes eGrandStrategy;

		int iPlayerLoop;
		PlayerTypes eLoopPlayer;
		int iOtherPlayerLoop;
		PlayerTypes eLoopOtherPlayer;


		for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(IsPlayerValid(eLoopPlayer))
			{
				otherPlayerName = GET_PLAYER(eLoopPlayer).getCivilizationShortDescription();

				eGrandStrategy = GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(eLoopPlayer);

				CvAIGrandStrategyXMLEntry* pEntry = (eGrandStrategy != NO_AIGRANDSTRATEGY)? GC.getAIGrandStrategyInfo(eGrandStrategy) : NULL;
				if(pEntry != NULL)
				{
					strTemp.Format("%s", pEntry->GetType());
					strOtherPlayerGrandStrategy = strTemp;


					switch(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategyConfidence(eLoopPlayer))
					{
					case GUESS_CONFIDENCE_POSITIVE:
						strTemp.Format("POSITIVE");
						break;
					case GUESS_CONFIDENCE_LIKELY:
						strTemp.Format("Likely");
						break;
					case GUESS_CONFIDENCE_UNSURE:
						strTemp.Format("unsure");
						break;
					default:
						strTemp.Format("XXX");
						break;
					}

					strOtherPlayerGrandStrategy += ", " + strTemp;
				}
				else
				{
					strOtherPlayerGrandStrategy.Format("No GS Guess, ");
				}


				for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_MAJOR_CIVS; iOtherPlayerLoop++)
				{
					eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;


					if(eLoopPlayer != eLoopOtherPlayer)
					{

						if(IsPlayerValid(eLoopOtherPlayer, true) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopOtherPlayer))
						{
							thirdPlayerName = GET_PLAYER(eLoopOtherPlayer).getCivilizationShortDescription();

							eGrandStrategy = GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(eLoopOtherPlayer);
							if(eGrandStrategy != NO_AIGRANDSTRATEGY)
							{
								strTemp.Format("%s", GC.getAIGrandStrategyInfo(eGrandStrategy)->GetType());
								strThirdPlayerGrandStrategy = strTemp;


								switch(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategyConfidence(eLoopOtherPlayer))
								{
								case GUESS_CONFIDENCE_POSITIVE:
									strTemp.Format("Positive");
									break;
								case GUESS_CONFIDENCE_LIKELY:
									strTemp.Format("Likely");
									break;
								case GUESS_CONFIDENCE_UNSURE:
									strTemp.Format("Unsure");
									break;
								default:
									strTemp.Format("XXX");
									break;
								}

								strThirdPlayerGrandStrategy += ", " + strTemp;
							}
							else
							{
								strThirdPlayerGrandStrategy.Format("No GS Guess, ");
							}

							strOutBuf = strBaseString + ", " + otherPlayerName + ", " + strOtherPlayerGrandStrategy + ", " + thirdPlayerName + ", " + strThirdPlayerGrandStrategy;


							switch(GetMajorCivOtherPlayerOpinion(eLoopPlayer, eLoopOtherPlayer))
							{
							case MAJOR_CIV_OPINION_ALLY:
								strTemp.Format("OPN **ALLY**");
								break;
							case MAJOR_CIV_OPINION_FRIEND:
								strTemp.Format("OPN FRIEND");
								break;
							case MAJOR_CIV_OPINION_FAVORABLE:
								strTemp.Format("OPN Favorable");
								break;
							case MAJOR_CIV_OPINION_NEUTRAL:
								strTemp.Format("OPN N");
								break;
							case MAJOR_CIV_OPINION_COMPETITOR:
								strTemp.Format("OPN Competitor");
								break;
							case MAJOR_CIV_OPINION_ENEMY:
								strTemp.Format("OPN ENEMY");
								break;
							case MAJOR_CIV_OPINION_UNFORGIVABLE:
								strTemp.Format("OPN **UNFORGIVABLE**");
								break;
							default:
								strTemp.Format("XXX");
								break;
							}
							strOutBuf += ", " + strTemp;


							switch(GetEstimateOtherPlayerMilitaryThreat(eLoopPlayer, eLoopOtherPlayer))
							{
							case THREAT_CRITICAL:
								strTemp.Format("M_THT **CRITICAL**");
								break;
							case THREAT_SEVERE:
								strTemp.Format("M_THT SEVERE");
								break;
							case THREAT_MAJOR:
								strTemp.Format("M_THT Major");
								break;
							case THREAT_MINOR:
								strTemp.Format("M_THT mnr");
								break;
							case THREAT_NONE:
								strTemp.Format("M_THT N");
								break;
							default:
								strTemp.Format("XXX");
								break;
							}
							strOutBuf += ", " + strTemp;


							switch(GetOtherPlayerWarDamageLevel(eLoopPlayer, eLoopOtherPlayer))
							{
							case WAR_DAMAGE_LEVEL_CRIPPLED:
								strTemp.Format("W_DMG **CRIPPLED*");
								break;
							case WAR_DAMAGE_LEVEL_SERIOUS:
								strTemp.Format("W_DMG SERIOUS");
								break;
							case WAR_DAMAGE_LEVEL_MAJOR:
								strTemp.Format("W_DMG Major");
								break;
							case WAR_DAMAGE_LEVEL_MINOR:
								strTemp.Format("W_DMG mnr");
								break;
							case WAR_DAMAGE_LEVEL_NONE:
								strTemp.Format("");
								break;
							default:
								strTemp.Format("XXX");
								break;
							}
							strOutBuf += ", " + strTemp;


							switch(GET_PLAYER(eLoopPlayer).GetProximityToPlayer(eLoopOtherPlayer))
							{
							case PLAYER_PROXIMITY_NEIGHBORS:
								strTemp.Format("PRX Neighbors");
								break;
							case PLAYER_PROXIMITY_CLOSE:
								strTemp.Format("PRX Close");
								break;
							case PLAYER_PROXIMITY_FAR:
								strTemp.Format("PRX Far");
								break;
							case PLAYER_PROXIMITY_DISTANT:
								strTemp.Format("PRX Distant");
								break;
							default:
								strTemp.Format("XXX");
								break;
							}
							strOutBuf += ", " + strTemp;


							switch(GetEstimateOtherPlayerLandDisputeLevel(eLoopPlayer, eLoopOtherPlayer))
							{
							case DISPUTE_LEVEL_FIERCE:
								strTemp.Format("LND **FIERCE**");
								break;
							case DISPUTE_LEVEL_STRONG:
								strTemp.Format("LND STRONG");
								break;
							case DISPUTE_LEVEL_WEAK:
								strTemp.Format("LND Weak");
								break;
							case DISPUTE_LEVEL_NONE:
								strTemp.Format("LND N");
								break;
							default:
								strTemp.Format("XXX");
								break;
							}
							strOutBuf += ", " + strTemp;


							switch(GetEstimateOtherPlayerVictoryDisputeLevel(eLoopPlayer, eLoopOtherPlayer))
							{
							case DISPUTE_LEVEL_FIERCE:
								strTemp.Format("VCT **FIERCE**");
								break;
							case DISPUTE_LEVEL_STRONG:
								strTemp.Format("VCT STRONG");
								break;
							case DISPUTE_LEVEL_WEAK:
								strTemp.Format("VCT Weak");
								break;
							case DISPUTE_LEVEL_NONE:
								strTemp.Format("VCT N");
								break;
							default:
								strTemp.Format("XXX");
								break;
							}
							strOutBuf += ", " + strTemp;

							pLog->Msg(strOutBuf);
						}
					}
				}
			}
		}
	}
}


void CvDiplomacyAI::LogGrandStrategy(CvString& strString)
{
	CvString strTemp;

	AIGrandStrategyTypes eGrandStrategy =GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy();

	if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST"))
	{
		strTemp.Format("WC");
	}
	else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP"))
	{
		strTemp.Format("Spaceship");
	}
	else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS"))
	{
		strTemp.Format("Diplomacy");
	}
	else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
	{
		strTemp.Format("Culture");
	}

	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogMajorCivApproach(CvString& strString, MajorCivApproachTypes eNewMajorCivApproach, WarFaceTypes eNewWarFace)
{
	CvString strTemp;

	switch(eNewMajorCivApproach)
	{
	case MAJOR_CIV_APPROACH_WAR:
		switch(eNewWarFace)
		{
		case WAR_FACE_HOSTILE:
			strTemp.Format("** WAR (H) **");
			break;
		case WAR_FACE_NEUTRAL:
			strTemp.Format("** WAR (N) **");
			break;
		case WAR_FACE_FRIENDLY:
			strTemp.Format("** WAR (F) **");
			break;
		default:
			strTemp.Format("** WAR **");
			break;
		}
		break;
	case MAJOR_CIV_APPROACH_HOSTILE:
		strTemp.Format("HOSTILE");
		break;
	case MAJOR_CIV_APPROACH_DECEPTIVE:
		strTemp.Format("DECEPTIVE");
		break;
	case MAJOR_CIV_APPROACH_GUARDED:
		strTemp.Format("GUARDED");
		break;
	case MAJOR_CIV_APPROACH_AFRAID:
		strTemp.Format("AFRAID");
		break;
	case MAJOR_CIV_APPROACH_FRIENDLY:
		strTemp.Format("FRIENDLY");
		break;
	case MAJOR_CIV_APPROACH_NEUTRAL:
		strTemp.Format("N");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}



void CvDiplomacyAI::LogMinorCivApproach(CvString& strString, MinorCivApproachTypes eNewMinorCivApproach)
{
	CvString strTemp;

	switch(eNewMinorCivApproach)
	{
	case MINOR_CIV_APPROACH_IGNORE:
		strTemp.Format("IG");
		break;
	case MINOR_CIV_APPROACH_FRIENDLY:
		strTemp.Format("FRIENDLY");
		break;
	case MINOR_CIV_APPROACH_PROTECTIVE:
		strTemp.Format("PROTECTIVE");
		break;
	case MINOR_CIV_APPROACH_CONQUEST:
		strTemp.Format("** CONQUEST **");
		break;
	case MINOR_CIV_APPROACH_BULLY:
		strTemp.Format("BULLY");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogMinorCivQuestType(CvString& strString, MinorCivQuestTypes eQuestType)
{
	CvString strTemp;

	switch(eQuestType)
	{
	case MINOR_CIV_QUEST_ROUTE:
		strTemp.Format("Route");
		break;
	case MINOR_CIV_QUEST_KILL_CAMP:
		strTemp.Format("Kill Camp");
		break;
	case MINOR_CIV_QUEST_CONNECT_RESOURCE:
		strTemp.Format("Connect Resource");
		break;
	case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
		strTemp.Format("Construct Wonder");
		break;
	case MINOR_CIV_QUEST_GREAT_PERSON:
		strTemp.Format("Great Person");
		break;
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
		strTemp.Format("Kill City State");
		break;
	case MINOR_CIV_QUEST_FIND_PLAYER:
		strTemp.Format("Find Player");
		break;
	case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
		strTemp.Format("Find Natural Wonder");
		break;
	case MINOR_CIV_QUEST_GIVE_GOLD:
		strTemp.Format("Give Gold");
		break;
	case MINOR_CIV_QUEST_PLEDGE_TO_PROTECT:
		strTemp.Format("Pledge to Protect");
		break;
	case MINOR_CIV_QUEST_CONTEST_CULTURE:
		strTemp.Format("Contest Culture");
		break;
	case MINOR_CIV_QUEST_CONTEST_FAITH:
		strTemp.Format("Contest Faith");
		break;
	case MINOR_CIV_QUEST_CONTEST_TECHS:
		strTemp.Format("Contest Techs");
		break;
	case MINOR_CIV_QUEST_INVEST:
		strTemp.Format("Invest");
		break;
	case MINOR_CIV_QUEST_BULLY_CITY_STATE:
		strTemp.Format("Bully City-State");
		break;
	case MINOR_CIV_QUEST_DENOUNCE_MAJOR:
		strTemp.Format("Denounce Major");
		break;
	case MINOR_CIV_QUEST_SPREAD_RELIGION:
		strTemp.Format("Spread Religion");
		break;
	case MINOR_CIV_QUEST_TRADE_ROUTE:
		strTemp.Format("Trade Route");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}

	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogOpinion(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	switch(GetMajorCivOpinion(ePlayer))
	{
	case MAJOR_CIV_OPINION_ALLY:
		strTemp.Format("OPN **ALLY**");
		break;
	case MAJOR_CIV_OPINION_FRIEND:
		strTemp.Format("OPN FRIEND");
		break;
	case MAJOR_CIV_OPINION_FAVORABLE:
		strTemp.Format("OPN Favorable");
		break;
	case MAJOR_CIV_OPINION_NEUTRAL:
		strTemp.Format("OPN N");
		break;
	case MAJOR_CIV_OPINION_COMPETITOR:
		strTemp.Format("OPN Competitor");
		break;
	case MAJOR_CIV_OPINION_ENEMY:
		strTemp.Format("OPN ENEMY");
		break;
	case MAJOR_CIV_OPINION_UNFORGIVABLE:
		strTemp.Format("OPN **UNFORGIVABLE**");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogWarmongerThreat(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format(",");
	}
	else
	{
		switch(GetWarmongerThreat(ePlayer))
		{
		case THREAT_CRITICAL:
			strTemp.Format("W_THT CRIT, %d/%d", GetOtherPlayerWarmongerAmount(ePlayer), GetOtherPlayerWarmongerScore(ePlayer));
			break;
		case THREAT_SEVERE:
			strTemp.Format("W_THT SEVR, %d/%d", GetOtherPlayerWarmongerAmount(ePlayer), GetOtherPlayerWarmongerScore(ePlayer));
			break;
		case THREAT_MAJOR:
			strTemp.Format("W_THT MAJR, %d/%d", GetOtherPlayerWarmongerAmount(ePlayer), GetOtherPlayerWarmongerScore(ePlayer));
			break;
		case THREAT_MINOR:
			strTemp.Format("W_THT MINR, %d/%d", GetOtherPlayerWarmongerAmount(ePlayer), GetOtherPlayerWarmongerScore(ePlayer));
			break;
		case THREAT_NONE:
			strTemp.Format("W_THT NONE, %d/%d", GetOtherPlayerWarmongerAmount(ePlayer), GetOtherPlayerWarmongerScore(ePlayer));
			break;
		default:
			strTemp.Format("W_THT XXXX, %d/%d", GetOtherPlayerWarmongerAmount(ePlayer), GetOtherPlayerWarmongerScore(ePlayer));
			break;
		}
	}

	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogMilitaryThreat(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{
		switch(GetMilitaryThreat(ePlayer))
		{
		case THREAT_CRITICAL:
			strTemp.Format("M_THT **CRITICAL**");
			break;
		case THREAT_SEVERE:
			strTemp.Format("M_THT SEVERE");
			break;
		case THREAT_MAJOR:
			strTemp.Format("M_THT Major");
			break;
		case THREAT_MINOR:
			strTemp.Format("M_THT mnr");
			break;
		case THREAT_NONE:
			strTemp.Format("M_THT N");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogMilitaryStrength(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	switch(GetPlayerMilitaryStrengthComparedToUs(ePlayer))
	{
	case STRENGTH_IMMENSE:
		strTemp.Format("Their MSTR **IMMENSE**");
		break;
	case STRENGTH_POWERFUL:
		strTemp.Format("Their MSTR POWERFUL");
		break;
	case STRENGTH_STRONG:
		strTemp.Format("Their MSTR Strong");
		break;
	case STRENGTH_AVERAGE:
		strTemp.Format("Their MSTR A");
		break;
	case STRENGTH_POOR:
		strTemp.Format("Their MSTR Poor");
		break;
	case STRENGTH_WEAK:
		strTemp.Format("Their MSTR WEAK");
		break;
	case STRENGTH_PATHETIC:
		strTemp.Format("Their MSTR **PATHETIC**");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;

}


void CvDiplomacyAI::LogEconomicStrength(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	switch(GetPlayerEconomicStrengthComparedToUs(ePlayer))
	{
	case STRENGTH_IMMENSE:
		strTemp.Format("Their ESTR **IMMENSE**");
		break;
	case STRENGTH_POWERFUL:
		strTemp.Format("Their ESTR POWERFUL");
		break;
	case STRENGTH_STRONG:
		strTemp.Format("Their ESTR Strong");
		break;
	case STRENGTH_AVERAGE:
		strTemp.Format("Their ESTR A");
		break;
	case STRENGTH_POOR:
		strTemp.Format("Their ESTR Poor");
		break;
	case STRENGTH_WEAK:
		strTemp.Format("Their ESTR Weak");
		break;
	case STRENGTH_PATHETIC:
		strTemp.Format("Their ESTR Pathetic");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;

}


void CvDiplomacyAI::LogTargetValue(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	switch(GetPlayerTargetValue(ePlayer))
	{
	case TARGET_VALUE_IMPOSSIBLE:
		strTemp.Format("TGT Impossible");
		break;
	case TARGET_VALUE_BAD:
		strTemp.Format("TGT Bad");
		break;
	case TARGET_VALUE_AVERAGE:
		strTemp.Format("TGT A");
		break;
	case TARGET_VALUE_FAVORABLE:
		strTemp.Format("TGT Favorable");
		break;
	case TARGET_VALUE_SOFT:
		strTemp.Format("TGT Soft");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogWarGoal(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	switch(GetWarGoal(ePlayer))
	{
	case WAR_GOAL_DEMAND:
		strTemp.Format("WGL Demand");
		break;
	case WAR_GOAL_PREPARE:
		strTemp.Format("WGL Preparing");
		break;
	case WAR_GOAL_CONQUEST:
		strTemp.Format("WGL Conquest");
		break;
	case WAR_GOAL_DAMAGE:
		strTemp.Format("WGL Damage");
		break;
	case WAR_GOAL_PEACE:
		strTemp.Format("WGL PEACE");
		break;
	default:
		if(IsAtWar(ePlayer))
		{
			strTemp.Format("XXX");
		}
		else
		{
			strTemp.Format("");
		}
		break;
	}
	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogWarPeaceWillingToOffer(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{

		switch(GetTreatyWillingToOffer(ePlayer))
		{
		case NO_PEACE_TREATY_TYPE:
			strTemp.Format("");
			break;
		case PEACE_TREATY_WHITE_PEACE:
			strTemp.Format("PeaceOff White Peace (0)");
			break;
		case PEACE_TREATY_ARMISTICE:
			strTemp.Format("PeaceOff Armistice (1)");
			break;
		case PEACE_TREATY_SETTLEMENT:
			strTemp.Format("PeaceOff Settlement (2)");
			break;
		case PEACE_TREATY_BACKDOWN:
			strTemp.Format("PeaceOff Backdown (3)");
			break;
		case PEACE_TREATY_SUBMISSION:
			strTemp.Format("PeaceOff Submission (4)");
			break;
		case PEACE_TREATY_SURRENDER:
			strTemp.Format("PeaceOff Surrender (5)");
			break;
		case PEACE_TREATY_CESSION:
			strTemp.Format("PeaceOff Cession (6)");
			break;
		case PEACE_TREATY_CAPITULATION:
			strTemp.Format("PeaceOff CAPITULATION (7)");
			break;
		case PEACE_TREATY_UNCONDITIONAL_SURRENDER:
			strTemp.Format("PeaceOff **UN SURRENDER (8)**");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogWarPeaceWillingToAccept(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{

		switch(GetTreatyWillingToAccept(ePlayer))
		{
		case NO_PEACE_TREATY_TYPE:
			strTemp.Format("");
			break;
		case PEACE_TREATY_WHITE_PEACE:
			strTemp.Format("PeaceAcc White Peace (0)");
			break;
		case PEACE_TREATY_ARMISTICE:
			strTemp.Format("PeaceAcc Armistice (1)");
			break;
		case PEACE_TREATY_SETTLEMENT:
			strTemp.Format("PeaceAcc Settlement (2)");
			break;
		case PEACE_TREATY_BACKDOWN:
			strTemp.Format("PeaceAcc Backdown (3)");
			break;
		case PEACE_TREATY_SUBMISSION:
			strTemp.Format("PeaceAcc Submission (4)");
			break;
		case PEACE_TREATY_SURRENDER:
			strTemp.Format("PeaceAcc Surrender (5)");
			break;
		case PEACE_TREATY_CESSION:
			strTemp.Format("PeaceAcc Cession (6)");
			break;
		case PEACE_TREATY_CAPITULATION:
			strTemp.Format("PeaceAcc CAPITULATION (7)");
			break;
		case PEACE_TREATY_UNCONDITIONAL_SURRENDER:
			strTemp.Format("PeaceAcc **UN SURRENDER (8)**");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}



void CvDiplomacyAI::LogWarState(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	bool bShowOperationProgress = false;


	if(GetWarGoal(ePlayer) == WAR_GOAL_PREPARE)
	{
		CvAIOperation* pOperation = GetPlayer()->GetMilitaryAI()->GetSneakAttackOperation(ePlayer);

		if(pOperation)
		{
			bShowOperationProgress = true;
			int iOperationPercentMustered = pOperation->PercentFromMusterPointToTarget();

			if(iOperationPercentMustered > 0)
			{
				strTemp.Format("PREP PRGS %2d", iOperationPercentMustered);
			}
			else
			{
				strTemp.Format("PREP Gathering");
			}
		}
	}

	if(GetWarGoal(ePlayer) == WAR_GOAL_DEMAND)
	{
		CvAIOperation* pOperation = GetPlayer()->GetMilitaryAI()->GetShowOfForceOperation(ePlayer);

		if(pOperation)
		{
			bShowOperationProgress = true;
			int iOperationPercentMustered = pOperation->PercentFromMusterPointToTarget();

			if(iOperationPercentMustered > 0)
			{
				strTemp.Format("DMND PRGS %2d", iOperationPercentMustered);
			}
			else
			{
				strTemp.Format("DMND Gathering");
			}
		}
	}

	if(!bShowOperationProgress)
	{

		switch(GetWarState(ePlayer))
		{
		case WAR_STATE_CALM:
			strTemp.Format("WST Calm");
			break;
		case WAR_STATE_NEARLY_WON:
			strTemp.Format("WST Nearly Won");
			break;
		case WAR_STATE_OFFENSIVE:
			strTemp.Format("WST Offensive");
			break;
		case WAR_STATE_STALEMATE:
			strTemp.Format("WST Stalemate");
			break;
		case WAR_STATE_DEFENSIVE:
			strTemp.Format("WST DEFENSIVE");
			break;
		case WAR_STATE_NEARLY_DEFEATED:
			strTemp.Format("WST **NEARLY DEFEATED**");
			break;
		default:
			if(IsAtWar(ePlayer))
			{
				strTemp.Format("XXX");
			}
			else
			{
				strTemp.Format("");
			}
			break;
		}
	}

	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogWarProjection(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	switch(GetWarProjection(ePlayer))
	{
	case WAR_PROJECTION_DESTRUCTION:
		strTemp.Format("W_PRJ **DESTRUCTION**");
		break;
	case WAR_PROJECTION_DEFEAT:
		strTemp.Format("W_PRJ DEFEAT");
		break;
	case WAR_PROJECTION_STALEMATE:
		strTemp.Format("W_PRJ Stalemate");
		break;
	case WAR_PROJECTION_UNKNOWN:
		strTemp.Format("W_PRJ U");
		break;
	case WAR_PROJECTION_GOOD:
		strTemp.Format("W_PRJ GOOD");
		break;
	case WAR_PROJECTION_VERY_GOOD:
		strTemp.Format("W_PRJ ** VERY GOOD **");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogWarDamage(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	switch(GetWarDamageLevel(ePlayer))
	{
	case WAR_DAMAGE_LEVEL_CRIPPLED:
		strTemp.Format("W_DMG **CRIPPLED**");
		break;
	case WAR_DAMAGE_LEVEL_SERIOUS:
		strTemp.Format("W_DMG SERIOUS");
		break;
	case WAR_DAMAGE_LEVEL_MAJOR:
		strTemp.Format("W_DMG Major");
		break;
	case WAR_DAMAGE_LEVEL_MINOR:
		strTemp.Format("W_DMG mnr");
		break;
	case WAR_DAMAGE_LEVEL_NONE:
		strTemp.Format("");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogMilitaryAggressivePosture(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	switch(GetMilitaryAggressivePosture(ePlayer))
	{
	case AGGRESSIVE_POSTURE_NONE:
		strTemp.Format("");
		break;
	case AGGRESSIVE_POSTURE_LOW:
		strTemp.Format("MAGG Low");
		break;
	case AGGRESSIVE_POSTURE_MEDIUM:
		strTemp.Format("MAGG Medium");
		break;
	case AGGRESSIVE_POSTURE_HIGH:
		strTemp.Format("MAGG HIGH");
		break;
	case AGGRESSIVE_POSTURE_INCREDIBLE:
		strTemp.Format("MAGG **INCREDIBLE**");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogExpansionAggressivePosture(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	switch(GetExpansionAggressivePosture(ePlayer))
	{
	case AGGRESSIVE_POSTURE_NONE:
		strTemp.Format("");
		break;
	case AGGRESSIVE_POSTURE_LOW:
		strTemp.Format("EAGG Low");
		break;
	case AGGRESSIVE_POSTURE_MEDIUM:
		strTemp.Format("EAGG Medium");
		break;
	case AGGRESSIVE_POSTURE_HIGH:
		strTemp.Format("EAGG HIGH");
		break;
	case AGGRESSIVE_POSTURE_INCREDIBLE:
		strTemp.Format("EAGG **INCREDIBLE**");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogPlotBuyingAggressivePosture(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	switch(GetPlotBuyingAggressivePosture(ePlayer))
	{
	case AGGRESSIVE_POSTURE_NONE:
		strTemp.Format("");
		break;
	case AGGRESSIVE_POSTURE_LOW:
		strTemp.Format("BAGG Low");
		break;
	case AGGRESSIVE_POSTURE_MEDIUM:
		strTemp.Format("BAGG Medium");
		break;
	case AGGRESSIVE_POSTURE_HIGH:
		strTemp.Format("BAGG HIGH");
		break;
	case AGGRESSIVE_POSTURE_INCREDIBLE:
		strTemp.Format("BAGG **INCREDIBLE**");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogLandDispute(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{
		switch(GetLandDisputeLevel(ePlayer))
		{
		case DISPUTE_LEVEL_FIERCE:
			strTemp.Format("LND **FIERCE**");
			break;
		case DISPUTE_LEVEL_STRONG:
			strTemp.Format("LND STRONG");
			break;
		case DISPUTE_LEVEL_WEAK:
			strTemp.Format("LND Weak");
			break;
		case DISPUTE_LEVEL_NONE:
			strTemp.Format("LND N");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogVictoryDispute(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{

		switch(GetVictoryDisputeLevel(ePlayer))
		{
		case DISPUTE_LEVEL_FIERCE:
			strTemp.Format("VCT **FIERCE**");
			break;
		case DISPUTE_LEVEL_STRONG:
			strTemp.Format("VCT STRONG");
			break;
		case DISPUTE_LEVEL_WEAK:
			strTemp.Format("VCT Weak");
			break;
		case DISPUTE_LEVEL_NONE:
			strTemp.Format("VCT N");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogWonderDispute(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{

		switch(GetWonderDisputeLevel(ePlayer))
		{
		case DISPUTE_LEVEL_FIERCE:
			strTemp.Format("WND **FIERCE**");
			break;
		case DISPUTE_LEVEL_STRONG:
			strTemp.Format("WND STRONG");
			break;
		case DISPUTE_LEVEL_WEAK:
			strTemp.Format("WND Weak");
			break;
		case DISPUTE_LEVEL_NONE:
			strTemp.Format("WND N");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogMinorCivDispute(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{

		switch(GetMinorCivDisputeLevel(ePlayer))
		{
		case DISPUTE_LEVEL_FIERCE:
			strTemp.Format("MCF **FIERCE**");
			break;
		case DISPUTE_LEVEL_STRONG:
			strTemp.Format("MCF STRONG");
			break;
		case DISPUTE_LEVEL_WEAK:
			strTemp.Format("MCF Weak");
			break;
		case DISPUTE_LEVEL_NONE:
			strTemp.Format("MCF N");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogProximity(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;


	switch(GetPlayer()->GetProximityToPlayer(ePlayer))
	{
	case PLAYER_PROXIMITY_NEIGHBORS:
		strTemp.Format("PRX Neighbors");
		break;
	case PLAYER_PROXIMITY_CLOSE:
		strTemp.Format("PRX Close");
		break;
	case PLAYER_PROXIMITY_FAR:
		strTemp.Format("PRX Far");
		break;
	case PLAYER_PROXIMITY_DISTANT:
		strTemp.Format("PRX Distant");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}


void CvDiplomacyAI::LogStatementToPlayer(PlayerTypes ePlayer, DiploStatementTypes eMessage)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());


		strBaseString += playerName;


		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;

		MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer,                       false);

		bool bMinorMessage = false;

		switch(eMessage)
		{
		case DIPLO_STATEMENT_REQUEST_PEACE:
			strTemp.Format("We request peace!!!");
			break;
		case DIPLO_STATEMENT_AGGRESSIVE_MILITARY_WARNING:
			strTemp.Format("Your Military is stationed aggressively!");
			break;
		case DIPLO_STATEMENT_KILLED_PROTECTED_CITY_STATE:
			strTemp.Format("You Killed a Protected City State!");
			bMinorMessage = true;
			break;
		case DIPLO_STATEMENT_ATTACKED_PROTECTED_CITY_STATE:
			strTemp.Format("You Attacked a Protected City State!");
			bMinorMessage = true;
			break;
		case DIPLO_STATEMENT_BULLIED_PROTECTED_CITY_STATE:
			strTemp.Format("You Bullied a Protected City State!");
			bMinorMessage = true;
			break;
		case DIPLO_STATEMENT_EXPANSION_SERIOUS_WARNING:
			strTemp.Format("Expansion SERIOUS Warning!");
			break;
		case DIPLO_STATEMENT_EXPANSION_WARNING:
			strTemp.Format("Expansion Warning!");
			break;
		case DIPLO_STATEMENT_EXPANSION_BROKEN_PROMISE:
			strTemp.Format("Expansion Broken Promise!");
			break;
		case DIPLO_STATEMENT_PLOT_BUYING_SERIOUS_WARNING:
			strTemp.Format("Plot Buying SERIOUS Warning!");
			break;
		case DIPLO_STATEMENT_PLOT_BUYING_WARNING:
			strTemp.Format("Plot Buying Warning!");
			break;
		case DIPLO_STATEMENT_PLOT_BUYING_BROKEN_PROMISE:
			strTemp.Format("Plot Buying Broken Promise!");
			break;
		case DIPLO_STATEMENT_WE_ATTACKED_YOUR_MINOR:
			strTemp.Format("Haha! We attacked your Minor!");
			bMinorMessage = true;
			break;
		case DIPLO_STATEMENT_WE_BULLIED_YOUR_MINOR:
			strTemp.Format("Haha! We bullied your Minor!");
			bMinorMessage = true;
			break;
		case DIPLO_STATEMENT_WORK_WITH_US:
			strTemp.Format("Wanna team up?");
			break;
		case DIPLO_STATEMENT_END_WORK_WITH_US:
			strTemp.Format("***** We're done working with you. *****");
			break;
		case DIPLO_STATEMENT_DENOUNCE:
			strTemp.Format("Denounce!");
			break;
		case DIPLO_STATEMENT_DENOUNCE_RANDFAILED:
			strTemp.Format("Denounce RANDFAILED");
			break;
		case DIPLO_STATEMENT_END_WORK_AGAINST_SOMEONE:
			strTemp.Format("***** We're done working against someone with you. *****");
			break;
		case DIPLO_STATEMENT_COOP_WAR_REQUEST:
			strTemp.Format("Wanna coop war against someone?");
			break;
		case DIPLO_STATEMENT_COOP_WAR_TIME:
			strTemp.Format("It's time to coop war against someone!");
			break;
		case DIPLO_STATEMENT_NOW_UNFORGIVABLE:
			strTemp.Format("***** You are Unforgivable! *****");
			break;
		case DIPLO_STATEMENT_NOW_ENEMY:
			strTemp.Format("***** You are an Enemy! *****");
			break;
		case DIPLO_STATEMENT_DEMAND:
			strTemp.Format("***** Give in to my demands or else! *****");
			break;
		case DIPLO_STATEMENT_REQUEST:
			strTemp.Format("***** Can you spare something for a friend? *****");
			break;
		case DIPLO_STATEMENT_REQUEST_RANDFAILED:
			strTemp.Format("***** Request RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_LUXURY_TRADE:
			strTemp.Format("***** You have a Luxury I would like *****");
			break;
		case DIPLO_STATEMENT_OPEN_BORDERS_EXCHANGE:
			strTemp.Format("Open Borders Exchange");
			break;
		case DIPLO_STATEMENT_OPEN_BORDERS_OFFER:
			strTemp.Format("Open Borders Offer");
			break;
		case DIPLO_STATEMENT_PLAN_RESEARCH_AGREEMENT:
			strTemp.Format("Research Plan");
			break;
		case DIPLO_STATEMENT_RESEARCH_AGREEMENT_OFFER:
			strTemp.Format("Research Agreement Offer");
			break;
		case DIPLO_STATEMENT_RENEW_DEAL:
			strTemp.Format("Renew Deal");
			break;
		case DIPLO_STATEMENT_INSULT:
			strTemp.Format("Insult");
			break;
		case DIPLO_STATEMENT_COMPLIMENT:
			if(eApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
			{
				strTemp.Format("DECEPTIVE Compliment");
			}
			else
			{
				strTemp.Format("Compliment");
			}
			break;
		case DIPLO_STATEMENT_BOOT_KISSING:
			strTemp.Format("Boot Kissing");
			break;
		case DIPLO_STATEMENT_WARMONGER:
			strTemp.Format("Warmonger!");
			break;
		case DIPLO_STATEMENT_DENOUNCE_FRIEND:
			strTemp.Format("***** DENOUNCING A FRIEND *****");
			break;
		case DIPLO_STATEMENT_REQUEST_FRIEND_DENOUNCE:
			strTemp.Format("***** DENOUNCE SOMEONE FOR ME? *****");
			break;
		case DIPLO_STATEMENT_REQUEST_FRIEND_DENOUNCE_RANDFAILED:
			strTemp.Format("***** Denounce someone for me? RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_REQUEST_FRIEND_WAR:
			strTemp.Format("***** DECLARE WAR ON SOMEONE FOR ME? *****");
			break;
		case DIPLO_STATEMENT_ANGRY_BEFRIEND_ENEMY:
			strTemp.Format("***** YOU BEFRIENDED AN ENEMY OF MINE! *****");
			break;
		case DIPLO_STATEMENT_ANGRY_BEFRIEND_ENEMY_RANDFAILED:
			strTemp.Format("***** You befriended an enemy of mine! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_ANGRY_DENOUNCED_FRIEND:
			strTemp.Format("***** YOU DENOUNCED A FRIEND OF MINE! *****");
			break;
		case DIPLO_STATEMENT_ANGRY_DENOUNCED_FRIEND_RANDFAILED:
			strTemp.Format("***** You denounced a friend of mine! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_HAPPY_DENOUNCED_ENEMY:
			strTemp.Format("***** YAY - YOU DENOUNCED AN ENEMY! *****");
			break;
		case DIPLO_STATEMENT_HAPPY_DENOUNCED_ENEMY_RANDFAILED:
			strTemp.Format("***** Yay - you denounced an enemy! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_HAPPY_BEFRIENDED_FRIEND:
			strTemp.Format("***** YAY - YOU BEFRIENDED A FRIEND! *****");
			break;
		case DIPLO_STATEMENT_HAPPY_BEFRIENDED_FRIEND_RANDFAILED:
			strTemp.Format("***** Yay - you befriended a friend! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_ENEMY:
			strTemp.Format("***** JUST FYI - I BEFRIENDED YOUR ENEMY! *****");
			break;
		case DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_ENEMY_RANDFAILED:
			strTemp.Format("***** Just FYI - I befriended your enemy! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_FRIEND:
			strTemp.Format("***** JUST FYI - I DENOUNCED YOUR FRIEND! *****");
			break;
		case DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_FRIEND_RANDFAILED:
			strTemp.Format("***** Just FYI - I denounced your friend! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_ENEMY:
			strTemp.Format("***** JUST FYI - I DENOUNCED YOUR ENEMY! *****");
			break;
		case DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_ENEMY_RANDFAILED:
			strTemp.Format("***** Just FYI - I denounced your enemy! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_FRIEND:
			strTemp.Format("***** JUST FYI - I BEFRIENDED YOUR FRIEND! *****");
			break;
		case DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_FRIEND_RANDFAILED:
			strTemp.Format("***** Just FYI - I befriended your friend! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_SAME_POLICIES_AUTOCRACY:
		case DIPLO_STATEMENT_SAME_POLICIES_FREEDOM:
		case DIPLO_STATEMENT_SAME_POLICIES_ORDER:
			strTemp.Format("***** Yay! We like the same late game social policy tree *****");
			break;
		case DIPLO_STATEMENT_WE_LIKED_THEIR_PROPOSAL:
			strTemp.Format("***** We liked their World Congress proposal *****");
			break;
		case DIPLO_STATEMENT_WE_DISLIKED_THEIR_PROPOSAL:
			strTemp.Format("***** We disliked their World Congress proposal *****");
			break;
		case DIPLO_STATEMENT_THEY_SUPPORTED_OUR_PROPOSAL:
			strTemp.Format("***** They supported our World Congress proposal *****");
			break;
		case DIPLO_STATEMENT_THEY_FOILED_OUR_PROPOSAL:
			strTemp.Format("***** They foiled our World Congress proposal *****");
			break;
		case DIPLO_STATEMENT_THEY_SUPPORTED_OUR_HOSTING:
			strTemp.Format("***** They supported our World Congress hosting *****");
			break;
		case DIPLO_STATEMENT_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_FREEDOM:
		case DIPLO_STATEMENT_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_ORDER:
		case DIPLO_STATEMENT_YOUR_IDEOLOGY_CAUSING_CIVIL_UNREST_AUTOCRACY:
			strTemp.Format("***** Your ideology is causing civil unrest in our civ *****");
			break;
		case DIPLO_STATEMENT_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_FREEDOM:
		case DIPLO_STATEMENT_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_ORDER:
		case DIPLO_STATEMENT_OUR_IDEOLOGY_CAUSING_CIVIL_UNREST_AUTOCRACY:
			strTemp.Format("***** Taunt - our ideology is causing unrest in your civ *****");
			break;
		case DIPLO_STATEMENT_SWITCH_OUR_IDEOLOGY_FREEDOM:
		case DIPLO_STATEMENT_SWITCH_OUR_IDEOLOGY_ORDER:
		case DIPLO_STATEMENT_SWITCH_OUR_IDEOLOGY_AUTOCRACY:
			strTemp.Format("***** Yay! They switched to our ideology *****");
			break;
		case DIPLO_STATEMENT_YOUR_CULTURE_INFLUENTIAL:
			strTemp.Format("***** Your culture is now influential over us *****");
			break;
		case DIPLO_STATEMENT_OUR_CULTURE_INFLUENTIAL:
			strTemp.Format("***** Taunt - Our culture is now influential over you *****");
			break;
		default:
			strTemp.Format("Unknown message!!!");
			break;
		}

		strOutBuf += ", " + strTemp;


		if(eMessage != DIPLO_STATEMENT_DENOUNCE &&
		        eMessage != DIPLO_STATEMENT_COOP_WAR_REQUEST)
			pLog->Msg(strOutBuf);


		if(bMinorMessage)
		{

			if(GC.getPlayerAndCityAILogSplit())
				strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
			else
				strLogName = "DiplomacyAI_MinorCiv_Log.csv";

			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
		}

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


void CvDiplomacyAI::TestUIDiploStatement(PlayerTypes eToPlayer, DiploStatementTypes eStatement, int iArg1)
{
	m_eTestToPlayer = eToPlayer;
	m_eTestStatement = eStatement;
	m_iTestStatementArg1 = iArg1;
}


void CvDiplomacyAI::LogOpenEmbassy(PlayerTypes ePlayer)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + ",***** OPENED EMBASSY @ " + otherPlayerName + "! *****";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


void CvDiplomacyAI::LogCloseEmbassy(PlayerTypes ePlayer)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;


		playerName = GetPlayer()->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + ",***** CLOSED EMBASSY @ " + otherPlayerName + "! *****";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}




bool CvDiplomacyAI::IsValidUIDiplomacyTarget(PlayerTypes eTargetPlayer)
{
	if(eTargetPlayer != NO_PLAYER)
	{
		CvPlayer& kTarget = GET_PLAYER(eTargetPlayer);
		if(m_eTargetPlayer == NO_PLAYER || m_eTargetPlayer == eTargetPlayer || ((DiplomacyPlayerType)m_eTargetPlayer == DIPLO_AI_PLAYERS && !kTarget.isHuman()) || ((DiplomacyPlayerType)m_eTargetPlayer == DIPLO_HUMAN_PLAYERS && kTarget.isHuman()))
			return true;
	}

	return false;
}


FDataStream& operator<<(FDataStream& saveTo, const DiploLogData& readFrom)
{
	saveTo << readFrom.m_eDiploLogStatement;
	saveTo << readFrom.m_iTurn;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, DiploLogData& writeTo)
{
	loadFrom >> writeTo.m_eDiploLogStatement;
	loadFrom >> writeTo.m_iTurn;
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const DeclarationLogData& readFrom)
{
	saveTo << readFrom.m_eDeclaration;
	saveTo << readFrom.m_iData1;
	saveTo << readFrom.m_iData2;
	saveTo << readFrom.m_eMustHaveMetPlayer;
	saveTo << readFrom.m_bActive;
	saveTo << readFrom.m_iTurn;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, DeclarationLogData& writeTo)
{
	loadFrom >> writeTo.m_eDeclaration;
	loadFrom >> writeTo.m_iData1;
	loadFrom >> writeTo.m_iData2;
	loadFrom >> writeTo.m_eMustHaveMetPlayer;
	loadFrom >> writeTo.m_bActive;
	loadFrom >> writeTo.m_iTurn;
	return loadFrom;
}



int CvDiplomacyAIHelpers::GetWarmongerOffset(int iNumCitiesRemaining, bool bIsMinor)
{
	int iEstimatedCitiesOnMap = GC.getMap().getWorldInfo().GetEstimatedNumCities();
	int iActualCitiesOnMap = GC.getGame().getNumCities();
	int iWarmongerOffset = (1000 * iEstimatedCitiesOnMap) / (max(iActualCitiesOnMap, 1) * iNumCitiesRemaining);


	if (bIsMinor)
	{
		iWarmongerOffset = (iWarmongerOffset * GC.getWARMONGER_ON_CITY_STATE_MULTIPLIER()) / 100;
	}

	return iWarmongerOffset;
}

CvString CvDiplomacyAIHelpers::GetWarmongerPreviewString(PlayerTypes eCurrentOwner)
{
	CvString szRtnValue;

	CvPlayer &kPlayer = GET_PLAYER(eCurrentOwner);
	int iNumCities = max(kPlayer.getNumCities(), 1);
	int iWarmongerOffset = CvDiplomacyAIHelpers::GetWarmongerOffset(iNumCities, kPlayer.isMinorCiv());
	iWarmongerOffset = iWarmongerOffset * GC.getEraInfo(GC.getGame().getCurrentEra())->getWarmongerPercent() / 100;

	if (iWarmongerOffset < GC.getWARMONGER_THREAT_MINOR_ATTACKED_WEIGHT())
	{
		szRtnValue = Localization::Lookup("TXT_KEY_WARMONGER_PREVIEW_MINOR").toUTF8();
	}
	else if (iWarmongerOffset < GC.getWARMONGER_THREAT_MAJOR_CONQUERED_WEIGHT())
	{
		szRtnValue = Localization::Lookup("TXT_KEY_WARMONGER_PREVIEW_MAJOR").toUTF8();
	}
	else
	{
		szRtnValue = Localization::Lookup("TXT_KEY_WARMONGER_PREVIEW_EXTREME").toUTF8();
	}

	return szRtnValue;
}

CvString CvDiplomacyAIHelpers::GetLiberationPreviewString(PlayerTypes eOriginalOwner)
{
	CvString szRtnValue;

	CvPlayer &kPlayer = GET_PLAYER(eOriginalOwner);
	int iNumCities = kPlayer.getNumCities() + 1;
	int iWarmongerOffset = CvDiplomacyAIHelpers::GetWarmongerOffset(iNumCities, kPlayer.isMinorCiv());
	iWarmongerOffset = iWarmongerOffset * GC.getEraInfo(GC.getGame().getCurrentEra())->getWarmongerPercent() / 100;

	if (iWarmongerOffset < GC.getWARMONGER_THREAT_MINOR_ATTACKED_WEIGHT())
	{
		szRtnValue = Localization::Lookup("TXT_KEY_LIBERATION_PREVIEW_MINOR").toUTF8();
	}
	else if (iWarmongerOffset < GC.getWARMONGER_THREAT_MAJOR_CONQUERED_WEIGHT())
	{
		szRtnValue = Localization::Lookup("TXT_KEY_LIBERATION_PREVIEW_MAJOR").toUTF8();
	}
	else
	{
		szRtnValue = Localization::Lookup("TXT_KEY_LIBERATION_PREVIEW_EXTREME").toUTF8();
	}
	return szRtnValue;

}

void CvDiplomacyAIHelpers::ApplyWarmongerPenalties(PlayerTypes eConqueror, PlayerTypes eConquered)
{
	CvPlayer &kConqueringPlayer = GET_PLAYER(eConqueror);

	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		PlayerTypes eMajor = (PlayerTypes)iMajorLoop;
		if (eConqueror != eMajor && GET_PLAYER(eMajor).isAlive())
		{

			CvTeam &kAffectedTeam = GET_TEAM(GET_PLAYER(eMajor).getTeam());
			if (kAffectedTeam.isHasMet(kConqueringPlayer.getTeam()))
			{
				int iNumCities = max(GET_PLAYER(eConquered).getNumCities(), 1);
				int iWarmongerOffset = CvDiplomacyAIHelpers::GetWarmongerOffset(iNumCities, GET_PLAYER(eConquered).isMinorCiv());


				if (kAffectedTeam.isAtWar(GET_PLAYER(eConquered).getTeam()))
				{
					iWarmongerOffset /= 2;
				}

				GET_PLAYER(eMajor).GetDiplomacyAI()->ChangeOtherPlayerWarmongerAmount(eConqueror, iWarmongerOffset);
			}
		}
	}
}
