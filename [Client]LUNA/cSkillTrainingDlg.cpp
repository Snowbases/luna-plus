#include "stdafx.h"
#include ".\cskilltrainingdlg.h"
#include ".\interface\clistdialog.h"
#include ".\interface\cstatic.h"
#include "WindowIDEnum.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[cc]skill/client/info/activeskillinfo.h"
#include "ObjectManager.h"
#include "Hero.h"
#include "GameIn.h"
#include "cskilltreemanager.h"
#include "cSkillBase.h"
#include "itemmanager.h"
#include "./Interface/cScriptManager.h"
#include "ChatManager.h"
#include "cMsgBox.h"
#include "../Interface/cWindowManager.h"

// 080310 LUJ, 스킬 요구 레벨 표시하기 위해 포함
#include "cResourceManager.h"
// 080417 LUJ, 길드 스킬 표시위해 포함
#include "GuildManager.h"


cSkillTrainingDlg::cSkillTrainingDlg(void) :
mGuildMode( FALSE )
{
	m_Index = 0;
}

cSkillTrainingDlg::~cSkillTrainingDlg(void)
{
}

void cSkillTrainingDlg::Linking()
{
	m_SkillListDlg = ( cListDialog* )GetWindowForID( SKILL_TRAINLIST );
	m_SkillListDlg->SetShowSelect( TRUE );
	m_SkillListDlg->SetAutoScroll( FALSE );

	m_SkillTipListDlg = ( cListDialog* )GetWindowForID( SKILL_TRAINTIPLIST );
	m_SkillTipListDlg->SetShowSelect( FALSE );
	m_SkillTipListDlg->SetAutoScroll( FALSE );;

	m_SkillPoint = ( cStatic* )GetWindowForID( SKILL_TRAINPOINT );
	m_Gold = ( cStatic* )GetWindowForID( SKILL_TRAINGOLD );

	m_Icon = ( cStatic* )GetWindowForID( SKILL_TRAIN_ICON );
	m_NeedSp = ( cStatic* )GetWindowForID( SKILL_TRAIN_SP );
	m_NeedGold = ( cStatic* )GetWindowForID( SKILL_TRAIN_GOLD );

	m_AddDamageOptTooltip = ( cStatic* )GetWindowForID( SKILL_TRAIN_ADDDAMAGE_OPT_TOOLTIP_TEXT );
}

