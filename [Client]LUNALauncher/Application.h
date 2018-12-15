#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


/////////////////////////////////////////////////////////////////////////////
// CApplication:
// See MHAutoPatch.cpp for the implementation of this class
//
// 081202 LUJ, 다운로드를 메모리 상의 파일로 받는다
class CZipMemFile;

class CApplication : public CWinApp
{
protected:
	HANDLE	m_hMap;

public:
	CApplication();
	virtual ~CApplication();

	void ReleaseHandle() { if( m_hMap ) CloseHandle(m_hMap); }
	// 080625 LUJ, 네트워크 DLL을 해제한다
	void ReleaseNetwork() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApplication)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CApplication)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	struct Position
	{
		CRect mRect;
		CPoint mPoint;
	};
	struct Config
	{
		CPoint mResolutionCombo;
		CPoint mResolutionArrow;
		CPoint mInterfaceCombo;
		CPoint mInterfaceArrow;
		// 080507 LUJ, 이미지 경로 선택 컨트롤 위치
		typedef std::list< CString > TextList;
		TextList mInterfaceTextList;

		bool m_isWindowMode;

		CString mImage;
		Position mHtmlPosition;
		Position mBackgroundPosition;
		Position mCloseUpPosition;
		Position mCloseDownPosition;
		Position mBeginUpPosition;
		Position mBeginDownPosition;
		Position mCheckedPosition;
		Position mUncheckedPosition;
		Position mWindowModePosition;
		Position mProgressPosition;
		Position mProgressCellPosition;
		Position mProgressCellPackPosition;
		Position mTitleDecoratorCellPosition;
		Position mTitleTextPosition;
		Position mArrowPosition;
		Position mArrowPushedPosition;
		Position mTopCombo;
		Position mTopOverCombo;
		Position mMiddleCombo;
		Position mMiddleOverCombo;
		Position mBottomCombo;
		Position mBottomOverCombo;
		Position mCoverCombo;
		
		struct Font
		{
			RGBQUAD mColor;
			int mWeight;
			int mSize;
			RGBQUAD mBorderColor;
			int mBorderSize;
		};
		Font mControlFont;
		Font mTitleFont;
		Font mWindowModeFont;

		struct Tip
		{
			CString mText;
			CPoint mTextOffset;
			Position mPosition;
			Font mFont;
		};
		Tip mInterfaceTip;
		Tip mResolutionTip;
		Tip mLoadingTip;
		Tip mPackingTip;
		
		// 080507 LUJ, 선택된 이미지 경로 파일 이름
		CString m_SelectedInterface;
		CString m_textCaption;
		CString m_textTitle;
		CString m_textWindowMode;
		CString m_textCheck;
		CString m_textPack;
		CString m_textBegin;
		CString m_textDownload;
		CString m_textExtract;

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

		// 090819 ONS 퍼플런처관련 메세지 추가
		CString m_messageBoxPrupleInvalidArgument;
		CString m_messageBoxPrupleInvalidHandle;
		CString m_fontName;
		
		// 081202 LUJ, 다운로드받은 데이터를 어디에 저장할지 결정한다
		enum FileType
		{
			FileTypeNone,
			FileTypeDisk,
			FileTypeMemory,
		}
		mFileType;
		CSize mResolution;

		Config() :
		m_isWindowMode(false),
		mFileType(FileTypeDisk)
		{
			ZeroMemory(
				&mControlFont,
				sizeof(mControlFont));
			ZeroMemory(
				&mTitleFont,
				sizeof(mTitleFont));
		}
	};

	// 080515 LUJ, 설정 사항을 반환한다
	Config& GetConfig();
	
private:
	Config mConfig;
	
	// 080515 LUJ, 서비스 지역 처리
public:
	struct Local
	{
		enum Type
		{
			TypeNone,
			TypeKorea,
			TypeKoreaTest,
			TypeKoreaPlus,
			TypeThai,
			TypeTaiwan,
			TypeTaiwanTest,
			TypeSingapore,
			TypeMalaysia,
			TypeMalaysiaTestServer,
			TypeUSA,
			TypeUSATest,
			TypeChina,
			TypeChinaTest,
			TypeChinaISBN,
			TypeHongKongTest,
			TypeHongKongMain,
			TypePhilippinesTest,
			TypePhilippinesMain,
			// 090819 ONS 일본 로컬 추가.
			TypeJapanTest,
			TypeJapanMain,
		}
		mType;

		struct Download
		{
			CString mPath;
			CString mVersionTag;
			CString mVersionFile;
		}
		mDownload;

		struct URL
		{
			CString mJoin;
			CString mHomepage;
			CString mContent;
		}
		mURL;
	};

	// 080515 LUJ, 서비스 지역에 따른 정보를 반환한다
	const Local& GetLocal() const;

private:
	// 080515 LUJ, 서비스 지역을 설정한다
	void SetLocal( Local::Type );

	Local mLocal;

	// 080515 LUJ, 다운로드 처리
public:
	// 080515 LUJ, 결과 및 진행 처리를 위한 콜백 함수 포인터 선언
	typedef void ( *DiskCallBackFunction )		( const TCHAR* );
	typedef void ( *MemoryCallBackFunction )	( CZipMemFile& );
	// 090819 ONS 퍼플런처UI에 전송속도와 남은시간을 전달하기위해 수정
	typedef void ( *ProgressCallBackFunction )	( double percent, double remaintime, double down_speed );
	// 080515 LUJ, 파일을 다운로드받고 완료 후 콜백 함수에 결과를 전달한다
	// 081201 LUJ, 다운로드 실패 시 처리할 콜백 함수를 지정한다
	// 081202 LUJ, 다운로드받은 데이터를 메모리에 쓸지 결정하는 플래그 추가
	void Download( const TCHAR* path, const TCHAR* file, DiskCallBackFunction, MemoryCallBackFunction, ProgressCallBackFunction, CApplication::Config::FileType );	

private:
	struct DownloadVariable
	{
		// 081201 LUJ, 데이터를 어떤 형태로 받을지 결정하는 플래그
		enum Type
		{
			TypeNone,
			TypeDisk,
			TypeMemory,
		}
		mType;
		CString						mPath;
		CString						mFile;
		DiskCallBackFunction		mDiskCallBack;
		MemoryCallBackFunction		mMemoryCallBack;
		ProgressCallBackFunction	mProgressCallBack;

		DownloadVariable() :
			mDiskCallBack( 0 ),
			mMemoryCallBack( 0 ),
			mProgressCallBack( 0 )
		{}
	};

	// 080515 LUJ, 힙에 할당된 다운로드 변수 컨테이너
	typedef CList< DownloadVariable*, DownloadVariable*& >	DownloadVariableList;
	DownloadVariableList									mDownloadVariableList;

	// 080515 LUJ, 파일을 다운로드받고 완료 후 콜백 함수에 결과를 전달한다. 스레드로 실행되어야 한다
	static UINT Download( LPVOID );

#ifdef _LOCAL_JP_
	struct PurpleInfo
	{
		HWND		m_hTarget;
		CString		m_LauncherPath;
		CString		m_GameInstallPath;
		CString		m_ETCPath;
	}
	m_Purple;

public:
	HWND			GetTargetHandle() { return m_Purple.m_hTarget; }
	// 090819 ONS 퍼플런처로부터 전달된 서버주소를 설정
	void SetLocalEx();

#endif
};

extern CApplication theApp;
