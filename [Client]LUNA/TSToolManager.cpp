// TSToolManager.cpp - iros
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _TESTCLIENT_

#include "TSToolManager.h"
#include "GameIn.h"
#include "InventoryExDialog.h"
#include "ObjectManager.h"
#include "ItemManager.h"
#include "CharMakeManager.h"
#include "AppearanceManager.h"
#include "cSkillBase.h"
#include "cSkillTreeManager.h"
#include "QuickManager.h"
#include "cQuickItem.h"
#include "MapChange.h"
#include "MainGame.h"
#include "MoveManager.h"
#include "cResourceManager.h"
#include "Quest.h"
#include "../[CC]Quest/QuestInfo.h"
#include "../[CC]Quest/SubQuestInfo.h"
#include "../[CC]Quest/QuestLimit.h"
#include "../[CC]Quest/QuestLimitKind.h"
#include "../[CC]Quest/QuestLimitKind_Level.h"
#include "../[CC]Quest/QuestLimitKind_Quest.h"
#include "../[CC]Quest/QuestLimitKind_Stage.h"

#include "../[CC]Quest/QuestTrigger.h"
#include "../[CC]Quest/QuestCondition.h"
#include "../[CC]Quest/QuestEvent.h"
#include "../[CC]Quest/QuestExecute.h"
#include "../[CC]Quest/QuestExecute_Count.h"
#include "../[CC]Quest/QuestExecute_Item.h"
#include "../[CC]Quest/QuestExecute_Quest.h"
#include "../[CC]Quest/QuestExecute_Time.h"

#include "cPage.h"
#include "NpcScriptManager.h"
#include "../[CC]Quest/QuestNpc.h"
#include "../[CC]Quest/QuestNpcData.h"
#include "../[CC]Quest/QuestNpcScript.h"

#include "QuestManager.h"
#include "NpcRecallMgr.h"

#include "../[CC]Skill/Client/Manager/SkillManager.h"

#include "../[CC]Skill/Client//Info/SkillInfo.h"
#include "../[CC]Skill/Client//Info/ActiveSkillInfo.h"
#include "../[CC]Skill/Client//Info/BuffSkillInfo.h"
#include "../[CC]Skill/Client//Object/SkillObject.h"
#include "../[CC]Skill/Client//Object/BuffSkillObject.h"
#include "WeatherManager.h"
#include "ChatManager.h"
#include "MHMap.h"
#include "Commdlg.h"
#include <CommCtrl.h>
#include "MHFile.h"

extern HWND _g_hWnd;
extern HINSTANCE g_hInst;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLOBALTON(CTSToolManager)

extern int g_MapChannelNum[100];

CTSToolManager::CTSToolManager()
{
	m_hWndDlg = NULL;
	m_hWndOutDlg = NULL;
	ZeroMemory( m_hWndSub, sizeof( m_hWndSub ) );

	m_nSelectMenu	= eTSMenu_Count;
	m_bShow			= FALSE;
	m_bSubShow		= FALSE;
	m_cbChannelCount	= 0;

	m_lBossStartIndex	= 0;
}

CTSToolManager::~CTSToolManager()
{

}

BOOL CTSToolManager::CreateTSDialog()
{
	if( m_hWndDlg ) return FALSE;

	m_hWndDlg = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_TSDIALOG), _g_hWnd, TSDlgProc );
	m_hWndOutDlg = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_TS_OUTPUT_DIALOG), _g_hWnd, TSOutDlgProc );

	CreateTSSubDialog();

	//	CHEATMGR->SetCheatEnable( TRUE );
	return TRUE;
}

BOOL CTSToolManager::CreateTSSubDialog()
{
	m_hWndSub[eTSMenu_Char]	 = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_TS_CHAR_DIALOG), m_hWndDlg, TSSubCharDlgProc );
	m_hWndSub[eTSMenu_Item]	 = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_TS_ITEM_DIALOG), m_hWndDlg, TSSubItemDlgProc );
	m_hWndSub[eTSMenu_Mob]	 = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_TS_REGEN_DIALOG), m_hWndDlg, TSSubRegenDlgProc );
	m_hWndSub[eTSMenu_Move]	 = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_TS_MOVE_DIALOG), m_hWndDlg, TSSubMoveDlgProc );
	m_hWndSub[eTSMenu_Quest] = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_TS_QUEST_DIALOG), m_hWndDlg, TSSubQuestDlgProc );
	m_hWndSub[eTSMenu_Weather] = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_TS_WEATHER_DIALOG), m_hWndDlg, TSSubWeatherDlgProc );
	return TRUE;
}

BOOL CTSToolManager::DestroyTSDialog()
{
	if( !m_hWndDlg ) return FALSE;

	//DestroyWindow( m_hWndDlg );

	m_hWndDlg = NULL;
	m_hWndOutDlg = NULL;

	DestroyTSSubDialog();

	m_nSelectMenu = eTSMenu_Count;

	return TRUE;
}

BOOL CTSToolManager::DestroyTSSubDialog()
{
	for( int i = 0 ; i < eTSMenu_Count ; ++i )
	{
		if( m_hWndSub[i] )
		{
			//			DestroyWindow( m_hWndSub[i] );
			m_hWndSub[i] = NULL;
		}
	}

	return TRUE;
}


