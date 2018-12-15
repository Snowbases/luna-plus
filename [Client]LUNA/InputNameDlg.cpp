#include "stdafx.h"
#include "InputNameDlg.h"
#include "WindowIDEnum.h"
#include "cEditBox.h"
#include "../input/Mouse.h"
#include "cMsgBox.h"
#include "ChatManager.h"
#include "PartyManager.h"
#include "ObjectManager.h"

CInputNameDlg::CInputNameDlg()
{
}

CInputNameDlg::~CInputNameDlg()
{
}

void CInputNameDlg::Linking()
{
	m_CharName	= (cEditBox*)GetWindowForID( ID_INPUTNAMEDLG_EDIT_NAME );
}

void CInputNameDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	if( we & WE_BTNCLICK )
	{
 		switch( lId )
		{
		case ID_INPUTNAMEDLG_BUTTON_OK:
			{
				// 작업제한시간 확인
				const DWORD dwLastTime = HERO->GetInvitePartyByNameLastTime();
				const DWORD dwLimitTime = HERO->GetInvitePartyByNameLimitTime();

				// 작업 가능시간이 경과하지 않은경우
				if( dwLastTime != 0 &&
					gCurTime < dwLastTime + dwLimitTime )
				{
					float fRemainTime = float( dwLimitTime - (gCurTime - dwLastTime) ) / 1000.0f;
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1923 ), fRemainTime );
					return;
				}

				// 작업 제한시간 해제
				HERO->SetInvitePartyByNameLastTime( 0 );
				HERO->SetInvitePartyByNameLimitTime( 0 );

				char* const szCharName = m_CharName->GetEditText();

				if( strlen( szCharName ) <= 0 )
					break;

				// 자기자신의 이름과 같은 경우(대소문자 구분없이 비교)
				if( stricmp( HERO->GetObjectName(), szCharName ) == 0 )
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1269 ) );
					SetActive( FALSE );
					break;
				}

				// 파티 초대 메세지를 보낸다
				PARTYMGR->InvitePartyMemberByNameSyn( szCharName );

				SetActive( FALSE );
			}
			break;
		}
	}
}

void CInputNameDlg::SetActive(BOOL val)
{
	cDialog::SetActive( val );

	// 창이 닫히는 경우 EditBox(이름) 초기화
	if( !val )
	{
		m_CharName->SetEditText( "" );
	}
}