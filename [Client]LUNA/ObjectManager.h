#pragma once


#include "Hero.h"
#include "Monster.h"
#include "BossMonster.h"
#include "Npc.h"
#include "MapObject.h"
#include "Pet.h"
#include "HeroPet.h"
#include "Furniture.h"
// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.05.07
#include "../hseos/Farm/SHFarmRenderObj.h"
// E 농장시스템 추가 added by hseos 2007.05.07
// 090422 ShinJS --- 탈것 추가
#include "Vehicle.h"

#define OBJECTMGR USINGTON(CObjectManager)
#define HERO OBJECTMGR->GetHero()
#define HEROPET OBJECTMGR->GetHeroPet()
#define HEROPETBACKUP OBJECTMGR->GetHeroPetBackup()


class CMonster;
class CNpc;
class CMapObject;
class CSHFarmRenderObj;
class CBossMonster;
class CObject;
class CSkillArea;
class cSkillObject;
class CCharacterCalcManager;
class CMonsterGuageDlg;
class CFurniture;
class CItem;

//class CPet;

enum eOPTION_OVERINFO
{
	eOO_NAME_NONE			= 0,
	eOO_NAME_CLICKEDONLY	= 1,
	eOO_NAME_WHOLE			= 2,
	eOO_NAME_PARTY			= 4,
	eOO_NAME_MUNPA			= 8,
	eOO_CHAT_NOBALLOON		= 16,
	eOO_DAMAGE_MEMBERS		= 32,
	// 090116 ShinJS --- 캐릭터/펫 보이기 플래그 추가
	eOO_HIDE_PLAYER			= 64,
	eOO_HIDE_PET			= 128,
	eOO_HIDE_STREETSTALLTITLE = 256,
};

enum eOPTION_SHADOW
{
	eOO_SHADOW_NONE				= 0,
	eOO_SHADOW_ONLYHERO			= 1,
	eOO_SHADOW_MONSTER			= 2,
	eOO_SHADOW_OTHERCHARACTER	= 4,
};


enum eOPTION_EFFECT
{
	eOO_EFFECT_DEFAULT			= 0,
	eOO_EFFECT_ONLYHERO			= 1,
	eOO_EFFECT_NOT_USE			= 2,
};


class CObjectManager  
{
	CHero* m_pHero;
	CHeroPet* mpHeroPet;
	CHeroPet* mpHeroPetBackup;

	typedef WORD MonsterKind;
	typedef std::map< MonsterKind, HEFFPROC > RestHandleContainer;
	RestHandleContainer mRestHandleContainer;
	void RemoveObject(CObject** ppObject);
	CCharacterCalcManager * m_pCharCalcMgr;

	CYHHashTable<CObject> m_ObjectTable;
	CYHHashTable<CObject> m_HideObjectTable;

	CMemoryPoolTempl<CPlayer>		m_PlayerPool;
	CMemoryPoolTempl<CMonster>		m_MonsterPool;
	CMemoryPoolTempl<CNpc>			m_NpcPool;
	CMemoryPoolTempl<CBossMonster>	m_BossMonsterPool;
	CMemoryPoolTempl<CMapObject>	m_MapObjectPool;
	CMemoryPoolTempl<CPet>			m_PetPool;

	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.05.07
	CMemoryPoolTempl<CSHFarmRenderObj>	m_csFarmRenderObjPool;
	// E 농장시스템 추가 added by hseos 2007.05.07
	// 090422 ShinJS --- 탈것 추가
	CMemoryPoolTempl<CVehicle>		m_VehiclePool;

	CMemoryPoolTempl<CFurniture>	m_FurniturePool;

	DWORD	m_SelectedObjectID;
	DWORD	m_SelectEffectHandle;
	DWORD	m_dwOverInfoOption;
	BOOL	m_bAllNameShow;
	BOOL	m_bPress_ShowNameKey;
	DWORD	m_dwShadowOption;
	DWORD	m_dwLodMode;
	DWORD	m_dwEffectOption;
	
public:
	//MAKESINGLETON(CObjectManager)
		
	CObjectManager();
	virtual ~CObjectManager();
	void RegisterHero(SEND_HERO_TOTALINFO*);
	CPlayer* AddPlayerThroughCache(BASEOBJECT_INFO*,BASEMOVE_INFO*, CHARACTER_TOTALINFO*, SEND_CHARACTER_TOTALINFO*);
	CPlayer* AddPlayer(BASEOBJECT_INFO*, BASEMOVE_INFO*, CHARACTER_TOTALINFO*);
	CMonster* AddMonster(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,MONSTER_TOTALINFO* pTotalInfo);
	CBossMonster* AddBossMonster(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,MONSTER_TOTALINFO* pTotalInfo);
	CNpc* AddNpc(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,NPC_TOTALINFO* pTotalInfo);
	void AddSkillObject(cSkillObject* pSObj);
	CMapObject* AddMapObject(BASEOBJECT_INFO* pBaseObjectInfo, BASEMOVE_INFO* pMoveInfo, char* strData, float fScale, float fAngle );

