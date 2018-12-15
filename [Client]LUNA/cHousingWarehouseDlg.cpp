#include "stdafx.h"
#include "cHousingWarehouseDlg.h"
#include "WindowIDEnum.h"
#include "Interface/cWindowManager.h"
#include "Interface/cIconGridDialog.h"
#include "cHousingMgr.h"
#include "ChatManager.h"
#include "cScrollIconGridDialog.h"
#include "cHousingDecoPointDlg.h"
#include "GameIn.h"
#include "PKManager.h"
#include "Item.h"
#include "cHousingStoredIcon.h"
#include "ItemManager.h"
#include "cMsgBox.h"
#include "ObjectStateManager.h"
#include "ObjectManager.h"

cHousingWarehouseDlg::cHousingWarehouseDlg()
{
	m_type				= WT_HOUSING_WAREHOUSE_DLG;

	m_StoredIconHash.Initialize( 100 );
}

cHousingWarehouseDlg::~cHousingWarehouseDlg()
{
	m_StoredIconHash.RemoveAll();
}
void cHousingWarehouseDlg::Add(cWindow * window)
{
// 071210 LYW --- InventoryExDialog : 인벤토리 확장에 따른 확장아이템의 활성화 여부 처리.
	if(window->GetType() == WT_PUSHUPBUTTON)
	{
		BYTE byCurIdx = 0 ;
		byCurIdx = curIdx1 ;

		AddTabBtn(curIdx1++, (cPushupButton * )window);

        if( byCurIdx >= 2 )
		{
			//window->SetActive(FALSE) ;
			((cPushupButton*)window)->SetActive(FALSE) ;
		}
	}
	else if(window->GetType() == WT_SCROLLICONGRIDDLG)
	{
		BYTE byCurIdx = 0 ;
		byCurIdx = curIdx2 ;

		AddTabSheet(curIdx2++, window);
		((cIconGridDialog*)window)->SetDragOverIconType( WT_ITEM );
	}
	else 
		cDialog::Add(window);
}

DWORD cHousingWarehouseDlg::ActionEvent(CMouse * mouseInfo)
{
	return cTabDialog::ActionEvent( mouseInfo );	
}

void cHousingWarehouseDlg::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	if(! HOUSINGMGR->IsHouseInfoLoadOk() || ! HOUSINGMGR->IsHouseOwner() )
	{
		//하우스 로딩이 되어있지 않으면 닫자 (하우징 맵에 들어갈때만 로딩이 완료)
		//집주인이 아니면 닫자  
		val = FALSE;
	}

	cTabDialog::SetActive(val);
}

void cHousingWarehouseDlg::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();
	cTabDialog::RenderTabComponent();
}

void cHousingWarehouseDlg::Linking()
{
	//m_BtDecorationMode  = (cButton*)GetWindowForID(HOUSING_WH_DECOMODE_BTN);
	//m_BtDecoPoint		= (cButton*)GetWindowForID(HOUSING_WH_DECOPOINT_BTN);
	m_BtLeftScroll		= (cButton*)GetWindowForID(HOUSING_WH_LEFT_BTN);
	m_BtRightScroll		= (cButton*)GetWindowForID(HOUSING_WH_RIGHT_BTN);
	m_BtUpScroll		= (cButton*)GetWindowForID(HOUSING_WH_UP_BTN);
	m_BtDownScroll		= (cButton*)GetWindowForID(HOUSING_WH_DOWN_BTN);
	m_pStatic_Basic		= (cStatic*) GetWindowForID( HOUSING_WH_STATIC_TABBTN_BASIC_TEXTPOS ) ;
	m_pStatic_Push		= (cStatic*) GetWindowForID( HOUSING_WH_STATIC_TABBTN_PUSH_TEXTPOS ) ;

	//090714 pdy 탭버튼 기능 수정
	//최초에 한번 셀렉트탭을 해주자 이유는 탭버튼의 텍스트 위치때문이다
	SelectTab(m_bSelTabNum);
}


