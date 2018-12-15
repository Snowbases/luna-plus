#include "stdafx.h"
#include "GuildMarkDialog.h"
//#include "NpcScriptDialog.h"
#include "WindowIDEnum.h"
#include "ObjectStateManager.h"
#include "ObjectManager.h"
//#include "GameIn.h"
#include "ChatManager.h"
//#include "interface/cTextArea.h"
#include "GuildUnionMarkMgr.h"
#include "GuildMarkManager.h"
#include "GuildUnion.h"
#include "GuildManager.h"
#include "interface/cEditBox.h"
#include "interface/cStatic.h"
#include "interface/cWindowManager.h"

#include "../Interface/cResourceManager.h"


extern int g_nServerSetNum;


CGuildMarkDialog::CGuildMarkDialog()
{
	m_type = WT_GUILDMARKDLG;
}


CGuildMarkDialog::~CGuildMarkDialog()
{}


void CGuildMarkDialog::Linking()
{
	//cTextArea* textArea = (cTextArea*)GetWindowForID(GDM_INFOTEXT);
	//textArea->SetScriptText( CHATMGR->GetChatMsg(303) );
	
	mGuildMarkSubmit	=				GetWindowForID( GDM_REGISTOKBTN );
	mUnionMarkSubmit	=				GetWindowForID( GUM_REGISTOKBTN );
	pMarkName			= ( cEditBox* )	GetWindowForID( GDM_NAMEEDIT );
	mTitle				= ( cStatic* )	GetWindowForID( GDM_TITLE );
}

void CGuildMarkDialog::SetActive(BOOL val)
{
	CHero* hero = OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}

	if( val )
	{
		if( strcmp( hero->GetObjectName(), GUILDMGR->GetMasterName() ) )
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(330));
			return;
		}

		const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting( GUILDMGR->GetLevel() );
		ASSERT( setting );

        if( ! setting	||
			! setting->mGuildMark )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 326 ) );
			return;
		}

		pMarkName->SetEditText(
			"");
		pMarkName->SetFocusEdit(
			TRUE);
	}
	else
	{		
		pMarkName->SetFocusEdit(FALSE);

		// NPC Script Dialog
		cDialog* dialog = WINDOWMGR->GetWindowForID( NSI_SCRIPTDLG );
		ASSERT( dialog );

		if( ( hero->GetState() == eObjectState_Deal ) && ! dialog->IsActive() )
		{
			OBJECTSTATEMGR->EndObjectState( hero, eObjectState_Deal);
		}
	}

	cDialog::SetActive(val);
}


void CGuildMarkDialog::ShowGuildMark() 
{
	SetActive( TRUE );

	mGuildMarkSubmit->SetActive( TRUE );
	mUnionMarkSubmit->SetActive( FALSE );
	
	mTitle->SetStaticText( RESRCMGR->GetMsg(53) );
}


void CGuildMarkDialog::ShowGuildUnionMark()
{
	if( ! GUILDUNION->CheckGuildUnionCondition( 5 ) )
	{
		return;
	}

	SetActive( TRUE );

	mGuildMarkSubmit->SetActive( FALSE );
	mUnionMarkSubmit->SetActive( TRUE );
	
	mTitle->SetStaticText( CHATMGR->GetChatMsg(1469) );
}


void CGuildMarkDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	switch( id )
	{
	// 길드 마크 등록
	case GDM_REGISTOKBTN:
		{
			const char* name = pMarkName->GetEditText();

			if( !*name )
			{
				// 091105 ONS 길드마크 공백 입력시 기존 마크를 삭제한다.
				MSG_DWORD message;
				ZeroMemory(&message, sizeof(MSG_DWORD));
				message.Category	= MP_GUILD;
				message.Protocol	= MP_GUILDMARK_DEL_SYN;
				message.dwObjectID	= HEROID;
				message.dwData		= HERO->GetGuildIdx();
				NETWORK->Send( &message, sizeof( message ) );
			}

			if( *name && (!GUILDMARKMGR->ChangeGuildMarkSyn( HERO->GetGuildIdx(), name )) )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(549));
			}

			SetActive(
				FALSE);
			break;
		}
	// 길드 연합 마크 등록
	case GUM_REGISTOKBTN:
		{
			const char* name = pMarkName->GetEditText();

			if( GUILDUNION->CheckGuildUnionCondition( 5 ) && 
				! GUILDUNIONMARKMGR->RegistGuildUnionMark( 
				g_nServerSetNum,
				HERO->GetGuildUnionIdx(),
				name ))
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(549));
			}

			SetActive(
				FALSE);
			break;
		}
	}
}