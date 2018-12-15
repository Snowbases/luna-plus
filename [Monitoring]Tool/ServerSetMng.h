// ServerSetMng.h: interface for the CServerSetMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERSETMNG_H__9059DEBB_297D_498B_A74A_94846B527852__INCLUDED_)
#define AFX_SERVERSETMNG_H__9059DEBB_297D_498B_A74A_94846B527852__INCLUDED_

#include "ServerSet.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SERVERSETMNG USINGTON(CServerSetMng)

class CServerSetMng  
{	
protected:
	CServerSet	m_ServerSet[MAX_SERVERSET];
	int			m_nMaxServerSet;
	
public:
	DWORD		m_dwTotalUserCount;

public:
	CServerSetMng();
	virtual ~CServerSetMng();

	BOOL	Init( char* filename );
	BOOL	ReadMASList( char* filename );
	void	Release();

	CServerSet*	GetServerSet( int setindex );
	CServerSet*	GetServerSet( LPCTSTR setname );
	int			GetMaxServerSet()					{ return m_nMaxServerSet; }
	DWORD		GetTotalUserCount();
	
	void		WriteClientExeVersion( LPCTSTR setname, LPCTSTR version );
	void		WriteUserCount( SYSTEMTIME time );
	BOOL		IsThereOnServer( LPCTSTR setname );

public:	// server operate func
	void	DoServerSetOn( LPCTSTR setname );
	void	DoServerSetOff( LPCTSTR setname );
	void	DoServerSetRefresh( LPCTSTR setname );

	void	DoQueryClientExeVersion();
	void	DoChangeClientExeVersion( LPCTSTR setname, LPCTSTR version );
	void	DoQueryMaxUser();
	void	DoChangeMaxUser( LPCTSTR setname, DWORD maxuser );
	void	DoQueryUserLevel();
	void	DoChangeUserLevel( LPCTSTR setname, int userlevel );	

	void	DoQueryUserCount();
	void	DoSendMessage( LPCTSTR setname, const char* pmsg );
	void	DoSendEventMessage( LPCTSTR setname, LPCTSTR pTitle, LPCTSTR pMsg );
	void	DoSendEventMessageOff( LPCTSTR setname );

	void	DoAutoPatch( LPCTSTR setname );
	void	DoAssertMsgBox( LPCTSTR setname, BOOL bState );
	void	DoAllUserDisconnect( LPCTSTR setname );
	void	DoServerSetOffAfterUserDiscon( LPCTSTR setname );

	void	DoMSStateAll( LPCTSTR setname );
	void	DoQueryAutoPatchVersion( LPCTSTR setname );

	void	DoQueryChannelInfo( LPCTSTR setname );

	void	CheckAndSendSMS();
};

EXTERNGLOBALTON(CServerSetMng)

#endif // !defined(AFX_SERVERSETMNG_H__9059DEBB_297D_498B_A74A_94846B527852__INCLUDED_)
