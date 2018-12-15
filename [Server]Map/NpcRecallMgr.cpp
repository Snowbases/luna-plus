//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMgr.cpp
//	DESC		: Implementation part of CNpcRecallMgr class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "MHFile.h"

#include "./NpcRecallMgr.h"

#include "./CharMove.h"

#include "./Npc.h"

#include "./Network.h"

#include "./ObjectFactory.h"

#include "./UserTable.h"

#include "../[CC]Header/GameResourceManager.h"

#include "./AISystem.h"

#include "./ChannelSystem.h"

#include "./MapDBMsgParser.h"

#include "./Player.h"

#include "./LootingManager.h"

//#include "../hseos/Date/SHDateManager.h"

#include "./SiegeWarfareMgr.h" 

#include "../[CC]Skill/Server/Info/BuffSkillInfo.h"

#include "../[CC]Skill/Server/Manager/SkillManager.h"

#include "ItemManager.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMgr
//	DESC		: 생성자 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMgr::CNpcRecallMgr(void)
{
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

	// DB에 저장되어있던 소환 Npc 불러오기 
	Load_NpcFromDB() ;
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


	// SiegeRecall.bin 파일을 읽기 모드로 연다.
	if( !file.Init("./system/resource/NpcRecallInfo.bin", "rb") )
	{
		Throw_Error( "Failed to initialize SiegeRecall.bin", __FUNCTION__ ) ;
		return ;
	}


	// 파일로 부터 스크립트 정보를 로딩한다.
	char szBuff[257] = {0, } ;
	BYTE byCommandKind = e_RNC_MAX ;

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

void CNpcRecallMgr::Load_NpcFromDB()
{
	// DB에 남은 시간을 업데이트 
	char txt[256] = {0, } ;
	sprintf(txt, "EXEC %s %d", MP_NPCREMAINTIME_LOAD,
		g_pServerSystem->GetMapNum() ) ;

	g_DB.Query( eQueryType_FreeQuery, eNpcRemainTime_Load, 0, txt) ;
}

void CNpcRecallMgr::OnLoadNpc_FromDB( DWORD dwRecallidx , DWORD dwChenel , DWORD dwRemainTime )
{
	BYTE byRecallType = Get_NpcType( dwRecallidx ) ;

	switch( byRecallType ) 
	{
		case eNR_ItSelf :
			break;
		case eNR_RemainTime :
			{
				CNpcRecallRemainTime* pRecallNpc = Get_TimeNpc( dwRecallidx ) ;

				if( ! pRecallNpc )
				{
					return;
				}

				CNpcRecallRemainTime Temp;
				memcpy( &Temp , pRecallNpc , sizeof( CNpcRecallRemainTime ) ) ;

				DWORD dwTimeDelta = pRecallNpc->Get_RemainTime() - dwRemainTime ;
				Temp.Set_RemainTime( dwRemainTime ) ;
				Temp.Set_RecallTime( GetTickCount() - dwTimeDelta ) ;

				Recall_Npc( &Temp ) ;
			}
			break;
		case eNR_Moveable :
			{
				CNpcRecallMoveable* pRecallNpc = Get_MoveNpc( dwRecallidx ) ;

				if( ! pRecallNpc )
				{
					return;
				}

				CNpcRecallMoveable Temp;
				memcpy( &Temp , pRecallNpc , sizeof( CNpcRecallMoveable ) ) ;

				DWORD dwTimeDelta = pRecallNpc->Get_RemainTime() - dwRemainTime ;
				Temp.Set_RemainTime( dwRemainTime ) ;
				Temp.Set_RecallTime( GetTickCount() - dwTimeDelta ) ;

				Recall_Npc( &Temp ) ;
			}
			break;
		default:
			break;
	}

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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return byCommandKind ;
	}


	// 스크립트 명령어의 스트링 사이즈 체크.
	if( strlen(pString) <= 1 )
	{
		Throw_Error("Invalid string size!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	//// 동일한 npc id가 있는지 확인한다.
	//DWORD dwNpcId ;

	//MAP_RSELF_NPC::iterator itRS ;
	//for( itRS = m_mRSelf_Npc.begin() ; itRS != m_mRSelf_Npc.end() ; ++itRS )
	//{
	//	dwNpcId = 0 ;
	//	dwNpcId = itRS->second.Get_NpcIndex() ;

	//	if(dwNpcId == 0) continue ;

	//	if( dwNpcId == pBase->Get_NpcIndex() )
	//	{
	//		Throw_Error("Clone npc id!!", __FUNCTION__) ;
	//		return ;
	//	}
	//}

	//MAP_RTIME_NPC::iterator itRT ;
	//for( itRT = m_mRTime_Npc.begin() ; itRT != m_mRTime_Npc.end() ; ++itRT )
	//{
	//	dwNpcId = 0 ;
	//	dwNpcId = itRT->second.Get_NpcIndex() ;

	//	if(dwNpcId == 0) continue ;

	//	if( dwNpcId == pBase->Get_NpcIndex() )
	//	{
	//		Throw_Error("Clone npc id!!", __FUNCTION__) ;
	//		return ;
	//	}
	//}

	//MAP_RMOVE_NPC::iterator itRM ;
	//for( itRM = m_mRMove_Npc.begin() ; itRM != m_mRMove_Npc.end() ; ++itRM )
	//{
	//	dwNpcId = 0 ;
	//	dwNpcId = itRM->second.Get_NpcIndex() ;

	//	if(dwNpcId == 0) continue ;

	//	if( dwNpcId == pBase->Get_NpcIndex() )
	//	{
	//		Throw_Error("Clone npc id!!", __FUNCTION__) ;
	//		return ;
	//	}
	//}

	
	// Npc인덱스를 설정한다.
	pBase->Set_NpcIndex( pFile->GetDword() ) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_ItSelf )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	BYTE byRecallCondition = pBase->Get_RecallCondition() ;
	if( byRecallCondition < eNR_RemainTime || byRecallCondition > eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
			Throw_Error("Can't find npc info!!", __FUNCTION__) ;
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
			Throw_Error("Can't find npc info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// 좌표를 세팅한다.
	it->second.Set_MoveposZ(pFile->GetFloat()) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// 좌표를 세팅한다.
	it->second.Set_ChangeMapXPos(pFile->GetWord()) ;
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
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}


	// Npc 타입을 확인한다.
	if( pBase->Get_RecallCondition() != eNR_Moveable )
	{
		Throw_Error("Invalid Npc type!!", __FUNCTION__) ;
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
		Throw_Error("Can't find npc info!!", __FUNCTION__) ;
		return ;
	}


	// 좌표를 세팅한다.
	it->second.Set_ChangeMapZPos(pFile->GetWord()) ;
}

void CNpcRecallMgr::Load_SetParentNpcID(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	pBase->Set_ParentNpcIndex( pFile->GetDword() ) ;
}

void CNpcRecallMgr::Load_SetParentNpcType(CMHFile* pFile) 
{
	// 함수 인자 확인.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	// 현재 로딩중인 npc 기본 정보를 받는다.
	CNpcRecallBase* pBase = NULL ;
	pBase = Get_CurNpcBase() ;

	if(!pBase)
	{
		Throw_Error("Invalid Base info!!", __FUNCTION__) ;
		return ;
	}

	
	pBase->Set_ParentNpcType( pFile->GetByte() ) ;

}


//-------------------------------------------------------------------------------------------------
//	NAME		: Process
//	DESC		: 프로세싱 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Process() 
{
	// 시간체크를 1분 단위로 한다.
	DWORD dwCurTick = GetTickCount() ;
	static DWORD dwLastCheckTime = 0;

	if( dwCurTick - dwLastCheckTime < 60000 )	return ;	
	else dwLastCheckTime = dwCurTick ;


	// 스스로 소환/소멸되는 npc 스케쥴을 확인한다.
	CheckSchedule_SelfRecallNpc() ;


	// 일정 시간동안 소환/소멸되는 npc의 스케쥴을 체크한다.
	CheckSchedule_RemainTimeNpc() ;


	// 이동 가능한 npc의 스케쥴을 체크한다.
	CheckSchedule_MoveableNpc() ;
}

//-------------------------------------------------------------------------------------------------
//	NAME		: NetworkMsgParser
//	DESC		: 메시지 파서 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 20, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::NetworkMsgParser(DWORD dwIndex, void* pMsg, DWORD dwLength)
{
	// 함수 인자를 확인한다.
	if(!pMsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 기본형 메시지로 변환한다.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 프로토콜을 확인한다.
	switch(pmsg->Protocol)
	{
	case MP_RECALLNPC_CTOM_RECALL_SYN :		CTOM_Recall_Syn(dwIndex, (char*)pMsg, dwLength) ;			break ;

	case MP_RECALLNPC_CTOAM_CHANGEMAP_SYN :	CTOAM_ChangeMap_Syn(dwIndex, (char*)pMsg, dwLength) ;		break ;

	case MP_RECALLNPC_CHEAT_RECALL_SYN :	Cheat_CTOA_Recall_Syn(dwIndex, (char*)pMsg, dwLength) ;	break ;

	case MP_RECALLNPC_ATOM_CHANGEMAP_ACK :	ATOM_ChangeMap_Ack(dwIndex, pMsg, dwLength) ;	break ;

	default : break ;
	}
}

//-------------------------------------------------------------------------------------------------
//	NAME		: CheckSchedule_SelfRecallNpc
//	DESC		: 스스로 소환/소멸되는 npc의 스케쥴을 체크한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 22, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::CheckSchedule_SelfRecallNpc() 
{
	// 현재 시스템의 시간 정보를 받는다.
	SYSTEMTIME cTime ;
	GetLocalTime( &cTime ) ;


	// 컨테이너에 담긴 npc들의 스케쥴을 확인한다.
	BYTE byTableCount ;
	BYTE byCheckCount ;

	BYTE IsRunningNpc ;

	st_RECALL_TIMETABLE* pTable ;

	MAP_RSELF_NPC::iterator it ;
	MAP_ARSELF_NPC::iterator checkIt ;

	cPtrList	DeleteNpcList;

	// NPC 수 만큼 FOR문을 돌린다.
	for( it = m_mRSelf_Npc.begin() ; it != m_mRSelf_Npc.end() ;  ++it)
	{
		if( it->second.Get_RecallMap() != g_pServerSystem->GetMapNum() ) 
			continue;

		// 테이블 카운트를 받는다.
		byTableCount = 0 ;
		byTableCount = it->second.Get_TableCount() ;

		enum eSCHEDULE { eSCHEDULE_NONE, eSCHEDULE_START, eSCHEDULE_END}	
		eResultSchedule = eSCHEDULE_NONE ;

		// 테이블 수 만큼 for문을 돌린다.
		for( byCheckCount = 0 ; byCheckCount < byTableCount ; ++byCheckCount )
		{
			// 테이블 정보를 받는다.
			pTable = NULL ;
			pTable = it->second.Get_TimeTable(byCheckCount) ;

			if(!pTable)
			{
				char szErr[125] = {0, } ;
				sprintf(szErr, "Invalid table info!! : %d, %d", it->second.Get_NpcRecallIndex(), byCheckCount ) ;
				Throw_Error(szErr, __FUNCTION__) ;
				return ;
			}

			// 스케쥴을 확인한다.
			IsRunningNpc = FALSE ;


			// 091111 pdy 시간채크가 잘못되어 있어 수정
			// 요일의 범위안이 아닌가 채크 
			if(  pTable->wStartDay ==  pTable->wEndDay && cTime.wDayOfWeek != pTable->wStartDay )
			{
				continue;
			}

			if( pTable->wStartDay < pTable->wEndDay && 
				( cTime.wDayOfWeek < pTable->wStartDay || cTime.wDayOfWeek > pTable->wEndDay ) )
			{
				//소환시간이 아니다.
				continue;
			}

			if( pTable->wStartDay > pTable->wEndDay && 
				cTime.wDayOfWeek <  pTable->wStartDay && cTime.wDayOfWeek > pTable->wEndDay )
			{
				//소환시간이 아니다.
				continue;
			}

			//소환 시작 시간보다 작은가 채크 
			if( cTime.wDayOfWeek == pTable->wStartDay )
			{
				//시작하는 시간보다 시간이 작거나 시간이 같고 시작 분보다 작다면 
				if( cTime.wHour < pTable->wStartHour ||
					cTime.wHour == pTable->wStartHour && cTime.wMinute < pTable->wStartMinute )
				{
					continue;
				}
			}

			//소환 종료 시간을 지났는지 채크 
			if( cTime.wDayOfWeek == pTable->wEndDay )
			{
				//끝나는 시간보다 시간이 지났거나 시간이 같고 끝나는 분보다 같거나 크다면
				if( cTime.wHour > pTable->wEndHour ||
					cTime.wHour == pTable->wEndHour && cTime.wMinute >= pTable->wEndMinute )
				{
					eResultSchedule = eSCHEDULE_END ;
					continue;
				}
			}

			//여기까지 왔다면 소환 시간이다. 
			eResultSchedule = eSCHEDULE_START ;
			break;
		}

		if( eResultSchedule == eSCHEDULE_START ) 
		{
			// 활성화 된 npc 리스트에 없다면, 추가 처리를 한다.
			CNpcRecallBase* pRecalledNpcBase = Get_ActiveSelfNpc( it->second.Get_NpcRecallIndex() , 1 ) ;

			if( pRecalledNpcBase == NULL )
			{
				Recall_Npc( &it->second ) ;
			}
		}
		else if( eResultSchedule == eSCHEDULE_END )
		{
			// 채널 수를 받는다.
			BYTE byChannelCount = (BYTE)CHANNELSYSTEM->GetChannelCount() ;

			// npc 추가 작업.
			for( BYTE byCount = 0 ; byCount < byChannelCount ; ++byCount )
			{
				WORD wChenel = byCount +1 ;
				CNpcRecallBase* pActivedNpcBase = Get_ActiveSelfNpc( it->second.Get_NpcRecallIndex() , wChenel ) ;

				if( pActivedNpcBase == NULL) 
					continue;

				DeleteNpcList.AddTail( pActivedNpcBase ) ;
			}
		}
	}

	PTRLISTPOS pos = DeleteNpcList.GetHeadPosition();
	while( pos )
	{
		CNpcRecallBase* pRecallBase = ( CNpcRecallBase* )DeleteNpcList.GetNext( pos );
		Delete_Npc( pRecallBase ) ;
	}
	DeleteNpcList.RemoveAll() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: CheckSchedule_RemainTimeNpc
//	DESC		: 일정 시간동안 소환/소멸되는 npc의 스케쥴을 체크한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 22, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::CheckSchedule_RemainTimeNpc()
{
	// 활성화 중인 npc의 이동가능 시간/ 소멸 시간을 확인한다.
	DWORD dwRemainTime ;
	MAP_ARTIME_NPC::iterator it ;

	cPtrList	DeleteNpcList;

	// 맵 컨테이너에 담긴 수 만큼 for문을 돌린다.
	for( it = m_mARTime_Npc.begin() ; it != m_mARTime_Npc.end() ; ++it )
	{
		if( it->second.Get_RecallMap() != g_pServerSystem->GetMapNum() ) 
			continue;

		// 남은 시간을 받는다.
		dwRemainTime = 0 ;
		dwRemainTime = it->second.Get_RemainTime() ;

		// 남은시간이 1분 이상일경우, 남은 시간을 감소시킨다.
		if( dwRemainTime >= SCHEDULE_CHECK_TIME )
		{
			dwRemainTime = dwRemainTime - SCHEDULE_CHECK_TIME ;

			if( dwRemainTime > it->second.Get_RemainTime() ) 
			{
				dwRemainTime = 0 ;
			}

			it->second.Set_RemainTime( dwRemainTime ) ;

			// DB에 남은 시간을 업데이트 
			char txt[256] = {0, } ;
			sprintf(txt, "EXEC %s %u, %d, %u", MP_NPCREMAINTIME_UPDATE,
				it->second.Get_NpcRecallIndex(), it->second.Get_RecalledChenel() ,dwRemainTime ) ;

			g_DB.Query( eQueryType_FreeQuery, eNpcRemainTime_Update, 0, txt) ;
			continue ;
		}
		// 남은 시간이 1분 미만일 경우, 삭제 준비를 세팅하거나, 삭제 처리를 한다.
		else
		{
			// 맵 서버로, npc 삭제 요청.
			if( it->second.Is_ReadyToDelete() )
			{
				DeleteNpcList.AddTail( &it->second ) ;
			}
			// 삭제 대기상태로 세팅.
			else
			{
				//it->second.Set_RemainTime( 0 ) ;
				it->second.ClearRemainTime() ;
				it->second.Set_ReadyToDelete(TRUE) ;
			}
		}
	}

	PTRLISTPOS pos = DeleteNpcList.GetHeadPosition();
	while( pos )
	{
		CNpcRecallBase* pRecallBase = ( CNpcRecallBase* )DeleteNpcList.GetNext( pos );
		Delete_Npc( pRecallBase ) ;
	}
	DeleteNpcList.RemoveAll() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: CheckSchedule_MoveableNpc
//	DESC		: 이동 가능한 npc의 스케쥴을 체크한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 22, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::CheckSchedule_MoveableNpc() 
{
	// 활성화 중인 npc의 이동가능 시간/ 소멸 시간을 확인한다.
	DWORD dwRemainTime ;
	MAP_ARMOVE_NPC::iterator	it ;

	cPtrList	DeleteNpcList;

	// 맵 컨테이너에 담긴 수 만큼 for문을 돌린다.
	for( it = m_mARMove_Npc.begin() ; it != m_mARMove_Npc.end() ; ++it )
	{
		if( it->second.Get_RecallMap() != g_pServerSystem->GetMapNum() ) 
			continue;

		// npc 스스로 이동 여부를 처리한다.
		if( !it->second.Is_Moved() )
		{
			// 맵 서버로, npc 이동 요청.
			if( GetTickCount() - it->second.Get_RecallTime() >= it->second.Get_MoveTime() )
			{
				it->second.Set_Moved(TRUE) ;

				Move_Npc( &it->second ) ;
			}
		}

		// 남은 시간을 받는다.
		dwRemainTime = 0 ;
		dwRemainTime = it->second.Get_RemainTime() ;

		// 남은시간이 1분 이상일경우, 남은 시간을 감소시킨다.
		if( dwRemainTime >= SCHEDULE_CHECK_TIME )
		{
			dwRemainTime = dwRemainTime - SCHEDULE_CHECK_TIME ;

			if( dwRemainTime > it->second.Get_RemainTime() ) 
			{
				dwRemainTime = 0 ;
			}

			it->second.Set_RemainTime( dwRemainTime ) ;

			// DB에 남은 시간을 업데이트 
			char txt[256] = {0, } ;
			sprintf(txt, "EXEC %s %u, %d, %u", MP_NPCREMAINTIME_UPDATE,
				it->second.Get_NpcRecallIndex(), it->second.Get_RecalledChenel() ,dwRemainTime ) ;

			g_DB.Query( eQueryType_FreeQuery, eNpcRemainTime_Update, 0, txt) ;
			continue ;
		}
		// 남은 시간이 1분 미만일 경우, 삭제 준비를 세팅하거나, 삭제 처리를 한다.
		else
		{
			// 맵 서버로, npc 삭제 요청.
			if( it->second.Is_ReadyToDelete() )
			{
				DeleteNpcList.AddTail( &it->second ) ;
			}
			// 삭제 대기상태로 세팅.
			else
			{
				it->second.ClearRemainTime() ;
				it->second.Set_ReadyToDelete(TRUE) ;
			}
		}
	}

	PTRLISTPOS pos = DeleteNpcList.GetHeadPosition();
	while( pos )
	{
		CNpcRecallBase* pRecallBase = ( CNpcRecallBase* )DeleteNpcList.GetNext( pos );
		Delete_Npc( pRecallBase ) ;
	}
	DeleteNpcList.RemoveAll() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: CTOA_Recall_Syn
//	DESC		: 클라이언트로 부터 npc 소환 요청이 들어왔을 때, 처릴하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 19, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::CTOM_Recall_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) 
{
	// 함수 인자 확인.
	if( !pMsg )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// 원본 메시지 변환.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}


	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );								

	if( !pPlayer)
	{
		Throw_Error("Invalid Player ID !!", __FUNCTION__) ;
		return ;
	}

	// NPC 인덱스를 확인하여, 해당하는 NPC 정보를 받는다.

	CNpcRecallBase* pRecallBase = Get_RecallBase( pmsg->dwData ) ;
	BYTE byNpctype = (BYTE)((!pRecallBase)? eNR_None : pRecallBase->Get_RecallCondition() );

	if( byNpctype <= eNR_ItSelf || byNpctype >= eNR_NpcRecall_Max )
	{
		// 클라이언트로 에러 메시지를 전송한다.
		MSG_BYTE msg ;

		msg.Category	= MP_RECALLNPC ;
		msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

		msg.dwObjectID	= dwIndex ;

		msg.bData		= e_RNET_INVALID_NPC_TYPE  ;

		pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

		// 서버에 에러 박스나, 로그를 남긴다.
		Throw_Error("Invalid npc type!!", __FUNCTION__) ;
		return;
	}

	// NPC 타입을 확인한다.
	switch(byNpctype)
	{
	case eNR_RemainTime : 
		{
			// Npc 정보를 확인한다.
			CNpcRecallRemainTime* pNpc = NULL ;
			pNpc = Get_TimeNpc(pmsg->dwData ) ;

			if(!pNpc)
			{
				// 클라이언트로 에러 메시지를 전송한다.
				MSG_BYTE msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

				msg.dwObjectID	= pmsg->dwObjectID ;

				msg.bData		= e_RNET_FAILED_RECEIVE_REMAINNPC  ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

				// 서버에 에러 박스나, 로그를 남기고 return 한다.
				Throw_Error("Failed to receive remain npc!!", __FUNCTION__) ;
				return ;
			}

			if(! CheckHackRecallNpc( pPlayer , pmsg->dwData ) )
			{
				// 클라이언트로 에러 메시지를 전송한다.
				MSG_BYTE msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

				msg.dwObjectID	= dwIndex ;

				msg.bData		= e_RNET_FAILED_CREATE_NPC  ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;
				return;
			}

			// 활성화 된 npc 정보를 확인한다.
			CNpcRecallRemainTime* pANpc = NULL ;
			pANpc = Get_ActiveTimeNpc(pmsg->dwData) ;

			if(pANpc)
			{
				// 클라이언트로, 이미 소환되었음을 전송한다.
				MSG_DWORD msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_ALREADY_RECALL_NACK ;

				msg.dwObjectID	= pmsg->dwObjectID ;

				msg.dwData		= pmsg->dwData ;
				pPlayer->SendMsg(&msg, sizeof(MSG_DWORD)) ;

				//// 서버에 에러 박스나, 로그를 남기고 return 한다.
				//Throw_Error("Already recalled npc!!", __FUNCTION__) ;
				return ;
			}

			Recall_Npc( pNpc ) ;
		}
		break ;

	case eNR_Moveable : 
		{
			// Npc 정보를 확인한다.
			CNpcRecallMoveable* pNpc = NULL ;
			pNpc = Get_MoveNpc(pmsg->dwData ) ;

			if(!pNpc)
			{
				// 클라이언트로 에러 메시지를 전송한다.
				MSG_BYTE msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

				msg.dwObjectID	= dwIndex ;

				msg.bData		= e_RNET_FAILED_RECEIVE_REMAINNPC  ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;
		
				// 서버에 에러 박스나, 로그를 남기고 return 한다.
				Throw_Error("Failed to receive remain npc!!", __FUNCTION__) ;
				return ;
			}

			if(! CheckHackRecallNpc( pPlayer , pmsg->dwData ) )
			{
				// 클라이언트로 에러 메시지를 전송한다.
				MSG_BYTE msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

				msg.dwObjectID	= dwIndex ;

				msg.bData		= e_RNET_FAILED_CREATE_NPC  ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;
				return;
			}

			// 활성화 된 npc 정보를 확인한다.
			CNpcRecallMoveable* pANpc = NULL ;
			pANpc = Get_ActiveMoveNpc(pmsg->dwData) ;

			if(pANpc)
			{
				// 클라이언트로, 이미 소환되었음을 전송한다.
				MSG_DWORD msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_ALREADY_RECALL_NACK ;

				msg.dwObjectID	= pmsg->dwObjectID ;

				msg.dwData		= pmsg->dwData ;

				pPlayer->SendMsg(&msg, sizeof(MSG_DWORD)) ;

				//// 서버에 에러 박스나, 로그를 남기고 return 한다.
				//Throw_Error("Already recalled npc!!", __FUNCTION__) ;
				return ;
			}

			Recall_Npc( pNpc ) ;
		}
		break ;

	default : break ;
	}
}

//-------------------------------------------------------------------------------------------------
//	NAME		: WriteRecallNpcLog
//	DESC		: The function to create a error log for recall npc.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 28, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::WriteRecallNpcLog( char* pMsg )
{
	// 함수 인자를 확인한다.
	if( !pMsg ) return ;


	// 로그를 남긴다.
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	TCHAR szFile[_MAX_PATH] = {0, } ;
	sprintf( szFile, "Log/Siege_RecallNpcLog_%04d%02d%02d.log", time.wYear, time.wMonth, time.wDay ) ;

	FILE *fp = fopen(szFile, "a+") ;
	if (fp)
	{
		fprintf(fp, "[%s]\t%s\n", szTime, pMsg) ;
		fclose(fp) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: LoadNpcBuffList
//	DESC		: Npc가 시전할 수 있는 버프 스킬 리스트를 로드하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::LoadNPCBuffList()
{
	DWORD	dwNpcIdx ;
	DWORD	dwBuffIdx ;
	DWORD	dwMoney ;

	char	szBuff[2048] ;
	char	seps[] = "\t\n" ;
	char*	token ;

	CMHFile fp ;
	fp.Init("./System/Resource/NpcBuffList.bin", "rb") ;

	M_NPCBUFFMAP::iterator it ;
	stNPCBuffData::L_BUFFLIST::iterator it_list ;
	BYTE bySameBuffIdx ;
	while(FALSE == fp.IsEOF())
	{
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





// 080826 KTH
BYTE CNpcRecallMgr::ExcuteNPCBuff(CPlayer* pPlayer, WORD wNPCIndex, DWORD dwSelectBuff)
{
	// Player 정보를 확인한다.
	if( !pPlayer ) return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_PLAYERINFO ;
	
	// Npc 정보를 확인한다.
	STATIC_NPCINFO* pInfo = GAMERESRCMNGR->GetStaticNpcInfo( wNPCIndex ) ;
	if( !pInfo ) return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_NPCINFO ;


	// Npc와 Player의 거리를 체크한다.
	VECTOR3 vPos ;
	pPlayer->GetPosition( &vPos ) ;

	float dist = CalcDistanceXZ( &vPos, &pInfo->vPos ) ;

	if( dist > 3000.0f ) return e_REQUEST_BUFFSKILL_TOO_FAR_DISTANCE ;


	// Npc의 버프 데이터를 받는다.
	stNPCBuffData* pNPCBuff = GetNPCBuffData(wNPCIndex) ;

	if( !pNPCBuff) return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFDATA ;


	// Npc 버프 타입(모두/특정버프)를 확인한다.
	DWORD dwBuffPay = 0 ;

	// 일치하는 npc의 버프 데이터를 받는다.
	M_NPCBUFFMAP::iterator it ;
	it = m_MNpcBuffMap.find( wNPCIndex ) ;
	if( it == m_MNpcBuffMap.end() )
	{
		return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_NPCINFO ; 
	}

	switch( dwSelectBuff )
	{
	case 0:
		{
			// 일치하는 버프 정보를 받는다.
			stNPCBuffData::L_BUFFLIST::iterator it_list ;

			for( it_list = it->second.buffList.begin() ; it_list != it->second.buffList.end() ; ++it_list )
			{
				// 스킬 시전비를 지불한다.
				if( pPlayer->GetMoney() < it_list->dwMoney ) 
				{
					return e_REQUEST_BUFFSKILL_NOTENOUGH_PAY ;
				}
				else
				{
					pPlayer->SetMoney( it_list->dwMoney, MONEY_SUBTRACTION ) ;
				}

				// 버프 스킬 시전 금액을 설정한다.
				dwBuffPay += it_list->dwMoney ;

				// 081113 LYW --- NpcRecallMgr : 집행위원으로 부터 스킬을 받고 맵이동하면, 버프가 사라지는 현상 수정.
				cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( it_list->dwBuffIdx );
				if( !pSkillInfo ) return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFSKILLINFO ;

				sSKILL_CREATE_INFO createinfo;

				createinfo.level			= pSkillInfo->GetLevel() ;
				createinfo.operatorId		= pPlayer->GetID() ;
				createinfo.mainTarget.SetMainTarget( pPlayer->GetID() ) ;
				createinfo.pos				= *CCharMove::GetPosition( pPlayer ) ;
				createinfo.skillObjectId	= 0 ;
				createinfo.remainTime		= 0 ;
				createinfo.count			= 1 ;

				SKILLMGR->OnBuffSkillStart( pSkillInfo, &createinfo ) ;
			}
		}
		break;
	default:
		{
			// 일치하는 버프 정보를 받는다.
			stNPCBuffData::L_BUFFLIST::iterator it_list ;

			for( it_list = it->second.buffList.begin() ; it_list != it->second.buffList.end() ; ++it_list )
			{
				// 일치하는 버프 정보가 없으면 continue.
				if( it_list->dwBuffIdx != dwSelectBuff ) continue ;

				const cBuffSkillInfo* const pBuffSkillInfo = SKILLMGR->GetBuffInfo(it_list->dwBuffIdx);

				if( !pBuffSkillInfo ) return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFSKILLINFO ;

				// 스킬 시전비를 지불한다.
				if( pPlayer->GetMoney() < it_list->dwMoney ) 
				{
					return e_REQUEST_BUFFSKILL_NOTENOUGH_PAY ;
				}
				else
				{
					pPlayer->SetMoney( it_list->dwMoney, MONEY_SUBTRACTION ) ;
				}

				// 버프 스킬 시전 금액을 설정한다.
				dwBuffPay += it_list->dwMoney ;

				cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( it_list->dwBuffIdx );
				if( !pSkillInfo ) return e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFSKILLINFO ;

				sSKILL_CREATE_INFO createinfo;

				createinfo.level			= pSkillInfo->GetLevel() ;
				createinfo.operatorId		= pPlayer->GetID() ;
				createinfo.mainTarget.SetMainTarget( pPlayer->GetID() ) ;
				createinfo.pos				= *CCharMove::GetPosition( pPlayer ) ;
				createinfo.skillObjectId	= 0 ;
				createinfo.remainTime		= 0 ;
				createinfo.count			= 1 ;

				SKILLMGR->OnBuffSkillStart( pSkillInfo, &createinfo ) ;

				break ;
			}
		}
		break;
	}

	MSG_DWORD msg ; 

	msg.Category	= MP_SIEGEWARFARE ;
	msg.Protocol	= MP_SIEGEWARFARE_BUFF_ACK ;

	msg.dwData		= pPlayer->GetID() ;

	msg.dwData		= dwBuffPay ;

	pPlayer->SendMsg( &msg, sizeof(MSG_DWORD) ) ;

	return e_REQUEST_BUFFSKILL_SUCCESS ;

}

//-------------------------------------------------------------------------------------------------
//	NAME		: Cheat_CTOA_Recall_Syn
//	DESC		: 테스트용 치트 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 20, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Cheat_CTOA_Recall_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 인자 확인.
	if( !pMsg )
	{
		Throw_Error("Invalid parameter!!", "CNpcRecallMgr::Cheat_CTOA_Recall_Syn") ;
		return ;
	}


	// 원본 메시지 변환.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message to original!!", "CNpcRecallMgr::Cheat_CTOA_Recall_Syn") ;
		return ;
	}

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );								

	if( !pPlayer)
	{
		Throw_Error("Invalid Player ID !!", __FUNCTION__) ;
		return ;
	}


	// NPC 인덱스를 확인하여, 해당하는 NPC 정보를 받는다.
	BYTE byNpctype = Get_NpcType( pmsg->dwData ) ;

	if( byNpctype <= eNR_ItSelf || byNpctype >= eNR_NpcRecall_Max )
	{
		// 클라이언트로 에러 메시지를 전송한다.
		MSG_BYTE msg ;

		msg.Category	= MP_RECALLNPC ;
		msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

		msg.dwObjectID	= dwIndex ;

		msg.bData		= e_RNET_INVALID_NPC_TYPE  ;

		pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

		// 서버에 에러 박스나, 로그를 남긴다.
		Throw_Error("Invalid npc type!!", "CNpcRecallMgr::Cheat_CTOA_Recall_Syn") ;
	}


	// NPC 타입을 확인한다.
	switch(byNpctype)
	{
	case eNR_RemainTime : 
		{
			// Npc 정보를 확인한다.
			CNpcRecallRemainTime* pNpc = NULL ;
			pNpc = Get_TimeNpc(pmsg->dwData) ;

			if(!pNpc)
			{
				// 클라이언트로 에러 메시지를 전송한다.
				MSG_BYTE msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

				msg.dwObjectID	= dwIndex ;

				msg.bData		= e_RNET_FAILED_RECEIVE_REMAINNPC  ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

				// 서버에 에러 박스나, 로그를 남기고 return 한다.
				Throw_Error("Failed to receive remain npc!!", "CNpcRecallMgr::Cheat_CTOA_Recall_Syn") ;
				return ;
			}

			Recall_Npc( pNpc ) ;
		}
		break ;

	case eNR_Moveable : 
		{
			// Npc 정보를 확인한다.
			CNpcRecallMoveable* pNpc = NULL ;
			pNpc = Get_MoveNpc(pmsg->dwData) ;

			if(!pNpc)
			{
				// 클라이언트로 에러 메시지를 전송한다.
				MSG_BYTE msg ;

				msg.Category	= MP_RECALLNPC ;
				msg.Protocol	= MP_RECALLNPC_MTOC_RECALL_NACK ;

				msg.dwObjectID	= dwIndex ;

				msg.bData		= e_RNET_FAILED_RECEIVE_REMAINNPC  ;

				pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;		

				// 서버에 에러 박스나, 로그를 남기고 return 한다.
				Throw_Error("Failed to receive remain npc!!", "CNpcRecallMgr::Cheat_CTOA_Recall_Syn" ) ;
				return ;
			}

			Recall_Npc( pNpc ) ;
		}
		break ;

	default : break ;
	}
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

	wTotalCount = WORD( wTotalCount + m_mRSelf_Npc.size() );

	wTotalCount = WORD( wTotalCount + m_mRTime_Npc.size() );

	wTotalCount = WORD( wTotalCount + m_mRMove_Npc.size() );

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


//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ActiveSelfNpc
//	DESC		: 스스로 소환/소멸되는 npc 정보를 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallSelf* CNpcRecallMgr::Get_ActiveSelfNpc(DWORD dwIndex , WORD wChenel) 
{
	// npc 정보를 확인한다.
	MAP_ARSELF_NPC::iterator it ;

	for( it = m_mARSelf_Npc.begin() ; it != m_mARSelf_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;
		if( it->second.Get_RecalledChenel() != wChenel ) continue ;

		return &it->second ;
	}

	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ActiveTimeNpc
//	DESC		: 일정시간 소환/소멸되는 npc 정보를 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallRemainTime* CNpcRecallMgr::Get_ActiveTimeNpc(DWORD dwIndex) 
{
	// npc 정보를 확인한다.
	MAP_ARTIME_NPC::iterator it ;

	for( it = m_mARTime_Npc.begin() ; it != m_mARTime_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;

		return &it->second ;
	}

	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ActiveMoveNpc
//	DESC		: 이동가능한 npc 정보를 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMoveable* CNpcRecallMgr::Get_ActiveMoveNpc(DWORD dwIndex) 
{
	// npc 정보를 확인한다.
	MAP_ARMOVE_NPC::iterator it ;

	for( it = m_mARMove_Npc.begin() ; it != m_mARMove_Npc.end() ; ++it )
	{
		if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;

		return &it->second ;
	}

	return NULL ;
}


CNpcRecallBase* CNpcRecallMgr::Get_ActiveRecallBase(DWORD dwIndex , WORD wchenel )
{
	{
		// npc 정보를 확인한다.
		MAP_ARSELF_NPC::iterator it ;

		for( it = m_mARSelf_Npc.begin() ; it != m_mARSelf_Npc.end() ; ++it )
		{
			if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;
			if( it->second.Get_RecalledChenel() != wchenel ) continue ;

			return &it->second ;
		}
	}

	{
		// npc 정보를 확인한다.
		MAP_ARTIME_NPC::iterator it ;

		for( it = m_mARTime_Npc.begin() ; it != m_mARTime_Npc.end() ; ++it )
		{
			if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;
			if( it->second.Get_RecalledChenel() != wchenel ) continue ;

			return &it->second ;
		}
	}


	{
		// npc 정보를 확인한다.
		MAP_ARMOVE_NPC::iterator it ;

		for( it = m_mARMove_Npc.begin() ; it != m_mARMove_Npc.end() ; ++it )
		{
			if( it->second.Get_NpcRecallIndex() != dwIndex ) continue ;
			if( it->second.Get_RecalledChenel() != wchenel ) continue ;

			return &it->second ;
		}
	}

	return NULL ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ActiveMoveNpcByCreateIdx
//	DESC		: 생성 아이디로, 소환중인 이동 npc 정보를 검색하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: September 9, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMoveable* CNpcRecallMgr::Get_ActiveMoveNpcByCreateIdx(DWORD dwIndex)
{
	// npc 정보를 확인한다.
	MAP_ARMOVE_NPC::iterator it ;

	for( it = m_mARMove_Npc.begin() ; it != m_mARMove_Npc.end() ; ++it )
	{
		if( it->second.Get_CreatedIdx() != dwIndex ) continue ;

		return &it->second ;
	}

	return NULL ;
}

// 소환 NPC 핵 채크  
BOOL CNpcRecallMgr::CheckHackRecallNpc( CPlayer* pPlayer , DWORD dwRecallIndex ) 
{
	CNpcRecallBase* pRecallBase = Get_RecallBase( dwRecallIndex ) ;

	if( ! pRecallBase || !pPlayer )
		return FALSE ;

	CNpcRecallBase* pActived = NPCRECALLMGR->Get_ActiveRecallBase( dwRecallIndex , (WORD) pPlayer->GetChannelID() ) ;

	//소환후의 체크 
	if( pActived )
	{
		//소환후 채크는 현재 소환된 위치 값과 플레이어 위치 값으로 채크를 한다.
		VECTOR3 vPos ,vRecallPos ;
		pPlayer->GetPosition( &vPos );
		pActived->Get_RecallPos( &vRecallPos ) ;

		float dist = CalcDistanceXZ( &vPos, &vRecallPos );
		if( dist > 3000.0f )
			return FALSE;
	}
	//소환전의 체크 
	else
	{
		//소환전에는 요청된 RecallNpc를 통하여 부모의 Npc정보를 알아내 검사를 한다.
		DWORD dwParentNpcIdx  = pRecallBase->Get_ParentNpcIndex() ;
		DWORD dwParentNpcType = (DWORD) pRecallBase->Get_ParentNpcType() ;

		if(	dwParentNpcType == CNpcRecallBase::eParentNpcType_Static )
		{
			//부모가 Static Npc라면 ITEMMGR->CheckHackNpc() 호출
			if( ! ITEMMGR->CheckHackNpc( pPlayer, (WORD)dwParentNpcIdx ) )
				return FALSE;
		}
		else if( dwParentNpcType == CNpcRecallBase::eParentNpcType_Recall )
		{
			//부모가 RecallNpc라면 소환되어있는 부모Npc정보를 가져와 거리체크 
			CNpcRecallBase* pActivedParentBase = NPCRECALLMGR->Get_ActiveRecallBase( dwParentNpcIdx , (WORD) pPlayer->GetChannelID() );
			if( pActivedParentBase == NULL )
				return FALSE;

			VECTOR3 vPos , vRecallPos;

			pPlayer->GetPosition( &vPos );
			pActivedParentBase->Get_RecallPos( &vRecallPos ) ;

			float dist = CalcDistanceXZ( &vPos, &vRecallPos );
			if( dist > 3000.0f )
				return FALSE;
		}
	}

	return TRUE;
}



//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::Throw_Error(char* szErr, char* szCaption)
{
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
}





//-------------------------------------------------------------------------------------------------
//	NAME		: WriteLog
//	DESC		: The function to create a error log for siege recall manager.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::WriteLog(char* pMsg)
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

void CNpcRecallMgr::Recall_Npc(CNpcRecallBase* pNpcRecallBase)
{
	// 081029 LYW --- NpcRecallMgr : npc 삭제 로그를 남긴다.
	WriteRecallNpcLog( "Call - Recall_Npc\n" ) ;

	if( pNpcRecallBase == NULL )
	{
		Throw_Error("Invalid parameter!!", "CNpcRecallMgr::Recall_Npc" ) ;
		return;
	}

	// 소환 맵인지 확인한다.
	if( pNpcRecallBase->Get_RecallMap() != g_pServerSystem->GetMapNum() ) return ;

	// Npc list 정보를 받는다.
	NPC_LIST* pNpcList = NULL ;
	pNpcList = GAMERESRCMNGR->GetNpcList(pNpcRecallBase->Get_NpcKind()) ;

	if(pNpcList == 0)
	{
		Recall_Failed( pNpcRecallBase , e_RNET_FAILED_RECEIVE_NPCLIST ) ;

		// 서버 에러메시지를 띄우거나, 로그를 남긴다.
		Throw_Error("Failed to receive npc list info!!", __FUNCTION__) ;
		return ;
	}

	// 채널 수를 받는다.
	BYTE byChannelCount ;
	if(pNpcRecallBase->Get_RecallCondition() == eNR_ItSelf)
	{
		byChannelCount = (BYTE)CHANNELSYSTEM->GetChannelCount() ;
	}
	else
	{
		byChannelCount = 1 ;
	}

	CNpcRecallBase*			pTempRecallBase = NULL ;
	CNpcRecallSelf			TempRecallSelf;
	CNpcRecallRemainTime	TempRemainTime;
	CNpcRecallMoveable		TempMoveble;

	switch( pNpcRecallBase->Get_RecallCondition() ) 
	{
		case eNR_ItSelf : 
			{
				TempRecallSelf.Copy( (CNpcRecallSelf*)pNpcRecallBase ) ;
				pTempRecallBase = &TempRecallSelf;
			}
			break;
		case eNR_RemainTime :
			{
				memcpy( &TempRemainTime , pNpcRecallBase , sizeof( CNpcRecallRemainTime ) ) ;
				pTempRecallBase = &TempRemainTime;
			}
			break;
		case eNR_Moveable :
			{
				memcpy( &TempMoveble , pNpcRecallBase , sizeof( CNpcRecallMoveable ) ) ;
				pTempRecallBase = &TempMoveble;
			}
			break;
		default:
			return;
	}

	// npc 추가 작업.
	for( BYTE byCount = 0 ; byCount < byChannelCount ; ++byCount )
	{
		// 임시 변수 선언.
		BASEOBJECT_INFO Baseinfo ;
		NPC_TOTALINFO NpcTotalInfo ;
		VECTOR3 vPos ;

		Baseinfo.dwObjectID = g_pAISystem.GeneraterMonsterID();

		pTempRecallBase->Set_CreatedIdx( Baseinfo.dwObjectID  ) ;

		if( pTempRecallBase->Get_RecallCondition() == eNR_ItSelf)
		{
			Baseinfo.BattleID = byCount + 1 ;	
		}
		else
		{
			Baseinfo.BattleID = 1 ;	
		}

		pTempRecallBase->Set_RecalledChenel( (WORD)Baseinfo.BattleID ) ;

		// NPC 속성을 설정한다.
		SafeStrCpy(Baseinfo.ObjectName, pTempRecallBase->Get_NpcName(), MAX_NPC_NAME_LENGTH+1);
		NpcTotalInfo.Group			= 0 ;
		NpcTotalInfo.MapNum			= g_pServerSystem->GetMapNum() ;
		NpcTotalInfo.NpcJob			= pNpcList->JobKind ;
		NpcTotalInfo.NpcKind		= pNpcList->NpcKind ;
		NpcTotalInfo.dwRecallNpcIdx	= pTempRecallBase->Get_NpcRecallIndex() ;
		NpcTotalInfo.NpcUniqueIdx	= (WORD)pTempRecallBase->Get_NpcIndex() ;

		// NPC 위치를 설정한다.
		vPos.x = pTempRecallBase->Get_RecallposX() ;
		vPos.y = 0.0f ;
		vPos.z = pTempRecallBase->Get_RecallposZ() ;

		// NPC를 추가한다.
		CNpc* pNpc = NULL ;
		pNpc = g_pServerSystem->AddNpc(&Baseinfo, &NpcTotalInfo, &vPos) ;

		if( !pNpc )
		{
			Recall_Failed( pTempRecallBase , e_RNET_FAILED_CREATE_NPC );
		
			// 서버 에러메시지를 띄우거나, 로그를 남긴다.
			Throw_Error("Failed to create npc!!", __FUNCTION__) ;
			return ;
		}

		// 자동소멸되지 않도록 설정한다.
		if(pNpc) pNpc->SetDieTime(0) ;

		// 유저 테이블에 npc를 추가한다.
		//g_pUserTable->AddUser( pNpc, pNpc->GetID() ) ;
		Recall_success( pTempRecallBase ) ;
	}

	// 081029 LYW --- NpcRecallMgr : npc 삭제 로그를 남긴다.
	WriteRecallNpcLog( "End - Recall_Npc\n" ) ;
}

void CNpcRecallMgr::Recall_success( CNpcRecallBase* pNpcRecallBase )
{
	if( pNpcRecallBase == NULL )
	{
		Throw_Error("Invalid parameter!!", "CNpcRecallMgr::Recall_success" ) ;
		return;
	}

	// npc 타입을 확인한다.
	switch( pNpcRecallBase->Get_RecallCondition() )
	{
		case eNR_ItSelf :
		{
			// 동일한 npc가 존재하는지 검사한다.
			MAP_ARSELF_NPC::iterator it ;
			it = m_mARSelf_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it != m_mARSelf_Npc.end() )
			{
				Throw_Error("Clon npc!!", "CNpcRecallMgr::Recall_success" ) ;
				return ;
			}

			// ncp 정보를 복사, 생성 아이디를 세팅한다.
			CNpcRecallSelf newNpc ;
			newNpc.Copy( (CNpcRecallSelf*)pNpcRecallBase ) ; 

			if( pNpcRecallBase->Get_RecallTime() == 0 )
				newNpc.Set_RecallTime( GetTickCount() ) ;

			// npc를 활성화 중인 npc를 담는 컨테이너에 추가한다.
			m_mARSelf_Npc.insert( std::make_pair( pNpcRecallBase->Get_CreatedIdx() , newNpc ) ) ;

			// 081028 LYW --- NpcRecallMgr : npc 소환 정보를 남긴다.
			char szLog[ 1024 ] = {0, } ;
			sprintf( szLog, "Recalled Npc - %u %u %d %d", newNpc.Get_CreatedIdx(), newNpc.Get_NpcRecallIndex() , 
				newNpc.Get_RecallMap() , newNpc.Get_RecalledChenel() ) ;
			WriteRecallNpcLog( szLog ) ;
		}
		break ;

		case eNR_RemainTime :
		{
			// 동일한 npc가 존재하는지 검사한다.
			MAP_ARTIME_NPC::iterator it ;
			it = m_mARTime_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it != m_mARTime_Npc.end() )
			{
				Throw_Error("Clon npc!!", __FUNCTION__) ;
				return ;
			}

			// ncp 정보를 복사, 생성 아이디를 세팅한다.
			CNpcRecallRemainTime newNpc ;
			memcpy(&newNpc, pNpcRecallBase, sizeof(CNpcRecallRemainTime)) ;

			if( pNpcRecallBase->Get_RecallTime() == 0 )
				newNpc.Set_RecallTime( GetTickCount() ) ;

			// npc를 활성화 중인 npc를 담는 컨테이너에 추가한다.
			m_mARTime_Npc.insert( std::make_pair( pNpcRecallBase->Get_CreatedIdx() , newNpc ) ) ;

			// DB에 npc 정보를 업데이트 한다.
			DWORD dwRecallidx	= newNpc.Get_NpcRecallIndex() ;
			DWORD dwNpcIdx		= newNpc.Get_NpcIndex() ;
			DWORD dwRecallMap	= newNpc.Get_RecallMap() ;
			DWORD dwRemainTime	= newNpc.Get_RemainTime() ;
			DWORD dwChenel		= newNpc.Get_RecalledChenel() ;

			char txt[256] = {0, } ;
			sprintf( txt, "EXEC %s %u, %u, %u, %u, %u", MP_NPCREMAINTIME_INSERT, dwRecallidx, 
				dwNpcIdx, dwRecallMap, dwChenel , dwRemainTime ) ;

			g_DB.Query(  eQueryType_FreeQuery, eNpcRemainTime_Insert, 0 , txt ) ;

			// 081028 LYW --- NpcRecallMgr : npc 소환 정보를 남긴다.
			char szLog[ 1024 ] = {0, } ;
			sprintf( szLog, "Recalled Npc - %u %u %d %d", newNpc.Get_CreatedIdx(), newNpc.Get_NpcRecallIndex() , 
				newNpc.Get_RecallMap() , newNpc.Get_RecalledChenel() ) ;
		}
		break ;

		case eNR_Moveable :
		{
			// 동일한 npc가 존재하는지 검사한다.
			MAP_ARMOVE_NPC::iterator it ;
			it = m_mARMove_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it != m_mARMove_Npc.end() )
			{
				Throw_Error("Clon npc!!", __FUNCTION__) ;
				return ;
			}

			// ncp 정보를 복사, 생성 아이디를 세팅한다.
			CNpcRecallMoveable newNpc ;
			memcpy(&newNpc, pNpcRecallBase, sizeof(CNpcRecallMoveable)) ;

			if( newNpc.Get_RecallTime() == 0 )
				newNpc.Set_RecallTime( GetTickCount() ) ;

			// npc를 활성화 중인 npc를 담는 컨테이너에 추가한다.
			m_mARMove_Npc.insert( std::make_pair( pNpcRecallBase->Get_CreatedIdx() , newNpc ) ) ;

			// DB에 npc 정보를 업데이트 한다.
			DWORD dwRecallidx	= newNpc.Get_NpcRecallIndex() ;
			DWORD dwNpcIdx		= newNpc.Get_NpcIndex() ;
			DWORD dwRecallMap	= newNpc.Get_RecallMap() ;
			DWORD dwRemainTime	= newNpc.Get_RemainTime() ;
			DWORD dwChenel		= newNpc.Get_RecalledChenel() ;

			char txt[256] = {0, } ;
			sprintf( txt, "EXEC %s %u, %u, %u, %u, %u", MP_NPCREMAINTIME_INSERT, dwRecallidx, 
				dwNpcIdx, dwRecallMap, dwChenel , dwRemainTime ) ;

			g_DB.Query(  eQueryType_FreeQuery, eNpcRemainTime_Insert, 0 , txt ) ;

			// 081028 LYW --- NpcRecallMgr : npc 소환 정보를 남긴다.
			char szLog[ 1024 ] = {0, } ;
			sprintf( szLog, "Recalled Npc - %u %u %d %d", newNpc.Get_CreatedIdx(), newNpc.Get_NpcRecallIndex() , 
				newNpc.Get_RecallMap() , newNpc.Get_RecalledChenel() ) ;
		}
		break ;

	default : break ;
	}



}

void CNpcRecallMgr::Recall_Failed( CNpcRecallBase* pNpcRecallBase , BYTE byError )
{
	// 에러 메시지를 확인한다.
	switch( byError )
	{
	case e_RNET_FAILED_RECEIVE_STATICNPCINFO : break ;
	case e_RNET_FAILED_CREATE_NPC : break ;

	default : break ;
	}
}

void CNpcRecallMgr::Delete_Npc( CNpcRecallBase* pNpcRecallBase )
{
	// 081029 LYW --- NpcRecallMgr : npc 삭제 로그를 남긴다.
	WriteRecallNpcLog( "Call - Delete_Npc\n" ) ;

	// 함수 인자 확인.
	if( !pNpcRecallBase )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// 소환 맵인지 확인한다.
	if( pNpcRecallBase->Get_RecallMap() != g_pServerSystem->GetMapNum() ) return ;

	// Npc 삭제 처리를 한다.
	CObject* pObject = NULL ;
	pObject = g_pUserTable->FindUser( pNpcRecallBase->Get_CreatedIdx() ) ;

	// 081029 LYW --- NpcRecallMgr : 소환되는 npc 아이디를 남긴다.
	char szNpcID[256] = {0, } ;
	sprintf(szNpcID, "Delete Npc Id : %u", pNpcRecallBase->Get_CreatedIdx() ) ;
	WriteRecallNpcLog( szNpcID ) ;

	if(!pObject)
	{
		Delete_Failed( pNpcRecallBase , e_RNET_FAILED_RECEIVE_NPC_FROM_USERTABLE ) ;

		Throw_Error("Failed to find npc!!", __FUNCTION__) ;
		return ;
	}

	//CCharMove::ReleaseMove(pObject) ;

	g_pServerSystem->RemoveNpc( pNpcRecallBase->Get_CreatedIdx() ) ;

	Delete_Success(  pNpcRecallBase ) ;

	WriteRecallNpcLog( "End - Delete_Npc\n" ) ;
	//g_pObjectFactory->ReleaseObject(pObject) ;
	//g_pUserTable->RemoveUser(pmsg->dwData) ;
}

void CNpcRecallMgr::Delete_Success( CNpcRecallBase* pNpcRecallBase )
{
	// 함수 인자 확인.
	if( !pNpcRecallBase )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	BYTE byRecallCondition = pNpcRecallBase->Get_RecallCondition();

	// npc 타입을 확인한다.
	switch( byRecallCondition )
	{
	case eNR_ItSelf :
		{
			// 동일한 npc가 존재하는지 검사한다.
			MAP_ARSELF_NPC::iterator it ;
			it = m_mARSelf_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it == m_mARSelf_Npc.end() )
			{
				Throw_Error("Can't find active npc!!", __FUNCTION__) ;
				return ;
			}

			// 081028 LYW --- NpcRecallMgr : npc 소환 정보를 남긴다.
			char szLog[ 1024 ] = {0, } ;
			sprintf( szLog, "Delete Npc - %u %u %d %d", pNpcRecallBase->Get_CreatedIdx(), pNpcRecallBase->Get_NpcRecallIndex() , 
				pNpcRecallBase->Get_RecallMap() , pNpcRecallBase->Get_RecalledChenel() ) ;
			WriteRecallNpcLog( szLog ) ;

			m_mARSelf_Npc.erase( it ) ;
		}
		break ;

	case eNR_RemainTime :
		{
			// 동일한 npc가 존재하는지 검사한다.
			MAP_ARTIME_NPC::iterator it ;
			it = m_mARTime_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it == m_mARTime_Npc.end() )
			{
				Throw_Error("Can't find active npc!!", __FUNCTION__) ;
				return ;
			}

			// DB에서 남은시간 정보를 삭제한다.
			char txt[256] = {0, } ;
			sprintf(txt, "EXEC %s %u, %d", MP_NPCREMAINTIME_DELETE,
				it->second.Get_NpcRecallIndex(), it->second.Get_RecalledChenel() ) ;

			g_DB.Query( eQueryType_FreeQuery, eNpcRemainTime_Delete, 0, txt) ;

			// 081028 LYW --- NpcRecallMgr : npc 소환 정보를 남긴다.
			char szLog[ 1024 ] = {0, } ;
			sprintf( szLog, "Delete Npc - %u %u %d %d", pNpcRecallBase->Get_CreatedIdx(), pNpcRecallBase->Get_NpcRecallIndex() , 
				pNpcRecallBase->Get_RecallMap() , pNpcRecallBase->Get_RecalledChenel() ) ;
			WriteRecallNpcLog( szLog ) ;

			m_mARTime_Npc.erase( it ) ;
		}
		break ;

	case eNR_Moveable :
		{
			// 동일한 npc가 존재하는지 검사한다.
			MAP_ARMOVE_NPC::iterator it ;
			it = m_mARMove_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it == m_mARMove_Npc.end() )
			{
				Throw_Error("Can't find active npc!!", __FUNCTION__) ;
				return ;
			}

			// DB에서 남은시간 정보를 삭제한다.
			char txt[256] = {0, } ;
			sprintf(txt, "EXEC %s %u, %d", MP_NPCREMAINTIME_DELETE,
				it->second.Get_NpcRecallIndex(), it->second.Get_RecalledChenel() ) ;

			g_DB.Query( eQueryType_FreeQuery, eNpcRemainTime_Delete, 0, txt) ;

			// 081028 LYW --- NpcRecallMgr : npc 소환 정보를 남긴다.
			char szLog[ 1024 ] = {0, } ;
			sprintf( szLog, "Delete Npc - %u %u %d %d", pNpcRecallBase->Get_CreatedIdx(), pNpcRecallBase->Get_NpcRecallIndex() , 
				pNpcRecallBase->Get_RecallMap() , pNpcRecallBase->Get_RecalledChenel() ) ;
			WriteRecallNpcLog( szLog ) ;

			m_mARMove_Npc.erase( it ) ;
		}
		break ;

	default : 
		{
			Throw_Error("Can't find npc type!!", __FUNCTION__) ;
			return ;
		}
		break ;
	}

}

void CNpcRecallMgr::Delete_Failed( CNpcRecallBase* pNpcRecallBase , BYTE byError ) 
{
	// 함수 인자 확인.
	if( !pNpcRecallBase )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// 에러 메시지를 확인한다.
	switch( byError )
	{
	case e_RNET_FAILED_RECEIVE_NPC_FROM_USERTABLE : break ;

	default : break ;
	}
}

void CNpcRecallMgr::Move_Npc( CNpcRecallBase* pNpcRecallBase )
{
	// 함수 인자 확인.
	if( !pNpcRecallBase )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// 소환 맵인지 확인한다.
	if( pNpcRecallBase->Get_RecallMap() != g_pServerSystem->GetMapNum() ) return ;


	// Npc 이동 처리를 한다.
	CObject* pObject = NULL ;
	pObject = g_pUserTable->FindUser( pNpcRecallBase->Get_CreatedIdx() ) ;

	if(!pObject)
	{
		Move_Npc_Failed( pNpcRecallBase , e_RNET_FAILED_RECEIVE_NPC_FROM_USERTABLE ) ;
		
		Throw_Error("Failed to find npc!!", __FUNCTION__) ;
		return ;
	}

	VECTOR3 vec ;

	vec.x = ((CNpcRecallMoveable*)pNpcRecallBase)->Get_MovePosX() ;
	vec.y = 0 ;
	vec.z = ((CNpcRecallMoveable*)pNpcRecallBase)->Get_MovePosZ();

	CCharMove::Warp(pObject, &vec) ;

	//CCharMove::SetPosition(pObject, &vec) ;
	//CCharMove::InitMove(pObject, &vec) ;

	Move_Npc_Success( pNpcRecallBase ) ; 

	// 081028 LYW --- NpcRecallMgr : npc 소환 정보를 남긴다.
	char szLog[ 1024 ] = {0, } ;
	sprintf( szLog, "Move Npc - %u %u", g_pServerSystem->GetMapNum() , pNpcRecallBase->Get_NpcRecallIndex() ) ;
	WriteRecallNpcLog( szLog ) ;
}

void CNpcRecallMgr::Move_Npc_Success( CNpcRecallBase* pNpcRecallBase ) 
{
	// 함수 인자 확인.
	if( !pNpcRecallBase )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	BYTE byRecallCondition = pNpcRecallBase->Get_RecallCondition() ;
	// npc 타입을 확인한다.
	switch( byRecallCondition )
	{
	case eNR_ItSelf :
		{
			Throw_Error("invalid move type!!", __FUNCTION__) ;
			return ;
		}
		break ;

	case eNR_RemainTime :
		{
			Throw_Error("invalid move type!!", __FUNCTION__) ;
			return ;
		}
		break ;

	case eNR_Moveable :
		{
			// 동일한 npc가 존재하는지 검사한다.
			MAP_ARMOVE_NPC::iterator it ;
			it = m_mARMove_Npc.find( pNpcRecallBase->Get_CreatedIdx() ) ;

			if( it == m_mARMove_Npc.end() )
			{
				Throw_Error("Can't find active npc!!", __FUNCTION__) ;
				return ;
			}

			it->second.Set_RecallPosX( it->second.Get_MovePosX() ) ;
			it->second.Set_RecallPosZ( it->second.Get_MovePosZ() ) ;
		}
		break ;

	default : 
		{
			Throw_Error("Can't find npc type!!", __FUNCTION__) ;
			return ;
		}
		break ;
	}
}

void CNpcRecallMgr::Move_Npc_Failed( CNpcRecallBase* pNpcRecallBase , BYTE byError ) 
{
	// 함수 인자 확인.
	if( !pNpcRecallBase )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// 에러 메시지를 확인한다.
	switch( byError )
	{
	case e_RNET_FAILED_RECEIVE_NPC_FROM_USERTABLE : break ;

	default : break ;
	}
}

void CNpcRecallMgr::CTOAM_ChangeMap_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) 
{
	// 함수 인자 확인.
	if( !pMsg )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );								

	if( !pPlayer)
	{
		Throw_Error("Invalid Player ID !!", __FUNCTION__) ;
		return ;
	}

	// 활성화 된(소환된)npc 정보가 유효한지 체크한다.
	CNpcRecallMoveable* pMoveNpc = NULL ;
	pMoveNpc = Get_ActiveMoveNpc(pmsg->dwData) ;

	if( !pMoveNpc )
	{
		// 클라이언트로 에러 메시지 전송.
		MSG_BYTE msg ;

		msg.Category	= MP_RECALLNPC ;
		msg.Protocol	= MP_RECALLNPC_MTOC_CHANGEMAP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= e_RNET_FAILED_RECEIVE_NPCINFO ;

		pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

		// 서버에 에러 메시지 출력 처리.
		Throw_Error("Failed to receive npc info!!", __FUNCTION__) ;
		return ;
	}

	if( ! CheckHackRecallNpc( pPlayer , pmsg->dwData ) )
	{
		// 클라이언트로 에러 메시지 전송.
		MSG_BYTE msg ;

		msg.Category	= MP_RECALLNPC ;
		msg.Protocol	= MP_RECALLNPC_MTOC_CHANGEMAP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= e_RNET_FAILED_CHECK_HACK_RECALLNPC_CHECK ;

		pPlayer->SendMsg(&msg, sizeof(MSG_BYTE)) ;

		// 서버에 에러 메시지 출력 처리.
		Throw_Error("Failed to CheckHackRecallNpc !!", __FUNCTION__) ;
		return ;
	}

	MSG_DWORD3 msg ;
	msg.Category		= MP_RECALLNPC ;
	msg.Protocol		= MP_RECALLNPC_MTOA_CHANGEMAP_SYN ;

	msg.dwObjectID		= pmsg->dwObjectID ;
	msg.dwData1			= pMoveNpc->Get_RecallMap() ;
	msg.dwData2			= pMoveNpc->Get_ChangeMapNum() ;
	msg.dwData3			= pmsg->dwData;

	g_Network.Send2AgentServer((char*)&msg, sizeof(MSG_DWORD3));
}

//-------------------------------------------------------------------------------------------------
//	NAME		: ATOM_ChangeMap_Ack
//	DESC		: 이동 가능한 npc를 통해, 맵 이동 요청이 들어왔을 때 처리하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMgr::ATOM_ChangeMap_Ack(DWORD dwIndex, void* pMsg, DWORD dwLength) 
{
	// 함수 인자 확인.
	if( !pMsg )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	if( !pmsg )
	{
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}

	CNpcRecallMoveable* pMoveNpc = NULL ;
	pMoveNpc = Get_ActiveMoveNpc(pmsg->dwData) ;

	if( !pMoveNpc) 
	{
		Throw_Error("ATOM_ChangeMap_Ack !pMoveNpc", __FUNCTION__) ;
		return ;
	}
			
	MAPTYPE ChangeMapNum	= pMoveNpc->Get_ChangeMapNum() ;			
	BYTE    ChannelNum		= 0 ;							
	float	fChangeMapXPos	= pMoveNpc->Get_ChangeMapXPos() ;									
	float	fChangeMapZPos	= pMoveNpc->Get_ChangeMapZPos() ;					


	// 하위에서 계속 사용 될 맵 번호를 받아둔다.
	WORD wCurMapNum = g_pServerSystem->GetMapNum() ;


	// 맵 인덱스를 확인한다.
	if( pMoveNpc->Get_RecallMap() != wCurMapNum ) return ;


	// Player 정보를 받는다.
	CPlayer* pPlayer = NULL ;
	pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID) ;

	if(pPlayer == NULL)
	{
		// 서버에 에러 메시지 출력 처리를 한다.
		Throw_Error("Failed to receive player info!!", __FUNCTION__) ;
		return ;
	}
	// 090701 LUJ, 맵 이동 시 플레이어 정보를 DB에 쓰는데, 초기화가 끝나지 않았을 경우 쓰레기값이
	//		저장될 수 있다. 초기화가 끝난 객체만 처리한다
	else if( FALSE == pPlayer->GetInited() )
	{
		return;
	}

	// 이동할 맵이 공성 지역이라면 공성중인지 채크를 하자 
	if( SIEGEWARFAREMGR->IsSiegeWarfareZone( ChangeMapNum , FALSE ) )
	{
		// 공성전이지 않을 경우에는, 실패 처리를 한다.
		if( SIEGEWARFAREMGR->GetState() <= eSiegeWarfare_State_Before )
		{
			// 클라이언트로 에러 메시지를 보낸다.
			MSG_BYTE msg ;

			msg.Category	= MP_RECALLNPC ;
			msg.Protocol	= MP_RECALLNPC_MTOA_CHANGEMAP_NACK ;

			msg.dwObjectID	= pmsg->dwObjectID ;

			msg.bData		= e_RNET_FAILED_NOT_SIEGEWARTIME ;

			pPlayer->SendMsg( &msg, sizeof(MSG_BYTE) ) ;

			return ;
		}
	}

	
	// 오토노트/ PK모드 / 루팅 / 다이 / 데이트 존 체크.
	if( pPlayer->GetAutoNoteIdx() || pPlayer->IsPKMode() ||
		LOOTINGMGR->IsLootedPlayer( pPlayer->GetID() ) ||
		pPlayer->GetState() == eObjectState_Die //&& 
		//g_csDateManager.IsChallengeZoneHere() == FALSE) 
		)
	{
		// 클라이언트로 에러 메시지를 보낸다.
		MSG_BYTE msg ;

		msg.Category	= MP_RECALLNPC ;
		msg.Protocol	= MP_RECALLNPC_MTOA_CHANGEMAP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= e_RNET_FAILED_CHANGEMAP_INVALID_STATE ;

		pPlayer->SendMsg( &msg, sizeof(MSG_BYTE) ) ;

		// 서버에 에러 메시지 출력 처리를 한다.
		Throw_Error("Invalid player state!!", __FUNCTION__) ;
		return ;
	}


	// 펫 / 퀵슬롯 정보등을 db에 업데이트 한다.
	pPlayer->UpdateLogoutToDB(FALSE) ;


	// Player의 맵 이동 정보를 세팅한다.
	pPlayer->SetMapMoveInfo( ChangeMapNum, (DWORD)fChangeMapXPos, (DWORD)fChangeMapZPos ) ;

	// 081218 LUJ, 해제하기 전에 값을 복사하자
	const DWORD		playerIndex		= pPlayer->GetID();
	const DWORD		userIndex		= pPlayer->GetUserID();
	const MONEYTYPE	inventoryMoney	= pPlayer->GetMoney( eItemTable_Inventory );
	const MONEYTYPE	storageMoney	= pPlayer->GetMoney( eItemTable_Storage );

	g_pServerSystem->RemovePlayer( playerIndex );

	// 081218 LUJ, 업데이트보다 맵 이동이 빨리 진행될 경우 업데이트되지 않은 정보가 오게된다.
	//			이를 막기 위해 프로시저 처리 완료 후 진행한다
	UpdatePlayerOnMapOut(
		playerIndex,
		userIndex,
		inventoryMoney,
		storageMoney,
		ChangeMapNum,
		WORD( ChannelNum ) );
}













