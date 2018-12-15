#include "stdafx.h"
#include "ReinforceGuideDialog.h"
#include "WindowIDEnum.h"
#include "Interface/cPushupButton.h"
#include "interface/cStatic.h"
#include "interface/cIconDialog.h"
#include "ItemManager.h"
#include "ItemShow.h"
#include "cScriptManager.h"
#include "MHFile.h"


CReinforceGuideDialog::CReinforceGuideDialog(void)
{}


CReinforceGuideDialog::~CReinforceGuideDialog(void)
{
	Release();
}


void CReinforceGuideDialog::Release()
{
	for( IconList::iterator it = mIconList.begin();
		mIconList.end() != it;
		++it )
	{
		SAFE_DELETE( *it );
	}
}


void CReinforceGuideDialog::Add( cWindow* window )
{
	switch( window->GetType() )
	{
	case WT_PUSHUPBUTTON:
		{
			AddTabBtn( curIdx1++, ( cPushupButton* )window );
			break;
		}
	case WT_ICONDIALOG:
		{
			AddTabSheet( curIdx2++, window );
			break;
		}
	default:
		{
			cDialog::Add(window);
			break;
		}
	}
}


void CReinforceGuideDialog::Linking()
{	
	Release();

	typedef std::map< DWORD, std::string >				ReinforceDescription;
	ReinforceDescription								mReinforceDescription;

	// 강화 아이템마다 짧은 도움말을 바인딩해놓는다.
	{
		CMHFile file;

		if( ! file.Init( "data/interface/windows/ReinforceGuideDescription.bin", "rb" ) )
		{
			ASSERT( 0 );
			return;
		}

		while( ! file.IsEOF() )
		{
			const DWORD itemIndex	= file.GetDword();
			const char* text		= file.GetString();

			mReinforceDescription[ itemIndex ] = std::string( text );
		}
	}

	typedef std::pair< DWORD, const ReinforceScript* > Script;
	typedef std::multimap< EWEARED_ITEM, Script > ScriptAsSlot;
	ScriptAsSlot mScriptAsSlot;

	const CGameResourceManager::ReinforceScriptContainer& scriptContainer = g_CGameResourceManager.GetReinforceScript();

	// 강화 스크립트를 읽어 부위별로 분류해놓는다.
	for(CGameResourceManager::ReinforceScriptContainer::const_iterator it = scriptContainer.begin();
		scriptContainer.end() != it;
		++it )
	{
		const DWORD				itemIndex		= it->first;
		const ReinforceScript&	reinforceScript	= it->second;

		// 레어 강화용 아이콘은 표시하지 않는다.
		if( reinforceScript.mForRare )
		{
			continue;
		}
        
		for(	ReinforceScript::Slot::const_iterator inner = reinforceScript.mSlot.begin();
				reinforceScript.mSlot.end() != inner;
				++inner )
		{
			const EWEARED_ITEM slot = *inner;

			// 반지2와 귀걸이2가 세팅된 아이템은 반지1과 귀걸이1도 세팅되어 있으므로 둘 중 하나만 처리해야한다.
			if(eWearedItem_Ring2 != slot &&
				eWearedItem_Earring2 != slot)
			{
				mScriptAsSlot.insert(
					std::make_pair(slot, Script(itemIndex, &reinforceScript)));
			}
		}
	}

	cImage image;
	SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );

	// 분류한 것을 부위별로 표시한다
	for(ScriptAsSlot::const_iterator it = mScriptAsSlot.begin();
		mScriptAsSlot.end() != it;
		++it )
	{
		const EWEARED_ITEM slot	= it->first;
		cIconDialog* dialog	= GetTabDialog( slot );

		if( !	dialog ||
				dialog->GetType() != WT_ICONDIALOG )
		{
			//ASSERT( 0 && "해당 정보를 표시할 창이 정의되지 않았거나 아이콘 창이 아님" );
			continue;
		}

		CItemShow*	icon		= new CItemShow;
		WORD		iconIndex	= 0;

		for( ; dialog->GetCellNum() > iconIndex; ++iconIndex )
		{
			if( dialog->IsAddable( iconIndex ) )
			{
				dialog->AddIcon( iconIndex, icon );
				break;
			}
		}

		if( dialog->GetCellNum() <= iconIndex )
		{
			ASSERT( 0 && "아이콘 창에 새로운 아이콘을 추가할 공간이 없음" );
			SAFE_DELETE( icon );
			continue;
		}

		// 나중에 일괄 삭제하기 위해 리스트에 보관하자
		mIconList.push_back( icon );

		const DWORD				itemIndex	= it->second.first;
		const ReinforceScript*	script		= it->second.second;

		ASSERT( itemIndex && script );

		// 아이콘 세팅
		{
			icon->SetMovable( FALSE );
			icon->SetVisibleDurability( FALSE );
			icon->SetData( itemIndex );
			icon->Init( itemIndex, 1 );

			ITEMMGR->AddToolTip( icon );
		}
		
		// 텍스트 세팅
		{
			cStatic*			titleText		= GetTitleStatic( dialog, iconIndex );
			cStatic*			descriptionText	= GetDescriptionStatic( dialog, iconIndex );
			const ITEM_INFO*	info			= ITEMMGR->GetItemInfo( itemIndex );

			ASSERT( titleText && descriptionText && info );
			
			const std::string& text = mReinforceDescription[ itemIndex ];

			descriptionText->SetStaticText( text.c_str() );
			titleText->SetStaticText( info->ItemName );
		}
	}
}


