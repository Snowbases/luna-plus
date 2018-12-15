// EngineObject.cpp: implementation of the CEngineObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EngineObject.h"
#include "GraphicEngine.h"

#include "../MHFile.h"
#include "EngineObjectCache.h"
#include "./Effect/Effect.h"
#include "./Effect/EffectManager.h"

CAniInfoManager CEngineObject::m_staAniMgr;
extern VECTOR3 gHeroPos;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngineObject::CEngineObject()
{
	m_pCacheListData = NULL;
	m_fAlpha = 1.0f;	
	m_GXOHandle = NULL;
	m_CurMotionNum = -1;  
	m_bMotionPause = FALSE;
	m_BaseMotionNum = 1;
	m_AniInfo = NULL;
	m_bApplyHeight = FALSE;
	m_pObjectLinkTo = NULL;
	// 090422 ShinJS --- Attach 부모에 대한 정보 초기화
	m_pParentEngineObj = NULL;
	m_byAttachedType = eEngineAttachType_None;
	m_dwLinkToEffectID = 0;
}

CEngineObject::~CEngineObject()
{
	RemoveAllAttachedRider();
	Release();
}

void CEngineObject::PreLoadObject(char* filename)
{
	g_pExecutive->PreLoadGXObject(filename);
}


BOOL CEngineObject::Init(char* filename,CObjectBase* pObject,BYTE EngineObjectType)
{
	m_NotProcessedAnimateKey = 0;
	
	if(m_GXOHandle == NULL)
		m_CurMotionNum = 1;

	Release();
	
	//m_CurMotionNum = -1;
	//m_bAnimationRoop = TRUE;
	m_bMotionPause = FALSE;	
	m_bEndMotion = FALSE;

	m_pObject = pObject;
	if(m_pObject)
		m_pObject->m_EngineObjectType = EngineObjectType;
	
	DWORD dwFlag = 0;
	if( EngineObjectType == eEngineObjectType_Effect )
		dwFlag = GXOBJECT_CREATE_TYPE_EFFECT;	

	//trustpak 2005/06/27
	if ( EngineObjectType == eEngineObjectType_GameSystemObject)
	{
		dwFlag = GXOBJECT_CREATE_TYPE_EFFECT;
	}

	if (EngineObjectType == eEngineObjectType_Shadow)
	{
		dwFlag = GXOBJECT_CREATE_TYPE_EFFECT;
	}
	///

	if(EngineObjectType == eEngineObjectType_Character)
		DIRECTORYMGR->SetLoadMode(eLM_Character);
	else if(EngineObjectType == eEngineObjectType_Monster)
		DIRECTORYMGR->SetLoadMode(eLM_Monster);
	else if(EngineObjectType == eEngineObjectType_Npc)
		DIRECTORYMGR->SetLoadMode(eLM_Npc);
	else if(EngineObjectType == eEngineObjectType_Effect)
		DIRECTORYMGR->SetLoadMode(eLM_Effect);
	else if(EngineObjectType == eEngineObjectType_Weapon)
		DIRECTORYMGR->SetLoadMode(eLM_Character);
	else if(EngineObjectType == eEngineObjectType_Pet)
		DIRECTORYMGR->SetLoadMode(eLM_Pet);
	else if(EngineObjectType == eEngineObjectType_GameSystemObject)
		DIRECTORYMGR->SetLoadMode(eLM_Effect);
	else if(EngineObjectType == eEngineObjectType_Shadow)
		DIRECTORYMGR->SetLoadMode(eLM_Effect);
	else if(EngineObjectType == eEngineObjectType_MapObject)
		DIRECTORYMGR->SetLoadMode(eLM_MapObject);
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.05.07	2007.10.10
	else if(EngineObjectType == eEngineObjectType_FarmRenderObj)
		DIRECTORYMGR->SetLoadMode(eLM_Farm);
	// E 농장시스템 추가 added by hseos 2007.05.07	2007.10.10
	else if(EngineObjectType == eEngineObjectType_Vehicle)
		DIRECTORYMGR->SetLoadMode(eLM_Monster);
	else if(EngineObjectType == eEngineObjectType_Furniture)
		DIRECTORYMGR->SetLoadMode(eLM_Housing);

	PreLoadObject(filename);	//KES 맵이동 할때까지 해제 안한다. 070805

	m_GXOHandle = g_pEngineObjectCache->CreateGXObjectThroughCache(filename, MHPlayerPROC, this, dwFlag);
	
	if(m_GXOHandle == NULL)
	{
		FILE* fp = fopen("MissedObjectList.txt","a+");
		if(fp)
		{
			fprintf(fp,"%s\n",filename);
			fclose(fp);
		}
		DIRECTORYMGR->SetLoadMode(eLM_Root);
		return FALSE;
	}


	if(EngineObjectType == eEngineObjectType_Effect)
		EnableShadow(FALSE);

	//trustpak 2005/06/27
	if (EngineObjectType == eEngineObjectType_GameSystemObject)
		EnableShadow(FALSE);

	if (EngineObjectType == eEngineObjectType_Shadow)
		EnableShadow(FALSE);
	///
	
	//yh2do test
	//g_pExecutive->EnableUpdateShading(m_GXOHandle);
	if(strcmp( "E_Man.chx" ,   filename ) == 0 )
	{
		int a=0;
		a++;
	}
	m_AniInfo = m_staAniMgr.GetAniInfoObj(filename,m_GXOHandle);

	DIRECTORYMGR->SetLoadMode(eLM_Root);
	
	//FILE* fp = fopen("
	if(m_CurMotionNum == -1)
		ChangeMotion(1);
	SetEngineToCurmotion();

	m_AnimateRate = 1.0f;
	m_SkipRate = 0.0f;

	// 090422 ShinJS --- 부모에 대한 정보가 있는경우 다시 Attach 시켜준다
	AttachParent();
	// 090422 ShinJS --- Rider로 저장된 Object에 Attach한다
	AllAttachRider();
	return TRUE;
}

