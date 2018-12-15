// LunaGameDataExtracterDlg.h : 헤더 파일
//

#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "TooltipMsgConverter.h"

UINT WINAPI GameDataLoadThread(LPVOID lpVoid);
UINT WINAPI ExtractGameDataThread(LPVOID lpVoid);
DWORD _stdcall AfterRenderFunc();

class CSimpleTextListCtrl;

// CLunaGameDataExtracterDlg 대화 상자
class CLunaGameDataExtracterDlg : public CDialog
{
	CxImage			m_BackgroundImg;

	HMODULE m_hExecutiveHandle;
	I4DyuchiGXExecutive* m_pExecutive;
	GXOBJECT_HANDLE m_AddedGXObjectHandel;
	CFont m_Font;
	IDIFontObject* m_pFontObject;

	CString m_strExecutedPath;
	CString m_strLunaDataPath;

	LPCTSTR SETTING_LUA_SCRIPT_FILENAME;

	BOOL m_bLoadCompleted;
	BOOL m_bExtracting;

	HANDLE m_hLodingThread;
	HANDLE m_ExtractingThread;

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	// Game Data
	/////////////////////////////////////////////////////////////////////////////
	enum EObjectKind
	{
		// 080616 LUJ, 초기화용 값 추가
		eObjectKind_None,
		eObjectKind_Player = 1,
		eObjectKind_Npc		=	2,
		eObjectKind_Item	=	4,
		eObjectKind_Tactic	=	8,
		eObjectKind_SkillObject=16,
		eObjectKind_Monster	=	32,
		eObjectKind_BossMonster=33,
		eObjectKind_SpecialMonster=34,
		// 필드보스 - 05.12 이영준
		eObjectKind_FieldBossMonster=35,
		eObjectKind_FieldSubMonster=36,
		eObjectKind_ToghterPlayMonster=37,
		// 080616 LUJ, 함정 추가
		eObjectKind_Trap = 39,
		// 090422 ShinJS --- 탈것 추가(생성시 Monster로 생성, ObjectKind로 탈것 구분하기때문에 리소스와 서버의 eObjectKind_Vehicle를 맞추어야함)
		eObjectKind_Vehicle = 40,
		eObjectKind_Pet = 64,
		eObjectKind_MapObject = 128,
		// S 농장시스템 추가 added by hseos 2007.05.07
		eObjectKind_FarmObj= 130,
		// E 농장시스템 추가 added by hseos 2007.05.07

		// 090527 pdy 하우징
		eObjectKind_Furniture = 131,
	};

	struct stImagePathInfo{
		DWORD index;
		std::string path;
		LONG width;
		LONG height;

		stImagePathInfo()
		{
			index = 0;
			width = height = 0;
		}
	};

	struct stImageInfo{
		DWORD index;
		DWORD imagePathInfoIdx;
        RECT rect;

		stImageInfo()
		{
			index = imagePathInfoIdx = 0;
			SetRect( &rect, 0, 0, 0, 0 );
		}
	};

	struct stQuestItenInfo{
		DWORD index;
		std::string name;
		DWORD questindex;
		DWORD subquestindex;
		std::string itemdesc;
	};


	std::map< DWORD, std::string >			m_SysteMsgList;
	std::map< DWORD, std::string >			m_InterfaceMsgList;

	CTooltipMsgConverter					m_TooltipConverter;
	std::map< DWORD, std::string >			m_TooltipMsgList;

	std::map< DWORD, stImagePathInfo >		m_ImagePathInfoList;

	typedef std::map< DWORD, stImageInfo > IMAGEINFOMAP;
	IMAGEINFOMAP							m_ItemIconImageInfoList;
	IMAGEINFOMAP							m_SkillIconImageInfoList;

	std::map< DWORD, BASE_MONSTER_LIST >	m_MonsterInfoList;
	typedef std::set< DWORD > MAPINDEXLIST;
	std::map< DWORD, MAPINDEXLIST >			m_MonsterRegenInfoList;

	std::map< DWORD, ITEM_INFO >			m_ItemInfoList;
	// setitem info
	// setitem Group에서 정보를 가지고있는 키아이템을 저장.
	std::map< DWORD, DWORD >				m_SetItemKeyIndexList;
	// setitem Group들의 키아이템으로 지정된 Script 저장
	std::map< DWORD, SetScript >			m_SetItemInfoList;

	std::map< DWORD, ACTIVE_SKILL_INFO >	m_ActiveSkillInfoList;
	std::map< DWORD, BUFF_SKILL_INFO >		m_BuffSkillInfoList;

	typedef std::vector< DWORD > CANLEARNSKILLINDEXLIST;
	// < 클래스 Index, 배울수있는 스킬목록 >
	std::map< DWORD, CANLEARNSKILLINDEXLIST >	m_ClassCanLearnSkillIndex;
	// < Skill Index, 클래스 Index >
	std::map< DWORD, DWORD >				m_SkillIndexBelongToClass;

	// Quest Data
	std::map< DWORD, std::string >			m_NpcMsgList;
	std::map< DWORD, std::string >			m_NpcHyperTextList;

	typedef std::map< DWORD, std::string >	NPCPAGEMSG;
	std::map< DWORD, NPCPAGEMSG >			m_NpcScriptMsgList;

	std::map< DWORD, std::string >			m_NpcName;
	std::map< std::pair< DWORD, DWORD >, std::string > m_QuestTitleList;

	std::map< DWORD, stQuestItenInfo >		m_QuestItemInfo;

