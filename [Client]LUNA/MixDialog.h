/**********************************************************************

이름: MixDialog
작성: 2007/10/22 10:16:28, 이웅주

목적: 재료를 조합하여 새로운 아이템을 제작함.
                                                                     
***********************************************************************/
#pragma once

#include "interface/cDialog.h"


class cStatic;
class cIconDialog;
class cIconGridDialog;
class CItem;
class CItemShow;
class cListDialog;
class cSpin;
class cComboBox;
class cEditBox;

struct ItemMixResult;

#define LESSSTRINGLEN	4

// 090812 ShinJS --- 조합 인터페이스 수정, Category Dialog 종류
enum eMixDlgCategoryType
{
	eMixDlgCategoryType_1,							// 1번째 카테고리 - Item Category
	eMixDlgCategoryType_2,							// 2번째 카테고리 - Item Equip, Expand, Material
	eMixDlgCategoryType_3,							// 3번째 카테고리 - 세부 사항
	eMixDlgCategoryType_4,							// 4번째 카테고리 - 조합식 종류
	eMixDlgCategoryType_Max,
};

// 090901 ShinJS --- 아이템 정렬 기준
enum eMixItemSortType
{
	eMixItemSortType_ByName,
	eMixItemSortType_ByLevel,
};

// 091126 ShinJS --- Mix Dialog의 상태
enum eMixDialogState
{
	eMixDialogState_Normal,		// 기본 상태
	eMixDialogState_Drag,		// Item을 Drag한 상태
	eMixDialogState_Search,		// 검색 상태
};

// 090812 ShinJS --- 조합 인터페이스 수정, 표시할 Item을 구분하기 위한 Category 정보
struct stCategoryInfo
{
	eMixDlgCategoryType		categoryType;			// Dialog의 Category 위치
	WORD				wID;					// ListItem ID
	WORD				wHighCategoryID;		// 상위 ListItem ID
    char				szName[ 256 ];			// List 상의 이름
	std::set<DWORD>		setItemType;			// Item Type
	std::set<DWORD>		setItemTypeDetail;		// Item Type Detail

	stCategoryInfo()
	{
		categoryType = eMixDlgCategoryType_Max;
		wID = 0;
		wHighCategoryID = 0;
		ZeroMemory( szName, 256 );
	}
};

class CMixDialog : public cDialog
{	
public:
	CMixDialog();
	virtual ~CMixDialog();
	
	virtual void SetActive( BOOL );
	virtual void SetActiveRecursive(BOOL val );
	virtual	DWORD ActionKeyboardEvent( CKeyboard* keyInfo );
	
	void Linking();	
	void OnActionEvent( LONG lId, void* p, DWORD we );

	void Fail( const MSG_ITEM_MIX_ACK& );
	void Succeed( const MSG_ITEM_MIX_ACK& );
	void Restore();
	void Send();
	void Refresh();
	void RefreshResult( const ITEMBASE& );


	// 080228 LUJ, 보호 아이템 관리
public:	
	void SetProtectedActive( CItem& );

private:
	ITEMBASE mProtectionItem;


private:
	// 조합을 요청한다. 인자는 제작 개수. isCheck가 참이면 값이 맞는지 체크하여 바로 전송한다
	void Submit( DURTYPE mixSize, BOOL isCheck );
	
	void RemoveResult();																		// 조합 아이템 정보 제거

	BOOL FakeMoveIcon( LONG x, LONG y, cIcon* icon );

	// 071218 LUJ, 조합 기본 아이템을 인벤토리로 돌린다
	void PutDescription( const ItemMixResult* );
	
	void Release();

	// 090812 ShinJS --- 조합 인터페이스 수정
	void LoadMixDlgCategory();																	// ListDialog의 Category List 정보를 읽어온다

	stCategoryInfo* GetSelectedCategoryInfo( eMixDlgCategoryType eCategory );					// ListDlg에서 현재 선택된 행의 Category 정보를 반환
	ITEM* GetSelectedItemFromListDlg( eMixDlgCategoryType eCategory, BOOL bCancelOpt=FALSE );	// ListDlg에서 현재 선택된 행의 ITEM 정보를 반환
	ITEM* GetSelectedItemFromListDlg( LONG lId );												// ListDlg에서 현재 선택된 행의 ITEM 정보를 반환, Category의 ID를 판단하여 GetSelectedCategoryInfo 실행

