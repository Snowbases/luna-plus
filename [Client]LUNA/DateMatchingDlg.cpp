#include "stdafx.h"
#include "cWindowManager.h"
#include "WindowIDEnum.h"
#include ".\datematchingdlg.h"
#include "cStatic.h"
#include "cEditBox.h"
#include "cIconGridDialog.h"
#include "cScriptManager.h"
#include "cIcon.h"
#include "cComboBox.h"
//#include "cListCtrl.h"
#include "cListDialog.h"
#include "ObjectManager.h"
#include "cMsgBox.h"
#include "ChatManager.h"

#include "../Interface/cPushupButton.h"

#include "../hseos/ResidentRegist/SHResidentRegistManager.h"

#include "../GameIn.h"
#include "../NpcScriptDialog.h"
#include "../cIMEex.h"
#include "../MHFile.h"
#include "ObjectStateManager.h"

#include "cRitemEx.h"
#include "../Interface/cCheckBox.h"
#include "../Interface/cTextArea.h"
#include "../Interface/cListCtrl.h"
#include "../Interface/cResourceManager.h"

#include "../Input/Mouse.h"
#include "../Input/Keyboard.h"

#include "./Audio/MHAudioManager.h"

#include "../hseos/ResidentRegist/SHResidentRegistManager.h"
#include "../hseos/Date/SHDateZoneListDlg.h"
#include "../hseos/Date/SHChallengeZoneListDlg.h"
#include "../hseos/Date/SHDateManager.h"

#include "./Interface/cFont.h"

CIdentification::CIdentification()
{
	//m_type = WT_MATCH_IDENTIFICATIONDLG ;

	//m_pFacePicture	= NULL ;

	m_pNickName		= NULL ;
	m_pGender		= NULL ;
	m_pAge			= NULL ;
	m_pRegion		= NULL ;
	m_pFavor		= NULL ;

	m_pGoodIcon		= NULL ;
	m_pBadIcon		= NULL ;

	m_pSaveBtn		= NULL ;
	m_pCancelBtn	= NULL ;

	m_pRegionItem	= NULL ;

	memset(&m_RegistInfo, 0, sizeof(CSHResidentRegistManager::stRegistInfo)) ;

	m_IconIndexCreator.Init(MAX_ITEMICON_NUM, IG_IDENTIFICATION_STARTINDEX) ;
}

CIdentification::~CIdentification()
{}

void CIdentification::Linking()
{
	//m_pFacePicture	= (cStatic*)GetWindowForID(DMD_IDT_MYFACE) ;

	m_pNickName		= (cStatic*)GetWindowForID(DMD_IDT_NICKNAME) ;
	m_pGender		= (cStatic*)GetWindowForID(DMD_IDT_GENDER) ;
	m_pAge			= (cEditBox*)GetWindowForID(DMD_IDT_AGE) ;
	m_pAge->SetValidCheck( VCM_NUMBER );
	m_pRegion		= (cListDialog*)GetWindowForID(DMD_IDT_REGION) ;
	m_pFavor		= (cStatic*)GetWindowForID(DMD_IDT_GOOD_FEELING_DEGREE) ;

	m_pGoodIcon		= (cIconGridDialog*)GetWindowForID(DMD_IDT_GOOD_FEELING_ICONGRID) ;
	m_pBadIcon		= (cIconGridDialog*)GetWindowForID(DMD_IDT_BAD_FEELING_ICONGRID) ;

	m_pSaveBtn		= (cButton*)GetWindowForID(DMD_IDT_SAVE_BTN) ;
	m_pCancelBtn	= (cButton*)GetWindowForID(DMD_IDT_CANCEL_BTN) ;
}

void CIdentification::SetActive( BOOL val )
{
	if( val )
	{
		if(cDialog* const dialog = WINDOWMGR->GetWindowForID(DMD_FAVOR_ICON_DLG))
		{
			SetAbsXY(
				LONG(dialog->GetAbsX() + abs(float(GetWidth()) - float(dialog->GetWidth())) / 2.0f),
				LONG(dialog->GetAbsY() < GetHeight() ? dialog->GetAbsY() + GetHeight() : dialog->GetAbsY() - GetHeight()));
		}

		SettingControls();
	}

	cDialog::SetActive( val );
}

void CIdentification::SettingControls()
{
	// resetting regist info.
	memset(&m_RegistInfo, 0, sizeof(CSHResidentRegistManager::stRegistInfo)) ;

	// setting combobox of region part.
	CSHResidentRegistManager::stADDInfo* pInfo = g_csResidentRegistManager.GetADDInfo() ;
	m_pRegion->RemoveAll();
	for(int count = 1 ; count < pInfo->m_nMaxLocation ; ++count)
	{
		const CSHResidentRegistManager::stLocation& location = pInfo->m_pTblLocation[ count ];

		m_pRegion->AddItem(
			location.szName,
			RGB( 255, 255, 255 ) );;
	}

	CHARACTER_TOTALINFO* pTotalInfo = ((CPlayer*)HERO)->GetCharacterTotalInfo() ;

	if( !pTotalInfo ) return ;
	// setting nickname.
	m_pNickName->SetStaticText(HERO->GetObjectName()) ;

	// setting gender.
	if( pTotalInfo->Gender == 0 )
	{
		m_pGender->SetStaticText(RESRCMGR->GetMsg(508)) ;
	}
	else
	{
		m_pGender->SetStaticText(RESRCMGR->GetMsg(509)) ;
	}

	// setting age.
	m_pAge->SetEditText("") ;

	// setting face.
	int nFaceNum = 0 ;

	// 090521 ONS 신규종족 데이트매칭관련 처리 추가
	// check racial.
	int nRacial = HERO->GetCharacterTotalInfo()->Race ;

	// setting face num.
	if( nRacial == e_MATCH_HUMAN )
	{
		// setting human.
		if( pTotalInfo->Gender == e_MATCH_MAN)
		{
			nFaceNum = e_MATCH_HUMAN_MAN ;
		}
		else
		{
			nFaceNum = e_MATCH_HUMAN_WOMAN ;
		}
	}
	else if(nRacial == e_MATCH_ELF)
	{
		// setting elf.
		if( pTotalInfo->Gender == e_MATCH_MAN)
		{
			nFaceNum = e_MATCH_ELF_MAN ;
		}
		else
		{
			nFaceNum = e_MATCH_ELF_WOMAN ;
		}
	}
	else
	{
		// setting devil.
		if( pTotalInfo->Gender == e_MATCH_MAN)
		{
			nFaceNum = e_MATCH_DEVIL_MAN ;
		}
		else
		{
			nFaceNum = e_MATCH_DEVIL_WOMAN ;
		}
	}

	// setting face image.
	SCRIPTMGR->GetImage( nFaceNum, &m_FacePicture, PFT_HARDPATH ) ;

	// setting favor.
	m_pFavor->SetStaticText("0") ;

	// reset icon part.
	CFavorItem* pCheckItem = NULL ;

	for(WORD GoodCount = 0 ; GoodCount < MAX_GOOD_ICON_COUNT ; ++GoodCount )
	{
		pCheckItem = (CFavorItem*)m_pGoodIcon->GetIconForIdx(GoodCount) ;

		if( pCheckItem )
		{
			pCheckItem->SetActive(FALSE) ;
			DeleteItem(pCheckItem) ;

			pCheckItem->SetDepend(TRUE) ;
			WINDOWMGR->SetMouseInputProcessed() ;
		}
	}
	m_pGoodIcon->SetCurSelCellPos(-1) ;

	for(WORD BadCount = 0 ; BadCount < MAX_BAD_ICON_COUNT ; ++BadCount )
	{
		pCheckItem = (CFavorItem*)m_pBadIcon->GetIconForIdx(BadCount) ;

		if( pCheckItem )
		{
			pCheckItem->SetActive(FALSE) ;
			DeleteItem(pCheckItem) ;

			pCheckItem->SetDepend(TRUE) ;
			WINDOWMGR->SetMouseInputProcessed() ;
		}
	}
	m_pBadIcon->SetCurSelCellPos(-1) ;

	m_RegistInfo.nPropensityLike01 = WORD(-1);
	m_RegistInfo.nPropensityLike02 = WORD(-1);
	m_RegistInfo.nPropensityLike03 = WORD(-1);
	m_RegistInfo.nPropensityDisLike01 = WORD(-1);
	m_RegistInfo.nPropensityDisLike02 = WORD(-1);
}

void CIdentification::ResetAllData()
{
	m_pAge->SetEditText("") ;
	m_pRegion->RemoveAll();
	m_pFavor->SetStaticText("0") ;

	memset(&m_RegistInfo, 0, sizeof(CSHResidentRegistManager::stRegistInfo)) ;

	CFavorItem* pCheckItem = 0;

	for(WORD count = 0 ; count < MAX_GOOD_ICON_COUNT ; ++count )
	{
		pCheckItem = (CFavorItem*)m_pGoodIcon->GetIconForIdx(count) ;

		if( pCheckItem )
		{
			//m_pGoodIcon->DeleteIcon(pCheckItem) ;

			pCheckItem->SetActive(FALSE) ;
			DeleteItem(pCheckItem) ;

			pCheckItem->SetDepend(TRUE) ;
			WINDOWMGR->SetMouseInputProcessed() ;
		}
	}

	for(WORD count = 0 ; count < MAX_BAD_ICON_COUNT ; ++count )
	{
		pCheckItem = (CFavorItem*)m_pBadIcon->GetIconForIdx(count) ;

		if( pCheckItem )
		{
			//m_pBadIcon->DeleteIcon(pCheckItem) ;

			pCheckItem->SetActive(FALSE) ;
			DeleteItem(pCheckItem) ;

			pCheckItem->SetDepend(TRUE) ;
			WINDOWMGR->SetMouseInputProcessed() ;
		}
	}
}

void CIdentification::LoadFavorItem()
{
	// read favor item list.
	CMHFile	file ;

	file.Init( FILE_FAVOR_ITEM_LIST, "rb" ) ;

	if( file.IsInited() == FALSE )	return ; // 추후 ASSERT 처리 할 것.

	m_FavorIconList.GetHeadPosition() ;

	cImage tmpImage ;
	CFavorItem* pItem = NULL ;

	while(1)
	{
		if( file.IsEOF() != FALSE ) break ;

		pItem = new CFavorItem ;

		pItem->SetItemIdx( file.GetDword() ) ;
		pItem->SetItemName( file.GetString() ) ;
		// setting tooltip.
		pItem->SetToolTipNum( file.GetDword() ) ;
		cImage imgToolTip;
		SCRIPTMGR->GetImage( 0, &imgToolTip, PFT_HARDPATH );		
		pItem->SetToolTip( pItem->GetItemName(), RGB(0, 255, 255), &imgToolTip);
		pItem->AddToolTipLine("") ;
		pItem->AddToolTipLine(RESRCMGR->GetMsg(pItem->GetToolTipNum()), RGB(255, 255, 255)) ;
		pItem->SetImageNum( file.GetWord() ) ;
		pItem->SetPos( file.GetWord() ) ;

		pItem->Init(0, 0, 26, 26, GAMEIN->GetFavorIconDlg()->GetIconImage(pItem->GetImageNum(), &tmpImage), m_IconIndexCreator.GenerateIndex()) ;
		WINDOWMGR->AddWindow(pItem);
		pItem->SetDepend(TRUE) ;
		pItem->SetParent(this);

		m_FavorIconList.AddTail(pItem) ;
	}

	file.Release() ;
}

