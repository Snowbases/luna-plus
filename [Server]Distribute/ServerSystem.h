// ServerSystem.h: interface for the CServerSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERSYSTEM_H__FD3EBFC3_EE3D_4505_A5A1_24DA471D20AB__INCLUDED_)
#define AFX_SERVERSYSTEM_H__FD3EBFC3_EE3D_4505_A5A1_24DA471D20AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../4dyuchigxgfunc/global.h"
#include "UserTable.h"

enum eNATION
{
	eNATION_KOREA,
	eNATION_CHINA,
};

class CBillConnector;

class CServerSystem  
{
	DWORD m_DBThreadID;
	const std::auto_ptr< CBillConnector > mBillConnector;
	BOOL m_bUserFireWall;
	char	m_strFireWallIP[16];

//국가
	int		m_Nation;

	ScriptCheckValue m_ScriptCheckValue;

public:
	DWORD m_DBThreadTerminate;
	
	CServerSystem();
	virtual ~CServerSystem();
	CBillConnector& GetBilling() { return *(mBillConnector.get()); }
	void Process();
	void Start(WORD ServerNum);
	void End();		

	DWORD MakeAuthKey();
	void ReleaseAuthKey(DWORD key);
	
	void LoadFireWallIP();
	BOOL IsUsedFireWall() { return m_bUserFireWall;	}
	char* GetFireWallIP() { return m_strFireWallIP;	}

//국가
	void SetNation();
	int GetNation() { return m_Nation; }
	
	void ConnectionCheck();
	void SendConnectionCheckMsg(USERINFO* pInfo);
	BOOL IsInvalidCharInclude( char* pStr );

	const ScriptCheckValue* GetScriptCheckValue() {return &m_ScriptCheckValue;}
};

extern CServerSystem * g_pServerSystem;
void __stdcall OnConnectServerSuccess(DWORD dwIndex, void* pVoid);
void __stdcall OnConnectServerFail(void* pVoid);

#endif // !defined(AFX_SERVERSYSTEM_H__FD3EBFC3_EE3D_4505_A5A1_24DA471D20AB__INCLUDED_)
