// SheetPageMonster.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "RegenTool.h"
#include "MainFrm.h"
#include "RegenToolView.h"
#include "grid_control/GridCellCombo.h"
#include "grid_control/GridCellNumeric.h"
#include "grid_control/GridCellCheck.h"
#include "LoadList.h"
#include "GroupInfoInputDlg.h"
#include "ScriptViewDlg.h"
#include "SheetPageMonster.h"

UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

int CALLBACK MonsterTreeCompareProc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )				return 0;
	CSheetPageMonster* pMonsterPage = (CSheetPageMonster*)pFrm->GetMonsterPage();
	if( !pMonsterPage )		return 0;

	DWORD dwLeftGroup = pMonsterPage->GetGroupIndexFromTreeItemData( (DWORD) lParam1 );
	DWORD dwRightGroup = pMonsterPage->GetGroupIndexFromTreeItemData( (DWORD) lParam2 );

	DWORD dwLeftCondition = pMonsterPage->GetConditionIndexFromTreeItemData( (DWORD) lParam1 );
	DWORD dwRightCondition = pMonsterPage->GetConditionIndexFromTreeItemData( (DWORD) lParam2 );

	// 컨디션이 없는 경우( Group, Monster )
	if( dwLeftCondition == dwRightCondition )
	{
		if( dwLeftGroup == dwRightGroup )
			return 0;
		else if( dwLeftGroup < dwRightGroup )
			return -1;

		return 1;
	}
	else if( dwLeftCondition < dwRightCondition )
		return -1;
		
	return 1;
}


// CSheetPageMonster 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSheetPageMonster, CPropertyPage)
CSheetPageMonster::CSheetPageMonster()
	: CPropertyPage(CSheetPageMonster::IDD)
{
	m_bUseTreeSelectFunc = TRUE;

	m_strDefaultGroupName = _T("일반그룹_1");
	m_DefaultCondition.dwConditionIdx = 0;
	m_DefaultCondition.mGroupIndex = 0;
	m_DefaultCondition.mRatio = 0.5f;
	m_DefaultCondition.mDelay = 15000;
	m_DefaultCondition.mIsRegen = 1;
	m_DefaultCondition.mRange = 1000;

	m_pFindDialog = NULL;
}

CSheetPageMonster::~CSheetPageMonster()
{
	m_mapMonsterTreeInfo.clear();
}

void CSheetPageMonster::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_MONTERVIEW, m_MonsterTree);
	DDX_Control(pDX, IDC_ADD_GROUP_BUTTON, m_AddGroupBtn);
	//	DDX_Control(pDX, IDC_LISTCTRL_INFO, m_InfoListCtrl);
	DDX_Control(pDX, IDC_STATIC_MONSTERLIST, m_MonsterListStatic);
	DDX_Control(pDX, IDC_COMBO_MONSTER_LIST, m_MonsterListCombo);
	DDX_Control(pDX, IDC_LISTVIEW_BTN, m_ScriptViewBtn);
	DDX_Control(pDX, IDC_BUTTON_MODIFY_INFO, m_ModifyInfoBtn);
}


BEGIN_MESSAGE_MAP(CSheetPageMonster, CPropertyPage)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_MONTERVIEW, OnTvnSelchangedTreeMonterview)
	ON_BN_CLICKED(IDC_LISTVIEW_BTN, OnBnClickedListviewBtn)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_MONTERVIEW, OnNMRclickTreeMonterview)
	ON_COMMAND(IDR_MENU_MONSTER_TREE_ADD_GROUP, OnMenuMonsterTreeAddGroup)
	ON_COMMAND(IDR_MENU_MONSTER_TREE_ADD_CONDITON, OnMenuMonsterTreeAddConditon)
	ON_COMMAND(IDR_MENU_MONSTER_TREE_DELETE, OnMenuMonsterTreeDelete)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT_SETTING, OnBnClickedButtonDefaultSetting)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_INFO, OnBnClickedButtonModifyInfo)
	ON_REGISTERED_MESSAGE( WM_FINDREPLACE, OnFindReplace )
END_MESSAGE_MAP()


// CSheetPageMonster 메시지 처리기입니다.

void CSheetPageMonster::ResetTree()
{
	m_MonsterTree.DeleteAllItems();
	m_mapMonsterTreeInfo.clear();

	TVINSERTSTRUCT tvi;
	ZeroMemory(&tvi, sizeof(TVINSERTSTRUCT));
	tvi.hParent = TVI_ROOT;
	tvi.hInsertAfter = TVI_LAST;
	tvi.item.pszText = _T("Monster");
	tvi.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM;
	tvi.item.cChildren = 1;
	tvi.item.lParam = 0;

	m_hTopItem = m_MonsterTree.InsertItem(&tvi);

	m_InfoListCtrl.DeleteAllItems();
}

void CSheetPageMonster::ExpandTree()
{
	for(HTREEITEM treeItem = m_MonsterTree.GetRootItem();
		0 < treeItem;
		treeItem = m_MonsterTree.GetNextItem(treeItem, TVGN_NEXT))
	{
		for(HTREEITEM childItem = m_MonsterTree.GetChildItem(treeItem);
			0 < childItem;
			childItem = m_MonsterTree.GetNextItem(childItem, TVGN_NEXT))
		{
			m_MonsterTree.Expand(
				childItem,
				TVE_EXPAND);
		}

		m_MonsterTree.Expand(
			treeItem,
			TVE_EXPAND);
	}
}

HTREEITEM CSheetPageMonster::AddTreeItem( LPCTSTR name, HTREEITEM hParent, LPARAM lParam, BOOL bHasChild )
{
	TVINSERTSTRUCT tvi = {0};
	tvi.hParent = hParent;
	tvi.hInsertAfter = TVI_LAST;
	tvi.item.pszText = (LPTSTR)name;
	tvi.item.mask = TVIF_TEXT | TVIF_PARAM;
	if( bHasChild )
		tvi.item.mask |= TVIF_CHILDREN;
	tvi.item.cChildren = 1;
	tvi.item.lParam = lParam;

	HTREEITEM hAddItem = m_MonsterTree.InsertItem(&tvi);

	TVSORTCB tvs;
	tvs.hParent = hParent;
	tvs.lpfnCompare = MonsterTreeCompareProc;
	tvs.lParam = (LPARAM)&m_MonsterTree;

	m_MonsterTree.SortChildrenCB( &tvs );

	return hAddItem;
}

