#include "stdafx.h"
#include "GuildDialog.h"
#include "WindowIDEnum.h"
#include "Interface/cResourceManager.h"
#include "interface/cEditBox.h"
#include "interface/cListDialog.h"
#include "Interface/cWindowManager.h"
#include "interface/cStatic.h"
#include "interface/cTextArea.h"
#include "cMsgBox.h"
#include "../Input/Mouse.h"
#include "GuildFieldWarDialog.h"
#include "GuildManager.h"
//#include "Guild.h"
#include "GuildFieldWar.h"
#include "GuildUnion.h"
#include "GuildLevelUpDialog.h"
#include "GuildNoticeDlg.h"
#include "GuildJoinDialog.h"
#include "GameIn.h"
#include "ObjectManager.h"
#include "ChatManager.h"
#include "CharacterCalcManager.h"
#include "FilteringTable.h"
// 080417 길드 스킬 학습위해 참조
#include "cSkillTrainingDlg.h"


//extern int	g_nServerSetNum;


// STL 컨테이너의 커스텀 정렬을 위한 함수자
class SortLevelByLowOrder
{
public:
	bool operator() ( const GUILDMEMBERINFO& lhs, const GUILDMEMBERINFO& rhs ) const
	{
		return lhs.Memberlvl < rhs.Memberlvl;
	}
};


class SortLevelByHighOrder
{
public:
	bool operator() ( const GUILDMEMBERINFO& lhs, const GUILDMEMBERINFO& rhs ) const
	{
		return lhs.Memberlvl > rhs.Memberlvl;
	}
};


class SortRankByLowOrder
{
public:
	bool operator() ( const GUILDMEMBERINFO& lhs, const GUILDMEMBERINFO& rhs ) const
	{
		return lhs.Rank < rhs.Rank;
	}
};


class SortRankByHighOrder
{
public:
	bool operator() ( const GUILDMEMBERINFO& lhs, const GUILDMEMBERINFO& rhs ) const
	{
		return lhs.Rank > rhs.Rank;
	}
};


CGuildDialog::CGuildDialog() :
mSortRankFlag( FALSE ),
mSortLevelFlag( FALSE ),
mMasterIndex( 0 )
{
	m_type = WT_GUILDDLG;
	m_CurGuildMode = -1;
}


CGuildDialog::~CGuildDialog()
{}


void CGuildDialog::Add( cWindow* window )
{
	switch( window->GetType() )
	{
	case WT_PUSHUPBUTTON:
		{
			AddTabBtn( curIdx1++, ( cPushupButton* )window );
			break;
		}
	case WT_DIALOG:
		{
			AddTabSheet( curIdx2++, window );
			break;
		}
	default:
		{
			cTabDialog::Add(window);
			break;
		}
	}
}


void CGuildDialog::Linking()
{
	// 초기화
	{
		mMemberList.clear();
		mLoginPlayerSet.clear();
	}

	ASSERT( GetTabNum() == 2 );

	// 첫번째 시트에는 세팅할 텍스트 컨트롤과 관리용 버튼 몇개가 들어있다.
	{
		cDialog* dialog = ( cDialog* )GetTabSheet( 0 );

		mGuildName	= ( cStatic* )dialog->GetWindowForID( GD_NAME		);
		mGuildLevel	= ( cStatic* )dialog->GetWindowForID( GD_LEVEL		);
		mGuildScore	= ( cStatic* )dialog->GetWindowForID( GD_SCORE		);
		mMasterName	= ( cStatic* )dialog->GetWindowForID( GD_MASTER		);
		mMemberSize	= ( cStatic* )dialog->GetWindowForID( GD_MEMBERNUM	);
		mUnionName	= ( cStatic* )dialog->GetWindowForID( GD_UNIONNAME	);
		
		ASSERT( mGuildName && mGuildLevel && mGuildScore && mMasterName && mMemberSize && mUnionName );
	}

	// 두번째 시트에는 길드 기능을 실행할 버튼 컨트롤이 들어있다.
	{
		cDialog* dialog = ( cDialog* )GetTabSheet( 1 );

		mKickMemberButton		= dialog->GetWindowForID( GD_BANMEMBER		);
		mNameMemberButton		= dialog->GetWindowForID( GD_GIVEMEMBERNICK );		
		mInviteMemberButton		= dialog->GetWindowForID( GD_INVITEMEMBER	);
		mNoticeButton			= dialog->GetWindowForID( GD_NOTICE			);
		mLeaveMemberButton		= dialog->GetWindowForID( GD_SECEDE			);
		mUnionInviteButton		= dialog->GetWindowForID( GD_INVITEUNION	);
		mLeaveUnionButton		= dialog->GetWindowForID( GD_SECEDEUNION 	);
		mKickUnionButton		= dialog->GetWindowForID( GD_REMOVEUNION 	);
		mDeclareWarButton		= dialog->GetWindowForID( GD_DECLARE 		);
		mSuggestStopWarButton	= dialog->GetWindowForID( GD_SUGGEST 		);
		mSurrenderButton		= dialog->GetWindowForID( GD_SURRENDER		);
		mAddTimeButton			= dialog->GetWindowForID( GD_PLUSTIME		);
		mManageApprenticeButton	= dialog->GetWindowForID( GD_MUNHA			);
		mUnionCreateButton		= dialog->GetWindowForID( GD_CREATE_UNION	);
		mUnionDisbandButton		= dialog->GetWindowForID( GD_DISBAND_UNION	);
		mRetireButton			= dialog->GetWindowForID( GD_RETIRE_MASTER	);
		mWarehouseRankButton	= dialog->GetWindowForID( GD_WAREHOUSE_RANK );
		mRankMemberButton		= dialog->GetWindowForID( GD_GIVEMEMBERRANK );
		
		ASSERT( mKickMemberButton && mNameMemberButton && mRankMemberButton &&
				mInviteMemberButton && mNoticeButton && mLeaveMemberButton &&
				mUnionInviteButton && mLeaveUnionButton && mKickUnionButton &&
				mDeclareWarButton && mSuggestStopWarButton && mSurrenderButton &&
				mAddTimeButton && mManageApprenticeButton &&
				mUnionCreateButton && mUnionDisbandButton );
		ASSERT( mWarehouseRankButton && mRetireButton && mRankMemberButton );
	}

	// 길드원 목록 컨트롤 바인딩
	{
		mMemberNameList	= (cListDialog*)GetWindowForID( GD_MEMBERNAMELIST );
		mMemberRankList	= (cListDialog*)GetWindowForID( GD_MEMBERRANKLIST );
		mMemberLvList	= (cListDialog*)GetWindowForID( GD_MEMBERLVLIST );

		mMemberNameList->SetShowSelect( TRUE );
		mMemberRankList->SetShowSelect( TRUE );
		mMemberLvList->SetShowSelect( TRUE );

		mMemberNameList->SetHeight( 158 );
		mMemberRankList->SetHeight( 158 );
		mMemberLvList->SetHeight( 158 );
	}
}


