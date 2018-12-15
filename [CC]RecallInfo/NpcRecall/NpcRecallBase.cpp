//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallBase.cpp
//	DESC		: Implementation part of CNpcRecallCondition class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "MHFile.h"

#include "./NpcRecallBase.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallBase
//	DESC		: 생성자 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallBase::CNpcRecallBase(void)
{
	// 멤버 변수 초기화.
	m_byRecallCodition = eNR_None ;

	m_dwNpcIdx			= 0 ;
	m_wNpcKind			= 0 ;

	m_RecallMapNum		= 0 ;

	m_fXpos				= 0.0f ;
	m_fZpos				= 0.0f ;

	m_byActive			= FALSE ;

	m_dwCreatedIdx		= 0 ;

	m_byReadyToDelete	= FALSE ;

	m_dwRecallTime		= 0 ;

	m_wRecallFailedMsg	= 0 ;

	m_wDir				= 0 ;

	m_dwNpcRecallIdx	= 0 ;

	m_wRecalledChenel	= 0 ;

	ZeroMemory( this, sizeof(char) * ( MAX_NAME_LENGTH+1 ) );

	m_dwParentNpcIdx	= 0 ;
	
	m_byParentType		= eParentNpcType_UnKnown ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CNpcRecallBase
//	DESC		: 소멸자 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallBase::~CNpcRecallBase(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_RecallCondition
//	DESC		: 소환 조건을 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Set_RecallCondition(BYTE byCondition)
{
	// 인자로 넘어온 상태 유효를 체크한다.
	if( byCondition == eNR_None || byCondition >= eNR_NpcRecall_Max )
	{
		Throw_Error("Invalid recall type!!", __FUNCTION__) ;
		return ;
	}


	// 소환 조건을 설정한다.
	m_byRecallCodition = byCondition ;
}


void CNpcRecallBase::Set_NpcRecallIndex(DWORD dwNpcRecallIdx)
{
	if( dwNpcRecallIdx == 0 )
	{
		Throw_Error("Invalid NpcRecall Index!!", __FUNCTION__) ;
		return ;
	}

	m_dwNpcRecallIdx = dwNpcRecallIdx ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: Set_NpcIndex
//	DESC		: Npc 인덱스를 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Set_NpcIndex(DWORD dwNpcIdx) 
{
	// 인자로 넘어오는 npc 인덱스 제한을 체크한다.
	if( dwNpcIdx >= 65000 )
	{
		Throw_Error("Invalid Npc index!!", __FUNCTION__) ;
		return ;
	}


	// Npc 인덱스를 설정한다.
	m_dwNpcIdx = dwNpcIdx ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_NpcKind
//	DESC		: Npc 종류를 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Set_NpcKind(WORD wNpcKind) 
{
	// 인자로 넘어오는 npc 인덱스 제한을 체크한다.
	if( wNpcKind >= 65000 || wNpcKind == 0 )
	{
		Throw_Error("Invalid Npc kind!!", __FUNCTION__) ;
		return ;
	}


	// Npc 인덱스를 설정한다.
	m_wNpcKind = wNpcKind ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_RecallMap
//	DESC		: Npc의 소환 맵 번호를 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Set_RecallMap(MAPTYPE mapNum) 
{
	// 인자로 넘어오는 맵 번호를 확인한다.
	if( mapNum == 0 )
	{
		Throw_Error("Invalid map number!!", __FUNCTION__) ;
		return ;
	}


	// 소환 할 맵 번호를 설정한다.
	m_RecallMapNum = mapNum ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_RecallPosX
//	DESC		: Npc의 소환 X좌표를 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Set_RecallPosX(float fXpos) 
{
	// 인자로 넙어오는 소환 좌표를 확인한다.
	if( fXpos <= 0.0f )
	{
		Throw_Error("Invalid recall position!!", __FUNCTION__) ;
		return ;
	}


	// 소환 할 좌표를 설정한다.
	m_fXpos = fXpos ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_RecallPosZ
//	DESC		: Npc의 소환 Z좌표를 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Set_RecallPosZ(float fZpos) 
{
	// 인자로 넙어오는 소환 좌표를 확인한다.
	if( fZpos <= 0.0f )
	{
		Throw_Error("Invalid recall position!!", __FUNCTION__) ;
		return ;
	}


	// 소환 할 좌표를 설정한다.
	m_fZpos = fZpos ;
}

void CNpcRecallBase::Get_RecallPos(VECTOR3* vOutPut)
{
	if( !vOutPut )
		return; 

	vOutPut->x = m_fXpos ;
	vOutPut->z = m_fZpos;
}

void CNpcRecallBase::Set_RecallDir(WORD wDir) 
{
	// 인자로 넙어오는 소환 좌표를 확인한다.
	if( wDir > 360 )
	{
		Throw_Error("Invalid recall Dir!!", __FUNCTION__) ;
		return ;
	}


	// 소환 할 좌표를 설정한다.
	m_wDir = wDir ;

}

void CNpcRecallBase::Set_NpcName(char* pNpcName) 
{
	SafeStrCpy(m_NpcName, pNpcName, MAX_NPC_NAME_LENGTH+1); 
}

void CNpcRecallBase::Set_RecalledChenel(WORD wChenel )
{
	if( wChenel == 0 )
	{
		Throw_Error("Invalid RecalledChenel Index!!", __FUNCTION__) ;
		return ;
	}

	m_wRecalledChenel = wChenel ;
}

void CNpcRecallBase::Set_ParentNpcIndex(DWORD dwIndex) 
{
	// 인자로 넙어오는 소환 좌표를 확인한다.
	if( dwIndex == 0 )
	{
		Throw_Error("Invalid ParentNpc Index!!", __FUNCTION__) ;
		return ;
	}

	m_dwParentNpcIdx = dwIndex ;
}

void CNpcRecallBase::Set_ParentNpcType(BYTE byType) 
{
	// 인자로 넙어오는 소환 좌표를 확인한다.
	if( byType == eParentNpcType_UnKnown || byType >= eParentNpcType_Max )
	{
		Throw_Error("Invalid ParentNpc Type!!", __FUNCTION__) ;
		return ;
	}

	m_byParentType = byType ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallBase::Throw_Error(char* szErr, char* szCaption)
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
void CNpcRecallBase::WriteLog(char* pMsg)
{
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	FILE *fp = NULL ;
#ifdef _AGENTSERVER
	fp = fopen("Log/AgentServer-SiegeRecallMgr.log", "a+") ;
#elif _MAPSERVER_
	fp = fopen("Log/MapServer-SiegeRecallMgr.log", "a+") ;
#endif // _AGENTSERVER_

	if (fp)
	{
		fprintf(fp, "%s [%s]\n", pMsg,  szTime) ; 
		fclose(fp) ;
	}
}




