HTREEITEM CSheetPageMonster::GetTreeItemFromItemText( const CString& str, HTREEITEM hItem, BOOL bSearchDown, BOOL bNextItem )
{
	// 모든 Tree노드에서 찾는다.
	HTREEITEM hFind = NULL, hChild = NULL, hSibling = NULL;

	if( hItem == NULL )
		hItem = m_hTopItem;

	CString strItem = m_MonsterTree.GetItemText( hItem );

	if( !bNextItem && strItem.Find( str ) >= 0 )
		return hItem;

	// 자식 노드 검색
	if( bSearchDown )
		hChild = m_MonsterTree.GetChildItem( hItem );
	else
	{
		HTREEITEM hLastSibling = m_MonsterTree.GetChildItem( hItem );
		while( (hLastSibling = m_MonsterTree.GetNextSiblingItem( hLastSibling )) != NULL )
		{
			hChild = hLastSibling;
		}
	}

	if( hChild )
	{
		hFind = GetTreeItemFromItemText( str, hChild, bSearchDown );
	}

	// 형제 노드 검색
	if( bSearchDown )
		hSibling = m_MonsterTree.GetNextSiblingItem( hItem );
	else
		hSibling = m_MonsterTree.GetPrevSiblingItem( hItem );
	if( hFind == NULL && hSibling )
	{
		hFind = GetTreeItemFromItemText( str, hSibling, bSearchDown );
	}

	return hFind;
}

HTREEITEM CSheetPageMonster::GetTreeItemFromGroup( DWORD dwGroup, HTREEITEM hItem )
{
	// 모든 Tree노드에서 찾는다.
	HTREEITEM hFind = NULL, hChild = NULL, hSibling = NULL;

	if( hItem == NULL )
		hItem = m_hTopItem;

	DWORD dwIndex = (DWORD)m_MonsterTree.GetItemData( hItem );
	std::map< DWORD, stMonsterTreeInfo >::const_iterator iter = m_mapMonsterTreeInfo.find( dwIndex );
	if( iter != m_mapMonsterTreeInfo.end() )
	{
		const stMonsterTreeInfo& treeInfo = iter->second;
		if( treeInfo.eType == eMonsterTreeInfoType_Group &&
			treeInfo.dwGroup == dwGroup )
		{
			return hItem;
		}
	}
	

	// 자식 노드 검색
	hChild = m_MonsterTree.GetChildItem( hItem );
	if( hChild )
	{
		hFind = GetTreeItemFromGroup( dwGroup, hChild );
	}

	// 형제 노드 검색
	hSibling = m_MonsterTree.GetNextSiblingItem( hItem );
	if( hFind == NULL && hSibling )
	{
		hFind = GetTreeItemFromGroup( dwGroup, hSibling );
	}

	return hFind;
}

HTREEITEM CSheetPageMonster::GetTreeItemFromCondition( DWORD dwGroup, DWORD dwCondition, HTREEITEM hItem )
{
	// 모든 Tree노드에서 찾는다.
	HTREEITEM hFind = NULL, hChild = NULL, hSibling = NULL;

	if( hItem == NULL )
		hItem = m_hTopItem;

	DWORD dwIndex = (DWORD)m_MonsterTree.GetItemData( hItem );
	std::map< DWORD, stMonsterTreeInfo >::const_iterator iter = m_mapMonsterTreeInfo.find( dwIndex );
	if( iter != m_mapMonsterTreeInfo.end() )
	{
		const stMonsterTreeInfo& treeInfo = iter->second;
		if( treeInfo.eType == eMonsterTreeInfoType_Condition &&
			treeInfo.dwGroup == dwGroup &&
			treeInfo.dwConditionIdx == dwCondition )
		{
			return hItem;
		}
	}
	

	// 자식 노드 검색
	hChild = m_MonsterTree.GetChildItem( hItem );
	if( hChild )
	{
		hFind = GetTreeItemFromCondition( dwGroup, dwCondition, hChild );
	}

	// 형제 노드 검색
	hSibling = m_MonsterTree.GetNextSiblingItem( hItem );
	if( hFind == NULL && hSibling )
	{
		hFind = GetTreeItemFromCondition( dwGroup, dwCondition, hSibling );
	}

	return hFind;
}

HTREEITEM CSheetPageMonster::GetTreeItemFromMonsterHandle( GXOBJECT_HANDLE handle, HTREEITEM hItem )
{
	// 모든 Tree노드에서 찾는다.
	HTREEITEM hFind = NULL, hChild = NULL, hSibling = NULL;

	if( hItem == NULL )
		hItem = m_hTopItem;

	DWORD dwIndex = (DWORD)m_MonsterTree.GetItemData( hItem );
	std::map< DWORD, stMonsterTreeInfo >::const_iterator iter = m_mapMonsterTreeInfo.find( dwIndex );
	if( iter != m_mapMonsterTreeInfo.end() )
	{
		const stMonsterTreeInfo& treeInfo = iter->second;
		if( treeInfo.eType == eMonsterTreeInfoType_Monster &&
			treeInfo.hMonster == handle )
		{
			return hItem;
		}
	}
	

	// 자식 노드 검색
	hChild = m_MonsterTree.GetChildItem( hItem );
	if( hChild )
	{
		hFind = GetTreeItemFromMonsterHandle( handle, hChild );
	}

	// 형제 노드 검색
	hSibling = m_MonsterTree.GetNextSiblingItem( hItem );
	if( hFind == NULL && hSibling )
	{
		hFind = GetTreeItemFromMonsterHandle( handle, hSibling );
	}

	return hFind;
}

int CSheetPageMonster::GetTreeInfoType( HTREEITEM hItem )
{
	if( hItem == NULL )
		return eMonsterTreeInfoType_None;

	if( hItem == m_hTopItem )
		return eMonsterTreeInfoType_Root;

	DWORD dwIndex = (DWORD)m_MonsterTree.GetItemData( hItem );
	std::map< DWORD, stMonsterTreeInfo >::const_iterator iter = m_mapMonsterTreeInfo.find( dwIndex );
	if( iter == m_mapMonsterTreeInfo.end() )
		return eMonsterTreeInfoType_None;
	
	const stMonsterTreeInfo& treeInfo = iter->second;

	return treeInfo.eType;
}

int CSheetPageMonster::GetSelectedTreeInfoTypeFromTree()
{
	HTREEITEM hSelectedItem = m_MonsterTree.GetSelectedItem();
	if( hSelectedItem == NULL )
		return eMonsterTreeInfoType_None;

	return GetTreeInfoType( hSelectedItem );
}

void* CSheetPageMonster::GetTreeInfo( HTREEITEM hItem )
{
	if( hItem == NULL )
		return NULL;

	DWORD dwIndex = (DWORD)m_MonsterTree.GetItemData( hItem );
	std::map< DWORD, stMonsterTreeInfo >::iterator iter = m_mapMonsterTreeInfo.find( dwIndex );
	if( iter == m_mapMonsterTreeInfo.end() )
		return NULL;
	
	stMonsterTreeInfo& treeInfo = iter->second;

	return &treeInfo;
}

