#include "cHousingMgr.h"

#include "MouseCursor.h"
#include "MHMap.h"
#include "npc.h"
#include "GameResourceManager.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "tilemanager.h"
#include "ObjectStateManager.h"
#include "AppearanceManager.h"
#include "Furniture.h"
#include "MouseEventReceiver.h"
#include "Item.h"
#include "mhfile.h"
#include "GameIn.h"
#include "MHCamera.h"
#include "ChatManager.h"
#include "MainTitle.h"
#include "ItemManager.h"
#include "cScrollIconGridDialog.h"
#include "cWindowManager.h"
#include "WindowIDEnum.h"
#include "cMsgBox.h"
#include "MiniMapDlg.h"
#include "../hseos/Monstermeter/SHMonstermeterDlg.h"

#include "cHouseNameDlg.h"
#include "InventoryExDialog.h"
#include "HousingRevolDlg.h"
#include "cHouseSearchDlg.h"
#include "cHousingDecoPointDlg.h"
#include "cHousingActionPopupMenuDlg.h"
#include "cHousingMainPointDlg.h"
#include "cHousingWarehouseDlg.h"
#include "cHousingWarehouseButtonDlg.h"
#include "cHousingStoredIcon.h"

//090708 pdy 기획팀 요청으로 UI 수정 
#include "cHousingDecoModeBtDlg.h"
#include "cHousingWebDlg.h"
#include "../[cc]skill/Client/Info//ActiveSkillInfo.h"
#include "../[cc]skill/client/manager/skillmanager.h"

#include "../input/UserInput.h"

GLOBALTON(cHousingMgr)
cHousingMgr::cHousingMgr(void)
{
	HousingInfoClear();
}

cHousingMgr::~cHousingMgr(void)
{
	Release();
}

void cHousingMgr::HousingInfoClear()				// 하우징관련정보 한방에 클리어.
{
	m_bDecorationMode = FALSE;	
	m_bDecoMove = FALSE;		
	m_bDecoAngle = FALSE;
	m_pDecoObj = NULL;
	m_PickAnotherObjHandle = NULL;
	m_bDoDecoration = FALSE;
	m_bHouseOwner = FALSE;
	m_bShowTest = FALSE;
	m_dwCameraFilterIdxOnDecoMode = 0 ;

	//091016 pdy 필드 설치 한도 카운트가 초기화 안되있는 버그 수정
	m_dwFieldFurnitureNum = 0;

	m_CurActionInfo.Clear();
	m_CurHouse.Clear();
}

void cHousingMgr::Init()
{
	HousingInfoClear();
	Release();

	//테스트
	LoadHousing_Setting();

	if( !IsHousingMap() ) 
		return;

	m_stFurniturePool.Init(MAX_FURNITURE_STORAGE_NUM, 50 ,"stFurniture");
	m_CurHouse.pFurnitureList.Initialize(MAX_FURNITURE_STORAGE_NUM);
	m_CurHouse.pNotDeleteFurnitureList.Initialize(100);
	m_FieldFurnitureTable.Initialize(MAX_FURNITURE_STORAGE_NUM);	//090325 pdy 필드에 최대등록수는 카테고리 * 슬롯  

}

void cHousingMgr::LoadHousing_Setting()
{
	m_HousingSettingInfo.Clear();

	CMHFile file;

	char StrBuf[256] = {0,};

	bool bPasing = FALSE;
	int	 iGroupCount = -1;
	DWORD dwCount = 0;
	const float f_1Radian = 0.01745f;
	const float fPI	  = 3.14159f;
	DWORD dwCurMapNum = 0;

	if( ! file.Init("System/Resource/Housing_Setting.bin", "rb" ) )
		return;

	while( ! file.IsEOF() )
	{
		file.GetString(StrBuf);

		if(! bPasing)		
		{
			//Todo OneLine Passing 
			if( StrBuf[0] != '#' )
			{
				file.GetLine(StrBuf,256);
				continue;
			}
			else if( strcmp( StrBuf , "#RANKPOINT_WEIGHT" ) == 0 )				//하우징 순위산출 가중치 
			{
				m_HousingSettingInfo.fDecoPoint_Weight	= file.GetFloat();
				m_HousingSettingInfo.fRecommend_Weight	= file.GetFloat();
				m_HousingSettingInfo.fVisitor_Weight	= file.GetFloat();
				continue;
			}
			else if( strcmp( StrBuf , "#STARPOINT_NOMAL" ) == 0 )				//별포인트		(별포인트 = 꾸미기보너스 사용시 차감되는 포인트)
			{
				m_HousingSettingInfo.dwStarPoint_Nomal = file.GetDword();
			}
			else if( strcmp( StrBuf , "#STARPOINT_RANKER" ) == 0 )				//랭커용 별포인트 
			{
				m_HousingSettingInfo.dwStarPoint_Ranker = file.GetDword();

			}
			else if(strcmp( StrBuf , "#RANKINGDAY" ) == 0 )						//랭킹을 계산하는 날자 
			{
				m_HousingSettingInfo.dwRankingDay = file.GetDword();
			}
			else if( strcmp( StrBuf , "#GENERALFUNITURE_NUM" ) == 0 )			//기본제공가구 숫자 ( 단계별로 존재 ) 
			{
				iGroupCount++;
				dwCount=0;

				DWORD dwNum = file.GetFloat();
				m_HousingSettingInfo.dwGeneralFunitureNumArr[iGroupCount] = dwNum;
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ] = new stGeneralFunitureInfo[ dwNum ];
				continue;
			}
			else if( strcmp( StrBuf , "#GENERALFUNITURE" ) == 0 )				//기본제공 가구 정보 
			{
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].dwItemIndex = file.GetDword();
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].vWorldPos.x = file.GetFloat();
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].vWorldPos.y = file.GetFloat();
				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].vWorldPos.z = file.GetFloat();

				float fAngle = 0.0f;
				fAngle = file.GetFloat();
				if( fAngle != 0.0f )
				{
					//스크립트의 각도값은 -180~180도로 넣게 된다. 이것을 라디안 값으로 바꾸자.
					fAngle *= f_1Radian;

					//180도보다 
					if( fAngle > fPI )
						fAngle = fPI ; 

					if( fAngle < -fPI )
						fAngle = -fPI ;

				}

				m_HousingSettingInfo.pGeneralFunitureList[ iGroupCount ][ dwCount ].fAngle	= fAngle;
				dwCount++;
				continue;
			}
			else if( strcmp( StrBuf , "#DynamicRankHouseNpcList" ) == 0 )				//동적생성 랭컹 하우스 Npc List
			{
				stDynamicHouseNpcMapInfo* pMapInfo = new stDynamicHouseNpcMapInfo();
				dwCurMapNum = pMapInfo->MapIndex = file.GetDword();
				m_HousingSettingInfo.m_DynamicHouseNpcMapInfoList.Add( pMapInfo , pMapInfo->MapIndex );
			}
			else if( strcmp( StrBuf , "#DynamicRankHouseNpc" ) == 0 )					//동적생성 랭컹 하우스 Npc 정보 
			{
				stDynamicHouseNpcMapInfo* pMapInfo = m_HousingSettingInfo.m_DynamicHouseNpcMapInfoList.GetData( dwCurMapNum ) ;

				stDynamicHouseNpcInfo* pNpcInfo = new stDynamicHouseNpcInfo();
				pNpcInfo->dwRankTypeIndex = file.GetDword();
				pNpcInfo->dwNpcKind = file.GetDword();
				pNpcInfo->fWorldPosX = file.GetFloat();
				pNpcInfo->fWorldPosZ = file.GetFloat();

				float fAngle = 0.0f;
				fAngle = file.GetFloat();
				pNpcInfo->fAnlge = fAngle;

				pNpcInfo->fDistance  = file.GetFloat();

				pMapInfo->pDynamicHouseNpcList.Add(pNpcInfo , pNpcInfo->dwRankTypeIndex );
			}
			else if( strcmp( StrBuf , "#DynamicRankHouseNpcEnd" ) == 0 )
			{
				dwCurMapNum = 0 ;
			}
			//클라이언트에서만 사용 
			else if(  strcmp( StrBuf , "#CameraFilterIndexOnDecoMode" ) == 0 )
			{
				m_dwCameraFilterIdxOnDecoMode = file.GetDword();
			}
		}
	}
}

void cHousingMgr::Release()
{
	m_HousingSettingInfo.Clear();
	ReleaseAllMarkingTest();

	m_stFurniturePool.Release();			
	m_CurHouse.pFurnitureList.RemoveAll();		
	m_CurHouse.pNotDeleteFurnitureList.RemoveAll();
	m_FieldFurnitureTable.RemoveAll();
	m_WaitForMakingObjList.RemoveAll();
	m_AlphaHandleList.RemoveAll();
}

// 091016 pdy 로딩중 백업 패킷리스트에 Add시 AddHead를 AddTail로 수정
BOOL cHousingMgr::BackupNetworkMsg(BYTE Protocol,void* pMsg)
{
	//로딩중에 백업이 필요한 프로토콜만 넣자 
	switch(Protocol)
	{
		case MP_HOUSE_STORED_NACK:
		case MP_HOUSE_RESTORED_ACK:
		case MP_HOUSE_RESTORED_NACK:
		case MP_HOUSE_DECOMODE_ACK:
		case MP_HOUSE_DECOMODE_NACK:
		case MP_HOUSE_INSTALL_NACK:
		case MP_HOUSE_UNINSTALL_NACK:
		case MP_HOUSE_ACTION_NACK:
		case MP_HOUSE_UPDATE_NACK:	
		case MP_HOUSE_DESTROY_ACK:
		case MP_HOUSE_DESTROY_NACK:		
		case MP_HOUSE_VOTE_ACK:
		case MP_HOUSE_VOTE_NACK:
		case MP_HOUSE_EXIT_NACK:
		case MP_HOUSE_NOTIFY_VISIT:
		case MP_HOUSE_NOTIFY_EXIT:
		case MP_HOUSE_NOTIFY_ACTION:
			{
				MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
				MSG_DWORD* pBackupMsg = new MSG_DWORD;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
		case MP_HOUSE_BONUS_ACK :
		case MP_HOUSE_BONUS_NACK :
			{
				MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
				MSG_DWORD2* pBackupMsg = new MSG_DWORD2;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}	
			break;
		case MP_HOUSE_UNINSTALL_ACK:
			{
				MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
				MSG_DWORD3* pBackupMsg = new MSG_DWORD3;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
		case MP_HOUSE_USEITEM_NACK:	
			{
				MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
				MSG_DWORD4* pBackupMsg = new MSG_DWORD4;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
		case MP_HOUSE_ACTION_FORCE_GETOFF_ACK:
			{
				MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
				MSG_DWORD3* pBackupMsg = new MSG_DWORD3;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
		case MP_HOUSE_ACTION_ACK:
			{
				MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;
				MSG_DWORD5* pBackupMsg = new MSG_DWORD5;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
		case MP_HOUSE_STORED_ACK:
		case MP_HOUSE_INSTALL_ACK:
		case MP_HOUSE_UPDATE_ACK:
			{
				MSG_HOUSE_FURNITURE* pmsg = (MSG_HOUSE_FURNITURE*)pMsg;
				MSG_HOUSE_FURNITURE* pBackupMsg = new MSG_HOUSE_FURNITURE;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
		case MP_HOUSE_EXTEND:
			{
				MSG_HOUSE_EXTEND* pmsg = (MSG_HOUSE_EXTEND*)pMsg;
				MSG_HOUSE_EXTEND* pBackupMsg = new MSG_HOUSE_EXTEND;
				*pBackupMsg = *pmsg;
				m_BackupPacketList.AddTail(pBackupMsg);
			}
			break;
	default:
		{
			return FALSE;
		}
	}

	return TRUE;
}

void cHousingMgr::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	if( m_dwHouseLoadState & eHSLoad_NOWLOADING )		//eHSLoad_NOWLOADING 플레그가 설정되어있다면 하우스 로딩중이다. 
	{
		if ( BackupNetworkMsg( Protocol, pMsg) )		//로딩관련 이외의 패킷은 리스트에 담자 
		{
			return;	//로딩과 관련없는 패킷이라면 백업되었음으로 처리없이 리턴 
		}
	}

	switch(Protocol)
	{
	case MP_HOUSE_CREATE_ACK:			House_Create_Ack(pMsg);break;				//하우스 생성	
	case MP_HOUSE_CREATE_NACK:			House_Create_Nack(pMsg);break;				//하우스 생성실패
	case MP_HOUSE_ENTRANCE_NACK:		House_Entrance_Nack(pMsg);break;			//하우스 입장실패 
	case MP_HOUSE_INFO:					House_Info(pMsg);break;						//하우스 정보			
	case MP_HOUSE_FURNITURELIST:		House_FurnitureList(pMsg);break;			//가구정보 리스트 
	case MP_HOUSE_STORED_ACK:			House_Stored_Ack(pMsg);break;				//가구창고보관		(인벤 아이템 -> 창고 가구) 
	case MP_HOUSE_STORED_NACK:			House_Stored_Nack(pMsg);break;				//가구창고보관실패
	case MP_HOUSE_RESTORED_ACK:			House_Restored_Ack(pMsg);break;				//가구창고보관해제	(창고 가구 -> 인벤아이템 )
	case MP_HOUSE_RESTORED_NACK:		House_Restored_Nack(pMsg);break;			//가구창고보관해제 실패 
	case MP_HOUSE_DECOMODE_ACK:			House_Decomode_Ack(pMsg);break;				//꾸미기모드 On/Off   
	case MP_HOUSE_DECOMODE_NACK:		House_Decomode_Nack(pMsg);break;			//꾸미기모드 On/Off 실패 
	case MP_HOUSE_INSTALL_ACK:			House_Install_Ack(pMsg);break;				//필드에 가구설치
	case MP_HOUSE_INSTALL_NACK:			House_Install_Nack(pMsg);break;				//필드에 가구설치 실패 
	case MP_HOUSE_UNINSTALL_ACK:		House_UnInstall_Ack(pMsg);break;			//필드에서 가구설치 해제 
	case MP_HOUSE_UNINSTALL_NACK:		House_UnInstall_Nack(pMsg);break;			//필드에서 가구설치 해제 실패
	case MP_HOUSE_ACTION_FORCE_GETOFF_ACK:	House_Action_Force_GetOff_Ack(pMsg);	break;	// 가구에서 강제로 내리기
	case MP_HOUSE_ACTION_FORCE_GETOFF_NACK:	House_Action_Force_GetOff_Nack(pMsg);	break;	// 가구에서 강제로 내리기 실패
	case MP_HOUSE_ACTION_ACK:			House_Action_Ack(pMsg);break;				//가구에 대한 액션 발동 
	case MP_HOUSE_ACTION_NACK:			House_Action_Nack(pMsg);break;				//가구에 대한 액션 발동 실패 
	case MP_HOUSE_BONUS_ACK :			House_Bonus_Ack(pMsg);break;				//꾸미기 보너스 사용
	case MP_HOUSE_BONUS_NACK :			House_Bonus_Nack(pMsg);break;				//꾸미기 보너스 사용 실패 
	case MP_HOUSE_UPDATE_ACK:			House_Update_Ack(pMsg);break;				//가구 정보 갱신 (위치,회전,머터리얼,기간오버삭제)
	case MP_HOUSE_UPDATE_NACK:			House_Update_Nack(pMsg);break;				//가구 정보 갱신 실패 
	case MP_HOUSE_UPDATE_MATERIAL_ACK:  House_Update_Material_Ack(pMsg);break;		//가구 머터리얼 정보 갱신
	case MP_HOUSE_DESTROY_ACK:			House_Destroy_Ack(pMsg);break;				//가구 삭제 
	case MP_HOUSE_DESTROY_NACK:			House_Destroy_Nack(pMsg);break;				//가구 삭제 실패 
	case MP_HOUSE_VOTE_ACK:				House_Vote_Ack(pMsg);break;					//하우스 추천 
	case MP_HOUSE_VOTE_NACK:			House_Vote_Nack(pMsg);break;				//하우스 추천 실패
	case MP_HOUSE_EXIT_NACK:			House_Exit_Nack(pMsg);break;				//하우스 나가기 실패 
	case MP_HOUSE_NOTIFY_VISIT:			House_Notify_Visit(pMsg);break;				//Player 하우스 방문 알림
	case MP_HOUSE_NOTIFY_EXIT:			House_Notify_Exit(pMsg);break;				//Player 하우스 나가기 알림
	case MP_HOUSE_NOTIFY_ACTION:		House_Notify_Action(pMsg);break;			//Player 액션 알림 		
	case MP_HOUSE_EXTEND:				House_Extend(pMsg);break;					//하우스 확장 처리 (문 & 문설치류 & 기본제공가구가 설치해제or교체 된다)
	case MP_HOUSE_USEITEM_NACK:			House_UseItem_Nack(pMsg);break;				//하우스 관련아이템 사용실패 
	case MP_HOUSE_CHEAT_GETINFOALL_ACK:	House_Cheat_GetInfoAll_Ack(pMsg);break;		//모든 하우스 정보 출력 치트
	case MP_HOUSE_CHEAT_GETINFOONE_ACK:	House_Cheat_GetInfoOne_Ack(pMsg);break;		//특정 하우스 정보 출력 치트 
	case MP_HOUSE_CHEAT_DELETE_ACK:		House_Cheat_Delete_Ack(pMsg);break;			//특정 하우스 삭제 
	case MP_HOUSE_CHEAT_DELETE_NACK:	House_Cheat_Delete_Nack(pMsg);break;		//특정 하우스 삭제 실패 
	}

	//-----------------------------------------------------------------------------------------------
	//	하우스 로딩이 끝났다면 리스트에 저장된 패킷을 한프레임안에 일괄적으로 처리해주자 
	//-----------------------------------------------------------------------------------------------

	if( m_dwHouseLoadState & eHSLoad_NOWLOADING )						//eHSLoad_NOWLOADING 플레그가 설정되어있다면 로드가 완료되었는지 봐야한다  
	{
		if( (m_dwHouseLoadState - eHSLoad_NOWLOADING) == eHSLoad_OK )	//현프레임에 로드가 완료되었다면 필요한 처리를 하자. 
		{
			m_dwHouseLoadState -= eHSLoad_NOWLOADING;					//eHSLoad_NOWLOADING 플레그제거 로드처리 OK 

			//보관 아이콘 프로세스의 마지막시간을 현프레임으로 
			m_dwLastRemainProcessTime = gCurTime;

#ifdef _GMTOOL_
			//091016 pdy 내부 테스트를 위한 하우징 정보 로그추가
			SYSTEMTIME systemTime ;
			GetLocalTime( &systemTime ) ;
			FILE *fp = fopen("HouseInfoLog.log", "a+");
			if( fp)
			{
				fprintf(fp,"\n");
				fprintf(fp,"HouseLoad_OK [%02d:%02d:%02d] \n", systemTime.wHour , systemTime.wMinute , systemTime.wSecond);
				fprintf(fp,"Installed Furniture Num : %d\n" ,m_dwFieldFurnitureNum);
				fprintf(fp,"Stored Furnitrue Num By OwnerList: %d\n" ,m_CurHouse.pFurnitureList.GetDataNum() ) ;
				for(int i=0; i< MAX_HOUSING_CATEGORY_NUM ; i++)
				{
					fprintf(fp,"Stored Num By Category : Category %d = %d \n",i,m_CurHouse.m_dwCategoryNum[i]);
				}
				fprintf(fp,"BackupPacketList Num %d \n" ,m_BackupPacketList.GetCount() ) ;
				fprintf(fp,"\n");
				fclose(fp);
			}
#endif

			//백업된 패킷을 처리하자 
			PTRLISTPOS pos = m_BackupPacketList.GetHeadPosition();
			while( pos )
			{
				MSGBASE* pBackupMsg = (MSGBASE*)m_BackupPacketList.GetNext( pos ) ;
				if( pBackupMsg )
				{
					NetworkMsgParse(pBackupMsg->Protocol,pBackupMsg);	//eHSLoad_NOWLOADING 플레그가 없어졌기때문에 무한 재귀호출이 아니다. 
					SAFE_DELETE( pBackupMsg );
				}
			}
			m_BackupPacketList.RemoveAll();

			//내집창고가 항상떠있는 것으로 수정되었다. 
			//하우스 로딩이 끝난후 띄워주자. 
			if( GAMEIN->GetHousingWarehouseDlg() )
			{
				GAMEIN->GetHousingWarehouseDlg()->SetActive( TRUE ) ;							
			}
		}
	}
}

void cHousingMgr::House_Create_Ack(void* pMsg)	
{
	//090527 pdy 하우징 시스템메세지 [집생성성공]  
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1880 ));	//하우스 생성 성공

	GAMEIN->GetHouseNameDlg()->SetDisable(FALSE);
	GAMEIN->GetHouseNameDlg()->SetActive(FALSE);
}

void cHousingMgr::House_Create_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	//090527 pdy 하우징 시스템메세지 서버NACK [집생성실패]

	//이미 하우스를 가지고있다.
	if( eHOUSEERR_HAVEHOUSE == pmsg->dwData )		
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1881 ) ); 
	}
	//주민등록증 없음
	else if( eHOUSEERR_NOTREGIST == pmsg->dwData )	
	{				
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1284 ) );
	}
	//하우스 생성 실패 
	else											
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1882 ) ); 
	}

	PrintDebugErrorMsg( "MP_HOUSE_CREATE_NACK" ,pmsg->dwData);

	GAMEIN->GetHouseNameDlg()->SetDisable(FALSE);
}

