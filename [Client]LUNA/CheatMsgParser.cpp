#include "stdafx.h"
#include "ChatManager.h"
#include "CheatMsgParser.h"
#include "objectmanager.h"
#include "Movemanager.h"
#include "ItemManager.h"
#include "GameIn.h"
#include "MAINGAME.h"
#include "CharacterCalcManager.h"
#include "ObjectStateManager.h"
#include "./Audio/MHAudioManager.h"
#include "FilteringTable.h"
#ifdef _GMTOOL_
#include "GMToolManager.h"
#include "MHCamera.h"
#endif
#include "ObjectBalloon.h"
#include "mhfile.h"
#include "InventoryExDialog.h"
#include "WindowIdEnum.h"
#include "./Interface/cWindowManager.h"
#include "cDivideBox.h"
#include "QuickManager.h"
#include "UserInfoManager.h"

#include "MapChange.h"
#include "GMNotifyManager.h"
#include "GuildUnion.h"
#include "MHMap.h"
#include "CharacterDialog.h"
#include "Party.h"
#include "NpcImageDlg.h"
#include "DealDialog.h"
#include "ShowdownManager.h"
#include "FacialManager.h"
#include "PartyManager.h"
#include "cMsgBox.h"
#include "../hseos/Monstermeter/SHMonstermeterDlg.h"
#include "../hseos/Debug/SHDebug.h"
#include "cSkillTrainingDlg.h"
#include "FamilyCreateDialog.h"
#include "FamilyDialog.h"
#include "FamilyNickNameDialog.h"
#include "cSkillTreeManager.h"
#include "GlobalEventFunc.h"
#include "GuildMarkDialog.h"
#include "AutoNoteManager.h"
#include "OptionManager.h"
#include "cHousingMgr.h"
#include "MHTimeManager.h"
#include "VehicleManager.h"
#include "ChatRoomMgr.h"
#include "Questquickviewdialog.h"
#include "QuestDialog.h"
#include "QuestManager.h"

GLOBALTON(CCheatMsgParser)

#ifdef _CHEATENABLE_

