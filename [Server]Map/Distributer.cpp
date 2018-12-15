#include "stdafx.h"
#include "Distributer.h"
#include "UserTable.h"
#include "PartyManager.h"
#include "Party.h"
#include "Monster.h"
#include "BossRewardsManager.h"
#include "FieldBossMonsterManager.h"
#include "ItemManager.h"
#include "Player.h"
#include "ItemDrop.h"
#include "../hseos/Date/SHDateManager.h"
#include "../hseos/Family/SHFamilyManager.h"
#include "petmanager.h"
#include "pet.h"
#include "MapDBMsgParser.h"
#include "GameResourceManager.h"

float gEventRate[eEvent_Max];
float gEventRateFile[eEvent_Max];

CDistributer::CDistributer()																	// 생성자 함수.
{
	m_DamageObjectTableSolo.Initialize(2);
	m_DamageObjectTableParty.Initialize(2);

	m_1stPartyID = 0;
	m_1stPlayerID = 0;
	m_TotalDamage = 0;
	m_FirstDamage = 0;
	m_nTotalKillerCount = 0;
	m_HighLevel = 0;
	m_pKiller = NULL;
	m_pMob = NULL;
	m_dwDropItemID = 0;
	m_dwDropItemRatio = 0;
}

CDistributer::~CDistributer()																	// 소멸자 함수.
{
	Release();																					// 해제 함수를 호출한다.
}

void CDistributer::Release()																	// 해제 함수.
{
	m_1stPartyID = 0;																			// 첫 데미지를 준 파티 아이디 초기화.
	m_1stPlayerID = 0;																			// 첫 데미지를 준 플레이어 아이디 초기화.
	m_TotalDamage = 0;																			// 데미지 합계 초기화.
	m_FirstDamage = 0 ;																			// 첫 데미지 초기화.
	m_nTotalKillerCount = 0 ;																	// 킬러 수 초기화.
	m_HighLevel = 0 ;																			// 킬러 들 중 가장 레벨이 높은 킬러의 레벨.

	DAMAGEOBJ *sobj, *pobj = NULL;																// 개인 및 파티 데미지 테이블의 포인터.

	m_DamageObjectTableSolo.SetPositionHead();													// 개인 데미지 테이블을 헤더로 세팅한다.

	while((sobj = m_DamageObjectTableSolo.GetData())!= NULL)												// 데미지 오브젝트가 없을 때 까지 while.
	{
		delete sobj;																			// 데미지 오브젝트를 삭제한다.
		sobj = NULL ;																			// 데미지 오브젝트 NULL처리.
	}

	m_DamageObjectTableSolo.RemoveAll();														// 데미지 오브젝트를 비운다.				

	m_DamageObjectTableParty.SetPositionHead();													// 파티 데미지 테이블을 헤더로 세팅한다.

	while((pobj = m_DamageObjectTableParty.GetData())!= NULL)											// 데미지 오브젝트가 없을 때 까지 while.		
	{																							
		delete pobj;																			// 데미지 오브젝트를 삭제한다.
		pobj = NULL ;																			// 데미지 오브젝트 NULL처리.
	}																							
																								
	m_DamageObjectTableParty.RemoveAll();														// 데미지 오브젝트를 비운다.					

	m_pKiller = NULL;
	m_pMob = NULL;
	m_dwDropItemID		= 0 ;																	// 드랍 아이템 아이디를 담는 변수 초기화.
	m_dwDropItemRatio	= 0 ;																	// 드랍률을 담는 변수 초기화.							

    mPlayerIndexContainer.clear();
}

void CDistributer::AddDamageObject(CPlayer* pPlayer, DWORD damage, DWORD plusdamage)
{
	if( pPlayer ) 																				// 플레이어 정보가 유효한지 체크한다.
	{
		DoAddDamageObj(
			m_DamageObjectTableSolo,
			pPlayer->GetID(),
			damage + plusdamage);

		//if(plusdamage != 0)																		// 추가 데미지가 있는지 체크한다.
		if( m_1stPlayerID == 0 )
		{
			m_1stPlayerID = pPlayer->GetID();													// 첫 데미지를 준 플레이어 아이디를 받는다.
			m_1stPartyID = pPlayer->GetPartyIdx();												// 첫 데미지를 준 플레이어의 파티 인덱스를 받는다.
		}
		
		m_TotalDamage += damage + plusdamage;
	}
}

void CDistributer::DoAddDamageObj(CYHHashTable<DAMAGEOBJ>& pTable, DWORD dwID, DWORD damage)
{
	if(DAMAGEOBJ* pDObj = pTable.GetData(dwID))
	{
		pDObj->dwData += damage;
	}
	else
	{
		pDObj = new DAMAGEOBJ;
		ZeroMemory(
			pDObj,
			sizeof(*pDObj));
		pDObj->dwID = dwID;
		pDObj->dwData = damage;
		
		pTable.Add(
			pDObj,
			dwID);
	}
}


