// QuestInfo.cpp: implementation of the CQuestInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																	// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestInfo.h"																// 퀘스트 정보 헤더파일을 불러온다.

#include "SubQuestInfo.h"															// 서브 퀘스트 정보 헤더 파일을 불러온다.

#ifdef _CLIENT_
	#include "MHTimeManager.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestInfo::CQuestInfo( DWORD dwQuestIdx )											// 생성자 함수.
{
	m_dwQuestIdx = dwQuestIdx;														// 퀘스트 인덱스를 세팅한다.

	m_dwSubQuestCount = 0;															// 서브 퀘스트 카운트를 0으로 세팅한다.
	m_dwEndParam = 0;																// 종료 파라메터를 0으로 세팅한다.
	m_SubQuestTable.Initialize( MAX_SUBQUEST );										// 서브 퀘스트 정보 테이블을 최대 서브 퀘스트 개수로 초기화 한다.

	m_bRepeat = FALSE;
}

CQuestInfo::~CQuestInfo()															// 소멸자 함수.
{
	Release();																		// 해제 함수를 호출한다.
}

void CQuestInfo::Release()															// 해제 함수.
{
	CSubQuestInfo* pSubQuestInfo = NULL;											// 서브 퀘스트 정보를 받을 포인터 변수를 선언하고, null 처리를 한다.

	m_SubQuestTable.SetPositionHead();												// 서브 퀘스트 테이블을 헤드로 세팅한다.

	while( (pSubQuestInfo = m_SubQuestTable.GetData()) != NULL)					// 서브 퀘스트 정보가 있는 동안 while 문을 돌린다.
	{
		delete pSubQuestInfo;														// 서브 퀘스트 정보를 삭제한다.
	}

	m_SubQuestTable.RemoveAll();													// 서브 퀘스트 정보 테이블을 모두 비운다.
}

void CQuestInfo::AddSubQuestInfo( DWORD dwSubQuestIdx, CSubQuestInfo* pSubQuest )	// 서브 퀘스트 정보를 추가하는 함수.
{
	if( dwSubQuestIdx >= MAX_SUBQUEST ) return;										// 서브 퀘스트 인덱스가, 최대 서브 퀘스트 인덱스를 넘어가면, 리턴 처리를 한다.

	m_SubQuestTable.Add( pSubQuest, dwSubQuestIdx );								// 서브 퀘스트 정보 테이블에 서브 퀘스트를 추가한다.
	
	++m_dwSubQuestCount;															// 서브 퀘스트 카운터를 증가한다.
}

