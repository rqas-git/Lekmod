






#pragma once

#ifndef CIV5_BUILDER_TASKING_AI_H
#define CIV5_BUILDER_TASKING_AI_H

#define SAFE_ESTIMATE_NUM_EXTRA_PLOTS 64

class CvPlayer;






struct BuilderDirective
{
	typedef enum BuilderDirectiveType
	{
	    BUILD_IMPROVEMENT_ON_RESOURCE,
	    BUILD_IMPROVEMENT,
	    BUILD_ROUTE,
	    REPAIR,
	    CHOP,
	    REMOVE_ROAD,
	    NUM_DIRECTIVES
#ifdef AUI_WARNING_FIXES
	} _BuilderDirectiveType;
#else
	};
#endif

	BuilderDirective() :
		m_eDirective(NUM_DIRECTIVES)
		, m_eBuild(NO_BUILD)
		, m_eResource(NO_RESOURCE)
		, m_sX(-1)
		, m_sY(-1)
		, m_sMoveTurnsAway(-1)
	{
	}

	BuilderDirectiveType m_eDirective;

	BuildTypes m_eBuild;
	ResourceTypes m_eResource;
#ifdef AUI_WARNING_FIXES
	int m_sX;
	int m_sY;
#else
	short m_sX;
	short m_sY;
#endif

#ifdef AUI_WARNING_FIXES
	int m_sMoveTurnsAway;
#else
	short m_sMoveTurnsAway;
#endif
};







class CvBuilderTaskingAI
{
public:
	CvBuilderTaskingAI(void);
	~CvBuilderTaskingAI(void);

	void Init(CvPlayer* pPlayer);
	void Uninit(void);


	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	void Update(void);
	void UpdateRoutePlots(void);
	static void ClearBuildTypeCache();
	static void CacheBuildTypes();

	bool EvaluateBuilder(CvUnit* pUnit, BuilderDirective* paDirectives, UINT uaDirectives, bool bKeepOnlyBest = false, bool bOnlyEvaluateWorkersPlot = false);

	void AddImprovingResourcesDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);
	void AddImprovingPlotsDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);
#ifdef AUI_WORKER_ADD_IMPROVING_MINOR_PLOTS_DIRECTIVES
	void AddImprovingMinorPlotsDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);
#endif
	void AddRouteDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);
	void AddRepairDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);
	void AddChopDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);
	void AddRemoveUselessRoadDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);
	void AddScrubFalloutDirectives(CvUnit* pUnit, CvPlot* pPlot, int iMoveTurnsAway);

	bool ShouldBuilderConsiderPlot(CvUnit* pUnit, CvPlot* pPlot);
	int FindTurnsAway(CvUnit* pUnit, CvPlot* pPlot);

	int GetBuildCostWeight(int iWeight, CvPlot* pPlot, BuildTypes eBuild);
	int GetBuildTimeWeight(CvUnit* pUnit, CvPlot* pPlot, BuildTypes eBuild, bool bIgnoreFeatureTime = false, int iAdditionalTime = 0);
	int GetResourceWeight(ResourceTypes eResource, ImprovementTypes eImprovement, int iQuantity);
#ifndef NQM_PRUNING
	bool IsImprovementBeneficial(CvPlot* pPlot, const CvBuildInfo& kBuild, YieldTypes eYield, bool bIsBreakEvenOK = false);
#endif

	CvCity* GetWorkingCity(CvPlot* pPlot);
	bool DoesBuildHelpRush(CvUnit* pUnit, CvPlot* pPlot, BuildTypes eBuild);

#ifdef AUI_WORKER_SCORE_PLOT_CHOP
	int ScorePlot(BuildTypes eBuild) const;
#elif defined(AUI_CONSTIFY)
	int ScorePlot() const;
#else
	int ScorePlot();
#endif

#ifdef AUI_CONSTIFY
	BuildTypes GetBuildTypeFromImprovement(ImprovementTypes eImprovement) const;
	BuildTypes GetRepairBuild() const;
	FeatureTypes GetFalloutFeature() const;
	BuildTypes GetFalloutRemove() const;
#else
	BuildTypes GetBuildTypeFromImprovement(ImprovementTypes eImprovement);

	BuildTypes GetRepairBuild(void);
	FeatureTypes GetFalloutFeature(void);
	BuildTypes GetFalloutRemove(void);
#endif

#ifdef AUI_WARNING_FIXES
	static void LogInfo(const CvString& str, CvPlayer* pPlayer, bool bWriteToOutput = false);
	static void LogYieldInfo(const CvString& strNewLogStr, CvPlayer* pPlayer);
#else
	static void LogInfo(CvString str, CvPlayer* pPlayer, bool bWriteToOutput = false);
	static void LogYieldInfo(CvString strNewLogStr, CvPlayer* pPlayer);
#endif

	static CvWeightedVector<BuilderDirective, 100, true> m_aDirectives;
	static FStaticVector<int, SAFE_ESTIMATE_NUM_EXTRA_PLOTS, true, c_eCiv5GameplayDLL, 0> m_aiNonTerritoryPlots;


protected:

	void LogFlavors(FlavorTypes eFlavor);
	void LogDirectives(CvUnit* pUnit);
	void LogDirective(BuilderDirective directive, CvUnit* pUnit, int iWeight, bool bChosen = false);

	void ConnectCitiesToCapital(CvCity* pPlayerCapital, CvCity* pTargetCity, RouteTypes eRoute);
	void ConnectCitiesForScenario(CvCity* pFirstCity, CvCity* pSecondCity, RouteTypes eRoute);

	void UpdateCurrentPlotYields(CvPlot* pPlot);
	void UpdateProjectedPlotYields(CvPlot* pPlot, BuildTypes eBuild);

	CvPlayer* m_pPlayer;
	BuildTypes m_eRepairBuild;
	CvPlotsVector m_aiPlots;
	bool m_bLogging;
	int m_iNumCities;

	CvPlot* m_pTargetPlot;
	int m_aiCurrentPlotYields[NUM_YIELD_TYPES];
	int m_aiProjectedPlotYields[NUM_YIELD_TYPES];

	FeatureTypes m_eFalloutFeature;
	BuildTypes m_eFalloutRemove;

#ifndef AUI_WORKER_UNHARDCODE_NO_REMOVE_FEATURE_THAT_IS_REQUIRED_FOR_UNIQUE_IMPROVEMENT
	bool m_bKeepMarshes;
	bool m_bKeepJungle;
#endif
};

#endif
