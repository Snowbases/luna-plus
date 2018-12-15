#include "StdAfx.h"
#include "Action.h"
#include "ActionGetItem.h"
#include "Trigger.h"
#include "Manager.h"
#include "..\UserTable.h"
#include "..\Object.h"
#include "..\Dungeon\DungeonMgr.h"
#include "..\Party.h"
#include "..\PartyManager.h"
#include "..\Player.h"

namespace Trigger
{
	CAction::CAction() :
	mObjectIndexIterator(mObjectIndexList.end())
	{}

	CAction::~CAction(void)
	{}

	void CAction::SetParameter(const CAction::Parameter& parameter)
	{
		mParameter = parameter;
	}

	const CTrigger& CAction::GetTrigger() const
	{
		return TRIGGERMGR->GetTrigger(mParameter.mTriggerIndex);
	}

	int CAction::GetValue(eProperty property) const
	{
		const CTrigger& trigger = GetTrigger();
		const int value = trigger.GetActionValue(mParameter.mDescIndex, property);

		// 100111 LUJ, 이름인 경우 설정된 플래그라 해도 실제적인 값이 필요하다
		if(eProperty_Name == property)
		{
			return value;
		}
		else if(CManager::GetInstance().IsFlag(
			trigger.GetOwnerType(),
			trigger.GetOwnerIndex(),
			value))
		{
			return CManager::GetInstance().GetFlag(
				trigger.GetOwnerType(),
				trigger.GetOwnerIndex(),
				value);
		}

		return value;
	}

	DWORD CAction::GetChannelID() const
	{
		return mParameter.mChannelID;
	}

	DWORD CAction::GetHeadTarget() const
	{
		// LUJ, 이미 조사한 경우 재수행이 필요없다.
		if(false == mObjectIndexList.empty())
		{
			mObjectIndexIterator = mObjectIndexList.begin();
			return GetNextTarget();
		}

		switch(GetValue(eProperty_Target))
		{
			// 타겟이 없으면 현재 설정을 상속한다
		case eTarget_None:
			{
				mObjectIndexList.push_back(mParameter.mOwnerIndex);
				break;
			}
		case eTarget_Alias:
			{
				const DWORD aliasValue = GetValue(eProperty_Alias);
				g_pUserTable->SetPositionUserHeadChannel(mParameter.mChannelID);

				for(CObject* object = g_pUserTable->GetUserDataChannel(mParameter.mChannelID);
					0 < object;
					object = g_pUserTable->GetUserDataChannel(mParameter.mChannelID))
				{
					if(aliasValue != object->GetAlias())
					{
						continue;
					}

					mObjectIndexList.push_back(object->GetID());
				}

				break;
			}
		case eTarget_GroupAlias:
			{
				const DWORD aliasValue = GetValue(eProperty_GroupAlias);
				g_pUserTable->SetPositionUserHeadChannel(mParameter.mChannelID);

				for(CObject* object = g_pUserTable->GetUserDataChannel(mParameter.mChannelID);
					0 < object;
					object = g_pUserTable->GetUserDataChannel(mParameter.mChannelID))
				{
					if(aliasValue != object->GetGroupAlias())
					{
						continue;
					}

					mObjectIndexList.push_back(object->GetID());
				}

				break;
			}
		case eTarget_Condition:
			{
				const DWORD aliasValue = GetValue(eProperty_ConditionAlias);
				const CTrigger& trigger = GetTrigger();
				const DWORD objectIndex = trigger.GetSuccessObjectIndex(aliasValue);

				mObjectIndexList.push_back(objectIndex);
				break;
			}
		case eTarget_ChannelPlayer:
			{
				if(DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
				{
					const DWORD dwPartyIndex = DungeonMGR->GetPartyIndexFromDungeon(mParameter.mChannelID);
					
					if(CParty* const pParty = PARTYMGR->GetParty(dwPartyIndex))
					{
						for(int i = 0; i < MAX_PARTY_LISTNUM; ++i)
						{
							if(const DWORD dwObjectIndex = pParty->GetMemberID(i))
							{
								mObjectIndexList.push_back(dwObjectIndex);
							}
						}
					}

					break;
				}

				g_pUserTable->SetPositionUserHeadChannel(mParameter.mChannelID);

				while(CObject* const object = g_pUserTable->GetUserDataChannel(mParameter.mChannelID))
				{
					if(object->GetObjectKind() != eObjectKind_Player)
					{
						continue;
					}

					mObjectIndexList.push_back(object->GetID());
				}

				break;
			}
		case eTarget_PartyMaster:
			{
				if(DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
				{
					CParty* const party = PARTYMGR->GetParty(DungeonMGR->GetPartyIndexFromDungeon(mParameter.mChannelID));

					if(party)
					{
						const PARTY_MEMBER& partyMember = party->GetMember(0);
						mObjectIndexList.push_back(partyMember.dwMemberID);
					}

					break;
				}

				g_pUserTable->SetPositionUserHeadChannel(mParameter.mChannelID);

				for(CPlayer* player = (CPlayer*)g_pUserTable->GetUserDataChannel(mParameter.mChannelID);
					0 < player;
					player = (CPlayer*)g_pUserTable->GetUserDataChannel(mParameter.mChannelID))
				{
					if(player->GetObjectKind() != eObjectKind_Player)
					{
						continue;
					}

					CParty* const party = PARTYMGR->GetParty(player->GetPartyIdx());

					if(0 == party)
					{
						continue;
					}
					else if(player->GetID() != party->GetMasterID())
					{
						continue;
					}

					mObjectIndexList.push_back(player->GetID());
				}

				break;
			}
		}

		// 부하가 큰 재조사를 막기 위해 0값을 넣는다
		mObjectIndexList.push_back(0);
		mObjectIndexIterator = mObjectIndexList.begin();
		return GetNextTarget();
	}

	DWORD CAction::GetNextTarget() const
	{
		if(mObjectIndexList.end() == mObjectIndexIterator )
		{
			return 0;
		}

		return *(mObjectIndexIterator++);
	}

	LPCTSTR CAction::GetAliasName(DWORD hashCode) const
	{
		return TRIGGERMGR->GetAliasName(hashCode);
	}
}