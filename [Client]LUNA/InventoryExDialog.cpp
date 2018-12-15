#include "stdafx.h"
#include "InventoryExDialog.h"
#include "WindowIDEnum.h"
#include "Interface/cWindowManager.h"
#include "Interface/cIconGridDialog.h"
#include "Interface/cStatic.h"
#include "Interface/cPushupButton.h"
#include "ObjectStateManager.h"

#include "GameIn.h"

#include "ItemManager.h"
#include "ObjectManager.h"
#include "cSkillTreeManager.h"
#include "ChatManager.h"

#include "DealItem.h"
#include "cDivideBox.h"
#include "./Audio/MHAudioManager.h"

#include "DealDialog.h"

#include "./Input/UserInput.h"
// 080916 LUJ, 아이템 합성 창
#include "ComposeDialog.h"
#include "PKManager.h"
#include "cMsgBox.h"
#include "QuestManager.h"
#include "QuickSlotDlg.h"
#include "ProgressDialog.h"
#include "StreetStallManager.h"
#include "ShoutDialog.h"

// 080218 LUJ
#include "ApplyOptionDialog.h"

#include "StreetStallManager.h"
#include "ShoutDialog.h"

#include "ShowdownManager.h"
#include "MoveManager.h"
#include "./Interface/cScriptManager.h"

#include "NpcScriptManager.h"
#include "MHMap.h"
// 080228 LUJ, 보호 아이템 처리
#include "MixDialog.h"
#include "ReinforceDlg.h"
#include "EnchantDialog.h"
#include "../[CC]Header/GameResourceManager.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "TutorialManager.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "cChangeNameDialog.h"
#include "WearedExDialog.h"
#include "../hseos/Family/SHFamilyManager.h"

// 080414 LUJ, 외양 변환 창
#include "BodyChangeDialog.h"

#include "FishingDialog.h"
#include "FishingManager.h"

#include "PetManager.h"
#include "PetResurrectionDialog.h"
#include "PetWearedDialog.h"
#include "./interface/cResourceManager.h"

#include "GuildTournamentMgr.h"

#include "cHousingMgr.h"

// 080917 KTH -- SiegeWarfareManager Include
#include "SiegeWarfareMgr.h"

//090423 pdy 하우징창고아이콘 
#include "cHousingStoredIcon.h"

//090526 pdy 하우징 집방문 UI
#include "cHouseSearchDlg.h"

#include "cCookMgr.h"
#include "MiniMapDlg.h"
#include "Questquickviewdialog.h"
#include "QuestDialog.h"

#include "ChangeClassDlg.h"
#include "DissolveDialog.h"

CInventoryExDialog::CInventoryExDialog()
{
	m_type				= WT_INVENTORYDIALOG;
	m_pWearedDlg		= NULL;
	m_pMoneyEdit		= NULL;
	m_BtnPushstartTime		= 0;
	m_BtnPushDelayTime		= 500;
	m_bPushTabWithMouseOver = FALSE;

	ZeroMemory(
		m_pBackground,
		sizeof(m_pBackground));
	// 071204 LYW --- InventoryExDialog : 마나 감소 물약 사용 용 아이템 포인터 추가.
	// 마나 감소 물약을 사용하고 있을 때, 다시 한번 마나 감소 물약을 사용하면, 메시지를 띄운다.
	// 메시지 처리 부에서, Yes를 선택하면, 사용 요청을 보내는데, 이때 아이템 정보가 필요하다.
	// 하지만, 아이템을 퀵 슬롯에 등록하고 인벤은 열지도 않고 사용할 경우가 생기는데, 
	// 이때 GetCurSelectedItem()에서 제대로 된 포인터를 넘기지 못하기 때문에 
	// 이 때만 사용 할 임시 아이템 포인터를 선언 및 사용한다.
	m_pQuickSelectedItem = NULL ;
}

CInventoryExDialog::~CInventoryExDialog()
{}

void CInventoryExDialog::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();
	cTabDialog::RenderTabComponent();
}
void CInventoryExDialog::Add(cWindow * window)
{
	// 071210 LYW --- InventoryExDialog : 인벤토리 확장에 따른 확장아이템의 활성화 여부 처리.
	if(window->GetType() == WT_PUSHUPBUTTON)
	{
		BYTE byCurIdx = 0 ;
		byCurIdx = curIdx1 ;

		AddTabBtn(curIdx1++, (cPushupButton * )window);

        if( byCurIdx >= 2 )
		{
			//window->SetActive(FALSE) ;
			((cPushupButton*)window)->SetActive(FALSE) ;
		}
	}
	else if(window->GetType() == WT_ICONGRIDDIALOG)
	{
		BYTE byCurIdx = 0 ;
		byCurIdx = curIdx2 ;

		AddTabSheet(curIdx2++, window);
		((cIconGridDialog*)window)->SetDragOverIconType( WT_ITEM );

		if( byCurIdx >= 2 )
		{
			//window->SetActive(FALSE) ;
			((cIconGridDialog*)window)->SetActive(FALSE) ;
		}
	}
	else 
		cDialog::Add(window);
}


DWORD CInventoryExDialog::ActionEvent(CMouse * mouseInfo)
{
	//아이콘 드래그중일때.. 다른 탭으로 이동해야한다.
	if( WINDOWMGR->IsDragWindow() && WINDOWMGR->GetDragDlg() )
	{
		if(WINDOWMGR->GetDragDlg()->GetType() == WT_ITEM)
		{
			DWORD we = WE_NULL;
			if(!m_bActive) return we;
			we = cDialog::ActionEvent(mouseInfo);

			BOOL bMouseOver = FALSE;
			for( int i = 0 ; i < m_bTabNum ; i++ )
			{
				m_ppPushupTabBtn[i]->ActionEvent( mouseInfo );
				
				if( we & WE_MOUSEOVER )
				if( m_ppPushupTabBtn[i]->IsActive() && m_ppPushupTabBtn[i]->PtInWindow( mouseInfo->GetMouseX(), mouseInfo->GetMouseY() ) )
				{
					bMouseOver = TRUE;
					
					if( m_bPushTabWithMouseOver )
					{
						if( i != m_bSelTabNum )
						{
							SelectTab(i);		//순서지키기(수련창때문..)
							m_bSelTabNum = i;							
						}
					}
					else
					{
						if( m_BtnPushstartTime == 0 )
							m_BtnPushstartTime = gCurTime;
						else if( gCurTime - m_BtnPushstartTime > m_BtnPushDelayTime )
							m_bPushTabWithMouseOver = TRUE;
					}				
				}
			}
			
			if( !bMouseOver )
				m_BtnPushstartTime = 0;
			
			we |= m_ppWindowTabSheet[m_bSelTabNum]->ActionEvent( mouseInfo );
			return we;
		}
	}

	m_BtnPushstartTime		= 0;
	m_bPushTabWithMouseOver = FALSE;
	return cTabDialog::ActionEvent( mouseInfo );	
}


WORD CInventoryExDialog::GetTabIndex( POSTYPE absPos )
{
	return ( absPos - TP_INVENTORY_START ) / TABCELL_INVENTORY_NUM;
}

void CInventoryExDialog::RefreshInvenItem()
{
	if(!HERO)
		return;

	BYTE TabNum = GetTabNum();
	for( BYTE i = 0 ; i < TabNum ; ++i )
	{
		cIconGridDialog* gridDlg = (cIconGridDialog*)GetTabSheet(i);

		for( WORD j = 0 ; j < gridDlg->GetCellNum() ; ++j )
		{
			CItem * pItem = (CItem *)gridDlg->GetIconForIdx(TP_INVENTORY_START+j);
			
			if(pItem)
				ITEMMGR->RefreshItem( pItem );
		}
	}

	// 착용중인 장비도 갱신
	for( WORD j = eWearedItem_Weapon ; j < eWearedItem_Max ; ++j )
	{
		CItem * pItem = GetItemForPos( TP_WEAR_START + j );
		if(pItem)
			ITEMMGR->RefreshItem( pItem );
	}
}

BOOL CInventoryExDialog::AddItem(ITEMBASE * itemBase)
{
	if( itemBase->dwDBIdx == 0 )
	{
//		DEBUGMSG( 0, "Item DB idx == 0" );
		return FALSE;
	}

	CItem * newItem = ITEMMGR->MakeNewItem( itemBase, "AddItem" );
	if(newItem == NULL)
		return FALSE;

	// 071125 LYW --- InventoryExDialog : 시간 아이템일 경우, 남은 시간 세팅.
	if( itemBase->nSealed == eITEM_TYPE_UNSEAL )
	{
		newItem->SetItemBaseInfo( *itemBase ) ;
	}
	
	return AddItem( newItem );
}

// 영약 Dura도 증가 시켜 준다.리소스 삭제까지.
BOOL CInventoryExDialog::AddItem(CItem* pItem)
{
	if(pItem == NULL)
	{
		ASSERT(pItem);
		return FALSE;
	}

	//색 변환
	ITEMMGR->RefreshItem( pItem );
	//add 하는 아이템만 refresh 하면 된다?

	// 071210 LYW --- InventoryExDialog : 아래 소스에서 포지션을 여러번 계속 받아서 참조하는 것을,
	// 한번 받아서 계속 사용하도록 수정함.
	POSTYPE pos = 0 ;
	pos = pItem->GetPosition() ;
	BOOL bRes = FALSE;


	if(TP_INVENTORY_START <= pos && pos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		
		// 인벤토리 
		int tabIndex = GetTabIndex( pItem->GetPosition() );
		cIconGridDialog * dlg = (cIconGridDialog *)GetTabSheet(tabIndex);

//		WORD relPos = pItem->GetPosition()-INVENTORY_STARTPOSITION-CELLNUMPERPAGE*tabIndex;
		WORD relPos = pos-TP_INVENTORY_START-TABCELL_INVENTORY_NUM*tabIndex;
		CItem * item = (CItem * )dlg->GetIconForIdx(relPos);
		if(item)
		{
			ASSERT(0);
			return FALSE;
		}
		else
		{
			//return dlg->AddIcon(relPos, pItem);
			bRes = dlg->AddIcon(relPos, pItem);
			ITEMMGR->UpdateDlgCoolTime(pItem->GetItemIdx());	// 080326 NYJ --- 쿨타임갱신

			// 100422 ShinJS --- 인벤토리 아이템 변경시 퀘스트 정보를 갱신한다.
			GAMEIN->GetQuestQuickViewDialog()->ResetQuickViewDialog();
			GAMEIN->GetQuestDialog()->RefreshQuestList();
			QUESTMGR->ProcessNpcMark();
			return bRes;
		}
	}
	else if(TP_WEAR_START <= pos && pos < TP_WEAR_END)
	{
		// 장착창
		//return m_pWearedDlg->AddItem(pos-TP_WEAR_START, pItem);
		bRes = m_pWearedDlg->AddItem(pos-TP_WEAR_START, pItem);
		ITEMMGR->UpdateDlgCoolTime(pItem->GetItemIdx());	// 080326 NYJ --- 쿨타임갱신
		return bRes;
	}

    return FALSE;
}

BOOL CInventoryExDialog::DeleteItem(POSTYPE Pos,CItem** ppItem)
{
	BOOL bRet = FALSE;
//	if(INVENTORY_STARTPOSITION <= Pos && Pos < INVENTORY_ENDPOSITION)
	if(TP_INVENTORY_START <= Pos && Pos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		int TabIndex = GetTabIndex( Pos );
		cIconGridDialog * Dlg = (cIconGridDialog *)GetTabSheet(TabIndex);
		if(!Dlg->IsAddable(Pos - TP_INVENTORY_START-TABCELL_INVENTORY_NUM*TabIndex))
			bRet = Dlg->DeleteIcon(Pos-TP_INVENTORY_START-TABCELL_INVENTORY_NUM*TabIndex, (cIcon **)ppItem);
	}
//	else if(WEAR_STARTPOSITION <= Pos && Pos < WEAR_ENDPOSITION)
	else if(TP_WEAR_START <= Pos && Pos < TP_WEAR_END)
	{
//		if(!m_pWearedDlg->IsAddable(Pos - WEAR_STARTPOSITION))
//			return m_pWearedDlg->DeleteItem(Pos-WEAR_STARTPOSITION, (cIcon **)ppItem);
		if(!m_pWearedDlg->IsAddable(Pos - TP_WEAR_START))
			bRet = m_pWearedDlg->DeleteItem(Pos-TP_WEAR_START, (cIcon **)ppItem);
	}

	// 100422 ShinJS --- 인벤토리 아이템 변경시 퀘스트 정보를 갱신한다.
	GAMEIN->GetQuestQuickViewDialog()->ResetQuickViewDialog();
	GAMEIN->GetQuestDialog()->RefreshQuestList();
	QUESTMGR->ProcessNpcMark();
    return bRet;
}

