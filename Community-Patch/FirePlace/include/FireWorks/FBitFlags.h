//------------------------------------------------------------------------------------------------
//
//  *****************   FIRAXIS GAME ENGINE   ********************
//
//! \file		FBitFlags.h
//! \author		Bart Muzzin -- 5-2-2006
//! \brief		Going to finally make a bitflags macro!
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2006 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#pragma once
#ifndef FBITFLAGS_H
#define FBITFLAGS_H

#define FBITFLAGS(FlagType) \
	public:\
		bool GetBit(FlagType uiFlag) const { return (m_uiFlags & uiFlag) != 0; } \
		void SetBit(FlagType uiFlag, bool bOn) {  if (bOn) m_uiFlags |= uiFlag; else  m_uiFlags &= ~uiFlag; } \
		void ToggleBit( FlagType uiFlag ) {  if( !(m_uiFlags & uiFlag) ) m_uiFlags |= uiFlag; else  m_uiFlags &= ~uiFlag; } \
		void ClearFlags() { m_uiFlags = 0; } \
		FlagType GetFlags() const { return m_uiFlags; } \
		void SetFlags(FlagType val) { m_uiFlags = val; } \
	private: \
		FlagType m_uiFlags;

template <typename T>
class FTBitFlag;

template <>
class FTBitFlag<unsigned char>
{
	FBITFLAGS(unsigned char);
public:
	FTBitFlag() : m_uiFlags(0) { }
	FTBitFlag(unsigned char uiInit) : m_uiFlags(uiInit) {}
};

template <>
class FTBitFlag<unsigned short>
{
	FBITFLAGS(unsigned short);
public:
	FTBitFlag() : m_uiFlags(0) { }
	FTBitFlag(unsigned short uiInit) : m_uiFlags(uiInit) {}
};

template <>
class FTBitFlag<unsigned int>
{
	FBITFLAGS(unsigned int);
public:
	FTBitFlag() : m_uiFlags(0) { }
	FTBitFlag(unsigned int uiInit) : m_uiFlags(uiInit) {}
};

typedef FTBitFlag<unsigned int> FBitFlag;

#endif // FBITFLAGS_H
