#include "stdafx.h"
#include "GuildWarehouseRankDialog.h"
#include "WindowIDEnum.h"
#include "ChatManager.h"
#include "GuildManager.h"
#include "cComboBox.h"
#include "ObjectManager.h"


CGuildWarehouseRankDialog::CGuildWarehouseRankDialog()
{
	m_type = WT_GUILDWAREHOUSERANKDLG;
}


CGuildWarehouseRankDialog::~CGuildWarehouseRankDialog()
{}


void CGuildWarehouseRankDialog::Linking()
{
	m_pRankComboBox		= ( cComboBox* )GetWindowForID( GDWHR_RANKCOMBOBOX );
}


void CGuildWarehouseRankDialog::SetActive( BOOL isActive )
{
	if( isActive )
	{
		CHero* hero = OBJECTMGR->GetHero();

		if( ! hero )
		{
			return;
		}
		
        const BYTE rank = hero->GetGuildMemberRank();

		if( rank != GUILD_MASTER &&
			rank != GUILD_VICEMASTER )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 579 ) );
			return;
		}
	}

	cDialog::SetActive( isActive );
}


void CGuildWarehouseRankDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	switch( id )
	{
	case GDWHR_OKBTN:
		{
			BYTE rank;

			switch( m_pRankComboBox->GetCurSelectedIdx() )
			{
			case -1:
			case 0:
				{
					rank = GUILD_JUNIOR;
					break;
				}
			case 1:
				{
					rank = GUILD_SENIOR;
					break;
				}
			case 2:
				{
					rank = GUILD_VICEMASTER;
					break;
				}
			default:
				{
					ASSERT( 0 && "it's undefined rank" );
					return;
				}
			}

			{
				CHero* hero = OBJECTMGR->GetHero();
				ASSERT( hero );

				MSG_GUILD_SET_WAREHOUSE_RANK message;
				message.Category	= MP_GUILD;
				message.Protocol	= MP_GUILD_SET_WAREHOUSE_RANK_SYN;
				message.dwObjectID	= hero->GetID();
				message.mGuildIndex	= hero->GetGuildIdx();
				message.mRank		= rank;

				NETWORK->Send( &message, sizeof( message ) );
			}			

			SetActive( FALSE );
			break;
		}
	case GDWHR_CANCELBTN:
		{
			SetActive( FALSE );
			break;
		}
	}
}