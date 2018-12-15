#pragma once

#include "ImageComboBox.h"
#include "HtmlViewNotice.h"

class CMainDialog : public CDialog
{
public:
	CMainDialog(CWnd* pParent = NULL);
	virtual ~CMainDialog();

	// Dialog Data
#ifdef _FOR_SERVER_
	enum { IDD = IDD_MHSERVERPATCH_DIALOG };
#else
	enum { IDD = IDD_MHAUTOPATCH_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected: 
	CHtmlViewNotice* mHtmlViewNotice;
	CImageComboBox mResolutionCombo;
	CImageComboBox mInterfaceCombo;

	// Generated message map functions
	//{{AFX_MSG(CMainDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	virtual void OnCancel();
	virtual void OnOK();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CxImage mImage;
	CxImage m_backgroundImage;
	CxImage m_beginImage;
	CxImage m_closeImage;
	CxImage m_checkBoxImage;
	CxImage mInterfaceArrowImage;
	CxImage mResolutionArrowImage;
	CxImage	mInterfaceTipImage;
	CxImage mResolutionTipImage;
	CxImage mLoadingTipImage;
	CxImage mPackingTipImage;
	
	struct ImageProgress
	{
		CxImage m_cellImage;
		float m_value;	// 0 ~ 1
	}
	m_progress;

	void PutProgress( double ratio, bool isInstant = false );
	void PutBackground();
	void PutTitle();
	void PutTip();

private:
	// 091214 LUJ, 패킹이 스레드로 이뤄져서 사양이 낮은 컴퓨터인 경우 파일을 삭제하지 못하는 경우가 있다.
	//			패킹으로 인해 이미 열려있어 발생한다. 이를 피하기 위해 런처가 실행될 때 이전의 파일들을
	//			지우도록 했다
	void DeleteOldFile();
	void UpdateResolutionCombo();
	void UpdateInterfaceCombo();
	bool mIsReady;

public:	
	// 080306 LUJ, ESC키를 눌러 프로그램을 종료하거나, 엔터키를 눌러 패치 중에 게임을 실행할 수 있어 이를 막는다
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnSelchangeInterface();
	afx_msg void OnCbnSelchangeScreensize();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// 080515 LUJ, 다운로드 후 파일 처리
public:
	// 080515 LUJ, 버전 파일을 체크한다
	static void CheckVersionToMemory( CZipMemFile& );
	// 080515 LUJ, 버전 파일을 체크한다
	static void CheckVersionToDisk( const TCHAR* );
	// 080812 LUJ, 패치 파일을 받았는지 체크한다
	static void CheckPatchToMemory( CZipMemFile& );
	// 080812 LUJ, 패치 파일을 받았는지 체크한다
	static void CheckPatchToDisk( const TCHAR* );
	// 080515 LUJ, 다운로드받은 패치 파일의 압축을 해제한다
	static void ExtractToMemory( CZipMemFile& );
	// 080515 LUJ, 다운로드받은 패치 파일의 압축을 해제한다
	static void ExtractToDisk( const TCHAR* );
	// 090819 ONS 퍼플런처UI에 전송속도와 남은시간 출력.
	// 080515 LUJ, 경과를 표시한다
	static void Pursuit( double percent, double remaintime, double down_speed );
	// 080515 LUJ, 파일을 패킹한다
	static UINT Pack( LPVOID );	
	// 080814 LUJ, 런처를 준비 상태로 만든다
	static void Ready();
private:
	// 081202 LUJ, 패치 버전을 체크한다
	static void CheckVersion( CString serverVersion, CString clientVersion );
	// 090720 ShinJS --- 버전정보를 저장하고 다운로드한 버전파일을 제거한다
	static void SaveVersion();
private:
	CString mPatchVersion;

	// 090819 ONS 퍼플런쳐 윈도우 핸들
	HWND m_hTarget;
	// 090819 ONS 퍼플런처로 UI로 데이터를 전송하는 함수
	void SendMessageToPurple(int nType, LPCTSTR lpszMsg, DWORD dwValue);
};