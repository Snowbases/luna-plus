// common.h : main header file for the MHAUTOPATCH application
//

#if !defined(AFX_MHAUTOPATCH_H__3FF53E45_BF6A_4397_9854_E3217D68EA2B__INCLUDED_)
#define AFX_MHAUTOPATCH_H__3FF53E45_BF6A_4397_9854_E3217D68EA2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


/////////////////////////////////////////////////////////////////////////////
// CApplication:
// See MHAutoPatch.cpp for the implementation of this class
//

#define URL_LENGTH	256

class CApplication : public CWinApp
{
protected:
	HANDLE	m_hMap;

public:
	CApplication();
	virtual ~CApplication();

	void ReleaseHandle() { if( m_hMap ) CloseHandle(m_hMap); }


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApplication)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	BOOL SaveFileList();
	BOOL GetVersionInfoFile( char* lpszVerInfoFile, char* lpszVerInfo );

	BOOL InitFTP();
	void UnInitFTP();
	
	//{{AFX_MSG(CApplication)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	struct Config
	{
		CPoint m_title;
		CPoint m_windowMode;

		CRect m_html;
		CRect m_status;
		CRect m_homePage;
		CRect m_register;
		CRect m_checkBox;
		CRect m_begin;
		CRect m_close;
		CRect m_progress;
		CRect m_resolution;
		// 080507 LUJ, 이미지 경로 선택 컨트롤 위치
		CRect m_interface;

		CStringList m_ftp;
		
		bool m_isWindowMode;		
		long m_SelectedResolution;	// 0: 800x600, 1: 1024x768, 2: 1280x1024
		// 080507 LUJ, 선택된 이미지 경로 파일 이름
		CString m_SelectedInterface;

		CString m_imagePath;
		CString m_imageBackground;
		CString m_imageCloseUp;
		CString m_imageCloseDown;
		CString m_imageProgressBar;
		CString m_imageProgressCell;
		CString m_imageHomepageUp;
		CString m_imageHomepageDown;
		CString m_imageRegisterUp;
		CString m_imageRegisterDown;
		CString m_imageCombo1;
		CString m_imageCombo2;
		CString m_imageCombo3;
		CString m_imageCombo1Selected;
		CString m_imageCombo2Selected;
		CString m_imageCombo3Selected;
		// 080507 LUJ, 해상도 변경 콤보 커버
		CString m_ImageComboCover;
		CString m_imageCheckBoxChecked;
		CString m_imageCheckBoxUnchecked;
		CString m_imageBeginUp;
		CString m_imageBeginDown;
		// 080507 LUJ, 이미지 경로 선택용 콤보 배경
		CString m_imageInterfaceCombo1;
		CString m_ImageInterfaceCombo2;
		CString m_ImageInterfaceCombo3;
		CString m_ImageInterfaceCombo1Selected;
		CString m_ImageInterfaceCombo2Selected;
		CString m_ImageInterfaceCombo3Selected;
		CString m_ImageInterfaceComboCover;

		CString m_textCaption;
		CString m_textTitle;
		CString m_textWindowMode;
		CString m_textCheck;
		CString m_textPack;
		CString m_textBegin;
		CString m_textDownload;
		CString m_textExtract;

		bool m_flagShowTitle;
		bool m_flagProgressCellType;

		CString m_messageBoxCaption;
		CString m_messageBoxSelfUpdateFailed;
		CString m_messageBoxNotFoundVersionFile;
		CString m_messageBoxIncorrectVersionFile;
		CString m_messageBoxFailedVersionFileDelete;
		CString m_messageBoxFailedVersionFileRename;
		CString m_messageBoxFailedCreateExecutive;
		CString m_messageBoxFailedRunClient;
		CString m_messageBoxFailedExitDuringPacking;
		CString m_messageBoxFailedConnect;
		CString m_messageBoxInvalidateVersion;
		CString m_messageBoxFailedVersionFileOpen;
		CString m_messageBoxFailedFileDownload;
		CString m_messageBoxFailedExtract;

		CString m_fontName;
		long m_fontSize;
		RGBQUAD m_fontStatusColor;
		RGBQUAD m_fontControlColor;
		RGBQUAD m_fontTitleColor;
	}
	m_config;

	void SaveOption( const Config& );

	static CApplication* GetInstance();

	CZipArchive m_zipArchive;

private:
	static CApplication* m_self;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MHAUTOPATCH_H__3FF53E45_BF6A_4397_9854_E3217D68EA2B__INCLUDED_)
