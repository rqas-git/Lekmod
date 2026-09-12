#ifndef _FIGAMEINIPARSER_H
#define _FIGAMEINIPARSER_H

#include "FFileDefines.h"


class FIGameIniParser
{
public:

	virtual ~FIGameIniParser() {}

	virtual bool GetKeyValue(LPCTSTR szGroupKey, LPCTSTR szKeyValue, FString* pValue, LPCTSTR szKeyDesc=NULL, const char* keyDefaultVal=NULL) = 0;
	virtual bool GetKeyValue(LPCTSTR szGroupKey, LPCTSTR szKeyValue, int* pValue, LPCTSTR szKeyDesc=NULL, int keyDefaultVal = 0) = 0;
	virtual bool GetKeyValue(LPCTSTR szGroupKey, LPCTSTR szKeyValue, uint* pValue, LPCTSTR szKeyDesc=NULL, uint keyDefaultVal = 0) = 0;
	virtual bool GetKeyValue(LPCTSTR szGroupKey, LPCTSTR szKeyValue, float* pValue, LPCTSTR szKeyDesc=NULL, float keyDefaultVal = 0.0f) = 0;


	virtual bool Write( void ) = 0;


	virtual bool WriteIfDirty() = 0;
	virtual void EnableAutoWriting( bool bEnable = true ) = 0;

	virtual bool IsDirty() const = 0;

	virtual bool WarnCorrupted() const = 0;

	virtual void ClearCorruptedWarning() = 0;
	virtual FileErr GetLastFileErr() = 0;

	virtual bool IsInitialized() = 0;
};

#endif
