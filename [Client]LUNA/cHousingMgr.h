#pragma once
#include "stdafx.h"
#include "../[cc]Header/CommonGameDefine.h"
#include "../[cc]Header/CommonStruct.h"
#include "input/Mouse.h"
#include "MovePoint.h"
#include "Object.h"

class CItem;
class CFurniture;
class cHousingStoredIcon;

#define HOUSINGMGR USINGTON(cHousingMgr)
#define HOUSINGMAP	99

#define HOUSE_DUMMYID 999
#define HOUSE_ATTACHBONENAME "ride"

class cHousingMgr
{
private:
	enum eHouseLoadState				//하우스 로드상태 
	{
		eHsLoad_NONE = 0 ,				//디폴트 상태 
		eHSLoad_NOWLOADING	  = 1,		//로딩을 시작한상태 
		eHSLoad_HOUSEINFO	  = 2,		//하우스정보
		eHSLoad_FURNITURELIST = 4,		//가구리스트 
		eHSLoad_OK	= eHSLoad_HOUSEINFO + eHSLoad_FURNITURELIST,	//하우스정보와 가구리스트를 읽었으면 로드완료 
	} ;
	DWORD m_dwHouseLoadState;			//하우스 로드상태 변수 

	cPtrList m_BackupPacketList;		//백업 패킷리스트 
	BOOL BackupNetworkMsg(BYTE Protocol,void* pMsg);				//로드중에 다른 패킷이 들어오면 리스트에 백업후 로드가 끝난후 일괄처리 

	//서버와 연동되는 변수 
private:
	stHouse							m_CurHouse;						//하우스각종정보 
	BOOL							m_bHouseOwner;					//집주인인가 여부
	BYTE							m_byRankHouse;					//하우스 랭킹정보
	BOOL							m_bDecorationMode;				//꾸미기모드여부

	//관리를 위해 클라이언트 자체에서 쓰는 변수 
private:
	stHousingSettingInfo			m_HousingSettingInfo;			//하우스셋팅
	CMemoryPoolTempl<stFurniture>	m_stFurniturePool;				//stFurniture 메모리풀 
	CYHHashTable<CFurniture>		m_FieldFurnitureTable;			//필드에 설치된 cFuniture리스트  
	cPtrList						m_AlphaHandleList;				//알파프로세스 리스트 
	cPtrList						m_WaitForMakingObjList;			//마킹을 대기하고있는 오브젝트 리스트 
	DWORD							m_dwLastRemainProcessTime;		//ProcessRemainTime()
	DWORD							m_dwFieldFurnitureNum;			//필드에 설치된 가구숫자 

	BOOL							m_bDoDecoration;		//가구를 필드에 셋팅중 
	BOOL							m_bDecoMove;			//픽업이동
	BOOL							m_bDecoAngle;			//설치회전

	GXOBJECT_HANDLE					m_PickAnotherObjHandle;	//설치시 피킹된 가장 가까운 가구 오브젝트 핸들 ( 설치중인 Obj는 제외 ) 
	CFurniture*						m_pDecoObj;				//설치obj
	VECTOR3							m_vDecoPos;				//현재위치 
	VECTOR3							m_vBackupDecoPos;		//변경전 위치 
	float							m_fBackupDecoAngle;		//변경전 앵글 

	struct stUseActionInfo			//가구액션정보 
	{
		stFurniture* pActionFuniture;	//선택된 액션을 보유 가구  
		stHouseActionInfo* pActionInfo;	//액션의 정보 

		stUseActionInfo(){Clear();}
		void Clear(){ memset(this, 0, sizeof(stUseActionInfo)); }
	} ;
	stUseActionInfo m_CurActionInfo;	//현재선택된 액션 

	//091006 pdy 꾸미기 모드시 카메라 필터 추가 HousingSetting.Bin에서 읽어온다. 
	DWORD							m_dwCameraFilterIdxOnDecoMode;

public:
	cHousingMgr(void);
	virtual ~cHousingMgr(void);

	void HousingInfoClear();				//하우징정보 클리어 
	void LoadHousing_Setting();				//하우스 각종 셋팅값 로드 
	void Init();							
	void Release();							
	void Process();			
	void NetworkMsgParse(BYTE Protocol,void* pMsg);	

	DWORD GetCurHouseOwnerIndex()		{return m_CurHouse.HouseInfo.dwOwnerUserIndex;}

public:
	void ProcessCulling();					//카메라 각도에 맞추어 외벽 & 벽장식 컬링 

	void ProcessAlpha();					//가구가 캐릭터를 가리면 알파를 먹인다.

	void ProcessWaitForMaikingObjList();	//마킹시도시 HERO와 충돌되어 대기중인 가구의 리스트처리 	

