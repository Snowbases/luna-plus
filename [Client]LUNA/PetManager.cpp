#include "stdafx.h"
#include ".\petmanager.h"
#include "MHFile.h"
#include "objectmanager.h"
#include "AppearanceManager.h"
#include "chatmanager.h"
#include "itemmanager.h"
#include "gamein.h"
#include "PetResurrectionDialog.h"
#include "petstatedialog.h"
#include "petweareddialog.h"
#include "inventoryexdialog.h"
#include "storagedialog.h"
#include "ProgressDialog.h"
#include "QuickManager.h"
#include "WindowIDEnum.h"
#include "interface/cWindowManager.h"
#include "CommonCalcFunc.h"

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
	mPetFriendlyPenaltyTable.Initialize( 1 );
	mPetObjectInfoTable.Initialize( 1 );

	mPetSpeechInfoTable.Initialize( 1 );
	mPetEmoticonInfoTable.Initialize( 1 );
	mFriendlyStateTable.Initialize( 1 );

	mLastSummonTime = 0;

	for( int i = 0; i < ePetType_Max; i++ )
	{
		mPetStatusInfoTable[ i ].Initialize( 20 );
	}

	LoadPetInfo();
}

CPetManager::~CPetManager(void)
{
	SAFE_DELETE_ARRAY(
		mGradeUpLevel);
	SAFE_DELETE_ARRAY(
		mSummonPlayerLevel);
	SAFE_DELETE_ARRAY(
		mExp);
	SAFE_DELETE_ARRAY(
		mExtendedSkillSlot);

	mPetInfoTable.SetPositionHead();

	for(PET_INFO* pInfo = mPetInfoTable.GetData();
		0 < pInfo;
		pInfo = mPetInfoTable.GetData())
	{
		delete pInfo;
	}
	mPetInfoTable.RemoveAll();

	for( int i = 0; i < ePetType_Max; i++ )
	{
		mPetStatusInfoTable[ i ].SetPositionHead();

		for(PET_STATUS_INFO* pStatusInfo = mPetStatusInfoTable[ i ].GetData();
			0 < pStatusInfo;
			pStatusInfo = mPetStatusInfoTable[ i ].GetData())
		{
			delete pStatusInfo;
		}
		mPetStatusInfoTable[ i ].RemoveAll();
	}

	mPetSpeechInfoTable.SetPositionHead();

	for(cPtrList* pList = mPetSpeechInfoTable.GetData();
		0 < pList;
		pList = mPetSpeechInfoTable.GetData())
	{
		PTRLISTPOS pos = pList->GetHeadPosition();
		while(pos)
		{
			char* node = (char *)pList->GetNext(pos);
			SAFE_DELETE_ARRAY(node);
		}
		pList->RemoveAll();

		delete pList;
	}
	mPetSpeechInfoTable.RemoveAll();

	mPetEmoticonInfoTable.SetPositionHead();

	for(cPtrList* pList = mPetEmoticonInfoTable.GetData();
		0 < pList;
		pList = mPetEmoticonInfoTable.GetData())
	{
		PTRLISTPOS pos = pList->GetHeadPosition();
		while(pos)
		{
			DWORD* node = (DWORD *)pList->GetNext(pos);
			SAFE_DELETE(node);
		}
		pList->RemoveAll();

		delete pList;
	}
	mPetEmoticonInfoTable.RemoveAll();

	mFriendlyStateTable.SetPositionHead();

	while(PET_FRIENDLY_STATE* pFriendlyState = mFriendlyStateTable.GetData())
	{
		delete pFriendlyState;
	}

	mFriendlyStateTable.RemoveAll();

	mPetObjectInfoTable.SetPositionHead();

	while(PET_OBJECT_INFO* info = mPetObjectInfoTable.GetData())
	{
		SAFE_DELETE(
			info);
	}

	mPetObjectInfoTable.RemoveAll();
}

