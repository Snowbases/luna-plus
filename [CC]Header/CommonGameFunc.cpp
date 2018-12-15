#include "stdafx.h"

#include <map>

#include "MHFile.h"
using namespace std;

BOOL g_bAssertMsgBox = FALSE;
char _g_AssertErrorMessage[255];

#include "CommonGameFunc.h"
#include "..\4DyuchiGXGFunc/global.h"
#ifdef _CLIENT_
#include "../ChatManager.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[Client]Luna/interface/cWindowDef.h"
#endif

DWORD DayOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
DWORD DayOfMonth_Yundal[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static char Days[7][16] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
static char WeatherState[][16] = { "CLEAR", "SNOW" };

#ifdef _CLIENT_

void AddComma( char* pBuf )
{
	if( *pBuf == 0 ) return;

	char buf[64];
//	strcpy( buf, pBuf );	//buf = 원본
	SafeStrCpy( buf, pBuf, 64 );
	char* p = buf;

	*(pBuf++) = *(p++);

	int n = strlen( pBuf ) % 3;

	while( *p )
	{
		if( n != 0 )
		{
			*pBuf = *p;
			++p;
		}
		else
		{
			*pBuf = ',';
		}

		++pBuf;
		n = ( n == 0 ) ? 3 : n - 1;
	}
	
	*pBuf = 0;
}

char* AddComma( DWORDEX dwMoney )
{
	static char buf[32];

	wsprintf( buf, "%I64d", dwMoney );
	AddComma( buf );
	return buf;
}

char* RemoveComma( char* str )
{
	static char buf[32];
	char* p = buf;

	while( *str )
	{
		if( *str != ',' )
		{
			*p = *str;
			++p;
		}

		++str;
	}

	*p = 0;

	return buf;
}
#else

GameRoomType GetGameRoomType(const _PACKET_GAME& packetGame)
{
	if(0 == packetGame.Packet_Result)
	{
		return GameRoomIsPremium;
	}
	else if(0 == _tcsicmp("PC", packetGame.User_Status))
	{
		return GameRoomIsFree;
	}

	return GameRoomIsInvalid;
}
#endif

// 090603 LUJ, 맵 추가 작업이 쉽도록 수정
LPTSTR GetMapName( MAPTYPE mapIndex )
{
	switch( MapName( mapIndex ) )
	{
	case Alker_Farm:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1067 );
#else
		return _T( "Alker Farm" );
#endif
		// 090921 LUJ, 네라 농장 추가
	case Nera_Farm:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1937 );
#else
		return _T( "Nera Farm" );
#endif
	case Moon_Blind_Swamp:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 142 );
#else
		return _T( "Moon Blind Swamp" );
#endif
	case Red_Orc_Outpost:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 143 );
#else
		return _T( "Red Orc Outpost" );
#endif
	case Moon_Blind_Forest:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 141 );
#else
		return _T( "Moon Blind Forest" );
#endif
	case Haunted_Mine_1F:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1226 );
#else
		return _T( "Haunted Mine 1F" );
#endif
	case Haunted_Mine_2F:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1227 );
#else
		return _T( "Haunted Mine 2F" );
#endif
	case The_Gate_Of_Alker:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 137 );
#else
		return _T( "The Gate Of Alker" );
#endif
	case Alker_Harbor:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 138 );
#else
		return _T( "Alker Harbor" );
#endif
	case Ruins_Of_Draconian:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 139 );
#else
		return _T( "Ruins of Draconian" );
#endif
	case Zakandia:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 145 );
#else
		return _T( "Zakandia" );
#endif
	case Tarintus:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 144 );
#else
		return _T( "Tarintus" );
#endif
	case Mont_Blanc_Port:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 146 );
#else
		return _T( "Mont Blanc Port" );
#endif
	case Dryed_Gazell_Fall:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 147 );
#else
		return _T( "Dried Gazell Fall" );
#endif
	case Zakandia_Outpost:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 140 );
#else
		return _T( "Zakandia Outpost" );
#endif
	case The_Dark_Portal:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 150 );
#else
		return _T( "The Dark Portal" );
#endif
	case Distorted_crevice:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1260 );
#else
		return _T( "Distorted crevice" );
#endif
	case The_Way_To_The_Howling_Ravine:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 148 );
#else
		return _T( "The Way to the Howling Ravine" );
#endif
	case Howling_Ravine:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1036 );
#else
		return _T( "Howling Ravine" );
#endif
	case Howling_Cave_1F:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1032 );
#else
		return _T( "Howling Cave 1F" );
#endif
	case Howling_Cave_2F:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1033 );
#else
		return _T( "Howling Cave 2F" );
#endif
	case Ghost_Tree_Swamp:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1251 );
#else
		return _T( "Ghost Tree Swamp" );
#endif
	case Lair_of_Kierra:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1328 );
#else
		return _T( "Lair of Kierra" );
#endif
	case The_Valley_Of_Fairy:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1426 );
#else
		return _T( "The Valley Of Fairy" );
#endif
	case The_Town_of_Nera_Castle:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1569 );
#else
		return _T( "The Town of Nera Castle" );
#endif
	case The_Great_Garden:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1572 );
#else
		return _T( "The Great Garden" );
#endif
	case TheKnightsGrave:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1596 );
#else
		return _T( "The Knights Grave" );
#endif
	case A_Harbor_of_Nera:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1570 );
#else
		return _T( "A Harbor of Nera" );
#endif
	case Temple_of_Greed:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 2209 );
#else
		return _T( "Temple_of_Greed" );
#endif
	case Broken_Shipwrecked:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 2208 );
#else
		return _T( "Broken_Shipwrecked" );
#endif

	case The_tower_of_wizardly_Low:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1758 );
#else
		return _T( "The Tower of Wizardly Low" );
#endif
	case The_tower_of_wizardly_Middle:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1759 );
#else
		return _T( "The Tower of Wizardly Middle" );
#endif
	case The_tower_of_wizardly_High:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1760 );
#else
		return _T( "The Tower of Wizardly High" );
#endif
	case The_Death_tower:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 2030 );
#else
		return _T( "The Death tower" );
#endif
	case The_Spirit_Forest:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 2031 );
#else
		return _T( "The Spirit Forest" );
#endif
	case The_Cristal_Empire:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 2032 );
#else
		return _T( "The Cristal Empire" );
#endif
	case TreeOfKenai:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 2331 );
#else
		return _T( "Tree Of Kenai" );
#endif
	case TheCryingTavern:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 2332 );
#else
		return _T( "The Crying Tavern" );
#endif
	case TheUndergroundPassage:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg(2324);
#else
		return _T("The underground passage");
#endif
	case SahelZone:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1911 );
#else
		return _T( "Sahel Zone" );
#endif
	case MirageInTheParasus:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1912 );
#else
		return _T( "Mirage in the Parasus" );
#endif
	case RuinsOfAncientElpis:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1913 );
#else
		return _T( "Ruins of Ancient Elpis" );
#endif
	case DragoniansTomb1:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1914 );
#else
		return _T( "Dragonian's Tomb #1" );
#endif
	case DragoniansTomb2:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1915 );
#else
		return _T( "Dragonian's Tomb #2" );
#endif
	case The_Castle_of_Rushen:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1675 );
#else
		return _T( "The Castle of Rushen" );
#endif
	case The_Culvert:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1673 );
#else
		return _T( "The Culvert" );
#endif
	case The_Castle_of_Zevyn:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1674 );
#else
		return _T( "The Castle of Zevyn" );
#endif
	case Advanced_Dungeon:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1676 );
#else
		return _T( "Advanced Dungeon" );
#endif
	case DateDungeon_Cemetery:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 2317 );
#else
		return _T( "Date Dungeon - Cemetery" );
#endif
	case DateDungeon_BrokenHouse:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 2318 );
#else
		return _T( "Date Dungeon - Broken House" );
#endif
	case DateDungeon_DarkCave:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 2319 );
#else
		return _T( "Date Dungeon - Dark Cave" );
