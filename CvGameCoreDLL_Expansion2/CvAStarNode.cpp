#include "CvGameCoreDLLPCH.h"
#include "CvAStarNode.h"

CvAStarNode::CvAStarNode()
{
	m_iX = -1;
	m_iY = -1;
	m_iTotalCost = 0;
	m_iKnownCost = 0;
	m_iHeuristicCost = 0;
	m_iMoves = 0;
	m_iTurns = 0;

	m_bOnStack = false;

	m_eCvAStarListType = NO_CVASTARLIST;

	m_pParent = NULL;
	m_pNext = NULL;
	m_pPrev = NULL;
	m_pStack = NULL;
}

void CvAStarNode::clear()
{
	m_iTotalCost = 0;
	m_iKnownCost = 0;
	m_iHeuristicCost = 0;
	m_iMoves = 0;
	m_iTurns = 0;

	m_pParent = NULL;
	m_pNext = NULL;
	m_pPrev = NULL;
	m_eCvAStarListType = NO_CVASTARLIST;

	m_pStack = NULL;
	m_bOnStack = false;

	m_apChildren.clear();
	m_kCostCacheData.clear();
}

void CvAStarNode::setPrev(CvAStarNode * p)
{
	if (p != this)
		m_pPrev = p;
	else
		OutputDebugString("endless loop in node list!");
}

void CvAStarNode::setNext(CvAStarNode * p)
{
	if (p != this)
		m_pNext = p;
	else
		OutputDebugString("endless loop in node list!");
}

CvAStarNode* CvAStarNode::getPrev() const
{
	return m_pPrev;
}

CvAStarNode* CvAStarNode::getNext() const
{
	return m_pNext;
}