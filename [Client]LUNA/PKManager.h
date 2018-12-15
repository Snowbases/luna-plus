// PKManager.h: interface for the CPKManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PKMANAGER_H__0681C2A4_4B93_460A_A1F3_2655FAFD3396__INCLUDED_)
#define AFX_PKMANAGER_H__0681C2A4_4B93_460A_A1F3_2655FAFD3396__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PKMGR USINGTON(CPKManager)

class CPlayer;
class CPKLootingDialog;

class CPKManager  
{
protected:
	BOOL	m_bSyncing;
	DWORD	m_dwPKModeStartTime;
	DWORD	m_dwPKContinueTime;
	
	BOOL	m_bPKAllow;
	
//---·çÆÃ´çÇÔ
	BOOL	m_bLooted;
//---·çÆÃÁß
	BOOL	m_bLooting;

//---------------
//ÀÎÅÍÆäÀÌ½º °ü·Ã
	CYHHashTable<CPKLootingDialog>	m_htLootingDlg;
	CIndexGenerator					m_IndexGenerator;	

public:
	CPKManager();
	virtual ~CPKManager();

	void	Init();
	void	Release();
	BOOL	HEROPKModeOn();
	BOOL	HEROPKModeOff();
	BOOL	ToggleHeroPKMode();
	void	SendMsgPKOn();
	
	void	SetPlayerPKMode( CPlayer* pPlayer, BOOL bPKMode );
	void	SetPKStartTimeReset()				{ m_dwPKModeStartTime = gCurTime; }
	void	SetPKContinueTime( DWORD dwTime )	{ m_dwPKContinueTime = dwTime; }
	DWORD	GetPKContinueTime()					{ return m_dwPKContinueTime; }

	void	SetPKAllow( BOOL bAllow )	{ m_bPKAllow = bAllow; }
	BOOL	IsPKAllow()					{ return m_bPKAllow; }
//parsing
	void NetworkMsgParse( BYTE Protocol, void* pMsg );

//---·çÆÃ´çÇÔ
	void SetPKLooted( BOOL bLooted )	{ m_bLooted = bLooted; }
	BOOL IsPKLooted()					{ return m_bLooted; }
//---·çÆÃÇÔ
	void SetPKLooting( BOOL bLooting )	{ m_bLooting = bLooting; }
	BOOL IsPKLooting()					{ return m_bLooting; }
	
//---------------
//ÀÎÅÍÆäÀÌ½º °ü·Ã
	DWORD AllocWindowId()	{ return m_IndexGenerator.GenerateIndex(); }
	void FreeWindowId( DWORD dwID ) { m_IndexGenerator.ReleaseIndex( dwID ); }

	CPKLootingDialog* CreateLootingDlg( DWORD dwDiePlayerIdx );
	void RemoveAllLootingDlg();
	void PositioningDlg( LONG& rX, LONG& rY );
	void CloseLootingDialog( CPKLootingDialog* pLootingDlg, BOOL bMsgToServer );
	
	int GetLootingChance( DWORD dwBadFame );
	int GetLootingItemNum( DWORD dwBadFame );

	// 070205 LYW --- Add functions to process network msg.
public :
	void Pk_PkOn_Ack( void* pMsg ) ;
	void Pk_PkOn_Nack( void* pMsg ) ;
	void Pk_PkOff_Ack( void* pMsg ) ;
	void Pk_Disappear( void* pMsg ) ;
	void Pk_PkOff_Nack() ;
	void Pk_Looting_Start( void* pMsg ) ;
	void Pk_Looting_BeingLooted( void* pMsg ) ;
	void Pk_Looting_ItemLooting( void* pMsg ) ;
	void Pk_Looting_ItemLooted( void* pMsg ) ;
	void Pk_Looting_MoenyLooting( void* pMsg ) ;
	void Pk_Looting_MoenyLooted( void* pMsg ) ;
	void Pk_Looting_ExpLooting( void* pMsg ) ;
	void Pk_Looting_ExpLooted( void* pMsg ) ;
	void Pk_Looting_NoLooting( void* pMsg ) ;
	void Pk_Looting_NoInvenspace( void* pMsg ) ;
	void Pk_Looting_EndLooting( void* pMsg ) ;
	void Pk_Destroy_Item( void* pMsg ) ;
	void Pk_Looting_Error( void* pMsg ) ;
	void Pk_Pkallow_Changed( void* pMsg ) ;
};

EXTERNGLOBALTON(CPKManager)

#endif // !defined(AFX_PKMANAGER_H__0681C2A4_4B93_460A_A1F3_2655FAFD3396__INCLUDED_)