#endif
	case GuildTournamentStadium:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1654 );
#else
		return _T( "Guild Tournament" );
#endif
	case DesolatedGorge:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1918 );
#else
		return _T( "Desolated Gorge" );
#endif
	case DateDungeon_CookieGarden:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 2320 );
#else
		return _T( "Date Dungeon - Cookie Garden" );
#endif
	case AlkerResidence:
#ifdef _CLIENT_
		return CHATMGR->GetChatMsg( 1917 );
#else
		return _T( "Alker residence" );
#endif
	}

	return _T( "?" );
}

MAPTYPE GetMapNumForName( LPCTSTR name )
{
	for( MAPTYPE i = 0; i < MAX_MAP_NUM; ++i )
	{
		if( 0 == _tcscmp( GetMapName( i ), name ) )
		{
			return i;
		}
	}

	return 0;
}

void SetProtocol(MSGBASE* pMsg,BYTE bCategory,BYTE bProtocol)
{
	pMsg->Category = bCategory;
	pMsg->Protocol = bProtocol;	
}

eITEMTABLE GetTableIdxPosition(WORD abs_pos)
{
	//if(TP_INVENTORY_START <= abs_pos && abs_pos < TP_INVENTORY_END)
	// 071210 KTH -- 확장인벤토리까지 검사
	if(TP_INVENTORY_START <= abs_pos && abs_pos < TP_EXTENDED_INVENTORY2_END)
	{
		return eItemTable_Inventory;
	}
	else if(TP_WEAR_START <= abs_pos && abs_pos < TP_WEAR_END)
	{
		return eItemTable_Weared;
	}
	else if(TP_STORAGE_START <= abs_pos && abs_pos < TP_STORAGE_END)
	{
		return eItemTable_Storage;
	}

	return eItemTable_Max;
}

DWORD GetMainTargetID(MAINTARGET* pMainTarget)
{
	if(pMainTarget->MainTargetKind == MAINTARGET::MAINTARGETKIND_OBJECTID)
		return pMainTarget->dwMainTargetID;
	else
		return 0;
}

#ifdef _MAPSERVER_
#include "UserTable.h"
#include "CharMove.h"
#endif
#ifdef _CLIENT_
#include "ObjectManager.h"
#endif

BOOL GetMainTargetPos(MAINTARGET* pMainTarget,VECTOR3* pRtPos, CObject ** ppObject)
{	
	if(pMainTarget->MainTargetKind == MAINTARGET::MAINTARGETKIND_POS)
	{
		pMainTarget->cpTargetPos.Decompress(pRtPos);
		if(ppObject)
			*ppObject = NULL;
		return TRUE;
	}
	else
	{
		
#ifdef _MAPSERVER_
		CObject* pTarget;
		pTarget = g_pUserTable->FindUser(pMainTarget->dwMainTargetID);
		if(ppObject)
			*ppObject = pTarget;
		if(pTarget == NULL)
			return FALSE;
		*pRtPos = *CCharMove::GetPosition(pTarget);
#endif
#ifdef _CLIENT_
		CObject* pTarget;
		pTarget = OBJECTMGR->GetObject(pMainTarget->dwMainTargetID);
		if(ppObject)
			*ppObject = pTarget;
		if(pTarget == NULL)
			return FALSE;
		pTarget->GetPosition(pRtPos);
#endif
		return TRUE;
	}
}

void SetVector3(VECTOR3* vec,float x,float y,float z)
{
	vec->x = x;
	vec->y = y;
	vec->z = z;
}

#ifdef _CLIENT_
void VRand(VECTOR3* pResult,VECTOR3* pOriginPos,VECTOR3* pRandom)
{
	int temp;
	temp = (int)pRandom->x;
	pResult->x = temp ? pOriginPos->x + (rand() % (2*temp)) - temp : pOriginPos->x;
	temp = (int)pRandom->y;
	pResult->y = temp ? pOriginPos->y + (rand() % (2*temp)) - temp : pOriginPos->y;
	temp = (int)pRandom->z;
	pResult->z = temp ? pOriginPos->z + (rand() % (2*temp)) - temp : pOriginPos->z;
}

void TransToRelatedCoordinate(VECTOR3* pResult,VECTOR3* pOriginPos,float fAngleRadY)
{
	if(pResult == pOriginPos)
	{
		VECTOR3 tempOri = *pOriginPos;
		pOriginPos = &tempOri;
	}
	static BOOL bb = FALSE;
	if(bb == FALSE)
	{
		MATRIX4 my;
		SetRotationYMatrix(&my,-fAngleRadY);
		
		TransformVector3_VPTR2(pResult,pOriginPos,&my,1);
	}
	else
	{
		RotateVectorAxisY(pResult,pOriginPos,fAngleRadY);
	}
}

void RotateVectorAxisX(VECTOR3* pResult,VECTOR3* pOriginVector,float fAngleRadX)
{
	if(fAngleRadX  == 0)
	{
		*pResult = *pOriginVector;
	}
	else
	{
		MATRIX4 mx;
		SetRotationXMatrix(&mx,-fAngleRadX);
		
		TransformVector3_VPTR2(pResult,pOriginVector,&mx,1);
	}
}

void RotateVectorAxisZ(VECTOR3* pResult,VECTOR3* pOriginVector,float fAngleRadZ)
{
	if(fAngleRadZ  == 0)
	{
		*pResult = *pOriginVector;
	}
	else
	{
		pResult->x = pOriginVector->x*cosf(fAngleRadZ) - pOriginVector->y*sinf(fAngleRadZ);
		pResult->y = pOriginVector->x*sinf(fAngleRadZ) + pOriginVector->y*cosf(fAngleRadZ);
		pResult->z = pOriginVector->z;
	}
}

void RotateVector( VECTOR3* pResult, VECTOR3* pOriginVector, float x, float y, float z )
{
	RotateVectorAxisX( pResult, pOriginVector, x );
	RotateVectorAxisY( pResult, pResult, y );
	RotateVectorAxisZ( pResult, pResult, z );
}

void RotateVector( VECTOR3* pResult, VECTOR3* pOriginVector, VECTOR3 vRot )
{
	RotateVectorAxisX( pResult, pOriginVector, vRot.x );
	RotateVectorAxisY( pResult, pResult, vRot.y );
	RotateVectorAxisZ( pResult, pResult, vRot.z );
}

#endif


void RotateVectorAxisY(VECTOR3* pResult,VECTOR3* pOriginVector,float fAngleRadY)
{
	if(fAngleRadY == 0)
	{
		*pResult = *pOriginVector;
	}
	else
	{
		pResult->x = pOriginVector->x*cosf(fAngleRadY) - pOriginVector->z*sinf(fAngleRadY);
		pResult->y = pOriginVector->y;
		pResult->z = pOriginVector->x*sinf(fAngleRadY) + pOriginVector->z*cosf(fAngleRadY);
	}
}

void BeelinePoint(VECTOR3 * origin, VECTOR3 * dir, float dist, VECTOR3 * result)
{
/*
	float m = dir->x / dir->z;
	result->x = origin->x + dist*cosf(m);
	result->z = origin->z + dist*sinf(m);
*/
//KES
	float rrr = sqrt( dir->x*dir->x + dir->z*dir->z );
	result->x = origin->x + dir->x * dist / rrr;
	result->z = origin->z + dir->z * dist / rrr;
}
void AdjacentPointForOriginPoint(VECTOR3 * origin, VECTOR3 * target, float dist, VECTOR3 * result)
{
	float rrr = CalcDistanceXZ(origin,target);

	if(rrr < dist)
	{
		result->x = target->x;
		result->z = target->z;
		return;
	}

//KES
	result->x = origin->x + ( target->x - origin->x ) * dist / rrr;
	result->z = origin->z + ( target->z - origin->z ) * dist / rrr;

/*
	float m = (origin->x - target->x)/(origin->z - target->z);
	// taiyo 2004/5/18 : origin->x + , origin->z+
	result->x = origin->x + dist*cosf(m);
	result->z = origin->z + dist*sinf(m);
*/
}
float CalcDistanceXZ(VECTOR3* v1,VECTOR3* v2)
{
//	float x = v1->x - v2->x;
//	float z = v1->z - v2->z;
//	return sqrtf(x*x + z*z);
	return sqrtf( (v1->x - v2->x)*(v1->x - v2->x) + (v1->z - v2->z)*(v1->z - v2->z) );
}

