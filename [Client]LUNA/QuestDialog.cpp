#include "stdafx.h"																	// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.

#include "WindowIDEnum.h"															// 윈도우 아이디 이넘 헤더를 불러온다.

#include "../Input/Mouse.h"															// 마우스 클래스 헤더를 불러온다.

#include "GameIn.h"

#include "ChatManager.h"															// 채팅 매니져 클래스 헤더를 불러온다.
#include "ObjectManager.h"															// 오브젝트 매니져 클래스 헤더를 불러온다.
#include "QuestManager.h"															// 퀘스트 매니져 클래스 헤더를 불러온다.

#include "./Interface/cScriptManager.h"												// 스크립트 매니져 클래스 헤더를 불러온다.
#include "./Interface/cWindowManager.h"												// 윈도우 매니져 클래스 헤더를 불러온다.

#include "../ItemManager.h"

#include "NpcScriptManager.h"														// npc 스크립트 매니져 헤더를 불러온다.

#include "./Interface/cFont.h"														// 폰트 클래스 헤더를 불러온다.
#include "./Interface/cIcon.h"														// 아이콘 클래스 헤더를 불러온다.
#include "./Interface/cIconGridDialog.h"											// 아이콘 그리드 다이얼로그 클래스 헤더를 불러온다.
#include "./Interface/cListDialog.h"												// 리스트 다이얼로그 클래스 헤더를 불러온다.
#include "./Interface/cPushupButton.h"												// 푸시업 버튼 클래스 헤더를 불러온다.
#include "./Interface/cStatic.h"													// 스태틱 클래스 헤더를 불러온다.

#include "cDialogueList.h"															// 다이얼로그 리스트 헤더를 불러온다.
#include "cMsgBox.h"																// 메시지 박스 클래스 헤더를 불러온다.
#include "cPage.h"																	// 페이지 클래스 헤더를 불러온다.
#include "QuestDialog.h"															// 퀘스트 다이얼로그 클래스 헤더를 불러온다.
#include "Quest.h"																	// 퀘스트 클래스 헤더를 불러온다.

#include "..\[CC]Quest\QuestExecute.h"												// 퀘스트 실행 클래스 헤더를 불러온다.
#include "..\[CC]Quest\QuestExecute_Item.h"											// 퀘스트 실행 아이템 클래스 헤더를 불러온다.
#include "..\[CC]Quest\QuestInfo.h"													// 퀘스트 정보 클래스 헤더를 불러온다.
#include "..\[CC]Quest\QuestNpcScript.h"											// 퀘스트 npc 스크립트 클래스 헤더를 불러온다.
#include "..\[CC]Quest\QuestString.h"												// 퀘스트 스트링 클래스 헤더를 불러온다.
#include "..\[CC]Quest\QuestTrigger.h"												// 퀘스트 트리거 클래스 헤더를 불러온다.
#include "..\[CC]Quest\SubQuestInfo.h"												// 서브 퀘스트 정보 클래스 헤더를 불러온다.
#include "..\[CC]Quest\QuestExecute_SelectItem.h"											// 퀘스트 실행 아이템 클래스 헤더를 불러온다.
#include "QuestQuickViewDialog.h"
#include "InventoryExDialog.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestDialog::CQuestDialog()														// 생성자 함수.
{
	m_type	= WT_QUESTDIALOG;														// 퀘스트 다이얼로그의 타입을 WT_QUESTDIALOG로 세팅한다.
	m_SelectedQuestID	= 0;														// 선택 된 퀘스트 아이디를 -1로 세팅한다.
	m_QListCount		= 0;														// 퀘스트 리스트 카운트를 0으로 세팅한다.
	m_ChangedRow		= -1;														// 변경 된 줄 번호를 -1로 세팅한다.
	m_QuestItemPage		= 0;														// 퀘스트 아이템 페이지를 0으로 세팅한다.
	m_TooltipIdx		= -1;														// 툴팁 인덱스를 -1로 세팅한다.
	m_RequitalItemTooltipIdx = -1 ;
	memset(m_QuestList, 0, sizeof(CQuestString*)*MAX_PROCESSQUEST);					// 퀘스트 리스트를 메모리 셋한다.

	m_IndexGenerator.Init( 999, IG_QUESTITEM_STARTINDEX);							// 퀘스트 아이템 시작 인덱스를 초기화 한다.

	m_pQuestCount		= NULL ;													// 퀘스트 보유수를 출력하는 스태틱 포인터를 NULL처리한다.

	m_pNoticeBtn		= NULL ;													// 퀘스트 알림이 버튼 포인터를 NULL 처리 한다.
	m_pGiveupBtn		= NULL ;													// 퀘스트 포기 버튼 포인터를 NULL처리 한다.

	m_pQuestDescList	= NULL ;													// 퀘스트 설명 리스트 다이얼로그 포인터를 NULL 처리 한다.

	m_pRequitalGrid		= NULL ;													// 퀘스트 보상 아이템 다이얼로그 포인터를 NULL 처리 한다.

	m_pRequitalGold		= NULL ;													// 골드 보상 스태틱 포인터를 NULL 처리 한다.
	m_pExperience		= NULL ;													// 경험치 보상 스태틱 포인터를 NULL 처리 한다.

	m_nQuestCount = 0 ;

	m_dwNpcIdx = 0 ;
	m_dwScriptPage = 0 ;
	m_bRequitalType = BYTE(eQuestExecute_TakeItem);
}

CQuestDialog::~CQuestDialog()														// 소멸자 함수.
{
	m_IndexGenerator.Release();														// 인덱스 생성기를 해제한다.

	{
		sQuestItem* pItem;

		PTRLISTPOS pos = NULL ;
		pos = m_QuestItemList.GetHeadPosition() ;

		while(pos)
		{
			pItem = NULL ;
			pItem = (sQuestItem*)m_QuestItemList.GetNext(pos) ;

			if( pItem )
			{
				m_QuestItemList.Remove(pItem) ;

				delete pItem ;
			}
		}

		m_QuestItemList.RemoveAll() ;
	}

	{
		PTRLISTPOS deletePos = m_QuestRequitalList.GetHeadPosition() ;

		while(deletePos)
		{
			RequitalItem* pDeleteItem = (RequitalItem*)m_QuestRequitalList.GetNext(deletePos) ;

			if( pDeleteItem )
			{
				m_QuestRequitalList.Remove(pDeleteItem) ;

				delete pDeleteItem ;
			}
		}

		m_QuestRequitalList.RemoveAll() ;
	}

	{
		// 100414 ONS 선택보상리스트 메모리 해제
		PTRLISTPOS deletePos = m_QuestSelectRequitalList.GetHeadPosition() ;
		while(deletePos)
		{
			RequitalItem* pDeleteItem = (RequitalItem*)m_QuestSelectRequitalList.GetNext(deletePos) ;
			if( pDeleteItem )
			{
				m_QuestSelectRequitalList.Remove(pDeleteItem) ;
				delete pDeleteItem ;
			}
		}

		m_QuestSelectRequitalList.RemoveAll() ;
	}

	{
		QuestTree* pTree ;
		DWORD* pQuestIdx ;

		PTRLISTPOS treePos ;
		PTRLISTPOS idPos ;

		treePos = NULL ;
		treePos = m_QuestTree.GetHeadPosition() ;

		while(treePos)
		{
			pTree = NULL ;
			pTree = (QuestTree*)m_QuestTree.GetNext(treePos) ;

			if( !pTree ) continue ;

			idPos = NULL ;
			idPos = pTree->list.GetHeadPosition() ;

			while(idPos)
			{
				pQuestIdx = NULL ;
				pQuestIdx = (DWORD*)pTree->list.GetNext(idPos) ;

				if( !pQuestIdx ) continue ;

				pTree->list.Remove(pQuestIdx) ;

				delete pQuestIdx ;
			}

			pTree->list.RemoveAll() ;

			m_QuestTree.Remove(pTree) ;

			delete pTree ;
			pTree = NULL ;
		}
		m_QuestTree.RemoveAll() ;
	}
}

void CQuestDialog::DeleteTree(QuestTree* pTree)
{
	if(!pTree)
	{
		return ;
	}

	CQuestString* pDeleteString ;

	PTRLISTPOS pos = NULL ;
	pos = pTree->list.GetHeadPosition() ;

	while(pos)
	{
		pDeleteString = NULL ;
		pDeleteString = (CQuestString*)pTree->list.GetNext(pos) ;

		if( pDeleteString )
		{
			pTree->list.Remove(pDeleteString) ;

			delete pDeleteString ;
			pDeleteString = NULL ;
		}
	}

	pTree->list.RemoveAll() ;

	m_QuestTree.Remove(pTree) ;

	delete pTree ;

	pTree = NULL ;
}

void CQuestDialog::Linking()														// 하위 컨트롤들을 링크하는 함수.
{
	for(int i=0; i<MAX_QUEST_PAGEBTN; ++i)											// 최대 퀘스트 페이지 수 만큼 for문을 돌린다.
	{
		m_pPageBtn[i] = (cPushupButton*)GetWindowForID(QUE_PAGE1BTN+i);				// 퀘스트 아이템 페이지 정보를 링크한다.
	}

	m_pPageBtn[0]->SetPush(TRUE);													// 첫 페이지 버튼을 눌린 상태로 세팅한다.
	
//	m_pTitleBtn = (cButton*)GetWindowForID(QUE_TITLEBTN);
	m_pQuestListDlg = (cListDialog*)GetWindowForID(QUE_QUESTLIST);					// 퀘스트 리스트 정보를 링크한다.
	m_pIconGridDlg = (cIconGridDialog*)GetWindowForID(QUE_ITEM_GRID);				// 퀘스트 아이템의 아이콘 그리드 정보를 링크한다.

	m_pQuestCount = (cStatic*)GetWindowForID(QUE_COUNTSTATIC) ;						// 퀘스트 보유수를 출력하는 스태틱 정보를 링크한다.

	m_pNoticeBtn = (cButton*)GetWindowForID(QUE_NOTICEBTN) ;						// 퀘스트 알림이 버튼 정보를 링크한다.
	m_pGiveupBtn = (cButton*)GetWindowForID(QUE_GIVEUPBTN) ;						// 퀘스트 포기 버튼 정보를 링크한다.

	m_pQuestDescList	= (cListDialog*)GetWindowForID(QUE_DESCDLG) ;				// 퀘스트 설명 리스트 다이얼로그 정보를 링크한다.

	m_pRequitalGrid		= (cIconGridDialog*)GetWindowForID(QUE_REQUITAL_GRID) ;		// 퀘스트 보상 아이템 다이얼로그 정보를 링크한다.

	m_pRequitalGold		= (cStatic*)GetWindowForID(QUE_REQUITAL_GOLD) ;				// 골드 보상 스태틱 포인터를 정보를 링크한다.
	m_pExperience		= (cStatic*)GetWindowForID(QUE_REQUITAL_EXP) ;				// 경험치 보상 스태틱 포인터를 정보를 링크한다.
	
	// 100414 ONS 선택보상 관련 UI추가
	m_pSelectRequitalGrid	= (cIconGridDialog*)GetWindowForID(QUE_SELECT_REQUITAL_GRID) ;
	m_pRequitalBtn			= (cPushupButton*)GetWindowForID(QUE_BTN_REQUITAL_FIX);
	m_pSelectRequitalBtn	= (cPushupButton*)GetWindowForID(QUE_BTN_REQUITAL_SELECT);

	m_pItemSignalOn			= (cStatic*)GetWindowForID(QUE_STATIC_REQUITAL_ON);
	m_pItemSignalOff		= (cStatic*)GetWindowForID(QUE_STATIC_REQUITAL_OFF);
	m_pSelectItemSignalOn	= (cStatic*)GetWindowForID(QUE_STATIC_SELECTREQUITAL_ON);
	m_pSelectItemSignalOff	= (cStatic*)GetWindowForID(QUE_STATIC_SELECTREQUITAL_OFF);

	m_pRequitalBtn->SetPush(TRUE);
	m_pRequitalGrid->SetActive(TRUE);
	m_pSelectRequitalBtn->SetPush(FALSE);
	m_pSelectRequitalGrid->SetActive(FALSE);

}

