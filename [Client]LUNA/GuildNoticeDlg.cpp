#include "stdafx.h"
#include "GuildNoticeDlg.h"
#include "Interface/cTextArea.h"
#include "WindowIDEnum.h"
#include "ObjectManager.h"


CGuildNoticeDlg::CGuildNoticeDlg(void)
{}


CGuildNoticeDlg::~CGuildNoticeDlg(void)
{}


void CGuildNoticeDlg::Linking()
{
	mNotice = (cTextArea*)GetWindowForID(GNotice_TEXTREA);
	
	mNotice->SetEnterAllow(FALSE);
	//mNotice->SetScriptText("");
}

void CGuildNoticeDlg::OnActionEvent(LONG lId, void * p, DWORD we)
{
	if( we & WE_BTNCLICK )
	{
		switch(lId)
		{
		case GNotice_SENDOKBTN:
			{
				CHero* hero = OBJECTMGR->GetHero();
				ASSERT( hero );

				MSG_GUILDNOTICE message;
				
				message.Category	= MP_GUILD;
				message.Protocol	= MP_GUILD_NOTICE_SYN;
				message.dwObjectID	= hero->GetID();
				message.dwGuildId	= hero->GetGuildIdx();
				
				mNotice->GetScriptText( message.Msg );

				// 이전에 저장된 메시지와 같은 경우 업데이트할 필요가 없다.
				if( mPreviousNotice != message.Msg )
				{
					NETWORK->Send( &message, message.GetMsgLength() );
				}
				
				SetActive(FALSE);
				break;
			}
		case GNotice_CANCELBTN:
			{
				SetActive(FALSE);
				break;
			}
		}
	}
}

void CGuildNoticeDlg::SetActive( BOOL isActive )
{
	cDialog::SetActive( isActive );

	{
		char text[ MAX_PATH ];
		mNotice->GetScriptText( text );

		mPreviousNotice = text;
	}	
}


void CGuildNoticeDlg::SetNotice( const char* text )
{
	mNotice->SetScriptText( text );
}