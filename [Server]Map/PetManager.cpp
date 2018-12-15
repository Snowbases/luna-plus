#include "stdafx.h"
#include ".\petmanager.h"
#include "MHFile.h"
#include "Player.h"
#include "charmove.h"
#include "tilemanager.h"
#include "Pet.h"
#include "UserTable.h"
#include "itemmanager.h"
#include "MapDBMsgParser.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "PackedData.h"
#include "..\[cc]header\GameResourceManager.h"

GLOBALTON(CPetManager);

CPetManager::CPetManager(void)
{
	mMaxGrade = 0;
	mMaxDistance = 3000;
	mDistanceCheckTime = 1000;
	mAreaDistance = 300;
	
	mGradeUpLevel = NULL;
	mSummonPlayerLevel = NULL;
	mExp = NULL;
	mExtendedSkillSlot = NULL;

	mPetInfoTable.Initialize( 10 );
	mPetHPMPInfoTable.Initialize( 10 );
	mPetFriendlyPenaltyTable.Initialize( 1 );

	mFriendlyReduceTime = 60*60*1000;
	mFriendlyReducePoint = 1;
	
	for( int i = 0; i < ePetType_Max; i++ )
	{
		mPetStatusInfoTable[ i ].Initialize( 20 );
	}
}

CPetManager::~CPetManager(void)
{
	//delete mGradeUpLevel;
	//delete mSummonPlayerLevel;
	//delete mExp;
	//delete mExtendedSkillSlot;

	// 081017 NYJ - 배열로 할당 했으므로 메모리 해제방식 변경
	delete [] mGradeUpLevel;
	delete [] mSummonPlayerLevel;
	delete [] mExp;
	delete [] mExtendedSkillSlot;

	PET_INFO* pInfo = NULL;

	mPetInfoTable.SetPositionHead();
	while( (pInfo = mPetInfoTable.GetData() )!= NULL)
	{
		delete pInfo;
	}
	mPetInfoTable.RemoveAll();

	PET_STATUS_INFO* pStatusInfo;

	for( int i = 0; i < ePetType_Max; i++ )
	{
		mPetStatusInfoTable[ i ].SetPositionHead();
		while( (pStatusInfo = mPetStatusInfoTable[ i ].GetData() )!= NULL)
		{
			delete pStatusInfo;
		}
		mPetStatusInfoTable[ i ].RemoveAll();
	}


	// 081017 NYJ - 메모리 해제코드 추가
	PET_HP_MP_INFO* pHPMPInfo = NULL;
	mPetHPMPInfoTable.SetPositionHead();
	while((pHPMPInfo = mPetHPMPInfoTable.GetData())!= NULL)
	{
		if(pHPMPInfo)
		{
			delete pHPMPInfo;
			pHPMPInfo = NULL;
		}
	}

	// 081017 NYJ - 메모리 해제코드 추가
	PET_FRIENDLY_PENALTY* pFPenalty = NULL;
	mPetFriendlyPenaltyTable.SetPositionHead();
	while((pFPenalty = mPetFriendlyPenaltyTable.GetData())!= NULL)
	{
		if(pFPenalty)
		{
			delete pFPenalty;
			pFPenalty = NULL;
		}
	}

	// 091214 ONS 펫소환관련 메모리 해제
	m_setSummonPlayerIndex.clear();
}

void CPetManager::SealPet( CPet* pPet )
{
	if(0 == pPet)
	{
		return;
	}

	pPet->SetPetObjectState(
		ePetState_None);

	g_pServerSystem->RemovePet(
		pPet->GetID());

	CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(
		pPet->GetOwnerIndex());

	if(player &&
		eObjectKind_Player == player->GetObjectKind())
	{
		player->SetPetItemDbIndex(
			0);
	}
}

void CPetManager::SummonPet( CPlayer* pPlayer, DWORD ItemDBIdx, BOOL bIsSummoned )
{
	const PET_OBJECT_INFO& petObjectInfo = GetObjectInfo(
		ItemDBIdx);
	const PET_INFO* const pPetInfo = mPetInfoTable.GetData(
		petObjectInfo.Kind);

	if(0 == pPetInfo)
	{
		return;
	}

	if(bIsSummoned == TRUE)
	{
		m_setSummonPlayerIndex.insert(
			pPlayer->GetID());
	}

	pPlayer->SetPetItemDbIndex(
		ItemDBIdx);
	PetItemInfoLoad(
		pPlayer->GetID(),
		petObjectInfo.ID);
}

