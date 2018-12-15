// MHTimeManager.cpp: implementation of the CMHTimeManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MHTimeManager.h"
#include "mmsystem.h"
#include "./interface/cFont.h"
#include "ObjectManager.h"
#include "QuestManager.h"


DWORD gCurTime = 0;		//0 초기화 추가...
DWORD gTickTime;
float gAntiGravity = 1.0f;

#define TICK_PER_DAY 86400000		// 24 * 60 * 60 * 1000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CMHTimeManager)
CMHTimeManager::CMHTimeManager()
{
	m_MHDate = 0;
	m_MHTime = 0;

	m_lasttime = 0;
	ZeroMemory(
		m_StartCount,
		sizeof(m_StartCount));
	ZeroMemory(
		m_LastCount,
		sizeof(m_LastCount));
	ZeroMemory(
		m_ElaspedCount,
		sizeof(m_ElaspedCount));
	ZeroMemory(
		m_ElaspedCountAccum,
		sizeof(m_ElaspedCountAccum));
	ZeroMemory(
		m_AccumNumber,
		sizeof(m_AccumNumber));
	ZeroMemory(
		m_Frequency,
		sizeof(m_Frequency));
	ZeroMemory(
		m_StartTime,
		sizeof(m_StartTime));
	ZeroMemory(
		m_LastTime,
		sizeof(m_LastTime));
	ZeroMemory(
		m_ElaspedTime,
		sizeof(m_ElaspedTime));
	ZeroMemory(
		m_ElaspedTime_Accum,
		sizeof(m_ElaspedTime_Accum));

	// -1로 초기화해야됨
	for ( int i = 0 ; i < eRT_NUMBER_TYPE ; ++i )
		m_MinimizeTime[i] = m_MaximizeTime[i] = -1.0;

	// 위의 것은 누적이라서 오차가 쌓일 수 있다.
	// 시작시간과의 상대차를 계산하는 방법
	SetStartTime_g_Curtime();

	m_bDrawStatus = FALSE;

	m_ServerTimeTick = 0;
	_time64( &m_ServerTime );
}

CMHTimeManager::~CMHTimeManager()
{

}

void CMHTimeManager::Init(DWORD mhDate,DWORD mhTime)
{
	m_MHDate = mhDate;
	m_MHTime = mhTime;
}

void CMHTimeManager::Process()
{
	static bool bFirst = true;
	static DWORD curtime = 0;
	static int tempDay = 0;

	if( bFirst )
	{
		curtime = m_lasttime = GetTickCount();
		bFirst = false;
	}
	else
	{
		curtime = GetTickCount();		

		if( curtime < m_lasttime )		// DWORD 형의 한계를 넘어갔다
			gTickTime = curtime - m_lasttime + 4294967295; //( 2^32 - 1 )
		else
			gTickTime = curtime - m_lasttime;
		m_lasttime = curtime;
		
		gCurTime += gTickTime;

		
		//////////////////////////////////////////////////////////////////////////
		// 묵향력-_-a; 
		// 묵향력 사용하지 않아 현실과 같은 시계로 변경
		m_MHTime += gTickTime;
		if(m_MHTime >= TICK_PER_DAY)
		{
			++m_MHDate;
			m_MHTime -= TICK_PER_DAY;
		}
	}

	// 100408 ShinJS --- 서버 시간 갱신
	m_ServerTimeTick += (int)gTickTime;
	int sec = int( m_ServerTimeTick / 1000 );
	m_ServerTimeTick = m_ServerTimeTick % 1000;
	m_ServerTime += sec;
}

DWORD CMHTimeManager::GetNewCalcCurTime()	//cur타임 새로 받아오기 기존 gCurTime, gTickTime에 영향없다.
{
	DWORD lcurtime = GetTickCount();
	DWORD lTickTime;
		
	if( lcurtime < m_lasttime )		// DWORD 형의 한계를 넘어갔다
		lTickTime = lcurtime - m_lasttime + 4294967295; //( 2^32 - 1 )
	else
		lTickTime = lcurtime - m_lasttime;
	
	return gCurTime + lTickTime;
}

DWORD CMHTimeManager::GetMHDate()
{
	return m_MHDate;
}

DWORD CMHTimeManager::GetMHTime()
{
	return m_MHTime;
}

void CMHTimeManager::GetMHDate(BYTE& year,BYTE& month,BYTE& day)
{
	year = (BYTE)(m_MHDate / 360) + 1;
	month = (BYTE)((m_MHDate - year) / 30) + 1;
	day = (BYTE)m_MHDate % 30 + 1;  // -_-a; 묵향에선 모든 달은 30일까지다 -_-	
}

void CMHTimeManager::GetMHTime(BYTE& hour,BYTE& minute)
{
	hour = BYTE(m_MHTime / 3600000);
	minute = BYTE((m_MHTime - hour) / 60000);
}

