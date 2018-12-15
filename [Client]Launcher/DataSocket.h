// DataSocket.h: interface for the CDataSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATASOCKET_H__ACCE2EEE_5C92_482D_9601_DD144D19FF49__INCLUDED_)
#define AFX_DATASOCKET_H__ACCE2EEE_5C92_482D_9601_DD144D19FF49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FtpClientSocket.h"

class CDataSocket : public CFtpClientSocket  
{
public:
	int SendInformationData(char* pszBuffer, int iLength);
	int RecvInformationData(char* pszBuffer, int iLength);
	int ConnectFtp(char *pszServerName, unsigned int uiPort);


	// 2007/10/08 by yuchi
	int ConnectFtp(sockaddr_in* pAddr,const char* pszServerName, unsigned int uiPort);
	CDataSocket();
	virtual ~CDataSocket();

};

#endif // !defined(AFX_DATASOCKET_H__ACCE2EEE_5C92_482D_9601_DD144D19FF49__INCLUDED_)