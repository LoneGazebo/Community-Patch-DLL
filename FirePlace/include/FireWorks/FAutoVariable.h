//---------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:		FAutoVariable.h
//
//  AUTHOR:		Justin Randall	--  07/20/2009
//
//  PURPOSE:	Wraps serializeable types to intercept write operations, tracks changes
//              to the serializeable type, notifies an AutoArchive if a change has been
//              made, and registers itself with the containing AutoArchive for automatic
//              serialization.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2009 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------
#ifndef _INCLUDED_FAutoVariable_H
#define _INCLUDED_FAutoVariable_H

//---------------------------------------------------------------------------------------

#include "FAutoVariableBase.h"
#include "FAutoArchiveClassContainer.h"

//---------------------------------------------------------------------------------------

// Track baselines and deltas. Currently used to track out of sync bugs
// but can be adapted for general serialization to/from stream types
// and named values (XML or SQL, for example)
template<typename ObjectType, typename ContainerType>
class FAutoVariable : public FAutoVariableBase
{
public:
	FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ContainerType> &);
	FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ContainerType> &, const ObjectType &);

	// used for extended debugging out of sync errors. Does nothing in release builds
	FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ContainerType> &, bool callStackTracking);
	FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ContainerType> &, const ObjectType &, bool callStackTracking);

	~FAutoVariable();

	const ObjectType & get() const;
	
	ObjectType & dirtyGet();

	operator const ObjectType& () const;

	void set(const ObjectType & source);
	ObjectType & operator=(const ObjectType &);

	void load(FDataStream & loadFrom);
	void loadDelta(FDataStream & loadFrom);
	void save(FDataStream & saveTo) const;
	void saveDelta(FDataStream & saveTo) const;
	void clearDelta();
	bool compare(FDataStream & otherValue) const;
	void reset();

	const std::string & name() const;
	std::string  debugDump(const std::vector<std::pair<std::string, std::string> > &) const;
	std::string toString() const;

	void setStackTraceRemark();

	FAutoVariable & operator-=(const ObjectType & rhs);
	FAutoVariable & operator+=(const ObjectType & rhs);
	FAutoVariable & operator++(int);
	FAutoVariable & operator--(int);
	FAutoVariable & operator=(const FAutoVariable &);

private:
	// keep these out of containers by value, they won't do what is expected
	FAutoVariable(const FAutoVariable &);

private:
	ObjectType  m_value;
	FAutoArchiveClassContainer<ContainerType> &  m_owner;

	// let's help the debugger find the name of the variable
	// name() still works in non-debug builds. Excluding it
	// in Release is a memory optimization. It's here strictly
	// for debugging and development purposes.
#ifdef _DEBUG
	const std::string & m_name;
#endif
#if !defined(FINAL_RELEASE)
	mutable ObjectType m_remoteValue;
#endif
};

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
FDataStream & operator<<(FDataStream & archive, const FAutoVariable<ObjectType, ContainerType> & object)
{
	object.save(archive);
	return archive;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
FDataStream & operator>>(FDataStream & archive, FAutoVariable<ObjectType, ContainerType> & object)
{
	object.load(archive);
	return archive;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
FAutoVariable<ObjectType, ContainerType>::FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ContainerType> & owner) :
FAutoVariableBase(name, owner)
, m_value()
, m_owner(owner)
#ifdef _DEBUG
, m_name(*owner.getVariableName(*this))
#endif//_DEBUG
{
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
FAutoVariable<ObjectType, ContainerType>::FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ContainerType> & owner, const ObjectType & source) :
FAutoVariableBase(name, owner)
, m_value(source)
, m_owner(owner)
#ifdef _DEBUG
, m_name(*owner.getVariableName(*this))
#endif//_DEBUG
{
}

//---------------------------------------------------------------------------------------

// used for extended debugging out of sync errors. Does nothing in release builds
template<typename ObjectType, typename ContainerType>
FAutoVariable<ObjectType, ContainerType>::FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ContainerType> & owner, bool callStackTracking) :
FAutoVariableBase(name, owner, callStackTracking)
, m_value()
, m_owner(owner)
#ifdef _DEBUG
, m_name(*owner.getVariableName(*this))
#endif//_DEBUG
{
}

//---------------------------------------------------------------------------------------