BOOL IsSameDay(char* strDay,WORD Year,WORD Month,WORD Day)
{
	//ex) 2002.02.08
	char temp[64];
	sprintf(temp,"%d.%02d.%02d",Year,Month,Day);
	return strcmp(strDay,temp) == 0;
}

void WriteAssertMsg(char* pStrFileName,int Line,char* pMsg)
{
	if(g_bAssertMsgBox)
	{
		wsprintf(_g_AssertErrorMessage, "ASSERT() file:%s,line:%d (%s) do you debug?", pStrFileName, Line, pMsg);
		if(IDYES == MessageBox(NULL, _g_AssertErrorMessage, NULL, MB_YESNO|MB_TOPMOST))
			__asm int 3;
		return;
	}
	
	char temp[MAX_PATH] = {0};

	if(pMsg)
	{
		_sntprintf(
			temp,
			_countof(temp) - 1,
			"[%s] %s line:%d, msg:%s, ",
			g_szHeroIDName,
			pStrFileName,
			Line,
			pMsg);
	}
	else
	{
		_sntprintf(
			temp,
			_countof(temp) - 1,
			"[%s] %s line:%d,",
			g_szHeroIDName,
			pStrFileName,Line);
	}
	
	SYSTEMTIME time;
	char szFile[256] = {0,};
	GetLocalTime( &time );
#ifdef _MAPSERVER_
	sprintf( szFile, "./Log/Assert_%02d_%4d%02d%02d.txt", g_pServerSystem->GetMapNum(), time.wYear, time.wMonth, time.wDay );
#else
	sprintf( szFile, "./Log/Assert_%4d%02d%02d.txt", time.wYear, time.wMonth, time.wDay );
#endif

	//파일의 용량검사.

	static BOOL bWrite = FALSE;

	if( bWrite == FALSE )
	{
		DWORD dwFileSize;
		DWORD dwSizeLow;
		DWORD dwSizeHigh;
		HANDLE hFile;
		OFSTRUCT data;

		hFile = (HANDLE)::OpenFile(szFile,&data,OF_READ);

		if((HFILE)hFile == 0xffffffff)
		{
			::CloseHandle(hFile);

			if(FILE* fp = fopen(szFile,"a+"))
			{
				fprintf(fp,"%stime:%s\n",temp,GetCurTimeToString());

				fclose(fp);
			}

			return ;
		}

		dwSizeLow = GetFileSize(hFile, &dwSizeHigh);
		dwFileSize = dwSizeHigh << 16 | dwSizeLow;
		::CloseHandle(hFile);


		if(dwFileSize <= 1000000) //1M 보다 작다면 기록
		{
			FILE* fp = fopen(szFile,"a+");
			if(fp)
			{
				fprintf(fp,"%stime:%s\n",temp,GetCurTimeToString());

				fclose(fp);
			}
		}
		else
		{
			bWrite = TRUE;
			
		}

		

	}

}

void CriticalAssertMsg(char* pStrFileName,int Line,char* pMsg)
{
	wsprintf(_g_AssertErrorMessage, "ASSERT() file:%s,line:%d (%s) do you debug?", pStrFileName, Line, pMsg);
	if(IDYES == MessageBox(NULL, _g_AssertErrorMessage, NULL, MB_YESNO|MB_TOPMOST))
		__asm int 3;
	return;
}


void SafeStrCpy( char* pDest, const char* pSrc, int nDestBufSize )
{
	strncpy( pDest, pSrc, nDestBufSize -1 );
	pDest[nDestBufSize -1] = 0;
}


void SafeStrCpyEmptyChange( char* pDest, const char* pSrc, int nDestBufSize )
{
	// Hard.. Hard.. Temp.. 난중에 유연하게 바꾸도록 하자.. ㅋㅋㅋ
	// char b, char a   b->a 로 인자로 받아서 바꾸도록..
	char tmp[256] = { 0, };
	strncpy(tmp, pSrc, nDestBufSize-1);

	for(int i=0; i<256; i++)
	{
		if(tmp[i] == '_')		pDest[i] = ' ';
		else					pDest[i] = tmp[i];		
	}

	pDest[nDestBufSize-1] = 0;
}

char* GetCurTimeToString()
{
	static char temp[256];
	SYSTEMTIME ti;
	GetLocalTime(&ti);
	sprintf(temp,"%d월/%d일 %d시%d분%d.%d초",		// 월/일 시:분:초
		ti.wMonth,ti.wDay,
		ti.wHour,ti.wMinute,ti.wSecond,ti.wMilliseconds);
	return temp;
}

DWORD GetCurTimeValue()
{
	char temp2[256];

	SYSTEMTIME ti;
	GetLocalTime(&ti);
	sprintf(temp2,"%d%02d%02d",ti.wYear,ti.wMonth,ti.wDay);
	DWORD v2 = atoi(temp2);

	return v2;
}

DWORD GetCurTime()
{
	// 시간계산
	SYSTEMTIME systime;
	GetLocalTime( &systime );
	stTIME time;
	time.SetTime( systime.wYear-2000, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond );

	return time.value;
}

DWORD GetCurTimeToSecond()
{
	stTIME curtime;
	curtime.value = GetCurTime();

	DWORD timetosecond = 0;
	timetosecond += curtime.GetYear() * YEARTOSECOND;
	timetosecond += curtime.GetMonth() * MONTHTOSECOND;
	timetosecond += curtime.GetDay() * DAYTOSECOND;
	timetosecond += curtime.GetHour() * HOURTOSECOND;
	timetosecond += curtime.GetMinute() * MINUTETOSECOND;
	timetosecond += curtime.GetSecond();

	return timetosecond;
}

void StringtimeToSTTIME(stTIME* Time, char* strTime)
{
 	char year[3]		= { 0, };
 	char month[3]		= { 0, };
 	char day[3]			= { 0, };
 	char hour[3]		= { 0, };
 	char minute[3]		= { 0, };

	strncpy(year, &strTime[2], 2);
 	strncpy(month, &strTime[5], 2);
 	strncpy(day, &strTime[8], 2);
 	strncpy(hour, &strTime[11], 2);
 	strncpy(minute, &strTime[14],2);
 	Time->SetTime( atoi(year), atoi(month), atoi(day), atoi(hour), atoi(minute), 0 );
}

BOOL CheckValidPosition(VECTOR3& pos)
{
	if(pos.x < 0 || pos.z < 0)
		return FALSE;
	if(pos.x >= 51200 || pos.z >= 51200)
		return FALSE;
	return TRUE;
}


float roughGetLength( float fDistX,float fDistY )
{
	if( fDistX < 0.0f ) fDistX = -fDistX;
	if( fDistY < 0.0f ) fDistY = -fDistY;
	
	int min = (int)(( fDistX > fDistY ) ? fDistY : fDistX);

	return ( fDistX + fDistY - ( min >> 1 ) - ( min >> 2 ) + ( min >> 4 ) );
}

//////////////////////////////////////////////////////////////////////////
// Effect번호를 파일이름으로 찾기 위한 부분
map<string,DWORD> g_fileTable;

void LoadEffectFileTable(char* pListFileName)
{
#ifdef _CLIENT_
	CMHFile file;
	file.Init(pListFileName,"rb",MHFILE_FLAG_DONOTCHANGE_DIRECTORY_TOROOT);

	DWORD MaxEffectDesc = file.GetDword();
	
	DWORD effnum;
	char efffile[256];
	for(DWORD n=0;n<MaxEffectDesc;++n)
	{
		//ASSERT(!pFile->IsEOF());

		effnum = file.GetDword();
		file.GetString(efffile);
		strupr(efffile);
		if(strcmp(efffile,"NULL") == 0)
			continue;
		
		g_fileTable.insert(map<string,DWORD>::value_type(efffile,effnum));
	}	

	file.Release();
#endif
}

