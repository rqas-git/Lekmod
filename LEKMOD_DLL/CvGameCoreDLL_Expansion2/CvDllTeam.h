






#pragma once
#include "CvDllInterfaces.h"

class CvDllTeam : public ICvTeam1
{
public:
	CvDllTeam(_In_ CvTeam* pTeam);
	~CvDllTeam();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvTeam* GetInstance();

	bool DLLCALL CanEmbarkAllWaterPassage() const;
	int DLLCALL GetAtWarCount(bool bIgnoreMinors) const;
	EraTypes DLLCALL GetCurrentEra() const;
	PlayerTypes DLLCALL GetLeaderID() const;
	int DLLCALL GetProjectCount(ProjectTypes eIndex) const;
	int DLLCALL GetTotalSecuredVotes() const;
	void DLLCALL Init(TeamTypes eID);
	bool DLLCALL IsAlive() const;
	bool DLLCALL IsAtWar(TeamTypes eIndex) const;
	bool DLLCALL IsBarbarian() const;
	bool DLLCALL IsBridgeBuilding() const;
	bool DLLCALL IsHasMet(TeamTypes eIndex) const;
	bool DLLCALL IsHomeOfUnitedNations() const;
	void DLLCALL Uninit();
	void DLLCALL Read(FDataStream& kStream);
	void DLLCALL Write(FDataStream& kStream) const;


	virtual int DLLCALL GetTechCount() const;

	virtual int DLLCALL GetTechs(TechTypes* pkTechArray, uint uiArraySize) const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvTeam* m_pTeam;
};