void CGuildDialog::DeleteMemberAll()
{	
	mMemberNameList->RemoveAll();
	mMemberRankList->RemoveAll();
	mMemberLvList->RemoveAll();
	mMemberList.clear();
}


void CGuildDialog::SetActive( BOOL isActive )
{
	CHero* hero = OBJECTMGR->GetHero();

	if(		isActive		&&
			hero			&&
		!	hero->GetGuildIdx() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 578 ) );
		return;
	}

	cTabDialog::SetActive( isActive );

	if( isActive )
	{
		mKickMemberIndex = 0;

		// 첫번째 탭을 켜자...
		{
			cDialog* dialog = ( cDialog* )GetTabSheet( 0 );
			ASSERT( dialog );

			dialog->SetActiveRecursive( TRUE );
		}

		// 081106 LUJ, 해외 요청으로 비활성화 버튼 색상을 좀더 강하게 처리함
		const DWORD unableColor = RGB( 0, 0, 0 );
		const DWORD enableColor = RGB( 255, 255, 255 );

		// 모든 길드 기능을 일단 켜자
		{
			mKickMemberButton->SetActive( TRUE );
			mNameMemberButton->SetActive( TRUE );
			mRankMemberButton->SetActive( TRUE );
			mInviteMemberButton->SetActive( TRUE );			
			mLeaveMemberButton->SetActive( TRUE );
			mNoticeButton->SetActive( TRUE );			
			mLeaveUnionButton->SetActive( TRUE );
			mKickUnionButton->SetActive( TRUE );
			mDeclareWarButton->SetActive( TRUE );
			mSuggestStopWarButton->SetActive( TRUE );
			mSurrenderButton->SetActive( TRUE );
			mUnionInviteButton->SetActive( TRUE );
			mUnionCreateButton->SetActive( TRUE );
			mUnionDisbandButton->SetActive( TRUE );
			mRetireButton->SetActive( TRUE );
			mWarehouseRankButton->SetActive( TRUE );
			mManageApprenticeButton->SetActive( TRUE );
			mAddTimeButton->SetActive( TRUE );

			mKickMemberButton->SetDisable( FALSE );
			mNameMemberButton->SetDisable( FALSE );
			mRankMemberButton->SetDisable( FALSE );
			mInviteMemberButton->SetDisable( FALSE );			
			mLeaveMemberButton->SetDisable( FALSE );		
			mNoticeButton->SetDisable( FALSE );			
			mLeaveUnionButton->SetDisable( FALSE );
			mKickUnionButton->SetDisable( FALSE );
			mDeclareWarButton->SetDisable( FALSE );
			mSuggestStopWarButton->SetDisable( FALSE );
			mSurrenderButton->SetDisable( FALSE );
			mManageApprenticeButton->SetDisable( FALSE );
			mUnionInviteButton->SetDisable( FALSE );
			mUnionCreateButton->SetDisable( FALSE );
			mUnionDisbandButton->SetDisable( FALSE );
			mRetireButton->SetDisable( FALSE );
			mWarehouseRankButton->SetDisable( FALSE );
			mManageApprenticeButton->SetDisable( TRUE );
			mAddTimeButton->SetDisable( FALSE );
			
			mKickMemberButton->SetImageRGB( enableColor );
			mNameMemberButton->SetImageRGB( enableColor );
			mRankMemberButton->SetImageRGB( enableColor );
			mInviteMemberButton->SetImageRGB( enableColor );			
			mLeaveMemberButton->SetImageRGB( enableColor );		
			mNoticeButton->SetImageRGB( enableColor );			
			mLeaveUnionButton->SetImageRGB( enableColor );
			mKickUnionButton->SetImageRGB( enableColor );
			mDeclareWarButton->SetImageRGB( enableColor );
			mSuggestStopWarButton->SetImageRGB( enableColor );
			mSurrenderButton->SetImageRGB( enableColor );
			mManageApprenticeButton->SetImageRGB( enableColor );
			mUnionInviteButton->SetImageRGB( enableColor );
			mUnionCreateButton->SetImageRGB( enableColor );
			mUnionDisbandButton->SetImageRGB( enableColor );
			mRetireButton->SetImageRGB( enableColor );
			mManageApprenticeButton->SetImageRGB( unableColor );
			mWarehouseRankButton->SetImageRGB( enableColor );
			mAddTimeButton->SetImageRGB( enableColor );
		}

		// 길드 상태에 따라 끌 것들을 처리한다
		{
			// 연합
			if( hero->GetGuildUnionIdx() )
			{
				mUnionCreateButton->SetDisable( TRUE );
				mUnionCreateButton->SetImageRGB( unableColor );

				if( GUILDUNION->IsMaster( hero ) )
				{
					mLeaveUnionButton->SetDisable( TRUE );
					mLeaveUnionButton->SetImageRGB( unableColor );
				}
				else
				{
					mUnionCreateButton->SetDisable( TRUE );
					mUnionCreateButton->SetImageRGB( unableColor );

					mUnionDisbandButton->SetDisable( TRUE );
					mUnionDisbandButton->SetImageRGB( unableColor );

					mUnionInviteButton->SetDisable( TRUE );
					mUnionInviteButton->SetImageRGB( unableColor );

					mKickUnionButton->SetDisable( TRUE );
					mKickUnionButton->SetImageRGB( unableColor );

					mDeclareWarButton->SetDisable( TRUE );
					mDeclareWarButton->SetImageRGB( unableColor );

					mSuggestStopWarButton->SetDisable( TRUE );
					mSuggestStopWarButton->SetImageRGB( unableColor );

					mSurrenderButton->SetDisable( TRUE );
					mSurrenderButton->SetImageRGB( unableColor );
				}
			}
			else
			{
				mUnionInviteButton->SetDisable( TRUE );
				mUnionInviteButton->SetImageRGB( unableColor );

				mLeaveUnionButton->SetDisable( TRUE );
				mLeaveUnionButton->SetImageRGB( unableColor );

				mKickUnionButton->SetDisable( TRUE );
				mKickUnionButton->SetImageRGB( unableColor );

				mUnionDisbandButton->SetDisable( TRUE );
				mUnionDisbandButton->SetImageRGB( unableColor );
			}

			// 길드전
			if( GUILDWARMGR->GetEnemySize() )
			{
				mUnionCreateButton->SetDisable( TRUE );
				mUnionCreateButton->SetImageRGB( unableColor );
			}
			else
			{
				mSuggestStopWarButton->SetDisable( TRUE );
				mSuggestStopWarButton->SetImageRGB( unableColor );

				mSurrenderButton->SetDisable( TRUE );
				mSurrenderButton->SetImageRGB( unableColor );
			}
		}
		
		// 직위에 따라 보이지 않아야하는 것만 처리한다
		switch( hero->GetGuildMemberRank() )
		{
		case GUILD_MASTER:
			{
				mLeaveMemberButton->SetDisable( TRUE );
				mLeaveMemberButton->SetImageRGB( unableColor );
				break;
			}
		case GUILD_VICEMASTER:
			{
				mUnionInviteButton->SetDisable( TRUE );
				mUnionCreateButton->SetDisable( TRUE );
				mUnionDisbandButton->SetDisable( TRUE );
				mLeaveUnionButton->SetDisable( TRUE );
				mKickUnionButton->SetDisable( TRUE );
				mDeclareWarButton->SetDisable( TRUE );
				mSuggestStopWarButton->SetDisable( TRUE );
				mSurrenderButton->SetDisable( TRUE );
				//mManageApprenticeButton->SetDisable( TRUE );
				mRetireButton->SetDisable( TRUE );
				mAddTimeButton->SetDisable( TRUE );

				mUnionInviteButton->SetImageRGB( unableColor );
				mUnionCreateButton->SetImageRGB( unableColor );
				mUnionDisbandButton->SetImageRGB( unableColor );
				mLeaveUnionButton->SetImageRGB( unableColor );
				mKickUnionButton->SetImageRGB( unableColor );
				mDeclareWarButton->SetImageRGB( unableColor );
				mSuggestStopWarButton->SetImageRGB( unableColor );
				mSurrenderButton->SetImageRGB( unableColor );
				//mManageApprenticeButton->SetImageRGB( unableColor );
				mRetireButton->SetImageRGB( unableColor );
				mAddTimeButton->SetImageRGB( unableColor );

				break;
			}
		case GUILD_SENIOR:
		case GUILD_JUNIOR:
			{
				mKickMemberButton->SetDisable( TRUE );
				mRankMemberButton->SetDisable( TRUE );
				mNoticeButton->SetDisable( TRUE );
				mUnionInviteButton->SetDisable( TRUE );
				mUnionCreateButton->SetDisable( TRUE );
				mUnionDisbandButton->SetDisable( TRUE );
				mLeaveUnionButton->SetDisable( TRUE );
				mKickUnionButton->SetDisable( TRUE );
				mDeclareWarButton->SetDisable( TRUE );
				mSuggestStopWarButton->SetDisable( TRUE );
				mSurrenderButton->SetDisable( TRUE );
				//mManageApprenticeButton->SetDisable( TRUE );
				mRetireButton->SetDisable( TRUE );
				mWarehouseRankButton->SetDisable( TRUE );
				mAddTimeButton->SetDisable( TRUE );

				mKickMemberButton->SetImageRGB( unableColor );
				mRankMemberButton->SetImageRGB( unableColor );
				mNoticeButton->SetImageRGB( unableColor );
				mUnionInviteButton->SetImageRGB( unableColor );
				mUnionCreateButton->SetImageRGB( unableColor );
				mUnionDisbandButton->SetImageRGB( unableColor );
				mLeaveUnionButton->SetImageRGB( unableColor );
				mKickUnionButton->SetImageRGB( unableColor );
				mDeclareWarButton->SetImageRGB( unableColor );
				mSuggestStopWarButton->SetImageRGB( unableColor );
				mSurrenderButton->SetImageRGB( unableColor );
				//mManageApprenticeButton->SetImageRGB( unableColor );
				mRetireButton->SetImageRGB( unableColor );
				mWarehouseRankButton->SetImageRGB( unableColor );
				mAddTimeButton->SetImageRGB( unableColor );

				break;
			}
		case GUILD_MEMBER:
		case GUILD_STUDENT:
			{
				mNameMemberButton->SetDisable( TRUE );
				mInviteMemberButton->SetDisable( TRUE );
				mKickMemberButton->SetDisable( TRUE );
				mRankMemberButton->SetDisable( TRUE );
				mNoticeButton->SetDisable( TRUE );
				mUnionInviteButton->SetDisable( TRUE );
				mUnionCreateButton->SetDisable( TRUE );
				mUnionDisbandButton->SetDisable( TRUE );
				mLeaveUnionButton->SetDisable( TRUE );
				mKickUnionButton->SetDisable( TRUE );
				mDeclareWarButton->SetDisable( TRUE );
				mSuggestStopWarButton->SetDisable( TRUE );
				mSurrenderButton->SetDisable( TRUE );
				//mManageApprenticeButton->SetDisable( TRUE );
				mRetireButton->SetDisable( TRUE );
				mWarehouseRankButton->SetDisable( TRUE );
				mAddTimeButton->SetDisable( TRUE );

				mNameMemberButton->SetImageRGB( unableColor );
				mInviteMemberButton->SetImageRGB( unableColor );
				mKickMemberButton->SetImageRGB( unableColor );
				mRankMemberButton->SetImageRGB( unableColor );
				mNoticeButton->SetImageRGB( unableColor );
				mUnionInviteButton->SetImageRGB( unableColor );
				mUnionCreateButton->SetImageRGB( unableColor );
				mUnionDisbandButton->SetImageRGB( unableColor );
				mLeaveUnionButton->SetImageRGB( unableColor );
				mKickUnionButton->SetImageRGB( unableColor );
				mDeclareWarButton->SetImageRGB( unableColor );
				mSuggestStopWarButton->SetImageRGB( unableColor );
				mSurrenderButton->SetImageRGB( unableColor );
				//mManageApprenticeButton->SetImageRGB( unableColor );
				mRetireButton->SetImageRGB( unableColor );
				mWarehouseRankButton->SetImageRGB( unableColor );
				mAddTimeButton->SetImageRGB( unableColor );

				break;
			}
		default:
			{
				ASSERT( 0 && "not defined rank detected" );

				mLeaveMemberButton->SetDisable( TRUE );
				mNameMemberButton->SetDisable( TRUE );
				mInviteMemberButton->SetDisable( TRUE );
				mKickMemberButton->SetDisable( TRUE );
				mRankMemberButton->SetDisable( TRUE );
				mNoticeButton->SetDisable( TRUE );
				mUnionInviteButton->SetDisable( TRUE );
				mUnionCreateButton->SetDisable( TRUE );
				mUnionDisbandButton->SetDisable( TRUE );
				mLeaveUnionButton->SetDisable( TRUE );
				mKickUnionButton->SetDisable( TRUE );
				mDeclareWarButton->SetDisable( TRUE );
				mSuggestStopWarButton->SetDisable( TRUE );
				mSurrenderButton->SetDisable( TRUE );
				//mManageApprenticeButton->SetDisable( TRUE );
				mRetireButton->SetDisable( TRUE );
				mWarehouseRankButton->SetDisable( TRUE );
				mAddTimeButton->SetDisable( TRUE );

				mLeaveMemberButton->SetImageRGB( unableColor );
				mNameMemberButton->SetImageRGB( unableColor );
				mInviteMemberButton->SetImageRGB( unableColor );
				mKickMemberButton->SetImageRGB( unableColor );
				mRankMemberButton->SetImageRGB( unableColor );
				mNoticeButton->SetImageRGB( unableColor );
				mUnionInviteButton->SetImageRGB( unableColor );
				mUnionCreateButton->SetImageRGB( unableColor );
				mUnionDisbandButton->SetImageRGB( unableColor );
				mLeaveUnionButton->SetImageRGB( unableColor );
				mKickUnionButton->SetImageRGB( unableColor );
				mDeclareWarButton->SetImageRGB( unableColor );
				mSuggestStopWarButton->SetImageRGB( unableColor );
				mSurrenderButton->SetImageRGB( unableColor );
				//mManageApprenticeButton->SetImageRGB( unableColor );
				mRetireButton->SetImageRGB( unableColor );
				mWarehouseRankButton->SetImageRGB( unableColor );
				mAddTimeButton->SetDisable( unableColor );

				break;
			}
		}
	}
	else
	{
		// 탈퇴창을 닫자...
		{
			cWindow* window = WINDOWMGR->GetWindowForID( MBI_GUILD_SECEDE );

			if( window )
			{
				WINDOWMGR->AddListDestroyWindow( window );
			}
		}

		// 추방창을 닫자...
		{
			cWindow* window = WINDOWMGR->GetWindowForID( MBI_GUILD_BAN );

			if( window )
			{
				WINDOWMGR->AddListDestroyWindow( window );
			}
		}

		// 길드 직위창을 닫자...
		{
			cWindow* window = WINDOWMGR->GetWindowForID( GDR_RANKDLG );
			ASSERT( window );

			window->SetActive( FALSE );
		}

		// 별명짓기 창을 닫자
		{
			cWindow* window = WINDOWMGR->GetWindowForID( GD_NICKNAMEDLG );
			ASSERT( window );

			window->SetActive( FALSE );
		}
	}	
}


