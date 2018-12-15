// PlustimeMgr.h: interface for the CPlustimeMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUSTIMEMGR_H__D961E6C3_E821_4E3C_9751_17200D0647BD__INCLUDED_)
#define AFX_PLUSTIMEMGR_H__D961E6C3_E821_4E3C_9751_17200D0647BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PLUSTIMEMGR CPlustimeMgr::GetInstance()


struct stPlusTime
{
	WORD	StartDay;
	WORD	StartHour;
	WORD	StartMinute;
	WORD	EndDay;
	WORD	EndHour;
	WORD	EndMinute;
	WORD	Index;
	WORD	EventIndex;
	DWORD	Rate;
	char	strTitle[32];
	char	strContext[128];
};


class CPlustimeMgr
{
	// 090923 ONS 메모리풀 교체
	CPool<stPlusTime>*				m_pPlusTimePool;
	CYHHashTable<stPlusTime>		m_PlusTimeTable;
	CYHHashTable<stPlusTime>		m_AppliedTable;

	DWORD		m_PlustimeCount;
	BOOL		m_bToggleOn;

	char		m_Context[1024];

	//
	void PlusTimeOn( WORD Index, WORD Rate );
	void PlusTimeOff( WORD Index );

public:
	MAKESINGLETON( CPlustimeMgr );

	CPlustimeMgr();
	virtual ~CPlustimeMgr();

	void Init();
	void ResetPlusTimeInfo();

	void PlusTimeOn();
	void PlusTimeOff();
	void PlustimeAllOff();

	void Process();	
};


#endif // !defined(AFX_PLUSTIMEMGR_H__D961E6C3_E821_4E3C_9751_17200D0647BD__INCLUDED_)