void CEngineObject::Release()
{
	// 090422 ShinJS --- 자신이 Attach되어 있는 경우 부모에게 자신을 Detach시키도록 한다(Attach List에서 제거하지 않음, Init시 다시 Attach 가능)
	DetachParent();
	// 090422 ShinJS --- Rider로 저장된 Object를 Detach한다
	AllDetachRider();

	if(m_pObjectLinkTo)
	{
		m_pObjectLinkTo->m_LinkedObject.Remove(this);
		// 100603 ShinJS --- Link된 Effect의 상태를 Reset시켜 다시 Link하도록 한다.
		m_pObjectLinkTo->m_setLinkedEffectID.erase( m_dwLinkToEffectID );
		CEffect* pLinkedEffect = EFFECTMGR->GetEffect( m_dwLinkToEffectID );
		if( pLinkedEffect )
		{
			m_dwLinkToEffectID = 0;
			pLinkedEffect->Reset();
		}
		m_pObjectLinkTo = NULL;
	}
	PTRLISTSEARCHSTART(m_LinkedObject,CEngineObject*,pLinkChild)
		pLinkChild->LinkTo(NULL, 0);
	PTRLISTSEARCHEND

	m_bApplyHeight = FALSE;
	RemoveAllAttachedWeapon();
	RemoveAllAttachedDress();
	if(m_GXOHandle)
	{
		AddGarbageObject(m_GXOHandle);
		Hide();
		m_GXOHandle = NULL;
	}
	if(m_AniInfo)
	{
		m_AniInfo = NULL;
	}
}

void CEngineObject::Animate(int animatekey)
{
	if(animatekey == 0)
		return;
	if(m_bMotionPause)
		return;
	if(m_AniInfo->GetMaxAniNum() == 0)
		return;

	//////////////////////////////////////////////////////////////////////////
	// 게임이 느리면 한번 걸러 한번씩 에니메이션을 처리한다.
	if(IsGameSlow() == TRUE)
	{
		VECTOR3 thispos;
		int SkipAnimateCount;
		g_pExecutive->GXOGetPosition(m_GXOHandle,&thispos);
		float gx = thispos.x - gHeroPos.x;
		float gz = thispos.z - gHeroPos.z;
		float dist = gx*gx + gz*gz;
		if(dist < 490000)	// 700*700
			SkipAnimateCount = 0;
		else if(dist < 4000000)		// 2000*2000
			SkipAnimateCount = 3;
		else
			SkipAnimateCount = 7;
		if(m_NotProcessedAnimateKey < SkipAnimateCount)
		{
			m_NotProcessedAnimateKey += animatekey;
			return;
		}
	}
	if(m_NotProcessedAnimateKey != 0)
	{
		animatekey += m_NotProcessedAnimateKey;
		m_NotProcessedAnimateKey = 0;
	}
	//////////////////////////////////////////////////////////////////////////
		
	DWORD	dwMotionIndex = g_pExecutive->GXOGetCurrentMotionIndex(m_GXOHandle);
	DWORD	dwFrame = g_pExecutive->GXOGetCurrentFrame(m_GXOHandle);
	MOTION_DESC pMotionDesc;
	if(dwMotionIndex == 0)
		dwMotionIndex = 1;
	g_pExecutive->GXOGetMotionDesc(m_GXOHandle,&pMotionDesc,dwMotionIndex,0);

	////////////////////////////
	/// 07. 08. 09 이영준
	/// 에니메이션 속도 조절
	float rate = (float)(int)m_AnimateRate;
	int skip = 0;
	
	m_SkipRate += (m_AnimateRate - rate);

	if( m_SkipRate >= 1.0f )
	{
		skip = 1;
		m_SkipRate = 0.0f;
	}
	////////////////////////////


	animatekey = (int)(animatekey * m_AnimateRate + animatekey * skip);

	DWORD CurFrame = dwFrame+animatekey;
	
//	if( m_AnimateRate > 1.0f )
//	{
//		printf( "%d\n", CurFrame );
//	}

	if(CurFrame > pMotionDesc.dwLastFrame || CurFrame > m_pCurAniInfo->EndFrame)
	{
		if(m_bAnimationRoop)
		{
			g_pExecutive->GXOSetCurrentFrame(m_GXOHandle, m_pCurAniInfo->StartFrame);
			m_bEndMotion = TRUE;
		}
		else
		{
			m_bEndMotion = TRUE;
			ChangeMotion(m_BaseMotionNum);
		}
	}
	else
	{
		g_pExecutive->GXOSetCurrentFrame(m_GXOHandle, CurFrame);
		m_bEndMotion = FALSE;
	}
	
	if( m_bEndMotion )
	{
		m_SkipRate = 0.0f;
	}
}