void CIdentification::Render()
{
	if( !IsActive() ) return ;
	cDialog::RenderWindow();
	cDialog::RenderComponent();

	// render face.
	VECTOR2 scale;
	scale.x = 1;
	scale.y = 1;

	VECTOR2 vPos;
	vPos.x = (float)(GetAbsX() + 15) ;
	vPos.y = (float)(GetAbsY() + 45) ;

	DWORD dwColor = 0xffffffff ;

	m_FacePicture.RenderSprite(&scale, NULL, 0, &vPos, dwColor) ;
}
void CIdentification::OnActionEvent(LONG lId, void * p, DWORD we)
{
	switch(lId)
	{
	case DMD_IDT_CANCEL_BTN :
	case DMD_CLOSEBTN :
		{
			CancelRegist() ;
		}
		break ;

	case DMD_IDT_SAVE_BTN :
		{
			// check age.
			int nAge = atoi(m_pAge->GetEditText()) ;
			// 090212 NYJ - 주민등록시 최대나이를 100세 => 99세로 변경.
			if( strlen(m_pAge->GetEditText()) <= 0 || nAge <= 0 || nAge > 99 )
			//if( strlen(m_pAge->GetEditText()) <= 0 || nAge <= 0 || nAge > 100 )
			{
				// notice warning.MBI_INVALID_AGE
				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, CHATMGR->GetChatMsg( 1306 ) );
				return ;
			}

			// check region.
			if( m_pRegion->GetClickedRowIdx() <= -1 )
			{
				// notice warning.
				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, CHATMGR->GetChatMsg( 1307 ) );
				return ;
			}

			// check good feeling.
			if( m_RegistInfo.nPropensityLike01 == 65535 )
			{
				char buffer[ MAX_PATH ];
				sprintf( buffer, CHATMGR->GetChatMsg( 1308 ), 1 );

				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, buffer );
				return ;
			}

			if( m_RegistInfo.nPropensityLike02 == 65535 )
			{
				char buffer[ MAX_PATH ];
				sprintf( buffer, CHATMGR->GetChatMsg( 1308 ), 2 );

				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, buffer );
				return ;
			}

			if( m_RegistInfo.nPropensityLike03 == 65535 )
			{
				char buffer[ MAX_PATH ];
				sprintf( buffer, CHATMGR->GetChatMsg( 1308 ), 3 );

				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, buffer );
				return ;
			}

			// check bad feeling.
			if( m_RegistInfo.nPropensityDisLike01 == 65535 )
			{
				char buffer[ MAX_PATH ];
				sprintf( buffer, CHATMGR->GetChatMsg( 1309 ), 1 );

				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, buffer );
				return ;
			}

			if( m_RegistInfo.nPropensityDisLike02 == 65535 )
			{
				char buffer[ MAX_PATH ];
				sprintf( buffer, CHATMGR->GetChatMsg( 1309 ), 2 );

				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, buffer );
				return ;
			}

			if( m_RegistInfo.nPropensityLike01 == m_RegistInfo.nPropensityLike02 )
			{
				char buffer[ MAX_PATH ];
				sprintf( buffer, CHATMGR->GetChatMsg( 1309 ), 3 );

				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, buffer );
				return ;
			}

			if( m_RegistInfo.nPropensityLike01 == m_RegistInfo.nPropensityLike03 )
			{
				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, "" );
				return ;
			}

			if( m_RegistInfo.nPropensityLike01 == m_RegistInfo.nPropensityDisLike01 )
			{
				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, "" );
				return ;
			}

			if( m_RegistInfo.nPropensityLike01 == m_RegistInfo.nPropensityDisLike02 )
			{
				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, "" );
				return ;
			}

			if( m_RegistInfo.nPropensityLike02 == m_RegistInfo.nPropensityLike03 )
			{
				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, "" );
				return ;
			}

			if( m_RegistInfo.nPropensityLike02 == m_RegistInfo.nPropensityDisLike01 )
			{
				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, "" );
				return ;
			}

			if( m_RegistInfo.nPropensityLike02 == m_RegistInfo.nPropensityDisLike02 )
			{
				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, "" );
				return ;
			}

			if( m_RegistInfo.nPropensityLike03 == m_RegistInfo.nPropensityDisLike01 )
			{
				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, "" );
				return ;
			}

			if( m_RegistInfo.nPropensityLike03 == m_RegistInfo.nPropensityDisLike02 )
			{
				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, "" );
				return ;
			}

			if( m_RegistInfo.nPropensityDisLike01 == m_RegistInfo.nPropensityDisLike02 )
			{
				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, "" );
				return ;
			}

			// notice once more.
			WINDOWMGR->MsgBox( MBI_IDENTIFICATION_REGIST_INFO, MBT_YESNO, CHATMGR->GetChatMsg( 1310 ) );
			//g_csResidentRegistManager.CLI_RequestRegist(HEROID, &RegistInfo) ;
		}
		break ;
	}
}

BOOL CIdentification::FakeMoveIcon(LONG x, LONG y, cIcon * icon) 
{
	if( !icon ) return FALSE ;

	if( m_bDisable ) return FALSE ;

	if( icon->GetType() == WT_ICON )
	{
		return FakeMoveItem( x, y, (CFavorItem*)icon ) ;
	}

	return FALSE ;
}

BOOL CIdentification::FakeMoveItem(LONG mouseX, LONG mouseY, cIcon * icon)
{
	// setting to pos
	WORD wToPos = 0 ;
	if( !GetPositionForXYRef(mouseX, mouseY, wToPos) ) return FALSE ;

	AUDIOMGR->Play(
		58,
		gHeroID);

	// setting icon to slot.
	if( m_pGoodIcon->PtInWindow(mouseX, mouseY) )
	{
		SetGoodIcon(wToPos, (CFavorItem*)icon) ;
	}
	else if( m_pBadIcon->PtInWindow(mouseX, mouseY) )
	{
		SetBadIcon(wToPos, (CFavorItem*)icon) ;
	}

	return TRUE ;
}

BOOL CIdentification::GetPositionForXYRef(LONG x, LONG y, WORD& pos)
{
	WORD wPosition = 0 ;

	if( m_pGoodIcon->PtInWindow(x, y) )
	{
		if(m_pGoodIcon->GetPositionForXYRef(x, y, wPosition))
		{
			pos = wPosition ;
			return TRUE ;
		}
	}
	else if( m_pBadIcon->PtInWindow(x, y) )
	{
		if(m_pBadIcon->GetPositionForXYRef(x, y, wPosition))
		{
			pos = wPosition ;
			return TRUE ;
		}
	}

	return FALSE ;
}

void CIdentification::SetGoodIcon(int nPos, CFavorItem* pItem)
{
	if( !CheckSameItem(pItem) ) return;

	PTRLISTPOS pos = m_FavorIconList.GetHeadPosition();

	for(CFavorItem* pNewItem = (CFavorItem*)m_FavorIconList.GetNext(pos);
		0 < pNewItem;
		pNewItem = (CFavorItem*)m_FavorIconList.GetNext(pos))
	{
		if( pItem->GetItemIdx() == pNewItem->GetItemIdx() )
		{
			CFavorItem* pDeleteItem = (CFavorItem*)m_pGoodIcon->GetIconForIdx(
				WORD(nPos));

			if( pDeleteItem )
			{
				m_pGoodIcon->DeleteIcon(pDeleteItem);
				pDeleteItem->SetActive(FALSE) ;
				pDeleteItem->SetDepend(TRUE);
				pDeleteItem->SetParent(this);
			}

			m_pGoodIcon->AddIcon(
				WORD(nPos),
				pNewItem);

			switch(nPos)
			{
			case 0 : m_RegistInfo.nPropensityLike01 = (WORD)pItem->GetItemIdx()+1 ; break ;
			case 1 : m_RegistInfo.nPropensityLike02 = (WORD)pItem->GetItemIdx()+1 ; break ;
			case 2 : m_RegistInfo.nPropensityLike03 = (WORD)pItem->GetItemIdx()+1 ; break ;
			}
		}
	}
}

void CIdentification::SetBadIcon(int nPos, CFavorItem* pItem)
{
	if( !CheckSameItem(pItem) ) return;

	PTRLISTPOS pos = m_FavorIconList.GetHeadPosition() ;

	for(CFavorItem* pNewItem = (CFavorItem*)m_FavorIconList.GetNext(pos);
		0 < pNewItem;
		pNewItem = (CFavorItem*)m_FavorIconList.GetNext(pos))
	{
		if( pItem->GetItemIdx() == pNewItem->GetItemIdx() )
		{
			CFavorItem* pDeleteItem = (CFavorItem*)m_pBadIcon->GetIconForIdx(
				WORD(nPos));

			if( pDeleteItem )
			{
				m_pBadIcon->DeleteIcon(pDeleteItem);
				pDeleteItem->SetActive(FALSE) ;
				pDeleteItem->SetDepend(TRUE);
				pDeleteItem->SetParent(this);
			}

			m_pBadIcon->AddIcon(
				WORD(nPos),
				pNewItem);

			switch(nPos)
			{
			case 0 : m_RegistInfo.nPropensityDisLike01 = (WORD)pItem->GetItemIdx()+1 ; break ;
			case 1 : m_RegistInfo.nPropensityDisLike02 = (WORD)pItem->GetItemIdx()+1 ; break ;
			}
		}
	}
}

BOOL CIdentification::CheckSameItem(CFavorItem* pItem)
{
	CFavorItem* pCheckItem = NULL ;

	for(WORD count = 0 ; count < MAX_GOOD_ICON_COUNT ; ++count )
	{
		pCheckItem = (CFavorItem*)m_pGoodIcon->GetIconForIdx(count) ;

		if( pCheckItem )
		{
			if( pCheckItem->GetItemIdx() == pItem->GetItemIdx() )
			{
				// notice error.
				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, CHATMGR->GetChatMsg( 1312 ) );

				return FALSE ;
			}
		}
	}

	for(WORD count = 0 ; count < MAX_BAD_ICON_COUNT ; ++count )
	{
		pCheckItem = (CFavorItem*)m_pBadIcon->GetIconForIdx(count) ;

		if( pCheckItem )
		{
			if( pCheckItem->GetItemIdx() == pItem->GetItemIdx() )
			{
				// notice error.
				WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ERROR, MBT_OK, CHATMGR->GetChatMsg( 1312 ) );

				return FALSE ;
			}
		}
	}

	return TRUE ;
}

void CIdentification::DeleteItem(CFavorItem* pItem)
{
	CFavorItem* pDeleteItem = NULL;
	WORD itemCount = m_pGoodIcon->GetCellNum();

	for(WORD count = 0 ; count < itemCount ; ++count )
	{
		pDeleteItem = (CFavorItem*)m_pGoodIcon->GetIconForIdx(count) ;

		if( pDeleteItem )
		{
			if( pDeleteItem->GetItemIdx() == pItem->GetItemIdx() )
			{
				// delete item from icon grid.
				m_pGoodIcon->DeleteIcon(pDeleteItem) ;

				// delete propensity like.
				switch(count)
				{
				case 0 : m_RegistInfo.nPropensityLike01 = 65535 ;	break ;
				case 1 : m_RegistInfo.nPropensityLike02 = 65535 ;	break ;
				case 2 : m_RegistInfo.nPropensityLike03 = 65535 ;	break ;
				}

				return ;
			}
		}
	}

	// check bad icon part.
	itemCount =  m_pBadIcon->GetCellNum() ;

	for(WORD count = 0 ; count < itemCount ; ++count )
	{
		pDeleteItem = (CFavorItem*)m_pBadIcon->GetIconForIdx(count) ;

		if( pDeleteItem )
		{
			if( pDeleteItem->GetItemIdx() == pItem->GetItemIdx() )
			{
				// delete item from icon grid.
				m_pBadIcon->DeleteIcon(pDeleteItem) ;

				// delete propensity dislike.
				switch(count)
				{
				case 0 : m_RegistInfo.nPropensityDisLike01 = 65535 ;	break ;
				case 1 : m_RegistInfo.nPropensityDisLike02 = 65535 ;	break ;
				}

				return ;
			}
		}
	}
}

