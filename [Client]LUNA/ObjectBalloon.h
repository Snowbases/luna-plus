#pragma once


#include "INTERFACE\cWindow.h"
// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.09.20
#include "../hseos/Common/SHProgressBar.h"
// E 농장시스템 추가 added by hseos 2007.09.20

enum 
{
	ObjectBalloon_Name	= 1,
	ObjectBalloon_Chat	= 2,
	ObjectBalloon_Title = 4,
	ObjectBalloon_MunpaMark = 8,
};

class COBalloonName;
class COBalloonChat;
class CStreetStallTitleTip;
class CGuildMark;
class CFamilyMark;

class CObjectBalloon  : public cWindow  
{
public:
	CObjectBalloon();
	virtual ~CObjectBalloon();
	void InitBalloon(CObject * pParent, LONG tall, BYTE flag);
	void Release();
	void Render();
	COBalloonName * GetOBalloonName()			{ return m_pName; }
	COBalloonChat * GetOBalloonChat()			{ return m_pChat; }
	CStreetStallTitleTip* GetSSTitleTip()		{ return m_pSSTitle; }
	CStreetStallTitleTip* GetSBSTitleTip()		{ return m_pSBSTitle; }
	CGuildMark* GetGuildMark()					{ return m_pGuildMark;	}

	void SetNameColor(DWORD color)			{ m_fgColor = color; }
	
	BOOL SetGuildMark( DWORD GuildIdx );
	void SetNickName(char* NickName);
	
	BOOL SetFamilyMark( DWORD FamilyIdx );
	void SetFamilyNickName(char* NickName);
	
	void SetTall(LONG	Tall)				{ m_lTall = Tall;	}

//----
	void SetOverInfoOption( DWORD dwOption );
	void ShowObjectName( BOOL bShow, DWORD dwColor );
	void ShowChatBalloon( BOOL bShow, char* chatMsg, DWORD dwColor, DWORD dwAliveTime );
//---KES 상점검색 2008.3.11
	void ShowStreetStallTitle( BOOL bShow, char* strTitle, DWORD dwFontColor, DWORD dwBGColor );
	void ShowStreetBuyStallTitle( BOOL bShow, char* strTitle, DWORD dwFontColor, DWORD dwBGColor );	
//---------------

	//090116 pdy 클라이언트 최적화 작업 ( Hide 노점Title )
	void HideStreetStallTitle(BOOL bHide);
	void SetGTName(DWORD NameType, char* pGuildName);
	void SetMasterName(char* MasterName);

	virtual void Add( cWindow* );

	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.09.20	2007.10.01
	CSHProgressBar*	GetProgressBar()			{ return &m_csProgressBar; }
	// E 농장시스템 추가 added by hseos 2007.09.20	2007.10.01

	// 071214 LUJ,	오브젝트가 가진 기본 이름 길이는 16이나, 최대 60글자까지 세팅할 수 있다.
	//				그러려면 이 메소드를 호출하면 된다
	void SetObjectName( const char* );

protected:
	CObject *				m_pObject;

	COBalloonName *			m_pName;
	COBalloonChat *			m_pChat;
	CStreetStallTitleTip *	m_pSSTitle;
	CStreetStallTitleTip *	m_pSBSTitle;
	CGuildMark*				m_pGuildMark;
	CFamilyMark*			m_pFamilyMark;

	//	cImage*					m_pNpcMark;

	LONG		m_lTall;
	DWORD		m_fgColor;

	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.09.20	2007.10.01
	CSHProgressBar			m_csProgressBar;
	// E 농장시스템 추가 added by hseos 2007.09.20	2007.10.01

	// 080827 LYW --- ObjectBalloon : 공성중인지 여부를 담을 변수.
	BYTE					m_byIsSiegeWarfare ;
	VECTOR2					m_OldAbsPos;
};
