C// TournamentDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "TournamentDialog.h"
#include ".\tournamentdialog.h"

// CTournamentDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTournamentDialog, CDialog)
CTournamentDialog::CTournamentDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CTournamentDialog::IDD, pParent)
	, mApplication( application )
	, mServerIndex( 0 )
	, mMaxTournamentCount( 0 )
	, mSelectedGuildIndex( 0 )
	, mSelectedPlayerIndex( 0 )
{
	EnableAutomation();
}

CTournamentDialog::~CTournamentDialog()
{
}

void CTournamentDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CTournamentDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TOURNAMENT_GUILD_TREE, mGuildTreeCtrl);
	DDX_Control(pDX, IDC_TOURNAMENT_ROUND_TREE, mTableTreeCtrl);
	DDX_Control(pDX, IDC_TOURNAMENT_SERVER_COMBO, mServerCombo);
	DDX_Control(pDX, IDC_TOURNAMENT_COUNT_COMBO, mTournamentCountCombo);
	DDX_Control(pDX, IDC_TOURNAMENT_STATUS_STATIC, mStatusStatic);
}


BEGIN_MESSAGE_MAP(CTournamentDialog, CDialog)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_TOURNAMENT_COUNT_COMBO, OnCbnSelchangeTournamentCountCombo)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TOURNAMENT_GUILD_TREE, OnTvnSelchangedTournamentGuildTree)
	ON_CBN_SELCHANGE(IDC_TOURNAMENT_SERVER_COMBO, OnCbnSelchangeTournamentServerCombo)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CTournamentDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_ITournamentDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {F75F2FFE-E890-4776-A2AB-5272758DCE60}
static const IID IID_ITournamentDialog =
{ 0xF75F2FFE, 0xE890, 0x4776, { 0xA2, 0xAB, 0x52, 0x72, 0x75, 0x8D, 0xCE, 0x60 } };

BEGIN_INTERFACE_MAP(CTournamentDialog, CDialog)
	INTERFACE_PART(CTournamentDialog, IID_ITournamentDialog, Dispatch)
END_INTERFACE_MAP()

void CTournamentDialog::Request( const CString& serverName )
{
	// 081027 LUJ, 서버에 최대 라운드 정보를 요청한다
	{
		MSGBASE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_TOURNAMENT;
		message.Protocol	= MP_RM_TOURNAMENT_GET_ROUND_SYN;

		mApplication.Send( serverName, message, sizeof( message ) );
	}

	mTournamentMap.clear();
}

// CTournamentDialog 메시지 처리기입니다.

void CTournamentDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	mServerIndex = serverIndex;

	switch( message->Protocol )
	{
		// 081027 LUJ, 회차 정보를 가져온다
	case MP_RM_TOURNAMENT_GET_ROUND_ACK:
		{
			const MSG_DWORD* m	= ( MSG_DWORD* )message;			
			mMaxTournamentCount	= m->dwData;

			// 081027 LUJ, 길드 토너먼트 정보가 테이블에 전혀 없음
			if( ! mMaxTournamentCount )
			{
				CString textThereIsNoResult;
				textThereIsNoResult.LoadString( IDS_STRING1 );

				MessageBox( textThereIsNoResult, _T( "" ), MB_ICONINFORMATION | MB_OK );
				break;
			}

			// 081027 LUJ, 회차 콤보를 갱신한다
			{
				mTournamentCountCombo.ResetContent();

				for( DWORD count = 1; count <= mMaxTournamentCount; ++count )
				{
					CString text;
					text.Format(
						_T( "%d" ),
						count );
					mTournamentCountCombo.AddString( text );
				}

				mTournamentCountCombo.SetCurSel( max( 0, mTournamentCountCombo.GetCount() - 1 ) );
			}

			// 081027 LUJ, 최신 회차의 정보를 요청한다
			OnCbnSelchangeTournamentCountCombo();
			break;
		}
	case MP_RM_TOURNAMENT_GET_ROUND_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( textYouHaveNoAuthority, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	case MP_RM_TOURNAMENT_GET_DATA_PLAYER_ACK:
		{
			const MSG_RM_TOURNAMENT_PLAYER* m = ( MSG_RM_TOURNAMENT_PLAYER* )message;

			if( ! m->mTournamentCount )
			{
				break;
			}

			UnitMap&	unitMap = mTournamentMap[ m->mTournamentCount ];
			Unit&		unit	= unitMap[ m->mGuildIndex ];

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_TOURNAMENT_PLAYER::Player& player = m->mPlayer[ i ];

				unit.mPlayerList.push_back( player.mIndex );

				CString text( player.mName );
				mPlayerNameMap[ player.mIndex ] = text;
			}

			RefreshGuildTree(
				mGuildTreeCtrl,
				unitMap );
			RefreshTableTree(
				mTableTreeCtrl,
				unitMap );
			break;
		}
	case MP_RM_TOURNAMENT_GET_DATA_GUILD_ACK:
		{
			const MSG_RM_TOURNAMENT_GUILD* m = ( MSG_RM_TOURNAMENT_GUILD* )message;

			if( ! m->mTournamentCount )
			{
				break;
			}

			UnitMap& unitMap = mTournamentMap[ m->mTournamentCount ];
			
			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const MSG_RM_TOURNAMENT_GUILD::Guild& guild = m->mGuild[ i ];

				Unit& unit = unitMap[ guild.mIndex ];
				unit.mGuild	= guild;

				if( guild.mIndex )
				{
					mGuildNameMap[ guild.mIndex ] = CString( guild.mName );
				}
			}

			RefreshGuildTree(
				mGuildTreeCtrl,
				unitMap );
			RefreshTableTree(
				mTableTreeCtrl,
				unitMap );
			break;
		}
	case MP_RM_TOURNAMENT_GET_DATA_NACK:
		{
			break;
		}
	case MP_RM_TOURNAMENT_GET_TABLE_ACK:
		{
			break;
		}
	case MP_RM_TOURNAMENT_GET_TABLE_NACK:
		{
			break;
		}
	}
}

void CTournamentDialog::RefreshGuildTree( CTreeCtrl& treeCtrl, const CTournamentDialog::UnitMap& unitMap )
{
	// 081027 LUJ, 초기화
	treeCtrl.DeleteAllItems();

	class 
	{
	public:
		CString operator()( DWORD state ) const
		{
			switch( state )
			{
			case eGTState_BeforeRegist:
				{
					CString textBeforeRegist;
					textBeforeRegist.LoadString( IDS_STRING421 );

					return textBeforeRegist;
				}
			case eGTState_Regist:
				{
					CString textRegist;
					textRegist.LoadString( IDS_STRING422 );

					return textRegist;
				}
			case eGTState_BeforeEntrance:
				{
					CString textBeforeEntrance;
					textBeforeEntrance.LoadString( IDS_STRING423 );

					return textBeforeEntrance;
				}
			case eGTState_Entrance:
				{
					CString textEntrance;
					textEntrance.LoadString( IDS_STRING424 );

					return textEntrance;
				}
			case eGTState_Process:
				{
					CString textProceed;
					textProceed.LoadString( IDS_STRING425 );

					return textProceed;
				}
			case eGTState_Leave:
				{
					CString textEndBattle;
					textEndBattle.LoadString( IDS_STRING426 );

					return textEndBattle;
				}
			case eGTState_AutoSchedule:
				{
					CString textAutoSchedule;
					textAutoSchedule.LoadString( IDS_STRING427 );

					return textAutoSchedule;
				}
			}

			return _T( "" );
		}				
	}
	GetText;

	CString textGuild;
	textGuild.LoadString( IDS_STRING164 );
	MakeHeader( textGuild );
	CString textPlayer;
	textPlayer.LoadString( IDS_STRING54 );
	
	for(
		UnitMap::const_iterator unit_it = unitMap.begin();
		unitMap.end() != unit_it;
		++unit_it )
	{
		const Unit&			unit	= unit_it->second;
		const Unit::Guild&	guild	= unit.mGuild;

		// 081027 LUJ, 길드 번호가 없을 경우 상태 정보를 출력한다
		if( ! guild.mIndex )
		{
			const DWORD state = guild.mPosition;
			mStatusStatic.SetWindowText( GetText( state ) );
			continue;
		}

		CString name;
		name.Format(
			_T( "%s%s(%d)" ),
			textGuild,
			CString( guild.mName ),
			guild.mIndex );

		HTREEITEM treeItem = treeCtrl.InsertItem(
			name,
			0,
			0,
			TVI_ROOT,
			TVI_LAST );

		const Unit::PlayerList&	playerList	= unit.mPlayerList;

		for(
			Unit::PlayerList::const_iterator player_it = playerList.begin();
			playerList.end() != player_it;
			++player_it )
		{
			const DWORD playerIndex = *player_it;

			name.Format(
				_T( "%s(%d)" ),
				GetName( mPlayerNameMap, playerIndex ),
				playerIndex );

			treeCtrl.InsertItem(
				name,
				1,
				1,
				treeItem,
				TVI_LAST );
		}
	}

	// 081027 LUJ, 상태 정보를 위해 레코드 한 개가 있을 수 있다
	if( unitMap.size() <= 1 )
	{
		mTableTreeCtrl.EnableWindow( FALSE );
		mGuildTreeCtrl.EnableWindow( FALSE );
		return;
	}

	mTableTreeCtrl.EnableWindow( TRUE );
	mGuildTreeCtrl.EnableWindow( TRUE );
}

