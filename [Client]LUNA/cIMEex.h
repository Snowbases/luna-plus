#ifndef _CIMEEX_H
#define _CIMEEX_H


//=====================================================================
// Since : 2003.8.30
// Kang Eun Sung
// Last Updated : 2003.9.15
//=====================================================================

#define MAX_LINE	256


enum eEndKind	//sLineDesc ÀÇ nEndKind Á¾·ù
{
	EK_END,
	EK_ENTER,
	EK_WRAP,	
};


struct sLineDesc
{
	int nStartIndex;
	int nLen;
	int nEndKind;
	//¹®ÀÚ¿­ÀÇ ½ÇÁ¦ ±æÀÌ¸¦ ³ÖÀÚ
};


struct sInsertPos
{
	int nLine;
	int nIndex;
};


enum eValidCheckMehtod
{
	VCM_DEFAULT,
	VCM_NUMBER,
	VCM_CHARNAME,
	VCM_ID,
	VCM_PASSWORD,
	VCM_DBPARAM,
	VCM_SPACE,

	// 080403 LYW --- cIMEex : Add a enum type to use number without comma.
	VCM_NORMAL_NUMBER,
};


enum eIMEEVENT
{
	eIMEE_ENTER,
	eIMEE_SETFOCUS,
	eIMEE_KILLFOCUS,
//	eIMEE_INSERTTEXT,
};


/////////////////////////////
// Ãß°¡ µÉ °Í
// 1. ¸¶¿ì½º·Î Ä³·µ Æ÷Áö¼Ç ¹Ù²Ù±â
// 2. ºí·° Áö¿ì±â, º¹»ç, ºÙÀÌ±â
// 3. PageUp, Down
// 4. Insert - Overide toggle

class cEditBox;

class cIMEex
{

protected:

	WORD			m_wFontIdx;

	char*			m_pStrBuf;
	sLineDesc		m_pLineDesc[MAX_LINE];
	sInsertPos		m_InsertPos;
	int				m_nCurTotalLen;
	int				m_nCurTotalLine;
	int				m_nMaxTotalLen;
	int				m_nMaxTotalLine;
	int				m_nLimitWidth;
	BOOL			m_bCompositing;
	BOOL			m_bMultiLine;

	BOOL			m_bChanged;
	
	cEditBox*		m_pParentEdit;

	BOOL			m_bEnterAllow;

//--- For Invalid Check.
	int				m_nValidCheckMethod;
	
	// 091126 ShinJS --- CheckMethod 에서 사용하는 기본문자를 설정한다.
	char			m_CheckMethodBaseStr[ MAX_LINE ];

	// 100607 ONS 첫입력인지 여부를 체크한다.
	BOOL 			m_bIsFirstInput;

protected:

	int		GetInsertIndex()	{ return m_pLineDesc[m_InsertPos.nLine].nStartIndex + m_InsertPos.nIndex; }
	char*	GetInsertBufPtr()	{ return m_pStrBuf + GetInsertIndex(); }

public:

	cIMEex();
	virtual ~cIMEex();

	void Init( int nMaxBufSize = 256, int nPixelWidth = 1024, BOOL bMultiLine = FALSE, int nMaxLine = MAX_LINE );
	void Release();

//Input String
	BOOL Insert( char* str );
	BOOL Paste( char* str );

	BOOL Enter();
	BOOL Wrap( int nStartLine );		//protected?


//Get String Func.
	BOOL GetLineText( int nLine, char* strText, int* pStrLen = NULL );
	sInsertPos* GetInsertPos() { return &m_InsertPos; }
	DWORD GetBufferSize() const { return m_nMaxTotalLen; }
	BOOL CaretMoveLeft();
	BOOL CaretMoveRight();
	BOOL CaretMoveUp();
	BOOL CaretMoveDown();
	BOOL CaretMoveHome();
	BOOL CaretMoveEnd();
	BOOL CaretMoveFirst();

//Delete Func.
	BOOL DeleteAfter();		//Delete key
	BOOL DeleteBefore();	//Back Space key
	BOOL DeleteAllText();
//	BOOL DeleteLine();

//Set Input Help Func.
	void SetCompositing( BOOL bCompositing )	{ m_bCompositing = bCompositing; }

//Set Attr Func.
	void SetFont( WORD wFontIdx ) { m_wFontIdx = wFontIdx; }
	
//Get Attr Func.
	int GetTotalLine()	{ return m_nCurTotalLine; }
	int GetValidCheckMethod() { return m_nValidCheckMethod; }
//For Script
	void SetScriptText( const char* str );
	void GetScriptText( char* str );

//For Invalid Check.
	void SetValidCheckMethod( int nSetNum );
	BOOL IsValidChar( unsigned char* str );

	BOOL IsChanged( BOOL bInit = TRUE )
	{
		BOOL bResult = m_bChanged;
		if( bInit ) m_bChanged = FALSE;
		return bResult;
	}

	void CheckAfterChange();

//For Limit
	BOOL SetLimitLine( int nMaxLine );
	void SetEnterAllow( BOOL bAllow ) { m_bEnterAllow = bAllow; }

//Set Parent for only Edit	
	void SetParentEdit( cEditBox* pEdit )		{ m_pParentEdit = pEdit; }
	cEditBox* GetParentEdit()					{ return m_pParentEdit; }

	// 091126 ShinJS --- CheckMethod 에서 사용하는 기본문자를 설정한다.
	void SetCheckMethodBaseStr( const char* pStr ) { SafeStrCpy( m_CheckMethodBaseStr, pStr, MAX_LINE ); }

	void SetFirstInput( BOOL bVal ) { m_bIsFirstInput = bVal; }
};




#endif
