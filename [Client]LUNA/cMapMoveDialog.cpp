#include "stdafx.h"															// 표준 헤더파일을 포함한다.
#include "../WindowIDEnum.h"
#include "../Interface/cWindowManager.h"
#include "../ChatManager.h"
#include "../cMsgBox.h"
#include "..\MHFile.h"
#include "ChatManager.h"
#include "./Input/Mouse.h"
#include "ObjectManager.h"
#include "Item.h"
#include "./Interface/cListDialog.h"
#include ".\cmapmovedialog.h"
#include "ObjectStateManager.h"

// 081006 LYW --- cMapMoveDialog : 공성 매니져 헤더 포함.
#include "./SiegeWarfareMgr.h"

// 081125 LYW --- cMapMoveDialog : 리미트 던전 매니져 헤더 포함.
#include "./LimitDungeonMgr.h"
#include "./MHMap.h"
#include "DungeonMgr.h"
#include "Npc.h"

struct stMAPMOVEITEM
{
	MAPTYPE mapnum;
	TCHAR mapName[MAX_PATH];
};

cMapMoveDialog::cMapMoveDialog(void)
{
	m_pMapListDlg = 0;
	m_pMoveBtn = 0;
	m_pCloseBtn = 0;
	m_pXCloseBtn = 0;
	m_nSelectedMapNum = -1;
	m_pItem = 0;
	m_byUseCase = eMapMoveNone;
}





cMapMoveDialog::~cMapMoveDialog(void)
{
	ReleaseMapList();
}





void cMapMoveDialog::Linking()
{
	m_pMapListDlg	= (cListDialog*)GetWindowForID(MMD_MAPLIST) ;				// 맵 리스트를 링크한다.

	m_pMoveBtn		= (cButton*)GetWindowForID(MMD_MOVEBTN) ;					// 이동 버튼을 링크한다.
	m_pCloseBtn		= (cButton*)GetWindowForID(MMD_CLOSEBTN) ;					// 닫기 버튼을 링크한다.
	m_pXCloseBtn	= (cButton*)GetWindowForID(MMD_XCLOSEBTN) ;					// X닫기 버튼을 링크한다.

	InitializeMapList() ;														// 맵 리스트 정보를 초기화 한다.
}

DWORD cMapMoveDialog::ActionEvent(CMouse * mouseInfo)
{
	if(IsDisable())
	{
		return WE_NULL;
	}

	const DWORD we = cDialog::ActionEvent( mouseInfo );

	if(m_pMapListDlg->PtInWindow(mouseInfo->GetMouseX(), mouseInfo->GetMouseY()))
	{
		if(we & WE_LBTNCLICK)
		{
			const ITEM* const pItem =  m_pMapListDlg->GetItem(m_pMapListDlg->GetCurSelectedRowIdx()) ;

			if(pItem)
			{
				m_nSelectedMapNum = pItem->line;
			}
	
			switch(m_byUseCase)
			{
			case eNormalStyle:
				SetMapMoveInfo();
				break;
			case eCastle_Rushen:
			case eCastle_Zevyn:
				SetSiegeWarMoveInfo();
				break;
			case eLimitDungeon:
				SetLimitDungeon();
				break;
			case eDungeon:
				SetDungeon();
				break;
			}
		}
		else if( we & WE_LBTNDBLCLICK)
		{
			MoveNotice() ;														// 맵 이동 싱크를 요청한다.
		}
	}

	return we ;
}





void cMapMoveDialog::OnActionEvent( LONG id, void* p, DWORD event ) 
{
	if( event != WE_BTNCLICK ) return ;

	switch( id )
	{
	case MMD_MOVEBTN :
		{
			MoveNotice() ;
		}
		break ;

	case MMD_XCLOSEBTN :
	case MMD_CLOSEBTN :
		{
			// 080919 LYW --- cMapMoveDialog : 사용 용도에 따른 구분 처리.
			if( m_byUseCase == eNormalStyle )
			{
				if( m_pItem )
				{
					m_pItem->SetLock(FALSE) ;
					m_pItem = NULL ;
				}
			}
			
			SetActive( FALSE ) ;
		}
		break ;
	}
}

