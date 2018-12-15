/*********************************************************************

	 파일		: SHFarmManager.h
	 작성자		: hseos
	 작성일		: 2007/04/10

	 파일설명	: 농장 매니저 클래스의 헤더

 *********************************************************************/

#pragma once

class CPlayer;
class CSHFamily;

#include "SHFarmZone.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//
class CSHFarmManager
{
public:
	//----------------------------------------------------------------------------------------------------------------
	enum FARM_HARVEST_KIND																		// 농작물 수확 종류
	{								
		FARM_HARVEST_KIND_GREAT = 0,															// ..대풍
		FARM_HARVEST_KIND_GOOD,																	// ..풍작
		FARM_HARVEST_KIND_NORMAL,																// ..평작
		FARM_HARVEST_KIND_BAD,																	// ..흉작
		FARM_HARVEST_KIND_WORST,																// ..대흉
		FARM_HARVEST_KIND_MAX,
	};

	enum FARM_ANIMAL_KIND
	{
		FARM_ANIMAL_KIND_GOOD = 0,
		FARM_ANIMAL_KIND_NORMAL,
		FARM_ANIMAL_KIND_BAD,
		FARM_ANIMAL_KIND_MAX
	};

	//----------------------------------------------------------------------------------------------------------------
	enum FARM_TIMEDELAY_KIND																	// 타임 딜레이 종류
	{								
		FARM_TIMEDELAY_KIND_PLANT = 0,															// ..심기
		FARM_TIMEDELAY_KIND_MANURE,																// ..비료주기
		// 080430 KTH -- Animal Delay Add
		FARM_TIMEDELAY_KIND_BREED,
		FARM_TIMEDELAY_KIND_FEED,
		FARM_TIMEDELAY_KIND_CLEAN,
	};

	//----------------------------------------------------------------------------------------------------------------
	enum FARM_EFFECT_KIND																		// 이펙트 종류
	{								
		FARM_EFFECT_KIND_CROP_PLANT = 0,														// ..농작물 심기
		FARM_EFFECT_KIND_CROP_MANURE,															// ..농작물 비료주기
		FARM_EFFECT_KIND_CROP_STEPUP,															// ..농작물 성장
		FARM_EFFECT_KIND_CROP_HARVEST,															// ..농작물 수확
		FARM_EFFECT_KIND_CROP_DIE,																// ..농작물 소멸
		FARM_EFFECT_KIND_UPGRADE_GARDEN,														// ..업그레이드 텃밭
		FARM_EFFECT_KIND_UPGRADE_FENCE,															// ..업그레이드 울타리
		FARM_EFFECT_KIND_UPGRADE_ANIMALCAGE,													// ..업그레이드 축사
		FARM_EFFECT_KIND_UPGRADE_HOUSE,															// ..업그레이드 집
		FARM_EFFECT_KIND_UPGRADE_WAREHOUSE,														// ..업그레이드 창고
		// 080312 KTH --축사 관련
		FARM_EFFECT_KIND_ANIMAL_IN_STALL,															// ..가축 축사에 넣기
		FARM_EFFECT_KIND_ANIMAL_FEED,															// ..가축 사료주기
		FARM_EFFECT_KIND_CLEANNING,																// ..축사 청도 이펙트
		FARM_EFFECT_KIND_ANIMAL_REWARD,														// ..가축 아이템 얻기
		FARM_EFFECT_KIND_ANIMAL_DIE,															// ..가축 소멸
		FARM_EFFECT_KIND_MAX,		
	};

	//----------------------------------------------------------------------------------------------------------------
	enum FARM_DEL_KIND																			// 농장 삭제 종류
	{								
		FARM_DEL_KIND_BREAKUP_FAMILY = 0,														// ..패밀리 해체
		FARM_DEL_KIND_TAX,																		// ..관리비 미납
	};

	//----------------------------------------------------------------------------------------------------------------
	enum FARM_UI_NOTIFY_KIND																	// 농장 UI 알림 종류
	{								
		FARM_UI_NOTIFY_KIND_NORMAL = 0,															// ..일반적
		FARM_UI_NOTIFY_KIND_CONN_MAP,															// ..맵에 접속
	};

	//----------------------------------------------------------------------------------------------------------------
	static const int		CHECK_DISTANCE;														// 농장 처리 사항을 플레이어에게 적용할 거리 (농장 오브젝트는 그리드를 사용하지 않음)
	static const int		OBJECT_CONTROL_DISTANCE;											// 농장 오브젝트 조작 가능 거리

