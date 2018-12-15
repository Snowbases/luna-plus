#include "StdAfx.h"
#include "WindowIDEnum.h"
#include "interface\cFont.h"
#include "Interface\cListDialog.h"
#include "interface\cWindowManager.h"
#include "Audio/MHAudioManager.h"
#include "TutorialManager.h"
#include "Tutorial.h"
#include "Tutorial_Msg.h"
#include "Tutorialdlg.h"
#include "cImageSelf.h"
#include "MHFile.h"

const DWORD tutorialFontIndex = 7;

cTutorialDlg::cTutorialDlg(void) :
mCharacterSizePerLine(0),
mInvisibleTick(UINT_MAX),
mWishedAlpha(255),
m_NpcImage(new cImageSelf)
{
	SHORT_RECT rect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&rect,
		&value,
		&value);
	m_nDispWidth  = rect.right;
	m_nDispHeight = rect.bottom;

	m_nBtnStartXpos = 0 ;													// 버튼 시작 x좌표를 0으로 세팅한다.
	m_nBtnStartYpos = 0 ;													// 버튼 시작 y좌표를 0으로 세팅한다.

	m_vImagePos.x = 0 ;														// 출력 위치 x 를 0으로 세팅한다.
	m_vImagePos.y = 0 ;														// 출력 위치 y 를 0으로 세팅한다.
	m_pSpeechList	= NULL ;												// 튜토리얼 대사 리스트 다이얼로그를 null 처리한다.

	m_pOkBtn		= NULL ;												// 확인 버튼 포인터를 null 처리한다.
	m_pPrevBtn		= NULL ;												// 이전 버튼 포인터를 null 처리한다.
	m_pNextBtn		= NULL ;												// 다음 버튼 포인터를 null 처리한다.
	m_pPassBtn		= NULL ;												// 패스 버튼 포인터를 null 처리한다.

	m_pSpeechBack	= NULL ;												// 대사 배경 스태틱 포인터를 null 처리한다.

	m_pTutorial = NULL ;													// 멤버 튜토리얼 포인터를 null 처리를 한다.
	m_pCurSubTutorial = NULL ;												// 멤버 현재 서브 튜토리얼 포인터를 null 처리를 한다.
	m_nCurSubIndex = 0 ;													// 현재 서브 튜토리얼 인덱스를 0으로 세팅한다.
	m_nTotalSubCount = 0 ;													// 총 서브 튜토리얼 카운트를 0으로 세팅한다.
}

cTutorialDlg::~cTutorialDlg(void)
{
	SAFE_DELETE(m_NpcImage);
}

void cTutorialDlg::Linking() 												// 다이얼로그 하위 컨트롤들을 링크하는 함수.
{
	m_pSpeechList = (cListDialog*)GetWindowForID(TTR_SPEECH_LIST) ;			// 대사 리스트 다이얼로그를 링크한다.
	m_pSpeechList->SetStyle(LDS_FONTSHADOW) ;								// 그림자 출력되는 리스트 스타일로 세팅한다.
	m_pSpeechList->Set_ShadowFontIndex(tutorialFontIndex) ;					// 출력 폰트를 세팅한다.
	m_pSpeechList->Set_ShadowDistance(2) ;									// 그림자 거리를 2로 세팅한다.

	m_pOkBtn = GetWindowForID(TTR_OKBTN);
	m_pOkBtn->SetActive(FALSE);

	m_pPrevBtn = GetWindowForID(TTR_PREVBTN);
	m_pPrevBtn->SetActive(FALSE);

	m_pNextBtn = GetWindowForID(TTR_NEXTBTN);
	m_pNextBtn->SetActive(FALSE);

	m_pPassBtn = GetWindowForID(TTR_PASSBTN);
	m_pPassBtn->SetActive(FALSE);

	m_pSpeechBack = GetWindowForID(TTR_SP_BACK);
	m_pSpeechBack->SetActive(FALSE);

	// 100111 LUJ, 라인별 표시 가능한 글자 개수를 얻는다
	cImageRect* const backImageRect = m_pSpeechBack->GetBasicImage()->GetImageRect();
	mCharacterSizePerLine = abs(backImageRect->right - backImageRect->left) / CFONT_OBJ->GetTextExtentWidth(tutorialFontIndex, "?", 1);
}





