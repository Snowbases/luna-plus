// AppearanceManager.h: interface for the CAppearanceManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPEARANCEMANAGER_H__6121D532_2201_469B_A298_35F0C2E66876__INCLUDED_)
#define AFX_APPEARANCEMANAGER_H__6121D532_2201_469B_A298_35F0C2E66876__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define APPEARANCEMGR USINGTON(CAppearanceManager)

class CPlayer;
class CMonster;
class CNpc;
class CObject;
class CBossMonster;
class CMapObject;
class CPet;
class CEngineObject;
class CEngineEffect;
class CFurniture;
// 090422 ShinJS --- 탈것 추가잡업
class CVehicle;
// S 농장시스템 추가 added by hseos 2007.05.07
class CSHFarmRenderObj;
// E 농장시스템 추가 added by hseos 2007.05.07

// 090205 LYW --- AppearanceManager : 캐릭터 갱신 처리.
#include "./AppearPlayer.h"


struct RESERVATION_ITEM_PLAYER
{
	CPlayer*					pObject;
	SEND_CHARACTER_TOTALINFO	Msg;
};



#define MAX_RESERVATION_ITEM_PLAYER 1024


class CAppearanceManager  
{
	struct APPEAR_CHANGE
	{
		DWORD ChangeFlag;

		APPEAR_CHANGE()
		{
			ChangeFlag = 0;
		}
	};
	typedef std::map< DWORD, APPEAR_CHANGE > ChangeHashMap;
	ChangeHashMap m_ChangeHashMap;

	//cPtrList m_PtrList;

	void SetCharacterAppearance(CPlayer* pPlayer, DWORD changeFlag);
	void SetMonsterAppearance(CMonster* pMonster);
	void SetBossMonsterAppearance(CBossMonster* pMonster);
	void SetNpcAppearance(CNpc* pNpc);
	void SetPetAppearance(CPet* pPet);
	void SetFurnitureAppearance(CFurniture* pObject);
	// 090422 ShinJS --- 탈것
	void SetVehicleAppearance(CVehicle* pVehicle);
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.05.07
	void SetFarmRenderObjAppearance(CSHFarmRenderObj* pFarmRenderObj);
	// E 농장시스템 추가 added by hseos 2007.05.07


	void SetCharacterAppearanceToEffect(CPlayer* pPlayer,CEngineEffect* pRtEffect);

	// 090205 LYW --- AppearanceManager : 캐릭터 Appear처리를 관리하는 클래스 추가.
private : 
	CAppearPlayer m_AppearPlayer ;

private : 
	void RenewalAppear(const BOOL bForEffect, CPlayer* pPlayer, CEngineObject* pEngineObject, DWORD changeFlag ) ;

	cPtrList	m_lstReserveToAddPlayerList;
	//cPtrList	m_lstReserveToAddMonsterList;		

	cPtrList	m_lstAlphaProcessing;
	DWORD		m_dwLastProcessReservationTime;	
	BOOL		m_bUseReservation;	


	RESERVATION_ITEM_PLAYER*	m_pReserItemPlayer;
	cPtrList					m_lstFreeReserItemPlayer;

	DWORD						m_dwUsedReserItemPlayerNum;

	cPtrList	m_lstRemoveReserPlayer;

public:

	CAppearanceManager();
	~CAppearanceManager();

	void Init();
	void Release();
	
	void InitAppearance(CObject* pObject);
	void AddCharacterPartChange(DWORD PlayerID, DWORD changeFlag = eCHAPPEAR_INIT);
	void ProcessAppearance();
	
	void ShowWeapon(CPlayer* pPlayer);
	void HideWeapon(CPlayer* pPlayer);
	void ShowWeapon(CPlayer* pPlayer,CEngineObject* pEngineObject);
	void HideWeapon(CPlayer* pPlayer,CEngineObject* pEngineObject);
	
	void InitAppearanceToEffect(CObject* pObject,CEngineEffect* pEffect);
	void SetScalebyGuageBar(DWORD PlayerID);
	void SetMapObjectAppearance(CMapObject* pMapObject, char* strData, float fScale );
	VOID ProcessAlphaAppearance(VOID);

	BOOL ReserveToAppearObject(CObject* pObject, void* pMsg);	
	VOID ProcessReservation(void);
	
	VOID CancelReservation(CObject* pObject);
	VOID CancelAlphaProcess(CObject* pObject);

	VOID EnbaleReservation(void)		{ m_bUseReservation = TRUE; }
	VOID DisableReservation(void)		{ m_bUseReservation = FALSE; }

	RESERVATION_ITEM_PLAYER*	AllocReservationItem(VOID);
	BOOL						FreeReservationItem(RESERVATION_ITEM_PLAYER* pItem);
};


EXTERNGLOBALTON(CAppearanceManager)

void PostProcessAddPlayer(CPlayer* pPlayer, SEND_CHARACTER_TOTALINFO* pMsg);

#endif // !defined(AFX_APPEARANCEMANAGER_H__6121D532_2201_469B_A298_35F0C2E66876__INCLUDED_)

