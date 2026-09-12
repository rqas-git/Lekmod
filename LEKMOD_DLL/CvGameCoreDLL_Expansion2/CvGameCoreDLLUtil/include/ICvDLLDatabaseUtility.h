#pragma once

#include "CvDllInterfaces.h"





class ICvDLLDatabaseUtility1 : public ICvUnknown
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvDLLDatabaseUtility1; }




	virtual bool DLLCALL CacheGameDatabaseData() = 0;



	virtual bool DLLCALL FlushGameDatabaseData() = 0;




	virtual bool DLLCALL PerformDatabasePostProcessing() = 0;

};
