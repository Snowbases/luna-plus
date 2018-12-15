#include "stdafx.h"

#ifdef _GMTOOL_

#include "GMToolManager.h"
#include "ObjectManager.h"
#include "ItemManager.h"
#include "CheatMsgParser.h"
#include "FilteringTable.h"
#include "ChatManager.h"

#include "MHMap.h"
#include "Commdlg.h"
#include "CommCtrl.h"
#include "MHFile.h"
#include "cSkillTreeManager.h"
#include "MainGame.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[cc]skill/client/info/activeskillinfo.h"
#include "WeatherManager.h"
#include "DungeonMgr.h"
#include "cResourceManager.h"
#include "GameIn.h"
#include "BigMapDlg.h"
#include "TileManager.h"

extern HWND _g_hWnd;
extern HINSTANCE g_hInst;

GLOBALTON(CGMToolManager)

//extern HWND ghWnd;

extern int g_MapChannelNum[100];




CGMToolManager::CGMToolManager()
{
	m_hWndDlg = NULL;
	ZeroMemory( m_hWndSub, sizeof( m_hWndSub ) );

	m_nSelectMenu	= eMenu_Count;
	m_bShow			= FALSE;
	m_bCanUse		= FALSE;
	m_bSubShow		= FALSE;
	m_cbChannelCount = 0;
	m_hWndChatList = NULL;
	m_nNumNow			= 0;
	m_nNumWait			= 0;
	m_nBufLen			= 0;

	m_bLogin			= FALSE;
	m_nPower			= eGM_POWER_MAX;	//3
	m_hWndLoginDlg		= NULL;
//임시
	m_nNullMove			= 0;
}

CGMToolManager::~CGMToolManager()
{

}

BOOL CGMToolManager::CreateGMDialog()
{
	if( m_hWndDlg ) return FALSE;

	m_hWndDlg = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GMDIALOG), _g_hWnd, GMDlgProc );	
	m_hWndLoginDlg = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GMLOGIN), _g_hWnd, GMLoginDlgProc );

	CreateGMSubDialog();

//	CHEATMGR->SetCheatEnable( TRUE );
	return TRUE;
}

BOOL CGMToolManager::CreateGMSubDialog()
{
	m_hWndSub[eMenu_Move]	= CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_MOVE_DIALOG), m_hWndDlg, GMSubMoveDlgProc );
	m_hWndSub[eMenu_Where]	= CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_WHERE_DIALOG), m_hWndDlg, GMSubWhereDlgProc );
	m_hWndSub[eMenu_Item]	= CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_ITEM_DIALOG), m_hWndDlg, GMSubItemDlgProc );
	m_hWndSub[eMenu_Hide]	= CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_HIDE_DIALOG), m_hWndDlg, GMSubHideDlgProc );

	m_hWndSub[eMenu_PK]		= CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_PK_DIALOG), m_hWndDlg, GMSubPKDlgProc );
	m_hWndSub[eMenu_Discon] = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_DISCON_DIALOG), m_hWndDlg, GMSubDisconDlgProc );
	m_hWndSub[eMenu_ForbidChat]	= CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_CHAT_DIALOG), m_hWndDlg, GMSubForbidChatDlgProc );
	
	m_hWndSub[eMenu_Mob]	= CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_REGEN_DIALOG), m_hWndDlg, GMSubRegenDlgProc );
	m_hWndSub[eMenu_Counsel]= CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_COUNSEL_DIALOG), m_hWndDlg, GMSubCounselDlgProc );
	m_hWndSub[eMenu_Notice]	= CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_NOTICE_DIALOG), m_hWndDlg, GMSubNoticeDlgProc );
	m_hWndSub[eMenu_Event] = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_EVENT_DIALOG), m_hWndDlg, GMSubEventDlgProc );
	m_hWndSub[eMenu_EventMap] = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_EVENTMAP_DIALOG), m_hWndDlg, GMSubEventMapDlgProc );
	m_hWndSub[eMenu_EventNotify] = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_EVENTNOTIFY_DIALOG), m_hWndDlg, GMSubEventNotifyDlgProc );
	m_hWndSub[eMenu_Weather]= CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_WEATHER_DIALOG), m_hWndDlg, GMWeatherDlgProc );

	m_hWndSub[eMenu_Dungeon]= CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_GM_DUNGEON_DIALOG), m_hWndDlg, GMDungeonDlgProc );
	return TRUE;
}

BOOL CGMToolManager::DestroyGMDialog()
{
	if( !m_hWndDlg ) return FALSE;

	SaveChatList();

//	DestroyWindow( m_hWndDlg );
	m_hWndDlg = NULL;
	m_hWndLoginDlg = NULL;

	DestroyGMSubDialog();

	m_nSelectMenu = eMenu_Count;

	return TRUE;
}

BOOL CGMToolManager::DestroyGMSubDialog()
{
	for( int i = 0 ; i < eMenu_Count ; ++i )
	{
		if( m_hWndSub[i] )
		{
//			DestroyWindow( m_hWndSub[i] );
			m_hWndSub[i] = NULL;
		}
	}

	return TRUE;
}

void CGMToolManager::ShowGMDialog( BOOL bShow, BOOL bUpdate )
{
	if( !m_hWndDlg )		return;
	if( !m_bCanUse )		return;


//여기서부터 로그인(Login) 체크 하는곳
	if( bShow && !m_bLogin && MAINGAME->GetUserLevel() == eUSERLEVEL_GM )
	{
		ShowWindow( m_hWndLoginDlg, SW_SHOW );

		return;
	}
	else if( MAINGAME->GetUserLevel() > eUSERLEVEL_GM )
	{
		return;
	}
//여기까지


	if( bShow )
	{
		ShowWindow( m_hWndDlg, SW_SHOWNA );
		SetPositionByMainWindow();
/*
		// 100613 ONS Developer와 GM은 Move, Item&Skill, Hide, Mob기능만 이용가능.
		BOOL bIsEnable = TRUE;
		if( MAINGAME->GetUserLevel() == eUSERLEVEL_GM		|| 
			MAINGAME->GetUserLevel() == eUSERLEVEL_DEVELOPER )
		{
			bIsEnable = FALSE;
		}
		else
		{
			bIsEnable = TRUE;
		}

		EnableWindow( GetDlgItem( m_hWndDlg, IDC_GM_MNBTN_WHERE ), bIsEnable );
		EnableWindow( GetDlgItem( m_hWndDlg, IDC_GM_MNBTN_PK ), bIsEnable );
		EnableWindow( GetDlgItem( m_hWndDlg, IDC_GM_MNBTN_DISCON ), bIsEnable );
		EnableWindow( GetDlgItem( m_hWndDlg, IDC_GM_MNBTN_FORBIDCHAT ), bIsEnable );
		EnableWindow( GetDlgItem( m_hWndDlg, IDC_GM_MNBTN_COUNSEL ), bIsEnable );
		EnableWindow( GetDlgItem( m_hWndDlg, IDC_GM_MNBTN_NOTICE ), bIsEnable );
		EnableWindow( GetDlgItem( m_hWndDlg, IDC_GM_MNBTN_EVENT ), bIsEnable );
		EnableWindow( GetDlgItem( m_hWndDlg, IDC_GM_MNBTN_EVENTMAP ), bIsEnable );
		EnableWindow( GetDlgItem( m_hWndDlg, IDC_GM_MNBTN_EVENTNOTIFY ), bIsEnable );
		EnableWindow( GetDlgItem( m_hWndDlg, IDC_GM_MNBTN_WEATHER ), bIsEnable );
		EnableWindow( GetDlgItem( m_hWndDlg, IDC_GM_MNBTN_DUNGEON ), bIsEnable );
*/
	}
	else
	{
		GMTOOLMGR->OnClickMenu( eMenu_Count );
		ShowWindow( m_hWndDlg, SW_HIDE );		
	}

	if( bUpdate )
		m_bShow = bShow;

//임시로 여기위치
	static int bRegist = FALSE;
	int i;
	char bufNum[20];
	//combobox등록
	if( !bRegist )
	{
//		if( m_hWndSub[eMenu_Item] )
		{
			ITEMMGR->SetItemIfoPositionHead();
			ITEM_INFO* pInfo = NULL;
			char cbItemString[MAX_ITEMNAME_LENGTH+16];
			while( (pInfo=ITEMMGR->GetItemInfoData()) != NULL )
			{
				wsprintf( cbItemString, "%s(%d)", pInfo->ItemName, pInfo->ItemIdx );

				if( pInfo->Category == eItemCategory_Equip )
				{
					switch( pInfo->EquipType )
					{
					case eEquipType_Weapon:
						{
//							if( pInfo->Grade == 0 )
								SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_WEAPON, CB_ADDSTRING, 0, (LPARAM)cbItemString );
						}
						break;
					case eEquipType_Armor:			
						{
//							if( pInfo->Grade == 0 )
								SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_CLOTHES, CB_ADDSTRING, 0, (LPARAM)cbItemString );
						}
						break;
					case eEquipType_Accessary:
					case eEquipType_Pet:
						{
							SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_ACCESSORY, CB_ADDSTRING, 0, (LPARAM)cbItemString );
						}
						break;
					}
				}
				else if( pInfo->Category == eItemCategory_Expend )
				{
					SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_POTION, CB_ADDSTRING, 0, (LPARAM)cbItemString );
				}
				else
				{
					SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_ETC, CB_ADDSTRING, 0, (LPARAM)cbItemString );
				}
			}
			SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_WEAPON, CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_CLOTHES, CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_ACCESSORY, CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_POTION, CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_ETC, CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_SKILL, CB_SETCURSEL, 0, 0 );

			EnableWindow( GetDlgItem( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_SKILLSUNG ),		FALSE );

//Grade
			for( i = 0 ; i <= 9 ; ++i )
			{
				wsprintf( bufNum, "+%d", i );
				SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_WEAPONGRADE, CB_ADDSTRING, 0, (LPARAM)bufNum );
				SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_CLOTHESGRADE, CB_ADDSTRING, 0, (LPARAM)bufNum );
			}

			SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_WEAPONGRADE, CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_CLOTHESGRADE, CB_SETCURSEL, 0, 0 );
			SetDlgItemInt( m_hWndSub[eMenu_Item], IDC_GMITEM_EDT_POTION, 1, TRUE );
			SetDlgItemInt( m_hWndSub[eMenu_Item], IDC_GMITEM_EDT_ETC, 1, TRUE );
			SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_SPN_POTION, UDM_SETRANGE, 0, (LPARAM)MAKELONG(50, 1) );
			SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_SPN_ETC, UDM_SETRANGE, 0, (LPARAM)MAKELONG(50, 1) );

		}

		{
			CheckDlgButton(
				m_hWndSub[eMenu_Mob],
				IDC_GMREGEN_SUMMON_MONSTER_CHECK,
				TRUE);
			CheckDlgButton(
				m_hWndSub[eMenu_Mob],
				IDC_GMREGEN_SUMMON_VEHICLE_CHECK,
				TRUE);
			SendMessage(
				m_hWndSub[eMenu_Mob],
				WM_COMMAND,
				IDC_GMREGEN_SUMMON_MONSTER_CHECK,
				BN_CLICKED);

			ITEMMGR->SetItemIfoPositionHead();

			for(const ITEM_INFO* pInfo = ITEMMGR->GetItemInfoData();
				0 < pInfo;
				pInfo = ITEMMGR->GetItemInfoData())
			{
				SendDlgItemMessage( m_hWndSub[eMenu_Mob], IDC_GMREGEN_CMB_ITEM, CB_ADDSTRING, 0, (LPARAM)pInfo->ItemName );
			}
			SendDlgItemMessage( m_hWndSub[eMenu_Mob], IDC_GMREGEN_CMB_ITEM, CB_SETCURSEL, 0, 0 );

			for(MAPTYPE i = 1; i < MAX_MAP_NUM; ++i )
			{
				LPCTSTR pStrMap = GetMapName(i);

				if(0 == _tcsicmp("?", pStrMap))
				{
					continue;
				}

				SendDlgItemMessage( m_hWndSub[eMenu_Mob],		IDC_GMREGEN_CMB_MAP,	CB_ADDSTRING, 0, (LPARAM)pStrMap );
				SendDlgItemMessage( m_hWndSub[eMenu_Move],		IDC_GMMOVE_CMB_MAP,		CB_ADDSTRING, 0, (LPARAM)pStrMap );
				SendDlgItemMessage( m_hWndSub[eMenu_Discon],	IDC_GMDISCON_CMB_MAP,	CB_ADDSTRING, 0, (LPARAM)pStrMap );
				SendDlgItemMessage( m_hWndSub[eMenu_PK],		IDC_GMPK_CMB_MAP,		CB_ADDSTRING, 0, (LPARAM)pStrMap );
				SendDlgItemMessage( m_hWndSub[eMenu_Notice],	IDC_GMNOTICE_CMB_MAP,	CB_ADDSTRING, 0, (LPARAM)pStrMap );

				if(DungeonMGR->IsDungeonMap(MAPTYPE(i)))
				{
					SendDlgItemMessage( m_hWndSub[eMenu_Dungeon],		IDC_GMDUNGEON_CMB_MAPLIST,		CB_ADDSTRING, 0, (LPARAM)pStrMap );
				}
			}

			SendDlgItemMessage( m_hWndSub[eMenu_Mob],		IDC_GMREGEN_CMB_MAP,	CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eMenu_Move],		IDC_GMMOVE_CMB_MAP,		CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eMenu_Discon],	IDC_GMDISCON_CMB_MAP,	CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eMenu_PK],		IDC_GMPK_CMB_MAP,		CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eMenu_Notice],	IDC_GMNOTICE_CMB_MAP,	CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eMenu_Dungeon],	IDC_GMDUNGEON_CMB_MAPLIST,	CB_SETCURSEL, 0, 0 );
		}


#ifdef _TL_LOCAL_
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"Character Exp" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"Item Drop" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"Money Drop" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"---" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"---" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"---" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"---" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"---" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"---" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"Money Amount" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"---" );
#else
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"경험치율" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"아이템드랍율" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"돈드랍율" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"받는데미지율" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"주는데미지율" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"내력소모율" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"운기조식속도" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"파티경험치율" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"특기수련치율" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"돈드랍액수율" );
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_ADDSTRING, 0, (LPARAM)"무공경험치율" );
#endif
		SendDlgItemMessage( m_hWndSub[eMenu_Event],		IDC_GMEVENT_CMB_EVENT,	CB_SETCURSEL, 0, 0 );
		bRegist = TRUE;
	}
}

BOOL CGMToolManager::IsGMDialogMessage( LPMSG pMessage)
{
	if( IsWindow( m_hWndDlg ) && IsDialogMessage( m_hWndDlg, pMessage ) )
		return TRUE;

	if( m_nSelectMenu < eMenu_Count )
	if( m_hWndSub[m_nSelectMenu] )
	{
		if( IsWindow( m_hWndSub[m_nSelectMenu] ) && IsDialogMessage( m_hWndSub[m_nSelectMenu], pMessage ) )
			return TRUE;
	}

	return FALSE;
}

void CGMToolManager::SetPositionByMainWindow()
{
	RECT rcMain, rcDlg;
	GetWindowRect( _g_hWnd, &rcMain );
	GetWindowRect( m_hWndDlg, &rcDlg );
	int nDlgWidth = rcDlg.right - rcDlg.left;
	int X = rcMain.left - nDlgWidth;
	if( X < 0 ) X = rcMain.left;
	int Y = rcMain.top+GetSystemMetrics(SM_CYSIZEFRAME)+GetSystemMetrics(SM_CYCAPTION);
	
	SetWindowPos( m_hWndDlg, NULL, X, Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );

	SetPositionSubDlg();
}

