#include "StdAfx.h"
#include "common.h"
#include "CheckFunc.h"
#include "Message.h"
#include "Condition.h"
#include "Trigger.h"
#include "Manager.h"
#include "Timer.h"
#include "..\Object.h"
#include "..\Monster.h"
#include "..\UserTable.h"
#include "..\ChannelSystem.h"
#include "..\Player.h"
#include "..\Dungeon\DungeonMgr.h"
#include "..\GuildScore.h"
#include "..\GuildManager.h"
#include "..\..\[CC]Quest\QuestDefines.h"
#include "..\Quest.h"
#include "..\PartyManager.h"
#include "..\Party.h"
#include "..\FixedTile.h"
#include "..\FixedTileInfo.h"
#include "..\TileManager.h"
#include "..\MHMap.h"

namespace Trigger
{
	BOOL IsTrue(const stProperty& property, int value)
	{
		switch(property.mOperation)
		{
		case eOperation_More:
			{
				return property.mValue < value;
			}
		case eOperation_Less:
			{
				return property.mValue > value;
			}
		case eOperation_Equal:
			{
				return property.mValue == value;
			}
		case eOperation_Not:
			{
				return property.mValue != value;
			}
		case eOperation_LessThan:
			{
				return property.mValue >= value;
			}
		case eOperation_MoreThan:
			{
				return property.mValue <= value;
			}
		}

		return FALSE;
	}

	BOOL IsObjectStateChanged( const stProperty& property, int value )
	{
		if(property.mOperation != eOperation_Equal)
			return FALSE;

		static int Curstate = -1;
		
		if(value == property.mValue)
		{	
			if(Curstate == value)
			{
				return FALSE;
			}
			else
			{
				Curstate = value;
				return TRUE;
			}
		}

		return FALSE;
	}

	BOOL CheckNone(const CCondition&, const CMessage&)
	{
		return TRUE;
	}

	BOOL CheckFlag(const CCondition&, const CMessage&)
	{
		return TRUE;
	}

	BOOL CheckHP(const CCondition& condition, const CMessage& msg)
	{
		CObject* const pObject = g_pUserTable->FindUser(msg.GetValue(eProperty_ObjectIndex));

		if(0 == pObject)
		{
			return FALSE;
		}

		const stProperty& property = condition.GetProperty(eProperty_Health);
		float value = 0;

		if(property.mIsPercent)
		{
			value = float(pObject->GetLife()) / float(pObject->GetMaxLife()) * 100.0f;
		}
		else
		{
			value = float(pObject->GetLife());
		}

		return IsTrue(property, int(value));
	}

	BOOL CheckMP(const CCondition& condition, const CMessage& msg)
	{
		CObject* const pObject = g_pUserTable->FindUser(msg.GetValue(eProperty_ObjectIndex));

		if(0 == pObject)
		{
			return FALSE;
		}

		const stProperty& property = condition.GetProperty(eProperty_Health);
		float value = 0;

		if(property.mIsPercent)
		{
			value = float(pObject->GetMana()) / float(pObject->GetMaxMana()) * 100.0f;
		}
		else
		{
			value = float(pObject->GetMana());
		}

		return IsTrue(property, int(value));
	}

	BOOL CheckRange(const CCondition& condition, const CMessage& msg)
	{
		CObject* const pObject = g_pUserTable->FindUser(msg.GetValue(eProperty_ObjectIndex));

		if(pObject)
		{
			VECTOR3 scriptPos = {0};
			scriptPos.x = (float)condition.GetValue(eProperty_RangeX) * 100;
			scriptPos.z = (float)condition.GetValue(eProperty_RangeZ) * 100;

			VECTOR3 pos = {0};
			pObject->GetPosition(&pos);

			const stProperty& property = condition.GetProperty(eProperty_Range);
			const int dist = (int)CalcDistanceXZ(&pos, &scriptPos);
			return IsTrue(property, dist);
		}

		return FALSE;
	}

	BOOL CheckEvent(const CCondition& condition, const CMessage& msg)
	{
		const eEvent event = (eEvent)msg.GetValue(eProperty_Event);
		const stProperty& property = condition.GetProperty(eProperty_Event);

		return IsTrue(property, event);
	}