void CPetManager::LoadPetInfo()
{
	CMHFile file;
	BOOL	didLoadMaxGrade = FALSE;
	BOOL	didLoadMaxLevel = FALSE;

	file.Init("System/Resource/PetSystemInfo.bin","rb");
	
	char buf[1024]={0,};

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

		if( strcmp( buf, "#HATCHEFFECT" ) == 0 )
		{
			mHatchEffect = file.GetWord();
		}

		if( strcmp( buf, "#SUMMONEFFECT" ) == 0 )
		{
			mSummonEffect = file.GetWord();
		}

		if( strcmp( buf, "#SEALEFFECT" ) == 0 )
		{
			mSealEffect = file.GetWord();
		}

		if( strcmp( buf, "#APPEAREFFECT" ) == 0 )
		{
			mAppearEffect = file.GetWord();
		}

		if( strcmp( buf, "#DISAPPEAREFFECT" ) == 0 )
		{
			mDisappearEffect = file.GetWord();
		}

		if( strcmp( buf, "#LEVELUPEFFECT" ) == 0 )
		{
			mLevelUpEffect = file.GetWord();
		}

		if( strcmp( buf, "#GRADEUPEFFECT" ) == 0 )
		{
			mGradeUpEffect = file.GetWord();
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

		if( strcmp( buf, "#NAMEHEIGHT" ) == 0 )
		{
			mNameHight = file.GetWord();
		}

		if( strcmp( buf, "#FRIENDLYSTATE" ) == 0 )
		{
			BYTE friendly = file.GetByte();

			PET_FRIENDLY_STATE* pFriendlyState = mFriendlyStateTable.GetData( friendly );

			if( !pFriendlyState )
			{
				pFriendlyState = new PET_FRIENDLY_STATE;
				pFriendlyState->Friendly = friendly;
				mFriendlyStateTable.Add( pFriendlyState, friendly );
			}
			
			file.GetString( buf );
		
			if( strcmp( buf, "*SPEACH" ) == 0 )
			{
				pFriendlyState->SpeechIndex = file.GetDword();
				pFriendlyState->SpeechRate = file.GetByte();
			}
			else if( strcmp( buf, "*EMOTICON" ) == 0 )
			{
				pFriendlyState->EmoticonIndex = file.GetDword();
				pFriendlyState->EmoticonRate = file.GetByte();
			}
			else
			{
				//데이터 오류?
			}
		}

		if( strcmp( buf, "#SPEACH" ) == 0 )
		{
			BOOL start = FALSE;
			cPtrList* pList = new cPtrList;

			DWORD index = file.GetDword();

			while(FALSE == file.IsEOF())
			{
				file.GetLine( buf, 1024 );

				if( buf[ 0 ] == '@' )
				{
					continue;
				}

				if( buf[ 0 ] == '{' )
				{
					start = TRUE;
					continue;
				}

				if( buf[ 0 ] == '}' )
				{
					break;
				}

				if( start )
				{
					char* pData = new char[ strlen( buf ) + 1 ];

					strcpy( pData, buf );

					pData[ strlen( buf ) ] = 0;

					pList->AddTail( pData );
				}
			}

			mPetSpeechInfoTable.Add( pList, index );
		}

		if( strcmp( buf, "#EMOTICON" ) == 0 )
		{
			BOOL start = FALSE;
			cPtrList* pList = new cPtrList;

			DWORD index = file.GetDword();

			while(FALSE == file.IsEOF())
			{
				file.GetString( buf );

				if( buf[ 0 ] == '@' )
				{
					continue;
				}

				if( buf[ 0 ] == '{' )
				{
					start = TRUE;
					continue;
				}

				if( buf[ 0 ] == '}' )
				{
					break;
				}

				if( start )
				{
					DWORD* pData = new DWORD;

					*pData = ( DWORD )atoi( buf );

					pList->AddTail( pData );
				}
			}

			mPetEmoticonInfoTable.Add( pList, index );
		}

		if( strcmp( buf, "#ANIMATION" ) == 0 )
		{
			while(FALSE == file.IsEOF())
			{
				file.GetString( buf );

				if( buf[ 0 ] == '@' )
				{
					file.GetLineX( buf, 256 );
					continue;
				}

				if( strcmp( buf, "{" ) == 0 )
				{
					continue;
				}

				if( strcmp( buf, "}" ) == 0 )
				{
					break;
				}

				if( strcmp( buf, "*RUN" ) == 0 )
				{
					mRunAnimation = file.GetByte();
				}

				if( strcmp( buf, "*WALK" ) == 0 )
				{
					mWalkAnimation = file.GetByte();
				}

				if( strcmp( buf, "*IDLE" ) == 0 )
				{
					mIdleAnimation = file.GetByte();
				}

				if( strcmp( buf, "*DAMAGE" ) == 0 )
				{
					mDamageAnimation = file.GetByte();
				}

			}
		}

		if( strcmp( buf, "#CLOSEDSKILL" ) == 0 )
		{
			mClosedSkillIndex = file.GetDword();
		}

		if( strcmp( buf, "#RANGESKILL" ) == 0 )
		{
			mRangeSkillIndex = file.GetDword();
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

		if(_countof(mPetStatusInfoTable) <= type)
		{
			break;
		}

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
			_countof(text),
			FALSE);

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

		TCHAR name[MAX_PATH] = {0};
		_parsingKeywordString(
			textName ? textName : "",
			name);
		SafeStrCpy(
			pInfo->Name,
			name,
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
}

void CPetManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch( Protocol )
	{
	case MP_PET_HATCH:
		{
			MSG_PET_INFO* pmsg = ( MSG_PET_INFO* )pMsg;

			PET_OBJECT_INFO* pObjectInfo = new PET_OBJECT_INFO;

			memcpy( pObjectInfo, &( pmsg->PetObjectInfo ), sizeof( PET_OBJECT_INFO ) );

			mPetObjectInfoTable.Add( pObjectInfo, pObjectInfo->ItemDBIdx );

			PET_INFO* pInfo = mPetInfoTable.GetData( pObjectInfo->Kind );
			CItem* pItem = ITEMMGR->GetItem( pObjectInfo->ItemDBIdx );

			if( !pInfo || !pItem )
				return;

			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1573), pInfo->Name, pObjectInfo->SkillSlot );

			pItem->ForcedUnseal();
			ITEMMGR->RefreshItem( pItem );
		}
		break;
	case MP_PET_EXP_NOTIFY:
		{
			HEROPET->AddExp();
		}
		break;
	case MP_PET_LEVELUP_NOTIFY:
		{
			MSG_DWORD* pmsg = ( MSG_DWORD* )pMsg;

			CPet* pPet = ( CPet* )OBJECTMGR->GetObject( pmsg->dwData );

			if( !pPet )
				return;

			pPet->LevelUp();

			APPEARANCEMGR->InitAppearance( pPet );

			if( pPet == HEROPET )
			{
				HEROPET->CalcStats();
				// 081112 LYW --- PetManager : 펫이 레벨업 했을경우 잘못 출력되던
				// 메시지 번호를 수정함. 1647 ->1659 (정진문)
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1659));
			}
			TARGETSET set;
			set.pTarget = pPet;
			EFFECTMGR->StartEffectProcess(mLevelUpEffect,pPet,&set,0,pPet->GetID());
		}
		break;
	case MP_PET_LEVEL_NOTIFY:
		{
			MSG_DWORDBYTE* pmsg = ( MSG_DWORDBYTE* )pMsg;

			CPet* pPet = ( CPet* )OBJECTMGR->GetObject( pmsg->dwData );

			if( !pPet )
				return;

			pPet->SetLevel( pmsg->bData );

			APPEARANCEMGR->InitAppearance( pPet );

			if( pPet == HEROPET )
			{
				HEROPET->CalcStats();
				// 081112 LYW --- PetManager ; 펫이 레벨업 했을 경우 잘못 출력되던 메시지 번호를 수정함.
				// 메시지 번호를 수정함. 1647 ->1659 (정진문)
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1659));
			}
		}
		break;
	case MP_PET_GRADEUP_NOTIFY:
		{
			MSG_DWORDBYTE2* pmsg = ( MSG_DWORDBYTE2* )pMsg;

			CPet* pPet = ( CPet* )OBJECTMGR->GetObject( pmsg->dwData );

			if( !pPet )
				return;

			pPet->GradeUp();

			APPEARANCEMGR->InitAppearance( pPet );
			
			if( pPet == HEROPET )
			{
				HEROPET->SetType(
					ePetType(pmsg->bData1));
				HEROPET->SetSkillSlot(
					pmsg->bData2);
				HEROPET->CalcStats();
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1576));
				
				CItem* pItem = ITEMMGR->GetItem( HEROPET->GetItemDBIdx() );

				if( pItem )
				{
					ITEMMGR->RefreshItem( pItem );
				}
			}

			TARGETSET set;
			set.pTarget = pPet;
			EFFECTMGR->StartEffectProcess(mGradeUpEffect,pPet,&set,0,pPet->GetID());
		}
		break;
	case MP_PET_FRIENDLY_SYNC:
		{
			MSG_BYTE* pmsg = ( MSG_BYTE* )pMsg;

			HEROPET->SetFriendly( pmsg->bData );
		}
		break;
	case MP_PET_RESURRECTION_ACK:
		{
			MSG_PET_INFO* pmsg = ( MSG_PET_INFO* )pMsg;

			PET_OBJECT_INFO* oldinfo = mPetObjectInfoTable.GetData( pmsg->PetObjectInfo.ItemDBIdx );
			
			if( oldinfo )
			{
				memcpy( oldinfo, &( pmsg->PetObjectInfo ), sizeof( PET_OBJECT_INFO ) );
			}

			CItem* pItem = ITEMMGR->GetItem( pmsg->PetObjectInfo.ItemDBIdx );

			if( pItem )
			{
				ITEMMGR->RefreshItem( pItem );
			}

			CPetResurrectionDialog* window = GAMEIN->GetPetResurrectionDlg();

			if(	window && window->IsActive() )
			{
				window->SetUsedItem( NULL );
				window->SetActive( FALSE );
			}

			QUICKMGR->RefreshQickItem();
		}
		break;
	case MP_PET_LIFE_NOTIFY:
		{
			MSG_INT* pmsg = ( MSG_INT* )pMsg;

			if( HEROPET )
			HEROPET->SetLife( HEROPET->GetLife() + pmsg->nData );
		}
		break;
	case MP_PET_MAXLIFE_NOTIFY:
		{
			MSG_DWORD* pmsg = ( MSG_DWORD* )pMsg;

			CPet*	pPet	=	(CPet*)OBJECTMGR->GetObject( pmsg->dwObjectID );
			if( pPet )
				pPet->SetMaxLife( pmsg->dwData );
		}
		break;

	case MP_PET_MANA_NOTIFY:			
		{
			MSG_DWORD* pmsg = ( MSG_DWORD* )pMsg;

			if( HEROPET )
			HEROPET->SetMana( pmsg->dwData );
		}
		break;
	case MP_PET_MAXMANA_NOTIFY:
		{
			MSG_DWORD* pmsg = ( MSG_DWORD* )pMsg;

			if( HEROPET )
			HEROPET->SetMaxMana( pmsg->dwData );
		}
		break;
	case MP_PET_OBJECT_INFO:
		{
			MSG_PET_INFO* pmsg = ( MSG_PET_INFO* )pMsg;

			PET_OBJECT_INFO* oldinfo = mPetObjectInfoTable.GetData( pmsg->PetObjectInfo.ItemDBIdx );
			
			if( oldinfo )
			{
				memcpy( oldinfo, &( pmsg->PetObjectInfo ), sizeof( PET_OBJECT_INFO ) );
			}
			else
			{
				PET_OBJECT_INFO* pObjectInfo = new PET_OBJECT_INFO;

				memcpy( pObjectInfo, &( pmsg->PetObjectInfo ), sizeof( PET_OBJECT_INFO ) );

				mPetObjectInfoTable.Add( pObjectInfo, pObjectInfo->ItemDBIdx );
			}

			ITEMMGR->RefreshItemToolTip( pmsg->PetObjectInfo.ItemDBIdx );
			// 퀵슬롯이 세팅된 후에 펫 정보가 전송된다. 따라서 사망 등의 정보가 변경되려면 재호출한다
			QUICKMGR->RefreshQickItem();
		}
		break;

	case MP_PET_DIE_NOTIFY:
		{
			MSG_DWORD* pmsg = ( MSG_DWORD* )pMsg;
			
			PET_OBJECT_INFO* oldinfo = mPetObjectInfoTable.GetData( pmsg->dwData );
			if( oldinfo )
			{
				oldinfo->HP = 0;
				oldinfo->MP = 0;
				oldinfo->Friendly = 0;
				oldinfo->State = ePetState_Die;
			}

			CItem* pItem = ITEMMGR->GetItem( pmsg->dwData );

			if( pItem )
			{
				ITEMMGR->RefreshItem( pItem );
			}

			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1578));

			// 081112 LYW --- PetManager : 펫이 사망하였을 경우 펫 ui를 닫도록 수정함.
			cDialog* pDlg = GAMEIN->GetPetUIDlg() ;
			if( pDlg )
			{
				pDlg->SetActive( FALSE ) ;
			}

			// 090212 NYJ - 봉인중 죽어버리면 봉인취소
			CProgressDialog* pProgressDlg = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
			if(pProgressDlg && pProgressDlg->GetActionType()==CProgressDialog::eActionPetSummon)
			{
				pProgressDlg->Cancel();
			}

			QUICKMGR->RefreshQickItem();
		}
		break;
	case MP_PET_ITEM_INFO:
		{
			MSG_PET_ITEM_INFO* pmsg = ( MSG_PET_ITEM_INFO* )pMsg;

			CPet* pPet = ( CPet* )OBJECTMGR->GetObject( pmsg->ID );

			if( pPet )
			{
				pPet->SetWearedItem( pmsg->Item );
				APPEARANCEMGR->InitAppearance( pPet );
			}
		}
		break;

	case MP_PET_ITEM_DISCARD_ACK:
		{
			MSG_BYTE2* pmsg = ( MSG_BYTE2* )pMsg;

			//시간 소모로 삭제 되었다면 TRUE
			if( pmsg->bData2 )
			{
				CPetWearedDialog* pDlg = GAMEIN->GetPetWearedDlg();

				CItem* pItem = (pDlg) ? (CItem*)pDlg->GetIconForIdx( pmsg->bData1 ) : NULL ;

				if( pItem )
				{
					CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1192 ), CHATMGR->GetChatMsg( 1926 ), pItem->GetItemInfo()->ItemName );
				}
			}

			GAMEIN->GetPetWearedDlg()->DeleteAck( pmsg->bData1 );
		}
		break;
	case MP_PET_ITEM_MOVE_PLAYER_ACK:
		{
			MSG_PET_ITEM_MOVE_SYN* pmsg = ( MSG_PET_ITEM_MOVE_SYN* )pMsg;

			CInventoryExDialog* pPlayerInven = GAMEIN->GetInventoryDialog();
			CStorageDialog* pPlayerStorage = GAMEIN->GetStorageDialog();
			CPetWearedDialog* pPetInven = GAMEIN->GetPetWearedDlg();

			CItem* pPlayerItem = NULL;
			const eITEMTABLE TableIdx = ITEMMGR->GetTableIdxForAbsPos( pmsg->PlayerPos );
			
			if( TableIdx == eItemTable_Storage)
			{
				pPlayerStorage->DeleteItem( pmsg->PlayerPos, &pPlayerItem );
			}
			else if( TableIdx == eItemTable_Inventory )
			{
				pPlayerInven->DeleteItem( pmsg->PlayerPos, &pPlayerItem );
			}
			else
				return;


			cIcon* pIcon = NULL;
			CItem* pPetItem = NULL;
			pPetInven->DeleteItem( pmsg->PetPos, &pIcon );
			pPetItem = ( CItem* )pIcon;

			if( pPlayerItem )
			{
				pPlayerItem->SetPosition( pmsg->PetPos );
				pPetInven->AddItem( pmsg->PetPos, pPlayerItem );
			}

			if( pPetItem )
			{
				pPetItem->SetPosition( pmsg->PlayerPos );

				if( TableIdx == eItemTable_Storage)
				{
					pPlayerStorage->AddItem( pPetItem );
				}
				else if( TableIdx == eItemTable_Inventory )
				{
					pPlayerInven->AddItem( pPetItem );
				}
				else
					return;
			}
		}
		break;
	case MP_PET_ITEM_MOVE_PLAYER_NACK:
		{
			MSG_WORD* msg = (MSG_WORD*)pMsg;

			switch(msg->wData)
			{
			case 1:
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1739));
				break;
			}
		}
		break;
	// 090625 pdy 펫 기간제 아이템 착용처리 추가
	case MP_PET_ITEM_TIMELIMT_ONEMINUTE_FROM_PETINVEN:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg ;

			ITEM_INFO* pItemInfo = NULL ;
			pItemInfo = ITEMMGR->GetItemInfo( pmsg->dwData1 ) ;
			if( !pItemInfo ) return ;

			POSTYPE pos = (POSTYPE)pmsg->dwData2 ;

			CPetWearedDialog* pDlg = GAMEIN->GetPetWearedDlg();

			CItem* pItem= (pDlg) ? (CItem*)pDlg->GetIconForIdx(pos) : NULL ;

			const ITEMBASE* pItemBase = (pItem) ? &pItem->GetItemBaseInfo() : NULL ;
			
			if( pItemBase )
			{
				CItem* pItem = NULL ;
				pItem = ITEMMGR->GetItem( pItemBase->dwDBIdx ) ;

				if( pItem )
				{
					pItem->SetItemBaseInfo( *pItemBase ) ;
					pItem->SetLastCheckTime(gCurTime) ;
					pItem->SetRemainTime( 50 ) ;
				}
			}

			CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1191 ), CHATMGR->GetChatMsg( 1926 ), pItemInfo->ItemName ) ;
		}
		break;

	case MP_PET_LIFE_BROAD:
		{
			const MSG_INT* const msg	=	(MSG_INT*)pMsg;
			CPet*	pPet	=	(CPet*)OBJECTMGR->GetObject( msg->dwObjectID );
			if( pPet )
			{
				DWORD	dwNewLife	=	pPet->GetLife() + msg->nData;
				pPet->SetLife( dwNewLife );
			}
		}
		break;
	}
}

