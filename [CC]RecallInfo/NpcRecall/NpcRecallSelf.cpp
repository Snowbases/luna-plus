//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallSelf.cpp
//	DESC		: Implementation part of CNpcRecallSelf class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "./NpcRecallSelf.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallSelf
//	DESC		: 생성자 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallSelf::CNpcRecallSelf(void)
{
	Set_ParentNpcType( eParentNpcType_OnlySelf ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CNpcRecallSelf
//	DESC		: 소멸자 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
CNpcRecallSelf::~CNpcRecallSelf(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Insert_TimeTable
//	DESC		: Npc 소환/소멸 시간을 담고 있는 정보 추가 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
void CNpcRecallSelf::Insert_TimeTable(st_RECALL_TIMETABLE* pInfo)
{
	// 인자로 넘어온 정보 유효 확인.
	if( !pInfo )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// 동일 인덱스 테이블이 있는지 확인한다.
	MAP_TIMETABLE::iterator it ;
	for( it = m_mTimeTable.begin() ; it != m_mTimeTable.end() ; ++it )
	{
		if( it->second.byTableIdx == pInfo->byTableIdx )
		{
			Throw_Error("Failed to insert time table!!\\[Same time table index]", __FUNCTION__) ;
			return ;
		}
	}


	// 테이블을 추가한다.
	m_mTimeTable.insert( std::make_pair( pInfo->byTableIdx, *pInfo ) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_TimeTable
//	DESC		: Npc 소환/소멸 시간을 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------
st_RECALL_TIMETABLE* CNpcRecallSelf::Get_TimeTable(BYTE byIdx)
{
	// 일치하는 테이블이 있는지 확인한다.
	MAP_TIMETABLE::iterator it ;
	for( it = m_mTimeTable.begin() ; it != m_mTimeTable.end() ; ++it )
	{
		if( it->second.byTableIdx == byIdx )
		{
			return &it->second ;
		}
	}


	// null을 return.
	return NULL ;
}

void CNpcRecallSelf::Copy(CNpcRecallSelf* pSrc)
{
	memcpy(this , pSrc , sizeof( CNpcRecallBase )  ) ;

	MAP_TIMETABLE::iterator it ;
	for( it = pSrc->m_mTimeTable.begin() ; it != pSrc->m_mTimeTable.end() ; ++it )
	{
		this->m_mTimeTable.insert( std::make_pair( it->second.byTableIdx, it->second ) ) ;
	}
}















