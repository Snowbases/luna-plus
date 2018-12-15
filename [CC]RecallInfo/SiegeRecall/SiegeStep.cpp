//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeStep.cpp
//	DESC		: Implementation part of CSiegeStep clas.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "./SiegeStep.h"

#include "./Siege_AddObj.h"

//#include "./SiegeCommand.h"
//#include "./SiegeTerm.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeStep
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------
CSiegeStep::CSiegeStep(void)
{
	// Initialze variables.
	m_ParentMapNum = 0 ;

	m_byStepIdx = 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CSiegeStep
//	DESC		: The function destructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------
CSiegeStep::~CSiegeStep(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_ParentMapNum
//	DESC		: The function to setting parent map number of this step.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 03, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeStep::Set_ParentMapNum(MAPTYPE mapNum)
{
	// Check map num.
	if( mapNum == 0 )
	{
		Throw_Error("Invalid map num!!", __FUNCTION__) ;
		return ;
	}


	// Setting parent map number.
	m_ParentMapNum = mapNum ;
}





////-------------------------------------------------------------------------------------------------
////	NAME		: Set_GroupIndex
////	DESC		: The function to setting group index of this step.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 27, 2008
////-------------------------------------------------------------------------------------------------
//void CSiegeStep::Set_GroupIndex(BYTE byGroupIdx)
//{
//	// Check index limit.
//	if( byGroupIdx == 0 || byGroupIdx >= MAX_GROUP_IDX )
//	{
//		Throw_Error("Invalid index!!", __FUNCTION__) ;
//		return ;
//	}
//
//
//	// Setting group index.
//	m_byGroupIdx = byGroupIdx ;
//}
//
//
//
//
//
//-------------------------------------------------------------------------------------------------
//	NAME		: Set_StepIndex
//	DESC		: The function to setting index of this step.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeStep::Set_StepIndex(BYTE byIdx)
{
	// Check index limit.
	if( byIdx >= MAX_STEP_IDX )
	{
		Throw_Error("Invalid index!!", __FUNCTION__) ;
		return ;
	}


	// Setting index.
	m_byStepIdx = byIdx ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ChildCount
//	DESC		: The function to return child controls count.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiegeStep::Get_ChildCount()
{
	// Declare child count.
	BYTE byChildCount = 0 ;


	// Check add object.
	M_MADDOBJ::iterator it_addobj ;
	for( it_addobj = m_mAddObj.begin() ; it_addobj != m_mAddObj.end() ; ++it_addobj )
	{
		if( it_addobj == m_mAddObj.end() ) break ;

		++byChildCount ; 
	}


	// Return count.
	return byChildCount ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ChildKind
//	DESC		: The function to return child kind.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiegeStep::Get_ChildKind(BYTE byIdx)
{
	// Declare result.
	BYTE byChildkind = e_STEP_MAX ;


	// Check add object.
	M_MADDOBJ::iterator it_addobj ;
	for( it_addobj = m_mAddObj.begin() ; it_addobj != m_mAddObj.end() ; ++it_addobj )
	{
		if( it_addobj == m_mAddObj.end() ) break ;

		if( it_addobj->second.Get_Index() != byIdx ) continue ;

		return it_addobj->second.Get_BaseKind() ;
	}


	// Return result.
	return byChildkind ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Insert_AddObj
//	DESC		: The function to insert add object info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeStep::Insert_AddObj(BYTE byIndex, CSiege_AddObj* pAddObj)
{
	// Check parameter.
	if(!pAddObj)
	{
		Throw_Error("Invalid index!!", __FUNCTION__) ;
		return ;
	}


	// Insert add object info.
	m_mAddObj.insert( std::make_pair( byIndex, *pAddObj ) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_AddObjInfo
//	DESC		: The function to return add object info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
CSiege_AddObj* CSiegeStep::Get_AddObjInfo(BYTE byIndex)
{
	// Check index.
	M_MADDOBJ::iterator it ;
	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		if( it == m_mAddObj.end() ) break ;

		if( it->second.Get_Index() != byIndex ) continue ;

		return &it->second ;
	}


	// Return null.
	return NULL ;
}





////-------------------------------------------------------------------------------------------------
////	NAME		: Set_StepKind
////	DESC		: The function to setting step kind of this step.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 25, 2008
////-------------------------------------------------------------------------------------------------
//void CSiegeStep::Set_StepKind(BYTE byStepKind)
//{
//	// Check step kind limit.
//	if( byStepKind >= e_STEP_MAX )
//	{
//		Throw_Error("Invalid index!!", __FUNCTION__) ;
//		return ;
//	}
//
//
//	// Setting step kind.
//	m_byStepKind = byStepKind ;
//}
//
//
//
//
//
////-------------------------------------------------------------------------------------------------
////	NAME		: Get_Command
////	DESC		: The function to return command info.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 25, 2008
////-------------------------------------------------------------------------------------------------
//CSiegeCommand* CSiegeStep::Get_Command( BYTE byIdx )
//{
//	// Check index limit.
//	if( byIdx == 0 || byIdx >= MAX_COMMAND_COUNT )
//	{
//		Throw_Error("Index limit over!!", __FUNCTION__) ;
//		return NULL ;
//	}
//
//
//	// Search command.
//	M_COMMAND::iterator it ;
//	for( it = m_mCommand.begin() ; it != m_mCommand.end() ; ++it )
//	{
//		if(it == m_mCommand.end()) break ;
//
//		//if(it->second.m_mAddObj
//	}
//
//
//	// Return null.
//	return NULL ;
//}
//
//
//
//
//
////-------------------------------------------------------------------------------------------------
////	NAME		: Insert_Command
////	DESC		: The function to insert command to this step.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 25, 2008
////-------------------------------------------------------------------------------------------------
//void CSiegeStep::Insert_Command(BYTE byIdx, CSiegeCommand* pCommand)
//{
//	// Check parameter.
//	if(!pCommand)
//	{
//		Throw_Error("Index limit over!!", __FUNCTION__) ;
//		return ;
//	}
//
//
//	// Insert a command to this step.
//	m_mCommand.insert( std::make_pair( byIdx, *pCommand ) ) ;
//}





//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeStep::Throw_Error(char* szErr, char* szCaption)
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
void CSiegeStep::WriteLog(char* pMsg)
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







