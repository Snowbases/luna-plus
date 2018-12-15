#include "StdAfx.h"
#include "SkillObject.h"
#include "Battle.h"
#include "UserTable.h"

// 090205 LUJ, 광역 스킬 허용 오차의 기본값
float cSkillObject::mAllowRangeForWideSkill = 300.0f;

cSkillObject::cSkillObject(const SKILL_INFO& info) :
mSkillIndex(info.Index),
mSkillLevel(info.Level),
mSkillKind(info.Kind)
{
	// 080602 LUJ, 오브젝트 타입을 초기화한다
	mType = cSkillObject::TypeNone;
}

cSkillObject::~cSkillObject(void)
{}

CObject* cSkillObject::GetOperator()
{
	return g_pUserTable->FindUser( mSkillObjectInfo.operatorId );
//	return mSkillObjectInfo.pOperator;
}

void cSkillObject::Init( sSKILL_CREATE_INFO* pInfo )
{
	/// 스킬 생성 정보 설정
	memcpy(	&mSkillObjectInfo, pInfo, sizeof( sSKILL_CREATE_INFO ) );

	/// 기본 정보 셋팅
	m_BaseObjectInfo.dwObjectID = pInfo->skillObjectId;
	strcpy(m_BaseObjectInfo.ObjectName,"SkillObject");
	m_BaseObjectInfo.ObjectState = eObjectState_None;
	m_SkillDir = pInfo->skillDir;

	/// 사용자 정보가 잘못되었을 경우 실패
	if( g_pUserTable->FindUser( pInfo->operatorId ) == NULL )
	{
		return;
	}
}

void cSkillObject::EndState()
{
	mState = SKILL_STATE_DESTROY;
	GetOperator()->CurCastingSkillID = 0;
}

DWORD cSkillObject::SetRemoveMsg(DWORD dwReceiverID, MSGBASE*& sendMessage)
{
	static MSG_DWORD message;
	ZeroMemory(&message, sizeof(message));
	message.Category = MP_SKILL;
	message.Protocol = MP_SKILL_SKILLOBJECT_REMOVE;
	message.dwObjectID = dwReceiverID;
	message.dwData = GetID();

	sendMessage = &message;
	return sizeof(message);
}