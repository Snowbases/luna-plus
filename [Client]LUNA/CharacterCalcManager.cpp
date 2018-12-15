#include "stdafx.h"
#include "CharacterCalcManager.h"
#include "GameIn.h"
#include "ObjectManager.h"
#include "ChatManager.h"
#include "GameResourceManager.h"
#include "CharacterDialog.h"
#include "ObjectStateManager.h"
#include "ObjectActionManager.h"
#include "ExitManager.h"
#include "QuestManager.h"
#include "QuickManager.h"
#include "cSkillTreeManager.h"
#include "ItemManager.h"
#include "WindowIDEnum.h"
#include "Interface/cResourceManager.h"
#include "Interface/cWindowManager.h"
#include "MixDialog.h"
#include "FamilyDialog.h"
#include "CommonCalcFunc.h"
#include "cSkillTrainingDlg.h"
#include "TutorialManager.h"
#include "PartyManager.h"
#include "MiniMapDlg.h"

GLOBALTON(CCharacterCalcManager)
CCharacterCalcManager::CCharacterCalcManager()
{
	LoadJobSkillList();
}


CCharacterCalcManager::~CCharacterCalcManager()
{}


void CCharacterCalcManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)  
	{
	case MP_CHAR_LIFE_ACK:							Char_Life_Ack( pMsg ) ;					break;
	case MP_CHAR_LIFE_NACK:							ASSERT(0);								break;
	case MP_CHAR_MANA_ACK:							Char_Mana_Ack( pMsg ) ;					break;
	case MP_CHAR_MANA_NACK:							ASSERT(0);								break;
	case MP_CHAR_EXPPOINT_ACK:						Char_Exppoint_Ack( pMsg ) ;				break;
	case MP_CHAR_EXPPOINT_NACK:						ASSERT(0);								break;		
	case MP_CHAR_MAXLIFE_NOTIFY:					Char_MaxLife_Notify( pMsg ) ;			break;
	case MP_CHAR_MAXMANA_NOTIFY:					Char_MaxMana_Notify( pMsg ) ;			break;
	case MP_CHAR_LEVEL_NOTIFY:						Char_Level_Notify( pMsg ) ;				break;
	case MP_CHAR_LEVELUPPOINT_NOTIFY:				Char_LevelupPoint_Notify( pMsg ) ;		break;
	case MP_CHAR_STR_NOTIFY:						Char_Str_Notify( pMsg ) ;				break;
	case MP_CHAR_DEX_NOTIFY:						Char_Dex_Notify( pMsg ) ;				break;
	case MP_CHAR_VIT_NOTIFY:						Char_Vit_Notify( pMsg ) ;				break;
	case MP_CHAR_WIS_NOTIFY:						Char_Wis_Notify( pMsg ) ;				break;
	// 070411 LYW --- CharacterCalcManager : Add network message of Intelligence part.
	case MP_CHAR_INT_NOTIFY :						Char_Int_Notify( pMsg ) ;				break ;
	case MP_CHAR_BADFAME_NOTIFY:					Char_BadFame_Notify( pMsg ) ;			break;
	case MP_CHAR_EXITSTART_ACK:						Char_ExitStart_Ack( pMsg ) ;			break;
	case MP_CHAR_EXITSTART_NACK:					Char_ExitStart_Nack( pMsg ) ;			break;
	case MP_CHAR_EXIT_ACK:							EXITMGR->Exit();						break;
	case MP_CHAR_EXIT_NACK:							Char_Exit_Nack( pMsg ) ;				break;
	case MP_CHAR_YOUAREDIED:						Char_YouAreDied( pMsg ) ;				break;
	case MP_CHAR_STAGE_NOTIFY :						Char_Stage_Notify( pMsg ) ;				break ;

	// desc_hseos_몬스터미터01
	// S 몬스터미터 추가 added by hseos 2007.05.23
	case MP_CHAR_MONSTERMETER_KILLMONSTER:			Char_Monstermeter_KillMonster( pMsg );	break;
	case MP_CHAR_MONSTERMETER_PLAYTIME:				Char_Monstermeter_PlayTime( pMsg );		break;
	// E 몬스터미터 추가 added by hseos 2007.05.23

	case MP_CHAR_HIDE_NOTIFY:						Char_Hide_Notify( pMsg );				break;
	case MP_CHAR_DETECT_NOTIFY:						Char_Detect_Notify( pMsg );				break;
		// 090507 LUJ, 요청/대상 오브젝트 모두의 HP를 갱신하도록 수정
	case MP_CHAR_LIFE_GET_ACK:
		{
			const MSG_DWORD3* const message = ( MSG_DWORD3* )pMsg;
			const DWORD requestObjectLife = message->dwData1;
			const DWORD targetObjectIndex = message->dwData2;
			const DWORD targetObjectLife = message->dwData3;
			CObject* const targetObject = OBJECTMGR->GetObject( targetObjectIndex );

			if( targetObject )
			{
				targetObject->SetLife( targetObjectLife );
			}

			HERO->SetLife( requestObjectLife );
			break;
		}
		// 100223 ShinJS --- 마나 요청추가
	case MP_CHAR_MANA_GET_ACK:
		{
			const MSG_DWORD* const pmsg = (MSG_DWORD*)pMsg;
			CObject* pObject = OBJECTMGR->GetObject( pmsg->dwObjectID );
			if( pObject )
				pObject->SetMana( pmsg->dwData );
		}
		break;

	case MP_CHAR_LIFE_BROADCAST:
		{
			const MSG_INT* const msg = (MSG_INT*)pMsg;
			CPlayer*	pPlayer	=	(CPlayer*)OBJECTMGR->GetObject( msg->dwObjectID );

			if( pPlayer )
			{
				DWORD newlife = min( pPlayer->GetMaxLife(), pPlayer->GetLife() + msg->nData );
				pPlayer->SetLife( newlife );
			}
			
		}
		break;
	}
}


void CCharacterCalcManager::Char_Life_Ack( void* pMsg )
{
	MSG_INT* msg = (MSG_INT*)pMsg;
	if(HERO)
	{
		HERO->ChangeLife(msg->nData);
	}
}


void CCharacterCalcManager::Char_Mana_Ack( void* pMsg )
{
	MSG_DWORD * msg = (MSG_DWORD *)pMsg;
	if(HERO)
	{
		// 100223 ShinJS --- 마나 데미지 추가로 인한 마나 정보를 변화량으로 수정.
		HERO->ChangeMana(msg->dwData);
	}		
}


void CCharacterCalcManager::Char_Exppoint_Ack( void* pMsg )
{
	MSG_DWORD* pmsg = ( MSG_DWORD* )pMsg ;

    // 080930 LYW --- CharacterCalcManager : 캐릭터 사망시 틩김 현상 HERO 정보가 NULL인경우가 있다.
	//HERO->SetExpPoint(pmsg->dwData) ;
	if( HERO )
	{
		HERO->SetExpPoint(pmsg->dwData) ;
	}
}


void CCharacterCalcManager::Char_MaxLife_Notify( void* pMsg )
{
	MSG_DWORD * msg = (MSG_DWORD *)pMsg;
	// Player의 MaxLife도 갱신함
	CPlayer*	pPlayer	=	(CPlayer*)OBJECTMGR->GetObject( msg->dwObjectID );
	if( pPlayer )
	{
		pPlayer->SetMaxLife( msg->dwData );
	}
}


void CCharacterCalcManager::Char_MaxMana_Notify( void* pMsg )
{
	MSG_DWORD * msg = (MSG_DWORD *)pMsg;
	if(HERO)
	{
		HERO->SetMaxMana(msg->dwData);
	}
}

void CCharacterCalcManager::Char_Level_Notify( void* pMsg )
{
	const MSG_LEVEL* const msg = (MSG_LEVEL*)pMsg;
	CPlayer* const player = (CPlayer*)OBJECTMGR->GetObject(msg->dwObjectID);

	if(0 == player)
	{
		return;
	}
	else if(eObjectKind_Player != player->GetObjectKind())
	{
		return;
	}

	const LEVELTYPE previousLevel = player->GetLevel();

	if(previousLevel < msg->Level)
	{
		EFFECTMGR->StartEffectProcess(
			eEffect_LevelUpSentence,
			player,
			0,
			0,
			0);
	}

	player->SetLevel(
		msg->Level);
	PARTYMGR->SetMemberLevel(
		player->GetID(),
		msg->Level);

	if(player->GetID() == gHeroID)
	{
		CHero* const hero = (CHero*)player;
		hero->SetMaxExpPoint(
			msg->MaxExpPoint);
		hero->SetExpPoint(
			msg->CurExpPoint);
		QUESTMGR->ProcessNpcMark();

		if(previousLevel < msg->Level)
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(822),
				hero->GetObjectName(),
				msg->Level);
		}

		GAMEIN->GetFamilyDlg()->UpdateAllInfo();
		GAMEIN->GetCharacterDialog()->RefreshInfo();
		GAMEIN->GetMixDialog()->Refresh();
		GAMEIN->GetSkillTrainingDlg();
		GAMEIN->GetMiniMapDialog()->ChangeLevelUpBTNState();

		if(cSkillTrainingDlg* const dialog = GAMEIN->GetSkillTrainingDlg())
		{
			if(dialog->IsActive())
			{
				dialog->OpenDialog();
				dialog->SetSkillInfo();
			}
		}
	}
}


void CCharacterCalcManager::Char_LevelupPoint_Notify( void* pMsg )
{
	MSG_WORD * msg = (MSG_WORD *)pMsg;
	// 레벨 셋팅 msg->dwData1
	if(HERO)
	{
		if(msg->wData != 0)
		{				
			GAMEIN->GetCharacterDialog()->SetPointLeveling(TRUE, msg->wData);		// 포인트 배분
		}
		else
 		{
 			GAMEIN->GetCharacterDialog()->SetPointLevelingHide();
 		}
	}

	// 080506 LYW --- CharacterCalcManager : 레벨업과 관련 된 tutorial을 체크하는 함수 호출.
	TUTORIALMGR->Check_LevelUp() ;
}


void CCharacterCalcManager::Char_Str_Notify( void* pMsg )
{
	MSG_DWORD * msg = (MSG_DWORD *)pMsg;
	if(HERO)
	{
		HERO->SetStrength((WORD)msg->dwData);
	}
}


