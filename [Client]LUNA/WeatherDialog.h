#pragma once
#include ".\interface\cdialog.h"
#include ".\interface\cButton.h"

class CWeatherDialog :
	public cDialog
{

	cButton*		m_pWeatherBtn[eWS_Max];

public:
	CWeatherDialog(void);
	virtual ~CWeatherDialog(void);

	void Linking();

	// 081020 LYW --- WeatherDialog : SetActive 함수 추가(공성전에서는 알아서 활성화 되지 않도록 수정한다. - 송가람).
	virtual void SetActive(BOOL val) ;

	void SetWeather(WORD wState);
};
