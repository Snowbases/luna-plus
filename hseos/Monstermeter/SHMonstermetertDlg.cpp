#include "stdafx.h"
#include "SHMonstermeterDlg.h"
#include "GameIn.h"
#include "GameResourceManager.h"
#include "ObjectManager.h"
#include "WindowIDEnum.h"
#include "cStatic.h"
#include "..\effect\DamageNumber.h"
#include "ObjectBalloon.h"

#include "GMNotifyManager.h"
#include "./interface/cFont.h"

// 081020 LYW --- WeatherDialog : SetActive 함수 추가(공성전에서는 알아서 활성화 되지 않도록 수정한다. - 송가람).
#include "./SiegeWarfareMgr.h"

// 090820 pdy 맵이동시 Gift이벤트 유무에따라 활성화/비활성화가 정상작동안하는 버그 수정 
#include "ItemManager.h"

CSHMonstermeterDlg::CSHMonstermeterDlg()
{
	m_GiftEventTimeNumber.Init( 12, 0 );
	for(int i=0; i<10; i++)
	{
		m_GiftEventTimeNumber.InitDamageNumImage(CDamageNumber::GetImage(eDNK_Yellow, i), i);
	}
	m_GiftEventTimeNumber.SetFillZero( TRUE );
	m_GiftEventTimeNumber.SetScale( 0.5f, 0.5f );
	m_GiftEventTimeNumber.SetLimitCipher( 2 );

	m_ImageNumber.Init( 12, 0 );
	for(int i=0; i<10; i++)
	{
		m_ImageNumber.InitDamageNumImage(CDamageNumber::GetImage(eDNK_Yellow, i), i);
	}
	m_ImageNumber.SetFillZero( FALSE );
	m_ImageNumber.SetScale( 0.5f, 0.5f );
	m_ImageNumber.SetLimitCipher( 7 );

	m_PlayTimeText = NULL;
	m_KillMonNumText = NULL;

	m_GiftEventTime = 0;

	//090703 pdy Gift이벤트 유무에따라 활성화/비활성화 할 수 있도록 기능 개선  
	m_bGiftEvent = FALSE;
}

CSHMonstermeterDlg::~CSHMonstermeterDlg()
{
}
void CSHMonstermeterDlg::SetActive( BOOL val )
{
	// 081020 LYW --- SHMonstermeterDlg : SetActive 함수 추가(공성전에서는 알아서 활성화 되지 않도록 수정한다. - 송가람).
	if( SIEGEWARFAREMGR->IsSiegeWarfareZone() )
  	{
  		cDialog::SetActive( FALSE ) ;
  		return ;
  	}
	else
	{
		cDialog::SetActive( val ) ;
	}
	//cDialog::SetActive( val );
	//090703 pdy Gift이벤트 유무에따라 활성화/비활성화 할 수 있도록 기능 개선  
	if( m_GiftDlg )
	{
		m_GiftDlg->SetActive( m_bGiftEvent );
	}
}

void CSHMonstermeterDlg::Linking()
{
 	m_PlayTimeText = (cStatic*)GetWindowForID(MONSTERMETER_DLG_PLAYTIME);
	m_KillMonNumText = (cStatic*)GetWindowForID(MONSTERMETER_DLG_KILLMONNUM);

	//090703 pdy Gift이벤트 유무에따라 활성화/비활성화 할 수 있도록 기능 개선  
	m_GiftDlg		= (cDialog*)GetWindowForID(MONSTERMETER_DLG_GIFT_DLG);
	m_GiftTimeText1 = (cStatic*)m_GiftDlg->GetWindowForID( MONSTERMETER_DLG_GIFTTIME1 );
	m_GiftTimeText2 = (cStatic*)m_GiftDlg->GetWindowForID( MONSTERMETER_DLG_GIFTTIME2 );
	m_GiftTimeText3 = (cStatic*)m_GiftDlg->GetWindowForID( MONSTERMETER_DLG_GIFTTIME3 );

	// 090820 pdy 맵이동시 Gift이벤트 유무에따라 활성화/비활성화가 정상작동안하는 버그 수정 
	SetGiftEvent( ITEMMGR->IsGiftEvent() ) ;
}

