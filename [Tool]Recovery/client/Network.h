#pragma once


#include <initguid.h>


interface ISC_BaseNetwork;


class CNetwork  
{
public:	
	struct Server
	{
		CString	mName;
		CString	mIP;
		WORD	mPort;
	};
	typedef std::map< DWORD, Server > ServerMap;
	typedef stdext::hash_map< DWORD, DWORD > ConnectionMap;

public:
	static CNetwork& GetInstance();
	BOOL Initialise( HWND );
	BOOL Connect();
	void Send( DWORD serverIndex, const MSGROOT&, size_t );
	void SendAll( const MSGROOT&, size_t );
	// 081021 LUJ, 유효한 서버 인덱스 하나를 반환한다
	DWORD GetServerIndex() const;
	void Release();
	void Disconnect( DWORD serverIndex );
	void AllDisconnect();
	DWORD GetServerIndex( const CString& ) const;
	LPCTSTR GetServerName( DWORD serverIndex ) const;
	void LoadScript();
	ISC_BaseNetwork* GetBaseNetwork() const { return mInterface; }
	const ServerMap& GetServerMap() const { return mServerMap; }
	const ConnectionMap& GetConnectionMap() const { return mConnectionMap; }

private:
	HMODULE mModule;
	ISC_BaseNetwork* mInterface;
	// 키: 서버 인덱스
	ServerMap mServerMap;
	// 키: 서버 인덱스, 값: 컨넥션 인덱스
	ConnectionMap mConnectionMap;

private:
	// 싱글턴이므로 생성자와 소멸자를 직접 사용할 수 없도록 한다
	CNetwork();
	virtual ~CNetwork();
	static void OnReceive( DWORD connectionIndex, char* message, DWORD msglen );
	static void OnConnect( DWORD connectionIndex );
	static void OnDisconnect( DWORD connectionIndex );
	// 문자열에서 서버 접속을 위한 정보를 얻어낸다
	void SetConnectData(const CString&, Server&) const;
};


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