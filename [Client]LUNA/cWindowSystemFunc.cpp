#include "stdafx.h"
#include "cWindowSystemFunc.h"

#include "WindowIDEnum.h"
#include "interface/cWindowManager.h"
#include "globaleventfunc.h"
#include "GameIn.h"
#include "CharMake.h"
#include "./Audio/MHAudioManager.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "InventoryExDialog.h"
#include "StorageDialog.h"
#include "ExchangeDialog.h"
#include "StallKindSelectDlg.h"
#include "StreetStall.h"
#include "MonsterGuageDlg.h"
#include "ChannelDialog.h"
#include "QuickManager.h"
#include "ItemManager.h"
#include "Item.h"
#include "cSkillBase.h"
#include "ExchangeItem.h"
#include "cMsgBox.h"
#include "cImeEx.h"
#include "MainTitle.h"
#include "ServerListDialog.h"
#include "PKManager.h"
#include "CharMakeNewDlg.h"
#include "CharSelect.h"
#include "ProfileDlg.h"
#include "CertificateDlg.h"
#include "DateMatchingDlg.h"
#include "cNumberPadDialog.h"
#ifdef TAIWAN_LOCAL
#include "ChinaAdviceDlg.h"
#endif

#include "QuickSlotDlg.h"
#include "petweareddialog.h"
#include "cHousingWarehouseDlg.h"
#include "petmanager.h"

#include "BattleGuageDlg.h"

void cbDragDropProcess(LONG curDragX, LONG curDragY, LONG id, LONG beforeDragX, LONG beforeDragY)
{
	cDialog * srcDlg = WINDOWMGR->GetWindowForID(id);
	if(!srcDlg) return;
	ASSERT(srcDlg);

	// 070611 LYW --- cWindowSystemFunc : Add check routine.
	// check favor item part.
	if( srcDlg->GetType() == WT_ICON )
	{
		if( id >= IG_FAVOR_STARTINDEX && id <= IG_MATCH_ENDINDEX )
		{
			favorDragDropProcess(curDragX, curDragY, id, beforeDragX, beforeDragY) ;
			return ;
		}
	}

	cDialog * ptDlg = WINDOWMGR->GetWindowForXYExceptIcon(beforeDragX,beforeDragY);
	cDialog * ptardlg = WINDOWMGR->GetWindowForXYExceptIcon(curDragX,curDragY);
	if(ptDlg && !ptardlg)
	{
		if( ptDlg->GetType() == WT_ITEMSHOPDIALOG )
		{
			srcDlg->SetAbsXY((LONG)beforeDragX, (LONG)beforeDragY);
//---임시 : 진군에게 물어보기
			if(!srcDlg->GetParent())
				return;
			if(!srcDlg->GetParent()->IsActive())
				srcDlg->SetActive(FALSE);

			srcDlg->SetDepend(TRUE);
			WINDOWMGR->SetMouseInputProcessed();
			return;
		}
	}

//	srcDlg->ExcuteCBFunc( WE_DRAGEND );
	
	if(srcDlg->GetType() & WT_ICON) // 아이콘타입이다
	{
		//////////////////////////////////////////////////////////////////////////
		// YHEDIT [8/27/2003]
		WORD wtype = srcDlg->GetType();
		if( !(wtype == WT_ITEM ||
			wtype == WT_SKILL ||
			wtype == WT_JINBUB ||
			wtype == WT_QUICKITEM ||
			wtype == WT_EXCHANGEITEM ||
			wtype == WT_STALLITEM ||
			// 070216 LYW --- cWindowSystemFunc : Check window type.
			wtype == WT_CONDUCT ||
			wtype == WT_DEALITEM) )
		{
			ASSERT(0);
		}
		cIcon* pIcon = (cIcon*)srcDlg;
		cDialog * targetdlg = WINDOWMGR->GetWindowForXYExceptIcon(curDragX,curDragY);
		if(targetdlg)
		{
			// quest 땜시... jsd(2004.09.14)			
			if( pIcon->CanMoveToDialog(targetdlg) == FALSE )
			{
				srcDlg->SetAbsXY((LONG)beforeDragX, (LONG)beforeDragY);	// SetDragEnd()가 있는데 필요한가???체크필
			}
			else
			{
				if(wtype == WT_QUICKITEM)
				{
					if(targetdlg->GetID() == QI1_QUICKSLOTDLG || targetdlg->GetID() == QI2_QUICKSLOTDLG)
					{
						cDialog* pSrcDialog = NULL ;
						pSrcDialog = WINDOWMGR->GetWindowForXYExceptIcon(beforeDragX, beforeDragY) ;

						if(pSrcDialog)
						{
							if(!((cQuickSlotDlg*)targetdlg)->FakeMoveIcon(pSrcDialog, curDragX,curDragY,(cIcon*)srcDlg))
							{
								srcDlg->SetAbsXY((LONG)beforeDragX, (LONG)beforeDragY) ;
							}
						}
					}
					else
					{
						( ( cQuickSlotDlg* )ptDlg )->FakeDeleteQuickItem();
						AUDIOMGR->Play(
							69,
							gHeroID);
						SendDeleteIconMsg((cIcon*)srcDlg);
						srcDlg->SetAbsXY((LONG)beforeDragX, (LONG)beforeDragY) ;
					}
				}
				else
				{
					if(targetdlg->GetID() == QI1_QUICKSLOTDLG || targetdlg->GetID() == QI2_QUICKSLOTDLG)
					{
						if(!((cQuickSlotDlg*)targetdlg)->FakeMoveIcon(NULL, curDragX,curDragY,(cIcon*)srcDlg))
						{
							srcDlg->SetAbsXY((LONG)beforeDragX, (LONG)beforeDragY) ;
						}
					}
					else
					{
						if(!targetdlg->FakeMoveIcon(curDragX,curDragY,(cIcon*)srcDlg))
						{
							srcDlg->SetAbsXY((LONG)beforeDragX, (LONG)beforeDragY) ;
						}
					}
				}


				switch(targetdlg->GetType())
				{
				case WT_QUICKDIALOG:
					{
						AUDIOMGR->Play(
							61,
							gHeroID);
					}
					break;
				case WT_INVENTORYDIALOG:
					break;
				default:
					{
						AUDIOMGR->Play(
							58,
							gHeroID);
					}
				}				
			}
		}
		else
		{
			// drop in GROUND
			// or dump icon and item, skill icon
			if( pIcon->CanDelete() )
			{
				if(pIcon->GetType() == WT_QUICKITEM)
				{
					( ( cQuickSlotDlg* )ptDlg )->FakeDeleteQuickItem();
					AUDIOMGR->Play(
						69,
						gHeroID);
				}

				SendDeleteIconMsg((cIcon*)srcDlg);
			}

			srcDlg->SetAbsXY((LONG)beforeDragX, (LONG)beforeDragY);	// SetDragEnd()가 있는데 필요한가???체크필
		}

		if(srcDlg->GetParent())		//임시다!!! 퀵아이이템 에러다. 이런경우 안생기게 수정하시오!
		{
			if(!srcDlg->GetParent()->IsActive())
				srcDlg->SetActive(FALSE);

			srcDlg->SetDepend(TRUE);
		}

		WINDOWMGR->SetMouseInputProcessed();
	}
}