void CGMToolManager::SetPositionSubDlg()
{
	RECT rcDlg;
	GetWindowRect( m_hWndDlg, &rcDlg );

	if( m_nSelectMenu < eMenu_Count )
	{
		if( m_hWndSub[m_nSelectMenu] )
		{
			int subX = rcDlg.right;
			int subY = rcDlg.top + 36 + m_nSelectMenu * 28;

			if(m_nSelectMenu == eMenu_Dungeon)
				subY = rcDlg.top + 36;

			SetWindowPos( m_hWndSub[m_nSelectMenu], NULL, subX, subY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
		}
	}
}

void CGMToolManager::OnClickMenu( int nMenu )
{
	switch( nMenu )
	{
	case eMenu_Move:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		break;
			case eGM_POWER_PATROLLER:	break;
			case eGM_POWER_AUDITOR:		break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			break;
			}
		}
		break;
	case eMenu_Where:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		break;
			case eGM_POWER_PATROLLER:	break;
			case eGM_POWER_AUDITOR:		break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			break;
			}
		}
		break;
	case eMenu_Item:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		return;	break;
			case eGM_POWER_PATROLLER:	return;	break;
			case eGM_POWER_AUDITOR:		return;	break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			return;	break;
			}
		}
		break;
	case eMenu_Hide:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		break;
			case eGM_POWER_PATROLLER:	break;
			case eGM_POWER_AUDITOR:		break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			break;
			}
		}
		break;
	case eMenu_Chat:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		break;
			case eGM_POWER_PATROLLER:	break;
			case eGM_POWER_AUDITOR:		break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			break;
			}
		}
		break;
	case eMenu_PK:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		return;	break;
			case eGM_POWER_PATROLLER:	return;	break;
			case eGM_POWER_AUDITOR:		return;	break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			break;
			}
		}
		break;
	case eMenu_Discon:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		return;	break;
			case eGM_POWER_PATROLLER:	return;	break;
			case eGM_POWER_AUDITOR:		break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			break;
			}
		}
		break;
	case eMenu_ForbidChat:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		return;	break;
			case eGM_POWER_PATROLLER:	return;	break;
			case eGM_POWER_AUDITOR:		break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			return;	break;
			}
		}
		break;
	case eMenu_Mob:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		return;	break;
			case eGM_POWER_PATROLLER:	return;	break;
			case eGM_POWER_AUDITOR:		return;	break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			break;
			}
		}
		break;
	case eMenu_Counsel:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		return;	break;
			case eGM_POWER_PATROLLER:	return;	break;
			case eGM_POWER_AUDITOR:		break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			return;	break;
			}
		}
		break;
	case eMenu_Notice:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		break;
			case eGM_POWER_PATROLLER:	return;	break;
			case eGM_POWER_AUDITOR:		break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			break;
			}
		}
		break;
	case eMenu_Event:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		return;	break;
			case eGM_POWER_PATROLLER:	return;	break;
			case eGM_POWER_AUDITOR:		return;	break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			break;
			}
		}
		break;
	case eMenu_EventMap:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		break;
			case eGM_POWER_PATROLLER:	break;
			case eGM_POWER_AUDITOR:		break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			break;
			}
		}
		break;
	case eMenu_EventNotify:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		break;
			case eGM_POWER_PATROLLER:	return;	break;
			case eGM_POWER_AUDITOR:		break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			break;
			}
		}
		break;
	case eMenu_Weather:
		{
			switch( m_nPower )
			{
			case eGM_POWER_MONITOR:		break;
			case eGM_POWER_PATROLLER:	return;	break;
			case eGM_POWER_AUDITOR:		break;
			case eGM_POWER_EVENTER:		break;
			case eGM_POWER_QA:			break;
			}
		}
		break;
	}

	// 090909 ONS 아이템 윈도우를 활성화하기전 스킬리스트를 설정한다.
	switch( nMenu )
	{
	case eMenu_Move:
		{
			if( !IsDlgButtonChecked(m_hWndDlg, IDC_GM_MNBTN_MOVE )					&&
				IsDlgButtonChecked(m_hWndSub[eMenu_Move],IDC_GMMOVE_BTN_MINIMAP)	&&
				SendMessage( GetDlgItem( m_hWndSub[eMenu_Move], IDC_GMMOVE_CHK_MINIMAP_ON ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
				GMTOOLMGR->ActiveMiniMap( TRUE );
		}
		break;
	case eMenu_Item:
		{
			char cbSkillString[MAX_PATH] = {0};
			const SkillData* pSkillList = GetSkillTreeList();

			SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_SKILL, CB_RESETCONTENT, 0, 0 );

			int nCount = 0;

			for(cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( pSkillList[nCount++].index );
				0 < pSkillInfo;
				pSkillInfo = SKILLMGR->GetSkillInfo( pSkillList[nCount++].index ))
			{
				for (DWORD level=0; level < pSkillList[nCount].level; ++level)
				{
					pSkillInfo = SKILLMGR->GetSkillInfo( pSkillList[nCount].index + level );

					if(0 == pSkillInfo)
					{
						continue;
					}

					wsprintf( cbSkillString, "%s(%d)", pSkillInfo->GetName(), pSkillInfo->GetIndex());

					SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_SKILL, CB_ADDSTRING, 0, (LPARAM)cbSkillString);
				}
			}

			SendDlgItemMessage( m_hWndSub[eMenu_Item],	IDC_GMITEM_CMB_SKILL,	CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eMenu_Item],	IDC_GMITEM_CMB_SKILLSUNG,	CB_SETCURSEL, 0, 0 );
		}
		break;

	case eMenu_Weather:
		{
			// 등록된 날씨 정보를 메뉴에 등록시킨다.
			SendDlgItemMessage( m_hWndSub[eMenu_Weather], IDC_GMWEATHER_CMB_TYPE, CB_RESETCONTENT, 0, 0 );
			WEATHERMGR->AddGMToolMenu();
		}
		break;
	}

	if( IsDlgButtonChecked(m_hWndDlg, IDC_GM_MNBTN_MOVE ) )
	{
		GMTOOLMGR->ActiveMiniMap( FALSE );
	}

	if( nMenu >= eMenu_Count )
	{
		if( m_nSelectMenu < eMenu_Count )
		{
			if( m_hWndSub[m_nSelectMenu] )
				ShowWindow( m_hWndSub[m_nSelectMenu], SW_HIDE );

			CheckDlgButton( m_hWndDlg, IDC_GM_MNBTN_MOVE + m_nSelectMenu, BST_UNCHECKED );			
		}

		m_nSelectMenu = eMenu_Count;
		CheckDlgButton( m_hWndDlg, IDC_GM_MNBTN_NONE, BST_CHECKED );

		m_bSubShow = FALSE;
		SetFocus( _g_hWnd );
	}
	else
	{
		if( m_nSelectMenu == nMenu )
		{
			if( m_hWndSub[m_nSelectMenu] )
				ShowWindow( m_hWndSub[m_nSelectMenu], SW_HIDE );
			CheckDlgButton( m_hWndDlg, IDC_GM_MNBTN_MOVE + m_nSelectMenu, BST_UNCHECKED );

			m_nSelectMenu = eMenu_Count;
			CheckDlgButton( m_hWndDlg, IDC_GM_MNBTN_NONE, BST_CHECKED );

			m_bSubShow = FALSE;
			SetFocus( _g_hWnd );
		}
		else
		{
			ShowWindow( m_hWndSub[m_nSelectMenu], SW_HIDE );
			CheckDlgButton( m_hWndDlg, IDC_GM_MNBTN_MOVE + m_nSelectMenu, BST_UNCHECKED );
			
			m_nSelectMenu = nMenu;
			SetPositionSubDlg();
			CheckDlgButton( m_hWndDlg, IDC_GM_MNBTN_MOVE + m_nSelectMenu, BST_CHECKED );

			if( m_hWndSub[m_nSelectMenu] )
				AnimateWindow( m_hWndSub[m_nSelectMenu], 200, AW_SLIDE | AW_HOR_POSITIVE );

			m_bSubShow = TRUE;
		}
	}	
}

BOOL CGMToolManager::OnGMMoveCommand( int nMethod )
{
	if( !HERO ) return FALSE;
	if( !m_hWndSub[eMenu_Move] ) return FALSE;
	SetFocus( _g_hWnd );

	if( nMethod == 0 )
	{
		BOOL	bRt;
		VECTOR3 pos;
		pos.x = GetDlgItemInt( m_hWndSub[eMenu_Move], IDC_GMMOVE_EDT_X, &bRt, TRUE ) * 100.0f;
		if( !bRt ) return FALSE;
		pos.y = 0;
		pos.z = GetDlgItemInt( m_hWndSub[eMenu_Move], IDC_GMMOVE_EDT_Y, &bRt, TRUE ) * 100.0f;
		if( !bRt ) return FALSE;

		MOVE_POS msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_MOVE_SYN;
		msg.dwObjectID	= gHeroID;
		msg.dwMoverID	= gHeroID;
		msg.cpos.Compress(&pos);
		NETWORK->Send(&msg,sizeof(msg));
	}
	else if( nMethod == 1 )
	{
		char buf[MAX_NAME_LENGTH+1] = {0,};
		int nLen = GetDlgItemText( m_hWndSub[eMenu_Move], IDC_GMMOVE_EDT_NAME, buf, MAX_NAME_LENGTH+1 );
		if( nLen > MAX_NAME_LENGTH || nLen < 4 ) return FALSE;

		if( strncmp( HERO->GetObjectName(), buf, MAX_NAME_LENGTH+1 ) == 0  )	//자기 자신
			return FALSE;

		MSG_NAME_DWORD msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_MOVETOCHAR_SYN;
		msg.dwObjectID	= gHeroID;
		msg.dwData		= 0;
		SafeStrCpy( msg.Name, buf, MAX_NAME_LENGTH + 1 );
		NETWORK->Send(&msg, sizeof(msg));
	}
	else if( nMethod == 2)
	{
		char buf[MAX_MAP_NAME_LENGTH+1] = {0,};
		//int nLen = GetDlgItemText( m_hWndSub[eMenu_Move], IDC_GMMOVE_EDT_MAP, buf, MAX_NAME_LENGTH+1);
		int nLen = GetDlgItemText( m_hWndSub[eMenu_Move], IDC_GMMOVE_CMB_MAP, buf, MAX_MAP_NAME_LENGTH+1 );
		if( nLen > MAX_MAP_NAME_LENGTH ) return FALSE;

		WORD wMapNum = GetMapNumForName( buf );
		if( wMapNum == 0 ) return FALSE;

		//BOOL bRt;
		//int nChannel = GetDlgItemInt( m_hWndSub[eMenu_Move], IDC_GMMOVE_EDT_CHANNEL, &bRt, TRUE );
		//int nChannel = GetDlgItemInt( m_hWndSub[eMenu_Move], IDC_GMMOVE_CMB_CHANNEL, &bRt, TRUE );
		int nChannel = SendDlgItemMessage( m_hWndSub[eMenu_Move], IDC_GMMOVE_CMB_CHANNEL, CB_GETCURSEL, 0, 0 ) + 1;

		//if( bRt )
		//if( m_cbChannelCount > 0 && nChannel > 0 && nChannel <= m_cbChannelCount )
		if(0<nChannel &&nChannel<=g_MapChannelNum[wMapNum])
		{
			nChannel = nChannel - 1; //0부터 시작하는 인덱스라 -1
		}

		MSG_NAME_DWORD2 msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_CHANGEMAP_SYN;
		msg.dwObjectID	= gHeroID;
		msg.dwData1		= (DWORD)wMapNum;
		msg.dwData2		= (DWORD)nChannel;
		msg.Name[0]		= 0;	//hero
		NETWORK->Send( &msg, sizeof(msg) );
	}
	else if( nMethod == 3)
	{
		// 100427 ONS NPC로의 이동처리 추가
		int nIndex = SendDlgItemMessage( m_hWndSub[eMenu_Move], IDC_GMMOVE_CMB_NPC, CB_GETCURSEL, 0, 0 );
		VECTOR3 pos = m_NpcList[nIndex].Pos;

		MOVE_POS msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_MOVE_SYN;
		msg.dwObjectID	= gHeroID;
		msg.dwMoverID	= gHeroID;
		msg.cpos.Compress(&pos);
		NETWORK->Send(&msg,sizeof(msg));
	}

	return TRUE;
}

BOOL CGMToolManager::OnUserMoveCommand( int nMethod )
{
	if( !m_hWndSub[eMenu_Move] ) return FALSE;
	SetFocus( _g_hWnd );

	char UserName[MAX_NAME_LENGTH+1] = {0,};
	int nLength = GetDlgItemText( m_hWndSub[eMenu_Move], IDC_GMMOVE_EDT_USER, UserName, MAX_NAME_LENGTH+1 );
	if( nLength > MAX_NAME_LENGTH || nLength < 4  ) return FALSE;
	if( (FILTERTABLE->IsInvalidCharInclude((unsigned char*)UserName)) == TRUE )
		return FALSE;

	if( nMethod == 0 )
	{
		BOOL	bRt;
		VECTOR3 pos;
		pos.x = GetDlgItemInt( m_hWndSub[eMenu_Move], IDC_GMMOVE_EDT_X, &bRt, TRUE ) * 100.0f;
		if( !bRt ) return FALSE;
		pos.y = 0;
		pos.z = GetDlgItemInt( m_hWndSub[eMenu_Move], IDC_GMMOVE_EDT_Y, &bRt, TRUE ) * 100.0f;
		if( !bRt ) return FALSE;

		MOVE_POS_USER msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_MOVEUSER_SYN;
		msg.dwObjectID	= gHeroID;
		SafeStrCpy( msg.Name, UserName, MAX_NAME_LENGTH + 1 );
		msg.cpos.Compress(&pos);
		NETWORK->Send(&msg,sizeof(msg));
	}
	else if( nMethod == 1 )
	{
		if( strncmp( HERO->GetObjectName(), UserName, MAX_NAME_LENGTH ) == 0  )	//자기 자신
			return FALSE;

		MSG_NAME_DWORD msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_MOVETOCHAR_SYN;
		msg.dwObjectID	= gHeroID;
		msg.dwData		= 1;		//데려오기
		SafeStrCpy( msg.Name, UserName, MAX_NAME_LENGTH + 1 );
		NETWORK->Send(&msg, sizeof(msg));

	}
	else if( nMethod == 2 )
	{
		char buf[MAX_MAP_NAME_LENGTH+1] = {0,};
		int nLen = GetDlgItemText( m_hWndSub[eMenu_Move], IDC_GMMOVE_CMB_MAP, buf, MAX_MAP_NAME_LENGTH+1 );
		if( nLen > MAX_MAP_NAME_LENGTH ) return FALSE;
		WORD wMapNum = GetMapNumForName( buf );
		int nChannel = SendDlgItemMessage( m_hWndSub[eMenu_Move], IDC_GMMOVE_CMB_CHANNEL, CB_GETCURSEL, 0, 0 ) + 1;
		if(0<nChannel &&nChannel<=g_MapChannelNum[wMapNum])
		{
			nChannel = nChannel - 1;
		}

		MSG_NAME_DWORD2 msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_CHANGEMAP_SYN;
		msg.dwObjectID	= gHeroID;
		msg.dwData1		= (DWORD)wMapNum;
		msg.dwData2		= (DWORD)nChannel;
		SafeStrCpy( msg.Name, UserName, MAX_NAME_LENGTH + 1 );
		NETWORK->Send( &msg, sizeof(msg) );
	}
	else if( nMethod == 3 )
	{
		int nIndex = SendDlgItemMessage( m_hWndSub[eMenu_Move], IDC_GMMOVE_CMB_NPC, CB_GETCURSEL, 0, 0 );
		VECTOR3 pos = m_NpcList[nIndex].Pos;

		MOVE_POS_USER msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_MOVEUSER_SYN;
		msg.dwObjectID	= gHeroID;
		SafeStrCpy( msg.Name, UserName, MAX_NAME_LENGTH + 1 );
		msg.cpos.Compress(&pos);
		NETWORK->Send(&msg,sizeof(msg));
	}

	return TRUE;
}

BOOL CGMToolManager::OnItemCommand()
{
	if( !m_hWndSub[eMenu_Item] ) return FALSE;
	SetFocus( _g_hWnd );

	int nResult = 0;
	WORD wCount = 1;
	char buf[64];
	DWORD idx = (DWORD)-1;

	if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Item], IDC_GMITEM_BTN_WEAPON ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		GetDlgItemText( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_WEAPON, buf, MAX_ITEMNAME_LENGTH+16 );
		sscanf(strrchr(buf,'('), "(%lu)", &idx);

		int nSel = SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_WEAPONGRADE, CB_GETCURSEL, 0, 0 );
		if( nSel > 0 && nSel <= 9 )
			wsprintf( buf, "%s+%d", buf, nSel );

		nResult = 1;	//1:Item
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Item], IDC_GMITEM_BTN_CLOTHES ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		GetDlgItemText( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_CLOTHES, buf, MAX_ITEMNAME_LENGTH+16 );
		sscanf(strrchr(buf,'('), "(%lu)", &idx);

		int nSel = SendDlgItemMessage( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_CLOTHESGRADE, CB_GETCURSEL, 0, 0 );
		if( nSel > 0 && nSel <= 9 )
			wsprintf( buf, "%s+%d", buf, nSel );
		nResult = 1;	//1:Item
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Item], IDC_GMITEM_BTN_ACCESSORY ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		GetDlgItemText( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_ACCESSORY, buf, MAX_ITEMNAME_LENGTH+16 );
		sscanf(strrchr(buf,'('), "(%lu)", &idx);

		nResult = 1;	//1:Item
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Item], IDC_GMITEM_BTN_POTION ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		GetDlgItemText( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_POTION, buf, MAX_ITEMNAME_LENGTH+16 );
		sscanf(strrchr(buf,'('), "(%lu)", &idx);

		BOOL rt;
		int nCount = GetDlgItemInt( m_hWndSub[eMenu_Item], IDC_GMITEM_EDT_POTION, &rt, TRUE );
		if( rt )
		{
			if( nCount < 0 )			nCount = 0;
			else if( nCount > 99 )		nCount = 99;

			wCount = (WORD)nCount;
			nResult = 1;	//1:Item;
		}
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Item], IDC_GMITEM_BTN_ETC ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		GetDlgItemText( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_ETC, buf, MAX_ITEMNAME_LENGTH+16 );
		sscanf(strrchr(buf,'('), "(%lu)", &idx);

		BOOL rt;
		int nCount = GetDlgItemInt( m_hWndSub[eMenu_Item], IDC_GMITEM_EDT_ETC, &rt, TRUE );
		if( rt )
		{
			if( nCount < 0 )			nCount = 0;
			else if( nCount > 50 )		nCount = 50;

			wCount = (WORD)nCount;
			nResult = 1;	//1:Item;
		}
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Item], IDC_GMITEM_BTN_MONEY ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		BOOL rt;
		DWORD dwMoney = GetDlgItemInt( m_hWndSub[eMenu_Item], IDC_GMITEM_EDT_MONEY, &rt, TRUE );
		if( !rt )	return FALSE;

		SetDlgItemInt( m_hWndSub[eMenu_Item], IDC_GMITEM_EDT_MONEY, 0, TRUE );

		MSG_DWORD msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_MONEY_SYN;
		msg.dwObjectID	= gHeroID;
		msg.dwData		= dwMoney;
		NETWORK->Send( &msg, sizeof(msg) );		
	}
	// 090831 ONS 스킬포인트 획득 추가	
	else if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Item], IDC_GMITEM_BTN_SKILLPOINT ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		BOOL rt;
		DWORD dwpoint = GetDlgItemInt( m_hWndSub[eMenu_Item], IDC_GMITEM_EDT_SKILLPOINT, &rt, TRUE );
		if( !rt )	return FALSE;

		SetDlgItemInt( m_hWndSub[eMenu_Item], IDC_GMITEM_EDT_SKILLPOINT, 0, TRUE );

		MSG_DWORD msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_SKILLPOINT_SYN;
		msg.dwObjectID	= gHeroID;
		msg.dwData		= dwpoint;
		NETWORK->Send( &msg, sizeof(msg) );		
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Item], IDC_GMITEM_BTN_SKILL ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		// 090909 ONS 스킬획득 기능 추가
		GetDlgItemText( m_hWndSub[eMenu_Item], IDC_GMITEM_CMB_SKILL, buf, MAX_ITEMNAME_LENGTH+16 );
		sscanf(strrchr(buf,'('), "(%lu)", &idx);

		cSkillInfo* pInfo = SKILLMGR->GetSkillInfo( idx );
		if( pInfo == NULL ) return FALSE;

		MSG_DWORD_WORD msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_UPDATE_SKILL_SYN;
		msg.dwObjectID	= gHeroID;
		msg.dwData		= (pInfo->GetIndex()/100)*100 +1;
		msg.wData		= WORD(pInfo->GetIndex() % 100);
		NETWORK->Send(&msg,sizeof(msg));
	}
	if( nResult == 1 )
	{
		if (idx == -1) return FALSE;

		ITEM_INFO* pInfo = ITEMMGR->GetItemInfo(idx);
		if( pInfo == NULL ) return FALSE;

		MSG_DWORD_WORD msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_ITEM_SYN;
		msg.dwObjectID	= gHeroID;
		msg.dwData		= pInfo->ItemIdx;
		msg.wData		= wCount;

		NETWORK->Send(&msg,sizeof(msg));
	}

	return TRUE;
}