	CPet* AddPet(DWORD MasterIdx, BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,PET_OBJECT_INFO* pPetObjectInfo, ITEMBASE* pItemInfo);
	CHeroPet* AddHeroPet(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,PET_OBJECT_INFO* pPetObjectInfo, ITEMBASE* pItemInfo);
	void RemoveHeroPet();
	CSHFarmRenderObj* AddFarmRenderObj(BASEOBJECT_INFO* pBaseObjectInfo, BASEMOVE_INFO* pMoveInfo, CSHFarmRenderObj::stFarmRenderObjInfo* pFarmRenderObjInfo);
	CVehicle* AddVehicle( BASEOBJECT_INFO* pBaseObjectInfo, BASEMOVE_INFO* pMoveInfo, MONSTER_TOTALINFO* pTotalInfo );		// 탈것 추가
	void RemoveVehicle( DWORD dwVehicleId );																				// 탈것 제거

	CFurniture* AddFuniture(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,stFurniture* pFurnitureInfo);

	CObject* GetObject(DWORD dwObjectID);
	
	void RemoveAllObject();
	void AddGarbageObject(CObject*);
	void GetTargetInRange(VECTOR3* pPivotPos,float Radius,ObjectArray<CObject*>* pObjectArray,BOOL bPK,WORD PARAM);
	DWORD GetSingleTargetInRange(VECTOR3* pPivotPos,float Radius, BOOL bPK,WORD PARAM);
	CObject*	GetSelectedObject();
	void		SetSelectedObject(CObject* pObject);
	DWORD		GetSelectedObjectID() { return m_SelectedObjectID; }
	void		SetSelectedObjectID(DWORD dwId);
	void		SetSelectedEffect(CObject* pObject);
	BOOL SeletedLastObject(CObject*, CMonsterGuageDlg*);
	void SelectedPlayer(CObject*, CMonsterGuageDlg*);
	void SelectedPet(CObject*, CMonsterGuageDlg*);
	void SelectedNpc(CObject*, CMonsterGuageDlg*);
	void SelectedMonster(CObject*, CMonsterGuageDlg*);
	void SelectedFarmObj(CObject*, CMonsterGuageDlg*);
	void SelectedFurniture(CObject*, CMonsterGuageDlg*);
	void SelectedVehicle(CObject*, CMonsterGuageDlg*);
	void OnMouseOver(CObject*);
	void OnMouseLeave(CObject*);
	void SetOverInfoOption( DWORD dwOption );
	void ApplyOverInfoOption( CObject* pObject );
	void ApplyOverInfoOptionToAll();
	BOOL CanOverInfoChange( CObject* pObject );
	DWORD GetObjectNameColor( CObject* pObject );
	void ShowChatBalloon( CObject* pObject, char* pStrChat, DWORD dwColor = RGB_HALF(70,70,70), DWORD dwAliveTime = 5000 );
	void HideChatBalloonAll();
	void SetAllNameShow( BOOL bShow );
    void SetPress_ShowNameKey(BOOL bPressKey);	
	void SetLodMode(DWORD dwOption);
	void ApplyPlayerLodModeToAll();

//---Shadow
	void SetShadowOption( DWORD dwOption );
	void ApplyShadowOption( CObject* pObject );
	void ApplyShadowOptionToAll();
	void AddShadow(CObject* pObject);
	void RemoveShadow(CObject* pObject);

	void SetEffectOption(DWORD dwOption) { m_dwEffectOption = dwOption; }
	DWORD GetEffectOption() const { return m_dwEffectOption; }
	DWORD GetSelEffectHandle() const { return m_SelectEffectHandle; }
	void AllPlayerPKOff();
	inline CHero* GetHero()	{	return m_pHero;	}
	inline CHeroPet* GetHeroPet() { return mpHeroPet; }
	inline CHeroPet* GetHeroPetBackup() { return mpHeroPetBackup; }
	void NetworkMsgParse(BYTE Protocol, LPVOID);
	void HideNpc(WORD UniqueIdx, BOOL);
	CObject* GetObjectByName(LPCTSTR);
	DWORD GetObjectByChatName(LPCTSTR);
	DWORD GetNearMonsterByName(LPCTSTR);
	CYHHashTable< CObject >& GetObjectTable() { return m_ObjectTable; }
	float GetDistance(CObject*, CObject*);
	void SetHide(DWORD id, WORD level);
	void SetDetect(DWORD id, WORD level);
	BOOL IsOverInfoOption( DWORD dwFlag ) const { return m_dwOverInfoOption & dwFlag; }
	BOOL IsShadowOption( DWORD dwFlag ) const { return m_dwShadowOption & dwFlag; }
	void RemoveNPC(DWORD dwNpcId);
	BOOL StartVehicleInstall( WORD wMonsterKind, CItem* pUseItem );
	CNpc* GetNpcByUniqueIdx(WORD UniqueIdx);
};
EXTERNGLOBALTON(CObjectManager);
