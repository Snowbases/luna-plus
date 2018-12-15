#include "stdafx.h"
#include "Object.h"
#include "MoveManager.h"
#include "ObjectManager.h"
#include "GameIn.h"
#include "ObjectStateManager.h"
#include "interface/cWindowManager.h"
#include "Interface/cScriptManager.h"
#include "ObjectBalloon.h"
#include "OBalloonChat.h"
#include "OBalloonName.h"
#include "StreetStallTitleTip.h"
#include "MHFile.h"
#include "ObjectGuagen.h"
#include "GameResourceManager.h"
#include "MOTIONDESC.h"
#include "BattleSystem_Client.h"
#include "MonsterGuageDlg.h"
#include "ChatManager.h"
#include "FacialManager.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "PKManager.h"
#include "petmanager.h"
#include "Furniture.h"
#include "MonsterTargetDlg.h"
#include "BattleGuageDlg.h"

CObject::CObject()
{
	m_pObjectBalloon		= NULL;
	m_bObjectBalloonBits	= 0;
	m_StateEffectList.Initialize(3);
	m_pMotionDESC = NULL;
	m_bSelected	= FALSE;
	m_ObjectKind = eObjectKind_None;
	m_dwShockedTime = 0;
}

CObject::~CObject()
{
	Release();
}


BOOL CObject::Init(EObjectKind kind,BASEOBJECT_INFO* pBaseObjectInfo)
{
	m_bMoveSkipCount = 0;
	m_bIsYMoving = FALSE;
	m_BaseObjectInfo = *pBaseObjectInfo;
	m_BaseObjectInfo.ObjectBattleState = eObjectBattleState_Peace;
	m_ObjectKind = kind;
	m_ObjectState.bEndState = FALSE;

	SetFlag(FALSE);
//m_bDieFlag = FALSE;
	m_bInited = TRUE;
	mBattleStateCheckDelay.Init(
		10000);
	m_bHideEffect = FALSE;
	m_dwShockedTime = 0;
	return TRUE;
}


