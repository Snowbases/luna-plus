// Battle_GTournament.cpp: implementation of the CBattle_GTournament class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Battle_GTournament.h"
#include "Object.h"
#include "ObjectStateManager.h"
#include "Player.h"
#include "../[CC]Header/GameResourceManager.h"
#include "GuildTournamentMgr.h"
#include "Guild.h"
#include "GuildManager.h"
#include "Pet.h"

#ifdef _MAPSERVER_
#include "UserTable.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "CharMove.h"
#include "PackedData.h"
#else
#include "MHMap.h"
#include "TileManager.h"
#include "../[Client]LUNA/ObjectManager.h"
#include "../[Client]LUNA/Interface/cScriptManager.h"
#include "GameIn.h"
#include "GTScoreInfoDialog.h"
#include "GTResultDlg.h"
#include "ChatManager.h"
#endif

#define GT_WAITZONE_BLOCKSIZE	500.0f
#define GT_WAITZONE_BLOCKSIZE2	(GT_WAITZONE_BLOCKSIZE + 50.0f)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBattle_GTournament::CBattle_GTournament()
{
#ifndef _MAPSERVER_
	m_pCurShowImage	= NULL;
	m_nLastNum = 0;
#endif

}


CBattle_GTournament::~CBattle_GTournament()
{

}


void CBattle_GTournament::Release()
{
	// 091231 ONS 배틀정보를 해제한다.
	CBattle::Release();
}

void CBattle_GTournament::Initialize(BATTLE_INFO_BASE* pCreateInfo, CBattleTeam* pTeam1, CBattleTeam* pTeam2)
{
	CBattle::Initialize( pCreateInfo, pTeam1, pTeam2 );

	// Player 대기위치
	m_Team1Pos.x = 23400;
	m_Team1Pos.y = 0;
	m_Team1Pos.z = 11500;
	m_Team2Pos.x = 3800;
	m_Team2Pos.y = 0;
	m_Team2Pos.z = 11500;
	m_ObserverPos.x = 13600;
	m_ObserverPos.y = 0;
	m_ObserverPos.z = 11500;

#ifdef _MAPSERVER_
	BATTLE_INFO_GTOURNAMENT* pInfo = (BATTLE_INFO_GTOURNAMENT*)pCreateInfo;
	m_ReadyTime = pInfo->m_dwEntranceTime;
	m_FightTime = pInfo->m_dwFightTime;
	m_ResultTime = pInfo->m_dwLeaveTime;

#else
	m_RenderTime = 5000;
#endif
}


void CBattle_GTournament::OnCreate(BATTLE_INFO_BASE* pCreateInfo, CBattleTeam* pTeam1, CBattleTeam* pTeam2)
{
	CBattle::OnCreate( pCreateInfo, pTeam1, pTeam2 );

	//
	BATTLE_INFO_GTOURNAMENT* pInfo = (BATTLE_INFO_GTOURNAMENT*)pCreateInfo;
	((CBattleTeam_GTournament*)pTeam1)->Init( &pInfo->MemberInfo, eBattleTeam1 );
	((CBattleTeam_GTournament*)pTeam2)->Init( &pInfo->MemberInfo, eBattleTeam2 );

	memcpy( &m_GTInfo, pCreateInfo, sizeof(BATTLE_INFO_GTOURNAMENT) );

#ifndef _MAPSERVER_
	m_ReadyTime = pInfo->m_dwEntranceTime;
	m_FightTime = pInfo->m_dwFightTime;
	m_ResultTime = pInfo->m_dwLeaveTime;

	m_ImageNumber.Init( 32, 0 );
	m_ImageNumber.SetFillZero( FALSE );
	m_ImageNumber.SetLimitCipher( 2 );
	m_ImageNumber.SetScale( 3.0f, 3.0f );
	const DISPLAY_INFO&	dispInfo2 = GAMERESRCMNGR->GetResolution();
	m_ImageNumber.SetPosition( (dispInfo2.dwWidth-16)/2, 150 );


	m_GuildIdx[0] = pInfo->MemberInfo.TeamGuildIdx[0];
	m_GuildIdx[1] = pInfo->MemberInfo.TeamGuildIdx[1];

	SCRIPTMGR->GetImage( 141, &m_ImageWin, PFT_HARDPATH );
	SCRIPTMGR->GetImage( 140, &m_ImageLose, PFT_HARDPATH );
	
	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
	m_vTitlePos.x = (float)((dispInfo.dwWidth - (dispInfo.dwWidth/2)) / 2);	//128 width
	// 070202 LYW --- End.
	//m_vTitlePos.y = 100;
	m_vTitlePos.y = (float)((dispInfo.dwHeight - (dispInfo.dwHeight/2)) / 2);	//128 = width
	m_vTitleScale.x = 1.0f;
	m_vTitleScale.y = 1.0f;

	if( pInfo->BattleState == eBATTLE_STATE_READY )
	{
		m_bMovedBridge = FALSE;
		SetBrokenBridge();
	}
	else
	{
		m_bMovedBridge = TRUE;
		VECTOR3 Pos1;	Pos1.x = 0;	Pos1.y = 0;	Pos1.z = 0;
		OBJECTEFFECTDESC desc( FindEffectNum("GTMAP_Area_e.beff"), EFFECT_FLAG_ABSOLUTEPOS, &Pos1 );
		HERO->AddObjectEffect( BATTLE_GTTEAM3_EFFECTID, &desc, 1 );
	}
#else

#endif
}


