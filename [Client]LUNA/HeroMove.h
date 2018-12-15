// HeroMove.h: interface for the CHeroMove class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEROMOVE_H__AD3FC9BB_1B15_4CD4_B83B_6641CCE27C86__INCLUDED_)
#define AFX_HEROMOVE_H__AD3FC9BB_1B15_4CD4_B83B_6641CCE27C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MouseEventReceiver.h"

// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.05.07
class CSHFarmRenderObj;
// E 농장시스템 추가 added by hseos 2007.05.07

//090508 pdy 하우징 추가
class CFurniture;

class CHeroMove : public CMouseEventReceiver  
{
	void OnAttack(CObject* pTargetObj,DWORD MouseEvent);
// LBS 노점상 관련 03.09.30
	void OnEnterStreetStall( CObject* pTargetObj );
//
public:
	CHeroMove();
	virtual ~CHeroMove();

	void OnMouseOver( CObject* pObject );
	void OnMouseLeave( CObject* pObject );
	void OnClickPlayer(DWORD MouseEvent,CPlayer* pPlayer);
	void OnClickMonster(DWORD MouseEvent,CMonster* pMonster);
	void OnClickNpc(DWORD MouseEvent,CNpc* pNpc);
	void OnClickGround(DWORD MouseEvent,VECTOR3* pPos);
	void OnClickSkillObject(DWORD MouseEvent,cSkillObject* pObj);
	void OnClickMapObject(DWORD MouseEvent, CObject* pObj);
	void OnClickFarmRenderObj(DWORD MouseEvent, CSHFarmRenderObj* pObj);
	void OnClickVehicle(DWORD MouseEvent,CVehicle* pVehicle);
	void OnClickFurniture(DWORD MouseEvent, CFurniture* pFurniture);

	void OnDbClickPlayer(DWORD MouseEvent,CPlayer* pPlayer);
	void OnDbClickMonster(DWORD MouseEvent,CMonster* pMonster);
	void OnDbClickNpc(DWORD MouseEvent, CNpc* pNpc);
	void OnDbClickPet(DWORD MouseEvent, CPet* pPet);
	void OnDbClickVehicle(DWORD MouseEvent, CVehicle* pVehicle);
	
	void AttackSelectedObject();
	
	void SelectLoginPoint( WORD LoginPointIndex );
	void MapChange( WORD MapIndex, DWORD ItemIdx=0, DWORD ItemPos=0 );
	void MapChangeToNpc(DWORD dwMapNum, float xPos, float zPos) ;
	void ChangeMap( WORD MapIndex ) ;
};

#endif // !defined(AFX_HEROMOVE_H__AD3FC9BB_1B15_4CD4_B83B_6641CCE27C86__INCLUDED_)