	void ProcessRemainTime();				//가구의 시간을 1분에 한번씩 깍는다 

	void ProcessMouseEvent_DecorationMode(CMouse* Mouse,DWORD MouseEvent);	//꾸미기모드시 마우스이벤트 처리 

	void MoveDecorationMousePoint(CMouse* Mouse);							//가구의 마우스 피킹 이동 처리 

public:
	BOOL RequestStoredItem(POSTYPE ToPos, CItem * pFromItem);					//보관을 요청
	BOOL RequestRestoredICon(POSTYPE ToPos, cHousingStoredIcon * pFromIcon);	//보관해제를 요청 

	BOOL RequestInstallCurDeco();												//필드에 설치 요청
	BOOL RequestUninstallDecoFromUseIcon(stFurniture* pstFurniture);			//아이콘 더블클릭 설치해제 요청
	BOOL RequestUninstallCurDeco();												//가구 꾸미기중 설치해제 요청
	BOOL RequestUpdateCurDeco();												//가구의 셋팅 변경 요청
	BOOL RequestUseDecoBonus(DWORD dwBonusIndex);								//꾸미기보너스 사용 요청
	BOOL FakeUseAction(stFurniture* pFuniture,stHouseActionInfo* ActionInfo);	//액션팝업창 클릭시 호출 
	BOOL RequestUseCurAction();													//현재 선택된 액션을 요청 
	BOOL RequestAction_GetOff();