void CPetManager::LoadPetInfo()
{
	BOOL	didLoadMaxGrade = FALSE;
	BOOL	didLoadMaxLevel = FALSE;

	CMHFile file;
	file.Init("System/Resource/PetSystemInfo.bin","rb");
	
	char buf[256]={0,};
	while(FALSE == file.IsEOF())
	{
		file.GetString( buf );

		if( buf[ 0 ] == '@' )
		{
			file.GetLineX( buf, 256 );
			continue;
		}

		if( strcmp( buf,"#MAXGRADE" ) == 0 )
		{
			mMaxGrade = file.GetByte();
			didLoadMaxGrade = TRUE;
		}

		if( strcmp( buf, "#MAXLEVEL" ) == 0 )
		{
			mMaxLevel = file.GetByte();
			didLoadMaxLevel = TRUE;
		}

		if( strcmp( buf, "#GRADEUPLEVEL" ) == 0 )
		{
			if( didLoadMaxGrade )
			{
				mGradeUpLevel = new BYTE[ mMaxGrade ];

				for( int i = 0; i < mMaxGrade; i++ )
				{
					mGradeUpLevel[ i ] = file.GetByte();
				}
			}
			else
			{
				// 최대 진화 단계를 설정하지 않고 진화 레벨을 설정하였음
			}
		}

		if( strcmp( buf, "#SUMMONPLAYERLEVEL" ) == 0 )
		{
			if( didLoadMaxGrade )
			{
				mSummonPlayerLevel = new BYTE[ mMaxGrade + 1 ];

				for( int i = 0; i < mMaxGrade + 1; i++ )
				{
					mSummonPlayerLevel[ i ] = file.GetByte();
				}
			}
			else
			{
				// 최대 진화 단계를 설정하지 않고 소환 레벨 제한을 설정하였음
			}
		}

		if( strcmp( buf, "#EXP" ) == 0 )
		{
			if( didLoadMaxGrade )
			{
				mExp = new WORD[ mMaxGrade + 1 ];

				for( int i = 0; i < mMaxGrade + 1; i++ )
				{
					mExp[ i ] = file.GetWord();
				}
			}
			else
			{
				// 최대 진화 단계를 설정하지 않고 경험치를 설정하였음
			}
		}

		if( strcmp( buf, "#EXPPENALTYLEVEL" ) == 0 )
		{
			mExpPenaltyLevel = file.GetByte();
		}

		if( strcmp( buf, "#SHOPPETBASESKILLSLOT" ) == 0 )
		{
			mShopBaseSkillSlot = file.GetByte();
		}

		if( strcmp( buf, "#BASESKILLSLOT" ) == 0 )
		{
			mMinBaseSkillSlot = file.GetByte();
			mMaxBaseSkillSlot = file.GetByte();
		}

		if( strcmp( buf, "#EXTENDEDSKILLSLOT" ) == 0)
		{
			if( didLoadMaxGrade )
			{
				mExtendedSkillSlot = new BYTE[ mMaxGrade ];

				for( int i = 0; i < mMaxGrade; i++ )
				{
					mExtendedSkillSlot[ i ] = file.GetByte();
				}
			}
			else
			{
				// 최대 진화 단계를 설정하지 않고 진화 레벨을 설정하였음
			}
		}

		if( strcmp( buf, "#MAXDISTANCE" ) == 0 )
		{
			mMaxDistance = file.GetWord();
		}
		
		if( strcmp( buf, "#AREADISTANCE" ) == 0 )
		{
			mAreaDistance = file.GetWord();
		}

		if( strcmp( buf, "#DISTANCECHECKTIME" ) == 0 )
		{
			mDistanceCheckTime = file.GetDword();
		}

		if( strcmp( buf, "#HATCHDELAY" ) == 0 )
		{
			mHatchDelay = file.GetDword();
		}
		
		if( strcmp( buf, "#SUMMONDELAY" ) == 0 )
		{
			mSummonDelay = file.GetDword();
		}

		if( strcmp( buf, "#SEALDELAY" ) == 0 )
		{
			mSealDelay = file.GetDword();
		}

		if( strcmp( buf, "#SUMMONPENALTYDELAY" ) == 0 )
		{
			mSummonPenaltyDelay = file.GetDword();
		}

		if( strcmp( buf, "#RESUMMONPENALTYDELAY" ) == 0 )
		{
			mResummonPenaltyDelay = file.GetDword();
		}
		
		if( strcmp( buf, "#CLOSEDSKILL" ) == 0 )
		{
			mClosedSkillIndex = file.GetDword();
		}

		if( strcmp( buf, "#RANGESKILL" ) == 0 )
		{
			mRangeSkillIndex = file.GetDword();
		}

		if( strcmp( buf, "#FRIENDLYREDUCE" ) == 0 )
		{
			mFriendlyReduceTime = file.GetDword() * 1000 * 60;
			mFriendlyReducePoint = file.GetByte();
		}

		if( strcmp( buf, "#FRIENDLYPENALTY" ) == 0 )
		{
			PET_FRIENDLY_PENALTY* pPenalty = new PET_FRIENDLY_PENALTY;

			pPenalty->Friendly = file.GetByte();
			pPenalty->Penalty = file.GetByte();

			mPetFriendlyPenaltyTable.Add( pPenalty, pPenalty->Friendly );
		}
	}

	file.Release();
	file.Init("System/Resource/PetStatusInfo.bin","rb");
	
	while(FALSE == file.IsEOF())
	{
		BYTE type = file.GetByte();

		PET_STATUS_INFO* pStatusInfo = new PET_STATUS_INFO;

		pStatusInfo->Level	= file.GetByte();
		pStatusInfo->STR	= file.GetDword();
		pStatusInfo->DEX	= file.GetDword();
		pStatusInfo->VIT	= file.GetDword();
		pStatusInfo->INT	= file.GetDword();
		pStatusInfo->WIS	= file.GetDword();

		mPetStatusInfoTable[ type ].Add( pStatusInfo, pStatusInfo->Level );
	}

	file.Release();

	file.Init("System/Resource/PetList.bin","rb");
	
	while(FALSE == file.IsEOF())
	{
		TCHAR text[MAX_PATH] = {0};
		file.GetLine(
			text,
			_countof(text));

		LPCTSTR textSeperator = " \t";
		LPCTSTR textIndex = _tcstok(
			text,
			textSeperator);
		LPCTSTR textName = _tcstok(
			0,
			textSeperator);
		LPCTSTR textModelName = _tcstok(
			0,
			textSeperator);
		LPCTSTR textImage = _tcstok(
			0,
			textSeperator);
		LPCTSTR textSight = _tcstok(
			0,
			textSeperator);
		LPCTSTR textCostume = _tcstok(
			0,
			textSeperator);
		LPCTSTR textStaticItemIndex = _tcstok(
			0,
			textSeperator);
		LPCTSTR textScale = _tcstok(
			0,
			textSeperator);

		PET_INFO* pInfo = new PET_INFO;
		ZeroMemory(
			pInfo,
			sizeof(*pInfo));
		pInfo->Index = _ttoi(
			textIndex ? textIndex : "");

		SafeStrCpy(
			pInfo->Name,
			textName ? textName : "",
			_countof(pInfo->Name));
		SafeStrCpy(
			pInfo->ModelName,
			textModelName,
			_countof(pInfo->ModelName));
		pInfo->Image = _ttoi(
			textImage ? textImage : "");
		pInfo->Sight = _ttoi(
			textSight ? textSight : "");
		pInfo->Costume = BYTE(_ttoi(
			textCostume ? textCostume : ""));
		pInfo->StaticItemIndex = _ttoi(
			textStaticItemIndex ? textStaticItemIndex : "");
		pInfo->Scale = float(_tstof(
			textScale ? textScale : ""));

		mPetInfoTable.Add( pInfo, pInfo->Index );
	}

	file.Release();
	file.Init("System/Resource/PetHPMPInfo.bin","rb");
	
	while(FALSE == file.IsEOF())
	{
		PET_HP_MP_INFO* pInfo = new PET_HP_MP_INFO;

		pInfo->Index	= file.GetByte();
		pInfo->HP		= file.GetByte();
		pInfo->MP		= file.GetByte();

		mPetHPMPInfoTable.Add( pInfo, pInfo->Index );
	}

	file.Release();
}