void cHousingMgr::House_Entrance_Nack(void* pMsg)
{
	MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
	DWORD dwErr			= pmsg->dwData1;
	DWORD dwLinkKind	= pmsg->dwData2;
	DWORD dwSlot		= pmsg->dwData4;

	//090527 pdy 하우징 시스템메세지 서버NACK [집방문 실패]  
	switch(dwErr)
	{
		case eHOUSEERR_NOHOUSE :	//하우스 미존재 
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1883 ) ); 
			break;
		case eHOUSEERR_FULL :		//동시 Open 하우스 재한 (1000개)
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1875 ) );
			break;
		case eHOUSEERR_HOME2HOME_FAIL:	//내집 -> 내집 이동 막기 
			CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1904) ); 
			break;
		case eHOUSEERR_ONVEHICLE:		//탈것 -> 집이동 실패 
			CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(2037) ); 
			break;
		default:					//입장 실패
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1884 ) );
			break;
	}

	PrintDebugErrorMsg("MP_HOUSE_ENTRANCE_NACK",dwErr);

	if( dwLinkKind == eHouseVisitByItem )
	{
		CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForPos(dwSlot);
		if( pItem )
			pItem->SetLock(FALSE);
	}

	GAMEIN->GetHouseSearchDlg()->SetDisableState(FALSE);
}

void cHousingMgr::House_Info(void* pMsg)
{
	//하우스 각종 정보를 받아 놓는다  

	MSG_HOUSE_HOUSEINFO* pmsg = (MSG_HOUSE_HOUSEINFO*)pMsg;
	m_CurHouse.dwChannelID = pmsg->dwChannel;
	m_CurHouse.dwDecoPoint = pmsg->dwDecoPoint;
	m_byRankHouse = pmsg->cRank;

	memcpy(&(m_CurHouse.HouseInfo),&(pmsg->HouseInfo),sizeof(stHouseInfo));					//하우스 정보를 복사한다
	
	DWORD dwUserIdx = TITLE->GetUserIdx();													
	m_bHouseOwner = (dwUserIdx ==  m_CurHouse.HouseInfo.dwOwnerUserIndex )? TRUE : FALSE;	//집주인인가 확인후 저장한다.

	cHousingMainPointDlg* pMainPointDlg = NULL;
	pMainPointDlg = GAMEIN->GetHousingMainPointDlg();

	if( pMainPointDlg )																		//매인 유아이의 집정보를 셋팅한다 
	{
		pMainPointDlg->SetDecoPoint( m_CurHouse.dwDecoPoint );
		pMainPointDlg->SetVisitCount( m_CurHouse.HouseInfo.dwTotalVisitCount ) ;
	}

	if( GAMEIN->GetHousingDecoPointDlg() )
		GAMEIN->GetHousingDecoPointDlg()->RefreshBonusList(m_CurHouse.dwDecoPoint,m_CurHouse.HouseInfo.dwDecoUsePoint);

	// NYJ
	if(pmsg->bInit)																			//하우스 정보가 최초로딩때 온것이라면 
	{
		m_dwHouseLoadState |= eHSLoad_HOUSEINFO;											//m_dwHouseLoadState에 eHSLoad_HOUSEINFO를 추가 

		if( pMainPointDlg )
			pMainPointDlg->SetHouseName( m_CurHouse.HouseInfo.szHouseName );				//매인 유아이의 집이름은 최초로딩때 한번 셋팅한다 

		if( GAMEIN->GetHousingMainPointDlg() )												//집정보가 왔으니 매인 유아이를 연다  
			GAMEIN->GetHousingMainPointDlg()->SetActive( TRUE ) ;

		if( GAMEIN->GetHousingWarehouseButtonDlg())											//창고버튼도 연다 
			GAMEIN->GetHousingWarehouseButtonDlg()->SetActive( TRUE ) ;

		//090708 pdy 기획팀 요청으로 UI 수정 
		if( GAMEIN->GetHousingDecoModeBtDlg())												//꾸미기 모드 버튼도 연다 
			GAMEIN->GetHousingDecoModeBtDlg()->SetActive( TRUE ) ;

	}

#ifdef _GMTOOL_
	//091016 pdy 내부 테스트를 위한 하우징 정보 로그추가
	if( pmsg->bInit )
	{
		SYSTEMTIME systemTime ;
		GetLocalTime( &systemTime ) ;
		FILE *fp = fopen("HouseInfoLog.log", "a+");
		if( fp)
		{
			fprintf(fp,"MP_HOUSE_INFO HouseName : %s Chanel : %d [%02d:%02d:%02d] \n", pmsg->HouseInfo.szHouseName,pmsg->dwChannel, systemTime.wHour , systemTime.wMinute , systemTime.wSecond);
			fprintf(fp,"\n");
			fclose(fp);
		}
	}
#endif
}

void cHousingMgr::House_FurnitureList(void* pMsg)
{
	//보유중인 가구 리스트를 받는다 현재 카테고리 별로 한뭉텅이씩 날라온다.

	static DWORD dwLoadCount = 0;

	MSG_HOUSE_FURNITURELIST* pmsg = (MSG_HOUSE_FURNITURELIST*)pMsg;

	if( pmsg->wNum )
	{
		DWORD dwUserIdx = TITLE->GetUserIdx();
	
		for( int i=0; i < pmsg->wNum ; i++)
		{  
			if( pmsg->Furniture[i].dwObjectIndex )
			{
				m_CurHouse.m_dwFurnitureList[pmsg->wCategory][pmsg->Furniture[i].wSlot] = pmsg->Furniture[i].dwObjectIndex;

				//상태값이 인스톨이면 필드에 설치 
				if( pmsg->Furniture[i].wState == eHOUSEFURNITURE_STATE_INSTALL )
				{
					InstallFunitureToField( &pmsg->Furniture[i] , TRUE );
				}

				if( pmsg->Furniture[i].dwOwnerUserIndex == dwUserIdx )
				{
					//집주인이면 퍼니쳐리스트에 넣자 (창고에 보유 가구리스트)
					stFurniture* pNewFuniture = m_stFurniturePool.Alloc();
					*pNewFuniture = pmsg->Furniture[i];
					m_CurHouse.pFurnitureList.Add( pNewFuniture, pNewFuniture->dwObjectIndex );

					if( pmsg->Furniture[i].bNotDelete  )
					{
						m_CurHouse.pNotDeleteFurnitureList.Add(pNewFuniture , pNewFuniture->dwObjectIndex);
					}

					cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();	
					pDlg->AddIcon( pNewFuniture );													//창고Dlg에 아이콘을 Add 

					m_CurHouse.m_dwCategoryNum[pmsg->wCategory]++;									//카테고리에 보유갯수 ++ 
				}
				else if( pmsg->Furniture[i].bNotDelete )
				{
					CFurniture*  pFurniture = m_FieldFurnitureTable.GetData(pmsg->Furniture[i].dwObjectIndex);		
					if(! pFurniture )
						continue;

					//기본설치 품목은 집주인이 아니어도 가지고 있자. 
					m_CurHouse.pNotDeleteFurnitureList.Add(pFurniture->GetFurnitureInfo() , pFurniture->GetFurnitureInfo()->dwObjectIndex);
				}
			}
		}
	}

	dwLoadCount++;
	if( MAX_HOUSING_CATEGORY_NUM == dwLoadCount	)
	{
		m_dwHouseLoadState |= eHSLoad_FURNITURELIST;
		dwLoadCount = 0;

		//090608 pdy 마킹관련 버그 수정
		CheckAllMarkingObjByHeroZone();		//모든가구를 받았다면 히어로와 겹치는 마킹이 되어있는지 채크 
	}

#ifdef _GMTOOL_
	//091016 pdy 내부 테스트를 위한 하우징 정보 로그추가
	SYSTEMTIME systemTime ;
	GetLocalTime( &systemTime ) ;
	FILE *fp = fopen("HouseInfoLog.log", "a+");
	if( fp)
	{
		fprintf(fp,"MP_HOUSE_FURNITURELIST Categoty : %d [%02d:%02d:%02d] \n", pmsg->wCategory,systemTime.wHour , systemTime.wMinute , systemTime.wSecond);
		fclose(fp);
	}
#endif
}

void cHousingMgr::House_Stored_Ack(void* pMsg)
{
	//가구 창고에 보관 
	MSG_HOUSE_FURNITURE* pmsg = (MSG_HOUSE_FURNITURE*)pMsg;

	DWORD dwUserIdx = TITLE->GetUserIdx();
	if( pmsg->Furniture.dwOwnerUserIndex ==  dwUserIdx )							//가구 주인일경우 
	{
		stFurniture* pNewFuniture = m_stFurniturePool.Alloc();						//메모리 할당 

#ifdef _GMTOOL_
		if(! pNewFuniture)
			CHATMGR->AddMsg( CTC_SYSMSG, "HousingErr(Cli) : Memory Alloc Failed" );

		if(! pNewFuniture->dwObjectIndex)
			CHATMGR->AddMsg( CTC_SYSMSG, "HousingErr(Srv) : ObjectIndex Alloc Failed" );
#endif

		*pNewFuniture = pmsg->Furniture;
		m_CurHouse.pFurnitureList.Add( pNewFuniture, pNewFuniture->dwObjectIndex );	//창고테이블에 등록 

		cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();				

		pDlg->AddIcon( pNewFuniture );												//창고Dlg에 아이콘을 Add 
		pDlg->FocusOnByFurniture( pNewFuniture );									//스크롤,텝 등을 현가구로 맞춰준다.

		m_CurHouse.m_dwFurnitureList[pNewFuniture->wCategory][pNewFuniture->wSlot] = pNewFuniture->dwObjectIndex;
		m_CurHouse.m_dwCategoryNum[pNewFuniture->wCategory]++;						//카테고리에 보유갯수 ++ 


		const ITEM_INFO* info = ITEMMGR->GetItemInfo( pNewFuniture->dwLinkItemIndex );

		if( info )
		{
			//090527 pdy 하우징 시스템메세지 [인벤->창고 보관성공]
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1879 ) , info->ItemName ); //1879	"%s가 내 집 창고로 이동 하였습니다."
		}
	}
}

void cHousingMgr::House_Stored_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_STORED_NACK",pmsg->dwData);
}

void cHousingMgr::House_Restored_Ack(void* pMsg)
{
	//가구 창고에서 보관해제 
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	DWORD dwUserIdx = TITLE->GetUserIdx();
	if( m_CurHouse.HouseInfo.dwOwnerUserIndex == dwUserIdx)
	{
		stFurniture* pRestoredFn = m_CurHouse.pFurnitureList.GetData(pmsg->dwData);	//가구 주인일경우가지고있다 
		if( pRestoredFn )
		{
			//보관해제이므로 가구를 삭제하자
			DestroyFuniture(pRestoredFn);
		}
	}
}

void cHousingMgr::House_Restored_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	stFurniture* pRestoredFn = m_CurHouse.pFurnitureList.GetData(pmsg->dwData);	//가구 주인일경우가지고있다 
	if( pRestoredFn )
	{
		cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();

		if( pDlg)
		{
			cHousingStoredIcon* pICon = pDlg->GetStoredIconIcon(pRestoredFn->wCategory-1 , pRestoredFn->wSlot);

			if( pICon )
				pICon->SetLock(FALSE);
		}
	}
	PrintDebugErrorMsg("MP_HOUSE_RESTORED_NACK",pmsg->dwData);
}

void cHousingMgr::House_Decomode_Ack(void* pMsg)
{
	//꾸미기 모드 On/Off
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	if( pmsg->dwObjectID == gHeroID)
	{
		//집주인이 자신일때 꾸미기 모드 실행 
		SetDecorationMode(BOOL(pmsg->dwData));
	}
	else
	{
		//3자일때 
		CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwObjectID);
		if(! pPlayer )
			return;

		if( pmsg->dwData ) 
		{
			OBJECTSTATEMGR->StartObjectState(pPlayer, eObjectState_Housing);
		}
		else
		{
			OBJECTSTATEMGR->EndObjectState(pPlayer, eObjectState_Housing);
		}
	}
}

void cHousingMgr::House_Decomode_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	if( pmsg->dwObjectID == gHeroID)
	{
		PrintDebugErrorMsg("MP_HOUSE_DECOMODE_NACK",pmsg->dwData);
	}
}

void cHousingMgr::House_Install_Ack(void* pMsg)
{
	//가구 필드에 설치 
	MSG_HOUSE_FURNITURE* pmsg = (MSG_HOUSE_FURNITURE*)pMsg;
	DWORD dwUserIdx = TITLE->GetUserIdx();

	if( pmsg->Furniture.dwOwnerUserIndex ==  dwUserIdx )						//가구 주인일경우 가구정보,아이콘,꾸미기포인트,보너스리스트,필드에설치
	{
		stFurniture* pFuniture = m_CurHouse.pFurnitureList.GetData(pmsg->Furniture.dwObjectIndex);

		if( ! pFuniture )
			return;

		*pFuniture = pmsg->Furniture;

		InstallFunitureToField( &pmsg->Furniture );										
		cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();	

		//090708 pdy RefreshIcon 수정 
		pDlg->RefreshIcon( pFuniture );

		m_CurHouse.dwDecoPoint = pmsg->dwDecoPoint;

		if( IsHouseOwner() )
		{
			if( GAMEIN->GetHousingMainPointDlg() )
				GAMEIN->GetHousingMainPointDlg()->SetDecoPoint( m_CurHouse.dwDecoPoint );

			if( GAMEIN->GetHousingDecoPointDlg() )
				GAMEIN->GetHousingDecoPointDlg()->RefreshBonusList(m_CurHouse.dwDecoPoint,m_CurHouse.HouseInfo.dwDecoUsePoint);
		}
	}
	else		
	{	
		InstallFunitureToField( &pmsg->Furniture );								//가구주인이 아닌경우 필드에 모델 설치 
	}
}

void cHousingMgr::House_Install_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_INSTALL_NACK",pmsg->dwData);

	RemoveCurDeco();
	EndDecoration();
}

void cHousingMgr::House_UnInstall_Ack(void* pMsg)
{
	//가구 필드에서 설치 해제 

	//설치 해제 되는 가구정보가 다날라오는 것이 아니라 오브젝트 인덱스가 날라와 
	//클라이언트에서 직접 현재 상태값을 바꾸게 되어있다

	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

	DWORD dwUserIdx = TITLE->GetUserIdx();

	DWORD dwTargetIdx = pmsg->dwData1 ;
	BOOL  bDestroy = pmsg->dwData2 ;
	DWORD dwDecoPoint = pmsg->dwData3;

	if( m_CurHouse.HouseInfo.dwOwnerUserIndex ==  dwUserIdx )								//집주인 주인일경우  가구정보,아이콘,꾸미기포인트,보너스리스트,필드에서 해제
	{
		stFurniture* pFuniture = m_CurHouse.pFurnitureList.GetData(dwTargetIdx);
		if( ! pFuniture )
			return;

		pFuniture->wState = eHOUSEFURNITURE_STATE_UNINSTALL;								
		UnInstallFunitureFromField(dwTargetIdx);

		cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();	
		//090708 pdy RefreshIcon 수정 
		pDlg->RefreshIcon( pFuniture );	

		m_CurHouse.dwDecoPoint = dwDecoPoint;

		if( GAMEIN->GetHousingMainPointDlg() )
			GAMEIN->GetHousingMainPointDlg()->SetDecoPoint( m_CurHouse.dwDecoPoint );

		if( GAMEIN->GetHousingDecoPointDlg() )
			GAMEIN->GetHousingDecoPointDlg()->RefreshBonusList(m_CurHouse.dwDecoPoint,m_CurHouse.HouseInfo.dwDecoUsePoint);

		if( bDestroy )	//삭제하라는 정보가 참이면 가구를 삭제한다
		{
			stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(pFuniture->dwFurnitureIndex);
			if( stFurnitureInfo )
			{
				//090527 pdy 하우징 시스템메세지 [기간완료가구삭제]
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1861 ) , stFurnitureInfo->szName ); //1861	"%s이(가) 기간이 지나 삭제되었습니다"
			}

			DestroyFuniture(pFuniture);
		}
	}
	else				
	{
		UnInstallFunitureFromField(dwTargetIdx);						//가구주인이 아닌경우 필드에서 해제
	}
}

void cHousingMgr::House_UnInstall_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_UNINSTALL_NACK",pmsg->dwData);

	EndDecoration();
}

void cHousingMgr::House_Action_Force_GetOff_Ack(void* pMsg)
{
	// 같은 채널에 있는 유저에게통보
	// dwData1(가구Index), dwData2(Attach슬롯), dwData3(액션Index)
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
	DWORD dwFunitureID = pmsg->dwData1;
	DWORD dwActionIndex = pmsg->dwData3;

	CPlayer* pActionPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwObjectID);
	if( ! pActionPlayer )
		return;

	stHouseActionInfo* pActionInfo = GAMERESRCMNGR->GetHouseActionInfo(dwActionIndex);
	if( !pActionInfo)
		return;

	CFurniture* pActionFuniture = m_FieldFurnitureTable.GetData(dwFunitureID);		
	if( ! pActionFuniture ) 
		return;

	stFurniture* pstFurniture = NULL;
	if( IsHouseOwner() )
	{
		pstFurniture = m_CurHouse.pFurnitureList.GetData(dwFunitureID);
	}
	else
	{
		pstFurniture = pActionFuniture->GetFurnitureInfo();
	}

	if( ! pstFurniture) 
			return;

	stFunitureInfo* pstFunitureInfo = GAMERESRCMNGR->GetFunitureInfo(pstFurniture->dwFurnitureIndex);
	if( !pstFunitureInfo)
		return;

	RideOffPlayerFromFuniture(pActionPlayer, TRUE);
	APPEARANCEMGR->ShowWeapon( pActionPlayer ) ;
}

void cHousingMgr::House_Action_Force_GetOff_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_ACTION_FORCE_GETOFF_NACK",pmsg->dwData);
}

void cHousingMgr::House_Action_Ack(void* pMsg)
{
	// 같은 채널에 있는 유저에게통보
	// dwData1(가구Index), dwData2(Attach슬롯), dwData3(액션Index), dwData4(x위치), dwData5(z위치)
	MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;
	DWORD dwFunitureID = pmsg->dwData1;
	DWORD dwRideSlot = pmsg->dwData2;
	DWORD dwActionIndex = pmsg->dwData3;
	DWORD dwPosX = pmsg->dwData4;
	DWORD dwPosZ = pmsg->dwData5;

	CPlayer* pActionPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwObjectID);
	if( ! pActionPlayer )
		return;

	stHouseActionInfo* pActionInfo = GAMERESRCMNGR->GetHouseActionInfo(dwActionIndex);
	if( !pActionInfo)
		return;

	CFurniture* pActionFuniture = m_FieldFurnitureTable.GetData(dwFunitureID);		
	if( ! pActionFuniture ) 
		return;

	stFurniture* pstFurniture = NULL;
	if( IsHouseOwner() )
	{
		pstFurniture = m_CurHouse.pFurnitureList.GetData(dwFunitureID);
	}
	else
	{
		pstFurniture = pActionFuniture->GetFurnitureInfo();
	}

	if( ! pstFurniture) 
			return;

	stFunitureInfo* pstFunitureInfo = GAMERESRCMNGR->GetFunitureInfo(pstFurniture->dwFurnitureIndex);
	if( !pstFunitureInfo)
		return;

	if( pActionInfo->dwActionType != eHOUSE_ACTION_TYPE_RIDE && 
		pActionInfo->dwActionType != eHOUSE_ACTION_TYPE_GETOFF )
	{
		//탑승 관련 액션이 아니면 설정된 모션과 이팩트를 적용하여준다. 
		if( pActionInfo->dwPlayer_MotionIndex )
			pActionPlayer->ChangeMotion( pActionInfo->dwPlayer_MotionIndex ,FALSE);

		if( pActionInfo->dwPlayer_EffectIndex )
		{
			OBJECTEFFECTDESC desc( pActionInfo->dwPlayer_EffectIndex );				//플레이어에 이팩트가 있으면 적용 
			pActionPlayer->AddObjectEffect( pActionInfo->dwPlayer_EffectIndex , &desc, 1 );
		}

		if( pActionInfo->dwFurniture_MotionIndex )
			pActionFuniture->ChangeMotion( pActionInfo->dwFurniture_MotionIndex ,FALSE);		//가구에 모션이 있다면 적용 

		if( pActionInfo->dwFurniture_EffectIndex )
		{
			OBJECTEFFECTDESC desc( pActionInfo->dwFurniture_EffectIndex );			//가구에 이팩트가 있으면 적용 
			pActionFuniture->AddObjectEffect( pActionInfo->dwFurniture_EffectIndex , &desc, 1 );
		}
	}

	switch(pActionInfo->dwActionType)
	{
		case eHOUSE_ACTION_TYPE_RIDE:
			{
				pActionPlayer->SetRideFurnitureID(dwFunitureID);
				pActionPlayer->SetRideFurnitureSeatPos(dwRideSlot);

				RideOnPlayerToFuniture(pActionPlayer);

				VECTOR3 Position = {0,};
				Position.x = dwPosX;
				Position.z = dwPosZ;
				pActionPlayer->SetPosition(&Position);

				APPEARANCEMGR->HideWeapon( pActionPlayer ) ;
			}
			break;
		case eHOUSE_ACTION_TYPE_GETOFF:
			{
				RideOffPlayerFromFuniture(pActionPlayer);

				VECTOR3 Position = {0,};
				Position.x = dwPosX;
				Position.z = dwPosZ;
				pActionPlayer->SetPosition(&Position);

				APPEARANCEMGR->ShowWeapon( pActionPlayer ) ;
			}
			break;
		case eHOUSE_ACTION_TYPE_BUFF:
			{
				if( gHeroID == pActionPlayer->GetID() )
				{
					cActiveSkillInfo* info = ( cActiveSkillInfo* )SKILLMGR->GetSkillInfo( pActionInfo->dwActionValue );
					if(info)
					{
						ACTIVE_SKILL_INFO* skillInfo = info->GetSkillInfo();
						if(skillInfo)
						{
							//SKILLMGR->AddBuffSkill( *pPlayer, *skillInfo );
							CHATMGR->AddMsg( CTC_SYSMSG, "%s 버프 효과를 얻었습니다.", skillInfo->Name);
						}
					}
				}
			}
			break;
		// 091105 pdy 하우징 가구 액션추가 ( 미니홈피 웹 브라우저 링크 ) 
		case eHOUSE_ACTION_TYPE_OPEN_HOMEPAGE:
			{
				if( gHeroID ==  pActionPlayer->GetID() )
				{
					// 웹브라우저를 띄우자. 
					cHousingWebDlg* pDlg = GAMEIN->GetHousingWebDlg();

					if( pDlg )
					{
						pDlg->OpenMiniHomePage( GetCurHouseOwnerIndex() );
					}
				}
			}
			break;
	}
}

