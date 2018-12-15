// ServerSet.h: interface for the CServerSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERSET_H__152BB200_A677_440D_A31F_DDDB7EB7DB68__INCLUDED_)
#define AFX_SERVERSET_H__152BB200_A677_440D_A31F_DDDB7EB7DB68__INCLUDED_

#include "Server.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;

class CServerSet  
{
public:
	TCHAR		m_sMASIP[16];
	int			m_nMASPort;
	
	TCHAR		m_sServerInfoFileName[256];
	TCHAR		m_sClientExeVersion[256];

	// Set
	int			m_nServerSet;						// == Index
	TCHAR		m_sServerSetName[256];
	
	vector<CServer*>	m_vecServer;

	DWORD		m_dwTotalUserCount;
	int			m_nUserLevel;						// userlevel
	TCHAR		m_sUserLevel[256];

	BOOL		m_bInit;
	
	DWORD		m_dwMsgCheckTime;
	
	DWORD		m_dwMaxUser;

	vector<CServer*>	m_vecMSServer;

public:
	CServerSet();
	virtual ~CServerSet();

	BOOL		Init(int ServerSetNum, char* setName, char* setFileName);
	BOOL		InitFromFile( char* setFileName);	
	void		Release();
	
	CServer*	GetServer( WORD serverkind, WORD servernum  );
	CServer*	GetServer( SERVERSTATE* pState );
	CServer*	GetMSServer( MSSTATE* pState );
	CServer*	GetServer( LPCTSTR pServerIp, WORD wServerPort );

	void		SetUserLevel( int userlevel );
	void		ReadClientExeVersion();
	void		WriteClientExeVersion( char* version );
	void		CreateUserCountFile();
	void		WriteUserCount( SYSTEMTIME time );
	BOOL		IsThereOnServer();
	BOOL		IsThereAbnormalServer();
	BOOL		IsThereAbnormalMS();
	DWORD		GetTotalUserCount();

	BOOL		CheckDisconnectUser();	
	
	void		ClearMSStateAll( int nState );
	void		CheckMSState();

public:	// operate func
	void	DoServerSetOn();
	void	DoServerSetOff();
	void	DoServerSetRefresh();
	void	DoServerOn( WORD serverkind, WORD servernum );
	void	DoServerOff( WORD serverkind, WORD servernum );
	
	void	DoQueryClientExeVersion();
	void	DoChangeClientExeVersion( LPCTSTR version );
	void	DoQueryMaxUser();
	void	DoChangeMaxUser( DWORD maxuser );
	void	DoQueryUserLevel();
	void	DoChangeUserLevel( int userlevel );
	
	void	DoQueryUserCount();
	void	DoSendMessage( char* pmsg );
	void	DoSendEventMessage( LPCTSTR pTitle, LPCTSTR pMsg );
	void	DoSendEventMessageOff();

	void	DoAutoPatch();
	void	DoAssertMsgBox( BOOL bState );		
	void	DoAllUserDisconnect();	
	void	DoServerSetOffAfterUserDiscon();	

	void	DoMSStateAll();	
	void	DoQueryAutoPatchVersion();
	
	void	DoQueryChannelInfo();
};

#endif // !defined(AFX_SERVERSET_H__152BB200_A677_440D_A31F_DDDB7EB7DB68__INCLUDED_)