BOOL CDistributer::Chk(CPlayer& pPlayer, DWORD GridID)
{
	if(pPlayer.GetInited())
	{
		if(FALSE == pPlayer.IsShowdown())
		{
			VECTOR3 PlayerPosition = {0};
			pPlayer.GetPosition(
				&PlayerPosition);

			const float fDistance = CalcDistanceXZ(
				&m_pKilledPosition,
				&PlayerPosition);

			if(fDistance <= POINT_VALID_DISTANCE)
			{
				if(pPlayer.GetState() != eObjectState_Die)
				{
					if(pPlayer.GetGridID() == GridID)
					{
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE ;																				// FALSE 리턴.
}

void CDistributer::ChooseOne(const DAMAGEOBJ& pobj, DWORD& pBigDamage, DWORD& pBigID)
{
	if(pBigDamage < pobj.dwData)
	{
		pBigDamage = pobj.dwData;
		pBigID = pobj.dwID;
	}
	else if(pBigDamage == pobj.dwData)
	{
		if(1 == (rand() % 2))
		{
			pBigID = pobj.dwID;
		}
	}
}

void CDistributer::DistributeItemPerDamage(CMonster& pMob)
{
	CPlayer* TargetPlayerTable[100] = {0};
	WORD TargetPos = 0;
	
	if(FIELDBOSSDROPITEMLIST* const pDropItemList = FIELDBOSSMONMGR->GetFieldBossDropItemList(pMob.GetMonsterKind()))													
	{
		for(WORD index = 0; index < MAX_FIELDBOSS_DROPITEM_GROUP_NUM; index++)
		{
			// 드랍 아이템 정보가 있는가? 없다면 종료
			if( pDropItemList->m_ItemList[ index ].m_wItemIndex[0] == 0 )
				break;
	        
			// 아이템 그룹 내의 어떤 아이템을 줄지 결정
			for( WORD dropitemnum = 0; dropitemnum < MAX_FIELDBOSS_DROPITEM_NUM; dropitemnum++)
			{
				if(pDropItemList->m_ItemList[ index ].m_wItemIndex[dropitemnum] == 0)
					break;
			}

			WORD select = WORD( rand() % dropitemnum );

			// 드랍될 횟수를 가져온다
			WORD count = pDropItemList->m_ItemList[ index ].m_wCount;

			for(WORD i = 0; i < count; i++)
			{
				// 드랍 할것인가?
				if( (rand() % 100) < pDropItemList->m_ItemList[ index ].m_wDropRate )
				{
					// 변수초기화
					memset( TargetPlayerTable, 0, sizeof(CPlayer*) * 100 );
					TargetPos = 0;

					// 공격한 사람들을 검색해서 기준치 이상 데미지를 준 사람들 가려낸다
					m_DamageObjectTableSolo.SetPositionHead();

					while(const DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
					{
						if(obj->dwData == 0)
							continue;

						CPlayer* const pReceivePlayer = (CPlayer *)g_pUserTable->FindUser(obj->dwID);

						if(!pReceivePlayer)	
							continue;
						if(pReceivePlayer->GetInited())
							continue;
						if(pReceivePlayer->GetGridID() != pMob.GetGridID())
							continue;
						if(pReceivePlayer->GetLevel() > pMob.GetSMonsterList().Level + 8 )
							continue;
						
						// 데미지 비율 계산
						WORD DamageRate = (WORD)(((double)obj->dwData / m_TotalDamage) * 100);

						// 기준치 이상 데미지를 주었다면
						if( DamageRate >= pDropItemList->m_ItemList[ index ].m_wDamageRate )
						{
							// 데미지 비율만큼 대상 테이블에 등록한다
							for(DWORD rate = 0; rate < DamageRate; rate++)
							{
								TargetPlayerTable[ TargetPos++ ] = pReceivePlayer;
							}
						}
					}

					// 대상 테이블에 등록된 유저가 있다면
					if( TargetPos > 0 )
					{
						// 랜덤으로 테이블 위치를 결정한다
						WORD TargetIndex = WORD( rand() % TargetPos );

						if(CPlayer* const TargetPlayer = TargetPlayerTable[TargetIndex])
						{
							ITEMMGR->MonsterObtainItem(
								TargetPlayer,
								pDropItemList->m_ItemList[index].m_wItemIndex[select],
								pMob.GetMonsterKind(),
								pDropItemList->m_ItemList[index].m_wItemCount[select]);
						}
					}
				}
			}
		}
	}

	
}

void CDistributer::DistributePerDamage(CMonster& pMob)
{	
	MONEYTYPE MaxMoney = 0;
	DWORDEX MaxExp = 0;

	BOSSREWARDSMGR->GetBossRewardsInfo(
		pMob.GetMonsterKind(),
		MaxExp,
		MaxMoney);

	m_DamageObjectTableSolo.SetPositionHead();

	while(DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
	{
		if(obj->dwData == 0)
			continue;
		CPlayer* pReceivePlayer = (CPlayer *)g_pUserTable->FindUser(obj->dwID);

		if(!pReceivePlayer)	
			continue;
		if(pReceivePlayer->GetInited())
			continue;
		if(pReceivePlayer->GetGridID() != pMob.GetGridID())
			continue;

		EXPTYPE Exp = (DWORD)(((double)obj->dwData * MaxExp) / m_TotalDamage);
		MONEYTYPE Money = (DWORD)(((double)obj->dwData * MaxMoney) / m_TotalDamage);

		float fBuffExp = pReceivePlayer->GetRateBuffStatus()->GetExp;
		Exp = (DWORDEX)(Exp*gEventRate[eEvent_ExpRate]) ;
		Exp += EXPTYPE(Exp*(fBuffExp/100)) ;

		if(pMob.GetObjectKind() == eObjectKind_FieldBossMonster)
		{
			if(pReceivePlayer->GetLevel() > pMob.GetSMonsterList().Level + 6 )
				continue;

			if(Exp == 0) Exp = 1;
			if(Money == 0) Money = 1;

			pReceivePlayer->AddPlayerExpPoint(Exp);
			AddMoney(
				pReceivePlayer,
				Money,
				pMob.GetMonsterKind());
		}
		else
		{
			if(Exp >= 10000)
				pReceivePlayer->AddPlayerExpPoint(Exp);

			if(Money >= 100000)
				AddMoney(
					pReceivePlayer,
					Money,
					pMob.GetMonsterKind());
		}
	}
}

void CDistributer::DamageInit()
{
	m_1stPartyID = 0;																			// 첫 데미지를 준 파티 아이디 초기화.
	m_1stPlayerID = 0;																			// 첫 데미지를 준 플레이어 아이디 초기화.
	m_TotalDamage = 0;																			// 데미지 합계 초기화.
	m_FirstDamage = 0 ;																			// 첫 데미지 초기화.
	m_nTotalKillerCount = 0 ;																	// 킬러 수 초기화.
	m_HighLevel = 0 ;																			// 킬러 들 중 가장 레벨이 높은 킬러의 레벨.

	m_DamageObjectTableSolo.RemoveAll();														// 데미지 오브젝트 테이블을 모두 비운다.	

	m_pKiller = NULL;
	m_pMob = NULL;
	m_dwDropItemID		= 0 ;																	// 드랍 아이템 아이디를 담는 변수 초기화.
	m_dwDropItemRatio	= 0 ;																	// 드랍률을 담는 변수 초기화.							
}

void CDistributer::DeleteDamagedPlayer(DWORD CharacterID)
{	
	DAMAGEOBJ* pData = NULL ;																	// 데미지 오브젝트 정보를 담을 포인터를 선언 및 NULL 초기화.

	pData = m_DamageObjectTableSolo.GetData( CharacterID );										// 받은 아이디로 데미지 테이블에서 데미지 오브젝트 포인터를 받는다.

	if( pData )																					// 데미지 오브젝트 포인터가 유효한지 체크한다.
	{
		if( m_TotalDamage >= pData->dwData )													// 포인터의 데미지가 데미지 합계보다 크거나 같으면,
		{
			m_TotalDamage -= pData->dwData;														// 데미지 합계에서 포인터의 데미지 만큼 뺀다.
		}
		else																					// 포인터의 데미지가 데미지 합계보자 작으면
		{
			m_TotalDamage = 0;																	// 합계 데미지를 0으로 세팅한다.
		}
	} 

	m_DamageObjectTableSolo.Remove(CharacterID);												// 데미지 오브젝트 테이블에서 주어진 아이디의 데미지 오브젝트를 비운다.
}

void CDistributer::SetInfoToDistribute(DWORD dwKillerID, DWORD DropItemID, DWORD DropItemRatio,CMonster& pMob)
{
	CObject* pKiller = g_pUserTable->FindUser(dwKillerID);

	pMob.GetPosition(
		&m_pKilledPosition);

	if( pKiller->GetObjectKind() == eObjectKind_Player )
	{
		m_pKiller = (CPlayer *)pKiller;
	}
	else if( pKiller->GetObjectKind() == eObjectKind_Pet )
	{
		CObject* const object = g_pUserTable->FindUser(
			pKiller->GetOwnerIndex());

		if(object &&
			eObjectKind_Player == object->GetObjectKind())
		{
			m_pKiller = (CPlayer*)object;
		}
	}
	else if(CObject* const owner = g_pUserTable->FindUser(pKiller->GetOwnerIndex()))
	{
		if(eObjectKind_Player == owner->GetObjectKind())
		{
			m_pKiller = (CPlayer*)owner;
		}
	}

	m_pMob = &pMob;
	m_dwDropItemID = DropItemID;
	m_dwDropItemRatio = DropItemRatio;
}		

// 091111 pdy 파티 데미지 산정 방식을 몬스터 보상시에 한번 계산 하도록 변경
void CDistributer::Distribute()
{
	GetAllPartyDamage();
	GetHighLevelOfKillers();
	GetTotalKillerCount();

	if( m_nTotalKillerCount > 1 )																// 몬스터를 처치 한 플레이어가 다수라면, 
	{
		DistributeToKillers() ;																	// 다수 처리를 한다.
	}
	else																						// 몬스터를 처치 한 플레이어가 한명이라면, 
	{
		DistributeToKiller() ;																	// 개인 처리를 한다.
	}

	Release();																					// 데미지 테이블 등 모든 참조 정보를 초기화 한다.
}

void CDistributer::DistributeToKillers()
{
	if(!GetFirstDamange())																		// 선공격자 데미지를 구한다.
	{
		if( m_pMob->GetObjectKind() == eObjectKind_BossMonster )
		{
			SYSTEMTIME time;
			char szFile[256] = {0,};
			GetLocalTime( &time );

			sprintf(szFile, "./Log/BossMonsterAssertLog_%02d_%04d%02d%02d.txt", g_pServerSystem->GetMapNum(), time.wYear, time.wMonth, time.wDay );
			FILE* fp;
			fp = fopen(szFile, "a+");
			if (fp)
			{
				fprintf(fp, "BossMonster Not FirstDamage Return\n");
				fclose(fp);
			}
		}
	}

	int nPenaltyType = GetLevelPenaltyToMonster(m_HighLevel);

	MONEYTYPE money = ITEMDROP_OBJ->DropMoney(
		m_pMob->GetSMonsterList(),
		nPenaltyType);

	if( m_pMob->GetObjectKind() == eObjectKind_BossMonster )
	{
		SYSTEMTIME time;
		char szFile[256] = {0,};
		GetLocalTime( &time );

		sprintf(szFile, "./Log/BossMonsterLog_%02d_%04d%02d%02d_%d.txt", g_pServerSystem->GetMapNum(), time.wYear, time.wMonth, time.wDay, m_pMob->GetMonsterKind() );
		FILE* fp;
		fp = fopen(szFile, "a+");
		if (fp)
		{
			fprintf(fp, "Time : [%2d:%2d], HighLevel : %d, Drop Money : %u, nPenaltyType : %d\n", time.wHour, time.wMinute, m_HighLevel, money, nPenaltyType);

			m_DamageObjectTableSolo.SetPositionHead() ;

			while(DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
			{
				if(CObject* const object = g_pUserTable->FindUser(obj->dwID))
				{
					if(object->GetGridID() == m_pMob->GetGridID())
					{
						fprintf(
							fp,
							"PlayerID : %u,		Damage : %u,	Level : %u,	CharacterName : %s\n",
							obj->dwID,
							obj->dwData,
							object->GetLevel(),
							object->GetObjectName());
					}
				}
			}

			m_DamageObjectTableParty.SetPositionHead();

			while(DAMAGEOBJ* const obj = m_DamageObjectTableParty.GetData())
			{
				CParty* pParty = PARTYMGR->GetParty(obj->dwID) ;
				if( !pParty ) continue;

				fprintf(fp, "\n[PartyIdx : %u][PartyDamage : %u]\n", obj->dwID, obj->dwData);

				for(int i=0; i<MAX_PARTY_LISTNUM; i++)
				{
					if(CObject* const object = g_pUserTable->FindUser(pParty->GetMemberID(i)))
					{
						if(object->GetGridID() == m_pMob->GetGridID())
						{
							fprintf(fp, "[PartyMemberIdx : %d]\n", pParty->GetMemberID(i));
						}
					}
				}
			}

			fclose(fp);
		}
	}

	DistributerToSolo(money) ;																	// 개인 경험치와 머니 처리를 한다.

	DistributeToPartys(money) ;																	// 파티 경험치와 머니 처리를 한다.

	DistributeItemToKillers() ;																	// 아이템 처리를 한다.
}

void CDistributer::DistributerToSolo(MONEYTYPE money)
{
	BYTE byPet_ExpPenaltyLevel = PETMGR->GetExpPenaltyLevel() ;

	m_DamageObjectTableSolo.SetPositionHead() ;													// 데미지 테이블을 해드로 세팅한다.

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
	{
		if(obj->dwData > 0)																	// 데미지가 있는지 여부를 체크한다.
		{
			if(CPlayer* const pPlayer = (CPlayer *)g_pUserTable->FindUser(obj->dwID))
			{
				if(pPlayer->GetGridID() != m_pMob->GetGridID())
				{
					continue;
				}
				else if(0 == PARTYMGR->GetParty(pPlayer->GetPartyIdx()))
				{
					if( pPlayer->GetID() == m_1stPlayerID )									// 첫 데미지를 준 플레이어라면, 
					{
						DistributeToFirstKiller(
							*pPlayer,
							obj->dwData);
					}
					else
					{
						DistributeToOtherKiller(
							*pPlayer,
							obj->dwData);
					}

					if(CPet* const pPet = PETMGR->GetPet(pPlayer->GetPetItemDbIndex()))
					{
						if( pPlayer->GetLevel() < m_pMob->GetLevel() + byPet_ExpPenaltyLevel )
						{
							pPet->AddExp() ;
						}
					}

					float fDamageRate = (float)obj->dwData / (float)(m_TotalDamage+(m_FirstDamage*0.3f))  ;		// 총 데미지에서 플레이어의 데미지 백분율을 구한다.

					float fMoney = money * fDamageRate ;									// 몬스터가 드랍한 머니에서 데미지 비율로 머니를 받는다.

					MONEYTYPE dwMoney = (DWORD)fMoney ;										// 소수 버림으로 머니 결정.
					SendMoneyToPerson(pPlayer, dwMoney) ;									// 머니를 전송한다.
				}
			}
		}
	}
}

void CDistributer::DistributeToFirstKiller(CPlayer& pPlayer, DWORD dwMyDamage)
{
	if(pPlayer.GetState() == eObjectState_Die)
	{
		return;
	}

	DWORD dwMonsterExp = CalcObtainExp(
		pPlayer.GetLevel(),
		1);

	if( m_TotalDamage < 1 )																	// 토탈 데미지가 1보다 작으면,
	{
		m_TotalDamage = 1 ;																	// 토탈 데미지를 1로 세팅한다.
	}

	float fExpVal_A = (float)(dwMyDamage * 0.3f) ;											// 자신의 데미지 * 0.3
	float fExpVal_B = (float)(dwMyDamage + fExpVal_A) ;										// 자신의 데미지 + fExpVal_A
	float fExpVal_C = (float)(fExpVal_B / (m_TotalDamage+fExpVal_A)) ;						// fExpVal_B / 모든 유저의 데미지 총합
	float fExpVal_D = (float)(fExpVal_C * dwMonsterExp) ;									// fExpVal_C * 몬스터 경험치

	// 080102 LYW --- Distributer : 솔로 플레이어 경험치 추가 처리.
	DWORD dwCurExp = (DWORD)floor(fExpVal_D) ;
	DWORD dwExp = dwCurExp + (DWORD)(dwCurExp*0.15f) ;												// 반올림 한 경험치를 받는다.

	SendToPersonalExp(
		&pPlayer,
		dwExp);

	if( pPlayer.GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
	{
		CPet* const petObject = PETMGR->GetPet(
			pPlayer.GetPetItemDbIndex());

		if(petObject)
		{
			petObject->AddExp();
		}
	}

	g_csFamilyManager.SRV_ProcessHonorPoint(
		&pPlayer,
		dwExp);
}

void CDistributer::DistributeToOtherKiller(CPlayer& pPlayer, DWORD dwMyDamage)
{
	DWORD dwMonsterExp = CalcObtainExp(pPlayer.GetLevel(), 1) ;
	int nKillerCount = 1;

	if(m_nTotalKillerCount > 1)
	{
		nKillerCount = m_nTotalKillerCount-1;
	}

	if( m_TotalDamage < 1 )
	{
		m_TotalDamage = 1;
	}

	float fExpVal_A = (float)(m_FirstDamage * 0.3f) ;										// 선공격자 데미지 * 0.3
	float fExpVal_B = (float)(fExpVal_A / (nKillerCount)) ;									// fExpVal_A / (공격자 총수 - 1)
	if( fExpVal_B <= 1 ) fExpVal_B = 1 ;
	float fExpVal_C = (float)(dwMyDamage - fExpVal_B) ;										// 자신의 데미지 - fExpVal_B
	if( fExpVal_C <= 1 ) fExpVal_C = 1 ;
	float fExpVal_D = (float)(fExpVal_C / (m_TotalDamage+fExpVal_A)) ;									// fExpVal_C / 모든 유저의 데미지 총합
	float fExpVal_E = (float)(fExpVal_D * dwMonsterExp) ;									// fExpVal_D * 몬스터 경험치.

	DWORD dwCurExp = (DWORD)floor(fExpVal_E);
	DWORD dwExp = dwCurExp + (DWORD)(dwCurExp*0.15f);

	SendToPersonalExp(
		&pPlayer,
		dwExp);

	if( pPlayer.GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
	{
		if(CPet* const petObject = PETMGR->GetPet(pPlayer.GetPetItemDbIndex()))
		{
			petObject->AddExp();
		}
	}

	g_csFamilyManager.SRV_ProcessHonorPoint(
		&pPlayer,
		dwExp);
}

void CDistributer::DistributeToPartys(MONEYTYPE money)
{
	m_DamageObjectTableParty.SetPositionHead();

	while(DAMAGEOBJ* const obj = m_DamageObjectTableParty.GetData())
	{
		if(obj->dwData > 0)																	// 데미지가 있는지 여부를 체크한다.
		{
			if(CParty* const pParty = PARTYMGR->GetParty(obj->dwID))
			{
				if( pParty->GetPartyIdx() == m_1stPartyID )									// 첫 데미지를 준 파티라면, 
				{
					DistributeToFirstParty(
						*pParty,
						obj->dwData);
				}
				else
				{
					DistributeToOtherParty(
						*pParty,
						obj->dwData);
				}

				float fDamageRate = (float)obj->dwData / (float)(m_TotalDamage+(m_FirstDamage*0.3f))  ;			// 전체 데미지에 대한 파티 데미지의 백분율을 구한다.

				float fMoney = money * fDamageRate ;										// 몬스터가 드랍한 머니에서 데미지 비율만큼 머니를 구한다.

				SendMoneyToPartys(fMoney) ;													// 파티로 머니를 전송한다.
			}
		}
	}
}

void CDistributer::DistributeToFirstParty(CParty& pParty, DWORD dwPartyDamage)
{
	LEVELTYPE highLevel = 0;
	LEVELTYPE totalLevel = 0;

	SaveCandidacy(
		pParty);
	GetPartyLevels(
		pParty,
		highLevel,
		totalLevel);

	DWORD dwMonsterExp = CalcObtainExp(highLevel, mPlayerIndexContainer.size()) ;

	if( m_TotalDamage < 1 )
	{
		m_TotalDamage = 1 ;
	}

	float fExpVal_A = (float)(dwPartyDamage * 0.3f) ;										// 자신의 파티데미지 * 0.3
	float fExpVal_B = (float)(dwPartyDamage + fExpVal_A) ;									// 자신의 파티데미지 + fExpVal_A
	float fExpVal_C = (float)(fExpVal_B / (m_TotalDamage+fExpVal_A)) ;						// fExpVal_B / 모든 파티의 데미지 총합
	float fExpVal_D = (float)(fExpVal_C * dwMonsterExp) ;									// fExpVal_C * 몬스터 경험치.			

	if(mPlayerIndexContainer.empty())
	{
		return;
	}
	else if(1 == mPlayerIndexContainer.size())
	{
		if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(*mPlayerIndexContainer.begin()))
		{
			if(pMember->GetGridID() != m_pMob->GetGridID())
			{
				return;
			}

			DWORD dwExp = (DWORD)fExpVal_D;
			g_csFamilyManager.SRV_ProcessHonorPoint(pMember, dwExp);

			DWORD dwCurExp = dwExp + (DWORD)(dwExp*0.15f) ;
			SendToPersonalExp(
				pMember,
				dwCurExp);

			if( pMember->GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
			{
				CPet* const petObject = PETMGR->GetPet(
					pMember->GetPetItemDbIndex());

				if(petObject)
				{
					petObject->AddExp();
				}
			}
		}
	}
	// 보상을 받을 다른 플레이어가 있으면
	else
	{
		BOOL bLevelPenalty = FALSE ;
		LEVELTYPE memberLevel ;
		LEVELTYPE lowLevel = 150 ;
		float fTotalLvWeight = 0 ;

		GetPartyLevels(
			pParty,
			highLevel,
			totalLevel) ;

		for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
			mPlayerIndexContainer.end() != iterator;
			++iterator)
		{
			if(CPlayer* pMember = (CPlayer*)g_pUserTable->FindUser(*iterator))
			{
				memberLevel = pMember->GetLevel();
				// 레벨비중	= ( 상수 - ( 최고레벨 - 자기레벨 ) ) / 상수
				fTotalLvWeight += abs( (float)( MAX_CHARACTER_LEVEL_NUM - ( highLevel - memberLevel ) ) / (float)MAX_CHARACTER_LEVEL_NUM );
			}
		}

		if( (highLevel - lowLevel) >= 21 ) bLevelPenalty = TRUE ;

		float applyRate = 0.0f ;

		switch(mPlayerIndexContainer.size())
		{
		case 2 : applyRate = 0.50f ;	break ;
		case 3 : applyRate = 0.86f ;	break ;
		case 4 : applyRate = 1.21f ;	break ;
		case 5 : applyRate = 1.58f ;	break ;
		case 6 : applyRate = 1.98f ;	break ;
		case 7 : applyRate = 2.41f ;	break ;
		}

		if( bLevelPenalty )
		{
			applyRate = 0.0f ;
		}

		for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
			mPlayerIndexContainer.end() != iterator;
			++iterator)
		{
			if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(*iterator))
			{
				if(pMember->GetGridID() != m_pMob->GetGridID())
				{
					continue;
				}

				memberLevel = pMember->GetLevel();

				// 레벨비중	= ( ( 상수 - ( 최고레벨 - 자기레벨 ) ) / 상수 ) / 모든파티원의 레벨비중 총합 )
				float fExpA = ( abs( (float)( MAX_CHARACTER_LEVEL_NUM - ( highLevel - memberLevel ) ) / (float)MAX_CHARACTER_LEVEL_NUM ) ) / fTotalLvWeight;
				float fExpB = (fExpVal_D * ( 1.f + applyRate ));
				float fExp = fExpA * fExpB;

				if(fExp > 0)
				{
					DWORD dwExp = (DWORD)fExp;

					g_csFamilyManager.SRV_ProcessHonorPoint(pMember, dwExp);

					if( (highLevel - memberLevel) >= 21 )
					{
						SendToPersonalExp(pMember,1);
					}
					else
					{
						SendToPersonalExp(pMember,dwExp);
					}

					if(CPet* const pPet = PETMGR->GetPet(pMember->GetPetItemDbIndex()))
					{
						if( pMember->GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
						{
							pPet->AddExp() ;
						}
					}
				}
			}
		}
	}
}

void CDistributer::DistributeToOtherParty(CParty& pParty, DWORD dwPartyDamage)
{
	LEVELTYPE highLevel = 0;
	LEVELTYPE totalLevel = 0;

	SaveCandidacy(
		pParty);
	GetPartyLevels(
		pParty,
		highLevel,
		totalLevel);

	DWORD dwMonsterExp = CalcObtainExp(highLevel, mPlayerIndexContainer.size());
	int nKillerCount = 1;

	if( m_nTotalKillerCount > 1 )
	{
		nKillerCount = m_TotalDamage -1;
	}

	if( m_TotalDamage < 1 )
	{
		m_TotalDamage = 1;
	}

	float fExpVal_A = (float)(m_FirstDamage * 0.3f) ;										// 선공격 파티 데미지 * 0.3
	float fExpVal_B = (float)(fExpVal_A / (nKillerCount)) ;									// fExpVal_A / (파티 총 수 - 1)

	if( fExpVal_B <= 1 ) fExpVal_B = 1 ;													// 음수 체크를 한다.

	float fExpVal_C = (float)(dwPartyDamage - fExpVal_B) ;									// 자신의 파티 데미지 - fExpVal_B

	if( fExpVal_C <= 1 ) fExpVal_C = 1 ;													// 음수 체크를한다.

	float fExpVal_D = (float)(fExpVal_C / (m_TotalDamage+fExpVal_A)) ;						// fExpVal_C / 모든 파티의 데미지 총합
	float fExpVal_E = (float)(fExpVal_D * dwMonsterExp) ;

	if(mPlayerIndexContainer.empty())
	{
		return;
	}

	if(1 == mPlayerIndexContainer.size())
	{
		if(CPlayer* pMember = (CPlayer*)g_pUserTable->FindUser(*mPlayerIndexContainer.begin()))
		{
			if(pMember->GetGridID() != m_pMob->GetGridID())
			{
				return;
			}

			DWORD dwExp = (DWORD)fExpVal_E ;
			g_csFamilyManager.SRV_ProcessHonorPoint(
				pMember,
				dwExp);
			SendToPersonalExp(
				pMember,
				dwExp);

			if( pMember->GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
			{
				if(CPet* const petObject = PETMGR->GetPet(pMember->GetPetItemDbIndex()))
				{
					petObject->AddExp();
				}
			}
		}
	}
	// 보상을 받을 다른 플레이어가 있으면
	else
	{
		BOOL bLevelPenalty = FALSE;
		LEVELTYPE memberLevel;
		LEVELTYPE lowLevel = 150;
		float fTotalLvWeight = 0;

		for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
			mPlayerIndexContainer.end() != iterator;
			++iterator)
		{
			if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(*iterator))														// 멤버 정보가 유효한지 체크한다.
			{
				memberLevel = pMember->GetLevel();
				// 레벨비중	= ( 상수 - ( 최고레벨 - 자기레벨 ) ) / 상수
				fTotalLvWeight += abs( (float)( MAX_CHARACTER_LEVEL_NUM - ( highLevel - memberLevel ) ) / (float)MAX_CHARACTER_LEVEL_NUM );
			}
		}

		if( (highLevel - lowLevel) >= 21 ) bLevelPenalty = TRUE ;

		float applyRate = 0.0f;

		switch(mPlayerIndexContainer.size())
		{
		case 2 : applyRate = 0.50f ;	break ;
		case 3 : applyRate = 0.86f ;	break ;
		case 4 : applyRate = 1.21f ;	break ;
		case 5 : applyRate = 1.58f ;	break ;
		case 6 : applyRate = 1.98f ;	break ;
		case 7 : applyRate = 2.41f ;	break ;
		}

		if( bLevelPenalty )
		{
			applyRate = 0.0f ;
		}

		for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
			mPlayerIndexContainer.end() != iterator;
			++iterator)
		{
			if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(*iterator))
			{
				if(pMember->GetGridID() != m_pMob->GetGridID())
				{
					continue;
				}

				memberLevel = pMember->GetLevel() ;

				// 레벨비중	= ( ( 상수 - ( 최고레벨 - 자기레벨 ) ) / 상수 ) / 모든파티원의 레벨비중 총합 )
				float fExpA = ( abs( (float)( MAX_CHARACTER_LEVEL_NUM - ( highLevel - memberLevel ) ) / (float)MAX_CHARACTER_LEVEL_NUM ) ) / fTotalLvWeight;
				float fExpB = (fExpVal_E * ( 1.f + applyRate ));
				float fExp = fExpA * fExpB ;

				if(fExp > 0)
				{
					DWORD dwExp = (DWORD)fExp ;
					g_csFamilyManager.SRV_ProcessHonorPoint(pMember, dwExp);

					if( (highLevel - memberLevel) >= 21 )
					{
						SendToPersonalExp(pMember,1);
					}
					else
					{
						SendToPersonalExp(pMember,dwExp);
					}

					if(CPet* const pPet = PETMGR->GetPet(pMember->GetPetItemDbIndex()))
					{
						if( pMember->GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
						{
							pPet->AddExp() ;
						}
					}
				}
			}
		}
	}
}

void CDistributer::SendMoneyToPartys(float ChangeValue)
{
	if(mPlayerIndexContainer.empty())
	{
		return;
	}

	float fMoney = ChangeValue / float(mPlayerIndexContainer.size());
	MONEYTYPE dwMoney = (DWORD)fMoney ;

	if(0 == m_pMob)
	{
		return;
	}

	for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
		mPlayerIndexContainer.end() != iterator;
		++iterator)
	{
		CPlayer* pMember = (CPlayer*)g_pUserTable->FindUser(*iterator) ;						// 멤버 아이디로 멤버 정보를 받는다.

		if( pMember )																	// 멤버 정보가 유효한지 체크한다.
		{
			if(pMember->GetGridID() != m_pMob->GetGridID())
			{
				continue;
			}

			if( pMember->SetMoney(dwMoney, MONEY_ADDITION, MF_OBTAIN, eItemTable_Inventory,	// 각 멤버에게 머니를 전송한다.
				eMoneyLog_GetMonster, m_pMob->GetMonsterKind()) != dwMoney )
			{
				// error msg 보낸다. 제한량 초과
				MSGBASE msg;
				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_MONEY_ERROR;
				msg.dwObjectID = pMember->GetID();

				pMember->SendMsg(&msg, sizeof(msg));
			}
		}
	}
}





//=========================================================================
//	NAME : DistributeItemToKillers
//	DESC : The function to distribute item to killers. 080114 LYW
//=========================================================================
void CDistributer::DistributeItemToKillers() 
{
	BOOL bNoPlayer = FALSE ;
	BOOL bNoParty  = FALSE ;
	DWORD dwEventRewardItem = 0;
	if(m_pMob->GetObjectKind()==eObjectKind_BossMonster)
		dwEventRewardItem = BOSSREWARDSMGR->GetBossEventRewardItem(m_pMob->GetMonsterKind());
	else
		dwEventRewardItem = GAMERESRCMNGR->GetMonsterEventRewardItem(m_pMob->GetMonsterKind());

	// 개인 플레이어 중 아이템 획득 후보를 뽑는다.

	DWORD dwBigPlayerDamage = 0 ;																// 가장 큰 개인 데미지를 담을 변수.
	DWORD dwBigPlayerID = 0 ;																	// 가장 큰 데미지를 준 플레이어 아이디를 담을 변수.

	m_DamageObjectTableSolo.SetPositionHead() ;													// 데미지 테이블을 해드로 세팅한다.

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
	{
		CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(obj->dwID);

		if( !pPlayer ) continue ;																// 플레이어 정보가 유효한지 체크한다.

		if( pPlayer->GetState() == eObjectState_Die ) continue ;								// 플레이어가 죽었다면, 대상에서 제외 시킨다.

		// 090702 - NYJ 보상아이템이 있는경우 모두에게 준다.
		if(dwEventRewardItem)
			ITEMMGR->ObtainGeneralItem(pPlayer, dwEventRewardItem, 1, eLog_ItemObtainMonster, MP_ITEM_MONSTER_OBTAIN_NOTIFY);

		if(0 == PARTYMGR->GetParty(pPlayer->GetPartyIdx()))
		{
			ChooseOne(
				*obj,
				dwBigPlayerDamage,
				dwBigPlayerID);
		}
	}

	if( dwBigPlayerDamage == 0 ) bNoPlayer = TRUE ;												// 아이템 습특 후보에 개인 플레이어가 없다.

	// 파티 중 아이템 획득 파티를 뽑는다.

	DWORD dwBigPartyDamage = 0 ;																// 가장 큰 파티 데미지를 담을 변수.
	DWORD dwBigPartyID = 0 ;																	// 가장 큰 파티 테미지를 준 파타 아이디를 담을 변수.

	m_DamageObjectTableParty.SetPositionHead() ;												// 데미지 테이블을 해드로 세팅한다.

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableParty.GetData())
	{
		ChooseOne(
			*obj,
			dwBigPartyDamage,
			dwBigPartyID);
	}

	if( dwBigPartyDamage == 0 ) bNoParty = TRUE ;

	if( bNoPlayer && bNoParty ) return;

	if( !bNoPlayer && !bNoParty )
	{
		// 파티와 개인의 데미지를 비교한다.
		if( dwBigPlayerDamage == dwBigPartyDamage )													// 개인의 데미지와 파티의 데미지가 같다면,
		{
			int nResult = random(0, 100) ;															// 랜덤 수를 구한다.

			if( nResult < 50 )																		// 랜덤 수가 50미만이면, 
			{
				dwBigPlayerDamage = 0 ;																// 파티에게 아이템을 돌린다.
				dwBigPartyDamage  = 1 ;
			}
			else																					// 랜덤 수가 50을 초과하면,
			{
				dwBigPlayerDamage = 1 ;																// 개인에게 아이템을 돌린다.
				dwBigPartyDamage  = 0 ;
			}
		}

		if( dwBigPlayerDamage > dwBigPartyDamage )													// 개인의 데미지가 파티의 데미지 보다 크면,
		{
			if(CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(dwBigPlayerID))
			{
				SendItemToPerson(pPlayer) ;															// 가장 큰 데미지를 준 플레이어에게 아이템을 전송한다.
			}
		}
		else if( dwBigPlayerDamage < dwBigPartyDamage )												// 파티 데미지가 개인의 데미지 보다 크면,
		{
			if(CParty* const pParty = PARTYMGR->GetParty(dwBigPartyID))
			{
				SaveCandidacy(
					*pParty);

				switch(pParty->GetOption())															// 파티의 아이템 분배 방식을 받아온다.
				{
				case ePartyOpt_Sequence : DistribuSequence(*pParty); break;
				case ePartyOpt_Damage :	  DistributeDamage(); break;
				}
			}
		}
	}
	else
	{
		if( bNoPlayer )
		{
			if(CParty* const pParty = PARTYMGR->GetParty(dwBigPartyID))
			{
				SaveCandidacy(
					*pParty);

				switch(pParty->GetOption())															// 파티의 아이템 분배 방식을 받아온다.
				{
				case ePartyOpt_Sequence : DistribuSequence(*pParty); break;
				case ePartyOpt_Damage :	  DistributeDamage(); break;
				}
			}
		}
		else if( bNoParty )
		{
			if(CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(dwBigPlayerID))
			{
				SendItemToPerson(pPlayer) ;															// 가장 큰 데미지를 준 플레이어에게 아이템을 전송한다.
			}
		}
	}	
}

void CDistributer::DistributeToKiller()
{
	if( m_pKiller )																				// 킬러 정보가 유효한지 체크한다.
	{
		if(0 == PARTYMGR->GetParty(m_pKiller->GetPartyIdx()))
		{
			DistributeToPerson(
				*m_pKiller);
		}
		else																					// 킬러가 파티에 속해있다면,
		{
			DistributeToParty(
				*m_pKiller);
		}
	}
}

void CDistributer::DistributeToPerson(CPlayer& pPlayer)
{
	if(m_pMob)
	{
		if(pPlayer.GetGridID() != m_pMob->GetGridID())
		{
			return;
		}
		else if(pPlayer.GetState() == eObjectState_Die)
		{
			return;
		}

		DWORD dwCur = CalcObtainExp(pPlayer.GetLevel(), 1);
		EXPTYPE dwExp = dwCur + (EXPTYPE)(dwCur * 0.15f);
		int nPenaltyType = GetLevelPenaltyToMonster(m_HighLevel);
		MONEYTYPE money = ITEMDROP_OBJ->DropMoney(
			m_pMob->GetSMonsterList(),
			nPenaltyType);

		g_csFamilyManager.SRV_ProcessHonorPoint(
			&pPlayer,
			DWORD(dwExp));
		SendToPersonalExp(
			&pPlayer,
			dwExp);

		if( pPlayer.GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
		{
			if(CPet* const petObject = PETMGR->GetPet(pPlayer.GetPetItemDbIndex()))
			{
				petObject->AddExp();
			}
		}

		SendMoneyToPerson(
			&pPlayer,
			money);
		SendItemToPerson(
			&pPlayer);

		if(DWORD dwEventRewardItem = GAMERESRCMNGR->GetMonsterEventRewardItem(m_pMob->GetMonsterKind()))
		{
			ITEMMGR->ObtainGeneralItem(
				&pPlayer,
				dwEventRewardItem,
				1,
				eLog_ItemObtainMonster,
				MP_ITEM_MONSTER_OBTAIN_NOTIFY);
		}

		// 081027 LUJ, 특수 몬스터일 경우 획득한 경험치를 로그에 남긴다
		switch( m_pMob->GetObjectKind() )
		{
		case eObjectKind_BossMonster:
		case eObjectKind_FieldBossMonster:
		case eObjectKind_FieldSubMonster:
			{
				InsertLogExp(
					eExpLog_Get,
					pPlayer.GetID(),
					pPlayer.GetLevel(),
					dwExp,
					pPlayer.GetPlayerExpPoint(),
					m_pMob->GetMonsterKind(),
					m_pMob->GetObjectKind(),
					0 );
				break;
			}
		}
	}
}

void CDistributer::DistributeToParty(CPlayer& pPlayer)
{	
	if( !m_pMob ) return ;

	if(CParty* pParty = PARTYMGR->GetParty(pPlayer.GetPartyIdx()))
	{
		SaveCandidacy(
			*pParty);

		if(mPlayerIndexContainer.empty())
		{
			return;
		}
		else if(1 == mPlayerIndexContainer.size())
		{
			DistributeToPerson(
				pPlayer);
			return;
		}
		// 보상을 받을 다른 플레이어가 있으면
		else
		{
			DWORD dwEventRewardItem = GAMERESRCMNGR->GetMonsterEventRewardItem(m_pMob->GetMonsterKind());
			BOOL bLevelPenalty = FALSE;
			LEVELTYPE highLevel = 0;
			LEVELTYPE totalLevel = 0;
			LEVELTYPE lowLevel = 150;
			LEVELTYPE memberLevel = 0;
			float fTotalLvWeight = 0;

			GetPartyLevels(
				*pParty,
				highLevel,
				totalLevel);

			for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
				mPlayerIndexContainer.end() != iterator;
				++iterator)
			{
				const DWORD playerIndex = *iterator;

				if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(playerIndex))
				{
					// 090702 - NYJ 보상아이템이 있는경우 데미지를 준 후보만 준다.
					if(dwEventRewardItem)
					{
						if(DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData(playerIndex))
						{
							ITEMMGR->ObtainGeneralItem(pMember, dwEventRewardItem, 1, eLog_ItemObtainMonster, MP_ITEM_MONSTER_OBTAIN_NOTIFY);
						}
					}

					memberLevel = pMember->GetLevel() ;
					// 레벨비중	= ( 상수 - ( 최고레벨 - 자기레벨 ) ) / 상수
					fTotalLvWeight += abs( (float)( MAX_CHARACTER_LEVEL_NUM - ( highLevel - memberLevel ) ) / (float)MAX_CHARACTER_LEVEL_NUM );
				}
			}

			// 091123 ONS 챌린지존 경험치 분배 처리 추가 : 챌린지존일경우 패널티를 주지 않는다.
			BOOL bIsChallengeZone = FALSE;

			if(g_csDateManager.IsChallengeZoneHere())
			{
				bIsChallengeZone = TRUE;
			}

			if( ((highLevel - lowLevel) >= 21) && !bIsChallengeZone ) bLevelPenalty = TRUE ;

			DWORD partyexp = CalcObtainExp(highLevel, mPlayerIndexContainer.size());
			float applyRate = 0.0f;

			switch(mPlayerIndexContainer.size())
			{
			case 2 : applyRate = 0.50f ;	break ;
			case 3 : applyRate = 0.86f ;	break ;
			case 4 : applyRate = 1.21f ;	break ;
			case 5 : applyRate = 1.58f ;	break ;
			case 6 : applyRate = 1.98f ;	break ;
			case 7 : applyRate = 2.41f ;	break ;
			}

			if( bLevelPenalty )
			{
				applyRate = 0.0f ;
			}

			const int nPenaltyType = GetLevelPenaltyToMonster(m_HighLevel);
			const MONEYTYPE money = ITEMDROP_OBJ->DropMoney(
				m_pMob->GetSMonsterList(),
				nPenaltyType);
			const MONEYTYPE ObtainMoney = money / mPlayerIndexContainer.size();

			for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
				mPlayerIndexContainer.end() != iterator;
				++iterator)
			{
				if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(*iterator))
				{
					if(pMember->GetGridID() != m_pMob->GetGridID())
					{
						continue;
					}

					float fDistributedExp = 0.0f;

					if( bIsChallengeZone )
					{
						float fExpRate = ((float)pMember->GetChallengeZoneExpRate()) * 0.01f;
						fDistributedExp = (partyexp * ( 1.f + applyRate )) * fExpRate;
					}
					else
					{
						int nPersonPenaltyType = GetLevelPenaltyToMonster(pMember->GetLevel());
						if(nPersonPenaltyType == 3)
						{
							SendToPersonalExp(pMember,1);
							continue;
						}

						memberLevel = pMember->GetLevel();

						// 레벨비중	= ( ( 상수 - ( 최고레벨 - 자기레벨 ) ) / 상수 ) / 모든파티원의 레벨비중 총합 )
						float fExpA = ( abs( (float)( MAX_CHARACTER_LEVEL_NUM - ( highLevel - memberLevel ) ) / (float)MAX_CHARACTER_LEVEL_NUM ) ) / fTotalLvWeight;
						float fExpB = (partyexp * ( 1.f + applyRate )) ;				// 경험치 포인트B를 구한다.
						fDistributedExp = fExpA * fExpB ;
					}

					if(fDistributedExp >= 1.0f)
					{
						DWORD dwExp = (DWORD)fDistributedExp;

						if( ((highLevel - memberLevel) >= 21) && !bIsChallengeZone )
						{
							SendToPersonalExp(pMember,1);
						}
						else
						{
							SendToPersonalExp(pMember,dwExp);
						}


						if(CPet* const pPet = PETMGR->GetPet(pMember->GetPetItemDbIndex()))
						{
							if( pMember->GetLevel() < m_pMob->GetLevel() + PETMGR->GetExpPenaltyLevel() )
							{
								pPet->AddExp() ;
							}
						}

						g_csFamilyManager.SRV_ProcessHonorPoint(
							&pPlayer,
							dwExp);
						SendMoneyToPerson(
							pMember,
							ObtainMoney);
					}
				}
			}

			SaveCandidacy(
				*pParty);

			switch(pParty->GetOption())
			{
			case ePartyOpt_Sequence : DistribuSequence(*pParty); break;
			case ePartyOpt_Damage :	  DistributeDamage(); break;
			}
		}
	}
}

void CDistributer::SendMoneyToPerson(CPlayer* pPlayer, MONEYTYPE ChangeValue)
{
	if( pPlayer && m_pMob )																		// 플레이어와 몬스터의 정보가 유효한지 체크한다.
	{
		if(pPlayer->GetGridID() != m_pMob->GetGridID())
		{
			return;
		}

		MONEYTYPE eventValue = (MONEYTYPE)(gEventRate[eEvent_MoneyRate] * ChangeValue);

		// 머니를 플레이어에게 전송한다.
		if( pPlayer->SetMoney(eventValue, MONEY_ADDITION, MF_OBTAIN, eItemTable_Inventory, eMoneyLog_GetMonster, m_pMob->GetMonsterKind()) != eventValue )
		{
			// error msg 보낸다. 제한량 초과
			MSGBASE msg;
			msg.Category = MP_ITEM;
			msg.Protocol = MP_ITEM_MONEY_ERROR;
			msg.dwObjectID = pPlayer->GetID();
			
			pPlayer->SendMsg(&msg, sizeof(msg));
		}
	}
}

void CDistributer::SendItemToPerson(CPlayer* pPlayer)
{
	if( !pPlayer || !m_pMob ) return ;															// 플레이어와 몬스터 정보가 유효한지 체크한다.

	if(pPlayer->GetGridID() != m_pMob->GetGridID())
	{
		return;
	}

	int nMaxItemCount = eDROPITEMKIND_MAX-1;
	const BASE_MONSTER_LIST& baseMonsterList = m_pMob->GetSMonsterList();
	BOOL bObtainItem = FALSE;

	if( m_HighLevel <= 0 )																		// 혹시라도 최고레벨의 값이 문제가 있다면,
	{
		m_HighLevel = pPlayer->GetMaxLevel() ;													// 최고 레벨을 플레이어의 MAX레벨로 세팅한다.
	}

	int nPenaltyType = GetLevelPenaltyToMonster(m_HighLevel) ;									// 몬스터와 레벨차에 따른 패널티 값을 받는다.

	// 090818 ShinJS --- GM-Tool Monster Regen Item Drop 설정시
	if( m_dwDropItemID )
	{
		// 설정된 확률 검사
		if( m_dwDropItemRatio > DWORD( rand() % 100 ) )
		{
			ITEMMGR->MonsterObtainItem(
				pPlayer,
				m_dwDropItemID,
				m_pMob->GetMonsterKind(),
				1);
		}

		m_dwDropItemID = 0;
		m_dwDropItemRatio = 0;
	}

	if( nPenaltyType == 3 ) return ;															// 패널티 값이 3, 21레벨 이상 차이나면 아이템 드랍은 없다.

	if( m_pMob->GetObjectKind() == eObjectKind_FieldBossMonster )
	{
		return;
	}

	for( int count = 0 ;  count < nMaxItemCount ; ++count )
	{
		if(ITEMDROP_OBJ->DropItem(
			m_pMob->GetMonsterKind(),
			baseMonsterList.dropItem[count],
			pPlayer,
			nPenaltyType))
		{
			bObtainItem = TRUE;
		}
	}

	if( bObtainItem )																			// 아이템 할당이 이루어 졌다면, 
	{
		CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx() ) ;							// 파티 정보를 받는다.

		if( pParty )																			// 파티 정보가 유효한지 체크한다.
		{
			BYTE winnerIdx = pParty->GetWinnerIdx() ;											// 당첨자 번호를 받는다.
			
			if(winnerIdx == MAX_PARTY_LISTNUM-1 )												// 당첨 번호 루프의 마지막인지 체크한다.
			{
				pParty->SetWinnerIdx(0) ;														// 당첨 번호를 처음으로 세팅한다.
			}
			else																				// 루프의 마지막이 아니면, 
			{
				pParty->SetWinnerIdx(++winnerIdx) ;												// 당첨 번호를 증가한다.						
			}
		}
	}
}

