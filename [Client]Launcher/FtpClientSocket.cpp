// FtpClientSocket.cpp: implementation of the CFtpClientSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FtpClientSocket.h"
#include "DataSocket.h"

#ifdef	__UNIX__
#include <sys/time.h>

void UnixSleep(int iMiliSec)
{
	struct  timeval timedelay;

	timedelay.tv_sec = (iMiliSec * 1000) / 1000000;
	timedelay.tv_usec = (iMiliSec * 1000) % 1000000;

	select(0,(fd_set*)0,(fd_set*)0,(fd_set*)0,&timedelay);
}
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFtpClientSocket::CFtpClientSocket()
{
	InitMember();
}

CFtpClientSocket::~CFtpClientSocket()
{
	printf("!!!!!!!!!!!! Destroy BaseSocket\n");
	CloseFtp();

	//루나테스트
	//	FILE* pFile = fopen( "launcherdebug.txt", "at" );
	//	if( pFile )
	//	{
	//		fprintf( pFile, "****After CloseFtp()\n" );
	//		fclose(pFile);
	//	}
	//---------
}

int CFtpClientSocket::InitFtpSocket()
{
	InitMember();

	if(gethostname(m_szHostName, sizeof(m_szHostName))==SOCKET_ERROR)
		sprintf(m_szHostName, "Local");

#ifndef __UNIX__
	WSADATA	wsaData;
	memset(&wsaData, 0x00, sizeof(WSADATA));
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)==SOCKET_ERROR)
	{
		CloseFtp();
		return RETURN_FAIL;
	}
	printf("!!!! Call Success WSAStartup\n");
	m_bStartSocket = TRUE;
#endif

	//성공값은 0이다.
	//Socket과 관련된 함수가 실패시에 -1을 return한다.(성공시에는 0)
	return RETURN_GOOD;
}

void CFtpClientSocket::InitMember()
{
	m_socketId = SOCKET_ERROR;
	memset(m_szCommandBuffer, 0x00, CMD_BUF_SIZE);
	memset(m_szHostName, 0x00, MAX_HOSTNAME_SIZE);
	memset(m_szTransferBuffer, 0x00, MAX_HOSTNAME_SIZE);
	m_pHostInfo = NULL;

	m_uiServerPort = 0;
	memset(m_szServerName, 0x00, sizeof(m_szServerName));
	memset(m_szUserId, 0x00, sizeof(m_szUserId));
	memset(m_szPassword, 0x00, sizeof(m_szPassword));

	m_bStartSocket = FALSE;

	ITR_COMMAND_RESULT_VEC itrCommand = m_vectorCommand.begin();
	while(itrCommand!=m_vectorCommand.end())
	{
		delete [] (*itrCommand);
		(*itrCommand) = NULL;
		itrCommand++;
	}

	m_vectorCommand.clear();
}

void CFtpClientSocket::CloseFtp()
{
	if(m_socketId>=0)
	{
#ifdef __UNIX__
		shutdown(m_socketId, SHUT_RDWR);
		close(m_socketId);
#else
		shutdown(m_socketId, SD_BOTH);
		closesocket(m_socketId);
#endif
	}
#ifndef __UNIX__
	if(m_bStartSocket==TRUE)
	{
		if(WSACleanup()==SOCKET_ERROR)
		{
			printf("!!!! ===> Error Call WSACleanup ErrNo => %d\n", ::WSAGetLastError());
		}
		else
		{
			printf("!!!! Call Success WSACleanup\n");
		}

		m_bStartSocket = FALSE;
	}
#endif
	InitMember();
}

