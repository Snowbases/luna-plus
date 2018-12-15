#include "stdafx.h"																	// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.

#include ".\questquickviewdialog.h"													// 퀘스트 알림이 클래스 헤더를 불러온다.

#include "WindowIDEnum.h"															// 윈도우 아이디 이넘 헤더를 불러온다.

#include "./Interface/cWindowManager.h"												// 윈도우 매니져 클래스 헤더를 불러온다.
#include "./Interface/cListDialog.h"												// 리스트 다이얼로그 클래스 헤더를 불러온다.
#include "QuestManager.h"															// 퀘스트 매니져 클래스 헤더를 불러온다.

#include "ChatManager.h"

#include "Quest.h"																	// 퀘스트 클래스 헤더를 불러온다.

#include "..\[CC]Quest\QuestInfo.h"
#include "..\[CC]Quest\QuestString.h"												// 퀘스트 스트링 클래스 헤더를 불러온다.

#include "cMsgBox.h"																// 알림 메시지 박스 클래스 헤더를 불러온다.
#include "QuestDialog.h"															// 퀘스트 다이얼로그 클래스 헤더를 불러온다.
#include "../[CC]Quest/SubQuestInfo.h"
#include "../[CC]Quest/QuestExecute.h"
#include "../[CC]Quest/QuestExecute_Item.h"
#include "ItemManager.h"
#include "GameIn.h"
#include "InventoryExDialog.h"

#include "../Input/Mouse.h"															// 마우스 클래스 헤더를 불러온다.
#include "./Interface/cStatic.h"
#include "./Interface/cButton.h"
#include "NpcScriptDialog.h"

// 080304 ggomgrak --- 퀘스트자동알리미 처리를 위해 추가 (현재 사용되지 않음)
//#include "ChatManager.h"

CQuestQuickViewDialog::CQuestQuickViewDialog(void)									// 생성자 함수.
{
	m_type = WT_QUESTQUICKVIEWDIALOG ;												// 윈도우 타입을 알림이 다이얼로그로 세팅한다.

	m_QuestTreeList.RemoveAll() ;													// 퀘스트 트리 리스트를 모두 비운다.

	m_pViewList = NULL ;															// 트리 리스트 다이얼로그 포인터를 null로 세팅한다.
	m_bIsUpdate = FALSE;
}

CQuestQuickViewDialog::~CQuestQuickViewDialog(void)									// 소멸자 함수.
{
	Release() ;																		// 해제 함수를 호출한다.
}

void CQuestQuickViewDialog::Release()												// 사용한 리소스를 해제하는 함수.
{
	DWORD* pDeleteIdx ;																// 삭제할 퀘스트 인덱스를 받을 포인터를 선언한다.

	PTRLISTPOS pos = NULL ;															// 위치 정보를 받을 포인터를 선언하고 null 처리 한다.
	pos = m_QuestTreeList.GetHeadPosition() ;										// 퀘스트 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while(pos)																		// 위치 정보가 유효할 동안 while문을 돌린다.
	{
		pDeleteIdx = NULL ;															// 삭제할 인덱스를 담을 포인터를 null 처리한다.
		pDeleteIdx = (DWORD*)m_QuestTreeList.GetNext(pos) ;							// 위치에 해당하는 인덱스를 받고, 위치를 다음으로 세팅한다.

		if( !pDeleteIdx ) continue ;												// 퀘스트 인덱스가 유효하지 않으면, continue 한다.

		m_QuestTreeList.Remove( pDeleteIdx ) ;										// 퀘스트 리스트에서 현재 인덱스를 꺼낸다.
	
		delete pDeleteIdx ;															// 현재 퀘스트 인덱스를 삭제한다.
	}

	m_QuestTreeList.RemoveAll() ;													// 퀘스트 리스트를 모두 비운다.
}

