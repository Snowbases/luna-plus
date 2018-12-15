#include "stdafx.h"
#include "VideoCaptureDlg.h"
#include "WindowIDEnum.h"
#include "cComboBox.h"
#include "cCheckBox.h"
#include "./Interface/cResourceManager.h"

CVideoCaptureDlg::CVideoCaptureDlg()
{
}

CVideoCaptureDlg::~CVideoCaptureDlg()
{
}

void CVideoCaptureDlg::Linking()
{
	m_pSizeCombo			= (cComboBox*)GetWindowForID( VIDEOCAPTURE_SIZE_COMBO );
	m_pOptCombo				= (cComboBox*)GetWindowForID( VIDEOCAPTURE_OPT_COMBO );
	m_pNoCursor				= (cCheckBox*)GetWindowForID( VIDEOCAPTURE_NO_CURSOR_CHECKBOX );

	m_pSizeCombo->RemoveAll();
	m_pOptCombo->RemoveAll();

	// 해상도 ComboBox 목록 설정
	for( int i=0 ; i<eVideoCaptureSize_Max ; ++i )
	{
		ITEM* pItem = new ITEM;
		ZeroMemory( pItem, sizeof(ITEM) );
		SafeStrCpy( pItem->string, GetTextVideoCaptureSize( (eVideoCaptureSize)i ), MAX_LISTITEM_SIZE );
		pItem->rgb = RGB(255, 255, 255);

		m_pSizeCombo->AddItem( pItem );
	}

	// 화질 ComboBox 목록 설정
	for( int i=0 ; i<eVideoCaptureOpt_Max ; ++i )
	{
		ITEM* pItem = new ITEM;
		ZeroMemory( pItem, sizeof(ITEM) );
		SafeStrCpy( pItem->string, GetTextVideoCaptureOpt( (eVideoCaptureOpt)i ), MAX_LISTITEM_SIZE );
		pItem->rgb = RGB(255, 255, 255);

		m_pOptCombo->AddItem( pItem );
	}

	m_pSizeCombo->SelectComboText( 0 );
	m_pOptCombo->SelectComboText( 0 );
}

const char* CVideoCaptureDlg::GetTextVideoCaptureSize( eVideoCaptureSize eSize )
{
	switch( eSize )
	{
	case eVideoCaptureSize_Default:
		return RESRCMGR->GetMsg( 1205 );	// "전체화면"
	case eVideoCaptureSize_500x375:
		return RESRCMGR->GetMsg( 1206 );	// "500x375"
	}

	return "";
}

const char* CVideoCaptureDlg::GetTextVideoCaptureOpt( eVideoCaptureOpt eOpt )
{
	switch( eOpt )
	{
	case eVideoCaptureOpt_High:
		return RESRCMGR->GetMsg( 1208 );	//"고화질"
	case eVideoCaptureOpt_Low:
		return RESRCMGR->GetMsg( 1209 );	//"저화질"
	}

	return "";
}

int CVideoCaptureDlg::GetSelecedSize()
{
	return m_pSizeCombo->GetCurSelectedIdx();
}

int CVideoCaptureDlg::GetSelecedOpt()
{
	return m_pOptCombo->GetCurSelectedIdx();
}

BOOL CVideoCaptureDlg::IsIncludeCursor()
{
	return !m_pNoCursor->IsChecked();
}