void cHousingWarehouseDlg::OnActionEvent(LONG lId, void * p, DWORD we)
{
	switch(lId)
	{
		case HOUSING_WH_LEFT_BTN:
			{
				cScrollIconGridDialog* pDlg = (cScrollIconGridDialog*)GetTabSheet( GetCurTabNum() );
				pDlg->SetWheelProcessXY(1,0);
				pDlg->WheelEventProcess(1);
				pDlg->SetWheelProcessXY(0,1);
				pDlg->SortShowGridIconPos();
			}
			break;
		case HOUSING_WH_RIGHT_BTN:
			{
				cScrollIconGridDialog* pDlg = (cScrollIconGridDialog*)GetTabSheet( GetCurTabNum() );
				pDlg->SetWheelProcessXY(1,0);
				pDlg->WheelEventProcess(-1);
				pDlg->SetWheelProcessXY(0,1);
				pDlg->SortShowGridIconPos();
			}
			break;
		case HOUSING_WH_UP_BTN:
			{
				cScrollIconGridDialog* pDlg = (cScrollIconGridDialog*)GetTabSheet( GetCurTabNum() );
				pDlg->SetWheelProcessXY(0,1);
				pDlg->WheelEventProcess(1);
				pDlg->SortShowGridIconPos();
			}
			break;
		case HOUSING_WH_DOWN_BTN:
			{
				cScrollIconGridDialog* pDlg = (cScrollIconGridDialog*)GetTabSheet( GetCurTabNum() );
				pDlg->SetWheelProcessXY(0,1);
				pDlg->WheelEventProcess(-1);
				pDlg->SetWheelProcessXY(1,0);
				pDlg->SortShowGridIconPos();
			}
			break;
		//case HOUSING_WH_DECOPOINT_BTN:
		//	{
		//		cHousingDecoPointDlg* pDlg = (cHousingDecoPointDlg*) GAMEIN->GetHousingDecoPointDlg();
		//		pDlg->SetActive(TRUE);
		//	}
		//	break;
		case HOUSING_WH_TABDLG1:
		case HOUSING_WH_TABDLG2:
		case HOUSING_WH_TABDLG3:
		case HOUSING_WH_TABDLG4:
		case HOUSING_WH_TABDLG5:
		case HOUSING_WH_TABDLG6:
		case HOUSING_WH_TABDLG7:
		case HOUSING_WH_TABDLG8:
		case HOUSING_WH_TABDLG9:
			{
				if(we == WE_LBTNDBLCLICK)
				{
					DWORD CurTabNum = GetCurTabNum();
					cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(CurTabNum);
					if( ! pGridDlg)
						return; 

					int CurSelCellPos = pGridDlg->GetCurSelCellPos();
					if( CurSelCellPos == -1 )
						return;

					cIcon* pIcon = pGridDlg->GetIconForIdx(CurSelCellPos);
					if(!pIcon) 
						return;

					UseIcon(pGridDlg->GetIconForIdx(CurSelCellPos));
						//if ( 퍼니쳐 리스트[GetCurTabNum()][CurSelCellPos].스테이트 == 보관 ) 
						//보관중이면 꾸미기실행 (더미만들고..)
						//else if( 퍼니쳐 리스트[GetCurTabNum()][CurSelCellPos].스테이트 == 설치 )
								//설치해제 패킷을 보네자 
						//}

				}
			}
			break;
	}
}

BOOL cHousingWarehouseDlg::FakeMoveIcon(LONG x, LONG y, cIcon * icon)
{
	ASSERT(icon);
	if( m_bDisable )	return FALSE;

	if( PKMGR->IsPKLooted() )	return FALSE;	//죽었을때 다막을까.. //KES 040801

	if( icon->GetType() == WT_STALLITEM || icon->GetType() == WT_EXCHANGEITEM ) return FALSE;

	if(icon->GetType() == WT_ITEM)
	{
		if( FALSE == FakeMoveItem(x, y, (CItem *)icon) )
		{
			//090527 pdy 하우징 시스템메세지 [보관해제실패]
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1873 ) ); //1873	"해당 아이템은 이동 할 수 없습니다."
		}
	}
	return FALSE;
}

