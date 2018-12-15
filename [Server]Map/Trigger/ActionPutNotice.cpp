#include "StdAfx.h"
#include "ActionPutNotice.h"
#include "common.h"
#include "..\UserTable.h"
#include "..\Object.h"

namespace Trigger
{
	void CActionPutNotice::DoAction()
	{
		TESTMSG message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_SIGNAL;
		message.Protocol = MP_SIGNAL_COMMONUSER;

		const DWORD hashCode = GetValue(eProperty_Text);
		LPCTSTR text = GetAliasName(hashCode);
		// 091019 LUJ, 문자열과 대응하지 않는 해쉬 코드는 SystemMsg.bin의 번호로 간주한다
		const BOOL isSytemMessage = (0 == _tcsicmp("?", text));

		if(isSytemMessage)
		{
			_stprintf(
				message.Msg,
				"SystemMsg.bin\'%u",
				hashCode);
		}
		else
		{
			SafeStrCpy(
				message.Msg,
				text,
				sizeof(message.Msg)/sizeof(*message.Msg));
		}

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