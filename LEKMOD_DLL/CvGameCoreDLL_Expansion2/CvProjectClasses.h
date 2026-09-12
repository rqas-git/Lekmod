






#pragma once

#ifndef CIV5_PROJECT_CLASSES_H
#define CIV5_PROJECT_CLASSES_H










class CvProjectEntry: public CvBaseInfo
{
public:
	CvProjectEntry(void);
	~CvProjectEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	int GetVictoryPrereq() const;
	int GetTechPrereq() const;
	int GetAnyoneProjectPrereq() const;
	void SetAnyoneProjectPrereq(int i);
	int GetMaxGlobalInstances() const;
	int GetMaxTeamInstances() const;
	int GetProductionCost() const;
	int GetNukeInterception() const;
	int GetCultureBranchesRequired() const;
	int GetTechShare() const;
	int GetEveryoneSpecialUnit() const;
	int GetVictoryDelayPercent() const;
	int GetFlavorValue(int i) const;
	bool IsSpaceship() const;
	bool IsAllowsNukes() const;
	const char* GetMovieArtDef() const;

	const char* GetCreateSound() const;
	void SetCreateSound(const char* szVal);


#ifdef AUI_WARNING_FIXES
	int GetResourceQuantityRequirement(uint i) const;
	int GetVictoryThreshold(uint i) const;
	int GetVictoryMinThreshold(uint i) const;
	int GetProjectsNeeded(uint i) const;
#else
	int GetResourceQuantityRequirement(int i) const;
	int GetVictoryThreshold(int i) const;
	int GetVictoryMinThreshold(int i) const;
	int GetProjectsNeeded(int i) const;
#endif

protected:
	int m_iVictoryPrereq;
	int m_iTechPrereq;
	int m_iAnyoneProjectPrereq;
	int m_iMaxGlobalInstances;
	int m_iMaxTeamInstances;
	int m_iProductionCost;
	int m_iNukeInterception;
	int m_iCultureBranchesRequired;
	int m_iTechShare;
	int m_iEveryoneSpecialUnit;
	int m_iVictoryDelayPercent;

	bool m_bSpaceship;
	bool m_bAllowsNukes;

	CvString m_strCreateSound;
	CvString m_strMovieArtDef;


	int* m_piResourceQuantityRequirements;
	int* m_piVictoryThreshold;
	int* m_piVictoryMinThreshold;
	int* m_piProjectsNeeded;
	int* m_piFlavorValue;
};












class CvProjectXMLEntries
{
public:
	CvProjectXMLEntries(void);
	~CvProjectXMLEntries(void);


	std::vector<CvProjectEntry*>& GetProjectEntries();
#ifdef AUI_WARNING_FIXES
	uint GetNumProjects() const;
	_Ret_maybenull_ CvProjectEntry* GetEntry(uint index);
#else
	int GetNumProjects();
	_Ret_maybenull_ CvProjectEntry* GetEntry(int index);
#endif

	void DeleteArray();

private:
	std::vector<CvProjectEntry*> m_paProjectEntries;
};

#endif