void CEngineObject::SetEngineToCurmotion()
{
	if(m_GXOHandle == NULL)		return;

	m_pCurAniInfo = m_AniInfo->GetAniInfo(m_CurMotionNum);
	g_pExecutive->GXOSetCurrentMotionIndex(m_GXOHandle,m_pCurAniInfo->EngineMotionIndex);
	g_pExecutive->GXOSetCurrentFrame(m_GXOHandle, m_pCurAniInfo->StartFrame);
}

//////////////////////////////////////////////////////////////////////////
void CEngineObject::SetEngObjPosition(VECTOR3* pPos)
{
	if(m_GXOHandle == NULL)		return;
	
	
//	ASSERTMSG(pPos->x > -1000,"이 메세지 도 윤호한테 연락을... Set");
//	ASSERTMSG(pPos->z > -1000,"이 메세지 도 윤호한테 연락을... Set");

	VECTOR3 position;
	if(g_bFixHeight)
	{
		position.x = pPos->x;
		position.z = pPos->z;
		
		if(m_bApplyHeight)
			position.y = g_fFixHeight;
		else
			position.y = pPos->y;

		g_pExecutive->GXOSetPosition(m_GXOHandle,&position,FALSE);
	}
	else
		g_pExecutive->GXOSetPosition(m_GXOHandle,pPos,FALSE);

	if(m_LinkedObject.GetCount() != 0)
	{
		GetEngObjPosition(&position);
		PTRLISTSEARCHSTART(m_LinkedObject,CEngineObject*,pLinkChild)
			pLinkChild->SetEngObjPosition(&position);
		PTRLISTSEARCHEND
	}
}

void CEngineObject::SetEngObjAngle(float angleRad)
{
	if(m_GXOHandle == NULL)		return;

	static VECTOR3 axis = { 0,1,0};
	g_pExecutive->GXOSetDirection(m_GXOHandle,&axis,angleRad);
}

void CEngineObject::SetEngObjAngleX(float angleRad)
{
	if(m_GXOHandle == NULL)		return;

	static VECTOR3 axis = { 1,0,0};
	g_pExecutive->GXOSetDirection(m_GXOHandle,&axis,angleRad);
}
void CEngineObject::SetEngObjAngleZ(float angleRad)
{
	if(m_GXOHandle == NULL)		return;

	static VECTOR3 axis = { 0,0,1};
	g_pExecutive->GXOSetDirection(m_GXOHandle,&axis,angleRad);
}

void CEngineObject::RotateEngObj( float x, float y, float z )
{
	if(m_GXOHandle == NULL)		return;

	if( x != 0.0f ) SetEngObjAngleX( x );
	if( y != 0.0f ) SetEngObjAngle( y );
	if( z != 0.0f ) SetEngObjAngleZ( z );
}

void CEngineObject::RotateEngObj( VECTOR3 vRot )
{
	if(m_GXOHandle == NULL)		
	{
		return;
	}

	if( vRot.x != 0.0f ) SetEngObjAngleX( vRot.x );
	if( vRot.y != 0.0f ) SetEngObjAngle( vRot.y );
	if( vRot.z != 0.0f ) SetEngObjAngleZ( vRot.z );
}

void CEngineObject::GetEngObjPosition(VECTOR3* pPos)
{
	if(m_GXOHandle == NULL)
	{
		pPos->x = pPos->y = pPos->z = 0;
		return;
	}

	g_pExecutive->GXOGetPosition(m_GXOHandle,pPos);
}

float CEngineObject::GetEngObjAngle()
{
	if(m_GXOHandle == NULL)		return 0;

	static VECTOR3 axis = { 0,1,0};
	float angle;
	g_pExecutive->GXOGetDirection(m_GXOHandle,&axis,&angle);
	return angle;
}