BOOL CInventoryExDialog::UpdateItemDurabilityAdd(POSTYPE absPos, DURTYPE dur)
{
//	if(INVENTORY_STARTPOSITION <= absPos && absPos < INVENTORY_ENDPOSITION)
	if(TP_INVENTORY_START <= absPos && absPos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		int TabIndex = GetTabIndex( absPos );
		cIconGridDialog * Dlg = (cIconGridDialog *)GetTabSheet(TabIndex);
//		WORD relPos = absPos-INVENTORY_STARTPOSITION-CELLNUMPERPAGE*TabIndex;
		WORD relPos = absPos-TP_INVENTORY_START-TABCELL_INVENTORY_NUM*TabIndex;
		CItem * item = (CItem * )Dlg->GetIconForIdx(relPos);
		item->SetDurability(item->GetDurability()+dur);
		
		// 100422 ShinJS --- 인벤토리 아이템 변경시 퀘스트 정보를 갱신한다.
		GAMEIN->GetQuestQuickViewDialog()->ResetQuickViewDialog();
		GAMEIN->GetQuestDialog()->RefreshQuestList();
		QUESTMGR->ProcessNpcMark();
		return TRUE;
	}

    return FALSE;
}
CItem * CInventoryExDialog::GetItemForPos(POSTYPE absPos)
{
//	if(INVENTORY_STARTPOSITION <= absPos && absPos < INVENTORY_ENDPOSITION)
	if(TP_INVENTORY_START <= absPos && absPos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		int TabIndex = GetTabIndex( absPos );
		cIconGridDialog * Dlg = (cIconGridDialog *)GetTabSheet(TabIndex);
		return (CItem *)Dlg->GetIconForIdx(absPos-TP_INVENTORY_START-TABCELL_INVENTORY_NUM*TabIndex);
//		return (CItem *)Dlg->GetIconForIdx(absPos-INVENTORY_STARTPOSITION-CELLNUMPERPAGE*TabIndex);
	}
//	else if(WEAR_STARTPOSITION <= absPos && absPos < WEAR_ENDPOSITION)
	else if(TP_WEAR_START <= absPos && absPos < TP_WEAR_END)
	{
//		return (CItem *)m_pWearedDlg->GetIconForIdx(absPos-WEAR_STARTPOSITION);
		return (CItem *)m_pWearedDlg->GetIconForIdx(absPos-TP_WEAR_START);
	}

	return NULL;
}

void CInventoryExDialog::UseItem(CItem* pItem)
{
	ASSERT(pItem);

	CHero* hero = OBJECTMGR->GetHero();
	ASSERT( hero );
	
	if( WINDOWMGR->IsDragWindow() && WINDOWMGR->GetDragDlg() )
	if( WINDOWMGR->GetDragDlg()->GetType() == WT_ITEM)
		return;

	// 080822 LUJ, 쿨타임을 시분초 단위로 표시함
	{
		DWORD coolTime = pItem->GetRemainedCoolTime();

		if( coolTime )
		{
			const DWORD oneSecond	= 1000;
			const DWORD oneMinute	= oneSecond * 60;
			const DWORD oneHour		= oneMinute * 60;
			
			char text[ MAX_PATH ]	= { 0 };
			char buffer[ MAX_PATH ] = { 0 };

			// 080820 LUJ, 시 구하기
			{
				const DWORD hour = coolTime / oneHour;

				if( hour )
				{
					sprintf( buffer, CHATMGR->GetChatMsg( 1410 ), hour );
					strcat( text, buffer );

					coolTime = coolTime - hour * oneHour;
				}
			}

			// 080820 LUJ, 분 구하기
			{
				const DWORD minute	= coolTime / oneMinute;

				if( minute )
				{
					sprintf( buffer, CHATMGR->GetChatMsg( 1411 ), minute );
					strcat( text, buffer );

					coolTime = coolTime - minute * oneMinute;
				}
			}

			// 080820 LUJ, 초 구하기
			{
				const float second	= float( coolTime ) / oneSecond;

				if( second )
				{
					sprintf( buffer, "%0.1f%s", second, CHATMGR->GetChatMsg( 795 ) );
					strcat( text, buffer );
				}
			}

			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg( 1658 ),
				text );
			return;
		}
	}
//KES...
	if( m_bDisable )	return;		//인벤 디스에이블이면 아이템 사용 불가능

	if( pItem->GetItemInfo()->EquipType == eEquipType_Pet )
	{
		if( !HEROPET )
			return;

		GAMEIN->GetPetWearedDlg()->FakeMoveIcon( 0, 0, pItem );
		return;
	}	
//KES EXCHANGE 031002