void* CSheetPageMonster::GetSelectedTreeInfoFromTree()
{
	HTREEITEM hSelectedItem = m_MonsterTree.GetSelectedItem();
	if( hSelectedItem == NULL )
		return NULL;

	return GetTreeInfo( hSelectedItem );
}

DWORD CSheetPageMonster::GetSelectedGroupIndexFromTree()
{
	stMonsterTreeInfo* pInfo = (stMonsterTreeInfo*)GetSelectedTreeInfoFromTree();
	if( !pInfo )
		return 0;

	return pInfo->dwGroup;
}

DWORD CSheetPageMonster::GetGroupNumFromMonsterHandle( GXOBJECT_HANDLE handle )
{
	for( std::map< DWORD, stMonsterTreeInfo >::const_iterator iter = m_mapMonsterTreeInfo.begin() ; iter != m_mapMonsterTreeInfo.end() ; ++iter )
	{
		const stMonsterTreeInfo& treeInfo = iter->second;
		if( treeInfo.hMonster == handle )
			return treeInfo.dwGroup;
	}

	return 0;
}

DWORD CSheetPageMonster::GetGroupIndexFromTreeItemData( DWORD dwItemData )
{
	std::map< DWORD, stMonsterTreeInfo >::const_iterator iter = m_mapMonsterTreeInfo.find( dwItemData );
	if( iter == m_mapMonsterTreeInfo.end() )
		return 0;

	const stMonsterTreeInfo& treeInfo = iter->second;

	return treeInfo.dwGroup;
}

DWORD CSheetPageMonster::GetConditionIndexFromTreeItemData( DWORD dwItemData )
{
	std::map< DWORD, stMonsterTreeInfo >::const_iterator iter = m_mapMonsterTreeInfo.find( dwItemData );
	if( iter == m_mapMonsterTreeInfo.end() )
		return 0;

	const stMonsterTreeInfo& treeInfo = iter->second;

	return treeInfo.dwConditionIdx;
}

void CSheetPageMonster::SelectMonster( GXOBJECT_HANDLE handle )
{
	HTREEITEM hItem = GetTreeItemFromMonsterHandle( handle );
	if( hItem == NULL )
		return;

	// Select() 를 이용하여 선택한경우 TVN_SELCHANGED 가 발생시 구분할수 있도록 변수를 설정한다.
	m_bUseTreeSelectFunc = FALSE;
	m_MonsterTree.Select( hItem, TVGN_CARET );
}

void CSheetPageMonster::SelectTreeItem( HTREEITEM hItem, BOOL bUseTreeSelectFuc )
{
	// Select() 를 이용하여 선택한경우 TVN_SELCHANGED 가 발생시 구분할수 있도록 변수를 설정한다.
	m_bUseTreeSelectFunc = TRUE;
	m_MonsterTree.Select( hItem, TVGN_CARET );
}

void CSheetPageMonster::UpdateSelectedInfo()
{
	if( m_InfoListCtrl.GetSafeHwnd() == NULL )
		return;

	m_InfoListCtrl.DeleteAllItems();

	CRect rect;
	m_InfoListCtrl.GetWindowRect( &rect );
	m_InfoListCtrl.SetColumnWidth( 0, int(rect.Width()*0.5) );
	m_InfoListCtrl.SetColumnWidth( 1, int(rect.Width()*0.5) );

	stMonsterTreeInfo* pInfo = (stMonsterTreeInfo*)GetSelectedTreeInfoFromTree();
	if(0 == pInfo)
	{
		return;
	}

	switch( GetSelectedTreeInfoTypeFromTree() )
	{
	case eMonsterTreeInfoType_Group:
		{
			UpdateGroupInfo(
				pInfo->dwGroup);
		}
		break;
	case eMonsterTreeInfoType_Condition:
		{
			UpdateConditionInfo(
				*pInfo);
		}
		break;
	case eMonsterTreeInfoType_Monster:
		{
			UpdateMonsterInfo( pInfo->dwGroup, pInfo->hMonster );

			UpdateConditionInfo(
				*pInfo);

			// Tree에서 직접 선택한 경우 해당 몬스터에게로 카메라를 이동시킨다.
			if( m_bUseTreeSelectFunc )
			{
				CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
				if( !pFrm )				return;
				CRegenToolView* pView = (CRegenToolView*)pFrm->GetActiveView();
				if( !pView )				return;

				pView->SetCameraPos( pInfo->hMonster );
				pView->SetSelectedObjectHandle( pInfo->hMonster );
			}
		}
		break;
	}

	m_InfoListCtrl.ExpandColumnsToFit();
}

int CSheetPageMonster::UpdateGroupInfo(DWORD dwGroup)
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )				return 0;
	CRegenToolDoc* pDoc = (CRegenToolDoc*)pFrm->GetActiveDocument();
	if( !pDoc )				return 0;

	Script* const script = pDoc->GetScriptInfo(
		dwGroup);

	if(0 == script)
	{
		return 0;
	}

	LPCTSTR titleArray[] = {
		_T("이름"),
	};

	m_InfoListCtrl.SetRowCount(
		_countof(titleArray));
	m_InfoListCtrl.SetColumnCount(
		2);

	for(size_t i = 0; i < _countof(titleArray); ++i)
	{
		GV_ITEM item = {0};
		item.nState = GVIS_READONLY;
		item.strText = titleArray[i];
		item.crBkClr = MAKE_COLORREF(222, 222, 222, 0);
		item.row = i;
		item.mask = GVIF_TEXT | GVIF_STATE | GVIF_BKCLR;
		m_InfoListCtrl.SetItem(
			&item);

		item.col = 1;
		item.nState = GVIS_MODIFIED;
		item.crBkClr = MAKE_COLORREF(255, 255, 255, 0);

		switch(i)
		{
		case 0:
			{
				item.strText = script->mGroupName;
				break;
			}
		}

		m_InfoListCtrl.SetItem(
			&item);
		m_InfoListCtrl.SetRowHeight(
			item.row,
			GRID_ROW_DEFAULT_HEIGHT);
	}

	return m_InfoListCtrl.GetRowCount();
}