void cMapMoveDialog::SetUseCase(eMapMoveType type, DWORD dwSelectNpcID)
{
	// 091106 LUJ, GM툴이 아닌 경우에는 반복 처리하지 않도록 한다
#ifndef _GMTOOL_
	if(m_byUseCase == type)
	{
		return;
	}
#endif

	m_byUseCase = type;
	m_dwSelectedNpcID = dwSelectNpcID;

	switch(type)
	{
	case eNormalStyle:
		SetMapMoveInfo();
		break;
	case eCastle_Rushen:
	case eCastle_Zevyn:
		SetSiegeWarMoveInfo();
		break;
	case eLimitDungeon:
		SetLimitDungeon();
		break;
	case eDungeon:
		SetDungeon();
		break;
	}

	m_pMapListDlg->SetCurSelectedRowIdx(-1);
}

void cMapMoveDialog::InitializeMapList()
{
	CMHFile file;
	file.Init(
		"./System/Resource/MapMoveScroll.bin",
		"rb");

	while(!file.IsEOF())
	{
		stMAPMOVEITEM* const pItem = new stMAPMOVEITEM;

		if(0 == pItem)
		{
			continue;
		}
		
		m_MapMoveList.AddTail(pItem);
		ZeroMemory(pItem, sizeof(*pItem));

		pItem->mapnum = file.GetWord();
		SafeStrCpy(
			pItem->mapName,
			GetMapName(pItem->mapnum),
			sizeof(pItem->mapName) / sizeof(*pItem->mapName));
	}
}

void cMapMoveDialog::ReleaseMapList()
{
	PTRLISTPOS pDeletePos = m_MapMoveList.GetHeadPosition() ;

	while(pDeletePos)
	{
		stMAPMOVEITEM* pItem = (stMAPMOVEITEM*)m_MapMoveList.GetNext(pDeletePos) ;
		SAFE_DELETE(pItem);
	}

	m_MapMoveList.RemoveAll();
}





void cMapMoveDialog::MoveMapSyn()
{
	// 081125 LYW --- cMapMoveDialog : 맵이동 요청을 상황에 따라 처리 한다.
	switch( m_byUseCase )
	{
	case eNormalStyle :
		{
			if( m_nSelectedMapNum <= 0 || m_nSelectedMapNum >= 100 )
			{
				WINDOWMGR->MsgBox( MBI_MAPMOVE_NOTICE, MBT_OK, CHATMGR->GetChatMsg( 56 ) ) ;	
				break;
			}
			else if( HERO->IsPKMode() )
			{
				WINDOWMGR->MsgBox( MBI_MAPMOVE_NOTICE, MBT_OK, CHATMGR->GetChatMsg( 358 ) ) ;	
				break;
			}
			
			MSG_DWORD3 msg;
			ZeroMemory(&msg, sizeof(msg));
			msg.Category	= MP_ITEM ;
			msg.Protocol	= MP_ITEM_MAPMOVE_SCROLL_SYN ;
			msg.dwObjectID	= HEROID ;
			msg.dwData1		= m_nSelectedMapNum ;
			msg.dwData2		= m_pItem->GetPosition() ;
			msg.dwData3		= m_pItem->GetItemIdx() ;

			NETWORK->Send( &msg, sizeof(MSG_DWORD3) ) ;
		}
		break ;

	case eCastle_Rushen :
	case eCastle_Zevyn :
		{
			MSG_WORD2 msg ;

			msg.Category	= MP_SIEGEWARFARE ;
			msg.Protocol	= MP_SIEGEWARFARE_CHANGEMAP_GUILDCOMMITTEE_SYN ;
			msg.dwObjectID	= gHeroID ;
			msg.wData1		= WORD(m_byUseCase);
			msg.wData2		= WORD(m_nSelectedMapNum);

			NETWORK->Send( &msg, sizeof(MSG_WORD2) ) ;
		}
		break;
	}
}