//	
	if( hero->GetState() == eObjectState_Die )	//죽은 상태에서는 아이템 사용 못한다.
		return;

	// 080917 KTH -- 공성존에서 아이템 사용금지 검사
   	if( SIEGEWARFAREMGR->IsSiegeWarfareZone() )
   	{
   		// 공성존의 사용금지 스킬 또는 사용 금지 아이템 이라면 사용 실패를 알린다.
   		if( SIEGEWARFAREMGR->IsFobiddenItem(pItem->GetItemIdx()) )
   		{
   			// 공성 지역에선 사용할 수 없는 아이템입니다. 라는 시스템 메시지를 출력 하여 준다.
   			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1691 ) );
   			return;
   		}
   	}

	if( HOUSINGMGR->IsHousingMap() ) 
	{
		//090527 pdy 하우징 시스템메세지 하우징 맵일때 제한행동 [아이템사용]
		if( !HOUSINGMGR->CanUseItemFromHousingMap(pItem) )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1867 ) );	//1867	"이 지역에서는 사용하실 수 없습니다."
			return;
		}

		//090527 pdy 하우징 시스템메세지 꾸미기 모드일시 제한사항 [아이템사용]
		if( hero->GetState() == eObjectState_Housing )
		if( !HOUSINGMGR->CanUseItemFromDecorationMode(pItem) )
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1874 ) );	//1874	"꾸미기 모드에선 사용하실 수 없습니다."
			return;
		}
	}

	// 080411 LUJ, 아이템 사용 레벨을 체크하도록 한다. 주의: 사용 제한이 없는 아이템은 0으로 설정되어 있다
	{
		const ITEM_INFO* info = pItem->GetItemInfo();

		if( info				&&
			info->LimitLevel	&&
			info->LimitLevel > hero->GetLevel() )
		{
			return;
		}
	}

	// 090422 ShinJS --- 탈것 봉인처리를 위해 추가
	if( pItem->GetItemInfo()->SupplyType != ITEM_KIND_PET &&
		pItem->GetItemInfo()->SupplyType != ITEM_KIND_SUMMON_VEHICLE &&
		pItem->IsLocked() ) 
		return;

	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.09.18
	// ..농장 모션 중일 땐 리턴
	if( g_csFarmManager.CLI_GetIngCharMotion() )
		return;
	// E 농장시스템 추가 added by hseos 2007.09.18

	if( !pItem->GetItemInfo()->Battle && HERO->GetObjectBattleState() == eObjectBattleState_Battle )
		return;

	if( !pItem->GetItemInfo()->Peace && HERO->GetObjectBattleState() == eObjectBattleState_Peace )
		return;

	if( !pItem->GetItemInfo()->Move && HERO->GetBaseMoveInfo()->bMoving )
		return;

	if( !pItem->GetItemInfo()->Stop && !HERO->GetBaseMoveInfo()->bMoving )
		return;

	if( pItem->GetItemInfo()->SupplyType == ITEM_KIND_PET_NORMAL_RESURRECTION ||
		pItem->GetItemInfo()->SupplyType == ITEM_KIND_PET_SPECIAL_RESURRECTION )
	{
		CPetResurrectionDialog* window = GAMEIN->GetPetResurrectionDlg();

		if(		window &&
			!	window->IsActive() )
		{
			window->SetUsedItem( pItem );
			window->SetActive( TRUE );
		}

		return;
	}

    if(g_CGameResourceManager.IsApplyOptionItem(pItem->GetItemIdx()))
	{
		CApplyOptionDialog* window = ( CApplyOptionDialog* )WINDOWMGR->GetWindowForID( APPLY_OPTION_DIALOG );

		if(		window &&
			!	window->IsActive() )
		{
			window->SetSourceItem( *pItem );
			window->SetActive( TRUE );
		}

		return;
	}
	// 080228 LUJ, 조합 보호 아이템일 경우 조합 창 표시
	else if( ITEMMGR->IsMixProtectionItem( pItem->GetItemBaseInfo() ) )
	{
		CMixDialog* window = ( CMixDialog* )WINDOWMGR->GetWindowForID( ITMD_MIXDLG );

		if(		window &&
			!	window->IsActive() )
		{
			window->SetActive( TRUE );

			// 081209 LUJ, 창이 정상적으로 열린 경우에만 보호 모드를 설정한다
			if( window->IsActive() )
			{
				window->SetProtectedActive( *pItem );
			}
		}

		return;
	}
	// 080925 LUJ, 조합 보조 아이템으로 조합 시작
	else if( ITEMMGR->GetMixSupportScript( pItem->GetItemIdx() ) )
	{
		CMixDialog* window = ( CMixDialog* )WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
	
		if(		window &&
			!	window->IsActive() )
		{
			window->SetActive( TRUE );

			// 081209 LUJ, 창이 정상적으로 열린 경우에만 보호 모드를 설정한다
			if( window->IsActive() )
			{
				pItem->SetLock( TRUE );
				window->SetSupportItem( pItem->GetItemBaseInfo() );
			}
		}
	}
	else if(g_CGameResourceManager.IsReinforceProtectionItem(pItem->GetItemBaseInfo()))
	{
		CReinforceDlg* window = ( CReinforceDlg* )WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );

		if(	window )
		{
			window->SetActive( TRUE );

			// 081209 LUJ, 창이 정상적으로 열린 경우에만 보호 모드를 설정한다
			if( window->IsActive() )
			{
				window->SetProtectedActive( *pItem );
			}
		}

		return;
	}
	else if(g_CGameResourceManager.GetReinforceSupportScript(pItem->GetItemIdx()))
	{
		CReinforceDlg* dialog = ( CReinforceDlg* )WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );

		if(		dialog &&
			!	dialog->IsActive() )
		{
			dialog->SetActive( TRUE );

			// 081209 LUJ, 창이 정상적으로 열린 경우에만 보조 모드를 설정한다
			if( dialog->IsActive() )
			{
				pItem->SetLock( TRUE );
				dialog->SetSupportItem( pItem->GetItemBaseInfo() );
			}
		}

		return;
	}
	else if( GAMERESRCMNGR->IsComposeKeyItem( pItem->GetItemIdx() ) )
	{
		CComposeDialog* dialog = GAMEIN->GetComposeDialog();

		if(		dialog &&
			!	dialog->IsActive() )
		{
			pItem->SetLock( TRUE );

			dialog->SetKeyItem( pItem->GetItemBaseInfo() );
			dialog->SetActive( TRUE );
		}

		return;
	}

	// 080228 LUJ, 인챈트 보호 아이템일 경우 인챈트 창 표시
	// 080320 LUJ, 인챈트 제외 보호/전체 보호 상태에 따라 시스템 메시지 분리
	else
	{
		const EnchantProtection& protection	= g_CGameResourceManager.GetEnchantProtection();
		const EnchantProtection::ItemMap::const_iterator it	= protection.mItemMap.find(
			pItem->GetItemIdx());
		CEnchantDialog* window = (CEnchantDialog*)WINDOWMGR->GetWindowForID(
			ITD_UPGRADEDLG);

		// 080328 LUJ, 인챈트 보호 종류를 한 가지로 함
		if(	window &&
			protection.mItemMap.end() != it )
		{
			window->SetActive( TRUE );

			if( window->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1454 ) );

				window->SetProtectedActive( *pItem );
				return;
			}
		}		
	}
      
	// 080414 LUJ, 외양 변환 아이템일 경우 해당 창 표시
	{
		typedef CItemManager::BodyChangeScriptListMap BodyChangeScriptListMap;

		const BodyChangeScriptListMap&			scriptListMap	= ITEMMGR->GetBodyChangeScriptListMap();
		BodyChangeScriptListMap::const_iterator	it				= scriptListMap.find(  pItem->GetItemIdx() );

		CBodyChangeDialog*	window = ( CBodyChangeDialog* )WINDOWMGR->GetWindowForID( BODY_CHANGE_DIALOG );
    
		cDialog* stallShopDialog	= WINDOWMGR->GetWindowForID( SSI_STALLDLG );	//노점
 
		if(	window &&
			scriptListMap.end() != it )
		{
			if( !stallShopDialog->IsActive() )		//100525 개인상점이 열려있다면 외양 변환아이템 사용하지 않도록 한다.
			{
				window->SetActive( TRUE, *pItem );

				SetActive( FALSE );
				ITEMMGR->SetDisableDialog( TRUE, eItemTable_Inventory );
			}
			else
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1078 ) );
			}
			return;
		}
	}

	switch( pItem->GetItemInfo()->SupplyType )
	{
	case ITEM_KIND_PET_SKILL:					
	case ITEM_KIND_PET_COSTUME:				
	case ITEM_KIND_PET_EQUIP:					
	case ITEM_KIND_PET_EGG:					
	case ITEM_KIND_PET_FOOD:				
	case ITEM_KIND_PET_HP_RECOVER:			
	case ITEM_KIND_PET_MP_RECOVER:			
		{
			if( !HEROPET )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(360));
				return;
			}
		}
		break;
	case ITEM_KIND_PET_EVOLUTION_PYHSIC_TYPE:	
	case ITEM_KIND_PET_EVOLUTION_MAGIC_TYPE:	
	case ITEM_KIND_PET_EVOLUTION_SUPPORT_TYPE:
		{
			if( !HEROPET )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(360));
				return;
			}
			if( HEROPET->GetLevel() != PETMGR->GetGradeUpLevel( HEROPET->GetGrade() ) )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(789));
				return;
			}

			if( HEROPET->GetType() != ePetType_Basic &&
				HEROPET->GetType() != pItem->GetItemInfo()->SupplyType - ITEM_KIND_PET_EVOLUTION_PYHSIC_TYPE + 1 )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1577));
				return;
			}
		}
		break;
	}

	switch( pItem->GetItemInfo()->Category )
	{
	case eItemCategory_Equip:
		{
			if( OBJECTSTATEMGR->CheckAvailableState(hero,eCAS_EQUIPITEM) == FALSE )
			{
				CAction act;
				act.InitAction(eActionKind_EquipItem,(void*)pItem->GetDBIdx(),0,NULL);
				hero->SetNextAction(&act);
				return;
			}

			// 장착 아이템	
			switch( GetTableIdxPosition( pItem->GetPosition() ) )
			{
			case eItemTable_Inventory:
				{
					ITEM_INFO * pInfo = ITEMMGR->GetItemInfo(pItem->GetItemIdx()) ;
					if(!pInfo) return ;

					BYTE byEquipResult = ITEMMGR->CanEquip(pItem->GetItemIdx()) ;
					if( byEquipResult == eEquip_Able )
					{
						EWEARED_ITEM EquipToPos = pInfo->EquipSlot;
						CItem * pToItem = GetItemForPos(pInfo->EquipSlot + TP_WEAR_START);

						switch(EquipToPos)
						{
						case eWearedItem_Ring1:
							{
								if(0 == GetItemForPos(eWearedItem_Ring2 + TP_WEAR_START))
								{
									pToItem = GetItemForPos(eWearedItem_Ring2 + TP_WEAR_START );
									EquipToPos = eWearedItem_Ring2;
								}

								break;
							}
						case eWearedItem_Earring1:
							{
								if( !GetItemForPos(eWearedItem_Earring2 + TP_WEAR_START))
								{
									pToItem = GetItemForPos(eWearedItem_Earring2 + TP_WEAR_START);
									EquipToPos = eWearedItem_Earring2;
								}
								break;
							}
						case eWearedItem_Shield:
							{
								if( hero->GetWeaponAniType() == eWeaponAnimationType_TwoHand ||
									hero->GetWeaponAniType() == eWeaponAnimationType_Staff ||
									hero->GetWeaponAniType() == eWeaponAnimationType_Bow ||
									hero->GetWeaponAniType() == eWeaponAnimationType_Gun ||
									// 090521 ONS 신규종족 무기 타입 속성 추가
									hero->GetWeaponAniType() == eWeaponAnimationType_BothDagger )
								{
									CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(772) );
									return;
								}
								break;
							}
						}
						
						FakeGeneralItemMove(EquipToPos + TP_WEAR_START, pItem, pToItem);
					}
					else
					{
						// 090114 LYW --- InventoryExDialog : 아이템 착용 가능 체크 함수 수정에 따른 처리.
						switch( byEquipResult )
						{
						case eEquip_RaceFail :			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(823), pInfo->ItemName) ;	break ;
						case eEquip_GenderFail :		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(824), pInfo->ItemName) ;	break ;
						case eEquip_LevelFail :			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(825), pInfo->ItemName) ;	break ;
						case eEquip_FishingLevelFail :	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1530)) ;					break ;
						default :						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(73)) ;						break ;
						}
					}
				}
				break;
			case eItemTable_Weared:
				{
					POSTYPE emptyPos;
					if( GetBlankPositionRestrictRef( emptyPos ) )
					{
						CItem * pToItem = GetItemForPos( emptyPos );
						FakeGeneralItemMove(emptyPos, pItem, pToItem);
					}
				}
				break;
			}
		}
		break;
	case eItemCategory_Expend:
		{
			// desc_hseos_농장시스템_01
			// S 농장시스템 추가 added by hseos 2007.05.14	2007.08.21
			// ..농장 아이템 일부는 타겟을 설정하고 사용해야 하기 때문에 기존과 조금 다르다. 변수 추가
			const ITEM_INFO * pInfo = ITEMMGR->GetItemInfo(pItem->GetItemIdx());

			if( ! pInfo )
			{
				return;
			}

			// 100224 ONS 소모성 아이템에도 성별구분 처리 추가.
			if(pInfo->LimitGender)
			{
				if(pInfo->LimitGender != HERO->GetGender()+1)
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(2036), pInfo->ItemName) ;
					return ;
				}
			}

			switch( pInfo->SupplyType )
			{
			case ITEM_KIND_BUFF_ITEMS :
				{
					// 길드토너먼트맵 에서는 사용불가.
					if(MAP->GetMapNum()==GTMAPNUM)
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(91)) ;
						return;
					}

					// 071224 LYW --- InventoryExDialog : 파티중이면, 사용 못한다는 에러 메시지 출력을 한다.
					if( HERO->GetPartyIdx() != 0 )
					{
						const ITEM_INFO* pGetItemInfo = pItem->GetItemInfo() ;

						if( !pGetItemInfo )
						{
							MessageBox(NULL, "Failed to receive iinfo", "Use item", MB_OK) ;
							return ;
						}

						if( !SKILLMGR->IsUseableBuff_InParty(pGetItemInfo->SupplyValue) )
						{
							WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1252) ) ;
							return ;
						}
					}

					if( HERO->GetState()==eObjectState_Fishing || HERO->GetState()==eObjectState_FishingResult )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					Set_QuickSelectedItem(pItem) ;

					if( ITEMMGR->IsUsingItem(pItem) )
					{
						WINDOWMGR->MsgBox( MBI_FORCE_USEBUFFITEM_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1224) ) ;

						// 080728 LUJ, 인벤토리를 사용할 수 없도록 잠근다
						SetDisable( TRUE );
						return ;
					}

					ASSERT(pInfo) ;

					char tempBuf[128] = {0, } ;
					char newItemName[128] = {0, } ;

					char* pPersent = NULL ;

					pPersent = strchr( pInfo->ItemName, '%' ) ;
					if( !pPersent )
					{
						WINDOWMGR->MsgBox( MBI_USEBUFFITEM_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1244), pInfo->ItemName ) ;

						// 080728 LUJ, 인벤토리를 사용할 수 없도록 잠근다
						SetDisable( TRUE );
					}
					else
					{
						strncpy(tempBuf, pInfo->ItemName, strlen(pInfo->ItemName) - strlen(pPersent)) ;

						sprintf(newItemName, "%s%c", tempBuf, '%') ;
						strcat(newItemName, pPersent) ;

						memset(tempBuf, 0, sizeof(tempBuf)) ;
						sprintf(tempBuf, CHATMGR->GetChatMsg(1244), newItemName) ;

						WINDOWMGR->MsgBox( MBI_USEBUFFITEM_AREYOUSURE, MBT_YESNO, tempBuf ) ;

						// 080728 LUJ, 인벤토리를 사용할 수 없도록 잠근다
						SetDisable( TRUE );
					}
				}
				return ;

			case ITEM_KIND_FOOD:
				{
					Set_QuickSelectedItem(pItem) ;
					ITEMMGR->UseItem_ItemMallItem(pItem, HERO) ;
					return;
				}
				return;

			case ITEM_KIND_GTBUFF_ITEMS:
				{
					CObject* pObject = OBJECTMGR->GetSelectedObject();
					if(!pObject)
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1037)) ;
						return;
					}
					
					if(pObject->GetObjectKind() != eObjectKind_Player)
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1043)) ;
						return;
					}

					// 071224 LYW --- InventoryExDialog : 파티중이면, 사용 못한다는 에러 메시지 출력을 한다.
					if( HERO->GetPartyIdx() != 0 )
					{
						const ITEM_INFO* pGetItemInfo = pItem->GetItemInfo() ;

						if( !pGetItemInfo )
						{
							MessageBox(NULL, "Failed to receive iinfo", "Use item", MB_OK) ;
							return ;
						}

						if( !SKILLMGR->IsUseableBuff_InParty(pGetItemInfo->SupplyValue) )
						{
							WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1252) ) ;
							return ;
						}
					}

					if( HERO->GetState()==eObjectState_Fishing || HERO->GetState()==eObjectState_FishingResult )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					Set_QuickSelectedItem(pItem) ;
					ITEMMGR->UseItem_ItemMallItem(pItem, OBJECTMGR->GetSelectedObject()) ;
					return;
				}
				return;

			// 100511 ONS 전직 초기화 아이템 추가
			case ITEM_KIND_CHANGE_CLASS:
				{
					if( HERO->GetState() != eObjectState_None )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					// 080124 KTH -- 피케이 모드일때에는 사용할 수 없습니다.
					if( HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(1263) ) ;
						return;
					}

					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;

					CChangeClassDlg* pDlg = GAMEIN->GetChangeClassDlg();
					if(!pDlg) return;

					pDlg->SetDisable( FALSE );
					pDlg->SetActive( TRUE );
				}
				return;

			// 071203 LYW --- InventoryExDialog : 스탯 초기화 아이템 추가.
			case ITEM_KIND_RESET_STATS :
				{
					if( HERO->GetState() != eObjectState_None )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					// 080124 KTH -- 피케이 모드일때에는 사용할 수 없습니다.
					if( HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(1263) ) ;
						return;
					}

					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;
					WINDOWMGR->MsgBox( MBI_RESETSTATS_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1223) ) ;
					return ;
				}
				return ;

			// 071203 LYW --- InventoryExDialog : 스킬 초기화 아이템 추가.
			case ITEM_KIND_RESET_SKILL :
				{
					if( HERO->GetState() != eObjectState_None )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}
					
					// 080124 KTH -- 피케이 모드일때에는 사용할 수 없습니다.
					if( HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(1263) ) ;
						return;
					}

					// 080703 LUJ, 이도류 장비를 착용한 상태로 스킬 초기화를 할 경우, 필요한 스킬이
					//				없는데도 이도류 상태가 된다. 이 때문에 이도류인 경우에는 스킬 초기화를
					//				할 수 없다
					{
						const ITEMBASE* weaponItem = ITEMMGR->GetItemInfoAbsIn( HERO, TP_WEAR_START + eWearedItem_Weapon );
						const ITEMBASE* shieldItem = ITEMMGR->GetItemInfoAbsIn( HERO, TP_WEAR_START + eWearedItem_Shield );
						
						const ITEM_INFO* weaponItemInfo	= ( weaponItem ? ITEMMGR->GetItemInfo( weaponItem->wIconIdx ) : 0 );
						const ITEM_INFO* shieldItemInfo	= ( shieldItem ? ITEMMGR->GetItemInfo( shieldItem->wIconIdx ) : 0 );

						if( weaponItemInfo &&
							shieldItemInfo &&
							weaponItemInfo->WeaponType == shieldItemInfo->WeaponType )
						{
							// 080714 LUJ, 리소스가 잘못 업데이트되어 메시지 번호를 변경
							CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1590 ) );
							return;
						}
					}

					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;
					WINDOWMGR->MsgBox( MBI_RESETSKILL_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1222) ) ;
					return ;
				}
				return ;

			// 080312 NYJ --- 전체외치기(월드확성기) 아이템
			case ITEM_KIND_WORLDSHOUT:
				{
					ITEMMGR->UseItem_WorldShout(pItem);
				}
				return;

			case ITEM_KIND_FISHING_BAIT:
				{
					CFishingDialog*	window = GAMEIN->GetFishingDlg();

					if(	window && !FISHINGMGR->IsActive())
					{
						//  낚시 창이 닫힌상태면 낚시창 열기
						window->SetActive( TRUE );
						window->ChangeBaitItem(pItem);
					}
					else if( window && FISHINGMGR->IsActive())
					{
						// 낚시 창이 열린상태면 미끼등록 or 변경
						window->ChangeBaitItem(pItem);
					}
				}
				return;

			case ITEM_KIND_FISHING_FISH:
				{
					CHATMGR->AddMsg( CTC_EXPENDEDITEM, CHATMGR->GetChatMsg(772));
					return;
				}
				return;

			case ITEM_KIND_RESET_REGIDENT_REGIST :
				{
					if( HERO->GetState() != eObjectState_None )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}
					
					// 080124 KTH -- 피케이 모드일때에는 사용할 수 없습니다.
					if( HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(1263) ) ;
						return;
					}

					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;
					WINDOWMGR->MsgBox( MBI_RESET_REGIDENTREGIST, MBT_YESNO, CHATMGR->GetChatMsg(1244), pItem->GetItemInfo()->ItemName) ;
					return ;
				}
				return ;

			// 071201 LYW --- InventoryExDialog : 맵 이동 주문서 처리 추가.
			case ITEM_KIND_MAP_MOVE_SCROLL :
				{
					if( HERO->GetState() != eObjectState_None || HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					ITEMMGR->UseItem_MapMoveScroll(pItem) ;
				}
				return ;

			// 071206 LYW --- InventoryExDialog : 창고 확장 주문서 처리 추가.
			case ITEM_KIND_EXTEND_STORAGE :
				{
					BYTE byNum = 0 ;
					byNum = HERO->GetStorageNum() ;

					if( 2 > byNum || byNum >= MAX_STORAGELIST_NUM )
					{
						WINDOWMGR->MsgBox( MBI_RESETSKILL_AREYOUSURE, MBT_OK, CHATMGR->GetChatMsg(1243) ) ;
						return ;
					}

					ITEMMGR->UseItem_ExtendStorage(pItem) ;
				}
				return ;

			// 071210 LYW --- InventoryExDialog : 인벤토리 확장 주문서 처리 추가.
			case ITEM_KIND_EXTEND_INVEN :
				{
					// HERO의 인벤토리 개수를 가져 온다.
					int nInvenCount = HERO->Get_HeroExtendedInvenCount() ;

					if( HERO->GetState() != eObjectState_None )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					if( nInvenCount >= 2)
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1248) ) ;
						return ;
					}
					else
					{
						pItem->SetLock(TRUE) ;
						Set_QuickSelectedItem(pItem) ;

						WINDOWMGR->MsgBox( MBI_EXTENDINVEN_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1247) ) ;
						return ;
					}
				}
				return ;

			// 071228 LYW --- InventoryExDialog : 이름 변경 아이템 사용 처리 추가.
			case ITEM_KIND_CHANGE_NAME :
				{
					if( HERO->GetState() != eObjectState_None )
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					if( HERO->GetPartyIdx() != 0 )
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1258) ) ;
						return ;
					}

					if( HERO->GetGuildIdx() != 0 )
					{
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1257) ) ;
						return ;
					}

					CSHFamily* pFamily = NULL ;
					pFamily = HERO->GetFamily() ;

					if( pFamily )
					{
						if( pFamily->Get()->nMasterID != 0 )
						{
							WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1259) ) ;
							return ;
						}
					}

					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;

					cChangeNameDialog* pChangeNameDlg = NULL ;
					pChangeNameDlg = GAMEIN->GetChangeNameDlg() ;

					if( !pChangeNameDlg ) return ;

					pChangeNameDlg->Clear_NameBox() ;
					pChangeNameDlg->SetActive(TRUE) ;
				}
				return ;

				// ..농작물 심기
			case ITEM_KIND_FARM_PLANT:
				g_csFarmManager.CLI_RequestPlantCropInGarden(pItem->GetPosition(), pItem->GetItemIdx());
				return;
			// ..농작물 비료 주기
			case ITEM_KIND_FARM_MANURE:
				g_csFarmManager.CLI_RequestManureCropInGarden(pItem->GetPosition(), pItem->GetItemIdx());
				return;
			// ..농작물 수확
			case ITEM_KIND_FARM_HARVEST:
				g_csFarmManager.CLI_RequestHarvestropInGarden(pItem->GetPosition(), pItem->GetItemIdx());
				return;
			// 080421 KTH 
			case ITEM_KIND_ANIMAL:
				//if( g_csFarmManager.CLI_GetTargetObj()->GetInfo()->nKind == CSHFarmManager::RENDER_BASIC_ANIMAL_CAGE_SLOT_KIND )
				{
					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;
					WINDOWMGR->MsgBox( MBI_FARM_ANIMAL_INSTALL, MBT_YESNO, CHATMGR->GetChatMsg( 1571 ) );
					//g_csFarmManager.CLI_RequestInstallAnimalInAnimalCage(pItem->GetPosition(), pItem->GetItemIdx());
				}
				return;

			// 080415 KTH ---
			// ..가축 사료 주기
			case ITEM_KIND_ANIMAL_FOOD:
				g_csFarmManager.CLI_RequestFeedAnimalInCage(pItem->GetPosition(), pItem->GetItemIdx());
				return;
			//080515 KTH
			// ..가축 청소 아이템
			case ITEM_KIND_ANIMAL_CLEANER:
				g_csFarmManager.CLI_RequestCleaningAnimalInCage(pItem->GetPosition(), pItem->GetItemIdx());
				return;
			case ITEM_KIND_OPENSTORAGE_BYITEM : 
				{
					cDialog* storageDialog = WINDOWMGR->GetWindowForID( PYO_STORAGEDLG );
	
					if( ! storageDialog )
					{
						return;
					}
					else if( storageDialog->IsActive() )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1552 ) );
						return;
					}
					else if( ! HERO->GetStorageNum() )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1551 ) );
						return;
					}
					// 090403 ONS 조합,분해,강화,인챈트 다이얼로그가 활성화 되어 있는 상태에서 창고소환 아이템 사용 불가
					else
					{
						cDialog* pMixDialog				= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );			// 조합
						cDialog* pDissolutioniDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );	// 분해
						cDialog* pReinforceDialog		= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );	// 강화
						cDialog* pUpgradeDialog			= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );		//인첸트

						if( !pMixDialog				|| 
							!pDissolutioniDialog	|| 
							!pReinforceDialog		|| 
							!pUpgradeDialog	) 
						{
								return;
						}

						if(	pMixDialog->IsActive()			|| 
							pDissolutioniDialog->IsActive() || 
							pReinforceDialog->IsActive()	|| 
							pUpgradeDialog->IsActive()		)
						{
							CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1080 ) );
							return;
						}
					}
				}	
				break;
			case ITEM_KIND_GET_SKILLPOINT :
				{
					ITEMMGR->GetSkillPointByItem(pItem) ;
				}
				return ;

				// 080826 LYW --- InventoryExDialog : 워터시드 사용 추가.
   			case ITEM_KIND_SIEGEWARFARE_WATERSEED :
   				{
   					// 아이템 사용 요청을 보낸다.
   					ITEMMGR->UseItem_WarterSeed(pItem) ;
   					return ;
   				}
   				break ;
			case ITEM_KIND_FURNITURE :
				{
					//090527 pdy 하우징 시스템메세지 ITEM_KIND_FURNITURE은 사용불가능 [ITEM_KIND_FURNITURE]
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1906) );	//1906	"가구아이템은 인벤토리에서 사용 불가능"
					return;

				}
				break ;

			//090518 pdy 하우징 머터리얼 교체 
			case ITEM_KIND_FURNITURE_WALLPAPER :
			case ITEM_KIND_FURNITURE_FLOORPAPER :
			case ITEM_KIND_FURNITURE_CEILINGPAPER :
				{
					//090527 pdy 하우징 시스템메세지 하우징 맵이 아닐때 제한행동 [머터리얼교체]
					if( HOUSINGMGR->IsHousingMap() == FALSE )
					{
						CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1867) );	//1867	"이 지역에서는 사용하실 수 없습니다."
						return;
					}

					// 091019 pdy 하우징맵에서 로딩중에 각종 하우징 기능 사용금지 추가
					if( HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
					{
						return;
					}

					//090527 pdy 하우징 시스템메세지 꾸미기 모드가 아닐시 제한사항 [머터리얼교체]
					if( HERO->GetState() != eObjectState_Housing  )
					{
						CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1878) );	//1878	"꾸미기 모드일때만 사용하실 수 있습니다."
						return;
					}
				//090527 pdy 하우징 시스템메세지 탑승상태시 제한사항 [머터리얼교체]
					if( HERO->GetState() == eObjectState_HouseRiding  )
					{
						CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1876) ); //1876	"탑승 상태를 해제한 후 사용하셔야 합니다."
						return;
					}

					ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( pItem->GetItemIdx() );
					if( pInfo && HOUSINGMGR->IsHouseInfoLoadOk() && HOUSINGMGR->IsHouseOwner() ) 
					{
						CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();

						if( ! pDlg )
							return;

						pDlg->SetDisable(TRUE);
						pItem->SetLock(TRUE) ;
						Set_QuickSelectedItem(pItem) ;
						HOUSINGMGR->ChangeMaterialFurniture(pInfo->SupplyType , pInfo->SupplyValue, 0 );
						//090527 pdy 하우징 팝업창 [머터리얼 변경]
						WINDOWMGR->MsgBox( MBI_HOUSE_CHANGEMATERIAL_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1896)) ; //1896	"재질을 변경하시겠습니까?"
					}
					return;
				}
				break;
			case ITEM_KIND_VISIT_HOUSE:				
				{
					// 091019 pdy 하우징맵에서 로딩중에 각종 하우징 기능 사용금지 추가
					if( HOUSINGMGR->IsHousingMap() &&	
						HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
					{
						return;
					}

					if( HERO->GetState() != eObjectState_None || HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					if(HERO->GetVehicleID())
					{
						CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1839) ); //1839	"탑승 도구를 이용 시에는 해당 기능을 사용하실 수 없습니다."
						return;
					}

					CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
					if( ! pDlg )
						return;

					cHouseSearchDlg* pSearchDlg = GAMEIN->GetHouseSearchDlg();
					if( ! pSearchDlg )
						return;

					//이미 사용중이면 닫는다 
					if( pSearchDlg->IsActive() ) 
						pSearchDlg->SetActive(FALSE);
		

					pSearchDlg->SetVisitInfo(eHouseVisitByItem ,pItem->GetItemIdx() , pItem->GetPosition() );
					pSearchDlg->SetActive(TRUE);

					pDlg->SetDisable(TRUE);
					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;

					return;
				}
				break;
			case ITEM_KIND_VISIT_HOME:			
				{
					// 091019 pdy 하우징맵에서 로딩중에 각종 하우징 기능 사용금지 추가
					if( HOUSINGMGR->IsHousingMap() &&	
						HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
					{
						return;
					}

					if( HERO->GetState() != eObjectState_None || HERO->IsPKMode() )
					{
						WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
						return ;
					}

					if(HERO->GetVehicleID())
					{
						CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1839) ); //1839	"탑승 도구를 이용 시에는 해당 기능을 사용하실 수 없습니다."
						return;
					}

					CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
					if( ! pDlg )
						return;

					//090527 pdy 하우징 시스템메세지 집주인일시 제한사항 [내집방문]
					if( HOUSINGMGR->IsHouseOwner() )	
					{
						CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1904) ); //1904	"이미 내집에 들어와있습니다."
						return;
					}

					pDlg->SetDisable(TRUE);
					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;

					//090527 pdy 하우징 팝업창 [자기집방문]
					WINDOWMGR->MsgBox( MBI_HOUSE_VISIT_MYHOME_AREYOUSURE,MBT_YESNO, CHATMGR->GetChatMsg(1897)); //1897	"집으로 이동하시겠습니까?"
					OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal) ;
					return;
				}
				break;
			case ITEM_KIND_EXTEND_HOUSE :
				{
					//090527 pdy 하우징 시스템메세지 하우징 맵이 아닐때 제한행동 [하우징 확장]
					if( HOUSINGMGR->IsHousingMap() == FALSE )
					{
						CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1867) );	//1867	"이 지역에서는 사용하실 수 없습니다."
						return;
					}

					// 091019 pdy 하우징맵에서 로딩중에 각종 하우징 기능 사용금지 추가
					if( HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
					{
						return;
					}

					//090527 pdy 하우징 시스템메세지 최대확장 단계 제한사항 [하우징 확장]
					if(! HOUSINGMGR->CanExtendHouse() )
					{
						CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1866) );	//1866	"더 이상 확장 하실 수 없습니다."
						return;
					}

					//090527 pdy 하우징 시스템메세지 꾸미기 모드일시 제한사항 [하우징 확장]
					if( HERO->GetState() == eObjectState_Housing  )
					{
						CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1874) );	//1874	"꾸미기 모드에선 사용하실 수 없습니다."
						return;
					}

					CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();

					if( ! pDlg )
						return;

					pDlg->SetDisable(TRUE);
					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;

					//090527 pdy 하우징 팝업창 [하우스확장]
					WINDOWMGR->MsgBox( MBI_HOUSE_EXTEND_AREYOUSURE, MBT_YESNO,  CHATMGR->GetChatMsg(1898) ); //1898	"집을 확장 하시겠습니까?"

					return;
				}
				break;
			case ITEM_KIND_DUNGEON_KEY:
				{
					stDungeonKey* pKey = GAMERESRCMNGR->GetDungeonKey(pInfo->SupplyValue);
					if(!pKey)
						return;

					if(MAP->GetMapNum() == pKey->wMapNum)
					{
						CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1977) );
						return;
					}

					MSG_DWORD4 msg;
					msg.Category = MP_DUNGEON;
					msg.Protocol = MP_DUNGEON_ENTRANCE_SYN;
					msg.dwData1 = pKey->dwIndex;
					msg.dwData2 = pKey->wMapNum;
					msg.dwData3 = pItem->GetItemIdx();
					msg.dwData4 = pItem->GetPosition();
					NETWORK->Send(&msg,sizeof(msg));
					return;
				}
				break;
			// 090422 ShinJS --- 탈것 아이템 사용
			case ITEM_KIND_SUMMON_VEHICLE :
				{
					// 이미 소환되어 있는 경우 봉인 처리
					if( pItem->IsLocked() )
					{
						// 090423 ShinJS --- 탈것 봉인 가능여부 확인
						if( !HERO->CanSealVehicle() )
							return;

						StartVehicleSeal( HERO->GetVehicleID() );
						return;
					}

					// 090423 ShinJS --- 소환 가능상태 확인
					if( !HERO->CanSummonVehicle() )
						return;

					WORD wMonsterKind = WORD( pItem->GetItemInfo()->SupplyValue );
					const VehicleScript& script = GAMERESRCMNGR->GetVehicleScript( wMonsterKind );

					// 탈것 스크립트 확인
					if( script.mMonsterKind == wMonsterKind )
					{
						// 탈것 설치 시작
						OBJECTMGR->StartVehicleInstall( wMonsterKind, pItem );
					}
					return;
				}
				break;

			case ITEM_KIND_LEVEL_UP:
				{
					if(HERO->GetLevel() >= MAX_CHARACTER_LEVEL_NUM)
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2019 ) );
						return;
					}
				}
				break;

			case ITEM_KIND_GUILDPOINT_UP:
				{
					if( HERO->GetGuildIdx() == 0 )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2020 ) );
						return ;
					}
				}
				break;

			case ITEM_KIND_FAMILYPOINT_UP:
				{
					if( HERO->GetFamilyIdx() == 0 )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2021 ) );
						return ;
					}
				}
				break;
			
			case ITEM_KIND_EXP_UP:
				{
					if(HERO->GetLevel() >= MAX_CHARACTER_LEVEL_NUM)
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2019 ) );
						return;
					}
				}
				break;

			case ITEM_KIND_PCROOMPOINT_UP:
				{
					CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
					cWindow* pMallBtn = pMiniMapDlg->GetWindowForID( MNM_PCROOM_MALL_BTN );
					if( !pMallBtn )
						break;

					if(! pMallBtn->IsActive())
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2039 ) );
						return;
					}

					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;
				}
				break;

			case ITEM_KIND_FISHPOINT_UP:
				{
					if(FISHINGMGR->GetFishingLevel() >= MAX_FISHING_LEVEL)
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2040 ) );
						return;
					}
				}
				break;

			case ITEM_KIND_COOKLEVEL_UP:
				{
					if(COOKMGR->GetCookLevel() >= MAX_COOKING_LEVEL)
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2040 ) );
						return;
					}
				}
				break;

			case ITEM_KIND_DECOUSEPOINT_UP:
				{
					if(! HOUSINGMGR->IsHousingMap())
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2041 ) );
						return;
					}
				}
				break;

			case ITEM_KIND_PKPOINT_UP:
			case ITEM_KIND_PKPOINT_DN:
				{
					pItem->SetLock(TRUE) ;
					Set_QuickSelectedItem(pItem) ;
				}
				break;
			}

			// E 농장시스템 추가 added by hseos 2007.05.14	2007.08.21

			{
				mMessage.Category = MP_ITEM;
				// 080820 LUJ, 획득 시 쿨타임을 얻는 아이템은 변환 시 다른 프로토콜을 적용한다
				mMessage.Protocol = ( ITEM_KIND_COOLTIME == pInfo->SupplyType ? MP_ITEM_USE_CHANGEITEM_SYN : MP_ITEM_USE_SYN );
				mMessage.dwObjectID = gHeroID;
				mMessage.TargetPos = pItem->GetPosition();
				mMessage.wItemIdx = pItem->GetItemIdx();
			
				if( pItem->GetItemInfo()->SupplyType == ITEM_KIND_PET )
				{
					if( HEROPET && pItem->GetDBIdx() != HEROPET->GetItemDBIdx() )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1598) );
						return;
					}

					if( PETMGR->GetLastSummonTime() != 0 && gCurTime - PETMGR->GetLastSummonTime() < PETMGR->GetSummonPenaltyDelay() )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1597) );
						return;
					}

					if( HERO->GetObjectBattleState() == eObjectBattleState_Battle )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1599) );
						return;
					}

					PET_OBJECT_INFO* pObjectInfo = PETMGR->GetPetObjectInfo( pItem->GetDBIdx() );

					if( !pObjectInfo || pObjectInfo->State == ePetState_Die )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(789) );
						return;
					}
			
					if( !HEROPET && HERO->GetLevel() < PETMGR->GetSummonPlayerLevel( pObjectInfo->Grade ) )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1600) );
						return;
					}

					SetDisable( TRUE );

					// 071213 KTH -- 딜레이 5초에서 2초로 바뀜.
					//const DWORD changeSecond = 2000;

					//SetCoolTime( pItem->GetItemIdx(), changeSecond );
					SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );

					{
						cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
						ASSERT( dialog );

						//dialog->SetCoolTime( pItem->GetItemIdx(), changeSecond );
						dialog->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );
					}

					{
						CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
						ASSERT( dialog );

						if( HEROPET )
						{
							dialog->SetText( RESRCMGR->GetMsg( 1007 ) );
						}
						else
						{
							dialog->SetText( RESRCMGR->GetMsg( 1006 ) );
						}
						dialog->Wait( CProgressDialog::eActionPetSummon );
					}
				}
				else if( pItem->GetItemInfo()->SupplyType == ITEM_KIND_CHANGESIZE_UPWEIGHT ||
						pItem->GetItemInfo()->SupplyType == ITEM_KIND_CHANGESIZE_DNWEIGHT ||
						pItem->GetItemInfo()->SupplyType == ITEM_KIND_CHANGESIZE_DEFALUT)
				{
					// 길토맵에선 키변환 관련 아이템 사용을 금한다
					if( MAP->GetMapNum()==GTMAPNUM )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 91 ) );
						return;
					}
					// 공성맵에서도 키변환 관련 아이템 사용을 금한다
					else if( SIEGEWARFAREMGR->IsSiegeWarfareKindMap(MAP->GetMapNum()) && 
							 SIEGEWARFAREMGR->Is_CastleMap() == FALSE )
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 91 ) );
						return;
					}

					SetDisable( TRUE );

					SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );
	
					cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
					ASSERT( dialog );

					//dialog->SetCoolTime( pItem->GetItemIdx(), changeSecond );
					dialog->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );

					// 080706 LYW --- InventoryExDialog : 확장 퀵 슬롯 추가 처리.
					cQuickSlotDlg* dialog2 = NULL ;
					dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
					ASSERT(dialog2) ;

					if(dialog2)
					{
						dialog2->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND ) ;
					}

					{
						CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
						ASSERT( dialog );

						dialog->Wait( CProgressDialog::eActionChangeSize );
					}
				}
				else if( pItem->GetItemInfo()->SupplyType == ITEM_KIND_SUMMON_MONSTER )
				{
					SetDisable( TRUE );

					SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );
	
					cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
					ASSERT( dialog );

					//dialog->SetCoolTime( pItem->GetItemIdx(), changeSecond );
					dialog->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND);

					// 080706 LYW --- InventoryExDialog : 확장 퀵 슬롯 추가 처리.
					cQuickSlotDlg* dialog2 = NULL ;
					dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
					ASSERT(dialog2) ;

					if(dialog2)
					{
						dialog2->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND ) ;
					}

					{
						CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
						ASSERT( dialog );

						dialog->Wait( CProgressDialog::eActionSummonMonster );
					}
				}
				else if(pItem->GetItemInfo()->SupplyType == ITEM_KIND_SUMMON_EFFECT)
				{
					// 100125 ShinJS --- 정지 후 Progress 사용하지 않고 실행하도록 수정
					MOVEMGR->HeroMoveStop();

					SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );
	
					cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
					ASSERT( dialog );

					dialog->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND);

					// 080706 LYW --- InventoryExDialog : 확장 퀵 슬롯 추가 처리.
					cQuickSlotDlg* dialog2 = NULL ;
					dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
					ASSERT(dialog2) ;

					if(dialog2)
					{
						dialog2->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND ) ;
					}

					NETWORK->Send( &mMessage, sizeof( mMessage ) );
				}

				else if( pItem->GetItemInfo()->SupplyType == ITEM_KIND_PET_EVOLUTION_PYHSIC_TYPE ||
						 pItem->GetItemInfo()->SupplyType == ITEM_KIND_PET_EVOLUTION_MAGIC_TYPE ||
						 pItem->GetItemInfo()->SupplyType == ITEM_KIND_PET_EVOLUTION_SUPPORT_TYPE )
				{
					SetDisable( TRUE );

					WINDOWMGR->MsgBox( MBI_PETGRADEUP, MBT_YESNO, CHATMGR->GetChatMsg(1585) ) ;
				}
				else
				{
					NETWORK->Send( &mMessage, sizeof( mMessage ) );
				}
			}
		}
		break;
	default:
		{
		}
		break;
	}
}

