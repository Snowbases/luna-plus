#include "stdafx.h"
#include "WindowIDEnum.h"
#include "./Interface/cResourceManager.h"
#include "./interface/cStatic.h"
#include "./interface/cComboBox.h"
#include "ObjectManager.h"
#include "ChangeClassDlg.h"
#include "InventoryExDialog.h"
#include "GameIn.h"
#include "ItemManager.h"
#include "Interface/cWindowManager.h"
#include "ChatManager.h"
#include "cMsgBox.h"

void PressSubmitInMessageBox(LONG, LPVOID, DWORD windowEvent)
{
	CInventoryExDialog* pInvenDlg = GAMEIN->GetInventoryDialog() ;
	if( !pInvenDlg )
	{
		return;
	}

	CItem* pItem = pInvenDlg->Get_QuickSelectedItem() ;
	if( !pItem )
	{
		return;
	}

	if( MBI_YES == windowEvent)
	{
		ITEMMGR->UseItem_ChangeClass( pItem );
		pItem->SetLock(FALSE) ;
	}

	pItem->SetLock(FALSE) ;
	pInvenDlg->Set_QuickSelectedItem(NULL) ;
}

CChangeClassDlg::CChangeClassDlg()
{
	for( int i = 0; i < MAX_CLASS_LEVEL; i++ )
	{
		m_pClassKindStatic[i] = NULL;
		m_pClassKindCombo[i] = NULL;
	}
}

CChangeClassDlg::~CChangeClassDlg()
{

}

void CChangeClassDlg::Linking()
{
	// 직업명 타이틀
	for( int i = 0; i < MAX_CLASS_LEVEL; i++ )
	{
		char buf[32] = {0,};
		m_pClassKindStatic[i] = ( cStatic* )GetWindowForID( CHANGE_CLASS_STATIC_1ST + i );
		sprintf(buf, "[ %d%s ]", i + 1, RESRCMGR->GetMsg(1446));
		m_pClassKindStatic[i]->SetStaticText(buf);
	}

	// 기본직업명
	m_pFirstClassKind = ( cStatic* )GetWindowForID( CHANGE_CLASS_FIX_1ST );
		
	// 1차전직이후 직업명
	for( i = 0; i < MAX_CLASS_LEVEL - 1; i++ )
	{
		m_pClassKindCombo[i] = ( cComboBox* )GetWindowForID(CHANGE_CLASS_COMBOBOX_2ND + i);
	}
}

void CChangeClassDlg::SetActive( BOOL val )
{
	if( val )
	{
		SettingClassList();
	}
	cDialog::SetActive( val );
}

void CChangeClassDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	switch( lId )
	{
	case CHANGE_CLASS_BTN_SUBMIT:
		{
			cMsgBox* const messageBox = WINDOWMGR->MsgBox(
				MBI_CHANGE_CLASS,
				MBT_YESNO,
				CHATMGR->GetChatMsg(2224));

			if(0 == messageBox)
			{
				break;
			}

			SetActive(
				FALSE);
			messageBox->SetMBFunc(
				PressSubmitInMessageBox);
			SetDisable(
				TRUE);
		}
		break;
	case CHANGE_CLASS_BTN_CANCEL:
		{
			CInventoryExDialog* pInvenDlg = GAMEIN->GetInventoryDialog() ;
			if( !pInvenDlg ) return ;

			CItem* pItem = pInvenDlg->Get_QuickSelectedItem() ;
			if( !pItem ) return ;

			pItem->SetLock(FALSE) ;
			pInvenDlg->Set_QuickSelectedItem(NULL) ;
			SetActive(FALSE);
		}
		break;
	}
}