void CPetManager::UsePetItem( CPlayer* pPlayer, DWORD ItemDBIdx )
{
	CPet* const petObject = GetPet(
		ItemDBIdx);

	if(0 == petObject)
	{
		SummonPet(
			pPlayer,
			ItemDBIdx);
		return;
	}

	SealPet(
		petObject);
}

void CPetManager::AddObject(DWORD itemDbIndex, DWORD objectIndex)		
{
	mPetIndexContainer[itemDbIndex] = objectIndex;
}

void CPetManager::RemoveObject(DWORD itemDbIndex)
{
	mPetIndexContainer.erase(
		itemDbIndex);
}

void CPetManager::NetworkMsgParse( BYTE Protocol, void* pMsg, DWORD dwLength )
{
	MSGBASE* pmsg = ( MSGBASE* )pMsg;

	switch(Protocol)
	{
	case MP_PET_RECALL_SYN:
		{
			CPlayer* pPlayer = ( CPlayer* )g_pUserTable->FindUser( pmsg->dwObjectID );

			if( !pPlayer )
			{
				return;
			}

			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(0 == petObject)
			{
				break;
			}

			PetRecall(
				petObject);
		}
		break;
	case MP_PET_TARGET_SYN:
		{
			CPlayer* pPlayer = ( CPlayer* )g_pUserTable->FindUser( pmsg->dwObjectID );

			if( !pPlayer )
			{
				return;
			}

			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(0 == petObject)
			{
				break;
			}

			MSG_DWORD* pmsg = ( MSG_DWORD* )pMsg;

			SetTarget(
				petObject,
				pmsg->dwData);
		}
		break;
	case MP_PET_ATTACK_SYN:
		{
			CPlayer* pPlayer = ( CPlayer* )g_pUserTable->FindUser( pmsg->dwObjectID );

			if( !pPlayer )
			{
				return;
			}

			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(0 == petObject)
			{
				break;
			}
			else if(FALSE == PetAttack(petObject))
			{
				MSGBASE msg;
				msg.Category = MP_PET;
				msg.Protocol = MP_PET_ATTACK_NACK;
				pPlayer->SendMsg( &msg, sizeof( msg ) ); 
			}
		}
		break;
	case MP_PET_SKILL_SYN:
		{
			CPlayer* pPlayer = ( CPlayer* )g_pUserTable->FindUser( pmsg->dwObjectID );

			if( !pPlayer )
			{
				return;
			}

			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(0 == petObject)
			{
				break;
			}

			MSG_DWORD* pmsg = ( MSG_DWORD* )pMsg;

			if(FALSE == PetSkillUse( petObject, pmsg->dwData ))
			{
				MSGBASE msg;
				msg.Category = MP_PET;
				msg.Protocol = MP_PET_SKILL_NACK;
				pPlayer->SendMsg( &msg, sizeof( msg ) ); 
			}
		}
		break;
	case MP_PET_RESURRECTION_SYN:
		{
			const MSG_DWORD3* pmsg = ( MSG_DWORD3* )pMsg;
			const DWORD playerIndex = pmsg->dwObjectID;
			const DWORD itemDbIndex = pmsg->dwData1;
			const POSTYPE itemPosition = POSTYPE(pmsg->dwData2);
			const DWORD itemIndex = pmsg->dwData3;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(
				playerIndex);

			if(0 == pPlayer)
			{
				break;
			}
			else if(eObjectKind_Player != pPlayer->GetObjectKind())
			{
				break;
			}
			else if(eItemUseSuccess != ITEMMGR->UseItem( pPlayer, itemPosition, itemIndex ))
			{
				break;
			}

			{
				MSG_ITEM_USE_ACK message;
				message.Category	= MP_ITEM;
				message.Protocol	= MP_ITEM_USE_ACK;
				message.TargetPos	= itemPosition;
				message.dwItemIdx	= itemIndex;
				message.eResult		= eItemUseSuccess;
				pPlayer->SendMsg(
					&message,
					sizeof(message));
			}

			const ITEM_INFO* const pItemInfo = ITEMMGR->GetItemInfo(
				itemIndex);

			if(0 == pItemInfo)
			{
				break;
			}

			PET_OBJECT_INFO petObjectInfo = PETMGR->GetObjectInfo(
				itemDbIndex);
			
			switch( pItemInfo->SupplyType )
			{
			case ITEM_KIND_PET_NORMAL_RESURRECTION:
				{
					petObjectInfo.Friendly = 30;
					petObjectInfo.HP = DWORD(petObjectInfo.MAXHP * 0.3f);
					petObjectInfo.MP = DWORD(petObjectInfo.MAXMP * 0.3f);
				}
				break;
			case ITEM_KIND_PET_SPECIAL_RESURRECTION:
				{
					petObjectInfo.Friendly = 100;
					petObjectInfo.HP = petObjectInfo.MAXHP;
					petObjectInfo.MP = petObjectInfo.MAXMP;
				}
				break;
			}

			petObjectInfo.State = ePetState_None;

			PETMGR->Update(
				petObjectInfo);
			PetInfoUpdate(
				playerIndex,
				petObjectInfo.ItemDBIdx,
				petObjectInfo.Level,
				petObjectInfo.Grade,
				petObjectInfo.Type,
				petObjectInfo.SkillSlot,
				petObjectInfo.Exp,
				petObjectInfo.Friendly,
				petObjectInfo.MAXHP,
				petObjectInfo.HP,
				petObjectInfo.MAXMP,
				petObjectInfo.MP,
				petObjectInfo.AI,
				petObjectInfo.State,
				petObjectInfo.FriendlyCheckTime);

			{
				MSG_PET_INFO message;
				ZeroMemory(
					&message,
					sizeof(message));
				message.Category = MP_PET;
				message.Protocol = MP_PET_RESURRECTION_ACK;
				message.PetObjectInfo = petObjectInfo;
				pPlayer->SendMsg(
					&message,
					sizeof(message));
			}

			LogPet(
				petObjectInfo,
				ePetLogRevive);
		}
		break;
	case MP_PET_ITEM_DISCARD_SYN:
		{
			MSG_DWORDBYTE* pmsg = ( MSG_DWORDBYTE* )pMsg;

			CPlayer* pPlayer = ( CPlayer* )g_pUserTable->FindUser( pmsg->dwObjectID );

			if( !pPlayer )
			{
				return;
			}

			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(0 == petObject)
			{
				break;
			}

			ITEMBASE* pItem = petObject->GetWearedItem( pmsg->bData );

			if( !pItem )
			{
				return;
			}

			if( pItem->dwDBIdx != pmsg->dwData )
			{
				return;
			}
			
			ItemDeleteToDB( pmsg->dwData );
			
			LogItemMoney(pmsg->dwObjectID, pPlayer->GetObjectName(), 0, "",
				eLog_ItemDiscard, pPlayer->GetMoney(eItemTable_Inventory), 0, 0,
				pItem->wIconIdx, pItem->dwDBIdx,  pItem->Position, 0,
				pItem->Durability, pPlayer->GetPlayerExpPoint());
			
			petObject->SetWearedItem(
				pmsg->bData,
				0);

			MSG_BYTE2 msg;
			msg.Category = MP_PET;
			msg.Protocol = MP_PET_ITEM_DISCARD_ACK;
			msg.bData1 = pmsg->bData ; 
			msg.bData2 = FALSE ;

			pPlayer->SendMsg( &msg, sizeof( msg ) );
		}
		break;
	case MP_PET_ITEM_MOVE_PLAYER_SYN:
		{
			MSG_PET_ITEM_MOVE_SYN* pmsg = ( MSG_PET_ITEM_MOVE_SYN* )pMsg;

			CPlayer* pPlayer = ( CPlayer* )g_pUserTable->FindUser( pmsg->dwObjectID );

			if( !pPlayer )
			{
				return;
			}

			CPet* const pPet = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(0 == pPet)
			{
				break;
			}

			DWORD PlayerItemDBIdx = 0;
			DWORD PetItemDBIdx  = 0;
			WORD TableIdx = (WORD)GetTableIdxPosition( pmsg->PlayerPos );
			BYTE param = 0;

			if( TableIdx == eItemTable_Storage)
			{
				param = 1;
			}
			else if( TableIdx == eItemTable_Inventory )
			{
				param = 0;
			}
			else
				return;

			PET_INFO* pPetInfo = GetInfo(pPet->GetObjectInfo().Kind);

			if( !pPetInfo->Costume && pmsg->PetPos >= ePetEquipSlot_Face && pmsg->PetPos <= ePetEquipSlot_Tail )
			{
				MSG_WORD msg;
				msg.Category = MP_PET;
				msg.Protocol = MP_PET_ITEM_MOVE_PLAYER_NACK;
				msg.wData = 1;
			
				pPlayer->SendMsg( &msg, sizeof( msg ) );
				return;
			}

			// 090720 ONS 펫고정스킬아이템은 캐릭터 인벤토리로 이동시킬 수 없다.
			if(IsPetStaticSkill(pPetInfo, pmsg->PetPos) && param == 0)
			{
				MSG_WORD msg;
				msg.Category = MP_PET;
				msg.Protocol = MP_PET_ITEM_MOVE_PLAYER_NACK;
				msg.wData = 0;
			
				pPlayer->SendMsg( &msg, sizeof( msg ) );
				return;
			}

			CItemSlot* pPlayerSlot = pPlayer->GetSlot( pmsg->PlayerPos );
			const ITEMBASE* pPlayerItemBase = pPlayerSlot->GetItemInfoAbs( pmsg->PlayerPos );
			ITEMBASE PlayerItem;
			ZeroMemory( &PlayerItem, sizeof( ITEMBASE ) );
			
			ITEMBASE* pPetItemBase = pPet->GetWearedItem( pmsg->PetPos );
			ITEMBASE PetItem;
			ZeroMemory( &PetItem, sizeof( ITEMBASE ) );

			if( pPetItemBase )
			{
				PetItem = *pPetItemBase;
				PetItemDBIdx = PetItem.dwDBIdx;
			}


			// 090202 LYW --- PetManager : 아이템 이동이 Swap인지 단순 추가 삭제인지 구분하여 처리 하도록 한다.

			if( pPlayerItemBase->dwDBIdx )
			{
				PlayerItem.Position = pmsg->PlayerPos ;
				PlayerItemDBIdx = pPlayerItemBase->dwDBIdx ;
			}

			// 아이템 교체 처리를 한다.
			if( pPlayerItemBase->dwDBIdx && PetItem.dwDBIdx )
			{
				if( !pPlayerItemBase || !pPetItemBase ) return ;

				// 090625 pdy 펫 기간제 아이템 착용처리 추가
				// 090713 pdy 조건 채크를 eITEM_TYPE_SEAL로 수정 
				if( eITEM_TYPE_SEAL == pPlayerItemBase->nSealed )
					return ;

				ITEMBASE tempItemBase ;
				memcpy( &tempItemBase, pPlayerItemBase, sizeof(ITEMBASE) ) ;

				// 090204 LYW --- PetManager : 아이템 추가 삭제 처리 버그 수정.
				pPlayerSlot->DeleteItemAbs( pPlayer, pmsg->PlayerPos, &PlayerItem ) ;

				PetItem.Position = pmsg->PlayerPos;
				pPlayerSlot->InsertItemAbs( pPlayer, pmsg->PlayerPos, &PetItem ) ;

				PlayerItem.Position = pmsg->PetPos;
				pPet->SetWearedItem( pmsg->PetPos, &PlayerItem ) ;

				ItemMoveUpdatePetInven( pPlayer->GetID(), pPlayer->GetUserID(), pPet->GetID(), PlayerItemDBIdx, pmsg->PlayerPos, PetItem.dwDBIdx, pmsg->PetPos, param ) ;

				LogItemMoney(	pPlayer->GetID(), 
								pPlayer->GetObjectName(),
								pPet->GetID(), 
								pPet->GetObjectName(),
								eLog_ItemMoveFromPlayerToPet,
								pPlayer->GetMoney(),
								0, 
								0, 
								pmsg->PlayerItemIdx,
								PlayerItemDBIdx,
								pmsg->PlayerPos,
								pmsg->PetPos,
								0, 
								0 ) ;
			}
			// 펫 창에 아이템 추가 처리를 한다.
			else if( pPlayerItemBase->dwDBIdx && !PetItem.dwDBIdx )
			{
				if( !pPlayerItemBase ) return ;

				// 090625 pdy 펫 기간제 아이템 착용처리 추가
				// 090713 pdy 조건 채크를 eITEM_TYPE_SEAL로 수정 
				if( eITEM_TYPE_SEAL == pPlayerItemBase->nSealed )
					return ;

				pPlayerSlot->DeleteItemAbs( pPlayer, pmsg->PlayerPos, &PlayerItem ) ;

				PlayerItem.Position = pmsg->PetPos ;
				pPet->SetWearedItem( pmsg->PetPos, &PlayerItem ) ;

				ItemMoveUpdatePetInven( pPlayer->GetID(), pPlayer->GetUserID(), pPet->GetID(), PlayerItemDBIdx, 0, 0, pmsg->PetPos, param ) ;

				LogItemMoney(	pPlayer->GetID(), 
								pPlayer->GetObjectName(),
								pPet->GetID(), 
								pPet->GetObjectName(),
								eLog_ItemMoveFromPlayerToPet,
								pPlayer->GetMoney(),
								0, 
								0, 
								pmsg->PlayerItemIdx,
								PlayerItemDBIdx,
								pmsg->PlayerPos,
								pmsg->PetPos,
								0, 
								0 ) ;
			}
			// 펫 창에서 아이템 삭제 처리를 한다.
			else if( !pPlayerItemBase->dwDBIdx && PetItem.dwDBIdx )
			{
				if( !pPetItemBase ) return ;

				pPetItemBase->Position = pmsg->PlayerPos ;
				pPlayerSlot->InsertItemAbs( pPlayer, pmsg->PlayerPos, (ITEMBASE*)pPetItemBase ) ;

				pPet->SetWearedItem( pmsg->PetPos, NULL ) ;

				ItemMoveUpdatePetInven( pPlayer->GetID(), pPlayer->GetUserID(), pPet->GetID(), 0, pmsg->PlayerPos, PetItem.dwDBIdx, 0, param ) ;

				LogItemMoney(	pPlayer->GetID(), 
								pPlayer->GetObjectName(),
								pPet->GetID(), 
								pPet->GetObjectName(),
								eLog_ItemMoveFromPetToPlayer,
								pPlayer->GetMoney(),
								0, 
								0, 
								pmsg->PlayerItemIdx,
								PetItem.dwDBIdx,
								pmsg->PetPos,
								pmsg->PlayerPos,
								0, 
								0 ) ;
			}
			else
			{
				return ;
			}

			MSG_PET_ITEM_MOVE_SYN msg;
			msg = *pmsg;
			msg.Protocol = MP_PET_ITEM_MOVE_PLAYER_ACK;
			
			pPlayer->SendMsg( &msg, sizeof( msg ) );
		}
		break;
	}
}

