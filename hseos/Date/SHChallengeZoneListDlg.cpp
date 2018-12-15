#include "stdafx.h"
#include "SHChallengeZoneListDlg.h"
#include "GameIn.h"
#include "GameResourceManager.h"
#include "WindowIDEnum.h"
#include "cStatic.h"
#include "cScriptManager.h"
#include "cResourceManager.h"
#include "cRITEMEx.h"
#include "../hseos/Date/SHDateManager.h"
#include "../hseos/ResidentRegist/SHResidentRegistManager.h"
#include "cGuageBar.h"
#include "cButton.h"
#include "ObjectManager.h"

CSHChallengeZoneListDlg::CSHChallengeZoneListDlg()
{
	m_nSelChallengeZone = 0;
	m_GuageRightBtn		= NULL;
	m_GuageLeftBtn		= NULL;
	m_MoveBtn			= NULL;
	m_CancleBtn			= NULL;
	m_GuageBar			= NULL;
	m_GuageManRate		= NULL;
	m_GuageWomanRate	= NULL;
	PushedGuageBtn		= FALSE;
}

CSHChallengeZoneListDlg::~CSHChallengeZoneListDlg()
{
	m_pcsChallengeZoneList->DeleteAllItems() ;
}

void CSHChallengeZoneListDlg::SetActive( BOOL val )
{
	cDialog::SetActive( val );

	// 091120 ONS 경험치 분배 게이지바 초기값 설정
	if(val)
	{
		InitControls();

		m_GuageBar->SetCurRate(0.5f);
		m_GuageManRate->SetStaticText(" 50%");
		m_GuageWomanRate->SetStaticText(" 50%");
	}
}

void CSHChallengeZoneListDlg::Linking()
{
	m_pcsChallengeZoneList = ( cListCtrl* )GetWindowForID( DMD_CHALLENGEZONE_MOVELIST_LISTCTRL ) ;

	// 091120 ONS 경험치 분배 버튼/게이지바 설정
	m_GuageRightBtn			= ( cButton* )GetWindowForID( DMD_DATEZONE_BTN_RIGHT );
	m_GuageLeftBtn			= ( cButton* )GetWindowForID( DMD_DATEZONE_BTN_LEFT );	
	m_MoveBtn				= ( cButton* )GetWindowForID( DMD_CHALLENGEZONE_MOVELIST_OK );
	m_ConfirmBtn			= ( cButton* )GetWindowForID( DMD_CHALLENGEZONE_CONFIRM );
	m_CancleBtn				= ( cButton* )GetWindowForID( DMD_CHALLENGEZONE_MOVELIST_CANCEL );
	m_GuageBar				= ( cGuageBar* )GetWindowForID( DMD_DATEZONE_BTN_SCROLL );	
	m_Confirm				= ( cStatic* )GetWindowForID( DMD_CHALLENGEZONE_NOTICE );	
	m_GuageManRate			= ( cStatic* )GetWindowForID( DMD_DATEZONE_VALUE_MAN );	
	m_GuageWomanRate		= ( cStatic* )GetWindowForID( DMD_DATEZONE_VALUE_WOMAN );		


	for(int i=0; i<m_pcsChallengeZoneList->GetRowsNum(); i++)
	{
		cRITEMEx *ritem = new cRITEMEx(1);

		strcpy(ritem->pString[0], RESRCMGR->GetMsg(m_pcsChallengeZoneList->GetRowsMsgID(i)));
		if (i == 0) ritem->rgb[0] = RGBA_MAKE(255, 255, 255, 255);
		else		ritem->rgb[0] = RGBA_MAKE(10, 10, 10, 255);

		m_pcsChallengeZoneList->InsertItem(
			WORD(i),
			ritem);
	}
} 

void CSHChallengeZoneListDlg::Render()
{
 	if( !IsActive() ) return;

	cDialog::Render();

	// 091120 ONS 경험치 분배 게이지값 출력
	if(m_GuageBar->IsDrag() || PushedGuageBtn)
	{
		WORD dwWomanRate = (WORD)(m_GuageBar->GetCurRate()*100);
		WORD dwManRate = 100 - dwWomanRate;

		char BufManRate[12] = {0, } ;
		sprintf(BufManRate, "%3u%c", dwManRate, '%');
		m_GuageManRate->SetStaticText(BufManRate);

		char BufWomanRate[12] = {0, } ;
		sprintf(BufWomanRate, "%3u%c", dwWomanRate, '%');
		m_GuageWomanRate->SetStaticText(BufWomanRate);

		PushedGuageBtn	= FALSE;
	}
}

