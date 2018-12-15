#include "stdafx.h"																		// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.

#include ".\gamenotifymanager.h"														// 게임 공지 매니져 클래스 헤더를 불러온다.

#include "GameIn.h"																		// 게임 인 클래스 헤더를 불러온다.

#include "./Interface/cFont.h"

#include "QuestManager.h"
#include "..\[CC]Quest\QuestDefines.h"	
#include "..\[CC]Quest\QuestString.h"
#include "..\[CC]Quest\QuestInfo.h"
#include "../[CC]Header/GameResourceManager.h"

#include "Item.h"

#include "ObjectManager.h"

#include "ChatManager.h"

GLOBALTON(CGameNotifyManager)

CGameNotifyManager::CGameNotifyManager(void)											// 생성자 함수.
{
	m_pNotifyDlg = NULL ;																// 게임 공지 출력 용 다이얼로그 포인터를 null처리를 한다.

	Init() ;																			// 초기화 함수를 호출한다.
}

CGameNotifyManager::~CGameNotifyManager(void)											// 소멸자 함수.
{
	PTRLISTPOS pos = NULL ;
	pos = m_MsgList.GetHeadPosition() ;

	sGAMENOTIFY_MSG* pGameNotifyMsg ;

	while(pos)
	{
		pGameNotifyMsg = NULL ;
		pGameNotifyMsg = (sGAMENOTIFY_MSG*)m_MsgList.GetNext(pos) ;

		if( pGameNotifyMsg )
		{
			cPtrList* pList = NULL ;
			pList = &(pGameNotifyMsg->itemList) ;

			if( pList )
			{
				PTRLISTPOS itemPos = NULL ;
				itemPos = pList->GetHeadPosition() ;

				ITEM* pItem ;

				while(itemPos)
				{
					pItem = NULL ;
					pItem = (ITEM*)pList->GetNext(itemPos) ;

					if( pItem )
					{
						pList->Remove(pItem) ;

						delete pItem ;
						pItem = NULL ;
					}
				}

				pList->RemoveAll() ;

				m_MsgList.Remove(pGameNotifyMsg) ;

				delete pGameNotifyMsg ;
				pGameNotifyMsg = NULL ;
			}
		}
	}

	m_MsgList.RemoveAll() ;
}

void CGameNotifyManager::Init()
{}

void CGameNotifyManager::AddMsg(sGAMENOTIFY_MSG* pGameNotifyMsg) 						// 게임 공지 메시지 추가 함수.
{
	m_MsgList.AddTail(pGameNotifyMsg) ;
}

void CGameNotifyManager::Render()
{
	PTRLISTPOS pos = m_MsgList.GetHeadPosition() ;

	if( m_MsgList.GetCount() == 0 )
	{
		return ;
	}

	if( !pos )
	{
		return ;
	}

	PTRLISTPOS pos2;
	sGAMENOTIFY_MSG* pGameNotifyMsg = 0;
	cPtrList tempList;
	cPtrList* pList;
	LONG lTextWidth = 0;
	ITEM* pItem = 0;
	RECT rect = {0};

	while(pos)
	{
		pGameNotifyMsg = NULL ;
		pGameNotifyMsg = (sGAMENOTIFY_MSG*)m_MsgList.GetNext(pos) ;

		if( pGameNotifyMsg )
		{
			tempList.AddTail(pGameNotifyMsg) ;

			pList = &(pGameNotifyMsg->itemList) ;

			if( pList )
			{
				lTextWidth = 0 ;

				pos2 = pList->GetHeadPosition() ;

				while(pos2)
				{
					pItem = NULL ;
					pItem = (ITEM*)pList->GetNext(pos2) ;

					if( pItem )
					{
						lTextWidth += CFONT_OBJ->GetTextExtentWidth(6, pItem->string, strlen(pItem->string)) ;
					}
				}
			}
		}
	}

	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
	int nStartXpos = (int)(dispInfo.dwWidth - lTextWidth)/2 ;

	pos = m_MsgList.GetHeadPosition() ;

	while(pos)
	{
		pGameNotifyMsg = NULL ;
		pGameNotifyMsg = (sGAMENOTIFY_MSG*)m_MsgList.GetNext(pos) ;

		if( pGameNotifyMsg )
		{
			pList = &(pGameNotifyMsg->itemList) ;

			if( pList )
			{
				if( pList->GetCount() == 0 )
				{
					m_MsgList.Remove(pGameNotifyMsg) ;

					delete pGameNotifyMsg ;
					pGameNotifyMsg = NULL ;

					continue ;
				}

				lTextWidth = 0 ;

				pos2 = pList->GetHeadPosition() ;

				while(pos2)
				{
					rect.left = nStartXpos + lTextWidth ;
					rect.top = pGameNotifyMsg->rect.top ;
					rect.right = 1 ;
					rect.bottom = 1 ;

					pItem = NULL ;
					pItem = (ITEM*)pList->GetNext(pos2) ;

					if( pItem )
					{
						if( pItem->nAlpha <= 10 )
						{
							pList->Remove(pItem) ;

							delete pItem ;
							pItem = NULL ;

							continue ;
						}

						DWORD dwFrontColor = RGBA_MERGE(pItem->rgb, pItem->nAlpha) ;
						DWORD dwBackColor  = RGBA_MERGE(RGB(10, 10, 10), pItem->nAlpha) ;
						CFONT_OBJ->RenderNoticeMsg( 6, pItem->string, lstrlen(pItem->string), &rect, dwFrontColor, dwBackColor);

						pItem->nAlpha -=2 ;

						lTextWidth += CFONT_OBJ->GetTextExtentWidth(6, pItem->string, strlen(pItem->string)) ;
					}
				}
			}

			pGameNotifyMsg->rect.top -= 1 ;
		}
	}
}