DWORD CDistributer::CalcObtainExp(LEVELTYPE KillerLevel, int nMemberCount)
{
	if(NULL == m_pMob)
	{
		return 0;
	}

	DWORD dwExp = m_pMob->GetSMonsterList().ExpPoint;

	if(m_pMob->GetObjectKind() == eObjectKind_BossMonster)
	{
		dwExp = (DWORD)BOSSREWARDSMGR->GetExpOfBossMonster(m_pMob->GetMonsterKind());
	}

	const LEVELTYPE MonsterLevel = m_pMob->GetSMonsterList().Level;
	const LEVELTYPE levelGap = (MonsterLevel > KillerLevel ? MonsterLevel - KillerLevel : KillerLevel - MonsterLevel);																// 몬스터와 플레이어의 레벨 차이.

	if( MonsterLevel > KillerLevel + 15 )
	{
		if( m_pMob->GetObjectKind() == eObjectKind_BossMonster )
		{
			//if(MonsterLevel > KillerLevel+16)
			if(MonsterLevel > KillerLevel+20)													// 몬스터 레벨이 플레이어 레벨 보다 21레벨 이상일 때 경험치 1 적용
				return 1;
			else
				return dwExp ;
		}

		double dwLevel = KillerLevel ;												// 레벨 포인트 세팅.
		// 080116 LYW --- Distributer : HP가 절반인 몬스터를 잡았을 때, 획득 경험치 보정 처리.
		double dwPenalty = 1.95f ;																// 페널티 포인트 세팅.
		double result = pow(dwLevel, dwPenalty) ;												// 레벨 포인트 ^ 페널티 포인트를 구한다.

		float fExp = (float)(result + 15) ;														// 플로트형의 크기 내에서 경험치를 구한다.

		if( fExp > dwExp )
		{
			return dwExp ;
		}
		else
		{
			return (DWORD)floor(fExp) ;															// 경험치를 반올림 하여 DWORD형으로 반환.
		}
	}
	else if( MonsterLevel < KillerLevel )														// 플레이어 레벨이 몬스터 레벨보다 큰 경우.
	{
		float fPenalty = 1.0f ;																	// 페널티 포인트.
		float fExp     = 0.0f ;																	// 플로트형 경험치.

		// 080116 LYW --- Distributer : 데이트 매칭 존일 경우에는 패널티를 주지 않는다.
		if (!g_csDateManager.IsChallengeZoneHere())
		{
			// 091023 pdy 몬스터 레벨 패널티 기준 , 파티 보너스 경험치 기획 변경
			if( levelGap >= 8 && levelGap <= 15 )												// 플레이어 레벨이 +8~+15일 때.
			{
				fPenalty = 0.5f ;																// 페널티 포인트 50%로 세팅
			}
			else if( levelGap >= 16 && levelGap <= 20 )											// 플레이어 레벨이 +15~+20일 때.
			{
				fPenalty = 0.25f ;																// 페널티 포인트 25%로 세팅.
			}
			else if( levelGap >= 21 )															// 플레이어 레벨이 +21 이상일 때.
			{
				return 1 ;																		// 경험치는 무조건 1로 반환한다.
			}
		}

		fExp = (float)(dwExp * fPenalty) ;														// 경험치를 플로트형으로 구한다.

		return (DWORD)floor(fExp) ;																// DWORD형으로 페널티 경험치 반환.
	}	
	else																						// 몬스터와 플레이어 레벨이 같을 때.
	{
		return dwExp ;																			// 플레이어와 몬스터의 레벨이 같을경우 몬스터 경험치 반환.			
	}
}

