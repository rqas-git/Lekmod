







#include "CvGameCoreDLLPCH.h"
#include "CvCityManager.h"
#include "CvGameCoreUtils.h"

typedef std::map<CvCity*, CvCityManager::CityList> CityMap;

static CityMap ms_kCityMap;

static CvCityManager::CityList ms_kEmptyList;


void CvCityManager::Reset()
{
	ms_kCityMap.clear();
}


void CvCityManager::Shutdown()
{
	ms_kCityMap.clear();
}


static void AddToSortedList(CvCityManager::CityList &kCityList, int iFromX, int iFromY, CvCity* pkAddCity)
{
	if (kCityList.size() == 0)
	{

		kCityList.push_back(pkAddCity);
	}
	else
	{

		int iAddDistance = plotDistance(iFromX, iFromY, pkAddCity->getX(), pkAddCity->getY());
		bool bAdded = false;
		for (CvCityManager::CityList::iterator itrNearby = kCityList.begin(); itrNearby != kCityList.end(); ++itrNearby)
		{
			CvCity* pkNearbyCity = (*itrNearby);
			int iDistance = plotDistance(iFromX, iFromY, pkNearbyCity->getX(), pkNearbyCity->getY());
			if (iAddDistance <= iDistance)
			{
				kCityList.insert(itrNearby, pkAddCity);
				bAdded = true;
				break;
			}
		}

		if (!bAdded)
			kCityList.push_back(pkAddCity);
	}
}


void CvCityManager::OnCityCreated(CvCity* pkAddCity)
{

	for (CityMap::iterator itr = ms_kCityMap.begin(); itr != ms_kCityMap.end(); ++itr)
	{
		CvCity* pkCity = (*itr).first;
		if ((*itr).first != pkAddCity)
		{
			CityList &kCityList = (*itr).second;

			AddToSortedList(kCityList, pkCity->getX(), pkCity->getY(), pkAddCity);
		}
	}


	std::pair<CityMap::iterator, bool> kResult = ms_kCityMap.insert(CityMap::value_type(pkAddCity, ms_kEmptyList));

	CityList& kCityList = (*kResult.first).second;
	kCityList.reserve(256);



	int iFromX = pkAddCity->getX();
	int iFromY = pkAddCity->getY();
	for (CityMap::iterator itr = ms_kCityMap.begin(); itr != ms_kCityMap.end(); ++itr)
	{
		CvCity* pkCity = (*itr).first;
		if ((*itr).first != pkAddCity)
		{
			AddToSortedList(kCityList, iFromX, iFromY, pkCity);
		}
	}
}


void CvCityManager::OnCityDestroyed(CvCity* pkCity)
{

	for (CityMap::iterator itr = ms_kCityMap.begin(); itr != ms_kCityMap.end(); ++itr)
	{
		if ((*itr).first != pkCity)
		{
			CityList &kCityList = (*itr).second;
			for (CityList::iterator itrNearby = kCityList.begin(); itrNearby != kCityList.end(); ++itrNearby)
			{
				if ((*itrNearby) == pkCity)
				{
					kCityList.erase(itrNearby);
					break;
				}
			}
		}
	}


	CityMap::iterator itr = ms_kCityMap.find(pkCity);
	if (itr != ms_kCityMap.end())
	{
		ms_kCityMap.erase(itr);
	}
}


const CvCityManager::CityList& CvCityManager::GetNearbyCities(CvCity* pkCity)
{
	CvAssert(pkCity);
	if (pkCity)
	{
		CityMap::const_iterator itr = ms_kCityMap.find(pkCity);

		CvAssert(itr != ms_kCityMap.end());
		if (itr != ms_kCityMap.end())
		{
			return (*itr).second;
		}
	}


	return ms_kEmptyList;
}
