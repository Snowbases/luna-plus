#include "StdAfx.h"
#include "ActionPlayWeather.h"
#include "common.h"
#include "..\UserTable.h"
#include "..\Object.h"

namespace Trigger
{
	void CActionPlayWeather::DoAction()
	{
		DWORD dwWeatherTypeHashCode = GetValue(eProperty_Name);
		BOOL bIsOn					= GetValue(eProperty_WeatherState);
		DWORD dwIntensity			= (DWORD)GetValue(eProperty_WeatherIntensity);

		MSG_DWORD3 message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_TRIGGER;
		message.Protocol = MP_TRIGGER_WEATHER_CHANGE_ACK;
		message.dwData1 = dwWeatherTypeHashCode;
		message.dwData2 = (bIsOn ? 1 : 0);
		message.dwData3 = dwIntensity;

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(object)
			{
				object->SendMsg(&message, sizeof(message));
			}
		}
	}
}