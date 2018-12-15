// cTabDialog.cpp: implementation of the cTabDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cTabDialog.h"
#include "../Input/Mouse.h"
#include "cPushupButton.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cTabDialog::cTabDialog()
{
	m_ppPushupTabBtn=NULL;
	m_ppWindowTabSheet=NULL;
	m_bTabNum=0;
	m_bSelTabNum=0;
	m_type = WT_TABDIALOG;
	curIdx1 = 0;
	curIdx2 = 0;
//	m_BtnPushstartTime = 0;
//	m_BtnPushDelayTime = 700; 

	// 071210 LYW --- cTabDialog : 탭 다이얼로그의 스타일을 담는 변수를 초기화 한다.
	m_byTabStyle = e_Normal ;
	// 071210 LYW --- cTabDialog : 보여 질 탭 카운트를 담는 변수를 추가한다.
	m_byVisibleTabCount = 0 ;
}

cTabDialog::~cTabDialog()
{
	for(int i = 0 ; i < m_bTabNum ; i++)			// ÁÖÀÇ!: windowmanager¿¡ µî·ÏÇÏ°í ´Ù½Ã ¿©±â µî·ÏÇÒ¼ö ÀÖÀ¸´Ï..
	{
		SAFE_DELETE(m_ppPushupTabBtn[i]);
		SAFE_DELETE(m_ppWindowTabSheet[i]);
	}
	
	SAFE_DELETE_ARRAY(m_ppPushupTabBtn);
	SAFE_DELETE_ARRAY(m_ppWindowTabSheet);
	
}
void cTabDialog::InitTab(BYTE tabNum)
{
	curIdx1		= 0;
	curIdx2		= 0;
	m_bTabNum	= tabNum;

	if( m_ppPushupTabBtn )
	{
		SAFE_DELETE_ARRAY( m_ppPushupTabBtn );
	}

	if( m_ppWindowTabSheet )
	{
		SAFE_DELETE_ARRAY( m_ppWindowTabSheet );
	}

	m_ppPushupTabBtn	= new cPushupButton*	[ m_bTabNum ];
	m_ppWindowTabSheet	= new cWindow*			[ m_bTabNum ];

	ZeroMemory( m_ppPushupTabBtn,	sizeof( cPushupButton* )	* m_bTabNum );
	ZeroMemory( m_ppWindowTabSheet, sizeof( cWindow* )			* m_bTabNum );
}

void cTabDialog::SetAlpha( BYTE dwAlpha )
{
	cDialog::SetAlpha( dwAlpha );

	for(int i = 0 ; i < m_bTabNum ; ++i )
	{
		cPushupButton*	button	= m_ppPushupTabBtn[ i ];
		cWindow*		sheet	= m_ppWindowTabSheet[ i ];

		if( button )
		{
			button->SetAlpha( dwAlpha );
		}

		if( sheet )
		{
			sheet->SetAlpha( dwAlpha );
		}
	}
}

//KES 030825
void cTabDialog::SetOptionAlpha( DWORD dwAlpha)
{
	cDialog::SetOptionAlpha( dwAlpha );

	for(int i = 0 ; i < m_bTabNum ; i++)
	{
		cPushupButton*	button	= m_ppPushupTabBtn[ i ];
		cWindow*		sheet	= m_ppWindowTabSheet[ i ];

		if( button )
		{
			button->SetOptionAlpha( dwAlpha );
		}

		if( sheet )
		{
			sheet->SetOptionAlpha( dwAlpha );
		}
	}
}