void cMapMoveDialog::MoveNotice() 
{
	switch(m_byUseCase)
	{
	case eCastle_Rushen:
		{
			st_GUILDMAPMOVEINFO Info = {0};
			SIEGEWARFAREMGR->Get_GuildMapMoveInfo_Rushen(
				&Info,
				WORD(m_nSelectedMapNum));

			if(0 == Info.mapNum)
			{
				break;
			}

			WINDOWMGR->MsgBox(
				MBI_MAPMOVE_AREYOUSURE,
				MBT_YESNO,
				CHATMGR->GetChatMsg(480),
				GetMapName(Info.mapNum));
		}
		break ;

	case eCastle_Zevyn:
		{
			st_GUILDMAPMOVEINFO Info = {0};
			SIEGEWARFAREMGR->Get_GuildMapMoveInfo_Zevyn(
				&Info,
				WORD(m_nSelectedMapNum));

			if(0 == Info.mapNum)
			{
				break;
			}

			WINDOWMGR->MsgBox(
				MBI_MAPMOVE_AREYOUSURE,
				MBT_YESNO,
				CHATMGR->GetChatMsg(480),
				GetMapName(Info.mapNum));
		}
		break;
	case eNormalStyle:
		{
			PTRLISTPOS pDeletePos = m_MapMoveList.GetHeadPosition();

			while( pDeletePos )
			{
				stMAPMOVEITEM* pItem = (stMAPMOVEITEM*)m_MapMoveList.GetNext(pDeletePos);

				if( !pItem ) continue ;

				if( pItem->mapnum == m_nSelectedMapNum )
				{
					WINDOWMGR->MsgBox(
						MBI_MAPMOVE_AREYOUSURE,
						MBT_YESNO,
						CHATMGR->GetChatMsg(480),
						pItem->mapName);
				}
			}
		}
		break;
	case eLimitDungeon:
		{
			typedef CGameResourceManager::LimitDungeonScriptMap ScriptMap;
			const ScriptMap& scriptMap = g_CGameResourceManager.GetLimitDungeonScriptMap();
			const LimitDungeonScript* script = 0;
			int line = 0;

			for(ScriptMap::const_iterator iter = scriptMap.begin();
				scriptMap.end() != iter;
				++iter)
			{
				if(++line == m_nSelectedMapNum)
				{
					script = &(iter->second);
					LIMITDUNGEONMGR->AskJoin(
						script->mMapType,
						script->mChannel);
					break;
				}
			}
		}
		break;

	case eDungeon:
		{
			const ITEM* const pItem =  m_pMapListDlg->GetItem( m_pMapListDlg->GetClickedRowIdx() );
			if( !pItem )
				break;

			const DWORD dwDungeonKey = pItem->dwData;
			stDungeonKey* pDungeonKeyInfo = GAMERESRCMNGR->GetDungeonKey( dwDungeonKey );
			if( !pDungeonKeyInfo )
				break;

			if(MAP->GetMapNum() == pDungeonKeyInfo->wMapNum)
			{
				CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1977) );
				break;
			}

			CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( m_dwSelectedNpcID );
			if( !pNpc )
				break;

			// 인던 입장 요청
			MSG_DWORD4 msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_DUNGEON;
			msg.Protocol = MP_DUNGEON_PORTAL_ENTRANCE_SYN;
			msg.dwData1 = pDungeonKeyInfo->dwIndex;
			msg.dwData2 = pDungeonKeyInfo->wMapNum;
			msg.dwData3 = pNpc->GetID();
			msg.dwData4 = pNpc->GetNpcUniqueIdx();
			NETWORK->Send(&msg,sizeof(msg));
		}
		break;
	}
}

