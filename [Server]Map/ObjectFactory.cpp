// ObjectFactory.cpp: implementation of the CObjectFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "ObjectFactory.h"
#include "Player.h"
#include "BossMonster.h"
#include "Npc.h"
#include "MapObject.h"
#include "FieldBossMonster.h"
#include "FieldSubMonster.h"
// 080616 LUJ, 함정 기능위해 참조
#include "Trap.h"
#include "Pet.h"
// 090316 LUJ, 탈것 위해 추가
#include "Vehicle.h"
#include "Trigger\Manager.h"
#include "..\[CC]Skill\Server\Manager\SkillManager.h"

CObjectFactory * g_pObjectFactory=NULL;

CObjectFactory::CObjectFactory()
{
	PlayerPool = NULL;
	MonsterPool = NULL;
	NpcPool = NULL;
	BossMonsterPool = NULL;
	MapObjectPool = NULL;
	PetPool = NULL;

	// 필드보스 - 05.12 이영준
	FieldBossMonsterPool = NULL;
	FieldSubMonsterPool = NULL;
	// 080616 LUJ, 함정 추가
	TrapPool = NULL;
	// 090316 LUJ, 탈것 추가
	VehiclePool = NULL;
}

CObjectFactory::~CObjectFactory()
{
	Release();
}

void CObjectFactory::Init()
{	
	PlayerPool		= new	CPool<CPlayer>;
	MonsterPool		= new	CPool<CMonster>;
	NpcPool			= new	CPool<CNpc>;
	BossMonsterPool = new 	CPool<CBossMonster>;
	MapObjectPool	= new CPool<CMapObject>;
	// 080616 LUJ, 함정 추가
	TrapPool		= new CPool<CTrap>;
	PetPool			= new CPool<CPet>;
    // 090316 LUJ, 탈것 추가
	VehiclePool		= new CPool<CVehicle>;
	// 필드보스 - 05.12 이영준
	FieldBossMonsterPool = new CPool<CFieldBossMonster>;
	FieldSubMonsterPool = new CPool<CFieldSubMonster>;

	PlayerPool->Init(MAX_TOTAL_PLAYER_NUM,10,"PlayerPool");
	MonsterPool->Init(MAX_TOTAL_MONSTER_NUM,100,"MonsterPool");
	BossMonsterPool->Init(MAX_TOTAL_BOSSMONSTER_NUM, 1, "BossMonsterPool");

	// 필드보스 - 05.12 이영준
	FieldBossMonsterPool->Init(MAX_TOTAL_BOSSMONSTER_NUM, 1, "FieldBossMonsterPool");
	FieldSubMonsterPool->Init(MAX_TOTAL_BOSSMONSTER_NUM * 10, 5, "FieldSubMonsterPool");
	
	NpcPool->Init(MAX_TOTAL_NPC_NUM,10,"NpcPool");
	MapObjectPool->Init( MAX_MAPOBJECT_NUM, 10, "MapObjectPool" );
	// 080616 LUJ, 함정 추가
	TrapPool->Init( MAX_MAPOBJECT_NUM, 10, "TrapPool" );
	PetPool->Init(MAX_TOTAL_PET_NUM,10,"PetPool");
	// 090316 LUJ, 탈것 추가
	VehiclePool->Init( MAX_MAPOBJECT_NUM, 10, "VehiclePool" );
}

void CObjectFactory::Release()
{
	SAFE_DELETE(PlayerPool);
	SAFE_DELETE(MonsterPool);
	SAFE_DELETE(NpcPool);
	SAFE_DELETE(BossMonsterPool);
	
	// 필드보스 - 05.12 이영준
	SAFE_DELETE(FieldBossMonsterPool);
	SAFE_DELETE(FieldSubMonsterPool);

	SAFE_DELETE(MapObjectPool);
	// 080616 LUJ, 함정 추가
	SAFE_DELETE(TrapPool);
	SAFE_DELETE(PetPool);//PetPool = new CMemoryPoolTempl<CPet>;
	// 090707 ShinJS --- 메모리 해제 누락 추가
	SAFE_DELETE(VehiclePool);

}

CObject* CObjectFactory::MakeNewObject(EObjectKind Kind,DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo)
{
	CObject* pObject = NULL;
	switch(Kind)
	{
	case eObjectKind_Player:
		pObject = PlayerPool->Alloc();
		break;

    case eObjectKind_ToghterPlayMonster:
	case eObjectKind_SpecialMonster:
	case eObjectKind_Monster:
		pObject = MonsterPool->Alloc();
		break;

	case eObjectKind_Npc:
		pObject = NpcPool->Alloc();
		break;

	case eObjectKind_BossMonster:
		pObject = BossMonsterPool->Alloc();
		break;	
	
	// 필드보스 - 05.12 이영준
	case eObjectKind_FieldBossMonster:
		pObject = FieldBossMonsterPool->Alloc();
		break;	
	case eObjectKind_FieldSubMonster:
		pObject = FieldSubMonsterPool->Alloc();
		break;

	case eObjectKind_MapObject:
		pObject = MapObjectPool->Alloc();
		break;

		// 080616 LUJ, 함정 추가
	case eObjectKind_Trap:
		pObject = TrapPool->Alloc();
		break;
	case eObjectKind_Pet:
		pObject = PetPool->Alloc();
		break;
		// 090316 LUJ, 탈것 추가
	case eObjectKind_Vehicle:
		pObject = VehiclePool->Alloc();
		break;
	}
	
	ASSERT(pObject);

	// 081012 LYW --- ObjectFactory : 오브젝트 널포인트 체크.
	if( !pObject ) return NULL ;
	
	if(pObject->Init(Kind,AgentNum,pBaseObjectInfo) == FALSE)
	{
		ReleaseObject(pObject);
		return FALSE;
	}
	
	return pObject;
}

void CObjectFactory::ReleaseObject(CObject* pObject)
{
	// 오브젝트가 소유한 트리거를 종료시킨다
	TRIGGERMGR->StopObjectTrigger(
		EObjectKind(pObject->GetObjectKind()),
		pObject->GetID());

	const EObjectKind Kind = pObject->GetObjectKind();
	pObject->Release();
	switch(Kind)
	{
	case eObjectKind_Player:
		PlayerPool->Free((CPlayer*)pObject);
		break;

	case eObjectKind_ToghterPlayMonster:
	case eObjectKind_SpecialMonster:
	case eObjectKind_Monster:
		MonsterPool->Free((CMonster*)pObject);
		break;

	case eObjectKind_Npc:
		NpcPool->Free((CNpc*)pObject);
		break;

	case eObjectKind_BossMonster:
		BossMonsterPool->Free((CBossMonster*)pObject);
		break;
	
	// 필드보스 - 05.12 이영준
	case eObjectKind_FieldBossMonster:
		FieldBossMonsterPool->Free((CFieldBossMonster*)pObject);
		break;
	case eObjectKind_FieldSubMonster:
		FieldSubMonsterPool->Free((CFieldSubMonster*)pObject);
		break;
		
	case eObjectKind_MapObject:
		MapObjectPool->Free((CMapObject*)pObject);
		break;

		// 080616 LUJ, 함정 추가
	case eObjectKind_Trap:
		TrapPool->Free( ( CTrap* )pObject );
		break;

	case eObjectKind_Pet:
		{
			PetPool->Free((CPet*)pObject);
		}
		break;

		// 090316 LUJ, 탈것 추가
	case eObjectKind_Vehicle:
		{
			VehiclePool->Free( ( CVehicle* )pObject );
		}
		break;
	}
}
