/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#ifndef CVUNITCYCLER_H
#define CVUNITCYCLER_H
#pragma once
#include "LinkedList.h"
class CvPlayer;

class CvUnitCycler
{
public:

	CvUnitCycler(CvPlayer* pkPlayer);
	typedef CLLNode<int> Node;
	typedef CLinkList<int> NodeList;

	void		Clear();
	void		Rebuild(CvUnit* pkStartUnit = NULL);
	CvUnit*		Cycle(CvUnit* pUnit, bool bForward, bool bWorkers, bool* pbWrap);

	void		RemoveUnit(int iID);
	void		AddUnit(int iID);
	Node*		DeleteNode(Node* pNode);
	Node*		NextNode(Node* pNode);
	const Node*	NextNode(const Node* pNode) const;
	const Node* PreviousNode(const Node* pNode) const;
	const Node* HeadNode() const;
	Node*		HeadNode();
	const Node* TailNode() const;

protected:
	NodeList	m_kNodeList;
	CvPlayer*	m_pkPlayer;

	friend FDataStream & operator>>(FDataStream & loadFrom, CvUnitCycler & writeTo);
	friend FDataStream & operator<<(FDataStream & saveTo, const CvUnitCycler & readFrom);
};

FDataStream & operator>>(FDataStream & loadFrom, CvUnitCycler & writeTo);
FDataStream & operator<<(FDataStream & saveTo, const CvUnitCycler & readFrom);

#endif
