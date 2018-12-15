#include "stdafx.h"
#include "cIcon.h"

#include "cWindowManager.h"
#include "cScriptManager.h"
#include "cButton.h"
#include "./Audio/MHAudioManager.h"
#include "../Input/Mouse.h"
#include "cIconDialog.h"


cIcon::cIcon()
{
	SCRIPTMGR->GetImage( 2, &mCoolTime.mImage );
	mCoolTime.mMax	= 0;

	m_IconType		= 1;
	m_dwData		= 0;
	m_type			= WT_ICON;

	m_bLock			= FALSE;
//	m_pLockImage	= NULL;

	m_bZeroCount = FALSE ;

	m_bState = FALSE;

	m_bSeal = FALSE ;
}


cIcon::~cIcon()
{
	PTRLISTSEARCHSTART(m_LinkDialogList,cIconDialog*,pDlg)
		pDlg->NotifyLinkIconDeleted(this);
	PTRLISTSEARCHEND
}


void cIcon::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
	RECT capRect={0,0,wid,hei};
	SetCaptionRect(&capRect);
	
	//m_LockImage = WINDOWMGR->GetImageOfFile( "./Data/Interface/2DImage/GameIn/98/9810110.tif", 98 );
	
	SCRIPTMGR->GetImage( 3, &m_LockImage );
	SCRIPTMGR->GetImage( 104, &m_ZeroImage ) ;
	// 071126 LYW --- cIcon : 봉인 이미지 추가.
	SCRIPTMGR->GetImage( 123, &m_SealImage ) ;
}


void cIcon::Render()
{
	if( !m_bActive )		return;

	cDialog::Render();
	
	if( m_bLock )
		m_LockImage.RenderSprite( NULL, NULL, 0, &m_absPos, 
									RGBA_MERGE(0xffffff, m_alpha * m_dwOptionAlpha / 100));

	// 071126 LYW --- cIcon : 봉인 이미지 처리.
	if( m_bSeal )
	{
		m_SealImage.RenderSprite( NULL, NULL, 0, &m_absPos, 
									RGBA_MERGE(0xffffff, 255));
	}

	if( m_bState )
	{
		m_StateImage.RenderSprite( NULL, NULL, 0, &m_absPos, 
									RGBA_MERGE(0xffffff, 255));
	}

	if( mCoolTime.mMax )
	{
		const DWORD	tickCount = GetTickCount();

		// 남은 시간 별로 정렬된 우선순위 큐의 가장 상위에 있는 유닛의 시간이 만료되었는지 체크하자
		if( mCoolTime.mBeginTick > mCoolTime.mEndTick && tickCount < mCoolTime.mBeginTick && tickCount > mCoolTime.mEndTick ||	// 오버플로
			mCoolTime.mBeginTick < mCoolTime.mEndTick && mCoolTime.mEndTick < tickCount )										// 일반
		{
			mCoolTime.mMax	= 0;
		}
		else 
		{
			const float distance	= float( tickCount > mCoolTime.mEndTick ? UINT_MAX - tickCount + mCoolTime.mEndTick : mCoolTime.mEndTick - tickCount );
			const float rate		= distance / mCoolTime.mMax;
			VECTOR2		scale		= { 1.0f, m_height * rate };
			VECTOR2		position	= { m_absPos.x, m_absPos.y + m_height * ( 1.0f - rate ) };

			mCoolTime.mImage.RenderSprite( &scale, 0, 0, &position, 0xff000000 );
		}
	}
}

void cIcon::Render(VECTOR2* pPos)
{
	VECTOR2 backup = m_absPos;
	m_absPos = *pPos;
	Render();
	m_absPos = backup;
}