void CGuildDialog::SetUnionName( const char* text )
{
	mUnionName->SetStaticText( text );
}


const char* CGuildDialog::GetUnionName() const
{
	return mUnionName->GetStaticText();
}


void CGuildDialog::SetScore( DWORD score )
{
	mGuildScore->SetStaticValue( score );
}


void CGuildDialog::SortByRank()//Add 060803 by wonju
{
	if( mSortRankFlag )
	{
		//std::sort( mMemberList.begin(), mMemberList.end(), SortRankByHighOrder() );
		mMemberList.sort( SortRankByHighOrder() );
	}
	else
	{
		//std::sort( mMemberList.begin(), mMemberList.end(), SortRankByLowOrder() );
		mMemberList.sort( SortRankByLowOrder() );
	}

	mSortRankFlag =! mSortRankFlag;

	RefreshMemeber();
}

void CGuildDialog::SortByLevel()//Add 060803 by wonju
{
	if( mSortLevelFlag )
	{
		//std::sort( mMemberList.begin(), mMemberList.end(), SortLevelByHighOrder() );
		mMemberList.sort( SortLevelByHighOrder() );
	}
	else
	{
		//std::sort( mMemberList.begin(), mMemberList.end(), SortLevelByLowOrder() );
		mMemberList.sort( SortLevelByLowOrder() );
	}

	mSortLevelFlag = ! mSortLevelFlag;

	RefreshMemeber();
}


void CGuildDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	switch( id )
	{
	// 길드 정보 탭 버튼
	case GD_MEMBERINFOPUSHUPBTN:
		{
			cDialog* dialog = ( cDialog* )GetTabSheet( 0 );
			ASSERT( dialog );

			dialog->SetActiveRecursive( TRUE );
			break;
		}
	// 길드 기능 탭 버튼
	case GD_GUILFUNCPUSHUPBTN:
		{
			cDialog* dialog = ( cDialog* )GetTabSheet( 1 );
			ASSERT( dialog );

			dialog->SetActiveRecursive( TRUE );
			break;
		}
	// 가입 권유
	case GD_INVITEMEMBER:
		{
			CPlayer* player = ( CPlayer* )OBJECTMGR->GetSelectedObject();

			if( !	player ||
					player->GetObjectKind() != eObjectKind_Player )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 655 ) );
				break;
			}
			else if( player->GetGuildIdx() )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(547));
				break;
			}
			else
			{
				const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting( GetLevel() );

				if( ! setting )
				{
					ASSERT( 0 );
					return;
				}

				CGuildJoinDialog* dialog = ( CGuildJoinDialog* )WINDOWMGR->GetWindowForID( JOIN_OPTIONDLG );
				ASSERT( dialog );

				dialog->SetEnableMember(
					GetMemberSize() - GetStudentSize() < setting->mMemberSize );

				dialog->SetEnableStudent(
					player->GetLevel()			< GUILD_STUDENT_MAX_LEVEL	&&
					GetStudentSize()	< setting->mApprenticeSize );

				dialog->SetActive( TRUE );
			}

			break;
		}
	// 080417 LUJ, 길드 버프
	case GD_PLUSTIME:
		{			
			cSkillTrainingDlg* dialog = ( cSkillTrainingDlg* )WINDOWMGR->GetWindowForID( SKILL_TRAINING );

			if( ! dialog )
			{
				break;
			}			
			else if(	dialog->IsActive() &&
						dialog->IsGuildMode() )
			{
				dialog->SetActive( FALSE );
			}
			else
			{
				dialog->OpenGuildSkill();
			}
		}
		break;
	// 동맹 관계 표시
	case GD_RELATIONBTN:
		{
			// send event to CGuildWarInfoDlg
			cDialog* dialog = WINDOWMGR->GetWindowForID( GW_INFODLG );
			ASSERT( dialog );
			
			dialog->SetActive( TRUE );
			break;
		}
	// 공지
	case GD_NOTICE:
		{
			// send event to CGuildNoticeDlg
			cDialog* dialog = WINDOWMGR->GetWindowForID( GUILDNOTICE_DLG );
			ASSERT( dialog );

			dialog->SetActive( TRUE );
			break;
		}
	// 회원 목록을 직위로 정렬
	case GD_POSITION:
		{
			SortByRank();
			break;
		}
	// 회원 목록을 레벨로 정렬
	case GD_RANK:
		{
			SortByLevel();
			break;
		}
		// 회원 추방
	case GD_BANMEMBER:
		{
			const BYTE rank = HERO->GetGuildMemberRank();
			if( rank != GUILD_VICEMASTER &&
				rank != GUILD_MASTER )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 323 ) );
				break;
			}
			
			// 대상 검사
			{
				const GUILDMEMBERINFO* member = GetSelectedMember();

				if( ! member		||
					! member->MemberIdx )
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(545) );
					break;
				}
				else if( member->MemberIdx == HEROID )
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1138 ) );
					break;
				}
				else if( member->Rank	== GUILD_MASTER )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 323 ) );
					break;
				}
				
				// 메시지 박스 표시
				{
					char line[ MAX_TEXT_SIZE ];
					
					sprintf( line, CHATMGR->GetChatMsg( 546 ), member->MemberName );

					WINDOWMGR->MsgBox( MBI_GUILD_BAN, MBT_YESNO, line );

					//GUILDMGR->SetActiveFunc(TRUE);

					mKickMemberIndex = member->MemberIdx;
				}
			}

			// 080417 LUJ, 메시지 박스가 표시된 후 리스트의 선택 항목이 바뀌는 것을 방지하기 위함. 메시지 창 처리 후 반드시 활성화시켜야함
			SetDisable( TRUE );
			break;
		}
	// 호칭 부여
	case GD_GIVEMEMBERNICK:
		{	
			cDialog* dialog = WINDOWMGR->GetWindowForID( GD_NICKNAMEDLG );
			ASSERT( dialog );

			dialog->SetActive( TRUE );
			break;
		}
	// 직위 부여
	case GD_GIVEMEMBERRANK:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( GDR_RANKDLG );
			ASSERT( dialog );

			dialog->SetActive( TRUE );			
			break;
		}
	// 길드를 자퇴
	case GD_SECEDE:
		{
			if(! HERO->GetGuildIdx() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 578 ) );
			}
			else
			{
				const DWORD rank = HERO->GetGuildMemberRank();

				if( GUILD_STUDENT	 <= rank &&
					GUILD_VICEMASTER >= rank )
				{
					WINDOWMGR->MsgBox( MBI_GUILD_SECEDE, MBT_YESNO, CHATMGR->GetChatMsg( 467 ) );
				}
			}
		}
		break;
	// 동맹에 초대
	case GD_INVITEUNION:
		{
			if( ! GUILDUNION->IsMaster( HERO ) )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 372 ) );
				break;
			}

			CPlayer* player = ( CPlayer* )OBJECTMGR->GetSelectedObject();

			if( !	player	||
					player->GetObjectKind() != eObjectKind_Player )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 655 ) );
			}
			else if( ! player->GetGuildIdx() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1023 ) );
			}
			else if( GUILDWARMGR->IsEnemy( player->GetGuildName() ) )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1029 ) );
			}
			else if( GUILDUNION->CheckGuildUnionCondition( 2 ) )
			{
				CHero* hero = OBJECTMGR->GetHero();
				ASSERT( hero );

				MSG_DWORD2 message;
				message.Category	= MP_GUILD_UNION;
				message.Protocol	= MP_GUILD_UNION_INVITE;
				message.dwObjectID	= hero->GetID();
				message.dwData1		= player->GetID();
				message.dwData2		= hero->GetGuildUnionIdx();

				NETWORK->Send( &message, sizeof(message) );
			}

			break;
		}		
	// 동맹 자퇴
	case GD_SECEDEUNION:
		{
			CHero* hero = OBJECTMGR->GetHero();
			ASSERT( hero );

			//if( ! GUILDUNION->IsGuildUnionMaster( hero ) )
			//{
			//	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 372 ) );
			//}
			//else 
			if( GUILDUNION->CheckGuildUnionCondition( 4 ) )
			{
				WINDOWMGR->MsgBox( MBI_UNION_LEAVE, MBT_YESNO, CHATMGR->GetChatMsg( 1139 ) );
			}

			break;
		}
	// 동맹 중 일원을 강퇴
	case GD_REMOVEUNION:
		{
			if( GUILDUNION->IsMaster( HERO ) )
			{
				GAMEIN->GetGFWarInfoDlg()->ShowGuildUnion();
			}
			else
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 372 ) );
			}
			
			break;
		}
	// 선전 포고
	case GD_DECLARE:
		{
			CHero* hero = OBJECTMGR->GetHero();
			ASSERT( hero );

			if(		hero->GetGuildUnionIdx() &&
				!	GUILDUNION->IsMaster( hero ) )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1140 ) );
				break;
			}
			// 길드 레벨, 회원 수 체크
			else if( 5 > mMemberList.size() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 855 ) );
				break;
			}

			//GAMEIN->GetGFWarDeclareDlg()->SetActive( TRUE );
			cDialog* dialog = WINDOWMGR->GetWindowForID( GFW_DECLAREDLG );
			ASSERT( dialog );

			dialog->SetActive( TRUE );
			break;
		}
	// 종전 제안
	case GD_SUGGEST:
		{
			if( GUILDWARMGR->IsValid( 1 ) )
			{
				GAMEIN->GetGFWarInfoDlg()->ShowSuggest();
			}

			break;
		}
	// 항복
	case GD_SURRENDER:
		{
			if( GUILDWARMGR->IsValid( 2 ) )
			{
				GAMEIN->GetGFWarInfoDlg()->ShowSurrend();
			}

			break;
		}
	// 권한 이양
	case GD_RETIRE_MASTER:
		{
			if( GUILD_MASTER != HERO->GetGuildMemberRank() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 579 ) );
				break;
			}

			// 창고 창이 열린 상태면 안됨. 권한 이양하면서 창고를 열 수 있는 권한이 없어질 수 있기 때문임
			{
				cDialog* dialog = WINDOWMGR->GetWindowForID( GDW_WAREHOUSEDLG );
				ASSERT( dialog );

				if( dialog->IsActive() )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1088 ) );
					break;
				}
			}
			
			if( 1 == GetMemberSize() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1089 ) );
				break;
			}
			// 길드전 중인가
			else if( GUILDWARMGR->GetEnemySize() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1090 ) );
				break;
			}
			
			const DWORD guildUnionIndex = HERO->GetGuildUnionIdx();

			// 연합에 가입된 상태이면 이양할 수 없음
			if( guildUnionIndex )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1091 ) );
                break;
			}

			const GUILDMEMBERINFO* member = GetSelectedMember();

			if( ! member		||
				! member->MemberIdx )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 545 ) );
				break;
			}
			else if( member->Rank	== GUILD_MASTER )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 607 ) );
				break;
			}
			else if( member->Rank	== GUILD_STUDENT )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 606 ) );
				break;
			}
			else if( mLoginPlayerSet.end() == mLoginPlayerSet.find( member->MemberIdx ) )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 93 ), member->MemberName );
				break;
			}

			{
				char text[ MAX_PATH ];
				sprintf( text, CHATMGR->GetChatMsg( 987 ),  member->MemberName );

				WINDOWMGR->MsgBox( MBI_GUILD_RETIRE_NOTIFY, MBT_YESNO, text );
			}

			// 080417 LUJ, 메시지 박스가 표시된 후 리스트의 선택 항목이 바뀌는 것을 방지하기 위함. 메시지 창 처리 후 반드시 활성화시켜야함
			SetDisable( TRUE );
			break;
		}
	// 동맹 창설
	case GD_CREATE_UNION:
		{
			// 주의: 마스터가 아닌 자는 이 버튼에 접근해서는 안됨
			{
				// send event to CGuildUnionCreateDialog
				cDialog* dialog = WINDOWMGR->GetWindowForID( GDU_CREATEDLG );
				ASSERT( dialog );

				dialog->SetActive( TRUE );			
			}
			
			break;
		}
	case GD_DISBAND_UNION:
		{
			// 주의: 마스터가 아닌 자는 이 버튼에 접근해서는 안됨

			if( 1 < GUILDUNION->GetSize() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1141 ) );
			}
			else if( GUILDUNION->CheckGuildUnionCondition( 1 ) )
			{
				WINDOWMGR->MsgBox( MBI_UNION_DESTROY, MBT_YESNO, CHATMGR->GetChatMsg( 539 ) );
			}
			else
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 372 ) );
			}
			
			break;
		}
	case GD_WAREHOUSE_RANK:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( GDWHR_RANKDLG );
			ASSERT( dialog );

			dialog->SetActive( TRUE );
			break;
		}

	// 091208 ShinJS --- 분할된 길드원 정보 List 를 클릭한 경우
	case GD_MEMBERNAMELIST:
		{
			switch( event )
			{
			case WE_ROWCLICK:
				{
					int nClickIdx = mMemberNameList->GetClickedRowIdx();
					if( mMemberRankList->GetClickedRowIdx() != nClickIdx )
						mMemberRankList->SetCurSelectedRowIdxWithoutRowMove( nClickIdx );

					if( mMemberLvList->GetClickedRowIdx() != nClickIdx )
						mMemberLvList->SetCurSelectedRowIdxWithoutRowMove( nClickIdx );
				}
				break;
			case WE_TOPROWCHANGE:
				{
					int nTopListItemIdx = mMemberNameList->GetTopListItemIdx();
					if( mMemberRankList->GetTopListItemIdx() != nTopListItemIdx )
						mMemberRankList->SetTopListItemIdx( nTopListItemIdx );

					if( mMemberLvList->GetTopListItemIdx() != nTopListItemIdx )
						mMemberLvList->SetTopListItemIdx( nTopListItemIdx );
				}
				break;
			}
		}
		break;
	}
}


