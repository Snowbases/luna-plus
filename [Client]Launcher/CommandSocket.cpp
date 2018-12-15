// CommandSocket.cpp: implementation of the CCommandSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CommandSocket.h"
#include "FtpFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommandSocket::CCommandSocket()
{
	/*
#ifdef __UNIX__
	mkdir(FTPLOG_PATH_NAME, S_IRWXU|S_IRWXG|S_IRWXO);
#else
	mkdir(FTPLOG_PATH_NAME);
#endif
	*/
		m_iResponseTimeOut = 1;
}

CCommandSocket::~CCommandSocket()
{
	printf("!!!!!!!!!!!! Destroy CommandSocket!!!!\n");
	CloseConnection();
}

void CCommandSocket::CloseConnection()
{
	int iCode = 0;
	if(SendCommandData("QUIT")>=0)
		GetCommandCodeFromServer(&iCode);

	//	//루나테스트
	//	FILE* pFile = fopen( "launcherdebug.txt", "at" );
	//	if( pFile )
	//	{
	//		fprintf( pFile, "****After CloseConnection()!****\n" );
	//		fclose(pFile);
	//	}
	//	//---------
}

int CCommandSocket::ConnectFtp(const char *pszServerName, unsigned int uiPort)
{
	int iConnect = CFtpClientSocket::ConnectFtp(pszServerName, uiPort);
	if(iConnect==RETURN_FAIL)
		return RETURN_FAIL;

	//인사말을 읽어들인다.
	int iReturnCode = -1;
	if(GetCommandCodeFromServer(&iReturnCode)<0)
		return RETURN_FAIL;

	if( iReturnCode != WELCOME_MESSAGE )
		printf("It is not welcom message => %d\n", iReturnCode);

	return iConnect;
}

int CCommandSocket::LogIn(const char *pszUserId, const char *pszPassword)
{
	char szCommand[MAX_COMMAND_BUFFER];
	//memset(szCommand, 0x00, sizeof(szCommand));

	sprintf(szCommand, "USER %s", pszUserId);
	if(SendCommandData(szCommand)<0)
	{
		printf("!!! Fail to Log In On Send..\n");
		return RETURN_FAIL;
	}

	int iCode = 0;
	if(GetCommandCodeFromServer(&iCode)<0)
	{
		printf("!!! Fail to Log In On Recv..\n");
		return RETURN_FAIL;		
	}

	//패스워드 없이 그냥 패스됨...
	if(iCode==LOGIN_SUCCESS)
		return RETURN_GOOD;

	if(iCode!=PASSWORD_REQUIRED)
		return RETURN_FAIL;

	//memset(szCommand, 0x00, sizeof(szCommand));
	sprintf(szCommand, "PASS %s", pszPassword);
	if(SendCommandData(szCommand)<0)
	{
		printf("!!! Fail to Log In PASS On Send..\n");
		return RETURN_FAIL;		
	}

	iCode = 0;
	if(GetCommandCodeFromServer(&iCode)<0)
	{
		printf("!!! Fail to Log In PASS On Recv..\n");
		return RETURN_FAIL;		
	}

	if(iCode!=LOGIN_SUCCESS)
	{
		printf("!!! Fail to Log In Error from SERVER..%d\n", iCode);
		return RETURN_FAIL;		
	}

	//정보저장해놓음.....
	memset(m_szUserId, 0x00, sizeof(m_szUserId));
	sprintf(m_szUserId, "%s", pszUserId);
	memset(m_szPassword, 0x00, sizeof(m_szPassword));
	sprintf(m_szPassword, "%s", pszPassword);

	return RETURN_GOOD;
}

