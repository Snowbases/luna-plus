// MHTimeManager.h: interface for the CMHTimeManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MHTIMEMANAGER_H__890F483A_4FB0_4A20_91BF_BD8BD48941A0__INCLUDED_)
#define AFX_MHTIMEMANAGER_H__890F483A_4FB0_4A20_91BF_BD8BD48941A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MHTIMEMGR_OBJ CMHTimeManager::GetInstance()

class CMHTimeManager  
{
	DWORD m_MHDate;
	DWORD m_MHTime;

	DWORD m_lasttime;

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.27
	SYSTEMTIME		m_stLocalTime;
	SYSTEMTIME		m_stOldLocalTime;
	// E 데이트 존 추가 added by hseos 2007.11.27

public:
	GETINSTANCE(CMHTimeManager);
	
	CMHTimeManager();
	virtual ~CMHTimeManager();

	void Init(DWORD mhDate,DWORD mhTime);
	void Process();

	DWORD GetMHDate();
	DWORD GetMHTime();

	void GetMHDate(BYTE& year,BYTE& month,BYTE& day);
	void GetMHTime(BYTE& hour,BYTE& minute);

	DWORD GetNewCalcCurTime();

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.27
	SYSTEMTIME*		GetCurLocalTime()	{ return &m_stLocalTime; }
	SYSTEMTIME*		GetOldLocalTime()	{ return &m_stOldLocalTime; }
	void			ProcCurLocalTime();
	void			ProcOldLocalTime()	{ m_stOldLocalTime = m_stLocalTime; } 
	// E 데이트 존 추가 added by hseos 2007.11.27
};

#endif // !defined(AFX_MHTIMEMANAGER_H__890F483A_4FB0_4A20_91BF_BD8BD48941A0__INCLUDED_)