// 070611 LYW --- cWindowSystemFunc : Add function to processing drag and drop of favor item part.
void favorDragDropProcess(LONG curDragX, LONG curDragY, LONG id, LONG beforeDragX, LONG beforeDragY)
{
	cDialog* srcDlg = WINDOWMGR->GetWindowForID(id) ;

	// check srcDlg.
	if( !srcDlg ) return ;
	ASSERT(srcDlg) ;

	// check prev dialog and next dialog.
	cDialog* prevDlg = WINDOWMGR->GetWindowForXYExceptIcon(beforeDragX, beforeDragY) ;
	cDialog* nextDlg = WINDOWMGR->GetWindowForXYExceptIcon(curDragX, curDragY) ;

	// drop to ground.
	if( !prevDlg || !nextDlg )
	{
		// return icon.
		ReturnIconToPrevDlg(srcDlg, beforeDragX, beforeDragY) ;
		return ;
	}

	// check type of next dialog.
	LONG lPrevDlgID = prevDlg->GetID() ;
	LONG lNextDlgID = nextDlg->GetID() ;

	// check same dlg.
	if( lPrevDlgID == lNextDlgID )
	{
		// return icon.
		ReturnIconToPrevDlg(srcDlg, beforeDragX, beforeDragY) ;
		return ;
	}

	// npc image dialog.
	if( lNextDlgID == NI_DIALOG )
	{
		switch(lPrevDlgID)
		{
		case DMD_IDENTIFICATIONDLG :
		case DMD_IDT_GOOD_FEELING_ICONGRID :
		case DMD_IDT_BAD_FEELING_ICONGRID :
			{
				srcDlg->SetActive(FALSE) ;

				// delete icon.
				CIdentification* pDlg = GAMEIN->GetIdentificationDlg() ;

				if( pDlg )
				{
					pDlg->DeleteItem((CFavorItem*)srcDlg) ;
				}

				// return icon.
				ReturnIconToPrevDlg(srcDlg, beforeDragX, beforeDragY) ;
				return ;
			}
			break ;

		case DMD_FAVOR_ICON :
		case DMD_FAVOR_ICON_DLG :
			{
				// return to favor icon dialog.
				ReturnIconToPrevDlg(srcDlg, beforeDragX, beforeDragY) ;
			}
			break ;

		case NI_DIALOG :
			{
				srcDlg->SetActive(FALSE) ;
				// delete icon.
				CIdentification* pDlg = GAMEIN->GetIdentificationDlg() ;

				if( pDlg )
				{
					pDlg->DeleteItem((CFavorItem*)srcDlg) ;
				}

				// return icon.
				ReturnIconToPrevDlg(srcDlg, beforeDragX, beforeDragY) ;
				return ;
			}
			break ;

		default :
			break ;
		}
	}
	// not npc image dialog. common dialog.
	else
	{
		// return to prev dialog.
		ReturnIconToPrevDlg(srcDlg, beforeDragX, beforeDragY) ;

		// check favor icon dialog.
		if( (lPrevDlgID == DMD_FAVOR_ICON_DLG || lPrevDlgID == NI_DIALOG) && lNextDlgID == DMD_IDENTIFICATIONDLG )
		{
			nextDlg->FakeMoveIcon(curDragX, curDragY, (cIcon*)srcDlg) ;
		}
	}
}