void CEngineObject::Show()
{
	if(m_GXOHandle == NULL)		return;

	if(m_CurMotionNum == -1)
	{
		ChangeMotion(1);
	}
	
	g_pExecutive->GXOSetCurrentFrame(m_GXOHandle, m_pCurAniInfo->StartFrame);
	m_bEndMotion = FALSE;
	g_pExecutive->EnableSchedule(m_GXOHandle);
	g_pExecutive->EnableRender(m_GXOHandle);

	// 070503 LYW --- EngineObject : Add for check show.
	m_bShow = TRUE ;
}

void CEngineObject::Hide()
{
	if(m_GXOHandle == NULL)		return;
	if(g_pExecutive == NULL)	return;

	g_pExecutive->DisableSchedule(m_GXOHandle);
	g_pExecutive->DisableRender(m_GXOHandle);

	// 070503 LYW --- EngineObject : Add for check show.
	m_bShow = FALSE ;
}

void CEngineObject::HideWithScheduling()
{
	if(m_GXOHandle == NULL)		return;

	g_pExecutive->EnableSchedule(m_GXOHandle);
	g_pExecutive->DisableRender(m_GXOHandle);
}

void CEngineObject::ChangeBaseMotion(int BaseMotionNum)
{
	m_BaseMotionNum = BaseMotionNum;
}

void CEngineObject::ChangeMotion(int MotionNum,BOOL bRoop,float rate)
{
	if(m_GXOHandle == NULL)		return;

	m_AnimateRate = rate;

	if(m_CurMotionNum != MotionNum)
	{
		m_CurMotionNum = MotionNum;

		m_pCurAniInfo = m_AniInfo->GetAniInfo(m_CurMotionNum);
		g_pExecutive->GXOSetCurrentMotionIndex(m_GXOHandle,m_pCurAniInfo->EngineMotionIndex);
		g_pExecutive->GXOSetCurrentFrame(m_GXOHandle, m_pCurAniInfo->StartFrame);
		m_bMotionPause = FALSE;
	}
	m_bAnimationRoop = bRoop;
}

void CEngineObject::ChangeCustumMotion(int MotionNum,int StartFrame,int EndFrame,BOOL bRoop,float rate)
{
	if(m_GXOHandle == NULL)		return;

	m_AnimateRate = rate;
	m_CurMotionNum = MotionNum;
	m_CustumAniInfo.EngineMotionIndex = WORD(MotionNum);
	m_CustumAniInfo.StartFrame = WORD(StartFrame);
	m_CustumAniInfo.EndFrame = WORD(EndFrame);
	m_pCurAniInfo = &m_CustumAniInfo;
	
	g_pExecutive->GXOSetCurrentMotionIndex(m_GXOHandle,m_pCurAniInfo->EngineMotionIndex);
	g_pExecutive->GXOSetCurrentFrame(m_GXOHandle, m_pCurAniInfo->StartFrame);
	m_bMotionPause = FALSE;

	m_bAnimationRoop = bRoop;
}

BOOL CEngineObject::Attach(CEngineObject* pToObject,char* szObjectName,BOOL enableScale/*=FALSE*/)
{
	if(m_GXOHandle == NULL)		return FALSE;
	
	if (NULL == pToObject->m_GXOHandle)
	{
		return FALSE;
	}

	if (enableScale)
		g_pExecutive->GXOEnableScaleOfAttachedModel(m_GXOHandle);
	else
		g_pExecutive->GXODisableScaleOfAttachedModel(m_GXOHandle);

	return g_pExecutive->GXOAttach(m_GXOHandle,pToObject->m_GXOHandle,szObjectName);
}

void CEngineObject::Detach(CEngineObject* pChildObject)
{
	if(m_GXOHandle == NULL)		return;
	//090902 pdy GXOHandle이 유효한지 채크 추가
	if( pChildObject == NULL )  return;
	if( pChildObject->m_GXOHandle == NULL ) return;

	g_pExecutive->GXODetach(m_GXOHandle,pChildObject->m_GXOHandle);
}

void CEngineObject::DrawFitInViewPort(int ViewPortNum,float fFOV)
{
	if(m_GXOHandle == NULL)		return;

	g_pExecutive->SetCameraFitGXObject(m_GXOHandle,1,1000,fFOV,ViewPortNum);
	g_pExecutive->GetGeometry()->SetAmbientColor(ViewPortNum,0xffffffff);
	g_pExecutive->GetGeometry()->BeginRender(ViewPortNum,0xffff0000,BEGIN_RENDER_FLAG_DONOT_CLEAR_FRAMEBUFFER);
	g_pExecutive->GXORender(m_GXOHandle);
	g_pExecutive->GetGeometry()->EndRender();
}

void CEngineObject::SetScale(VECTOR3* pScale)
{
	if(m_GXOHandle == NULL)		return;

	g_pExecutive->GXOSetScale(m_GXOHandle,pScale);
}

void CEngineObject::GetScale(VECTOR3* pScale)
{
	if(m_GXOHandle == NULL)		return;

	g_pExecutive->GXOGetScale(m_GXOHandle,pScale);
}