void CGameNotifyManager::UpdateSubQuest(SEND_SUBQUEST_UPDATE* msg) 						// 퀘스트 업데이트 내용을 공지하는 함수.
{
	if( !msg )
	{
		return ;
	}

	WORD wQuestIdx = 0 ;																// 퀘스트 인덱스를 담을 변수를 선언하고 0으로 세팅한다.
	wQuestIdx = msg->wQuestIdx ;														// 인자로 넘어온 메시지로 부터 퀘스트 인덱스를 받는다.

	WORD wSubQuestIdx = 0 ;																// 서브 퀘스트 인덱스를 담을 변수를 선언하고 0으로 세팅한다.
	wSubQuestIdx = msg->wSubQuestIdx ;													// 인자로 넘어온 메시지로 부터 서브 퀘스트 인덱스를 받는다.

	int nCombineKey = 0 ;
	COMBINEKEY(wQuestIdx, wSubQuestIdx, nCombineKey) ;
	CQuestString* pQuestString = QUESTMGR->GetQuestString(nCombineKey);

	if( pQuestString )
	{
		char tempBuf[1024] = {0, } ;													// 임시 버퍼를 선언한다.

		cPtrList* pTitleList = NULL ;													// 업데이트 스트링의 리스트를 받을 포인터를 선언하고 null처리를 한다.
		pTitleList = pQuestString->GetTitle() ;											// 업데이트 스트링의 리스트를 받는다.

		if( pTitleList )																// 리스트 정보가 유효하면,
		{
			sGAMENOTIFY_MSG* pNoticeMsg ;												// 게임 공지 메시지 구조체를 선언한다.
			pNoticeMsg = new sGAMENOTIFY_MSG ;
			pNoticeMsg->nNotifyType = eSTYLE_QUEST ;									// 공지 타입을 퀘스트로 세팅한다.

			pNoticeMsg->rect.left = 500 ;
			//pNoticeMsg->rect.top = 500 ;
			//pNoticeMsg->rect.top = HERO->GetTall() ;
			VECTOR3 OutPos3, inPos3;
			HERO->GetPosition(&inPos3);

			inPos3.y += 300 ;
			GetScreenXYFromXYZ(g_pExecutive->GetGeometry(), 0, &inPos3, &OutPos3);
			pNoticeMsg->rect.top = (LONG)(GAMERESRCMNGR->m_GameDesc.dispInfo.dwHeight*OutPos3.y); 

			pNoticeMsg->rect.right = 1 ;
			pNoticeMsg->rect.bottom = 500 ;

			pNoticeMsg->rect.top += ((CFONT_OBJ->GetTextHeight(6) + 5)* m_MsgList.GetCount()) ;


			ITEM* pQString = NULL ;													// 퀘스트 스트링을 받을 포인터를 선언하고 null처리를 한다.

			PTRLISTPOS titlePos = NULL ;												// 업데이트 할 스트링의 리스트 상의 위치를 받을 포인터를 선언하고 null처리 한다.
			titlePos = pTitleList->GetHeadPosition() ;									// 업데이트 할 스트링의 리스트를 헤드로 세팅한다.

			while(titlePos)																// 위치 정보가 유효할 동안 while문을 돌린다.
			{
				pQString = NULL ;														// 포인터를 null처리를 한다.
				pQString = (ITEM*)pTitleList->GetNext(titlePos) ;					// 위치에 따른 스트링 정보를 받는다.

				if( pQString )															// 스트링 정보가 유효하면,
				{
					strcat(tempBuf, pQString->string) ;									// 임시버퍼에 스트링을 담는다.

					ITEM* pNoticeItem = new ITEM ;										// 서브 퀘스트 아이템을 담을 아이템을 생성한다.

					pNoticeItem->rgb = dwQuestStringColor[eQC_SUBQUEST_NAME] ;			// 서브 퀘스트 스트링의 색상을 세팅한다.
					pNoticeItem->nAlpha = 255 ;

					/*pNoticeMsg->red = 0 ;
					pNoticeMsg->gree = 255 ;
					pNoticeMsg->blue =  0  ;
					pNoticeMsg->nAlpha = 255 ;*/

					strcpy(pNoticeItem->string, pQString->string) ;						// 서브 퀘스트 스트링을 복사한다.

					pNoticeMsg->itemList.AddTail(pNoticeItem);							// 공지 메시지 구조체의 리스트에 서브 퀘스트 아이템을 추가한다.
				}
			}

			char tempBuf2[128] = {0, } ;												// 두번째 임시 버퍼를 선언한다.

			if( msg->dwMaxCount == 0 )
			{
				sprintf(tempBuf2, " (%d)", msg->dwData) ;// 임시 버퍼에 현재 진행을 표현한다.
			}
			else
			{
				sprintf(tempBuf2, " (%d/%d)", msg->dwData, msg->dwMaxCount) ;// 임시 버퍼에 현재 진행을 표현한다.
			}

			strcat(tempBuf, tempBuf2) ;													// 첫 번째 임시 버퍼에 두 번째 임시 버퍼를 추가한다.

			pQuestString->SetTitleStr(tempBuf) ;										// 스트링의 타이틀을 세팅한다.


			ITEM* pCountItem = new ITEM ;												// 카운트를 담을 아이템을 생성한다.

			strcpy(pCountItem->string, tempBuf2) ;										// 카운트 아이템에 카운트 스트링을 복사한다.

			pCountItem->rgb = dwQuestStringColor[eQC_COUNT];							// 카운트 스트링의 색상을 세팅한다.
			pCountItem->nAlpha = 255 ;

			/*pNoticeMsg->red = 0 ;
			pNoticeMsg->gree = 0 ;
			pNoticeMsg->blue = 255  ;
			pNoticeMsg->nAlpha = 255 ;*/

			pNoticeMsg->itemList.AddTail(pCountItem) ;									// 공지 메시지 구조체의 리스트에 카운트 아이템을 추가한다.
			

			m_MsgList.AddTail(pNoticeMsg) ;												// 게임 공지 매니져에 공지 메시지를 추가한다.
		}
	}
}