DWORD FindEffectNum(LPCTSTR pFileName)
{
#ifdef _CLIENT_
	TCHAR buffer[MAX_PATH] = {0};
	SafeStrCpy(
		buffer,
		pFileName,
		sizeof(buffer) / sizeof(*buffer));
	const map<string,DWORD>::const_iterator iterator = g_fileTable.find(
		_tcsupr(buffer));

	return g_fileTable.end() == iterator ? -1 : iterator->second;
#else
	return 0 < _tcslen(pFileName);
#endif
}

void ERRORBSGBOX(char* str,...)
{
	char msg2[255];
	va_list argList;

	va_start(argList, str);
	vsprintf(msg2,str,argList);
	va_end(argList);

	MessageBox(NULL,msg2,NULL,NULL);
	
}

// Weather System
const char* GetDay(WORD nDay)
{
	return Days[nDay];
}

WORD GetDay(const char* strDay)
{
	for(WORD i = 0; i < 7; i++)
	{
		if(strcmp(strDay, Days[i]) == 0)
			return i;
	}

	return (WORD)-1;
}

const char* GetWeather(WORD nWeatherState)
{
	return WeatherState[nWeatherState];
}

WORD GetWeather(const char* strWeather)
{
	for(WORD i = 0; i < eWS_Max; i++)
	{
		if(strcmp(strWeather, WeatherState[i]) == 0)
			return i;
	}

	return (WORD)-1;
}