int CSheetPageMonster::UpdateConditionInfo(const stMonsterTreeInfo& treeInfo)
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )				return 0;
	CRegenToolDoc* pDoc = (CRegenToolDoc*)pFrm->GetActiveDocument();
	if( !pDoc )				return 0;

	const Script::Condition* const condition = pDoc->GetConditionInfo(
		treeInfo.dwGroup,
		treeInfo.dwConditionIdx);

	if(0 == condition)
	{
		return 0;
	}

	LPCTSTR titleArray[] = {
		_T("대상 그룹"),
		_T("생성 비율(%)"),
		_T("소요 시간(초)"),
		_T("생성 범위(m)"),
		_T("생성 여부"),
	};

	m_InfoListCtrl.SetRowCount(
		_countof(titleArray));
	m_InfoListCtrl.SetColumnCount(
		2);

	for(size_t i = 0; i < _countof(titleArray); ++i)
	{
		GV_ITEM item = {0};
		item.nState = GVIS_READONLY;
		item.strText = titleArray[i];
		item.crBkClr = MAKE_COLORREF(222, 222, 222, 0);
		item.row = i;
		item.mask = GVIF_TEXT | GVIF_STATE | GVIF_BKCLR;
		m_InfoListCtrl.SetItem(
			&item);

		item.col = 1;
		item.nState = GVIS_MODIFIED;
		item.crBkClr = MAKE_COLORREF(255, 255, 255, 0);

		switch(item.row)
		{
		case 0:
			{
				m_InfoListCtrl.SetCellType(
					item.row,
					1,
					RUNTIME_CLASS(CGridCellNumeric));

				item.strText.Format(
					_T("%u"),
					condition->mGroupIndex);
				break;
			}
		case 1:
			{
				m_InfoListCtrl.SetCellType(
					item.row,
					1,
					RUNTIME_CLASS(CGridCellNumeric));

				item.strText.Format(
					_T("%0.0f"),
					condition->mRatio * 100.0f);
				break;
			}
		case 2:
			{
				m_InfoListCtrl.SetCellType(
					item.row,
					1,
					RUNTIME_CLASS(CGridCellNumeric));

				item.strText.Format(
					_T("%.1f"),
					condition->mDelay / 1000.0f);
				break;
			}
		case 3:
			{
				m_InfoListCtrl.SetCellType(
					item.row,
					1,
					RUNTIME_CLASS(CGridCellNumeric));
				item.strText.Format(
					_T("%u"),
					condition->mRange);
				break;
			}
		case 4:
			{
				m_InfoListCtrl.SetCellType(
					item.row,
					1,
					RUNTIME_CLASS(CGridCellCheck));

				CGridCellCheck* const cellCheck = (CGridCellCheck*)m_InfoListCtrl.GetCell(
					item.row,
					1);
				cellCheck->SetCheck(
					condition->mIsRegen);

				item.strText.Empty();
				break;
			}
		}

		m_InfoListCtrl.SetItem(
			&item);
		m_InfoListCtrl.SetRowHeight(
			item.row,
			GRID_ROW_DEFAULT_HEIGHT);
	}

	return m_InfoListCtrl.GetRowCount();
}

int CSheetPageMonster::UpdateConditionInfo(DWORD dwGroup)
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )				return 0;
	CRegenToolDoc* pDoc = (CRegenToolDoc*)pFrm->GetActiveDocument();
	if( !pDoc )				return 0;

	Script* pScript = pDoc->GetScriptInfo( dwGroup );
	if( !pScript )			return 0;

	LPCTSTR titleArray[] = {
		_T("조건"),
		_T("대상 그룹"),
		_T("생성 비율(%)"),
		_T("소요 시간(초)"),
		_T("생성 범위(m)"),
		_T("생성 여부"),
	};

	m_InfoListCtrl.SetRowCount(
		_countof(titleArray) * pScript->mConditionList.size() + pScript->mConditionList.size());
	m_InfoListCtrl.SetColumnCount( 2 );

	int conditionCnt=0;
	for( Script::ConditionList::const_iterator condition = pScript->mConditionList.begin() ; condition != pScript->mConditionList.end() ; ++condition, ++conditionCnt )
	{
		for(size_t i = conditionCnt * _countof(titleArray) + conditionCnt ; i < (conditionCnt+1) * _countof(titleArray) + conditionCnt ; ++i )
		{
			GV_ITEM item = {0};
			item.nState = GVIS_READONLY;
			item.strText = titleArray[ i - conditionCnt * _countof(titleArray) - conditionCnt ];
			item.crBkClr = MAKE_COLORREF(222, 222, 222, 0);
			item.row = i;
			item.mask = GVIF_TEXT | GVIF_STATE | GVIF_BKCLR;
			m_InfoListCtrl.SetItem(
				&item);

			item.col = 1;
			item.nState = GVIS_MODIFIED;
			item.crBkClr = MAKE_COLORREF(255, 255, 255, 0);

			switch( item.row - ( conditionCnt * _countof(titleArray) ) - conditionCnt )
			{
			case 0:
				{
					item.strText.Format(
						_T("%u"),
						conditionCnt + 1 );
					break;
				}
			case 1:
				{
					m_InfoListCtrl.SetCellType(
						item.row,
						1,
						RUNTIME_CLASS(CGridCellNumeric));

					item.strText.Format(
						_T("%u"),
						condition->mGroupIndex);
					break;
				}
			case 2:
				{
					m_InfoListCtrl.SetCellType(
						item.row,
						1,
						RUNTIME_CLASS(CGridCellNumeric));

					item.strText.Format(
						_T("%0.0f"),
						condition->mRatio * 100.0f);
					break;
				}
			case 3:
				{
					m_InfoListCtrl.SetCellType(
						item.row,
						1,
						RUNTIME_CLASS(CGridCellNumeric));

					item.strText.Format(
						_T("%.1f"),
						condition->mDelay / 1000.0f);
					break;
				}
			case 4:
				{
					m_InfoListCtrl.SetCellType(
						item.row,
						1,
						RUNTIME_CLASS(CGridCellNumeric));
					item.strText.Format(
						_T("%u"),
						condition->mRange);
					break;
				}
			case 5:
				{
					m_InfoListCtrl.SetCellType(
						item.row,
						1,
						RUNTIME_CLASS(CGridCellCheck));

					CGridCellCheck* const cellCheck = (CGridCellCheck*)m_InfoListCtrl.GetCell(
						item.row,
						1);
					cellCheck->SetCheck(
						condition->mIsRegen);

					item.strText.Empty();
					break;
				}
			}

			m_InfoListCtrl.SetItem(
				&item);
			m_InfoListCtrl.SetRowHeight(
				item.row,
				GRID_ROW_DEFAULT_HEIGHT);
		}
	}

	return m_InfoListCtrl.GetRowCount();
}

