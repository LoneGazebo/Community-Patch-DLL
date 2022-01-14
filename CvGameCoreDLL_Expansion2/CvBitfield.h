/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#ifndef CVBITFIELD_H
#define CVBITFIELD_H

#pragma once

/// Bitfield class that can hold a variable number of bits.
class CvBitfield
{
public:

	CvBitfield()
		: m_pBits(NULL)
		, m_uiByteSize(0)
	{
	}

	CvBitfield(uint uiSize)
		: m_pBits(NULL)
		, m_uiByteSize(0)
	{
		SetSize(uiSize);
	}

	CvBitfield(const CvBitfield& src)
		: m_pBits(NULL)
		, m_uiByteSize(0)
	{
		*this = src;
	}

	~CvBitfield()
	{
		SetSize(0);
	}

	CvBitfield& operator=(const CvBitfield& rhs)
	{
		resizeBytes(rhs.m_uiByteSize);
		memcpy(m_pBits, rhs.m_pBits, m_uiByteSize);
		return *this;
	}

	/// Get a bit
	bool GetBit(uint uiIndex) const
	{
		uint uiByteIndex = uiIndex >> 3;
		if (uiByteIndex < m_uiByteSize)
		{
			return (m_pBits[uiByteIndex] & (1 << (uiIndex&0x7))) != 0;
		}
		return false;
	}

	/// Set a bit on/off
	void SetBit(uint uiIndex, bool bValue)
	{
		uint uiByteIndex = uiIndex >> 3;
		if (uiByteIndex < m_uiByteSize)
		{
			if (bValue)
				m_pBits[uiByteIndex] |= (1 << (uiIndex&0x7));
			else
				m_pBits[uiByteIndex] &= ~(1 << (uiIndex&0x7));
		}
		else if (bValue) // Only resize if setting the value
		{
			SetSize(uiIndex + 1);
			m_pBits[uiByteIndex] |= (1 << (uiIndex & 0x7));
		}
	}
	/// Clear all the bits
	void Clear()
	{
		memset(m_pBits, 0, m_uiByteSize);
	}
	/// Get the bit capacity. Please note that internally this will be rounded up to to the nearest 8 bits
	/// Do not expect the exact number that was given to SetSize.  e.g.  if SetSize(12) then GetSize() == 16
	uint GetSize() const
	{
		return m_uiByteSize << 3;
	}
	/// Set the bit capacity.  Please note that internally this will be rounded up to to the nearest 8 bits.
	void SetSize(uint uiSize)
	{
		uint uiByteSize = (uiSize >> 3) + ((uiSize&0x07)?1:0);
		resizeBytes(uiByteSize);
	}

private:
	void resizeBytes(uint uiByteSize)
	{
		if (uiByteSize == 0)
		{
			delete[]m_pBits;
			m_uiByteSize = 0;
			m_pBits = NULL;
		}
		else if (uiByteSize != m_uiByteSize)
		{
			byte* pNewBits = new byte[uiByteSize];
			memset(pNewBits, 0, uiByteSize);
			if (m_uiByteSize)
			{
				const uint retainSize = uiByteSize > m_uiByteSize ? m_uiByteSize : uiByteSize;
				memcpy(pNewBits, m_pBits, retainSize);
				delete[]m_pBits;
			}
			m_pBits = pNewBits;
			m_uiByteSize = uiByteSize;
		}
	}

	uint m_uiByteSize;
	byte* m_pBits;

	friend FDataStream& operator<<(FDataStream&, const CvBitfield&);
	friend FDataStream& operator>>(FDataStream&, CvBitfield&);
};

inline FDataStream& operator<<(FDataStream& saveTo, const CvBitfield& readFrom)
{
	saveTo << readFrom.m_uiByteSize;
	if (readFrom.m_uiByteSize > 0)
		saveTo.WriteIt(readFrom.m_uiByteSize, readFrom.m_pBits);
	return saveTo;
}
inline FDataStream& operator>>(FDataStream& loadFrom, CvBitfield& writeTo)
{
	uint uiByteSize;
	loadFrom >> uiByteSize;
	writeTo.resizeBytes(uiByteSize);
	if (uiByteSize > 0)
		loadFrom.ReadIt(uiByteSize, writeTo.m_pBits);
	return loadFrom;
}

#endif // CVBITFIELD_H