void CGuildDialog::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();	
	cTabDialog::RenderTabComponent();
}


const GUILDMEMBERINFO* CGuildDialog::GetSelectedMember() const
{
	const int row = mMemberNameList->GetClickedRowIdx();

	if( 0 > row							||
		mMemberList.size() <= DWORD( row ) )
	{
		return 0;
	}

	MemberList::const_iterator it = mMemberList.begin();

	std::advance( it, row );

	const GUILDMEMBERINFO& member = *it;

	return &member;
}

void CGuildDialog::SetData( const MSG_GUILD_DATA& message )
{
	mGuildName->SetStaticText( message.mName );
	mGuildLevel->SetStaticValue( message.mLevel );
	mGuildScore->SetStaticValue( message.mScore );

	// 레벨 세팅
	{
		CGuildLevelUpDialog* dialog = ( CGuildLevelUpDialog* )WINDOWMGR->GetWindowForID( GD_LEVELUPDLG );

		dialog->SetLevel( message.mLevel );
	}

	// 길드 공지도 세팅
	{
		CGuildNoticeDlg*	dialog	= ( CGuildNoticeDlg* )WINDOWMGR->GetWindowForID( GUILDNOTICE_DLG );
		cTextArea*			text	= ( cTextArea* )dialog->GetWindowForID( GNotice_TEXTREA );

		text->SetScriptText( message.mIntro );
	}

	// 회원 정보 세팅
	{
		// 초기화
		{
			mLoginPlayerSet.clear();
			mMemberList.clear();
		}

		// 데이터를 복사하자
		{
			mMemberSize->SetStaticValue( message.mMemberSize );

			for( DWORD i = 0; i < message.mMemberSize; ++i )
			{
				const GUILDMEMBERINFO& member = message.mMember[ i ];

				mMemberList.push_back( member );

				if( member.Rank == GUILD_MASTER )
				{
					mMasterName->SetStaticText( member.MemberName );

					mMasterIndex = member.MemberIdx;
				}

				if( member.bLogged )
				{
					mLoginPlayerSet.insert( member.MemberIdx );
				}
			}
		}	

		mMemberList.sort( SortRankByHighOrder() );

		RefreshMemeber();
	}
}