BOOL CheatFunc(char* cheat)
{
	char code[256];
	char buf[256] = {0,};
	sscanf(cheat,"%s",code);
	strupr(code);

#if defined(_GMTOOL_)
	//if( strcmp(code,"MONEYBEGETSMONEY") == 0 )
	if( strcmp(code,"AA") == 0 )
	{
		CHEATMGR->SetCheatEnable( TRUE );
		
		CHATMGR->AddMsg( CTC_CHEAT_1, "01001010010101110100101010101010010010011" );
		CHATMGR->AddMsg( CTC_CHEAT_1, "01001 [ Welcome To the Real World ] 11011" );
		CHATMGR->AddMsg( CTC_CHEAT_1, "11001101010100111010100001010010101111011" );
		
		return TRUE;
	}
	// 071106 LYW --- CheatMsgParser : Add a routine to check command to setting enable cheat mode.
	else if( strcmp( code, "ㅁㅁ") == 0 )
	{
		CHEATMGR->SetCheatEnable( TRUE );
		
		CHATMGR->AddMsg( CTC_CHEAT_1, "01001010010101110100101010101010010010011" );
		CHATMGR->AddMsg( CTC_CHEAT_1, "01001 [ Welcome To the Real World ] 11011" );
		CHATMGR->AddMsg( CTC_CHEAT_1, "11001101010100111010100001010010101111011" );
		
		return TRUE;
	}
#endif

	CSHDebug::ParseTextInput(cheat, code);
	
	if( !CHEATMGR->IsCheatEnable() ) return FALSE;


	if(strcmp(code,"===")==0)
	{
		CObject* pObj = OBJECTMGR->GetSelectedObject();
		if( pObj == NULL ) return TRUE;

		if( !( pObj->GetObjectKind() & eObjectKind_Monster ) &&
			( pObj->GetObjectKind() != eObjectKind_Npc ) )
			return TRUE;

		if( strlen(cheat) > 4 )
		{
			TESTMSGID msg;
			msg.Category = MP_CHEAT;
			msg.Protocol = MP_CHEAT_MOB_CHAT;
			msg.dwSenderID = pObj->GetID();
			SafeStrCpy( msg.Msg, cheat+4, MAX_CHAT_LENGTH+1 );
			NETWORK->Send(&msg,msg.GetMsgLength());
		}

		return TRUE;
	}

//---KES CHEAT PKEVENT
	else if( strcmp(code, "PKEVENT" )==0 )
	{
		int on;
		sscanf(cheat,"%s %d",code,&on);
		if( on != 0 && on !=1 ) return TRUE;

		MSG_BYTE msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_PKEVENT;
		msg.dwObjectID	= HEROID;
		msg.bData		= (BYTE)on;
		NETWORK->Send(&msg,sizeof(msg));

		return TRUE;
	}
//--------------
//---KES CHEAT CHANGESIZE
	else if( strcmp(code, "CHANGESIZE" )==0 )
	{
		CObject* pObject = OBJECTMGR->GetSelectedObject();
		
		if( pObject->GetObjectKind() == eObjectKind_Player )
		{
			DWORD size;
			sscanf(cheat, "%s %u",code,&size);
			if( size > 300 || size < 30 ) return TRUE;

			MSG_DWORD2 msg;
			msg.Category	= MP_CHEAT;
			msg.Protocol	= MP_CHEAT_CHANGESIZE;
			msg.dwObjectID	= HEROID;
			msg.dwData1		= pObject->GetID();
			msg.dwData2		= size;
			NETWORK->Send(&msg,sizeof(msg));
		}

		return TRUE;
	}
	else if( strcmp( code, "RETURNTO" )== 0 )
	{
		int point;
		sscanf(cheat,"%s %d",code,&point);

		MSG_WORD msg;
		msg.dwObjectID = HEROID;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_RETURN_SYN;
		msg.wData = (WORD)point;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if( strcmp( code, "RETURN" )== 0 )
	{
		WORD point;

		point = HERO->GetCharacterTotalInfo()->LoginPoint_Index;
		MSG_WORD msg;
		msg.dwObjectID = HEROID;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_RETURN_SYN;
		msg.wData = point;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code,"CAMERAMODE")==0)
	{
		int onoff;
		sscanf(cheat,"%s %d",code,&onoff);

		if(onoff)
			HERO->GetEngineObject()->Show();
		else
			HERO->GetEngineObject()->HideWithScheduling();

		MSG_DWORD2 msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_SETVISIBLE;
		msg.dwObjectID = HEROID;
		msg.dwData1 = HEROID;
		msg.dwData2 = (DWORD)onoff;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code,"MAP")==0)
	{
		int mapnum;
		sscanf(cheat,"%s %d",code,&mapnum);

		MSG_NAME_DWORD2 msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_CHANGEMAP_SYN;
		msg.dwObjectID	= HEROID;
		msg.dwData1		= (DWORD)mapnum;
		msg.dwData2		= (DWORD)gChannelNum;
		msg.Name[0]		= 0;		
		NETWORK->Send( &msg, sizeof(msg) );

		return TRUE;
	}
	else if(strcmp(code,"RELOAD")==0)
	{
		MSGBASE msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_RELOADING;
		msg.dwObjectID = HEROID;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code,"ITEM")==0)
	{
		int item;
		sscanf(cheat,"%s %d",code,&item);

		MSG_WORD2 msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_ITEM_SYN;
		msg.dwObjectID = HEROID;
		msg.wData1 = (WORD)item;
		msg.wData2 = 1;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code,"ITEMS")==0)
	{
		int item;
		int num;
		sscanf(cheat,"%s %d %d",code,&item, &num);

		MSG_WORD2 msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_ITEM_SYN;
		msg.dwObjectID = HEROID;
		msg.wData1 = (WORD)item;
		msg.wData2 = (WORD)num;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code, "MANA") == 0)
	{
		int mp;
		sscanf(cheat,"%s %d",code,&mp);

		MSG_WORD msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_MANA_SYN;
		msg.dwObjectID = HEROID;
		msg.wData = (WORD)mp;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;

	}
	else if(strcmp(code, "SKILLLEVEL") == 0)
	{
		int skill = 0;
		int level = 0;
		
		sscanf(cheat,"%s %d %d",code,&skill,&level);

		MSG_WORD2 msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_UPDATE_SKILL_SYN;
		msg.dwObjectID = HEROID;
		msg.wData1 = (WORD)skill;
		msg.wData2 = (WORD)level;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code, "MONEY") == 0)
	{
#ifdef _GMTOOL_
		if( GMTOOLMGR->GetGMPower() != eGM_POWER_MASTER )
			return TRUE;
#endif

		DWORD money;
		sscanf(cheat,"%s %d",code,&money);

		MSG_DWORD msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_MONEY_SYN;
		msg.dwObjectID = HEROID;
		msg.dwData = money;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code, "CLEARSTATE") == 0)
	{
		OBJECTSTATEMGR->EndObjectState(HERO, HERO->GetState());
		
		return TRUE;
	}
	else if(strcmp(code, "LEVELUP") == 0)
	{
		int val = 1;
		sscanf(cheat,"%s %d",code,&val);
		if(val <= HERO->GetLevel())
			val = HERO->GetLevel()+1;
		MSG_WORD msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_LEVELUP_SYN;
		msg.dwObjectID = HEROID;
		msg.wData = (WORD)val;
		NETWORK->Send(&msg,sizeof(msg));
		
		return TRUE;
	}
	else if(strcmp(code, "STR") == 0)
	{
		int val = 0;
		sscanf(cheat,"%s %d",code,&val);
		if(val == 0)
			return TRUE;
		if((val < 1) || (val > 9999))
			val = 1;
		MSG_WORD msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_STR_SYN;
		msg.dwObjectID = HEROID;
		msg.wData = (WORD)val;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code, "DEX") == 0)
	{
		int val = 0;
		sscanf(cheat,"%s %d",code,&val);
		if(val == 0)
			return TRUE;
		if((val < 1) || (val > 9999))
			val = 1;
		MSG_WORD msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_DEX_SYN;
		msg.dwObjectID = HEROID;
		msg.wData = (WORD)val;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code, "VIT") == 0)
	{
		int val = 0;
		sscanf(cheat,"%s %d",code,&val);
		if(val == 0)
			return TRUE;
		if((val < 1) || (val > 9999))
			val = 1;
		MSG_WORD msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_VIT_SYN;
		msg.dwObjectID = HEROID;
		msg.wData = (WORD)val;
		NETWORK->Send(&msg,sizeof(msg));

		return TRUE;
	}
	else if(strcmp(code, "WIS") == 0)
	{
		int val = 0;
		sscanf(cheat,"%s %d",code,&val);
		if(val == 0)
			return TRUE;
		if((val < 1) || (val > 9999))
			val = 1;
		MSG_WORD msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_WIS_SYN;
		msg.dwObjectID = HEROID;
		msg.wData = (WORD)val;
		NETWORK->Send(&msg,sizeof(msg));

		return TRUE;
	}
	else if(strcmp(code, "INT") == 0)
	{
		int val = 0;
		sscanf(cheat,"%s %d",code,&val);
		if(val == 0)
			return TRUE;
		if((val < 1) || (val > 9999))
			val = 1;
		MSG_WORD msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_INT_SYN;
		msg.dwObjectID = HEROID;
		msg.wData = (WORD)val;
		NETWORK->Send(&msg,sizeof(msg));

		return TRUE;
	}	else if( strcmp(code, "MOVETO") == 0 )
	{
		sscanf(cheat, "%s %s", code, buf);
		//buf-A³¸?AI¸i

		if( strncmp( HERO->GetObjectName(), buf, MAX_NAME_LENGTH+1 ) == 0  )	//AU±a AU½A
			return TRUE;

		MSG_NAME msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_MOVETOCHAR_SYN;
		msg.dwObjectID	= HEROID;
		SafeStrCpy( msg.Name, buf, MAX_NAME_LENGTH + 1 );
		NETWORK->Send(&msg, sizeof(msg));
		return TRUE;
	}
	else if( strcmp(code, "WHEREIS") == 0 )
	{
		sscanf(cheat,"%s %s", code, buf);

//		if( strncmp( HERO->GetObjectName(), buf, MAX_NAME_LENGTH+1 ) == 0  )	//AU±a AU½A
//			return TRUE;
		if( (FILTERTABLE->IsInvalidCharInclude((unsigned char*)buf)) == TRUE )
		{
			return TRUE;
		}
		MSG_NAME msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_WHEREIS_SYN;
		msg.dwObjectID	= HEROID;
		SafeStrCpy( msg.Name, buf, MAX_NAME_LENGTH+1 );
		
		NETWORK->Send(&msg,sizeof(msg));

		return TRUE;
	}
	else if( strcmp( code, "BAN") == 0 )
	{
		sscanf(cheat, "%s %s", code, buf);
		//buf-A³¸?AI¸i

		if( strncmp( HERO->GetObjectName(), buf, MAX_NAME_LENGTH+1 ) == 0  )	//AU±a AU½A
			return TRUE;
		if( (FILTERTABLE->IsInvalidCharInclude((unsigned char*)buf)) == TRUE )
			return TRUE;

		MSG_NAME msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_BANCHARACTER_SYN;
		msg.dwObjectID	= HEROID;
		SafeStrCpy( msg.Name, buf, MAX_NAME_LENGTH+1 );

		NETWORK->Send(&msg,sizeof(msg));
		
		return TRUE;
	}
	else if( strcmp( code, "BLOCK") == 0 )
	{
		sscanf(cheat, "%s %s", code, buf);
		if( strncmp( HERO->GetObjectName(), buf, MAX_NAME_LENGTH+1 ) == 0  )	//AU±a AU½A
			return TRUE;

		MSG_NAME_DWORD msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_BLOCKCHARACTER_SYN;
		msg.dwObjectID	= HEROID;
		msg.dwData		= 1;	//block
		SafeStrCpy( msg.Name, buf, MAX_NAME_LENGTH+1 );

		NETWORK->Send(&msg,sizeof(msg));

		return TRUE;
	}
	else if( strcmp( code, "NONBLOCK" ) == 0 )
	{
		sscanf(cheat, "%s %s", code, buf);
		if( strncmp( HERO->GetObjectName(), buf, MAX_NAME_LENGTH+1 ) == 0  )	//AU±a AU½A
			return TRUE;

		MSG_NAME_DWORD msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_BLOCKCHARACTER_SYN;
		msg.dwObjectID	= HEROID;
		msg.dwData		= 0;	//nonblock
		SafeStrCpy( msg.Name, buf, MAX_NAME_LENGTH+1 );

		NETWORK->Send(&msg,sizeof(msg));

		return TRUE;
	}
	else if( strcmp( code, "PKMODE" ) == 0 )
	{
		int nAllow = -1;
		sscanf(cheat, "%s %s %d", code, buf, &nAllow );
		if( nAllow != 0 && nAllow != 1 )	return TRUE;
		WORD wMapNum = GetMapNumForName( buf );
		if( wMapNum == 0 )	return TRUE;
		
		MSG_WORD2 msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_PKALLOW_SYN;
		msg.dwObjectID	= HEROID;
		msg.wData1		= wMapNum;
		msg.wData2		= (WORD)nAllow;

		NETWORK->Send(&msg,sizeof(msg));

		return TRUE;
	}	
	else if( strcmp( code, "KILLMONSTER" ) == 0 )
	{
		MSGBASE msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_KILL_ALLMONSTER_SYN;
		msg.dwObjectID	= HEROID;
		
		NETWORK->Send(&msg,sizeof(msg));

		return TRUE;
	}
	else if(strcmp(code, "PARTYINFO") == 0)
	{
		DWORD PartyIdx;
#ifdef TAIWAN_LOCAL
		PartyIdx = HERO->GetPartyIdx();
#else
		sscanf(cheat,"%s %d",code,&PartyIdx);
#endif
		MSG_DWORD msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_PARTYINFO_SYN;
		msg.dwObjectID = HEROID;
		msg.dwData = PartyIdx;
		NETWORK->Send(&msg,sizeof(msg));
		CHATMGR->AddMsg( CTC_SYSMSG, "Party Info Cheat");
		return TRUE;
	}
	else if( strcmp( code, "TOURNAMENTINFO" ) == 0 )
	{
		DWORD param = 0;
		sscanf( cheat, "%s %d", code, &param );

		MSG_DWORD msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_CHEAT;
		msg.dwObjectID = HEROID;
		msg.dwData = param;
		NETWORK->Send( &msg, sizeof(msg) );
		return TRUE;
	}
	else if( strcmp( code, "NPC" ) == 0 )
	{
		int idx;
		int emotion;

		sscanf( cheat, "%s %d %d", code, &idx, &emotion );

		if( idx == 0 )
		{
			GAMEIN->GetNpcImageDlg()->SetActive( FALSE );
		}
		else
		{
			GAMEIN->GetNpcImageDlg()->SetNpc( ( WORD )idx );
			GAMEIN->GetNpcImageDlg()->SetEmotion( ( BYTE )emotion );
			GAMEIN->GetNpcImageDlg()->SetActive( TRUE );
		}
		return TRUE;
	}
	else if( strcmp( code, "HELMET" ) == 0 )
	{
		OBJECTEFFECTDESC desc(FindEffectNum("item_head_01.beff"));
		
		HERO->AddObjectEffect( HELMET_EFFECT, &desc, 1, HERO);
		// 070129 LYW --- Cheat Key : Return true.
		return TRUE ;
	}
	else if( strcmp( code, "SHIELD" ) == 0 )
	{
		OBJECTEFFECTDESC desc(FindEffectNum("item_shi_01.beff"));
		
		HERO->AddObjectEffect( SHIELD_EFFECT, &desc, 1, HERO);
		// 070129 LYW --- Cheat Key : Return true.
		return TRUE ;
	}
	else if( strcmp( code, "FACE" ) == 0 )
	{
		int emotion;

		sscanf( cheat, "%s %d", code, &emotion );

		HERO->GetEngineObject()->SetMaterialIndex( emotion );
		// 070129 LYW --- Cheat Key : Return true.
		return TRUE ;
	}
	else if( strcmp( code, "PVP" ) == 0 )
	{
		SHOWDOWNMGR->ApplyShowdown();
		// 070129 LYW --- Cheat Key : Return true.
		return TRUE ;
	}
	else if( strcmp( code, "MOTION" ) == 0 )
	{
		int motionNum = 0 ;
		sscanf( cheat, "%s %d", code, &motionNum ) ;

		HERO->GetEngineObject()->ChangeMotion(motionNum,FALSE);
		// 070129 LYW --- Cheat Key : Return true.
		return TRUE ;
	}
	else if(strcmp(code, "FISHINGEXP") == 0)
	{
		DWORD dwExp;
		sscanf(cheat,"%s %d",code,&dwExp);

		MSG_DWORD msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_FISHING_FISHINGEXP;
		msg.dwObjectID = HEROID;
		msg.dwData = dwExp;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code, "FISHPOINT") == 0)
	{
		DWORD dwPoint;
		sscanf(cheat,"%s %d",code,&dwPoint);

		MSG_DWORD msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_FISHING_FISHPOINT;
		msg.dwObjectID = HEROID;
		msg.dwData = dwPoint;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code, "COOK") == 0)
	{
		DWORD dwLevel, dwCook, dwEat, dwFire;
		sscanf(cheat,"%s %d %d %d %d",code,&dwLevel,&dwCook,&dwEat,&dwFire);

		MSG_DWORD4 msg;
		msg.Category = MP_COOK;
		msg.Protocol = MP_COOK_CHEAT;
		msg.dwObjectID = HEROID;
		msg.dwData1 = dwLevel;
		msg.dwData2 = dwCook;
		msg.dwData3 = dwEat;
		msg.dwData4 = dwFire;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code, "HS_CREATE") == 0)
	{
		char szHouseName[MAX_HOUSING_NAME_LENGTH];
		sscanf(cheat,"%s %s",code,szHouseName);

		MSG_HOUSE_CREATE msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_CREATE_SYN;
		msg.dwObjectID = HEROID;
		msg.dwPlayerID = HEROID;
		strcpy(msg.szHouseName, szHouseName);
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code, "HS_VISIT") == 0)
	{
		char szOwnerName[MAX_HOUSING_NAME_LENGTH];
		sscanf(cheat,"%s %s",code,szOwnerName);

		MSG_HOUSE_VISIT msg;
		ZeroMemory( &msg, sizeof( msg ) );
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
		msg.dwObjectID = HEROID;
		msg.cKind	= eHouseVisitByCheat ;
		msg.dwValue1 = 0 ;
		msg.dwValue2 = 0 ;

		SafeStrCpy(msg.Name, szOwnerName , MAX_NAME_LENGTH+1);
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code, "HS_INFO") == 0)
	{
		DWORD dwUserIdx = 0;
		sscanf(cheat,"%s %d",code, &dwUserIdx);

		MSG_DWORD2 msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_CHEAT_GETINFO_SYN;
		msg.dwObjectID = HEROID;
		msg.dwData1 = dwUserIdx;
		msg.dwData2 = 0;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code, "HS_DEL") == 0)
	{
		char szOwnerName[MAX_HOUSING_NAME_LENGTH];
		sscanf(cheat,"%s %s",code,szOwnerName);

		MSG_DWORD_NAME msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_CHEAT_DELETE_SYN;
		msg.dwObjectID = HEROID;
		msg.dwData = HEROID;
		strcpy(msg.Name, szOwnerName);
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if( strcmp( code, "HS_EXIT" ) == 0 )
	{
		MSG_DWORD msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_EXIT_SYN;
		msg.dwObjectID = HEROID;
		msg.dwData = 0;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if( strcmp( code, "HS_CLOSE" ) == 0 )
	{
		DWORD dwOwnerUserIndex = 0;
		sscanf(cheat,"%s %d",code,&dwOwnerUserIndex);

		MSG_DWORD2 msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_CHEAT_CLOSE_SYN;
		msg.dwObjectID = HEROID;
		msg.dwData1 = dwOwnerUserIndex;
		msg.dwData2 = 0;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if(strcmp( code, "HS_RANK" ) == 0 )
	{
		MSG_DWORD msg;
		msg.Category = MP_HOUSE;
		msg.Protocol = MP_HOUSE_CHEAT_CALCRANK;
		msg.dwObjectID = HEROID;
		NETWORK->Send(&msg,sizeof(msg));
		return TRUE;
	}
	else if( strcmp( code, "HS_KICK" ) == 0 )
	{
		CObject* pObject =  (CObject*)OBJECTMGR->GetSelectedObject();
		if(pObject->GetObjectKind() == eObjectKind_Player)
		{
			MSG_DWORD msg;
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_KICK_SYN;
			msg.dwObjectID = HEROID;
			msg.dwData = pObject->GetID();
			NETWORK->Send(&msg,sizeof(msg));
			return TRUE;
		}
	}

	else if(strcmp(code, "GT_REG") == 0)
	{
		DWORD dwGuildMasterIdx, dwRound=99, dwPosition=99;
		sscanf(cheat,"%s %d %d %d",code, &dwGuildMasterIdx, &dwRound, &dwPosition);

		MSG_DWORD3 msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_FORCEREG;
		msg.dwObjectID = HEROID;
		msg.dwData1 = dwGuildMasterIdx;
		msg.dwData2 = dwRound;
		msg.dwData3 = dwPosition;
		NETWORK->Send( &msg, sizeof(msg) );
		return TRUE;
	}
	else if(strcmp(code, "GT_STATE")==0)
	{
		DWORD dwState;
		sscanf(cheat, "%s %d", &code, &dwState);

		MSG_DWORD msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_CHEAT_SETSTATE;
		msg.dwObjectID = HEROID;
		msg.dwData = dwState;
		NETWORK->Send( &msg, sizeof(msg) );
		return TRUE;
	}
	else if(strcmp(code, "GT_GETSTATE")==0)
	{
		MSG_DWORD2 msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_CHEAT_GETSTATE_SYN;
		msg.dwObjectID = HEROID;
		msg.dwData1 = 0;
		msg.dwData2 = 0;
		NETWORK->Send( &msg, sizeof(msg) );
		return TRUE;
	}
	else if(strcmp(code, "GT_RESET")==0)
	{
		MSG_DWORD msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_CHEAT_RESET;
		msg.dwObjectID = HEROID;
		msg.dwData = 0;
		NETWORK->Send( &msg, sizeof(msg) );
		return TRUE;
	}
	else if(strcmp(code, "GT_OB")==0)
	{
		DWORD dwBattleID;
		sscanf(cheat, "%s %d", &code, &dwBattleID);

		MSG_DWORD msg;
		msg.Category = MP_GTOURNAMENT;
		msg.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_SYN;
		msg.dwObjectID = HEROID;
		msg.dwData = dwBattleID;
		NETWORK->Send( &msg, sizeof(msg) );
		return TRUE;
	}
#ifdef _GMTOOL_
	// 080115 KTH -- PVP 데미지 비율
	else if( strcmp( code, "PVP_DAMAGE_SET" ) == 0 )
	{
		int DamagePercent = 100;
		sscanf( cheat, "%s %d", code, &DamagePercent );

		MSG_BYTE msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_PVP_DAMAGE_RATE_SYN;
		msg.dwObjectID	= gHeroID;
		msg.bData		= (BYTE)DamagePercent;

		NETWORK->Send( &msg, sizeof(msg) );

		return TRUE;
	}
#endif
	// 070415 LYW --- CheatMsgParser : Add cheat message for change stage.
	else if( strcmp( code, "CHANGE_STAGE" ) == 0 )
	{
		int stage = 0 ;

		sscanf( cheat, "%s %d", code, &stage ) ;

		MSG_BYTE2 msg;
		msg.Category	= MP_QUEST;
		msg.Protocol	= MP_QUEST_CHANGESTAGE_SYN;
		msg.dwObjectID	= gHeroID;
		msg.bData1		= BYTE(stage / 10);
		msg.bData2		= BYTE(stage % 10);

		NETWORK->Send( &msg, sizeof(msg) );
		return TRUE;
	}
	else if( strcmp( code, "RESET_INVENTORY" ) == 0 )
	{
		MSGBASE msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_RESET_INVENTORY_SYN;
		msg.dwObjectID = HEROID;

		NETWORK->Send( &msg, sizeof(msg) );

		HERO->Set_HeroExtendedInvenCount(0);

		CInventoryExDialog* pDlg = NULL ;
		pDlg = GAMEIN->GetInventoryDialog() ;

		if( pDlg )
		{
			pDlg->ShowTab(2,FALSE) ;
			pDlg->ShowTab(3,FALSE) ;

			pDlg->SelectTab(0) ;
		}

		return TRUE;
	}
	// 080213 KTH -- ClearInventory
	else if( strcmp( code, "CLEAR_INVENTORY" ) == 0 )
	{
		MSGBASE msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_CELAR_INVENTORY_SYN;
		msg.dwObjectID = HEROID;

		NETWORK->Send( &msg, sizeof(msg) );
		return TRUE;
	}
	else if( strcmp( code, "SP" ) == 0 )
	{
		DWORD point = 0;
		sscanf( cheat, "%s %d", code, &point ) ;	

		MSG_DWORD Msg;
		Msg.Category = MP_CHEAT;
		Msg.Protocol = MP_CHEAT_SKILLPOINT_SYN;
		Msg.dwObjectID = HEROID;
		Msg.dwData = point;
		NETWORK->Send( &Msg, sizeof(Msg) );
		return TRUE;
	}
	else if( strcmp( code, "GOD" ) == 0 )
	{
		DWORD god = 0;
		sscanf( cheat, "%s %d", code, &god ) ;	

		MSG_DWORD2 Msg;
		Msg.Category = MP_CHEAT;
		Msg.Protocol = MP_CHEAT_GOD_SYN;
		Msg.dwObjectID = HEROID;

		DWORD id = OBJECTMGR->GetSelectedObjectID();

		if( !id )
		{
			id = HEROID;
		}

		Msg.dwData1 = id;
		Msg.dwData2 = god;
		NETWORK->Send( &Msg, sizeof(Msg) );
		return TRUE;
	}
	// 081022 LYW --- CheatMsgPasrer : 펫 버그/기타 테스트를 위하여 Pet에도 GOD 모드를 적용한다.
	else if( strcmp( code, "PETGOD" ) == 0 )
	{
		DWORD god = 0 ;
		sscanf( cheat, "%s %d", code, &god ) ;

		MSG_DWORD msg ;

		msg.Category	= MP_CHEAT ;
		msg.Protocol	= MP_CHEAT_PETGOD_SYN ;

		msg.dwObjectID	= HEROID ;

		msg.dwData		= god ;

		NETWORK->Send( &msg, sizeof(MSG_DWORD)) ;

		return TRUE ;
	}
	else if( strcmp( code, "PETLEVEL" ) == 0 )
	{
		int val = 1;
		sscanf(cheat,"%s %d",code,&val);
		if(val <= HEROPET->GetLevel())
			val = HEROPET->GetLevel()+1;
		MSG_WORD msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_PET_LEVELUP_SYN;
		msg.dwObjectID = HEROID;
		msg.wData = (WORD)val;
		NETWORK->Send(&msg,sizeof(msg));
		
		return TRUE;
	}
	else if(strcmp(code, "MUNPACLEAR") == 0)
	{
		MSGBASE msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_MUNPADATECLR_SYN;
		msg.dwObjectID = HEROID;
		NETWORK->Send(&msg,sizeof(msg));
		HERO->SetGuildEntryDate("2004.01.01");
		return TRUE;
	}
	else if( strcmp( code, "GUILDUNIONCLEAR" ) == 0 )
	{
		MSG_DWORD Msg;
		Msg.Category = MP_CHEAT;
		Msg.Protocol = MP_CHEAT_GUILDUNION_CLEAR;
		Msg.dwObjectID = HEROID;
		Msg.dwData = 0;
		NETWORK->Send( &Msg, sizeof(Msg) );
		return TRUE;
	}
  	else if( strcmp( code, "EXE_STEP" ) == 0 )
  	{
  		WORD wThemeIdx	= 0 ;
  		WORD wMapIdx	= 0 ;
  		WORD wStepIdx	= 0 ;
  
  		sscanf( cheat, "%s %d %d %d", code, &wThemeIdx, &wMapIdx, &wStepIdx ) ;
  
          MSG_WORD3 msg ;
  
  		msg.Category	= MP_SIEGERECALL ;
  		msg.Protocol	= MP_SIEGE_CHEAT_EXECUTE_STEP_SYN ;
  
  		msg.dwObjectID	= HEROID ;
  
  		msg.wData1		= wThemeIdx ;
  		msg.wData2		= wMapIdx ;
  		msg.wData3		= wStepIdx ;
  
  		NETWORK->Send(&msg, sizeof(MSG_WORD3)) ;
  
  		return TRUE ;
  	}
  	// 080820 LYW --- CheatMsgParser : npc 소환 치트를 추가.
  	else if( strcmp( code, "RECALL_NPC" ) == 0 )
  	{
  		WORD wNpcIdx = 0 ;
  		sscanf( cheat, "%s %d", code, &wNpcIdx ) ;
  
  		MSG_WORD msg ;
  
  		msg.Category	= MP_RECALLNPC ;
  		msg.Protocol	= MP_RECALLNPC_CHEAT_RECALL_SYN ;
  
  		msg.dwObjectID	= HEROID ;
  
  		msg.wData		= wNpcIdx ;
  
  		NETWORK->Send(&msg, sizeof(MSG_WORD)) ;
  
  		return TRUE ;
  	}
  	// 080827 LYW --- CheatMsgParser : 공성 상태를 변경하는 치트를 추가.
  	else if( strcmp( code, "SIEGESTATE" ) == 0 )
  	{
  		WORD wSiegeState = 0 ;
  		sscanf( cheat, "%s %u", code, &wSiegeState ) ;
  
  		MSG_WORD msg ;
  
  		msg.Category	= MP_SIEGEWARFARE ;
  		msg.Protocol	= MP_SIEGEWARFARE_CHEAT_SETSTATE_SYN ;
  
  		msg.dwObjectID	= HEROID ;
  
  		msg.wData		= wSiegeState ;
  
  		NETWORK->Send( &msg, sizeof(MSG_WORD) ) ;
  
  		return TRUE ;
  	}
  	else if( strcmp( code, "POS" ) == 0 )
  	{
  		VECTOR3 pos ;
  
  		memset( &pos, 0, sizeof(VECTOR3) ) ;
  		sscanf( cheat, "%s %f %f", code, &pos.x, &pos.z ) ;
  
  		MOVE_POS msg ;
  		msg.Category = MP_CHEAT ; 
  		msg.Protocol = MP_CHEAT_MOVE_SYN ;
  		msg.dwObjectID = HEROID ;
  		msg.dwMoverID = HEROID ;
  		msg.cpos.Compress(&pos) ;
  		NETWORK->Send(&msg,sizeof(msg)) ;
  
  		NETWORK->Send( &msg, sizeof( MOVE_POS ) ) ;
  
  		return TRUE ;
  	}
	// 081011 LYW --- Protocol : 공성 소환물의 소환 상태를 다음 단계로 넘기는 치트 관련 추가.
	else if( strcmp( code, "SW_END" ) == 0 )
	{
		int nThemeIdx = 0 ;
		int nMapIdx	= 0 ;
		int nStepIdx	= 0 ;
		int nObjIdx	= 0 ;
		int nIsParent = 0 ;

		sscanf( cheat, "%s %d %d %d %d %d", code, &nThemeIdx, &nMapIdx, &nStepIdx, &nObjIdx, &nIsParent ) ;

		MSG_BYTE5 msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_SYN ;

		msg.dwObjectID	= HEROID ;

		msg.bData1		= (BYTE)nThemeIdx ;
		msg.bData2		= (BYTE)nMapIdx ;
		msg.bData3		= (BYTE)nStepIdx ;
		msg.bData4		= (BYTE)nObjIdx ;
		msg.bData5		= (BYTE)nIsParent ;

		NETWORK->Send( &msg, sizeof(MSG_BYTE5) ) ;

		return TRUE ;
	}
	else if( strcmp( code, "SW_ENDM" ) == 0 )
	{
		int nThemeIdx = 0 ;
		int nMapIdx	= 0 ;

		sscanf( cheat, "%s %d %d", code, &nThemeIdx, &nMapIdx) ;

		MSG_BYTE2 msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_ENDSTEP_SYN ;

		msg.dwObjectID	= HEROID ;

		msg.bData1		= (BYTE)nThemeIdx ;
		msg.bData2		= (BYTE)nMapIdx ;

		NETWORK->Send( &msg, sizeof(MSG_BYTE2) ) ;

		return TRUE ;
	}

	// 090116 ShinJS --- 캐릭터 안보이기 옵션에 대한 치트 추가
	if( strcmp( code, "HIDE_CHAR" ) == 0 )
	{
		if( OPTIONMGR->GetGameOption()->bHideOtherPlayer )
		{
			OPTIONMGR->GetGameOption()->bHideOtherPlayer = FALSE;			
			OPTIONMGR->ApplyEnvironmemt();
			CHATMGR->AddMsg(CTC_SYSMSG, "옵션변경 : 캐릭터 보이기" );
		}
		else
		{
			OPTIONMGR->GetGameOption()->bHideOtherPlayer = TRUE;
			OPTIONMGR->GetGameOption()->bHideOtherPet = TRUE;
			OPTIONMGR->ApplyEnvironmemt();
			CHATMGR->AddMsg(CTC_SYSMSG, "옵션변경 : 캐릭터/펫 안보이기" );
		}

		return TRUE;
	}
	// 090116 ShinJS --- 펫 안보이기 옵션에 대한 적용을 위해 실행
	else if( strcmp( code, "HIDE_PET" ) == 0 )
	{
		if( OPTIONMGR->GetGameOption()->bHideOtherPet )
		{
			if( OPTIONMGR->GetGameOption()->bHideOtherPlayer )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, "옵션변경실패 : 캐릭터 안보이기 옵션 적용중입니다." );
				return TRUE;
			}

			OPTIONMGR->GetGameOption()->bHideOtherPet = FALSE;
			OPTIONMGR->ApplyEnvironmemt();
			CHATMGR->AddMsg(CTC_SYSMSG, "옵션변경 : 펫 보이기" );
		}
		else
		{
			OPTIONMGR->GetGameOption()->bHideOtherPet = TRUE;
			OPTIONMGR->ApplyEnvironmemt();
			CHATMGR->AddMsg(CTC_SYSMSG, "옵션변경 : 펫 안보이기" );
		}

		return TRUE;
	}
	// 090907 ShinJS --- 길찾기 상태 데이터(시간,위치) Draw 여부 설정
	else if( strcmp( code, "DRAWTIMEINFO" ) == 0 )
	{
		int nOnOff = 0;
		sscanf( cheat, "%s %d", code, &nOnOff );

		BOOL bOnOff = nOnOff != 0 ? TRUE : FALSE;
		MHTIMEMGR->SetDrawStatus( bOnOff );

		return TRUE;
	}
	// 090908 ShinJS --- 길찾기 막힌 타일 Render 여부 설정
	else if( strcmp( code, "RENDERPATH" ) == 0 )
	{
		int nOnOff = 0;
		sscanf( cheat, "%s %d", code, &nOnOff );

		BOOL bOnOff = nOnOff != 0 ? TRUE : FALSE;
		MAP->SetRenderTileData( bOnOff );

		return TRUE;
	}
	// 091013 ShinJS --- 탈것 시간검사 설정
	else if( strcmp( code, "VEHICLE_CHECK" ) == 0 )
	{
		int nOnOff = 0;
		sscanf( cheat, "%s %d", code, &nOnOff );

		BOOL bOnOff = nOnOff != 0 ? TRUE : FALSE;
		VEHICLEMGR->SetCheckTime( bOnOff );

		char* msg = (bOnOff == TRUE ? "설정" : "해제");
		CHATMGR->AddMsg(CTC_SYSMSG, "탈것 시간검사 %s", msg );
		return TRUE;
	}
	// 091106 ShinJS --- Beff 파일 항상 재로드 설정
	else if( strcmp( code, "UPDATEBEFF" ) == 0 )
	{
		int nOnOff = 0;
		sscanf( cheat, "%s %d", code, &nOnOff );

		BOOL bOnOff = nOnOff != 0 ? TRUE : FALSE;
		EFFECTMGR->SetUpdateBeffFile( bOnOff );

		char* msg = (bOnOff == TRUE ? "설정" : "해제");
		CHATMGR->AddMsg(CTC_SYSMSG, "Beff File 재로드 : %s", msg );
		return TRUE;
	}
	else if( strcmp( code, "DUNGEON_OB" ) == 0 )
	{
		int nMapNum = 0;
		int nChannel = 0;
		sscanf( cheat, "%s %d %d", code, &nMapNum, &nChannel );

		MSG_WORD3 msg ;

		msg.Category	= MP_CHEAT ;
		msg.Protocol	= MP_CHEAT_DUNGEON_OBSERVER_SYN ;
		msg.dwObjectID	= HEROID ;

		msg.wData1		= (WORD)nMapNum;
		msg.wData2		= (WORD)nChannel;
		msg.wData3		= 0;

		NETWORK->Send( &msg, sizeof(MSG_WORD3) ) ;
		return TRUE;
	}
	else if( strcmp( code, "GETSERVERTIME" ) == 0 )
	{
		MSGBASE msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_GETSERVERTIME_SYN;
		msg.dwObjectID = HEROID;
		NETWORK->Send( &msg, sizeof(msg) );
		return TRUE;
	}
	else if( strcmp( code, "PRINTSERVERTIME" ) == 0 )
	{
		__time64_t curTime = MHTIMEMGR->GetServerTime();
		struct tm curTimeWhen = *_localtime64( &curTime );
		CHATMGR->AddMsg( CTC_SYSMSG, "현재서버시각 : %d-%d-%d %02d:%02d:%02d", 
			curTimeWhen.tm_year + 1900,
			curTimeWhen.tm_mon + 1,
			curTimeWhen.tm_mday,
			curTimeWhen.tm_hour,
			curTimeWhen.tm_min,
			curTimeWhen.tm_sec );
		return TRUE;
	}

	return FALSE;
}

