









#pragma once
#ifndef CIV5_ADVISOR_RECOMMENDER_H
#define CIV5_ADVISOR_RECOMMENDER_H

class CvAdvisorRecommender
{
public:
	CvAdvisorRecommender();
	~CvAdvisorRecommender();

	void Init();
	void Uninit();
	void Reset();


	void ResetCity();
	void UpdateCityRecommendations(CvCity* pCity);
	bool IsUnitRecommended(UnitTypes eUnit, AdvisorTypes eAdvisor);
	bool IsBuildingRecommended(BuildingTypes eBuilding, AdvisorTypes eAdvisor);
	bool IsProjectRecommended(ProjectTypes eProject, AdvisorTypes eAdvisor);


	void ResetTechs();
	void UpdateTechRecommendations(PlayerTypes ePlayer);
	bool IsTechRecommended(TechTypes eTech, AdvisorTypes eAdvisor);

	int AdvisorInterestInFlavor(AdvisorTypes eAdvisor, FlavorTypes eFlavor);
	AdvisorTypes FindUnassignedAdvisorForTech(PlayerTypes ePlayer, TechTypes eTech);
#if defined(AUI_POLICY_BUILDING_CLASS_FLAVOR_MODIFIERS) || defined(AUI_BELIEF_BUILDING_CLASS_FLAVOR_MODIFIERS)
	AdvisorTypes FindUnassignedAdvisorForBuildable(const CvCity* pCity, CvCityBuildable& buildable);
#else
	AdvisorTypes FindUnassignedAdvisorForBuildable(PlayerTypes ePlayer, CvCityBuildable& buildable);
#endif

	CvCityBuildable m_aRecommendedBuilds[NUM_ADVISOR_TYPES];
	TechTypes m_aRecommendedTechs[NUM_ADVISOR_TYPES];

	CvWeightedVector<int, 80, true> m_aResearchableTechs;
	CvWeightedVector<int, NUM_ADVISOR_TYPES, true> m_aFinalRoundTechs;

	CvWeightedVector<CvCityBuildable, (50), true> m_aCityBuildables;
	CvWeightedVector<CvCityBuildable, NUM_ADVISOR_TYPES, true> m_aFinalRoundBuildables;
};

#endif