	void InitCategory();																		// Category ListDlg 초기화
	void UpdateCategory();																		// 현재 선택된 정보로 Category ListDlg Update
	void UpdateCategoryByItemIdx( DWORD dwItemIdx );											// 조합 Script에 해당하는 정보로 Category Update
	void UpdateHighCategory( DWORD dwResultItemIdx );											// 결과 아이템에 해당하는 정보로 상위 Category Update
	void UpdateOwnStuff();																		// 현재 선택된 조합아이템의 재료정보 소지현황 Update
	void UpdateCostInfo();																		// 비용정보 Update

	void SetSortType( eMixItemSortType eSortType );												// Mix Item List 정렬 방식 설정
	void ClearSelectedCategoryInfo();															// Category 선택시 저장한 정보들을 제거한다

	int GetCategoryIndexFromName( eMixDlgCategoryType eCategory, const char* szItemName );		// Category List에 추가된 이름으로 List의 Index를 구한다.
	int GetCategoryIndexFromItemIdx( eMixDlgCategoryType eCategory, DWORD dwItemIdx );			// Category List에 추가된 Data(ItemIndex)로 List의 Index를 구한다.

	void SearchMixResultItem();																	// 조합 결과아이템 검색

	void UpdateMixInfo();																		// 현재 선택된 List아이템으로 조합정보 Update
	void UpdateMixScript();																		// 현재 선택된 List아이템의 조합식에 대한 Update

public:
	void UpdateCategory( eMixDlgCategoryType eCategory );										// 해당 Category 를 Update
	void PushCatagory1BTN();
	void PushCatagory2BTN();
	void CatagoryScrollEnd();

	// 080925 LUJ, 조합 보조 아이템
public:
	// 080925 LUJ, 조합 보조 아이템 설정
	void SetSupportItem( const ITEMBASE& );
private:
	ICONBASE mSupportItem;
	
private:
	cListDialog*		mDescription;															// 조합 결과물 설명 ListDialog
	cIconDialog*		mResultIconDialog;														// 조합 결과물 IconDialog
	cIconGridDialog*	mIconGridDialog;														// 조합 재료 IconGridDialog
	
	CYHHashTable<stCategoryInfo>	m_htCategoryInfo;											// Category List 정보를 담은 Table

	int					m_SelectedCategory[ eMixDlgCategoryType_Max ];							// Category의 ListDlg에서 선택된 Item Index
	BOOL				m_bChangedCategory[ eMixDlgCategoryType_Max ];							// Category의 선택 변경 여부
	cListDialog*		m_CategoryDlg[ eMixDlgCategoryType_Max ];								// Category ListDialog
	cSpin*				m_pCntSpin;																// 조합 개수 설정을 위한 Spin
	cStatic*			m_pInvenMoney;															// 소지 금액 출력 static
	cStatic*			m_pMixCost;																// 조합 비용 출력 static
	cComboBox*			m_pSortTypeCombo;														// 정렬 Type 선택 ComboBox
	

	std::vector< CItemShow* >				mStuffIcon;											// 재료 정보를 담을 아이콘

	CItem*									mResultIcon;										// 조합 결과 아이콘

	ItemMixResult*							m_pSelectedMixResult;								// 선택된 조합 아이템의 스크립트 저장
	DURTYPE									m_MixItemCnt;										// 선택된 조합 아이템의 조합 개수
	CYHHashTable<ItemMixResult>				m_htEqualSelectedMixResult;							// 선택된 조합 아이템의 스크립트 Table (기본 재료아이템과 결과 아이템이 같고 조건이 다른 스크립트들 저장)
	eMixItemSortType						m_eMixItemSortType;									// 조합 아이템 List 정렬 방식
	eMixDialogState							m_eMixDialogState;									// 조합 Dialog 상태

	MSG_ITEM_MIX_SYN mMessage;

	// 080228 LUJ, 일반/보호 모드 표시 위한 컨트롤
	cWindow*	mTitle;
	cWindow*	mProtectTitle;
	cWindow*	mProtectSymbol;

	// 091124 ShinJS --- 검색 기능 추가
	cEditBox*								m_pSearchNameEdit;									// 검색어 입력 Edit Box
	cEditBox*								m_pSearchMinLv;										// 검색 Lv 입력 Edit Box
	cEditBox*								m_pSearchMaxLv;
	cButton*								m_pSearchConditionInitBTN;

	DWORD									m_dwDragItemIdx;									// Drag 시 Item Index 저장
	DWORD									m_dwMixResultCnt;
	BOOL									m_bIsEnoughMoney;									
};