/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"

// include this after all other headers!
#include "LintFree.h"

CvProjectEntry::CvProjectEntry(void):
m_piResourceQuantityRequirements(NULL),
m_piVictoryThreshold(NULL),
m_piVictoryMinThreshold(NULL),
m_piProjectsNeeded(NULL),
m_piFlavorValue(NULL)
{
}
//------------------------------------------------------------------------------
CvProjectEntry::~CvProjectEntry(void)
{
	SAFE_DELETE_ARRAY(m_piResourceQuantityRequirements);
	SAFE_DELETE_ARRAY(m_piVictoryThreshold);
	SAFE_DELETE_ARRAY(m_piVictoryMinThreshold);
	SAFE_DELETE_ARRAY(m_piProjectsNeeded);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
}
//------------------------------------------------------------------------------
bool CvProjectEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	m_iMaxGlobalInstances = kResults.GetInt("MaxGlobalInstances");
	m_iMaxTeamInstances = kResults.GetInt("MaxTeamInstances");
	m_iProductionCost = kResults.GetInt("Cost");
	m_iNukeInterception = kResults.GetInt("NukeInterception");
	m_iCultureBranchesRequired = kResults.GetInt("CultureBranchesRequired");
	m_iTechShare = kResults.GetInt("TechShare");
	m_iVictoryDelayPercent = kResults.GetInt("VictoryDelayPercent");

	m_bSpaceship = kResults.GetBool("Spaceship");
	m_bAllowsNukes = kResults.GetBool("AllowsNukes");

	m_strMovieArtDef = kResults.GetText("MovieDefineTag");

	const char* szVictoryPrereq = kResults.GetText("VictoryPrereq");
	m_iVictoryPrereq = GC.getInfoTypeForString(szVictoryPrereq, true);

	const char* szTechPrereq = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTechPrereq, true);

	const char* szEveryoneSpecialUnit = kResults.GetText("EveryoneSpecialUnit");
	m_iEveryoneSpecialUnit = GC.getInfoTypeForString(szEveryoneSpecialUnit, true);

	const char* szCreateSound = kResults.GetText("CreateSound");
	SetCreateSound(szCreateSound);

	const char* szAnyonePrereqProject = kResults.GetText("AnyonePrereqProject");
	m_iAnyoneProjectPrereq = GC.getInfoTypeForString(szAnyonePrereqProject, true);

	//Arrays
	const char* szProjectType = GetType();
	kUtility.PopulateArrayByValue(m_piResourceQuantityRequirements, "Resources", "Project_ResourceQuantityRequirements", "ResourceType", "ProjectType", szProjectType, "Quantity");

	//Victory Thresholds
	{
		const int iNumVictories = kUtility.MaxRows("Victories");

		kUtility.InitializeArray(m_piVictoryThreshold, iNumVictories);
		kUtility.InitializeArray(m_piVictoryMinThreshold, iNumVictories);

		Database::Results kDBResults;
		char szQuery[512] = {0};
		sprintf_s(szQuery, "select VictoryType, Threshold, MinThreshold from Project_VictoryThresholds where ProjectType = '%s';", szProjectType);
		if(DB.Execute(kDBResults, szQuery))
		{
			while(kDBResults.Step())
			{
				const char* szVictoryType = kDBResults.GetText("VictoryType");
				const int idx = GC.getInfoTypeForString(szVictoryType, true);

				const int iThreshold = kDBResults.GetInt("Threshold");
				const int iMinThreshold = kDBResults.GetInt("MinThreshold");

				m_piVictoryThreshold[idx] = iThreshold;
				m_piVictoryMinThreshold[idx] = iMinThreshold;
			}
		}
	}

	kUtility.SetFlavors(m_piFlavorValue, "Project_Flavors", "ProjectType", szProjectType);
	kUtility.PopulateArrayByValue(m_piProjectsNeeded, "Projects", "Project_Prereqs", "PrereqProjectType", "ProjectType", szProjectType, "AmountNeeded");

	return true;
}
//------------------------------------------------------------------------------
/// What victory does this contribute to?
int CvProjectEntry::GetVictoryPrereq() const
{
	return m_iVictoryPrereq;
}

/// Technology prerequisite
int CvProjectEntry::GetTechPrereq() const
{
	return m_iTechPrereq;
}

/// Is there a project someone must have completed?
int CvProjectEntry::GetAnyoneProjectPrereq() const
{
	return m_iAnyoneProjectPrereq;
}

