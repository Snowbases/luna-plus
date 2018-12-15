#include "stdafx.h"
#include ".\facialmanager.h"

#include "ObjectManager.h"

// desc_hseos_¾ó±¼±³Ã¼01
// S ¾ó±¼±³Ã¼ Ãß°¡ added by hseos 2007.06.08
#include "MHFile.h"
// E ¾ó±¼±³Ã¼ Ãß°¡ added by hseos 2007.06.08

GLOBALTON(CFacialManager)

// desc_hseos_¾ó±¼±³Ã¼01
// S ¾ó±¼±³Ã¼ Ãß°¡ added by hseos 2007.06.08
DWORD CFacialManager::PLAYER_FACE_EYE_BLINK_OPEN_TIME			= 4000;			
DWORD CFacialManager::PLAYER_FACE_EYE_BLINK_OPEN_ADD_RND_TIME	= 1000;
DWORD CFacialManager::PLAYER_FACE_EYE_BLINK_CLOSE_TIME			= 90;
DWORD CFacialManager::PLAYER_FACE_EYE_BLINK_CLOSE_ADD_RND_TIME	= 0;
DWORD CFacialManager::PLAYER_FACE_SHAPE_TIME					= 3000;
// E ¾ó±¼±³Ã¼ Ãß°¡ added by hseos 2007.06.08

CFacialManager::CFacialManager(void)
{
	// desc_hseos_¾ó±¼±³Ã¼01
	// S ¾ó±¼±³Ã¼ Ãß°¡ added by hseos 2007.06.08
	LoadScriptFileData();
	// E ¾ó±¼±³Ã¼ Ãß°¡ added by hseos 2007.06.08
}

CFacialManager::~CFacialManager(void)
{
}

// desc_hseos_¾ó±¼±³Ã¼01
// S ¾ó±¼±³Ã¼ Ãß°¡ added by hseos 2007.06.08
BOOL CFacialManager::LoadScriptFileData()
{
	CMHFile fp;

	char szLine[MAX_PATH];
	int nKind = 0;

	fp.Init("./System/Resource/FaceAction.bin", "rb");
	if(!fp.IsInited())
	{
		return FALSE;
	}

	while(TRUE)
	{
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//")) continue;			
		else if (strstr(szLine, "END_KIND")) 
		{
			nKind++;
			continue;
		}

		switch(nKind)
		{
		// ´«±ôºýÀÓ ½Ã°£
		case 0: 
			sscanf(szLine, "%d %d %d %d", &PLAYER_FACE_EYE_BLINK_OPEN_TIME,
										  &PLAYER_FACE_EYE_BLINK_OPEN_ADD_RND_TIME, 
										  &PLAYER_FACE_EYE_BLINK_CLOSE_TIME, 
										  &PLAYER_FACE_EYE_BLINK_CLOSE_ADD_RND_TIME);
			break;
		// Ã¤ÆÃ´Ü¾î Ç¥Á¤ Áö¼Ó ½Ã°£
		case 1:
			sscanf(szLine, "%d", &PLAYER_FACE_SHAPE_TIME);
			break;
		}

		if (fp.IsEOF()) break;
	}

	fp.Release();

	return TRUE;
}
// E ¾ó±¼±³Ã¼ Ãß°¡ added by hseos 2007.06.08

void CFacialManager::ChangeHeroFace(BYTE faceNum) 
{
	HERO->ChangeFace( faceNum ) ;  

	SendToServerFace( faceNum ) ;
}

void CFacialManager::ChangePlayerFace(CPlayer* pPlayer, BYTE faceNum)
{
	pPlayer->ChangeFace( faceNum ) ;
}

void CFacialManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch( Protocol )
	{
	case MP_FACIAL_NORMAL :
		{
			MSG_BYTE* data = ( MSG_BYTE* )pMsg ;

			CPlayer* pPlayer = ( CPlayer* )OBJECTMGR->GetObject( data->dwObjectID ) ;

			if( !pPlayer ) return ;

			// desc_hseos_¾ó±¼±³Ã¼01
			// S ¾ó±¼±³Ã¼ Ãß°¡ added by hseos 2007.06.08
			// ChangePlayerFace( pPlayer, data->bData ) ;
			pPlayer->SetFaceAction(FACE_ACTION_KIND_EYE_CHATTING_CMD, data->bData);
			// E ¾ó±¼±³Ã¼ Ãß°¡ added by hseos 2007.06.08
		}
		break ;
	}
}

void CFacialManager::CloseHeroEye()
{
	HERO->CloseEye() ;
}

void CFacialManager::SendToServerFace(BYTE faceNum)
{
	MSG_BYTE data ;

	data.Category	= MP_FACIAL ;
	data.Protocol	= MP_FACIAL_NORMAL ;
	data.dwObjectID	= HEROID ;
	data.bData		= faceNum ;
	
	NETWORK->Send(&data, sizeof(MSG_BYTE));
}