void CSHMonstermeterDlg::Render()
{
 	if( !IsActive() ) return;

	cDialog::Render();

	// 몬스터 킬 정보
	//m_ImageNumber.SetPosition((int)m_PlayTimeText->GetAbsX(), (int)m_PlayTimeText->GetAbsY());
	//m_ImageNumber.SetNumber(HERO->GetMonstermeterInfo()->nPlayTimeTotal);
	//m_ImageNumber.RenderWithDamageNumImage();

	//m_ImageNumber.SetPosition((int)m_KillMonNumText->GetAbsX(), (int)m_KillMonNumText->GetAbsY());
	//m_ImageNumber.SetNumber(HERO->GetMonstermeterInfo()->nKillMonsterNumTotal);
	//m_ImageNumber.RenderWithDamageNumImage();


	//090703 pdy Gift이벤트 유무에따라 UI랜더링 활성화/비활성화 기능 추가 
	//이벤트 중이면 관련 렌더링 실행 
	if( m_bGiftEvent )
	{
		DWORD hour = m_GiftEventTime / ( 60 * 60 * 1000 );
		DWORD minute = ( m_GiftEventTime / ( 60 * 1000 ) ) % 60;
		DWORD second = ( m_GiftEventTime /  1000 ) % 60;

		m_GiftEventTimeNumber.SetPosition((int)m_GiftTimeText3->GetAbsX(), (int)m_GiftTimeText3->GetAbsY());
		m_GiftEventTimeNumber.SetNumber(hour);
		m_GiftEventTimeNumber.RenderWithDamageNumImage();

		m_GiftEventTimeNumber.SetPosition((int)m_GiftTimeText2->GetAbsX(), (int)m_GiftTimeText2->GetAbsY());
		m_GiftEventTimeNumber.SetNumber(minute);
		m_GiftEventTimeNumber.RenderWithDamageNumImage();

		m_GiftEventTimeNumber.SetPosition((int)m_GiftTimeText1->GetAbsX(), (int)m_GiftTimeText1->GetAbsY());
		m_GiftEventTimeNumber.SetNumber(second);
		m_GiftEventTimeNumber.RenderWithDamageNumImage();
	}

    if( NOTIFYMGR->IsEventNotifyUse() )
	{
		if( NOTIFYMGR->GetEventNotifyContext() == NULL || NOTIFYMGR->GetEventNotifyTitle() == NULL ) return;

		RECT rect = {
			LONG(GetAbsX()),
			LONG(GetAbsY()) + GetHeight() + 5,
			1,
			1};
		int nStrLen = strlen( NOTIFYMGR->GetEventNotifyTitle() );
		CFONT_OBJ->RenderFontShadow( 0, 1, NOTIFYMGR->GetEventNotifyTitle(), nStrLen, &rect, RGBA_MAKE(255,255,255,255) );
		rect.top += 15;
		
		nStrLen = strlen( NOTIFYMGR->GetEventNotifyContext() );
		int nSecLineLen = 0;
		if( nStrLen > 23 )
		{
			char* p = NOTIFYMGR->GetEventNotifyContext() + 23;	//끊을 곳
			if( p != CharNext( CharPrev( NOTIFYMGR->GetEventNotifyContext(), p ) ) ) //끊어지는 곳이 2바이트문자 중간이라면
			{
				nSecLineLen = nStrLen - 22;
				nStrLen = 22;
			}
			else
			{
				nSecLineLen = nStrLen - 23;
				nStrLen = 23;
			}
		}

		CFONT_OBJ->RenderFontShadow( 0, 1, NOTIFYMGR->GetEventNotifyContext(), nStrLen, &rect, RGBA_MAKE(255,255,255,255) );
		if( nSecLineLen )
		{
			rect.top += 15;
			CFONT_OBJ->RenderFontShadow( 0, 1, NOTIFYMGR->GetEventNotifyContext() + nStrLen, nSecLineLen, &rect, RGBA_MAKE(255,255,255,255) );
		}
	}
}

//090703 pdy Gift이벤트 유무에따라 활성화/비활성화 할 수 있도록 기능 개선  
void CSHMonstermeterDlg::SetGiftEvent(BOOL bGift)
{
	if( m_GiftDlg )
		m_GiftDlg->SetActive( bGift );

	m_bGiftEvent = bGift;
}