void CQuestQuickViewDialog::Linking()												// 링크 함수.
{
	m_pViewList = (cListDialog*)GetWindowForID(QUE_QUICKVIEWLIST) ;					// 트리 리스트 다이얼로그를 링크한다.
	m_pViewList->SetStyle( LDS_FONTSHADOW, 0 ) ;										// 리스트 다이얼로그를 그림자 출력 스타일로 세팅한다.
	// 071018 LYW --- QuestQuickViewDialog : Setting shadow distance of shadow font to use render shadowfont.
	m_pViewList->Set_ShadowDistance(1) ;

	// 100601 ONS 알리미등록창 배경 UI추가
    m_pQuickViewTop		= (cStatic*)GetWindowForID(QUE_QUICKVIEW_TOP);
	m_pQuickViewMiddle	= (cStatic*)GetWindowForID(QUE_QUICKVIEW_MIDDLE);
	m_pQuickViewBottom	= (cStatic*)GetWindowForID(QUE_QUICKVIEW_BOTTOM);

	m_LineHeight		= m_pQuickViewMiddle->GetHeight();

	for( int i = 0; i < MAX_QUICKVIEW_QUEST; i++ )
	{
		m_pQuickViewRemoveBtn[i] = (cButton*)GetWindowForID(QUE_QUICKVIEW_CLOSE_1 + i);
	}

	m_pQuickViewRemoveAllBtn = (cButton*)GetWindowForID(QUE_QUICKVIEW_CLOSE_ALL);
	
}

BOOL CQuestQuickViewDialog::CheckSameQuest(DWORD dwQuestIdx)						// 퀘스트 리스트 내에 같은 퀘스트가 있는지 여부를 반환하는 함수.
{
	BOOL b_Result = FALSE ;															// 결과 값을 담을 변수를 선언하고 값을 false로 세팅한다.

	DWORD* pCheckIdx ;																// 퀘스트 인덱스 정보를 받을 포인터를 선언한다.

	PTRLISTPOS pos = NULL ;															// 위치 정보를 받을 포인터를 선언하고 null 처리 한다.
	pos = m_QuestTreeList.GetHeadPosition() ;										// 퀘스트 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while(pos)																		// 위치 정보가 유효할 동안 while문을 돌린다.
	{
		pCheckIdx = NULL ;															// 퀘스트 인덱스 포인터를 null 처리 한다.
		pCheckIdx = (DWORD*)m_QuestTreeList.GetNext(pos) ;							// 위치에 해당하는 퀘스트 인덱스 정보를 받고, 다음 위치로 세팅한다.

		if( !pCheckIdx ) continue ;													// 퀘스트 인덱스 정보가 유효하지 않으면, continue 한다.

		if( *pCheckIdx != dwQuestIdx ) continue ;									// 인자로 넘어온 인덱스와, 현재 퀘스트 인덱스가 다르면, continue 한다.

		b_Result = TRUE ;															// 결과 값을 true로 세팅한다.

		break ;																		// while문을 빠져나간다.
	}

	return b_Result ;																// 결과 값을 return 처리 한다.
}

