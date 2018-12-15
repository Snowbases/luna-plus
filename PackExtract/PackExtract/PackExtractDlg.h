// PackExtractDlg.h : 헤더 파일
//

#pragma once

#include "../../4DyuchiGRX_common/IFileStorage.h"
#include "afxwin.h"
#include "afxcmn.h"

#define UM_UPDATEDATA	WM_USER+1

enum ePackMode{
	ePackMode_Extract,
	ePackMode_Compress,
};

class CPackExtractDlg;

UINT CallExtractPackFile( LPVOID pParam );
UINT CallCompressPackFile( LPVOID pParam );
DWORD __stdcall CallUnlockFileProc( DWORD dwCurCount,DWORD dwTotalCount,void* pArg );

// CPackExtractDlg 대화 상자
class CPackExtractDlg : public CDialog
{
	I4DyuchiFileStorage*	m_pStorage;

	ePackMode				m_eMode;
	vector<CString>			m_vPackFile;
	vector<CString>			m_vCompressFolder;

	CString					m_strSelectedPackFileName;
	vector<CString>			m_vSelectedPackFileList;
	void*					m_pSelectedPackFileHandle;
	
	char					m_szBaseDirectory[ MAX_PATH ];					// 프로그램 실행 폴더
	char					m_szCurDirectory[ MAX_PATH ];					// 현재 작업 폴더
	DWORD					m_dwUnlockFileProcTotalNum;

	BOOL					m_bIsWorking;

	LPITEMIDLIST			m_pidlDesktop;
	LPSHELLFOLDER			m_pDesktopFolder;
	LPMALLOC				m_pMalloc;
	HTREEITEM				m_hDesktopItem;

	CTreeCtrl				m_ExtractFolderPos;
	CImageList				m_IconImageList;

	CListBox m_PackListBox;
	CProgressCtrl m_ListProgress;
	CProgressCtrl m_PackProgress;
	CButton m_ModeExtractBtn;
	CButton m_ModeCompressBtn;

	void EnumChildItems(LPSHELLFOLDER pFolder, LPITEMIDLIST pParent, HTREEITEM hParent);
	void FillItem(TVINSERTSTRUCT &tvi, LPSHELLFOLDER pFolder, LPITEMIDLIST pidl, LPITEMIDLIST pidlRelative);
	void UTStrretToString(LPITEMIDLIST pidl, LPSTRRET pStr, LPSTR pszBuf);
	void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	LPITEMIDLIST ILGetNextItemID(LPCITEMIDLIST pidl);
	UINT ILGetSize(LPCITEMIDLIST pidl);
	LPITEMIDLIST ILAppend(LPITEMIDLIST pidlParent, LPITEMIDLIST pidlChild);
	
	void Release();
	BOOL GetSelectedFolderByTree( char* pDirName, DWORD dwDirNameLen );
	void LoadFolderFile( LPCTSTR strLoadPath, LPCTSTR strFileFilter, vector<CString>& vecName, BOOL bLoadDirectory, BOOL bFullPath = TRUE );
	void RemoveFileInDir( LPCTSTR strDirPath );
	void RemoveDir( LPCTSTR strDirPath );

	void ExpandTreeItem( HTREEITEM hItem );											// 해당 HTREEITEM을 Expand 한다.
	HTREEITEM GetTreeItem( HTREEITEM hRootItem, LPCTSTR szName, BOOL bExactly );	// hRootItem 하위 Tree에서 이름으로 HTREEITEM을 구한다.
	HTREEITEM GetTreeItem( HTREEITEM hRootItem, int csidl );						// hRootItem 하위 특수 id list의 HTREEITEM을 구한다.
	BOOL UpdateTreePath( CString strPath );											// 넘겨준 Path로 Tree를 설정한다.

// 생성
public:
	CPackExtractDlg(CWnd* pParent = NULL);	// 표준 생성자
	virtual ~CPackExtractDlg();

// 대화 상자 데이터
	enum { IDD = IDD_PACKEXTRACT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExtractPackfileButton();
	afx_msg void OnBnClickedDeletePackfileButton();
	afx_msg void OnTvnItemexpandedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedModeExtractRadio();
	afx_msg void OnBnClickedModeCompressRadio();

	void LoadPackFile();
	void LoadCompressFolder();	

	void ExtractPackFile();
	void ExtractPackFile( char* szExtractFileName );
	void DeletePackFile( char* szExtractFileName );

	void OpenExtractPackFileWithSelectListDlg( CString strSelectedPackFile );
	void AddListFromSelectedPackFile( CListBox& listbox );

	void StartExtractPackFileWithSelectListDlg( BOOL bClearFoloder = FALSE );
	void EndExtractPackFileWithSelectListDlg();
	BOOL StartDeletePackFileWithSelectListDlg();
	void EndDeletePackFileWithSelectListDlg();
	
	void SetPackProgressRange( int nMax )	{ m_PackProgress.SetRange( 0, nMax ); }
	void SetPackProgressPos( int nPos )		{ m_PackProgress.SetPos( nPos ); UpdateData( FALSE ); }

    void CompressPackFile();
	void CompressFile( CString strFolderName );	

	void UnlockFileProc( DWORD dwCurCount, DWORD dwTotalCount, void* pArg );

	afx_msg void OnBnClickedDeleteAfterProcCheck();
	BOOL m_bDeleteSource;

	afx_msg LRESULT OnUpdateDlg( WPARAM wParam, LPARAM lParam );
	CButton m_ExtractFileNameInputBtn;
	afx_msg void OnBnClickedExtractFilenameButton1();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedHelpButton();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLbnDblclkPackList();
};