BOOL CDistributer::GetFirstDamange()
{
	m_FirstDamage = 0 ;																			// 첫 데미지 변수를 0으로 세팅한다.
	
	CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(m_1stPlayerID) ;						// 첫 데미지를 준 플레이어 정보를 받는다.

	if( pPlayer )																				// 플레이어 정보가 유효한지 체크한다.
	{
		DAMAGEOBJ* pDObj = NULL ;																// 데미지 정보를 담을 포인터 변수를 선언한다.

		CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx()) ;							// 플레이어의 파티정보를 받는다.

		if( pParty )																			// 파티에 속해있다면,
		{
			pDObj = m_DamageObjectTableParty.GetData(pParty->GetPartyIdx()) ;					// 파티의 데미지 정보를 받는다.

			if( pDObj )																			// 데미지 정보가 유효한지 체크한다.
			{
				m_FirstDamage = pDObj->dwData ;													// 첫 데미지를 세팅한다.

				return TRUE ;																	// 리턴 TRUE.
			}
		}
		else																					// 파티에 속해있지 않다면,
		{
			pDObj = m_DamageObjectTableSolo.GetData(pPlayer->GetID()) ;							// 플레이어의 데미지 정보를 받는다.

			if( pDObj )																			// 데미지 정보가 유효한지 체크한다.
			{
				m_FirstDamage = pDObj->dwData ;													// 첫 데미지를 세팅한다.

				return TRUE ;																	// 리턴 TRUE.
			}
		}
	}

	return FALSE ;																				// 리턴 FALSE.
}

