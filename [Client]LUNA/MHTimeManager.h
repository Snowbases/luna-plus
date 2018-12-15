// MHTimeManager.h: interface for the CMHTimeManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MHTIMEMANAGER_H__890F483A_4FB0_4A20_91BF_BD8BD48941A0__INCLUDED_)
#define AFX_MHTIMEMANAGER_H__890F483A_4FB0_4A20_91BF_BD8BD48941A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 090907 ShinJS --- 5팀 길찾기 Source 적용시 추가. 길찾기 정보 출력
#define MHTIMEMGR	USINGTON(CMHTimeManager)
#define HTR_S(x)	USINGTON(CMHTimeManager)->RecordTime_Start(x);
#define HTR_E(x)	USINGTON(CMHTimeManager)->RecordTime_End(x);


enum eRecordType	
{
	// 시간기록을 동시에 여러 곳에서 할수 있도록 타입을 두어서
	// 원하는 타입의 시간을 얻어내게 한다.
	eRT_OneLineCheck,		// 직선으로 이동가능한가
	eRT_InputPoint,
	eRT_PathFind_Astar,
	eRT_MakeAStarPath,
	eRT_Optimize,
	eRT_Optimize1,
	eRT_Optimize2,
	eRT_AStarTotal,
	eRT_PathManagerLoad,

	eRT_NUMBER_TYPE,
};

class CMHTimeManager  
{
	DWORD m_MHDate;
	DWORD m_MHTime;
	
	DWORD m_lasttime;

	int m_ServerTimeTick;
	__time64_t m_ServerTime;

public:
//	//MAKESINGLETON(CMHTimeManager);
	
	CMHTimeManager();
	virtual ~CMHTimeManager();

	void Init(DWORD mhDate,DWORD mhTime);
	void Process();

	DWORD GetMHDate();
	DWORD GetMHTime();
	
	void GetMHDate(BYTE& year,BYTE& month,BYTE& day);
	void GetMHTime(BYTE& hour,BYTE& minute);

	DWORD GetNewCalcCurTime();

	void SetServerTime( __time64_t serverTime );
	__time64_t GetServerTime() const { return m_ServerTime; }

	///--------------------------------------------------
	/// 프로파일링 - 특정 구역의 시간을 재기 위한 함수들
	/// 단순 시간, 누적평균, 최소 최대를 잴 수 있다.
	///--------------------------------------------------
private:
	LARGE_INTEGER		m_StartCount[eRT_NUMBER_TYPE];
	LARGE_INTEGER		m_LastCount[eRT_NUMBER_TYPE];
	LARGE_INTEGER		m_ElaspedCount[eRT_NUMBER_TYPE];
	LARGE_INTEGER		m_ElaspedCountAccum[eRT_NUMBER_TYPE];
	int					m_AccumNumber[eRT_NUMBER_TYPE];
	LARGE_INTEGER		m_Frequency[eRT_NUMBER_TYPE];

	double				m_StartTime[eRT_NUMBER_TYPE];
	double				m_LastTime[eRT_NUMBER_TYPE];
	double				m_ElaspedTime[eRT_NUMBER_TYPE];			// 기록된 시간
	double				m_ElaspedTime_Accum[eRT_NUMBER_TYPE];	// 기록된 누적 시간
	double				m_MinimizeTime[eRT_NUMBER_TYPE];		// 기록된 최소시간
	double				m_MaximizeTime[eRT_NUMBER_TYPE];		// 기록된 최대시간.

public:
	void		RecordTime_Start(eRecordType eType);			// 시간 기록 시작
	void		RecordTime_End(eRecordType eType);				// 시간 기록 끝
	double		GetRecordTime(eRecordType eType);				// 기록한 시간 제공(ms)
	double		GetRecordTime_Sec(eRecordType eType);			// 기록한 시간 제공(Sec)
	double		GetRecordTime_Accum(eRecordType eType);			// 누적 시간
	double		GetRecordTime_Min(eRecordType eType);
	double		GetRecordTime_Max(eRecordType eType);
	void		ResetAccumulation(eRecordType eType);			// 변화량이 들쭉날쭉한경우가 있을수 있으므로 누적량을 리셋해줄 필요도 있다
	void		DrawStatus();									// 각종 데이터를 그린다
	void		SetDrawStatus( BOOL bDrawStatus )				{ m_bDrawStatus = bDrawStatus; }
private:

private:
	LARGE_INTEGER		m_StartTime_g_Curtime;
	LARGE_INTEGER		m_NowTime_g_Curtime;
	LARGE_INTEGER		m_Frequency_g_Curtime;
	BOOL				m_bDrawStatus;

	void	SetStartTime_g_Curtime();
	DWORD	GetCurrentTime_g_Curtime();	// DWORD로서 프로그램이 시작된 이후 값(ms)

	
};
EXTERNGLOBALTON(CMHTimeManager)
#endif // !defined(AFX_MHTIMEMANAGER_H__890F483A_4FB0_4A20_91BF_BD8BD48941A0__INCLUDED_)
