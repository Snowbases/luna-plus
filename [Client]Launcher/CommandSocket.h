// CommandSocket.h: interface for the CCommandSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDSOCKET_H__AF25C9EB_C199_4708_8887_0684A67CDD6B__INCLUDED_)
#define AFX_COMMANDSOCKET_H__AF25C9EB_C199_4708_8887_0684A67CDD6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FtpClientSocket.h"
#include "DataSocket.h"

/*
#ifdef __UNIX__
const static char FTPLOG_PATH_NAME[] = {"./ctmp4log/"};
#else
const static char FTPLOG_PATH_NAME[] = {".\\ctmp4log\\"};
#endif
*/

class CCommandSocket : public CFtpClientSocket  
{
public:
	void SetResponseTimeOut(int iWaitLimit);
	int RemoteRenameFile(char* pszSourceFileName, char* pszDestFileName);
	int RemoteDeleteFile(char* pszFileName);
	int ConnectFtp(const char* pszServerName, unsigned int uiPort); //연결..
	int LogIn(const char* pszUserId, const char* pszPassword); //로그인..
	int GetRemotePwd(char* pszRemotePwd); //현재 폴더 경로를 읽어옴..
	int ProcedureConnectDataSocket(char* pszCommand, CDataSocket* pDataSocket); //DataSocket을 연결함.
	int ConfirmEndTransfer(); //DataSocket으로 데이터를 다 보냈다는 것을 확인함.
	int GetRemoteFileSize(char* pszFileName); //서버의 파일 사이즈를 읽어옮.
	int SetTransferType(BOOL bBin=FALSE); //전송 Type을 설정함.
	int RemoteChangeDirectory(char* pszPath); //서버의 작업폴더를 변경함..

	CCommandSocket();
	virtual ~CCommandSocket();

private:
	int m_iResponseTimeOut;
	void WriteFtpCommandLog(const char *pszFormat, ...);
	void CloseConnection();
	int GetCommandCodeFromServer(int* pIntCode, char* pszCodeString=NULL);
	int SendCommandData(char* pszCommand); //0이상일 경우, 성공임.

	// 2007/10/08 by yuchi

	int		GetPortForDataConnection(BOOL bPassiveMode,sockaddr_in* pOutAddr);

	//int GetPortForDataConnection(BOOL bPassiveMode=TRUE); //data Socket의 Port를 가져옴.


	int InsertEachCommand(char* pszCommandsBuffer);
	int RecvCommandResultData(); //0이상일 경우, 성공임.
	BOOL GetLastLineInRecv(char* pszSource, char* pszLast); //받은데이터 중에 마지막 라인을 반환한다.
	BOOL IsRecvLastLine(char* pszLine);
	BOOL IsRecvLineComplete(char *pszLine, int iSize); //iSize는 종료문자를 제외한 길이를 입력한다.
};

#endif // !defined(AFX_COMMANDSOCKET_H__AF25C9EB_C199_4708_8887_0684A67CDD6B__INCLUDED_)