BOOL IsEmptyLine(char* pszText)
{
	int nLen = strlen(pszText);

	// 문자열 중에 스페이스나 탭 외의 문자가 섞여 있으면 FALSE
	for(int i=0; i<nLen; i++)
	{
		if (pszText[i] == ' ' ||
			pszText[i] == '	')
		{
			continue;
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

// 0710111524 와 같은 int 형식의 날짜를 사용
int	 GetPastTime(DWORD nDate)
{
	SYSTEMTIME t;
	GetLocalTime(&t);

	DWORD nYear			= nDate/100000000;
	DWORD nMonth		= (nDate - nYear*100000000)/1000000;
	DWORD nDay			= (nDate - nYear*100000000 - nMonth*1000000)/10000;
	DWORD nHour			= (nDate - nYear*100000000 - nMonth*1000000 - nDay*10000)/100;
	DWORD nMin			= (nDate - nYear*100000000 - nMonth*1000000 - nDay*10000 - nHour*100);
	DWORD tblMonth[]	= {NULL, 1,	 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 1,	 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12};
	DWORD tblDay[]		= {NULL, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	// 4년마다 2월은 29일이당..
	if (t.wYear%4 == 0)
	{
		tblDay[2]  = 29;
		tblDay[14] = 29;
	}

	// 일단 년도 차이에 따른 일 계산
	int nPastTime = (t.wYear-2000 - nYear) * 365;

	// 현재 월이 과거 월보다 크다면 과거 월 -> 현재 월 순서로 날짜 합산
	if (t.wMonth >= nMonth)
	{
		for(int i=nMonth; i<25; i++)
		{
			if (t.wMonth == tblMonth[i])
			{
				break;
			}

			// 월 차이에 따른 일 계산
			nPastTime += tblDay[i];
		}
	}
	// 현재 월이 과거 월보다 작다면 현재 월 <- 과거 월 순서로 날짜 차감
	else
	{
		for(int i=nMonth; i>=0; i--)
		{
			if (t.wMonth == tblMonth[i])
			{
				break;
			}

			// 월 차이에 따른 일 계산
			nPastTime -= tblDay[i];
		}
	}

	// 일 차이, 시간에 따른 총 지난 시간 계산
	nPastTime += (t.wDay - nDay);
	nPastTime = nPastTime*24 + ((t.wHour - nHour) - (59 - (t.wMinute - nMin))/60);

	return nPastTime;
}

// '2007-12-11 17:46' 와 같은 char 형식의 날짜를 사용
int	 GetPastTime(char* pszDate)
{
	// 잘못된 버퍼를 사용했을 경우 처리
	if (strlen(pszDate) < 16) return 0;

	char szTmp[16] = {0,};

	SYSTEMTIME t;
	GetLocalTime(&t);

	DWORD nYear			= atoi(strncpy(szTmp, pszDate, 4));
	ZeroMemory(szTmp, sizeof(szTmp));
	DWORD nMonth		= atoi(strncpy(szTmp, &pszDate[5], 2));
	ZeroMemory(szTmp, sizeof(szTmp));
	DWORD nDay			= atoi(strncpy(szTmp, &pszDate[8], 2));
	ZeroMemory(szTmp, sizeof(szTmp));
	DWORD nHour			= atoi(strncpy(szTmp, &pszDate[11], 2));
	ZeroMemory(szTmp, sizeof(szTmp));
	DWORD nMin			= atoi(strncpy(szTmp, &pszDate[14], 2));
	ZeroMemory(szTmp, sizeof(szTmp));
	DWORD tblMonth[]	= {NULL, 1,	 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 1,	 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12};
	DWORD tblDay[]		= {NULL, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	// 4년마다 2월은 29일이당..
	if (t.wYear%4 == 0)
	{
		tblDay[2]  = 29;
		tblDay[14] = 29;
	}

	// 일단 년도 차이에 따른 일 계산
	int nPastTime = (t.wYear - nYear) * 365;
	// 현재 월이 과거 월보다 크다면 과거 월 -> 현재 월 순서로 날짜 합산
	if (t.wMonth >= nMonth)
	{
		for(int i=nMonth; i<25; i++)
		{
			if (t.wMonth == tblMonth[i])
			{
				break;
			}

			// 월 차이에 따른 일 계산
			nPastTime += tblDay[i];
		}
	}
	// 현재 월이 과거 월보다 작다면 현재 월 <- 과거 월 순서로 날짜 차감
	else
	{
		for(int i=nMonth; i>=0; i--)
		{
			if (t.wMonth == tblMonth[i])
			{
				break;
			}

			// 월 차이에 따른 일 계산
			nPastTime -= tblDay[i];
		}
	}

	// 일 차이, 시간에 따른 총 지난 시간 계산
	nPastTime += (t.wDay - nDay);
	nPastTime = nPastTime*24 + ((t.wHour - nHour) - (59 - (t.wMinute - nMin))/60);

	return nPastTime;
}

// 090109 LUJ, 스크립트 검증 값을 설정한다
void SetScriptCheckValue( ScriptCheckValue& checkValue )
{
#ifdef _UNICODE
	typedef	std::wstring Text;
#else
	typedef	std::string	Text;
#endif
	typedef std::list< Text >	NameList;
	NameList					nameList;

	// 090106 LUJ, 폴더에 불필요한 bin 파일이 있을 경우 접속 불능 사유가 되므로,
	//			불편해도 필수 파일을 수동으로 목록에 넣자
	// 090119 LUJ, 파일 이름에 붙은 확장자 제거/파일 목록 수정. 확장자는 빼고
	//			파일명을 입력해야한다. 또한, 확인이 쉽도록 정렬시켜 입력하자.
	{
		nameList.push_back( _T( "BodyList_E_M" ) );
		nameList.push_back( _T( "BodyList_E_W" ) );
		nameList.push_back( _T( "BodyList_H_M" ) );
		nameList.push_back( _T( "BodyList_H_W" ) );
		nameList.push_back( _T( "BossMonsterfileList" ) );
		nameList.push_back( _T( "BRList" ) );
		nameList.push_back( _T( "CharMake_List" ) );
		nameList.push_back( _T( "DateMatching" ) );
		nameList.push_back( _T( "Dealitem" ) );
		nameList.push_back( _T( "Event_get_list" ) );
		nameList.push_back( _T( "Family" ) );
		nameList.push_back( _T( "Farm" ) );
		nameList.push_back( _T( "FarmRenderObj" ) );
		nameList.push_back( _T( "FieldBossList" ) );
		nameList.push_back( _T( "FilterWord" ) );
		nameList.push_back( _T( "FishingExpPoint" ) );
		nameList.push_back( _T( "FishingMissionCode" ) );
		nameList.push_back( _T( "FishingRateDefine" ) );
		nameList.push_back( _T( "guild_setting" ) );
		nameList.push_back( _T( "ItemApplyOption" ) );
		nameList.push_back( _T( "ItemBodyChange" ) );
		nameList.push_back( _T( "itemCoolTime" ) );
		nameList.push_back( _T( "itemDissolve" ) );
		nameList.push_back( _T( "ItemEnchant" ) );
		nameList.push_back( _T( "ItemList" ) );
		nameList.push_back( _T( "ItemMix" ) );
		nameList.push_back( _T( "itemReinforce" ) );
		nameList.push_back( _T( "LoginPoint" ) );
		nameList.push_back( _T( "MapMoveList" ) );
		nameList.push_back( _T( "MapMoveScroll" ) );
		nameList.push_back( _T( "mix_setting" ) );
		nameList.push_back( _T( "MonsterList" ) );
		nameList.push_back( _T( "MonsterMeterReward" ) );
		nameList.push_back( _T( "MonSummonItem" ) );
		nameList.push_back( _T( "NpcBuffList" ) );
		nameList.push_back( _T( "NpcList" ) );
		nameList.push_back( _T( "NpcRecallInfo" ) );
		nameList.push_back( _T( "Pet_Costume" ) );
		nameList.push_back( _T( "PetHPMPInfo" ) );
		nameList.push_back( _T( "Petlist" ) );
		nameList.push_back( _T( "PetStatusInfo" ) );
		nameList.push_back( _T( "PetSystemInfo" ) );
		nameList.push_back( _T( "QuestItemList" ) );
		nameList.push_back( _T( "QuestScript" ) );
		nameList.push_back( _T( "ResidentRegist" ) );
		nameList.push_back( _T( "SafetyLoc" ) );
		nameList.push_back( _T( "SetItem" ) );
		nameList.push_back( _T( "SkillList" ) );
		nameList.push_back( _T( "SkillMoney" ) );
		nameList.push_back( _T( "Skill_Buff_List" ) );
		nameList.push_back( _T( "skill_get_list" ) );
		nameList.push_back( _T( "StorageListInfo" ) );
	}

	LONGLONG	base		= 1;
	const Text	path		= _T( "system\\resource\\" );
	const Text	extenstion	= _T( ".bin" );

	// 090106 LUJ, 코드 값을 얻어낸다
	for(	NameList::const_iterator nameIterator = nameList.begin();
			nameList.end() != nameIterator;
			++nameIterator, base *= 10 )
	{
		const Text& name( *nameIterator );
		const Text	fileName( path + name + extenstion );

		CMHFile mhFile;
		mhFile.Init( const_cast< TCHAR* >( fileName.c_str() ), _T( "rb" ) );

		checkValue.mValue = ( checkValue.mValue + LONGLONG( mhFile.GetCrc1() ) * base + LONGLONG( mhFile.GetCrc2() ) * base * 10 );
	}
}

BOOL IsCharInString( char* pStr, char* pNoChar )
{
	while( *pStr )
	{
		if( IsDBCSLeadByte( *pStr ) )
		{
			++pStr;			
		}
		else
		{
			char* p = pNoChar;
			while( *p )
			{
				if( *pStr == *p ) return TRUE;
				++p;
			}
		}
		++pStr;
	}

	return FALSE;
}

// 090319 NYJ - 하우징시스템 사용 공용함수
DWORD Get_HighCategory(DWORD FunitureIndex)	{return FunitureIndex/100000000;}
DWORD Get_LowCategory(DWORD FunitureIndex)	{return (FunitureIndex % 100000000) / 1000000;}
DWORD Get_Work(DWORD FunitureIndex)			{return (FunitureIndex % 1000000) / 10000;}
DWORD Get_Index(DWORD FunitureIndex)		{return (FunitureIndex % 10000) / 10;}
DWORD Get_Function(DWORD FunitureIndex)		{return FunitureIndex % 10;}

BOOL Get_SplitedFurnitureIndexFromFileName(char* pFileName, DWORD& HighCategory, DWORD& LowCategory, DWORD& Work, DWORD& Index, DWORD& Function)
{
	if(pFileName)
	{
		char szSplit[5][32] = {0,};
		strupr(pFileName);

		char* pString = strtok(pFileName, "_");

		int nCnt = 0;
		while(NULL != pString)
		{
			if(nCnt > 5)
				return FALSE;

			strcpy(szSplit[nCnt], pString);
			nCnt++;
			pString = strtok(NULL, "_");
		}

		if(nCnt < 5)
			return FALSE;


		// 대분류
		if(!strcmp(szSplit[0], HOUSE_HIGHCATEGORY_CHAR_FURINITURE))
		{
			HighCategory = eHOUSE_HighCategory_Furniture;

			// 가구소분류
			if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_FURNITURE_ETC))
				LowCategory = eHOUSE_LowCategory_Furniture_Etc;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_FURNITURE_TABLE))
				LowCategory = eHOUSE_LowCategory_Furniture_Table;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_FURNITURE_CHAIR))
				LowCategory = eHOUSE_LowCategory_Furniture_Chair;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_FURNITURE_SOFA))
				LowCategory = eHOUSE_LowCategory_Furniture_Sofa;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_FURNITURE_CLOSET))
				LowCategory = eHOUSE_LowCategory_Furniture_Closet;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_FURNITURE_CHEST))
				LowCategory = eHOUSE_LowCategory_Furniture_Chest;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_FURNITURE_DRAWER))
				LowCategory = eHOUSE_LowCategory_Furniture_Drawer;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_FURNITURE_BED))
				LowCategory = eHOUSE_LowCategory_Furniture_Bed;
			else
				return FALSE;
		}
		else if(!strcmp(szSplit[0], HOUSE_HIGHCATEGORY_CHAR_ELECTRIC))
		{
			HighCategory = eHOUSE_HighCategory_Electric;

			// 가전소분류
			if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_ELECTRIC_ETC))
				LowCategory = eHOUSE_LowCategory_Electric_Etc;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_ELECTRIC_REF))
				LowCategory = eHOUSE_LowCategory_Electric_Ref;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_ELECTRIC_TV))
				LowCategory = eHOUSE_LowCategory_Electric_Tv;
			else
				return FALSE;
		}
		else if(!strcmp(szSplit[0], HOUSE_HIGHCATEGORY_CHAR_DOOR))
		{
			HighCategory = eHOUSE_HighCategory_Door;

			// 문소분류
			if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_DOOR_ETC))
				LowCategory = eHOUSE_LowCategory_Door_Etc;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_DOOR_FRAME))
				LowCategory = eHOUSE_LowCategory_Door_Frame;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_DOOR_WINDOWS))
				LowCategory = eHOUSE_LowCategory_Door_Window;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_DOOR_WATCH))
				LowCategory = eHOUSE_LowCategory_Door_Watch;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_DOOR_DOOR))
				LowCategory = eHOUSE_LowCategory_Door_Door;
			else
				return FALSE;
		}
		else if(!strcmp(szSplit[0], HOUSE_HIGHCATEGORY_CHAR_WALL))
		{
			HighCategory = eHOUSE_HighCategory_Wall;

			// 벽소분류
			if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_WALL_ETC))
				LowCategory = eHOUSE_LowCategory_Wall_Etc;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_WALL_WALL))
				LowCategory = eHOUSE_LowCategory_Wall_Wall;
			else
				return FALSE;
		}
		else if(!strcmp(szSplit[0], HOUSE_HIGHCATEGORY_CHAR_CARPET))
		{
			HighCategory = eHOUSE_HighCategory_Carpet;

			// 바닥소분류
			if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_CARPET_ETC))
				LowCategory = eHOUSE_LowCategory_Carpet_Etc;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_CARPET_TILE))
				LowCategory = eHOUSE_LowCategory_Carpet_Tile;
			
			else
				return FALSE;
		}
		else if(!strcmp(szSplit[0], HOUSE_HIGHCATEGORY_CHAR_PROPERTIES))
		{
			HighCategory = eHOUSE_HighCategory_Properties;

			// 소품소분류
			if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_PROPERTIES_ETC))
				LowCategory = eHOUSE_LowCategory_Properties_Etc;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_PROPERTIES_FRAME))
				LowCategory = eHOUSE_LowCategory_Properties_Frame;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_PROPERTIES_FLOWERPOT))
				LowCategory = eHOUSE_LowCategory_Properties_FlowerPot;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_PROPERTIES_CUSHION))
				LowCategory = eHOUSE_LowCategory_Properties_Cushion;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_PROPERTIES_CLOCK))
				LowCategory = eHOUSE_LowCategory_Properties_Clock;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_PROPERTIES_DOLL))
				LowCategory = eHOUSE_LowCategory_Properties_Doll;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_PROPERTIES_BASKET))
				LowCategory = eHOUSE_LowCategory_Properties_Basket;
			else
				return FALSE;
		}
		else if(!strcmp(szSplit[0], HOUSE_HIGHCATEGORY_CHAR_DECORATION))
		{
			HighCategory = eHOUSE_HighCategory_Decoration;

			// 장식소분류
			if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_DECORATION_ETC))
				LowCategory = eHOUSE_LowCategory_Decoration_Etc;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_DECORATION_FIREPLACE))
				LowCategory = eHOUSE_LowCategory_Decoration_FirePlace;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_DECORATION_MARK))
				LowCategory = eHOUSE_LowCategory_Decoration_Mark;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_DECORATION_LAMP))
				LowCategory = eHOUSE_LowCategory_Decoration_Lamp;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_DECORATION_BRONZE))
				LowCategory = eHOUSE_LowCategory_Decoration_Bronze;
			else if(!strcmp(szSplit[1], HOUSE_LOWCATEGORY_CHAR_DECORATION_CCOVER))
				LowCategory = eHOUSE_LowCategory_Decoration_CCover;
			else
				return FALSE;
		}
		else
			return FALSE;


		// 작업
		if(!strcmp(szSplit[2], HOUSE_WORK_CHAR_A0))
			Work = eHOUSE_WorkIndex_A0;
		else if(!strcmp(szSplit[2], HOUSE_WORK_CHAR_B0))
			Work = eHOUSE_WorkIndex_B0;
		else if(!strcmp(szSplit[2], HOUSE_WORK_CHAR_C0))
			Work = eHOUSE_WorkIndex_C0;
		else if(!strcmp(szSplit[2], HOUSE_WORK_CHAR_D0))
			Work = eHOUSE_WorkIndex_D0;
		else if(!strcmp(szSplit[2], HOUSE_WORK_CHAR_E0))
			Work = eHOUSE_WorkIndex_E0;
		else if(!strcmp(szSplit[2], HOUSE_WORK_CHAR_F0))
			Work = eHOUSE_WorkIndex_F0;
		else
			return FALSE;

		// 인덱스
		Index = atoi(szSplit[3]);

		// 기능
		if(!strcmp(szSplit[4], HOUSE_FUNCTION_CHAR_NONE))
			Function = eHOUSE_Function_None;
		else if(!strcmp(szSplit[4], HOUSE_FUNCTION_CHAR_WALL))
			Function = eHOUSE_Function_Wall;
		else if(!strcmp(szSplit[4], HOUSE_FUNCTION_CHAR_FLOOR))
			Function = eHOUSE_Function_Floor;
		else if(!strcmp(szSplit[4], HOUSE_FUNCTION_CHAR_DOOR))
			Function = eHOUSE_Function_Door;
		else if(!strcmp(szSplit[4], HOUSE_FUNCTION_CHAR_START))
			Function = eHOUSE_Function_Start;
		else if(!strcmp(szSplit[4], HOUSE_FUNCTION_CHAR_ACTION))
			Function = eHOUSE_Function_Action;
		else
			return FALSE;
	}

	return TRUE;
}

