#include "StdAfx.h"
#include "ActionPlaySound.h"
#include "common.h"
#include "..\UserTable.h"
#include "..\Object.h"

namespace Trigger
{
	void CActionPlaySound::DoAction()
	{
		BOOL bIsBGM		= GetValue(eProperty_BGM);
		WORD wSoundNum	= (WORD)GetValue(eProperty_Sound);

		MSG_WORD2 message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_TRIGGER;
		message.Protocol = MP_TRIGGER_PLAYSOUND_ACK;
		message.wData1 = WORD( bIsBGM );
		message.wData2 = wSoundNum;

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