void CBattle_GTournament::OnDestroy()
{
#ifdef _MAPSERVER_
	ReleaseAllObject();

	ToEachTeam(pTeam)
		((CBattleTeam_GTournament*)pTeam)->AliveTeamMember();
		((CBattleTeam_GTournament*)pTeam)->ReturnToMap();
	EndToEachTeam
		m_Observer.ReturnToMap();
#else
	if(GAMEIN->GetGTScoreInfoDlg())
		GAMEIN->GetGTScoreInfoDlg()->EndBattle();
#endif
}


BOOL CBattle_GTournament::OnTeamMemberDie(int Team,DWORD VictimMemberID,DWORD KillerID)
{
#ifdef _MAPSERVER_
	CPlayer* pDeadObject = NULL;
	CPlayer* pKillerObject = NULL;

	CObject* pTempDeadObject = g_pUserTable->FindUser(VictimMemberID);
	if(!pTempDeadObject)	return FALSE;

	if(pTempDeadObject->GetObjectKind() == eObjectKind_Pet)
		return FALSE;
	else if(pTempDeadObject->GetObjectKind() == eObjectKind_Player)
		pDeadObject = (CPlayer*)pTempDeadObject;

	if(!pDeadObject)		return FALSE;

	CObject* pTempKillerObject = g_pUserTable->FindUser(KillerID);
	if(!pTempKillerObject)	return FALSE;

	if(pTempKillerObject->GetObjectKind() == eObjectKind_Pet)
	{
		CObject* const object = g_pUserTable->FindUser(
			pKillerObject->GetOwnerIndex());

		if(0 == object)
		{
			pKillerObject = 0;
		}
		else if(eObjectKind_Player == object->GetObjectKind())
		{
			pKillerObject = (CPlayer*)object;
		}
	}
	else if(pTempKillerObject->GetObjectKind() == eObjectKind_Player)
		pKillerObject = (CPlayer*)pTempKillerObject;

	if(!pKillerObject)		return FALSE;

	//DWORD point = CalcKillPoint(pKillerObject->GetLevel(), pDeadObject->GetLevel());
	DWORD point = CalcKillPointByStage(pKillerObject->GetStage(), pDeadObject->GetStage());
	DWORD dwRespawnTime = GTMGR->GetRespawnByLevel(pDeadObject->GetLevel());
	pKillerObject->AddKillCountFromGT();
	pKillerObject->AddKillPointFromGT((WORD)point);

	GTMGR->SetPlayerScore(pKillerObject->GetGuildIdx(), pKillerObject->GetID(), pKillerObject->GetKillCountFromGT());

	if( Team < 2 )
	{
		((CBattleTeam_GTournament*)m_TeamArray[1-Team])->AddKillPoint((WORD)point);
		((CBattleTeam_GTournament*)m_TeamArray[Team])->IncreaseDieCount();

		MSG_DWORD6 msg;
		SetProtocol(&msg,MP_GTOURNAMENT,MP_GTOURNAMENT_TEAMMEMBER_DIE);
		msg.Category = MP_GTOURNAMENT;
		msg.dwData1 = Team;
		msg.dwData2 = pKillerObject->GetID();
		msg.dwData3 = pDeadObject->GetID();
		msg.dwData4 = pKillerObject->GetKillPointFromGT();//pKillerObject->GetKillCountFromGT();
		msg.dwData5 = dwRespawnTime;
		msg.dwData6 = ((CBattleTeam_GTournament*)m_TeamArray[1-Team])->GetKillPoint();
		ToEachTeam(pTeam)
		pTeam->SendTeamMsg( &msg, sizeof(msg) );
		EndToEachTeam
		m_Observer.SendTeamMsg( &msg, sizeof(msg) );
	}
	OBJECTSTATEMGR_OBJ->StartObjectState( pDeadObject, eObjectState_Die, 0 );
	((CPlayer*)pDeadObject)->SetReadyToRevive( TRUE );
	
	pDeadObject->SetRespawnTime(dwRespawnTime);
	pDeadObject->m_bNeedRevive = TRUE;
#endif

	//return TRUE;
	return FALSE;
}

void CBattle_GTournament::OnTeamMemberRevive(int Team,CObject* pReviver)
{
#ifdef _MAPSERVER_
	if( !pReviver )			return;

	if( Team < 2 )
	{		
		WORD wCode = ((CPlayer*)pReviver)->GetJobCodeForGT();
		DWORD dwImmortalTime = GTMGR->GetImmortalTimeByClass(wCode);

		MSG_DWORD2 msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_TEAMMEMBER_MORTAL;
		msg.dwData1 = pReviver->GetID();
		msg.dwData2 = dwImmortalTime;
		ToEachTeam(pTeam)
		pTeam->SendTeamMsg( &msg, sizeof(msg) );
		EndToEachTeam
		m_Observer.SendTeamMsg( &msg, sizeof(msg) );
	}
#else
#endif
}


void CBattle_GTournament::StartBattle()
{
	m_GTInfo.m_dwEntranceTime = 0;
	OnFightStart();

#ifdef _MAPSERVER_

	MSG_DWORD msg;
	msg.Category = MP_BATTLE;
	msg.Protocol = MP_BATTLE_START_NOTIFY;
	msg.dwData = GetBattleID();

	ToEachTeam(pTeam)
		pTeam->SendTeamMsg(&msg,sizeof(msg));
	EndToEachTeam
	m_Observer.SendTeamMsg( &msg, sizeof(msg) );
#else
	if(GAMEIN->GetGTResultDlg())
		GAMEIN->GetGTResultDlg()->FillName();
#endif
}