void CTournamentDialog::RefreshTableTree( CTreeCtrl& treeCtrl, const UnitMap& unitMap )
{
	// 081027 LUJ, 초기화
	treeCtrl.DeleteAllItems();

	// 081027 LUJ, 키: 위치, 값: 길드 정보
	typedef std::map< DWORD, Unit::Guild >	PositionMap;
	PositionMap								positionMap;

	POSTYPE maxPosition = 1;
	DWORD	maxRound	= 0;
	DWORD	minRound	= INT_MAX;

	// 081027 LUJ, 전투 개수를 알기 위해 길드 위치보다 큰 2의 n승 값을 구한다
	for(
		UnitMap::const_iterator unit_it = unitMap.begin();
		unitMap.end() != unit_it;
		++unit_it )
	{
		const Unit&			unit				= unit_it->second;
		const Unit::Guild&	guild				= unit.mGuild;
		const DWORD			beforeBattleStatus	= 34;

		// 081027 LUJ, 전투 전인 경우 대진표가 정해지지 않았으므로 표시할 필요가 없다
		if( guild.mPosition == beforeBattleStatus )
		{
			return;
		}
		else if( ! guild.mIndex )
		{
			continue;
		}

		while( maxPosition < guild.mPosition + 1 )
		{
			maxPosition *= 2;
		}

		maxRound = max( maxRound, guild.mRound );
		minRound = min( minRound, guild.mRound );

		// 081027 LUJ, 길드 위치 별로 분류한다
		positionMap[ guild.mPosition ] = guild;
	}

	// 081027 LUJ, 전투가 구성되지 않은 상태
	if( minRound == INT_MAX )
	{
		return;
	}

	// 081027 LUJ, 길드 번호가 쌍으로 담긴다. 이 쌍들이 대결 상대를 의미
	typedef std::pair< DWORD, DWORD >		Battle;
	// 081027 LUJ, 대결 정보가 목록으로 담긴다
	typedef std::list< Battle >				BattleList;
	// 081027 LUJ, 라운드 별로 정보가 담긴다
	typedef	std::map< DWORD, BattleList >	RoundMap;
	RoundMap								roundMap;

	// 081027 LUJ, 최초 라운드를 구성한다
	{
		BattleList& battleList = roundMap[ minRound ];
		// 081027 LUJ, 한 전투는 두 길드로 이뤄지므로, 전투 규모는 길드 위치의 절반이다.
		const DWORD battleSize = maxPosition / 2;

		// 081027 LUJ, 리스트를 구성한다. 부전승을 구분하기 위해 빈값도 필요하다
		for(
			POSTYPE position = 0;
			position < battleSize;
			++position )
		{
			battleList.push_back( Battle() );
		}

		// 081027 LUJ, 각 전투 정보에 배치한다
		for(
			POSTYPE position = 0;
			position < maxPosition;
			++position )
		{
			// 081027 LUJ, 한 전투는 두 길드 번호의 쌍으로 배치되므로 현재 위치를 2로 나누어야한다
			const POSTYPE battlePosition = position / 2;

			if( battlePosition >= battleList.size() )
			{
				ASSERT( 0 );
				break;
			}

			BattleList::iterator battle_it = battleList.begin();
			std::advance( battle_it, battlePosition );

            if( battleList.end() == battle_it )			
			{
				continue;
			}
			else if( positionMap.end() == positionMap.find( position ) )
			{
				continue;
			}

			Battle&				battle	= *battle_it;
			const Unit::Guild&	guild	= positionMap[ position ];

			// 081027 LUJ, 홀수이면 오른쪽에, 짝수이면 왼쪽에 배치한다
			( position % 2 ) ? ( battle.second = guild.mIndex ) : ( battle.first = guild.mIndex );
		}
	}

	// 081027 LUJ, 전투에서 승리한 길드 번호를 반환한다
	class 
	{
	public:
		DWORD operator()( const Battle& battle, DWORD round, const UnitMap& unitMap ) const
		{
			// 081027 LUJ, 현재 라운드에서 승리한 길드인지 판단하는 내부 함수
			class 
			{
			public:
				bool operator()( DWORD guildIndex, DWORD round, const UnitMap& unitMap ) const
				{
					UnitMap::const_iterator it = unitMap.find( guildIndex );

					if( unitMap.end() == it )
					{
						return false;
					}

					const Unit& unit = it->second;

					return unit.mGuild.mRound > round;
				}
			}
			IsWonGuild;

			const bool IsWonfirst	= IsWonGuild( battle.first, round, unitMap );
			const bool IsWonSecond	= IsWonGuild( battle.second, round, unitMap );

			if( IsWonfirst )
			{
				return battle.first;
			}
			else if( IsWonSecond )
			{
				return battle.second;
			}

			// 081027 LUJ, 해당 전투에 참가팀이 없을 경우이다. 공간을 채우기 위해 빈 값을 넘겨준다
			return 0;
		}
	}
	GetWonGuildIndex;
	
	// 081027 LUJ, 최대 라운드까지 구성한다
	for(
		DWORD round = minRound;
		round < maxRound;
		++round )
	{
		const BattleList&			battleList	= roundMap[ round ];
		BattleList::const_iterator	battle_it	= battleList.begin();

		BattleList&	nextBattleList	= roundMap[ round + 1 ];

		while( battleList.end() != battle_it )
		{
			Battle emptyBattle;

			// 081027 LUJ, 두 개의 전투 정보를 꺼낸다
			const Battle& firstBattle	= ( battleList.end() == battle_it ? emptyBattle : *battle_it++ );
			const Battle& secondBattle	= ( battleList.end() == battle_it ? emptyBattle : *battle_it++ );

			const Battle battle(
				GetWonGuildIndex( firstBattle, round, unitMap ),
				GetWonGuildIndex( secondBattle, round, unitMap ) );
			nextBattleList.push_back( battle );
		}
	}

	int count = 0;

	// 081027 LUJ, 트리 컨트롤을 구성한다
	for(
		RoundMap::const_iterator round_it = roundMap.begin();
		roundMap.end() != round_it;
		++round_it )
	{
		const BattleList& battleList = round_it->second;

		CString textRound;
		textRound.LoadString( IDS_STRING428 );

		TCHAR text[ MAX_PATH ] = { 0 };
		_stprintf(
			text,
			_T( "%d %s\n" ),
			++count,
			textRound );

		HTREEITEM treeItem = treeCtrl.InsertItem(
			text,
			0,
			0,
			TVI_ROOT,
			TVI_LAST );

		for(
			BattleList::const_iterator battle_it = battleList.begin();
			battleList.end() != battle_it;
			++battle_it )
		{
			const Battle& battle			= *battle_it;
			const CString firstGuildName	= GetName( mGuildNameMap, battle.first );
			const CString secondGuildName	= GetName( mGuildNameMap, battle.second );

			const TCHAR* textAbsence = _T( "[N/A]" );

			_stprintf(
				text,
				_T( "\t%s vs %s\n" ),
				firstGuildName.IsEmpty()	? textAbsence : firstGuildName,
				secondGuildName.IsEmpty()	? textAbsence : secondGuildName );

			treeCtrl.InsertItem(
				text,
				0,
				0,
				treeItem,
				TVI_LAST );
		}
	}
}

