#include "stdafx.h"
#include "cHouseNameDlg.h"
#include "WindowIDEnum.h"
#include "cImeEx.h"
#include "cWindowManager.h"
#include "cMsgBox.h"
#include "../ChatManager.h"

//090410 pdy 하우징 하우스 이름설정 UI
cHouseNameDlg::cHouseNameDlg()
{
	m_type = WT_HOUSE_NAME_DLG;
}

cHouseNameDlg::~cHouseNameDlg()
{

}

void cHouseNameDlg::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
	m_type = WT_HOUSE_NAME_DLG;
}

void cHouseNameDlg::Linking()
{
	m_pHouseName = (cStatic*)GetWindowForID(HOUSE_SEARCH_NAME);
	m_pHouseNameEdit = (cEditBox*)GetWindowForID(HOUSE_NAME_EDIT);
	m_pHouseNameEdit->SetValidCheck(/*VCM_CHARNAME*/VCM_DEFAULT);
	m_pHouseNameEdit->SetEditText("");
	m_pHouseNameOkBtn = (cButton*)GetWindowForID(HOUSE_NAME_OKBTN);
}

void cHouseNameDlg::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	cDialog::SetActive(val);
	cDialog::SetActiveRecursive(val);

	m_pHouseNameEdit->SetFocusEdit(val);

	if(val)
		m_pHouseNameEdit->SetEditText("");
}

void cHouseNameDlg::SetName(char* Name)
{
	m_pHouseNameEdit->SetEditText(Name);
}

char* cHouseNameDlg::GetName()
{
	return m_pHouseNameEdit->GetEditText();
}

void cHouseNameDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	switch(lId)
	{
		case HOUSE_NAME_OKBTN :
			{
				//이름을 채크하고 
				char* name = "";
				name = m_pHouseNameEdit->GetEditText();

				char MsgBuf[128] = {0,};
				sprintf(MsgBuf,CHATMGR->GetChatMsg(1889), name); //1889	"%s를(을) 사용하시겠습니까?"

				//090608 pdy 하우스이름 채크 추가 
				if(IsCharInString(name, "'"))
				{
					//090605 pdy 하우징 시스템메세지 HouseNameUI 사용할수없는 이름 [하우스 이름]  
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 551 ) );		//551	"사용할 수 없는 이름입니다."
					return;
				}

				//090527 pdy 하우징 팝업창 [하우스이름정하기]
				WINDOWMGR->MsgBox( MBI_HOUSENAME_AREYOUSURE, MBT_YESNO, MsgBuf) ; 
				SetDisable(TRUE) ;
			}
			break;
	}

}

