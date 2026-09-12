






#pragma once

#ifndef CIV5_NOTIFICATIONS_H
#define CIV5_NOTIFICATIONS_H

class CvPlayer;




#define NOTIFICATION_PLAYER_KICKED (NotificationTypes)0xABD3C7BA 
#define NOTIFICATION_LEAGUE_PROJECT_COMPLETE ((NotificationTypes)0x0611623D)
#define NOTIFICATION_LEAGUE_PROJECT_PROGRESS ((NotificationTypes)0x66774058)







class CvNotifications
{
public:
	CvNotifications(void);
	~CvNotifications(void);

	void Init(PlayerTypes ePlayer);
	void Uninit(void);


	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	void Update(void);
	void EndOfTurnCleanup(void);

	int  AddByName(const char* szNotificationName, const char* strMessage, const char* strSummary, int iX, int iY, int iGameDataIndex, int iExtraGameData=-1);
	int  Add(NotificationTypes eNotificationType, const char* strMessage, const char* strSummary, int iX, int iY, int iGameDataIndex, int iExtraGameData=-1);
	void Activate(int iLookupIndex);
	void Dismiss(int iLookupIndex, bool bUserInvoked);
	bool MayUserDismiss(int iLookupIndex);
	void Rebroadcast(void);

	bool GetEndTurnBlockedType(EndTurnBlockingTypes& eBlockingType, int& iNotificationIndex);


	int GetNumNotifications(void);
	CvString GetNotificationStr(int iZeroBasedIndex);
	CvString GetNotificationSummary(int iZeroBasedIndex);
	int GetNotificationID(int iZeroBasedIndex);
	int GetNotificationTurn(int iZeroBasedIndex);
	bool IsNotificationDismissed(int iZeroBasedIndex);

	struct Notification
	{
		void Clear();

		NotificationTypes m_eNotificationType;
		PlayerTypes m_ePlayerID;
		CvString m_strMessage;
		CvString m_strSummary;
		int m_iX;
		int m_iY;
		int m_iGameDataIndex;
		int m_iExtraGameData;
		int m_iTurn;
		int m_iLookupIndex;
		bool m_bNeedsBroadcast;
		bool m_bDismissed;
		bool m_bWaitExtraTurn;
	};

	void Activate(Notification& notification);

	bool IsNotificationRedundant(Notification& notification);

	bool IsNotificationExpired(int iIndex);
	bool IsNotificationEndOfTurnExpired(int iIndex);
	bool IsNotificationTypeEndOfTurnExpired(NotificationTypes eNotificationType, int iForSpecificEntry = -1);

	static void AddToPlayer(PlayerTypes ePlayer, NotificationTypes eNotificationType, const char* strMessage, const char* strSummary, int iX=-1, int iY=-1, int iGameDataIndex=-1, int iExtraGameData=-1);

protected:

	bool IsArrayFull();
	void RemoveOldestNotification();
	void IncrementBeginIndex();
	void IncrementEndIndex();

	PlayerTypes m_ePlayer;

	std::vector<Notification> m_aNotifications;
	int m_iNotificationsBeginIndex;
	int m_iNotificationsEndIndex;

	int m_iCurrentLookupIndex;
};

FDataStream& operator>>(FDataStream&, CvNotifications::Notification&);
FDataStream& operator<<(FDataStream&, const CvNotifications::Notification&);

#endif