// 2007/10/08 by yuchi
int CCommandSocket::GetPortForDataConnection(BOOL bPassiveMode,sockaddr_in* pOutAddr)
{
	char szCommand[MAX_COMMAND_BUFFER];
	memset(szCommand, 0x00, sizeof(szCommand));

	if(bPassiveMode==TRUE)
	{
		sprintf(szCommand, "PASV");
	}
	else
	{
		printf("!!! Support Only the 'PassiveMode'. Change the setting.\n");
		return RETURN_FAIL;
	}

	if(SendCommandData(szCommand)<0)
	{
		printf("!!! Fail to PASV On Send..\n");
		return RETURN_FAIL;
	}

	int iCode = 0;
	char szResultPort[MAX_COMMAND_BUFFER];
	memset(szResultPort, 0x00, sizeof(szResultPort));

	if(GetCommandCodeFromServer(&iCode, szResultPort)<0)
	{
		printf("!!! Fail to PASV On Recv..\n");
		return RETURN_FAIL;
	}

	if(ENTER_PASSIVE_MODE!=iCode)
	{
		printf("!!! Fail to Enter Passive Mode => error code %d\n", iCode);
		return RETURN_FAIL;
	}

	//앞에 return된 code값 제거..
	int iLen = strlen(szResultPort+CODE_LENGTH);
	for(int iIndex= 0; iIndex<iLen; iIndex++)
	{
		if(!isdigit(szResultPort[CODE_LENGTH+iIndex]))
			szResultPort[CODE_LENGTH+iIndex] = ' ';
	}

	unsigned int arrPortInfo[6];
	memset(arrPortInfo, 0x00, sizeof(arrPortInfo));
	if(PORT_INFO_LENGTH!=sscanf(szResultPort+CODE_LENGTH,
		"%u %u %u %u %u %u",
		arrPortInfo,
		arrPortInfo+1,
		arrPortInfo+2,
		arrPortInfo+3,
		arrPortInfo+4,
		arrPortInfo+5))
	{
		return RETURN_FAIL;
	}

	//return (arrPortInfo[PORT_NUMBER_START_INDEX]*PORT_MAKE_KEY) + arrPortInfo[PORT_NUMBER_ADD_INDEX];

	// 2007/10/08 by yuchi
	// 서버에서 접속하라고 알려준 주소와 ip를 세팅해서 되돌립니다.
	// FTP 도메인 네임으로 접속하면 안됩니다. 여기서 얻은 IP와 포트만이 유효합니다.

	WORD	wPort = (arrPortInfo[PORT_NUMBER_START_INDEX]*PORT_MAKE_KEY) + arrPortInfo[PORT_NUMBER_ADD_INDEX];

	memset(pOutAddr,0,sizeof(sockaddr_in));
	pOutAddr->sin_family = AF_INET;
	pOutAddr->sin_addr.S_un.S_un_b.s_b1 = arrPortInfo[0];
	pOutAddr->sin_addr.S_un.S_un_b.s_b2 = arrPortInfo[1];
	pOutAddr->sin_addr.S_un.S_un_b.s_b3 = arrPortInfo[2];
	pOutAddr->sin_addr.S_un.S_un_b.s_b4 = arrPortInfo[3];

	pOutAddr->sin_port = htons((unsigned short)wPort);

	return wPort;

}



int CCommandSocket::SendCommandData(char* pszCommand)
{
	//strlne은 null종료문자를 뺀 길이를 Return한다.
	//따라서, 내용길이+종료문자(2바이트)+종료문자(1바이트)
	char* pszLine = new char[strlen(pszCommand)+sizeof(LINE_END_CHAR)+1];
	memset(pszLine, 0x00, strlen(pszCommand)+sizeof(LINE_END_CHAR)+1);

	sprintf(pszLine, "%s%s", pszCommand, LINE_END_CHAR);

	//SendRawData는 종료문자를 빼고, 데이터를 보낸다.
	//LINE_END_CHAR의 사이즈에도 종료문자가 포함되어 있어서 빼야한다.
	//pszCommand의 사이즈느 strlen를 사용하면, 종료문자 제외한 길이가 리턴된다.
	int iResult = SendRawData(pszLine, strlen(pszCommand)+(sizeof(LINE_END_CHAR)-1));

	/*DEBUG*/
	if(iResult>0)
	{
		printf("S => %s", pszLine);
		WriteFtpCommandLog("S => %s", pszLine);
	}
	/**/

	delete [] pszLine;
	pszLine = NULL;

	return iResult;
}

