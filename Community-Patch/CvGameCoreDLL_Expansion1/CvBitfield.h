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

	CvBitfield() : m_pBits(NULL), m_uiByteSize(0) {};
	CvBitfield(uint uiSize) : m_pBits(NULL), m_uiByteSize(0) { SetSize(uiSize); };
	~CvBitfield() { SetSize(0); }

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
		else
			if (bValue)		// Only resize if setting the value
			{
				SetSize(uiIndex + 1);
				m_pBits[uiByteIndex] |= (1 << (uiIndex&0x7));
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
		if (uiByteSize == 0)
		{
			delete []m_pBits;
			m_uiByteSize = 0;
			m_pBits = NULL;
		}
		else
			if (uiByteSize > m_uiByteSize)
			{
				byte* pNewBits = new byte[uiByteSize];
				memset(pNewBits, 0, uiByteSize);
				if (m_uiByteSize)
				{
					memcpy(pNewBits, m_pBits, m_uiByteSize);
					delete []m_pBits;
				}
				m_pBits = pNewBits;
				m_uiByteSize = uiByteSize;
			}
			else
				if (uiByteSize < m_uiByteSize)
				{
					byte* pNewBits = new byte[uiByteSize];
					memset(pNewBits, 0, uiByteSize);
					if (m_uiByteSize)
					{
						memcpy(pNewBits, m_pBits, uiByteSize);
						delete []m_pBits;
					}
					m_pBits = pNewBits;
					m_uiByteSize = uiByteSize;
				}
	}

private:
	uint m_uiByteSize;
	byte* m_pBits;
};

#endif // CVBITFIELD_H