BOOL CGMToolManager::OnDisconCommand()
{
	if( !m_hWndSub[eMenu_Discon] ) return FALSE;
	SetFocus( _g_hWnd );

	char buf[MAX_NAME_LENGTH+1] = { 0, };

	if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Discon], IDC_GMDISCON_BTN_USER ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		int nLen = GetDlgItemText( m_hWndSub[eMenu_Discon], IDC_GMDISCON_EDT_CHAR, buf, MAX_NAME_LENGTH+1 );
		if( nLen < 4 || nLen > MAX_NAME_LENGTH )
			return FALSE;

		if( strncmp( HERO->GetObjectName(), buf, MAX_NAME_LENGTH ) == 0  )
			return FALSE;
		
		if( (FILTERTABLE->IsInvalidCharInclude((unsigned char*)buf)) == TRUE )
			return FALSE;

		MSG_NAME msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_BANCHARACTER_SYN;
		msg.dwObjectID	= gHeroID;
		SafeStrCpy( msg.Name, buf, MAX_NAME_LENGTH + 1 );
		NETWORK->Send( &msg,sizeof(msg) );
	}
	else	//map전체
	{
		int nLen = GetDlgItemText( m_hWndSub[eMenu_Discon], IDC_GMDISCON_CMB_MAP, buf, MAX_NAME_LENGTH+1 );
		if( nLen > MAX_NAME_LENGTH )
			return FALSE;
		
		WORD wMapNum = GetMapNumForName( buf );
		if( wMapNum == 0 ) return FALSE;

		WORD wExceptSelf = 0;
		if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Discon], IDC_GMDISCON_BTN_EXCEPTSELF ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
			wExceptSelf = 1;
		
		//channel도 구분할 필요가 있는가... Agent가 채널을 알고 있나?
		MSG_WORD2 msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_BANMAP_SYN;
		msg.dwObjectID	= gHeroID;
		msg.wData1		= wMapNum;
		msg.wData2		= wExceptSelf;
		NETWORK->Send( &msg, sizeof(msg) );
	}

	return TRUE;
}

BOOL CGMToolManager::OnForbidChatCommand( int nMethod )
{
	if( !HERO || 
		!m_hWndSub[eMenu_ForbidChat] )
	{
		return FALSE;
	}
	SetFocus( _g_hWnd );

	char Name[MAX_NAME_LENGTH+1] = { 0, };
	int nLen = GetDlgItemText( m_hWndSub[eMenu_ForbidChat], IDC_GMCHAT_EDT_USER, Name, MAX_NAME_LENGTH+1 );
	if( strncmp( HERO->GetObjectName(), Name, MAX_NAME_LENGTH ) == 0	||
		nLen < 4														|| 
		nLen > MAX_NAME_LENGTH											||
		(FILTERTABLE->IsInvalidCharInclude((unsigned char*)Name)) == TRUE )
	{
		return FALSE;
	}

	if( 1 == nMethod )
	{
		char Reason[MAX_CHAT_LENGTH+1] = { 0, };
		nLen = GetDlgItemText( m_hWndSub[eMenu_ForbidChat], IDC_GMCHAT_EDT_REASON, Reason, MAX_CHAT_LENGTH+1 );
		if( nLen < 1 || nLen > 100 ) 
		{
			return FALSE;
		}

		BOOL rt = FALSE;
		const int nSecond = GetDlgItemInt( m_hWndSub[eMenu_ForbidChat], IDC_GMCHAT_EDT_SEC, &rt, TRUE);
		if(FALSE == rt)
		{
			return FALSE;
		}

		MSG_FORBID_CHAT msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_FORBID_CHAT_SYN;
		msg.dwObjectID	= gHeroID;
		msg.dwData		= nSecond;
		SafeStrCpy( msg.Name, Name, MAX_NAME_LENGTH+1 );
		SafeStrCpy( msg.Reason, Reason, MAX_CHAT_LENGTH+1 );
		NETWORK->Send(&msg,sizeof(msg));
	}
	else
	{
		MSG_NAME msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_PERMIT_CHAT_SYN;
		msg.dwObjectID	= gHeroID;
		SafeStrCpy( msg.Name, Name, MAX_NAME_LENGTH+1 );
		NETWORK->Send(&msg,sizeof(msg));
	}

	return TRUE;
}

BOOL CGMToolManager::OnWhereCommand()
{
	if( !m_hWndSub[eMenu_Where] ) return FALSE;
	SetFocus( _g_hWnd );

	MSG_NAME msg;
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_WHEREIS_SYN;
	msg.dwObjectID	= gHeroID;

	int nLen = GetDlgItemText( m_hWndSub[eMenu_Where], IDC_GMWHERE_EDT_USER, msg.Name, MAX_NAME_LENGTH+1 );

	if( nLen < 4 || nLen > MAX_NAME_LENGTH ) return FALSE;

	msg.Name[MAX_NAME_LENGTH] = 0;
	
	if( (FILTERTABLE->IsInvalidCharInclude((unsigned char*)msg.Name)) == TRUE )
	{
		return TRUE;
	}
	NETWORK->Send( &msg, sizeof(msg) );
	
//초기화
	SetDlgItemText( m_hWndSub[eMenu_Where], IDC_GMWHERE_EDT_MAP, "" );
	SetDlgItemInt( m_hWndSub[eMenu_Where], IDC_GMWHERE_EDT_CHANNEL, 0, TRUE );
	SetDlgItemInt( m_hWndSub[eMenu_Where], IDC_GMWHERE_EDT_X, 0, TRUE );
	SetDlgItemInt( m_hWndSub[eMenu_Where], IDC_GMWHERE_EDT_Y, 0, TRUE );
//
	return TRUE;
}

void CGMToolManager::DisplayWhereInfo( char* MapName, int nChannel, int nX, int nY )
{
	if( !m_hWndSub[eMenu_Where] ) return;
	SetFocus( _g_hWnd );

	SetDlgItemText( m_hWndSub[eMenu_Where], IDC_GMWHERE_EDT_MAP, MapName );
	SetDlgItemInt( m_hWndSub[eMenu_Where], IDC_GMWHERE_EDT_CHANNEL, nChannel, FALSE );
	SetDlgItemInt( m_hWndSub[eMenu_Where], IDC_GMWHERE_EDT_X, nX, FALSE );
	SetDlgItemInt( m_hWndSub[eMenu_Where], IDC_GMWHERE_EDT_Y, nY, FALSE );
}

BOOL CGMToolManager::OnHideCommand()
{
	if( !m_hWndSub[eMenu_Hide] ) return FALSE;
	SetFocus( _g_hWnd );

//	char buf[MAX_NAME_LENGTH+1] = {0,};
	int  nLen	= 0;
	BOOL bHide = TRUE;

	if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Hide], IDC_GMHIDE_BTN_HIDE ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
		bHide = TRUE;
	else
		bHide = FALSE;

	// 06. 05 HIDE NPC - 이영준
	if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Hide], IDC_GMHIDE_BTN_NPC ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		CObject* pObject = OBJECTMGR->GetSelectedObject();
		if( !pObject )	return FALSE;
		if( pObject->GetObjectKind() != eObjectKind_Npc )	return FALSE;

		MSG_WORD3 msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_NPCHIDE_SYN;
		msg.dwObjectID	= gHeroID;
		msg.wData1 = ((CNpc*)pObject)->GetNpcUniqueIdx();
		msg.wData2 = (WORD)SendDlgItemMessage( m_hWndSub[eMenu_Hide], IDC_GMHIDE_CMB_CHANNEL, CB_GETCURSEL, 0, 0 );
		msg.wData3 = !bHide;

		NETWORK->Send( &msg, sizeof(msg) );
		return TRUE;
	}
	else
	{
		MSG_NAME_DWORD msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_HIDE_SYN;
		msg.dwObjectID	= gHeroID;
		msg.dwData		= (DWORD)bHide;

		if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Hide], IDC_GMHIDE_BTN_USER ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )	
		{
			nLen = GetDlgItemText( m_hWndSub[eMenu_Hide], IDC_GMHIDE_EDT_USER, msg.Name, MAX_NAME_LENGTH+1 );
			if( nLen < 4 || nLen > MAX_NAME_LENGTH )
				return FALSE;
		}
		else
		{
			msg.Name[0] = 0;	//hero
		}

		msg.Name[MAX_NAME_LENGTH] = 0;
		NETWORK->Send( &msg, sizeof(msg) );

		return TRUE;
	}
}

BOOL CGMToolManager::OnPKCommand()
{
	if( !m_hWndSub[eMenu_PK] ) return FALSE;
	SetFocus( _g_hWnd );

	char buf[MAX_NAME_LENGTH+1] = {0,};
	int nLen = GetDlgItemText( m_hWndSub[eMenu_PK], IDC_GMPK_CMB_MAP, buf, MAX_NAME_LENGTH+1 );
	if( nLen > MAX_NAME_LENGTH ) return FALSE;

	WORD wMapNum = GetMapNumForName( buf );
	if( wMapNum == 0 ) return FALSE;

	MSG_WORD2 msg;
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_PKALLOW_SYN;
	msg.dwObjectID	= gHeroID;
	msg.wData1		= wMapNum;

	if( SendMessage( GetDlgItem( m_hWndSub[eMenu_PK], IDC_GMPK_BTN_NOPK ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		msg.wData2	= 0;		//not allow
	}
	else
	{
		msg.wData2	= 1;		//allow		
	}

	NETWORK->Send(&msg,sizeof(msg));	

	return TRUE;
}

BOOL CGMToolManager::OnRegenCommand()
{
	if( !m_hWndSub[eMenu_Mob] ) return FALSE;
	SetFocus( _g_hWnd );

	char buf[MAX_ITEMNAME_LENGTH+1] = {0};

	if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Mob], IDC_GMREGEN_BTN_MODEDIRECT ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		const HWND menuHandle = m_hWndSub[eMenu_Mob];
		const int nSelectedSel = SendMessage(
			GetDlgItem(menuHandle, IDC_GMREGEN_CMB_MOBNAME),
			CB_GETCURSEL,
			0,
			0);
		const DWORD monsterKind = SendMessage(
			GetDlgItem(menuHandle, IDC_GMREGEN_CMB_MOBNAME),
			CB_GETITEMDATA,
			nSelectedSel,
			0);

		BOOL rt = FALSE;
		const int nMobCount = GetDlgItemInt(
			menuHandle,
			IDC_GMREGEN_EDT_MOBNUM,
			&rt,
			TRUE);

		if(FALSE == rt)
		{
			return FALSE;
		}
		else if(0 == GetDlgItemText(
			menuHandle,
			IDC_GMREGEN_CMB_MAP,
			buf,
			sizeof(buf) / sizeof(*buf)))
		{
			return FALSE;
		}

		const MAPTYPE wMapNum = GetMapNumForName(
			buf);

		if(0 == wMapNum)
		{
			return FALSE;
		}

		const int nChannel = SendDlgItemMessage(
			menuHandle,
			IDC_GMREGEN_CMB_CHANNEL,
			CB_GETCURSEL,
			0,
			0);

		if(0 > nChannel)
		{
			return FALSE;
		}
		else if(nChannel > g_MapChannelNum[wMapNum])
		{
			return FALSE;
		}

		const float nX = float(GetDlgItemInt(
			menuHandle,
			IDC_GMREGEN_EDT_X,
			&rt,
			TRUE));

		if(FALSE == rt)
		{
			return FALSE;
		}

		const float nY = float(GetDlgItemInt(
			menuHandle,
			IDC_GMREGEN_EDT_Y,
			&rt,
			TRUE));

		if(FALSE == rt)
		{
			return FALSE;
		}

		const int nRad = GetDlgItemInt(
			menuHandle,
			IDC_GMREGEN_EDT_RAD,
			&rt,
			TRUE );

		if(FALSE == rt)
		{
			return FALSE;
		}
		else if(0 == GetDlgItemText(
			menuHandle,
			IDC_GMREGEN_CMB_ITEM,
			buf,
			sizeof(buf) / sizeof(*buf)))
		{
			return FALSE;
		}

		const ITEM_INFO* const pInfo = ITEMMGR->FindItemInfoForName(
			buf);

		if(0 == pInfo)
		{
			return FALSE;
		}

		const int nDropRatio = GetDlgItemInt(
			menuHandle,
			IDC_GMREGEN_EDT_ITEM,
			&rt,
			TRUE);

		const VehicleScript& vehicleScript = GAMERESRCMNGR->GetVehicleScript(
			monsterKind);

		if(0 == vehicleScript.mMonsterKind)
		{
			MSG_EVENT_MONSTERREGEN message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_CHEAT;
			message.Protocol = MP_CHEAT_EVENT_MONSTER_REGEN;
			message.dwObjectID = gHeroID;
			message.MonsterKind = WORD(monsterKind);
			message.cbMobCount = BYTE(min(100, max(nMobCount, 1)));
			message.wMap = wMapNum;
			message.cbChannel = (BYTE)nChannel;
			message.Pos.x = max(0, nX * 100);
			message.Pos.z = max(0, nY * 100);
			message.wRadius	= WORD(min(10000, max(0, nRad * 100)));
			message.ItemID = pInfo->ItemIdx;
			message.dwDropRatio	= min(100, max(0, nDropRatio));

			NETWORK->Send(
				&message,
				sizeof(message));
		}
		else
		{
			MSG_DWORD4 message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_CHEAT;
			message.Protocol = MP_CHEAT_VEHICLE_SUMMON_SYN;
			message.dwObjectID = gHeroID;
			message.dwData1 = monsterKind;
			message.dwData2 = DWORD(nX * 100);
			message.dwData3 = DWORD(nY * 100);
			NETWORK->Send(
				&message,
				sizeof(message));
		}

		return TRUE;
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Mob], IDC_GMREGEN_BTN_MODEFILE ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		char lpstrFile[MAX_PATH] = {0,};
		GetDlgItemText( m_hWndSub[eMenu_Mob], IDC_GMREGEN_EDT_FILE, lpstrFile, MAX_PATH );
	
		if( lpstrFile[0] == 0 )
			return FALSE;
		
		CMHFile fp;		
		if( !fp.Init( lpstrFile, "rt" ) )
			return FALSE;
		char buff[256]={0,};

		while(1)
		{
			fp.GetString(buff);
			if(fp.IsEOF())
				break;
			if(buff[0] == '@')
			{
				fp.GetLineX(buff, 256);
				continue;
			}
			if( strcmp( buff, "$REGEN" ) == 0 )
			{
				DWORD dwGroup = fp.GetDword();
				if( dwGroup == 0 )
				{
					fp.Release();
					return FALSE;
				}
				if((fp.GetString())[0] == '{')
				{
					MSG_EVENT_MONSTERREGEN msg;
					msg.Category	= MP_CHEAT;
					msg.Protocol	= MP_CHEAT_EVENT_MONSTER_REGEN;
					msg.dwObjectID	= gHeroID;
					msg.MonsterKind = WORD(GAMERESRCMNGR->GetMonsterIndexForName(fp.GetString()));
					msg.cbMobCount	= fp.GetByte();
					msg.wMap		= GetMapNumForName( fp.GetString() );
					msg.cbChannel	= fp.GetByte();
					msg.Pos.x		= (float)fp.GetWord() * 100.0f;
					msg.Pos.z		= (float)fp.GetWord() * 100.0f;
					msg.wRadius		= fp.GetWord()*100;
					ITEM_INFO* pInfo = ITEMMGR->FindItemInfoForName( fp.GetString() );
					if( pInfo )
						msg.ItemID		= pInfo->ItemIdx;
					else
						msg.ItemID		= 0;
					msg.dwDropRatio	= fp.GetDword();

					//if( msg.MonsterKind == 0 || msg.wMap == 0 || msg.cbChannel > m_cbChannelCount
					if( msg.MonsterKind == 0 || msg.wMap == 0 || msg.cbChannel > g_MapChannelNum[msg.wMap]
						|| ( msg.ItemID == 0 && msg.dwDropRatio != 0 ) )
					{
						ASSERT(0);
					}
					else
					{
						NETWORK->Send( &msg,sizeof(msg) );
					}					
				}
				else
				{
					fp.Release();
					return FALSE;
				}

				if((fp.GetString())[0] != '}')
				{
					fp.Release();
					return FALSE;
				}
			}
		}
		fp.Release();
		return TRUE;
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Mob], IDC_GMREGEN_BTN_DELETE ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		MSGBASE msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_EVENT_MONSTER_DELETE;
		msg.dwObjectID	= gHeroID;

		NETWORK->Send( &msg, sizeof(msg) );
		return TRUE;
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Mob], IDC_GMREGEN_BTN_UNSUMMON_VEHICLE ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		TCHAR text[MAX_PATH] = {0};
		GetDlgItemText(
			m_hWndSub[eMenu_Mob],
			IDC_GMREGEN_VEHICLE_DROPDOWN,
			text,
			sizeof(text) / sizeof(*text));

		LPCTSTR textSeperator = "[], ";
		LPCTSTR textPositionX = _tcstok(
			text,
			textSeperator);
		LPCTSTR textPositionY = _tcstok(
			0,
			textSeperator);

		MSG_DWORD2 message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_CHEAT;
		message.Protocol = MP_CHEAT_VEHICLE_UNSUMMON_SYN;
		message.dwObjectID = gHeroID;
		message.dwData1 = _ttoi(
			textPositionX ? textPositionX : "") * 100;
		message.dwData2 = _ttoi(
			textPositionY ? textPositionY : "") * 100;
		NETWORK->Send(
			&message,
			sizeof(message));

		return TRUE;
	}
	else if(SendMessage(GetDlgItem(m_hWndSub[eMenu_Mob], IDC_GMREGEN_SCRIPT_RADIO), BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		TCHAR textObject[MAX_PATH] = {0};
		GetDlgItemText(
			m_hWndSub[eMenu_Mob],
			IDC_SCRIPT_OBJECT_EDIT,
			textObject,
			_countof(textObject));

		TCHAR textFileName[MAX_PATH] = {0};
		GetDlgItemText(
			m_hWndSub[eMenu_Mob],
			IDC_SCRIPT_FILE_EDIT,
			textFileName,
			_countof(textFileName));

		LPCTSTR textSeperator = " ";
		LPCTSTR textObjectIndex = _tcstok(
			textObject,
			textSeperator);

		MSG_GUILDNOTICE message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_CHEAT;
		message.Protocol = MP_CHEAT_MONSTER_SCRIPT_SYN;
		message.dwObjectID = gHeroID;
		message.dwGuildId = _ttoi(
			textObjectIndex ? textObjectIndex : "");
		SafeStrCpy(
			message.Msg,
			textFileName,
			_countof(message.Msg));
		NETWORK->Send(
			&message,
			sizeof(message));
	}

	return FALSE;
}

