#include "stdafx.h"
#include ".\skillmanager.h"
#include "../Info/ActiveSkillInfo.h"
#include "../Info/BuffSkillInfo.h"
#include "../Delay/Delay.h"
#include "../Tree/SkillTree.h"
#include "../[CC]Header/GameResourceManager.h"
#include "UserTable.h"
#include "MHFile.h"
#include "Player.h"
// 080708 LUJ, 광역 스킬을 사용하는 함정 처리를 위해 참조
#include "Trap.h"
#include "../[CC]Skill/Server/Object/BuffSkillObject.h"
#include "CharMove.h"
#include "PackedData.h"

#include "BattleSystem_Server.h"
#include "MapDBMsgParser.h"
#include "SkillTreeManager.h"

#include "ObjectStateManager.h"

#include "ItemManager.h"
// 080602 LUJ, 전서버 대상 스킬 기능위해 참조
#include "Guild.h"
#include "GuildManager.h"
#include "PartyManager.h"
#include "Party.h"
#include "..\hseos\Family\SHFamilyManager.h"
#include "pet.h"
#include "MoveManager.h"
#include "SiegeWarfareMgr.h"
// 080821 KTH -- SIegeDungeonManager Include
#include "../[CC]SiegeDungeon/SiegeDungeonMgr.h"
#include "VehicleManager.h"
// 081006 LYW --- SkillManager : 공성전 캐터펄트 공격 처리에 필요한 헤더 포함.
#include "./SiegeRecallMgr.h"
#include "./SiegeWarfareMgr.h"
// 081209 LUJ, 스킬 타겟 정상을 체크할 때 길드전 여부도 검사하기 위해 참조
#include "GuildFieldWarMgr.h"

cSkillManager::cSkillManager(void)
{
}

cSkillManager::~cSkillManager(void)
{
}

void cSkillManager::Init()
{
	/// Hash 테이블 초기화
	m_SkillInfoTable.Initialize(1000);
	m_SkillObjectTable.Initialize(10000);
	m_SkillTreeTable.Initialize(64);

	/// 스킬 리스트 로딩
	LoadSkillInfoList();
}

void cSkillManager::Release()
{
	/// 스킬 오브젝트 삭제
	cSkillObject* pObject;
	m_SkillObjectTable.SetPositionHead();
	while((pObject = m_SkillObjectTable.GetData())!= NULL)
	{
		ReleaseSkillObject(pObject);
	}
	m_SkillObjectTable.RemoveAll();

	/// 스킬 정보 삭제
	cSkillInfo* pSInfo = NULL;
	m_SkillInfoTable.SetPositionHead();
	while((pSInfo = m_SkillInfoTable.GetData())!= NULL)
	{
		delete pSInfo;
	}
	m_SkillInfoTable.RemoveAll();

	SKILL_TREE_INFO* pInfo = NULL;

	m_SkillTreeTable.SetPositionHead();
	while((pInfo = m_SkillTreeTable.GetData())!= NULL)
	{
		delete pInfo;
	}
	m_SkillTreeTable.RemoveAll();

}


void cSkillManager::LoadSkillInfoList()
{
	CMHFile file;
	file.Init("System/Resource/SkillList.bin","rb");

	while(FALSE == file.IsEOF())
	{
		TCHAR text[MAX_PATH] = {0};
		file.GetLine(
			text,
			sizeof(text) / sizeof(*text));

		cActiveSkillInfo* const pInfo = new cActiveSkillInfo;
		pInfo->Parse(text);

		if( pInfo->GetIndex() == 0 )
		{
			delete pInfo;
			continue;
		}

		m_SkillInfoTable.Add(pInfo,pInfo->GetIndex());

		// 091211 LUJ, 스킬 개수를 센다
		const DWORD index = pInfo->GetIndex() / 100 * 100 + 1;
		++mSkillSize[index];
	}

	file.Release();
	file.Init("System/Resource/Skill_Buff_List.bin","rb");	

	while(FALSE == file.IsEOF())
	{
		TCHAR text[MAX_PATH] = {0};
		file.GetLine(
			text,
			sizeof(text) / sizeof(*text));

		cBuffSkillInfo* const pInfo = new cBuffSkillInfo;
		pInfo->Parse(text);

		if( pInfo->GetIndex() == 0 )
		{
			delete pInfo;
			continue;
		}

		m_SkillInfoTable.Add(pInfo,pInfo->GetIndex());

		// 091211 LUJ, 스킬 개수를 센다
		const DWORD index = pInfo->GetIndex() / 100 * 100 + 1;
		++mSkillSize[index];
	}

	file.Release();
	file.Init("System/Resource/skill_get_list.bin","rb");

	while(FALSE == file.IsEOF())
	{
		SKILL_TREE_INFO* pInfo = new SKILL_TREE_INFO;

		pInfo->ClassIndex = file.GetWord();
		pInfo->Count = file.GetWord();

		for( WORD cnt = 0; cnt < pInfo->Count; cnt++ )
		{
			pInfo->SkillIndex[ cnt ] = file.GetDword();
		}

		m_SkillTreeTable.Add( pInfo, pInfo->ClassIndex );
	}

	file.Release();
}

cSkillObject* cSkillManager::GetSkillObject(DWORD SkillObjID)
{
	return (cSkillObject*)m_SkillObjectTable.GetData(SkillObjID);
}

cSkillInfo* cSkillManager::GetSkillInfo(DWORD SkillInfoIdx)
{
	return (cSkillInfo*)m_SkillInfoTable.GetData(SkillInfoIdx);
}

void cSkillManager::Process()
{
	for( std::list< DWORD >::iterator processIter = mProcessList.begin() ; processIter != mProcessList.end() ; ++processIter )
	{
		const DWORD objectIndex = *processIter;
		cSkillObject* const skillObject = GetSkillObject(objectIndex);
		if( 0 == skillObject ||
			SKILL_STATE_DESTROY == skillObject->Update() )
		{
			ReleaseSkillObject(skillObject);
			processIter = mProcessList.erase( processIter );
			if( processIter == mProcessList.end() )
				break;

			continue;
		}
	}
}

void cSkillManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch(Protocol)
	{
	case MP_SKILL_START_SYN:
		{
			MSG_SKILL_START_SYN* pmsg = ( MSG_SKILL_START_SYN* )pMsg;
				
			CObject* object = g_pUserTable->FindUser( pmsg->Operator );
		
			if( ! object )
			{
				break;
			}

			// 080904 LYW --- SkillManager : 공성전 중 사용할 수 없는 스킬 체크.
			if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
				SIEGEWARFAREMGR->IsSiegeWarfareZone() &&
				SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
			{
				if( SIEGEWARFAREMGR->IsFobiddenSkill(pmsg->SkillIdx) ) return ;
			}

			SKILL_RESULT result = SKILL_FAIL_ETC;

			if( object->GetObjectKind() == eObjectKind_Player )
				result = OnSkillStartSyn( pmsg );
			else if( object->GetObjectKind() == eObjectKind_Pet )
				result = OnPetSkillStartSyn( pmsg );

			// 080610 LUJ, 스킬 실패 시 메시지를 반환하도록 함
			if( result != SKILL_SUCCESS )
			{
				CObject* object = g_pUserTable->FindUser( pmsg->Operator );

				if( ! object )
				{
					break;
				}

				MSG_DWORD2 message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_SKILL;
				message.Protocol	= MP_SKILL_START_NACK;
				message.dwData1		= pmsg->SkillIdx;
				message.dwData2		= result;

				object->SendMsg( &message, sizeof( message ) );
			}
		}
		break;
	// 080602 LUJ, 전서버 대상 스킬이 전파됨
	case MP_SKILL_START_TO_MAP:
		{
			const MSG_ITEM_SKILL_START_TO_MAP* const receivedMessage = (MSG_ITEM_SKILL_START_TO_MAP*)pMsg;
			const cActiveSkillInfo* const skillInfo = GetActiveInfo( receivedMessage->mSkill.wSkillIdx );

			if(0 == skillInfo ||
				TARGET_KIND_WORLD != skillInfo->GetInfo().Area)
			{
				break;
			}

			typedef std::set< DWORD >	ObjectIndexSet;
			ObjectIndexSet				objectIndexSet;

			switch(skillInfo->GetInfo().AreaTarget)
			{
			case eSkillAreaTargetGuild:
				{
					GUILDMEMBERINFO memberInfo[ MAX_GUILD_MEMBER ] = { 0 };
					CGuild*	guild = GUILDMGR->GetGuild( receivedMessage->dwObjectID );

					if( ! guild )
					{
						break;
					}

					guild->GetTotalMember( memberInfo );

					for(
						DWORD size = sizeof( memberInfo ) / sizeof( *memberInfo );
						size--; )
					{
						objectIndexSet.insert( memberInfo[ size ].MemberIdx );
					}

					break;
				}
			case eSkillAreaTargetFamily:
				{
					const DWORD familyIndex	 = receivedMessage->dwObjectID;
					// 080602 LUJ, 패밀리는 맵 서버에 데이타가 없고, 에이전트에 있다. 번거롭지만
					//				DB에 회원 목록을 쿼리해서 버프를 적용하도록 하자.
					// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
					g_DB.FreeMiddleQuery(
						RSkillAddToFamily,
						skillInfo->GetIndex(),
						"EXEC dbo.MP_FAMILY_MEMBER_LOADINFO %d",
						familyIndex );
					break;
				}
			case eSkillAreaTargetParty:
				{
					const DWORD	partyIndex	= receivedMessage->dwObjectID;
					CParty*		party		= PARTYMGR->GetParty( partyIndex );

					if( ! party )
					{
						break;
					}

					for( DWORD i = 0; i < MAX_PARTY_LISTNUM; ++i )
					{
						objectIndexSet.insert(party->GetMemberID(i));
					}

					break;
				}
			}

			// 080602 LUJ, 선택된 집단을 대상으로 버프 스킬을 적용한다
			for(
				ObjectIndexSet::const_iterator it = objectIndexSet.begin();
				objectIndexSet.end() != it;
				++it )
			{
				CPlayer* player = ( CPlayer* )g_pUserTable->FindUser( *it );

				if( !	player ||
						player->GetObjectKind() != eObjectKind_Player )
				{
					continue;
				}

				AddBuffSkill(
					*player,
					skillInfo->GetInfo());
			}

			break;
		}
	case MP_SKILL_UPDATE_TARGET_SYN:
		{
			MSG_SKILL_UPDATE_TARGET* pmsg = ( MSG_SKILL_UPDATE_TARGET* )pMsg;

			OnSkillTargetUpdate( pmsg );
		}
		break;
	case MP_SKILL_CANCEL_SYN:
		{
			MSG_DWORD* pmsg = ( MSG_DWORD* )pMsg;
			OnSkillCancel( pmsg );
		}
		break;
		// 091127 LUJ, 플레이어를 소환한다
	case MP_SKILL_RECALL_SYN:
		{
			const MSG_NAME_DWORD3* const receivedMessage = (MSG_NAME_DWORD3*)pMsg;
			const DWORD	recallPlayerIndex = receivedMessage->dwObjectID;
			const DWORD	targetPlayerinex = receivedMessage->dwData1;
			const DWORD	skillIndex = receivedMessage->dwData2;
			const MAPTYPE targetMap = MAPTYPE(receivedMessage->dwData3);
			LPCTSTR targetPlayerName = receivedMessage->Name;

			Recall(
				recallPlayerIndex,
				targetPlayerinex,
				targetPlayerName,
				targetMap,
				skillIndex);
		}
		break;
	// 100211 ONS 부활계열 스킬 사용시 대상자에게 부활의사를 묻는 처리 추가
	case MP_SKILL_RESURRECT_ACK:
		{
			MSGBASE* pmsg = ( MSGBASE* )pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );

			if( ! pPlayer || eObjectKind_Player != pPlayer->GetObjectKind() )
			{
				break;
			}

			const cActiveSkillInfo* const pSkillInfo = GetActiveInfo(
				pPlayer->GetCurResurrectIndex());

			if(0 == pSkillInfo)
			{
				break;
			}
			
			// 부활대상이 수락했을 경우, 부활 처리를 실행한다.
			EXPTYPE exp = pPlayer->OnResurrect();
			exp = (EXPTYPE)(exp * ( pSkillInfo->GetInfo().UnitData / 100.f )) ;
			if( exp )
			{
				pPlayer->AddPlayerExpPoint( exp );
			}
			pPlayer->SetCurResurrectIndex( 0 );	
		}
		break;
	// 부활 스킬에 의한 부활을 거부했을 경우
	case MP_SKILL_RESURRECT_NACK:
		{
			MSGBASE* pmsg = ( MSGBASE* )pMsg;
			CObject* object = g_pUserTable->FindUser( pmsg->dwObjectID );
			if( ! object || eObjectKind_Player != object->GetObjectKind() )
			{
				break;
			}

			CPlayer* pPlayer = (CPlayer*)object;
			pPlayer->SetCurResurrectIndex( 0 );	
		}
		break;
	}
}

// 080602 LUJ, 버프 스킬을 적용한다
// 080616 LUJ, CPlayer -> CObject로 변경
void cSkillManager::AddBuffSkill( CObject& object, const ACTIVE_SKILL_INFO& skillInfo )
{
	for(
		DWORD size = sizeof( skillInfo.Buff ) / sizeof( *( skillInfo.Buff ) );
		size--; )
	{
		const DWORD	buffSkillIndex = skillInfo.Buff[ size ];
		const cBuffSkillInfo* const info = GetBuffInfo( buffSkillIndex );

		if( !	info ||
				info->GetKind() != SKILLKIND_BUFF )
		{
			continue;
		}

		BuffSkillStart(
			object.GetID(),
			info->GetInfo().Index,
			info->GetInfo().DelayTime ? info->GetInfo().DelayTime : 1,
			info->GetInfo().Count );

		if( info->GetInfo().NoUpdate )
			return;

		// 080602 LUJ, DelayTime을 최소 1이상 준 이유: CBuffSkillObject::ProcessStart()의 버프 스킬 생성 부분에서 참조함
		CharacterBuffAdd(
			object.GetID(),
			info->GetInfo().Index,
			info->GetInfo().DelayTime ? info->GetInfo().DelayTime : 1,
			info->GetInfo().Count,
			info->GetInfo().IsEndTime );
	}
}