BOOL CPetManager::SetTarget( CPet* pPet, DWORD id )
{
	CObject* pTarget = g_pUserTable->FindUser( id );

	if( !pTarget )
	{
		return FALSE;
	}

	if( pTarget->GetState() == eObjectState_Die )
	{
		return FALSE;
	}

	pPet->SetTarget( id );

	return TRUE;
}

BOOL CPetManager::PetAttack( CPet* pPet )
{
	DWORD id = pPet->GetTarget();

	CObject* pTarget = g_pUserTable->FindUser( id );

	if( !pTarget )
	{
		return FALSE;
	}

	if( pTarget->GetState() == eObjectState_Die )
	{
		return FALSE;
	}

	DWORD skillidx = 0;

	switch( pPet->GetObjectInfo().Type )
	{
	case ePetType_Basic:
	case ePetType_Physic:	skillidx = mClosedSkillIndex;	break;
	case ePetType_Magic:
	case ePetType_Support:	skillidx = mRangeSkillIndex;	break;
	}

	return SKILLMGR->PetAttack( skillidx, pPet, pTarget );
}

BOOL CPetManager::PetSkillUse( CPet* pPet, DWORD idx )
{
	DWORD id = pPet->GetTarget();

	CObject* pTarget = g_pUserTable->FindUser( id );

	if( !pTarget )
	{
		return FALSE;
	}

	if( pTarget->GetState() == eObjectState_Die )
	{
		return FALSE;
	}

	return SKILLMGR->PetAttack( idx, pPet, pTarget );
}

