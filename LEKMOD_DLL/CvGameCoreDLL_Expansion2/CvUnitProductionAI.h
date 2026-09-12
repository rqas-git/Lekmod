






#pragma once

#ifndef CIV5_UNIT_PRODUCTION_AI_H
#define CIV5_UNIT_PRODUCTION_AI_H




#define SAFE_ESTIMATE_NUM_UNITS 90









class CvUnitProductionAI
{
public:
	CvUnitProductionAI(CvCity* pCity, CvUnitXMLEntries* pUnits);
	~CvUnitProductionAI(void);


	void Reset();


	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;


	void AddFlavorWeights(FlavorTypes eFlavor, int iWeight);
	int GetWeight(UnitTypes eUnit);


	UnitTypes RecommendUnit(UnitAITypes eUnitAIType = NO_UNITAI);


	void LogPossibleBuilds(UnitAITypes eUnitAIType = NO_UNITAI);

private:


	CvCity* m_pCity;
	CvUnitXMLEntries* m_pUnits;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_UNITS, true> m_UnitAIWeights;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_UNITS, true> m_Buildables;
};

#endif