BOOL CEngineObject::ChangePart(int PartNum,char* pFileName)
{
	if(m_GXOHandle == NULL)		return FALSE;

	if(m_pObject->m_EngineObjectType == eEngineObjectType_Character)
		DIRECTORYMGR->SetLoadMode(eLM_Character);
	else if(m_pObject->m_EngineObjectType == eEngineObjectType_Monster)
		DIRECTORYMGR->SetLoadMode(eLM_Monster);
	else if(m_pObject->m_EngineObjectType == eEngineObjectType_Npc)
		DIRECTORYMGR->SetLoadMode(eLM_Npc);
	else if(m_pObject->m_EngineObjectType == eEngineObjectType_Effect)
		DIRECTORYMGR->SetLoadMode(eLM_Effect);
	else if(m_pObject->m_EngineObjectType == eEngineObjectType_Weapon)
		DIRECTORYMGR->SetLoadMode(eLM_Character);
	else if(m_pObject->m_EngineObjectType == eEngineObjectType_Pet)
		DIRECTORYMGR->SetLoadMode(eLM_Monster);
	else if(m_pObject->m_EngineObjectType == eEngineObjectType_MapObject )
		DIRECTORYMGR->SetLoadMode(eLM_MapObject);

	BOOL rt = g_pExecutive->GXOReplaceModel(m_GXOHandle,PartNum,pFileName);

	
		DIRECTORYMGR->SetLoadMode(eLM_Root);
	return rt;
}

void CEngineObject::ApplyHeightField(BOOL bApply)
{
	if(m_GXOHandle == NULL)		return;

	m_bApplyHeight = bApply;
	if(g_bFixHeight)
		return;
	
	if(bApply)
		g_pExecutive->GXOEnableHFieldApply(m_GXOHandle);
	else
		g_pExecutive->GXODisableHFieldApply(m_GXOHandle);
}

void CEngineObject::EnableShadow(BOOL bShadow)
{
	if(m_GXOHandle == NULL)		return;

	if(bShadow)
		g_pExecutive->EnableSendShadow(m_GXOHandle);
	else
		g_pExecutive->DisableSendShadow(m_GXOHandle);
	
	PTRLISTSEARCHSTART(m_AttachedWeapon,CEngineObject*,pWeapon)
		pWeapon->EnableShadow(bShadow);
	PTRLISTSEARCHEND
	
	PTRLISTSEARCHSTART(m_AttachedDress,CEngineObject*,pDress)
		pDress->EnableShadow(bShadow);
	PTRLISTSEARCHEND

	// 090422 ShinJS --- 탈것 추가작업
	PTRLISTSEARCHSTART(m_AttachedRider,CEngineObject*,pRider)
		pRider->EnableShadow(bShadow);
	PTRLISTSEARCHEND
}

void CEngineObject::SetAlpha(float Alpha)
{
	if(m_GXOHandle == NULL)		return;

	m_fAlpha = Alpha;

	DWORD dAlpha = (DWORD)(Alpha * 255);
	if(dAlpha == 0)
		dAlpha = 1;
	
	g_pExecutive->SetAlphaFlag(m_GXOHandle,dAlpha);
	
	
	PTRLISTSEARCHSTART(m_AttachedWeapon,CEngineObject*,pWeapon)
		pWeapon->SetAlpha(Alpha);
	PTRLISTSEARCHEND
	
	PTRLISTSEARCHSTART(m_AttachedDress,CEngineObject*,pDress)
		pDress->SetAlpha(Alpha);
	PTRLISTSEARCHEND

	// 090422 ShinJS --- 탈것 추가작업
	PTRLISTSEARCHSTART(m_AttachedRider,CEngineObject*,pRider)
		pRider->SetAlpha(Alpha);
	PTRLISTSEARCHEND
}

void CEngineObject::EnablePick()
{
	if(m_GXOHandle == NULL)		return;

	g_pExecutive->EnablePick(m_GXOHandle);
/*	
	PTRLISTSEARCHSTART(m_AttachedWeapon,CEngineObject*,pWeapon)
		pWeapon->EnablePick();
	PTRLISTSEARCHEND
	
	PTRLISTSEARCHSTART(m_AttachedDress,CEngineObject*,pDress)
		pDress->EnablePick();
	PTRLISTSEARCHEND
*/
}

void CEngineObject::DisablePick()
{
	if(m_GXOHandle == NULL)		return;

	g_pExecutive->DisablePick(m_GXOHandle);
/*	
	PTRLISTSEARCHSTART(m_AttachedWeapon,CEngineObject*,pWeapon)
		pWeapon->DisablePick();
	PTRLISTSEARCHEND
	
	PTRLISTSEARCHSTART(m_AttachedDress,CEngineObject*,pDress)
		pDress->DisablePick();
	PTRLISTSEARCHEND
*/
}