void CGameNotifyManager::EndSubQuest(SEND_QUEST_IDX* msg) 								// 서브 퀘스트 종료시 내용을 공지하는 함수.
{
	if( !msg )
	{
		return ;
	}

	WORD wQuestIdx = 0 ;																// 퀘스트 인덱스를 담을 변수를 선언하고 0으로 세팅한다.
	wQuestIdx = msg->MainQuestIdx ;														// 인자로 넘어온 메시지로 부터 퀘스트 인덱스를 받는다.

	WORD wSubQuestIdx = 0 ;																// 서브 퀘스트 인덱스를 담을 변수를 선언하고 0으로 세팅한다.
	wSubQuestIdx = msg->SubQuestIdx ;													// 인자로 넘어온 메시지로 부터 서브 퀘스트 인덱스를 받는다.

	int nCombineKey = 0 ;
	COMBINEKEY(wQuestIdx, wSubQuestIdx, nCombineKey) ;
	CQuestString* pQuestString = QUESTMGR->GetQuestString(nCombineKey);

	if( pQuestString )																	// 퀘스트 스트링 정보가 유효하면,
	{
		char tempBuf[1024] = {0, } ;													// 임시 버퍼를 선언한다.

		cPtrList* pTitleList = NULL ;													// 업데이트 스트링의 리스트를 받을 포인터를 선언하고 null처리를 한다.
		pTitleList = pQuestString->GetTitle() ;											// 업데이트 스트링의 리스트를 받는다.

		if( pTitleList )																// 리스트 정보가 유효하면,
		{
			ITEM* pQString = NULL ;													// 퀘스트 스트링을 받을 포인터를 선언하고 null처리를 한다.

			PTRLISTPOS titlePos = NULL ;												// 업데이트 할 스트링의 리스트 상의 위치를 받을 포인터를 선언하고 null처리 한다.
			titlePos = pTitleList->GetHeadPosition() ;									// 업데이트 할 스트링의 리스트를 헤드로 세팅한다.

			sGAMENOTIFY_MSG* pNoticeMsg ;												// 게임 공지 메시지 구조체를 선언한다.
			pNoticeMsg = new sGAMENOTIFY_MSG ;
			pNoticeMsg->nNotifyType = eSTYLE_QUEST ;									// 공지 타입을 퀘스트로 세팅한다.

			pNoticeMsg->rect.left = 500 ;
			VECTOR3 OutPos3, inPos3;
			HERO->GetPosition(&inPos3);

			inPos3.y += 300 ;
			GetScreenXYFromXYZ(g_pExecutive->GetGeometry(), 0, &inPos3, &OutPos3);
			pNoticeMsg->rect.top = (LONG)(GAMERESRCMNGR->m_GameDesc.dispInfo.dwHeight*OutPos3.y); 
			pNoticeMsg->rect.right = 1 ;
			pNoticeMsg->rect.bottom = 500 ;

			pNoticeMsg->rect.top += ((CFONT_OBJ->GetTextHeight(6) + 5)* m_MsgList.GetCount()) ;

			while(titlePos)																// 위치 정보가 유효할 동안 while문을 돌린다.
			{
				pQString = NULL ;														// 포인터를 null처리를 한다.
				pQString = (ITEM*)pTitleList->GetNext(titlePos) ;					// 위치에 따른 스트링 정보를 받는다.

				if( pQString )															// 스트링 정보가 유효하면,
				{
					strcat(tempBuf, pQString->string) ;									// 임시버퍼에 스트링을 담는다.

					ITEM* pNoticeItem = new ITEM ;										// 서브 퀘스트 아이템을 담을 아이템을 생성한다.

					pNoticeItem->rgb = dwQuestStringColor[eQC_SUBQUEST_NAME] ;			// 서브 퀘스트 스트링의 색상을 세팅한다.
					pNoticeItem->nAlpha = 255 ;

					strcpy(pNoticeItem->string, tempBuf) ;								// 서브 퀘스트 스트링을 복사한다.

					pNoticeMsg->itemList.AddTail(pNoticeItem);							// 공지 메시지 구조체의 리스트에 서브 퀘스트 아이템을 추가한다.
				}				
			}

			char tempBuf2[128] = {0, } ;												// 두번째 임시 버퍼를 선언한다.

			sprintf(tempBuf2, CHATMGR->GetChatMsg(1463)) ;												// 임시 버퍼에 현재 진행을 표현한다.

			ITEM* pCountItem = new ITEM ;												// 카운트를 담을 아이템을 생성한다.

			strcpy(pCountItem->string, tempBuf2) ;										// 카운트 아이템에 카운트 스트링을 복사한다.

			pCountItem->rgb = dwQuestStringColor[eQC_COUNT];							// 카운트 스트링의 색상을 세팅한다.
			pCountItem->nAlpha = 255 ;

			pNoticeMsg->itemList.AddTail(pCountItem);
			m_MsgList.AddTail(pNoticeMsg);
		}
	}
}