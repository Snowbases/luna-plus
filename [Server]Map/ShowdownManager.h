#ifndef _SHOWDOWNMANAGER_H
#define _SHOWDOWNMANAGER_H


#define SHOWDOWNMGR CShowdownManager::GetInstance()


class CShowdownManager
{
protected:

public:

	MAKESINGLETON( CShowdownManager );	

	CShowdownManager();
	virtual ~CShowdownManager();

	void UserLogOut( CPlayer* pPlayer );
	BOOL CanApplyShowdown( CPlayer* pApplyer, CPlayer* pAccepter );
	BOOL CanAcceptShowdown( CPlayer* pApplyer, CPlayer* pAccepter );

	void NetworkMsgParse(BYTE Protocol,void* pMsg);
	
};


#endif