void CEngineObject::EnableSelfIllumin()
{
	if(m_GXOHandle == NULL)		return;

	g_pExecutive->GXOEnableSelfIllumin(m_GXOHandle);
	
	PTRLISTSEARCHSTART(m_AttachedWeapon,CEngineObject*,pWeapon)
		pWeapon->EnableSelfIllumin();
	PTRLISTSEARCHEND
	
	PTRLISTSEARCHSTART(m_AttachedDress,CEngineObject*,pDress)
		pDress->EnableSelfIllumin();
	PTRLISTSEARCHEND

	// 090422 ShinJS --- 탈것 추가작업
	PTRLISTSEARCHSTART(m_AttachedRider,CEngineObject*,pRider)
		pRider->EnableSelfIllumin();
	PTRLISTSEARCHEND
}

void CEngineObject::DisableSelfIllumin()
{
	if(m_GXOHandle == NULL)		return;

	g_pExecutive->GXODisableSelfIllumin(m_GXOHandle);
	
	PTRLISTSEARCHSTART(m_AttachedWeapon,CEngineObject*,pWeapon)
		pWeapon->DisableSelfIllumin();
	PTRLISTSEARCHEND
	
	PTRLISTSEARCHSTART(m_AttachedDress,CEngineObject*,pDress)
		pDress->DisableSelfIllumin();
	PTRLISTSEARCHEND

	// 090422 ShinJS --- 탈것 추가작업
	PTRLISTSEARCHSTART(m_AttachedRider,CEngineObject*,pRider)
		pRider->DisableSelfIllumin();
	PTRLISTSEARCHEND
}

BOOL CEngineObject::IsVisible()
{
	if(m_GXOHandle == NULL)		return FALSE;
	// 090422 ShinJS --- 플레이어가 Attach된 경우(탈것에 탄경우) IsVisible이 정상적이지 않아 ParentObject의 IsVisible을 이용하도록 수정
	if( m_pObject->GetEngineObjectType() == eEngineObjectType_Character && m_pParentEngineObj != NULL )
		return m_pParentEngineObj->IsVisible();

	return g_pExecutive->IsInViewVolume(m_GXOHandle);
}

DWORD CEngineObject::GetAnimationTime(DWORD MotionNum)
{
	if(m_GXOHandle == NULL)		return 1;
	if(MotionNum == (DWORD)-1)	return 0;

	ANIINFO* AniInfo = m_AniInfo->GetAniInfo(MotionNum);
	MOTION_DESC pMotionDesc;
	g_pExecutive->GXOGetMotionDesc(m_GXOHandle,&pMotionDesc,AniInfo->EngineMotionIndex,0);

	DWORD lastFrame = pMotionDesc.dwLastFrame < AniInfo->EndFrame ?
							pMotionDesc.dwLastFrame : AniInfo->EndFrame;
	DWORD startFrame = AniInfo->StartFrame;

	if(startFrame > lastFrame)
	{
		MessageBox(NULL,"StartFrame is bigger than LastFrame. Check chi File",0,0);
		return 0;
	}
	DWORD frame = lastFrame - startFrame;
	
	return (DWORD)(frame * gTickPerFrame);
}

void CEngineObject::RandMotionFrame()
{
	if(m_GXOHandle == NULL)		return;

	DWORD MotionNum = m_CurMotionNum;
	MOTION_DESC pMotionDesc;
	g_pExecutive->GXOGetMotionDesc(m_GXOHandle,&pMotionDesc,MotionNum,0);

	ANIINFO* AniInfo = m_AniInfo->GetAniInfo(MotionNum);
	DWORD aniinfo = AniInfo->EndFrame - AniInfo->StartFrame;

	DWORD frame = pMotionDesc.dwLastFrame < aniinfo ? 
					pMotionDesc.dwLastFrame : aniinfo;
	
	if(pMotionDesc.dwLastFrame == 0)
	{
		if(AniInfo->EndFrame == (WORD)-1)
		{
			g_pExecutive->GXOGetMotionDesc(m_GXOHandle,&pMotionDesc,AniInfo->EngineMotionIndex,0);
			frame = pMotionDesc.dwLastFrame - AniInfo->StartFrame;
		}
		else
			frame = aniinfo;
	}

	DWORD rrr = 0;
	if(frame)
		rrr = rand() % frame;

	g_pExecutive->GXOSetCurrentFrame(m_GXOHandle,rrr);
}
void CEngineObject::SetDirection(VECTOR3* pDir)
{
	if(m_GXOHandle == NULL)		return;

	if(pDir->x == 0 && pDir->y == 0 && pDir->z == 0)
		return;

	VECTOR3 ypr;
	ypr.x = (float)(atan2(pDir->y, roughGetLength(pDir->x,pDir->z)));
	ypr.y = -(float)(atan2(pDir->z, pDir->x) + gPHI/2.f);
	ypr.z = 0;

	g_pExecutive->GXOSetDirectionFPSStyle(m_GXOHandle,&ypr);
}