// 080304 ggomgrak --- 퀘스트자동알리미 처리를 위해 추가 (현재 사용되지 않음)
void CQuestQuickViewDialog::RegistQuest(DWORD dwQuestIdx)							// 알림이 창에 퀘스트를 등록 시키는 함수.
{
	CQuest* pQuest = QUESTMGR->GetQuest(dwQuestIdx) ;								// 인자로 넘어온 퀘스트 인덱스로 퀘스트 정보를 받는다.

	if( !pQuest ) return ;															// 퀘스트 정보가 유효하지 않으면, return 처리한다.

	m_bIsUpdate = TRUE;

	if( pQuest->GetSubQuestFlag() == 0 )
	{
		DeleteQuest(dwQuestIdx) ;

		return ;
	}

	if( CheckSameQuest(dwQuestIdx) )												// 알림이 창에 인자로 넘어온 퀘스트와 같은 퀘스트가 있으면,
	{
		DeleteQuest(dwQuestIdx) ;													// 퀘스트를 삭제한다.
		return ;																	// return 처리한다.
	}

	int nRegistedCount = 0 ;														// 현재 등록 된 카운트 값을 담을 변수를 선언하고 0으로 세팅한다.
	nRegistedCount = m_QuestTreeList.GetCount() ;									// 퀘스트 트리 리스트의 카운트 값을 받는다.

	if( nRegistedCount >= MAX_QUICKVIEW_QUEST )										// 등록 된 퀘스트 수가 최대 등록 가능수 이상이면,
	{
		char tempBuf[128] = {0} ;													// 퀘스트 증록에 사용할 임시 버퍼를 선언한다.

		sprintf(tempBuf, CHATMGR->GetChatMsg(1465), 
			MAX_QUICKVIEW_QUEST) ;													// 등록 오류 메시지를 생성한다.

		WINDOWMGR->MsgBox( MBI_QUESTQUICKVIEW_FULL, MBT_OK, tempBuf) ;				// 오류 메시지를 출력한다.

		return ;																	// return 처리한다.
	}

	DWORD* pQuestIdx = new DWORD ;													// DWORD 형 데이터를 생성하고 포인터에 담는다.

	*pQuestIdx = dwQuestIdx ;														// 등록하려는 퀘스트 인덱스를 세팅한다.

	m_QuestTreeList.AddTail(pQuestIdx) ;											// 퀘스트 리스트에 추가한다.
}

void CQuestQuickViewDialog::DeleteQuest(DWORD dwQuestIdx) 							// 알림이 창에 있는 퀘스트를 삭제 하는 함수.
{
	DWORD* pDeleteIdx ;																// 삭제할 퀘스트 인덱스를 받을 포인터를 선언한다.

	PTRLISTPOS pos = NULL ;															// 위치 정보를 받을 포인터를 선언하고 null 처리 한다.
	pos = m_QuestTreeList.GetHeadPosition() ;										// 퀘스트 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while(pos)																		// 위치 정보가 유효할 동안 while문을 돌린다.
	{
		pDeleteIdx = NULL ;															// 삭제할 인덱스를 담을 포인터를 null 처리한다.
		pDeleteIdx = (DWORD*)m_QuestTreeList.GetNext(pos) ;							// 위치에 해당하는 인덱스를 받고, 위치를 다음으로 세팅한다.

		if( !pDeleteIdx ) continue ;												// 퀘스트 인덱스가 유효하지 않으면, continue 한다.

		if( *pDeleteIdx != dwQuestIdx ) continue ;									// 인자로 넘어온 인덱스와 현재 인덱스가 다르면, continue 한다.

		m_QuestTreeList.Remove( pDeleteIdx ) ;										// 퀘스트 리스트에서 현재 인덱스를 꺼낸다.
	
		delete pDeleteIdx ;															// 현재 퀘스트 인덱스를 삭제한다.

		break ;																		// while문을 탈출한다.
	}

	ResetQuickViewDialog() ;														// 알림이 창을 새로고침 한다.
}

