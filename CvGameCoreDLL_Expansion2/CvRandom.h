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

#include <string>

class CvRandom
{

public:
	CvRandom(const std::string& name);
	virtual ~CvRandom();

	void init(unsigned long long ulSeed);
	void uninit();
	void reset(unsigned long long ulSeed = 0);

	unsigned long get(unsigned long usNum, const char* pszLog = NULL);  //  Returns value from 0 to num-1 inclusive.

	float getFloat();

	void reseed(unsigned long long ulNewValue);
	unsigned long long getSeed() const;
	unsigned long getCallCount() const;
	unsigned long getResetCount() const;

	// for serialization
	void read(FDataStream& Stream);
	void write(FDataStream& Stream) const;

	bool operator==(const CvRandom& rhs) const;
	bool operator!=(const CvRandom& rhs) const;

	void CopyFrom(const CvRandom& rhs);

protected:
	unsigned long long m_ullRandomSeed;

	// for OOS checks/debugging
	std::string m_name;
	unsigned long m_ulCallCount;
	unsigned long m_ulResetCount;

	// If true, the instance is marked as being one that should be synchronous across multi-player games.
	bool m_bSynchronous;

private:
	//nobody should be calling this
	CvRandom(const CvRandom& source);
	CvRandom();
	CvRandom operator=(const CvRandom&);
};

FDataStream& operator<<(FDataStream& saveTo, const CvRandom& readFrom);
FDataStream& operator>>(FDataStream& loadFrom, CvRandom& writeTo);
#endif