int CCommandSocket::RecvCommandResultData()
{
	//Socket에서 직접 읽어오는 버퍼임....
	char szDataBuffer[MAX_SOCKET_BUFFER];
	//이전에 짤렸던 Data를 보관하고 있는 장소임..
	char szRemainDataBuffer[MAX_LINE_LENGTH];
	memset(szRemainDataBuffer, 0x00, sizeof(szRemainDataBuffer));
	//소켓에서 받은 라인 중, 완성된 최종라인을 보관하는 장소임..
	char szLastLine[MAX_LINE_LENGTH];

	int iTotRecvLen = 0;
	int iRecvLen = 0;

	char szCodeCheckBuffer[MAX_CODE_CHECK_BUFFER];
	memset(szCodeCheckBuffer, 0x00, sizeof(szCodeCheckBuffer));

	//먼저.. 데이터를 읽어온다.
	while(1)
	{
		memset(szDataBuffer, 0x00, sizeof(szDataBuffer));
		iRecvLen = RecvRawData(szDataBuffer, sizeof(szDataBuffer)-1, m_iResponseTimeOut);
		if(iRecvLen<0)
			return iRecvLen; //Error Code

		//전체 받은 데이터를 합한다....
		iTotRecvLen += iRecvLen;

		/*DEBUG*/
		//		if(iRecvLen>0)
		//			printf("R => %s", szDataBuffer);
		/**/

		//Data를 받았으면, 이전에 받았던 불완전한 데이터의 뒤에붙인다.
		if(strlen(szRemainDataBuffer)>0)
			strcat(szRemainDataBuffer, szDataBuffer);
		else
			sprintf(szRemainDataBuffer, "%s", szDataBuffer);

		//이하 szRemainDataBuffer를 가지고 작업한다.

		//마지막 라인을 Return한다.
		memset(szLastLine, 0x00, sizeof(szLastLine));
		if(GetLastLineInRecv(szRemainDataBuffer, szLastLine)==FALSE)
			return 0; //szRemainDataBuffer 데이터가 없을 경우에만, FALSE임..
		//=> 따라서 받은 데이터가 없음.

		//저장하기 전에 검사한다.
		if((unsigned int)iTotRecvLen>sizeof(szCodeCheckBuffer))
		{
			printf("!!! too many data from server. Length=>%d\n", iTotRecvLen);
			printf("%s", szCodeCheckBuffer);
			printf("%s", szRemainDataBuffer);
			return RETURN_FAIL;
		}
		//여기서 받아놓은 데이터를 전체 버퍼에 저장한다.
		if(strlen(szCodeCheckBuffer)>0)
			strcat(szCodeCheckBuffer, szRemainDataBuffer);
		else
			sprintf(szCodeCheckBuffer, "%s", szRemainDataBuffer);

		//먼저 라인이 마지막까지 들어왔는지 검사한다.
		//라인이 짤렸으면, 현재 가지고 있는 데이터(szLastLine)의 뒤에 소켓의 데이터를 붙여야 한다.
		memset(szRemainDataBuffer, 0x00, sizeof(szRemainDataBuffer));
		if(IsRecvLineComplete(szLastLine, strlen(szLastLine))==FALSE)
		{
			sprintf(szRemainDataBuffer, "%s", szLastLine);
			continue;
		}

		//마지막 데이터 인지 검사한다.
		if(IsRecvLastLine(szLastLine)==TRUE)
			break;
	}

	// 주의: 큰 배열들이 스택에서 선언되어 자칫 스택 오버플로가 발생할 수 있다.
	//		발생할 경우 MAX_SOCKET_BUFFER 값을 적당히 줄여주자. 너무 줄이면 전송속도가 느려짐을 염두에 두자.
	InsertEachCommand(szCodeCheckBuffer);

	return iTotRecvLen;
}

//1Line이 짤려서 들어왔을 경우,
//1Line이 딱 맞추어서 들어왔을 경우,
//2Line이상이 걸려서 들어왔을 경우..
BOOL CCommandSocket::IsRecvLineComplete(char *pszLine, int iSize/*종료문자제외.*/)
{
	if(iSize<=0)
		return FALSE;

	//마지막 두바이트가 종료문자인지.......
	if((pszLine[iSize-2]==LINE_END_CR)&&(pszLine[iSize-1]==LINE_END_LF))
		return TRUE;

	return FALSE;
}

