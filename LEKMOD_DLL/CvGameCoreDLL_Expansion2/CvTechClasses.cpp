






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvTechAI.h"
#include "CvFlavorManager.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvCitySpecializationAI.h"
#include "CvGrandStrategyAI.h"
#include "CvInfosSerializationHelper.h"

#include "LintFree.h"


CvTechEntry::CvTechEntry(void):
	m_iAIWeight(0),
	m_iAITradeModifier(0),
	m_iResearchCost(0),
	m_iAdvancedStartCost(0),
	m_iEra(NO_ERA),
	m_iFeatureProductionModifier(0),
	m_iUnitFortificationModifier(0),
	m_iUnitBaseHealModifier(0),
	m_iWorkerSpeedModifier(0),
	m_iFirstFreeUnitClass(NO_UNITCLASS),
	m_iFirstFreeTechs(0),
	m_iEmbarkedMoveChange(0),


	m_iEmbarkedSightChange(0),
	m_iInternationalTradeRoutesChange(0),
	m_iInfluenceSpreadModifier(0),
	m_iExtraVotesPerDiplomat(0),
	m_iGridX(0),
	m_iGridY(0),
	m_bEndsGame(false),
	m_bAllowsEmbarking(false),


	m_bAllowsEmbarkingCivilian(false),

#if defined(MISC_CHANGES)
	m_iExtraLeagueVotes(0),
#endif

	m_bAllowsDefensiveEmbarking(false),
	m_bEmbarkedAllWaterPassage(false),
	m_bAllowsBarbarianBoats(false),
	m_bRepeat(false),
	m_bTrade(false),
	m_bDisable(false),
	m_bDisableTechSteal(false),
	m_bGoodyTech(false),
	m_bExtraWaterSeeFrom(false),
	m_bMapCentering(false),
	m_bMapVisible(false),
	m_bMapTrading(false),
	m_bTechTrading(false),
	m_bGoldTrading(false),
	m_bAllowEmbassyTradingAllowed(false),
	m_bOpenBordersTradingAllowed(false),
	m_bDefensivePactTradingAllowed(false),
	m_bResearchAgreementTradingAllowed(false),
	m_bTradeAgreementTradingAllowed(false),
	m_bPermanentAllianceTrading(false),
	m_bBridgeBuilding(false),
	m_bWaterWork(false),
	m_bTriggersArchaeologicalSites(false),
	m_bAllowsWorldCongress(false),
	m_piDomainExtraMoves(NULL),
	m_piTradeRouteDomainExtraRange(NULL),
	m_piFlavorValue(NULL),
	m_piPrereqOrTechs(NULL),
	m_piPrereqAndTechs(NULL),
	m_pabFreePromotion(NULL)
{
}


CvTechEntry::~CvTechEntry(void)
{
	SAFE_DELETE_ARRAY(m_piDomainExtraMoves);
	SAFE_DELETE_ARRAY(m_piTradeRouteDomainExtraRange);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piPrereqOrTechs);
	SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	SAFE_DELETE_ARRAY(m_pabFreePromotion);
}

bool CvTechEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;


	m_iAIWeight = kResults.GetInt("AIWeight");
	m_iAITradeModifier = kResults.GetInt("AITradeModifier");
	m_iResearchCost = kResults.GetInt("Cost");
	m_iAdvancedStartCost = kResults.GetInt("AdvancedStartCost");
	m_iFeatureProductionModifier = kResults.GetInt("FeatureProductionModifier");
	m_iUnitFortificationModifier = kResults.GetInt("UnitFortificationModifier");
	m_iUnitBaseHealModifier = kResults.GetInt("UnitBaseHealModifier");
	m_iWorkerSpeedModifier = kResults.GetInt("WorkerSpeedModifier");
	m_iFirstFreeTechs = kResults.GetInt("FirstFreeTechs");
	m_iEmbarkedMoveChange = kResults.GetInt("EmbarkedMoveChange");

	m_iEmbarkedSightChange = kResults.GetInt("EmbarkedSightChange");

	m_iInternationalTradeRoutesChange = kResults.GetInt("InternationalTradeRoutesChange");
	m_iInfluenceSpreadModifier = kResults.GetInt("InfluenceSpreadModifier");
	m_iExtraVotesPerDiplomat = kResults.GetInt("ExtraVotesPerDiplomat");
	m_bEndsGame = kResults.GetBool("EndsGame");
	m_bAllowsEmbarking = kResults.GetBool("AllowsEmbarking");


	m_bAllowsEmbarkingCivilian = kResults.GetBool("AllowsEmbarkingCivilian");

#if defined(MISC_CHANGES)
	m_iExtraLeagueVotes = kResults.GetInt("AdditionalDelegates");
#endif

	m_bAllowsDefensiveEmbarking = kResults.GetBool("AllowsDefensiveEmbarking");
	m_bEmbarkedAllWaterPassage = kResults.GetBool("EmbarkedAllWaterPassage");
	m_bAllowsBarbarianBoats = kResults.GetBool("AllowsBarbarianBoats");
	m_bRepeat = kResults.GetBool("Repeat");
	m_bTrade = kResults.GetBool("Trade");
	m_bDisable = kResults.GetBool("Disable");
	m_bDisableTechSteal = kResults.GetBool("DisableTechSteal");
	m_bGoodyTech = kResults.GetBool("GoodyTech");
	m_bTriggersArchaeologicalSites = kResults.GetBool("TriggersArchaeologicalSites");
	m_bAllowsWorldCongress = kResults.GetBool("AllowsWorldCongress");
	m_bExtraWaterSeeFrom = kResults.GetBool("ExtraWaterSeeFrom");
	m_bMapCentering = kResults.GetBool("MapCentering");
	m_bMapVisible = kResults.GetBool("MapVisible");
	m_bMapTrading = kResults.GetBool("MapTrading");
	m_bTechTrading = kResults.GetBool("TechTrading");
	m_bGoldTrading = kResults.GetBool("GoldTrading");
	m_bAllowEmbassyTradingAllowed = kResults.GetBool("AllowEmbassyTradingAllowed");
	m_bOpenBordersTradingAllowed = kResults.GetBool("OpenBordersTradingAllowed");
	m_bDefensivePactTradingAllowed = kResults.GetBool("DefensivePactTradingAllowed");
	m_bResearchAgreementTradingAllowed = kResults.GetBool("ResearchAgreementTradingAllowed");
	m_bTradeAgreementTradingAllowed = kResults.GetBool("TradeAgreementTradingAllowed");
	m_bPermanentAllianceTrading = kResults.GetBool("PermanentAllianceTradingAllowed");
	m_bBridgeBuilding = kResults.GetBool("BridgeBuilding");
	m_bWaterWork = kResults.GetBool("WaterWork");
	m_iGridX = kResults.GetInt("GridX");
	m_iGridY = kResults.GetInt("GridY");


	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("Era");
	m_iEra = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("FirstFreeUnitClass");
	m_iFirstFreeUnitClass = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("Quote");
	SetQuoteKey(szTextVal);

	szTextVal = kResults.GetText("AudioIntro");
	SetSound(szTextVal);

	szTextVal = kResults.GetText("AudioIntroHeader");
	SetSoundMP(szTextVal);


	const char* szTechType = GetType();
	kUtility.PopulateArrayByValue(m_piDomainExtraMoves, "Domains", "Technology_DomainExtraMoves", "DomainType", "TechType", szTechType, "Moves", 0, NUM_DOMAIN_TYPES);
	kUtility.PopulateArrayByValue(m_piTradeRouteDomainExtraRange, "Domains", "Technology_TradeRouteDomainExtraRange", "DomainType", "TechType", szTechType, "Range", 0, NUM_DOMAIN_TYPES);
	kUtility.PopulateArrayByExistence(m_pabFreePromotion, "UnitPromotions", "Technology_FreePromotions", "PromotionType", "TechType", szTechType);
	kUtility.SetFlavors(m_piFlavorValue, "Technology_Flavors", "TechType", szTechType);

	const size_t TechnologiesCount = kUtility.MaxRows("Technologies");


	{


		kUtility.InitializeArray(m_piPrereqOrTechs, TechnologiesCount, NO_TECH);

		std::string strKey = "Technologies - Technology_ORPrereqTechs";
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Technologies.ID from Technology_ORPrereqTechs inner join Technologies on Technologies.Type = PrereqTech where TechType = ?;");

		pResults->Bind(1, szTechType, -1, false);

		int i = 0;
		while(pResults->Step())
		{
			m_piPrereqOrTechs[i++] = pResults->GetInt(0);
		}

		pResults->Reset();
	}


	{

		kUtility.InitializeArray(m_piPrereqAndTechs, TechnologiesCount, NO_TECH);

		std::string strKey = "Technologies - Technology_PrereqTechs";
		Database::Results* pResults = kUtility.GetOrPrepareResults(strKey,
			"select Technologies.ID from Technology_PrereqTechs inner join Technologies on Technologies.Type = PrereqTech where TechType = ?;");

		pResults->Bind(1, szTechType, -1, false);

		int i = 0;
		while(pResults->Step())
		{
			m_piPrereqAndTechs[i++] = pResults->GetInt(0);
		}

		pResults->Reset();
	}
	return true;
}


