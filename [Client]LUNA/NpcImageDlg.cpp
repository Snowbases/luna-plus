#include "stdafx.h"																	// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "WindowIdEnum.h"															// 윈도우 아이디가 정의 되어 있는 헤더 파일을 불러온다.
#include "MHFile.h"																	// 묵향 파일 클래스 헤더파일을 불러온다.
#include "../[Client]LUNA/Interface/cScriptManager.h"								// 스크립트 매니져 헤더파일을 불러온다.
#include "./interface/cWindowManager.h"												// 윈도우 매니져 헤더파일을 불러온다.
#include ".\npcimagedlg.h"															// NPC 이미지 다이얼로그 클래스 헤더파일을 불러온다.

cNpcImageDlg::cNpcImageDlg(void)													// 생성자 함수.
{
	COLOR = RGBA_MAKE(255,255,255,150);												// 색상을 세팅한다.
	COLOR2 = RGBA_MAKE(255,255,255,255);												// 색상을 세팅한다.

	SCALE.x = 450.f/512.f;															// 스케일을 세팅한다.
	SCALE.y = 450.f/512.f;
	BACK_POS.x = 0;																	// 배경 위치를 세팅한다.
	BACK_POS.y = 0;

	mEmotion = EMOTION_NORMAL;
	
	m_pTopImage	= NULL ;															// 탑 이미지 포인터를 NULL 처리를 한다.
	m_pBottomImage = NULL ;															// 버텀 이미지 포인터를 NULL 처리를 한다.
	ZeroMemory(
		&mScreenRect,
		sizeof(mScreenRect));
	ZeroMemory(
		&mpCurImage,
		sizeof(mpCurImage));
	ZeroMemory(
		&mImageBack,
		sizeof(mImageBack));
}

cNpcImageDlg::~cNpcImageDlg(void)													// 소멸자 함수.
{}

void cNpcImageDlg::Linking()														// 링크 함수.
{
	m_pTopImage	= (cStatic*)GetWindowForID(NI_TOPIMG) ;								// 탑 이미지를 링크한다.
	m_pBottomImage = (cStatic*)GetWindowForID(NI_BOTTOMIMG) ;						// 버텀 이미지를 링크한다.

	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&mScreenRect,
		&value,
		&value);
}

void cNpcImageDlg::LoadingImage(WORD idx)											// 이미지를 로드하는 함수.
{
	CMHFile file;																	// 묵향 파일을 선언한다.

	file.Init("Data/Interface/Windows/NpcImageList.bin","rb");						// npc 이미지 리스트를 읽기모드로 연다.

	if(file.IsInited() == FALSE)													// 열기에 실패하면,
	{
		return;																		// 리턴 처리를 한다.
	}

	SCRIPTMGR->GetImage( 12, &mImageBack, PFT_HARDPATH );							// 뒷배경 상하 이미지를 받는다.

	VECTOR2 vDisp1, vDisp2, vDisp3 ;												// 위치 벡터를 선언한다.

	vDisp1.x = file.GetFloat() ;													// 위치 벡터 1의 정보를 읽어들인다.
	vDisp1.y = file.GetFloat() ;

	vDisp2.x = file.GetFloat() ;													// 위치 벡터 2의 정보를 읽어들인다.
	vDisp2.y = file.GetFloat() ;

	vDisp3.x = file.GetFloat() ;													// 위치 벡터 3의 정보를 읽어들인다.
	vDisp3.y = file.GetFloat() ;

	BACK_POS.y = (float)mScreenRect.bottom - mImageBack.GetImageRect()->bottom;
	BACK_SCALE.x = (float)mScreenRect.right / 2;
	BACK_SCALE.y = 1.0f;

	switch(mScreenRect.right)
	{
	case 800:
		{
			POS.x = vDisp1.x;
			POS.y = vDisp1.y;
		}
		break;
	case 1024:
		{
			POS.x = vDisp2.x;
			POS.y = vDisp2.y;
		}
		break;
	case 1280:
		{
			POS.x = vDisp3.x;
			POS.y = vDisp3.y;
		}
		break ;
	default:
		{
			const SIZE standardResolution = {1024, 768};
			const float ratioWidth = float(mScreenRect.right) / standardResolution.cx;
			const float ratioHeight = float(mScreenRect.bottom) / standardResolution.cy;
			POS.x = vDisp2.x * ratioWidth;
			POS.y = vDisp2.y * ratioHeight;
		}
		break;
	}

	char buf[256] = {0};
	ZeroMemory(
		&buf,
		sizeof(buf));
	ZeroMemory(
		&mpCurImage,
		sizeof(mpCurImage));

	while(FALSE == file.IsEOF())
	{
		WORD wIndex  = file.GetWord();
		BYTE emotion = file.GetByte();
		char*	szFileName	= file.GetString();

		if( wIndex == idx )															// 임시 인덱스와 들어온 인덱스가 같다면,
		{
			if( strlen( szFileName) == 0) break;									// 파일명이 0과 같다면, while문을 탈출한다.

			sprintf( buf, "%s%s", "./data/interface/2dimage/npciamge/", szFileName);// 임시 버퍼에 경로및, 파일명을 세팅한다.

			mpCurImage.idx = idx;													// 현재 이미지의 인덱스를 세팅한다.
			mpCurImage.image[ emotion ].LoadSprite( buf );							// 현재 이미지의 모션에 따른 스프라이트를 로드한다.
		}
	}
}