void cHousingMgr::House_Action_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_ACTION_NACK",pmsg->dwData);
}

void cHousingMgr::House_Bonus_Ack(void* pMsg)
{
	//꾸미기 보너스 사용시 

	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	if( IsHouseOwner() )
	{
		m_CurHouse.HouseInfo.dwDecoUsePoint =  pmsg->dwData2;

		if( GAMEIN->GetHousingMainPointDlg() )
			GAMEIN->GetHousingMainPointDlg()->SetDecoPoint( m_CurHouse.dwDecoPoint );

		if( GAMEIN->GetHousingDecoPointDlg() )
			GAMEIN->GetHousingDecoPointDlg()->RefreshBonusList( m_CurHouse.dwDecoPoint ,m_CurHouse.HouseInfo.dwDecoUsePoint);
	}

	stHouseBonusInfo* pBonusInfo = GAMERESRCMNGR->GetHouseBonusInfo(pmsg->dwData1);

	if( ! pBonusInfo )
		return;

	//091020 pdy 꾸미기 보너스 사용 메세지 집주인에게만 띄우도록 변경
	if( IsHouseOwner() )
	{
		switch(pBonusInfo->dwBonusType)
		{
			case 0 :	//버프타입 
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1871 ) , pBonusInfo->szName); //%s의 효과를 얻었습니다. 
				}
				break;
		}
	}
}

void cHousingMgr::House_Bonus_Nack(void* pMsg)
{
	//꾸미기 보너스를 사용하지 못하였다.
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	char buf[128] = {0,};
	sprintf(buf,"MP_HOUSE_BONUS_NACK BonusIndex : %d",pmsg->dwData1);
	PrintDebugErrorMsg(buf,pmsg->dwData2);
}
void cHousingMgr::House_Update_Ack(void* pMsg)
{
	//가구의 설정이 업데이트되었다. (위치,회전,머터리얼 등)
	MSG_HOUSE_FURNITURE* pmsg = (MSG_HOUSE_FURNITURE*)pMsg;
	DWORD dwUserIdx = TITLE->GetUserIdx();
	if( pmsg->Furniture.dwOwnerUserIndex ==  dwUserIdx )								
	{
		stFurniture* pFuniture = m_CurHouse.pFurnitureList.GetData(pmsg->Furniture.dwObjectIndex);
		if( pFuniture )
			*pFuniture = pmsg->Furniture;		//가구주인인 경우 가구정보를 갱신해 준다.
	}

	UpdateFunitere( &pmsg->Furniture ) ;
}
void cHousingMgr::House_Update_Nack(void* pMsg)
{
	//가구 업데이트 실패
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_UPDATE_NACK",pmsg->dwData);
}

void cHousingMgr::House_Update_Material_Ack(void* pMsg)
{
	MSG_DWORD2*	 pmsg = (MSG_DWORD2*)pMsg;
	DWORD	dwFnObjdx = pmsg->dwData1 ;
	int		nMeterial =	(int) pmsg->dwData2 ;

	CFurniture* pFuniture = m_FieldFurnitureTable.GetData(dwFnObjdx);	
	if(! pFuniture) 
		return ;

	pFuniture->GetFurnitureInfo()->nMaterialIndex = nMeterial ;

	pFuniture->GetEngineObject()->SetMaterialIndex(nMeterial);

	// 집주인일 경우 가구정보도 바꿔 주자
	if( IsHouseOwner( ) )
	{
		stFurniture* pFuniture = m_CurHouse.pFurnitureList.GetData(dwFnObjdx);
		if( pFuniture )
		{
			pFuniture->nMaterialIndex = nMeterial ;
		}
	}
}

void cHousingMgr::House_Destroy_Ack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	stFurniture* pFuniture = m_CurHouse.pFurnitureList.GetData(pmsg->dwData);

	if( !pFuniture )
		return;

	DestroyFuniture(pFuniture);
}
void cHousingMgr::House_Destroy_Nack(void* pMsg)
{
	//가구삭제 실패
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_DESTROY_NACK",pmsg->dwData);
}

void cHousingMgr::House_Vote_Ack(void* pMsg)
{
	//추천 성공
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1885 )); //추천하였습니다.
}

void cHousingMgr::House_Vote_Nack(void* pMsg)
{
	//추천 실패 
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	//090527 pdy 하우징 시스템메세지 서버NACK [추천실패]  
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1886 )); //1886	"추천하지못했습니다."
	PrintDebugErrorMsg("MP_HOUSE_VOTE_NACK",pmsg->dwData);
}

void cHousingMgr::House_Exit_Nack(void* pMsg)
{
	//집에서 나가기 실패 
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_EXIT_NACK",pmsg->dwData);

	//090618 pdy 하우징 나가기 액션 버그 수정 
	OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
}
// 하우스관련 통보
void cHousingMgr::House_Notify_Visit(void* pMsg)
{
	//플레이어 방문시 
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	m_CurHouse.dwJoinPlayerNum = pmsg->dwData;

	if( GAMEIN->GetHousingMainPointDlg() )	
			GAMEIN->GetHousingMainPointDlg()->SetVisitCount( m_CurHouse.dwJoinPlayerNum );
}
void cHousingMgr::House_Notify_Exit(void* pMsg)
{
	//플레이어 나갈시
}
void cHousingMgr::House_Notify_Action(void* pMsg)
{
	//플레이어 액션시
}

void cHousingMgr::House_Extend(void* pMsg)
{
	MSG_HOUSE_EXTEND* pmsg = (MSG_HOUSE_EXTEND*)pMsg;

	if( IsHouseOwner() )
	{
		m_CurHouse.dwDecoPoint =  pmsg->dwDecoPoint;
		m_CurHouse.HouseInfo.ExtendLevel = pmsg->wExtendLevel;

		if( GAMEIN->GetHousingMainPointDlg() )	
			GAMEIN->GetHousingMainPointDlg()->SetDecoPoint( m_CurHouse.dwDecoPoint );

		if( GAMEIN->GetHousingDecoPointDlg() )	
			GAMEIN->GetHousingDecoPointDlg()->RefreshBonusList( m_CurHouse.dwDecoPoint ,m_CurHouse.HouseInfo.dwDecoUsePoint);
	}

	//확장이 날라왔으면 기본설치 품목을 일단 다 Destroy시킨다.
	m_CurHouse.pNotDeleteFurnitureList.SetPositionHead();
	while(stFurniture* pstFurniture= m_CurHouse.pNotDeleteFurnitureList.GetData())
	{
		UnInstallFunitureFromField(pstFurniture->dwObjectIndex);
		if( IsHouseOwner() )	
		{
			DestroyFuniture(pstFurniture);
		}
	}
	m_CurHouse.pNotDeleteFurnitureList.RemoveAll();

	//서버에서 언인스톨을 요청한것들을 다 언인스톨시킨다.
	for(int i=0 ; i < pmsg->wUnInstallNum ; i++)
	{
		stFurniture* pstFurniture = NULL;
		if( IsHouseOwner() )					//가구 주인일경우 
		{
			pstFurniture = m_CurHouse.pFurnitureList.GetData(pmsg->UnInstall[i].dwObjectIndex);
			*pstFurniture = pmsg->UnInstall[i]; //가구 정보 카피 
			pstFurniture->wState = eHOUSEFURNITURE_STATE_UNINSTALL; //언인스톨 상태 

			cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();				
			//090708 pdy RefreshIcon 수정 
			pDlg->RefreshIcon( pstFurniture );
		}
		else
		{
			CFurniture* pFuniture = m_FieldFurnitureTable.GetData(pmsg->UnInstall[i].dwObjectIndex);	
			if( pFuniture)
				pstFurniture = pFuniture ->GetFurnitureInfo();
		}

		if(!pstFurniture)
			continue;

		UnInstallFunitureFromField(pstFurniture->dwObjectIndex);
	}

	//서버에서 인스톨을 요청한 것들을 다 인스톨 시킨다.
	for(int i=0 ; i < pmsg->wInstallNum ; i++)
	{
		m_CurHouse.m_dwFurnitureList[pmsg->Install[i].wCategory][pmsg->Install[i].wSlot] = pmsg->Install[i].dwObjectIndex;

		InstallFunitureToField( &pmsg->Install[i] , TRUE );

		if( IsHouseOwner() )	//집주인이면 퍼니쳐리스트에 넣자 (창고에 보유 가구리스트)
		{
			stFurniture* pNewFuniture = m_stFurniturePool.Alloc();
			*pNewFuniture = pmsg->Install[i];
			m_CurHouse.pFurnitureList.Add( pNewFuniture, pNewFuniture->dwObjectIndex );

			if( pmsg->Install[i].bNotDelete  )
			{
				m_CurHouse.pNotDeleteFurnitureList.Add(pNewFuniture , pNewFuniture->dwObjectIndex);
			}

			cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();				

			pDlg->AddIcon( pNewFuniture );													//창고Dlg에 아이콘을 Add 
			m_CurHouse.m_dwCategoryNum[pmsg->Install[i].wCategory]++;						//카테고리에 보유갯수 ++ 
		}
		else if( pmsg->Install[i].bNotDelete )
		{
			CFurniture*  pFurniture = m_FieldFurnitureTable.GetData(pmsg->Install[i].dwObjectIndex);		
			if(! pFurniture )
				continue;

			//기본설치 품목은 집주인이 아니어도 가지고 있자. 
			m_CurHouse.pNotDeleteFurnitureList.Add(pFurniture->GetFurnitureInfo() , pFurniture->GetFurnitureInfo()->dwObjectIndex);
		}
	}	

	//090608 pdy 마킹관련 버그 수정
	CheckAllMarkingObjByHeroZone();		//필드에 설치된 오브젝트중 히어로 위치에 마킹된 오브젝트를 검출한다.

	//090527 pdy 하우징 시스템메세지 [하우스확장] 
	CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1865 ) , pmsg->wExtendLevel+1  );//1865	"집이 %d 단계로 확장되었습니다."
}

void cHousingMgr::House_UseItem_Nack(void* pMsg)
{
	// 인벤에서 사용가능한 하우징아이템 사용실패 처리  
	MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
	DWORD dwItemPos = pmsg->dwData2;
	DWORD dwSupplyType = pmsg->dwData3;

	switch(dwSupplyType)
	{
		case ITEM_KIND_FURNITURE_WALLPAPER:
		case ITEM_KIND_FURNITURE_FLOORPAPER:
		case ITEM_KIND_FURNITURE_CEILINGPAPER:
			{
				ChangeMaterialFurniture( pmsg->dwData1 , 0 ,/*RollBack*/ 1 );
			}
			break;
	}

	CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForPos(dwItemPos);
	if( pItem )
		pItem->SetLock(FALSE);

	PrintDebugErrorMsg("MP_HOUSE_USEITEM_NACK",pmsg->dwData4);
}
void cHousingMgr::House_Cheat_GetInfoAll_Ack(void* pMsg)
{
	//맵서버의 모든 하우스정보 가날라왔으니 띄워주자 
	MSG_HOUSE_CHEATINFO_ALL* pmsg = (MSG_HOUSE_CHEATINFO_ALL*)pMsg;

	char Buf[256] = {0,};

	CHATMGR->AddMsg( CTC_SYSMSG, "---------CHEATINFO_ONEINFO--------");
	sprintf(Buf,"HouseNum: %d UserNum: %d ReservationNum: %d" ,pmsg->dwHouseNum ,pmsg->dwUserNum , pmsg->dwReservationNum );
	CHATMGR->AddMsg( CTC_SYSMSG, Buf);

	CHATMGR->AddMsg( CTC_SYSMSG, "---------CHEATINFO_INFOALL--------");
	for(DWORD i=0; i < pmsg->dwHouseNum ; i++ )
	{
		sprintf(Buf,"HouseName: %s" ,pmsg->HouseInfo[i].szHouseName);
		CHATMGR->AddMsg( CTC_SYSMSG, Buf);

		sprintf(Buf,"ChannelID: %d OwnerUserIdx: %d VisiterNum: %d" 
			,pmsg->HouseInfo[i].dwChannelID,pmsg->HouseInfo[i].dwOwnerUserIndex,pmsg->HouseInfo[i].dwVisiterNum);
			CHATMGR->AddMsg( CTC_SYSMSG, Buf);

	}
}
void cHousingMgr::House_Cheat_GetInfoOne_Ack(void* pMsg)
{
	//맵서버의 하우스정보 가날라왔으니 띄워주자 
	MSG_HOUSE_CHEATINFO_ONE* pmsg = (MSG_HOUSE_CHEATINFO_ONE*)pMsg;

	char Buf[256] = {0,};

	CHATMGR->AddMsg( CTC_SYSMSG, "---------CHEATINFO_ONEINFO--------");
	sprintf(Buf,"Name: %s, Ch: %d OwnerUserIdx: %d Visiter: %d",
			pmsg->HouseInfo.szHouseName, pmsg->HouseInfo.dwChannelID, pmsg->HouseInfo.dwOwnerUserIndex, pmsg->HouseInfo.dwVisiterNum);
			CHATMGR->AddMsg( CTC_SYSMSG, Buf);

	if(strlen(pmsg->HouseInfo.szOwnerName) > 2)
	{
		sprintf(Buf, "HouseOwner [%s] in HousingMap", pmsg->HouseInfo.szOwnerName);
		CHATMGR->AddMsg( CTC_SYSMSG, Buf);
	}

	for(int i=1; i<MAX_HOUSING_CATEGORY_NUM; i++)
	{
		sprintf(Buf, "Slot[%d] : (%d / %d)", i, pmsg->HouseInfo.m_dwInstalledNum[i], pmsg->HouseInfo.m_dwFurnitureNum[i]);
		CHATMGR->AddMsg( CTC_SYSMSG, Buf);
	}
}

void cHousingMgr::House_Cheat_Delete_Ack(void* pMsg)
{
	CHATMGR->AddMsg( CTC_SYSMSG, "House_Cheat_Delete OK");
}

void cHousingMgr::House_Cheat_Delete_Nack(void* pMsg)
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	PrintDebugErrorMsg("MP_HOUSE_CHEAT_DELETE_NACK",pmsg->dwData);
}


void cHousingMgr::Process()
{
	if(! IsHouseInfoLoadOk() )			
		return ;


	// 사용중인 가구를 시간을 1분에 한번. 
	// 1분에 한번씩 시간소모시킬것.
	ProcessRemainTime();

	if( m_bShowTest )
		ShowAllMarkingTest();

	//090323 pdy 하우징 설치 UI추가 
	if( m_bDecoAngle )
	{
		CHousingRevolDlg* pRevolDlg = GAMEIN->GetHousingRevolDlg();

		if( ! pRevolDlg & ! m_pDecoObj )
			return;

		float fAngle = pRevolDlg->GetRotateAngleFromBarRate();

		m_pDecoObj->SetAngle( fAngle );
	}

	ProcessCulling();

	ProcessAlpha();

	ProcessWaitForMaikingObjList();
}

//090406 pdy 벽,벽장식 컬링 
void cHousingMgr::ProcessCulling()
{
	static DWORD dwLastCheckTime = 0;
	DWORD ElapsedTime = gCurTime - dwLastCheckTime;

	if( ElapsedTime < 50 )		//실시간은 좀그렇다 
		return;

	dwLastCheckTime = gCurTime;

	if( m_dwFieldFurnitureNum != 0)
	{
		MHCAMERADESC* pCurCamera = CAMERA->GetCameraDesc() ;

		m_FieldFurnitureTable.SetPositionHead();
		
		for(CFurniture* pFieldObj = m_FieldFurnitureTable.GetData();
			0 < pFieldObj;
			pFieldObj = m_FieldFurnitureTable.GetData())
		{
			DWORD dwIdx = pFieldObj->GetFurnitureInfo()->dwFurnitureIndex;

			if( m_pDecoObj && pFieldObj == m_pDecoObj )
				continue;

			//벽장식이나 외벽이 아닐경운 필요없음 
			if( ! ( Get_HighCategory(dwIdx) ==  eHOUSE_HighCategory_Door ||  IsExteriorWall(dwIdx) ) ) 
				continue;

			VECTOR3 vDirz = {0,};
			float fAngle = pFieldObj->GetFurnitureInfo()->fAngle;

			//Y축회전만 가정하면 오브젝트의 Z축은 다음과같다 (바라보는 방향)

			//Max의 Front에 맞춰서 모델링을 했다면 Dirz(회전값이 0도일때) = 0 , 0 . -1 이 된다.

			//그러므로 x = Sinf 가되고 z = -Cosf가 된다. 

			vDirz.x = sinf(fAngle);
			vDirz.z = - cosf(fAngle);
			Normalize(&vDirz,&vDirz);

			VECTOR3 vObjPos;
			pFieldObj->GetPosition( &vObjPos );

			VECTOR3 vCamera2Obj = vObjPos - pCurCamera->m_CameraPos;

			float fDot = vDirz * vCamera2Obj ;

			if( fDot > 0.0f )
			{
				//상대백터(카메라->오브젝트)와 Obj의 Z축의 내적값이 양수일경우에 Hide를 시켜주자 
				pFieldObj->GetEngineObject()->HideWithScheduling() ;
			}
			else
			{
				//음수일경운 Show 
				pFieldObj->GetEngineObject()->Show();
			}
		}
	}
}

