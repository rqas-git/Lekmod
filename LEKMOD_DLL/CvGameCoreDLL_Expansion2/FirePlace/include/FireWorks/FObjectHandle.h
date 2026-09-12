














#ifndef _INCLUDED_FObjectHandle_H
#define _INCLUDED_FObjectHandle_H






#if defined(TRACK_HANDLE_STACKS)
#	define OBJECT_HANDLE_STACK recordCallstack()
#	ifdef _WINPC
#		include "Win32/FDebugHelper.h"
#	endif
#else
#	define OBJECT_HANDLE_STACK
#endif

template<typename NotificationTarget>
struct DestructionNotification
{
	~DestructionNotification()
	{
#if defined(LEKMOD_MACOS)
		typename std::set<const NotificationTarget *>::const_iterator i;
#else
		std::set<const NotificationTarget *>::const_iterator i;
#endif
		for(i = m_targets.begin(); i != m_targets.end(); ++i)
		{
			const NotificationTarget * t = *i;
			t->removeTarget();
		}
	}

#if defined(AUI_WARNING_FIXES) && !defined(TRACK_DESTRUCTION_NOTIFICATION)
	void addTarget(const NotificationTarget &           )
#else
	void addTarget(const NotificationTarget & target)
#endif
	{
#ifdef TRACK_DESTRUCTION_NOTIFICATION
		m_targets.insert(&target);
#endif
	}

#if defined(AUI_WARNING_FIXES) && !defined(TRACK_DESTRUCTION_NOTIFICATION)
	void removeTarget(const NotificationTarget &           )
#else
	void removeTarget(const NotificationTarget & target)
#endif
	{
#ifdef TRACK_DESTRUCTION_NOTIFICATION
		m_targets.erase(&target);
#endif
	}

	std::set<const NotificationTarget *> m_targets;
};

template<class PointingTo>
class FObjectHandle
{
public:

	PointingTo * pointer()
	{
		return m_target;
	}

	PointingTo * pointer() const
	{
		return m_target;
	}

	PointingTo * operator -> ()
	{
		return m_target;
	}

	const PointingTo * operator -> () const
	{
		return m_target;
	}

	PointingTo & operator * ()
	{
		return *m_target;
	}

	const PointingTo & operator * () const
	{
		return *m_target;
	}

	operator bool() const
	{
		return m_target != 0;
	}

	bool operator ! () const
	{
		return m_target == 0;
	}

	bool operator != (const PointingTo * rhs) const
	{
		return m_target != rhs;
	}

	bool operator == (const PointingTo * rhs) const
	{
		return m_target == rhs;
	}

	void ignoreDestruction(bool ignore)
	{
		m_ignoreDestruction = ignore;
	}

	void removeTarget() const
	{
#if defined(TRACK_HANDLE_STACKS)
		std::string ownerCallstackString = m_ownerCallstack.toString(false);
		std::string msg = "An object is being destroyed while a handle for it still exists. The handle is no longer valid. If this is acceptable, invoke handle.ignoreDestruction() before use and this assert will not trigger.\n";
		msg += "Owner Callstack : \n";
		msg += ownerCallstackString;
		FAssertMsg(m_ignoreDestruction, msg.c_str());
#else
		FAssertMsg(m_ignoreDestruction, "An object is being destroyed while a handle for it still exists. The handle is no longer valid. If this is acceptable, invoke handle.ignoreDestruction() before use and this assert will not trigger.");
#endif
		const_cast<FObjectHandle *>(this)->m_target = 0;
	}

	~FObjectHandle()
	{
		if(m_target) 
		{
			m_target->getDestructionNotification().removeTarget(*this);
		}
	}

	FObjectHandle(PointingTo * target) :
	m_target(target)
	, m_ignoreDestruction(false)
	{
		OBJECT_HANDLE_STACK;
		if(m_target)
		{
			m_target->getDestructionNotification().addTarget(*this);
		}
	}

	FObjectHandle & operator=(PointingTo * target)
	{
		OBJECT_HANDLE_STACK;
		if(m_target)
		{
			m_target->getDestructionNotification().removeTarget(*this);
		}
		m_target = target;
		if(m_target)
		{
			m_target->getDestructionNotification().addTarget(*this);
		}
		return *this;
	}

	const FObjectHandle & operator=(const PointingTo * target) const
	{
		OBJECT_HANDLE_STACK;
		if(m_target)
		{
			const_cast<PointingTo *>(m_target)->getDestructionNotification().removeTarget(*this);
		}
		const_cast<FObjectHandle *>(this)->m_target = const_cast<PointingTo *>(target);
		if(m_target)
		{
			const_cast<PointingTo *>(m_target)->getDestructionNotification().addTarget(*this);
		}
		return *this;
	}

	const FObjectHandle & operator=(const FObjectHandle & rhs) const
	{
		if(&rhs != this)
		{
			OBJECT_HANDLE_STACK;
			if(m_target)
			{
				const_cast<FObjectHandle *>(this)->m_target->getDestructionNotification().removeTarget(*this);
			}
			const_cast<FObjectHandle *>(this)->m_target = rhs.m_target;
			if(m_target)
			{
				const_cast<FObjectHandle *>(this)->m_target->getDestructionNotification().addTarget(*this);
			}
		}
		return *this;
	}

	FObjectHandle() :
	m_target(0)
	, m_ignoreDestruction(false)
	{
		OBJECT_HANDLE_STACK;
	}

	FObjectHandle(const FObjectHandle & source) :
	m_target(source.m_target)
	, m_ignoreDestruction(source.m_ignoreDestruction)
	{
		OBJECT_HANDLE_STACK;
		if(m_target)
		{
			m_target->getDestructionNotification().addTarget(*this);
		}
	}
#if defined(TRACK_HANDLE_STACKS)

private:
	void recordCallstack() const
	{
		FDebugHelper & debugHelper = FDebugHelper::GetInstance();
		debugHelper.GetCallStack(&m_ownerCallstack, 2);
	}

#endif
private:
	PointingTo *  m_target;
	bool          m_ignoreDestruction;
#if defined(TRACK_HANDLE_STACKS)
	mutable FCallStack   m_ownerCallstack;
#endif
};

template<class PointingTo>
const FObjectHandle<PointingTo> ConstHandle(const PointingTo * target) 
{
	FObjectHandle<PointingTo> result;
	result = target;
	return result;
}

#endif