SKILL_RESULT cSkillManager::OnSkillStartSyn(MSG_SKILL_START_SYN* pmsg, ITEMBASE* pItemBase)
{
	CPlayer* const pOperator = (CPlayer*)g_pUserTable->FindUser(pmsg->Operator);

	if(0 == pOperator)
	{
		return SKILL_OPERATOR_INVALUED;
	}
	else if(eObjectKind_Player != pOperator->GetObjectKind())
	{
		return SKILL_OPERATOR_INVALUED;
	}
	else if(0 < pOperator->CurCastingSkillID)
	{
		return SKILL_OPERATOR_INVALUED;
	}
	else if(eBATTLE_KIND_GTOURNAMENT == pOperator->GetBattle()->GetBattleKind())
	{
		if(eBattleTeam_Max == pOperator->GetBattleTeam())
		{
			return SKILL_STATE_INVALUED;
		}
	}

	const cActiveSkillInfo* const pSkillInfo = SKILLMGR->GetActiveInfo( pmsg->SkillIdx );

	if(0 == pSkillInfo)
	{
		return SKILL_INFO_INVALUED;
	}
	else if(FALSE == pSkillInfo->IsExcutableSkillState(*pOperator, pmsg->mConsumeItem))
	{
		return SKILL_STATE_INVALUED;
	}
	else if(pOperator->IsCoolTime(pSkillInfo->GetInfo()))
	{
		return SKILL_COOLING;
	}
	else if(0 ==  pItemBase)
	{
		const SkillScript& skillScript = GAMERESRCMNGR->GetSkillScript( pmsg->SkillIdx );

		if( skillScript.mIsNeedCheck )
		{
			const SKILL_BASE* const skillBase = pOperator->GetSkillTree().GetData(
				pmsg->SkillIdx / 100 * 100 + 1 );

			if( ! skillBase )
			{
				LogScriptHack(
					*pOperator,
					"SkillList.bin",
					"*player has no skill" );
				return SKILL_OPERATOR_INVALUED;
			}

			const BYTE skillLevelFromClient = BYTE( pmsg->SkillIdx % 100 );
			const BOOL isWrongSkillLevel = ( skillLevelFromClient > skillBase->Level );

			if( isWrongSkillLevel )
			{
				LogScriptHack(
					*pOperator,
					"SkillList.bin",
					"*wrong level" );
				return SKILL_OPERATOR_INVALUED;
			}
		}
	}
	// 080410 KTH -- 아이템 사용으로 스킬을 사용할 경우 아이템의 정보를 확인한다.
	else
	{
		const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo( pItemBase->wIconIdx );

		if(0 == itemInfo)
		{
			return SKILL_OPERATOR_INVALUED;
		}
		else if(pSkillInfo->GetIndex() != itemInfo->SupplyValue)
		{
			return SKILL_OPERATOR_INVALUED;
		}
	}

	switch(pSkillInfo->GetInfo().Unit)
	{
	case UNITKIND_PHYSIC_ATTCK:
		{
			if(pOperator->GetAbnormalStatus()->IsBlockAttack)
			{
				return SKILL_STATE_INVALUED;
			}

			break;
		}
	default:
		{
			if(pOperator->GetAbnormalStatus()->IsSilence)
			{
				return SKILL_STATE_INVALUED;
			}

			break;
		}
	}

	OBJECTSTATEMGR_OBJ->EndObjectState(
		pOperator,
		eObjectState_Immortal);

	VECTOR3 TargetPos = {0};
	CObject* pTargetObject = 0;

	if(FALSE == GetMainTargetPos(&pmsg->MainTarget, &TargetPos, &pTargetObject))
	{
		return SKILL_TARGET_INVALUED;
	}

	switch(pTargetObject ? pTargetObject->GetObjectKind() : eObjectKind_None)
	{
	case eObjectKind_Player:
		{
			CPlayer* const targetPlayer = (CPlayer*)pTargetObject;

			if(targetPlayer->GetImmortalTime() &&
				targetPlayer->GetBattleTeam() != pOperator->GetBattleTeam())
			{
				return SKILL_TARGET_IMMORTALED;
			}
			else if(pOperator->IsPKMode())
			{
				break;
			}
			else if(targetPlayer->IsPKMode())
			{
				break;
			}
			else if(SIEGEWARFAREMGR->IsSiegeWarfareZone(g_pServerSystem->GetMapNum()))
			{
				break;
			}
			else if(SIEGEDUNGEONMGR->IsSiegeDungeon(g_pServerSystem->GetMapNum()))
			{
				break;
			}
			else if(GUILDWARMGR->IsGuildWar(targetPlayer->GetGuildIdx()) &&
				GUILDWARMGR->IsGuildWar(pOperator->GetGuildIdx()))
			{
				break;
			}
			else if(UNITKIND_PHYSIC_ATTCK != pSkillInfo->GetInfo().Unit &&
				UNITKIND_MAGIC_ATTCK != pSkillInfo->GetInfo().Unit)
			{
				if(UNITKIND_RESURRECTION == pSkillInfo->GetInfo().Unit &&
					eObjectState_Die != OBJECTSTATEMGR_OBJ->GetObjectState(pTargetObject))
				{
					return SKILL_TARGET_INVALUED;
				}

				break;
			}
			else if(eBATTLE_KIND_NONE != pOperator->GetBattle()->GetBattleKind() &&
				eBATTLE_KIND_NONE != targetPlayer->GetBattle()->GetBattleKind())
			{
				break;
			}
			else if(pTargetObject != pOperator)
			{
				return SKILL_TARGET_INVALUED;
			}

			for(CTargetListIterator iter(&pmsg->TargetList);
				0 < iter.GetNextTarget();)
			{
				CObject* const pObject = g_pUserTable->FindUser(
					iter.GetTargetID());

				if(0 == pObject)
				{
					continue;
				}
				else if(eObjectKind_Player != pObject->GetObjectKind())
				{
					continue;
				}

				return SKILL_TARGET_INVALUED;
			}

			break;
		}
	case eObjectKind_Monster:
	case eObjectKind_BossMonster:
	case eObjectKind_SpecialMonster:
	case eObjectKind_FieldBossMonster:
	case eObjectKind_FieldSubMonster:
	case eObjectKind_ToghterPlayMonster:
	case eObjectKind_ChallengeZoneMonster:
		{
			pOperator->SetLastActionTime(gCurTime);
			break;
		}
	}

	switch(pSkillInfo->GetInfo().Target)
	{
	case eSkillTargetKind_None:
		{
			return SKILL_TARGET_INVALUED;
		}
	case eSkillTargetKind_OnlyOthers:
		{
			if( pTargetObject == pOperator )
			{
				return SKILL_TARGET_INVALUED;
			}

			break;
		}
	case eSkillTargetKind_OnlySelf:
		{
			if( pTargetObject != pOperator )
			{
				return SKILL_TARGET_INVALUED;
			}

			break;
		}
	}

	const float TargetRadius = (pTargetObject ? pTargetObject->GetRadius() : 0);

	if(pSkillInfo->IsInRange(*pOperator, TargetPos, TargetRadius) == FALSE )
	{
		return SKILL_RANGE_NOTENOUGH;
	}
	else if(SKILLKIND_ONOFF == pSkillInfo->GetKind())
	{
		BOOL destroy = FALSE;

		for(WORD i = 0; i < MAX_BUFF_COUNT; ++i)
		{
			const DWORD idx = pSkillInfo->GetInfo().Buff[ i ];
			cSkillObject* const pSkill = pTargetObject->GetBuffList().GetData( idx / 100 );

			if(0 == pSkill)
			{
				continue;
			}
			else if(pSkill->GetSkillIdx() > pSkillInfo->GetInfo().Buff[ i ])
			{
				continue;
			}

			pSkill->SetEndState();
			pSkill->EndState();
			destroy = TRUE;
		}

		if( destroy )
		{
			// 100615 ShinJS --- 스킬제거 정보를 전송한다.
			MSG_DWORD msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_SKILL;
			msg.Protocol = MP_SKILL_START_DESTROY_ACK;
			msg.dwObjectID = pOperator->GetID();
			msg.dwData = pmsg->mConsumeItem.dwDBIdx;

			pOperator->SendMsg( &msg, sizeof(msg) );

			return SKILL_SUCCESS;
		}
	}

	// 090629 ONS 현재 실행한 스킬과 기존에 실행중인 버프스킬이 같은 그룹이 존재할 경우 해당 버프스킬을 제거한다.
	// 마족의 가호계열(베르제의 가호, 루피아의 가호, 파르세이카의 가호)의 버프스킬의 경우, 
	// 스킬 발동시 기존에 실행중이던 가호 버프스킬이 있다면 해제하고 실행해야한다. 
	// 확장을 위해 가호계열을 하나의 그룹으로 지정하고 다른 그룹에 영향을 받지 않고 제거할 수 있도록 처리.
	RemoveGroupBuffSkill(
		pTargetObject,
		pSkillInfo->GetInfo());

	// 080602 LUJ, 전역 스킬은 DB에서 직접 처리한 후 전송된다. 취소 등을 위해 오브젝트 ID를 직접 받아온다
	const DWORD skillObjectIndex = GetNewSkillObjectID();

	// 길드/패밀리 포인트 등 동기화가 필요한 경우에만 처리한다. 이러한 것들은 일단 스킬을 발동시키고
	// 조건이 적합지 않을 때 취소한다. 따라서 스킬을 취소해도 소모가 된다.
	switch(pSkillInfo->GetInfo().mConsumePoint.mType)
	{
	case ACTIVE_SKILL_INFO::ConsumePoint::TypeFamily:
	case ACTIVE_SKILL_INFO::ConsumePoint::TypeGuild:
		{
			const ACTIVE_SKILL_INFO::ConsumePoint& consumePoint = pSkillInfo->GetInfo().mConsumePoint;

			g_DB.FreeMiddleQuery(
				RSkillCheckUsable,
				skillObjectIndex,
				"EXEC dbo.MP_SKILL_CHECK_USABLE %d, %d, %d, %d",
				pOperator->GetID(),
				pSkillInfo->GetIndex(),
				ACTIVE_SKILL_INFO::ConsumePoint::TypeFamily == consumePoint.mType ? consumePoint.mValue : 0,
				ACTIVE_SKILL_INFO::ConsumePoint::TypeGuild == consumePoint.mType ? consumePoint.mValue : 0);
			break;
		}
	}

	cSkillObject* const pSObj = pSkillInfo->GetSkillObject();

	if(0 == pSObj)
	{
		return SKILL_INFO_INVALUED;
	}

	sSKILL_CREATE_INFO info;
	info.level = pSkillInfo->GetLevel();
	info.operatorId = pOperator->GetID();
	info.mainTarget = pmsg->MainTarget;
	info.skillDir = pmsg->SkillDir;	
	// 080602 LUJ, DB 업데이트 실패 시 스킬 오브젝트를 취소할 수 있도록 위에서 인덱스를 미리 받아놓도록 했다
	info.skillObjectId = skillObjectIndex;
	info.pos = (pSkillInfo->GetInfo().Target ? TargetPos : *CCharMove::GetPosition(pOperator));
	info.usedItem = pmsg->mConsumeItem;

	// 090108 LUJ, 멀티 타겟을 설정할 때 유효 여부를 검사하므로, 먼저 초기화를 해야한다.
	pSObj->Init( &info );
	pSObj->SetMultiTarget( &( pmsg->TargetList ) );
	pSObj->SetBattle( pOperator->GetBattleID(), pOperator->GetBattleTeam() );
	pSObj->SetGridID( pOperator->GetGridID() );

	/// 테이블에 등록
	m_SkillObjectTable.Add( pSObj, pSObj->GetID() );
	mProcessList.push_back(pSObj->GetID());
	g_pServerSystem->AddSkillObject( pSObj, &info.pos );

	pOperator->SetCurrentSkill(pSObj->GetID());
	pOperator->SetCoolTime(pSkillInfo->GetInfo());

	/// 스킬 생성 성공
	return SKILL_SUCCESS;
}