void CEngineObject::AttachToCamera(float fDist)
{
	if(m_GXOHandle == NULL)		return;
	g_pExecutive->GXOAttachCameraFront(m_GXOHandle,fDist);
}
void CEngineObject::DetachFromCamera()
{
	if(m_GXOHandle == NULL)		return;
	g_pExecutive->GXODetachCameraFront(m_GXOHandle);
}

//////////////////////////////////////////////////////////////////////////

BOOL CEngineObject::AttachWeapon(CEngineObject* pWeaponObject,char* szObjectName)
{
	BOOL rt = pWeaponObject->Attach(this,szObjectName);
	if(rt)
	{
		// 090422 ShinJS --- Attach 하는경우 부모에 대한 정보와 Attach 정보 저장
		pWeaponObject->SetParentEngineObj( this, eEngineAttachType_Weapon, szObjectName );
		pWeaponObject->DisablePick();
		m_AttachedWeapon.AddHead(pWeaponObject);
	}

	return rt;
}

BOOL CEngineObject::AttachDress(CEngineObject* pDressObject,char* szObjectName)
{
	BOOL rt = pDressObject->Attach(this,szObjectName);
	if(rt)
	{
		// 090422 ShinJS --- Attach 하는경우 부모에 대한 정보와 Attach 정보 저장
		pDressObject->SetParentEngineObj( this, eEngineAttachType_Dress, szObjectName );
		pDressObject->DisablePick();
		m_AttachedDress.AddHead(pDressObject);
	}

	return rt;
}

void CEngineObject::RemoveAllAttachedWeapon()
{
	for(CEngineObject* engineObject = (CEngineObject*)m_AttachedWeapon.RemoveTail();
		0 < engineObject;
		engineObject = (CEngineObject*)m_AttachedWeapon.RemoveTail())
	{
		Detach(
			engineObject);
		SAFE_DELETE(
			engineObject);
	}
}

void CEngineObject::RemoveAllAttachedDress()
{
	for(CEngineObject* engineObject = (CEngineObject*)m_AttachedDress.RemoveTail();
		0 < engineObject;
		engineObject = (CEngineObject*)m_AttachedDress.RemoveTail())
	{
		Detach(
			engineObject);
		SAFE_DELETE(
			engineObject);
	}
}

void CEngineObject::LinkTo(CEngineObject* pParent, DWORD dwEffectID )
{
	if(pParent != m_pObjectLinkTo)
	{
		if(m_pObjectLinkTo)
		{
			m_pObjectLinkTo->m_LinkedObject.Remove(this);
			// 100603 ShinJS --- Link된 Effect의 상태를 Reset시켜 다시 Link하도록 한다.
			m_pObjectLinkTo->m_setLinkedEffectID.erase( m_dwLinkToEffectID );
			CEffect* pLinkedEffect = EFFECTMGR->GetEffect( m_dwLinkToEffectID );
			if( pLinkedEffect )
			{
				m_dwLinkToEffectID = 0;
				pLinkedEffect->Reset();
			}
		}

		if(pParent)
			pParent->m_LinkedObject.AddTail(this);

		m_pObjectLinkTo = pParent;
	}

	m_dwLinkToEffectID = dwEffectID;
}


DWORD __stdcall MHPlayerPROC(I4DyuchiGXExecutive* pExecutive, GXMAP_OBJECT_HANDLE gxh, DWORD msg, int arg1, int arg2, void* pData)
{
	CEngineObject* pObject = (CEngineObject*)pData;
	if(pObject == NULL)
		return 0;

	if(arg1)
		pObject->Animate(arg1);

	if(pObject->m_pObject)
		pObject->m_pObject->Process();

	return 0;
}

void CEngineObject::SetCacheListData(void* pData)
{
	m_pCacheListData = pData;
}
void* CEngineObject::GetCacheListData(void)
{
	return m_pCacheListData;
}

VOID CEngineObject::SetMaterialIndex(DWORD dwIndex)
{
	if (NULL == m_GXOHandle)
	{
		return;
	}

	// Index의 범위가 넘어가더라도 내부에서 예외처리를 하기 때문에 아무런 이상 없음. 
	// chr이나 chx에 명시된 MTL 파일의 순서가 Index가 된다. 
	g_pExecutive->GXOSetCurrentMaterialIndex(m_GXOHandle, dwIndex);
}

// 090422 ShinJS --- Attach/Detach 시 부모에 대한 정보 설정
void CEngineObject::SetParentEngineObj( CEngineObject* pParentEngineObj, BYTE byAttachType, char* szObjectName )
{
	m_pParentEngineObj = pParentEngineObj;
	m_byAttachedType = byAttachType;
	if( szObjectName )
		SafeStrCpy( m_szAttachedObjName, szObjectName, 32 );
}