//=========================================================================
//		튜토리얼 다이얼로그 렌더 함수.
//=========================================================================
void cTutorialDlg::Render()													// 렌더 함수.
{
	if( !IsActive() ) return ;												// 활성화 상태가 아니면, return 처리를 한다.

	if(mWishedAlpha < m_alpha)
	{
		SetAlpha(
			BYTE(max(int(mWishedAlpha), int(m_alpha) - 5)));
		m_pSpeechList->SetAlpha(
			BYTE(max(int(mWishedAlpha), int(m_alpha) - 5)));
	}
	else if(mWishedAlpha > m_alpha)
	{
		SetAlpha(
			BYTE(min(int(mWishedAlpha), int(m_alpha) + 5)));
		m_pSpeechList->SetAlpha(
			BYTE(min(int(mWishedAlpha), int(m_alpha) + 5)));
	}
	else if(mWishedAlpha == 0)
	{
		SetActive(
			FALSE);
		return;
	}

	m_NpcImage->RenderSprite(
		0,
		0,
		0,
		&m_vImagePos,
		RGBA_MAKE(255,255,255, m_alpha));

	cDialog::RenderWindow();												// 윈도우를 렌더한다.
	cDialog::RenderComponent();												// 하위 객체들을 렌더한다.

	if(mInvisibleTick < gCurTime)
	{
		mWishedAlpha = 0;
	}
}

void cTutorialDlg::OnActionEvent(LONG lId, void* p, DWORD we) 				// 지정 된 컨트롤 이벤트를 처리 할 함수.
{
	switch(lId)																// 컨트롤 아이디를 확인한다.
	{
	case TTR_OKBTN :														// 확인 버튼을 눌렀다면,
		{
			if(0 == m_pTutorial)
			{
				SetActive(FALSE);
				break;
			}

			m_pTutorial->Set_Complete(TRUE) ;
			UpdateTutorial() ;												// 튜토리얼을 종료하고, 업데이트 한다.
			// 090417 ONS 튜토리얼 다이얼로그 Off상태 
			TUTORIALMGR->SetOnOffTutorial(FALSE);							
		}
		break ;

	case TTR_PREVBTN :														// 이전 버튼을 눌렀다면,
		{
			if( m_nCurSubIndex <= 0 )
			{
				// 에러처리 한다~~~!!!
				// 0번 서브 퀘에서 이전 버튼이 떠??!!!
				return ;
			}

			SUB_TUTORIAL* pPrevSubTutorial = NULL ;
			pPrevSubTutorial = m_pTutorial->Get_SubTutorial(m_nCurSubIndex-1) ;		// 이전 서브 튜토리얼 정보를 받는다.

			if( pPrevSubTutorial )											// 이전 서브 튜토리얼이 있을경우,
			{
				--m_nCurSubIndex ;											// 현재 튜토리얼 인덱스를 감소한다.
				Prepare_Tutorial() ;										// 튜토리얼 진행을 준비한다.
			}
		}
		break ;

	case TTR_NEXTBTN :
		{
			SUB_TUTORIAL* pNextSubTutorial = m_pTutorial->Get_SubTutorial(m_nCurSubIndex+1) ;

			if( pNextSubTutorial )											// 다음 서브 튜토리얼이 있을경우,
			{
				++m_nCurSubIndex ;											// 현재 튜토리얼 인덱스를 증가한다.
				Prepare_Tutorial() ;										// 튜토리얼 진행을 준비한다.
			}
		}
		break ;

	case TTR_PASSBTN :														// 통과 버튼을 눌렀다면,
		{
			if(0 == m_pTutorial)
			{
				SetActive(FALSE);
				break;
			}

			m_pTutorial->Set_Complete(TRUE) ;
			UpdateTutorial() ;												// 튜토리얼을 종료하고, 업데이트 한다.
			// 090417 ONS 튜토리얼 다이얼로그 Off상태 
			TUTORIALMGR->SetOnOffTutorial(FALSE);							

		}
		break ;
	}
}

