#pragma once

#ifdef _NPROTECT_

#define NPROTECTMGR	USINGTON(CNProtectManager)

class CNPGameLib;
class cTimeDelay;

class CNProtectManager
{
	char m_szHackMsg[256];
	cTimeDelay* mpCheckDelay;

public:
	CNProtectManager(void);
	~CNProtectManager(void);

	char* GetMsg();

	void NetworkMsgParse(BYTE Protocol, void* pMsg);
	BOOL MsgProc(DWORD dwMsg, DWORD dwArg);

	BOOL Init(HWND hWnd);
	void Release();

	void Check();
	void UserIDSend( char* userID );
};

EXTERNGLOBALTON(CNProtectManager);
extern CNPGameLib* g_pNPGame;

#endif