DWORD cIcon::ActionEvent(CMouse * mouseInfo)
{
	DWORD we = WE_NULL;
	we |= cWindow::ActionEvent(mouseInfo);
	if(m_bDisable) return we;
	we |= cDialog::ActionEventComponent(mouseInfo);
//	if( !m_bActive ) return we;

//	long x = mouseInfo->GetMouseX();
//	long y = mouseInfo->GetMouseY();
	
	long x2 = mouseInfo->GetMouseEventX();
	long y2 = mouseInfo->GetMouseEventY();

	if( m_pCloseBtn )
	{
		we = m_pCloseBtn->ActionEvent(mouseInfo);

		if(we & WE_BTNCLICK)
		{
			cbWindowFunc(m_ID, this, WE_CLOSEWINDOW);
			return (we |= WE_CLOSEWINDOW);
		}

		if( m_pCloseBtn->IsClickInside() )
			return we;
	}

	if( mouseInfo->LButtonDown() && !WINDOWMGR->IsMouseDownUsed() )
	{
		if( PtInWindow( x2, y2 ) )
		{
			if( m_bMovable && PtInCaption( x2, y2 ) )
				m_bCaptionHit = TRUE;
			
			we |= WE_TOPWINDOW;
		}
	}

	if( mouseInfo->LButtonUp() )
		m_bCaptionHit = FALSE;

	if( m_bCaptionHit )
	{
		if( mouseInfo->LButtonDrag() )
		{
			if(WINDOWMGR->m_OldDragFlag == FALSE)
			{					
				SetDepend(FALSE);
				
				LONG OrigX = (LONG)m_absPos.x;
				LONG OrigY = (LONG)m_absPos.y;
				
				WINDOWMGR->SetDragStart(OrigX, OrigY, DEFAULT_ICONSIZE/2, DEFAULT_ICONSIZE/2, m_ID);
				
				m_bCaptionHit = FALSE;
				
				AUDIOMGR->Play(
					57,
					gHeroID);
			}
		}
	}
	return we;
}


void cIcon::SetLock( BOOL bLock )
{
	m_bLock = bLock;

//	SetMovable( !bLock );
}

void cIcon::AddLinkIconDialog(cIconDialog* pDialog)
{
	m_LinkDialogList.AddTail(pDialog);
}
void cIcon::RemoveLinkIconDialog(cIconDialog* pDialog)
{
	m_LinkDialogList.Remove(pDialog);
}

void cIcon::SetCoolTimeAbs(DWORD dwBegin, DWORD dwEnd, DWORD miliSecond)
{
	mCoolTime.mBeginTick	= dwBegin;
	mCoolTime.mEndTick		= dwEnd;
	mCoolTime.mMax			= miliSecond;
}

void cIcon::SetCoolTime( DWORD miliSecond )
{	
	const DWORD currentTick	= GetTickCount();

	mCoolTime.mBeginTick	= currentTick;
	mCoolTime.mEndTick		= currentTick + miliSecond;
	mCoolTime.mMax			= miliSecond;

	//m_bDisable				= TRUE;
}


DWORD cIcon::GetRemainedCoolTime() const
{
	if( ! mCoolTime.mMax )
	{
		return 0;
	}

	const DWORD tick = GetTickCount();

	// 주의: 렌더링이 끝나기 직전에 해당 요청이 오는 경우, 값이 넘칠 수 있다. 주의깊게 체크해야 한다.

	if( mCoolTime.mBeginTick < mCoolTime.mEndTick )
	{
		return mCoolTime.mEndTick < tick ? 0 : mCoolTime.mEndTick - tick;
	}

	// 쿨타임 설정 정보는 오버플로 상태이나, tick은 아직 그렇지 않음
	if( mCoolTime.mEndTick < tick && mCoolTime.mBeginTick < tick )
	{
		return mCoolTime.mEndTick + ( UINT_MAX - tick );
	}
	// tick도 오버플로되었음
	else
	{
		return mCoolTime.mEndTick < tick ? 0 : mCoolTime.mEndTick - tick;
	}
}


// 080225 LUJ, 툴팁을 강제로 렌더링한다
void cIcon::ToolTipForcedRender()
{
	//여기서 윈도우 메니져에게 보낼까?
	WINDOWMGR->SetToolTipWindow( &toolTip );

	toolTip.SetXY(
		LONG(m_absPos.x + TOOLTIP_MOUSESIZE_X),
		LONG(m_absPos.y + TOOLTIP_MOUSESIZE_Y));
}