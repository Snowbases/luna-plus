//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMoveable.cpp
//	DESC		: Implementation part of CNpcRecallMoveable class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "./NpcRecallMoveable.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMoveable
//	DESC		: 생성자 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMoveable::CNpcRecallMoveable(void)
{
	// 멤버 함수를 초기화 한다.
	m_dwRemainTime		= 0 ;

	m_dwMoveTime		= 0 ;

	m_fMoveXpos			= 0.0f ;
	m_fMoveZpos			= 0.0f ;

	m_ChangeMapNum		= 0 ;

	m_fChangeMapXPos	= 0.0f ;
	m_fChangeMapZPos	= 0.0f ;

	m_byMoved			= FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CNpcRecallMoveable
//	DESC		: 소멸자 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallMoveable::~CNpcRecallMoveable(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_RemainTime
//	DESC		: Npc의 소환이 유지 될 시간을 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_RemainTime(DWORD dwRemainTime)
{
	// 이동 할 시간보다 작은 수치가 들어오는지 확인한다.
	// 081029 LYW --- NpcRecallMoveable : 공성 게이트 소환되지 않는 버그 수정.
	//if( dwRemainTime <= m_dwMoveTime )
	//{
	//	Throw_Error("Invalid remain time!!", __FUNCTION__) ;
	//	return ;
	//}


	// 소환 유지 시간을 설정한다.
	m_dwRemainTime = dwRemainTime ; 
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_MoveTime
//	DESC		: Npc가 이동할 X/Z좌표를 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_MoveTime(DWORD dwMoveTime)
{
	// 소환 유지 시간 보다 큰 수치가 들어오는지 확인한다.
	if( dwMoveTime == 0 || dwMoveTime >= m_dwRemainTime )
	{
		Throw_Error("Invalid move time!!", __FUNCTION__) ;
		return ;
	}


	// 이동 할 시간을 설정한다.
	m_dwMoveTime = dwMoveTime ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_MoveposX
//	DESC		: Npc가 이동할 X좌표를 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_MoveposX(float fXpos)
{
	// 이동할 좌표의 유효성을 확인한다.
	if( fXpos <= 0.0f )
	{
		Throw_Error("Invalid move position!!", __FUNCTION__) ;
		return ;
	}


	// 이동 할 좌표를 설정한다.
	m_fMoveXpos = fXpos ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_MoveposZ
//	DESC		: Npc가 이동할 Z좌표를 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_MoveposZ(float fZpos)
{
	// 이동할 좌표의 유효성을 확인한다.
	if( fZpos <= 0.0f )
	{
		Throw_Error("Invalid move position!!", __FUNCTION__) ;
		return ;
	}


	// 이동 할 좌표를 설정한다.
	m_fMoveZpos = fZpos ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: IsMoveable
//	DESC		: 이동이 가능 한 상태인지 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
BYTE CNpcRecallMoveable::IsMoveable(DWORD dwCurTime)
{
	// 이동이 가능한 시간이 지났는지 확인한다.
	DWORD dwCheckTime = dwCurTime - Get_RecallTime() ;


	// 결과를 return 한다.
	if(dwCheckTime >= m_dwMoveTime) return TRUE ;
	else return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: IsEndRemainTime
//	DESC		: 소환 유지 시간이 지났는지 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
BYTE CNpcRecallMoveable::IsEndRemainTime(DWORD dwCurTime) 
{
	// 소환 유지 시간이 지났는지 확인한다.
	DWORD dwCheckTime = dwCurTime - Get_RecallTime() ;


	// 결과를 return 한다.
	if( dwCheckTime >= m_dwRemainTime ) return TRUE ;
	else return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_ChangeMapNum
//	DESC		: npc를 통해 이동할 맵의 맵 번호를 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_ChangeMapNum(MAPTYPE mapNum) 
{
	// 인자로 넘어오는 맵 번호를 확인한다.
	if( mapNum == 0 )
	{
		Throw_Error("Invalid map number!!", __FUNCTION__) ;
		return ;
	}


	// 소환 할 맵 번호를 설정한다.
	m_ChangeMapNum = mapNum ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_ChangeMapXPos
//	DESC		: npc를 통해 이동할 맵의 x좌표를 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_ChangeMapXPos(float fXpos) 
{
	// 인자로 넙어오는 소환 좌표를 확인한다.
	if( fXpos <= 0.0f )
	{
		Throw_Error("Invalid recall position!!", __FUNCTION__) ;
		return ;
	}


	// 소환 할 좌표를 설정한다.
	m_fChangeMapXPos = fXpos ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_ChangeMapZPos
//	DESC		: npc를 통해 이동할 맵의 z좌표를 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 21, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallMoveable::Set_ChangeMapZPos(float fZpos) 
{
	// 인자로 넙어오는 소환 좌표를 확인한다.
	if( fZpos <= 0.0f )
	{
		Throw_Error("Invalid recall position!!", __FUNCTION__) ;
		return ;
	}


	// 소환 할 좌표를 설정한다.
	m_fChangeMapZPos = fZpos ;
}