//090406 pdy 하우징 가구 알파프로세스
void cHousingMgr::ProcessAlpha()
{
	//필드에 위치한 벽걸이용을 제외한 오브젝트를 가지고 
	//카메라에 방향성에 내적시킨 거리 - 반지름 가 HERO와 카메라의 거리보다 가깝고 
	//높이채크를 제외하고 XZ레이로 바운딩 박스에 교차하느냐로 알파리스트에 ADD 한다. 

	if( ! HERO )
		return;

	static DWORD dwLastCheckTime = 0;
	DWORD ElapsedTime = gCurTime - dwLastCheckTime;
	DWORD interval = ElapsedTime/5;

	if( ElapsedTime >= 50 )	//---너무 자주 호출 되는 것을 방지
	{
		//////////////////////////////////
		//알파적용할 오브젝트 순회 시작 //
		//////////////////////////////////
		MHCAMERADESC* pCurCamera = CAMERA->GetCameraDesc() ;

		m_FieldFurnitureTable.SetPositionHead();

		for(CFurniture* pFieldObj = m_FieldFurnitureTable.GetData();
			0 < pFieldObj;
			pFieldObj = m_FieldFurnitureTable.GetData())
		{
			DWORD dwIdx = pFieldObj->GetFurnitureInfo()->dwFurnitureIndex;

			if( pFieldObj->GetID() == HERO->GetRideFurnitureID() )	//HERO가 탑승한 가구는 Alpha가 필요없다.
				continue;	

			//090615 pdy 하우징 알파버그 수정
			if( m_bDoDecoration && pFieldObj == m_pDecoObj )	//셋팅중인 가구라면 
				continue;	

			//벽장식이나 외벽일 경운 알파 필요없음 
			if( Get_HighCategory(dwIdx) ==  eHOUSE_HighCategory_Door ||  IsExteriorWall(dwIdx)  ) 
					continue;

			CEngineObject* pEngineObj = pFieldObj->GetEngineObject();

			if( ! pEngineObj)	//이럴경운 곤란하다..
				continue;

			GXOBJECT_HANDLE GxObjHandle = pEngineObj->GetGXOHandle();

			if( !GxObjHandle )		//설마..하지만 일단 예외처리
				continue;

			VECTOR3 vObjPos;													
			pFieldObj->GetPosition( &vObjPos );

			VECTOR3 vCamera2Obj = vObjPos - pCurCamera->m_CameraPos;			//카메라->오브젝트 상대백터 
			float fCamera2FieldObj = VECTOR3Length(&vCamera2Obj);				//카메라->오브젝트 거리

			VECTOR3 vHeroPos;				
			HERO->GetPosition( &vHeroPos );										//히어로위치를 가져온다.
			vHeroPos.y += 100.0f;												//허리높이쯤으로 높이를 올려준다.

			VECTOR3 vCamera2Hero = vHeroPos - pCurCamera->m_CameraPos;			//카메라->히어로 상대백터 
			float fCamera2HeroLength = VECTOR3Length(&vCamera2Hero);			//카메라->히어로까지의 거리 

			COLLISION_MESH_OBJECT_DESC FieldObjDesc;				
			g_pExecutive->GXOGetCollisionMesh(pFieldObj->GetEngineObject()->GetGXOHandle(), &FieldObjDesc);	

			float fFieldObjRadius = FieldObjDesc.boundingSphere.fRs ;			//오브젝트의 반지름 

			if( fCamera2HeroLength <  ( fCamera2FieldObj - fFieldObjRadius ) )	// (카메라->히어로)거리가 (카메라->FieldObj)-반지름 보다 크면 continue;
				continue;														

			VECTOR3 vDir = vCamera2Hero;										
			Normalize(&vDir,&vDir);

			VECTOR3 vPos = pCurCamera->m_CameraPos;								
			float fDist = 0.0f;
			VECTOR3 vIntetSectPoint = {0};


			if(! IsCollisionMeshAndRay(&vIntetSectPoint,&fDist,&FieldObjDesc,&vPos,&vDir) )	
			{
				continue;														// Ray(카메라->히어로)를 오브젝트에 바운딩박스에 쏘아서 교차가 안된다면 continue 
			}
			else if(fDist > fCamera2HeroLength )
			{
				continue;														// 교차는되었으나 히어로보다 멀다면 continue 
			}


			//알파리스트에 담자

			int setAlpha = g_pExecutive->GetAlphaFlag(GxObjHandle) - (interval*2);
			if( setAlpha < 100 )
			{
				setAlpha = 100;	//--- 최소 알파수치 = 0
			}

			g_pExecutive->SetAlphaFlag(GxObjHandle,setAlpha);

			if( m_AlphaHandleList.Find(GxObjHandle) == NULL )	//---리스트에 없다면 리스트에 추가.
			{
				//090615 pdy 하우징 알파버그 수정
				stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( pFieldObj->GetFurnitureInfo()->dwFurnitureIndex );
				if( stFurnitureInfo )
					g_pExecutive->GXOSetZOrder( pFieldObj->GetEngineObject()->GetGXOHandle() , 8 );			//알파소팅문제로 가구에 알파가 들어가면 뒤에서 그려줘야한다.

				m_AlphaHandleList.AddTail(GxObjHandle);
			}
		}
		/////////////////////////////////////////////////////////////////////////

		//---리스트의 모든 오브젝트는 알파를 더해주어 원래 상태로 돌아가게 해준다.
		//---위에서 interval*2 로 알파를 두배로 빼주는 이유는, 여기서 interval만큼 더해주기 때문이다.
		PTRLISTPOS pos = m_AlphaHandleList.GetHeadPosition();
		while(pos)
		{
			PTRLISTPOS oldPos = pos;
			GXOBJECT_HANDLE gxHandle = (GXOBJECT_HANDLE)m_AlphaHandleList.GetNext(pos);

			if( gxHandle == NULL )
			{
				m_AlphaHandleList.RemoveAt( oldPos );
				continue;
			}

			if(g_pExecutive->IsValidHandle(gxHandle) == GX_MAP_OBJECT_TYPE_INVALID)
			{
				m_AlphaHandleList.RemoveAt( oldPos );
				continue;
			}

			DWORD curAlpha = g_pExecutive->GetAlphaFlag(gxHandle);
			DWORD setAlpha = curAlpha + interval;
			if( setAlpha >= 255 )
			{
				CEngineObject* pEngineObj = (CEngineObject*)g_pExecutive->GetData( gxHandle );
				CObjectBase* pBaseObj = NULL;

				//090615 pdy 하우징 알파버그 수정
				if( pEngineObj && pEngineObj->GetBaseObject() )
				{
					pBaseObj = pEngineObj->GetBaseObject();
					DWORD dwFunuitureIdx = ((CFurniture*)pBaseObj)->GetFurnitureInfo()->dwFurnitureIndex;

					stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(dwFunuitureIdx);
					if( stFurnitureInfo && stFurnitureInfo->bHasAlpha == FALSE )							//자체알파가 없는 가구는 
						g_pExecutive->GXOSetZOrder( gxHandle , 0 );											//알파가 끝났다면 Zorder를 0으로  
				}

				setAlpha = 255;
				m_AlphaHandleList.RemoveAt( oldPos );
			}
			g_pExecutive->SetAlphaFlag( gxHandle, setAlpha );			
		}

		dwLastCheckTime = gCurTime;
	}
}

void cHousingMgr::ProcessWaitForMaikingObjList()
{
	static DWORD dwLastCheckTime = 0;
	DWORD ElapsedTime = gCurTime - dwLastCheckTime;

	if( ElapsedTime < 1000 )	//1초에 한번씩 하자 
		return;

	dwLastCheckTime = gCurTime;

	PTRLISTPOS pos = m_WaitForMakingObjList.GetHeadPosition();
	while( pos )
	{
		CObject* pCurObject= (CObject*)m_WaitForMakingObjList.GetNext( pos ) ;

		if( CanMakingToFiled(pCurObject) )			//필드에 마킹이 현재 가능하면 
		{
			MarkingObjZone( pCurObject , TRUE ) ;	//필드에 마킹을 하고
			RemoveWaitForMakingObj(pCurObject);		//리스트에서 지워준다 
		}								
	}
}

void cHousingMgr::ProcessRemainTime()
{
	//가구 남은 시간 감소 시키기

	if( m_dwLastRemainProcessTime > gCurTime )
		m_dwLastRemainProcessTime = gCurTime;

	DWORD dwDeltaSecond = (gCurTime - m_dwLastRemainProcessTime) /1000;

	if( dwDeltaSecond < 60 )
		return;

	m_dwLastRemainProcessTime = gCurTime;

	for(int i = 0 ; i<MAX_HOUSING_CATEGORY_NUM; i++)			//모든 가구정보리스트를 돌면서 시간값을 60초씩 감소 시킨다 
	{
		if( m_CurHouse.m_dwCategoryNum[i] == 0)
			continue;

		for( int j = 0 ; j < MAX_HOUSING_SLOT_NUM ; j++ )
		{
			DWORD dwObjectIndex = m_CurHouse.m_dwFurnitureList[i][j];

			if( dwObjectIndex )
			{
				stFurniture* pstFurniture =  m_CurHouse.pFurnitureList.GetData(dwObjectIndex);

				if( !pstFurniture )
					continue;

				if( pstFurniture->wState == eHOUSEFURNITURE_STATE_KEEP )	//보관중인가구는 시간이 감소 하지 않는다. ( 한번도 인스톨 되지 않은상태 )
					continue;

				if( pstFurniture->dwRemainTime < dwDeltaSecond )
				{
					pstFurniture->dwRemainTime = 0;
				}
				else 
				{
					pstFurniture->dwRemainTime -= dwDeltaSecond;
				}
			}
		}
	}
}

//꾸미기 모드일때는 마우스 이벤트를 매니져에서 처리하자
void cHousingMgr::ProcessMouseEvent_DecorationMode(CMouse* Mouse,DWORD MouseEvent)
{
	if( ! m_bDecorationMode )
		return;

	//위치 설정 중일때  
	if( m_bDecoMove )
	{
		if( MouseEvent == MOUSEEVENT_NONE )
		{
			//마우스이벤트가 없으면 픽킹포지션으로 
			MoveDecorationMousePoint(Mouse);
			CURSOR->SetCursor( eCURSOR_DEFAULT );
		}
		else if(  MouseEvent & MOUSEEVENT_LCLICK )
		{
			if ( CanInstallFunitureToField() )
			{
				//설치가능한 상황 처리
				EndDecoMove();
				StartDecoAngle();
			}
			else
			{
				//설치불가능한 상황처리
				CURSOR->SetCursor(eCURSOR_HOUSING_CANNOT_INSTALL);

				//090527 pdy 하우징 시스템메세지 [설치 불가능]
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1860 ) );	//1860	"이곳에 해당 가구를 배치 할 수 없습니다."
			}
		}
	}
	else if( MouseEvent & MOUSEEVENT_LCLICK ) 	//위치 설정 중이 아닐때 
	{
		if( ! m_bDoDecoration )					//오브젝트 설정중이 아니라면 
		{
			PICK_GXOBJECT_DESC gxoDesc[5] = {0,};
			GetSelectedObject( gxoDesc, Mouse->GetMouseX(), Mouse->GetMouseY(), PICK_TYPE_PER_FACE , TRUE );

			CObject* pPickObject	= NULL ;
			m_PickAnotherObjHandle  = NULL ;

			for( int i=0; i < 5 ; i++ )
			{
				if(! gxoDesc[i].gxo || ! (g_pExecutive->GetData(gxoDesc[i].gxo) ) )
					continue;

				CObjectBase* pBaseObj = ( (CEngineObject*)g_pExecutive->GetData(gxoDesc[i].gxo) )->GetBaseObject();
				if( ! pBaseObj || pBaseObj->GetEngineObjectType() != eEngineObjectType_Furniture )	
				{
					//가구가 아니면 픽킹되지 않게..
					gxoDesc[i].gxo = NULL;
					continue;
				}

				if( ! pPickObject )					
				{
					pPickObject = (CObject*) pBaseObj;
				}
				else if( ! m_PickAnotherObjHandle )
				{
					m_PickAnotherObjHandle = gxoDesc[i].gxo;
					break;
				}
			}

			//가장 가까운 오브젝트를 가져온다 
			//pPickObject = (CObject*)GetSelectedObject( Mouse->GetMouseX(), Mouse->GetMouseY(), PICK_TYPE_PER_FACE , /*bSort = TRUE */ TRUE );
			if( pPickObject )
			{
				//091214 pdy 꾸미기 모드시 바닥에 클릭했을 경우에 이동불가 메세지 띄운다.
				if( IsFloor ( ((CFurniture*) pPickObject)->GetFurnitureInfo()->dwFurnitureIndex ) )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2007 ) );
				}

				//아니면 가구리셋을 시작 
				if( FALSE == StartDecoReset(pPickObject) )
				{
					//리셋이 안되는 가구 처리 
				}
			}
			else
			{
				// 픽킹이 안되었다면 외부에 클릭한 상태 이동불가 메세지 띄운다.
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2007 ) );
			}
		}
	}
}

//픽업된 Obj 이동처리 
void cHousingMgr::MoveDecorationMousePoint(CMouse* Mouse)
{
	if( !m_pDecoObj )	//픽업인데 데코가 없다?
			return;

	PICK_GXOBJECT_DESC gxoDesc[5] = {0,};
	GetSelectedObject( gxoDesc, Mouse->GetMouseX(), Mouse->GetMouseY(), PICK_TYPE_PER_FACE );

	//가장 가까운 삼각형을 구한다.
	float fDist = 100000.0f;	//임시로 정한 거리100000.0f을 넘을리는 없겠지..
	int	  nIndex = 0;
	m_PickAnotherObjHandle = NULL ;
	DWORD Count = 0;

	for(int i=0;i<5;i++)
	{
		if(! gxoDesc[i].gxo || 
			gxoDesc[i].gxo == m_pDecoObj->GetEngineObject()->GetGXOHandle() ||		//설치 Obj는 제외한다
			! (g_pExecutive->GetData(gxoDesc[i].gxo) ) )
		{
			//맵오브젝트는 엔진오브젝트가 없다 걸러내자..
			gxoDesc[i].gxo = NULL;
			continue;
		}

		CObjectBase* pBaseObj = ( (CEngineObject*)g_pExecutive->GetData(gxoDesc[i].gxo) )->GetBaseObject();
		if( ! pBaseObj || pBaseObj->GetEngineObjectType() != eEngineObjectType_Furniture )	
		{
			//가구가 아니면 픽킹되지 않게..
			gxoDesc[i].gxo = NULL;
			continue;
		}

		if(fDist > gxoDesc[i].fDist)
		{
			fDist = gxoDesc[i].fDist;
			m_PickAnotherObjHandle = gxoDesc[i].gxo;	
			nIndex = i;
			Count++;
		}
	}

	DWORD dwDecoIdx = m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex;

	if(  Get_HighCategory(dwDecoIdx) ==  eHOUSE_HighCategory_Door ) 
	{
		//벽장식용일경우 
		if( m_PickAnotherObjHandle )
		{
			float fDist = 100000.0f;
			int	  nIndex = 0;

			CEngineObject* pPickWallObj = NULL;
			m_PickAnotherObjHandle = NULL;
			for(int i=0;i<5;i++)
			{
				if(! gxoDesc[i].gxo  ) 
					continue;

				//최초 피킹처리 루프에서 이미 예외처리 했으니 예외처리필요없다.. 
				CEngineObject* pCurEngineObj = (CEngineObject*)g_pExecutive->GetData(gxoDesc[i].gxo);
				CObjectBase* pBaseObj = pCurEngineObj->GetBaseObject();

				DWORD dwIdx = ( (CFurniture*)pBaseObj )->GetFurnitureInfo()->dwFurnitureIndex;

				if( IsExteriorWall(dwIdx) )
				{
					if(fDist > gxoDesc[i].fDist)
					{
						fDist = gxoDesc[i].fDist;
						m_PickAnotherObjHandle = gxoDesc[i].gxo;	
						pPickWallObj = pCurEngineObj;
						nIndex = i;
					}
				}
			}

			//벽면이 픽킹되었다면 벽의 회전값을 적용시켜주자 
			if( pPickWallObj )
			{
				VECTOR3 vIntersectPos;
				VECTOR3 vColTri[3];
				POINT pt = {Mouse->GetMouseX(), Mouse->GetMouseY()};

				if(g_pExecutive->GXOIsCollisionBoungingBoxWithScreenCoord(m_PickAnotherObjHandle, &vIntersectPos, &vColTri[0], &pt, 0))
				{
					VECTOR3 edge1,edge2;
					VECTOR3_SUB_VECTOR3(&edge1, &vColTri[1], &vColTri[0]);
					VECTOR3_SUB_VECTOR3(&edge2, &vColTri[2], &vColTri[0]);

					VECTOR3 normal;
					CrossProduct(&normal, &edge1, &edge2 );
					Normalize(&normal, &normal);

					if( normal.y == 0 )
					{
						float fAngle = pPickWallObj->GetEngObjAngle();
						m_vDecoPos = gxoDesc[nIndex].v3IntersectPoint;
						m_pDecoObj->SetAngle(fAngle);
						m_pDecoObj->GetEngineObject()->ApplyHeightField(FALSE);
						m_pDecoObj->SetPosition(&m_vDecoPos);
						m_pDecoObj->GetEngineObject()->ApplyHeightField(TRUE);
					}

				}
			}
		}
	}
	else if( m_PickAnotherObjHandle )	
	{
		//벽장식이 아닌 기본 오브젝트 
		//최초 피킹처리 루프에서 이미 예외처리 했으니 여기선 예외처리필요없다.. 
		CEngineObject* pCurEngineObj = (CEngineObject*)g_pExecutive->GetData(m_PickAnotherObjHandle);
		CObjectBase* pBaseObj = pCurEngineObj->GetBaseObject();

		DWORD dwIdx = ( (CFurniture*)pBaseObj )->GetFurnitureInfo()->dwFurnitureIndex;

		//090603 pdy 하우징 기본오브젝트 피킹이동은 피킹된 오브젝트가 벽장식 or 외벽이 아닐때만 이동가능
		if(! IsExteriorWall(dwIdx) &&  Get_HighCategory(dwIdx) !=  eHOUSE_HighCategory_Door )		
		{
			m_vDecoPos = gxoDesc[nIndex].v3IntersectPoint;

			if( IsFloor(dwIdx) )			//바닥일경우 필드의 높이로 맞추자 
			{
				float fFieldHeight = 0.0f;
				g_pExecutive->GXMGetHFieldHeight(&fFieldHeight,m_vDecoPos.x,m_vDecoPos.z);
				m_vDecoPos.y = fFieldHeight;
			}
			else
			{
				m_vDecoPos.y+= 0.1f;			//바닥이 아닐경우 0.1 정도올려주자 
			}

			m_pDecoObj->GetEngineObject()->ApplyHeightField(FALSE);
			m_pDecoObj->SetPosition(&m_vDecoPos);
			m_pDecoObj->GetEngineObject()->ApplyHeightField(TRUE);
		}
	}
	else
	{
		//픽킹된 가구 핸들이 없다면 하우스 밖이므로 이동시키지 않는다.
	}
}

BOOL cHousingMgr::RequestInstallCurDeco()
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	//필드에서 현재 선택된 가구를 추가하도록 요청
	if( !m_pDecoObj )
		return FALSE;

	if(! CanInstallFunitureToField() )
	{
		//설치 불가능한 상황 처리
		//090527 pdy 하우징 시스템메세지 [설치 불가능]
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1860 ) );	//1860	"이곳에 해당 가구를 배치 할 수 없습니다."
		return FALSE;
	}

	stFurniture* pstFuniture = m_pDecoObj->GetFurnitureInfo();


	WORD wCategory = Get_HighCategory(pstFuniture->dwFurnitureIndex);
	WORD wSlot = pstFuniture->wSlot;
	DWORD dwObjectIdx = m_CurHouse.m_dwFurnitureList[wCategory][wSlot];

	MSG_HOUSE_FURNITURE_INSTALL msg;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category	    = MP_HOUSE;
	msg.Protocol		= MP_HOUSE_INSTALL_SYN;
	msg.dwObjectID		= HEROID;
	msg.dwChannel		= m_CurHouse.dwChannelID;
	msg.dwFurnitureObjectIndex = dwObjectIdx;
	msg.fAngle			= m_pDecoObj->GetAngle();
	msg.vPos			= m_vDecoPos;
	msg.wSlot			= wSlot;

	NETWORK->Send(&msg,sizeof(msg));

	EndDecoAngle();
	return TRUE;
}

BOOL cHousingMgr::RequestUninstallCurDeco()
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	//필드에서 현재 선택된 가구를 삭제하도록 요청
	if( !m_pDecoObj )
		return FALSE;

	//090611 pdy 하우징 기본품목과 문류는 모두 설치해제 불가능 
	if( m_pDecoObj->GetFurnitureInfo()->bNotDelete || IsDoor( m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex ) )	
	{
		//090527 pdy 하우징 시스템메세지 기본제공가구 [설치해제]
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1888 ) );	//1888	"기본 물품은 설치해제 할 수 없습니다."
		return FALSE;
	}

	if( m_pDecoObj->GetID() == HOUSE_DUMMYID  )
	{
		CancelDecoration();
		return TRUE;
	}

	//dwData1(채널) dwData2(가구ObjectIndex)
	MSG_DWORD2 msg ;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category	    = MP_HOUSE;
	msg.Protocol		= MP_HOUSE_UNINSTALL_SYN;
	msg.dwObjectID		= HEROID;
	msg.dwData1			= m_CurHouse.dwChannelID;
	msg.dwData2			= m_pDecoObj->GetFurnitureInfo()->dwObjectIndex;

	NETWORK->Send(&msg,sizeof(msg));

	EndDecoAngle();
	return TRUE;
}

//필드상의 오브젝트를 재설정할때 요청한다.
BOOL cHousingMgr::RequestUpdateCurDeco()
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	if( !m_pDecoObj )
		return FALSE;

	//꾸미기전 셋팅과 현재설정된 셋팅이 거의 같으면 업데이트요청 실패   
	if( fabs(m_fBackupDecoAngle - m_pDecoObj->GetAngle() ) < 0.001f && 
		fabs(m_vDecoPos.x - m_vBackupDecoPos.x ) < 0.001f &&
		fabs(m_vDecoPos.y - m_vBackupDecoPos.y ) < 0.001f &&
		fabs(m_vDecoPos.z - m_vBackupDecoPos.z ) < 0.001f		)
	{
		//090605 pdy 마킹버그 수정 
		//가구셋팅을 취소하자.
		CancelDecoration();
		return FALSE;
	}

	if(! CanInstallFunitureToField() )
	{
		//설치 불가능한 셋팅이므로 업데이트 요청실패 
		//090527 pdy 하우징 시스템메세지 [설치 불가능]
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1860 ) );	//1860	"이곳에 해당 가구를 배치 할 수 없습니다."
		return FALSE;
	}

	stFurniture* pstFuniture = m_pDecoObj->GetFurnitureInfo();
	WORD wCategory = Get_HighCategory(pstFuniture->dwFurnitureIndex);
	WORD wSlot = pstFuniture->wSlot;
	DWORD dwObjectIdx = m_CurHouse.m_dwFurnitureList[wCategory][wSlot];

	MSG_HOUSE_FURNITURE_INSTALL msg;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category	    = MP_HOUSE;
	msg.Protocol		= MP_HOUSE_UPDATE_SYN;
	msg.dwObjectID		= HEROID;
	msg.dwChannel		= m_CurHouse.dwChannelID;
	msg.dwFurnitureObjectIndex = dwObjectIdx;
	msg.fAngle			= m_pDecoObj->GetAngle();
	msg.vPos			= m_vDecoPos;
	msg.wSlot			= wSlot;

	NETWORK->Send(&msg,sizeof(msg));

	EndDecoAngle();

	return TRUE;
}


//오브젝트꾸미기중이 아니면서 아이콘클릭으로 
//설치 해제 요청을할땐 퍼니쳐정보로 바로 요청하자
BOOL cHousingMgr::RequestUninstallDecoFromUseIcon(stFurniture* pstFurniture)
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	if( !pstFurniture || m_bDoDecoration ) 	//현재 오브젝트 설치 중이면 안된다.
		return FALSE;

	if( pstFurniture->wState != eHOUSEFURNITURE_STATE_INSTALL )	//설치중이 아닌 가구는 안된다.
		return FALSE;

	
	//dwData1(채널) dwData2(가구ObjectIndex)
	MSG_DWORD2 msg ;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category	    = MP_HOUSE;
	msg.Protocol		= MP_HOUSE_UNINSTALL_SYN;
	msg.dwObjectID		= HEROID;
	msg.dwData1			= m_CurHouse.dwChannelID;
	msg.dwData2			= pstFurniture->dwObjectIndex;

	NETWORK->Send(&msg,sizeof(msg));

	return TRUE;
}

