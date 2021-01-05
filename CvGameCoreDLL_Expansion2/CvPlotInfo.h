#pragma once

#ifndef CV_PLOT_INFO_H
#define CV_PLOT_INFO_H

#if defined(MOD_API_PLOT_YIELDS)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvPlotInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlotInfo : public CvBaseInfo
{
public:
	CvPlotInfo();
	virtual ~CvPlotInfo();

	bool isWater() const;
	bool isImpassable() const;

	// Arrays
#if defined(MOD_PLOTS_EXTENSIONS)
	int GetAdjacentFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield) const;
	bool IsAdjacentFeatureYieldChange() const;
	bool IsAdjacentFeatureYieldChange(FeatureTypes eFeature) const;
#endif
	int getYield(int i) const;

	// Other
	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	bool m_bWater;
	bool m_bImpassable;

	// Arrays
#if defined(MOD_PLOTS_EXTENSIONS)
	std::map<int, std::map<int, int>> m_ppiAdjacentFeatureYieldChange;
#endif
	int* m_piYields;

private:
	CvPlotInfo(const CvPlotInfo&);
	CvPlotInfo& operator=(const CvPlotInfo&);
};

#endif

#endif
