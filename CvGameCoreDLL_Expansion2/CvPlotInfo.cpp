#include "CvGameCoreDLLPCH.h"
#include "CvPlotInfo.h"

// must be included after all other headers
#include "LintFree.h"

#if defined(MOD_API_PLOT_YIELDS)

CvPlotInfo::CvPlotInfo() :
	m_bWater(false),
	m_bImpassable(false),
	m_piYields(NULL)
{
}

CvPlotInfo::~CvPlotInfo()
{
	SAFE_DELETE_ARRAY(m_piYields);
}

bool CvPlotInfo::isWater() const
{
	return m_bWater;
}

bool CvPlotInfo::isImpassable() const
{
	return m_bImpassable;
}

int CvPlotInfo::getYield(int i) const
{
	CvAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piYields ? m_piYields[i] : -1;
}

bool CvPlotInfo::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	m_bWater = kResults.GetBool("Water");
	m_bImpassable = kResults.GetBool("Impassable");

	//Arrays
	const char* szPlotType = GetType();
	kUtility.SetYields(m_piYields, "Plot_Yields", "PlotType", szPlotType);

	return true;
}

#endif
