// UserTable.cpp: implementation of the CUserTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UserTable.h"
#include "CharMove.h"
#include "Player.h"

CUserTable * g_pUserTable = 0;

CUserTable::CUserTable()
{
}

CUserTable::~CUserTable()
{
}


void CUserTable::Init(DWORD dwBucket)
{
	CYHHashTable<CObject>::Initialize(dwBucket);
	m_dwUserCount = 0;
	m_dwMonsterCount = 0;
	m_dwNpcCount = 0;
	m_dwExtraCount = 0;

	m_SearchUserIndexIterator = m_SearchUserIndexSet.begin();
}

CObject* CUserTable::FindUser(DWORD dwKey)
{
	return CYHHashTable<CObject>::GetData(dwKey);
}

CObject* CUserTable::FindUserForName(LPCTSTR name)
{
	return FindUser(
		GetObjectIndex(name));
}

BOOL CUserTable::AddUser(CObject* pObject,DWORD dwKey)
{
	switch(pObject->GetObjectKind())
	{
	case eObjectKind_Player:
		{
			SetObjectIndex(
				pObject->GetObjectName(),
				pObject->GetID());

			++m_dwUserCount;
		}
		break;
	case eObjectKind_Monster:
		{
			++m_dwMonsterCount;
		}
		break;
	case eObjectKind_BossMonster:
		{
			++m_dwMonsterCount;
		}
		break;
	case eObjectKind_SpecialMonster:
		{
			++m_dwMonsterCount;
		}
		break;
	case eObjectKind_ToghterPlayMonster:
		{
			++m_dwMonsterCount;
		}
		break;
	case eObjectKind_Npc:
		{
			++m_dwNpcCount;
		}
		break;
	default:
		{
			++m_dwExtraCount;
		}
		break;
	}

	// 오브젝트 인덱스를 채널별로 분리해서 삽입한다.
	UserIndexSet& set = m_UserIndexListInChannel[pObject->GetGridID()];
	set.insert(pObject->GetID());

	return CYHHashTable<CObject>::Add(pObject,dwKey);	
}

CObject * CUserTable::RemoveUser(DWORD dwKey)
{
	CObject* info = CYHHashTable< CObject >::GetData(dwKey);
	CYHHashTable< CObject >::Remove(dwKey);
	
	if( info == NULL ) return NULL;

	switch(info->GetObjectKind())
	{
	case eObjectKind_Player:
		{
			mAliasContainer.erase(
				info->GetObjectName());

			--m_dwUserCount;
		}
		break;
	case eObjectKind_Monster:
		{
			--m_dwMonsterCount;
		}
		break;
	case eObjectKind_Npc:
		{
			--m_dwNpcCount;
		}
		break;
	default:
		{
			--m_dwExtraCount;
		}
		break;
	}

	// 채널별로 분리된 오브젝트 인덱스를 삭제한다.
	UserIndexListInChannel::iterator iter = m_UserIndexListInChannel.find(info->GetGridID());

	if(iter != m_UserIndexListInChannel.end())
	{
		UserIndexSet& userIndexSet = iter->second;
		userIndexSet.erase(dwKey);

		if(userIndexSet.empty())
			m_UserIndexListInChannel.erase(iter->first);
	}

	return info;
}

void CUserTable::GetTargetInRange(VECTOR3* pPivotPos,float Radius,CTargetList* pTList,DWORD GridID, float SafeRange)
{
	pTList->Clear();
	CTargetListIterator iter(pTList);

	CObject* pObject;
	VECTOR3* pObjectPos;
	float dist;
	float dx,dz;

	SetPositionHead();
	while((pObject = GetData())!= NULL)
	{
		if(	pObject->GetObjectKind() == eObjectKind_Player )
		{
			// 080604 KTH -- 그리드 체크 부분 추가
			if(pObject->GetState() == eObjectState_Die || pObject->GetState() == eObjectState_Immortal || pObject->GetGridID() != GridID)
				continue;
			
			pObjectPos = CCharMove::GetPosition(pObject); 
			dx = (pPivotPos->x - pObjectPos->x);
			dz = (pPivotPos->z - pObjectPos->z);
			dist = sqrtf(dx*dx + dz*dz) - pObject->GetRadius();
			/// 사정거리 안쪽이고 안전거리 바깥쪽만 타겟이다
			if(dist <= Radius && dist >= SafeRange)
			{
				iter.AddTargetWithNoData(pObject->GetID(),SKILLRESULTKIND_NONE);
			}
		}
	}
	iter.Release();
}

