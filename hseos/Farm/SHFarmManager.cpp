/*********************************************************************

	 파일		: SHFarmManager.cpp
	 작성자		: hseos
	 작성일		: 2007/04/10

	 파일설명	: 농장 매니저 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "../SHMain.h"
#include "../Common/SHMath.h"
#include "SHFarmManager.h"

#include "MHFile.h"

#if defined(_AGENTSERVER)
	#include "Network.h"
	#include "AgentDBMsgParser.h"
	#include "AgentNetworkMsgParser.h"
	#include "../Family/SHFamilyManager.h"
#elif defined(_MAPSERVER_)
	#include "Network.h"
	#include "MapDBMsgParser.h"
	#include "UserTable.h"
	#include "Player.h"
	#include "PackedData.h"
	#include "ItemManager.h"
	#include "..\[CC]Header\GameResourceStruct.h"
	#include "MHTimeManager.h"
#else
	#include "ObjectManager.h"
	#include "GameIn.h"
	#include "MonsterGuageDlg.h"
	#include "ObjectBalloon.h"
	#include "OBalloonName.h"
	#include "ObjectGuagen.h"
	#include "ChatManager.h"
	#include "SHFarmBuyDlg.h"
	#include "MHMap.h"
	#include "NpcScriptDialog.h"
	#include "SHFarmUpgradeDlg.h"
	#include "SHFarmManageDlg.h"
	#include "../Monstermeter/SHMonstermeterDlg.h"
	#include "MiniMapDlg.h"
	#include "../Interface/cWindowManager.h"
	#include "../WindowIDEnum.h"	
	#include "../cMsgBox.h"
	#include "../Farm/SHFarmManageDlg.h"
	#include "MiniMapDlg.h"
	#include "BigMapDlg.h"
	#include "FarmAnimalDlg.h"
	#include "MoveManager.h"
	// 090707 LUJ, 아이템 세금 납부로 인해 추가
	#include "ItemManager.h"
	#include "interface/cResourceManager.h"
#endif

const int CSHFarmManager::CHECK_DISTANCE					= 10000;					// 농장 처리 사항을 플레이어에게 적용할 거리 (농장 오브젝트는 그리드를 사용하지 않음)
const int CSHFarmManager::OBJECT_CONTROL_DISTANCE			= 100;						// 농장 오브젝트 조작 가능 거리
const int CSHFarmManager::PEN_OBJECT_CONTROL_DISTANCE		= 600;						// 가축 오브젝트 조작 가능 거리

const int CSHFarmManager::RENDER_SALEBOARD_KIND				= 801;						// 화면 출력 농장판매 팻말 종류 번호. NpcList.bin 에서 정의
const int CSHFarmManager::RENDER_STEWARD_KIND				= 802;						// 화면 출력 집사 종류 번호
const int CSHFarmManager::RENDER_FENCE_KIND					= 101;						// 화면 출력 울타리 종류 번호
const int CSHFarmManager::RENDER_ANIMALCAGE_KIND			= 301;						// 화면 출력 축사 종류 번호
const int CSHFarmManager::RENDER_HOUSE_KIND					= 201;						// 화면 출력 집 종류 번호
const int CSHFarmManager::RENDER_GARDENPATCH_KIND			= 501;						// 화면 출력 텃밭흙더미 종류 번호
const int CSHFarmManager::RENDER_WAREHOUSE_KIND				= 401;						// 화면 출력 창고 종류 번호
const int CSHFarmManager::RENDER_CROP_KIND					= 901;						// 화면 출력 농작물 종류 번호. 
const int CSHFarmManager::RENDER_BASIC_GARDENPATCH_KIND		= 601;						// 화면 출력 기본 텃밭흙더미 종류 번호
const int CSHFarmManager::RENDER_BASIC_FENCE				= 701;						// 화면 출력 기본 울타리 종류 번호

// 080307 KTH -- 가축 추가
//-----------------------------------------------------------------------------------------------------------------------------------------------------
const int CSHFarmManager::RENDER_BASIC_ANIMAL_CAGE_SLOT_KIND	= 2001;						// 화면 출력 기본 가축 슬롯 번호
//const int CSHFarmManager::RENDER_ANIMAL_CAGE_SLOT_KIND			= 2001;						// 화면 출력 가축 슬롯 번호
const int CSHFarmManager::RENDER_ANIMAL_KIND					= 2501;						// 화면 출력 가축 번호
UINT	CSHFarmManager::FARM_BREED_CHAR_MOTION;												// 축사에 가축을 배치하는 캐릭터 모션 번호
UINT	CSHFarmManager::FARM_FEED_CHAR_MOTION;												// 가축 사료주기 캐릭터 모션 번호
UINT	CSHFarmManager::FARM_REWARD_ANIMAL_CHAR_MOTION;										// 축사에서 아이템 얻기 캐릭터 모션 번호
UINT	CSHFarmManager::FARM_BREED_ANIMAL_GRADE_DIFF;										// 축사와 가축의 등급차이
UINT	CSHFarmManager::FARM_CLEANING_CHAR_MOTION;											// 가축을 씻길때 캐릭터 모션 번호
int		CSHFarmManager::RENDER_ANIMAL_KIND_NUM					= 3;

UINT	CSHFarmManager::FARM_ANIMAL_MOTION_01					= 0;
UINT	CSHFarmManager::FARM_ANIMAL_MOTION_02					= 1;

UINT	CSHFarmManager::FARM_ANIMAL_REWARD_BAD;
UINT	CSHFarmManager::FARM_ANIMAL_REWARD_NORMAL;
//-----------------------------------------------------------------------------------------------------------------------------------------------------

int		CSHFarmManager::RENDER_CROP_KIND_NUM					= 0;						// 농작물 종류 개수

UINT	CSHFarmManager::FARM_BUY_MAX_NUM;												// 최대로 가질 수 있는 농장 개수
UINT	CSHFarmManager::FARM_BUY_FAMILY_MEMNUM;											// 농장을 구입하기 위해 필요한 패밀리 최소 인원
UINT	CSHFarmManager::FARM_BUY_MASTER_LEVEL;											// 농장 구입 금액
UINT	CSHFarmManager::FARM_BUY_FAMILY_HONORPOINT;										// 농장 구입 패밀리 명성 포인트
UINT	CSHFarmManager::FARM_BUY_MONEY;													// 농장 구입 패밀리 마스터 레벨
UINT	CSHFarmManager::FARM_TAX_PAY_WEEK_INTERVAL;										// 농장 관리비 징수 주일 간격
UINT	CSHFarmManager::FARM_TAX_PAY_LIMIT_DAY;											// 농장 관리비 징수 기한 요일 (일:0  월:1  화:2  수:3  목:4  금:5  토:6)
UINT	CSHFarmManager::FARM_TAX_PAY_LIMIT_HOUR;										// 농장 관리비 징수 기한 시간 (00 ~ 23)
UINT	CSHFarmManager::FARM_TAX_PAY_ARREARAGE_MAX_FREQ;								// 농장 관리비 연체 가능 횟수
UINT	CSHFarmManager::FARM_TAX_PAY_ARREARAGE_ADD_RATE;								// 농장 관리비 연체료 비율 (%)
UINT	CSHFarmManager::FARM_TAX_PAY_NOTIFY_NONPAYMENT_DDAY;							// 농장 관리비 기한일 몇 일 전부터 미납을 알릴 것인가(-1:은 알리지 않음) 

UINT	CSHFarmManager::FARM_PLANT_CHAR_MOTION_NUM;										// 농작물 심기 캐릭터 모션 번호
UINT	CSHFarmManager::FARM_MANURE_CHAR_MOTION_NUM;									// 농작물 비료주기 캐릭터 모션 번호
UINT	CSHFarmManager::FARM_HARVEST_CHAR_MOTION_NUM;									// 농작물 수확하기 캐릭터 모션 번호
UINT	CSHFarmManager::FARM_CHAR_MOTION_PROGRESSBAR_TYPE;								// 캐릭터 모션 프로그레스바 타입
UINT	CSHFarmManager::FARM_CHAR_MOTION_PROGRESSBAR_WIDTH;								// 캐릭터 모션 프로그레스바 너비

UINT	CSHFarmManager::FARM_PLANT_GARDEN_GRADE_DIFF;									// 농작물을 심기 불가능한 농작물과 텃밭과의 등급 차이

int		CSHFarmManager::FARM_EFFECT_NUM[FARM_EFFECT_KIND_MAX];							// 농장 오브젝트 이펙트	

CSHFarmManager	g_csFarmManager;

int g_tbl_farm[][7] = 
{
	// 080308 KTH, 알케르 농장. 2번 맵, 농장 60개, 텃밭 1개, 텃밭 농작물 15개, 축사 1개, 가축 5개
	{ CSHFarmZone::FARM_ZONE_ALKER, Alker_Farm, 60, 1, 15, 1, 5 },
	// 090921 LUJ, 네라 농장 추가
	{ CSHFarmZone::FARM_ZONE_NERA, Nera_Farm, 60, 1, 15, 1, 5 },
};

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHFarmManager Method																										  생성자
//
CSHFarmManager::CSHFarmManager()
{
	// 090707 LUJ, 세금 납부에 필요한 아이템 컨테이너 초기화
	mTaxHashTable.Initialize( 1 );
	m_pcsFarmZone.Initialize( 1 );
	m_pcsSelFarmObj = NULL;

	ZeroMemory(m_stUpgradeCost, sizeof(m_stUpgradeCost));

	m_nMotionID = NULL;

	m_pstBFarm = NULL;
	m_pstFarmRenderObjList= NULL;
	m_pszTblFarmCropName = NULL;
	m_pstCropHarvestReward = NULL;
	m_pstAnimalReward = NULL;

	ZeroMemory(FARM_EFFECT_NUM, sizeof(FARM_EFFECT_NUM));

	m_nTaxCollectPastWeek = 0;

	// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 추가 : 모드설정(농장맵 or 타맵)
	m_ePayTaxMode = ePayTaxMode_FarmNpc;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHFarmManager Method																										  파괴자
//
CSHFarmManager::~CSHFarmManager()
{
	// 농장 상태를 파일로 남긴다.
#if defined(_MAPSERVER_)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		char szFile[256];
		sprintf(szFile, "./Log/FARM_LOAD_END_%4d%02d%02d%02d%02d.txt", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
		FILE* fp;
		fp = fopen(szFile, "a+");

		sprintf(szFile, "./Log/FARMCROP_LOAD_END_%4d%02d%02d%02d%02d.txt", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
		FILE* fp2;
		fp2 = fopen(szFile, "a+");

		m_pcsFarmZone.SetPositionHead();
		
		for( CSHFarmZone* farmZone = 0;
			(farmZone = m_pcsFarmZone.GetData())!=NULL; )
		{
			for( int j = 0; j < farmZone->GetFarmNum(); ++j )
			{
				CSHFarm* const pFarm = farmZone->GetFarm( j );

				fprintf(fp, "ZONE:%d FARM:%d OWNSTATE:%d OWNER:%d GARDENGRD:%d HOUSEGRD:%d WAREHOUSEGRD:%d ANIMALCAGEGRD:%d FENCEGRD:%d\n",
					farmZone->GetMapID(),
					pFarm->GetID(),
					pFarm->GetOwnState(),
					pFarm->GetOwner(),
					pFarm->GetGarden(0)->GetGrade(),
					pFarm->GetHouse(0)->GetGrade(),
					pFarm->GetWarehouse(0)->GetGrade(),
					pFarm->GetAnimalCage(0)->GetGrade(),
					pFarm->GetFence(0)->GetGrade());

 				for(int k=0; k<pFarm->GetGardenNum(); k++)
				{
					// 텃밭
					CSHGarden* pGarden = pFarm->GetGarden(k);
					for(int p=0; p<pGarden->GetCropNum(); p++)
					{
						// 농작물
						CSHCrop* pCrop = pGarden->GetCrop(p);
						if (fp2)
						{
							fprintf(fp2, "ZONE:%d FARM:%d CROP:%d OWNER:%d STEP:%d LIFE:%d SEEDGRD:%d KIND:%d NEXTSTEPTIME:%d\n",
								farmZone->GetMapID(),
								pFarm->GetID(),
								pCrop->GetID(),
								pCrop->GetOwner(),
								pCrop->GetStep(),
								pCrop->GetLife(),
								pCrop->GetSeedGrade(),
								pCrop->GetKind(),
								pCrop->GetNextStepTime());
						}
					}
				}
				
				for(int k=0; k<pFarm->GetAnimalCageNum(); k++)
				{
					CPen* pPen = pFarm->GetAnimalCage(k);

					for( int p=0; p<pPen->GetAnimalNum(); p++ )
					{
						CAnimal* pAnimal = pPen->GetAnimal(p);
						if(fp2)
						{
							fprintf(fp2, "ZONE:%d PEN:%d ANIMAL:%d OWNER:%d STEP:%d LIFE:%d SEEDGRD:%d KIND:%d NEXTSTEPTIME:%d\n",
								farmZone->GetMapID(),
								pPen->GetID(),
								pAnimal->GetID(),
								pAnimal->GetOwner(),
								pAnimal->GetStep(),
								pAnimal->GetLife(),
								pAnimal->GetGrade(),
								pAnimal->GetKind(),
								pAnimal->GetNextStepTime());
						}
					}
				}
			}
		}							

		fprintf(fp2, "\n\n");
		fprintf(fp, "\n\n");

		fclose(fp2);
		fclose(fp);
	}
#endif

	Release();
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  Init Method																													  초기화
//
VOID CSHFarmManager::Init( int nMapNum )
{
	// 변수 초기화
  	m_pcsSelFarmObj = NULL;
	m_nMotionID = NULL;

	// 자원 해제
	Release();
	// 090520 LUJ, 맵 번호 추가
	LoadFarmInfo( (MAPTYPE)nMapNum, 0 );

	// 091020 pdy 맵타입 채크를 필수 리소스 로딩후의 시점으로 변경
	if( FALSE == GetFarmZoneID( nMapNum, 0 ) )
	{
		return;
	}

	for(int i = 0; i < CSHFarmZone::FARM_ZONE_MAX; ++i)
	{
		const int mapType = g_tbl_farm[ i ][ 1 ];
		const int farmSize = g_tbl_farm[ i ][ 2 ];

		if( nMapNum != mapType )
		{
			continue;
		}

		// 농장 생성 (농장 지역에 속하는)
		CSHFarmZone* const farmZone = new CSHFarmZone( CSHFarmZone::FARM_ZONE( i ), mapType, farmSize );
		farmZone->SetID( (WORD)i );
		m_pcsFarmZone.Add( farmZone, i );

		const DWORD gardenSize = g_tbl_farm[ i ][ 3 ];
		const DWORD cropSize = g_tbl_farm[ i ][ 4 ];
		const DWORD cageSize = g_tbl_farm[ i ][ 5 ];
		const DWORD animalSize = g_tbl_farm[ i ][ 6 ];

		for( int farmIndex = 0 ; farmIndex < farmZone->GetFarmNum(); ++farmIndex )
		{
			// 텃밭 생성 (농장에 속하는)
			CSHFarm* pcsFarm = farmZone->GetFarm( farmIndex );
			pcsFarm->CreateGarden( gardenSize, cropSize );
			pcsFarm->CreateAnimalCage( cageSize, animalSize );
			pcsFarm->CreateFence( 1 );
			pcsFarm->CreateHouse( 1 );
			pcsFarm->CreateWarehouse( 1 );
		}
	}

	// 090520 LUJ, 맵 번호 추가
	LoadFarmInfo( (MAPTYPE)nMapNum, 1 );

	m_BuyFarm_WaitInfoList.Initialize( 100 ) ;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  Release Method																													해제
//
VOID CSHFarmManager::Release()
{
	m_pcsFarmZone.SetPositionHead();
	CSHFarmZone* farmZone = NULL;
	while( (farmZone = m_pcsFarmZone.GetData()) != NULL )
	{
		SAFE_DELETE( farmZone );
	}
	m_pcsFarmZone.RemoveAll();

	// 090707 LUJ, 세금 납부 컨테이너 초기화
	mTaxHashTable.SetPositionHead();
	Tax* tax = NULL;
	while( (tax = mTaxHashTable.GetData()) != NULL )
	{
		SAFE_DELETE( tax );
	}
	mTaxHashTable.RemoveAll();

 	SAFE_DELETE_ARRAY(m_pstBFarm);
	SAFE_DELETE_ARRAY(m_pstFarmRenderObjList);
	SAFE_DELETE_ARRAY(m_pszTblFarmCropName);
	SAFE_DELETE_ARRAY(m_pstCropHarvestReward);
	SAFE_DELETE_ARRAY(m_pstAnimalReward);

	stBuyFarmWaitInfo* pWaitList = NULL ;
	m_BuyFarm_WaitInfoList.SetPositionHead();
	while( (pWaitList = m_BuyFarm_WaitInfoList.GetData()) != NULL )
	{
		SAFE_DELETE( pWaitList );
	}
}

BOOL CSHFarmManager::LoadFarmInfo( MAPTYPE mapType, int nType)
{
#if !defined(_AGENTSERVER)
	// 농장 수치 데이터 읽기
 	char szFile[MAX_PATH];
	char szLine[MAX_PATH];
	int	 nKind = 0, nCnt = 0;
	CMHFile fp;

	if (nType == 0)
	{
		sprintf(szFile, "./System/Resource/FarmRenderObjName.bin");
		fp.Init(szFile, "rb");
		if(!fp.IsInited())
		{
			char szTmp[256];
			sprintf(szTmp, "%s 파일이 존재하지 않습니다.", szFile);
			ASSERTMSG(0, szTmp);
			return FALSE;
		}

		// ..먼저 개수 체크
		nCnt = 0;
		while(1)
		{
			if(fp.IsEOF()) break;
			fp.GetLine(szLine, sizeof(szLine));
			if (strstr(szLine, "//") ||
				IsEmptyLine(szLine))
			{
				continue;			
			}

			nCnt++;
		}
		fp.Release();

		RENDER_CROP_KIND_NUM = nCnt / CSHCrop::CROP_STEP_MAX;

		SAFE_DELETE_ARRAY(m_pszTblFarmCropName);
		m_pszTblFarmCropName = new stFARMOBJNAME[nCnt];

		// ..데이터 읽기
		fp.Init(szFile, "rb");
		if(!fp.IsInited()) return FALSE;
		for(int i=0; i<nCnt; i++)
		{
			if (fp.IsEOF()) break;
			fp.GetLine(szLine, sizeof(szLine));
			if (strstr(szLine, "//") ||
				IsEmptyLine(szLine))
			{
				i--;
				continue;			
			}

			SafeStrCpy(m_pszTblFarmCropName[i].szName, szLine, sizeof(m_pszTblFarmCropName[i].szName));
		}

		fp.Release();

		SAFE_DELETE_ARRAY(m_pstCropHarvestReward);
		m_pstCropHarvestReward = new stCROPHARVESTREWARD[RENDER_CROP_KIND_NUM];
		ZeroMemory(m_pstCropHarvestReward, sizeof(stCROPHARVESTREWARD)*RENDER_CROP_KIND_NUM);

		SAFE_DELETE_ARRAY(m_pstAnimalReward);
		m_pstAnimalReward = new stANIMALREWARD[FARM_ANIMAL_KIND_MAX];
		ZeroMemory(m_pstAnimalReward, sizeof(stANIMALREWARD)*FARM_ANIMAL_KIND_MAX);

		// 각종 수치 정보 읽기
		sprintf(szFile, "./System/Resource/Farm.bin");
		fp.Init(szFile, "rb");
		if(!fp.IsInited())
		{
			char szTmp[256];
			sprintf(szTmp, "%s 파일이 존재하지 않습니다.", szFile);
			ASSERTMSG(0, szTmp);
			return FALSE;
		}

		BOOL bVerify = FALSE;
		while(TRUE)
		{
			if (fp.IsEOF()) break;
			fp.GetLine(szLine, sizeof(szLine));
			if (strstr(szLine, "//") ||
				IsEmptyLine(szLine))
			{
				continue;			
			}
			else if (strstr(szLine, "END_KIND")) 
			{
			// 데이터 검증
			#if defined(_DEBUG)
				if (bVerify && nCnt != RENDER_CROP_KIND_NUM)
				{
					char szTmp[256];
					sprintf(szTmp, "농작물의 이름 개수와 농작물의 수확 시 생성 아이템 개수가 다릅니다.\n데이터 파일을 확인해 주세요.[%s]", szFile);
					ASSERTMSG(0, szTmp);
				}
			#endif

				nCnt = 0;
				nKind++;
				bVerify = FALSE;
				continue;
			}

			switch(nKind)
			{
			case 0:
//				sscanf(szLine, "%d %d %d %d %d",	&FARM_PLANT_CHAR_MOTION_NUM,
				sscanf(szLine, "%d %d %d %d %d %d %d %d %d",	&FARM_PLANT_CHAR_MOTION_NUM,
													&FARM_MANURE_CHAR_MOTION_NUM,
													&FARM_HARVEST_CHAR_MOTION_NUM,
													&FARM_CHAR_MOTION_PROGRESSBAR_TYPE,
													&FARM_CHAR_MOTION_PROGRESSBAR_WIDTH,
													&FARM_BREED_CHAR_MOTION,
													&FARM_FEED_CHAR_MOTION,
													&FARM_REWARD_ANIMAL_CHAR_MOTION,
													&FARM_CLEANING_CHAR_MOTION);
				break;
			case 1:		
				sscanf(szLine, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",	&FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_PLANT],
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_MANURE],
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_STEPUP],		
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_HARVEST],
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_DIE],	
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_GARDEN],	
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_FENCE],	
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_ANIMALCAGE],
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_HOUSE],	
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_WAREHOUSE],
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_ANIMAL_IN_STALL],
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_ANIMAL_FEED],
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_CLEANNING],
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_ANIMAL_REWARD],
																				&FARM_EFFECT_NUM[FARM_EFFECT_KIND_ANIMAL_DIE]);
				break;
			case 2:		
				sscanf(szLine, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",	&CSHCrop::CROP_LIFE_DECREASE_TIME,
																		&CSHCrop::CROP_PLANT_SEEDING_TIME,	
																		&CSHCrop::CROP_PLANT_RETRY_TIME,
																		&CSHCrop::CROP_MANURE_TIME,		
																		&CSHCrop::CROP_MANURE_RETRY_TIME,
																		&CSHCrop::CROP_HARVEST_TIME,
																		&CAnimal::ANIMAL_LIFE_DECREASE_TIME,
																		&CAnimal::ANIMAL_IN_STALL_TIME,
																		&CAnimal::ANIMAL_RETRY_TIME,
																		&CAnimal::ANIMAL_FEED_TIME,
																		&CAnimal::ANIMAL_FEED_DELAY_TIME,
																		&CAnimal::ANIMAL_STALL_REWARD_TIME,
																		&CAnimal::ANIMAL_CONTENTMENT_DECREASE_TIME,
																		&CAnimal::ANIMAL_INTEREST_DECREASE_TIME,
																		&CAnimal::ANIMAL_CLEANING_TIME,
																		&CAnimal::ANIMAL_CLEANING_RETRY_TIME);
				break;
			case 3:
				sscanf(szLine, "%d %d",				&FARM_PLANT_GARDEN_GRADE_DIFF,
													&FARM_BREED_ANIMAL_GRADE_DIFF);
				break;
			case 4:
				sscanf(szLine, "%d %d %d %d %d",	&FARM_BUY_MAX_NUM,		
													&FARM_BUY_FAMILY_MEMNUM,
													&FARM_BUY_MASTER_LEVEL,			
													&FARM_BUY_FAMILY_HONORPOINT,
													&FARM_BUY_MONEY);
				break;
			case 5:
				{
					LPCTSTR seperator = "\t\n ";
					LPCTSTR textPayWeekInterval = _tcstok( szLine, seperator );
					LPCTSTR textPayLimitDay = _tcstok( 0, seperator );
					LPCTSTR textPayLimitHour = _tcstok( 0, seperator );
					LPCTSTR textPayArrearageMaxFrequecy = _tcstok( 0, seperator );
					LPCTSTR textPayArrearageAddRate = _tcstok( 0, seperator );
					LPCTSTR textPayNotifyNonPaymentDay = _tcstok( 0, seperator );

					FARM_TAX_PAY_WEEK_INTERVAL = _ttoi( textPayWeekInterval ? textPayWeekInterval : "" );
					FARM_TAX_PAY_LIMIT_DAY = _ttoi( textPayLimitDay ? textPayLimitDay : "" );
					FARM_TAX_PAY_LIMIT_HOUR = _ttoi( textPayLimitHour ? textPayLimitHour : "" );
					FARM_TAX_PAY_ARREARAGE_MAX_FREQ = _ttoi( textPayArrearageMaxFrequecy ? textPayArrearageMaxFrequecy : "" );
					FARM_TAX_PAY_ARREARAGE_ADD_RATE = _ttoi( textPayArrearageAddRate ? textPayArrearageAddRate : "" );
					FARM_TAX_PAY_NOTIFY_NONPAYMENT_DDAY = _ttoi( textPayNotifyNonPaymentDay ? textPayNotifyNonPaymentDay : "" );					
				}
				break;

			case 6:
				sscanf(szLine, "%d %d %d",			&CSHCrop::CROP_NEXT_STEP_TIME[nCnt][0],
													&CSHCrop::CROP_NEXT_STEP_TIME[nCnt][1],
													&CSHCrop::CROP_NEXT_STEP_TIME[nCnt][2]);
				break;
			case 7:
				sscanf(szLine, "%d %d %d %d %d %d",	&CSHCrop::CROP_STEP_MIN_LIFE[nCnt][0], &CSHCrop::CROP_STEP_MAX_LIFE[nCnt][0],
													&CSHCrop::CROP_STEP_MIN_LIFE[nCnt][1], &CSHCrop::CROP_STEP_MAX_LIFE[nCnt][1],
													&CSHCrop::CROP_STEP_MIN_LIFE[nCnt][2], &CSHCrop::CROP_STEP_MAX_LIFE[nCnt][2]);
				break;
			case 8:
				sscanf(szLine, "%d %d %d %d %d %d",	&CSHCrop::CROP_STEP_MANURE_MIN_LIFE[nCnt][0], &CSHCrop::CROP_STEP_MANURE_MAX_LIFE[nCnt][0],
													&CSHCrop::CROP_STEP_MANURE_MIN_LIFE[nCnt][1], &CSHCrop::CROP_STEP_MANURE_MAX_LIFE[nCnt][1],
													&CSHCrop::CROP_STEP_MANURE_MIN_LIFE[nCnt][2], &CSHCrop::CROP_STEP_MANURE_MAX_LIFE[nCnt][2]);
				break;
			case 9:
				sscanf(szLine, "%d %d %d %d %d %d %d",	&CSHCrop::CROP_HARVEST_PROBA[nCnt][0], 
														&CSHCrop::CROP_HARVEST_PROBA[nCnt][1], 
														&CSHCrop::CROP_HARVEST_PROBA[nCnt][2], 
														&CSHCrop::CROP_HARVEST_PROBA[nCnt][3], 
														&CSHCrop::CROP_HARVEST_PROBA[nCnt][4], 
														&CSHCrop::CROP_HARVEST_PROBA[nCnt][5], 
														&CSHCrop::CROP_HARVEST_PROBA[nCnt][6]);
				break;
			case 10:
				sscanf(szLine, "%d %d %d %d %d",	&CSHCrop::CROP_HARVEST_DELTA_PROBA[nCnt][0],
													&CSHCrop::CROP_HARVEST_DELTA_PROBA[nCnt][1],
													&CSHCrop::CROP_HARVEST_DELTA_PROBA[nCnt][2],
													&CSHCrop::CROP_HARVEST_DELTA_PROBA[nCnt][3],
													&CSHCrop::CROP_HARVEST_DELTA_PROBA[nCnt][4]);
				break;
			case 11:
				sscanf(szLine, "%d %d",				&m_stUpgradeCost[CSHFarm::FARM_EVENT_GARDEN-1][nCnt].nGold,
													&m_stUpgradeCost[CSHFarm::FARM_EVENT_GARDEN-1][nCnt].nFamilyHonorPoint);
				break;
			case 12:
				sscanf(szLine, "%d %d",				&m_stUpgradeCost[CSHFarm::FARM_EVENT_FENCE-1][nCnt].nGold,
													&m_stUpgradeCost[CSHFarm::FARM_EVENT_FENCE-1][nCnt].nFamilyHonorPoint);
				break;
			case 13:
				sscanf(szLine, "%d %d",				&m_stUpgradeCost[CSHFarm::FARM_EVENT_HOUSE-1][nCnt].nGold,
													&m_stUpgradeCost[CSHFarm::FARM_EVENT_HOUSE-1][nCnt].nFamilyHonorPoint);
				break;
			case 14:
				sscanf(szLine, "%d %d",				&m_stUpgradeCost[CSHFarm::FARM_EVENT_WAREHOUSE-1][nCnt].nGold,
													&m_stUpgradeCost[CSHFarm::FARM_EVENT_WAREHOUSE-1][nCnt].nFamilyHonorPoint);
				break;
			case 15:
				sscanf(szLine, "%d %d",				&m_stUpgradeCost[CSHFarm::FARM_EVENT_ANIMALCAGE-1][nCnt].nGold,
													&m_stUpgradeCost[CSHFarm::FARM_EVENT_ANIMALCAGE-1][nCnt].nFamilyHonorPoint);
				break;
			case 16:
				
				sscanf(szLine, "%u %u %u %u %u %u %u %u %u %u",	&m_pstCropHarvestReward[nCnt].nItemID[0], &m_pstCropHarvestReward[nCnt].nItemNum[0],
																&m_pstCropHarvestReward[nCnt].nItemID[1], &m_pstCropHarvestReward[nCnt].nItemNum[1],
																&m_pstCropHarvestReward[nCnt].nItemID[2], &m_pstCropHarvestReward[nCnt].nItemNum[2],
																&m_pstCropHarvestReward[nCnt].nItemID[3], &m_pstCropHarvestReward[nCnt].nItemNum[3],
																&m_pstCropHarvestReward[nCnt].nItemID[4], &m_pstCropHarvestReward[nCnt].nItemNum[4]);
				bVerify = TRUE;
				break;
			// 축사등급에 따른 기본 만족도와 관심도
			case 17:
				sscanf(szLine, "%u %u",	&CAnimal::ANIMAL_STEP_MIN_LIFE[nCnt][0], &CAnimal::ANIMAL_STEP_MIN_LIFE[nCnt][1]);
				break;
			// 가축 먹이 관련 증가량.
			case 18:
				sscanf(szLine, "%d %d",	&CAnimal::ANIMAL_STEP_FEED_LIFE[nCnt][0], &CAnimal::ANIMAL_STEP_FEED_LIFE[nCnt][1]);
				break;
			// 가축 청소 관련 증가량.
			case 19:
				sscanf(szLine, "%d %d",	&CAnimal::ANIMAL_STEP_CLEANING_LIFE[nCnt][0], &CAnimal::ANIMAL_STEP_CLEANING_LIFE[nCnt][1]);				
				break;
			// 가축 보상 bad, normal 구간
			case 20:
				sscanf(szLine, "%u %u", &FARM_ANIMAL_REWARD_BAD, &FARM_ANIMAL_REWARD_NORMAL);
				break;
			// 가축 보상 테이블
			case 21:
				sscanf(szLine,"%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u",	&m_pstAnimalReward[nCnt].nItemID[0], &m_pstAnimalReward[nCnt].nItemNum[0], &m_pstAnimalReward[nCnt].nPercent[0],
															&m_pstAnimalReward[nCnt].nItemID[1], &m_pstAnimalReward[nCnt].nItemNum[1], &m_pstAnimalReward[nCnt].nPercent[1],
															&m_pstAnimalReward[nCnt].nItemID[2], &m_pstAnimalReward[nCnt].nItemNum[2], &m_pstAnimalReward[nCnt].nPercent[2],
															&m_pstAnimalReward[nCnt].nItemID[3], &m_pstAnimalReward[nCnt].nItemNum[3], &m_pstAnimalReward[nCnt].nPercent[3],
															&m_pstAnimalReward[nCnt].nItemID[4], &m_pstAnimalReward[nCnt].nItemNum[4], &m_pstAnimalReward[nCnt].nPercent[4],
															&m_pstAnimalReward[nCnt].nItemID[5], &m_pstAnimalReward[nCnt].nItemNum[5], &m_pstAnimalReward[nCnt].nPercent[5],
															&m_pstAnimalReward[nCnt].nItemID[6], &m_pstAnimalReward[nCnt].nItemNum[6], &m_pstAnimalReward[nCnt].nPercent[6],
															&m_pstAnimalReward[nCnt].nItemID[7], &m_pstAnimalReward[nCnt].nItemNum[7], &m_pstAnimalReward[nCnt].nPercent[7],
															&m_pstAnimalReward[nCnt].nItemID[8], &m_pstAnimalReward[nCnt].nItemNum[8], &m_pstAnimalReward[nCnt].nPercent[8]
															);
				break;
			// 가축 이름 테이블
			case 22:
				SafeStrCpy(CAnimal::ANIMAL_NAME_TBL[nCnt], szLine, 256);
				//sscanf(szLine,"%s", CAnimal::ANIMAL_NAME_TBL[nCnt]);
				break;
			// 090710 LUJ, 텃밭 레벨에 따라 세금을 차등 적용함
			case 23:
				{
					LPCTSTR seperator = "\t\n ";
					LPCTSTR textFarmLevel = _tcstok( szLine, seperator );
					LPCTSTR textTaxMoney = _tcstok( 0, seperator );

					if( 0 == textFarmLevel ||
						0 == textTaxMoney )
					{
						break;
					}

					const LEVELTYPE farmLevel = (LEVELTYPE)_ttoi( textFarmLevel );
					Tax* tax = mTaxHashTable.GetData( farmLevel );

					if( 0 == tax )
					{
						tax = new Tax;
						mTaxHashTable.Add( tax, farmLevel );
					}

					tax->mFarmLevel = farmLevel;
					tax->mMoney = _ttoi( textTaxMoney );
					const DWORD itemMaxSize = ( sizeof( tax->mItem ) / sizeof( *tax->mItem ) );

					for( DWORD i = 0; i < itemMaxSize; ++i )
					{
						LPCTSTR textItemIndex = _tcstok( 0, seperator );
						LPCTSTR textItemQuantity = _tcstok( 0, seperator );

						if( 0 == textItemIndex ||
							0 == textItemQuantity )
						{
							break;
						}

						Tax::Item& item = tax->mItem[ i ];
						item.mIndex = _ttoi( textItemIndex );
						item.mQuantity = _ttoi( textItemQuantity );
						++( tax->mItemSize );
					}
				}
				break;
			}

			nCnt++;
		}

		fp.Release();
	}
	else
	{
		// 090520 LUJ, 맵 번호에 따라 정보를 읽도록 한다
		sprintf(szFile, "System/Resource/FarmRenderObjP%02d.bin", mapType );
		fp.Init(szFile, "rb");
		if(fp.IsInited() == FALSE)
		{
			char szTmp[256];
			sprintf(szTmp, "%s 파일이 존재하지 않습니다.", szFile);
			ASSERTMSG(0, szTmp);
			return FALSE;
		}

		int nFarmID = 0, nFarmID2 = 0, nCropID = 0;
		// 080410 KTH 
		int nPenID = 0;
		VECTOR3 stPos;

		while(1)
		{
			if(fp.IsEOF()) break;
			fp.GetLine(szLine, 256);

			if (strstr(szLine, "01_jakmul03_lv2.MOD"))
			{
				char szTmp[256];
				float nValue[13];

				sscanf(szLine, "%s %u %f %f %f %f %f %f %f %f %f %f %f", szTmp,	&nValue[1], &nValue[2], &nValue[3], &nValue[4],
																				&nValue[5], &nValue[6], &nValue[7], &nValue[8],
																				&nValue[9], &nValue[10], &nValue[11], &nValue[12]);
				stPos.x = nValue[5];
				stPos.y = 0;
				stPos.z = nValue[7];
				int farmZoneId = 0;

				if( FALSE == GetFarmZoneID( mapType, &farmZoneId ) )
				{
					continue;
				}

				CSHFarmZone* const farmZone = GetFarmZone( (WORD)farmZoneId );

				if( 0 == farmZone )
				{
					continue;
				}

				farmZone->GetFarm(nCropID/15)->GetGarden(0)->GetCrop(nCropID%15)->SetPos(&stPos);
 				farmZone->GetFarm(nCropID/15)->GetGarden(0)->GetCrop(nCropID%15)->SetDir((float)CSHMath::GetRandomNum(1, 20000));
				nCropID++;
			}
			else if (strstr(szLine, "01_farm_fence_lv1.MOD"))
			{
				char szTmp[256];
				float nValue[13];

				sscanf(szLine, "%s %u %f %f %f %f %f %f %f %f %f %f %f", szTmp,	&nValue[1], &nValue[2], &nValue[3], &nValue[4],
																				&nValue[5], &nValue[6], &nValue[7], &nValue[8],
																				&nValue[9], &nValue[10], &nValue[11], &nValue[12]);
				stPos.x = nValue[5];
				stPos.y = 0;
				stPos.z = nValue[7];
				int farmZoneId = 0;

				if( FALSE == GetFarmZoneID( mapType, &farmZoneId ) )
				{
					continue;
				}

				CSHFarmZone* const farmZone = GetFarmZone( (WORD)farmZoneId );

				if( 0 == farmZone )
				{
					continue;
				}

				farmZone->GetFarm(nFarmID)->SetPos(&stPos);
				farmZone->GetFarm(nFarmID)->SetDir(nValue[11]);
				nFarmID++;
			}
			else if (strstr(szLine, "01_farm_housepost.MOD"))
			{
				char szTmp[256];
				float nValue[13];

				sscanf(szLine, "%s %u %f %f %f %f %f %f %f %f %f %f %f", szTmp,	&nValue[1], &nValue[2], &nValue[3], &nValue[4],
																				&nValue[5], &nValue[6], &nValue[7], &nValue[8],
																				&nValue[9], &nValue[10], &nValue[11], &nValue[12]);
				stPos.x = nValue[5];
				stPos.y = 0;
				stPos.z = nValue[7];
				int farmZoneId = 0;

				if( FALSE == GetFarmZoneID( mapType, &farmZoneId ) )
				{
					continue;
				}

				CSHFarmZone* const farmZone = GetFarmZone( (WORD)farmZoneId );

				if( 0 == farmZone )
				{
					continue;
				}

				farmZone->GetFarm(nFarmID2)->GetFence(0)->SetPos(&stPos);
				farmZone->GetFarm(nFarmID2)->GetFence(0)->SetDir(nValue[11]);
				nFarmID2++;
			}
			else if(strstr(szLine, "m030_lod00.MOD"))
			{
				char szTmp[256];
				float nValue[13];

				sscanf(szLine, "%s %u %f %f %f %f %f %f %f %f %f %f %f", szTmp,	&nValue[1], &nValue[2], &nValue[3], &nValue[4],
																				&nValue[5], &nValue[6], &nValue[7], &nValue[8],
																				&nValue[9], &nValue[10], &nValue[11], &nValue[12]);

				stPos.x = nValue[5];
				stPos.y = 0;
				stPos.z = nValue[7];
				int farmZoneId = 0;

				if( FALSE == GetFarmZoneID( mapType, &farmZoneId ) )
				{
					continue;
				}

				CSHFarmZone* const farmZone = GetFarmZone( (WORD)farmZoneId );

				if( 0 == farmZone )
				{
					continue;
				}

				farmZone->GetFarm(nPenID/5)->GetAnimalCage(0)->GetAnimal(nPenID%5)->SetPos(&stPos);
				farmZone->GetFarm(nPenID/5)->GetAnimalCage(0)->GetAnimal(nPenID%5)->SetDir(nValue[11]);
				nPenID++;
			}
		}

		fp.Release();

	#if !defined(_MAPSERVER_)
		// 렌더링 오브젝트 리스트 정보 읽기
		sprintf(szFile, "System/Resource/FarmRenderObjList.bin");
		fp.Init(szFile, "rb");
		if(fp.IsInited() == FALSE)
		{
			char szTmp[256];
			sprintf(szTmp, "%s 파일이 존재하지 않습니다.", szFile);
			ASSERTMSG(0, szTmp);
			return FALSE;
		}

		// ..먼저 개수 체크
		nCnt = 0;
		while(1)
		{
			if(fp.IsEOF()) break;
			fp.GetLine(szLine, sizeof(szLine));
			if (strstr(szLine, "//") ||
				IsEmptyLine(szLine))
			{
				continue;			
			}

			nCnt++;
		}

		fp.Release();

		SAFE_DELETE_ARRAY(m_pstFarmRenderObjList);
		m_pstFarmRenderObjList = new stFARMRENDEROBJLIST[nCnt];
		ZeroMemory(m_pstFarmRenderObjList, sizeof(stFARMRENDEROBJLIST)*nCnt);

		// ..데이터 읽기
		fp.Init(szFile, "rb");
		if(fp.IsInited() == FALSE)
			return FALSE;

 		nCnt = 0;
		while(1)
		{
			if(fp.IsEOF()) break;
			fp.GetLine(szLine, sizeof(szLine), FALSE);
			if (strstr(szLine, "//") ||
				IsEmptyLine(szLine))
			{
				continue;			
			}

			sscanf(szLine, "%d %s %s %f %d %d",	&m_pstFarmRenderObjList[nCnt].nID,
												m_pstFarmRenderObjList[nCnt].szGroupName,
												m_pstFarmRenderObjList[nCnt].szChxName,
												&m_pstFarmRenderObjList[nCnt].nScale,
												&m_pstFarmRenderObjList[nCnt].nNameYpos,
												&m_pstFarmRenderObjList[nCnt].bShowGroupName);
			char szTmp[256];
			_parsingKeywordString(m_pstFarmRenderObjList[nCnt].szGroupName, szTmp);
			lstrcpy(m_pstFarmRenderObjList[nCnt].szGroupName, szTmp);
			strcat(m_pstFarmRenderObjList[nCnt].szChxName, ".chx");
			nCnt++;
		}

   		fp.Release();

		// 데이터 검증
		#if defined(_DEBUG)
			int nCropNum = 0;
			for(int i=0; i<nCnt; i++)
			{
				if (GetRenderObjKind(m_pstFarmRenderObjList[i].nID) == RENDER_CROP_KIND)
					nCropNum++;
			}
			if (nCropNum != RENDER_CROP_KIND_NUM*CSHCrop::CROP_STEP_MAX)
			{
				char szTmp[256];
				sprintf(szTmp, "농작물의 이름 개수와 농작물 3D 그래픽 데이터 정보 개수가 다릅니다.\n데이터 파일을 확인해 주세요.[%s]", szFile);
				ASSERTMSG(0, szTmp);
			}

			for(int i=0; i<nCnt-1; i++)
			{
				for(int j=i+1; j<nCnt; j++)
				{
					if (m_pstFarmRenderObjList[i].nID == m_pstFarmRenderObjList[j].nID)
					{
						char szTmp[256];
						sprintf(szTmp, "농작물 3D 그래픽 데이터 파일의 정보 중 ID가 중복되는 항목이 있습니다.[ID:%d]\n데이터 파일을 확인해 주세요.[%s]", m_pstFarmRenderObjList[i].nID, szFile);
						ASSERTMSG(0, szTmp);
					}
				}
			}
		#endif


		// 렌더링 오브젝트 정보 읽기
		sprintf( szFile, "System/Resource/FarmRenderObj%02d.bin", mapType );
		fp.Init(szFile, "rb");
		if(fp.IsInited() == FALSE)
			return FALSE;

		// ..먼저 개수 체크
		nCnt = 0;
		while(1)
		{
			if(fp.IsEOF()) break;
			fp.GetLine(szLine, sizeof(szLine));
			if (strstr(szLine, "//") ||
				IsEmptyLine(szLine))
			{
				continue;			
			}

			nCnt++;
		}

		fp.Release();

		SAFE_DELETE_ARRAY(m_pstBFarm);
		// nCnt 를 +1 해서 메모리 공간을 잡는다. 왜인지는 모르겠지만, nCnt 로 new 하면 delete 시 메모리 에러가 발생함.
		// 배열의 범위를 넘어서서 값을 쓴 것으로 처리됨. (DAMAGE: after Normal block ..... 하는 에러)
		// 마지막 값(m_pstBFarm[nCnt].nID)을 읽는 부분이 delete 시에 에러를 발생시켰음. 이 부분만 제거하면 에러 발생 안 함.
		// 이상한 것은 sscanf 를 fscanf 로 바꿔서 코드를 작성해서 테스트하면 문제 없음.
		// 왜 ???
		nCnt += 1;
		m_pstBFarm = new stBFARM[nCnt];
		ZeroMemory(m_pstBFarm, sizeof(stBFARM)*nCnt);

		// ..데이터 읽기
		fp.Init(szFile, "rb");
		if(fp.IsInited() == FALSE)
			return FALSE;

		nCnt = 0;
		while(1)
		{
			if(fp.IsEOF()) break;
			fp.GetLine(szLine, sizeof(szLine), FALSE);
			if (strstr(szLine, "//") ||
				IsEmptyLine(szLine))
			{
				continue;			
			}

			m_pstBFarm[nCnt].nMapNum = mapType;
			sscanf(
				szLine,
				"%d %s %d",
				&m_pstBFarm[nCnt].nKind,
                m_pstBFarm[nCnt].szName,
				&m_pstBFarm[nCnt].nID);
			char szTmp[256];
			_parsingKeywordString(m_pstBFarm[nCnt].szName, szTmp);
			lstrcpy(m_pstBFarm[nCnt].szName, szTmp);

			nCnt++;
		}

		fp.Release();
	#endif
	}

#endif

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  GetFarmZoneID Method																							 농장 지역 번호 얻기
//
BOOL CSHFarmManager::GetFarmZoneID(int nMapNum, int* pZone)
{
	BOOL bResult = FALSE;

	for(int i=0; i<CSHFarmZone::FARM_ZONE_MAX; i++)
	{
		if (g_tbl_farm[i][1] == nMapNum)
		{
			if (pZone) *pZone = i;
			bResult = TRUE;
		}
	}

	return bResult;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  GetFarmMapNum Method																							   농장 맵 번호 얻기
//
BOOL CSHFarmManager::GetFarmMapNum(UINT nFarmZone, int* pZone)
{
	if (nFarmZone < CSHFarmZone::FARM_ZONE_MAX)
	{
		if (pZone) *pZone = g_tbl_farm[nFarmZone][1];
		return TRUE;
	}

	return FALSE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  MainLoop Method																											   메인 루프
//
VOID CSHFarmManager::MainLoop()
{
#if defined(_AGENTSERVER)
	int nMapNum = 0;
#elif defined(_MAPSERVER_)
	int nMapNum = g_pServerSystem->GetMapNum();
#else
 	int nMapNum = MAP->GetMapNum();
#endif

	int farmZoneId = 0;

	// 090520 LUJ, 맵에 해당하는 프로세스만 실행한
	if( GetFarmZoneID( nMapNum, &farmZoneId ) )
	{
		CSHFarmZone* const farmZone = GetFarmZone( (WORD)farmZoneId );

		if( farmZone )
		{
			farmZone->MainLoop();
		}
	}

	// 이벤트 처리
	SRV_ProcessEvent();

	// 농장 관리비 처리
#if defined(_MAPSERVER_)
	SRV_ProcTaxCollect();
	SRV_ProcessFarmBuyWaitList();
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  GetRenderObjKind Method																					   렌더링 오브젝트 종류 얻기
//
int CSHFarmManager::GetRenderObjKind(int nID)
{
	int nKind = 0;

	if (RENDER_SALEBOARD_KIND  == nID)
	{
		nKind = RENDER_SALEBOARD_KIND;
	}
	else if (RENDER_STEWARD_KIND == nID)
	{
		nKind = RENDER_STEWARD_KIND;
	}
	else if (RENDER_FENCE_KIND <= nID && nID < RENDER_FENCE_KIND+CSHFarmObj::MAX_GRADE)
	{
		nKind = RENDER_FENCE_KIND;
	}
	else if (RENDER_ANIMALCAGE_KIND <= nID && nID < RENDER_ANIMALCAGE_KIND+CSHFarmObj::MAX_GRADE)
	{
		nKind = RENDER_ANIMALCAGE_KIND;
	}
	else if (RENDER_HOUSE_KIND <= nID && nID < RENDER_HOUSE_KIND+CSHFarmObj::MAX_GRADE)
	{
		nKind = RENDER_HOUSE_KIND;
	}
	else if (RENDER_GARDENPATCH_KIND <= nID && nID < RENDER_GARDENPATCH_KIND+CSHFarmObj::MAX_GRADE)
	{
		nKind = RENDER_GARDENPATCH_KIND;
	}
	else if (RENDER_WAREHOUSE_KIND <= nID && nID < RENDER_WAREHOUSE_KIND+CSHFarmObj::MAX_GRADE)
	{
		nKind = RENDER_WAREHOUSE_KIND;
	}
	else if (RENDER_CROP_KIND <= nID && nID < RENDER_CROP_KIND+100*(RENDER_CROP_KIND_NUM-1)+CSHCrop::CROP_STEP_MAX)
	{
		nKind = RENDER_CROP_KIND;
	}
	else if (RENDER_BASIC_FENCE == nID)
	{
		nKind = RENDER_BASIC_FENCE;
	}
	else if (RENDER_BASIC_GARDENPATCH_KIND == nID)
	{
		nKind = RENDER_BASIC_GARDENPATCH_KIND;
	}
	else if (RENDER_ANIMAL_KIND <= nID && nID < RENDER_ANIMAL_KIND+(RENDER_ANIMAL_KIND_NUM))
	{
		nKind = RENDER_ANIMAL_KIND;
	}
	else if (RENDER_BASIC_ANIMAL_CAGE_SLOT_KIND == nID)
	{
		nKind = RENDER_BASIC_ANIMAL_CAGE_SLOT_KIND;
	}
	//else if (RENDER_ANIMAL_CAGE_SLOT_KIND <= nID && nID < RENDER_ANIMAL_CAGE_SLOT_KIND+CSHFarmObj::MAX_GRADE)
	//{
	//	nKind = RENDER_ANIMAL_CAGE_SLOT_KIND;
	//}

	return nKind;
}

// 090710 LUJ, 농장에 맞는 세금 정보를 반환한다
const CSHFarmManager::Tax& CSHFarmManager::GetTax( DWORD gardenLevel )
{
	const Tax* tax = mTaxHashTable.GetData( gardenLevel );

	// 090710 LUJ, 가장 마지막에 등록된 세금 정보를 찾는다
	if( 0 == tax )
	{
		mTaxHashTable.SetPositionHead();
        Tax* lastTax = NULL;
		while( (lastTax = mTaxHashTable.GetData()) != NULL )
		{
			tax = lastTax;
		}
	}

	static const Tax emptyTax;
	return tax ? *tax : emptyTax;
}

VOID CSHFarmManager::SRV_BuyFarm(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, int nAgentChecked)
{
#if defined(_MAPSERVER_)
	CSHMain::RESULT eResult = CSHMain::RESULT_OK;

	CSHFarmZone* const pFarmZone = GetFarmZone( (WORD)eFarmZone );

	if( 0 == pFarmZone )
	{
		return;
	}

	CSHFarm* pcsFarm = pFarmZone->GetFarm(nFarmID);

	// 비어있는 농장인가?
	if (pcsFarm->GetOwnState() != CSHFarmObj::OWN_STATE_EMPTY)
	{
		return;
	}
	else if( FARM_BUY_MAX_NUM <= UINT( pFarmZone->GetFarmNum( pPlayer->GetID() ) ) )
	{
		eResult = CSHMain::RESULT_FAIL_01;
		goto FAILED;
	}

	// 레벨 체크
	if (pPlayer->GetLevel() < FARM_BUY_MASTER_LEVEL)
	{
		eResult = CSHMain::RESULT_FAIL_02;
		goto FAILED;
	}

	// 돈 체크
	if (pPlayer->GetMoney() < FARM_BUY_MONEY)
	{
		eResult = CSHMain::RESULT_FAIL_03;
		goto FAILED;
	}

	// 거리체크..
	VECTOR3 stPlayerPos;
	pPlayer->GetPosition(&stPlayerPos);
	float nDistance = CalcDistanceXZ(pcsFarm->GetPos(), &stPlayerPos); 
	if (nDistance > OBJECT_CONTROL_DISTANCE*15)
	{
		eResult = CSHMain::RESULT_FAIL_04;
		goto FAILED;
	}

	if (nAgentChecked == 0)
	{
		// 에이전트에서 패밀리 상태 확인
		MSG_DWORD4					stPacket;

		stPacket.Category			= MP_FARM;
		stPacket.Protocol			= MP_FARM_BUY_AGENT_CHECK;
		stPacket.dwObjectID 		= pPlayer->GetID();
		stPacket.dwData1			= eFarmZone;
		stPacket.dwData2			= nFarmID;
		stPacket.dwData3			= FARM_BUY_FAMILY_MEMNUM;
		stPacket.dwData4			= FARM_BUY_FAMILY_HONORPOINT;

		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
		return;
	}

	// 농장 구입
	// ..돈 소비
	if( pPlayer->SetMoney(FARM_BUY_MONEY, MONEY_SUBTRACTION, MF_NOMAL, eItemTable_Inventory, eMoneyLog_LoseFarmBuy) != FARM_BUY_MONEY )
	{
		MSGBASE msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_MONEY_ERROR;
		msg.dwObjectID = pPlayer->GetID();
		
		pPlayer->SendMsg(&msg, sizeof(msg));
		return;
	}

	pcsFarm->SetOwnState(CSHFarmObj::OWN_STATE_OWNED);
	pcsFarm->SetOwner(pPlayer->GetID());
	pcsFarm->SetGrade(CSHFarmObj::GRADE_1);
	pcsFarm->GetGarden(0)->SetGrade(CSHFarmObj::GRADE_1);
	pcsFarm->GetHouse(0)->SetGrade(CSHFarmObj::GRADE_1);
	pcsFarm->GetWarehouse(0)->SetGrade(CSHFarmObj::GRADE_1);
	pcsFarm->GetAnimalCage(0)->SetGrade(CSHFarmObj::GRADE_1);
	pcsFarm->GetFence(0)->SetGrade(CSHFarmObj::GRADE_1);

	// DB에 저장
	SRV_SaveFarmInfoToDB((WORD)eFarmZone, nFarmID, pcsFarm);

	// 로그
	InsertLogFarm(eFarmZone, nFarmID, FARM_LOG_KIND_BUY, pPlayer->GetID());

FAILED:
	// 클라이언트에 알리기
	PACKET_FARM_STATE			stPacket;

	stPacket.Category			= MP_FARM;
	stPacket.Protocol			= MP_FARM_BUY;
	stPacket.dwObjectID 		= pPlayer->GetID();
	stPacket.nFarmZone			= (WORD)eFarmZone;
	stPacket.nFarmID			= nFarmID;
	stPacket.nFarmOwner 		= pPlayer->GetID();
	stPacket.nGardenGrade		= pcsFarm->GetGarden(0)->GetGrade();
	stPacket.nFenceGrade		= pcsFarm->GetFence(0)->GetGrade();
	stPacket.nAnimalCageGrade	= pcsFarm->GetAnimalCage(0)->GetGrade();
	stPacket.nHouseGrade		= pcsFarm->GetHouse(0)->GetGrade();
	stPacket.nWarehouseGrade	= pcsFarm->GetWarehouse(0)->GetGrade();
	stPacket.nResult			= (WORD)eResult;

	if (eResult == CSHMain::RESULT_OK)
	{
		SRV_SendFarmInfoToNearPlayer(pcsFarm, &stPacket, sizeof(stPacket));

		// 100219 ONS 농장구입시 패밀리원에게 농장정보를 전달한다.
		MSGBASE msg;
		ZeroMemory(&msg, sizeof(msg));
		msg.Category = MP_FARM;
		msg.Protocol = MP_FARM_BUY_RESULT;
		msg.dwObjectID = pPlayer->GetID();
		g_Network.Send2AgentServer( (char*)&msg, sizeof(msg) ) ;
	}
	else
	{
		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_PlantToGarden Method																								 농작물 심기
//
VOID CSHFarmManager::SRV_PlantCrop(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nGardenID, WORD nCropID, WORD nItemTargetPos, DWORD nItemIdx, int nAgentChecked)
{

#if defined(_MAPSERVER_)
	RESULT eResult = RESULT_OK;
	int nCropKind = 0;
	UINT nCropSeedGrade = 0;
	CSHFarmZone* const pFarmZone = GetFarmZone( (WORD)eFarmZone );

	if (pFarmZone == NULL) return;

	CSHFarm* const pFarm = pFarmZone->GetFarm( nFarmID );

	if( 0 == pFarm )
	{
		return;
	}

	CSHGarden* pGarden = pFarm->GetGarden(nGardenID);

	if( 0 == pGarden )
	{
		return;
	}

	// 아이템 체크
	ITEM_INFO * pItemInfo = ITEMMGR->GetUseItemInfo(pPlayer, nItemTargetPos, nItemIdx);
	if (pItemInfo == NULL || pItemInfo->SupplyType != ITEM_KIND_FARM_PLANT)
	{
		return;
	}
	nCropKind = pItemInfo->SupplyValue;
	nCropSeedGrade = pItemInfo->Grade;

	CSHCrop* pCrop = pGarden->GetCrop(nCropID);

	// 씨앗 종류 범위 체크
	if ( 0 == pCrop ||
		0 == nCropKind ||
		nCropKind > RENDER_CROP_KIND_NUM)
	{
		return;
	}

	// 씨앗 등급 범위 체크
	if (nCropSeedGrade == 0 || nCropSeedGrade > CSHCrop::CROP_SEED_GRADE_MAX)
	{
		return;
	}

	// 패밀리 상태
	// ..에이전트에서 관리하므로 에이전트로 보낸다.
	if (nAgentChecked == 0)
	{
		PACKET_FARM_GARDEN_CROP_PLANT stPacket;

		stPacket.Category 		= MP_FARM;
		stPacket.Protocol 		= MP_FARM_GARDEN_PLANT_AGENT_CHECK;
		stPacket.dwObjectID		= pPlayer->GetID();
		stPacket.nFarmZone		= (WORD)eFarmZone;
		stPacket.nFarmID		= nFarmID;
		stPacket.nGardenID		= nGardenID;
		stPacket.nCropID		= nCropID;
		stPacket.nItemTargetPos	= nItemTargetPos;
		stPacket.nItemIdx		= nItemIdx;
		stPacket.nOwner			= pFarm->GetOwner();
		// 멤버 수 체크를 위해 nResult 변수에다가...
		stPacket.nResult		= (WORD)FARM_BUY_FAMILY_MEMNUM;

		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
		return;
	}

	// 비어있는 농작물 공간인가?
	if (pCrop->GetStep() != CSHCrop::CROP_STEP_EMPTY)
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
	}

	// 텃밭의 등급보다 FARM_PLANT_GARDEN_GRADE_DIFF등급 이상의 씨앗은 심을 수 없다.
	if (nCropSeedGrade >= pGarden->GetGrade()+FARM_PLANT_GARDEN_GRADE_DIFF)
	{
		eResult = RESULT_FAIL_02;
		goto FAILED;
	}

	// 다시 심는데 필요한 시간이 있나?
	if (pPlayer->GetFarmInfo()->nCropPlantRetryTime)
	{
		eResult = RESULT_FAIL_03;
		goto FAILED;
	}

	// 거리체크..
	VECTOR3 stPlayerPos;
	pPlayer->GetPosition(&stPlayerPos);
	float nDistance = CalcDistanceXZ(pCrop->GetPos(), &stPlayerPos); 
	if (nDistance > OBJECT_CONTROL_DISTANCE)
	{
		eResult = RESULT_FAIL_04;
		goto FAILED;
	}

	// 아이템 제거
	if (ITEMMGR->UseItem(pPlayer, nItemTargetPos, nItemIdx) == eItemUseSuccess)
	{
		MSG_ITEM_USE_ACK msg;
		msg.Category	= MP_ITEM;
		msg.Protocol	= MP_ITEM_USE_ACK;
		msg.dwObjectID	= pPlayer->GetID();
		msg.TargetPos	= nItemTargetPos;
		msg.dwItemIdx	= nItemIdx;
		msg.eResult		= eItemUseSuccess;

		pPlayer->SendMsg(&msg, sizeof(msg));
	}
	else
	{
		return;
	}

	// 농작물 심기
	pCrop->Plant(pPlayer->GetID(), nCropSeedGrade, nCropKind);

	// DB에 저장
	SRV_SaveCropInfoToDB((WORD)eFarmZone, nFarmID, pCrop);

	// 다시 심는데 필요한 시간 설정
	pPlayer->SetFarmCropPlantRetryTime(CSHCrop::CROP_PLANT_RETRY_TIME);
	Farm_SetTimeDelay(pPlayer->GetID(), FARM_TIMEDELAY_KIND_PLANT, CSHCrop::CROP_PLANT_RETRY_TIME);

	// 071114 웅주, 로그
	InsertLogFarmCrop( pFarm, pCrop, eLog_FamilyFarmPutCrop );

FAILED:
	// 클라이언트에 알리기
	PACKET_FARM_GARDEN_CROP_PLANT stPacket;

	stPacket.Category			= MP_FARM;
	stPacket.Protocol			= MP_FARM_GARDEN_PLANT;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.nFarmZone			= (WORD)eFarmZone;
	stPacket.nFarmID			= nFarmID;
	stPacket.nGardenID			= nGardenID;
	stPacket.nCropID			= nCropID;
	stPacket.nCropOwner			= pPlayer->GetID();
	stPacket.nCropSeedGrade		= (WORD)nCropSeedGrade;
	stPacket.nCropKind			= (WORD)nCropKind;
	stPacket.nCropLife			= pCrop->GetLife();
	stPacket.nResult			= (WORD)eResult;

	if (eResult == CSHMain::RESULT_OK)
	{
		SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));
		// 농장UI 정보 전송
		SRV_SendFarmUIInfoToClient((WORD)eFarmZone, nFarmID, nGardenID, pCrop, pFarm->GetOwner());
	}
	else
	{
		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ManureCrop Method																								농작물 비료 주기
//
VOID CSHFarmManager::SRV_ManureCrop(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nGardenID, WORD nCropID, WORD nItemTargetPos, DWORD nItemIdx)
{
#if defined(_MAPSERVER_)
	RESULT eResult = RESULT_OK;
	int nManureKind = 0;
	int nManureGrade = 0;

	CSHFarmZone* const pFarmZone = GetFarmZone( (WORD)eFarmZone );
	if (pFarmZone == NULL) return;

	CSHFarm* const pFarm = pFarmZone->GetFarm(nFarmID);

	if ( 0 == pFarm )
	{
		return;
	}

	CSHGarden* pGarden = pFarm->GetGarden(nGardenID);

	if ( 0 == pGarden )
	{
		return;
	}

	// 아이템 체크
	ITEM_INFO * pItemInfo = ITEMMGR->GetUseItemInfo(pPlayer, nItemTargetPos, nItemIdx);
	if (pItemInfo == NULL || pItemInfo->SupplyType != ITEM_KIND_FARM_MANURE)
	{
		return;
	}
	nManureKind = pItemInfo->SupplyValue;
	nManureGrade = pItemInfo->Grade;

	// 비료 종류 범위 체크
	if (nManureKind > CSHCrop::CROP_MANURE_KIND_MAX)
	{
		return;
	}

	// 씨앗 등급 범위 체크
	if (nManureGrade > CSHCrop::CROP_SEED_GRADE_MAX)
	{
		return;
	}

	CSHCrop* pCrop = pGarden->GetCrop(nCropID);

	// 농작물이 심어져 있나?
	if ( 0 == pCrop ||
		pCrop->GetStep() < CSHCrop::CROP_STEP_1 )
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
	}

	// 농작물 주인만 비료를 줄 수 있다.
	if (pCrop->GetOwner() != pPlayer->GetID())
	{
		eResult = RESULT_FAIL_02;
		goto FAILED;
	}

	// 농작물이 다 자랐나?
	if (pCrop->GetStep() == CSHCrop::CROP_STEP_COMPLETE)
	{
		eResult = RESULT_FAIL_03;
		goto FAILED;
	}

	// 다시 비료를 주는 데 필요한 시간이 있나?
	if (pPlayer->GetFarmInfo()->nCropManureRetryTime)
	{
		eResult = RESULT_FAIL_04;
		goto FAILED;
	}

	// 거리체크..
	VECTOR3 stPlayerPos;
	pPlayer->GetPosition(&stPlayerPos);
	float nDistance = CalcDistanceXZ(pCrop->GetPos(), &stPlayerPos); 
	if (nDistance > OBJECT_CONTROL_DISTANCE)
	{
		eResult = RESULT_FAIL_05;
		goto FAILED;
	}

	// 아이템 제거
	if (ITEMMGR->UseItem(pPlayer, nItemTargetPos, nItemIdx) == eItemUseSuccess)
	{
		MSG_ITEM_USE_ACK msg;
		msg.Category	= MP_ITEM;
		msg.Protocol	= MP_ITEM_USE_ACK;
		msg.dwObjectID	= pPlayer->GetID();
		msg.TargetPos	= nItemTargetPos;
		msg.dwItemIdx	= nItemIdx;
		msg.eResult		= eItemUseSuccess;

		pPlayer->SendMsg(&msg, sizeof(msg));
	}
	else
	{
		return;
	}

	// 농작물 비료주기
	pCrop->Manure(nManureGrade, nManureKind);

	// DB에 저장
	SRV_SaveCropInfoToDB((WORD)eFarmZone, nFarmID, pCrop);

	// 다시 비료를 주는 데 필요한 시간 설정
	pPlayer->SetFarmCropManureRetryTime(CSHCrop::CROP_MANURE_RETRY_TIME);
	Farm_SetTimeDelay(pPlayer->GetID(), FARM_TIMEDELAY_KIND_MANURE, CSHCrop::CROP_MANURE_RETRY_TIME);

	// 071114 웅주, 로그
	InsertLogFarmCrop( pFarm, pCrop, eLog_FamilyFarmCareCrop );

FAILED:
	// 클라이언트에 알리기
	PACKET_FARM_GARDEN_CROP_MANURE stPacket;

	stPacket.Category			= MP_FARM;
	stPacket.Protocol			= MP_FARM_GARDEN_MANURE;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.nFarmZone			= (WORD)eFarmZone;
	stPacket.nFarmID			= nFarmID;
	stPacket.nGardenID			= nGardenID;
	stPacket.nCropID			= nCropID;
	stPacket.nCropLife			= pCrop->GetLife();
	stPacket.nManureKind		= (WORD)nManureKind;
	stPacket.nManureGrade		= (WORD)nManureGrade;
	stPacket.nResult			= (WORD)eResult;

	if (eResult == RESULT_OK)
	{
		SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));
		// 농장UI 정보 전송
		SRV_SendFarmUIInfoToClient((WORD)eFarmZone, nFarmID, nGardenID, pCrop, pFarm->GetOwner());
	}
	else
	{
		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_HarvestCrop Method																								 농작물 수확하기
//
VOID CSHFarmManager::SRV_HarvestCrop(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nGardenID, WORD nCropID, WORD nItemTargetPos, DWORD nItemIdx, int nType)
{
#if defined(_MAPSERVER_)
	RESULT eResult = RESULT_OK;

	CSHFarmZone* const pFarmZone = GetFarmZone( (WORD)eFarmZone );
	if (pFarmZone == NULL) return;

	CSHFarm* const pFarm = pFarmZone->GetFarm(nFarmID);

	if ( 0 == pFarm )
	{
		return;
	}

	CSHGarden* pGarden = pFarm->GetGarden(nGardenID);

	if ( 0 == pGarden )
	{
		return;
	}

	// 아이템 체크
	ITEM_INFO * pItemInfo = ITEMMGR->GetUseItemInfo(pPlayer, nItemTargetPos, nItemIdx);
	if (pItemInfo == NULL || pItemInfo->SupplyType != ITEM_KIND_FARM_HARVEST)
	{
		return;
	}

	CSHCrop* pCrop = pGarden->GetCrop(nCropID);

	// 농작물 주인만..
	if ( 0 == pCrop ||
		pCrop->GetOwner() != pPlayer->GetID() )
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
	}

	// 농작물이 다 자랐나?
	if (pCrop->GetStep() != CSHCrop::CROP_STEP_COMPLETE)
	{
		eResult = RESULT_FAIL_02;
		goto FAILED;
	}

	// 거리체크..
	VECTOR3 stPlayerPos;
	pPlayer->GetPosition(&stPlayerPos);
	float nDistance = CalcDistanceXZ(pCrop->GetPos(), &stPlayerPos); 
	if (nDistance > OBJECT_CONTROL_DISTANCE)
	{
		eResult = RESULT_FAIL_03;
		goto FAILED;
	}

	// 아이템 공간 체크
	CItemSlot* pSlot = pPlayer->GetSlot(eItemTable_Inventory);
	if(NULL == pSlot) ASSERTMSG(0,"올바른 아이템 슬롯을 얻을 수 없습니다.");

	// 071211 KTH -- pPlayer 추가
	DWORD nNum = ITEMMGR->GetTotalEmptySlotNum(pSlot, pPlayer);
	if(nNum == 0)
	{
		eResult = RESULT_FAIL_04;
		goto FAILED;
	}

	// 처음 수확 요청 시
	if (nType == RESULT_OK)
	{
		goto FAILED;
	}
	// 모션 종료 후 실제 수확 요청 시
	else
	{
		eResult = RESULT_OK_02;
	}

	// 아이템 제거
	if (ITEMMGR->UseItem(pPlayer, nItemTargetPos, nItemIdx) == eItemUseSuccess)
	{
		MSG_ITEM_USE_ACK msg;
		msg.Category	= MP_ITEM;
		msg.Protocol	= MP_ITEM_USE_ACK;
		msg.dwObjectID	= pPlayer->GetID();
		msg.TargetPos	= nItemTargetPos;
		msg.dwItemIdx	= nItemIdx;
		msg.eResult		= eItemUseSuccess;

		pPlayer->SendMsg(&msg, sizeof(msg));
	}
	else
	{
		return;
	}

	// 수확하기
	int nHarvestKind = (int)pCrop->Harvest();

	// 아이템 지급
	int nItemID = m_pstCropHarvestReward[pCrop->GetKind()-1].nItemID[nHarvestKind];
	int nItemNum = m_pstCropHarvestReward[pCrop->GetKind()-1].nItemNum[nHarvestKind];

	// ..유효 아이템인가?
	// ..스크립트 데이터 파일이나 기타등등의 오류로 유효하지 않은 아이템이라면 처리하지 않는다.
	ITEM_INFO* pstItem = ITEMMGR->GetItemInfo(nItemID);
	if (pstItem == NULL)
	{
		return;
	}

	// 071114 LUJ, 로그
	InsertLogFarmCrop( pFarm, pCrop, eLog_FamilyFarmHarvestCrop );

	// 수확 했으니 제거
	pCrop->SetCrop(CSHCrop::CROP_RESULT_DIE);

	// DB에 저장
	SRV_SaveCropInfoToDB((WORD)eFarmZone, nFarmID, pCrop);

	ITEMMGR->ObtainMonstermeterItem(pPlayer, nItemID, (WORD)nItemNum);
	
FAILED:
	// 클라이언트에 알리기
	PACKET_FARM_GARDEN_CROP_GROW stPacket;

	stPacket.Category			= MP_FARM;
	stPacket.Protocol			= MP_FARM_GARDEN_HARVEST;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.nFarmZone			= (WORD)eFarmZone;
	stPacket.nFarmID			= nFarmID;
	stPacket.nGardenID			= nGardenID;
	stPacket.nCropID			= nCropID;
	stPacket.nResult			= (WORD)eResult;

	if (eResult == RESULT_OK ||
		eResult == RESULT_OK_02)
	{
		SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));
		// 농장UI 정보 전송
		SRV_SendFarmUIInfoToClient((WORD)eFarmZone, nFarmID, nGardenID, pCrop, pFarm->GetOwner());
	}
	else
	{
		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_UpgradeFarm Method																								 농장 업그레이드
//
VOID CSHFarmManager::SRV_UpgradeFarm(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nObjKind, int nAgentChecked)
{
#if defined(_MAPSERVER_)
	RESULT eResult = RESULT_OK;
	CSHFarmZone* const pFarmZone = GetFarmZone( (WORD)eFarmZone );
	
	if (pFarmZone == NULL) return;

	CSHFarm* const pFarm = pFarmZone->GetFarm(nFarmID);

	if( 0 == pFarm )
	{
		return;
	}

	CSHFarmObj* pFarmObj = 0;
	switch(nObjKind)
	{
	case CSHFarm::FARM_EVENT_GARDEN:		pFarmObj = (CSHFarmObj*)pFarm->GetGarden(0);		break;
	case CSHFarm::FARM_EVENT_ANIMALCAGE:	pFarmObj = (CSHFarmObj*)pFarm->GetAnimalCage(0);	break;
	case CSHFarm::FARM_EVENT_FENCE:			pFarmObj = (CSHFarmObj*)pFarm->GetFence(0);			break;
	case CSHFarm::FARM_EVENT_HOUSE:			pFarmObj = (CSHFarmObj*)pFarm->GetHouse(0);			break;
	case CSHFarm::FARM_EVENT_WAREHOUSE:		pFarmObj = (CSHFarmObj*)pFarm->GetWarehouse(0);		break;
	}

	if( 0 == pFarmObj )
	{
		return;
	}

	int nOldGrade = pFarmObj->GetGrade();
	if (nOldGrade >= CSHFarmObj::MAX_GRADE-2)
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
	}

	if (nOldGrade >= CSHFarmObj::MAX_GRADE-4 && nObjKind == CSHFarm::FARM_EVENT_GARDEN)
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
	}
		
	// ..패밀리 상태
	if (nAgentChecked == 0)
	{
		MSG_DWORD5 stPacket;

		stPacket.Category			= MP_FARM;
		stPacket.Protocol			= MP_FARM_UPGRADE_AGENT_CHECK_1;
		stPacket.dwObjectID			= pPlayer->GetID();
		stPacket.dwData1			= (WORD)eFarmZone;
		stPacket.dwData2			= nFarmID;
		stPacket.dwData3			= nObjKind;
		stPacket.dwData4			= pFarm->GetOwner();
		stPacket.dwData5			= FARM_BUY_FAMILY_MEMNUM;
		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
		return;
	}

	// 비용 체크
	// ..골드
	MONEYTYPE nNeedMoney = m_stUpgradeCost[nObjKind-1][nOldGrade-1].nGold;
	if (pPlayer->GetMoney() < nNeedMoney)
	{
		eResult = RESULT_FAIL_02;
		goto FAILED;
	}

	// ..패밀리 명성 포인트
	if (nAgentChecked == 1)
	{
		// ..에이전트에서 관리하므로 에이전트로 체크 보냄..
		int nNeedPoint = m_stUpgradeCost[nObjKind-1][nOldGrade-1].nFamilyHonorPoint;
		MSG_DWORD5 stPacket;

		stPacket.Category			= MP_FARM;
		stPacket.Protocol			= MP_FARM_UPGRADE_AGENT_CHECK_2;
		stPacket.dwObjectID			= pPlayer->GetID();
		stPacket.dwData1			= (WORD)eFarmZone;
		stPacket.dwData2			= nFarmID;
		stPacket.dwData3			= nObjKind;
		stPacket.dwData4			= nNeedPoint;
		stPacket.dwData5			= FARM_BUY_FAMILY_MEMNUM;
		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
		return;
	}

	// ..골드 소비
	if( pPlayer->SetMoney(nNeedMoney, MONEY_SUBTRACTION, MF_NOMAL, eItemTable_Inventory, eMoneyLog_LoseFarmUpgrade) != nNeedMoney )
	{
		MSGBASE msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_MONEY_ERROR;
		msg.dwObjectID = pPlayer->GetID();
		
		pPlayer->SendMsg(&msg, sizeof(msg));
		return;
	}

	// 등급 설정
	pFarmObj->SetGrade(WORD(nOldGrade+1));

	// DB에 저장
	SRV_SaveFarmInfoToDB((WORD)eFarmZone, nFarmID, pFarm);

	// 로그
	switch(nObjKind)
	{
	case CSHFarm::FARM_EVENT_GARDEN:		InsertLogFarm(eFarmZone, nFarmID, FARM_LOG_KIND_UPGRADE, pPlayer->GetID(), FARM_UPGRADE_LOG_KIND_GARDEN,		pFarmObj->GetGrade());		break;
	case CSHFarm::FARM_EVENT_ANIMALCAGE:	InsertLogFarm(eFarmZone, nFarmID, FARM_LOG_KIND_UPGRADE, pPlayer->GetID(), FARM_UPGRADE_LOG_KIND_FENCE,			pFarmObj->GetGrade());		break;
	case CSHFarm::FARM_EVENT_FENCE:			InsertLogFarm(eFarmZone, nFarmID, FARM_LOG_KIND_UPGRADE, pPlayer->GetID(), FARM_UPGRADE_LOG_KIND_HOUSE,			pFarmObj->GetGrade());		break;
	case CSHFarm::FARM_EVENT_HOUSE:			InsertLogFarm(eFarmZone, nFarmID, FARM_LOG_KIND_UPGRADE, pPlayer->GetID(), FARM_UPGRADE_LOG_KIND_WAREHOUSE,		pFarmObj->GetGrade());		break;
	case CSHFarm::FARM_EVENT_WAREHOUSE:		InsertLogFarm(eFarmZone, nFarmID, FARM_LOG_KIND_UPGRADE, pPlayer->GetID(), FARM_UPGRADE_LOG_KIND_ANIMALCAGE,	pFarmObj->GetGrade());		break;
	default:								InsertLogFarm(eFarmZone, nFarmID, FARM_LOG_KIND_UPGRADE, pPlayer->GetID(), FARM_UPGRADE_LOG_KIND_MYSTERY,		pFarmObj->GetGrade());		break;
	}

FAILED:
	// 클라이언트에 알리기
	PACKET_FARM_UPGRADE stPacket;

	stPacket.Category			= MP_FARM;
	stPacket.Protocol			= MP_FARM_UPGRADE;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.nFarmZone			= (WORD)eFarmZone;
	stPacket.nFarmID			= nFarmID;
	stPacket.nObjKind			= nObjKind;
	stPacket.nResult			= (WORD)eResult;

	if (eResult == RESULT_OK)
	{
		SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));
	}
	else
	{
		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
	}
#endif
}

// 090707 LUJ, 아이템도 납부할 수 있도록 함
VOID CSHFarmManager::SRV_PayTax(CPlayer* pPlayer, const PACKET_FARM_TAX_SYN& packet )
{
#if defined(_MAPSERVER_)
	const CSHFarmZone::FARM_ZONE eFarmZone =  CSHFarmZone::FARM_ZONE( packet.dwData1 );
	const WORD nFarmID = WORD( packet.dwData2 );
	const DWORD nOwnerID = packet.dwData3;
	const int nKind = packet.dwData4;
	// 090707 LUJ, 입력으로 받을 수 있는 아이템 최대 입력 개수
	const int maxInputItemSize = sizeof( packet.mItem ) / sizeof( *packet.mItem );
	ITEMBASE foundTaxItemArray[ maxInputItemSize ];
	ZeroMemory( foundTaxItemArray, sizeof( foundTaxItemArray ) );
	DWORD foundTaxItemArraySize = 0;
	RESULT eResult = RESULT_OK;
	CSHFarmZone* const pFarmZone = GetFarmZone( (WORD)eFarmZone );

	if (pFarmZone == NULL) return;

	CSHFarm* const pFarm = pFarmZone->GetFarm(nFarmID);

	if( 0 == pFarm )
	{
		return;
	}

	const Tax& tax = GetTax( pFarm->GetGarden( 0 )->GetGrade() );
	const MONEYTYPE requestedTaxMoney = tax.mMoney + tax.mMoney * pFarm->GetTaxArrearageFreq() + tax.mMoney * pFarm->GetTaxArrearageFreq() * FARM_TAX_PAY_ARREARAGE_ADD_RATE / 100;

	VECTOR3 stPlayerPos = { 0 };
	pPlayer->GetPosition(&stPlayerPos);
	float nDistance = CalcDistanceXZ(pFarm->GetPos(), &stPlayerPos); 
	if (nDistance > OBJECT_CONTROL_DISTANCE*15)
	{
		return;
	}

	// 권한 확인
	// ..패밀리 멤버가 아님
	if (pFarm->GetOwner() != nOwnerID)
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
	}

	// 납부 상태 확인
	if (pFarm->IsPayTax())
	{
		eResult = RESULT_FAIL_02;
		goto FAILED;
	}

	// 먼저 클라이언트에 관리비 금액을 알리고 납부할 것인지 묻는다.
	if (nKind == 0)
	{
		PACKET_FARM_TAX_ACK stPacket;
		ZeroMemory( &stPacket, sizeof( stPacket ) );
		stPacket.Category = MP_FARM;
		stPacket.Protocol = MP_FARM_PAY_TAX;
		stPacket.dwData1 = RESULT_OK_02;
		stPacket.dwData2 = requestedTaxMoney;
		pPlayer->SendMsg(&stPacket, stPacket.GetSize() );
		return;
	}

	// 기존의 세금으로 납부할 아이템검사를 함수로 처리.
	eResult = SRV_PayItem( pPlayer, packet, tax, requestedTaxMoney );
	if( RESULT_OK != eResult )
	{
		if(RESULT_FAIL_INVALID == eResult )
		{
			return;
		}
		else
		{
			goto FAILED;
		}
	}

	// 납부 설정
	pFarm->SetTaxArrearageFreq(0);
	pFarm->SetTaxPayPlayerName(pPlayer->GetObjectName());

	// DB에 저장
	Farm_SetTaxInfo(eFarmZone, nFarmID, pFarm->GetTaxArrearageFreq(), pPlayer->GetID());
	// 081205 LUJ, 로그
	LogItemMoney(
		pPlayer->GetID(),
		pPlayer->GetObjectName(),
		0,
		"*farmTax",
		eLog_FarmTaxPaid,
		pPlayer->GetMoney(),
		0,
		requestedTaxMoney,
		0,
		0,
		0,
		0,
		0,
		0 );
	// 로그
	InsertLogFarm(eFarmZone, nFarmID, FARM_LOG_KIND_PAY_TAX, pPlayer->GetID());

FAILED:
	PACKET_FARM_TAX_ACK stPacket;
	ZeroMemory( &stPacket, sizeof( stPacket ) );
	stPacket.Category = MP_FARM;
	stPacket.Protocol = MP_FARM_PAY_TAX;
	stPacket.dwData1 = eResult;
	stPacket.dwData2 = eFarmZone;
	stPacket.dwData3 = nFarmID;
	stPacket.mTaxMoney = requestedTaxMoney;
	SafeStrCpy( stPacket.Name, pFarm->GetTaxPayPlayerName(), MAX_NAME_LENGTH + 1 );
	const DWORD itemSize = sizeof( stPacket.mItem ) / sizeof( *stPacket.mItem );
	stPacket.mSize = min( itemSize, foundTaxItemArraySize );

	for( DWORD i = 0; i < stPacket.mSize; ++i )
	{
		stPacket.mItem[ i ] = foundTaxItemArray[ i ];
	}

	pPlayer->SendMsg( &stPacket, stPacket.GetSize() );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ProcTaxCollect Method																				       농장 관리비 징수 처리
//
VOID CSHFarmManager::SRV_ProcTaxCollect()
{
#if defined(_MAPSERVER_)
	// 징수 시간, 요일이 됐나?
	if (MHTIMEMGR_OBJ->GetOldLocalTime()->wHour			!= MHTIMEMGR_OBJ->GetCurLocalTime()->wHour	&&
		MHTIMEMGR_OBJ->GetCurLocalTime()->wHour			== FARM_TAX_PAY_LIMIT_HOUR					&&
		MHTIMEMGR_OBJ->GetCurLocalTime()->wDayOfWeek	== FARM_TAX_PAY_LIMIT_DAY)
	{
		// 전에 징수한 이후로 1주가 지났다.
		m_nTaxCollectPastWeek++;

		// 다시 징수할 차례의 주가 지났나?
		if (m_nTaxCollectPastWeek >= FARM_TAX_PAY_WEEK_INTERVAL)
		{
			m_nTaxCollectPastWeek = 0;

			m_pcsFarmZone.SetPositionHead();
			for( CSHFarmZone* farmZone = 0;
				(farmZone = m_pcsFarmZone.GetData()) != NULL; )
			{
				for(int j = 0; j < farmZone->GetFarmNum(); ++j )
				{
					CSHFarm* const pFarm = farmZone->GetFarm( j );

					if( 0 == pFarm )
					{
						continue;
					}
					else if (pFarm->GetOwnState() == CSHFarmObj::OWN_STATE_EMPTY)
					{
						continue;
					}
					else if (pFarm->IsPayTax())
					{
						// 관리비를 냈다. 다음에 징수할 관리비를 위해 정보 초기화
						pFarm->SetTaxArrearageFreq(0);
						pFarm->SetTaxPayPlayerName(0);
						// DB 저장
						Farm_SetTaxInfo( farmZone->GetID(), j, pFarm->GetTaxArrearageFreq(), 0);
					}
					else
					{
						// 관리비를 내지 않았다.
						// ..허용 가능한 연체 횟수를 넘었나?
						if (pFarm->GetTaxArrearageFreq() >= FARM_TAX_PAY_ARREARAGE_MAX_FREQ)
						{
							// 더 이상 연체 불가. 농장 삭제
							SRV_DelFarmByFarmID( pFarm->GetOwner(), farmZone->GetID(), (WORD)j, FARM_DEL_KIND_TAX );
							// 관리비 정보 DB 저장
							Farm_SetTaxInfo( farmZone->GetID(), j, pFarm->GetTaxArrearageFreq(), 0 );
						}
						else
						{
							// 연체 증가
							pFarm->SetTaxArrearageFreq(pFarm->GetTaxArrearageFreq()+1);
							// DB 저장
							Farm_SetTaxInfo( farmZone->GetID(), j, pFarm->GetTaxArrearageFreq(), 0 );
						}
					}
				}
			}
		}

		// 파일에 징수 주기 저장. (2주 이상 적용 시 작업 필요)
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_NotifyTaxNonpayment Method																				   농장 관리비 미납 알림
//
VOID CSHFarmManager::SRV_NotifyTaxNonpayment(DWORD nPlayerID, CSHFarm* pcsFarm)
{
#if defined(_MAPSERVER_)
	//  관리비 납부 기한 주 중에 기한 일 하루 전, 또는 당일이면서 아직 기한 시간 전이라면 맵에 접속할 때마다 관리비 미납을 알린다.
	if (pcsFarm->IsPayTax() == FALSE &&
		m_nTaxCollectPastWeek == FARM_TAX_PAY_WEEK_INTERVAL-1)
	{
		if (FARM_TAX_PAY_NOTIFY_NONPAYMENT_DDAY == 999) return;

		BOOL bNotiryTaxPay = FALSE;
		int nTblDay[] = { 0,1,2,3,4,5,6,  0,1,2,3,4,5,6 };
		if (FARM_TAX_PAY_NOTIFY_NONPAYMENT_DDAY > 5) FARM_TAX_PAY_NOTIFY_NONPAYMENT_DDAY = 1;

		for(int i=FARM_TAX_PAY_NOTIFY_NONPAYMENT_DDAY; i>= 0; i--)
		{
			if (MHTIMEMGR_OBJ->GetCurLocalTime()->wDayOfWeek == nTblDay[7+FARM_TAX_PAY_LIMIT_DAY - i])
			{
				// 기한 당일이면 기한 시간 전까지만 알림
				if (MHTIMEMGR_OBJ->GetCurLocalTime()->wDayOfWeek == FARM_TAX_PAY_LIMIT_DAY)
				{
					if (MHTIMEMGR_OBJ->GetCurLocalTime()->wHour < FARM_TAX_PAY_LIMIT_HOUR)
					{
						bNotiryTaxPay = TRUE;
					}
				}
				else
				{
					bNotiryTaxPay = TRUE;
				}

				break;
			}
		}

		// 관리비 미납 알림
		if (bNotiryTaxPay)
		{
			MSG_DWORD stPacket;
			stPacket.Category = MP_FARM;
			stPacket.Protocol = MP_FARM_TAX_NONPAYMENT_NOTIFY;
			stPacket.dwObjectID = nPlayerID;
			stPacket.dwData		= pcsFarm->GetTaxArrearageFreq();

			g_Network.Broadcast2AgentServer((char*)&stPacket, sizeof(stPacket));
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_DelCrop Method																										 농작물 삭제
//
VOID CSHFarmManager::SRV_DelCrop(DWORD nOwnerID)
{
#if defined(_MAPSERVER_)
	// 농장맵인가?
	if (GetFarmZoneID(g_pServerSystem->GetMapNum(), NULL) == FALSE) return;

	m_pcsFarmZone.SetPositionHead();
	for( CSHFarmZone* farmZone = 0;
		(farmZone = m_pcsFarmZone.GetData() )!= NULL; )
	{
 		for( int j=0; j<farmZone->GetFarmNum(); ++j )
		{
			CSHFarm* const pFarm = farmZone->GetFarm( j );

			if( 0 == pFarm )
			{
				continue;
			}

 			for(int k=0; k<pFarm->GetGardenNum(); k++)
			{
				// 텃밭
				CSHGarden* pGarden = pFarm->GetGarden(k);
				for(int p=0; p<pGarden->GetCropNum(); p++)
				{
					// 농작물
					CSHCrop* pCrop = pGarden->GetCrop(p);
					if (pCrop->GetOwner() == nOwnerID)
					{
						// 071114 LUJ, 로그
						InsertLogFarmCrop( pFarm, pCrop, eLog_FamilyFarmLostRight );

						pCrop->AddEvent(NULL, CSHCrop::CROP_RESULT_DIE);
						pCrop->SetCrop(CSHCrop::CROP_RESULT_DIE);
					}
				}
			}
		}
	}							
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_DelFarm Method																										   농장 삭제
//
VOID CSHFarmManager::SRV_DelFarm(DWORD nOwnerID, FARM_DEL_KIND eDelKind)
{
#if defined(_MAPSERVER_)
	// 농장맵인가?
	if (GetFarmZoneID(g_pServerSystem->GetMapNum(), NULL) == FALSE) return;

	m_pcsFarmZone.SetPositionHead();
	for( CSHFarmZone* farmZone = 0;
		(farmZone = m_pcsFarmZone.GetData())!= NULL; )
	{
 		for( int j = 0; j < farmZone->GetFarmNum(); ++j )
		{
			CSHFarm* const pFarm = farmZone->GetFarm( j );

			if( 0 == pFarm )
			{
				continue;
			}
			else if (pFarm->GetOwner() == nOwnerID)
			{
				// 091126 pdy 농장 삭제가 DB에 제대로 안되는 문제수정 
				// OWN_STATE_EMPTY로 설정하면 DB에서 해당 농장이 삭제가 된다
				pFarm->SetOwnState( CSHFarmObj::OWN_STATE_EMPTY );
			
				// DB에 저장
				SRV_SaveFarmInfoToDB( farmZone->GetID(), (WORD)j, pFarm );

				// 삭제
				pFarm->Init();

				// 로그
				switch(eDelKind)
				{
				case FARM_DEL_KIND_BREAKUP_FAMILY:	InsertLogFarm(farmZone->GetID(), j, FARM_LOG_KIND_DEL_FAMILY_BREAKUP, nOwnerID);	break;
				default:							InsertLogFarm(farmZone->GetID(), j, FARM_LOG_KIND_MYSTERY, nOwnerID);				break;
				}

				// 클라이언트에 알리기
				PACKET_FARM_STATE			stPacket;

				stPacket.Category			= MP_FARM;
				stPacket.Protocol			= MP_FARM_DEL;
				stPacket.dwObjectID 		= nOwnerID;
				stPacket.nFarmZone			= farmZone->GetID();
				stPacket.nFarmID			= WORD( j );
				stPacket.nFarmState			= CSHFarm::OWN_STATE_EMPTY;
				stPacket.nFarmOwner 		= 0;
				stPacket.nGardenGrade		= 0;
				stPacket.nFenceGrade		= 0;
				stPacket.nAnimalCageGrade	= 0;
				stPacket.nHouseGrade		= 0;
				stPacket.nWarehouseGrade	= 0;
				stPacket.nResult			= (WORD)eDelKind;

				SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));

 				for(int k=0; k<pFarm->GetGardenNum(); k++)
				{
					// 텃밭
					CSHGarden* pGarden = pFarm->GetGarden(k);
					for(int p=0; p<pGarden->GetCropNum(); p++)
					{
						// 농작물
						CSHCrop* pCrop = pGarden->GetCrop(p);
						if (pCrop->GetLife())
						{
							// 071114 웅주, 로그
							InsertLogFarmCrop( pFarm, pCrop, eLog_FamilyFarmDisband );

							pCrop->SetCrop(CSHCrop::CROP_RESULT_DIE);
							SRV_SaveCropInfoToDB( farmZone->GetID(), pFarm->GetID(), pCrop );
						}
					}
				}
				// 080423 KTH
				for( int k=0; k<pFarm->GetAnimalCageNum(); k++ )
				{
					CPen* pAnimalCage = pFarm->GetAnimalCage(k);
					for( int p=0; p<pAnimalCage->GetAnimalNum(); p++ )
					{
						CAnimal* pAnimal = pAnimalCage->GetAnimal(p);
						if( pAnimal->GetLife() )
						{
							pAnimal->SetAnimal( CAnimal::ANIMAL_RESULT_DIE );
							SRV_SaveAnimalInfoToDB( farmZone->GetID(), pFarm->GetID(), pAnimal );
						}
					}
				}
			}
		}
	}							
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_DelFarmByFarmID Method																								   농장 삭제
//
VOID CSHFarmManager::SRV_DelFarmByFarmID(DWORD nOwnerID, WORD nFarmZone, WORD nFarmID, FARM_DEL_KIND eDelKind)
{
#if defined(_MAPSERVER_)
	CSHFarmZone* const pFarmZone = GetFarmZone( nFarmZone );
	if (pFarmZone == NULL) return;

	CSHFarm* const pFarm = pFarmZone->GetFarm(nFarmID);
	
	if( 0 == pFarm )
	{
		return;
	}

	// 091126 pdy 농장 삭제가 DB에 제대로 안되는 문제수정 
	// OWN_STATE_EMPTY로 설정하면 DB에서 해당 농장이 삭제가 된다
	pFarm->SetOwnState( CSHFarmObj::OWN_STATE_EMPTY );

	// DB에 저장
	SRV_SaveFarmInfoToDB(nFarmZone,	nFarmID, pFarm);

	// 삭제
	pFarm->Init();

	// 로그, 쪽지
	switch(eDelKind)
	{
	case FARM_DEL_KIND_TAX:	
		InsertLogFarm(nFarmZone, nFarmID, FARM_LOG_KIND_DEL_TAX_ARREARAGE, nOwnerID);	
		break;
	default:				
		InsertLogFarm(nFarmZone, nFarmID, FARM_LOG_KIND_MYSTERY, nOwnerID);
	}

	// 클라이언트에 알리기
	PACKET_FARM_STATE			stPacket;

	stPacket.Category			= MP_FARM;
	stPacket.Protocol			= MP_FARM_DEL;
	stPacket.dwObjectID 		= nOwnerID;
	stPacket.nFarmZone			= nFarmZone;
	stPacket.nFarmID			= nFarmID;
	stPacket.nFarmState			= CSHFarm::OWN_STATE_EMPTY;
	stPacket.nFarmOwner 		= nOwnerID;
	stPacket.nGardenGrade		= 0;
	stPacket.nFenceGrade		= 0;
	stPacket.nAnimalCageGrade	= 0;
	stPacket.nHouseGrade		= 0;
	stPacket.nWarehouseGrade	= 0;
	stPacket.nResult			= (WORD)eDelKind;

	// 주변에 있는 플레이어들에게 알리기 (오브젝트 표시 사항만)
	SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));
	// 에이전트로 Broadcast 해서 멤버들에게 알리기
	{
		g_pServerTable->SetPositionHead();
		SERVERINFO* pInfo = g_pServerTable->GetNextAgentServer();

		MSG_DWORD2 stPacket;

		stPacket.Category		= MP_FARM;
		stPacket.Protocol		= MP_FARM_DEL_NOTIFY_TO_MEMBER;
		stPacket.dwObjectID		= nOwnerID;
		stPacket.dwData1		= eDelKind;
		stPacket.dwData2		= pInfo->wServerNum;

		g_Network.Broadcast2AgentServer((char*)&stPacket, sizeof(stPacket));
	}

 	for(int k=0; k<pFarm->GetGardenNum(); k++)
	{
		// 텃밭
		CSHGarden* pGarden = pFarm->GetGarden(k);
		for(int p=0; p<pGarden->GetCropNum(); p++)
		{
			// 농작물
			CSHCrop* pCrop = pGarden->GetCrop(p);
			if (pCrop->GetLife())
			{
				switch(eDelKind)
				{
				case FARM_DEL_KIND_TAX:
					InsertLogFarmCrop( pFarm, pCrop, eLog_FamilyFarmTax );
					break;
				default:
					InsertLogFarmCrop( pFarm, pCrop, eLog_FamilyFarmMystery );
				}

				pCrop->SetCrop(CSHCrop::CROP_RESULT_DIE);
				SRV_SaveCropInfoToDB(nFarmZone, pFarm->GetID(), pCrop);
			}
		}

		// 080423 KTH
		CPen* pAnimalCage = pFarm->GetAnimalCage(k);
		for(int p=0; p<pAnimalCage->GetAnimalNum(); p++)
		{
			CAnimal* pAnimal = pAnimalCage->GetAnimal(p);
			if( pAnimal->GetLife() )
			{
				switch(eDelKind)
				{
				case FARM_DEL_KIND_TAX:
					break;
				default:
					break;
				}

				pAnimal->SetAnimal(CAnimal::ANIMAL_RESULT_DIE);
				SRV_SaveAnimalInfoToDB(nFarmZone, pFarm->GetID(), pAnimal);
			}
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SendFarmInfoToNearPlayer Method															주변의 다른 플레이어들에게 데이터 보내기
//
VOID CSHFarmManager::SRV_SendFarmInfoToNearPlayer(CSHFarm* pcsFarm, MSGBASE* pPacket, int nSize)
{
#if defined(_MAPSERVER_)
	CObject* pObject;
	g_pUserTable->SetPositionUserHead();
	while( (pObject = g_pUserTable->GetUserData() )!= NULL)
	{
		if( pObject->GetObjectKind() != eObjectKind_Player ) continue;

		VECTOR3 pos;
		pObject->GetPosition(&pos);
		float nDis1 = abs(pos.x - pcsFarm->GetPos()->x);
		float nDis2 = abs(pos.z - pcsFarm->GetPos()->z);

		nDis1 = max(nDis1, nDis2);
		if (nDis1 < CHECK_DISTANCE)
		{
			pObject->SendMsg(pPacket, nSize);
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ProcessEvent Method																									 이벤트 처리
//
VOID CSHFarmManager::SRV_ProcessEvent()
{
#if !defined(_AGENTSERVER)
	m_pcsFarmZone.SetPositionHead();
	for( CSHFarmZone* farmZone = 0;
		(farmZone = m_pcsFarmZone.GetData())!= NULL; )
	{
 		for(WORD j = 0; j < farmZone->GetFarmNum(); ++j )
		{
			CSHFarm* const pFarm = farmZone->GetFarm( j );

		#if !defined(_MAPSERVER_)
 			// 플레이어와의 거리 체크
			VECTOR3 pos = HERO->GetCurPosition();
			float nDis1 = abs(pos.x - pFarm->GetPos()->x);
			float nDis2 = abs(pos.z - pFarm->GetPos()->z);
			nDis1 = max(nDis1, nDis2);

			if (pFarm->GetIsRendering() == FALSE)
			{
				// 표시 거리 안에 들어오면 서버에 렌더링 오브젝트 추가 요청
				if (nDis1 < CHECK_DISTANCE)
				{
					pFarm->SetIsRendering(TRUE);
					CLI_RequestFarmInfo( CSHFarmZone::FARM_ZONE( farmZone->GetID() ), j );
				}
			}
			else
			{
				// 표시 거리 밖으로 나가면 렌더링 오브젝트 제거
				if (nDis1 > CHECK_DISTANCE)
				{
					pFarm->SetIsRendering(FALSE);
					CLI_RemoveRenderFarmObjFromScreen(pFarm);
				}
			}
		#endif

			// 
			for(int k=0; k<pFarm->GetAnimalCageNum(); k++)
			{
				// 축사
				CPen* pPen = pFarm->GetAnimalCage(k);

				if( 0 == pPen )
				{
					continue;
				}

				for(int p=0; p<pPen->GetAnimalNum(); p++)
				{
#ifdef _MAPSERVER_
					BOOL bSetToDB = FALSE;
#endif
					CAnimal* pAnimal = pPen->GetAnimal(p);

					if( 0 == pAnimal )
					{
						continue;
					}

					for( int q=0; q<pAnimal->GetEvent()->nEventNum; q++ )
					{
						switch( pAnimal->GetEvent()->nEvent[q])
						{
						case CAnimal::ANIMAL_RESULT_DIE:
						case CAnimal::ANIMAL_RESULT_LIFE:
						case CAnimal::ANIMAL_RESULT_CONTENTMENT:
						case CAnimal::ANIMAL_RESULT_INTEREST:
						//case CAnimal::ANIMAL_RESULT_REWARD:
							#if defined(_MAPSERVER_)
							{
								// DB 저장 설정
								bSetToDB = TRUE;
								// 클라이언트에 알리기
								PACKET_FARM_CAGE_ANIMAL_GROW stPacket;
								stPacket.Category			= MP_FARM;
								stPacket.Protocol			= MP_FARM_ANIMAL_GROW;
								stPacket.dwObjectID			= pAnimal->GetEvent()->nOwnerID;
								stPacket.nFarmZone			= farmZone->GetID();
								stPacket.nAnimalCageID		= pPen->GetID();
								stPacket.nFarmID			= pFarm->GetID();
								stPacket.nAnimalID			= pAnimal->GetID();
								stPacket.nResult			= (WORD)pAnimal->GetEvent()->nEvent[q];
								switch( pAnimal->GetEvent()->nEvent[q] )
								{
								case CAnimal::ANIMAL_RESULT_LIFE:
									stPacket.nValue			= pAnimal->GetLife();
									break;
								case CAnimal::ANIMAL_RESULT_CONTENTMENT:
									stPacket.nValue			= pAnimal->GetContentment();
									break;
								case CAnimal::ANIMAL_RESULT_INTEREST:
									stPacket.nValue			= pAnimal->GetInterest();
									break;
								}

								SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));

								// 농장UI 정보 전송
								//SRV_SendFarmUIInfoToClient(i, pFarm->GetID(), pGarden->GetID(), pCrop, pFarm->GetOwner());
							}
							#endif
							break;
						case CAnimal::ANIMAL_RESULT_IN_STALL:
							#if !defined(_MAPSERVER_)
							{
								// 플레이어와의 거리 체크
								VECTOR3 pos = HERO->GetCurPosition();
								float nDis1 = abs(pos.x - pFarm->GetPos()->x);
								float nDis2 = abs(pos.z - pFarm->GetPos()->z);
								nDis1 = max(nDis1, nDis2);

								// 체크 거리 안에 있을 경우에 렌더링 오브젝트 출력
								if (nDis1 < CHECK_DISTANCE)
								{
									pPen->GetRenderObjEx(pAnimal->GetID()+5)->GetEngineObject()->DisablePick();
									// 기존 오브젝트가 선택되어 있었다면..
									BOOL bSel = FALSE;
									if ((CObject*)pPen->GetRenderObjEx(pAnimal->GetID()+5) == OBJECTMGR->GetSelectedObject())
									{
										// 타켓 해제
 										if(OBJECTMGR->GetSelEffectHandle())
										{
											EFFECTMGR->ForcedEndEffect(OBJECTMGR->GetSelEffectHandle());
										}

										if (OBJECTMGR->GetSelectedObject())
										{
											OBJECTMGR->GetSelectedObject()->ShowObjectName( FALSE );
											OBJECTMGR->GetSelectedObject()->OnDeselected();
											OBJECTMGR->SetSelectedObjectID(0);
										}

										CLI_SetTargetObj(NULL);
										bSel = TRUE;
									}

									// ..렌더링용 오브젝트 추가
									CLI_AddRenderFarmAnimalObjToScreen(pAnimal);

									// ..이름 변경
									char* pszName = CAnimal::ANIMAL_NAME_TBL[pAnimal->GetKind()];
                                    pAnimal->GetRenderObj()->SetName(pszName);
									pAnimal->GetRenderObj()->GetObjectBalloon()->GetOBalloonName()->SetName(pszName);
									if (bSel)
									{
										CActionTarget Target;
										Target.InitActionTarget( pAnimal->GetRenderObj(), NULL );
										// 조작할 농작물 선택
										g_csFarmManager.CLI_SetTargetObj(pAnimal->GetRenderObj());
										pAnimal->GetRenderObj()->ShowObjectName( TRUE, NAMECOLOR_SELECTED );
										OBJECTMGR->SetSelectedObject( pAnimal->GetRenderObj() );
									}
									// ..다이얼로그에 이름 변경
									CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;
									if (pGuageDlg)
									{
										if ((CObject*)pAnimal->GetRenderObj() == OBJECTMGR->GetSelectedObject())
										{
											pGuageDlg->SetName( (CObject*)pAnimal->GetRenderObj(), CAnimal::ANIMAL_NAME_TBL[pAnimal->GetKind()] ) ; 
										}
									}
									// ..이펙트
									if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_ANIMAL_IN_STALL])
									{
										TARGETSET set;
										set.pTarget = pAnimal->GetRenderObj();
										EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_ANIMAL_IN_STALL], pAnimal->GetRenderObj(), &set, 0, pAnimal->GetRenderObj()->GetID());
									}
								}
							}
							#endif
							break;
						case CAnimal::ANIMAL_RESULT_REWARD:
							#if !defined(_MAPSERVER_)
							{
								CLI_RequestAnimalGetItem(RESULT_OK_02);
							}
							#endif
							break;
						}
						#if defined(_MAPSERVER_)
						if( bSetToDB )
						{
							SRV_SaveAnimalInfoToDB( farmZone->GetID(), pFarm->GetID(), pAnimal );
						}
						#endif
					}
					// 이벤트 초기화
					pAnimal->InitEvent();
				}

				// 텃밭
				CSHGarden* pGarden = pFarm->GetGarden(k);
				for(int p=0; p<pGarden->GetCropNum(); p++)
				{
					CSHCrop* pCrop = pGarden->GetCrop(p);
					for(int q=0; q<pCrop->GetEvent()->nEventNum; q++)
					{
#ifdef _MAPSERVER_
						BOOL bSetToDB = FALSE;
#endif
						switch(pCrop->GetEvent()->nEvent[q])
						{
						// 생명력 변화
						case CSHCrop::CROP_RESULT_LIFE:
							#if defined(_MAPSERVER_)
							{
								// DB 저장 설정
								bSetToDB = TRUE;
								// 클라이언트에 알리기
								PACKET_FARM_GARDEN_CROP_GROW stPacket;
								stPacket.Category			= MP_FARM;
								stPacket.Protocol			= MP_FARM_GARDEN_GROW;
								stPacket.dwObjectID			= pCrop->GetEvent()->nOwnerID;
								stPacket.nFarmZone			= farmZone->GetID();
								stPacket.nFarmID			= pFarm->GetID();
								stPacket.nGardenID			= pGarden->GetID();
								stPacket.nCropID			= pCrop->GetID();
								stPacket.nValue				= pCrop->GetLife();
								stPacket.nResult			= (WORD)pCrop->GetEvent()->nEvent[q];

								SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));

								// 농장UI 정보 전송
								SRV_SendFarmUIInfoToClient( farmZone->GetID(), pFarm->GetID(), pGarden->GetID(), pCrop, pFarm->GetOwner() );
							}
							#endif
							break;
						// 농작물 죽음
						case CSHCrop::CROP_RESULT_DIE:
							#if defined(_MAPSERVER_)
							{
								// DB 저장 설정
								bSetToDB = TRUE;
								// 클라이언트에 알리기
								PACKET_FARM_GARDEN_CROP_GROW stPacket;
								stPacket.Category			= MP_FARM;
								stPacket.Protocol			= MP_FARM_GARDEN_GROW;
								stPacket.dwObjectID			= pCrop->GetEvent()->nOwnerID;
								stPacket.nFarmZone			= farmZone->GetID();
								stPacket.nFarmID			= pFarm->GetID();
								stPacket.nGardenID			= pGarden->GetID();
								stPacket.nCropID			= pCrop->GetID();
								stPacket.nResult			= (WORD)pCrop->GetEvent()->nEvent[q];

								SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));

								// 농장UI 정보 전송
								SRV_SendFarmUIInfoToClient( farmZone->GetID(), pFarm->GetID(), pGarden->GetID(), pCrop, pFarm->GetOwner() );
							}
							#endif
							break;
						// 농작물 단계 상승
						case CSHCrop::CROP_RESULT_STEP_UP:
							#if defined(_MAPSERVER_)
							{
								// DB 저장 설정
								bSetToDB = TRUE;
								// 클라이언트에 알리기
								PACKET_FARM_GARDEN_CROP_GROW stPacket;
								stPacket.Category			= MP_FARM;
								stPacket.Protocol			= MP_FARM_GARDEN_GROW;
								stPacket.dwObjectID			= pCrop->GetEvent()->nOwnerID;
								stPacket.nFarmZone			= farmZone->GetID();
								stPacket.nFarmID			= pFarm->GetID();
								stPacket.nGardenID			= pGarden->GetID();
								stPacket.nCropID			= pCrop->GetID();
								stPacket.nResult			= (WORD)pCrop->GetEvent()->nEvent[q];

								SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));

								// 농장UI 정보 전송
								SRV_SendFarmUIInfoToClient( farmZone->GetID(), pFarm->GetID(), pGarden->GetID(), pCrop, pFarm->GetOwner() );
							}
							#endif
							break;
						// 농작물 단계 상승 완료
						case CSHCrop::CROP_RESULT_STEP_UP_COMPLETE:
							#if defined(_MAPSERVER_)
							{
								// DB 저장 설정
								bSetToDB = TRUE;
								// 클라이언트에 알리기
								PACKET_FARM_GARDEN_CROP_GROW stPacket;
								stPacket.Category			= MP_FARM;
								stPacket.Protocol			= MP_FARM_GARDEN_GROW;
								stPacket.dwObjectID			= pCrop->GetEvent()->nOwnerID;
								stPacket.nFarmZone			= farmZone->GetID();
								stPacket.nFarmID			= pFarm->GetID();
								stPacket.nGardenID			= pGarden->GetID();
								stPacket.nCropID			= pCrop->GetID();
								stPacket.nResult			= (WORD)pCrop->GetEvent()->nEvent[q];

								SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));

								// 농장UI 정보 전송
								SRV_SendFarmUIInfoToClient( farmZone->GetID(), pFarm->GetID(), pGarden->GetID(), pCrop, pFarm->GetOwner() );
							}
							#endif
							break;
						// 농작물 다음 단계 시간
						case CSHCrop::CROP_RESULT_NEXT_STEP_TIME:
							#if defined(_MAPSERVER_)
							{
								// DB 저장 설정
								bSetToDB = TRUE;
								// 클라이언트에 알리기
								PACKET_FARM_GARDEN_CROP_GROW stPacket;
								stPacket.Category			= MP_FARM;
								stPacket.Protocol			= MP_FARM_GARDEN_GROW;
								stPacket.dwObjectID			= pCrop->GetEvent()->nOwnerID;
								stPacket.nFarmZone			= farmZone->GetID();
								stPacket.nFarmID			= pFarm->GetID();
								stPacket.nGardenID			= pGarden->GetID();
								stPacket.nCropID			= pCrop->GetID();
								stPacket.nValue				= pCrop->GetNextStepTime();
								stPacket.nResult			= (WORD)pCrop->GetEvent()->nEvent[q];

								SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));

								// 농장UI 정보 전송
								SRV_SendFarmUIInfoToClient( farmZone->GetID(), pFarm->GetID(), pGarden->GetID(), pCrop, pFarm->GetOwner() );
							}
							#endif
							break;
						// 농작물 심기
						case CSHCrop::CROP_RESULT_PLANT:
							#if !defined(_MAPSERVER_)
							{
 								// 플레이어와의 거리 체크
								VECTOR3 pos = HERO->GetCurPosition();
								float nDis1 = abs(pos.x - pFarm->GetPos()->x);
								float nDis2 = abs(pos.z - pFarm->GetPos()->z);
								nDis1 = max(nDis1, nDis2);

								// 체크 거리 안에 있을 경우에 렌더링 오브젝트 출력
								if (nDis1 < CHECK_DISTANCE)
								{
									pGarden->GetRenderObjEx(pCrop->GetID()+15)->GetEngineObject()->DisablePick();
									// 기존 오브젝트가 선택되어 있었다면..
									BOOL bSel = FALSE;
									if ((CObject*)pGarden->GetRenderObjEx(pCrop->GetID()+15) == OBJECTMGR->GetSelectedObject())
									{
										// 타켓 해제
 										if(OBJECTMGR->GetSelEffectHandle())
										{
											EFFECTMGR->ForcedEndEffect(OBJECTMGR->GetSelEffectHandle());
										}

										if (OBJECTMGR->GetSelectedObject())
										{
											OBJECTMGR->GetSelectedObject()->ShowObjectName( FALSE );
											OBJECTMGR->GetSelectedObject()->OnDeselected();
											OBJECTMGR->SetSelectedObjectID(0);
										}

										CLI_SetTargetObj(NULL);
										bSel = TRUE;
									}

									// ..렌더링용 오브젝트 추가
									CLI_AddRenderFarmCropObjToScreen(pCrop);

									// ..이름 변경
									char* pszName = m_pszTblFarmCropName[CSHCrop::CROP_STEP_MAX*(pCrop->GetKind()-1)+pCrop->GetStep()-CSHCrop::CROP_STEP_1].szName;
									pCrop->GetRenderObj()->SetName(pszName);
									pCrop->GetRenderObj()->GetObjectBalloon()->GetOBalloonName()->SetName(pszName);
									if (bSel)
									{
										CActionTarget Target;
										Target.InitActionTarget( pCrop->GetRenderObj(), NULL );
										// 조작할 농작물 선택
										g_csFarmManager.CLI_SetTargetObj(pCrop->GetRenderObj());
										pCrop->GetRenderObj()->ShowObjectName( TRUE, NAMECOLOR_SELECTED );
										OBJECTMGR->SetSelectedObject( pCrop->GetRenderObj() );
									}
									// ..다이얼로그에 이름 변경
									CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;
									if (pGuageDlg)
									{
										if ((CObject*)pCrop->GetRenderObj() == OBJECTMGR->GetSelectedObject())
										{
											pGuageDlg->SetName( (CObject*)pCrop->GetRenderObj(), pszName ) ; 
										}
									}
									// ..이펙트
									if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_STEPUP])
									{
										TARGETSET set;
										set.pTarget = pCrop->GetRenderObj();
										EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_STEPUP], pCrop->GetRenderObj(), &set, 0, pCrop->GetRenderObj()->GetID());
									}
								}
							}
							#endif
							break;
						case CSHCrop::CROP_RESULT_MANURE:
							#if !defined(_MAPSERVER_)
							{
							}
							#endif
							break;
						case CSHCrop::CROP_RESULT_HARVEST:
							#if !defined(_MAPSERVER_)
							{
								CLI_RequestHarvestropInGarden(NULL, NULL, RESULT_OK_02);
							}
							#endif
							break;
						}

						// DB에 저장
					#if defined(_MAPSERVER_)
						if (bSetToDB)
						{
							SRV_SaveCropInfoToDB( farmZone->GetID(), pFarm->GetID(), pCrop );
						}
					#endif
					}
					// 이벤트 초기화
					pCrop->InitEvent();
				}
			}
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SaveFarmInfoToDB Method																					   DB에 농장 데이터 저장
//
VOID CSHFarmManager::SRV_SaveFarmInfoToDB(WORD nFarmZone, WORD nFarmID, CSHFarm* pcsFarm)
{
#if defined(_MAPSERVER_)
	char szText[256];
	sprintf(szText, "EXEC %s %d, %d, %d, %d, %d, %d, %d, %d, %d",	STORED_FARM_SET_FARMSTATE,
																	nFarmZone, 
																	nFarmID, 
																	pcsFarm->GetOwnState(), 
																	pcsFarm->GetOwner(),
																	pcsFarm->GetGarden(0)->GetGrade(),
																	pcsFarm->GetHouse(0)->GetGrade(),
																	pcsFarm->GetWarehouse(0)->GetGrade(),
																	pcsFarm->GetAnimalCage(0)->GetGrade(),
																	pcsFarm->GetFence(0)->GetGrade());

	g_DB.Query(eQueryType_FreeQuery, eFarm_SetFarmState, 0, szText);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SaveCropInfoToDB Method																					 DB에 농작물 데이터 저장
//
VOID CSHFarmManager::SRV_SaveCropInfoToDB(WORD nFarmZone, WORD nFarmID, CSHCrop* pCrop)
{
#if defined(_MAPSERVER_)
	char szText[256];
	sprintf(szText, "EXEC %s %d, %d, %d, %d, %d, %d, %d, %d, %d",	STORED_FARM_SET_CROPINFO, 
																	nFarmZone,
																	nFarmID, 
																	pCrop->GetID(),
																	pCrop->GetOwner(),
																	pCrop->GetKind(), 
																	pCrop->GetStep(), 
																	pCrop->GetLife(), 
																	pCrop->GetNextStepTime(), 
																	pCrop->GetSeedGrade());
	g_DB.Query(eQueryType_FreeQuery, eFarm_SetCropInfo, 0, szText);

	// 점검 후에 농작물이 사라진다는 제보가 있어, 파일로도 데이터를 남긴다.
	// 이 데이터와 서버를 시작할 때 DB에서 로드한 데이터와 비교한다.
	char szFile[256];
	sprintf(szFile, "./Log/FARMCROP_VERIFY_%02d%02d%02d.txt", nFarmZone, nFarmID, pCrop->GetID());
	if (pCrop->GetLife())
	{
		FILE* fp;
		fp = fopen(szFile, "wt");
		if (fp)
		{
			fprintf(fp, "%d %d %d %d %d %d\n",
				pCrop->GetOwner(),
				pCrop->GetStep(),
				pCrop->GetLife(),
				pCrop->GetSeedGrade(),
				pCrop->GetKind(),
				pCrop->GetNextStepTime());
			fclose(fp);
		}
	}
	else
	{
		DeleteFile(szFile);
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SetFarmStateFromDBQueue Method															 		  DB큐를 참조하여 농장 상태 설정
//
VOID CSHFarmManager::SRV_SetFarmStateFromDBQueue(CPlayer* pPlayer, PACKET_FARM_STATE_WITH_TAX* pPacket)
{
#if defined(_MAPSERVER_)
	CSHFarmZone* const farmZone = GetFarmZone( pPacket->nFarmZone );

	if( 0 == farmZone )
	{
		return;
	}

	CSHFarm* const pcsFarm = farmZone->GetFarm(pPacket->nFarmID);

	if( 0 == pcsFarm )
	{
		return;
	}

	pcsFarm->SetOwnState((CSHFarm::OWN_STATE)pPacket->nFarmState);
	pcsFarm->SetOwner(pPacket->nFarmOwner);
	pcsFarm->GetGarden(0)->SetGrade(pPacket->nGardenGrade);
	pcsFarm->GetHouse(0)->SetGrade(pPacket->nHouseGrade);
	pcsFarm->GetWarehouse(0)->SetGrade(pPacket->nWarehouseGrade);
	pcsFarm->GetAnimalCage(0)->SetGrade(pPacket->nAnimalCageGrade);
	pcsFarm->GetFence(0)->SetGrade(pPacket->nFenceGrade);
	pcsFarm->SetTaxArrearageFreq(pPacket->nTaxArrearageFreq);
	if (pPacket->szTaxPayPlayerName[0])
	{
		pcsFarm->SetTaxPayPlayerName(pPacket->szTaxPayPlayerName);
	}

	// 농장 로드 상태를 파일로 남긴다.
	SYSTEMTIME st;
	GetLocalTime(&st);

	char szFile[256];
	sprintf(szFile, "./Log/FARM_LOAD_START_%4d%02d%02d%02d%02d.txt", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
	FILE* fp;

	fp = fopen(szFile, "a+");
	if (fp)
	{
		fprintf(fp, "ZONE:%d FARM:%d OWNSTATE:%d OWNER:%d GARDENGRD:%d HOUSEGRD:%d WAREHOUSEGRD:%d ANIMALCAGEGRD:%d FENCEGRD:%d\n",
			pPacket->nFarmZone,
			pcsFarm->GetID(),
			pcsFarm->GetOwnState(),
			pcsFarm->GetOwner(),
			pcsFarm->GetGarden(0)->GetGrade(),
			pcsFarm->GetHouse(0)->GetGrade(),
			pcsFarm->GetWarehouse(0)->GetGrade(),
			pcsFarm->GetAnimalCage(0)->GetGrade(),
			pcsFarm->GetFence(0)->GetGrade());
		fclose(fp);
	}
#endif
}
// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SetCropInfoFromDBQueue Method																 	     DB큐를 참조하여 농작물 설정
//
VOID CSHFarmManager::SRV_SetCropInfoFromDBQueue(CPlayer* pPlayer, PACKET_FARM_GARDEN_CROP_PLANT* pPacket)
{
#if defined(_MAPSERVER_)
	CSHFarmZone* const farmZone = GetFarmZone( pPacket->nFarmZone );
	
	if( 0 == farmZone )
	{
		return;
	}

	CSHFarm* const pcsFarm = farmZone->GetFarm( pPacket->nFarmID );

	if( 0 == pcsFarm )
	{
		return;
	}

	CSHGarden* const pcsGarden = pcsFarm->GetGarden( pPacket->nGardenID );

	if( 0 == pcsGarden )
	{
		return;
	}

	CSHCrop* pcsCrop = pcsGarden->GetCrop(pPacket->nCropID);

	if( 0 == pcsCrop )
	{
		return;
	}

	pcsCrop->SetOwner(pPacket->nCropOwner);
	pcsCrop->SetStep((CSHCrop::CROP_STEP)pPacket->nCropStep);
	pcsCrop->SetLife(pPacket->nCropLife);
	pcsCrop->SetSeedGrade(pPacket->nCropSeedGrade);
	pcsCrop->SetKind(pPacket->nCropKind);
	pcsCrop->SetNextStepTime(pPacket->nCropNextStepTime);

	// 농작물 로드 상태를 파일로 남긴다.
	SYSTEMTIME st;
	GetLocalTime(&st);

	FILE* fp;
	char szFile[256];

	// 파일로 남기는 마지막 농작물의 데이터와 비교한다.
	BOOL bWrongData = FALSE;
	sprintf(szFile, "./Log/FARMCROP_VERIFY_%02d%02d%02d.txt", pPacket->nFarmZone, pPacket->nFarmID, pcsCrop->GetID());
	fp = fopen(szFile, "rt");
	if (fp)
	{
		int nOwner = 0, nStep = 0, nLife = 0, nSeedGrade = 0, nKind = 0, nNextStepTime = 0;
		fscanf(fp, "%d %d %d %d %d %d", 
			&nOwner,
			&nStep,
			&nLife,
			&nSeedGrade,
			&nKind,
			&nNextStepTime);
		fclose(fp);

		if (pcsCrop->GetOwner()			!=	(DWORD)nOwner	||
			pcsCrop->GetStep()			!=	nStep			||
			pcsCrop->GetLife()			!=	nLife			||
			pcsCrop->GetSeedGrade()		!=	nSeedGrade		||
			pcsCrop->GetKind()			!=	nKind			||
			pcsCrop->GetNextStepTime()	!=	nNextStepTime)
		{
			bWrongData = TRUE;
		}
	}
	else
	{
		// DB에서는 가져왔는 데 파일로는 안 남았다?
		bWrongData = 2;
	}

	sprintf(szFile, "./Log/FARMCROP_VERIFY_CHECK_%02d%02d%02d.txt", pPacket->nFarmZone, pPacket->nFarmID, pcsCrop->GetID());
	fp = fopen(szFile, "wt");
	if (fp)
	{
		if (bWrongData == FALSE)		fprintf(fp, "DATA_FROM_FILE and FROM_DB OK!!");
		else if (bWrongData == TRUE)	fprintf(fp, "DATA_FROM_FILE and FROM_DB DATA DIFF!!");
		else if (bWrongData == 2)		fprintf(fp, "FROM_DB Only...??");
		fclose(fp);
	}

	// DB에서 로드한 데이터 기록
	sprintf(szFile, "./Log/FARMCROP_LOAD_START_%4d%02d%02d%02d%02d.txt", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
	fp = fopen(szFile, "a+");
	if (fp)
	{
		fprintf(fp, "ZONE:%d FARM:%d CROP:%d OWNER:%d STEP:%d LIFE:%d SEEDGRD:%d KIND:%d NEXTSTEPTIME:%d\n",
			pPacket->nFarmZone,
			pcsFarm->GetID(),
			pcsCrop->GetID(),
			pcsCrop->GetOwner(),
			pcsCrop->GetStep(),
			pcsCrop->GetLife(),
			pcsCrop->GetSeedGrade(),
			pcsCrop->GetKind(),
			pcsCrop->GetNextStepTime());
		fclose(fp);
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_ParseRequestFromClient Method																 클라이언트 요청 분석/처리(에이전트)
//
VOID CSHFarmManager::ASRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength)
{
#if defined(_AGENTSERVER)
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;

	// 클라이언트에서 패킷 조작으로 남의 캐릭터 ID 를 보낼 수 있기 때문에
	// g_pUserTableForObjectID->FindUser 로 UserInfo 를 구하면 안 됨!!
	USERINFO* pUserInfo = g_pUserTable->FindUser( dwConnectionID );
	if( pUserInfo == NULL ) return;
	pTempMsg->dwObjectID = pUserInfo->dwCharacterID;

	switch( pTempMsg->Protocol )
	{
	// 아래 프로토콜은 에이전트->맵서버 이기 때문에 유저로부터의 해킹에 대비해 리턴하도록 한다.
	case MP_FARM_BUY_AGENT_CHECK:
	case MP_FARM_GARDEN_PLANT_AGENT_CHECK:
	case MP_FARM_UPGRADE_AGENT_CHECK_1:
	case MP_FARM_UPGRADE_AGENT_CHECK_2:
	// 080428 KTH 
	case MP_FARM_ANIMAL_INSTALL_AGENT_CHECK:
		return;
	case MP_FARM_PAY_TAX:
		{
			PACKET_FARM_TAX_SYN* const pPacket = ( PACKET_FARM_TAX_SYN* )pMsg;
			
			if(CSHFamily* const family = g_csFamilyManager.GetFamily(pUserInfo->mFamilyIndex))
			{
				pPacket->dwData3 = family->Get()->nMasterID;
			}
			else
			{
				pPacket->dwData3 = 0;
			}

			g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, pMsg, dwLength );
			break;
		}
		break;
	default:
		g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, pMsg, dwLength );
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_ParseRequestFromServer Method																	   서버 요청 분석/처리(에이전트)
//
VOID CSHFarmManager::ASRV_ParseRequestFromServer(DWORD dwConnectionID, char* pMsg, DWORD dwLength)
{
#if defined(_AGENTSERVER)
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;

	// 농장이 삭제되어 멤버에게 알릴 경우
	if (pTempMsg->Protocol == MP_FARM_DEL_NOTIFY_TO_MEMBER)
	{
		MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;

		// 쪽지는 에이전트 하나에서만 보내야 한당.
		if (g_pServerSystem->GetServerNum() == pPacket->dwData2)
		{
			Farm_SendNote_DelFarm(pPacket->dwObjectID);
		}

		CSHFamily* pcsFamily = g_csFamilyManager.GetFamilyByMasterID(pPacket->dwObjectID);
		if (pcsFamily)
		{
			MSG_DWORD2 stPacket;
			for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
			{
				USERINFO* pMemberInfo = g_pUserTableForObjectID->FindUser(pcsFamily->GetMember(i)->Get()->nID);
				if (pMemberInfo == NULL) continue;

				// 메세지
				memcpy(&stPacket, pMsg, dwLength);
				g_Network.Send2User( pMemberInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
		}
		return;
	}


	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pTempMsg->dwObjectID);
	if (pUserInfo == NULL)
	{
		switch( pTempMsg->Protocol )
		{
		// 농장UI 정보
		case MP_FARM_MANAGE_UI_CROP_DATA:
			{
 				PACKET_FARM_GARDEN_CROP_PLANT stPacket;
				memcpy(&stPacket, pMsg, sizeof(PACKET_FARM_GARDEN_CROP_PLANT));
				
				CSHFamily* pcsFamily = g_csFamilyManager.GetFamilyByMasterID(stPacket.nOwner);
				if (pcsFamily)
				{
					for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
					{
						USERINFO* pMemberInfo = g_pUserTableForObjectID->FindUser(pcsFamily->GetMember(i)->Get()->nID);
						if (pMemberInfo == NULL) continue;

						memcpy(&stPacket, pMsg, sizeof(PACKET_FARM_GARDEN_CROP_PLANT));
						g_Network.Send2User( pMemberInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
					}
				}
				break;
			}
		}
		return;
	}

	switch( pTempMsg->Protocol )
	{
	// 농장 구입
	case MP_FARM_BUY_AGENT_CHECK:
		{
			RESULT eResult = RESULT_OK;
 			MSG_DWORD4* pPacket = (MSG_DWORD4*)pMsg;
			CSHFamily* const family = g_csFamilyManager.GetFamily(
				pUserInfo->mFamilyIndex);

			if(0 == family)
			{
				eResult = RESULT_FAIL_05;
				goto FAILED_BUY;
			}
			else if (pUserInfo->dwCharacterID != family->Get()->nMasterID)
			{
				eResult = RESULT_FAIL_06;
				goto FAILED_BUY;
			}
			else if(family->Get()->nMemberNum < pPacket->dwData3)
			{
				eResult = RESULT_FAIL_07;
				goto FAILED_BUY;
			}
			else if(family->GetEx()->nHonorPoint < pPacket->dwData4)
			{
				eResult = RESULT_FAIL_08;
				goto FAILED_BUY;
			}

FAILED_BUY:
			PACKET_FARM_STATE			stPacket;

			stPacket.Category			= MP_FARM;
			stPacket.Protocol			= MP_FARM_BUY_AGENT_CHECK;
			stPacket.dwObjectID 		= pUserInfo->dwCharacterID;
			stPacket.nFarmZone			= (WORD)pPacket->dwData1;
			stPacket.nFarmID			= (WORD)pPacket->dwData2;
			stPacket.nResult			= (WORD)eResult;

			if (eResult == RESULT_OK)
			{
				// 명예 포인트 절감
				g_csFamilyManager.ASRV_UpdateHonorPoint(pUserInfo, (DWORD)(-1*(int)pPacket->dwData4));
				g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
			else
			{
				stPacket.Protocol = MP_FARM_BUY;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
			break;
		}
	// 농작물 심기
	case MP_FARM_GARDEN_PLANT_AGENT_CHECK:
		{
			RESULT eResult = RESULT_OK;
 			PACKET_FARM_GARDEN_CROP_PLANT* pPacket = (PACKET_FARM_GARDEN_CROP_PLANT*)pMsg;
			CSHFamily* const family = g_csFamilyManager.GetFamily(
				pUserInfo->mFamilyIndex);

			if(0 == family)
			{
				eResult = RESULT_FAIL_05;
				goto FAILED_PLANT;
			}
			else if(family->Get()->nMasterID != pPacket->nOwner)
			{
				eResult = RESULT_FAIL_06;
				goto FAILED_PLANT;
			}
			else if (family->Get()->nMemberNum < pPacket->nResult)
			{
				eResult = RESULT_FAIL_07;
				goto FAILED_PLANT;
			}

FAILED_PLANT:
			PACKET_FARM_GARDEN_CROP_PLANT stPacket;

			stPacket.Category			= MP_FARM;
			stPacket.Protocol			= MP_FARM_GARDEN_PLANT_AGENT_CHECK;
			stPacket.dwObjectID 		= pUserInfo->dwCharacterID;
			stPacket.nFarmZone			= pPacket->nFarmZone;
			stPacket.nFarmID			= pPacket->nFarmID;
			stPacket.nGardenID			= pPacket->nGardenID;
			stPacket.nCropID			= pPacket->nCropID;
			stPacket.nItemTargetPos		= pPacket->nItemTargetPos;
			stPacket.nItemIdx			= pPacket->nItemIdx;
			stPacket.nResult			= (WORD)eResult;

			if (eResult == RESULT_OK)
			{
				g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
			else
			{
				stPacket.Protocol = MP_FARM_GARDEN_PLANT;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
			break;
		}

	// 농장 업그레이드1
	case MP_FARM_UPGRADE_AGENT_CHECK_1:
		{
			RESULT eResult = RESULT_OK;
 			MSG_DWORD5* pPacket = (MSG_DWORD5*)pMsg;
			CSHFamily* const family = g_csFamilyManager.GetFamily(
				pUserInfo->mFamilyIndex);

			if(0 == family)
			{
				eResult = RESULT_FAIL_03;
				goto FAILED_UPGRADE_1;
			}
			else if(pUserInfo->dwCharacterID != family->Get()->nMasterID ||
				pUserInfo->dwCharacterID != pPacket->dwData4 )
			{
				eResult = RESULT_FAIL_04;
				goto FAILED_UPGRADE_1;
			}
			else if(family->Get()->nMemberNum < pPacket->dwData5)
			{
				eResult = RESULT_FAIL_06;
				goto FAILED_UPGRADE_1;
			}

FAILED_UPGRADE_1:
			PACKET_FARM_UPGRADE			stPacket;

			stPacket.Category			= MP_FARM;
			stPacket.Protocol			= MP_FARM_UPGRADE_AGENT_CHECK_1;
			stPacket.dwObjectID 		= pUserInfo->dwCharacterID;
			stPacket.nFarmZone			= (WORD)pPacket->dwData1;
			stPacket.nFarmID			= (WORD)pPacket->dwData2;
			stPacket.nObjKind			= (WORD)pPacket->dwData3;
			stPacket.nResult			= (WORD)eResult;

			if (eResult == RESULT_OK)
			{
				g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
			else
			{
				stPacket.Protocol = MP_FARM_UPGRADE;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
			break;
		}
	// 농장 업그레이드2
	case MP_FARM_UPGRADE_AGENT_CHECK_2:
		{
			RESULT eResult = RESULT_OK;
 			MSG_DWORD5* pPacket = (MSG_DWORD5*)pMsg;
			CSHFamily* const family = g_csFamilyManager.GetFamily(
				pUserInfo->mFamilyIndex);

			if(0 == family)
			{
				eResult = RESULT_FAIL_03;
				goto FAILED_UPGRADE_2;
			}
			else if(pUserInfo->dwCharacterID != family->Get()->nMasterID)
			{
				eResult = RESULT_FAIL_04;
				goto FAILED_UPGRADE_2;
			}
			else if(family->GetEx()->nHonorPoint < pPacket->dwData4)
			{
				eResult = RESULT_FAIL_05;
				goto FAILED_UPGRADE_2;
			}
			else if(family->Get()->nMemberNum < pPacket->dwData5)
			{
				eResult = RESULT_FAIL_06;
				goto FAILED_UPGRADE_2;
			}

FAILED_UPGRADE_2:
			PACKET_FARM_UPGRADE			stPacket;

			stPacket.Category			= MP_FARM;
			stPacket.Protocol			= MP_FARM_UPGRADE_AGENT_CHECK_2;
			stPacket.dwObjectID 		= pUserInfo->dwCharacterID;
			stPacket.nFarmZone			= (WORD)pPacket->dwData1;
			stPacket.nFarmID			= (WORD)pPacket->dwData2;
			stPacket.nObjKind			= (WORD)pPacket->dwData3;
			stPacket.nResult			= (WORD)eResult;

			if (eResult == RESULT_OK)
			{
				// 명예 포인트 절감
				g_csFamilyManager.ASRV_UpdateHonorPoint(pUserInfo, (DWORD)(-1*(int)pPacket->dwData4));
				g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
			else
			{
				stPacket.Protocol = MP_FARM_UPGRADE;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
			break;
		}
	// 농장UI 정보
	case MP_FARM_MANAGE_UI_CROP_DATA:
		{
 			PACKET_FARM_GARDEN_CROP_PLANT stPacket;
			memcpy(&stPacket, pMsg, sizeof(PACKET_FARM_GARDEN_CROP_PLANT));
			
			if (stPacket.nOwner == NULL)
			{
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pMsg, dwLength );
			}
			else
			{
				CSHFamily* pcsFamily = g_csFamilyManager.GetFamilyByMasterID(stPacket.nOwner);
				if (pcsFamily)
				{
					for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
					{
						USERINFO* pMemberInfo = g_pUserTableForObjectID->FindUser(pcsFamily->GetMember(i)->Get()->nID);
						if (pMemberInfo == NULL) continue;

						memcpy(&stPacket, pMsg, sizeof(PACKET_FARM_GARDEN_CROP_PLANT));
						g_Network.Send2User( pMemberInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
					}
				}
			}
			break;
		}
	// 관리비
	case MP_FARM_PAY_TAX:
		{
			const PACKET_FARM_TAX_ACK* const pPacket = ( PACKET_FARM_TAX_ACK* )pMsg;

			if (pPacket->dwData1 == RESULT_OK)
			{
				if(CSHFamily* const pcsFamily = g_csFamilyManager.GetFamily(pUserInfo->mFamilyIndex))
				{
					for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
					{
						USERINFO* pMemberInfo = g_pUserTableForObjectID->FindUser(pcsFamily->GetMember(i)->Get()->nID);
						if (pMemberInfo == NULL) continue;

						PACKET_FARM_TAX_ACK stPacket = *pPacket;
						g_Network.Send2User( pMemberInfo->dwConnectionIndex, (char*)&stPacket, dwLength );
					}
				}
			}
			else
			{
				g_Network.Send2User( pUserInfo->dwConnectionIndex, pMsg, dwLength );
			}
			break;
		}
	// 가축 넣기
	case MP_FARM_ANIMAL_INSTALL_AGENT_CHECK:
		{
			RESULT eResult = RESULT_OK;
 			PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT* pPacket = (PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT*)pMsg;
			CSHFamily* const family = g_csFamilyManager.GetFamily(
				pUserInfo->mFamilyIndex);

			if(0 == family)
			{
				eResult = RESULT_FAIL_05;
				goto FAILED_INSTALL_ANIMAL;
			}
			else if(family->Get()->nMasterID != pPacket->nOwner)
			{
				eResult = RESULT_FAIL_06;
				goto FAILED_INSTALL_ANIMAL;
			}
			else if(family->Get()->nMemberNum < pPacket->nResult)
			{
				eResult = RESULT_FAIL_07;
				goto FAILED_INSTALL_ANIMAL;
			}

FAILED_INSTALL_ANIMAL:
			PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT stPacket;

			stPacket.Category			= MP_FARM;
			stPacket.Protocol			= MP_FARM_ANIMAL_INSTALL_AGENT_CHECK;
			stPacket.dwObjectID 		= pUserInfo->dwCharacterID;
			stPacket.nFarmZone			= pPacket->nFarmZone;
			stPacket.nFarmID			= pPacket->nFarmID;
			stPacket.nAnimalCageID		= pPacket->nAnimalCageID;
			stPacket.nAnimalID			= pPacket->nAnimalID;
			stPacket.nItemTargetPos		= pPacket->nItemTargetPos;
			stPacket.nItemIdx			= pPacket->nItemIdx;
			stPacket.nAnimalContentment = pPacket->nAnimalContentment;
			stPacket.nAnimalInterest	= pPacket->nAnimalInterest;
			stPacket.nResult			= (WORD)eResult;

			if (eResult == RESULT_OK)
			{
				g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
			else
			{
				stPacket.Protocol = MP_FARM_ANIMAL_INSTALL;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
			break;
		}
	// 091204 ONS 다른맵에서 변경된 농장세금관련 정보를 농장맵으로 전달한다.
	case MP_FARM_SETFARM_TAXINFO_TO_FARMMAP:
		{
			MSG_DWORD3_NAME* pmsg = (MSG_DWORD3_NAME*)pMsg;			
			WORD wFarmZone = (WORD)pmsg->dwData1;
			WORD wMapport = g_pServerTable->GetServerPort( eSK_MAP, (WORD)(Alker_Farm + wFarmZone) );
			if( wMapport )
			{
				SERVERINFO* pSInfo	= g_pServerTable->FindServer( wMapport );
				if( !pSInfo )			break;

				MSG_DWORD3_NAME msg;
				ZeroMemory( &msg, sizeof(MSG_DWORD3_NAME) );

				msg.Category	= MP_FARM;
				msg.Protocol	= MP_FARM_SETFARM_TAXINFO;
				msg.dwObjectID	= pmsg->dwObjectID;
				msg.dwData1		= pmsg->dwData1;
				msg.dwData2		= pmsg->dwData2;
				msg.dwData3		= pmsg->dwData3;
				SafeStrCpy(msg.Name, pmsg->Name, MAX_NAME_LENGTH+1);
				g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_DWORD3_NAME) );
			}
		}
		break;
	// 100219 ONS 농장구입시 패밀리원에게 농장정보를 전달한다.
	case MP_FARM_BUY_RESULT:
		{
			CSHFamily* const family = g_csFamilyManager.GetFamily(
				pUserInfo->mFamilyIndex);

			if(0 == family)
			{
				break;
			}

			for(UINT i = 0; i < family->Get()->nMemberNum; ++i)
			{
				ASRV_RequestFarmUIInfoToMap(
					family->GetMember(i)->Get()->nID,
					family);
			}
		}
		break;
	default:
		g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pMsg, dwLength );
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_RequestFarmUIInfoToMap Method																		  	맵서버에 농장UI정보 요청
//
VOID CSHFarmManager::ASRV_RequestFarmUIInfoToMap(DWORD nPlayerID, CSHFamily* pcsFamily)
{
#if defined(_AGENTSERVER)
	MSG_DWORD stPacket;

	stPacket.Category	= MP_FARM;
	stPacket.Protocol	= MP_FARM_MANAGE_UI_FARM_DATA;
	stPacket.dwObjectID	= nPlayerID;
	stPacket.dwData		= pcsFamily->Get()->nMasterID;
	g_Network.Broadcast2MapServer((char*)&stPacket, sizeof(stPacket));
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ParseRequestFromClient Method																			클라이언트 요청 분석/처리
//
VOID CSHFarmManager::SRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength)
{
#if defined(_MAPSERVER_)
	MSGBASE* pPacket = (MSGBASE*)pMsg;

	// 잘못된 플레이어 처리
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pPacket->dwObjectID );
	if( !pPlayer )	
	{
		switch(pPacket->Protocol)
		{
		// 농장UI 정보 - 농장 맵에서 데이터를 보내줘야 하는데, 접속 플레이어가 농장이 아닌 다른 맵에 접속 했을 수 있다. 
		case MP_FARM_MANAGE_UI_FARM_DATA:
			{
				MSG_DWORD* pPacket = (MSG_DWORD*)pMsg;
				// 농장 관리비 미납을 알리기 위해 FARM_UI_NOTIFY_KIND_CONN_MAP 를 설정.
				// 정확하게는 맵 접속 시, 농장 구입 시에 관리비 미납 상태를 알리는 것이 맞지만,
				// 농장 구입 후 패밀리 멤버 초대 완료 시에도 이곳이 수행되기도 한다.
				// 추가 작업을 하기엔 크게 중요하지 않은 사항이기에 그냥 이렇게 처리.
				SRV_SendFarmUIInfoToClient(pPacket->dwObjectID, pPacket->dwData, FARM_UI_NOTIFY_KIND_CONN_MAP);
			}
			break;
		// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 추가 : 변경된 농장정보를 설정한다.
		case MP_FARM_SETFARM_TAXINFO:
			{
				MSG_DWORD3_NAME* pPacket = (MSG_DWORD3_NAME*)pMsg;
				SRV_SetFarmTaxInfo( (CSHFarmZone::FARM_ZONE)pPacket->dwData1, pPacket->dwData2, pPacket->dwData3, pPacket->Name );
			}
			break;
		// 091207 ONS 패밀리 마스터 이양시 농장 소유주도 변경한다.
		case MP_FARM_SETFARM_CHANGE_OWNER:
			{
				MSG_DWORD3* pPacket = (MSG_DWORD3*)pMsg;
				SRV_SetFarmChangeOwner( (CSHFarmZone::FARM_ZONE)pPacket->dwData1, pPacket->dwData2, pPacket->dwData3 );
			}
			break;
		}
		return;
	}

	switch(pPacket->Protocol)
	{
	// 농장 구입
	case MP_FARM_BUY:
		{
			PACKET_FARM_STATE* pPacket = (PACKET_FARM_STATE*)pMsg;

			// 농장구입은 1분에 10회 제한이 있다. 
			stBuyFarmWaitInfo* pWaitInfo = m_BuyFarm_WaitInfoList.GetData( pPlayer->GetID() ) ; 

			if( pWaitInfo == NULL )
			{
				// 대기리스트에 ADD 
				pWaitInfo = new stBuyFarmWaitInfo();
				ZeroMemory( pWaitInfo , sizeof( stBuyFarmWaitInfo ) ) ;

				pWaitInfo->dwPlayerIdx		=	pPacket->dwObjectID ;
				pWaitInfo->dwRequestTime	=	gCurTime;

				m_BuyFarm_WaitInfoList.Add( pWaitInfo , pWaitInfo->dwPlayerIdx );
			}

			pWaitInfo->nFarmID			=	pPacket->nFarmID ;
			pWaitInfo->nFarmZone		=	pPacket->nFarmZone;

			if( pWaitInfo->bQueryToDB )
			{
				//쿼리중이라면 카운트를 증가하지 않고 리턴 
				return;
			}

			if(  gCurTime - pWaitInfo->dwRequestTime  > 60000 )
			{
				// 지난 요청시간이 1분을 넘겼다면 재활용
				pWaitInfo->dwRequestCount = 0 ;
				pWaitInfo->dwRequestTime = gCurTime ;
			}

			pWaitInfo->dwRequestCount = pWaitInfo->dwRequestCount + 1;

			if( pWaitInfo->dwRequestCount > 10 )
			{
				// 1분에 10회를 넘겼다면 실패
				PACKET_FARM_STATE			stPacket;
				ZeroMemory( &stPacket , sizeof( PACKET_FARM_STATE ) );

				stPacket.Category			= MP_FARM;
				stPacket.Protocol			= MP_FARM_BUY;
				stPacket.dwObjectID 		= pPlayer->GetID();
				stPacket.nFarmZone			= pWaitInfo->nFarmZone;
				stPacket.nFarmID			= pWaitInfo->nFarmID;
				stPacket.nResult			= CSHMain::RESULT_FAIL_09;

				pPlayer->SendMsg(&stPacket, sizeof(stPacket));

				return;
			}

			SRV_BuyFarm(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID);
		}
		break;
	// 농장 구입 - 에이전트 체크 후
	case MP_FARM_BUY_AGENT_CHECK:
		{
			stBuyFarmWaitInfo* pWaitInfo = m_BuyFarm_WaitInfoList.GetData( pPlayer->GetID() ) ; 

			if( !pWaitInfo || pWaitInfo->bQueryToDB == TRUE )
			{
				// pWaitInfo 없다면 정상적이지 않은 요청이다. 
				// 쿼리중에 에이전트 체크가 온다면 중복 요청일것이다. 처리하지 말자
				return;
			}

			g_DB.FreeMiddleQuery(
				RCheckHaveFarmNumToDB,
				pPlayer->GetID(),
				"SELECT* FROM TB_FARM_FARM WHERE FarmOwnerID = %d",
				pPlayer->GetID() );

			pWaitInfo->bQueryToDB = TRUE ;
		}
		break;
	// 농작물 심기
	case MP_FARM_GARDEN_PLANT:
		{
			PACKET_FARM_GARDEN_CROP_PLANT* pPacket = (PACKET_FARM_GARDEN_CROP_PLANT*)pMsg;
			SRV_PlantCrop(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID, pPacket->nGardenID, pPacket->nCropID, pPacket->nItemTargetPos, pPacket->nItemIdx);
		}
		break;
	// 농작물 심기 - 에이전트 체크 후
	case MP_FARM_GARDEN_PLANT_AGENT_CHECK:
		{
			PACKET_FARM_GARDEN_CROP_PLANT* pPacket = (PACKET_FARM_GARDEN_CROP_PLANT*)pMsg;
			SRV_PlantCrop(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID, pPacket->nGardenID, pPacket->nCropID, pPacket->nItemTargetPos, pPacket->nItemIdx, 1);
		}
		break;
	// 농작물 비료주기
	case MP_FARM_GARDEN_MANURE:
		{
			PACKET_FARM_GARDEN_CROP_MANURE* pPacket = (PACKET_FARM_GARDEN_CROP_MANURE*)pMsg;
			SRV_ManureCrop(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID, pPacket->nGardenID, pPacket->nCropID, pPacket->nItemTargetPos, pPacket->nItemIdx);
		}
		break;
	// 농작물 수확하기
	case MP_FARM_GARDEN_HARVEST:
		{
			PACKET_FARM_GARDEN_CROP_GROW* pPacket = (PACKET_FARM_GARDEN_CROP_GROW*)pMsg;
			SRV_HarvestCrop(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID, pPacket->nGardenID, pPacket->nCropID, pPacket->nItemTargetPos, pPacket->nItemIdx, pPacket->nResult);
		}
		break;
	// 농장 등급 업그레이드
	case MP_FARM_UPGRADE:
		{
			PACKET_FARM_UPGRADE* pPacket = (PACKET_FARM_UPGRADE*)pMsg;
			SRV_UpgradeFarm(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID, pPacket->nObjKind);
		}
		break;
	// 농장 등급 업그레이드 - 에이전트 체크1 후
	case MP_FARM_UPGRADE_AGENT_CHECK_1:
		{
			PACKET_FARM_UPGRADE* pPacket = (PACKET_FARM_UPGRADE*)pMsg;
			SRV_UpgradeFarm(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID, pPacket->nObjKind, 1);
		}
		break;
	// 농장 등급 업그레이드 - 에이전트 체크2 후
	case MP_FARM_UPGRADE_AGENT_CHECK_2:
		{
			PACKET_FARM_UPGRADE* pPacket = (PACKET_FARM_UPGRADE*)pMsg;
			SRV_UpgradeFarm(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID, pPacket->nObjKind, 2);
		}
		break;
	// 농장 정보
	case MP_FARM_FARMSTATE_LOAD_FROM_DB:
		{
			PACKET_FARM_STATE* pPacket = (PACKET_FARM_STATE*)pMsg;
			SRV_SendFarmInfoToClient(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID);
		}
		break;
	// 농장UI 정보
	case MP_FARM_MANAGE_UI_FARM_DATA:
		{
			MSG_DWORD* pPacket = (MSG_DWORD*)pMsg;
			// 농장 관리비 미납을 알리기 위해 FARM_UI_NOTIFY_KIND_CONN_MAP 를 설정.
			// 정확하게는 맵 접속 시, 농장 구입 시에 관리비 미납 상태를 알리는 것이 맞지만,
			// 농장 구입 후 패밀리 멤버 초대 완료 시에도 이곳이 수행되기도 한다.
			// 추가 작업을 하기엔 크게 중요하지 않은 사항이기에 그냥 이렇게 처리.
			SRV_SendFarmUIInfoToClient(pPacket->dwObjectID, pPacket->dwData, FARM_UI_NOTIFY_KIND_CONN_MAP);
		}
		break;
	// 관리비 납부
	case MP_FARM_PAY_TAX:
		{
			const PACKET_FARM_TAX_SYN* const taxPacket = ( PACKET_FARM_TAX_SYN* )pPacket;
			DWORD dwKind = taxPacket->dwData4;
			if( 0 == dwKind || 1 == dwKind )
			{
				m_ePayTaxMode = ePayTaxMode_FarmNpc;
				SRV_PayTax(pPlayer, *taxPacket );
			}
			else
			{
				// 패밀리 다이얼로그의 "세금"버튼으로 농장 관리비를 납부할경우 처리.
				// 농장맵 이외의 맵에서 납부가 가능해야하기때문에 기존의 처리와 구분해서 처리.
				m_ePayTaxMode = ePayTaxMode_PayBtn;
				SRV_PayTaxFromFamilyDialog(pPlayer, *taxPacket );
			}
		}
		break;
	// 080415 KTH
	// 축사에 가축 넣기
	case MP_FARM_ANIMAL_INSTALL:
		{
			PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT* pPacket = (PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT*)pMsg;
			SRV_InstallAnimalInAnimalCage(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID, pPacket->nAnimalCageID, pPacket->nAnimalID, pPacket->nItemTargetPos, pPacket->nItemIdx, pPacket->nAnimalContentment, pPacket->nAnimalInterest);
		}
		break;
	// 축사에 가축 넣기 에어전트 체크
	case MP_FARM_ANIMAL_INSTALL_AGENT_CHECK:
		{
			PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT* pPacket = (PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT*)pMsg;
			SRV_InstallAnimalInAnimalCage(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID, pPacket->nAnimalCageID, pPacket->nAnimalID, pPacket->nItemTargetPos, pPacket->nItemIdx, pPacket->nAnimalContentment, pPacket->nAnimalInterest, 1);
		}
		break;
	// 가축 먹이 주기
	case MP_FARM_ANIMAL_FEED:
		{
			PACKET_FARM_CAGE_ANIMAL_FEED* pPacket = (PACKET_FARM_CAGE_ANIMAL_FEED*)pMsg;
			SRV_FeedAnimal(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID, pPacket->nAnimalCageID, pPacket->nAnimalID, pPacket->nItemTargetPos, pPacket->nItemIdx);
		}
		break;
	// 축사 청소
	case MP_FARM_ANIMAL_CLEANING:
		{
			PACKET_FARM_CAGE_ANIMAL_FEED* pPacket = (PACKET_FARM_CAGE_ANIMAL_FEED*)pMsg;
			SRV_CleaningAnimal(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID, pPacket->nAnimalCageID, pPacket->nAnimalID, pPacket->nItemTargetPos, pPacket->nItemIdx);
		}
		break;
	// 가축 보상
	case MP_FARM_ANIMAL_REWARD:
		{
			PACKET_FARM_CAGE_ANIMAL_REWARD* pPacket = (PACKET_FARM_CAGE_ANIMAL_REWARD*)pMsg;
			SRV_AnimalGetItem(pPlayer, (CSHFarmZone::FARM_ZONE)pPacket->nFarmZone, pPacket->nFarmID, pPacket->nAnimalCageID, pPacket->nAnimalID, pPacket->nResult);
		}
		break;
	// 091207 ONS 패밀리 마스터 이양시 농장 소유주도 변경한다.
	case MP_FARM_SETFARM_CHANGE_OWNER:
		{
			MSG_DWORD3* pPacket = (MSG_DWORD3*)pMsg;
			SRV_SetFarmChangeOwner( (CSHFarmZone::FARM_ZONE)pPacket->dwData1, pPacket->dwData2, pPacket->dwData3 );
		}
		break;
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SendFarmInfoToClient Method																		 클라이언트에게 농장 정보 보내기
//
VOID CSHFarmManager::SRV_SendFarmInfoToClient(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID)
{
#if defined(_MAPSERVER_)
	CSHFarmZone* const pFarmZone = GetFarmZone( (WORD)eFarmZone );

	// 090520 LUJ, 검사 코드 추가
	if( 0 == pFarmZone )
	{
		return;
	}

	CSHFarm* const pFarm = pFarmZone->GetFarm( nFarmID );

	if( 0 == pFarm )
	{
		return;
	}

	CSHFarmManager::PACKET_FARM_STATE				stPacket;
	CSHFarmManager::PACKET_FARM_GARDEN_CROP_PLANT	stPacket2;
	CSHFarmManager::PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT	stPacket3;

	stPacket.Category			= MP_FARM;
	stPacket.Protocol			= MP_FARM_FARMSTATE_LOAD_FROM_DB;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.nFarmZone			= (WORD)eFarmZone;
	stPacket.nFarmID			= pFarm->GetID();
	stPacket.nFarmState			= (WORD)pFarm->GetOwnState();
	stPacket.nFarmOwner			= pFarm->GetOwner();
	stPacket.nGardenGrade		= pFarm->GetGarden(0)->GetGrade();
	stPacket.nHouseGrade		= pFarm->GetHouse(0)->GetGrade();
	stPacket.nWarehouseGrade	= pFarm->GetWarehouse(0)->GetGrade();
	stPacket.nAnimalCageGrade	= pFarm->GetAnimalCage(0)->GetGrade();
	stPacket.nFenceGrade		= pFarm->GetFence(0)->GetGrade();

	pPlayer->SendMsg(&stPacket, sizeof(stPacket));

	for(int k=0; k<pFarm->GetGardenNum(); k++)
	{
		CSHGarden* pGarden = pFarm->GetGarden(k);
		for(int p=0; p<pGarden->GetCropNum(); p++)
		{
			CSHCrop* pCrop = pGarden->GetCrop(p);

			stPacket2.Category 			= MP_FARM;
			stPacket2.Protocol 			= MP_FARM_CROPINFO_LOAD_FROM_DB;
			stPacket2.dwObjectID		= pPlayer->GetID();
			stPacket2.nFarmZone			= (WORD)eFarmZone;
			stPacket2.nFarmID			= pFarm->GetID();
			stPacket2.nGardenID			= pGarden->GetID();
			stPacket2.nCropID			= pCrop->GetID();
			stPacket2.nCropOwner		= pCrop->GetOwner();
			stPacket2.nCropKind			= (WORD)pCrop->GetKind();
			stPacket2.nCropStep			= (WORD)pCrop->GetStep();
			stPacket2.nCropLife			= pCrop->GetLife();
			stPacket2.nCropNextStepTime	= pCrop->GetNextStepTime();
			stPacket2.nCropSeedGrade	= pCrop->GetSeedGrade();

			pPlayer->SendMsg(&stPacket2, sizeof(stPacket2));
		}
	}

	// 080417 KTH -- 축사 정보를 전송
	for(int k = 0; k < pFarm->GetAnimalCageNum(); k++)
	{
		CPen* pAnimalCage = pFarm->GetAnimalCage(k);

		for(int p = 0; p < pAnimalCage->GetAnimalNum(); p++)
		{
			CAnimal* pAnimal = pAnimalCage->GetAnimal(p);

			stPacket3.Category				= MP_FARM;
			stPacket3.Protocol				= MP_FARM_ANIMALINFO_LOAD_FROM_DB;
			stPacket3.dwObjectID			= pPlayer->GetID();
			stPacket3.nFarmZone				= (WORD)eFarmZone;
			stPacket3.nAnimalCageID			= pAnimalCage->GetID();
			stPacket3.nFarmID				= pFarm->GetID();
			stPacket3.nAnimalID				= pAnimal->GetID();
			stPacket3.nAnimalOwner			= pAnimal->GetOwner();
			stPacket3.nAnimalKind			= (WORD)pAnimal->GetKind();
			stPacket3.nAnimalStep			= (WORD)pAnimal->GetStep();
			stPacket3.nAnimalLife			= pAnimal->GetLife();
			stPacket3.nAnimalNextStepTime	= pAnimal->GetNextStepTime();
			stPacket3.nAnimalContentment	= pAnimal->GetContentment();
			stPacket3.nAnimalInterest		= pAnimal->GetInterest();

			pPlayer->SendMsg(&stPacket3, sizeof(stPacket3));
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SendFarmUIInfoToClient Method																		클라이언트에 농장UI정보 전송
//
VOID CSHFarmManager::SRV_SendFarmUIInfoToClient(DWORD nPlayerID, DWORD nFamilyMasterID, FARM_UI_NOTIFY_KIND eKind)
{
#if defined(_MAPSERVER_)
	for(int i=0; i<CSHFarmZone::FARM_ZONE_MAX; i++)
	{
		CSHFarmZone* pcsFarmZone = g_csFarmManager.GetFarmZone((WORD)i);
		if (pcsFarmZone == NULL) continue;

		for(int j=0; j<pcsFarmZone->GetFarmNum(); j++)
		{
			CSHFarm* pcsFarm = pcsFarmZone->GetFarm(j);
			if (pcsFarm == NULL) continue;

			if (pcsFarm->GetOwner() == nFamilyMasterID)
			{
				CSHFarmManager::PACKET_FARM_STATE_WITH_TAX		stPacket;
				CSHFarmManager::PACKET_FARM_GARDEN_CROP_PLANT	stPacket2;

				stPacket.Category			= MP_FARM;
				stPacket.Protocol			= MP_FARM_MANAGE_UI_FARM_DATA;
				stPacket.dwObjectID			= nPlayerID;
				stPacket.nFarmZone			= (WORD)i;
				stPacket.nFarmID			= pcsFarm->GetID();
				stPacket.nFarmState			= (WORD)pcsFarm->GetOwnState();
				stPacket.nGardenGrade		= pcsFarm->GetGarden(0)->GetGrade();
				stPacket.nFenceGrade		= pcsFarm->GetFence(0)->GetGrade();
				stPacket.nAnimalCageGrade	= pcsFarm->GetAnimalCage(0)->GetGrade();
				stPacket.nHouseGrade		= pcsFarm->GetHouse(0)->GetGrade();
				stPacket.nWarehouseGrade	= pcsFarm->GetWarehouse(0)->GetGrade();
				stPacket.nTaxArrearageFreq	= (WORD)pcsFarm->GetTaxArrearageFreq();
				SafeStrCpy(stPacket.szTaxPayPlayerName, pcsFarm->GetTaxPayPlayerName(), MAX_NAME_LENGTH+1);

				g_Network.Broadcast2AgentServer((char*)&stPacket, sizeof(stPacket));
				// 맵에 접속해서 수행되는 경우라면, 농장 관리비 납부 사실을 체크해서 미납 사실을 알린다.
				if (eKind == FARM_UI_NOTIFY_KIND_CONN_MAP)
				{
					SRV_NotifyTaxNonpayment(nPlayerID, pcsFarm);
				}

				for(int k=0; k<pcsFarm->GetGardenNum(); k++)
				{
					CSHGarden* pcsGarden = pcsFarm->GetGarden(k);
					for(int p=0; p<pcsGarden->GetCropNum(); p++)
					{
						CSHCrop* pcsCrop = pcsGarden->GetCrop(p);

						stPacket2.Category 			= MP_FARM;
						stPacket2.Protocol 			= MP_FARM_MANAGE_UI_CROP_DATA;
						stPacket2.dwObjectID		= nPlayerID;
						stPacket2.nFarmZone			= (WORD)i;
						stPacket2.nFarmID			= pcsFarm->GetID();
						stPacket2.nGardenID			= pcsGarden->GetID();
						stPacket2.nCropID			= pcsCrop->GetID();
						stPacket2.nCropOwner		= pcsCrop->GetOwner();
						stPacket2.nCropKind			= (WORD)pcsCrop->GetKind();
						stPacket2.nCropStep			= (WORD)pcsCrop->GetStep();
						stPacket2.nCropLife			= pcsCrop->GetLife();
						stPacket2.nCropNextStepTime	= pcsCrop->GetNextStepTime();
						stPacket2.nCropSeedGrade	= pcsCrop->GetSeedGrade();
						stPacket2.nOwner			= NULL;

						g_Network.Broadcast2AgentServer((char*)&stPacket2, sizeof(stPacket2));
					}
				}
			}
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SendFarmUIInfoToClient Method																		클라이언트에 농장UI정보 전송
//
VOID CSHFarmManager::SRV_SendFarmUIInfoToClient(WORD nFarmZone, WORD nFarmID, WORD nGardenID, CSHCrop* pCrop, DWORD nFarmOwner)
{
#if defined(_MAPSERVER_)
	// 농장UI 정보 전송
	PACKET_FARM_GARDEN_CROP_PLANT stPacket;

	stPacket.Category			= MP_FARM;
	stPacket.Protocol			= MP_FARM_MANAGE_UI_CROP_DATA;
	stPacket.dwObjectID			= NULL;
	stPacket.nFarmZone			= nFarmZone;
	stPacket.nFarmID			= nFarmID;
	stPacket.nGardenID			= nGardenID;
	stPacket.nCropID			= pCrop->GetID();
	stPacket.nCropOwner			= pCrop->GetOwner();
	stPacket.nCropSeedGrade		= pCrop->GetSeedGrade();
	stPacket.nCropStep			= (WORD)pCrop->GetStep();
	stPacket.nCropKind			= (WORD)pCrop->GetKind();
	stPacket.nCropLife			= pCrop->GetLife();
	stPacket.nCropNextStepTime  = pCrop->GetNextStepTime();
	stPacket.nOwner				= nFarmOwner;

	g_Network.Broadcast2AgentServer((char*)&stPacket, sizeof(stPacket));
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SendFarmUIInfoToClient Method															   클라이언트의 맵접속 시 농장 정보 전송
//
VOID CSHFarmManager::SRV_SendPlayerFarmInfo(CPlayer* pPlayer)
{
#if defined(_MAPSERVER_)
	MSG_DWORD3 msg;

	// 심기 딜레이 시간
	if (pPlayer->GetFarmInfo()->nCropPlantRetryTime)
	{
		msg.Category = MP_FARM;
		msg.Protocol = MP_FARM_TIMEDELAY;
		msg.dwData1	 = FARM_TIMEDELAY_KIND_PLANT;
		msg.dwData2	 = CSHCrop::CROP_PLANT_RETRY_TIME;
		msg.dwData3	 = pPlayer->GetFarmInfo()->nCropPlantRetryTime;
		pPlayer->SendMsg(&msg, sizeof(msg));
	}

	// 비료주기 딜레이 시간
	if (pPlayer->GetFarmInfo()->nCropManureRetryTime)
	{
		msg.Category = MP_FARM;
		msg.Protocol = MP_FARM_TIMEDELAY;
		msg.dwData1	 = FARM_TIMEDELAY_KIND_MANURE;
		msg.dwData2	 = CSHCrop::CROP_MANURE_RETRY_TIME;
		msg.dwData3	 = pPlayer->GetFarmInfo()->nCropManureRetryTime;
		pPlayer->SendMsg(&msg, sizeof(msg));
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_GetFarmRenderObjList Method																			 렌더링 오브젝트 리스트 얻기
//
CSHFarmManager::stFARMRENDEROBJLIST* CSHFarmManager::CLI_GetFarmRenderObjList(int nID)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	int nCnt = 0;
	while(m_pstFarmRenderObjList[nCnt].nID)
	{
		if (m_pstFarmRenderObjList[nCnt].nID == nID)
			return &m_pstFarmRenderObjList[nCnt];

		nCnt++;
	}
#endif
	return NULL;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_SetTargetCrop Method																				   조작할 농장 오브젝트 선택
//
int CSHFarmManager::CLI_SetTargetObj(CSHFarmRenderObj* pFarmRenderObj, BOOL bShowDlg)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	m_pcsSelFarmObj = pFarmRenderObj;
 	if (m_pcsSelFarmObj)
	{
		int nFarmZone = 0;
		int nObjKind = GetRenderObjKind(pFarmRenderObj->GetInfo()->nKind);
		int nObjID = pFarmRenderObj->GetInfo()->nID;


		BOOL bResult = GetFarmZoneID(pFarmRenderObj->GetInfo()->nMapNum, &nFarmZone);
		if (bResult == FALSE)
		{
			return 0;
		}

		m_stSelFarmObj.nFarmZone = WORD(nFarmZone);

		switch(nObjKind)
		{
		// 창고
		case RENDER_WAREHOUSE_KIND:
		// 집
		case RENDER_HOUSE_KIND:
		// 축사
		case RENDER_ANIMALCAGE_KIND:
		// 울타리
		case RENDER_FENCE_KIND:
		// 기본 울타리
		case RENDER_BASIC_FENCE:
		// 농장 판매 팻말
		case RENDER_SALEBOARD_KIND:
		// 집사
		case RENDER_STEWARD_KIND:
			{
				m_stSelFarmObj.nFarmID		= nObjID/100-1;

				if (bShowDlg)
				{
					CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;
					if (pGuageDlg)
					{
						pGuageDlg->GetMonsterGuage()->SetActive(FALSE);
					}
				}
			}
			break;
		// 텃밭흙더미
		case RENDER_GARDENPATCH_KIND:
		case RENDER_BASIC_GARDENPATCH_KIND:
			{
  				m_stSelFarmObj.nFarmID		= nObjID/100-1;
				m_stSelFarmObj.nGardenID	= 0;
				m_stSelFarmObj.nCropID		= (nObjID%10000)-(m_stSelFarmObj.nFarmID+1)*100-1;

				// 심기 딜레이 게이지 설정
				CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;
				if (pGuageDlg)
				{
 					CSHFarmZone* const pcsFarmZone = GetFarmZone(
						WORD(m_stSelFarmObj.nFarmZone));

					// 090520 LUJ, 검사 코드 추가
					if( 0 == pcsFarmZone )
					{
						break;
					}

					CSHFarm*		pcsFarm			= pcsFarmZone->GetFarm(m_stSelFarmObj.nFarmID);
					CSHGarden*		pcsGarden		= pcsFarm->GetGarden(0);
					CSHCrop*		pcsCrop			= pcsGarden->GetCrop(m_stSelFarmObj.nCropID);

					if (bShowDlg)
					{
						if (pcsCrop->GetStep() == CSHCrop::CROP_STEP_SEEDING)
						{
							pGuageDlg->GetMonsterGuage()->SetActive(TRUE);
						}
						else
						{
							pGuageDlg->GetMonsterGuage()->SetActive(FALSE);
						}
					}
				}
			}
			break;
		// 축사 슬롯
		case RENDER_BASIC_ANIMAL_CAGE_SLOT_KIND:
			{
				m_stSelFarmObj.nFarmID			= nObjID/100-1;
				m_stSelFarmObj.nAnimalCageID	= 0;
				m_stSelFarmObj.nAnimalID		= (nObjID%10000)-(m_stSelFarmObj.nFarmID+1)*100-1;

				// 심기 딜레이 게이지 설정
				CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;
				if (pGuageDlg)
				{
 					CSHFarmZone* const pcsFarmZone = GetFarmZone(
						WORD(m_stSelFarmObj.nFarmZone));

					// 090520 LUJ, 검사 코드 추가
					if( 0 == pcsFarmZone )
					{
						break;
					}

					CSHFarm*		pcsFarm			= pcsFarmZone->GetFarm(m_stSelFarmObj.nFarmID);
					CPen*			pcsPen			= pcsFarm->GetAnimalCage(0);
					CAnimal*		pcsAnimal		= pcsPen->GetAnimal(m_stSelFarmObj.nAnimalID);

					if (bShowDlg)
					{
						if (pcsAnimal->GetStep() == CAnimal::ANIMAL_STEP_IN_STALL)
						{
							pGuageDlg->GetMonsterGuage()->SetActive(TRUE);
						}
						else
						{
							pGuageDlg->GetMonsterGuage()->SetActive(FALSE);
							// 여기 쯤에 다이얼로그 불러 오면 될거 같음...;
						}
					}
				}
			}
			break;
		// 가축 설정
		case RENDER_ANIMAL_KIND:
			{
				m_stSelFarmObj.nFarmID			= nObjID/100-1;
				m_stSelFarmObj.nAnimalCageID	= 0;
				// 080618 Fix nAnimalCageID -> nFarmID
				m_stSelFarmObj.nAnimalID		= (nObjID%10000)-(m_stSelFarmObj.nFarmID+1)*100-1;

				if( bShowDlg )
				{
					CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;
					if( pGuageDlg )
					{
						CSHFarmZone* const pcsFarmZone = GetFarmZone(
							WORD(m_stSelFarmObj.nFarmZone));

						// 090520 LUJ, 검사 코드 추가
						if( 0 == pcsFarmZone )
						{
							break;
						}

						CSHFarm*		pcsFarm			= pcsFarmZone->GetFarm(m_stSelFarmObj.nFarmID);
						CPen*			pcsAnimalCage	= pcsFarm->GetAnimalCage(0);
						CAnimal*		pcsAnimal		= pcsAnimalCage->GetAnimal(m_stSelFarmObj.nAnimalID);

						pGuageDlg->SetActive(TRUE);
						pGuageDlg->GetMonsterGuage()->SetActive(TRUE);
						pGuageDlg->SetMonsterLife(pcsAnimal->GetContentment(), 100);
						// 이름 설정
						pGuageDlg->SetName(pcsAnimal->GetRenderObj(), CAnimal::ANIMAL_NAME_TBL[pcsAnimal->GetKind()]);
					}
				}
			}
			break;
		// 농작물 설정
		case RENDER_CROP_KIND:
			{
				m_stSelFarmObj.nFarmID		= nObjID/100-1;
				m_stSelFarmObj.nGardenID	= 0;
				m_stSelFarmObj.nCropID		= (nObjID%10000)-(m_stSelFarmObj.nFarmID+1)*100-1;

				// 농작물 생명력 게이지 설정
				if (bShowDlg)
				{
					CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;
					if (pGuageDlg)
					{
 						CSHFarmZone* const pcsFarmZone = GetFarmZone(
							WORD(m_stSelFarmObj.nFarmZone));

						// 090520 LUJ, 검사 코드 추가
						if( 0 == pcsFarmZone )
						{
							break;
						}

						CSHFarm*		pcsFarm			= pcsFarmZone->GetFarm(m_stSelFarmObj.nFarmID);
						CSHGarden*		pcsGarden		= pcsFarm->GetGarden(0);
						CSHCrop*		pcsCrop			= pcsGarden->GetCrop(m_stSelFarmObj.nCropID);

						pGuageDlg->SetActive(TRUE);
						pGuageDlg->GetMonsterGuage()->SetActive(TRUE);
						pGuageDlg->SetMonsterLife(pcsCrop->GetLife(), pcsCrop->GetMaxLife());
						pGuageDlg->SetName(pcsCrop->GetRenderObj(), m_pszTblFarmCropName[CSHCrop::CROP_STEP_MAX*(pcsCrop->GetKind()-1)+pcsCrop->GetStep()-CSHCrop::CROP_STEP_1].szName);
					}
				}
			}
			break;
		}

		return nObjKind;
	}

#endif
	return 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_SetFarmObjForRender Method																	 화면 출력을 위한 농장 오브젝트 설정
//
VOID CSHFarmManager::CLI_SetFarmObjForRender(CSHFarmRenderObj* pFarmRenderObj)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	CSHFarmRenderObj* pcsLastSetFarmObj = m_pcsSelFarmObj;
	// m_stSelFarmObj 를 설정하기 위해 아래 함수 사용
	CLI_SetTargetObj(pFarmRenderObj, FALSE);

 	CSHFarmZone* const pcsFarmZone = GetFarmZone(
		WORD(m_stSelFarmObj.nFarmZone));

	// 090520 LUJ, 검사 코드 추가
	if( 0 == pcsFarmZone )
	{
		return;
	}

	CSHFarm*		pcsFarm			= pcsFarmZone->GetFarm(m_stSelFarmObj.nFarmID);
	CSHGarden*		pcsGarden		= pcsFarm->GetGarden(0);
	CSHFarmObj*		pcsFence		= pcsFarm->GetFence(0);
	//CSHFarmObj*		pcsAnimalCage	= pcsFarm->GetAnimalCage(0);
	CSHFarmObj*		pcsHouse		= pcsFarm->GetHouse(0);
	CSHFarmObj*		pcsWarehouse	= pcsFarm->GetWarehouse(0);

	CSHCrop*		pcsCrop			= pcsGarden->GetCrop(m_stSelFarmObj.nCropID);

	// 080408 KTH 
	CPen*			pcsPen			= pcsFarm->GetAnimalCage(0);
	CAnimal*		pcsAnimal		= pcsPen->GetAnimal(m_stSelFarmObj.nAnimalID);

	// 오브젝트의 종류에 따라 출력 오브젝트 설정
	int nKind = GetRenderObjKind(pFarmRenderObj->GetInfo()->nKind);

 	switch(nKind)
	{
	// ..창고
	case RENDER_WAREHOUSE_KIND:
		pFarmRenderObj->GetEngineObject()->DisablePick();
		pcsWarehouse->SetRenderObj(pFarmRenderObj);
		break;
	// ..집
	case RENDER_HOUSE_KIND:
		pFarmRenderObj->GetEngineObject()->DisablePick();
		pcsHouse->SetRenderObj(pFarmRenderObj);
		break;
	// ..축사
	case RENDER_ANIMALCAGE_KIND:
		pFarmRenderObj->GetEngineObject()->DisablePick();
		//pFarmRenderObj->GetEngineObject()->
		pcsPen->SetRenderObj(pFarmRenderObj);
		break;
	// ..울타리
	case RENDER_FENCE_KIND:
		pFarmRenderObj->GetEngineObject()->DisablePick();
		pcsFence->SetRenderObj(pFarmRenderObj);
		break;
	// ..판매 팻말
	case RENDER_SALEBOARD_KIND:
		pcsFarm->SetRenderObjEx(pFarmRenderObj, 0);
		break;
	// ..기본 울타리
	case RENDER_BASIC_FENCE:
		pFarmRenderObj->GetEngineObject()->DisablePick();
		pcsFarm->SetRenderObjEx(pFarmRenderObj, 1);
		break;
	// ..집사
	case RENDER_STEWARD_KIND:
		pcsFarm->SetRenderObjEx(pFarmRenderObj, 2);
		break;
	// ..텃밭흙더미
	// ..텃밭은 텃밭 자체의 그래픽이 아니고 농작물 개수만큼의 흙더미 그래픽으로 표현하므로 Ex 함수를 사용
	case RENDER_GARDENPATCH_KIND:
		pFarmRenderObj->GetEngineObject()->DisablePick();
		pcsGarden->SetRenderObjEx(pFarmRenderObj, m_stSelFarmObj.nCropID);
		break;
	case RENDER_BASIC_GARDENPATCH_KIND:
		if (pcsCrop->GetStep() > 0) pFarmRenderObj->GetEngineObject()->DisablePick();
		pcsGarden->SetRenderObjEx(pFarmRenderObj, m_stSelFarmObj.nCropID+15);
		break;
	// ..농작물
	case RENDER_CROP_KIND:
		// 이름 변경
		{
			char* pszName = m_pszTblFarmCropName[CSHCrop::CROP_STEP_MAX*(pcsCrop->GetKind()-1)+pcsCrop->GetStep()-CSHCrop::CROP_STEP_1].szName;
 			pFarmRenderObj->SetName(pszName);
			pFarmRenderObj->GetObjectBalloon()->GetOBalloonName()->SetName(pszName);
			pcsCrop->SetRenderObj(pFarmRenderObj);
			if (pcsCrop->GetStep() > 0) pcsGarden->GetRenderObjEx(pcsCrop->GetID()+15)->GetEngineObject()->DisablePick();
		}
		break;
	//case RENDER_ANIMAL_CAGE_SLOT_KIND:
	//	pFarmRenderObj->GetEngineObject()->DisablePick();
	//	pcsPen->SetRenderObjEx(pFarmRenderObj, m_stSelFarmObj.nAnimalID);
	//	break;
	case RENDER_BASIC_ANIMAL_CAGE_SLOT_KIND:
		{
			if( pcsAnimal->GetStep() > 0 ) 
			{
				pFarmRenderObj->GetEngineObject()->DisablePick();
			}
			else
			{
				pFarmRenderObj->GetEngineObject()->Show();
			}
			pcsPen->SetRenderObjEx(pFarmRenderObj, m_stSelFarmObj.nAnimalID+5);
		}
		break;
	case RENDER_ANIMAL_KIND:
		{
			if( pcsAnimal )
			{
				pcsPen->GetRenderObjEx(m_stSelFarmObj.nAnimalID+5)->GetEngineObject()->DisablePick();
				pcsPen->GetRenderObjEx(m_stSelFarmObj.nAnimalID+5)->GetEngineObject()->Hide();
				//OBJECTMGR->AddGarbageObject(pcsPen->GetRenderObjEx(pcsAnimal->GetID()+5));
			}

			pFarmRenderObj->SetName(CAnimal::ANIMAL_NAME_TBL[pcsAnimal->GetKind()]);
			pFarmRenderObj->GetObjectBalloon()->GetOBalloonName()->SetName(CAnimal::ANIMAL_NAME_TBL[pcsAnimal->GetKind()]);
			pcsAnimal->SetRenderObj(pFarmRenderObj);
		}
		break;
	}

	// 원래 선택되어 있던 obj 복구
	CLI_SetTargetObj(pcsLastSetFarmObj, FALSE);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_AddRenderFarmObjToScreen Method																		 화면에 렌더링 오브젝트 추가
//
VOID CSHFarmManager::CLI_AddRenderFarmObjToScreen(CSHFarm* pFarm, int nTarget)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	BASEOBJECT_INFO		binfo;
	BASEMOVE_INFO		minfo;
	CSHFarmRenderObj::stFarmRenderObjInfo stFarmRenderObj;

	int nCnt = 0;
	DWORD n = 0;
	while(m_pstBFarm[nCnt].nID)
	{
		stFarmRenderObj.nKind = m_pstBFarm[nCnt].nKind;
		SafeStrCpy( binfo.ObjectName, m_pstBFarm[nCnt].szName, MAX_NAME_LENGTH+1 );
		stFarmRenderObj.nID = m_pstBFarm[nCnt].nID + pFarm->GetID()*100;
		stFarmRenderObj.nMapNum = m_pstBFarm[nCnt].nMapNum;

		if (nTarget)
		{
			if (nTarget != stFarmRenderObj.nKind)
			{
				goto NEXT;
			}
		}

		// 비어있는 농장이라면 팻말과 기본 울타리만 추가
		if (pFarm->GetOwnState() == CSHFarm::OWN_STATE_EMPTY)
		{
			if (stFarmRenderObj.nKind == RENDER_SALEBOARD_KIND ||
				stFarmRenderObj.nKind == RENDER_BASIC_FENCE)
			{
			}
			else
			{
				goto NEXT;
			}
		}
		// 농장이 활성화 되었으면 팻말과 기본 울타리 제외
		else
		{
			if (stFarmRenderObj.nKind == RENDER_SALEBOARD_KIND ||
				stFarmRenderObj.nKind == RENDER_BASIC_FENCE)
			{
				goto NEXT;
			}
		}
	
		float fDir;
		int nFarmObjID = 0;
 		if (stFarmRenderObj.nKind == RENDER_GARDENPATCH_KIND)
		{
			nFarmObjID = (stFarmRenderObj.nID%10000)-stFarmRenderObj.nID/100*100-1;
			minfo.CurPosition.x = pFarm->GetGarden(0)->GetCrop(nFarmObjID)->GetPos()->x;
			minfo.CurPosition.y = 0;
 			minfo.CurPosition.z = pFarm->GetGarden(0)->GetCrop(nFarmObjID)->GetPos()->z;
			fDir = pFarm->GetGarden(0)->GetCrop(nFarmObjID)->GetDir();

			stFarmRenderObj.nKind = stFarmRenderObj.nKind + pFarm->GetGarden(0)->GetGrade() - 1;
		}
 		else if (stFarmRenderObj.nKind == RENDER_CROP_KIND)
		{
			goto NEXT;
		}
		else if (stFarmRenderObj.nKind == RENDER_BASIC_GARDENPATCH_KIND)
		{
			nFarmObjID = (stFarmRenderObj.nID%10000)-stFarmRenderObj.nID/100*100-1;
			minfo.CurPosition.x = pFarm->GetGarden(0)->GetCrop(nFarmObjID)->GetPos()->x;
			minfo.CurPosition.y = 0;
 			minfo.CurPosition.z = pFarm->GetGarden(0)->GetCrop(nFarmObjID)->GetPos()->z;
			fDir = pFarm->GetGarden(0)->GetCrop(nFarmObjID)->GetDir();
		}
		else if( stFarmRenderObj.nKind == RENDER_ANIMAL_KIND )
		{
			goto NEXT;
		}
		else if (stFarmRenderObj.nKind == RENDER_BASIC_ANIMAL_CAGE_SLOT_KIND)
		{
			nFarmObjID = (stFarmRenderObj.nID%10000)-stFarmRenderObj.nID/100*100-1;
			minfo.CurPosition.x = pFarm->GetAnimalCage(0)->GetAnimal(nFarmObjID)->GetPos()->x;
			minfo.CurPosition.y = 0;
			minfo.CurPosition.z = pFarm->GetAnimalCage(0)->GetAnimal(nFarmObjID)->GetPos()->z;
			fDir = pFarm->GetAnimalCage(0)->GetAnimal(nFarmObjID)->GetDir();
		}
		else if (stFarmRenderObj.nKind == RENDER_SALEBOARD_KIND ||
				 stFarmRenderObj.nKind == RENDER_STEWARD_KIND)
		{
			minfo.CurPosition.x = pFarm->GetFence(0)->GetPos()->x;
			minfo.CurPosition.y = 0;
 			minfo.CurPosition.z = pFarm->GetFence(0)->GetPos()->z;
 			fDir = pFarm->GetFence(0)->GetDir();
			if (stFarmRenderObj.nKind == RENDER_STEWARD_KIND) fDir += 1.5f;
		}
		else
		{
			minfo.CurPosition.x = pFarm->GetPos()->x;
			minfo.CurPosition.y = 0;
 			minfo.CurPosition.z = pFarm->GetPos()->z;
			fDir = pFarm->GetDir();

			// 등급에 따른 오브젝트 종류 설정
 			switch(stFarmRenderObj.nKind)
			{
			// ..창고
			case RENDER_WAREHOUSE_KIND:
 				stFarmRenderObj.nKind += pFarm->GetWarehouse(0)->GetGrade() - 1;
				break;
			// ..집
			case RENDER_HOUSE_KIND:
				stFarmRenderObj.nKind += pFarm->GetHouse(0)->GetGrade() - 1;
				break;
			// ..축사
			case RENDER_ANIMALCAGE_KIND:
				stFarmRenderObj.nKind += pFarm->GetAnimalCage(0)->GetGrade() - 1;
				break;
			// ..울타리
			case RENDER_FENCE_KIND:
				stFarmRenderObj.nKind += pFarm->GetFence(0)->GetGrade() - 1;
				break;
			}
		}


		binfo.dwObjectID = FARM_RENDER_OBJ_ID_START + n++;
		while(OBJECTMGR->GetObject(binfo.dwObjectID))
		{
			binfo.dwObjectID = FARM_RENDER_OBJ_ID_START + n++;
		}

		binfo.ObjectState = eObjectState_None;
		minfo.bMoving = FALSE;
		minfo.KyungGongIdx = 0;
		minfo.MoveMode = eMoveMode_Run;

		CSHFarmRenderObj* pFarmRenderObj = OBJECTMGR->AddFarmRenderObj(&binfo, &minfo, &stFarmRenderObj);

 		pFarmRenderObj->SetAngle(fDir);
		CLI_SetFarmObjForRender(pFarmRenderObj);
NEXT:
		nCnt++;
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RemoveRenderFarmObjFromScreen Method															   화면에서 렌더링 오브젝트 제거
//
//091117 pdy 농장 이벤트로인해 주변 농장의 타겟이 해제되는 버그 수정 
VOID CSHFarmManager::CLI_RemoveRenderFarmObjFromScreen(CSHFarm* pFarm, int nTarget)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)

	BOOL bClearSelTarget = FALSE ;

	if (nTarget)
	{
		switch(nTarget)
		{
		// 농작물
		case RENDER_CROP_KIND :
			for(int i=0; i<15; i++)
			{
				if (pFarm->GetGarden(0)->GetCrop(i)->GetRenderObj())
				{
					if((CObject*)pFarm->GetGarden(0)->GetCrop(i)->GetRenderObj() == OBJECTMGR->GetSelectedObject())
					{
						bClearSelTarget = TRUE ;
					}

					OBJECTMGR->AddGarbageObject(pFarm->GetGarden(0)->GetCrop(i)->GetRenderObj());
					pFarm->GetGarden(0)->GetCrop(i)->SetRenderObj(NULL);						
				}
			}
			break;
		// 흙더미
		case RENDER_GARDENPATCH_KIND :	
			for(int i=0; i<15; i++)
			{
				if (pFarm->GetGarden(0)->GetRenderObjEx(i))
				{
					if((CObject*)pFarm->GetGarden(0)->GetRenderObjEx(i) == OBJECTMGR->GetSelectedObject())
					{
						bClearSelTarget = TRUE ;
					}

					OBJECTMGR->AddGarbageObject(pFarm->GetGarden(0)->GetRenderObjEx(i));
					pFarm->GetGarden(0)->SetRenderObjEx(NULL, i);
				}
			}
			break;
		// 기본 흙더미
		case RENDER_BASIC_GARDENPATCH_KIND :	
			for(int i=15; i<30; i++)
			{
				if (pFarm->GetGarden(0)->GetRenderObjEx(i))
				{
					if((CObject*)pFarm->GetGarden(0)->GetRenderObjEx(i) == OBJECTMGR->GetSelectedObject())
					{
						bClearSelTarget = TRUE ;
					}

					OBJECTMGR->AddGarbageObject(pFarm->GetGarden(0)->GetRenderObjEx(i));
					pFarm->GetGarden(0)->SetRenderObjEx(NULL, i);
				}
			}
			break;
		case RENDER_HOUSE_KIND :			
			// 집
			if(pFarm->GetHouse(0)->GetRenderObj() && ((CObject*)pFarm->GetHouse(0)->GetRenderObj() == OBJECTMGR->GetSelectedObject()))
			{
				bClearSelTarget = TRUE ;
			}

			OBJECTMGR->AddGarbageObject(pFarm->GetHouse(0)->GetRenderObj());
			pFarm->GetHouse(0)->SetRenderObj(NULL);
			break;
		case RENDER_WAREHOUSE_KIND :
			// 창고
			if(pFarm->GetWarehouse(0)->GetRenderObj() && ((CObject*)pFarm->GetWarehouse(0)->GetRenderObj() == OBJECTMGR->GetSelectedObject()))
			{
 				bClearSelTarget = TRUE ;
			}

			OBJECTMGR->AddGarbageObject(pFarm->GetWarehouse(0)->GetRenderObj());
			pFarm->GetWarehouse(0)->SetRenderObj(NULL);
			break;
		case RENDER_ANIMALCAGE_KIND :	
			// 축사
			if(pFarm->GetAnimalCage(0)->GetRenderObj() && ((CObject*)pFarm->GetAnimalCage(0)->GetRenderObj() == OBJECTMGR->GetSelectedObject()))
			{
				bClearSelTarget = TRUE ;
			}

			OBJECTMGR->AddGarbageObject(pFarm->GetAnimalCage(0)->GetRenderObj());
			pFarm->GetAnimalCage(0)->SetRenderObj(NULL);
			break;
		case RENDER_FENCE_KIND :			
			// 울타리
			if(pFarm->GetFence(0)->GetRenderObj() && ((CObject*)pFarm->GetFence(0)->GetRenderObj() == OBJECTMGR->GetSelectedObject()))
			{
				bClearSelTarget = TRUE ;
			}

			OBJECTMGR->AddGarbageObject(pFarm->GetFence(0)->GetRenderObj());
			pFarm->GetFence(0)->SetRenderObj(NULL);
			break;
		}
	}
	else
	{
		for(int i=0; i<15; i++)
		{
			// 농작물
			if (pFarm->GetGarden(0)->GetCrop(i)->GetRenderObj())
			{
				if((CObject*)pFarm->GetGarden(0)->GetCrop(i)->GetRenderObj() == OBJECTMGR->GetSelectedObject())
				{
					bClearSelTarget = TRUE ;
				}

				OBJECTMGR->AddGarbageObject(pFarm->GetGarden(0)->GetCrop(i)->GetRenderObj());
				pFarm->GetGarden(0)->GetCrop(i)->SetRenderObj(NULL);
			}
		}

		for(int i=0; i<15; i++)
		{
			// 흙더미
			if (pFarm->GetGarden(0)->GetRenderObjEx(i))
			{
				if((CObject*)pFarm->GetGarden(0)->GetRenderObjEx(i) == OBJECTMGR->GetSelectedObject())
				{
					bClearSelTarget = TRUE ;
				}

				OBJECTMGR->AddGarbageObject(pFarm->GetGarden(0)->GetRenderObjEx(i));
				pFarm->GetGarden(0)->SetRenderObjEx(NULL, i);
			}
			if (pFarm->GetGarden(0)->GetRenderObjEx(i+15))
			{
				if((CObject*)pFarm->GetGarden(0)->GetRenderObjEx(i+15) == OBJECTMGR->GetSelectedObject())
				{
					bClearSelTarget = TRUE ;
				}

				OBJECTMGR->AddGarbageObject(pFarm->GetGarden(0)->GetRenderObjEx(i+15));
				pFarm->GetGarden(0)->SetRenderObjEx(NULL, i+15);
			}
		}

		
		for(int i = 0; i < 5; i++ )
		{
			if( pFarm->GetAnimalCage(0)->GetAnimal(i)->GetRenderObj())
			{
				OBJECTMGR->AddGarbageObject(pFarm->GetAnimalCage(0)->GetAnimal(i)->GetRenderObj());
				pFarm->GetAnimalCage(0)->GetAnimal(i)->SetRenderObj(NULL);
			}
		}
	
		for(int i=0; i<5; i++)
		{
			if( pFarm->GetAnimalCage(0)->GetRenderObjEx(i) )
			{
				OBJECTMGR->AddGarbageObject(pFarm->GetAnimalCage(0)->GetRenderObjEx(i));
				pFarm->GetAnimalCage(0)->SetRenderObjEx(NULL, i);
			}
		}

		// 집
		if(pFarm->GetHouse(0)->GetRenderObj() && ((CObject*)pFarm->GetHouse(0)->GetRenderObj() == OBJECTMGR->GetSelectedObject()))
		{
			bClearSelTarget = TRUE ;
		}
		OBJECTMGR->AddGarbageObject(pFarm->GetHouse(0)->GetRenderObj());
		pFarm->GetHouse(0)->SetRenderObj(NULL);
		
		// 창고
		if(pFarm->GetWarehouse(0)->GetRenderObj() && ((CObject*)pFarm->GetWarehouse(0)->GetRenderObj() == OBJECTMGR->GetSelectedObject()))
		{
			bClearSelTarget = TRUE ;
		}
		OBJECTMGR->AddGarbageObject(pFarm->GetWarehouse(0)->GetRenderObj());
		pFarm->GetWarehouse(0)->SetRenderObj(NULL);
		
		// 축사
		if(pFarm->GetAnimalCage(0)->GetRenderObj() && ((CObject*)pFarm->GetAnimalCage(0)->GetRenderObj() == OBJECTMGR->GetSelectedObject()))
		{
			bClearSelTarget = TRUE ;
		}
		OBJECTMGR->AddGarbageObject(pFarm->GetAnimalCage(0)->GetRenderObj());
		pFarm->GetAnimalCage(0)->SetRenderObj(NULL);
		
		// 울타리
		if(pFarm->GetFence(0)->GetRenderObj() && ((CObject*)pFarm->GetFence(0)->GetRenderObj() == OBJECTMGR->GetSelectedObject()))
		{
			bClearSelTarget = TRUE ;
		}
		OBJECTMGR->AddGarbageObject(pFarm->GetFence(0)->GetRenderObj());
		pFarm->GetFence(0)->SetRenderObj(NULL);

		// 팻말
		if(pFarm->GetRenderObjEx(0) && ((CObject*)pFarm->GetRenderObjEx(0) == OBJECTMGR->GetSelectedObject()))
		{
			bClearSelTarget = TRUE ;
		}
 		OBJECTMGR->AddGarbageObject(pFarm->GetRenderObjEx(0));
		pFarm->SetRenderObjEx(NULL, 0);

		// 기본 울타리
		if(pFarm->GetRenderObjEx(1) && ((CObject*)pFarm->GetRenderObjEx(1) == OBJECTMGR->GetSelectedObject()))
		{
			bClearSelTarget = TRUE ;
		}
		OBJECTMGR->AddGarbageObject(pFarm->GetRenderObjEx(1));
		pFarm->SetRenderObjEx(NULL, 1);

		// 집사
		if(pFarm->GetRenderObjEx(2) && ((CObject*)pFarm->GetRenderObjEx(2) == OBJECTMGR->GetSelectedObject()))
		{
			bClearSelTarget = TRUE ;
		}
		OBJECTMGR->AddGarbageObject(pFarm->GetRenderObjEx(2));
		pFarm->SetRenderObjEx(NULL, 2);
	}

	if( bClearSelTarget == TRUE )
	{
		// 타켓 해제
 		if(OBJECTMGR->GetSelEffectHandle())
		{
			EFFECTMGR->ForcedEndEffect(OBJECTMGR->GetSelEffectHandle());
		}

		if(OBJECTMGR->GetSelectedObject())
		{
			OBJECTMGR->GetSelectedObject()->ShowObjectName( FALSE );
			OBJECTMGR->GetSelectedObject()->OnDeselected();
			OBJECTMGR->SetSelectedObjectID(0);
			CLI_SetTargetObj(NULL);
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_AddRenderFarmCropObjToScreen Method															  화면에 렌더링 농작물 오브젝트 추가
//
VOID CSHFarmManager::CLI_AddRenderFarmCropObjToScreen(CSHCrop* pCrop)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if (pCrop->GetStep() < CSHCrop::CROP_STEP_1) return;

	BASEOBJECT_INFO		binfo;
	BASEMOVE_INFO		minfo;
	CSHFarmRenderObj::stFarmRenderObjInfo stFarmRenderObj;

	int nCnt = 0;
	DWORD n = 0;
	while(m_pstBFarm[nCnt].nID)
	{
		if (m_pstBFarm[nCnt].nKind != RENDER_CROP_KIND)
		{
			nCnt++;
			continue;
		}

		stFarmRenderObj.nKind = m_pstBFarm[nCnt].nKind;
		int nStep = pCrop->GetStep() - CSHCrop::CROP_STEP_1;
		if (nStep < 0) nStep = 0;
		if (nStep > CSHCrop::CROP_STEP_MAX - 1) nStep = CSHCrop::CROP_STEP_MAX - 1;

		stFarmRenderObj.nKind = stFarmRenderObj.nKind + WORD((pCrop->GetKind()-1)*100 + nStep);

		SafeStrCpy( binfo.ObjectName, m_pstBFarm[nCnt].szName, MAX_NAME_LENGTH+1 );
		stFarmRenderObj.nID = (pCrop->GetParent()->GetParent()->GetID()+1)*100 + pCrop->GetID()+1;
		stFarmRenderObj.nMapNum = m_pstBFarm[nCnt].nMapNum;

		binfo.dwObjectID = FARM_RENDER_OBJ_ID_START + n++;
		while(OBJECTMGR->GetObject(binfo.dwObjectID))
		{
			binfo.dwObjectID = FARM_RENDER_OBJ_ID_START + n++;
		}

		binfo.ObjectState = eObjectState_None;
		minfo.bMoving = FALSE;
		minfo.KyungGongIdx = 0;
		minfo.MoveMode = eMoveMode_Run;

		float fDir = DEGTORAD(CSHMath::GetRandomNum(1, 20000));

		minfo.CurPosition.x = pCrop->GetPos()->x;
		minfo.CurPosition.y = 0;
 		minfo.CurPosition.z = pCrop->GetPos()->z;
		pCrop->GetDir();


		CSHFarmRenderObj* pFarmRenderObj = OBJECTMGR->AddFarmRenderObj(&binfo, &minfo, &stFarmRenderObj);

 		pFarmRenderObj->SetAngle(fDir);
		CLI_SetFarmObjForRender(pFarmRenderObj);
		break;
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RemoveRenderFarmCropObjToScreen Method														  화면에 렌더링 농작물 오브젝트 제거
//
VOID CSHFarmManager::CLI_RemoveRenderFarmCropObjToScreen(CSHCrop* pCrop)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	// 농작물
	OBJECTMGR->AddGarbageObject(pCrop->GetRenderObj());
	pCrop->SetRenderObj(NULL);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_ProcUpgradeDlg Method																				농장 업그레이드 대화 창 처리
//
BOOL CSHFarmManager::CLI_ProcUpgradeDlg(int nObjKind)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if (m_pcsSelFarmObj == NULL || 
		m_pcsSelFarmObj->GetInfo()->nKind != RENDER_STEWARD_KIND)
	{
		return FALSE;
	}

	WORD nGrade = 0;
	CSHFarm* const farm = g_csFarmManager.GetFarmZone(WORD(m_stSelFarmObj.nFarmZone))->GetFarm(m_stSelFarmObj.nFarmID);

	if(0 == farm)
	{
		return FALSE;
	}

	switch(nObjKind)
	{
	case CSHFarm::FARM_EVENT_GARDEN:
		nGrade = farm->GetGarden(0)->GetGrade();
		break;
	case CSHFarm::FARM_EVENT_FENCE:
		nGrade = farm->GetFence(0)->GetGrade();
		break;
	case CSHFarm::FARM_EVENT_HOUSE:
		nGrade = farm->GetHouse(0)->GetGrade();
		break;
	case CSHFarm::FARM_EVENT_WAREHOUSE:
		nGrade = farm->GetWarehouse(0)->GetGrade();
		break;
	case CSHFarm::FARM_EVENT_ANIMALCAGE:
		nGrade = farm->GetAnimalCage(0)->GetGrade();
		break;
	default:
		return FALSE;
	}

	// 등급이 최대값 이상이면 대화창을 열 필요 없다.
	if (nGrade >= CSHFarmObj::MAX_GRADE-2)
	{
		GAMEIN->GetFarmUpgradeDlg()->SetActive(FALSE);
 		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1063 ) );
		return TRUE;
	}

	if (nGrade >= CSHFarmObj::MAX_GRADE-4 && nObjKind == CSHFarm::FARM_EVENT_GARDEN)
	{
		GAMEIN->GetFarmUpgradeDlg()->SetActive(FALSE);
 		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1063 ) );
		return TRUE;
	}

	GAMEIN->GetFarmUpgradeDlg()->SetUpgradeTarget(m_stSelFarmObj.nFarmZone, m_stSelFarmObj.nFarmID, nObjKind);
	GAMEIN->GetFarmUpgradeDlg()->SetActive(TRUE);

#endif
	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_ProcPayTaxDlg Method																					농장 관리비 납부 대화 창
//
BOOL CSHFarmManager::CLI_ProcPayTaxDlg()
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if (m_pcsSelFarmObj == NULL || 
		m_pcsSelFarmObj->GetInfo()->nKind != RENDER_STEWARD_KIND)
	{
		return FALSE;
	}

	CLI_RequestPayTax();
#endif

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_ChangeChannel Method																							   채널 변환하기
//
VOID CSHFarmManager::CLI_ChangeChannel(BOOL bLogin)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	// 농장 맵이라면 채널을 무조건 0 으로 한다.
	static int nOriginalChannelNum = gChannelNum;
	static BOOL bOriginal = FALSE;

	// ..로그인 시점이라면 원래 채널번호 초기화
	//---KES 채널이동 수정
	if (bLogin || gCheatMove)
	{
		nOriginalChannelNum = gChannelNum;
		gCheatMove = FALSE;
	}

	// ..농장맵이라면 채널을 0 으로
	if (GetFarmZoneID(MAP->GetMapNum(), NULL))
	{
		if (bOriginal) nOriginalChannelNum = gChannelNum;
		gChannelNum = 0;
		bOriginal = FALSE;
	}
	else
	{
		bOriginal = TRUE;
		gChannelNum = nOriginalChannelNum;
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestBuyFarmToSrv Method																				   서버에 농장 구입 요청
//
VOID CSHFarmManager::CLI_RequestBuyFarmToSrv(CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if (m_pcsSelFarmObj == NULL) return;

	PACKET_FARM_STATE stPacket;
	stPacket.Category 		= MP_FARM;
	stPacket.Protocol 		= MP_FARM_BUY;
	stPacket.dwObjectID		= HEROID;
	stPacket.nFarmZone		= (WORD)eFarmZone;
	stPacket.nFarmID		= nFarmID;

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestBuyFarmToSrv Method																				   서버에 농장 구입 요청
//
VOID CSHFarmManager::CLI_RequestBuyFarmToSrv()
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if (m_pcsSelFarmObj == NULL) return;

	PACKET_FARM_STATE stPacket;

	stPacket.Category 	= MP_FARM;
	stPacket.Protocol 	= MP_FARM_BUY;
	stPacket.dwObjectID	= gHeroID;
	stPacket.nFarmZone	= WORD(m_stSelFarmObj.nFarmZone);
	stPacket.nFarmID	= WORD(m_stSelFarmObj.nFarmID);

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestPlantCropInGarden Method																			 서버에 농작물 심기 요청
//
VOID CSHFarmManager::CLI_RequestPlantCropInGarden(WORD nItemTargetPos, DWORD nItemIdx)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if (m_pcsSelFarmObj)
	{
		if (GetRenderObjKind(m_pcsSelFarmObj->GetInfo()->nKind) == RENDER_BASIC_GARDENPATCH_KIND)
		{
			PACKET_FARM_GARDEN_CROP_PLANT stPacket;

			stPacket.Category 		= MP_FARM;
			stPacket.Protocol 		= MP_FARM_GARDEN_PLANT;
			stPacket.dwObjectID		= gHeroID;
			stPacket.nFarmZone		= WORD(m_stSelFarmObj.nFarmZone);
			stPacket.nFarmID		= WORD(m_stSelFarmObj.nFarmID);
			stPacket.nGardenID		= WORD(m_stSelFarmObj.nGardenID);
			stPacket.nCropID		= WORD(m_stSelFarmObj.nCropID);
			stPacket.nItemTargetPos	= nItemTargetPos;
			stPacket.nItemIdx		= nItemIdx;

			NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestManureCropInGarden Method																	 서버에 농작물 비료주기 요청
//
VOID CSHFarmManager::CLI_RequestManureCropInGarden(WORD nItemTargetPos, DWORD nItemIdx)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if (m_pcsSelFarmObj)
	{
		if (GetRenderObjKind(m_pcsSelFarmObj->GetInfo()->nKind) == RENDER_CROP_KIND)
		{
			PACKET_FARM_GARDEN_CROP_MANURE stPacket;

			stPacket.Category 		= MP_FARM;
			stPacket.Protocol 		= MP_FARM_GARDEN_MANURE;
			stPacket.dwObjectID		= gHeroID;
			stPacket.nFarmZone		= WORD(m_stSelFarmObj.nFarmZone);
			stPacket.nFarmID		= WORD(m_stSelFarmObj.nFarmID);
			stPacket.nGardenID		= WORD(m_stSelFarmObj.nGardenID);
			stPacket.nCropID		= WORD(m_stSelFarmObj.nCropID);
			stPacket.nItemTargetPos	= nItemTargetPos;
			stPacket.nItemIdx		= nItemIdx;

			NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestHarvestropInGarden Method																	 서버에 농작물 수확하기 요청
//
VOID CSHFarmManager::CLI_RequestHarvestropInGarden(WORD nItemTargetPos, DWORD nItemIdx, WORD nType)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	static WORD _nItemTargetPos = nItemTargetPos;
	static DWORD _nItemIdx = nItemIdx;

	if (nItemTargetPos)
	{
		_nItemTargetPos = nItemTargetPos;
		_nItemIdx = nItemIdx;
	}

	if (m_pcsSelFarmObj)
	{
		if (GetRenderObjKind(m_pcsSelFarmObj->GetInfo()->nKind) == RENDER_CROP_KIND)
		{
			PACKET_FARM_GARDEN_CROP_GROW stPacket;

			stPacket.Category 		= MP_FARM;
			stPacket.Protocol 		= MP_FARM_GARDEN_HARVEST;
			stPacket.dwObjectID		= gHeroID;
			stPacket.nFarmZone		= WORD(m_stSelFarmObj.nFarmZone);
			stPacket.nFarmID		= WORD(m_stSelFarmObj.nFarmID);
			stPacket.nGardenID		= WORD(m_stSelFarmObj.nGardenID);
			stPacket.nCropID		= WORD(m_stSelFarmObj.nCropID);
			stPacket.nItemTargetPos	= _nItemTargetPos;
			stPacket.nItemIdx		= _nItemIdx;
			stPacket.nResult		= nType;

			NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
		}
	}
#endif
}

// 080414 KTH
// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestInstallAnimalInAnimalCage Method																		서버에 가축 넣기 요청
//
VOID CSHFarmManager::CLI_RequestInstallAnimalInAnimalCage(WORD nItemTargetPos, DWORD nItemIdx)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if (m_pcsSelFarmObj)
	{
		if (GetRenderObjKind(m_pcsSelFarmObj->GetInfo()->nKind) == RENDER_BASIC_ANIMAL_CAGE_SLOT_KIND)
		{
			PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT stPacket;

			stPacket.Category 		= MP_FARM;
			stPacket.Protocol 		= MP_FARM_ANIMAL_INSTALL;
			stPacket.dwObjectID		= gHeroID;
			stPacket.nFarmZone		= WORD(m_stSelFarmObj.nFarmZone);
			stPacket.nFarmID		= WORD(m_stSelFarmObj.nFarmID);
			stPacket.nAnimalCageID	= WORD(m_stSelFarmObj.nAnimalCageID);
			stPacket.nAnimalID		= WORD(m_stSelFarmObj.nAnimalID);
			stPacket.nItemTargetPos	= nItemTargetPos;
			stPacket.nItemIdx		= nItemIdx;

			NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
		}
		else
		{
			// 가축이 아닌 아이템은 넣을수 없다.
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 91 ) );
		}
	}
#endif
}

// 080415 KTH
// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_InstallAnimalInAnimalCage Method																					가축 넣기 요청
//
VOID CSHFarmManager::SRV_InstallAnimalInAnimalCage(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nAnimalCageID, WORD nAnimalID, WORD nItemTargetPos, DWORD nItemIdx, WORD nAnimalContentment, WORD nAnimalInterest, int nAgentChecked)
{
#if defined(_MAPSERVER_)
	RESULT eResult = RESULT_OK;
	
	CSHFarmZone* const pFarmZone = GetFarmZone( (WORD)eFarmZone );
	if (pFarmZone == NULL) return;

	CSHFarm* const pFarm = pFarmZone->GetFarm( nFarmID );

	if( 0 == pFarm )
	{
		return;
	}

	CPen* pPen = pFarm->GetAnimalCage(nAnimalCageID);

	if( 0 == pPen )
	{
		return;
	}

	// 아이템 체크
	ITEM_INFO * pItemInfo = ITEMMGR->GetUseItemInfo(pPlayer, nItemTargetPos, nItemIdx);
	if (pItemInfo == NULL || pItemInfo->SupplyType != ITEM_KIND_ANIMAL)
	{
		return;
	}

	const int nAnimalKind = pItemInfo->SupplyValue;
	CAnimal* const pAnimal = pPen->GetAnimal(nAnimalID);

	if( 0 == pAnimal )
	{
		return;
	}
	else if( nAnimalKind > RENDER_ANIMAL_KIND_NUM )
	{
		return;
	}

	// 패밀리 상태
	// ..에이전트에서 관리하므로 에이전트로 보낸다.
	if (nAgentChecked == 0)
	{
		PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT stPacket;

		stPacket.Category 		= MP_FARM;
		stPacket.Protocol 		= MP_FARM_ANIMAL_INSTALL_AGENT_CHECK;
		stPacket.dwObjectID		= pPlayer->GetID();
		stPacket.nFarmZone		= (WORD)eFarmZone;
		stPacket.nFarmID		= nFarmID;
		stPacket.nAnimalCageID	= nAnimalCageID;
		stPacket.nAnimalID		= nAnimalID;
		stPacket.nItemTargetPos	= nItemTargetPos;
		stPacket.nItemIdx		= nItemIdx;
		stPacket.nOwner			= pFarm->GetOwner();
		// 멤버 수 체크를 위해 nResult 변수에다가...
		stPacket.nResult		= (WORD)FARM_BUY_FAMILY_MEMNUM;
		stPacket.nAnimalContentment = nAnimalContentment;
		stPacket.nAnimalInterest = nAnimalInterest;

		// 090113 LUJ, 메시지를 에이전트에 보내서 가축 등록 가능한 상태인지 검사한다.
		//		그런데 플레이어가 접속하지 않은 에이전트에 농장 맵이 연결된 경우
		//		그 에이전트에 패밀리 정보가 없다.  이로 인해 항상 검사 실패가 반환된다.
		//		이를 막으려면 플레이어가 접속된 에이전트로 메시지를 전송해야 한다
		g_Network.Send2Server(
			pPlayer->GetAgentNum(),
			( char* )&stPacket,
			sizeof( stPacket ) );
		return;
	}

	// 비어 있는 축사 인가?
	if( pAnimal->GetStep() != CAnimal::ANIMAL_STEP_EMPTY )
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
	}

	VECTOR3 stPlayerPos;
	pPlayer->GetPosition(&stPlayerPos);
	float nDistance = CalcDistanceXZ(pAnimal->GetPos(), &stPlayerPos); 
	if (nDistance > PEN_OBJECT_CONTROL_DISTANCE)
	{
		eResult = RESULT_FAIL_04;
		goto FAILED;
	}

	// 아이템 제거
	if (ITEMMGR->UseItem(pPlayer, nItemTargetPos, nItemIdx) == eItemUseSuccess)
	{
		MSG_ITEM_USE_ACK msg;
		msg.Category	= MP_ITEM;
		msg.Protocol	= MP_ITEM_USE_ACK;
		msg.dwObjectID	= pPlayer->GetID();
		msg.TargetPos	= nItemTargetPos;
		msg.dwItemIdx	= nItemIdx;
		msg.eResult		= eItemUseSuccess;

		pPlayer->SendMsg(&msg, sizeof(msg));
	}
	else
	{
		return;
	}

	pAnimal->Breed(pPlayer->GetID(), pPen->GetGrade(), nAnimalKind);

	// DB에 저장
	SRV_SaveAnimalInfoToDB((WORD)eFarmZone, nFarmID, pAnimal);

	// 다시 심는데 필요한 시간 설정
	//pPlayer->SetFarmCropPlantRetryTime(CSHCrop::CROP_PLANT_RETRY_TIME);
	//Farm_SetTimeDelay(pPlayer->GetID(), FARM_TIMEDELAY_KIND_PLANT, CSHCrop::CROP_PLANT_RETRY_TIME);

	// Log
	InsertLogFarmAnimal( pFarm, pAnimal, eLog_FamilyFarmInstallAnimal );

FAILED:
	// 클라이언트에 알리기
	PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT stPacket;

	stPacket.Category			= MP_FARM;
	stPacket.Protocol			= MP_FARM_ANIMAL_INSTALL;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.nFarmZone			= (WORD)eFarmZone;
	stPacket.nFarmID			= nFarmID;
	stPacket.nAnimalCageID		= nAnimalCageID;
	stPacket.nAnimalID			= nAnimalID;
	stPacket.nOwner				= pPlayer->GetID();
	stPacket.nAnimalKind		= (WORD)nAnimalKind;
	stPacket.nAnimalLife		= pAnimal->GetContentment();
	stPacket.nAnimalOwner		= pPlayer->GetID();
	stPacket.nResult			= (WORD)eResult;
	stPacket.nAnimalContentment = pAnimal->GetContentment();
	stPacket.nAnimalInterest	= pAnimal->GetInterest();

	if (eResult == CSHMain::RESULT_OK)
	{
		SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));
		// 농장UI 정보 전송
		//SRV_SendFarmUIInfoToClient(eFarmZone, nFarmID, nGardenID, pCrop, pFarm->GetOwner());
	}
	else
	{
		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestUpgradeFarm Method																 	서버에 농장 오브젝트 업그레이드 요청
//
VOID CSHFarmManager::CLI_RequestUpgradeFarm(CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, int nObjKind)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	PACKET_FARM_UPGRADE stPacket;

	stPacket.Category 	= MP_FARM;
	stPacket.Protocol 	= MP_FARM_UPGRADE;
	stPacket.dwObjectID	= gHeroID;
	stPacket.nFarmZone	= WORD(eFarmZone);
	stPacket.nFarmID	= nFarmID;
	stPacket.nObjKind	= WORD(nObjKind);

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestPayTax Method																 					 서버에 관리비 납부 요청
//
VOID CSHFarmManager::CLI_RequestPayTax(UINT nKind)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	// 090707 LUJ, 인벤토리를 조회해서 세금 납부에 필요한 아이템을 얻어온다
	CHero* const hero = OBJECTMGR->GetHero();

	if( 0 == hero )
	{
		return;
	}

	PACKET_FARM_TAX_SYN message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_FARM;
	message.Protocol = MP_FARM_PAY_TAX;
	message.dwObjectID = gHeroID;
	message.dwData1 = m_stSelFarmObj.nFarmZone;
	message.dwData2 = m_stSelFarmObj.nFarmID;
	message.dwData4 = nKind;

	// 090707 LUJ, 이때는 서버에 요청해서 납부할 관리비를 얻어온다
	if( 0 == nKind )
	{
		m_ePayTaxMode = ePayTaxMode_FarmNpc;

		NETWORK->Send( &message, message.GetSize() );
		return;
	}
	else if( 2 == nKind || 3 == nKind )
	{
		// 091204 ONS 패밀리다이얼로그의 세금납부버튼납부시 필요한 농장정보를 전송한다.
		// 농장이외의 맵은 농장정보를 가지고 있지 않으므로 농장인터페이스로 전송된 기존 농장정보를 이용한다.
		m_ePayTaxMode = ePayTaxMode_PayBtn;

		CSHFarmManageDlg* pDlg = GAMEIN->GetFarmManageDlg();
		if(!pDlg) return;

		message.dwData1 = pDlg->GetFarmZoneNum();
		message.dwData2 = pDlg->GetFarmID();

		if( 2 == nKind )
		{
			NETWORK->Send( &message, message.GetSize() );
			return;
		}
	}

	// 091204 ONS 관리비납부모드에 따라서 다른방법으로 텃밭레벨을 구해온다.
	WORD wFarmGrade = 0;
	if( ePayTaxMode_FarmNpc == m_ePayTaxMode )
	{
		CSHFarmZone* const farmZone = GetFarmZone(
			WORD(m_stSelFarmObj.nFarmZone));

		if( 0 == farmZone )
		{
			return;
		}

		CSHFarm* const farm = farmZone->GetFarm( m_stSelFarmObj.nFarmID );

		if( 0 == farm )
		{
			return;
		}
		wFarmGrade = farm->GetGarden( 0 )->GetGrade();
	}
	else	// ePayTaxMode_FamilyBtn
	{
		CSHFarmManageDlg* pDlg = GAMEIN->GetFarmManageDlg();
		if(!pDlg) return;

		wFarmGrade = pDlg->GetFarmGrade();
	}

	const Tax& tax = GetTax(wFarmGrade);

	for( DWORD i = 0; i < tax.mItemSize; ++i )
	{
		const Tax::Item& taxItem = tax.mItem[ i ];
		DURTYPE paidItemQuantity = 0;

		for(POSTYPE position = POSTYPE(TP_INVENTORY_END + TABCELL_INVENTORY_NUM * hero->Get_HeroExtendedInvenCount());
			TP_INVENTORY_START < position--; )
		{
			CItem* const item = ITEMMGR->GetItemofTable( eItemTable_Inventory, position );

			if( 0 == item )
			{
				continue;
			}
			else if( taxItem.mIndex != item->GetItemIdx() )
			{
				continue;
			}

			const DWORD taxItemSize = sizeof( message.mItem ) / sizeof( *message.mItem );

			if( taxItemSize <= message.mSize )
			{
				break;
			}

			paidItemQuantity += ( ITEMMGR->IsDupItem( taxItem.mIndex ) ? item->GetDurability() : 1 );

			ICONBASE& iconBase = message.mItem[ message.mSize++ ];
			iconBase.dwDBIdx = item->GetDBIdx();
			iconBase.Position = item->GetPosition();
			iconBase.wIconIdx = item->GetItemIdx();

			if( taxItem.mQuantity <= paidItemQuantity )
			{
				break;
			}
		}

		if( taxItem.mQuantity > paidItemQuantity )
		{
			TCHAR textMessage[ MAX_PATH ] = { 0 };
			_stprintf(
				textMessage,
				CHATMGR->GetChatMsg( 1333 ),
				CLI_GetTaxText( 0 ) );
			WINDOWMGR->MsgBox(
				MBI_NOTICE,
				MBT_OK,
				textMessage );

			// 091207 ONS 소지금(아이템) 부족시 아이템들의 잠김상태를 해제한다.
			for( DWORD i = 0; i < message.mSize; ++i )
			{
				const ICONBASE& iconBase = message.mItem[ i ];
				CItem* const item = ITEMMGR->GetItemofTable( eItemTable_Inventory, iconBase.Position );

				if( item )
				{
					item->SetLock( FALSE );
				}
			}
			return;
		}

		// 090710 LUJ, 서버에 처리 요청한 아이템을 잠금 처리한다
		for( DWORD i = 0; i < message.mSize; ++i )
		{
			const ICONBASE& iconBase = message.mItem[ i ];
			CItem* const item = ITEMMGR->GetItemofTable( eItemTable_Inventory, iconBase.Position );

			if( item )
			{
				item->SetLock( TRUE );
			}
		}
	}

	NETWORK->Send( &message, message.GetSize() );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestFarmInfo Method																 					   서버에 농장 정보 요청
//
VOID CSHFarmManager::CLI_RequestFarmInfo(CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	PACKET_FARM_STATE stPacket;

	stPacket.Category 	= MP_FARM;
	stPacket.Protocol 	= MP_FARM_FARMSTATE_LOAD_FROM_DB;
	stPacket.dwObjectID	= gHeroID;
	stPacket.nFarmZone	= WORD(eFarmZone);
	stPacket.nFarmID	= nFarmID;

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_ParseAnswerFromSrv Method																			  서버로부터의 응답 분석/처리
//
VOID CSHFarmManager::CLI_ParseAnswerFromSrv(void* pMsg)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
 	MSGBASE* pTmp = (MSGBASE*)pMsg;

	switch(pTmp->Protocol)
	{
	// DB 큐로부터 농장 상태 설정
	case MP_FARM_FARMSTATE_LOAD_FROM_DB:
		{
			PACKET_FARM_STATE* pPacket = (PACKET_FARM_STATE*)pMsg;

			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if (pFarmZone == NULL) return;
			CSHFarm*	 pFarm	   = pFarmZone->GetFarm(pPacket->nFarmID);

			pFarm->SetOwner(pPacket->nFarmOwner);
			pFarm->SetOwnState((CSHFarmObj::OWN_STATE)pPacket->nFarmState);
			pFarm->GetGarden(0)->SetGrade(pPacket->nGardenGrade);
			pFarm->GetFence(0)->SetGrade(pPacket->nFenceGrade);
			pFarm->GetAnimalCage(0)->SetGrade(pPacket->nAnimalCageGrade);
			pFarm->GetHouse(0)->SetGrade(pPacket->nHouseGrade);
			pFarm->GetWarehouse(0)->SetGrade(pPacket->nWarehouseGrade);
			// 새로운 오브젝트 추가
			if (pFarm->GetIsRendering()) CLI_AddRenderFarmObjToScreen(pFarm);
		}
		break;

	// DB 큐로부터 농작물 상태 설정
	case MP_FARM_CROPINFO_LOAD_FROM_DB:
		{
 			PACKET_FARM_GARDEN_CROP_PLANT* pPacket = (PACKET_FARM_GARDEN_CROP_PLANT*)pMsg;

			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if (pFarmZone == NULL) return;
			CSHFarm*	 pFarm	   = pFarmZone->GetFarm(pPacket->nFarmID);
			CSHGarden*	 pGarden   = pFarm->GetGarden(pPacket->nGardenID);
			CSHCrop*	 pCrop	   = pGarden->GetCrop(pPacket->nCropID);

			pCrop->SetOwner(pPacket->nCropOwner);
			pCrop->SetStep((CSHCrop::CROP_STEP)pPacket->nCropStep);
			pCrop->SetLife(pPacket->nCropLife);	
			pCrop->SetSeedGrade(pPacket->nCropSeedGrade);
			pCrop->SetKind(pPacket->nCropKind);
			pCrop->SetNextStepTime(pPacket->nCropNextStepTime);
			// 새로운 오브젝트 추가
			if (pFarm->GetIsRendering()) CLI_AddRenderFarmCropObjToScreen(pCrop);
		}
		break;
	// 080417 KTH -- DB 큐로부터 가축 상태 설정
	case MP_FARM_ANIMALINFO_LOAD_FROM_DB:
		{
			PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT* pPacket = (PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT*)pMsg;

			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if( pFarmZone == NULL ) return;
			CSHFarm*	pFarm			= pFarmZone->GetFarm(pPacket->nFarmID);
			CPen*		pAnimalCage		= pFarm->GetAnimalCage(pPacket->nAnimalCageID);
			CAnimal*	pAnimal			= pAnimalCage->GetAnimal(pPacket->nAnimalID);

			pAnimal->SetOwner(pPacket->nAnimalOwner);
			pAnimal->SetStep((CAnimal::ANIMAL_STEP)pPacket->nAnimalStep);
			pAnimal->SetLife(pPacket->nAnimalLife);
			pAnimal->SetKind(pPacket->nAnimalKind);
			pAnimal->SetNextStepTime(pPacket->nAnimalNextStepTime);
			pAnimal->SetContentment(pPacket->nAnimalContentment);
			pAnimal->SetInterest(pPacket->nAnimalInterest);

			if( pFarm->GetIsRendering() ) CLI_AddRenderFarmAnimalObjToScreen(pAnimal);
		}
		break;

	// 농장UI 농장 정보 
	case MP_FARM_MANAGE_UI_FARM_DATA:
		{
			PACKET_FARM_STATE_WITH_TAX* pPacket = (PACKET_FARM_STATE_WITH_TAX*)pMsg;
			CSHFarmManageDlg* pDlg = GAMEIN->GetFarmManageDlg();

			if (HERO == NULL) return;

			if (pDlg)
			{
				// 091126 ONS 농장 UI정보를 설정한다.
				pDlg->SetFarm(pPacket->nFarmZone, pPacket->nFarmID, pPacket->nGardenGrade);
				CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
				if (pMiniMapDlg)
				{
					CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

					if (pFarmZone == NULL) return;
					CSHFarm*	 pFarm	   = pFarmZone->GetFarm(pPacket->nFarmID);

					pMiniMapDlg->AddFarmIcon(pFarm);

					// 관리비 납부 상태
					pFarm->SetTaxArrearageFreq(pPacket->nTaxArrearageFreq);
					pFarm->SetTaxPayPlayerName(pPacket->szTaxPayPlayerName);
				}
			}			
		}
		break;

	// 농장UI 농작물 정보 
	case MP_FARM_MANAGE_UI_CROP_DATA:
		{
			//091116 pdy 농장 Event로인한 UI정보가 HERO가 유효하지 않을때 올수있어 클라이언트가 오류나는 버그 수정  
			if (HERO == NULL) return;

 			PACKET_FARM_GARDEN_CROP_PLANT* pPacket = (PACKET_FARM_GARDEN_CROP_PLANT*)pMsg;
			CSHCrop* pCrop = GAMEIN->GetFarmManageDlg()->GetFarm()->GetGarden(0)->GetCrop(pPacket->nCropID);

			pCrop->SetID(pPacket->nCropID);
			pCrop->SetOwner(pPacket->nCropOwner);
			pCrop->SetStep((CSHCrop::CROP_STEP)pPacket->nCropStep);
			pCrop->SetLife(pPacket->nCropLife);	
			pCrop->SetSeedGrade(pPacket->nCropSeedGrade);
			pCrop->SetKind(pPacket->nCropKind);
 			pCrop->SetNextStepTime(pPacket->nCropNextStepTime);
			GAMEIN->GetFarmManageDlg()->SetCropImg(pPacket->nCropID, pPacket->nCropKind, pPacket->nCropOwner);
			GAMEIN->GetFarmManageDlg()->SetCropViewInfo(pPacket->nCropID, TRUE);
		}
		break;

	// 농장 구입
	case MP_FARM_BUY:
		{
			PACKET_FARM_STATE* pPacket = (PACKET_FARM_STATE*)pMsg;

			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if (pFarmZone == NULL) return;
			CSHFarm*	 pFarm	   = pFarmZone->GetFarm(pPacket->nFarmID);

			switch(pPacket->nResult)
			{
			case CSHMain::RESULT_OK:
				pFarm->SetOwnState(CSHFarmObj::OWN_STATE_OWNED);
				pFarm->SetOwner(pPacket->nFarmOwner);
				pFarm->GetGarden(0)->SetGrade(pPacket->nGardenGrade);
				pFarm->GetHouse(0)->SetGrade(pPacket->nHouseGrade);
				pFarm->GetFence(0)->SetGrade(pPacket->nFenceGrade);
				pFarm->GetAnimalCage(0)->SetGrade(pPacket->nAnimalCageGrade);
				pFarm->GetWarehouse(0)->SetGrade(pPacket->nWarehouseGrade);

				// 구입 창 닫기
				if (GAMEIN->GetFarmBuyDlg())
				{
					GAMEIN->GetFarmBuyDlg()->SetActive(FALSE);
				}

				// 기존 오브젝트 제거
				CLI_RemoveRenderFarmObjFromScreen(pFarm);
				// 새로운 오브젝트 추가
				CLI_AddRenderFarmObjToScreen(pFarm);

				if (gHeroID == pFarm->GetOwner())
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1105 ) );
				break;

			case CSHMain::RESULT_FAIL_01:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1044 ) );
				break;
			case CSHMain::RESULT_FAIL_02:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1045 ) );
				break;
			case CSHMain::RESULT_FAIL_03:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1046 ) );
				break;
			case CSHMain::RESULT_FAIL_04:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1047 ) );
				break;
			case CSHMain::RESULT_FAIL_05:	
			case CSHMain::RESULT_FAIL_06:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1048 ) );
				break;
			case CSHMain::RESULT_FAIL_07:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1049 ) );
				break;
			case CSHMain::RESULT_FAIL_08:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1050 ) );
				break;
			case CSHMain::RESULT_FAIL_09:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1993 ));
				break;
			}
		}
		break;
	// 농장 삭제
	case MP_FARM_DEL:
		{
			PACKET_FARM_STATE* pPacket = (PACKET_FARM_STATE*)pMsg;

			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if (pFarmZone == NULL) return;
			CSHFarm*	 pFarm	   = pFarmZone->GetFarm(pPacket->nFarmID);
			CSHGarden*	 pGarden   = pFarm->GetGarden(0);
			// 080423 KTH
			CPen* pAnimalCage = pFarm->GetAnimalCage(0);

			// 삭제
			pFarm->Init();
			for(int i=0; i<pGarden->GetCropNum(); i++)
			{
				pGarden->GetCrop(i)->SetCrop(CSHCrop::CROP_RESULT_DIE);
			}
			// 080423 KTH
			for( int i=0; i<pAnimalCage->GetAnimalNum(); i++ )
			{
				pAnimalCage->GetAnimal(i)->SetAnimal(CAnimal::ANIMAL_RESULT_DIE);
			}

			// 기존 오브젝트 제거
			CLI_RemoveRenderFarmObjFromScreen(pFarm);
			// 새로운 오브젝트 추가
			CLI_AddRenderFarmObjToScreen(pFarm);
		}
		break;
	// 텃밭에 농작물 심기
	case MP_FARM_GARDEN_PLANT:
		{
 			PACKET_FARM_GARDEN_CROP_PLANT* pPacket = (PACKET_FARM_GARDEN_CROP_PLANT*)pMsg;

 			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if (pFarmZone == NULL) return;
			CSHFarm*	 pFarm	   = pFarmZone->GetFarm(pPacket->nFarmID);
			CSHGarden*	 pGarden   = pFarm->GetGarden(pPacket->nGardenID);
			CSHCrop*	 pCrop	   = pGarden->GetCrop(pPacket->nCropID);

 			switch(pPacket->nResult)
			{
			case CSHMain::RESULT_OK:
				{
					// 심기
					pCrop->Plant(pPacket->nCropOwner, pPacket->nCropSeedGrade, pPacket->nCropKind);
					// 캐릭터 모션 연출
					CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pCrop->GetOwner());
					if( !pPlayer )	return;
					pPlayer->StartFarmMotion(FARM_PLANT_CHAR_MOTION_NUM, CSHCrop::CROP_PLANT_SEEDING_TIME);
					
					if (gHeroID == pPlayer->GetID())
					{
						CSHMonstermeterDlg* pDlg = GAMEIN->GetMonstermeterDlg() ;
						if (pDlg) pDlg->GetFarmRePlantTimeBar()->CLI_Start(CSHProgressBar::BARTYPE_FARM_REPLANT, NULL, CSHCrop::CROP_PLANT_RETRY_TIME*1000*60);
					}

					CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;
					if (pGuageDlg)
					{
						if ((CObject*)pGarden->GetRenderObjEx(pCrop->GetID()+15) == OBJECTMGR->GetSelectedObject())
						{
							pGuageDlg->GetMonsterGuage()->SetActive(TRUE);
						}
					}
				}
				break;
			case CSHMain::RESULT_FAIL_02:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1051 ), 2 );
				break;
			case CSHMain::RESULT_FAIL_03:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1052 ), CSHCrop::CROP_PLANT_RETRY_TIME );
				break;
			case CSHMain::RESULT_FAIL_04:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1053 ) );
				break;
			case CSHMain::RESULT_FAIL_05:	
			case CSHMain::RESULT_FAIL_06:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1054 ) );
				break;
			case CSHMain::RESULT_FAIL_07:	
 				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1160 ), FARM_BUY_FAMILY_MEMNUM );
				break;
			}
		}
		break;

	// 텃밭의 농작물 가꾸기
	case MP_FARM_GARDEN_GROW:
		{
			PACKET_FARM_GARDEN_CROP_GROW* pPacket = (PACKET_FARM_GARDEN_CROP_GROW*)pMsg;
			
			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if (pFarmZone == NULL) return;
			CSHFarm*	 pFarm	   = pFarmZone->GetFarm(pPacket->nFarmID);
			CSHGarden*	 pGarden   = pFarm->GetGarden(pPacket->nGardenID);
			CSHCrop*	 pCrop	   = pGarden->GetCrop(pPacket->nCropID);

			switch(pPacket->nResult)
			{
			case CSHCrop::CROP_RESULT_LIFE:
				{
					// 농작물 설정
					pCrop->SetLife(pPacket->nValue);
					// 다이얼로그에 변화 적용
					CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;
					if (pGuageDlg)
					{
						if ((CObject*)pCrop->GetRenderObj() == OBJECTMGR->GetSelectedObject())
						{
							pGuageDlg->SetMonsterLife(pCrop->GetLife(), pCrop->GetMaxLife());
						}
					}
				}
				break;

			case CSHCrop::CROP_RESULT_DIE:
				{
					// 오브젝트 표시 설정
					// 타켓 해제
					CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;
					if (pGuageDlg)
					{
						if ((CObject*)pCrop->GetRenderObj() == OBJECTMGR->GetSelectedObject())
						{
							pGuageDlg->GetMonsterGuage()->SetActive(FALSE);
 							if(OBJECTMGR->GetSelEffectHandle())
							{
								EFFECTMGR->ForcedEndEffect(OBJECTMGR->GetSelEffectHandle());
							}

							if (OBJECTMGR->GetSelectedObject())
							{
								OBJECTMGR->GetSelectedObject()->ShowObjectName( FALSE );
								OBJECTMGR->GetSelectedObject()->OnDeselected();
								OBJECTMGR->SetSelectedObjectID(0);
							}

							CLI_SetTargetObj(NULL);
						}
					}

					// ..이펙트
					if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_DIE])
					{
						TARGETSET set;
						set.pTarget = pCrop->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_DIE], pCrop->GetRenderObj(), &set, 0, pCrop->GetRenderObj()->GetID());
					}

					// 농작물 렌더링 오브젝트 제거
					CLI_RemoveRenderFarmCropObjToScreen(pCrop);
					// 농작물 설정
					pCrop->SetCrop((CSHCrop::CROP_RESULT)pPacket->nResult);
					pGarden->GetRenderObjEx(pPacket->nCropID+15)->GetEngineObject()->EnablePick();
				}
				break;

			case CSHCrop::CROP_RESULT_STEP_UP:
				{
					// 기존 오브젝트가 선택되어 있었다면..
					BOOL bSel = FALSE;
					if ((CObject*)pCrop->GetRenderObj() == OBJECTMGR->GetSelectedObject())
					{
						// 타켓 해제
 						if(OBJECTMGR->GetSelEffectHandle())
						{
							EFFECTMGR->ForcedEndEffect(OBJECTMGR->GetSelEffectHandle());
						}

						if (OBJECTMGR->GetSelectedObject())
						{
							OBJECTMGR->GetSelectedObject()->ShowObjectName( FALSE );
							OBJECTMGR->GetSelectedObject()->OnDeselected();
							OBJECTMGR->SetSelectedObjectID(0);
						}

						CLI_SetTargetObj(NULL);
						bSel = TRUE;
					}
					// 기존 오브젝트 제거
					CLI_RemoveRenderFarmCropObjToScreen(pCrop);
					// 농작물 설정
					pCrop->SetCrop((CSHCrop::CROP_RESULT)pPacket->nResult);
					// 새로운 오브젝트 추가
					CLI_AddRenderFarmCropObjToScreen(pCrop);
					if (bSel)
					{
						CActionTarget Target;
						Target.InitActionTarget( pCrop->GetRenderObj(), NULL );
						// 조작할 농작물 선택
						g_csFarmManager.CLI_SetTargetObj(pCrop->GetRenderObj());
						pCrop->GetRenderObj()->ShowObjectName( TRUE, NAMECOLOR_SELECTED );
						OBJECTMGR->SetSelectedObject( pCrop->GetRenderObj() );
					}
					// 이름 변경
					char* pszName = m_pszTblFarmCropName[CSHCrop::CROP_STEP_MAX*(pCrop->GetKind()-1)+pCrop->GetStep()-CSHCrop::CROP_STEP_1].szName;
					pCrop->GetRenderObj()->SetName(pszName);
					pCrop->GetRenderObj()->GetObjectBalloon()->GetOBalloonName()->SetName(pszName);
					// 다이얼로그에 이름 변경
					{
						CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;
						if (pGuageDlg)
						{
							if ((CObject*)pCrop->GetRenderObj() == OBJECTMGR->GetSelectedObject())
							{
								pGuageDlg->SetName( (CObject*)pCrop->GetRenderObj(), pszName) ; 
								pGuageDlg->SetMonsterLife(pCrop->GetLife(), pCrop->GetMaxLife());
							}
						}
					}
					// ..이펙트
					if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_STEPUP])
					{
						TARGETSET set;
						set.pTarget = pCrop->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_STEPUP], pCrop->GetRenderObj(), &set, 0, pCrop->GetRenderObj()->GetID());
					}
				}
				break;

			case CSHCrop::CROP_RESULT_STEP_UP_COMPLETE:
				{
					BOOL bSel = FALSE;
					if ((CObject*)pCrop->GetRenderObj() == OBJECTMGR->GetSelectedObject())
					{
						// 타켓 해제
 						if(OBJECTMGR->GetSelEffectHandle())
						{
							EFFECTMGR->ForcedEndEffect(OBJECTMGR->GetSelEffectHandle());
						}

						if (OBJECTMGR->GetSelectedObject())
						{
							OBJECTMGR->GetSelectedObject()->ShowObjectName( FALSE );
							OBJECTMGR->GetSelectedObject()->OnDeselected();
							OBJECTMGR->SetSelectedObjectID(0);
						}

						CLI_SetTargetObj(NULL);
						bSel = TRUE;
					}
					// 기존 오브젝트 제거
					CLI_RemoveRenderFarmCropObjToScreen(pCrop);
					// 농작물 설정
					pCrop->SetCrop((CSHCrop::CROP_RESULT)pPacket->nResult);
					// 새로운 오브젝트 추가
					CLI_AddRenderFarmCropObjToScreen(pCrop);
					if (bSel)
					{
						CActionTarget Target;
						Target.InitActionTarget( pCrop->GetRenderObj(), NULL );
						// 조작할 농작물 선택
						g_csFarmManager.CLI_SetTargetObj(pCrop->GetRenderObj());
						pCrop->GetRenderObj()->ShowObjectName( TRUE, NAMECOLOR_SELECTED );
						OBJECTMGR->SetSelectedObject( pCrop->GetRenderObj() );
					}
					// 이름 변경
					char* pszName = m_pszTblFarmCropName[CSHCrop::CROP_STEP_MAX*(pCrop->GetKind()-1)+pCrop->GetStep()-CSHCrop::CROP_STEP_1].szName;
					pCrop->GetRenderObj()->SetName(pszName);
					pCrop->GetRenderObj()->GetObjectBalloon()->GetOBalloonName()->SetName(pszName);
					// 다이얼로그에 이름 변경
					{
						CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;
						if (pGuageDlg)
						{
							if ((CObject*)pCrop->GetRenderObj() == OBJECTMGR->GetSelectedObject())
							{
								pGuageDlg->SetName( (CObject*)pCrop->GetRenderObj(), pszName ) ; 
							}
						}
					}
					// ..이펙트
					if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_STEPUP])
					{
						TARGETSET set;
						set.pTarget = pCrop->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_STEPUP], pCrop->GetRenderObj(), &set, 0, pCrop->GetRenderObj()->GetID());
					}
				}
				break;
			case CSHCrop::CROP_RESULT_NEXT_STEP_TIME:
				{
					pCrop->SetNextStepTime(pPacket->nValue);
				}
				break;
			}
		}
		break;

	// 텃밭의 농작물 비료주기
	case MP_FARM_GARDEN_MANURE:
		{
			PACKET_FARM_GARDEN_CROP_MANURE* pPacket = (PACKET_FARM_GARDEN_CROP_MANURE*)pMsg;
			
			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if (pFarmZone == NULL) return;
			CSHFarm*	 pFarm	   = pFarmZone->GetFarm(pPacket->nFarmID);
			CSHGarden*	 pGarden   = pFarm->GetGarden(pPacket->nGardenID);
			CSHCrop*	 pCrop	   = pGarden->GetCrop(pPacket->nCropID);

			switch(pPacket->nResult)
			{
			case CSHMain::RESULT_OK:
				{
					// 캐릭터 모션 연출
					CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pCrop->GetOwner());
					if( !pPlayer )	return;
					pPlayer->StartFarmMotion(CSHFarmManager::FARM_MANURE_CHAR_MOTION_NUM, CSHCrop::CROP_PLANT_SEEDING_TIME);
					// 비료 준 시간 설정
					pCrop->SetManureTimeTick(gCurTime);
					pCrop->Manure(pPacket->nManureGrade, pPacket->nManureKind);
					// 비료로 + 되는 생명력은 랜덤이므로 서버로부터의 값으로 재설정 필요.
					pCrop->SetLife(pPacket->nCropLife);
			
					if (gHeroID == pPlayer->GetID())
					{
						CSHMonstermeterDlg* pDlg = GAMEIN->GetMonstermeterDlg() ;
						if (pDlg) pDlg->GetFarmReManureTimeBar()->CLI_Start(CSHProgressBar::BARTYPE_FARM_REMANURE, NULL, CSHCrop::CROP_MANURE_RETRY_TIME*1000);
					}
					// ..이펙트
					if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_MANURE])
					{
						TARGETSET set;
						set.pTarget = pCrop->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_MANURE], pCrop->GetRenderObj(), &set, 0, pCrop->GetRenderObj()->GetID());
					}
				}
				break;

			case CSHMain::RESULT_FAIL_02:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1055 ) );
				break;
			case CSHMain::RESULT_FAIL_03:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1056 ) );
				break;
			case CSHMain::RESULT_FAIL_04:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1057 ), CSHCrop::CROP_MANURE_RETRY_TIME );
				break;
			case CSHMain::RESULT_FAIL_05:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1058 ) );
				break;
			}
		}
		break;

	// 텃밭의 농작물 수확하기
	case MP_FARM_GARDEN_HARVEST:
		{
 			PACKET_FARM_GARDEN_CROP_GROW* pPacket = (PACKET_FARM_GARDEN_CROP_GROW*)pMsg;
			
			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if (pFarmZone == NULL) return;
			CSHFarm*	 pFarm	   = pFarmZone->GetFarm(pPacket->nFarmID);
			CSHGarden*	 pGarden   = pFarm->GetGarden(pPacket->nGardenID);
			CSHCrop*	 pCrop	   = pGarden->GetCrop(pPacket->nCropID);

			switch(pPacket->nResult)
			{
			// 모션 시작
			case CSHMain::RESULT_OK:
				{
					// 캐릭터 모션 연출
					CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pCrop->GetOwner());
					if( !pPlayer )	return;
					pPlayer->StartFarmMotion(CSHFarmManager::FARM_HARVEST_CHAR_MOTION_NUM, CSHCrop::CROP_PLANT_SEEDING_TIME);
					// 수확 모션 시간 설정
					pCrop->SetPlantTimeTick(gCurTime);
				}
				break;
			// 모션 종료 & 실제 수확
			case CSHMain::RESULT_OK_02:
				{
					// ..이펙트
					if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_HARVEST])
					{
						TARGETSET set;
						set.pTarget = pCrop->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_CROP_HARVEST], pCrop->GetRenderObj(), &set, 0, pCrop->GetRenderObj()->GetID());
					}

					// 기존 오브젝트 제거
 					CLI_RemoveRenderFarmCropObjToScreen(pCrop);
					pGarden->GetRenderObjEx(pPacket->nCropID+15)->GetEngineObject()->EnablePick();
					pCrop->SetCrop(CSHCrop::CROP_RESULT_DIE);

					if ((CObject*)pCrop->GetRenderObj() == OBJECTMGR->GetSelectedObject())
					{
						// 타켓 해제
 						if(OBJECTMGR->GetSelEffectHandle())
						{
							EFFECTMGR->ForcedEndEffect(OBJECTMGR->GetSelEffectHandle());
						}

						if (OBJECTMGR->GetSelectedObject())
						{
							OBJECTMGR->GetSelectedObject()->ShowObjectName( FALSE );
							OBJECTMGR->GetSelectedObject()->OnDeselected();
							OBJECTMGR->SetSelectedObjectID(0);
						}

						//091117 pdy 농장 이벤트로인해 주변 농장의 타겟이 해제되는 버그 수정 
						CLI_SetTargetObj(NULL);
					}
				}
				break;
			case CSHMain::RESULT_FAIL_01:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1059 ) );
				break;
			case CSHMain::RESULT_FAIL_02:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1060 ) );
				break;
			case CSHMain::RESULT_FAIL_03:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1061 ) );
				break;
			case CSHMain::RESULT_FAIL_04:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1062 ) );
				break;
			}
		}
		break;
	// 농장 업그레이드
	case MP_FARM_UPGRADE:
		{
 			PACKET_FARM_UPGRADE* pPacket = (PACKET_FARM_UPGRADE*)pMsg;
			
			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if (pFarmZone == 0) return;

			CSHFarm* const pFarm = pFarmZone->GetFarm(pPacket->nFarmID);

			if(0 == pFarmZone)
			{
				return;
			}

			int nRenderObjKind = 0;
			WORD nGrade = 0;

			switch(pPacket->nResult)
			{
			case CSHMain::RESULT_OK:
 				switch(pPacket->nObjKind)
				{
				// ..텃밭
				case CSHFarm::FARM_EVENT_GARDEN:
					pFarm->GetGarden(0)->SetGrade(nGrade = pFarm->GetGarden(0)->GetGrade()+1);
					nRenderObjKind = RENDER_GARDENPATCH_KIND;

					if (gHeroID == pFarm->GetOwner())
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1106 ), nGrade );
					// ..이펙트
					if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_GARDEN])
					{
						TARGETSET set;
						set.pTarget = pFarm->GetGarden(0)->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_GARDEN], pFarm->GetGarden(0)->GetRenderObjEx(8), &set, 0, pFarm->GetGarden(0)->GetRenderObj()->GetID());
					}
					break;
				// ..축사
				case CSHFarm::FARM_EVENT_ANIMALCAGE:
					pFarm->GetAnimalCage(0)->SetGrade(nGrade = pFarm->GetAnimalCage(0)->GetGrade()+1);
					nRenderObjKind = RENDER_ANIMALCAGE_KIND;

					if (gHeroID == pFarm->GetOwner())
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1107 ), nGrade );
					// ..이펙트
					if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_ANIMALCAGE])
					{
						TARGETSET set;
						set.pTarget = pFarm->GetAnimalCage(0)->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_ANIMALCAGE], pFarm->GetAnimalCage(0)->GetRenderObj(), &set, 0, pFarm->GetAnimalCage(0)->GetRenderObj()->GetID());
					}
					break;
				// ..울타리
				case CSHFarm::FARM_EVENT_FENCE:
					pFarm->GetFence(0)->SetGrade(nGrade =pFarm->GetFence(0)->GetGrade()+1);
					nRenderObjKind = RENDER_FENCE_KIND;

					if (gHeroID == pFarm->GetOwner())
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1109 ), nGrade );
					// ..이펙트
					if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_FENCE])
					{
						TARGETSET set;
						set.pTarget = pFarm->GetFence(0)->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_FENCE], pFarm->GetFence(0)->GetRenderObj(), &set, 0, pFarm->GetFence(0)->GetRenderObj()->GetID());
					}
					break;
				// ..집
				case CSHFarm::FARM_EVENT_HOUSE:
					pFarm->GetHouse(0)->SetGrade(nGrade =pFarm->GetHouse(0)->GetGrade()+1);
					nRenderObjKind = RENDER_HOUSE_KIND;

					if (gHeroID == pFarm->GetOwner())
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1108 ), nGrade );
					// ..이펙트
					if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_HOUSE])
					{
						TARGETSET set;
						set.pTarget = pFarm->GetHouse(0)->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_HOUSE], pFarm->GetHouse(0)->GetRenderObj(), &set, 0, pFarm->GetHouse(0)->GetRenderObj()->GetID());
					}
					break;
				// ..창고
				case CSHFarm::FARM_EVENT_WAREHOUSE:
					pFarm->GetWarehouse(0)->SetGrade(nGrade =pFarm->GetWarehouse(0)->GetGrade()+1);
					nRenderObjKind = RENDER_WAREHOUSE_KIND;

					if (gHeroID == pFarm->GetOwner())
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1110 ), nGrade );
					// ..이펙트
					if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_WAREHOUSE])
					{
						TARGETSET set;
						set.pTarget = pFarm->GetWarehouse(0)->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_UPGRADE_WAREHOUSE], pFarm->GetWarehouse(0)->GetRenderObj(), &set, 0, pFarm->GetWarehouse(0)->GetRenderObj()->GetID());
					}
					break;
				}

				// 기존 오브젝트 제거
				CLI_RemoveRenderFarmObjFromScreen(pFarm, nRenderObjKind);
				// 새로운 오브젝트 추가
				CLI_AddRenderFarmObjToScreen(pFarm, nRenderObjKind);

				// 최대 등급이 되면 대화창 닫기
				if (nGrade >= CSHFarmObj::MAX_GRADE-2)
				{
					if (GAMEIN->GetFarmUpgradeDlg())
					{
						GAMEIN->GetFarmUpgradeDlg()->SetActive(FALSE);
					}
				}

				if (nGrade >= CSHFarmObj::MAX_GRADE-4 && pPacket->nObjKind == CSHFarm::FARM_EVENT_GARDEN)
				{
					if (GAMEIN->GetFarmUpgradeDlg())
					{
						GAMEIN->GetFarmUpgradeDlg()->SetActive(FALSE);
					}
				}
				break;

			case CSHMain::RESULT_FAIL_01:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1063 ) );
				break;
			case CSHMain::RESULT_FAIL_02:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1064 ) );
				break;
			case CSHMain::RESULT_FAIL_03:	
			case CSHMain::RESULT_FAIL_04:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1065 ) );
				break;
			case CSHMain::RESULT_FAIL_05:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1066 ) );
				break;
			case CSHMain::RESULT_FAIL_06:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1160 ), FARM_BUY_FAMILY_MEMNUM );
				break;
			}
		}
		break;
	case MP_FARM_TIMEDELAY:
		{
 			MSG_DWORD3* pPacket = (MSG_DWORD3*)pMsg;

			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pPacket->dwObjectID);
			if( !pPlayer )	return;

			CSHMonstermeterDlg* pDlg = GAMEIN->GetMonstermeterDlg() ;
			if (pDlg == NULL) return;

			switch(pPacket->dwData1)
			{
			case FARM_TIMEDELAY_KIND_PLANT:		pDlg->GetFarmRePlantTimeBar()->CLI_Start(CSHProgressBar::BARTYPE_FARM_REPLANT, NULL, pPacket->dwData2*1000*60, pPacket->dwData3*1000*60);	break;
			case FARM_TIMEDELAY_KIND_MANURE:	pDlg->GetFarmReManureTimeBar()->CLI_Start(CSHProgressBar::BARTYPE_FARM_REMANURE, NULL, pPacket->dwData2*1000*60, pPacket->dwData3*1000*60);	break;
			// 080508 KTH -- 가축 먹이 주기 딜레이 추가
			//case FARM_TIMEDELAY_KIND_FEED:		pDlg->GetFarmReManureTimeBar()->CLI_Start(CSHProgressBar::BARTYPE_FARM_REMANURE, NULL, pPacket->dwData2*1000*60, pPacket->dwData3*1000*60);	break;
			}
			
		}
		break;
	// 관리비 납부
	case MP_FARM_PAY_TAX:
		{
			char szTxt[ MAX_PATH ] = { 0 };
 			PACKET_FARM_TAX_ACK* const pPacket = ( PACKET_FARM_TAX_ACK* )pMsg;

			switch(pPacket->dwData1)
			{
			// 납부
			case RESULT_OK:
				{
					// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 추가.
					if( ePayTaxMode_FarmNpc == m_ePayTaxMode )
					{
						CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->dwData2));

						if (pFarmZone == NULL) return;
						CSHFarm*	 pFarm	   = pFarmZone->GetFarm((WORD)pPacket->dwData3);

						pFarm->SetTaxArrearageFreq(0);
						pFarm->SetTaxPayPlayerName(pPacket->Name);
					}

					if (gHeroID != pPacket->dwObjectID)
					{
						break;
					}

					WINDOWMGR->MsgBox( MBI_FARM_TAX_PAY_RESULT, MBT_OK, CHATMGR->GetChatMsg( 1330 ) );

					// 090707 LUJ, 아이템 소모 처리
					for( DWORD packetArrayIndex = 0; packetArrayIndex < pPacket->mSize; ++packetArrayIndex )
					{
						const ITEMBASE& itemBase = pPacket->mItem[ packetArrayIndex ];
						CItem* const item = ITEMMGR->GetItem( itemBase.dwDBIdx );

						if( 0 == item )
						{
							continue;
						}
						else if( item->GetItemIdx() != itemBase.wIconIdx )
						{
							continue;
						}

						item->SetLock( FALSE );

						if( itemBase.Durability )
						{
							item->SetDurability( itemBase.Durability );
						}
						else
						{
							CItem* deletedItem = 0;
							ITEMMGR->DeleteItem(
								item->GetPosition(),
								&deletedItem );
						}
					}
				}
				break;
			// 납부 금액을 받아서 창 띄우기
			case RESULT_OK_02:
				{
					const MONEYTYPE taxMoney = pPacket->dwData2;
					CHero* const hero = OBJECTMGR->GetHero();
					TCHAR textMessage[ MAX_PATH ] = { 0 };

					if( 0 == hero )
					{
						break;
					}
					else if( taxMoney > hero->GetMoney() )
					{
						_stprintf(
							textMessage,
							CHATMGR->GetChatMsg( 1333 ),
							CLI_GetTaxText( taxMoney ) );
						WINDOWMGR->MsgBox( MBI_FARM_TAX_PAY_RESULT, MBT_OK, textMessage );
						break;
					}

					_stprintf(
						textMessage,
						CHATMGR->GetChatMsg( 1331 ),
						CLI_GetTaxText( taxMoney ) );
					WINDOWMGR->MsgBox(
						MBI_FARM_TAX_PAY_REQUEST,
						MBT_YESNO,
						textMessage );
				}
				break;
			case RESULT_FAIL_01:		
				WINDOWMGR->MsgBox( MBI_FARM_TAX_PAY_RESULT, MBT_OK, CHATMGR->GetChatMsg(1054) );
				break;
			case RESULT_FAIL_02:		
				sprintf(szTxt, CHATMGR->GetChatMsg(1332), pPacket->Name);
				WINDOWMGR->MsgBox( MBI_FARM_TAX_PAY_RESULT, MBT_OK, szTxt, pPacket->Name);		
				break;
			case RESULT_FAIL_03:
				{
					TCHAR textMessage[ MAX_PATH ] = { 0 };
					_stprintf(
						textMessage,
						CHATMGR->GetChatMsg( 1333 ),
						CLI_GetTaxText( pPacket->mTaxMoney ) );
					WINDOWMGR->MsgBox( MBI_FARM_TAX_PAY_RESULT, MBT_OK, textMessage );
				}
				break;
			// 091207 ONS 농장을 소유하지 않은 상태에서 세금납부 시도시 메세지 출력.
			case RESULT_FAIL_04:		
				WINDOWMGR->MsgBox( MBI_FARM_TAX_PAY_RESULT, MBT_OK, CHATMGR->GetChatMsg(1997) );
				break;
			}
		}
		break;
	// 멤버들에게 농장 삭제 알림
	case MP_FARM_DEL_NOTIFY_TO_MEMBER:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;

			switch(pPacket->dwData1)
			{
			case FARM_DEL_KIND_TAX:
				{
					WINDOWMGR->MsgBox( MBI_FARM_TAX_PAY_RESULT, MBT_OK, CHATMGR->GetChatMsg(1334));

					// 농장관리 창 닫기
					CSHFarmManageDlg* pFarmDlg = GAMEIN->GetFarmManageDlg();
					if (pFarmDlg)
					{
						pFarmDlg->Clear();
						pFarmDlg->SetActive(FALSE);
						// 미니맵의 농장 아이콘 제거
						CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
						if (pMiniMapDlg)
						{
							pMiniMapDlg->RemoveIcon(pFarmDlg->GetFarm()->GetID()+FARM_RENDER_OBJ_ID_START);
						}
						CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
						if (pBigMapDlg)
						{
							pBigMapDlg->RemoveIcon(pFarmDlg->GetFarm()->GetID()+FARM_RENDER_OBJ_ID_START);
						}
					}
				}
				break;
			}
		}
		break;
	// 관리비 미납 알림
	case MP_FARM_TAX_NONPAYMENT_NOTIFY:
		{
			MSG_DWORD* pPacket = (MSG_DWORD*)pMsg;

			char* pszDay;
			char szTxt[256];

			switch(FARM_TAX_PAY_LIMIT_DAY)
			{
			case 0: pszDay = CHATMGR->GetChatMsg(1335);	break;
			case 1: pszDay = CHATMGR->GetChatMsg(1336);	break;
			case 2: pszDay = CHATMGR->GetChatMsg(1337);	break;
			case 3: pszDay = CHATMGR->GetChatMsg(1338);	break;
			case 4: pszDay = CHATMGR->GetChatMsg(1339);	break;
			case 5: pszDay = CHATMGR->GetChatMsg(1340);	break;
			case 6: pszDay = CHATMGR->GetChatMsg(1341);	break;
			default: pszDay = CHATMGR->GetChatMsg(1335);
			}

			if (pPacket->dwData > FARM_TAX_PAY_ARREARAGE_MAX_FREQ - 1)
			{
				sprintf(szTxt, CHATMGR->GetChatMsg(1342), pszDay, FARM_TAX_PAY_LIMIT_HOUR);
			}
			else
			{
				sprintf(szTxt, CHATMGR->GetChatMsg(1343), pszDay, FARM_TAX_PAY_LIMIT_HOUR, FARM_TAX_PAY_WEEK_INTERVAL, pszDay, FARM_TAX_PAY_LIMIT_HOUR);
			}
			WINDOWMGR->MsgBox( MBI_FARM_TAX_PAY_RESULT, MBT_OK, szTxt);
		}
		break;
	// 080430 KTH -- 축사에 가축 넣기
	case MP_FARM_ANIMAL_INSTALL:
		{
			PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT* pPacket = (PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT*)pMsg;

			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if( pFarmZone == NULL ) return;
			CSHFarm*	pFarm	= pFarmZone->GetFarm(pPacket->nFarmID);
			CPen*		pPen	= pFarm->GetAnimalCage(pPacket->nAnimalCageID);
			CAnimal*	pAnimal	= pPen->GetAnimal(pPacket->nAnimalID);

			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pPacket->nOwner);

			if( !pPlayer )
			{
				return;
			}
			else
			{
				VECTOR3* pos;
				pos = pAnimal->GetPos();
				MOVEMGR->SetLookatPos(pPlayer,pos,0,gCurTime);
			}

			switch( pPacket->nResult )
			{
			case CSHMain::RESULT_OK:
				{
					pAnimal->Breed(pPacket->nAnimalOwner, pPen->GetGrade(), pPacket->nAnimalKind);
					pAnimal->SetContentment(pPacket->nAnimalContentment);
					pAnimal->SetInterest(pPacket->nAnimalInterest);

					CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pAnimal->GetOwner());
					if( !pPlayer ) return;
					// 모션
					pPlayer->StartFarmMotion(CSHFarmManager::FARM_BREED_CHAR_MOTION, CAnimal::ANIMAL_IN_STALL_TIME);

					CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg();
					if( pGuageDlg )
					{
						if( (CObject*)pPen->GetRenderObjEx(pAnimal->GetID()+5) == OBJECTMGR->GetSelectedObject() )
						{
							pGuageDlg->GetMonsterGuage()->SetActive(TRUE);
						}
					}
				}
				break;
			// 비어있는 축사가 아님
			case CSHMain::RESULT_FAIL_01:
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1557 ) );
				}
				break;
			case CSHMain::RESULT_FAIL_02:
				{
				}
				break;
			case CSHMain::RESULT_FAIL_03:
				{
				}
				break;
			// 가축과 거리가 멀다...
			case CSHMain::RESULT_FAIL_04:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1554 ) );
				break;
			// 해당 패밀리가 없거나 해당맴버가 아닐때 에러 처리
			case CSHMain::RESULT_FAIL_05:
			case CSHMain::RESULT_FAIL_06:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1555 ) );
				break;
			// 필요 패밀리 맴버수가 부족함.
			case CSHMain::RESULT_FAIL_07:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1564 ) );
				break;
			}
		}
		break;
	// 080508 KTH -- 가축 사료 주기
	case MP_FARM_ANIMAL_FEED:
		{
			PACKET_FARM_CAGE_ANIMAL_FEED* pPacket = (PACKET_FARM_CAGE_ANIMAL_FEED*)pMsg;
			
			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if (pFarmZone == NULL) return;
			CSHFarm*		pFarm		= pFarmZone->GetFarm(pPacket->nFarmID);
			CPen*			pAnimalCage	= pFarm->GetAnimalCage(pPacket->nAnimalCageID);
			CAnimal*		pAnimal		= pAnimalCage->GetAnimal(pPacket->nAnimalID);
			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pAnimal->GetOwner());

			if( !pPlayer )
				return;
			else
			{
				VECTOR3* pos;
				pos = pAnimal->GetPos();
				MOVEMGR->SetLookatPos(pPlayer,pos,0,gCurTime);
			}

			switch(pPacket->nResult)
			{
			case CSHMain::RESULT_OK:
				{
					// 캐릭터 모션 연출
					CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pAnimal->GetOwner());
					if( !pPlayer )	return;

					pPlayer->StartFarmMotion(CSHFarmManager::FARM_FEED_CHAR_MOTION, CAnimal::ANIMAL_FEED_TIME);
					// 사료 준 시간 설정
					pAnimal->SetFeedTimeTick(gCurTime);
					pAnimal->Feed(pPacket->nFoodKind);
					pAnimal->SetContentment(pPacket->nAnimalContentment);
					pAnimal->SetInterest(pPacket->nAnimalInterest);

					EFFECTMGR->StartEffectProcess(FindEffectNum("stable_cattle_03.beff"), pAnimal->GetRenderObj(), NULL, 0, pAnimal->GetID());

					if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_ANIMAL_FEED])
					{
						TARGETSET set;
						set.pTarget = pAnimal->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_ANIMAL_FEED], pAnimal->GetRenderObj(), &set, 0, pAnimal->GetRenderObj()->GetID());
					}
				}
				break;

			// 현재 해당하는곳에 가축이 없다.
			case CSHMain::RESULT_FAIL_01:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1556 ) );
				break;
			// 소유자가 틀리다!
			case CSHMain::RESULT_FAIL_02:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1557 ) );
				break;
			// 이미 가축의 성장이 끝났다!
			case CSHMain::RESULT_FAIL_03:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1558 ) );
				break;
			// 먹이를 줄수 있는 딜레이 시간이 남아있다!
			case CSHMain::RESULT_FAIL_04:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1559 ), CAnimal::ANIMAL_FEED_DELAY_TIME );
				break;
			// 가축과의 거리가 멀다
			case CSHMain::RESULT_FAIL_05:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1554 ) );
				break;
			}
			GAMEIN->GetAnimalDialog()->RefreshAnimalInfo();
		}
		break;
	// 080508 KTH -- 가축 청소 하기
	case MP_FARM_ANIMAL_CLEANING:
		{
			PACKET_FARM_CAGE_ANIMAL_FEED* pPacket = (PACKET_FARM_CAGE_ANIMAL_FEED*)pMsg;
			
			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if (pFarmZone == NULL) return;
			CSHFarm*		pFarm		= pFarmZone->GetFarm(pPacket->nFarmID);
			CPen*			pAnimalCage	= pFarm->GetAnimalCage(pPacket->nAnimalCageID);
			CAnimal*		pAnimal		= pAnimalCage->GetAnimal(pPacket->nAnimalID);

			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pAnimal->GetOwner());

			if( pPlayer != NULL )
			{
				VECTOR3* pos;
				pos = pAnimal->GetPos();
				MOVEMGR->SetLookatPos(pPlayer,pos,0,gCurTime);
			}

			switch(pPacket->nResult)
			{
			case CSHMain::RESULT_OK:
				{
					// 캐릭터 모션 연출
					CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pAnimal->GetOwner());
					if( !pPlayer )	return;
					pPlayer->StartFarmMotion(CSHFarmManager::FARM_CLEANING_CHAR_MOTION, CAnimal::ANIMAL_CLEANING_TIME);
					// 비료 준 시간 설정
					pAnimal->SetCleaningTimeTick(gCurTime);
					pAnimal->Cleaning(pPacket->nFoodKind);
					pAnimal->SetContentment(pPacket->nAnimalContentment);
					pAnimal->SetInterest(pPacket->nAnimalInterest);

					EFFECTMGR->StartEffectProcess(FindEffectNum("stable_cattle_02.beff"), pAnimal->GetRenderObj(), NULL, 0, pAnimal->GetID());
					
					if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_CLEANNING])
					{
						TARGETSET set;
						set.pTarget = pAnimal->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_CLEANNING], pAnimal->GetRenderObj(), &set, 0, pAnimal->GetRenderObj()->GetID());
					}
				}
				break;

			// 현재 해당하는 곳에 가축이 없다!
			case CSHMain::RESULT_FAIL_01:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1556 ) );
				break;
			// 소유자가 틀리다!
			case CSHMain::RESULT_FAIL_02:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1557 ) );
				break;
			// 이미 가축의 성장이 끝났다!
			case CSHMain::RESULT_FAIL_03:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1560 ) );
				break;
			// 아직 먹이를 줄 시간 딜레이가 끝나지 않았다!
			case CSHMain::RESULT_FAIL_04:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1561 ), CAnimal::ANIMAL_CLEANING_RETRY_TIME );
				break;
			case CSHMain::RESULT_FAIL_05:	
				//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1058 ) );
				break;
			}
			GAMEIN->GetAnimalDialog()->RefreshAnimalInfo();
		}
		break;
	case MP_FARM_ANIMAL_REWARD:
		{
 			PACKET_FARM_CAGE_ANIMAL_REWARD* pPacket = (PACKET_FARM_CAGE_ANIMAL_REWARD*)pMsg;
			
			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if (pFarmZone == NULL) return;
			CSHFarm*		pFarm			= pFarmZone->GetFarm(pPacket->nFarmID);
			CPen*			pAnimalCage		= pFarm->GetAnimalCage(pPacket->nAnimalCageID);
			CAnimal*		pAnimal			= pAnimalCage->GetAnimal(pPacket->nAnimalID);

			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pAnimal->GetOwner());
			if( !pPlayer )
				return;

			VECTOR3* pos;
			pos = pAnimal->GetPos();
			MOVEMGR->SetLookatPos(pPlayer,pos,0,gCurTime);

			switch(pPacket->nResult)
			{
			// 모션 시작
			case CSHMain::RESULT_OK:
				{
					// 캐릭터 모션 연출
					CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pAnimal->GetOwner());
					if( !pPlayer )	return;
					// 테스트 용도로 텃밭 수확과 같은 타임을 사용함.
					pPlayer->StartFarmMotion(CSHFarmManager::FARM_REWARD_ANIMAL_CHAR_MOTION, CAnimal::ANIMAL_STALL_REWARD_TIME);
					// 수확 모션 시간 설정
					pAnimal->SetBreedTimeTick(gCurTime);
				}
				break;
			// 모션 종료 & 실제 수확
			case CSHMain::RESULT_OK_02:
				{
					// ..이펙트
					if (FARM_EFFECT_NUM[FARM_EFFECT_KIND_ANIMAL_REWARD])
					{
						TARGETSET set;
						set.pTarget = pAnimal->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_ANIMAL_REWARD], pAnimal->GetRenderObj(), &set, 0, pAnimal->GetRenderObj()->GetID());
					}

					// 기존 오브젝트 제거
 					CLI_RemoveRenderFarmAnimalObjToScreen(pAnimal);
					pAnimalCage->GetRenderObjEx(pPacket->nAnimalID+5)->GetEngineObject()->EnablePick();
					pAnimal->SetAnimal(CAnimal::ANIMAL_RESULT_DIE);

					if ((CObject*)pAnimal->GetRenderObj() == OBJECTMGR->GetSelectedObject())
					{
						// 타켓 해제
 						if(OBJECTMGR->GetSelEffectHandle())
						{
							EFFECTMGR->ForcedEndEffect(OBJECTMGR->GetSelEffectHandle());
						}

						if(OBJECTMGR->GetSelectedObject())
						{
							OBJECTMGR->GetSelectedObject()->ShowObjectName( FALSE );
							OBJECTMGR->GetSelectedObject()->OnDeselected();
							OBJECTMGR->SetSelectedObjectID(0);
						}

						//091117 pdy 농장 이벤트로인해 주변 농장의 타겟이 해제되는 버그 수정 
						CLI_SetTargetObj(NULL);
					}

					GAMEIN->GetAnimalDialog()->SetActive(FALSE);
				}
				break;
			// 소유자가 틀리다
			case CSHMain::RESULT_FAIL_01:
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1557 ) );
				break;
			// 가축이 다 자라지 않았다
			case CSHMain::RESULT_FAIL_02:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1562 ) );
				break;
			// 가축과의 거리가 멀다.
			case CSHMain::RESULT_FAIL_03:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1554 ) );
				break;
			// 보상을 받을 공간이 없다.
			case CSHMain::RESULT_FAIL_04:	
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1563 ) );
				break;
			}
		}
		break;
	// 080417 KTH -- 가축 성장
	case MP_FARM_ANIMAL_GROW:
		{
			PACKET_FARM_CAGE_ANIMAL_GROW* pPacket = (PACKET_FARM_CAGE_ANIMAL_GROW*)pMsg;

			CSHFarmZone* pFarmZone = GetFarmZone(WORD(pPacket->nFarmZone));

			if( pFarmZone == NULL ) return;

			CSHFarm*	pFarm		= pFarmZone->GetFarm(pPacket->nFarmID);
			if( !pFarm )
				break;
			CPen*		pAnimalCage = pFarm->GetAnimalCage(pPacket->nAnimalCageID);
			if( !pAnimalCage )
				break;
			CAnimal*	pAnimal		= pAnimalCage->GetAnimal(pPacket->nAnimalID);
			if( !pAnimal )
				break;

			switch( pPacket->nResult )
			{
			// 만족도 변화
			case CAnimal::ANIMAL_RESULT_CONTENTMENT:
				{
					pAnimal->SetContentment(pPacket->nValue);
					GAMEIN->GetAnimalDialog()->RefreshAnimalInfo();
				}
				break;
			// 관심도 변화
			case CAnimal::ANIMAL_RESULT_INTEREST:
				{
					pAnimal->SetInterest(pPacket->nValue);
					GAMEIN->GetAnimalDialog()->RefreshAnimalInfo();
				}
				break;
			// 생명력 변화..
			case CAnimal::ANIMAL_RESULT_LIFE:
				{
					pAnimal->SetLife(pPacket->nValue);

					if(pPacket->nValue == 0)
						pAnimal->SetStep(CAnimal::ANIMAL_STEP_COMPLETE);

					CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg();
					if( pGuageDlg )
					{
						if( (CObject*)pAnimal->GetRenderObj() == OBJECTMGR->GetSelectedObject() )
						{
							pGuageDlg->SetMonsterLife(pAnimal->GetContentment(), 100);
						}
					}
					GAMEIN->GetAnimalDialog()->RefreshAnimalInfo();
				}
				break;
			case CAnimal::ANIMAL_RESULT_REWARD:
				{
					//if( pAnimal->GetInterest() > 10 && pAnimal->GetContentment() == 100 )
					//	GAMEIN->GetAnimalDialog()->GetItemBtn()->SetActive(TRUE);
						//GAMEIN->GetAnimalDialog()->AbleGetItemBtn();
					if( GAMEIN->GetAnimalDialog()->IsActive() )
						GAMEIN->GetAnimalDialog()->SetActive(FALSE);
				}
				break;
			// 죽음..
			case CAnimal::ANIMAL_RESULT_DIE:
				{
					CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg();
					if( pGuageDlg )
					{
						if( (CObject*)pAnimal->GetRenderObj() == OBJECTMGR->GetSelectedObject() )
						{
							pGuageDlg->GetMonsterGuage()->SetActive(FALSE);
							if( OBJECTMGR->GetSelEffectHandle() )
							{
								EFFECTMGR->ForcedEndEffect(OBJECTMGR->GetSelEffectHandle());
							}

							if( OBJECTMGR->GetSelectedObject() )
							{
								OBJECTMGR->GetSelectedObject()->ShowObjectName(FALSE);
								OBJECTMGR->GetSelectedObject()->OnDeselected();
								OBJECTMGR->SetSelectedObjectID(0);
							}

							CLI_SetTargetObj(NULL);
						}
					}

					if( GAMEIN->GetAnimalDialog()->IsActive() )
						GAMEIN->GetAnimalDialog()->SetActive(FALSE);

					// 이펙트
					if( FARM_EFFECT_NUM[FARM_EFFECT_KIND_ANIMAL_DIE] )
					{
						TARGETSET set;
						if( !pAnimal->GetRenderObj() )
							return;
						set.pTarget = pAnimal->GetRenderObj();
						EFFECTMGR->StartEffectProcess(FARM_EFFECT_NUM[FARM_EFFECT_KIND_ANIMAL_DIE], pAnimal->GetRenderObj(), &set, 0, pAnimal->GetRenderObj()->GetID());
					}

					CLI_RemoveRenderFarmAnimalObjToScreen(pAnimal);

					pAnimal->SetAnimal((CAnimal::ANIMAL_RESULT)pPacket->nResult);
					pAnimalCage->GetRenderObjEx(pPacket->nAnimalID+5)->GetEngineObject()->EnablePick();
				}
				break;
			}
		}
		break;
	}
#endif
}

LPCTSTR CSHFarmManager::CLI_GetTaxText( MONEYTYPE taxMoney )
{
#if defined(_MAPSERVER_) || defined(_AGENTSERVER)
	return "";
#else
	// 090707 LUJ, 세금 납부에 쓰이는 아이템이 있을 경우 표시한다. 일단 임시로 텍스트로 표시하자
    LPCTSTR textComma = _T( ", " );
	static TCHAR textMessage[ MAX_PATH ];
	ZeroMemory( textMessage, sizeof( textMessage ) );
	const DWORD textMessageSize = sizeof( textMessage ) / sizeof( *textMessage );
	TCHAR textQuantity[ MAX_PATH ] = { 0 };
	_stprintf(
		textQuantity,
		"%s",
		RESRCMGR->GetMsg( 630 ) );
	TCHAR textGold[ MAX_PATH ] = { 0 };
	_stprintf(
		textGold,
		"%s",
		CHATMGR->GetChatMsg( 1467 ) );

	// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 추가.
	// 모드별로 구분해서 관리비를 구한다.
	WORD wFarmGrade = 0;
	if( ePayTaxMode_FarmNpc == m_ePayTaxMode )
	{
		CSHFarmZone* const farmZone = GetFarmZone(
			WORD(m_stSelFarmObj.nFarmZone));

		if( 0 == farmZone )
		{
			return "";
		}

		CSHFarm* const farm = farmZone->GetFarm( m_stSelFarmObj.nFarmID );

		if( 0 == farm )
		{
			return "";
		}

		wFarmGrade = farm->GetGarden( 0 )->GetGrade();
	}
	else
	{
		CSHFarmManageDlg* pDlg = GAMEIN->GetFarmManageDlg();
		if(pDlg)
		{
			wFarmGrade = pDlg->GetFarmGrade();
		}
	}		

	const Tax& tax = GetTax( wFarmGrade );

	for( DWORD i = 0; i < tax.mItemSize; ++i )
	{
		const Tax::Item& taxItem = tax.mItem[ i ];
		const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo( taxItem.mIndex );

		if( 0 == itemInfo )
		{
			continue;
		}

		TCHAR text[ MAX_PATH ] = { 0 };
		_stprintf(
			text,
			"%s %d%s",
			itemInfo->ItemName,
			taxItem.mQuantity,
			textQuantity );
		_tcscat(
			textMessage,
			textMessageSize > _tcslen( textMessage ) + _tcslen( text ) ? text : "" );

		if( 1 + i == tax.mItemSize )
		{
			continue;
		}

		_tcscat(
		textMessage,
		textMessageSize > _tcslen( textMessage ) + _tcslen( textComma ) ? textComma : "" );
	}

	if( 0 < taxMoney )
	{
		_tcscat( textMessage, 0 < _tcslen( textMessage ) ? textComma : "" );

		TCHAR text[ MAX_PATH ] = { 0 };
		_stprintf(
			text,
			"%s%s",
			AddComma( taxMoney ),
			textGold );
		_tcscat(
			textMessage,
			textMessageSize > _tcslen( textMessage ) + _tcslen( text ) ? text : "" );
	}

	return textMessage;
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SaveAnimalInfoToDB Method																					 DB에 가축 데이터 저장
//
VOID CSHFarmManager::SRV_SaveAnimalInfoToDB(WORD nFarmZone, WORD nFarmID, CAnimal* pAnimal)
{
#if defined(_MAPSERVER_)
	char szText[256];
	sprintf(szText, "EXEC %s %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",	STORED_FARM_SET_ANIMALINFO,
																		nFarmZone,
																		nFarmID, 
																		pAnimal->GetID(),
																		pAnimal->GetOwner(),
																		pAnimal->GetKind(), 
																		pAnimal->GetStep(), 
																		pAnimal->GetLife(), 
																		pAnimal->GetNextStepTime(),
																		pAnimal->GetContentment(),
																		pAnimal->GetInterest()); 
	g_DB.Query(eQueryType_FreeQuery, eFarm_SetAnimalInfo, 0, szText);

	// 점검 후에 농작물이 사라진다는 제보가 있어, 파일로도 데이터를 남긴다.
	// 이 데이터와 서버를 시작할 때 DB에서 로드한 데이터와 비교한다.
	char szFile[256];
	sprintf(szFile, "./Log/FARMANIMAL_VERIFY_%02d%02d%02d.txt", nFarmZone, nFarmID, pAnimal->GetID());
	if (pAnimal->GetLife())
	{
		FILE* fp;
		fp = fopen(szFile, "wt");
		if (fp)
		{
			fprintf(fp, "%d %d %d %d %d\n",
				pAnimal->GetOwner(),
				pAnimal->GetStep(),
				pAnimal->GetLife(),
				pAnimal->GetKind(),
				pAnimal->GetNextStepTime(),
				pAnimal->GetContentment(),
				pAnimal->GetInterest());
			fclose(fp);
		}
	}
	else
	{
		DeleteFile(szFile);
	}
#endif
}
// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SendAnimalInfoFromDBQueue Method																		DB 큐를 참조하여 가축 설정
//
VOID CSHFarmManager::SRV_SetAnimalInfoFromDBQueue(CPlayer* pPlayer, CSHFarmManager::PACKET_FARM_ANIMAL_CAGE_INSTALL_PLANT* pPacket)
{
#if defined(_MAPSERVER_)
	CSHFarmZone* const farmZone = GetFarmZone( pPacket->nFarmZone );

	if( 0 == farmZone )
	{
		return;
	}

	CSHFarm* const pcsFarm = farmZone->GetFarm( pPacket->nFarmID );

	if( 0 == pcsFarm )
	{
		return;
	}

	CPen* const pcsPen = pcsFarm->GetAnimalCage( pPacket->nAnimalCageID );

	if( 0 == pcsPen )
	{
		return;
	}

	CAnimal* const pcsAnimal = pcsPen->GetAnimal( pPacket->nAnimalID );

	if( 0 == pcsAnimal )
	{
		return;
	}

	pcsAnimal->SetOwner(pPacket->nAnimalOwner);
	pcsAnimal->SetStep((CAnimal::ANIMAL_STEP)pPacket->nAnimalStep);
	pcsAnimal->SetLife(pPacket->nAnimalLife);
	pcsAnimal->SetKind(pPacket->nAnimalKind);
	pcsAnimal->SetNextStepTime(pPacket->nAnimalNextStepTime);
	pcsAnimal->SetContentment(pPacket->nAnimalContentment);
	pcsAnimal->SetInterest(pPacket->nAnimalInterest);

	// 가축 로드 상태를 파일로 남긴다.
	SYSTEMTIME st;
	GetLocalTime(&st);

	FILE* fp;
	char szFile[256];

	// 파일로 남기는 마지막 농작물의 데이터와 비교한다.
	BOOL bWrongData = FALSE;
	sprintf(szFile, "./Log/FARMANIMAL_VERIFY_%02d%02d%02d.txt", pPacket->nFarmZone, pPacket->nFarmID, pcsAnimal->GetID());
	fp = fopen(szFile, "rt");
	if (fp)
	{
		int nOwner = 0, nStep = 0, nLife = 0, nKind = 0, nNextStepTime = 0, nAnimalContentment = 0, nAnimalInterest = 0;
		fscanf(fp, "%d %d %d %d %d %d %d %d", 
			&nOwner,
			&nStep,
			&nLife,
			&nKind,
			&nNextStepTime,
			&nAnimalContentment,
			&nAnimalInterest);
		fclose(fp);

		if (pcsAnimal->GetOwner()			!=	(DWORD)nOwner	||
			pcsAnimal->GetStep()			!=	nStep			||
			pcsAnimal->GetLife()			!=	nLife			||
			pcsAnimal->GetKind()			!=	nKind			||
			pcsAnimal->GetNextStepTime()	!=	nNextStepTime	||
			pcsAnimal->GetContentment()		!=	nAnimalContentment ||
			pcsAnimal->GetInterest()		!=	nAnimalInterest )
		{
			bWrongData = TRUE;
		}
	}
	else
	{
		// DB에서는 가져왔는 데 파일로는 안 남았다?
		bWrongData = 2;
	}

	sprintf(szFile, "./Log/FARMCROP_VERIFY_CHECK_%02d%02d%02d.txt", pPacket->nFarmZone, pPacket->nFarmID, pcsAnimal->GetID());
	fp = fopen(szFile, "wt");
	if (fp)
	{
		if (bWrongData == FALSE)		fprintf(fp, "DATA_FROM_FILE and FROM_DB OK!!");
		else if (bWrongData == TRUE)	fprintf(fp, "DATA_FROM_FILE and FROM_DB DATA DIFF!!");
		else if (bWrongData == 2)		fprintf(fp, "FROM_DB Only...??");
		fclose(fp);
	}

	// DB에서 로드한 데이터 기록
	sprintf(szFile, "./Log/FARMCROP_LOAD_START_%4d%02d%02d%02d%02d.txt", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
	fp = fopen(szFile, "a+");
	if (fp)
	{
		fprintf(fp, "ZONE:%d FARM:%d CROP:%d OWNER:%d STEP:%d LIFE:%d SEEDGRD:%d KIND:%d NEXTSTEPTIME:%d CONTENTMENT:%d INTEREST:%d\n",
			pPacket->nFarmZone,
			pcsFarm->GetID(),
			pcsAnimal->GetID(),
			pcsAnimal->GetOwner(),
			pcsAnimal->GetStep(),
			pcsAnimal->GetLife(),
			pcsAnimal->GetKind(),
			pcsAnimal->GetNextStepTime(),
			pcsAnimal->GetContentment(),
			pcsAnimal->GetInterest());
		fclose(fp);
	}
#endif
}


// 080417 KTH 
// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_AddRenderFarmAnimalObjToScreen Method															  화면에 렌더링 가축 오브젝트 추가
//
VOID CSHFarmManager::CLI_AddRenderFarmAnimalObjToScreen(CAnimal* pAnimal)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if( pAnimal->GetStep() < CAnimal::ANIMAL_STEP_1 ) return;

	BASEOBJECT_INFO		binfo;
	BASEMOVE_INFO		minfo;
	CSHFarmRenderObj::stFarmRenderObjInfo stFarmRenderObj;

	int nCnt = 0;
	DWORD n = 0;
	while(m_pstBFarm[nCnt].nID)
	{
		if (m_pstBFarm[nCnt].nKind != RENDER_ANIMAL_KIND)
		{
			nCnt++;
			continue;
		}

		stFarmRenderObj.nKind = m_pstBFarm[nCnt].nKind + WORD(pAnimal->GetKind());
		SafeStrCpy(
			binfo.ObjectName,
			m_pstBFarm[nCnt].szName,
			_countof(binfo.ObjectName));

		stFarmRenderObj.nID = (pAnimal->GetParent()->GetParent()->GetID()+1)*100 + pAnimal->GetID()+1;
		stFarmRenderObj.nMapNum = m_pstBFarm[nCnt].nMapNum;

		binfo.dwObjectID = FARM_RENDER_OBJ_ID_START + n++;
		while(OBJECTMGR->GetObject(binfo.dwObjectID))
		{
			binfo.dwObjectID = FARM_RENDER_OBJ_ID_START + n++;
		}

		binfo.ObjectState = eObjectState_None;
		minfo.bMoving = FALSE;
		minfo.KyungGongIdx = 0;
		minfo.MoveMode = eMoveMode_Run;

		float fDir = DEGTORAD(CSHMath::GetRandomNum(1, 20000));

		minfo.CurPosition.x = pAnimal->GetPos()->x;
		minfo.CurPosition.y = 0;
 		minfo.CurPosition.z = pAnimal->GetPos()->z;
		fDir = pAnimal->GetParent()->GetParent()->GetDir();//pAnimal->GetDir();
		fDir += 1.5f;
		//fDir = pAnimal->GetDir();


		CSHFarmRenderObj* pFarmRenderObj = OBJECTMGR->AddFarmRenderObj(&binfo, &minfo, &stFarmRenderObj);

 		pFarmRenderObj->SetAngle(fDir);
		CLI_SetFarmObjForRender(pFarmRenderObj);
		break;
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_DelAnimal Method																										가축 삭제
//
VOID CSHFarmManager::SRV_DelAnimal(DWORD nOwnerID)
{
#if defined(_MAPSERVER_)
	// 농장맵인가?
	if (GetFarmZoneID(g_pServerSystem->GetMapNum(), NULL) == FALSE) return;

	m_pcsFarmZone.SetPositionHead();
	for( CSHFarmZone* farmZone = 0;
		( farmZone = m_pcsFarmZone.GetData() ) != NULL ; )
	{
 		for( int j = 0; j < farmZone->GetFarmNum(); ++j )
		{
			CSHFarm* const pFarm = farmZone->GetFarm( j );

			for(int k=0; k<pFarm->GetAnimalCageNum(); k++)
			{
				CPen* pAnimalCage = pFarm->GetAnimalCage(k);
				for(int p=0; p<pAnimalCage->GetAnimalNum(); p++)
				{
					CAnimal* pAnimal = pAnimalCage->GetAnimal(p);
					if (pAnimal->GetOwner() == nOwnerID)
					{
						// Log
						InsertLogFarmAnimal( pFarm, pAnimal, eLog_FamilyFarmLostRight );

						pAnimal->AddEvent(NULL, CAnimal::ANIMAL_RESULT_DIE);
						pAnimal->SetAnimal(CAnimal::ANIMAL_RESULT_DIE);
					}
				}
			}
		}
	}							
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RemoveRenderFarmAnimalObjToScreen Method														  화면에 렌더링 가축 오브젝트 제거
//
VOID CSHFarmManager::CLI_RemoveRenderFarmAnimalObjToScreen(CAnimal* pAnimal)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	// 농작물
	OBJECTMGR->AddGarbageObject(pAnimal->GetRenderObj());
	pAnimal->SetRenderObj(NULL);

	CPen* pAnimalCage = (CPen*)pAnimal->GetParent();

	// 080428 KTH -- 보이게 해준다~!~!
	pAnimalCage->GetRenderObjEx(pAnimal->GetID()+5)->GetEngineObject()->Show();
	pAnimalCage->GetRenderObjEx(pAnimal->GetID()+5)->GetEngineObject()->EnablePick();

#endif
}

VOID CSHFarmManager::SRV_FeedAnimal(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nAnimalCageID, WORD nAnimalID, WORD nItemTargetPos, DWORD nItemIdx)
{
#if defined(_MAPSERVER_)
	RESULT eResult = RESULT_OK;
	
	CSHFarmZone* const pFarmZone = GetFarmZone( (WORD)eFarmZone );
	if (pFarmZone == NULL) return;

	CSHFarm* const pFarm = pFarmZone->GetFarm( nFarmID );

	if( 0 == pFarm )
	{
		return;
	}

	CPen* pAnimalCage = pFarm->GetAnimalCage(nAnimalCageID);

	if( 0 == pAnimalCage )
	{
		return;
	}

	// 아이템 체크
	ITEM_INFO * pItemInfo = ITEMMGR->GetUseItemInfo(pPlayer, nItemTargetPos, nItemIdx);
	if (pItemInfo == NULL || pItemInfo->SupplyType != ITEM_KIND_ANIMAL_FOOD)
	{
		return;
	}

	const int nFoodKind = pItemInfo->SupplyValue;

	// 사료 종류 범위 체크
	if (nFoodKind > CAnimal::ANIMAL_FEED_KIND_MAX) 
	{
		return;
	}

	// 씨앗 등급 범위 체크
/*	if (nManureGrade > CSHCrop::CROP_SEED_GRADE_MAX)
	{
		return;
	}*/

	CAnimal* pAnimal = pAnimalCage->GetAnimal(nAnimalID);

	if( 0 == pAnimal )
	{
		return;
	}
	else if( pAnimal->GetStep() < CAnimal::ANIMAL_STEP_1 )
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
	}

	if( pAnimal->GetOwner() != pPlayer->GetID() )
	{
		eResult = RESULT_FAIL_02;
		goto FAILED;
	}

	if( pAnimal->GetStep() == CAnimal::ANIMAL_STEP_COMPLETE )
	{
		eResult = RESULT_FAIL_03;
		goto FAILED;
	}

	if( pAnimal->GetContentment() == 100)
	{
		eResult = RESULT_FAIL_03;
		goto FAILED;
	}

	if( pPlayer->GetFarmInfo()->nAnimalFeedRetryTime )
	{
		eResult = RESULT_FAIL_04;
		goto FAILED;
	}

	// 거리체크..
	VECTOR3 stPlayerPos;
	pPlayer->GetPosition(&stPlayerPos);
	float nDistance = CalcDistanceXZ(pAnimal->GetPos(), &stPlayerPos); 
	if (nDistance > PEN_OBJECT_CONTROL_DISTANCE)
	{
		eResult = RESULT_FAIL_05;
		goto FAILED;
	}

	// 아이템 제거
	if (ITEMMGR->UseItem(pPlayer, nItemTargetPos, nItemIdx) == eItemUseSuccess)
	{
		MSG_ITEM_USE_ACK msg;
		msg.Category	= MP_ITEM;
		msg.Protocol	= MP_ITEM_USE_ACK;
		msg.dwObjectID	= pPlayer->GetID();
		msg.TargetPos	= nItemTargetPos;
		msg.dwItemIdx	= nItemIdx;
		msg.eResult		= eItemUseSuccess;

		pPlayer->SendMsg(&msg, sizeof(msg));
	}
	else
	{
		return;
	}

	pAnimal->Feed(nFoodKind);

	SRV_SaveAnimalInfoToDB((WORD)eFarmZone, nFarmID, pAnimal);

	pPlayer->SetFarmAnimalFeedRetryTime(CAnimal::ANIMAL_FEED_DELAY_TIME);
	Farm_SetTimeDelay(pPlayer->GetID(), FARM_TIMEDELAY_KIND_FEED, CAnimal::ANIMAL_FEED_DELAY_TIME);

	InsertLogFarmAnimal( pFarm, pAnimal, eLog_FamilyFarmFeedAnimal );

FAILED:
	// 클라이언트에 알리기
	PACKET_FARM_CAGE_ANIMAL_FEED stPacket;

	stPacket.Category			= MP_FARM;
	stPacket.Protocol			= MP_FARM_ANIMAL_FEED;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.nFarmZone			= (WORD)eFarmZone;
	stPacket.nFarmID			= nFarmID;
	stPacket.nAnimalCageID		= nAnimalCageID;
	stPacket.nAnimalID			= pAnimal->GetID();
	stPacket.nAnimalLife		= pAnimal->GetContentment();
	stPacket.nAnimalContentment	= pAnimal->GetContentment();
	stPacket.nAnimalInterest	= pAnimal->GetInterest();
	stPacket.nFoodKind			= (WORD)nFoodKind;
	stPacket.nResult			= (WORD)eResult;

	if (eResult == RESULT_OK)
	{
		SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));
		// 농장UI 정보 전송
		//SRV_SendFarmUIInfoToClient(eFarmZone, nFarmID, nGardenID, pCrop, pFarm->GetOwner());
	}
	else
	{
		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
	}
#endif
}

VOID CSHFarmManager::CLI_RequestFeedAnimalInCage(WORD nItemTargetPos, DWORD nItemIdx)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if (m_pcsSelFarmObj)
	{
		if (GetRenderObjKind(m_pcsSelFarmObj->GetInfo()->nKind) == RENDER_ANIMAL_KIND)
		{
			PACKET_FARM_CAGE_ANIMAL_FEED stPacket;

			stPacket.Category 		= MP_FARM;
			stPacket.Protocol 		= MP_FARM_ANIMAL_FEED;
			stPacket.dwObjectID		= gHeroID;
			stPacket.nFarmZone		= WORD(m_stSelFarmObj.nFarmZone);
			stPacket.nFarmID		= WORD(m_stSelFarmObj.nFarmID);
			stPacket.nAnimalCageID	= WORD(m_stSelFarmObj.nAnimalCageID);
			stPacket.nAnimalID		= WORD(m_stSelFarmObj.nAnimalID);
			stPacket.nItemTargetPos	= nItemTargetPos;
			stPacket.nItemIdx		= nItemIdx;

			NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
		}
	}
#endif
}

CAnimal* CSHFarmManager::CLI_GetAnimalInfo(CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nAnimalCageID, WORD nAnimalID)
{
	CAnimal* pAnimal = NULL;
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	CSHFarmZone* const pFarmZone = GetFarmZone(
		WORD(eFarmZone));

	if (pFarmZone == NULL) return NULL;

	CSHFarm* const pFarm = pFarmZone->GetFarm(nFarmID);

	if( 0 == pFarm )
	{
		return NULL;
	}

	CPen* pAnimalCage = pFarm->GetAnimalCage(nAnimalCageID);

	if( 0 == pAnimalCage )
	{
		return NULL;
	}

	 pAnimal = pAnimalCage->GetAnimal(nAnimalID);

#endif
	return pAnimal;
}

VOID CSHFarmManager::SRV_CleaningAnimal(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nAnimalCageID, WORD nAnimalID, WORD nItemTargetPos, DWORD nItemIdx)
{
#if defined(_MAPSERVER_)
	RESULT eResult = RESULT_OK;
	
	CSHFarmZone* const pFarmZone = GetFarmZone( (WORD)eFarmZone );
	if (pFarmZone == NULL) return;

	CSHFarm* const pFarm = pFarmZone->GetFarm(nFarmID);

	if( 0 == pFarm )
	{
		return;
	}

	CPen* pAnimalCage = pFarm->GetAnimalCage(nAnimalCageID);

	if( 0 == pAnimalCage )
	{
		return;
	}

	// 아이템 체크
	ITEM_INFO * pItemInfo = ITEMMGR->GetUseItemInfo(pPlayer, nItemTargetPos, nItemIdx);
	if (pItemInfo == NULL || pItemInfo->SupplyType != ITEM_KIND_ANIMAL_CLEANER)
	{
		return;
	}

	const int nCleaningKind = pItemInfo->SupplyValue;

	// 클리러 종류 범위 체크
	if (nCleaningKind > CAnimal::ANIMAL_CLEANER_KIND_MAX) 
	{
		return;
	}

	CAnimal* pAnimal = pAnimalCage->GetAnimal(nAnimalID);

	if( 0 == pAnimal )
	{
		return;
	}
	else if( pAnimal->GetStep() < CAnimal::ANIMAL_STEP_1 )
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
	}

	if( pAnimal->GetOwner() != pPlayer->GetID() )
	{
		eResult = RESULT_FAIL_02;
		goto FAILED;
	}

	if( pAnimal->GetStep() == CAnimal::ANIMAL_STEP_COMPLETE )
	{
		eResult = RESULT_FAIL_03;
		goto FAILED;
	}

	if( pPlayer->GetFarmInfo()->nAnimalCleanRetryTime )
	{
		eResult = RESULT_FAIL_04;
		goto FAILED;
	}

	// 거리체크..
	VECTOR3 stPlayerPos;
	pPlayer->GetPosition(&stPlayerPos);
	float nDistance = CalcDistanceXZ(pAnimal->GetPos(), &stPlayerPos); 
	if (nDistance > PEN_OBJECT_CONTROL_DISTANCE)
	{
		eResult = RESULT_FAIL_05;
		goto FAILED;
	}

	// 아이템 제거
	if (ITEMMGR->UseItem(pPlayer, nItemTargetPos, nItemIdx) == eItemUseSuccess)
	{
		MSG_ITEM_USE_ACK msg;
		msg.Category	= MP_ITEM;
		msg.Protocol	= MP_ITEM_USE_ACK;
		msg.dwObjectID	= pPlayer->GetID();
		msg.TargetPos	= nItemTargetPos;
		msg.dwItemIdx	= nItemIdx;
		msg.eResult		= eItemUseSuccess;

		pPlayer->SendMsg(&msg, sizeof(msg));
	}
	else
	{
		return;
	}

	pAnimal->Cleaning(nCleaningKind);

	SRV_SaveAnimalInfoToDB((WORD)eFarmZone, nFarmID, pAnimal);

	pPlayer->SetFarmAnimalCleanRetryTime(CAnimal::ANIMAL_CLEANING_RETRY_TIME);
	Farm_SetTimeDelay(pPlayer->GetID(), FARM_TIMEDELAY_KIND_CLEAN, CAnimal::ANIMAL_CLEANING_RETRY_TIME);

	InsertLogFarmAnimal( pFarm, pAnimal, eLog_FamilyFarmCleanAnimal );

FAILED:
	// 클라이언트에 알리기
	PACKET_FARM_CAGE_ANIMAL_FEED stPacket;

	stPacket.Category			= MP_FARM;
	stPacket.Protocol			= MP_FARM_ANIMAL_CLEANING;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.nFarmZone			= (WORD)eFarmZone;
	stPacket.nFarmID			= nFarmID;
	stPacket.nAnimalCageID		= nAnimalCageID;
	stPacket.nAnimalID			= pAnimal->GetID();
	stPacket.nAnimalLife		= pAnimal->GetContentment();
	stPacket.nAnimalContentment	= pAnimal->GetContentment();
	stPacket.nAnimalInterest	= pAnimal->GetInterest();
	stPacket.nFoodKind			= (WORD)nCleaningKind;
	stPacket.nResult			= (WORD)eResult;

	if (eResult == RESULT_OK)
	{
		SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));
		// 농장UI 정보 전송
		//SRV_SendFarmUIInfoToClient(eFarmZone, nFarmID, nGardenID, pCrop, pFarm->GetOwner());
	}
	else
	{
		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
	}
#endif
}

VOID CSHFarmManager::CLI_RequestCleaningAnimalInCage(WORD nItemTargetPos, DWORD nItemIdx)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if (m_pcsSelFarmObj)
	{
		if (GetRenderObjKind(m_pcsSelFarmObj->GetInfo()->nKind) == RENDER_ANIMAL_KIND)
		{
			PACKET_FARM_CAGE_ANIMAL_FEED stPacket;

			stPacket.Category 			= MP_FARM;
			stPacket.Protocol 			= MP_FARM_ANIMAL_CLEANING;
			stPacket.dwObjectID			= gHeroID;
			stPacket.nFarmZone			= WORD(m_stSelFarmObj.nFarmZone);
			stPacket.nFarmID			= WORD(m_stSelFarmObj.nFarmID);
			stPacket.nAnimalCageID		= WORD(m_stSelFarmObj.nAnimalCageID);
			stPacket.nAnimalID			= WORD(m_stSelFarmObj.nAnimalID);
			stPacket.nItemTargetPos		= nItemTargetPos;
			stPacket.nItemIdx			= nItemIdx;

			NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
		}
	}
#endif
}

VOID CSHFarmManager::CLI_RequestAnimalGetItem(WORD nType)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	if( m_pcsSelFarmObj )
	{
		if( GetRenderObjKind(m_pcsSelFarmObj->GetInfo()->nKind) == RENDER_ANIMAL_KIND )
		{
			PACKET_FARM_CAGE_ANIMAL_REWARD stPacket;

			stPacket.Category		= MP_FARM;
			stPacket.Protocol		= MP_FARM_ANIMAL_REWARD;
			stPacket.dwObjectID		= gHeroID;
			stPacket.nFarmZone		= WORD(m_stSelFarmObj.nFarmZone);
			stPacket.nFarmID		= WORD(m_stSelFarmObj.nFarmID);
			stPacket.nAnimalCageID	= WORD(m_stSelFarmObj.nAnimalCageID);
			stPacket.nAnimalID		= WORD(m_stSelFarmObj.nAnimalID);
			stPacket.nResult		= nType;

			NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
		}
	}
#endif
}

VOID CSHFarmManager::SRV_AnimalGetItem(CPlayer* pPlayer, CSHFarmZone::FARM_ZONE eFarmZone, WORD nFarmID, WORD nAnimalCageID, WORD nAnimalID, WORD nType)
{
#if defined(_MAPSERVER_)
	RESULT eResult = RESULT_OK;
	CSHFarmZone* const pFarmZone = GetFarmZone( (WORD)eFarmZone );
	if (pFarmZone == NULL) return;

	CSHFarm* const pFarm = pFarmZone->GetFarm( nFarmID );

	if( 0 == pFarm )
	{
		return;
	}

	CPen* pAnimalCage = pFarm->GetAnimalCage(nAnimalCageID);

	if( 0 == pAnimalCage )
	{
		return;
	}

	CAnimal* pAnimal = pAnimalCage->GetAnimal(nAnimalID);

	if( 0 == pAnimal )
	{
		return;
	}
	else if( pAnimal->GetOwner() != pPlayer->GetID() )
	{
		eResult = RESULT_FAIL_01;
		goto FAILED;
	}

	if(pAnimal->GetLife() > 0)
	{
		eResult = RESULT_FAIL_02;
		goto FAILED;
	}

	if( pAnimal->GetStep() != CAnimal::ANIMAL_STEP_COMPLETE )
	{
		pAnimal->SetStep(CAnimal::ANIMAL_STEP_COMPLETE);
	}

	// 거리체크..
	VECTOR3 stPlayerPos;
	pPlayer->GetPosition(&stPlayerPos);
	float nDistance = CalcDistanceXZ(pAnimal->GetPos(), &stPlayerPos); 
	if (nDistance > PEN_OBJECT_CONTROL_DISTANCE)
	{
		eResult = RESULT_FAIL_03;
		goto FAILED;
	}

	// 아이템 공간 체크
	CItemSlot* pSlot = pPlayer->GetSlot(eItemTable_Inventory);
	if(NULL == pSlot) ASSERTMSG(0,"올바른 아이템 슬롯을 얻을 수 없습니다.");

	// 071211 KTH -- pPlayer 추가
	DWORD nNum = ITEMMGR->GetTotalEmptySlotNum(pSlot, pPlayer);
	if(nNum == 0)
	{
		eResult = RESULT_FAIL_04;
		goto FAILED;
	}

	// 처음 수확 요청 시
	if (nType == RESULT_OK)
	{
		goto FAILED;
	}
	// 모션 종료 후 실제 수확 요청 시
	else
	{
		eResult = RESULT_OK_02;
	}

	DWORD nPercent = CSHMath::GetRandomNum(1, 1000);
	DWORD nResult = 0;
	DWORD nRow = 0;

	if( pAnimal->GetInterest() <= FARM_ANIMAL_REWARD_BAD )
	{
		nRow = CAnimal::ANIMAL_BREED_KIND_BAD;
	}
	else if( pAnimal->GetInterest() < FARM_ANIMAL_REWARD_NORMAL )
	{
		nRow = CAnimal::ANIMAL_BREED_KIND_NORMAL;
	}
	else
	{
		nRow = CAnimal::ANIMAL_BREED_KIND_GOOD;
	}

	if( m_pstAnimalReward[pAnimal->GetKind()].nPercent[nRow*3]*10 > nPercent )
	{
		nResult = (3*nRow)+CAnimal::ANIMAL_BREED_KIND_BAD;
	}
	else if( m_pstAnimalReward[pAnimal->GetKind()].nPercent[nRow*3]*10 + m_pstAnimalReward[pAnimal->GetKind()].nPercent[nRow*3+1]*10 > nPercent )
	{
		nResult = (3*nRow)+CAnimal::ANIMAL_BREED_KIND_NORMAL;
	}
	else
	{
		nResult = (3*nRow)+CAnimal::ANIMAL_BREED_KIND_GOOD;
	}

	DWORD nItemID = m_pstAnimalReward[pAnimal->GetKind()].nItemID[nResult];
	DWORD nItemNum = m_pstAnimalReward[pAnimal->GetKind()].nItemNum[nResult];

	// ..유효 아이템인가?
	// ..스크립트 데이터 파일이나 기타등등의 오류로 유효하지 않은 아이템이라면 처리하지 않는다.
	ITEM_INFO* pstItem = ITEMMGR->GetItemInfo(nItemID);
	if (pstItem == NULL)
	{
		return;
	}

	InsertLogFarmAnimal( pFarm, pAnimal, eLog_FamilyFarmAnimalReward );

	// 수확 했으니 제거
	pAnimal->SetAnimal(CAnimal::ANIMAL_RESULT_DIE);

	// DB에 저장
	SRV_SaveAnimalInfoToDB((WORD)eFarmZone, nFarmID, pAnimal);
	ITEMMGR->ObtainMonstermeterItem(pPlayer, nItemID, WORD( nItemNum ) );
	
FAILED:
	// 클라이언트에 알리기
	PACKET_FARM_CAGE_ANIMAL_REWARD stPacket;

	stPacket.Category			= MP_FARM;
	stPacket.Protocol			= MP_FARM_ANIMAL_REWARD;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.nFarmZone			= (WORD)eFarmZone;
	stPacket.nFarmID			= nFarmID;
	stPacket.nAnimalCageID		= nAnimalCageID;
	stPacket.nAnimalID			= nAnimalID;
	stPacket.nResult			= (WORD)eResult;

	if (eResult == RESULT_OK ||
		eResult == RESULT_OK_02)
	{
		SRV_SendFarmInfoToNearPlayer(pFarm, &stPacket, sizeof(stPacket));
		// 농장UI 정보 전송
		//SRV_SendFarmUIInfoToClient(eFarmZone, nFarmID, nGardenID, pCrop, pFarm->GetOwner());
	}
	else
	{
		pPlayer->SendMsg(&stPacket, sizeof(stPacket));
	}
#endif
}

// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 추가.
// 기존의 농장맵 NPC를 통한 납부와는 처리형태가 다르므로 구분해서 따로 처리한다.
VOID CSHFarmManager::SRV_PayTaxFromFamilyDialog(CPlayer* pPlayer, const PACKET_FARM_TAX_SYN& packet )
{
#if defined(_MAPSERVER_)
	const CSHFarmZone::FARM_ZONE eFarmZone =  CSHFarmZone::FARM_ZONE( packet.dwData1 );
	const WORD nFarmID = WORD( packet.dwData2 );
	const int nKind = packet.dwData4;
	// 090707 LUJ, 입력으로 받을 수 있는 아이템 최대 입력 개수
	const int maxInputItemSize = sizeof( packet.mItem ) / sizeof( *packet.mItem );
	ITEMBASE foundTaxItemArray[ maxInputItemSize ];
	ZeroMemory( foundTaxItemArray, sizeof( foundTaxItemArray ) );
	DWORD foundTaxItemArraySize = 0;
	MONEYTYPE requestedTaxMoney = 0;
	RESULT eResult = RESULT_OK;
	
	if(!pPlayer) 
	{
		return;
	}

	if( 2 == nKind )
	{
		// 클라이언트로부터 보내온 농장아이디를 이용하여 DB로부터 해당 농장정보를 읽어들인후,
		// 납부 금액을 클라이언트에게 전송한다.
		Farm_GetFarmInfo( pPlayer->GetID(), eFarmZone, nFarmID );
		return;
	}

	const Tax& tax = GetTax( m_stFarmTaxState.nGardenGrade );
	requestedTaxMoney = tax.mMoney + tax.mMoney * m_stFarmTaxState.nTaxArrearageFreq + 
										tax.mMoney * m_stFarmTaxState.nTaxArrearageFreq * FARM_TAX_PAY_ARREARAGE_ADD_RATE / 100;

	// 기존의 세금으로 납부할 아이템검사를 함수로 처리.
	eResult = SRV_PayItem( pPlayer, packet, tax, requestedTaxMoney );
	if( RESULT_OK != eResult )
	{
		if(RESULT_FAIL_INVALID == eResult )
		{
			return;
		}
		else
		{
			goto FAILED;
		}
	}

	// DB에 저장
	Farm_SetTaxInfo(eFarmZone, nFarmID, m_stFarmTaxState.nTaxArrearageFreq, pPlayer->GetID());

	{
		// 농장맵으로 변경된 납부설정을 전달한다.
		MSG_DWORD3_NAME msg;
		ZeroMemory( &msg, sizeof(MSG_DWORD3_NAME) );
		msg.Category = MP_FARM;
		msg.Protocol = MP_FARM_SETFARM_TAXINFO_TO_FARMMAP;
		msg.dwObjectID = pPlayer->GetID();
		msg.dwData1  = eFarmZone;
		msg.dwData2  = nFarmID;
		msg.dwData3	 = m_stFarmTaxState.nTaxArrearageFreq;
		SafeStrCpy(msg.Name, pPlayer->GetObjectName(), MAX_NAME_LENGTH+1);
		g_Network.Send2AgentServer( (char*)&msg, sizeof(MSG_DWORD3_NAME) );	
	}

	// 081205 LUJ, 로그
	LogItemMoney(
		pPlayer->GetID(),
		pPlayer->GetObjectName(),
		0,
		"*farmTax",
		eLog_FarmTaxPaid,
		pPlayer->GetMoney(),
		0,
		requestedTaxMoney,
		0,
		0,
		0,
		0,
		0,
		0 );
	// 로그
	InsertLogFarm(eFarmZone, nFarmID, FARM_LOG_KIND_PAY_TAX, pPlayer->GetID());

FAILED:
	PACKET_FARM_TAX_ACK stPacket;
	ZeroMemory( &stPacket, sizeof( stPacket ) );
	stPacket.Category = MP_FARM;
	stPacket.Protocol = MP_FARM_PAY_TAX;
	stPacket.dwData1 = eResult;
	stPacket.dwData2 = eFarmZone;
	stPacket.dwData3 = nFarmID;
	stPacket.mTaxMoney = requestedTaxMoney;
	SafeStrCpy( stPacket.Name, m_stFarmTaxState.szTaxPayPlayerName, MAX_NAME_LENGTH + 1 );
	const DWORD itemSize = sizeof( stPacket.mItem ) / sizeof( *stPacket.mItem );
	stPacket.mSize = min( itemSize, foundTaxItemArraySize );

	for( DWORD i = 0; i < stPacket.mSize; ++i )
	{
		stPacket.mItem[ i ] = foundTaxItemArray[ i ];
	}

	pPlayer->SendMsg( &stPacket, stPacket.GetSize() );
#endif
}

// 091204 ONS 관리비 납부 상태를 체크한후, 클라이언트에게 납부금액을 알려준다.
VOID CSHFarmManager::SRV_SendTaxToClient(CPlayer* pPlayer, stFARMTAXSTATE* stFarmTaxState )
{
#if defined(_MAPSERVER_)
	if( !pPlayer || !stFarmTaxState )
	{
		return;
	}

	// 관리비 납부 상태를 체크한다.
	if(strlen(stFarmTaxState->szTaxPayPlayerName) > 0)
	{
		PACKET_FARM_TAX_ACK stPacket;
		ZeroMemory( &stPacket, sizeof( stPacket ) );

		stPacket.Category = MP_FARM;
		stPacket.Protocol = MP_FARM_PAY_TAX;
		stPacket.dwData1 = RESULT_FAIL_02;
		stPacket.dwData2 = stFarmTaxState->nFarmZone;
		stPacket.dwData3 = stFarmTaxState->nFarmID;
		stPacket.mTaxMoney = 0;
		SafeStrCpy( stPacket.Name, stFarmTaxState->szTaxPayPlayerName, MAX_NAME_LENGTH + 1 );
		pPlayer->SendMsg( &stPacket, stPacket.GetSize() );
		return;
	}


	// DB로부터 읽어들인 농장정보를 저장한다.
	if(stFarmTaxState)
	{
		m_stFarmTaxState.nFarmZone			= stFarmTaxState->nFarmZone;
		m_stFarmTaxState.nFarmID			= stFarmTaxState->nFarmID;
		m_stFarmTaxState.nFarmOwner			= stFarmTaxState->nFarmOwner;
		m_stFarmTaxState.nFarmState			= stFarmTaxState->nFarmState;
		m_stFarmTaxState.nFenceGrade		= stFarmTaxState->nFenceGrade;
		m_stFarmTaxState.nGardenGrade		= stFarmTaxState->nGardenGrade;
		m_stFarmTaxState.nHouseGrade		= stFarmTaxState->nHouseGrade;
		m_stFarmTaxState.nTaxArrearageFreq	= stFarmTaxState->nTaxArrearageFreq;
		m_stFarmTaxState.nWarehouseGrade	= stFarmTaxState->nWarehouseGrade;
		SafeStrCpy(m_stFarmTaxState.szTaxPayPlayerName, stFarmTaxState->szTaxPayPlayerName, MAX_NAME_LENGTH + 1);
	}


	const Tax& tax = GetTax( m_stFarmTaxState.nGardenGrade );
	const MONEYTYPE requestedTaxMoney = tax.mMoney + tax.mMoney * m_stFarmTaxState.nTaxArrearageFreq + 
										tax.mMoney * m_stFarmTaxState.nTaxArrearageFreq * FARM_TAX_PAY_ARREARAGE_ADD_RATE / 100;

	// 먼저 클라이언트에 관리비 금액을 알리고 납부할 것인지 묻는다.
	PACKET_FARM_TAX_ACK stPacket;
	ZeroMemory( &stPacket, sizeof( stPacket ) );
	stPacket.Category = MP_FARM;
	stPacket.Protocol = MP_FARM_PAY_TAX;
	stPacket.dwData1 = RESULT_OK_02;
	stPacket.dwData2 = requestedTaxMoney;
	pPlayer->SendMsg(&stPacket, stPacket.GetSize() );
#endif
}

// 091204 ONS 기존의 아이템 납부 처리를 함수로 변경한다.
RESULT CSHFarmManager::SRV_PayItem( CPlayer* pPlayer, const PACKET_FARM_TAX_SYN& packet, const Tax& tax, MONEYTYPE requestedTaxMoney )
{
#if defined(_MAPSERVER_)
	// 090707 LUJ, 입력으로 받을 수 있는 아이템 최대 입력 개수
	const int maxInputItemSize = sizeof( packet.mItem ) / sizeof( *packet.mItem );
	ITEMBASE foundTaxItemArray[ maxInputItemSize ];
	ZeroMemory( foundTaxItemArray, sizeof( foundTaxItemArray ) );
	DWORD foundTaxItemArraySize = 0;
	CItemSlot* const itemSlot = pPlayer->GetSlot( eItemTable_Inventory );
	if( 0 == itemSlot )
	{
		return RESULT_FAIL_INVALID;
	}

	// 090710 LUJ, 클라이언트에서 전송된 정보를 이용해서, 세금으로 낼 아이템이 충분한지 검사한다
	for( DWORD taxItemArrayIndex = 0; taxItemArrayIndex < tax.mItemSize; ++taxItemArrayIndex )
	{
        const Tax::Item& taxItem = tax.mItem[ taxItemArrayIndex ];
		DURTYPE paidItemQuantity = 0;
		const DWORD itemArraySize = min( maxInputItemSize, packet.mSize );

		for( DWORD itemArrayIndex = 0; itemArraySize > itemArrayIndex; ++itemArrayIndex )
		{
			const ICONBASE& iconBase = packet.mItem[ itemArrayIndex ];
			const ITEMBASE* const itemBase = itemSlot->GetItemInfoAbs( iconBase.Position );

			if( 0 == itemBase )
			{
				continue;
			}
			else if( iconBase.dwDBIdx != itemBase->dwDBIdx )
			{
				continue;
			}
			else if( taxItem.mIndex != itemBase->wIconIdx )
			{
				continue;
			}
			else if( taxItem.mQuantity <= paidItemQuantity )
			{
				break;
			}

			// 090707 LUJ, 납부할 아이템 정보를 저장한다. 아이템을 납부한 후 남은 수량을 설정한다
			ITEMBASE& foundTaxItem = foundTaxItemArray[ foundTaxItemArraySize++ ];
			foundTaxItem = *itemBase;

			if( ITEMMGR->IsDupItem( itemBase->wIconIdx ) )
			{
				const DURTYPE requestedItemQuantity = min( foundTaxItem.Durability, taxItem.mQuantity - paidItemQuantity );

				foundTaxItem.Durability -= requestedItemQuantity;
				paidItemQuantity += requestedItemQuantity;
			}
			else
			{
				foundTaxItem.Durability = 0;
				++paidItemQuantity;
			}
		}

		// 090710 LUJ, 납부 아이템 검사가 실패하면 차라리 처리하지 않는 편이 낫다
		if( taxItem.mQuantity != paidItemQuantity )
		{
			return RESULT_FAIL_03;
		}
	}

	// 090710 LUJ, 돈을 차감한다. 검사는 해당 함수에 포함되어 있다. 아이템 검사 후 처리한다
	if( requestedTaxMoney != pPlayer->SetMoney( requestedTaxMoney, MONEY_SUBTRACTION, MF_NOMAL, eItemTable_Inventory, eMoneyLog_LoseFarmTax ) )
	{
		return RESULT_FAIL_03;
	}

	// 090710 LUJ, 아이템을 차감한다
	for( int i = foundTaxItemArraySize;
		0 < i--; )
	{
		const ITEMBASE& foundTaxItem = foundTaxItemArray[ i ];

		// 090707 LUJ, 아이템을 소모 처리한다
		if( 0 == foundTaxItem.Durability )
		{
			const ITEMBASE* const itemBase = itemSlot->GetItemInfoAbs( foundTaxItem.Position );

			if( 0 == itemBase )
			{
				return RESULT_FAIL_INVALID;
			}

			const ITEMBASE deletedItemBase = *itemBase;

			if( EI_TRUE != itemSlot->DeleteItemAbs(
				pPlayer,
				foundTaxItem.Position,
				0 ) )
			{
				return RESULT_FAIL_03;
			}

			ItemDeleteToDB( deletedItemBase.dwDBIdx );
			LogItemMoney(
				pPlayer->GetID(),
				pPlayer->GetObjectName(),
				0,
				"*farmTax",
				eLog_FarmTaxPaid,
				pPlayer->GetMoney(),
				0,
				0,
				deletedItemBase.wIconIdx,
				deletedItemBase.dwDBIdx,
				deletedItemBase.Position,
				0,
				deletedItemBase.Durability,
				0 );
		}
		else
		{
			if( EI_TRUE != itemSlot->UpdateItemAbs(
				pPlayer,
				foundTaxItem.Position,
				foundTaxItem.dwDBIdx,
				foundTaxItem.wIconIdx,
				foundTaxItem.Position,
				foundTaxItem.QuickPosition,
				foundTaxItem.Durability ) )
			{
				return RESULT_FAIL_03;			
			}

			ItemUpdateToDB(
				pPlayer->GetID(),
				foundTaxItem.dwDBIdx,
				foundTaxItem.wIconIdx,
				foundTaxItem.Durability,
				foundTaxItem.Position,
				foundTaxItem.QuickPosition );
			LogItemMoney(
				pPlayer->GetID(),
				pPlayer->GetObjectName(),
				0,
				"*farmTax",
				eLog_FarmTaxPaid,
				pPlayer->GetMoney(),
				0,
				0,
				foundTaxItem.wIconIdx,
				foundTaxItem.dwDBIdx,
				foundTaxItem.Position,
				0,
				foundTaxItem.Durability,
				0 );
		}
	}

	// 081205 LUJ, 로그
	LogItemMoney(
		pPlayer->GetID(),
		pPlayer->GetObjectName(),
		0,
		"*farmTax",
		eLog_FarmTaxPaid,
		pPlayer->GetMoney(),
		0,
		requestedTaxMoney,
		0,
		0,
		0,
		0,
		0,
		0 );

	// 로그
	InsertLogFarm(m_stFarmTaxState.nFarmZone, m_stFarmTaxState.nFarmID, FARM_LOG_KIND_PAY_TAX, pPlayer->GetID());
#endif
	return RESULT_OK;
}

// 091204 ONS 농장 관리비납부 정보설정
VOID CSHFarmManager::SRV_SetFarmTaxInfo(DWORD dwFarmZone, DWORD dwFarmID, DWORD dwTaxArrearageFreq, char* szName)
{
	CSHFarmZone* const pFarmZone = GetFarmZone( (WORD)dwFarmZone );
	if (pFarmZone == NULL) return;

	CSHFarm* const pFarm = pFarmZone->GetFarm(dwFarmID);
	if( 0 == pFarm )
	{
		return;
	}

	pFarm->SetTaxArrearageFreq(dwTaxArrearageFreq);
	pFarm->SetTaxPayPlayerName(szName);
}

// 091204 ONS 농장 소유주 변경 처리
VOID CSHFarmManager::SRV_SetFarmChangeOwner( DWORD dwFarmZone, DWORD dwFarmID, DWORD dwOwnerID )
{
	CSHFarmZone* const pFarmZone = GetFarmZone( (WORD)dwFarmZone );
	if (pFarmZone == NULL) return;

	CSHFarm* const pFarm = pFarmZone->GetFarm(dwFarmID);
	if( 0 == pFarm )
	{
		return;
	}

	pFarm->SetOwner( dwOwnerID );
}

VOID CSHFarmManager::SRV_ProcessFarmBuyWaitList()
{
#if defined(_MAPSERVER_)
	static DWORD LastCheckTime = gCurTime ; 

	if( gCurTime - LastCheckTime >= 300000 )	// 5분에 한번씩 시간채크 하여 삭제해주자 

	m_BuyFarm_WaitInfoList.SetPositionHead() ;

	stBuyFarmWaitInfo* pWaitInfo = NULL ;

	while( ( pWaitInfo = m_BuyFarm_WaitInfoList.GetData() ) != NULL )
	{
		if( pWaitInfo->bQueryToDB == TRUE )
		{
			// DB요청중이면 삭제하지말자
			continue;
		}

		if( gCurTime - pWaitInfo->dwRequestTime > 60000 )
		{
			m_BuyFarm_WaitInfoList.Remove( pWaitInfo->dwPlayerIdx ) ;
			delete pWaitInfo;
		}
	}

	LastCheckTime = gCurTime ;
#endif
}
VOID CSHFarmManager::SRV_OnCheckHaveFarmNumToDB(DWORD dwPlayerIdx , DWORD ResultNum )
{
#if defined(_MAPSERVER_)
	stBuyFarmWaitInfo* pWaitInfo = m_BuyFarm_WaitInfoList.GetData( dwPlayerIdx ) ; 
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( dwPlayerIdx );

	if( ! pWaitInfo )
	{
		return;
	}

	if( pPlayer )
	{
		if( FARM_BUY_MAX_NUM <= ResultNum )
		{
			PACKET_FARM_STATE			stPacket;
			ZeroMemory( &stPacket , sizeof( PACKET_FARM_STATE ) );

			stPacket.Category			= MP_FARM;
			stPacket.Protocol			= MP_FARM_BUY;
			stPacket.dwObjectID 		= pPlayer->GetID();
			stPacket.nFarmZone			= pWaitInfo->nFarmZone;
			stPacket.nFarmID			= pWaitInfo->nFarmID;
			stPacket.nResult			= CSHMain::RESULT_FAIL_01;

			pPlayer->SendMsg(&stPacket, sizeof(stPacket));
		}
		else
		{
			SRV_BuyFarm(pPlayer, (CSHFarmZone::FARM_ZONE)pWaitInfo->nFarmZone, pWaitInfo->nFarmID , 1 );
		}
	}
#endif
}
