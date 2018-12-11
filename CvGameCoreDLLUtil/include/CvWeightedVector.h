#pragma once

#ifndef CIV5_WEIGHTED_VECTOR_H
#define CIV5_WEIGHTED_VECTOR_H

#include <algorithm>
#include "EventSystem/FastDelegate.h"
#include "FFastVector.h"

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
template< class T, unsigned int L = 1, bool bPODType = false> class CvWeightedVector
{
public:
	/// One element stored in our weighted vector
	struct WeightedElement
	{
		WeightedElement() : 
		m_Element()
		, m_iWeight ()
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
		assert(iIndex < m_pItems.size());
		return m_pItems[iIndex].m_Element;
	};

	/// Accessor for element
	void SetElement (unsigned int iIndex, const T& iValue)
	{
		assert(iIndex < m_pItems.size());
		if (iIndex<m_pItems.size())
			m_pItems[iIndex].m_Element = iValue;
	};

	/// Accessors for weight
	int GetWeight (unsigned int	 iIndex) const
	{
		assert(iIndex < m_pItems.size());
		return m_pItems[iIndex].m_iWeight;
	}
	void IncreaseWeight (unsigned int iIndex, int iWeight)
	{
		if (iIndex<m_pItems.size())
			m_pItems[iIndex].m_iWeight += iWeight;
		CvAssertMsg(m_pItems[iIndex].m_iWeight >= 0, "Weight should not be negative.");
	}
	void SetWeight (unsigned int iIndex, int iWeight)
	{
		if (iIndex<m_pItems.size())
			m_pItems[iIndex].m_iWeight = iWeight;
		CvAssertMsg(m_pItems[iIndex].m_iWeight >= 0, "Weight should not be negative.");
	}

	/// Return total of all weights stored in vector
	int GetTotalWeight ()
	{
		int rtnValue = 0;

		for (unsigned int i = 0; i < m_pItems.size(); i++)
		{
			WeightedElement elem = m_pItems[i];
			rtnValue += elem.m_iWeight;
		}

		return rtnValue;
	};

	/// Add an item to the end of the vector
	unsigned int push_back (const T& element, int iWeight)
	{
//		FAssertMsg(iWeight >= 0, "Weight should not be negative.");

		WeightedElement weightedElem;
		weightedElem.m_Element = element;
		weightedElem.m_iWeight = iWeight;

		return m_pItems.push_back(weightedElem);
	};

	/// Clear out the vector
	void clear ()
	{
		m_pItems.clear();
	};

	/// Resize the vector
	void resize (uint uiNewSize)
	{
		m_pItems.resize(uiNewSize);
	};

	/// Set the vector reserve
	void reserve (uint uiNewSize)
	{
		m_pItems.reserve(uiNewSize);
	};

	/// Number of items
	int size ()
	{
		return m_pItems.size();
	};

	/// Sort this stuff from highest to lowest
	void SortItems ()
	{
		std::sort(m_pItems.begin(), m_pItems.end());
	}

	void StableSortItems()
	{
		std::stable_sort(m_pItems.begin(), m_pItems.end());
	}

	/// Return a random entry by weight, but avoid unlikely candidates (by only looking at candidates with a certain percentage chance)
	T ChooseAbovePercentThreshold(int iPercent, RandomNumberDelegate *rndFcn, const char *szRollName)
	{
		// Compute cutoff for the requested percentage
		int iCutoff = GetTotalWeight() * iPercent / 100;

		// Create a new weighted vector for this decision
		CvWeightedVector<T, L, bPODType> tempVector;

		// Loop through until adding each item that is above threshold
		for (unsigned int i = 0; i < m_pItems.size(); i++)
		{
			WeightedElement elem = m_pItems[i];
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
		// Based on the number of elements we have, pick one at random
		int iChoice = (*rndFcn)(m_pItems.size(), szRollName);
		return m_pItems[iChoice].m_Element;
	};

	/// Choose by weight (even considering unlikely candidates)
	T ChooseByWeight(RandomNumberDelegate *rndFcn, const char *szRollName)
	{
		// Random roll up to total weight
		int iChoice = (*rndFcn)(GetTotalWeight(), szRollName);

		// Loop through until we find the item that is in the range for this roll
		for (unsigned int i = 0; i < m_pItems.size(); i++)
		{
			WeightedElement elem = m_pItems[i];
			iChoice -= elem.m_iWeight;
			if (iChoice < 0)
				return elem.m_Element;
		}

		// We should have found something, so reaching here is an error
		assert(false);
		return T();
	};

	/// Pick an element from the top iNumChoices
	T ChooseFromTopChoices(int iNumChoices, RandomNumberDelegate *rndFcn, const char *szRollName)
	{
		// Loop through the top choices, or the total vector size, whichever is smaller
		if (iNumChoices > (int) m_pItems.size())
			iNumChoices = (int) m_pItems.size();

		assert(iNumChoices > 0);

		// Get the total weight (as long as the weights are in a similar range)
		int iTotalTopChoicesWeight = m_pItems[0].m_iWeight;
		for (int i = 1; i < iNumChoices; i++)
		{
			if (m_pItems[i].m_iWeight*2 < m_pItems[i-1].m_iWeight)
			{
				//ignore the rest
				iNumChoices = i;
				break;
			}

			iTotalTopChoicesWeight += m_pItems[i].m_iWeight;
		}

		// the easy case
		if (iNumChoices == 1)
			return m_pItems[0].m_Element;

		int iChoice = (*rndFcn)(iTotalTopChoicesWeight, szRollName);

		// Find out which element was chosen
		for (int i = 0; i < iNumChoices; i++)
		{
			WeightedElement elem = m_pItems[i];
			iChoice -= elem.m_iWeight;

			if (iChoice < 0)
				return elem.m_Element;
		}

		// We should have found something, so reaching here is an error
		assert(false);
		return T();
	};

private:
	FStaticVector<WeightedElement, L, bPODType> m_pItems;
};

#endif //CIV5_WEIGHTED_VECTOR_H