BOOL cHousingWarehouseDlg::FakeMoveItem(LONG mouseX, LONG mouseY, CItem * pFromItem)
{
	//가구 아이템이아니면 안됨 
	if( pFromItem->GetItemInfo()->SupplyType != ITEM_KIND_FURNITURE ) 
		return FALSE;

	if( pFromItem->IsLocked() ) return FALSE;

		//서플라이 벨류는 퍼니쳐 인덱스로 통일하자. 
	DWORD dwFromItemCategory = Get_HighCategory( pFromItem->GetItemInfo()->SupplyValue );

	if( (dwFromItemCategory) >= eFN_HighCatefory_Max ) 
		return FALSE;


	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(dwFromItemCategory-1);	
	if( ! pGridDlg)
		return FALSE; 

	WORD ToPos = 0;

	//if( dwCategoryTabNum == m_bSelTabNum )
	//{
	//	//같을경우 원하는 위치에 놓을수도 있다 아이템<->보관아이콘 교체가 될수도 있다
	//	//일단 원하는위치가 빈자리일때만 처리해주자 
	//	pGridDlg->GetPositionForXYRef(mouseX,mouseY,ToPos);
	//	cIcon* pIcon = pGridDlg->GetIconForIdx(ToPos);
	//	if( pIcon )
	//	{
	//		//아이템 < - > 보관아이콘 과연 나중에 추가될까? 
	//		return FALSE;
	//	}
	//}
	//else if(HOUSINGMGR->GetStoredFunitureNumByCategory( dwCategoryTabNum+1 ) > MAX_HOUSING_SLOT_NUM)
	//{
	//	//카테고리가 같지안은데 빈자리가 없다면 실패 
	//	return FALSE;
	//}

	DWORD dwCurActivedCategory = GetCurTabNum()+1;

	if( HOUSINGMGR->GetStoredFunitureNumByCategory( dwCurActivedCategory ) >= MAX_HOUSING_SLOT_NUM)
	{
		//현재 열린 텝의 공간이 비어있지 않다면 리턴 
		return FALSE;
	}

	if( dwFromItemCategory != dwCurActivedCategory ) 
	{
		//현재 열린 텝과 아이템이 보관될 텝이 다르다면 
		//보관될 텝이 가득차 있는지 확인
		if( HOUSINGMGR->GetStoredFunitureNumByCategory( dwFromItemCategory ) >= MAX_HOUSING_SLOT_NUM)
		{
			return FALSE;
		}
	}

	//빈곳이 있을경우 빈슬롯위치를 가져와 보관을 요청하자 
	ToPos = HOUSINGMGR->GetBlankSlotIndexFromStoredFunitureListByCategory( dwFromItemCategory );
	return HOUSINGMGR->RequestStoredItem(ToPos,pFromItem);
}

BOOL cHousingWarehouseDlg::AddIcon(stFurniture* pstFuniture)
{
	if( !pstFuniture )
		return FALSE;

	cHousingStoredIcon* pNewIcon = MakeHousingStoredIcon(pstFuniture);

	return AddIcon(pNewIcon);
}

BOOL cHousingWarehouseDlg::AddIcon(cIcon* pIcon)
{
	if(!pIcon || pIcon->GetType() != WT_HOUSING_STORED_ICON)
		return FALSE;

	cHousingStoredIcon* pHsStoredIcon =  (cHousingStoredIcon*)pIcon;

	stFurniture* pFurniture = pHsStoredIcon->GetLinkFurniture();
	if( ! pFurniture)
		return FALSE;

	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet((pFurniture->wCategory-1));
	if( ! pGridDlg )
		return FALSE;

	//090713 pdy 하우징 창고 정렬 기능 추가 -- 맨뒤로 Add후 정렬 
	if( ! pGridDlg->AddIcon( MAX_HOUSING_SLOT_NUM - 1 , pIcon) )
	{
		WINDOWMGR->DeleteWindow(pIcon);
		return FALSE;
	}

	m_StoredIconHash.Add( pHsStoredIcon , pFurniture->dwObjectIndex );

	//Refresh되면 안에서 상태에 맞게 정렬도 해준다. 정렬은 빈칸없이 상태별로 차곡차곡..
	RefreshIcon( pFurniture );

	return TRUE;
}

