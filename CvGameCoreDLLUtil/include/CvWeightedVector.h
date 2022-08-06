#pragma once

#ifndef CIV5_WEIGHTED_VECTOR_H
#define CIV5_WEIGHTED_VECTOR_H

#include <vector>
#include <algorithm>
#include "EventSystem/FastDelegate.h"

// Functor for random number callback routine
typedef fastdelegate::FastDelegate2<int, const char *, int> RandomNumberDelegate;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:     CvWeightedVector
//!  \brief	   Container holding objects with associated weighting
//
//!  Key Attributes:
//!  - Underlying data structure is FFastVector from Fireworks
//!  - See documentation on template parameters from FFastVector; same ones used here
//!  - Main purpose of class is use for weighted AI selection
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template<class T> class CvWeightedVector
{
public:
	/// One element stored in our weighted vector
	struct WeightedElement
	{
		WeightedElement() : 
		m_Element()
		, m_iWeight(0)
		{
		}

		WeightedElement(const T& e, int w) : 
		m_Element(e)
		, m_iWeight(w)
		{
		}

		WeightedElement(const WeightedElement & source) :
		m_Element(source.m_Element)
		, m_iWeight(source.m_iWeight)
		{
		}

		T m_Element;
		int m_iWeight;

		bool operator< (const WeightedElement& b2) const
		{
			// Reverse of the normal direction because we want highest weight first in our list
			return m_iWeight > b2.m_iWeight;
		};

	};

	/// Default constructor
	CvWeightedVector(void)
	{
	};

	/// Destructor
	~CvWeightedVector(void)
	{
	};

	/// Accessor for element
	const T& GetElement (unsigned int iIndex) const
	{
		assert(iIndex < m_items.size());
		return m_items[iIndex].m_Element;
	};

	/// Accessor for element
	void SetElement (unsigned int iIndex, const T& iValue)
	{
		assert(iIndex < m_items.size());
		if (iIndex<m_items.size())
			m_items[iIndex].m_Element = iValue;
	};

	/// Accessors for weight
	int GetWeight (unsigned int	 iIndex) const
	{
		assert(iIndex < m_items.size());
		return m_items[iIndex].m_iWeight;
	}
	void IncreaseWeight (unsigned int iIndex, int iWeight)
	{
		if (iIndex<m_items.size())
			m_items[iIndex].m_iWeight += iWeight;
		CvAssertMsg(m_items[iIndex].m_iWeight >= 0, "Weight should not be negative.");
	}
	void SetWeight (unsigned int iIndex, int iWeight)
	{
		if (iIndex<m_items.size())
			m_items[iIndex].m_iWeight = iWeight;
		CvAssertMsg(m_items[iIndex].m_iWeight >= 0, "Weight should not be negative.");
	}

	/// Return total of all weights stored in vector
	int GetTotalWeight () const
	{
		int rtnValue = 0;

		for (unsigned int i = 0; i < m_items.size(); i++)
		{
			WeightedElement elem = m_items[i];
			rtnValue += elem.m_iWeight;
		}

		return rtnValue;
	};

	/// Add an item to the end of the vector
	unsigned int push_back (const T& element, int iWeight)
	{
		m_items.push_back( WeightedElement(element,iWeight) );
		return m_items.size();
	};

	/// Clear out the vector
	void clear ()
	{
		m_items.clear();
	};

	/// Resize the vector
	void resize (uint uiNewSize)
	{
		m_items.resize(uiNewSize);
	};

	/// Set the vector reserve
	void reserve (uint uiNewSize)
	{
		m_items.reserve(uiNewSize);
	};

	/// Number of items
	int size () const
	{
		return m_items.size();
	};

	/// Sort this stuff from highest to lowest
	void SortItems ()
	{
		std::sort(m_items.begin(), m_items.end());
	}

	void StableSortItems()
	{
		std::stable_sort(m_items.begin(), m_items.end());
	}

	/// Return a random entry by weight, but avoid unlikely candidates (by only looking at candidates with a certain percentage chance)
	T ChooseAbovePercentThreshold(int iPercent, RandomNumberDelegate *rndFcn, const char *szRollName)
	{
		// Compute cutoff for the requested percentage
		int iCutoff = GetTotalWeight() * iPercent / 100;

		// Create a new weighted vector for this decision
		std::vector<T> tempVector;

		// Loop through until adding each item that is above threshold
		for (unsigned int i = 0; i < this->m_pItems.size(); i++)
		{
			WeightedElement elem = m_items[i];
			if (elem.m_iWeight >= iCutoff)
			{
				tempVector.push_back (elem.m_Element, elem.m_iWeight);
			}
		}

		// If have something in the temp vector, do a regular weighted choice from the temporary vector
		if (tempVector.size() > 0)
		{
			return tempVector.ChooseByWeight (rndFcn, szRollName);
		}

		// Otherwise ignore the cutoff percentage and go back to the original items
		else
		{
			return ChooseByWeight (rndFcn, szRollName);
		}
	};

	/// Return a random entry (ignoring weight)
	T ChooseAtRandom(RandomNumberDelegate *rndFcn, const char *szRollName)
	{
		// the easy case
		if (CvPreGame::isReallyNetworkMultiPlayer())
			return m_items[0].m_Element;

		// Based on the number of elements we have, pick one at random
		int iChoice = (*rndFcn)(m_items.size(), szRollName);
		return m_items[iChoice].m_Element;
	};

	/// Choose by weight (even considering unlikely candidates)
	T ChooseByWeight(RandomNumberDelegate *rndFcn, const char *szRollName)
	{
		// the easy case
		if (CvPreGame::isReallyNetworkMultiPlayer())
			return m_items[0].m_Element;

		// Random roll up to total weight
		int iChoice = (*rndFcn)(GetTotalWeight(), szRollName);

		// Loop through until we find the item that is in the range for this roll
		for (unsigned int i = 0; i < m_items.size(); i++)
		{
			WeightedElement elem = m_items[i];
			iChoice -= elem.m_iWeight;
			if (iChoice < 0)
				return elem.m_Element;
		}

		// We should have found something, so reaching here is an error
		UNREACHABLE();
	};

	/// Pick an element from the top iNumChoices
	T ChooseFromTopChoices(int iNumChoices, RandomNumberDelegate *rndFcn, const char *szRollName)
	{
		// Loop through the top choices, or the total vector size, whichever is smaller
		if (iNumChoices > (int) m_items.size())
			iNumChoices = (int) m_items.size();

		assert(iNumChoices > 0);

		// Get the total weight (as long as the weights are in a similar range)
		int iTotalTopChoicesWeight = m_items[0].m_iWeight;
		for (int i = 1; i < iNumChoices; i++)
		{
			if (m_items[i].m_iWeight*2 < m_items[i-1].m_iWeight)
			{
				//ignore the rest
				iNumChoices = i;
				break;
			}

			iTotalTopChoicesWeight += m_items[i].m_iWeight;
		}

		// the easy case
		if (iNumChoices == 1 || CvPreGame::isReallyNetworkMultiPlayer())
			return m_items[0].m_Element;

		// All of the choices have a weight of 0 so pick randomly
		if (iTotalTopChoicesWeight == 0)
			return ChooseAtRandom(rndFcn, szRollName);

		int iChoice = (*rndFcn)(iTotalTopChoicesWeight, szRollName);

		// Find out which element was chosen
		for (int i = 0; i < iNumChoices; i++)
		{
			WeightedElement elem = m_items[i];
			iChoice -= elem.m_iWeight;

			if (iChoice < 0)
				return elem.m_Element;
		}

		// We should have found something, so reaching here is an error
		UNREACHABLE();
	};

private:
	std::vector<WeightedElement> m_items;
};

//-------------------------------
// Serialization helper templates:
//-------------------------------

template<typename T>
FDataStream & operator<<(FDataStream & writeTo, const CvWeightedVector<T> & readFrom)
{
	writeTo << readFrom.size();
	for (int i = 0; i < readFrom.size(); i++)
	{
		writeTo << readFrom.GetElement(i);
		writeTo << readFrom.GetWeight(i);
	}
	return writeTo;
}

template<typename T>
FDataStream & operator>>(FDataStream & readFrom, CvWeightedVector<T> & writeTo)
{
	int nItems;
	readFrom >> nItems;

	writeTo.clear();
	for (int i=0; i<nItems; i++)
	{
		T e;
		int w;
		readFrom >> e;
		readFrom >> w;
		writeTo.push_back(e,w);
	}
	return readFrom;
}

#endif //CIV5_WEIGHTED_VECTOR_H
