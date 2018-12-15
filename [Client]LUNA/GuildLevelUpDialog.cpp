#include "stdafx.h"
#include "GuildLevelUpDialog.h"
#include "interface/cStatic.h"
#include "WindowIDEnum.h"
#include "GuildManager.h"
#include "ObjectManager.h"
//#include "GuildDialog.h"
#include "GameIn.h"
#include "NpcScriptDialog.h"
#include "ObjectStateManager.h"
#include "cWindowManager.h"
#include "ChatManager.h"


CGuildLevelUpDialog::CGuildLevelUpDialog()
{
	m_type = WT_GUILDLEVELUPDLG;
}


CGuildLevelUpDialog::~CGuildLevelUpDialog()
{}


void CGuildLevelUpDialog::Linking()
{
	// 버튼 바인딩
	{
		mSubmitButton = GetWindowForID( GD_LUOKBTN );
		ASSERT( mSubmitButton );
	}

	// 길드 레벨 충족되었을 경우 이미지
	{
		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU1COMPLETE );
			ASSERT( image );

			mCompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU2COMPLETE );
			ASSERT( image );

			mCompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU3COMPLETE );
			ASSERT( image );

			mCompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU4COMPLETE );
			ASSERT( image );

			mCompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU5COMPLETE );
			ASSERT( image );

			mCompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU6COMPLETE );
			ASSERT( image );

			mCompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU7COMPLETE );
			ASSERT( image );

			mCompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU8COMPLETE );
			ASSERT( image );

			mCompleteStaticList.push_back( image );
		}
	}

	// 길드 레벨을 충족하지 못했을 경우 이미지
	{
		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU1NOTCOMPLETE );
			ASSERT( image );

			mIncompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU2NOTCOMPLETE );
			ASSERT( image );

			mIncompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU3NOTCOMPLETE );
			ASSERT( image );

			mIncompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU4NOTCOMPLETE );
			ASSERT( image );

			mIncompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU5NOTCOMPLETE );
			ASSERT( image );

			mIncompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU6NOTCOMPLETE );
			ASSERT( image );

			mIncompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU7NOTCOMPLETE );
			ASSERT( image );

			mIncompleteStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU8NOTCOMPLETE );
			ASSERT( image );

			mIncompleteStaticList.push_back( image );
		}
	}

	// 레벨 텍스트
	{
		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU1 );
			ASSERT( image );

			mLevelStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU2 );
			ASSERT( image );

			mLevelStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU3 );
			ASSERT( image );

			mLevelStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU4 );
			ASSERT( image );

			mLevelStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU5 );
			ASSERT( image );

			mLevelStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU6 );
			ASSERT( image );

			mLevelStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU7 );
			ASSERT( image );

			mLevelStaticList.push_back( image );
		}

		{
			cStatic* image = ( cStatic* )GetWindowForID( GD_LU8 );
			ASSERT( image );

			mLevelStaticList.push_back( image );
		}
	}

	ASSERT( mCompleteStaticList.size() == mIncompleteStaticList.size() && mCompleteStaticList.size() == mLevelStaticList.size() );
}


