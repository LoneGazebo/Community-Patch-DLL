#pragma once
// LinkedList.h
// A doubly-linked list

#ifndef		LINKEDLIST_H
#define		LINKEDLIST_H
#pragma		once

template <class tVARTYPE> class CLinkList;


template <class tVARTYPE> class CLLNode
{

friend class CLinkList<tVARTYPE>;

public:

    CLLNode(const tVARTYPE& val)
          {
	          m_data = val;

	          m_pNext = NULL;
	          m_pPrev = NULL;
          }
	virtual ~CLLNode() {}

	tVARTYPE	m_data;		//list of vartype

protected:

	CLLNode<tVARTYPE>*	m_pNext;
	CLLNode<tVARTYPE>*	m_pPrev;

};


template <class tVARTYPE> class CLinkList
{

public:

	CLinkList();
	virtual ~CLinkList();

	void clear();

	void insertAtBeginning(const tVARTYPE& val);
	void insertAtEnd(const tVARTYPE& val);
	void insertBefore(const tVARTYPE& val, CLLNode<tVARTYPE>* pThisNode);
	void insertAfter(const tVARTYPE& val, CLLNode<tVARTYPE>* pThisNode);
	CLLNode<tVARTYPE>* deleteNode(CLLNode<tVARTYPE>* pNode);
	void moveToEnd(CLLNode<tVARTYPE>* pThisNode);

	const CLLNode<tVARTYPE>* next(const CLLNode<tVARTYPE>* pNode) const;
	CLLNode<tVARTYPE>* next(CLLNode<tVARTYPE>* pNode);

	const CLLNode<tVARTYPE>* prev(const CLLNode<tVARTYPE>* pNode) const;
	CLLNode<tVARTYPE>* prev(CLLNode<tVARTYPE>* pNode);

	CLLNode<tVARTYPE>* nodeNum(int iNum) const;
	void swapUp(int iNum);

	void Read( FDataStream& kStream );
	void Write( FDataStream& kStream ) const;

	int getLength() const
	{
		return m_iLength;
	}

	const CLLNode<tVARTYPE>* head() const
	{
		return m_pHead;
	}

	CLLNode<tVARTYPE>* head()
	{
		return m_pHead;
	}

	const CLLNode<tVARTYPE>* tail() const
	{
		return m_pTail;
	}

	CLLNode<tVARTYPE>* tail()
	{
		return m_pTail;
	}

	bool operator==(const CLinkList & rhs) const;
	bool operator!=(const CLinkList & rhs) const;

protected:
	int m_iLength;

	CLLNode<tVARTYPE>* m_pHead;
	CLLNode<tVARTYPE>* m_pTail;
};



//constructor
//resets local vars
template <class tVARTYPE>
inline CLinkList<tVARTYPE>::CLinkList()
{
	m_iLength = 0;

	m_pHead = NULL;
	m_pTail = NULL;
}


//Destructor
//resets local vars
template <class tVARTYPE>
inline CLinkList<tVARTYPE>::~CLinkList()
{
  clear();
}


template <class tVARTYPE>
inline void CLinkList<tVARTYPE>::clear()
{
	CLLNode<tVARTYPE>* pCurrNode;
	CLLNode<tVARTYPE>* pNextNode;

	pCurrNode = m_pHead;
	while (pCurrNode != NULL)
	{
		pNextNode = pCurrNode->m_pNext;
		SAFE_DELETE(pCurrNode);
		pCurrNode = pNextNode;
	}

	m_iLength = 0;

	m_pHead = NULL;
	m_pTail = NULL;
}


//inserts at the tail of the list
template <class tVARTYPE>
inline void CLinkList<tVARTYPE>::insertAtBeginning(const tVARTYPE& val)
{	
	CLLNode<tVARTYPE>* pNode;

	assert((m_pHead == NULL) || (m_iLength > 0));

	pNode = FNEW(CLLNode<tVARTYPE>(val), c_eCiv5GameplayDLL, 0);

	if (m_pHead != NULL)
	{
		m_pHead->m_pPrev = pNode;
		pNode->m_pNext = m_pHead;
		m_pHead = pNode;
	}
	else
	{
		m_pHead = pNode;
		m_pTail = pNode;
	}

	m_iLength++;
}


//inserts at the tail of the list
template <class tVARTYPE>
inline void CLinkList<tVARTYPE>::insertAtEnd(const tVARTYPE& val)
{	
	CLLNode<tVARTYPE>* pNode;

	assert((m_pHead == NULL) || (m_iLength > 0));

	pNode = FNEW(CLLNode<tVARTYPE>(val), c_eCiv5GameplayDLL, 0);

	if (m_pTail != NULL)
	{
		m_pTail->m_pNext = pNode;
		pNode->m_pPrev = m_pTail;
		m_pTail = pNode;
	}
	else
	{
		m_pHead = pNode;
		m_pTail = pNode;
	}

	m_iLength++;
}


