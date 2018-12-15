// EngineObject.h: interface for the CEngineObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENGINEOBJECT_H__50E9AA05_A5A2_4CF4_BA4A_4FE8CED446F3__INCLUDED_)
#define AFX_ENGINEOBJECT_H__50E9AA05_A5A2_4CF4_BA4A_4FE8CED446F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GraphicEngine.h"
#include "../AniInfoManager.h"


//trustpak 2005/06/22
/*
enum EEngineObjectType
{
	eEngineObjectType_Character,
	eEngineObjectType_Monster,
	eEngineObjectType_Npc,
	eEngineObjectType_Weapon,
	eEngineObjectType_Effect,
	eEngineObjectType_SkillObject, 
	eEngineObjectType_Shadow,
	eEngineObjectType_Pet,	
};
*/

enum EEngineObjectType
{
	eEngineObjectType_Character,
	eEngineObjectType_Monster,
	eEngineObjectType_Npc,
	eEngineObjectType_Weapon,
	eEngineObjectType_Effect,
	eEngineObjectType_SkillObject, 
	eEngineObjectType_Shadow,
	eEngineObjectType_Pet,	
	eEngineObjectType_GameSystemObject,
	eEngineObjectType_MapObject,
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.05.07
	eEngineObjectType_FarmRenderObj,
	// E 농장시스템 추가 added by hseos 2007.05.07
	eEngineObjectType_Vehicle,						// 090422 ShinJS --- 탈것 추가
	//090521 pdy 하우징 가구타입 추가 
	eEngineObjectType_Furniture,
};
///

// 090422 ShinJS --- Attach/Detach 에 대한 자신의 정보
enum
{
	eEngineAttachType_None,
	eEngineAttachType_Weapon,
	eEngineAttachType_Dress,
	eEngineAttachType_Rider,		// 탑승자
};
 
class CObjectBase
{
	virtual void Process() = 0;

	friend DWORD __stdcall MHPlayerPROC(I4DyuchiGXExecutive* pExecutive, GXMAP_OBJECT_HANDLE gxh, DWORD msg, int arg1, int arg2, void* pData);
	friend DWORD __stdcall MHEffectPROC(I4DyuchiGXExecutive* pExecutive, GXMAP_OBJECT_HANDLE gxh, DWORD msg, int arg1, int arg2, void* pData);

public:
	inline int GetEngineObjectType()	{	return m_EngineObjectType;	}
protected:
	int m_EngineObjectType;

	friend class CEngineObject;
	friend class CEngineEffect;
};

class CEngineObject  
{
private:
	void*		m_pCacheListData;	
	float		m_fAlpha;

public:
	void		SetCacheListData(void* pData);
	void*		GetCacheListData(void);

protected:
	static CAniInfoManager m_staAniMgr;

	BOOL m_bApplyHeight;

	CObjectBase* m_pObject;
	
	GXOBJECT_HANDLE m_GXOHandle;

	BOOL m_bEndMotion;
	BOOL m_bAnimationRoop;
	void Animate(int animatekey);


	int m_BaseMotionNum;
	int m_CurMotionNum;
	bool m_bMotionPause;
	float m_AnimateRate;
	float m_SkipRate;
	
	int m_NotProcessedAnimateKey;

	ANIINFO	m_CustumAniInfo;
	ANIINFO* m_pCurAniInfo;
	CAniInfo* m_AniInfo;
	
	cPtrList m_AttachedWeapon;
	cPtrList m_AttachedDress;

	// 090422 ShinJS --- 탈것 추가작업
	cPtrList m_AttachedRider;				// 탑승자 정보 저장
	// 090422 ShinJS --- Attach/Detach 시 부모와 자신의 Attach 정보
	CEngineObject* m_pParentEngineObj;		// 부모의 EnginObject
	BYTE m_byAttachedType;					// Attached 종류
	char m_szAttachedObjName[32];			// Attached 부위

	cPtrList m_LinkedObject;
	CEngineObject* m_pObjectLinkTo;
	std::set< DWORD > m_setLinkedEffectID;
	DWORD m_dwLinkToEffectID;

	// 070503 LYW --- EngineObject : Add member for check show.
	BOOL m_bShow ;

public:
	CEngineObject();
	virtual ~CEngineObject();

	// 070503 LYW --- EngineObject : Add function to check show.
	BOOL IsShow() { return m_bShow ; }

	static void PreLoadObject(char* filename);
	
	BOOL IsInited()		{	return m_GXOHandle ? TRUE : FALSE;	}

	BOOL Init(char* filename,CObjectBase* pObject,BYTE EngineObjectType);
	
	virtual void Release();

	BOOL IsEndMotion()		{	return m_bEndMotion;	}
	
	void SetEngineToCurmotion();
	
	BOOL IsVisible();