void cTutorialDlg::Set_TutorialInfo(cTutorial* pTutorial)
{
	m_pTutorial = pTutorial;
}

void cTutorialDlg::Start_Tutorial() 										// 튜토리얼 진행함수.
{
	m_nCurSubIndex = 0 ;													// 현재 서브 튜토리얼 인덱스를 0으로 세팅한다.
	m_nTotalSubCount = m_pTutorial->Get_SubTutorial_Count() ;				// 현재 튜토리얼의 서브 튜토리얼 카운트를 받는다.

	Prepare_Tutorial();
	SetActive(
		TRUE);
	m_pSpeechBack->SetActive(
		TRUE);

	if(cDialog* dialog = WINDOWMGR->GetWindowForID(NSI_SCRIPTDLG))
	{
		dialog->SetActive(FALSE);
	}
}

void cTutorialDlg::LoadNpcImage()
{
	if(0 == m_pCurSubTutorial)
	{
		return;
	}

	m_NpcImage->Release();

	CMHFile file;
	file.Init("Data/Interface/Windows/NpcImageList.bin","rb");

	while(FALSE == file.IsEOF())
	{
		WORD wIndex  = file.GetWord();											// 인덱스를 읽어들인다.
		BYTE emotion = file.GetByte();											// 모션을 읽어들인다.
		char*	szFileName	=	file.GetString();							// 파일명을 세팅한다.

		if( strlen(szFileName) == 0 )
		{
			break;
		}

		if( wIndex != m_pCurSubTutorial->nNpc_Idx ) continue ;				// npc 인덱스가 같지 않으면 continue 처리를 한다.

		if( emotion != m_pCurSubTutorial->nFace_Idx ) continue ;			// emotion 인덱스가 같지 않으면, continue 처리를 한다.

		char buf[1024] = {0};
		sprintf( buf, "%s%s", "data/interface/2dImage/npcImage/", szFileName);
		m_NpcImage->LoadSprite(buf);

		m_vImagePos.x = (float)(m_nDispWidth - m_NpcImage->GetImageRect()->right);
		m_vImagePos.y = (float)(m_nDispHeight - m_NpcImage->GetImageRect()->bottom);
		break;
	}
}





