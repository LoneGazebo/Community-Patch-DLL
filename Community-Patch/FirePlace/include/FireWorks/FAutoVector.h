//---------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:		FAutoVariable.h
//
//  AUTHOR:		Justin Randall	--  07/29/2009
//
//  PURPOSE:	Wraps a vector of serializeable types to intercept write operations, 
//              tracks changes to the elements, notifies an AutoArchive if a change has 
//              been made, and registers itself with the containing AutoArchive for 
//              automatic serialization.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2009 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------
#ifndef _INCLUDED_FAutoVector_H
#define _INCLUDED_FAutoVector_H

//---------------------------------------------------------------------------------------

#include "FAutoVariable.h"
#include <vector>

//---------------------------------------------------------------------------------------
// The order of operations for changes to a vector must be preserved to apply a sparse
// delta to another copy of the vector. For example, an erase, then push back produces
// a different vector than a push_back, then erase (last element changes on the first 
// case, the vector is unchanged in the other). Actions mutating the underlying vector
// are captured, in order, using AutoVectorCommands.
template<typename ElementType>
struct AutoVectorCommand
{
	enum CommandTypes
	{
		ERASE
		, INSERT
		, SET
		, PUSH_BACK
		, CLEAR
		, RESIZE
		, RESIZE_WITH_VALUE
		, NO_COMMAND

	};

	AutoVectorCommand(CommandTypes c, unsigned int i, const ElementType & v) :
	command(c)
	, index(i)
	, value(v)
	{
	}

	AutoVectorCommand() : command(NO_COMMAND), index(0), value() {}

	bool operator==(const AutoVectorCommand & rhs) const
	{
		if(command != rhs.command || index != rhs.index) return false;
		switch(command)
		{
		case AutoVectorCommand<ElementType>::ERASE:
		case AutoVectorCommand<ElementType>::INSERT:
		case AutoVectorCommand<ElementType>::SET:
		case AutoVectorCommand<ElementType>::RESIZE_WITH_VALUE:
		case AutoVectorCommand<ElementType>::PUSH_BACK:
			{
				if(!(value == rhs.value)) return false;
			}
		}
		return true;
	}

	unsigned char command;
	unsigned int  index;
	ElementType   value;
};

//---------------------------------------------------------------------------------------
// The FAutoVariable is specialized for vectors to allow smaller updates to be captured
// against potentially large data sets. Without the specialization, the entire vector
// would need to be serialized, sent and compared, even if only one element changed.
// The FAutoVariable template provides no element-level access to it's type. This 
// specialization tries to present an interface that is close to the STL vector 
// interface. The interface differs from STL vector when mutable operations could not
// otherwise be intercepted.
template<typename ElementType, typename ClassContainer>
class FAutoVariable<std::vector<ElementType>, ClassContainer> : public FAutoVariableBase
{
public:
	typedef typename std::vector<ElementType>::const_iterator const_iterator;
    typedef typename std::vector<ElementType>::const_reference const_reference;
    
	FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ClassContainer> & container);
	FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ClassContainer> & container, bool callStackTracking);
	FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ClassContainer> & container, size_t reserveSize, bool callStackTracking);

	void load(FDataStream & loadFrom);
	void loadDelta(FDataStream & loadFrom);
	void save(FDataStream & saveTo) const;
	void saveDelta(FDataStream & saveTo) const;
	void clearDelta();
	void reset();
	bool compare(FDataStream & otherValue) const;
	void setStackTraceRemark();
	std::string toString() const;

	const std::string & name() const;
	size_t size() const;

	operator const std::vector<ElementType> &() const;
	// Get direct access to the vector.  Use for unserialization only please!
	std::vector<ElementType> & dirtyGet();

	// for whatever reason, the VC9 doesn't deal with
	// const_iterator use outside the declaration, otherwise
	// it would exist with the rest of the definitions below
	const_iterator begin() const { return m_value.begin(); }
	const_iterator end() const { return m_value.end(); }


	const_reference operator[](size_t index) const;

	FAutoVariable & operator=(const std::vector<ElementType> & rhs);

	void setAt(size_t index, const ElementType & source);
	void push_back(const ElementType & source);
	void erase(size_t index);
	void insert(size_t index, const ElementType & source);
	void clear();
	void resize(size_t n);
	void resize(size_t n, const ElementType & v);

