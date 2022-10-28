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
	m_iTotalCost = -1; //marker for untouched node
	m_iKnownCost = 0;
	m_iHeuristicCost = 0;
	m_iMoves = 0;
	m_iTurns = 0;
	m_iStartMovesForTurn = 1; //safe default

	m_bIsOpen = false;
	m_pParent = NULL;

	m_apChildren.clear();
	m_kCostCacheData.clear();
}