void CQuestQuickViewDialog::ResetQuickViewDialog()									// 퀘스트 리스트를 새로고침하는 함수.
{
	int nItemCount = 0 ;															// 알림이 리스트의 아이템 카운트를 담을 변수를 선언하고 0으로 세팅한다.
	nItemCount = m_pViewList->GetItemCount() ;										// 알림이 리스트의 아이템 카운트를 받는다.

	m_pViewList->RemoveAll() ;														// 퀘스트 리스트 다이얼로그를 모두 비운다.

	for( int count = 0 ; count < nItemCount ; ++count )								// 알림이 리스트의 아이템 카운트 만큼 for문을 돌린다.
	{
		m_pViewList->SetExtendReduction(LD_REDUCTION) ;								// 리스트 다이얼로그를 축소한다.
	}

	for( count = 0 ; count < MAX_QUICKVIEW_QUEST ; ++count )
	{
		m_pQuickViewRemoveBtn[count]->SetActive( FALSE );
	}

	PTRLISTPOS pos = NULL ;															// 위치 정보를 담을 포인터를 선언하고 null 처리한다.
	pos = m_QuestTreeList.GetHeadPosition() ;										// 퀘스트 리스트를 헤드로 세팅하고, 위치값을 세팅한다.
	if( NULL == pos )
	{
		SetActive(FALSE);
		return;
	}

	DWORD* pCheckIdx ;																// 인덱스 정보를 받을 포인터를 선언한다.
	CQuest* pQuest ;																// 퀘스트 정보를 받을 포인터를 선언한다.
	CSubQuestInfo* pSubQuestInfo ;													// 서브 퀘스트 정보를 받을 포인터를 선언한다.
	SUBQUEST* pSubQuest ;															// 서브 퀘스트를 받을 포인터를 선언한다.
	DWORD dwIndex = 0;
	while(pos)																		// 위치 정보가 유효할 동안 while문을 돌린다.
	{
		pCheckIdx = NULL ;															// 인덱스를 받을 포인터를 null 처리한다.
		pCheckIdx = (DWORD*)m_QuestTreeList.GetNext(pos) ;							// 위치에 해당하는 인덱스를 받고, 위치를 다음으로 세팅한다.

		if( !pCheckIdx ) continue ;													// 인덱스 정보가 유효하지 않으면, continue 한다.

		pQuest = NULL ;																// 퀘스트 정보를 받을 포인터를 null 처리한다.
		pQuest = QUESTMGR->GetQuest(*pCheckIdx) ;									// 현재 인덱스로 퀘스트 매니져에서 퀘스트 정보를 받는다.

		if( !pQuest ) continue ;													// 퀘스트 정보가 유효하지 않으면, continue 한다.

		if( pQuest->IsQuestComplete() || !pQuest->GetSubQuest(0) )					// 만약 완료했거나, 서브 퀘스트 0번 정보가 없으면,
		{
			DeleteQuest(*pCheckIdx) ;												// 퀘스트를 트리에서 삭제한다.
		}

		// 100708 ONS 알리미다이얼로그의 메인퀘스트명에 삭제버튼을 추가한다.
		m_pQuickViewRemoveBtn[dwIndex]->SetAbsY(LONG(m_pViewList->GetAbsY() + m_pViewList->GetItemCount() * 15) );
		m_pQuickViewRemoveBtn[dwIndex]->SetRelXY( LONG(m_pQuickViewRemoveBtn[dwIndex]->GetRelX()), 
												  LONG(m_pViewList->GetRelY() + m_pViewList->GetItemCount() * 15) );
		dwIndex++;

		AddMainString( *pCheckIdx, 0 ) ;											// 퀘스트 제목을 추가한다.

		for( int count = 0 ; count < MAX_SUBQUEST ; ++count )						// 한 퀘스트가 가질 수 있는 최대 수 만큼 for문을 돌린다.
		{
			pSubQuestInfo = NULL ;													// 서브 퀘스트 정보를 받을 포인터를 null 처리한다.
			pSubQuestInfo = pQuest->GetSubQuestInfoArray(count) ;					// 카운트에 해당하는 서브 퀘스트 정보를 받는다.

			if( !pSubQuestInfo ) continue ;											// 서브 퀘스트 정보가 유효하지 않으면, continue 한다.

			pSubQuest = NULL ;														// 서브 퀘스트 정보를 받을 포인터를 null처리 한다.
			pSubQuest = pQuest->GetSubQuest(count) ;								// 카운트에 해당하는 서브 퀘스트 정보를 받는다.

			if(!pSubQuest) continue ;												// 서브 퀘스트 정보가 유효하지 않으면, continue 한다.

			int nSubQuestType = e_SQT_NORMAL ;										// 퀘스트 타입을 담을 변수를 선언하고, 기본으로 세팅한다.

			if( pSubQuest->dwMaxCount > 0 )											// 목표 카운트가 있으면,
			{
				nSubQuestType = e_SQT_HUNT ;										// 사냥 퀘스트이다.
			}
			else																	// 목표 카운트가 없으면,
			{
				if( pSubQuest->dwData > 0 )											// 현재 카운트가 있으면,
				{
					nSubQuestType = e_SQT_COLLECTION ;								// 수집 퀘스트이다.
				}
				else																// 현재 카운트가 없으면,
				{
					nSubQuestType = e_SQT_COMPLETED ;								// 완료 퀘스트 이다.
				}
			}

			AddRemandQuestExecuteString( *pCheckIdx );

			AddSubString( *pCheckIdx, count, nSubQuestType ) ;						// 서브 퀘스트 스트링을 추가한다.
		}
	}

	UpdateQuickView();
}