void CTSToolManager::ShowTSDialog( BOOL bShow, BOOL bUpdate )
{
	if( !m_hWndDlg )		return;

	if( bShow )
	{
		ShowWindow( m_hWndDlg, SW_SHOWNA );
		SetPositionByMainWindow();

		if (m_hWndOutDlg)
		{
			ShowWindow( m_hWndOutDlg, SW_SHOWNA );
			SetPositionByOutputWindow();
		}
	}
	else
	{
		TSTOOLMGR->OnClickMenu( eTSMenu_Count );
		ShowWindow( m_hWndDlg, SW_HIDE );

		if (m_hWndOutDlg)
		{
			ShowWindow( m_hWndOutDlg, SW_HIDE );
		}
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
		//		if( m_hWndSub[eTSMenu_Char]
		{
			for( i = 0 ; i < 3 ; ++i)
			{
				wsprintf( bufNum, "%d", i );
				SendDlgItemMessage( m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_RACE, CB_ADDSTRING, 0, (LPARAM)bufNum );
			}
			for( i = 0 ; i < 2 ; ++i)
			{
				wsprintf( bufNum, "%d", i );
				SendDlgItemMessage( m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_GENDER, CB_ADDSTRING, 0, (LPARAM)bufNum );
			}
			for( i = 0 ; i < 20 ; ++i)
			{
				wsprintf( bufNum, "%d", i );
				SendDlgItemMessage( m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_JOB, CB_ADDSTRING, 0, (LPARAM)bufNum );
			}

			for( i = 0 ; i < 13 ; ++i)
			{
				wsprintf( bufNum, "%d", i );
				SendDlgItemMessage( m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_HAIR, CB_ADDSTRING, 0, (LPARAM)bufNum );
			}

			for( i = 0 ; i < 13 ; ++i)
			{
				wsprintf( bufNum, "%d", i );
				SendDlgItemMessage( m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_FACE, CB_ADDSTRING, 0, (LPARAM)bufNum );
			}

			SendDlgItemMessage( m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_RACE, CB_SETCURSEL, (WPARAM)GAMERESRCMNGR->m_TestClientInfo.Race, 0 );
			SendDlgItemMessage( m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_GENDER, CB_SETCURSEL, (WPARAM)GAMERESRCMNGR->m_TestClientInfo.Gender, 0 );
			SendDlgItemMessage( m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_JOB, CB_SETCURSEL, (WPARAM)GAMERESRCMNGR->m_TestClientInfo.Job, 0 );
			SendDlgItemMessage( m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_HAIR, CB_SETCURSEL, (WPARAM)GAMERESRCMNGR->m_TestClientInfo.HairType, 0 );
			SendDlgItemMessage( m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_FACE, CB_SETCURSEL, (WPARAM)GAMERESRCMNGR->m_TestClientInfo.FaceType, 0 );
		}

		//		if( m_hWndSub[eMenu_Item] )
		{
			ITEMMGR->SetItemIfoPositionHead();
			ITEM_INFO* pInfo = NULL;
			char cbItemString[MAX_ITEMNAME_LENGTH+16];
			while( (pInfo=ITEMMGR->GetItemInfoData())!=NULL )
			{
				wsprintf( cbItemString, "%s(%d)", pInfo->ItemName, pInfo->ItemIdx );

				if( pInfo->Category == eItemCategory_Equip )
				{
					switch( pInfo->EquipType )
					{
					case eEquipType_Weapon:
						{
							SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_WEAPON, CB_ADDSTRING, 0, (LPARAM)cbItemString );
						}
						break;
					case eEquipType_Armor:			
						{
							//if(pInfo->EquipSlot)
							//	;
							SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_CLOTHES, CB_ADDSTRING, 0, (LPARAM)cbItemString );
						}
						break;
					case eEquipType_Accessary:
					case eEquipType_Pet:
						{
							SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_ACCESSORY, CB_ADDSTRING, 0, (LPARAM)cbItemString );
						}
						break;
					}
				}
				else if( pInfo->Category == eItemCategory_Expend )
				{
					SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_POTION, CB_ADDSTRING, 0, (LPARAM)cbItemString );
				}
				else
				{
					SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_ETC, CB_ADDSTRING, 0, (LPARAM)cbItemString );
				}
			}
			SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_WEAPON, CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_CLOTHES, CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_ACCESSORY, CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_SKILLBOOK, CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_POTION, CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_ETC, CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_SKILL, CB_SETCURSEL, 0, 0 );

			//Grade
			for( i = 0 ; i <= 9 ; ++i )
			{
				wsprintf( bufNum, "+%d", i );
				SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_WEAPONGRADE, CB_ADDSTRING, 0, (LPARAM)bufNum );
				SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_CLOTHESGRADE, CB_ADDSTRING, 0, (LPARAM)bufNum );
			}
			for( i = 0 ; i <= 12 ; ++i )
			{
				wsprintf( bufNum, "%d", i );
				SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_SKILLSUNG, CB_ADDSTRING, 0, (LPARAM)bufNum );
			}
			SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_WEAPONGRADE, CB_SETCURSEL, 0, 0 );
			SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_CLOTHESGRADE, CB_SETCURSEL, 0, 0 );
			SetDlgItemInt( m_hWndSub[eTSMenu_Item], IDC_TSITEM_EDT_POTION, 1, TRUE );
			SetDlgItemInt( m_hWndSub[eTSMenu_Item], IDC_TSITEM_EDT_ETC, 1, TRUE );
			SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_SPN_POTION, UDM_SETRANGE, 0, (LPARAM)MAKELONG(50, 1) );
			SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_SPN_ETC, UDM_SETRANGE, 0, (LPARAM)MAKELONG(50, 1) );

			SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_SKILLSUNG, CB_SETCURSEL, 0, 0 );
		}

		//		Skill
		{
			CYHHashTable< cSkillInfo >& skillTable = SKILLMGR->GetSkillInfoHashTable();

			cSkillInfo* pSkill = NULL;
			skillTable.SetPositionHead();
			char cbSkillString[MAX_SKILL_NAME_LENGTH+10];
			while ((pSkill=skillTable.GetData())!=NULL)
			{
				if(pSkill)
				{
					wsprintf( cbSkillString, "%s(%d)", pSkill->GetName(), pSkill->GetIndex() );

					const char* skillname = pSkill->GetName();
					SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_SKILL, CB_ADDSTRING, 0, (LPARAM)cbSkillString );
				}
			}

			for( i = 0 ; i <= 9 ; ++i )
			{
				wsprintf( bufNum, "%d", i );
				SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_TOSLOT, CB_ADDSTRING, 0, (LPARAM)bufNum );
			}
			
			SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_TOSLOT,	CB_SETCURSEL, 0, 0 );
		}

		//		if( m_hWndSub[eTSMenu_Mob] )
		{
			//몬스터 등록
			BASE_MONSTER_LIST* pList = NULL;
			char cbMonsterString[MAX_MONSTER_NAME_LENGTH+10];
			for( i=1 ; (pList=GAMERESRCMNGR->GetMonsterListInfo(i))!=NULL ; ++i )
			{
				wsprintf( cbMonsterString, "%s(%d)", pList->Name, pList->Level );
				SendDlgItemMessage( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_CMB_MOBNAME, CB_ADDSTRING, 0, (LPARAM)cbMonsterString );
			}
			m_lBossStartIndex = i - 1;

			LPCTSTR pStrMap = 0;
			for( i = 1; i < MAX_MAP_NUM; ++i )
			{
				if( *( pStrMap = GetMapName(static_cast<MAPTYPE>(i)) ) )
				{
					if (_tcscmp(pStrMap, _T("?")) == 0)
					{
						continue;
					}
					SendDlgItemMessage( m_hWndSub[eTSMenu_Mob],		IDC_TSREGEN_CMB_MAP,	CB_ADDSTRING, 0, (LPARAM)pStrMap );
					SendDlgItemMessage( m_hWndSub[eTSMenu_Move],	IDC_TSMOVE_CMB_MAP,		CB_ADDSTRING, 0, (LPARAM)pStrMap );
				}
			}

			SendDlgItemMessage( m_hWndSub[eTSMenu_Mob],		IDC_TSREGEN_CMB_MAP,	CB_SETCURSEL, 0, 0 );
		}
	}

	//		QUEST
	{
		LVCOLUMN COL;

		ListView_SetExtendedListViewStyle(GetDlgItem(m_hWndSub[eTSMenu_Quest], IDC_TSQUEST_LIST), LVS_EX_FULLROWSELECT);

		COL.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		COL.fmt = LVCFMT_LEFT;

		COL.cx = 80;
		COL.pszText = "QuestIdx";
		COL.iSubItem = 0;
		SendDlgItemMessage( m_hWndSub[eTSMenu_Quest],		IDC_TSQUEST_LIST,  LVM_INSERTCOLUMN, 0,  (LPARAM)&COL );

		COL.cx = 80;
		COL.pszText = "MapIdx";
		COL.iSubItem = 0;
		SendDlgItemMessage( m_hWndSub[eTSMenu_Quest],		IDC_TSQUEST_LIST,  LVM_INSERTCOLUMN, 1,  (LPARAM)&COL );

		COL.cx = 80;
		COL.pszText = "NpcIdx";
		COL.iSubItem = 0;
		SendDlgItemMessage( m_hWndSub[eTSMenu_Quest],		IDC_TSQUEST_LIST,  LVM_INSERTCOLUMN, 2,  (LPARAM)&COL );

		CYHHashTable<CQuest>* QuestTable = QUESTMGR->GetQuestTable();

		QuestTable->SetPositionHead();
		char cbQuestString[MAX_SKILL_NAME_LENGTH+10];
		CQuest* pQuest = QuestTable->GetData();
		int row = 0;
		while (pQuest)
		{
			CQuestInfo* pQuestInfo = QUESTMGR->GetQuestInfo(pQuest->GetQuestIdx());
			if (pQuestInfo)
			{
				LVITEM LI;

				LI.mask = LVIF_TEXT;
				LI.state = 0;
				LI.stateMask = 0;
				LI.iItem = row++;

				_stprintf( cbQuestString, "%d", pQuestInfo->GetQuestIdx());
				LI.iSubItem = 0;
				LI.pszText = cbQuestString;
				SendDlgItemMessage( m_hWndSub[eTSMenu_Quest], IDC_TSQUEST_LIST,  LVM_INSERTITEM, 0, (LPARAM)&LI );

				_stprintf( cbQuestString, "%d", pQuestInfo->GetMapNum());
				LI.iSubItem = 1;
				LI.pszText = cbQuestString;
				SendDlgItemMessage( m_hWndSub[eTSMenu_Quest], IDC_TSQUEST_LIST,  LVM_SETITEM, 0, (LPARAM)&LI );

				_stprintf( cbQuestString, "%d", pQuestInfo->GetNpcIdx());
				LI.iSubItem = 2;
				LI.pszText = cbQuestString;
				SendDlgItemMessage( m_hWndSub[eTSMenu_Quest], IDC_TSQUEST_LIST,  LVM_SETITEM, 0, (LPARAM)&LI );
			}
			pQuest = QuestTable->GetData();
		}
	}
	bRegist = TRUE;
}

BOOL CTSToolManager::IsTSDialogMessage( LPMSG pMessage)
{
	if( IsWindow( m_hWndDlg ) && IsDialogMessage( m_hWndDlg, pMessage ) )
		return TRUE;

	if( m_nSelectMenu < eTSMenu_Count )
		if( m_hWndSub[m_nSelectMenu] )
		{
			if( IsWindow( m_hWndSub[m_nSelectMenu] ) && IsDialogMessage( m_hWndSub[m_nSelectMenu], pMessage ) )
				return TRUE;
		}

		return FALSE;
}

void CTSToolManager::SetPositionByMainWindow()
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
void CTSToolManager::SetPositionByOutputWindow()
{
	RECT rcMain, rcDlg;
	GetWindowRect( _g_hWnd, &rcMain );
	GetWindowRect( m_hWndOutDlg, &rcDlg );
	int nMainHeight = rcMain.bottom - rcMain.top - GetSystemMetrics(SM_CYSIZEFRAME) - GetSystemMetrics(SM_CYCAPTION);
	int nDlgWidth = rcDlg.right - rcDlg.left;
	int X = rcMain.right;
	int Y = rcMain.top+GetSystemMetrics(SM_CYSIZEFRAME)+GetSystemMetrics(SM_CYCAPTION);

	SetWindowPos( m_hWndOutDlg, NULL, X, Y, nDlgWidth, nMainHeight, SWP_NOZORDER | SWP_NOACTIVATE );

	GetClientRect(m_hWndOutDlg, &rcDlg);
	int nWidth = rcDlg.right - rcDlg.left;
	int nHeight = rcDlg.bottom - rcDlg.top;
	MoveWindow(GetDlgItem( m_hWndOutDlg, IDC_TSOUTPUT_EDIT ), 0, 0, nWidth, nHeight, TRUE);
}
void CTSToolManager::SetPositionSubDlg()
{
	RECT rcDlg;
	GetWindowRect( m_hWndDlg, &rcDlg );

	if( m_nSelectMenu < eTSMenu_Count )
	{
		if( m_hWndSub[m_nSelectMenu] )
		{
			int subX = rcDlg.right;
			int subY = rcDlg.top + 36 + m_nSelectMenu * 28;

			SetWindowPos( m_hWndSub[m_nSelectMenu], NULL, subX, subY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
		}
	}
}

void CTSToolManager::OnClickMenu( int nMenu )
{
	switch( nMenu )
	{
	case eTSMenu_Weather:
		{
			// 등록된 날씨 정보를 메뉴에 등록시킨다.
			SendDlgItemMessage( m_hWndSub[eTSMenu_Weather], IDC_TSWEATHER_CMB_TYPE, CB_RESETCONTENT, 0, 0 );
			WEATHERMGR->AddTSToolMenu();
		}
		break;
	}

	if( nMenu >= eTSMenu_Count )
	{
		if( m_nSelectMenu < eTSMenu_Count )
		{
			if( m_hWndSub[m_nSelectMenu] )
				ShowWindow( m_hWndSub[m_nSelectMenu], SW_HIDE );

			CheckDlgButton( m_hWndDlg, IDC_TS_MNBTN_CHARACTOR + m_nSelectMenu, BST_UNCHECKED );			
		}

		m_nSelectMenu = eTSMenu_Count;
		CheckDlgButton( m_hWndDlg, IDC_TS_MNBTN_NONE, BST_CHECKED );

		m_bSubShow = FALSE;
		SetFocus( _g_hWnd );
	}
	else
	{
		if( m_nSelectMenu == nMenu )
		{
			if( m_hWndSub[m_nSelectMenu] )
				ShowWindow( m_hWndSub[m_nSelectMenu], SW_HIDE );
			CheckDlgButton( m_hWndDlg, IDC_TS_MNBTN_CHARACTOR + m_nSelectMenu, BST_UNCHECKED );

			m_nSelectMenu = eTSMenu_Count;
			CheckDlgButton( m_hWndDlg, IDC_TS_MNBTN_NONE, BST_CHECKED );

			m_bSubShow = FALSE;
			SetFocus( _g_hWnd );
		}
		else
		{
			ShowWindow( m_hWndSub[m_nSelectMenu], SW_HIDE );
			CheckDlgButton( m_hWndDlg, IDC_TS_MNBTN_CHARACTOR + m_nSelectMenu, BST_UNCHECKED );

			m_nSelectMenu = nMenu;
			SetPositionSubDlg();
			CheckDlgButton( m_hWndDlg, IDC_TS_MNBTN_CHARACTOR + m_nSelectMenu, BST_CHECKED );

			if( m_hWndSub[m_nSelectMenu] )
				AnimateWindow( m_hWndSub[m_nSelectMenu], 200, AW_SLIDE | AW_HOR_POSITIVE );

			m_bSubShow = TRUE;
		}
	}
}

BOOL CTSToolManager::OnCharCommand()
{
	if( !m_hWndSub[eTSMenu_Char] ) return FALSE;
	SetFocus( _g_hWnd );

	//char buf[64];
	int race, gender, job, hair, face;

	race = GetDlgItemInt(m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_RACE, &race, MAX_ITEMNAME_LENGTH+1 );
	gender = GetDlgItemInt(m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_GENDER, &gender, MAX_ITEMNAME_LENGTH+1 );
	job = GetDlgItemInt(m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_JOB, &job, MAX_ITEMNAME_LENGTH+1 );
	hair = GetDlgItemInt(m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_HAIR, &hair, MAX_ITEMNAME_LENGTH+1 );
	face = GetDlgItemInt(m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_FACE, &face, MAX_ITEMNAME_LENGTH+1 );

	if (race >= RaceType_Max)
		race = 0;
	HERO->GetCharacterTotalInfo()->Race = static_cast<BYTE>(race);

	if (gender > 1)
		gender = 0;
	HERO->GetCharacterTotalInfo()->Gender = static_cast<BYTE>(gender);
	

	//GetDlgItemText( m_hWndSub[eTSMenu_Char], IDC_TSCHAR_CMB_GENDER, buf, MAX_ITEMNAME_LENGTH+1 );

	// Job 필요할까 ?
	//if (job >= JobType_MAX)
	//HERO->GetCharacterTotalInfo()->Job;

	//CHARMAKEMGR->GetOptionList();

	HERO->GetCharacterTotalInfo()->HairType = static_cast<BYTE>(hair);
	HERO->GetCharacterTotalInfo()->FaceType = static_cast<BYTE>(face);
	APPEARANCEMGR->AddCharacterPartChange(gHeroID);

	return TRUE;
}

void CTSToolManager::ItemDlgCtrlDisableAll()
{
	HWND hWnd = m_hWndSub[eTSMenu_Item];

	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_WEAPON ),		FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_WEAPONGRADE ),	FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_CLOTHES ),		FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_CLOTHESGRADE ),	FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_ACCESSORY ),		FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_SKILLBOOK ),		FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_POTION ),		FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDT_POTION ),		FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_SPN_POTION ),		FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_ETC ),			FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDT_ETC ),			FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_SPN_ETC ),			FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDT_MONEY ),			FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_SKILL ),			FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_TOSLOT ),		FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDT_ABILITY ),		FALSE );

	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_BTN_SEARCH ),		FALSE );
	EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDIT_SEARCH),		FALSE );
}