// 071214 LUJ, 코드 정리
void CObject::InitObjectBalloon(BYTE bitFlag)
{
	m_pObjectBalloon		= new CObjectBalloon;
	m_bObjectBalloonBits	= bitFlag;

	switch( GetObjectKind() )
	{
	case eObjectKind_BossMonster:
	case eObjectKind_FieldBossMonster:
	case eObjectKind_Monster:
	// 090422 ShinJS --- 탈것 추가
	case eObjectKind_Vehicle:
		{
			// 080616 LUJ, 잘못된 정보가 와서 가끔 튕기고 있음. 이를 막기 위해 널포인터 검사 추가
			const BASE_MONSTER_LIST* monsterList = ( ( CMonster* )this )->GetSInfo();

			if( monsterList )
			{
				m_pObjectBalloon->InitBalloon( this, monsterList->Tall, bitFlag );
			}

			break;
		}
	case eObjectKind_Player:
		{
			m_pObjectBalloon->InitBalloon(this, 180, bitFlag);
			break;
		}
	case eObjectKind_Npc:
		{
			CNpc* npc = ( CNpc* )this;
			m_pObjectBalloon->InitBalloon( npc, npc->GetSInfo()->Tall, bitFlag );
			break;
		}
	case eObjectKind_FarmObj:
		{
			const CSHFarmRenderObj::stFarmRenderObjInfo* const info = ( ( CSHFarmRenderObj* )this )->GetInfo();

			// 090806 LUJ, 존재하지 않는 리소스가 클라이언트를 강제 종료하게 하므로 검사한다
			if( 0 == info )
			{
				break;
			}

			const CSHFarmManager::stFARMRENDEROBJLIST* const list = g_csFarmManager.CLI_GetFarmRenderObjList( info->nKind );

			// 090806 LUJ, 존재하지 않는 리소스가 클라이언트를 강제 종료하게 하므로 검사한다
			if( 0 == list )
			{
				break;
			}

			m_pObjectBalloon->InitBalloon( this, list->nNameYpos, bitFlag );
			break;
		}
	case eObjectKind_Pet:
		{
			m_pObjectBalloon->InitBalloon(this, PETMGR->GetNameHeight(), bitFlag);
			break;
		}
	//090525 pdy 하우징 
	case eObjectKind_Furniture:
		{
			float fHeightSize = ((CFurniture*)this)->GetMarkingInfo()->fHeightSize;
			m_pObjectBalloon->InitBalloon(this, static_cast<long>(fHeightSize) , bitFlag);
			break;
		}
	default:
		{
			//m_pObjectBalloon->InitBalloon(this, 180, bitFlag);
			m_pObjectBalloon->InitBalloon(this, 180, bitFlag);
			break;
		}
	}

	if(! ( bitFlag & ObjectBalloon_Name ) )
	{
		return;
	}

	COBalloonName* balloonName =  m_pObjectBalloon->GetOBalloonName();

	if( ! balloonName )
	{
		return;
	}

	switch( GetObjectKind() )
	{
	case eObjectKind_Player:
		{	
			balloonName->InitName(m_BaseObjectInfo.ObjectName);

			CPlayer* player = ( CPlayer* )this;

			SetGuildMark();
			SetNickName();
			SetFamilyMark();

			m_pObjectBalloon->SetFamilyNickName(
				player->GetFamilyNickName());
			break;
		}
	case eObjectKind_Npc:
		{				
			CNpc* npc = ( CNpc* )this;

			if( npc->GetSInfo()->ShowJob )
			{
				m_pObjectBalloon->SetNickName( npc->GetSInfo()->Name );
			}

			const MAPCHANGE_INFO* info  = GAMERESRCMNGR->GetMapChangeInfo( npc->GetNpcUniqueIdx() );

			balloonName->InitName( info ? info->ObjectName : m_BaseObjectInfo.ObjectName );
			break;
		}
	default:
		{
			balloonName->InitName(
				GetObjectName());
			break;
		}
	}
}

void CObject::SetOverInfoOption( DWORD dwOption )
{
	if( m_pObjectBalloon )	
		m_pObjectBalloon->SetOverInfoOption( dwOption );
}

void CObject::SetGuildMark()
{
	if( m_pObjectBalloon )
		m_pObjectBalloon->SetGuildMark( ((CPlayer*)this)->GetGuildMarkName() );
}

void CObject::SetFamilyMark()
{
	if( m_pObjectBalloon )
		m_pObjectBalloon->SetFamilyMark( ((CPlayer*)this)->GetFamilyMarkName() );
}

void CObject::SetNickName()
{
	if( m_pObjectBalloon )
		m_pObjectBalloon->SetNickName( ((CPlayer*)this)->GetNickName() );
}

void CObject::ShowObjectName( BOOL bShow, DWORD dwColor )		//show/hide
{
	if( !m_pObjectBalloon )								return;
	if( !(m_bObjectBalloonBits & ObjectBalloon_Name) )	return;

	m_pObjectBalloon->ShowObjectName( bShow, dwColor );
}

void CObject::ShowChatBalloon( BOOL bShow, char* chatMsg, DWORD dwColor, DWORD dwAliveTime )
{
	if( !m_pObjectBalloon )								return;
	if( !(m_bObjectBalloonBits & ObjectBalloon_Chat) )	return;

	m_pObjectBalloon->ShowChatBalloon( bShow, chatMsg, dwColor, dwAliveTime );
}

//090116 pdy 클라이언트 최적화 작업 ( Hide 노점Title)
void CObject::HideStreetStallTitle( BOOL bHide )		
{
	if( !m_pObjectBalloon )								return;

	m_pObjectBalloon->HideStreetStallTitle(bHide);
}

//-----

