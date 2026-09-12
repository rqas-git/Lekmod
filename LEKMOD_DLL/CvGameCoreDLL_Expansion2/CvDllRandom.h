






#pragma once
#include "CvDllInterfaces.h"

class CvDllRandom : public ICvRandom1
{
public:
	CvDllRandom(_In_ CvRandom* pRandom);
	~CvDllRandom();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvRandom* GetInstance();

	void DLLCALL Init(unsigned long ulSeed);
	void DLLCALL Reset(unsigned long ulSeed = 0);

	void DLLCALL CopyFrom(ICvRandom1* pOther);

	unsigned short DLLCALL Get(unsigned short usNum, const char* pszLog = NULL);
	float DLLCALL GetFloat();

	unsigned long DLLCALL GetSeed() const;

	void DLLCALL Read(FDataStream& kStream);
	void DLLCALL Write(FDataStream& kStream) const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvRandom* m_pRandom;
};
