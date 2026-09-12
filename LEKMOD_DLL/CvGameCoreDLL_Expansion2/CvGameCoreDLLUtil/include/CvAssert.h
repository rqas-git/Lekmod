










#ifndef _CVASSERT_H
#define _CVASSERT_H
#pragma once
#include <set>

bool CvAssertDlg(const char* expr, const char* szFile, unsigned int uiLine, bool& bIgnoreAlways, const char* msg);


#if defined (_WIN32)
#define CVASSERT_BREAKPOINT __debugbreak()
#else
#define CVASSERT_BREAKPOINT assert(0)
#endif




#if !defined(FINAL_RELEASE)
#define		CVASSERT_ENABLE
#endif


#ifdef CVASSERT_ENABLE 

#define CvAssertMsg(expr, msg)																\
{																							\
	static bool bIgnoreAlways = false;														\
	if( !bIgnoreAlways && !(expr) )								                							\
	{																						\
		if(CvAssertDlg(#expr, __FILE__, __LINE__, bIgnoreAlways, msg))						\
			{ CVASSERT_BREAKPOINT; }														\
	}																						\
}

#define CvAssertFmt(expr, fmt, ...)															\
{																							\
	static bool bIgnoreAlways = false;														\
	if( !bIgnoreAlways && !(expr) )															\
	{																						\
		CvString str;																		\
		CvString::format(str, fmt, __VA_ARGS__);											\
		if(CvAssertDlg(#expr, __FILE__, __LINE__,											\
			bIgnoreAlways, str.c_str()))													\
				{ CVASSERT_BREAKPOINT; }													\
	}																						\
}

#define CvAssert( expr ) CvAssertMsg(expr, "")


#ifdef _DEBUG
#define CvAssert_Debug( expr ) CvAssertMsg(expr, "")
#define CvAssertMsg_Debug(expr, msg) CvAssertMsg(expr, msg)
#else
#define CvAssert_Debug(expr)
#define CvAssertMsg_Debug(expr, msg)
#endif

#if 0
#include "FSTLBlockAllocator.h"

template<typename ObjectType, unsigned int pool_type>
struct ObjectValidator
{
	std::set<const ObjectType *, std::less<const ObjectType *>, FSTLBlockAllocator<ObjectType,1024,pool_type> > m_deletedObjects;
	void objectAllocated(const ObjectType * obj)
	{
		m_deletedObjects.erase(obj);
	}
	void objectDestroyed(const ObjectType * obj)
	{
		m_deletedObjects.insert(obj);
	}
	bool isObjectValid(const ObjectType * obj)
	{
		bool result = (obj != 0);
		if(result) 
			result = (m_deletedObjects.find(obj) == m_deletedObjects.end());
		CvAssertMsg(result, "Object is invalid in this context");
		return result;
	}
};

#define OBJECT_VALIDATE_DEFINITION(ObjectType) ObjectValidator<ObjectType, c_eDebugSafetyNet> s_objectValidator;

#define OBJECT_ALLOCATED s_objectValidator.objectAllocated(this);;
#define OBJECT_DESTROYED s_objectValidator.objectDestroyed(this);
#define VALIDATE_OBJECT s_objectValidator.isObjectValid(this);
#else
#define OBJECT_VALIDATE_DEFINITION(ObjectType)
#define OBJECT_ALLOCATED
#define OBJECT_DESTROYED
#define VALIDATE_OBJECT
#endif

#else

#define CvAssertFmt(expr, fmt, ...)
#define CvAssertMsg(expr, msg)
#define CvAssert(expr)
#define CvAssert_Debug(expr)
#define CvAssertMsg_Debug(expr, msg)

#define OBJECT_VALIDATE_DEFINITION(ObjectType)
#define OBJECT_ALLOCATED
#define OBJECT_DESTROYED
#define VALIDATE_OBJECT

#endif


#endif