//090331 pdy Box To Box 채크 
BOOL cHousingMgr::TestCollisionBox2Box(BOUNDING_BOX* pSrcBoundingBox, BOUNDING_BOX* pDestBoundingBox , BOOL bCheckHeight)
{
	// 연산을 줄이기위한 정석 OBB가 아닌 간단한 높이와 박스의 2D분할축 채크만 하는 제한적 충돌채크   
	// 요구조건. BOUNDINGBOX의 회전축은 Y축만있다고 가정한다. ( 높이체크를 간단하게 하기위해..)

	//바운딩박스 8점 인덱싱 
	//위에꺼
	//	[0] [3]   
	//	[4] [7]

	//아래꺼 
	//	[1]	[2]	  
	//	[5] [6]

	if( bCheckHeight )
	{
		//간단한 높이채크
		//Src의 높이 영역이 Dest의 높이 영역에 포함되지 않는경우 충돌 = FALSE 
		if( pSrcBoundingBox->v3Oct[4].y < pDestBoundingBox->v3Oct[5].y	||		// Src.MAXY < Dest.MINY || 
			pSrcBoundingBox->v3Oct[5].y > pDestBoundingBox->v3Oct[4].y	 )		// Src.MINY > Dest.MAXY
			return FALSE;
	}

	//2D 분할축 채크 
	//2D 사각형의 각각의 X축 Z축이 분할축이 되며  
	//두 사각형의 각 정점을 분할축에 정사형했을때 
	//Min Max구간이 서로 겹치지 않으면 분할축이 존재하여 
	//두 사각형은 겹쳐있지 않다.

	VECTOR3 vAxis[4] = {0,};

	vAxis[0] = pSrcBoundingBox->v3Oct[1] - pSrcBoundingBox->v3Oct[5];
	vAxis[1] = pSrcBoundingBox->v3Oct[6] - pSrcBoundingBox->v3Oct[5];

	vAxis[2] = pDestBoundingBox->v3Oct[1] - pDestBoundingBox->v3Oct[5];
	vAxis[3] = pDestBoundingBox->v3Oct[6] - pDestBoundingBox->v3Oct[5];

	for( int i=0; i<4 ; i++)
		Normalize( &vAxis[i] , &vAxis[i] );


	DWORD dwVertexIndexArr[4] = {1,2,5,6};	//2D이므로 아래쪽의 버택스의 인덱스를 담아두자

	for( int i=0 ; i<4; i++)
	{
		float fSrcMin,fDestMin; 
		float fSrcMax,fDestMax;

		fSrcMin = fDestMin =  1000000.0f;
		fSrcMax = fDestMax = -1000000.0f;

		for( int j=0; j<4; j++)
		{
			float fSrcDotLength = 0 , fDestDotLength = 0;

			fSrcDotLength  =  vAxis[i] * pSrcBoundingBox->v3Oct[dwVertexIndexArr[j]];
			fDestDotLength =  vAxis[i] * pDestBoundingBox->v3Oct[dwVertexIndexArr[j]]; 

			if( fSrcDotLength < fSrcMin )
				fSrcMin = fSrcDotLength;

			if( fSrcDotLength > fSrcMax )
				fSrcMax = fSrcDotLength;

			if( fDestDotLength < fDestMin )
				fDestMin = fDestDotLength;

			if( fDestDotLength > fDestMax )
				fDestMax = fDestDotLength;
		}

		float fabsMinValue = (fSrcMin < fDestMin )? fabs(fSrcMin)+1.0f : fabs(fDestMin)+1.0f;

		//Src 와 Dest의 정사형된 Min Max값이 서로 겹치지 않다면 분할축 존재 
		if( (fSrcMax + fabsMinValue)  <  (fDestMin + fabsMinValue ) || (fSrcMin + fabsMinValue) > ( fDestMax + fabsMinValue) )
		{
			//분할축이 존재하므로 충돌되지 않았다.
			return FALSE;	
		}
	}
	//분할축이 존재하지 않다면 두박스는 겹쳐있다. 
	return TRUE;
}

//인벤Item->창고Icon
BOOL cHousingMgr::RequestStoredItem(POSTYPE ToPos, CItem * pFromItem)
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	pFromItem->SetLock(TRUE);

	//dwData1(채널) dwData2(아이템Index) dwData3(아이템Slot) dwData4(내집창고Slot)
	MSG_DWORD4 msg ;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category = MP_HOUSE;
	msg.Protocol = MP_HOUSE_STORED_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1		= m_CurHouse.dwChannelID;
	msg.dwData2		= pFromItem->GetItemIdx();
	msg.dwData3		= pFromItem->GetPosition();
	msg.dwData4		= ToPos;

	NETWORK->Send(&msg,sizeof(msg));


	return TRUE;
}

//창고Icon->인벤Item
BOOL cHousingMgr::RequestRestoredICon(POSTYPE ToPos, cHousingStoredIcon * pFromIcon)
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	pFromIcon->SetLock(TRUE);

	//dwData1(채널) dwData2(가구ObjectIndex) dwData3(내집창고Slot)
	MSG_DWORD3 msg;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category = MP_HOUSE;
	msg.Protocol = MP_HOUSE_RESTORED_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1		= m_CurHouse.dwChannelID; 
	msg.dwData2		= pFromIcon->GetLinkFurniture()->dwObjectIndex;
	msg.dwData3		= pFromIcon->GetLinkFurniture()->wSlot;

	NETWORK->Send(&msg,sizeof(msg));

	return TRUE;
}

void cHousingMgr::RequestDecoRationMode()
{
	if(! IsHouseInfoLoadOk() )
		return;

	//꾸미기 모드 끝네기는 바로 요청 
	//dwData1(채널), dwData2(On/Off)
	MSG_DWORD2 msg;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category	    = MP_HOUSE;
	msg.Protocol		= MP_HOUSE_DECOMODE_SYN;
	msg.dwData1			= m_CurHouse.dwChannelID;
	msg.dwData2			= (!m_bDecorationMode);

	NETWORK->Send(&msg,sizeof(msg));
}

void cHousingMgr::RequestDestroyFuniture(stFurniture* pFuniture)
{
	if(! IsHouseInfoLoadOk() )
		return;

	if( ! pFuniture )
		return;

	// dwData1(채널), dwData2(가구Index)
	MSG_DWORD2 msg;
	ZeroMemory( &msg, sizeof( msg ) );
	msg.Category	    = MP_HOUSE;
	msg.Protocol		= MP_HOUSE_DESTROY_SYN;
	msg.dwData1			= m_CurHouse.dwChannelID;
	msg.dwData2			= pFuniture->dwObjectIndex;

	NETWORK->Send(&msg,sizeof(msg));
}

BOOL cHousingMgr::FakeUseAction(stFurniture* pFuniture,stHouseActionInfo* pActionInfo)
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	if( ! pFuniture || ! pActionInfo )
		return FALSE;

	//090618 pdy 액션 제한사항 변경 히어로 상태가 NONE이아닐때 내리기,창고열기를 제외한 액션 사용불가.
	if( HERO->GetState() != eObjectState_None &&
		pActionInfo->dwActionType != eHOUSE_ACTION_TYPE_GETOFF &&
		pActionInfo->dwActionType != eHOUSE_ACTION_TYPE_STORAGE	)		
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(789) ) ;
		return FALSE;
	}

	//문을 제외한 액션은 거리체크를 하여야 한다.
	//090618 pdy 하우징 기능추가 창고열기 액션도 거리체크에서 제외
	if( ! IsDoor( pFuniture->dwFurnitureIndex ) &&
		pActionInfo->dwActionType != eHOUSE_ACTION_TYPE_STORAGE &&
		pActionInfo->dwActionType != eHOUSE_ACTION_TYPE_OPEN_HOMEPAGE )
	{
		VECTOR3 vHeroPos = {0,};
		HERO->GetPosition(&vHeroPos);

		float fLength = CalcDistanceXZ(&vHeroPos, &pFuniture->vPosition ) ;

		//090527 pdy 하우징 시스템메세지 액션 거리가 멀때 제한사항 [액션]
		if( fLength > MAX_HOUSE_ACTION_DISTANCE )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1900 ) );	//1900	"사용하시려면 근처로 가셔야합니다."
			return FALSE;
		}
	}

	// 설치리스트에 없으면 리턴
	if(!m_FieldFurnitureTable.GetData(pFuniture->dwObjectIndex))			
		return FALSE;

	m_CurActionInfo.Clear();

	m_CurActionInfo.pActionFuniture = pFuniture;
	m_CurActionInfo.pActionInfo = pActionInfo;

	switch(pActionInfo->dwActionType)
	{
	case eHOUSE_ACTION_TYPE_DOOR_EXITHOUSE :
		{
			//090527 pdy 하우징 팝업창 [하우스나가기]
			WINDOWMGR->MsgBox( MBI_HOUSE_EXIT_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1894)) ; //1894	"정말로 나가시겠습니까?"
			GAMEIN->GetHousingActionPopupMenuDlg()->SetDisable(TRUE);

			//090618 pdy 하우징 나가기 액션 버그 수정 
			OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal) ;
			return FALSE; 
		}
		break;
	case eHOUSE_ACTION_TYPE_DOOR_VOTEHOUSE :
		{
			//090527 pdy 하우징 팝업창 [하우스추천하기]
			WINDOWMGR->MsgBox( MBI_HOUSE_VOTEHOUSE_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1895)) ; //1895	"이 집을 추천 하시겠습니까?"
			GAMEIN->GetHousingActionPopupMenuDlg()->SetDisable(TRUE);
			return FALSE; 
		}
		break;
	}
	//패킷을 보네자 

	return RequestUseCurAction();
}

BOOL cHousingMgr::RequestUseCurAction()
{
	if( ! m_CurActionInfo.pActionFuniture || !m_CurActionInfo.pActionInfo )
		return FALSE;

	switch(m_CurActionInfo.pActionInfo->dwActionType)
	{
		case eHOUSE_ACTION_TYPE_RIDE:
			{
				DWORD* pSortedNearBoneNumArr = GAMEIN->GetHousingActionPopupMenuDlg()->GetSortedNearBoneNumArr();
				stFunitureInfo* pstFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(m_CurActionInfo.pActionFuniture->dwFurnitureIndex);
				if( ! pstFurnitureInfo )
					return FALSE;
				//dwData1(채널), dwData2(가구Index), dwData3(Attach슬롯), dwData4(액션Type)

				int nAttachSlot = -1;
				for( int i=0; i < pstFurnitureInfo->byAttachBoneNum ; i++)
				{
					if( m_CurActionInfo.pActionFuniture->dwRidingPlayer[i] == 0 )
					{
						nAttachSlot = pSortedNearBoneNumArr[i] -1;
						break;
					}
				}

				//090527 pdy 하우징 시스템메세지 탑승시 제한사항 [빈자리가 없을때]
				if( -1 == nAttachSlot )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1877 ) ); //1877	"인원이 가득차 탑승 하실 수 없습니다."
					return FALSE;
				}

				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData1 = m_CurHouse.dwChannelID;
				msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
				msg.dwData3 = nAttachSlot;
				msg.dwData4 = m_CurActionInfo.pActionInfo->dwActionIndex;//m_CurActionInfo.pActionInfo->dwActionType;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		case eHOUSE_ACTION_TYPE_GETOFF : 
			{
				stFunitureInfo* pstFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(m_CurActionInfo.pActionFuniture->dwFurnitureIndex);

				if( ! pstFurnitureInfo )
					return FALSE;

				int nAttachSlot = -1;
				for( int i=0; i < pstFurnitureInfo->byAttachBoneNum ; i++)
				{
					if( m_CurActionInfo.pActionFuniture->dwRidingPlayer[i] ==  gHeroID )
					{
						nAttachSlot = i;
						break;
					}
				}

				if( -1 == nAttachSlot )
				{
					return FALSE;
				}

				//dwData1(채널), dwData2(가구Index), dwData3(Attach슬롯), dwData4(액션Index)
				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData1 = m_CurHouse.dwChannelID;
				msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
				msg.dwData3 = nAttachSlot;
				msg.dwData4 = m_CurActionInfo.pActionInfo->dwActionIndex;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		case eHOUSE_ACTION_TYPE_NORMAL : 
			{
				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData1 = m_CurHouse.dwChannelID;
				msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
				msg.dwData3 = 0;
				msg.dwData4 = m_CurActionInfo.pActionInfo->dwActionIndex;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		case eHOUSE_ACTION_TYPE_BUFF :
			{
				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData1 = m_CurHouse.dwChannelID;
				msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
				msg.dwData3 = 0;
				msg.dwData4 = m_CurActionInfo.pActionInfo->dwActionIndex;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		case eHOUSE_ACTION_TYPE_DOOR_EXITHOUSE :	
			{
				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_EXIT_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData = m_CurHouse.dwChannelID;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		case eHOUSE_ACTION_TYPE_DOOR_VOTEHOUSE :
			{
				MSG_DWORD msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_VOTE_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData = m_CurHouse.dwChannelID;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		//090616 pdy 하우징 기능추가 [창고열기 가구액션 추가]
		case eHOUSE_ACTION_TYPE_STORAGE:
			{
				if( ! HOUSINGMGR->IsHouseOwner() )
				{
					//090616 pdy 하우징 시스템메세지 집주인이 아닐시 제한사항 [창고열기 액션]
					CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1887) ); //1887	"집주인이 아닐시 사용하실 수 없습니다"
					return FALSE;
				}

				//창고열기 사용 
				cDialog* storageDialog = WINDOWMGR->GetWindowForID( PYO_STORAGEDLG );
	
				if( ! storageDialog )
				{
					return FALSE;
				}
				else if( storageDialog->IsActive() )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1552 ) );
					return FALSE;
				}
				else if( ! HERO->GetStorageNum() )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1551 ) );
					return FALSE;
				}
				// 090403 ONS 조합,분해,강화,인챈트 다이얼로그가 활성화 되어 있는 상태에서 창고소환 아이템 사용 불가
				else
				{
					cDialog* pMixDialog				= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );			// 조합
					cDialog* pDissolutioniDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );	// 분해
					cDialog* pReinforceDialog		= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );	// 강화
					cDialog* pUpgradeDialog			= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );		//인첸트

					if( !pMixDialog				|| 
						!pDissolutioniDialog	|| 
						!pReinforceDialog		|| 
						!pUpgradeDialog	) 
					{
							return FALSE;
					}

					if(	pMixDialog->IsActive()			|| 
						pDissolutioniDialog->IsActive() || 
						pReinforceDialog->IsActive()	|| 
						pUpgradeDialog->IsActive()		)
					{
						CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1080 ) );
						return FALSE;
					}
				}

				//패킷을 보네자.
				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData1 = m_CurHouse.dwChannelID;
				msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
				msg.dwData3 = 0;
				msg.dwData4 = m_CurActionInfo.pActionInfo->dwActionIndex;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
		// 091105 pdy 하우징 가구 액션추가 ( 미니홈피 웹 브라우저 링크 ) 
		case eHOUSE_ACTION_TYPE_OPEN_HOMEPAGE:
			{
				MSG_DWORD4 msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ACTION_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData1 = m_CurHouse.dwChannelID;
				msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
				msg.dwData3 = 0;
				msg.dwData4 = m_CurActionInfo.pActionInfo->dwActionIndex;
				NETWORK->Send(&msg,sizeof(msg));
			}
			break;
	}

	return TRUE;
}

BOOL cHousingMgr::RequestAction_GetOff()
{
	stFunitureInfo* pstFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(m_CurActionInfo.pActionFuniture->dwFurnitureIndex);

	if( ! pstFurnitureInfo )
		return FALSE;

	int nAttachSlot = -1;
	for( int i=0; i < pstFurnitureInfo->byAttachBoneNum ; i++)
	{
		if( m_CurActionInfo.pActionFuniture->dwRidingPlayer[i] ==  gHeroID )
		{
			nAttachSlot = i;
			break;
		}
	}

	if( -1 == nAttachSlot )
	{
		return FALSE;
	}

	// 즉시이동을 해야하니 응답받기전에 먼저 블럭을 풀어주자.
	CheckAllMarkingObjByHeroZone();

	//dwData1(채널), dwData2(가구Index), dwData3(Attach슬롯), dwData4(액션Index)
	MSG_DWORD4 msg;
	msg.Category = MP_HOUSE;
	msg.Protocol = MP_HOUSE_ACTION_FORCE_GETOFF_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = m_CurHouse.dwChannelID;
	msg.dwData2 = m_CurActionInfo.pActionFuniture->dwObjectIndex;  
	msg.dwData3 = nAttachSlot;
	msg.dwData4 = HOUSE_ACTIONINDEX_GETOFF;
	NETWORK->Send(&msg,sizeof(msg));

	return TRUE;
}

BOOL cHousingMgr::RequestUseDecoBonus(DWORD dwBonusIndex)
{
	if(! IsHouseInfoLoadOk() )
		return FALSE;

	stHouseBonusInfo* pBonusInfo = GAMERESRCMNGR->GetHouseBonusInfo(dwBonusIndex);

	if( !pBonusInfo )
		return FALSE;

	if(! HOUSINGMGR->CanUseDecoBonus(pBonusInfo) )
		return FALSE;

	//dwData1(채널), dwData2(보너스index)
	MSG_DWORD2 msg;
	msg.Category = MP_HOUSE;
	msg.Protocol = MP_HOUSE_BONUS_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = m_CurHouse.dwChannelID;
	msg.dwData2 = dwBonusIndex;
	NETWORK->Send(&msg,sizeof(msg));

	return TRUE;
}

void cHousingMgr::StartHouseInfoLoading()
{
	m_dwHouseLoadState = eHSLoad_NOWLOADING;

	// 몬스터 미터 창 비활성화.
	if( GAMEIN->GetMonstermeterDlg() )
		GAMEIN->GetMonstermeterDlg()->SetActive(FALSE) ;
}

//서버에서 NACK 메세지가 왔을경우 호출된다
void cHousingMgr::PrintDebugErrorMsg(char* pstrMsg,WORD dwError)
{
	//GMTOOL 에서만 서버에서 보네준 원인을모두 표시해준다.
#if defined(_GMTOOL_)	
	if( pstrMsg == NULL )
		return ;

	char MsgBuf[256] ={0,};
	CHATMGR->AddMsg( CTC_SYSMSG,"HousingMgr::PrintDebugErrorMsg [ONLY GMTOOL]");

	switch(dwError)
	{
		case eHOUSEERR_NOOWNER:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOOWNER");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NOHOUSE:	
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOHOUSE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_HAVEHOUSE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_HAVEHOUSE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_ENTRANCEFAIL:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_ENTRANCEFAIL");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_LOADING:	
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_LOADING");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_FULL:	
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_FULL");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_DONOT_HOUSE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_DONOT_HOUSE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_DO_HOUSE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_DO_HOUSE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NOTOWNER:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOTOWNER");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_MAX_EXTEND:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_MAX_EXTEND");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_DECOMODE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_DECOMODE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NOTENOUGHPOINT:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOTENOUGHPOINT");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_INVAILDSTATE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_INVAILDSTATE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NOFURNITURE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOFURNITURE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_HASRIDER:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_HASRIDER");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NOTRIDING:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOTRIDING");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_RIDING:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_RIDING");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_RESERVATING:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_RESERVATING");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_DISTANCE_FAR:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_DISTANCE_FAR");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NOTREGIST:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NOTREGIST");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_HOME2HOME_FAIL:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_HOME2HOME_FAIL");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_ERROR:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_ERROR");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_ONVEHICLE:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_ONVEHICLE");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case eHOUSEERR_NORANKER:
			{
				sprintf(MsgBuf,"%s %s",pstrMsg,"eHOUSEERR_NORANKER");
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		case 777:
			{
				sprintf(MsgBuf,"CLIENT_DEBUG_MSG : %s",pstrMsg);
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
		default:
			{
				sprintf(MsgBuf,"NOT DEFINED ERROR NUMBER %s %d",pstrMsg,dwError);
				CHATMGR->AddMsg( CTC_SYSMSG, MsgBuf);
			}
			break;
	}
#endif
}

CObjectBase* cHousingMgr::GetSelectedObjectFromHouseMap(int MouseX,int MouseY,int PickOption , BOOL bSort)
{
	//하우스 맵에서 꾸미기 모드가 아닐때는 이함수를 이용하여 피킹된 오브젝트를 가져오자 
	CEngineObject* pEngineObject = NULL;
	PICK_GXOBJECT_DESC gxoDesc[5] = {0,};
	GetSelectedObject( gxoDesc, MouseX, MouseY, PickOption , /*bSort = TRUE*/ bSort);

	for( DWORD i = 0 ; i < 5 ; ++i )
	{
		if(gxoDesc[i].gxo)
		{
			pEngineObject = (CEngineObject*)g_pExecutive->GetData(gxoDesc[i].gxo);
			if( pEngineObject != NULL )
			{
				if( HERO )
				if( HERO->GetEngineObject() != pEngineObject )
				{
					if( pEngineObject->GetBaseObject() )
					{
						if( pEngineObject->GetBaseObject()->GetEngineObjectType() == eEngineObjectType_Monster )
						{
							if( ((CMonster*)(pEngineObject->GetBaseObject()))->GetSInfo()->SpecialType == 2 ) //강아지 클릭 안되도록
							{
								pEngineObject = NULL;
								continue;
							}
						}
						//하우징맵에서는 가구가 액션이 없으면 클릭이 안된다
						else if(	HERO->GetState() != eObjectState_Housing &&
									pEngineObject->GetBaseObject()->GetEngineObjectType() == eEngineObjectType_Furniture	)
						{
							if( ! IsActionableFurniture( (CFurniture *)pEngineObject->GetBaseObject()) )
							{
								pEngineObject = NULL;
								continue;
							}
						}
					}
					break;	//---pick first one
				}
			}
		}
	}

	if(pEngineObject == NULL)
		return NULL;
	
	return pEngineObject->GetBaseObject();
}