cHousingStoredIcon* cHousingWarehouseDlg::MakeHousingStoredIcon(stFurniture* pstFuniture)
{
	stFunitureInfo* stFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( pstFuniture->dwFurnitureIndex );
	if(!stFurnitureInfo )
	{
		return NULL;
	}

	cHousingStoredIcon* pNewIcon = new cHousingStoredIcon( pstFuniture );
	pNewIcon->SetSeal(FALSE);

	cImage image;

	long IconID = IG_HS_STOREDICON_START + ( MAX_HOUSING_SLOT_NUM * (pstFuniture->wCategory-1) ) + pstFuniture->wSlot;
	pNewIcon->Init(
		0,
		0,
		DEFAULT_ICONSIZE,
		DEFAULT_ICONSIZE,
		ITEMMGR->GetIconImage(  pstFuniture->dwLinkItemIndex , &image ),
		IconID );
	pNewIcon->SetData( pstFuniture->dwLinkItemIndex );
	WINDOWMGR->AddWindow(pNewIcon);

	return pNewIcon;
}

//BOOL cHousingWarehouseDlg::DeleteIcon(WORD wTapNum , WORD wSlot)
//{	
//	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(wTapNum);
//	if( ! pGridDlg)
//		return FALSE; 
//
//	cIcon* pIcon = pGridDlg->GetIconForIdx(wSlot);
//
//	if(!pIcon )
//		return FALSE;
//
//	pGridDlg->DeleteIcon( pIcon  );
//	WINDOWMGR->DeleteWindow(pIcon);
//
//	return FALSE;
//}

//090708 pdy 기획팀 요청에따라 가구 상태에따른 정렬기능 추가 
BOOL cHousingWarehouseDlg::DeleteIcon(stFurniture* pFurniture)
{
	if( pFurniture == NULL )
		return FALSE;

	WORD wTapNum = pFurniture->wCategory -1 ;

	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(wTapNum);
	if( ! pGridDlg)
		return FALSE; 

	cHousingStoredIcon* pResultIcon = m_StoredIconHash.GetData( pFurniture->dwObjectIndex );
	
	if( pResultIcon == NULL )
		return FALSE;
	
	m_StoredIconHash.Remove( pFurniture->dwObjectIndex );
	pGridDlg->DeleteIcon( pResultIcon );
	WINDOWMGR->DeleteWindow( pResultIcon );

	//아이콘이 삭제되면 해당 탭을 정렬시켜주자.
	SortIconByTapNum( wTapNum );

	return TRUE;
}

void cHousingWarehouseDlg::SelectTab(BYTE idx)
{
	//090714 pdy 탭버튼 기능 수정
	BYTE byOldTab = GetCurTabNum();
	BYTE byNewTab = idx;

	if(  m_pStatic_Basic && m_pStatic_Push )
	{
		// 탭버튼이 상태에따라 이미지가 다르다 (푸쉬될때 우측으로 이동) 
		// 상태에 맞게 텍스트도 이동시키자.
		cPushupButton* pPushBt			  = (cPushupButton*) GetWindowForID( HOUSING_WH_TABBTN1 + byNewTab ) ;
		cPushupButton* pOldPushBt		  = (cPushupButton*) GetWindowForID( HOUSING_WH_TABBTN1 + byOldTab ) ;

		if( pPushBt )
		{
			pPushBt->SetTextXY( m_pStatic_Push->GetRelX() , m_pStatic_Push->GetRelY() );
			pPushBt->SetRenderArea();
		}

		if( pOldPushBt && byOldTab != byNewTab )
		{
			pOldPushBt->SetTextXY( m_pStatic_Basic->GetRelX() , m_pStatic_Basic->GetRelY() );
			pOldPushBt->SetRenderArea();
		}
	}
	cTabDialog::SelectTab(idx);

	cScrollIconGridDialog* pDlg = (cScrollIconGridDialog*)GetTabSheet( idx );
	pDlg->InitScrollXY();
}

