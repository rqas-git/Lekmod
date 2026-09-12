







#include "CvGameCoreDLLPCH.h"
#include "CvPlot.h"
#include "CvGlobals.h"
#include "CvMap.h"
#include "CvGameCoreUtils.h"
#include "CvTargeting.h"


#include "LintFree.h"

#if !defined(FINAL_RELEASE)
#define TARGETING_DEBUG
#endif

#if defined(TARGETING_DEBUG)
static std::vector<int> ms_lastTargetingPath;

 #define TRACK_LAST_TARGET_PATH_ENTRY(x)	if (x) ms_lastTargetingPath.push_back(x->GetPlotIndex())
 #define CLEAR_TARGET_PATH_DEBUG	ms_lastTargetingPath.clear()
#else
 #define TRACK_LAST_TARGET_PATH_ENTRY __noop
 #define CLEAR_TARGET_PATH_DEBUG	__noop
#endif


int GetLastTargetingPathIndex(int plotIndex)
{
#if defined(TARGETING_DEBUG)
	int iIndex = 0;
	for (std::vector<int>::const_iterator itr = ms_lastTargetingPath.begin(); itr != ms_lastTargetingPath.end(); ++itr)
	{
		if ((*itr) == plotIndex)
			return iIndex;
		++iIndex;
	}
#else
	UNREFERENCED_PARAMETER(plotIndex);
#endif

	return -1;
}



			







static int ms_LookBackX[6] = 
{  
	 0,
	-1,
	-1,
	 0,
	 1,
	 1
};
static int ms_LookBackY[6] = 
{ 
	-1,
	 0,
	 1,
	 1,
	 0,
	-1
};



static int ms_HexDirection[3][3] =
{
	{
		0      ,	4,			5
	},
	{
		3,			0       ,	0
	},
	{
		2,			1,			0
	}
};

#define STRICT_LOOSE_CUTOFF	2






static bool CanSeeDisplacementPlot_Strict(int startX, int startY, int dx, int dy, int fromLevel)
{
	int originalDX = dx;
	int originalDY = dy;

	CvMap& kMap = GC.getMap();














#ifdef AUI_PLOT_VISIBILITY_OPTIMIZATIONS
	int stepY = 1;
#else
	int stepY;
#endif
	if (dy < 0) 
	{ 
		dy = -dy;  
		stepY = -1; 
	} 
#ifdef AUI_PLOT_VISIBILITY_OPTIMIZATIONS
	int stepX = 1;
#else
	else 
		stepY = 1; 

	int stepX;
#endif
	if (dx < 0) 
	{ 
		dx = -dx;  
		stepX = -1; 
	} 
#ifndef AUI_PLOT_VISIBILITY_OPTIMIZATIONS
	else 
		stepX = 1;
#endif

	dy <<= 1;
	dx <<= 1;

	int currentDX = 0;
	int currentDY = 0;


	bool oddQuadrant = stepX != stepY;


#ifdef AUI_PLOT_VISIBILITY_OPTIMIZATIONS
	bool straightThrough = dx == 0 || dy == 0 || (dx == dy && oddQuadrant);
#else
	bool straightThrough = dx == 0 || dy == 0;
#endif


	int lastDX = currentDX;
	int lastDY = currentDY;
	int lookbackDirection = -1;
	int stepCount = 0;
	int iBlockedCount = 0;


	if (dx > dy) 
	{
		int shortAxisStepFactor = (dx >> 1); 
		while (currentDX != originalDX) 
		{
			shortAxisStepFactor += dy;

			if (shortAxisStepFactor >= dx) 
			{
				shortAxisStepFactor -= dx;

				if (oddQuadrant)
				{
					currentDX += stepX;
					currentDY += stepY;
				}
				else
				{
					currentDX += stepX;


					if (!straightThrough && lookbackDirection != -1)
					{
						CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[lookbackDirection], startY + currentDY + ms_LookBackY[lookbackDirection]);
						TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
						if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
						{
							++iBlockedCount;
							if (stepCount > STRICT_LOOSE_CUTOFF || iBlockedCount >= 2)
								return false;
						}
					}


					if (currentDX == originalDX && currentDY == originalDY)
						break;	

					CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
					TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
					if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
						return false;

					lookbackDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];

					++stepCount;
					lastDX = currentDX; lastDY = currentDY;

					currentDY += stepY;
				}
			}
			else
				currentDX += stepX;


			if (!straightThrough && lookbackDirection != -1)
			{
				CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[lookbackDirection], startY + currentDY + ms_LookBackY[lookbackDirection]);
				TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
				if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
				{
					if (stepCount > STRICT_LOOSE_CUTOFF || ++iBlockedCount >= 2)
						return false;
				}
			}


			if (currentDX == originalDX && currentDY == originalDY)
				break;	

			CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
			TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
			if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
				return false;

			lookbackDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];

			++stepCount;
			lastDX = currentDX; lastDY = currentDY;
		}
	} 
	else 
	{



		int shortAxisStepFactor = (dy!=dx && dx != 0 && (dy>>1) == dx)?((dy >> 1) - dx):(dy >> 1);

		while (currentDY != originalDY) 
		{
			shortAxisStepFactor += dx;

			if (shortAxisStepFactor >= dy) 
			{
				shortAxisStepFactor -= dy;

				if (oddQuadrant)
				{

					currentDX += stepX;
					currentDY += stepY;
				}
				else
				{

					currentDX += stepX;


					if (!straightThrough && lookbackDirection != -1)
					{
						CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[lookbackDirection], startY + currentDY + ms_LookBackY[lookbackDirection]);
						TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
						if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
						{
							if (stepCount > STRICT_LOOSE_CUTOFF || ++iBlockedCount >= 2)
								return false;
						}
					}


					if (currentDX == originalDX && currentDY == originalDY)
						break;	

					CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
					TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
					if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
						return false;

					lookbackDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];

					++stepCount;
					lastDX = currentDX; lastDY = currentDY;

					currentDY += stepY;
				}
			}
			else
				currentDY += stepY;


			if (!straightThrough && lookbackDirection != -1)
			{
				CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[lookbackDirection], startY + currentDY + ms_LookBackY[lookbackDirection]);
				TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
				if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
				{
					if (stepCount > STRICT_LOOSE_CUTOFF || ++iBlockedCount >= 2)
						return false;
				}
			}


			if (currentDX == originalDX && currentDY == originalDY)
				break;	

			CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
			TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
			if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
				return false;

			lookbackDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];

			++stepCount;
			lastDX = currentDX; lastDY = currentDY;
		}
	}

	return true;
}





