//---------------------------------------------------------------------------------------
//  Copyright (c) 2007 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructors

inline FScopedCriticalSection::FScopedCriticalSection( FCriticalSection& kCriticalSection ): m_kCriticalSection( kCriticalSection )
{
	m_kCriticalSection.Enter();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Destructors

inline FScopedCriticalSection::~FScopedCriticalSection( void )
{
	m_kCriticalSection.Leave();
}

