// DataSocket.cpp: implementation of the CDataSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DataSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataSocket::CDataSocket()
{

}

CDataSocket::~CDataSocket()
{
	printf("!!!!!!!!!!!! Destroy DataSocket!!!!\n");
}

int CDataSocket::ConnectFtp(char *pszServerName, unsigned int uiPort)
{
	return CFtpClientSocket::ConnectFtp(pszServerName, uiPort);
}

// 2007/10/08 by yuchi
int CDataSocket::ConnectFtp(sockaddr_in* pAddr,const char* pszServerName, unsigned int uiPort)
{
	return CFtpClientSocket::ConnectFtp(pAddr,pszServerName, uiPort);
}

int CDataSocket::RecvInformationData(char *pszBuffer, int iLength)
{
	//뒤에 1초를 주는 것이 현명한 것 같다...... ===> 취소. FILE SIZE를 CHECK할 수 있음.
	return RecvRawData(pszBuffer, iLength/*, 1*/);
}

int CDataSocket::SendInformationData(char *pszBuffer, int iLength)
{
	return SendRawData(pszBuffer, iLength);
}