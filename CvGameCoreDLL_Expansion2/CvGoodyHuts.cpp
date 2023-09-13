/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvGoodyHuts.h"
#include "CvSpanSerialization.h"

// static
int **CvGoodyHuts::m_aaiPlayerGoodyHutResults = NULL;

//	---------------------------------------------------------------------------
/// New Goody Hut Result from a Player
void CvGoodyHuts::DoPlayerReceivedGoody(PlayerTypes ePlayer, GoodyTypes eGoody)
{
	FAssert(ePlayer >= 0);
	FAssert(ePlayer < MAX_MAJOR_CIVS);
	FAssert(eGoody >= 0);
	//	FAssert(eGoody < DB.count("GoodyHuts"));

	// Push elements up in the array so that we free up element 0
	for (int iGoodySlotLoop = 0; iGoodySlotLoop < NUM_GOODIES_REMEMBERED-1; iGoodySlotLoop++)
	{
		m_aaiPlayerGoodyHutResults[ePlayer][iGoodySlotLoop+1] = m_aaiPlayerGoodyHutResults[ePlayer][iGoodySlotLoop];
	}

	// Most recent Goody gets slot 0
	m_aaiPlayerGoodyHutResults[ePlayer][0] = eGoody;
}

//	---------------------------------------------------------------------------
/// Are we allowed to get this Goody right now?
bool CvGoodyHuts::IsCanPlayerReceiveGoody(PlayerTypes ePlayer, GoodyTypes eGoody)
{
	return !IsHasPlayerReceivedGoodyLately(ePlayer, eGoody);
}

//	---------------------------------------------------------------------------
/// Have we gotten this type of Goody lately? (in the last 3 Goodies, defined by NUM_GOODIES_REMEMBERED)
bool CvGoodyHuts::IsHasPlayerReceivedGoodyLately(PlayerTypes ePlayer, GoodyTypes eGoody)
{
	FAssert(ePlayer >= 0);
	FAssert(ePlayer < MAX_MAJOR_CIVS);
	FAssert(eGoody >= 0);
	//	FAssert(eGoody < DB.count("GoodyHuts"));

	// Look at all of our Goody slots and see if the requested Goody matches anything
	for (int iGoodySlotLoop = 0; iGoodySlotLoop < NUM_GOODIES_REMEMBERED; iGoodySlotLoop++)
	{
		if (m_aaiPlayerGoodyHutResults[ePlayer][iGoodySlotLoop] == eGoody)
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// Reset
void CvGoodyHuts::Reset()
{
	int iI = 0;
	int iJ = 0;

	// Allocate memory
	if (m_aaiPlayerGoodyHutResults == NULL)
	{
		m_aaiPlayerGoodyHutResults = FNEW(int*[MAX_MAJOR_CIVS], c_eCiv5GameplayDLL, 0);
		for (iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_aaiPlayerGoodyHutResults[iI] = FNEW(int[NUM_GOODIES_REMEMBERED], c_eCiv5GameplayDLL, 0);
		}
	}

	// Default values
	for (iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		for (iJ = 0; iJ < NUM_GOODIES_REMEMBERED; iJ++)
		{
			m_aaiPlayerGoodyHutResults[iI][iJ] = -1;
		}
	}
}

//	---------------------------------------------------------------------------
/// Uninit
void CvGoodyHuts::Uninit()
{
	if (m_aaiPlayerGoodyHutResults != NULL)
	{
		for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			SAFE_DELETE_ARRAY(m_aaiPlayerGoodyHutResults[iI]);
		}
		SAFE_DELETE_ARRAY(m_aaiPlayerGoodyHutResults);
	}
}

//	---------------------------------------------------------------------------
template<typename Visitor>
void CvGoodyHuts::Serialize(Visitor& visitor)
{
	for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		visitor(MakeConstSpan(m_aaiPlayerGoodyHutResults[iI], NUM_GOODIES_REMEMBERED));
	}
}

//	---------------------------------------------------------------------------
void CvGoodyHuts::Read(FDataStream& kStream)
{
	Reset();

	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(serialVisitor);
}

//	---------------------------------------------------------------------------
void CvGoodyHuts::Write(FDataStream& kStream)
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(serialVisitor);
}
