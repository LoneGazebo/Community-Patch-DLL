/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"

// must be included after all other headers
#include "LintFree.h"

/// Constructor
CvEmphasisEntry::CvEmphasisEntry(void):
m_bAvoidGrowth(false),
m_bGreatPeople(false),
m_piYieldModifiers(NULL)
{
}

/// Destructor
CvEmphasisEntry::~CvEmphasisEntry(void)
{
	SAFE_DELETE_ARRAY(m_piYieldModifiers);
}

/// Read from XML file (pass 1)
bool CvEmphasisEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	m_bAvoidGrowth = kResults.GetBool("AvoidGrowth");
	m_bGreatPeople = kResults.GetBool("GreatPeople");

	const char* szEmphasizeType = GetType();
	kUtility.SetYields(m_piYieldModifiers, "EmphasizeInfo_Yields", "EmphasizeType", szEmphasizeType);

	return true;
}

/// Does this type of emphasis mean we shouldn't let the city grow?
bool CvEmphasisEntry::IsAvoidGrowth() const
{
	return m_bAvoidGrowth;
}
// Is this type of emphasis to promote development of great people
bool CvEmphasisEntry::IsGreatPeople() const
{
	return m_bGreatPeople;
}
// What is the yield change for this yield type?
int CvEmphasisEntry::GetYieldChange(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldModifiers ? m_piYieldModifiers[i] : -1;
}

//=====================================
// CvEmphasisXMLEntries
//=====================================
/// Constructor
CvEmphasisXMLEntries::CvEmphasisXMLEntries(void)
{

}

/// Destructor
CvEmphasisXMLEntries::~CvEmphasisXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of policy entries
std::vector<CvEmphasisEntry*>& CvEmphasisXMLEntries::GetEmphasisEntries()
{
	return m_paEmphasisEntries;
}

/// Number of defined policies
int CvEmphasisXMLEntries::GetNumEmphases()
{
	return m_paEmphasisEntries.size();
}

/// Clear policy entries
void CvEmphasisXMLEntries::DeleteArray()
{
	for (std::vector<CvEmphasisEntry*>::iterator it = m_paEmphasisEntries.begin(); it != m_paEmphasisEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paEmphasisEntries.clear();
}

/// Get a specific entry
CvEmphasisEntry *CvEmphasisXMLEntries::GetEntry(int index)
{
	return m_paEmphasisEntries[index];
}

//=====================================
// CvCityEmphases
//=====================================
/// Constructor
CvCityEmphases::CvCityEmphases ()
{
	m_pbEmphasize = NULL;
}

/// Destructor
CvCityEmphases::~CvCityEmphases (void)
{

}

/// Initialize
void CvCityEmphases::Init(CvEmphasisXMLEntries *pEmphases, CvCity *pCity)
{
	// Store off the pointers to objects we'll need later
	m_pEmphases = pEmphases;
	m_pCity = pCity;

	Reset();
}

/// Deallocate memory created in initialize
void CvCityEmphases::Uninit()
{
	SAFE_DELETE_ARRAY(m_pbEmphasize);
}

/// Reset status arrays to all false
void CvCityEmphases::Reset()
{
	Uninit();

	m_iEmphasizeAvoidGrowthCount = 0;
	m_iEmphasizeGreatPeopleCount = 0;

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiEmphasizeYieldCount[iI] = 0;
	}

	CvAssertMsg(m_pbEmphasize == NULL, "m_pbEmphasize not NULL!!!");
	CvAssertMsg(GC.getNumEmphasisInfos() > 0,  "GC.getNumEmphasizeInfos() is not greater than zero but an array is being allocated in CvCityEmphases::Reset");
	m_pbEmphasize = FNEW(bool[GC.getNumEmphasisInfos()], c_eCiv5GameplayDLL, 0);
	for (int iI = 0; iI < GC.getNumEmphasisInfos(); iI++)
	{
		m_pbEmphasize[iI] = false;
	}
}

/// How much does this city emphasize avoiding growth?
int CvCityEmphases::GetEmphasizeAvoidGrowthCount()
{
	return m_iEmphasizeAvoidGrowthCount;
}

/// Is this city avoiding growth at all?
bool CvCityEmphases::IsEmphasizeAvoidGrowth()
{
	return (GetEmphasizeAvoidGrowthCount() > 0);
}

/// What is this city's yield boost due to emphasis?
int CvCityEmphases::GetEmphasizeYieldCount(YieldTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	if(eIndex >= 0 && eIndex < NUM_YIELD_TYPES)
		return m_aiEmphasizeYieldCount[eIndex];
	return 0; // default set during "reset"
}

/// Is the city emphasizing a specific yield?
bool CvCityEmphases::IsEmphasizeYield(YieldTypes eIndex)
{
	return (GetEmphasizeYieldCount(eIndex) > 0);
}

/// Is this emphasis turned on?
bool CvCityEmphases::IsEmphasize(EmphasizeTypes eIndex)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumEmphasisInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvAssertMsg(m_pbEmphasize != NULL, "m_pbEmphasize is not expected to be equal with NULL");

	if(!m_pbEmphasize) return false;

	return m_pbEmphasize[eIndex];
}

/// Turn on this emphasis
void CvCityEmphases::SetEmphasize(EmphasizeTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumEmphasisInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (IsEmphasize(eIndex) != bNewValue)
	{
		m_pbEmphasize[eIndex] = bNewValue;

		CvEmphasisEntry* pkEmphasis = GC.getEmphasisInfo(eIndex);

		if (pkEmphasis->IsAvoidGrowth())
		{
			m_iEmphasizeAvoidGrowthCount += ((IsEmphasize(eIndex)) ? 1 : -1);
			CvAssert(GetEmphasizeAvoidGrowthCount() >= 0);
		}

		if (pkEmphasis->IsGreatPeople())
		{
			m_iEmphasizeGreatPeopleCount += ((IsEmphasize(eIndex)) ? 1 : -1);
		}

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if (pkEmphasis->GetYieldChange(iI))
			{
				m_aiEmphasizeYieldCount[iI] += ((IsEmphasize(eIndex)) ? 1 : -1);
				CvAssert(GetEmphasizeYieldCount((YieldTypes)iI) >= 0);
			}
		}

		if ((m_pCity->getOwner() == GC.getGame().getActivePlayer()) && m_pCity->isCitySelected())
		{
			GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

/// Serialization read
void CvCityEmphases::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_iEmphasizeAvoidGrowthCount;
	kStream >> m_iEmphasizeGreatPeopleCount;

	kStream >> m_aiEmphasizeYieldCount;
	ArrayWrapper<bool> wrapgetNumEmphasisInfos(GC.getNumEmphasisInfos(), m_pbEmphasize);
	kStream >> wrapgetNumEmphasisInfos;
}

/// Serialization write
void CvCityEmphases::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << m_iEmphasizeAvoidGrowthCount;
	kStream << m_iEmphasizeGreatPeopleCount;

	kStream << m_aiEmphasizeYieldCount;
	kStream << ArrayWrapper<bool>(GC.getNumEmphasisInfos(), m_pbEmphasize);
}