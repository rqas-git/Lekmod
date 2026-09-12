














#ifndef _INCLUDED_FAutoArchiveClassContainer_H
#define _INCLUDED_FAutoArchiveClassContainer_H



#include "FAutoArchive.h"





template<class ClassType>
class FAutoArchiveClassContainer : public FAutoArchive
{
public:
	FAutoArchiveClassContainer(ClassType & owner) :
	FAutoArchive()
	, m_classInstance(owner)
	{
	}

	ClassType & owner()
	{
		return m_classInstance;
	}

	const ClassType & owner() const
	{
		return m_classInstance;
	}

	virtual const std::string * getVariableName(const FAutoVariableBase & var) const
	{
		const std::string * result = 0;

		size_t key = std::distance(m_contents.begin(), std::find(m_contents.begin(), m_contents.end(), &var));
		std::map<size_t, std::string> & names = getNames();
		std::map<size_t, std::string>::const_iterator f = names.find(key);
		
		if(f != names.end())
		{
			result = &f->second;
		}
		return result;
	}

	virtual void setVariableName(const FAutoVariableBase & var, const std::string & name) const
	{
		std::map<size_t, std::string> & names = getNames();

		size_t offset = std::distance(m_contents.begin(), std::find(m_contents.begin(), m_contents.end(), &var));
		std::map<size_t, std::string>::const_iterator f = names.find(offset);
		if(f == names.end())
		{
			names[offset] = name;
		}
	}

	virtual std::string debugDump(const FAutoVariableBase & var) const
	{
		return std::string("\n") + m_classInstance.debugDump(var);
	}

	virtual std::string stackTraceRemark(const FAutoVariableBase & var) const
	{
		return m_classInstance.stackTraceRemark(var);
	}

protected:
	std::map<size_t, std::string> & getNames() const;

private:
	ClassType & m_classInstance;
};






template<class ClassType>
std::map<size_t, std::string> & FAutoArchiveClassContainer<ClassType>::getNames() const
{
	static std::map<size_t, std::string> names;
	return names;
}



template<class ClassType>
FDataStream & operator<<(FDataStream & saveTo, const FAutoArchiveClassContainer<ClassType> & readFrom)
{
	readFrom.save(saveTo);
	return saveTo;
}



template<class ClassType>
FDataStream & operator>>(FDataStream & loadFrom, FAutoArchiveClassContainer<ClassType> & writeTo)
{
	writeTo.load(loadFrom);
	return loadFrom;
}



#endif