//=========================================================================
//		튜토리얼 진행을 위한 준비 처리 함수.	
//=========================================================================
void cTutorialDlg::Prepare_Tutorial() 										// 튜토리얼 진행을 미리 준비하는 함수.
{
	m_pCurSubTutorial = m_pTutorial->Get_SubTutorial(m_nCurSubIndex) ;		// 현재 서브 튜토리얼 정보를 받는다.

	if( !m_pTutorial )														// 튜토리얼 정보가 유효하지 않으면,
	{
		return;															// return 처리를 한다.
	}
	else if( !m_pCurSubTutorial )												// 서브 튜토리얼 정보를 받는데 실패 했다면,
	{
		#ifdef _GMTOOL_														// GM툴일 경우,
		char tempBuf[128] = {0, } ;											// 에러 출력용 임시 버퍼를 선언한다.
		sprintf(tempBuf, "Failed to receive subtutorial info : %d", m_pTutorial->Get_Index()) ;			// 서브 튜토리얼 받기 실패~!!
		MessageBox(NULL, tempBuf, "cTutorialDlg::Prepare_Tutorial", MB_OK);	// 에러 메시지를 메시지 박스로 띄운다.
		#endif
		return ;															// return 처리를 한다.
	}

	cTutorial_Msg* pMsg = TUTORIALMGR->Get_TutorialMsg( m_pCurSubTutorial->nSpeech_Idx );

	if( !pMsg )																// 메시지를 받는데 실패했다면,
	{
		return ;															// return 처리를 한다.
	}

	m_pSpeechList->RemoveAll() ;											// 대사 출력 리스트를 모두 비운다.

	for( int count = 0 ; count < pMsg->Get_LineCount() ; ++count )						// 메시지 라인 수 만큼 for문을 돌린다.
	{
		MSG_LINE* pMsgLine = pMsg->Get_MsgLine(count) ;								// 카운트에 해당하는 메시지 라인을 받는다.

		if( !pMsgLine )
		{
			continue ;														// continue 처리를 한다.
		}

		m_pSpeechList->AddItem(pMsgLine->msg, RGB(255, 255, 255), count) ;	// 튜토리얼 대사를 추가한다.

        if( count != (pMsg->Get_LineCount() - 1) ) continue ;							// 현재 카운트가, 마지막 카운트가 아니면 continue 처리를 한다.

		m_nBtnStartXpos = int(m_pSpeechList->GetAbsX() + 
						  CFONT_OBJ->GetTextWidth(tutorialFontIndex) + 
					      CFONT_OBJ->GetTextExtentWidth(tutorialFontIndex, pMsgLine->msg, strlen(pMsgLine->msg)) ) ;			// 버튼 출력의 시작 x좌표를 구한다.

		m_nBtnStartYpos = int(m_pSpeechList->GetAbsY() + 6 + 
						  (CFONT_OBJ->GetTextHeight(tutorialFontIndex) * (pMsg->Get_LineCount() - 1)));
	}

	DeActiveAllBtn();

	switch(m_pCurSubTutorial->nComplete_Condition1)
	{
	case e_CLICK_OKBTN:
		{
			ActiveBtns();
			break;
		}
	case e_OPEN_DIALOG:
		{
			cWindow* pWindow = WINDOWMGR->GetWindowForID(IDSEARCH(m_pCurSubTutorial->szCompleteCondition2)) ;

			if( pWindow )
			{
				pWindow->SetActive(FALSE) ;
			}

			break;
		}
	}

	LoadNpcImage();

	AUDIOMGR->StopAll();
	AUDIOMGR->Play(
		m_pCurSubTutorial->nSound_Idx,
		gHeroID);
}

void cTutorialDlg::DeActiveAllBtn() 										// 모든 버튼을 비활성화 하는 함수.
{
	m_pOkBtn->SetActive(FALSE) ;											// 확인 버튼을 비활성화 한다.
	m_pPrevBtn->SetActive(FALSE) ;											// 이전 버튼을 비활성화 한다.
	m_pNextBtn->SetActive(FALSE) ;											// 다음 버튼을 비활성화 한다.
	m_pPassBtn->SetActive(FALSE) ;											// 통과 버튼을 비활성화 한다.
}

