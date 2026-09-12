







#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include "CvGame.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvTeam.h"
#include "CvGlobals.h"
#include "CvGameQueries.h"


UnitClassTypes CvGameQueries::GetLeastAdvancedUnitClassNobodyHas(bool bUseRandom              )
{
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_UNITS, true> UnitClassesVector;
	int iWeight;

	bool bValid;

#ifdef AUI_WARNING_FIXES
	for (uint i = 0; i < GC.getNumUnitClassInfos(); i++)
#else
	for (int i = 0; i < GC.getNumUnitClassInfos(); i++)
#endif
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(i);
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if(pkUnitClassInfo)
		{
			const UnitTypes eDefaultUnit = (UnitTypes) pkUnitClassInfo->getDefaultUnitIndex();


			if (eDefaultUnit == NO_UNIT)
			{
				continue;
			}

			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eDefaultUnit);
			if(pkUnitInfo == NULL)
			{
				continue;
			}


			if (pkUnitInfo->GetDomainType() != DOMAIN_LAND)
			{
				continue;
			}

			if (pkUnitInfo->GetCombat() <= 0)
			{
				continue;
			}

			const TechTypes ePrereqTech = (TechTypes) pkUnitInfo->GetPrereqAndTech();


			if (ePrereqTech == NO_TECH)
			{
				continue;
			}

			CvTechEntry* pkTechInfo = GC.getTechInfo(ePrereqTech);
			if(pkTechInfo == NULL)
			{
				continue;
			}

			bValid = true;
			bool bAllPlayersHaveTech = true;

			for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
			{
				if (GET_PLAYER((PlayerTypes) iMajorLoop).isAlive())
				{

					if (GET_PLAYER((PlayerTypes) iMajorLoop).getUnitClassCount(eUnitClass) > 0)
					{
						bValid = false;
						break;
					}


					if (bAllPlayersHaveTech && !GET_TEAM((TeamTypes) GET_PLAYER((PlayerTypes) iMajorLoop).getTeam()).GetTeamTechs()->HasTech(ePrereqTech))
					{
						bAllPlayersHaveTech = false;
					}
				}
			}
			if (!bValid || bAllPlayersHaveTech)
			{
				continue;
			}

			iWeight = pkUnitInfo->GetPower();


			iWeight += (pkTechInfo->GetResearchCost() / 8);


			if (bUseRandom)
			{
				iWeight += GC.getGame().getJonRandNum(iWeight / 10, "Minor Civ Quest: Gift Unit - Adding random weight to Unit Chosen");
			}

			UnitClassesVector.push_back(i, iWeight);
		}
	}

	UnitClassesVector.SortItems();



	bValid = true;

	for (int i = UnitClassesVector.size() - 1; i >= 0; i--)
	{
		const UnitClassTypes eUnitClass = (UnitClassTypes) UnitClassesVector.GetElement(i);


		if (bValid)
		{
			return eUnitClass;
		}
	}

	return NO_UNITCLASS;
}







int CvGameQueries::GetTeamClosenessScore(int** aaiDistances, int* aiStartingLocs)
{
	int iScore = 0;

	for(int iTeam = 0; iTeam < MAX_CIV_TEAMS; iTeam++)
	{
		if(GET_TEAM((TeamTypes)iTeam).isAlive())
		{
			int iTeamTotalDist = 0;
			int iNumEdges = 0;
			for(int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
			{
				if(GET_PLAYER((PlayerTypes)iPlayer).isAlive())
				{
					if(GET_PLAYER((PlayerTypes)iPlayer).getTeam() == (TeamTypes)iTeam)
					{
						for(int iOtherPlayer = 0; iOtherPlayer < iPlayer; iOtherPlayer++)
						{
							if(GET_PLAYER((PlayerTypes)iOtherPlayer).getTeam() == (TeamTypes)iTeam)
							{

								iNumEdges++;
								int iPlayerStart = aiStartingLocs[iPlayer];
								int iOtherPlayerStart = aiStartingLocs[iOtherPlayer];

								if(iPlayerStart < iOtherPlayerStart)
								{
									int iTemp = iPlayerStart;
									iPlayerStart = iOtherPlayerStart;
									iOtherPlayerStart = iTemp;
								}
								else if(iPlayerStart == iOtherPlayerStart)
								{
									CvAssertMsg(false, "Two players are (hypothetically) assigned to the same starting location!");
								}
								iTeamTotalDist += aaiDistances[iPlayerStart][iOtherPlayerStart];
							}
						}
					}
				}
			}

			int iTeamScore;
			if(iNumEdges == 0)
			{
				iTeamScore = 0;
			}
			else
			{
				iTeamScore = iTeamTotalDist/iNumEdges;
			}

			iScore += iTeamScore;
		}
	}
	return iScore;
}


bool CvGameQueries::AreUnitsSameType(UnitTypes eFirstUnitType, UnitTypes eSecondUnitType)
{
	CvUnitEntry* pkFirstUnitInfo = GC.getUnitInfo(eFirstUnitType);
	CvUnitEntry* pkSecondUnitInfo = GC.getUnitInfo(eSecondUnitType);

	if(pkFirstUnitInfo == NULL || pkSecondUnitInfo == NULL)
		return false;

	int eFirstDomain = pkFirstUnitInfo->GetDomainType();
	int eSecondDomain = pkSecondUnitInfo->GetDomainType();



	if (eFirstDomain == DOMAIN_HOVER && eSecondDomain == DOMAIN_HOVER)
	{

	}
	else if (eFirstDomain == DOMAIN_HOVER)
	{
		eFirstDomain = eSecondDomain;
	}
	else if (eSecondDomain == DOMAIN_HOVER)
	{
		eSecondDomain = eFirstDomain;
	}


	if(eFirstDomain == eSecondDomain)
	{

		if(eFirstDomain == DOMAIN_AIR)
		{
			return false;
		}

		bool bUnit1Combat = false;
		bool bUnit2Combat = false;


		if(pkFirstUnitInfo->GetCombat() > 0 || pkFirstUnitInfo->GetRange() > 0)
		{
			bUnit1Combat = true;
		}


		if(pkSecondUnitInfo->GetCombat() > 0 || pkSecondUnitInfo->GetRange() > 0)
		{
			bUnit2Combat = true;
		}


		if(bUnit1Combat == bUnit2Combat)
		{

			return true;
		}
	}

	return false;
}