//1Line을 입력받고, 마지막 Line인지를 검사한다.
BOOL CCommandSocket::IsRecvLastLine(char *pszLine)
{
	if(strlen(pszLine)<CODE_LENGTH)
		return FALSE;

	if(pszLine[CODE_LENGTH-1]=='-')
		return FALSE;

	return TRUE;
}

//받은데이터 중에 마지막 라인을 반환한다.
BOOL CCommandSocket::GetLastLineInRecv(char *pszSource, char *pszLast)
{
	if(strlen(pszSource)<=0)
		return FALSE;

	char* pToken = NULL;
	int iNextPointer = 0;
	int iLastPointer = 0;

	while((pToken = strstr(pszSource+iNextPointer, LINE_END_CHAR))!=NULL)
	{
		iLastPointer = iNextPointer;
		iNextPointer += (pToken-(pszSource+iNextPointer))+(sizeof(LINE_END_CHAR)-1);
	}

	sprintf(pszLast, "%s", pszSource+iNextPointer);
	if(strlen(pszLast)==0) //마지막 라인까지 온전하게 데이터가 들어온것이다.
		sprintf(pszLast, "%s", pszSource+iLastPointer);

	return TRUE;
}

int CCommandSocket::InsertEachCommand(char *pszCommandsBuffer)
{
	if(strlen(pszCommandsBuffer)<=0)
		return RETURN_FAIL;

	char szNowCommand[MAX_LINE_LENGTH]; //1라인 읽어서 저장하는 버퍼..
	memset(szNowCommand, 0x00, sizeof(szNowCommand));
	char szOneCommand[MAX_CODE_CHECK_BUFFER]; //같은 Code에 대한 결과를 모아놓은 버퍼..
	memset(szOneCommand, 0x00, sizeof(szOneCommand));

	int iPreCode = 0;
	int iNowCode = 0;

	char* pToken = NULL;
	int iNextPointer = 0;
	int iLastPointer = 0;

	while((pToken = strstr(pszCommandsBuffer+iNextPointer, LINE_END_CHAR))!=NULL)
	{
		iLastPointer = iNextPointer;
		iNextPointer += 
			(pToken-(pszCommandsBuffer+iNextPointer))+(sizeof(LINE_END_CHAR)-1);

		//한줄 읽어준다...
		memset(szNowCommand, 0x00, sizeof(szNowCommand));
		memcpy(szNowCommand, pszCommandsBuffer+iLastPointer, iNextPointer-iLastPointer);
		//읽어 온 것에서 코드를 읽어준다..
		if(strlen(szNowCommand)>0)
			sscanf(szNowCommand, "%d", &iNowCode);

		//이전 코드와 같지 않으면, 이전 코드값들을 Vector에 저장한다.
		if(iNowCode!=iPreCode)
		{
			//이전코드로 저장한 데이터가 있어야 한다.
			if(strlen(szOneCommand)>0)
			{
				//메모리를 생성하고, 복사한다.
				char* pszCommand = new char[strlen(szOneCommand)+1];
				sprintf(pszCommand, "%s", szOneCommand);
				//pszCommand의 Pointer를 vector에 저장한다.
				m_vectorCommand.push_back(pszCommand);
			}
			memset(szOneCommand, 0x00, sizeof(szOneCommand));
			sprintf(szOneCommand, "%s", szNowCommand);
		}
		else
		{
			if(strlen(szOneCommand)>0)
				strcat(szOneCommand, szNowCommand);
			else
				sprintf(szOneCommand, "%s", szNowCommand);
		}
		iPreCode = iNowCode;
	}

	if(strlen(szOneCommand)>0)
	{
		//메모리를 생성하고, 복사한다.
		char* pszCommand = new char[strlen(szOneCommand)+1];
		sprintf(pszCommand, "%s", szOneCommand);
		//pszCommand의 Pointer를 vector에 저장한다.
		m_vectorCommand.push_back(pszCommand);
		//저장한 후에, 메모리를 클리어 하고 모두 초기화 한다.
		memset(szOneCommand, 0x00, sizeof(szOneCommand));
	}

	return RETURN_GOOD;
}

