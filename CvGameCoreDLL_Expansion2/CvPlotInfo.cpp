#include "CvGameCoreDLLPCH.h"
#include "CvPlotInfo.h"

// must be included after all other headers
#include "LintFree.h"

#if defined(MOD_API_PLOT_YIELDS)

CvPlotInfo::CvPlotInfo() :
	m_bWater(false),
	m_bImpassable(false),
#if defined(MOD_PLOTS_EXTENSIONS)
	m_ppiAdjacentFeatureYieldChange(),
#endif
	m_piYields(NULL)
{
}

CvPlotInfo::~CvPlotInfo()
{
#if defined(MOD_PLOTS_EXTENSIONS)
	m_ppiAdjacentFeatureYieldChange.clear();
#endif
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

#if defined(MOD_PLOTS_EXTENSIONS)
//------------------------------------------------------------------------------
int CvPlotInfo::GetAdjacentFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield) const
{
	int iFeature = (int)eFeature;
	int iYield = (int)eYield;

	CvAssertMsg(iFeature < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(iFeature > -1, "Index out of bounds");
	CvAssertMsg(iYield < NUM_YIELD_TYPES, "Index out of bounds");
	CvAssertMsg(iYield > -1, "Index out of bounds");

	std::map<int, std::map<int, int>>::const_iterator itFeature = m_ppiAdjacentFeatureYieldChange.find(iFeature);
	if (itFeature != m_ppiAdjacentFeatureYieldChange.end())
	{
		std::map<int, int>::const_iterator itYield = itFeature->second.find(iYield);
		if (itYield != itFeature->second.end())
		{
			return itYield->second;
		}
	}

	return 0;
}
//------------------------------------------------------------------------------
/// Quick check if a plot gains yields from any adjacent feature
bool CvPlotInfo::IsAdjacentFeatureYieldChange() const
{
	return !m_ppiAdjacentFeatureYieldChange.empty();
}
//------------------------------------------------------------------------------
/// Check if a plot gains yields from a particular feature
bool CvPlotInfo::IsAdjacentFeatureYieldChange(FeatureTypes eFeature) const
{
	int iFeature = (int)eFeature;

	CvAssertMsg(iFeature < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(iFeature > -1, "Index out of bounds");

	std::map<int, std::map<int, int>>::const_iterator itFeature = m_ppiAdjacentFeatureYieldChange.find(iFeature);
	if (itFeature != m_ppiAdjacentFeatureYieldChange.end())
	{
		return true;
	}

	return false;
}
#endif

bool CvPlotInfo::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	m_bWater = kResults.GetBool("Water");
	m_bImpassable = kResults.GetBool("Impassable");

	//Arrays
	const char* szPlotType = GetType();
	kUtility.SetYields(m_piYields, "Plot_Yields", "PlotType", szPlotType);

#if defined(MOD_PLOTS_EXTENSIONS)
	//Plot_AdjacentFeatureYieldChanges
	{
		std::string strKey("Plot_AdjacentFeatureYieldChanges");
		Database::Results* pResults = kUtility.GetResults(strKey);
		if (pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Features.ID as FeatureID, Yields.ID as YieldID, Yield from Plot_AdjacentFeatureYieldChanges inner join Features on FeatureType = Features.Type inner join Yields on YieldType = Yields.Type where PlotType = ?");
		}

		pResults->Bind(1, szPlotType);

		while (pResults->Step())
		{
			const int iFeature = pResults->GetInt(0);
			const int iYield = pResults->GetInt(1);
			const int iYieldChange = pResults->GetInt(2);

			m_ppiAdjacentFeatureYieldChange[iFeature][iYield] += iYieldChange;
		}

		pResults->Reset();

		//Trim extra memory off container since this is mostly read-only.
		std::map<int, std::map<int, int>>(m_ppiAdjacentFeatureYieldChange).swap(m_ppiAdjacentFeatureYieldChange);
	}
#endif

	return true;
}

#endif