void CQuestQuickViewDialog::AddMainString(DWORD dwQuestIdx, DWORD dwSubQuestIdx)	// 퀘스트 제목을 추가하는 함수.
{
	CQuestString* pString = NULL ;													// 퀘스트 스트링 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pString = QUESTMGR->GetQuestString(dwQuestIdx, dwSubQuestIdx) ;					// 퀘스트 인덱스로, 퀘스트 스트링 정보를 받아온다.

	if( !pString ) return ;															// 퀘스트 스트링 정보가 유효하지 않으면, return 처리 한다.	

	char tempBuf[256] = {0,} ;														// 임시 버퍼를 선언한다.
	GetTitleString( tempBuf, pString ) ;											// 퀘스트 스트링의 제목을 받는다.

	if( strlen(tempBuf) <= 4 ) return ;												// 제목이 유효하지 않으면 return 처리한다.

	m_pViewList->AddItem( tempBuf, RGB(0, 255, 255) ) ;								// 퀘스트 제목을 추가한다.
	m_pViewList->SetExtendReduction(LD_EXTEND) ;									// 리스트 다이얼로그를 확장한다.
}

void CQuestQuickViewDialog::AddSubString(DWORD dwQuestIdx, DWORD dwSubQuestIdx, int nType)		// 서브 퀘스트 제목을 추가하는 함수.
{
	CQuestString* pString = NULL ;													// 퀘스트 스트링 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pString = QUESTMGR->GetQuestString(dwQuestIdx, dwSubQuestIdx) ;					// 퀘스트 인덱스로, 퀘스트 스트링 정보를 받아온다.

	if( !pString ) return ;															// 퀘스트 스트링 정보가 유효하지 않으면, return 처리 한다.	

	CQuest* pQuest = NULL ;															// 퀘스트 정보를 받을 포인터를 선언하고 null 처리한다.
	pQuest = QUESTMGR->GetQuest(dwQuestIdx) ;										// 인덱스에 해당하는 퀘스트 정보를 받는다.

	if( !pQuest ) return ;															// 퀘스트 정보가 유효하지 않으면, return 처리 한다.

	SUBQUEST* pSubQuest ;															// 서브 퀘스트 정보를 받을 포인터를 선언한다.
	pSubQuest = pQuest->GetSubQuest(dwSubQuestIdx) ;								// 서브 퀘스트 정보를 받는다.

	if( !pSubQuest ) return ;														// 서브 퀘스트 정보가 유효하지 않으면, return 처리 한다.

	CQuestString* pQuestString ;														// 퀘스트 스트링 정보를 받을 포인터를 선언한다.

	pQuestString = NULL ;														// 퀘스트 스트링 정보를 받을 포인터를 null 처리를 한다.
	pQuestString = QUESTMGR->GetQuestString(dwQuestIdx, 0) ;					// 현재 퀘스트의 제목 스트링을 받아온다.

	if( !pQuestString ) return ;

	CQuestInfo* pQuestInfo ;															// 퀘스트 정보를 받을 포인터를 선언한다.

	int nSubQuestCount = 0 ;
	pQuestInfo = QUESTMGR->GetQuestInfo(pQuestString->GetQuestID()) ;				// 퀘스트 번호로 퀘스트 정보를 받는다.

	if( !pQuestInfo ) return ;

	char tempBuf[256] = {0,} ;														// 임시 버퍼를 선언한다.
	strcat(tempBuf, "	- ") ;														// 하이픈을 추가한다.
	GetTitleString( tempBuf, pString ) ;											// 퀘스트 스트링의 제목을 받는다.

	if( strlen(tempBuf) <= 4 ) return ;												// 제목이 유효하지 않으면 return 처리한다.

	char typeBuf[32] = {0, } ;														// 타입에 따른 처리를 위해 타입 버퍼를 선언한다.

	switch(nType)																	// 서브 퀘스트 타입을 확인한다.
	{
	case e_SQT_NORMAL : break ;														// 기본 타입인경우,
	case e_SQT_HUNT :																// 사냥 타입인 경우,
		{
			sprintf(typeBuf, "(%d/%d)", pSubQuest->dwData, pSubQuest->dwMaxCount) ; // 현재/목표 카운트를 추가해 준다.
		}
		break ;

	case e_SQT_COLLECTION :															// 수집 타입인 경우,
		{
			sprintf(typeBuf, "(%d)", pSubQuest->dwData) ;							// 핸재 카운트를 추가해 준다.
		}
		break ;

	case e_SQT_COMPLETED :															// 완료 타입인 경우,
		{
		}
		break ;
	}

	strcat(tempBuf, typeBuf) ;

	nSubQuestCount = pQuestInfo->GetSubQuestCount() ;							// 서브 퀘스트 수를 받는다.

	// 100422 ShinJS --- 퀘스트 요구사항이 완료되었는지 판단한다.
	if( nSubQuestCount - 1 == int(pQuest->GetCurSubQuestIdx()) &&
		pQuest->IsCompletedDemandQuestExecute() )
	{
		strcat(tempBuf, CHATMGR->GetChatMsg(1463)) ;
	}

	// 100726 ONS 서브 퀘스트 툴팁을 추가한다.
	cListDialog::ToolTipTextList toolTipList;
	cPtrList* pDescList = pString->GetDesc();
	if( pDescList )
	{
		char line[MAX_PATH] = {0};
		for( int count = 0; count < pDescList->GetCount(); ++count )
		{
			PTRLISTPOS pos = pDescList->FindIndex(count);
			if( !pos ) continue;						

			ITEM* pDescLine = (ITEM*)pDescList->GetAt(pos);
			if( pDescLine )									
			{
				sprintf(line, "%s", pDescLine->string);
				toolTipList.push_back(line);
			}
		}		
	}

	const WORD TOOLTIP_BACK_IMAGE_INDEX = 149;
	m_pViewList->AddItem(tempBuf, RGB(255, 255, 255), toolTipList, -1, 0, TOOLTIP_BACK_IMAGE_INDEX );

	m_pViewList->SetExtendReduction(LD_EXTEND) ;									// 리스트 다이얼로그를 확장한다.
	m_bIsUpdate = TRUE;
}