int CSheetPageMonster::UpdateMonsterInfo( DWORD dwGroup, GXOBJECT_HANDLE handle )
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )				return 0;
	CRegenToolDoc* pDoc = (CRegenToolDoc*)pFrm->GetActiveDocument();
	if( !pDoc )				return 0;
	HTREEITEM hItem = GetTreeItemFromGroup( dwGroup );
	if( hItem == NULL )		return 0;

	DWORD dwIndex = (DWORD)m_MonsterTree.GetItemData( hItem );
	std::map< DWORD, stMonsterTreeInfo >::const_iterator iter = m_mapMonsterTreeInfo.find( dwIndex );
	if( iter == m_mapMonsterTreeInfo.end() )		return 0;

	Script* pScript = pDoc->GetScriptInfo( dwGroup );
	if( !pScript )			return 0;
	Script::Monster* pMonster = pDoc->GetMonsterInfo( handle );
	if( !pMonster )			return 0;

	LPCTSTR titleArray[] = {
		_T("종류"),
		_T("위치 x"),
		_T("위치 z"),
		_T("인공지능"),
	};

	int nRow = UpdateConditionInfo( dwGroup );

	m_InfoListCtrl.SetRowCount(
		++nRow + _countof(titleArray));
	m_InfoListCtrl.SetColumnCount(
		2);

	for(size_t i = nRow; i < _countof(titleArray) + nRow; ++i)
	{
		GV_ITEM item = {0};
		item.nState = GVIS_READONLY;
		item.strText = titleArray[ i - nRow ];
		item.crBkClr = MAKE_COLORREF(222, 222, 222, 0);
		item.row = i;
		item.mask = GVIF_TEXT | GVIF_STATE | GVIF_BKCLR;
		m_InfoListCtrl.SetItem(
			&item);

		item.col = 1;
		item.crBkClr = MAKE_COLORREF(255, 255, 255, 0);

		switch(item.row - nRow)
		{
		case 0:
			{
				item.nState = GVIS_MODIFIED;
				
				m_InfoListCtrl.SetCellType(
					item.row,
					1,
					RUNTIME_CLASS(CGridCellCombo));
				
				CStringArray stringArray;
				stringArray.Add(
					_T("<없음>"));
				stringArray.Add(
					_T("몬스터"));
				stringArray.Add(
					_T("보스 몬스터"));
				stringArray.Add(
					_T("필드 보스 몬스터"));

				const BASE_MONSTER_LIST& monsterInfo = g_pLoadList.GetMonster(
					pMonster->mMonsterKind);

				// Object.h 참조
				switch(monsterInfo.ObjectKind)
				{
				case 32:
					{
						item.strText = stringArray.GetAt(
							1);
						break;
					}
				case 33:
					{
						item.strText = stringArray.GetAt(
							2);
						break;
					}
				case 35:
					{
						item.strText = stringArray.GetAt(
							3);
						break;
					}
				default:
					{
						item.strText = stringArray.GetAt(
							0);
						break;
					}
				}

				CGridCellCombo* const cellCombo = (CGridCellCombo*)m_InfoListCtrl.GetCell(
					item.row,
					1);
				cellCombo->SetOptions(
					stringArray);
				cellCombo->SetStyle(
					CBS_DROPDOWN);
				break;
			}
		case 1:
			{
				m_InfoListCtrl.SetCellType(
					item.row,
					1,
					RUNTIME_CLASS(CGridCellNumeric));

				item.strText.Format(
					_T("%0.0f"),
					pMonster->mPosition.x);
				break;
			}
		case 2:
			{
				m_InfoListCtrl.SetCellType(
					item.row,
					1,
					RUNTIME_CLASS(CGridCellNumeric));

				item.strText.Format(
					_T("%0.0f"),
					pMonster->mPosition.z);
				break;
			}
		case 3:
			{
				item.nState = GVIS_MODIFIED;

				item.strText = pMonster->mFiniteStateMachine;
				m_InfoListCtrl.SetItem(
					&item);
				m_InfoListCtrl.SetRowHeight(
					item.row,
					GRID_ROW_DEFAULT_HEIGHT);

				break;
			}
		}

		m_InfoListCtrl.SetItem(
			&item);
		m_InfoListCtrl.SetRowHeight(
			item.row,
			GRID_ROW_DEFAULT_HEIGHT);
	}

	return m_InfoListCtrl.GetRowCount();
}

void CSheetPageMonster::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	CRect totalRect( 5, 5, cx-10, cy-10 );
	CRect rect( totalRect.left, totalRect.top, totalRect.right, int(totalRect.Height() * 0.40) );
	if( m_MonsterTree.GetSafeHwnd() )
		m_MonsterTree.MoveWindow( &rect );


	// Script View Btn
	rect.SetRect( int(totalRect.Width()*0.2), int(totalRect.Height() * 0.40), int(totalRect.Width()*0.6), int(totalRect.Height() * 0.45) );
	if( m_ScriptViewBtn.GetSafeHwnd() )
		m_ScriptViewBtn.MoveWindow( &rect );
	// AddGroup Btn
	rect.SetRect( int(totalRect.Width()*0.6), int(totalRect.Height() * 0.40), totalRect.right, int(totalRect.Height() * 0.45) );
	if( m_AddGroupBtn.GetSafeHwnd() )
		m_AddGroupBtn.MoveWindow( &rect );


	// Monster List
	rect.SetRect( totalRect.left, int(totalRect.Height() * 0.475), int(totalRect.Width()*0.4), int(totalRect.Height() * 0.50) );
	if( m_MonsterListStatic.GetSafeHwnd() )
		m_MonsterListStatic.MoveWindow( &rect );
	rect.SetRect( int(totalRect.Width()*0.4), int(totalRect.Height() * 0.47), totalRect.right, totalRect.bottom );
	if( m_MonsterListCombo.GetSafeHwnd() )
		m_MonsterListCombo.MoveWindow( &rect );

	// Info ListCtrl
	rect.SetRect( totalRect.left, int(totalRect.Height() * 0.5), totalRect.right, int(totalRect.Height() * 0.95) );
	CWnd *pHolder = GetDlgItem(IDC_LISTCTRL_INFO);
	if( pHolder && pHolder->GetSafeHwnd() )
		pHolder->MoveWindow( &rect );
	rect.SetRect( totalRect.left, int(totalRect.Height() * 0.5), totalRect.right, int(totalRect.Height() * 0.95) );
	if( m_InfoListCtrl.GetSafeHwnd() )
		m_InfoListCtrl.MoveWindow( &rect );

	rect.SetRect( totalRect.left, int(totalRect.Height() * 0.95), totalRect.right, totalRect.bottom );
	if( m_ModifyInfoBtn.GetSafeHwnd() )
		m_ModifyInfoBtn.MoveWindow( &rect );
	


	// Select() 를 이용하여 선택한경우 TVN_SELCHANGED 가 발생시 구분할수 있도록 변수를 설정한다.
	m_bUseTreeSelectFunc = FALSE;
	UpdateSelectedInfo();
}