BOOL CPetManager::PetRecall( CPet* pPet )
{
	CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(
		pPet->GetOwnerIndex());

	if(0 == pPlayer)
	{
		return FALSE;
	}
	else if(eObjectKind_Player != pPlayer->GetObjectKind())
	{
		return FALSE;
	}

	VECTOR3* pPlayerPos = CCharMove::GetPosition( pPlayer );
	VECTOR3 pos;

	int ran=0;
	float dx=0,dz=0;
	ran = rand();
	dx = float(ran%mAreaDistance) * (ran%2?1:-1);
	ran = rand();
	dz = float(ran%mAreaDistance) * (ran%2?1:-1);
	pos.x = pPlayerPos->x + dx;
	pos.z = pPlayerPos->z + dz;

	if( pos.x < 0 )				pos.x = 0;
	else if( pos.x > 51199 )	pos.x = 51199;

	if( pos.z < 0 )				pos.z = 0;
	else if( pos.z > 51199 )	pos.z = 51199;

	pos.x = ((DWORD)(pos.x/TILECOLLISON_DETAIL)*TILECOLLISON_DETAIL);
	pos.z = ((DWORD)(pos.z/TILECOLLISON_DETAIL)*TILECOLLISON_DETAIL);

	CCharMove::SetPosition( pPet, &pos );

	MOVE_POS msg;
	msg.Category = MP_MOVE;
	msg.Protocol = MP_MOVE_WARP;
	msg.dwMoverID = pPet->GetID();
	msg.cpos.Compress(&pos);
	
	PACKEDDATA_OBJ->QuickSend(
		pPlayer,
		&msg,
		sizeof(msg));

	return TRUE;
}

