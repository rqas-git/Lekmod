#ifndef CVCITYMANAGER_H
#define CVCITYMANAGER_H

#include "CvCity.h"

#include <vector>
class CvCityManager
{
public:
	static void Reset();
	static void Shutdown();

	static void OnCityCreated(CvCity* pkCity);
	static void OnCityDestroyed(CvCity* pkCity);

	typedef std::vector<CvCity*> CityList;

	static const CityList& GetNearbyCities(CvCity* pkCity);
};

#endif
