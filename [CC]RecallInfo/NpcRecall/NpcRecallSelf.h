#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallSelf.h
//	DESC		: 자체 시간 테이블을 가지고 있어, 스스로 소환 / 소멸을 하는 npc 클래스.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include <map>

#include "./NpcRecallBase.h"





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
struct st_RECALL_TIMETABLE								// Npc의 소환/소멸 시간 정보를 담을 구조체 정의.
{
	BYTE byTableIdx ;									// 테이블 인덱스.

	WORD wStartDay ;									// 시작 날짜.
	WORD wStartHour ;									// 시작 시간.
	WORD wStartMinute ;									// 시작 분.

	WORD wEndDay ;										// 종료 날짜.
	WORD wEndHour ;										// 종료 시간.
	WORD wEndMinute ;									// 종료 분.
} ;




//-------------------------------------------------------------------------------------------------
//		The class CNpcRecallSelf.
//-------------------------------------------------------------------------------------------------
class CNpcRecallSelf : public CNpcRecallBase
{
	typedef std::map< BYTE, st_RECALL_TIMETABLE >		MAP_TIMETABLE ;				// 맵 컨테이너에 담을 인덱스와 데이터 정의.
	MAP_TIMETABLE										m_mTimeTable ;				// 소환 시간 테이블을 담을 맵 컨테이너.

public:
	CNpcRecallSelf(void);
	virtual ~CNpcRecallSelf(void);
	BYTE Get_TableCount() { return BYTE(m_mTimeTable.size()); }
	void Insert_TimeTable(st_RECALL_TIMETABLE*);
	st_RECALL_TIMETABLE* Get_TimeTable(BYTE byIdx);
	void Copy(CNpcRecallSelf*);
};