void cTabDialog::AddTabBtn(BYTE idx, cPushupButton * btn)
{
	ASSERT(idx < m_bTabNum);
	ASSERT(!m_ppPushupTabBtn[idx]);
	btn->SetAbsXY((LONG)(m_absPos.x+btn->m_relPos.x), (LONG)(m_absPos.y+btn->m_relPos.y));
	btn->SetParent(this);
	btn->SetPassive(TRUE);

	// 071210 LYW --- cTablDialog : 보이기 옵션에 따라 활성화 여부를 설정한다.
	if( m_byTabStyle == e_VisibleOption )
	{
		if( idx >= m_byVisibleTabCount )
		{
			btn->SetDisable(TRUE) ;
			btn->SetActive(FALSE) ;
		}
	}

	if(idx == m_bSelTabNum)
		btn->SetPush(TRUE);
	else
		btn->SetPush(FALSE);
	m_ppPushupTabBtn[idx] = btn;

}
void cTabDialog::AddTabSheet(BYTE idx, cWindow * sheet)
{
	ASSERT(idx < m_bTabNum);
	ASSERT(!m_ppWindowTabSheet[idx]);
	sheet->SetAbsXY((LONG)(m_absPos.x+sheet->m_relPos.x), (LONG)(m_absPos.y+sheet->m_relPos.y));
	sheet->SetParent(this);

	// 071210 LYW --- cTablDialog : 보이기 옵션에 따라 활성화 여부를 설정한다.
	if( m_byTabStyle == e_VisibleOption )
	{
		if( idx >= m_byVisibleTabCount )
		{
			sheet->SetDisable(FALSE) ;
			sheet->SetActive(FALSE) ;
		}
	}

	m_ppWindowTabSheet[idx] = sheet;
}


DWORD cTabDialog::ActionEvent(CMouse * mouseInfo)
{
	if( ! m_bActive )
	{
		return WE_NULL;
	}
	
	DWORD we = cDialog::ActionEvent(mouseInfo);
	
	for(BYTE i = 0; i < m_bTabNum; ++i)
	{
		cPushupButton* button = m_ppPushupTabBtn[ i ];

		if( button )
		{
			DWORD we2 = button->ActionEvent( mouseInfo );

			if( we2 & WE_PUSHDOWN && m_bSelTabNum != i )//&& m_ppPushupTabBtn[i]->PtInWindow(mouseInfo->GetMouseX(), mouseInfo->GetMouseY()) && mouseInfo->LButtonPressed()))
			{
				SelectTab(i);		//¼ø¼­ÁöÅ°±â(¼ö·ÃÃ¢¶§¹®..)
				m_bSelTabNum = i;
			}	
		}
	}

	cWindow* sheet = m_ppWindowTabSheet[ m_bSelTabNum ];

	if( sheet )
	{
		we |= sheet->ActionEvent( mouseInfo );
	}

	return we;
}

void cTabDialog::SelectTab(BYTE idx)
{
	if( idx >= m_bTabNum )
	{
		return;
	}
	
	// 선택된 버튼과 시트 처리
	{
		cPushupButton*	button	= m_ppPushupTabBtn[idx];
		cWindow*		sheet	= m_ppWindowTabSheet[idx];

		if( ! button || ! sheet )
		{
			ASSERT( ! button && ! sheet && "Tab button and sheet should be match each other" );
			return;
		}

		button->SetPush( TRUE );
		sheet->SetActive( TRUE );
	}
	
	// 선택되지 않은 시트, 버튼 처리
	for(int i = 0 ; i < m_bTabNum ; ++i )
	{
		if( idx != i )
		{
			cPushupButton*	button	= m_ppPushupTabBtn[i];
			cWindow*		sheet	= m_ppWindowTabSheet[i];

			if( button )
			{
				button->SetPush( FALSE );
			}

			if( sheet )
			{
				sheet->SetActive( FALSE );
			}
		}
	}

	m_bSelTabNum = idx;
}

void cTabDialog::Render()
{
	cDialog::RenderWindow();
	cTabDialog::RenderTabComponent();
	cDialog::RenderComponent();
}

