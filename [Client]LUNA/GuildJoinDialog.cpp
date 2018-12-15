#include "stdafx.h"
#include "GuildJoinDialog.h"
//#include "GuildDialog.h"
#include "GuildManager.h"
#include "ObjectManager.h"
#include "ChatManager.h"
#include "WindowIdEnum.h"
#include "cWindowManager.h"
#include "MHMap.h"


const DWORD enableColor = RGB( 255, 255, 255 );
const DWORD unableColor = RGB( 190, 190, 190 );


CGuildJoinDialog::CGuildJoinDialog(void)
{
}

CGuildJoinDialog::~CGuildJoinDialog(void)
{}


void CGuildJoinDialog::Linking()
{
}

	
void CGuildJoinDialog::OnActionEvent(LONG lId, void* p, DWORD we)
{
	const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting( GUILDMGR->GetLevel() );

	if( ! setting )
	{
		ASSERT( 0 );
		return;
	}

	switch(lId)
	{
	case JO_MEMBERBTN:
		{
			CHero* hero = OBJECTMGR->GetHero();
			ASSERT( hero );

			if( hero->GetGuildMemberRank() < GUILD_JUNIOR )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(596) );
				break;
			}
			else if( setting->mMemberSize <= GUILDMGR->GetMemberSize() - GUILDMGR->GetStudentSize() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 597 ) );
				break;
			}
			
			CPlayer* object = ( CPlayer* )OBJECTMGR->GetSelectedObject();

			if(	!	object ||
					object->GetObjectKind() != eObjectKind_Player )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 606 ) );
				break;
			}
			else if( object->GetGuildIdx() )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(599));
				break;
			}
			
			{
				MSG_DWORD msg;
				msg.Category	= MP_GUILD;
				msg.Protocol	= MP_GUILD_ADDMEMBER_SYN;
				msg.dwData		= object->GetID();
				msg.dwObjectID	= hero->GetID();
				NETWORK->Send(&msg, sizeof(msg));
			}

			SetActive(FALSE);
			break;
		}		
	case JO_STUDENTBTN:
		{
			CPlayer* player = ( CPlayer* )OBJECTMGR->GetSelectedObject();

			if(	!	player ||
					player->GetObjectKind() != eObjectKind_Player )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 606 ) );
				break;
			}
			else if( player->GetGuildIdx() )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(599));
				break;
			}
			else if( GUILD_STUDENT_MAX_LEVEL <= player->GetLevel() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 603 ) );
				break;
			}

			const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting( GUILDMGR->GetLevel() );
			ASSERT( setting );

			/*
			if( ! setting->mApprenticeSize )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(626) );
				break;
			}
			*/
			
			{
				MSG_DWORD msg;
				msg.Category	= MP_GUILD;
				msg.Protocol	= MP_GUILD_ADDSTUDENT_SYN;
				msg.dwData		= player->GetID();
				msg.dwObjectID	= HEROID;
				NETWORK->Send(&msg, sizeof(msg));
			}

			SetActive( FALSE );
			break;
		}
		// 080417 LUJ, Ã¢ ´Ý±â
	case JO_CANCELBTN:
		{
			SetActive( FALSE );
			break;
		}
	default:
		{
			ASSERT(0);
			break;
		}
	}
}


void CGuildJoinDialog::SetEnableStudent( BOOL isEnable )
{
	cWindow* window = GetWindowForID( JO_STUDENTBTN );
	ASSERT( window );

	if( isEnable )
	{
		window->SetImageRGB( enableColor );
		window->SetDisable( FALSE );
	}
	else
	{
		window->SetImageRGB( unableColor );
		window->SetDisable( TRUE );
	}
}


void CGuildJoinDialog::SetEnableMember( BOOL isEnable )
{
	cWindow* window = GetWindowForID( JO_MEMBERBTN );
	ASSERT( window );

	if( isEnable )
	{
		window->SetImageRGB( enableColor );
		window->SetDisable( FALSE );
	}
	else
	{
		window->SetImageRGB( unableColor );
		window->SetDisable( TRUE );
	}
}