void CReinforceGuideDialog::OnActionEvent(LONG id, void * p, DWORD we)
{
	if( we & WE_BTNCLICK && id == RFGUIDE_OKBTN )
	{
		SetActive( FALSE );
	}
}


void CReinforceGuideDialog::SetActive( BOOL isActive )
{	
	if( isActive )
	{
		cWindow* window = GetTabSheet( 0 );
		ASSERT( window );

		window->SetActive( TRUE );
	}

	cTabDialog::SetActive( isActive );
}


void CReinforceGuideDialog::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();	
	cTabDialog::RenderTabComponent();
}


cIconDialog* CReinforceGuideDialog::GetTabDialog(EWEARED_ITEM slot)
{
	// 부위별로 탭을 가져온다. 이건 부득이하지만 하드코딩할 수 밖에 없다.
	// 무기 탭은 0번...

	cWindow* window = 0;

	switch( slot )
	{
	case eWearedItem_Weapon:
		{
			window = GetWindowForID( RFGUIDE_SHEET1 );
			break;
		}
	case eWearedItem_Shield:
		{
			window = GetWindowForID( RFGUIDE_SHEET2 );
			break;
		}
	case eWearedItem_Dress:
		{
			window = GetWindowForID( RFGUIDE_SHEET3 );
			break;
		}
	case eWearedItem_Hat:
		{
			window = GetWindowForID( RFGUIDE_SHEET4 );
			break;
		}
	case eWearedItem_Glove:
		{
			window = GetWindowForID( RFGUIDE_SHEET5 );
			break;
		}
	case eWearedItem_Shoes:
		{
			window = GetWindowForID( RFGUIDE_SHEET6 );
			break;
		}
	case eWearedItem_Ring1:
	case eWearedItem_Ring2:
		{
			window = GetWindowForID( RFGUIDE_SHEET7 );
			break;
		}
	case eWearedItem_Necklace:
		{
			window = GetWindowForID( RFGUIDE_SHEET8 );
			break;
		}
	case eWearedItem_Earring1:
	case eWearedItem_Earring2:
		{
			window = GetWindowForID( RFGUIDE_SHEET9 );
			break;
		}
	case eWearedItem_Belt:
		{
			window = GetWindowForID( RFGUIDE_SHEET10 );
			break;
		}
	}

	return ( cIconDialog* )window;
}


cStatic* CReinforceGuideDialog::GetTitleStatic( cDialog* dialog, DWORD iconIndex )
{
	ASSERT( dialog );

	cWindow* window = 0;

	switch( dialog->GetID() )
	{
	case RFGUIDE_SHEET1:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET1_TITLE1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET2:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET2_TITLE1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET3:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET3_TITLE1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET4:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET4_TITLE1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET5:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET5_TITLE1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET6:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET6_TITLE1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET7:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET7_TITLE1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET8:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET8_TITLE1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET9:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET9_TITLE1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET10:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET10_TITLE1 + iconIndex );
			break;
		}
	}

	return ( cStatic* )window;
}


cStatic* CReinforceGuideDialog::GetDescriptionStatic( cDialog* dialog, DWORD iconIndex )
{
	ASSERT( dialog );

	cWindow* window = 0;

	switch( dialog->GetID() )
	{
	case RFGUIDE_SHEET1:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET1_DESC1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET2:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET2_DESC1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET3:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET3_DESC1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET4:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET4_DESC1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET5:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET5_DESC1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET6:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET6_DESC1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET7:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET7_DESC1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET8:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET8_DESC1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET9:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET9_DESC1 + iconIndex );
			break;
		}
	case RFGUIDE_SHEET10:
		{
			window = dialog->GetWindowForID( RFGUIDE_SHEET10_DESC1 + iconIndex );
			break;
		}
	}

	return ( cStatic* )window;
}