/// Set whether or not there a project someone must have completed
void CvProjectEntry::SetAnyoneProjectPrereq(int i)
{
	m_iAnyoneProjectPrereq = i;
}

/// Is there a maximum number of these in the world?
int CvProjectEntry::GetMaxGlobalInstances() const
{
	return m_iMaxGlobalInstances;
}

/// Is there a maximum number of these for one team?
int CvProjectEntry::GetMaxTeamInstances() const
{
	return m_iMaxTeamInstances;
}

/// Shields to construct the building
int CvProjectEntry::GetProductionCost() const
{
	return m_iProductionCost;
}

/// Percent chance of intercepting nukes
int CvProjectEntry::GetNukeInterception() const
{
	return m_iNukeInterception;
}

/// Number of Policy branches required to build this project
int CvProjectEntry::GetCultureBranchesRequired() const
{
	return m_iCultureBranchesRequired;
}

/// Does it grant all techs known by X civs? (X is value returned)
int CvProjectEntry::GetTechShare() const
{
	return m_iTechShare;
}

/// Special unit enabled by this wonder
int CvProjectEntry::GetEveryoneSpecialUnit() const
{
	return m_iEveryoneSpecialUnit;
}

/// Percent delay in declaring victory
int CvProjectEntry::GetVictoryDelayPercent() const
{
	return m_iVictoryDelayPercent;
}

/// Find value of flavors associated with this building
int CvProjectEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFlavorTypes() && m_piFlavorValue)
	{
		return  m_piFlavorValue[i];
	}

	return 0;
}

/// Is this a spaceship part?
bool CvProjectEntry::IsSpaceship() const
{
	return m_bSpaceship;
}

/// Does this allow you to build nukes?
bool CvProjectEntry::IsAllowsNukes() const
{
	return m_bAllowsNukes;
}

/// Retrieve movie file name
const char* CvProjectEntry::GetMovieArtDef() const
{
	return m_strMovieArtDef;
}

/// Retrieve sound to play on creation
const char* CvProjectEntry::GetCreateSound() const
{
	return m_strCreateSound;
}

/// Set sound to play on creation
void CvProjectEntry::SetCreateSound(const char* szVal)
{
	m_strCreateSound = szVal;
}

// ARRAYS

/// Resources consumed to construct
int CvProjectEntry::GetResourceQuantityRequirement(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumResourceInfos() && m_piResourceQuantityRequirements)
	{
		return  m_piResourceQuantityRequirements[i];
	}

	return -1;
}

/// Maximum number of these needed for victory condition
int CvProjectEntry::GetVictoryThreshold(int i) const
{
	CvAssertMsg(i < GC.getNumVictoryInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumVictoryInfos() && m_piVictoryThreshold)
	{
		return  m_piVictoryThreshold[i];
	}

	return -1;
}

/// Minimum number of these needed for victory condition
int CvProjectEntry::GetVictoryMinThreshold(int i) const
{
	CvAssertMsg(i < GC.getNumVictoryInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumVictoryInfos())
	{
		if (m_piVictoryMinThreshold && m_piVictoryMinThreshold[i] != 0)
		{
			return m_piVictoryMinThreshold[i];
		}

		return GetVictoryThreshold(i);
	}

	return 0;
}

/// Other projects required before this one can be built
int CvProjectEntry::GetProjectsNeeded(int i) const
{
	CvAssertMsg(i < GC.getNumProjectInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumProjectInfos() && m_piProjectsNeeded)
	{
		return m_piProjectsNeeded[i];
	}

	return 0;
}

//=====================================
// CvProjectXMLEntries
//=====================================
/// Constructor
CvProjectXMLEntries::CvProjectXMLEntries(void)
{

}

/// Destructor
CvProjectXMLEntries::~CvProjectXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of project entries
std::vector<CvProjectEntry*>& CvProjectXMLEntries::GetProjectEntries()
{
	return m_paProjectEntries;
}

/// Number of defined projects
int CvProjectXMLEntries::GetNumProjects()
{
	return m_paProjectEntries.size();
}

/// Clear project entries
void CvProjectXMLEntries::DeleteArray()
{
	for (std::vector<CvProjectEntry*>::iterator it = m_paProjectEntries.begin(); it != m_paProjectEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paProjectEntries.clear();
}

/// Get a specific entry
CvProjectEntry *CvProjectXMLEntries::GetEntry(int index)
{
	return m_paProjectEntries[index];
}
