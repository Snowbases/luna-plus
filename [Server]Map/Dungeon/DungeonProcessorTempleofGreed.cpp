#include "StdAfx.h"
#include ".\dungeonprocessortempleofgreed.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "..\Monster.h"
#include "..\RegenManager.h"
#include "..\AISystem.h"
#include "..\Trigger\Manager.h"

CDungeonProcessorTempleofGreed::CDungeonProcessorTempleofGreed(void)
{
}

CDungeonProcessorTempleofGreed::~CDungeonProcessorTempleofGreed(void)
{
}

void CDungeonProcessorTempleofGreed::SetBossMonster(stDungeon* pDungeon)
{
	if(! pDungeon)
		return;

	WORD wBossMonsterKind = 0;

	switch(pDungeon->difficulty)
	{
	case eDIFFICULTY_EASY:
		{
			if(0<=pDungeon->dwPoint && pDungeon->dwPoint<1200)
			{
				wBossMonsterKind = 663;
			}
			else if(1200<=pDungeon->dwPoint && pDungeon->dwPoint<1700)
			{
				wBossMonsterKind = 664;
			}
			else if(1700<=pDungeon->dwPoint && pDungeon->dwPoint<2200)
			{
				wBossMonsterKind = 665;
			}
			else
			{
				wBossMonsterKind = 666;
			}
		}
		break;
	case eDIFFICULTY_NORMAL:
		{
			if(0<=pDungeon->dwPoint && pDungeon->dwPoint<1200)
			{
				wBossMonsterKind = 667;
			}
			else if(1200<=pDungeon->dwPoint && pDungeon->dwPoint<1700)
			{
				wBossMonsterKind = 668;
			}
			else if(1700<=pDungeon->dwPoint && pDungeon->dwPoint<2200)
			{
				wBossMonsterKind = 669;
			}
			else
			{
				wBossMonsterKind = 670;
			}
		}
		break;
	case eDIFFICULTY_HARD:
		{
			if(0<=pDungeon->dwPoint && pDungeon->dwPoint<1200)
			{
				wBossMonsterKind = 671;
			}
			else if(1200<=pDungeon->dwPoint && pDungeon->dwPoint<1700)
			{
				wBossMonsterKind = 672;
			}
			else if(1700<=pDungeon->dwPoint && pDungeon->dwPoint<2200)
			{
				wBossMonsterKind = 673;
			}
			else
			{
				wBossMonsterKind = 674;
			}
		}
		break;
	}

	if(! wBossMonsterKind)
		return;

	const BASE_MONSTER_LIST* pMonsterList = GAMERESRCMNGR->GetMonsterListInfo(wBossMonsterKind);
	if(! pMonsterList)
		return;

	VECTOR3 vPos = {39300, 0, 41000};
	// 100104 LUJ, 서브 인덱스는 리젠 스크립트(Monster_%02d.bin)를 읽을 때
	//			생성되는 고유 번호이다. 따라서 리젠 스크립트에서 생성되지
	//			않는 몬스터는 0이어도 관계없다
	CMonster* pAddMonster = REGENMGR->RegenObject(
		g_pAISystem.GeneraterMonsterID(),
		0,
		pDungeon->dwChannelID,
		pMonsterList->ObjectKind,
		wBossMonsterKind,
		&vPos,
		(DWORD)DUNGEON_MONSTER_GROUP_ID,
		0,
		0,
		FALSE);

	
	if(pAddMonster)
	{
		DWORD alias = 0;
		DWORD count = 1;
		DWORD groupAlias = 0;
		// 별칭은 맵전용으로 등록한다.
		char buffer[MAX_PATH] = {0};
		sprintf(
			buffer,
			"[%s%d].%d.%d.%d",
			"보스", count,
			Trigger::eOwner_Server,
			pDungeon->dwChannelID,
			pDungeon->dwChannelID);
		alias = TRIGGERMGR->GetHashCode(buffer);
		pAddMonster->SetAlias(alias);

		// 그룹별칭은 맵전용으로 등록한다.
		sprintf(
			buffer,
			"[%s].%d.%d.%d",
			"보스그룹",
			Trigger::eOwner_Server,
			pDungeon->dwChannelID,
			pDungeon->dwChannelID);
		groupAlias = TRIGGERMGR->GetHashCode(buffer);
		pAddMonster->SetGroupAlias(groupAlias);

		// 보스몬스터가 한마리라는 가정(수정예정)
		pDungeon->m_CurBossMonsterState.wIndex = wBossMonsterKind;
		pDungeon->m_CurBossMonsterState.dwObjectIndex = pAddMonster->GetID();
		pDungeon->m_CurBossMonsterState.wReturnPosX = (WORD)39300.0f;
		pDungeon->m_CurBossMonsterState.wReturnPosZ = (WORD)41000.0f;
	}

}