void CTSToolManager::AddWeatherType( LPCTSTR weatherName )
{
	SendDlgItemMessage( m_hWndSub[eTSMenu_Weather], IDC_TSWEATHER_CMB_TYPE, CB_ADDSTRING, 0, (LPARAM)weatherName );
}

BOOL CTSToolManager::OnItemCommand()
{
	if( !m_hWndSub[eTSMenu_Item] ) return FALSE;
	SetFocus( _g_hWnd );

	int nResult = 0;
	WORD wCount = 1;
	char buf[64];
	DWORD idx = (DWORD)-1;
	if( SendMessage( GetDlgItem( m_hWndSub[eTSMenu_Item], IDC_TSITEM_BTN_WEAPON ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		GetDlgItemText( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_WEAPON, buf, MAX_ITEMNAME_LENGTH+1 );
		sscanf(strchr(buf,'('), "(%lu)", &idx);

		nResult = 1;	//1:Item
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eTSMenu_Item], IDC_TSITEM_BTN_CLOTHES ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		GetDlgItemText( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_CLOTHES, buf, MAX_ITEMNAME_LENGTH+1 );
		sscanf(strchr(buf,'('), "(%lu)", &idx);

		nResult = 1;	//1:Item
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eTSMenu_Item], IDC_TSITEM_BTN_ACCESSORY ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		GetDlgItemText( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_ACCESSORY, buf, MAX_ITEMNAME_LENGTH+1 );
		sscanf(strchr(buf,'('), "(%lu)", &idx);

		nResult = 1;	//1:Item
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eTSMenu_Item], IDC_TSITEM_BTN_SKILLBOOK ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		GetDlgItemText( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_SKILLBOOK, buf, MAX_ITEMNAME_LENGTH+1 );
		sscanf(strchr(buf,'('), "(%lu)", &idx);

		nResult = 1;	//1:Item
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eTSMenu_Item], IDC_TSITEM_BTN_POTION ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		GetDlgItemText( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_POTION, buf, MAX_ITEMNAME_LENGTH+1 );
		sscanf(strchr(buf,'('), "(%lu)", &idx);

		BOOL rt;
		int nCount = GetDlgItemInt( m_hWndSub[eTSMenu_Item], IDC_TSITEM_EDT_POTION, &rt, TRUE );
		if( rt )
		{
			if( nCount < 0 )			nCount = 0;
			else if( nCount > 99 )		nCount = 99;

			wCount = (WORD)nCount;
			nResult = 1;	//1:Item;
		}
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eTSMenu_Item], IDC_TSITEM_BTN_ETC ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		GetDlgItemText( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_ETC, buf, MAX_ITEMNAME_LENGTH+1 );
		sscanf(strchr(buf,'('), "(%lu)", &idx);

		BOOL rt;
		int nCount = GetDlgItemInt( m_hWndSub[eTSMenu_Item], IDC_TSITEM_EDT_ETC, &rt, TRUE );
		if( rt )
		{
			if( nCount < 0 )			nCount = 0;
			else if( nCount > 50 )		nCount = 50;

			wCount = (WORD)nCount;
			nResult = 1;	//1:Item;
		}
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eTSMenu_Item], IDC_TSITEM_BTN_MONEY ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		BOOL rt;
		DWORD dwMoney = GetDlgItemInt( m_hWndSub[eTSMenu_Item], IDC_TSITEM_EDT_MONEY, &rt, TRUE );
		if( !rt )	return FALSE;

		SetDlgItemInt( m_hWndSub[eTSMenu_Item], IDC_TSITEM_EDT_MONEY, 0, TRUE );

		MSG_DWORD msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_MONEY_SYN;
		msg.dwObjectID	= HEROID;
		msg.dwData		= dwMoney;
		NETWORK->Send( &msg, sizeof(msg) );
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eTSMenu_Item], IDC_TSITEM_BTN_SKILL ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		GetDlgItemText( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_SKILL, buf, MAX_ITEMNAME_LENGTH+1 );

		sscanf(strchr(buf,'('), "(%lu)", &idx);

		int nSel = SendDlgItemMessage( m_hWndSub[eTSMenu_Item], IDC_TSITEM_CMB_TOSLOT, CB_GETCURSEL, 0, 0 );

		if( nSel < 0 )			nSel = 0;
		else if( nSel > 10 )	nSel = 9;

		cSkillInfo* pInfo = SKILLMGR->GetSkillInfo( idx );
		
		if( pInfo != NULL )
		{
			MSG_QUICK_ADD_SYN msg;
			msg.tabNum = 0;
			msg.pos = static_cast<BYTE>(nSel);
			msg.slot.data = pInfo->GetLevel();
			msg.slot.dbIdx;// = nSel;
			msg.slot.idx = pInfo->GetIndex();
			msg.slot.kind = QuickKind_Skill;

			QUICKMGR->Quick_Add_Ack(&msg);
		}
	}


	if( nResult == 1 )
	{
		ITEM_INFO* pInfo = ITEMMGR->GetItemInfo(idx);
		//ITEM_INFO* pInfo = ITEMMGR->FindItemInfoForName( buf );
		if( pInfo == NULL ) return FALSE;

		if ((pInfo->EquipSlot!=(WORD)-1) && (pInfo->EquipSlot < SLOT_WEAR_NUM))
		{
			HERO->SetWearedItemIdx(pInfo->EquipSlot, pInfo->ItemIdx);
			APPEARANCEMGR->AddCharacterPartChange(gHeroID);
		}
	}

	return TRUE;
}