int CCommandSocket::GetCommandCodeFromServer(int* pIntCode, char *pszCodeString/*=NULL*/)
{
	//Command Vector가 비었으면, Data를 읽어와야 하는데..
	if(m_vectorCommand.empty()==true)
	{
		//소켓에도 데이터가 없다면.. 실패이다....
		if(RecvCommandResultData()<=0)
			return RETURN_FAIL;
	}

	ITR_COMMAND_RESULT_VEC itrCommand = m_vectorCommand.begin();

	if(itrCommand==m_vectorCommand.end())
		return RETURN_FAIL;

	if((*itrCommand)==NULL)
		return RETURN_FAIL;

	sscanf((*itrCommand), "%d", pIntCode);
	/*Debug*/
	WriteFtpCommandLog("R => %s", (*itrCommand));
	printf("R => %s", (*itrCommand));

	char		szTxt[1024];
	sprintf(szTxt,"R => %s\n", (*itrCommand));


	OutputDebugString(szTxt);
	/**/

	if(pszCodeString!=NULL)
		sprintf(pszCodeString, "%s", (*itrCommand));

	delete [] (*itrCommand);
	(*itrCommand) = NULL;
	m_vectorCommand.erase(itrCommand);

	return *pIntCode;
}

int CCommandSocket::RemoteChangeDirectory(char *pszPath)
{
	char szCommand[MAX_COMMAND_BUFFER];
	memset(szCommand, 0x00, sizeof(szCommand));

	sprintf(szCommand, "CWD %s", pszPath);
	if(SendCommandData(szCommand)<0)
	{
		printf("!!! Fail to Send 'CWD'..\n");
		return RETURN_FAIL;		
	}

	int iCode = 0;
	if(GetCommandCodeFromServer(&iCode)<0)
	{
		printf("!!! Fail to Get CODE for 'CWD'..\n");
		return RETURN_FAIL;		
	}

	//패스워드 없이 그냥 패스됨...
	if(iCode!=SUCCESS_COMMAND)
		return RETURN_FAIL;

	return RETURN_GOOD;
}

int CCommandSocket::SetTransferType(BOOL bBin/*=FLASE*/)
{
	char szCommand[MAX_COMMAND_BUFFER];
	memset(szCommand, 0x00, sizeof(szCommand));

	if(bBin==FALSE)
		sprintf(szCommand, "TYPE A");
	else
		sprintf(szCommand, "TYPE I");

	if(SendCommandData(szCommand)<0)
	{
		printf("!!! Error Send Data...[TYPE A]\n");
		return RETURN_FAIL;
	}
	int iCode = 0;
	if(GetCommandCodeFromServer(&iCode)<0)
	{
		printf("!!! Error Recv Open Connection For Dir-List Message\n");
		return RETURN_FAIL;
	}
	if(iCode!=CHANGE_TYPE_SUCCESS)
	{
		printf("!!! Error it's not code=>%d\n", iCode);
		return RETURN_FAIL;
	}

	return RETURN_GOOD;
}

int CCommandSocket::GetRemoteFileSize(char *pszFileName)
{
	char szCommand[MAX_COMMAND_BUFFER];
	memset(szCommand, 0x00, sizeof(szCommand));
	sprintf(szCommand, "SIZE %s", pszFileName);

	if(SendCommandData(szCommand)<0)
	{
		printf("!!! Error Send Data...[%s]\n", szCommand);
		return RETURN_FAIL;
	}

	char szReturnString[MAX_LINE_LENGTH];
	memset(szReturnString, 0x00, sizeof(szReturnString));

	int iCode = 0;
	if(GetCommandCodeFromServer(&iCode, szReturnString)<0)
	{
		printf("!!! Error Recv %s\n", szCommand);
		return RETURN_FAIL;
	}
	if(iCode!=TARGET_FILE_SIZE)
	{
		printf("!!! Error it's not code=>%s\n", szReturnString);
		return RETURN_FAIL;
	}

	int iFileSize = 0;
	sscanf(szReturnString+CODE_LENGTH, "%d", &iFileSize);
	if(iFileSize<=0)
	{
		printf("!!! FileSize is Error => %d\n", iFileSize);
		return RETURN_FAIL;
	}

	return iFileSize;
}