VECTOR3* CBattle_GTournament::GetRevivePoint(DWORD dwTeam)
{
	if(dwTeam == eBattleTeam1)
		return &m_Team1Pos;
	else if(dwTeam == eBattleTeam2)
		return &m_Team2Pos;
	else
		return &m_ObserverPos;
}


void CBattle_GTournament::OnTick()
{
	ySWITCH(m_BattleInfo.BattleState)
		yCASE(eBATTLE_STATE_READY)
			if( gTickTime < m_GTInfo.m_dwEntranceTime)
			{
				m_GTInfo.m_dwEntranceTime -= gTickTime;
			}
			else
			{
				m_GTInfo.m_dwEntranceTime = 0;
			}
		yCASE(eBATTLE_STATE_FIGHT)
			if( gTickTime < m_GTInfo.m_dwFightTime)
			{
				m_GTInfo.m_dwFightTime -= gTickTime;
				Judge();
			}
			else
			{
				m_GTInfo.m_dwFightTime = 0;
				Judge();
			}
		yCASE(eBATTLE_STATE_RESULT)
			if( gTickTime < m_GTInfo.m_dwLeaveTime )
			{
				m_GTInfo.m_dwLeaveTime -= gTickTime;
			}
			else
			{
				m_GTInfo.m_dwLeaveTime = 0;

				// 일단 우선 내보낸다.
				SetDestroyFlag();
			}
	yENDSWITCH
}



void CBattle_GTournament::OnFightStart()
{
	CBattle::OnFightStart();

#ifdef _MAPSERVER_

#else
	m_dwMoveBridgeTime = 3500;
	HERO->RemoveObjectEffect( BATTLE_GTTEAM1_EFFECTID );
	
	VECTOR3 Pos1;	Pos1.x = 0;	Pos1.y = 0;	Pos1.z = 0;
	OBJECTEFFECTDESC desc( FindEffectNum("GTMAP_Area_r.beff"), EFFECT_FLAG_ABSOLUTEPOS, &Pos1 );
	HERO->AddObjectEffect( BATTLE_GTTEAM2_EFFECTID, &desc, 1 );
	
	if(GAMEIN->GetGTScoreInfoDlg())
		GAMEIN->GetGTScoreInfoDlg()->StartBattle();
	m_RenderTime = 5000;
#endif
}



BOOL CBattle_GTournament::Judge()
{
#ifdef _MAPSERVER_
	// 문파원 체크
	if( JudgeMemberExist() )
		return TRUE;

	// 목표킬수를 채웠을때
	DWORD dwType, dwObject;
	GTMGR->GetPlayTypeObject(dwType, dwObject);
	
	if(dwType==0)	// 데스매치
	{
		DWORD data1 = ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam1])->GetKillPoint();
		DWORD data2 = ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam2])->GetKillPoint();

		if(dwObject<=data1 && dwObject<=data2)
			LoseAll();
		else if(dwObject<=data1)
			Victory(eBattleTeam1, eBattleTeam2);
		else if(dwObject<=data2)
			Victory(eBattleTeam2, eBattleTeam1);
	}
#endif

	// 시간이 끝났을때
	if( m_GTInfo.m_dwFightTime == 0 )
	{
		if(((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam1])->GetRemainMember() == 0)
			Victory( eBattleTeam2, eBattleTeam1 );
		else if(((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam2])->GetRemainMember() == 0)
			Victory( eBattleTeam1, eBattleTeam2 );
		else
		{
			if( JudgeKillPoint() == FALSE )				// 길드 킬스코어 높은쪽
			//if( JudgeMemberKill() == FALSE )			// 많이 죽인 Kill 수
			if( JudgeGuildLevel() == FALSE )			// 길드레벨 낮은쪽
			//if( JudgeMemberLevel() == FALSE )			// 살아남은 사람의 Level의 합이 낮은쪽
			//if( JudgeMemberLevelExp() == FALSE )		// 살아남은 사람의 경험치 총합이 낮은쪽
			{
				// Random
				DWORD data = rand()%2;
				Victory( data, 1-data );
			}
		}
	}

	return FALSE;
}


BOOL CBattle_GTournament::JudgeMemberExist()
{
	if ( ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam1])->GetRemainMember() == 0 )
	{
		if( ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam2])->GetRemainMember() == 0 )
		{
			LoseAll();
			return TRUE;
		}
		else
		{
			Victory( eBattleTeam2, eBattleTeam1 );
			return TRUE;
		}
	}

	if( ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam2])->GetRemainMember() == 0 )
	{
		if( ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam1])->GetRemainMember() == 0 )
		{
			LoseAll();
			return TRUE;
		}
		else
		{
			Victory( eBattleTeam1, eBattleTeam2 );			
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CBattle_GTournament::JudgeKillPoint()
{
	DWORD data1 = ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam1])->GetKillPoint();
	DWORD data2 = ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam2])->GetKillPoint();

	if(data1 == data2)				return FALSE;
	else if( data1 > data2 )
		Victory( eBattleTeam1, eBattleTeam2 );
	else
		Victory( eBattleTeam2, eBattleTeam1 );

	return TRUE;
}

BOOL CBattle_GTournament::JudgeGuildLevel()
{
	DWORD data1 = ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam1])->GetGuildLevel();
	DWORD data2 = ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam2])->GetGuildLevel();

	if(data1 == data2)
		return FALSE;
	else if(data1 < data2)
		Victory(eBattleTeam1, eBattleTeam2);
	else if(data1 > data2)
		Victory(eBattleTeam2, eBattleTeam1);

	return TRUE;
}