BOOL CPetManager::HatchPet( CPlayer* pPlayer, const ITEMBASE* pItem )
{
	const ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo( pItem->wIconIdx );

	BYTE skillslot = 0;

	if( pItemInfo->Shop )
	{
		skillslot = mShopBaseSkillSlot;
	}
	else
	{
		skillslot = (BYTE)random( mMinBaseSkillSlot, mMaxBaseSkillSlot );
	}

	// 090720 ONS 펫 고정스킬과 장착슬롯번호를 추가한다.
	DWORD dwPetKind = pItemInfo->SupplyValue;
	PET_INFO* pPetInfo = GetInfo(dwPetKind);
	if(pPetInfo == NULL)
		return FALSE;

	PetInfoAdd(pPlayer->GetID(), pPlayer->GetUserID(), pItem->dwDBIdx, (WORD)pItemInfo->SupplyValue, skillslot, pPetInfo->StaticItemIndex, ePetEquipSlot_Skill6);
	
	return TRUE;
}

WORD CPetManager::GetMaxEXP( BYTE grade )
{
	if( grade > mMaxGrade )
	{
		return 0;
	}

	return mExp[ grade ];
}

BYTE CPetManager::GetGradeUpLevel( BYTE grade )
{
	if( grade >= mMaxGrade )
	{
		return 0;
	}

	return mGradeUpLevel[ grade ];
}