void CObject::InitMove(BASEMOVE_INFO* pMoveInfo)
{
	memset(&m_MoveInfo,0,sizeof(MOVE_INFO));
	memcpy(&m_MoveInfo,pMoveInfo,sizeof(BASEMOVE_INFO));
}

void CObject::Release()
{
	m_bMoveSkipCount = 0;
	m_bInited = FALSE;

	if(m_ShadowObj.IsInited())
	{
		m_ShadowObj.Release();
	}
	RemoveAllObjectEffect();

	m_ObjectKind = eObjectKind_None;
	m_BaseObjectInfo.ObjectState = eObjectState_None;
	m_EngineObject.Release();
	
	SAFE_DELETE( m_pObjectBalloon );
	m_bObjectBalloonBits = 0;
	ReleaseBuffSkillList();
}

void CObject::GetBaseObjectInfo(BASEOBJECT_INFO* pRtInfo)
{
	memcpy(pRtInfo,&m_BaseObjectInfo,sizeof(BASEOBJECT_INFO));
}

void CObject::GetBaseMoveInfo(BASEMOVE_INFO* pRtInfo)
{
	memcpy(pRtInfo,&m_MoveInfo,sizeof(BASEMOVE_INFO));
}

void CObject::Process()
{
	OBJECTSTATEMGR->StateProcess(this);
	MOVEMGR->MoveProcess(this);

	if(m_ShadowObj.IsInited())
		m_ShadowObj.SetEngObjPosition(&GetCurPosition());

	if( m_BaseObjectInfo.ObjectBattleState )
	{
		if( !mBattleStateCheckDelay.Check() )
		{
			SetObjectBattleState( eObjectBattleState_Peace );
		}
	}
	// E 전투평화상태 주석처리 added by hseos 2007.08.02
}

void CObject::SetFlag(BOOL val)
{
	if( val )
	{
		if( GAMEIN->GetMonsterGuageDlg()->GetTargetVersusID() == GetID() )
		{
			GAMEIN->GetMonsterGuageDlg()->TargetVersusDie();
		}
	}
	m_bDieFlag = val; 
}

void CObject::SetDieFlag()
{
	m_DiedTime = gCurTime;

	// 죽었을때의 처리
	SetFlag(TRUE);
//	m_bDieFlag = TRUE;
	MOVEMGR->MoveStop(this);
	HERO->ClearTarget(this);

	if( HEROPET )
		HEROPET->ClearTarget(this);

	// LBS 03.11.21
	if( OBJECTMGR->GetSelectedObjectID() == GetID() )
	{
		// 070510 LYW --- Object : Modified.
		//OBJECTMGR->SetSelectedObjectID(0);
		GAMEIN->GetMonsterGuageDlg()->StartFadeOut() ;

		//KES 추가	//여기서 하는게 이상하다. 몬스터게이지를 살려두기위해서 좀 꼬였다.
		OnDeselected();
		if( OBJECTMGR->CanOverInfoChange( this ) )
			ShowObjectName( FALSE );
		else
			ShowObjectName( TRUE, OBJECTMGR->GetObjectNameColor(this) );
		//
	}
	
//	OBJECTSTATEMGR->StartObjectState(this, eObjectState_Die);
	
	//CHATMGR->AddMsg( CTC_TOWHOLE, CHATMGR->GetChatMsg(133), );
	// 죽으면 클릭 안되게 함
	if( m_ObjectKind == eObjectKind_Player )
		m_EngineObject.EnablePick();
	else
		m_EngineObject.DisablePick();
}

void CObject::Revive(VECTOR3* pRevivePos)
{
	SetFlag(FALSE);
//	m_bDieFlag = FALSE;
	MOVEMGR->SetPosition(this,pRevivePos);
}