#define MAX_PREFACE_LENGTH	30

BOOL CGMToolManager::OnNoticeCommand()
{
	if( !m_hWndSub[eMenu_Notice] ) return FALSE;
	SetFocus( _g_hWnd );

	char buf[MAX_CHAT_LENGTH+1] = {0,};
	char bufMap[MAX_NAME_LENGTH+1] = {0,};

	int nLen = GetWindowText( GetDlgItem( m_hWndSub[eMenu_Notice], IDC_GMNOTICE_EDT_PREFACE ), buf, MAX_PREFACE_LENGTH+1 );
	int nLen2 = GetWindowText( GetDlgItem( m_hWndSub[eMenu_Notice], IDC_GMNOTICE_EDT_NOTICE ), buf+nLen, MAX_CHAT_LENGTH-MAX_PREFACE_LENGTH );
	
	if( nLen2 == 0 ) return FALSE;
//	if( strlen( buf ) == 0 ) return FALSE;

	MSG_CHAT_WORD msg;
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_NOTICE_SYN;
	msg.dwObjectID	= gHeroID;
	SafeStrCpy( msg.Msg, buf, MAX_CHAT_LENGTH+1 );

	if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Notice], IDC_GMNOTICE_BTN_ALLMAP ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		msg.wData = 0;
	}
	else
	{
		GetDlgItemText( m_hWndSub[eMenu_Notice], IDC_GMNOTICE_CMB_MAP, bufMap, MAX_NAME_LENGTH+1 );
		msg.wData = GetMapNumForName(bufMap);
	}

	NETWORK->Send( &msg, msg.GetMsgLength() );

	SetWindowText( GetDlgItem( m_hWndSub[eMenu_Notice], IDC_GMNOTICE_EDT_NOTICE ), "" );
	SetFocus( GetDlgItem( m_hWndSub[eMenu_Notice], IDC_GMNOTICE_EDT_NOTICE ) );

	return TRUE;
}

BOOL CGMToolManager::OnEventCommand()
{
	if( !m_hWndSub[eMenu_Notice] ) return FALSE;
	SetFocus( _g_hWnd );


	//정보 읽어 오기
	int nEvent = SendDlgItemMessage( m_hWndSub[eMenu_Event], IDC_GMEVENT_CMB_EVENT, CB_GETCURSEL, 0, 0 )+1;
	BOOL rt;
	int nRatio = GetDlgItemInt( m_hWndSub[eMenu_Event], IDC_GMEVENT_EDT_RATE, &rt, TRUE );

	// 071210 LYW --- GMToolManager : 이벤트 경험치만 기능을 살린다.
	// 경험치 최고 30배로 제한을 둔다.
	if( nRatio > 3000 ) 
	{
		MessageBox( NULL, CHATMGR->GetChatMsg(1476), "GM-Tool", MB_OK) ;
		return FALSE ;
	}

	MSG_WORD2 msg;
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_EVENT_SYN;
	msg.dwObjectID	= gHeroID;
	msg.wData1		= WORD(nEvent);
	msg.wData2		= WORD(nRatio);
	NETWORK->Send( &msg, sizeof(msg) );
	CHATMGR->AddMsg( CTC_TOGM, "Event Applied!" );

	return TRUE;
}

BOOL CGMToolManager::OnEventNotifyCommand( BOOL bOn )
{
	if( !m_hWndSub[eMenu_EventNotify] ) return FALSE;
	SetFocus( _g_hWnd );

	if( bOn )
	{
		//정보 읽어 오기
		char bufTitle[32] = {0,};
		char bufContext[128] = {0,};
		int nLen = 0;

		nLen = GetWindowText( GetDlgItem( m_hWndSub[eMenu_EventNotify], IDC_GMEVENTNOTIFY_EDT_TITLE ), bufTitle, 32 );
		if( nLen <=0 )
		{
			CHATMGR->AddMsg( CTC_TOGM, "Input Notify Title!" );
			return FALSE;
		}
		nLen = GetWindowText( GetDlgItem( m_hWndSub[eMenu_EventNotify], IDC_GMEVENTNOTIFY_EDT_CONTEXT ), bufContext, 128 );
		if( nLen <=0 )
		{
			CHATMGR->AddMsg( CTC_TOGM, "Input Notify Context!" );
			return FALSE;
		}

		if( !m_hWndSub[eMenu_Notice] ) return FALSE;
		SetFocus( _g_hWnd );
		//정보 읽어 오기
		int nEvent = SendDlgItemMessage( m_hWndSub[eMenu_Event], IDC_GMEVENT_CMB_EVENT, CB_GETCURSEL, 0, 0 )+1;
	
		bufTitle[31] = 0;
		bufContext[127] = 0;

		MSG_EVENTNOTIFY_ON msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_EVENTNOTIFY_ON;
		msg.dwObjectID = gHeroID;
		memset( msg.EventList, 0, sizeof(BYTE)*eEvent_Max );
		msg.EventList[nEvent] = 1;
		SafeStrCpy( msg.strTitle, bufTitle, 32 );
		SafeStrCpy( msg.strContext, bufContext, 128 );

		NETWORK->Send( &msg, sizeof(msg) );
	}
	else
	{
		MSGBASE msg;
		msg.Category = MP_CHEAT;
		msg.Protocol = MP_CHEAT_EVENTNOTIFY_OFF;
		msg.dwObjectID = gHeroID;

		NETWORK->Send( &msg, sizeof(msg) );
	}

	return TRUE;
}

//---KES GM툴 로그아웃 처리 추가
void CGMToolManager::LogOut()
{
	m_bLogin = FALSE;
	m_nPower = eGM_POWER_MAX;
}
//-------------------------

void CGMToolManager::Login( BOOL bLogin, int nGMPower )
{
	static char power_str[eGM_POWER_MAX][16] = { "Master", "Monitor", "Patroller", "Auditor", "Eventer", "QA" };

	char buf[128];
	if( bLogin )
	{
		wsprintf( buf, "< GM Power : %s >", power_str[nGMPower] );
		MessageBox( m_hWndLoginDlg, buf, "Login OK!", MB_OK );
		m_bLogin = TRUE;
		m_nPower = nGMPower;
		ShowWindow( m_hWndLoginDlg, SW_HIDE );
	}
	else
	{
		if( nGMPower == 1)		//nGMPower = error code : if bLogin == FALSE
		{
			MessageBox( m_hWndLoginDlg, "Invalid ID/PW", "Login Failed!", MB_OK );
		}
		else
		{
			MessageBox( m_hWndLoginDlg, "Error!", "Login Failed!", MB_OK );
		}		
	}
}

void SetChannelFromMap(HWND hWnd, int nMapCBoxID, int nChannelCBoxID)
{
	const MAPTYPE mapType = GetMapNumForName(
		GetMapName(MAP->GetMapNum()));

	if(_countof(g_MapChannelNum) < mapType)
	{
		return;
	}

	SendDlgItemMessage(
		hWnd,
		nChannelCBoxID,
		CB_RESETCONTENT,
		0,
		0);

	switch(nMapCBoxID)
	{
	case IDC_GMREGEN_CMB_MAP:
		SendDlgItemMessage( hWnd, nChannelCBoxID, CB_ADDSTRING, 0, (LPARAM)"all" );
		break;
	}

	for(WORD channel = 1 ; channel <= g_MapChannelNum[mapType] ; ++channel)
	{
		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			"%u",
			channel);
		SendDlgItemMessage( hWnd, nChannelCBoxID, CB_ADDSTRING, 0, (LPARAM)text );
	}

	switch(nMapCBoxID)
	{
	case IDC_GMREGEN_CMB_MAP:
		SendDlgItemMessage( hWnd, nChannelCBoxID, CB_SETCURSEL, gChannelNum-1, 0 );
		break;
	default:
		SendDlgItemMessage( hWnd, nChannelCBoxID, CB_SETCURSEL, gChannelNum, 0 );
		break;
	}
}

INT_PTR CALLBACK GMLoginDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch(message)
	{
	case WM_INITDIALOG:
		{
			RECT rcGame;
			RECT rcDlg;
			POINT pt;
			GetClientRect( hWnd, &rcDlg );
			GetClientRect( _g_hWnd, &rcGame );
			pt.x = rcGame.left;
			pt.y = rcGame.top;
			ClientToScreen( _g_hWnd, &pt );

			const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
			int x = ( ( dispInfo.dwWidth - ( rcDlg.right - rcDlg.left ) ) / 2 ) + pt.x;
			int y = ( ( dispInfo.dwHeight - ( rcDlg.bottom - rcDlg.top ) ) / 2 ) + pt.y;
			// 070202 LYW --- End.

			SetWindowPos( hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );

			SendMessage( GetDlgItem( hWnd, IDC_GMLOGIN_EDT_ID ), EM_LIMITTEXT, MAX_NAME_LENGTH+1, 0 );
			SendMessage( GetDlgItem( hWnd, IDC_GMLOGIN_EDT_PW ), EM_LIMITTEXT, MAX_NAME_LENGTH+1, 0 );
		}
		return TRUE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				if( LOWORD( wParam ) == IDC_GMLOGIN_BTN_CONNECT )
				{
					char strID[MAX_NAME_LENGTH+1];
					char strPW[MAX_NAME_LENGTH+1];
					GetWindowText( GetDlgItem(hWnd, IDC_GMLOGIN_EDT_ID), strID, MAX_NAME_LENGTH+1 );
					GetWindowText( GetDlgItem(hWnd, IDC_GMLOGIN_EDT_PW), strPW, MAX_NAME_LENGTH+1 );
					
					if( strID[0] == 0 || strPW[0] == 0 )
					{
						MessageBox( hWnd, "Input ID/PW", "Error", MB_OK );
						return TRUE;
					}
				
					MSG_NAME2 msg;
					msg.Category = MP_CHEAT;
					msg.Protocol = MP_CHEAT_GM_LOGIN_SYN;
					msg.dwObjectID = gHeroID;
					SafeStrCpy( msg.str1, strID, MAX_NAME_LENGTH+1 );
					SafeStrCpy( msg.str2, strPW, MAX_NAME_LENGTH+1 );

					NETWORK->Send( &msg, sizeof(msg) );

					SetWindowText( GetDlgItem(hWnd, IDC_GMLOGIN_EDT_ID), "" );
					SetWindowText( GetDlgItem(hWnd, IDC_GMLOGIN_EDT_PW), "" );
				}
				else if( LOWORD( wParam ) == IDC_GMLOGIN_BTN_CANCEL )
				{
					ShowWindow( hWnd, SW_HIDE );
					SetWindowText( GetDlgItem(hWnd, IDC_GMLOGIN_EDT_ID), "" );
					SetWindowText( GetDlgItem(hWnd, IDC_GMLOGIN_EDT_PW), "" );
				}

			}
		}
		return TRUE;
	}
    return FALSE;
}


INT_PTR CALLBACK GMDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch(message)
	{
	case WM_INITDIALOG:
		{
			CheckDlgButton( hWnd, IDC_GM_MNBTN_NONE, BST_CHECKED );

//#ifdef TAIWAN_LOCAL	//중국은 이벤트 활성화 안됨
//			EnableWindow( GetDlgItem( hWnd, IDC_GM_MNBTN_EVENT ), FALSE );
//#endif
		}
		return FALSE;	//not active

	case WM_NCHITTEST:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		{
			SetFocus( _g_hWnd );
		}
		break;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				GMTOOLMGR->OnClickMenu( LOWORD( wParam ) - IDC_GM_MNBTN_MOVE );
			}
		}
		return TRUE;

	case WM_MOVE:
		{
			GMTOOLMGR->SetPositionSubDlg();
		}
		break;

	case WM_CLOSE:
		{
			GMTOOLMGR->ShowGMDialog( FALSE, TRUE );
		}
		return TRUE;
	}


    return FALSE;
}