// 3
BOOL CBattle_GTournament::JudgeMemberKill()
{
	DWORD data1 = ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam1])->GetDieCount();
	DWORD data2 = ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam2])->GetDieCount();

	if( data1 == data2 )			return FALSE;
	else if( data1 > data2 )
		Victory( eBattleTeam2, eBattleTeam1 );
	else
		Victory( eBattleTeam1, eBattleTeam2 );

	return TRUE;
}
// 4
BOOL CBattle_GTournament::JudgeMemberLevel()
{
	DWORD data1 = ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam1])->GetTotalLevel();
	DWORD data2 = ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam2])->GetTotalLevel();

	if( data1 == data2 )			return FALSE;
	else if( data1 > data2 )
		Victory( eBattleTeam2, eBattleTeam1 );
	else
		Victory( eBattleTeam1, eBattleTeam2 );

	return TRUE;
}
// 5
BOOL CBattle_GTournament::JudgeMemberLevelExp()
{
	DOUBLE data1 = ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam1])->GetTotalLevelExp();
	DOUBLE data2 = ((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam2])->GetTotalLevelExp();

	if( data1 == data2 )			return FALSE;
	else if( data1 > data2 )
		Victory( eBattleTeam2, eBattleTeam1 );
	else
		Victory( eBattleTeam1, eBattleTeam2 );

	return TRUE;
}



void CBattle_GTournament::Victory(int WinnerTeamNum,int LoserTeamNum)
{
	CBattle::Victory( WinnerTeamNum, LoserTeamNum );

	((CBattleTeam_GTournament*)m_TeamArray[WinnerTeamNum])->SetResult( TRUE );
	((CBattleTeam_GTournament*)m_TeamArray[LoserTeamNum])->SetResult( FALSE );

#ifndef _MAPSERVER_
	if(GAMEIN->GetGTResultDlg())
		GAMEIN->GetGTResultDlg()->FillName();

	if(HERO->GetBattleTeam() == eBattleTeam_Max)
		return;

	if( HERO->GetBattleTeam() == WinnerTeamNum )
		m_pCurShowImage = &m_ImageWin;
	else
		m_pCurShowImage = &m_ImageLose;

	if(GAMEIN->GetGTResultDlg())
			GAMEIN->GetGTResultDlg()->SetResult();

	m_vTitleScale.x = 1.0f;
	m_vTitleScale.y = 1.0f;

	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
	m_vTitlePos.x = (dispInfo.dwWidth - (dispInfo.dwWidth/2) * m_vTitleScale.x ) / 2;	//128 = width
	// 070202 LYW --- End.

	//m_vTitlePos.y = 100;
	m_vTitlePos.y = (dispInfo.dwHeight - (dispInfo.dwHeight/2) * m_vTitleScale.y ) / 2;	//128 = width
	m_RenderTime = 10000;

	//AutoAttack멈춤
	HERO->DisableAutoAttack();			//공격중 이던거 취소되나?
	HERO->SetNextAction(NULL);			//스킬 쓰던것도 취소되나?

#else
	MSG_DWORD2 msg;
	msg.Category = MP_BATTLE;
	msg.Protocol = MP_BATTLE_VICTORY_NOTIFY;
	msg.dwData1 = WinnerTeamNum;
	msg.dwData2 = LoserTeamNum;
	m_Observer.SendTeamMsg( &msg, sizeof(msg) );
#endif

}


void CBattle_GTournament::Draw()
{
	CBattle::Draw();

#ifndef _MAPSERVER_
	


#endif

}


void CBattle_GTournament::LoseAll()
{
	// 양쪽다 진상태.
#ifdef _MAPSERVER_

	m_WinnerTeam = 2;
	m_BattleInfo.BattleState = eBATTLE_STATE_RESULT;
	m_BattleInfo.BattleTime = gCurTime;

	((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam1])->SetResult( FALSE );
	((CBattleTeam_GTournament*)m_TeamArray[eBattleTeam2])->SetResult( FALSE );
#else

#endif

}


void CBattle_GTournament::OnTeamMemberAdd(int Team,DWORD MemberID,char* Name)
{
#ifdef _MAPSERVER_

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( MemberID );
	if( !pPlayer )		return;

	VECTOR3 vPos = Team==eBattleTeam1?m_Team1Pos:Team==eBattleTeam2?m_Team2Pos:m_ObserverPos;
	pPlayer->SetStartPosOnGTMAP(&vPos);

	if( Team < 2 )
	{
		MSG_NAME_DWORD3 msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_TEAMMEMBER_ADD;
		msg.dwData1 = Team;
		msg.dwData2 = MemberID;
		msg.dwData3 = GTMGR->GetPlayerScore(pPlayer->GetGuildIdx(), pPlayer->GetID());
		SafeStrCpy(msg.Name, pPlayer->GetObjectName(), sizeof(msg.Name));
		ToEachTeam(pTeam)
			pTeam->SendTeamMsg( &msg, sizeof(msg) );
		EndToEachTeam
			m_Observer.SendTeamMsg( &msg, sizeof(msg) );
	}

#else

#endif
}


DWORD CBattle_GTournament::GetBattleTeamID( CObject* pObject )
{
#ifdef _MAPSERVER_
	for(int n=0; n<eBattleTeam_Max; ++n)
	{
		if( m_TeamArray[n] && m_TeamArray[n]->IsTeamMember( pObject ) )		
			return n;
	}
	
	if( m_Observer.IsTeamMember( pObject ) )
		return 2;
#else
	for(int n=0; n<eBattleTeam_Max; ++n)
	{
		if( ((CPlayer*)pObject)->GetGuildIdx() == m_GuildIdx[n] )
			return n;
	}
#endif
	return 2;
}