// 080919 LYW --- cMapMoveDialog : 사용 용도에 따른 맵이동 정보 설정함수.
void cMapMoveDialog::SetMapMoveInfo()
{
	// 임시 변수들을 선언 / 초기화 한다.
	char tempBuf[512] ;
	DWORD dwColor = RGB_HALF(10, 10, 10) ;

	int BackupLine = 0 ;
	BackupLine = m_pMapListDlg->GetTopListItemIdx() ;


	// 리스트 다이얼로그를 비운다.
	m_pMapListDlg->RemoveAll() ;

	stMAPMOVEITEM* pNormalItem ;


	// 맵 이동 정보를 설정한다.
	PTRLISTPOS pos ;
	pos = m_MapMoveList.GetHeadPosition() ;	

	// 리스트 다이얼로그를 세팅한다.
	while(pos)
	{
		pNormalItem = NULL ;
		pNormalItem = (stMAPMOVEITEM*)m_MapMoveList.GetNext(pos) ;

		if( !pNormalItem ) continue ;

		if( m_nSelectedMapNum == pNormalItem->mapnum )
		{
			memset(tempBuf, 0, sizeof(tempBuf)) ;
			
			strcpy(tempBuf, "[ ") ;
			strcat(tempBuf, GetMapName(MAPTYPE(m_nSelectedMapNum)));
			strcat(tempBuf, " ]") ;

			m_pMapListDlg->AddItem(tempBuf, RGB_HALF(255, 255, 0), pNormalItem->mapnum) ;
			continue ;
		}

		m_pMapListDlg->AddItem(pNormalItem->mapName, dwColor, pNormalItem->mapnum) ;
	}


	// 아이템의 툴팁 인덱스를 세팅한다.
	m_pMapListDlg->ResetGuageBarPos() ;
	m_pMapListDlg->SetTopListItemIdx(BackupLine) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: SetSiegeWarMoveInfo()
//	DESC		: 공성전에서 집행위원을 통해 맵 이동을 할 때, 맵이동 정보를 설정하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: November 25, 2008
//-------------------------------------------------------------------------------------------------
void cMapMoveDialog::SetSiegeWarMoveInfo()
{
	const int BackupLine = m_pMapListDlg->GetTopListItemIdx();
	m_pMapListDlg->RemoveAll();
	const CSiegeWarfareMgr::L_GUILDMAPMOVEINFO& moveList = (m_byUseCase == eCastle_Zevyn ? SIEGEWARFAREMGR->Get_MapMoveList_Zevyn() : SIEGEWARFAREMGR->Get_MapMoveList_Rushen());

	for(CSiegeWarfareMgr::L_GUILDMAPMOVEINFO::const_iterator it = moveList.begin();
		it != moveList.end();
		++it )
	{
		const st_GUILDMAPMOVEINFO& info = *it ;

		if(m_nSelectedMapNum == info.mapNum)
		{
			TCHAR text[MAX_PATH] = {0};
			_stprintf(
				text,
				"[ %s ]",
				GetMapName(info.mapNum));
			m_pMapListDlg->AddItem
				(text,
				RGB_HALF(255, 255, 0),
				info.mapNum);
		}
		else
		{
			m_pMapListDlg->AddItem(
				GetMapName(info.mapNum),
				RGB_HALF(10, 10, 10),
				info.mapNum);
		}
	}


	// 아이템의 툴팁 인덱스를 세팅한다.
	m_pMapListDlg->ResetGuageBarPos() ;
	m_pMapListDlg->SetTopListItemIdx(BackupLine) ;
}

void cMapMoveDialog::SetLimitDungeon()
{
	const int previousLine = m_pMapListDlg->GetTopListItemIdx();
	m_pMapListDlg->RemoveAll();

	typedef CGameResourceManager::LimitDungeonScriptMap ScriptMap;
	const ScriptMap& scriptMap = g_CGameResourceManager.GetLimitDungeonScriptMap();
	int line = 0;

	for(ScriptMap::const_iterator iter = scriptMap.begin();
		scriptMap.end() != iter;
		++iter)
	{
		const LimitDungeonScript& script = iter->second;
		cListDialog::ToolTipTextList toolTipList;
		LIMITDUNGEONMGR->SetToolTip(
			script,
			toolTipList);
		TCHAR titleText[MAX_PATH] = {0};
		_stprintf(
			titleText,
			"%s (%s)",
			GetMapName(script.mMapType),
			CHATMGR->GetChatMsg(script.mNameIndex));

		if(m_nSelectedMapNum == line + 1)
		{
			TCHAR text[MAX_PATH] = {0};
			_stprintf(
				text,
				"[ %s ]",
				titleText);
			m_pMapListDlg->AddItem(
				text,
				RGB_HALF(255, 255, 0),
				toolTipList,
				++line);
		}
		else
		{
			m_pMapListDlg->AddItem(
				titleText,
				RGB_HALF(10, 10, 10),
				toolTipList,
				++line);
		}
	}

	m_pMapListDlg->ResetGuageBarPos();
	m_pMapListDlg->SetTopListItemIdx(previousLine);
}

void cMapMoveDialog::SetDungeon()
{
	const int prevTopList = m_pMapListDlg->GetTopListItemIdx();
	const int prevClickedList = m_pMapListDlg->GetClickedRowIdx();
	m_pMapListDlg->RemoveAll();


	CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( m_dwSelectedNpcID );
	if( !pNpc )
		return;

	std::multimap< DWORD, DWORD >& entranceNpcJobMap = DungeonMGR->GetEntranceNpcJobMap();
	typedef std::multimap<DWORD, DWORD>::const_iterator EntranceNpcJobIter;
	std::pair<EntranceNpcJobIter, EntranceNpcJobIter> EqualRange;
	EqualRange = entranceNpcJobMap.equal_range( pNpc->GetNpcJob() );

	if( EqualRange.first == entranceNpcJobMap.end() &&
		EqualRange.second == entranceNpcJobMap.end() )
	{
		return;
	}

	int line = 0;
	for( EntranceNpcJobIter npcIter = EqualRange.first ; npcIter != EqualRange.second ; ++npcIter )
	{
		const DWORD dwDungeonKey	= npcIter->second;

		stDungeonKey* pDungeonKeyInfo = GAMERESRCMNGR->GetDungeonKey( dwDungeonKey );
		if( !pDungeonKeyInfo )
			return;

		// 100616 ONS 인던 입장가능 레벨대 필터링 및 툴팁 출력기능 추가
		const WORD ENTRANCE_PLUS_LEVEL = 20;
		LEVELTYPE heroLevel = HERO->GetLevel() ;
		if( heroLevel + ENTRANCE_PLUS_LEVEL <= MAX_CHARACTER_LEVEL_NUM )
		{
			// 최대레벨을 넘지 않는다면 현재레벨 +20까지 입장가능하도록 체크한다.
			if( !(heroLevel >= pDungeonKeyInfo->wMinLevel					|| 
				  heroLevel + ENTRANCE_PLUS_LEVEL >= pDungeonKeyInfo->wMinLevel)	||
				  heroLevel > pDungeonKeyInfo->wMaxLevel )
			{
				continue;
			}
		}
		else
		{
			// +20이 최대레벨을 넘을경우(레벨131이상)는 해당되는 레벨만 입장가능하도록 체크한다.
			if( heroLevel < pDungeonKeyInfo->wMinLevel	|| 
				heroLevel > pDungeonKeyInfo->wMaxLevel )
			{
				continue;
			}
		}

		cListDialog::ToolTipTextList toolTipList;
		TCHAR toolTipTxt[MAX_PATH] = {0};
		_stprintf( toolTipTxt, "%s", CHATMGR->GetChatMsg( pDungeonKeyInfo->dwTooltipIndex ) );
		toolTipList.push_back(toolTipTxt);

		TCHAR titleText[MAX_PATH] = {0};
		_stprintf( titleText, CHATMGR->GetChatMsg( 1764 ), 
								GetMapName( pDungeonKeyInfo->wMapNum ), 
								CHATMGR->GetChatMsg( 1761 + pDungeonKeyInfo->difficulty - 1 ),
								pDungeonKeyInfo->wMinLevel, 
								pDungeonKeyInfo->wMaxLevel );

		if( m_nSelectedMapNum == ++line )
		{
			TCHAR text[MAX_PATH] = {0};
			_stprintf( text, "[ %s ]", titleText );
			m_pMapListDlg->AddItem( text, RGB_HALF(255, 255, 0), toolTipList, line, dwDungeonKey );
		}
		else
		{
			m_pMapListDlg->AddItem( titleText, RGB_HALF(10, 10, 10), toolTipList, line, dwDungeonKey );
		}
	}

	m_pMapListDlg->ResetGuageBarPos();
	m_pMapListDlg->SetTopListItemIdx( prevTopList );
	m_pMapListDlg->SetClickedRowIdx( prevClickedList );
}

void cMapMoveDialog::SetActive(BOOL isActive)
{
	if(isActive)
	{
		OBJECTSTATEMGR->StartObjectState(
			OBJECTMGR->GetObject(gHeroID),
			eObjectState_Deal);
	}
	else
	{
		OBJECTSTATEMGR->EndObjectState(
			OBJECTMGR->GetObject(gHeroID),
			eObjectState_Deal);

		// 100319 ShinJS --- ESC키 사용시 Item Lock이 풀리지 않는 현상 수정.
		if( m_pItem )
		{
			m_pItem->SetLock(FALSE) ;
			m_pItem = NULL ;
		}
	}

	cDialog::SetActive(isActive);
}

DWORD cMapMoveDialog::GetMapListCount() const
{ 
	return m_pMapListDlg->GetItemCount(); 
}