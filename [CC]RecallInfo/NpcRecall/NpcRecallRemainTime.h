#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallRemainTime.h
//	DESC		: 한번 소환 된 후, 일정 시간이 지나면, 스스로 소멸되는 npc 클래스.
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
//		The class CNpcRecallRemainTime.
//-------------------------------------------------------------------------------------------------
class CNpcRecallRemainTime : public CNpcRecallBase
{
	DWORD	m_dwRemainTime ;								// 소환이 유지 될 시간.
	
public:
	CNpcRecallRemainTime(void);
	virtual ~CNpcRecallRemainTime(void);
	// 소환이 유지 될 시간을 설정/반환 하는 함수.
	void Set_RemainTime(DWORD dwRemainTime) { m_dwRemainTime = dwRemainTime ; }
	DWORD Get_RemainTime() { return m_dwRemainTime ; }


	// 소환이 유지 될 시간이 지났는지 반환하는 함수.
	BYTE IsEndRemainTime(DWORD dwCurTime) ;


	// 소환이 유지 될 시간을 초기화 하는 함수.
	void ClearRemainTime() { m_dwRemainTime = 0 ; }
};