void CDistributer::GetTotalKillerCount()
{
	m_nTotalKillerCount = 0;

	m_DamageObjectTableSolo.SetPositionHead() ;													// 개인 데미지 테이블 체크.

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
	{
		if(obj->dwData > 0)																	// 데미지가 있는지 여부를 체크한다.
		{
			if(CPlayer* const pPlayer = (CPlayer *)g_pUserTable->FindUser(obj->dwID))
			{
				if(0 == PARTYMGR->GetParty(pPlayer->GetPartyIdx()))
				{
					++m_nTotalKillerCount ;													// 솔로 플레이어 수를 늘린다.
				}
			}
			// 091111 pdy 파티 데미지 산정 방식을 몬스터 보상시에 한번 계산 하도록 변경
			else
			{
				++m_nTotalKillerCount ;														// 접속을 끊은 플레이어 파티원모두 솔로 플레이어로 간주하고 수를 늘린다.
			}
		}
	}

	m_DamageObjectTableParty.SetPositionHead() ;												// 파티 테이블 체크.

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableParty.GetData())
	{
		if(obj->dwData > 0)																	// 데미지가 있는지 여부를 체크한다.
		{
			if(PARTYMGR->GetParty(obj->dwID))
			{
				++m_nTotalKillerCount ;														// 솔로 플레이어 수를 늘린다.
			}
		}
	}
}

