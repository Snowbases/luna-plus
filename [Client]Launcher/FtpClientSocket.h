#pragma once


#define CMD_BUF_SIZE 2047
#define MAX_HOSTNAME_SIZE 256
#define TRANSFER_BUFFER_SIZE 2048

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#ifdef __UNIX__
typedef int SOCKET;
//결국 INVALID_SOCKET ==> -1과 같은 비트 조합을 가진다.
#define INVALID_SOCKET (SOCKET)(~0)
#endif

const char LOCAL_NAME[] = {"Local"};
const char LINE_END_CHAR[] = {"\r\n"};
const int LINE_END_CR = '\r';
const int LINE_END_LF = '\n';

//return값 정의
const int RETURN_FAIL = -1;
const int RETURN_GOOD = 0;

// 071005 웅주, 이 수치를 조정하여 오류가 발생할 경우 readme.txt를 참조할 것
//const int MAX_SOCKET_BUFFER = 1024;
const int MAX_SOCKET_BUFFER = 102400;

//recv와 send 시 얼마나 다시 재시도 할 것인가를 설정한다.
const int SOCKET_TIME_OUT = 5;
//소켓에 두개 이상의 code값이 들어올 경우를 대비해서
//들어온 데이터를 다 모아놓았다가 몇개의 code가 들어왔는지 검사한다.
const int MAX_CODE_CHECK_BUFFER = MAX_SOCKET_BUFFER*3;

//Code 검사에 필요한 길이.
const int CODE_LENGTH = 4;
const int MAX_LINE_LENGTH = 2048;
const int MAX_COMMAND_BUFFER = 256;

const int PORT_INFO_LENGTH = 6;
const int PORT_MAKE_KEY = 256;
const int PORT_NUMBER_START_INDEX = 4;
const int PORT_NUMBER_ADD_INDEX = 5;


typedef vector<char*> VEC_COMMAND_RESULT;
typedef VEC_COMMAND_RESULT::iterator ITR_COMMAND_RESULT_VEC;

class CFtpClientSocket
{
public:
	CFtpClientSocket();
	virtual ~CFtpClientSocket();

protected:
	int ConnectFtp(sockaddr_in* pAddr,const char* pszServerName, unsigned int uiPort);
	int ConnectFtp(const char* pszServerName, unsigned int uiPort/*, BOOL bReadGreeting=TRUE*/);
	int SendRawData(char* pszCommand, int iLength, SOCKET socketId=-1); //iLength는 종료문자 제외한 길이임.
	int RecvRawData(char* pszBuffer, int iLength, int iWaitData=0, SOCKET socketId=-1); //iLength는 종료문자 제외한 길이임.

	int InitFtpSocket();
	void InitMember();

	VEC_COMMAND_RESULT m_vectorCommand;

	char m_szPassword[64];
	char m_szUserId[64];
	unsigned int m_uiServerPort;
	char m_szServerName[64];

	struct hostent* m_pHostInfo; //host 정보를 찾아서 저장한다.
	char m_szTransferBuffer[TRANSFER_BUFFER_SIZE]; //Data 전송에 사용되는 버퍼..
	char m_szHostName[MAX_HOSTNAME_SIZE]; //호스트이름을 저장하는 버퍼..
	char m_szCommandBuffer[CMD_BUF_SIZE+1]; //Command를 저장하는 버퍼..
	SOCKET m_socketId; //해당 소켓 ID
private:
	void CloseFtp();
	BOOL m_bStartSocket;
};