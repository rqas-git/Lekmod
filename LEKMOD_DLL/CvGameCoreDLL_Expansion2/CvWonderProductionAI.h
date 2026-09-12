






#pragma once

#ifndef CIV5_WONDER_PRODUCTION_AI_H
#define CIV5_WONDER_PRODUCTION_AI_H




#define SAFE_ESTIMATE_NUM_WONDERS 50









class CvWonderProductionAI: public CvFlavorRecipient
{
public:
	CvWonderProductionAI(CvPlayer* pPlayer, CvBuildingXMLEntries* pBuildings);
	~CvWonderProductionAI(void);
	void Init(CvBuildingXMLEntries* pBuildings, CvPlayer* pPlayer, bool bIsCity);


	void Reset();


	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;


	void FlavorUpdate();


	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight);
	int GetWeight(BuildingTypes eBuilding);


	BuildingTypes ChooseWonder(bool bUseAsyncRandom, bool bAdjustForOtherPlayers, int& iWonderWeight);
	BuildingTypes ChooseWonderForGreatEngineer(bool bUseAsyncRandom, int& iWonderWeight, CvCity*& pCityToBuildAt);


	void LogFlavors(FlavorTypes eFlavor = NO_FLAVOR);
	void LogPossibleWonders();

	bool IsWonder(const CvBuildingEntry& kBuilding) const;

private:
	bool HaveCityToBuild(BuildingTypes eBuilding) const;


	CvPlayer* m_pPlayer;
	CvBuildingXMLEntries* m_pBuildings;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_BUILDINGS, true> m_WonderAIWeights;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_WONDERS, true> m_Buildables;
};

#endif