void CObject::SetState(EObjectState State)
{
	SetMotionInState(State);
	
	if(m_ObjectState.State_Start_Motion != -1)
	{
		ChangeMotion(m_ObjectState.State_Start_Motion,FALSE);

		if(m_ObjectState.State_Ing_Motion != -1)
			ChangeBaseMotion(m_ObjectState.State_Ing_Motion);
	}
	
	m_BaseObjectInfo.ObjectState		= State;
	m_ObjectState.State_Start_Time = gCurTime;
	m_ObjectState.bEndState		= FALSE;
}

void CObject::SetPosition(VECTOR3* pPos)
{
	GetEngineObject()->SetEngObjPosition(pPos);
}
void CObject::SetAngle(float AngleRad)
{
	GetEngineObject()->SetEngObjAngle(AngleRad);
}
void CObject::GetPosition(VECTOR3* pPos)
{
	GetEngineObject()->GetEngObjPosition(pPos);
}
float CObject::GetAngle()
{
	return GetEngineObject()->GetEngObjAngle();
}
DIRINDEX CObject::GetDirectionIndex()
{
	float angleRad = GetAngle();
	DIRINDEX dir = RADTODIR(angleRad);
	while(dir >= MAX_DIRECTION_INDEX)
		dir -= MAX_DIRECTION_INDEX;
	return dir;
}

void CObject::ChangeMotion(int motion,BOOL bRoop, float rate)
{
	GetEngineObject()->ChangeMotion(motion,bRoop,rate);
}
void CObject::ChangeBaseMotion(int motion)
{
	GetEngineObject()->ChangeBaseMotion(motion);
}

// 070222 LYW --- Object : Add function to change emotion of hero.
void CObject::ChangeEmotion( int emotion ) 
{
	GetEngineObject()->ChangeMotion(emotion,FALSE);
	((CPlayer*)this)->SetRunningEmotion(TRUE) ;
}


void CObject::OnSelected()
{
 	GetEngineObject()->EnableSelfIllumin();
	m_bSelected = TRUE;
}

void CObject::OnDeselected()
{
	// desc_hseos_라이트효과01
	// S 라이트효과 추가 added by hseos 2007.06.13
	//	GetEngineObject()->DisableSelfIllumin();
	// E 라이트효과 추가 added by hseos 2007.06.13
	m_bSelected = FALSE;
}

void CObject::SetBattle(DWORD BattleID,BYTE Team)
{
	m_BaseObjectInfo.BattleID = BattleID;
	m_BaseObjectInfo.BattleTeam = Team;
}

void CObject::AddObjectEffect(DWORD KeyValue,OBJECTEFFECTDESC* EffectNumArray,WORD NumOfEffect,CObject* pSkillOperator)
{
	HEFFPROC heff = 0;

	m_StateEffectList.StartGetMultiData(KeyValue);
	// 100111 LUJ, 기존 이펙트가 있는지 검사한다
	while((heff = (HEFFPROC)m_StateEffectList.GetMultiData()) != NULL)
	{
		EFFECTMGR->IncEffectRefCount(heff);
		break;
	}

	// 없었으면 새로 만든다.
	for(int n=0;n<NumOfEffect;++n)
	{
		OBJECTEFFECTDESC& description = EffectNumArray[n];

		if(description.Effect == 0)
			continue;

		heff = EFFECTMGR->StartSkillMainTainEffect(
			description.Effect,
			this,
			description.Flag,
			&description.Position,
			pSkillOperator);

		if(heff == NULL)
			continue;

		m_StateEffectList.Add(
			LPVOID(heff),
			KeyValue);
	}
}