CString CTournamentDialog::GetServerName() const
{
	CString name;

	mServerCombo.GetLBText( mServerCombo.GetCurSel(), name );

	return name;
}

BOOL CTournamentDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 콤보 컨트롤 초기화
	{
		CRect rect;

		mServerCombo.GetWindowRect( rect );
		mServerCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

		mTournamentCountCombo.GetWindowRect( rect );
		mTournamentCountCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTournamentDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	mApplication.SetServerComboBox( mServerCombo );

	// 081027 LUJ, 서버에 토너먼트 최대 회차를 얻어온다
	{
		MSGBASE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_TOURNAMENT;
		message.Protocol	= MP_RM_TOURNAMENT_GET_ROUND_SYN;

		mApplication.Send(
			GetServerName(),
			message,
			sizeof( message ) );
	}	
}

void CTournamentDialog::OnCbnSelchangeTournamentCountCombo()
{
	CString textTournamentCount;
	mTournamentCountCombo.GetLBText(
		mTournamentCountCombo.GetCurSel(),
		textTournamentCount );

	const DWORD tournamentCount = _ttoi( textTournamentCount );
	
	// 081027 LUJ, 최신 회차 정보가 아닌 경우 메모리에 보관된 정보를 사용한다	
	if( mMaxTournamentCount != tournamentCount &&
		mTournamentMap.end() != mTournamentMap.find( tournamentCount ) )
	{
		const UnitMap& unitMap = mTournamentMap[ tournamentCount ];

		RefreshGuildTree(
			mGuildTreeCtrl,
			unitMap );
		RefreshTableTree(
			mTableTreeCtrl,
			unitMap );
		return;
	}
	
	// 081027 LUJ, 서버에 데이터 요청
	{
		// 081027 LUJ, 보관된 정보는 삭제한다
		mTournamentMap.erase( tournamentCount );

		MSGBASE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_RM_TOURNAMENT;
		message.Protocol	= MP_RM_TOURNAMENT_GET_DATA_SYN;
		message.dwObjectID	= tournamentCount;

		mApplication.Send(
			mServerIndex,
			message,
			sizeof( message ) );
	}
}