BOOL CBattle_GTournament::IsEnemy(CObject* pOperator,CObject* pTarget)
{
	if( m_BattleInfo.BattleState != eBATTLE_STATE_FIGHT )
		return FALSE;
	if( pOperator->GetObjectKind() != eObjectKind_Player ||
		pTarget->GetObjectKind() != eObjectKind_Player )
		return FALSE;
	if( pOperator->GetBattleID() != pTarget->GetBattleID() )
		return FALSE;
	if( pOperator->GetBattleTeam() == 2 )
		return FALSE;

	if(((CPlayer*)pTarget)->GetImmortalTime())
		return FALSE;


#ifndef _MAPSERVER_

	if( pTarget->GetBattleTeam() != 2 && pOperator->GetBattleTeam() != pTarget->GetBattleTeam() )
		return TRUE;
#else
	if( m_TeamArray[0]->IsTeamMember( pOperator ) )
		return m_TeamArray[1]->IsTeamMember( pTarget );
	else if( m_TeamArray[1]->IsTeamMember( pOperator ) )
		return m_TeamArray[0]->IsTeamMember( pTarget );
#endif

	return FALSE;
}



BOOL CBattle_GTournament::IsFriend(CObject* pOperator,CObject* pTarget)
{
	if( m_BattleInfo.BattleState != eBATTLE_STATE_FIGHT )
		return TRUE;
	if( pOperator->GetObjectKind() != eObjectKind_Player ||
		pTarget->GetObjectKind() != eObjectKind_Player )
		return FALSE;
	if( pOperator->GetBattleID() != pTarget->GetBattleID() )
		return FALSE;

#ifndef _MAPSERVER_
	if( pOperator->GetBattleTeam() == pTarget->GetBattleTeam() )
		return TRUE;	
#else
	if( m_TeamArray[0]->IsTeamMember( pOperator ) )
		return !m_TeamArray[1]->IsTeamMember( pTarget );
	else if( m_TeamArray[1]->IsTeamMember( pOperator ) )
		return !m_TeamArray[0]->IsTeamMember( pTarget );
#endif

	return TRUE;
}


void CBattle_GTournament::ReturnToMap( DWORD playerID )
{
	m_Observer.ReturnToMap( playerID );
}