INT_PTR CALLBACK GMSubMoveDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckRadioButton( hWnd, IDC_GMMOVE_BTN_ME, IDC_GMMOVE_BTN_USER, IDC_GMMOVE_BTN_ME );
			CheckRadioButton( hWnd, IDC_GMMOVE_BTN_XY, IDC_GMMOVE_BTN_NPC, IDC_GMMOVE_BTN_XY );			
			CheckDlgButton(	hWnd, IDC_GMMOVE_CHK_MINIMAP_ON, TRUE);
			SetChannelFromMap(hWnd, IDC_GMMOVE_CMB_MAP, IDC_GMMOVE_CMB_CHANNEL);
		}
		return FALSE;	//not active

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMMOVE_OK:	//Do Move
					{
						if( SendMessage( GetDlgItem( hWnd, IDC_GMMOVE_BTN_XY ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{							
							if( SendMessage( GetDlgItem( hWnd, IDC_GMMOVE_BTN_ME ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
								GMTOOLMGR->OnGMMoveCommand( 0 );
							else
								GMTOOLMGR->OnUserMoveCommand( 0 );
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_GMMOVE_BTN_NAME ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							if( SendMessage( GetDlgItem( hWnd, IDC_GMMOVE_BTN_ME ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
								GMTOOLMGR->OnGMMoveCommand( 1 );
							else
								GMTOOLMGR->OnUserMoveCommand( 1 );
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_GMMOVE_BTN_MAP ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							if( SendMessage( GetDlgItem( hWnd, IDC_GMMOVE_BTN_ME ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
								GMTOOLMGR->OnGMMoveCommand( 2 );
							else
								GMTOOLMGR->OnUserMoveCommand( 2 );
						}
						// 100427 ONS NPC로의 이동처리 추가
						else if( SendMessage( GetDlgItem( hWnd, IDC_GMMOVE_BTN_NPC ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							if( SendMessage( GetDlgItem( hWnd, IDC_GMMOVE_BTN_ME ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
								GMTOOLMGR->OnGMMoveCommand( 3 );
							else
								GMTOOLMGR->OnUserMoveCommand( 3 );
						}

					}
					break;

				case IDC_GMMOVE_CANCEL:
					{
						GMTOOLMGR->OnClickMenu( eMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;

				case IDC_GMMOVE_BTN_ME:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_USER ), FALSE );

						if( SendMessage( GetDlgItem( hWnd, IDC_GMMOVE_BTN_NAME ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_NAME ), TRUE );
						}

						SetWindowText( GetDlgItem( hWnd, IDC_GMMOVE_BTN_NAME ), "Character" );
					}
					break;

				case IDC_GMMOVE_BTN_USER:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_USER ), TRUE );
						CObject* pObject = OBJECTMGR->GetSelectedObject();
						if( pObject )
						if( pObject->GetObjectKind() == eObjectKind_Player )
						{
							SetDlgItemText( hWnd, IDC_GMMOVE_EDT_USER, pObject->GetObjectName() );
						}

						SetFocus( GetDlgItem( hWnd, IDC_GMMOVE_EDT_USER ) );

						//IDC_GMMOVE_BTN_NAME이 체크되어 있고
						//IDC_GMMOVE_EDT_NAME가 enable이면 disable로
						if( SendMessage( GetDlgItem( hWnd, IDC_GMMOVE_BTN_NAME ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_NAME ), FALSE );
						}

						SetWindowText( GetDlgItem( hWnd, IDC_GMMOVE_BTN_NAME ), "To GM" );

						//character글씨를 To GM 으로?
					}
					break;

				case IDC_GMMOVE_BTN_XY:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_X ), TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_Y ), TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_NAME ), FALSE );
						//EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_MAP ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_MAP ), FALSE );
						//EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_CHANNEL ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_CHANNEL ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_NPC ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CHK_MINIMAP_ON ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CHK_MINIMAP_OFF ), FALSE );

						SetFocus( GetDlgItem( hWnd, IDC_GMMOVE_EDT_X ) );
					}
					break;

				case IDC_GMMOVE_BTN_NAME:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_X ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_Y ), FALSE );
						//EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_NAME ), TRUE );
						//EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_MAP ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_MAP ), FALSE );
						//EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_CHANNEL ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_CHANNEL ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_NPC ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CHK_MINIMAP_ON ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CHK_MINIMAP_OFF ), FALSE );
						
						if( SendMessage( GetDlgItem( hWnd, IDC_GMMOVE_BTN_ME ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_NAME ), TRUE );

							CObject* pObject = OBJECTMGR->GetSelectedObject();
							if( pObject )
							if( pObject->GetObjectKind() == eObjectKind_Player )
							{
								SetDlgItemText( hWnd, IDC_GMMOVE_EDT_NAME, pObject->GetObjectName() );
							}

							SetFocus( GetDlgItem( hWnd, IDC_GMMOVE_EDT_NAME ) );
						}
					}
					break;

				case IDC_GMMOVE_BTN_MAP:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_X ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_Y ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_NAME ), FALSE );
						//EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_MAP ), TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_MAP ), TRUE );
						//EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_CHANNEL ), TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_CHANNEL ), TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_NPC ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CHK_MINIMAP_ON ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CHK_MINIMAP_OFF ), FALSE );

						//SetFocus( GetDlgItem( hWnd, IDC_GMMOVE_EDT_MAP ) );
						SetFocus( GetDlgItem( hWnd, IDC_GMMOVE_CMB_MAP ) );

						SetChannelFromMap(hWnd, IDC_GMMOVE_CMB_MAP, IDC_GMMOVE_CMB_CHANNEL);
					}
					break;
					// 100427 ONS 맵이동 NPC목록 추가
				case IDC_GMMOVE_BTN_NPC:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_X ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_Y ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_NAME ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_MAP ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_CHANNEL ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_NPC ), TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CHK_MINIMAP_ON ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CHK_MINIMAP_OFF ), FALSE );

						GMTOOLMGR->SetNpcList();
						SetFocus( GetDlgItem( hWnd, IDC_GMMOVE_CMB_NPC ) );
					}
					break;
				case IDC_GMMOVE_BTN_MINIMAP:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_X ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_Y ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_EDT_NAME ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_MAP ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_CHANNEL ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CMB_NPC ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CHK_MINIMAP_ON ), TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMMOVE_CHK_MINIMAP_OFF ), TRUE );

						if( SendMessage( GetDlgItem( hWnd, IDC_GMMOVE_CHK_MINIMAP_ON ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							GMTOOLMGR->ActiveMiniMap( TRUE );
						}
					}
					break;
				case IDC_GMMOVE_CHK_MINIMAP_ON:
				case IDC_GMMOVE_CHK_MINIMAP_OFF:
					{
						GMTOOLMGR->ActiveMiniMap( (LOWORD( wParam ) == IDC_GMMOVE_CHK_MINIMAP_ON) ? TRUE : FALSE );
					}
					break;
				}
			}
			else if( HIWORD( wParam ) == CBN_SELENDOK )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMMOVE_CMB_MAP:
					{
						SetChannelFromMap(hWnd, IDC_GMMOVE_CMB_MAP, IDC_GMMOVE_CMB_CHANNEL);
					}
					break;
				}
			}
		}
		return TRUE;

	case WM_SETFOCUS:
		{
#ifdef TAIWAN_LOCAL
		HIMC hIMC = ImmGetContext( _g_hWnd );
		ImmSetOpenStatus( hIMC, TRUE );
		ImmReleaseContext( _g_hWnd, hIMC );

		HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
		SendMessage( hWndIme, WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0 );
#endif
		}
		break;
	case WM_KILLFOCUS:
		{
#ifdef TAIWAN_LOCAL
			HIMC hIMC = ImmGetContext( _g_hWnd );
			ImmSetOpenStatus( hIMC, FALSE );
			ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			ImmNotifyIME( hIMC, NI_CLOSECANDIDATE, 0, 0 );
			ImmReleaseContext( _g_hWnd, hIMC );

			HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
			SendMessage( hWndIme, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0 );
#endif
		}
		break;
	}

	return FALSE;
}

INT_PTR CALLBACK GMSubForbidChatDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMCHAT_BTN_FORBID:
					{
						GMTOOLMGR->OnForbidChatCommand(1);
					}
					break;
				case IDC_GMCHAT_BTN_PERMIT:
					{
						GMTOOLMGR->OnForbidChatCommand(0);
					}
					break;					
				case IDC_GMBLOCK_BTN_CANCEL:
					{
						GMTOOLMGR->OnClickMenu( eMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;

				case IDC_GMBLOCK_BTN_USER:
					{
						SetFocus( GetDlgItem( hWnd, IDC_GMBLOCK_EDT_CHAR ) );

						CObject* pObject = OBJECTMGR->GetSelectedObject();
						if( pObject )
						if( pObject->GetObjectKind() == eObjectKind_Player )
						{
							SetDlgItemText( hWnd, IDC_GMBLOCK_EDT_CHAR, pObject->GetObjectName() );
						}
					}
					break;
				}
			}
		}
		return TRUE;

	case WM_SETFOCUS:
		{
#ifdef TAIWAN_LOCAL
		HIMC hIMC = ImmGetContext( _g_hWnd );
		ImmSetOpenStatus( hIMC, TRUE );
		ImmReleaseContext( _g_hWnd, hIMC );

		HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
		SendMessage( hWndIme, WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0 );
#endif
		}
		break;
	case WM_KILLFOCUS:
		{
#ifdef TAIWAN_LOCAL
			HIMC hIMC = ImmGetContext( _g_hWnd );
			ImmSetOpenStatus( hIMC, FALSE );
			ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			ImmNotifyIME( hIMC, NI_CLOSECANDIDATE, 0, 0 );
			ImmReleaseContext( _g_hWnd, hIMC );

			HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
			SendMessage( hWndIme, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0 );
#endif

		}
		break;
	}

	return FALSE;
}


INT_PTR CALLBACK GMSubHideDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckRadioButton( hWnd, IDC_GMHIDE_BTN_ME, IDC_GMHIDE_BTN_NPC, IDC_GMHIDE_BTN_ME );
			CheckRadioButton( hWnd, IDC_GMHIDE_BTN_HIDE, IDC_GMHIDE_BTN_SHOW, IDC_GMHIDE_BTN_HIDE );

			//SetChannelFromMap(hWnd, IDC_GMHIDE_CMB_MA, IDC_GMHIDE_CMB_CHANNEL);
		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMHIDE_BTN_APPLY:
					{
						GMTOOLMGR->OnHideCommand();
					}
					break;
				case IDC_GMHIDE_BTN_CANCEL:
					{
						GMTOOLMGR->OnClickMenu( eMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;
				case IDC_GMHIDE_BTN_ME:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMHIDE_EDT_USER ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMHIDE_CMB_CHANNEL ), FALSE );
						CheckDlgButton( hWnd, IDC_GMHIDE_BTN_NPC, FALSE );
					}
					break;
				case IDC_GMHIDE_BTN_USER:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMHIDE_EDT_USER ), TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMHIDE_CMB_CHANNEL ), FALSE );
						SetFocus( GetDlgItem( hWnd, IDC_GMHIDE_EDT_USER ) );

						CObject* pObject = OBJECTMGR->GetSelectedObject();
						if( pObject )
						if( pObject->GetObjectKind() == eObjectKind_Player )
						{
							SetDlgItemText( hWnd, IDC_GMHIDE_EDT_USER, pObject->GetObjectName() );
						}

						CheckDlgButton( hWnd, IDC_GMHIDE_BTN_NPC, FALSE );
					}
					break;
				case IDC_GMHIDE_BTN_NPC:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMHIDE_EDT_USER ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMHIDE_CMB_CHANNEL ), TRUE );

						CheckDlgButton( hWnd, IDC_GMHIDE_BTN_ME, FALSE );
						CheckDlgButton( hWnd, IDC_GMHIDE_BTN_USER, FALSE );
					}
					break;
				}
			}
		}
		return TRUE;


	case WM_SETFOCUS:
		{
#ifdef TAIWAN_LOCAL
		HIMC hIMC = ImmGetContext( _g_hWnd );
		ImmSetOpenStatus( hIMC, TRUE );
		ImmReleaseContext( _g_hWnd, hIMC );

		HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
		SendMessage( hWndIme, WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0 );
#endif
		}
		break;
	case WM_KILLFOCUS:
		{
#ifdef TAIWAN_LOCAL
			HIMC hIMC = ImmGetContext( _g_hWnd );
			ImmSetOpenStatus( hIMC, FALSE );
			ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			ImmNotifyIME( hIMC, NI_CLOSECANDIDATE, 0, 0 );
			ImmReleaseContext( _g_hWnd, hIMC );

			HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
			SendMessage( hWndIme, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0 );
#endif
		}
		break;
	}
	return FALSE;
}


INT_PTR CALLBACK GMSubWhereDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckDlgButton( hWnd, IDC_GMWHERE_BTN_USER, BST_CHECKED );
			CheckDlgButton( hWnd, IDC_GMWHERE_BTN_MAP, BST_CHECKED );
			CheckDlgButton( hWnd, IDC_GMWHERE_BTN_XY, BST_CHECKED );
		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMWHERE_BTN_WHERE:
					{
						GMTOOLMGR->OnWhereCommand();
					}
					break;
				case IDC_GMWHERE_BTN_CANCEL:
					{
						GMTOOLMGR->OnClickMenu( eMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;
				case IDC_GMWHERE_BTN_MAP:
					{
					}
					break;
				case IDC_GMWHERE_BTN_XY:
					{
					}
					break;
				}
			}
		}
		return TRUE;

	case WM_SETFOCUS:
		{
#ifdef TAIWAN_LOCAL
		HIMC hIMC = ImmGetContext( _g_hWnd );
		ImmSetOpenStatus( hIMC, TRUE );
		ImmReleaseContext( _g_hWnd, hIMC );

		HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
		SendMessage( hWndIme, WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0 );
#endif
		}
		break;
	case WM_KILLFOCUS:
		{
#ifdef TAIWAN_LOCAL
			HIMC hIMC = ImmGetContext( _g_hWnd );
			ImmSetOpenStatus( hIMC, FALSE );
			ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			ImmNotifyIME( hIMC, NI_CLOSECANDIDATE, 0, 0 );
			ImmReleaseContext( _g_hWnd, hIMC );

			HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
			SendMessage( hWndIme, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0 );
#endif
		}
		break;
	}

	return FALSE;
}

INT_PTR CALLBACK GMSubItemDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckRadioButton( hWnd, IDC_GMITEM_BTN_WEAPON, IDC_GMITEM_BTN_ABILITY, IDC_GMITEM_BTN_WEAPON );
			EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_BTN_SEARCH ),		TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDIT_SEARCH),		TRUE );
			SetWindowText( GetDlgItem( hWnd, IDC_GMITEM_SEARCHFROM), "from Weapon List");
		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
					case IDC_GMITEM_BTN_SEARCH:
					{
						int nCurSelDlgID = -1;
						int nCurSelCBID = -1;
						
						if( SendMessage( GetDlgItem( hWnd, IDC_GMITEM_BTN_WEAPON ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_GMITEM_BTN_WEAPON;
							nCurSelCBID = IDC_GMITEM_CMB_WEAPON;
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_GMITEM_BTN_CLOTHES ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_GMITEM_BTN_CLOTHES;
							nCurSelCBID = IDC_GMITEM_CMB_CLOTHES;
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_GMITEM_BTN_ACCESSORY ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_GMITEM_BTN_ACCESSORY;
							nCurSelCBID = IDC_GMITEM_CMB_ACCESSORY;
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_GMITEM_BTN_SKILLBOOK ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_GMITEM_BTN_SKILLBOOK;
							nCurSelCBID = IDC_GMITEM_CMB_SKILLBOOK;
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_GMITEM_BTN_POTION ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_GMITEM_BTN_POTION;
							nCurSelCBID = IDC_GMITEM_CMB_POTION;
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_GMITEM_BTN_ETC ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_GMITEM_BTN_ETC;
							nCurSelCBID = IDC_GMITEM_CMB_ETC;
						}
						// 090909 ONS 스킬 검색처리 추가
						else if( SendMessage( GetDlgItem( hWnd, IDC_GMITEM_BTN_SKILL ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_GMITEM_BTN_SKILL;
							nCurSelCBID = IDC_GMITEM_CMB_SKILL;
						}

						if(-1<nCurSelDlgID && -1<nCurSelCBID)
						{
							HWND hWndCB = GetDlgItem(hWnd, IDC_GMITEM_EDIT_SEARCH);
							int nSel = SendDlgItemMessage( hWnd, nCurSelCBID, CB_GETCURSEL, 0, 0 );
							int nCount = SendDlgItemMessage( hWnd, nCurSelCBID, CB_GETCOUNT, 0, 0 );

							char szSearch[256] = {0,};
							GetWindowText(hWndCB, szSearch, 255);
							
							if(2 <= strlen(szSearch))
							{
								int i, nFind=-1;
								for(i=nSel+1; i<nCount; i++)
								{
									char szName[256] = {0,};
									SendDlgItemMessage( hWnd, nCurSelCBID, CB_GETLBTEXT, i, (LPARAM)szName );

									if(strstr(szName, szSearch) != NULL)
									{
										SendDlgItemMessage( hWnd, nCurSelCBID, CB_SETCURSEL, nFind, 0 );
										nFind = i;
										break;
									}
								}

								if(-1 < nFind)
								{
									SendDlgItemMessage( hWnd, nCurSelCBID, CB_SETCURSEL, nFind, 0 );
									// 090909 ONS 검색된 스킬의 레벨범위를 표시한다.
									if(nCurSelDlgID == IDC_GMITEM_BTN_SKILL && nCurSelCBID == IDC_GMITEM_CMB_SKILL)
									{
										cSkillInfo* pSkillInfo = NULL ;
										const SkillData* pSkillList = GetSkillTreeList();
										pSkillInfo = SKILLMGR->GetSkillInfo( pSkillList[nSel].index );
										if(pSkillInfo)
										{
											for(int j=0; j<30; j++)
											{
												SendDlgItemMessage( hWnd, IDC_GMITEM_CMB_SKILLSUNG, CB_DELETESTRING, 0, 0 );
											}
											for(int i=0; i<pSkillList[nFind].level; i++)
											{
												char bufNum[20];
												wsprintf( bufNum, "%d", i+1 );
												SendDlgItemMessage( hWnd, IDC_GMITEM_CMB_SKILLSUNG, CB_ADDSTRING, 0, (LPARAM)bufNum );
											}
											SendDlgItemMessage( hWnd, IDC_GMITEM_CMB_SKILLSUNG, CB_SETCURSEL, 0, 0 );
										}
									}
								}
							}
						}
					}
					break;

				case IDC_GMITEM_BTN_GET:
					{
						GMTOOLMGR->OnItemCommand();
					}
					break;
				case IDC_GMITEM_BTN_CANCEL:
					{
						GMTOOLMGR->OnClickMenu( eMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;
				case IDC_GMITEM_BTN_WEAPON:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPON ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPONGRADE ),	TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHES ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHESGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ACCESSORY ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLBOOK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_MONEY ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILL ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLSUNG ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ABILITY ),		FALSE );

						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_GMITEM_SEARCHFROM), "from Weapon List");
					}
					break;
				case IDC_GMITEM_BTN_CLOTHES:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPON ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPONGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHES ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHESGRADE ),	TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ACCESSORY ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLBOOK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_MONEY ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILL ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLSUNG ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ABILITY ),		FALSE );

						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_GMITEM_SEARCHFROM), "from Clothes List");
					}
					break;
				case IDC_GMITEM_BTN_ACCESSORY:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPON ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPONGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHES ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHESGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ACCESSORY ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLBOOK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_MONEY ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILL ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLSUNG ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ABILITY ),		FALSE );

						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_GMITEM_SEARCHFROM), "from Accessory List");
					}
					break;
				case IDC_GMITEM_BTN_SKILLBOOK:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPON ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPONGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHES ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHESGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ACCESSORY ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLBOOK ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_MONEY ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILL ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLSUNG ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ABILITY ),		FALSE );

						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_GMITEM_SEARCHFROM), "from SkillBook List");
					}
					break;
					case IDC_GMITEM_BTN_POTION:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPON ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPONGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHES ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHESGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ACCESSORY ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLBOOK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_POTION ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_POTION ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_POTION ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_MONEY ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILL ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLSUNG ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ABILITY ),		FALSE );

						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_GMITEM_SEARCHFROM), "from Potion List");
					}
					break;
				case IDC_GMITEM_BTN_ETC:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPON ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPONGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHES ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHESGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ACCESSORY ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLBOOK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ETC ),			TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ETC ),			TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_ETC ),			TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_MONEY ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILL ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLSUNG ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ABILITY ),		FALSE );

						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_GMITEM_SEARCHFROM), "from Etc List");
					}
					break;
				case IDC_GMITEM_BTN_MONEY:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPON ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPONGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHES ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHESGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ACCESSORY ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLBOOK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_MONEY ),			TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILL ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLSUNG ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ABILITY ),		FALSE );

						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_BTN_SEARCH ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDIT_SEARCH),		FALSE );
						SetWindowText( GetDlgItem( hWnd, IDC_GMITEM_SEARCHFROM), "from List");
					}
					break;
				case IDC_GMITEM_BTN_SKILL:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPON ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPONGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHES ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHESGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ACCESSORY ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLBOOK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_MONEY ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILL ),			TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLSUNG ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ABILITY ),		FALSE );

						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_GMITEM_SEARCHFROM), "from Skill List");
					}
					break;
				case IDC_GMITEM_BTN_ABILITY:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPON ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_WEAPONGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHES ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_CLOTHESGRADE ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ACCESSORY ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLBOOK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_POTION ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_SPN_ETC ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_MONEY ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILL ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_CMB_SKILLSUNG ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDT_ABILITY ),		TRUE );

						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_BTN_SEARCH ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMITEM_EDIT_SEARCH),		FALSE );
					}
					break;
				}
			}
			else if( HIWORD( wParam ) == CBN_SELCHANGE )
			{
				if( LOWORD( wParam ) == IDC_GMITEM_CMB_SKILL )
				{
					char buf[64];
					
					//GetDlgItemText( hWnd, IDC_GMITEM_CMB_SKILL, buf, MAX_ITEMNAME_LENGTH+1 );
					int nSel = SendDlgItemMessage( hWnd, IDC_GMITEM_CMB_SKILL, CB_GETCURSEL, 0, 0 );

					SendDlgItemMessage( hWnd, IDC_GMITEM_CMB_SKILL, CB_GETLBTEXT, nSel, (LPARAM)buf );

					// 090904 ONS 선택된 스킬의 최대레벨을 표시한다.
					// 레벨리스트 초기화
					for(int j=0; j<30; j++)
					{
						SendDlgItemMessage( hWnd, IDC_GMITEM_CMB_SKILLSUNG, CB_DELETESTRING, 0, 0 );
					}
										
					cSkillInfo* pSkillInfo = NULL ;
					const SkillData* pSkillList = GetSkillTreeList();
					pSkillInfo = SKILLMGR->GetSkillInfo( pSkillList[nSel].index );
					if(pSkillInfo)
					{
						for(int i=0; i<pSkillList[nSel].level; i++)
						{
							char bufNum[20];
							wsprintf( bufNum, "%d", i+1 );
							SendDlgItemMessage( hWnd, IDC_GMITEM_CMB_SKILLSUNG, CB_ADDSTRING, 0, (LPARAM)bufNum );
						}
						SendDlgItemMessage( hWnd, IDC_GMITEM_CMB_SKILLSUNG, CB_SETCURSEL, 0, 0 );
					}
				}
			}
		}
		return TRUE;

	case WM_SETFOCUS:
		{
#ifdef TAIWAN_LOCAL
		HIMC hIMC = ImmGetContext( _g_hWnd );
		ImmSetOpenStatus( hIMC, TRUE );
		ImmReleaseContext( _g_hWnd, hIMC );

		HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
		SendMessage( hWndIme, WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0 );
#endif
		}
		break;
	case WM_KILLFOCUS:
		{
#ifdef TAIWAN_LOCAL
			HIMC hIMC = ImmGetContext( _g_hWnd );
			ImmSetOpenStatus( hIMC, FALSE );
			ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			ImmNotifyIME( hIMC, NI_CLOSECANDIDATE, 0, 0 );
			ImmReleaseContext( _g_hWnd, hIMC );

			HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
			SendMessage( hWndIme, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0 );
#endif
		}
		break;
	}

	return FALSE;
}


