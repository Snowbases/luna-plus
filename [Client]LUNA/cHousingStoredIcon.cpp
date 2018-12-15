#include "cHousingStoredIcon.h"
#include "../[cc]Header/CommonStruct.h"
#include "ItemManager.h"
#include "cScriptManager.h"
#include "ChatManager.h"


cHousingStoredIcon::cHousingStoredIcon( stFurniture* pstFurniture  )
{
	m_type = WT_HOUSING_STORED_ICON;
	m_pLinkstFurniture = pstFurniture;
}

cHousingStoredIcon::~cHousingStoredIcon()
{

}
void cHousingStoredIcon::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cIcon::Init( x,  y,  wid,  hei,  basicImage, ID);
}

BOOL cHousingStoredIcon::CanDelete()
{
	if( m_pLinkstFurniture->bNotDelete )				//기본설치 품목일경우 아이템삭제 없음 
	{
		//090527 pdy 하우징 시스템메세지 기본제공가구 삭제제한 [가구삭제]
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1870 ) );	//1870	"기본 물품은 삭제 하실 수 없습니다."
		return FALSE;
	}
		
	if( m_pLinkstFurniture->wState == eHOUSEFURNITURE_STATE_INSTALL	)
	{
		//090527 pdy 하우징 시스템메세지 설치중인가구 삭제제한 [가구삭제]
		CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1905) );	//1905	"설치중인 품목은 바로 삭제하실수 없습니다."
		return FALSE;
	}

	return TRUE;
}

BOOL cHousingStoredIcon::CanMoveToDialog( cDialog* ptargetdlg )
{
	//창고 보관아이콘은 인벤토리로만 이동가능하다.
	if(	ptargetdlg->GetType() == WT_INVENTORYDIALOG )
		return TRUE;

	return FALSE;
}

DWORD cHousingStoredIcon::ActionEvent(CMouse* mouseInfo)
{
	DWORD we = WE_NULL;

	if( ! m_bActive )
		return we;

	we |= cWindow::ActionEvent(mouseInfo);

	if( !m_bDisable )
		we |= ActionEventWindow(mouseInfo);

	we |= ActionEventComponent(mouseInfo);

	if( we == WE_MOUSEOVER )
	{
		const ITEM_OPTION pOption ={0,}; 
		ITEMMGR->AddTimeToolTip( this, pOption, FALSE);
	}

	return we ;
}

void cHousingStoredIcon::Render()
{
	if( ! m_bActive )
		return;

	cIcon::Render();
}

void cHousingStoredIcon::Refresh()
{
	//가구 상태에 따라 색갈표시 
	switch(m_pLinkstFurniture->wState)
	{
		case eHOUSEFURNITURE_STATE_KEEP :
			{
				ClearStateImage();
			}
			break;
		case eHOUSEFURNITURE_STATE_INSTALL :
			{
				ClearStateImage();
				cImage dieimage;
				SCRIPTMGR->GetImage( 3, &dieimage );
				SetStateImage( &dieimage );
			}
			break;
		case eHOUSEFURNITURE_STATE_UNINSTALL :
			{
				ClearStateImage();
				cImage dieimage;
				SCRIPTMGR->GetImage( 145, &dieimage );
				SetStateImage( &dieimage );
			}
			break;
	}
}