void CInventoryExDialog::SetActive(BOOL val)
{
	// 080221 LYW --- MacroManager : 인벤토리 확장에 따른 처리.
	int nExtendedInven = 0 ;
	nExtendedInven = HERO->Get_HeroExtendedInvenCount() ;

	ShowTab(2,FALSE) ;
	ShowTab(3,FALSE) ;

	// 100305 확장된 인벤토리 탭의 Disible 처리가 안되는 문제 수정
	switch(nExtendedInven)
	{
	case 1 : 
		{
			ShowTab(2,TRUE); 
			SetVisibleTabCount(3);
		}
		break ;
	case 2 :
		{
			ShowTab(2,TRUE) ;
			ShowTab(3,TRUE) ;
			SetVisibleTabCount(4);
		}
		break ;

	default : break ;
	}

	if( !m_bDisable )
	{
		if(val == FALSE)
		{
			CDealDialog * dDlg = GAMEIN->GetDealDialog();
			if(dDlg)
				if(dDlg->IsShow())
					dDlg->SetActive(FALSE);

			cDialog* dialog = WINDOWMGR->GetWindowForID( GDW_WAREHOUSEDLG );
			ASSERT( dialog );

			if( dialog->IsActive() )
			{
				dialog->SetActive( FALSE );
			}
		}

		cTabDialog::SetActive(val);
	}

	CheckBackground();

	// 인벤 창이 닫히면 아래 창들도 끈다.
	if( ! val )
	{
		cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
		cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
		cDialog* dissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );

		enchantDialog->SetActive( FALSE );
		reinforceDialog->SetActive( FALSE );
		dissoloveDialog->SetActive( FALSE );
	}

	// 071022 LYW --- InventoryExDialog : Check open dialog.
	TUTORIALMGR->Check_OpenDialog(this->GetID(), val) ;

	// 100616 인벤토리의 위치조정
	if( val )
		RevisionDialogPos();
}

