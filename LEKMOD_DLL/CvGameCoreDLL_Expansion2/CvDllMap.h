






#pragma once
#include "CvDllInterfaces.h"

class CvDllMap : public ICvMap1
{
public:
	CvDllMap(_In_ CvMap* pMap);
	~CvDllMap();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvMap* GetInstance();

	void DLLCALL Init(CvMapInitData* pInitData=NULL);
	void DLLCALL Uninit();
	void DLLCALL UpdateSymbolVisibility();
	void DLLCALL UpdateLayout(bool bDebug);
	void DLLCALL UpdateDeferredFog();
	ICvCity1* DLLCALL FindCity(int iX,
	                           int iY,
	                           PlayerTypes eOwner = NO_PLAYER,
	                           TeamTypes eTeam = NO_TEAM,
	                           bool bSameArea = true,
	                           bool bCoastalOnly = false,
	                           TeamTypes eTeamAtWarWith = NO_TEAM,
	                           DirectionTypes eDirection = NO_DIRECTION,
	                           ICvCity1* pSkipCity = NULL);

	int DLLCALL NumPlots() const;
	int DLLCALL PlotNum(int iX, int iY) const;
	int DLLCALL GetGridWidth() const;
	int DLLCALL GetGridHeight() const;
	bool DLLCALL IsWrapX() const;
	bool DLLCALL IsWrapY() const;
	ICvPlot1* DLLCALL GetPlotByIndex(int iIndex) const;
	ICvPlot1* DLLCALL GetPlot(int iX, int iY) const;
	void DLLCALL RecalculateLandmasses();
	void DLLCALL Read(FDataStream& kStream);
	void DLLCALL Write(FDataStream& kStream) const;
	int DLLCALL Validate();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvMap* m_pMap;
};
