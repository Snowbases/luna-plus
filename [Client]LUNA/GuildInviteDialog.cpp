#include "stdafx.h"
#include "GuildInviteDialog.h"
#include "interface/cTextArea.h"
#include "GuildManager.h"
#include "ChatManager.h"
#include "WindowIDEnum.h"
#include "cResourceManager.h"


CGuildInviteDialog::CGuildInviteDialog() :
mInvitedPlayerIndex( 0 ),
mInvitedKind( eKindNone )
{
	m_type = WT_GUILDINVITEDLG;
}


CGuildInviteDialog::~CGuildInviteDialog()
{}


void CGuildInviteDialog::Linking()
{
	m_pInviteMsg = (cTextArea*)GetWindowForID(GD_IINVITE);                                                                                                                                                                                                                                                             

	mInvitedPlayerIndex = 0;	
}

//void CGuildInviteDialog::SetInfo( const char* GuildName, char* MasterName, int FlgKind)
//{
//	char text[128];
//
//	if(FlgKind == AsMember)
//	{
//		sprintf(text, CHATMGR->GetChatMsg(45), GuildName, MasterName);
//	}
//	else	//AsStudent
//	{
//		sprintf(text, CHATMGR->GetChatMsg(1370), MasterName, GuildName);
//	}
//	m_pInviteMsg->SetScriptText(text);
//}


void CGuildInviteDialog::SetData( const SEND_GUILD_INVITE* message )
{
	ASSERT( message );

	char text[ MAX_TEXT_SIZE ];

	switch( message->Protocol )
	{
	case MP_GUILD_INVITE:
		{
			//sprintf( text, RESRCMGR->GetMsg( 559 ), message->GuildName, message->MasterName );
			sprintf( text, RESRCMGR->GetMsg( 559 ), message->GuildName, message->MasterName );

			mInvitedPlayerIndex = message->MasterIdx;
			mInvitedKind		= eKindMember;

			m_pInviteMsg->SetScriptText(text);
			
			break;
		}
	case MP_GUILD_INVITE_AS_STUDENT:
		{
			sprintf( text, CHATMGR->GetChatMsg(1468), message->GuildName );

			mInvitedPlayerIndex = message->MasterIdx;
			mInvitedKind		= eKindStudent;

			m_pInviteMsg->SetScriptText(text);
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CGuildInviteDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	switch( id )
	{
	// 가입 승인
	case GD_IOKBTN:
		{
			switch( mInvitedKind )
			{
			case eKindMember:
				{
					MSG_DWORD msg;
					msg.Category	= MP_GUILD;
					msg.Protocol	= MP_GUILD_INVITE_ACCEPT;
					msg.dwObjectID	= HEROID;
					msg.dwData		= mInvitedPlayerIndex;

					NETWORK->Send(&msg, sizeof(msg));
					SetActive( FALSE );

					break;
				}
			case eKindStudent:
				{
					MSG_DWORD msg;
					msg.Category	= MP_GUILD;
					msg.Protocol	= MP_GUILD_INVITE_AS_STUDENT_ACCEPT;
					msg.dwObjectID	= HEROID;
					msg.dwData		= mInvitedPlayerIndex;

					NETWORK->Send(&msg, sizeof(msg));
					SetActive(FALSE);
					break;
				}
			}

			break;
		}
	// 가입 거부
	case GD_ICANCELBTN:
		{
			MSG_DWORD msg;
			msg.Category	= MP_GUILD;
			msg.Protocol	= MP_GUILD_INVITE_DENY;
			msg.dwObjectID	= HEROID;
			msg.dwData		= mInvitedPlayerIndex;

			NETWORK->Send(&msg, sizeof(msg));
			
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(598));

			SetActive( FALSE );
			break;
		}
	}
}