void CSHChallengeZoneListDlg::OnActionEvent( LONG lId, void* p, DWORD we )																	
{
 	if( we & WE_BTNCLICK )																											
	{
 		switch( lId )																												
		{
		case DMD_CHALLENGEZONE_MOVELIST_OK:
			{
				// 091124 ONS 이동버튼 누를때 경험치비율을 함께 서버로 전달한다.
				DWORD dwManExpRate = (DWORD)(m_GuageBar->GetCurRate()*100);
				g_csDateManager.CLI_RequestDateMatchingEnterChallengeZone(m_nSelChallengeZone, dwManExpRate);
				SetActive(FALSE);
			}
			break;
		case DMD_CHALLENGEZONE_CONFIRM:
			{
				if(m_bDiffAgent)
				{
					MSG_DWORD6 stPacket;
					ZeroMemory(&stPacket, sizeof(stPacket));

					stPacket.Category 		= MP_DATE;
					stPacket.Protocol 		= MP_DATE_ENTER_CHALLENGEZONE_2_DIFF_AGENT;
					stPacket.dwObjectID		= HEROID;
					stPacket.dwData1		= 0;
					stPacket.dwData2		= m_dwPartnerIndex;
					stPacket.dwData3		= m_dwDiffAgentID;
					stPacket.dwData4		= m_dwZoneIndex;
					stPacket.dwData5		= RESULT_OK;
					if (HERO->GetObjectBattleState() == eObjectBattleState_Battle)
					{
						stPacket.dwData5 = RESULT_FAIL_01;
					}
					// 091223 ONS 경험치율 전달 처리 추가
					stPacket.dwData6		= m_dwExpRate;

					NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
				}
				else
				{
					MSG_DWORD3 stPacket;
					stPacket.Category 		= MP_DATE;
					stPacket.Protocol 		= MP_DATE_ENTER_CHALLENGEZONE_2;
					stPacket.dwObjectID		= HEROID;
					stPacket.dwData1		= m_dwZoneIndex;
					stPacket.dwData2		= RESULT_OK;
					if (HERO->GetObjectBattleState() == eObjectBattleState_Battle)
					{
						stPacket.dwData2 = RESULT_FAIL_01;
					}

					stPacket.dwData3		= m_dwExpRate;
					NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
				}
			}
			break;
		case DMD_CHALLENGEZONE_MOVELIST_CANCEL:
			g_csResidentRegistManager.CLI_CancelDateMatchingChat();
			SetActive(FALSE);
			break;
		// 091124 ONS 경험치비율조절 좌우 버튼처리 추가
		case DMD_DATEZONE_BTN_LEFT:
		case DMD_DATEZONE_BTN_RIGHT:
			{
				SetDistributeRateByBtn( lId );
			}
			break;
		}
	}
}

// 091124 ONS 좌/우 버튼을 이용해서 경험치를 25%단위로 조절한다.
void CSHChallengeZoneListDlg::SetDistributeRateByBtn(DWORD dwWindowID)
{
	float fCurRate = m_GuageBar->GetCurRate();
	if( 0 == ((WORD)(fCurRate*100))%25 )
	{
		if(dwWindowID == DMD_DATEZONE_BTN_RIGHT && fCurRate < 1.0f)
			m_GuageBar->SetCurRate(fCurRate + 0.25f);
		if(dwWindowID == DMD_DATEZONE_BTN_LEFT && fCurRate > 0.0f)
			m_GuageBar->SetCurRate(fCurRate - 0.25f);
	}
	else
	{
		if( fCurRate > 0.0f && fCurRate < 0.25f )
		{
			if(dwWindowID == DMD_DATEZONE_BTN_RIGHT)
				m_GuageBar->SetCurRate(0.25f);
			else
				m_GuageBar->SetCurRate(0.0f);
		}
		else if( fCurRate > 0.25f && fCurRate < 0.5f )
		{
			if(dwWindowID == DMD_DATEZONE_BTN_RIGHT)
				m_GuageBar->SetCurRate(0.5f);
			else
				m_GuageBar->SetCurRate(0.25f);
		}
		else if( fCurRate > 0.5f && fCurRate < 0.75f )
		{
			if(dwWindowID == DMD_DATEZONE_BTN_RIGHT)
				m_GuageBar->SetCurRate(0.75f);
			else
				m_GuageBar->SetCurRate(0.5f);
		}
		else if( fCurRate > 0.75f && fCurRate < 1.0f )
		{
			if(dwWindowID == DMD_DATEZONE_BTN_RIGHT)
				m_GuageBar->SetCurRate(1.0f);
			else
				m_GuageBar->SetCurRate(0.75f);
		}
		else
		{
			if(dwWindowID == DMD_DATEZONE_BTN_RIGHT)
				m_GuageBar->SetCurRate(1.0f);
			else
				m_GuageBar->SetCurRate(0.75f);
		}
	}

	PushedGuageBtn		= TRUE;
}

