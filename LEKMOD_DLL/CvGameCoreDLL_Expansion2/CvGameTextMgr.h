









#pragma once
#ifndef CIV5_GAME_TEXT_MGR_H
#define CIV5_GAME_TEXT_MGR_H

#pragma warning( disable: 4251 )





class CvGameTextMgr
{
public:

	static void setYearStr(CvString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed);
	static void setDateStr(CvString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed);
};

#endif
