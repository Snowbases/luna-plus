#ifndef _SHOWDOWNMANAGER_H
#define _SHOWDOWNMANAGER_H


#define SHOWDOWNMGR USINGTON(CShowdownManager)

class CPlayer;

class CShowdownManager 
{

protected:

	BOOL m_bIsShowdown;

public:

	//MAKESINGLETON(CShowdownManager);

	CShowdownManager();
	virtual ~CShowdownManager();

	void Init();
	void ApplyShowdown();		
	void CancelApply();
	void AcceptShowdown( BOOL bAccept );
	BOOL CanAcceptShowdown( CPlayer* pAccepter );

	void NetworkMsgParse(BYTE Protocol,void* pMsg);
	
	void SetShowdown( BOOL bShowdown );
	BOOL IsShowdown() { return m_bIsShowdown; }

};

EXTERNGLOBALTON(CShowdownManager);
#endif