DWORD CQuestDialog::ActionEvent(CMouse* mouseInfo)										// 퀘스트 다이얼로그의 이벤트를 처리하는 함수.
{
	DWORD we = WE_NULL;																	// 이벤트를 담을 변수를 선언하고 NULL 처리를 한다.

	if( !m_bActive )																	// 퀘스트 다이얼로그가 열려있는 상태가 아니라면, 
	{
		return we;																		// 이벤트를 리턴한다.
	}

	we = cDialog::ActionEvent(mouseInfo);												// 다이얼로그의 이벤트를 받는다.

	int nMouseX = mouseInfo->GetMouseX() ;												// 이벤트가 발생하는 마우스 X좌표를 받는다.
	int nMouseY = mouseInfo->GetMouseY() ;												// 이벤트가 발생하는 마우스 Y좌표를 받는다.
    																					// Show QuestItem Tooltip
	if(m_pIconGridDlg->PtInWindow(nMouseX,nMouseY))										// 마우스가 아이콘 그리드 안에 있으면,
	{
		WORD Pos = WORD(-1);

		if(m_pIconGridDlg->GetPositionForXYRef(nMouseX,nMouseY,Pos))					// 아이콘 그리드 상에서 위치를 받는다. 위치가 유효하면,
		{
			WORD RealPos = WORD(Pos + m_QuestItemPage*VIEW_QUESTITEM_PERPAGE);

			if(m_TooltipIdx != RealPos)													// 툴팁 인덱스가 페이지와 같지 않으면,
			{
				ShowItemToolTip(RealPos);												// 현재 위치의 툴팁을 보여준다.
			}

			return 0;																	// 0을 리턴한다.
		}
	}
	// 100414 ONS 일반보상 / 선택보상 툴팁출력
	if( m_bRequitalType == (BYTE)eQuestExecute_TakeItem &&
		m_pRequitalGrid->PtInWindow(nMouseX,nMouseY))
	{
		WORD Pos = WORD(-1);

		if(m_pRequitalGrid->GetPositionForXYRef(nMouseX,nMouseY,Pos))					// 아이콘 그리드 상에서 위치를 받는다. 위치가 유효하면,
		{
			WORD RealPos = Pos ;														// 페이지를 참조하여 실제 위치를 받는다.
			ShowRequitalItemToolTip(m_QuestRequitalList, RealPos);											// 현재 위치의 툴팁을 보여준다.
			return 0;																	// 0을 리턴한다.
		}
	}


	if(  m_bRequitalType == (BYTE)eQuestExecute_TakeSelectItem &&
		m_pSelectRequitalGrid->PtInWindow(nMouseX,nMouseY))
	{
		WORD Pos = WORD(-1);

		if(m_pSelectRequitalGrid->GetPositionForXYRef(nMouseX,nMouseY,Pos))					// 아이콘 그리드 상에서 위치를 받는다. 위치가 유효하면,
		{
			WORD RealPos = Pos ;														// 페이지를 참조하여 실제 위치를 받는다.
			ShowRequitalItemToolTip(m_QuestSelectRequitalList, RealPos);											// 현재 위치의 툴팁을 보여준다.
			return 0;																	// 0을 리턴한다.
		}
	}

	if( m_pQuestListDlg->PtInWindow(nMouseX, nMouseY) )									// 마우스가 퀘스트 리스트 안에 있으면,
	{
		int nSelectedLine = -1 ;														// 리스트에서 선택 된 라인을 받을 변수를 선언하고 -1로 세팅한다.

		nSelectedLine = m_pQuestListDlg->GetCurSelectedRowIdx() ;						// 퀘스트 리스트에서 선택 된 라인을 받는다.

		m_ChangedRow = nSelectedLine ;													// 색상을 바꿀 라인번호를 세팅한다.

		ITEM* pItem = NULL ;
		pItem = m_pQuestListDlg->GetItem(nSelectedLine) ;

		if( !pItem ) return we ;

		//if( pItem->rgb == RGBA_MAKE(200, 128, 0, 255) )									// 선택 된 줄의 색상이 맵 트리 색상이면,
		if(pItem->nFontIdx == cFont::FONT1)
		{
			if( we & WE_LBTNDBLCLICK )													// 더블 클릭을 했다면,
			{
				QuestTreeStateCheck(nSelectedLine) ;
			}
		}
		else
		{
			if( nSelectedLine != -1 )													// 선택 된 라인이 -1과 같지 않으면,
			{
				DWORD QuestIdx = NULL ;
			
				QuestIdx = m_dwQuestList[nSelectedLine] ;								// 선택 된 라인의 퀘스트 스트링을 받는다.

				if( QuestIdx != 0xFFFFFFFF )											// 퀘스트 스트링 정보가 유효하면,
				{				
					if( we & WE_LBTNCLICK )												// 클릭을 했다면,
					{
						m_SelectedQuestID = QuestIdx ;									// 선택 된 퀘스트 인덱스를 세팅한다.

						for(int i = 0; i < m_pQuestListDlg->GetItemCount(); ++i)
						{
							if(ITEM* const item = m_pQuestListDlg->GetItem(i))
							{
								item->rgb = (m_ChangedRow == i ? RGBA_MAKE(255, 255, 0, 255) : RGBA_MAKE(255, 255, 255, 255));
							}
						}

						QuestDescView(
							QUESTMGR->GetQuestString(m_SelectedQuestID, 0));

					}
				}
			}
		}
	}

	return we;
}

void CQuestDialog::SetQuestCount()
{
	char tempBuf[128] = {0, } ;

	DWORD dwColor = RGBA_MAKE(255, 255, 255, 255) ;

	int nCount = 0 ;

	QuestTree* pTree ;
	DWORD* pQuestIdx ;

	PTRLISTPOS treePos ;
	PTRLISTPOS idPos ;

	treePos = NULL ;
	treePos = m_QuestTree.GetHeadPosition() ;

	while(treePos)
	{
		pTree = NULL ;
		pTree = (QuestTree*)m_QuestTree.GetNext(treePos) ;

		if( !pTree ) continue ;

		idPos = NULL ;
		idPos = pTree->list.GetHeadPosition() ;

		while(idPos)
		{
			pQuestIdx = NULL ;
			pQuestIdx = (DWORD*)pTree->list.GetNext(idPos) ;

			if( !pQuestIdx ) continue ;

			++nCount ;
		}
	}

	if( nCount > MAX_QUEST_COUNT )
	{
		dwColor = RGBA_MAKE(255, 0, 0, 255) ;
	}

	sprintf(tempBuf, "(%d/%d)", nCount, MAX_QUEST_COUNT) ;

	m_pQuestCount->SetStaticText(tempBuf) ;
	m_pQuestCount->SetFGColor(dwColor) ;

	m_nQuestCount = nCount ;
}

void CQuestDialog::SetGold(DWORD dwGold) 
{
	char tempBuf[128] = {0, } ;

	sprintf(tempBuf, "%d", dwGold) ;

	m_pRequitalGold->SetStaticText(tempBuf) ;
}

void CQuestDialog::SetExp(DWORD dwExp)
{
	char tempBuf[128] = {0, } ;

	sprintf(tempBuf, "%d", dwExp) ;

	m_pExperience->SetStaticText(tempBuf) ;
}

void CQuestDialog::Render()															// 렌더 함수.
{
	cDialog::Render();																// 다이얼로그를 렌더한다.

	int Count = 0;																	// 카운트를 선언하고 0으로 세팅한다.
	int	nStart = m_QuestItemPage*VIEW_QUESTITEM_PERPAGE;							// 스타트 인덱스를 세팅한다.
	
	PTRLISTSEARCHSTART(m_QuestItemList, sQuestItem*, pSQuest);						// 퀘스트 아이템 리스트에서 퀘스트 아이템 정보를 찾는다.
		if(!pSQuest)				break;											// 퀘스트 아이템 정보가 유효하지 않으면, 빠져나간다.

		if(Count >= nStart)															// 카운트가 스타트 인덱스 이상이면,
		{
																					// 대충 위치 잡아서 찍는다.
			if(pSQuest->Count > 1)													// 퀘스트 아이템이 하나 이상이면,
			{
				static char nums[4];
				int absX=0;	int absY=0;												// 출력 위치 변수를 선언하고 0으로 세팅한다.

				wsprintf(nums,"%3d", pSQuest->Count);								// 세자리 수 까지 아이템 개수를 세팅한다.

				if(m_pIconGridDlg->GetCellAbsPos(pSQuest->Pos, absX, absY))			// 아이콘 그리드에서 위치 정보가 유효하면,
				{
					// 070209 LYW --- QuestDialog : Modified render part.
					m_renderRect.left	= (LONG)absX+16 ;							// 출력 위치를 세팅한다.
					m_renderRect.top	= (LONG)absY+24 ;
					m_renderRect.right	= 1 ;
					m_renderRect.bottom	= 1 ;
																					// 퀘스트 아이템 개수를 출력한다.
					CFONT_OBJ->RenderFontShadow(0, 1, nums,strlen(nums),&m_renderRect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));
				}
			}
		}
	
		++Count;																	// 카운트를 증가시킨다.

		if(Count >= nStart+VIEW_QUESTITEM_PERPAGE)		break;						// 카운트가 페이지 당 보여지는 아이템 수를 넘어가면, 빠져나온다.

	PTRLISTSEARCHEND																// 리스트 검색을 마친다.

	int Count2 = 0;
	int	nStart2 = 0;

	PTRLISTSEARCHSTART(m_QuestRequitalList, RequitalItem*, pSQuest2) ;
	if( !pSQuest2 || m_bRequitalType != eQuestExecute_TakeItem )			break ;

		if(Count2 >= nStart2)
		{
			if( pSQuest2->nItemCount > 1 )
			{
				static char nums[4] ;
				int absX=0; int absY=0;

				wsprintf(nums, "%3d", pSQuest2->nItemCount) ;

				if( m_pRequitalGrid->GetCellAbsPos(pSQuest2->wPos, absX, absY))
				{
					// 070209 LYW --- QuestDialog : Modified render part.
					m_renderRect.left	= (LONG)absX+16 ;							// 출력 위치를 세팅한다.
					m_renderRect.top	= (LONG)absY+24 ;
					m_renderRect.right	= 1 ;
					m_renderRect.bottom	= 1 ;
																					// 퀘스트 아이템 개수를 출력한다.
					CFONT_OBJ->RenderFontShadow(0, 1, nums,strlen(nums),&m_renderRect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));
				}
			}
		}

		++Count2;																	// 카운트를 증가시킨다.

		if(Count2 >= nStart2 + MAX_REQUITAL_ICON_CELL)		break;					// 카운트가 페이지 당 보여지는 아이템 수를 넘어가면, 빠져나온다.
	PTRLISTSEARCHEND

	// 100414 ONS 선택보상 아이템 갯수 출력
	Count2 = 0;
	PTRLISTSEARCHSTART(m_QuestSelectRequitalList, RequitalItem*, pSQuest2) ;
	if( !pSQuest2 || m_bRequitalType != eQuestExecute_TakeSelectItem )			break ;

		if(Count2 >= nStart2)
		{
			if( pSQuest2->nItemCount > 1)
			{
				static char nums[4] ;
				int absX=0; int absY=0;

				wsprintf(nums, "%3d", pSQuest2->nItemCount) ;

				if( m_pSelectRequitalGrid->GetCellAbsPos(pSQuest2->wPos, absX, absY))
				{
					// 070209 LYW --- QuestDialog : Modified render part.
					m_renderRect.left	= (LONG)absX+16 ;							// 출력 위치를 세팅한다.
					m_renderRect.top	= (LONG)absY+24 ;
					m_renderRect.right	= 1 ;
					m_renderRect.bottom	= 1 ;
																					// 퀘스트 아이템 개수를 출력한다.
					CFONT_OBJ->RenderFontShadow(0, 1, nums,strlen(nums),&m_renderRect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));
				}
			}
		}

		++Count2;																	// 카운트를 증가시킨다.

		if(Count2 >= nStart2 + MAX_REQUITAL_ICON_CELL)		break;					// 카운트가 페이지 당 보여지는 아이템 수를 넘어가면, 빠져나온다.
	PTRLISTSEARCHEND
}

void CQuestDialog::SubQuestAdd(CQuestString* pQuestString)							// 서브 퀘스트 추가 함수.
{
	if(!pQuestString)		return;													// 퀘스트 스트링 정보가 유효하지 않으면, 리턴처리를 한다.

	QuestTree* pData = new QuestTree;												// 퀘스트 트리 메모리를 할당한다.
	pData->QuestIdx = pQuestString->GetQuestID();									// 퀘스트 인덱스를 세팅한다.
	pData->State = eQTree_Close;													// 퀘스트 트리 상태를 닫혀진 상태로 세팅한다.
	pData->list.AddHead(pQuestString);												// 퀘스트 스트링을 트리 리스트에 추가한다.
	++pData->Count;																	// 퀘스트 트리의 카운트를 증가한다.
	
	m_QuestTree.AddTail(pData);														// 퀘스트 트리 리스트에 트리 정보를 추가한다.
}

void CQuestDialog::QuestTreeStateCheck(int RowIdx)									// 퀘스트 트리 상태를 체크하는 함수.
{
	if(RowIdx<0)			return;													// 줄 인덱스가 0이상이면, 리턴 처리를 한다.

	int mapNum = int(m_dwQuestList[RowIdx]);
	PTRLISTPOS pos = m_QuestTree.GetHeadPosition() ;

	while(pos)
	{
		QuestTree* pTree = (QuestTree*)m_QuestTree.GetNext(pos) ;

		if( !pTree ) continue ;

		if( pTree->nMapNum != mapNum ) continue ;

		if(pTree->State == eQTree_Open)									// 퀘스트 트리가 열린 상태라면,
		{
			pTree->State = eQTree_Close;									// 트리 상태를 닫힘으로 세팅한다.
		}
		else if(pTree->State == eQTree_Close)								// 퀘스트 트리가 닫혀있는 상태라면,
		{
			pTree->State = eQTree_Open;									// 퀘스트 트리를 열림 상태로 세팅한다.
		}

		break;																// 트리 리스트 검색을 빠져나간다.
	}

	RefreshQuestList();	
}

DWORD CQuestDialog::GetQuestIdx(int index)
{
	if( index < 0 || index >= m_QListCount )
	{
		return NULL ;
	}

	return m_dwQuestList[index] ;
}

