#include ".\giftmanager.h"
#include ".\giftevent.h"
#include "MHFile.h"
#include "Network.h"

GLOBALTON( cGiftManager )

cGiftManager::cGiftManager(void)
{
	mEventInfoTable.Initialize( 3 );

	mpGiftEventPool = new CPool<cGiftEvent>;
	mpGiftEventPool->Init( 300, 100, "GiftEventPool" );	
	mGiftEventTable.Initialize( 300 );
	mIDCreater.Init( 10000, 1 );		

	mStartEventCount = 0;
	mStartEventIndex = NULL;
}

cGiftManager::~cGiftManager(void)
{
	EVENTINFO* pInfo = NULL;

	mEventInfoTable.SetPositionHead();
	while( (pInfo = mEventInfoTable.GetData()) != NULL )
	{
		delete pInfo;
	}
	mEventInfoTable.RemoveAll();

	cGiftEvent* pEvent = NULL;

	mGiftEventTable.SetPositionHead();
	while( (pEvent = mGiftEventTable.GetData()) != NULL )
	{
		mpGiftEventPool->Free( pEvent );
	}
	mGiftEventTable.RemoveAll();

	mpGiftEventPool->Release();	
	delete mpGiftEventPool;

	mIDCreater.Release();		

	mStartEventCount = 0;
	delete[] mStartEventIndex;
}

void cGiftManager::Init()
{
	CMHFile file;
	if(!file.Init("System/Resource/Event_get_list.bin", "rb"))
		return;

	char Token[256];

	while(1)
	{
		if(file.IsEOF())
			break;

		file.GetString( Token );
		if( strcmp(Token,"#StartEventCount") == 0 )
		{
			mStartEventCount = file.GetDword();
			mStartEventIndex = new DWORD[mStartEventCount];
		}
		if( strcmp(Token,"#StartEventIndex") == 0 )
		{
			for( DWORD count = 0; count < mStartEventCount; count++ )
			{
				mStartEventIndex[ count ] = file.GetDword();
			}
		}
		if( strcmp(Token,"#EventInfo") == 0 )
		{
			while(1)
			{
				if(file.IsEOF())
					break;

				EVENTINFO* pInfo = new EVENTINFO;

				pInfo->Index = file.GetDword();
				pInfo->Delay = file.GetDword();
				pInfo->Next  = file.GetDword();
				pInfo->Item  = file.GetDword();
				pInfo->Count = file.GetDword();

				file.GetString();
				file.GetStringInQuotation();

				mEventInfoTable.Add( pInfo, pInfo->Index );
			}
		}
	}
}

void cGiftManager::Release()
{
}

void cGiftManager::AddEvent( DWORD CharacterIdx, DWORD EventIdx )
{
	EVENTINFO* pInfo = mEventInfoTable.GetData( EventIdx );

	if( !pInfo )
	{
		FILE* pFile = fopen( "./log/GiftEventLog.txt", "a+" );
		fprintf( pFile, "%u\tInvalid Data(cGiftManager::AddEvent) - event_idx : %u\n", gCurTime, EventIdx );
		fclose( pFile );
		return;
	}
	cGiftEvent* pEvent = mpGiftEventPool->Alloc();
	DWORD id = mIDCreater.GenerateIndex();

	if( pEvent->Init( CharacterIdx, id, pInfo ) )
	{
		mGiftEventTable.Add( pEvent, pEvent->GetID() );
	}
	else
	{
		mIDCreater.ReleaseIndex( id );
		pEvent->Release();
		mpGiftEventPool->Free( pEvent );
	}
}

void cGiftManager::RemoveEvent( DWORD EventID )
{
	mGiftEventTable.Remove( EventID );
}

void cGiftManager::ExcuteEvent( DWORD CharacterIdx, DWORD EventIdx )
{
	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser( CharacterIdx );
	if( !pUserInfo ) 
	{
		FILE* pFile = fopen( "./log/GiftEventLog.txt", "a+" );
		fprintf( pFile, "%u\tInvalid User(cGiftManager::ExcuteEvent) - character_idx : %u\n", gCurTime, CharacterIdx );
		fclose( pFile );
		return;
	}

	EVENTINFO* pInfo = mEventInfoTable.GetData( EventIdx );

	if( !pInfo )
	{
		FILE* pFile = fopen( "./log/GiftEventLog.txt", "a+" );
		fprintf( pFile, "%u\tInvalid Data(cGiftManager::ExcuteEvent) - event_idx : %u\n", gCurTime, EventIdx );
		fclose( pFile );
		return;
	}

	MSG_DWORD4 msg;
	msg.Category = MP_ITEM;
	msg.Protocol = MP_ITEM_GIFT_EVENT_NOTIFY;
	msg.dwData1 = pUserInfo->dwCharacterID;
	msg.dwData2 = pInfo->Index;
	msg.dwData3 = pInfo->Item;
	msg.dwData4 = pInfo->Count;

	g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, ( char* )&msg, sizeof( msg ) );
}

void cGiftManager::Process()
{
	cGiftEvent* pEvent = NULL;

	mGiftEventTable.SetPositionHead();
	while( (pEvent = mGiftEventTable.GetData()) != NULL )
	{
		if( pEvent->Excute() )
		{
			RemoveEvent( pEvent->GetID() );
			mIDCreater.ReleaseIndex( pEvent->GetID() );
			pEvent->Release();
			mpGiftEventPool->Free( pEvent );
		}
	}
}