BOOL CTSToolManager::OnRegenCommand()
{
	if( !m_hWndSub[eTSMenu_Mob] ) return FALSE;
	SetFocus( _g_hWnd );

	char buf[MAX_ITEMNAME_LENGTH+1] = {0,};

	if( SendMessage( GetDlgItem( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_BTN_MODEDIRECT ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		//mob
		int nSel = SendDlgItemMessage( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_CMB_MOBNAME, CB_GETCURSEL, 0, 0 );
		//		BYTE bBoss = 0;
		BASE_MONSTER_LIST* pList = NULL;
		if( nSel < m_lBossStartIndex )
		{
			pList = GAMERESRCMNGR->GetMonsterListInfo( nSel + 1 );
		}
		else
		{
			pList = GAMERESRCMNGR->GetMonsterListInfo( 1000 + nSel - m_lBossStartIndex );
			//			bBoss = 1;
		}

		if( pList == NULL ) return FALSE;
		//mob count
		BOOL rt;
		int nMobCount = GetDlgItemInt( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_EDT_MOBNUM, &rt, TRUE );
		if( !rt ) return FALSE;
		if( nMobCount <= 0 ) return FALSE;
		if( nMobCount > 100 ) return FALSE;	//임시로 한번에 100개 까지만
		//map
		int nLen = GetDlgItemText( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_CMB_MAP, buf, MAX_MAP_NAME_LENGTH+1 );
		if( nLen == 0 ) return FALSE;
		buf[MAX_MAP_NAME_LENGTH] = 0;
		WORD wMapNum = GetMapNumForName( buf );
		if( wMapNum == 0 ) return FALSE;
		//channel
		//int nChannel = GetDlgItemInt( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_EDT_CHANNEL, &rt, TRUE );
		//int nChannel = SendDlgItemMessage( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_CMB_CHANNEL, CB_GETCURSEL, 0, 0 );
		////if( !rt ) return FALSE;
		//if( nChannel < 0 ) return FALSE;
		////if( nChannel > m_cbChannelCount ) return FALSE;		
		//if(nChannel > g_MapChannelNum[wMapNum]) return FALSE;		
		//pos & radius
		int nX = GetDlgItemInt( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_EDT_X, &rt, TRUE ) * 100;
		if( !rt ) return FALSE;
		int nY = GetDlgItemInt( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_EDT_Y, &rt, TRUE ) * 100;
		if( !rt ) return FALSE;
		int nRad = GetDlgItemInt( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_EDT_RAD, &rt, TRUE ) * 100;
		if( !rt ) return FALSE;

		if( nX < 0 ) nX = 0;	//최대값 체크추가
		if( nY < 0 ) nY = 0;	
		if( nRad < 0 ) nRad = 0;
		else if( nRad > 10000 ) nRad = 10000;	//반경 100m로 제한

		VECTOR3 vPos = { (float)nX, 0.0f, (float)nY };
		////Item
		//nLen = GetDlgItemText( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_CMB_ITEM, buf, MAX_ITEMNAME_LENGTH+1 );
		//if( nLen == 0 ) return FALSE;
		//buf[MAX_ITEMNAME_LENGTH] = 0;
		//ITEM_INFO* pInfo = ITEMMGR->FindItemInfoForName( buf );
		//if( pInfo == NULL ) return FALSE;
		////Drop Ratio
		//int nDropRatio = GetDlgItemInt( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_EDT_ITEM, &rt, TRUE );
		//if( !rt ) return FALSE;
		//if( nDropRatio < 0 )	return FALSE;
		//if( nDropRatio > 100 )	return FALSE;


		int i;
		for( i=0; i<nMobCount; ++i)
		{
			SEND_MONSTER_TOTALINFO info;
			memset(&info,0,sizeof(info));
			info.BaseObjectInfo.dwObjectID = 100+i;
			info.BaseObjectInfo.BattleID = 0;
			sprintf(info.BaseObjectInfo.ObjectName,"Monster%02d",i);

			if( nRad != 0 )
			{
				vPos.x += (rand()%(nRad*2)) - nRad;
				vPos.z += (rand()%(nRad*2)) - nRad;
			}

			info.MoveInfo.CurPos.Compress(&vPos);
			BASEMOVE_INFO bminfo;
			bminfo.SetFrom(&info.MoveInfo);
			info.TotalInfo.MonsterKind = pList->MonsterKind;
			OBJECTMGR->AddMonster(&info.BaseObjectInfo,&bminfo,&info.TotalInfo);
		}
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_BTN_MODEFILE ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		char lpstrFile[MAX_PATH] = {0,};
		GetDlgItemText( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_EDT_FILE, lpstrFile, MAX_PATH );

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
					msg.dwObjectID	= HEROID;
					msg.MonsterKind = (WORD)GAMERESRCMNGR->GetMonsterIndexForName( fp.GetString() );
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
	}
	else if( SendMessage( GetDlgItem( m_hWndSub[eTSMenu_Mob], IDC_TSREGEN_BTN_DELETE ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
	{
		MSGBASE msg;
		msg.Category	= MP_CHEAT;
		msg.Protocol	= MP_CHEAT_EVENT_MONSTER_DELETE;
		msg.dwObjectID	= HEROID;

		NETWORK->Send( &msg, sizeof(msg) );
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CTSToolManager::OnMoveCommand( int nMethod )
{
	if( !HERO ) return FALSE;
	if( !m_hWndSub[eTSMenu_Move] ) return FALSE;
	SetFocus( _g_hWnd );

	if( nMethod == 0 )
	{
		BOOL	bRt;
		VECTOR3 pos;
		pos.x = GetDlgItemInt( m_hWndSub[eTSMenu_Move], IDC_TSMOVE_EDT_X, &bRt, TRUE ) * 100.0f;
		if( !bRt ) return FALSE;
		pos.y = 0;
		pos.z = GetDlgItemInt( m_hWndSub[eTSMenu_Move], IDC_TSMOVE_EDT_Y, &bRt, TRUE ) * 100.0f;
		if( !bRt ) return FALSE;

		BASEMOVE_INFO info;
		info.bMoving = FALSE;
		info.KyungGongIdx = 0;
		info.MoveMode = eMoveMode_Run;
		info.CurPosition = pos;
		MOVEMGR->InitMove(HERO,&info);
	}
	else if( nMethod == 1)
	{
		char buf[MAX_MAP_NAME_LENGTH+1] = {0,};

		int nLen = GetDlgItemText( m_hWndSub[eTSMenu_Move], IDC_TSMOVE_CMB_MAP, buf, MAX_MAP_NAME_LENGTH+1 );
		if( nLen > MAX_MAP_NAME_LENGTH ) return FALSE;

		WORD wMapNum = GetMapNumForName( buf );
		if( wMapNum == 0 ) return FALSE;

		//BOOL bRt;
		//int nChannel = GetDlgItemInt( m_hWndSub[eMenu_Move], IDC_TSMOVE_EDT_CHANNEL, &bRt, TRUE );
		//int nChannel = GetDlgItemInt( m_hWndSub[eMenu_Move], IDC_TSMOVE_CMB_CHANNEL, &bRt, TRUE );
		
		GAMERESRCMNGR->m_TestClientInfo.Map = wMapNum;
		MAPCHANGE->SetGameInInitKind(eGameInInitKind_MapChange);
		MAINGAME->SetGameState(eGAMESTATE_MAPCHANGE,&wMapNum,4);
	}

	return TRUE;
}

BOOL CTSToolManager::OnQuestCommand()
{
	if( !HERO ) return FALSE;
	if( !m_hWndSub[eTSMenu_Quest] ) return FALSE;
	SetFocus( _g_hWnd );

	char temp[256];
	HWND hLV = GetDlgItem(m_hWndSub[eTSMenu_Quest], IDC_TSQUEST_LIST);

	int nFind=-1;
	DWORD QuestIdx = (DWORD)-1;
	DWORD subQuestNum;

	nFind = ListView_GetNextItem(hLV, -1, LVNI_SELECTED);

	if (nFind == -1)
		return FALSE;

	m_outString = "  *** Start Quest Test *** \r\n\r\n";

	ListView_GetItemText(hLV, nFind, 0, temp, sizeof(temp));
	m_outString += "   Quest Idx = ";
	m_outString += temp;
	m_outString += "\r\n\r\n";

	QuestIdx = atoi(temp);

	CQuestInfo* pQuestInfo = QUESTMGR->GetQuestInfo(QuestIdx);
	CQuest* pQuest = QUESTMGR->GetQuest(QuestIdx);
	if (!pQuest)
	{
		m_outString += "!!! Error! GetQuest() : Can't Find Quest \r\n";
	}
	else if (!pQuestInfo)
	{
		m_outString += "!!! Error! GetQuestInfo() : Can't Find QuestInfo \r\n";
	}
	else
	{
		// check mapIdx
		int startMapIdx = pQuestInfo->GetMapNum();
		LPCTSTR pStrStartMap = 0;
		if( *( pStrStartMap = GetMapName(static_cast<MAPTYPE>(startMapIdx)) ) )
		{
			if (_tcscmp(pStrStartMap, _T("?")))
			{
				_stprintf(temp, "   Start map %d ( %s )\r\n", startMapIdx, pStrStartMap);
				m_outString += temp;
			}
		}
		else
		{
			_stprintf(temp, "!!! Error! Start map ( %d ) Invalid.\r\n", startMapIdx);
			m_outString += temp;
		}
		// Check NpcIdx
		int npcIdx = pQuestInfo->GetNpcIdx();
		
		stNPCIDNAME* pNpc = QUESTMGR->GetNpcIdName(npcIdx);

		if (pNpc)
		{
			_stprintf(temp, "   NpcIdx %d ( %s )\r\n", npcIdx, pNpc->name);
			m_outString += temp;
		}
		else
		{
			QUESTNPCINFO* pQuestNpcInfo = QUESTMGR->GetQusetNpcInfo(npcIdx);
			if (pQuestNpcInfo)
			{
				_stprintf(temp, "   QuestNpcIdx %d ( %s )\r\n", npcIdx, pQuestNpcInfo->sName);
			}
			else
			{
				CNpcRecallBase* pNpcRecallBase = NPCRECALLMGR->Get_RecallBaseByNpcId(npcIdx);
				if (pNpcRecallBase)
				{
					_stprintf(temp, "   RecallNpcIdx %d ( %s )\r\n", npcIdx, pNpcRecallBase->Get_NpcName());
				}
				else
				{
					_stprintf(temp, "!!! Error! NpcIdx ( %d ) Invalid.\r\n", npcIdx);
				}
			}
			m_outString += temp;
		}

		m_outString += "\r\n";

		subQuestNum = pQuestInfo->GetSubQuestCount();

		DWORD iSub;
		
		cPtrList* pLimitKindList = NULL;

		for (iSub=0; iSub<subQuestNum; ++iSub )
		{
			CSubQuestInfo* subQuestInfo = pQuestInfo->GetSubQuestInfo(iSub);
			SUBQUEST* pSub = pQuest->GetSubQuest(iSub);
			if (!subQuestInfo)
			{
				_stprintf(temp, "!!! Error! Can't Find SubQuestInfo( %d )\r\n", iSub);
				m_outString += temp;
				continue;
			}

			_stprintf(temp, "\r\n === Check SubQuestInfo( %d ) === \r\n\r\n", iSub);
			m_outString += temp;

			// #LIMIT Check;
			cPtrList& pLimitList = subQuestInfo->GetLimitList();
//			if (pLimitList)
			{
				PTRLISTPOS pos = pLimitList.GetHeadPosition();									

				CQuestLimit* pLimit = NULL;

				while( pos )
				{
					pLimit = (CQuestLimit*)pLimitList.GetNext( pos );

					if( pLimit )
					{
						pLimitKindList = pLimit->GetLimitKindList();
						if (!pLimitKindList)
						{
							continue;
						}
						
						PTRLISTPOS kindPos = pLimitKindList->GetHeadPosition();

						CQuestLimitKind* pQuestLimitKind = NULL;

						while( kindPos )
						{
							pQuestLimitKind = (CQuestLimitKind*)pLimitKindList->GetNext( kindPos );

							if( pQuestLimitKind )
							{
								eQuestLimitKind eKind = (eQuestLimitKind)pQuestLimitKind->GetLimitKind();
								switch (eKind)
								{
									case eQuestLimitKind_Level:
									{
										CQuestLimitKind_Level* pLimitKind = (CQuestLimitKind_Level*)pQuestLimitKind;
										_stprintf(temp, "   #Limit Level ( %d, %d )\r\n", pLimitKind->GetMin(), pLimitKind->GetMax());

										break;
									}
								case eQuestLimitKind_Money:
									{
										CQuestLimitKind_Level* pLimitKind = (CQuestLimitKind_Level*)pQuestLimitKind;
										_stprintf(temp, "   #Limit Money ( %d, %d )\r\n", pLimitKind->GetMin(), pLimitKind->GetMax());

										break;
									}

								case eQuestLimitKind_Quest:
									{
										CQuestLimitKind_Quest* pLimitKind = (CQuestLimitKind_Quest*)pQuestLimitKind;
										DWORD idx = pLimitKind->GetIdx();

										// idx 유효 검사.
										if (!QUESTMGR->GetQuest(idx))
										{
											_stprintf(temp, "!!! Error! Limit Quest ( %d ) is invalid index.\r\n", idx);
										}
										else
										{
											_stprintf(temp, "   #Limit Quest ( %d )\r\n", idx);
										}
										break;
									}
								case eQuestLimitKind_SubQuest:
									{
										CQuestLimitKind_Quest* pLimitKind = (CQuestLimitKind_Quest*)pQuestLimitKind;
										DWORD idx = pLimitKind->GetIdx();
	
										// idx 유효 검사.
										if (!pQuest->GetSubQuest(idx))
										{
											_stprintf(temp, "!!! Error! Limit SubQuest ( %d ) is invalid index.\r\n", idx);
										}
										else
										{
											_stprintf(temp, "   #Limit SubQuest ( %d )\r\n", idx);
										}
										break;
									}
								case eQuestLimitKind_Stage:
									{
										CQuestLimitKind_Stage* pLimitKind = (CQuestLimitKind_Stage*)pQuestLimitKind;

										BYTE count = pLimitKind->GetClassIndexCount();
										DWORD* pStage = pLimitKind->GetClassIndexs();
										_stprintf(temp, "   Limit Stage Count = %d \r\n", count);
										m_outString += temp;
										BYTE iStage;
										for (iStage=0; iStage<count; ++iStage)
										{
											// Stage 별로 유효한지 Test.
											if( pStage[iStage] <= 1000 )
											{
												_stprintf(temp, "!!! Error! Limit Stage Count(%d) is Invalid ( %d ) \r\n", iStage, pStage[iStage]);
												m_outString += temp;

												continue;
											}

											int msg = RESRCMGR->GetClassNameNum(pStage[iStage]);
											
											if (msg)
											{
												DWORD dwLimitClass	  = pStage[iStage] / 1000 ;
												DWORD dwLimitRacial	  = (pStage[iStage]  - (1000*dwLimitClass)) / 100 ;

												_stprintf(temp, "   #Limit Stage is %d ( race %d, %s )\r\n", pStage[iStage], dwLimitRacial, RESRCMGR->GetMsg(msg));
												m_outString += temp;
											}
											else
											{
												_stprintf(temp, "!!! Error! Limit Stage Count(%d) is Invalid ( %d ) \r\n", iStage, pStage[iStage]);
												m_outString += temp;
											}
										}

										temp[0] = 0;

										break;
									}
								case eQuestLimitKind_Attr:
									break;
								case eQuestLimitKind_RunningQuest:
									{
										CQuestLimitKind_Quest* pLimitKind = (CQuestLimitKind_Quest*)pQuestLimitKind;
										DWORD idx = pLimitKind->GetIdx();

										// idx 유효 검사.
										if (!QUESTMGR->GetQuest(idx))
										{
											_stprintf(temp, "!!! Error! Limit Running Quest ( %d ) is invalid index. \r\n", idx);
										}
										else
										{
											_stprintf(temp, "   #Limit Running Quest ( %d )\r\n", idx);
										}
										break;
									}
									break;
								}

								m_outString += temp;
							}
						}
					}
				}
			}
//			else
//			{
//				m_outString += "    No Limit \r\n";
//			}
			m_outString += "\r\n";

			// #TRIGGER Check;
			cPtrList& pTriggerList = subQuestInfo->GetTriggerList();
//			if (pTriggerList)
			{
				PTRLISTPOS pos = pTriggerList.GetHeadPosition();									

				CQuestTrigger* pTrigger = NULL;

				CQuestCondition* pCondition = NULL;
				CQuestEvent* pEvent = NULL;
				while( pos )
				{
					pTrigger = (CQuestTrigger*)pTriggerList.GetNext( pos );

					if( pTrigger )
					{
						// Condition Check;
						pCondition = pTrigger->GetQuestCondition();
						if (pCondition)
						{
							pEvent = pCondition->GetQuestEvent();
							if (pEvent)
							{
								DWORD param1 = pEvent->GetDwParam1();
								LONG param2 = pEvent->GetLParam2();

								switch (pEvent->GetQuestEventKind())
								{
									case eQuestEvent_NpcTalk:
									{
										// param1 == npcIdx , param2 == QuestIdx;
										pNpc = QUESTMGR->GetNpcIdName(param1);

										if (pNpc)
										{
											_stprintf(temp, "   @Event_NpcTalk NpcIdx %d ( %s )\r\n", param1, pNpc->name);
											m_outString += temp;
										}
										else
										{
											QUESTNPCINFO* pQuestNpcInfo = QUESTMGR->GetQusetNpcInfo(param1);
											if (pQuestNpcInfo)
											{
												_stprintf(temp, "   @Event_NpcTalk QuestNpcIdx %d ( %s )\r\n", param1, pQuestNpcInfo->sName);
											}
											else
											{
												CNpcRecallBase* pNpcRecallBase = NPCRECALLMGR->Get_RecallBaseByNpcId(param1);
												if (pNpcRecallBase)
												{
													_stprintf(temp, "   @Event_NpcTalk QuestNpcIdx %d ( %s )\r\n", param1, pNpcRecallBase->Get_NpcName());
												}
												else
												{
													_stprintf(temp, "!!! Error! @Event_NpcTalk NpcIdx ( %d ) Invalid.\r\n", param1);
												}
											}
											m_outString += temp;
										}

										if (!QUESTMGR->GetQuest(param2))
										{
											_stprintf(temp, "!!! Error! @Event_NpcTalk Quest ( %d ) is invalid index. \r\n", param2);
										}
										else
										{
											_stprintf(temp, "   @Event_NpcTalk Quest ( %d )\r\n", param2);
										}

										break;
									}
									case eQuestEvent_Hunt:
									{
										// check Param1 == monsterKind;
										BASE_MONSTER_LIST* pList = NULL;
										pList = GAMERESRCMNGR->GetMonsterListInfo(param1);
										if (pList)
										{
											_stprintf(temp, "   @Event_Hunt monsterIdx is  %d (%s) \r\n", param1, pList->Name);
										}
										else
										{
											_stprintf(temp, "!!! Error! @Event_Hunt monsterIdx ( %d ) is Invalid \r\n", param1);
										}

										m_outString += temp;

										break;
									}
									case eQuestEvent_Count:
									{
										// idx 유효 검사.
										if (!pQuest->GetSubQuest(param1))
										{
											_stprintf(temp, "!!! Error! @Event_Count subQuest ( %d ) is invalid. \r\n", param1);
										}
										else
										{
											_stprintf(temp, "   @Event_Count subQuest ( %d )\r\n", param1);
										}
										m_outString += temp;

										if (param2 <=0)
										{
											m_outString += "!!! Error! @Event_Count param2 > 0 \r\n";
										}

										break;
									}
									case eQuestEvent_GameEnter:
									{

										break;
									}
									case eQuestEvent_Level:
									{

										break;
									}
									case eQuestEvent_UseItem:
									{

										break;
									}
									case eQuestEvent_MapChange:
									{

										break;
									}
									case eQuestEvent_Die:
									{
										// Param1 is Map Num
										LPCTSTR pStrMap = 0;
										if( *( pStrMap = GetMapName(static_cast<MAPTYPE>(param1)) ) )
										{
											if (_tcscmp(pStrMap, _T("?")))
											{
												_stprintf(temp, "   @Event_Die map %d ( %s )\r\n", param1, pStrMap);
												m_outString += temp;
												break;
											}
										}

										_stprintf(temp, "!!! Error! @Event_Die map ( %d ) Invalid.\r\n", param1);
										m_outString += temp;
										break;
									}
									case eQuestEvent_HuntAll:
									{

										break;
									}
								}


							}
						}

						// Execute Check;
						cPtrList* pExeList = NULL ;
						pExeList = pTrigger->GetExeList() ;								// 트리거 실행 리스트를 받는다.

						if( !pExeList )	continue ;										// 트리거 실행 리스트 정보가 유효하다면,

						int nExeCount = pExeList->GetCount() ;					// 실행 리스트 카운트를 받는다.

						CQuestExecute* pExecute = NULL ;						// 퀘스트 실행 정보를 받을 포인터를 선언하고 null처리를 한다.
						PTRLISTPOS exePos = NULL ;

						for(int count2 = 0 ; count2 < nExeCount ; ++count2 )	// 실행 카운트 만큼 for문을 돌린다.
						{
							exePos = NULL ;										// 실행 위치 포인터를 null 처리를 한다.
							exePos = pExeList->FindIndex(count2) ;				// 카운트에 해당하는 실행 위치 정보를 받는다.									

							if( !exePos ) continue ;										// 퀘스트 실행 위치가 유효하면,

							pExecute = (CQuestExecute*)pExeList->GetAt(exePos) ;	// 퀘스트 실행 정보를 받는다.

							if( !pExecute ) continue ;								// 퀘스트 실행 정보가 유효하면,

							DWORD dwExeKind = pExecute->GetQuestExecuteKind() ;	// 실행 타입을 받는다.

							switch(dwExeKind)							// 실행 타입을 확인한다.
							{
							case eQuestExecute_EndQuest:
								{
									m_outString += "    *EndQuest \r\n";
								}
								break;
							case eQuestExecute_StartQuest:
								{
									// 안쓰이고 있음.
								}
								break;
							case eQuestExecute_EndSub:
								{
									m_outString += "    *EndSub \r\n";
								}
								break;
							case eQuestExecute_EndOtherSub:
								{
									m_outString += "    *EndOtherSub \r\n";
								}
								break;
							case eQuestExecute_StartSub:
								{
									CQuestExecute_Quest* pExecute_Quest = ((CQuestExecute_Quest*)pExecute);

									DWORD exeQuest = pExecute_Quest->GetQuestExeIdx();
									DWORD exeSubQuest = pExecute_Quest->GetSubQuestExeIdx();
									
									if (!QUESTMGR->GetQuest(exeQuest))
									{
										_stprintf(temp, "!!! Error! *StartSub Quest ( %d ) is invalid index.\r\n", exeQuest);
									}
									else
									{
										_stprintf(temp, "    *StartSub Quest ( %d )\r\n", exeQuest);
									}
									m_outString += temp;

									if (!pQuest->GetSubQuest(exeSubQuest))
									{
										_stprintf(temp, "!!! Error! *StartSub subQuest ( %d ) is invalid.\r\n", exeSubQuest);
									}
									else
									{
										_stprintf(temp, "    *StartSub subQuest ( %d )\r\n", exeSubQuest);
									}
									m_outString += temp;
									

								}
								break;

							case eQuestExecute_AddCount:
								{
									CQuestExecute_Count* pExecute_Count = ((CQuestExecute_Count*)pExecute);
									
									DWORD exeSubQuest = pExecute_Count->GetRealSubQuestIdx();
									DWORD maxCount = pExecute_Count->GetMaxCount();

									if (!pQuest->GetSubQuest(exeSubQuest))
									{
										_stprintf(temp, "!!! Error! *AddCount subQuest ( %d ) is invalid.\r\n", exeSubQuest);
									}
									else
									{
										_stprintf(temp, "    *AddCount subQuest ( %d ), maxCount = %d\r\n", exeSubQuest, maxCount);
									}
									m_outString += temp;

								}
								break;
							case eQuestExecute_MinusCount:
								{
									// 안쓰이는 듯
								}
								break;

							case eQuestExecute_GiveQuestItem:
								{
									CQuestExecute_Item* pExecute_Item = ((CQuestExecute_Item*)pExecute);

									QUEST_ITEM_INFO * pItemInfo = QUESTMGR->GetQuestItem(pExecute_Item->GetItemKind());
									int itemNum = pExecute_Item->GetItemNum();

									if ( pItemInfo )
									{
										_stprintf(temp, "    *GiveQuestItem item %d( %s ), num = %d \r\n", pItemInfo->ItemIdx, pItemInfo->ItemName, itemNum );									
									}
									else
									{
										_stprintf(temp, "!!! Error! *GiveQuestItem item %d \r\n", pExecute_Item->GetItemKind());
									}
									m_outString += temp;
								}
								break;
							case eQuestExecute_TakeQuestItem:
								{
									CQuestExecute_Item* pExecute_Item = ((CQuestExecute_Item*)pExecute);

									QUEST_ITEM_INFO * pItemInfo = QUESTMGR->GetQuestItem(pExecute_Item->GetItemKind());
									int itemNum = pExecute_Item->GetItemNum();
									int itemProb = pExecute_Item->GetItemProbability();

									if ( pItemInfo )
									{
										_stprintf(temp, "    *TakeQuestItem item %d( %s ), num = %d, Prob = %d\r\n", pItemInfo->ItemIdx, pItemInfo->ItemName, itemNum, itemProb);
									}
									else
									{
										_stprintf(temp, "!!! Error! *TakeQuestItem item %d \r\n", pExecute_Item->GetItemKind());
									}
									m_outString += temp;
								}
								break;
							case eQuestExecute_GiveItem:
								{
									CQuestExecute_Item* pExecute_Item = ((CQuestExecute_Item*)pExecute);

									ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(pExecute_Item->GetItemKind());
									int itemNum = pExecute_Item->GetItemNum();

									if ( pItemInfo )
									{
										_stprintf(temp, "    *GiveItem item %d( %s ), num = %d \r\n", pItemInfo->ItemIdx, pItemInfo->ItemName, itemNum );
									}
									else
									{
										_stprintf(temp, "!!! Error! *GiveItem item %d \r\n", pExecute_Item->GetItemKind());
									}
									m_outString += temp;
								}
								break;
							case eQuestExecute_GiveMoney:
								{
									CQuestExecute_Item* pExecute_Item = ((CQuestExecute_Item*)pExecute);

									_stprintf(temp, "    *GiveMoney  %d \r\n", pExecute_Item->GetItemNum());
									m_outString += temp;
								}
								break;

							case eQuestExecute_TakeItem :				// 실행 타입이 아이템을 받는 타입이라면,
								{
									CQuestExecute_Item* pExecute_Item = ((CQuestExecute_Item*)pExecute);

									ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(pExecute_Item->GetItemKind());
									int itemNum = pExecute_Item->GetItemNum();
									int itemProb = pExecute_Item->GetItemProbability();

									if ( pItemInfo )
									{
										_stprintf(temp, "    *TakeItem item %d( %s ), num = %d, Prob = %d\r\n", pItemInfo->ItemIdx, pItemInfo->ItemName, itemNum, itemProb);
									}
									else
									{
										_stprintf(temp, "!!! Error! *TakeItem item %d \r\n", pExecute_Item->GetItemKind());
									}
									m_outString += temp;
								}
								break ;

							case eQuestExecute_TakeMoney :				// 실행 타입이 골드를 받는 타입이라면,
								{
									CQuestExecute_Item* pExecute_Item = ((CQuestExecute_Item*)pExecute);

									_stprintf(temp, "     *TakeMoney  %d \r\n", pExecute_Item->GetItemNum());
									m_outString += temp;
								}
								break ;

							case eQuestExecute_TakeExp :				// 실행 타입이 경험치를 받는 타입이라면,
								{
									CQuestExecute_Item* pExecute_Item = ((CQuestExecute_Item*)pExecute);

									_stprintf(temp, "    *TakeExp  %d \r\n", pExecute_Item->GetItemNum());
									m_outString += temp;
								}
								break ;

							case eQuestExecute_TakeQuestItemFQW:
								{
									CQuestExecute_Item* pExecute_Item = ((CQuestExecute_Item*)pExecute);

									QUEST_ITEM_INFO * pItemInfo = QUESTMGR->GetQuestItem(pExecute_Item->GetItemKind());
									int itemNum = pExecute_Item->GetItemNum();
									int itemProb = pExecute_Item->GetItemProbability();
									int weaponKind = pExecute_Item->GetWeaponKind();
									ITEM_INFO * pWeaponInfo = ITEMMGR->GetItemInfo(weaponKind);

									if ( pItemInfo )
									{
										_stprintf(temp, "    *TakeQuestItemFQW item %d( %s ), num = %d, Prob = %d\r\n", pItemInfo->ItemIdx, pItemInfo->ItemName, itemNum, itemProb);
									}
									else
									{
										_stprintf(temp, "!!! Error! *TakeQuestItemFQW item %d \r\n", pExecute_Item->GetItemKind());
									}
									m_outString += temp;

									if ( pWeaponInfo )
									{
										_stprintf(temp, "    *TakeQuestItemFQW weapon %d( %s )\r\n", weaponKind, pWeaponInfo->ItemName);
									}
									else
									{
										_stprintf(temp, "!!! Error! *TakeQuestItemFQW weapon %d \r\n", weaponKind);
									}
									m_outString += temp;
								}
								break;
							case eQuestExecute_AddCountFQW:
								{
									CQuestExecute_Count* pExecute_Count = ((CQuestExecute_Count*)pExecute);

									DWORD exeSubQuest = pExecute_Count->GetRealSubQuestIdx();
									DWORD maxCount = pExecute_Count->GetMaxCount();
									int weaponKind = pExecute_Count->GetWeaponKind();
									ITEM_INFO * pWeaponInfo = ITEMMGR->GetItemInfo(weaponKind);

									if (!pQuest->GetSubQuest(exeSubQuest))
									{
										_stprintf(temp, "!!! Error! *AddCountFQW subQuest ( %d ) is invalid. \r\n", exeSubQuest);
									}
									else
									{
										_stprintf(temp, "    *AddCountFQW subQuest ( %d ), maxCount = %d \r\n", exeSubQuest, maxCount);
									}
									m_outString += temp;

									if ( pWeaponInfo )
									{
										_stprintf(temp, "    *AddCountFQW weapon %d( %s )\r\n", weaponKind, pWeaponInfo->ItemName);
									}
									else
									{
										_stprintf(temp, "!!! Error! *AddCountFQW weapon %d \r\n", weaponKind);
									}
									m_outString += temp;
								}
								break;
							case eQuestExecute_TakeMoneyPerCount:
								{
									CQuestExecute_Item* pExecute_Item = ((CQuestExecute_Item*)pExecute);

									ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(pExecute_Item->GetItemKind());
									int itemNum = pExecute_Item->GetItemNum();

									if ( pItemInfo )
									{
										_stprintf(temp, "    *TakeMoneyPerCount item %d( %s ), num = %d\r\n", pItemInfo->ItemIdx, pItemInfo->ItemName, itemNum);
									}
									else
									{
										_stprintf(temp, "!!! Error! *TakeMoneyPerCount item %d \r\n", pExecute_Item->GetItemKind());
									}
									m_outString += temp;
								}
								break;
							case eQuestExecute_MapChange:
								{
									CQuestExecute_Quest* pExecute_Quest = ((CQuestExecute_Quest*)pExecute);

									DWORD mapNum = pExecute_Quest->GetMapNum();
									float xPos = pExecute_Quest->GetXPos();
									float zPos = pExecute_Quest->GetZPos();
									
									LPCTSTR pStrStartMap = 0;
									if( *( pStrStartMap = GetMapName(static_cast<MAPTYPE>(mapNum)) ) )
									{
										if (_tcscmp(pStrStartMap, _T("?")))
										{
											_stprintf(temp, "    *MapChange map %d (%s), Pos ( %.1f , %.1f )\r\n", startMapIdx, pStrStartMap, xPos, zPos);
											m_outString += temp;
										}
									}
									else
									{
										_stprintf(temp, "!!! Error! *MapChange map ( %d ) Invalid.\r\n", mapNum);
										m_outString += temp;
									}

									break;
								}
							case eQuestExecute_ChangeStage:
								{
									CQuestExecute_Quest* pExecute_Quest = ((CQuestExecute_Quest*)pExecute);

									DWORD exeQuest = pExecute_Quest->GetQuestExeIdx();
									DWORD exeSubQuest = pExecute_Quest->GetSubQuestExeIdx();

									_stprintf(temp, "    *ChangeStage ( %d , %d ) \r\n", exeQuest, exeSubQuest);
									m_outString += temp;

									break;
								}
							default :
								{
									_stprintf(temp, "!!!  Warning!  UnDefined Kind %d \r\n", dwExeKind);
									m_outString += temp;
								}
							}
						}
					}
				}
			}
			m_outString += "\r\n";

			// #NPCSCRIPT Check

			CQuestNpcScript* pQuestNpcScript = subQuestInfo->GetQuestNpcScript();
			if( pQuestNpcScript )
			{
				DWORD npcIdx = pQuestNpcScript->GetNpcIdx();
				DWORD scriptPage = pQuestNpcScript->GetScriptPage();
				DWORD markType = pQuestNpcScript->GetNpcMarkType();
				DWORD DialogType = pQuestNpcScript->GetDialogType();

				stNPCIDNAME* pNpc = QUESTMGR->GetNpcIdName(npcIdx);

				if (pNpc)
				{
					_stprintf(temp, "    #QuestNpcScript NpcIdx %d ( %s )\r\n", npcIdx, pNpc->name);
				}
				else
				{
					QUESTNPCINFO* pQuestNpcInfo = QUESTMGR->GetQusetNpcInfo(npcIdx);
					if (pQuestNpcInfo)
					{
						_stprintf(temp, "    #QuestNpcScript QuestNpcIdx %d ( %s )\r\n", npcIdx, pQuestNpcInfo->sName);
					}
					else
					{
						CNpcRecallBase* pNpcRecallBase = NPCRECALLMGR->Get_RecallBaseByNpcId(npcIdx);
						if (pNpcRecallBase)
						{
							_stprintf(temp, "    #QuestNpcScript QuestNpcIdx %d ( %s )\r\n", npcIdx, pNpcRecallBase->Get_NpcName());
						}
						else
						{
							_stprintf(temp, "!!! Error! #QuestNpcScript NpcIdx ( %d ) Invalid.\r\n", npcIdx);
						}
					}
				}
				m_outString += temp;

				cPage* pPage = NPCSCRIPTMGR->GetPage( npcIdx, scriptPage );
				if (pPage)
				{
					_stprintf(temp, "    #QuestNpcScript scriptPage %d \r\n", scriptPage);
				}
				else
				{
					_stprintf(temp, "!!! Error! #QuestNpcScript scriptPage %d Invalid.\r\n", scriptPage);
				}
				m_outString += temp;

				_stprintf(temp, "    #QuestNpcScript markType = %d, DialogType = %d \r\n", markType, DialogType);
				m_outString += temp;
			}
			m_outString += "\r\n";

			// #NPCADD Check;
			CQuestNpc* pQuestNpc = NULL;

			cPtrList& pQuestNpcList = subQuestInfo->GetQuestNpcList();

			PTRLISTPOS questNpcPos = pQuestNpcList.GetHeadPosition();

			while( questNpcPos )
			{
				pQuestNpc = (CQuestNpc*)pQuestNpcList.GetNext( questNpcPos );

				if( pQuestNpc )
				{
					cPtrList& pNpcDataList = pQuestNpc->GetNpcDataList();
					PTRLISTPOS pos = pNpcDataList.GetHeadPosition();

					while( pos )																		// 위치 정보가 유효한 동안 while문을 돌린다.
					{
						CQuestNpcData* pData = (CQuestNpcData*)pNpcDataList.GetNext( pos );					// 위치에 해당하는 퀘스트 npc 데이터 정보를 받는다.

						if( pData )																		// 데이터 정보가 유효한지 체크한다.
						{
							DWORD npcIdx = pData->GetNpcIdx();
							stNPCIDNAME* pNpc = QUESTMGR->GetNpcIdName(npcIdx);

							if (pNpc)
							{
								_stprintf(temp, "    #AddNpc NpcIdx %d ( %s )\r\n", npcIdx, pNpc->name);
							}
							else
							{
								QUESTNPCINFO* pQuestNpcInfo = QUESTMGR->GetQusetNpcInfo(npcIdx);
								if (pQuestNpcInfo)
								{
									_stprintf(temp, "    #AddNpc QuestNpcIdx %d ( %s )\r\n", npcIdx, pQuestNpcInfo->sName);
								}
								else
								{
									CNpcRecallBase* pNpcRecallBase = NPCRECALLMGR->Get_RecallBaseByNpcId(npcIdx);
									if (pNpcRecallBase)
									{
										_stprintf(temp, "    #AddNpc QuestNpcIdx %d ( %s )\r\n", npcIdx, pNpcRecallBase->Get_NpcName());
									}
									else
									{
										_stprintf(temp, "!!! Error! #AddNpc NpcIdx ( %d ) Invalid.\r\n", npcIdx);
									}
								}
							}
							m_outString += temp;
						}
					}
				}
			}
		}
	}

	m_outString += "\r\n*** End Quest Test *** \r\n";
	UpdateOutString();

	return TRUE;
}

INT_PTR CALLBACK TSDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch(message)
	{
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
				TSTOOLMGR->OnClickMenu( LOWORD( wParam ) - IDC_TS_MNBTN_CHARACTOR  );
			}
		}
		return TRUE;
	case WM_CLOSE:
		{
			//			TSTOOLMGR->DestroyTSDialog();
			TSTOOLMGR->ShowTSDialog( FALSE, TRUE );
		}
		return TRUE;
	}


	return FALSE;
}
INT_PTR CALLBACK TSSubCharDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{

		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_TSCHAR_BTN_APPLY:
					{
						TSTOOLMGR->OnCharCommand();
					}
					break;
				case IDC_TSCHAR_BTN_CANCEL:
					{
						TSTOOLMGR->OnClickMenu( eTSMenu_Count );
						SetFocus( _g_hWnd );
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
INT_PTR CALLBACK TSOutDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
		case WM_INITDIALOG:
			{
				
			}
			break;
		case WM_COMMAND:
			break;

	}

	return FALSE;
}
INT_PTR CALLBACK TSSubItemDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckRadioButton( hWnd, IDC_TSITEM_BTN_WEAPON, IDC_TSITEM_BTN_ABILITY, IDC_TSITEM_BTN_WEAPON );
			EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_BTN_SEARCH ),		TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDIT_SEARCH),		TRUE );
			SetWindowText( GetDlgItem( hWnd, IDC_TSITEM_SEARCHFROM), "from Weapon List");
		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_TSITEM_BTN_SEARCH:
					{
						int nCurSelDlgID = -1;
						int nCurSelCBID = -1;

						if( SendMessage( GetDlgItem( hWnd, IDC_TSITEM_BTN_WEAPON ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_TSITEM_BTN_WEAPON;
							nCurSelCBID = IDC_TSITEM_CMB_WEAPON;
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_TSITEM_BTN_CLOTHES ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_TSITEM_BTN_CLOTHES;
							nCurSelCBID = IDC_TSITEM_CMB_CLOTHES;
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_TSITEM_BTN_ACCESSORY ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_TSITEM_BTN_ACCESSORY;
							nCurSelCBID = IDC_TSITEM_CMB_ACCESSORY;
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_TSITEM_BTN_SKILLBOOK ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_TSITEM_BTN_SKILLBOOK;
							nCurSelCBID = IDC_TSITEM_CMB_SKILLBOOK;
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_TSITEM_BTN_POTION ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_TSITEM_BTN_POTION;
							nCurSelCBID = IDC_TSITEM_CMB_POTION;
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_TSITEM_BTN_ETC ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_TSITEM_BTN_ETC;
							nCurSelCBID = IDC_TSITEM_CMB_ETC;
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_TSITEM_BTN_SKILL ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							nCurSelDlgID = IDC_TSITEM_BTN_SKILL;
							nCurSelCBID = IDC_TSITEM_CMB_SKILL;
						}

						if(-1<nCurSelDlgID && -1<nCurSelCBID)
						{
							HWND hWndCB = GetDlgItem(hWnd, IDC_TSITEM_EDIT_SEARCH);
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
								}
							}
						}
					}
					break;

				case IDC_TSITEM_BTN_GET:
					{
						TSTOOLMGR->OnItemCommand();
					}
					break;
				case IDC_TSITEM_BTN_CANCEL:
					{
						TSTOOLMGR->OnClickMenu( eTSMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;
				case IDC_TSITEM_BTN_WEAPON:
					{
						TSTOOLMGR->ItemDlgCtrlDisableAll();

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_WEAPON ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_WEAPONGRADE ),	TRUE );

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_TSITEM_SEARCHFROM), "from Weapon List");
					}
					break;
				case IDC_TSITEM_BTN_CLOTHES:
					{
						TSTOOLMGR->ItemDlgCtrlDisableAll();

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_CLOTHES ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_CLOTHESGRADE ),	TRUE );

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_TSITEM_SEARCHFROM), "from Clothes List");
					}
					break;
				case IDC_TSITEM_BTN_ACCESSORY:
					{
						TSTOOLMGR->ItemDlgCtrlDisableAll();

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_ACCESSORY ),		TRUE );

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_TSITEM_SEARCHFROM), "from Accessory List");
					}
					break;
				case IDC_TSITEM_BTN_SKILLBOOK:
					{
						TSTOOLMGR->ItemDlgCtrlDisableAll();

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_SKILLBOOK ),		TRUE );

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_TSITEM_SEARCHFROM), "from SkillBook List");
					}
					break;
				case IDC_TSITEM_BTN_POTION:
					{
						TSTOOLMGR->ItemDlgCtrlDisableAll();

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_POTION ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDT_POTION ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_SPN_POTION ),		TRUE );

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_TSITEM_SEARCHFROM), "from Potion List");
					}
					break;
				case IDC_TSITEM_BTN_ETC:
					{
						TSTOOLMGR->ItemDlgCtrlDisableAll();

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_ETC ),			TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDT_ETC ),			TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_SPN_ETC ),			TRUE );

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_TSITEM_SEARCHFROM), "from Etc List");
					}
					break;
				case IDC_TSITEM_BTN_MONEY:
					{
						TSTOOLMGR->ItemDlgCtrlDisableAll();

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDT_MONEY ),			TRUE );

						SetWindowText( GetDlgItem( hWnd, IDC_TSITEM_SEARCHFROM), "from List");
					}
					break;
				case IDC_TSITEM_BTN_SKILL:
					{
						TSTOOLMGR->ItemDlgCtrlDisableAll();

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_SKILL ),			TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_CMB_TOSLOT ),		TRUE );

						char buf[64];
						GetDlgItemText( hWnd, IDC_TSITEM_CMB_SKILL, buf, MAX_ITEMNAME_LENGTH+1 );

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_BTN_SEARCH ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDIT_SEARCH),		TRUE );
						SetWindowText( GetDlgItem( hWnd, IDC_TSITEM_SEARCHFROM), "from Skill List");
					}
					break;
				case IDC_TSITEM_BTN_ABILITY:
					{
						TSTOOLMGR->ItemDlgCtrlDisableAll();

						EnableWindow( GetDlgItem( hWnd, IDC_TSITEM_EDT_ABILITY ),		TRUE );
					}
					break;
				}
			}
			else if( HIWORD( wParam ) == CBN_SELCHANGE )
			{
				if( LOWORD( wParam ) == IDC_TSITEM_CMB_SKILL )
				{
					char buf[64];

					//GetDlgItemText( hWnd, IDC_TSITEM_CMB_SKILL, buf, MAX_ITEMNAME_LENGTH+1 );
					int nSel = SendDlgItemMessage( hWnd, IDC_TSITEM_CMB_SKILL, CB_GETCURSEL, 0, 0 );

					SendDlgItemMessage( hWnd, IDC_TSITEM_CMB_SKILL, CB_GETLBTEXT, nSel, (LPARAM)buf );
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

INT_PTR CALLBACK TSSubRegenDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckRadioButton( hWnd, IDC_TSREGEN_BTN_MODEDIRECT, IDC_TSREGEN_BTN_DELETE, IDC_TSREGEN_BTN_MODEDIRECT );

			SetDlgItemInt( hWnd, IDC_TSREGEN_EDT_MOBNUM, 1, TRUE );
			//SetDlgItemInt( hWnd, IDC_TSREGEN_EDT_CHANNEL, 0, TRUE );
			SetDlgItemInt( hWnd, IDC_TSREGEN_EDT_RAD, 10, TRUE );

			SetDlgItemInt( hWnd, IDC_TSREGEN_EDT_X, 0, TRUE );
			SetDlgItemInt( hWnd, IDC_TSREGEN_EDT_Y, 0, TRUE );

			SetDlgItemInt( hWnd, IDC_TSREGEN_EDT_ITEM, 0, TRUE );

			//SetChannelFromMap(hWnd, IDC_TSREGEN_CMB_MAP, IDC_TSREGEN_CMB_CHANNEL);
		}
		return FALSE;

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_TSREGEN_BTN_APPLY:
					{
						TSTOOLMGR->OnRegenCommand();
					}
					break;
				case IDC_TSREGEN_BTN_CANCEL:
					{
						TSTOOLMGR->OnClickMenu( eTSMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;
				case IDC_TSREGEN_BTN_MODEDIRECT:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_CMB_MOBNAME ),	TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_MOBNUM ),	TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_CMB_MAP ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_X ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_Y ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_BTN_MYPOS ),	TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_RAD ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_CMB_ITEM ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_ITEM ),		TRUE );

						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_BTN_OPEN ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_FILE ),		FALSE );
					}
					break;
				case IDC_TSREGEN_BTN_MODEFILE:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_CMB_MOBNAME ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_MOBNUM ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_CMB_MAP ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_X ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_Y ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_BTN_MYPOS ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_RAD ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_CMB_ITEM ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_ITEM ),		FALSE );

						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_BTN_OPEN ),		TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_FILE ),		TRUE );
					}
					break;
				case IDC_TSREGEN_BTN_DELETE:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_CMB_MOBNAME ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_MOBNUM ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_CMB_MAP ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_X ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_Y ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_BTN_MYPOS ),	FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_RAD ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_CMB_ITEM ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_ITEM ),		FALSE );

						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_BTN_OPEN ),		FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSREGEN_EDT_FILE ),		FALSE );
					}
					break;
				case IDC_TSREGEN_BTN_MYPOS:
					{
						if( HERO )
						{
							VECTOR3 vPos;
							HERO->GetPosition( &vPos );
							SetDlgItemInt( hWnd, IDC_TSREGEN_EDT_X, (UINT)(vPos.x / 100.0f), TRUE );
							SetDlgItemInt( hWnd, IDC_TSREGEN_EDT_Y, (UINT)(vPos.z / 100.0f), TRUE );
						}

						SetDlgItemText( hWnd, IDC_TSREGEN_CMB_MAP, GetMapName( MAP->GetMapNum() ) );
					}
					break;

				case IDC_TSREGEN_BTN_OPEN:
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
							SetDlgItemText( hWnd, IDC_TSREGEN_EDT_FILE, OFN.lpstrFile );
						}
					}
					break;
				}
			}
			else if( HIWORD( wParam ) == CBN_SELENDOK )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_TSREGEN_CMB_MAP:
					{
						//SetChannelFromMap(hWnd, IDC_TSREGEN_CMB_MAP, IDC_TSREGEN_CMB_CHANNEL);
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

INT_PTR CALLBACK TSSubMoveDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			CheckRadioButton( hWnd, IDC_TSMOVE_BTN_XY, IDC_TSMOVE_BTN_MAP, IDC_TSMOVE_BTN_XY );			
			//SetDlgItemInt( hWnd, IDC_TSMOVE_EDT_X, 0, TRUE );
			//SetDlgItemInt( hWnd, IDC_TSMOVE_EDT_Y, 0, TRUE );

			//SetChannelFromMap(hWnd, IDC_TSMOVE_CMB_MAP, IDC_TSMOVE_CMB_CHANNEL);
		}
		return FALSE;	//not active

	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_TSMOVE_OK:	//Do Move
					{
						if( SendMessage( GetDlgItem( hWnd, IDC_TSMOVE_BTN_XY ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{							
							TSTOOLMGR->OnMoveCommand( 0 );
						}
						else if( SendMessage( GetDlgItem( hWnd, IDC_TSMOVE_BTN_MAP ), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
						{
							TSTOOLMGR->OnMoveCommand( 1 );
						}
					}
					break;

				case IDC_TSMOVE_CANCEL:
					{
						TSTOOLMGR->OnClickMenu( eTSMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;

				case IDC_TSMOVE_BTN_XY:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_TSMOVE_EDT_X ), TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSMOVE_EDT_Y ), TRUE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSMOVE_CMB_MAP ), FALSE );

						SetFocus( GetDlgItem( hWnd, IDC_TSMOVE_EDT_X ) );
					}
					break;

				case IDC_TSMOVE_BTN_MAP:
					{
						EnableWindow( GetDlgItem( hWnd, IDC_TSMOVE_EDT_X ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSMOVE_EDT_Y ), FALSE );
						EnableWindow( GetDlgItem( hWnd, IDC_TSMOVE_CMB_MAP ), TRUE );

						SetFocus( GetDlgItem( hWnd, IDC_TSMOVE_CMB_MAP ) );
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


INT_PTR CALLBACK TSSubQuestDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_TSQUEST_BTN_TEST:	//Do Move
					{
						TSTOOLMGR->OnQuestCommand();
					}
					break;

				case IDC_TSQUEST_BTN_CANCLE:
					{
						TSTOOLMGR->OnClickMenu( eTSMenu_Count );
						SetFocus( _g_hWnd );
					}
					break;
				}
			}
		}
		return TRUE;
	}

	return FALSE;
}