void CQuestQuickViewDialog::GetTitleString( char* pBuf, CQuestString* pQuestString )// 퀘스트 스트링의 제목 스트링을 얻는 함수.
{
	if( !pQuestString ) return ;													// 퀘스트 스트링 정보가 유효하지 않으면, null을 return 한다.

	if( !pBuf ) return ;

	cPtrList* pList = NULL ;
	pList = pQuestString->GetTitle() ;

	if( !pList ) return ;

	ITEM* pItem ;

	PTRLISTPOS pos = NULL ;
	pos = pList->GetHeadPosition() ;

	while( pos )
	{
		pItem = NULL ;
		pItem = (ITEM*)pList->GetNext(pos) ;

		if( !pItem ) continue ;

		strcat(pBuf, pItem->string) ;
	}
}

void CQuestQuickViewDialog::AddRemandQuestExecuteString( DWORD dwQuestIdx )
{
	CQuest* pQuest = QUESTMGR->GetQuest(dwQuestIdx);
	if( !pQuest )
		return;

	const CQuest::DemandQuestExecuteKeyMap& demandQuestExecuteMap = pQuest->GetDemandQuestExecuteMap();
	for( CQuest::DemandQuestExecuteKeyMap::const_iterator iter = demandQuestExecuteMap.begin() ; iter != demandQuestExecuteMap.end() ; ++iter )
	{
		CQuestExecute* pQuestExecute = iter->second;
		switch( pQuestExecute->GetQuestExecuteKind() )
		{
		case eQuestExecute_GiveItem:
			{
				CQuestExecute_Item* pQuestExecute_Item = (CQuestExecute_Item*)pQuestExecute;

				const DWORD dwItemIdx = pQuestExecute_Item->GetItemKind();
				const DWORD dwNeedCnt = pQuestExecute_Item->GetItemNum();

				ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( dwItemIdx );
				if( !pItemInfo )
					continue;

				char buf[MAX_PATH] = {0,};
				char fmt[MAX_PATH] = {0,};

				const DWORD dwInvenCnt = GAMEIN->GetInventoryDialog()->GetTotalItemDurability( dwItemIdx );
				sprintf( fmt, "	- %s", CHATMGR->GetChatMsg( 2220 ) );
				sprintf( buf, fmt, pItemInfo->ItemName, dwInvenCnt, dwNeedCnt );

				if( dwInvenCnt >= dwNeedCnt )
					continue;

				m_pViewList->AddItem( buf, RGBA_MAKE(255, 255, 255, 255) );
				m_pViewList->SetExtendReduction(LD_EXTEND);
			}
			break;
		}
	}
}

