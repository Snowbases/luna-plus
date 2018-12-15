#include "stdafx.h"
#include ".\gtentryeditdlg.h"
#include "WindowIDEnum.h"
#include "interface/cListDialog.h"
#include "interface/cStatic.h"
#include "GuildDialog.h"
#include "GameIn.h"
#include "ObjectManager.h"
#include "ChatManager.h"

CGTEntryEditDlg::CGTEntryEditDlg(void)
{
	m_wLastSec = 0;
	m_bChangedColor = FALSE;
}

CGTEntryEditDlg::~CGTEntryEditDlg(void)
{
}

void CGTEntryEditDlg::Linking()
{
	m_pGMemberList = (cListDialog*)GetWindowForID( GTGMEMBERLIST );
	m_pEntryList = (cListDialog*)GetWindowForID( GTENTRYLIST );
	m_pRemainTime = (cStatic*)GetWindowForID( GTREMAINDTIME );
	m_pEntryChange = (cButton*)GetWindowForID( GTENTRYCHANGE );

	if(m_pGMemberList)
	{
		m_pGMemberList->SetShowSelect( TRUE );
		m_pGMemberList->SetHeight( 158 );
	}
	
	if(m_pEntryList)
	{
		m_pEntryList->SetShowSelect( TRUE );
		m_pEntryList->SetHeight( 158 );
	}

	m_wSelectedList = GT_ENTRYEDIT_MAX;
}

void CGTEntryEditDlg::SetActive(BOOL val )
{
	if( m_bDisable ) return;

	if(val == TRUE)
	{
		m_wLastSec = 0;
		m_bChangedColor = FALSE;
	}
	else
	{
	}

	cDialog::SetActive(val);
}

DWORD CGTEntryEditDlg::ActionEvent(CMouse * mouseInfo)
{
	DWORD we = WE_NULL;
	if( !m_bActive ) return we;
	we = cDialog::ActionEvent(mouseInfo);
	
	if(we & WE_LBTNCLICK)
	{
		int nCurSel = m_pGMemberList->GetCurSelectedRowIdx();
		if(-1 < nCurSel)
		{
			m_pEntryList->SetCurSelectedRowIdx(-1);
			m_wSelectedList = GT_ENTRYEDIT_GMEMBERLIST;
		}

		nCurSel = m_pEntryList->GetCurSelectedRowIdx();
		if(-1 < nCurSel)
		{
			m_pGMemberList->SetCurSelectedRowIdx(-1);
			m_wSelectedList = GT_ENTRYEDIT_ENTRYLIST;
		}
	}
	return we;
}

void CGTEntryEditDlg::OnActionEvent( LONG lId, void * p, DWORD we )
{
	if(!m_bActive)		return;

	if( we & WE_BTNCLICK )
	{
		switch(lId)
		{
		case GTENTRYEDIT_SYN:
			{
				if(m_pEntryList->GetItemCount() < 1)
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1644) );
					return;
				}

				if(14 < m_pEntryList->GetItemCount())
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1645) );
					return;
				}

				// 변경요청
				MSG_GTEDIT_PLAYERS msg;
				msg.Category = MP_GTOURNAMENT;
				msg.Protocol = MP_GTOURNAMENT_EDITPLAER_SYN;
				msg.dwObjectID = HEROID;
				msg.dwRemainTime = 0;

				std::list< DWORD >::iterator iter;
				int i;
				for(iter=m_lstEntryList.begin(), i=0; iter!=m_lstEntryList.end(); iter++, i++)
					msg.dwPlayers[i] = (*iter);

				NETWORK->Send( &msg, sizeof(msg) );

				SetActive(FALSE);
			}
			break;
		case GTENTRYCHANGE:
			{
				if(m_wSelectedList == GT_ENTRYEDIT_GMEMBERLIST)
				{
					// 길드원목록 -> 선수등록
					int CurSel = m_pGMemberList->GetClickedRowIdx();
					if(-1 < CurSel)
					{
						std::list< GUILDMEMBERINFO >::iterator iter;
						iter = m_lstMemberList.begin();
						std::advance(iter, CurSel);

						GUILDMEMBERINFO* pInfo = &(*iter);

						if(!IsInEntry(pInfo->MemberIdx))
							AddToEntry(pInfo->MemberIdx);
					}
				}
				else if(m_wSelectedList == GT_ENTRYEDIT_ENTRYLIST)
				{
					// 길드원목록 <- 선수목록
					int CurSel = m_pEntryList->GetClickedRowIdx();
					if(-1 < CurSel)
					{
						std::list< DWORD >::iterator iter;
						iter = m_lstEntryList.begin();
						std::advance(iter, CurSel);

						DWORD dwPlayerID = (*iter);
						m_pEntryList->RemoveItem(CurSel);
						m_lstEntryList.remove(dwPlayerID);
					}
				}
			}
			break;
		// 090822 ONS 참가자명단 다이얼로그를 닫을때, 선수목록이 한명도 없으면 닫을 수 없도록 처리.
		case GTENTRYEDIT_CLOSE:
			{
				if(m_pEntryList->GetItemCount() < 1)
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1644) );
					return;
				}
				else
				{
					SetActive(FALSE);
				}
			}
			break;
		}
	}
}