char* Get_HighCategorySeparator(DWORD FunitureIndex)
{
	static char szBuf[32] = {0,};
	DWORD dwHighCategory = Get_HighCategory(FunitureIndex);

	switch(dwHighCategory)
	{
		case eHOUSE_HighCategory_Furniture:		strcpy(szBuf, "FU_");		break;
		case eHOUSE_HighCategory_Electric:		strcpy(szBuf, "EL_");		break;
		case eHOUSE_HighCategory_Door:			strcpy(szBuf, "DO_");		break;
		case eHOUSE_HighCategory_Wall:			strcpy(szBuf, "WA_");		break;
		case eHOUSE_HighCategory_Carpet:		strcpy(szBuf, "CA_");		break;
		case eHOUSE_HighCategory_Properties:	strcpy(szBuf, "PR_");		break;
		case eHOUSE_HighCategory_Decoration:	strcpy(szBuf, "DE_");		break;
		default:
			return NULL;
	}

	return szBuf;
}

char* Get_LowCategorySeparator(DWORD FunitureIndex)
{
	static char szBuf[32] = {0,};

	DWORD dwHighCategory = Get_HighCategory(FunitureIndex);
	DWORD dwLowCategory = Get_LowCategory(FunitureIndex);

	switch(dwHighCategory)
	{
		case eHOUSE_HighCategory_Furniture:
			{
				if(eHOUSE_LowCategory_Furniture_Bed		== dwLowCategory)		strcpy(szBuf, "BED_");
				else if(eHOUSE_LowCategory_Furniture_Table	== dwLowCategory)		strcpy(szBuf, "TABLE_");
				else if(eHOUSE_LowCategory_Furniture_Chair	== dwLowCategory)		strcpy(szBuf, "CHAIR_");
				else if(eHOUSE_LowCategory_Furniture_Sofa	== dwLowCategory)		strcpy(szBuf, "SOFA_");
				else if(eHOUSE_LowCategory_Furniture_Closet	== dwLowCategory)		strcpy(szBuf, "CLOSET_");
				else if(eHOUSE_LowCategory_Furniture_Chest	== dwLowCategory)		strcpy(szBuf, "CHEST_");
				else if(eHOUSE_LowCategory_Furniture_Drawer	== dwLowCategory)		strcpy(szBuf, "DRAWER_");
				else if(eHOUSE_LowCategory_Furniture_Etc	== dwLowCategory)		strcpy(szBuf, "ETC_");
				else
					return NULL;
			}
			break;
		case eHOUSE_HighCategory_Electric:
			{
				if(eHOUSE_LowCategory_Electric_Tv	== dwLowCategory)			strcpy(szBuf, "TV_");
				else if(eHOUSE_LowCategory_Electric_Ref	== dwLowCategory)			strcpy(szBuf, "REF_");
				else if(eHOUSE_LowCategory_Electric_Etc	== dwLowCategory)			strcpy(szBuf, "ETC_");
				else
					return NULL;
			}
			break;
		case eHOUSE_HighCategory_Door:
			{
				if(eHOUSE_LowCategory_Door_Door		== dwLowCategory)			strcpy(szBuf, "DOOR_");
				else if(eHOUSE_LowCategory_Door_Frame	== dwLowCategory)			strcpy(szBuf, "FRAME_");
				else if(eHOUSE_LowCategory_Door_Window	== dwLowCategory)			strcpy(szBuf, "WINDOW_");
				else if(eHOUSE_LowCategory_Door_Watch	== dwLowCategory)			strcpy(szBuf, "WATCH_");
				//090324 pdy 
				else if(eHOUSE_LowCategory_Door_Etc		== dwLowCategory)			strcpy(szBuf, "ETC_");
				else
					return NULL;
			}
			break;
		case eHOUSE_HighCategory_Wall:
			{
				if(eHOUSE_LowCategory_Wall_Wall	== dwLowCategory)				strcpy(szBuf, "WALL_");
				else if(eHOUSE_LowCategory_Wall_Etc	== dwLowCategory)				strcpy(szBuf, "ETC_");
				else
					return NULL;
			}
			break;
		case eHOUSE_HighCategory_Carpet:
			{
				if(eHOUSE_LowCategory_Carpet_Tile	== dwLowCategory)			strcpy(szBuf, "TILE_");
				else if(eHOUSE_LowCategory_Carpet_Etc	== dwLowCategory)			strcpy(szBuf, "ETC_");
				else
					return NULL;
			}
			break;
		case eHOUSE_HighCategory_Properties:
			{
				if(eHOUSE_LowCategory_Properties_Cushion	== dwLowCategory)	strcpy(szBuf, "CUSHION_");
				else if(eHOUSE_LowCategory_Properties_Frame		== dwLowCategory)	strcpy(szBuf, "FRAME_");
				else if(eHOUSE_LowCategory_Properties_FlowerPot	== dwLowCategory)	strcpy(szBuf, "FLOWERPOT_");
				else if(eHOUSE_LowCategory_Properties_Etc		== dwLowCategory)	strcpy(szBuf, "ETC_");
				else if(eHOUSE_LowCategory_Properties_Clock		== dwLowCategory)	strcpy(szBuf, "CLOCK_");
				else if(eHOUSE_LowCategory_Properties_Doll		== dwLowCategory)	strcpy(szBuf, "DOLL_");
				else if(eHOUSE_LowCategory_Properties_Basket		== dwLowCategory)	strcpy(szBuf, "BASKET_");
				else
					return NULL;
			}
			break;
		case eHOUSE_HighCategory_Decoration:
			{
				if(eHOUSE_LowCategory_Decoration_Bronze		== dwLowCategory)	strcpy(szBuf, "BRONZE_");
				else if(eHOUSE_LowCategory_Decoration_FirePlace	== dwLowCategory)	strcpy(szBuf, "FIREPLACE_");
				else if(eHOUSE_LowCategory_Decoration_Mark		== dwLowCategory)	strcpy(szBuf, "MARK_");
				else if(eHOUSE_LowCategory_Decoration_Lamp		== dwLowCategory)	strcpy(szBuf, "LAMP_");
				else if(eHOUSE_LowCategory_Decoration_CCover	== dwLowCategory)	strcpy(szBuf, "CCOVER_");
				else if(eHOUSE_LowCategory_Decoration_Etc		== dwLowCategory)	strcpy(szBuf, "ETC_");
				else
					return NULL;
			}
			break;
		default:
			return NULL;
	}

	return szBuf;
}

