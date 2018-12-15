// WeatherManager.cpp: implementation of the CWeatherManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WeatherManager.h"
#include "Player.h"
#include "MhFile.h"
#include "..\[cc]header\GameResourceManager.h"
#include "UserTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLOBALTON(CWeatherManager);

CWeatherManager::CWeatherManager()
{
	m_dwLastCheckTime = 0;

	m_wState = eWS_Clear;

	ZeroMemory(m_nWeatherDecisionRate, sizeof(m_nWeatherDecisionRate));
	m_bAcceptWeather = FALSE;
	
}

CWeatherManager::~CWeatherManager()
{}

void CWeatherManager::Init()
{
	int nTemp[eWS_Max] = {51, 31, 10, 2, 5, 1};
	int nAccumulate = 0;

	int i;
	for(i=0; i<eWS_Max; i++)
	{
		if(100 <= nAccumulate)
		{
			m_nWeatherDecisionRate[i] = 0;
			continue;
		}

		m_nWeatherDecisionRate[i] = nAccumulate + nTemp[i];
		nAccumulate += nTemp[i];
	}

	if(nAccumulate == 100)
		m_bAcceptWeather = TRUE;
	else
		m_bAcceptWeather = FALSE;
}

void CWeatherManager::Process()
{
	if(!m_bAcceptWeather)
		return;

	int nChangeTime = 300000;	// default 5 minute

	switch(m_wState)
	{
	case eWS_Clear:			nChangeTime=300000;			break;
	case eWS_Cloudy:		nChangeTime=300000;			break;
	case eWS_Rainy:			nChangeTime=300000;			break;
	case eWS_Windy:			nChangeTime=120000;			break;
	case eWS_Foggy:			nChangeTime=60000;			break;
	case eWS_Typoon:		nChangeTime=60000;			break;
	}

	if( 0==m_dwLastCheckTime || m_dwLastCheckTime+nChangeTime < gCurTime)
	{
		int nNextWeather = rand()%100;
		int nNextWeatherState = eWS_Clear;

		if(0<=nNextWeather && nNextWeather<m_nWeatherDecisionRate[eWS_Clear])
		{
			nNextWeatherState = eWS_Clear;
		}
		else if(m_nWeatherDecisionRate[eWS_Clear]<=nNextWeather && nNextWeather<m_nWeatherDecisionRate[eWS_Cloudy])
		{
			nNextWeatherState = eWS_Cloudy;
		}
		else if(m_nWeatherDecisionRate[eWS_Cloudy]<=nNextWeather && nNextWeather<m_nWeatherDecisionRate[eWS_Rainy])
		{
			nNextWeatherState = eWS_Rainy;
		}
		else if(m_nWeatherDecisionRate[eWS_Rainy]<=nNextWeather && nNextWeather<m_nWeatherDecisionRate[eWS_Windy])
		{
			nNextWeatherState = eWS_Windy;
		}
		else if(m_nWeatherDecisionRate[eWS_Windy]<=nNextWeather && nNextWeather<m_nWeatherDecisionRate[eWS_Foggy])
		{
			nNextWeatherState = eWS_Foggy;
		}
		else if(m_nWeatherDecisionRate[eWS_Foggy]<=nNextWeather && nNextWeather<m_nWeatherDecisionRate[eWS_Typoon])
		{
			nNextWeatherState = eWS_Typoon;
		}

		m_wState = WORD( nNextWeatherState );
		m_dwLastCheckTime = gCurTime+1;
	}
}