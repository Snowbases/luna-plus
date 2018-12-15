//=================================================================================================
//	FILE		: cChangeNameDialog.cpp
//	PURPOSE		: Implemantacion part of change name dialog.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 27, 2007
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================





//=========================================================================
//		필요한 헤더파일을 불러온다.
//=========================================================================
#include "stdafx.h"
#include "../WindowIDEnum.h"
#include "../ItemManager.h"
#include "../InventoryExDialog.h"
#include "../GameIn.h"

#include ".\cchangenamedialog.h"

#include "./interface/cEditBox.h"
#include "../Interface/cWindowManager.h"
#include "../ChatManager.h"
#include "../cMsgBox.h"




//=========================================================================
//	NAME : cChangeNameDialog
//	DESC : Construct function.
//=========================================================================
cChangeNameDialog::cChangeNameDialog(void)
{
	m_pNameBox		= NULL ;

	m_pChangeBtn	= NULL ;
	m_pCancelBtn	= NULL ;
}






//=========================================================================
//	NAME : ~cChangeNameDialog
//	DESC : Descruct function.
//=========================================================================
cChangeNameDialog::~cChangeNameDialog(void)
{
}





//=========================================================================
//	NAME : Linking
//	DESC : The function to link all child controls.
//=========================================================================
void cChangeNameDialog::Linking()
{
	m_pNameBox		= (cEditBox*)GetWindowForID(NAME_BOX) ;

	m_pChangeBtn	= (cButton*)GetWindowForID(CHANGE_BTN) ;
	m_pCancelBtn	= (cButton*)GetWindowForID(CANCEL_BTN) ;
}





//=========================================================================
//	NAME : OnActionEvent
//	DESC : The function to process window event.
//=========================================================================
void cChangeNameDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	switch( id )
	{
	case CHANGE_BTN : 
		{
			// 091125 ONS 캐릭터명 변경시 팝업 메세지 출력
			WINDOWMGR->MsgBox( MBI_CHANGENAME_ACK, MBT_YESNO, CHATMGR->GetChatMsg(1976), GetChangeName());
		}
		break ;

	case CANCEL_BTN : 
		{
			Clear_NameBox() ;
			SetActive(FALSE) ;

			CInventoryExDialog* pInvenDlg = NULL ;
			pInvenDlg = GAMEIN->GetInventoryDialog() ;

			if( !pInvenDlg ) return ;

			CItem* pItem = NULL ;
			pItem = pInvenDlg->Get_QuickSelectedItem() ;

			if( !pItem ) return ;

			pItem->SetLock(FALSE) ;
			pInvenDlg->Set_QuickSelectedItem(NULL) ;
		}
		break ;
	}
}





////=========================================================================
////	NAME : Change_Name_Syn
////	DESC : The function to syn to server to change name.
////=========================================================================
//void cChangeNameDialog::Change_Name_Syn( char* pName )
//{
//	if( !pName ) return ;
//
//	CInventoryExDialog* pDlg = NULL ;
//	pDlg = GAMEIN->GetInventoryDialog() ;
//
//	if( !pDlg ) return ;
//
//	CItem* pItem = NULL ;
//	pItem = Get_QuickSelectedItem() ;
//
//	MSG_CHANGE_CHARACTER_NAME_SYN msg ;
//	memset(&msg, 0, sizeof(MSG_CHANGE_CHARACTER_NAME_SYN)) ;
//
//	msg.Category = MP_ITEM ;
//	msg.Protocol = MP_ITEM_CHANGE_CHARACTER_NAME_SYN ;
//}





//=========================================================================
//	NAME : Change_Name_Syn
//	DESC : The function to clear buffer for change name.
//=========================================================================
void cChangeNameDialog::Clear_NameBox()
{
	if( !m_pNameBox ) return ;

	m_pNameBox->SetEditText("") ;
}

// 091125 ONS 캐릭터명 변경시 팝업 출력 : 변경할 캐릭터명 반환 
char* cChangeNameDialog::GetChangeName()
{
	return m_pNameBox->GetEditText();
}