void CDistributer::GetHighLevelOfKillers()
{
	m_nTotalKillerCount = 0;

	m_DamageObjectTableSolo.SetPositionHead() ;													// 개인 데미지 테이블 체크.

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
	{
		if(obj->dwData > 0)																	// 데미지가 있는지 여부를 체크한다.
		{
			if(CPlayer* const pPlayer = (CPlayer *)g_pUserTable->FindUser(obj->dwID))
			{
				if(0 == PARTYMGR->GetParty(pPlayer->GetPartyIdx()))
				{
					//090402 pdy 몬스터 보상관련 레벨체크 수정 
					//LEVELTYPE playerLevel = pPlayer->GetLevel() ;							// 플레이어의 레벨을 받는다.
					LEVELTYPE playerLevel = pPlayer->GetMaxLevel() ;

					if( playerLevel > m_HighLevel )											// 플레이어의 레벨이 최고 레벨보다 크다면,
					{
						m_HighLevel = playerLevel ;											// 최고 레벨에 플레이어 레벨을 세팅한다.
					}
				}
			}
		}
	}

	m_DamageObjectTableParty.SetPositionHead() ;												// 파티 데미지 테이블 체크.

	while(DAMAGEOBJ* const obj = m_DamageObjectTableParty.GetData())
	{
		if(obj->dwData > 0)																	// 데미지가 있는지 여부를 체크한다.
		{
			if(CParty* const pParty = PARTYMGR->GetParty(obj->dwID))
			{
				LEVELTYPE highLevel = 0 ;													// 
				LEVELTYPE totalLevel = 0 ;

				GetPartyLevels(
					*pParty,
					highLevel,
					totalLevel,
					eGET_MAXLEVEL);

				if( highLevel > m_HighLevel )												// 플레이어의 레벨이 최고 레벨보다 크다면,
				{
					m_HighLevel = highLevel ;												// 최고 레벨에 플레이어 레벨을 세팅한다.
				}
			}
		}
	}
}

