/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvUnitCycler.h"
#include "CvPlayer.h"
#include "CvUnit.h"
#include "CvGameCoreUtils.h"

//	---------------------------------------------------------------------------
CvUnitCycler::CvUnitCycler(CvPlayer* pkPlayer)
{
	m_pkPlayer = pkPlayer;
}

//	---------------------------------------------------------------------------
void CvUnitCycler::Clear()
{
	m_kNodeList.clear();
}

//	---------------------------------------------------------------------------
void CvUnitCycler::Rebuild(CvUnit *pkStartUnit /* = NULL */)
{
	// Delete contents of old list
	Node* pUnitNode = HeadNode();
	while(pUnitNode != NULL)
	{
		DeleteNode(pUnitNode);
		pUnitNode = HeadNode();
	}

	if (pkStartUnit == NULL)
	{
		// If no unit supplied, use the selected unit.
		auto_ptr<ICvUnit1> pSelectedUnit(DLLUI->GetHeadSelectedUnit());
		pkStartUnit = GC.UnwrapUnitPointer(pSelectedUnit.get());

		if (pkStartUnit && pkStartUnit->getOwner() != m_pkPlayer->GetID())
		{
			// Not ours, fall back to just selecting a worker
			pkStartUnit = NULL;
		}
	}

	CvUnit* pLoopUnit;
	int iLoop;
	CvUnit* pkFirstUnit = m_pkPlayer->firstUnit(&iLoop);
	for(pLoopUnit = pkFirstUnit; pLoopUnit != NULL; pLoopUnit = m_pkPlayer->nextUnit(&iLoop))
	{
		// Reset cycle order (will be used later in this function)
		pLoopUnit->SetCycleOrder(0);

		if (!pkStartUnit)
		{
			// Workers first
			if(pLoopUnit->workRate(true) > 0)
				pkStartUnit = pLoopUnit;
		}
	}

	if (!pkStartUnit)
		pkStartUnit = pkFirstUnit;

	// Add first unit to list
	if(pkStartUnit)
	{
		m_kNodeList.insertAtEnd(pkStartUnit->GetID());

		// Current unit is the first one
		UnitHandle pCurrentUnit = pkStartUnit;
		pCurrentUnit->SetCycleOrder(1);

		// Loop through units until everyone is accounted for
		int iNumUnits = m_pkPlayer->getNumUnits();
		while(m_kNodeList.getLength() < iNumUnits)
		{
			int iBestUnitScore = MAX_INT;
			CvUnit* pBestUnit = NULL;

			int iX = pCurrentUnit->getX();
			int iY = pCurrentUnit->getY();
			for(pLoopUnit = m_pkPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pkPlayer->nextUnit(&iLoop))
			{
				// If we've already added this unit to the cycle list, skip it
				if(pLoopUnit->GetCycleOrder() == 1)
					continue;

				int iScore = plotDistance(iX, iY, pLoopUnit->getX(), pLoopUnit->getY());

				// Closest unit yet
				if(iScore < iBestUnitScore)
				{
					iBestUnitScore = iScore;
					pBestUnit = pLoopUnit;
				}
			}

			CvAssertMsg(pBestUnit, "Didn't find a unit to add to cycle list.");

			if (pBestUnit)
			{
				m_kNodeList.insertAtEnd(pBestUnit->GetID());

				// Now have a new current unit
				pCurrentUnit = pBestUnit;
				pCurrentUnit->SetCycleOrder(1);
			}
			else
				break;
		}
	}
}

//	---------------------------------------------------------------------------
void CvUnitCycler::AddUnit(int iID)
{
	Rebuild();
}
//	---------------------------------------------------------------------------
void CvUnitCycler::RemoveUnit(int iID)
{
	CLLNode<int>* pUnitNode;

	pUnitNode = HeadNode();

	while(pUnitNode != NULL)
	{
		if(pUnitNode->m_data == iID)
		{
			pUnitNode = DeleteNode(pUnitNode);
			break;
		}
		else
		{
			pUnitNode = NextNode(pUnitNode);
		}
	}
}

