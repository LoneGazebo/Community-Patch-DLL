/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// random.h

#ifndef CIV5_RANDOM_H
#define CIV5_RANDOM_H

#ifdef _DEBUG
#include <vector>
#endif//_DEBUG

class CvRandom
{

public:
	explicit CvRandom(bool extendedCallStackDebugging);
	CvRandom();
	CvRandom(const CvRandom & source);
	virtual ~CvRandom();

	void init(unsigned long ulSeed);
	void uninit();
	void reset(unsigned long ulSeed = 0);

	unsigned short get(unsigned short usNum, const char* pszLog = NULL);  //  Returns value from 0 to num-1 inclusive.
	float getFloat();

	void reseed(unsigned long ulNewValue);
	unsigned long getSeed() const;
	unsigned long getCallCount() const;
	unsigned long getResetCount() const;

	// for serialization
	void read(FDataStream& Stream);
	void write(FDataStream& Stream) const;

	bool operator==(const CvRandom & rhs) const;
	bool operator!=(const CvRandom & rhs) const;

	// for OOS debugging
	const std::vector<std::string> & getResolvedCallStacks() const;
	const std::vector<unsigned long> & getSeedHistory() const;
	void resolveCallStacks() const;
	bool callStackDebuggingEnabled() const;
	void setCallStackDebuggingEnabled(bool enabled);
	void clearCallstacks();

protected:
	void recordCallStack();

protected:

	unsigned long m_ulRandomSeed;

	// for OOS checks/debugging
	unsigned long m_ulCallCount;
	unsigned long m_ulResetCount;
	bool m_bSynchronous;		// If true, the instance is marked as being one that should be synchronous across multi-player games.

#ifdef _DEBUG
	bool m_bExtendedCallStackDebugging;
	// something awful is happening with synchronization, log call stacks
	mutable std::vector<FCallStack> m_kCallStacks;
	mutable std::vector<unsigned long> m_seedHistory;
	// just in case addresses don't match up, resolve symbols on send
	// and compare on the remote for somewhat meaningful output
	mutable std::vector<std::string>  m_resolvedCallStacks;
#endif//_DEBUG

};

FDataStream & operator<<(FDataStream & saveTo, const CvRandom & readFrom);
FDataStream & operator>>(FDataStream & loadFrom, CvRandom & writeTo);
#endif
