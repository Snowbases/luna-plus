#include "stdafx.h"															
#include ".\ItemPopupDlg.h"											

#include "../WindowIDEnum.h"												

#include "./Interface/cStatic.h"											
#include "./Interface/cTextArea.h"											
#include "cIMEex.h"

#include "GameIn.h"															
#include "ObjectManager.h"

cItemPopupDlg::cItemPopupDlg(void)										
{
	mpMiddle = NULL;
	mpBottom = NULL;
	mpTextArea = NULL;
}

cItemPopupDlg::~cItemPopupDlg(void)										
{
}


void cItemPopupDlg::Linking() 											
{
	mpMiddle = (cStatic*)GetWindowForID(ITEMPOPUP_MIDDLE) ;
	mpBottom = (cStatic*)GetWindowForID(ITEMPOPUP_BOTTOM) ;
	mpTextArea = (cTextArea*)GetWindowForID(ITEMPOPUP_TEXT) ;

	mpTextArea->SetFontIdx( 7 ) ;
	mpTextArea->GetIMEex()->SetFont( 7 );
	mpTextArea->SetLineHeight( mpMiddle->GetHeight() );

	mShadow = RGB(96, 98, 1);
	mFont = RGB(255, 255, 255);
	mTextPosX = int(mpTextArea->GetAbsX());
	mTextPosY = int(mpTextArea->GetAbsY());
}

void cItemPopupDlg::Render() 												
{
	if(!m_bActive) return;	

	if( mActiveTime + 5000 < gCurTime )
	{
		SetActive( FALSE );
		return;
	}

	cDialog::Render();

//	mpMiddle->Render();
//	mpBottom->Render();

	mpTextArea->SetFontColor( mShadow ) ;
	mpTextArea->SetAbsXY( mTextPosX + 1, mTextPosY + 2 );
	mpTextArea->Render();

	mpTextArea->SetFontColor( mFont );
	mpTextArea->SetAbsXY( mTextPosX, mTextPosY );
	mpTextArea->Render();
}

void cItemPopupDlg::SetText( char* text )
{
	mpTextArea->SetScriptText( text ) ;
}

void cItemPopupDlg::SetActive( BOOL val )
{
	if( val )
	{
		mActiveTime = gCurTime;

		int nTextLine = mpTextArea->GetIMEex()->GetTotalLine() ;

		if( nTextLine < 2 )
		{
			mpMiddle->SetActive( FALSE );
			mpBottom->SetAbsY(
				LONG(mpMiddle->GetAbsY()));
		}
		else
		{
			cImageScale scale;

			scale.x = 1.0f;
			scale.y = (float)( nTextLine - 1 );
			
			mpMiddle->SetActive( TRUE );
			mpMiddle->SetScale( &scale );
			mpBottom->SetAbsY(
				LONG(mpMiddle->GetAbsY() + ( mpMiddle->GetHeight() * ( nTextLine - 1 ))));
		}
	}

	cDialog::SetActive( val );
}

DWORD cItemPopupDlg::ActionEvent(CMouse * mouseInfo)
{
	return WE_NULL;
}