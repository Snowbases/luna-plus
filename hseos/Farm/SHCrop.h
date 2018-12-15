/*********************************************************************

	 파일		: SHCrop.h
	 작성자		: hseos
	 작성일		: 2007/04/10

	 파일설명	: 농작물 클래스의 헤더

 *********************************************************************/

#pragma once

#include "SHFarmObj.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
class CSHCrop : public CSHFarmObj
{
public:
	//----------------------------------------------------------------------------------------------------------------
	enum CROP_RESULT															// 농장 텃밭 관리 결과
	{
		CROP_RESULT_LIFE,														// ..생명력 변화
		CROP_RESULT_DIE,														// ..죽음
		CROP_RESULT_STEP_UP,													// ..단계 상승
		CROP_RESULT_STEP_UP_COMPLETE,											// ..단계 상승 완료
		CROP_RESULT_NEXT_STEP_TIME,												// ..다음 단계 시간
		CROP_RESULT_PLANT,														// ..심기
		CROP_RESULT_MANURE,														// ..비료주기
		CROP_RESULT_HARVEST,													// ..수확
		CROP_RESULT_MAX,														// ..끝
	} ;

	//----------------------------------------------------------------------------------------------------------------
	enum CROP_STEP																// 농작물 단계 정의											
	{
		CROP_STEP_EMPTY = 0,													// ..없음
		CROP_STEP_SEEDING,														// ..심는 중
		CROP_STEP_1,															// ..1단계
		CROP_STEP_2,															// ..2단계
		CROP_STEP_3,															// ..3단계
		CROP_STEP_COMPLETE,														// ..완료
		CROP_STEP_MAX = CROP_STEP_COMPLETE-CROP_STEP_SEEDING,
	} ;

	//----------------------------------------------------------------------------------------------------------------
	enum CROP_MANURE_KIND														// 비료 종류 정의											
	{
		CROP_MANURE_KIND_GRADE1 = 1,											// ..저급
		CROP_MANURE_KIND_GRADE2,												// ..중급
		CROP_MANURE_KIND_GRADE3,												// ..고급
		CROP_MANURE_KIND_MAX	= CROP_MANURE_KIND_GRADE3,
	};

	//----------------------------------------------------------------------------------------------------------------
	enum CROP_SEED_GRADE														// 씨앗 등급 정의
	{
		CROP_SEED_GRADE_1 = 1,
		CROP_SEED_GRADE_2,
		CROP_SEED_GRADE_3,
		CROP_SEED_GRADE_4,
		CROP_SEED_GRADE_5,
		CROP_SEED_GRADE_6,
		CROP_SEED_GRADE_7,
		CROP_SEED_GRADE_8,
		CROP_SEED_GRADE_9,
		CROP_SEED_GRADE_10,
		CROP_SEED_GRADE_11,
		CROP_SEED_GRADE_MAX = CROP_SEED_GRADE_11,
	} ;

	//----------------------------------------------------------------------------------------------------------------
	enum CROP_HARVEST_KIND														// 농작물 수확 종류
	{								
		CROP_HARVEST_KIND_GREAT = 0,											// ..대풍
		CROP_HARVEST_KIND_GOOD,													// ..풍작
		CROP_HARVEST_KIND_NORMAL,												// ..평작
		CROP_HARVEST_KIND_BAD,													// ..흉작
		CROP_HARVEST_KIND_WORST,												// ..대흉
		CROP_HARVEST_KIND_MAX,
	};