void cSkillManager::MonsterAttack(DWORD SkillIdx,CObject* pMonster,CObject* pTarget)
{
	// 081006 LYW --- AttackSkillUnit : 공성전의 캐터펄트가 성 마스터 길드는 공격하지 않도록 처리.
	if( !pMonster || !pTarget ) return ;

	// 공성 존인지 확인한다.
	if( SIEGEWARFAREMGR->IsSiegeWarfareZone() )
	{
		const EObjectKind byOperatorKind = pMonster->GetObjectKind();
		const EObjectKind byTargetKind	= pTarget->GetObjectKind();

		if(byOperatorKind & eObjectKind_Monster)
		{
			// 시전 몬스터가 캐터펄트인지 확인한다.
			if( SIEGERECALLMGR->IsRecallMonster( pMonster->GetID() ) )
			{
				DWORD dwTargetGuildIdx = 0 ;
				DWORD dwMasterGuildIdx = 0 ;

				// 캐터펄트가 루센/제뷘의 마스터 길드를 구분하기 위해 종족 대표코드를 사용하여
				// 캐터 펄트 구분을 하기로 한다 --- October 6, 2008 by 송가람
				// MonsterRace == 100 ==> 루쉔(81) / MonsterRace == 101 ==> 제뷘(83)
				const BASE_MONSTER_LIST& pInfo = ((CMonster*)pMonster)->GetSMonsterList() ;

				if( pInfo.MonsterRace == 100 )
				{
					dwMasterGuildIdx = SIEGEWARFAREMGR->GetCastleGuildIdx(eNeraCastle_Lusen) ;
				}
				else if( pInfo.MonsterRace == 101 )
				{
					dwMasterGuildIdx = SIEGEWARFAREMGR->GetCastleGuildIdx(eNeraCastle_Zebin) ;
				}

				// 타겟이 Player 라면,
				if( byTargetKind == eObjectKind_Player )
				{
					dwTargetGuildIdx = ((CPlayer*)pTarget)->GetGuildIdx() ;
				}
				// 타겟이 Pet 이라면 마스터를 받아 길드 인덱스를 받고, 
				// 성의 마스터 길드인지 확인한다.
				else if( byTargetKind == eObjectKind_Pet )
				{
					CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(
						pTarget->GetOwnerIndex());

					if(0 == player)
					{
						dwTargetGuildIdx = 0;
					}
					else if(eObjectKind_Player == player->GetObjectKind())
					{
						dwTargetGuildIdx = player->GetGuildIdx();
					}
				}

				// 성의 마스터 길드와 같으면 공격을 하지 않는다.
				if( dwMasterGuildIdx != 0 )
				{
					if( dwMasterGuildIdx == dwTargetGuildIdx ) return ;
				}
			}
		}
	}

	const cActiveSkillInfo* const pSkillInfo = GetActiveInfo(SkillIdx);
	
	if(pSkillInfo == NULL)
		return;

	if( pMonster->GetAbnormalStatus()->IsSilence )
	{
		// 080616 LUJ, 침묵 상태인 경우, 물리 공격을 제외한 모든 스킬을 차단한다
		if( pSkillInfo->GetInfo().Unit != UNITKIND_PHYSIC_ATTCK )
			return;
	}
	if( pMonster->GetAbnormalStatus()->IsBlockAttack )
	{
		if( pSkillInfo->GetInfo().Unit == UNITKIND_PHYSIC_ATTCK )
			return;
	}
	
	VECTOR3* pMonPos = CCharMove::GetPosition(pMonster);
	VECTOR3* pTargetPos = CCharMove::GetPosition(pTarget);
	VECTOR3 dir = *pTargetPos - *pMonPos;

	// 스킬 오브젝트 생성
	sSKILL_CREATE_INFO info;
	cSkillObject* pSObj = pSkillInfo->GetSkillObject();

	/// 스킬 객체 ID
	info.skillObjectId = GetNewSkillObjectID();

	info.level = 1;
//	info.pOperator = pMonster;
	info.operatorId = pMonster->GetID();
	info.mainTarget.SetMainTarget( pTarget->GetID() );
	info.skillDir = dir;	
	info.remainTime = 0;
	/// 스킬 생성 위치
	if( pSkillInfo->GetInfo().Target )
	{
		info.pos = *CCharMove::GetPosition(pTarget);
	}
	else
	{
		info.pos = *CCharMove::GetPosition(pMonster);
	}
	
	// battle
	//info.pBattle = pMonster->GetBattle();

	// 테이블에 등록
	pSObj->Init( &info );
	pSObj->SetBattle( pMonster->GetBattleID(), pMonster->GetBattleTeam() );
	pSObj->SetGridID( pMonster->GetGridID() );

	// 080708 LUJ, 타겟 목록을 담기 위해 선언
	CTargetList targetList;
	targetList.Clear();

	switch( pSkillInfo->GetInfo().Area )
	{
	case TARGET_KIND_MULTI:
	case TARGET_KIND_MULTI_MONSTER:
	case TARGET_KIND_MULTI_ALL:
		{
			CUserTable::ObjectKindSet objectKindeSet;
			objectKindeSet.insert( eObjectKind_Monster );
			objectKindeSet.insert( eObjectKind_BossMonster );
			objectKindeSet.insert( eObjectKind_FieldBossMonster );
			objectKindeSet.insert( eObjectKind_FieldSubMonster );
			objectKindeSet.insert( eObjectKind_Trap );
			objectKindeSet.insert( eObjectKind_Player );

			g_pUserTable->GetTargetInRange(
				CCharMove::GetPosition( pTarget ),
				pSkillInfo->GetInfo().Range,
				&targetList,
				pMonster->GetGridID(),
				0,
				objectKindeSet );
			break;
		}
	}

	CTargetList fileteredTargetList;
	fileteredTargetList.Clear();

	// 소유주 유무에 따라 특정 대상을 제외한다
	{
		std::set< EObjectKind > filterContainer;

		if(CObject* const owner = g_pUserTable->FindUser(pMonster->GetOwnerIndex()))
		{
			if(eObjectKind_Player == owner->GetObjectKind())
			{
				filterContainer.insert(
					eObjectKind_Monster);
				filterContainer.insert(
					eObjectKind_BossMonster);
				filterContainer.insert(
					eObjectKind_SpecialMonster);
				filterContainer.insert(
					eObjectKind_FieldBossMonster);
				filterContainer.insert(
					eObjectKind_FieldSubMonster);
				filterContainer.insert(
					eObjectKind_ToghterPlayMonster);
				filterContainer.insert(
					eObjectKind_ChallengeZoneMonster);
				filterContainer.insert(
					eObjectKind_Trap);
			}
			else if(eObjectKind_Monster & owner->GetObjectKind())
			{
				filterContainer.insert(
					eObjectKind_Player);
				filterContainer.insert(
					eObjectKind_Pet);
			}
		}
		else
		{
			filterContainer.insert(
				eObjectKind_Player);
			filterContainer.insert(
				eObjectKind_Pet);
		}

		CTargetListIterator iterator( &targetList );
		CTargetListIterator iteratorFiltered( &fileteredTargetList );

		while( iterator.GetNextTarget() )
		{
			if(CObject* const object = g_pUserTable->FindUser( iterator.GetTargetID() ))
			{
				if(filterContainer.end() == filterContainer.find(object->GetObjectKind()))
				{
					continue;
				}

				iteratorFiltered.AddTargetWithNoData( 
					object->GetID(),
					SKILLRESULTKIND_NONE);
			}
		}

		iteratorFiltered.Release();
	}
	
	pSObj->SetMultiTarget( &fileteredTargetList );

	m_SkillObjectTable.Add( pSObj, pSObj->GetID() );
	mProcessList.push_back(pSObj->GetID());
	g_pServerSystem->AddSkillObject( pSObj, &info.pos );

	pMonster->SetCurrentSkill( pSObj->GetID() );
}