	static const int		RENDER_SALEBOARD_KIND;												// 화면 출력 농장판매 비석 종류 번호
	static const int		RENDER_STEWARD_KIND;												// 화면 출력 집사 종류 번호
	static const int		RENDER_FENCE_KIND;													// 화면 출력 울타리 종류 번호
	static const int		RENDER_ANIMALCAGE_KIND;												// 화면 출력 축사 종류 번호
	static const int		RENDER_HOUSE_KIND;													// 화면 출력 집 종류 번호
	static const int		RENDER_GARDENPATCH_KIND;											// 화면 출력 텃밭흙더미 종류 번호
	static const int		RENDER_WAREHOUSE_KIND;												// 화면 출력 창고 종류 번호
	static const int		RENDER_CROP_KIND;													// 화면 출력 농작물 종류 번호
	static const int		RENDER_BASIC_FENCE;													// 화면 출력 기본 울타리 번호
	static const int		RENDER_BASIC_GARDENPATCH_KIND;										// 화면 출력 기본 텃밭흙더미 종류 번호
	// 080307 KTH -- 가축 추가
	//------------------------------------------------------------------------------------------------------------------------------------------------
	/*enum ANIMAL_BREED_KIND																			// 축사 수확
	{
		ANIMAL_BREED_KIND_GREAT,																	// 아주 좋음
		ANIMAL_BREED_KIND_GOOD,																		// 좋음
		ANIMAL_BREED_KIND_NORMAL,																	// 중간
		ANIMAL_BREED_KIND_BAD,																		// 안좋음 (보상 없음)
		ANIMAL_BREED_KIND_MAX,
	};*/

	//static const int		RENDER_ANIMAL_CAGE_SLOT_KIND;										// 축사 슬롯
	static const int		RENDER_BASIC_ANIMAL_CAGE_SLOT_KIND;									// 축사 출력 기본 슬롯
	static const int		RENDER_ANIMAL_KIND;													// 가축의 종류 번호
	static UINT				FARM_BREED_CHAR_MOTION;												// 가축 축사에 넣는 캐릭터 모션 번호
	static UINT				FARM_FEED_CHAR_MOTION;												// 가축 사료주기 캐릭터 모션 번호
	static UINT				FARM_REWARD_ANIMAL_CHAR_MOTION;										// 가축 수확하기 캐릭터 모션 번호
	static UINT				FARM_BREED_ANIMAL_GRADE_DIFF;										// 가축을 축사에 넣기 불가능한 가축과 축사와의 등급차이
	static UINT				FARM_CLEANING_CHAR_MOTION;											// 가축을 씻길때 캐릭터 모션 번호

	static UINT				FARM_ANIMAL_MOTION_01;												// 가축 모션 1
	static UINT				FARM_ANIMAL_MOTION_02;												// 가축 모션 2

	static int				RENDER_ANIMAL_KIND_NUM;												// 가축 종류 개수
	static const int		PEN_OBJECT_CONTROL_DISTANCE;										// 축사 오브젝트 조작 가능 거리

	static UINT				FARM_ANIMAL_REWARD_BAD;												// 가축 보상 bad 구간 (0% ~ )
	static UINT				FARM_ANIMAL_REWARD_NORMAL;											// 가축 보상 normal 구간 (bad% ~ )
	//------------------------------------------------------------------------------------------------------------------------------------------------

	static int				RENDER_CROP_KIND_NUM;												// 농작물 종류 개수

	static UINT				FARM_BUY_MAX_NUM;													// 최대로 가질 수 있는 농장 개수
	static UINT				FARM_BUY_FAMILY_MEMNUM;												// 농장을 구입하기 위해 필요한 패밀리 최소 인원
	static UINT				FARM_BUY_MASTER_LEVEL;												// 농장 구입 금액
	static UINT				FARM_BUY_FAMILY_HONORPOINT;											// 농장 구입 패밀리 명성 포인트
	static UINT				FARM_BUY_MONEY;														// 농장 구입 패밀리 마스터 레벨

	static UINT				FARM_PLANT_GARDEN_GRADE_DIFF;										// 농작물을 심기 불가능한 농작물과 텃밭과의 등급 차이
	static UINT				FARM_TAX_PAY_WEEK_INTERVAL;											// 농장 관리비 징수 주일 간격
	static UINT				FARM_TAX_PAY_LIMIT_DAY;												// 농장 관리비 징수 기한 요일 (일:0  월:1  화:2  수:3  목:4  금:5  토:6)
	static UINT				FARM_TAX_PAY_LIMIT_HOUR;											// 농장 관리비 징수 기한 시간 (00 ~ 23)
	static UINT				FARM_TAX_PAY_ARREARAGE_MAX_FREQ;									// 농장 관리비 연체 가능 횟수
	static UINT				FARM_TAX_PAY_ARREARAGE_ADD_RATE;									// 농장 관리비 연체료 비율 (%)
	static UINT				FARM_TAX_PAY_NOTIFY_NONPAYMENT_DDAY;								// 농장 관리비 기한일 몇 일 전부터 미납을 알릴 것인가(-1:은 알리지 않음) 
	// 090707 LUJ, 세금으로 아이템을 지불할 수 있다. 키: 아이템 번호, 값: 수량

