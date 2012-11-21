//---------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:		FAutoVariableBase.cpp
//
//  AUTHOR:		Justin Randall	--  07/20/2009
//
//  PURPOSE:	Base class declaration for FAutoVariables. See FAutoVariable.h
//              for a concrete description.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2009 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------
#ifndef _INCLUDED_FAutoVariableBase_H
#define _INCLUDED_FAutoVariableBase_H

#ifndef FINAL_RELEASE
#	include "FCallStack.h"
#endif//FINAL_RELEASE

#ifdef _WINPC
#	pragma warning ( disable : 4355 )  // Clients of this class will need to pass 'this' along to construct AutoVariables
#endif//_WINPC
#include <vector>

class FAutoArchive;
class FDataStream;
class FAutoVariableBase;

namespace FSerialization
{
	// converting types to printable strings, for debugging
	std::string toString(const bool & source);
	std::string toString(const unsigned char & source);
	std::string toString(const char & source);
	std::string toString(const unsigned short & source);
	std::string toString(const short & source);
	std::string toString(const unsigned int & source);
	std::string toString(const int & source);
	std::string toString(const float & source);
	std::string toString(const double & source);
	std::string toString(const std::string & source); // for completeness, no a-priori knowlege of what the var type is
	std::string toString(const FAutoVariableBase &);
	template<typename ValueType>
	std::string toString(const std::vector<ValueType> & source)
	{
		std::string result = "{";
		std::vector<ValueType>::const_iterator i;
		for(i = source.begin(); i != source.end(); ++i)
		{
			if(i != source.begin())
				result += std::string(",");
			result += toString(*i);
		}
		return result;
	}

	// temporary fall-through do-nothing case
	// comment this to let the compiler tell you which
	// types need implementations or which implementations
	// are not in scope for code that fails after removing
	// this function
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

	// used for extended debugging out of sync errors. Does nothing in release builds
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

	// used for extended debugging out of sync errors. Does nothing in release builds
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
	// keep these out of containers by value, they won't do what is expected
	FAutoVariableBase(const FAutoVariableBase &);
	FAutoVariableBase & operator=(const FAutoVariableBase &);

private:
	//std::string  m_name; // JAR : todo  - this is redundant for each class instance, this could be a FlyWeight
};

#endif//_INCLUDED_FAutoVariableBase_H
