







#ifndef CVGOODYHUTS_H
#define CVGOODYHUTS_H

#pragma once

#define NUM_GOODIES_REMEMBERED (4)

class CvGoodyHuts
{
public:
	static void DoPlayerReceivedGoody(PlayerTypes ePlayer, GoodyTypes eGoody);
	static bool IsCanPlayerReceiveGoody(PlayerTypes ePlayer, GoodyTypes eGoody);
	static bool IsHasPlayerReceivedGoodyLately(PlayerTypes ePlayer, GoodyTypes eGoody);

#ifdef LEKMOD_NEW_ANCIENT_RUIN_REWARDS
	static void DoPlayerReceivedGoodyOncePerGame(PlayerTypes iPlayer, GoodyTypes iGoodyHut);
	static bool IsGoodyHutOncePerGame(PlayerTypes ePlayer, GoodyTypes eGoody);
#endif

	static void Reset();
	static void Uninit();

	static void Read(FDataStream& kStream, uint uiParentVersion);
	static void Write(FDataStream& kStream);

private:
	static int** m_aaiPlayerGoodyHutResults;

#ifdef LEKMOD_NEW_ANCIENT_RUIN_REWARDS
	static int** m_aiGoodyHutsOncePerGame;
#endif

};

#endif
