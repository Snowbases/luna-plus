#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "RegenToolDoc.h"

int CALLBACK MonsterTreeCompareProc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );


// CSheetPageMonster 대화 상자입니다.

#define GRID_EMPTYROW_HEIGHT		5
#define GRID_ROW_DEFAULT_HEIGHT		22

class CGridCtrl;

class CSheetPageMonster : public CPropertyPage
{
	DECLARE_DYNAMIC(CSheetPageMonster)

public:
	CSheetPageMonster();
	virtual ~CSheetPageMonster();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_SHEET_PAGE_MONSTER };

protected:
	enum eMonsterTreeInfoType{
		eMonsterTreeInfoType_None,
		eMonsterTreeInfoType_Root,
		eMonsterTreeInfoType_Group,
		eMonsterTreeInfoType_Condition,
		eMonsterTreeInfoType_Monster,
	};

	struct stMonsterTreeInfo{
		eMonsterTreeInfoType eType;
		DWORD dwIndex;
		DWORD dwGroup;
		DWORD dwConditionIdx;
		GXOBJECT_HANDLE hMonster;

		stMonsterTreeInfo()
		{
			dwIndex = 0;
			dwGroup = 0;
			dwConditionIdx = 9999;
			hMonster = 0;
		}
	};

	std::map< DWORD, stMonsterTreeInfo > m_mapMonsterTreeInfo;

	HTREEITEM m_hTopItem;
	BOOL m_bUseTreeSelectFunc;

	// Group/Condition 기본값
	CString m_strDefaultGroupName;
	Script::Condition m_DefaultCondition;
	// 찾기
	CFindReplaceDialog* m_pFindDialog;
	CString m_strFind;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	CTreeCtrl m_MonsterTree;
	CButton m_ScriptViewBtn;
	CButton m_AddGroupBtn;
	CGridCtrl m_InfoListCtrl;
	CStatic m_MonsterListStatic;
	CComboBox m_MonsterListCombo;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();

	// Tree Control
	HTREEITEM AddTreeItem( LPCTSTR name, HTREEITEM hParent, LPARAM lParam=0, BOOL bHasChild=TRUE );
	HTREEITEM GetTreeItemFromItemText( const CString& str, HTREEITEM hItem=NULL, BOOL bSearchDown=TRUE, BOOL bNextItem=FALSE );
	HTREEITEM GetTreeItemFromGroup( DWORD dwGroup, HTREEITEM hItem=NULL );
	HTREEITEM GetTreeItemFromCondition( DWORD dwGroup, DWORD dwCondition, HTREEITEM hItem=NULL );
	HTREEITEM GetTreeItemFromMonsterHandle( GXOBJECT_HANDLE handle, HTREEITEM hItem=NULL );
	void* GetTreeInfo( HTREEITEM hItem );
	int GetTreeInfoType( HTREEITEM hItem );
	int GetSelectedTreeInfoTypeFromTree();
	void* GetSelectedTreeInfoFromTree();
	DWORD GetGroupNumFromMonsterHandle( GXOBJECT_HANDLE handle );
public:
	DWORD GetGroupIndexFromTreeItemData( DWORD dwItemData );
	DWORD GetConditionIndexFromTreeItemData( DWORD dwItemData );
	void SelectMonster( GXOBJECT_HANDLE handle );
	void SelectTreeItem( HTREEITEM hItem, BOOL bUseTreeSelectFuc=TRUE );

	// ListCtrl
	void SetUseTreeSelectFunc( BOOL bUse ) { m_bUseTreeSelectFunc = bUse; }
	void UpdateSelectedInfo();
	int UpdateGroupInfo( DWORD dwGroup );
	int UpdateConditionInfo(const stMonsterTreeInfo&);
	int UpdateConditionInfo(DWORD dwGroup);
	int UpdateMonsterInfo( DWORD dwGroup, GXOBJECT_HANDLE handle );

public:
	void ResetTree();
	void ExpandTree();
	HTREEITEM AddGroupInTree( DWORD dwGroup );
	HTREEITEM AddConditionInTree( DWORD dwGroup, DWORD dwConditionIdx );
	HTREEITEM AddMonsterInTree( DWORD dwGroup, LPCTSTR name, GXOBJECT_HANDLE handle );
	BOOL DeleteGroupInTree( DWORD dwGroup );
	BOOL DeleteConditionInTree( DWORD dwGroup, DWORD dwConditionIdx );
	BOOL DeleteMonsterInTree( GXOBJECT_HANDLE handle );
	DWORD GetSelectedGroupIndexFromTree();

	void ShowFindDialog();

	afx_msg void OnTvnSelchangedTreeMonterview(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox& GetMonsterListComboBox() { return m_MonsterListCombo; }
	afx_msg void OnBnClickedListviewBtn();
	afx_msg void OnNMRclickTreeMonterview(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuMonsterTreeAddGroup();
	afx_msg void OnMenuMonsterTreeAddConditon();
	afx_msg void OnMenuMonsterTreeDelete();
	afx_msg void OnBnClickedButtonDefaultSetting();
	afx_msg void OnBnClickedButtonModifyInfo();
	CButton m_ModifyInfoBtn;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LONG OnFindReplace(WPARAM wParam, LPARAM lParam);
};