void CQuestDialog::QuestListAdd(CQuestString* pQuestString, int SubID)				// 퀘스트 리스트에 스트링을 추가하는 함수.
{	
	if(!pQuestString)			return;												// 퀘스트 스트링 정보가 유효하지 않으면, 리턴 처리를 한다.

	ITEM* pQStr = 0;
	char buf[256] = {0};
	PTRLISTPOS spos = pQuestString->GetTitle()->GetHeadPosition();

	while(spos)																		// 위치 정보가 유효할 동안 while문을 돌린다.
	{
		pQStr = (ITEM*)pQuestString->GetTitle()->GetNext(spos);					// 위치 정보에 해당하는 퀘스트 스트링 정보를 받는다.

		if(pQStr)																	// 스트링 정보가 유효하면,
		{
			if( pQuestString->IsTreeTitle() )
			{
				pQStr->rgb = RGBA_MAKE(200, 128, 0, 255) ;
				pQStr->nFontIdx = cFont::FONT1 ;
			}
			else
			{
				if( m_SelectedQuestID == int(pQuestString->GetQuestID()))
				{
					pQStr->rgb = RGBA_MAKE(0, 0, 255, 255) ;
				}
				else
				{
					pQStr->rgb = RGBA_MAKE(10, 10, 10, 255) ;
				}
				pQStr->nFontIdx = cFont::FONT0 ;
			}

			if(pQuestString->IsTreeTitle())										// 스트링이 트리 타이틀이라면,
			{
				sprintf(buf, "* ") ;												// 임시 버퍼를 *로 세팅한다.
			}
			else																	// 서브 아이디가 있으면,
			{
				BOOL bResult = FALSE ;
				CQuestQuickViewDialog* pQuickView = NULL ;
				pQuickView = GAMEIN->GetQuestQuickViewDialog() ;

				if( !pQuickView ) continue ;

				DWORD* pCheckIdx = NULL ;

				cPtrList* pCheckList = NULL ;
				pCheckList = pQuickView->GetViewList() ;

				if( !pCheckList ) continue ;
				
				PTRLISTPOS checkPos = NULL ;
				checkPos = pCheckList->GetHeadPosition() ;

				while(checkPos)
				{
					pCheckIdx = NULL ;
					pCheckIdx = (DWORD*)pCheckList->GetNext(checkPos) ;

					if( !pCheckIdx ) continue ;

					if( *pCheckIdx == pQuestString->GetQuestID() )
					{
						bResult = TRUE ;
						break ;
					}
				}



				if( bResult == FALSE )
				{
					sprintf(buf, "	- ");												// 임시 버퍼를 -로 세팅한다.
				}
				else
				{
					sprintf(buf, "	√");												// 임시 버퍼를 -로 세팅한다.
				}
			}

			strcat(buf, pQStr->string);

			int nSubQuestCount = 0 ;
			CQuestInfo* pQuestInfo = QUESTMGR->GetQuestInfo(pQuestString->GetQuestID()) ;				// 퀘스트 번호로 퀘스트 정보를 받는다.

			if( pQuestInfo )
			{
				nSubQuestCount = pQuestInfo->GetSubQuestCount() ;							// 서브 퀘스트 수를 받는다.
			}

			if( pQuestString->IsComplete() || nSubQuestCount == 2 )
			{
				strcat(buf, CHATMGR->GetChatMsg(1463)) ;
			}

			m_pQuestListDlg->AddItem(buf, pQStr->rgb) ;
			memset(buf, 0, 256);													// 임시 버퍼를 메모리 셋 한다.
			
			// ListDialog 
			m_QuestList[m_QListCount] = pQuestString;								// 퀘스트 리스트에 퀘스트 스트링 정보를 대입한다.
			++m_QListCount;															// 퀘스트 리스트 카운트를 증가한다.
		}
	}
}

void CQuestDialog::UpdateQuestDescView(DWORD dwQuestIdx, DWORD dwSubQuestIdx)		// 퀘스트 설명창을 업데이트 하는 함수.
{
	int nQuestStringKey = 0 ;														// 퀘스트 스트링 키 값을 담을 변수를 선언하고 0으로 세팅한다.

	COMBINEKEY(dwQuestIdx, dwSubQuestIdx, nQuestStringKey) ;						// 메인 퀘스트, 서브 퀘스트 인덱스로, 스크링 키 값을 받는다.

	CQuestString* pQuestString = NULL ;												// 퀘스트 스트링을 받을 포인터를 선언하고 NULL 처리를 한다.
		
	pQuestString = QUESTMGR->GetQuestString(dwQuestIdx, dwSubQuestIdx);				// 퀘스트 매니져에서 키 값으로 퀘스트 스트링을 받아온다.

	if( pQuestString )
	{
		QuestDescView(pQuestString) ;												// 퀘스트 설명창을 업데이트 한다.
	}
}