///--------------------------------------------------
/// 특정 구역의 시간을 재기 위한 함수
/// 단순 시간, 누적평균, 최소 최대를 잴 수 있다.
///--------------------------------------------------
void CMHTimeManager::RecordTime_Start( eRecordType eType )
{
#ifdef _GMTOOL_
	assert( 0 <= eType );
	assert( eRT_NUMBER_TYPE > eType );

	// 시작 카운트 기록
	QueryPerformanceCounter(&m_StartCount[eType]);

	// 프리퀀시(아마도 해상도) 기록
	// 이것이 0이 나오면 이 해상도 함수를 쓸 수 없는 시스템이란거다
	QueryPerformanceFrequency(&m_Frequency[eType]);
	if ( m_Frequency[eType].QuadPart == 0 ) HTRACE( "QueryPerformanceFrequency가 0입니다!" );
#endif
}

void CMHTimeManager::RecordTime_End( eRecordType eType )
{
#ifdef _GMTOOL_
	assert( 0 <= eType );
	assert( eRT_NUMBER_TYPE > eType );

	// 최근 카운트 기록
	QueryPerformanceCounter(&m_LastCount[eType]);

	// 사이의 카운트를 연산
	m_ElaspedCount[eType].QuadPart	=	m_LastCount[eType].QuadPart	 - m_StartCount[eType].QuadPart;

	// 누적시킴
	m_ElaspedCountAccum[eType].QuadPart += m_ElaspedCount[eType].QuadPart;
	// 누적 횟수 
	m_AccumNumber[eType]++;


	// 최소 최대시간을 저장해준다.
	if ( m_MinimizeTime[eType] < 0.0 )
	{
		// 처음 시간을 잰것이면 그냥 저장한다.
		m_MinimizeTime[eType] = m_MaximizeTime[eType] = GetRecordTime(eType);
	}
	else
	{
		double		ElapsedTime = GetRecordTime(eType);
		m_MinimizeTime[eType] = min(ElapsedTime, m_MinimizeTime[eType]);
		m_MaximizeTime[eType] = max(ElapsedTime, m_MaximizeTime[eType]);		
	}
#endif
}

double CMHTimeManager::GetRecordTime( eRecordType eType )
{
	assert( 0 <= eType );
	assert( eRT_NUMBER_TYPE > eType );

	// 지난 시간 계산
	if (m_Frequency[eType].QuadPart == 0) return 0;
	m_ElaspedTime[eType]	=	(double)(m_ElaspedCount[eType].QuadPart)  /  (double)(m_Frequency[eType].QuadPart);

	return m_ElaspedTime[eType] * 1000;
}

double CMHTimeManager::GetRecordTime_Sec( eRecordType eType )
{
	assert( 0 <= eType );
	assert( eRT_NUMBER_TYPE > eType );

	if (m_Frequency[eType].QuadPart == 0) return 0;
	m_ElaspedTime[eType]	=	(double)(m_ElaspedCount[eType].QuadPart)  /  (double)(m_Frequency[eType].QuadPart);

	return m_ElaspedTime[eType];
}

double CMHTimeManager::GetRecordTime_Accum( eRecordType eType )
{
	assert( 0 <= eType );
	assert( eRT_NUMBER_TYPE > eType );

	if (m_Frequency[eType].QuadPart == 0) return 0;
	m_ElaspedTime_Accum[eType] = (double)(m_ElaspedCountAccum[eType].QuadPart) / (double)(m_Frequency[eType].QuadPart) / (double)m_AccumNumber[eType];

	return m_ElaspedTime_Accum[eType] * 1000;
}

double	CMHTimeManager::GetRecordTime_Min(eRecordType eType)
{
	return m_MinimizeTime[eType];
}

double	CMHTimeManager::GetRecordTime_Max(eRecordType eType)
{
	return m_MaximizeTime[eType];
}

void CMHTimeManager::ResetAccumulation( eRecordType eType )
{
	assert( 0 <= eType );
	assert( eRT_NUMBER_TYPE > eType );

	m_ElaspedCountAccum[eType].QuadPart = 0;
	m_ElaspedTime_Accum[eType] = 0;
	m_AccumNumber[eType] = 0;
	m_MinimizeTime[eType] = -1;
	m_MaximizeTime[eType] = -1;
}


