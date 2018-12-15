#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMoveable.h
//	DESC		: 소환 된 후, 일정 시간이 지나면, 동일 맵 내에서 주어진 좌표로 이동하는 npc 클래스.
//				: 1회 이동이 가능하며, 현재는 공성맵에서 워프 게이트로 이용한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "./NpcRecallBase.h"





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		The class CNpcRecallMoveable.
//-------------------------------------------------------------------------------------------------
class CNpcRecallMoveable : public CNpcRecallBase
{
	DWORD	m_dwRemainTime ;									// Npc의 소환 유지 시간.

	DWORD	m_dwMoveTime ;										// Npc가 이동을 시작 할 시간.

	float	m_fMoveXpos ;										// 이동 할 X 좌표.
	float	m_fMoveZpos ;										// 이동 할 Z 좌표.

	MAPTYPE m_ChangeMapNum ;									// 이동할 맵 번호.

	float	m_fChangeMapXPos ;									// 이동할 맵의 X좌표.
	float	m_fChangeMapZPos ;									// 이동할 맵의 Z좌표.

	BYTE	m_byMoved ;											// 이동을 했는지 여부를 담을 변수.

public:
	CNpcRecallMoveable(void);
	virtual ~CNpcRecallMoveable(void);
	// Npc의 소환이 유지 될 시간을 설정/반환 할 함수.
	void	Set_RemainTime(DWORD dwRemainTime) ;
	DWORD	Get_RemainTime() { return m_dwRemainTime ; }


	// Npc가 이동을 할 시간을 설정/반환 할 함수.
	void	Set_MoveTime(DWORD dwMoveTime) ;
	DWORD	Get_MoveTime() { return m_dwMoveTime ; }


	// Npc가 이동할 X/Z좌표를 설정/반환 할 함수.
	void	Set_MoveposX(float fXpos) ;
	void	Set_MoveposZ(float fZpos) ;
	float	Get_MovePosX() { return m_fMoveXpos ; }
	float	Get_MovePosZ() { return m_fMoveZpos ; }


	// 이동이 가능 한 상태인지 반환하는 함수.
	BYTE	IsMoveable(DWORD dwCurTime) ;


	// 소환 유지 시간이 지났는지 반환하는 함수.
	BYTE	IsEndRemainTime(DWORD dwCurTime) ;


	// npc를 통해 이동할 맵의 맵 번호를 설정/반환하는 함수.
	void	Set_ChangeMapNum(MAPTYPE mapNum) ;
	MAPTYPE Get_ChangeMapNum() { return m_ChangeMapNum ; }


	// npc를 통해 이동할 맵의 X/Z좌표를 설정/반환하는 함수.
	void	Set_ChangeMapXPos(float fXpos) ;
	void	Set_ChangeMapZPos(float fZpos) ;
	float	Get_ChangeMapXPos() { return m_fChangeMapXPos ; }
	float	Get_ChangeMapZPos() { return m_fChangeMapZPos ; }


	// npc가 이동을 했는지, 아닌지 여부를 설정/반환하는 함수.
	void	Set_Moved(BYTE byMoved) { m_byMoved = byMoved ; }
	BYTE	Is_Moved() { return m_byMoved ; }


	// 소환이 유지 될 시간을 초기화 하는 함수.
	void ClearRemainTime() { m_dwRemainTime = 0 ; }
};