char* Get_WorkSeparator(DWORD FunitureIndex)
{
	static char szBuf[32] = {0,};
	DWORD dwWork = Get_Work(FunitureIndex);

	switch(dwWork)
	{
		case eHOUSE_WorkIndex_A0:		strcpy(szBuf, "A0_");		break;
		case eHOUSE_WorkIndex_B0:		strcpy(szBuf, "B0_");		break;
		case eHOUSE_WorkIndex_C0:		strcpy(szBuf, "C0_");		break;
		case eHOUSE_WorkIndex_D0:		strcpy(szBuf, "D0_");		break;
		case eHOUSE_WorkIndex_E0:		strcpy(szBuf, "E0_");		break;
		case eHOUSE_WorkIndex_F0:		strcpy(szBuf, "F0_");		break;
		default:
			return NULL;
	}

	return szBuf;
}

char* Get_IndexSeparator(DWORD FunitureIndex)
{
	static char szBuf[32] = {0,};
	DWORD dwIndex = Get_Index(FunitureIndex);

	sprintf(szBuf, "%03d_", dwIndex);
	return szBuf;
}

char* Get_FuntionSeparator(DWORD FunitureIndex)
{
	static char szBuf[32] = {0,};
	DWORD dwFunction = Get_Function(FunitureIndex);

	switch(dwFunction)
	{
		case eHOUSE_Function_None:		strcpy(szBuf, "N");		break;
		case eHOUSE_Function_Wall:		strcpy(szBuf, "W");		break;
		case eHOUSE_Function_Floor:		strcpy(szBuf, "F");		break;
		case eHOUSE_Function_Door:		strcpy(szBuf, "D");		break;
		case eHOUSE_Function_Start:		strcpy(szBuf, "S");		break;
		case eHOUSE_Function_Action:	strcpy(szBuf, "A");		break;
		default:
			return NULL;
	}

	return szBuf;
}

char* ConvertFunitureIndex2FileName(DWORD FurnitureIndex)
{
	static char szBuf[256] = {0,};

	char* pHighCategory = Get_HighCategorySeparator(FurnitureIndex);
	char* pLowCategory = Get_LowCategorySeparator(FurnitureIndex);
	char* pWork = Get_WorkSeparator(FurnitureIndex);
	char* pIndex = Get_IndexSeparator(FurnitureIndex);
	char* pFuntion = Get_FuntionSeparator(FurnitureIndex);

	if(!pHighCategory || !pLowCategory || !pWork || !pIndex || !pFuntion)
		return NULL;

	strcpy(szBuf, pHighCategory);
	strcat(szBuf, pLowCategory);
	strcat(szBuf, pWork);
	strcat(szBuf, pIndex);
	strcat(szBuf, pFuntion);
	return szBuf;
}

DWORD ConvertFileName2FunitureIndex(char* pFileName)
{
	DWORD dwHighCategory, dwLowCategory, dwWork, dwIndex, dwFunction;
	dwHighCategory = dwLowCategory = dwWork = dwIndex = dwFunction = 0;

	if(Get_SplitedFurnitureIndexFromFileName(pFileName, dwHighCategory, dwLowCategory, dwWork, dwIndex, dwFunction))
	{
		DWORD dwFurnitureIndex = 0;
		dwFurnitureIndex += dwHighCategory * 100000000;		// 대분류
		dwFurnitureIndex += dwLowCategory * 1000000;		// 소분류
		dwFurnitureIndex += dwWork * 10000;					// 작업
		dwFurnitureIndex += dwIndex * 10;					// 인덱스
		dwFurnitureIndex += dwFunction;						// 기능

		return dwFurnitureIndex;
	}

	return 0;
}

BOOL IsExteriorWall(DWORD dwFurnitureIndex)
{
	DWORD dwFunction = Get_Function(dwFurnitureIndex);
	if(eHOUSE_Function_Wall == dwFunction)
		return TRUE;

	return FALSE;
}

