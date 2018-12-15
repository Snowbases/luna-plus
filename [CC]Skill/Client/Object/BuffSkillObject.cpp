#include "stdafx.h"
#include ".\buffskillobject.h"
#include "GameIn.h"
#include "CharacterDialog.h"
#include "../info/buffskillinfo.h"

#include "Object.h"
#include "ObjectManager.h"
#include "BattleSystem_Client.h"
#include "ObjectActionManager.h"
#include "StatusIconDlg.h"
#include "PetStatusIconDialog.h"

#include "ChatManager.h"

cBuffSkillObject::cBuffSkillObject(cBuffSkillInfo* pSkillInfo)
: cSkillObject( pSkillInfo )
// 080318 LUJ, 설정 시각 초기화
, mBeginTime( 0 )
{
}

cBuffSkillObject::~cBuffSkillObject(void)
{
	ReleaseSkillObject();
}

void cBuffSkillObject::Init(const SKILLOBJECT_INFO& pInfo)
{
	m_bInited = TRUE;
	m_bDieFlag = FALSE;
	// 090204 LUJ, 스킬 적용 여부
	mIsTurnOn	= TRUE;

	m_BaseObjectInfo.dwObjectID = pInfo.SkillObjectIdx;
	SafeStrCpy(m_BaseObjectInfo.ObjectName,mpSkillInfo->GetName(), MAX_NAME_LENGTH+1);
	m_BaseObjectInfo.ObjectState = eObjectState_None;

	mSkillObjectInfo.skillObjectId = pInfo.SkillObjectIdx;
	mSkillObjectInfo.level = pInfo.SkillLevel;
	mSkillObjectInfo.pSkillInfo = mpSkillInfo;
	mSkillObjectInfo.operatorId = pInfo.Operator;
	mSkillObjectInfo.mainTarget = pInfo.MainTarget;
	mSkillObjectInfo.pos = pInfo.Pos;
	mSkillObjectInfo.skillDir = pInfo.Direction;
	mSkillObjectInfo.isNew = pInfo.IsNew;
	mSkillObjectInfo.remainTime = pInfo.RemainTime;

	CObject* pObject = OBJECTMGR->GetObject( mSkillObjectInfo.mainTarget.dwMainTargetID );

	if( HERO == pObject )
	{
		( ( cBuffSkillInfo* )mpSkillInfo )->AddBuffStatus();

		if( mpSkillInfo->GetImage() )
		{
			DWORD time = 0;

			if( mSkillObjectInfo.remainTime )
			{
				time = mSkillObjectInfo.remainTime + gCurTime;
			}
			else if( ( ( cBuffSkillInfo* )mpSkillInfo )->GetSkillInfo()->DelayTime > 0 )
			{
				time = ( ( cBuffSkillInfo* )mpSkillInfo )->GetSkillInfo()->DelayTime + gCurTime;
			}

			STATUSICONDLG->AddSkillIcon( GetID(), mpSkillInfo->GetIndex(), time, pInfo.Count );
		}

		if( mSkillObjectInfo.isNew && mpSkillInfo->GetImage() )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(2004), mpSkillInfo->GetName() );
		}
	}
	if( HEROPET == pObject )
	{
		( ( cBuffSkillInfo* )mpSkillInfo )->AddPetBuffStatus();

		if( mpSkillInfo->GetImage() )
		{
			DWORD time = 0;

			if( mSkillObjectInfo.remainTime )
			{
				time = mSkillObjectInfo.remainTime + gCurTime;
			}
			else if( ( ( cBuffSkillInfo* )mpSkillInfo )->GetSkillInfo()->DelayTime > 0 )
			{
				time = ( ( cBuffSkillInfo* )mpSkillInfo )->GetSkillInfo()->DelayTime + gCurTime;
			}

			PETSTATUSICONDLG->AddSkillIcon( GetID(), mpSkillInfo->GetIndex(), time, pInfo.Count );
		}
	}

	cBuffSkillInfo* pSkillInfo = ( cBuffSkillInfo* )mpSkillInfo;
	mSkillEffect.Init(
		pSkillInfo->GetSkillInfo()->SkillEffect,
		GetOperator(),
		&(mSkillObjectInfo.mainTarget));
	mSkillEffect.StartTargetEffect(
		mSkillObjectInfo.skillObjectId);

	// 080318 LUJ, 설정된 시각 저장
	mBeginTime = gCurTime;

	GAMEIN->GetCharacterDialog()->RefreshInfo();
}

