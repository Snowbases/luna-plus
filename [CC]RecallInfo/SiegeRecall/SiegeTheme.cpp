//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeTheme.cpp
//	DESC		: Implementation part of CSiegeTheme class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "./SiegeTheme.h"

#include "./SiegeMap.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeTheme
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------
CSiegeTheme::CSiegeTheme(void)
{
	// Initialize variables.
	m_byIdx = 0 ;

	m_wThemeIdx = 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CSiegeTheme
//	DESC		: The function destructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------
CSiegeTheme::~CSiegeTheme(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_Idx
//	DESC		: The function to setting index.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeTheme::Set_Idx(BYTE byIdx)
{
	// Check index limit.
	if( byIdx >= MAX_SIEGETHEME_ID )
	{
		Throw_Error("Failed to setting theme index, Over limit!!", __FUNCTION__) ;
		return ;
	}


	// Setting index.
	m_byIdx = byIdx ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_ThemeIdx
//	DESC		: The function to setting theme index.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeTheme::Set_ThemeIdx(MAPTYPE wIdx) 
{
	// Check index limit.
	// 081028 LYW --- SiegeTheme : 공성 테마 인덱스로 0이 사용되고 있으므로 0체크를 수정한다.
	//if( wIdx == 0 || wIdx >= MAX_THEME_IDX )
	if( wIdx >= MAX_THEME_IDX )
	{
		Throw_Error("Failed to setting theme index, Over limit!!", __FUNCTION__) ;
		return ;
	}


	// Setting teme index.
	m_wThemeIdx = wIdx ; 
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_SiegeMap
//	DESC		: The function to add siege map.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeTheme::Add_SiegeMap(CSiegeMap* pMap)
{
	// Check parameter.
	if(!pMap)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Setting map index.
	BYTE byMapIdx = (BYTE)m_mMap.size() ;

	pMap->Set_Idx(byMapIdx) ;


	// Check map number.
	/*MAPTYPE mapNum = 0 ;
	mapNum = pMap->Get_MapNum() ;

	if( mapNum == 0 )
	{
		Throw_Error("Invalid a map number!!", __FUNCTION__) ;
		return ;
	}*/


	// Insert a siege map info to this theme.
	m_mMap.insert(std::make_pair(byMapIdx, *pMap)) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_SiegeMapByMapNum
//	DESC		: The function to return map info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
CSiegeMap* CSiegeTheme::Get_SiegeMapByMapNum( MAPTYPE mapNum )
{
	// Receive a iterator.
	M_MAP::iterator it ;
	for( it = m_mMap.begin() ; it != m_mMap.end() ; ++it )
	{
		if( it == m_mMap.end() ) return NULL ;

		if(it->second.Get_MapNum() != mapNum ) continue ;

		return &it->second ;
	}

	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_SiegeMapByIdx
//	DESC		: The function to return map info by index.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
CSiegeMap* CSiegeTheme::Get_SiegeMapByIdx( BYTE byIdx )
{
	// Receive a iterator.
	M_MAP::iterator it ;
	it = m_mMap.find( byIdx ) ;


	// Return map info.
	if( it == m_mMap.end() ) return NULL ;
	else return &it->second ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeTheme::Throw_Error(char* szErr, char* szCaption)
{
#ifdef _CLIENT_

	#ifdef _GMTOOL_
		MessageBox( NULL, szErr, szCaption, MB_OK) ;
	#endif //_GMTOOL_

#else

		// Check parameter of this function.
		if(!szErr || !szCaption) return ;

		// Check err string size.
		if(strlen(szErr) <= 1)
		{
	#ifdef _USE_NPCRECALL_ERRBOX_
			MessageBox( NULL, "Invalid err string size!!", __FUNCTION__, MB_OK ) ;
	#else
			char tempStr[257] = {0, } ;

			SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
			strcat( tempStr, " - " ) ;
			strcat( tempStr, "Invalid err string size!!" ) ;
			WriteLog( tempStr ) ;
	#endif // _USE_NPCRECALL_ERRBOX_
		}


		// Check caption string size.
		if(strlen(szCaption) <= 1)
		{
	#ifdef _USE_NPCRECALL_ERRBOX_
			MessageBox( NULL, "Invalid caption string size!!", __FUNCTION__, MB_OK ) ;
	#else
			char tempStr[257] = {0, } ;

			SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
			strcat( tempStr, " - " ) ;
			strcat( tempStr, "Invalid caption string size!!" ) ;
			WriteLog( tempStr ) ;
	#endif // _USE_NPCRECALL_ERRBOX_
		}

		// Print a err message.
	#ifdef _USE_NPCRECALL_ERRBOX_
		MessageBox( NULL, szErr, szCaption, MB_OK) ;
	#else
		char tempStr[257] = {0, } ;

		SafeStrCpy(tempStr, szCaption, 256) ;
		strcat(tempStr, " - ") ;
		strcat(tempStr, szErr) ;
		WriteLog(tempStr) ;
	#endif // _USE_NPCRECALL_ERRBOX_

#endif //_CLIENT_
}





//-------------------------------------------------------------------------------------------------
//	NAME		: WriteLog
//	DESC		: The function to create a error log for siege recall manager.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeTheme::WriteLog(char* pMsg)
{
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	FILE *fp = fopen("Log/Agent-SiegeRecallMgr.log", "a+") ;
	if (fp)
	{
		fprintf(fp, "%s [%s]\n", pMsg,  szTime) ;
		fclose(fp) ;
	}
}






