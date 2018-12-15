#include "stdafx.h"
#include "GuildFieldWarDialog.h"
#include "./Interface/cWindowManager.h"
#include "./interface/cStatic.h"
#include "./interface/cButton.h"
#include "./interface/cEditBox.h"
#include "./interface/cTextArea.h"
#include "./interface/cListDialog.h"
#include "./interface/cCheckBox.h"
#include "../Interface/cResourceManager.h"
#include "../Input/Mouse.h"
#include "./Interface/cScriptManager.h"
#include "WindowIDEnum.h"
#include "GameResourceManager.h"
#include "ChatManager.h"
#include "GuildFieldWar.h"
#include "GuildUnion.h"
#include "ObjectManager.h"
#include "GuildManager.h"
#include "cIMEEX.h"


CGFWarDeclareDlg::CGFWarDeclareDlg()
{
	m_type = WT_GUILDFIELDWAR_DECLARE_DLG;
	m_bRender = FALSE;
}

CGFWarDeclareDlg::~CGFWarDeclareDlg()
{
}

void CGFWarDeclareDlg::SetActive( BOOL val )
{
	cDialog::SetActive(val);
	
	m_pEditBox->SetFocusEdit( val );
	m_pEditBox->SetEditText( "" );
	if( val )
	{
		m_pMoneyCheck->SetChecked( FALSE );
		m_pMoneyEdit->SetDisable( TRUE );	
		m_pMoneyEdit->SetEditText( "" );
	}
	m_bRender = val;
}

void CGFWarDeclareDlg::Linking()
{
	m_pMsg = (cTextArea*)GetWindowForID( GFW_DECLARE_MSG );
	m_pEditBox = (cEditBox*)GetWindowForID( GFW_DECLARE_EDITBOX );
	m_pMsg1 = (cTextArea*)GetWindowForID( GFW_MONEY_MSG1 );
	m_pMsg2 = (cTextArea*)GetWindowForID( GFW_MONEY_MSG2 );
	m_pMsg3 = (cTextArea*)GetWindowForID( GFW_MONEY_MSG3 );
	m_pMoneyCheck = (cCheckBox*)GetWindowForID( GFW_MONEY_CHK );
	m_pMoneyEdit = (cEditBox*)GetWindowForID( GFW_MONEY_EDITBOX );
	m_pMoneyStatic = (cStatic*)GetWindowForID( GFW_MONEY_STATIC );
	m_pOkBtn = (cButton*)GetWindowForID( GFW_DECLARE_OKBTN );
	m_pCancelBtn = (cButton*)GetWindowForID( GFW_DECLARE_CANCELBTN );

	SCRIPTMGR->GetImage( 77, &m_LockImage, PFT_HARDPATH );
	
	m_pMsg->SetScriptText( CHATMGR->GetChatMsg( 945 ) );
	m_pMsg1->SetScriptText( CHATMGR->GetChatMsg( 946 ) );
	m_pMsg2->SetScriptText( CHATMGR->GetChatMsg( 947 ) );
	m_pMsg3->SetScriptText( CHATMGR->GetChatMsg( 948 ) );
	m_pMoneyStatic->SetStaticText( CHATMGR->GetChatMsg( 949 ) );

	// 090522 ShinJS --- 포상금 EditBox에 숫자만 입력되도록 한다
	m_pMoneyEdit->SetValidCheck( VCM_NUMBER );

	m_bRender = FALSE;
}

void CGFWarDeclareDlg::ShowMoneyEdit()
{
	const DWORD enableColor = RGB( 255, 255, 255 );

	if( m_pMoneyCheck->IsChecked() )
	{
		m_pMoneyEdit->SetImageRGB( enableColor );
        m_pMoneyEdit->SetDisable( FALSE );
		m_pMoneyEdit->SetEditText( "0" );
	}
	else
	{
		m_pMoneyEdit->SetImageRGB( enableColor );
		m_pMoneyEdit->SetDisable( TRUE );	
		m_pMoneyEdit->SetEditText( "" );
	}
}