////////////////////////////////////////////////////////////
// 데이터를 다 보냈다는 정보를 command connection에서 받는다.
int CCommandSocket::ConfirmEndTransfer()
{
	int iCode = 0;

	//time-out, socket error 모두 잡음..
	//Code가 에러로 오면 while문에서 탈출...
	//===> time-out도 잡기 위해서는 0도 Check를 해야한다.
	if(SOCKET_ERROR>=GetCommandCodeFromServer(&iCode))
	{
		printf("!!! Error On Result from command socket=>%d\n", iCode);
		return RETURN_FAIL;
	}

	//과연 실패 일까?
	if(iCode!=TRANSFER_COMPLETE)
		return RETURN_FAIL;

	return RETURN_GOOD;
}

int CCommandSocket::ProcedureConnectDataSocket(char *pszCommand,
											   CDataSocket* pDataSocket)
{
	// Get Data Connection Port
	sockaddr_in		addr;
	int iPort = GetPortForDataConnection(TRUE,&addr);

	//Port번호가 0보다는 커야한다. 원래.. 1024보다 커야함..
	if(iPort<=0)
	{
		printf("!!! Port is less then zero => %d\n", iPort);
		return RETURN_FAIL;
	}



	int		iCode = 0;

	// 2007/10/08 by yuchi
	// 도메인네임이 아닌 PASV모드응답으로 서버로부터 전송된 IP로 접속을 시도합니다.
	if(pDataSocket->ConnectFtp(&addr,m_szServerName, iPort)<0)
	{
		printf("!!! Data Socket Connect Fail\n");
		return RETURN_FAIL;
	}
	/*
	// 도메인 네임으로 접속을 시도하면 안됩니다.
	// Data Connection 연결....
	if(pDataSocket->ConnectFtp(m_szServerName, iPort)<0)
	{
		printf("!!! Data Socket Connect Fail\n");
		return RETURN_FAIL;
	}
	*/




		// Send Command
		if(SendCommandData(pszCommand)<0)
		{
			printf("!!! Error Send Data...[%s]\n", pszCommand);
			return RETURN_FAIL;
		}
		//Connection을 Open하라는 메시지를 받아야 한다.

		if(GetCommandCodeFromServer(&iCode)<0)
		{
			printf("!!! Error Recv Open Connection For download\n");
			return RETURN_FAIL;
		}

		if(iCode == CONNECT_ERROR )
		{
			return iCode;
		}

		if(iCode!=OPEN_CONNECT_FOR_DIRLIST)
		{
			//printf("!!! Recv Error On Open Connection For download => %d\n", iCode);
			//return RETURN_FAIL;
		}

		return RETURN_GOOD;
}

int CCommandSocket::GetRemotePwd(char *pszRemotePwd)
{
	char szCommand[MAX_COMMAND_BUFFER];
	memset(szCommand, 0x00, sizeof(szCommand));

	sprintf(szCommand, "PWD");
	if(SendCommandData(szCommand)<0)
	{
		printf("!!! Error Send Data...[%s]\n", szCommand);
		//		MessageBox( NULL, "101", "getremotepwd", MB_OK );
		return RETURN_FAIL;
	}

	char szReturnString[MAX_LINE_LENGTH];
	memset(szReturnString, 0x00, sizeof(szReturnString));
	int iCode = 0;
	if(GetCommandCodeFromServer(&iCode, szReturnString)<0)
	{
		printf("!!! Error Recv %s\n", szCommand);
		//		MessageBox( NULL, "102", "getremotepwd", MB_OK );
		return RETURN_FAIL;
	}
	if(iCode!=PRESENT_WORKING_DIR)	//KES 가끔 에러!! iCode 가 200 (257일때 성공) 실패시 connect부터 다시하면 어떨까.
	{
		printf("!!! Error it's not code=>%s\n", szReturnString);
		//		char buff[64];
		//		wsprintf( buff, "iCode:%d, %d, %s", iCode, PRESENT_WORKING_DIR, szReturnString );
		//		MessageBox( NULL, buff, "getremotepwd", MB_OK );
		return RETURN_FAIL;
	}

	CFtpFile ftpFile;
	if(ftpFile.GetRemoteDirectoryString(szReturnString, pszRemotePwd)==FALSE)
	{
		printf("!!! Error Directory Name =>%s \n", szReturnString);
		//		MessageBox( NULL, "104", "getremotepwd", MB_OK );
		return RETURN_FAIL;
	}

	return RETURN_GOOD;
}