void CIdentification::CancelRegist()
{
	// delete all data.
	//ResetAllData() ;
	SettingControls() ;

	// close this dialog.
	SetActive(FALSE) ;

	// close favor icon dialog.
	GAMEIN->GetFavorIconDlg()->SetActive(FALSE) ;

	// close npc dialog.
	GAMEIN->GetNpcScriptDialog()->SetActive(FALSE) ;

	// Deactive npc script dialog.
	if( HERO->GetState() != eObjectState_Die )
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
}










CFavorItem::CFavorItem()
{
}

CFavorItem::~CFavorItem() 
{
}









CFavorIcon::CFavorIcon()
{
	m_pFavorIcon = NULL;
	m_CurSelectedItemIdx = DWORD(-1);
	m_lCurSelItemPos = -1;
	m_IconIndexCreator.Init(MAX_ITEMICON_NUM, IG_FAVOR_STARTINDEX) ;
}

CFavorIcon::~CFavorIcon()
{
	m_IconIndexCreator.Release();
}

void CFavorIcon::Linking()
{
	m_pFavorIcon = (cIconGridDialog*)GetWindowForID(DMD_FAVOR_ICON) ;
}

void CFavorIcon::LoadFavorItem()
{
	// read favor item list.
	CMHFile	file ;

	file.Init( FILE_FAVOR_ITEM_LIST, "rb" ) ;

	if( file.IsInited() == FALSE )	return ; // 추후 ASSERT 처리 할 것.

	m_FavorItemList.GetHeadPosition() ;

	cImage tmpImage;
	CFavorItem* pItem = NULL ;

	while(1)
	{
		if( file.IsEOF() != FALSE ) break ;

		pItem = new CFavorItem ;

		pItem->SetItemIdx( file.GetDword() ) ;
		pItem->SetItemName( file.GetString() ) ;
		// setting tooltip.
		pItem->SetToolTipNum( file.GetDword() ) ;
		cImage imgToolTip;
		SCRIPTMGR->GetImage( 0, &imgToolTip, PFT_HARDPATH );		
		pItem->SetToolTip( pItem->GetItemName(), RGB(0, 255, 255), &imgToolTip);
		pItem->AddToolTipLine("") ;
		pItem->AddToolTipLine(RESRCMGR->GetMsg(pItem->GetToolTipNum()), RGB(255, 255, 255)) ;
		pItem->SetImageNum( file.GetWord() ) ;
		pItem->SetPos( file.GetWord() ) ;
		pItem->Init(0, 0, 26, 26, GetIconImage(pItem->GetImageNum(), &tmpImage), m_IconIndexCreator.GenerateIndex()) ;
		WINDOWMGR->AddWindow(pItem);
		pItem->SetDepend(TRUE) ;

		m_FavorItemList.AddTail(pItem) ;
	}

	file.Release() ;
}

void CFavorIcon::ShowFavorItem()
{
	m_FavorItemList.GetHeadPosition() ;

	CFavorItem* pItem = NULL ;
	PTRLISTPOS pos = m_FavorItemList.GetHeadPosition() ;

	int maxCount = m_FavorItemList.GetCount() ;

	for(WORD count = 0 ; count < maxCount ; ++count )
	{
		pItem = (CFavorItem*)m_FavorItemList.GetNext(pos) ;
		if( pItem )
		{
			m_pFavorIcon->AddIcon(count, pItem) ;
		}
	}
}

void CFavorIcon::Render()
{
	if( !IsActive() ) return ;
	cDialog::RenderWindow();
	cDialog::RenderComponent();

	ToolTipRender();
}
cImage * CFavorIcon::GetIconImage(WORD ImgIdx, cImage * pImageGet)
{
	SCRIPTMGR->GetImage( (int)ImgIdx, pImageGet, PFT_FAVOR_ICON_PATH );	
	return pImageGet;
}

CDateMatchingPartnerList::CDateMatchingPartnerList()
{
	m_type = WT_MATCHPARTNERLISTDLG ;

	m_pAge			= NULL ;
	m_pRegion		= NULL ;

	m_pGenderMan	= NULL ;
	m_pGenderWoman	= NULL ;

	m_pSearch		= NULL ;
	m_pRefresh		= NULL ;
	m_pChatting		= NULL ;

	m_pPartnerList	= NULL ;

	m_pList_Gender		= NULL ;
	m_pList_Age			= NULL ;
	m_pList_NickName	= NULL ;
	m_pList_Favor		= NULL ;
	m_pList_Point		= NULL ;

	//m_pOverImage		= NULL ;
	//m_pSelectImage		= NULL ;

	m_nPage = 0 ;
	m_nTotalPage = 0 ;
	// desc_hseos_데이트매칭01
	// S 데이트매칭 추가 added by hseos 2008.01.07
	m_nSerchGroupIndex = 0 ;
	m_nSerchGroupNum = 0 ;
	// E 데이트매칭 추가 added by hseos 2008.01.07

	m_pBtnPrev = NULL ;
	m_pBtnNext = NULL ;

	m_pPageNum	 = NULL ;
	m_pSlash	 = NULL ;
	m_pTotalPage = NULL ;

	m_nGender = 0 ; // man is default.
	m_dwSearchTargetIndex = 0;
}

CDateMatchingPartnerList::~CDateMatchingPartnerList()
{
	m_pPartnerList->DeleteAllItems() ;

	PTRLISTPOS pos = m_PartnerList.GetHeadPosition() ;

	for(CSHResidentRegistManager::stSerchResult* pInfo = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetNext(pos);
		0 < pInfo;
		pInfo = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetNext(pos))
	{
		SAFE_DELETE(
			pInfo);
	}

	m_PartnerList.RemoveAll() ;
}

DWORD CDateMatchingPartnerList::ActionEvent(CMouse* mouseInfo)
{
	DWORD we = WE_NULL ;

	if( !m_bActive )		return we;
	we = cDialog::ActionEvent(mouseInfo);

	int rowidx = 0 ; 
	if(we & WE_ROWCLICK)
	{
		rowidx = m_pPartnerList->GetSelectedRowIdx();
		RITEM* pRItem = NULL;
		int nMaxCount = m_pPartnerList->GetItemCount();

		for(WORD count = 0 ; count < nMaxCount ; ++count )
		{
			pRItem = m_pPartnerList->GetRItem(count) ;

			if(pRItem)
			{
				for( int count2 = 0 ; count2 < pRItem->cols ; ++count2 )
				{
					if( rowidx == count )
					{
						pRItem->rgb[count2] = RGBA_MAKE(255,0,0,255);
					}
					else
					{
						pRItem->rgb[count2] = RGBA_MAKE(255,255,255,255);
					}
				}
			}
		}

		// setting partner info.
		int nPosNum = (m_nPage-1)*MAX_LIST_LINE+rowidx ;
		PTRLISTPOS pos = m_PartnerList.FindIndex(nPosNum) ;

		if( pos )
		{
			CSHResidentRegistManager::stSerchResult* pSelectedInfo = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetAt(pos) ;

			if( pSelectedInfo )
			{
				// setting partner info dialog.
				CDateMatchingDlg* pDlg = GAMEIN->GetDateMatchingDlg() ;
				CDateMatchingInfoDlg* pPartnerInfoDlg = GAMEIN->GetMatchPartnerInfoDlg() ; 
				if( pPartnerInfoDlg )
				{
					pPartnerInfoDlg->UpdateInfo(pSelectedInfo) ;
					if( !pPartnerInfoDlg->IsActive() )
					{
						pDlg->SetLowerDlg(e_PartnerInfoDlg) ;
					}
				}
				//m_pSelectImage->SetActive(TRUE) ;
				//m_pSelectImage->SetAbsXY(xPos, yPos) ;
			}
		}
	}
	else if(we & WE_ROWDBLCLICK )
	{
		StartChatting() ;
	}
	else if( we & WE_LBTNCLICK )
	{
		//if( lId == DMD_GENDER_CHECKBOX_MALE )
		if( m_pGenderMan->PtInWindow((LONG)mouseInfo->GetMouseEventX(), (LONG)mouseInfo->GetMouseEventY()) )
		{
			SetCheckGender(e_MATCH_MAN) ;
		}
		//else if( lId == DMD_GENDER_CHECKBOX_FEMALE )
		if( m_pGenderWoman->PtInWindow((LONG)mouseInfo->GetMouseEventX(), (LONG)mouseInfo->GetMouseEventY()) )
		{
			SetCheckGender(e_MATCH_WOMAN) ;
		}
	}

	return we ;
}

void CDateMatchingPartnerList::SetCheckGender(int byGender )
{
	if( byGender == e_MATCH_MAN )
	{
		// setting control.
		m_pGenderMan->SetChecked(TRUE) ;
		m_pGenderWoman->SetChecked(FALSE) ;

		// setting gender.
		m_nGender = e_MATCH_MAN ;
	}
	else if( byGender == e_MATCH_WOMAN )
	{
		m_pGenderWoman->SetChecked(TRUE) ;
		m_pGenderMan->SetChecked(FALSE) ;

		// setting gender.
		m_nGender = e_MATCH_WOMAN ;
	}
}

void CDateMatchingPartnerList::OnActionEvent(LONG lId, void * p, DWORD we)
{
	switch(we)
	{
	case WE_BTNCLICK :
		{
			switch(lId)
			{
			case DMD_REFRESH_BTN :
			case DMD_SEARCH_BTN :
				{
					// check age.
					if( m_pAge->GetCurSelectedIdx() <=-1 )
					{
						m_pAge->SetCurSelectedIdx(0) ;
					}

					// check region.
					if( m_pRegion->GetClickedRowIdx() <= -1 )
					{
						m_pRegion->SetClickedRowIdx( 0 );
					}

					// check search time.

					// send search syn.
					m_pPartnerList->DeleteAllItems() ;

					PTRLISTPOS pos = m_PartnerList.GetHeadPosition() ;

					for(CSHResidentRegistManager::stSerchResult* pInfo = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetNext(pos);
						0 < pInfo;
						pInfo = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetNext(pos))
					{
						SAFE_DELETE(
							pInfo);
					}

					m_PartnerList.RemoveAll() ;

					m_nPage = 0;
					m_nTotalPage = 0;
					//
					int nGender = GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->GetSearchGender() ;
					g_csResidentRegistManager.CLI_RequestSerchDateMatching(
						WORD(m_pAge->GetCurSelectedIdx()),
						WORD(m_pRegion->GetClickedRowIdx()),
						WORD(nGender));
					SettingPage(DMD_REFRESH_BTN) ;
				}
				break ;

			case DMD_START_CHAT_BTN :
				{
					StartChatting() ;
				}
				break ;

			case DMD_PREV_BTN :
				{
					// setting page.
					/*if( m_nPage > MIN_LIST_PAGE )
					{
						--m_nPage ;
					}
					else
					{
						m_nPage = MIN_LIST_PAGE ;
					}*/
					SettingPage(DMD_PREV_BTN) ;

					// setting partner list.
					ResetPartnerList() ;
				}
				break ;

			case DMD_NEXT_BTN :
				{
					SettingPage(DMD_NEXT_BTN) ;

					// setting partner list.
					ResetPartnerList() ;
				}
				break ;
			// desc_hseos_데이트매칭01
			// S 데이트매칭 추가 added by hseos 2008.01.07
			case DMD_GROUP_PREV_BTN:
			case DMD_GROUP_NEXT_BTN:
			case DMD_GROUP_10PREV_BTN:
			case DMD_GROUP_10NEXT_BTN:
				SettingPage(lId);
				break;
			// E 데이트매칭 추가 added by hseos 2008.01.07

			default :
				break ;
			}
		}
		break ;

	default :
		break ;
	}
}