void cHousingWarehouseDlg::UseIcon(cIcon* pIcon)
{
	if(!pIcon || pIcon->GetType() != WT_HOUSING_STORED_ICON)
		return;

	cHousingStoredIcon* pStoredIcon = (cHousingStoredIcon*)pIcon;
	stFurniture* pstFuniture = pStoredIcon->GetLinkFurniture();

	if(! pstFuniture)
		return;

	if(HOUSINGMGR->IsDoDecoration() &&
		(pstFuniture->wState == eHOUSEFURNITURE_STATE_UNINSTALL ||
		pstFuniture->wState == eHOUSEFURNITURE_STATE_KEEP))
	{
		HOUSINGMGR->CancelDecoration();
	}
	else if(pstFuniture->wState == eHOUSEFURNITURE_STATE_INSTALL)
	{
		return;
	}

	//091224 NYJ eObjectState_None가 아니면 리턴.
	if( (pstFuniture->wState == eHOUSEFURNITURE_STATE_UNINSTALL ||
		pstFuniture->wState == eHOUSEFURNITURE_STATE_KEEP) &&
		(HERO->GetState() != eObjectState_None &&
		HERO->GetState() != eObjectState_Housing))
	{
		CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(2010) /*“가구를 설치할 수 없는 상태입니다.”*/ );
		return;
	}

	//090527 pdy 하우징 시스템메세지 꾸미기 모드가 아닐시 제한사항 [머터리얼교체]
	if( (pstFuniture->wState == eHOUSEFURNITURE_STATE_UNINSTALL ||
		pstFuniture->wState == eHOUSEFURNITURE_STATE_KEEP) &&
		HERO->GetState() != eObjectState_Housing )
	{
		//CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1878) );	//1878	"꾸미기 모드일때만 사용하실 수 있습니다."
		//return;

		//091224 NYJ 꾸미기모드가 아니면 꾸미기 모드로 변환
		HOUSINGMGR->RequestDecoRationMode();
		HOUSINGMGR->SetDecorationModeTemp();
	}

//	pStoredIcon->SetLock(TRUE) ;								// 사용할 아이템을 잠근다.

	if( pstFuniture->bNotDelete )								//기본설치 품목일경우 아이템사용 없음 
	if(! IsDoor( pstFuniture->dwFurnitureIndex)  )				//기본설치문은 아이템사용가능 
	if(! IsStart( pstFuniture->dwFurnitureIndex) )				//기본설치 스타트도 아이템사용가능 
		return;

	/*
	if( pstFuniture->wState == eHOUSEFURNITURE_STATE_INSTALL )
	{
		//기본설치 품목이나 문류는 설치 불가 
		if( pstFuniture->bNotDelete	|| IsDoor( pstFuniture->dwFurnitureIndex ) )
		{
			//090527 pdy 하우징 시스템메세지 기본제공가구 [설치해제]
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1888 ) );	//1888	"기본 물품은 설치해제 할 수 없습니다."
		}
		////091223 NYJ - 내집창고에서 더블클릭으로 가구회수기능 제거.
		//else
		//{
		//	HOUSINGMGR->RequestUninstallDecoFromUseIcon(pstFuniture);
		//}
		return;
	}
	*/

	//설치중이아니면 가구꾸미기 시작
	HOUSINGMGR->StartDecoration(pstFuniture);
}

//void cHousingWarehouseDlg::RefreshIcon(WORD wTapNum , WORD wSlot)
//{
//	cHousingStoredIcon* pStoredIcon = NULL;
//
//	pStoredIcon =GetStoredIconIcon(wTapNum , wSlot);
//
//	if( pStoredIcon ) 
//		pStoredIcon->Refresh();
//}

//090708 pdy 기획팀 요청에따라 가구 상태에따른 정렬기능 추가 
void cHousingWarehouseDlg::RefreshIcon(stFurniture* pFurniture)
{
	if( pFurniture == NULL )
		return;

	WORD wTapNum = pFurniture->wCategory -1 ;

	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(wTapNum);
	if( ! pGridDlg)
		return; 

	//정렬기능때문에 가구의 실제 슬롯위치와 아이콘UI상의 슬롯위치는 같지 않다. 
	//때문에 아이콘 해시에서 가구 Obj인덱스로 아이콘을 가져오자  

	cHousingStoredIcon* pResultIcon = m_StoredIconHash.GetData( pFurniture->dwObjectIndex );

	if( pResultIcon == NULL )
		return;

	//가구의 상태가 바뀌면 아이콘UI상의 해당탭의 배치정렬을 다시 해주어야 한다. 
	SortIconByTapNum(wTapNum) ;

	pResultIcon->Refresh();

	WORD wPosition = 0; pGridDlg->GetPositionForCell( pResultIcon->GetCellX() ,pResultIcon->GetCellY() );

	if( IsDoor( pFurniture->dwFurnitureIndex ) )
	{
		for(int i = 0; i < MAX_HOUSING_SLOT_NUM ; i++ )
		{
			cHousingStoredIcon* pDoorIcon = (cHousingStoredIcon*)pGridDlg->GetIconForIdx( i );

			if( pDoorIcon == NULL || IsDoor( pDoorIcon->GetLinkFurniture()->dwFurnitureIndex ) == FALSE )
				continue;

			if( pDoorIcon->GetLinkFurniture()->wState == eHOUSEFURNITURE_STATE_INSTALL )
			{
				wPosition = pGridDlg->GetPositionForCell( pDoorIcon->GetCellX() ,pDoorIcon->GetCellY() );
				break;
			}
		}
	}
	else 
	{
		wPosition = pGridDlg->GetPositionForCell( pResultIcon->GetCellX() ,pResultIcon->GetCellY() );
	}
	
	//아이콘 정렬이 된후 선택된 셀위치를 다시 맞추어 준다.
	pGridDlg->SetCurSelCellPos( wPosition );
}