void CTournamentDialog::MakeHeader( CString& text ) const
{
	text = _T( "[" ) + text + _T( "] " );
}

void CTournamentDialog::OnTvnSelchangedTournamentGuildTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	HTREEITEM treeItem = mGuildTreeCtrl.GetSelectedItem();

	if( ! treeItem )
	{
		return;
	}

	CString textGuild;
	textGuild.LoadString( IDS_STRING164 );
	MakeHeader( textGuild );
	
	class 
	{
	public:
		DWORD operator()( const NameMap& nameMap, const CString& keyword ) const
		{
			for(
				NameMap::const_iterator it = nameMap.begin();
				nameMap.end() != it;
				++it )
			{
				const DWORD		index	= it->first;
				const CString&	name	= it->second;

				if( name == keyword )
				{
					return index;
				}
			}

			return 0;
		}
	}
	GetIndex;

	CString text = mGuildTreeCtrl.GetItemText( treeItem );

	if( ! _tcsncmp( textGuild, text, _tcslen( textGuild ) ) )
	{
		mSelectedGuildIndex = GetIndex( mGuildNameMap, text );
	}
	else
	{
		mSelectedPlayerIndex = GetIndex( mGuildNameMap, text );
	}
}

CString	CTournamentDialog::GetName( const CTournamentDialog::NameMap& nameMap, DWORD index ) const
{
	const NameMap::const_iterator it = nameMap.find( index );

	return nameMap.end() == it ? _T( "" ) : it->second;
}

void CTournamentDialog::OnCbnSelchangeTournamentServerCombo()
{
	Request( GetServerName() );
}