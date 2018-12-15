/*********************************************************************

	 파일		: Animal.cpp
	 작성자		: Shinobi
	 작성일		: 2008/03/08

	 파일설명	: 가축 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "../Common/SHMath.h"

#include "Animal.h"

#if defined(_AGENTSERVER)
#elif defined(_MAPSERVER_)
	#include "../Farm/SHFarmManager.h"
	#include "MapDBMsgParser.h"
#else
	#include "../Farm/SHFarmManager.h"
	#include "ObjectManager.h"
	#include "GameIn.h"
	#include "MonsterGuageDlg.h"
	#include "ObjectBalloon.h"
	#include "OBalloonName.h"
	#include "ObjectGuagen.h"
#endif

DWORD CAnimal::ANIMAL_IN_STALL_TIME;															// 가축을 축사에 넣는 시간(분)
DWORD CAnimal::ANIMAL_LIFE_DECREASE_TIME;														// 가축의 생명력 -1 시간
DWORD CAnimal::ANIMAL_RETRY_TIME;																// 가축을 축사에 다시 소환하는 데 필요한 시간 (분)
DWORD CAnimal::ANIMAL_FEED_TIME;																// 가축에게 먹이를 주는 시간
DWORD CAnimal::ANIMAL_FEED_DELAY_TIME;															// 가축에게 다시 먹이를 주는데 필요한 시간
DWORD CAnimal::ANIMAL_CLEANING_TIME;															// 가축을 씻기는대 소요되는 시간 (분)
DWORD CAnimal::ANIMAL_CLEANING_RETRY_TIME;														// 가축을 다시 씻기는대 걸리는 시간 (분)
DWORD CAnimal::ANIMAL_DIE_DELAY_TIME = 24*60;													// 가축의 생명력이 0이 된 후 죽기까지의 딜레이 시간 (분)
DWORD CAnimal::ANIMAL_STALL_REWARD_TIME;														// 가축에게 아이템을 얻는대 걸리는 시간 (분)
DWORD CAnimal::ANIMAL_CONTENTMENT_DECREASE_TIME;												// 가축의 만족도가 -1 되는 시간
DWORD CAnimal::ANIMAL_INTEREST_DECREASE_TIME;													// 가축의 관심도가 -1 되는 시간

UINT CAnimal::ANIMAL_STEP_MIN_LIFE[8][2] = {0,};
UINT CAnimal::ANIMAL_STEP_MAX_LIFE = 4;
UINT CAnimal::ANIMAL_STEP_MAX_CONTENTMENT = 100;
UINT CAnimal::ANIMAL_STEP_MAX_INTEREST = 100;
//UINT CAnimal::ANIMAL_BREED_PROBA[5][ANIMAL_BREED_KIND_MAX+2] = {0,};
UINT		ANIMAL_REWARD_LIST[CAnimal::ANIMAL_KIND_MAX] = {0,};
UINT CAnimal::ANIMAL_STEP_FEED_LIFE[ANIMAL_FEED_KIND_MAX][2] = {0,};
UINT CAnimal::ANIMAL_STEP_CLEANING_LIFE[ANIMAL_CLEANER_KIND_MAX][2] = {0,};
char CAnimal::ANIMAL_NAME_TBL[ANIMAL_KIND_MAX][256];

//----------------------------------------------------------------------------------------------------------------------------------------
// CAnimal Method																													생성자
//
CAnimal::CAnimal()
{
	SetAnimal(ANIMAL_RESULT_DIE);
}

//----------------------------------------------------------------------------------------------------------------------------------------
// ~CAnimal Method																													파괴자
//
CAnimal::~CAnimal()
{
}
//----------------------------------------------------------------------------------------------------------------------------------------
// MainLoop Method																										축사 메인 루틴 처리
//
VOID CAnimal::MainLoop()
{
	CSHFarmObj::MainLoop();
#if defined(_AGENTSERVER)
#elif defined(_MAPSERVER_)
	if(m_nContentment > 0)
	{
		// 091211 pdy 다자란 가축이 삭제되지 않는 문제 수정
		if( m_eStep == ANIMAL_STEP_COMPLETE )
		{
			if (m_nDieDelayTimeTick)
			{
				if (gCurTime - m_nDieDelayTimeTick > SHMath_MINUTE(1))
				{
					m_nDieDelayTimeTick = gCurTime;
					m_nDieDelayTime++;

					if (m_nDieDelayTime >= ANIMAL_DIE_DELAY_TIME)	
					{
						CPen* pAnimalCage = (CPen*)GetParent();
						CSHFarm* pFarm = (CSHFarm*)(pAnimalCage ? pAnimalCage->GetParent() : 0 );

						if( pFarm )
						{
							InsertLogFarmAnimal(pFarm, this, eLog_FamilyFarmDieAnimal);
						}

						AddEvent(NULL, ANIMAL_RESULT_DIE);
						SetAnimal(ANIMAL_RESULT_DIE);
					}
				}
			}
			else
			{
				m_nDieDelayTimeTick = gCurTime ;
				m_nDieDelayTime = 0;
			}
		}
		else
		{
			// 생명력(기존만족도) 시간 체크
			if( m_nContentment > 0 && gCurTime - m_nContentmentTimeTick > ANIMAL_CONTENTMENT_DECREASE_TIME )
			{
				m_nContentmentTimeTick = gCurTime;
				m_nBreedTimeTick = gCurTime;
				m_nContentment--;

				AddEvent(NULL, ANIMAL_RESULT_CONTENTMENT);

				if(m_nContentment <= 0)
				{
					CPen* pAnimalCage = (CPen*)GetParent();
					CSHFarm* pFarm = (CSHFarm*)(pAnimalCage ? pAnimalCage->GetParent() : 0 );

					if( pFarm )
					{
						InsertLogFarmAnimal(pFarm, this, eLog_FamilyFarmDieAnimal);
					}

					AddEvent(NULL, ANIMAL_RESULT_DIE);
					SetAnimal(ANIMAL_RESULT_DIE);
				}
			}
			// 등급도(기존관심도) 시간 체크
			if( m_nInterest > 0 && gCurTime - m_nInterestTimeTick > ANIMAL_INTEREST_DECREASE_TIME )
			{
				m_nInterestTimeTick = gCurTime;
				m_nInterest--;

				if( m_nInterest < 0 )
					m_nInterest = 0;

				AddEvent(NULL, ANIMAL_RESULT_INTEREST);
			}
			// 수확카운트(기존생명력) 감소 시간 체크
			if( m_nLife > 0 && (gCurTime - m_nHavestTimeTick > ANIMAL_LIFE_DECREASE_TIME) )
			{
				m_nHavestTimeTick = gCurTime;
				m_nLife--;

				// 클라이언트에 알리기 위한 설정
				AddEvent(NULL, ANIMAL_RESULT_LIFE);

				if(m_nLife == 0)
				{
					SetAnimal(ANIMAL_RESULT_COMPLETE);
					AddEvent(NULL, ANIMAL_RESULT_COMPLETE);
				}
			}
		}
	}
	else
	{
		// 딜레이 타임 없음
/*		CPen*		pPen		= (CPen*)GetParent();
		CSHFarm*	pFarm		= (CSHFarm*)(pPen ? pPen->GetParent() : 0 );

		if( pFarm )
		{
			// 로그 추가 하여야함.
			//InsertLogFarmAnimal(farm, this, eLog_FamolyFarmDieAnimal);
		}

		// 클라이언트에 알리기 위한 설정
		AddEvent(NULL, ANIMAL_RESULT_DIE);
		SetAnimal(ANIMAL_RESULT_DIE);

		return;*/
	}