BOOL CGFWarDeclareDlg::IsChecked()
{
	return m_pMoneyCheck->IsChecked();
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGFWarResultDlg::CGFWarResultDlg()
{
	m_type = WT_GUILDFIELDWAR_RESULT_DLG;
}

CGFWarResultDlg::~CGFWarResultDlg()
{
}

void CGFWarResultDlg::Linking()
{
	m_pTitle = (cStatic*)GetWindowForID( GFW_RESULT_TITILE );
	m_pMsg = (cTextArea*)GetWindowForID( GFW_RESULT_MSG );
	m_pStatic0 = (cStatic*)GetWindowForID( GFW_RESULT_0 );
	m_pStatic1 = (cStatic*)GetWindowForID( GFW_RESULT_1 );
	m_pStatic2 = (cStatic*)GetWindowForID( GFW_RESULT_2 );
	m_pStatic3 = (cStatic*)GetWindowForID( GFW_RESULT_3 );
	m_pStatic4 = (cStatic*)GetWindowForID( GFW_RESULT_4 );
	m_pStatic5 = (cStatic*)GetWindowForID( GFW_RESULT_5 );
	m_pOkBtn = (cButton*)GetWindowForID( GFW_RESULT_OKBTN );
	m_pCancelBtn = (cButton*)GetWindowForID( GFW_RESULT_CANCELBTN );
	m_pConfirmBtn = (cButton*)GetWindowForID( GFW_RESULT_CONFIRMBTN );

	m_myGuildName	= ( cStatic* )GetWindowForID( GFW_MY_GUILD_NAME );
	m_myGuildLevel	= ( cStatic* )GetWindowForID( GFW_MY_GUILD_LEVEL );
	m_myGuildMaster	= ( cStatic* )GetWindowForID( GFW_MY_GUILD_MASTER );
	m_myGuildMoney	= ( cStatic* )GetWindowForID( GFW_MY_GUILD_MONEY );

	ASSERT( m_myGuildName && m_myGuildLevel && m_myGuildMaster && m_myGuildMoney );
}

//void CGFWarResultDlg::ShowDeclare( GUILDINFO* pInfo )
void CGFWarResultDlg::ShowDeclare( const MSG_GUILD_LIST::Data& data )
{
	// 상대로부터 선전포고를 받았을 때 실행됨

	m_pTitle->SetStaticText( CHATMGR->GetChatMsg( 966 ) );
	m_pMsg->SetScriptText( CHATMGR->GetChatMsg( 950 ) );
	m_pStatic0->SetStaticText( CHATMGR->GetChatMsg( 951 ) );

	char line[ MAX_PATH ];

	if( data.UnionIdx )
	{
		sprintf( line, CHATMGR->GetChatMsg(1466), data.GuildName, data.mUnionName );
	}
	else
	{
		sprintf( line, CHATMGR->GetChatMsg( 952 ), data.GuildName );
	}
	
	m_pStatic1->SetStaticText( line );
	sprintf( line, CHATMGR->GetChatMsg( 954 ), data.GuildLevel );
	m_pStatic2->SetStaticText( line );
	sprintf( line, CHATMGR->GetChatMsg( 955 ), data.MasterName );
	m_pStatic3->SetStaticText( line );
	//sprintf( line, CHATMGR->GetChatMsg( 956 ), data.MarkName );
	sprintf( line, CHATMGR->GetChatMsg( 956 ), AddComma( data.MarkName ) );
	m_pStatic4->SetStaticText( line );
	//sprintf( line, CHATMGR->GetChatMsg( 953 ), GetMapName( data.MapNum ) );
	//m_pStatic5->SetStaticText( line );

	{
		cDialog::SetActive( TRUE );
		m_pStatic3->SetActive( TRUE );
		m_pStatic4->SetActive( TRUE );
		m_pOkBtn->SetActive( TRUE );
		m_pCancelBtn->SetActive( TRUE );
		m_pConfirmBtn->SetActive( FALSE );

		// 길드창이 열려있으면 겹쳐지지 않게 배열하자
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
			ASSERT( dialog );

			if( dialog->IsActive() )
			{
                const DISPLAY_INFO& screen	= GAMERESRCMNGR->m_GameDesc.dispInfo;
				const DWORD			space	= 50;
				const DWORD			x		= ( screen.dwWidth - m_width - dialog->GetWidth() - space ) / 2;
				const DWORD			y		= ( screen.dwHeight - max( dialog->GetHeight(), m_height ) ) / 2;

				SetAbsXY( x + space + dialog->GetWidth(), y );

				dialog->SetAbsXY( x, y );
				dialog->SetActive( TRUE );
			}
		}
		
	}	

	// 자신의 정보를 표시한다
	{
		{			
			const char* allianceName = HERO->GetGuildUnionName();

			if( *allianceName )
			{
				sprintf( line, CHATMGR->GetChatMsg(1466), GUILDMGR->GetGuildName(), allianceName );
			}
			else
			{
				sprintf( line, CHATMGR->GetChatMsg( 952 ), GUILDMGR->GetGuildName() );
			}
			
			m_myGuildName->SetStaticText( line );
		}
		
		sprintf( line, CHATMGR->GetChatMsg( 954 ), GUILDMGR->GetLevel() );
		m_myGuildLevel->SetStaticText( line );

		sprintf( line, CHATMGR->GetChatMsg( 955 ), GUILDMGR->GetMasterName() );
		m_myGuildMaster->SetStaticText( line );

		//sprintf( line, CHATMGR->GetChatMsg( 956 ), data.MarkName );
		sprintf( line, CHATMGR->GetChatMsg( 956 ), AddComma( data.MarkName ) );
		m_myGuildMoney->SetStaticText( line );
	}
}