void cTabDialog::RenderTabComponent()
{
	if(!m_bActive) return;

	for( int i = 0 ; i < m_bTabNum ; ++i )
	{
		if(m_bSelTabNum == i)
		{
			cWindow* sheet = m_ppWindowTabSheet[ i ];

			if( sheet )
			{
				if( m_byTabStyle == e_VisibleOption )
				{
					if( sheet->IsActive() )
					{
						sheet->Render();
					}
				}
				else
				{
					sheet->Render();
				}
			}
		}

		cPushupButton* button = m_ppPushupTabBtn[ i ];

		if( button )
		{
			if( m_byTabStyle == e_VisibleOption )
			{
				if( button->IsActive() )
				{
					button->Render();
				}
			}
			else
			{
				button->Render();
			}
		}
	}
}

void cTabDialog::SetAbsXY(LONG x, LONG y)
{
	const LONG tmpX = x - (LONG)m_absPos.x;
	const LONG tmpY = y - (LONG)m_absPos.y;

	for(int i = 0 ; i < m_bTabNum ; ++i )
	{
		cPushupButton*	button	= m_ppPushupTabBtn[ i ];
		cWindow*		sheet	= m_ppWindowTabSheet[ i ];

		if( button )
		{
			const LONG absX = ( LONG )button->GetAbsX();
			const LONG absY = ( LONG )button->GetAbsY();

			button->SetAbsXY( absX + tmpX, absY + tmpY );
		}

		if( sheet )
		{
			const LONG absX = ( LONG )sheet->GetAbsX();
			const LONG absY = ( LONG )sheet->GetAbsY();

			sheet->SetAbsXY( absX + tmpX,  absY + tmpY );
		}
	}
	cDialog::SetAbsXY(x,y);
}

void cTabDialog::SetActive(BOOL isActive)
{
	if( m_bDisable )
	{
		return;
	}
	
	for(int i = 0 ; i < m_bTabNum ; ++i )
	{
		cPushupButton* button = m_ppPushupTabBtn[i];

		if( button )
		{
			if( m_byTabStyle == e_VisibleOption )
			{
				if( !button->IsDisable() )
				{
					button->SetActive( isActive ) ;
				}
			}
			else
			{
				button->SetActive( isActive );
			}
		}
		
		cWindow* sheet = m_ppWindowTabSheet[i];

		if( sheet )
		{
			if( m_byTabStyle == e_VisibleOption )
			{
				if( !sheet->IsDisable() )
				{
					if( !isActive || (isActive && i == m_bSelTabNum) )
					{
						sheet->SetActive( isActive );
					}
				}
			}
			else
			{
				if( !isActive || (isActive && i == m_bSelTabNum) )
				{
					sheet->SetActive( isActive );
				}
			}			
		}
	}

	cDialog::SetActiveRecursive(isActive);
}

//090119 pdy hide window
void cTabDialog::SetHide(BOOL Val)
{
	if( m_bDisable )
	{
		return ;
	}
	
	for(int i = 0 ; i < m_bTabNum ; ++i )
	{
		cPushupButton* button = m_ppPushupTabBtn[i];

		if( button )
		{
			if( m_byTabStyle == e_VisibleOption )
			{
				if( !button->IsDisable() )
				{
					button->SetHide( Val ) ;
				}
			}
			else
			{
				button->SetHide( Val );
			}
		}
		
		cWindow* sheet = m_ppWindowTabSheet[i];

		if( sheet )
		{
			if( m_byTabStyle == e_VisibleOption )
			{
				if( !sheet->IsDisable() )
				{
					if( !Val || (Val && i == m_bSelTabNum) )
					{
						sheet->SetHide( Val );
					}
				}
			}
			else
			{
				if( !Val || (Val && i == m_bSelTabNum) )
				{
					sheet->SetHide( Val );
				}
			}			
		}
	}

	cDialog::SetHideRecursive(Val);
}


cPushupButton * cTabDialog::GetTabBtn(BYTE idx)
{
	ASSERT(idx < m_bTabNum);
	return m_ppPushupTabBtn[idx];
}

