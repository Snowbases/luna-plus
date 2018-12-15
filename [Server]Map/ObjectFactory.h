// ObjectFactory.h: interface for the CObjectFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTFACTORY_H__324EFDDF_059A_428C_94F2_76BC8E75E765__INCLUDED_)
#define AFX_OBJECTFACTORY_H__324EFDDF_059A_428C_94F2_76BC8E75E765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
//class CObject;
class CPlayer;
class Pet;
class CMonster;
class CNpc;
class CBossMonster;
class CMapObject;
// 080616 LUJ, 함정 추가
class CTrap;
// 090316 LUJ, 탈것 추가
class CVehicle;

// 필드보스 - 05.12 이영준
class CFieldBossMonster;
class CFieldSubMonster;
class CPet;

class CObjectFactory
{
private:
	// 090923 ONS 메모리풀 교체
	CPool<CPlayer>* PlayerPool;
	CPool<CMonster>* MonsterPool;	
	CPool<CNpc>* NpcPool;	
	CPool<CBossMonster>* BossMonsterPool;
	CPool<CPet>* PetPool;
	
	// 필드보스 - 05.12 이영준
	CPool<CFieldBossMonster>* FieldBossMonsterPool;
	CPool<CFieldSubMonster>* FieldSubMonsterPool;
	
	CPool<CMapObject>*	MapObjectPool;
	// 080616 LUJ, 함정 추가
	CPool<CTrap>* TrapPool;
	// 090316 LUJ, 탈것 추가
	CPool<CVehicle>* VehiclePool;

public:
	CObjectFactory();
	virtual ~CObjectFactory();
	
	void Init();
	void Release();

	CObject* MakeNewObject(EObjectKind Kind,DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo);
	void ReleaseObject(CObject* pObject);
};
extern CObjectFactory * g_pObjectFactory; 
#endif // !defined(AFX_OBJECTFACTORY_H__324EFDDF_059A_428C_94F2_76BC8E75E765__INCLUDED_)