float CPetManager::GetFriendlyPenalty( CPet* pPet )
{
	float penalty = 1.0f;
	BYTE friendly = pPet->GetObjectInfo().Friendly;

	PET_FRIENDLY_PENALTY* pPenalty = NULL;

	mPetFriendlyPenaltyTable.SetPositionHead();
	while( (pPenalty = mPetFriendlyPenaltyTable.GetData() )!= NULL)
	{
		// 090810 ONS 친밀도에 따른 펫공격 페널티 설정 변경
		if( friendly <= pPenalty->Friendly )
		{
			penalty = ( 100 - pPenalty->Penalty ) * 0.01f;
		}
	}

	return penalty;
}

// 090720 ONS 펫스킬아이템인지 여부를 판단한다.
BOOL CPetManager::IsPetStaticSkill(PET_INFO* pPetInfo, WORD pos)
{
	if((pPetInfo->StaticItemIndex > 0) && (pos == ePetEquipSlot_Skill6))
		return TRUE;
	return FALSE;
}


// 091214 ONS 펫 맵이동시 소환/봉인 관련 메세지를 출력하지 않도록 처리.
// 펫을 아이템 소환한 캐릭터인덱스의 존재유무를 판단한다.
BOOL CPetManager::IsSummonPlayerIndex( DWORD dwIndex )
{
	std::set<DWORD>::iterator iter = m_setSummonPlayerIndex.find(dwIndex);
	if(iter != m_setSummonPlayerIndex.end())
	{
		m_setSummonPlayerIndex.erase(iter);
		return TRUE;
	}
	return FALSE;
}