int CFtpClientSocket::ConnectFtp(const char* pszServerName, unsigned int uiPort/*, BOOL bReadGreeting/?=TRUE?/*/)
{
	if(m_socketId!=SOCKET_ERROR)
	{
		printf("!!! Fail to Connect to Server. Another SocketId[%d] Exists!!\n", m_socketId);
		return RETURN_FAIL;		
	}

	if(InitFtpSocket()==RETURN_FAIL)
	{
		CloseFtp();
		return RETURN_FAIL;
	}

	m_socketId = socket(AF_INET, SOCK_STREAM, 0);

	if(m_socketId==INVALID_SOCKET)
	{
		printf("!!! Fail to Create Socket(%d)[socket] return -1. errno=>%d, errstr=>%s\n",
			m_socketId, errno, strerror(errno));
		CloseFtp();
		return RETURN_FAIL;
	}


	int iOptionValue = 8;
	int iOptionLen = sizeof(iOptionValue);
	//아래 setsockopt 당최 무슨말인지 모르겠음..
#ifdef __UNIX__
	if(setsockopt(m_socketId, IPPROTO_IP, IP_TOS, &iOptionValue, iOptionLen)==SOCKET_ERROR)
#else
	if(setsockopt(m_socketId, SOL_SOCKET, SO_LINGER,
		(char*)&iOptionValue, iOptionLen)==SOCKET_ERROR)
#endif
	{
		printf("!!! Fail to Create Socket(%d)[setsockopt] return -1. errno=>%d, errstr=>%s\n",
			m_socketId, errno, strerror(errno));
		CloseFtp();
		return RETURN_FAIL;
	}

	struct sockaddr_in address;
	memset(&address, 0x00, sizeof(sockaddr_in));
	address.sin_family = AF_INET;
	address.sin_port = htons((unsigned short)uiPort);

	if(m_pHostInfo==NULL)
	{
		m_pHostInfo = gethostbyname(pszServerName);
		if(m_pHostInfo==NULL)
		{
			printf("!!! Fail to Create Socket(%d)[gethostbyname] return -1. errno=>%d, errstr=>%s\n",
				m_socketId, errno, strerror(errno));
			CloseFtp();
			return RETURN_FAIL;
		}
	}

	memcpy(&address.sin_addr.s_addr, m_pHostInfo->h_addr, m_pHostInfo->h_length);
	if(connect(m_socketId, (struct sockaddr*)&address, sizeof(sockaddr_in))==SOCKET_ERROR)
	{
		printf("!!! Fail to Create Socket(%d)[connect(IP:%s,PORT:%d)] return -1. errno=>%d, errstr=>%s\n",
			m_socketId, pszServerName, uiPort, errno, strerror(errno));
		CloseFtp();
		return RETURN_FAIL;
	}

	//서버 이름(IP) 저장..
	memset(m_szServerName, 0x00, sizeof(m_szServerName));
	sprintf(m_szServerName, "%s", pszServerName);
	m_uiServerPort = uiPort;

	return m_socketId;
}

int CFtpClientSocket::ConnectFtp(sockaddr_in* pAddr,const char* pszServerName, unsigned int uiPort)
{
	if(m_socketId!=SOCKET_ERROR)
	{
		printf("!!! Fail to Connect to Server. Another SocketId[%d] Exists!!\n", m_socketId);
		return RETURN_FAIL;		
	}

	if(InitFtpSocket()==RETURN_FAIL)
	{
		CloseFtp();
		return RETURN_FAIL;
	}

	m_socketId = socket(AF_INET, SOCK_STREAM, 0);

	if(m_socketId==INVALID_SOCKET)
	{
		printf("!!! Fail to Create Socket(%d)[socket] return -1. errno=>%d, errstr=>%s\n",
			m_socketId, errno, strerror(errno));
		CloseFtp();
		return RETURN_FAIL;
	}


	int iOptionValue = 8;
	int iOptionLen = sizeof(iOptionValue);
	//아래 setsockopt 당최 무슨말인지 모르겠음..
#ifdef __UNIX__
	if(setsockopt(m_socketId, IPPROTO_IP, IP_TOS, &iOptionValue, iOptionLen)==SOCKET_ERROR)
#else
	if(setsockopt(m_socketId, SOL_SOCKET, SO_LINGER,
		(char*)&iOptionValue, iOptionLen)==SOCKET_ERROR)
#endif
	{
		printf("!!! Fail to Create Socket(%d)[setsockopt] return -1. errno=>%d, errstr=>%s\n",
			m_socketId, errno, strerror(errno));
		CloseFtp();
		return RETURN_FAIL;
	}


	if(connect(m_socketId, (struct sockaddr*)pAddr, sizeof(sockaddr_in))==SOCKET_ERROR)
	{

		CloseFtp();
		return RETURN_FAIL;
	}

	//서버 이름(IP) 저장..
	memset(m_szServerName, 0x00, sizeof(m_szServerName));
	sprintf(m_szServerName, "%s", pszServerName);
	m_uiServerPort = uiPort;

	return m_socketId;
}


