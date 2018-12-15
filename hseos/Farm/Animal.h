/*********************************************************************

	 파일		: Animal.h
	 작성자		: Shinobi
	 작성일		: 2008/03/08

	 파일설명	: 가축 클래스의 헤더

 *********************************************************************/

#pragma once

#include "SHFarmObj.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

class CAnimal : public CSHFarmObj
{
public:
	//-------------------------------------------------------------------------------------------------------------------------------------------------
	enum ANIMAL_RESULT																				// 가축 관리 결과
	{
		ANIMAL_RESULT_LIFE,																			// 생명력 변화
		ANIMAL_RESULT_DIE,																			// 죽음
		ANIMAL_RESULT_IN_STALL,																		// 축사에 가축 넣기
		ANIMAL_RESULT_FEEDING,																		// 먹이주기
		ANIMAL_RESULT_CLEANNING,																	// 씻기
		ANIMAL_RESULT_REWARD,																		// 생산한 아이템을 얻는다
		ANIMAL_RESULT_CONTENTMENT,																	// 만족도 변화
		ANIMAL_RESULT_INTEREST,																		// 관심도 변화
		ANIMAL_RESULT_COMPLETE,
		ANIMAL_RESULT_MAX,																			// 완료
	};

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	enum ANIMAL_STEP																				// 가축의 단계
	{
		ANIMAL_STEP_EMPTY = 0,																		// 없음.
		ANIMAL_STEP_IN_STALL,																		// 축사에 넣기
		ANIMAL_STEP_1,																				// 1단계
		ANIMAL_STEP_2,																				// 2단계
		ANIMAL_STEP_3,																				// 3단계
		ANIMAL_STEP_COMPLETE,																		// 완료
		ANIMAL_STEP_MAX = ANIMAL_STEP_COMPLETE - ANIMAL_STEP_IN_STALL,
	};

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	enum ANIMAL_FEED_KIND																			// 사료 등급
	{
		ANIMAL_FEED_KIND_GRADE_1 = 0,																// 일반 사료
		ANIMAL_FEED_KIND_GRADE_2,																	// 고급 사료
		ANIMAL_FEED_KIND_MAX
	};

	enum ANIMAL_CLEANER_KIND
	{
		ANIMAL_CLEANER_KIND_GRADE_1 = 0,
		ANIMAL_CLEANER_KIND_MAX
	};

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	enum ANIMAL_KIND
	{
		ANIMAL_KIND_MELEE,
		ANIMAL_KIND_MAGIC,
		ANIMAL_KIND_BUFF,
		ANIMAL_KIND_MAX,
	};


	enum ANIMAL_BREED_KIND																			// 축사 수확
	{
		ANIMAL_BREED_KIND_BAD,																		// 안좋음
		ANIMAL_BREED_KIND_NORMAL,																	// 중간
		ANIMAL_BREED_KIND_GOOD,																		// 좋음
		ANIMAL_BREED_KIND_WORST,																	// 매우 안좋음
		ANIMAL_BREED_KIND_MAX,
	};	

	static DWORD ANIMAL_IN_STALL_TIME;																// 가축을 축사에 넣는 시간 (분)
	static DWORD ANIMAL_LIFE_DECREASE_TIME;															// 가축의 생명력 -1 시간
	static DWORD ANIMAL_RETRY_TIME;																	// 가축을 축사에 다시 소환하는 데 필요한 시간 (분)
	static DWORD ANIMAL_FEED_TIME;																	// 가축에게 먹이를 주는 시간
	static DWORD ANIMAL_CLEANING_TIME;																	// 가축을 씻기는대 소요되는 시간
	static DWORD ANIMAL_CLEANING_RETRY_TIME;															// 가축을 다시 씻기는대 걸리는 시간 (분)
	static DWORD ANIMAL_DIE_DELAY_TIME;																// 가축의 생명력이 0이 된 후 죽기까지의 딜레이 시간 (분)
	static DWORD ANIMAL_STALL_REWARD_TIME;														// 가축에게 아이템을 얻는대 걸리는 시간 (분)
	static DWORD ANIMAL_FEED_DELAY_TIME;															// 가축에게 먹이를 다시 주는데 걸리는 시간

	static UINT		ANIMAL_STEP_MIN_LIFE[8][2];														// 단계에 따른 최소/최대 생명력
	static UINT		ANIMAL_STEP_MAX_LIFE;															// 가축의 최대 HP량
	static UINT		ANIMAL_STEP_FEED_LIFE[ANIMAL_FEED_KIND_MAX][2];									// 사료에 따른 증가 생명력
	static UINT		ANIMAL_STEP_CLEANING_LIFE[ANIMAL_CLEANER_KIND_MAX][2];							// 청소에 따른 증가 생명력

	static UINT		ANIMAL_STEP_MAX_CONTENTMENT;
	static UINT		ANIMAL_STEP_MAX_INTEREST;

	static DWORD ANIMAL_CONTENTMENT_DECREASE_TIME;													// 가축의 만족도가 -1 되는 시간
	static DWORD ANIMAL_INTEREST_DECREASE_TIME;														// 가축의 관심도가 -1 되는 시간

	//static UINT		ANIMAL_BREED_PROBA[5][ANIMAL_BREED_KIND_MAX+2];									// 가축 생명력에 따른 수확 확률