#endif //_CHEATENABLE_


CCheatMsgParser::CCheatMsgParser()
{
	m_bCheatEnable = FALSE;

	m_pTargetObj = NULL;
#ifdef _TESTCLIENT_
	m_bCheatEnable = TRUE;
#endif

	m_wItemOptionKind		= 0;
	m_bReadyToOptionCheat	= FALSE;
	m_wTargetItemIdx		= 0;
	m_ItemAbsPos			= 0;
	m_OptionCount			= 0;
}

CCheatMsgParser::~CCheatMsgParser()
{

}

void CCheatMsgParser::NetworkMsgParse(BYTE Category,BYTE Protocol,MSGBASE* pMsg)
{
	ASSERT(Category == MP_CHEAT);

	switch(Protocol) 
	{
#ifdef _CHEATENABLE_
	case MP_CHEAT_CHANGEMAP_ACK:				Cheat_ChangeMap_Ack( pMsg ) ;					break ;
	//case MP_CHEAT_CHANGEMAP_NACK:				Cheat_ChangeMap_Nack( pMsg ) ;					break ;
	case MP_CHEAT_WHEREIS_ACK:					Cheat_WhereIs_Ack( pMsg ) ;						break ;
	case MP_CHEAT_WHEREIS_MAPSERVER_ACK:		Cheat_WhereIs_MapServer_Ack( pMsg ) ;			break ;
	case MP_CHEAT_WHEREIS_NACK:					Cheat_WhereIs_Nack( pMsg ) ;					break ;
	case MP_CHEAT_BLOCKCHARACTER_ACK:			Cheat_BlockCharacter_Ack( pMsg ) ;				break ;
	case MP_CHEAT_BLOCKCHARACTER_NACK:			Cheat_BlockCharacter_Nack( pMsg ) ;				break ;
	case MP_CHEAT_BANCHARACTER_ACK:				Cheat_BanCharacter_Ack( pMsg ) ;				break ;
	//case MP_CHEAT_BANCHARACTER_NACK:			Cheat_BanCharacter_Nack( pMsg ) ;				break ;
	case MP_CHEAT_ITEM_ACK:						Cheat_Item_Ack( pMsg ) ;						break ;
	case MP_CHEAT_AGENTCHECK_ACK:				Cheat_AgentCheck_Ack( pMsg ) ;					break ;
#endif	//_CHEATENABLE_
	case MP_CHEAT_MOVE_ACK:						Cheat_Move_Ack( pMsg ) ;						break ;
	case MP_CHEAT_HIDE_ACK:						Cheat_Hide_Ack( pMsg ) ;						break ;
	case MP_CHEAT_CHANGESIZE:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwData1 );
			if( !pPlayer ) return;

			float fSize = (float)pmsg->dwData2 / 100.f;
			if( fSize > 3.0f || fSize < .3f ) return;

			pPlayer->SetCharacterSize( fSize );
		}
		break;

	// 090909 ONS 일반클라이언트에서 이벤트 공지 메세지가 출력되도록 수정.
	case MP_CHEAT_EVENTNOTIFY_ON:				Cheat_EventNotify_On( pMsg ) ;					break ;
	case MP_CHEAT_EVENTNOTIFY_OFF:				Cheat_EventNotify_Off( pMsg ) ;					break ;