// 080417 LUJ, 길드 스킬 표시
void cSkillTrainingDlg::OpenGuildSkill()
{
	CHero* hero = OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}
	// 080417 LUJ, 길드 마스터만 창을 열 수 있다
	else if(	!	hero->GetGuildIdx()		||
					strcmp( hero->GetObjectName(), GUILDMGR->GetMasterName() ) )
	{
        CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg( 976 ) );
		return;
	}
	
	mGuildMode = TRUE;

	// 080417 LUJ, 상황에 맞게 부속 창을 표시한다
	{
		cWindow* windowSkillText	= GetWindowForID( SKILL_TRAIN_POINT_TEXT );
		cWindow* windowSkillImage	= GetWindowForID( SKILL_TRAIN_POINT_IMAGE );

		cWindow* windowGuildText	= GetWindowForID( SKILL_TRAIN_GUILD_POINT_TEXT );
		cWindow* windowGuildImage	= GetWindowForID( SKILL_TRAIN_GUILD_POINT_IMAGE );

		if( windowSkillText && windowSkillImage &&
			windowGuildText && windowGuildImage )
		{
			windowSkillText->SetActive( FALSE );
			windowSkillImage->SetActive( FALSE );

			windowGuildText->SetActive( TRUE );
			windowGuildImage->SetActive( TRUE );
		}

		cStatic* windowTitle = ( cStatic* )GetWindowForID( SKILL_TRAIN_TITLE );

		if( windowTitle )
		{
			static const std::string title( RESRCMGR->GetMsg( 924 ) );

			windowTitle->SetStaticText( title.c_str() );
		}
	}

	const LONG count = m_SkillListDlg->GetItemCount();
	int top = m_SkillListDlg->GetTopListItemIdx();
	m_SkillListDlg->RemoveAll();

	WORD pos = 0;

	typedef CGuildManager::SkillSettingMap SkillSettingMap;
	const SkillSettingMap* settingMap			= GUILDMGR->GetSkillSetting( GUILDMGR->GetLevel() );
	const SkillSettingMap* maxLevelSettingMap	= GUILDMGR->GetMaxLevelSkillSetting();

	// 080417 LUJ, 레벨에 대한 설정이 없을 경우 최고 레벨의 설정을 사용한다
	if( ! maxLevelSettingMap )
	{
		static const std::string textGuild( RESRCMGR->GetMsg( 131 ) );

		CHATMGR->AddMsg( CTC_GUILD_NOTICE, "[%s] %s", textGuild.c_str(), CHATMGR->GetChatMsg( 538 ) );
		return;
	}
	else if( ! settingMap )
	{
		settingMap = maxLevelSettingMap;
	}

	typedef std::map< DWORD, BYTE > SkillMap;
	SkillMap						skillMap;

	// 080417 LUJ, 레벨에 따라 같은 스킬이 분포할 수 있으므로, 스킬 별로 최대 레벨 값과 짝지어 따로 맵을 구성한다
	for(
		SkillSettingMap::const_iterator it = maxLevelSettingMap->begin();
		maxLevelSettingMap->end() != it;
		++it )
	{
		const DWORD							index	= it->first;
		const CGuildManager::SkillSetting&	setting = it->second;

        if( skillMap.end() == skillMap.find( index ) )
		{
			skillMap.insert( std::make_pair( index, setting.mLevel ) );
		}
		// 080417 LUJ, 같은 스킬 정보가 있을 경우 항상 큰 레벨 값을 넣는다
		else
		{
			BYTE& level = skillMap[ index ];

			level = max( level, setting.mLevel );
		}
	}
	
	for(
		SkillMap::const_iterator it = skillMap.begin();
		skillMap.end() != it;
		++it )
	{
		const DWORD	index = it->first;
		const BYTE	level = it->second;
		
		cSkillInfo* skillInfo = SKILLMGR->GetSkillInfo( index );
		
		if( !	skillInfo ||
				// 080417 LUJ, 스킬을 모두 배웠으면 표시할 필요가 없으므로, 최대 레벨에 도달했는지 검사
				level <= GUILDMGR->GetSkillLevel( index ) )
		{
			continue;
		}

		BOOL isCheck = FALSE;

		for( DWORD i = 0; i < pos; ++i )
		{
			if( m_SkillList[ i ].index == index )
			{
				isCheck = TRUE;
				break;
			}
		}

		if( isCheck )
		{
			continue;
		}			

		const char* kind	= 0;
		DWORD		color	= 0;

		switch( skillInfo->GetKind() )
		{
		case SKILLKIND_PHYSIC:
		case SKILLKIND_MAGIC:
		case SKILLKIND_ONOFF:
			{
				kind = CHATMGR->GetChatMsg( 175 );
				color = RGB_HALF( 184, 117, 30 );
				break;
			}
		case SKILLKIND_PASSIVE:
			{
				kind = CHATMGR->GetChatMsg( 176 );
				color = RGB_HALF( 73, 65, 150 );
				break;
			}
		}

		const DWORD skillLevel = GUILDMGR->GetSkillLevel( index );

		// 080417 LUJ, 보유 스킬보다 레벨이 하나 더 높은 정보를 가져오도록 한다
		cSkillInfo* pRealSkillInfo	= SKILLMGR->GetSkillInfo( skillInfo->GetIndex() + skillLevel );

		if( ! pRealSkillInfo )
		{
			continue;
		}
		// 080310 LUJ,	스킬 습득 가능 레벨이 아닌 경우 다른 색을 표시한다
		else
		{
			const SkillSettingMap::const_iterator it = settingMap->find( index );

			if( settingMap->end() == it )
			{
				color = ICONCLR_DISABLE;
			}
			else
			{
				const CGuildManager::SkillSetting& setting = it->second;

				if( setting.mLevel < skillLevel + 1 )
				{
					color = ICONCLR_DISABLE;
				}
			}
		}
		
		char buf[ MAX_PATH ] = { 0 };
		sprintf( buf, "[%s] %s %d %s",
			kind,
			CHATMGR->GetChatMsg( 177 ),
			GUILDMGR->GetSkillLevel( index ) + 1,
			pRealSkillInfo->GetName() );

		m_SkillListDlg->AddItem( buf, color );
		m_SkillList[ pos ].index	= index;
		m_SkillList[ pos++ ].level	= GUILDMGR->GetSkillLevel( index ) + 1;
	}

	if( count != m_SkillListDlg->GetItemCount() )
	{
		m_Index = 0;
		m_SkillListDlg->SetCurSelectedRowIdx( 0 );
		top = 0;
	}

	m_SkillListDlg->SetTopListItemIdx( top );

	SetSkillInfo();
	SetGold();
	
	// 080417 LUJ, 길드 점수 설정
	{
		char text[ MAX_PATH ] = { 0 };
		sprintf( text, "%d", GUILDMGR->GetScore() );

		m_SkillPoint->SetStaticText( text );
	}

	// 080417 LUJ, 위치 계산. 길드창 좌측이나 우측 중 여유 공간이 있는 쪽에 스킬창을 표시한다
	{
		cDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );

		if( dialog &&
			dialog->IsActive() )
		{
			const DISPLAY_INFO& screen	= GAMERESRCMNGR->m_GameDesc.dispInfo;

			// 080417 LUJ, 우측에 여유 공간이 있는 경우
			if( screen.dwWidth > dialog->GetAbsX() + dialog->GetWidth() + GetWidth() )
			{
				SetAbsXY(
					LONG( dialog->GetAbsX() + dialog->GetWidth() ),
					LONG( dialog->GetAbsY() ) );
			}
			// 080417 LUJ, 좌측에 여유 공간이 있는 경우
			else
			{
				SetAbsXY(
					LONG( dialog->GetAbsX() - GetWidth() ),
					LONG( dialog->GetAbsY() ) );
			}
		}
	}

	SetActive( TRUE );
}