float CPetManager::GetFriendlyPenalty( BYTE friendly  )
{
	float penalty = 1.0f;

	mPetFriendlyPenaltyTable.SetPositionHead();

	for(PET_FRIENDLY_PENALTY* pPenalty = mPetFriendlyPenaltyTable.GetData();
		0 < pPenalty;
		pPenalty = mPetFriendlyPenaltyTable.GetData())
	{
		if( friendly <= pPenalty->Friendly )
		{
			penalty = ( 100 - pPenalty->Penalty )* 0.01f;
		}
	}

	return penalty;
}

void CPetManager::AddPetItemOptionStat( const ITEMBASE& petItemBase )
{
	const ITEM_INFO* pItemInfo		=	ITEMMGR->GetItemInfo( petItemBase.wIconIdx );
	const	SetScript::Element&	itemElement	=	GAMERESRCMNGR->GetItemElement( pItemInfo->ItemIdx );

	SetPlayerStat( HEROPET->GetItemOptionStat(), itemElement.mStat, SetValueTypeAdd );
}

void CPetManager::RemovePetItemOptionStat( const ITEMBASE& petItemBase )
{
	const ITEM_INFO* pItemInfo		=	ITEMMGR->GetItemInfo( petItemBase.wIconIdx );
	const	SetScript::Element&	itemElement	=	GAMERESRCMNGR->GetItemElement( pItemInfo->ItemIdx );

	SetPlayerStat( HEROPET->GetItemOptionStat(), itemElement.mStat, SetValueTypeRemove );
}