// used for extended debugging out of sync errors. Does nothing in release builds
template<typename ObjectType, typename ContainerType>
FAutoVariable<ObjectType, ContainerType>::FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ContainerType> & owner, const ObjectType & source, bool callStackTracking) :
FAutoVariableBase(name, owner, callStackTracking)
, m_value(source)
, m_owner(owner)
#ifdef _DEBUG
, m_name(*owner.getVariableName(*this))
#endif//_DEBUG
{
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
FAutoVariable<ObjectType, ContainerType>::~FAutoVariable()
{
	m_owner.remove(*this);
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
__forceinline const ObjectType & FAutoVariable<ObjectType, ContainerType>::get() const
{
	return m_value;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
ObjectType & FAutoVariable<ObjectType, ContainerType>::dirtyGet()
{
	m_owner.touch(*this);
	return m_value;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
__forceinline FAutoVariable<ObjectType, ContainerType>::operator const ObjectType &() const
{
	return m_value;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
__forceinline ObjectType & FAutoVariable<ObjectType, ContainerType>::operator=(const ObjectType & rhs)
{
	if(rhs != m_value)
	{
		set(rhs);
	}
	return m_value;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
__forceinline FAutoVariable<ObjectType, ContainerType> & FAutoVariable<ObjectType, ContainerType>::operator=(const FAutoVariable<ObjectType, ContainerType> & rhs)
{
	if(&rhs != this)
	{
		set(rhs.get());
	}
	return *this;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
__forceinline void FAutoVariable<ObjectType, ContainerType>::set(const ObjectType & source)
{
	if(m_value != source)
	{
		m_owner.touch(*this);
		m_value = source;
	}
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
__forceinline FAutoVariable<typename ObjectType, typename ContainerType> & FAutoVariable<typename ObjectType, typename ContainerType>::operator-=(const ObjectType & rhs)
{
	set(m_value - rhs);
	return *this;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
__forceinline FAutoVariable<typename ObjectType, typename ContainerType> & FAutoVariable<typename ObjectType, typename ContainerType>::operator+=(const ObjectType & rhs)
{
	set(m_value + rhs);
	return *this;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
__forceinline FAutoVariable<typename ObjectType, typename ContainerType> & FAutoVariable<typename ObjectType, typename ContainerType>::operator++(int)
{
	// prefix and postfix operators are not guaranteed to be applied before being passed to a function
	ObjectType tmp = m_value;
	tmp++;
	set(tmp);
	return *this;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
__forceinline FAutoVariable<typename ObjectType, typename ContainerType> & FAutoVariable<typename ObjectType, typename ContainerType>::operator--(int)
{
	// prefix and postfix operators are not guaranteed to be applied before being passed to a function
	ObjectType tmp = m_value;
	tmp--;
	set(tmp);
	return *this;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
void FAutoVariable<ObjectType, ContainerType>::load(FDataStream & loadFrom)
{
	loadFrom >> m_value;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
void FAutoVariable<ObjectType, ContainerType>::save(FDataStream & saveTo) const
{
	saveTo << m_value;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
void FAutoVariable<ObjectType, ContainerType>::loadDelta(FDataStream & loadFrom)
{
	load(loadFrom);
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
void FAutoVariable<ObjectType, ContainerType>::clearDelta()
{
	// nothing to do for this type
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
void FAutoVariable<ObjectType, ContainerType>::reset()
{
	m_value = ObjectType();
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
void FAutoVariable<ObjectType, ContainerType>::saveDelta(FDataStream & saveTo) const
{
	save(saveTo);
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
std::string FAutoVariable<ObjectType, ContainerType>::toString() const
{
	return FSerialization::toString(m_value);
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
bool FAutoVariable<ObjectType, ContainerType>::compare(FDataStream & otherValue) const
{
	ObjectType compareWith;
	otherValue >> compareWith;
#if !defined(FINAL_RELEASE)
	m_remoteValue = compareWith;
#endif

	return m_value == compareWith;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
const std::string & FAutoVariable<ObjectType, ContainerType>::name() const
{
	// dereferenced because if this variable exists, it MUST be in the map
	// (or it could never have been constructed in the first place)
	return *(m_owner.getVariableName(*this));
}

//---------------------------------------------------------------------------------------
template<typename ObjectType, typename ContainerType>
std::string FAutoVariable<ObjectType, ContainerType>::debugDump(const std::vector<std::pair<std::string, std::string> > & callStacks) const
{
	std::string result = FAutoVariableBase::debugDump(callStacks);
	result += std::string("\n") + m_owner.debugDump(*this) + std::string("\n");
#if !defined(FINAL_RELEASE)
	result += std::string("local value=") + FSerialization::toString(m_value) + "\n";
	result += std::string("remote value=") + FSerialization::toString(m_remoteValue) + "\n";
#endif
	return result;
}

//---------------------------------------------------------------------------------------

template<typename ObjectType, typename ContainerType>
void FAutoVariable<ObjectType, ContainerType>::setStackTraceRemark()
{
#ifndef FINAL_RELEASE
	m_callStackRemark = m_owner.stackTraceRemark(*this);
#endif
}

//---------------------------------------------------------------------------------------

#endif//_INCLUDED_FAutoVariable_H