//-----------------------
#ifdef _GMTOOL_
	case MP_CHEAT_GM_LOGIN_ACK:	Cheat_GM_Login_Ack(pMsg); break;
	case MP_CHEAT_GM_LOGIN_NACK: Cheat_GM_Login_Nack(pMsg);	break;
	case MP_CHEAT_PARTYINFO_ACK: Cheat_PartyInfo_Ack(pMsg);	break;
	case MP_CHEAT_NPCHIDE_ACK: Cheat_NpcHide_Ack(pMsg);	break;
	case MP_CHEAT_PVP_DAMAGE_RATE_ACK: Cheat_PvP_Damage_Rate(pMsg);	break;
	case MP_CHEAT_CELAR_INVENTORY_ACK: Cheat_Clear_Inventory(pMsg);	break;
	case MP_CHEAT_VEHICLE_GET_ACK: GMTOOLMGR->SetVehicle(*((MSG_SKILL_LIST*)pMsg));	break;
	case MP_CHEAT_DUNGEON_OBSERVER_ACK:	Cheat_Dungeon_Observer( pMsg );	break;
	case MP_CHEAT_DUNGEON_GETINFOALL_ACK: GMTOOLMGR->DungeonGetInfo_Ack(pMsg); break;
	case MP_CHEAT_DUNGEON_GETINFOONE_ACK: GMTOOLMGR->DungeonGetDetailInfo_Ack(pMsg); break;
	case MP_CHEAT_MONSTER_SCRIPT_ACK: Cheat_Monster_Script_Ack((TESTMSG*)pMsg);	break;
	case MP_CHEAT_MONSTER_SCRIPT_NACK: Cheat_Monster_Script_Nack((TESTMSG*)pMsg); break;
	// 100610 ONS 채팅금지 기능 추가.
	case MP_CHEAT_FORBID_CHAT_ACK:
		{
			CHATMGR->AddMsg( CTC_GMCHAT, "Add forbid-chat-list of successful" );
		}
		break;
	case MP_CHEAT_PERMIT_CHAT_ACK:
		{
			CHATMGR->AddMsg( CTC_GMCHAT, "Chat is allowed" );
		}
		break;
	case MP_CHEAT_FORBID_ALREADYEXIST:	
		{
			CHATMGR->AddMsg( CTC_GMCHAT, "Already exists in forbid-chat-list" );
		}
		break;
	case MP_CHEAT_FORBID_NONTARGET:
		{
			CHATMGR->AddMsg( CTC_GMCHAT, "Does not exist in forbid-chat-list" );
		}
		break;
