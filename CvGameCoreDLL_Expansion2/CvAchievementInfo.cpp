#include "CvGameCoreDLLPCH.h"
#include "CvAchievementInfo.h"

// must be included after all other headers
#include "LintFree.h"

#if defined(MOD_API_ACHIEVEMENTS) || defined(ACHIEVEMENT_HACKS)

CvAchievementInfo::CvAchievementInfo() :
	m_bBroken(false),
	m_bAchieved(false)
{
}

CvAchievementInfo::~CvAchievementInfo()
{
}

bool CvAchievementInfo::isBroken() const
{
	return m_bBroken;
}

bool CvAchievementInfo::isAchieved() const
{
	return m_bAchieved;
}

bool CvAchievementInfo::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	m_bBroken = kResults.GetBool("Broken");
	m_bAchieved = kResults.GetBool("Achieved");

	return true;
}

CvAchievementXMLEntries::CvAchievementXMLEntries(void)
{

}

/// Destructor
CvAchievementXMLEntries::~CvAchievementXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of policy entries
std::vector<CvAchievementInfo*>& CvAchievementXMLEntries::GetAchievementEntries()
{
	return m_paAchievementEntries;
}

/// Number of defined policies
int CvAchievementXMLEntries::GetNumAchievements()
{
	return m_paAchievementEntries.size();
}

/// Clear policy entries
void CvAchievementXMLEntries::DeleteArray()
{
	for(std::vector<CvAchievementInfo*>::iterator it = m_paAchievementEntries.begin(); it != m_paAchievementEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paAchievementEntries.clear();
}

/// Get a specific entry
CvAchievementInfo* CvAchievementXMLEntries::GetEntry(EAchievement index)
{
	return m_paAchievementEntries[index];
}

#endif