int CDistributer::GetLevelPenaltyToMonster(LEVELTYPE level) 
{

	if (g_csDateManager.IsChallengeZoneHere())
	{
		return 0;
	}

	LEVELTYPE MonsterLevel = m_pMob->GetSMonsterList().Level;
	LEVELTYPE levelGap = 0;
	
	if( MonsterLevel <= level )																	// 몬스터 레벨이 플레이어 보다 큰 경우.
	{
		levelGap = level - MonsterLevel ;														// 레벨 차이를 구한다.
	}

	// 091023 pdy 몬스터 레벨 패널티 기준 , 파티 보너스 경험치 기획 변경
	if( levelGap > 7 && levelGap < 16 )															// 레벨차이가 8~15이면, 
	{
		return 1;
	}
	else if( levelGap > 15 && levelGap < 21 )													// 레벨차이가 16~20이면, 
	{
		return 2;
	}
	else if( levelGap > 20 )																	// 레벨 차이가 20이상이면,
	{
		return 3;
	}

	return 0;
}

void CDistributer::GetPartyLevels(CParty& pParty, LEVELTYPE& pHighLevel, LEVELTYPE& pTotalLevel , eGET_LVTYPE eGetLvType )
{
	LEVELTYPE totallevel = 0;
	LEVELTYPE highlevel = 0;
	LEVELTYPE curlevel = 0;
	
	for(int count = 0 ; count < MAX_PARTY_LISTNUM ; ++count)									// 로그인 중인 파티원 수와 그 중 최고 레벨을 구한다.
	{
		if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(pParty.GetMemberID(count)))																			// 멤버 정보가 유효한지 체크.
		{
			if(Chk(*pMember, m_pMob->GetGridID()))
			{

				if( eGetLvType == eGET_MAXLEVEL )
					curlevel = pMember->GetMaxLevel();
				else
					curlevel = pMember->GetLevel();

				if(highlevel < curlevel)
				{
					highlevel = curlevel;
				}

				totallevel = LEVELTYPE( totallevel + curlevel);
			}
		}
	}

	pHighLevel = highlevel;
	pTotalLevel = totallevel;
}