#endif
	}
}



// 071128 LYW --- CheatMsgParser : 
#ifdef _CHEATENABLE_
void CCheatMsgParser::Cheat_ChangeMap_Ack( void* pMsg )
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;
	MAPTYPE mapnum = pmsg->wData;
	MAPCHANGE->SetGameInInitKind(eGameInInitKind_MapChange);
	MAINGAME->SetGameState(eGAMESTATE_MAPCHANGE,&mapnum,4);
}

void CCheatMsgParser::Cheat_WhereIs_Ack( void* pMsg ) 
{
	CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 1938 ) );
#ifdef _GMTOOL_
	GMTOOLMGR->DisplayWhereInfo( "SELECT", 0, 0, 0 );
#endif
}

void CCheatMsgParser::Cheat_WhereIs_MapServer_Ack( void* pMsg ) 
{
	MSG_WHERE_INFO* pmsg = (MSG_WHERE_INFO*)pMsg;

	VECTOR3 vPos;
	pmsg->cpos.Decompress( &vPos );

	char* strMapName = GetMapName( pmsg->wMapNum );
	if( strMapName[0] == 0 )
	{
		CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 1938 ) );
#ifdef _GMTOOL_
		GMTOOLMGR->DisplayWhereInfo( "SELECT", 0, 0, 0 );	