#ifdef _MAPSERVER_
BOOL CBattle_GTournament::AddObjectToBattle(CObject* pObject)
{
	BOOL rt = FALSE;
	int n = 0;

	if( m_Observer.IsTeamMember( pObject ) )
		goto AddObserver;
	
	for( n=0; n<eBattleTeam_Max; ++n )
	{
		if( m_TeamArray[n]->IsAddableTeamMember( pObject ) )
		{
			pObject->SetBattle( GetBattleID(), BYTE( n ) );
			pObject->SetGridID( m_BattleInfo.BattleID );
			
			rt = m_TeamArray[n]->AddTeamMember(pObject);
			ASSERT(rt);
			
			OnTeamMemberAdd(n,pObject->GetID(),pObject->GetObjectName());

			if(pObject->GetObjectKind() == eObjectKind_Player && !rt)
				SendBattleInfo((CPlayer*)pObject);
			else if(pObject->GetObjectKind() == eObjectKind_Player && rt)
			{
				CPlayer* pPlayer = (CPlayer*)pObject;
				SEND_BATTLESCORE_INFO msg;
				SetProtocol( &msg, MP_GTOURNAMENT, MP_GTOURNAMENT_BATTLEJOIN_ACK );
				ZeroMemory(msg.Players, sizeof(msg.Players));
				msg.Score[0] = GetKillPoint(eBattleTeam1);
				msg.Score[1] = GetKillPoint(eBattleTeam2);
				CGuild* pGuild = GUILDMGR->GetGuild( GetBattleTeamGuildIdx( eBattleTeam1 ) );
				if( pGuild )
				{
					strcpy( msg.GuildName[0], pGuild->GetGuildName() );

					int nCnt = 0;
					GTInfo* pInfo = GTMGR->GetGTEntryInfo(pGuild->GetIdx());
					if(pInfo)
					{
						GTPlayerInfo* pPlayerInfo = NULL;
						pInfo->PlayerList.SetPositionHead();
						while((pPlayerInfo = pInfo->PlayerList.GetData())!= NULL)
						{
							msg.Players[eBattleTeam1][nCnt] = pPlayerInfo->dwPlayerID;
							msg.PlayerScore[eBattleTeam1][nCnt] = pPlayerInfo->dwScore;
							nCnt++;
						}
					}
				}
				pGuild = GUILDMGR->GetGuild( GetBattleTeamGuildIdx( eBattleTeam2 ) );
				if( pGuild )
				{
					strcpy( msg.GuildName[1], pGuild->GetGuildName() );

					int nCnt = 0;
					GTInfo* pInfo = GTMGR->GetGTEntryInfo(pGuild->GetIdx());
					if(pInfo)
					{
						GTPlayerInfo* pPlayerInfo = NULL;
						pInfo->PlayerList.SetPositionHead();
						while((pPlayerInfo = pInfo->PlayerList.GetData())!= NULL)
						{
							msg.Players[eBattleTeam2][nCnt] = pPlayerInfo->dwPlayerID;
							msg.PlayerScore[eBattleTeam2][nCnt] = pPlayerInfo->dwScore;
							nCnt++;
						}
					}
				}
				msg.FightTime = GetFightTime();
				msg.EntranceTime = GetEntranceTime();
				msg.State = GTMGR->GetTournamentState();
				msg.Team = 0;	
				msg.GoalScore = GTMGR->GetGoal();

				pPlayer->SendMsg( &msg, sizeof(msg) );
			}
			
			return TRUE;
		}
	}

AddObserver:
	//// Observer
	//pObject->SetBattle( GetBattleID(), 2 );
	//pObject->SetGridID( m_BattleInfo.BattleID );
	//rt = m_Observer.AddTeamMember( pObject );
	//
	//OnTeamMemberAdd( 2, pObject->GetID(), pObject->GetObjectName() );

	//if(pObject->GetObjectKind() == eObjectKind_Player && rt)
	//	SendBattleInfo((CPlayer*)pObject);
	AddObserverToBattle(pObject);

	return TRUE;
}
BOOL CBattle_GTournament::AddObserverToBattle(CObject* pObject)
{
	BOOL rt = FALSE;
	
	// Observer
	pObject->SetBattle( GetBattleID(), 2 );
	pObject->SetGridID( m_BattleInfo.BattleID );
	rt = m_Observer.AddTeamMember( pObject );
	
	OnTeamMemberAdd( 2, pObject->GetID(), pObject->GetObjectName() );

	if(pObject->GetObjectKind() == eObjectKind_Player && !rt)
	{
		// 090217 LUJ, 목적에 맞게 함수 이름 변경
		((CPlayer*)pObject)->SetHideLevel( 1 );
		SendBattleInfo((CPlayer*)pObject);
	}
	else if(pObject->GetObjectKind() == eObjectKind_Player && rt)
	{
		CPlayer* pPlayer = (CPlayer*)pObject;
		SEND_BATTLESCORE_INFO msg;
		SetProtocol( &msg, MP_GTOURNAMENT, MP_GTOURNAMENT_BATTLEJOIN_ACK );
		ZeroMemory(msg.Players, sizeof(msg.Players));
		msg.Score[0] = GetKillPoint(eBattleTeam1);
		msg.Score[1] = GetKillPoint(eBattleTeam2);
		CGuild* pGuild = GUILDMGR->GetGuild( GetBattleTeamGuildIdx( eBattleTeam1 ) );
		if( pGuild )
		{
			strcpy( msg.GuildName[0], pGuild->GetGuildName() );

			int nCnt = 0;
			GTInfo* pInfo = GTMGR->GetGTEntryInfo(pGuild->GetIdx());
			if(pInfo)
			{
				GTPlayerInfo* pPlayerInfo = NULL;
				pInfo->PlayerList.SetPositionHead();
				while((pPlayerInfo = pInfo->PlayerList.GetData())!= NULL)
				{
					msg.Players[eBattleTeam1][nCnt] = pPlayerInfo->dwPlayerID;
					msg.PlayerScore[eBattleTeam1][nCnt] = pPlayerInfo->dwScore;
					nCnt++;
				}
			}
		}
		pGuild = GUILDMGR->GetGuild( GetBattleTeamGuildIdx( eBattleTeam2 ) );
		if( pGuild )
		{
			strcpy( msg.GuildName[1], pGuild->GetGuildName() );

			int nCnt = 0;
			GTInfo* pInfo = GTMGR->GetGTEntryInfo(pGuild->GetIdx());
			if(pInfo)
			{
				GTPlayerInfo* pPlayerInfo = NULL;
				pInfo->PlayerList.SetPositionHead();
				while((pPlayerInfo = pInfo->PlayerList.GetData())!= NULL)
				{
					msg.Players[eBattleTeam2][nCnt] = pPlayerInfo->dwPlayerID;
					msg.PlayerScore[eBattleTeam2][nCnt] = pPlayerInfo->dwScore;
					nCnt++;
				}
			}
		}
		msg.FightTime = GetFightTime();
		msg.EntranceTime = GetEntranceTime();
		msg.State = GTMGR->GetTournamentState();
		msg.Team = 0;
		msg.GoalScore = GTMGR->GetGoal();

		pPlayer->SendMsg( &msg, sizeof(msg) );
	}

	return TRUE;
}
BOOL CBattle_GTournament::DeleteObjectFromBattle(CObject* pObject)
{
	int Team = pObject->GetBattleTeam();
	if( Team == 2 )
	{
		m_Observer.DeleteTeamMember( pObject );
	}
	else if( Team < 2 )
	{
		ToEachTeam(pTeam)
			pTeam->DeleteTeamMember( pObject );
		EndToEachTeam
		
		MSG_NAME_DWORD2 msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_TEAMMEMBER_OUT;
		msg.dwData1 = Team;
		msg.dwData2 = pObject->GetID();
		SafeStrCpy(msg.Name, pObject->GetObjectName(), sizeof(msg.Name));
		ToEachTeam(pTeam)
			pTeam->SendTeamMsg( &msg, sizeof(msg) );
		EndToEachTeam
			m_Observer.SendTeamMsg( &msg, sizeof(msg) );
	}

	return TRUE;
}
DWORD CBattle_GTournament::GetTeamMemberNum(int i)
{
	if( i == 0 || i == 1 )
	{
		if( m_TeamArray[i] )
			return ((CBattleTeam_GTournament*)m_TeamArray[i])->GetRemainMember();
		else
			return 0;
	}
	else if( i == 2 )
		return m_Observer.GetRemainMember();
	
	return 0;
}

