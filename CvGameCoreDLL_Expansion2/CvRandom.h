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

// Get a fake random number which depends only on game state
// This should be good enough for most purposes
// Very importantly, it should reduce desyncs in multiplayer
inline uint hash32(uint input)
{
    uint state = input * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

/*
//here is another one which is supposed to be good
inline uint hash32(uint x)
{
    x ^= x >> 16;
    x *= 0xa812d533;
    x ^= x >> 15;
    x *= 0xb278e4ad;
    x ^= x >> 17;
    return x;
}
*/

/// Helper type for producing seed values.
struct NODISCARD CvSeeder {
    uint value;

    inline CvSeeder() : value(0) {}
    template<typename T> explicit inline CvSeeder(T value) : value(hash32(static_cast<uint>(value))) {}
    inline CvSeeder(const CvSeeder& other) : value(other.value) {}

    inline static CvSeeder fromRaw(uint rawValue)
    {
        CvSeeder newSeed;
        newSeed.value = rawValue;
        return newSeed;
    }

    template<typename T> inline CvSeeder& mixRawAssign(T otherValue)
    {
        value ^= static_cast<uint>(otherValue) + 0x9e3779b9 + (value << 6) + (value >> 2);
        return *this;
    }

    template<typename T> inline CvSeeder& mixAssign(T otherValue)
    {
        return mixRawAssign(hash32(static_cast<uint>(otherValue)));
    }

    // `mixAssign` is specialized for mixing two seeders.
    // This is done to avoid rehashing the value within the seeder because it is presumably already hashed.
    template<> inline CvSeeder& mixAssign<CvSeeder>(CvSeeder otherSeed)
    {
        return mixRawAssign(otherSeed.value);
    }

    template<typename T> inline CvSeeder mixRaw(T otherValue) const
    {
        CvSeeder newSeed = *this;
        newSeed.mixRawAssign(otherValue);
        return newSeed;
    }

    template<typename T> inline CvSeeder mix(T otherValue) const
    {
        CvSeeder newSeed = *this;
        newSeed.mixAssign(otherValue);
        return newSeed;
    }

    inline operator uint() const
    {
        return value;
    }
};

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
	template<typename Random, typename Visitor>
	static void Serialize(Random& random, Visitor& visitor);
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
