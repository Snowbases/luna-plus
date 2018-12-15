// MapObject.cpp: implementation of the CMapObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapObject.h"
#include "OptionManager.h"
#include "ObjectManager.h"
#include "PartyManager.h"
#include "GameIn.h"
#include "MonsterGuageDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapObject::CMapObject()
{

}

CMapObject::~CMapObject()
{

}

void CMapObject::InitMapObject( MAPOBJECT_INFO* pInitInfo )
{
	memcpy( &m_MapObjectInfo, pInitInfo, sizeof(MAPOBJECT_INFO) );
}


void CMapObject::Die(CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive)
{
	SetLife(0);
	
	CObject::Die(pAttacker,bFatalDamage,bCritical,bDecisive);
}

// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
void CMapObject::Damage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO& result)
{	
	if( pAttacker )
	{
		if( OPTIONMGR->IsShowMemberDamage() )
		{
			if( pAttacker == HERO || pAttacker == HEROPET || PARTYMGR->IsPartyMember( pAttacker->GetID() ) )
				EFFECTMGR->AddDamageNumber(Damage,pAttacker,this,DamageKind,result);
		}
		else
		{
			if( pAttacker == HERO || pAttacker == HEROPET )
				EFFECTMGR->AddDamageNumber(Damage,pAttacker,this,DamageKind,result);
		}

	}
	
	// Life 셋팅
	DWORD life = GetLife();
	if(life < Damage)
		life = 0;
	else
		life = life - Damage;
	SetLife(life);
}


void CMapObject::SetLife(DWORD val, BYTE type)
{
	m_MapObjectInfo.Life = val;

	if( GAMEIN->GetMonsterGuageDlg() )
	{
		if( GAMEIN->GetMonsterGuageDlg()->GetObjectType() & eObjectKind_MapObject )
			GAMEIN->GetMonsterGuageDlg()->SetMonsterLife( val, m_MapObjectInfo.MaxLife );
	}
}