	struct Tax
	{
		LEVELTYPE mFarmLevel;
		MONEYTYPE mMoney;
		DWORD mItemSize;
		struct Item
		{
			DWORD mIndex;
			DURTYPE mQuantity;
		}
		mItem[ 10 ];

		Tax()
		{
			ZeroMemory( this, sizeof( *this ) );
		}
	};
	// 090710 LUJ, 키 값: 텃밭 레벨
	typedef CYHHashTable< Tax > TaxHashTable;
	TaxHashTable mTaxHashTable;
	static UINT				FARM_PLANT_CHAR_MOTION_NUM;											// 농작물 심기 캐릭터 모션 번호
	static UINT				FARM_MANURE_CHAR_MOTION_NUM;										// 농작물 비료주기 캐릭터 모션 번호
	static UINT				FARM_HARVEST_CHAR_MOTION_NUM;										// 농작물 수확하기 캐릭터 모션 번호
	static UINT				FARM_CHAR_MOTION_PROGRESSBAR_TYPE;									// 캐릭터 모션 프로그레스바 타입 번호
	static UINT				FARM_CHAR_MOTION_PROGRESSBAR_WIDTH;									// 캐릭터 모션 프로그레스바 너비

	static int				FARM_EFFECT_NUM[FARM_EFFECT_KIND_MAX];								// 농장 오브젝트 이펙트	

	//----------------------------------------------------------------------------------------------------------------
	struct PACKET_FARM_STATE : public MSGBASE													// 농장 구입 패킷 구조
	{
		WORD				nFarmZone;															// ..농장 지역
		WORD				nFarmID;															// ..농장 인덱스
		WORD				nFarmState;															// ..농장 상태
		DWORD				nFarmOwner;															// ..농장 소유자
		WORD				nGardenGrade;														// ..텃밭 등급
		WORD				nFenceGrade;														// ..울타리 등급
		WORD				nAnimalCageGrade;													// ..축사 등급
		WORD				nHouseGrade;														// ..집 등급
		WORD				nWarehouseGrade;													// ..창고 등급
		WORD				nResult;															// ..결과 상태(서버에서 처리 후 결과를 설정해서 클라이언트에 전송)
	};

	//----------------------------------------------------------------------------------------------------------------
	struct PACKET_FARM_STATE_WITH_TAX : public PACKET_FARM_STATE								// 농장 구입 패킷 구조 (관리비 추가)
	{
		WORD				nTaxArrearageFreq;													// ..관리비 연체 횟수
		char				szTaxPayPlayerName[MAX_NAME_LENGTH+1];								// ..관리비 납부 플레이어
	};

	//----------------------------------------------------------------------------------------------------------------
	struct PACKET_FARM_GARDEN_CROP_PLANT : public MSGBASE										// 농작물 심기 패킷 구조
	{
		WORD				nFarmZone;															// ..농장 지역
		WORD				nFarmID;															// ..농장 인덱스
		WORD				nGardenID;															// ..텃밭 인덱스
		WORD				nCropID;															// ..농작물 인덱스
		DWORD				nCropOwner;															// ..농작물 소유자
		WORD				nCropKind;															// ..농작물 종류
		WORD				nCropStep;															// ..농작물 단계
		WORD				nCropLife;															// ..농작물 수명
		WORD				nCropNextStepTime;													// ..농작물 다음 단계 필요 시간
		WORD				nCropSeedGrade;														// ..씨앗 등급
		WORD				nItemTargetPos;														// ..아이템 정보
		DWORD				nItemIdx;															// ..아이템 정보
		DWORD				nOwner;																// ..농장 소유자
		WORD				nResult;															// ..결과 상태(서버에서 처리 후 결과를 설정해서 클라이언트에 전송)
	};

	//----------------------------------------------------------------------------------------------------------------
	struct PACKET_FARM_GARDEN_CROP_GROW : public MSGBASE										// 농작물 성장 패킷 구조
	{
		WORD				nFarmZone;															// ..농장 지역
		WORD				nFarmID;															// ..농장 인덱스
		WORD				nGardenID;															// ..텃밭 인덱스
		WORD				nCropID;															// ..농작물 인덱스
		WORD				nValue;																// ..수치값
		WORD				nItemTargetPos;														// ..아이템 정보
		DWORD				nItemIdx;															// ..아이템 정보
		WORD				nResult;															// ..결과 상태(서버에서 처리 후 결과를 설정해서 클라이언트에 전송)
	};