void CGTEntryEditDlg::Render()
{
	if(!m_bActive)		return;

	CalcRemainTime();
	cDialog::Render();
}

void CGTEntryEditDlg::CalcRemainTime()
{
	DWORD dwRemainTime = m_dwRemainTime;
	WORD wDay = WORD(dwRemainTime / (60000 * 60 * 24));
	if(wDay)
		dwRemainTime = dwRemainTime - (60000 * 60 * 24 * wDay);

	WORD wHour = WORD(dwRemainTime / (60000 * 60));
	if(wHour)
		dwRemainTime = dwRemainTime - (60000 * 60 * wHour);

	WORD wMin = WORD(dwRemainTime / 60000);
	if(wMin)
		dwRemainTime = dwRemainTime - (60000 * wMin);

	WORD wSec = WORD(dwRemainTime / 1000);

	if(m_wLastSec != wSec)
	{
		char buf[128] = {0,};
		if(wDay)
			sprintf(buf, "%dDay %02d:%02d:%02d\n", wDay, wHour, wMin, wSec);
		else
			sprintf(buf, "%02d:%02d:%02d\n", wHour, wMin, wSec);

		m_pRemainTime->SetStaticText(buf);

		if(0==wDay && 0==wHour && wMin<5 && !m_bChangedColor)
		{
			m_pRemainTime->SetFGColor(0xffff0000);
			m_bChangedColor = TRUE;
		}

		m_wLastSec = wSec;
	}
}

void CGTEntryEditDlg::SetList(DWORD* pEntryList)
{
	m_pGMemberList->RemoveAll();
	m_pEntryList->RemoveAll();
	m_wSelectedList = GT_ENTRYEDIT_MAX;	

	if(!pEntryList)		return;
	m_lstMemberList.clear();
	m_lstEntryList.clear();

	std::list< GUILDMEMBERINFO >* pGuildMemberList = NULL;
	std::list< GUILDMEMBERINFO >::iterator iter;

	if(GAMEIN->GetGuildDlg())
		pGuildMemberList = GAMEIN->GetGuildDlg()->GetGuildMemberList();

	char buf[128] = {0,};
	if(pGuildMemberList)
	{
		for(iter=pGuildMemberList->begin(); iter!=pGuildMemberList->end(); iter++)
		{
			GUILDMEMBERINFO member = (*iter);
			m_lstMemberList.push_back(member);

			sprintf(buf, "%4d %18s", iter->Memberlvl, iter->MemberName);
			m_pGMemberList->AddItem(buf, RGB( 255, 255, 255 ));
		}
	}

	int i;
	for(i=0; i<MAX_GTOURNAMENT_PLAYER; i++)
	{
		AddToEntry(pEntryList[i]);
	}

	SetActive(TRUE);
}

GUILDMEMBERINFO* CGTEntryEditDlg::GetMember( DWORD playerIndex )
{
	std::list< GUILDMEMBERINFO >::iterator iter;
	for(iter = m_lstMemberList.begin();	iter!=m_lstMemberList.end(); iter++)
	{
		if( playerIndex == (*iter).MemberIdx)
			return &(*iter);
	}

	return NULL;
}

BOOL CGTEntryEditDlg::IsInEntry(DWORD dwPlayerID)
{
	std::list< DWORD >::iterator iter;
	for(iter=m_lstEntryList.begin(); iter!=m_lstEntryList.end(); iter++)
	{
		if((*iter) == dwPlayerID)
			return TRUE;
	}

	return FALSE;
}
BOOL CGTEntryEditDlg::AddToEntry(DWORD dwPlayerID)
{
	char buf[128] = {0,};

	GUILDMEMBERINFO* pInfo = NULL;
	pInfo = GetMember(dwPlayerID);

	if(pInfo)
	{
		sprintf(buf, "%4d %18s", pInfo->Memberlvl, pInfo->MemberName);
		m_pEntryList->AddItem(buf, RGB( 255, 255, 255 ));
		m_lstEntryList.push_back(dwPlayerID);
		return TRUE;
	}

	return FALSE;
}