	BOOL CheckAlias(const CCondition& condition, const CMessage& msg)
	{
		// 오브젝트에는 소유주에 따라 별도의 별칭 해쉬코드가 할당되어 있다.
		// 스크립트는 그렇지 않으므로, 복사해와서 값을 변경한다
		stProperty property = condition.GetProperty(eProperty_Alias);
		property.mValue =  condition.GetTrigger().GetHashCode(property.mValue);

		const EObjectKind objectKind = (EObjectKind)msg.GetValue(eProperty_ObjectKind);

		// CObject만이 별칭을 갖고있다.
		if(eObjectKind_None == objectKind)
		{
			return FALSE;
		}

		const DWORD objectIndex = msg.GetValue(eProperty_ObjectIndex);
		const CObject* const object = g_pUserTable->FindUser(objectIndex);

		if(0 == object)
		{
			return FALSE;
		}

		const DWORD alias = object->GetAlias();
		return IsTrue(property, alias);
	}

	BOOL CheckGroupAlias(const CCondition& condition, const CMessage& msg)
	{
		// 오브젝트에는 소유주에 따라 별도의 별칭 해쉬코드가 할당되어 있다.
		// 스크립트는 그렇지 않으므로, 복사해와서 값을 변경한다
		stProperty property = condition.GetProperty(eProperty_GroupAlias);
		property.mValue =  condition.GetTrigger().GetHashCode(property.mValue);

		const EObjectKind objectKind = (EObjectKind)msg.GetValue(eProperty_ObjectKind);

		// CObject만이 별칭을 갖고있다.
		if(eObjectKind_None == objectKind)
		{
			return FALSE;
		}

		const DWORD objectIndex = msg.GetValue(eProperty_ObjectIndex);
		const CObject* const object = g_pUserTable->FindUser(objectIndex);

		if(0 == object)
		{
			return FALSE;
		}

		const DWORD alias = object->GetGroupAlias();
		return IsTrue(property, alias);
	}

	BOOL CheckElapsedDay(const CCondition& condition, const CMessage& msg)
	{
		BOOL bByMidnight = condition.GetValue( eProperty_ByMidnight );
		struct tm triggerWhen = condition.GetTrigger().GetRegTime();

		// 자정 기준 검사인 경우
		if( bByMidnight )
			triggerWhen.tm_sec = triggerWhen.tm_min = triggerWhen.tm_hour = 0;
			
		__time64_t nowTime, triggerRegTime;
		_time64( &nowTime );
		triggerRegTime = _mktime64( &triggerWhen );

		// 검사할 경과 시간
		const __time64_t OneDay(60 * 60 * 24);
		const DWORD dwElapsedDay = condition.GetValue(eProperty_ElapsedDay);
		const __time64_t compareTime(OneDay * dwElapsedDay);
		const int compareDay = int((nowTime - triggerRegTime) / compareTime);
		const stProperty& property = condition.GetProperty(eProperty_ElapsedDay);

		return IsTrue( property, compareDay );
	}

	BOOL CheckElapedSecond(const CCondition& condition, const CMessage& msg)
	{
		struct tm registedDate = condition.GetTrigger().GetRegTime();
		const __time64_t registedTime = _mktime64(&registedDate);
		__time64_t currentTime = 0;
		_time64(&currentTime);

		const int second = int(currentTime - registedTime);
		const stProperty& property = condition.GetProperty(eProperty_ElapsedSecond);

		return IsTrue(property, second);
	}

	BOOL CheckObjectKind(const CCondition& condition, const CMessage& msg)
	{
		const EObjectKind objectKind = (EObjectKind)msg.GetValue(eProperty_ObjectKind);
		const stProperty& property = condition.GetProperty(eProperty_ObjectKind);

		return IsTrue(property, objectKind);
	}

	BOOL CheckPlayerCountInChannel(const CCondition& condition, const CMessage& msg)
	{
		const DWORD playerCount = CHANNELSYSTEM->GetPlayerNumInChannel(condition.GetParameter().mChannelID);
		const stProperty& property = condition.GetProperty(eProperty_PlayerCountInChannel);

		return IsTrue(property, playerCount);
	}

	BOOL CheckWhatDay(const CCondition& condition, const CMessage& msg)
	{
		__time64_t time = 0;
		_time64(&time);
		const struct tm* const timeData = _localtime64(&time);
        const stProperty& property = condition.GetProperty(eProperty_WhatDay);

		return IsTrue(property, timeData->tm_wday);
	}

	BOOL CheckMonsterKind(const CCondition& condition, const CMessage& msg)
	{
		const DWORD objectIndex = msg.GetValue(eProperty_ObjectIndex);
		CObject* const pObject = g_pUserTable->FindUser(objectIndex);

		if(0 == pObject)
		{
			return FALSE;
		}

		const BOOL isNotMonster = ! ( pObject->GetObjectKind() & eObjectKind_Monster );

		if(isNotMonster)
		{
			return FALSE;
		}

		const WORD monsterKind = ((CMonster*)pObject)->GetMonsterKind();
		const stProperty& property = condition.GetProperty(eProperty_MonsterKind);

		return IsTrue(property, monsterKind);
	}