INT_PTR CALLBACK TSSubWeatherDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_COMMAND:
		{
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				switch( LOWORD( wParam ) )
				{
				case IDC_TSWEATHER_BTN_APPLY:
					{
						char weatherTxt[MAX_PATH]={0,};
						int nSel = SendDlgItemMessage( hWnd, IDC_TSWEATHER_CMB_TYPE, CB_GETCURSEL, 0, 0 );
						SendDlgItemMessage( hWnd, IDC_TSWEATHER_CMB_TYPE, CB_GETLBTEXT, nSel, (LPARAM)weatherTxt );

						BOOL rt;
						int nIntensity = GetDlgItemInt( hWnd, IDC_TSWEATHER_EDIT_INTENSITY, &rt, TRUE );
						if( !rt )	break;

						WEATHERMGR->EffectOn( weatherTxt, (WORD)nIntensity );
						CHATMGR->AddMsg( CTC_TOGM, "Weather Effect On : %s[%d]", weatherTxt, nIntensity );
					}
					break;

				case IDC_TSWEATHER_BTN_STOP:
					{
						WEATHERMGR->EffectOff();
						CHATMGR->AddMsg( CTC_TOGM, "Weather Effect Off" );
					}
					break;
				case IDC_TSWEATHER_BTN_SHOW_INFO:
					{
						WEATHERMGR->ShowDebugState( TRUE );
					}
					break;
				case IDC_TSWEATHER_BTN_HIDE_INFO:
					{
						WEATHERMGR->ShowDebugState( FALSE );
					}
					break;
				}
			}
		}
		return TRUE;
	}

	return FALSE;
}

#endif// _TESTCLIENT_