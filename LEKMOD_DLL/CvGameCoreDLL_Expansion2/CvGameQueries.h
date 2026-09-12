






#ifndef CVGAMEQUERIES_H
#define CVGAMEQUERIES_H

#pragma once

#include "CvEnums.h"


class CvGameQueries
{
public:
	static UnitClassTypes	GetLeastAdvancedUnitClassNobodyHas(bool bUseRandom = false);
	static int				GetTeamClosenessScore(int** aaiDistances, int* aiStartingLocs);
	static bool				AreUnitsSameType(UnitTypes eFirstUnitType, UnitTypes eSecondUnitType);

};

#endif
