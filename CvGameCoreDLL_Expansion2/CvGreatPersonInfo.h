#pragma once

#ifndef CV_GREATPERSON_INFO_H
#define CV_GREATPERSON_INFO_H

#if defined(MOD_API_UNIFIED_YIELDS)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvGreatPersonInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGreatPersonInfo : public CvBaseInfo
{
public:
	CvGreatPersonInfo();
	virtual ~CvGreatPersonInfo();

	int GetUnitClassType() const;
	int GetSpecialistType() const;

	// Other
	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iUnitClass;
	int m_iSpecialist;

private:
	CvGreatPersonInfo(const CvGreatPersonInfo&);
	CvGreatPersonInfo& operator=(const CvGreatPersonInfo&);
};

GreatPersonTypes GetGreatPersonFromUnitClass(UnitClassTypes eUnitClass);
GreatPersonTypes GetGreatPersonFromSpecialist(SpecialistTypes eSpecialist);

enum GreatPersonTypes			
{
	NO_GREATPERSON = -1,
};
#endif

#endif