BOOL CSheetPageMonster::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	TVINSERTSTRUCT tvi;
	ZeroMemory(&tvi, sizeof(TVINSERTSTRUCT));
	tvi.hParent = TVI_ROOT;
	tvi.hInsertAfter = TVI_LAST;
	tvi.item.pszText = _T("Monster");
	tvi.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM;
	tvi.item.cChildren = 1;
	tvi.item.lParam = 0;

	m_hTopItem = m_MonsterTree.InsertItem(&tvi);

	// GridCtrl
	RECT rect;
	CWnd *pHolder = GetDlgItem(IDC_LISTCTRL_INFO);
	if( !pHolder )
		return TRUE;

	pHolder->GetWindowRect( &rect );

	m_InfoListCtrl.Create( rect, this, IDC_LISTCTRL_INFO, AFX_WS_DEFAULT_VIEW );
	m_InfoListCtrl.SetEditable( TRUE );
	m_InfoListCtrl.SetListMode( FALSE );
	m_InfoListCtrl.EnableDragAndDrop( FALSE );
	m_InfoListCtrl.ModifyStyle( WS_VSCROLL, 0 );

	LOGFONT	logfont = {0};
	logfont.lfHeight = 12;
	logfont.lfWeight = FW_DONTCARE;
	logfont.lfCharSet = DEFAULT_CHARSET;
	logfont.lfQuality = PROOF_QUALITY;
	lstrcpy(logfont.lfFaceName,"굴림");
	CFont font;
	font.CreateFontIndirect( &logfont );

	m_InfoListCtrl.SetFont( &font );


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSheetPageMonster::OnBnClickedListviewBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )		return;
	CRegenToolDoc* pDoc = (CRegenToolDoc*)pFrm->GetActiveDocument();
	if( !pDoc )		return;

	CScriptViewDlg dlg;
	pDoc->DataToScript( dlg.m_strScriptEdit );
	dlg.DoModal();
	
}

void CSheetPageMonster::OnBnClickedButtonDefaultSetting()
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )
		return;

	CRegenToolDoc* pDoc = (CRegenToolDoc*)pFrm->GetActiveDocument();
	if( !pDoc )
		return;

	CGroupInfoInputDlg groupInfoDlg;
	groupInfoDlg.SetTitleName( _T("기본값 설정") );
	DWORD dwAddGroupIdx = pDoc->GetUsableGroupIndex();

	CString str;
	str = m_strDefaultGroupName + _T("%d");
	str.Format( str, dwAddGroupIdx );
	groupInfoDlg.SetGroupName( str );
	groupInfoDlg.SetTargetGroup( dwAddGroupIdx );

	groupInfoDlg.SetTargetGroup( m_DefaultCondition.mGroupIndex );
	groupInfoDlg.SetRatio( m_DefaultCondition.mRatio );
	groupInfoDlg.SetDelay( m_DefaultCondition.mDelay );
	groupInfoDlg.SetIsRegen( m_DefaultCondition.mIsRegen );
	groupInfoDlg.SetRegenRange( m_DefaultCondition.mRange );

	if( groupInfoDlg.DoModal() == IDOK )
	{
		// Group Name 
		m_strDefaultGroupName = groupInfoDlg.GetGroupName();

		// Condition 정보 입력
		m_DefaultCondition.mGroupIndex	= groupInfoDlg.GetTargetGroup();
		m_DefaultCondition.mRatio		= groupInfoDlg.GetRatio();
		m_DefaultCondition.mDelay		= groupInfoDlg.GetDelay();
		m_DefaultCondition.mIsRegen		= groupInfoDlg.GetIsRegen();
		m_DefaultCondition.mRange		= groupInfoDlg.GetRegenRange();
	}
}

void CSheetPageMonster::OnBnClickedButtonModifyInfo()
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )				return;
	CRegenToolDoc* pDoc = (CRegenToolDoc*)pFrm->GetActiveDocument();
	if( !pDoc )				return;
	stMonsterTreeInfo* pInfo = (stMonsterTreeInfo*)GetSelectedTreeInfoFromTree();
	if( !pInfo )			return;
	Script* pScript = pDoc->GetScriptInfo( pInfo->dwGroup );
	if( !pScript )			return;

	switch( GetSelectedTreeInfoTypeFromTree() )
	{
	case eMonsterTreeInfoType_Group:
		{
			pScript->mGroupName = m_InfoListCtrl.GetItemText(0, 1);
			break;
		}
	case eMonsterTreeInfoType_Monster:
		{
			Script::Monster* const monster = pDoc->GetMonsterInfo(
				pInfo->hMonster);

			if(0 == monster)
			{
				break;
			}

			CString objectKind = m_InfoListCtrl.GetItemText(
				1,
				1);
			
			if("몬스터" == objectKind)
			{
				monster->mObjectKind = eObjectKind_Monster;
			}
			else if("보스 몬스터" == objectKind)
			{
				monster->mObjectKind = eObjectKind_BossMonster;
			}
			else if("필드 몬스터" == objectKind)
			{
				monster->mObjectKind = eObjectKind_FieldBossMonster;
			}

			monster->mFiniteStateMachine = m_InfoListCtrl.GetItemText(
				5 < m_InfoListCtrl.GetRowCount() ? 11 : 4,
				1);
			break;
		}
	case eMonsterTreeInfoType_Condition:
		{
			Script::Condition* const condition = pDoc->GetConditionInfo(
				pInfo->dwGroup,
				pInfo->dwConditionIdx);

			if(0 == condition)
			{
				break;
			}

			condition->mGroupIndex = _ttoi(
				m_InfoListCtrl.GetItemText(0, 1));
			condition->mRatio = float(_tstof(
				m_InfoListCtrl.GetItemText(1, 1))) / 100.0f;
			condition->mDelay = _ttoi(
				m_InfoListCtrl.GetItemText(2, 1)) * 1000;
			condition->mRange = _ttoi(
				m_InfoListCtrl.GetItemText(3, 1));

			CGridCellCheck* const cellCheck = (CGridCellCheck*)m_InfoListCtrl.GetCell(
				4, 1);

			if(0 == cellCheck)
			{
				break;
			}
			else if(FALSE == cellCheck->IsKindOf(RUNTIME_CLASS(CGridCellCheck)))
			{
				return;
			}

			condition->mIsRegen = cellCheck->GetCheck();
			break;
		}
	}
}

HTREEITEM CSheetPageMonster::AddGroupInTree( DWORD dwGroup )
{
	if( GetTreeItemFromGroup( dwGroup ) != NULL )
		return NULL;

	CString str;
	str.Format( "[그룹 %u]", dwGroup );
	stMonsterTreeInfo treeInfo;
	treeInfo.eType = eMonsterTreeInfoType_Group;
	treeInfo.dwGroup = dwGroup;
	treeInfo.dwIndex = GetHashCodeFromText( str );

	m_mapMonsterTreeInfo.insert( std::make_pair<int, stMonsterTreeInfo>( treeInfo.dwIndex, treeInfo ) );
	return AddTreeItem( str, m_hTopItem, treeInfo.dwIndex, FALSE );
}

