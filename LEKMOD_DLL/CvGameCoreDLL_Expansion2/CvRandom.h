






#pragma once



#ifndef CIV5_RANDOM_H
#define CIV5_RANDOM_H

#ifdef _DEBUG
#include <vector>
#endif

#ifdef AUI_USE_SFMT_RNG
#include "SFMT\SFMT.h"
#endif

class CvRandom
{

public:
	explicit CvRandom(bool extendedCallStackDebugging);
	CvRandom();
	CvRandom(const CvRandom& source);
	virtual ~CvRandom();

#ifdef AUI_USE_SFMT_RNG
	void init(uint32_t ulSeed);
	void uninit();
	void reset(uint32_t ulSeed = 0);
#else
	void init(unsigned long ulSeed);
	void uninit();
	void reset(unsigned long ulSeed = 0);
#endif

#if defined(AUI_USE_SFMT_RNG) || defined(AUI_WARNING_FIXES)
	unsigned int get(unsigned int uiNum, const char* pszLog = NULL);
#else
	unsigned short get(unsigned short usNum, const char* pszLog = NULL);
#endif

#ifdef AUI_BINOM_RNG
	unsigned int getBinom(unsigned int uiNum, const char* pszLog = NULL);
#endif

	float getFloat();

#ifdef AUI_USE_SFMT_RNG
	void reseed(unsigned int uiNewSeed);
	std::pair<unsigned long, unsigned long> getSeed() const;
#else
	void reseed(unsigned long ulNewValue);
	unsigned long getSeed() const;
#endif
	unsigned long getCallCount() const;
	unsigned long getResetCount() const;


	void read(FDataStream& Stream);
	void write(FDataStream& Stream) const;

	bool operator==(const CvRandom& rhs) const;
	bool operator!=(const CvRandom& rhs) const;
#ifdef AUI_USE_SFMT_RNG
	void syncInternals(const CvRandom& rhs);
#endif


	const std::vector<std::string>& getResolvedCallStacks() const;
	const std::vector<unsigned long>& getSeedHistory() const;
	void resolveCallStacks() const;
	bool callStackDebuggingEnabled() const;
	void setCallStackDebuggingEnabled(bool enabled);
	void clearCallstacks();

protected:
	void recordCallStack();

protected:

#ifdef AUI_USE_SFMT_RNG
	SFMersenneTwister m_MersenneTwister;
#endif
#if defined(LEKMOD_MACOS)


	typedef uint32_t RandomWord;
	RandomWord m_ulRandomSeed;
#else
	unsigned long m_ulRandomSeed;
#endif


#if defined(LEKMOD_MACOS)
	RandomWord m_ulCallCount;
	RandomWord m_ulResetCount;
#else
	unsigned long m_ulCallCount;
	unsigned long m_ulResetCount;
#endif
	bool m_bSynchronous;

#ifdef _DEBUG
	bool m_bExtendedCallStackDebugging;

	mutable std::vector<FCallStack> m_kCallStacks;
	mutable std::vector<unsigned long> m_seedHistory;


	mutable std::vector<std::string>  m_resolvedCallStacks;
#endif

};

FDataStream& operator<<(FDataStream& saveTo, const CvRandom& readFrom);
FDataStream& operator>>(FDataStream& loadFrom, CvRandom& writeTo);
#ifdef AUI_USE_SFMT_RNG
FDataStream& operator<<(FDataStream& saveTo, const SFMersenneTwister& readFrom);
FDataStream& operator>>(FDataStream& loadFrom, SFMersenneTwister& writeTo);
#endif
#endif
