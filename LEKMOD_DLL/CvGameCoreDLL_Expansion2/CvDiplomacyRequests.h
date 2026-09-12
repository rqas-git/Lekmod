






#pragma once

#ifndef CIV5_DIPLOREQUESTS_H
#define CIV5_DIPLOREQUESTS_H

class CvPlayer;









class CvDiplomacyRequests
{
public:
	CvDiplomacyRequests(void);
	~CvDiplomacyRequests(void);

	void Init(PlayerTypes ePlayer);
	void Uninit(void);


	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	PlayerTypes GetNextAIPlayer() const;
	void SetNextAIPlayer(PlayerTypes eNextPlayer);
	bool HasPendingRequests() const;
	bool HasActiveRequest() const;
	bool HasActiveRequestFrom(PlayerTypes eFromPlayer) const;

	void Update(void);
	void BeginTurn(void);
	void EndTurn(void);

	bool  Add(PlayerTypes ePlayerID, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData = -1);
	void  ActiveRequestComplete();

	struct Request
	{
		void Clear();

		DiploUIStateTypes			m_eDiploType;
		PlayerTypes					m_eFromPlayer;
		CvString					m_strMessage;
		LeaderheadAnimationTypes	m_eAnimationType;
		int							m_iTurn;
		int							m_iLookupIndex;
		int							m_iExtraGameData;
	};

	void Activate(Request& kRequest);

	static void DoAIDiplomacy(PlayerTypes eTargetPlayer);

	static void SendRequest(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData = -1);
	static void SendDealRequest(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvDeal* pkDeal, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType);

	static bool HasActiveDiploRequestWithHuman(PlayerTypes eSourcePlayer);


protected:

	void Send(PlayerTypes eFromPlayer, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData = -1);

	PlayerTypes m_ePlayer;
	PlayerTypes	m_eNextAIPlayer;

	typedef std::list<Request> RequestList;
	RequestList m_aRequests;
	PlayerTypes	m_bRequestActiveFromPlayer;
	bool		m_bRequestActive;
};

FDataStream& operator>>(FDataStream&, CvDiplomacyRequests::Request&);
FDataStream& operator<<(FDataStream&, const CvDiplomacyRequests::Request&);

#endif
