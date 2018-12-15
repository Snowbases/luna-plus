// MHNetwork.h: interface for the CMHNetwork class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MHNETWORK_H__E93C75BD_70FA_4330_B63C_2B5A6A57132A__INCLUDED_)
#define AFX_MHNETWORK_H__E93C75BD_70FA_4330_B63C_2B5A6A57132A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <initguid.h>

interface ISC_BaseNetwork;

#define WM_SOCKEVENTMSG WM_USER+155

#define NETWORK USINGTON(CMHNetwork)

class CMHNetwork  
{
protected:
	ISC_BaseNetwork* m_pBaseNetwork;
	DWORD	m_dwConnectionIndex[MAX_SERVERSET];
	BYTE	m_CheckSum[MAX_SERVERSET];

public:
	HWND	m_hMainWnd;

public:
	CMHNetwork();
	virtual ~CMHNetwork();
	
	BOOL	Init(HWND hWnd);
	void	Release();

	BOOL	ConnectToServer( int serverset, char* ip, WORD port );
	void	Disconnect( int serverset );
	void	AllDisconnect();
	void	Send( int serverset, MSGROOT* pMsg, int MsgLen );
	void	SendAll( MSGROOT* pMsg, int MsgLen );

	ISC_BaseNetwork* GetBaseNetwork()		{ return m_pBaseNetwork; }
	friend void OnCon( DWORD dwConIndex );
	friend void OnDisCon( DWORD dwConIndex );
	friend void OnRecv( DWORD dwConIndex, char* pMsg, DWORD msglen);
};

EXTERNGLOBALTON(CMHNetwork)

struct DESC_BASENETWORK
{
	WORD		wSockEventWinMsgID;
	void		(*ReceivedMsg)(DWORD dwInex,char* pMsg,DWORD dwLen);
	void		(*OnDisconnect)(DWORD dwInex);
	void		(*OnConnect)(DWORD dwInex);
};

// {78771B7B-6E5E-4659-87E4-ABE9F0793AA1}
DEFINE_GUID(CLSID_SC_BASENETWORK_DLL, 
0x78771b7b, 0x6e5e, 0x4659, 0x87, 0xe4, 0xab, 0xe9, 0xf0, 0x79, 0x3a, 0xa1);

// {DCED45F1-980B-4578-9F1D-C0586A5E3964}
DEFINE_GUID(IID_SC_BASENETWORK_DLL, 
0xdced45f1, 0x980b, 0x4578, 0x9f, 0x1d, 0xc0, 0x58, 0x6a, 0x5e, 0x39, 0x64);

interface ISC_BaseNetwork : public IUnknown
{
	virtual void	__stdcall					CloseAllConnection() = 0;
	virtual void	__stdcall					CloseConnection(DWORD dwIndex) = 0;
	virtual BOOL	__stdcall					Send(DWORD dwConnectionIndex,char* msg,DWORD length) = 0;
	virtual DWORD	__stdcall					ConnectToServer(char* szIP,WORD port) = 0;
	virtual BOOL	__stdcall					StartServer(char* ip,WORD port) =0;
	virtual void	__stdcall					CompulsiveDisconnect(DWORD dwIndex) =0;
	virtual DWORD	__stdcall					InitNetwork(HWND hwnd, DWORD dwMaxConnect,DESC_BASENETWORK* pDesc )=0;
	virtual	void	__stdcall					SocketEventProc() = 0;
};

typedef HRESULT (__stdcall *DllGetClassObject_BaseNetwork)( REFCLSID , REFIID , LPVOID*);

#endif // !defined(AFX_MHNETWORK_H__E93C75BD_70FA_4330_B63C_2B5A6A57132A__INCLUDED_)
