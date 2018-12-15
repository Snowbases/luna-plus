//-------------------------------------------------------------------------------------------------
//	NAME		: CSiege_RecallBase.cpp
//	DESC		: Implementation part of CSiege_RecallBase class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "./Siege_RecallBase.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CSiege_RecallBase
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
CSiege_RecallBase::CSiege_RecallBase(void)
{
	// Initialize variables.
	m_byBaseKind = 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CSiege_RecallBase
//	DESC		: The function destructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
CSiege_RecallBase::~CSiege_RecallBase(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_BaseKind
//	DESC		: The function to setting base kind.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiege_RecallBase::Set_BaseKind(BYTE byKind)
{
	// Check kind limit.
	if( byKind >= e_SIEGE_BASEKIND_MAX )
	{
		Throw_Error("Over kind limit!!", __FUNCTION__) ;
		return ;
	}


	// Setting base kind.
	m_byBaseKind = byKind ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ChildCount
//	DESC		: The function to return child controls count.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiege_RecallBase::Get_ChildCount()
{
	// Declare child count.
	BYTE byCount = 0 ;


	// Check start term.
	byCount = byCount + (BYTE)m_mSTerm.size() ;

	// Check end term.
	byCount = byCount + (BYTE)m_mETerm.size() ;

	// Check command.
	byCount = byCount + (BYTE)m_mCommand.size() ;


	// Return count.
	return byCount ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ChildKind
//	DESC		: The function to return child kind.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiege_RecallBase::Get_ChildKind(BYTE byIdx)
{
	// Declare kind.
	BYTE byKind = e_SIEGE_BASEKIND_EMPTY ;


	// Check start term.
	M_STERM::iterator it_sterm ;
	for( it_sterm = m_mSTerm.begin() ; it_sterm != m_mSTerm.end() ; ++it_sterm )
	{
		if( it_sterm == m_mSTerm.end() ) break ;

		if( it_sterm->second.Get_Index() != byIdx ) continue ;

		byKind = e_SIEGE_BASEKIND_START_TERM ;

		return byKind ;
	}


	// Check end term.
	M_ETERM::iterator it_eterm ;
	for( it_eterm = m_mETerm.begin() ; it_eterm != m_mETerm.end() ; ++it_eterm )
	{
		if( it_eterm == m_mETerm.end() ) break ;

		if( it_eterm->second.Get_Index() != byIdx ) continue ;

		byKind = e_SIEGE_BASEKIND_END_TERM ;

		return byKind ;
	}


	// Check command.
	M_COMMAND::iterator it_command ;
	for( it_command = m_mCommand.begin() ; it_command != m_mCommand.end() ; ++it_command )
	{
		if( it_command == m_mCommand.end() ) break ;

		if( it_command->second.Get_Index() != byIdx ) continue ;

		byKind = e_SIEGE_BASEKIND_COMMAND ;

		return byKind ;
	}


	// Return result.
	return byKind ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Insert_Sterm
//	DESC		: The function to insert start term.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiege_RecallBase::Insert_Sterm(BYTE byIdx, CSiegeTerm* pTerm)
{
	// Check parameter.
	if(!pTerm)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check index.
	if( byIdx >= MAX_BASE_INDEX )
	{
		Throw_Error("Over index limit!!", __FUNCTION__) ;
		return ;
	}


	// Insert command.
	m_mSTerm.insert( std::make_pair( byIdx, *pTerm ) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Insert_ETerm
//	DESC		: The function to insert end term.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiege_RecallBase::Insert_ETerm(BYTE byIdx, CSiegeTerm* pTerm) 
{
	// Check parameter.
	if(!pTerm)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check index.
	if( byIdx >= MAX_BASE_INDEX )
	{
		Throw_Error("Over index limit!!", __FUNCTION__) ;
		return ;
	}


	// Insert command.
	m_mETerm.insert( std::make_pair( byIdx, *pTerm ) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Insert_Command
//	DESC		: The function to insert command.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiege_RecallBase::Insert_Command(BYTE byIdx, CSiegeCommand* pCommand)
{
	// Check parameter.
	if(!pCommand)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check index.
	if( byIdx >= MAX_BASE_INDEX )
	{
		Throw_Error("Over index limit!!", __FUNCTION__) ;
		return ;
	}


	// Insert command.
	m_mCommand.insert( std::make_pair( byIdx, *pCommand ) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_StartTermInfo
//	DESC		: The function to return start term info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
CSiegeTerm* CSiege_RecallBase::Get_StartTermInfo(BYTE byIdx)
{
	// Check index limit.
	if( byIdx >= MAX_BASE_INDEX )
	{
		Throw_Error("Overd index limit!!", __FUNCTION__) ;
		return NULL ;
	}


	// Check index.
	M_STERM::iterator it ;
	for( it = m_mSTerm.begin() ; it != m_mSTerm.end() ; ++it )
	{
		if( it == m_mSTerm.end() ) return NULL ;

		if( it->second.Get_Index() != byIdx ) continue ;

		return &it->second ;
	}


	// Return null.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_EndTermInfo
//	DESC		: The function to return end term info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
CSiegeTerm* CSiege_RecallBase::Get_EndTermInfo(BYTE byIdx)
{
	// Check index limit.
	if( byIdx >= MAX_BASE_INDEX )
	{
		Throw_Error("Overd index limit!!", __FUNCTION__) ;
		return NULL ;
	}


	// Check index.
	M_ETERM::iterator it ;
	for( it = m_mETerm.begin() ; it != m_mETerm.end() ; ++it )
	{
		if( it == m_mETerm.end() ) return NULL ;

		if( it->second.Get_Index() != byIdx ) continue ;

		return &it->second ;
	}


	// Return null.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_CommandInfo
//	DESC		: The function to return commmand ifno.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
CSiegeCommand* CSiege_RecallBase::Get_CommandInfo(BYTE byIdx)
{
	// Check index limit.
	if( byIdx >= MAX_BASE_INDEX )
	{
		Throw_Error("Overd index limit!!", __FUNCTION__) ;
		return NULL ;
	}


	// Check index.
	M_COMMAND::iterator it ;
	for( it = m_mCommand.begin() ; it != m_mCommand.end() ; ++it )
	{
		if( it == m_mCommand.end() ) return NULL ;

		if( it->second.Get_Index() != byIdx ) continue ;

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
void CSiege_RecallBase::Throw_Error(char* szErr, char* szCaption)
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
void CSiege_RecallBase::WriteLog(char* pMsg)
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









