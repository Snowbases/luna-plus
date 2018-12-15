// PKManager.h: interface for the CPKManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PKMANAGER_H__7471A7CD_E158_427A_B1A1_6E6943F84662__INCLUDED_)
#define AFX_PKMANAGER_H__7471A7CD_E158_427A_B1A1_6E6943F84662__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPlayer;

#define PKMGR CPKManager::GetInstance()


class CPKManager  
{
protected:
	
	BOOL	m_bPKAllow;

public:
	CPKManager();
	virtual ~CPKManager();

	MAKESINGLETON( CPKManager );

	void Init( BOOL bPKAllow );

	BOOL IsPKAllow() { return m_bPKAllow; }
	// 090824 ONS PK허용여부 설정.
	void SetPKAllow( BOOL bPKAllow ) { m_bPKAllow = bPKAllow; }

	void SetPKAllowWithMsg( BOOL bAllow );
	void NetworkMsgParse( BYTE Protocol, void* pMsg );

	void DiePanelty( CPlayer* pDiePlayer, CObject* pAttacker );	//죽은자와 죽인자에 대한 양쪽의 패널티 처리

	void NoPKModeDiePanelty( CPlayer* pPlayer );		//악명치에 대한 죽음 패널티(PK모드아닐때)
	void PKModeDiePanelty( CPlayer* pPlayer );			//악명치에 대한 죽음 패널티(PK모드일때)
	void PKModeTimeReduceByDie( CPlayer* pPlayer );		//죽음으로 인한 PKMode 시간 단축

	void PKPlayerKillPanelty( CPlayer* pDiePlayer, CPlayer* pAttacker );

//---KES CHEAT PKEVENT
protected:
	BOOL	m_bPKEvent;
public:
	BOOL IsPKEvent() { return m_bPKEvent; }
	void SetPKEvent( BOOL bEvent ) { m_bPKEvent = bEvent; }
//--------------------
};

#endif // !defined(AFX_PKMANAGER_H__7471A7CD_E158_427A_B1A1_6E6943F84662__INCLUDED_)