	//----------------------------------------------------------------------------------------------------------------
	static DWORD	CROP_LIFE_DECREASE_TIME; 												// 농작물 생명력 -1 시간
	static DWORD	CROP_PLANT_SEEDING_TIME;												// 농작물을 심고 싹이 나오는데 걸리는 시간
	static DWORD	CROP_PLANT_RETRY_TIME;													// 농작물을 다시 심는 데 필요한 시간 (분)
	static DWORD	CROP_MANURE_TIME;														// 농작물에 비료를 주는 시간
	static DWORD	CROP_MANURE_RETRY_TIME;													// 농작물에 다시 비료를 주는 데 필요한 시간 (분)
	static DWORD	CROP_HARVEST_TIME;														// 농작물의 수확 요청 후 수확하는데 걸리는 시간
	static DWORD	CROP_DIE_DELAY_TIME;													// 생명력이 0이 된 후 죽기까지의 딜레이 시간 (분)

	static UINT		CROP_NEXT_STEP_TIME[CROP_SEED_GRADE_MAX][CROP_STEP_MAX-1];				// 다음 단계가 되기 위해 필요한 시간 (분)
	static UINT		CROP_STEP_MIN_LIFE[CROP_SEED_GRADE_MAX][CROP_STEP_MAX-1];				// 단계에 따른 최소/최대 생명력
	static UINT		CROP_STEP_MAX_LIFE[CROP_SEED_GRADE_MAX][CROP_STEP_MAX-1];				// 단계에 따른 최소/최대 생명력
	static UINT		CROP_STEP_MANURE_MIN_LIFE[CROP_SEED_GRADE_MAX][CROP_MANURE_KIND_MAX];	// 비료에 따른 증가 생명력
	static UINT		CROP_STEP_MANURE_MAX_LIFE[CROP_SEED_GRADE_MAX][CROP_MANURE_KIND_MAX];	// 비료에 따른 증가 생명력

	static UINT		CROP_HARVEST_PROBA[5][CROP_HARVEST_KIND_MAX+2];							// 농작물 생명력에 따른 수확 확률
	static UINT		CROP_HARVEST_DELTA_PROBA[CROP_SEED_GRADE_MAX][CROP_HARVEST_KIND_MAX];	// 텃밭 등급에 따른 수확 증감 확률

	//----------------------------------------------------------------------------------------------------------------
	struct PACKET_FARM_GARDEN_CROP : public MSGBASE								// 텃밭 패킷 구조
	{
		int					nFarmZone;											// ..농장 지역
		int					nFarmID;											// ..농장 인덱스
		int					nGardenID;											// ..텃밭 인덱스
		int					nCropID;											// ..농작물 인덱스
		int					nResult;											// ..결과 상태(서버에서 처리 후 결과를 설정해서 클라이언트에 전송)
	};

	//----------------------------------------------------------------------------------------------------------------
	struct stResult																// 클라이언트에 알려야 할 농작물 결과 구조
	{
		CROP_RESULT eResult[256];												// ..결과 종류. 최대 256개
		int nResultNum;															// ..결과 개수						
	};

private:
	//----------------------------------------------------------------------------------------------------------------
	int						m_nKind;											// ..종류(ex: 화염초, 월광초..)
	int						m_nSeedGrade;										// ..씨앗 등급
	CROP_STEP				m_eStep;											// ..단계
	DWORD					m_nPlantTimeTick;									// ..심은 시간 틱
	DWORD					m_nManureTimeTick;									// ..비료 준 시간 틱
	WORD					m_nNextStepTime;									// ..심고 흐른 시간 (분)
	DWORD					m_nNextStepTimeTick;								// ..다음 단계 필요 시간 틱
	WORD					m_nBeforeLife;										// ..어떤 행위로 인해 생명력이 변화되기 전의 생명력
	DWORD					m_nDieDelayTimeTick;								// ..생명력이 1 이 되었을 때 죽기까지의 딜레이 시간 틱
	DWORD					m_nDieDelayTime;									// ..생명력이 1 이 되었을 때 죽기까지의 딜레이 시간 (분)

	int						m_nManureFreq[CROP_MANURE_KIND_MAX];				// ..비료를 사용한 횟수

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHCrop();
	// 090629 LUJ, 파생 클래스의 소멸자가 호출되도록 수정
	virtual ~CSHCrop();
	//							농작물 종류 설정
	VOID						SetKind(int nKind)			{ m_nKind = nKind; }
	//							농작물 종류 얻기
	int							GetKind()					{ return m_nKind; }