	BOOL CheckVictimObjectKind(const CCondition& condition, const CMessage& msg)
	{
		const stProperty& property = condition.GetProperty(eProperty_VictimObjectKind);		
		const EObjectKind victimObjectKind = EObjectKind(msg.GetValue(eProperty_VictimObjectKind));

		return IsTrue(property, victimObjectKind);
	}

	BOOL CheckVictimMonsterKind(const CCondition& condition, const CMessage& msg)
	{
		const stProperty& property = condition.GetProperty(eProperty_VictimMonsterKind);
		const DWORD monsterKind = msg.GetValue(eProperty_VictimMonsterKind);

		return IsTrue(property, monsterKind);
	}

	// 091015 ONS 죽은 몬스터의 별칭과 그룹별칭을 체크한다.
	BOOL CheckVictimMonsterAlias(const CCondition& condition, const CMessage& msg)
	{
		stProperty property = condition.GetProperty(eProperty_VictimMonsterAlias);
		property.mValue = condition.GetTrigger().GetHashCode(property.mValue);
		const DWORD monsterAlias = msg.GetValue(eProperty_VictimMonsterAlias);

		return IsTrue(property, monsterAlias);
	}

	BOOL CheckVictimMonsterGroupAlias(const CCondition& condition, const CMessage& msg)
	{
		stProperty property = condition.GetProperty(eProperty_VictimMonsterGroupAlias);
		property.mValue = condition.GetTrigger().GetHashCode(property.mValue);
		const DWORD monsterGroupAlias = msg.GetValue(eProperty_VictimMonsterGroupAlias);

		return IsTrue(property, monsterGroupAlias);
	}

	BOOL CheckLoginGuildMember(const CCondition& condition, const CMessage& msg)
	{
		const DWORD objectIndex = msg.GetValue(eProperty_ObjectIndex);
		CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(objectIndex);

		if(0 == player)
		{
			return FALSE;
		}
		else if(eObjectKind_Player != player->GetObjectKind())
		{
			return FALSE;
		}

		CGuildScore* const guildScore = GUILDMGR->GetGuildScore(player->GetGuildIdx());

		if(0 == guildScore)
		{
			return FALSE;
		}

		const stProperty& property = condition.GetProperty(eProperty_LoginGuildMember);

		return IsTrue(property, guildScore->GetPlayerSize());
	}

	BOOL CheckQuestState(const CCondition& condition, const CMessage& msg)
	{
		const DWORD objectIndex = msg.GetValue(eProperty_ObjectIndex);
		CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(objectIndex);

		if(0 == player)
		{
			return FALSE;
		}
		else if(eObjectKind_Player != player->GetObjectKind())
		{
			return FALSE;
		}

		const DWORD questIndex = condition.GetValue(eProperty_Quest);
		CQuest* const quest = player->GetQuestGroup().GetQuest(questIndex);

		if(0 == quest)
		{
			return FALSE;
		}

		switch(condition.GetValue(eProperty_QuestState))
		{
			// 퀘스트를 시작한 상태
		case eQuestExecute_StartQuest:
			{
				return FALSE == quest->IsQuestComplete();
			}
			// 퀘스트를 완료한 상태
		case eQuestExecute_EndQuest:
			{
				return quest->IsQuestComplete();
			}
		}

		return FALSE;
	}

	BOOL CheckGuildRank(const CCondition& condition, const CMessage& msg)
	{
		const DWORD objectIndex = msg.GetValue(eProperty_ObjectIndex);

		CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(objectIndex);

		if(0 == player)
		{
			return FALSE;
		}
		else if(eObjectKind_Player != player->GetObjectKind())
		{
			return FALSE;
		}

		const stProperty& property = condition.GetProperty(eProperty_GuildRank);

		return IsTrue(property, player->GetGuildMemberRank());
	}

	BOOL CheckPartySize(const CCondition& condition, const CMessage& msg)
	{
		const DWORD objectIndex = msg.GetValue(eProperty_ObjectIndex);

		CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(objectIndex);

		if(0 == player)
		{
			return FALSE;
		}
		else if(eObjectKind_Player != player->GetObjectKind())
		{
			return FALSE;
		}

		CParty* const party = PARTYMGR->GetParty(player->GetPartyIdx());

		if(0 == party)
		{
			return FALSE;
		}

		const stProperty& property = condition.GetProperty(eProperty_PartySize);

		return IsTrue(property, party->GetPartyMemberNum());
	}