// 090422 ShinJS --- 부모를 Attach한다
void CEngineObject::AttachParent()
{
	// 부모에 대한 정보가 있는경우 다시 Attach 시켜준다
	if( m_pParentEngineObj )
	{
		// Attach 종류
		switch( m_byAttachedType )
		{
		case eEngineAttachType_Weapon:
			m_pParentEngineObj->AttachWeapon( this, m_szAttachedObjName );
			break;

		case eEngineAttachType_Dress:
			m_pParentEngineObj->AttachDress( this, m_szAttachedObjName );
			break;

		case eEngineAttachType_Rider:
			m_pParentEngineObj->AttachRider( this, m_szAttachedObjName );
			break;
		}
	}
}

// 090422 ShinJS --- 부모를 Detach한다.
void CEngineObject::DetachParent()
{
	// 자신이 Attach되어 있는 경우 부모에게 자신을 Detach시키도록 한다(부모정보 제거하지 않음, Init시 다시 Attach 가능)
	if( m_pParentEngineObj )
	{
		switch( m_byAttachedType )
		{
		case eEngineAttachType_Weapon:
			m_pParentEngineObj->DetachWeapon( this, FALSE );
			break;

		case eEngineAttachType_Dress:
			m_pParentEngineObj->DetachDress( this, FALSE );
			break;

		case eEngineAttachType_Rider:
			m_pParentEngineObj->DetachRider( this, FALSE );
			break;
		}
	}
}

// 090422 ShinJS --- Rider로 저장된 Object에 Attach한다
void CEngineObject::AllAttachRider()
{
	// 090422 ShinJS --- 탑승자의 정보가 있는경우 다시 Attach 시켜준다
	PTRLISTSEARCHSTART(m_AttachedRider,CEngineObject*,pRider)
		BOOL rt = pRider->Attach(this, pRider->GetAttachedObjName() );
		if(rt)
		{
			pRider->SetParentEngineObj( this, eEngineAttachType_Rider, NULL );
		}
	PTRLISTSEARCHEND
}

// 090422 ShinJS --- Rider로 저장된 Object를 Detach한다
void CEngineObject::AllDetachRider()
{
	PTRLISTSEARCHSTART(m_AttachedRider,CEngineObject*,pRider)
		Detach( pRider );
		pRider->SetParentEngineObj( NULL, eEngineAttachType_None, NULL );
	PTRLISTSEARCHEND
}

// 090422 ShinJS --- Rider로 저장된 Object를 List에서 제거 후 Detach한다
void CEngineObject::RemoveAllAttachedRider()
{
	for(CEngineObject* engineObject = (CEngineObject*)m_AttachedRider.RemoveTail();
		0 < engineObject;
		engineObject = (CEngineObject*)m_AttachedRider.RemoveTail())
	{
		Detach(
			engineObject);
		engineObject->SetParentEngineObj(
			0,
			eEngineAttachType_None,
			0);
	}
}

BOOL CEngineObject::AttachRider( CEngineObject* pRiderObject, char* szObjectName )
{
	BOOL rt = pRiderObject->Attach(this,szObjectName);
	if(rt)
	{
		// Attach 하는경우 부모에 대한 정보와 Attach 정보 저장
		pRiderObject->SetParentEngineObj( this, eEngineAttachType_Rider, szObjectName );
		m_AttachedRider.AddHead( pRiderObject );
	}

	return rt;
}

void CEngineObject::DetachWeapon( CEngineObject* pWeaponObject, BOOL bInitParent )
{
	PTRLISTSEARCHSTART(m_AttachedWeapon,CEngineObject*,pWeapon)
	if( pWeapon == pWeaponObject )
	{
		Detach( pWeapon );
		m_AttachedWeapon.Remove( pWeapon );
		if( bInitParent )
			pWeapon->SetParentEngineObj( NULL, eEngineAttachType_None, NULL );

		break;
	}
	PTRLISTSEARCHEND
}

void CEngineObject::DetachDress( CEngineObject* pDressObject, BOOL bInitParent )
{
	PTRLISTSEARCHSTART(m_AttachedDress,CEngineObject*,pDress)
	if( pDress == pDressObject )
	{
		Detach( pDress );
		m_AttachedDress.Remove( pDress );
		if( bInitParent )
			pDress->SetParentEngineObj( NULL, eEngineAttachType_None, NULL );

		break;
	}
	PTRLISTSEARCHEND
}

void CEngineObject::DetachRider( CEngineObject* pRiderObject, BOOL bInitParent )
{
	PTRLISTSEARCHSTART(m_AttachedRider,CEngineObject*,pRider)
	if( pRider == pRiderObject )
	{
		Detach( pRider );
		m_AttachedRider.Remove( pRider );
		if( bInitParent )
			pRider->SetParentEngineObj( NULL, eEngineAttachType_None, NULL );

		break;
	}
	PTRLISTSEARCHEND
}