BOOL cSkillManager::PetAttack(DWORD SkillIdx,CObject* pPet,CObject* pTarget)
{
	const cActiveSkillInfo* const pSkillInfo = GetActiveInfo(SkillIdx);
	
	if(pSkillInfo == NULL)
		return SKILL_INFO_INVALUED;

	if( pPet->GetAbnormalStatus()->IsSilence )
	{
		// 080616 LUJ, 침묵 상태인 경우, 물리 공격을 제외한 모든 스킬을 차단한다
		if( pSkillInfo->GetInfo().Unit != UNITKIND_PHYSIC_ATTCK )
			return SKILL_STATE_INVALUED;
	}
	if( pPet->GetAbnormalStatus()->IsBlockAttack )
	{
		if( pSkillInfo->GetInfo().Unit == UNITKIND_PHYSIC_ATTCK )
			return SKILL_STATE_INVALUED;
	}
	
	VECTOR3* pPetPos = CCharMove::GetPosition(pPet);
	VECTOR3* pTargetPos = CCharMove::GetPosition(pTarget);
	VECTOR3 dir = *pTargetPos - *pPetPos;

	float dist = CalcDistanceXZ( pPetPos, pTargetPos ) - pTarget->GetRadius();

	if( dist > pSkillInfo->GetInfo().Range )
		return SKILL_RANGE_NOTENOUGH;

	// 스킬 오브젝트 생성
	sSKILL_CREATE_INFO info;
	cSkillObject* pSObj = pSkillInfo->GetSkillObject();

	/// 스킬 객체 ID
	info.skillObjectId = GetNewSkillObjectID();

	info.level = 1;
//	info.pOperator = pPet;
	info.operatorId = pPet->GetID();
	info.mainTarget.SetMainTarget( pTarget->GetID() );
	info.skillDir = dir;	
	info.remainTime = 0;
	/// 스킬 생성 위치
	if( pSkillInfo->GetInfo().Target )
	{
		info.pos = *CCharMove::GetPosition(pTarget);
	}
	else
	{
		info.pos = *CCharMove::GetPosition(pPet);
	}
	
	// battle
	//info.pBattle = pMonster->GetBattle();

	// 테이블에 등록
	pSObj->Init( &info );
	pSObj->SetBattle( pPet->GetBattleID(), pPet->GetBattleTeam() );
	pSObj->SetGridID( pPet->GetGridID() );

	m_SkillObjectTable.Add( pSObj, pSObj->GetID() );
	mProcessList.push_back(pSObj->GetID());
	g_pServerSystem->AddSkillObject( pSObj, &info.pos );

	pPet->SetCurrentSkill( pSObj->GetID() );

	return SKILL_SUCCESS;
}

cSkillObject* cSkillManager::OnBuffSkillStart( cSkillInfo* pSkillInfo, sSKILL_CREATE_INFO* pCreateInfo )
{
	CObject* pOperator = g_pUserTable->FindUser( pCreateInfo->operatorId );

	if( !pOperator )
		return NULL;

	CObject* pTargetObject = g_pUserTable->FindUser( pCreateInfo->mainTarget.dwMainTargetID );
	if( !pTargetObject )
	{
		return NULL;
	}

	// 100614 ShinJS --- 제거조건에 만족한 경우 버프를 생성하지 않는다.
	if( pSkillInfo->GetType() != cSkillInfo::TypeBuff )
		return NULL;

	const BUFF_SKILL_INFO& buffInfo = ((cBuffSkillInfo*)pSkillInfo)->GetInfo();
	BYTE buffRuleResult = eBUFF_START_RESULT_SUCCESS;

	if( pTargetObject->GetObjectBattleState() == eObjectBattleState_Peace && 
		buffInfo.mRulePeace == eBuffSkillActiveRule_Delete )
	{
		buffRuleResult = eBUFF_START_RESULT_PEACE_RULE_DELETE;
	}
	else if( pTargetObject->GetObjectBattleState() == eObjectBattleState_Battle &&
			 buffInfo.mRuleBattle == eBuffSkillActiveRule_Delete )
	{
		 buffRuleResult = eBUFF_START_RESULT_BATTLE_RULE_DELETE;
	}
	else if( CCharMove::IsMoving( pTargetObject ) == TRUE &&
			 buffInfo.mRuleMove == eBuffSkillActiveRule_Delete )
	{
		buffRuleResult = eBUFF_START_RESULT_MOVE_RULE_DELETE;
	}
	else if( CCharMove::IsMoving( pTargetObject ) == FALSE &&
			 buffInfo.mRuleStop == eBuffSkillActiveRule_Delete )
	{
		buffRuleResult = eBUFF_START_RESULT_STOP_RULE_DELETE;
	}
	else if( pTargetObject->GetState() == eObjectState_Rest &&
		buffInfo.mRuleRest == eBuffSkillActiveRule_Delete )
	{
		buffRuleResult = eBUFF_START_RESULT_REST_RULE_DELETE;
	}

	// 버프 해제조건만족시 대상자에게 알려줌.
	if( buffRuleResult != eBUFF_START_RESULT_SUCCESS )
	{
		MSG_DWORD2 msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category = MP_SKILL;
		msg.Protocol = MP_SKILL_BUFF_START_FAIL_NOTIFY;
		msg.dwObjectID = pTargetObject->GetID();
		msg.dwData1 = buffRuleResult;
		msg.dwData2 = buffInfo.Index;

		pTargetObject->SendMsg( &msg, sizeof(msg) );
		return NULL;
	}

	cSkillObject* const pSObj = pSkillInfo->GetSkillObject();
	if(0 == pSObj)
	{
		return 0;
	}

	/// 스킬 객체 ID
	pCreateInfo->skillObjectId = GetNewSkillObjectID();

	/// 스킬 객체 초기화
	pSObj->Init( pCreateInfo );
	pSObj->SetBattle( pOperator->GetBattleID(), pOperator->GetBattleTeam() );
	pSObj->SetGridID( pOperator->GetGridID() );

	/// 테이블에 등록
	m_SkillObjectTable.Add( pSObj, pSObj->GetID() );
	mProcessList.push_back(pSObj->GetID());
	g_pServerSystem->AddSkillObject( pSObj, &( pCreateInfo->pos ) );

	return pSObj;
}

// 090204 LUJ, count의 수치 범위를 확장함
void cSkillManager::BuffSkillStart( DWORD characteridx, DWORD skillidx, DWORD remaintime, int count, VECTOR3* pDir )
{
	CObject* pTarget = g_pUserTable->FindUser( characteridx );

	if( !pTarget )
	{
		return;
	}

	cSkillInfo* pSkillInfo = GetSkillInfo( skillidx );
	sSKILL_CREATE_INFO createinfo;

	createinfo.level = pSkillInfo->GetLevel();
	createinfo.operatorId = characteridx;
	createinfo.mainTarget.SetMainTarget(characteridx);
	createinfo.pos = *CCharMove::GetPosition(pTarget);
	createinfo.remainTime = remaintime;
	createinfo.count = count;
	createinfo.isNew = false;
	if( pDir != NULL )
	{
		createinfo.skillDir = *pDir;
	}

	if(cBuffSkillObject* const pBuff = (cBuffSkillObject*)OnBuffSkillStart( pSkillInfo, &createinfo ))
	{
		pTarget->AddEventSkill(*pBuff);
	}
}

void cSkillManager::AddPassive( CPlayer* pTarget, DWORD SkillIdx )
{
	const cActiveSkillInfo* const pActive = GetActiveInfo(
		SkillIdx);

	if(0 == pActive)
	{
		return;
	}
	
	for( DWORD cnt = 0; cnt < MAX_BUFF_COUNT; ++cnt)
	{
		const DWORD index = pActive->GetInfo().Buff[ cnt ];
		const cBuffSkillInfo* const pSkillInfo = GetBuffInfo( index );

		if( !pSkillInfo )
		{
			continue;
		}
		else if( BUFF_SKILL_INFO::Condition::TypeNone != pSkillInfo->GetInfo().mCondition.mType )
		{
			pTarget->AddSpecialSkill( pSkillInfo );
		}
		else if(pTarget->IsEnable(pSkillInfo->GetInfo()))
		{
			pSkillInfo->AddPassiveStatus( pTarget );
		}
	}
}

void cSkillManager::RemovePassive( CPlayer* pTarget, DWORD SkillIdx )
{
	const cActiveSkillInfo* const pActive = GetActiveInfo(
		SkillIdx);

	if(0 == pActive)
	{
		return;
	}

	for( DWORD cnt = 0; cnt < MAX_BUFF_COUNT; ++cnt)
	{
		const DWORD index = pActive->GetInfo().Buff[ cnt ];
		const cBuffSkillInfo* const pSkillInfo = GetBuffInfo( index );

		if( !pSkillInfo )
		{
			continue;
		}
		else if( BUFF_SKILL_INFO::Condition::TypeNone != pSkillInfo->GetInfo().mCondition.mType )
		{
			pTarget->RemoveSpecialSkill( pSkillInfo );
		}
		else if(FALSE == pTarget->IsEnable(pSkillInfo->GetInfo()))
		{
			pSkillInfo->RemovePassiveStatus( pTarget );
		}
	}
}