int CFtpClientSocket::RecvRawData(char *pszBuffer, int iLength, int iWaitData/*=0*/, SOCKET socketId/*=-1*/)
{
	if(socketId==-1)
		socketId = m_socketId;

	unsigned long uiLength = 0;

#ifdef __UNIX__
	if(ioctl(socketId, FIONREAD, &uiLength)==SOCKET_ERROR)
	{
		printf("!!! Fail to 'ioctl' Socket(%d) return -1. errno=>%d, errstr=>%s\n",
			m_socketId, errno, strerror(errno));
		return RETURN_FAIL;
	}
#else
	if(ioctlsocket(socketId, FIONREAD, &uiLength)==SOCKET_ERROR)
	{
		printf("!!! Fail to 'ioctlsocket' Socket(%d) return -1. errno=>%d\n",
			m_socketId, ::WSAGetLastError());
		return RETURN_FAIL;
	}
#endif

	//데이터가 없으면, 기다리지 않고 바로 Return하는 옵션..
	if(uiLength==0)
	{
		if(iWaitData==0)
			return 0;
	}

	//데이터가 없는데, 기다려야 하는 상황..
	if((uiLength==0)&&(iWaitData!=0))
	{
		time_t tCurrent;
		time(&tCurrent);
		time_t tTimeOver;

		if(iWaitData>0) //-1 이면, 끝까지 기다린다...
			tTimeOver = tCurrent + iWaitData;

		//데이터가 들어올 때까지...
		while(uiLength==0)
		{
#ifdef __UNIX__
			if(ioctl(socketId, FIONREAD, &uiLength)==SOCKET_ERROR)
#else
			if(ioctlsocket(socketId, FIONREAD, &uiLength)==SOCKET_ERROR)
#endif
			{
				printf("!!! Fail to 'ioctlsocket' Socket(%d) return -1. errno=>%d, errstr=>%s\n",
					m_socketId, errno, strerror(errno));
				return RETURN_FAIL;
			}

			if(iWaitData>0) //-1 이면, 끝까지 기다린다...
			{
				//아직도 Data가 없고, 시간이 넘었다면... 그냥 나간다.
				if((uiLength==0)&&(tTimeOver<tCurrent))
				{
					/*DEBUG*/
					printf("!!! ioctl Time-Over SocketId[%d], WaitTime[%d]\n", m_socketId, iWaitData);
					/**/
					return 0;
				}

				time(&tCurrent);
			}
#ifdef __UNIX__
			UnixSleep(1);
#else
			Sleep(1);
#endif
		}
	}

	//NT의 경우
	// -1 (SOCKET_ERROR) : 받는 과정에서 오류가 발생함.
	// 0 : 갑자기 소켓이 끊겼음..===> !!!!!!!!! 처리되지 않았음 
	//                           ===> 복잡..심난..!!!!!!!!!!!
	// 0 이상 : 전송에 성공하였음..
	int iRecvLength = recv(socketId, pszBuffer, iLength, 0);

	//에러가 발생했을 때만, 끊겼을 경우에는 0을 return..
	if(iRecvLength<0)
	{
		time_t tCurrent;
		time(&tCurrent);

		time_t tTimeOver = tCurrent + SOCKET_TIME_OUT;
		while(iRecvLength<0)
		{
#ifdef __UNIX__
			int iError = errno;
			if(iError==EWOULDBLOCK)
#else
			int iError = ::WSAGetLastError();
			if(iError==WSAEWOULDBLOCK)
#endif
			{
#ifdef __UNIX__
				UnixSleep(10);
#else
				Sleep(10);
#endif
				iRecvLength = recv(socketId, pszBuffer, iLength, 0);
			}
			else
			{
				return -iError;
			}

			//여전히 에러가 발생하고, 시간이 넘었다면... 그냥 나간다.
			if((iRecvLength<0)&&(tTimeOver<tCurrent))
			{
				printf("!!! Recv Time-Over SocketId[%d], WaitTime[%d]\n", m_socketId, iWaitData);
				return -iError;
			}

			time(&tCurrent);
		}
	}

	/*DEBUG*/
	//	if(iRecvLength>0)
	//		printf("recv_raw => %s", pszBuffer);
	/**/

	return iRecvLength;
}

int CFtpClientSocket::SendRawData(char *pszData, int iLength, SOCKET socketId/*=-1*/)
{
	if(socketId==-1)
		socketId = m_socketId;

	int iTotSendData = 0;
	int iSendLength = 0;

	time_t tCurrent;
	time(&tCurrent);
	time_t tTimeOver = tCurrent+SOCKET_TIME_OUT;

	int iError = 0;

	while(iTotSendData<iLength)
	{
		iSendLength = send(socketId, pszData+iTotSendData, iLength-iTotSendData, 0);

		//에러가 발생한 상황..
		if(iSendLength==SOCKET_ERROR)
		{
#ifdef __UNIX__
			iError = errno;
			if(iError==EWOULDBLOCK)
#else
			iError = ::WSAGetLastError();
			if(iError==WSAEWOULDBLOCK)
#endif
			{
#ifdef __UNIX__
				UnixSleep(10);
#else
				Sleep(10);
#endif
			}
			else
			{
				printf("!!! Error Send SocketId[%d] iError[%d]\n", m_socketId, iError);
				return -iError;
			}
		}
		else //보내긴 보낸 상황.. 바이트 체크는 while문에서..
		{
			iTotSendData += iSendLength;
			if(iTotSendData>=iLength)
				break;
			else
#ifdef __UNIX__
				UnixSleep(1);
#else
				Sleep(1);
#endif
		}

		if(tTimeOver<tCurrent)
		{
			printf("!!! Send Time-Over SocketId[%d], WaitTime[%d], Sent[%d], iError[%d]\n",
				m_socketId, SOCKET_TIME_OUT, iTotSendData, iError);

			if(iTotSendData>0)
				return RETURN_FAIL;
			else
				return -iError;
		}

		time(&tCurrent);
		iError = 0;
	}

	return iTotSendData;
}