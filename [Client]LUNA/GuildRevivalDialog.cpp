#include "stdafx.h"
#include "GuildRevivalDialog.h"
#include "WindowIDEnum.h"
#include "cWindowManager.h"
#include "cMsgBox.h"
#include "ChatManager.h"


CGuildRevivalDialog::CGuildRevivalDialog()
{}


CGuildRevivalDialog::~CGuildRevivalDialog()
{}


void CGuildRevivalDialog::Linking()
{}


void CGuildRevivalDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	switch( id )
	{
	// 마을에서 부활한다
	case GD_REVIVAL_AT_TOWN:
		{
			WINDOWMGR->MsgBox( MBI_GUILDFIELDWAR_REVIVE, MBT_YESNO, CHATMGR->GetChatMsg( 1305 ) );
			break;
		}
	}
}