void cNpcImageDlg::SetActive( BOOL val )											// 활성, 비활성화 함수.
{
	if( val )																		// val가 TRUE와 같다면,
	{
		if( IsActive() ) return ;													// 활성화 상태라면,

		//WINDOWMGR->m_pActivedWindowList->RemoveAll() ;								// 활성화 된 모든 창을 비운다.
		//WINDOWMGR->CloseAllWindow();												// 모든 윈도우를 닫는다.

		//090119 pdy Window Hide Mode
		WINDOWMGR->HideAllWindow();													// 모든 윈도우를 숨긴다.
	}
	else																			// val이 FALSE와 같다면,
	{									
		//WINDOWMGR->ShowAllActivedWindow();										// 모든 활성화 윈도우를 보여준다.

		//090119 pdy Window Hide Mode
		WINDOWMGR->UnHideAllActivedWindow();										// 숨긴위도우를 다시 보여준다.

		WINDOWMGR->SetOpendAllWindows( TRUE ) ;										// 모든 활성화 윈도우를 연다.

		WINDOWMGR->ShowBaseWindow() ;												// 기본 윈도우를 보여준다.
	}
	
	cDialog::SetActive(val);														// 다이얼로그의 활성화 여부를 세팅한다.
}

void cNpcImageDlg::SetNpc( WORD NpcUniqueIdx )										// Npc정보를 세팅하는 함수.
{
	mNpcIdx = NpcUniqueIdx;															// Npc인덱스를 세팅한다.

	LoadingImage(mNpcIdx) ;															// 이미지를 로드한다.
}

void cNpcImageDlg::SetEmotion(BYTE emotion)
{
	mEmotion = emotion;

	int nWidth  = mpCurImage.image[ mEmotion ].GetImageRect()->right;
	int nHeight = mpCurImage.image[ mEmotion ].GetImageRect()->bottom;

	POS.x = (float)(mScreenRect.right - nWidth);
	POS.y = (float)(mScreenRect.bottom - nHeight);
}

void cNpcImageDlg::Render()
{
	if( FALSE == IsActive() )
	{
		return;
	}

	mImageBack.RenderSprite(
		&BACK_SCALE,
		NULL,
		0,
		&BACK_POS,
		COLOR);	
	mpCurImage.image[mEmotion].RenderSprite(
		NULL,
		NULL,
		0,
		&POS,
		COLOR2);
}