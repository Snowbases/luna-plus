//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeTerm.cpp
//	DESC		: Implementation part of CSiegeTerm class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "./SiegeTerm.h"

#include "./Siege_AddObj.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeTerm
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
CSiegeTerm::CSiegeTerm(void)
{
	// Initialize variables.
	m_byIndex = 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CSiegeTerm
//	DESC		: The function destructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
CSiegeTerm::~CSiegeTerm(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_ParentIdx
//	DESC		: The function to setting parent index.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeTerm::Set_ParentIdx(BYTE byIndex)
{
	// Check index limit.
	if( byIndex == 0 || byIndex >= MAX_ADDOBJ_IDX )
	{
		Throw_Error("Index is over limit!!", __FUNCTION__) ;
		return ;
	}


	// Setting parent index.
	m_byParentIdx = byIndex ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_Index
//	DESC		: The function to setting index.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeTerm::Set_Index(BYTE byIndex)
{
	// Check index limit.
	if( byIndex >= MAX_TERM_INDEX )
	{
		Throw_Error("Index is over limit!!", __FUNCTION__) ;
		return ;
	}


	// Setting index.
	m_byIndex = byIndex ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ChildCount
//	DESC		: The function to return child count.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiegeTerm::Get_ChildCount() 
{
	// Declare child count.
	BYTE byChildCount = 0 ;


	// Check object die.
	byChildCount = byChildCount + (BYTE)m_mCheckObjDie.size() ;

	// Check use item.
	byChildCount = byChildCount + (BYTE)m_mCheckUseItem.size() ;


	// Return result.
	return byChildCount ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ChildKind
//	DESC		: The function to return child kind.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiegeTerm::Get_ChildKind(BYTE byIdx)
{
	// Declare kind variable.
	BYTE byKind = e_TERM_NONE ;


	// Check object die.
	M_CHKOBJDIE::iterator it_die ;
	for( it_die = m_mCheckObjDie.begin() ; it_die != m_mCheckObjDie.end() ; ++it_die )
	{
		if( it_die->second.byIdx != byIdx ) continue ;

		byKind = e_TERM_DIE_OBJ ;

		return byKind ;
	}


	// Check use item.
	M_CHKUSEITEM::iterator it_item ;
	for( it_item = m_mCheckUseItem.begin() ; it_item != m_mCheckUseItem.end() ; ++it_item )
	{
		if( it_item->second.byIdx != byIdx ) continue ;

		byKind = e_TERM_USE_ITEM ;

		return byKind ;
	}


	// Return kind.
	return byKind ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Insert_CheckObjDie
//	DESC		: The function to insert check object die.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeTerm::Insert_CheckObjDie(st_CHECK_OBJ_DIE* pInfo)
{
	// Check parameter.
	if(!pInfo)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Inasert term.
	m_mCheckObjDie.insert( std::make_pair( pInfo->byIdx, *pInfo ) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_CheckObjDie
//	DESC		: The function to remove check object die.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeTerm::Remove_CheckObjDie(st_CHECK_OBJ_DIE* pInfo)
{
	// Check parameter.
	if(!pInfo)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check index.
	M_CHKOBJDIE::iterator it ;

	for( it = m_mCheckObjDie.begin() ; it != m_mCheckObjDie.end() ; ++it )
	{
		if( it->second.byObjectIdx != pInfo->byObjectIdx ) continue ;

		m_mCheckObjDie.erase( it ) ;

		return ;
	}


	// Print error message.
	Throw_Error("Failed to erase info!!", __FUNCTION__) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Decrease_UseItemCount
//	DESC		: The function to decrease use item count.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeTerm::Decrease_UseItemCount(st_CHECK_USE_ITEM* pInfo) 
{
	// Check parameter.
	if(!pInfo)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check item count.
	if( pInfo->wUseCount == 0 )
	{
		M_CHKUSEITEM::iterator it ;

		it = m_mCheckUseItem.find(pInfo->byIdx) ;
		
		// Erase iterator.
		if( it != m_mCheckUseItem.end() )
		{
			m_mCheckUseItem.erase(it) ;
		}
	}
	else
	{
		// Decrease item count.
		-- pInfo->wUseCount ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Insert_CheckUseItem
//	DESC		: The function to insert chec use item.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeTerm::Insert_CheckUseItem(st_CHECK_USE_ITEM* pInfo) 
{
	// Check parameter.
	if(!pInfo)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Inasert term.
	m_mCheckUseItem.insert( std::make_pair( pInfo->byIdx, *pInfo ) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ChkObjDieInfo
//	DESC		: The function to return check object die info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
st_CHECK_OBJ_DIE* CSiegeTerm::Get_ChkObjDieInfo(BYTE byIdx)
{
	// Check index.
	M_CHKOBJDIE::iterator it ;
	for( it = m_mCheckObjDie.begin() ; it != m_mCheckObjDie.end() ; ++it )
	{
		if(it->second.byIdx != byIdx) continue ;

		return &it->second ;
	}


	// Return null.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ChkUseItemInfo
//	DESC		: The function to return check use item info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
st_CHECK_USE_ITEM* CSiegeTerm::Get_ChkUseItemInfo(BYTE byIdx)
{
	// Check index.
	M_CHKUSEITEM::iterator it ;
	for( it = m_mCheckUseItem.begin() ; it != m_mCheckUseItem.end() ; ++it )
	{
		if(it->second.byIdx != byIdx) continue ;

		return &it->second ;
	}


	// Return null.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeTerm::Throw_Error(char* szErr, char* szCaption)
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
void CSiegeTerm::WriteLog(char* pMsg)
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