void CBattle_GTournament::GetBattleInfo(char* pInfo,WORD* size)
{
	DWORD count = 0;
	ToEachTeam(pTeam)
		if( pTeam )
		{
			m_GTInfo.MemberInfo.TeamGuildIdx[count] = ((CBattleTeam_GTournament*)pTeam)->GetGuildIdx();
			m_GTInfo.MemberInfo.TeamLevel[count] = ((CBattleTeam_GTournament*)pTeam)->GetGuildLevel();
			m_GTInfo.MemberInfo.TeamMember[count] = (WORD)pTeam->GetMemberNum();
			m_GTInfo.MemberInfo.TeamDieMember[count] = (WORD)((CBattleTeam_GTournament*)pTeam)->GetDieCount();
			++count;
		}
	EndToEachTeam

	m_GTInfo.BattleID = m_BattleInfo.BattleID;
	m_GTInfo.BattleKind = m_BattleInfo.BattleKind;
	m_GTInfo.BattleState = m_BattleInfo.BattleState;
	m_GTInfo.BattleTime = m_BattleInfo.BattleTime;
	memcpy( pInfo, &m_GTInfo, sizeof(BATTLE_INFO_GTOURNAMENT) );
	*size = sizeof(BATTLE_INFO_GTOURNAMENT);
}
void CBattle_GTournament::ReleaseAllObject()
{
	ToEachTeam(pTeam)
		YHTPOSITION pos = pTeam->GetPositionHead();
		CObject* pObject = NULL ;
		while( ( pObject = pTeam->GetNextTeamMember( &pos ) )!= NULL)
		{
			if( pObject->GetObjectKind() == eObjectKind_Player )
				CCharMove::ReleaseMove( pObject );
			else if( pObject->GetObjectKind() & eObjectKind_Monster )
				g_pServerSystem->RemoveMonster( pObject->GetID() );
		}
	EndToEachTeam
	YHTPOSITION pos = m_Observer.GetPositionHead();
	CObject* pObject = NULL ;
	while( ( pObject = m_Observer.GetNextTeamMember( &pos ) )!= NULL)
	{
		if( pObject->GetObjectKind() == eObjectKind_Player )
			CCharMove::ReleaseMove( pObject );
		else if( pObject->GetObjectKind() & eObjectKind_Monster )
			g_pServerSystem->RemoveMonster( pObject->GetID() );
	}
}

WORD CBattle_GTournament::CalcKillPointByLevel(LEVELTYPE KillerLevel, LEVELTYPE VictimLevel)
{
	if(KillerLevel == VictimLevel)
		return 1;
	else if(KillerLevel < VictimLevel)
	{
		WORD point = 0;
		WORD LvDiff = (VictimLevel - KillerLevel) / 5;
		point = 1 + (1 * LvDiff);

		return point;
	}

	return 0;
}

WORD CBattle_GTournament::CalcKillPointByStage(BYTE KillerStage, BYTE VictimStage)
{
	WORD point = 1;

	if(KillerStage < VictimStage)
	{
		WORD wDiff = VictimStage - KillerStage;
		point = WORD( point + wDiff );

		return point;
	}

	return point;
}


#else