	void RequestDecoRationMode();							//꾸미기모드 On/Off 요청 
	void RequestDestroyFuniture(stFurniture* pFuniture);	//가구 버리기 요청 

public:
	void House_Create_Ack(void* pMsg);						//하우스 생성	
	void House_Create_Nack(void* pMsg);						//하우스 생성실패
	void House_Entrance_Nack(void* pMsg);					//하우스 입장실패 
	void House_Info(void* pMsg);							//하우스 정보			
	void House_FurnitureList(void* pMsg);					//가구정보 리스트 
	void House_Stored_Ack(void* pMsg);						//가구창고보관		(인벤 아이템 -> 창고 가구) 
	void House_Stored_Nack(void* pMsg);						//가구창고보관실패
	void House_Restored_Ack(void* pMsg);					//가구창고보관해제	(창고 가구 -> 인벤아이템 )
	void House_Restored_Nack(void* pMsg);					//가구창고보관해제 실패 
	void House_Decomode_Ack(void* pMsg);					//꾸미기모드 On/Off   
	void House_Decomode_Nack(void* pMsg);					//꾸미기모드 On/Off 실패 
	void House_Install_Ack(void* pMsg);						//필드에 가구설치
	void House_Install_Nack(void* pMsg);					//필드에 가구설치 실패 
	void House_UnInstall_Ack(void* pMsg);					//필드에서 가구설치 해제 
	void House_UnInstall_Nack(void* pMsg);					//필드에서 가구설치 해제 실패
	void House_Action_Force_GetOff_Ack(void* pMsg);			//가구에 대한 강제 내리기 
	void House_Action_Force_GetOff_Nack(void* pMsg);		//가구에 대한 강제 내리기 실패 
	void House_Action_Ack(void* pMsg);						//가구에 대한 액션 발동 
	void House_Action_Nack(void* pMsg);						//가구에 대한 액션 발동 실패 
	void House_Bonus_Ack(void* pMsg);						//꾸미기 보너스 사용
	void House_Bonus_Nack(void* pMsg);						//꾸미기 보너스 사용 실패 
	void House_Update_Ack(void* pMsg);						//가구 정보 갱신 (위치,회전,기간오버삭제)
	void House_Update_Nack(void* pMsg);						//가구 정보 갱신 실패 
	void House_Update_Material_Ack(void* pMsg);				//가구 머터리얼 정보 갱신
	void House_Destroy_Ack(void* pMsg);						//가구 삭제 
	void House_Destroy_Nack(void* pMsg);					//가구 삭제 실패 
	void House_Vote_Ack(void* pMsg);						//하우스 추천 
	void House_Vote_Nack(void* pMsg);						//하우스 추천 실패
	void House_Exit_Nack(void* pMsg);						//하우스 나가기 실패 
	void House_Notify_Visit(void* pMsg);					//Player 하우스 방문 알림
	void House_Notify_Exit(void* pMsg);						//Player 하우스 나가기 알림
	void House_Notify_Action(void* pMsg);					//Player 액션 알림 					
	void House_Extend(void* pMsg);							//하우스 확장 처리 (문 & 문설치류 & 기본제공가구가 설치해제or교체 된다)
	void House_UseItem_Nack(void* pMsg);					//하우스 관련아이템 사용실패 
	void House_Cheat_GetInfoAll_Ack(void* pMsg);			//모든 하우스 정보 출력 치트
	void House_Cheat_GetInfoOne_Ack(void* pMsg);			//특정 하우스 정보 출력 치트 
	void House_Cheat_Delete_Ack(void* pMsg);				//특정 하우스 삭제 
	void House_Cheat_Delete_Nack(void* pMsg);				//특정 하우스 삭제 실패 

public:
	BOOL		IsHouseInfoLoadOk(){return (m_dwHouseLoadState == eHSLoad_OK)? TRUE : FALSE;  }	//현재 로드가 완료되었는가 
	void		StartHouseInfoLoading();														//하우스 로드시작 
	void		PrintDebugErrorMsg(char* pstrMsg,WORD dwError);									//ifdef _GM_TOOL_ 일때 메세지창에 띄워준다 

public:	
	CObjectBase*				 GetSelectedObjectFromHouseMap(int MouseX,int MouseY,int PickOption , BOOL bSort);	//하우징전용 GetSelectedObject()
	stDynamicHouseNpcMapInfo*	 GetDynimiHouseNpcMapInfo(DWORD dwMapIndex);										//해당맵의 동적생성 랭킹하우스NPC 정보리스트를 가져온다
	stDynamicHouseNpcInfo*		 GetDynimiHouseNpcInfo(DWORD dwMapIndex, BYTE byRank, BYTE byExterioKind);			//동적생성 랭킹하우스NPC 정보를 가져온다.
	stDynamicHouseNpcInfo*		 FindDynimiHouseNpcInfoByNpcKind(DWORD dwMapIndex , DWORD dwNpcKind);				//동적생성 랭킹하우스NPC 정보를 dwNpcKind로 검색하여 가져온다.
	WORD						 GetStoredFunitureNumByCategory(WORD dwCategoryIdx);								//해당카테고리에 가구가 몇개 보관중인지 가져온다.
	WORD						 GetBlankSlotIndexFromStoredFunitureListByCategory(WORD dwCategoryIdx);				//해당카테고리의 빈슬롯 인덱스를 가져온다.
	DWORD						 GetCameraFilterIdxOnDecoMode(){ return m_dwCameraFilterIdxOnDecoMode; }			//꾸미기 모드시 띄울 카메라필터 인덱스를 가져온다.

public:
	BOOL		IsHousingMap();										//하우징 맵인가
	BOOL		IsRankHouse()		{return m_byRankHouse;}			//랭커하우스 인가
	BOOL		IsDecorationMode()	{return m_bDecorationMode;}		//꾸미기 모드인가
	BOOL		IsDecoMove()		{return m_bDecoMove;}			//현재 이동셋팅 유효한가
	BOOL		IsDecoAngle()		{return m_bDecoAngle;}			//현재 회전셋팅이 유효한가
	BOOL		IsHouseOwner()		{return m_bHouseOwner;}			//현재 하우스의 주인인가
	BOOL		IsActionableFurniture(CFurniture* pFurniture);		//액션이 있는 가구인가 
	bool		IsFieldHeightObject(CObject* pObject);				//필드의 높이와 같은 오브젝트인가
	BOOL		IsDoDecoration()	{return m_bDoDecoration;}		//가구를 필드에 설치중인가
	BOOL		IsHouseOwnerID(DWORD dwUserID)	{ return ( dwUserID == m_CurHouse.HouseInfo.dwOwnerUserIndex ) ? TRUE : FALSE; }

	float		GetStartAngle();									//시작위치를 가져오자.

public:
	BOOL CanUseItemFromHousingMap(CItem* pItem);								//하우징맵에서 사용가능한 아이템인가 

	BOOL CanUseItemFromDecorationMode(CItem* pItem);							//꾸미기모드중에 사용가능한 아이템인가

	BOOL CanExtendHouse(){return (m_CurHouse.HouseInfo.ExtendLevel < MAX_HOUSE_EXTEND_LEVEL-1 ) ? TRUE : FALSE; };	//확장을 더할수있는가 

	BOOL CanUseDecoBonus(stHouseBonusInfo* pBonusInfo,DWORD* pdwErr = NULL );	//해당 보너스를 사용할수 있는가 

	BOOL CanInstallFunitureToField();											//현재 셋팅중인 가구가 필드에 설치가 가능한가

	BOOL CanMakingToFiled(CObject* pMarkingObj);								//필드에 마킹을 할 수 있는가 (HERO BOX vs Marking Box)

	BOOL CanResetDecorationFuniture(CFurniture* pFurniture);					//리셋을 할수있는 가구인가 ?

