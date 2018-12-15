//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeMap.cpp
//	DESC		: Implementation part of CSiegeMap class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "./SiegeMap.h"

//#include "../[CC]ServerModule/Network.h"

#include "./Siege_AddObj.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeMap
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------
CSiegeMap::CSiegeMap(void)
{
	// Initialize variables.
	m_wThemeIdx = 0 ;

	m_byIdx		= 0 ;
	
	m_byMapNum	= 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CSiegeMap
//	DESC		: The function destructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------
CSiegeMap::~CSiegeMap(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_Idx
//	DESC		: The function to seting index of this class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeMap::Set_Idx(BYTE byIdx) 
{
	// Check index limit.
	if( byIdx >= 255 )
	{
		Throw_Error("Failed to setting index, Over limit!!", __FUNCTION__) ;
		return ;
	}


	// Setting index.
	m_byIdx = byIdx ; 
}





////-------------------------------------------------------------------------------------------------
////	NAME		: Set_CurStepIdx
////	DESC		: The function to setting current step index.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 28, 2008
////-------------------------------------------------------------------------------------------------
//void CSiegeMap::Set_CurStepIdx(BYTE byStep)
//{
//	// Check index limit.
//	if( byStep >= 255 )
//	{
//		Throw_Error("Failed to setting current step index, Over limit!!", __FUNCTION__) ;
//		return ;
//	}
//
//
//	// Setting current step.
//	m_byCurStep = byStep ; 
//}
//
//
//
//
//
//-------------------------------------------------------------------------------------------------
//	NAME		: Set_MapNum
//	DESC		: The function to setting map number.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeMap::Set_MapNum(MAPTYPE mapNum)
{
	// Check map number limit.
	if( mapNum == 0 || mapNum >= MAX_MAP_NUMBER )
	{
		Throw_Error("Failed to setting map nubmer, Map number limit is over!!", __FUNCTION__) ;
		return ;
	}

	m_byMapNum = mapNum ; 
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Step
//	DESC		: The function to add step info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 29, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeMap::Add_Step(BYTE byIndex, CSiegeStep* pStepInfo)
{
	// Check parameter.
	if(!pStepInfo)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Insert step info.
	pStepInfo->Set_ThemeIdx(m_wThemeIdx) ;

	m_mStep.insert( std::make_pair( byIndex, *pStepInfo ) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_StepInfo
//	DESC		: The function to return step info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
CSiegeStep* CSiegeMap::Get_StepInfo(BYTE byIndex)
{
	// Check index.
	M_STEP::iterator it ;
	for( it = m_mStep.begin() ; it != m_mStep.end() ; ++it )
	{
		if( it == m_mStep.end() ) break ;

		if( it->second.Get_StepIndex() != byIndex ) continue ;

		return &it->second ;
	}


	// Return null.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Execute_Step
//	DESC		: The function to execute step.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeMap::Execute_Step(MAPTYPE mapNum, BYTE byStepIdx)
{
	// Check map number.
	BYTE byCount = 0 ;
	BYTE byKind = e_STEP_ADD_OBJ ;

	M_STEP::iterator it ;

	for( it = m_mStep.begin() ; it != m_mStep.end() ; ++it )
	{
		byKind = it->second.Get_ChildKind(byCount) ;

		switch(byKind)
		{
		case e_STEP_ADD_OBJ :
			{

			}
			break ;
		}
	}
}





////-------------------------------------------------------------------------------------------------
////	NAME		: Get_AllStepCount
////	DESC		: The function to return all step count.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 28, 2008
////-------------------------------------------------------------------------------------------------
//BYTE CSiegeMap::Get_AllStepCount()
//{
//	// Declare temp variables.
//	BYTE byCount = 0 ;
//
//
//    // Check Add object part.
//	M_ADDOBJ::iterator it_addobj ;
//	for( it_addobj = m_mAddObj.begin() ; it_addobj != m_mAddObj.end() ; ++it_addobj )
//	{
//		if(it_addobj == m_mAddObj.end()) break ;
//
//		++byCount ;
//	}
//
//
//	// Return count.
//	return byCount ;
//}
//
//
//
//
//
////-------------------------------------------------------------------------------------------------
////	NAME		: Get_SiegeStepByIdx
////	DESC		: The function to return siege step info.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 25, 2008
////-------------------------------------------------------------------------------------------------
//CSiegeStep* CSiegeMap::Get_SiegeStepByIdx( BYTE byIdx )
//{
//	// Check index of step limit.
//	if( byIdx == 0 || byIdx > MAX_STEP_INDEX )
//	{
//		Throw_Error("Failed to return step info, index limit is over!!", __FUNCTION__) ;
//		return NULL ;
//	}
//
//	// Check Add object part.
//	M_ADDOBJ::iterator it_addobj ;
//	for( it_addobj = m_mAddObj.begin() ; it_addobj != m_mAddObj.end() ; ++it_addobj )
//	{
//		if(it_addobj == m_mAddObj.end()) break ;
//
//		if(it_addobj->second.Get_StepIndex() != byIdx) continue ;
//
//		return (CSiegeStep*)&it_addobj->second ;
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
////	NAME		: Get_AddObjInfo
////	DESC		: The function to return add object info.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 25, 2008
////-------------------------------------------------------------------------------------------------
//CSiege_AddObj* CSiegeMap::Get_AddObjInfo( BYTE byIdx ) 
//{
//	// Check index of step limit.
//	if( byIdx == 0 || byIdx > MAX_STEP_INDEX )
//	{
//		Throw_Error("Failed to return add obj info, index limit is over!!", __FUNCTION__) ;
//		return NULL ;
//	}
//
//	// Check Add object part.
//	M_ADDOBJ::iterator it_addobj ;
//	for( it_addobj = m_mAddObj.begin() ; it_addobj != m_mAddObj.end() ; ++it_addobj )
//	{
//		if(it_addobj == m_mAddObj.end()) break ;
//
//		if(it_addobj->second.Get_StepIndex() != byIdx) continue ;
//
//		return &it_addobj->second ;
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
////	NAME		: Insert_AddObj
////	DESC		: The function to insert add obj info.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 25, 2008
////-------------------------------------------------------------------------------------------------
//void CSiegeMap::Insert_AddObj(CSiege_AddObj* pAddObj)
//{
//	// Check parameter.
//	if(!pAddObj)
//	{
//		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
//		return ;
//	}
//
//
//	// Insert addobj.
//	m_mAddObj.insert( std::make_pair( pAddObj->Get_StepKind(), *pAddObj ) ) ;
//}
//
//
//
//
//
////-------------------------------------------------------------------------------------------------
////	NAME		: Increase_Step
////	DESC		: The function to increase step.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 28, 2008
////-------------------------------------------------------------------------------------------------
//void CSiegeMap::Increase_Step(DWORD dwIndex, DWORD dwObjID)
//{
//	// Check current step.
//	if( m_byCurStep >= MAX_STEP_INDEX )
//	{
//		m_byCurStep = 1 ;
//	}
//	else ++m_byCurStep ;
//
//
//	// Execute step.
//	Execute_Step( dwIndex, dwObjID, m_byCurStep ) ;
//}
//
//
//
//
//
////-------------------------------------------------------------------------------------------------
////	NAME		: Decrease_Step
////	DESC		: The function to decrease step.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 28, 2008
////-------------------------------------------------------------------------------------------------
//void CSiegeMap::Decrease_Step(DWORD dwIndex, DWORD dwObjID)
//{
//	// Check current step.
//	if( m_byCurStep <= 1 )
//	{
//		m_byCurStep = MAX_STEP_INDEX ;
//	}
//	else --m_byCurStep ;
//
//
//	// Execute step.
//	Execute_Step( dwIndex, dwObjID, m_byCurStep ) ;
//}
//
//
//
//
//
////-------------------------------------------------------------------------------------------------
////	NAME		: Execute_Step
////	DESC		: The function to execute step.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 28, 2008
////-------------------------------------------------------------------------------------------------
//void CSiegeMap::Execute_Step(DWORD dwIndex, DWORD dwObjID, BYTE byStep)
//{
//	// Check add object part.
//	BYTE byGroupIdx ;
//	st_SIEGEOBJ* pObjInfo ;
//	MSG_SIEGERECALL_OBJINFO msg_addobj ;
//
//	M_ADDOBJ::iterator it_addobj ;
//	for( it_addobj = m_mAddObj.begin() ; it_addobj != m_mAddObj.end() ; ++it_addobj )
//	{
//		// Check break timing.
//		if(it_addobj == m_mAddObj.end()) break ;
//
//		// Check group index.
//		byGroupIdx = 0 ;
//		byGroupIdx = it_addobj->second.Get_GroupIndex() ;
//		if(byGroupIdx != byStep) continue ;
//
//		// Receive object info.
//		pObjInfo = NULL ;
//		pObjInfo = it_addobj->second.Get_ObjInfo() ;
//
//		if(!pObjInfo) continue ;
//
//		// Send object info
//		msg_addobj.Category		= MP_SIEGERECALL ;
//		msg_addobj.Protocol		= MP_SIEGERECALL_ADDOBJ_SYN ;
//
//		msg_addobj.dwObjectID	= dwObjID ;
//
//		memcpy(&msg_addobj.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;
//
//		g_Network.Send2Server( dwIndex, (char*)&msg_addobj, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
//	}
//}











//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeMap::Throw_Error(char* szErr, char* szCaption)
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
void CSiegeMap::WriteLog(char* pMsg)
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