void CCharacterCalcManager::Char_Dex_Notify( void* pMsg )
{
	MSG_DWORD * msg = (MSG_DWORD *)pMsg;
	if(HERO)
	{
		HERO->SetDexterity((WORD)msg->dwData);
	}
}


void CCharacterCalcManager::Char_Vit_Notify( void* pMsg )
{
	MSG_DWORD * msg = (MSG_DWORD *)pMsg;
	if(HERO)
	{
		HERO->SetVitality((WORD)msg->dwData);
	}
}


void CCharacterCalcManager::Char_Wis_Notify( void* pMsg )
{
	MSG_DWORD * msg = (MSG_DWORD *)pMsg;
	if(HERO)
	{
		HERO->SetWisdom((WORD)msg->dwData);
	}
}

// 070411 LYW --- CharacterCalcManager : Add function to setting intelligence from notify.
void CCharacterCalcManager::Char_Int_Notify( void* pMsg )
{
	MSG_DWORD * msg = (MSG_DWORD *)pMsg;
	if(HERO)
	{
		HERO->SetIntelligence((WORD)msg->dwData);
	}
}

void CCharacterCalcManager::Char_BadFame_Notify( void* pMsg )
{
	MSG_FAME * pmsg = (MSG_FAME*)pMsg;
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwObjectID );
	if(pPlayer)
	{
		pPlayer->SetBadFame(pmsg->Fame);
	}
}

void CCharacterCalcManager::Char_ExitStart_Ack( void* pMsg )
{
	EXITMGR->StartExit();		
}


void CCharacterCalcManager::Char_ExitStart_Nack( void* pMsg )
{
	MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;			
	EXITMGR->RejectExit( pmsg->bData );
}


void CCharacterCalcManager::Char_Exit_Nack( void* pMsg )
{
	MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;			
	EXITMGR->CancelExit( pmsg->bData );
}


void CCharacterCalcManager::Char_YouAreDied( void* pMsg )
{
	if( HERO->GetState() != eObjectState_Die )
	{
		ASSERT(HERO->IsDied());

		if(gCurTime - HERO->GetDiedTime() < 10000)
			return;

		OBJECTACTIONMGR->Die(HERO,HERO,FALSE,FALSE,FALSE);
	}
}

// 070412 LYW --- CharacterCalcManager : Add function to change stage of character.
void CCharacterCalcManager::Char_Stage_Notify( void* pMsg )
{
	MSG_BYTE2* pmsg = (MSG_BYTE2*)pMsg ;

	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwObjectID) ;
	if(!pPlayer) return ;

	pPlayer->SetStage( pmsg->bData1, pmsg->bData2 ) ;

	BYTE jobType = pPlayer->GetCharacterTotalInfo()->Job[0] ;

	int nMsgNum = 0 ;

	switch( pmsg->bData1 )
	{
	case 2 : nMsgNum = Notify_SecondJob( jobType, pmsg->bData2 ) ;	break ;
	case 3 : nMsgNum = Notify_ThirdJob( jobType, pmsg->bData2 ) ;	break ;
	case 4 : nMsgNum = Notify_FourthJob( jobType, pmsg->bData2 ) ;	break ;
	case 5 : nMsgNum = Notify_FifthJob( jobType, pmsg->bData2 ) ;	break ;
	case 6 : nMsgNum = Notify_SixthJob( jobType, pmsg->bData2 ) ;	break ;
	}

	char tempStr[64] = { 0, } ;
	sprintf( tempStr, CHATMGR->GetChatMsg( 1445 ), RESRCMGR->GetMsg(nMsgNum) ) ;

	CHATMGR->AddMsg( CTC_SYSMSG, tempStr ) ;

	CCharacterDialog* pWindow = (CCharacterDialog*)WINDOWMGR->GetWindowForID(CI_CHARDLG) ;
	pWindow->SetClassName( RESRCMGR->GetMsg(nMsgNum) ) ;

	AddPlayerJobSkill(HERO);
	//GAMEIN->GetFamilyDlg()->InitFamilyDialog() ;

}

// 070416 LYW --- CharacterCalcManager : Add functions to notify class update.
int CCharacterCalcManager::Notify_SecondJob( BYTE jobType, BYTE byIndex )
{
	int nMsgNum = 0 ;

	switch( jobType )
	{
	case 1 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 365 ; break ;
			case 2 : nMsgNum = 366 ; break ;
			}
		}
		break ;

	case 2 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 384 ; break ;
			case 2 : nMsgNum = 385 ; break ;
			}
		}
		break ;

	case 3 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 401 ; break ;
			case 2 : nMsgNum = 402 ; break ;
			}
		}
		break ;
	// 090930 ONS 신규종족 전직메세지 추가
	case 4 :
		{
			nMsgNum = 1133 ;
		}
		break;
	}

	return nMsgNum ;
}

int CCharacterCalcManager::Notify_ThirdJob( BYTE jobType, BYTE byIndex )
{
	int nMsgNum = 0 ;

	switch( jobType )
	{
	case 1 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 367 ; break ;
			case 2 : nMsgNum = 368 ; break ;
			case 3 : nMsgNum = 369 ; break ;
			}
		}
		break ;

	case 2 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 386 ; break ;
			case 2 : nMsgNum = 387 ; break ;
			case 3 : nMsgNum = 388 ; break ;
			}
		}
		break ;

	case 3 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 403 ; break ;
			case 2 : nMsgNum = 404 ; break ;
			case 3 : nMsgNum = 405 ; break ;
			}
		}
		break ;
	// 090930 ONS 신규종족 전직메세지 추가
	case 4 :
		{
			nMsgNum = 1134 ;
		}
		break;
	}

	return nMsgNum ;
}

int CCharacterCalcManager::Notify_FourthJob( BYTE jobType, BYTE byIndex )
{
	int nMsgNum = 0 ;

	switch( jobType )
	{
	case 1 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 370 ; break ;
			case 2 : nMsgNum = 371 ; break ;
			case 3 : nMsgNum = 372 ; break ;
			case 4 : nMsgNum = 373 ; break ;
			}
		}
		break ;

	case 2 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 389 ; break ;
			case 2 : nMsgNum = 390 ; break ;
			case 3 : nMsgNum = 391 ; break ;
			case 4 : nMsgNum = 392 ; break ;
			}
		}
		break ;

	case 3 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 406 ; break ;
			case 2 : nMsgNum = 407 ; break ;
			case 3 : nMsgNum = 408 ; break ;
			case 4 : nMsgNum = 409 ; break ;
			}
		}
		break ;
	// 090930 ONS 신규종족 전직메세지 추가
	case 4 :
		{
			nMsgNum = 1135 ;
		}
		break;
	}

	return nMsgNum ;
}

int CCharacterCalcManager::Notify_FifthJob( BYTE jobType, BYTE byIndex )
{
	int nMsgNum = 0 ;

	switch( jobType )
	{
	case 1 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 374 ; break ;
			case 2 : nMsgNum = 375 ; break ;
			case 3 : nMsgNum = 376 ; break ;
			case 4 : nMsgNum = 377 ; break ;
			case 5 : nMsgNum = 378 ; break ;
			case 6 : nMsgNum = 379 ; break ;
			}
		}
		break ;

	case 2 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 393 ; break ;
			case 2 : nMsgNum = 394 ; break ;
			case 3 : nMsgNum = 395 ; break ;
			case 4 : nMsgNum = 396 ; break ;
			// 080709 LYW --- CharacterCalcManager : 아크레인저 직업 추가.
			case 5 : nMsgNum = 1004 ; break ;
			}
		}
		break ;

	case 3 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 410 ; break ;
			case 2 : nMsgNum = 411 ; break ;
			case 3 : nMsgNum = 412 ; break ;
			case 4 : nMsgNum = 413 ; break ;
			case 5 : nMsgNum = 414 ; break ;
			}
		}
		break ;
	// 090930 ONS 신규종족 전직메세지 추가
	case 4 :
		{
			nMsgNum = 1136 ;
		}
		break;
	}

	return nMsgNum ;
}

int CCharacterCalcManager::Notify_SixthJob( BYTE jobType, BYTE byIndex )
{
	int nMsgNum = 0 ;

	switch( jobType )
	{
	case 1 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 380 ; break ;
			case 2 : nMsgNum = 381 ; break ;
			case 3 : nMsgNum = 382 ; break ;
			}
		}
		break ;

	case 2 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 397 ; break ;
			case 2 : nMsgNum = 398 ; break ;
			case 3 : nMsgNum = 399 ; break ;
			}
		}
		break ;

	case 3 :
		{
			switch( byIndex )
			{
			case 1 : nMsgNum = 415 ; break ;
			case 2 : nMsgNum = 416 ; break ;
			case 3 : nMsgNum = 417 ; break ;
			}
		}
		break ;
	}

	return nMsgNum ;
}

// desc_hseos_몬스터미터01
// S 몬스터미터 추가 added by hseos 2007.05.23	2008.01.11:CPlayer->CHero
void CCharacterCalcManager::Char_Monstermeter_KillMonster(void* pMsg)
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	if( gHeroID == pmsg->dwObjectID )
	{
		HERO->SetMonstermeterKillMonNum(pmsg->dwData1, pmsg->dwData2);
	}
}

void CCharacterCalcManager::Char_Monstermeter_PlayTime(void* pMsg)
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	if( gHeroID == pmsg->dwObjectID )
	{
		HERO->SetMonstermeterPlaytime(pmsg->dwData1, pmsg->dwData2);
	}
}
// E 몬스터미터 추가 added by hseos 2007.05.23	2008.01.11:CPlayer->CHero