void CGuildLevelUpDialog::SetLevel( DWORD level )
{
	ASSERT( 0 < level );

	// C++의 인덱스는 0이 기준이나, 길드 레벨은 1부터 시작이므로 하나 감소
	--level;

	// 레벨 비충족 이미지를 켜고 끈다
	if( mCompleteStaticList.size() > level )
	{
		StaticList::const_iterator end_it = mIncompleteStaticList.begin();

		std::advance( end_it, level );

		for(	StaticList::const_iterator it = mIncompleteStaticList.begin();
				end_it != it;
				++it )
		{
			cStatic* image = *it;

			image->SetActive( FALSE );
		}

		for(	;
				mIncompleteStaticList.end() != end_it;
				++end_it )
		{
			cStatic* image = *it;

			image->SetActive( TRUE );
		}
	}

	// 레벨 충족 이미지를 켜고 끈다
	if( mCompleteStaticList.size() > level )
	{
		StaticList::const_iterator end_it = mCompleteStaticList.begin();

		std::advance( end_it, level );

		for(	StaticList::const_iterator it = mCompleteStaticList.begin();
				end_it != it;
				++it )
		{
			cStatic* image = *it;

			image->SetActive( TRUE );
		}

		for(	;
				mCompleteStaticList.end() != end_it;
				++end_it )
		{
			cStatic* image = *it;

			image->SetActive( FALSE );
		}
	}

	// 레벨 표시도 바꿔준다
	if( mLevelStaticList.size() > level )
	{
		for(	StaticList::const_iterator it = mLevelStaticList.begin();
				mLevelStaticList.end() != it;
				++it )
		{
			cStatic* image = *it;

			image->SetFGColor( RGB_HALF( 255, 255, 255 ) );
		}

		StaticList::const_iterator selected = mLevelStaticList.begin();

		std::advance( selected, level );

		cStatic* image = *selected;

		image->SetFGColor( RGB_HALF( 255, 255, 0 ) );
	}

	if( mLevelStaticList.size() - 1 == level )
	{
		mSubmitButton->SetActive( FALSE );
	}

	/*
	DWORD i = 0;

	for( i=0; i<lvl; ++i)
	{
		m_pLevelupNotComplete[i]->SetActive(FALSE);
		m_pLevelupComplete[i]->SetActive(TRUE);
	}
	for( i=lvl; i< 4; ++i)
	{
		m_pLevelupNotComplete[i]->SetActive(TRUE);
		m_pLevelupComplete[i]->SetActive(FALSE);
	}
	for( i=0; i<5; ++i )
	{
		m_pLevel[i]->SetFGColor(RGB_HALF(255, 255, 255));
	}

	m_pLevel[level-1]->SetFGColor(RGB_HALF(255,255,0));
	*/
}

void CGuildLevelUpDialog::SetActive( BOOL IsActive )
{
	if( IsActive )
	{
		// 길마가 아니면 이 창을 열 수 없음... 쉬운 길마 찾기: 길드 창의 마스터 이름과 같은지 비교
		{
			CHero* hero = OBJECTMGR->GetHero();

			if( strcmp( hero->GetObjectName(), GUILDMGR->GetMasterName() ) )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 579 ) );
				return;
			}
		}

		// 최대 레벨에 도달했어도 마찬가지. 어떻게 찾을까? 현재 레벨보다 높은 세팅이 없으면 도달했다는 반증
		{
			if( ! GUILDMGR->GetLevelSetting( GUILDMGR->GetLevel() + 1 ) )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 604 ) );
				return;
			}
		}

		// 레벨 세팅
		{
			SetLevel( GUILDMGR->GetLevel() );
		}		
	}
	else
	{
		// NPC 스크립트 창(NpcChat.bin)
		cDialog* dialog = WINDOWMGR->GetWindowForID( NSI_SCRIPTDLG );
		ASSERT( dialog );

		CHero* hero = OBJECTMGR->GetHero();

		if(		hero									&&
				eObjectState_Deal == hero->GetState()	&&
			!	dialog->IsActive()  )
		{
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
		}
	}

	cDialog::SetActive( IsActive );
}


void CGuildLevelUpDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	switch( id )
	{
	case GD_LUOKBTN:
		{
			// 주의: 최대 레벨 조건이나 마스터 여부는 이미 창 열때 조사했다.

			const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting( GUILDMGR->GetLevel() + 1 );
			ASSERT( setting );			

			CHero* hero = OBJECTMGR->GetHero();

			// 돈 충분하나
			if( setting->mRequiredMoney > hero->GetMoney() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 605 ) );
				break;
			}
			else if( setting->mRequiredMasterLevel > hero->GetLevel() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1092 ), setting->mRequiredMasterLevel );
				break;
			}

			// 길드 포인트는 어떤가
			if( setting->mRequiredScore > GUILDMGR->GetScore() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 605 ) );
				return;
			}

			// 서버에 요청(
			{
				MSGBASE message;
				message.Category	= MP_GUILD;
				message.Protocol	= MP_GUILD_LEVELUP_SYN;
				message.dwObjectID	= hero->GetID();

				NETWORK->Send( &message, sizeof( message ) );	
			}

			SetDisable( TRUE );
			break;
		}
	}
}