void cSkillTrainingDlg::DisplayGuildSkill()
{
	cWindow* windowSkillText	= GetWindowForID( SKILL_TRAIN_POINT_TEXT );
	cWindow* windowSkillImage	= GetWindowForID( SKILL_TRAIN_POINT_IMAGE );

	cWindow* windowGuildText	= GetWindowForID( SKILL_TRAIN_GUILD_POINT_TEXT );
	cWindow* windowGuildImage	= GetWindowForID( SKILL_TRAIN_GUILD_POINT_IMAGE );

	if( windowSkillText && windowSkillImage &&
		windowGuildText && windowGuildImage )
	{
		windowSkillText->SetActive( TRUE );
		windowSkillImage->SetActive( TRUE );

		windowGuildText->SetActive( FALSE );
		windowGuildImage->SetActive( FALSE );
	}

	cStatic* windowTitle = ( cStatic* )GetWindowForID( SKILL_TRAIN_TITLE );

	if( windowTitle )
	{
		static const std::string title( RESRCMGR->GetMsg( 351 ) );

		windowTitle->SetStaticText( title.c_str() );
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : GetSkillLevelFromJobTree
//	DESC : 080618 LYW : HERO의 이전 직업 들 중에 해당하는 스킬이 있는지 확인하고, 있다면,
//		 : 그 스킬이 습득 가능한 최대 레벨을 반환하는 함수 추가.
//-------------------------------------------------------------------------------------------------
int cSkillTrainingDlg::GetSkillLevelFromJobTree(DWORD dwSkillIdx, WORD wCurJobLevel)
{
	// 리턴할 결과를 담을 변수.
	int nResult = -1 ;


	// 현재 직업 레벨을 체크한다.
	if(wCurJobLevel == 1) return nResult ;


	// 직업 인덱스 / 스킬 정보를 담을 변수 선언.
	WORD wJobIdx ;
	SKILL_TREE_INFO* pInfo ;

	DWORD dwIndex ;

	WORD wCount ;

	BYTE byJobType ;


	// 이전 직업들을 체크하며, 스킬이 습득 가능했었는지 확인한다.
	for( WORD CurGrade = wCurJobLevel-1; CurGrade > 0; CurGrade-- )
	{
		byJobType = 0 ;

		// 직업 계열의 유효성 체크.
		if( CurGrade == 1 ) byJobType = 1 ;
		else byJobType = HERO->GetCharacterTotalInfo()->Job[ CurGrade - 1 ] ;

		if(!byJobType) return nResult ;

		// 현제 레벨에 맞는 직업 인덱스를 받는다.
		wJobIdx = 0 ;											
		wJobIdx = ( HERO->GetCharacterTotalInfo()->Job[ 0 ] * 1000 ) +
				( ( HERO->GetCharacterTotalInfo()->Race + 1 ) * 100 ) + 
				( ( CurGrade ) * 10 ) + 
				byJobType ;

		// 직업에 맞는 스킬 트리 정보를 받는다.
		pInfo = NULL ;
		pInfo = SKILLMGR->GetSkillTreeInfo( wJobIdx ) ;

		if(!pInfo) continue ;

		// 직업에 따른 스킬 개수 만큼 for문을 돌며 스킬을 추가한다.
		for( wCount = 0 ; wCount < pInfo->Count ; wCount++ )
		{
			// 스킬 인덱스 받기.
			dwIndex = 0 ;
			dwIndex = ( ( DWORD )pInfo->SkillIndex[ wCount ] / 100 ) * 100 + 1 ;

			// 인자로 넘어온 스킬과 같은 스킬인지 체크한다.
			if(dwSkillIdx != dwIndex) continue ;

			if(pInfo->SkillIndex[ wCount ] < dwIndex) return nResult ;

			nResult = ((int)pInfo->SkillIndex[ wCount ] - dwIndex) + 1 ;
			return nResult ;
		}
	}

	// 결과 리턴.
	return nResult ;
}





//-------------------------------------------------------------------------------------------------
// NAME : AddSkillToSkillListDlg
// DESC : 080618 LYW : 현재 직업 차수와 직업 번호를 입력 하면, 그에 따른 스킬을 스킬리스트 
//		: 다이얼로그에 추가하는 함수를 추가한다.
//-------------------------------------------------------------------------------------------------
void cSkillTrainingDlg::AddSkillToSkillListDlg(BYTE byJobType, WORD wJobLevel, WORD* pPos)
{
	// 직업 계열의 유효성 체크.
	if( wJobLevel == 1 ) byJobType = 1 ;
	if(!byJobType) return ;


	// 현제 레벨에 맞는 직업 인덱스를 받는다.
	WORD wJobIdx = 0 ;											
	wJobIdx = ( HERO->GetCharacterTotalInfo()->Job[ 0 ] * 1000 ) +
			( ( HERO->GetCharacterTotalInfo()->Race + 1 ) * 100 ) + 
			( ( wJobLevel ) * 10 ) + 
			byJobType ;


	// 직업에 맞는 스킬 트리 정보를 받는다.
	SKILL_TREE_INFO* pInfo = SKILLMGR->GetSkillTreeInfo( wJobIdx ) ;
	if(!pInfo) return ;


	// 직업에 따른 스킬을 추가하기 위한 변수들을 선언한다.
	WORD  wCount ;												// for문을 위한 카운트 변수.
	WORD  wCount2 ;												//

	DWORD dwIndex ;												// 스킬 인덱스.
	DWORD dwLevel ;												// 스킬 레벨.
	DWORD dwCurLevel ;											// 현재 스킬 레벨.

	cSkillInfo* pSkillInfo ;									// 스킬 정보 포인터.
	cSkillBase* pSkillBase ;									// 스킬 기본 정보 포인터.
	cSkillInfo* pRealSkillInfo ;								// 리얼 스킬 포인터.

	BYTE byChecked = FALSE;
	char buf[MAX_PATH] = {0};
	char* kind = "";
	DWORD dwColor = 0;

	// 직업에 따른 스킬 개수 만큼 for문을 돌며 스킬을 추가한다.
	for( wCount = 0 ; wCount < pInfo->Count ; wCount++ )
	{
		// 스킬 인덱스 받기.
		dwIndex = 0 ;
		dwIndex = ( ( DWORD )pInfo->SkillIndex[ wCount ] / 100 ) * 100 + 1 ;

		// 스킬 레벨 받기.
		dwLevel = 0 ;
		dwLevel = pInfo->SkillIndex[ wCount ] % 100 ;

		// 스킬 정보 받기.
		pSkillInfo = NULL ;
		pSkillInfo = SKILLMGR->GetSkillInfo( dwIndex ) ;

		// 스킬 기본 정보 받기.
		pSkillBase = NULL ;
		pSkillBase = SKILLTREEMGR->GetSkill( dwIndex ) ;

		// 스킬 정보 체크.
		if( !pSkillInfo ) continue ;

		// 체크 한 스킬인지 확인한다.
		byChecked = FALSE ;
		for( wCount2 = 0 ; wCount2 < *pPos ; wCount2++ )
		{
			if( m_SkillList[ wCount2 ].index == dwIndex )
			{
				byChecked = TRUE ;
				break ;
			}
		}
		if( byChecked ) continue ;

		// 스킬 기본 정보가 유효한지 체크 후, 스킬 레벨을 세팅한다.
		if( pSkillBase )
		{
			dwCurLevel = pSkillBase->GetSkillBase()->mLearnedLevel;

			if( dwLevel > dwCurLevel )
			{
				// 한단계 높은 레벨로 고정시킨다.
				dwLevel = dwCurLevel + 1 ;
			}
			// 이도 저도 아닌경우에는, 이전 직업에서 습득 한 스킬인지 확인한다.
			else 
			{
				// 이전 직업 들 중에서, 현재 스킬과 같은 스킬의 습득이 가능한지 체크하고,
				// 레벨을 넘겨 받는다.
				int nResult = -1 ;
				nResult = GetSkillLevelFromJobTree(dwIndex, wJobLevel) ;

				if(nResult == -1) continue ;
				else
				{
					dwLevel = ((DWORD)nResult) ;
					
					if(dwLevel > dwCurLevel)
					{
						dwLevel = dwCurLevel + 1 ;
					}
					else continue ;
				}
			}
		}
		// 스킬 기본 정보가 유효하지 않으면, 아직 습득하지 않은 스킬이므로, 레벨을 1로 세팅한다.
		else
		{
			dwLevel = 1 ;
		}

		// 스킬 종류에 따라, 스킬명과 레벨, 글자 색상을 세팅한다.
		switch( pSkillInfo->GetKind() )
		{
		case SKILLKIND_PHYSIC :										// 물리 스킬.
		case SKILLKIND_MAGIC :										// 마법 스킬.
		case SKILLKIND_ONOFF :										// ON/OFF형 스킬.
			{
				// [액티브] 문자열을 받는다.
				kind = CHATMGR->GetChatMsg( 175 ) ;
				// 080507 LUJ, 어떤 인터페이스 테마가 적용되어도 잘 보이도록 변경
				dwColor = RGB_HALF( 255, 255, 255 ) ;
			}
			break ;

		case SKILLKIND_PASSIVE :									// 패시브 스킬.
			{
				// [패시브] 문자열을 받는다.
				kind = CHATMGR->GetChatMsg( 176 ) ;
				// 패시브 색상을 세팅한다.
				dwColor = RGB_HALF( 73, 65, 150 ) ;
			}
			break ;

		//default : dwColor = 0 ; break ;
		}
		
		// 레벨에 맞는 진짜 스킬 정보를 받는다.
		pRealSkillInfo = NULL ;
		pRealSkillInfo = SKILLMGR->GetSkillInfo( pSkillInfo->GetIndex() - 1 + dwLevel ) ;

		// 스킬 정보 체크.
		if( ! pRealSkillInfo ) continue ;

		// 080310 LUJ,	스킬 습득 가능 레벨이 아닌 경우 다른 색을 표시한다
		//
		//				스킬 트리로는 필요한 정보를 얻을 수 없으므로, 표시할 스킬을
		//				최고 몇 레벨까지 배웠는지 조회하여 표시 여부를 결정한다.
		{
			// 080310 LUJ,	위험한 캐스팅이지만, 배우는 스킬은 모두 cActiveSkillInfo형태로 정적 캐스팅되어
			//				문제없이 사용되고 있으므로, 여기서도 사용한다
			const ACTIVE_SKILL_INFO* info = ( ( cActiveSkillInfo* )pRealSkillInfo )->GetSkillInfo() ;

			if( !info || info->RequiredPlayerLevel > HERO->GetLevel() )
			{
				dwColor = ICONCLR_DISABLE ;
			}
		}

		// 스킬 종류 / 스킬명 / 레벨 등의 정보를 버퍼에 세팅한다.
		// 레벨에 맞는 진짜 스킬레벨을 출력한다.
		sprintf( buf, "[%s] %s %d %s", kind, CHATMGR->GetChatMsg( 177 ), pRealSkillInfo->GetLevel(), pRealSkillInfo->GetName() ) ;

		// 스킬리스트 다이얼로그에 추가한다.
		m_SkillListDlg->AddItem( buf, dwColor ) ;

		// 스킬 리스트에 인덱스와 레벨을 추가한다.
		m_SkillList[ *pPos ].index = dwIndex ;
		m_SkillList[ *pPos ].level = (BYTE)dwLevel ;

		*pPos = (*pPos + 1) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : OpenDialog
//	DESC : 080618 LYW : 스킬 습득 창의 출력 방식을 변경한다.
//		 : 20080613의 버그리스트 38번
//		 : 이전 직업의 스킬 습득이 현재 직업보다 높을 경우, 그 이상 습득할 수 없는 문제.
//		 : 습득 가능하도록 수정 함.
//-------------------------------------------------------------------------------------------------
void cSkillTrainingDlg::OpenDialog()
{
	// 스킬 리스트 다이얼로그 체크.
	if(!m_SkillListDlg) return ;


	// 길드 모드를 FALSE로 세팅.
	mGuildMode = FALSE;


	// 상황에 맞게 길드 부속창을 표시한다.
	DisplayGuildSkill() ;

	const LONG count = m_SkillListDlg->GetItemCount();
	const top = m_SkillListDlg->GetTopListItemIdx();
	const int clickedRowIndex = m_SkillListDlg->GetClickedRowIdx();

	// 스킬 리스트 다이얼로그를 비운다.
	m_SkillListDlg->RemoveAll() ;
	

	// 스킬 세팅을 위해 필요한 변수들을 선언한다.
	WORD JobGrade = HERO->GetCharacterTotalInfo()->JobGrade ;	// 현재 직업 차수 (1~6차)


	// 지금 껏 HERO가 거쳐온 직업을 FOR문을 돌며 스킬을 스킬 리스트에 추가한다.
	BYTE byJobType ;
	WORD wPos = 0 ;												// 위치 값을 담을 변수.
	for( WORD CurGrade = JobGrade; CurGrade > 0; CurGrade-- )
	{
		byJobType = 0 ;
		byJobType = HERO->GetCharacterTotalInfo()->Job[ CurGrade - 1 ] ;

		AddSkillToSkillListDlg(byJobType, CurGrade, &wPos) ;
	}

	// 091211 LUJ, 스킬 개수가 동일한 경우, 이전 선택했던 것을 다시 지정하도록 한다
	if(count == m_SkillListDlg->GetItemCount())
	{
		m_SkillListDlg->SetClickedRowIdx(clickedRowIndex);
		m_SkillListDlg->SetTopListItemIdx(top);
	}
	else
	{
		m_Index = 0 ;
		m_SkillListDlg->SetCurSelectedRowIdx(0);
		m_SkillListDlg->SetTopListItemIdx(0);
	}

	SetSkillInfo();
	SetGold();
	SetSkillPoint();
	SetActive(TRUE);
}

void cSkillTrainingDlg::OnActionEvent(LONG lId, void * p, DWORD we)
{
	if( we & WE_BTNCLICK )
	{
		switch(lId)
		{
		case SKILL_TRAINOK:
			{
				if( m_Index != -1 )
				{
					DWORD skillIndex		= m_SkillList[ m_Index ].index;
					BYTE skillNextLevel	= m_SkillList[ m_Index ].level;
					cSkillBase* pSkill			= SKILLTREEMGR->GetSkill( skillIndex );

					if( pSkill )
					{
						if( pSkill->GetSkillBase()->mLearnedLevel >= skillNextLevel )
						{
							return;
						}
					}

					DWORD idx = skillIndex - 1 + skillNextLevel;

					cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( idx );

					if( !pSkillInfo )
					{
						return;
					}

					// 080417 LUJ, 길드 스킬인 경우 획득 조건을 충족하는지 검사
					if( mGuildMode )
					{
						const ACTIVE_SKILL_INFO* info = ( ( cActiveSkillInfo* )pSkillInfo )->GetSkillInfo();

						if( ! info )
						{
							static const std::string textGuild( RESRCMGR->GetMsg( 131 ) );

							CHATMGR->AddMsg(
								CTC_GUILD_NOTICE,
								"[%s] %s",
								textGuild.c_str(),
								CHATMGR->GetChatMsg( 1444 ) );
							break;
						}
						else if(	info->TrainMoney > HERO->GetMoney()	||
									info->TrainPoint > GUILDMGR->GetScore() )
						{
							CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg( 605 ) );
							break;
						}

						// 080417 LUJ, 길드 스킬 개수가 유효한지 검사(스킬 레벨업만 하는 경우는 관계없음)
						if( ! GUILDMGR->GetSkillLevel( skillIndex ) )
						{
							const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting( GUILDMGR->GetLevel() );

							if( !	setting	||
									setting->mSkillSize <= GUILDMGR->GetSkillSize() )
							{
								CHATMGR->AddMsg( CTC_GUILD_NOTICE, CHATMGR->GetChatMsg( 1807 ), GUILDMGR->GetLevel() + 1 );
								break;
							}
						}
					}

					cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_SKILLTRAIN, MBT_YESNO, CHATMGR->GetChatMsg( 1172 ), pSkillInfo->GetName(), pSkillInfo->GetLevel() );		

					if( !pMsgBox ) 
					{
						return;				
					}

					SetDisable( TRUE );
				}
			}
			break;
		case SKILL_TRAINCANCEL:
			{
				SetActive( FALSE );
			}
			break;
		}
	}
	else if( we & WE_ROWCLICK )
	{
		if( IsDisable() )
		{
			return;
		}

		m_Index = m_SkillListDlg->GetCurSelectedRowIdx();
		SetSkillInfo();
	}
}

void cSkillTrainingDlg::SetSkillInfo()
{
	if( m_Index == -1 )
		return;

	m_SkillTipListDlg->RemoveAll();

	DWORD index = ( ( m_SkillList[ m_Index ].index / 100 ) * 100 ) + m_SkillList[ m_Index ].level;
	
	cSkillInfo* pInfo = SKILLMGR->GetSkillInfo( index );

	if( !pInfo )
		return;

	DWORD tooltip = pInfo->GetTooltip();

	// 080417 LUJ, 길드 스킬 표시 상태일 때, 요구 레벨을 알려준다
	if( mGuildMode )
	{
		typedef CGuildManager::SkillSettingMap SkillSettingMap;
		const SkillSettingMap* currentSettingMap	= GUILDMGR->GetSkillSetting( GUILDMGR->GetLevel() );
		const SkillSettingMap* maxLevelSettingMap	= GUILDMGR->GetMaxLevelSkillSetting();

		if( ! maxLevelSettingMap )
		{
			return;
		}
		else if( ! currentSettingMap )
		{
			currentSettingMap = maxLevelSettingMap;
		}

		cWindow* window	= GetWindowForID( SKILL_TRAINOK );

		if( window )
		{
			const DWORD skillIndex			= m_SkillList[ m_Index ].index;
			const DWORD skillLevel			= m_SkillList[ m_Index ].level;
			DWORD		requiredGuildLevel	= 0;

			// 080417 LUJ, 해당 스킬이 요구하는 최소 길드 레벨을 찾는다
			for(
				const SkillSettingMap* settingMap = 0;
				( settingMap = GUILDMGR->GetSkillSetting( ++requiredGuildLevel ) ) != maxLevelSettingMap; )
			{
				if( ! settingMap )
				{
					currentSettingMap = maxLevelSettingMap;
					break;
				}
				
				const SkillSettingMap::const_iterator it = settingMap->find( skillIndex );

				if( settingMap->end() == it )
				{
					continue;
				}

				const CGuildManager::SkillSetting& setting = it->second;

				if( skillLevel <= setting.mLevel )
				{
					break;
				}	
			}

			char text[ MAX_PATH ] = { 0 };
			{
				// 080417 LUJ, 리소스 텍스트는 액세스 때마다 파일을 열어 가져오므로 상수 포인터로 참조할 수 없다. 값 복사를 해두어야 한다.
				static const std::string textGuild			( RESRCMGR->GetMsg( 131 ) );
				static const std::string textRequiredLevel	( RESRCMGR->GetMsg( 627 ) );

				sprintf( text, "[%s %s%d]",
					textGuild.c_str(),
					textRequiredLevel.c_str(),
					requiredGuildLevel );
			}

			// 080417 LUJ, 현재 레벨로 배울 수 없는 스킬일 경우 별도 표시
			{
				DWORD color = ICONCLR_DISABLE;

				const SkillSettingMap::const_iterator it = currentSettingMap->find( skillIndex );

				if( currentSettingMap->end() != it )
				{
					const CGuildManager::SkillSetting& setting = it->second;

					if( setting.mLevel >= skillLevel )
					{
						color = TTTC_RAREITEM;
					}
				}

				window->SetActive( TTTC_RAREITEM == color );
				m_SkillTipListDlg->AddItem( text, color );
			}
		}
	}
	// 080310 LUJ, 스킬의 학습 제한 조건으로 플레이어 레벨이 있을 경우 표시한다
	else
	{		
		cWindow* window	= GetWindowForID( SKILL_TRAINOK );

		// 080310 LUJ,	위험한 캐스팅이지만, 배우는 스킬은 모두 cActiveSkillInfo형태로 정적 캐스팅되어
		//				문제없이 사용되고 있으므로, 여기서도 사용한다
		const ACTIVE_SKILL_INFO* info = ( ( cActiveSkillInfo* )pInfo )->GetSkillInfo();

		if( window	&&
			info )
		{
			if( info->RequiredPlayerLevel )
			{
				char text[ MAX_PATH ] = { 0 };
				sprintf( text, "[%s%d]", RESRCMGR->GetMsg( 627 ), info->RequiredPlayerLevel );

				if( info->RequiredPlayerLevel > HERO->GetLevel() )
				{
					m_SkillTipListDlg->AddItem( text, ICONCLR_DISABLE );

					window->SetActive( FALSE );
				}
				else
				{
					m_SkillTipListDlg->AddItem( text, TTTC_RAREITEM );

					window->SetActive( TRUE );
				}
			}
			// 080319 LUJ, 배울 수 없는 스킬이 있다가 없을 경우 버튼이 표시되지 않는 문제 수정
			else
			{
				window->SetActive( TRUE );
			}

			// 091125 ONS 스킬 트레이닝창에 필요버프 내용 추가
			DWORD dwBuffSkillIdx = info->RequiredBuff;
			if(dwBuffSkillIdx)
			{
				char text[ MAX_PATH ] = { 0 };
				cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo(dwBuffSkillIdx) ;
				sprintf(text, CHATMGR->GetChatMsg( 1973 ), pSkillInfo->GetName());
				m_SkillTipListDlg->AddItem( text, TTTC_RAREITEM );
			}
		}
	}

	
	{
		// 080310 LUJ,	위험한 캐스팅이지만, 배우는 스킬은 모두 cActiveSkillInfo형태로 정적 캐스팅되어
		//				문제없이 사용되고 있으므로, 여기서도 사용한다
		const ACTIVE_SKILL_INFO* pActiveSkillInfo = ((cActiveSkillInfo*)pInfo)->GetSkillInfo();
		if( ! pActiveSkillInfo )
		{
			return;
		}

		TCHAR line[MAX_PATH] = {0};

		// 100511 ShinJS  --- 스킬 추가 데미지 옵션 정보 추가
		switch( pActiveSkillInfo->UnitDataType )
		{
		case 1:
			{
				switch( pActiveSkillInfo->Unit )
				{
				case UNITKIND_PHYSIC_ATTCK:		sprintf( line, "%s%s", CHATMGR->GetChatMsg( 157 ), CHATMGR->GetChatMsg( 2225 ) );		break;		// "힘"
				case UNITKIND_MAGIC_ATTCK:		sprintf( line, "%s%s", CHATMGR->GetChatMsg( 160 ), CHATMGR->GetChatMsg( 2225 ) );		break;		// "지능"
				}
				m_AddDamageOptTooltip->SetStaticText( line );
			}
			break;
		case 2:
			{
				switch( pActiveSkillInfo->Unit )
				{
				case UNITKIND_PHYSIC_ATTCK:		sprintf( line, "%s%s", CHATMGR->GetChatMsg( 2134 ), CHATMGR->GetChatMsg( 2225 ) );		break;		// "무기 물리공격력"
				case UNITKIND_MAGIC_ATTCK:		sprintf( line, "%s%s", CHATMGR->GetChatMsg( 2135 ), CHATMGR->GetChatMsg( 2225 ) );		break;		// "무기 마법공격력"
				}
				m_AddDamageOptTooltip->SetStaticText( line );
			}
			break;
		default:
			{
				m_AddDamageOptTooltip->SetStaticText( "" );
			}
			break;
		}

		const DWORD dwToolTipColor = TTTC_SKILLLEVEL;

		// 100511 ShinJS --- 스킬기본 정보 추가
		if( pInfo->GetKind() != SKILLKIND_PASSIVE )
		{
			char typeTxt[MAX_PATH]={0,};
			// 필요장비
			if( SKILLMGR->GetSkillEquipTypeText( pActiveSkillInfo, typeTxt, MAX_PATH ) )
			{
				sprintf(line, CHATMGR->GetChatMsg( 2049 ), SKILLMGR->GetSkillEquipTypeText( pActiveSkillInfo, typeTxt, MAX_PATH ) );
				m_SkillTipListDlg->AddItem( line, dwToolTipColor );
			}

			// Life 표시
			if( pActiveSkillInfo->mLife.mPercent )
			{
				sprintf( line, "%s%0.0f%%", CHATMGR->GetChatMsg( 2050 ), pActiveSkillInfo->mLife.mPercent * 100.0f );
				m_SkillTipListDlg->AddItem( line, dwToolTipColor );
			}
			else if( pActiveSkillInfo->mLife.mPlus )
			{
				sprintf( line, "%s%0.0f", CHATMGR->GetChatMsg( 2050 ), pActiveSkillInfo->mLife.mPlus );
				m_SkillTipListDlg->AddItem( line, dwToolTipColor );
			}

			// 080616 LUJ, 상대적으로 소요되는 마나 양 표시
			if( pActiveSkillInfo->mMana.mPercent )
			{
				sprintf( line, "%s%0.0f%%", CHATMGR->GetChatMsg( 768 ), pActiveSkillInfo->mMana.mPercent * 100.0f );
				m_SkillTipListDlg->AddItem( line, dwToolTipColor );
			}
			// 080616 LUJ, 절대적으로 표시되는 마나 양 표시
			else if( pActiveSkillInfo->mMana.mPlus )
			{
				sprintf( line, "%s%0.0f", CHATMGR->GetChatMsg( 768 ), pActiveSkillInfo->mMana.mPlus );
				m_SkillTipListDlg->AddItem( line, dwToolTipColor );
			}

			// 필요 아이템
			if( pActiveSkillInfo->mConsumeItem.wIconIdx )
			{
				const ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( pActiveSkillInfo->mConsumeItem.wIconIdx );
				if( pItemInfo )
				{
					sprintf( line, CHATMGR->GetChatMsg( 2051 ), pItemInfo->ItemName, pActiveSkillInfo->mConsumeItem.Durability );
					m_SkillTipListDlg->AddItem( line, dwToolTipColor );
				}
			}

			// 소비 비용
			if( pActiveSkillInfo->Money )
			{
				sprintf( line, CHATMGR->GetChatMsg( 2052 ), AddComma( pActiveSkillInfo->Money ) );
				m_SkillTipListDlg->AddItem( line, dwToolTipColor );
			}

			// 소비 포인트정보
			if( pActiveSkillInfo->mConsumePoint.mType != ACTIVE_SKILL_INFO::ConsumePoint::TypeNone )
			{
				m_SkillTipListDlg->AddItem( SKILLMGR->GetConsumePointTypeText( pActiveSkillInfo, typeTxt, MAX_PATH ), dwToolTipColor );
			}

			// 시/분/초로 표현
			char timeMsg[MAX_PATH]={0,};
			int coolHour=0, coolMin=0;
			float coolSec = pActiveSkillInfo->CoolTime / 1000.f;
			if( coolSec > 0.1f )
			{
				sprintf( line, CHATMGR->GetChatMsg( 2054 ) );		// "재사용시간: "
			}
			else if( coolSec >= 60.f )
			{
				coolMin = (int)(coolSec / 60.f);
				coolSec = coolSec - coolMin * 60.f;
				if( coolMin >= 60.f )
				{
					coolHour = (int)(coolMin / 60.f);
					coolMin = coolMin - coolHour * 60;
				}
			}

			if( coolHour )
			{
				sprintf( timeMsg, CHATMGR->GetChatMsg( 1410 ), coolHour, coolMin, coolSec );
				strcat( line, timeMsg );
			}
			if( coolMin )
			{
				sprintf( timeMsg, CHATMGR->GetChatMsg( 1411 ), coolMin, coolSec );
				strcat( line, timeMsg );
			}
			if( coolSec )
			{
				sprintf( timeMsg, CHATMGR->GetChatMsg( 2053 ), coolSec );
				strcat( line, timeMsg );
			}

			m_SkillTipListDlg->AddItem( line, dwToolTipColor );

			// 091105 ONS 툴팁에 "필요 버프" 추가
			DWORD dwBuffSkillIdx = pActiveSkillInfo->RequiredBuff;
			if(dwBuffSkillIdx)
			{
				cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo(dwBuffSkillIdx) ;
				sprintf(line, CHATMGR->GetChatMsg( 1973 ), pSkillInfo->GetName());
				m_SkillTipListDlg->AddItem( line, dwToolTipColor );
			}
		}
	}

	// 100511 ShinJS --- 전체 툴팁에서 Dialog 너비만큼 잘라서 보여준다.
	const std::string& strToolTip = ITEMMGR->GetTotalToolTipMsg( tooltip );

	std::vector< std::string > cutToolTipList;
	const int nCutWidth = m_SkillTipListDlg->GetWidth() - ((cWindow*)m_SkillTipListDlg->GetGuageBar())->GetWidth();
	ITEMMGR->CutToolTipMsg( strToolTip, cutToolTipList, nCutWidth );

	for( std::vector< std::string >::const_iterator cutToolTipListIter = cutToolTipList.begin() ; cutToolTipListIter != cutToolTipList.end() ; ++cutToolTipListIter )
	{
		const std::string& cutToolTip = *cutToolTipListIter;
		m_SkillTipListDlg->AddItem( cutToolTip.c_str(), RGB_HALF( 255, 255, 255 ) );
	}

	m_SkillTipListDlg->SetTopListItemIdx( 0 );

	cImage BasicImage;

	SCRIPTMGR->GetImage( pInfo->GetImage(), &BasicImage, PFT_SKILLPATH );
	
	LONG x = (LONG)(m_Icon->GetAbsX());
	LONG y = (LONG)(m_Icon->GetAbsY());

	m_Icon->Init((LONG)(m_Icon->GetRelX()), (LONG)(m_Icon->GetRelY()), m_Icon->GetWidth(), m_Icon->GetHeight(), &BasicImage, SKILL_TRAIN_ICON);

	m_Icon->SetAbsXY( x, y );

	cActiveSkillInfo* pSkillInfo = ( cActiveSkillInfo* )pInfo;

	char text[256] = "";

	sprintf( text, "%d", pSkillInfo->GetSkillInfo()->TrainPoint );
	m_NeedSp->SetStaticText( text );
	m_NeedGold->SetStaticText( AddComma( pSkillInfo->GetSkillInfo()->TrainMoney ) );
}

void cSkillTrainingDlg::SetGold()
{
	m_Gold->SetStaticText( AddComma( HERO->GetHeroTotalInfo()->Money ) );
}

void cSkillTrainingDlg::SetSkillPoint()
{
	char buf[256];

	sprintf( buf, "%d", HERO->GetHeroTotalInfo()->SkillPoint );
	m_SkillPoint->SetStaticText( buf );
}

void cSkillTrainingDlg::Training()
{
	if( m_Index != -1 )
	{
		const DWORD skillIndex		= m_SkillList[ m_Index ].index;
		const BYTE skillNextLevel	= m_SkillList[ m_Index ].level;
		cSkillBase* pSkill			= SKILLTREEMGR->GetSkill( skillIndex );

		if( pSkill )
		{
			if( pSkill->GetSkillBase()->mLearnedLevel >= skillNextLevel )
			{
				return;
			}
		}

		MSG_SKILL_UPDATE msg;
		ZeroMemory(
			&msg,
			sizeof(msg));
		msg.dwObjectID = gHeroID;
		msg.SkillBase.wSkillIdx = skillIndex;

		if(TRUE == mGuildMode)
		{
			msg.Category = MP_GUILD;
			msg.Protocol = MP_GUILD_ADD_SKILL_SYN;
			msg.SkillBase.Level = skillNextLevel;

		}
		else
		{
			msg.Category = MP_SKILLTREE;
			msg.Protocol = MP_SKILLTREE_UPDATE_SYN;
			msg.SkillBase.Level	= skillNextLevel - 1;
		}

		NETWORK->Send( &msg, sizeof( msg ) );
	}	
}