//inserts before the specified node
template <class tVARTYPE>
inline void CLinkList<tVARTYPE>::insertBefore(const tVARTYPE& val, CLLNode<tVARTYPE>* pThisNode)
{
	CLLNode<tVARTYPE>* pNode;

	assert((m_pHead == NULL) || (m_iLength > 0));

	if ((pThisNode == NULL) || (pThisNode->m_pPrev == NULL))
	{
		insertAtBeginning(val);
		return;
	}

	pNode = FNEW(CLLNode<tVARTYPE>(val), c_eCiv5GameplayDLL, 0);

	pThisNode->m_pPrev->m_pNext = pNode;
	pNode->m_pPrev = pThisNode->m_pPrev;
	pThisNode->m_pPrev = pNode;
	pNode->m_pNext = pThisNode;

	m_iLength++;
}


//inserts after the specified node
template <class tVARTYPE>
inline void CLinkList<tVARTYPE>::insertAfter(const tVARTYPE& val, CLLNode<tVARTYPE>* pThisNode)
{
	CLLNode<tVARTYPE>* pNode;

	assert((m_pHead == NULL) || (m_iLength > 0));

	if ((pThisNode == NULL) || (pThisNode->m_pNext == NULL))
	{
		insertAtEnd(val);
		return;
	}

	pNode = FNEW(CLLNode<tVARTYPE>(val), c_eCiv5GameplayDLL, 0);

	pThisNode->m_pNext->m_pPrev = pNode;
	pNode->m_pNext              = pThisNode->m_pNext;
	pThisNode->m_pNext          = pNode;
	pNode->m_pPrev			        = pThisNode;

	m_iLength++;
}


template <class tVARTYPE>
inline CLLNode<tVARTYPE>* CLinkList<tVARTYPE>::deleteNode(CLLNode<tVARTYPE>* pNode)
{
	CLLNode<tVARTYPE>* pPrevNode;
	CLLNode<tVARTYPE>* pNextNode;

	assert(pNode != NULL);
	if (!pNode) return NULL;

	pPrevNode = pNode->m_pPrev;
	pNextNode = pNode->m_pNext;

	if ((pPrevNode != NULL) && (pNextNode != NULL))
	{
		pPrevNode->m_pNext = pNextNode;
		pNextNode->m_pPrev = pPrevNode;
	}
	else if (pPrevNode != NULL)
	{
		pPrevNode->m_pNext = NULL;
		m_pTail = pPrevNode;
	}
	else if (pNextNode != NULL)
	{
		pNextNode->m_pPrev = NULL;
		m_pHead = pNextNode;
	}
	else
	{
		m_pHead = NULL;
		m_pTail = NULL;
	}

	SAFE_DELETE(pNode);

	m_iLength--;

	return pNextNode;
}


template <class tVARTYPE>
inline void CLinkList<tVARTYPE>::moveToEnd(CLLNode<tVARTYPE>* pNode)
{
	CLLNode<tVARTYPE>* pPrevNode;
	CLLNode<tVARTYPE>* pNextNode;

	assert(pNode != NULL);

	if (getLength() == 1)
	{
	return;
	}

	if (pNode == m_pTail)
	{
		return;
	}

	pPrevNode = pNode->m_pPrev;
	pNextNode = pNode->m_pNext;

	if ((pPrevNode != NULL) && (pNextNode != NULL))
	{
		pPrevNode->m_pNext = pNextNode;
		pNextNode->m_pPrev = pPrevNode;
	}
	else if (pPrevNode != NULL)
	{
		pPrevNode->m_pNext = NULL;
		m_pTail = pPrevNode;
	}
	else if (pNextNode != NULL)
	{
		pNextNode->m_pPrev = NULL;
		m_pHead = pNextNode;
	}
	else
	{
		m_pHead = NULL;
		m_pTail = NULL;
	}

	pNode->m_pNext = NULL;
	m_pTail->m_pNext = pNode;
	pNode->m_pPrev = m_pTail;
	m_pTail = pNode;
}


template <class tVARTYPE>
inline const CLLNode<tVARTYPE>* CLinkList<tVARTYPE>::next(const CLLNode<tVARTYPE>* pNode) const
{
	assert(pNode != NULL);
	return (pNode) ? (pNode->m_pNext) : (NULL);
}

template <class tVARTYPE>
inline CLLNode<tVARTYPE>* CLinkList<tVARTYPE>::next(CLLNode<tVARTYPE>* pNode)
{
	assert(pNode != NULL);
	return (pNode) ? (pNode->m_pNext) : (NULL);
}


template <class tVARTYPE>
inline const CLLNode<tVARTYPE>* CLinkList<tVARTYPE>::prev(const CLLNode<tVARTYPE>* pNode) const
{
	assert(pNode != NULL);
	return (pNode) ? (pNode->m_pPrev) : (NULL);
}