void CCharacterCalcManager::Initialize( CHero* hero )
{
	PlayerStat& base_stat = hero->GetItemBaseStats();
	PlayerStat& option_stat = hero->GetItemOptionStats();

	// 초기화
	{
		ZeroMemory( &base_stat, sizeof( PlayerStat ) );
		ZeroMemory( &option_stat, sizeof( PlayerStat ) );

		SKILLTREEMGR->ResetSetSkill();

		{
			CHero::SetItemLevel& setLevel = hero->GetSetItemLevel();

			setLevel.clear();
		}

		// 080313 LUJ, 세트 아이템 능력 초기화
		PlayerStat& setItemStat = hero->GetSetItemStats();

		ZeroMemory( &setItemStat, sizeof( setItemStat ) );
	}	

	typedef std::map< const SetScript*, DWORD > SetItemSize;
	SetItemSize									setItemSize;

	for(POSTYPE part = TP_WEAR_START ; part < TP_WEAR_END ; ++part )
	{
		const ITEMBASE* item = ITEMMGR->GetItemInfoAbsIn( hero, part);

		if(	0 == item || 
			0 == item->dwDBIdx )
		{
			continue;
		}

		const ITEM_INFO * info = ITEMMGR->GetItemInfo(item->wIconIdx);

		if( ! info )
		{
			continue;
		}
		// 091211 LUJ, 이도류 마스터리가 없는 경우에 왼손에 무기가 있을 경우 적용하지 않는다
		//			이것은 버그 플레이이며, 무기를 교환하거나 새로 장착할 수는 없으므로
		//			초기화시에만 처리한다
		else if(TP_WEAR_START + eWearedItem_Shield == part)
		{
			const BOOL isNoShield = (info->Part3DType != ePartType_Shield);
			const BOOL isUnableDualWield = hero->GetHeroTotalInfo()->bUsingTwoBlade;

			if(isNoShield &&
				isUnableDualWield)
			{
				continue;
			}
		}

		// 세트 아이템 능력 추가를 위해, 같은 종류의 세트 스크립트를 가진 아이템 개수도 조사해두자
		{
			const SetScript* script = GAMERESRCMNGR->GetSetScript( item->wIconIdx );

			if( script )
			{
				++setItemSize[ script ];

				// 081231 LUJ, 아이템의 추가 속성을 적용한다
				{
					const SetScript::Element& itemElement = GAMERESRCMNGR->GetItemElement( info->ItemIdx );

					for(	SetScript::Element::Skill::const_iterator skill_it = itemElement.mSkill.begin();
							itemElement.mSkill.end() != skill_it;
							++skill_it )
					{
						const DWORD	skillIndex	= skill_it->first;
						const BYTE	skillLevel	= skill_it->second;

						SKILLTREEMGR->AddSetSkill( skillIndex, skillLevel );
						QUICKMGR->RefreshSkillItem( skillIndex );
					}

					SetPlayerStat(
						hero->GetSetItemStats(),
						itemElement.mStat,
						SetValueTypeAdd );
				}
			}
		}		

		AddItemBaseStat( *info, ITEMMGR->GetOption( *item ), base_stat );
		AddItemOptionStat( *info, ITEMMGR->GetOption( *item ), option_stat );
	}

	// 세트 능력 적용. 낮은 단계부터 누적시키며 적용해야 한다
	for(
		SetItemSize::const_iterator it = setItemSize.begin();
		setItemSize.end() != it;
		++it )
	{
		const SetScript*	script	= it->first;
		const DWORD			size	= it->second;

		for( DWORD i = 1; i <= size; ++i )
		{
			AddSetItemStats( hero, *script, i );
		}
	}

	CalcCharStats( hero );
}

void CCharacterCalcManager::CalcCharStats( CHero* hero )
{
	PlayerStat& char_stats = hero->GetCharStats();
	ZeroMemory( &char_stats, sizeof( char_stats ) );

	HERO_TOTALINFO pHeroInfo;
	hero->GetHeroTotalInfo(&pHeroInfo);

	// 아이템 장착한거에 따른 캐릭터 수치 셋팅
	char_stats.mDexterity.mPlus		= float( pHeroInfo.Dex );
	char_stats.mStrength.mPlus		= float( pHeroInfo.Str );

	// 070411 LYW --- StatsCalcManager : Setting other stats.
	char_stats.mVitality.mPlus		= float( pHeroInfo.Vit );
	char_stats.mIntelligence.mPlus	= float( pHeroInfo.Int );
	char_stats.mWisdom.mPlus		= float( pHeroInfo.Wis );

	// 070412 LYW --- StatsCalcManager : Delete this part.
	/*
	char_stats.PhysicalDefense = (WORD)(((hero.GetVitality())/1.5)*0.6);
	char_stats.Critical = (hero.GetStrength()) / 4;
	char_stats.Decisive = (hero.GetWisdom()) / 4;
	*/
}


void CCharacterCalcManager::AddSetItemStats( CHero* hero, const SetScript& script, int setItemSize )
{
	// 세트 아이템은 다음과 같이 적용된다. 세트로 구성되는 아이템 그룹들이 있고, 장비한 세트 아이템 개수에 따라 능력이 차례로 부여된다.

	// 세트 스크립트로 부여된 레벨을 가져와서 그 다음부터 조사
	// 세트 스크립트의 능력이 요구하는 세트 아이템 개수 조건을 충족하면 능력 부여

	typedef CPlayer::SetItemLevel SetItemLevel;

	CPlayer::SetItemLevel& setLevel = hero->GetSetItemLevel();

	int& level = setLevel[ &script ];

	ASSERT( size_t( level ) < script.mAbility.size() );
	SetScript::Ability::const_iterator it = script.mAbility.begin();
	std::advance( it, level );

	for(	;
		script.mAbility.end() != it;
		++it, ++level )
	{
		const int size = it->first;

		if( size > setItemSize )
		{
			break;
		}

		// 능력을 부여하자
		{
			const SetScript::Element& element = it->second;

			AddStat( element.mStat, hero->GetSetItemStats() );
			
			// 주의! 맵 서버에서는 액티브 스킬을 추가할 필요 없음
			for(
				SetScript::Element::Skill::const_iterator inner = element.mSkill.begin();
				element.mSkill.end() != inner;
				++inner )
			{
				const DWORD index	= inner->first;
				const BYTE	level	= inner->second;

				SKILLTREEMGR->AddSetSkill( index, level );
				QUICKMGR->RefreshSkillItem( index );
			}
		}
	}

	if( 0 == level )
	{
		setLevel.erase( &script );
	}
}

// 081022 KTH -- Load File Job Skill List
BOOL CCharacterCalcManager::LoadJobSkillList()
{
	CMHFile fp;

	if( !fp.Init( "./System/Resource/AddJobSkillList.bin", "rb" ) )
	{
		MessageBox(NULL, "Loading failed!! ./System/Resource/AddJobSkillList.bin", 0, 0);
		return FALSE;
	}

	while( ! fp.IsEOF() )
	{
		char szLine[256];
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//")) continue;	

		JobSkillInfo stJobSkillInfo;
		memset(&stJobSkillInfo, NULL, sizeof(JobSkillInfo));

		stJobSkillInfo.wClassIndex = fp.GetWord();
		stJobSkillInfo.dwSkillIndex = fp.GetDword();
		stJobSkillInfo.byPlusLevel = fp.GetByte();

		m_JobSkillList.push_back(stJobSkillInfo);
	}

	return TRUE;
}

// 081023 KTH --
void CCharacterCalcManager::AddPlayerJobSkill(CHero* pHero)
{
	WORD wJobIdx ;

	BYTE byJobType ;

	CPlayer* pPlayer = (CPlayer*)pHero;
	CHARACTER_TOTALINFO pTotalInfo;
	pPlayer->GetCharacterTotalInfo( &pTotalInfo );
	WORD CurGrade = pTotalInfo.JobGrade;

	SKILLTREEMGR->ClearJobSkill();

	for( WORD grade = CurGrade; grade > 0; grade-- )
	{
		byJobType = 0 ;

		// 직업 계열의 유효성 체크.
		if( grade == 1 ) byJobType = 1 ;
		else byJobType = HERO->GetCharacterTotalInfo()->Job[ grade - 1 ] ;

		if(!byJobType) return;

		// 현제 레벨에 맞는 직업 인덱스를 받는다.
		wJobIdx = 0 ;											
		wJobIdx = ( HERO->GetCharacterTotalInfo()->Job[ 0 ] * 1000 ) +
				( ( HERO->GetCharacterTotalInfo()->Race + 1 ) * 100 ) + 
				( ( grade ) * 10 ) + 
				byJobType ;

		std::list<JobSkillInfo>::iterator BeginItr = m_JobSkillList.begin();

		for( ; BeginItr != m_JobSkillList.end(); BeginItr++ )
		{
			if( BeginItr->wClassIndex == wJobIdx )
			{
				SKILLTREEMGR->AddJobSkill(BeginItr->dwSkillIndex, BeginItr->byPlusLevel);
				//pPlayer->AddSetSkill(BeginItr->dwSkillIndex, BeginItr->byPlusLevel);
			}
		}
	}
}

void CCharacterCalcManager::RemoveSetItemStats( CHero* hero, const SetScript& script, int setItemSize )
{
	// 해당 플레이어가 가진 세트 스크립트로 부여된 레벨을 가져온다.
	// 있으면 거기부터 시작해서 아래로 조사. 없으면 그냥 끝
	// 캐릭터 세트 능력을 가져와 변경
	// 수치가 변화했으면 수치 재계산 요청

	typedef CPlayer::SetItemLevel SetItemLevel;

	CPlayer::SetItemLevel& setLevel = hero->GetSetItemLevel();

	// 해당 스크립트로 저장된 레벨이 없다는 건 적용된 세트 능력이 없다는 뜻
	if( setLevel.end() == setLevel.find( &script ) )
	{
		return;
	}

	int& level = setLevel[ &script ];

	ASSERT( 0 < level );
	ASSERT( size_t( level ) - 1 < script.mAbility.size() );
	SetScript::Ability::const_iterator it = script.mAbility.begin();
	std::advance( it, level - 1 );

	for( ;; )	
	{
		const int size = it->first;

		if( size <= setItemSize )
		{
			break;
		}

		// 능력을 제거하자
		{
			const SetScript::Element& element = it->second;

			RemoveStat( element.mStat, hero->GetSetItemStats() );

			for(SetScript::Element::Skill::const_iterator inner = element.mSkill.begin();
				element.mSkill.end() != inner;
				++inner )
			{
				SKILLTREEMGR->RemoveSetSkill(
					inner->first,
					inner->second);
				QUICKMGR->RefreshSkillItem(
					inner->first);
			}
		}

		--level;

		if( script.mAbility.begin() == it-- )
		{
			break;
		}
	}

	if( 0 == level )
	{
		setLevel.erase( &script );
	}
}

void CCharacterCalcManager::Char_Hide_Notify( void* pMsg )
{
	MSG_WORD* pmsg = ( MSG_WORD* )pMsg;

	OBJECTMGR->SetHide( pmsg->dwObjectID, pmsg->wData );
}