void cBuffSkillObject::ReleaseSkillObject()
{
	CObject* pObject = OBJECTMGR->GetObject( mSkillObjectInfo.mainTarget.dwMainTargetID );

	if(0 == pObject)
	{
		return;
	}
	else if(pObject->GetID() == gHeroID)
	{
		if(mIsTurnOn)
		{
			((cBuffSkillInfo*)mpSkillInfo)->RemoveBuffStatus();
			GAMEIN->GetCharacterDialog()->RefreshInfo();
		}
		
		STATUSICONDLG->RemoveSkillIcon(
			GetID(),
			mpSkillInfo->GetIndex() );
	}
	else if( HEROPET == pObject )
	{
		if(mIsTurnOn)
		{
			((cBuffSkillInfo*)mpSkillInfo)->RemovePetBuffStatus();
		}

		PETSTATUSICONDLG->RemoveSkillIcon(
			GetID(),
			mpSkillInfo->GetIndex() );
	}

	mSkillEffect.EndTargetEffect();
}

// 090204 LUJ, 조건에 따라 버프를 적용/해제시킨다
void cBuffSkillObject::Update()
{
	if(mSkillObjectInfo.mainTarget.dwMainTargetID == gHeroID)
	{
		cBuffSkillInfo* const skillInfo = ((cBuffSkillInfo*)GetSkillInfo());
		const BOOL isEnable	= HERO->IsEnable(*(skillInfo->GetSkillInfo()));
		const BOOL isTurnOn	= ( isEnable && ! mIsTurnOn );
		const BOOL isTurnOff = ( ! isEnable && mIsTurnOn );

		if( isTurnOn )
		{
			skillInfo->AddBuffStatus();
			mIsTurnOn = TRUE;
			GAMEIN->GetCharacterDialog()->RefreshInfo();
		}
		else if( isTurnOff )
		{
			skillInfo->RemoveBuffStatus();
			mIsTurnOn = FALSE;
			GAMEIN->GetCharacterDialog()->RefreshInfo();
		}
	}
}

void cBuffSkillObject::OnReceiveSkillResult( MSG_SKILL_RESULT* pmsg )
{
	if( pmsg->SkillDamageKind )
	{
		CTargetListIterator iter( &pmsg->TargetList );
		CObject* pObject;
		RESULTINFO rinfo;

		while(iter.GetNextTarget())
		{
			pObject = OBJECTMGR->GetObject(iter.GetTargetID());
			iter.GetTargetData(&rinfo);
			if(pObject == NULL)
				continue;

			OBJECTACTIONMGR->ApplyResult(
				pObject,
				GetOperator(),
				&rinfo,
				eDamageKind_Front);
		}

		iter.Release();
	}
	else
	{
		mSkillEffect.SetTargetArray(
			GetOperator(),
			&(pmsg->TargetList),
			&(mSkillObjectInfo.mainTarget));
	}
}

void cBuffSkillObject::ResetDelay()
{
	CObject* pObject = OBJECTMGR->GetObject( mSkillObjectInfo.mainTarget.dwMainTargetID );

	if( HERO == pObject )
	{
		// 081203 LUJ, 서버에서 버프의 유효 시간을 변경할 수 있으므로 전송된 패킷의 시간을 이용한다
		const DWORD time = mSkillObjectInfo.remainTime + gCurTime;

		STATUSICONDLG->UpdateSkillTime( GetID(), mpSkillInfo->GetIndex(), time );
	}

	mSkillEffect.EndTargetEffect();
	mSkillEffect.StartTargetEffect( mSkillObjectInfo.skillObjectId );
}

// 090204 LUJ, 수치 범위를 확장함
void cBuffSkillObject::SetCount( int count )
{
	CObject* pObject = OBJECTMGR->GetObject( mSkillObjectInfo.mainTarget.dwMainTargetID );

	if( HERO == pObject )
	{
		STATUSICONDLG->UpdateSkillCount( GetID(), mpSkillInfo->GetIndex(), count );
	}
}