stDynamicHouseNpcMapInfo* cHousingMgr::GetDynimiHouseNpcMapInfo(DWORD dwMapIndex)
{
	return m_HousingSettingInfo.m_DynamicHouseNpcMapInfoList.GetData(dwMapIndex);
}

stDynamicHouseNpcInfo* cHousingMgr::GetDynimiHouseNpcInfo(DWORD dwMapIndex, BYTE byRank, BYTE byExterioKind)
{
	stDynamicHouseNpcMapInfo* pMapInfo = GetDynimiHouseNpcMapInfo( dwMapIndex );

	if( ! pMapInfo ) 
		return NULL;

	DWORD dwRankTypeIndex = byRank*100 + byExterioKind;
	return pMapInfo->pDynamicHouseNpcList.GetData( dwRankTypeIndex );
}

stDynamicHouseNpcInfo*	cHousingMgr::FindDynimiHouseNpcInfoByNpcKind(DWORD dwMapIndex , DWORD dwNpcKind)
{
	stDynamicHouseNpcMapInfo* pMapInfo = GetDynimiHouseNpcMapInfo( dwMapIndex );

	if( ! pMapInfo ) 
		return NULL;

	pMapInfo->pDynamicHouseNpcList.SetPositionHead();

	for(stDynamicHouseNpcInfo* pNpcInfo = pMapInfo->pDynamicHouseNpcList.GetData();
		0 < pNpcInfo;
		pNpcInfo = pMapInfo->pDynamicHouseNpcList.GetData())
	{
		if( pNpcInfo->dwNpcKind == dwNpcKind )
		{
			return pNpcInfo;
		}
	}

	return 0;
}

WORD cHousingMgr::GetStoredFunitureNumByCategory(WORD dwCategoryIdx)
{
	return m_CurHouse.m_dwCategoryNum[dwCategoryIdx];
}

WORD cHousingMgr::GetBlankSlotIndexFromStoredFunitureListByCategory(WORD dwCategoryIdx)
{
	for(int i=0; i<MAX_HOUSING_SLOT_NUM; i++ )
	{
		if( m_CurHouse.m_dwFurnitureList[dwCategoryIdx][i] == 0 )
		{
			return i;
		}
	}

	return 0;
}

BOOL cHousingMgr::IsHousingMap()
{
	return MAP->GetMapNum() == HOUSINGMAP;
}

BOOL cHousingMgr::IsActionableFurniture(CFurniture* pFurniture)
{
	if(! pFurniture )
		return FALSE;

	DWORD dwIdx =  pFurniture->GetFurnitureInfo()->dwFurnitureIndex;

	if( ! IsActionable(dwIdx) && ! IsDoor(dwIdx) ) 
	{
		//기능성이 아닐때
		return FALSE;
	}

	return TRUE;
}

bool cHousingMgr::IsFieldHeightObject(CObject* pObject)
{
	VECTOR3 ObjPos;
	pObject->GetPosition(&ObjPos);

	float fFieldHeight = 0.0f;

	g_pExecutive->GXMGetHFieldHeight(&fFieldHeight,ObjPos.x,ObjPos.z);

	if( fabs(fFieldHeight - ObjPos.y) < 0.0001f )
		return true;

	return false;
}

float cHousingMgr::GetStartAngle()
{
	stFurniture* pFurniture = NULL;
	stFurniture* pStart = NULL;
	m_CurHouse.pNotDeleteFurnitureList.SetPositionHead();
	while((pFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData()) != NULL)
	{
		if(IsStart(pFurniture->dwFurnitureIndex))
			pStart = pFurniture;
	}

	if(pStart)
		return pStart->fAngle;

	return 0.0f;
}

BOOL cHousingMgr::CanUseItemFromHousingMap(CItem* pItem)
{
	//하우징 맵에서 사용 불가능한 아이템은 return FALSE

	if(	pItem->GetItemInfo()->SupplyType == ITEM_KIND_CHANGESIZE_UPWEIGHT ||	// 캐릭터 키변경 - 커질확률 가중치
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_CHANGESIZE_DNWEIGHT ||	// 캐릭터 키변경 - 작아질확률 가중치
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_CHANGESIZE_DEFALUT ||		// 캐릭터 키변경 - 원래대로
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_SUMMON_MONSTER	||		// 몬스터 소환
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_SUMMON_NPC )				// NPC 소환 
	{
			return FALSE;
	}

	return TRUE;
}

BOOL cHousingMgr::CanUseItemFromDecorationMode(CItem* pItem)
{
	//꾸미기 모드중에 사용 가능한 아이템은 return TRUE
	if(	pItem->GetItemInfo()->SupplyType == ITEM_KIND_FURNITURE_WALLPAPER ||		//벽지 교체 아이템
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_FURNITURE_FLOORPAPER ||		//바닥지 교체 아이템
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_FURNITURE_CEILINGPAPER )		//천장지 교체 아이템 
	{
			return TRUE;
	}

	return FALSE;
}

BOOL cHousingMgr::CanUseDecoBonus(stHouseBonusInfo* pBonusInfo,DWORD* pdwErr )
{
	if( !IsHouseOwner() )
	{
		//집주인이 아닐때
		if( pdwErr )
			*pdwErr = 1;
		return FALSE;
	}

	if( pBonusInfo->byKind == 1 && ! IsRankHouse() )
	{
		//랭커 보너스인데 랭커가 아닐때
		if( pdwErr )
			*pdwErr = 4;
		return FALSE;
	}

	if( m_CurHouse.dwDecoPoint < pBonusInfo->dwDecoPoint  )
	{
		//꾸미기 포인트 요구 조건이 부족할때
		if( pdwErr )
			*pdwErr = 2;
		return FALSE;
	}

	if( m_CurHouse.HouseInfo.dwDecoUsePoint < pBonusInfo->dwUsePoint )
	{
		//별 포인트 요구 조건이 부족할때
		if( pdwErr )
			*pdwErr = 3;
		return FALSE;
	}

	return TRUE;
}

BOOL cHousingMgr::CanInstallFunitureToField()
{
	//현재 꾸미기 중인 가구가 설치가 가능한지여부를 리턴
	if(!m_pDecoObj)
		return FALSE;

	if( ! m_pDecoObj->GetEngineObject()->GetGXOHandle() )
		return FALSE;

	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex );
	if( ! stFurnitureInfo )	
		return FALSE;

	if(! m_PickAnotherObjHandle )		//픽킹된 다른 오브젝트 핸들이 없을경우 리턴 (바닥 , 벽 , 오브젝트 등) 
		return FALSE;

	CObjectBase* pBaseObj = ( (CEngineObject*)g_pExecutive->GetData(m_PickAnotherObjHandle) )->GetBaseObject();
	DWORD dwPickAnotherObjIdx =  ((CFurniture*)pBaseObj)->GetFurnitureInfo()->dwFurnitureIndex;

	stFunitureInfo* pstPickAnotherInfo = GAMERESRCMNGR->GetFunitureInfo( dwPickAnotherObjIdx );
	if( !pstPickAnotherInfo )
		return FALSE;

	if( m_dwFieldFurnitureNum > MAX_FURNITURE_STORAGE_NUM )			//필드에 가구가 한계갯수보다 크면 설치 불가능 
		return FALSE;

	//------충돌 채크전에 가구 타입채크-----// 
	if( Get_HighCategory(stFurnitureInfo->dwFurnitureIndex) ==  eHOUSE_HighCategory_Door)		//자신이 벽장식일경우 
	{
		if(! IsExteriorWall(dwPickAnotherObjIdx) )					//픽킹된 오브젝트가 외벽이어야 한다. 
			return FALSE;
	}
	else if( IsStart(stFurnitureInfo->dwFurnitureIndex) )			//시작위치일경우  
	{
		//자신이 시작위치일경우 
		if( Get_HighCategory(dwPickAnotherObjIdx) != eHOUSE_HighCategory_Carpet )	//피킹된 다른오브젝트가 바닥류가 아니면 설치 불가능 
			return FALSE;

		VECTOR3 Pos;
		m_pDecoObj->GetPosition(&Pos);

		if( Pos.y >= MAX_START_FURNITURE_HEIGHT ) 
			return FALSE;

		if( ! pstPickAnotherInfo->bStackable )
			return FALSE;
	}
	else // 일반오브젝트일경우 
	{
		//m_PickAnotherObjHandle의 오브젝트가 bStackable이 가능해야 설치할수있다.
		if( ! pstPickAnotherInfo->bStackable )
			return FALSE;
	}
	//가구 타입채크 끝  

	if( m_dwFieldFurnitureNum != 0)									
	{
		m_FieldFurnitureTable.SetPositionHead();

		for(CFurniture* pDestObj = m_FieldFurnitureTable.GetData();
			0 < pDestObj;
			pDestObj = m_FieldFurnitureTable.GetData())
		{
			if( m_pDecoObj->GetID() == pDestObj->GetFurnitureInfo()->dwObjectIndex )
				continue;

			if( ! pDestObj->GetEngineObject()->GetGXOHandle() )
				continue;


			stFunitureInfo* stDestInfo = GAMERESRCMNGR->GetFunitureInfo( ((CFurniture*)pDestObj)->GetFurnitureInfo()->dwFurnitureIndex );

			if( IsFloor ( stDestInfo->dwFurnitureIndex ) )		//바닥과는 충돌채크 안한다.
				continue;

			COLLISION_MESH_OBJECT_DESC DecoObjDesc,DestObjDesc ;
			float fDecoRadius,fDestRadius;
			VECTOR3 vDecoPos , vDestPos ; 

			g_pExecutive->GXOGetCollisionMesh(m_pDecoObj->GetEngineObject()->GetGXOHandle(), &DecoObjDesc);
			g_pExecutive->GXOGetCollisionMesh(pDestObj->GetEngineObject()->GetGXOHandle(), &DestObjDesc);

			fDecoRadius = DecoObjDesc.boundingSphere.fRs ;
			fDestRadius = DestObjDesc.boundingSphere.fRs ;

			m_pDecoObj->GetPosition( &vDecoPos );
			pDestObj->GetPosition( &vDestPos );

			VECTOR3 vDeco2DestPos = vDestPos - vDecoPos;
			float fDeltaLength = VECTOR3Length(&vDeco2DestPos);

			if( fDeltaLength > (fDecoRadius + fDestRadius) ) //구가 겹치지 않으면 검사가 필요없다 다음Obj로 패스  	
				continue;


			// NYJ - 오브젝트의 바운딩박스끼리 충돌체크
			const int MAX_COLLISION_DESC_NUM = 10;
			COLLISION_MESH_OBJECT_DESC DecoObjectDesc[MAX_COLLISION_DESC_NUM];
			ZeroMemory(DecoObjectDesc, sizeof(DecoObjectDesc));
			COLLISION_MESH_OBJECT_DESC DestObjectDesc[MAX_COLLISION_DESC_NUM];
			ZeroMemory(DestObjectDesc, sizeof(DestObjectDesc));

			DWORD dwDecoColDescNum, dwDestColDescNum;
			DWORD dwDecoModelNum, dwDestModelNum;
			DWORD dwDecoObjectNum, dwDestObjectNum;

			dwDecoModelNum = g_pExecutive->GXOGetModelNum(m_pDecoObj->GetEngineObject()->GetGXOHandle());
			dwDestModelNum = g_pExecutive->GXOGetModelNum(pDestObj->GetEngineObject()->GetGXOHandle());

			DWORD i,j;
			dwDecoColDescNum = 0;
			for(i=0; i<dwDecoModelNum; i++)
			{
				dwDecoObjectNum = g_pExecutive->GXOGetObjectNum(m_pDecoObj->GetEngineObject()->GetGXOHandle(), i);

				for(j=0; j<dwDecoObjectNum; j++)
				{
					if(dwDecoColDescNum >= MAX_COLLISION_DESC_NUM)
					{
						__asm int 3;
						break;
					}

					if(!g_pExecutive->GXOGetCollisononObjectDesc(m_pDecoObj->GetEngineObject()->GetGXOHandle(), &DecoObjectDesc[dwDecoColDescNum], i, j))
						continue;

					dwDecoColDescNum++;
				}
			}

			// Dest충돌정보 가져오기
			dwDestColDescNum = 0;
			for(i=0; i<dwDestModelNum; i++)
			{
				dwDestObjectNum = g_pExecutive->GXOGetObjectNum(pDestObj->GetEngineObject()->GetGXOHandle(), i);

				for(j=0; j<dwDestObjectNum; j++)
				{
					if(dwDestColDescNum >= MAX_COLLISION_DESC_NUM)
					{
						__asm int 3;
						break;
					}

					if(!g_pExecutive->GXOGetCollisononObjectDesc(pDestObj->GetEngineObject()->GetGXOHandle(), &DestObjectDesc[dwDestColDescNum], i, j))
						continue;

					dwDestColDescNum++;
				}
			}

			// 충돌검사
			BOOL bBoxTest = FALSE;
			for(i=0; i<dwDecoColDescNum; i++)
			{
				for(j=0; j<dwDestColDescNum; j++)
				{
					if(TestCollisionBox2Box(&DecoObjectDesc[i].boundingBox, &DestObjectDesc[j].boundingBox))
						bBoxTest = TRUE;//return FALSE;
				}
			}

			if(!bBoxTest)
				continue;

			//삼각형검사가 둘다 필요없으면 설치 실패 
			if( !stFurnitureInfo->bMeshTest && !stDestInfo->bMeshTest  )
				return FALSE;
			
			VECTOR3 pos = {0};
			float dist = 0;
			DWORD modelindex,objectindex;

			// NYJ - 삼각형충돌은 대상만 하기로 (대상폴리곤 vs 데코오브젝트바운딩박스)
			if( g_pExecutive->GXOIsCollisionMeshWithObjectBoundingBox(pDestObj->GetEngineObject()->GetGXOHandle(), 
								&pos, &modelindex, &objectindex, &dist, m_pDecoObj->GetEngineObject()->GetGXOHandle()) )
								return FALSE;
		}
	}

	return TRUE;
}

//마킹을 필드에 할수있는가 
BOOL cHousingMgr::CanMakingToFiled(CObject* pObject)
{
	//안에서 히어로셀 vs 마킹BB 충돌채크가 들어간다.

	//일단 구충돌채크 
	COLLISION_MESH_OBJECT_DESC ObjDesc,HeroDesc ;
	float fObjRadius,fHeroRadius;
	VECTOR3 vObjPos , vHeroPos ; 

	stMarkingInfo* pMarkinginfo = ((CFurniture*)pObject)->GetMarkingInfo();

	if(! pMarkinginfo->bUseMaking )
		return FALSE;

	g_pExecutive->GXOGetCollisionMesh(pObject->GetEngineObject()->GetGXOHandle(), &ObjDesc);
	g_pExecutive->GXOGetCollisionMesh(HERO->GetEngineObject()->GetGXOHandle(), &HeroDesc);

	fObjRadius	= ObjDesc.boundingSphere.fRs ;
	fHeroRadius	= HeroDesc.boundingSphere.fRs ;

	pObject->GetPosition( &vObjPos );
	HERO->GetPosition( &vHeroPos );

	VECTOR3 vObj2HeroPos = vHeroPos - vObjPos;
	float fDeltaLength = VECTOR3Length(&vObj2HeroPos);

	if( fDeltaLength > (fObjRadius + fHeroRadius) )		//구가 겹치지 않으면 박스 검사가 필요없다   	
		return TRUE;

	//090608 마킹관련 버그수정	히어로가 위치한 셀 VS 마킹박스 
	DWORD dwCurTile_x =  DWORD((vHeroPos.x) / 50);
	DWORD dwCurTile_z =  DWORD((vHeroPos.z) / 50);

	VECTOR3 vHeroShellCenter = {0,};
	vHeroShellCenter.x =  float( (dwCurTile_x * 50.0f) + 25.0f );
	vHeroShellCenter.z =  float( (dwCurTile_z * 50.0f) + 25.0f );

	if( pMarkinginfo->bUseResize )						//마킹정보가 리사이즈한 바운딩 박스면 리사이즈 박스로 채크  
	{
		((CFurniture*)pObject)->UpdateMarkingInfo();
		return (! CheckPercentShellToBoundingBox( &pMarkinginfo->stResizeBoudingBox , &vHeroShellCenter ) );		//히어로의 셀과 충돌하지 않으면 return TRUE 
	}
	else												//아니라면 기본 박스로 채크 
	{
		return (! CheckPercentShellToBoundingBox( &ObjDesc.boundingBox , &vHeroShellCenter ) );						//히어로의 셀과 충돌하지 않으면 return TRUE 
	}
}

//재설정이 가능한 가구인가 
BOOL cHousingMgr::CanResetDecorationFuniture(CFurniture* pFurniture)
{
	if(! pFurniture )
		return FALSE;

	DWORD dwIdx = pFurniture->GetFurnitureInfo()->dwFurnitureIndex;

	if( pFurniture->GetFurnitureInfo()->bNotDelete )
	{
		//기본제공 가구일경우 문과 시작위치를 제외하고 리셋팅 불가능 
		if( ! IsDoor(dwIdx) && ! IsStart(dwIdx) ) 
			return FALSE;
	}

	return TRUE;
}

//셀에 포함되는 정도를 채크하여 해당 셀에 마킹을 해야하는지 여부를 리턴
BOOL cHousingMgr::CheckPercentShellToBoundingBox( BOUNDING_BOX* pSrcBoundingBox, VECTOR3* pMakingShellCenter)
{
	//바운딩박스 8점 인덱싱 
	//위에꺼
	//	[0] [3]   
	//	[4] [7]

	//아래꺼 
	//	[1]	[2]	  
	//	[5] [6]

	BOUNDING_BOX MakingBox = {0,};
	MakingBox.v3Oct[1].x = pMakingShellCenter->x - 25.0f;
	MakingBox.v3Oct[1].z = pMakingShellCenter->z + 25.0f;

	MakingBox.v3Oct[2].x = pMakingShellCenter->x + 25.0f;
	MakingBox.v3Oct[2].z = pMakingShellCenter->z + 25.0f;

	MakingBox.v3Oct[5].x = pMakingShellCenter->x - 25.0f;
	MakingBox.v3Oct[5].z = pMakingShellCenter->z - 25.0f;

	MakingBox.v3Oct[6].x = pMakingShellCenter->x + 25.0f;
	MakingBox.v3Oct[6].z = pMakingShellCenter->z - 25.0f;

	if( !TestCollisionBox2Box(pSrcBoundingBox ,&MakingBox,FALSE) )
		return FALSE;


	BOUNDING_BOX ShellCenterBox = {0,};
	ShellCenterBox.v3Oct[1].x = pMakingShellCenter->x - 6.25f;
	ShellCenterBox.v3Oct[1].z = pMakingShellCenter->z + 6.25f;

	ShellCenterBox.v3Oct[2].x = pMakingShellCenter->x + 6.25f;
	ShellCenterBox.v3Oct[2].z = pMakingShellCenter->z + 6.25f;

	ShellCenterBox.v3Oct[5].x = pMakingShellCenter->x - 6.25f;
	ShellCenterBox.v3Oct[5].z = pMakingShellCenter->z - 6.25f;

	ShellCenterBox.v3Oct[6].x = pMakingShellCenter->x + 6.25f;
	ShellCenterBox.v3Oct[6].z = pMakingShellCenter->z - 6.25f;

	if( ! TestCollisionBox2Box(pSrcBoundingBox ,&ShellCenterBox,FALSE) )
		return FALSE;

	return TRUE;
}

void cHousingMgr::SetDecorationModeTemp()
{
	if(! IsHouseInfoLoadOk() )
		return;

	// 집주인인 경우만 처리
	if(! IsHouseOwner() )
		return;

	m_bDecorationMode = TRUE;

	//090603 pdy 하우징 꾸미기 모드시엔 선택되었던 Object를 취소한다 
	CObject* pObject = OBJECTMGR->GetSelectedObject() ;								// 선택된 오브젝트 정보를 받아온다.

	if( pObject )																	// 오브젝트 정보가 유효하다면,
	{
		pObject->ShowObjectName( FALSE );											// 오브젝트 이름을 비활성화한다.

		OBJECTMGR->SetSelectedObject(NULL) ;										// 오브젝트 선택을 해제 한다.
	}
	OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Housing);
}

void cHousingMgr::SetDecorationMode(BOOL bMode)
{
	if(! IsHouseInfoLoadOk() )
		return;

	// 집주인인 경우만 처리
	if(! IsHouseOwner() )
		return;

	if( ! bMode )
	{
		if( m_bDoDecoration ) 
			CancelDecoration();
	}

	m_bDecorationMode = bMode;

	if( bMode )
	{
		//090603 pdy 하우징 꾸미기 모드시엔 선택되었던 Object를 취소한다 
		CObject* pObject = OBJECTMGR->GetSelectedObject() ;								// 선택된 오브젝트 정보를 받아온다.

		if( pObject )																	// 오브젝트 정보가 유효하다면,
		{
			pObject->ShowObjectName( FALSE );											// 오브젝트 이름을 비활성화한다.

			OBJECTMGR->SetSelectedObject(NULL) ;										// 오브젝트 선택을 해제 한다.
		}

		//090527 pdy 하우징 시스템메세지 [꾸미기모드 시작]
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1868 ) );	//1868	"꾸미기 모드를 시작합니다."
		OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Housing);
	}
	else
	{
		//090527 pdy 하우징 시스템메세지 [꾸미기모드 종료]
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1869 ) );
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Housing); //1869	"꾸미기 모드를 종료합니다."
	}

	cHousingDecoModeBtDlg* pDecoModeBtDlg = GAMEIN->GetHousingDecoModeBtDlg();

	if( pDecoModeBtDlg )
		pDecoModeBtDlg->OnChangeDecoMode( m_bDecorationMode );
}