void CDateMatchingPartnerList::Linking()
{
	m_pAge			= (cComboBox*)GetWindowForID(DMD_AGE_COMBO);
	m_pRegion		= (cListDialog*)GetWindowForID(DMD_REGION_COMBO);
	m_pGenderMan	= (cCheckBox*)GetWindowForID(DMD_GENDER_CHECKBOX_MALE) ;
	m_pGenderWoman	= (cCheckBox*)GetWindowForID(DMD_GENDER_CHECKBOX_FEMALE) ;

	//m_pGenderMan->SetDisable(TRUE) ;		
	//m_pGenderWoman->SetDisable(TRUE) ;

	m_pSearch		= (cButton*)GetWindowForID(DMD_SEARCH_BTN) ;
	m_pRefresh		= (cButton*)GetWindowForID(DMD_REFRESH_BTN) ;
	m_pChatting		= (cButton*)GetWindowForID(DMD_START_CHAT_BTN) ;

	m_pPartnerList	= (cListCtrl*)GetWindowForID(DMD_SEARCH_LIST) ;

	m_pList_Gender		= (cButton*)GetWindowForID(DMD_LIST_BTN_GENDER) ;
	m_pList_Age			= (cButton*)GetWindowForID(DMD_LIST_BTN_AGE) ;
	m_pList_NickName	= (cButton*)GetWindowForID(DMD_LIST_BTN_NICKNAME) ;
	m_pList_Favor		= (cButton*)GetWindowForID(DMD_LIST_BTN_FAVOR) ;
	m_pList_Point		= (cButton*)GetWindowForID(DMD_LIST_BTN_POINT) ;

	m_pBtnPrev = (cButton*)GetWindowForID(DMD_PREV_BTN) ;
	m_pBtnNext = (cButton*)GetWindowForID(DMD_NEXT_BTN) ;

	m_pPageNum		= (cStatic*)GetWindowForID(DMD_PAGE_NUM) ;
	m_pSlash		= (cStatic*)GetWindowForID(DMD_PAGE_SLASH) ;
	m_pTotalPage	= (cStatic*)GetWindowForID(DMD_PAGE_TOTAL) ;

	// desc_hseos_데이트매칭01
	// S 데이트매칭 추가 added by hseos 2008.01.07
	m_pGroupNum 		= (cStatic*)GetWindowForID(DMD_GROUP_NUM) ;
	m_pGroupSlash 		= (cStatic*)GetWindowForID(DMD_GROUP_SLASH) ;
	m_pGroupTotalPage	= (cStatic*)GetWindowForID(DMD_GROUP_TOTAL) ;
	// E 데이트매칭 추가 added by hseos 2008.01.07
}

void CDateMatchingPartnerList::SettingControls()
{
 	CSHResidentRegistManager::stADDInfo* pInfo = g_csResidentRegistManager.GetADDInfo() ;

	m_nPage = 0;
	m_nTotalPage = 0;
	m_nSerchGroupIndex = 0;
	m_nSerchGroupNum = 0;

	int nMaxAgeCount = pInfo->m_nMaxSerchAgeRange;
	m_pAge->SetMaxLine(
		WORD(nMaxAgeCount));

	// 090923 ShinJS --- Buffer Size 초과로 인하여 Error 발생. ITEM의 string만큼 크기를 늘려줌
	char tempBut[256] = {0, } ;

	for( int count = 0 ; count < nMaxAgeCount ; ++count )
	{
		ITEM* pItem = new ITEM ;
		memset(tempBut, 0, sizeof(tempBut)) ;
		pItem->line = count ;
		pItem->rgb = RGB(255, 255, 255) ;
		if( count == 0 )
		{
			// 090316 NYJ - 하드코딩을 InterfaceMsg.bin 에서 읽어오도록 변경
			strcpy(tempBut, RESRCMGR->GetMsg(1080)) ;
			//strcpy(tempBut, "상관없음") ;
		}
		else
		{
			sprintf(tempBut, "%d - %d", pInfo->m_pTblSerchAgeRange[count].nMin, pInfo->m_pTblSerchAgeRange[count].nMax) ;
		}
		strcpy(pItem->string, tempBut) ;
		m_pAge->AddItem(pItem) ;
	}
	//m_pAge->SelectComboText(0) ;
	m_pAge->SetCurSelectedIdx(0) ;
	m_pAge->SelectComboText(0) ;

	// setting combobox of region part.
	int nMaxRegionCount = pInfo->m_nMaxLocation ;
	m_pRegion->RemoveAll();

	for( int count = 0 ; count < nMaxRegionCount ; ++count )
	{
		m_pRegion->AddItem(
			pInfo->m_pTblLocation[count].szName,
			RGB( 255, 255, 255 ) );
	}
	
	m_pRegion->SetClickedRowIdx( 0 );

	// setting checkbox of gender part.
	const DATE_MATCHING_INFO& dateInfo = HERO->GetDateMatchingInfo() ;

	if( dateInfo.byGender == 0 )
	{
		m_pGenderMan->SetChecked(FALSE) ;
		m_pGenderWoman->SetChecked(TRUE) ;

		// setting gender to search partner.
		m_nGender = e_MATCH_WOMAN ;
	}
	else
	{
		m_pGenderMan->SetChecked(TRUE) ;
		m_pGenderWoman->SetChecked(FALSE) ;

		// setting gender to search partner.
		m_nGender = e_MATCH_MAN ;
	}

	// setting list.
	m_pPartnerList->DeleteAllItems() ;
	m_PartnerList.RemoveAll();

	// setting page.
	SettingPage(0) ;

	//// Test Code.
	//for( int count = 0 ; count < 80 ; ++count )
	//{
	//	UpdateInfo(count) ;
	//}
}

void CDateMatchingPartnerList::UpdateInfo(CSHResidentRegistManager::stSerchResult* pResult)

{
	if( !pResult ) return ;

	m_nPage = 1;
	char szTemp[128]  = {0, } ;
	char szGender[128] = {0, } ;
	char szNickName[32] = {0, } ;
	int nAge   = 0;
	int nPoint = 0 ;
	int dwClass = 0;

	CSHResidentRegistManager::stSerchResult* pPartnerInfo = new CSHResidentRegistManager::stSerchResult ;
	memcpy(pPartnerInfo, pResult, sizeof(CSHResidentRegistManager::stSerchResult)) ;

	// Sorting Part.
	PTRLISTPOS pos = m_PartnerList.GetHeadPosition() ;
	PTRLISTPOS beforePos = pos;
	BOOL bInserted = FALSE;

	for(CSHResidentRegistManager::stSerchResult* pItem = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetNext(pos);
		0 < pItem;
		pItem = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetNext(pos))
	{
		if( pPartnerInfo->nMatchingPoint > pItem->nMatchingPoint )
		{
			m_PartnerList.InsertBefore(beforePos, pPartnerInfo) ;
			bInserted = TRUE;
			break;
		}
		beforePos = pos;
	}

	if (bInserted == FALSE)
	{
		m_PartnerList.AddTail(pPartnerInfo) ;
	}

	SettingPage(0) ;

	pos = m_PartnerList.GetHeadPosition() ;
	CSHResidentRegistManager::stSerchResult* pInfo = NULL ;

	// Clear Partner List.
	m_pPartnerList->DeleteAllItems() ;	

	int nSearchTargetPos = -1;
	for(WORD count = 0 ; count < MAX_LIST_LINE ; ++count )
	{
		pos = m_PartnerList.FindIndex(count) ;
		if(pos)
		{
			pInfo = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetAt(pos) ;

			if( pInfo )
			{
				// setting gender.
				if(pInfo->stRegistInfo.nSex == 0)
				{
					strcpy(szGender, RESRCMGR->GetMsg(508)) ;
				}
				else
				{
					strcpy(szGender, RESRCMGR->GetMsg(509)) ;
				}

				// setting age.
				nAge = pInfo->stRegistInfo.nAge ;

				// setting nick name.
				strcpy(szNickName, pInfo->stRegistInfo.szNickName) ;

				// setting class.
				dwClass = pInfo->nClass;

				// setting matching point.
				nPoint = pInfo->nMatchingPoint ;

				// copy all info to temp.

				// setting item.
				cRITEMEx* pItem = new cRITEMEx(6) ;

				sprintf(szTemp, "%s", szGender) ;
				strcpy(pItem->pString[0], szTemp) ;

				sprintf(szTemp, "%d", nAge) ;
				strcpy(pItem->pString[1], szTemp) ;

				sprintf(szTemp, "%s", szNickName) ;
				strcpy(pItem->pString[2], szTemp) ;

				// 100106 ONS 호감도를 클래스로 변경
				sprintf(szTemp, "%s", RESRCMGR->GetMsg(RESRCMGR->GetClassNameNum( dwClass ))) ;
				strcpy(pItem->pString[3], szTemp) ;

				sprintf(szTemp, "%d", nPoint) ;
				strcpy(pItem->pString[4], szTemp) ;

				sprintf(szTemp, "%d", pInfo->nID);
				strcpy(pItem->pString[5], szTemp);

				if(m_dwSearchTargetIndex == DWORD(atoi(pItem->pString[5])))
				{
					nSearchTargetPos = count;
				}

				pItem->rgb[0] = RGB(255, 255, 255) ;
				pItem->rgb[1] = RGB(255, 255, 255) ;

				pItem->dwID = count ;

				m_pPartnerList->InsertItem(count, pItem) ;
			}
		}
	}

	if(nSearchTargetPos >= 0)
	{
		m_pPartnerList->SetSelectedRowIdx(nSearchTargetPos);

		int nPosNum = (m_nPage-1)*MAX_LIST_LINE+nSearchTargetPos ;
		PTRLISTPOS pos = m_PartnerList.FindIndex(nPosNum) ;

		if( pos )
		{
			CSHResidentRegistManager::stSerchResult* pSelectedInfo = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetAt(pos) ;

			if( pSelectedInfo )
			{
				CDateMatchingDlg* pDlg = GAMEIN->GetDateMatchingDlg() ;
				CDateMatchingInfoDlg* pPartnerInfoDlg = GAMEIN->GetMatchPartnerInfoDlg() ; 
				if( pPartnerInfoDlg )
				{
					pPartnerInfoDlg->UpdateInfo(pSelectedInfo) ;
					if( !pPartnerInfoDlg->IsActive() )
					{
						pDlg->SetLowerDlg(e_PartnerInfoDlg) ;
					}
				}
			}
		}

		// 대화요청 하자~!
		StartChatting();

		m_dwSearchTargetIndex = 0;
	}
}