void CGFWarResultDlg::ShowResult( DWORD dwKind )
{
	cDialog::SetActive( TRUE );
	m_pStatic3->SetActive( FALSE );
	m_pStatic4->SetActive( FALSE );
	m_pOkBtn->SetActive( FALSE );
	m_pCancelBtn->SetActive( FALSE );
	m_pConfirmBtn->SetActive( TRUE );

	m_myGuildName->SetStaticText( "" );
	m_myGuildLevel->SetStaticText( "" );
	m_myGuildMaster->SetStaticText( "" );
	m_myGuildMoney->SetStaticText( "" );

	m_pTitle->SetStaticText( CHATMGR->GetChatMsg( 944 ) );
	switch( dwKind )
	{
	case 0:		// fight
		{
			//임시로 막음
			m_pMsg->SetScriptText( CHATMGR->GetChatMsg( 957 ) );
			m_pStatic0->SetStaticText( CHATMGR->GetChatMsg( 958 ) );
			m_pStatic1->SetStaticText( CHATMGR->GetChatMsg( 959 ) );
//			sprintf( temp, CHATMGR->GetChatMsg( 960 ), 10 );			
//			m_pStatic2->SetStaticText( temp );
			m_pStatic2->SetStaticText( "" );
			//
		}
		break;
	case 1:		// surrend
		{
			//임시로 막음
			m_pMsg->SetScriptText( CHATMGR->GetChatMsg( 961 ) );
//			m_pStatic0->SetStaticText( CHATMGR->GetChatMsg( 958 ) );
			m_pStatic0->SetStaticText( "" );
//			sprintf( temp, CHATMGR->GetChatMsg( 960 ), 5 );			
//			m_pStatic1->SetStaticText( temp );
			m_pStatic1->SetStaticText( "" );
			m_pStatic2->SetStaticText( "" );
			//
		}
		break;
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGFWarInfoDlg::CGFWarInfoDlg()
{
	m_type = WT_GUILDFIELDWAR_INFO_DLG;
	m_nSelectedIdx = -1;
}

CGFWarInfoDlg::~CGFWarInfoDlg()
{
}

DWORD CGFWarInfoDlg::ActionEvent( CMouse* mouseInfo )
{
	DWORD we = WE_NULL;
	if( !m_bActive )	return we;
	we = cDialog::ActionEvent( mouseInfo );	

	if( m_pGuildListDlg->PtIdxInRow(mouseInfo->GetMouseX(),mouseInfo->GetMouseY()) != -1 )
	{
		if( we & WE_LBTNCLICK && (WINDOWMGR->IsMouseDownUsed() == FALSE) )
		{		
			int Idx = m_pGuildListDlg->GetCurSelectedRowIdx();
			if( Idx != -1 )
			{
				m_nSelectedIdx = Idx;
			}
		}
	}
	return we;
}

void CGFWarInfoDlg::Linking()
{
	m_pTitle = (cStatic*)GetWindowForID( GFW_INFO_TITLE );
	m_pMsg = (cStatic*)GetWindowForID( GFW_INFO_MSG );
	m_pGuildListDlg = (cListDialog*)GetWindowForID( GFW_INFO_LIST );
	m_pGuildListDlg->SetShowSelect(TRUE);
	m_pText = (cTextArea*)GetWindowForID( GFW_INFO_TEXT );
	m_pStatic0 = (cStatic*)GetWindowForID( GFW_INFO_STATIC_0 );
	m_pStatic1 = (cStatic*)GetWindowForID( GFW_INFO_STATIC_1 );
	m_pStatic2 = (cStatic*)GetWindowForID( GFW_INFO_STATIC_2 );
	m_pSuggestOkBtn = (cButton*)GetWindowForID( GFW_INFO_SUGGEST_OKBTN );
	m_pSurrendOkBtn = (cButton*)GetWindowForID( GFW_INFO_SURREND_OKBTN );
	m_pGuildUnionRemoveOkBtn = (cButton*)GetWindowForID(GFW_GDU_REMOVE_OKBTN);
	m_pCancelBtn = (cButton*)GetWindowForID( GFW_INFO_CANCELBTN );
}

void CGFWarInfoDlg::ShowSuggest()
{
	cDialog::SetActive( TRUE );

	m_pSuggestOkBtn->SetActive( TRUE );
	m_pSurrendOkBtn->SetActive( FALSE );
	m_pGuildUnionRemoveOkBtn->SetActive( FALSE );

	m_pTitle->SetStaticText( CHATMGR->GetChatMsg( 967 ) );
	m_pMsg->SetStaticText( CHATMGR->GetChatMsg( 964 ) );
	m_pText->SetScriptText( CHATMGR->GetChatMsg( 965 ) );
	m_pStatic0->SetStaticText( "" );
	m_pStatic1->SetStaticText( "" );
	m_pStatic2->SetStaticText( "" );

	AddGuildInfoToList();
}

void CGFWarInfoDlg::ShowSurrend()
{
	cDialog::SetActive( TRUE );
	
	m_pSurrendOkBtn->SetActive( TRUE );
	m_pSuggestOkBtn->SetActive( FALSE );
	m_pGuildUnionRemoveOkBtn->SetActive( FALSE );

	m_pTitle->SetStaticText( CHATMGR->GetChatMsg( 968 ) );
	m_pMsg->SetStaticText( CHATMGR->GetChatMsg( 962 ) );
	m_pText->SetScriptText( CHATMGR->GetChatMsg( 963 ) );
	m_pStatic0->SetStaticText( "" );
	m_pStatic1->SetStaticText( "" );
	m_pStatic2->SetStaticText( "" );

	AddGuildInfoToList();
}

void CGFWarInfoDlg::ShowGuildUnion()
{
	cDialog::SetActive( TRUE );

	m_pSuggestOkBtn->SetActive( FALSE );
	m_pSurrendOkBtn->SetActive( FALSE );
	m_pGuildUnionRemoveOkBtn->SetActive( TRUE );

	m_pTitle->SetStaticText( CHATMGR->GetChatMsg( 942 ) );
	m_pMsg->SetStaticText( CHATMGR->GetChatMsg( 943 ) );
	m_pText->SetScriptText( CHATMGR->GetChatMsg( 944 ) );
	m_pStatic0->SetStaticText( "" );
	m_pStatic1->SetStaticText( "" );
	m_pStatic2->SetStaticText( "" );

	AddGuildUnionInfoToList();
}

void CGFWarInfoDlg::AddGuildInfoToList()
{
	m_pGuildListDlg->RemoveAll();
	
	const CGuildWarManager::EnemyList& enemeyList = GUILDWARMGR->GetEnemyList();
	
	char line[ MAX_PATH ];
	
	for( CGuildWarManager::EnemyList::const_iterator it = enemeyList.begin(); enemeyList.end() != it; ++it )
	{
		const MSG_GUILD_LIST::Data& data = *it;

		char alliance[ MAX_PATH ] = { 0 };

		// 주의: MarkName을 길드전 선포금으로 쓰고 있다
		if( *( data.mUnionName ) )
		{
			sprintf( alliance, "%s%s",
				data.mUnionName,
				data.mIsUnionMaster ? "*" : "" );
		}

		char money[ MAX_PATH ] = { 0 };

		if( data.MarkName )
		{
			sprintf( money, "%s%s",
				AddComma( data.MarkName ),
				CHATMGR->GetChatMsg(1467) );
		}

		if( *alliance || *money )
		{
			sprintf( line, "%s (%s%s%s)",
				data.GuildName,
				alliance,
				*alliance && *money ? " " : "",
				money );
		}
		else
		{
			sprintf( line, "%s", data.GuildName );
		}
		
		m_pGuildListDlg->AddItem( line, 0xffffffff );
	}
}
	
void CGFWarInfoDlg::AddGuildUnionInfoToList() 
{
	m_pGuildListDlg->RemoveAll();

	const sGUILDIDXNAME* pUnion = GUILDUNION->GetData();
	for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
	{
		if( pUnion[i].dwGuildIdx && pUnion[i].dwGuildIdx != HERO->GetGuildIdx() )
			m_pGuildListDlg->AddItem( pUnion[i].sGuildName, 0xffffffff );
	}
}


void CGFWarInfoDlg::SetActive( BOOL isActive )
{
	if( isActive )
	{
		AddGuildInfoToList();
		AddGuildUnionInfoToList();
	}
	
	cDialog::SetActive( isActive );
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGuildWarInfoDlg::CGuildWarInfoDlg()
{
	m_type = WT_GUILDWAR_INFO_DLG;
}

CGuildWarInfoDlg::~CGuildWarInfoDlg()
{
}

void CGuildWarInfoDlg::Linking()
{
	m_pBlackGuildListDlg = (cListDialog*)GetWindowForID( GW_BLACKLIST );
	m_pWhiteGuildListDlg = (cListDialog*)GetWindowForID( GW_WHITELIST );
	m_pStatic = (cStatic*)GetWindowForID( GW_INFOSTATIC );
	m_pCloseBtn = (cButton*)GetWindowForID( GW_INFOCLOSEBTN );	
}

void CGuildWarInfoDlg::SetActive( BOOL val )
{
	cDialog::SetActive(val);

	if( val )	AddListData();
}

void CGuildWarInfoDlg::AddListData()
{
	char line[ MAX_PATH ];

	// 교전 중인 길드 정보를 업데이트한다
	{
		m_pBlackGuildListDlg->RemoveAll();

		const CGuildWarManager::EnemyList& enemeyList = GUILDWARMGR->GetEnemyList();

		for( CGuildWarManager::EnemyList::const_iterator it = enemeyList.begin(); enemeyList.end() != it; ++it )
		{
			const MSG_GUILD_LIST::Data& data = *it;

			char alliance[ MAX_PATH ] = { 0 };

			// 주의: MarkName을 길드전 선포금으로 쓰고 있다
			if( *( data.mUnionName ) )
			{
				sprintf( alliance, "%s%s",
					data.mUnionName,
					data.mIsUnionMaster ? "*" : "" );
			}

			char money[ MAX_PATH ] = { 0 };

			if( data.MarkName )
			{
				sprintf( money, "%s%s",
					AddComma( data.MarkName ),
					CHATMGR->GetChatMsg(1467) );
			}

			if( *alliance || *money )
			{
				sprintf( line, "%s (%s%s%s)",
					data.GuildName,
					alliance,
					*alliance && *money ? " " : "",
					money );
			}
			else
			{
				sprintf( line, "%s", data.GuildName );
			}

			m_pBlackGuildListDlg->AddItem( line, 0xffffffff );
		}

		if( m_pBlackGuildListDlg->GetItemCount() == 0 )
		{
			m_pBlackGuildListDlg->AddItem( CHATMGR->GetChatMsg( 969 ), 0xffffffff );
		}
	}
	
	// 동맹 길드 정보 업데이트
	{
		m_pWhiteGuildListDlg->RemoveAll();

		const sGUILDIDXNAME* pUnion = GUILDUNION->GetData();
		for( int i = 0; i < MAX_GUILD_UNION_NUM; ++i )
		{
			if( pUnion[i].dwGuildIdx && pUnion[i].dwGuildIdx != HERO->GetGuildIdx() )
				m_pWhiteGuildListDlg->AddItem( pUnion[i].sGuildName, 0xffffffff );
		}

		if( m_pWhiteGuildListDlg->GetItemCount() == 0 )
		{
			m_pWhiteGuildListDlg->AddItem( CHATMGR->GetChatMsg( 970 ), 0xffffffff );
		}
	}

	GUILDWARMGR->GetVictory( line );
	m_pStatic->SetStaticText( line );	
}