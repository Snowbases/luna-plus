#include "stdafx.h"
#include "AdditionalButtonDlg.h"
#include "WindowIDEnum.h"
#include "cButton.h"
#include "../input/Mouse.h"
#include "GameResourceManager.h"
#include "../Interface/cWindowManager.h"
#include "VideoCaptureManager.h"

cAdditionalButtonDlg::cAdditionalButtonDlg()
{
	m_pButtonRecordStart = NULL;
	m_pButtonRecordStop = NULL;
}

cAdditionalButtonDlg::~cAdditionalButtonDlg()
{
}

void cAdditionalButtonDlg::Linking()
{
	m_pButtonRecordStart = ( cButton* )GetWindowForID( ABD_VIDEO_REC_START_BTN );
	m_pButtonRecordStop = ( cButton* )GetWindowForID( ABD_VIDEO_REC_STOP_BTN );
}


void cAdditionalButtonDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	if( we & WE_BTNCLICK )
	{
 		switch( lId )
		{
			case ABD_VIDEO_REC_START_BTN:
			{
			}
			break;
			case ABD_VIDEO_REC_STOP_BTN:
			{
				ShowRecordStartBtn( TRUE );
				SetActive( FALSE );
				VIDEOCAPTUREMGR->CaptureStop();
			}
			break;
		}
	//	SetActive( FALSE );
	}
}

void cAdditionalButtonDlg::SetActive(BOOL val)
{
	cDialog::SetActive( val );
}

void cAdditionalButtonDlg::ShowRecordStartBtn( BOOL val )
{
	m_pButtonRecordStart->SetActive( val );
	m_pButtonRecordStop->SetActive( !val );
}