BOOL CQuestInfo::IsValidDateTime() const
{
	if( m_DateLimitList.empty() )
		return TRUE;

	// 현재 시간 구하기(서버시간으로 계산)
	__time64_t curTime=0;
	struct tm curTimeWhen={0,};

#ifdef _CLIENT_
	curTime = MHTIMEMGR->GetServerTime();
#else
	_time64( &curTime );
#endif

	curTimeWhen = *_localtime64( &curTime );
	
	for( std::vector< QuestDateLimitData >::const_iterator iterList = m_DateLimitList.begin() ; iterList != m_DateLimitList.end() ; ++iterList )
	{
		const struct tm& scriptStartTimeWhen = iterList->first;
		const struct tm& scriptEndTimeWhen = iterList->second;

		struct tm compareStartTimeWhen = curTimeWhen;
		struct tm compareEndTimeWhen = curTimeWhen;

		// 월/일 시:분
		if( scriptStartTimeWhen.tm_mon >= 0 )				compareStartTimeWhen.tm_mon = scriptStartTimeWhen.tm_mon;
		if( scriptStartTimeWhen.tm_mday > 0 )				compareStartTimeWhen.tm_mday = scriptStartTimeWhen.tm_mday;
		if( scriptStartTimeWhen.tm_hour > 0 )
		{
			compareStartTimeWhen.tm_hour = scriptStartTimeWhen.tm_hour;
			compareStartTimeWhen.tm_min = 0;
			compareStartTimeWhen.tm_sec = 0;
		}
		if( scriptStartTimeWhen.tm_min > 0 )
		{
			compareStartTimeWhen.tm_min = scriptStartTimeWhen.tm_min;
			compareStartTimeWhen.tm_sec = 0;
		}

		// 월/일 시:분
		if( scriptEndTimeWhen.tm_mon >= 0 )				compareEndTimeWhen.tm_mon = scriptEndTimeWhen.tm_mon;
		if( scriptEndTimeWhen.tm_mday > 0 )				compareEndTimeWhen.tm_mday = scriptEndTimeWhen.tm_mday;
		if( scriptEndTimeWhen.tm_hour > 0 )
		{
			compareEndTimeWhen.tm_hour = scriptEndTimeWhen.tm_hour;
			compareEndTimeWhen.tm_min = 0;
			compareEndTimeWhen.tm_sec = 0;
		}
		if( scriptEndTimeWhen.tm_min > 0 )
		{
			compareEndTimeWhen.tm_min = scriptEndTimeWhen.tm_min;
			compareEndTimeWhen.tm_sec = 0;
		}

		// 요일
		const int scriptStartWeekday = scriptStartTimeWhen.tm_wday;
		const int scriptEndWeekday = ( scriptStartTimeWhen.tm_wday > scriptEndTimeWhen.tm_wday ? scriptEndTimeWhen.tm_wday + 7 : scriptEndTimeWhen.tm_wday );
		const int curWeekday = ( scriptEndWeekday > 6 ? curTimeWhen.tm_wday + 7 : curTimeWhen.tm_wday );
		if( scriptStartWeekday >= 0 )
		{
			int day = scriptStartWeekday - curWeekday;
			day += ( scriptEndWeekday < curWeekday ? 7 : 0 );
			
			compareStartTimeWhen.tm_mday += day;
		}
		if( scriptEndWeekday >= 0 )
		{
			int day = scriptEndWeekday - curWeekday;
			day += ( scriptEndWeekday < curWeekday ? 7 : 0 );
			compareEndTimeWhen.tm_mday += day;
		}

		compareStartTimeWhen.tm_sec = compareEndTimeWhen.tm_sec = 0;
		__time64_t compareStartTime = _mktime64( &compareStartTimeWhen );
		__time64_t compareEndTime = _mktime64( &compareEndTimeWhen );

		if( compareStartTime <= curTime && curTime <= compareEndTime )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

__time64_t CQuestInfo::GetNextStartDateTime( __time64_t curTime ) const
{
	// 현재 시간 구하기(서버시간으로 계산)
	if( curTime == 0 )
	{
#ifdef _CLIENT_
		curTime = MHTIMEMGR->GetServerTime();
#else
		_time64( &curTime );
#endif
	}

	struct tm curTimeWhen = *_localtime64( &curTime );
	__time64_t nextTime=0;

	for( std::vector< QuestDateLimitData >::const_iterator iterList = m_DateLimitList.begin() ; iterList != m_DateLimitList.end() ; ++iterList )
	{
		const struct tm& scriptStartTimeWhen = iterList->first;
		const struct tm& scriptEndTimeWhen = iterList->second;

		struct tm startTimeWhen = curTimeWhen;
		startTimeWhen.tm_hour = startTimeWhen.tm_min = startTimeWhen.tm_sec = 0;

		const int scriptStartWeekday = scriptStartTimeWhen.tm_wday;
		const int scriptEndWeekday = ( scriptStartTimeWhen.tm_wday > scriptEndTimeWhen.tm_wday ? scriptEndTimeWhen.tm_wday + 7 : scriptEndTimeWhen.tm_wday );
		const int curWeekday = ( scriptEndWeekday > 6 ? curTimeWhen.tm_wday + 7 : curTimeWhen.tm_wday );

		// 요일
		if( scriptStartWeekday >= 0 )
		{
			int day = scriptStartWeekday - curWeekday;
			day += ( scriptEndWeekday <= curWeekday ? 7 : 0 );
			day %= 7;
			
			startTimeWhen.tm_mday += day;
		}

		// 분
		if( scriptStartTimeWhen.tm_min > 0 )		startTimeWhen.tm_min = scriptStartTimeWhen.tm_min;
		// 시
		if( scriptStartTimeWhen.tm_hour > 0 )		startTimeWhen.tm_hour = scriptStartTimeWhen.tm_hour;
		// 일
		if( scriptStartTimeWhen.tm_mday > 0 )		startTimeWhen.tm_mday = scriptStartTimeWhen.tm_mday;
		// 월
		if( scriptStartTimeWhen.tm_mon > 0 )		startTimeWhen.tm_mon = scriptStartTimeWhen.tm_mon;

		__time64_t startTime = _mktime64( &startTimeWhen );

		// 경과한 경우
		if( curTime > startTime )
		{
			struct {
				void GetNextStartDate( const struct tm& scriptStartTimeWhen, const struct tm curTimeWhen, struct tm& startTimeWhen ) {
					// 월
					if( scriptStartTimeWhen.tm_mon > 0 && 
						( curTimeWhen.tm_mon > scriptStartTimeWhen.tm_mon ||
						( curTimeWhen.tm_mon == scriptStartTimeWhen.tm_mon && scriptStartTimeWhen.tm_mday > 0 && scriptStartTimeWhen.tm_mday > 0 && curTimeWhen.tm_mday ) ) )
					{
						++startTimeWhen.tm_year;
						return;
					}
					// 일
					if( scriptStartTimeWhen.tm_mday > 0 && 
						( curTimeWhen.tm_mday > scriptStartTimeWhen.tm_mday ||
						( curTimeWhen.tm_mday == scriptStartTimeWhen.tm_mday && scriptStartTimeWhen.tm_hour > 0 && curTimeWhen.tm_hour > scriptStartTimeWhen.tm_hour ) ) )
					{
						++startTimeWhen.tm_mon;
						return;
					}
					// 시
					if( scriptStartTimeWhen.tm_hour > 0 &&
						( curTimeWhen.tm_hour > scriptStartTimeWhen.tm_hour ||
						( curTimeWhen.tm_hour == scriptStartTimeWhen.tm_hour && scriptStartTimeWhen.tm_min > 0 && curTimeWhen.tm_min > scriptStartTimeWhen.tm_min ) ) )
					{
						++startTimeWhen.tm_mday;
						return;
					}
					// 분
					if( scriptStartTimeWhen.tm_min > 0 && curTimeWhen.tm_min > scriptStartTimeWhen.tm_min )
					{
						++startTimeWhen.tm_hour;
						return;
					}
				}
			}GETNEXTSTARTDATE;

			GETNEXTSTARTDATE.GetNextStartDate( scriptStartTimeWhen, curTimeWhen, startTimeWhen );
			startTime = _mktime64( &startTimeWhen );
		}

		if( curTime < startTime && ( nextTime == 0 || nextTime > startTime ) )
			nextTime = startTime;
	}

	return nextTime;
}

__time64_t CQuestInfo::GetNextEndDateTime( __time64_t curTime ) const
{
	// 현재 시간 구하기(서버시간으로 계산)
	if( curTime == 0 )
	{
#ifdef _CLIENT_
		curTime = MHTIMEMGR->GetServerTime();
#else
		_time64( &curTime );
#endif
	}

	struct tm curTimeWhen = *_localtime64( &curTime );
	__time64_t nextTime=0;

	for( std::vector< QuestDateLimitData >::const_iterator iterList = m_DateLimitList.begin() ; iterList != m_DateLimitList.end() ; ++iterList )
	{
		const struct tm& scriptStartTimeWhen = iterList->first;
		const struct tm& scriptEndTimeWhen = iterList->second;

		struct tm endTimeWhen = curTimeWhen;
		endTimeWhen.tm_hour = endTimeWhen.tm_min = endTimeWhen.tm_sec = 0;

		const int scriptEndWeekday = ( scriptStartTimeWhen.tm_wday > scriptEndTimeWhen.tm_wday ? scriptEndTimeWhen.tm_wday + 7 : scriptEndTimeWhen.tm_wday );
		const int curWeekday = ( scriptEndWeekday > 6 ? curTimeWhen.tm_wday + 7 : curTimeWhen.tm_wday );
		
		// 요일
		if( scriptEndWeekday >= 0 )
		{
			int day = scriptEndWeekday - curWeekday;
			day += ( scriptEndWeekday < curWeekday ? 7 : 0 );
			endTimeWhen.tm_mday += day;
		}

		// 월/일 시:분
		if( scriptEndTimeWhen.tm_mon >= 0 )				endTimeWhen.tm_mon = scriptEndTimeWhen.tm_mon;
		if( scriptEndTimeWhen.tm_mday > 0 )				endTimeWhen.tm_mday = scriptEndTimeWhen.tm_mday;
		if( scriptEndTimeWhen.tm_hour > 0 )
		{
			endTimeWhen.tm_hour = scriptEndTimeWhen.tm_hour;
			endTimeWhen.tm_min = 0;
			endTimeWhen.tm_sec = 0;
		}
		if( scriptEndTimeWhen.tm_min > 0 )
		{
			endTimeWhen.tm_min = scriptEndTimeWhen.tm_min;
			endTimeWhen.tm_sec = 0;
		}

		__time64_t endTime = _mktime64( &endTimeWhen );
		if( curTime < endTime && ( nextTime == 0 || nextTime > endTime ) )
			nextTime = endTime;
	}

	return nextTime;
}