cHousingStoredIcon* cHousingWarehouseDlg::GetStoredIconIcon(WORD wTapNum , WORD wSlot)
{
	cHousingStoredIcon* pStoredIcon = NULL;

	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(wTapNum);
	if( ! pGridDlg)
		return NULL; 

	pStoredIcon =(cHousingStoredIcon*) pGridDlg->GetIconForIdx( wSlot ) ;

	return pStoredIcon;
}

void cHousingWarehouseDlg::FakeDeleteIcon(cHousingStoredIcon* pStoredIcon)
{
	stFurniture* pstFurniture = pStoredIcon->GetLinkFurniture();

	if(! pstFurniture )
		return ;

	if( pstFurniture->bNotDelete )
		return;

	if( pstFurniture->wState == eHOUSEFURNITURE_STATE_INSTALL )
	{
		return;
	}

	Set_QuickSelectedIcon(pStoredIcon) ;

	//090527 pdy 하우징 팝업창 [가구삭제]
	WINDOWMGR->MsgBox( MBI_HOUSE_DESTROY_FUTNITURE_AREYOUSURE , MBT_YESNO, CHATMGR->GetChatMsg(209)) ;
}

//090708 pdy 기획팀 요청에따라 가구 상태에따른 정렬기능 추가 
void cHousingWarehouseDlg::SortIconByTapNum(WORD wTapNum)
{
	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet(wTapNum);
	if( ! pGridDlg)
		return;

	//해당 텝의 모든 아이콘을 지우면서 아이콘 주소를 백업 한다.
	cIcon* TempArr[MAX_HOUSING_SLOT_NUM] = {0,};

	for(int i = 0; i < MAX_HOUSING_SLOT_NUM ; i++ )
	{
		pGridDlg->DeleteIcon( i , &TempArr[i] );
	}

	//아이콘을 백업하였으면 이젠 상태값에 따라 빈칸없이 차곡차곡 정렬해준다. 

	//우선순위 1 : 설치안한 가구 아이콘
	//우선순위 2 : 설치 한 후 해제한 가구 아이콘 
	//우선순위 3 : 설치 중인 가구 아이콘 

	//우선순위에 따른 Temp리스트를 각각 만든다
	cPtrList TempListNo1, TempListNo2, TempListNo3;

	for(int i=0; i < MAX_HOUSING_SLOT_NUM ; i++ )
	{
		if( TempArr[i] == NULL ||  TempArr[i]->GetType() != WT_HOUSING_STORED_ICON )
			continue;

		cHousingStoredIcon* pHsStoredIcon = (cHousingStoredIcon*) TempArr[i] ;

		stFurniture* pLinkFurniture = pHsStoredIcon->GetLinkFurniture();

		if( pLinkFurniture == NULL || pLinkFurniture->wState == eHOUSEFURNITURE_STATE_UNKEEP)
			continue;


		switch(pLinkFurniture->wState)
		{
			case eHOUSEFURNITURE_STATE_KEEP:		//설치 안한 가구 
				{
					TempListNo1.AddTail( TempArr[i] );
				}
				break;
			case eHOUSEFURNITURE_STATE_UNINSTALL :	//설치 한 후 해제한 가구 아이콘
				{
					TempListNo2.AddTail( TempArr[i] );
				}
				break;
			case eHOUSEFURNITURE_STATE_INSTALL:		//설치 중인 가구 아이콘 
				{
					TempListNo3.AddTail( TempArr[i] );
				}
				break;
		}
	}

	//우선순위에 맞게 리스트에 담았다면 순서대로 다시 ADD 
	WORD wIconPos = 0;

	PTRLISTPOS pos = TempListNo1.GetHeadPosition();
	while( pos )
	{
		cIcon* pIcon = (cIcon*)TempListNo1.GetNext( pos ) ;
		pGridDlg->AddIcon( wIconPos , pIcon );
		wIconPos++;
	}

	pos = TempListNo2.GetHeadPosition();
	while( pos )
	{
		cIcon* pIcon = (cIcon*)TempListNo2.GetNext( pos ) ;
		pGridDlg->AddIcon( wIconPos , pIcon );
		wIconPos++;
	}

	pos = TempListNo3.GetHeadPosition();
	while( pos )
	{
		cIcon* pIcon = (cIcon*)TempListNo3.GetNext( pos ) ;
		pGridDlg->AddIcon( wIconPos , pIcon );
		wIconPos++;
	}

	TempListNo1.RemoveAll();
	TempListNo2.RemoveAll();
	TempListNo3.RemoveAll();

}