// 091214 ONS 펫 맵이동시 봉인 관련 메세지를 출력하지 않도록 처리.
void CPetManager::RemovePet( CPet* pPet )
{
	g_pServerSystem->RemovePet( pPet->GetID(), FALSE  );

	CPlayer* const ownerPlayer = (CPlayer*)g_pUserTable->FindUser(
		pPet->GetOwnerIndex());

	if(ownerPlayer &&
		eObjectKind_Player == ownerPlayer->GetObjectKind())
	{
		ownerPlayer->SetPetItemDbIndex(
			0);
	}
}

void CPetManager::Update(const PET_OBJECT_INFO& info)
{
	mPreloadedPetInfoContainer[info.ItemDBIdx] = info;
}

const PET_OBJECT_INFO& CPetManager::GetObjectInfo(DWORD itemDbIndex)
{
	static const PET_OBJECT_INFO emptyInfo = {0};

	CPet* const petObject = GetPet(
		itemDbIndex);

	if(petObject)
	{
		return petObject->GetObjectInfo();
	}

	PetInfoContainer::const_iterator iterator = mPreloadedPetInfoContainer.find(
		itemDbIndex);

	return mPreloadedPetInfoContainer.end() == iterator ? emptyInfo : iterator->second;
}

CPet* CPetManager::GetPet(DWORD itemDbIndex)
{
	const PetIndexContainer::const_iterator iterator = mPetIndexContainer.find(
		itemDbIndex);

	CPet* const petObject = (CPet*)g_pUserTable->FindUser(
		mPetIndexContainer.end() == iterator ? 0 : iterator->second);

	if(0 == petObject)
	{
		return 0;
	}
	else if(eObjectKind_Pet != petObject->GetObjectKind())
	{
		return 0;
	}

	return petObject;
}

void CPetManager::AddPetItemOptionStat( const ITEMBASE& petItemBase, CPet* pPet )
{
	const ITEM_INFO* pItemInfo		=	ITEMMGR->GetItemInfo( petItemBase.wIconIdx );
	const	SetScript::Element&	itemElement	=	GAMERESRCMNGR->GetItemElement( pItemInfo->ItemIdx );

	SetPlayerStat( pPet->GetItemOptionStat(), itemElement.mStat, SetValueTypeAdd );
}

void CPetManager::RemovePetItemOptionStat( const ITEMBASE& petItemBase, CPet* pPet )
{
	const ITEM_INFO* pItemInfo		=	ITEMMGR->GetItemInfo( petItemBase.wIconIdx );
	const	SetScript::Element&	itemElement	=	GAMERESRCMNGR->GetItemElement( pItemInfo->ItemIdx );

	SetPlayerStat( pPet->GetItemOptionStat(), itemElement.mStat, SetValueTypeRemove );
}