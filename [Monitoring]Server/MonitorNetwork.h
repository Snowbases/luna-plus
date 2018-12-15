// MonitorNetwork.h: interface for the CMonitorNetwork class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONITORNETWORK_H__3673D8A1_C2EA_4E32_B00A_1455997BF324__INCLUDED_)
#define AFX_MONITORNETWORK_H__3673D8A1_C2EA_4E32_B00A_1455997BF324__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 080813 LUJ, 수정된 inetwork.dll을 사용하도록 참조
#include "..\4DYUCHINET_COMMON\inetwork.h"

#define MNETWORK USINGTON(CMonitorNetwork)

class CMonitorNetwork  
{
	I4DyuchiNET* m_pINet;
public:
	CMonitorNetwork();
	virtual ~CMonitorNetwork();

	BOOL Init(DESC_NETWORK * desc);
	void Release();
	I4DyuchiNET* GetINet() { return m_pINet; }
	
	//Connect to MAS
	void ConnectToMAS();
	// MAPServer¸¦ Accept OR MAS Accept
	BOOL StartServer(WORD type);

	void GetUserAddress(DWORD dwConnectionIndex, char* ip, WORD * port);

	//only ms
	void SendToMapServer(DWORD dwConnectionIdx, char* msg, DWORD size);
	void SendToMapServerAll(char* msg, DWORD size);
	void SendToMAS(char* msg, DWORD size);

	// only mas
	void SendToMC(DWORD dwConnectionIdx, char* msg, DWORD size);
	void SendToMS(DWORD dwConnectionIdx, char* msg, DWORD size);
	void SendToMSAll(char* msg, DWORD size);

	void DisconnectUser( DWORD dwConnectionIdx );

	// 080118 KTH -- MS 껏다 켜도 정보를 다시 받을수 있게끔 접속 시도
	void ConnectToServers();
};
EXTERNGLOBALTON(CMonitorNetwork)
#endif // !defined(AFX_MONITORNETWORK_H__3673D8A1_C2EA_4E32_B00A_1455997BF324__INCLUDED_)