// 070611 LYW --- cWindowSystemFunc : Add function to return icon.
void ReturnIconToPrevDlg(cDialog* srcDlg, LONG beforeDragX, LONG beforeDragY)
{
	// check parent dialog.
	if( !srcDlg->GetParent() ) return ;

	// check active.
	if( !srcDlg->GetParent()->IsActive() )
	{
		srcDlg->SetActive(FALSE) ;
	}

	// setting icon position to before position.
	srcDlg->SetAbsXY(beforeDragX, beforeDragY) ;

	srcDlg->SetDepend(TRUE) ;
	WINDOWMGR->SetMouseInputProcessed() ;
}

void SendDeleteIconMsg(cIcon * pIcon)
{
	if(pIcon->GetType() == WT_ITEM)
	{
		if( PKMGR->IsPKLooted() ) return;

		// 090817 ShinJS --- 조합 다이얼로그가 활성화 상태면 아이템을 버릴수 없도록 한다.
		cDialog* pMixDlg = WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
		if( pMixDlg && pMixDlg->IsActive() )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1656 ) );
			return;
		}

		if( pIcon->GetParent() == (cObject*)( GAMEIN->GetPetWearedDlg() ) )
		{
			// 090720 ONS 펫 고정스킬아이템은 파괴할 수 없다.
			CItem *pItem = (CItem *)pIcon;
			PET_OBJECT_INFO* pPetObjectInfo = PETMGR->GetPetObjectInfo( HEROPET->GetItemDBIdx() );
			PET_INFO* pPetInfo = PETMGR->GetPetInfo(pPetObjectInfo->Kind);
			if( (pPetInfo->StaticItemIndex > 0)	&&
				(pItem->GetItemBaseInfo().Position == ePetEquipSlot_Skill6)	)
			{
				return;
			}
			GAMEIN->GetPetWearedDlg()->FakeDelete( pIcon );
			GAMEIN->GetPetWearedDlg()->SetDisable( TRUE );
			WINDOWMGR->MsgBox( MBI_DELETEPETICONCHECK, MBT_YESNO, CHATMGR->GetChatMsg(209) );
		}
		else
		{
			ITEMMGR->FakeDeleteItem((CItem *)pIcon);
			WINDOWMGR->MsgBox( MBI_DELETEICONCHECK, MBT_YESNO, CHATMGR->GetChatMsg(209) );
			GAMEIN->GetInventoryDialog()->SetDisable( TRUE );
		}
	}
	else if( pIcon->GetType() == WT_STALLITEM )
	{
		GAMEIN->GetStreetStallDialog()->FakeDeleteItem( ((CExchangeItem*)pIcon)->GetPosition() );
		AUDIOMGR->Play(
			67,
			gHeroID);
	}
	else if( pIcon->GetType() == WT_HOUSING_STORED_ICON )
	{
		//하우징 아이콘일경우 
		GAMEIN->GetHousingWarehouseDlg()->FakeDeleteIcon((cHousingStoredIcon*)pIcon);
		AUDIOMGR->Play(
			67,
			gHeroID);
	}
}