#else
	// 클라이언트에서는 가축을 축사 넣는 연출을 위해 처음 Step이 ANIMAL_STEP_1이 아니기 때문에 딜레이 타임 후에 적용
if( m_nContentment > 0 )
	{
		if( m_eStep == ANIMAL_STEP_IN_STALL )
		{
			// 가축을 축사에 넣는 중일 때 게이지 표시를 위해...
			CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg();
			if( pGuageDlg )
			{
				if( (CObject*)m_pcsParent->GetRenderObjEx(GetID()+5) == OBJECTMGR->GetSelectedObject())
				{
					pGuageDlg->SetMonsterLife(gCurTime-GetBreedTimeTick(), CAnimal::ANIMAL_IN_STALL_TIME);
				}
			}
			// 넣는 모션 끝
			if( gCurTime - m_nBreedTimeTick > ANIMAL_IN_STALL_TIME )
			{
				m_eStep = ANIMAL_STEP_1;
				AddEvent(NULL, ANIMAL_RESULT_IN_STALL);
			}
		}
		// 수확 모션 시간 << 조건을 추가 해야함. 현재 아이템 수확이 가능한지 체크 하는 부분이 필요함.
		else if(m_nLife==0 && m_eStep == ANIMAL_STEP_COMPLETE)
		{
			if( m_nBreedTimeTick )
			{
				if( gCurTime - m_nBreedTimeTick > ANIMAL_STALL_REWARD_TIME )
				{
					m_nBreedTimeTick = NULL;
					// 수확 모션 끝
					AddEvent(NULL, ANIMAL_RESULT_REWARD);
				}
			}
		}
		else
		{
			// 가축에 사료를 주고 있는 시간
			if( GetFeedTimeTick() )
			{
				CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg();

				if( gCurTime - GetFeedTimeTick() > CAnimal::ANIMAL_FEED_TIME )
				{
					if( pGuageDlg )
					{
						if( (CObject*)GetRenderObj() == OBJECTMGR->GetSelectedObject() )
						{
							pGuageDlg->SetMonsterLife(GetContentment(), 100);
						}
					}
					SetFeedTimeTick(NULL);
					AddEvent(NULL, ANIMAL_RESULT_FEEDING);
				}
				else
				{
					if( pGuageDlg )
					{
						if( (CObject*)GetRenderObj() == OBJECTMGR->GetSelectedObject() )
						{
							int nDelta = ((GetContentment()-m_nBeforeContentment)*(gCurTime-GetFeedTimeTick())<<8)/CAnimal::ANIMAL_FEED_TIME;
							pGuageDlg->SetMonsterLife((m_nBeforeContentment<<8)+nDelta, 100<<8);
						}
					}
				}
			}
			// 축사 청소를 하고 있는 시간
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Breed Method																													축사 하기
//
VOID CAnimal::Breed(int nPlayerID, int nAnimalCageGrade, int nKind)
{
	m_nOwnerID = nPlayerID;
	// 클라이언트에서는 축사에 넣는 연출을 위해 Step을 다르게 한다. 단지 클라이언트상의 연출을 위함일 뿐임
#if defined(_MAPSERVER_)
	m_eStep = ANIMAL_STEP_1;
#else
	m_eStep = ANIMAL_STEP_IN_STALL;
#endif
	//CPen*		pPen		= (CPen*)GetParent();
	m_nKind = nKind;
	m_nBreedTimeTick = 0;
	m_nContentmentTimeTick = gCurTime;
	m_nInterestTimeTick = gCurTime;
	m_nGrade = 0;
	m_nLife = (WORD)ANIMAL_STEP_MAX_LIFE;
	m_nHavestTimeTick = gCurTime;
	m_nContentment	= (WORD)ANIMAL_STEP_MIN_LIFE[nAnimalCageGrade-1][0];
	m_nInterest		= (WORD)ANIMAL_STEP_MIN_LIFE[nAnimalCageGrade-1][1];
	//m_nLife = GetLifeFromTbl( ((CPen*)GetParent())->GetGrade() );
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Feed Method																													사료 주기
//
VOID CAnimal::Feed(int nGrade)
{
	int nPlusContentment = GetFeedContentmentFromTbl(nGrade);
	int nPlusInterest = GetFeedInterestFromTbl(nGrade);

	m_nBeforeContentment = m_nContentment;
	m_nBeforeInterest = m_nInterest;

	m_nContentment = (WORD)(m_nContentment + nPlusContentment);
	m_nInterest = (WORD)(m_nInterest + nPlusInterest);

	if( m_nContentment > 100 )
	{
		m_nContentment = 100;
	}

	if( m_nInterest > 100 )
	{
		m_nInterest = 100;
	}

	AddEvent(NULL, ANIMAL_RESULT_CONTENTMENT);
	AddEvent(NULL, ANIMAL_RESULT_INTEREST);

}

// ---------------------------------------------------------------------------------------------------------------------------------------
// GetItem Method																												아이템 얻기
//
//CAnimal::ANIMAL_BREED_KIND	CAnimal::Reward()
//{
	// 가축에서 아이템 얻기
	// 가축들의 생명령에 따른 아이템 확률
//	int nRandom = CSHMath::GetRandomNum(1, 100);
	//int nTbaItemIndex = ANIMAL_BREED_KIND_GREAT;
	//int nCheckNum = sizeof(ANIMAL_BREED_PROBA)/sizeof(ANIMAL_BREED_PROBA[0]);
	//for( int i = 0; i < nCheckNum; i++ )
	//{
	//	if( GetLife() >= ANIMAL_BREED_PROBA[i][0] && GetLife() <= ANIMAL_BREED_PROBA[i][1] )
	//	{
	//		nTbaItemIndex = i;
	//		break;
	//	}
	//}

	//// 축사의 등급에 따른 증감 확률
	//int nProba = 0;
	//int nItemKind = ANIMAL_BREED_KIND_NORMAL;
	//for(int i=0; i < ANIMAL_BREED_KIND_MAX; i++)
	//{
	//	nProba += ANIMAL_BREED_PROBA[nTbaItemIndex][2+i]+ANIMAL_BREED_DELTA_PROBA[m_pcsParent->GetGrade()-1][i];
	//	if (nRandom < nProba)
	//	{
	//		nItemKind = i;
	//		break;
	//	}
	//}

	//// 저급 사료를 사용한 적이 있으면
	//if( GetFeedFreq(ANIMAL_FEED_KIND_GRADE_1) )
	//{
	//	if( nItemKind == ANIMAL_BREED_KIND_GREAT ||
	//		nItemKind == ANIMAL_BREED_KIND_GOOD )
	//	{
	//		nItemKind = ANIMAL_BREED_KIND_NORMAL;
	//	}
	//}
	//// 중급 사료를 사용한 적이 있으면
	//else if( GetFeedFreq(ANIMAL_FEED_KIND_GRADE_2) )
	//{
	//	if( nItemKind == ANIMAL_BREED_KIND_GREAT )
	//	{
	//		nItemKind = ANIMAL_BREED_KIND_GOOD;
	//	}
	//}

	//return (ANIMAL_BREED_KIND)nItemKind;
	//return (ANIMAL_BREED_KIND)1;
//}

// -------------------------------------------------------------------------------------------------------------------------------------
// Plant Method																													가축 설정
//
VOID CAnimal::SetAnimal(ANIMAL_RESULT eResult)
{
	switch(eResult)
	{
	case ANIMAL_RESULT_DIE:
		m_nOwnerID = 0;
		m_nKind = 0;
		m_nGrade = 0;
		m_eStep = ANIMAL_STEP_EMPTY;
		m_nLife = 0;
		m_nBreedTimeTick = NULL;
		m_nFeedTimeTick = NULL;
		m_nNextStepTime = 0;
		m_nBeforeLife = 0;
		m_nContentment = 0;
		m_nInterest = 0;
		m_nDieDelayTimeTick = 0;				
		m_nDieDelayTime = 0;	
		break;
	case ANIMAL_RESULT_COMPLETE:
		m_eStep = ANIMAL_STEP_COMPLETE;
		m_nBreedTimeTick = NULL;
		m_nDieDelayTimeTick = 0;				
		m_nDieDelayTime = 0;	
		break;
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
// GetMaxLifeFromTbl Method																				가축 최대 생명력 얻기 (테이블에서)
//
WORD CAnimal::GetMaxLife()
{
	return (WORD)ANIMAL_STEP_MAX_LIFE;
}
// -------------------------------------------------------------------------------------------------------------------------------------
// GetMaxLifeFromTbl Method																			  사료 사용 생명력 얻기 (테이블에서)
//
WORD CAnimal::GetFeedLifeFromTbl(int nGrade)
{
	WORD nReturn = 0; //ANIMAL_STEP_FEED_LIFE[nGrade];

	return nReturn;
}

WORD CAnimal::GetFeedContentmentFromTbl(int nGrade)
{
	return (WORD)ANIMAL_STEP_FEED_LIFE[nGrade][0];;
}

WORD CAnimal::GetFeedInterestFromTbl(int nGrade)
{
	return (WORD)ANIMAL_STEP_FEED_LIFE[nGrade][1];
}

VOID CAnimal::Cleaning(int nGrade)
{
	//int nPlusContentment = GetFeedContentmentFromTbl(nGrade);
	//int nPlusInterest = GetFeedInterestFromTbl(nGrade);
	int nPlusContentment = ANIMAL_STEP_CLEANING_LIFE[nGrade-1][0];
	int nPlusInterest = ANIMAL_STEP_CLEANING_LIFE[nGrade-1][1];

	m_nBeforeContentment = m_nContentment;
	m_nBeforeInterest = m_nInterest;

	m_nContentment = (WORD)(m_nContentment + nPlusContentment);
	m_nInterest = (WORD)(m_nInterest + nPlusInterest);

	if( m_nContentment > 100 )
	{
		m_nContentment = 100;
	}

	if( m_nInterest > 100 )
	{
		m_nInterest = 100;
	}

	AddEvent(NULL, ANIMAL_RESULT_CONTENTMENT);
	AddEvent(NULL, ANIMAL_RESULT_INTEREST);

	if( m_nContentment == 100 && m_nInterest > 10 )
	{
		SetAnimal(ANIMAL_RESULT_COMPLETE);
		AddEvent(NULL, ANIMAL_RESULT_COMPLETE);
	}
}

WORD CAnimal::GetGrade()
{
	if( m_nInterest >= 10 && m_nInterest < 20 )
		return 0;
	else if( m_nInterest < 29 )
		return 1;
	else if( m_nInterest >= 29 )
		return 2;

	return 0;
}