// MouseEventReceiver.h: interface for the CMouseEventReceiver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUSEEVENTRECEIVER_H__925076FC_FD16_47F1_8259_136B9C5D0C3E__INCLUDED_)
#define AFX_MOUSEEVENTRECEIVER_H__925076FC_FD16_47F1_8259_136B9C5D0C3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPlayer;
class CMonster;
class CNpc;
class cSkillObject;
class CPet;
// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.05.07
class CSHFarmRenderObj;
// E 농장시스템 추가 added by hseos 2007.05.07
// 090422 ShinJS --- 탈것 추가작업
class CVehicle;
//090508 pdy 하우징 추가
class CFurniture;


enum MOUSEEVENT // LBS 03.09.30 ¼öÁ¤
{
	MOUSEEVENT_NONE				=0,
	MOUSEEVENT_LCLICK			=1,
	MOUSEEVENT_RCLICK			=2,
	MOUSEEVENT_DBLCLICK			=4,
	MOUSEEVENT_LPRESS			=8,
	// 090613 ONS 우클릭 캐릭터 팝업메뉴 생성처리를 위한 마우스 이벤트속성.
	MOUSEEVENT_LBUTTONUP		=16,
	MOUSEEVENT_RBUTTONUP		=32,

};


class CMouseEventReceiver  
{
public:
	CMouseEventReceiver();
	virtual ~CMouseEventReceiver();

	virtual void OnMouseOver( CObject* pObject )							{}
	virtual void OnMouseLeave( CObject* pObject )							{}
	virtual void OnClickPlayer(DWORD MouseEvent,CPlayer* pPlayer)			{}
	virtual void OnClickMonster(DWORD MouseEvent,CMonster* pMonster)		{}
	virtual void OnClickNpc(DWORD MouseEvent,CNpc* pNpc)					{}
	virtual void OnClickGround(DWORD MouseEvent,VECTOR3* pPos)				{}
	virtual void OnClickSkillObject(DWORD MouseEvent,cSkillObject* pObj)	{}
	virtual void OnClickMapObject(  DWORD MouseEvent, CObject* pObj )		{}
	virtual void OnClickFarmRenderObj(DWORD MouseEvent, CSHFarmRenderObj* pObj) {}
	virtual void OnClickVehicle(DWORD MouseEvent,CVehicle* pVehicle)		{}
	virtual void OnClickFurniture(DWORD MouseEvent, CFurniture* pFurniture)	{}

	virtual void OnDbClickPlayer(DWORD MouseEvent,CPlayer* pPlayer)			{}
	virtual void OnDbClickMonster(DWORD MouseEvent,CMonster* pMonster)		{}
	virtual void OnDbClickNpc(DWORD MouseEvent,CNpc* pNpc)					{}
	virtual void OnDbClickPet(DWORD MouseEvent,CPet* pPet)					{}
	virtual void OnDbClickVehicle(DWORD MouseEvent, CVehicle* pVehicle)		{}

};

#endif // !defined(AFX_MOUSEEVENTRECEIVER_H__925076FC_FD16_47F1_8259_136B9C5D0C3E__INCLUDED_)