void CreateMainTitle_m()
 { 
	if(TITLE->GetLoginKey().empty())
	{	
		if(cNumberPadDialog* const numberPadDialog = (cNumberPadDialog*)WINDOWMGR->GetDlgInfoFromFile("Data/Interface/Windows/IDPass.bin", "rb"))
		{
			numberPadDialog->Linking();

			GAMEIN->SetNumberPadDlg(
				numberPadDialog);
			WINDOWMGR->AddWindow(
				numberPadDialog);

			cEditBox* pEdit = (cEditBox*)numberPadDialog->GetWindowForID( MT_IDEDITBOX );
			if(GAMERESRCMNGR->m_GameDesc.LimitDay != 0 && GAMERESRCMNGR->m_GameDesc.LimitDay >= GetCurTimeValue())
				pEdit->SetEditText(GAMERESRCMNGR->m_GameDesc.LimitID);
			pEdit->SetValidCheck( VCM_ID );

			pEdit = (cEditBox*)numberPadDialog->GetWindowForID( MT_PWDEDITBOX );
			if(GAMERESRCMNGR->m_GameDesc.LimitDay != 0 && GAMERESRCMNGR->m_GameDesc.LimitDay >= GetCurTimeValue())
				pEdit->SetEditText(GAMERESRCMNGR->m_GameDesc.LimitPWD);
			pEdit->SetValidCheck( VCM_PASSWORD );
		}
	}
	else
	{
		if(cNumberPadDialog* const numberPadDialog = (cNumberPadDialog*)WINDOWMGR->GetDlgInfoFromFile("Data/Interface/Windows/WebLauncherIDPass.bin", "rb"))
		{
			numberPadDialog->Linking();

			GAMEIN->SetNumberPadDlg(
				numberPadDialog);
			WINDOWMGR->AddWindow(
				numberPadDialog);

			cEditBox* pEdit = (cEditBox*)numberPadDialog->GetWindowForID( MT_IDEDITBOX );
			pEdit->SetUseCaret( FALSE );
			pEdit = (cEditBox*)numberPadDialog->GetWindowForID( MT_PWDEDITBOX );
			pEdit->SetUseCaret( FALSE );
		}
	}

#ifndef TAIWAN_LOCAL
	cWindow* window1 = WINDOWMGR->GetDlgInfoFromFile("./Data/Interface/Windows/copyright.bin", "rb");
	WINDOWMGR->AddWindow(window1);

#else	//TAIWAN_LOCAL
	cWindow* window2 = WINDOWMGR->GetDlgInfoFromFile( "./Data/Interface/Windows/CNAdviceDlg.bin", "rb" );
	((CChinaAdviceDlg*)window2)->Linking();
	WINDOWMGR->AddWindow( window2 );
#endif
	
	// 070117 LYW --- Modified bin file name.
	//cWindow* window3 = WINDOWMGR->GetDlgInfoFromFile( "./Data/Interface/Windows/ServerListDlg.bin", "rb" );
	cWindow* window3 = WINDOWMGR->GetDlgInfoFromFile( "./Data/Interface/Windows/ServerSelect.bin", "rb" );
	WINDOWMGR->AddWindow( window3 );

	// dynamic dlg
#ifdef TAIWAN_LOCAL
	cWindow* window4 = WINDOWMGR->GetDlgInfoFromFile( "./Data/Interface/Windows/Dynamic.bin", "rb" );
	WINDOWMGR->AddWindow( window4 );
#endif

}


void NewCreateCharSelect_m()
{
	cWindow * window = WINDOWMGR->GetDlgInfoFromFile("./Data/Interface/Windows/CharSelect.bin", "rb");
	WINDOWMGR->AddWindow(window);
}

// 061218 LYW --- Add dialog for certificate some condition.
void CreateCertificateDlg()
{
	cWindow* window = WINDOWMGR->GetDlgInfoFromFile("./Data/Interface/Windows/CharDel.bin", "rb") ;
	WINDOWMGR->AddWindow(window) ;
	CHARSELECT->SetCertificateDlg((CCertificateDlg*)window) ;
	CHARSELECT->GetCertificateDlg()->Linking() ;
}