	BOOL CheckPercentShellToBoundingBox( BOUNDING_BOX* pSrcBoundingBox, VECTOR3* pMakingShellCenter);	//해당셀에 마킹을 할수있는가 채크 

	BOOL TestCollisionBox2Box(BOUNDING_BOX* pSrcBoundingBox, BOUNDING_BOX* pDestBoundingBox , BOOL bCheckHeight = TRUE);	//박스 vs 박스 충돌채크 

public:
	void		SetDecorationModeTemp();					//꾸미기 모드 임시셋팅(서버인증받기 전 임시로 클라에서먼저 처리)
	void		SetDecorationMode(BOOL bMode);				//꾸미기 모드 셋팅

	void		StartDecoration(stFurniture* pstFuniture);	//가구 셋팅시작 ( Start -> Install)
	void		EndDecoration();							//가구 셋팅끝 

	void		CancelDecoration();							//가구 셋팅 취소
	BOOL		DecideSettingCurDeco();						//가구 셋팅 확정 

	void		StartDecoMove();							//가구 이동 셋팅 시작 
	void		EndDecoMove();								//가구 이동 셋팅 끝

	void		StartDecoAngle();							//가구 회전 셋팅 시작
	void		EndDecoAngle();								//가구 회전 셋팅 끝

	BOOL		StartDecoReset(CObject* pPickObject);		//가구 리셋팅 시작 (Start -> Update ) 
	
	CFurniture*	MakeDummyFuniture(stFurniture* pstFuniture);	//가구 셋팅시 필드에 없는가구는 Dummy 생성 
	CFurniture*	MakeFuniture(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,stFurniture* pstFuniture);	//가구오브젝트 만들기 

	void		RemoveDeco(DWORD dwObjectID);				//가구삭제
	void		RemoveCurDeco();							//현재 셋팅중인 가구 삭제
	void		DestroyFuniture(stFurniture* pDestroyFn);	//가구정보 삭제 

	BOOL		InstallFunitureToField(stFurniture* pstFuniture , BOOL bFirstInstall = FALSE);	//가구정보로 가구를 만들어 필드에 설치를 한다
	BOOL		UpdateFunitere(stFurniture* pstFuniture);										//가구정보를 참조하여 가구 업데이트 
	BOOL		UnInstallFunitureFromField(DWORD dwObjectIdx);									//해당 아이디의 가구 언인스톨 

	void		MarkingObjZone(CObject* pObject, bool bMakingObjZone);							//TRUE : 필드에 마킹 , FALSE : 필드에서 마킹해제 
	void		AddWaitForMakingObj(CObject* pWaitObj);											//마킹대기 리스트에 등록 (HERO의 위치와 충돌시) 
	void		RemoveWaitForMakingObj(CObject* pWaitObj);										//마킹대기 리스트에서 소멸
	void		FindOverapMarkingToHeroObj(cPtrList* pResultList);								//HERO의 영역과 충돌되는 곳에 마킹된 오브젝트리스트를 리턴
	void		CheckAllMarkingObjByHeroZone();													//HERO의 영역과 충돌되는 곳에 마킹된 오브젝트들을 마킹해제후 마킹대기리스트에 등록

	void		ChangeMaterialFurniture(DWORD dwFurnitureKind,DWORD dwMaterialIndex,BYTE byFlag);	//벽,천장,바닥 머터리얼 변경 
	void		RideOnPlayerToFuniture(CPlayer* pPlayer);										//가구에 탑승액션 처리
	void		RideOffPlayerFromFuniture(CPlayer* pPlayer, BOOL bForceRideOff=FALSE);			//가구에 내기리액션 처리  


//////////////////////////////////////////////////////////////////////////////////////////////////
//여기서부터는 테스트를위한 코드 
//////////////////////////////////////////////////////////////////////////////////////////////////
public:
	// 마킹테스트용 코드 
	struct cMarkingTest
	{
		CObject*	m_pMarkinObj;
		VECTOR3*	m_pvPos;
		CMovePoint*	m_pTestShowObj;
	};
	cPtrList	m_MarkinTestList;		//설치된 가구가 마킹되면 이쪽으로 ADD된다 

	bool		m_bShowTest;			//TRUE : 마킹된 셀 표시 , FALSE : 마킹된 셀 표시안함 
	DWORD		m_dwLastShowTick;		

	void		ShowAllMarkingTest();	
	void		SetShowTestMaking(bool bShow);

	void		AddMarkingTest(CObject* pMarkingObj,VECTOR3* Pos);	
	void		ReleaseMarkingTest(CObject *pMarkingObj);		
	void		ReleaseAllMarkingTest();
	bool		IsShowTestMaking(){return m_bShowTest;}


};

EXTERNGLOBALTON(cHousingMgr)
