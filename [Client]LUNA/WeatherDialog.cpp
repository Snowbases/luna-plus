#include "stdafx.h"
#include "weatherdialog.h"
#include "WindowIDEnum.h"
#include ".\Interface\cWindowManager.h"

// 081020 LYW --- WeatherDialog : SetActive 함수 추가(공성전에서는 알아서 활성화 되지 않도록 수정한다. - 송가람).
#include "./SiegeWarfareMgr.h"

//090525 pdy 하우징 추가(하우징에서는 알아서 활성화 되지 않도록 수정한다 ).
#include "cHousingMgr.h"

CWeatherDialog::CWeatherDialog(void)
{
}

CWeatherDialog::~CWeatherDialog(void)
{
}

void CWeatherDialog::Linking()
{
/*	int i;
	for(i=0; i<eWS_Max; i++)
	{
		m_pWeatherBtn[i] = (cButton*)GetWindowForID(WEATHER_BTN_CLEAR + i);
		m_pWeatherBtn[i]->SetActive(FALSE);
		m_pWeatherBtn[i]->SetDisable(TRUE);
	}*/
}

void CWeatherDialog::SetWeather(WORD wState)
{
	int i;
	for(i=0; i<eWS_Max; i++)
	{
		m_pWeatherBtn[i]->SetDisable(FALSE);

		if(i == (int)wState)
			m_pWeatherBtn[i]->SetActive(TRUE);
		else
			m_pWeatherBtn[i]->SetActive(FALSE);

		m_pWeatherBtn[i]->SetDisable(TRUE);
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: SetActive
//	DESC		: SetActive 함수 추가(공성전에서는 알아서 활성화 되지 않도록 수정한다. - 송가람).
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 20, 2008
//-------------------------------------------------------------------------------------------------
void CWeatherDialog::SetActive(BOOL val)
{
	// 090525 pdy --- MiniMapDlg : 하우징 맵이면, CWeatherDialog을 비활성화 한다.
  	if( SIEGEWARFAREMGR->IsSiegeWarfareZone() || 
		HOUSINGMGR->IsHousingMap() )
  	{
  		cDialog::SetActive( FALSE ) ;
  		return ;
  	}
	else
	{
		cDialog::SetActive( val ) ;
	}
}