






#pragma once

#ifndef CIV5_PROCESS_PRODUCTION_AI_H
#define CIV5_PROCESS_PRODUCTION_AI_H

#define SAFE_ESTIMATE_NUM_PROCESSES 4

class CvCity;









class CvProcessProductionAI
{
public:
	CvProcessProductionAI(CvCity* pCity);
	~CvProcessProductionAI(void);


	void Reset();


	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;


	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight);
	int GetWeight(ProcessTypes eProject);


	void LogPossibleBuilds();

private:


	CvCity* m_pCity;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_PROCESSES, true> m_ProcessAIWeights;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_PROCESSES, true> m_Buildables;
};

#endif