	//----------------------------------------------------------------------------------------------------------------
	struct PACKET_FARM_GARDEN_CROP_MANURE : public MSGBASE										// 농작물 비료주기 패킷 구조
	{
		WORD				nFarmZone;															// ..농장 지역
		WORD				nFarmID;															// ..농장 인덱스
		WORD				nGardenID;															// ..텃밭 인덱스
		WORD				nCropID;															// ..농작물 인덱스
		WORD				nCropLife;															// ..농작물 수명
		WORD				nManureKind;														// ..비료 종류
		WORD				nManureGrade;														// ..비료 등급
		WORD				nItemTargetPos;														// ..아이템 정보
		DWORD				nItemIdx;															// ..아이템 정보
		WORD				nResult;															// ..결과 상태(서버에서 처리 후 결과를 설정해서 클라이언트에 전송)
	};
    // 090707 LUJ, 세금 납부 요청
	struct PACKET_FARM_TAX_SYN: public MSG_DWORD4
	{
		DWORD mSize;
		ICONBASE mItem[ SLOT_INVENTORY_NUM ];

		DWORD GetSize() const
		{
			return sizeof( *this ) - ( sizeof( mItem ) / sizeof( *mItem ) - mSize ) * sizeof( *mItem );
		}
	};
	// 090707 LUJ, 세금 납부 결과
	struct PACKET_FARM_TAX_ACK : public MSG_NAME_DWORD3
	{
		MONEYTYPE mTaxMoney;
		DWORD mSize;
		ITEMBASE mItem[ SLOT_INVENTORY_NUM ];

		DWORD GetSize() const
		{
			return sizeof( *this ) - ( sizeof( mItem ) / sizeof( *mItem ) - mSize ) * sizeof( *mItem );
		}
	};

	//----------------------------------------------------------------------------------------------------------------
	struct PACKET_FARM_UPGRADE : public MSGBASE													// 농장 등급 업그레이드 패킷 구조
	{
		WORD				nFarmZone;															// ..농장 지역
		WORD				nFarmID;															// ..농장 인덱스
		WORD				nObjKind;															// ..업그레이드 대상
		WORD				nResult;															// ..결과 상태(서버에서 처리 후 결과를 설정해서 클라이언트에 전송)
	};

	//----------------------------------------------------------------------------------------------------------------
	struct stBFARM																				// 농장 스크립트 정보
	{
		WORD nMapNum;																			// ..맵 번호
		WORD nKind;																				// ..종류
		char szName[MAX_NAME_LENGTH+1];															// ..이름
		WORD nID;																				// ..ID
	};

	//----------------------------------------------------------------------------------------------------------------
	struct stFARMRENDEROBJLIST																	// 농장 렌더링 오브젝트 리스트 정보
	{
		int		nID;																			// ..ID
		char	szGroupName[MAX_NPC_NAME_LENGTH+1];												// ..그룹이름
		char	szChxName[32];																	// ..chx 파일 이름
		float	nScale;																			// ..스케일
		int		nNameYpos;																		// ..그룹이름 높이
		BOOL	bShowGroupName;																	// ..그룹이름 표시 여부
	};

	//----------------------------------------------------------------------------------------------------------------
	struct stFARMOBJNAME																		// 농장 렌더링 오브젝트 이름 정보
	{
		char szName[64];
	};

	//----------------------------------------------------------------------------------------------------------------
	struct stSelFarmObj																			// 조작할 농작물 인덱스
	{
		int					nFarmZone;															// ..농장지역
		int					nFarmID;															// ..농장
		int					nGardenID;															// ..텃밭
		int					nCropID;															// ..농작물
		// 080408 KTH
		int					nAnimalCageID;														// ..축사
		int					nAnimalID;															// ..동물
	};

	//----------------------------------------------------------------------------------------------------------------
	struct stCROPHARVESTREWARD																	// 농작물 수확 보상
	{
		DWORD				nItemID[FARM_HARVEST_KIND_MAX];										// ..아이템 ID
		DWORD				nItemNum[FARM_HARVEST_KIND_MAX];									// ..아이템 개수
	};

	struct stANIMALREWARD
	{
		DWORD				nItemID[FARM_ANIMAL_KIND_MAX*3];
		DWORD				nItemNum[FARM_ANIMAL_KIND_MAX*3];
		DWORD				nPercent[FARM_ANIMAL_KIND_MAX*3];
	};

	//----------------------------------------------------------------------------------------------------------------
	struct stUPGRADECOST																		// 업그레이드 비용
	{
		UINT				nGold;																// ..골드
		UINT				nFamilyHonorPoint;													// ..패밀리 명성 포인트
	};

