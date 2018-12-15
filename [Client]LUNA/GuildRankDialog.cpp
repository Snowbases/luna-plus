#include "stdafx.h"
#include "GuildRankDialog.h"
#include "WindowIDEnum.h"
#include "interface/cTextArea.h"
#include "interface/cStatic.h"
#include "ChatManager.h"
#include "GuildManager.h"
#include "cComboBox.h"
#include "ObjectManager.h"
#include "cResourceManager.h"


CGuildRankDialog::CGuildRankDialog()
{
	m_type = WT_GUILDRANKDLG;

	ZeroMemory( &mSelectedMember, sizeof( mSelectedMember ) );
}


CGuildRankDialog::~CGuildRankDialog()
{}


void CGuildRankDialog::Linking()
{
	m_pRankMemberName	= ( cTextArea*	)GetWindowForID( GDR_MEMBERNAME );
	m_pRankComboBox		= ( cComboBox*	)GetWindowForID( GDR_RANKCOMBOBOX );
	mTitle				= ( cStatic*	)GetWindowForID( GDR_TITLE );
	//m_pDOkBtn = (cButton*)GetWindowForID(GDR_DOKBTN);
	//m_pOkBtn = 	(cButton*)GetWindowForID(GDR_OKBTN);

	//m_GuildRankCtrlList[eRankMode_Guild].AddTail(m_pRankComboBox);
	//m_GuildRankCtrlList[eRankMode_Guild].AddTail(m_pOkBtn);
}


void CGuildRankDialog::SetActive( BOOL isActive )
{
	if( isActive )
	{
		CHero* hero = OBJECTMGR->GetHero();

		if( ! hero )
		{
			return;
		}

		const GUILDMEMBERINFO* member = GUILDMGR->GetSelectedMember();

		if( ! member )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 545 ) );
			return;
		}
		else if( hero->GetID() == member->MemberIdx )
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1094 ) );
			return;
		}

		mSelectedMember = *member;
		
		const BYTE rank = hero->GetGuildMemberRank();

		if( rank != GUILD_MASTER &&
			rank != GUILD_VICEMASTER )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 579 ) );
			return;
		}
		else if( ! strcmp( member->MemberName, GUILDMGR->GetMasterName() ) )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1095 ) );
			 return;
		}
		else if( member->Rank >= hero->GetGuildMemberRank() )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1096 ) );
			return;
		}
		else if(	member->Rank == GUILD_STUDENT	&&
					member->Memberlvl < GUILD_STUDENT_MAX_LEVEL )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1273 ), GUILD_STUDENT_MAX_LEVEL );
			return;
		}

		m_pRankMemberName->SetScriptText( member->MemberName );

		{
			char buffer[ MAX_PATH ];

			sprintf(
				buffer,
				"%s",
				//RESRCMGR->GetMsg( 670 ),
				member->MemberName );

			mTitle->SetStaticText( buffer );
		}
	}
	
	cDialog::SetActive( isActive );
}


void CGuildRankDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	switch( id )
	{
	case GDR_OKBTN:
		{
			BYTE rank;

			switch( m_pRankComboBox->GetCurSelectedIdx() )
			{
			case -1:
			case 0:
				{
					rank = GUILD_MEMBER;
					break;
				}
			case 1:
				{
					rank = GUILD_JUNIOR;
					break;
				}
			case 2:
				{
					rank = GUILD_SENIOR;
					break;
				}
			case 3:
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

			if( mSelectedMember.Rank == rank )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(607));
				return;
			}
			else
			{
				CHero* hero = OBJECTMGR->GetHero();
				ASSERT( hero );

				// 자신과 같거나 높은 회원의 직위를 변경할 수 없음
				if( mSelectedMember.Rank >= hero->GetGuildMemberRank() )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 323 ) );
					return;
				}
				// 자신보다 높거나 같은 직위를 부여할 수 없음

				else if( rank >= hero->GetGuildMemberRank() )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1096 ) );
					return;
				}
			}

			{
				MSG_DWORDBYTE message;
				message.Category	= MP_GUILD;
				message.Protocol	= MP_GUILD_CHANGERANK_SYN;
				message.dwObjectID	= HEROID;
				message.dwData		= mSelectedMember.MemberIdx;
				message.bData		= rank;
				NETWORK->Send( &message, sizeof( message ) );
			}			

			SetActive( FALSE );
			break;
		}
	case GDR_CANCELBTN:
		{
			SetActive( FALSE );
			break;
		}
	}
}