INT_PTR CALLBACK GMSubRegenDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckRadioButton( hWnd, IDC_GMREGEN_BTN_MODEDIRECT, IDC_GMREGEN_BTN_DELETE, IDC_GMREGEN_BTN_MODEDIRECT );

			SetDlgItemInt( hWnd, IDC_GMREGEN_EDT_MOBNUM, 1, TRUE );
			//SetDlgItemInt( hWnd, IDC_GMREGEN_EDT_CHANNEL, 0, TRUE );
			SetDlgItemInt( hWnd, IDC_GMREGEN_EDT_RAD, 10, TRUE );

			SetDlgItemInt( hWnd, IDC_GMREGEN_EDT_X, 0, TRUE );
			SetDlgItemInt( hWnd, IDC_GMREGEN_EDT_Y, 0, TRUE );

			SetDlgItemInt( hWnd, IDC_GMREGEN_EDT_ITEM, 0, TRUE );

			SetChannelFromMap(hWnd, IDC_GMREGEN_CMB_MAP, IDC_GMREGEN_CMB_CHANNEL);
		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMREGEN_BTN_APPLY:
					{
						GMTOOLMGR->OnRegenCommand();
					}
					break;
				case IDC_GMREGEN_BTN_CANCEL:
					{
						GMTOOLMGR->OnClickMenu( eMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;
				case IDC_GMREGEN_BTN_MODEDIRECT:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_MOBNAME ),	TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_MOBNUM ),	TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_MAP ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_CHANNEL ),	TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_X ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_Y ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_BTN_MYPOS ),	TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_RAD ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_ITEM ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_ITEM ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_SUMMON_MONSTER_CHECK ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_SUMMON_VEHICLE_CHECK ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_BTN_OPEN ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_FILE ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_VEHICLE_GET_BUTTON ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_VEHICLE_DROPDOWN ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_OBJECT_EDIT ), FALSE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_OBJECT_FIND_BUTTON ), FALSE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_FILE_EDIT ), FALSE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_FILE_OPEN_BUTTON ), FALSE);
					}
					break;
				case IDC_GMREGEN_BTN_MODEFILE:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_MOBNAME ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_MOBNUM ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_MAP ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_CHANNEL ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_X ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_Y ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_BTN_MYPOS ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_RAD ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_ITEM ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_ITEM ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_SUMMON_MONSTER_CHECK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_SUMMON_VEHICLE_CHECK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_BTN_OPEN ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_FILE ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_VEHICLE_GET_BUTTON ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_VEHICLE_DROPDOWN ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_OBJECT_EDIT ), FALSE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_OBJECT_FIND_BUTTON ), FALSE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_FILE_EDIT ), FALSE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_FILE_OPEN_BUTTON ), FALSE);
					}
					break;
				case IDC_GMREGEN_BTN_DELETE:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_MOBNAME ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_MOBNUM ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_MAP ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_CHANNEL ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_X ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_Y ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_BTN_MYPOS ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_RAD ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_ITEM ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_ITEM ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_SUMMON_MONSTER_CHECK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_SUMMON_VEHICLE_CHECK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_BTN_OPEN ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_FILE ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_VEHICLE_GET_BUTTON ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_VEHICLE_DROPDOWN ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_OBJECT_EDIT ), FALSE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_OBJECT_FIND_BUTTON ), FALSE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_FILE_EDIT ), FALSE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_FILE_OPEN_BUTTON ), FALSE);
					}
					break;
				case IDC_GMREGEN_BTN_UNSUMMON_VEHICLE:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_MOBNAME ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_MOBNUM ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_MAP ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_CHANNEL ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_X ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_Y ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_BTN_MYPOS ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_RAD ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_ITEM ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_ITEM ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_SUMMON_MONSTER_CHECK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_SUMMON_VEHICLE_CHECK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_BTN_OPEN ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_FILE ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_VEHICLE_GET_BUTTON ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_VEHICLE_DROPDOWN ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_OBJECT_EDIT ), FALSE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_OBJECT_FIND_BUTTON ), FALSE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_FILE_EDIT ), FALSE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_FILE_OPEN_BUTTON ), FALSE);
					}
					break;
				case IDC_GMREGEN_SCRIPT_RADIO:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_MOBNAME ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_MOBNUM ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_MAP ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_CHANNEL ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_X ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_Y ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_BTN_MYPOS ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_RAD ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_CMB_ITEM ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_ITEM ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_SUMMON_MONSTER_CHECK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_SUMMON_VEHICLE_CHECK ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_BTN_OPEN ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_EDT_FILE ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_VEHICLE_GET_BUTTON ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMREGEN_VEHICLE_DROPDOWN ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_OBJECT_EDIT ), TRUE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_OBJECT_FIND_BUTTON ), TRUE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_FILE_EDIT ), TRUE);
						EnableWindow( GetDlgItem( hWnd, IDC_SCRIPT_FILE_OPEN_BUTTON ), TRUE);
					}
					break;
				case IDC_GMREGEN_BTN_MYPOS:
					{
						if( HERO )
						{
							VECTOR3 vPos;
							HERO->GetPosition( &vPos );
							SetDlgItemInt( hWnd, IDC_GMREGEN_EDT_X, (UINT)(vPos.x / 100.0f), TRUE );
							SetDlgItemInt( hWnd, IDC_GMREGEN_EDT_Y, (UINT)(vPos.z / 100.0f), TRUE );
						}

						SetDlgItemText( hWnd, IDC_GMREGEN_CMB_MAP, GetMapName( MAP->GetMapNum() ) );
						SetChannelFromMap(hWnd, IDC_GMREGEN_CMB_MAP, IDC_GMREGEN_CMB_CHANNEL);
						SendDlgItemMessage( hWnd, IDC_GMREGEN_CMB_CHANNEL, CB_SETCURSEL, gChannelNum+1, 0 );
					}
					break;

				case IDC_GMREGEN_BTN_OPEN:
					{
						//파일 다이얼로그
						char lpstrFile[MAX_PATH] = "";
						OPENFILENAME OFN;
						ZeroMemory( &OFN, sizeof( OPENFILENAME ) );
						OFN.lStructSize = sizeof( OPENFILENAME );
						OFN.hwndOwner	= hWnd;
						OFN.lpstrFilter	= "Text Files (.txt)\0*.txt\0All Files (*.*)\0*.*\0";
						OFN.lpstrFile	= lpstrFile;
						OFN.nMaxFile	= MAX_PATH;						
						OFN.Flags		= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST; 
						
						if( GetOpenFileName( &OFN ) != 0 )
						{
							SetDlgItemText( hWnd, IDC_GMREGEN_EDT_FILE, OFN.lpstrFile );
						}
					}
					break;
				case IDC_GMREGEN_SUMMON_MONSTER_CHECK:
				case IDC_GMREGEN_SUMMON_VEHICLE_CHECK:
					{
						SendDlgItemMessage(
							hWnd,
							IDC_GMREGEN_CMB_MOBNAME,
							CB_RESETCONTENT,
							0,
							0);

						const BOOL isCheckedMonster = IsDlgButtonChecked(
							hWnd,
							IDC_GMREGEN_SUMMON_MONSTER_CHECK);
						const BOOL isCheckedVehicle = IsDlgButtonChecked(
							hWnd,
							IDC_GMREGEN_SUMMON_VEHICLE_CHECK);

						int index = 0;

						for(const BASE_MONSTER_LIST* baseMonsterList = GAMERESRCMNGR->GetMonsterListInfo(++index);
							0 < baseMonsterList;
							baseMonsterList = GAMERESRCMNGR->GetMonsterListInfo(++index))
						{
							TCHAR text[MAX_PATH] = _T(" * * * * * ");
							const VehicleScript& vehicleScript = GAMERESRCMNGR->GetVehicleScript(
								baseMonsterList->MonsterKind);

							if(0 == vehicleScript.mMonsterKind)
							{
								if(FALSE == isCheckedMonster)
								{
									continue;
								}

								_stprintf(
									text,
									_T("%s(%u), lv%d"),
									baseMonsterList->Name,
									baseMonsterList->MonsterKind,									
									baseMonsterList->Level);
							}
							else if(isCheckedVehicle)
							{
								_stprintf(
									text,
									_T("%s(%u), %d seat"),									
									baseMonsterList->Name,
									baseMonsterList->MonsterKind,
									vehicleScript.mSeatSize);
							}

							LRESULT ret = SendDlgItemMessage(
								hWnd,
								IDC_GMREGEN_CMB_MOBNAME,
								CB_ADDSTRING,
								0,
								LPARAM(text));
							// MonsterKind를 Data로 등록
							SendDlgItemMessage(
								hWnd,
								IDC_GMREGEN_CMB_MOBNAME,
								CB_SETITEMDATA,
								WPARAM(ret),
								LPARAM(baseMonsterList->MonsterKind));
						}

						SendDlgItemMessage(
							hWnd,
							IDC_GMREGEN_CMB_MOBNAME,
							CB_SETCURSEL,
							0,
							0);
					}
					break;
				case IDC_GMREGEN_VEHICLE_GET_BUTTON:
					{
						MSGBASE message;
						ZeroMemory(
							&message,
							sizeof(message));
						message.Category = MP_CHEAT;
						message.Protocol = MP_CHEAT_VEHICLE_GET_SYN;
						message.dwObjectID = gHeroID;
						NETWORK->Send(
							&message,
							sizeof(message));
					}
					break;
				case IDC_SCRIPT_OBJECT_FIND_BUTTON:
					{
						CObject* const object = OBJECTMGR->GetSelectedObject();

						if(0 == object ||
							FALSE == (eObjectKind_Monster & object->GetObjectKind()))
						{
							MessageBox(
								0,
								"Select monster, please",
								"Alert",
								MB_OK);
							break;
						}

						TCHAR text[MAX_PATH] = {0};
						_stprintf(
							text,
							"%u %s",
							object->GetID(),
							object->GetObjectName());
						SetDlgItemText(
							hWnd,
							IDC_SCRIPT_OBJECT_EDIT,
							text);
					}
					break;
				case IDC_SCRIPT_FILE_OPEN_BUTTON:
					{
						TCHAR currentDirectory[MAX_PATH] = {0};
						GetCurrentDirectory(
							_countof(currentDirectory),
							currentDirectory);

						TCHAR text[MAX_PATH] = {0};
						OPENFILENAME openFileName = {0};
						openFileName.lStructSize = sizeof(openFileName);
						openFileName.hwndOwner	= hWnd;
						openFileName.lpstrFilter = "Bin Files\0*.bin";
						openFileName.lpstrFile	= text;
						openFileName.nMaxFile = _countof(text);
						openFileName.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST; 
						openFileName.lpstrInitialDir = "system\\resource\\AiScript";

						if(0 == GetOpenFileName(&openFileName))
						{
							SetCurrentDirectory(
								currentDirectory);
							break;
						}

						TCHAR textDrive[_MAX_DRIVE] = {0};
						TCHAR textDirectory[_MAX_DIR] = {0};
						TCHAR textFileName[_MAX_FNAME] = {0};
						TCHAR textExtension[_MAX_EXT] = {0};
						_splitpath(
							openFileName.lpstrFile,
							textDrive,
							textDirectory,
							textFileName,
							textExtension);
						_stprintf(
							text,
							"%s%s",
							textFileName,
							textExtension);
						SetDlgItemText(
							hWnd,
							IDC_SCRIPT_FILE_EDIT,
							text);
						SetCurrentDirectory(
							currentDirectory);
					}
					break;
				}
			}
			else if( HIWORD( wParam ) == CBN_SELENDOK )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMREGEN_CMB_MAP:
					{
						SetChannelFromMap(hWnd, IDC_GMREGEN_CMB_MAP, IDC_GMREGEN_CMB_CHANNEL);
					}
					break;
				}
			}
		}
		return TRUE;
	case WM_SETFOCUS:
		{
#ifdef TAIWAN_LOCAL
		HIMC hIMC = ImmGetContext( _g_hWnd );
		ImmSetOpenStatus( hIMC, TRUE );
		ImmReleaseContext( _g_hWnd, hIMC );

		HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
		SendMessage( hWndIme, WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0 );
#endif
		}
		break;
	case WM_KILLFOCUS:
		{
#ifdef TAIWAN_LOCAL
			HIMC hIMC = ImmGetContext( _g_hWnd );
			ImmSetOpenStatus( hIMC, FALSE );
			ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			ImmNotifyIME( hIMC, NI_CLOSECANDIDATE, 0, 0 );
			ImmReleaseContext( _g_hWnd, hIMC );

			HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
			SendMessage( hWndIme, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0 );
#endif
		}
		break;
	}

	return FALSE;
}


INT_PTR CALLBACK GMSubDisconDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckRadioButton( hWnd, IDC_GMDISCON_BTN_USER, IDC_GMDISCON_BTN_MAP, IDC_GMDISCON_BTN_USER );
		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMDISCON_BTN_APPLY:
					{
						GMTOOLMGR->OnDisconCommand();
					}
					break;
				case IDC_GMDISCON_BTN_CANCEL:
					{
						GMTOOLMGR->OnClickMenu( eMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;
				case IDC_GMDISCON_BTN_USER:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMDISCON_EDT_CHAR ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMDISCON_CMB_MAP ),			FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMDISCON_BTN_EXCEPTSELF ),	FALSE );
					}
					break;
				case IDC_GMDISCON_BTN_MAP:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMDISCON_EDT_CHAR ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMDISCON_CMB_MAP ),			TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_GMDISCON_BTN_EXCEPTSELF ),	TRUE );
					}
					break;
				}
			}
		}
		return TRUE;
	case WM_SETFOCUS:
		{
#ifdef TAIWAN_LOCAL
		HIMC hIMC = ImmGetContext( _g_hWnd );
		ImmSetOpenStatus( hIMC, TRUE );
		ImmReleaseContext( _g_hWnd, hIMC );

		HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
		SendMessage( hWndIme, WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0 );
#endif
		}
		break;
	case WM_KILLFOCUS:
		{
#ifdef TAIWAN_LOCAL
			HIMC hIMC = ImmGetContext( _g_hWnd );
			ImmSetOpenStatus( hIMC, FALSE );
			ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			ImmNotifyIME( hIMC, NI_CLOSECANDIDATE, 0, 0 );
			ImmReleaseContext( _g_hWnd, hIMC );

			HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
			SendMessage( hWndIme, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0 );
#endif
		}
		break;
	}

	return FALSE;
}