int CCommandSocket::RemoteDeleteFile(char *pszFileName)
{
	char szCommand[MAX_COMMAND_BUFFER];
	memset(szCommand, 0x00, sizeof(szCommand));

	sprintf(szCommand, "DELE %s", pszFileName);
	if(SendCommandData(szCommand)<0)
	{
		printf("!!! Error Send Data...[%s]\n", szCommand);
		return RETURN_FAIL;
	}

	int iCode = 0;
	if(GetCommandCodeFromServer(&iCode)<0)
	{
		printf("!!! Error Recv %d\n", iCode);
		return RETURN_FAIL;
	}
	if(iCode!=SUCCESS_COMMAND)
	{
		printf("!!! Error it's not code=>%d\n", iCode);
		return RETURN_FAIL;
	}

	return RETURN_GOOD;
}

int CCommandSocket::RemoteRenameFile(char *pszSourceFileName, char *pszDestFileName)
{
	char szCommand[MAX_COMMAND_BUFFER];
	memset(szCommand, 0x00, sizeof(szCommand));
	sprintf(szCommand, "RNFR %s", pszSourceFileName);
	if(SendCommandData(szCommand)<0)
	{
		printf("!!! Error Send Data...[%s]\n", szCommand);
		return RETURN_FAIL;
	}

	int iCode = 0;
	if(GetCommandCodeFromServer(&iCode)<0)
	{
		printf("!!! Error Recv %d\n", iCode);
		return RETURN_FAIL;
	}
	if(iCode!=WAIT_NEXT_COMMAND)
	{
		printf("!!! Error it's not code=>%d\n", iCode);
		return RETURN_FAIL;
	}

	memset(szCommand, 0x00, sizeof(szCommand));
	sprintf(szCommand, "RNTO %s", pszDestFileName);
	if(SendCommandData(szCommand)<0)
	{
		printf("!!! Error Send Data...[%s]\n", szCommand);
		return RETURN_FAIL;
	}

	if(GetCommandCodeFromServer(&iCode)<0)
	{
		printf("!!! Error Recv %d\n", iCode);
		return RETURN_FAIL;
	}
	if(iCode!=SUCCESS_COMMAND)
	{
		printf("!!! Error it's not code=>%d\n", iCode);
		return RETURN_FAIL;
	}

	return RETURN_GOOD;
}

void CCommandSocket::WriteFtpCommandLog(const char *pszFormat, ...)
{
	/*
#ifdef	__UNIX__
	struct timeb tbCurrent;
	ftime(&tbCurrent);
#else
	struct _timeb tbCurrent;
	_ftime(&tbCurrent);
#endif

	struct tm* pTimeCurrent = localtime(&(tbCurrent.time));

	char szTimeLog[256];
	memset(szTimeLog, 0x00, sizeof(szTimeLog));
	sprintf(szTimeLog,"[%02d:%02d:%02d.%03d] ",
		pTimeCurrent->tm_hour, pTimeCurrent->tm_min, 
		pTimeCurrent->tm_sec, tbCurrent.millitm);

	char szLogFileName[MAX_PATH];
	memset(szLogFileName, 0x00, sizeof(szLogFileName));
	sprintf(szLogFileName, "%sCtmpFtpProtocol-%04d%02d%02d.log",
		FTPLOG_PATH_NAME,
		pTimeCurrent->tm_year+1900,
		pTimeCurrent->tm_mon+1,
		pTimeCurrent->tm_mday);

	FILE* pLogFile;
	va_list vArgs;	
	va_start(vArgs, pszFormat);

	pLogFile = fopen(szLogFileName, "a+b");
	if(pLogFile!=NULL)
	{
		fprintf(pLogFile, "%s", (char*)szTimeLog);
		vfprintf(pLogFile, pszFormat, vArgs);
		fclose(pLogFile);
	}
	va_end(vArgs);
	*/
}

void CCommandSocket::SetResponseTimeOut(int iWaitLimit)
{
	m_iResponseTimeOut = iWaitLimit;
}