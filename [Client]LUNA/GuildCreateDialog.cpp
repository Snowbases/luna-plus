#include "stdafx.h"
#include "GuildCreateDialog.h"
#include "./Interface/cWindowManager.h"
#include "WindowIDEnum.h"
//#include "./interface/cStatic.h"
#include "./interface/cEditBox.h"
#include "./interface/cTextArea.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "GameIn.h"
#include "NpcScriptDialog.h"
#include "ChatManager.h"
#include "GuildManager.h"
#include "FilteringTable.h"
#include "GuildUnion.h"


CGuildCreateDialog::CGuildCreateDialog()
{
	m_type = WT_GUILDCREATEDLG;
}

CGuildCreateDialog::~CGuildCreateDialog()
{}


void CGuildCreateDialog::Linking()
{
	//m_pLocation = ( cStatic* )GetWindowForID(GD_CLOCATION);

	m_pGuildName	= ( cEditBox* )	GetWindowForID(GD_CNAME);

	ASSERT( m_pGuildName );
}

void CGuildCreateDialog::SetActive( BOOL isActive )
{
	if( isActive )
	{
		//m_pLocation->SetStaticText(GetMapName(HERO->GetCharacterTotalInfo()->CurMapNum));
		m_pGuildName->SetEditText("");
	}
	else
	{
		CHero* hero = OBJECTMGR->GetHero();
		
		if(		hero									&&
				hero->GetState() == eObjectState_Deal	&& 
			!	GAMEIN->GetNpcScriptDialog()->IsActive() )
		{
			OBJECTSTATEMGR->EndObjectState(hero, eObjectState_Deal);
		}
	}

	cDialog::SetActive( isActive );
}


void CGuildCreateDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	switch( id )
	{
	// 길드 창설
	case GD_CCREATEOKBTN:
		{
			CHero* hero = OBJECTMGR->GetHero();

			if( hero->GetGuildIdx() )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(543));
				break;
			}

			// 조건 체크
			{
				const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting( 1 );

				if( ! setting )
				{
					ASSERT( 0 );
					break;
				}
				else if( hero->GetMoney() < setting->mRequiredMoney )
				{
					//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 324 ) );

					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1272 ), AddComma( setting->mRequiredMoney ) );
					break;
				}
				else if( hero->GetLevel() < setting->mRequiredMasterLevel )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1087 ), setting->mRequiredMasterLevel );
					break;
				}
			}

			const char* name = "";

			// 길드 이름 체크
			{
				name = m_pGuildName->GetEditText();

				if( ! *name )
				{
					break;
				}
				else if(		FILTERTABLE->IsInvalidCharInclude( ( UCHAR* )name ) || 
							!	FILTERTABLE->IsUsableName( name ) )
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(590));
					return;
				}
			}

			char intro[ MAX_TEXT_SIZE ];

			// 길드 소개 체크
			{
				ZeroMemory(intro, sizeof(intro));

				if( MAX_GUILD_INTRO < strlen( intro ) )
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(544));
					break;
				}
			}

			// 메시지 전송
			{
				SEND_GUILD_CREATE_SYN message;
				message.Category	= MP_GUILD;
				message.Protocol	= MP_GUILD_CREATE_SYN;
				message.dwObjectID	= HEROID;

				SafeStrCpy( message.GuildName, name, sizeof( message.GuildName ) );
				SafeStrCpy( message.Intro, intro, sizeof( message.Intro ) );

				NETWORK->Send( &message, sizeof( message ) );
			}		

			SetDisable( TRUE );
			break;
		}
	}
}


CGuildUnionCreateDialog::CGuildUnionCreateDialog()
{
	m_type = WT_GUILDUNIONCREATEDLG;
}

CGuildUnionCreateDialog::~CGuildUnionCreateDialog()
{
}

void CGuildUnionCreateDialog::SetActive( BOOL val )
{
	CHero* hero = OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}

	if( val == TRUE )
	{
		if( strcmp( hero->GetObjectName(), GUILDMGR->GetMasterName() ) )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 323 ) );
			return;
		}
		else if( hero->GetGuildUnionIdx() )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1008 ) );
			return;
		}

		m_pNameEdit->SetEditText( "" );
	}	
	else
	{
		if(		hero->GetState() == eObjectState_Deal	&&
			!	GAMEIN->GetNpcScriptDialog()->IsActive() )
		{
			OBJECTSTATEMGR->EndObjectState( hero, eObjectState_Deal );
		}

		m_pNameEdit->SetFocusEdit( FALSE );
	}

	cDialog::SetActive(val);
}


void CGuildUnionCreateDialog::Linking()
{
	m_pNameEdit = (cEditBox*)GetWindowForID(GDU_NAME);
	m_pOkBtn = (cButton*)GetWindowForID(GDU_OKBTN);
	m_pText = (cTextArea*)GetWindowForID(GDU_TEXT);
}


void CGuildUnionCreateDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	switch( id )
	{
	// 동맹 창설
	case GDU_OKBTN:
		{
			cEditBox* unionName = (cEditBox*)WINDOWMGR->GetWindowForIDEx( GDU_NAME );

			const char* text = unionName->GetEditText();

			if( ! *text )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 11 ) );
			}
			else if( MAX_GUILD_NAME < strlen( text ) )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 550 ), MAX_TEXT_SIZE / 2 );
			}
			else
			{
				if( GUILDUNION->CheckGuildUnionCondition( 0 ) )
				{
					CHero* hero = OBJECTMGR->GetHero();
					ASSERT( hero );

					MSG_NAME_DWORD message;
					message.Category	= MP_GUILD_UNION;
					message.Protocol	= MP_GUILD_UNION_CREATE_SYN;
					message.dwObjectID	= hero->GetID();
					message.dwData		= hero->GetGuildIdx();

					SafeStrCpy( message.Name, text, sizeof( message.Name ) );

					NETWORK->Send( &message, sizeof( message ) );

					SetActive( FALSE );
				}
			}

			break;
		}
	case GUD_CANCELBTN:
		{
			SetActive( FALSE );

			break;
		}
	}
}