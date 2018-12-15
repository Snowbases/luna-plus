#include "stdafx.h"
#include "GuildNickNameDialog.h"
#include "interface/cTextArea.h"
#include "interface/cEditBox.h"
#include "interface/cStatic.h"
#include "cImeEx.h"
#include "WindowIDEnum.h"
#include "ChatManager.h"
#include "GuildManager.h"
#include "cResourceManager.h"
#include "FilteringTable.h"


CGuildNickNameDialog::CGuildNickNameDialog()
{
	m_type = WT_GUILDNICKNAMEDLG;

	ZeroMemory( &mSelectedMember, sizeof( mSelectedMember ) );
}


CGuildNickNameDialog::~CGuildNickNameDialog()
{

}

void CGuildNickNameDialog::Linking()
{
	m_pNickMsg = (cTextArea*)GetWindowForID(GD_NICKTEXTAREA);
	m_pNickName = (cEditBox*)GetWindowForID(GD_NICKNAMEEDIT);
	mTitle		= ( cStatic* )GetWindowForID( GD_NICKTITLE );

	m_pNickName->SetValidCheck( VCM_SPACE );
}

void CGuildNickNameDialog::SetActive(BOOL val)
{
	if( val )
	{
		const GUILDMEMBERINFO* member = GUILDMGR->GetSelectedMember();

		if( ! member )
		{			
			cDialog::SetActive(FALSE);
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 545 ) );
			return;
		}

		mSelectedMember = *member;

		m_pNickName->SetEditText("");

		char line[ MAX_PATH ];

		sprintf(
			line,
			"%s",
			//RESRCMGR->GetMsg( 56 ),
            mSelectedMember.MemberName ) ;

		mTitle->SetStaticText( line );

		//SetNickMsg(GUILDMGR->GetSelectedMemberName());
		m_pNickName->SetFocusEdit( TRUE );
	}
	else
	{
		m_pNickName->SetFocusEdit( FALSE );
	}

	cDialog::SetActive( val );
}

//void CGuildNickNameDialog::SetNickMsg( const char* Name )
//{
	//char text[128];
	//sprintf(text, "%s님에게 호칭을 부여하시겠습니까?", Name);
	//m_pNickMsg->SetScriptText(text);
//}


void CGuildNickNameDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	switch( id )
	{
	case GD_NREGISTOKBTN:
		{
			const char* name = m_pNickName->GetEditText();

			if( ! *name )
			{
				// 이름을 입력하세
//				return;
			}
			else if( strlen( name ) > MAX_GUILD_NICKNAME )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(550));
				return;
			}
			else if( !	FILTERTABLE->IsUsableName(name) ||
						FILTERTABLE->IsCharInString( name, " '") )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(551));
				return;
			}

			{
				SEND_GUILD_NICKNAME message;
				message.Category	= MP_GUILD;
				message.Protocol	= MP_GUILD_GIVENICKNAME_SYN;
				message.dwObjectID	= HEROID;
				message.TargetIdx	= mSelectedMember.MemberIdx;

				SafeStrCpy( message.NickName, name, sizeof( message.NickName ) );
				NETWORK->Send( &message, sizeof( message ) );
			}
			
			//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(609), mSelectedMember.MemberName );

			// 길드 닉네임 창을 끄자...
			SetActive( FALSE );

			break;
		}
	}
}