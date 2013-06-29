#pragma once

#ifndef CvDLLFAStarIFaceBase_h
#define CvDLLFAStarIFaceBase_h

//
// abstract interface for FAStar functions used by DLL
//

class CvAStar;
class FAStarNode;

// Function prototype for Cost and Validity functions
typedef int(*CvAPointFunc)(int, int, const void*, CvAStar*);
typedef int(*CvAHeuristic)(int, int, int, int);
typedef int(*CvAStarFunc)(FAStarNode*, FAStarNode*, int, const void*, CvAStar*);

class CvDLLFAStarIFaceBase
{
public:
	virtual CvAStar* create() = 0;
	virtual void destroy(CvAStar*& ptr, bool bSafeDelete=true) = 0;
	virtual bool GeneratePath(CvAStar*, int iXstart, int iYstart, int iXdest, int iYdest, bool bUseHexes = false, bool bCardinalOnly = false, int iInfo = 0, bool bReuse = false) = 0;
	virtual void Initialize(CvAStar*, int iColumns, int iRows, bool bWrapX, bool bWrapY, CvAPointFunc DestValidFunc, CvAHeuristic HeuristicFunc, CvAStarFunc CostFunc, CvAStarFunc ValidFunc, CvAStarFunc NotifyChildFunc, CvAStarFunc NotifyListFunc, void* pData) = 0;
	virtual void SetData(CvAStar*, const void* pData) = 0;
	virtual FAStarNode* GetLastNode(CvAStar*) = 0;
	virtual bool IsPathStart(CvAStar*, int iX, int iY) = 0;
	virtual bool IsPathDest(CvAStar*, int iX, int iY) = 0;
	virtual int GetStartX(CvAStar*) = 0;
	virtual int GetStartY(CvAStar*) = 0;
	virtual int GetDestX(CvAStar*) = 0;
	virtual int GetDestY(CvAStar*) = 0;
	virtual int GetInfo(CvAStar*) = 0;
	virtual void ForceReset(CvAStar*) = 0;
};

#endif	// CvDLLFAStarIFaceBase_h