void CInventoryExDialog::OnActionEvnet(LONG lId, void * p, DWORD we)
{
	if( !p ) return ;

	if(we == WE_LBTNDBLCLICK)
	{	
		CItem * pItem = NULL ;

		if(lId == IN_WEAREDDLG)
		{
			if( !m_pWearedDlg ) return ;

			WORD wIconIdx = 0 ;
			wIconIdx = (WORD)m_pWearedDlg->GetCurSelCellPos() ;

			pItem = (CItem*)m_pWearedDlg->GetIconForIdx( wIconIdx ) ;
		}
		else
		{
			cIconGridDialog * gridDlg = NULL ;
			gridDlg = (cIconGridDialog *)GetTabSheet(GetCurTabNum()) ;

			if( !gridDlg ) return ;

			pItem = (CItem*)gridDlg->GetIconForIdx((WORD)gridDlg->GetCurSelCellPos()) ;
		}

		if( !pItem ) return ;

		//if( pItem->GetItemParam() == eITEM_PARAM_SEAL )
		if( pItem->GetItemBaseInfo().nSealed == eITEM_TYPE_SEAL )
		{
			if( pItem->IsLocked() ) return ;

			WINDOWMGR->MsgBox(MBI_SHOPITEM_LOCK, MBT_YESNO, CHATMGR->GetChatMsg( 1178 )) ;

			// 090107 LYW --- InventoryExDialog : 봉인 해제 메시지가 떴을 경우에는, 
			// 메시지 뒤의 인벤토리 제어가 안되도록 수정한다.
			SetDisable( TRUE ) ;

			return ;
		}
		else
		{
			UseItem(pItem) ;
		}
	}
	else if(we == WE_RBTNCLICK)
	{
		// 100625 ONS 분해시 우클릭으로 아이템이동 가능하도록 수정.
		cIconGridDialog* const gridDlg = (cIconGridDialog *)GetTabSheet(GetCurTabNum());
		if( 0 == gridDlg )
		{
			return;
		}

		WORD wIconIdx		= (WORD)gridDlg->GetCurSelCellPos();
		CItem* const pItem	= (CItem*)gridDlg->GetIconForIdx(wIconIdx);
		if( 0 == pItem )
		{
			return;
		}

		// 상점
		CDealDialog* pDealDlg = GAMEIN->GetDealDialog() ;
		if( pDealDlg			&& 
			pDealDlg->IsActive() )
		{
			pDealDlg->SellItem(pItem) ;
			return;
		}
		
		// 분해
		CDissolveDialog* pDissolveDlg = GAMEIN->GetDissolveDialog();
		if( pDissolveDlg			&& 
			pDissolveDlg->IsActive() )
		{
			pDissolveDlg->MoveIcon(pItem);
		}
	}
}

CItem * CInventoryExDialog::GetCurSelectedItem(eITEMTABLE type)
{
	switch(type)
	{
	case eItemTable_Inventory:
		{
			// INVENTORY
			cIconGridDialog * gridDlg = (cIconGridDialog *)GetTabSheet(GetCurTabNum());
			if(gridDlg->GetCurSelCellPos() == -1) 
				return NULL;
			return (CItem *)gridDlg->GetIconForIdx((WORD)gridDlg->GetCurSelCellPos());
		}
		break;
	case eItemTable_Weared:
		{
			// WEARED
			if(m_pWearedDlg->GetCurSelCellPos() == -1) 
				return NULL;
			return (CItem *)m_pWearedDlg->GetIconForIdx((WORD)m_pWearedDlg->GetCurSelCellPos());
		}
		break;
	}
	return NULL;
}

BOOL CInventoryExDialog::GetBlankPositionRestrictRef(WORD & absPos)
{
	for(int i = 0 ; i < GetTabNum() ; ++i)
	{
		cIconGridDialog * gridDlg = (cIconGridDialog *)GetTabSheet(i);
		for(int j = 0 ; j < gridDlg->GetCellNum() ; ++j)
		{
			if(gridDlg->IsAddable(j))
			{
//				absPos = INVENTORY_STARTPOSITION+i*CELLNUMPERPAGE+j;
				absPos = TP_INVENTORY_START+i*TABCELL_INVENTORY_NUM+j;
				return TRUE;
			}
		}
		
	}
	return FALSE;
}

int CInventoryExDialog::GetBlankNum()
{
	int nBlankNum = 0;
	BYTE TabNum = GetTabNum();
	for( BYTE i = 0 ; i < TabNum ; ++i )
	{
		cIconGridDialog* gridDlg = (cIconGridDialog*)GetTabSheet(i);

		for( WORD j = 0 ; j < gridDlg->GetCellNum() ; ++j )
		{
			if( gridDlg->IsAddable(j) )
				++nBlankNum;
		}
	}

	return nBlankNum;
}