cHousingStoredIcon* cHousingWarehouseDlg::GetStoredIconByFurniture(stFurniture* pFurniture)
{
	if(! pFurniture )
		return NULL;

	return m_StoredIconHash.GetData( pFurniture->dwObjectIndex );
}

cHousingStoredIcon* cHousingWarehouseDlg::GetStoredIconByObjectIdx(DWORD dwObjectIdx)
{
	return m_StoredIconHash.GetData( dwObjectIdx );
}

void cHousingWarehouseDlg::FocusOnByFurniture(stFurniture* pFurniture)
{
	//   < 가구아이콘에 포커스를 맞춰준다 >  
	// * 탭선택을 가구 종류에 따라 탭에 맞춰주기 
	// * 탭이 바뀌면 탭버튼의 택스트 위치 바꿔주기 
	// * UI의 스크롤을 가구 아이콘의 위치에 맞춰주기

	if( ! pFurniture )
		return;

	cHousingStoredIcon* pIcon = m_StoredIconHash.GetData( pFurniture->dwObjectIndex );

	if( ! pIcon )
		return;

	cScrollIconGridDialog* pGridDlg = (cScrollIconGridDialog*)GetTabSheet( pFurniture->wCategory-1 );

	if( ! pGridDlg )
		return;

	BYTE byOldTab = m_bSelTabNum;
	BYTE byNewTab = pFurniture->wCategory - 1;

	if( byOldTab != byNewTab )								// 현재 탭이 가구의 종류랑 맞지 않는다면 
	{
		if(  m_pStatic_Basic && m_pStatic_Push )			// 탭이 바뀌면 탭버튼 텍스트 위치를 바꿔준다 
		{
			cPushupButton* pPushBt			  = (cPushupButton*) GetWindowForID( HOUSING_WH_TABBTN1 + byNewTab ) ;
			cPushupButton* pOldPushBt		  = (cPushupButton*) GetWindowForID( HOUSING_WH_TABBTN1 + byOldTab ) ;

			if( pPushBt )
			{
				pPushBt->SetTextXY( m_pStatic_Push->GetRelX() , m_pStatic_Push->GetRelY() );
				pPushBt->SetRenderArea();
			}

			if( pOldPushBt )
			{
				pOldPushBt->SetTextXY( m_pStatic_Basic->GetRelX() , m_pStatic_Basic->GetRelY() );
				pOldPushBt->SetRenderArea();
			}
		}

		cTabDialog::SelectTab(byNewTab);						// 가구종류에 맞는 탭을 선택해준다. 
	}

	WORD pos = pGridDlg->GetPositionForCell( pIcon->GetCellX() , pIcon->GetCellY() );

	pGridDlg->InitScrollXY( pos );								//스크롤을 아이콘에 위치에 맞춰준다  
	pGridDlg->SetCurSelCellPos( pos );							//커서를 아이콘 위치에 맞춰준다. 
}