	BOOL CheckTimerElapsedSecond(const CCondition& condition, const CMessage& msg)
	{
		const DWORD elapsedSecond = msg.GetValue(eProperty_TimerElapsedSecond);
		if(0 == elapsedSecond)
			return FALSE;

		// 091116 LUJ, 값 복사 대신 참조를 사용하도록 함
		const stProperty& property = condition.GetProperty(eProperty_TimerElapsedSecond);
		return IsTrue(property, elapsedSecond);
	}

	BOOL CheckTimerAlias(const CCondition& condition, const CMessage& msg)
	{
		const DWORD timerAlias = msg.GetValue(eProperty_TimerName);
		stProperty property = condition.GetProperty(eProperty_TimerName);
		property.mValue = condition.GetTrigger().GetHashCode(property.mValue);

		return IsTrue(property, timerAlias);
	}

	BOOL Dungeon_CheckSwitchOn(const CCondition& condition, const CMessage& msg)
	{
		const WORD switchNum = WORD(condition.GetValue(eProperty_Dungeon_SwitchOn));
		const DWORD channelID = condition.GetChannelID();
		return (TRUE == DungeonMGR->GetSwitch(channelID, switchNum - 1));
	}

	BOOL Dungeon_CheckSwitchOff(const CCondition& condition, const CMessage& msg)
	{
		const WORD switchNum = WORD(condition.GetValue(eProperty_Dungeon_SwitchOff));
		const DWORD channelID = condition.GetChannelID();
		return (TRUE == DungeonMGR->GetSwitch(channelID, switchNum - 1));
	}

	BOOL Dungeon_CheckDifficulty(const CCondition& condition, const CMessage& msg)
	{
		eDIFFICULTY difficulty = DungeonMGR->GetDifficulty(condition.GetChannelID());
		const stProperty& property = condition.GetProperty(eProperty_Dungeon_Difficulty);
		return IsTrue(property, difficulty);
	}

	BOOL Dungeon_CheckPoint(const CCondition& condition, const CMessage& msg)
	{
		int point = DungeonMGR->GetPoint(condition.GetChannelID());
		const stProperty& property = condition.GetProperty(eProperty_Dungeon_CheckPoint);
		return IsTrue(property, point);
	}

	BOOL Dungeon_CheckPlayerNum(const CCondition& condition, const CMessage& msg)
	{
		DWORD dwPlayerNum = DungeonMGR->GetPlayerNumInDungeon(condition.GetChannelID());
		const stProperty& property = condition.GetProperty(eProperty_Dungeon_CheckPlayerNum);
		return IsTrue(property, (int)dwPlayerNum);
	}

	// 091015 ONS 활성화된 NPC의 아이디를 체크한다.
	BOOL CheckActiveNpcId(const CCondition& condition, const CMessage& msg)
	{
		const stProperty& property = condition.GetProperty(eProperty_ActiveNpcId);
		const DWORD npcId = msg.GetValue(eProperty_ActiveNpcId);	
		
		return IsTrue(property, npcId);
	}

	BOOL CheckObjectState(const CCondition& condition, const CMessage& msg)
	{
		const stProperty& property = condition.GetProperty(eProperty_ObjectState);
		const DWORD objectIndex = msg.GetValue(eProperty_ObjectIndex);

		CObject* const player = (CObject*)g_pUserTable->FindUser(objectIndex);

		if(0 == player)
		{
			return FALSE;
		}
		else if(eObjectKind_Player != player->GetObjectKind())
		{
			return FALSE;
		}

		BASEMOVE_INFO* pInfo = player->GetMoveInfo();
		if(0 == pInfo) 
		{
			return FALSE;
		}

		const DWORD state = pInfo->bMoving + pInfo->MoveMode;

		return IsObjectStateChanged(property,state);
	}