HTREEITEM CSheetPageMonster::AddConditionInTree( DWORD dwGroup, DWORD dwConditionIdx )
{
	HTREEITEM hParent = GetTreeItemFromGroup( dwGroup );
	if( hParent == NULL )
		return NULL;

	CString str;
	stMonsterTreeInfo treeInfo;
	treeInfo.eType = eMonsterTreeInfoType_Condition;
	treeInfo.dwGroup = dwGroup;
	treeInfo.dwConditionIdx = dwConditionIdx;
	str.Format( "Group%d_Condition%d", dwGroup, dwConditionIdx );
	treeInfo.dwIndex = GetHashCodeFromText( str );
	m_mapMonsterTreeInfo.insert( std::make_pair<int, stMonsterTreeInfo>( treeInfo.dwIndex, treeInfo ) );
	
	return AddTreeItem(
		_T("[조건]"),
		hParent,
		(LPARAM)treeInfo.dwIndex,
		FALSE);
}

HTREEITEM CSheetPageMonster::AddMonsterInTree( DWORD dwGroup, LPCTSTR name, GXOBJECT_HANDLE handle )
{
	HTREEITEM hParent = GetTreeItemFromGroup( dwGroup );
	if( hParent == NULL )
		return NULL;

	stMonsterTreeInfo treeInfo;
	treeInfo.eType = eMonsterTreeInfoType_Monster;
	treeInfo.dwGroup = dwGroup;
	treeInfo.dwIndex = (DWORD)handle;
	treeInfo.hMonster = handle;
	m_mapMonsterTreeInfo.insert( std::make_pair<int, stMonsterTreeInfo>( treeInfo.dwIndex, treeInfo ) );
	return AddTreeItem( name, hParent, (LPARAM)treeInfo.dwIndex, FALSE );
}

BOOL CSheetPageMonster::DeleteGroupInTree( DWORD dwGroup )
{
	HTREEITEM hTreeItem = GetTreeItemFromGroup( dwGroup );
	if( hTreeItem == NULL )
		return FALSE;

	DWORD dwIndex = (DWORD)m_MonsterTree.GetItemData( hTreeItem );
	std::map< DWORD, stMonsterTreeInfo >::iterator iter = m_mapMonsterTreeInfo.find( dwIndex );
	if( iter == m_mapMonsterTreeInfo.end() )
		return FALSE;

	m_mapMonsterTreeInfo.erase( iter );

	m_MonsterTree.DeleteItem( hTreeItem );

	return TRUE;
}

BOOL CSheetPageMonster::DeleteConditionInTree( DWORD dwGroup, DWORD dwConditionIdx )
{
	HTREEITEM hTreeItem = GetTreeItemFromCondition( dwGroup, dwConditionIdx );
	if( hTreeItem == NULL )
		return FALSE;

	DWORD dwIndex = (DWORD)m_MonsterTree.GetItemData( hTreeItem );
	std::map< DWORD, stMonsterTreeInfo >::iterator iter = m_mapMonsterTreeInfo.find( dwIndex );
	if( iter == m_mapMonsterTreeInfo.end() )
		return FALSE;

	m_mapMonsterTreeInfo.erase( iter );

	m_MonsterTree.DeleteItem( hTreeItem );

	return TRUE;
}

BOOL CSheetPageMonster::DeleteMonsterInTree( GXOBJECT_HANDLE handle )
{
	HTREEITEM hTreeItem = GetTreeItemFromMonsterHandle( handle );
	if( hTreeItem == NULL )
		return FALSE;

	DWORD dwIndex = (DWORD)m_MonsterTree.GetItemData( hTreeItem );
	std::map< DWORD, stMonsterTreeInfo >::iterator iter = m_mapMonsterTreeInfo.find( dwIndex );
	if( iter == m_mapMonsterTreeInfo.end() )
		return FALSE;

	m_mapMonsterTreeInfo.erase( iter );

	m_MonsterTree.DeleteItem( hTreeItem );

	return TRUE;
}
void CSheetPageMonster::OnTvnSelchangedTreeMonterview(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateSelectedInfo();	

	m_bUseTreeSelectFunc = TRUE;
}

void CSheetPageMonster::OnNMRclickTreeMonterview(NMHDR *pNMHDR, LRESULT *pResult)
{
	TV_HITTESTINFO hit_info;
	POINT cursorPoint;

	// 화면상 마우스 위치
	::GetCursorPos( &cursorPoint );

	hit_info.pt = cursorPoint;

	// 트리컨트롤 기준의 좌표로 변경
	::ScreenToClient( m_MonsterTree.m_hWnd, &hit_info.pt );

	// 마우스 위치의 TREEITEM 얻기
	HTREEITEM hItem = m_MonsterTree.HitTest( &hit_info );
	if( hItem == NULL )
		return;

	// 선택
	m_MonsterTree.Select( hItem, TVGN_CARET );

	// Popup Menu
	CMenu menu;
	menu.LoadMenu( IDR_MENU_MONSTER_TREE );
	CMenu* const subMenu = menu.GetSubMenu(0);
	if(0 == subMenu)
		return;

	switch( GetTreeInfoType( hItem ) )
	{
	case eMonsterTreeInfoType_Root:
		{
			subMenu->EnableMenuItem( IDR_MENU_MONSTER_TREE_DELETE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
			subMenu->EnableMenuItem( IDR_MENU_MONSTER_TREE_ADD_CONDITON, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		}
		break;

	case eMonsterTreeInfoType_Group:
		{
		}
		break;

	case eMonsterTreeInfoType_Condition:
	case eMonsterTreeInfoType_Monster:
		{
			subMenu->EnableMenuItem( IDR_MENU_MONSTER_TREE_ADD_GROUP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
			subMenu->EnableMenuItem( IDR_MENU_MONSTER_TREE_ADD_CONDITON, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		}
		break;

	default:
		return;
	}

	subMenu->TrackPopupMenu(
		TPM_RIGHTALIGN | TPM_RIGHTBUTTON,
		cursorPoint.x,
		cursorPoint.y,
		this);
	menu.DestroyMenu();
}

void CSheetPageMonster::OnMenuMonsterTreeAddGroup()
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )		return;
	CRegenToolDoc* pDoc = (CRegenToolDoc*)pFrm->GetActiveDocument();
	if( !pDoc )		return;

	CGroupInfoInputDlg groupInfoDlg;
	groupInfoDlg.SetTitleName( _T("그룹 추가") );
	DWORD dwAddGroupIdx = pDoc->GetUsableGroupIndex();

	CString str;
	str.Format( _T("일반그룹_%d"), dwAddGroupIdx );
	groupInfoDlg.SetGroupName( str );
	groupInfoDlg.SetTargetGroup( dwAddGroupIdx );

	groupInfoDlg.SetReadyOnlyTargetGroup( TRUE );
	groupInfoDlg.SetReadyOnlyRatio( TRUE );
	groupInfoDlg.SetReadyOnlyDelay( TRUE );
	groupInfoDlg.SetReadyOnlyIsRegen( TRUE );
	groupInfoDlg.SetReadyOnlyRegenRange( TRUE );

	if( groupInfoDlg.DoModal() == IDOK )
	{
		Script* pScript = pDoc->AddGroup();
		if( pScript == NULL )
			return;

		pScript->mGroupName = groupInfoDlg.GetGroupName();

		HTREEITEM hTreeItem = AddGroupInTree( pScript->mGroupIndex );
		SelectTreeItem( hTreeItem );

		UpdateSelectedInfo();
	}
}

void CSheetPageMonster::OnMenuMonsterTreeAddConditon()
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )		return;
	CRegenToolDoc* pDoc = (CRegenToolDoc*)pFrm->GetActiveDocument();
	if( !pDoc )		return;


	DWORD dwSelectedGroup = GetSelectedGroupIndexFromTree();
	Script* pScript = pDoc->GetScriptInfo( dwSelectedGroup );
	if( pScript == NULL )
		return;

	CString str = pScript->mGroupName;
	CGroupInfoInputDlg groupInfoDlg;
	groupInfoDlg.SetTitleName( _T("Condition 추가") );
	groupInfoDlg.SetGroupName( str );

	groupInfoDlg.SetReadyOnlyGroupName( TRUE );

	if( groupInfoDlg.DoModal() == IDOK )
	{
		// Condition 정보 입력
		Script::Condition* pCondition = pDoc->AddCondition( dwSelectedGroup );
		if( !pCondition )
			return;

		pCondition->mGroupIndex	= groupInfoDlg.GetTargetGroup();
		pCondition->mRatio		= groupInfoDlg.GetRatio();
		pCondition->mDelay		= groupInfoDlg.GetDelay();
		pCondition->mIsRegen	= groupInfoDlg.GetIsRegen();
		pCondition->mRange		= groupInfoDlg.GetRegenRange();

		HTREEITEM hTreeItem = AddConditionInTree( pScript->mGroupIndex, pCondition->dwConditionIdx );
		SelectTreeItem( hTreeItem );

		UpdateSelectedInfo();
	}
}

