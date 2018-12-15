#ifndef __STREETSTALL_TITLETIP__
#define __STREETSTALL_TITLETIP__



#define SS_TITLE_LINE 2
#define SS_TITLE_LENGTH 32

class CStreetStallTitleTip
{
public:
	CStreetStallTitleTip();
	virtual ~CStreetStallTitleTip();

	BOOL IsActive()							{ return m_bActive;	}
	BOOL IsShow()							{ return m_bShow;	}
	void SetActive(BOOL val)				{ m_bActive = val;	}
	void Show(BOOL val)						{ m_bShow	= val;	}
	BOOL Render(LONG absX, LONG absY, int type );
	void SetTall(LONG	Tall)				{ m_lTall = Tall;	}
	void SetPosX(LONG	pos)				{ m_lPosX = pos;	}
	void SetTitle(char * chatMsg);

	void SetFGColor(DWORD col)				{ m_fgColor = col;	}

//---KES 상점검색 2008.3.11
	void SetBGColor(DWORD col)				{ m_bgColor = col;	}
	char* GetTitle()						{ return m_sztitle; }
	int GetLineCount() const { return m_nLineNum; }

protected:
	BOOL m_bActive;
	BOOL m_bShow;
	LONG m_lTall;
	LONG m_lPosX;
	char m_sztitle[MAX_STREETSTALL_TITLELEN+1];
	int	 m_TitleLen;
	WORD m_wFontIdx;
	DWORD m_fgColor;

//---KES 상점검색 2008.3.11
	DWORD m_bgColor;
//--------------

	int m_nLineNum;
	int m_nWidth;
	char m_szChatMsg[SS_TITLE_LINE][SS_TITLE_LENGTH+1];
	int	 m_ChatMsgLen[SS_TITLE_LINE];
};

#endif //__STREETSTALL_TITLETIP__