	//////////////////////////////////////////////////////////////////////////
	void SetEngObjPosition(VECTOR3* pPos);
	void GetEngObjPosition(VECTOR3* pPos);
	void SetEngObjAngle(float angleRad); // y축 기준
 	void SetEngObjAngleX(float angleRad); // x축 기준
	void SetEngObjAngleZ(float angleRad); // z축 기준
	void SetDirection(VECTOR3* pDir);
	void RotateEngObj( float x, float y, float z );
	void RotateEngObj( VECTOR3 vRot );
	float GetEngObjAngle();
	void Show();
	void Hide();
	void HideWithScheduling();
	void ChangeBaseMotion(int BaseMotionNum);
	void ChangeMotion(int MotionNum,BOOL bRoop = TRUE,float rate = 1.0f);
	void ChangeCustumMotion(int MotionNum,int StartFrame,int EndFrame,BOOL bRoop = TRUE,float rate = 1.0f);
	void RandMotionFrame();
	BOOL Attach(CEngineObject* pToObject,char* szObjectName,BOOL enableScale = FALSE);
	void Detach(CEngineObject* pChildObject);
	void DrawFitInViewPort(int ViewPortNum,float fFOV = gPHI/2);
	BOOL ChangePart(int PartNum,char* pFileName);
	void ApplyHeightField(BOOL bApply);
	void EnableShadow(BOOL bShadow);
	void SetScale(VECTOR3* pScale);
	void GetScale(VECTOR3* pScale);
	void SetAlpha(float Alpha);		// Alpha = [0,1]
	float GetAlpha(void) { return m_fAlpha; }
	void EnablePick();
	void DisablePick();
	void EnableSelfIllumin();
	void DisableSelfIllumin();
	void AttachToCamera(float fDist);
	void DetachFromCamera();
	int GetCurMotion() const { return m_CurMotionNum; }
	BOOL AttachWeapon(CEngineObject* pWeaponObject,char* szObjectName);
	void RemoveAllAttachedWeapon();
	BOOL AttachDress(CEngineObject* pDressObject,char* szObjectName);
	void RemoveAllAttachedDress();
	void LinkTo(CEngineObject*, DWORD);
	VOID SetMaterialIndex(DWORD dwIndex);

//	
	//090527 pdy 하우징
	CObjectBase* GetBaseObject(){return m_pObject;}
	
	DWORD GetAnimationTime(DWORD MotionNum);
	GXOBJECT_HANDLE GetGXOHandle(VOID)	{ return m_GXOHandle; }	

	friend class CEngineLight;
	friend class CEngineCamera;
	friend DWORD __stdcall MHPlayerPROC(I4DyuchiGXExecutive* pExecutive, GXOBJECT_HANDLE gxh, DWORD msg, int arg1, int arg2, void* pData);
	friend CObjectBase* GetSelectedObject(int MouseX,int MouseY,int PickOption = PICK_TYPE_PER_COLLISION_MESH , BOOL bSort = FALSE );
	friend CObjectBase* GetSelectedObject(PICK_GXOBJECT_DESC* pDesc, int MouseX,int MouseY,int PickOption = PICK_TYPE_PER_COLLISION_MESH , BOOL bSort = FALSE  );
	friend CObjectBase* GetSelectedObjectBoneCheck(int MouseX, int MouseY);

	// 090422 ShinJS --- 탈것 추가작업
private:
	void SetParentEngineObj( CEngineObject* pParentEngineObj, BYTE byAttachType, char* szObjectName );		// 부모에 대한 정보와 자신의 Attach 정보를 저장한다
	void AttachParent();					// 부모를 Attach한다 (Init시    부모존재하는 경우)
	void DetachParent();					// 부모를 Detach한다 (Release시 부모존재하는 경우)
	void AllAttachRider();					// Rider로 저장된 Object를 Attach한다 (Init시 Rider가 있는 경우)
	void AllDetachRider();					// Rider로 저장된 Object를 Detach한다 (Release시 Rider가 있는 경우)
	void RemoveAllAttachedRider();			// Rider로 저장된 Object를 List에서 제거 후 Detach한다

public:
	BOOL AttachRider( CEngineObject* pRiderObject, char* szObjectName );		// Rider Attach
	void DetachWeapon( CEngineObject* pWeaponObject, BOOL bInitParent=TRUE );	// Weapon Detach
	void DetachDress( CEngineObject* pDressObject, BOOL bInitParent=TRUE );		// Dress Detach
	void DetachRider( CEngineObject* pRiderObject, BOOL bInitParent=TRUE );		// Rider Detach
	char* GetAttachedObjName() { return m_szAttachedObjName; }					// 자신의 Attach정보(Attached 부위)를 반환한다
};

#endif // !defined(AFX_ENGINEOBJECT_H__50E9AA05_A5A2_4CF4_BA4A_4FE8CED446F3__INCLUDED_)