BOOL CInventoryExDialog::FakeMoveIcon(LONG x, LONG y, cIcon * icon)
{
	ASSERT(icon);
	if( m_bDisable )	return FALSE;

	if( PKMGR->IsPKLooted() )	return FALSE;	//죽었을때 다막을까.. //KES 040801

	if( icon->GetType() == WT_STALLITEM || icon->GetType() == WT_EXCHANGEITEM ) return FALSE;
	
	if(icon->GetType() == WT_ITEM)
	{
		return FakeMoveItem(x, y, (CItem *)icon);
	}
	else if(icon->GetType() == WT_DEALITEM)
	{
		FakeBuyItem(x, y, (CDealItem *)icon);
		AUDIOMGR->Play(
			58,
			gHeroID);
	}
	else if( icon->GetType() == WT_HOUSING_STORED_ICON )
	{
		if (FALSE == FakeMoveHousingStoredIcon(x, y,(cHousingStoredIcon*)icon) )
		{
			//090527 pdy 하우징 시스템메세지 [보관실패]
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1873 ) ); //1873	"해당 아이템은 이동 할 수 없습니다."
		}
	}
	return FALSE;
}

void CInventoryExDialog::FakeBuyItem( LONG mouseX, LONG mouseY, CDealItem * pFromItem )
{
//	if( pFromItem )
//		GAMEIN->GetDealDialog()->FakeBuyItem( pFromItem->GetAbsX(), pFromItem->GetAbsY(), pFromItem->GetItemIdx() );
	if( pFromItem )
		GAMEIN->GetDealDialog()->FakeBuyItem( (LONG)pFromItem->GetAbsX(), (LONG)pFromItem->GetAbsY(), pFromItem );
}

// 단축창 정보 이동
void CInventoryExDialog::MoveItem(POSTYPE FromPos, POSTYPE ToPos )
{
	CItem * FromItem = NULL;
	CItem * ToItem = NULL;

	DeleteItem(FromPos, &FromItem);
	DeleteItem(ToPos, &ToItem);

	if(FromItem)
	{
		FromItem->SetPosition(ToPos);
		if(!AddItem(FromItem))
			assert(0);
	}

	if(ToItem)
	{
		ToItem->SetPosition(FromPos);
		if(!AddItem(ToItem))
			assert(0);
	}
}

BOOL CInventoryExDialog::FakeMoveItem( LONG mouseX, LONG mouseY, CItem * pFromItem )
{
	WORD ToPos=0;
	
	if( !GetPositionForXYRef( 0, mouseX, mouseY, ToPos ) )	// 절대위치 넘어 옴
		return FALSE;

	CItem * pToItem = GetItemForPos( ToPos );
	
	//sound
	if(ITEMMGR->GetTableIdxForAbsPos(ToPos)==eItemTable_Weared)
	{
		AUDIOMGR->Play(
			59,
			gHeroID);
	}
	else
	{
		AUDIOMGR->Play(
			58,
			gHeroID);
	}

//KES EXCHANGE 031002
	if( pToItem )
	if( pToItem->IsLocked() ) return FALSE;
	
	if( pFromItem->IsLocked() ) return FALSE;

	if( pFromItem->GetParent() == ( cObject* )GAMEIN->GetPetWearedDlg() )
	{
		if( pToItem )
		{
			RECT rt;
			GAMEIN->GetPetWearedDlg()->GetIconCellRect( pFromItem->GetPosition(), rt );
			GAMEIN->GetPetWearedDlg()->FakeMoveIcon( rt.left, rt.top, pToItem );
		}
		else
		{
			MSG_PET_ITEM_MOVE_SYN msg;
			msg.Category = MP_PET;
			msg.Protocol = MP_PET_ITEM_MOVE_PLAYER_SYN;
			msg.dwObjectID = HEROID;
			msg.PlayerPos = ToPos;
			msg.PlayerItemIdx = 0;
			msg.PetPos = pFromItem->GetPosition();
			msg.PetItemIdx = pFromItem->GetItemIdx();

			NETWORK->Send( &msg, sizeof( msg ) );
		}
	
		return FALSE;
	}
	
	//---KES Item Combine Fix 071021
	if(TP_STORAGE_START <= pFromItem->GetPosition() && pFromItem->GetPosition() < TP_STORAGE_END)
	{
		if( ITEMMGR->IsDupItem( pFromItem->GetItemIdx() ) )
		{
			if( pToItem )
			if( pToItem->GetItemIdx() == pFromItem->GetItemIdx() )
			{
				return FakeItemCombine( ToPos, pFromItem, pToItem );
			}
		}

		if( ToPos == ( TP_WEAR_START + eWearedItem_Shield) )
		{
			switch( HERO->GetWeaponAniType() )
			{
			case eWeaponAnimationType_TwoHand:
			case eWeaponAnimationType_Staff:
			case eWeaponAnimationType_Bow:
			case eWeaponAnimationType_Gun:
			case eWeaponAnimationType_TwoBlade:
			// 090521 ONS 신규종족 무기 타입 속성 추가
			case eWeaponAnimationType_BothDagger:
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(772) );
					return FALSE;
				}
				break;
			}
		}

		return FakeGeneralItemMove( ToPos, pFromItem, pToItem );
	}
	else if(TP_SHOPITEM_START <= pFromItem->GetPosition() && pFromItem->GetPosition() < TP_SHOPITEM_END)
	{
		return FakeGeneralItemMove( ToPos, pFromItem, pToItem );
	}

	if( ITEMMGR->IsDupItem( pFromItem->GetItemIdx() ) )
	{
		if( IsDivideFunctionKey() && !( pToItem ) )
		{
			return FakeItemDivide( ToPos, pFromItem, pToItem );
		}
		else if( ( pToItem && pFromItem->GetItemIdx() == pToItem->GetItemIdx() ) )
		{
			return FakeItemCombine( ToPos, pFromItem, pToItem );
		}
	}

	if( ToPos == ( TP_WEAR_START + eWearedItem_Shield) )
	{
		switch( HERO->GetWeaponAniType() )
		{
		case eWeaponAnimationType_TwoHand:
		case eWeaponAnimationType_Staff:
		case eWeaponAnimationType_Bow:
		case eWeaponAnimationType_Gun:
		// 090521 ONS 신규종족 무기 타입 속성 추가
		case eWeaponAnimationType_BothDagger:
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(772) );
				return FALSE;
			}
			break;
		}
	}
		
	return FakeGeneralItemMove( ToPos, pFromItem, pToItem );
}

BOOL CInventoryExDialog::IsDivideFunctionKey()
{
	return KEYBOARD->GetKeyPressed(KEY_MENU);
}

BOOL CInventoryExDialog::FakeGeneralItemMove( POSTYPE ToPos, CItem * pFromItem, CItem * pToItem )
{
	if( CanBeMoved( pFromItem, ToPos ) == FALSE )
		return FALSE;

	BYTE FromIdx = ITEMMGR->GetTableIdxForAbsPos( pFromItem->GetPosition() );
	if(FromIdx >= eItemTable_Max) return FALSE;
	if( pToItem && CanBeMoved( pToItem, pFromItem->GetPosition() ) == FALSE )
	{
		return FALSE;
	}
	if(pToItem && (FromIdx == eItemTable_MunpaWarehouse))
	{
		if(FromIdx == eItemTable_MunpaWarehouse)
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(365) );
		return FALSE;
	}

	BYTE ToIdx = ITEMMGR->GetTableIdxForAbsPos( ToPos );
	if(ToIdx >= eItemTable_Max) return FALSE;


	MSG_ITEM_MOVE_SYN msg;
	msg.Category			= MP_ITEM;
	msg.Protocol			= MP_ITEM_MOVE_SYN;
	msg.dwObjectID			= HEROID;

	msg.FromPos				= pFromItem->GetPosition();
	msg.wFromItemIdx		= pFromItem->GetItemIdx();
	msg.ToPos				= ToPos;
	msg.wToItemIdx			= (pToItem?pToItem->GetItemIdx():0);

 	if( msg.FromPos == msg.ToPos )
		return FALSE;

	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Inventory);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Storage);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_MunpaWarehouse);

	NETWORK->Send( &msg, sizeof(msg) );
	// 080715 LUJ, 아이템 이동 체크가 서버에서 완료된 후 이동풩돈?FALSE를 반환해야한다
	return FALSE;
}

BOOL CInventoryExDialog::FakeItemDivide( POSTYPE ToPos, CItem * pFromItem, CItem * pToItem )
{
	if( !CanBeMoved( pFromItem, ToPos ) )
		return FALSE;
	if( pFromItem->GetPosition() == ToPos )
		return FALSE;
	if( pFromItem->IsLocked() )
		return FALSE;

	BYTE FromIdx = ITEMMGR->GetTableIdxForAbsPos( pFromItem->GetPosition() );
	if(FromIdx >= eItemTable_Max) return FALSE;

	BYTE ToIdx = ITEMMGR->GetTableIdxForAbsPos( ToPos );
	if(ToIdx >= eItemTable_Max) return FALSE;
	
	if(pFromItem->GetDurability() == 1)
	{
		return FakeGeneralItemMove(ToPos, pFromItem, pToItem);
	}

	m_divideMsg.Category			= MP_ITEM;
	m_divideMsg.Protocol			= MP_ITEM_DIVIDE_SYN;
	m_divideMsg.dwObjectID			= HEROID;

	m_divideMsg.wItemIdx = pFromItem->GetItemIdx();
	m_divideMsg.FromPos = pFromItem->GetPosition();
	m_divideMsg.FromDur = pFromItem->GetDurability();
	m_divideMsg.ToPos = ToPos;
	m_divideMsg.ToDur = 0;

	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Inventory);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Storage);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_MunpaWarehouse);

	cDivideBox * pDivideBox = WINDOWMGR->DivideBox( DBOX_DIVIDE_INV, (LONG)pFromItem->GetAbsX(), (LONG)pFromItem->GetAbsY(), OnFakeItemDivideOk, OnFakeItemDivideCancel, this, pFromItem, CHATMGR->GetChatMsg(185) );
	
	// 091215 ONS 아이템Stack최대수를 아이템별로 지정된 값으로 처리한다.
	const WORD wItemStackNum = ITEMMGR->GetItemStackNum( pFromItem->GetItemIdx() );	
	pDivideBox->SetMaxValue( wItemStackNum );
	pDivideBox->SetValue( 0 );
	//pDivideBox->SetValue( m_divideMsg.FromDur );
	
	return FALSE;
		
}

void CInventoryExDialog::OnFakeItemDivideCancel( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 )
{
	ITEMMGR->SetDisableDialog(FALSE, eItemTable_Inventory);
	ITEMMGR->SetDisableDialog(FALSE, eItemTable_Storage);
	ITEMMGR->SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
}

void CInventoryExDialog::OnFakeItemDivideOk( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 )
{
	CInventoryExDialog * tDlg = ( CInventoryExDialog * )vData1;
	if( param1 == 0 )
	{
		OnFakeItemDivideCancel(0,NULL,0,NULL,NULL);
		return;
	}

	const eITEMTABLE TargetTableIdx = ITEMMGR->GetTableIdxForAbsPos(tDlg->m_divideMsg.FromPos);
	CItem* pTargetItem = ITEMMGR->GetItemofTable(TargetTableIdx, tDlg->m_divideMsg.FromPos);

	if(!pTargetItem)
	{
		OnFakeItemDivideCancel(0,NULL,0,NULL,NULL);
		return ;
	}
	if(pTargetItem->IsLocked())
	{
		OnFakeItemDivideCancel(0,NULL,0,NULL,NULL);
		return ;
	}

	if(tDlg->m_divideMsg.wItemIdx != pTargetItem->GetItemIdx())
	{
		OnFakeItemDivideCancel(0,NULL,0,NULL,NULL);
		return ;
	}
	if(tDlg->m_divideMsg.FromDur != pTargetItem->GetDurability())
	{
		OnFakeItemDivideCancel(0,NULL,0,NULL,NULL);
		return ;
	}
	CItem * pToItem = ITEMMGR->GetItemofTable(ITEMMGR->GetTableIdxForAbsPos(tDlg->m_divideMsg.ToPos), tDlg->m_divideMsg.ToPos);
	if(pToItem)
	{
		OnFakeItemDivideCancel(0,NULL,0,NULL,NULL);
		return ;
	}

	if( tDlg->m_divideMsg.FromDur > param1 )
	{
		tDlg->m_divideMsg.ToDur			= param1;
		tDlg->m_divideMsg.FromDur		= tDlg->m_divideMsg.FromDur - param1;

		//---KES ItemDivide Fix 071020
		ITEMMGR->SetDivideItemInfo( tDlg->m_divideMsg.FromPos, tDlg->m_divideMsg.ToDur );
		//----------------------------

		NETWORK->Send( &tDlg->m_divideMsg, sizeof(tDlg->m_divideMsg) );
	}
	else
	{
		//아이템 이동
		tDlg->FakeGeneralItemMove(tDlg->m_divideMsg.ToPos, (CItem *)vData2, NULL);
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_Inventory);
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_Storage);
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_MunpaWarehouse);
	}
}