void CChangeClassDlg::SettingClassList()
{
	const BYTE MAX_CLASS_COUNT = 24;

	// 각 종족, 직업별 전직 정보를 상수로 정의한다. 현재 5차전직까지 정의.
	struct ClassInfo
	{
		WORD	wClassNo;				// 클래스 번호( InterfaceMsg에 정의)
		BYTE	byStage;				// 전직단계(1 ~ 5)
		BYTE	byRace;					// 종족(0:공통 1:휴먼 2:엘프)
	}
	const stClassInfo[eClass_MAXCLASS][MAX_CLASS_COUNT] = 
	{
		{
			{364, 1, 0}, {365, 2, 0}, {366, 2, 0}, {367, 3, 0}, {368, 3, 0}, {369, 3, 1}, 
			{370, 4, 0}, {371, 4, 0}, {372, 4, 1}, {373, 4, 2}, {374, 5, 0}, {375, 5, 0}, 
			{376, 5, 0}, {377, 5, 1}, {378, 5, 0}, {379, 5, 2}, {  0, 0, 0}, {  0, 0, 0},
			{  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}
		},
		{
			{383, 1, 0}, {384, 2, 0}, {385, 2, 0}, {386, 3, 0}, {387, 3, 1}, {388, 3, 2}, 
			{389, 4, 0}, {390, 4, 0}, {391, 4, 1}, {392, 4, 2}, {393, 5, 0}, {394, 5, 0}, 
			{395, 5, 1}, {396, 5, 2}, {1106,5, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0},
			{  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}
		},
		{
			{400, 1, 0}, {401, 2, 0}, {402, 2, 0}, {403, 3, 0}, {404, 3, 0}, {405, 3, 1}, 
			{406, 4, 0}, {407, 4, 0}, {408, 4, 1}, {409, 4, 2}, {410, 5, 0}, {411, 5, 0}, 
			{412, 5, 0}, {413, 5, 1}, {414, 5, 2}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0},
			{  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}
		},
		{
			{1132,1, 0}, {1133,2, 0}, {1134,3, 0}, {1135,4, 0}, {  0, 0, 0}, {  0, 0, 0},
			{  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0},
			{  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0},
			{  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}, {  0, 0, 0}
		}
	};


	// 전직정보를 얻기위해 캐릭터 정보를 얻는다.
	CHARACTER_TOTALINFO ChaInfo;
	OBJECTMGR->GetHero()->GetCharacterTotalInfo(&ChaInfo);
	const BYTE byClassType = ChaInfo.Job[0] - 1;
		
	// 기본직업은 고정출력한다.
	m_pFirstClassKind->SetStaticText(RESRCMGR->GetMsg( stClassInfo[byClassType][0].wClassNo ));

	// 초기화
	for( int i = 0; i < MAX_CLASS_LEVEL-1; i++ )
	{
		m_pClassKindCombo[i]->RemoveAll();
		m_pClassKindCombo[i]->SetDisable(FALSE);
		m_pClassKindStatic[i+1]->SetFGColor( RGBA_MAKE( 255, 255, 255, 255 ) );
	}

	int nIndex = 1;
	int nLine = 0;
	BYTE byOldStage = 0;
	BYTE bySelectIndex = 0;
	// 현재 전직 레벨까지만 콤보박스로 처리한다.
	while(	stClassInfo[byClassType][nIndex].wClassNo > 0 && 
			stClassInfo[byClassType][nIndex].byStage <= ChaInfo.JobGrade )
	{
		BYTE byCurStage = stClassInfo[byClassType][nIndex].byStage - 1;
		if( byOldStage != byCurStage )
		{
			// 전직 단계가 증가했을경우,
			nLine = 0;
			bySelectIndex = 0;
			byOldStage = byCurStage;
		}
		
		ITEM* pItem = new ITEM ;
		pItem->line = nLine;
		pItem->rgb = RGBA_MAKE(255, 255, 255, 255) ;
		strcpy(pItem->string, RESRCMGR->GetMsg( stClassInfo[byClassType][nIndex].wClassNo ));

		// 리스트에 직업명을 추가하고, 현재 전직정보를 표시한다.
		if( stClassInfo[byClassType][nIndex].byRace == 0 || 
			stClassInfo[byClassType][nIndex].byRace == ChaInfo.Race + 1 )
		{
			m_pClassKindCombo[ byCurStage - 1 ]->AddItem(pItem) ;

			if( nLine == ChaInfo.Job[byCurStage] - 1 )
			{
				m_pClassKindCombo[byCurStage - 1]->SetCurSelectedIdx( bySelectIndex );
				m_pClassKindCombo[byCurStage - 1]->SelectComboText( (WORD)bySelectIndex );
			}

			m_pClassKindCombo[byCurStage - 1]->SetComboTextColor(RGBA_MAKE(0, 255, 255, 255));
			bySelectIndex++;
		}

		nIndex++;
		nLine++;
	}

	// 남은 전직단계는 비활성화 처리한다.
	for( int nIdx = ChaInfo.JobGrade; nIdx <= MAX_CLASS_LEVEL - 1; nIdx++ )
	{
		m_pClassKindStatic[nIdx]->SetFGColor( RGBA_MAKE( 165, 165, 165, 255 ) );
		m_pClassKindCombo[nIdx - 1]->SetDisable(TRUE);
	}
}

int CChangeClassDlg::GetCurSelectedIdx( WORD wStage )
{
	return m_pClassKindCombo[wStage]->GetCurSelectedIdx();
}

ITEM* CChangeClassDlg::GetItem( WORD wStage, int nIndex )
{
	return m_pClassKindCombo[wStage]->GetItem(nIndex);
}
