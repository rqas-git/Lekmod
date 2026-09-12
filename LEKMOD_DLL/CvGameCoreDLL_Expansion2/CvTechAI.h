






#pragma once

#ifndef CIV5_TECHAI_H
#define CIV5_TECHAI_H




#define SAFE_ESTIMATE_NUM_TECHS 80



#define TECH_COST_WEIGHTING_MULTIPLIER 100









class CvTechAI
{
public:
	CvTechAI(CvPlayerTechs* currentTechs);
	~CvTechAI(void);


	void Reset();


	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;


	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight, int iPropagationPercent);


	TechTypes ChooseNextTech(CvPlayer *pPlayer, bool bFreeTech = false);
	TechTypes RecommendNextTech(CvPlayer *pPlayer, TechTypes eIgnoreTech = NO_TECH);

	int GetWeight(TechTypes eTech);

	float GetTechRatio();

private:

#ifdef AUI_WARNING_FIXES
	void WeightPrereqs(FFastVector<int, true> paiTempWeights, int iPropagationPercent);
#else
	void WeightPrereqs(int* paiTempWeights, int iPropagationPercent);
#endif
	void PropagateWeights(int iTech, int iWeight, int iPropagationPercent, int iPropagationLevel);


	void ReweightByCost(CvPlayer *pPlayer);


	void LogPossibleResearch();
	void LogResearchChoice(TechTypes eTech);
	CvString GetLogFileName(CvString& playerName) const;


	CvPlayerTechs* m_pCurrentTechs;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_TECHS, true> m_TechAIWeights;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_TECHS, true> m_ResearchableTechs;
};

#endif
