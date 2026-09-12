






#include "CvGameCoreDLLPCH.h"
#include "CvDiplomacyRequests.h"
#include "CvPlayer.h"
#include "FStlContainerSerialization.h"
#include "ICvDLLUserInterface.h"
#include "CvEnumSerialization.h"
#include "CvDiplomacyAI.h"
#include "CvDllInterfaces.h"


#include "LintFree.h"


FDataStream& operator>>(FDataStream& loadFrom, CvDiplomacyRequests::Request& writeTo)
{
	loadFrom >> writeTo.m_eDiploType;
	loadFrom >> writeTo.m_eAnimationType;
	loadFrom >> writeTo.m_strMessage;
	loadFrom >> writeTo.m_iExtraGameData;
	loadFrom >> writeTo.m_iTurn;
	loadFrom >> writeTo.m_iLookupIndex;
	loadFrom >> writeTo.m_eFromPlayer;

	return loadFrom;
}


FDataStream& operator<<(FDataStream& saveTo, const CvDiplomacyRequests::Request& readFrom)
{
	saveTo << readFrom.m_eDiploType;
	saveTo << readFrom.m_eAnimationType;
	saveTo << readFrom.m_strMessage;
	saveTo << readFrom.m_iExtraGameData;
	saveTo << readFrom.m_iTurn;
	saveTo << readFrom.m_iLookupIndex;
	saveTo << readFrom.m_eFromPlayer;

	return saveTo;
}

void CvDiplomacyRequests::Request::Clear()
{
	m_eDiploType = NO_DIPLO_UI_STATE;
	m_eAnimationType = NO_LEADERHEAD_ANIM;
	m_eFromPlayer = NO_PLAYER;
	m_strMessage = "";
	m_iTurn = -1;
	m_iLookupIndex = -1;
}


CvDiplomacyRequests::CvDiplomacyRequests(void)
{
	Uninit();
}


CvDiplomacyRequests::~CvDiplomacyRequests(void)
{
	Uninit();
}


void CvDiplomacyRequests::Init(PlayerTypes ePlayer)
{
	Uninit();
	m_ePlayer = ePlayer;

	m_aRequests.clear();
}


void CvDiplomacyRequests::Uninit(void)
{
	m_ePlayer = NO_PLAYER;
	m_eNextAIPlayer = NO_PLAYER;
	m_aRequests.clear();
	m_bRequestActive = false;
	m_bRequestActiveFromPlayer = NO_PLAYER;
}


void CvDiplomacyRequests::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_ePlayer;

	uint uiListSize;
	kStream >> uiListSize;

	if(uiListSize > 0)
	{
		for(uint ui = 0; ui < uiListSize; ui++)
		{
			m_aRequests.push_back(Request());
			kStream >> m_aRequests.back();
		}
	}
}


void CvDiplomacyRequests::Write(FDataStream& kStream) const
{

	uint uiVersion = 1;
	kStream << uiVersion;


	kStream << m_ePlayer;

	kStream << m_aRequests.size();

	for(RequestList::const_iterator i = m_aRequests.begin(); i != m_aRequests.end(); ++i)
	{
		kStream << (*i);
	}
}



void CvDiplomacyRequests::Update(void)
{
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();

	if(m_aRequests.size() && m_ePlayer == eActivePlayer && GET_PLAYER(eActivePlayer).isTurnActive())
	{
		CvDiplomacyRequests::Request& kRequest = m_aRequests.front();


		if(kRequest.m_eFromPlayer != NO_PLAYER && GET_PLAYER(kRequest.m_eFromPlayer).isAlive())
		{
			Send(kRequest.m_eFromPlayer, kRequest.m_eDiploType, kRequest.m_strMessage, kRequest.m_eAnimationType, kRequest.m_iExtraGameData);
		}
		m_aRequests.pop_front();
	}
}



void CvDiplomacyRequests::BeginTurn(void)
{
	m_eNextAIPlayer = (PlayerTypes)0;
}



void CvDiplomacyRequests::EndTurn(void)
{
	m_eNextAIPlayer = NO_PLAYER;
}



bool CvDiplomacyRequests::Add(PlayerTypes eFromPlayer, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData         )
{

	m_aRequests.push_back(Request());
	Request& newRequest = m_aRequests.back();

	newRequest.Clear();
	newRequest.m_eFromPlayer = eFromPlayer;
	newRequest.m_eDiploType = eDiploType;
	newRequest.m_strMessage = pszMessage;
	newRequest.m_iExtraGameData = iExtraGameData;
	newRequest.m_eAnimationType = eAnimationType;
	newRequest.m_iTurn = GC.getGame().getGameTurn();

	return true;
}


void CvDiplomacyRequests::Send(PlayerTypes eFromPlayer, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData         )
{
	gDLL->GameplayDiplomacyAILeaderMessage(eFromPlayer, eDiploType, pszMessage, eAnimationType, iExtraGameData);
	m_bRequestActiveFromPlayer = eFromPlayer;
	m_bRequestActive = true;
}