void CObject::RemoveObjectEffect(DWORD KeyValue)
{
	HEFFPROC heff;
	// 기존에 있던 이펙트를 찾는다.
//	BOOL bFound = FALSE;
	m_StateEffectList.StartGetMultiData(KeyValue);
	
	cPtrList list;

	while((heff = (HEFFPROC)m_StateEffectList.GetMultiData()) != NULL)
	{
		if(EFFECTMGR->ForcedEndEffect(heff) == TRUE)
		{
//			m_StateEffectList.RemoveCurMultiData();
			list.AddTail((void*)KeyValue);
		}
	}

	PTRLISTPOS pos = list.GetHeadPosition();
	while( pos )
	{
		DWORD k = (HEFFPROC)list.GetNext(pos);
		m_StateEffectList.Remove(k);
	}
	list.RemoveAll();

}
void CObject::RemoveAllObjectEffect()
{
	HEFFPROC heff;
	m_StateEffectList.SetPositionHead();
	while((heff = (HEFFPROC)m_StateEffectList.GetData()) != NULL)
	{
		while(EFFECTMGR->ForcedEndEffect(heff) == FALSE)
		{}
	}
	m_StateEffectList.RemoveAll();	//KES 리스트에서 지워도 되나? forceEnd가 안된경우는..confirm
}
int	CObject::GetMotionIDX(int mainMotion, int subMotion)
{
	return m_pMotionDESC->GetMotionIDX(mainMotion, subMotion);
}

void CObject::Die(CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive)
{
}

BOOL CObject::IsNpcMark( DWORD dwValue )
{
	HEFFPROC heff = (HEFFPROC)m_StateEffectList.GetData( dwValue );
	if( heff )
		return TRUE;
	return FALSE;
}

void CObject::SetObjectBattleState(eObjectBattleState state)
{
	if( state )
	{
		mBattleStateCheckDelay.Start();
	}
	else
	{
		mBattleStateCheckDelay.End();
	}

	if( m_BaseObjectInfo.ObjectBattleState != state )
	{
		m_BaseObjectInfo.ObjectBattleState = state;

		
		DWORD	dwTargetID	=	GAMEIN->GetMonsterGuageDlg()->GetWantedTargetID();
		
		if( dwTargetID == GetID() && state == eObjectBattleState_Peace )
		{
			GAMEIN->GetMonsterGuageDlg()->SetTargetVersusInfo( 0 );
		}
		
		if( GetObjectKind() == eObjectKind_Player )
		{
			((CPlayer*)this)->SetBaseMotion();
			if(GetState() == eObjectState_None)
				OBJECTSTATEMGR->EndObjectState(this,eObjectState_None);
		}
	}
}

//090901 pdy 이팩트 Hide처리 추가 
void CObject::HideEffect( BOOL bHide )
{
	m_bHideEffect = bHide ;
}

void CObject::SetShockTime( DWORD dwTime )
{
	m_dwShockedTime = gCurTime + dwTime;
}

BOOL CObject::IsShocked()
{
	if( gCurTime < m_dwShockedTime )
	{
		return TRUE;
	}

	m_dwShockedTime = 0;
	return FALSE;
}

void CObject::InsertBuffSkill( SKILLOBJECT_INFO* pBuffSkill )
{
	SKILLOBJECT_INFO* pBuffSkillObject	=	NULL;

	if( !pBuffSkill )
		return;

	bool bIsCorrect = false;
	
	for( BuffSkillList::iterator buffSkillListItor	=	m_BuffSkillList.begin();
		 buffSkillListItor != m_BuffSkillList.end(); 
		 ++buffSkillListItor )
	{
		pBuffSkillObject = (*buffSkillListItor);
		if( pBuffSkill->SkillIdx == pBuffSkillObject->SkillIdx )
			bIsCorrect = true;
	}
	
	if( !bIsCorrect )
	{
		SKILLOBJECT_INFO* pNewBuffSkillInfo = new SKILLOBJECT_INFO;
		memset( pNewBuffSkillInfo, 0, sizeof( SKILLOBJECT_INFO ) );
		memcpy( pNewBuffSkillInfo, pBuffSkill, sizeof( SKILLOBJECT_INFO ) );
		m_BuffSkillList.push_back( pNewBuffSkillInfo );
	}

}

