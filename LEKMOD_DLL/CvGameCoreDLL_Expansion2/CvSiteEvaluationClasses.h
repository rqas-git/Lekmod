






#pragma once

#ifndef CIV5_SITE_EVALUATION_CLASSES_H
#define CIV5_SITE_EVALUATION_CLASSES_H

#define NUM_CITY_RINGS 3

enum SiteEvaluationFactors
{
    SITE_EVALUATION_HAPPINESS = NUM_YIELD_TYPES,
    SITE_EVALUATION_RESOURCES,
    SITE_EVALUATION_STRATEGIC,
    NUM_SITE_EVALUATION_FACTORS
};









class CvCitySiteEvaluator
{
public:
	CvCitySiteEvaluator(void);
	~CvCitySiteEvaluator(void);

	virtual void Init();
	virtual void ComputeFlavorMultipliers(CvPlayer* pPlayer);
	virtual bool CanFound(CvPlot* pPlot, const CvPlayer* pPlayer, bool bTestVisible) const;
	virtual int PlotFoundValue(CvPlot* pPlot, CvPlayer* pPlayer, YieldTypes eYield = NO_YIELD, bool bCoastOnly=true);
	virtual int PlotFertilityValue(CvPlot* pPlot);
	virtual int BestFoundValueForSpecificYield(CvPlayer* pPlayer, YieldTypes eYield);

protected:

	virtual int ComputeFoodValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeHappinessValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeProductionValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeGoldValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeScienceValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeFaithValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeTradeableResourceValue(CvPlot* pPlot, CvPlayer* pPlayer);
	virtual int ComputeStrategicValue(CvPlot* pPlot, CvPlayer* pPlayer, int iPlotsFromCity);

	int m_iFlavorMultiplier[NUM_SITE_EVALUATION_FACTORS];
	int m_iRingModifier[NUM_CITY_RINGS+NUM_CITY_RINGS+2];

	int m_iExpansionIndex;
	int m_iGrowthIndex;
	int m_iNavalIndex;

	int m_iBrazilMultiplier;
	int m_iSpainMultiplier;
	int m_iMorrocoMultiplier;
	int m_iNetherlandsMultiplier;
	int m_iIncaMultiplier;
};









class CvSiteEvaluatorForSettler: public CvCitySiteEvaluator
{
public:
	CvSiteEvaluatorForSettler(void);
	~CvSiteEvaluatorForSettler(void);

	int PlotFoundValue(CvPlot* pPlot, CvPlayer* pPlayer, YieldTypes eYield = NO_YIELD, bool bCoastOnly=true);
};









class CvSiteEvaluatorForStart: public CvCitySiteEvaluator
{
public:
	CvSiteEvaluatorForStart(void);
	~CvSiteEvaluatorForStart(void);

	virtual void ComputeFlavorMultipliers(CvPlayer* pPlayer);
	int PlotFoundValue(CvPlot* pPlot, CvPlayer* pPlayer, YieldTypes eYield = NO_YIELD, bool bCoastOnly=true);
};

#endif