void CDateMatchingPartnerList::ResetPartnerList()
{
	// Check Page Count.
	if( m_nPage > m_nTotalPage ) return ;
	char szTemp[128]  = {0, } ;
	char szGender[128] = {0, } ;
	char szNickName[32] = {0, } ;
	int nAge   = 0;
	int nPoint = 0 ;
	int dwClass = 0;

	// remove all item of current list.
	m_pPartnerList->DeleteAllItems() ;

	PTRLISTPOS pos = NULL ;
	CSHResidentRegistManager::stSerchResult* pInfo = NULL ;

	for(WORD count = 0 ; count < MAX_LIST_LINE ; ++count )
	{
		pos = m_PartnerList.FindIndex((m_nPage-1)*5+count) ;
		if(pos)
		{
			pInfo = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetAt(pos) ;

			if( pInfo )
			{
				// setting gender.
				if(pInfo->stRegistInfo.nSex == 0)
				{
					strcpy(szGender, RESRCMGR->GetMsg(508)) ;
				}
				else
				{
					strcpy(szGender, RESRCMGR->GetMsg(509)) ;
				}

				// setting age.
				nAge = pInfo->stRegistInfo.nAge ;

				// setting nick name.
				strcpy(szNickName, pInfo->stRegistInfo.szNickName) ;

				// setting class.
				dwClass = pInfo->nClass;

				// setting matching point.
				nPoint = pInfo->nMatchingPoint ;

				// copy all info to temp.

				// setting item.
				cRITEMEx* pItem = new cRITEMEx(5) ;

				sprintf(szTemp, "%s", szGender) ;
				strcpy(pItem->pString[0], szTemp) ;

				sprintf(szTemp, "%d", nAge) ;
				strcpy(pItem->pString[1], szTemp) ;

				sprintf(szTemp, "%s", szNickName) ;
				strcpy(pItem->pString[2], szTemp) ;

				// 100106 ONS 호감도를 클래스로 변경
				sprintf(szTemp, "%s", RESRCMGR->GetMsg(RESRCMGR->GetClassNameNum( dwClass ))) ;
				strcpy(pItem->pString[3], szTemp) ;

				sprintf(szTemp, "%d", nPoint) ;
				strcpy(pItem->pString[4], szTemp) ;

				pItem->rgb[0] = RGB(255, 255, 255) ;
				pItem->rgb[1] = RGB(255, 255, 255) ;

				pItem->dwID = count ;

				m_pPartnerList->InsertItem(count, pItem) ;
			}
		}
	}	
}

void CDateMatchingPartnerList::StartChatting()
{
	// check selected partner.
	// 071207 LYW ---- DateMatchingDlg : 매칭 1:1 선택하는 사람, 페이지 적용.
	int rowidx = m_pPartnerList->GetSelectedRowIdx()+(m_nPage-1)*MAX_LIST_LINE ;	

	CSHResidentRegistManager::stSerchResult* pPartnerInfo = NULL ;

	PTRLISTPOS pos = m_PartnerList.FindIndex(rowidx) ;

	if( pos )
	{
		pPartnerInfo = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetAt(pos) ;

		if( pPartnerInfo->nID )
		{
			if( pPartnerInfo->nID == gHeroID )
			{
				// notice error.
				WINDOWMGR->MsgBox( MBI_MATTINGDLG_ERROR, MBT_OK, CHATMGR->GetChatMsg( 1313 ) );
				return ;
			}
			else if( pPartnerInfo->stRegistInfo.nSex == HERO->GetGender())
			{
				// notice error.
				WINDOWMGR->MsgBox( MBI_MATTINGDLG_ERROR, MBT_OK, CHATMGR->GetChatMsg( 2280 ) );
				return ;
			}
		
			g_csResidentRegistManager.CLI_RequestDateMatchingChat(pPartnerInfo) ;
			return ;
		}
	}

	// notice error.
	WINDOWMGR->MsgBox( MBI_MATCHAT_SELECT_PARTNER, MBT_OK, CHATMGR->GetChatMsg( 1314 ) );

	// notice wating msg.

	// start check time.
}

void CDateMatchingPartnerList::SettingPage(LONG lId)
{
 	int nPartnerCount =	m_PartnerList.GetCount() ;			// Get Partner Count Of Partner List.
	
	m_nTotalPage = (nPartnerCount/MAX_LIST_LINE) ;			// Get Count Of Total Page.
	if (nPartnerCount%MAX_LIST_LINE) m_nTotalPage++;

	switch(lId)
	{
	case DMD_PREV_BTN :
		{
			if( m_nPage <= MIN_LIST_PAGE ) return ;			// Check Page Count.
			--m_nPage;
		}
		break ;

	case DMD_NEXT_BTN :
		{
			if( m_nPage >= m_nTotalPage ) return ;			// Check Page Count.
			++m_nPage;
		}
		break ;

	// desc_hseos_데이트매칭01
	// S 데이트매칭 추가 added by hseos 2008.01.07
	case DMD_GROUP_PREV_BTN :
		{
			int nGender = GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->GetSearchGender() ;
			g_csResidentRegistManager.CLI_RequestSerchDateMatching(
				WORD(m_pAge->GetCurSelectedIdx()),
				WORD(m_pRegion->GetClickedRowIdx()),
				WORD(nGender),
				-1);
		}
		break ;

	case DMD_GROUP_NEXT_BTN :
		{
			int nGender = GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->GetSearchGender() ;
			g_csResidentRegistManager.CLI_RequestSerchDateMatching(
				WORD(m_pAge->GetCurSelectedIdx()),
				WORD(m_pRegion->GetClickedRowIdx()),
				WORD(nGender),
				1);
		}
		break ;
	case DMD_GROUP_10PREV_BTN :
		{
			int nGender = GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->GetSearchGender() ;
			g_csResidentRegistManager.CLI_RequestSerchDateMatching(
				WORD(m_pAge->GetCurSelectedIdx()),
				WORD(m_pRegion->GetClickedRowIdx()),
				WORD(nGender),
				-10);
		}
		break ;

	case DMD_GROUP_10NEXT_BTN :
		{
			int nGender = GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->GetSearchGender() ;
			g_csResidentRegistManager.CLI_RequestSerchDateMatching(
				WORD(m_pAge->GetCurSelectedIdx()),
				WORD(m_pRegion->GetClickedRowIdx()),
				WORD(nGender),
				10);
		}
		break ;
	}

	char szTempBuf[12] = {0, } ;

	sprintf(szTempBuf, "%d", m_nTotalPage) ;
	m_pTotalPage->SetStaticText(szTempBuf) ;

	ZeroMemory(szTempBuf, sizeof(szTempBuf)) ;
    sprintf(szTempBuf, "/") ;
	m_pSlash->SetStaticText(szTempBuf) ;

	ZeroMemory(szTempBuf, sizeof(szTempBuf)) ;
	sprintf(szTempBuf, "%d", m_nPage) ;
	m_pPageNum->SetStaticText(szTempBuf) ;	

	// desc_hseos_데이트매칭01
	// S 데이트매칭 추가 added by hseos 2008.01.07
	UpdateGroupInfo();
	// E 데이트매칭 추가 added by hseos 2008.01.07
}

// desc_hseos_데이트매칭01
// S 데이트매칭 추가 added by hseos 2008.01.07
void CDateMatchingPartnerList::UpdateGroupInfo()
{
	char szTempBuf[12] = {0, } ;

	sprintf(szTempBuf, "%d", m_nSerchGroupNum) ;
	m_pGroupTotalPage->SetStaticText(szTempBuf) ;

	ZeroMemory(szTempBuf, sizeof(szTempBuf)) ;
     sprintf(szTempBuf, "/") ;
	m_pGroupSlash->SetStaticText(szTempBuf) ;

	ZeroMemory(szTempBuf, sizeof(szTempBuf)) ;
	sprintf(szTempBuf, "%d", m_nSerchGroupIndex+1) ;
 	m_pGroupNum->SetStaticText(szTempBuf) ;	
	if (m_nSerchGroupNum <= 0)
	{
 		m_pGroupNum->SetStaticText("0") ;	
	}
}

void CDateMatchingPartnerList::ClearPartnerList()
{
	// check age.
	if( m_pAge->GetCurSelectedIdx() <=-1 )
	{
		m_pAge->SetCurSelectedIdx(0) ;
	}

	if( m_pRegion->GetClickedRowIdx() <= -1 )
	{
		m_pRegion->SetClickedRowIdx( 0 );
	}

	// check search time.

	// send search syn.
	m_pPartnerList->DeleteAllItems() ;

	PTRLISTPOS pos = m_PartnerList.GetHeadPosition() ;

	for(CSHResidentRegistManager::stSerchResult* pInfo = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetNext(pos);
		0 < pInfo;
		pInfo = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetNext(pos))
	{
		SAFE_DELETE(
			pInfo);
	}

	m_PartnerList.RemoveAll() ;

	m_nPage = 0;
	m_nTotalPage = 0;

	SettingPage(DMD_REFRESH_BTN) ;
}
// E 데이트매칭 추가 added by hseos 2008.01.07

void CDateMatchingPartnerList::OpenDlgForQuickDate(DWORD dwTargetIndex)
{
	// check age.
	if( m_pAge->GetCurSelectedIdx() <=-1 )
	{
		m_pAge->SetCurSelectedIdx(0) ;
	}

	// check region.
	if( m_pRegion->GetClickedRowIdx() <= -1 )
	{
		m_pRegion->SetClickedRowIdx( 0 );
	}

	// check search time.

	// send search syn.
	m_pPartnerList->DeleteAllItems() ;

	PTRLISTPOS pos = m_PartnerList.GetHeadPosition() ;

	for(CSHResidentRegistManager::stSerchResult* pInfo = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetNext(pos);
		0 < pInfo;
		pInfo = (CSHResidentRegistManager::stSerchResult*)m_PartnerList.GetNext(pos))
	{
		SAFE_DELETE(
			pInfo);
	}

	m_PartnerList.RemoveAll() ;

	m_nPage = 0;
	m_nTotalPage = 0;

	m_dwSearchTargetIndex = dwTargetIndex;


	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(dwTargetIndex);
	if(! pPlayer)
		return;

	if(pPlayer->GetGender() == HERO->GetGender())
	{
		WINDOWMGR->MsgBox( MBI_MATTINGDLG_ERROR, MBT_OK, CHATMGR->GetChatMsg( 2280 ) );
		return;
	}

	int nGender = !HERO->GetGender();
	g_csResidentRegistManager.CLI_RequestSerchDateMatching(
		WORD(m_pAge->GetCurSelectedIdx()),
		WORD(m_pRegion->GetClickedRowIdx()),
		WORD(nGender));
	SettingPage(DMD_REFRESH_BTN) ;
}









CDateMatchingChatDlg::CDateMatchingChatDlg() 
{
	m_type = WT_MATCHCHATDLG ;

	m_pChatList	= NULL ;

	m_pInputBox = NULL ;

	m_bChatOnOff = FALSE ;
}

CDateMatchingChatDlg::~CDateMatchingChatDlg()
{
}

