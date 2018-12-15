//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeCommand.cpp
//	DESC		: Implementation part of CSiegeCommand class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files
#include "./SiegeCommand.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeCommand
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
CSiegeCommand::CSiegeCommand(void)
{
	// Initialize variables.
	m_byIndex = 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CSiegeCommand
//	DESC		: The function destructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
CSiegeCommand::~CSiegeCommand(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_Index
//	DESC		: The function to setting index.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeCommand::Set_Index(BYTE byIdx)
{
	// Check index limit.
	if( byIdx >= MAX_COMMAND_IDX )
	{
		Throw_Error("Index is over limit!!", __FUNCTION__) ;
		return ;
	}


	// Setting index.
	m_byIndex = byIdx ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ChildCount
//	DESC		: The function to return child count.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiegeCommand::Get_ChildCount()
{
	// Declare count variable.
	BYTE byCount = 0 ;


	// Check die recall object.
	byCount = byCount + (BYTE)m_mRecallObj.size() ;


	// Return count.
	return byCount ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ChildKind
//	DESC		: The function to return child kind.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 01, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiegeCommand::Get_ChildKind(BYTE byIdx)
{
	// Declare kind.
	BYTE byKind = e_COMMAND_NONE ;


	// Check die object.
	M_RECALLOBJ::iterator it_die ;
	for( it_die = m_mRecallObj.begin() ; it_die != m_mRecallObj.end() ; ++it_die )
	{
		if( it_die->second.byIdx != byIdx ) continue ;

		byKind = e_COMMAND_RECALL_DIE_OBJ ;

		return byKind ;
	}


	// Return kind.
	return byKind ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_DieRecallObjInfo
//	DESC		: The function to return die recall object info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 01, 2008
//-------------------------------------------------------------------------------------------------
st_DIE_RECALL_OBJ* CSiegeCommand::Get_DieRecallObjInfo(BYTE byIdx)
{
	// Check die recall object.
	M_RECALLOBJ::iterator it_die ;
	for( it_die = m_mRecallObj.begin() ; it_die != m_mRecallObj.end() ; ++it_die )
	{
		if( it_die->second.byIdx != byIdx ) continue ;

		return &it_die->second ;
	}


	// Return null.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Die_Recall_Obj
//	DESC		: The function to add recall info when die.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeCommand::Add_Die_Recall_Obj(st_DIE_RECALL_OBJ* pInfo)
{
	// Check parameter.
	if(!pInfo)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Copy info.
	st_DIE_RECALL_OBJ recallObj ;
	memcpy(&recallObj, pInfo, sizeof(st_DIE_RECALL_OBJ)) ;


	// Insert recall info.
	m_mRecallObj.insert( std::make_pair( pInfo->byIdx, *pInfo ) ) ;
}




//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeCommand::Throw_Error(char* szErr, char* szCaption)
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
void CSiegeCommand::WriteLog(char* pMsg)
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