DWORD CQuestQuickViewDialog::ActionEvent(CMouse* mouseInfo)							
{
	DWORD we = WE_NULL;																

	if( !m_bActive )																
	{
		return we;																	
	}

	we = cDialog::ActionEvent(mouseInfo);											

	int nMouseX = mouseInfo->GetMouseX() ;											
	int nMouseY = mouseInfo->GetMouseY() ;											

	// 캡션영역을 제외한 영역을 클릭할경우 그라운드 클릭으로 처리되어야하므로 종료시킨다.
	BOOL bOnCaptionRect = m_pQuickViewTop->PtInWindow(nMouseX,nMouseY);
	BOOL bOnDrawRect = m_pQuickViewMiddle->PtInWindow(nMouseX,nMouseY);
	BOOL bActiveFlag = bOnDrawRect || bOnCaptionRect;

	if( m_pQuickViewMiddle->IsActive() != bActiveFlag	|| 
		m_bIsUpdate										)
	{
		// 알리미창의 Active상태가 바뀔때만 갱신한다. 
		m_pQuickViewTop->SetActive( bActiveFlag );
		m_pQuickViewMiddle->SetActive( bActiveFlag );
		m_pQuickViewBottom->SetActive( bActiveFlag );

		// 리스트가 갱신되면 버튼상태를 변경한다.
		m_pQuickViewRemoveAllBtn->SetActive( bActiveFlag );
		for( int i = 0; i < m_QuestTreeList.GetCount(); i++ )
		{
			m_pQuickViewRemoveBtn[i]->SetActive( bActiveFlag );
		}

		m_bIsUpdate = FALSE;
	}
    	
	if( (we & WE_LBTNCLICK)		&& 
		!bOnCaptionRect			&& 
		bOnDrawRect				)
	{
		// 퀘스트 개별삭제 버튼을 클릭했을 경우, 삭제처리를 수행한다.
		for( int i = 0; i < m_QuestTreeList.GetCount(); i++ )
		{
			if( m_pQuickViewRemoveBtn[i]->PtInWindow(nMouseX,nMouseY) )
			{
				return we;
			}
		}

		// 버튼이외의 영역을 클릭했을경우, 이동처리를 수행한다.
		return NULL;
	}

	return we;
}