void CQuestDialog::SetQuestPrevInfo(CQuestString* pQuestString)
{
	char tempBuf[128] = {0, } ;

	if( !pQuestString )
	{
		/*sprintf(tempBuf, "[SetQuestPrevInfo] - 퀘스트 스트링 정보가 유효하지 않아, 퀘스트 기본 정보를 세팅하지 못했습니다.") ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	DWORD dwQuestIdx = pQuestString->GetQuestID() ;

	// - 퀘스트 제목 세팅 - 
	CQuestString* pTitleString = NULL ;												// 퀘스트 제목 스트링 정보를 받을 포인터를 선언하고 null처리 한다.
	pTitleString = QUESTMGR->GetQuestString(dwQuestIdx, 0) ;						// 퀘스트 제목 스트링을 받는다.

	if( !pTitleString )
	{
		/*sprintf(tempBuf, "[SetQuestPrevInfo] - (%d)번 퀘스트 스트링을 받지 못해 퀘스트 제목 처리를 하지 못했습니다.", pQuestString->GetQuestID()) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	ITEM* pItem ;

	PTRLISTPOS pos = NULL ;
	pos = pTitleString->GetTitle()->GetHeadPosition();								// 퀘스트 스트링을 해드 위치로 세팅한다.

	while(pos)																		// 위치 정보가 유효할 동안 while문을 돌린다.
	{
		pItem = NULL ;
		pItem = (ITEM*)pTitleString->GetTitle()->GetNext(pos);						// 위치 정보에 해당하는 퀘스트 스트링 정보를 받고, 위치를 다음으로 세팅한다.

		if(!pItem) continue ;														// 퀘스트 스트링 정보가 유효하지 않으면 continue 처리를 한다.

		sprintf(tempBuf, "< %s >", pItem->string) ;									// 임시 버퍼에 퀘스트 제목을 복사한다.
		m_pQuestDescList->AddItem(tempBuf, RGBA_MAKE(255, 150, 0, 255));					// 퀘스트 리스트 다이얼로그에 추가한다.
	}

	m_pQuestDescList->AddItem(" ", 0);												// 퀘스트 리스트에 빈칸을 추가한다.

	CQuest* pQuest = NULL ;
	pQuest = QUESTMGR->GetQuest(dwQuestIdx) ;

	if( !pQuest )
	{
		/*sprintf(tempBuf, "[SetQuestPrevInfo] - (%d)번 퀘스트를 받지 못해서 퀘스트 의뢰자를 세팅하지 못했습니다.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	DWORD dwCurSubQuestIdx = pQuest->GetCurSubQuestIdx() ;							// 현재 진행중인 서브 퀘스트 인덱스를 받는다.

	if( dwCurSubQuestIdx != 0 )
	{
		--dwCurSubQuestIdx ;
	}

	CSubQuestInfo* pSubQuestInfo = NULL ;											// 서브 퀘스트 정보를 받을 포인터를 선언한다.
	pSubQuestInfo = pQuest->GetSubQuestInfo(dwCurSubQuestIdx) ;

	if( !pSubQuestInfo ) 
	{
		/*sprintf(tempBuf, "[SetQuestPrevInfo] - (%d)번 퀘스트의 (%d)번 서브 퀘스트 정보를 받지 못해서 의뢰자를 세팅하지 못했습니다.", dwQuestIdx, dwCurSubQuestIdx-1) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	// 100408 ShinJS --- 시간제한 퀘스트이고 현재 수행불가능한 시간인경우 다음 시간을 알려준다.
	if( pQuest->HasDateLimit() && !pQuest->IsValidDateTime() )
	{
		memset(tempBuf, 0, sizeof(tempBuf)) ;									// 임시 버퍼를 초기화 한다.
		__time64_t nextTime = pQuest->GetNextDateTime();
		struct tm nextTimeWhen = *_localtime64( & nextTime );

		sprintf(tempBuf, CHATMGR->GetChatMsg( 2215 ), nextTimeWhen.tm_mon + 1, nextTimeWhen.tm_mday, nextTimeWhen.tm_hour, nextTimeWhen.tm_min );		// "다음 수행 가능 시간 : %d월 %d일 %02d:%02d"
		m_pQuestDescList->AddItem(tempBuf, RGBA_MAKE(255, 255, 0, 255));		// 퀘스트 설명 리스트에 추가한다.
		m_pQuestDescList->AddItem(" ", 0);
	}

	// 100518 ShinJS --- 퀘스트의 ScriptData 변경
	m_dwNpcIdx = 0;
	CQuestNpcScript* pQuestNpcScript = pSubQuestInfo->GetQuestNpcScript();
	if( pQuestNpcScript )
	{
		m_dwNpcIdx = pQuestNpcScript->GetNpcIdx();
		m_dwScriptPage = pQuestNpcScript->GetNpcScriptPage();
		m_dwDialogType = pQuestNpcScript->GetDialogType();

		stNPCIDNAME* pNpcIdName = QUESTMGR->GetNpcIdName( m_dwNpcIdx );		// 퀘스트 매니져에서 npc 아이디, 이름을 받는다.
		if( !pNpcIdName ) 
			return;

		memset(tempBuf, 0, sizeof(tempBuf)) ;									// 임시 버퍼를 초기화 한다.
		sprintf(tempBuf, CHATMGR->GetChatMsg( 1446 ), pNpcIdName->name) ;
		m_pQuestDescList->AddItem(tempBuf, RGBA_MAKE(255, 255, 0, 255));		// 퀘스트 설명 리스트에 추가한다.
	}

	CQuestInfo* pQuestInfo = QUESTMGR->GetQuestInfo(dwQuestIdx) ;					// 퀘스트 번호로 퀘스트 정보를 받는다.

	if( !pQuestInfo )																// 퀘스트 정보가 유효하지 않다면,
	{
		/*sprintf(tempBuf, "[SetQuestPrevInfo] - (%d)번 퀘스트 정보를 받아오지 못해 퀘스트 수락 위치를 세팅하지 못했습니다.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;																	// 리턴 처리를 한다.
	}

	MAPTYPE nMapNum = MAPTYPE(pQuestInfo->GetMapNum());
	memset(tempBuf, 0, sizeof(tempBuf)) ;											// 임시 버퍼를 초기화 한다.
	char* pMapName = GetMapName(nMapNum) ;											// 맵 번호에 따른 맵 이름을 받는다.	
	sprintf(tempBuf, CHATMGR->GetChatMsg( 1447 ), pMapName) ;
	m_pQuestDescList->AddItem(tempBuf, RGBA_MAKE(255, 255, 0, 255));				// 퀘스트 설명에 추가한다.
}





void CQuestDialog::RequestInfo(CQuestString* pQuestString)
{
	char tempBuf[128] = {0, } ;

	if( !pQuestString )
	{
		/*sprintf(tempBuf, "[RequestInfo] - 퀘스트 스트링 정보가 유효하지 않아, 의뢰내용을 세팅하지 못했습니다.") ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	DWORD dwQuestIdx = pQuestString->GetQuestID() ;

	CQuest* pQuest = NULL ;
	pQuest = QUESTMGR->GetQuest(dwQuestIdx) ;

	CQuestInfo* pQuestInfo = QUESTMGR->GetQuestInfo(dwQuestIdx) ;				// 퀘스트 번호로 퀘스트 정보를 받는다.

	if( !pQuestInfo )																// 퀘스트 정보가 유효하지 않다면,
	{
		/*sprintf(tempBuf, "[QuestDescView] - (%d)번 퀘스트 정보를 받아오지 못해 퀘스트 설명창을 실행하지 못했습니다.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;																	// 리턴 처리를 한다.
	}

	CSubQuestInfo* pSubQuestInfo = NULL ;											// 서브 퀘스트 정보를 받을 포인터를 선언한다.
	CQuestString* pQString ;

	PTRLISTPOS pos ;

	cPtrList* pTitleList ;
	cPtrList* pDescList ;

	int QuestStringKey = 0 ;

	int nSubQuestCount = pQuestInfo->GetSubQuestCount() ;							// 서브 퀘스트 수를 받는다.

    for( int count = 0 ; count < nSubQuestCount ; ++count )							// 서브 퀘스트 수 만큼 for문을 돌린다.
	{
		pSubQuestInfo = NULL ;													// 서브 퀘스트 정보를 받을 포인터를 null처리 한다.
		pSubQuestInfo = pQuest->GetSubQuestInfoArray(count) ;					// 카운트에 해당하는 서브 퀘스트를 받는다.

		if( !pSubQuestInfo ) continue ;											// 서브 퀘스트 정보가 유효하지 않으면 continue한다.
		
		COMBINEKEY(dwQuestIdx, count, QuestStringKey) ;							// 퀘스트, 서브 퀘스트 인덱스로 퀘스트 스트링을 구한다.

		SUBQUEST* pSubQuest = pQuest->GetSubQuest(count) ;							// 서브 퀘스트 정보를 받는다.

		pQString = NULL ;
		pQString = QUESTMGR->GetQuestString(QuestStringKey);						// 서브 퀘스트 스트링을 받는다.

		if( !pQString )	continue ;													// 퀘스트 스트링 정보가 유효하지 않으면 continue한다.

		// - 서브 퀘스트 타이틀 세팅 -
		pTitleList = NULL ;
		pTitleList = pQString->GetTitle() ;											// 서브 퀘스트의 타이틀 리스트를 받는다.

		if( !pTitleList ) continue ;												// 타이틀 리스트가 유효하지 않으면 continue한다.

		m_pQuestDescList->AddItem(" ", 0);											// 퀘스트 리스트에 빈칸을 추가한다.

		int nTitleCount = 0 ;														// 서브 퀘스트의 타이틀 카운트를 담을 변수를 선언하고 0으로 세팅한다.

		nTitleCount = pTitleList->GetCount() ;										// 서브 퀘스트의 타이틀을 세팅한다.

		for( int count2 = 0 ; count2 < nTitleCount ; ++count2 )						// 서브 타이틀 카운트 만큼 for문을 돌린다.
		{
			pos = NULL ;															// 위치 정보를 받을 포인터를 null처리를 한다.
			pos = pTitleList->FindIndex(count2) ;									// 타이틀의 위치를 받는다.

			if( !pos ) continue ;													// 위치 정보가 유효하지 않으면 continue한다.

			ITEM* pTitleLine = (ITEM*)pTitleList->GetAt(pos) ;						// 타이틀 스트링을 받는다.

			if( !pTitleLine ) continue ;											// 타이틀 스트링이 유효하지 않으면, continue한다.

			if( !pSubQuest ) continue ;												// 서브 퀘스트 정보가 유효하지 않으면 continue한다.

			memset(tempBuf, 0, sizeof(tempBuf)) ;									// 임시 버퍼를 초기화 한다.

			if( pQString->IsComplete() )											// 퀘스트가 완료 되었으면,
			{
				//sprintf(tempBuf, "%s (완료)", pTitleLine->string) ;					// 제목에 완료를 추가한다.
				sprintf(tempBuf, "%s", pTitleLine->string) ;
				strcat(tempBuf, CHATMGR->GetChatMsg(1463)) ;

				m_pQuestDescList->AddItem(tempBuf, RGBA_MAKE(255, 255, 0, 255)) ;	// 제목과 완료를 출력한다.
			}
			else																	// 완료한 퀘스트가 아니면,
			{	
				if( pSubQuest->dwMaxCount == 0 )									// 서브 퀘스트의 토탈 카운트가 0과 같으면,
				{
					if( pSubQuest->dwData == 0 )									// 서브 퀘스트의 현재 카운트가 0과 같으면,
					{
						sprintf(tempBuf, "%s", pTitleLine->string) ;				// 퀘스트 제목만 임시버퍼에 복사한다.
					}
					else															// 서브 퀘스트의 현재 카운트가 0이 아니면,
					{
						sprintf(tempBuf, "%s (%d)", pTitleLine->string, pSubQuest->dwData) ; // 서브 퀘스트의 제목과, 개수를 임시 버퍼에 복사한다.
					}
				}
				else																// 서브 퀘스트의 토탈 카운트가 0이 아니면,
				{
					sprintf(tempBuf, "%s (%d/%d)", pTitleLine->string, pSubQuest->dwData, pSubQuest->dwMaxCount) ; // 서브 퀘스트의 제목과 진행 카운트를 임시 버퍼에 복사한다.
				}										

				m_pQuestDescList->AddItem(tempBuf, RGBA_MAKE(255, 255, 0, 255)) ;		// 세팅된 스트링을 퀘스트 설명 리스트 다이얼로그에 추가한다.
			}
		}

        // - 서브 퀘스트 설명 세팅
		pDescList = pQString->GetDesc() ;										// 서브 퀘스트 설명 리스트를 받는다.

		if( !pDescList ) continue ;												// 설명 리스트 정보가 유효하지 않으면, continue한다.

		int nDescCount = 0 ;													// 설명 개수를 받을 변수를 선언하고 0으로 세팅한다.

		nDescCount = pDescList->GetCount() ;									// 설명 리스트의 아이템 수를 받는다.

		for( int count2 = 0 ; count2 < nDescCount ; ++count2 )					// 설명 개수 만큼 for문을 돌린다.
		{
			pos = NULL ;														// 위치 정보를 담을 포인터를 null처리를 한다.
			pos = pDescList->FindIndex(count2) ;								// 카운트에 해당하는 위치 정보를 받는다.

			if( !pos ) continue ;												// 위치 정보가 유효하지 않으면 continue한다.

			ITEM* pDescLine = (ITEM*)pDescList->GetAt(pos) ;  					// 설명 스트링을 받는다.

			if( pDescLine )														// 설명 스트링 정보가 유효하다면,						
			{
				m_pQuestDescList->AddItem(pDescLine->string, pDescLine->rgb) ;	// 퀘스트 설명 리스트에 추가를 한다.
			}
		}

		pTitleList = NULL ;														// 제목 리스트 포인터를 null처리를 한다.
		pDescList  = NULL ;														// 설명 리스트 포인터를 null처리를 한다.

		pQString = NULL ;														// 스트링 포인터를 null처리를 한다.			
	}

	m_pQuestDescList->AddItem(" ", 0);												// 퀘스트 리스트에 빈칸을 추가한다.
	m_pQuestDescList->AddItem(CHATMGR->GetChatMsg( 1448 ), RGBA_MAKE(255, 255, 0, 255));				// <의뢰내용>을 추가한다.
	if( m_dwNpcIdx != 0 )
	{
		m_pQuestDescList->AddItem(" ", 0);												// 퀘스트 리스트에 빈칸을 추가한다.
	}

	DWORD scriptPage = m_dwScriptPage;

	if(0 < m_dwDialogType)
	{
		if(CSubQuestInfo* pSubQuestInfo = pQuestInfo->GetSubQuestInfo(0))
		{
			scriptPage = pSubQuestInfo->GetNpcScriptPage(m_dwNpcIdx);
		}
	}

	while(cPage* pPage = NPCSCRIPTMGR->GetPage(m_dwNpcIdx, scriptPage))
	{
		m_pQuestDescList->AddItemAutoLine(
			NPCSCRIPTMGR->GetDialogueList()->GetDialogue(pPage->GetDialogue()),
			RGBA_MAKE(255, 255, 255, 255));
		m_pQuestDescList->AddItem(
			"",
			0);

		if(HYPERLINK* const pHyperLink = pPage->GetHyperText(0))
		{
			scriptPage = pHyperLink->dwData;
		}
		else
		{
			break;
		}
	}
}




void CQuestDialog::RequitalInfo(CQuestString* pQuestString)
{
	SetGold(0) ;
	SetExp(0) ;
	// 100414 ONS 보상관련 UI초기화
	InitRequitalDlg();

	if( !pQuestString )
	{
		/*sprintf(tempBuf, "[RequitalInfo] - 퀘스트 스트링 정보가 유효하지 않아, 의뢰내용을 세팅하지 못했습니다.") ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	DWORD dwQuestIdx = pQuestString->GetQuestID() ;

	CQuest* pQuest = NULL ;
	pQuest = QUESTMGR->GetQuest(dwQuestIdx) ;

	CQuestInfo* pQuestInfo = QUESTMGR->GetQuestInfo(dwQuestIdx) ;				// 퀘스트 번호로 퀘스트 정보를 받는다.

	if( !pQuestInfo )																// 퀘스트 정보가 유효하지 않다면,
	{
		/*sprintf(tempBuf, "[RequitalInfo] - (%d)번 퀘스트 정보를 받아오지 못해 퀘스트 설명창을 실행하지 못했습니다.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;																	// 리턴 처리를 한다.
	}

	RequitalItem* pDeleteItem = NULL ;

	PTRLISTPOS deletePos = NULL ;
	deletePos = m_QuestRequitalList.GetHeadPosition() ;

	while(deletePos)
	{
		pDeleteItem = NULL ;
		pDeleteItem = (RequitalItem*)m_QuestRequitalList.GetNext(deletePos) ;

		if( !pDeleteItem ) continue ;

		m_pRequitalGrid->DeleteIcon( pDeleteItem->wPos, &pDeleteItem->pIcon ) ;

		pDeleteItem->pIcon->SetActive(FALSE) ;
		WINDOWMGR->AddListDestroyWindow(pDeleteItem->pIcon) ;

		m_IndexGenerator.ReleaseIndex( pDeleteItem->pIcon->GetID() ) ;

		m_QuestRequitalList.Remove(pDeleteItem) ;

		delete pDeleteItem ;
	}

	m_QuestRequitalList.RemoveAll() ;

	// 100314 ONS 선택보상 리스트를 갱신한다.
	deletePos = m_QuestSelectRequitalList.GetHeadPosition() ;
	while(deletePos)
	{
		pDeleteItem = NULL ;
		pDeleteItem = (RequitalItem*)m_QuestSelectRequitalList.GetNext(deletePos) ;
		if( !pDeleteItem ) continue ;

		m_pSelectRequitalGrid->DeleteIcon( pDeleteItem->wPos, &pDeleteItem->pIcon ) ;
		pDeleteItem->pIcon->SetActive(FALSE) ;
		WINDOWMGR->AddListDestroyWindow(pDeleteItem->pIcon) ;
		m_IndexGenerator.ReleaseIndex( pDeleteItem->pIcon->GetID() ) ;
		m_QuestSelectRequitalList.Remove(pDeleteItem) ;
		delete pDeleteItem ;
	}
	m_QuestSelectRequitalList.RemoveAll() ;

	RefreshRequitalItem() ;
	RefreshSelectRequitalItem() ;

	int nSubQuestCount = pQuestInfo->GetSubQuestCount() ;							// 서브 퀘스트 수를 받는다.

	if( nSubQuestCount < 1 )
	{
		/*sprintf(tempBuf, "[RequitalInfo] - (%d)번 퀘스트의 서브 퀘스트 정보가 없어, 보상 아이템 처리를 하지 못했습니다.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;																	// 리턴 처리를 한다.
	}

	CSubQuestInfo* pSubQuestInfo = pQuestInfo->GetSubQuestInfo(nSubQuestCount-1) ;					// 마지막 서브 퀘스트 정보를 받는다.

	if( !pSubQuestInfo )																// 서브 퀘스트 정보가 유효하다면,
	{
		/*sprintf(tempBuf, "[RequitalInfo] - (%d)번 퀘스트의 서브 퀘스트 정보가 없어, 보상 아이템 처리를 하지 못했습니다.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;																	// 리턴 처리를 한다.
	}

	cPtrList& pTriggerList = pSubQuestInfo->GetTriggerList();
	int nTriggerCount = pTriggerList.GetCount();
	CQuestTrigger* pTrigger = 0;
	PTRLISTPOS pos = 0;
	PTRLISTPOS exePos = 0;

	for( int count = 0 ; count < nTriggerCount ; ++count )					// 트리거 카운트 수 만큼 for문을 돌린다.
	{
		pos = pTriggerList.FindIndex(count);

		if( !pos ) continue ;												// 위치 정보가 유효하다면,

		pTrigger = (CQuestTrigger*)pTriggerList.GetAt(pos);

		if( !pTrigger )	 continue ;										// 트리거 정보가 유효하다면,

		cPtrList* pExeList = NULL ;
		pExeList = pTrigger->GetExeList() ;								// 트리거 실행 리스트를 받는다.

		if( !pExeList )	continue ;										// 트리거 실행 리스트 정보가 유효하다면,

		int nExeCount = pExeList->GetCount() ;					// 실행 리스트 카운트를 받는다.

		CQuestExecute* pExecute = NULL ;						// 퀘스트 실행 정보를 받을 포인터를 선언하고 null처리를 한다.

		for(int count2 = 0 ; count2 < nExeCount ; ++count2 )	// 실행 카운트 만큼 for문을 돌린다.
		{
			exePos = NULL ;										// 실행 위치 포인터를 null 처리를 한다.
			exePos = pExeList->FindIndex(count2) ;				// 카운트에 해당하는 실행 위치 정보를 받는다.									

			if( !exePos ) continue ;										// 퀘스트 실행 위치가 유효하면,

			pExecute = (CQuestExecute*)pExeList->GetAt(exePos) ;	// 퀘스트 실행 정보를 받는다.

			if( !pExecute ) continue ;								// 퀘스트 실행 정보가 유효하면,

			CQuestExecute_Item* pExecute_Item = ((CQuestExecute_Item*)pExecute) ;
			DWORD dwExeKind = pExecute->GetQuestExecuteKind() ;	// 실행 타입을 받는다.

			switch(dwExeKind)							// 실행 타입을 확인한다.
			{
			case eQuestExecute_TakeItem :				// 실행 타입이 아이템을 받는 타입이라면,
				{
					ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(pExecute_Item->GetItemKind());

					if( pItemInfo )
					{
						AddRequitalItem(pItemInfo, pExecute_Item->GetItemNum(), BYTE(eQuestExecute_TakeItem)) ;
					}
				}
				break ;

			// 100414 ONS 선택보상아이템 처리 추가.
			case eQuestExecute_TakeSelectItem:
				{
					CQuestExecute_SelectItem* pExecute_selectItem = ((CQuestExecute_SelectItem*)pExecute) ;

					DWORD dwMaxCount = pExecute_selectItem->GetMaxItemCount();
					for( DWORD i = 0; i < dwMaxCount; i++ )
					{
						ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(pExecute_selectItem->GetItemKind(i));
						if( !pItemInfo ) continue;

						AddRequitalItem( pItemInfo, pExecute_selectItem->GetItemNum(i), BYTE(eQuestExecute_TakeSelectItem) );
					}
				}
				break;

			case eQuestExecute_TakeMoney :				// 실행 타입이 골드를 받는 타입이라면,
				{
					SetGold( pExecute_Item->GetItemNum() ) ;
				}
				break ;

			case eQuestExecute_TakeExp :				// 실행 타입이 경험치를 받는 타입이라면,
				{
					SetExp( pExecute_Item->GetItemNum() ) ;
				}
				break ;

			}
		}
	}

	// 100414 ONS 보상텝버튼의 전구 활성화/비활성화처리
	if( m_QuestRequitalList.GetCount() > 0 )
	{
		m_pItemSignalOff->SetActive(FALSE);
		m_pItemSignalOn->SetActive(TRUE);
	}

	if( m_QuestSelectRequitalList.GetCount() > 0 )
	{
		m_pSelectItemSignalOff->SetActive(FALSE);
		m_pSelectItemSignalOn->SetActive(TRUE);
	}
}





void CQuestDialog::QuestDescView(CQuestString* pQuestString)						// 퀘스트 상세 설명을 처리 하는 함수.
{
	if( !m_bActive ) return ;														// 퀘스트 창이 열린 상태가 아니라면, 리턴 처리를 한다.

	if(!pQuestString)																// 인자로 넘어온 퀘스트 스트링 정보가 유요하지 않으면 리턴처리한다.
	{
		m_pQuestDescList->RemoveAll() ;												// 설명 리스트 다이얼로그르 모두 비운다.
		{
			int nCellCount = m_pRequitalGrid->GetCellNum() ;							// 아이템 보상 그리드의 셀 카운트를 받는다.

			cIcon* pIcon ;																// 아이콘 정보를 받을 포인터를 선언한다.

			for(WORD count = 0 ; count < nCellCount ; ++count)
			{
				pIcon = m_pRequitalGrid->GetIconForIdx(count) ;							// 카운트에 해당하는 셀에서 아이콘 정보를 받는다.

				if( pIcon )																// 아이콘 정보가 유효하면,
				{
					m_pRequitalGrid->DeleteIcon(pIcon) ;								// 보상 아이템 그리드에서 아이콘을 삭제한다.
				}
			}

			RequitalItem* item ;														// 보상 아이템의 정보를 받을 포인터를 선언한다.

			PTRLISTPOS pos = m_QuestRequitalList.GetHeadPosition() ;					// 퀘스트 보상 아이템 리스트를 헤드로 세팅하고 위치 정보를 받는다.

			while( pos )																// 위치 정보가 유효할 동안 while문을 돌린다.
			{
				item = NULL ;															// 보상 아이템 정보를 받을 포인터를 null처리 한다.
				item = ( RequitalItem* )m_QuestRequitalList.GetNext( pos ) ;			// 위치에 해당하는 보상아이템 정보를 받고, 위치를 다음으로 세팅한다.

				if( item )																// 아이템 정보가 유효하면,
				{
					WINDOWMGR->AddListDestroyWindow( item->pIcon );						// 윈도우 매니져에 삭제할 윈도우로 현재 아이템의 아이콘을 등록한다.
					SAFE_DELETE( item ) ;												// 아이템 정보를 안전하게 삭제한다.
				}
			}

			m_QuestRequitalList.RemoveAll() ;											// 보상 아이템 리스트를 모두 비운다.
		}
		// 100414 ONS 선택보상정보 삭제
		{
			int nCellCount = m_pSelectRequitalGrid->GetCellNum();

			for( WORD count = 0 ; count < nCellCount ; ++count )
			{
				cIcon* pIcon = m_pSelectRequitalGrid->GetIconForIdx(count) ;			 
				if( pIcon )																
				{
					m_pSelectRequitalGrid->DeleteIcon(pIcon) ;							
				}
			}

			PTRLISTPOS pos = m_QuestSelectRequitalList.GetHeadPosition() ;				
			while( pos )																
			{
				RequitalItem* item = ( RequitalItem* )m_QuestSelectRequitalList.GetNext( pos ) ;			
				if( item )														
				{
					WINDOWMGR->AddListDestroyWindow( item->pIcon );				
					SAFE_DELETE( item ) ;										
				}
			}
			m_QuestSelectRequitalList.RemoveAll() ;							
		}

		SetGold(0) ;																// 보상 골드 수치를 0으로 세팅한다.
		SetExp(0) ;																	// 보상 경험치 수치를 0으로 세팅한다.

		return;																		// return 처리 한다.
	}

	m_pQuestDescList->RemoveAll() ;													// 설명 리스트 다이얼로그르 모두 비운다.

	SetQuestPrevInfo(pQuestString) ;

	AddRemandQuestExecuteString( pQuestString->GetQuestID() );

	RequestInfo(pQuestString) ;

	RequitalInfo(pQuestString) ;

	m_pQuestDescList->ResetGuageBarPos() ;
}

void CQuestDialog::AddRequitalItem(ITEM_INFO* pItemInfo, DWORD Count, BYTE bReauitalType)
{
	if( !pItemInfo ) return ;

	RequitalItem* pSQuest = new RequitalItem;										// 퀘스트 아이템 메모리를 할당한다.
	memcpy(&pSQuest->itemInfo, pItemInfo, sizeof(ITEM_INFO)) ;						// 퀘스트 아이템 정보에 인자로 넘어온 정보를 세팅한다.
	pSQuest->nItemCount = (int)Count;												// 퀘스트 아이템의 카운트를 세팅한다.
	pSQuest->wPos = 0;																// 퀘스트 아이템의 위치를 -1로 세팅한다.

	cIcon* pIcon = new cIcon;														// 아이콘 메모리를 할당한다.
	cImage Image;																	// 이미지를 선언한다.

	SCRIPTMGR->GetImage(pSQuest->itemInfo.Image2DNum, &Image, PFT_ITEMPATH);		// 퀘스트 아이템 정보의 이미지로 이미지를 세팅한다.
	pIcon->SetBasicImage(&Image);													// 아이콘의 기본 이미지로 위에서 세팅한 이미지를 설정한다.
	pIcon->Init(0, 0, 40, 40, &Image, m_IndexGenerator.GenerateIndex() );			// 아이콘을 초기화 한다.
	pIcon->SetMovable(FALSE);														// 이동 여부를 FALSE로 세팅한다.
	pIcon->SetActive(FALSE);														// 활성화 여부를 FALSE로 세팅한다.

	pSQuest->pIcon = pIcon ;

	// 100414 ONS 일반보상과 선택보상을 구분해서 리스트에 저장한다.
	if( bReauitalType == BYTE(eQuestExecute_TakeSelectItem) )
		m_QuestSelectRequitalList.AddTail(pSQuest);
	else
		m_QuestRequitalList.AddTail(pSQuest);
	
	WINDOWMGR->AddWindow(pIcon);													// 윈도우 매니져에 아이콘 정보를 추가한다.

	RefreshRequitalItem() ;
	RefreshSelectRequitalItem();
}

void CQuestDialog::GiveupQuestDelete(DWORD QuestIdx)								// 퀘스트 포기 삭제 함수.
{
	PTRLISTSEARCHSTART(m_QuestTree, QuestTree*, pQTree);							// 퀘스트 트리 리스트를 검색한다.
	{
		if( pQTree )																// 퀘스트 트리 정보가 유효한지 체크한다.
		{
			if(pQTree->QuestIdx == QuestIdx)										// 퀘스트 트리 인덱스가 삭제할 인덱스와 같으면,
			{
				pQTree->list.RemoveAll();											// 트리 정보의 리스트를 모두 비운다.
				pQTree->State = eQTree_Close;										// 트리 정보의 상태를 닫혀있는 상태로 세팅한다.
				pQTree->Count = 0;													// 트리 정보의 카운트를 0으로 세팅한다.
				pQTree->QuestIdx = 0;												// 트리 정보의 퀘스트 인덱스르르 0으로 세팅한다.
				m_QuestTree.Remove(pQTree);											// 퀘스트 트리 리스트에서 트리 정보를 비운다.
				delete pQTree;														// 트리 정보를 삭제한다.
				break;																// 검색을 빠져나온다.
			}
		}
	}
	PTRLISTSEARCHEND																// 검색을 종료한다.

	DWORD Idx = 0;																	// 인덱스 변수를 선언하고 0으로 세팅한다.
	cIcon* pIcon = NULL;															// 아이콘 포인터를 선언하고 null처리를 한다.

	PTRLISTSEARCHSTART(m_QuestItemList, sQuestItem*, pSQuest);						// 퀘스트 아이템 리스트를 검색한다.
		if(!pSQuest)			break;												// 퀘스트 아이템 정보가 유효하지 않으면 검색을 빠져나간다.

		Idx = pSQuest->pQuestItemInfo->QuestKey/100;								// 퀘스트 아이템 인덱스를 받는다.

		if(Idx == QuestIdx)															// 인덱스가 포기하려는 퀘스트 인덱스와 같으면,
		{
			m_pIconGridDlg->DeleteIcon(pSQuest->Pos, &pIcon);						// 퀘스트 아이템 그리드에서 아이템을 삭제한다.

			if(!pIcon)			break;												// 아이콘 정보가 유효하지 않으면 검색을 빠져나간다.

			WINDOWMGR->AddListDestroyWindow(pIcon);									// 윈도우 매니저에 파괴할 윈도우로 아이콘을 추가한다.
			m_IndexGenerator.ReleaseIndex(pIcon->GetID());							// 인덱스 생성기에서 아이콘을 해제 한다.
			m_QuestItemList.Remove(pSQuest);										// 퀘스트 아이템 리스트에서, 아이템을 삭제한다.
		}	
	PTRLISTSEARCHEND																// 검색을 종료한다.

	RefreshQuestItem(m_QuestItemPage);												// 퀘스트 아이템 그리드를 새로고침한다.
}

void CQuestDialog::QuestItemAdd(QUEST_ITEM_INFO* pQuestItemInfo, DWORD Count)		// 퀘스트 아이템을 추가하는 함수.
{
	if(!pQuestItemInfo)			return;												// 퀘스트 아이템 정보가 유효하지 않으면 리턴 처리를 한다.

	PTRLISTSEARCHSTART(m_QuestItemList,sQuestItem*,pQuestItem)						// 퀘스트 아이템 리스트를 검색한다.
		if( pQuestItem->pQuestItemInfo->ItemIdx == pQuestItemInfo->ItemIdx )		// 퀘스트 아이템 정보의 아이템 인덱스와, 인자로 넘어온 아이템 인덱스가 같으면,
		{
			pQuestItem->Count = Count ;												// 퀘스트 아이템 리스트의 아이템 카운트를 세팅한다.
			return ;																// 리턴 처리를 한다.
		}
	PTRLISTSEARCHEND																// 퀘스트 아이템 리스트 검색을 종료한다.

	sQuestItem* pSQuest = new sQuestItem;											// 퀘스트 아이템 메모리를 할당한다.
	pSQuest->pQuestItemInfo = pQuestItemInfo;										// 퀘스트 아이템 정보에 인자로 넘어온 정보를 세팅한다.
	pSQuest->Count = Count;															// 퀘스트 아이템의 카운트를 세팅한다.
	pSQuest->Pos = WORD(-1);

	cIcon* pIcon = new cIcon;														// 아이콘 메모리를 할당한다.
	cImage Image;																	// 이미지를 선언한다.

	SCRIPTMGR->GetImage(pSQuest->pQuestItemInfo->Image2DNum, &Image, PFT_ITEMPATH);	// 퀘스트 아이템 정보의 이미지로 이미지를 세팅한다.
	pIcon->SetBasicImage(&Image);													// 아이콘의 기본 이미지로 위에서 세팅한 이미지를 설정한다.
	pIcon->Init(0, 0, 40, 40, &Image, m_IndexGenerator.GenerateIndex() );			// 아이콘을 초기화 한다.
	pIcon->SetMovable(FALSE);														// 이동 여부를 FALSE로 세팅한다.
	pIcon->SetActive(FALSE);														// 활성화 여부를 FALSE로 세팅한다.

	pSQuest->pIcon = pIcon;															// 퀘스트 아이템 정보의 아이콘을 위에서 설정한 아이콘으로 세팅한다.

	m_QuestItemList.AddTail(pSQuest);												// 퀘스트 아이템 정보를 퀘스트 아이템 리스트에 추가한다.

	WINDOWMGR->AddWindow(pIcon);													// 윈도우 매니져에 아이콘 정보를 추가한다.
	RefreshQuestItem(m_QuestItemPage);												// 퀘스트 아이템 페이지를 새로고침한다.
}

void CQuestDialog::QuestItemDelete(DWORD ItemIdx)									// 퀘스트 아이템을 삭제하는 함수.
{
	sQuestItem* pItem;

	PTRLISTPOS pos = NULL ;
	pos = m_QuestItemList.GetHeadPosition() ;

	while(pos)
	{
		pItem = NULL ;
		pItem = (sQuestItem*)m_QuestItemList.GetNext(pos) ;

		if( pItem )
		{
			if( pItem->pQuestItemInfo->ItemIdx == ItemIdx )
			{
				m_pIconGridDlg->DeleteIcon(pItem->Pos, &pItem->pIcon) ;

				pItem->pIcon->SetActive(FALSE);									
				WINDOWMGR->AddListDestroyWindow(pItem->pIcon);					
				m_IndexGenerator.ReleaseIndex(pItem->pIcon->GetID());			

				m_QuestItemList.Remove(pItem) ;

				delete pItem ;
			}
		}
	}

	RefreshQuestItem(m_QuestItemPage);
}

DWORD CQuestDialog::QuestItemUpdate(DWORD type, DWORD ItemIdx, DWORD data)			// 퀘스트 아이템을 업데이트 한다.
{
	PTRLISTSEARCHSTART(m_QuestItemList, sQuestItem*, pSQuest);						// 퀘스트 아이템 리스트에서 아이템 정보를 검색한다.

		if(!pSQuest)			break;												// 퀘스트 아이템 정보가 유효하지 않으면, 검색을 빠져나간다.

		if(pSQuest->pQuestItemInfo->ItemIdx == ItemIdx)								// 퀘스트 아이템 리스트 상의 아이템 인덱스와, 인자로 들어온 아이템 인덱스가 같으면,
		{
			switch(type)															// 업데이트 타입을 확인한다.
			{
			case eQItem_AddCount:													// 퀘스트 아이템 카운트를 추가하는 타입이면,
				if(pSQuest)		pSQuest->Count += data;								// 퀘스트 아이템 정보가 유효하면, 퀘스트 아이템 정보의 카운트에 데이터를 더한다.
				break;
			case eQItem_SetCount:													// 퀘스트 아이템 카운트를 세팅하는 타입이면,
				if(pSQuest)		pSQuest->Count = data;								// 퀘스트 아이템 정보가 유효하면, 퀘스트 아이템 정보의 카운트를 데이터 값으로 세팅한다.
				break;
			case eQItem_GetCount:													// 퀘스트 아이템 카운트를 반환하는 타입이면,
				if(pSQuest)		return pSQuest->Count;								// 퀘스트 아이템 정보가 유효하면, 퀘스트 아이템 정보의 카운트를 반환한다.
				break;
			}
			break;																	// 검색을 빠져나간다.
		}
	PTRLISTSEARCHEND																// 퀘스트 아이템 리스트 검색을 종료한다.
	
	return 0;																		// 0을 리턴한다.
}

void CQuestDialog::RefreshRequitalItem()
{
	cIcon* pIcon = 0;
	int Count = 0;
	WORD Pos = 0;
	int	nStart = 0;

	for(WORD i=0; i<m_pRequitalGrid->GetCellNum(); ++i)
	{
		m_pRequitalGrid->DeleteIcon(i, &pIcon);										// 아이콘 그리드에서 아이콘을 삭제한다.
	}														// 퀘스트 아이템 리스트 검색을 종료한다.

	RequitalItem* pItem = 0;
	PTRLISTPOS pos = m_QuestRequitalList.GetHeadPosition();

	while(pos)
	{
		pItem = (RequitalItem*)m_QuestRequitalList.GetNext(pos) ;

		if( pItem )
		{
			if( Count >= nStart && Count < 6 )
			{
				pItem->wPos = Pos ;
				m_pRequitalGrid->AddIcon( pItem->wPos, pItem->pIcon ) ;

				pItem->pIcon->SetActive(TRUE) ;

				++Pos ;
			}
			else
			{
				pItem->wPos = WORD(-1);
				pItem->pIcon->SetActive(FALSE) ;
			}

			++Count ;
		}
	}
}

void CQuestDialog::RefreshQuestItem(DWORD Page)										// 퀘스트 아이템 페이지를 새로고침하는 함수.
{	
	cIcon* pIcon = 0;
	int Count = 0;
	WORD Pos = 0;
	int	nStart = Page*VIEW_QUESTITEM_PERPAGE;

	for(WORD i=0; i<MAX_QUEST_PAGEBTN; ++i)
	{
		m_pPageBtn[i]->SetPush(FALSE);												// 페이지 버튼을 눌리지 않은 상태로 세팅한다.
	}
	m_pPageBtn[Page]->SetPush(TRUE);												// 인자로 넘어온 페이지 버튼을 눌린 상태로 세팅한다.

	for(WORD i=0; i<m_pIconGridDlg->GetCellNum(); ++i)
	{
		m_pIconGridDlg->DeleteIcon(i, &pIcon);										// 아이콘 그리드에서 아이콘을 삭제한다.
	}
	
	
	PTRLISTSEARCHSTART(m_QuestItemList, sQuestItem*, pSQuest);						// 퀘스트 아이템 리스트에서 퀘스트 아이템 정보를 검색한다.

		if(!pSQuest)			break;												// 퀘스트 아이템 정보가 유효하지 않으면, 검색을 빠져나온다.

		if(Count>=nStart && Count < nStart+VIEW_QUESTITEM_PERPAGE)					// 시작 인수가 카운트 값보다 작고, 페이지 당 시작 인수가 카운트 보다 작으면,
		{
			// Add
			pSQuest->Pos = Pos;														// 퀘스트 아이템 정보의 위치 정보를 세팅한다.
			m_pIconGridDlg->AddIcon(pSQuest->Pos, pSQuest->pIcon);					// 퀘스트 아이템 아이콘 그리드에, 아이콘 정보를 세팅한다.
			pSQuest->pIcon->SetActive(TRUE);										// 퀘스트 아이템 아이콘을 활성화 한다.
			++Pos;																	// 위치 정보를 증가한다.
		}
		else																		// 시작 인수 조건이 맞지 않으면,
		{
			pSQuest->Pos = WORD(-1);
			pSQuest->pIcon->SetActive(FALSE);										// 퀘스트 아이템의 아이콘을 비활성화 시킨다.
		}

		++Count;																	// 카운트 값을 증가시킨다.

	PTRLISTSEARCHEND																// 퀘스트 아이템 리스트 검색을 종료한다.

	m_QuestItemPage = Page;															// 퀘스트 아이템 페이지를 세팅한다.
}

void CQuestDialog::ShowRequitalItemToolTip(cPtrList& RequitalList, DWORD Index)
{
	char buf[64] = {0};
	DWORD dwCount = 0;

	PTRLISTSEARCHSTART(RequitalList, RequitalItem*, pSQuest);				// 퀘스트 아이템 리스트에서, 퀘스트 아이템 정보를 검색한다.

		if(!pSQuest)			break;												// 퀘스트 아이템 정보가 유효하지 않으면, 검색을 빠져나간다.

		if(int(dwCount) == m_RequitalItemTooltipIdx)										// 툴팁 인덱스와 카운트 값이 같으면,
		{
			pSQuest->pIcon->SetToolTip("");											// 퀘스트 아이템 정보의 툴팁을 공란으로 세팅한다.
		}

		if(dwCount == Index)														// 카운트 값과, 인자로 넘어온 인덱스가 같으면,
		{
			cImage Image;															// 이미지를 선언한다.
			SCRIPTMGR->GetImage(0, &Image, PFT_HARDPATH);							// 이미지를 세팅한다.
			pSQuest->pIcon->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &Image, TTCLR_ITEM_CANEQUIP );	// 아이콘 툴팁을 공란으로 세팅한다.

			wsprintf(buf, "[%s]", pSQuest->itemInfo.ItemName);				// 임시 버퍼에 아이템 이름을 세팅한다.
			pSQuest->pIcon->AddToolTipLine(buf, ITEMMGR->GetItemNameColor( pSQuest->itemInfo ));						// 퀘스트 아이템의 툴팁 라인에 임시 버퍼를 추가한다.
			
			ITEMMGR->AddToolTip( pSQuest->pIcon, pSQuest->itemInfo.ItemTooltipIdx );
		}

		++dwCount;																	// 카운트 값을 증가한다.

	PTRLISTSEARCHEND																// 퀘스트 아이템 리스트 검색을 종료한다.
		
	m_RequitalItemTooltipIdx = Index;												// 툴팁 인덱스를 인자로 넘어온 인덱스로 세팅한다.
}

void CQuestDialog::ShowItemToolTip(DWORD Index)										// 아이템 툴팁을 보여주는 함수.
{
	char buf[64] = {0};
	DWORD dwCount = 0;

	PTRLISTSEARCHSTART(m_QuestItemList, sQuestItem*, pSQuest);						// 퀘스트 아이템 리스트에서, 퀘스트 아이템 정보를 검색한다.

		if(!pSQuest)			break;												// 퀘스트 아이템 정보가 유효하지 않으면, 검색을 빠져나간다.

		if(int(dwCount) == m_TooltipIdx)													// 툴팁 인덱스와 카운트 값이 같으면,
		{
			pSQuest->pIcon->SetToolTip("");											// 퀘스트 아이템 정보의 툴팁을 공란으로 세팅한다.
		}

		if(dwCount == Index)														// 카운트 값과, 인자로 넘어온 인덱스가 같으면,
		{
			cImage Image;															// 이미지를 선언한다.
			SCRIPTMGR->GetImage(0, &Image, PFT_HARDPATH);							// 이미지를 세팅한다.
			pSQuest->pIcon->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &Image, TTCLR_ITEM_CANEQUIP );	// 아이콘 툴팁을 공란으로 세팅한다.

			wsprintf(buf, "[%s]", pSQuest->pQuestItemInfo->ItemName);				// 임시 버퍼에 아이템 이름을 세팅한다.
			pSQuest->pIcon->AddToolTipLine(buf, TTTC_DEFAULT);						// 퀘스트 아이템의 툴팁 라인에 임시 버퍼를 추가한다.
			pSQuest->pIcon->AddToolTipLine("");										// 퀘스트 아이템의 툴팁 라인에 공란을 추가한다.
			AddTooltipInfo(pSQuest->pQuestItemInfo->ItemDesc, pSQuest->pIcon);		// 퀘스트 아이템 정보에 아이템 설명으로 툴팁을 추가한다.
		}

		++dwCount;																	// 카운트 값을 증가한다.

	PTRLISTSEARCHEND																// 퀘스트 아이템 리스트 검색을 종료한다.
		
	m_TooltipIdx = Index;															// 툴팁 인덱스를 인자로 넘어온 인덱스로 세팅한다.
}

void CQuestDialog::AddTooltipInfo(char* pString, cIcon* pIcon)						// 툴팁 정보를 추가하는 함수.
{
	if(strlen(pString) > MAX_ITEMDESC_LINE)											// 최대 아이템 설명 라인보다 스트링 길이가 크면,
	{
		char* pStr = pString;														// 문자열 포인터를 선언하고 스트링 정보를 받는다.
		char tb[MAX_ITEMDESC_LINE+2] = { 0,  };										// 임시 버퍼를 선언한다.
		int len = 0;																// 길이를 0으로 세팅한다.
		
		while(*pStr)																// 문자열 정보가 유효할 동안 while문을 돌린다.
		{						
			tb[len]=*pStr;															// 임시 버퍼의 길이에 글자를 담는다.

			if( pStr + 1 != CharNext( CharPrev( pStr, pStr + 1 ) ) )				// 현재 글자의 다음 글자를 체크하여 2바이트 글자면, 
				tb[++len] = *(++pStr);												// 한 바이트를 더 담는다.

			++len;																	// 길이 값을 증가한다.
			++pStr;																	// 문자 포인터를 증가한다.
			
			// 일단은 그냥 막자.. ㅡ.ㅡ;;
			if(tb[0] == ' ')		{	len = 0;	}								// 임시 버퍼의 첫 바이트가 빈칸이면, 길이를 0으로 세팅한다.
			
			if(strlen(tb) >= MAX_ITEMDESC_LINE)										// 임시 버퍼 길이가 최대 아이템 설명 라인보다 크면,
			{
				pIcon->AddToolTipLine(tb, TTTC_DESCRIPTION);						// 임시 버퍼를 툴팁 라인에 추가한다.
				memset(tb, 0, MAX_ITEMDESC_LINE+2);									// 임시 버퍼를 메모리 셋한다.
				len = 0;															// 길이를 0으로 세팅한다.
			}
		}

		if(len)																		// 길이 정보가 있으면,
		{
			pIcon->AddToolTipLine(tb, TTTC_DESCRIPTION);							// 아이콘에 툴팁을 추가한다.
			memset(tb, 0, MAX_ITEMDESC_LINE+2);										// 임시 버퍼를 메모리 셋한다.
			len = 0;																// 길이 값을 0으로 세팅한다.
		}
	}
	else																			// 최대 아이템 설명 라인보다 스트링 길이가 작으면,
		pIcon->AddToolTipLine(pString, TTTC_DESCRIPTION);							// 아이콘에 툴팁을 추가한다.
}

void CQuestDialog::AddQuestTree(int nMapNum)
{
	QuestTree* pData = new QuestTree;									// 트리를 생성한다.

	CQuestString* pQuestString = new CQuestString;						// 트리에 담을 퀘스트 스트링을 생성한다.

	int nTitleLine = 0;
	pQuestString->AddLine(
		GetMapName(MAPTYPE(nMapNum)),
		nTitleLine,
		TRUE);
	pQuestString->SetIndex(nMapNum, 0);									// 트리 인덱스를 맵 번호의 0으로 세팅한다.
	pQuestString->SetTreeTitle(TRUE) ;									// 트리의 제목으로 세팅한다.
	pData->list.AddHead(pQuestString);									// 새로 생성한 트리에 맵 타이틀을 추가한다.
	++pData->Count;														// 트리의 스트링 카운트를 증가한다.
	pData->State = eQTree_Open ;										// 트리의 상태를 닫힌 상태로 세팅한다.
	pData->nMapNum = nMapNum ;											// 트리의 맵 번호를 세팅한다.

	m_QuestTree.AddTail(pData) ;
}

void CQuestDialog::AddQuestTitle(CQuestString* pQuestString, int nMapNum)
{
	if( !pQuestString )
	{
		return ;
	}

	QuestTree* pTree ;

	PTRLISTPOS pos = NULL ;
	pos = m_QuestTree.GetHeadPosition() ;

	while(pos)
	{
		pTree = NULL ;
		pTree = (QuestTree*)m_QuestTree.GetNext(pos) ;

		if( pTree )
		{
			if(pTree->nMapNum == nMapNum)
			{
				CQuestString* pNewQuestString = new CQuestString ;

				DWORD dwMainQuestIdx = pQuestString->GetQuestID() ;
				DWORD dwSubQuestIdx  = pQuestString->GetSubID() ;

				pNewQuestString->SetQuestID( dwMainQuestIdx ) ;
				pNewQuestString->SetSubID( dwSubQuestIdx ) ;
				pNewQuestString->SetIndex( dwMainQuestIdx, dwSubQuestIdx ) ;

				pNewQuestString->SetComplete( pQuestString->IsComplete() ) ;

				pNewQuestString->SetCount( pQuestString->GetCount() ) ;
				pNewQuestString->SetTotalCount( pQuestString->GetTotalCount() ) ;


				ITEM* pItem = NULL ;

				cPtrList* titleList = NULL ;
				titleList = pQuestString->GetTitle() ;

				if( titleList )
				{
					PTRLISTPOS titlePos = NULL ;
					titlePos = titleList->GetHeadPosition() ;

					while(titlePos)
					{
						pItem = NULL ;
						pItem = (ITEM*)titleList->GetNext(titlePos) ;

						if( pItem )
						{
							ITEM* pNewItem = new ITEM ;

							//memcpy(pNewItem, pItem, sizeof(ITEM)) ;

							memcpy(pNewItem->string, pItem->string, sizeof(pItem->string)) ;
							pNewItem->line		= pItem->line ;
							pNewItem->nAlpha	= pItem->nAlpha ;
							pNewItem->nFontIdx	= pItem->nFontIdx ;
							pNewItem->rgb		= pItem->rgb ;

							pNewQuestString->GetTitle()->AddTail(pNewItem) ;
						}
					}
				}

				cPtrList* descList = NULL ;
				descList = pQuestString->GetDesc() ;

				if( descList )
				{
					PTRLISTPOS descPos = NULL ;
					descPos = descList->GetHeadPosition() ;

					while(descPos)
					{
						pItem = NULL ;
						pItem = (ITEM*)descList->GetNext(descPos) ;

						if( pItem )
						{
							ITEM* pNewItem = new ITEM ;

							//memcpy(pNewItem, pItem, sizeof(ITEM)) ;

							memcpy(pNewItem->string, pItem->string, sizeof(pItem->string)) ;
							pNewItem->line		= pItem->line ;
							pNewItem->nAlpha	= pItem->nAlpha ;
							pNewItem->nFontIdx	= pItem->nFontIdx ;
							pNewItem->rgb		= pItem->rgb ;

							pNewQuestString->GetDesc()->AddTail(pNewItem) ;
						}
					}
				}

				pNewQuestString->SetTreeTitle( FALSE ) ;

				pTree->list.AddTail(pNewQuestString) ;
				++pTree->Count;
			}
		}
	}
}






void CQuestDialog::StartQuest(DWORD dwQuestIdx)
{
	CQuestInfo* pQuestInfo =NULL ;														// 퀘스트 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pQuestInfo = QUESTMGR->GetQuestInfo(dwQuestIdx) ;									// 퀘스트 인덱스로 퀘스트 정보를 받는다.

	if(!pQuestInfo)																		// 퀘스트 정보가 유효하지 않으면,
	{
		/*char tempBuf[128] = {0, } ;
		sprintf(tempBuf, "[CheckMapTree] - (%d)번 퀘스트 정보를 받지 못해서 퀘스트를 추가하지 못했습니다.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	int nMapNum = pQuestInfo->GetMapNum() ;												// 퀘스트 정보에 있는 맵 번호를 받는다.

	QuestTree* pTree ;																	// 퀘스트 트리 정보를 받을 포인터를 선언한다.
	
	PTRLISTPOS pos = NULL ;																// 트리의 위치를 받을 포인터를 선언하고 null 처리를 한다.
	pos = m_QuestTree.GetHeadPosition() ;												// 퀘스트 트리 리스트를 헤드로 세팅하고 위치를 받는다.

	while(pos)																			// 트리 위치 정보가 유효할 동안 whlie문을 돌린다.
	{
		pTree = NULL ;																	// 트리 정보를 받을 포인터를 null 처리를 한다.
		pTree = (QuestTree*)m_QuestTree.GetNext(pos) ;									// 현재 위치의 트리 정보를 받고, 위치를 다음으로 세팅한다.

		if( !pTree ) continue ;															// 트리 정보가 유효하지 않으면 continue 처리를 한다.

		if( pTree->nMapNum != nMapNum ) continue ;										// 현재 트리의 맵 번호와, 비교대상의 맵 번호가 다르면, continue 처리를 한다.

		BOOL b_SameQuestIdx = FALSE ;													// 트리 내에서 같은 인덱스가 있는지 결과값을 담을 변수를 선언하고 false로 세팅한다.

		DWORD* pQuestIdx ;																// 퀘스트 인덱스 번호를 받을 포인터를 선언한다.

		PTRLISTPOS idPos = NULL ;														// 퀘스트 인덱스 위치를 받을 포인터를 선언하고 null 처리를 한다.
		idPos = pTree->list.GetHeadPosition() ;											// 현재 트리의 리스트를 헤드로 세팅하고, 인덱스 위치를 받는다.

		while(idPos)																	// 인덱스 위치 정보가 유효할 동안 while문을 돌린다.
		{
			pQuestIdx = NULL ;															// 퀘스트 인덱스를 받을 포인터를 null 처리를 한다.
			pQuestIdx = (DWORD*)pTree->list.GetNext(idPos) ;							// 현재 위치의 퀘스트 인덱스를 받고, 위치를 다음으로 세팅한다.

			if( !pQuestIdx ) continue ;													// 퀘스트 인덱스 정보가 유효하지 않으면 continue 처리를 한다.

			if( *pQuestIdx == dwQuestIdx )												// 퀘스트 인덱스가 비교 대상의 퀘스트 인덱스와 같으면,
			{
				b_SameQuestIdx = TRUE ;													// 결과 값을 true로 세팅한다.
				break ;																	// 현재 while문을 탈출한다.
			}
		}

		if( b_SameQuestIdx )															// 결과 값이 true 이면,
		{
			return ;
		}
		else																			// 같은 퀘스트가 없다면,
		{
			DWORD* pNewQuest = new DWORD ;												// 퀘스트 인덱스를 담을 변수를 생성한다.
			*pNewQuest = dwQuestIdx ;													// 퀘스트 인덱스 값을 세팅한다.

			pTree->list.AddTail(pNewQuest) ;											// 현재 맵 트리에 퀘스트 인덱스를 추가한다.

			++m_nQuestCount ;
			return ;
		}
	}

	AddMapTree(nMapNum, dwQuestIdx) ;													// 새로운 맵 트리를 추가하고, 퀘스트도 추가한다.

	//SetSelectedQuestIdx(dwQuestIdx); // 080304 NYJ --- 퀘스트자동알리미 처리를 위해 추가 (현재 사용되지 않음)
	RefreshQuestList() ;																// 퀘스트 리스트를 새로고침한다.
}





void CQuestDialog::EndQuest(DWORD dwQuestIdx)
{
	CQuestInfo* pQuestInfo =NULL ;														// 퀘스트 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pQuestInfo = QUESTMGR->GetQuestInfo(dwQuestIdx) ;									// 퀘스트 인덱스로 퀘스트 정보를 받는다.

	if(!pQuestInfo)																		// 퀘스트 정보가 유효하지 않으면,
	{
		return ;
	}

	int nMapNum = pQuestInfo->GetMapNum() ;												// 퀘스트 정보에 있는 맵 번호를 받는다.

	QuestTree* pTree ;																	// 퀘스트 트리 정보를 받을 포인터를 선언한다.
	
	PTRLISTPOS pos = NULL ;																// 트리의 위치를 받을 포인터를 선언하고 null 처리를 한다.
	pos = m_QuestTree.GetHeadPosition() ;												// 퀘스트 트리 리스트를 헤드로 세팅하고 위치를 받는다.

	while(pos)																			// 트리 위치 정보가 유효할 동안 whlie문을 돌린다.
	{
		pTree = NULL ;																	// 트리 정보를 받을 포인터를 null 처리를 한다.
		pTree = (QuestTree*)m_QuestTree.GetNext(pos) ;									// 현재 위치의 트리 정보를 받고, 위치를 다음으로 세팅한다.

		if( !pTree ) continue ;															// 트리 정보가 유효하지 않으면 continue 처리를 한다.

		if( pTree->nMapNum != nMapNum ) continue ;										// 현재 트리의 맵 번호와, 비교대상의 맵 번호가 다르면, continue 처리를 한다.

		DWORD* pQuestIdx ;																// 퀘스트 인덱스 번호를 받을 포인터를 선언한다.

		PTRLISTPOS idPos = NULL ;														// 퀘스트 인덱스 위치를 받을 포인터를 선언하고 null 처리를 한다.
		idPos = pTree->list.GetHeadPosition() ;											// 현재 트리의 리스트를 헤드로 세팅하고, 인덱스 위치를 받는다.

		while(idPos)																	// 인덱스 위치 정보가 유효할 동안 while문을 돌린다.
		{
			pQuestIdx = NULL ;															// 퀘스트 인덱스를 받을 포인터를 null 처리를 한다.
			pQuestIdx = (DWORD*)pTree->list.GetNext(idPos) ;							// 현재 위치의 퀘스트 인덱스를 받고, 위치를 다음으로 세팅한다.

			if( !pQuestIdx ) continue ;													// 퀘스트 인덱스 정보가 유효하지 않으면 continue 처리를 한다.

			if( *pQuestIdx == dwQuestIdx )												// 퀘스트 인덱스가 비교 대상의 퀘스트 인덱스와 같으면,
			{
				pTree->list.Remove(pQuestIdx) ;
				delete pQuestIdx ;
			}
		}

		if( pTree->list.GetCount() < 1 ) DeleteMapTree(nMapNum) ;
	}

	RefreshQuestList() ;																// 퀘스트 리스트를 새로고침한다.

	// 081112 LYW --- QuestDialog : 퀘스트 목록에서 다른 퀘스트의 내용이 출력되는 현상.
	// 퀘스트 목록을 완료 및 포기 처리시 발생되는 퀘스트 내용의 포커스가
	// 기존 내용으로 유지되고 있는 현상 수정 -> 완료 및 포기 처리시 퀘스트 내용
	// 포커스를 Blank 상태로 처리 요망.(김남열)
	QuestDescView(NULL) ;

	m_SelectedQuestID = GetQuestIdx(1) ;
	SetSelectedQuestIdx(m_SelectedQuestID) ;

	// 100414 ONS 보상다이얼로그 초기화
	InitRequitalDlg();
}





void CQuestDialog::AddMapTree(int nMapNum, DWORD dwQuestIdx) 
{
	QuestTree* pNewQuestTree = new QuestTree ;
	pNewQuestTree->nMapNum = nMapNum ;
	pNewQuestTree->State = eQTree_Open ;

	DWORD* pNewQuest = new DWORD ;
	*pNewQuest = dwQuestIdx ;

	pNewQuestTree->list.AddTail(pNewQuest) ;

	m_QuestTree.AddTail(pNewQuestTree) ;
	++m_nQuestCount ;
}





void CQuestDialog::DeleteMapTree(int nMapNum)
{
	QuestTree* pTree ;																	// 퀘스트 트리 정보를 받을 포인터를 선언한다.
	
	PTRLISTPOS pos = NULL ;																// 트리의 위치를 받을 포인터를 선언하고 null 처리를 한다.
	pos = m_QuestTree.GetHeadPosition() ;												// 퀘스트 트리 리스트를 헤드로 세팅하고 위치를 받는다.

	while(pos)																			// 트리 위치 정보가 유효할 동안 whlie문을 돌린다.
	{
		pTree = NULL ;																	// 트리 정보를 받을 포인터를 null 처리를 한다.
		pTree = (QuestTree*)m_QuestTree.GetNext(pos) ;									// 현재 위치의 트리 정보를 받고, 위치를 다음으로 세팅한다.

		if( !pTree ) continue ;															// 트리 정보가 유효하지 않으면 continue 처리를 한다.

		if( pTree->nMapNum != nMapNum ) continue ;										// 현재 트리의 맵 번호와, 비교대상의 맵 번호가 다르면, continue 처리를 한다.

		DWORD* pQuestIdx ;																// 퀘스트 인덱스 번호를 받을 포인터를 선언한다.

		PTRLISTPOS idPos = NULL ;														// 퀘스트 인덱스 위치를 받을 포인터를 선언하고 null 처리를 한다.
		idPos = pTree->list.GetHeadPosition() ;											// 현재 트리의 리스트를 헤드로 세팅하고, 인덱스 위치를 받는다.

		while(idPos)																	// 인덱스 위치 정보가 유효할 동안 while문을 돌린다.
		{
			pQuestIdx = NULL ;															// 퀘스트 인덱스를 받을 포인터를 null 처리를 한다.
			pQuestIdx = (DWORD*)pTree->list.GetNext(idPos) ;							// 현재 위치의 퀘스트 인덱스를 받고, 위치를 다음으로 세팅한다.

			if( !pQuestIdx ) continue ;													// 퀘스트 인덱스 정보가 유효하지 않으면 continue 처리를 한다.

			pTree->list.Remove(pQuestIdx) ;

			delete pQuestIdx ;
		}

		pTree->list.RemoveAll() ;

		m_QuestTree.Remove(pTree) ;

		delete pTree ;
	}
}






void CQuestDialog::RefreshQuestList()
{
	if( !IsActive() )
		return;

	char buf[256] = { 0, };																// 임시 버퍼를 선언한다.	

	int BackupLine = m_pQuestListDlg->GetTopListItemIdx();							// 백업 라인을 세팅한다.

	CQuestQuickViewDialog* pQuickView = NULL ;
	pQuickView = GAMEIN->GetQuestQuickViewDialog() ;

	if( !pQuickView )
	{
		/*sprintf(buf, "퀘스트 알림창 정보를 받아오지 못해 퀘스트 리스트를 새로고침 하지 못했습니다.") ;
		CHATMGR->AddMsg(CTC_QUEST, buf) ;*/
		return ;
	}

	DWORD* pCheckIdx ;
	PTRLISTPOS checkPos ;																// 퀘스트 알림창의 위치 정보를 받을 포인터를 선언한다.

	cPtrList* pCheckList = NULL ;
	pCheckList = pQuickView->GetViewList() ;

	if( !pCheckList )
	{
		/*sprintf(buf, "퀘스트 알림창의 리스트 정보를 받아오지 못해 퀘스트 리스트를 새로고침 하지 못했습니다.") ;
		CHATMGR->AddMsg(CTC_QUEST, buf) ;*/
		return ;
	}

	m_pQuestListDlg->RemoveAll();														// 퀘스트 리스트 다이얼로그를 모두 비운다.

	m_QListCount = 0 ;
	memset(m_dwQuestList, 0, sizeof(CQuestString*)*MAX_PROCESSQUEST);					// 퀘스트 리스트를 메모리 셋 한다.
	
	DWORD Color = 0;																	// 색상 값을 담을 변수를 선언하고 0으로 세팅한다.

	QuestTree* pTree ;																	// 퀘스트 트리 정보를 받을 포인터를 선언한다.
	PTRLISTPOS pos ;																	// 트리의 위치를 받을 포인터를 선언한다.

	char* pMapName ;																	// 맵 이름을 받을 포인터를 선언한다.

	DWORD* pQuestIdx ;																	// 퀘스트 인덱스를 받을 포인터를 선언한다.
	CQuestInfo* pQuestInfo ;															// 퀘스트 정보를 받을 포인터를 선언한다.

	PTRLISTPOS idPos ;																	// 인덱스 위치 정보를 받을 포인터를 선언한다.
	CQuestString* pQuestString ;														// 퀘스트 스트링 정보를 받을 포인터를 선언한다.
	
	pos = NULL ;																		// 트리의 위치를 받을 포인터를 null 처리를 한다.
	pos = m_QuestTree.GetHeadPosition() ;												// 퀘스트 트리 리스트를 헤드로 세팅하고 위치를 받는다.

	while(pos)																			// 트리 위치 정보가 유효할 동안 whlie문을 돌린다.
	{
		pTree = NULL ;																	// 트리 정보를 받을 포인터를 null 처리를 한다.
		pTree = (QuestTree*)m_QuestTree.GetNext(pos) ;									// 현재 위치의 트리 정보를 받고, 위치를 다음으로 세팅한다.

		if( !pTree ) continue ;															// 트리 정보가 유효하지 않으면 continue 처리를 한다.

		if( pTree->list.GetCount() < 1 )
		{
			/*sprintf(buf, "(%d)번 맵의 등록된 퀘스트가 없어서, (%d)번 맵 퀘스트를 추가하지 못했습니다.", pTree->nMapNum) ;
			CHATMGR->AddMsg(CTC_QUEST, buf) ;*/
			continue ;
		}

		pMapName = GetMapName(
			MAPTYPE(pTree->nMapNum));

		if( !pMapName )
		{
			/*sprintf(buf, "(%d)번 맵 이름을 받아오지 못해 (%d)번 맵 퀘스트를 추가하지 못했습니다.", pTree->nMapNum) ;
			CHATMGR->AddMsg(CTC_QUEST, buf) ;*/
			continue ;
		}

		Color = RGBA_MAKE(255, 150, 0, 255) ;											// 맵 트리의 색상을 세팅한다.	
		sprintf(buf, "* ") ;															// 임시 버퍼를 *로 세팅한다.
		strcat(buf, pMapName) ;															// 임시 버퍼에 맵 이름을 추가한다.

		m_pQuestListDlg->AddItemWithFont(buf, Color, cFont::FONT1) ;							// 퀘스트 리스트에 맵 이름을 추가한다.
		//m_pQuestListDlg->AddItem(buf, Color) ;											// 퀘스트 리스트에 맵 이름을 추가한다.

		m_dwQuestList[m_QListCount] = pTree->nMapNum ;									// 퀘스트 리스트에 퀘스트 스트링 정보를 대입한다.
		++m_QListCount;																	// 퀘스트 리스트 카운트를 증가한다.

		if( pTree->State == eQTree_Close ) continue ;									// 트리가 닫혀진 상태라면, continue 처리를 한다.

		idPos = NULL ;																	// 인덱스 위치 정보를 받을 포인터 null 처리를 한다.
		idPos = pTree->list.GetHeadPosition() ;											// 현재 맵 트리의 리스트를 헤드로 세팅하고 위치를 받는다.

		while(idPos)																	// 위치 정보가 유효할 동안 while문을 돌린다.
		{
			pQuestIdx = NULL ;															// 퀘스트 인덱스를 받을 포인터를 null 처리를 한다.
			pQuestIdx = (DWORD*)pTree->list.GetNext(idPos) ;							// 현재 위치의 퀘스트 인덱스를 받고, 위치를 다음으로 세팅한다.

			if( !pQuestIdx ) continue ;													// 퀘스트 인덱스가 유효하지 않으면 continue 처리를 한다.

			memset(buf, 0, sizeof(buf)) ;												// 임시 버퍼를 메모리 셋한다.

			pQuestString = NULL ;														// 퀘스트 스트링 정보를 받을 포인터를 null 처리를 한다.
			pQuestString = QUESTMGR->GetQuestString(*pQuestIdx, 0) ;					// 현재 퀘스트의 제목 스트링을 받아온다.

			if( !pQuestString )															// 퀘스트 스트링 정보가 유효하지 않으면,
			{
				/*sprintf(buf, "(%d)번 퀘스트 정보를 받아오지 못해 (%d)번 퀘스트 제목을 추가하지 못했습니다.", pTree->nMapNum) ;
				CHATMGR->AddMsg(CTC_QUEST, buf) ;*/
				continue ;	
			}

			if( m_SelectedQuestID == int(pQuestString->GetQuestID()))
			{
				Color = RGBA_MAKE(255, 255, 0, 255) ;
			}
			else
			{
				Color = RGBA_MAKE(255, 255, 255, 255) ;
			}

			checkPos = NULL ;
			checkPos = pCheckList->GetHeadPosition() ;

			BOOL bResult = FALSE ;

			while(checkPos)
			{
				pCheckIdx = NULL ;
				pCheckIdx = (DWORD*)pCheckList->GetNext(checkPos) ;

				if( !pCheckIdx ) continue ;

				if( *pCheckIdx == pQuestString->GetQuestID() )
				{
					bResult = TRUE ;
					break ;
				}
			}

			if( bResult == FALSE )
			{
				sprintf(buf, "	- ");												// 임시 버퍼를 -로 세팅한다.
			}
			else
			{
				sprintf(buf, "	√");												// 임시 버퍼를 -로 세팅한다.
			}


			CQuest* pQuest = QUESTMGR->GetQuest(pQuestString->GetQuestID());

			// 100408 ShinJS --- 시간제한 퀘스트이고 현재 수행불가능한 시간인경우
			if( pQuest->HasDateLimit() && !QUESTMGR->IsValidDateTime( pQuestString->GetQuestID() ) )
			{
				strcat( buf, CHATMGR->GetChatMsg( 2216 ) );		// "[수행불가] "
			}

			ITEM* pItem ;

			cPtrList* pTitleList = NULL ;
			pTitleList = pQuestString->GetTitle() ; 

			PTRLISTPOS titlePos = NULL ;
			titlePos = pTitleList->GetHeadPosition() ;

			while(titlePos)
			{
				pItem = NULL ;
				pItem = (ITEM*)pTitleList->GetNext(titlePos) ;

				if( !pItem ) continue ;

				strcat(buf, pItem->string) ;
			}

			int nSubQuestCount = 0 ;
			pQuestInfo = QUESTMGR->GetQuestInfo(pQuestString->GetQuestID()) ;				// 퀘스트 번호로 퀘스트 정보를 받는다.

			if( pQuestInfo )
			{
				nSubQuestCount = pQuestInfo->GetSubQuestCount() ;							// 서브 퀘스트 수를 받는다.
			}

			// 100422 ShinJS --- 퀘스트 요구사항이 완료되었는지 판단한다.
			if( nSubQuestCount - 1 == int(pQuest->GetCurSubQuestIdx()) &&
				pQuest->IsCompletedDemandQuestExecute() )
			{
				strcat(buf, CHATMGR->GetChatMsg(1463)) ;
			}

			m_pQuestListDlg->AddItemWithFont(buf, Color, cFont::FONT0) ;							// 퀘스트 리스트에 맵 이름을 추가한다.
			//m_pQuestListDlg->AddItem(buf, Color) ;											// 퀘스트 리스트에 맵 이름을 추가한다.

			m_dwQuestList[m_QListCount] = *pQuestIdx ;										// 퀘스트 리스트에 퀘스트 스트링 정보를 대입한다.
			++m_QListCount;																	// 퀘스트 리스트 카운트를 증가한다.
		}
	}

	m_pQuestListDlg->ResetGuageBarPos() ;
	m_pQuestListDlg->SetTopListItemIdx(BackupLine);								// 아이템의 툴팁 인덱스를 세팅한다.

	SetQuestCount() ;

	CQuestString* pCurString = NULL ;
	pCurString = QUESTMGR->GetQuestString(m_SelectedQuestID, 0) ;
	// 071012 LYW --- QuestDialog : Modified.
	//QuestDescView(pCurString) ;	// 퀘스트가 없을때, 마지막으로 표시 했던 설명을 지우기위해 null을 추가한다.
	if( m_nQuestCount <= 0 ) 
	{
		QuestDescView(NULL) ;
	}
	else
	{
		QuestDescView(pCurString) ;
	}
}

