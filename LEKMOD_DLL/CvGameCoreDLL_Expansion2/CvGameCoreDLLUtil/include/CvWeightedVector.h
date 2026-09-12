#pragma once

#ifndef CIV5_WEIGHTED_VECTOR_H
#define CIV5_WEIGHTED_VECTOR_H

#include <algorithm>
#include "EventSystem/FastDelegate.h"
#include "FFastVector.h"


typedef fastdelegate::FastDelegate2<int, const char *, int> RandomNumberDelegate;










template< class T, unsigned int L = 1, bool bPODType = false> class CvWeightedVector
{
public:

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

			return m_iWeight > b2.m_iWeight;
		};

	};


	CvWeightedVector(void)
	{
	};


	~CvWeightedVector(void)
	{
	};


	const T& GetElement (unsigned int iIndex) const
	{
		assert(iIndex < m_pItems.size());
		return m_pItems[iIndex].m_Element;
	};


	int GetWeight (unsigned int	 iIndex) const
	{
		WeightedElement elem;
		assert(iIndex < m_pItems.size());
		elem = m_pItems[iIndex];
		return elem.m_iWeight;
	}
	void IncreaseWeight (unsigned int iIndex, int iWeight)
	{
		m_pItems[iIndex].m_iWeight += iWeight;
		CvAssertMsg(m_pItems[iIndex].m_iWeight >= 0, "Weight should not be negative.");
	}
	void SetWeight (unsigned int iIndex, int iWeight)
	{
		m_pItems[iIndex].m_iWeight = iWeight;
		CvAssertMsg(m_pItems[iIndex].m_iWeight >= 0, "Weight should not be negative.");
	}


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


	unsigned int push_back (const T& element, int iWeight)
	{


		WeightedElement weightedElem;
		weightedElem.m_Element = element;
		weightedElem.m_iWeight = iWeight;

		return m_pItems.push_back(weightedElem);
	};


	void clear ()
	{
		m_pItems.clear();
	};


	void resize (uint uiNewSize)
	{
		m_pItems.resize(uiNewSize);
	};


	void reserve (uint uiNewSize)
	{
		m_pItems.reserve(uiNewSize);
	};


	int size ()
	{
		return m_pItems.size();
	};


	void SortItems ()
	{
		std::sort(m_pItems.begin(), m_pItems.end());
	}

	void StableSortItems()
	{
		std::stable_sort(m_pItems.begin(), m_pItems.end());
	}


	T ChooseAbovePercentThreshold(int iPercent, RandomNumberDelegate *rndFcn, const char *szRollName)
	{
		int iCutoff;
		WeightedElement elem;


		iCutoff = GetTotalWeight() * iPercent / 100;


		CvWeightedVector<T, L, bPODType> tempVector;


		for (unsigned int i = 0; i < m_pItems.size(); i++)
		{
			elem = m_pItems[i];
			if (elem.m_iWeight >= iCutoff)
			{
				tempVector.push_back (elem.m_Element, elem.m_iWeight);
			}
		}


		if (tempVector.size() > 0)
		{
			return tempVector.ChooseByWeight (rndFcn, szRollName);
		}


		else
		{
			return ChooseByWeight (rndFcn, szRollName);
		}
	};


	T ChooseAtRandom(RandomNumberDelegate *rndFcn, const char *szRollName)
	{
		int iChoice;


		iChoice = (*rndFcn)(m_pItems.size(), szRollName);
		WeightedElement elem = m_pItems[iChoice];

		return elem.m_Element;
	};


	T ChooseByWeight(RandomNumberDelegate *rndFcn, const char *szRollName)
	{
		WeightedElement elem;
		int iChoice;


		iChoice = (*rndFcn)(GetTotalWeight(), szRollName);


		for (unsigned int i = 0; i < m_pItems.size(); i++)
		{
			elem = m_pItems[i];
			iChoice -= elem.m_iWeight;
			if (iChoice < 0)
			{
				return elem.m_Element;
			}
		}



		FAssertMsg (true, "Internal error in CvWeightedVector.");
		return elem.m_Element;
	};


	T ChooseFromTopChoices(int iNumChoices, RandomNumberDelegate *rndFcn, const char *szRollName)
	{

		if (iNumChoices > (int) m_pItems.size())
		{
			iNumChoices = (int) m_pItems.size();
		}

		WeightedElement elem;
		int i;
		int iChoice;
		int iTotalTopChoicesWeight = 0;


		for (i = 0; i < iNumChoices; i++)
		{
			elem = m_pItems[i];
			iTotalTopChoicesWeight += elem.m_iWeight;
		}


		iChoice = (*rndFcn)(iTotalTopChoicesWeight, szRollName);


		for (i = 0; i < iNumChoices; i++)
		{
			elem = m_pItems[i];
			iChoice -= elem.m_iWeight;
			if (iChoice < 0)
			{
				return elem.m_Element;
			}
		}



		CvAssertMsg (false, "Internal error in CvWeightedVector.");
		return elem.m_Element;
	};

private:
	FStaticVector<WeightedElement, L, bPODType> m_pItems;
};

#endif
