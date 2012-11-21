//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:    FObjectHandle.h
//
//  AUTHOR:  Justin Randall  --  4/13/2007
//
//  PURPOSE: Provides pointer-like access to objects in ways that can be easily checked for object
//           validity
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef _INCLUDED_FObjectHandle_H
#define _INCLUDED_FObjectHandle_H

// JAR - uncomment this if a stale handle assert is thrown and you don't know
// where the object handle was allocated
// #define TRACK_HANDLE_STACKS 1
//#define TRACK_DESTRUCTION_NOTIFICATION 1

#if defined(TRACK_HANDLE_STACKS)
#	define OBJECT_HANDLE_STACK recordCallstack()
#	ifdef _WINPC
#		include "Win32/FDebugHelper.h"
#	endif//_WINPC
#else
#	define OBJECT_HANDLE_STACK
#endif

template<typename NotificationTarget>
struct DestructionNotification
{
	~DestructionNotification()
	{
		std::set<const NotificationTarget *>::const_iterator i;
		for(i = m_targets.begin(); i != m_targets.end(); ++i)
		{
			const NotificationTarget * t = *i;
			t->removeTarget();
		}
	}

	void addTarget(const NotificationTarget & target)
	{
#ifdef TRACK_DESTRUCTION_NOTIFICATION
		m_targets.insert(&target);
#endif
	}

	void removeTarget(const NotificationTarget & target)
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
#endif//TRACK_OBJECT_STACKS
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

#endif//TRACK_HANDLE_STACKS
private:
	PointingTo *  m_target;
	bool          m_ignoreDestruction;
#if defined(TRACK_HANDLE_STACKS)
	mutable FCallStack   m_ownerCallstack;
#endif//TRACK_HANDLE_STACKS
};

template<class PointingTo>
const FObjectHandle<PointingTo> ConstHandle(const PointingTo * target) 
{
	FObjectHandle<PointingTo> result;
	result = target;
	return result;
}

#endif//_INCLUDED_FObjectHandle_H