// 081031 LUJ, 맵 정보 추가
void CGuildDialog::LoginMember( DWORD playerIndex, MAPTYPE mapType )
{
	// 081031 LUJ, 로그인된 맵 정보 변경
	{
		GUILDMEMBERINFO* member = GetMember( playerIndex );

		if( member )
		{
			member->mMapType	= mapType;
			member->bLogged		= TRUE;
		}
	}

	mLoginPlayerSet.insert( playerIndex );
	
	mMemberList.sort( SortRankByHighOrder() );

	RefreshMemeber();
}


void CGuildDialog::LogoutMember( DWORD playerIndex )
{
	// 081031 LUJ, 로그인된 맵 정보 변경
	{
		GUILDMEMBERINFO* member = GetMember( playerIndex );

		if( member )
		{
			member->mMapType	= 0;
			member->bLogged		= FALSE;
		}
	}

	mLoginPlayerSet.erase( playerIndex );
	
	mMemberList.sort( SortRankByHighOrder() );

	RefreshMemeber();
}


void CGuildDialog::RefreshMemeber()
{
	mMemberNameList->RemoveAll();
	mMemberRankList->RemoveAll();
	mMemberLvList->RemoveAll();

	for(
		MemberList::const_iterator it = mMemberList.begin();
		mMemberList.end() != it;
		++it )
	{
		const GUILDMEMBERINFO& member = *it;

		// 08225 LUJ, 회원의 직업을 툴팁으로 설정한다
		cListDialog::ToolTipTextList toolTipList;
		const WORD index = (1 < member.mJobGrade ? member.mJob[member.mJobGrade - 1] : 1);
		const WORD job = (member.mJob[0] * 1000 ) + ((member.mRace + 1) * 100) + (member.mJobGrade * 10) + index;
		char line[MAX_PATH] = {0};
		sprintf(
			line,
			member.bLogged ? "%s, %s" : "%s",
			RESRCMGR->GetMsg(RESRCMGR->GetClassNameNum(job)),
			GetMapName(member.mMapType));
		toolTipList.push_back(line);

		DWORD dwColor = ( mLoginPlayerSet.end() == mLoginPlayerSet.find( member.MemberIdx ) ? RGBA_MAKE( 124, 123, 123, 0 ) : RGBA_MAKE( 0, 48, 255, 0 ) );
		const char* rank = GUILDMGR->GetRankName( BYTE( member.Rank ) );
		char txtLv[5] = {0,};
		itoa( member.Memberlvl, txtLv, 10 );
		mMemberNameList->AddItem( member.MemberName, dwColor, toolTipList );
		mMemberRankList->AddItem( rank, dwColor, toolTipList );
		mMemberLvList->AddItem( txtLv, dwColor, toolTipList );
	}

	mMemberNameList->SetCurSelectedRowIdx( -1 );
	mMemberRankList->SetCurSelectedRowIdx( -1 );
	mMemberLvList->SetCurSelectedRowIdx( -1 );
}