BOOL IsInteriorWall(DWORD dwFurnitureIndex)
{
	DWORD dwFunction, dwHighCategory, dwLowCategory;
	dwFunction = Get_Function(dwFurnitureIndex);
	dwHighCategory = Get_HighCategory(dwFurnitureIndex);
	dwLowCategory = Get_LowCategory(dwFurnitureIndex);
	
	if(eHOUSE_Function_None==dwFunction &&
		dwHighCategory==eHOUSE_HighCategory_Wall &&
		dwLowCategory==eHOUSE_LowCategory_Wall_Wall)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL IsFloor(DWORD dwFurnitureIndex)
{
	DWORD dwFunction = Get_Function(dwFurnitureIndex);
	if(eHOUSE_Function_Floor == dwFunction)
		return TRUE;

	return FALSE;
}

BOOL IsCeiling(DWORD dwFurnitureIndex)
{
	DWORD dwHighCategory = Get_HighCategory(dwFurnitureIndex);
	DWORD dwLowCategory = Get_LowCategory(dwFurnitureIndex);
	
	if(dwHighCategory==eHOUSE_HighCategory_Decoration &&
		dwLowCategory==eHOUSE_LowCategory_Decoration_CCover)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL IsDoor(DWORD dwFurnitureIndex)
{
	DWORD dwFunction = Get_Function(dwFurnitureIndex);
	if(eHOUSE_Function_Door == dwFunction)
		return TRUE;

	return FALSE;
}

BOOL IsStart(DWORD dwFurnitureIndex)
{
	DWORD dwFunction = Get_Function(dwFurnitureIndex);
	if(eHOUSE_Function_Start == dwFunction)
		return TRUE;

	return FALSE;
}

BOOL IsActionable(DWORD dwFurnitureIndex)
{
	DWORD dwFunction = Get_Function(dwFurnitureIndex);
	if(eHOUSE_Function_Action == dwFunction)
		return TRUE;

	return FALSE;
}

void OutputDebug( char* pMsg, ... )
{
	char msg[1024] = {0,};
	
	va_list vList;
	va_start( vList, pMsg );
	vsprintf( msg, pMsg, vList );
	va_end( vList );

	SYSTEMTIME time = { 0 };
	::GetLocalTime( &time );

	TCHAR log[ 2048 ] = { 0 };
	_stprintf(
		log,
		_T( "%02d:%02d:%02d> %s\n" ),
		time.wHour,
		time.wMinute,
		time.wSecond,
		msg );
	OutputDebugString( log );

	// 091116 LUJ, 콘솔 창이 닫힌 때에는 로그를 출력하지 않도록 하여 부하를 최소화함
	if(const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE))
	{
		ULONG character = 0;
		WriteConsole(
			handle,
			log,
			_tcslen(log),
			&character,
			0);
	}
}

// 090909 ONS 스킬트리 정보를 가져오는 함수
#ifdef _CLIENT_
SkillData* GetSkillTreeList()
{
	static SkillData pSkillList[MAX_SKILL_TREE] = {0};
	const WORD JobGrade = HERO->GetCharacterTotalInfo()->JobGrade;

	WORD wPos = 0 ;												// 위치 값을 담을 변수.
	for( WORD CurGrade = JobGrade; CurGrade > 0; CurGrade-- )
	{
		BYTE byJobType = HERO->GetCharacterTotalInfo()->Job[ CurGrade - 1 ] ;

		// 직업 계열의 유효성 체크.
		if( CurGrade == 1 ) byJobType = 1 ;
		if(!byJobType) break;


		// 현제 레벨에 맞는 직업 인덱스를 받는다.
		WORD wJobIdx = 0 ;											
		wJobIdx = ( HERO->GetCharacterTotalInfo()->Job[ 0 ] * 1000 ) +
				( ( HERO->GetCharacterTotalInfo()->Race + 1 ) * 100 ) + 
				( ( CurGrade ) * 10 ) + 
				byJobType ;


		// 직업에 맞는 스킬 트리 정보를 받는다.
		SKILL_TREE_INFO* pInfo = SKILLMGR->GetSkillTreeInfo( wJobIdx ) ;
		if(!pInfo) break ;

		// 직업에 따른 스킬 개수 만큼 for문을 돌며 스킬을 추가한다.
		for(WORD wCount = 0 ; wCount < pInfo->Count ; wCount++ )
		{
			DWORD dwIndex = ( ( DWORD )pInfo->SkillIndex[ wCount ] / 100 ) * 100 + 1 ;
			DWORD dwLevel = pInfo->SkillIndex[ wCount ] % 100 ;
			cSkillInfo* const pSkillInfo = SKILLMGR->GetSkillInfo( dwIndex ) ;

			if( !pSkillInfo ) continue ;

			BYTE byChecked = FALSE ;

			for(WORD wCount2 = 0 ; wCount2 < wPos ; wCount2++ )
			{
				if( pSkillList[ wCount2 ].index == dwIndex )
				{
					byChecked = TRUE ;
					break ;
				}
			}
			if( byChecked ) continue ;

			// 스킬 리스트에 인덱스와 레벨을 추가한다.
			pSkillList[ wPos ].index = dwIndex ;
			pSkillList[ wPos ].level = (BYTE)dwLevel ;

			wPos = (wPos + 1) ;
		}
	}

	return pSkillList;
}

// 091112 ONS 금액표시 색상 반환 함수 추가
DWORD GetMoneyColor( DWORD dwMoney )
{
	DWORD dwColor;

	if( dwMoney < 10 )
	{
		dwColor = TTTC_ONE;
	}
	else if( dwMoney < 100 )
	{
		dwColor = TTTC_TEN;
	}
	else if( dwMoney < 1000 )
	{
		dwColor = TTTC_HUNDRED;
	}
	else if( dwMoney < 10000 )
	{
		dwColor = TTTC_THOUSAND;
	}
	else if( dwMoney < 100000 )
	{
		dwColor = TTTC_10THOUSAND;
	}
	else if( dwMoney < 1000000 )
	{
		dwColor = TTTC_ONDERMILLION;
	}
	else if( dwMoney < 10000000 )
	{
		dwColor = TTTC_MILLION;
	}
	else if( dwMoney < 100000000 )
	{
		dwColor = TTTC_10MILLION;
	}
	else //if( mMoney < 1000000000 )
	{
		dwColor = TTTC_100MILLION;
	}

	return dwColor;
}
#endif

// 출처: http://www.gpgstudy.com/forum/viewtopic.php?t=795&highlight=%C7%D8%BD%AC+%B9%AE%C0%DA%BF%AD
DWORD GetHashCodeFromTxt( LPCTSTR text )
{
	const DWORD length = strlen(text);
	DWORD hashCode = 5381;

	for(DWORD i = 0; i < length; ++i)
	{
		DWORD ch = DWORD(text[ i ]);
		hashCode = ((hashCode << 5) + hashCode) + ch;
	}

	return hashCode;
}

VECTOR3 GetRadToVector( const float fRad )
{
	MATRIX4 mat={0};
	VECTOR3 directionVector;
	SetVector3( &directionVector, 0, 0, -1 );
	SetRotationYMatrix( &mat, -fRad );
	TransformVector3_VPTR2( &directionVector, &directionVector, &mat ,1 );
	return directionVector;
}

DWORD DecodeHidePartsFlag(const char flagArray[eWearedItem_Max])
{
	DWORD flag = 0;

	if(flagArray[eWearedItem_Band])
	{
		flag |= eOPT_HIDEPARTS_EQUIP_BAND;
	}

	if(flagArray[eWearedItem_Glasses])
	{
		flag |= eOPT_HIDEPARTS_EQUIP_GLASSES;
	}

	if(flagArray[eWearedItem_Wing])
	{
		flag |= eOPT_HIDEPARTS_EQUIP_WING;
	}

	if(flagArray[eWearedItem_Costume_Head])
	{
		flag |= eOPT_HIDEPARTS_COSTUME_HEAD;
	}

	if(flagArray[eWearedItem_Costume_Dress])
	{
		flag |= eOPT_HIDEPARTS_COSTUME_DRESS;
	}

	if(flagArray[eWearedItem_Costume_Glove])
	{
		flag |= eOPT_HIDEPARTS_COSTUME_GLOVE;
	}

	if(flagArray[eWearedItem_Costume_Shoes])
	{
		flag |= eOPT_HIDEPARTS_COSTUME_GLOVE;
	}

	return flag;
}

void EncodeHidePartsFlag(DWORD flag, char flagArray[eWearedItem_Max])
{
	flagArray[eWearedItem_Band] = (0 < (flag & eOPT_HIDEPARTS_EQUIP_BAND));
	flagArray[eWearedItem_Glasses] = (0 < (flag & eOPT_HIDEPARTS_EQUIP_GLASSES));
	flagArray[eWearedItem_Wing] = (0 < (flag & eOPT_HIDEPARTS_EQUIP_WING));
	flagArray[eWearedItem_Costume_Head] = (0 < (flag & eOPT_HIDEPARTS_COSTUME_HEAD));
	flagArray[eWearedItem_Costume_Dress] = (0 < (flag & eOPT_HIDEPARTS_COSTUME_DRESS));
	flagArray[eWearedItem_Costume_Glove] = (0 < (flag & eOPT_HIDEPARTS_COSTUME_GLOVE));
	flagArray[eWearedItem_Costume_Shoes] = (0 < (flag & eOPT_HIDEPARTS_COSTUME_SHOES));
}