//꾸미기 시작 (아이템->필드)
void cHousingMgr::StartDecoration(stFurniture* pstFuniture)
{
	if(! pstFuniture )
		return;

	if( ! m_bDecorationMode || m_bDoDecoration )			//꾸미기 모드가 아닐시엔 꾸미기를 할수없다. || 꾸미기중이면 할수없다.
		return;

	//일반 가구 꾸미기 
	m_pDecoObj = HOUSINGMGR->MakeDummyFuniture(pstFuniture);

	if(! m_pDecoObj->GetEngineObject()->GetGXOHandle() )
	{
		#if defined(_GMTOOL_)
			char Buf[256];
			stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex );

			if( ! stFurnitureInfo )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, "모델 읽기 실패");
				CHATMGR->AddMsg(CTC_SYSMSG, "사용중인 가구가 가구목록에 없습니다.");
				::sprintf(Buf,"가구인덱스 : %d ",m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex);
				CHATMGR->AddMsg(CTC_SYSMSG, Buf);
			}
			else
			{
				CHATMGR->AddMsg(CTC_SYSMSG, "모델 읽기 실패" );
				CHATMGR->AddMsg(CTC_SYSMSG, "가구 목록엔 있지만 해당 파일명이 Data폴더안에 있는지 확인하여 주세요" );
				sprintf(Buf,"가구이름 : %s 가구인덱스 : %d " ,stFurnitureInfo->szName , stFurnitureInfo->dwFurnitureIndex );
				CHATMGR->AddMsg(CTC_SYSMSG, Buf);
				sprintf(Buf,"모델 파일 : %s " ,stFurnitureInfo->szModelFileName);
				CHATMGR->AddMsg(CTC_SYSMSG, Buf);
			}
		#endif
		RemoveCurDeco();
		return;
	}
	StartDecoMove();

	m_bDoDecoration = TRUE;

	//090615 pdy 하우징 알파버그 수정
	m_pDecoObj->GetEngineObject()->SetAlpha(0.5f);
	g_pExecutive->GXOSetZOrder( m_pDecoObj->GetEngineObject()->GetGXOHandle() , 8 );  // 가구에 알파가 들어가면 알파 소팅문제로 Zorder를 8로 설정해준다. 
}

//꾸미기 종료 
void cHousingMgr::EndDecoration()
{
	m_pDecoObj = NULL;
	m_bDoDecoration = FALSE;

	EndDecoAngle();										// 회전설치를 종료 
	EndDecoMove();										// 이동설치를 종료 

	CHousingRevolDlg* pRevolDlg = GAMEIN->GetHousingRevolDlg();

	if( pRevolDlg ) 
		pRevolDlg->SetActive(FALSE);					// 설치UI를 닫자 
}

//설치가 캔슬될때의 처리를 한다 
void cHousingMgr::CancelDecoration()
{
	if(! m_bDoDecoration )								// 설치중이 아니라면 return
		return;

	if( m_pDecoObj )
	{
		if( m_pDecoObj->GetID() == HOUSE_DUMMYID )			// 설치중인가구가 더미오브젝트면 삭제 처리  
		{
			RemoveCurDeco();
		}
		else												// 설치중인가구가 더미가 아니면 이전 위치,회전 값으로 돌리고 다시 마킹하자 
		{
			m_pDecoObj->GetEngineObject()->ApplyHeightField(FALSE);
			m_pDecoObj->SetPosition(&m_vBackupDecoPos);
			m_pDecoObj->GetEngineObject()->ApplyHeightField(TRUE);
			m_pDecoObj->SetAngle(m_fBackupDecoAngle);
			m_pDecoObj->GetEngineObject()->SetAlpha(1.0f);
			MarkingObjZone(m_pDecoObj,TRUE);

			//090615 pdy 하우징 알파버그 수정
			stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex );
			if( stFurnitureInfo && stFurnitureInfo->bHasAlpha == FALSE )							//자체 알파가 없는 가구는
				g_pExecutive->GXOSetZOrder( m_pDecoObj->GetEngineObject()->GetGXOHandle() , 0 );	//Zorder를 0으로 설정 
		}
	}

	EndDecoration();
}

//오브젝트꾸미기중 확정을 눌렀을때 호출 
BOOL cHousingMgr::DecideSettingCurDeco()
{
	//함수내에서 요청을 하였으면 return TRUE이다.
	stFurniture* pstFuniture = m_pDecoObj->GetFurnitureInfo();

	if( pstFuniture->wState == eHOUSEFURNITURE_STATE_KEEP )
	{
		//처음설치를 시도하는 가구는 팝업창을 띄워준후 OK시 설치 요청

		//090527 pdy 하우징 팝업창 [최초설치 요청]
		WINDOWMGR->MsgBox( MBI_DECOINSTALL_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1892) ) ; //1892	"사용한 시점부터 남은 시간이 감소됩니다. 사용하시겠습니까?"
		return FALSE;
	}

	if( pstFuniture->wState == eHOUSEFURNITURE_STATE_INSTALL	) 
	{
		//인스톨된 상황이면 업데이트를 요청하자 
		if( ! RequestUpdateCurDeco() )
			return FALSE;
			
	}
	else if( pstFuniture->wState == eHOUSEFURNITURE_STATE_UNINSTALL	)
	{
		//처음설치가 아닌 설치시도는 팝업창없이 바로 설치시도   
		if( ! RequestInstallCurDeco() )
			return FALSE;
	}

	return TRUE;
}

//위치설정 
void cHousingMgr::StartDecoMove()
{
	if(! m_pDecoObj ) 
		return;

	m_bDecoMove = TRUE;							
	m_PickAnotherObjHandle = NULL;
}

void cHousingMgr::EndDecoMove()
{
	//g_pExecutive->GetGeometry()->SetDrawDebugFlag(0);

	m_bDecoMove = FALSE;
	//m_PickAnotherObjHandle = NULL;
	//m_vDecoPos.x = m_vDecoPos.y = m_vDecoPos.z = 0.0f;
}

//회전설정
void cHousingMgr::StartDecoAngle()
{
	CHousingRevolDlg* pRevolDlg = GAMEIN->GetHousingRevolDlg();

	if( !pRevolDlg ) 
		return;

	m_bDecoAngle = TRUE;

	LONG x = MOUSE->GetMouseX() - 100;
	LONG y = MOUSE->GetMouseY() + 15;

	pRevolDlg->SetAbsXY( x, y );

	pRevolDlg->SetActive(TRUE);
	pRevolDlg->SetBarRateFromAngle( m_pDecoObj->GetAngle() );

	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( m_pDecoObj->GetFurnitureInfo()->dwFurnitureIndex );

	if( ! stFurnitureInfo )
		return ;

	if(stFurnitureInfo->bRotatable)
		pRevolDlg->StartRotate();
}

void cHousingMgr::EndDecoAngle()
{
	//회전이 끝날때
	m_bDecoAngle = FALSE;
}

//필드에 설치된 가구설정 변경 시작
BOOL cHousingMgr::StartDecoReset(CObject* pPickObject)
{  
	//꾸미기 모드시에 장식용 가구를 좌클릭시 호출된다. 
	if( !pPickObject )
		return FALSE;

	if( pPickObject->GetEngineObjectType() != eEngineObjectType_Furniture )					// Pick 오브젝트가 가구가 아니면 return 
		return FALSE;

	//090708 pdy 
	cHousingWarehouseDlg* pWarehouseDlg = GAMEIN->GetHousingWarehouseDlg();
	pWarehouseDlg->FocusOnByFurniture( ((CFurniture*) pPickObject)->GetFurnitureInfo() );	//스크롤,텝 등을 현가구로 맞춰준다.

	if( ! CanResetDecorationFuniture((CFurniture*)pPickObject) )
	{
		return FALSE;
	}

	m_pDecoObj = (CFurniture*)pPickObject;

	m_pDecoObj->GetPosition(&m_vDecoPos);			// 처음시작위치를 m_vDecoPos에도 저장						
	m_pDecoObj->GetPosition(&m_vBackupDecoPos);		// Reset되기전에 위치를 백업한다.
	m_fBackupDecoAngle = m_pDecoObj->GetAngle();	// Reset되기전에 회전값을 백업한다.
	m_bDoDecoration = TRUE;

	RemoveWaitForMakingObj(m_pDecoObj);
	MarkingObjZone(m_pDecoObj, FALSE);

	m_pDecoObj->GetEngineObject()->ApplyHeightField(FALSE);
	m_pDecoObj->SetPosition(&m_vDecoPos);
	m_pDecoObj->GetEngineObject()->ApplyHeightField(TRUE);

	//090615 pdy 하우징 알파버그 수정
	m_pDecoObj->GetEngineObject()->SetAlpha(0.5f);
	g_pExecutive->GXOSetZOrder( m_pDecoObj->GetEngineObject()->GetGXOHandle() , 8 );			// 가구에 알파가 들어가면 알파 소팅문제로 Zorder를 8로 설정해준다. 

	PTRLISTPOS pos = m_AlphaHandleList.Find( m_pDecoObj->GetEngineObject()->GetGXOHandle() );	//알파리스트에들어가 있다면 리스트에서 삭제하자 
	if( pos )
		m_AlphaHandleList.RemoveAt( pos );

	StartDecoAngle();

	return TRUE;
}

CFurniture* cHousingMgr::MakeDummyFuniture(stFurniture* pstFuniture)
{
	// 가구설치시 서버에 인증을 받기전에 생성되는 더미 가구 

	if(! pstFuniture )
		return NULL;

	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(pstFuniture->dwFurnitureIndex);

	if( stFurnitureInfo == 0)
		return NULL;

	BASEOBJECT_INFO Baseinfo;
	Baseinfo.dwObjectID =  HOUSE_DUMMYID;
	SafeStrCpy(Baseinfo.ObjectName, stFurnitureInfo->szName, MAX_NAME_LENGTH+1);

	BASEMOVE_INFO moveInfo;
	moveInfo.bMoving = FALSE;
	moveInfo.CurPosition.x = 0;
	moveInfo.CurPosition.y = 0;
	moveInfo.CurPosition.z = 0;

	stFurniture Furniture = *pstFuniture;
	Furniture.dwObjectIndex = HOUSE_DUMMYID;

	CFurniture* pDummy = MakeFuniture(&Baseinfo,&moveInfo,&Furniture);

	return pDummy;
}

CFurniture* cHousingMgr::MakeFuniture(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,stFurniture* pstFuniture)
{
	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(pstFuniture->dwFurnitureIndex);

	if( stFurnitureInfo == 0)
		return NULL;

	CFurniture* pObject = OBJECTMGR->AddFuniture(pBaseObjectInfo,pMoveInfo,pstFuniture);

	//090611 pdy 하우징 알파버그 수정 
	if( pObject && pObject->GetEngineObject()->GetGXOHandle() ) 
	{
		if(	stFurnitureInfo->bHasAlpha )		//자체 알파를 가지고 있는 가구라면 렌더링 우선순위를 뒤로 돌린다.
		{
			g_pExecutive->GXOSetZOrder( pObject->GetEngineObject()->GetGXOHandle() , 8 ); // 이팩트가 9니깐 그보다 한단계 낮은 8로 설정한다.
		}
	}

	return pObject;
}

//가구삭제 
void cHousingMgr::RemoveDeco(DWORD dwObjectID)
{
	CObject* pObject = OBJECTMGR->GetObject(dwObjectID);
	if(pObject)
	{
		if(OBJECTMGR->GetSelectedObjectID() == dwObjectID)
		{
			OBJECTMGR->SetSelectedObjectID(0);
		}

		OBJECTMGR->AddGarbageObject(pObject);
	}

}

//현재 꾸미기 중인 가구 삭제 
void cHousingMgr::RemoveCurDeco()
{
	if( m_pDecoObj)
		RemoveDeco(m_pDecoObj->GetID());
}

//가구정보 삭제 
void cHousingMgr::DestroyFuniture(stFurniture* pDestroyFn)
{
	if( pDestroyFn )
	{
		if( pDestroyFn->wState == eHOUSEFURNITURE_STATE_INSTALL	)
		{
			//혹시라도 삭제될가구가 인스톨되있다면 UnInstall하자
			UnInstallFunitureFromField(pDestroyFn->dwObjectIndex); 
		}

		m_CurHouse.m_dwFurnitureList[pDestroyFn->wCategory][pDestroyFn->wSlot] = 0;
		m_CurHouse.m_dwCategoryNum[pDestroyFn->wCategory]--;					//카테고리에 보유갯수 --

		cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();		

		//090708 pdy DeleteIcon 수정 
		pDlg->DeleteIcon( pDestroyFn );

		m_CurHouse.pFurnitureList.Remove(pDestroyFn->dwObjectIndex);							//창고리스트에서 삭제하고
		m_stFurniturePool.Free(pDestroyFn);									//메모리 해제하자
	}
}


//필드에 인스톨 
BOOL cHousingMgr::InstallFunitureToField(stFurniture* pstFuniture , BOOL bFirstInstall)
{
	if (! pstFuniture )
		return FALSE;

	//090325 필드테이블 추가 
	//필드테이블에 존재하면 재배치, 존재하지 않으면 새로생성 
	CFurniture* pFuniture = m_FieldFurnitureTable.GetData(pstFuniture->dwObjectIndex);	

	//서버에 인증을 받았으니 기존 더미는 지우고 새로만들자.
	RemoveCurDeco();

	//임시테스트 BASEOBJECT_INFO , BASEMOVE_INFO
	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(pstFuniture->dwFurnitureIndex);
	if( ! stFurnitureInfo )
	{
		return FALSE;
	}

	BASEOBJECT_INFO Baseinfo;
	Baseinfo.dwObjectID =  pstFuniture->dwObjectIndex;	
	SafeStrCpy(Baseinfo.ObjectName, stFurnitureInfo->szName, MAX_NAME_LENGTH+1);

	BASEMOVE_INFO moveInfo;
	moveInfo.bMoving = FALSE;
	moveInfo.CurPosition.x = pstFuniture->vPosition.x;
	moveInfo.CurPosition.y = pstFuniture->vPosition.y;
	moveInfo.CurPosition.z = pstFuniture->vPosition.z;

	pFuniture = (CFurniture*)MakeFuniture( &Baseinfo , &moveInfo ,  pstFuniture );

	if( ! pFuniture )
		return FALSE;

	//필드리스트에 Obj를 추가한다
	m_FieldFurnitureTable.Add(pFuniture , pFuniture->GetID());
	m_dwFieldFurnitureNum++;

	//로딩중 인스톨이면 마킹하고 리턴.
	if( bFirstInstall )
	{
		MarkingObjZone(pFuniture, TRUE);
		return TRUE;
	}

	RemoveWaitForMakingObj(pFuniture);

	if( CanMakingToFiled(pFuniture) )			//HERO에 위치에 마킹을 하는지 검사후 
	{
		MarkingObjZone(pFuniture, TRUE);		//마킹가능하면 마킹
	}
	else
	{
		AddWaitForMakingObj(pFuniture);			//불가능하면 대기리스트에 ADD
	}

	m_pDecoObj = NULL;							

	EndDecoration();							//설치가 완료되면 가구 꾸미기를 종료하자 

	return TRUE;
}

BOOL cHousingMgr::UpdateFunitere(stFurniture* pstFuniture)
{
	if (! pstFuniture )
		return FALSE;

	CFurniture* pFuniture = m_FieldFurnitureTable.GetData(pstFuniture->dwObjectIndex);	
	if(! pFuniture) 
		return FALSE;

	//090608 pdy 마킹관련 버그 수정
	if( IsHouseOwner() == FALSE )
	{
		//집주인일경우 업데이트 요청을 하기전에 마킹을 해제하고 보넨다.
		//그러나 3자일경우엔 마킹이 되어있으므로 업데이트가 왔을때 갱신전위치의 마킹을 해제해 주어야 한다.
		RemoveWaitForMakingObj(pFuniture);
		MarkingObjZone(pFuniture, FALSE);		//마킹을 해제하고 
	}

	pFuniture->InitFurniture(pstFuniture);

	pFuniture->GetEngineObject()->ApplyHeightField(FALSE);
	pFuniture->SetPosition(&pstFuniture->vPosition);
	pFuniture->GetEngineObject()->ApplyHeightField(TRUE);
	pFuniture->SetAngle(pstFuniture->fAngle);
	pFuniture->GetEngineObject()->SetAlpha(1.0f);
	pFuniture->GetEngineObject()->SetMaterialIndex(pstFuniture->nMaterialIndex);

	//090615 pdy 하우징 알파버그 수정
	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo(pstFuniture->dwFurnitureIndex);
	if( stFurnitureInfo && stFurnitureInfo->bHasAlpha == FALSE )									//자체 알파가 없는 가구는
		g_pExecutive->GXOSetZOrder( pFuniture->GetEngineObject()->GetGXOHandle() , 0 );				//Zorder를 0으로 셋팅 


	if( CanMakingToFiled(pFuniture) )		//HERO에 위치에 마킹을 하는지 검사후 
	{
		MarkingObjZone(pFuniture, TRUE);	//마킹가능하면 마킹
	}
	else
	{
		AddWaitForMakingObj(pFuniture);		//불가능하면 대기리스트에 ADD
	}

	m_pDecoObj = NULL;				

	//설치가 완료되면 꾸미기를 종료하자 
	EndDecoration();

	return TRUE;
}

BOOL cHousingMgr::UnInstallFunitureFromField(DWORD dwObjectIdx)
{
	CFurniture* pFuniture = m_FieldFurnitureTable.GetData(dwObjectIdx);		//필드 테이블에서 가구Obj를 가져온다

	if(!pFuniture)
		return FALSE;

	RemoveWaitForMakingObj(pFuniture);				//마킹 대기자 목록에 있다면 Remove

	MarkingObjZone(pFuniture, FALSE);				//마킹해제 

	RemoveDeco(dwObjectIdx);						//오브젝트를 제거		
	m_FieldFurnitureTable.Remove( dwObjectIdx );	//필드테이블에서 제거
	m_dwFieldFurnitureNum--;						//필드에 가구숫자 감소 

	m_pDecoObj = NULL;

	if(m_CurHouse.HouseInfo.dwOwnerUserIndex == TITLE->GetUserIdx()) // NYJ
		EndDecoration();

	return TRUE;
}