//	-----------------------------------------------------------------------------------------------
// Returns the next unit in the cycle...
CvUnit *CvUnitCycler::Cycle(CvUnit* pUnit, bool bForward, bool bWorkers, bool* pbWrap)
{
	const CLLNode<int>* pUnitNode;
	const CLLNode<int>* pFirstUnitNode;
	UnitHandle pLoopUnit;

	if (pbWrap != NULL)
	{
		*pbWrap = false;
	}

	pUnitNode = HeadNode();

	if (pUnit != NULL)
	{
		while (pUnitNode != NULL)
		{
			if (m_pkPlayer->getUnit(pUnitNode->m_data) == pUnit)
			{
				if (bForward)
				{
					pUnitNode = NextNode(pUnitNode);
				}
				else
				{
					pUnitNode = PreviousNode(pUnitNode);
				}
				break;
			}

			pUnitNode = NextNode(pUnitNode);
		}
	}

	if (pUnitNode == NULL)
	{
		if (bForward)
		{
			pUnitNode = HeadNode();
		}
		else
		{
			pUnitNode = TailNode();
		}

		if (pbWrap != NULL)
		{
			*pbWrap = true;
		}
	}

	if (pUnitNode != NULL)
	{
		pFirstUnitNode = pUnitNode;

		while (true)
		{
			pLoopUnit = m_pkPlayer->getUnit(pUnitNode->m_data);
			CvAssertMsg(pLoopUnit, "LoopUnit is not assigned a valid value");

			if (pLoopUnit && pLoopUnit->ReadyToSelect())
			{
				if (!bWorkers || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER_SEA)
				{
					if (pUnit && pLoopUnit== pUnit)
					{
						if (pbWrap != NULL)
						{
							*pbWrap = true;
						}
					}

					return pLoopUnit.pointer();
				}
			}

			if (bForward)
			{
				pUnitNode = NextNode(pUnitNode);

				if (pUnitNode == NULL)
				{
					pUnitNode = HeadNode();

					if (pbWrap != NULL)
					{
						*pbWrap = true;
					}
				}
			}
			else
			{
				pUnitNode = PreviousNode(pUnitNode);

				if (pUnitNode == NULL)
				{
					pUnitNode = TailNode();

					if (pbWrap != NULL)
					{
						*pbWrap = true;
					}
				}
			}

			if (pUnitNode == pFirstUnitNode)
			{
				break;
			}
		}
	}

	return NULL;
}


//	---------------------------------------------------------------------------
CvUnitCycler::Node* CvUnitCycler::DeleteNode(CvUnitCycler::Node* pNode)
{
	return m_kNodeList.deleteNode(pNode);
}

//	---------------------------------------------------------------------------
const CvUnitCycler::Node* CvUnitCycler::NextNode(const CvUnitCycler::Node* pNode) const
{
	return m_kNodeList.next(pNode);
}

//	---------------------------------------------------------------------------
CvUnitCycler::Node* CvUnitCycler::NextNode(CvUnitCycler::Node* pNode)
{
	return m_kNodeList.next(pNode);
}

//	---------------------------------------------------------------------------
const CvUnitCycler::Node* CvUnitCycler::PreviousNode(const CvUnitCycler::Node* pNode) const
{
	return m_kNodeList.prev(pNode);
}

//	---------------------------------------------------------------------------
const CvUnitCycler::Node* CvUnitCycler::HeadNode() const
{
	return m_kNodeList.head();
}

//	---------------------------------------------------------------------------
CvUnitCycler::Node* CvUnitCycler::HeadNode()
{
	return m_kNodeList.head();
}

//	---------------------------------------------------------------------------
const CvUnitCycler::Node* CvUnitCycler::TailNode() const
{
	return m_kNodeList.tail();
}

//	---------------------------------------------------------------------------
FDataStream & operator>>(FDataStream & loadFrom, CvUnitCycler & writeTo)
{
	writeTo.Clear();
	loadFrom >> writeTo.m_kNodeList;
	return loadFrom;
}

//	---------------------------------------------------------------------------
FDataStream & operator<<(FDataStream & saveTo, const CvUnitCycler & readFrom)
{
	saveTo << readFrom.m_kNodeList;
	return saveTo;
}
