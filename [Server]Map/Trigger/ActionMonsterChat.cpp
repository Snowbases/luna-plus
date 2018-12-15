#include "StdAfx.h"
#include "ActionMonsterChat.h"
#include "common.h"
#include "..\Object.h"
#include "..\UserTable.h"
#include "..\PackedData.h"

namespace Trigger
{
	void CActionMonsterChat::DoAction()
	{
		MSG_DWORD4 message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_CHAT;
		message.Protocol = MP_CHAT_MONSTERSPEECH;
		// 0: 말풍선만 표시, 1: 말풍선 + 채팅창 표시.
		// 1번으로 고정시키고 추가 요구가 있을때 확장한다
		message.dwData1 = 1;
		// 몬스터 대화 번호
		message.dwData2 = GetValue(eProperty_MonsterSpeech);

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}
			else if(FALSE == (object->GetObjectKind() & eObjectKind_Monster))
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