	// System Message Load
	BOOL LoadSystemMsg();
	// Interface Message Load
	BOOL LoadInterfaceMsg();

	// 2D Image Load
	BOOL Load2DImageInfo();

	// Monster Info Load
	BOOL LoadMonsterList();
	BOOL LoadMonsterRegenInfo();

	// Item Info Load
	BOOL LoadItemInfo();
	BOOL LoadSetItmeInfo();
	void ParseSetItemAbility( std::list< std::string >& text, SetScript& script, const std::map< DWORD, std::string>& name );

	// Skill Info Load
	BOOL LoadActiveSkillInfo();
	BOOL LoadBuffSkillInfo();
	BOOL LoadSkillGetListInfo();

	// Tooltip Message Load
	BOOL LoadToolTipMsg();

	// Quest Data Load
	BOOL LoadNpcMsg();
	BOOL LoadNpcHyperText();
	BOOL LoadNpcScript();
	BOOL LoadNpcInfo();
	BOOL LoadQuestTitleText();
	BOOL LoadQuestItemInfo();

	BOOL LoadAllData( int nProgressPercent );

public:

	I4DyuchiGXExecutive* GetExecutive() const { return m_pExecutive; }
	GXOBJECT_HANDLE GetAddedGXObject() const { return m_AddedGXObjectHandel; }
	LPCTSTR GetSettingScriptFileName() const { return SETTING_LUA_SCRIPT_FILENAME; }

	LPCTSTR GetMapName( DWORD mapIndex );
	DWORD GetClassNameNum( int classIdx );
	LPCTSTR GetSystemMsg( DWORD index );
	LPCTSTR GetInterfaceMsg( DWORD index );
	LPCTSTR GetToolTipMsg( DWORD index );

	const ITEM_INFO& GetItemInfo( DWORD index ) const;
	BOOL GetItemIconInfo( DWORD itemIconImageIndex, CString& strImageFilePath, RECT& imageRect ) const;
	BOOL GetSkillIconInfo( DWORD skillIconImageIndex, CString& strImageFilePath, RECT& imageRect ) const;
	BOOL IsSetItem( DWORD itemIndex ) const;
	const SetScript& GetSetItemScript( DWORD itemIndex ) const;
	const SetScript::Element& GetItemElement( DWORD itemIndex ) const;

	const ACTIVE_SKILL_INFO& GetActiveSkillInfo( DWORD index );
	const BUFF_SKILL_INFO& GetBuffSkillInfo( DWORD index );

protected:
	////////////////////////////////////
	////////////////////////////////////
	// Extract
	////////////////////////////////////

	// Image
	BOOL Extract2DItemIconImage( int progressStartPercent, int progressEndPercent, BOOL bAllExtract, DWORD startItemIdx, DWORD endItemIdx );
	BOOL Extract2DActiveSkillIconImage( int progressStartPercent, int progressEndPercent, BOOL bAllExtract, DWORD startItemIdx, DWORD endItemIdx );
	BOOL Extract2DBuffSkillIconImage( int progressStartPercent, int progressEndPercent, BOOL bAllExtract, DWORD startItemIdx, DWORD endItemIdx );
	BOOL Extract3DMonsterImage( int progressStartPercent, int progressEndPercent, BOOL bAllExtract, DWORD startItemIdx, DWORD endItemIdx );

	// Item
	BOOL ExtractItemData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel );
	LPCTSTR GetItemDetailTypeText( const ITEM_INFO& itemInfo );
	LPCTSTR GetItemGradeText( const ITEM_INFO& itemInfo );
	std::string GetItemOptionText( const ITEM_INFO& itemInfo );
	std::string GetItemEtcInfoText( const ITEM_INFO& itemInfo );
	void GetItemSetOptionText( const SetScript& script, std::string& setOptText );
	void MakeItemWeaponData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel );
	void MakeItemArmorData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel );
	void MakeItemAccessaryData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel );
	void MakeItemEtcData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel );
	void MakeItemSetData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel );

    // Monster
	BOOL ExtractMonsterData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel );

	// Skill
	BOOL ExtractEachClassSkillData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel );
	void MakeEachClassSkillData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel );
	void MakeEachClassAllSkillData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel );
	LPCTSTR GetAttackTypeStringOfActiveSkill( DWORD skillIndex );

	// Quest
	BOOL ExtractQuestScriptData( int progressStartPercent, int progressEndPercent, BOOL bOutText, BOOL bOutExcel );

	void SaveToText( LPCTSTR saveFile, const CSimpleTextListCtrl& listCtrl );
	void SaveToExcel( LPCTSTR savePath, const CSimpleTextListCtrl& listCtrl );

	////////////////////////////////////
	////////////////////////////////////

// 생성
public:
	CLunaGameDataExtracterDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_LUNAGAMEDATAEXTRACTER_DIALOG };

	void LoadGameData();
	void ExtractGameData();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	CStatic m_PictureCtrl;
	CStatic m_ProgressStatusStaticCtrl;
	CProgressCtrl m_ProgressCtrl;
	CButton m_ScriptReLoadBtn;
	CButton m_ExtractBtn;

	void InitializeWindowFromScript();
	void InitializeEngine();
	void ReleaseEngine();
	void InsertGXObject( LPCTSTR objectname, float fScale=1.0f );

	afx_msg void OnTimer(UINT nIDEvent);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButtonExtract();
	afx_msg void OnBnClickedButtonScriptReload();
	virtual void OnCancel();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};