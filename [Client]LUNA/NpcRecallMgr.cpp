#include "stdafx.h"
#include "MHFile.h"
#include "NpcRecallMgr.h"
#include "ChatManager.h"
#include "WindowIDEnum.h"
#include "cMsgBox.h"
#include "interface/cWindowManager.h"
#include "../[CC]Skill/Client/Info/BuffSkillInfo.h"
#include "GameIn.h"
#include "ObjectManager.h"
#include "DealDialog.h"
#include "NpcScriptDialog.h"
#include "QuestManager.h"

CNpcRecallMgr::CNpcRecallMgr(void)
{
	m_CurSelectedNpcID = 0 ;

	// 081023 LYW --- NpcRecallMgr : 메모리 문제로 인해, new delete를 따로 안하는 구조로 수정한다.
	//// 080826 KTH -- Initalize NPC Buff HashMap
	//m_NPCBuff.Initialize(20);
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CNpcRecallMgr
//	DESC		: 소멸자 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMgr::~CNpcRecallMgr(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Initialize
//	DESC		: 초기화 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Initialize() 
{
	// NPC 소환 정보를 가지고 있는 스크립트 정보를 로드한다.
	Load_RecallNpc() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_RecallNpc
//	DESC		: 소환용 npc 정보를 로드하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_RecallNpc()
{
	// 파일 클래스 선언.
	CMHFile file ;

	if( !file.Init("system/resource/NpcRecallInfo.bin", "rb") )
	{
		PrintError( "Failed to initialize SiegeRecall.bin", __FUNCTION__ ) ;
		return ;
	}

	char szBuff[257] = {0 };
	BYTE byCommandKind = e_RNC_MAX;

	while( TRUE )
	{
		// 파일의 끝인지 체크한다.
		if( file.IsEOF() ) break ;

		// 명령어를 받는다.
		memset( szBuff, 0, strlen(szBuff) ) ;
		file.GetString(szBuff) ;

		// 주석 명령어 라면, continue 처리를 한다..
		if(strcmp( szBuff, "//" ) == 0)
		{
			memset( szBuff, 0, strlen(szBuff) ) ;
			file.GetLine(szBuff, sizeof(256)) ;
			continue ;
		}
		else if( (strcmp( szBuff, "{" ) == 0) || (strcmp( szBuff, "}" ) == 0 ) ) continue ;

		// 스크립트 명령어 타입을 받는다.
		byCommandKind = Get_CommandKind( szBuff ) ;

		// 명령어 타입에 따른 처리를 한다.
		Command_Process(byCommandKind, &file) ;
	}


	// 파일 클래스를 해제한다.
	file.Release() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_CommandKind
//	DESC		: 스크립트 명령어 타입을 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
BYTE CNpcRecallMgr::Get_CommandKind(char* pString)
{
	BYTE byCommandKind = e_RNC_MAX ;

	// 함수 인자 확인.
	if(!pString)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return byCommandKind ;
	}


	// 스크립트 명령어의 스트링 사이즈 체크.
	if( strlen(pString) <= 1 )
	{
		PrintError("Invalid string size!!", __FUNCTION__) ;
		return byCommandKind ;
	}


	// 스크립트 명령어 타입을 설정한다.
	if( strcmp( pString, "$SELF_RECALL_NPC" ) == 0 )
	{
		byCommandKind = e_RNC_SELF_RECALL_NPC ;
	}

	if( strcmp( pString, "$REMAIN_TIME_NPC" ) == 0 )
	{
		byCommandKind = e_RNC_REMAIN_TIME_NPC ;
	}

	if( strcmp( pString, "$MOVEABLE_NPC" ) == 0 )
	{
		byCommandKind = e_RNC_MOVEABLE_NPC ;
	}

	if( strcmp( pString, "#NPCRECALLID" ) == 0 )
	{
		byCommandKind = e_RNC_NPCRECALLID ;
	}

	if( strcmp( pString, "#NPCID" ) == 0 )
	{
		byCommandKind = e_RNC_NPCID ;
	}

	if( strcmp( pString, "#NPCKIND" ) == 0 )
	{
		byCommandKind = e_RNC_NPCKIND ;
	}

	if( strcmp( pString, "#RECALLMAP" ) == 0 )
	{
		byCommandKind = e_RNC_RECALLMAP ;
	}

	if( strcmp( pString, "#XPOS" ) == 0 )
	{
		byCommandKind = e_RNC_XPOS ;
	}

	if( strcmp( pString, "#ZPOS" ) == 0 )
	{
		byCommandKind = e_RNC_ZPOS ;
	}

	if( strcmp( pString, "#DIR" ) == 0 )
	{
		byCommandKind = e_RNC_DIR ;
	}

	if( strcmp( pString, "#NAME" ) == 0 )
	{
		byCommandKind = e_RNC_NAME ;
	}


	if( strcmp( pString, "#TIMETABLE" ) == 0 )
	{
		byCommandKind = e_RNC_TIME_TABLE ;
	}

	if( strcmp( pString, "#REMAINTIME" ) == 0 )
	{
		byCommandKind = e_RNC_REMAIN_TIME ;
	}

	if( strcmp( pString, "#MOVETIME" ) == 0 )
	{
		byCommandKind = e_RNC_MOVE_TIME ;
	}

	if( strcmp( pString, "#MOVEXPOS" ) == 0 )
	{
		byCommandKind = e_RNC_MOVE_XPOS ;
	}

	if( strcmp( pString, "#MOVEZPOS" ) == 0 )
	{
		byCommandKind = e_RNC_MOVE_ZPOS ;
	}

	if( strcmp( pString, "#CHANGEMAPNUM" ) == 0 )
	{
		byCommandKind = e_RNC_CHANGE_MAPNUM ;
	}

	if( strcmp( pString, "#CHANGEMAPXPOS" ) == 0 )
	{
		byCommandKind = e_RNC_CHANGE_XPOS ;
	}

	if( strcmp( pString, "#CHANGEMAPZPOS" ) == 0 )
	{
		byCommandKind = e_RNC_CHANGE_ZPOS ;
	}

	if( strcmp( pString, "#RECALL_FAILED_MSG" ) == 0 )
	{
		byCommandKind = e_RNC_RECALL_FAILED_MSG ;
	}

	if( strcmp( pString, "#PARENT_NPCID" ) == 0 )
	{
		byCommandKind = e_RNC_PARENT_NPCID ; 
	}

	if( strcmp( pString, "#PARENT_NPCTYPE" ) == 0 )
	{
		byCommandKind = e_RNC_PARENT_TYPE ;
	}


	// 스크립트 명령어 타입을 반환한다.
	return byCommandKind ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Command_Process
//	DESC		: 스크립트 명령어 타입에 따른 처리를 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Command_Process(BYTE byCommandKind, CMHFile* pFile)
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	switch(byCommandKind)
	{
	case e_RNC_SELF_RECALL_NPC :	Load_SelfRecallNpc() ;				break ;
	case e_RNC_REMAIN_TIME_NPC :	Load_RemainTimeNpc() ;				break ;
	case e_RNC_MOVEABLE_NPC :		Load_MoveableNpc() ;				break ;
	case e_RNC_NPCRECALLID :		Load_SetNpcRecallId( pFile ) ;		break ;
	case e_RNC_NPCID :				Load_SetNpcId( pFile ) ;			break ;
	case e_RNC_NPCKIND :			Load_SetNpcKind( pFile ) ;			break ;
	case e_RNC_RECALLMAP :			Load_SetRecallMap( pFile ) ;		break ;
	case e_RNC_XPOS :				Load_SetXpos( pFile ) ;				break ;
	case e_RNC_ZPOS :				Load_SetZpos( pFile ) ;				break ;
	case e_RNC_DIR:					Load_SetDir( pFile ) ;				break ;
	case e_RNC_NAME:				Load_SetName( pFile ) ;				break ;
	case e_RNC_TIME_TABLE :			Load_TimeTable( pFile ) ;			break ;
	case e_RNC_REMAIN_TIME :		Load_SetRemainTime( pFile ) ;		break ;
	case e_RNC_MOVE_TIME :			Load_SetMoveTime( pFile ) ;			break ;
	case e_RNC_MOVE_XPOS :			Load_SetMoveXpos( pFile ) ;			break ;
	case e_RNC_MOVE_ZPOS :			Load_SetMoveZpos( pFile ) ;			break ;
	case e_RNC_CHANGE_MAPNUM :		Load_SetChangeMapNum( pFile ) ;		break ;
	case e_RNC_CHANGE_XPOS :		Load_SetChangeMapXPos( pFile ) ;	break ;
	case e_RNC_CHANGE_ZPOS :		Load_SetChangeMapZPos( pFile ) ;	break ;
	case e_RNC_RECALL_FAILED_MSG :	Load_SetRecallFailedMsg( pFile ) ;	break ;
	case e_RNC_PARENT_NPCID:		Load_SetParentNpcID( pFile ) ; 		break ;	
	case e_RNC_PARENT_TYPE:			Load_SetParentNpcType( pFile ) ; 	break ;	

	case e_RNC_MAX :
	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SelfRecallNpc
//	DESC		: 스스로 소환/소멸되는 npc 정보를 로드하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SelfRecallNpc() 
{
	// 스스로 소환/소멸할 수 있는 npc를 추가한다.
	CNpcRecallSelf newNpc ;

	newNpc.Set_NpcIndex(Get_CurTotalNpcCount()) ;
	newNpc.Set_RecallCondition(eNR_ItSelf) ;

	m_mRSelf_Npc.insert( std::make_pair(Get_CurTotalNpcCount(), newNpc) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_RemainTimeNpc
//	DESC		: 일정 시간 소환/소멸되는 npc 정보를 로드하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_RemainTimeNpc()
{
	// 일정 시간 소환/소멸되는 npc를 추가한다.
	CNpcRecallRemainTime newNpc ;

	newNpc.Set_NpcIndex(Get_CurTotalNpcCount()) ;
	newNpc.Set_RecallCondition(eNR_RemainTime) ;

	m_mRTime_Npc.insert( std::make_pair(Get_CurTotalNpcCount(), newNpc) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_MoveableNpc
//	DESC		: 이동 가능한 npc 정보를 로드하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_MoveableNpc() 
{
	// 이동 가능한 npc를 추가한다.
	CNpcRecallMoveable newNpc ;

	newNpc.Set_NpcIndex(Get_CurTotalNpcCount()) ;
	newNpc.Set_RecallCondition(eNR_Moveable) ;

	m_mRMove_Npc.insert( std::make_pair(Get_CurTotalNpcCount(), newNpc) ) ;
}





void CNpcRecallMgr::Load_SetNpcRecallId(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	// Npc인덱스를 설정한다.
	pBase->Set_NpcRecallIndex( pFile->GetDword() ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetNpcId
//	DESC		: 현재 로딩중인 npc의 인덱스를 세팅하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetNpcId(CMHFile* pFile)
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	pBase->Set_NpcIndex( pFile->GetWord() ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetNpcKind
//	DESC		: 현재 로딩중인 npc의 종류를 세팅하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetNpcKind(CMHFile* pFile)
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	// Npc인덱스를 설정한다.
	pBase->Set_NpcKind( pFile->GetWord() ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetRecallMap
//	DESC		: 소환 될 맵 번호를 세팅하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetRecallMap(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	// 소환 될 맵 인덱스를 설정한다.
	pBase->Set_RecallMap( pFile->GetWord() ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetXpos
//	DESC		: 소환 될 X좌표를 세팅하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetXpos(CMHFile* pFile)
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	// 소환될 X좌표를 세팅한다.
	pBase->Set_RecallPosX( pFile->GetFloat() ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetZpos
//	DESC		: 소환 될 Z좌표를 세팅하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetZpos(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	// 소환될 Z좌표를 세팅한다.
	pBase->Set_RecallPosZ( pFile->GetFloat() ) ;
}


void CNpcRecallMgr::Load_SetDir(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	// 소환될 Z좌표를 세팅한다.
	pBase->Set_RecallDir( pFile->GetWord() ) ;
}

void CNpcRecallMgr::Load_SetName(CMHFile* pFile ) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	pBase->Set_NpcName( pFile->GetString() ) ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: Load_TimeTable
//	DESC		: 스스로 소환될 시간 테이블을 로딩하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_TimeTable(CMHFile* pFile)
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_ItSelf )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// 스스로 소환/소멸하는 현재 npc 정보를 받는다.
	MAP_RSELF_NPC::iterator it ;
	for( it = m_mRSelf_Npc.begin() ; it != m_mRSelf_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

		else break ;
	}


	// Npc 정보를 확인한다.
	if( it == m_mRSelf_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// 타임 테이블 정보를 받는다.
	st_RECALL_TIMETABLE stTable ;
	memset(&stTable, 0, sizeof(st_RECALL_TIMETABLE)) ;

	stTable.byTableIdx		= it->second.Get_TableCount() ;

	stTable.wStartDay		= pFile->GetWord() ;			// 시작 날짜.
	stTable.wStartHour		= pFile->GetWord() ;			// 시작 시간.
	stTable.wStartMinute	= pFile->GetWord() ;			// 시작 분.

	stTable.wEndDay			= pFile->GetWord() ;			// 종료 날짜.
	stTable.wEndHour		= pFile->GetWord() ;			// 종료 시간.
	stTable.wEndMinute		= pFile->GetWord() ;			// 종료 분.


	// Npc 정보에 시간 테이블을 추가한다.
	it->second.Insert_TimeTable(&stTable) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetRemainTime
//	DESC		: 소환 유지 시간을 로딩하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetRemainTime(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	BYTE byRecallCondition = pBase->Get_RecallCondition() ;
	if( byRecallCondition < eNR_RemainTime || byRecallCondition > eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// 
	if( byRecallCondition == eNR_RemainTime )
	{
		MAP_RTIME_NPC::iterator it ;
		for( it = m_mRTime_Npc.begin() ; it != m_mRTime_Npc.end() ; ++it )
		{
			if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

			else break ;
		}


		// Npc 정보를 확인한다.
		if( it == m_mRTime_Npc.end() )
		{
			PrintError("Can't find npc info!!", __FUNCTION__) ;
			return ;
		}


		// 소환 유지 시간을 설정한다.
		it->second.Set_RemainTime(pFile->GetWord()) ;
	}
	else if( byRecallCondition == eNR_Moveable )
	{
		MAP_RMOVE_NPC::iterator it ;
		for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
		{
			if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

			else break ;
		}


		// Npc 정보를 확인한다.
		if( it == m_mRMove_Npc.end() )
		{
			PrintError("Can't find npc info!!", __FUNCTION__) ;
			return ;
		}


		// 소환 유지 시간을 설정한다.
		it->second.Set_RemainTime(pFile->GetDword()) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetMoveTime
//	DESC		: 이동을 시작 할 시간을 로딩하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetMoveTime(CMHFile* pFile)
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// 스스로 소환/소멸하는 현재 npc 정보를 받는다.
	MAP_RMOVE_NPC::iterator it ;
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

		else break ;
	}


	// Npc 정보를 확인한다.
	if( it == m_mRMove_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// 소환 유지 시간을 설정한다.
	it->second.Set_MoveTime(pFile->GetDword()) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetMoveXpos
//	DESC		: 이동 할 X좌표를 로딩하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetMoveXpos(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// 스스로 소환/소멸하는 현재 npc 정보를 받는다.
	MAP_RMOVE_NPC::iterator it ;
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

		else break ;
	}


	// Npc 정보를 확인한다.
	if( it == m_mRMove_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// 좌표를 세팅한다.
	it->second.Set_MoveposX(pFile->GetFloat()) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetMoveZpos
//	DESC		: 이동 할 Z좌표를 로딩하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetMoveZpos(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// 스스로 소환/소멸하는 현재 npc 정보를 받는다.
	MAP_RMOVE_NPC::iterator it ;
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

		else break ;
	}


	// Npc 정보를 확인한다.
	if( it == m_mRMove_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// 좌표를 세팅한다.
	it->second.Set_MoveposX(pFile->GetFloat()) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetChangeMapNum
//	DESC		: 포탈을 통해 이동할 맵 번호를 로딩하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetChangeMapNum(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// 스스로 소환/소멸하는 현재 npc 정보를 받는다.
	MAP_RMOVE_NPC::iterator it ;
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

		else break ;
	}


	// Npc 정보를 확인한다.
	if( it == m_mRMove_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// 좌표를 세팅한다.
	it->second.Set_ChangeMapNum(pFile->GetWord()) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetChangeMapXPos
//	DESC		: 포탈을 통해 이동할 맵의 X좌표를 로딩하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetChangeMapXPos(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// 스스로 소환/소멸하는 현재 npc 정보를 받는다.
	MAP_RMOVE_NPC::iterator it ;
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;

		else break ;
	}


	// Npc 정보를 확인한다.
	if( it == m_mRMove_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// 좌표를 세팅한다.
	it->second.Set_ChangeMapXPos(pFile->GetFloat()) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetChangeMapZPos
//	DESC		: 포탈을 통해 이동할 맵의 Z좌표를 로딩하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetChangeMapZPos(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		PrintError("Invalid Npc type!!", __FUNCTION__) ;
		return ;
	}


	// 스스로 소환/소멸하는 현재 npc 정보를 받는다.
	MAP_RMOVE_NPC::iterator it ;
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != pBase->Get_NpcRecallIndex() ) continue ;
		else break ;
	}


	// Npc 정보를 확인한다.
	if( it == m_mRMove_Npc.end() )
	{
		PrintError("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// 좌표를 세팅한다.
	it->second.Set_ChangeMapZPos(pFile->GetFloat()) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_SetRecallFailedMsg
//	DESC		: 이미 소환 된, npc 소환 실패 에러 메시지 번호를 로딩하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 27, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Load_SetRecallFailedMsg(CMHFile* pFile)
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	// 소환 실패시 출력할 에러 메시지를 세팅한다.
	pBase->Set_RecallFailedMsg( pFile->GetWord() ) ;
}

void CNpcRecallMgr::Load_SetParentNpcID(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	pBase->Set_ParentNpcIndex( pFile->GetDword() ) ;
}

void CNpcRecallMgr::Load_SetParentNpcType(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		PrintError("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		PrintError("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	pBase->Set_ParentNpcType( pFile->GetByte() ) ;

}




//-------------------------------------------------------------------------------------------------
//	NAME		: NetworkMsgParser
//	DESC		: 서버로 부터의 네트워크 메시지를 파싱하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 28, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::NetworkMsgParser( BYTE Protocol, void* pMsg ) 
{
	// 함수 인자 확인.
	if( !pMsg )
	{
		PrintError("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// 프로토콜 확인.
	switch( Protocol )
	{
	case MP_RECALLNPC_MTOC_RECALL_NACK :			RecallNpc_Nack( pMsg ) ;		break;

	case MP_RECALLNPC_MTOC_ALREADY_RECALL_NACK :	AlreadyRecallNpc_Nack( pMsg ) ;	break;

	case MP_RECALLNPC_ATOC_CHANGEMAP_NACK :
	case MP_RECALLNPC_MTOC_CHANGEMAP_NACK :			ChangeMap_Nack( pMsg ) ;		break;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: RecallNpc_Nack
//	DESC		: NPC 소환 실패 처리를 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 28, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::RecallNpc_Nack(void* pMsg) 
{
	// 함수 인자 확인.
	if( !pMsg ) 
	{
		PrintError("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	if( !pmsg )
	{
		PrintError("Failed to conver a message!!", __FUNCTION__) ;
		return ;
	}
    

	// 에러 메시지를 확인한다.
	switch( pmsg->bData )
	{
	case e_RNET_INVALID_NPC_TYPE : break ;

	case e_RNET_FAILED_RECEIVE_REMAINNPC : break ;

	case e_RNET_FAILED_ALREADY_RECALLED : break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ChangeMap_Nack
//	DESC		: npc를 통한 맵이동 실패 메시지 처리를 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: September 2, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::ChangeMap_Nack(void* pMsg) 
{
	// 함수 인자 확인.
	if( !pMsg ) 
	{
		PrintError("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	if( !pmsg )
	{
		PrintError("Failed to conver a message!!", __FUNCTION__) ;
		return ;
	}
    

	// 에러 메시지를 확인한다.
	switch( pmsg->bData )
	{
	case e_RNET_FAILED_RECEIVE_USERINFO : break ;

	case e_RNET_FAILED_NOT_SIEGEWARTIME : 
		{
			WINDOWMGR->MsgBox(MBI_RECALLPORTAL_NOTUSE, MBT_OK, "공성중이 아니라 이동할 수 없습니다.") ;
		}
		break ;

	case e_RNET_FAILED_CHANGEMAP_INVALID_STATE : break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: AlreadyRecallNpc_Nack
//	DESC		: 이미 소환 된 npc이기 때문에 소환 실패처리를 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 28, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::AlreadyRecallNpc_Nack(void* pMsg)
{
	// 함수 인자 확인.
	if( !pMsg ) 
	{
		PrintError("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// 원본 메시지 변환.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	if( !pmsg )
	{
		PrintError("Failed to conver a message!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	WORD wErrorMsg = 0 ;
	BYTE byNpcType = Get_NpcType(pmsg->dwData) ;

	switch(byNpcType)
	{
	case eNR_ItSelf : 
		{
			// npc 정보를 받는다.
			CNpcRecallSelf* pInfo = NULL ;
			pInfo = Get_SelfNpc( pmsg->dwData ) ;

			if( !pInfo )
			{
				PrintError("Failed to receive move npc!!", __FUNCTION__) ;
				return ;
			}

			// 에러 메시지를 받는다.
			wErrorMsg = pInfo->Get_RecallFailedMsg() ;
		}
		break ;

	case eNR_RemainTime : 
		{
			// npc 정보를 받는다.
			CNpcRecallRemainTime* pInfo = NULL ;
			pInfo = Get_TimeNpc( pmsg->dwData ) ;

			if( !pInfo )
			{
				PrintError("Failed to receive move npc!!", __FUNCTION__) ;
				return ;
			}

			// 에러 메시지를 받는다.
			wErrorMsg = pInfo->Get_RecallFailedMsg() ;
		}
		break ;

	case eNR_Moveable : 
		{
			// npc 정보를 받는다.
			CNpcRecallMoveable* pInfo = NULL ;
			pInfo = Get_MoveNpc( pmsg->dwData ) ;

			if( !pInfo )
			{
				PrintError("Failed to receive move npc!!", __FUNCTION__) ;
				return ;
			}

			// 에러 메시지를 받는다.
			wErrorMsg = pInfo->Get_RecallFailedMsg() ;
		}
		break ;

	default : break ;
	}	

	WINDOWMGR->MsgBox( MBI_NPCRECALLFAILED, MBT_OK, CHATMGR->GetChatMsg(wErrorMsg) );
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_CurTotalNpcCount
//	DESC		: 스크립트 로딩 중, 지금까지 로드 된 총 npc 수를 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
WORD CNpcRecallMgr::Get_CurTotalNpcCount()
{
	// NPC 타입별, 총 수를 반환한다.
	WORD wTotalCount = 0 ;

	wTotalCount = wTotalCount + (WORD)m_mRSelf_Npc.size() ;

	wTotalCount = wTotalCount + (WORD)m_mRTime_Npc.size() ;

	wTotalCount = wTotalCount + (WORD)m_mRMove_Npc.size() ;

	return wTotalCount ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_CurNpcBase
//	DESC		: 현재 로드 한, npc의 기본 정보를 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallBase* CNpcRecallMgr::Get_CurNpcBase()
{
	// 현재 로딩중인 npc의 인덱스를 체크한다.
	WORD wReturnNpcIdx = 0 ;
	wReturnNpcIdx = Get_CurTotalNpcCount() ;

	if(wReturnNpcIdx == 0) return NULL ;

	--wReturnNpcIdx ; 


	// 스스로 소환/소멸되는 npc인지 확인.
	MAP_RSELF_NPC::iterator itRS ;
	itRS = m_mRSelf_Npc.find(wReturnNpcIdx) ;

	if( itRS != m_mRSelf_Npc.end() ) return ((CNpcRecallBase*)&itRS->second) ;


	// 일정시간 동안 소환되는 npc인지 확인.
	MAP_RTIME_NPC::iterator itRT ;
	itRT = m_mRTime_Npc.find(wReturnNpcIdx) ;

	if( itRT != m_mRTime_Npc.end() ) return ((CNpcRecallBase*)&itRT->second) ;


	// 이동이 가능한 npc인지 확인.
	MAP_RMOVE_NPC::iterator itRM ;
	itRM = m_mRMove_Npc.find(wReturnNpcIdx) ;

	if( itRM != m_mRMove_Npc.end() ) return ((CNpcRecallBase*)&itRM->second) ;


	// null을 return.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_NpcType
//	DESC		: npc 인덱스로 npc의 타입을 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
BYTE CNpcRecallMgr::Get_NpcType(DWORD dwIndex) 
{
	// 인자로 넘어온 인덱스로 npc 타입을 체크한다.
	BYTE byNpcType = eNR_None ;


	// 스스로 소환/소멸되는 npc인지 확인.
	MAP_RSELF_NPC::iterator itRS ;

	for( itRS = m_mRSelf_Npc.begin() ; itRS != m_mRSelf_Npc.end() ; ++itRS )
	{
		if( itRS->second.Get_NpcRecallIndex() == dwIndex )
		{
			return eNR_ItSelf ;
		}
	}


	// 일정시간 동안 소환되는 npc인지 확인.
	MAP_RTIME_NPC::iterator itRT ;

	for( itRT = m_mRTime_Npc.begin() ; itRT != m_mRTime_Npc.end() ; ++itRT )
	{
		if( itRT->second.Get_NpcRecallIndex() == dwIndex )
		{
			return eNR_RemainTime ;
		}
	}


	// 이동이 가능한 npc인지 확인.
	MAP_RMOVE_NPC::iterator itRM ;

	for( itRM = m_mRMove_Npc.begin() ; itRM != m_mRMove_Npc.end() ; ++itRM )
	{
		if( itRM->second.Get_NpcRecallIndex() == dwIndex )
		{
			return eNR_Moveable ;
		}
	}


	// 결과를 return 한다.
	return byNpcType ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_SelfNpc
//	DESC		: 스스로 소환/소멸되는 npc 정보를 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallSelf* CNpcRecallMgr::Get_SelfNpc(DWORD dwIndex)
{
	// npc 정보를 확인한다.
	MAP_RSELF_NPC::iterator it ;

	for( it = m_mRSelf_Npc.begin() ; it != m_mRSelf_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;

		return &it->second ;
	}

	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_TimeNpc
//	DESC		: 일정시간 소환/소멸되는 npc 정보를 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallRemainTime* CNpcRecallMgr::Get_TimeNpc(DWORD dwIndex) 
{
	// npc 정보를 확인한다.
	MAP_RTIME_NPC::iterator it ;	

	for( it = m_mRTime_Npc.begin() ; it != m_mRTime_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;

		return &it->second ;
	}

	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_MoveNpc
//	DESC		: 이동가능한 npc 정보를 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMoveable* CNpcRecallMgr::Get_MoveNpc(DWORD dwIndex)
{
	// npc 정보를 확인한다.
	MAP_RMOVE_NPC::iterator it ;
	
	for( it = m_mRMove_Npc.begin() ; it != m_mRMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;

		return &it->second ;
	}

	return NULL ;
}

CNpcRecallBase* CNpcRecallMgr::Get_RecallBase(DWORD dwIndex)
{
	// 스스로 소환/소멸되는 npc인지 확인.
	MAP_RSELF_NPC::iterator itRS ;

	for( itRS = m_mRSelf_Npc.begin() ; itRS != m_mRSelf_Npc.end() ; ++itRS )
	{
		if( itRS->second.Get_NpcRecallIndex() == dwIndex )
		{
			return &itRS->second ;
		}
	}


	// 일정시간 동안 소환되는 npc인지 확인.
	MAP_RTIME_NPC::iterator itRT ;

	for( itRT = m_mRTime_Npc.begin() ; itRT != m_mRTime_Npc.end() ; ++itRT )
	{
		if( itRT->second.Get_NpcRecallIndex() == dwIndex )
		{
			return &itRT->second ;
		}
	}


	// 이동이 가능한 npc인지 확인.
	MAP_RMOVE_NPC::iterator itRM ;

	for( itRM = m_mRMove_Npc.begin() ; itRM != m_mRMove_Npc.end() ; ++itRM )
	{
		if( itRM->second.Get_NpcRecallIndex() == dwIndex )
		{
			return &itRM->second ;
		}
	}

	return NULL ;
}

CNpcRecallBase* CNpcRecallMgr::Get_RecallBaseByNpcId(DWORD dwNpcIndex)
{
	// 스스로 소환/소멸되는 npc인지 확인.
	MAP_RSELF_NPC::iterator itRS ;

	for( itRS = m_mRSelf_Npc.begin() ; itRS != m_mRSelf_Npc.end() ; ++itRS )
	{
		if( itRS->second.Get_NpcIndex() == dwNpcIndex )
		{
			return &itRS->second ;
		}
	}


	// 일정시간 동안 소환되는 npc인지 확인.
	MAP_RTIME_NPC::iterator itRT ;

	for( itRT = m_mRTime_Npc.begin() ; itRT != m_mRTime_Npc.end() ; ++itRT )
	{
		if( itRT->second.Get_NpcIndex() == dwNpcIndex )
		{
			return &itRT->second ;
		}
	}


	// 이동이 가능한 npc인지 확인.
	MAP_RMOVE_NPC::iterator itRM ;

	for( itRM = m_mRMove_Npc.begin() ; itRM != m_mRMove_Npc.end() ; ++itRM )
	{
		if( itRM->second.Get_NpcIndex() == dwNpcIndex )
		{
			return &itRM->second ;
		}
	}

	return NULL ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: Check_RecallNpc
//	DESC		: 소환 된 npc인지 체크하여, 다음 이벤트 처리를 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 22, 2008
//-------------------------------------------------------------------------------------------------
BYTE CNpcRecallMgr::Check_RecallNpc(DWORD dwRecallIdx) 
{
	// 현재 선택 된 npc 인덱스를 세팅한다.
	Set_CurSelectNpcId( dwRecallIdx ) ;


	// Npc 타입을 확인한다.
	BYTE byNpcType = Get_NpcType( dwRecallIdx ) ;

	switch(byNpcType)
	{
	case eNR_ItSelf : break ;
	case eNR_RemainTime : break ;
	case eNR_Moveable : 
		{
			// 이동 관련 정보를 받는다.
			CNpcRecallMoveable* pInfo = NULL ;
			pInfo = Get_MoveNpc( dwRecallIdx ) ;

			if( !pInfo )
			{
				PrintError("Failed to receive move npc!!", __FUNCTION__) ;
				return FALSE ;
			}

			// 맵 이동을 할 것인지 묻는 메시지 박스를 출력한다.
			WINDOWMGR->MsgBox(MBI_RECALLPORTAL_CHANGEMAP, MBT_YESNO, CHATMGR->GetChatMsg(480), GetMapName( pInfo->Get_ChangeMapNum() )) ;

			return TRUE ;
		}
		break ;

	default : break ;
	}

	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: PrintError
//	DESC		: 에러 메시지를 출력하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::PrintError(char* szErr, char* szCaption)
{
#ifdef _USE_NPCRECALL_ERRBOX_ 
	MessageBox(NULL, szErr, szCaption, MB_OK) ;
#endif //_USE_NPCRECALL_ERRBOX_
}





//-------------------------------------------------------------------------------------------------
//	NAME		: LoadNpcBuffList
//	DESC		: Npc가 시전할 수 있는 버프 스킬 리스트를 로드하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::LoadNPCBuffList()
{
	DWORD dwNpcIdx = 0;
	DWORD dwBuffIdx = 0;
	DWORD dwMoney = 0;
	char szBuff[2048] = {0};
	LPCTSTR seps = "\t\n" ;
	char* token = "";

	CMHFile fp;
	fp.Init("System/Resource/NpcBuffList.bin", "rb") ;
	
	M_NPCBUFFMAP::iterator it ;
	stNPCBuffData::L_BUFFLIST::iterator it_list ;
	BYTE bySameBuffIdx ;
	while(TRUE)
	{
		// 파일의 끝인지 체크한다.
		if( fp.IsEOF() ) break ;

		// 라인을 읽어들인다.
		fp.GetLine( szBuff, 2048 ) ;
		if(strlen(szBuff) == 0) continue ;

		// 081023 LYW --- NpcRecallMgr : npc 버브 정보를 로딩하는 스크립트에서, 주석 문장을 스킵 처리한다.
		// 다른 스크립트들을 보면 Type A 방식을 사용하는데, 이러면, 문자열 중간에 주석 명령이 처리 
		// 되었을 경우, 제대로 주석 처리를 못하게 된다. 문자열 중간에 주석 명령이 들어갈 예외를 생각하여
		// Type B를 사용하도록 하겠다.
		// Type A :
		//if( strstr(szBuff, "//") || IsEmptyLine(szBuff) ) continue ;

		// Type B :
		BYTE byIsCommend = FALSE ;
		for( const TCHAR* subString = szBuff ; *subString ; subString = _tcsinc( subString ) )
		{
			if( !_tcsncmp( subString, "//", _tcslen("//") ) )
			{
				byIsCommend = TRUE ;
				break ;
			}
		}

		if( byIsCommend ) continue ;

		// 라인의 끝인지 확인한다.
		token = strtok( szBuff, seps ) ;

		if(token == NULL) continue ;

		// npc 인덱스를 받는다.
		else dwNpcIdx = atoi(token) ;

		// 081023 LYW --- NpcRecallMgr : 이미 생성 된 npc 정보가 있는지 확인한다.
		it = m_MNpcBuffMap.end() ;
		it = m_MNpcBuffMap.find( dwNpcIdx ) ;

		// 생성된 정보가 없으면, 생성한다.
		if( it == m_MNpcBuffMap.end() )
		{
			stNPCBuffData buffData ;

			buffData.dwNpcIdx = dwNpcIdx ;

			while(TRUE)
			{
				// 버프 인덱스를 읽어들인다.
				token = strtok( NULL, seps ) ;
				if(token == NULL) break ;

				dwBuffIdx = atoi(token) ;

				// 금액을 읽어들인다.
				token = strtok( NULL, seps ) ;
				if(token == NULL) break ;

				dwMoney = atoi(token) ;

				// 버프 인덱스와 금액을 세팅한다.
				if( dwBuffIdx != 0 )
				{
					stNpcBuffInfo buffInfo ;

					buffInfo.dwBuffIdx  = dwBuffIdx ;
					buffInfo.dwMoney	= dwMoney ;

					buffData.buffList.push_back( buffInfo ) ;
				}
			}

			m_MNpcBuffMap.insert(std::make_pair( dwNpcIdx, buffData )) ;
		}
		// 생성 된 정보가 있으면 추가한다.
		else 
		{
			while(TRUE)
			{
				// 버프 인덱스를 읽어들인다.
				token = strtok( NULL, seps ) ;
				if(token == NULL) break ;

				dwBuffIdx = atoi(token) ;

				// 금액을 읽어들인다.
				token = strtok( NULL, seps ) ;
				if(token == NULL) break ;

				dwMoney = atoi(token) ;

				// 이미 있는 버프인지 확인한다.
				bySameBuffIdx = FALSE ;
				for( it_list = it->second.buffList.begin() ; it_list != it->second.buffList.end() ;	++it_list )
				{
					if( it_list->dwBuffIdx == dwBuffIdx ) 
					{
						bySameBuffIdx = TRUE ;
						break ;
					}
				}

				if( bySameBuffIdx ) continue ;

				// 버프 인덱스와 금액을 세팅한다.
				if( dwBuffIdx != 0 )
				{
					stNpcBuffInfo buffInfo ;

					buffInfo.dwBuffIdx  = dwBuffIdx ;
					buffInfo.dwMoney	= dwMoney ;

					it->second.buffList.push_back( buffInfo ) ;
				}
			}
		}
	}


	// 파일 클래스를 해제한다.
	fp.Release() ;
}

stNPCBuffData* CNpcRecallMgr::GetNPCBuffData(DWORD dwNPCIdx) 
{
	// 081023 LYW --- NpcRecallMgr : 메모리 문제로 인해, new delete를 따로 안하는 구조로 수정한다.
	//return m_NPCBuff.GetData(dwNPCIdx);

	M_NPCBUFFMAP::iterator it ;

	it = m_MNpcBuffMap.find( dwNPCIdx ) ;

	if( it == m_MNpcBuffMap.end() ) return NULL ;
	else return &it->second ;
}

void CNpcRecallMgr::OnAddRecallNpc(CNpc* pNpc )
{
	//퀘스트 마크를 달아주자
	QUESTMGR->ProcessNpcMark();
}

void CNpcRecallMgr::OnRemoveRecallNpc(CNpc* pNpc )
{
	if( pNpc == NULL )
		return;

	WORD RemoveNpcIndex = pNpc->GetNpcUniqueIdx() ;
	DWORD dwRecallNpcIdx = pNpc->GetNpcTotalInfo()->dwRecallNpcIdx ;
	BYTE byRecallType = Get_NpcType( dwRecallNpcIdx ) ;

	// 소환 npc라면 
	if( byRecallType != eNR_None )
	{
		CDealDialog* pDealDlg = GAMEIN->GetDealDialog();
		cNpcScriptDialog* pNpcScriptDlg	= GAMEIN->GetNpcScriptDialog();

		// DealDialog가 활성화 되어 있고 딜러가 소환NPC인 경우 작업창을 닫는다.
		if( pDealDlg )
		if( pDealDlg->IsActive() && pDealDlg->GetDealerIdx() == RemoveNpcIndex )
		{
			cDialog* pBuyDivideBox	= WINDOWMGR->GetWindowForID( DBOX_BUY );			// 물건 살때 갯수 입력창
			cDialog* pSellDivideBox	= WINDOWMGR->GetWindowForID( DBOX_SELL );			// 물건 팔때 갯수 입력창
			cDialog* pBuyMsgBox		= WINDOWMGR->GetWindowForID( MBI_BUYITEM );			// 물건 살때 MSG BOX
			cDialog* pSellMsgBox	= WINDOWMGR->GetWindowForID( MBI_SELLITEM );		// 물건 팔때 MSG BOX
			
			if( pBuyDivideBox )
			{
				pBuyDivideBox->SetActive( FALSE );
				WINDOWMGR->AddListDestroyWindow( pBuyDivideBox );
			}
			if( pSellDivideBox )
			{
				pSellDivideBox->SetActive( FALSE );
				WINDOWMGR->AddListDestroyWindow( pSellDivideBox );
			}
			if( pBuyMsgBox )
			{
				pBuyMsgBox->SetActive( FALSE );
				WINDOWMGR->AddListDestroyWindow( pBuyMsgBox );
			}
			if( pSellMsgBox )
			{
				pSellMsgBox->SetActive( FALSE );
				WINDOWMGR->AddListDestroyWindow( pSellMsgBox );
			}

			((cDialog*)GAMEIN->GetInventoryDialog())->SetDisable( FALSE );
			((cDialog*)GAMEIN->GetInventoryDialog())->SetActive( FALSE );				// 인벤토리를 닫는다
			pDealDlg->SetDisable( FALSE );
			pDealDlg->SetActive( FALSE );	
		}

		// ScriptDialog가 활성화 되어 있고 소환NPC인 경우 닫는다.
		if( pNpcScriptDlg )
		if( pNpcScriptDlg->IsActive() && pNpcScriptDlg->GetCurNpcIdx() == RemoveNpcIndex )
		{
			pNpcScriptDlg->SetActive( FALSE );											// NPC Script 창을 닫는다
		}
	}
}