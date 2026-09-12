






#pragma once

#ifndef CIV5_PROJECT_PRODUCTION_AI_H
#define CIV5_PROJECT_PRODUCTION_AI_H




#define SAFE_ESTIMATE_NUM_PROJECTS 18

class CvCity;









class CvProjectProductionAI
{
public:
	CvProjectProductionAI(CvCity* pCity);
	~CvProjectProductionAI(void);


	void Reset();


	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;


	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight);
	int GetWeight(ProjectTypes eProject);


	ProjectTypes RecommendProject();


	void LogPossibleBuilds();

private:


	CvCity* m_pCity;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_PROJECTS, true> m_ProjectAIWeights;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_PROJECTS, true> m_Buildables;
};

#endif
