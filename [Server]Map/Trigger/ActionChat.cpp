#include "StdAfx.h"
#include "ActionChat.h"
#include "common.h"
#include "..\Object.h"
#include "..\UserTable.h"
#include "..\PackedData.h"

namespace Trigger
{
	void CActionChat::DoAction()
	{
		TESTMSG message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_CHAT;
		message.Protocol = MP_CHAT_NORMAL;

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

			if(0 == object)
			{
				continue;
			}
			else if(eObjectKind_Player != object->GetObjectKind())
			{
				continue;
			}

			message.dwObjectID = objectIndex;
			PACKEDDATA_OBJ->QuickSend(
				object,
				&message,
				sizeof(message));
		}
	}
}