void CGuildDialog::AddMember( const GUILDMEMBERINFO& member )
{
	mMemberSize->SetStaticValue( mMemberSize->GetStaticValue() + 1 );

	mLoginPlayerSet.insert( member.MemberIdx );

	mMemberList.push_back( member );

	RefreshMemeber();
}


void CGuildDialog::SetLevel( DWORD level )
{
	mGuildLevel->SetStaticValue( level );
}


DWORD CGuildDialog::GetLevel() const
{
	return mGuildLevel->GetStaticValue();
}


void CGuildDialog::RemoveMember( DWORD playerIndex )
{
	mMemberSize->SetStaticValue( mMemberSize->GetStaticValue() - 1 );

	for(	MemberList::iterator it = mMemberList.begin();
			mMemberList.end() != it;
			++it )
	{
		const GUILDMEMBERINFO& member = *it;

		if( member.MemberIdx == playerIndex )
		{
			mLoginPlayerSet.erase( playerIndex );
			mMemberList.erase( it );

			break;
		}
	}

	RefreshMemeber();
}


const char* CGuildDialog::GetMasterName() const
{
	return mMasterName->GetStaticText();
}


const char* CGuildDialog::GetGuildName() const
{
	return mGuildName->GetStaticText();
}


DWORD CGuildDialog::GetScore() const
{
	return mGuildScore->GetStaticValue();
}


