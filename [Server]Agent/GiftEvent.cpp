#include ".\giftevent.h"
#include ".\giftmanager.h"
#include "UserTable.h"
#include "Network.h"

cGiftEvent::cGiftEvent(void)
{
	Release();
}

cGiftEvent::~cGiftEvent(void)
{
	Release();
}

BOOL cGiftEvent::Init( DWORD CharacterIdx, DWORD ID, EVENTINFO* pInfo )
{
	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser( CharacterIdx );
	if( !pUserInfo )
	{
		FILE* pFile = fopen( "./log/GiftEventLog.txt", "a+" );
		fprintf( pFile, "%u\tInvalid User(cGiftEvent::Init) - character_idx : %u\n", gCurTime, CharacterIdx );
		fclose( pFile );
		return FALSE;
	}

	mCharacterIdx = CharacterIdx;
	mID = ID;
	mpInfo = pInfo;
	mDelay = pInfo->Delay * 1000;
	mCheckTime = gCurTime;

	MSG_DWORD2 msg;
	msg.Category = MP_ITEM;
	msg.Protocol = MP_ITEM_GIFT_EVENT_NOTIFY;
	msg.dwData1 = mpInfo->Index;
	msg.dwData2 = 1;

	g_Network.Send2User( pUserInfo->dwConnectionIndex, ( char* )&msg, sizeof( msg ) );

	return TRUE;
}

BOOL cGiftEvent::Excute()
{
	// 아이템 지급
	USERINFO* pInfo = g_pUserTableForObjectID->FindUser( mCharacterIdx );
	if( !pInfo )
	{
		return TRUE;
	}

	// 시간 확인
	if( gCurTime - mCheckTime < mDelay )
	{
		mDelay -= gCurTime - mCheckTime;
		mCheckTime = gCurTime;
		return FALSE;
	}

	MSG_DWORD4 msg;
	msg.Category = MP_ITEM;
	msg.Protocol = MP_ITEM_GIFT_EVENT_NOTIFY;
	msg.dwData1 = mCharacterIdx;
	msg.dwData2 = mpInfo->Index;
	msg.dwData3 = mpInfo->Item;
	msg.dwData4 = mpInfo->Count;

	g_Network.Send2Server( pInfo->dwMapServerConnectionIndex, ( char* )&msg, sizeof( msg ) );

	// 다음 이벤트
	GIFTMGR->AddEvent( mCharacterIdx, mpInfo->Next );

	return TRUE;
}

void cGiftEvent::Release()
{
	mID = 0;			
	mCharacterIdx = 0;	

	mpInfo = NULL;			

	mDelay = 0;			
	mCheckTime = 0;		
}