void CDateMatchingChatDlg::AddMsg(char* msg, DWORD dwColor)
{
	char buf[ MAX_MATCH_CHAT_MSG ] = {0, } ;

#ifdef _TL_LOCAL_
	int nLen = strlen( msg );

	BYTE brk[512];
	int nBrkNum = g_TBreak.FindThaiWordBreak( msg, nLen, brk, 512, FTWB_SEPARATESYMBOL );
	int nCutPos = 0;

	int nCount = 0;

	for( int i = 0 ; i < nBrkNum ; ++i )
	{
		++nCount;	//몇단어썼나
		nCutPos += brk[i];
		if( CFONT_OBJ->GetTextExtentEx(0, msg, nCutPos ) > MAX_MATCH_CHAT_MSG_EXTENT )
		{
			if( nCount == 1 )	//한단어만 썼는데 넘어가버리면 강제로 끊는다.
			{
				for( int k = 1 ; k <= nCutPos ; ++k )
				{
					if( CFONT_OBJ->GetTextExtentEx(0, msg, k ) > MAX_MATCH_CHAT_MSG_EXTENT )
					{
						nCutPos = nCutPos - brk[i] + k-1; //쓴 글자까지만
						strncpy( buf, msg, nCutPos );	//전단계까지
						buf[nCutPos] = 0;
						m_pChatList->AddItem(buf, dwColor);
						msg += nCutPos;
						brk[i] -= nCutPos;	//나머지부분 계산
						nCutPos = 0;
						nCount = 0; //초기화
						--i;
						break;
					}
				}
			}
			else
			{
				nCutPos -= brk[i];
				strncpy( buf, msg, nCutPos );	//전단계까지
				buf[nCutPos] = 0;
				m_pChatList->AddItem(buf, dwColor);
				--i;
				msg += nCutPos;
				nCutPos = 0;
				nCount = 0; //초기화
			}

			if( *msg == ' ' )
			{
				++msg;	//라인 첫글자가 스페이스가 아니도록...
				++i;
			}
		}
	}
#else
	int nLen	= strlen( msg ) ;
	int nCpyNum = 0 ;

	while( nLen > MAX_MATCH_CHAT_MSG )
	{
		if( (msg + MAX_MATCH_CHAT_MSG ) != CharNext( CharPrev( msg, msg + MAX_MATCH_CHAT_MSG ) ) )
		{
			nCpyNum = MAX_MATCH_CHAT_MSG - 1 ;
		}
		else
		{
			nCpyNum = MAX_MATCH_CHAT_MSG ;
		}

		strncpy(buf, msg, nCpyNum) ;
		buf[ MAX_MATCH_CHAT_MSG-1 ] = 0 ;
		m_pChatList->AddItem(buf, dwColor) ;

		nLen -= MAX_MATCH_CHAT_MSG ;
		msg  += nCpyNum ;

		if( *msg == ' ' ) ++msg ;
	}
#endif

	if( nLen > 0 )
	{
		m_pChatList->AddItem( msg, dwColor ) ;
	}
}








CDateMatchingInfoDlg::CDateMatchingInfoDlg()
{
	m_type = WT_MATCHINFODLG ;

	m_pNickName		= NULL ;
	m_pGender		= NULL ;
	m_pAge			= NULL ;
	m_pRegion		= NULL ;
	m_pFavor		= NULL ;

	m_pGoodIcon		= NULL ;
	m_pBadIcon		= NULL ;

	m_pIntroduce	= NULL ;

	m_pLeftBtn		= NULL ;
	m_pRightBtn		= NULL ;

	m_pIntroduce_up		= NULL ;
	m_pIntroduce_down	= NULL ;
	m_pIntroduce_guage	= NULL ;

	int nCount = 0 ;
	for(nCount = 0 ; nCount < MAX_GOOD_ICON_COUNT ; ++nCount )
	{
		m_pGoodItem[nCount] = NULL ;
	}

	for(nCount = 0 ; nCount < MAX_BAD_ICON_COUNT ; ++nCount )
	{
		m_pBadItem[nCount] = NULL ;
	}
}

CDateMatchingInfoDlg::~CDateMatchingInfoDlg()
{}

void CDateMatchingInfoDlg::SetGoodIcon(int nPos, DWORD dwIconNum)
{
	if( nPos < MAX_GOOD_ICON_COUNT )
	{
		PTRLISTPOS pos = m_FavorIconList.FindIndex(dwIconNum) ;

		if( pos )
		{
			CFavorItem* pAddItem = (CFavorItem*)m_FavorIconList.GetAt(pos) ;

			if( pAddItem )
			{
				cImage imgToolTip;
				SCRIPTMGR->GetImage( 0, &imgToolTip, PFT_HARDPATH );
				pAddItem->SetToolTip( pAddItem->GetItemName(), RGB(0, 255, 255), &imgToolTip);
				pAddItem->AddToolTipLine(RESRCMGR->GetMsg(848), RGB(255, 255, 255)) ;

				m_pGoodItem[nPos] = pAddItem ;
				m_pGoodItem[nPos]->SetActive(TRUE) ;
				m_pGoodItem[nPos]->SetDepend(TRUE) ;
				m_pGoodItem[nPos]->SetParent(this) ;

				m_pGoodIcon->m_pIconGridCell[nPos].use = FALSE ;

				m_pGoodIcon->AddIcon(
					WORD(nPos),
					m_pGoodItem[nPos]) ;
			}
		}
	}
}

void CDateMatchingInfoDlg::SetBadIcon(int nPos, DWORD dwIconNum)
{
	if( nPos < MAX_GOOD_ICON_COUNT )
	{
		PTRLISTPOS pos = m_FavorIconList.FindIndex(dwIconNum) ;

		if( pos )
		{
			CFavorItem* pAddItem = (CFavorItem*)m_FavorIconList.GetAt(pos) ;

			if( pAddItem )
			{
				cImage imgToolTip;
				SCRIPTMGR->GetImage( 0, &imgToolTip, PFT_HARDPATH );
				pAddItem->SetToolTip( pAddItem->GetItemName(), RGB(0, 255, 255), &imgToolTip);
				pAddItem->AddToolTipLine(RESRCMGR->GetMsg(849), RGB(255, 255, 255)) ;

				m_pBadItem[nPos] = pAddItem ;
				m_pBadItem[nPos]->SetActive(TRUE) ;
				m_pBadItem[nPos]->SetDepend(TRUE) ;
				m_pBadItem[nPos]->SetParent(this) ;

				m_pBadIcon->m_pIconGridCell[nPos].use = FALSE ;

				m_pBadIcon->AddIcon(
					WORD(nPos),
					m_pBadItem[nPos]) ;
			}
		}
	}
}

BOOL CDateMatchingInfoDlg::UpdateInfo(CSHResidentRegistManager::stSerchResult* pInfo)
{
	if(!pInfo) return FALSE ;

	// check racial.
	int nRacial = (int)pInfo->stRegistInfo.nRace ;
	int nFaceNum = 0 ;

	/*if( pPartner )
	{
		nRacial = pPartner->GetCharacterTotalInfo()->Race ;
	}*/
	/*else
	{
		WINDOWMGR->MsgBox( MBI_MATTINGDLG_ERROR, MBT_OK, "로그아웃 된 유저입니다." );
		return FALSE ;
	}*/

	CSHResidentRegistManager::stRegistInfo* pRegistInfo = &pInfo->stRegistInfo ;

	if( !pRegistInfo ) return FALSE ;

	char tempBuf[12] = {0, } ;

	// setting nickname.
	m_pNickName->SetStaticText(pRegistInfo->szNickName) ;

	// setting gender.
	if( pRegistInfo->nSex == 0 )
	{
		m_pGender->SetStaticText(RESRCMGR->GetMsg(508)) ;
	}
	else
	{
		m_pGender->SetStaticText(RESRCMGR->GetMsg(509)) ;
	}

	// 090521 ONS 신규종족 데이트매칭관련 처리 추가
	// setting face num.
	if( nRacial == e_MATCH_HUMAN )
	{
		// setting human.
		if( pRegistInfo->nSex == e_MATCH_MAN)
		{
			nFaceNum = e_MATCH_HUMAN_MAN ;
		}
		else
		{
			nFaceNum = e_MATCH_HUMAN_WOMAN ;
		}
	}
	else if( nRacial == e_MATCH_ELF )
	{
		// setting elf.
		if( pRegistInfo->nSex == e_MATCH_MAN)
		{
			nFaceNum = e_MATCH_ELF_MAN ;
		}
		else
		{
			nFaceNum = e_MATCH_ELF_WOMAN ;
		}
	}
	else
	{
		// setting devil.
		if( pRegistInfo->nSex == e_MATCH_MAN)
		{
			nFaceNum = e_MATCH_DEVIL_MAN ;
		}
		else
		{
			nFaceNum = e_MATCH_DEVIL_WOMAN ;
		}
	}

	// setting face image.
	SCRIPTMGR->GetImage( nFaceNum, &m_FacePicture, PFT_HARDPATH ) ;

	// setting age.
	memset(tempBuf, 0, sizeof(tempBuf)) ;
	sprintf(tempBuf, "%d", pRegistInfo->nAge) ;
	m_pAge->SetStaticText(tempBuf) ;

	// setting region.
	CSHResidentRegistManager::stADDInfo* pRegionInfo = g_csResidentRegistManager.GetADDInfo() ;
	m_pRegion->SetStaticText(pRegionInfo->m_pTblLocation[pRegistInfo->nLocation].szName) ;

	// 100106 ONS 호감도를 매칭포인트으로 변경
	memset(tempBuf, 0, sizeof(tempBuf)) ;
	sprintf(tempBuf, "%d", pInfo->nMatchingPoint) ;
	m_pFavor->SetStaticText(tempBuf) ;

	// setting good icon.
	SetGoodIcon(0, pRegistInfo->nPropensityLike01-1) ;
	SetGoodIcon(1, pRegistInfo->nPropensityLike02-1) ;
	SetGoodIcon(2, pRegistInfo->nPropensityLike03-1) ;

	// setting bad icon.
	SetBadIcon(0, pRegistInfo->nPropensityDisLike01-1) ;
	SetBadIcon(1, pRegistInfo->nPropensityDisLike02-1) ;
	
	// setting introduce.
	m_pIntroduce->SetScriptText(pInfo->szIntroduction) ;
	m_pIntroduce->SetReadOnly(TRUE) ;

	return TRUE ;
}








void CDateMatchingInfoDlg::LoadFavorItem()
{
	// read favor item list.
	CMHFile	file ;

	file.Init( FILE_FAVOR_ITEM_LIST, "rb" ) ;

	if( file.IsInited() == FALSE )	return ; // 추후 ASSERT 처리 할 것.

	m_FavorIconList.GetHeadPosition() ;

	cImage tmpImage ;
	CFavorItem* pItem = NULL ;

	while(1)
	{
		if( file.IsEOF() != FALSE ) break ; 

		pItem = new CFavorItem ;

		pItem->SetItemIdx( file.GetDword() ) ;
		pItem->SetItemName( file.GetString() ) ;

		// setting tooltip.
		pItem->SetToolTipNum( file.GetDword() ) ;
		cImage imgToolTip;
		SCRIPTMGR->GetImage( 0, &imgToolTip, PFT_HARDPATH );		
		//pItem->SetToolTip( pItem->GetItemName(), RGB(0, 255, 255), &imgToolTip);
		//pItem->AddToolTipLine("") ;
		//pItem->AddToolTipLine(RESRCMGR->GetMsg(pItem->GetToolTipNum()), RGB(255, 255, 255)) ;

		pItem->SetImageNum( file.GetWord() ) ;
		pItem->SetPos( file.GetWord() ) ;

		pItem->Init(0, 0, 26, 26, GAMEIN->GetFavorIconDlg()->GetIconImage(pItem->GetImageNum(), &tmpImage), m_IconIndexCreator.GenerateIndex()) ;
		pItem->SetMovable(FALSE) ;
		WINDOWMGR->AddWindow(pItem);
		pItem->SetDepend(TRUE) ;
		pItem->SetParent(this);

		m_FavorIconList.AddTail(pItem) ;
	}

	file.Release() ;
}

