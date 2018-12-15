/*********************************************************************

	 파일		: SHCrop.cpp
	 작성자		: hseos
	 작성일		: 2007/04/10

	 파일설명	: 농작물 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "../Common/SHMath.h"

#include "SHCrop.h"

#if defined(_AGENTSERVER)
#elif defined(_MAPSERVER_)
	#include "../Farm/SHFarmManager.h"

	// 071220 LUJ
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

DWORD	CSHCrop::CROP_LIFE_DECREASE_TIME;														// 농작물 생명력 -1 시간
DWORD	CSHCrop::CROP_PLANT_SEEDING_TIME;														// 농작물을 심고 싹이 나오는데 걸리는 시간
DWORD	CSHCrop::CROP_PLANT_RETRY_TIME;															// 농작물을 다시 심는 데 필요한 시간 (분)
DWORD	CSHCrop::CROP_MANURE_TIME;																// 농작물에 비료를 주는 시간
DWORD	CSHCrop::CROP_MANURE_RETRY_TIME;														// 농작물에 다시 비료를 주는 데 필요한 시간 (초)
DWORD	CSHCrop::CROP_HARVEST_TIME;																// 농작물의 수확 요청 후 수확하는데 걸리는 시간
DWORD	CSHCrop::CROP_DIE_DELAY_TIME = 24*60;													// 생명력이 1이 된 후 죽기까지의 딜레이 시간 (분)

UINT	CSHCrop::CROP_NEXT_STEP_TIME[CROP_SEED_GRADE_MAX][CROP_STEP_MAX-1] = {0,};				// 다음 단계가 되기 위해 필요한 시간 (분)
UINT	CSHCrop::CROP_STEP_MIN_LIFE[CROP_SEED_GRADE_MAX][CROP_STEP_MAX-1] = {0,};				// 단계에 따른 최소/최대 생명력
UINT	CSHCrop::CROP_STEP_MAX_LIFE[CROP_SEED_GRADE_MAX][CROP_STEP_MAX-1] = {0,};				// 단계에 따른 최소/최대 생명력
UINT	CSHCrop::CROP_STEP_MANURE_MIN_LIFE[CROP_SEED_GRADE_MAX][CROP_MANURE_KIND_MAX] = {0,};	// 비료에 따른 증가 생명력
UINT	CSHCrop::CROP_STEP_MANURE_MAX_LIFE[CROP_SEED_GRADE_MAX][CROP_MANURE_KIND_MAX] = {0,};	// 비료에 따른 증가 생명력

UINT	CSHCrop::CROP_HARVEST_PROBA[5][CROP_HARVEST_KIND_MAX+2] = {0,};							// 농작물 생명력에 따른 수확 확률
UINT	CSHCrop::CROP_HARVEST_DELTA_PROBA[CROP_SEED_GRADE_MAX][CROP_HARVEST_KIND_MAX] = {0,};	// 텃밭 등급에 따른 수확 증감 확률

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHCrop Method																												  생성자
//
CSHCrop::CSHCrop()
{
	SetCrop(CROP_RESULT_DIE);
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHCrop Method																												 파괴자
//
CSHCrop::~CSHCrop()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------
// MainLoop Method																									 텃밭 메인 루틴 처리
//
VOID CSHCrop::MainLoop()
{
  	CSHFarmObj::MainLoop();
#if defined(_AGENTSERVER)
#elif defined(_MAPSERVER_)
//	if (g_csFarmManager.GetFarmZone(0) == NULL) return;

 	if (m_nLife > 0)
	{
		// 091211 pdy 다자란 농작물이 삭제가 안되는 문제 수정 
		if (m_eStep == CROP_STEP_COMPLETE)
		{
			if (m_nDieDelayTimeTick)
			{
				if (gCurTime - m_nDieDelayTimeTick > SHMath_MINUTE(1))
				{
					m_nDieDelayTimeTick = gCurTime;
					m_nDieDelayTime++;

					if (m_nDieDelayTime >= CROP_DIE_DELAY_TIME)
					{
						// 071218 LUJ,	로그. 농작물은 정원(SHGarden)에 속해있고, 정원은 농장(CSHFarm)에 속해있다
						{
							CSHGarden*	garden	= ( CSHGarden* )GetParent();
							CSHFarm*	farm	= ( CSHFarm* )( garden ? garden->GetParent() : 0 );

							if( farm )
							{
								InsertLogFarmCrop( farm, this, eLog_FamilyFarmDieCrop );
							}
						}
						
						m_nLife = 0;
						// 클라이언트에 알리기 위한 설정
 						AddEvent(NULL, CROP_RESULT_DIE);
						SetCrop(CROP_RESULT_DIE);
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
			// 생명력이 1 이면 죽기까지의 딜레이 시간을 체크해서 죽음 처리를 한다.
			if (m_nLife == 1)
			{
				if (m_nDieDelayTimeTick)
				{
					if (gCurTime - m_nDieDelayTimeTick > SHMath_MINUTE(1))
					{
						m_nDieDelayTimeTick = gCurTime;
						m_nDieDelayTime++;

						// 091211 pdy 보통단계의 작물 삭제시간이 24시간으로 설정되어있던 버그 수정 
						if (m_nDieDelayTime >= (CROP_LIFE_DECREASE_TIME/60000))	
						{
							// 071218 LUJ,	로그. 농작물은 정원(SHGarden)에 속해있고, 정원은 농장(CSHFarm)에 속해있다
							{
								CSHGarden*	garden	= ( CSHGarden* )GetParent();
								CSHFarm*	farm	= ( CSHFarm* )( garden ? garden->GetParent() : 0 );

								if( farm )
								{
									InsertLogFarmCrop( farm, this, eLog_FamilyFarmDieCrop );
								}
							}
							
							m_nLife = 0;
							// 클라이언트에 알리기 위한 설정
 							AddEvent(NULL, CROP_RESULT_DIE);
							SetCrop(CROP_RESULT_DIE);
						}
					}
					return;
				}
			}

			// 생명력 감소 시간 체크
			if (gCurTime - m_nPlantTimeTick > CROP_LIFE_DECREASE_TIME)
			{
				m_nPlantTimeTick = gCurTime;
				m_nLife--;
				// 생명력이 다함
				if (m_nLife == 0)
				{
					m_nLife = 1;
					// 죽기 까지의 딜레이시간 설정
					m_nDieDelayTimeTick = gCurTime;
					m_nDieDelayTime = 0;
					// 클라이언트에 알리기 위한 설정
 					AddEvent(NULL, CROP_RESULT_LIFE);
					return;
				}
				else
				{
					// 클라이언트에 알리기 위한 설정
 					AddEvent(NULL, CROP_RESULT_LIFE);
				}
			}

			//  단계 상승 처리
			if (gCurTime - m_nNextStepTimeTick > SHMath_MINUTE(1))
			{
				m_nNextStepTimeTick = gCurTime;
				m_nNextStepTime++;
				if (m_nNextStepTime >= GetNextStepTimeTickFromTbl(m_nSeedGrade, m_eStep))
				{
					int nTmpStep = m_eStep + 1;

					// 성장 완료
					if (nTmpStep >= CROP_STEP_COMPLETE)
					{
						// 클라이언트에 알리기 위한 설정
						AddEvent(NULL, CROP_RESULT_STEP_UP_COMPLETE);
						SetCrop(CROP_RESULT_STEP_UP_COMPLETE);
					}
					else
					{
						// 클라이언트에 알리기 위한 설정
						AddEvent(NULL, CROP_RESULT_STEP_UP);
						SetCrop(CROP_RESULT_STEP_UP);
					}
				}
				else
				{
					AddEvent(NULL, CROP_RESULT_NEXT_STEP_TIME);
				}
			}
		}
	}
#else
	// 클라이언트에서는 심는 연출을 위해 처음 Step이 CROP_STEP_1이 아니기 때문에 딜레이 타임 후에 적용
   	if (m_nLife > 0)
	{
 		if (m_eStep == CROP_STEP_SEEDING)
		{
			// 농작물을 심는 중일 때 게이지 표시를 위해..
			CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;
			if (pGuageDlg)
			{
				if ((CObject*)m_pcsParent->GetRenderObjEx(GetID()+15) == OBJECTMGR->GetSelectedObject())
				{
					pGuageDlg->SetMonsterLife(gCurTime-GetPlantTimeTick(), CSHCrop::CROP_PLANT_SEEDING_TIME);
				}
			}
			// 심는 모션 끝
			if (gCurTime - m_nPlantTimeTick > CROP_PLANT_SEEDING_TIME)
			{
				m_eStep = CROP_STEP_1;
				AddEvent(NULL, CROP_RESULT_PLANT);
			}
		}
		// 수확 모션 시간
		else if (m_eStep == CROP_STEP_COMPLETE)
		{
			if (m_nPlantTimeTick)
			{
				if (gCurTime - m_nPlantTimeTick > CROP_HARVEST_TIME)
				{
					m_nPlantTimeTick = NULL;
					// 수확 모션 끝
					AddEvent(NULL, CROP_RESULT_HARVEST);
				}
			}
		}
		else
		{
 			// 농작물에 비료를 주고 있는 시간
   			if (GetManureTimeTick())
			{
				CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;

				if (gCurTime - GetManureTimeTick() > CSHCrop::CROP_MANURE_TIME)
				{
					if (pGuageDlg)
					{
						if ((CObject*)GetRenderObj() == OBJECTMGR->GetSelectedObject())
						{
							pGuageDlg->SetMonsterLife(GetLife(), GetMaxLife());
						}
					}
					SetManureTimeTick(NULL);
					AddEvent(NULL, CROP_RESULT_MANURE);
				}
				else
				{
					if (pGuageDlg)
					{
						if ((CObject*)GetRenderObj() == OBJECTMGR->GetSelectedObject())
						{
							int nDelta = ((GetLife()-GetBeforeLife())*(gCurTime - GetManureTimeTick())<<8)/CSHCrop::CROP_MANURE_TIME;
							pGuageDlg->SetMonsterLife((GetBeforeLife()<<8)+nDelta, GetMaxLife()<<8);
						}
					}
				}
			}
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Plant Method																												 농작물 심기
//
VOID CSHCrop::Plant(int nPlayerID, int nSeedGrade, int nKind)
{
	m_nOwnerID = nPlayerID;
	// 클라이언트에서는 심는 연출을 위해 Step 를 다르게 한다. 단지 클라이언트상의 연출을 위함일 뿐
#if defined(_MAPSERVER_)
	m_eStep = CROP_STEP_1;
#else
	m_eStep = CROP_STEP_SEEDING;
#endif
	m_nKind = nKind;
	m_nPlantTimeTick = gCurTime;
	m_nSeedGrade = nSeedGrade;
	m_nNextStepTimeTick = gCurTime;
	m_nLife = GetLifeFromTbl(m_nSeedGrade, m_eStep);
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Manure Method																											   비료 주기
//
VOID CSHCrop::Manure(int nManureGrade, int nManureKind)
{
	int nPlusLife = GetManureLifeFromTbl(nManureGrade, nManureKind);

	m_nBeforeLife = m_nLife;
	m_nLife = m_nLife + (WORD)nPlusLife;

	int nMaxLife = GetMaxLifeFromTbl(m_nSeedGrade, m_eStep);
	if (m_nLife > nMaxLife)
	{
		m_nLife = (WORD)nMaxLife;
	}

	m_nManureFreq[nManureKind-1]++;

	m_nDieDelayTimeTick = 0;
	m_nDieDelayTime = 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Harvest Method																												수확하기
//
CSHCrop::CROP_HARVEST_KIND CSHCrop::Harvest()
{
	// 농작물 수확하기
	// ..농작물의 생명력에 따른 수확 확률
	int nRandom = CSHMath::GetRandomNum(1, 100);
	int nTbaHarvestIndex = CROP_HARVEST_KIND_GREAT;
	int nCheckNum = sizeof(CROP_HARVEST_PROBA)/sizeof(CROP_HARVEST_PROBA[0]);
	for(int i=0; i<nCheckNum; i++)
	{
		if ( GetLife() >= CROP_HARVEST_PROBA[i][0] && GetLife() <= CROP_HARVEST_PROBA[i][1])
		{
			nTbaHarvestIndex = i;
			break;
		}
	}

	// ..텃밭의 등급에 따른 증감 확률
	int nProba = 0;
	int nHarvestKind = CROP_HARVEST_KIND_NORMAL;
	for(int i=0; i<CROP_HARVEST_KIND_MAX; i++)
	{
		nProba += CROP_HARVEST_PROBA[nTbaHarvestIndex][2+i]+CROP_HARVEST_DELTA_PROBA[m_pcsParent->GetGrade()-1][i];
		if (nRandom < nProba)
		{
			nHarvestKind = i;
			break;
		}
	}

	// 저급 비료를 사용한 적이 있으면 대풍,풍작 -> 평작
	if (GetManureFreq(CROP_MANURE_KIND_GRADE1))
	{
		if (nHarvestKind == CROP_HARVEST_KIND_GREAT || 
			nHarvestKind == CROP_HARVEST_KIND_GOOD)
		{
			nHarvestKind = CROP_HARVEST_KIND_NORMAL;
		}
	}
	// 중급 비료를 사용한 적이 있으면 대풍 -> 풍작
	else if (GetManureFreq(CROP_MANURE_KIND_GRADE2))
	{
		if (nHarvestKind == CROP_HARVEST_KIND_GREAT)
		{
			nHarvestKind = CROP_HARVEST_KIND_GOOD;
		}
	}

	return (CROP_HARVEST_KIND)nHarvestKind;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Plant Method																												 농작물 설정
//
VOID CSHCrop::SetCrop(CROP_RESULT eResult)
{
	switch(eResult)
	{
	case CROP_RESULT_DIE:
		m_nOwnerID = 0;
		m_nKind = 0;			
		m_nSeedGrade = 0;			
		m_eStep = CROP_STEP_EMPTY;				
		m_nLife = 0;				
		m_nPlantTimeTick = NULL;		
		m_nManureTimeTick = NULL;
		m_nNextStepTimeTick = gCurTime;	
		m_nNextStepTime = 0;
		m_nBeforeLife = 0;
		m_nDieDelayTimeTick = 0;
		m_nDieDelayTime = 0;
		ZeroMemory(m_nManureFreq, sizeof(m_nManureFreq));
		break;
	case CROP_RESULT_STEP_UP:
		m_nPlantTimeTick = NULL;		
		m_nManureTimeTick = NULL;
		m_nNextStepTimeTick = gCurTime;	
		m_nNextStepTime = 0;
		m_nDieDelayTimeTick = 0;
		m_nDieDelayTime = 0;
		m_eStep = (CROP_STEP)((int)m_eStep + 1);
		m_nLife = GetLife();						// 100623 ONS 작물진화시 현재생명치를 유지한다.
		break;
	case CROP_RESULT_STEP_UP_COMPLETE:
		m_nPlantTimeTick = NULL;		
		m_nManureTimeTick = NULL;
		m_nDieDelayTimeTick = 0;
		m_nDieDelayTime = 0;
		m_nNextStepTimeTick = gCurTime;	
		m_eStep = (CROP_STEP)((int)m_eStep + 1);
		break;
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
// GetNextStepTimeTick Method																농작물 다음 단계 필요 시간 얻기 (테이블에서)
//
WORD CSHCrop::GetNextStepTimeTickFromTbl(int nSeedGrade, CROP_STEP eCurStep)
{
	if (eCurStep < CROP_STEP_1) eCurStep = CROP_STEP_1;
	else if (eCurStep == CROP_STEP_COMPLETE) eCurStep = (CROP_STEP)(CROP_STEP_COMPLETE-1);

	return (WORD)CROP_NEXT_STEP_TIME[nSeedGrade-1][eCurStep-CROP_STEP_1];
}

// -------------------------------------------------------------------------------------------------------------------------------------
// GetLifeFromTbl Method																				 농작물 생명력 얻기 (테이블에서)
//
WORD CSHCrop::GetLifeFromTbl(int nSeedGrade, CROP_STEP eCurStep)
{
	if (eCurStep < CROP_STEP_1) eCurStep = CROP_STEP_1;
	else if (eCurStep == CROP_STEP_COMPLETE) eCurStep = (CROP_STEP)(CROP_STEP_COMPLETE-1);

	return (WORD)CROP_STEP_MIN_LIFE[nSeedGrade-1][eCurStep-CROP_STEP_1];
}

// -------------------------------------------------------------------------------------------------------------------------------------
// GetMaxLifeFromTbl Method																			농작물 최대 생명력 얻기 (테이블에서)
//
WORD CSHCrop::GetMaxLifeFromTbl(int nSeedGrade, CROP_STEP eCurStep)
{
	if (eCurStep < CROP_STEP_1) eCurStep = CROP_STEP_1;
	else if (eCurStep == CROP_STEP_COMPLETE) eCurStep = (CROP_STEP)(CROP_STEP_COMPLETE-1);

	return (WORD)CROP_STEP_MAX_LIFE[nSeedGrade-1][eCurStep-CROP_STEP_1];
}

// -------------------------------------------------------------------------------------------------------------------------------------
// GetMaxLifeFromTbl Method																			  비료 사용 생명력 얻기 (테이블에서)
//
WORD CSHCrop::GetManureLifeFromTbl(int nGrade, int nKind)
{
	int nReturn = CSHMath::GetRandomNum(CROP_STEP_MANURE_MIN_LIFE[nGrade-1][nKind-1], CROP_STEP_MANURE_MAX_LIFE[nGrade-1][nKind-1]);

	return (WORD)nReturn;
}