void cHousingMgr::MarkingObjZone(CObject* pObject, bool bMakingObjZone)
{
	if( ! pObject)
		return;

	if( ! IsFieldHeightObject(pObject) )
		return;

	if(! pObject->GetEngineObject()->GetGXOHandle() )
		return;

	stMarkingInfo* pMarkingInfo = ((CFurniture*)pObject)->GetMarkingInfo();

	if( ! pMarkingInfo || ! pMarkingInfo->bUseMaking )
		return;

	GXOBJECT_HANDLE PickUpHANDLE = pObject->GetEngineObject()->GetGXOHandle();

	BOUNDING_BOX* pBoundingBox = NULL; 

	if( pMarkingInfo->bUseResize )
	{
		if( bMakingObjZone ) 
			((CFurniture*)pObject)->UpdateMarkingInfo();

		pBoundingBox = &pMarkingInfo->stResizeBoudingBox ;
	}
	else 
	{
		COLLISION_MESH_OBJECT_DESC CollisionInfo = {0,}; 
		g_pExecutive->GXOGetCollisionMesh( PickUpHANDLE , &CollisionInfo);
		pBoundingBox = &(CollisionInfo.boundingBox);
	}

	// 091016 마킹관련 버그로 겹침 채크 주석처리
	//cPtrList NearObjList; 
	//
	//if( ! bMakingObjZone )
	//{
	//	//마킹을 해제할시에 영역이 겹치는 리스트를 담아둔다.
	//	m_FieldFurnitureTable.SetPositionHead();
	//	CFurniture* pDestObj= NULL;

	//	float fDecoRadius = pMarkingInfo->f2DRadius;
	//	VECTOR3 vDecoPos;
	//	pObject->GetPosition( &vDecoPos );

	//	while( pDestObj = m_FieldFurnitureTable.GetData() )
	//	{
	//		if( ! IsFieldHeightObject(pDestObj)  )
	//			continue;

	//		if( pObject->GetID() == pDestObj->GetFurnitureInfo()->dwObjectIndex )
	//			continue;

	//		stMarkingInfo* pDestMarkingInfo = pDestObj->GetMarkingInfo();

	//		if( ! pDestMarkingInfo || ! pDestMarkingInfo->bUseMaking )
	//			continue;			

	//		float fDestRadius = pDestMarkingInfo->f2DRadius;
	//		VECTOR3 vDestPos ;
	//		pDestObj->GetPosition( &vDestPos );

	//		float fDeltaLength = CalcDistanceXZ(&vDestPos ,&vDecoPos );

	//		if( fDeltaLength <= (fDecoRadius + fDestRadius) ) //구가 겹치면 리스트에 담자  	
	//			NearObjList.AddTail(pDestObj);
	//	}
	//}


	VECTOR3 Temp[4] ={0,};
	Temp[0] = pBoundingBox->v3Oct[1];
	Temp[1] = pBoundingBox->v3Oct[2];
	Temp[2] = pBoundingBox->v3Oct[5];
	Temp[3] = pBoundingBox->v3Oct[6];

	// 박스의 최대 최소 위치를 구한다.
	float fXMin = 100000.0f;
	float fXMax = 0.0f;
	float fZMin = 100000.0f;
	float fZMax = 0.0f;

	for(int i=0;i<4;i++)
	{
		if( Temp[i].x < fXMin )
			fXMin = Temp[i].x;

		if( Temp[i].x > fXMax )
			fXMax = Temp[i].x;

		if( Temp[i].z < fZMin )
			fZMin = Temp[i].z;

		if( Temp[i].z > fZMax )
			fZMax = Temp[i].z;
	}

	//끝부분을 50단위로 반올림 내림 한다.
	fXMin = (float)(DWORD(fXMin / 50) * 50)- 50;
	fXMax = (float)(DWORD(fXMax / 50) * 50)+ 50; 
	fZMin = (float)(DWORD(fZMin / 50) * 50)- 50;
	fZMax = (float)(DWORD(fZMax / 50) * 50)+ 50; 
	CTileManager* pTileManager = MAP->GetTileManager();
	DWORD dwTileWidth = pTileManager->GetTileWidth();

	for( float Curz = fZMax; Curz > fZMin;Curz -= 50.0f ) 
	{
		if( Curz < 0.0f )
			continue;

		for( float Curx = fXMin; Curx < fXMax ; Curx += 50.0f)
		{
			if( Curx < 0.0f )
				continue;

			DWORD CurTile_x =  DWORD((Curx) / 50);
			DWORD CurTile_z =  DWORD((Curz) / 50);

			if( CurTile_z > dwTileWidth || CurTile_x > dwTileWidth)
				continue;

			VECTOR3 MakingShellCenter = {0,};
			MakingShellCenter.x =  float( (CurTile_x * 50.0f) + 25.0f );
			MakingShellCenter.z =  float( (CurTile_z * 50.0f) + 25.0f );
			
			if( ! CheckPercentShellToBoundingBox( pBoundingBox ,&MakingShellCenter) )
				continue;
		
			if( bMakingObjZone ) 
			{
				pTileManager->AddTileAttrByAreaData( NULL, &MakingShellCenter, SKILLAREA_ATTR_BLOCK );

				if( m_bShowTest )
					AddMarkingTest(pObject,&MakingShellCenter);
			}
			else
			{
				// 091016 마킹관련 버그로 겹침 채크 주석처리
				//BOOL bOverlapShell = FALSE;

				//PTRLISTPOS pos = NearObjList.GetHeadPosition();
				//while( pos )
				//{
				//	CFurniture* pDestObj = (CFurniture*)NearObjList.GetNext( pos ) ;

				//	stMarkingInfo* pDestMarkingInfo = pDestObj->GetMarkingInfo();

				//	if( ! pDestMarkingInfo )
				//		continue;

				//	BOUNDING_BOX* pDestBoundingBox = NULL;

				//	if( pDestMarkingInfo->bUseResize )
				//	{
				//		pDestBoundingBox = &pDestMarkingInfo->stResizeBoudingBox;
				//	}
				//	else
				//	{
				//		COLLISION_MESH_OBJECT_DESC DestObjDesc ;
				//		g_pExecutive->GXOGetCollisionMesh(pDestObj->GetEngineObject()->GetGXOHandle(), &DestObjDesc);

				//		pDestBoundingBox = &DestObjDesc.boundingBox;
				//	}

				//	if( CheckPercentShellToBoundingBox( pDestBoundingBox ,&MakingShellCenter) )
				//	{
				//		bOverlapShell = TRUE;
				//		break;
				//	}
				//}

				//if( ! bOverlapShell )
				//{
					pTileManager->RemoveTileAttrByAreaData( NULL, &MakingShellCenter, SKILLAREA_ATTR_BLOCK );
				//}

				if( m_bShowTest )
					ReleaseMarkingTest(pObject);
			}
		}
	}
}

void cHousingMgr::AddWaitForMakingObj(CObject* pObject)
{
	//090608 pdy 마킹관련 버그 수정 
	if( pObject == NULL )
		return;

	if( pObject->GetObjectKind() == eObjectKind_Furniture )
	{
		if( ((CFurniture*)pObject)->GetMarkingInfo()->bUseMaking == FALSE )
			return;

		if( ! IsFieldHeightObject(pObject) )
			return;
	}

	//중복된거면 ADD하지 않는다 
	PTRLISTPOS pos = m_WaitForMakingObjList.GetHeadPosition();
	while( pos )
	{
		CObject* pCurObject = (CObject*)m_WaitForMakingObjList.GetNext( pos ) ;
		if( pCurObject == pObject)
			return ;
	}

	//리스트에 등록 
	m_WaitForMakingObjList.AddTail(pObject);
}

void cHousingMgr::RemoveWaitForMakingObj(CObject* pObject)
{
	//리스트에서 소멸 

	PTRLISTPOS pos = m_WaitForMakingObjList.GetHeadPosition();
	while( pos )
	{
		CObject* pCurObject = (CObject*)m_WaitForMakingObjList.GetNext( pos ) ;

		if( pCurObject == pObject)
		{
			m_WaitForMakingObjList.Remove( pCurObject );
			return ;
		}
	}
}

//090608 pdy 마킹관련 버그 수정
void cHousingMgr::FindOverapMarkingToHeroObj(cPtrList* pResultList)
{
	//HERO의 영역과 충돌되는 곳에 마킹된 오브젝트리스트를 리턴

	if(! pResultList) 
		return;

	m_FieldFurnitureTable.SetPositionHead();

	for(CFurniture* pFnObject = m_FieldFurnitureTable.GetData();
		0 < pFnObject;
		pFnObject = m_FieldFurnitureTable.GetData())
	{
		DWORD dwFunitureIndex = pFnObject->GetFurnitureInfo()->dwFurnitureIndex;

		if( IsExteriorWall( dwFunitureIndex ) )				// 외벽이라면 Contunue 
			continue;

		if(! CanMakingToFiled( pFnObject ) )				// HERO의 영역과 출돌이 된다면 
		{
			pResultList->AddTail( pFnObject );				// 결과 리스트에 ADD
		}
	}
}

//090608 pdy 마킹관련 버그 수정
void cHousingMgr::CheckAllMarkingObjByHeroZone()
{
	//HERO의 영역과 충돌되는 곳에 마킹된 오브젝트들을 마킹해제후 마킹대기리스트에 등록
	cPtrList OverrapMarkingList;
	FindOverapMarkingToHeroObj(&OverrapMarkingList);		//필드에 설치된 오브젝트중 히어로 위치에 마킹된 오브젝트를 검출한다.

	PTRLISTPOS pos = OverrapMarkingList.GetHeadPosition();
	while( pos )
	{
		CFurniture* pObject = (CFurniture*)OverrapMarkingList.GetNext( pos ) ;

		if( pObject )
		{
			MarkingObjZone( pObject , FALSE );		//마킹을 해제 
			AddWaitForMakingObj(pObject);			//대기목록에 등록 
		}
	}
}

void cHousingMgr::ChangeMaterialFurniture(DWORD dwFurnitureKind,DWORD dwMaterialIndex,BYTE byFlag)
{
	//byFlag 0 FreeView , 1 RollBack , 2 RealChange 
	switch(dwFurnitureKind)
	{
		case ITEM_KIND_FURNITURE_WALLPAPER :
		{
			//벽이면 NotDelte 리스트에서 외벽만찾아 텍스쳐를 바꿔주자
			m_CurHouse.pNotDeleteFurnitureList.SetPositionHead();

			for(stFurniture* pstFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData();
				0 < pstFurniture;
				pstFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData())
			{
				if(! IsExteriorWall(pstFurniture->dwFurnitureIndex) )
					continue;

				CFurniture* pFuniture = (CFurniture*)m_FieldFurnitureTable.GetData(pstFurniture->dwObjectIndex);	

				if(! pFuniture )
					continue;

				if( 0 == byFlag )
				{
					//byFlag 0 FreeView 
					pFuniture->GetEngineObject()->SetMaterialIndex(dwMaterialIndex);
				}
				else if( 1 == byFlag )
				{
					//byFlag 1 RollBack
					pFuniture->GetEngineObject()->SetMaterialIndex( pstFurniture->nMaterialIndex );
				}
				else if( 2 == byFlag )
				{
					//byFlag 1 RealChange
					pstFurniture->nMaterialIndex = dwMaterialIndex;
					pFuniture->GetFurnitureInfo()->nMaterialIndex = dwMaterialIndex;
					pFuniture->GetEngineObject()->SetMaterialIndex(dwMaterialIndex);
				}
			}
		}
		break;
	case ITEM_KIND_FURNITURE_FLOORPAPER:
		{
			//바닥이면 NotDelte 리스트에서 외벽만찾아 텍스쳐를 바꿔주자 
			m_FieldFurnitureTable.SetPositionHead();

			for(stFurniture* pstFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData();
				0 < pstFurniture;
				pstFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData())
			{
				if(! IsFloor(pstFurniture->dwFurnitureIndex) )
					continue;

				CFurniture* pFuniture = (CFurniture*)m_FieldFurnitureTable.GetData(pstFurniture->dwObjectIndex);	

				if(! pFuniture )
					continue;

				if( 0 == byFlag )
				{
					//byFlag 0 FreeView 
					pFuniture->GetEngineObject()->SetMaterialIndex(dwMaterialIndex);
				}
				else if( 1 == byFlag )
				{
					//byFlag 1 RollBack
					pFuniture->GetEngineObject()->SetMaterialIndex( pstFurniture->nMaterialIndex );
				}
				else if( 2 == byFlag )
				{
					//byFlag 1 RealChange
					pstFurniture->nMaterialIndex = dwMaterialIndex;
					pFuniture->GetFurnitureInfo()->nMaterialIndex = dwMaterialIndex;
					pFuniture->GetEngineObject()->SetMaterialIndex(dwMaterialIndex);
				}
			}
		}
		break;
	case ITEM_KIND_FURNITURE_CEILINGPAPER:
		{
			//천장이면 NotDelte 리스트에서 외벽만찾아 텍스쳐를 바꿔주자 
			m_CurHouse.pNotDeleteFurnitureList.SetPositionHead();

			for(stFurniture* pstFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData();
				0 < pstFurniture;
				pstFurniture = m_CurHouse.pNotDeleteFurnitureList.GetData())
			{
				if( Get_HighCategory(pstFurniture->dwFurnitureIndex) != eHOUSE_HighCategory_Decoration || 
					Get_LowCategory(pstFurniture->dwFurnitureIndex)	 != eHOUSE_LowCategory_Decoration_CCover )
					continue;

				CFurniture* pFuniture = (CFurniture*)m_FieldFurnitureTable.GetData(pstFurniture->dwObjectIndex);	

				if(! pFuniture )
					continue;

				if( 0 == byFlag )
				{
					//byFlag 0 FreeView 
					pFuniture->GetEngineObject()->SetMaterialIndex(dwMaterialIndex);
				}
				else if( 1 == byFlag )
				{
					//byFlag 1 RollBack
					pFuniture->GetEngineObject()->SetMaterialIndex( pstFurniture->nMaterialIndex );
				}
				else if( 2 == byFlag )
				{
					//byFlag 1 RealChange
					pstFurniture->nMaterialIndex = dwMaterialIndex;
					pFuniture->GetFurnitureInfo()->nMaterialIndex = dwMaterialIndex;
					pFuniture->GetEngineObject()->SetMaterialIndex(dwMaterialIndex);
				}
			}
		}
		break;
	}

}

void cHousingMgr::RideOnPlayerToFuniture(CPlayer* pPlayer)
{
	//090603 pdy 하우징 탑승버그 수정
	if( pPlayer == NULL || pPlayer->IsRideFurniture() || ! pPlayer->GetRideFurnitureID() )
		return;

	const DWORD dwFurnitureID			= pPlayer->GetRideFurnitureID();
	const DWORD dwFurnitureRideSlot = pPlayer->GetRideFurnitureSeatPos();

	if( dwFurnitureID == 0)
		return;

	CFurniture* pFurniture = (CFurniture*)OBJECTMGR->GetObject( dwFurnitureID );
	if(! pFurniture )
		return;

	if(! pFurniture->GetEngineObject()->IsInited() &&
	   ! pPlayer->GetEngineObject()->IsInited()			) 
	   return;

	stFurniture* 	pstFurniture = NULL;
	if( IsHouseOwner() )
	{
		pstFurniture = m_CurHouse.pFurnitureList.GetData(dwFurnitureID);
	}
	else
	{
		pstFurniture = pFurniture->GetFurnitureInfo();
	}

	stFunitureInfo* pstFunitureInfo = GAMERESRCMNGR->GetFunitureInfo(pFurniture->GetFurnitureInfo()->dwFurnitureIndex);

	if(! pFurniture || !pstFunitureInfo )
	{
		return;
	}

	if( ! pstFunitureInfo->dwActionIndex )
	{
		return;
	}

	char RideNodeName[32] = {0,};
	sprintf(RideNodeName,"%s0%d",HOUSE_ATTACHBONENAME , dwFurnitureRideSlot+1  );

	stHouseActionGroupInfo* pGroupInfo = GAMERESRCMNGR->GetHouseActionGroupInfo(pstFunitureInfo->dwActionIndex);

	if(! pGroupInfo )
	{
		return;
	}

	stHouseActionInfo* pActionInfo = NULL;
	for( int j=0; j < pGroupInfo->byActionNum ; j++)
	{
		stHouseActionInfo* pCurActionInfo = GAMERESRCMNGR->GetHouseActionInfo( pGroupInfo->dwActionIndexList[j] );
		if( pCurActionInfo->dwActionType == eHOUSE_ACTION_TYPE_RIDE )
		{
			pActionInfo = pCurActionInfo;
			break;
		}
	}

	if( ! pActionInfo )
	{
		return;
	}

	pFurniture->GetEngineObject()->AttachRider(pPlayer->GetEngineObject() , RideNodeName);
	pstFurniture->dwRidingPlayer[dwFurnitureRideSlot] = pPlayer->GetID();
	pFurniture->InitFurniture(pstFurniture);

	//알파리스트에 존재하는 가구면 리스트에서 삭제해준다.
	PTRLISTPOS FindPos = m_AlphaHandleList.Find( pFurniture->GetEngineObject()->GetGXOHandle() );
	if( FindPos )
	{
		m_AlphaHandleList.RemoveAt( FindPos );

		//090615 pdy 하우징 알파버그 수정
		if( pstFunitureInfo->bHasAlpha == FALSE )											 //자체 알파가 없는 가구는
			g_pExecutive->GXOSetZOrder( pFurniture->GetEngineObject()->GetGXOHandle() , 0 ); //Zorder를 0으로 설정한다.

		pFurniture->GetEngineObject()->SetAlpha(1.0f);
	}

	OBJECTSTATEMGR->StartObjectState(pPlayer, eObjectState_HouseRiding);
	pPlayer->SetRideFurnitureMotion(pActionInfo->dwPlayer_MotionIndex);
	pPlayer->SetBaseMotion();										// 기본모션 재설정
	OBJECTSTATEMGR->InitObjectState( pPlayer );						// 변경된 모션 적용할 수 있도록 상태 초기화

	if( pActionInfo->dwPlayer_EffectIndex )
	{
		OBJECTEFFECTDESC desc( pActionInfo->dwPlayer_EffectIndex );				//플레이어에 이팩트가 있으면 적용 
		pPlayer->AddObjectEffect( pActionInfo->dwPlayer_EffectIndex , &desc, 1 );
	}

	if( pActionInfo->dwFurniture_MotionIndex )
	{
		pFurniture->ChangeMotion( pActionInfo->dwFurniture_MotionIndex ,FALSE);		//가구에 모션이 있다면 적용 
	}

	if( pActionInfo->dwFurniture_EffectIndex )
	{
		OBJECTEFFECTDESC desc( pActionInfo->dwFurniture_EffectIndex );			//가구에 이팩트가 있으면 적용 
		pFurniture->AddObjectEffect( pActionInfo->dwFurniture_EffectIndex , &desc, 1 );
	}

	pPlayer->SetAngle(pFurniture->GetAngle());
	pPlayer->SetRideFurniture(TRUE);

	//090603 pdy 하우징 탑승시 그림자 없애기 추가
	OBJECTMGR->ApplyShadowOption( pPlayer );
}

void cHousingMgr::RideOffPlayerFromFuniture(CPlayer* pPlayer, BOOL bForceRideOff)
{
	//090603 pdy 하우징 탑승버그 수정
	if( pPlayer == NULL || ! pPlayer->IsRideFurniture()  || ! pPlayer->GetRideFurnitureID() )
		return;

	const DWORD dwFurnitureID		= pPlayer->GetRideFurnitureID();
	const DWORD dwFurnitureRideSlot = pPlayer->GetRideFurnitureSeatPos();

	CFurniture* pFurniture = (CFurniture*)OBJECTMGR->GetObject( dwFurnitureID );
	if(! pFurniture )
		return;

	if( pFurniture->GetFurnitureInfo()->dwRidingPlayer[dwFurnitureRideSlot] != pPlayer->GetID() )
		return;

	stFurniture* 	pstFurniture = NULL;
	if( IsHouseOwner() )
	{
		pstFurniture = m_CurHouse.pFurnitureList.GetData(dwFurnitureID);
	}
	else
	{
		pstFurniture = pFurniture->GetFurnitureInfo();
	}
	stFunitureInfo* pstFunitureInfo = GAMERESRCMNGR->GetFunitureInfo(pFurniture->GetFurnitureInfo()->dwFurnitureIndex);

	if(! pFurniture || !pstFunitureInfo )
	{
		return;
	}

	if( ! pstFunitureInfo->dwActionIndex )
	{
		return;
	}

	if( pstFurniture->dwRidingPlayer[dwFurnitureRideSlot] != pPlayer->GetID() )
	{
		return;
	}

	pFurniture->GetEngineObject()->DetachRider(pPlayer->GetEngineObject());
	pstFurniture->dwRidingPlayer[dwFurnitureRideSlot] = 0;
	pFurniture->InitFurniture(pstFurniture);

	if( pPlayer == HERO )					//HERO가 내렸으면
	{
		//090608 pdy 마킹관련 버그 수정
		CheckAllMarkingObjByHeroZone();		//필드에 설치된 오브젝트중 히어로 위치에 마킹된 오브젝트를 검출하여 처리한다.
	}

	pPlayer->SetRideFurnitureID(0);
	pPlayer->SetRideFurnitureSeatPos(0);

	OBJECTSTATEMGR->EndObjectState(pPlayer, eObjectState_HouseRiding);

	// 강제내리기는 이동상태로 만들어주자~
	if(bForceRideOff)
		OBJECTSTATEMGR->StartObjectState(pPlayer, eObjectState_Move);

	pPlayer->SetBaseMotion();											// 기본모션 재설정
	pPlayer->SetAngle(pFurniture->GetAngle());
	pPlayer->SetRideFurniture(FALSE);

	APPEARANCEMGR->InitAppearance( pPlayer );							// 탑승해제 후 다른 Player의 경우 보이지 않아 보이게 함
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//여기서부터는 테스트를위한 코드 
//////////////////////////////////////////////////////////////////////////////////////////////////
void cHousingMgr::ShowAllMarkingTest()
{
	DWORD dwCurTick = GetTickCount();

	if( dwCurTick < ( m_dwLastShowTick + 2000 ) )
		return;

	m_dwLastShowTick = dwCurTick;

	PTRLISTPOS pos = m_MarkinTestList.GetHeadPosition();
	while( pos )
	{
		cHousingMgr::cMarkingTest* pMarkingTest = (cMarkingTest*)m_MarkinTestList.GetNext( pos ) ;
		pMarkingTest->m_pTestShowObj->SetPoint( pMarkingTest->m_pvPos ) ;
	}
}

void cHousingMgr::SetShowTestMaking(bool bShow)
{ 
	m_bShowTest =  bShow;

	if( !bShow ) 
		ReleaseAllMarkingTest();
}	

//테스트를 위한 마킹정보 등록
void cHousingMgr::AddMarkingTest(CObject* pMarkingObj,VECTOR3* Pos)
{
	if(! pMarkingObj )
		return;

	cMarkingTest* pMarkingTest = new cMarkingTest();

	pMarkingTest->m_pTestShowObj = new CMovePoint();
	pMarkingTest->m_pTestShowObj->Init();

	pMarkingTest->m_pvPos = new VECTOR3();
	*pMarkingTest->m_pvPos = *Pos;

	pMarkingTest->m_pMarkinObj = pMarkingObj;

	m_MarkinTestList.AddTail(pMarkingTest);
}

//테스트를 위한 마킹정보 해제
void cHousingMgr::ReleaseMarkingTest(CObject* pMarkingObj)
{
	PTRLISTPOS pos = m_MarkinTestList.GetHeadPosition();
	while( pos )
	{
		cMarkingTest* pMarkingTest = (cMarkingTest*)m_MarkinTestList.GetNext( pos ) ;

		if( pMarkingTest->m_pMarkinObj == pMarkingObj)
		{
			m_MarkinTestList.Remove( pMarkingTest );

			pMarkingTest->m_pTestShowObj->Release();
			SAFE_DELETE( pMarkingTest->m_pTestShowObj );
			SAFE_DELETE( pMarkingTest->m_pvPos );
			SAFE_DELETE( pMarkingTest );

			return;
		}
	}
}
void cHousingMgr::ReleaseAllMarkingTest()
{
	PTRLISTPOS pos = m_MarkinTestList.GetHeadPosition();
	while( pos )
	{
		cMarkingTest* pMarkingTest = (cMarkingTest*)m_MarkinTestList.GetNext( pos ) ;

		pMarkingTest->m_pTestShowObj->Release();
		SAFE_DELETE( pMarkingTest->m_pTestShowObj);
		SAFE_DELETE( pMarkingTest->m_pvPos);
		SAFE_DELETE( pMarkingTest);
	}
	m_MarkinTestList.RemoveAll();
}

