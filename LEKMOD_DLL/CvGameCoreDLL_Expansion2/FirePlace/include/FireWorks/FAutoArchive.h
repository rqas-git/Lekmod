















#ifndef _INCLUDED_FAutoArchive_H
#define _INCLUDED_FAutoArchive_H



#include <set>
#include <vector>



class FDataStream;
class FAutoVariableBase;



#if !defined(FINAL_RELEASE)
#define FAUTOARCHIVE_DEBUG
#endif

class FAutoArchive
{
public:
	FAutoArchive();
	virtual ~FAutoArchive() = 0;
	void add(FAutoVariableBase &);
	void remove(FAutoVariableBase &);

	void load(FDataStream & loadFrom);
	void save(FDataStream & saveTo) const;

	void saveDelta(FDataStream & saveTo, std::vector<std::pair<std::string, std::string> > & callStacks) const;
	void loadDelta(FDataStream & loadFrom);
	void clearDelta();

	void reset();

	bool hasDeltas() const;
	__forceinline void touch(FAutoVariableBase & dirtyVariable)
	{
#ifdef FAUTOARCHIVE_DEBUG
		debugHelp(dirtyVariable);
#endif
		m_deltas.insert(&dirtyVariable);
	}


	virtual const std::string * getVariableName(const FAutoVariableBase &) const = 0;
	virtual void setVariableName(const FAutoVariableBase &, const std::string &) const = 0;
	virtual std::string debugDump(const FAutoVariableBase &) const = 0;
	virtual std::string stackTraceRemark(const FAutoVariableBase &) const = 0;

	std::vector<const FAutoVariableBase *> findMismatchedVariables(FDataStream &) const;
	const FAutoVariableBase * findVariable(const std::string & name) const;


	static	void EnableCallStacks(bool bState);
	static	bool AreCallStacksEnabled();
protected:
#ifdef FAUTOARCHIVE_DEBUG
	void debugHelp(FAutoVariableBase & dirtyVariable);
#endif
	std::vector<FAutoVariableBase *>       m_contents;
private:
	mutable std::set<FAutoVariableBase *>  m_deltas;
};



FDataStream & operator<<(FDataStream & saveTo, const FAutoArchive & readFrom);
FDataStream & operator>>(FDataStream & loadFrom, FAutoArchive & writeTo);



#endif