void cTutorialDlg::ActiveBtns() 											// 상황에 맞는 버튼을 활성화 하는 함수.
{
	int nCurSubIndex = m_pCurSubTutorial->nIndex ;							// 현재 서브 튜토리얼의 인덱스를 구한다.
	int nLastSubIndex = m_nTotalSubCount-1 ;								// 마지막 서브 튜토리얼의 인덱스를 구한다.

	int nBtnDistance = 5 ;													// 버튼 사이 거리 값을 5로 세팅한다.

	if( nCurSubIndex == 0 )													// 현재 서브 튜토리얼 인덱스가 0과 같으면,
	{
		if( nCurSubIndex == nLastSubIndex )									// 처음이자 마지막이면,
		{
			m_pOkBtn->SetAbsXY(m_nBtnStartXpos, m_nBtnStartYpos) ;			// ok 버튼의 위치를 잡고,
			m_pOkBtn->SetActive(TRUE) ;										// 활성화 시킨다.
		}
		else																// 다수 페이지 중 처음이면,
		{
			m_pNextBtn->SetAbsXY(m_nBtnStartXpos, m_nBtnStartYpos) ;		// ok 버튼의 위치를 잡고,
			m_pNextBtn->SetActive(TRUE) ;									// 활성화 시킨다.

			int nSecontX = m_nBtnStartXpos + m_pNextBtn->GetWidth() + nBtnDistance ;	// 다음 버튼의 시작 좌표를 구한다.

			m_pPassBtn->SetAbsXY(nSecontX, m_nBtnStartYpos) ;				// 다음 버튼의 위치를 잡고,
			m_pPassBtn->SetActive(TRUE) ;									// 활성화 시킨다.
		}
	}
	else if( nCurSubIndex == nLastSubIndex )								// 현재 서브 튜토리얼이 마지막 튜토리얼이면,
	{
		if(m_nTotalSubCount > 1)											// 서브 튜토리얼 총 페이지가 1페이지 이상이면,
		{
			m_pPrevBtn->SetAbsXY(m_nBtnStartXpos, m_nBtnStartYpos) ;		// 이전 버튼의 위치를 잡고,
			m_pPrevBtn->SetActive(TRUE) ;									// 활성화 시킨다.

			int nSecontX = m_nBtnStartXpos + m_pPrevBtn->GetWidth() + nBtnDistance ;	// 확인 버튼의 좌표를 구한다.

			m_pOkBtn->SetAbsXY(nSecontX, m_nBtnStartYpos) ;					// 확인 버튼의 위치를 잡고,
			m_pOkBtn->SetActive(TRUE) ;										// 활성화 시킨다.
		}
		else																// 현재 서브 튜토리얼 페이지가 마지막 페이지라면,
		{
			m_pOkBtn->SetAbsXY(m_nBtnStartXpos, m_nBtnStartYpos) ;			// 확인 버튼의 위치를 잡고,
			m_pOkBtn->SetActive(TRUE) ;										// 활성화 시킨다.
		}
	}
	else																	// 첫페이지나 마지막 페이지가 아니면,
	{		
		m_pPrevBtn->SetAbsXY(m_nBtnStartXpos, m_nBtnStartYpos) ;			// 이전 버튼의 위치를 잡는다.
		m_pPrevBtn->SetActive(TRUE) ;										// 활성화 시킨다.
	
		int nSecontX = m_nBtnStartXpos + m_pPrevBtn->GetWidth() + nBtnDistance ;	// 다음 버튼의 시작 좌표를 구한다.

		m_pNextBtn->SetAbsXY(nSecontX, m_nBtnStartYpos) ;					// 다음 버튼의 위치를 잡고,
		m_pNextBtn->SetActive(TRUE) ;										// 활성화 시킨다.

		int nThirdX = nSecontX + m_pNextBtn->GetWidth() + nBtnDistance ;	// 통과 버튼의 시작 좌표를 구한다.

		m_pPassBtn->SetAbsXY(nThirdX, m_nBtnStartYpos) ;					// 통과 버튼의 위치를 잡고,
		m_pPassBtn->SetActive(TRUE) ;										// 활성화 시킨다.
	}
}





//=========================================================================
//		튜토리얼 종료 조건과 같은 npc 인지 체크하는 함수.
//=========================================================================
BOOL cTutorialDlg::IsSameEndNpc(DWORD dwNpcIdx) 							// 튜토리얼 종료 조건과 같은 npc 인지 체크하는 함수.
{
	if( !m_pCurSubTutorial ) return FALSE ;

	int nEndCondition = m_pCurSubTutorial->nComplete_Condition1 ;				// 첫 번재 완료 조건을 받는다.

	if( nEndCondition != e_NPC_TALK )										// 완료 조건 첫번째가 npc와 대화가 아니면,
	{
		return FALSE ;														// false return 처리를 한다.
	}

	return dwNpcIdx == DWORD(m_pCurSubTutorial->nComplete_Condition2);
}

BOOL cTutorialDlg::IsHaveNextSubTutorial()									// 다음 서브 튜토리얼이 있는지 여부를 체크하는 함수.
{
	return 0 < m_pTutorial->Get_SubTutorial(m_nCurSubIndex+1);
}