int CvTechEntry::GetAIWeight() const
{
	return m_iAIWeight;
}


int CvTechEntry::GetAITradeModifier() const
{
	return m_iAITradeModifier;
}


int CvTechEntry::GetResearchCost() const
{
	return m_iResearchCost;
}


int CvTechEntry::GetAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}


int CvTechEntry::GetEra() const
{
	return m_iEra;
}


int CvTechEntry::GetFeatureProductionModifier() const
{
	return m_iFeatureProductionModifier;
}


int CvTechEntry::GetUnitFortificationModifier() const
{
	return m_iUnitFortificationModifier;
}


int CvTechEntry::GetUnitBaseHealModifier() const
{
	return m_iUnitBaseHealModifier;
}


int CvTechEntry::GetWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}


int CvTechEntry::GetFirstFreeUnitClass() const
{
	return m_iFirstFreeUnitClass;
}


int CvTechEntry::GetFirstFreeTechs() const
{
	return m_iFirstFreeTechs;
}


int CvTechEntry::GetEmbarkedMoveChange() const
{
	return m_iEmbarkedMoveChange;
}


int CvTechEntry::GetEmbarkedSightChange() const
{
	return m_iEmbarkedSightChange;
}


int CvTechEntry::GetNumInternationalTradeRoutesChange (void) const
{
	return m_iInternationalTradeRoutesChange;
}


int CvTechEntry::GetInfluenceSpreadModifier() const
{
	return m_iInfluenceSpreadModifier;
}


int CvTechEntry::GetExtraVotesPerDiplomat() const
{
	return m_iExtraVotesPerDiplomat;
}


int CvTechEntry::GetGridX() const
{
	return m_iGridX;
}


int CvTechEntry::GetGridY() const
{
	return m_iGridY;
}


bool CvTechEntry::IsEndsGame() const
{
	return m_bEndsGame;
}


bool CvTechEntry::IsAllowsEmbarking() const
{
	return m_bAllowsEmbarking;
}


bool CvTechEntry::IsAllowsEmbarkingCivilian() const
{
	return m_bAllowsEmbarkingCivilian;
}

#if defined(MISC_CHANGES)

int CvTechEntry::GetExtraLeagueVotes() const
{
	return m_iExtraLeagueVotes;
}
#endif


bool CvTechEntry::IsAllowsDefensiveEmbarking() const
{
	return m_bAllowsDefensiveEmbarking;
}


bool CvTechEntry::IsEmbarkedAllWaterPassage() const
{
	return m_bEmbarkedAllWaterPassage;
}


bool CvTechEntry::IsAllowsBarbarianBoats() const
{
	return m_bAllowsBarbarianBoats;
}


bool CvTechEntry::IsRepeat() const
{
	return m_bRepeat;
}


bool CvTechEntry::IsTrade() const
{
	return m_bTrade;
}


bool CvTechEntry::IsDisable() const
{
	return m_bDisable;
}



bool CvTechEntry::IsGoodyTech() const
{
	return m_bGoodyTech;
}


bool CvTechEntry::IsTriggersArchaeologicalSites() const
{
	return m_bTriggersArchaeologicalSites;
}


bool CvTechEntry::IsAllowsWorldCongress() const
{
	return m_bAllowsWorldCongress;
}


bool CvTechEntry::IsExtraWaterSeeFrom() const
{
	return m_bExtraWaterSeeFrom;
}


bool CvTechEntry::IsMapCentering() const
{
	return m_bMapCentering;
}


bool CvTechEntry::IsMapVisible() const
{
	return m_bMapVisible;
}


bool CvTechEntry::IsMapTrading() const
{
	return m_bMapTrading;
}


bool CvTechEntry::IsTechTrading() const
{
	return m_bTechTrading;
}


bool CvTechEntry::IsGoldTrading() const
{
	return m_bGoldTrading;
}


bool CvTechEntry::IsAllowEmbassyTradingAllowed() const
{
	return m_bAllowEmbassyTradingAllowed;
}


bool CvTechEntry::IsOpenBordersTradingAllowed() const
{
	return m_bOpenBordersTradingAllowed;
}


bool CvTechEntry::IsDefensivePactTradingAllowed() const
{
	return m_bDefensivePactTradingAllowed;
}


bool CvTechEntry::IsResearchAgreementTradingAllowed() const
{
	return m_bResearchAgreementTradingAllowed;
}


bool CvTechEntry::IsTradeAgreementTradingAllowed() const
{
	return m_bTradeAgreementTradingAllowed;
}


bool CvTechEntry::IsPermanentAllianceTrading() const
{
	return m_bPermanentAllianceTrading;
}


bool CvTechEntry::IsBridgeBuilding() const
{
	return m_bBridgeBuilding;
}


bool CvTechEntry::IsWaterWork() const
{
	return m_bWaterWork;
}


int CvTechEntry::IsFreePromotion(int i) const
{
	return m_pabFreePromotion ? m_pabFreePromotion[i] : -1;
}


const char* CvTechEntry::GetQuote()
{
	return m_wstrQuote.c_str();
}


void CvTechEntry::SetQuoteKey(const char* szVal)
{
	m_strQuoteKey = szVal;
	m_wstrQuote = GetLocalizedText(m_strQuoteKey);
}


const char* CvTechEntry::GetSound() const
{
	return m_strSound;
}


void CvTechEntry::SetSound(const char* szVal)
{
	m_strSound = szVal;
}


const char* CvTechEntry::GetSoundMP() const
{
	return m_strSoundMP;
}


void CvTechEntry::SetSoundMP(const char* szVal)
{
	m_strSoundMP = szVal;
}




int CvTechEntry::GetDomainExtraMoves(int i) const
{
	return m_piDomainExtraMoves ? m_piDomainExtraMoves[i] : -1;
}


int CvTechEntry::GetTradeRouteDomainExtraRange(int i) const
{
	return m_piTradeRouteDomainExtraRange ? m_piTradeRouteDomainExtraRange[i] : -1;
}



int CvTechEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : -1;
}


int CvTechEntry::GetPrereqOrTechs(int i) const
{
	return m_piPrereqOrTechs ? m_piPrereqOrTechs[i] : -1;
}


int CvTechEntry::GetPrereqAndTechs(int i) const
{
	return m_piPrereqAndTechs ? m_piPrereqAndTechs[i] : -1;
}





CvTechXMLEntries::CvTechXMLEntries(void)
{

}


CvTechXMLEntries::~CvTechXMLEntries(void)
{
	DeleteArray();
}


std::vector<CvTechEntry*>& CvTechXMLEntries::GetTechEntries()
{
	return m_paTechEntries;
}


#ifdef AUI_WARNING_FIXES
uint CvTechXMLEntries::GetNumTechs() const
#else
int CvTechXMLEntries::GetNumTechs()
#endif
{
	return m_paTechEntries.size();
}


