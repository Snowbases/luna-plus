#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeTheme.h
//	DESC		: The class to contain a theme info for siege war.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include <map>

#include "./SiegeMap.h"





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define MAX_SIEGETHEME_ID	250
#define MAX_THEME_IDX		250										// Use this to check limit theme index. current theme index is 250





//-------------------------------------------------------------------------------------------------
//		The class CSiegeTheme.
//-------------------------------------------------------------------------------------------------
class CSiegeTheme
{
	BYTE m_byIdx ;
	// The Index variable for Theme.
	MAPTYPE m_wThemeIdx ;

	// The Map container to contain a map info.
	typedef std::map< MAPTYPE, CSiegeMap > M_MAP ;
	M_MAP	m_mMap ;

public:
	CSiegeTheme();
	virtual ~CSiegeTheme();
	void Set_Idx(BYTE byIdx) ;									// The function to setting index.
	BYTE Get_Idx() { return m_byIdx ; }							// The function to return index.

	void Set_ThemeIdx(MAPTYPE wIdx) ;							// The function to setting theme index.
	MAPTYPE Get_ThemeIdx() { return m_wThemeIdx ; }				// The function to return thtme index.

	void Add_SiegeMap(CSiegeMap* pMap) ;						// The function to add siege map.
	CSiegeMap* Get_SiegeMapByMapNum( MAPTYPE mapNum ) ;			// The function to return map info by map num.
	CSiegeMap* Get_SiegeMapByIdx( BYTE byIdx ) ;				// The function to return map info by index.
	BYTE Get_MapCount() { return (BYTE)m_mMap.size() ; }		// The function to return map count.


	////////////////////////////////
	// [ 에러 메시지 처리 파트. ] //
	////////////////////////////////

	void Throw_Error(char* szErr, char* szCaption) ;							// 에러 메시지 처리를 하는 함수.
	void WriteLog(char* pMsg) ;													// 로그를 남기는 처리를 하는 함수.
};