#endif
	}
	else
	{
		CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 1939 ),
			strMapName, pmsg->bChannel,
			(int)(vPos.x/100.0f), (int)(vPos.z/100.0f) );
#ifdef _GMTOOL_
		GMTOOLMGR->DisplayWhereInfo( strMapName, pmsg->bChannel,
			(int)(vPos.x/100.0f), (int)(vPos.z/100.0f) );
#endif
	}
}

void CCheatMsgParser::Cheat_WhereIs_Nack( void* pMsg ) 
{
	MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;

	switch( pmsg->bData )
	{
	case 1:
		CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 210 ) );
		break;
	case 2:
		CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 211 ) );
		break;
	default:
		CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 1940 ) );
		break;
	}
}

void CCheatMsgParser::Cheat_BlockCharacter_Ack( void* pMsg ) 
{
	CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 1941 ) );
}

void CCheatMsgParser::Cheat_BlockCharacter_Nack( void* pMsg ) 
{
	CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 210 ) );
}

void CCheatMsgParser::Cheat_BanCharacter_Ack( void* pMsg ) 
{
	CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 1942 ) );
}

void CCheatMsgParser::Cheat_Item_Ack( void* pMsg ) 
{
	ITEMOBTAINARRAY * pmsg = (ITEMOBTAINARRAY *)pMsg;
	CItem* pItem = NULL;
	
	for(WORD i = 0 ; i < pmsg->ItemNum ; ++i)
	{
		ITEMBASE* pItemBase = pmsg->GetItem(i) ;

		if( !pItemBase ) continue ;

		pItem = GAMEIN->GetInventoryDialog()->GetItemForPos(
			pmsg->GetItem(i)->Position);

		if(pItem)
		{
			pItem->SetItemBaseInfo(*pItemBase);

			// 100422 ShinJS --- 인벤토리 아이템 변경시 퀘스트 정보를 갱신한다.
			GAMEIN->GetQuestQuickViewDialog()->ResetQuickViewDialog();
			GAMEIN->GetQuestDialog()->RefreshQuestList();
			QUESTMGR->ProcessNpcMark();
		}
		else
		{
			pItem = ITEMMGR->MakeNewItem(pItemBase,"MP_CHEAT_ITEM_ACK");

			if( !pItem ) continue ;

			ITEM_INFO* pInfo = NULL ;
			pInfo = ITEMMGR->GetItemInfo( pItem->GetItemIdx() ) ;

			if( !pInfo ) continue ;

			if( pInfo->wSeal == eITEM_TYPE_SEAL )
			{
				pItemBase->nSealed = eITEM_TYPE_SEAL ;
				pItemBase->nRemainSecond = pInfo->dwUseTime ;

				pItem->SetItemBaseInfo( *pItemBase ) ;
			}

			BOOL rt = GAMEIN->GetInventoryDialog()->AddItem(pItem);
			if(!rt)
			{
				ASSERT(0);
			}
		}
	}

	cDialog* pDlg = WINDOWMGR->GetWindowForID( DBOX_DIVIDE_INV );
	if( pDlg )
	{
		((cDivideBox*)pDlg)->ExcuteDBFunc( 0 );	//Ae¼O¹oÆ° ´ⓒ¸￡±a
	}

	CHATMGR->AddMsg( CTC_GETITEM, CHATMGR->GetChatMsg( 101 ), pItem->GetItemInfo()->ItemName );
	ITEMMGR->ItemDropEffect( pItem->GetItemIdx() );

	QUICKMGR->RefreshQickItem();
}

	void CCheatMsgParser::Cheat_AgentCheck_Ack( void* pMsg ) 
	{
		CHATMGR->AddMsg( CTC_SYSMSG, "CHEAT_AGENT_ACK!" );
	}