void CDateMatchingInfoDlg::Render()
{
	if( !IsActive() ) return ;
	cDialog::RenderWindow();
	cDialog::RenderComponent();

	ToolTipRender();

	// render face.
	VECTOR2 scale;
	scale.x = 1;
	scale.y = 1;

	VECTOR2 vPos;
	vPos.x = (float)(GetAbsX() + 9) ;
	vPos.y = (float)(GetAbsY() + 15) ;

	DWORD dwColor = 0xffffffff ;

	m_FacePicture.RenderSprite(&scale, NULL, 0, &vPos, dwColor) ;
}














CDateMatchingRecordDlg::CDateMatchingRecordDlg()
{
	m_type = WT_MATCHRECORDDLG ;

	m_pDateRecord = NULL ;

	m_pPaperBtn	  = NULL ;

	m_pList_NickName = NULL ;
	m_pList_LastDate = NULL ;
	m_pList_Point	 = NULL ;
}

CDateMatchingRecordDlg::~CDateMatchingRecordDlg() 
{
}

void CDateMatchingRecordDlg::Linking()
{
	m_pDateRecord	 = (cListCtrl*)GetWindowForID(DMD_DATE_LIST) ;

	m_pPaperBtn		 = (cButton*)GetWindowForID(DMD_SEND_MESSAGE_BTN) ;

	m_pList_NickName = (cButton*)GetWindowForID(DMD_RECORD_LIST_NICKNAME) ;
	m_pList_LastDate = (cButton*)GetWindowForID(DMD_RECORD_LIST_LASTDATE) ;
	m_pList_Point	 = (cButton*)GetWindowForID(DMD_RECORD_LIST_POINT) ;
}

CDateMatchingDlg::CDateMatchingDlg(void)
{
	m_pPartnerList		= NULL ;
	m_pChatDlg			= NULL ;

	m_pMyInfo			= NULL ;
	m_pPartnerInfo		= NULL ;

	m_pDateRecord		= NULL ;

	m_pBtnPartnerList	= NULL ;
	m_pBtnChatDlg		= NULL ;

	m_pBtnMyInfo		= NULL ;
	m_pBtnPartnerInfo	= NULL ;
	m_pBtnDateRecord	= NULL ;

	m_nSelectedPatner = - 1 ;
}

CDateMatchingDlg::~CDateMatchingDlg(void)
{
	// 071213 LYW --- CDateMatchingDlg : 인덱스 제네레이터 해제 처리 추가.
	m_pMyInfo->GetIndexCreator()->Release() ;
}

void CDateMatchingDlg::Linking()
{
	m_pPartnerList		= (CDateMatchingPartnerList*)GetWindowForID(DMD_PARTNERLISTDLG) ;
	LinkPartnerList() ;

	m_pChatDlg			= (CDateMatchingChatDlg*)GetWindowForID(DMD_CHATDLG) ;
	LinkChatDlg() ;

	m_pMyInfo			= (CDateMatchingInfoDlg*)GetWindowForID(DMD_MYINFODLG) ;
	LinkMyInfoDlg() ;

	m_pPartnerInfo		= (CDateMatchingInfoDlg*)GetWindowForID(DMD_PARTNERINFODLG) ;
	LinkPartnerInfo() ;

	m_pDateRecord		= (CDateMatchingRecordDlg*)GetWindowForID(DMD_RECORDDLG) ;
	LinkRecordDlg() ;

	m_pBtnPartnerList	= (cPushupButton*)GetWindowForID(DMD_UPPERTAB1) ;
	m_pBtnChatDlg		= (cPushupButton*)GetWindowForID(DMD_UPPERTAB2) ;

	m_pBtnMyInfo		= (cPushupButton*)GetWindowForID(DMD_LOWERTAB1) ;
	m_pBtnPartnerInfo	= (cPushupButton*)GetWindowForID(DMD_LOWERTAB2) ;
	m_pBtnDateRecord	= (cPushupButton*)GetWindowForID(DMD_LOWERTAB3) ;

	// desc_hseos_데이트존_01
	// S 데이트존 추가 added by hseos 2007.11.13 2007.11.15
	((cButton*)GetWindowForID(DMD_ENTER_DATEZONE_BTN))->SetDisable(TRUE);
	((cButton*)GetWindowForID(DMD_ENTER_DATEZONE_BTN))->SetImageRGB(RGB(190,190,190));
	((cButton*)GetWindowForID(DMD_ENTER_CHALLENGEZONE_BTN))->SetDisable(TRUE);
	((cButton*)GetWindowForID(DMD_ENTER_CHALLENGEZONE_BTN))->SetImageRGB(RGB(190,190,190));
	// E 데이트존 추가 added by hseos 2007.11.13 2007.11.15

	// desc_hseos_데이트존_01
	// S 데이트존 추가 added by hseos 2008.01.08
	m_pTitle			= (cStatic*)GetWindowForID(DMD_TITLE) ;
	//..타이틀 텍스트에 %d가 들어가 있고 게임중에 값을 설정하므로 %d가 포함된 초기 텍스트를 저장해 두어야 함.
	strcpy(m_szTitle, m_pTitle->GetStaticText());
	// E 데이트존 추가 added by hseos 2008.01.08

	// setting upper dialog.
	SetUpperDlg(e_PartnerListDlg) ;

	// setting lower dialog.
	SetLowerDlg(e_MyInfoDlg) ;
}

DWORD CDateMatchingDlg::ActionEvent(CMouse* mouseInfo)
{
	DWORD we = WE_NULL ;

	if( !m_bActive )		return we;
	we = cDialog::ActionEvent(mouseInfo);

	if( we & WE_PUSHDOWN )
	{
		if( m_pBtnPartnerList->PtInWindow((LONG)mouseInfo->GetMouseEventX(), (LONG)mouseInfo->GetMouseEventY()) )
		{
			// check chatting.
			CDateMatchingChatDlg* pChatDlg = GAMEIN->GetDateMatchingDlg()->GetChatingDlg() ;
			if( pChatDlg )
			{
				if( pChatDlg->IsOnChatMode() )
				{
					// notice end chatting.
					WINDOWMGR->MsgBox( MBI_END_CHATTING_PARTNERLIST, MBT_YESNO, CHATMGR->GetChatMsg( 1304 ) );
				}
				else
				{
					SetUpperDlg(e_PartnerListDlg) ;	
				}
			}
		}
	}

	return we ;
}


void CDateMatchingDlg::OnActionEvent(LONG lId, void * p, DWORD we)
{
	switch(we)
	{
	case WE_PUSHDOWN :
		{
			switch(lId)
			{
			case DMD_UPPERTAB2 : SetUpperDlg(e_ChatDlg) ;			break ;
			case DMD_LOWERTAB1 : SetLowerDlg(e_MyInfoDlg) ;			break ;
			case DMD_LOWERTAB2 : SetLowerDlg(e_PartnerInfoDlg) ;	break ;
			case DMD_LOWERTAB3 : SetLowerDlg(e_DateRecordDlg) ;		break ;
			}
		}
		break ;

	case WE_BTNCLICK :
		{
			switch(lId)
			{
			case DMD_CLOSEBTN :
				{
					// check chatting.
					CDateMatchingChatDlg* pChatDlg = GAMEIN->GetDateMatchingDlg()->GetChatingDlg() ;
					if( pChatDlg )
					{
						if( pChatDlg->IsOnChatMode() )
						{
							// notice end chatting.
							WINDOWMGR->MsgBox( MBI_END_CHATTING_CLOSE, MBT_YESNO, CHATMGR->GetChatMsg( 1304 ) );
						}
						else
						{
							SetActive(FALSE) ;
						}
					}
					
				}
				break ;
			case DMD_ENTER_CHALLENGEZONE_BTN:
				{
					GAMEIN->GetChallengeZoneListDlg()->SetActive(TRUE);
				}
				break;
			}
		}
		break ;
	}
}

void CDateMatchingDlg::SetUpperDlg(int nUpperDlg)
{
	// deactive upper dialog.
	m_pPartnerList->SetActiveRecursive(FALSE) ;
	m_pChatDlg->SetActiveRecursive(FALSE) ;

	// setting pushup upper button.
	m_pBtnPartnerList->SetPush(FALSE) ;
	m_pBtnPartnerList->SetDisable(FALSE) ;
	m_pBtnChatDlg->SetPush(FALSE) ;
	m_pBtnChatDlg->SetDisable(FALSE) ;


	// active selected dialog.
	switch(nUpperDlg)
	{
	case e_PartnerListDlg :
		{
			// update partner list.

			// active partner list dialog.
			m_pPartnerList->SetActiveRecursive(TRUE) ;

			// setting to push selected button.
			m_pBtnPartnerList->SetPush(TRUE) ;
			m_pBtnPartnerList->SetDisable(TRUE) ;
		}
		break ;

	case e_ChatDlg :
		{
			// check and initialize chatting part.

			// active chatting dialog.
			m_pChatDlg->SetActiveRecursive(TRUE) ;

			// setting to push selected button.
			m_pBtnChatDlg->SetPush(TRUE) ;
			m_pBtnChatDlg->SetDisable(TRUE) ;
		}
		break ;
	}
}

void CDateMatchingDlg::SetLowerDlg(int nLowerDlg)
{
	// deactive lower dialog.
	m_pMyInfo->SetActiveRecursive(FALSE) ;			
	m_pPartnerInfo->SetActiveRecursive(FALSE) ;		
	m_pDateRecord->SetActiveRecursive(FALSE) ;	

	// setting pushup lower button.
	m_pBtnMyInfo->SetPush(FALSE) ;	
	m_pBtnMyInfo->SetDisable(FALSE) ;
	m_pBtnPartnerInfo->SetPush(FALSE) ;	
	m_pBtnPartnerInfo->SetDisable(FALSE) ;
	m_pBtnDateRecord->SetPush(FALSE) ;
	m_pBtnDateRecord->SetDisable(FALSE) ;

	// active selected dialog.
	switch(nLowerDlg)
	{
	case e_MyInfoDlg :
		{
			// update my info.

			// active my info dialog.
			m_pMyInfo->SetActiveRecursive(TRUE) ;

			// setting to push selected button.
			m_pBtnMyInfo->SetPush(TRUE) ;	
			m_pBtnMyInfo->SetDisable(TRUE) ;
		}
		break ;

	case e_PartnerInfoDlg :
		{
			m_pPartnerInfo->SetActiveRecursive(TRUE);
			m_pBtnPartnerInfo->SetPush(TRUE);
			m_pBtnPartnerInfo->SetDisable(TRUE);
		}
		break;
	case e_DateRecordDlg:
		{
			// update record info.

			// active record dialog.
			m_pDateRecord->SetActiveRecursive(TRUE) ;

			// setting to push selected button.
			m_pBtnDateRecord->SetPush(TRUE) ;
			m_pBtnDateRecord->SetDisable(TRUE) ;
		}
		break ;
	}
}