	//----------------------------------------------------------------------------------------------------------------
	//							농작물 단계 설정
	VOID						SetStep(CROP_STEP eStep)	{ m_eStep = eStep; }
	//							농작물 단계 얻기
	CROP_STEP					GetStep()					{ return m_eStep; }

	//----------------------------------------------------------------------------------------------------------------
	//							농작물 심은 시간 설정
	VOID						SetPlantTimeTick(DWORD nTick) { m_nPlantTimeTick = nTick; }
	//							농작물 심은 시간 얻기
	DWORD						GetPlantTimeTick()			{ return m_nPlantTimeTick; }

	//----------------------------------------------------------------------------------------------------------------
	//							비료 준 시간 설정
	VOID						SetManureTimeTick(DWORD nTick) { m_nManureTimeTick = nTick; }
	//							비료 준 시간 얻기
	DWORD						GetManureTimeTick()			{ return m_nManureTimeTick; }

	//----------------------------------------------------------------------------------------------------------------
	//							농작물 설정
	VOID						SetCrop(CROP_RESULT eResult); 

	//----------------------------------------------------------------------------------------------------------------
	//							농작물 씨앗 등급 설정
	VOID						SetSeedGrade(int nGrade)	{ m_nSeedGrade = nGrade; } 
	WORD						GetSeedGrade()				{ return static_cast<WORD>(m_nSeedGrade); } 
	int							GetManureFreq(CROP_MANURE_KIND eManureKind)		{ return m_nManureFreq[eManureKind-1]; }
	//----------------------------------------------------------------------------------------------------------------
	//							농작물 심기
	VOID						Plant(int nPlayerID, int nSeedGrade, int nKind);
	//							비료 주기
	VOID						Manure(int nManureGrade, int nManureKind);
	//							수확하기
	CROP_HARVEST_KIND			Harvest();

	//----------------------------------------------------------------------------------------------------------------
	//							농작물 다음 단계 필요 시간 얻기 (테이블에서)
	WORD						GetNextStepTimeTickFromTbl(int nSeedGrade, CROP_STEP eCurStep);
	//							농작물 심고 흐른 시간 얻기
	WORD						GetNextStepTime()				{ return m_nNextStepTime; }
	//							농작물 심고 흐른 시간 설정
	VOID						SetNextStepTime(WORD nTime)		{ m_nNextStepTime = nTime; }

	//----------------------------------------------------------------------------------------------------------------
	//							농작물 생명력 얻기 (테이블에서)
	WORD						GetLifeFromTbl(int nSeedGrade, CROP_STEP eCurStep);
	//							농작물 최대 생명력 얻기 (테이블에서)
	WORD						GetMaxLifeFromTbl(int nSeedGrade, CROP_STEP eCurStep);
	//							농작물 최대 생명력 얻기
	WORD						GetMaxLife()				{ return GetMaxLifeFromTbl(m_nSeedGrade, m_eStep); }
	//							비료 사용 생명력 얻기 (테이블에서)
	WORD						GetManureLifeFromTbl(int nGrade, int nKind);
	
	//----------------------------------------------------------------------------------------------------------------
	//							변화 전 생명력 설정
	WORD						SetBeforeLife() { m_nBeforeLife = m_nLife; }
	//							변화 전 생명력 얻기
	WORD						GetBeforeLife()	{ return m_nBeforeLife; }

	//----------------------------------------------------------------------------------------------------------------
	//							죽기 전 딜레이타임 설정
	VOID						SetDieDelayTimeTick(DWORD nTick) { m_nDieDelayTimeTick = nTick; }

	//----------------------------------------------------------------------------------------------------------------
	//							메인 루틴 처리
	VOID						MainLoop();																
};