void cSkillManager::ReleaseSkillObject(cSkillObject* pSkillObject)
{
	/// 스킬을 삭제한다
	m_SkillObjectTable.Remove( pSkillObject->GetID() );
	g_pServerSystem->RemoveSkillObject( pSkillObject->GetID() );
	
	// 091211 LUJ, 스킬 오브젝트 재사용 과정에서 치명적인 메모리 누수가 있다
	//			정적 캐스팅으로 메모리 사용 크기에 관계없이 사용하여 비허용
	//			메모리에 대한 쓰기가 점진적으로 발생한다. 이를 피하기 위해
	//			일단 재사용을 하지 않도록 한다. 후에 좀더 안전하도록 스킬
	//			오브젝트 별로 정적 메모리 풀을 만들 예정이다
	SAFE_DELETE(pSkillObject);
}

DWORD cSkillManager::GetNewSkillObjectID()
{
	static tempID = SKILLOBJECT_ID_START;
	if(++tempID >= SKILLOBJECT_ID_MAX)
		tempID = SKILLOBJECT_ID_START;
	return tempID;
}


DWORD cSkillManager::GetSkillSize( DWORD index ) const
{
	SkillSize::const_iterator it = mSkillSize.find( index );

	return mSkillSize.end() == it ? 0 : it->second;
}


cSkillManager* cSkillManager::GetInstance()
{
	static cSkillManager instance;

	return &instance;
}


void cSkillManager::OnSkillTargetUpdate( MSG_SKILL_UPDATE_TARGET* pmsg )
{
	cSkillObject* pSkillObject = m_SkillObjectTable.GetData( pmsg->SkillObjectID );

	if(0 == pSkillObject)
	{
		return;
	}

	CObject* const object = pSkillObject->GetOperator();

	// 100115 LUJ, 다른 플레이어에 의해 타겟이 바뀌지 않도록 검사를 철저히 한다
	if(0 == object)
	{
		return;
	}
	else if(eObjectKind_Player != object->GetObjectKind())
	{
		return;
	}
	else if(pmsg->dwObjectID != object->GetID())
	{
		return;
	}

    pSkillObject->SetMultiTarget(&(pmsg->TargetList));
}

void cSkillManager::OnSkillCancel( MSG_DWORD* pmsg )
{
	CObject* pObject = g_pUserTable->FindUser( pmsg->dwObjectID );

	if( !pObject || pObject->GetObjectKind() != eObjectKind_Player )
	{
		return;
	}

	CPlayer* pOperator = (CPlayer*)pObject;
	cSkillObject* pSkillObject = m_SkillObjectTable.GetData( pmsg->dwData );
	if( !pSkillObject )
	{
		return;
	}

	const cActiveSkillInfo* pSkillInfo = GetActiveInfo( pSkillObject->GetSkillIdx() );
	if( !pSkillInfo )
		return;
	else if(SKILL_STATE_END == pSkillObject->GetSkillState() ||
		FALSE == pOperator->IsCanCancelSkill())
	{
		MSG_DWORD msg;
		msg.Category = MP_SKILL;
		msg.Protocol = MP_SKILL_CANCEL_NACK;
		msg.dwObjectID = pmsg->dwObjectID;
		msg.dwData = pOperator->GetSkillCancelDelay();

		pOperator->SendMsg( &msg, sizeof(msg) );
		return;
	}

	MSG_DWORD msg;
	msg.Category = MP_SKILL;
	msg.Protocol = MP_SKILL_CANCEL_NOTIFY;
	msg.dwObjectID = pmsg->dwObjectID;
	msg.dwData = pmsg->dwData;

	PACKEDDATA_OBJ->QuickSend( pOperator, &msg, sizeof( MSG_DWORD ) );

	pSkillObject->SetEndState();
	pSkillObject->EndState();
	pOperator->ResetCoolTime( pSkillInfo->GetInfo() );
}

// 080616 LUJ, 액티브 스킬을 추가한다
void cSkillManager::AddActiveSkill( CObject& object, DWORD skillIndex )
{
	const cActiveSkillInfo* const skillInfo = GetActiveInfo( skillIndex );

	if( ! skillInfo )
	{
		return;
	}

	cSkillObject* skillObject = skillInfo->GetSkillObject();

	if( ! skillObject )
	{
		return;
	}

	/// 스킬 생성 정보
	sSKILL_CREATE_INFO createInfo;
	ZeroMemory( &createInfo, sizeof( createInfo ) );
	createInfo.skillObjectId	= skillObject->GetID();
	createInfo.level			= skillInfo->GetLevel();
//	createInfo.pOperator		= &object;
	createInfo.operatorId		= object.GetID();
	createInfo.mainTarget.SetMainTarget( object.GetID() );
	createInfo.skillDir			= skillObject->GetSkillDir();

	/// 스킬 생성 위치
	if( skillInfo->GetInfo().Target )
	{
		VECTOR3 position = { 0 };
		object.GetPosition( &position );

		createInfo.pos.x = position.x;
		createInfo.pos.z = position.z;
	}
	else
	{
		createInfo.pos = *CCharMove::GetPosition( &object );
	}

	CTargetList targetList;
	// 080708 LUJ, 반드시 초기화가 필요하다
	targetList.Clear();
	
	switch(skillInfo->GetInfo().Area )
	{
	case TARGET_KIND_SINGLE:
	case TARGET_KIND_SINGLE_CIRCLE:
		{
			CTargetListIterator iter( &targetList );

			iter.AddTargetWithNoData( object.GetID(), SKILLRESULTKIND_NONE );
			// 080708 LUJ, Release()가 호출되어야 타겟 내의 버퍼 길이가 설정된다
			iter.Release();
			break;
		}
	case TARGET_KIND_MULTI:
	case TARGET_KIND_MULTI_ARC:
	case TARGET_KIND_MULTI_CIRCLE:
	case TARGET_KIND_MULTI_RECT:
		{
			VECTOR3 position = { 0 };
			object.GetPosition( &position );

			g_pUserTable->GetTargetInRange(
				&position,
				skillInfo->GetInfo().AreaData,
				&targetList,
				object.GetGridID() );
			break;
		}
	case TARGET_KIND_MULTI_MONSTER:
		{
			CUserTable::ObjectKindSet objectKindeSet;
			objectKindeSet.insert( eObjectKind_Monster );
			objectKindeSet.insert( eObjectKind_BossMonster );
			objectKindeSet.insert( eObjectKind_FieldBossMonster );
			objectKindeSet.insert( eObjectKind_FieldSubMonster );
			objectKindeSet.insert( eObjectKind_ChallengeZoneMonster );
			objectKindeSet.insert( eObjectKind_Trap );

			VECTOR3 position = { 0 };
			object.GetPosition( &position );

			g_pUserTable->GetTargetInRange(
				&position,
				skillInfo->GetInfo().AreaData,
				&targetList,
				object.GetGridID(),
				0,
				objectKindeSet );
			break;
		}
		// 090204 LUJ, 몬스터와 플레이어 모두를 타겟으로 잡는다
	case TARGET_KIND_MULTI_ALL:
		{
			CUserTable::ObjectKindSet objectKindSet;
			objectKindSet.insert( eObjectKind_Monster );
			objectKindSet.insert( eObjectKind_BossMonster );
			objectKindSet.insert( eObjectKind_FieldBossMonster );
			objectKindSet.insert( eObjectKind_FieldSubMonster );
			objectKindSet.insert( eObjectKind_ChallengeZoneMonster );
			objectKindSet.insert( eObjectKind_Trap );

			// 100601 ShinJS --- PK 가능 지역에서만 PVP 허용
			const BOOL isValidPlayerKill(TRUE == g_pServerSystem->GetMap()->IsPKAllow());
			if( object.GetObjectKind() == eObjectKind_Player && isValidPlayerKill )
				objectKindSet.insert( eObjectKind_Player );

			VECTOR3 position = { 0 };
			object.GetPosition( &position );

			g_pUserTable->GetTargetInRange(
				&position,
				skillInfo->GetInfo().AreaData,
				&targetList,
				object.GetGridID(),
				0,
				objectKindSet );
			break;
		}
	}

	// 090204 LUJ, 먼저 초기화되어야 한다
	skillObject->Init( &createInfo );
	skillObject->SetMultiTarget( &targetList );
	skillObject->SetBattle( object.GetBattleID(), object.GetBattleTeam() );
	skillObject->SetGridID( object.GetGridID() );

	/// 테이블에 등록
	m_SkillObjectTable.Add( skillObject, skillObject->GetID() );
	mProcessList.push_back(skillObject->GetID());
	g_pServerSystem->AddSkillObject( skillObject, &createInfo.pos );
}

