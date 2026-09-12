#ifndef _INCLUDED_FCallStack_H
#define _INCLUDED_FCallStack_H

#include "FSTLBlockAllocator.h"
#include <map>
#include <vector>

#ifdef _WINPC
typedef unsigned long long FCallStackAddressEntry;
typedef unsigned long long FCallStackHashEntry;
#else
typedef UINT64 FCallStackAddressEntry;
typedef UINT64 FCallStackHashEntry;
#endif


class FCallStack : public std::vector<FCallStackAddressEntry, FSTLBlockAllocator<FCallStackAddressEntry, 1024> >
{
public:
	unsigned int m_uiReferences;
	FCallStack() : m_uiReferences(0) {};
	std::string toString(bool stripPaths, int iCSVIndent = 0) const;
};


typedef std::pair <const FCallStackHashEntry, FCallStack*> FCallStackMapEntry;
typedef std::map<FCallStackHashEntry, FCallStack*, std::less<const FCallStackHashEntry>, FSTLBlockAllocator<FCallStackMapEntry, 1024> > FCallStackMap;

#endif