void CBattle_GTournament::Render()
{
	DWORD ElapsedTime = gCurTime - m_BattleInfo.BattleTime;
	switch(m_BattleInfo.BattleState )
	{
	case eBATTLE_STATE_READY:
		{
			ElapsedTime = m_GTInfo.m_dwEntranceTime;

			DWORD RemainTime = 10000 > ElapsedTime ? ElapsedTime : 0;
			m_ImageNumber.SetNumber( RemainTime / 1000 );
			if( m_ImageNumber.IsNumberChanged() )
				m_ImageNumber.SetFadeOut( 500 );

			if(RemainTime)
				m_ImageNumber.Render();
		}
		break;

	case eBATTLE_STATE_FIGHT:
		{
			if( m_pCurShowImage )
				m_pCurShowImage->RenderSprite( &m_vTitleScale, NULL, 0.0f, &m_vTitlePos, 0xffffffff );

			if(!m_bMovedBridge)
			{
				if(ElapsedTime > m_dwMoveBridgeTime)
				{
					SetMoveBridge();
					m_bMovedBridge = TRUE;
				}
			}
		}
		break;

	case eBATTLE_STATE_RESULT:
		{
			DWORD RemainTime = m_ResultTime > ElapsedTime ? m_ResultTime - ElapsedTime : 0;
			//m_ImageNumber.SetNumber( RemainTime / 1000 );
			//m_ImageNumber.Render();

			DWORD dwSec = (RemainTime / 1000) + 1;
			if(m_nLastNum!=dwSec && (0<dwSec && dwSec<6))
			{
				CHATMGR->AddMsg( CTC_GTOURNAMENT, CHATMGR->GetChatMsg(1638), dwSec);
				m_nLastNum = dwSec;
			}

			if(m_WinnerTeam == eBattleTeam_Max)
			{
				// draw
				if( m_pCurShowImage )
					m_pCurShowImage->RenderSprite( &m_vTitleScale, NULL, 0.0f, &m_vTitlePos, 0xffffffff );
			}
			else
			{
				DWORD dwAlpha = 255;
				if(m_pCurShowImage && ElapsedTime > 2500)
				{
					dwAlpha = DWORD(255 - (255 * ((float)ElapsedTime-2500)/2000.0f));

					if(dwAlpha < 30)
						m_pCurShowImage = NULL;
				}

				if( m_pCurShowImage )
					m_pCurShowImage->RenderSprite( &m_vTitleScale, NULL, 0.0f, &m_vTitlePos, 0x00ffffff | (dwAlpha<<24));
			}
		}
		break;
	}
}
void CBattle_GTournament::SetBrokenBridge()
{
	// 대기장소 만들기 추가
	CTileManager* pTileManager = MAP->GetTileManager();

	for(int i=eBattleTeam1; i<eBattleTeam_Max; ++i)
	{
		VECTOR3 vPos;
		vPos.y = 0;
		vPos.z = 11725;
		if( i == eBattleTeam1 )		vPos.x = 21900;//vPos.x = 21875;
		else						vPos.x = 5500;
		VECTOR3 vStgPos;

		float fx, fz = vPos.z;
		float fAdd = -50.0f;
		
		for( fx = vPos.x - GT_WAITZONE_BLOCKSIZE ; fx <= vPos.x + GT_WAITZONE_BLOCKSIZE + fAdd ; fx += 50.0f )//
		{
			vStgPos.x = fx;
			vStgPos.z = fz - GT_WAITZONE_BLOCKSIZE;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
			vStgPos.z = fz + GT_WAITZONE_BLOCKSIZE + fAdd;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		}
		
		fx = vPos.x;
		for( fz = vPos.z - GT_WAITZONE_BLOCKSIZE ; fz <= vPos.z + GT_WAITZONE_BLOCKSIZE + fAdd; fz += 50.0f )
		{
			vStgPos.x = fx - GT_WAITZONE_BLOCKSIZE;
			vStgPos.z = fz;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
			vStgPos.x = fx + GT_WAITZONE_BLOCKSIZE + fAdd;
			vStgPos.z = fz;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		}
		
		// 이거 안하면 빙글빙글 돈다.
		fz = vPos.z;
		for( fx = vPos.x - GT_WAITZONE_BLOCKSIZE2 ; fx <= vPos.x + GT_WAITZONE_BLOCKSIZE2 + fAdd ; fx += 50.0f )//
		{
			vStgPos.x = fx;
			vStgPos.z = fz - GT_WAITZONE_BLOCKSIZE2;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
			vStgPos.z = fz + GT_WAITZONE_BLOCKSIZE2 + fAdd;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		}
		
		fx = vPos.x;
		for( fz = vPos.z - GT_WAITZONE_BLOCKSIZE2 ; fz <= vPos.z + GT_WAITZONE_BLOCKSIZE2 + fAdd ; fz += 50.0f )
		{
			vStgPos.x = fx - GT_WAITZONE_BLOCKSIZE2;
			vStgPos.z = fz;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
			vStgPos.x = fx + GT_WAITZONE_BLOCKSIZE2 + fAdd;
			vStgPos.z = fz;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		}
	}

	VECTOR3 Pos1 = {0};
	OBJECTEFFECTDESC desc( FindEffectNum("GTMAP_Area_s.beff"), EFFECT_FLAG_ABSOLUTEPOS, &Pos1 );
	HERO->AddObjectEffect( BATTLE_GTTEAM1_EFFECTID, &desc, 1 );
}
void CBattle_GTournament::SetMoveBridge()
{
	// 대기장소 해제.
	CTileManager* pTileManager = MAP->GetTileManager();
 
	for(int i=eBattleTeam1; i<eBattleTeam_Max; ++i)
	{
		VECTOR3 vPos;
		vPos.y = 0;
		vPos.z = 11725;
		if( i == eBattleTeam1 )		vPos.x = 21900;
		else						vPos.x = 5500;
		VECTOR3 vStgPos;
		
		float fx, fz = vPos.z;
		float fAdd = -50.0f;
		
		for( fx = vPos.x - GT_WAITZONE_BLOCKSIZE ; fx <= vPos.x + GT_WAITZONE_BLOCKSIZE + fAdd ; fx += 50.0f )//
		{
			vStgPos.x = fx;
			vStgPos.z = fz - GT_WAITZONE_BLOCKSIZE;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
			vStgPos.z = fz + GT_WAITZONE_BLOCKSIZE + fAdd;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		}
		
		fx = vPos.x;
		for( fz = vPos.z - GT_WAITZONE_BLOCKSIZE ; fz <= vPos.z + GT_WAITZONE_BLOCKSIZE + fAdd; fz += 50.0f )
		{
			vStgPos.x = fx - GT_WAITZONE_BLOCKSIZE;
			vStgPos.z = fz;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
			vStgPos.x = fx + GT_WAITZONE_BLOCKSIZE + fAdd;
			vStgPos.z = fz;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		}
		
		//
		fz = vPos.z;
		for( fx = vPos.x - GT_WAITZONE_BLOCKSIZE2 ; fx <= vPos.x + GT_WAITZONE_BLOCKSIZE2 + fAdd ; fx += 50.0f )//
		{
			vStgPos.x = fx;
			vStgPos.z = fz - GT_WAITZONE_BLOCKSIZE2;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
			vStgPos.z = fz + 4250.00f + fAdd;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		}
		
		fx = vPos.x;
		for( fz = vPos.z - GT_WAITZONE_BLOCKSIZE2 ; fz <= vPos.z + GT_WAITZONE_BLOCKSIZE2 + fAdd; fz += 50.0f )
		{
			vStgPos.x = fx - GT_WAITZONE_BLOCKSIZE2;
			vStgPos.z = fz;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
			vStgPos.x = fx + GT_WAITZONE_BLOCKSIZE2 + fAdd;
			vStgPos.z = fz;
			if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
				pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		}
	}
}

BOOL CBattle_GTournament::IsObserver( CObject* pObject )
{
	if( m_Observer.IsTeamMember( pObject ) )
		return TRUE;
	
	return FALSE;
}

void CBattle_GTournament::SetGTName( CObject* pObject, DWORD NameType)
{
	if(!pObject)	return;

	CPlayer* pPlayer = (CPlayer*)pObject;
	pPlayer->SetGTName(NameType);
}

#endif