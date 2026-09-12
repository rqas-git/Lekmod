






#pragma once

#ifndef CIV5_BUILDING_PRODUCTION_AI_H
#define CIV5_BUILDING_PRODUCTION_AI_H

#include "CvWeightedVector.h"




#define SAFE_ESTIMATE_NUM_BUILDINGS 135

class CvCity;
class CvCityBuildings;









class CvBuildingProductionAI
{
public:
	CvBuildingProductionAI(CvCity* pCity, CvCityBuildings* pCityBuildings);
	~CvBuildingProductionAI(void);


	void Reset();


	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);


	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight);
	int GetWeight(BuildingTypes eBuilding);


	BuildingTypes RecommendBuilding();


	void LogPossibleBuilds();

private:


	CvCity* m_pCity;
	CvCityBuildings* m_pCityBuildings;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_BUILDINGS, true> m_BuildingAIWeights;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_BUILDINGS, true> m_Buildables;
};

#endif
