






#pragma once

#ifndef CIV5_CITY_CONNECTIONS_H
#define CIV5_CITY_CONNECTIONS_H

class CvPlayer;







class CvCityConnections
{
public:
	CvCityConnections(void);
	~CvCityConnections(void);

	void Init(CvPlayer* pPlayer);
	void Uninit(void);


	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	void Update(void);
	void UpdateCityPlotIDs(void);
	void UpdateRouteInfo(void);

	void ResetRouteInfo(void);
	void ResetCityPlotIDs(void);

	bool IsEmpty(void);

	uint GetIndexFromCity(CvCity* pCity);
	CvCity* GetCityFromIndex(int iIndex);

	uint GetNumConnectableCities(void);

	bool ShouldConnectToOtherPlayer(PlayerTypes eMinor);

	CvPlayer* m_pPlayer;

	typedef enum RouteState
	{
	    HAS_ANY_ROUTE   = 0x1,
	    HAS_WATER_ROUTE = 0x2,
	    HAS_BEST_ROUTE  = 0x4,
#ifdef AUI_WARNING_FIXES
	} _RouteState;
#else
	};
#endif

	struct RouteInfo
	{
		RouteInfo() :
			m_cRouteState(0)
			, m_cPassEval(0)
		{
		}

		char m_cRouteState;
		char m_cPassEval;
	};

	uint m_uiRouteInfosDimension;
	RouteInfo* m_aRouteInfos;

	RouteInfo* GetRouteInfo(uint uiFirstCityIndex, uint uiSecondCityIndex);



#ifdef AUI_WARNING_FIXES
	FStaticVector<uint, SAFE_ESTIMATE_NUM_CITIES, true, c_eCiv5GameplayDLL, 0> m_aiCityPlotIDs;
#else
	FStaticVector<int, SAFE_ESTIMATE_NUM_CITIES, true, c_eCiv5GameplayDLL, 0> m_aiCityPlotIDs;
#endif
	FStaticVector<BuildingTypes, 10, true, c_eCiv5GameplayDLL, 0> m_aBuildingsAllowWaterRoutes;


protected:

	void UpdatePlotRouteStates(void);
	void BroadcastPlotRouteStateChanges(void);
	void ConnectPlotRoute(CvPlot* pPlot);

	void ResizeRouteInfo(uint uiNewSize);


	typedef enum PlotRouteState
	{
	    NO_CONNECTION = 0x0,
	    CONNECTION = 0x1,
	    CONNECTION_LAST_TURN = 0x2
#ifdef AUI_WARNING_FIXES
	} _PlotRouteState;
#else
	};
#endif

	struct PlotRouteInfo
	{
		PlotRouteInfo() :
			m_iPlotIndex(-1)
			, m_bPlotRouteState(0)
		{
		}

		int m_iPlotIndex;
		byte m_bPlotRouteState;
	};

	FStaticVector<PlotRouteInfo, 100, true, c_eCiv5GameplayDLL, 0> m_aPlotRouteInfos;
};

#endif