INT_PTR CALLBACK GMSubPKDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckRadioButton( hWnd, IDC_GMPK_BTN_MAP, IDC_GMPK_BTN_MAP, IDC_GMPK_BTN_MAP );
			CheckRadioButton( hWnd, IDC_GMPK_BTN_NOPK, IDC_GMPK_BTN_ALLOW, IDC_GMPK_BTN_NOPK );

			SetChannelFromMap(hWnd, IDC_GMPK_CMB_MAP, IDC_GMPK_CMB_CHANNEL);
		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMPK_BTN_APPLY:
					{
						GMTOOLMGR->OnPKCommand();
					}
					break;
				case IDC_GMPK_BTN_CANCEL:
					{
						GMTOOLMGR->OnClickMenu( eMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;
				}
			}
			else if( HIWORD( wParam ) == CBN_SELENDOK )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMPK_CMB_MAP:
					{
						SetChannelFromMap(hWnd, IDC_GMREGEN_CMB_MAP, IDC_GMPK_CMB_CHANNEL);
					}
					break;
				}
			}
		}
		return TRUE;

	case WM_SETFOCUS:
		{
#ifdef TAIWAN_LOCAL
		HIMC hIMC = ImmGetContext( _g_hWnd );
		ImmSetOpenStatus( hIMC, TRUE );
		ImmReleaseContext( _g_hWnd, hIMC );

		HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
		SendMessage( hWndIme, WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0 );
#endif
		}
		break;
	case WM_KILLFOCUS:
		{
#ifdef TAIWAN_LOCAL
			HIMC hIMC = ImmGetContext( _g_hWnd );
			ImmSetOpenStatus( hIMC, FALSE );
			ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			ImmNotifyIME( hIMC, NI_CLOSECANDIDATE, 0, 0 );
			ImmReleaseContext( _g_hWnd, hIMC );

			HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
			SendMessage( hWndIme, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0 );
#endif
		}
		break;
	}
	return FALSE;
}

INT_PTR CALLBACK GMSubEventDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckRadioButton( hWnd, IDC_GMCHAT_BTN_USER, IDC_GMCHAT_BTN_MAP, IDC_GMCHAT_BTN_USER );
		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMEVENT_BTN_APPLY:
					{
						GMTOOLMGR->OnEventCommand();
					}
					break;
				case IDC_GMEVENT_BTN_CLOSE:
					{
						GMTOOLMGR->OnClickMenu( eMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;
				}
			}
		}
		break;
	}

	return FALSE;
}

INT_PTR CALLBACK GMSubNoticeDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckRadioButton( hWnd, IDC_GMNOTICE_BTN_ALLMAP, IDC_GMNOTICE_BTN_MAP, IDC_GMNOTICE_BTN_ALLMAP );
			//입력에디트 제한글자 설정 //focus
			SendMessage( GetDlgItem( hWnd, IDC_GMNOTICE_EDT_PREFACE ), EM_LIMITTEXT, MAX_PREFACE_LENGTH, 0 );
			SendMessage( GetDlgItem( hWnd, IDC_GMNOTICE_EDT_NOTICE ), EM_LIMITTEXT, MAX_CHAT_LENGTH - MAX_PREFACE_LENGTH, 0 );
			SetFocus( GetDlgItem( hWnd, IDC_GMNOTICE_EDT_PREFACE ) );
		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMNOTICE_OK:
					{
						GMTOOLMGR->OnNoticeCommand();
					}
					break;
				case IDC_GMNOTICE_CANCEL:
					{
						GMTOOLMGR->OnClickMenu( eMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;
				case IDC_GMNOTICE_BTN_ALLMAP:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMNOTICE_CMB_MAP ), FALSE );
					}
					break;
				case IDC_GMNOTICE_BTN_MAP:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_GMNOTICE_CMB_MAP ), TRUE );
					}
					break;
				}
			}
		}
		return TRUE;
	case WM_CLOSE:
		{
			GMTOOLMGR->OnClickMenu( eMenu_Count );
			SetFocus( _g_hWnd );
		}
		return TRUE;

	case WM_SETFOCUS:
		{
#ifdef TAIWAN_LOCAL
		HIMC hIMC = ImmGetContext( _g_hWnd );
		ImmSetOpenStatus( hIMC, TRUE );
		ImmReleaseContext( _g_hWnd, hIMC );

		HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
		SendMessage( hWndIme, WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0 );
#endif
		}
		break;
	case WM_KILLFOCUS:
		{
#ifdef TAIWAN_LOCAL
			HIMC hIMC = ImmGetContext( _g_hWnd );
			ImmSetOpenStatus( hIMC, FALSE );
			ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			ImmNotifyIME( hIMC, NI_CLOSECANDIDATE, 0, 0 );
			ImmReleaseContext( _g_hWnd, hIMC );

			HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
			SendMessage( hWndIme, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0 );
#endif
		}
		break;
	}

	return FALSE;
}


INT_PTR CALLBACK GMSubEventMapDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckDlgButton( hWnd, IDC_GMEVENTMAP_BTN_USER, BST_CHECKED );
			CheckDlgButton( hWnd, IDC_GMEVENTMAP_BTN_CHNANNEL, BST_CHECKED );
			CheckDlgButton( hWnd, IDC_GMEVENTMAP_BTN_TEAM, BST_CHECKED );
			CheckDlgButton( hWnd, IDC_GMEVENTMAP_BTN_CHNANNEL2, BST_CHECKED );
			SendMessage( GetDlgItem( hWnd, IDC_GMEVENTMAP_EDT_CHAR ), EM_LIMITTEXT, MAX_NAME_LENGTH+1, 0 );
		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMEVENTMAP_BTN_CLOSE:
					{
						GMTOOLMGR->OnClickMenu( eMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;
				case IDC_GMEVENTMAP_BTN_USER:
					{
						CObject* pObject = OBJECTMGR->GetSelectedObject();
						if( pObject )
						if( pObject->GetObjectKind() == eObjectKind_Player )
						{
							SetDlgItemText( hWnd, IDC_GMEVENTMAP_EDT_CHAR, pObject->GetObjectName() );
						}
					}
					break;
				}
			}
		}
		break;
	}

	return FALSE;
}


INT_PTR CALLBACK GMSubEventNotifyDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckDlgButton( hWnd, IDC_GMEVENTNOTIFY_BTN_NOTIFY, BST_CHECKED );
//			CheckDlgButton( hWnd, IDC_GMEVENTNOTIFY_BTN_SOUND, BST_CHECKED );
			SendMessage( GetDlgItem( hWnd, IDC_GMEVENTNOTIFY_EDT_TITLE ), EM_LIMITTEXT, 10, 0 );
			SendMessage( GetDlgItem( hWnd, IDC_GMEVENTNOTIFY_EDT_CONTEXT ), EM_LIMITTEXT, 40, 0 );
		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMEVENTNOTIFY_BTN_ON:
					{
						GMTOOLMGR->OnEventNotifyCommand( TRUE );
					}
					break;
				case IDC_GMEVENTNOTIFY_BTN_OFF:
					{
						GMTOOLMGR->OnEventNotifyCommand( FALSE );
					}
					break;
				case IDC_GMEVENTNOTIFY_BTN_CLOSE:
					{
						GMTOOLMGR->OnClickMenu( eMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;
				}
			}
		}
		break;
	}

	return FALSE;
}

INT_PTR CALLBACK GMWeatherDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMWEATHER_BTN_APPLY:
					{
						char weatherTxt[MAX_PATH]={0,};
						int nSel = SendDlgItemMessage( hWnd, IDC_GMWEATHER_CMB_TYPE, CB_GETCURSEL, 0, 0 );
						SendDlgItemMessage( hWnd, IDC_GMWEATHER_CMB_TYPE, CB_GETLBTEXT, nSel, (LPARAM)weatherTxt );

						BOOL rt;
						int nIntensity = GetDlgItemInt( hWnd, IDC_GMWEATHER_EDIT_INTENSITY, &rt, TRUE );
						if( !rt )	break;

						WEATHERMGR->EffectOn( weatherTxt, WORD(nIntensity));
						CHATMGR->AddMsg( CTC_TOGM, "Weather Effect On : %s[%d]", weatherTxt, nIntensity );
					}
					break;
				case IDC_GMWEATHER_BTN_STOP:
					{
						WEATHERMGR->EffectOff();
						CHATMGR->AddMsg( CTC_TOGM, "Weather Effect Off" );
					}
					break;

				case IDC_GMWEATHER_BTN_SHOW_INFO:
					{
						WEATHERMGR->ShowDebugState( TRUE );
					}
					break;
				case IDC_GMWEATHER_BTN_HIDE_INFO:
					{
						WEATHERMGR->ShowDebugState( FALSE );
					}
					break;
				}
			}
		}
		break;
	}

	return FALSE;
}

INT_PTR CALLBACK GMDungeonDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMDUNGEON_BTN_GETINFO:
					{
						GMTOOLMGR->OnDungeonGetInfo_Syn();
					}
					break;

				case IDC_GMDUNGEON_BTN_GETDETAILINFO:
					{
						int nSel = SendDlgItemMessage( hWnd, IDC_GMDUNGEON_LSB_DUNGEONLIST, LB_GETCURSEL, 0, 0 );

						if(nSel<1 || MAX_DUNGEON_NUM<nSel)
							return FALSE;

						GMTOOLMGR->OnDungeonGetDetailInfo_Syn(nSel);
					}
					break;

				case IDC_GMDUNGEON_BTN_CLEARDETAILINFO:
					{
						GMTOOLMGR->DungeonClearDetailInfo();
					}
					break;

				case IDC_GMDUNGEON_BTN_MORNITORING:
					{
						int nSel = SendDlgItemMessage( hWnd, IDC_GMDUNGEON_LSB_DUNGEONLIST, LB_GETCURSEL, 0, 0 );

						if(nSel<1 || MAX_DUNGEON_NUM<nSel)
							return FALSE;

						GMTOOLMGR->OnDungeonMornitoring_Syn(nSel);
					}
					break;
				}
			}
		}
		break;
	default:
		break;
	}

	return FALSE;
}



WNDPROC OldEditProc;
WNDPROC OldChatProc;
HWND hWndComboID;

INT_PTR CALLBACK GMSubCounselDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			//입력에디트 제한글자 설정 //focus
			SendMessage( GetDlgItem( hWnd, IDC_GMCOUNSEL_EDT_INPUT ), EM_LIMITTEXT, 127, 0 );
			SetFocus( GetDlgItem( hWnd, IDC_GMCOUNSEL_EDT_INPUT ) );

			OldEditProc = (WNDPROC)SetWindowLong( GetDlgItem( hWnd, IDC_GMCOUNSEL_EDT_INPUT ), 
												  GWL_WNDPROC, (LONG)GMCounselEditProc);

			OldChatProc = (WNDPROC)SetWindowLong( GetDlgItem( hWnd, IDC_GMCOUNSEL_EDT_CHAT ), 
												  GWL_WNDPROC, (LONG)GMCounselChatProc);

			hWndComboID = GetDlgItem( hWnd, IDC_GMCOUNSEL_CMB_ID );
			SendMessage( hWndComboID, CB_INSERTSTRING, 0, (LPARAM)"----------------" );

			GMTOOLMGR->SetChatListHwnd( GetDlgItem( hWnd, IDC_GMCOUNSEL_EDT_CHAT ) );
		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_GMCOUNSEL_BTN_SAVE:
					{
						char FileName[MAX_PATH] = "";
						OPENFILENAME OFN;
						memset( &OFN, 0, sizeof(OPENFILENAME) );
						OFN.lStructSize = sizeof(OPENFILENAME);
						OFN.hwndOwner	= hWnd;					
						OFN.lpstrFilter = "Text Files(*.txt)\0*.txt\0";
						OFN.lpstrFile	= FileName;
						OFN.nMaxFile	= MAX_PATH;
						OFN.lpstrDefExt = "txt";
						OFN.Flags		= OFN_OVERWRITEPROMPT;
						if( GetSaveFileName(&OFN) != 0 )
						{
							GMTOOLMGR->CaptureChat( FileName );
						}
					}
					break;
				}
			}
		}
		return TRUE;
	case WM_CLOSE:
		{

			GMTOOLMGR->OnClickMenu( eMenu_Count );
			SetFocus( _g_hWnd );
	
		}
		return TRUE;
	case WM_DESTROY:
		{
			SetWindowLong( GetDlgItem( hWnd, IDC_GMCOUNSEL_EDT_INPUT ), GWL_WNDPROC, (LONG)OldEditProc );
			SetWindowLong( GetDlgItem( hWnd, IDC_GMCOUNSEL_EDT_CHAT ), GWL_WNDPROC, (LONG)OldChatProc );
		}
		return TRUE;
	case WM_SETFOCUS:
		{
#ifdef TAIWAN_LOCAL
		HIMC hIMC = ImmGetContext( _g_hWnd );
		ImmSetOpenStatus( hIMC, TRUE );
		ImmReleaseContext( _g_hWnd, hIMC );

		HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
		SendMessage( hWndIme, WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0 );
#endif

		}
		break;
	case WM_KILLFOCUS:
		{
#ifdef TAIWAN_LOCAL
			HIMC hIMC = ImmGetContext( _g_hWnd );
			ImmSetOpenStatus( hIMC, FALSE );
			ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			ImmNotifyIME( hIMC, NI_CLOSECANDIDATE, 0, 0 );
			ImmReleaseContext( _g_hWnd, hIMC );

			HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
			SendMessage( hWndIme, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0 );
#endif
		}
		break;
	}

	return FALSE;
}

LRESULT CALLBACK GMCounselEditProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_KEYDOWN:
		{
			if( wParam == VK_RETURN )
			{
				char buf[128];
				GetWindowText( hWnd, buf, 127 );
				if( buf[0] == 0 ) return TRUE;
				
				char Name[MAX_NAME_LENGTH+2] = {0,};

				int nLen = GetWindowText( hWndComboID, Name, MAX_NAME_LENGTH+1 );
				if( nLen < 4 || nLen > MAX_NAME_LENGTH )
					return TRUE;
		
				// 아이디확인 - RaMa 04.10.20
				if( (FILTERTABLE->IsInvalidCharInclude((unsigned char*)Name)) == TRUE )
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(368));
					return TRUE;
				}
					
				//---귓말 보내기
				MSG_CHAT data;
				data.Category	= MP_CHAT;
				data.Protocol	= MP_CHAT_WHISPER_SYN;
				data.dwObjectID	= gHeroID;
				SafeStrCpy( data.Name, Name, MAX_NAME_LENGTH+1 );
				SafeStrCpy( data.Msg, buf, MAX_CHAT_LENGTH+1 );
				//		NETWORK->Send(&data,sizeof(data));
				NETWORK->Send(&data,data.GetMsgLength());	//CHATMSG 040324
				
				SetWindowText( hWnd, "" );

				return TRUE;
			}
			else if( wParam == VK_F8 )
			{
				GMTOOLMGR->RemoveIdFromList();
			}
		}
		break;
	}

	return CallWindowProc( OldEditProc, hWnd, message, wParam, lParam );
}


LRESULT CALLBACK GMCounselChatProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_CHAR:
	case WM_LBUTTONDOWN:
		return TRUE;
	}

	return CallWindowProc( OldChatProc, hWnd, message, wParam, lParam );
}

void CGMToolManager::RemoveIdFromList()
{
	char Name[MAX_NAME_LENGTH+2] = {0,};
	int nLen = GetWindowText( hWndComboID, Name, MAX_NAME_LENGTH+1 );
	if( nLen < 4 || nLen > MAX_NAME_LENGTH )
		return;
				
	int nIndex = SendMessage( hWndComboID, CB_FINDSTRING, 0, (LPARAM)Name );
	if( nIndex != CB_ERR && nIndex != m_nNumNow )
	{
		SendMessage( hWndComboID, CB_DELETESTRING, nIndex, 0 );
		SendMessage( hWndComboID, CB_SETCURSEL, 0, 0 );
				
		if( nIndex > m_nNumNow )
			--m_nNumWait;
		else
			--m_nNumNow;
	}
}