SKILL_RESULT cSkillManager::OnPetSkillStartSyn(MSG_SKILL_START_SYN* pmsg, ITEMBASE* pItemBase)
{
	/// 시전자 정보가 없다면 실패
	// 080602 LUJ, 스킬 발동은 플레이어만 할 수 있으므로 캐스팅한다
	CPet* pOperator = ( CPet* )g_pUserTable->FindUser( pmsg->Operator );

	// 080602 LUJ, 검사 구문 추가
	if( !	pOperator											||
			pOperator->GetObjectKind()	!= eObjectKind_Pet	||
			pOperator->CurCastingSkillID )
	{
		return SKILL_OPERATOR_INVALUED;
	}

	// 080410 KTH -- 일반 스킬일 경우 스킬 트리에 포함 되어있는 스킬인지 확인한다.
	if( pItemBase )
	{
		ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( pItemBase->wIconIdx );
		const cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( pmsg->SkillIdx );

		if( pSkillInfo == NULL || pItemInfo == NULL )
			return SKILL_INFO_INVALUED;

		if( pmsg->SkillIdx / 100 != pItemInfo->SupplyValue / 100 )
			return SKILL_INFO_INVALUED;
	}
	// 090226 LUJ, 스킬 검사
	else
	{
		const SkillScript&	skillScript	= GAMERESRCMNGR->GetSkillScript( pmsg->SkillIdx );
		const BOOL			checkFailed	= ( skillScript.mIsNeedCheck && ! pOperator->IsHaveSkill( pmsg->SkillIdx ) );

		if( checkFailed )
		{
			LogScriptHack(
				*pOperator,
				"SkillList.bin",
				"*pet has no skill" );
			return SKILL_OPERATOR_INVALUED;
		}
	}

	const cSkillInfo* ptemp = GetSkillInfo( pmsg->SkillIdx );

	if( ptemp == NULL )
	{
		return SKILL_INFO_INVALUED;
	}

	cActiveSkillInfo* pSkillInfo = ( cActiveSkillInfo* )ptemp;

	if( pOperator->GetAbnormalStatus()->IsSilence )
	{
		// 080616 LUJ, 침묵 중에는 물리 공격만 가능하다
		if( pSkillInfo->GetKind() != SKILLKIND_PHYSIC )
		{
			return SKILL_STATE_INVALUED;
		}
	}
	if( pOperator->GetAbnormalStatus()->IsBlockAttack )
	{
		if( pSkillInfo->GetInfo().Unit == UNITKIND_PHYSIC_ATTCK )
			return SKILL_STATE_INVALUED;
	}

	/// 최초 접속시 무적 상태 해제
	if( pOperator->GetState() == eObjectState_Immortal )
		OBJECTSTATEMGR_OBJ->EndObjectState( pOperator, eObjectState_Immortal, 0 );

	/// 길드 토너먼트나 공성전에서 관람자는 스킬을 사용할 수 없다
	{
		CObject* const ownerObject = g_pUserTable->FindUser(
			pOperator->GetOwnerIndex());

		if(0 == ownerObject)
		{
			return SKILL_STATE_INVALUED;
		}
		else if(2 == ownerObject->GetBattleTeam())
		{
			return SKILL_STATE_INVALUED;
		}
	}

	if(FALSE == pSkillInfo->IsExcutableSkillState(*pOperator, pmsg->mConsumeItem))
	{
		return SKILL_STATE_INVALUED;
	}

	VECTOR3 TargetPos;
	CObject * pTargetObject = NULL;
	float TargetRadius;

	/// 타겟 설정 확인
	if(GetMainTargetPos(&pmsg->MainTarget,&TargetPos, &pTargetObject) == FALSE)
	{
		/// 타겟이 없다
		return SKILL_TARGET_INVALUED;
	}

	switch( pSkillInfo->GetInfo().Target )
	{
	case eSkillTargetKind_None:
		{
			return SKILL_TARGET_INVALUED;
		}
		break;
	case eSkillTargetKind_OnlyOthers:
		{
			if( pTargetObject == pOperator )
			{
				return SKILL_TARGET_INVALUED;
			}
		}
		break;
	case eSkillTargetKind_Ground:
		{
		}
		break;
	case eSkillTargetKind_OnlySelf:
		{
			if( pTargetObject != pOperator )
			{
				return SKILL_TARGET_INVALUED;
			}
		}
		break;
	case eSkillTargetKind_AllObject:
		{
		}
		break;
	}

	/// 타겟 오브젝트의 크기를 구한다
	if(pmsg->MainTarget.MainTargetKind == MAINTARGET::MAINTARGETKIND_POS)
		TargetRadius = 0;
	else
	{
		pTargetObject = g_pUserTable->FindUser( pmsg->MainTarget.dwMainTargetID );
		TargetRadius = pTargetObject->GetRadius();
	}	

	if( pSkillInfo->IsInRange(*pOperator, TargetPos, TargetRadius) == FALSE)
	{
		if(pTargetObject)
		{
			RESULTINFO DamageInfo = {0};
			DamageInfo.mSkillIndex = pSkillInfo->GetIndex();

			pTargetObject->Damage(pOperator, &DamageInfo);
		}
		return SKILL_RANGE_NOTENOUGH;
	}
	
	if( pSkillInfo->GetKind() == SKILLKIND_ONOFF )
	{
		BOOL destroy = FALSE;

		for( WORD i = 0; i < MAX_BUFF_COUNT; i++ )
		{
			const DWORD idx = pSkillInfo->GetInfo().Buff[ i ];
			
			if( cSkillObject* pSkill = pTargetObject->GetBuffList().GetData( idx / 100 ) )
			{
				if( pSkill->GetSkillIdx() <= pSkillInfo->GetInfo().Buff[ i ] )
				{
					pSkill->SetEndState();
					pSkill->EndState();
					destroy = TRUE;
				}
			}
		}
		
		if( destroy )
		{
			return SKILL_DESTROY;
		}
	}

	// 080602 LUJ, 전역 스킬은 DB에서 직접 처리한 후 전송된다. 취소 등을 위해 오브젝트 ID를 직접 받아온다
	const DWORD skillObjectIndex = GetNewSkillObjectID();

	sSKILL_CREATE_INFO info;
	info.level = pSkillInfo->GetInfo().Level;
	info.operatorId = pOperator->GetID();
	info.mainTarget = pmsg->MainTarget;
	info.skillDir = pmsg->SkillDir;	

	/// 스킬 오브젝트 생성
	cSkillObject* pSObj = pSkillInfo->GetSkillObject();

	/// 스킬 객체 ID
	// 080602 LUJ, DB 업데이트 실패 시 스킬 오브젝트를 취소할 수 있도록 위에서 인덱스를 미리 받아놓도록 했다
	info.skillObjectId = skillObjectIndex;
	
	if( pSkillInfo->GetInfo().Target )
	{
		info.pos.x = TargetPos.x;
		info.pos.z = TargetPos.z;
	}
	else
	{
		info.pos = *CCharMove::GetPosition(pOperator);
	}

	// 090123 LUJ, 객체를 초기화한 후 타겟 검사를 수행해야한다.
	pSObj->Init( &info );
	pSObj->SetMultiTarget( &( pmsg->TargetList ) );
	pSObj->SetBattle( pOperator->GetBattleID(), pOperator->GetBattleTeam() );
	pSObj->SetGridID( pOperator->GetGridID() );

	/// 테이블에 등록
	m_SkillObjectTable.Add( pSObj, pSObj->GetID() );
	mProcessList.push_back(pSObj->GetID());
	g_pServerSystem->AddSkillObject( pSObj, &info.pos );

	/// 스킬 생성 성공
	return SKILL_SUCCESS;
}

