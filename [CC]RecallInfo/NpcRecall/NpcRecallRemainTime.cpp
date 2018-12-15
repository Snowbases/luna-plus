//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallRemainTime.cpp
//	DESC		: Implementation part of CNpcRecallRemainTime class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "./NpcRecallRemainTime.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallRemainTime
//	DESC		: 생성자 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallRemainTime::CNpcRecallRemainTime(void)
{
	// 멤버 변수를 초기화 한다.
	m_dwRemainTime	= 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CNpcRecallRemainTime
//	DESC		: 소멸자 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallRemainTime::~CNpcRecallRemainTime(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: IsEndRemainTime
//	DESC		: 소환이 유지 될 시간이 지났는지 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
BYTE CNpcRecallRemainTime::IsEndRemainTime(DWORD dwCurTime)
{
	// 소환 유지 시간이 지났는지 확인한다.
	DWORD dwCheckTime = dwCurTime - Get_RecallTime() ;

	if( dwCheckTime > m_dwRemainTime ) return TRUE ;
	else return FALSE ;

	// 1분 주기로 Remain Time을 DB에 업데이트 할까?
}