void CCharacterCalcManager::Char_Detect_Notify( void* pMsg )
{
	MSG_WORD* pmsg = ( MSG_WORD* )pMsg;

	OBJECTMGR->SetDetect( pmsg->dwObjectID, pmsg->wData );
}


void CCharacterCalcManager::AddItem( CHero* hero, const ITEMBASE& item )
{
	PlayerStat& base_stat = hero->GetItemBaseStats();
	PlayerStat& option_stat = hero->GetItemOptionStats();

	const ITEM_INFO * info = ITEMMGR->GetItemInfo( item.wIconIdx );

	if( ! info )
	{
		return;
	}

	const ITEM_OPTION& option = ITEMMGR->GetOption( item );
	AddItemBaseStat( *info, option, base_stat );
	AddItemOptionStat( *info, option, option_stat );

	// 100226 ShinJS --- 인챈트 레벨 정보 추가
	switch( info->EquipType )
	{
	case eEquipType_Weapon:
		{
			// 100831 ShinJS 왼손, 오른손 장비를 확인
			const ITEMBASE* weaponItem = ITEMMGR->GetItemInfoAbsIn( hero, TP_WEAR_START + eWearedItem_Weapon );
			const ITEMBASE* shieldItem = ITEMMGR->GetItemInfoAbsIn( hero, TP_WEAR_START + eWearedItem_Shield );
			
			const ITEM_INFO* weaponItemInfo	= ( weaponItem ? ITEMMGR->GetItemInfo( weaponItem->wIconIdx ) : 0 );
			const ITEM_INFO* shieldItemInfo	= ( shieldItem ? ITEMMGR->GetItemInfo( shieldItem->wIconIdx ) : 0 );

			DWORD weaponEnchantLevel = 0;

			if( weaponItemInfo != NULL )
			{
				const ITEM_OPTION& weaponItemOption = ITEMMGR->GetOption( *weaponItem );
				weaponEnchantLevel = weaponItemOption.mEnchant.mLevel;
				
			}

			// 왼손 장비가 무기인 경우
			if( shieldItemInfo != NULL && shieldItemInfo->EquipType == eEquipType_Weapon )
			{
				const ITEM_OPTION& shieldItemOption = ITEMMGR->GetOption( *shieldItem );
				weaponEnchantLevel = (DWORD)max((weaponEnchantLevel + shieldItemOption.mEnchant.mLevel) / 2.f, 0);
			}

			hero->SetWeaponEnchantLevel( weaponEnchantLevel );
		}
		break;
	case eEquipType_Armor:
		{
			switch( info->EquipSlot )
			{
			case eWearedItem_Dress:
			case eWearedItem_Hat:
			case eWearedItem_Glove:
			case eWearedItem_Shoes:
			case eWearedItem_Belt:
			case eWearedItem_Band:
				{
					hero->SetPhysicDefenseEnchantLevel( max( hero->GetPhysicDefenseEnchantLevel() + option.mEnchant.mLevel, 0 ) );
				}
				break;
			}
		}
		break;
	case eEquipType_Accessary:
		{
			switch( info->EquipSlot )
			{
			case eWearedItem_Ring1:
			case eWearedItem_Ring2:
			case eWearedItem_Necklace:
			case eWearedItem_Earring1:
			case eWearedItem_Earring2:
				{
					hero->SetMagicDefenseEnchantLevel( max( hero->GetMagicDefenseEnchantLevel() + option.mEnchant.mLevel, 0 ) );
				}
				break;
			}
		}
		break;
	}

	// 세트 아이템 계산, 웅주 070613
	{
		const SetScript* setScript = GAMERESRCMNGR->GetSetScript( item.wIconIdx );

		if( setScript )
		{
			AddSetItemStats( hero, *setScript, ITEMMGR->GetSetItemSize( hero, setScript ) );

			// 081231 LUJ, 아이템의 추가 속성을 적용한다
			{
				const SetScript::Element& itemElement = GAMERESRCMNGR->GetItemElement( info->ItemIdx );

				for(	SetScript::Element::Skill::const_iterator skill_it = itemElement.mSkill.begin();
						itemElement.mSkill.end() != skill_it;
						++skill_it )
				{
					const DWORD	skillIndex	= skill_it->first;
					const BYTE	skillLevel	= skill_it->second;

					SKILLTREEMGR->AddSetSkill( skillIndex, skillLevel );
					QUICKMGR->RefreshSkillItem( skillIndex );
				}

				SetPlayerStat(
					hero->GetSetItemStats(),
					itemElement.mStat,
					SetValueTypeAdd );
			}
		}
	}

	// TODO : 아이템 장착
	CalcCharStats( hero );
}


void CCharacterCalcManager::RemoveItem( CHero* hero, const ITEMBASE& item )
{
	const ITEM_INFO * info = ITEMMGR->GetItemInfo( item.wIconIdx );

	if( ! info )
	{
		return;
	}

	PlayerStat& base_stat = hero->GetItemBaseStats();
	PlayerStat& option_stat = hero->GetItemOptionStats();

	const ITEM_OPTION& option = ITEMMGR->GetOption( item );
	RemoveItemBaseStat( *info, option, base_stat );
	RemoveItemOptionStat( *info, option, option_stat );

	// 100226 ShinJS --- 인챈트 레벨 정보 추가
	switch( info->EquipType )
	{
	case eEquipType_Weapon:
		{
			// 100831 ShinJS 왼손, 오른손 장비를 확인
			const ITEMBASE* weaponItem = ITEMMGR->GetItemInfoAbsIn( hero, TP_WEAR_START + eWearedItem_Weapon );
			const ITEMBASE* shieldItem = ITEMMGR->GetItemInfoAbsIn( hero, TP_WEAR_START + eWearedItem_Shield );
			
			const ITEM_INFO* weaponItemInfo	= ( weaponItem ? ITEMMGR->GetItemInfo( weaponItem->wIconIdx ) : 0 );
			const ITEM_INFO* shieldItemInfo	= ( shieldItem ? ITEMMGR->GetItemInfo( shieldItem->wIconIdx ) : 0 );

			DWORD weaponEnchantLevel = 0;

			if( weaponItemInfo != NULL )
			{
				const ITEM_OPTION& weaponItemOption = ITEMMGR->GetOption( *weaponItem );
				weaponEnchantLevel = weaponItemOption.mEnchant.mLevel;
				
			}

			// 왼손 장비가 무기인 경우
			if( shieldItemInfo != NULL && shieldItemInfo->EquipType == eEquipType_Weapon )
			{
				const ITEM_OPTION& shieldItemOption = ITEMMGR->GetOption( *shieldItem );
				weaponEnchantLevel = (DWORD)max((weaponEnchantLevel + shieldItemOption.mEnchant.mLevel) / 2.f, 0);
			}

			hero->SetWeaponEnchantLevel( weaponEnchantLevel );
		}
		break;
	case eEquipType_Armor:
		{
			switch( info->EquipSlot )
			{
			case eWearedItem_Dress:
			case eWearedItem_Hat:
			case eWearedItem_Glove:
			case eWearedItem_Shoes:
			case eWearedItem_Belt:
			case eWearedItem_Band:
				{
					hero->SetPhysicDefenseEnchantLevel( max( hero->GetPhysicDefenseEnchantLevel() - option.mEnchant.mLevel, 0 ) );
				}
				break;
			}
		}
		break;
	case eEquipType_Accessary:
		{
			switch( info->EquipSlot )
			{
			case eWearedItem_Ring1:
			case eWearedItem_Ring2:
			case eWearedItem_Necklace:
			case eWearedItem_Earring1:
			case eWearedItem_Earring2:
				{
					hero->SetMagicDefenseEnchantLevel( max( hero->GetMagicDefenseEnchantLevel() - option.mEnchant.mLevel, 0 ) );
				}
				break;
			}
		}
		break;
	}

	// 세트 아이템 계산, 웅주 070613
	{
		const SetScript* setScript = GAMERESRCMNGR->GetSetScript( item.wIconIdx );

		if( setScript )
		{
			RemoveSetItemStats( hero, *setScript, ITEMMGR->GetSetItemSize( hero, setScript ) );

			// 081231 LUJ, 아이템의 추가 속성을 적용한다
			{
				const SetScript::Element& itemElement = GAMERESRCMNGR->GetItemElement( info->ItemIdx );

				for(	SetScript::Element::Skill::const_iterator skill_it = itemElement.mSkill.begin();
						itemElement.mSkill.end() != skill_it;
						++skill_it )
				{
					SKILLTREEMGR->RemoveSetSkill(
						skill_it->first,
						skill_it->second);
					QUICKMGR->RefreshSkillItem(
						skill_it->first);
				}

				SetPlayerStat(
					hero->GetSetItemStats(),
					itemElement.mStat,
					SetValueTypeRemove );
			}
		}
	}

	// TODO : 아이템 해제
	CalcCharStats( hero );
}


void CCharacterCalcManager::AddStat(const ITEM_INFO& info, PlayerStat& stat )
{
	stat.mPhysicAttack.mPlus	+= float( info.PhysicAttack );
	stat.mMagicAttack.mPlus		+= float( info.MagicAttack );
	stat.mPhysicDefense.mPlus	+= float( info.PhysicDefense );
	stat.mMagicDefense.mPlus	+= float( info.MagicDefense );
	stat.mStrength.mPlus		+= float( info.ImprovementStr );
	stat.mDexterity.mPlus		+= float( info.ImprovementDex );
	stat.mVitality.mPlus 		+= float( info.ImprovementVit );
	stat.mWisdom.mPlus 			+= float( info.ImprovementWis );
	stat.mIntelligence.mPlus	+= float( info.ImprovementInt );
	stat.mLife.mPlus 			+= float( info.ImprovementLife );
	stat.mMana.mPlus 			+= float( info.ImprovementMana );
}