	//----------------------------------------------------------------------------------------------------------------
	struct PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT : public MSGBASE								// 가축 넣기 패킷 구조
	{
		WORD				nFarmZone;															// ..농장 지역
		WORD				nFarmID;															// ..농장 인덱스
		WORD				nAnimalCageID;														// ..축사 인덱스
		WORD				nAnimalID;															// ..가축 인덱스
		DWORD				nAnimalOwner;														// ..농작물 소유자
		WORD				nAnimalKind;														// ..농작물 종류
		WORD				nAnimalStep;														// ..농작물 단계
		WORD				nAnimalLife;														// ..농작물 수명
		WORD				nAnimalNextStepTime;												// ..농작물 다음 단계 필요 시간
		WORD				nAnimalGrade;														// ..가축 등급
		WORD				nItemTargetPos;														// ..아이템 정보
		DWORD				nItemIdx;															// ..아이템 정보
		DWORD				nOwner;																// ..농장 소유자
		WORD				nResult;															// ..결과 상태(서버에서 처리 후 결과를 설정해서 클라이언트에 전송)
		WORD				nAnimalContentment;													// ..
		WORD				nAnimalInterest;
	};

	//----------------------------------------------------------------------------------------------------------------
	struct PACKET_FARM_CAGE_ANIMAL_GROW : public MSGBASE										// 가축 성장 패킷 구조
	{
		WORD				nFarmZone;															// ..농장 지역
		WORD				nFarmID;															// ..농장 인덱스
		WORD				nAnimalCageID;															// ..축사 인덱스
		WORD				nAnimalID;															// ..가축 인덱스
		WORD				nValue;																// ..수치값
		WORD				nItemTargetPos;														// ..아이템 정보
		DWORD				nItemIdx;															// ..아이템 정보
		WORD				nResult;															// ..결과 상태(서버에서 처리 후 결과를 설정해서 클라이언트에 전송)
	};

	struct PACKET_FARM_CAGE_ANIMAL_FEED : public MSGBASE										// 가축 사료주기 패킷 구조
	{
		WORD				nFarmZone;															// ..농장 지역
		WORD				nFarmID;															// ..농장 인덱스
		WORD				nAnimalCageID;														// ..축사 인덱스
		WORD				nAnimalID;															// ..가축 인덱스
		WORD				nAnimalLife;														// ..가축 수명
		WORD				nAnimalContentment;													// ..가축 만족도
		WORD				nAnimalInterest;													// ..가축 관심도
		WORD				nFoodKind;															// ..비료 종류
		WORD				nItemTargetPos;														// ..아이템 정보
		DWORD				nItemIdx;															// ..아이템 정보
		WORD				nResult;															// ..결과 상태(서버에서 처리 후 결과를 설정해서 클라이언트에 전송)
	};

	struct PACKET_FARM_CAGE_ANIMAL_REWARD : public MSGBASE
	{
		WORD				nFarmZone;
		WORD				nFarmID;
		WORD				nAnimalCageID;
		WORD				nAnimalID;
		WORD				nResult;
	};

	// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 추가.
	enum ePayTaxMode
	{
		ePayTaxMode_FarmNpc = 0,
		ePayTaxMode_PayBtn
	};

	struct stFARMTAXSTATE
	{
		WORD				nFarmZone;															// ..농장 지역
		WORD				nFarmID;															// ..농장 인덱스
		WORD				nFarmState;															// ..농장 상태
		DWORD				nFarmOwner;															// ..농장 소유자
		WORD				nGardenGrade;														// ..텃밭 등급
		WORD				nFenceGrade;														// ..울타리 등급
		WORD				nAnimalCageGrade;													// ..축사 등급
		WORD				nHouseGrade;														// ..집 등급
		WORD				nWarehouseGrade;													// ..창고 등급
		WORD				nTaxArrearageFreq;													// ..관리비 연체 횟수
		char				szTaxPayPlayerName[MAX_NAME_LENGTH+1];								// ..관리비 납부 플레이어

	};

	struct stBuyFarmWaitInfo
	{
		DWORD				dwPlayerIdx;														// 요청한 플레이어 인덱스
		WORD				nFarmZone;															// ..농장 지역
		WORD				nFarmID;															// ..농장 인덱스
		DWORD				dwRequestTime;														// 요청 시간 
		DWORD				dwRequestCount;														// 요청 횟수
		BOOL				bQueryToDB;															// 쿼리중인가?
	};


private:
	// 090629 LUJ, 농장 자료 구조 개선
	CYHHashTable< CSHFarmZone > m_pcsFarmZone;
	CSHFarmRenderObj*		m_pcsSelFarmObj;													// 조작하려고 선택한 농장 오브젝트
	stSelFarmObj			m_stSelFarmObj;

	stFARMOBJNAME*			m_pszTblFarmCropName;														// 농작물 이름 테이블
	stCROPHARVESTREWARD*	m_pstCropHarvestReward;														// 농작물 수확 보상

	stBFARM*				m_pstBFarm;																	// 농장 스크립트 정보
	stFARMRENDEROBJLIST*	m_pstFarmRenderObjList;														// 농장 렌더링 오브젝트 리스트