cWindow * cTabDialog::GetTabSheet(BYTE idx)
{
	ASSERT(idx < m_bTabNum);
	return m_ppWindowTabSheet[idx];
}


void cTabDialog::SetDisable( BOOL isDisable )
{
	cDialog::SetDisable( isDisable );

	for(int i = 0 ; i < m_bTabNum ; i++)
	{
		cPushupButton*	button	= m_ppPushupTabBtn[i];
		cWindow*		sheet	= m_ppWindowTabSheet[i];

		if( m_byTabStyle == e_VisibleOption )
		{
			if( i >= m_byVisibleTabCount ) continue ;

			if( button )
			{
				button->SetDisable( isDisable );
			}

			if( sheet )
			{
				sheet->SetDisable( isDisable );
			}
		}
		else
		{
			if( button )
			{
				button->SetDisable( isDisable );
			}

			if( sheet )
			{
				sheet->SetDisable( isDisable );
			}
		}
	}
}


cWindow* cTabDialog::GetWindowForID(LONG id)
{
	cWindow* pWindow = cDialog::GetWindowForID( id );

	if( !pWindow )
	{
		for(int i = 0 ; i < m_bTabNum ; ++i )
		{
			cPushupButton* button = m_ppPushupTabBtn[i];

			if( button->GetID() == id )
			{
				pWindow = button;
				break;
			}
			
			cWindow* sheet = m_ppWindowTabSheet[i];

			if( sheet->GetID() == id )
			{
				pWindow = sheet;
				break;
			}
		}
	}

	return pWindow;
}


// 071210 LYW --- cTabDialog : 탭 다이얼로그의 스타일을 설정하는 함수 추가.
void cTabDialog::SetTabStyle(BYTE byStyle)
{
	if( byStyle >= e_Max_Style )
	{
		char tempBuf[128] = {0, } ;
		sprintf(tempBuf, "%d, Invalid tab style.", m_ID) ;
		MessageBox(NULL, tempBuf, "cTabDialog", MB_OK);

		return ;
	}

	m_byTabStyle = byStyle ;
}

// 071210 LYW --- cTabDialog : 보여질 탭 카운트를 담는 변수를 설정하는 함수 추가.
void cTabDialog::SetVisibleTabCount(BYTE byCount)
{
	if( byCount > m_bTabNum )
	{
		char tempBuf[128] = {0, } ;
		sprintf(tempBuf, "%d, Invalid visible count.", m_ID) ;
		MessageBox(NULL, tempBuf, "cTabDialog", MB_OK);

		return ;
	}

	m_byVisibleTabCount = byCount ;
}

// 071210 LYW --- cTabDialog : 선택한 탭을 보이게 하는 함수 추가.
void cTabDialog::ShowTab(BYTE byTabNum, BOOL bShow)
{
	if( byTabNum >= m_bTabNum )
	{
		char tempBuf[128] = {0, } ;
		sprintf(tempBuf, "%d, Show tab - Invalid tab number.", byTabNum) ;
		MessageBox(NULL, tempBuf, "cTabDialog", MB_OK);

		return ;
	}

	if( !m_ppPushupTabBtn[byTabNum] ) return ;
	if( !m_ppWindowTabSheet[byTabNum] ) return ;

	BOOL bDisable = FALSE ;
	BOOL bActive  = FALSE ;

	if( bShow )
	{
		bActive  = TRUE ;
	}

	m_ppPushupTabBtn[byTabNum]->SetDisable(bDisable) ;
	m_ppPushupTabBtn[byTabNum]->SetActive(bActive) ;

	m_ppWindowTabSheet[byTabNum]->SetDisable(bDisable) ;
	m_ppWindowTabSheet[byTabNum]->SetActive(bActive) ;

	if( !bShow )
	{
		m_ppPushupTabBtn[byTabNum]->SetDisable(TRUE) ;
		m_ppWindowTabSheet[byTabNum]->SetDisable(TRUE) ;
	}
}