private:

	typedef std::vector< AutoVectorCommand<ElementType> /*, FSTLBlockAllocator< AutoVectorCommand<ElementType>, 1, c_eCiv5GameplayDLL >*/ > COMMAND_VEC_TYPE;

	mutable COMMAND_VEC_TYPE m_commands;
	std::vector< ElementType >  m_value;
	FAutoArchiveClassContainer<ClassContainer> &  m_owner;

	// When debugging, help the developer out and provide
	// the variable's name in the debugger. This is already
	// available by way of the container, but is not as
	// readily available when debugging.
#ifdef _DEBUG
	const std::string & m_name;
#endif//_DEBUG
};

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
FDataStream & operator<<(FDataStream & saveTo, const FAutoVariable<std::vector<ElementType>, ClassContainer> & readFrom)
{
	readFrom.save(saveTo);
	return saveTo;
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
FDataStream & operator>>(FDataStream & loadFrom, FAutoVariable<std::vector<ElementType>, ClassContainer> & writeTo)
{
	writeTo.load(loadFrom);
	return loadFrom;
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
FAutoVariable<std::vector<ElementType>, ClassContainer>::FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ClassContainer> & container) :
FAutoVariableBase(name, container)
, m_commands()
, m_value()
, m_owner(container)
#ifdef _DEBUG
, m_name(*container.getVariableName(*this))
#endif//_DEBUG
{
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
FAutoVariable<std::vector<ElementType>, ClassContainer>::FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ClassContainer> & container, bool callStackTracking) :
FAutoVariableBase(name, container, callStackTracking)
, m_commands()
, m_value()
, m_owner(container)
#ifdef _DEBUG
, m_name(*container.getVariableName(*this))
#endif//_DEBUG
{
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
FAutoVariable<std::vector<ElementType>, ClassContainer>::FAutoVariable(const std::string & name, FAutoArchiveClassContainer<ClassContainer> & container, size_t reserveSize, bool callStackTracking) :
FAutoVariableBase(name, container, callStackTracking)
, m_commands()
, m_value(reserveSize)
, m_owner(container)
#ifdef _DEBUG
, m_name(*container.getVariableName(*this))
#endif//_DEBUG
{
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
FAutoVariable<std::vector<ElementType>, ClassContainer> & FAutoVariable<std::vector<ElementType>, ClassContainer>::operator=(const std::vector<ElementType> & rhs)
{
	clear();
	std::vector<ElementType>::const_iterator i;
	for(i = rhs.begin(); i != rhs.end(); ++i)
	{
		push_back(*i);
	}
	return *this;
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
FAutoVariable<std::vector<ElementType>, ClassContainer>::operator const std::vector<ElementType> &() const
{
	return m_value;
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
std::vector<ElementType>& FAutoVariable<std::vector<ElementType>, ClassContainer>::dirtyGet()
{
	return m_value;
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
std::string FAutoVariable<std::vector<ElementType>, ClassContainer>::toString() const
{
	return FSerialization::toString(m_value);
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::load(FDataStream & loadFrom)
{
	clear();
	m_commands.clear();
	loadFrom >> m_value;
	m_commands.clear();
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::save(FDataStream & saveTo) const
{
	saveTo << m_value;
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::loadDelta(FDataStream & loadFrom)
{
	loadFrom >> m_value;
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::saveDelta(FDataStream & saveTo) const
{
	save(saveTo);
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::clearDelta()
{
	m_commands.clear();
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
size_t FAutoVariable<std::vector<ElementType>, ClassContainer>::size() const
{
	return m_value.size();
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
bool FAutoVariable<std::vector<ElementType>, ClassContainer>::compare(FDataStream & otherValue) const
{
	std::vector< ElementType > otherValueVec;
	otherValue >> otherValueVec;
	bool result = m_value == otherValueVec;
	m_commands.clear();
	return result;
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
const std::string & FAutoVariable<std::vector<ElementType>, ClassContainer>::name() const
{
	return *(m_owner.getVariableName(*this));
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
typename FAutoVariable<std::vector<ElementType>, ClassContainer>::const_reference 
FAutoVariable<std::vector<ElementType>, ClassContainer>::operator[](size_t index) const
{
	return m_value[index];
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::setStackTraceRemark()
{
#ifndef FINAL_RELEASE
	m_callStackRemark = m_owner.stackTraceRemark(*this);
#endif
}


//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::setAt(size_t index, const ElementType & v)
{
	if(!(v == m_value[index]))
	{
		m_owner.touch(*this);
		m_value[index] = v;
		m_commands.push_back(AutoVectorCommand<ElementType>(AutoVectorCommand<ElementType>::SET, index, v));
	}
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::insert(size_t index, const ElementType & v)
{
	m_owner.touch(*this);
	std::vector<ElementType>::iterator i = m_value.begin() + index;
	m_value.insert(i, v);
	m_commands.push_back(AutoVectorCommand<ElementType>(AutoVectorCommand<ElementType>::INSERT, index, v));
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::push_back(const ElementType & v)
{
	m_owner.touch(*this);
	m_commands.push_back(AutoVectorCommand<ElementType>(AutoVectorCommand<ElementType>::PUSH_BACK, 0, v));
	m_value.push_back(v);
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::clear()
{
	m_owner.touch(*this);
	m_commands.push_back(AutoVectorCommand<ElementType>(AutoVectorCommand<ElementType>::CLEAR, 0, ElementType()));
	m_value.clear();
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::reset()
{
	m_commands.clear();
	m_value.clear();
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::erase(size_t index)
{
	m_owner.touch(*this);

	std::vector<ElementType>::iterator i = m_value.begin() + index;
	m_value.erase(i);
	m_commands.push_back(AutoVectorCommand<ElementType>(AutoVectorCommand<ElementType>::ERASE, index, ElementType()));
	
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::resize(size_t count)
{
	m_owner.touch(*this);
	m_value.resize(count);
	m_commands.push_back(AutoVectorCommand<ElementType>(AutoVectorCommand<ElementType>::RESIZE, count, ElementType()));
	
}

//---------------------------------------------------------------------------------------

template<typename ElementType, typename ClassContainer>
void FAutoVariable<std::vector<ElementType>, ClassContainer>::resize(size_t count, const ElementType & v)
{
	m_owner.touch(*this);
	m_value.resize(count, v);
	m_commands.push_back(AutoVectorCommand<ElementType>(AutoVectorCommand<ElementType>::RESIZE_WITH_VALUE, count, v));
	
}

//---------------------------------------------------------------------------------------

template<typename ElementType>
FDataStream & operator>>(FDataStream & loadFrom, AutoVectorCommand<ElementType> & writeTo)
{
	loadFrom >> writeTo.command;
	switch(writeTo.command)
	{
	case AutoVectorCommand<ElementType>::ERASE:
	case AutoVectorCommand<ElementType>::INSERT:
	case AutoVectorCommand<ElementType>::SET:
	case AutoVectorCommand<ElementType>::RESIZE_WITH_VALUE:
		{
			loadFrom >> writeTo.index;
			loadFrom >> writeTo.value;
		}
		break;
	case AutoVectorCommand<ElementType>::RESIZE:
		{
			loadFrom >> writeTo.index;
		}
		break;
	case AutoVectorCommand<ElementType>::PUSH_BACK:
		{
			loadFrom >> writeTo.value;
		}
		break;
	}
	return loadFrom;
}

//---------------------------------------------------------------------------------------

template<typename ElementType>
FDataStream & operator<<(FDataStream & saveTo, const AutoVectorCommand<ElementType> & readFrom)
{
	saveTo << readFrom.command;
	switch(readFrom.command)
	{
	case AutoVectorCommand<ElementType>::ERASE:
	case AutoVectorCommand<ElementType>::INSERT:
	case AutoVectorCommand<ElementType>::SET:
	case AutoVectorCommand<ElementType>::RESIZE_WITH_VALUE:
		{
			saveTo << readFrom.index;
			saveTo << readFrom.value;
		}
		break;
	case AutoVectorCommand<ElementType>::RESIZE:
		{
			saveTo << readFrom.index;
		}
		break;
	case AutoVectorCommand<ElementType>::PUSH_BACK:
		{
			saveTo << readFrom.value;
		}
		break;
	}

	return saveTo;
}

//---------------------------------------------------------------------------------------

#endif//_INCLUDED_FAutoVector_H