// 100601 ONS 퀘스트알리미창을 업데이트한다.
void CQuestQuickViewDialog::UpdateQuickView()
{
	const WORD ItemCount = (WORD)m_pViewList->GetItemCount();
	if( ItemCount == 0 )
	{
		// 알리미가 없으면 다이얼로그를 비활성화.
		SetActive(FALSE);
		return;
	}
	
	if( !IsActive() )
	{
		m_pQuickViewTop->SetActive(FALSE);
		m_pQuickViewMiddle->SetActive(FALSE);
		m_pQuickViewBottom->SetActive(FALSE);

		for( int i = 0; i < m_QuestTreeList.GetCount(); i++ )
		{
			m_pQuickViewRemoveBtn[i]->SetActive(FALSE);
		}
		m_pQuickViewRemoveAllBtn->SetActive(FALSE);

		SetActive( TRUE );
	}
	// 알리미창 배경은 리스트가 추가/삭제될때 이미지사이즈를 계산해서 스케일을 조절해서 표현한다.
	VECTOR2 vScale = {0};
	vScale.x = 1.0f;
	vScale.y = (float)ItemCount;
	m_pQuickViewMiddle->SetScale( &vScale );

	m_pQuickViewMiddle->SetWH(		m_pQuickViewMiddle->GetWidth(),  
									m_LineHeight * ItemCount );	
	
	m_pViewList->SetWH( GetWidth(), m_LineHeight * ItemCount );

	// 리스트개수가 변경됨에따라 BOTTOM이미지의 위치를 갱신시킨다.
	m_pQuickViewBottom->SetAbsXY(	(LONG)m_pQuickViewBottom->GetAbsX(), 
									(LONG)(m_pQuickViewMiddle->GetAbsY() + m_LineHeight * ItemCount) );

	m_pQuickViewBottom->SetRelXY(	(LONG)m_pQuickViewBottom->GetRelX(),  
									(LONG)(m_pQuickViewMiddle->GetRelY() + m_LineHeight * ItemCount) );
}

void CQuestQuickViewDialog::OnActionEvent(LONG lId, void * p, DWORD we)
{
	if( lId >= QUE_QUICKVIEW_CLOSE_1	&& 
		lId <= QUE_QUICKVIEW_CLOSE_10 )
	{
		// 퀘스트 알리미 개별 삭제 처리
		PTRLISTPOS pos = m_QuestTreeList.FindIndex( int(lId - QUE_QUICKVIEW_CLOSE_1) );
		if( pos )
		{
			DWORD* pQuestIndex = (DWORD*)m_QuestTreeList.GetAt(pos);
			if( 0 == pQuestIndex )
			{
				return;
			}

			DeleteQuest(*pQuestIndex);
			// 알리미 배경이미지의 크기를 갱신한다.
			UpdateQuickView();
			// 퀘스트다이얼로그의 리스트를 갱신한다.
			GAMEIN->GetQuestDialog()->RefreshQuestList();
		}
	}
	else if( lId == QUE_QUICKVIEW_CLOSE_ALL )
	{
		// 퀘스트 알리미 일괄 삭제 처리
		PTRLISTPOS pos = m_QuestTreeList.GetHeadPosition() ;
		while(pos)
		{
			DWORD* pDeleteIdx = (DWORD*)m_QuestTreeList.GetNext(pos);
			if( 0 == pDeleteIdx )
			{
				continue;
			}

			m_QuestTreeList.Remove( pDeleteIdx );
			delete pDeleteIdx;
		}
		UpdateQuickView();
		ResetQuickViewDialog();
		GAMEIN->GetQuestDialog()->RefreshQuestList();
	}
}