#endif	//_CHEATENABLE_
void CCheatMsgParser::Cheat_Move_Ack( void* pMsg )
{
	MOVE_POS* pmsg = (MOVE_POS*)pMsg;
	VECTOR3 pos;			
	pmsg->cpos.Decompress(&pos);

	BASEMOVE_INFO info;
	info.bMoving = FALSE;
	info.KyungGongIdx = 0;
	info.MoveMode = eMoveMode_Run;
	info.CurPosition = pos;
	MOVEMGR->InitMove(HERO,&info);

}

void CCheatMsgParser::Cheat_Hide_Ack( void* pMsg ) 
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwData1 );
	if( !pPlayer ) return;
	
	ASSERT(pPlayer->GetObjectKind() == eObjectKind_Player);
	
	if(pmsg->dwData2 == 0 )
	{
#ifdef _GMTOOL_
		if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM  && CAMERA->GetCameraMode() != eCM_EyeView )
			pPlayer->GetEngineObject()->SetAlpha( 1.0f );
		else
#endif
			pPlayer->GetEngineObject()->SetAlpha( 1.0f );
			pPlayer->GetEngineObject()->Show();
		
		pPlayer->GetCharacterTotalInfo()->bVisible = TRUE;
	}
	else
	{
#ifdef _GMTOOL_
		if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM  && CAMERA->GetCameraMode() != eCM_EyeView )
			pPlayer->GetEngineObject()->SetAlpha( 0.3f );
		else