// 090105 LUJ, 플레이어가 학습 가능한 스킬인지 조사한다
BOOL cSkillManager::IsLearnableSkill( const CHARACTER_TOTALINFO& totalInfo, const SKILL_BASE& skillBase )
{
	// 090106 LUJ, grade 값을 사용한 후 감산하도록 수정
	for(	WORD grade = totalInfo.JobGrade;
			0 < grade;
			--grade )
	{
		const DWORD jobType		= DWORD( totalInfo.Job[ grade - 1 ] );
		// 090106 LUJ, 직업이 1단계인 경우 jobType은 항상 1이 되어야한다
		const DWORD jobIndex	= ( totalInfo.Job[ 0 ] * 1000 ) + ( ( totalInfo.Race + 1 ) * 100 ) + ( grade * 10 ) + ( 1 == grade ? 1 : jobType );
		// 090105 LUJ, 직업 번호를 이용해서 스킬 학습 트리의 포인터를 획득한다
		const SKILL_TREE_INFO* const skillTree = m_SkillTreeTable.GetData( jobIndex );

		if( ! skillTree )
		{
			break;
		}

		// 090105 LUJ, 스킬 트리에 저장된 스킬 중 클라이언트 요청에 적합한 조건이 있는지 검사한다
		for(	DWORD i = 0;
				i < skillTree->Count;
				++i )
		{
			const DWORD serverSkillIndex = skillTree->SkillIndex[ i ] / 100;
			const DWORD clientSkillIndex = skillBase.wSkillIdx / 100;

			if( serverSkillIndex != clientSkillIndex )
			{
				continue;
			}

			const DWORD serverSkillLevel = skillTree->SkillIndex[ i ] % 100;
			// 090106 LUJ, 불필요한 연산 제거
			const DWORD clientSkillLevel = skillBase.Level;

			if( serverSkillLevel >= clientSkillLevel )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void cSkillManager::RemoveGroupBuffSkill( CObject* pObject, const ACTIVE_SKILL_INFO& skillInfo)
{
	cPtrList templist;

	const SkillScript& curSkillScript = GAMERESRCMNGR->GetSkillScript( skillInfo.Index );
	pObject->GetBuffList().SetPositionHead();

	while(cBuffSkillObject* const pSObj = pObject->GetBuffList().GetData())
	{
		const SkillScript& skillScript = GAMERESRCMNGR->GetSkillScript( pSObj->GetSkillIdx() );
		if( ( curSkillScript.mSkillGroupNum > 0 )	&&
			( skillScript.mSkillGroupNum	> 0	)	&&
			( skillScript.mSkillGroupNum == curSkillScript.mSkillGroupNum )	)
		{
			templist.AddTail( pSObj );
		}
	}

	PTRLISTPOS pos = templist.GetHeadPosition();
	while( pos )
	{
		cSkillObject* const pSObj = ( cSkillObject* )templist.GetNext( pos );
		pSObj->SetEndState();
		pSObj->EndState();
		pObject->GetBuffList().Remove( pSObj->GetSkillIdx() );
	}
}

void cSkillManager::Recall(DWORD recallPlayerIndex, DWORD targetPlayerIndex, LPCTSTR targetPlayerName, MAPTYPE targetMap, DWORD skillIndex)
{
	CPlayer* const recallPlayer = (CPlayer*)g_pUserTable->FindUser(recallPlayerIndex);

	if(0 == recallPlayer ||
		recallPlayer->GetObjectKind() != eObjectKind_Player)
	{
		return;
	}

	MSG_RECALL_NACK errorMessage(
		MP_MOVE,
		MP_MOVE_RECALL_NACK,
		MSG_RECALL_NACK::TypeNone,
		recallPlayerIndex,
		0);
	SafeStrCpy(
		errorMessage.mRecallPlayerName,
		recallPlayer->GetObjectName(),
		sizeof(errorMessage.mRecallPlayerName) / sizeof(*errorMessage.mRecallPlayerName));

	if(g_pServerSystem->IsNoRecallMap(*recallPlayer))
	{
		errorMessage.mType = MSG_RECALL_NACK::TypePlayerLocateOnBlockedMap;
		recallPlayer->SendMsg(
			&errorMessage,
			sizeof(errorMessage));
		return;
	}
	else if(recallPlayerIndex == targetPlayerIndex)
	{
		errorMessage.mType = MSG_RECALL_NACK::TypePlayerCannotRecallOneself;
		recallPlayer->SendMsg(
			&errorMessage,
			sizeof(errorMessage));
		return;
	}

	const BOOL isHasNoSkill = (0 == recallPlayer->GetSkillTree().GetData(skillIndex));
	const cActiveSkillInfo* const activeSkillInfo = GetActiveInfo(skillIndex);

	if(isHasNoSkill ||
		0 == activeSkillInfo)
	{
		errorMessage.mType = MSG_RECALL_NACK::TypePlayerHasNoSkill;
		recallPlayer->SendMsg(
			&errorMessage,
			sizeof(errorMessage));
		return;
	}
	else if(IsInvalidRecallTarget(
		*recallPlayer,
		targetPlayerIndex,
		activeSkillInfo->GetInfo()))
	{
		errorMessage.mType = MSG_RECALL_NACK::TypeTargetIsNotExist;
		recallPlayer->SendMsg(
			&errorMessage,
			sizeof(errorMessage));
		return;
	}

	// 091127 LUJ, 정보를 설정하고 소환한다
	MSG_RECALL_SYN sendMessage(
		MP_MOVE,
		MP_MOVE_RECALL_ASK_SYN_TO_SERVER);
	sendMessage.mSkillIndex	= skillIndex;
	sendMessage.mRandomPositionRange = activeSkillInfo->GetInfo().AreaData;
	sendMessage.mLimitMiliSecond = activeSkillInfo->GetInfo().UnitData * 1000;
	sendMessage.mChannelIndex = recallPlayer->GetCurChannel();

	// 091127 LUJ, 리콜 사용자 정보 복사
	{
		MSG_RECALL_SYN::Player& player = sendMessage.mRecallPlayer;
		player.mIndex = recallPlayerIndex;
		player.mMapType	= g_pServerSystem->GetMapNum();
		SafeStrCpy(
			player.mName,
			recallPlayer->GetObjectName(),
			sizeof(player.mName) / sizeof(*player.mName));
	}

	// 091127 LUJ, 리콜 대상자 정보 복사
	{
		MSG_RECALL_SYN::Player& player = sendMessage.mTargetPlayer;
		player.mIndex = targetPlayerIndex;
		player.mMapType	= targetMap;
		SafeStrCpy(
			player.mName,
			targetPlayerName,
			sizeof(player.mName) / sizeof(*player.mName));
	}

	MOVEMGR->RequestToRecall(sendMessage);
}

BOOL cSkillManager::IsInvalidRecallTarget(CPlayer& recallPlayer, DWORD targetPlayerIndex, const ACTIVE_SKILL_INFO& info)
{
	switch(info.AreaTarget)
	{
	case eSkillAreaTargetGuild:
		{
			CGuild* const guild = GUILDMGR->GetGuild(recallPlayer.GetGuildIdx());

			if(0 == guild)
			{
				break;
			}

			const GUILDMEMBERINFO* const member = guild->GetMemberInfo(targetPlayerIndex);

			if(0 == member)
			{
				break;
			}

			return FALSE;
		}
	case eSkillAreaTargetParty:
		{
			CParty* const party = PARTYMGR->GetParty(recallPlayer.GetPartyIdx());

			if(0 == party)
			{
				break;
			}
			else if(party->GetMasterID() != recallPlayer.GetID())
			{
				break;
			}
			else if(FALSE == party->IsPartyMember(targetPlayerIndex))
			{
				break;
			}

			return FALSE;
		}
	}

	return TRUE;
}

SKILL_RESULT cSkillManager::CreateActiveSkill( DWORD dwSkillidx, DWORD dwOperatorID, DWORD dwTargetID, const VECTOR3& dir )
{
	const cActiveSkillInfo* pSkillInfo = GetActiveInfo( dwSkillidx );

	if( ! pSkillInfo )
		return SKILL_INFO_INVALUED;

	CObject* pOperator = g_pUserTable->FindUser( dwOperatorID );
	if( ! pOperator )
		return SKILL_OPERATOR_INVALUED;

	CObject* pTarget = g_pUserTable->FindUser( dwTargetID );
	if( ! pTarget )
		return SKILL_TARGET_INVALUED;

	// 스킬 오브젝트 생성
	sSKILL_CREATE_INFO info;
	info.skillObjectId = GetNewSkillObjectID();
	info.level = 1;
	info.operatorId = dwOperatorID;
	info.mainTarget.SetMainTarget( dwTargetID );
	info.skillDir = dir;
	info.pos = *CCharMove::GetPosition( pTarget );

	cSkillObject* pSObj = pSkillInfo->GetSkillObject();
	pSObj->Init( &info );
	pSObj->SetBattle( pOperator->GetBattleID(), pOperator->GetBattleTeam() );
	pSObj->SetGridID( pOperator->GetGridID() );

	m_SkillObjectTable.Add( pSObj, pSObj->GetID() );
	mProcessList.push_back( pSObj->GetID() );
	g_pServerSystem->AddSkillObject( pSObj, &info.pos );

	return SKILL_SUCCESS;
}

const cActiveSkillInfo* cSkillManager::GetActiveInfo(DWORD skillIndex)
{
	if(const cSkillInfo* const skillInfo = GetSkillInfo(skillIndex))
	{
		return cSkillInfo::TypeActive == skillInfo->GetType() ? (cActiveSkillInfo*)skillInfo : 0;
	}

	return 0;
}

const cBuffSkillInfo* cSkillManager::GetBuffInfo(DWORD skillIndex)
{
	if(const cSkillInfo* const skillInfo = GetSkillInfo(skillIndex))
	{
		return cSkillInfo::TypeBuff == skillInfo->GetType() ? (cBuffSkillInfo*)skillInfo : 0;
	}

	return 0;
}