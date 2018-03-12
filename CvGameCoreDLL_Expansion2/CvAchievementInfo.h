#pragma once

#ifndef CV_ACHIEVEMENT_INFO_H
#define CV_ACHIEVEMENT_INFO_H

#if defined(MOD_API_ACHIEVEMENTS) || defined(ACHIEVEMENT_HACKS)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvAchievementInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAchievementInfo : public CvBaseInfo
{
public:
	CvAchievementInfo();
	virtual ~CvAchievementInfo();

	bool isBroken() const;
	bool isAchieved() const;

	// Other
	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	bool m_bBroken;
	bool m_bAchieved;

private:
	CvAchievementInfo(const CvAchievementInfo&);
	CvAchievementInfo& operator=(const CvAchievementInfo&);
};


class CvAchievementXMLEntries
{
public:
	CvAchievementXMLEntries(void);
	~CvAchievementXMLEntries(void);

	// Accessor functions
	std::vector<CvAchievementInfo*>& GetAchievementEntries();
	int GetNumAchievements();
	_Ret_maybenull_ CvAchievementInfo* GetEntry(EAchievement index);

	void DeleteArray();

private:
	std::vector<CvAchievementInfo*> m_paAchievementEntries;
};

#endif

#endif
