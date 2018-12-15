// GuildTournamentMgr.cpp: implementation of the CGuildTournamentMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuildTournamentMgr.h"
#include "GameIn.h"
#include "MapChange.h"
#include "MainGame.h"
#include "ObjectStateManager.h"
#include "ObjectManager.h"
#include "../Interface/cWindowManager.h"
#include "ChatManager.h"
#include "WindowIDEnum.h"
#include "UserInfoManager.h"
#include "cMsgBox.h"
#include "GTBattleListDialog.h"
#include "GTStandingDialog.h"
#include "GTStandingDialog16.h"
#include "GTScoreInfoDialog.h"
#include "GTResultDlg.h"
#include "MiniMapDlg.h"
#include "./Input/UserInput.h"
#include "MHMap.h"
#include "GTournament/Battle_GTournament.h"
#include "BattleSystem_Client.h"
#include "GTEntryEditDlg.h"

#include "ItemManager.h"

// 080411 LYW --- GuildTournamentMgr : 채팅방 매니저 헤더 호출.
#include "./ChatRoomMgr.h"
#include <math.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGuildTournamentMgr::CGuildTournamentMgr()
{
}

CGuildTournamentMgr::~CGuildTournamentMgr()
{

}

void CGuildTournamentMgr::Process()
{
	if(GAMEIN->GetGTEntryEditDlg())
		GAMEIN->GetGTEntryEditDlg()->ReduceTickTime(gTickTime);
	if(GAMEIN->GetGTStandingDlg())
		GAMEIN->GetGTStandingDlg()->ReduceTickTime(gTickTime);

	CBattle* pBattle = BATTLESYSTEM->GetBattle();
	if(!pBattle)		return;

	if(pBattle->GetBattleKind()==eBATTLE_KIND_GTOURNAMENT && pBattle->GetBattleState()==eBATTLE_STATE_READY)
	{
		((CBattle_GTournament*)pBattle)->ReduceEntranceTime(gTickTime);
	}
}

BOOL CGuildTournamentMgr::IsGTMap()
{
	if(GTMAPNUM == MAP->GetMapNum())
		return TRUE;

	return FALSE;
}

void CGuildTournamentMgr::GetDateFromMSec(DWORD dwTime, WORD& day, WORD& hour, WORD&min, WORD&sec)
{
	day=hour=min=sec = 0;

	DWORD dwMSec = dwTime;

	day = WORD(dwMSec / (60000 * 60 * 24));
	dwMSec -= day * (60000 * 60 * 24);

	hour = WORD(dwMSec / (60000 * 60));
	dwMSec -= hour * (60000 * 60);

	min = WORD(dwMSec / (60000));
	dwMSec -= min * 60000;

	sec = WORD(dwMSec / 1000);
	dwMSec -= sec * 1000;
}