void CObject::RemoveBuffSkill( DWORD dwSkillObjectIdx )
{
	SKILLOBJECT_INFO* pBuffSkillObject	=	NULL;

	CMonsterGuageDlg* pMonsterGuage	=	GAMEIN->GetMonsterGuageDlg();

	for( BuffSkillList::iterator buffSkillListItor	=	m_BuffSkillList.begin();
		 buffSkillListItor != m_BuffSkillList.end(); 
		 ++buffSkillListItor )
	{
		pBuffSkillObject = (*buffSkillListItor);
		if( dwSkillObjectIdx == pBuffSkillObject->SkillIdx )
		{
			pMonsterGuage->RemoveSkill( dwSkillObjectIdx );

			m_BuffSkillList.erase( buffSkillListItor );
			buffSkillListItor = m_BuffSkillList.end();
		}
	}
}


void CObject::ReleaseBuffSkillList()
{
	SKILLOBJECT_INFO* pBuffSkillObject	=	NULL;

	for( BuffSkillList::iterator buffSkillListItor	=	m_BuffSkillList.begin();
		 buffSkillListItor != m_BuffSkillList.end(); 
		 ++buffSkillListItor )
	{
		pBuffSkillObject = (*buffSkillListItor);
		SAFE_DELETE( pBuffSkillObject );
	}
	m_BuffSkillList.clear();
}

void CObject::UpdateTargetBuffSkill()
{
	SKILLOBJECT_INFO* pBuffSkillObject	=	NULL;

	CMonsterGuageDlg* pMonsterGuage	=	GAMEIN->GetMonsterGuageDlg();

	pMonsterGuage->RemoveAllSkillIcon();

	for( BuffSkillList::iterator buffSkillListItor	=	m_BuffSkillList.begin();
		 buffSkillListItor != m_BuffSkillList.end(); 
		 ++buffSkillListItor )
	{
		pBuffSkillObject = (*buffSkillListItor);
		pMonsterGuage->InsertSkillIcon( pBuffSkillObject->SkillObjectIdx, 
										pBuffSkillObject->SkillIdx, 
										0, 
										0 );
	}
}

void CObject::SetLife( DWORD life, BYTE type )
{
	CObject* pTargetObject	=	GAMEIN->GetBattleGuageDlg()->GetCurTargetObject();
	if( pTargetObject )
	{
		if( pTargetObject->GetID() == GetID() )
			GAMEIN->GetBattleGuageDlg()->SetBattleTargetInfo( this );
	}

	if( GAMEIN->GetMonsterGuageDlg()->GetCurTarget() )
	{
		if( GAMEIN->GetMonsterGuageDlg()->GetCurTarget()->GetTargetVSID() == GetID() )
		{
			float	fLife		=	(float)GetLife();
			float	fMaxLife	=	(float)GetMaxLife();

			GAMEIN->GetMonsterGuageDlg()->GetCurTarget()->SetTargetVSGuage( fLife / fMaxLife );
		}
	}
}

void CObject::ApplyGravity( BOOL bApplyGravity, float fGravityAcceleration, DWORD dwValidTime )
{
	CEngineObject* pEngineObject = GetEngineObject();
	if( pEngineObject == NULL )
		return;

	pEngineObject->ApplyHeightField( !bApplyGravity );
	
	m_MoveInfo.bApplyGravity = bApplyGravity;
	m_MoveInfo.dwValidGravityTime = gCurTime + dwValidTime;
	m_MoveInfo.fGravityAcceleration = fGravityAcceleration;
	m_MoveInfo.fGravitySpeed = 0.f;

	if( bApplyGravity == FALSE )
	{
		VECTOR3 curPosition={0};
		GetPosition( &curPosition );
		g_pExecutive->GXMGetHFieldHeight( &curPosition.y, curPosition.x, curPosition.z );
		m_MoveInfo.CurPosition.y = curPosition.y;
		SetPosition( &curPosition );
	}
}