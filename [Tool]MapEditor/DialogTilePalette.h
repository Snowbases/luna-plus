#pragma once

class CDialogEditEntry;
class CTileView;
class CDialogTileSet;

class CDialogTilePalette : public CDialog
{	
	I4DyuchiGXRenderer*		m_pRenderer;
	DWORD					m_dwNumX;
	DWORD					m_dwNumY;
	float					m_fTileWidth;
	float					m_fTileHeight;	

	DWORD					m_dwTextureTableNum;
	TEXTURE_TABLE*			m_pTextureTable;

	IDISpriteObject**		m_ppSpriteTable;
	IDISpriteObject*		m_pOutLineSprite;

	WORD					m_wSelectedTileIndex;
	char					m_szFileName[MAX_NAME_LEN];
	RECT					m_rcClip;
	int						m_iOffsetX;
	int						m_iOffsetY;
	DWORD					m_dwAlphaBrushActionFlag;
	CTileView*				m_pTileView;
	CDialogEditEntry*		m_pEditEntryDlg;
	CDialogTileSet*			m_pTileSetDlg;
	INT						m_iScrollRange;

	VOID					DisplaySelectedTileName(WORD wTileIndex);

	//



public:
	BOOL					LoadPalette(char* szFileName);
	BOOL					CreatePalette(DWORD dwNum);
	BOOL					SetPalette(TEXTURE_TABLE* pTable,DWORD dwNum);
	void					SetTile(DWORD dwTileIndex);

	void					CleanTextureTable();
	void					RenderPalette();
	
//	void					RenderTileView(WORD* pwIndexedTileTable,DWORD dwNumX,DWORD dwNumY,int iOffsetX,int iOffsetY,RECT* pRectClip);
	
	//BOOL					Initialize(DWORD dwTileSize,CDialogEditEntry* pEditEntryDlg);
	//trustpak
	BOOL					Initialize(DWORD dwTileSize,CDialogEditEntry* pEditEntryDlg, CDialogTileSet* pTileSetDlg);
	///
	BOOL					GetTextureTable(TEXTURE_TABLE** ppTable,DWORD* pdwNum);
	char*					GetFileName() {return m_szFileName;}
	void					SelectTile(POINT* pPoint);

	WORD					GetCurrentTileIndex() {return m_wSelectedTileIndex;}
	DWORD					GetAlphaBrushActionFlag();

	IDISpriteObject*		GetSprite(DWORD dwIndex) {return m_ppSpriteTable[dwIndex];}


	CDialogTilePalette(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogTilePalette();
	void					EnableView();
	void					DisableView();
	void					DisplayCurrentTilePalleteFileName(void);
	void UpdateTile(LPCTSTR, DWORD index);

// Dialog Data
	//{{AFX_DATA(CDialogTilePalette)
	enum { IDD = IDD_DIALOG1 };
	CStatic	m_wndSelectTileName;
	CScrollBar	m_scrBarHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogTilePalette)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogTilePalette)
	afx_msg void OnButtonRefresh();
	afx_msg void OnButtonLoadPalette();
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnButtonFillSelectedTile();
	afx_msg void OnButtonReplaceTexture();
	afx_msg void OnButtonRotateTexture();
	afx_msg void OnRadioAlphaBrushAdd();
	afx_msg void OnRadioAlphaBrushSub();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};