// 구조체에 그린다
void CMHTimeManager::DrawStatus()
{
#ifdef _GMTOOL_

	if( !m_bDrawStatus )
		return;

	char temp[ 128 ];
	RECT rc;
	rc.left = 220;
	rc.right = 1220;
	rc.top = 20 * 3;
	rc.bottom = rc.top + 20;

	/// millisecond단위로 재는 QueryPerformanceCounter이용 조사한 것들...
	sprintf( temp, "OneLineCheck     : %7.4fms", GetRecordTime( eRT_OneLineCheck ) );
	rc.top = 20 * 3;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "OneLineCheck Avg : %7.4fms", GetRecordTime_Accum( eRT_OneLineCheck ) );
	rc.top = 20 * 4;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "InputPoint     : %7.4fms", GetRecordTime( eRT_InputPoint ) );
	rc.top = 20 * 5;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "InputPoint Avg : %7.4fms", GetRecordTime_Accum( eRT_InputPoint ) );
	rc.top = 20 * 6;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "PathFind_Astar     : %7.4fms", GetRecordTime( eRT_PathFind_Astar ) );
	rc.top = 20 * 7;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "PathFind_Astar Avg : %7.4fms", GetRecordTime_Accum( eRT_PathFind_Astar ) );
	rc.top = 20 * 8;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "MakeAStarPath     : %7.4fms", GetRecordTime( eRT_MakeAStarPath ) );
	rc.top = 20 * 9;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "MakeAStarPath Avg : %7.4fms", GetRecordTime_Accum( eRT_MakeAStarPath ) );
	rc.top = 20 * 10;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "Optimize     : %7.4fms", GetRecordTime( eRT_Optimize ) );
	rc.top = 20 * 11;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "Optimize Avg : %7.4fms", GetRecordTime_Accum( eRT_Optimize ) );
	rc.top = 20 * 12;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "Optimize1     : %7.4fms", GetRecordTime( eRT_Optimize1 ) );
	rc.top = 20 * 13;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "Optimize1 Avg : %7.4fms", GetRecordTime_Accum( eRT_Optimize1 ) );
	rc.top = 20 * 14;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "Optimize2     : %7.4fms", GetRecordTime( eRT_Optimize2 ) );
	rc.top = 20 * 15;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "Optimize2 Avg : %7.4fms", GetRecordTime_Accum( eRT_Optimize2 ) );
	rc.top = 20 * 16;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));


	rc.left = 430;
	rc.right = 1430;

	sprintf( temp, "AStarTotal     : %7.4fms", GetRecordTime( eRT_AStarTotal ) );
	rc.top = 20 * 11;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "AStarTotal Avg : %7.4fms", GetRecordTime_Accum( eRT_AStarTotal ) );
	rc.top = 20 * 12;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	sprintf( temp, "PathManagerLoad     : %7.4fms", GetRecordTime( eRT_PathManagerLoad ) );
	rc.top = 20 * 14;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));
	sprintf( temp, "PathManagerLoad Avg : %7.4fms", GetRecordTime_Accum( eRT_PathManagerLoad ) );
	rc.top = 20 * 15;	rc.bottom = rc.top + 20;
	CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, RGBA_MAKE(255,255,255,200));

	// 주인공 위치 출력
	if( HERO )
	{
		VECTOR3 pos;
		HERO->GetPosition( &pos );
		sprintf( temp, "Hero Pos : %7.4f, %7.4f, %7.4f", pos.x, pos.y, pos.z );
		rc.left = 10;		rc.right = 1010;
		rc.top = 20 * 21;	rc.bottom = rc.top + 20;
		CFONT_OBJ->RenderFont( 0, temp, strlen(temp), &rc, 0xFF00FF00);
	}

#endif // _GMTOOL_

}


void CMHTimeManager::SetStartTime_g_Curtime()
{
	// 시작 카운트 기록
	QueryPerformanceCounter(&m_StartTime_g_Curtime);

	// 프리퀀시(아마도 해상도) 기록
	// 이것이 0이 나오면 이 해상도 함수를 쓸 수 없는 시스템이란거다
	QueryPerformanceFrequency(&m_Frequency_g_Curtime);
	if ( m_Frequency_g_Curtime.QuadPart == 0 ) HTRACE( "QueryPerformanceFrequency가 0입니다!" );
}

DWORD CMHTimeManager::GetCurrentTime_g_Curtime()
{
	// 최근 카운트 기록
	QueryPerformanceCounter(&m_NowTime_g_Curtime);

	// 사이의 카운트를 연산
	double t_ElapsedTime = (double)(m_NowTime_g_Curtime.QuadPart - m_StartTime_g_Curtime.QuadPart) / (double)m_Frequency_g_Curtime.QuadPart;
	
	return (DWORD)(t_ElapsedTime * 1000);
}

void CMHTimeManager::SetServerTime( __time64_t serverTime )
{
	m_ServerTimeTick = 0;
	m_ServerTime = serverTime;

	// 서버 시간을 사용하는 작업들을 초기화 시킨다.
	// 시간 설정 퀘스트 초기화
	QUESTMGR->InitDateTime();
}