// 061215 LYW --- Add profile dialog for character information.
void CreateProfileDlg()
{
	cWindow* window = WINDOWMGR->GetDlgInfoFromFile("./Data/Interface/Windows/CharMakeProfile.bin", "rb") ;
	WINDOWMGR->AddWindow(window) ;
	
	CHARSELECT->SetProfileDlg((CProfileDlg*)window) ;
	CHARSELECT->GetProfileDlg()->Linking() ;
}

// 061214 LYW --- CharMake : Add new function to create dialog for character property.
void CreateCharMakeNewDlg()
{
	cWindow* window = WINDOWMGR->GetDlgInfoFromFile( "./Data/Interface/Windows/CharMake.bin", "rb" ) ;
	WINDOWMGR->AddWindow( window ) ;
	
	CHARMAKE->SetCharMakeNewDlg( (CCharMakeNewDlg*)window ) ;
	CHARMAKE->GetCharMakeNewDlg()->Linking() ;

	cEditBox* pEdit = ( cEditBox* )CHARMAKE->GetCharMakeNewDlg()->GetWindowForID( CM_IDEDITBOX ) ;
	pEdit->SetValidCheck( VCM_CHARNAME ) ;
}

cWindow * CreateGameLoading_m()
{
	cWindow * window = WINDOWMGR->GetDlgInfoFromFile("./Data/Interface/Windows/NewLoadDlg.bin", "rb");
	WINDOWMGR->AddWindow(window);

	return window;
}

//KES LOGO 031107
void CreateInitLogo_m()
{
	cWindow* const window = WINDOWMGR->GetDlgInfoFromFile(
		"./Data/Interface/Windows/InitDlg.bin",
		"rb");

	if(0 == window)
	{
		return;
	}

	WINDOWMGR->AddWindow(
		window);

	SHORT_RECT rect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&rect,
		&value,
		&value);

	VECTOR2 scale = {
		float(rect.right) / window->GetWidth(),
		float(rect.bottom) / window->GetHeight()
	};

	window->SetScale(
		&scale);
	window->SetAbsXY(
		0,
		0);
}

void CreateMonsterGuageDlg()
{
	cWindow * window = WINDOWMGR->GetDlgInfoFromFile("./Data/Interface/Windows/Target.bin", "rb");
	//보스몬스터 TargetDlg 
	cDialog * pBossTargetDlg = (cDialog*)WINDOWMGR->GetDlgInfoFromFile("./Data/Interface/Windows/TargetBoss.bin", "rb");
	//일반몬스터 TargetDlg
	cDialog * pMonsteTargetDlg = (cDialog*)WINDOWMGR->GetDlgInfoFromFile("./Data/Interface/Windows/TargetMonster.bin", "rb");

	cDialog * pCharacterGuageDlg = (cDialog*)WINDOWMGR->GetDlgInfoFromFile("./Data/Interface/Windows/TargetCharacter.bin", "rb");
		
	cBattleGuageDlg * pBattleGuageDlg	=	(cBattleGuageDlg*)WINDOWMGR->GetDlgInfoFromFile("./Data/Interface/Windows/BattleGuage.bin", "rb");

	WINDOWMGR->AddWindow(window);
	WINDOWMGR->AddWindow( pBossTargetDlg );
	WINDOWMGR->AddWindow( pMonsteTargetDlg );
	WINDOWMGR->AddWindow( pBattleGuageDlg );
	WINDOWMGR->AddWindow( pCharacterGuageDlg );

	GAMEIN->SetMonsterGuageDlg((CMonsterGuageDlg * )window);

	GAMEIN->GetMonsterGuageDlg()->SetBossTargetDlg( pBossTargetDlg );
	GAMEIN->GetMonsterGuageDlg()->SetMonsterTargetDlg( pMonsteTargetDlg );
	GAMEIN->GetMonsterGuageDlg()->SetCharacterTargetDlg( pCharacterGuageDlg );

	GAMEIN->GetMonsterGuageDlg()->Linking();

	GAMEIN->SetBattleGuageDlg( pBattleGuageDlg );
	pBattleGuageDlg->Linking();
	
}

void CreateChannelDlg()
{
	CChannelDialog* window = (CChannelDialog*)WINDOWMGR->GetDlgInfoFromFile("./Data/Interface/Windows/Channel.bin", "rb");
	CHARSELECT->SetChannelDialog( window );
	window->Linking();

	WINDOWMGR->AddWindow(window);
}