// 091111 pdy 파티 데미지 산정 방식을 몬스터 보상시에 한번 계산 하도록 변경
void CDistributer::GetAllPartyDamage()
{
	m_DamageObjectTableSolo.SetPositionHead();

	while(const DAMAGEOBJ* const obj = m_DamageObjectTableSolo.GetData())
	{
		if(obj->dwData > 0)																	// 데미지가 있는지 여부를 체크한다.
		{
			if(CPlayer* const pPlayer = (CPlayer *)g_pUserTable->FindUser(obj->dwID))
			{
				if(pPlayer->GetGridID() != m_pMob->GetGridID())
				{
					continue;
				}
				else if(PARTYMGR->GetParty(pPlayer->GetPartyIdx()))
				{
					DoAddDamageObj(
						m_DamageObjectTableParty,
						pPlayer->GetPartyIdx(),
						obj->dwData);
				}
			}
		}
	}
}

void CDistributer::SaveCandidacy(CParty& pParty)
{
	if(NULL == m_pMob)
	{
		return;
	}

	mPlayerIndexContainer.clear();

	for(int count = 0 ; count < MAX_PARTY_LISTNUM ; ++count)									// 최대 파티 멤버 수 만큼 포문.
	{
		if(CPlayer* const pMember = (CPlayer*)g_pUserTable->FindUser(pParty.GetMemberID(count)))
		{
			// 071215 LYW --- Distributer : 파티 멤버가 죽은 상태에서는, 보상멤버로 추가하지 않는다.
			if( pMember->GetState() == eObjectState_Die ) continue ;

			if(Chk(*pMember, m_pMob->GetGridID()))
			{
				mPlayerIndexContainer.push_back(
					pMember->GetID());
			}
		}
	}
}

void CDistributer::DistribuSequence(CParty& pParty)
{
	if(mPlayerIndexContainer.empty())
	{
		return;
	}

	const size_t nWinnerIdx = size_t(pParty.GetWinnerIdx());
	DWORD playerIndex = 0;

	if( nWinnerIdx >= mPlayerIndexContainer.size())
	{
		pParty.SetWinnerIdx(0);
		playerIndex = *mPlayerIndexContainer.begin();
	}
	else if(nWinnerIdx < mPlayerIndexContainer.size())
	{
		IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
		std::advance(
			iterator,
			nWinnerIdx);

		playerIndex = *iterator;
	}

	CPlayer* pMember = (CPlayer*)g_pUserTable->FindUser(
		playerIndex);

	if( pMember )																	// 멤버 정보가 유효한제 체크한다.
	{
		if(pMember->GetGridID() != m_pMob->GetGridID())
		{
			return;
		}

		SendItemToPerson( pMember ) ;												// 아이템을 멤버에게 전송한다.
	}
}

void CDistributer::DistributeDamage()
{
	DWORD bigDamage = 0 ;																		// 가장 큰 데미지.
	DWORD bigID = 0 ;																			// 가장 큰 데미지를 준 아이디.

	for(IndexContainer::const_iterator iterator = mPlayerIndexContainer.begin();
		mPlayerIndexContainer.end() != iterator;
		++iterator)
	{
		DAMAGEOBJ* obj = m_DamageObjectTableSolo.GetData(*iterator) ;								// 데미지 테이블에서 아이디로 데미지 정보를 받는다.

		if( obj )																		// 데미지 정보가 유효한지 체크한다.
		{
			if(bigDamage < obj->dwData)													// 최대 데미지와 멤버의 데미지 비교.
			{
				bigDamage = obj->dwData ;												// 최대 데미지에 멤버의 데미지를 대입.
				bigID = obj->dwID ;														// 최대 데미지를 준 멤버의 아이디를 저장.				
			}
		}
	}

	if( bigID == 0 ) return ;																	// 가장 큰 데미지의 멤버 아이디를 체크한다.

	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(bigID);									// 멤버 정보를 받는다.

	if( pPlayer )																				// 멤버 정보가 유효한지 체크.
	{
		if(pPlayer->GetGridID() != m_pMob->GetGridID())
		{
			return;
		}

		SendItemToPerson( pPlayer ) ;															// 아이템을 전송한다.							
	}
}

void CDistributer::SendToPersonalExp(CPlayer* pReceivePlayer, EXPTYPE Exp)
{
	Exp = (DWORD)(Exp*gEventRate[eEvent_ExpRate]);
	//// 071221 KTH -- 경험치 상승 버프 적용
	Exp += EXPTYPE(Exp * (pReceivePlayer->GetRateBuffStatus()->GetExp/100));

	LEVELTYPE PlayerLv = pReceivePlayer->GetLevel();
	DWORD LimitExp = 0;

	if( m_pMob->GetObjectKind() == eObjectKind_BossMonster )
	{
		LEVELTYPE MonsterLv = m_pMob->GetLevel();

		if( abs( int(MonsterLv) - int(PlayerLv) ) >= 21 )	// 보스몬스터일 경우 21레벨(high/low 모두) 이상 차이나면 채크한다
		{
			double dwLevel = PlayerLv ;																								
			double result = pow(dwLevel, 1.95) ;												
			float fExp = (float)( (result + 15) * 20) ;														
			LimitExp = (DWORD)floor(fExp) ;			

			if( LimitExp < Exp )							// 지급 받는 exp가 상한선 보다 높다면 상한선 경험치로 설정
			{
				Exp = LimitExp ;
			}
		}
	}
	else 
	{
		if(g_csDateManager.IsChallengeZoneHere())
		{
			double dwLevel = PlayerLv ;						// 데이트존일경우 상한선 = ( (레벨^1.95) + 15 ) * 10																			
			double result = pow(dwLevel, 1.95) ;												
			float fExp = (float)( (result + 15) * 5) ;														
			LimitExp = (DWORD)floor(fExp) ;			
		}
		else
		{
			double dwLevel = PlayerLv ;						// 일반의 경우 상한선 = ( (레벨^1.95) + 15 ) * 20																										
			double result = pow(dwLevel, 1.95) ;												
			float fExp = (float)( (result + 15) * 20) ;														
			LimitExp = (DWORD)floor(fExp) ;					
		}

		if( LimitExp < Exp )							// 지급 받는 exp가 상한선 보다 높다면 상한선 경험치로 설정
		{
			Exp = LimitExp ;
		}
	}

	pReceivePlayer->AddPlayerExpPoint(Exp);
}

void CDistributer::AddMoney(CPlayer* pPlayer, MONEYTYPE Money, WORD MonsterKind)
{
	if( pPlayer->SetMoney(Money, MONEY_ADDITION, MF_OBTAIN, eItemTable_Inventory, eMoneyLog_GetMonster, MonsterKind) != Money )
	{
		MSGBASE msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_MONEY_ERROR;

		pPlayer->SendMsg(&msg, sizeof(msg));
	}
}