PlayerTypes CvDiplomacyRequests::GetNextAIPlayer() const
{
	return m_eNextAIPlayer;
}

void CvDiplomacyRequests::SetNextAIPlayer(PlayerTypes eNextPlayer)
{
	m_eNextAIPlayer = eNextPlayer;
}


bool CvDiplomacyRequests::HasPendingRequests() const
{
	return !m_aRequests.empty() || m_bRequestActive;
}


bool CvDiplomacyRequests::HasActiveRequest() const
{
	return m_bRequestActive;
}


bool CvDiplomacyRequests::HasActiveRequestFrom(PlayerTypes eFromPlayer) const
{
	return m_bRequestActive && m_bRequestActiveFromPlayer == eFromPlayer;
}






void CvDiplomacyRequests::SendRequest(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData         )
{
	CvPlayer& kPlayer = GET_PLAYER(eToPlayer);
	CvDiplomacyRequests* pkDiploRequests = kPlayer.GetDiplomacyRequests();
	if(pkDiploRequests)
	{
#ifdef NO_LEADER_SCREEN
		CvPreGame::pushGameType(GAME_NETWORK_MULTIPLAYER);
#endif
		if(!CvPreGame::isNetworkMultiplayerGame() && GC.getGame().getActivePlayer() == eToPlayer)
		{

			pkDiploRequests->Send(eFromPlayer, eDiploType, pszMessage, eAnimationType, iExtraGameData);
		}
		else
			pkDiploRequests->Add(eFromPlayer, eDiploType, pszMessage, eAnimationType, iExtraGameData);
	}
}




void CvDiplomacyRequests::SendDealRequest(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvDeal* pkDeal, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType)
{

	if(GC.getGame().getActivePlayer() == eToPlayer)
	{
		auto_ptr<ICvDeal1> pDeal = GC.WrapDealPointer(pkDeal);
		GC.GetEngineUserInterface()->SetScratchDeal(pDeal.get());
		SendRequest(eFromPlayer, eToPlayer, eDiploType, pszMessage, eAnimationType, -1);
	}
}





void CvDiplomacyRequests::DoAIDiplomacy(PlayerTypes eTargetPlayer)
{
	if(eTargetPlayer != NO_PLAYER)
	{
		ICvUserInterface2* pkIFace = GC.GetEngineUserInterface();

		CvAssert(!CvPreGame::isNetworkMultiplayerGame());


		if(pkIFace->IsModalStackEmpty() && !pkIFace->isDiploOrPopupWaiting() && !pkIFace->isCityScreenUp())
		{
			CvPlayer& kTargetPlayer = GET_PLAYER((PlayerTypes) eTargetPlayer);
			CvDiplomacyRequests* pkTargetRequests = kTargetPlayer.GetDiplomacyRequests();
			if(pkTargetRequests)
			{
				if(!pkTargetRequests->HasPendingRequests())
				{
					PlayerTypes ePlayerLoop = pkTargetRequests->GetNextAIPlayer();
					if(ePlayerLoop != NO_PLAYER)
					{
						int iMaxCivPlayers = MAX_CIV_PLAYERS;
						while((int)ePlayerLoop < iMaxCivPlayers)
						{
							CvPlayer& kPlayer = GET_PLAYER(ePlayerLoop);

							if(eTargetPlayer != ePlayerLoop && kPlayer.isAlive() && !kPlayer.isHuman() && !kPlayer.isMinorCiv() && !kPlayer.isBarbarian())
							{
								kPlayer.GetDiplomacyAI()->DoTurn(eTargetPlayer);
								if(pkTargetRequests->HasPendingRequests())
								{
									ePlayerLoop = (PlayerTypes)((int)ePlayerLoop + 1);
									break;
								}
							}

							ePlayerLoop = (PlayerTypes)((int)ePlayerLoop + 1);
						}

						pkTargetRequests->SetNextAIPlayer((((int)ePlayerLoop >= iMaxCivPlayers)?NO_PLAYER:ePlayerLoop));
					}
				}
			}
		}
	}

}


void CvDiplomacyRequests::ActiveRequestComplete()
{
	m_bRequestActive = false;
	m_bRequestActiveFromPlayer = NO_PLAYER;
}







bool CvDiplomacyRequests::HasActiveDiploRequestWithHuman(PlayerTypes eSourcePlayer)
{
	for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		CvPlayer& kTargetPlayer = GET_PLAYER((PlayerTypes)i);
		if (kTargetPlayer.isHuman() && kTargetPlayer.isAlive() && (PlayerTypes)i != eSourcePlayer)
		{
			if (kTargetPlayer.GetDiplomacyRequests()->HasActiveRequestFrom(eSourcePlayer))
				return true;
		}
	}
	return false;
}