void CGuildTournamentMgr::NetworkMsgParse( BYTE Protocol,void* pMsg )
{
	switch( Protocol )
	{
	case MP_GTOURNAMENT_REGIST_ACK:
		{
			MSG_NAME_DWORD4* pmsg = (MSG_NAME_DWORD4*)pMsg;

			CHATMGR->AddMsg(CTC_GETMONEY, CHATMGR->GetChatMsg(1602), AddComma(pmsg->dwData2));
			CHATMGR->AddMsg(CTC_GTOURNAMENT, CHATMGR->GetChatMsg(1603), pmsg->dwData1, pmsg->Name);

			// 명단수정요청
			if(HERO->GetGuildIdx() == pmsg->dwData4)
			{
				MSGBASE msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_REGISTPLAYER_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send( &msg, sizeof(msg) );
			}
		}
		break;
	case MP_GTOURNAMENT_REGIST_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			char buf[256];
			switch( pmsg->dwData )
			{
			case eGTError_NOTREGISTDAY:
				sprintf( buf, CHATMGR->GetChatMsg(1604) );
				WINDOWMGR->MsgBox( MBI_GTREGIST_NACK, MBT_OK, buf );
				break;
			case eGTError_DONTFINDGUILDINFO:
				sprintf( buf, CHATMGR->GetChatMsg(1605) );
				WINDOWMGR->MsgBox( MBI_GTREGIST_NACK, MBT_OK, buf );
				break;
			case eGTError_MAXGUILDOVER:
				sprintf( buf, CHATMGR->GetChatMsg(1606) );
				WINDOWMGR->MsgBox( MBI_GTREGIST_NACK, MBT_OK, buf );
				break;
			case eGTError_ALREADYREGISTE:
				sprintf( buf, CHATMGR->GetChatMsg(1607) );
				WINDOWMGR->MsgBox( MBI_GTREGIST_NACK, MBT_OK, buf );
				break;
			case eGTError_ALREADYCANCELREGISTE:				
				//sprintf( buf, "eGTError_ALREADYCANCELREGISTE" );
				//WINDOWMGR->MsgBox( MBI_GTREGIST_NACK, MBT_OK, buf );				
				break;
			case eGTError_UNDERLIMITEMEMBER:
				sprintf( buf, CHATMGR->GetChatMsg(1608) );
				CHATMGR->AddMsg( CTC_SYSMSG, buf );
				break;
			case eGTError_UNDERLEVEL:
				sprintf( buf, CHATMGR->GetChatMsg(1609) );
				CHATMGR->AddMsg( CTC_SYSMSG, buf );
				break;
			case eGTError_DELETEAPPLIED:
				//sprintf( buf, "eGTError_DELETEAPPLIED" );
				//CHATMGR->AddMsg( CTC_SYSMSG, buf );
				break;
			case eGTError_NOGUILDMASTER:
				sprintf( buf, CHATMGR->GetChatMsg(1610) );
				CHATMGR->AddMsg( CTC_SYSMSG, buf );
				break;
			case eGTError_DONTFINDPOSITION:
				//sprintf( buf, "eGTError_DONTFINDPOSITION" );
				//CHATMGR->AddMsg( CTC_SYSMSG, buf );
				break;
			case eGTError_DONTFINDBATTLE:
				//sprintf( buf, "eGTError_DONTFINDBATTLE" );
				//CHATMGR->AddMsg( CTC_SYSMSG, buf );
				break;
			case eGTError_NOTENOUGHFEE:
				sprintf( buf, CHATMGR->GetChatMsg(1611) );
				CHATMGR->AddMsg( CTC_SYSMSG, buf );
				break;
			default:
				//sprintf( buf, "default" );
				//CHATMGR->AddMsg( CTC_SYSMSG, buf );
				break;
			}

			if( HERO->GetState() == eObjectState_Deal )
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
		}
		break;
	case MP_GTOURNAMENT_REGISTPLAYER_ACK:
		{
			MSG_GTEDIT_PLAYERS* pmsg = (MSG_GTEDIT_PLAYERS*)pMsg;

			if(GAMEIN->GetGTEntryEditDlg())
			{
				GAMEIN->GetGTEntryEditDlg()->SetRemainTime(pmsg->dwRemainTime);
				GAMEIN->GetGTEntryEditDlg()->SetList(pmsg->dwPlayers);
			}
		}
		break;
	case MP_GTOURNAMENT_REGISTPLAYER_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			switch(pmsg->dwData)
			{
			case eGTError_NOTREGISTDAY:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1604) );
				break;
			case eGTError_NOGUILDMASTER:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1610) );
				break;
			case eGTError_DONTFINDGUILDINFO:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1605) );
				break;
			}
		}
		break;
	case MP_GTOURNAMENT_EDITPLAYER_ACK:
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1612) );
		}
		break;
	case MP_GTOURNAMENT_EDITPLAYER_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			switch(pmsg->dwData)
			{
			case eGTError_NOGUILDMASTER:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1610) );
				break;
			case eGTError_DONTFINDGUILDINFO:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1605) );
				break;
			}
		}
		break;
	case MP_GTOURNAMENT_FORCEREG_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			switch(pmsg->dwData)
			{
			case eGTError_DONTFINDGUILDINFO:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1605) );
				break;

			case eGTError_ALREADYREGISTE:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1607) );
				break;

			case eGTError_FAILTOREGIST:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1613) );
				break;
			}
		}
		break;
	case MP_GTOURNAMENT_MOVETOBATTLEMAP_ACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			if(GAMEIN->GetGTResultDlg())
				GAMEIN->GetGTResultDlg()->ClearResult();
			
			DWORD Mapnum = GTMAPNUM;
			if( pmsg->dwData == 0 )
				MAPCHANGE->SetGameInInitKind( eGameInInitKind_GTEnter );
			else
				MAPCHANGE->SetGameInInitKind( eGameInInitKind_GTObserverEnter );
			MAINGAME->SetGameState( eGAMESTATE_MAPCHANGE, &Mapnum, 4 );
			OBJECTSTATEMGR->EndObjectState( HERO, eObjectState_Deal );
			GAMEIN->SetGuildIdx( HERO->GetGuildIdx() );
			GAMEIN->SetMoveMap( MAP->GetMapNum() );
			GAMEIN->SetBattleIdx( pmsg->dwData );
			CHATMGR->SaveChatList();

			// 080411 LYW --- GuildTournamentMgr : 채팅방에서 채팅중이었다면, 채팅내용을 저장한다.
			CHATROOMMGR->SaveChat() ;
			
			WINDOWMGR->MsgBox( MBI_NOBTNMSGBOX, MBT_NOBTN, CHATMGR->GetChatMsg(206));
		}
		break;
	case MP_GTOURNAMENT_MOVETOBATTLEMAP_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			switch( pmsg->dwData )
			{
			case eGTError_DONTFINDBATTLE:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1614) );
				break;
			case eGTError_DONTFINDGUILDINFO:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1605) );
				break;
			case eGTError_NOTENTERTIME:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1615) );
				break;
			case eGTError_READYTONEXTMATCH:
				WINDOWMGR->MsgBox( MBI_GTREGIST_NACK, MBT_OK, CHATMGR->GetChatMsg(1616) );
				break;
			case eGTError_NOTALLOWOBSERVER:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1617) );
				break;
			case eGTError_STUDENTCANTENTER:
			default:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1618) );
				break;
			}
		}
		break;
	case MP_GTOURNAMENT_BATTLEJOIN_ACK:
		{
			SEND_BATTLESCORE_INFO* pmsg = (SEND_BATTLESCORE_INFO*)pMsg;

			if( GAMEIN->GetGTScoreInfoDlg() )
			{
				GAMEIN->GetGTScoreInfoDlg()->SetActive( TRUE );
				GAMEIN->GetGTScoreInfoDlg()->SetBattleInfo( pmsg );
				
			//	if( pmsg->State == eGTState_Process )
			//		GAMEIN->GetGTScoreInfoDlg()->StartBattle();		
			//	
			//	if( pmsg->Team == 2 )
			//		GAMEIN->GetGTScoreInfoDlg()->ShowOutBtn( TRUE );
			//	else
			//		GAMEIN->GetGTScoreInfoDlg()->ShowOutBtn( FALSE );
			}

			int nTeam = pmsg->Team;
			if( GAMEIN->GetGTResultDlg()/* && nTeam<2*/)
			{
				GAMEIN->GetGTResultDlg()->SetGuildName(pmsg->GuildName[0], pmsg->GuildName[1]);

				int i;
				for(i=0; i<MAX_GTOURNAMENT_PLAYER; i++)
					GAMEIN->GetGTResultDlg()->SetKillCnt(pmsg->Players[nTeam][i], pmsg->PlayerScore[nTeam][i]);
			}
		}
		break;
	case MP_GTOURNAMENT_RETURNTOMAP:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			GAMEIN->GetMiniMapDialog()->SetActive( TRUE );

			MAPCHANGE->SetGameInInitKind( eGameInInitKind_GTLeave );
			DWORD MapNum = pmsg->dwData;
			MAINGAME->SetGameState( eGAMESTATE_MAPCHANGE, &MapNum, 4 );
			CHATMGR->SaveChatList();

			// 080411 LYW --- GuildTournamentMgr : 채팅방에서 채팅중이었다면, 채팅내용을 저장한다.
			CHATROOMMGR->SaveChat() ;
		}
		break;
	case MP_GTOURNAMENT_STANDINGINFO_ACK:
		{
			SEND_REGISTEDGUILDINFO* pmsg = (SEND_REGISTEDGUILDINFO*)pMsg;

			if(pmsg->MaxTeam == 8)
			{
				// 8강전 테이블표시
				if( GAMEIN->GetGTStandingDlg() )
				{
					GAMEIN->GetGTStandingDlg()->ResetAll();

					bool bInvaildPosition = false;
					for(int i=0; i<pmsg->Count; ++i)
					{
						if(pmsg->GInfo[i].Position > eGTStanding_8TH)
						{
							bInvaildPosition = true;
							break;
						}

						GAMEIN->GetGTStandingDlg()->AddGuildInfo( &pmsg->GInfo[i] );
					}

					if(bInvaildPosition)
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1656) ) ;
					}
					else
					{
						GAMEIN->GetGTStandingDlg()->SetCurTournament( pmsg->CurTournament );
						GAMEIN->GetGTStandingDlg()->SetCurRound(pmsg->CurRound);
						GAMEIN->GetGTStandingDlg()->SetCurState(pmsg->CurState);
						GAMEIN->GetGTStandingDlg()->SetRemainTime(pmsg->RemainTime);
						GAMEIN->GetGTStandingDlg()->RefreshGuildInfo();
						GAMEIN->GetGTStandingDlg()->SetActive( TRUE );
					}
				}
			}
			else if(pmsg->MaxTeam == 16)
			{
				// 16강전 테이블표시
				if( GAMEIN->GetGTStandingDlg16() )
				{
					GAMEIN->GetGTStandingDlg16()->ResetAll();

					bool bInvaildPosition = false;
					for(int i=0; i<pmsg->Count; ++i)
					{
						if(pmsg->GInfo[i].Position > eGTStanding_16TH)
						{
							bInvaildPosition = true;
							break;
						}

						GAMEIN->GetGTStandingDlg16()->AddGuildInfo( &pmsg->GInfo[i] );
					}

					if(bInvaildPosition)
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1656) ) ;
					}
					else
					{
						GAMEIN->GetGTStandingDlg16()->SetCurTournament( pmsg->CurTournament );
						GAMEIN->GetGTStandingDlg16()->SetCurRound(pmsg->CurRound);
						//GAMEIN->GetGTStandingDlg16()->SetCurState(pmsg->CurState);
						GAMEIN->GetGTStandingDlg16()->RefreshGuildInfo();
						GAMEIN->GetGTStandingDlg16()->SetActive( TRUE );
					}
				}
			}
		}
		break;
	case MP_GTOURNAMENT_STANDINGINFO_NACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			switch(pmsg->dwData2)
			{
			case eGTError_INVALIDDAY:
			default:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1619));
				break;
			}
		}
		break;
	case MP_GTOURNAMENT_TEAMMEMBER_ADD:
		{
			MSG_NAME_DWORD3* pmsg = (MSG_NAME_DWORD3*)pMsg;

			if(pmsg->dwData1 < eBattleTeam_Max)
			{
				if(HERO)
				{
					if(HERO->GetBattleTeam()==pmsg->dwData1)
					{
						if(0 != strcmp(pmsg->Name, ""))
							CHATMGR->AddMsg( CTC_GTOURNAMENT, CHATMGR->GetChatMsg(1620), pmsg->Name );
					}

					if(GAMEIN->GetGTResultDlg())
						GAMEIN->GetGTResultDlg()->SetKillCnt(pmsg->dwData2, pmsg->dwData3);
				}
			}
		}
		break;
	case MP_GTOURNAMENT_TEAMMEMBER_OUT:
		{
			MSG_NAME_DWORD2* pmsg = (MSG_NAME_DWORD2*)pMsg;

			if(pmsg->dwData1 < eBattleTeam_Max)
			{
				if(HERO && (HERO->GetBattleTeam()==pmsg->dwData1))
				{
					if(0 == strcmp(pmsg->Name, ""))
						return;

					CHATMGR->AddMsg( CTC_GTOURNAMENT, CHATMGR->GetChatMsg(1621), pmsg->Name );
				}
			}
		}		
		break;
	case MP_GTOURNAMENT_TEAMMEMBER_DIE:
		{
			MSG_DWORD6* pmsg = (MSG_DWORD6*)pMsg;
			
			CPlayer* pKiller = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwData2);
			if(!pKiller)	return;
			CPlayer* pVictim = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwData3);
			if(!pVictim)	return;

			CHATMGR->AddMsg( CTC_GTOURNAMENT, CHATMGR->GetChatMsg(1622), pVictim->GetObjectName(), pKiller->GetObjectName() );

			if( GAMEIN->GetGTScoreInfoDlg() )
				GAMEIN->GetGTScoreInfoDlg()->SetTeamScore( 1 - pmsg->dwData1, pmsg->dwData6 ); 


			if(pVictim->GetID() == HEROID)
			{
				CHATMGR->AddMsg( CTC_GTOURNAMENT, CHATMGR->GetChatMsg(1623), pmsg->dwData5/1000 );
			}

			if(GAMEIN->GetGTResultDlg())
				GAMEIN->GetGTResultDlg()->SetKillCnt(pKiller->GetID(), pmsg->dwData4);
		}
		break;
	case MP_GTOURNAMENT_TEAMMEMBER_MORTAL:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwData1);
			if(!pPlayer)	return;

			pPlayer->SetImmortalTimeOnGTMAP(pmsg->dwData2);

			if(pmsg->dwData2)
			{
				OBJECTEFFECTDESC desc( 250 );
				pPlayer->AddObjectEffect( BATTLE_GTIMMORTAL_ID, &desc, 1, HERO);
			}
			else
			{
				pPlayer->RemoveObjectEffect( BATTLE_GTIMMORTAL_ID);
			}
		}
		break;
	case MP_GTOURNAMENT_NOTIFY_WINLOSE:
		{
			SEND_GTWINLOSE* pmsg = (SEND_GTWINLOSE*)pMsg;

			char buf[256] = {0,};
			char* pRound = NULL;

			switch(pmsg->Param)
			{
			case eGTFight_32:	pRound = CHATMGR->GetChatMsg(1624);		break;
			case eGTFight_16:	pRound = CHATMGR->GetChatMsg(1625);		break;
			case eGTFight_8:	pRound = CHATMGR->GetChatMsg(1626);		break;
			case eGTFight_4:	pRound = CHATMGR->GetChatMsg(1627);		break;
			case eGTFight_2:	pRound = CHATMGR->GetChatMsg(1628);		break;
			}

			if(pmsg->bWin)
			{
				if(eGTFight_1 == pmsg->Param)	// 결승에서 우승
				{
					sprintf(buf, CHATMGR->GetChatMsg(1639), pmsg->GuildName, pmsg->TournamentCount);
					CHATMGR->AddMsg( CTC_GTOURNAMENT, buf);
				}
				else
				{
					sprintf(buf, CHATMGR->GetChatMsg(1640), pmsg->GuildName, pmsg->TournamentCount, pRound);
					CHATMGR->AddMsg( CTC_GTOURNAMENT, buf);
				}
			}
			else
			{
				if(eGTFight_1 == pmsg->Param)	// 결승에서 패배(=준우승)
				{
					sprintf(buf, CHATMGR->GetChatMsg(1653), pmsg->GuildName, pmsg->TournamentCount);
					CHATMGR->AddMsg( CTC_GTOURNAMENT, buf);
				}
			}
		}
		break;
	case MP_GTOURNAMENT_NOTIFY:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

			char buf[256] = {0,};
			char* pRound = NULL;

			DWORD dwRemain;
			double dRemain;

			if(eGTNotify_RoundStart==pmsg->dwData1 || eGTNotify_RoundEnd==pmsg->dwData1 || eGTNotify_EnteranceRemain==pmsg->dwData1)
			{
				switch(pmsg->dwData2)
				{
				case eGTFight_32:	pRound = CHATMGR->GetChatMsg(1624);		break;
				case eGTFight_16:	pRound = CHATMGR->GetChatMsg(1625);		break;
				case eGTFight_8:	pRound = CHATMGR->GetChatMsg(1626);		break;
				case eGTFight_4:	pRound = CHATMGR->GetChatMsg(1627);		break;
				case eGTFight_2:	pRound = CHATMGR->GetChatMsg(1628);		break;
				}
			}

			switch(pmsg->dwData1)
			{
			case eGTNotify_RegistStart:		sprintf(buf, CHATMGR->GetChatMsg(1631), pmsg->dwData2);	break;
			case eGTNotify_RegistEnd:		sprintf(buf, CHATMGR->GetChatMsg(1632), pmsg->dwData2);	break;
			case eGTNotify_TournamentStart:	sprintf(buf, CHATMGR->GetChatMsg(1633), pmsg->dwData2);	break;
			case eGTNotify_TournamentEnd:	sprintf(buf, CHATMGR->GetChatMsg(1634), pmsg->dwData2);	break;
			case eGTNotify_RoundStart:
				{
					if(pmsg->dwData3 == 0)
					{
						sprintf(buf, CHATMGR->GetChatMsg(1651), pRound);
					}
					else
					{
						dRemain = (double)pmsg->dwData3 / 60000.f;
						dwRemain = (DWORD)floor(dRemain+0.5f);
						sprintf(buf, CHATMGR->GetChatMsg(1635), pRound, dwRemain);
					}

				}break;
			case eGTNotify_RoundEnd:
				{
 					sprintf(buf, CHATMGR->GetChatMsg(1636), pRound);
					break;
				}
			case eGTNotify_EnteranceRemain:
				{
					dRemain = (double)pmsg->dwData3 / 60000.f;
					dwRemain = (DWORD)floor(dRemain+0.5f);
					sprintf(buf, CHATMGR->GetChatMsg(1637), pRound, dwRemain);
				}break;
			case eGTNotify_RegistRemain:
				{
					dRemain = (double)pmsg->dwData2 / 60000.f;
					dwRemain = (DWORD)floor(dRemain+0.5f);
					sprintf(buf, CHATMGR->GetChatMsg(1641), dwRemain);
				}break;
			case eGTNotify_BattleTableOpen:
				{
					dRemain = (double)pmsg->dwData3 / 60000.f;
					dwRemain = (DWORD)floor(dRemain+0.5f);
					sprintf(buf, CHATMGR->GetChatMsg(1642), pmsg->dwData2, dwRemain);
				}break;
			}

			CHATMGR->AddMsg( CTC_GTOURNAMENT, buf);
		}
		break;
	case MP_GTOURNAMENT_NOTIFY2GUILD:
		{
			MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;

			char buf[256] = {0,};
			char* pRound = NULL;

			if(eGTNotify_WinLose==pmsg->dwData2)
			{
				switch(pmsg->dwData4)
				{
				case eGTFight_32:	pRound = CHATMGR->GetChatMsg(1624);		break;
				case eGTFight_16:	pRound = CHATMGR->GetChatMsg(1625);		break;
				case eGTFight_8:	pRound = CHATMGR->GetChatMsg(1626);		break;
				case eGTFight_4:	pRound = CHATMGR->GetChatMsg(1627);		break;
				case eGTFight_2:	pRound = CHATMGR->GetChatMsg(1628);		break;
				}
			}

			switch(pmsg->dwData2)
			{
			case eGTNotify_WinLose:
				{
					if(pmsg->dwData3)
					{
						sprintf(buf, CHATMGR->GetChatMsg(1629), pRound);
						if(pmsg->dwData5)	// 부전승
							strcat(buf, CHATMGR->GetChatMsg(1630));

						CHATMGR->AddMsg( CTC_GTOURNAMENT, buf);
					}
					else
					{
						sprintf(buf, CHATMGR->GetChatMsg(1643), pRound);
						CHATMGR->AddMsg( CTC_GTOURNAMENT, buf);
					}
				}
				break;
			}
		}
		break;
	case MP_GTOURNAMENT_NOTIFY2PLAYER:
		{
			MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;
			char* pRound = NULL;

			if(pmsg->dwData2 == eGTNotify_RoundStartforPlayer)
			{
				switch(pmsg->dwData3)
				{
				case eGTFight_32:	pRound = CHATMGR->GetChatMsg(1624);		break;
				case eGTFight_16:	pRound = CHATMGR->GetChatMsg(1625);		break;
				case eGTFight_8:	pRound = CHATMGR->GetChatMsg(1626);		break;
				case eGTFight_4:	pRound = CHATMGR->GetChatMsg(1627);		break;
				case eGTFight_2:	pRound = CHATMGR->GetChatMsg(1628);		break;
				}
			}

			switch(pmsg->dwData2)
			{
			case eGTNotify_RoundStartforPlayer:
				{
					if(MAP->GetMapNum() != GTMAPNUM)
					{
						double dRemain = (double)pmsg->dwData4 / 60000.f;
						DWORD dwMin = (DWORD)floor(dRemain+0.5f);

						// 091113 ONS 결승전일경우 최대금액이 40억이 넘으면 우승상금을 정상적으로 받을 수 없다는 메세지 출력	
						cMsgBox* pMsgBox = NULL;
						if( eGTFight_2 == pmsg->dwData3 )
						{
							pMsgBox = WINDOWMGR->MsgBox( MBI_GTENTER_SYN, MBT_YESNO, CHATMGR->GetChatMsg(1987), pRound, dwMin);
						}
						else
						{
							pMsgBox = WINDOWMGR->MsgBox( MBI_GTENTER_SYN, MBT_YESNO, CHATMGR->GetChatMsg(1652), pRound, dwMin);
						}
						if(pMsgBox)
							pMsgBox->SetVisibleTime(60000);
					}
				}
				break;
			}
		}
		break;
	case MP_GTOURNAMENT_CHEAT_GETSTATE_ACK:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

			WORD wDay, wHour, wMin, wSec;
			GetDateFromMSec(pmsg->dwData2, wDay, wHour, wMin, wSec);

			char buf[128] = {0,};
			if(wDay)
				sprintf(buf, "Remain(%dD:%dH:%dM:%dS)\n", wDay, wHour, wMin, wSec);
			else if(wHour)
				sprintf(buf, "Remain(%dH:%dM:%dS)\n", wHour, wMin, wSec);
			else if(wMin)
				sprintf(buf, "Remain(%dMin:%dSec)\n", wMin, wSec);
			else if(wSec)
				sprintf(buf, "Remain(%dSec)\n", wSec);

			switch(pmsg->dwData1)
			{
			case eGTState_BeforeRegist:		CHATMGR->AddMsg(CTC_GTOURNAMENT, "State_BeforeRegist, %s", buf);	break;
			case eGTState_Regist:			CHATMGR->AddMsg(CTC_GTOURNAMENT, "State_Regist, %s", buf);			break;
			case eGTState_BeforeEntrance:	CHATMGR->AddMsg(CTC_GTOURNAMENT, "State_BeforeEntrance, %s", buf);	break;
			case eGTState_Entrance:			CHATMGR->AddMsg(CTC_GTOURNAMENT, "State_Entrance, %s", buf);		break;
			case eGTState_Process:			CHATMGR->AddMsg(CTC_GTOURNAMENT, "State_Process, %s", buf);			break;
			case eGTState_Leave:			CHATMGR->AddMsg(CTC_GTOURNAMENT, "State_Leave, %s", buf);			break;
			}
		}
	case MP_GTOURNAMENT_CHEAT:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

			printf("TournamentState[%d] Guild[%d]\n", pmsg->dwData2, pmsg->dwData3);
		}
		break;

	case MP_GTOURNAMENT_REWARD:
		{
			// 091201 ONS 길드토너먼트 우승/준우승 상금 아이템명을 알려준다.
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			const ITEM_INFO* const pItemInfo = ITEMMGR->GetItemInfo(pmsg->dwData);
			if(!pItemInfo)
				break;
			
			CHATMGR->AddMsg(CTC_GTOURNAMENT, CHATMGR->GetChatMsg(1655), pItemInfo->ItemName );
		}
		break;

	// 091201 ONS 길드토너먼트 우승/준우승 상금지급을 알린다.
	case MP_GTOURNAMENT_REWARD_MONEY:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			
			CHATMGR->AddMsg(CTC_GTOURNAMENT, CHATMGR->GetChatMsg(1992), pmsg->dwData);
		}
		break;
	}
}