void CSheetPageMonster::OnMenuMonsterTreeDelete()
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )		return;
	CRegenToolDoc* pDoc = (CRegenToolDoc*)pFrm->GetActiveDocument();
	if( !pDoc )		return;
	CRegenToolView* pView = (CRegenToolView*)pFrm->GetActiveView();
	if( !pView )		return;

	switch( GetSelectedTreeInfoTypeFromTree() )
	{
	case eMonsterTreeInfoType_Group:
		{
			const DWORD dwGroup = GetSelectedGroupIndexFromTree();
			// Group 제거
			if( !pDoc->DeleteGroup( dwGroup ) )
				return;

			DeleteGroupInTree( dwGroup );
		}
		break;

	case eMonsterTreeInfoType_Condition:
		{
			stMonsterTreeInfo* pTreeInfo = (stMonsterTreeInfo*)GetSelectedTreeInfoFromTree();
			if( !pTreeInfo )
				return;

			// Condition 제거
			if( !pDoc->DeleteCondition( pTreeInfo->dwGroup, pTreeInfo->dwConditionIdx ) )
				return;

			DeleteConditionInTree( pTreeInfo->dwGroup, pTreeInfo->dwConditionIdx );
		}
		break;

	case eMonsterTreeInfoType_Monster:
		{
			stMonsterTreeInfo* pTreeInfo = (stMonsterTreeInfo*)GetSelectedTreeInfoFromTree();
			if( !pTreeInfo )
				return;

			// 몬스터 제거
			if( !pDoc->DeleteMonster( pTreeInfo->hMonster ) )
				return;
			
			if( pView->GetSelectedObjectHandle() == pTreeInfo->hMonster )
				pView->SetSelectedObjectHandle( NULL );
			g_pExecutive->DeleteGXObject( pTreeInfo->hMonster );
			DeleteMonsterInTree( pTreeInfo->hMonster );
		}
		break;

	default:
		return;
	}
}

BOOL CSheetPageMonster::PreTranslateMessage(MSG* pMsg)
{	
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			switch( pMsg->wParam )
			{
			// Tree에서 찾기
			case 'f':
			case 'F':
				{
					BOOL bIsPressCtrl = GetAsyncKeyState( VK_CONTROL );
					if( !bIsPressCtrl )
						break;

					ShowFindDialog();
				}
				break;
			}
		}
		break;
	}
	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CSheetPageMonster::ShowFindDialog()
{
	if( m_pFindDialog )
	{
		m_pFindDialog->SetFocus();
		return;
	}

	m_pFindDialog = new CFindReplaceDialog;
	m_pFindDialog->Create( TRUE, m_strFind, NULL, FR_DOWN, this );
	m_pFindDialog->ShowWindow( SW_SHOW );
}

LONG CSheetPageMonster::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	CFindReplaceDialog *pDlg = CFindReplaceDialog::GetNotifier(lParam);
	if( !pDlg || m_pFindDialog != pDlg )
		return 0;

	m_pFindDialog = NULL;
	m_strFind = pDlg->GetFindString();

	if( pDlg->IsTerminating() )
		return 0;

	HTREEITEM hItem = m_MonsterTree.GetSelectedItem();
	// 지정된 아이템의 다음부터 찾기
	const BOOL bNextItem = TRUE;
	// 아래로 검색
	const BOOL bSearchDown = pDlg->SearchDown();

	hItem = GetTreeItemFromItemText( m_strFind, hItem, bSearchDown, bNextItem );
	// NULL 인 경우 현재 Group의 다음 형제부터 검사
	if( hItem == NULL )
	{
		hItem = m_MonsterTree.GetSelectedItem();
		if( hItem == NULL )
			return 0;

		DWORD dwItemData = DWORD( m_MonsterTree.GetItemData( hItem ) );
		DWORD dwGroup = GetGroupIndexFromTreeItemData( dwItemData );
		hItem = GetTreeItemFromGroup( dwGroup );

		if( bSearchDown )
			hItem = m_MonsterTree.GetNextSiblingItem( hItem );
		else
			hItem = m_MonsterTree.GetPrevSiblingItem( hItem );

		hItem = GetTreeItemFromItemText( m_strFind, hItem, bSearchDown );
	}

	m_MonsterTree.Select( hItem, TVGN_CARET );

	return 0;
}