// 080319 LUJ, 추가 인챈트 수치를 부여한다
// 080320 LUJ, 추가 인챈트 수치를 함수로 가져옴
void CCharacterCalcManager::AddStat( const ITEM_INFO& info,  const ITEM_OPTION& option, PlayerStat& stat )
{
	const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
	const ITEM_OPTION::Mix&			mix			= option.mMix;

	stat.mPhysicAttack.mPlus	+= float( reinforce.mPhysicAttack );
	stat.mPhysicDefense.mPlus	+= float( reinforce.mPhysicDefence );
	stat.mMagicAttack.mPlus		+= float( reinforce.mMagicAttack );
	stat.mMagicDefense.mPlus	+= float( reinforce.mMagicDefence );
	stat.mStrength.mPlus		+= float( reinforce.mStrength		+ mix.mStrength );
	stat.mDexterity.mPlus		+= float( reinforce.mDexterity		+ mix.mDexterity );
	stat.mVitality.mPlus		+= float( reinforce.mVitality		+ mix.mVitality );
	stat.mWisdom.mPlus			+= float( reinforce.mWisdom			+ mix.mWisdom );
	stat.mIntelligence.mPlus	+= float( reinforce.mIntelligence	+ mix.mIntelligence );
	stat.mLife.mPlus			+= float( reinforce.mLife );
	stat.mMana.mPlus			+= float( reinforce.mMana );
	stat.mRecoveryLife.mPlus	+= float( reinforce.mLifeRecovery );
	stat.mRecoveryMana.mPlus	+= float( reinforce.mManaRecovery );
	stat.mCriticalRate.mPlus	+= float( reinforce.mCriticalRate );
	stat.mCriticalDamage.mPlus	+= float( reinforce.mCriticalDamage );
	stat.mAccuracy.mPlus		+= float( reinforce.mAccuracy );
	stat.mMoveSpeed.mPlus		+= float( reinforce.mMoveSpeed );
	stat.mEvade.mPlus			+= float( reinforce.mEvade );

	const ITEM_OPTION::Drop& drop = option.mDrop;

	for( DWORD i = 0; i < sizeof( drop.mValue ) / sizeof( *drop.mValue ); ++i )
	{
		const ITEM_OPTION::Drop::Value& value = drop.mValue[ i ];

		switch( value.mKey )
		{
		case ITEM_OPTION::Drop::KeyPlusStrength:
			{
				stat.mStrength.mPlus = Round( stat.mStrength.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusIntelligence:
			{
				stat.mIntelligence.mPlus = Round( stat.mIntelligence.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusDexterity:
			{
				stat.mDexterity.mPlus = Round( stat.mDexterity.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusWisdom:
			{
				stat.mWisdom.mPlus = Round( stat.mWisdom.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusVitality:
			{
				stat.mVitality.mPlus = Round( stat.mVitality.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
			{
				stat.mPhysicAttack.mPlus = Round( stat.mPhysicAttack.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
			{
				stat.mPhysicDefense.mPlus = Round( stat.mPhysicDefense.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
			{
				stat.mMagicAttack.mPlus = Round( stat.mMagicAttack.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
			{
				stat.mMagicDefense.mPlus = Round( stat.mMagicDefense.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalRate:
			{
				stat.mCriticalRate.mPlus = Round( stat.mCriticalRate.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
			{
				stat.mCriticalDamage.mPlus = Round( stat.mCriticalDamage.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusAccuracy:
			{
				stat.mAccuracy.mPlus = Round( stat.mAccuracy.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusEvade:
			{
				stat.mEvade.mPlus = Round( stat.mEvade.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
			{
				stat.mMoveSpeed.mPlus = Round( stat.mMoveSpeed.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLife:
			{
				stat.mLife.mPlus = Round( stat.mLife.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMana:
			{
				stat.mMana.mPlus = Round( stat.mMana.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
			{
				stat.mRecoveryLife.mPlus = Round( stat.mRecoveryLife.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusManaRecovery:
			{
				stat.mRecoveryMana.mPlus = Round( stat.mRecoveryMana.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentStrength:
			{
				stat.mStrength.mPercent = Round( stat.mStrength.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentIntelligence:
			{
				stat.mIntelligence.mPercent = Round( stat.mIntelligence.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentDexterity:
			{
				stat.mDexterity.mPercent = Round( stat.mDexterity.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentWisdom:
			{
				stat.mWisdom.mPercent = Round( stat.mWisdom.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentVitality:
			{
				stat.mVitality.mPercent = Round( stat.mVitality.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
			{
				stat.mPhysicAttack.mPercent = Round( stat.mPhysicAttack.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
			{
				stat.mPhysicDefense.mPercent = Round( stat.mPhysicDefense.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
			{
				stat.mMagicAttack.mPercent = Round( stat.mMagicAttack.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
			{
				stat.mMagicDefense.mPercent = Round( stat.mMagicDefense.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalRate:
			{
				stat.mCriticalRate.mPercent = Round( stat.mCriticalRate.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
			{
				stat.mCriticalDamage.mPercent = Round( stat.mCriticalDamage.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentAccuracy:
			{
				stat.mAccuracy.mPercent = Round( stat.mAccuracy.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentEvade:
			{
				stat.mEvade.mPercent = Round( stat.mEvade.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
			{
				stat.mMoveSpeed.mPercent = Round( stat.mMoveSpeed.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLife:
			{
				stat.mLife.mPercent = Round( stat.mLife.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMana:
			{
				stat.mMana.mPercent = Round( stat.mMana.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
			{
				stat.mRecoveryLife.mPercent = Round( stat.mRecoveryLife.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentManaRecovery:
			{
				stat.mRecoveryMana.mPercent = Round( stat.mRecoveryMana.mPercent + value.mValue, 3 );
				break;
			}
		}
	}

	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript( option.mEnchant.mIndex );
	const stEnchantLvBonusRate& pEnchantLvBonusRate = g_CGameResourceManager.GetEnchantLvBonusInfo( option.mEnchant.mLevel );

	if( script && option.mEnchant.mLevel )
	{
		for(EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
			++it )
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus += GetBonusEnchantValue( info, option, info.ImprovementStr, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus += GetBonusEnchantValue( info, option, info.ImprovementDex, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus += GetBonusEnchantValue( info, option, info.ImprovementVit, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus += GetBonusEnchantValue( info, option, info.ImprovementWis, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus += GetBonusEnchantValue( info, option, info.ImprovementInt, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus += GetBonusEnchantValue( info, option, info.ImprovementLife, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus += GetBonusEnchantValue( info, option, info.ImprovementMana, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus += GetBonusEnchantValue( info, option, info.PhysicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus += GetBonusEnchantValue( info, option, info.PhysicDefense, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus += GetBonusEnchantValue( info, option, info.MagicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus += GetBonusEnchantValue( info, option, info.MagicDefense, pEnchantLvBonusRate );
					break;
				}
				// 080319 LUJ, 추가 인챈트 수치를 부여할 수 없는 속성들
			case ReinforceScript::eTypeMoveSpeed:
			case ReinforceScript::eTypeEvade:
			case ReinforceScript::eTypeAccuracy:
			case ReinforceScript::eTypeLifeRecovery:
			case ReinforceScript::eTypeManaRecovery:
			case ReinforceScript::eTypeCriticalRate:
			case ReinforceScript::eTypeCriticalDamage:
				{
					break;
				}
			}
		}
	}
}


void CCharacterCalcManager::AddStat( const PlayerStat& srcStat, PlayerStat& destStat )
{
	destStat.mPhysicAttack.mPercent		+= srcStat.mPhysicAttack.mPercent;
	destStat.mPhysicAttack.mPlus		+= srcStat.mPhysicAttack.mPlus;

	destStat.mMagicAttack.mPercent		+= srcStat.mMagicAttack.mPercent;
	destStat.mMagicAttack.mPlus			+= srcStat.mMagicAttack.mPlus;

	destStat.mPhysicDefense.mPercent	+= srcStat.mPhysicDefense.mPercent;
	destStat.mPhysicDefense.mPlus		+= srcStat.mPhysicDefense.mPlus;

	destStat.mMagicDefense.mPercent		+= srcStat.mMagicDefense.mPercent;
	destStat.mMagicDefense.mPlus		+= srcStat.mMagicDefense.mPlus;

	destStat.mStrength.mPercent			+= srcStat.mStrength.mPercent;
	destStat.mStrength.mPlus			+= srcStat.mStrength.mPlus;

	destStat.mDexterity.mPercent 		+= srcStat.mDexterity.mPercent;
	destStat.mDexterity.mPlus 			+= srcStat.mDexterity.mPlus;

	destStat.mVitality.mPercent			+= srcStat.mVitality.mPercent;
	destStat.mVitality.mPlus			+= srcStat.mVitality.mPlus;

	destStat.mIntelligence.mPercent 	+= srcStat.mIntelligence.mPercent;
	destStat.mIntelligence.mPlus 		+= srcStat.mIntelligence.mPlus;

	destStat.mWisdom.mPercent 			+= srcStat.mWisdom.mPercent;
	destStat.mWisdom.mPlus 				+= srcStat.mWisdom.mPlus;

	destStat.mLife.mPercent				+= srcStat.mLife.mPercent;
	destStat.mLife.mPlus				+= srcStat.mLife.mPlus;

	destStat.mMana.mPercent				+= srcStat.mMana.mPercent;
	destStat.mMana.mPlus				+= srcStat.mMana.mPlus;

	destStat.mRecoveryLife.mPercent	+= srcStat.mRecoveryLife.mPercent;
	destStat.mRecoveryLife.mPlus		+= srcStat.mRecoveryLife.mPlus;

	destStat.mRecoveryMana.mPercent		+= srcStat.mRecoveryMana.mPercent;
	destStat.mRecoveryMana.mPlus		+= srcStat.mRecoveryMana.mPlus;

	destStat.mAccuracy.mPercent			+= srcStat.mAccuracy.mPercent;
	destStat.mAccuracy.mPlus			+= srcStat.mAccuracy.mPlus;

	destStat.mEvade.mPercent			+= srcStat.mEvade.mPercent;
	destStat.mEvade.mPlus				+= srcStat.mEvade.mPlus;

	destStat.mCriticalRate.mPercent		+= srcStat.mCriticalRate.mPercent;
	destStat.mCriticalRate.mPlus		+= srcStat.mCriticalRate.mPlus;

	destStat.mCriticalDamage.mPercent	+= srcStat.mCriticalDamage.mPercent;
	destStat.mCriticalDamage.mPlus		+= srcStat.mCriticalDamage.mPlus;

	destStat.mMoveSpeed.mPercent		+= srcStat.mMoveSpeed.mPercent;
	destStat.mMoveSpeed.mPlus			+= srcStat.mMoveSpeed.mPlus;
}


void CCharacterCalcManager::RemoveStat( const ITEM_INFO& info, PlayerStat& stat )
{
	stat.mPhysicAttack.mPlus	-= float( info.PhysicAttack );
	stat.mMagicAttack.mPlus		-= float( info.MagicAttack );
	stat.mPhysicDefense.mPlus	-= float( info.PhysicDefense );
	stat.mMagicDefense.mPlus	-= float( info.MagicDefense );
	stat.mStrength.mPlus		-= float( info.ImprovementStr );
	stat.mDexterity.mPlus		-= float( info.ImprovementDex );
	stat.mVitality.mPlus 		-= float( info.ImprovementVit );
	stat.mWisdom.mPlus 			-= float( info.ImprovementWis );
	stat.mIntelligence.mPlus	-= float( info.ImprovementInt );
	stat.mLife.mPlus 			-= float( info.ImprovementLife );
	stat.mMana.mPlus 			-= float( info.ImprovementMana );
}


// 080319 LUJ, 추가 인챈트 수치를 제거함
// 080320 LUJ, 추가 인챈트 수치를 함수를 통해 가져옴
void CCharacterCalcManager::RemoveStat( const ITEM_INFO& info, const ITEM_OPTION& option, PlayerStat& stat )
{
	const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
	const ITEM_OPTION::Mix&			mix			= option.mMix;

	stat.mPhysicAttack.mPlus	-= float( reinforce.mPhysicAttack );
	stat.mPhysicDefense.mPlus	-= float( reinforce.mPhysicDefence );
	stat.mMagicAttack.mPlus		-= float( reinforce.mMagicAttack );
	stat.mMagicDefense.mPlus	-= float( reinforce.mMagicDefence );
	stat.mStrength.mPlus		-= float( reinforce.mStrength		+ mix.mStrength );
	stat.mDexterity.mPlus		-= float( reinforce.mDexterity		+ mix.mDexterity );
	stat.mVitality.mPlus		-= float( reinforce.mVitality		+ mix.mVitality );
	stat.mWisdom.mPlus			-= float( reinforce.mWisdom			+ mix.mWisdom );
	stat.mIntelligence.mPlus	-= float( reinforce.mIntelligence	+ mix.mIntelligence );
	stat.mLife.mPlus			-= float( reinforce.mLife );
	stat.mMana.mPlus			-= float( reinforce.mMana );
	stat.mRecoveryLife.mPlus	-= float( reinforce.mLifeRecovery );
	stat.mRecoveryMana.mPlus	-= float( reinforce.mManaRecovery );
	stat.mCriticalRate.mPlus	-= float( reinforce.mCriticalRate );
	stat.mCriticalDamage.mPlus	-= float( reinforce.mCriticalDamage );
	stat.mAccuracy.mPlus		-= float( reinforce.mAccuracy );
	stat.mMoveSpeed.mPlus		-= float( reinforce.mMoveSpeed );
	stat.mEvade.mPlus			-= float( reinforce.mEvade );

	const ITEM_OPTION::Drop& drop = option.mDrop;

	for( DWORD i = 0; i < sizeof( drop.mValue ) / sizeof( *drop.mValue ); ++i )
	{
		const ITEM_OPTION::Drop::Value& value = drop.mValue[ i ];

		switch( value.mKey )
		{
		case ITEM_OPTION::Drop::KeyPlusStrength:
			{
				stat.mStrength.mPlus = Round( stat.mStrength.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusIntelligence:
			{
				stat.mIntelligence.mPlus = Round( stat.mIntelligence.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusDexterity:
			{
				stat.mDexterity.mPlus = Round( stat.mDexterity.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusWisdom:
			{
				stat.mWisdom.mPlus = Round( stat.mWisdom.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusVitality:
			{
				stat.mVitality.mPlus = Round( stat.mVitality.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
			{
				stat.mPhysicAttack.mPlus = Round( stat.mPhysicAttack.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
			{
				stat.mPhysicDefense.mPlus = Round( stat.mPhysicDefense.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
			{
				stat.mMagicAttack.mPlus = Round( stat.mMagicAttack.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
			{
				stat.mMagicDefense.mPlus = Round( stat.mMagicDefense.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalRate:
			{
				stat.mCriticalRate.mPlus = Round( stat.mCriticalRate.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
			{
				stat.mCriticalDamage.mPlus = Round( stat.mCriticalDamage.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusAccuracy:
			{
				stat.mAccuracy.mPlus = Round( stat.mAccuracy.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusEvade:
			{
				stat.mEvade.mPlus = Round( stat.mEvade.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
			{
				stat.mMoveSpeed.mPlus = Round( stat.mMoveSpeed.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLife:
			{
				stat.mLife.mPlus = Round( stat.mLife.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMana:
			{
				stat.mMana.mPlus = Round( stat.mMana.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
			{
				stat.mRecoveryLife.mPlus = Round( stat.mRecoveryLife.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusManaRecovery:
			{
				stat.mRecoveryMana.mPlus = Round( stat.mRecoveryMana.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentStrength:
			{
				stat.mStrength.mPercent = Round( stat.mStrength.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentIntelligence:
			{
				stat.mIntelligence.mPercent = Round( stat.mIntelligence.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentDexterity:
			{
				stat.mDexterity.mPercent = Round( stat.mDexterity.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentWisdom:
			{
				stat.mWisdom.mPercent = Round( stat.mWisdom.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentVitality:
			{
				stat.mVitality.mPercent = Round( stat.mVitality.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
			{
				stat.mPhysicAttack.mPercent = Round( stat.mPhysicAttack.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
			{
				stat.mPhysicDefense.mPercent = Round( stat.mPhysicDefense.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
			{
				stat.mMagicAttack.mPercent = Round( stat.mMagicAttack.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
			{
				stat.mMagicDefense.mPercent = Round( stat.mMagicDefense.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalRate:
			{
				stat.mCriticalRate.mPercent = Round( stat.mCriticalRate.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
			{
				stat.mCriticalDamage.mPercent = Round( stat.mCriticalDamage.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentAccuracy:
			{
				stat.mAccuracy.mPercent = Round( stat.mAccuracy.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentEvade:
			{
				stat.mEvade.mPercent = Round( stat.mEvade.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
			{
				stat.mMoveSpeed.mPercent = Round( stat.mMoveSpeed.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLife:
			{
				stat.mLife.mPercent = Round( stat.mLife.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMana:
			{
				stat.mMana.mPercent = Round( stat.mMana.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
			{
				stat.mRecoveryLife.mPercent = Round( stat.mRecoveryLife.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentManaRecovery:
			{
				stat.mRecoveryMana.mPercent = Round( stat.mRecoveryMana.mPercent - value.mValue, 3 );
				break;
			}
		}
	}

	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript( option.mEnchant.mIndex );
	const stEnchantLvBonusRate& pEnchantLvBonusRate = g_CGameResourceManager.GetEnchantLvBonusInfo( option.mEnchant.mLevel );

	if( script && option.mEnchant.mLevel )
	{
		for(EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
			++it)
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementStr, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementDex, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementVit, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementWis, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementInt, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementLife, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementMana, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus -= GetBonusEnchantValue( info, option, info.PhysicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus -= GetBonusEnchantValue( info, option, info.PhysicDefense, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus -= GetBonusEnchantValue( info, option, info.MagicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus -= GetBonusEnchantValue( info, option, info.MagicDefense, pEnchantLvBonusRate );
					break;
				}
				// 080319 LUJ, 추가 인챈트 수치를 부여할 수 없는 속성들
			case ReinforceScript::eTypeMoveSpeed:
			case ReinforceScript::eTypeEvade:
			case ReinforceScript::eTypeAccuracy:
			case ReinforceScript::eTypeLifeRecovery:
			case ReinforceScript::eTypeManaRecovery:
			case ReinforceScript::eTypeCriticalRate:
			case ReinforceScript::eTypeCriticalDamage:
				{
					break;
				}
			}
		}
	}
}


void CCharacterCalcManager::RemoveStat( const PlayerStat& srcStat, PlayerStat& destStat )
{
	destStat.mPhysicAttack.mPercent		-= srcStat.mPhysicAttack.mPercent;
	destStat.mPhysicAttack.mPlus		-= srcStat.mPhysicAttack.mPlus;

	destStat.mMagicAttack.mPercent		-= srcStat.mMagicAttack.mPercent;
	destStat.mMagicAttack.mPlus			-= srcStat.mMagicAttack.mPlus;

	destStat.mPhysicDefense.mPercent	-= srcStat.mPhysicDefense.mPercent;
	destStat.mPhysicDefense.mPlus		-= srcStat.mPhysicDefense.mPlus;

	destStat.mMagicDefense.mPercent		-= srcStat.mMagicDefense.mPercent;
	destStat.mMagicDefense.mPlus		-= srcStat.mMagicDefense.mPlus;

	destStat.mStrength.mPercent			-= srcStat.mStrength.mPercent;
	destStat.mStrength.mPlus			-= srcStat.mStrength.mPlus;

	destStat.mDexterity.mPercent 		-= srcStat.mDexterity.mPercent;
	destStat.mDexterity.mPlus 			-= srcStat.mDexterity.mPlus;

	destStat.mVitality.mPercent			-= srcStat.mVitality.mPercent;
	destStat.mVitality.mPlus			-= srcStat.mVitality.mPlus;

	destStat.mIntelligence.mPercent 	-= srcStat.mIntelligence.mPercent;
	destStat.mIntelligence.mPlus 		-= srcStat.mIntelligence.mPlus;

	destStat.mWisdom.mPercent 			-= srcStat.mWisdom.mPercent;
	destStat.mWisdom.mPlus 				-= srcStat.mWisdom.mPlus;

	destStat.mLife.mPercent				-= srcStat.mLife.mPercent;
	destStat.mLife.mPlus				-= srcStat.mLife.mPlus;

	destStat.mMana.mPercent				-= srcStat.mMana.mPercent;
	destStat.mMana.mPlus				-= srcStat.mMana.mPlus;

	destStat.mRecoveryLife.mPercent	-= srcStat.mRecoveryLife.mPercent;
	destStat.mRecoveryLife.mPlus		-= srcStat.mRecoveryLife.mPlus;

	destStat.mRecoveryMana.mPercent		-= srcStat.mRecoveryMana.mPercent;
	destStat.mRecoveryMana.mPlus		-= srcStat.mRecoveryMana.mPlus;

	destStat.mAccuracy.mPercent			-= srcStat.mAccuracy.mPercent;
	destStat.mAccuracy.mPlus			-= srcStat.mAccuracy.mPlus;

	destStat.mEvade.mPercent			-= srcStat.mEvade.mPercent;
	destStat.mEvade.mPlus				-= srcStat.mEvade.mPlus;

	destStat.mCriticalRate.mPercent		-= srcStat.mCriticalRate.mPercent;
	destStat.mCriticalRate.mPlus		-= srcStat.mCriticalRate.mPlus;

	destStat.mCriticalDamage.mPercent	-= srcStat.mCriticalDamage.mPercent;
	destStat.mCriticalDamage.mPlus		-= srcStat.mCriticalDamage.mPlus;

	destStat.mMoveSpeed.mPercent		-= srcStat.mMoveSpeed.mPercent;
	destStat.mMoveSpeed.mPlus			-= srcStat.mMoveSpeed.mPlus;
}


void CCharacterCalcManager::AddItemBaseStat( const ITEM_INFO& info, const ITEM_OPTION& option, PlayerStat& stat )
{
	stat.mPhysicAttack.mPlus	+= float( info.PhysicAttack );
	stat.mMagicAttack.mPlus		+= float( info.MagicAttack );
	stat.mPhysicDefense.mPlus	+= float( info.PhysicDefense );
	stat.mMagicDefense.mPlus	+= float( info.MagicDefense );
	stat.mStrength.mPlus		+= float( info.ImprovementStr );
	stat.mDexterity.mPlus		+= float( info.ImprovementDex );
	stat.mVitality.mPlus 		+= float( info.ImprovementVit );
	stat.mWisdom.mPlus 			+= float( info.ImprovementWis );
	stat.mIntelligence.mPlus	+= float( info.ImprovementInt );
	stat.mLife.mPlus 			+= float( info.ImprovementLife );
	stat.mMana.mPlus 			+= float( info.ImprovementMana );
	
	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript( option.mEnchant.mIndex );
	const stEnchantLvBonusRate& pEnchantLvBonusRate = g_CGameResourceManager.GetEnchantLvBonusInfo( option.mEnchant.mLevel );

	if( script && option.mEnchant.mLevel )
	{
		for(EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
			++it)
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mStrength.mPlus += GetBonusEnchantValue( info, option, info.ImprovementStr, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mDexterity.mPlus += GetBonusEnchantValue( info, option, info.ImprovementDex, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mVitality.mPlus += GetBonusEnchantValue( info, option, info.ImprovementVit, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mWisdom.mPlus += GetBonusEnchantValue( info, option, info.ImprovementWis, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mIntelligence.mPlus += GetBonusEnchantValue( info, option, info.ImprovementInt, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mLife.mPlus += GetBonusEnchantValue( info, option, info.ImprovementLife, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mMana.mPlus += GetBonusEnchantValue( info, option, info.ImprovementMana, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mPhysicAttack.mPlus += GetBonusEnchantValue( info, option, info.PhysicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mPhysicDefense.mPlus += GetBonusEnchantValue( info, option, info.PhysicDefense, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mMagicAttack.mPlus += GetBonusEnchantValue( info, option, info.MagicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mMagicDefense.mPlus += GetBonusEnchantValue( info, option, info.MagicDefense, pEnchantLvBonusRate );
					break;
				}
				// 080319 LUJ, 추가 인챈트 수치를 부여할 수 없는 속성들
			case ReinforceScript::eTypeMoveSpeed:
			case ReinforceScript::eTypeEvade:
			case ReinforceScript::eTypeAccuracy:
			case ReinforceScript::eTypeLifeRecovery:
			case ReinforceScript::eTypeManaRecovery:
			case ReinforceScript::eTypeCriticalRate:
			case ReinforceScript::eTypeCriticalDamage:
				{
					break;
				}
			}
		}
	}
}

void CCharacterCalcManager::RemoveItemBaseStat( const ITEM_INFO& info, const ITEM_OPTION& option, PlayerStat& stat )
{
	stat.mPhysicAttack.mPlus	-= float( info.PhysicAttack );
	stat.mMagicAttack.mPlus		-= float( info.MagicAttack );
	stat.mPhysicDefense.mPlus	-= float( info.PhysicDefense );
	stat.mMagicDefense.mPlus	-= float( info.MagicDefense );
	stat.mStrength.mPlus		-= float( info.ImprovementStr );
	stat.mDexterity.mPlus		-= float( info.ImprovementDex );
	stat.mVitality.mPlus 		-= float( info.ImprovementVit );
	stat.mWisdom.mPlus 			-= float( info.ImprovementWis );
	stat.mIntelligence.mPlus	-= float( info.ImprovementInt );
	stat.mLife.mPlus 			-= float( info.ImprovementLife );
	stat.mMana.mPlus 			-= float( info.ImprovementMana );
	
	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript( option.mEnchant.mIndex );
	const stEnchantLvBonusRate& pEnchantLvBonusRate = g_CGameResourceManager.GetEnchantLvBonusInfo( option.mEnchant.mLevel );

	if( script && option.mEnchant.mLevel )
	{
		for(
			EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
		++it )
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mStrength.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementStr, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mDexterity.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementDex, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mVitality.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementVit, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mWisdom.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementWis, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mIntelligence.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementInt, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mLife.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementLife, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mMana.mPlus -= GetBonusEnchantValue( info, option, info.ImprovementMana, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mPhysicAttack.mPlus -= GetBonusEnchantValue( info, option, info.PhysicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mPhysicDefense.mPlus -= GetBonusEnchantValue( info, option, info.PhysicDefense, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mMagicAttack.mPlus -= GetBonusEnchantValue( info, option, info.MagicAttack, pEnchantLvBonusRate );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					stat.mMagicDefense.mPlus -= GetBonusEnchantValue( info, option, info.MagicDefense, pEnchantLvBonusRate );
					break;
				}
				// 080319 LUJ, 추가 인챈트 수치를 부여할 수 없는 속성들
			case ReinforceScript::eTypeMoveSpeed:
			case ReinforceScript::eTypeEvade:
			case ReinforceScript::eTypeAccuracy:
			case ReinforceScript::eTypeLifeRecovery:
			case ReinforceScript::eTypeManaRecovery:
			case ReinforceScript::eTypeCriticalRate:
			case ReinforceScript::eTypeCriticalDamage:
				{
					break;
				}
			}
		}
	}
}

void CCharacterCalcManager::AddItemOptionStat( const ITEM_INFO& info, const ITEM_OPTION& option, PlayerStat& stat )
{
	const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
	const ITEM_OPTION::Mix&			mix			= option.mMix;

	stat.mPhysicAttack.mPlus	+= float( reinforce.mPhysicAttack );
	stat.mPhysicDefense.mPlus	+= float( reinforce.mPhysicDefence );
	stat.mMagicAttack.mPlus		+= float( reinforce.mMagicAttack );
	stat.mMagicDefense.mPlus	+= float( reinforce.mMagicDefence );
	stat.mStrength.mPlus		+= float( reinforce.mStrength		+ mix.mStrength );
	stat.mDexterity.mPlus		+= float( reinforce.mDexterity		+ mix.mDexterity );
	stat.mVitality.mPlus		+= float( reinforce.mVitality		+ mix.mVitality );
	stat.mWisdom.mPlus			+= float( reinforce.mWisdom			+ mix.mWisdom );
	stat.mIntelligence.mPlus	+= float( reinforce.mIntelligence	+ mix.mIntelligence );
	stat.mLife.mPlus			+= float( reinforce.mLife );
	stat.mMana.mPlus			+= float( reinforce.mMana );
	stat.mRecoveryLife.mPlus	+= float( reinforce.mLifeRecovery );
	stat.mRecoveryMana.mPlus	+= float( reinforce.mManaRecovery );
	stat.mCriticalRate.mPlus	+= float( reinforce.mCriticalRate );
	stat.mCriticalDamage.mPlus	+= float( reinforce.mCriticalDamage );
	stat.mAccuracy.mPlus		+= float( reinforce.mAccuracy );
	stat.mMoveSpeed.mPlus		+= float( reinforce.mMoveSpeed );
	stat.mEvade.mPlus			+= float( reinforce.mEvade );

	const ITEM_OPTION::Drop& drop = option.mDrop;

	for( DWORD i = 0; i < sizeof( drop.mValue ) / sizeof( *drop.mValue ); ++i )
	{
		const ITEM_OPTION::Drop::Value& value = drop.mValue[ i ];

		// 080410 LUJ, 실수 오차를 없애기 위해 소수점 세자리 아래는 절삭한다
		switch( value.mKey )
		{
		case ITEM_OPTION::Drop::KeyPlusStrength:
			{
				stat.mStrength.mPlus = Round( stat.mStrength.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusIntelligence:
			{
				stat.mIntelligence.mPlus = Round( stat.mIntelligence.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusDexterity:
			{
				stat.mDexterity.mPlus = Round( stat.mDexterity.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusWisdom:
			{
				stat.mWisdom.mPlus = Round( stat.mWisdom.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusVitality:
			{
				stat.mVitality.mPlus = Round( stat.mVitality.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
			{
				stat.mPhysicAttack.mPlus = Round( stat.mPhysicAttack.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
			{
				stat.mPhysicDefense.mPlus = Round( stat.mPhysicDefense.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
			{
				stat.mMagicAttack.mPlus = Round( stat.mMagicAttack.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
			{
				stat.mMagicDefense.mPlus = Round( stat.mMagicDefense.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalRate:
			{
				stat.mCriticalRate.mPlus = Round( stat.mCriticalRate.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
			{
				stat.mCriticalDamage.mPlus = Round( stat.mCriticalDamage.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusAccuracy:
			{
				stat.mAccuracy.mPlus = Round( stat.mAccuracy.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusEvade:
			{
				stat.mEvade.mPlus = Round( stat.mEvade.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
			{
				stat.mMoveSpeed.mPlus = Round( stat.mMoveSpeed.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLife:
			{
				stat.mLife.mPlus = Round( stat.mLife.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMana:
			{
				stat.mMana.mPlus = Round( stat.mMana.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
			{
				stat.mRecoveryLife.mPlus = Round( stat.mRecoveryLife.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusManaRecovery:
			{
				stat.mRecoveryMana.mPlus = Round( stat.mRecoveryMana.mPlus + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentStrength:
			{
				stat.mStrength.mPercent = Round( stat.mStrength.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentIntelligence:
			{
				stat.mIntelligence.mPercent = Round( stat.mIntelligence.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentDexterity:
			{
				stat.mDexterity.mPercent = Round( stat.mDexterity.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentWisdom:
			{
				stat.mWisdom.mPercent = Round( stat.mWisdom.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentVitality:
			{
				stat.mVitality.mPercent = Round( stat.mVitality.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
			{
				stat.mPhysicAttack.mPercent = Round( stat.mPhysicAttack.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
			{
				stat.mPhysicDefense.mPercent = Round( stat.mPhysicDefense.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
			{
				stat.mMagicAttack.mPercent = Round( stat.mMagicAttack.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
			{
				stat.mMagicDefense.mPercent = Round( stat.mMagicDefense.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalRate:
			{
				stat.mCriticalRate.mPercent = Round( stat.mCriticalRate.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
			{
				stat.mCriticalDamage.mPercent = Round( stat.mCriticalDamage.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentAccuracy:
			{
				stat.mAccuracy.mPercent = Round( stat.mAccuracy.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentEvade:
			{
				stat.mEvade.mPercent = Round( stat.mEvade.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
			{
				stat.mMoveSpeed.mPercent = Round( stat.mMoveSpeed.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLife:
			{
				stat.mLife.mPercent = Round( stat.mLife.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMana:
			{
				stat.mMana.mPercent = Round( stat.mMana.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
			{
				stat.mRecoveryLife.mPercent = Round( stat.mRecoveryLife.mPercent + value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentManaRecovery:
			{
				stat.mRecoveryMana.mPercent = Round( stat.mRecoveryMana.mPercent + value.mValue, 3 );
				break;
			}
		}
	}

	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript( option.mEnchant.mIndex );

	if( script && option.mEnchant.mLevel )
	{
		for(
			EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
		++it )
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus -= ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
				// 080319 LUJ, 추가 인챈트 수치를 부여할 수 없는 속성들
			case ReinforceScript::eTypeMoveSpeed:
			case ReinforceScript::eTypeEvade:
			case ReinforceScript::eTypeAccuracy:
			case ReinforceScript::eTypeLifeRecovery:
			case ReinforceScript::eTypeManaRecovery:
			case ReinforceScript::eTypeCriticalRate:
			case ReinforceScript::eTypeCriticalDamage:
				{
					break;
				}
			}
		}
	}
}

void CCharacterCalcManager::RemoveItemOptionStat( const ITEM_INFO& info, const ITEM_OPTION& option, PlayerStat& stat )
{
	const ITEM_OPTION::Reinforce&	reinforce	= option.mReinforce;
	const ITEM_OPTION::Mix&			mix			= option.mMix;

	stat.mPhysicAttack.mPlus	-= float( reinforce.mPhysicAttack );
	stat.mPhysicDefense.mPlus	-= float( reinforce.mPhysicDefence );
	stat.mMagicAttack.mPlus		-= float( reinforce.mMagicAttack );
	stat.mMagicDefense.mPlus	-= float( reinforce.mMagicDefence );
	stat.mStrength.mPlus		-= float( reinforce.mStrength		+ mix.mStrength );
	stat.mDexterity.mPlus		-= float( reinforce.mDexterity		+ mix.mDexterity );
	stat.mVitality.mPlus		-= float( reinforce.mVitality		+ mix.mVitality );
	stat.mWisdom.mPlus			-= float( reinforce.mWisdom			+ mix.mWisdom );
	stat.mIntelligence.mPlus	-= float( reinforce.mIntelligence	+ mix.mIntelligence );
	stat.mLife.mPlus			-= float( reinforce.mLife );
	stat.mMana.mPlus			-= float( reinforce.mMana );
	stat.mRecoveryLife.mPlus	-= float( reinforce.mLifeRecovery );
	stat.mRecoveryMana.mPlus	-= float( reinforce.mManaRecovery );
	stat.mCriticalRate.mPlus	-= float( reinforce.mCriticalRate );
	stat.mCriticalDamage.mPlus	-= float( reinforce.mCriticalDamage );
	stat.mAccuracy.mPlus		-= float( reinforce.mAccuracy );
	stat.mMoveSpeed.mPlus		-= float( reinforce.mMoveSpeed );
	stat.mEvade.mPlus			-= float( reinforce.mEvade );

	const ITEM_OPTION::Drop& drop = option.mDrop;

	for( DWORD i = 0; i < sizeof( drop.mValue ) / sizeof( *drop.mValue ); ++i )
	{
		const ITEM_OPTION::Drop::Value& value = drop.mValue[ i ];

		// 080410 LUJ, 실수 오차를 없애기 위해 소수점 세자리 아래는 절삭한다
		switch( value.mKey )
		{
		case ITEM_OPTION::Drop::KeyPlusStrength:
			{
				stat.mStrength.mPlus = Round( stat.mStrength.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusIntelligence:
			{
				stat.mIntelligence.mPlus = Round( stat.mIntelligence.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusDexterity:
			{
				stat.mDexterity.mPlus = Round( stat.mDexterity.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusWisdom:
			{
				stat.mWisdom.mPlus = Round( stat.mWisdom.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusVitality:
			{
				stat.mVitality.mPlus = Round( stat.mVitality.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
			{
				stat.mPhysicAttack.mPlus = Round( stat.mPhysicAttack.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
			{
				stat.mPhysicDefense.mPlus = Round( stat.mPhysicDefense.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
			{
				stat.mMagicAttack.mPlus = Round( stat.mMagicAttack.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
			{
				stat.mMagicDefense.mPlus = Round( stat.mMagicDefense.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalRate:
			{
				stat.mCriticalRate.mPlus = Round( stat.mCriticalRate.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
			{
				stat.mCriticalDamage.mPlus = Round( stat.mCriticalDamage.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusAccuracy:
			{
				stat.mAccuracy.mPlus = Round( stat.mAccuracy.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusEvade:
			{
				stat.mEvade.mPlus = Round( stat.mEvade.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
			{
				stat.mMoveSpeed.mPlus = Round( stat.mMoveSpeed.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLife:
			{
				stat.mLife.mPlus = Round( stat.mLife.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusMana:
			{
				stat.mMana.mPlus = Round( stat.mMana.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
			{
				stat.mRecoveryLife.mPlus = Round( stat.mRecoveryLife.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPlusManaRecovery:
			{
				stat.mRecoveryMana.mPlus = Round( stat.mRecoveryMana.mPlus - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentStrength:
			{
				stat.mStrength.mPercent = Round( stat.mStrength.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentIntelligence:
			{
				stat.mIntelligence.mPercent = Round( stat.mIntelligence.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentDexterity:
			{
				stat.mDexterity.mPercent = Round( stat.mDexterity.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentWisdom:
			{
				stat.mWisdom.mPercent = Round( stat.mWisdom.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentVitality:
			{
				stat.mVitality.mPercent = Round( stat.mVitality.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
			{
				stat.mPhysicAttack.mPercent = Round( stat.mPhysicAttack.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
			{
				stat.mPhysicDefense.mPercent = Round( stat.mPhysicDefense.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
			{
				stat.mMagicAttack.mPercent = Round( stat.mMagicAttack.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
			{
				stat.mMagicDefense.mPercent = Round( stat.mMagicDefense.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalRate:
			{
				stat.mCriticalRate.mPercent = Round( stat.mCriticalRate.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
			{
				stat.mCriticalDamage.mPercent = Round( stat.mCriticalDamage.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentAccuracy:
			{
				stat.mAccuracy.mPercent = Round( stat.mAccuracy.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentEvade:
			{
				stat.mEvade.mPercent = Round( stat.mEvade.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
			{
				stat.mMoveSpeed.mPercent = Round( stat.mMoveSpeed.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLife:
			{
				stat.mLife.mPercent = Round( stat.mLife.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentMana:
			{
				stat.mMana.mPercent = Round( stat.mMana.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
			{
				stat.mRecoveryLife.mPercent = Round( stat.mRecoveryLife.mPercent - value.mValue, 3 );
				break;
			}
		case ITEM_OPTION::Drop::KeyPercentManaRecovery:
			{
				stat.mRecoveryMana.mPercent = Round( stat.mRecoveryMana.mPercent - value.mValue, 3 );
				break;
			}
		}
	}
	
	const EnchantScript* script = g_CGameResourceManager.GetEnchantScript( option.mEnchant.mIndex );

	if( script && option.mEnchant.mLevel )
	{
		for(
			EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
		++it )
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypeStrength:
				{
					stat.mStrength.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeDexterity:
				{
					stat.mDexterity.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeVitality:
				{
					stat.mVitality.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeWisdom:
				{
					stat.mWisdom.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeIntelligence:
				{
					stat.mIntelligence.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeLife:
				{
					stat.mLife.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeMana:
				{
					stat.mMana.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypePhysicAttack:
				{
					stat.mPhysicAttack.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypePhysicDefence:
				{
					stat.mPhysicDefense.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeMagicAttack:
				{
					stat.mMagicAttack.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
			case ReinforceScript::eTypeMagicDefence:
				{
					stat.mMagicDefense.mPlus += ( option.mEnchant.mLevel * info.EnchantDeterm );
					break;
				}
				// 080319 LUJ, 추가 인챈트 수치를 부여할 수 없는 속성들
			case ReinforceScript::eTypeMoveSpeed:
			case ReinforceScript::eTypeEvade:
			case ReinforceScript::eTypeAccuracy:
			case ReinforceScript::eTypeLifeRecovery:
			case ReinforceScript::eTypeManaRecovery:
			case ReinforceScript::eTypeCriticalRate:
			case ReinforceScript::eTypeCriticalDamage:
				{
					break;
				}
			}
		}
	}
}

BOOL CCharacterCalcManager::IsJobSkill(DWORD skillIndex)
{
	for(JobSkillContainer::const_iterator iterator = m_JobSkillList.begin();
		m_JobSkillList.end() != iterator;
		++iterator)
	{
		const JobSkillInfo& skillInfo = *iterator;

		if(skillInfo.dwSkillIndex == skillIndex)
		{
			return TRUE;
		}
	}

	return FALSE;
}