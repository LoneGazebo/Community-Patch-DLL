//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//  FILE:		FCriticalSection.h
//
//  AUTHOR:		Mike Breitkreutz	--  05/03/2005
//
//  PURPOSE:	Critical Section object wrapper to bury platform-specific code.
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef		FCRITICALSECTION_H
#define		FCRITICALSECTION_H
#pragma		once

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  CLASS:		FCriticalSection
//
//  PURPOSE:	Critical section object wrapper
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class FCriticalSection
{
public:
	// Constructors:
	//-------------
	FCriticalSection( void );

	// Destructors:
	//------------
	~FCriticalSection( void );

	// Methods:
	//--------
	bool Try( void );

	void Enter( void );
	void Leave( void );

	int GetLockCount() const;
	int GetRecursionCount() const;

private:
	// Members:
	//--------
#ifdef		WIN32
		CRITICAL_SECTION m_kCriticalSection;	// The critical section object
#else	//	WIN32
#ifdef		_PS3
		sys_lwmutex_t m_kCriticalSection;		// The critical section object
#endif	//	_PS3
#endif	//	WIN32

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  CLASS:		FScopedCriticalSection
//
//  PURPOSE:	Critical section object wrapper
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class FScopedCriticalSection
{
public:
	// Constructors:
	//-------------
	FScopedCriticalSection( FCriticalSection& kCriticalSection );

	// Destructors:
	//------------
	~FScopedCriticalSection( void );


private:
	// Members:
	//--------
	FCriticalSection& m_kCriticalSection;	// A reference to the critical section object
};

#include	"FCriticalSection.inl"

#endif	//	FCRITICALSECTION_H
