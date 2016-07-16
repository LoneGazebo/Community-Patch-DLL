#include "CvGameCoreDLLPCH.h"
#include "CvAStarNode.h"

CvAStarNode::CvAStarNode()
{
	m_iX = -1;
	m_iY = -1;

	clear();
}

void CvAStarNode::clear()
{
	m_iTotalCost = 0;
	m_iKnownCost = 0;
	m_iHeuristicCost = 0;
	m_iMoves = 0;
	m_iTurns = 0;

	m_pParent = NULL;
	m_eCvAStarListType = NO_CVASTARLIST;

	m_apChildren.clear();
	m_kCostCacheData.clear();
}