	stUPGRADECOST			m_stUpgradeCost[CSHFarm::FARM_EVENT_MAX][CSHFarmObj::MAX_GRADE-1];			// 업그레이드 비용

	DWORD					m_nMotionID;																// 캐릭터 모션ID

	UINT					m_nTaxCollectPastWeek;														// 관리비를 징수하고 흐른 일주일의 횟수

	// 080410 KTH
	stFARMOBJNAME*			m_pszTblFarmAnimalName;														// 가축 이름 테이블
	stANIMALREWARD*			m_pstAnimalReward;															// 가축 보상
	
	// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 추가.
	ePayTaxMode				m_ePayTaxMode;
	stFARMTAXSTATE			m_stFarmTaxState;			

	// 091126 농장 지역추가 관련 농장구입 프로세스 변경
	CYHHashTable< stBuyFarmWaitInfo > m_BuyFarm_WaitInfoList;											// 농장 구입 대기자 리스트

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHFarmManager();
	~CSHFarmManager();


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 공용
	//
	//----------------------------------------------------------------------------------------------------------------
	//						메인 루프
	VOID					MainLoop();														

	//						초기화
	VOID					Init(int nMapNum);
	//						해제
	VOID					Release();
	// 090520 LUJ, 맵 번호 추가
	BOOL					LoadFarmInfo( MAPTYPE, int nType);
	//						농장 지역 번호 얻기
	BOOL					GetFarmZoneID(int nMapNum, int* pZone);
	//						농장 지역 얻기
	CSHFarmZone*			GetFarmZone(WORD eFarmZone) { return m_pcsFarmZone.GetData( eFarmZone ); }
	//						농장 맵 번호 얻기
	BOOL					GetFarmMapNum(UINT nFarmZone, int* pMapNum);
	//						렌더링 오브젝트 종류 얻기
	int						GetRenderObjKind(int nID);
	//						농작물 이름 얻기
	char*					GetRenderObjNameFromTbl(int nKind, int nStep)	{ return m_pszTblFarmCropName[CSHCrop::CROP_STEP_MAX*(nKind-1)+nStep-CSHCrop::CROP_STEP_1].szName; }
	//						업그레이드 비용 얻기
	stUPGRADECOST*			GetUpgradeCost(CSHFarm::FARM_EVENT eObjKind, int nGrade)	{ return &m_stUpgradeCost[eObjKind-CSHFarm::FARM_EVENT_GARDEN][nGrade-1]; }
	// 090710 LUJ, 세금 정보를 반환한다
	const Tax&				GetTax( DWORD gardenLevel );
	ePayTaxMode				GetPayTaxMode() { return m_ePayTaxMode; }
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 서버
	//
	//----------------------------------------------------------------------------------------------------------------
	//	에이전트
	//						클라이언트 요청 분석/처리
	VOID					ASRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength);
	VOID					ASRV_ParseRequestFromServer(DWORD dwConnectionID, char* pMsg, DWORD dwLength);
	//						맵서버에 농장UI정보 요청
	VOID					ASRV_RequestFarmUIInfoToMap(DWORD nPlayerID, CSHFamily* pcsFamily);

	//----------------------------------------------------------------------------------------------------------------
	//	맵
	//						클라이언트 요청 분석/처리
	VOID					SRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength);

	//						주변의 다른 플레이어들에게 농장 데이터 보내기
	VOID					SRV_SendFarmInfoToNearPlayer(CSHFarm* pcsFarm, MSGBASE* pPacket, int nSize);
	//						이벤트 처리
	VOID					SRV_ProcessEvent();

	VOID					SRV_ProcessFarmBuyWaitList();

	// 091126 pdy DB에 보유한 농장 갯수 결과시 호출되는 함수
	VOID					SRV_OnCheckHaveFarmNumToDB(DWORD dwPlayerIdx , DWORD ResultNum );

	//----------------------------------------------------------------------------------------------------------------
	//						농장 구입
	VOID					SRV_BuyFarm(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, int nAgentChecked = 0);
	//						농작물 심기
	VOID					SRV_PlantCrop		(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nGardenID, WORD nCropID, WORD nItemTargetPos, DWORD nItemIdx, int nAgentChecked = 0);
	//						농작물 비료 주기
	VOID					SRV_ManureCrop		(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nGardenID, WORD nCropID, WORD nItemTargetPos, DWORD nItemIdx);
	//						농작물 수확하기
	VOID					SRV_HarvestCrop		(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nGardenID, WORD nCropID, WORD nItemTargetPos, DWORD nItemIdx, int nType);
	//						농장 등급 업그레이드
	VOID					SRV_UpgradeFarm		(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nObjKind, int nAgentChecked = 0);
	//						농장 관리비 납부
	// 090707 LUJ, 아이템도 납부할 수 있도록 함
	VOID					SRV_PayTax( CPlayer*, const PACKET_FARM_TAX_SYN& );
	// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 추가.
	VOID					SRV_PayTaxFromFamilyDialog( CPlayer*, const PACKET_FARM_TAX_SYN& );
	VOID					SRV_SendTaxToClient(CPlayer* pPlayer, stFARMTAXSTATE* stFarmTaxState);

	//						농장 관리비 징수 처리
	VOID					SRV_ProcTaxCollect	();
	//						농장 관리비 미납 알림
	VOID					SRV_NotifyTaxNonpayment(DWORD nPlayerID, CSHFarm* pcsFarm);

	//						농작물 삭제(패밀리 탈퇴 등의..)
	VOID					SRV_DelCrop(DWORD nOwnerID);
	//						농장 삭제
	VOID					SRV_DelFarm(DWORD nOwnerID, FARM_DEL_KIND eDelKind);
	//						농장 삭제
	VOID					SRV_DelFarmByFarmID(DWORD nOwnerID, WORD nFarmZone, WORD nFarmID, FARM_DEL_KIND eDelKind);

	//----------------------------------------------------------------------------------------------------------------
	//						DB에 농장 데이터 저장
	VOID					SRV_SaveFarmInfoToDB(WORD nFarmZone, WORD nFarmID, CSHFarm* pcsFarm);
	//						DB에 농작물 데이터 저장
	VOID					SRV_SaveCropInfoToDB(WORD nFarmZone, WORD nFarmID, CSHCrop* pCrop);
	//						DB 큐를 참조하여 농장 상태 설정
	VOID					SRV_SetFarmStateFromDBQueue(CPlayer* pPlayer, PACKET_FARM_STATE_WITH_TAX* pPacket);
	//						DB 큐를 참조하여 농작물 설정
	VOID					SRV_SetCropInfoFromDBQueue(CPlayer* pPlayer, PACKET_FARM_GARDEN_CROP_PLANT* pPacket);

	//----------------------------------------------------------------------------------------------------------------
	//						클라이언트에게 농장 정보 보내기
	VOID					SRV_SendFarmInfoToClient(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID);
	//						클라이언트에 농장UI정보 전송
	VOID					SRV_SendFarmUIInfoToClient(DWORD nPlayerID, DWORD nFamilyMasterID, FARM_UI_NOTIFY_KIND eKind = FARM_UI_NOTIFY_KIND_NORMAL);
	//						클라이언트에 농장UI 농작물 정보 전송
	VOID					SRV_SendFarmUIInfoToClient(WORD nFarmZone, WORD nFarmID, WORD nGardenID, CSHCrop* pCrop, DWORD nFarmOwner);
	//						클라이언트의 맵접속 시 농장 정보 전송
	VOID					SRV_SendPlayerFarmInfo(CPlayer* pPlayer);

	//						DB에 가축 데이터 저장
	VOID					SRV_SaveAnimalInfoToDB(WORD nFarmZone, WORD nFarmID, CAnimal* pAnimal);
	//						DB 큐를 참조하여 가축 설정
	VOID					SRV_SetAnimalInfoFromDBQueue(CPlayer* pPlayer, PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT* pPacket);
	//						가축 삭제
	VOID					SRV_DelAnimal(DWORD nOwnerID);
	
	// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 추가.
	RESULT					SRV_PayItem( CPlayer* pPlayer, const PACKET_FARM_TAX_SYN& packet, const Tax& tax, MONEYTYPE requestedTaxMoney );
	VOID					SRV_SetFarmTaxInfo(DWORD dwFarmZone, DWORD dwFarmID, DWORD dwTaxArrearageFreq, char* szName);
	// 091126 ONS 패밀리장 이양 기능 추가
	VOID					SRV_SetFarmChangeOwner( DWORD dwFarmZone, DWORD dwFarmID, DWORD dwOwnerID );
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 클라이언트
	//
	//----------------------------------------------------------------------------------------------------------------
	//
	//						렌더링 오브젝트 리스트 얻기
	stFARMRENDEROBJLIST*	CLI_GetFarmRenderObjList(int nID);

	//						조작할 농작물 선택
	int						CLI_SetTargetObj(CSHFarmRenderObj* pFarmRenderObj, BOOL bShowDlg = TRUE);
	CSHFarmRenderObj*		CLI_GetTargetObj()		{ return m_pcsSelFarmObj; }
	stSelFarmObj*			CLI_GetTargetObjID()	{ return &m_stSelFarmObj; }
	//						화면 출력을 위한 농장 오브젝트 설정
	VOID					CLI_SetFarmObjForRender(CSHFarmRenderObj* pFarmRenderObj);

	//						화면에 렌더링 오브젝트 추가
	VOID					CLI_AddRenderFarmObjToScreen(CSHFarm* pFarm, int nTarget = 0);
	//						화면에서 렌더링 오브젝트 제거
	VOID					CLI_RemoveRenderFarmObjFromScreen(CSHFarm* pFarm, int nTarget = 0);
	//						화면에 렌더링 농작물 오브젝트 추가
	VOID					CLI_AddRenderFarmCropObjToScreen(CSHCrop* pCrop);
	//						화면에서 렌더링 농작물 오브젝트 제거
	VOID					CLI_RemoveRenderFarmCropObjToScreen(CSHCrop* pCrop);

	// 080417 KTH -- 화면에 랜더링 가축 오브젝트 추가	------------------------------
	VOID					CLI_AddRenderFarmAnimalObjToScreen(CAnimal* pAnimal);
	// 080417 KTH -- 화면에 랜더링 가축 오브젝트 제거
	VOID					CLI_RemoveRenderFarmAnimalObjToScreen(CAnimal* pAnimal);
	//--------------------------------------------------------------------------------

	//						농장 업그레이드 대화 창 처리
	BOOL					CLI_ProcUpgradeDlg(int nObjKind);
	//						농장 관리비 납부 대화 창
	BOOL					CLI_ProcPayTaxDlg();
	// 090710 LUJ, 세금으로 납부할 정보를 반환한다
	LPCTSTR					CLI_GetTaxText( MONEYTYPE );
	//						캐릭터 모션 중 설정
	VOID					CLI_SetIngCharMotion(DWORD nMotionID)	{ m_nMotionID = nMotionID; }
	//						캐릭터 모션 중 얻기
	DWORD					CLI_GetIngCharMotion()					{ return m_nMotionID; }
	
	//						채널 변환하기
	VOID					CLI_ChangeChannel(BOOL bLogin);

	//----------------------------------------------------------------------------------------------------------------
	//						서버에 농장 구입 요청
	VOID					CLI_RequestBuyFarmToSrv();
	VOID					CLI_RequestBuyFarmToSrv(CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID);
	//						서버에 농작물 심기 요청
	VOID					CLI_RequestPlantCropInGarden(WORD nItemTargetPos, DWORD nItemIdx);
	//						서버에 농작물 비료주기 요청
	VOID					CLI_RequestManureCropInGarden(WORD nItemTargetPos, DWORD nItemIdx);
	//						서버에 농작물 수확하기 요청
	VOID					CLI_RequestHarvestropInGarden(WORD nItemTargetPos, DWORD nItemIdx, WORD nType = RESULT_OK);
	//						서버에 농장 오브젝트 업그레이드 요청
	VOID					CLI_RequestUpgradeFarm(CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, int nObjKind);
	//						서버에 관리비 납부 요청
	VOID					CLI_RequestPayTax(UINT nKind = 0);

	//						서버에 농장 정보 요청
	VOID					CLI_RequestFarmInfo(CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID);

	//----------------------------------------------------------------------------------------------------------------
	//						서버로부터의 응답 분석/처리
	VOID					CLI_ParseAnswerFromSrv(void* pMsg);

	//----------------------------------------------------------------------------------------------------------------
	//						서버에 가축 넣기 요청
	VOID					CLI_RequestInstallAnimalInAnimalCage(WORD nItemTargetPos, DWORD nItemIdx);
	//						가축 축사에 넣기
	VOID					SRV_InstallAnimalInAnimalCage(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nAnimalCageID, WORD nAnimalID, WORD nItemTargetPos, DWORD nItemIdx, WORD nContentment, WORD nInterest, int nAgentChecked = 0);
	//						서버에 가축 사료주기 요청
	VOID					CLI_RequestFeedAnimalInCage(WORD nItemTargetPos, DWORD nItemIdx);
	//						가축에 먹이주기
	VOID					SRV_FeedAnimal(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nAnimalCageID, WORD nAnimalID, WORD nItemTargetPos, DWORD nItemIdx);
	//						가축 청소 하기 (map server)
	VOID					SRV_CleaningAnimal(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nAnimalCageID, WORD nAnimalID, WORD nItemTargetPos, DWORD nItemIdx);
	//						가축 가져오기
	CAnimal*				CLI_GetAnimalInfo(CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nAnimalCageID, WORD nAnimalID);
	//						가축 청소 하기 (client)
	VOID					CLI_RequestCleaningAnimalInCage(WORD nItemTargetPos, DWORD nItemIdx);
	//						가축 보상 아이템 받기 (client)
	VOID					CLI_RequestAnimalGetItem(WORD nType = RESULT_OK);
	VOID					SRV_AnimalGetItem(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nAnimalCageID, WORD nAnimalID, WORD nType);
};

extern CSHFarmManager		g_csFarmManager;