void CDateMatchingDlg::LinkPartnerList()
{
	m_pPartnerList->Linking() ;
}

void CDateMatchingDlg::LinkChatDlg()
{
	m_pChatDlg->SetChatList((cListDialog*)m_pChatDlg->GetWindowForID(DMD_CHAT_WINDOW_LIST)) ;

	m_pChatDlg->SetInputBox((cEditBox*)m_pChatDlg->GetWindowForID(DMD_CHAT_EDIT)) ;
}

void CDateMatchingDlg::LinkMyInfoDlg()
{
	m_pMyInfo->SetNickName((cStatic*)m_pMyInfo->GetWindowForID(DMD_MY_NICKNAME)) ;

	m_pMyInfo->SetGender((cStatic*)m_pMyInfo->GetWindowForID(DMD_MY_GENDER)) ;
	
	m_pMyInfo->SetAge((cStatic*)m_pMyInfo->GetWindowForID(DMD_MY_AGE)) ;

	m_pMyInfo->SetRegion((cStatic*)m_pMyInfo->GetWindowForID(DMD_MY_REGION)) ;

	m_pMyInfo->SetFavor((cStatic*)m_pMyInfo->GetWindowForID(DMD_MY_GOOD_FEELING_DEGREE)) ;

	m_pMyInfo->SetGoodIcon((cIconGridDialog*)m_pMyInfo->GetWindowForID(DMD_MY_GOOD_FEELING_ICONGRID)) ;

	m_pMyInfo->SetBadIcon((cIconGridDialog*)m_pMyInfo->GetWindowForID(DMD_MY_BAD_FEELING_ICONGRID)) ;

	m_pMyInfo->SetIntroduce((cTextArea*)m_pMyInfo->GetWindowForID(DMD_MY_INTRODUCTION_EDIT)) ;

	m_pMyInfo->SetLeftBtn((cButton*)m_pMyInfo->GetWindowForID(DMD_CHANGE_MY_INTRODUCTION_BTN)) ;

	m_pMyInfo->SetRightBtn((cButton*)m_pMyInfo->GetWindowForID(DMD_SAVE_MY_INTRODUCTION_BTN)) ;

	/*m_pMyInfo->SetIntroduceUp((cButton*)m_pMyInfo->GetWindowForID(DMD_MY_INTRODUCE_UP)) ;
	m_pMyInfo->SetIntroduceDown((cButton*)m_pMyInfo->GetWindowForID(DMD_MY_INTRODECE_DOWN)) ;
	m_pMyInfo->SetIntroduceGuage((cButton*)m_pMyInfo->GetWindowForID(DMD_MY_INTRODECE_GUAGE)) ;

	m_pMyInfo->GetIntroduce()->Add(m_pMyInfo->GetIntroduceUp()) ;
	m_pMyInfo->GetIntroduce()->Add(m_pMyInfo->GetIntroduceDown()) ;
	m_pMyInfo->GetIntroduce()->Add(m_pMyInfo->GetIntroduceGuage()) ;*/

	GAMEIN->SetMatchMyInfoDlg(m_pMyInfo) ;

	m_pMyInfo->GetIndexCreator()->Init(MAX_ITEMICON_NUM, IG_MATCH_MYINFO_STARTINDEX) ;
	m_pMyInfo->LoadFavorItem() ;
}

void CDateMatchingDlg::ActivateBtnEnterChallengeZone(BOOL bActive)
{
	if (bActive)
	{
		if (g_csDateManager.CLI_IsPossibleEnterChallengeZone())
		{
			if ((cButton*)GetWindowForID(DMD_ENTER_CHALLENGEZONE_BTN))
			{
				((cButton*)GetWindowForID(DMD_ENTER_CHALLENGEZONE_BTN))->SetDisable(FALSE);
				((cButton*)GetWindowForID(DMD_ENTER_CHALLENGEZONE_BTN))->SetImageRGB(RGB(255,255,255));
			}
		}
	}
	else
	{
		((cButton*)GetWindowForID(DMD_ENTER_CHALLENGEZONE_BTN))->SetDisable(TRUE);
		((cButton*)GetWindowForID(DMD_ENTER_CHALLENGEZONE_BTN))->SetImageRGB(RGB(190,190,190));
	}
}

void CDateMatchingDlg::SetEnterChallengeZoneFreq(int nFreq)
{
	char szText[256]="";

	sprintf(szText, m_szTitle, nFreq);
	m_pTitle->SetStaticText(szText);
}
// E 데이트존 추가 added by hseos 2007.11.13 2007.11.15	2008.01.08

void CDateMatchingDlg::LinkPartnerInfo()
{
	m_pPartnerInfo->SetNickName((cStatic*)m_pPartnerInfo->GetWindowForID(DMD_PARTNER_NICKNAME)) ;

	m_pPartnerInfo->SetGender((cStatic*)m_pPartnerInfo->GetWindowForID(DMD_PARTNER_GENDER)) ;
	
	m_pPartnerInfo->SetAge((cStatic*)m_pPartnerInfo->GetWindowForID(DMD_PARTNER_AGE)) ;

	m_pPartnerInfo->SetRegion((cStatic*)m_pPartnerInfo->GetWindowForID(DMD_PARTNER_REGION)) ;

	m_pPartnerInfo->SetFavor((cStatic*)m_pPartnerInfo->GetWindowForID(DMD_PARTNER_GOOD_FEELING_DEGREE)) ;

	m_pPartnerInfo->SetGoodIcon((cIconGridDialog*)m_pPartnerInfo->GetWindowForID(DMD_PARTNER_GOOD_FEELING_ICONGRID)) ;

	m_pPartnerInfo->SetBadIcon((cIconGridDialog*)m_pPartnerInfo->GetWindowForID(DMD_PARTNER_BAD_FEELING_ICONGRID)) ;

	m_pPartnerInfo->SetIntroduce((cTextArea*)m_pPartnerInfo->GetWindowForID(DMD_PARTNER_INTRODUCTION_EDIT)) ;

	m_pPartnerInfo->SetLeftBtn((cButton*)m_pPartnerInfo->GetWindowForID(DMD_REGIST_PARTNER_BTN)) ;

	m_pPartnerInfo->SetRightBtn((cButton*)m_pPartnerInfo->GetWindowForID(DMD_RECOMMEND_PARTNER_BTN)) ;

	GAMEIN->SetMatchPartnerInfoDlg(m_pPartnerInfo) ;

	m_pPartnerInfo->GetIndexCreator()->Init(MAX_ITEMICON_NUM, IG_MATCH_PARTNERINFO_STARTINDEX) ;
	m_pPartnerInfo->LoadFavorItem() ;
}

void CDateMatchingDlg::LinkRecordDlg()
{
	m_pDateRecord->Linking() ;
	GAMEIN->SetMatchRecordDlg(m_pDateRecord) ;
}

void CDateMatchingDlg::UpdateMyInfo(DATE_MATCHING_INFO* pInfo)
{
	if(!pInfo) return ;

	char tempBuf[12] = {0, } ;

	// check info.
	if( pInfo->bIsValid )
	{
		// setting nickname.
		m_pMyInfo->GetNickName()->SetStaticText(pInfo->szName) ;

		// setting gender.
		if( pInfo->byGender == 0 )
		{
			m_pMyInfo->GetGender()->SetStaticText(RESRCMGR->GetMsg(508)) ;
		}
		else
		{
			m_pMyInfo->GetGender()->SetStaticText(RESRCMGR->GetMsg(509)) ;
		}

		// setting face.
		int nFaceNum = 0 ;

		// 090521 ONS 신규종족 데이트매칭관련 처리 추가
		// check racial.
		int nRacial = HERO->GetCharacterTotalInfo()->Race ;

		// setting face num.
		if( nRacial == e_MATCH_HUMAN )
		{
			// setting human.
			if( pInfo->byGender == e_MATCH_MAN)
			{
				nFaceNum = e_MATCH_HUMAN_MAN ;
			}
			else
			{
				nFaceNum = e_MATCH_HUMAN_WOMAN ;
			}
		}
		else if( nRacial == e_MATCH_ELF )
		{
			// setting human.
			if( pInfo->byGender == e_MATCH_MAN)
			{
				nFaceNum = e_MATCH_ELF_MAN ;
			}
			else
			{
				nFaceNum = e_MATCH_ELF_WOMAN ;
			}
		}
		else
		{
			// setting human.
			if( pInfo->byGender == e_MATCH_MAN)
			{
				nFaceNum = e_MATCH_DEVIL_MAN ;
			}
			else
			{
				nFaceNum = e_MATCH_DEVIL_WOMAN ;
			}
		}

		// setting face image.
		SCRIPTMGR->GetImage( nFaceNum, m_pMyInfo->GetFacePicture(), PFT_HARDPATH ) ;

		// setting age.
		memset(tempBuf, 0, sizeof(tempBuf)) ;
		sprintf(tempBuf, "%d", pInfo->dwAge) ;
		m_pMyInfo->GetAge()->SetStaticText(tempBuf) ;

		// setting region.
		CSHResidentRegistManager::stADDInfo* pRegionInfo = g_csResidentRegistManager.GetADDInfo() ;
		m_pMyInfo->GetRegion()->SetStaticText(pRegionInfo->m_pTblLocation[pInfo->dwRegion].szName) ;

		// 100106 ONS 호감도를 매칭포인트로 변경
		memset(tempBuf, 0, sizeof(tempBuf)) ;
		sprintf(tempBuf, "%d", pInfo->nMatchingPoint) ;
		m_pMyInfo->GetFavor()->SetStaticText(tempBuf) ;

		// setting good icon.
		int count = 0 ;

		for( count = 0 ; count < MAX_GOOD_ICON_COUNT ; ++count )
		{
			m_pMyInfo->SetGoodIcon(count, pInfo->pdwGoodFeeling[count]-1) ;
		}

		// setting bad icon.
		for( count = 0 ; count < MAX_BAD_ICON_COUNT ; ++count )
		{
			m_pMyInfo->SetBadIcon(count, pInfo->pdwBadFeeling[count]-1) ;
		}

		// setting introduce.
		m_pMyInfo->GetIntroduce()->SetScriptText(pInfo->szIntroduce) ;
		m_pMyInfo->GetIntroduce()->SetReadOnly(TRUE) ;
	}
	else
	{
		CHARACTER_TOTALINFO* pTotalInfo = ((CPlayer*)HERO)->GetCharacterTotalInfo() ;

		if( !pTotalInfo ) return ;
		// setting nickname.
		m_pMyInfo->GetNickName()->SetStaticText(HERO->GetObjectName()) ;

		// setting gender.
		if( pTotalInfo->Gender == 0 )
		{
			m_pMyInfo->GetGender()->SetStaticText(RESRCMGR->GetMsg(508)) ;
		}
		else
		{
			m_pMyInfo->GetGender()->SetStaticText(RESRCMGR->GetMsg(509)) ;
		}

		// setting age.
		m_pMyInfo->GetAge()->SetStaticText("") ;

		// setting region.

		// setting favor.
		m_pMyInfo->GetFavor()->SetStaticText("0") ;

		// setting good icon.

		// setting introduce.
		m_pMyInfo->GetIntroduce()->SetScriptText(CHATMGR->GetChatMsg(1470)) ;
		m_pMyInfo->GetIntroduce()->SetReadOnly(TRUE) ;
	}
}