static bool CanSeeDisplacementPlot_Loose(int startX, int startY, int dx, int dy, int fromLevel)
{
	int originalDX = dx;
	int originalDY = dy;

	CvMap& kMap = GC.getMap();














#ifdef AUI_PLOT_VISIBILITY_OPTIMIZATIONS
	int stepY = 1;
#else
	int stepY;
#endif
	if (dy < 0) 
	{ 
		dy = -dy;  
		stepY = -1; 
	} 
#ifdef AUI_PLOT_VISIBILITY_OPTIMIZATIONS
	int stepX = 1;
#else
	else 
		stepY = 1; 

	int stepX;
#endif
	if (dx < 0) 
	{ 
		dx = -dx;  
		stepX = -1; 
	} 
#ifndef AUI_PLOT_VISIBILITY_OPTIMIZATIONS
	else 
		stepX = 1;
#endif

	dy <<= 1;
	dx <<= 1;

	int currentDX = 0;
	int currentDY = 0;

	bool bBlocked = false;

	bool oddQuadrant = stepX != stepY;


#ifdef AUI_PLOT_VISIBILITY_OPTIMIZATIONS
	bool straightThrough = dx == 0 || dy == 0 || (dx == dy && oddQuadrant);
#else
	bool straightThrough = dx == 0 || dy == 0;
#endif


	int lastDX = currentDX;
	int lastDY = currentDY;
	int blockedDirection = 0;


	if (dx > dy) 
	{
		int shortAxisStepFactor = (dx >> 1); 
		while (currentDX != originalDX) 
		{
			shortAxisStepFactor += dy;

			if (shortAxisStepFactor >= dx) 
			{
				shortAxisStepFactor -= dx;

				if (oddQuadrant)
				{
					currentDX += stepX;
					currentDY += stepY;
				}
				else
				{
					currentDX += stepX;


					if (bBlocked)
					{
						if (!straightThrough)
						{
							CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[blockedDirection], startY + currentDY + ms_LookBackY[blockedDirection]);
							if(passThroughPlot)
							{
								TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
								bBlocked = (fromLevel < passThroughPlot->seeThroughLevel());
							}

							if (bBlocked)
								return false;
						}
						else
							return false;
					}


					if (currentDX == originalDX && currentDY == originalDY)
						break;	

					CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
					TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
					if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
					{
						bBlocked = true;
						blockedDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];
					}

					lastDX = currentDX; lastDY = currentDY;

					currentDY += stepY;
				}
			}
			else
				currentDX += stepX;


			if (bBlocked)
			{
				if (!straightThrough)
				{
					CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[blockedDirection], startY + currentDY + ms_LookBackY[blockedDirection]);
					if(passThroughPlot)
					{
						TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
						bBlocked = (fromLevel < passThroughPlot->seeThroughLevel());
					}

					if (bBlocked)
						return false;
				}
				else
					return false;
			}


			if (currentDX == originalDX && currentDY == originalDY)
				break;	

			CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
			TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
			if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
			{
				bBlocked = true;
				blockedDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];
			}

			lastDX = currentDX; lastDY = currentDY;
		}
	} 
	else 
	{



		int shortAxisStepFactor = (dy!=dx && dx != 0 && (dy>>1) == dx)?((dy >> 1) - dx):(dy >> 1);

		while (currentDY != originalDY) 
		{
			shortAxisStepFactor += dx;

			if (shortAxisStepFactor >= dy) 
			{
				shortAxisStepFactor -= dy;

				if (oddQuadrant)
				{

					currentDX += stepX;
					currentDY += stepY;
				}
				else
				{

					currentDX += stepX;


					if (bBlocked)
					{
						if (!straightThrough)
						{
							CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[blockedDirection], startY + currentDY + ms_LookBackY[blockedDirection]);
							if(passThroughPlot)
							{
								TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
								bBlocked = (fromLevel < passThroughPlot->seeThroughLevel());
							}

							if (bBlocked)
								return false;
						}
						else
							return false;
					}


					if (currentDX == originalDX && currentDY == originalDY)
						break;	

					CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
					TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
					if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
					{
						bBlocked = true;
						blockedDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];
					}

					lastDX = currentDX; lastDY = currentDY;

					currentDY += stepY;
				}
			}
			else
				currentDY += stepY;


			if (bBlocked)
			{
				if (!straightThrough)
				{
					CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX + ms_LookBackX[blockedDirection], startY + currentDY + ms_LookBackY[blockedDirection]);
					if(passThroughPlot)
					{
						TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
						bBlocked = (fromLevel < passThroughPlot->seeThroughLevel());
					}

					if (bBlocked)
						return false;
				}
				else
					return false;
			}


			if (currentDX == originalDX && currentDY == originalDY)
				break;	

			CvPlot* passThroughPlot = PlotFromHex(kMap, startX + currentDX, startY + currentDY);
			TRACK_LAST_TARGET_PATH_ENTRY(passThroughPlot);
			if(!passThroughPlot || fromLevel < passThroughPlot->seeThroughLevel())
			{
				bBlocked = true;
				blockedDirection = ms_HexDirection[(currentDX - lastDX) + 1][(currentDY - lastDY) + 1];
			}

			lastDX = currentDX; lastDY = currentDY;
		}
	}

	return true;
}














bool CvTargeting::CanSeeDisplacementPlot(int startX, int startY, int dx, int dy, int fromLevel)
{
	CLEAR_TARGET_PATH_DEBUG;



	startX = xToHexspaceX(startX, startY);

	CvMap& kMap = GC.getMap();
	CvPlot* pPlot = PlotFromHex(kMap, startX + dx, startY + dy);
	if(pPlot != NULL)
	{
		int iHexDistanceBetweenPlots = hexDistance(dx, dy);

		if(iHexDistanceBetweenPlots <= 1)
			return true;


		if (iHexDistanceBetweenPlots > STRICT_LOOSE_CUTOFF                                                       )
			return CanSeeDisplacementPlot_Strict(startX, startY, dx, dy, fromLevel);
		else
			return CanSeeDisplacementPlot_Loose(startX, startY, dx, dy, fromLevel);

	}

	return false;
}