//=========================================================================
//		다음 서브 튜토리얼을 진행하는 함수.
//=========================================================================
void cTutorialDlg::StartNextSubTutorial() 									// 다음 서브 튜토리얼을 진행하는 함수.
{
	SUB_TUTORIAL* pNextSubTutorial = m_pTutorial->Get_SubTutorial(m_nCurSubIndex+1) ;		// 현재 서브 튜토리얼 정보를 받는다.

	if( pNextSubTutorial )													// 다음 서브 튜토리얼이 있을경우,
	{
		++m_nCurSubIndex ;													// 현재 튜토리얼 인덱스를 증가한다.
		Prepare_Tutorial() ;												// 튜토리얼 진행을 준비한다.
	}
}





//=========================================================================
//		다음 서브 튜토리얼을 진행하는 함수.
//=========================================================================
void cTutorialDlg::UpdateTutorial() 										// 튜토리얼을 완료하고 업데이트 하는 함수.
{
	SetActive(
		FALSE);
	TUTORIALMGR->UpdateTutorial(
		m_pTutorial->Get_Index());
}

SUB_TUTORIAL* cTutorialDlg::GetCurSubTutorial()
{
	return m_pCurSubTutorial;
}

void cTutorialDlg::SetActive(BOOL isActive)
{
	if(FALSE == isActive)
	{
		mInvisibleTick = UINT_MAX;
		mWishedAlpha = 255;

		SetAlpha(
			255);
	}

	cDialog::SetActive(isActive);

	cDialog* const tutorialNoticeDialog = WINDOWMGR->GetWindowForID(TUTORIALBTN_DLG);

	if(tutorialNoticeDialog)
	{
		tutorialNoticeDialog->SetActive(! isActive);
	}
}

void cTutorialDlg::SetNpcImage(LPCTSTR fileName)
{
	TCHAR path[MAX_PATH] = {0};
	_stprintf(
		path,
		"data/interface/2dImage/npcImage/%s",
		fileName);

	m_NpcImage->Release();
	m_NpcImage->LoadSprite(
		path);
	m_vImagePos.x = (float)(m_nDispWidth - m_NpcImage->GetImageRect()->right);
	m_vImagePos.y = (float)(m_nDispHeight - m_NpcImage->GetImageRect()->bottom);	
}

void cTutorialDlg::SetText(LPCTSTR text)
{
	if(0 == _tcslen(text))
	{
		mWishedAlpha = 0;
		return;
	}

	m_pSpeechList->RemoveAll();

	// 110111 LUJ, 반으로 나눠쓰지 않으면 메모리 폴트가 발생한다
	TCHAR itemText[MAX_LISTITEM_SIZE / 2] = {0};

	do
	{
		ZeroMemory(
			itemText,
			sizeof(itemText));
		_mbsnbcpy(
			PUCHAR(itemText),
			PUCHAR(text),
			mCharacterSizePerLine);
		m_pSpeechList->AddItem(
			LPTSTR(itemText),
			RGB(255, 255, 255));
		text += _tcslen(itemText);
	} while (*text);

	if(FALSE == IsActive())
	{
		SetAlpha(0);
	}

	m_pSpeechBack->SetActive(TRUE);
	mWishedAlpha = 255;
	
	DeActiveAllBtn();

	m_pOkBtn->SetAbsXY(
		LONG(m_pSpeechList->GetAbsX() + CFONT_OBJ->GetTextWidth(tutorialFontIndex) + CFONT_OBJ->GetTextExtentWidth(tutorialFontIndex, LPTSTR(itemText), strlen(itemText))),
		LONG(m_pSpeechList->GetAbsY() + 6 + CFONT_OBJ->GetTextHeight(tutorialFontIndex) * (m_pSpeechList->GetItemCount() - 1)));
	m_pOkBtn->SetActive(TRUE);
}

void cTutorialDlg::SetInvisibleSecond(DWORD second)
{
	mInvisibleTick = gCurTime + second * 1000;
}