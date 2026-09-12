






#pragma once
#include "CvDllInterfaces.h"

class CvDllGameDeals : public ICvGameDeals1
{
public:
	CvDllGameDeals(_In_ CvGameDeals* pGameDeals);
	~CvDllGameDeals();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvGameDeals* GetInstance();

	void DLLCALL AddProposedDeal(ICvDeal1* pDeal);
	bool DLLCALL FinalizeDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, bool bAccepted);

	ICvDeal1* DLLCALL GetTempDeal();
	void DLLCALL SetTempDeal(ICvDeal1* pDeal);

	PlayerTypes DLLCALL HasMadeProposal(PlayerTypes eFromPlayer);
	bool DLLCALL ProposedDealExists(PlayerTypes eFromPlayer, PlayerTypes eToPlayer);
	ICvDeal1* DLLCALL GetProposedDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer);

	ICvDeal1* DLLCALL GetCurrentDeal(PlayerTypes ePlayer, unsigned int index);
	ICvDeal1* DLLCALL GetHistoricDeal(PlayerTypes ePlayer, unsigned int indx);
	unsigned int DLLCALL GetNumCurrentDeals(PlayerTypes ePlayer);
	unsigned int DLLCALL GetNumHistoricDeals(PlayerTypes ePlayer);

	unsigned int DLLCALL CreateDeal();
	ICvDeal1* DLLCALL GetDeal(unsigned int index);
	void DLLCALL DestroyDeal(unsigned int index);

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvGameDeals* m_pGameDeals;
};