DWORD CSHChallengeZoneListDlg::ActionEvent(CMouse* mouseInfo)
{
	DWORD we = WE_NULL;
	if( !m_bActive ) return we;
	we = cDialog::ActionEvent(mouseInfo);
	
	int nRowIndex = m_pcsChallengeZoneList->GetSelectedRowIdx();
	if(we & WE_ROWCLICK)
	{
		SelectChallengeZone(nRowIndex);
	}
	else if( we & WE_ROWDBLCLICK )
	{
		// 091124 ONS 더블클릭시 경험치비율을 서버로 전달한다.
		DWORD dwManExpRate = (DWORD)(m_GuageBar->GetCurRate()*100);
		g_csDateManager.CLI_RequestDateMatchingEnterChallengeZone(nRowIndex, dwManExpRate);
		SetActive(FALSE);
	}
	return we;
}

void CSHChallengeZoneListDlg::SelectChallengeZone(int nRowIndex)
{
	for(int i=0; i<m_pcsChallengeZoneList->GetItemCount(); i++)
	{
		cRITEMEx* pRItem = (cRITEMEx*)m_pcsChallengeZoneList->GetRItem(WORD(i));
		if (i == nRowIndex)
		{
			pRItem->rgb[0] = RGBA_MAKE(255, 255, 255, 255);
			m_nSelChallengeZone = nRowIndex;
		}
		else
		{
			pRItem->rgb[0] = RGBA_MAKE(10, 10, 10, 255);
		}
	}
}

void CSHChallengeZoneListDlg::InitControls()
{
	m_pcsChallengeZoneList->SetDisable(FALSE);
	m_GuageRightBtn->SetDisable(FALSE);
	m_MoveBtn->SetActive(TRUE);
	m_ConfirmBtn->SetActive(FALSE);
	m_Confirm->SetActive(FALSE);
	m_GuageLeftBtn->SetDisable(FALSE);
	m_GuageBar->SetDisable(FALSE);

	m_bDiffAgent = FALSE;
	m_dwZoneIndex = 0;
	m_dwExpRate = 0;
	m_dwDiffAgentID = 0;
	m_dwPartnerIndex = 0;
}

void CSHChallengeZoneListDlg::SetGuestMode(DWORD dwZoneIndex, DWORD dwExpRate, BOOL bDiffAgent, DWORD dwDiffAgentID, DWORD dwPartnerIndex)
{
	m_bDiffAgent = bDiffAgent;
	m_dwZoneIndex = dwZoneIndex;
	m_dwExpRate = dwExpRate;
	m_dwDiffAgentID = dwDiffAgentID;
	m_dwPartnerIndex = dwPartnerIndex;
	int nZoneIndex = (int)m_dwZoneIndex;
	float fExpRate = (float)m_dwExpRate/100.0f;

	m_pcsChallengeZoneList->SetDisable(TRUE);
	SelectChallengeZone(nZoneIndex);

	m_GuageRightBtn->SetDisable(TRUE);
	m_MoveBtn->SetActive(FALSE);
	m_ConfirmBtn->SetActive(TRUE);
	m_Confirm->SetActive(TRUE);
	m_GuageLeftBtn->SetDisable(TRUE);
	m_GuageBar->SetDisable(TRUE);
	m_GuageBar->SetCurRate(fExpRate);

	WORD dwManRate = WORD(100 - m_dwExpRate);
	char BufManRate[12] = {0, } ;
	sprintf(BufManRate, "%3u%c", dwManRate, '%');
	m_GuageManRate->SetStaticText(BufManRate);

	char BufWomanRate[12] = {0, } ;
	sprintf(BufWomanRate, "%3u%c", m_dwExpRate, '%');
	m_GuageWomanRate->SetStaticText(BufWomanRate);
}