// 080708 LUJ, 특정 오브젝트 종류를 타겟으로 잡는다
void CUserTable::GetTargetInRange(VECTOR3* pPivotPos,float Radius,CTargetList* pTList,DWORD GridID, float SafeRange, const CUserTable::ObjectKindSet& objectKindSet )
{
	CTargetListIterator iter(pTList);
	SetPositionHead();

	for(CObject* object = GetData();
		0 != object;
		object = GetData())
	{
		if(objectKindSet.end() == objectKindSet.find(EObjectKind(object->GetObjectKind())))
		{
			continue;
		}
		else if(eObjectState_Die == object->GetState())
		{
			continue;
		}
		else if(eObjectState_Immortal == object->GetState())
		{
			continue;
		}
		else if(GridID != object->GetGridID())
		{
			continue;
		}

		const float distance = CalcDistanceXZ(
			CCharMove::GetPosition(object),
			pPivotPos);

		if(distance > Radius)
		{
			continue;
		}
		else if(distance < SafeRange)
		{
			continue;
		}

		iter.AddTargetWithNoData(
			object->GetID(),
			SKILLRESULTKIND_NONE);
	}

	iter.Release();
}

void CUserTable::SetPositionUserHeadChannel(DWORD dwChannel)
{
	UserIndexListInChannel::iterator iter = m_UserIndexListInChannel.find(dwChannel);

	if(iter == m_UserIndexListInChannel.end())
		return;

	m_SearchUserIndexSet = iter->second;
	m_SearchUserIndexIterator = m_SearchUserIndexSet.begin();
}

CObject* CUserTable::GetUserDataChannel(DWORD dwChannel)
{
	if(m_SearchUserIndexIterator == m_SearchUserIndexSet.end())
		return NULL;

	DWORD dwObjectIndex = *(m_SearchUserIndexIterator);
	CObject* pObject = g_pUserTable->FindUser(dwObjectIndex);

	m_SearchUserIndexIterator++;

	return pObject;
}

void CUserTable::SetObjectIndex(LPCTSTR name, CUserTable::ObjectIndex index)
{
	mAliasContainer[name] = index;
}

CUserTable::ObjectIndex CUserTable::GetObjectIndex(LPCTSTR name) const
{
	const AliasContainer::const_iterator iterator = mAliasContainer.find(
		name);
	return mAliasContainer.end() == iterator ? 0 : iterator->second;
}

const CUserTable::Team& CUserTable::GetTeam(LPCTSTR teamName) const
{
	static const Team emptyTeam;
	const TeamContainer::const_iterator iterator = mTeamContainer.find(
		teamName);
	return mTeamContainer.end() == iterator ? emptyTeam : iterator->second;
}

void CUserTable::AddTeamMember(LPCTSTR teamName, ObjectIndex objectIndex)
{
	Team& team = mTeamContainer[teamName];
	team.mMemberContainer.insert(
		objectIndex);
}

void CUserTable::SetTeamLeader(LPCTSTR teamName, ObjectIndex objectIndex)
{
	Team::ObjectIndexContainer teamContainer;
	Team& team = mTeamContainer[teamName];

	for(Team::ObjectIndexContainer::const_iterator iterator = team.mMemberContainer.begin();
		team.mMemberContainer.end() != iterator;
		++iterator)
	{
		const Team::ObjectIndex objectIndex = *iterator;

		if(0 == FindUser(objectIndex))
		{
			continue;
		}
		teamContainer.insert(
			objectIndex);
	}

	team.mMemberContainer = teamContainer;
	team.mLeaderObjectIndex = objectIndex;
	team.mMemberContainer.insert(
		objectIndex);
}