BOOL CInventoryExDialog::FakeItemCombine( POSTYPE ToPos, CItem * pFromItem, CItem * pToItem )
{
	if( pFromItem->GetPosition() == pToItem->GetPosition() )
		return FALSE;
	if( pFromItem->GetItemIdx() != pToItem->GetItemIdx() )
		return FALSE;

	if( CanBeMoved( pFromItem, ToPos ) == FALSE )
		return FALSE;

	if( pToItem && CanBeMoved( pToItem, pFromItem->GetPosition() ) == FALSE )
		return FALSE;

	if( pFromItem->IsLocked() || pToItem->IsLocked() )
		return FALSE;

	BYTE FromIdx = ITEMMGR->GetTableIdxForAbsPos( pFromItem->GetPosition() );
	if(FromIdx >= eItemTable_Max) return FALSE;

	BYTE ToIdx = ITEMMGR->GetTableIdxForAbsPos( ToPos );
	if(ToIdx >= eItemTable_Max) return FALSE;

	MSG_ITEM_COMBINE_SYN msg;
	msg.Category			= MP_ITEM;
	msg.Protocol			= MP_ITEM_COMBINE_SYN;
	msg.dwObjectID			= HEROID;

	msg.FromPos				= pFromItem->GetPosition();
	msg.ToPos				= pToItem->GetPosition();
	msg.wItemIdx			= pFromItem->GetItemIdx();
	msg.FromDur				= pFromItem->GetDurability();
	msg.ToDur				= pToItem->GetDurability();

	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Inventory);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Storage);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_MunpaWarehouse);

	NETWORK->Send( &msg, sizeof(msg) );

	// 080715 LUJ, 아이템 이동 체크가 서버에서 완료된 후 이동되도록 FALSE를 반환해야한다
	return FALSE;
}

//090423 하우징창고 보관아이콘->인벤토리 회수
BOOL	CInventoryExDialog::FakeMoveHousingStoredIcon(LONG mouseX, LONG mouseY, cHousingStoredIcon* pFromStoredIcon)
{
	if( pFromStoredIcon->IsLocked() ) return FALSE;

	WORD ToPos=0;
	
	if( !GetPositionForXYRef( 0, mouseX, mouseY, ToPos ) )	// 절대위치 넘어 옴
		return FALSE;

	//sound
	if(ITEMMGR->GetTableIdxForAbsPos(ToPos)!=eItemTable_Inventory)
		return FALSE;
	else
	{
		AUDIOMGR->Play(
			58,
			gHeroID);
	}

	CItem * pToItem = GetItemForPos( ToPos );

	if( pToItem )		//ToPos에 아이템이 있으면 실패?
		return FALSE;

	//아이템이 없으면 들어감 

	stFurniture* pstFurniture = pFromStoredIcon->GetLinkFurniture();
	if ( pstFurniture->wState != eHOUSEFURNITURE_STATE_KEEP )
		return FALSE;


	if(! GetBlankPositionRestrictRef( ToPos ) )
		return FALSE;

	return HOUSINGMGR->RequestRestoredICon(ToPos,pFromStoredIcon);
}

void CInventoryExDialog::Linking()
{
	m_pWearedDlg = (CWearedExDialog *)GetWindowForID(IN_WEAREDDLG);
	m_pMoneyEdit = (cStatic *)GetWindowForID(IN_MONEYEDIT);

	for(int count = 0 ; count < MAX_BACKGROUND ; ++count)
	{
		m_pBackground[count] = GetWindowForID(IN_BACKGROUND1 + count);
		m_pBackground[count]->SetActive( FALSE ) ;
	}
}

// 090824 ShinJS --- Seal Option 을 지정하여 해당 Opt을 제외한 Item을 찾을수 있도록 수정
// 091106 LUJ, 개수를 만족하는 아이템을 반환할 수 있도록 수정
CItem * CInventoryExDialog::GetItemForIdx(DWORD wItemIdx, WORD wExceptSealOpt, DURTYPE quantity, BOOL bExceptItemOpt)
{
	const ITEM_INFO* const pItemInfo = ITEMMGR->GetItemInfo(wItemIdx);

	if(0 == pItemInfo)
	{
		return 0;
	}

	// 091106 LUJ, 겹쳐지는 아이템만 개수가 유효하다
	quantity = (pItemInfo->Stack ? quantity : 0);

	for(BYTE i = 0; i < GetTabNum(); ++i)
	{
		cIconGridDialog* gridDlg = (cIconGridDialog*)GetTabSheet(i);

		for( WORD j = 0 ; j < gridDlg->GetCellNum() ; ++j )
		{
			if(!gridDlg->IsAddable(j) )
			{
				CItem* const pItem = (CItem *)gridDlg->GetIconForIdx(TP_INVENTORY_START + j);

				if(pItem->GetItemIdx() != wItemIdx)
				{
					continue;
				}
				else if(pItem->GetItemBaseInfo().nSealed == wExceptSealOpt)
				{
					continue;
				}
				else if(0 < quantity)
				{
					if(pItem->GetDurability() < quantity)
					{
						continue;
					}
				}
				else if( bExceptItemOpt )
				{
					// 091127 ONS 강화, 조합, 인챈트 아이템은 제외
					const ITEM_OPTION& option = ITEMMGR->GetOption( pItem->GetDBIdx() );
					if( option.mItemDbIndex )
					{
						continue;
					}
				}

				return pItem;
			}
		}
	}
	return NULL;
}

DURTYPE	CInventoryExDialog::GetTotalItemDurability(DWORD wItemIdx)
{
	DURTYPE DurSum = 0;
	BYTE TabNum = GetTabNum();
	for( BYTE i = 0 ; i < TabNum ; ++i )
	{
		cIconGridDialog* gridDlg = (cIconGridDialog*)GetTabSheet(i);

		for( WORD j = 0 ; j < gridDlg->GetCellNum() ; ++j )
		{
			if(!gridDlg->IsAddable(j) )
			{
				CItem * pItem = (CItem *)gridDlg->GetIconForIdx(TP_INVENTORY_START+j/*+TABCELL_INVENTORY_NUM*i*/);

				// 090824 ShinJS --- 쌓이는 아이템 판단하여 개수 파악할 수 있도록 수정
				if(pItem->GetItemIdx() == wItemIdx)
				{
					DURTYPE dur = ITEMMGR->IsDupItem( pItem->GetItemIdx() ) ? pItem->GetDurability() : 1;
					DurSum += dur;
				}
			}
		}
	}

	return DurSum;
}
// 0(all),1(inv),2
BOOL CInventoryExDialog::GetPositionForXYRef(BYTE param, LONG x, LONG y, POSTYPE& pos)
{
	WORD position=0;										// 상대 위치

	if(param != 2)
	{
		cIconGridDialog * gridDlg = (cIconGridDialog *)GetTabSheet(GetCurTabNum());
		if(gridDlg->GetPositionForXYRef(x, y, position))
		{
			pos = position+TP_INVENTORY_START+TABCELL_INVENTORY_NUM*GetCurTabNum();
			return TRUE;
		}
	}

	if(param != 1)
	if(m_pWearedDlg->GetPositionForXYRef(x, y, position))
	{
//		pos = position+WEAR_STARTPOSITION;					// -> 절대 위치로 바꿈
		pos = position+TP_WEAR_START;					// -> 절대 위치로 바꿈
		return TRUE;
	}
	
	return FALSE;
}

void CInventoryExDialog::SetMoney(DWORD Money)
{
	m_pMoneyEdit->SetStaticText(AddComma(Money));
}

BOOL CInventoryExDialog::IsExist(POSTYPE abs_pos)
{
	BOOL bExist = FALSE;
	if(TP_INVENTORY_START <= abs_pos && abs_pos < (TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() )))
	{
		int tabIndex = GetTabIndex( abs_pos );		
		cIconGridDialog * dlg = (cIconGridDialog *)GetTabSheet(tabIndex);
		if(dlg->GetIconForIdx(abs_pos-TABCELL_INVENTORY_NUM*tabIndex-TP_INVENTORY_START))
			bExist = TRUE;
	}
	else if(TP_WEAR_START <= abs_pos && abs_pos < TP_WEAR_END)
	{
		if( m_pWearedDlg->GetIconForIdx(abs_pos - TP_WEAR_START) )
			bExist = TRUE;
	}

	return bExist;
}

// 080703 LUJ, 이도류를 적용함. 중복 코드를 제거하고 정리함.
BOOL CInventoryExDialog::CanBeMoved( CItem* pItem, POSTYPE toPos )
{
	if( ! pItem )
	{
		return FALSE;
	}

	const POSTYPE		fromPos				= pItem->GetPosition();
	const ITEM_INFO*	movingItemInfo		= pItem->GetItemInfo();

	if( ! movingItemInfo )
	{
		return FALSE;
	}	
	else if( TP_WEAR_START <= toPos && toPos < TP_WEAR_END )
	{
		// 080410 LUJ, 창고 -> 장비 이동을 막는다
		switch( ITEMMGR->GetTableIdxForAbsPos( fromPos ) )
		{
		case eItemTable_Storage:
		case eItemTable_Shop:
		case eItemTable_MunpaWarehouse:
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 76 ) );
				return FALSE;
			}
		}

		if( pItem->GetItemBaseInfo().nSealed == eITEM_TYPE_SEAL ) 
		{
			WINDOWMGR->MsgBox( MBI_SEALITEM_CANTEQUIP, MBT_OK, CHATMGR->GetChatMsg( 1177 ) );
			return FALSE ;
		}
		// 080703 LUJ, 무기 슬롯에 아이템이 있는 경우 종류가 일치하는지 검사한다
		else if( TP_WEAR_START == toPos )
		{
			// 080703 LUJ, 무기만 장착 가능
			if( ePartType_OneHand	!= movingItemInfo->Part3DType	&&
				ePartType_TwoHand	!= movingItemInfo->Part3DType   &&
				ePartType_TwoBlade	!= movingItemInfo->Part3DType   )
			{
				return FALSE;
			}

			CItem*				shieldItem		= GetItemForPos( TP_WEAR_START + eWearedItem_Shield );
			const ITEM_INFO*	shieldItemInfo	= ( shieldItem ? shieldItem->GetItemInfo() : 0 );

			// 080703 LUJ, 방패 슬롯에 아이템이 있음
			if( shieldItemInfo )
			{
				// 080703 LUJ, 양손 무기인 경우 당연 실패
				if( ePartType_TwoHand == movingItemInfo->Part3DType ||
					ePartType_TwoBlade == movingItemInfo->Part3DType )
				{
					// 090729 ShinJS --- 착용 실패시 에러메세지 출력
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(772) );
					return FALSE;
				}
				// 080703 LUJ, 방패가 아니고 무기가 있을 경우 종류가 일치해야 장착 가능
				else if(	shieldItemInfo->Part3DType	!= ePartType_Shield		&&
							shieldItemInfo->EquipType	!= movingItemInfo->EquipType )
				{
					return FALSE;
				}
			}
		}
		// 080703 LUJ, 방패 슬롯에 무기를 장착할 수 있도록 체크한다
		else if( TP_WEAR_START + eWearedItem_Shield == toPos )
		{
			CItem*				weaponItem			= GetItemForPos( TP_WEAR_START + eWearedItem_Weapon );
			const ITEM_INFO*	weaponItemInfo		= ( weaponItem ? weaponItem->GetItemInfo() : 0 );
			const BOOL			isEnableTwoBlade	= HERO->GetHeroTotalInfo()->bUsingTwoBlade;

			// 080703 LUJ, 무기 슬롯에 아이템이 있는 경우, 같은 종류만 장착할 수 있다
			if( weaponItemInfo )
			{
				// 080703 LUJ, 양손 무기는 장착 불가
				if( ePartType_TwoHand == weaponItemInfo->Part3DType ||
					ePartType_TwoHand == movingItemInfo->Part3DType )
				{
					return FALSE;
				}				
				// 080703 LUJ, 이도류 마스터가 없는데 무기를 착용하려 한 경우
				else if( ePartType_Shield != movingItemInfo->Part3DType )
				{
					if( ! isEnableTwoBlade )
					{
						// 080714 LUJ, 리소스가 잘못 업데이트되어 메시지 번호를 변경
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1591 ) );
						return FALSE;
					}
					else if( movingItemInfo->WeaponType	!= weaponItemInfo->WeaponType )
					{
						// 080714 LUJ, 리소스가 잘못 업데이트되어 메시지 번호를 변경
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1592 ) );
						return FALSE;
					}
					else if( movingItemInfo->WeaponType == eWeaponType_Staff )
					{
						// 080714 LUJ, 리소스가 잘못 업데이트되어 메시지 번호를 변경
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1593 ) );
						return FALSE;
					}
					// 090910 pdy 방패슬롯 무기장착시 채크 추가 
					// 무기슬롯->방패슬롯 이동의경우 방패슬롯의 무기가 같은무기가 아니면 불가
					else if( TP_WEAR_START + eWearedItem_Weapon == fromPos)
					{
						CItem*				ShieldItem			= GetItemForPos( TP_WEAR_START + eWearedItem_Shield );
						const ITEM_INFO*	ShieldItemInfo		= ( ShieldItem ? ShieldItem->GetItemInfo() : 0 );

						if( ! ShieldItemInfo )
						{
							CHATMGR->AddMsg(
							CTC_SYSMSG,
							CHATMGR->GetChatMsg( 1594 ) );

							return FALSE;
						}
						else if( movingItemInfo->WeaponType !=  ShieldItemInfo->WeaponType )
						{
							return FALSE;
						}
					}
				}
			}
			// 080703 LUJ, 무기 슬롯이 빈 경우 방패만 장착할 수 있다
			else if( ePartType_Shield != movingItemInfo->Part3DType )
			{
				CHATMGR->AddMsg(
					CTC_SYSMSG,
					// 080714 LUJ, 리소스가 잘못 업데이트되어 메시지 번호를 변경
					isEnableTwoBlade ? CHATMGR->GetChatMsg( 1594 ) : CHATMGR->GetChatMsg( 1591 ) );
				
				return FALSE;
			}
		}
		else if( movingItemInfo->EquipSlot == eWearedItem_Earring1 )
		{	
			if(toPos-TP_WEAR_START != eWearedItem_Earring1 && toPos-TP_WEAR_START != eWearedItem_Earring2)
				return FALSE;
		}
		else if( movingItemInfo->EquipSlot == eWearedItem_Ring1 )
		{	
			if(toPos-TP_WEAR_START != eWearedItem_Ring1 && toPos-TP_WEAR_START != eWearedItem_Ring2)
				return FALSE;
		}		
		else if( movingItemInfo->EquipSlot == eWearedItem_Earring1 )
		{	
			if( toPos - TP_WEAR_START != eWearedItem_Earring1 && toPos - TP_WEAR_START != eWearedItem_Earring2)
			{
				return FALSE;
			}
		}
		else if( movingItemInfo->EquipSlot == eWearedItem_Ring1 )
		{	
			if( toPos - TP_WEAR_START != eWearedItem_Ring1 && toPos - TP_WEAR_START != eWearedItem_Ring2)
			{
				return FALSE;
			}
		}
		else if( movingItemInfo->EquipSlot + TP_WEAR_START != toPos )
		{
			return FALSE;
		}

		// 090114 LYW --- InvantoryExDialog : 아이템 착용 가능 체크 함수 수정에 따른 처리.
		const BYTE byEnableEquip = ITEMMGR->CanEquip(pItem->GetItemIdx() ) ;
		if( byEnableEquip != eEquip_Able )
		{
			switch( byEnableEquip )
			{
				case eEquip_RaceFail :			
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(823), movingItemInfo->ItemName) ;
					}
					break ;

				case eEquip_GenderFail :		
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(824), movingItemInfo->ItemName) ;
					}
					break ;

				case eEquip_LevelFail :			
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(825), movingItemInfo->ItemName) ;	
					}
					break ;

				case eEquip_FishingLevelFail :	
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1530)) ;
					}
					break ;
				case eEquip_CookingLevelFail :
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1661 ) );
					}
					break;

				case eEquip_Disable : 
				default :				
					{
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(73)) ;								
					}
					break ;
			}

			return FALSE;
		}
	}
	// 080703 LUJ, 무기 슬롯에서 아이템을 빼낼 때, 이도류 상태인 경우 뺄 수 없다
	//				방패 슬롯에만 무기가 있을 경우 애니메이션이 매우 어색하다
	else if( fromPos == TP_WEAR_START + eWearedItem_Weapon )
	{
		CItem* item = GetItemForPos( TP_WEAR_START + eWearedItem_Shield );

		if( item &&
			item->GetItemInfo()->WeaponType == movingItemInfo->WeaponType )
		{
			if(movingItemInfo->WeaponType == eWeaponType_CookUtil)
			{
				// NYJ - 요리도구는 별도의 메세지를 출력한다.
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1811 ) );
				return FALSE;
			}
			else
			{
				// 080714 LUJ, 리소스가 잘못 업데이트되어 메시지 번호를 변경
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1595 ) );
				return FALSE;
			}
		}
	}
	// 071210 LYW --- InventoryExDialog : 인벤토리 확장에 따른 처리를 한다.
	else if( TP_INVENTORY_START<= toPos && toPos < ( TP_INVENTORY_END + ( TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount() ) ) )
	{
		return TRUE;
	}	
	else if( TP_STORAGE_START <= toPos && toPos < TP_STORAGE_END )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void CInventoryExDialog::ItemUseForQuestStart()
{
	if( m_pTempQuestItem )
	{
		// quest 아이템의 사용
		MSG_ITEM_USE_SYN msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_USE_FOR_QUESTSTART_SYN;
		msg.dwObjectID = HEROID;
		msg.TargetPos = m_pTempQuestItem->GetPosition();
		msg.wItemIdx = m_pTempQuestItem->GetItemIdx();

		NETWORK->Send(&msg,sizeof(msg));
	}
}