#endif
		// 090909 ONS 일반클라이언트에서 자신의 캐릭터가 사라지지 않도록 수정.
		{
			if( pPlayer == HERO )
			{
				pPlayer->GetEngineObject()->SetAlpha( 0.3f );
			}
			else
			{
				pPlayer->GetEngineObject()->HideWithScheduling();
			}
		}

		pPlayer->GetCharacterTotalInfo()->bVisible = FALSE;
		if( pPlayer->GetID() == OBJECTMGR->GetSelectedObjectID() )
		{
			OBJECTMGR->SetSelectedObject( NULL );
		}
	}

	OBJECTMGR->ApplyShadowOption(pPlayer);
	OBJECTMGR->ApplyOverInfoOption(pPlayer);
}

#ifdef _GMTOOL_
void CCheatMsgParser::Cheat_GM_Login_Ack( void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
#ifdef _GMTOOL_
	if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM )
	{
		GMTOOLMGR->Login( TRUE, (int)pmsg->dwData );
	}
#endif
}

void CCheatMsgParser::Cheat_GM_Login_Nack( void* pMsg )
{
	MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;
#ifdef _GMTOOL_
	if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM )
	{
		GMTOOLMGR->Login( FALSE, (int)pmsg->bData );
	}
#endif
}

void CCheatMsgParser::Cheat_PartyInfo_Ack( void* pMsg ) 
{
	PARTY_INFO * pmsg = (PARTY_INFO*)pMsg;
	for(int i=0; i<MAX_PARTY_LISTNUM; ++i)
	{
		if(pmsg->Member[i].dwMemberID == 0)
			SafeStrCpy(pmsg->Member[i].Name, "!!!", MAX_NAME_LENGTH+1);
		CHATMGR->AddMsg( CTC_SYSMSG, "%s, %d, %d, %d", 
			pmsg->Member[i].Name, pmsg->Member[i].Level, pmsg->Member[i].bLogged,
			pmsg->Member[i].LifePercent);
	}
}

void CCheatMsgParser::Cheat_NpcHide_Ack( void* pMsg )
{
	MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;

	OBJECTMGR->HideNpc(pmsg->wData1, (BOOL)pmsg->wData2);
}
#endif

void CCheatMsgParser::Cheat_PvP_Damage_Rate( void* pMsg )
{
	MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;

	char Text[256] = { 0, };
	sprintf( Text, CHATMGR->GetChatMsg(1471), pmsg->bData );
	CHATMGR->AddMsg(CTC_SYSMSG, Text) ;
}
void CCheatMsgParser::Cheat_Clear_Inventory( void* pMsg )
{
	CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();

	if( pDlg )
	{
		pDlg->ClearInventory();
	}
}
void CCheatMsgParser::Cheat_Dungeon_Observer( void* pMsg )
{
	MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;
	
	DWORD Mapnum = pmsg->wData1;
	WORD wChannel = pmsg->wData2;

	if(Mapnum==0 || wChannel==0)
		return;

	MAPCHANGE->SetGameInInitKind( eGameInInitKind_DungeonObserverEnter );
	MAINGAME->SetGameState( eGAMESTATE_MAPCHANGE, &Mapnum, 4 );
	OBJECTSTATEMGR->EndObjectState( HERO, eObjectState_Deal );
	//GAMEIN->SetGuildIdx( HERO->GetGuildIdx() );
	GAMEIN->SetMoveMap( MAP->GetMapNum() );
	GAMEIN->SetDestMoveMap( Mapnum );
	GAMEIN->SetBattleIdx( wChannel );
	CHATMGR->SaveChatList();

	// 080411 LYW --- GuildTournamentMgr : 채팅방에서 채팅중이었다면, 채팅내용을 저장한다.
	CHATROOMMGR->SaveChat() ;
	
	WINDOWMGR->MsgBox( MBI_NOBTNMSGBOX, MBT_NOBTN, CHATMGR->GetChatMsg(206));
}

// 090909 ONS 일반클라이언트에서도 출력되도록 수정.
void CCheatMsgParser::Cheat_EventNotify_On( void* pMsg ) 
{
	MSG_EVENTNOTIFY_ON* pmsg = (MSG_EVENTNOTIFY_ON*)pMsg;

	NOTIFYMGR->SetEventNotifyStr( pmsg->strTitle, pmsg->strContext );
	NOTIFYMGR->SetEventNotify( TRUE );
	NOTIFYMGR->SetEventNotifyChanged( TRUE );

	NOTIFYMGR->ResetEventApply();
	for(int i=0; i<eEvent_Max; ++i)
	{
		if( pmsg->EventList[i] )
			NOTIFYMGR->SetEventApply( i );
	}
}

void CCheatMsgParser::Cheat_EventNotify_Off( void* pMsg ) 
{
	NOTIFYMGR->SetEventNotify( FALSE );
	NOTIFYMGR->SetEventNotifyChanged( FALSE );
}

void CCheatMsgParser::Cheat_Monster_Script_Ack(TESTMSG* message)
{
	const DWORD objectIndex = message->dwObjectID;
	LPCTSTR fileName = message->Msg;

	CObject* const object = OBJECTMGR->GetObject(
		objectIndex);

	if(0 == object)
	{
		return;
	}

	object->GetObjectBalloon()->SetNickName(
		LPTSTR(fileName));
	OBJECTMGR->ShowChatBalloon(
		object,
		LPTSTR(fileName));
}

void CCheatMsgParser::Cheat_Monster_Script_Nack(TESTMSG* message)
{
	LPCTSTR fileName = message->Msg;

	CHATMGR->AddMsg(
		CTC_SYSMSG,
		"%s is failed applying to monster",
		fileName);
}