// 100414 ONS 선택보상 아이템을 갱신한다.
void CQuestDialog::RefreshSelectRequitalItem()
{
	cIcon* pIcon = NULL;
	WORD Count = 0;
	WORD Pos = 0;

	for(WORD i=0; i<m_pSelectRequitalGrid->GetCellNum(); ++i)
	{
		m_pSelectRequitalGrid->DeleteIcon(i, &pIcon);								// 아이콘 그리드에서 아이콘을 삭제한다.
	}																				// 퀘스트 아이템 리스트 검색을 종료한다.

	PTRLISTPOS pos = m_QuestSelectRequitalList.GetHeadPosition() ;
	while(pos)
	{
		RequitalItem* pItem = (RequitalItem*)m_QuestSelectRequitalList.GetNext(pos) ;
		if( pItem ) 
		{
			if( Count >= 0 && Count < MAX_REQUITAL_ICON_CELL )
			{
				pItem->wPos = Pos ;
				m_pSelectRequitalGrid->AddIcon( pItem->wPos, pItem->pIcon ) ;
				pItem->pIcon->SetActive(TRUE) ;
				++Pos ;
			}
			else
			{
				pItem->wPos = WORD(-1);
				pItem->pIcon->SetActive(FALSE) ;
			}
			++Count ;
		}
	}
}