void CvTechXMLEntries::DeleteArray()
{
	for(std::vector<CvTechEntry*>::iterator it = m_paTechEntries.begin(); it != m_paTechEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paTechEntries.clear();
}


#ifdef AUI_WARNING_FIXES
_Ret_maybenull_ CvTechEntry* CvTechXMLEntries::GetEntry(uint index)
#else
CvTechEntry* CvTechXMLEntries::GetEntry(int index)
#endif
{
	return m_paTechEntries[index];
}






CvPlayerTechs::CvPlayerTechs():
	m_pabResearchingTech(NULL),
	m_piCivTechPriority(NULL),
	m_piLocaleTechPriority(NULL),
	m_peLocaleTechResources(NULL),
	m_peCivTechUniqueBuildings(NULL),
	m_peCivTechUniqueUnits(NULL),
	m_peCivTechUniqueImprovements(NULL),
	m_pTechs(NULL),
	m_pPlayer(NULL),
	m_pTechAI(NULL)
{
}


CvPlayerTechs::~CvPlayerTechs(void)
{
}


void CvPlayerTechs::Init(CvTechXMLEntries* pTechs, CvPlayer* pPlayer, bool bIsCity)
{

	CvFlavorRecipient::Init();


	m_bIsCity = bIsCity;
	m_pTechs = pTechs;
	m_pPlayer = pPlayer;


	const int iNumTechs = m_pTechs->GetNumTechs();

	CvAssertMsg(m_pabResearchingTech==NULL, "about to leak memory, CvPlayerTechs::m_pabResearchingTech");
	m_pabResearchingTech = FNEW(bool[iNumTechs], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_piCivTechPriority==NULL, "about to leak memory, CvPlayerTechs::m_piCivTechPriority");
	m_piCivTechPriority = FNEW(int[iNumTechs], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_piLocaleTechPriority==NULL, "about to leak memory, CvPlayerTechs::m_piLocaleTechPriority");
	m_piLocaleTechPriority = FNEW(int[iNumTechs], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_peLocaleTechResources==NULL, "about to leak memory, CvPlayerTechs::m_peLocaleTechResources");
	m_peLocaleTechResources = FNEW(ResourceTypes[iNumTechs], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_peCivTechUniqueUnits==NULL, "about to leak memory, CvPlayerTechs::m_peCivTechUniqueUnits");
	m_peCivTechUniqueUnits = FNEW(UnitTypes[iNumTechs], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_peCivTechUniqueBuildings==NULL, "about to leak memory, CvPlayerTechs::m_peCivTechUniqueBuildings");
	m_peCivTechUniqueBuildings = FNEW(BuildingTypes[iNumTechs], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_peCivTechUniqueImprovements==NULL, "about to leak memory, CvPlayerTechs::m_peCivTechUniqueImprovements");
	m_peCivTechUniqueImprovements = FNEW(ImprovementTypes[iNumTechs], c_eCiv5GameplayDLL, 0);


	m_pTechAI = FNEW(CvTechAI(this), c_eCiv5GameplayDLL, 0);

	Reset();
}


void CvPlayerTechs::Uninit()
{

	CvFlavorRecipient::Uninit();

	SAFE_DELETE_ARRAY(m_pabResearchingTech);
	SAFE_DELETE_ARRAY(m_piCivTechPriority);
	SAFE_DELETE_ARRAY(m_piLocaleTechPriority);
	SAFE_DELETE_ARRAY(m_peLocaleTechResources);
	SAFE_DELETE_ARRAY(m_peCivTechUniqueBuildings);
	SAFE_DELETE_ARRAY(m_peCivTechUniqueUnits);
	SAFE_DELETE_ARRAY(m_peCivTechUniqueImprovements);
	SAFE_DELETE(m_pTechAI);
}


void CvPlayerTechs::Reset()
{
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif
	CvBuildingXMLEntries* pkGameBuildings = GC.GetGameBuildings();

	for(iI = 0; iI < m_pTechs->GetNumTechs(); iI++)
	{
		m_pabResearchingTech[iI] = false;
		m_piCivTechPriority[iI] = 1;
		m_piLocaleTechPriority[iI] = 1;
		m_peLocaleTechResources[iI] = NO_RESOURCE;
		m_peCivTechUniqueUnits[iI] = NO_UNIT;
		m_peCivTechUniqueBuildings[iI] = NO_BUILDING;
		m_peCivTechUniqueImprovements[iI] = NO_IMPROVEMENT;
	}


	if(!m_pPlayer->isMinorCiv() && !m_pPlayer->isBarbarian() && m_pPlayer->getCivilizationType() != NO_CIVILIZATION)
	{
		CvCivilizationInfo* pkInfo = GC.getCivilizationInfo(m_pPlayer->getCivilizationType());
		if(pkInfo)
		{

			for(iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
			{
				const BuildingTypes eBuilding = static_cast<BuildingTypes>(pkInfo->getCivilizationBuildings(iI));
				CvBuildingEntry* pkBuildingInfo = NULL;

				if(eBuilding != -1)
					pkBuildingInfo = pkGameBuildings->GetEntry(eBuilding);

				if(pkBuildingInfo)
				{

					if(pkInfo->isCivilizationBuildingOverridden(iI))
					{
						int iTech = pkBuildingInfo->GetPrereqAndTech();
						if(iTech != NO_TECH)
						{
							m_piCivTechPriority[iTech] *= GC.getTECH_PRIORITY_UNIQUE_ITEM();
							m_peCivTechUniqueBuildings[iTech] = eBuilding;
						}
					}
				}
			}


			for(iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
			{

				if(pkInfo->isCivilizationUnitOverridden(iI))
				{
					UnitTypes eCivilizationUnit = static_cast<UnitTypes>(pkInfo->getCivilizationUnits(iI));
					if(eCivilizationUnit != NO_UNIT)
					{
						CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eCivilizationUnit);
						if(pkUnitEntry)
						{
							int iTech = pkUnitEntry->GetPrereqAndTech();
							if(iTech != NO_TECH)
							{
								m_piCivTechPriority[iTech] *= GC.getTECH_PRIORITY_UNIQUE_ITEM();
								m_peCivTechUniqueUnits[iTech] = (UnitTypes)pkInfo->getCivilizationUnits(iI);
							}
						}
					}
				}
			}


			for(iI = 0; iI < GC.getNumImprovementInfos(); iI++)
			{
				CvImprovementEntry* pkImprovementEntry = GC.getImprovementInfo((ImprovementTypes)iI);
				if(pkImprovementEntry)
				{
					if(pkImprovementEntry->IsSpecificCivRequired() && pkImprovementEntry->GetRequiredCivilization() == m_pPlayer->getCivilizationType())
					{

#ifdef AUI_WARNING_FIXES
						for (uint jJ = 0; jJ < GC.getNumBuildInfos(); jJ++)
#else
						for(int jJ = 0; jJ < GC.getNumBuildInfos(); jJ++)
#endif
						{
							CvBuildInfo* pkBuildEntry = GC.getBuildInfo((BuildTypes)jJ);
#ifdef AUI_WARNING_FIXES
							if (pkBuildEntry && (uint)pkBuildEntry->getImprovement() == iI)
#else
							if(pkBuildEntry && pkBuildEntry->getImprovement() == iI)
#endif
							{
								int iTech = pkBuildEntry->getTechPrereq();
								if(iTech != NO_TECH)
								{
									m_piCivTechPriority[iTech] *= GC.getTECH_PRIORITY_UNIQUE_ITEM();
									m_peCivTechUniqueImprovements[iTech] = static_cast<ImprovementTypes>(iI);
								}
							}
						}
					}
				}
			}
		}


#ifdef AUI_WARNING_FIXES
		for (uint iTraitLoop = 0; iTraitLoop < GC.getNumTraitInfos(); iTraitLoop++)
#else
		int iNumTraits = GC.getNumTraitInfos();
		for(int iTraitLoop = 0; iTraitLoop < iNumTraits; iTraitLoop++)
#endif
		{
			TraitTypes eTraitLoop = (TraitTypes) iTraitLoop;

			CvLeaderHeadInfo* pkLeaderInfo = &m_pPlayer->getLeaderInfo();
			if(pkLeaderInfo)
			{
				if(!pkLeaderInfo->hasTrait(iTraitLoop))
					continue;

				CvTraitEntry* pkTraitInfo = GC.getTraitInfo(eTraitLoop);
				if(pkTraitInfo)
				{

					if(pkTraitInfo->IsMayaCalendarBonuses())
					{
						int iPrereqTech = pkTraitInfo->GetPrereqTech();
						if(iPrereqTech != NO_TECH)
						{
							m_piCivTechPriority[iPrereqTech] *= GC.getTECH_PRIORITY_MAYA_CALENDAR_BONUS();
						}
					}


					int iPrereqTech = pkTraitInfo->GetFreeUnitPrereqTech();
					if (iPrereqTech != NO_TECH)
					{
						m_piCivTechPriority[iPrereqTech] += GC.getTECH_PRIORITY_MAYA_CALENDAR_BONUS();
					}

					iPrereqTech = pkTraitInfo->GetCapitalFreeBuildingPrereqTech();
					if (iPrereqTech != NO_TECH)
					{
						m_piCivTechPriority[iPrereqTech] += GC.getTECH_PRIORITY_MAYA_CALENDAR_BONUS();
					}


					iPrereqTech = pkTraitInfo->GetFreeBuildingPrereqTech();
					if (iPrereqTech != NO_TECH)
					{
						m_piCivTechPriority[iPrereqTech] += GC.getTECH_PRIORITY_MAYA_CALENDAR_BONUS();
					}
				}
			}
		}
	}


	m_pTechAI->Reset();
}


void CvPlayerTechs::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;


	CvAssertMsg(m_pTechs != NULL && m_pTechs->GetNumTechs() > 0, "Number of techs to serialize is expected to greater than 0");
	if(m_pTechs != NULL)
	{
		const int iNumTechs = m_pTechs->GetNumTechs();

		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabResearchingTech, iNumTechs);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_piCivTechPriority, iNumTechs);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_piLocaleTechPriority, iNumTechs);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_peLocaleTechResources, iNumTechs);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_peCivTechUniqueUnits, iNumTechs);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_peCivTechUniqueBuildings, iNumTechs);
		CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_peCivTechUniqueImprovements, iNumTechs);
	}


	m_pTechAI->Read(kStream);

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");

	int iNumFlavors;
	kStream >> iNumFlavors;

	ArrayWrapper<int> kLatestFlavorWrapper(iNumFlavors, m_piLatestFlavorValues);
	kStream >> kLatestFlavorWrapper;
}


