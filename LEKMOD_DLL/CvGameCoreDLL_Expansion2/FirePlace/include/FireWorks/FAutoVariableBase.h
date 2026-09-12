













#ifndef _INCLUDED_FAutoVariableBase_H
#define _INCLUDED_FAutoVariableBase_H

#ifndef FINAL_RELEASE
#	include "FCallStack.h"
#endif

#ifdef _WINPC
#	pragma warning ( disable : 4355 )
#endif
#include <vector>

class FAutoArchive;
class FDataStream;
class FAutoVariableBase;

namespace FSerialization
{

	std::string toString(const bool & source);
	std::string toString(const unsigned char & source);
	std::string toString(const char & source);
	std::string toString(const unsigned short & source);
	std::string toString(const short & source);
	std::string toString(const unsigned int & source);
	std::string toString(const int & source);
	std::string toString(const float & source);
	std::string toString(const double & source);
	std::string toString(const std::string & source);
	std::string toString(const FAutoVariableBase &);
	template<typename ValueType>
	std::string toString(const std::vector<ValueType> & source)
	{
		std::string result = "{";
#if defined(LEKMOD_MACOS)
		typename std::vector<ValueType>::const_iterator i;
#else
		std::vector<ValueType>::const_iterator i;
#endif
		for(i = source.begin(); i != source.end(); ++i)
		{
			if(i != source.begin())
				result += std::string(",");
			result += toString(*i);
		}
		return result;
	}






	template<typename ValueType>
	std::string toString(const ValueType &)
	{
		static const std::string result("UNKNOWN");
		return result;
	}
}

class FAutoVariableBase
{
public:
	FAutoVariableBase(const std::string & name, FAutoArchive & owner);


	FAutoVariableBase(const std::string & name, FAutoArchive & owner, bool callStackTracking);

	virtual ~FAutoVariableBase() = 0;
	virtual void load(FDataStream & loadFrom) = 0;
	virtual void loadDelta(FDataStream & loadFrom) = 0;
	virtual void save(FDataStream & saveTo) const = 0;
	virtual void saveDelta(FDataStream & saveTo) const = 0;
	virtual void clearDelta() = 0;
	virtual bool compare(FDataStream & otherValue) const = 0;
	virtual void reset() = 0;

	virtual const std::string & name() const = 0;


	std::string  getStackTrace() const;
	std::string  getStackTraceRemark() const;
	virtual void setStackTraceRemark() = 0;
	virtual std::string  debugDump(const std::vector<std::pair<std::string, std::string> > &) const;
	virtual std::string toString() const = 0;

protected:
#ifndef FINAL_RELEASE
	friend class FAutoArchive;
	FCallStack   m_lastCallStackToChangeThisVariable;
	std::string  m_callStackRemark;
	bool         m_callStackTracking;
#endif
private:

	FAutoVariableBase(const FAutoVariableBase &);
	FAutoVariableBase & operator=(const FAutoVariableBase &);

private:

};

#endif
