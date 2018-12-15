#pragma once


#include "..\4DYUCHINET_COMMON\inetwork.h"

#define NETWORK USINGTON(CNetwork)


class CNetwork  
{
	I4DyuchiNET* m_pINet;
public:
	CNetwork();
	virtual ~CNetwork();

	bool Init(DESC_NETWORK * desc);
	void Release();
	I4DyuchiNET* GetINet() { return m_pINet; }
	
	bool StartServer();

	void GetUserAddress(DWORD dwConnectionIndex, char* ip, WORD * port);

	void Send(DWORD connectionIndex, const MSGROOT& message, DWORD size);
	
	void DisconnectUser( DWORD connectionIndex );

public:
	struct Address
	{
		char	mIp[ MAX_PATH ];
		DWORD	mPort;
	}
	mAddress;

    const Address& GetAddress() const;
};
EXTERNGLOBALTON(CNetwork)