void CvPlayerTechs::Write(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;


	CvAssertMsg(m_pTechs != NULL && m_pTechs->GetNumTechs() > 0, "Number of techs to serialize is expected to greater than 0");
	if(m_pTechs != NULL)
	{
		const int iNumTechs = m_pTechs->GetNumTechs();

		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, bool>(kStream, m_pabResearchingTech, iNumTechs);
		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, int>(kStream, m_piCivTechPriority, iNumTechs);
		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, int>(kStream, m_piLocaleTechPriority, iNumTechs);
		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, ResourceTypes>(kStream, m_peLocaleTechResources, iNumTechs);
		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, UnitTypes>(kStream, m_peCivTechUniqueUnits, iNumTechs);
		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, BuildingTypes>(kStream, m_peCivTechUniqueBuildings, iNumTechs);
		CvInfosSerializationHelper::WriteHashedDataArray<TechTypes, ImprovementTypes>(kStream, m_peCivTechUniqueImprovements, iNumTechs);
	}


	m_pTechAI->Write(kStream);

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");
	kStream << GC.getNumFlavorTypes();
	kStream << ArrayWrapper<int>(GC.getNumFlavorTypes(), m_piLatestFlavorValues);
}


void CvPlayerTechs::FlavorUpdate()
{
	SetLocalePriorities();
	AddFlavorAsStrategies(GC.getTECH_WEIGHT_PROPAGATION_PERCENT());
}


CvPlayer* CvPlayerTechs::GetPlayer()
{
	return m_pPlayer;
}


CvTechAI* CvPlayerTechs::GetTechAI()
{
	return m_pTechAI;
}


bool CvPlayerTechs::IsResearchingTech(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabResearchingTech[eIndex];
}


void CvPlayerTechs::SetResearchingTech(TechTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(m_pabResearchingTech[eIndex] != bNewValue)
	{
		m_pabResearchingTech[eIndex] = bNewValue;
	}
}


void CvPlayerTechs::SetCivTechPriority(TechTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	m_piCivTechPriority[eIndex] = iNewValue;
}


int CvPlayerTechs::GetCivTechPriority(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_piCivTechPriority[eIndex];
}


void CvPlayerTechs::SetLocaleTechPriority(TechTypes eIndex, int iNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	m_piLocaleTechPriority[eIndex] = iNewValue;
}


int CvPlayerTechs::GetLocaleTechPriority(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_piLocaleTechPriority[eIndex];
}

ResourceTypes CvPlayerTechs::GetLocaleTechResource(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	return m_peLocaleTechResources[eIndex];
}

UnitTypes CvPlayerTechs::GetCivTechUniqueUnit(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	return m_peCivTechUniqueUnits[eIndex];
}

BuildingTypes CvPlayerTechs::GetCivTechUniqueBuilding(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	return m_peCivTechUniqueBuildings[eIndex];
}

ImprovementTypes CvPlayerTechs::GetCivTechUniqueImprovement(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	return m_peCivTechUniqueImprovements[eIndex];
}