	static UINT		ANIMAL_REWARD_LIST[ANIMAL_KIND_MAX];											// 가축 수확에 얻게될 아이템
	static char		ANIMAL_NAME_TBL[ANIMAL_KIND_MAX][256];
private:
	//----------------------------------------------------------------------------------------------------------------
	int						m_nKind;																// ..종류
	ANIMAL_STEP				m_eStep;																// ..단계
	DWORD					m_nBreedTimeTick;														// ..사육한 시간 틱
	DWORD					m_nFeedTimeTick;														// ..사료 준 시간 틱
	DWORD					m_nCleaningTimeTick;														// ..청소 시간 틱
	WORD					m_nNextStepTime;														// ..사육후 흐른 시간 (분)
	WORD					m_nBeforeLife;															// ..어떤 행위로 인해 생명력이 변화되기 전의 생명력
	WORD					m_nBeforeContentment;
	WORD					m_nBeforeInterest;

	WORD					m_nContentment;															// ..만족
	WORD					m_nInterest;															// ..관심
	DWORD					m_nContentmentTimeTick;
	DWORD					m_nInterestTimeTick;

	// 091211 pdy 다자란 가축이 삭제가 안되는 문제 수정 
	DWORD					m_nDieDelayTimeTick;								// 다자란 가축이 되었을 때 죽기까지의 딜레이 시간 틱
	DWORD					m_nDieDelayTime;									// 다자란 가축이 되었을 때 죽기까지의 딜레이 시간 (분)
	DWORD					m_nHavestTimeTick;									// 수확카운트 감소 틱

	//int						m_nFeedFreq[ANIMAL_FEED_KIND_MAX];										// ..사료를 사용한 횟수

public:
	//----------------------------------------------------------------------------------------------------------------
	CAnimal();
	// 090629 LUJ, 파생 클래스의 소멸자가 호출되도록 수정
	virtual ~CAnimal();
	VOID						SetContentment(WORD contentment)	{	m_nContentment = contentment;	}
	WORD						GetContentment()					{	return m_nContentment;	}

	VOID						SetInterest(WORD interest)			{	m_nInterest = interest;	}
	WORD						GetInterest()						{	return m_nInterest;	}

	//----------------------------------------------------------------------------------------------------------------
	//							가축 종류 설정
	VOID						SetKind(int nKind)			{ m_nKind = nKind; }
	//							가축 종류 얻기
	int							GetKind()					{ return m_nKind; }

	//----------------------------------------------------------------------------------------------------------------
	//							가축 단계 설정
	VOID						SetStep(ANIMAL_STEP eStep)	{ m_eStep = eStep; }
	//							가축 단계 얻기
	ANIMAL_STEP					GetStep()					{ return m_eStep; }

	//----------------------------------------------------------------------------------------------------------------
	//							사육 시킨 시간 설정
	VOID						SetBreedTimeTick(DWORD nTick) { m_nBreedTimeTick = nTick; }
	//							가축을 사육한 시간 얻기
	DWORD						GetBreedTimeTick()			{ return m_nBreedTimeTick; }

	//----------------------------------------------------------------------------------------------------------------
	//							사료 준 시간 설정
	VOID						SetFeedTimeTick(DWORD nTick) { m_nFeedTimeTick = nTick; }
	//							사료 준 시간 얻기
	DWORD						GetFeedTimeTick()			{ return m_nFeedTimeTick; }

	//							청소 한 시간 설정
	VOID						SetCleaningTimeTick(DWORD nTick)	{	m_nCleaningTimeTick = nTick;	}
	//							청소 한 시간 얻기
	DWORD						GetCleaningTimeTick()		{	return m_nCleaningTimeTick;	}

	//----------------------------------------------------------------------------------------------------------------
	//							농작물 설정
	VOID						SetAnimal(ANIMAL_RESULT eResult); 

	//----------------------------------------------------------------------------------------------------------------
	//int							GetFeedFreq(ANIMAL_FEED_KIND eManureKind)		{ return m_nFeedFreq[eManureKind-1]; }
	//----------------------------------------------------------------------------------------------------------------
	//							가축 사육하기
	VOID						Breed(int nPlayerID, int nAnimalCageGrade, int nKind);
	//							사료 주기
	VOID						Feed(int nGrade);
	//							가축 청소
	VOID						Cleaning(int nGrade);
	//							아이템 얻기
	//DWORD						Reward();

	//----------------------------------------------------------------------------------------------------------------
	//							가축 다음 단계 필요 시간 얻기 (테이블에서)
	WORD						GetNextStepTimeTickFromTbl(int nGrade, ANIMAL_STEP eCurStep);
	//							가축을 사육하고 흐른 시간 얻기
	WORD						GetNextStepTime()				{ return m_nNextStepTime; }
	//							가축을 사육하고 흐른 시간 저장
	VOID						SetNextStepTime(WORD nTime)		{ m_nNextStepTime = nTime; }

	//----------------------------------------------------------------------------------------------------------------
	//							가축 최대 생명력 얻기
	WORD						GetMaxLife();
	//							사료 사용 생명력 얻기 (테이블에서)
	WORD						GetFeedLifeFromTbl(int nGrade);

	WORD						GetFeedContentmentFromTbl(int nGrade);
	WORD						GetFeedInterestFromTbl(int nGrade);
	
	//----------------------------------------------------------------------------------------------------------------
	//							변화 전 생명력 설정
	WORD						SetBeforeLife() { m_nBeforeLife = m_nLife; }
	//							변화 전 생명력 얻기
	WORD						GetBeforeLife()	{ return m_nBeforeLife; }

	//----------------------------------------------------------------------------------------------------------------
	//							죽기 전 딜레이타임 설정
//	VOID						SetDieDelayTimeTick(DWORD nTick) { m_nDieDelayTimeTick = nTick; }

	WORD						GetGrade();

	//----------------------------------------------------------------------------------------------------------------
	//							메인 루틴 처리
	VOID						MainLoop();	
};
