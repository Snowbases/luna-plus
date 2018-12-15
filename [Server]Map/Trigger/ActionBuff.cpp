#include "StdAfx.h"
#include "ActionBuff.h"
#include "common.h"
#include "Trigger.h"
#include "Manager.h"
#include "..\UserTable.h"
#include "..\MapDBMsgParser.h"
#include "..\..\[cc]skill\Server\Info\BuffSkillInfo.h"
#include "..\..\[cc]skill\server\manager\skillmanager.h"
#include "..\CharMove.h"
#include "..\PackedData.h"
#include "./Dungeon/DungeonMgr.h"

namespace Trigger
{
	void CActionBuff::DoAction()
	{
		const DWORD buffSkillIndex = GetValue(eProperty_SkillIndex);
		const cBuffSkillInfo* const skillInfo = SKILLMGR->GetBuffInfo(buffSkillIndex);

		if(0 == skillInfo ||
			skillInfo->GetKind() != SKILLKIND_BUFF)
		{
			return;
		}

		float fRandomRad = 0.f;
		const int randomAnlgeRange = GetValue( eProperty_BuffRandomAnlgeRange );
		if( randomAnlgeRange > 0 )
		{
			fRandomRad = DEGTORAD( (rand() % (randomAnlgeRange * 2)) - randomAnlgeRange );
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
			else if(eObjectKind_Player == object->GetObjectKind())
			{
				// 100419 ONS 인던 스위치버프를 저장한다.
				// 100720 NYJ 플레이어에게 적용되는 스위치버프만 저장한다.
				DungeonMGR->StoreSwitchBuff( GetChannelID(), buffSkillIndex );	
			}

			// 100818 ShinJS 회전이 설정된 경우 대상의 방향에서 지정된 범위로 회전시킨다.
			VECTOR3 dir={0,0,-1};
			if( randomAnlgeRange > 0 )
			{
				MOVE_INFO* pMoveInfo = (MOVE_INFO*)object->GetMoveInfo();
				if( pMoveInfo == NULL )
					continue;

				RotateVectorAxisY( &dir, &pMoveInfo->Move_Direction, fRandomRad );
			}

			SKILLMGR->BuffSkillStart(
				objectIndex,
				buffSkillIndex,
				skillInfo->GetInfo().DelayTime,
				skillInfo->GetInfo().Count,
				&dir );

			if( skillInfo->GetInfo().NoUpdate )
				continue;

			// 플레이어만 버프 정보를 DB에 저장한다
			if(eObjectKind_Player == object->GetObjectKind())
			{
				CharacterBuffAdd(
					objectIndex,
					buffSkillIndex,
					skillInfo->GetInfo().DelayTime ? skillInfo->GetInfo().DelayTime : 1,
					skillInfo->GetInfo().Count,
					skillInfo->GetInfo().IsEndTime );
			}
		}
	}

	void CActionActiveSkill::DoAction()
	{
		const DWORD dwActiveSkillidx = GetValue(eProperty_SkillIndex);
		const float fRad = DEGTORAD( (float)GetValue(eProperty_Angle) );
		const VECTOR3 dir = GetRadToVector( fRad );

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}

			// 정지
			VECTOR3* pos = CCharMove::GetPosition( object );
			CCharMove::EndMove( object, gCurTime, pos );
			MOVE_POS msg;
			msg.Category = MP_MOVE;
			msg.Protocol = MP_MOVE_STOP;
			msg.dwMoverID = objectIndex;
			msg.cpos.Compress(pos);
			PACKEDDATA_OBJ->QuickSend( object, &msg, sizeof(msg) );

			// 스킬 발동
			SKILLMGR->CreateActiveSkill( dwActiveSkillidx, objectIndex, objectIndex, dir );
		}
	}
}
