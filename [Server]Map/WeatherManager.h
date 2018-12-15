// WeatherManager.h: interface for the CWeatherManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEATHERMANAGER_H__D6E55A4F_B4CE_4D4B_856C_AFA1A1947582__INCLUDED_)
#define AFX_WEATHERMANAGER_H__D6E55A4F_B4CE_4D4B_856C_AFA1A1947582__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "..\[cc]header\commonstruct.h"

#define WEATHERMGR USINGTON(CWeatherManager)

class CPlayer;

class CWeatherManager  
{
	DWORD m_dwLastCheckTime;
	WORD m_wState;
	int m_nWeatherDecisionRate[eWS_Max];
	BOOL m_bAcceptWeather;
	
		
public:
	CWeatherManager();
	virtual ~CWeatherManager();

	void Init();
	void Process();

	WORD GetWeatherState() { return m_wState; };
};

EXTERNGLOBALTON(CWeatherManager);

#endif // !defined(AFX_WEATHERMANAGER_H__D6E55A4F_B4CE_4D4B_856C_AFA1A1947582__INCLUDED_)