void CGuildDialog::SetMemberLevel( DWORD playerIndex, DWORD level )
{
	for(	MemberList::iterator it = mMemberList.begin();
			mMemberList.end() != it;
			++it )
	{
		GUILDMEMBERINFO& member = *it;

		if( member.MemberIdx == playerIndex )
		{
			member.Memberlvl = LEVELTYPE( level );

			break;
		}
	}

	RefreshMemeber();
}


DWORD CGuildDialog::GetMemberSize() const
{
	return mMemberList.size();
}


DWORD CGuildDialog::GetStudentSize() const
{
	int count = 0;

	for(	MemberList::const_iterator it = mMemberList.begin();
			mMemberList.end() != it;
			++it )
	{
		const GUILDMEMBERINFO& member = *it;

		if( GUILD_STUDENT == member.Rank )
		{
			++count;
		}
	}

	return count;
}


void CGuildDialog::SetRank( DWORD playerIndex, BYTE rank )
{
	for(	MemberList::iterator it = mMemberList.begin();
			mMemberList.end() != it;
			++it )
	{
		GUILDMEMBERINFO& member = *it;

		if( member.MemberIdx == playerIndex )
		{
			if( GUILD_MASTER == ( member.Rank = rank ) )
			{
				mMasterName->SetStaticText( member.MemberName );

				mMasterIndex = member.MemberIdx;
			}

			break;
		}
	}

	RefreshMemeber();
}


DWORD CGuildDialog::GetMasterIndex() const
{
	return mMasterIndex;
}


DWORD CGuildDialog::GetKickMemberIndex() const
{
	return mKickMemberIndex;
}


// 080225 LUJ, 특정 플레이어에 대한 길드 회원 정보를 반환한다
GUILDMEMBERINFO* CGuildDialog::GetMember( DWORD playerIndex )
{
	for(
		MemberList::iterator it = mMemberList.begin();
		mMemberList.end() != it;
		++it )
	{
		GUILDMEMBERINFO& member = *it;

		if( member.MemberIdx == playerIndex )
		{
			return &member;
		}
	}

	return 0;
}

// 080225 LUJ, 특정 플레이어에 대한 길드 회원 정보를 반환한다
GUILDMEMBERINFO* CGuildDialog::GetMember( const TCHAR* playeName )
{
	if( ! playeName )
	{
		return 0;
	}

	for(
		MemberList::iterator it = mMemberList.begin();
		mMemberList.end() != it;
		++it )
	{
		GUILDMEMBERINFO& member = *it;

		if( ! _tcscmp( member.MemberName, playeName ) )
		{
			return &member;
		}
	}

	return 0;
}


// 080225 LUJ, 정보를 설정한다
void CGuildDialog::SetMember( const GUILDMEMBERINFO& member )
{
	GUILDMEMBERINFO* storedMember = GetMember( member.MemberIdx );

	if( ! storedMember )
	{
		AddMember( member );
		return;
	}

	*storedMember = member;

	RefreshMemeber();
}