// 100414 ONS 일반보상과 선택보상버튼 선택시 활성화처리.
void CQuestDialog::SetActiveSelectRequital( BOOL bVal )
{
	m_pSelectRequitalGrid->SetActive(bVal);
	m_pRequitalGrid->SetActive(!bVal);

	m_pRequitalBtn->SetPush(bVal);
	m_pSelectRequitalBtn->SetPush(!bVal);
}

// 100414 ONS 일반보상/선택보상관련 UI초기화
void CQuestDialog::InitRequitalDlg()
{
	m_bRequitalType = eQuestExecute_TakeItem;

	m_pSelectRequitalGrid->SetActive(FALSE);
	m_pRequitalGrid->SetActive(TRUE);

	m_pRequitalBtn->SetPush(FALSE);
	m_pSelectRequitalBtn->SetPush(TRUE);

	m_pItemSignalOn->SetActive(FALSE);
	m_pItemSignalOff->SetActive(TRUE);
	m_pSelectItemSignalOn->SetActive(FALSE);
	m_pSelectItemSignalOff->SetActive(TRUE);
}

void CQuestDialog::AddRemandQuestExecuteString( DWORD dwQuestIdx )
{
	CQuest* pQuest = QUESTMGR->GetQuest(dwQuestIdx);
	if( !pQuest )
		return;

	BOOL bAdded = FALSE;

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

				if( !bAdded )
				{
					bAdded = TRUE;
					m_pQuestDescList->AddItem( " ", 0 );
				}

				char buf[MAX_PATH] = {0,};

				DWORD dwInvenCnt = GAMEIN->GetInventoryDialog()->GetTotalItemDurability( dwItemIdx );
				sprintf( buf, CHATMGR->GetChatMsg( 2220 ), pItemInfo->ItemName, dwInvenCnt, dwNeedCnt );

				m_pQuestDescList->AddItem( buf, RGBA_MAKE(255, 255, 0, 255) );
			}
			break;
		}
	}
}