void CvPlayerTechs::SetLocalePriorities()
{
	int iLoop;
	CvCity* pCity;
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif

	for(iI = 0; iI < m_pTechs->GetNumTechs(); iI++)
	{
		m_piLocaleTechPriority[iI] = 1;
		m_peLocaleTechResources[iI] = NO_RESOURCE;
	}


	for(pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{

		for(int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
		{
			CvPlot* pLoopPlot = plotCity(pCity->getX(), pCity->getY(), iPlotLoop);

			if(pLoopPlot != NULL)
			{
				int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pCity->getX(), pCity->getY());

				if(pLoopPlot->getOwner() == pCity->getOwner() || (iDistance <= 2 && pLoopPlot->getOwner() == NO_PLAYER))
				{
					if(!pLoopPlot->isWater())
					{
						ResourceTypes eResource = pLoopPlot->getResourceType(m_pPlayer->getTeam());
						if(eResource == NO_RESOURCE)
						{
							continue;
						}


						ImprovementTypes eCorrectImprovement = NO_IMPROVEMENT;
						BuildTypes eCorrectBuild = NO_BUILD;
#ifdef AUI_WARNING_FIXES
						uint iBuildIndex;
#else
						int iBuildIndex;
#endif
						for(iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
						{
							const BuildTypes eBuild = static_cast<BuildTypes>(iBuildIndex);
							CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
							if(pkBuildInfo)
							{

								const ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
								if(eImprovement != NO_IMPROVEMENT)
								{
									CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
									if(pkImprovementInfo && pkImprovementInfo->IsImprovementResourceTrade(eResource))
									{
										eCorrectBuild = eBuild;
										eCorrectImprovement = eImprovement;
										break;
									}
								}
							}
						}


						if(eCorrectBuild == NO_BUILD || eCorrectImprovement == NO_IMPROVEMENT)
						{
							continue;
						}



						if(!m_pPlayer->canBuild(pLoopPlot, eCorrectBuild, false, false))
						{

#ifdef AUI_WARNING_FIXES
							uint iTech = (uint)GC.getBuildInfo(eCorrectBuild)->getTechPrereq();
							CvAssert(iTech < m_pTechs->GetNumTechs());
							if (iTech < m_pTechs->GetNumTechs())
#else
							int iTech = GC.getBuildInfo(eCorrectBuild)->getTechPrereq();
							CvAssert(iTech < m_pTechs->GetNumTechs());
							if (iTech >= 0 && iTech < m_pTechs->GetNumTechs())
#endif
							{
								m_piLocaleTechPriority[iTech]++;
								m_peLocaleTechResources[iTech] = eResource;
							}
						}
					}
				}
			}
		}
	}
}




bool CvPlayerTechs::IsResearch() const
{
#ifdef AUI_PLAYERTECH_FIX_CAN_RESEARCH_WITH_NO_FOUNDED_CITY
	return (m_pPlayer->getNumCities() > 0);
#else

	if(!m_pPlayer->isFoundedFirstCity())
	{
		return false;
	}

	return true;
#endif
}


bool CvPlayerTechs::CanEverResearch(TechTypes eTech) const
{
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo == NULL)
	{
		return false;
	}

	if(pkTechInfo->IsDisable())
	{
		return false;
	}

	if(m_pPlayer->getCivilizationInfo().isCivilizationDisableTechs(eTech))
	{
		return false;
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(eTech);



		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanEverResearch", args.get(), bResult))
		{

			if(bResult == false)
			{
				return false;
			}
		}
	}


	return true;
}


bool CvPlayerTechs::CanResearch(TechTypes eTech, bool bTrade) const
{
	bool bFoundPossible;
	bool bFoundValid;
	int iI;

	CvTechEntry* pkTechEntry = GC.getTechInfo(eTech);
	if(pkTechEntry == NULL)
		return false;

	if(!IsResearch() && m_pPlayer->getAdvancedStartPoints() < 0)
	{
		return false;
	}


	if(GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(eTech))
	{
		return false;
	}

	bFoundPossible = false;
	bFoundValid = false;


	for(iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
	{
		TechTypes ePrereq = (TechTypes)pkTechEntry->GetPrereqOrTechs(iI);
		if(ePrereq != NO_TECH)
		{
			bFoundPossible = true;

			if(GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(ePrereq))
			{
				if(!bTrade || !GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->IsNoTradeTech(ePrereq))
				{
					bFoundValid = true;
					break;
				}
			}
		}
	}

	if(bFoundPossible && !bFoundValid)
	{
		return false;
	}


	for(iI = 0; iI < GC.getNUM_AND_TECH_PREREQS(); iI++)
	{
		TechTypes ePrereq = (TechTypes)pkTechEntry->GetPrereqAndTechs(iI);
		if(ePrereq != NO_TECH)
		{
			if(!GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(ePrereq))
			{
				return false;
			}

			if(bTrade && GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->IsNoTradeTech(ePrereq))
			{
				return false;
			}
		}
	}


	if(!CanEverResearch(eTech))
	{
		return false;
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(eTech);



		bool bResult = false;
		if(LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanResearch", args.get(), bResult))
		{

			if(bResult == false)
			{
				return false;
			}
		}
	}

	return true;
}


bool CvPlayerTechs::CanResearchForFree(TechTypes eTech) const
{
#ifdef AUI_WARNING_FIXES
	CvAssertMsg(uint(eTech) < GC.getNumTechInfos(), "eTech is expected to be within maximum bounds (invalid Index)");
	if (uint(eTech) >= GC.getNumTechInfos()) return false;
#else
	CvAssertMsg(eTech >= 0, "eTech is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTech < GC.getNumTechInfos(), "eTech is expected to be within maximum bounds (invalid Index)");
	if(eTech < 0 || eTech >= GC.getNumTechInfos()) return false;
#endif


	return CanResearch(eTech);
}


TechTypes CvPlayerTechs::GetCurrentResearch() const
{
	CLLNode<TechTypes>* pResearchNode;

	pResearchNode = m_pPlayer->headResearchQueueNode();

	if(pResearchNode != NULL)
	{
		return pResearchNode->m_data;
	}
	else
	{
		return NO_TECH;
	}
}


bool CvPlayerTechs::IsCurrentResearchRepeat() const
{
	const TechTypes eCurrentResearch = GetCurrentResearch();
	if(eCurrentResearch == NO_TECH)
	{
		return false;
	}

	CvTechEntry* pkTechInfo = GC.getTechInfo(eCurrentResearch);
	if(pkTechInfo == NULL)
	{
		return false;
	}

	return pkTechInfo->IsRepeat();
}


bool CvPlayerTechs::IsNoResearchAvailable() const
{
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif

	if(GetCurrentResearch() != NO_TECH)
	{
		return false;
	}

	for(iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if(CanResearch((TechTypes)iI))
		{
			return false;
		}
	}
	return true;
}



void CvPlayerTechs::CheckForTechAchievement() const
{
	if(m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer())
	{

		if((CvString)m_pPlayer->getLeaderTypeKey() == "LEADER_CATHERINE")
		{
#ifdef AUI_WARNING_FIXES
			for (uint iI = 0; iI < GC.getNumTechInfos(); iI++)
#else
			for(int iI = 0; iI < GC.getNumTechInfos(); iI++)
#endif
			{
				const TechTypes eTech = static_cast<TechTypes>(iI);
				CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
				if(pkTechInfo)
				{
					CvString szCurrentTech = (CvString) pkTechInfo->GetType();
					if(szCurrentTech == "TECH_HORSEBACK_RIDING")
					{
						if(GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(eTech))
						{
							int iJ;
							int iNumPlayersWith = 0;
							for(iJ = 0; iJ < MAX_MAJOR_CIVS; iJ++)
							{
								if(!GET_PLAYER((PlayerTypes)iJ).isBarbarian() && !GET_PLAYER((PlayerTypes)iJ).isMinorCiv())
								{
									if(GET_TEAM(GET_PLAYER((PlayerTypes)iJ).getTeam()).GetTeamTechs()->HasTech(eTech))
									{
										iNumPlayersWith++;
									}
								}
							}
							if(iNumPlayersWith <= 1)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_PONY);
							}
						}
					}
				}
			}
		}

		if(m_pPlayer->GetPlayerTechs()->IsCurrentResearchRepeat())
		{
#ifdef AUI_WARNING_FIXES
			for (uint iI = 0; iI < GC.getNumTechInfos() - 1; iI++)
#else
			for(int iI = 0; iI < GC.getNumTechInfos() - 1; iI++)
#endif
			{
				const TechTypes eTech = static_cast<TechTypes>(iI);
				CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
				if(pkTechInfo)
				{
					if(!GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech(eTech))
					{
						return;
					}
				}

			}

			if(GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetTechCount((TechTypes)m_pPlayer->GetPlayerTechs()->GetCurrentResearch()) < 1)
			{
				return;
			}
			gDLL->UnlockAchievement(ACHIEVEMENT_ALL_TECHS);
		}
	}
}


int CvPlayerTechs::GetResearchTurnsLeft(TechTypes eTech, bool bOverflow) const
{
	int iTurnsLeft = GetResearchTurnsLeftTimes100(eTech, bOverflow);

	if(iTurnsLeft == INT_MAX)
	{
		return INT_MAX;
	}

	iTurnsLeft = (iTurnsLeft + 99) / 100;

	return std::max(1, iTurnsLeft);
}


int CvPlayerTechs::GetResearchTurnsLeftTimes100(TechTypes eTech, bool bOverflow) const
{
	int iResearchRate;
	int iOverflow;
	int iTurnsLeft;
	int iI;

	iResearchRate = 0;
	iOverflow = 0;

	for(iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{

			if(kPlayer.getTeam() == m_pPlayer->getTeam())
			{

				if((iI == m_pPlayer->GetID()) || kPlayer.GetPlayerTechs()->GetCurrentResearch() == eTech)
				{
					iResearchRate += kPlayer.GetScienceTimes100();
#ifdef AUI_PLAYER_FIX_NO_RESEARCH_OVERFLOW_DOUBLE_DIP
					iOverflow += kPlayer.getOverflowResearch();
#else
					iOverflow += (kPlayer.getOverflowResearch() * m_pPlayer->calculateResearchModifier(eTech)) / 100;
#endif
				}
			}
		}
	}

	if(iResearchRate == 0)
	{
		return INT_MAX;
	}

	int iResearchCost = GetResearchCost(eTech);

	int iResearchProgress = GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetResearchProgress(eTech);

	int iResearchLeft = std::max(0, (iResearchCost - iResearchProgress));


	if(bOverflow)
	{
		iResearchLeft -= iOverflow;
	}

	iResearchLeft *= 10000;

	iTurnsLeft = (iResearchLeft / iResearchRate);

	if(iTurnsLeft * iResearchRate < iResearchLeft)
	{
		++iTurnsLeft;
	}

	return std::max(1, iTurnsLeft);
}


int CvPlayerTechs::GetNumTechsCanBeResearched() const
{
	int rtnValue = 0;

#ifdef AUI_WARNING_FIXES
	for (uint iTechLoop = 0; iTechLoop < GetTechs()->GetNumTechs(); iTechLoop++)
#else
	for(int iTechLoop = 0; iTechLoop < GetTechs()->GetNumTechs(); iTechLoop++)
#endif
	{
		if(CanResearch((TechTypes)iTechLoop))
		{
			rtnValue++;
		}
	}

	return rtnValue;
}


CvTechXMLEntries* CvPlayerTechs::GetTechs() const
{
	return m_pTechs;
}




int CvPlayerTechs::GetResearchCost(TechTypes eTech) const
{
#ifdef AUI_TECH_FIX_TEAMER_RESEARCH_COSTS
	return GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetResearchCost(eTech);
#else

	int iResearchCost = GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetResearchCost(eTech);
	

	int iResearchMod = std::max(1, m_pPlayer->calculateResearchModifier(eTech));
	iResearchCost = ((iResearchCost * 10000) / iResearchMod);


	int iMod = GC.getMap().getWorldInfo().GetNumCitiesTechCostMod();


	int iResearchModDiscount = m_pPlayer->GetNumCitiesResearchCostDiscount();
	if (iResearchModDiscount != 0)
	{
		iMod = iMod * (100 + iResearchModDiscount);
		iMod /= 100;
	}


#ifdef NQ_IGNORE_PUPPETS_FOR_RESEARCH_COSTS_FROM_POLICIES
	bool bIncludePuppets = (GC.getGame().isOption("GAMEOPTION_FAST_HAND"));
	iMod = iMod * m_pPlayer->GetMaxEffectiveCities(bIncludePuppets);
#else
	iMod = iMod * m_pPlayer->GetMaxEffectiveCities(                    true);
#endif
	iResearchCost = iResearchCost * (100 + iMod) / 100;



	if((iResearchCost % 100) != 0)
		iResearchCost = (iResearchCost / 100) + 1;
	else
		iResearchCost = (iResearchCost / 100);

	return iResearchCost;
#endif
}



int CvPlayerTechs::GetResearchProgress(TechTypes eTech) const
{

	int iResearchProgress = GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetResearchProgress(eTech);



	iResearchProgress += m_pPlayer->getOverflowResearch();

	return iResearchProgress;
}


int CvPlayerTechs::GetMedianTechResearch() const
{
	vector<int> aiTechCosts;
	int iRtnValue = 0;

#ifdef AUI_WARNING_FIXES
	for (uint iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
#else
	for(int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
#endif
	{
		TechTypes eTech = (TechTypes)iTechLoop;

		if(CanResearch(eTech))
		{
			aiTechCosts.push_back(GetResearchCost(eTech));
		}
	}

	int iNumEntries = aiTechCosts.size();
	if(iNumEntries > 0)
	{
		std::sort(aiTechCosts.begin(), aiTechCosts.end());


		if((iNumEntries / 2) * 2 != iNumEntries)
		{
			iRtnValue = aiTechCosts[iNumEntries / 2];
		}


		else
		{
			iRtnValue = (aiTechCosts[(iNumEntries - 1) / 2] + aiTechCosts[iNumEntries / 2]) / 2;
		}
	}

	return iRtnValue;
}

#ifdef ESPIONAGE_SYSTEM_REWORK
int CvPlayerTechs::GetMedianTechToStealResearch(PlayerTypes eTarget) const
{
	vector<int> aiTechCosts;
	int iRtnValue = 0;

	for (int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
	{
		TechTypes eTech = (TechTypes)iTechLoop;

		if (m_pPlayer->canStealTech(eTarget, eTech))
		{
			aiTechCosts.push_back(GetResearchCost(eTech));
		}
	}

	int iNumEntries = aiTechCosts.size();
	if (iNumEntries > 0)
	{
		std::sort(aiTechCosts.begin(), aiTechCosts.end());


		if ((iNumEntries / 2) * 2 != iNumEntries)
		{
			iRtnValue = aiTechCosts[iNumEntries / 2];
		}


		else
		{
			iRtnValue = (aiTechCosts[(iNumEntries - 1) / 2] + aiTechCosts[iNumEntries / 2]) / 2;
		}
	}

	return iRtnValue;
}

#endif




void CvPlayerTechs::AddFlavorAsStrategies(int iPropagatePercent)
{

	m_pTechAI->Reset();

	int iBiggestFlavor = -1000;
	for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
	{
		int iFlavorValue = GetLatestFlavorValue((FlavorTypes) iFlavor);
		if(iFlavorValue > iBiggestFlavor)
		{
			iBiggestFlavor = iFlavorValue;
		}
	}


	int iGameProgressFactor = (GC.getGame().getElapsedGameTurns() * 1000) / GC.getGame().getDefaultEstimateEndTurn();
	iGameProgressFactor = min(900,max(100,iGameProgressFactor));
	for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
	{
		int iCurrentFlavorValue = GetLatestFlavorValue((FlavorTypes) iFlavor);


		iCurrentFlavorValue = (iCurrentFlavorValue * 10) / iBiggestFlavor;

		int iPersonalityFlavorValue = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavor);



		int iFlavorValue = ((iCurrentFlavorValue * (1000 - iGameProgressFactor)) + (iPersonalityFlavorValue * iGameProgressFactor)) / 1000;

		if(iFlavorValue > 0)
		{
			m_pTechAI->AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue, iPropagatePercent);
		}
	}
}

void CvPlayerTechs::LogFlavors(FlavorTypes eFlavor)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;
		CvString strLogName;


		playerName = m_pPlayer->getCivilizationShortDescription();


		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "TechAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "TechAILog.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);


		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";


		if(eFlavor == NO_FLAVOR)
		{
			for(int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
			{
				strTemp.Format("Flavor, %s, %d", GC.getFlavorTypes((FlavorTypes)iI).GetCString(), m_piLatestFlavorValues[iI]);
				strOutBuf = strBaseString + strTemp;
				pLog->Msg(strOutBuf);
			}
		}
		else
		{
			strTemp.Format("Flavor, %s, %d", GC.getFlavorTypes(eFlavor).GetCString(), m_piLatestFlavorValues[eFlavor]);
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}





CvTeamTechs::CvTeamTechs():
#ifdef AUI_WARNING_FIXES
	m_pTechs(NULL),
	m_pTeam(NULL),
	m_eLastTechAcquired(NO_TECH),
#endif
	m_pabHasTech(NULL),
#ifdef HAS_TECH_BY_HUMAN
	m_pabHasTechByHuman(NULL),
#endif
#ifdef CAN_PROPOSE_ENACT_UPDATES_ONCE_PER_SESSION
	m_pabHasTechForLeague(NULL),
#endif
	m_pabNoTradeTech(NULL),
	m_paiResearchProgress(NULL),
	m_paiTechCount(NULL)
{
}


CvTeamTechs::~CvTeamTechs(void)
{
}


void CvTeamTechs::Init(CvTechXMLEntries* pTechs, CvTeam* pTeam)
{

	m_pTechs = pTechs;
	m_pTeam = pTeam;


	CvAssertMsg(m_pabHasTech==NULL, "about to leak memory, CvTeamTechs::m_pabHasTech");
	m_pabHasTech = FNEW(bool[m_pTechs->GetNumTechs()], c_eCiv5GameplayDLL, 0);
#ifdef HAS_TECH_BY_HUMAN
	CvAssertMsg(m_pabHasTechByHuman == NULL, "about to leak memory, CvTeamTechs::m_pabHasTechByHuman");
	m_pabHasTechByHuman = FNEW(bool[m_pTechs->GetNumTechs()], c_eCiv5GameplayDLL, 0);
#endif
#ifdef CAN_PROPOSE_ENACT_UPDATES_ONCE_PER_SESSION
	CvAssertMsg(m_pabHasTechForLeague == NULL, "about to leak memory, CvTeamTechs::m_pabHasTechForLeague");
	m_pabHasTechForLeague = FNEW(bool[m_pTechs->GetNumTechs()], c_eCiv5GameplayDLL, 0);
#endif
	CvAssertMsg(m_pabNoTradeTech==NULL, "about to leak memory, CvTeamTechs::m_pabNoTradeTech");
	m_pabNoTradeTech = FNEW(bool[m_pTechs->GetNumTechs()], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_paiResearchProgress==NULL, "about to leak memory, CvTeamTechs::m_paiResearchProgress");
	m_paiResearchProgress = FNEW(int [m_pTechs->GetNumTechs()], c_eCiv5GameplayDLL, 0);
	CvAssertMsg(m_paiTechCount==NULL, "about to leak memory, CvTeamTechs::m_paiTechCount");
	m_paiTechCount = FNEW(int [m_pTechs->GetNumTechs()], c_eCiv5GameplayDLL, 0);

	Reset();
}


void CvTeamTechs::Uninit()
{
	SAFE_DELETE_ARRAY(m_pabHasTech);
#ifdef HAS_TECH_BY_HUMAN
	SAFE_DELETE_ARRAY(m_pabHasTechByHuman);
#endif
#ifdef CAN_PROPOSE_ENACT_UPDATES_ONCE_PER_SESSION
	SAFE_DELETE_ARRAY(m_pabHasTechForLeague);
#endif
	SAFE_DELETE_ARRAY(m_pabNoTradeTech);
	SAFE_DELETE_ARRAY(m_paiResearchProgress);
	SAFE_DELETE_ARRAY(m_paiTechCount);
}


void CvTeamTechs::Reset()
{
#ifdef AUI_WARNING_FIXES
	uint iI;
#else
	int iI;
#endif

	m_eLastTechAcquired = NO_TECH;

	for(iI = 0; iI < m_pTechs->GetNumTechs(); iI++)
	{
		m_pabHasTech[iI] = false;
#ifdef HAS_TECH_BY_HUMAN
		m_pabHasTechByHuman[iI] = false;
#endif
#ifdef CAN_PROPOSE_ENACT_UPDATES_ONCE_PER_SESSION
		m_pabHasTechForLeague[iI] = false;
#endif
		m_pabNoTradeTech[iI] = false;
		m_paiResearchProgress[iI] = 0;
		m_paiTechCount[iI] = 0;
	}
}


const char* ms_V0ExpansionTechTags[81] =
{
	"TECH_AGRICULTURE",
	"TECH_POTTERY",
	"TECH_ANIMAL_HUSBANDRY",
	"TECH_ARCHERY",
	"TECH_MINING",
	"TECH_SAILING",
	"TECH_CALENDAR",
	"TECH_WRITING",
	"TECH_TRAPPING",
	"TECH_THE_WHEEL",
	"TECH_MASONRY",
	"TECH_BRONZE_WORKING",
	"TECH_OPTICS",
	"TECH_HORSEBACK_RIDING",
	"TECH_MATHEMATICS",
	"TECH_CONSTRUCTION",
	"TECH_PHILOSOPHY",
	"TECH_DRAMA",
	"TECH_CURRENCY",
	"TECH_ENGINEERING",
	"TECH_IRON_WORKING",
	"TECH_THEOLOGY",
	"TECH_CIVIL_SERVICE",
	"TECH_GUILDS",
	"TECH_METAL_CASTING",
	"TECH_COMPASS",
	"TECH_EDUCATION",
	"TECH_CHIVALRY",
	"TECH_MACHINERY",
	"TECH_PHYSICS",
	"TECH_STEEL",
	"TECH_ASTRONOMY",
	"TECH_ACOUSTICS",
	"TECH_BANKING",
	"TECH_PRINTING_PRESS",
	"TECH_GUNPOWDER",
	"TECH_NAVIGATION",
	"TECH_ARCHITECTURE",
	"TECH_ECONOMICS",
	"TECH_METALLURGY",
	"TECH_CHEMISTRY",
	"TECH_ARCHAEOLOGY",
	"TECH_SCIENTIFIC_THEORY",
	"TECH_INDUSTRIALIZATION",
	"TECH_MILITARY_SCIENCE",
	"TECH_RIFLING",
	"TECH_FERTILIZER",
	"TECH_BIOLOGY",
	"TECH_STEAM_POWER",
	"TECH_DYNAMITE",
	"TECH_ELECTRICITY",
	"TECH_REPLACEABLE_PARTS",
	"TECH_RAILROAD",
	"TECH_REFRIGERATION",
	"TECH_HYDROELECTRICS",
	"TECH_RADIO",
	"TECH_FLIGHT",
	"TECH_BALLISTICS",
	"TECH_COMBUSTION",
	"TECH_PLASTIC",
	"TECH_PENICILIN",
	"TECH_ELECTRONICS",
	"TECH_RADAR",
	"TECH_COMBINED_ARMS",
	"TECH_ATOMIC_THEORY",
	"TECH_ECOLOGY",
	"TECH_COMPUTERS",
	"TECH_TELECOM",
	"TECH_ROCKETRY",
	"TECH_NUCLEAR_FISSION",
	"TECH_GLOBALIZATION",
	"TECH_ROBOTICS",
	"TECH_SATELLITES",
	"TECH_STEALTH",
	"TECH_MOBILE_TACTICS",
	"TECH_LASERS",
	"TECH_ADVANCED_BALLISTICS",
	"TECH_PARTICLE_PHYSICS",
	"TECH_NANOTECHNOLOGY",
	"TECH_NUCLEAR_FUSION",
	"TECH_FUTURE_TECH"
};



void CvTeamTechs::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_eLastTechAcquired;


#ifdef AUI_WARNING_FIXES
	uint iNumSavedTechs;
	kStream >> iNumSavedTechs;

	if (iNumSavedTechs)
	{
		uint iNumActiveTechs = m_pTechs->GetNumTechs();
#else
	int iNumSavedTechs;
	kStream >> iNumSavedTechs;

	if(iNumSavedTechs)
	{
		int iNumActiveTechs = m_pTechs->GetNumTechs();
#endif


		CvAssert(m_pTechs == GC.GetGameTechs());
		int* paTechIDs = (int*)_malloca(iNumSavedTechs * sizeof(int));
		CvInfosSerializationHelper::ReadHashedTypeArray(kStream, iNumSavedTechs, paTechIDs, iNumSavedTechs);

		CvInfosSerializationHelper::ReadAndRemapDataArray(kStream, iNumSavedTechs, m_pabHasTech, iNumActiveTechs, paTechIDs);
#ifdef HAS_TECH_BY_HUMAN
#ifdef SAVE_BACKWARDS_COMPATIBILITY
		if (uiVersion >= BUMP_SAVE_VERSION_TECHS)
		{
#endif
			CvInfosSerializationHelper::ReadAndRemapDataArray(kStream, iNumSavedTechs, m_pabHasTechByHuman, iNumActiveTechs, paTechIDs);
#ifdef SAVE_BACKWARDS_COMPATIBILITY
		}
		else
		{
			for (int iI = 0; iI < m_pTechs->GetNumTechs(); iI++)
			{
				m_pabHasTechByHuman[iI] = false;
			}
		}
#endif
#endif
#ifdef CAN_PROPOSE_ENACT_UPDATES_ONCE_PER_SESSION
# ifdef SAVE_BACKWARDS_COMPATIBILITY
		if (uiVersion >= 1000)
		{
# endif
			CvInfosSerializationHelper::ReadAndRemapDataArray(kStream, iNumSavedTechs, m_pabHasTechForLeague, iNumActiveTechs, paTechIDs);
# ifdef SAVE_BACKWARDS_COMPATIBILITY
		}
		else
		{
			for (int iI = 0; iI < m_pTechs->GetNumTechs(); iI++)
			{
				m_pabHasTechForLeague[iI] = false;
			}
		}
# endif
#endif
		CvInfosSerializationHelper::ReadAndRemapDataArray(kStream, iNumSavedTechs, m_pabNoTradeTech, iNumActiveTechs, paTechIDs);
		CvInfosSerializationHelper::ReadAndRemapDataArray(kStream, iNumSavedTechs, m_paiResearchProgress, iNumActiveTechs, paTechIDs);
		CvInfosSerializationHelper::ReadAndRemapDataArray(kStream, iNumSavedTechs, m_paiTechCount, iNumActiveTechs, paTechIDs);

		_freea(paTechIDs);
	}
}



void CvTeamTechs::Write(FDataStream& kStream)
{

	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_eLastTechAcquired;

	if(m_pTechs != NULL && m_pTechs->GetNumTechs())
	{

#ifdef AUI_WARNING_FIXES
		uint iNumTechs = m_pTechs->GetNumTechs();
		kStream << iNumTechs;

		for (uint i = 0; i < iNumTechs; ++i)
#else
		int iNumTechs = m_pTechs->GetNumTechs();
		kStream << (int)iNumTechs;

		for(int i = 0; i < iNumTechs; ++i)
#endif
			CvInfosSerializationHelper::WriteHashed(kStream, m_pTechs->GetEntry(i));

		kStream << ArrayWrapper<bool>(iNumTechs, m_pabHasTech);
#ifdef HAS_TECH_BY_HUMAN
		kStream << ArrayWrapper<bool>(iNumTechs, m_pabHasTechByHuman);
#endif
#ifdef CAN_PROPOSE_ENACT_UPDATES_ONCE_PER_SESSION
		kStream << ArrayWrapper<bool>(iNumTechs, m_pabHasTechForLeague);
#endif
		kStream << ArrayWrapper<bool>(iNumTechs, m_pabNoTradeTech);
		kStream << ArrayWrapper<int>(iNumTechs, m_paiResearchProgress);
		kStream << ArrayWrapper<int>(iNumTechs, m_paiTechCount);
	}
	else
	{
		kStream << (int)0;
	}
}


void CvTeamTechs::SetHasTech(TechTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(m_pabHasTech[eIndex] != bNewValue)
	{
		m_pabHasTech[eIndex] = bNewValue;

		if(bNewValue)
			SetLastTechAcquired(eIndex);

		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if(pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(m_pTeam->GetID());
			args->Push(eIndex);
			args->Push(bNewValue);



			bool bResult = false;
			LuaSupport::CallHook(pkScriptSystem, "TeamSetHasTech", args.get(), bResult);
		}
	}
}


bool CvTeamTechs::HasTech(TechTypes eIndex) const
{
	if(eIndex == NO_TECH)
	{
		return true;
	}

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(m_pabHasTech != NULL, "m_pabHasTech is not expected to be equal with NULL");
	if(m_pabHasTech != NULL)
		return m_pabHasTech[eIndex];
	else
		return false;
}

#ifdef HAS_TECH_BY_HUMAN

void CvTeamTechs::SetHasTechByHuman(TechTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (m_pabHasTechByHuman[eIndex] != bNewValue)
	{
		m_pabHasTechByHuman[eIndex] = bNewValue;
	}
}


bool CvTeamTechs::HasTechByHuman(TechTypes eIndex) const
{
	if (eIndex == NO_TECH)
	{
		return true;
	}

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(m_pabHasTechByHuman != NULL, "m_pabHasTechByHuman is not expected to be equal with NULL");
	if (m_pabHasTechByHuman != NULL)
		return m_pabHasTechByHuman[eIndex];
	else
		return false;
}

#endif
#ifdef CAN_PROPOSE_ENACT_UPDATES_ONCE_PER_SESSION
void CvTeamTechs::SetHasTechForLeague(TechTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(m_pabHasTechForLeague[eIndex] != bNewValue)
	{
		m_pabHasTechForLeague[eIndex] = bNewValue;
	}
}

bool CvTeamTechs::HasTechForLeague(TechTypes eIndex) const
{
	if(eIndex == NO_TECH)
	{
		return true;
	}

	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(m_pabHasTechForLeague != NULL, "m_pabHasTechForLeague is not expected to be equal with NULL");
	if(m_pabHasTechForLeague != NULL)
		return m_pabHasTechForLeague[eIndex];
	else
		return false;
}

#endif


TechTypes CvTeamTechs::GetLastTechAcquired() const
{
	return m_eLastTechAcquired;
}


void CvTeamTechs::SetLastTechAcquired(TechTypes eTech)
{
	CvAssertMsg(eTech >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTech < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	m_eLastTechAcquired = eTech;
}


int CvTeamTechs::GetNumTechsKnown() const
{
	int iNumTechs = 0;

#ifdef AUI_WARNING_FIXES
	for (uint iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
#else
	for(int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
#endif
	{
		if(HasTech((TechTypes) iTechLoop))
		{
			iNumTechs++;
		}
	}

	return iNumTechs;
}


bool CvTeamTechs::HasResearchedAllTechs() const
{
#ifdef AUI_WARNING_FIXES
	uint iNumTechs = 0;
	for (uint iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
#else
	int iNumTechs = 0;

	for(int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
#endif
	{
		if(HasTech((TechTypes) iTechLoop) || GetTechCount((TechTypes)iTechLoop) > 0)
		{
			iNumTechs++;
		}
	}

	return (iNumTechs >= m_pTechs->GetNumTechs());
}


void CvTeamTechs::SetNoTradeTech(TechTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(m_pabNoTradeTech[eIndex] != bNewValue)
	{
		m_pabNoTradeTech[eIndex] = bNewValue;
	}
}


bool CvTeamTechs::IsNoTradeTech(TechTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabNoTradeTech[eIndex];
}


void CvTeamTechs::IncrementTechCount(TechTypes eIndex)
{
	m_paiTechCount[eIndex]++;
}


int CvTeamTechs::GetTechCount(TechTypes eIndex)const
{
	return m_paiTechCount[eIndex];
}


void CvTeamTechs::SetResearchProgress(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer)
{
	SetResearchProgressTimes100(eIndex, iNewValue * 100, ePlayer);
}


void CvTeamTechs::SetResearchProgressTimes100(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if(GetResearchProgressTimes100(eIndex) != iNewValue)
	{
		m_paiResearchProgress[eIndex] = iNewValue;
		CvAssert(GetResearchProgressTimes100(eIndex) >= 0);

		if(m_pTeam->GetID() == GC.getGame().getActiveTeam())
		{
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(Score_DIRTY_BIT, true);
		}

		int iResearchProgress = GetResearchProgressTimes100(eIndex);
#ifdef AUI_TECH_FIX_PLAYER_BASED_RESEARCH_COST_ONLY_AESTHETIC
		int iResearchCost = GET_PLAYER(ePlayer).GetPlayerTechs()->GetResearchCost(eIndex) * 100;
#else
		int iResearchCost = GetResearchCost(eIndex) * 100;


		int iResearchMod = std::max(1, GET_PLAYER(ePlayer).calculateResearchModifier(eIndex));
		iResearchCost = (iResearchCost * 100) / iResearchMod;
		int iNumCitiesMod = GC.getMap().getWorldInfo().GetNumCitiesTechCostMod();
		iNumCitiesMod = iNumCitiesMod * GET_PLAYER(ePlayer).GetMaxEffectiveCities(                    true);
		iResearchCost = iResearchCost * (100 + iNumCitiesMod) / 100;
#endif
		
		int iOverflow = iResearchProgress - iResearchCost;



		int iMaxOverflow = GetMaxResearchOverflow(eIndex, ePlayer);
		if (iOverflow > iMaxOverflow)
		{
			iOverflow = iMaxOverflow;
		}

		if(iOverflow >= 0)
		{
			GET_PLAYER(ePlayer).changeOverflowResearchTimes100(iOverflow);
			m_pTeam->setHasTech(eIndex, true, ePlayer, true, true);
			SetNoTradeTech(eIndex, true);


			GET_PLAYER(ePlayer).GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_RESEARCH_COMPLETE);


			PlayerTypes eLoopPlayer;
			int iCulture;
			TeamTypes eTeamID = m_pTeam->GetID();
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;
				CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);

				if(kLoopPlayer.getTeam() == eTeamID)
				{
					iCulture = kLoopPlayer.GetCulturePerTechResearched();
					if(iCulture != 0)
					{
						kLoopPlayer.changeJONSCulture(iCulture);

#ifdef UPDATE_CULTURE_NOTIFICATION_DURING_TURN

						if (kLoopPlayer.isAlive() && kLoopPlayer.isHuman() && kLoopPlayer.getNumCities() > 0)
						{
							kLoopPlayer.TestMidTurnPolicyNotification();
						}
#endif
					}
				}
			}
		}
	}
}


int CvTeamTechs::GetResearchProgress(TechTypes eIndex) const
{
	if(eIndex != NO_TECH)
	{
		return m_paiResearchProgress[eIndex] / 100;
	}
	else
	{
		return 0;
	}
}


int CvTeamTechs::GetResearchProgressTimes100(TechTypes eIndex) const
{
	if(eIndex != NO_TECH)
	{
		return m_paiResearchProgress[eIndex];
	}
	else
	{
		return 0;
	}
}


int CvTeamTechs::GetResearchCost(TechTypes eTech) const
{
	CvAssertMsg(eTech != NO_TECH, "Tech is not assigned a valid value");
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo == NULL)
	{
		return 0;
	}

	int iCost = pkTechInfo->GetResearchCost();

	CvHandicapInfo* pkHandicapInfo = GC.getHandicapInfo(m_pTeam->getHandicapType());
	if(pkHandicapInfo)
	{
		iCost *= pkHandicapInfo->getResearchPercent();
		iCost /= 100;
	}



	iCost *= GC.getMap().getWorldInfo().getResearchPercent();
	iCost /= 100;

	iCost *= GC.getGame().getGameSpeedInfo().getResearchPercent();
	iCost /= 100;

	iCost *= GC.getGame().getStartEraInfo().getResearchPercent();
	iCost /= 100;

	iCost *= std::max(0, ((GC.getTECH_COST_EXTRA_TEAM_MEMBER_MODIFIER() * (m_pTeam->getNumMembers() - 1)) + 100));
	iCost /= 100;

#ifdef AUI_TECH_FIX_TEAMER_RESEARCH_COSTS

	int iResearchMod = MAX(1, m_pTeam->calculateResearchModifier(eTech));
	iCost = (iCost * 10000) / iResearchMod;


	int iCityCountMod = GC.getMap().getWorldInfo().GetNumCitiesTechCostMod();


	int iWeightedResearchModDiscount = 0;
	int iCityCount = 0;
	int iLoopCityCount = 0;
	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.getTeam() == m_pTeam->GetID())
		{
			iLoopCityCount = kLoopPlayer.GetMaxEffectiveCities(                    true);
			iCityCount += iLoopCityCount;
			iWeightedResearchModDiscount += kLoopPlayer.GetNumCitiesResearchCostDiscount() * iLoopCityCount;
		}
	}
	if (iCityCount > 0)
	{
		if (iWeightedResearchModDiscount != 0)
		{
			iCityCountMod = iCityCountMod * ((100 * iCityCount) + iWeightedResearchModDiscount) / iCityCount;
			iCityCountMod /= 100;
		}
	}


	iCityCountMod *= iCityCount;
	iCost = iCost * (100 + iCityCountMod) / 100;



	if ((iCost % 100) != 0)
		iCost = (iCost / 100) + 1;
	else
		iCost = (iCost / 100);
#endif

	return std::max(1, iCost);
}


int CvTeamTechs::GetResearchLeft(TechTypes eTech) const
{
	return std::max(0, (GetResearchCost(eTech) - GetResearchProgress(eTech)));
}


CvTechXMLEntries* CvTeamTechs::GetTechs() const
{
	return m_pTechs;
}


void CvTeamTechs::ChangeResearchProgress(TechTypes eIndex, int iChange, PlayerTypes ePlayer)
{
	ChangeResearchProgressTimes100(eIndex, iChange * 100, ePlayer);
}


void CvTeamTechs::ChangeResearchProgressTimes100(TechTypes eIndex, int iChange, PlayerTypes ePlayer)
{
	SetResearchProgressTimes100(eIndex, (GetResearchProgressTimes100(eIndex) + iChange), ePlayer);
}


int CvTeamTechs::ChangeResearchProgressPercent(TechTypes eIndex, int iPercent, PlayerTypes ePlayer)
{
	int iBeakers = 0;

	if(0 != iPercent && !HasTech(eIndex))
	{
		if(iPercent > 0)
		{
#ifdef AUI_TECH_FIX_PLAYER_BASED_RESEARCH_COST_ONLY_AESTHETIC
			iBeakers = std::min(GetResearchLeft(eIndex), (GET_PLAYER(ePlayer).GetPlayerTechs()->GetResearchCost(eIndex) * iPercent) / 100);
#else
			iBeakers = std::min(GetResearchLeft(eIndex), (GetResearchCost(eIndex) * iPercent) / 100);
#endif
		}
		else
		{
#ifdef AUI_TECH_FIX_PLAYER_BASED_RESEARCH_COST_ONLY_AESTHETIC
			iBeakers = std::max(GetResearchLeft(eIndex) - GET_PLAYER(ePlayer).GetPlayerTechs()->GetResearchCost(eIndex), (GET_PLAYER(ePlayer).GetPlayerTechs()->GetResearchCost(eIndex) * iPercent) / 100);
#else
			iBeakers = std::max(GetResearchLeft(eIndex) - GetResearchCost(eIndex), (GetResearchCost(eIndex) * iPercent) / 100);
#endif
		}

		ChangeResearchProgress(eIndex, iBeakers, ePlayer);
	}

	return iBeakers;
}



int CvTeamTechs::GetMaxResearchOverflow(TechTypes eTech, PlayerTypes ePlayer) const
{
	CvPlayer &kPlayer = GET_PLAYER(ePlayer);


	int iReturnValue = kPlayer.GetScienceTimes100() * 5;   


	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo == NULL)
	{
		return 0;
	}

	int iCost = pkTechInfo->GetResearchCost() * 100;

	iReturnValue = max(iCost, iReturnValue);

	return iReturnValue;
}