void CGMToolManager::AddChatMsg( char* pName, char* pMsg, int nKind )
{
	if( !m_hWndChatList ) return;

	char buf[160];

	if( nKind == 1 )	//했다.
	{
		wsprintf( buf, "*%-20s: %s", pName, pMsg );
		
		int nIndex = SendMessage( hWndComboID, CB_FINDSTRING, 0, (LPARAM)pName );
		if( nIndex == CB_ERR )
		{
			SendMessage( hWndComboID, CB_INSERTSTRING, 0, (LPARAM)pName );
			++m_nNumNow;			
		}
		else if( nIndex != 0 )
		{
			SendMessage( hWndComboID, CB_DELETESTRING, nIndex, 0 );
			SendMessage( hWndComboID, CB_INSERTSTRING, 0, (LPARAM)pName );
			SendMessage( hWndComboID, CB_SETCURSEL, 0, 0 );
		}
		
		if( m_nNumNow > 10 )
		{
			SendMessage( hWndComboID, CB_DELETESTRING, 10, 0 );	//제일 아래것 지우기
			--m_nNumNow;
		}
		
	}
	else if( nKind == 2 )	//받았다.
	{
		wsprintf( buf, "FROM %-16s: %s", pName, pMsg );

		int nIndex = SendMessage( hWndComboID, CB_FINDSTRING, 0, (LPARAM)pName );
		if( nIndex == CB_ERR )
		{
			SendMessage( hWndComboID, CB_INSERTSTRING, m_nNumNow, (LPARAM)pName );
			++m_nNumWait;
		}

		int nCount = SendMessage( hWndComboID, CB_GETCOUNT, 0, 0 );

		if( m_nNumWait > 10 )
		{
			SendMessage( hWndComboID, CB_DELETESTRING, nCount-1, 0 );	//제일 아래것 지우기
			--m_nNumWait;
		}
	}
	else if( nKind == 3 )	//에러
	{
		wsprintf( buf, "%22s - %s -", pName, pMsg );
	}

	int nLen = strlen( buf );
	buf[nLen]	= 13;
	buf[nLen+1] = 10;
	buf[nLen+2] = 0;

	SendMessage( m_hWndChatList, EM_REPLACESEL, FALSE, (LPARAM)buf );

	strcpy( m_strChatList+m_nBufLen, buf );
	m_nBufLen += (nLen+2);

	//너무 많이 차면 지우자.
	if( m_nBufLen > 1024*10 )
	{
		char FileName[MAX_PATH];
		SYSTEMTIME ti;
		GetLocalTime( &ti );
		wsprintf( FileName, "GM_Counsel/%s%02d%02d%02d.txt", HERO->GetObjectName(), ti.wYear, ti.wMonth, ti.wDay );
		m_nBufLen -= 1024;

		DIRECTORYMGR->SetLoadMode(eLM_Root);
		CreateDirectory( "GM_Counsel", NULL );

		HANDLE	hFile;
		hFile = CreateFile( FileName, GENERIC_WRITE, 0, NULL, 
				OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

		if( hFile == INVALID_HANDLE_VALUE )
			return;

		DWORD dwWritten;
		SetFilePointer( hFile, 0, NULL, FILE_END );
		WriteFile( hFile, m_strChatList, 1024, &dwWritten, NULL );
		CloseHandle( hFile );
		
		memmove( m_strChatList, m_strChatList+1024, m_nBufLen );
		m_strChatList[m_nBufLen] = 0;
		SetWindowText( m_hWndChatList, m_strChatList );
		SendMessage( m_hWndChatList, EM_SETSEL, m_nBufLen, m_nBufLen );
	}
}

void CGMToolManager::CaptureChat( char* FileName )
{
	HANDLE	hFile;
	hFile = CreateFile( FileName, GENERIC_WRITE, 0, NULL, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
								
	if( hFile == INVALID_HANDLE_VALUE )
		return;
								
	DWORD dwWritten;
	WriteFile( hFile, m_strChatList, m_nBufLen, &dwWritten, NULL );
	CloseHandle( hFile );
}

void CGMToolManager::SaveChatList()
{
	if( m_strChatList[0] == 0 ) return;

	char FileName[MAX_PATH];
	SYSTEMTIME ti;
	GetLocalTime( &ti );
	
	CHero* hero = OBJECTMGR->GetHero();

	if( hero )
	{
		wsprintf( FileName, "GM_Counsel/%s%02d%02d%02d.txt", hero->GetObjectName(), ti.wYear, ti.wMonth, ti.wDay );
	}
	
	
	DIRECTORYMGR->SetLoadMode(eLM_Root);
	CreateDirectory( "GM_Counsel", NULL );
	
	HANDLE	hFile;
	hFile = CreateFile( FileName, GENERIC_WRITE, 0, NULL, 
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	
	if( hFile == INVALID_HANDLE_VALUE )
		return;
	
	DWORD dwWritten;
	SetFilePointer( hFile, 0, NULL, FILE_END );
	WriteFile( hFile, m_strChatList, m_nBufLen, &dwWritten, NULL );
	CloseHandle( hFile );

	m_strChatList[0] = 0;
	m_nBufLen = 0;
	
	SetWindowText( m_hWndChatList, "" );
}

void CGMToolManager::AddWeatherType( LPCTSTR weatherName )
{
	SendDlgItemMessage( m_hWndSub[eMenu_Weather], IDC_GMWEATHER_CMB_TYPE, CB_ADDSTRING, 0, (LPARAM)weatherName );
}

BOOL CGMToolManager::OnDungeonGetInfo_Syn()
{
	if( !m_hWndSub[eMenu_Dungeon] ) return FALSE;
	SetFocus( _g_hWnd );

	char buf[256] = {0,};
	int nLen = GetDlgItemText( m_hWndSub[eMenu_Dungeon], IDC_GMDUNGEON_CMB_MAPLIST, buf, MAX_MAP_NAME_LENGTH+1 );
	if( nLen == 0 ) return FALSE;
	buf[MAX_MAP_NAME_LENGTH] = 0;
	WORD wMapNum = GetMapNumForName( buf );
	if( wMapNum == 0 ) return FALSE;

	MSG_WORD2 msg;
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_DUNGEON_GETINFOALL_SYN;
	msg.dwObjectID	= gHeroID;
	msg.wData1 = wMapNum;
	msg.wData2 = 0;

	NETWORK->Send( &msg, sizeof(msg) );

	return TRUE;
}

void CGMToolManager::DungeonGetInfo_Ack(MSGBASE* pMsg)
{
	MSG_DUNGEON_INFO_ALL* pmsg = (MSG_DUNGEON_INFO_ALL*)pMsg;

	if(pmsg->dwObjectID != gHeroID)
		return;

	int nIndex = 0;
	char buf[256] = {0,};

	SendDlgItemMessage( m_hWndSub[eMenu_Dungeon], IDC_GMDUNGEON_LSB_DUNGEONLIST, LB_RESETCONTENT, 0, 0 );

	if(! pmsg->dwDungeonNum)
	{
		SendDlgItemMessage( m_hWndSub[eMenu_Dungeon], IDC_GMDUNGEON_LSB_DUNGEONLIST, LB_INSERTSTRING, WPARAM(-1), (LPARAM)"No Active Dungeons" );
	}
	else
	{
		sprintf(buf, "Active Dungeons : %d, Players in Dungeons : %d", pmsg->dwDungeonNum, pmsg->dwUserNum);
		SendDlgItemMessage( m_hWndSub[eMenu_Dungeon], IDC_GMDUNGEON_LSB_DUNGEONLIST, LB_INSERTSTRING, nIndex, (LPARAM)buf);

		for(DWORD i=0; i<pmsg->dwDungeonNum; i++)
		{
			sprintf(buf, "[%03d] : Channel(%d), PartyIdx(%d), PlayerNum(%d), Point(%d), ", 
				i, pmsg->DungeonInfo[i].dwChannel, pmsg->DungeonInfo[i].dwPartyIndex, pmsg->DungeonInfo[i].dwJoinPlayerNum,
				pmsg->DungeonInfo[i].dwPoint);
			if(pmsg->DungeonInfo[i].difficulty == eDIFFICULTY_EASY)
				strcat(buf, "Level(Easy)");
			else if(pmsg->DungeonInfo[i].difficulty == eDIFFICULTY_NORMAL)
				strcat(buf, "Level(Normal)");
			else if(pmsg->DungeonInfo[i].difficulty == eDIFFICULTY_HARD)
				strcat(buf, "Level(Hard)");
			else
				strcat(buf, "Level(None)");
			
			nIndex = SendDlgItemMessage( m_hWndSub[eMenu_Dungeon], IDC_GMDUNGEON_LSB_DUNGEONLIST, LB_INSERTSTRING, WPARAM(-1), (LPARAM)buf );
			SendDlgItemMessage( m_hWndSub[eMenu_Dungeon], IDC_GMDUNGEON_LSB_DUNGEONLIST, LB_SETITEMDATA, nIndex, (LPARAM)pmsg->DungeonInfo[i].dwChannel );
		}
	}

}

BOOL CGMToolManager::OnDungeonGetDetailInfo_Syn(int nIndex)
{
	if( !m_hWndSub[eMenu_Where] ) return FALSE;
	SetFocus( _g_hWnd );

	char buf[256] = {0,};
	int nLen = GetDlgItemText( m_hWndSub[eMenu_Dungeon], IDC_GMDUNGEON_CMB_MAPLIST, buf, MAX_MAP_NAME_LENGTH+1 );
	if( nLen == 0 ) return FALSE;
	buf[MAX_MAP_NAME_LENGTH] = 0;
	WORD wMapNum = GetMapNumForName( buf );
	if( wMapNum == 0 ) return FALSE;

	WORD wChannel = WORD(SendDlgItemMessage( m_hWndSub[eMenu_Dungeon], IDC_GMDUNGEON_LSB_DUNGEONLIST, LB_GETITEMDATA, nIndex, 0 ));
	if( wChannel == 0) return FALSE;

	MSG_WORD3 msg;
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_DUNGEON_GETINFOONE_SYN;
	msg.dwObjectID	= gHeroID;
	msg.wData1 = wMapNum;
	msg.wData2 = wChannel;
	msg.wData3 = 0;

	NETWORK->Send( &msg, sizeof(msg) );

	return TRUE;
}

void CGMToolManager::DungeonGetDetailInfo_Ack(MSGBASE* pMsg)
{
	MSG_DUNGEON_INFO_ONE* pmsg = (MSG_DUNGEON_INFO_ONE*)pMsg;

	if(pmsg->dwObjectID != gHeroID)
		return;

	char buf[256] = {0,};
	strcpy(&m_strDetailInfo[0], "");
	sprintf(&m_strDetailInfo[0], "- Player List\r\n");

	int i;
	PARTY_MEMBER member;
	for(i=0; i<MAX_PARTY_LISTNUM; i++)
	{
		member = pmsg->DungeonInfo.PartyMember[i];
		if(member.dwMemberID)
		{
			const WORD index = ( 1 < member.mJobGrade ? member.mJob[ member.mJobGrade - 1 ] : 1 );
			const WORD job = ( member.mJob[ 0 ] * 1000 ) + ( ( member.mRace + 1 ) * 100 ) + ( member.mJobGrade * 10 ) + index;
			char* pClass = RESRCMGR->GetMsg(RESRCMGR->GetClassNameNum(job));

			sprintf(buf, "[%s (%d)] : %s\r\n", member.Name, member.Level, pClass);
			strcat(&m_strDetailInfo[0], buf);
		}
	}
	strcat(m_strDetailInfo, "\r\n");

	strcat(m_strDetailInfo, "- Active Warp List\r\n");
	for(i=0; i<MAX_DUNGEON_WARP; i++)
	{
		if(pmsg->DungeonInfo.warpState[i].dwIndex &&
			pmsg->DungeonInfo.warpState[i].bActive)
		{
			sprintf(buf, "[Warp %02d] : ON\r\n", i);
			strcat(&m_strDetailInfo[0], buf);
		}
	}
	strcat(m_strDetailInfo, "\r\n");

	strcat(m_strDetailInfo, "- Active Switch List\r\n");
	for(i=0; i<MAX_DUNGEON_SWITCH; i++)
	{
		if(pmsg->DungeonInfo.switchState[i])
		{
			sprintf(buf, "[Switch %02d] : ON\r\n", i);
			strcat(&m_strDetailInfo[0], buf);
		}
	}

	SetWindowText( GetDlgItem( m_hWndSub[eMenu_Dungeon], IDC_GMDUNGEON_EDT_DETAILINFO ), &m_strDetailInfo[0] );
}

void CGMToolManager::DungeonClearDetailInfo()
{
	SetWindowText( GetDlgItem( m_hWndSub[eMenu_Dungeon], IDC_GMDUNGEON_EDT_DETAILINFO ), "" );
}

BOOL CGMToolManager::OnDungeonMornitoring_Syn(int nIndex)
{
	if( !m_hWndSub[eMenu_Dungeon] ) return FALSE;
	SetFocus( _g_hWnd );

	char buf[256] = {0,};
	int nLen = GetDlgItemText( m_hWndSub[eMenu_Dungeon], IDC_GMDUNGEON_CMB_MAPLIST, buf, MAX_MAP_NAME_LENGTH+1 );
	if( nLen == 0 ) return FALSE;
	buf[MAX_MAP_NAME_LENGTH] = 0;
	WORD wMapNum = GetMapNumForName( buf );
	if( wMapNum == 0 || wMapNum == MAP->GetMapNum()) return FALSE;

	WORD wChannel = WORD(SendDlgItemMessage( m_hWndSub[eMenu_Dungeon], IDC_GMDUNGEON_LSB_DUNGEONLIST, LB_GETITEMDATA, nIndex, 0 ));
	if( wChannel == 0) return FALSE;

	MSG_WORD3 msg;
	msg.Category	= MP_CHEAT;
	msg.Protocol	= MP_CHEAT_DUNGEON_OBSERVER_SYN;
	msg.dwObjectID	= gHeroID;
	msg.wData1 = wMapNum;
	msg.wData2 = wChannel;
	msg.wData3 = 0;

	NETWORK->Send( &msg, sizeof(msg) );

	return TRUE;
}

void CGMToolManager::SetVehicle(const MSG_SKILL_LIST& message)
{
	SendDlgItemMessage(
		m_hWndSub[eMenu_Mob],
		IDC_GMREGEN_VEHICLE_DROPDOWN,
		CB_RESETCONTENT,
		0,
		0);

	for(DWORD index = 0; message.mSize > index; ++index)
	{
		const SKILL_BASE& data = message.mData[index];

		const VECTOR3 position = {
			LOWORD(data.dwDBIdx),
			0,
			HIWORD(data.dwDBIdx),
		};
		const DWORD monsterKind = data.wSkillIdx;
		const BASE_MONSTER_LIST* const baseMonsterList = GAMERESRCMNGR->GetMonsterListInfo(
			monsterKind);

		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			"[%05.0f, %05.0f] %s",
			position.x / 100.0f,
			position.z / 100.0f,
			baseMonsterList ? baseMonsterList->Name : "?");
		SendDlgItemMessage(
			m_hWndSub[eMenu_Mob],
			IDC_GMREGEN_VEHICLE_DROPDOWN,
			CB_ADDSTRING,
			0,
			LPARAM(text));
	}

	SendDlgItemMessage(
		m_hWndSub[eMenu_Mob],
		IDC_GMREGEN_VEHICLE_DROPDOWN,
		CB_SETCURSEL,
		0,
		0);
}

void CGMToolManager::InitNpcData()
{
	m_NpcList.clear();
}

void CGMToolManager::SetNpcData( DWORD dwIndex, const char* pName, VECTOR3 pos )
{
	NPCINFO	NpcInfo;
	ZeroMemory( &NpcInfo, sizeof(NpcInfo) );

	NpcInfo.Index = dwIndex;
	SafeStrCpy( NpcInfo.Name, pName, MAX_NAME_LENGTH + 1 );
	NpcInfo.Pos.x = pos.x;
	NpcInfo.Pos.z = pos.z;

	m_NpcList.push_back( NpcInfo );
}

void CGMToolManager::SetNpcList()
{
	SendDlgItemMessage( m_hWndSub[eMenu_Move], IDC_GMMOVE_CMB_NPC, CB_RESETCONTENT, 0, 0 );

	for( DWORD i = 0; i < m_NpcList.size(); i++ )
	{
		SendDlgItemMessage( m_hWndSub[eMenu_Move], IDC_GMMOVE_CMB_NPC, CB_ADDSTRING, 0, (LPARAM)m_NpcList[i].Name );
	}

	SendDlgItemMessage( m_hWndSub[eMenu_Move], IDC_GMMOVE_CMB_NPC, CB_SETCURSEL, 0, 0 );
}

void CGMToolManager::ActiveMiniMap( BOOL val )
{
	CBigMapDlg* pDlg = GAMEIN->GetBigMapDialog();
	if( pDlg )
		pDlg->SetActive( val );
}

BOOL CGMToolManager::MoveOnMiniMap( float pos_x, float pos_z )
{
	if( !IsDlgButtonChecked(m_hWndDlg, IDC_GM_MNBTN_MOVE ) ) return FALSE;
	if( !SendMessage( GetDlgItem( m_hWndSub[eMenu_Move], IDC_GMMOVE_CHK_MINIMAP_ON ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
		return FALSE;

	VECTOR3 destPos = {0};

	destPos.x = pos_x * 100.0f;
	destPos.y = 0.0f;
	destPos.z = pos_z * 100.0f;

	destPos.x = (int(destPos.x / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;
	destPos.y = 0;
	destPos.z = (int(destPos.z / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;

	if(MAP->CollisionCheck_OneTile( &destPos ) == FALSE)
		return TRUE;

	if( SendMessage( GetDlgItem( m_hWndSub[eMenu_Move], IDC_GMMOVE_BTN_ME ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		MOVE_POS msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_MOVE_SYN;
		msg.dwObjectID	= gHeroID;
		msg.dwMoverID	= gHeroID;
		msg.cpos.Compress(&destPos);
		NETWORK->Send(&msg,sizeof(msg));
	}
	else
	{
		char buf[MAX_NAME_LENGTH+1] = {0,};
		int nLen = GetDlgItemText( m_hWndSub[eMenu_Move], IDC_GMMOVE_EDT_USER, buf, MAX_NAME_LENGTH+1 );
		if( nLen > MAX_NAME_LENGTH || nLen < 4  ) return TRUE;
		if( (FILTERTABLE->IsInvalidCharInclude((unsigned char*)buf)) == TRUE )
			return TRUE;

		MOVE_POS_USER msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_MOVEUSER_SYN;
		msg.dwObjectID	= gHeroID;
		SafeStrCpy( msg.Name, buf, MAX_NAME_LENGTH + 1 );
		msg.cpos.Compress(&destPos);
		NETWORK->Send(&msg,sizeof(msg));
	}
	return TRUE;
}
#endif //_GMTOOL_
