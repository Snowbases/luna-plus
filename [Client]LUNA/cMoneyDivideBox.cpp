#include "stdafx.h"
#include ".\cmoneydividebox.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cButton.h"
#include "./Interface/cStatic.h"
#include "./Interface/cSpin.h"
#include "WindowIDEnum.h"
#include "cIMEEX.h"

cMoneyDivideBox::cMoneyDivideBox(void)
{
	mMoney = 0;
	mColor = TTTC_ONDERMILLION;
}

cMoneyDivideBox::~cMoneyDivideBox(void)
{
}

void cMoneyDivideBox::Render()
{
	if( m_bActive )
	{
		if( mMoney != DWORD(GetValue()))
		{
			mMoney = DWORD(GetValue());
			mColor = GetMoneyColor(mMoney);
		}
		m_pSpin->SetActiveTextColor( mColor );
		m_pSpin->SetNonactiveTextColor( mColor );
	}

	cDivideBox::Render();
}

void cMoneyDivideBox::CreateDivideBox( LONG x, LONG y, LONG ID, cbDivideFUNC cbFc1, cbDivideFUNC cbFc2, void * vData1, void* vData2, char* strTitle )
{
	m_cbDivideFunc = cbFc1;
	m_cbCancelFunc = cbFc2;
	m_vData1 = vData1;
	m_vData2 = vData2;
	SetAbsXY(x,y);
	SetID(ID);

	m_pOKBtn = (cButton *)GetWindowForID(CMI_MONEYDIVIDEOK);
	m_pCancelBtn = (cButton *)GetWindowForID(CMI_MONEYDIVIDECANCEL);

	m_pSpin = (cSpin *)GetWindowForID(CMI_MONEYDIVIDESPIN);
	m_pSpin->SetValidCheck(VCM_NUMBER); // 숫자만 입력 되도록 한다.
	m_pSpin->SetAlign( TXT_RIGHT );		// 오른정렬
	m_pSpin->SetFocusEdit( TRUE );
//	m_pSpin->SetUnit( 0 );

	// Title Text 입력 LBS 03.11.19 : 수정 KES
	if( strTitle )
	{
		cStatic* pStatic = new cStatic;
		
		// 070504 LYW --- 타이틀 위치 수정
		//pStatic->Init( 13, 2, 0, 0, NULL );
		pStatic->Init( 13, 10, 0, 0, NULL );
		pStatic->SetShadow( TRUE );
		pStatic->SetFontIdx( 2 );
		pStatic->SetAlign( TXT_LEFT );
		pStatic->SetStaticText( strTitle );
		
		Add( pStatic );
	}
}