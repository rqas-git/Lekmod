






#pragma once

#ifndef CIV5_EMPHASIS_CLASSES_H
#define CIV5_EMPHASIS_CLASSES_H










class CvEmphasisEntry: public CvBaseInfo
{
public:
	CvEmphasisEntry(void);
	~CvEmphasisEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

public:
	bool IsAvoidGrowth() const;
	bool IsGreatPeople() const;


	int GetYieldChange(int i) const;

protected:

	bool m_bAvoidGrowth;
	bool m_bGreatPeople;


	int* m_piYieldModifiers;

private:
	CvEmphasisEntry(const CvEmphasisEntry&);
	CvEmphasisEntry& operator=(const CvEmphasisEntry&);
};












class CvEmphasisXMLEntries
{
public:
	CvEmphasisXMLEntries(void);
	~CvEmphasisXMLEntries(void);


	std::vector<CvEmphasisEntry*>& GetEmphasisEntries();
#ifdef AUI_WARNING_FIXES
	uint GetNumEmphases() const;
	_Ret_maybenull_ CvEmphasisEntry* GetEntry(uint index);
#else
	int GetNumEmphases();
	_Ret_maybenull_ CvEmphasisEntry* GetEntry(int index);
#endif

	void DeleteArray();

private:
	std::vector<CvEmphasisEntry*> m_paEmphasisEntries;
};









class CvCityEmphases
{
public:
#ifdef AUI_CITY_FIX_COMPONENT_CONSTRUCTORS_CONTAIN_POINTERS
	CvCityEmphases(CvCity* pCity);
#else
	CvCityEmphases(void);
#endif
	~CvCityEmphases(void);
	void Init(CvEmphasisXMLEntries* pEmphases, CvCity* pCity);
	void Uninit();
	void Reset();


	int GetEmphasizeAvoidGrowthCount();
	bool IsEmphasizeAvoidGrowth();
	int GetEmphasizeYieldCount(YieldTypes eIndex);
	bool IsEmphasizeYield(YieldTypes eIndex);
	bool IsEmphasize(EmphasizeTypes eIndex);
	void SetEmphasize(EmphasizeTypes eIndex, bool bNewValue);


	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

private:
	int m_iEmphasizeAvoidGrowthCount;
	int m_iEmphasizeGreatPeopleCount;
	int m_aiEmphasizeYieldCount[NUM_YIELD_TYPES];
	bool* m_pbEmphasize;

	CvEmphasisXMLEntries* m_pEmphases;
	CvCity* m_pCity;
};

#endif
