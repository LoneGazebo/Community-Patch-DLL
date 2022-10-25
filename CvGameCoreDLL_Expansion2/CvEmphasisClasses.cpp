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
#include "CvInfosSerializationHelper.h"
#include "CvEnumMapSerialization.h"

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
CvEmphasisXMLEntries::CvEmphasisXMLEntries(void) : m_paEmphasisEntries()
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
	for(std::vector<CvEmphasisEntry*>::iterator it = m_paEmphasisEntries.begin(); it != m_paEmphasisEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paEmphasisEntries.clear();
}

/// Get a specific entry
CvEmphasisEntry* CvEmphasisXMLEntries::GetEntry(int index)
{
#if defined(MOD_BALANCE_CORE)
	return (index!=NO_EMPHASIZE) ? m_paEmphasisEntries[index] : NULL;
#else
	return m_paEmphasisEntries[index];
#endif
}

//=====================================
// CvCityEmphases
//=====================================
/// Constructor
CvCityEmphases::CvCityEmphases() : m_iEmphasizeAvoidGrowthCount(), m_iEmphasizeGreatPeopleCount(), m_aiEmphasizeYieldCount(), m_pbEmphasize(), m_pCity()
{
}

/// Destructor
CvCityEmphases::~CvCityEmphases(void)
{

}

/// Initialize
void CvCityEmphases::Init(CvCity* pCity)
{
	// Store off the pointers to objects we'll need later
	m_pCity = pCity;

	Reset();
}

/// Deallocate memory created in initialize
void CvCityEmphases::Uninit()
{
	m_aiEmphasizeYieldCount.uninit();
	m_pbEmphasize.uninit();
}

/// Reset status arrays to all false
void CvCityEmphases::Reset()
{
	Uninit();

	m_iEmphasizeAvoidGrowthCount = 0;
	m_iEmphasizeGreatPeopleCount = 0;

	m_aiEmphasizeYieldCount.init(0);
	CvAssertMsg(GC.getNumEmphasisInfos() > 0,  "GC.getNumEmphasizeInfos() is not greater than zero but an array is being allocated in CvCityEmphases::Reset");
	m_pbEmphasize.init(false);
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
	CvAssertMsg(m_pbEmphasize.valid(), "m_pbEmphasize is expected to be valid");

	return m_pbEmphasize[eIndex];
}

/// Turn on this emphasis
void CvCityEmphases::SetEmphasize(EmphasizeTypes eIndex, bool bNewValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumEmphasisInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(IsEmphasize(eIndex) != bNewValue)
	{
		m_pbEmphasize[eIndex] = bNewValue;

		CvEmphasisEntry* pkEmphasis = GC.getEmphasisInfo(eIndex);

		if(pkEmphasis->IsAvoidGrowth())
		{
			m_iEmphasizeAvoidGrowthCount += ((IsEmphasize(eIndex)) ? 1 : -1);
			CvAssert(GetEmphasizeAvoidGrowthCount() >= 0);
		}

		if(pkEmphasis->IsGreatPeople())
		{
			m_iEmphasizeGreatPeopleCount += ((IsEmphasize(eIndex)) ? 1 : -1);
		}

		for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if(pkEmphasis->GetYieldChange(iI))
			{
				m_aiEmphasizeYieldCount[iI] += ((IsEmphasize(eIndex)) ? 1 : -1);
				CvAssert(GetEmphasizeYieldCount((YieldTypes)iI) >= 0);
			}
		}

		if((m_pCity->getOwner() == GC.getGame().getActivePlayer()) && m_pCity->isCitySelected())
		{
			GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

///
template<typename CityEmphases, typename Visitor>
void CvCityEmphases::Serialize(CityEmphases& cityEmphases, Visitor& visitor)
{
	visitor(cityEmphases.m_iEmphasizeAvoidGrowthCount);
	visitor(cityEmphases.m_iEmphasizeGreatPeopleCount);
	visitor(cityEmphases.m_aiEmphasizeYieldCount);
	visitor(cityEmphases.m_pbEmphasize);
}

/// Serialization read
void CvCityEmphases::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	CvCityEmphases::Serialize(*this, serialVisitor);
}

/// Serialization write
void CvCityEmphases::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	CvCityEmphases::Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvCityEmphases& cityEmphases)
{
	cityEmphases.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvCityEmphases& cityEmphases)
{
	cityEmphases.Write(stream);
	return stream;
}