//---------------------------------------------------------------------------------------------------------------------------
//	071114 LYW --- IventoryExDialog : 아이템 봉인 해제 요청.
//---------------------------------------------------------------------------------------------------------------------------
void CInventoryExDialog::ItemUnSealingSync()
{
	CItem* pItem = NULL ;
	pItem = GetCurSelectedItem(eItemTable_Inventory) ;

	if( !pItem ) return ;

	const ITEMBASE& pItemBase = pItem->GetItemBaseInfo() ;

	if( pItemBase.nSealed != eITEM_TYPE_SEAL ) return ;	
	
	if( pItem->IsLocked() ) return ;

	MSG_ITEM_USE_SYN msg;
	msg.Category = MP_ITEM ;
	msg.Protocol = MP_ITEM_USE_SYN ;
	msg.dwObjectID = HEROID ;
	msg.wItemIdx = pItem->GetItemIdx() ;
	msg.TargetPos = pItem->GetPosition() ;

	NETWORK->Send(&msg,sizeof(msg)) ;
}

void CInventoryExDialog::ItemUnSealing(POSTYPE absPos)
{
	CItem* const pItem = ITEMMGR->GetItemofTable(
		eItemTable_Inventory,
		absPos);

	if( !pItem ) return ;

	pItem->SetItemParam( eITEM_PARAM_UNSEAL ) ;

	// 툴팁을 다시 정해준다.
	ITEM_INFO* pItemInfo = NULL ;
	pItemInfo = ITEMMGR->GetItemInfo( pItem->GetItemIdx() ) ;

	if( !pItemInfo ) return ;

	ITEMMGR->AddToolTip( pItem ) ;

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(366) ) ;
}

void CInventoryExDialog::CheckBackground() 
{
	CHero* pHero = HERO ;

	if(pHero == NULL ) return ;

	CHARACTER_TOTALINFO* pTotalInfo = pHero->GetCharacterTotalInfo() ;
	
	if(pTotalInfo)
	{
		BYTE byRacial = pTotalInfo->Race ;
		BYTE byGender = pTotalInfo->Gender ;

		for( BYTE count = 0 ; count < MAX_BACKGROUND ; ++count )
		{
			m_pBackground[count]->SetActive( FALSE ) ;
		}

		// 090521 ONS 신규종족 추가로 인한 background처리변경
		if( byRacial == RaceType_Human )
		{
			if( !byGender )
			{
				m_pBackground[0]->SetActive( TRUE ) ;
			}
			else
			{
				m_pBackground[1]->SetActive( TRUE ) ;
			}
		}
		else if( byRacial == RaceType_Elf )
		{
			if( !byGender )
			{
				m_pBackground[2]->SetActive( TRUE ) ;
			}
			else
			{
				m_pBackground[3]->SetActive( TRUE ) ;
			}
		}
		else
		{
			if( !byGender )
			{
				m_pBackground[4]->SetActive( TRUE ) ;
			}
			else
			{
				m_pBackground[5]->SetActive( TRUE ) ;
			}
		}
	}
}


void CInventoryExDialog::SetCoolTime( DWORD itemIndex, DWORD miliSecond )
{
	const DWORD groupIndex = ITEMMGR->GetCoolTimeGroupIndex( itemIndex );

	for( BYTE tabIndex = 0; tabIndex < GetTabNum(); ++tabIndex )
	{
		cIconGridDialog* tab = ( cIconGridDialog* )GetTabSheet( tabIndex );
		
		if( ! tab )
		{
			continue;
		}

		for( WORD iconIndex = 0; iconIndex < tab->GetCellNum(); ++iconIndex )
		{
			cIcon* icon = tab->GetIconForIdx( iconIndex );

			if(		icon &&
				(	icon->GetData() == itemIndex ||
					groupIndex && ITEMMGR->GetCoolTimeGroupIndex( icon->GetData() ) == groupIndex ) )
			{
				icon->SetCoolTime( miliSecond );
			}
		}
	}
}

void CInventoryExDialog::SetCoolTimeAbs( DWORD itemIndex, DWORD dwBegin, DWORD dwEnd, DWORD miliSecond )
{
	const DWORD groupIndex = ITEMMGR->GetCoolTimeGroupIndex( itemIndex );

	for( BYTE tabIndex = 0; tabIndex < GetTabNum(); ++tabIndex )
	{
		cIconGridDialog* tab = ( cIconGridDialog* )GetTabSheet( tabIndex );
		
		if( ! tab )
		{
			continue;
		}

		for( WORD iconIndex = 0; iconIndex < tab->GetCellNum(); ++iconIndex )
		{
			cIcon* icon = tab->GetIconForIdx( iconIndex );

			if(		icon &&
				(	icon->GetData() == itemIndex ||
					groupIndex && ITEMMGR->GetCoolTimeGroupIndex( icon->GetData() ) == groupIndex ) )
			{
				icon->SetCoolTimeAbs( dwBegin, dwEnd, miliSecond );
			}
		}
	}
}


void CInventoryExDialog::Send()
{
	MSG_ITEM_USE_SYN message( mMessage );

	NETWORK->Send( &message, sizeof( message ) );
}


void CInventoryExDialog::Restore()
{
	SetDisable( FALSE );

	CItem* item = GetItemForPos( mMessage.TargetPos );

	if( item )
	{
		item->SetCoolTime( 0 );

		cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
		ASSERT( dialog );

		dialog->SetCoolTime( item->GetItemIdx(), 0 );


		// 080706 LYW --- InventoryExDialog : 확장 퀵 슬롯 추가 처리.
		cQuickSlotDlg* dialog2 = NULL ;
		dialog2 = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI2_QUICKSLOTDLG ) ;
		ASSERT(dialog2) ;

		if(dialog2)
		{
			dialog2->SetCoolTime( item->GetItemIdx(), 0 ) ;
		}
	}
}






// 071210 LYW --- CInventoryExDialog : 확장 인벤토리의 활성화 여부를 세팅하는 함수 추가.
void CInventoryExDialog::InitExtended_Inventory( CHARACTER_TOTALINFO* pInfo )
{
	if( !pInfo ) return ;

	if( pInfo->wInventoryExpansion == 0 ) return ;

	if( pInfo->wInventoryExpansion == 1 )
	{
		ShowTab(2,TRUE) ;

		HERO->Set_HeroExtendedInvenCount(1) ;
	}
	else if( pInfo->wInventoryExpansion == 2 )
	{
		ShowTab(2,TRUE) ;
		ShowTab(3,TRUE) ;

		HERO->Set_HeroExtendedInvenCount(2) ;
	}
}
// 080213 KTH -- ClearInventory
BOOL CInventoryExDialog::ClearInventory()
{
	for( int i = TP_INVENTORY_START; i < (TP_INVENTORY_END + (TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount())); i++ )
	{
			ITEMMGR->DeleteItemofTable( eItemTable_Inventory, i, false );
	}

	return TRUE;
}

void CInventoryExDialog::SetMultipleItemLock(DWORD dwItemIdx, BOOL bVal)
{
	for( BYTE tabIndex = 0; tabIndex < GetTabNum(); ++tabIndex )
	{
		cIconGridDialog* tab = ( cIconGridDialog* )GetTabSheet( tabIndex );
		
		if( ! tab )
		{
			continue;
		}

		for( WORD iconIndex = 0; iconIndex < tab->GetCellNum(); ++iconIndex )
		{
			cIcon* icon = tab->GetIconForIdx( iconIndex );

			if(icon && icon->GetData()==dwItemIdx)
			{
				icon->SetLock(bVal);
			}
		}
	}
}

void CInventoryExDialog::PetGradeUp()
{
	SetCoolTime( mMessage.wItemIdx, WAITMILISECOND );

	{
		cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
		ASSERT( dialog );

		dialog->SetCoolTime( mMessage.wItemIdx, WAITMILISECOND );
	}

	{
		CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
		ASSERT( dialog );

		dialog->SetText( RESRCMGR->GetMsg( 1008 ) );
		dialog->Wait( CProgressDialog::eActionPetGradeUp );
	}
}

void CInventoryExDialog::StartVehicleSummon( const VECTOR3& vecPos )
{
	CItem* pItem = Get_QuickSelectedItem();
	if( !pItem )		return;

	// 쿨타임 적용
	SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );

	cQuickSlotDlg* pQuickSlotDlg = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
	ASSERT( pQuickSlotDlg );
	if( pQuickSlotDlg )
		pQuickSlotDlg->SetCoolTime( pItem->GetItemIdx(), WAITMILISECOND );

	// Progress 실행
	CProgressDialog* pProgressDlg = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
	ASSERT( pProgressDlg );
	if( pProgressDlg )
	{
		pProgressDlg->SetVehicleSummonPos( vecPos );						// 설치 위치저장
		pProgressDlg->SetText(  RESRCMGR->GetMsg( 1006 ) );					// InterfaceMsg : 소환합니다
		pProgressDlg->Wait( CProgressDialog::eActionVehicleSummon );		// 탈것소환Progress 실행		
	}
}

void CInventoryExDialog::StartVehicleSeal( DWORD dwVehicleID )
{
	// Progress 실행
	CProgressDialog* pProgressDlg = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
	ASSERT( pProgressDlg );
	if( pProgressDlg )
	{
		pProgressDlg->SetText(  RESRCMGR->GetMsg( 1007 ) );					// InterfaceMsg : 봉인합니다
		pProgressDlg->Wait( CProgressDialog::eActionVehicleSeal );			// 탈것봉인Progress 실행
	}
}

void CInventoryExDialog::RefreshArmorItem()
{
	 if(m_pWearedDlg)
	 {
		 m_pWearedDlg->RefreshArmorItem();
	 }
}