// 100611 ONS 퀘스트다이얼로그 리스트에 선택한 퀘스트가 존재하는지 검사한다.
BOOL CQuestDialog::IsExistInQuestList( DWORD dwQuestIdx )
{
	if( 0 >= dwQuestIdx )
	{
		return FALSE;
	}

	PTRLISTPOS pos = m_QuestTree.GetHeadPosition();
	while(pos)										
	{
		QuestTree* pTree = (QuestTree*)m_QuestTree.GetNext(pos);
		if( NULL == pTree || 
			pTree->list.GetCount() < 1)
		{
			continue;
		}
		
		PTRLISTPOS idPos = pTree->list.GetHeadPosition();
		while(idPos)
		{
			const DWORD* pQuestIdx = (DWORD*)pTree->list.GetNext(idPos);
			if( *pQuestIdx == dwQuestIdx )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

// 100617 ONS 퀘스트 트리로부터 선택한 항목의 인덱스를 반환한다.
DWORD CQuestDialog::GetSelectedQuestIDFromTree( DWORD dwSelectIndex )
{
	DWORD dwCount = 0;
	PTRLISTPOS pos = m_QuestTree.GetHeadPosition();
	while(pos)										
	{
		const QuestTree* pTree = (QuestTree*)m_QuestTree.GetNext(pos);
		if( 0 == pTree || 
			pTree->list.GetCount() < 1)
		{
			return 0;
		}

		PTRLISTPOS idPos = pTree->list.GetHeadPosition();
		while(idPos)
		{
			const DWORD* pQuestIdx = (DWORD*)pTree->list.GetNext(idPos);
			if( *pQuestIdx > 0 && dwCount == dwSelectIndex )
			{
				return *pQuestIdx;
			}
			dwCount++;
		}
	}
	return 0;
}