	BOOL CheckRectangle(const CCondition& condition, const CMessage& msg)
	{
		CObject* const object = g_pUserTable->FindUser(msg.GetValue(eProperty_ObjectIndex));

		if(0 == object)
		{
			return FALSE;
		}

		VECTOR3 objectPosition = {0};
		object->GetPosition(&objectPosition);

		float x1 = 100.0f * condition.GetValue(eProperty_RectangleLeft);
		float z1 = 100.0f * condition.GetValue(eProperty_RectangleTop);
		float x2 = 100.0f * condition.GetValue(eProperty_RectangleRight);
		float z2 = 100.0f * condition.GetValue(eProperty_RectangleBottom);
		VECTOR3 point1 = {x1, 0, z1};
		VECTOR3 point2 = {x2, 0, z2};

		if(x1 > x2)
		{
			point1.x = x2;
			point2.x = x1;
		}

		if(z1 > z2)
		{
			point1.z = z2;
			point2.z = z1;
		}

		BOOL isOnOfSquare = TRUE;

		if(objectPosition.x < point1.x)
		{
			isOnOfSquare = FALSE;
		}
		else if(objectPosition.z < point1.z)
		{
			isOnOfSquare = FALSE;
		}
		else if(objectPosition.x > point2.x)
		{
			isOnOfSquare = FALSE;
		}
		else if(objectPosition.z > point2.z)
		{
			isOnOfSquare = FALSE;
		}

		const stProperty& property = condition.GetProperty(eProperty_Rectangle);
		return IsTrue(
			property,
			isOnOfSquare);
	}

	BOOL CheckMoveMode(const CCondition& condition, const CMessage& msg)
	{
		const DWORD dwObjectIdx = msg.GetValue(eProperty_ObjectIndex);

		CObject* const pObject = (CObject*)g_pUserTable->FindUser(dwObjectIdx);
		if( !pObject )
		{
			return FALSE;
		}

		BASEMOVE_INFO* const pMoveInfo = pObject->GetMoveInfo();
		if( !pMoveInfo )
		{
			return FALSE;
		}

		// 0:뛰기 1:걷기
		const stProperty& property = condition.GetProperty(eProperty_MoveMode);
		return IsTrue( property, pMoveInfo->MoveMode );
	}

	BOOL CheckMoving(const CCondition& condition, const CMessage& msg)
	{
		const DWORD dwObjectIdx = msg.GetValue(eProperty_ObjectIndex);

		CObject* const pObject = (CObject*)g_pUserTable->FindUser(dwObjectIdx);
		if( !pObject )
		{
			return FALSE;
		}

		BASEMOVE_INFO* const pMoveInfo = pObject->GetMoveInfo();
		if( !pMoveInfo )
		{
			return FALSE;
		}

		const stProperty& property = condition.GetProperty(eProperty_Moving);
		return IsTrue( property, pMoveInfo->bMoving );
	}

	// 100818 ShinJS 현재 위치의 타일 속성값을 비교한다.
	BOOL CheckTtbProperty(const CCondition& condition, const CMessage& msg)
	{
		const DWORD dwObjectIdx = msg.GetValue(eProperty_ObjectIndex);

		CObject* const pObject = (CObject*)g_pUserTable->FindUser(dwObjectIdx);
		if( !pObject )
		{
			return FALSE;
		}

		CTileManager* pTileManager = g_pServerSystem->GetMap()->GetTileManager();
		if( pTileManager == NULL )
		{
			return FALSE;
		}

		CFixedTileInfo* pTileInfo = pTileManager->GetFixedTileInfo( pObject );
		if( pTileInfo == NULL )
		{
			return FALSE;
		}

		VECTOR3 objectPosition={0};
		pObject->GetPosition( &objectPosition );

		int tileposX = (int)(objectPosition.x / TILECOLLISON_DETAIL);
		int tileposZ = (int)(objectPosition.z / TILECOLLISON_DETAIL);

		const FIXEDTILE_ATTR& tileAttr = pTileInfo->GetTileAttrib( tileposX, tileposZ );
		const stProperty& property = condition.GetProperty(eProperty_TtbProperty);

		return IsTrue( property, tileAttr.uFixedAttr );
	}

	BOOL CheckRangeFlag(const CCondition& condition, const CMessage& msg)
	{
		const stProperty& propertyRangeFlag = condition.GetProperty(eProperty_RangeFlag);
		const stProperty& propertyRangeMin = condition.GetProperty(eProperty_RangeMin);
		const stProperty& propertyRangeMax = condition.GetProperty(eProperty_RangeMax);

		const int flag = CManager::GetInstance().GetFlag(
			condition.GetParameter().mOwnerType,
			condition.GetParameter().mOwnerIndex,
			propertyRangeFlag.mValue);

		if(eProperty_RangeMax == propertyRangeMax.mType)
		{
			if(FALSE == IsTrue(propertyRangeMax, flag))
			{
				return FALSE;
			}
		}

		if(eProperty_RangeMin == propertyRangeMin.mType)
		{
			if(FALSE == IsTrue(propertyRangeMin, flag))
			{
				return FALSE;
			}
		}

		return TRUE;
	}
}