//---------------------------------------------------------------------------------------
//
//  *****************   CIV 5 World Builder Map   ********************
//
//  FILE:    FRelationshipBitset.h
//
//  AUTHOR:  Eric Jordan  --  4/14/2010
//
//  PURPOSE:
//		This little monstrosity was created to support the Civ 5 World Builder map format.
//		It creates a bitset which stores boolean relationships between elements.  In the
//		case of the World Builder maps it is used to store relationships between teams.
//		It stores a unique bit for the relationship between each element with every other
//		element.  For example: a bit for each pair of teams in a civ game representing if
//      they are at war with each other.
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2010 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#pragma once
#ifndef FRelationshipBitset_h
#define FRelationshipBitset_h

// TElementCount - The number of elements that have relationships between them.  (more than 0 please)
// TSelfRelation - The relationship an element has with itself.
template<uint TElementCount, bool TSelfRelation>
class FRelationshipBitset
{
public:
	FRelationshipBitset()
	{
		ZeroMemory(m_Mem, sizeof(m_Mem));
	}

	const bool Get(uint uiFirst, uint uiSecond) const
	{
		if( uiFirst == uiSecond )
		{
			return TSelfRelation;
		}
		else
		{
			const uint uiBitIndex = GetBitIndex(uiFirst, uiSecond);
			const uint uiByte = uiBitIndex / 8;
			const uint uiBit = uiBitIndex % 8;
			const byte byMask = (1 << uiBit);
			return (m_Mem[uiByte] & byMask) != 0;
		}
	}

	void Set(uint uiFirst, uint uiSecond, bool bVal)
	{
		FAssertMsg(uiFirst != uiSecond, "Cannot set self-relationships");
		if( uiFirst != uiSecond )
		{
			const uint uiBitIndex = GetBitIndex(uiFirst, uiSecond);
			const uint uiByte = uiBitIndex / 8;
			const uint uiBit = uiBitIndex % 8;
			const byte byMask = (1 << uiBit);
			if( bVal ) m_Mem[uiByte] |= byMask;
			else m_Mem[uiByte] &= ~byMask;
		}
	}

	static uint GetSerializedSize(uint uiElementCount)
	{
		const uint uiBitCount = GetTotalBitCount(uiElementCount);
		return uiBitCount / 8 + ((uiBitCount % 8)? 1 : 0);
	}

	void Serialize(void *pvBuffer, uint uiElementCount) const
	{
		FAssert(uiElementCount < TElementCount);
		memcpy(pvBuffer, m_Mem, GetSerializedSize(uiElementCount));
	}

	void Deserialize(const void *pvBuffer, uint uiElementCount)
	{
		FAssert(uiElementCount < TElementCount);
		memcpy(m_Mem, pvBuffer, GetSerializedSize(uiElementCount));
	}

private:
	// BitCount = ((n - 1) / 2) * n where n is the team count
	static const uint BitCount  = ((TElementCount - 1) * TElementCount) / 2;
	static const uint ByteCount = BitCount / 8 + ((BitCount % 8)? 1 : 0);

	// returns the total number of bits for a relationship map
	static const uint GetTotalBitCount(uint uiElementCount)
	{
		// BitCount = ((n - 1) / 2) * n where n is the team count
		return ((uiElementCount - 1) * uiElementCount) / 2;
	}

	static const uint GetBitIndex(uint uiFirst, uint uiSecond)
	{
		FAssertMsg(uiFirst != uiSecond, "There are no self relations in this bitset");

		uint uiLarge, uiSmall;
		if( uiFirst > uiSecond )
		{
			uiLarge = uiFirst;
			uiSmall = uiSecond;
		}
		else
		{
			uiLarge = uiSecond;
			uiSmall = uiFirst;
		}

		FAssertMsg(uiLarge < TElementCount, "Out of range!");

		// uiStart represents the start of the bitfield for the large index
		const uint uiStart = GetTotalBitCount(uiLarge); // Get the bitcount leading up to this
		return uiStart + uiSmall;
	}

	byte m_Mem[ByteCount];
};

#endif // FRelationshipBitset_h