template <class tVARTYPE>
inline CLLNode<tVARTYPE>* CLinkList<tVARTYPE>::prev(CLLNode<tVARTYPE>* pNode)
{
	assert(pNode != NULL);
	return (pNode) ? (pNode->m_pPrev) : (NULL);
}

template <class tVARTYPE>
inline CLLNode<tVARTYPE>* CLinkList<tVARTYPE>::nodeNum(int iNum) const
{
	CLLNode<tVARTYPE>* pNode;
	int iCount;

	iCount = 0;
	pNode = m_pHead;

	while (pNode != NULL)
	{
		if (iCount == iNum)
		{
			return pNode;
		}

		iCount++;
		pNode = pNode->m_pNext;
	}

	return NULL;
}


template <class tVARTYPE>
inline void CLinkList<tVARTYPE>::swapUp(int iNum)
{
	CLLNode<tVARTYPE>* pPrevNode;
	CLLNode<tVARTYPE>* pNextNode;
	CLLNode<tVARTYPE>* pNextNextNode;

	if ( iNum > m_iLength - 1 )
		return;

	int iCount = 0;
	CLLNode<tVARTYPE>*pNode = m_pHead;

	while (pNode != NULL)
	{
		if (iCount == iNum)
		{
			break;
		}
		iCount++;
		pNode = pNode->m_pNext;
	}

	pPrevNode = pNode->m_pPrev;
	pNextNode = pNode->m_pNext;
	if (!pNextNode)
		return;
	pNextNextNode = pNextNode->m_pNext;

	if ((pPrevNode != NULL) && (pNextNextNode != NULL))
	{
		pPrevNode->m_pNext = pNextNode;
		pNextNode->m_pPrev = pPrevNode;
		pNextNode->m_pNext = pNode;
		pNode->m_pPrev = pNextNode;
		pNode->m_pNext = pNextNextNode;
		pNextNextNode->m_pPrev = pNode;
	}
	else if ((pPrevNode == NULL) && (pNextNextNode != NULL))
	{
		pNextNode->m_pPrev = NULL;
		pNextNode->m_pNext = pNode;
		pNode->m_pPrev = pNextNode;
		pNode->m_pNext = pNextNextNode;
		pNextNextNode->m_pPrev = pNode;
	}
	else if ((pPrevNode != NULL) && (pNextNextNode == NULL))
	{
		pPrevNode->m_pNext = pNextNode;
		pNextNode->m_pPrev = pPrevNode;
		pNextNode->m_pNext = pNode;
		pNode->m_pPrev = pNextNode;
		pNode->m_pNext = NULL;
	}
	else // ((pPrevNode == NULL) && (pNextNextNode == NULL))
	{
		pNextNode->m_pPrev = NULL;
		pNextNode->m_pNext = pNode;
		pNode->m_pPrev = pNextNode;
		pNode->m_pNext = NULL;
	}
	if (m_pHead == pNode)
	{
		m_pHead = pNextNode;
	}
	if (m_pTail == pNextNode)
	{
		m_pTail = pNode;
	}
}



//
// use when linked list contains non-streamable types
//
template < class T >
inline void CLinkList< T >::Read( FDataStream& kStream )
{
	int iLength;
	kStream >>  iLength ;
	clear();

	if ( iLength )
	{
		T* pData = FNEW(T, c_eCiv5GameplayDLL, 0);;
		for ( int i = 0; i < iLength; i++ )
		{
			kStream >>  *pData ;
			insertAtEnd( *pData );
		}
		SAFE_DELETE( pData );
	}
}

template < class T >
inline void CLinkList< T >::Write( FDataStream& kStream ) const
{
	int iLength = getLength();
	kStream <<  iLength ;
	const CLLNode< T >* pNode = head();
	while ( pNode )
	{
		kStream << pNode->m_data ;
		pNode = next( pNode );
	}
}

template<typename ValueType>
bool CLinkList<ValueType>::operator==(const CLinkList<ValueType> & rhs) const
{
	if(&rhs != this)
	{
		const CLLNode<ValueType> * myNode = head();
		const CLLNode<ValueType> * theirNode = rhs.head();
		while(myNode && theirNode)
		{
			if(myNode->m_data != theirNode->m_data)
				return false;
			myNode = next(myNode);
			theirNode = rhs.next(theirNode);
		}
	}
	return true;
}

template<typename ValueType>
bool CLinkList<ValueType>::operator!=(const CLinkList<ValueType> & rhs) const
{
	return !(*this == rhs);
}

template<typename ValueType>
FDataStream & operator<<(FDataStream & saveTo, const CLinkList<ValueType> & readFrom)
{
	readFrom.Write(saveTo);
	return saveTo;
}

template<typename ValueType>
FDataStream & operator>>(FDataStream & loadFrom, CLinkList<ValueType> & writeTo)
{
	writeTo.Read(loadFrom);
	return loadFrom;
}
#endif	//LINKEDLIST_H
