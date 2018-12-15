// QuickManager.h: interface for the CQuickManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUICKMANAGER_H__81B555F7_56A1_47A6_93C7_D3F6D13C4EA6__INCLUDED_)
#define AFX_QUICKMANAGER_H__81B555F7_56A1_47A6_93C7_D3F6D13C4EA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define QUICKMNGR CQuickManager::GetInstance()

class CPlayer;

class CQuickManager  
{
	CQuickManager();
public:
	virtual ~CQuickManager();
	GETINSTANCE(CQuickManager);
	void NetworkMsgParse( BYTE Protocol, void* pMsg );
	
	void SendQuickInfo( CPlayer* pPlayer );
	BOOL AddQuickInfo( CPlayer* pPlayer, BYTE tabNum, BYTE pos, SLOT_INFO* pSlot );
	BOOL RemoveQuickInfo( CPlayer* pPlayer, BYTE tabNum, BYTE pos );
	BOOL ChangeQuickInfo( CPlayer* pPlayer, BYTE tabNum1, BYTE pos1, BYTE tabNum2, BYTE pos2 );
};

#endif // !defined(AFX_QUICKMANAGER_H__81B555F7_56A1_47A6_93C7_D3F6D13C4EA6__INCLUDED_)
