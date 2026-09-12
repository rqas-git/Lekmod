







#include "CvGameCoreDLLPCH.h"
#include "CvGlobals.h"
#include "CvPlayer.h"
#include "CvPlayerManager.h"
#include "CvDangerPlots.h"



void CvPlayerManager::RefreshDangerPlots()
{
	for(int iPlayerCivLoop = 0; iPlayerCivLoop < MAX_CIV_PLAYERS; iPlayerCivLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayerCivLoop;

		CvPlayer& kPlayer = GET_PLAYER(ePlayer);

		if(!kPlayer.isAlive())
			continue;

		if(kPlayer.m_pDangerPlots && kPlayer.m_pDangerPlots->IsDirty())
			kPlayer.UpdateDangerPlots();
	}
}
