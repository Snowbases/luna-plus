#include "stdafx.h"
#include "./autonotedlg.h"
#include "WindowIDEnum.h"

#include "./Interface/cTextArea.h"
#include "./Interface/cButton.h"
#include "./Interface/cListDialog.h"
#include "cMsgBox.h"

#include "cWindowManager.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "AutoNoteManager.h"

CAutoNoteDlg::CAutoNoteDlg(void)
{
	m_pTextAreaManual	= NULL;
	m_pBtnAsk			= NULL;
	m_pListAuto			= NULL;

}

CAutoNoteDlg::~CAutoNoteDlg(void)
{
	if( m_pListAuto )
		m_pListAuto->RemoveAll();
}

void CAutoNoteDlg::Linking()
{
	m_pTextAreaManual	= (cTextArea*)GetWindowForID( AND_TEXTAREA_MANUAL );
	m_pBtnAsk			= (cButton*)GetWindowForID( AND_BTN_ASK );
	m_pListAuto			= (cListDialog*)GetWindowForID( AND_LIST_AUTO );

	//m_pTextAreaManual->SetScriptText( "1. 이 노트에 이름을 쓰면 죽는다.^n2. 사냥 중인 캐릭터에게만 사용할 수 있다.^n3.기타등등" );
	m_pTextAreaManual->SetScriptText( CHATMGR->GetChatMsg( 1416 ) );
}

void CAutoNoteDlg::OnActionEvent(LONG lId, void * p, DWORD we)
{
	if( we & WE_BTNCLICK )
	{
		if( lId == AND_BTN_ASK )
		{
			CObject* pObject = OBJECTMGR->GetSelectedObject();
			if( pObject == NULL )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1415 ) );//"[오토노트]먼저 대상을 선택해주세요." );
				return;
			}

			if( pObject->GetObjectKind() != eObjectKind_Player )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1415 ) );//"[오토노트]먼저 대상을 선택해주세요." );
				return;
			}

#ifndef _GMTOOL_
			if( pObject == HERO )	//자기 자신
			{
				return;
			}
#endif

//			char buf[128];
//			wsprintf( buf, "[%s]을(를) 오토노트에 쓰기 위해 시험을 하시겠습니까?", pObject->GetObjectName() );
//			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_AUTONOTEASK, MBT_YESNO, buf );

			AUTONOTEMGR->AskToAutoUser( pObject->GetID(), rand()%100 );
		}
	}
}

void CAutoNoteDlg::AddAutoList( char* strName, char* strDate )
{
	char buf[128];
	wsprintf( buf, "%-16s %s", strName, strDate );
	m_pListAuto->AddItem( buf, RGB_HALF(10,10,10) );
}