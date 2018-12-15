// FtpClient.cpp: implementation of the CFtpClient class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FtpClient.h"
#include "FtpFile.h"
#include "common.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFtpClient::CFtpClient()
{
	InitVariables();
}

CFtpClient::~CFtpClient()
{
	SafeCloseDataSocket();
	SafeCloseCommandSocket();

	printf("!!!!!!!!!!!! Destroy FtpClient\n");
}

BOOL CFtpClient::InitalizeCheckCommandSocket(BOOL bCreate/*=FALSE*/)
{
	// 무조건 만들라고 했을 경우, 삭제하고 다시 만든다. 무조건 Return True이다.
	if(bCreate==TRUE)
	{
		if(m_pCommandSocket!=NULL)
		{
			delete m_pCommandSocket;
			m_pCommandSocket = NULL;
		}
		m_pCommandSocket = new CCommandSocket;

		/*여기서 변수들을 설정해주어야 한다. 1차로..

		*/

			return TRUE;
	}
	//bCreate = FALSE 일 경우...NULL을 Check한다.
	else
	{
		if(m_pCommandSocket==NULL)
			return FALSE;
		else
			return TRUE;
	}

	return FALSE;
}

void CFtpClient::SafeCloseCommandSocket()
{
	if(m_pCommandSocket!=NULL)
	{
		//		m_pCommandSocket->CloseFtp();
		//		m_pCommandSocket->CloseConnection();
		//		m_pCommandSocket->ConfirmEndTransfer();
		//		m_pCommandSocket->
		delete m_pCommandSocket;
		m_pCommandSocket = NULL;
	}
}

BOOL CFtpClient::InitalizeCheckDataSocket(BOOL bCreate)
{
	// 무조건 만들라고 했을 경우, 삭제하고 다시 만든다. 무조건 Return True이다.
	if(bCreate==TRUE)
	{
		if(m_pDataSocket!=NULL)
		{
			delete m_pDataSocket;
			m_pDataSocket = NULL;
		}
		m_pDataSocket = new CDataSocket;
		return TRUE;
	}
	//bCreate = FALSE 일 경우...NULL을 Check한다.
	else
	{
		if(m_pDataSocket==NULL)
			return FALSE;
		else
			return TRUE;
	}

	return FALSE;
}

void CFtpClient::SafeCloseDataSocket()
{
	if(m_pDataSocket!=NULL)
	{
		delete m_pDataSocket;
		m_pDataSocket = NULL;
	}
}

int CFtpClient::ConnectFtp(const char *pszServerName, unsigned int uiPort, int iTimeOut/*=0*/)
{
	if(InitalizeCheckCommandSocket(TRUE)==FALSE)
		return RETURN_FAIL;

	//memset(m_szServerName, 0x00, sizeof(m_szServerName));
	//m_uiServerPort = 0;

	sprintf(m_szServerName, "%s", pszServerName);
	m_uiServerPort = uiPort;

	if(iTimeOut!=0)
		m_pCommandSocket->SetResponseTimeOut(iTimeOut);

	return m_pCommandSocket->ConnectFtp(pszServerName, uiPort);
}

int CFtpClient::LogIn( const char *pszUserId, const char *pszPassword)
{
	if(InitalizeCheckCommandSocket()==FALSE)
		return RETURN_FAIL;

	//memset(m_szUserId, 0x00, sizeof(m_szUserId));
	//memset(m_szPassword, 0x00, sizeof(m_szPassword));

	//sprintf(m_szUserId, "%s", pszUserId);
	//sprintf(m_szPassword, "%s", pszPassword);

	strcpy( m_szUserId, pszUserId );
	strcpy( m_szPassword, pszPassword );

	return m_pCommandSocket->LogIn(pszUserId, pszPassword);
}

int CFtpClient::GetRemoteFileList(char* pszFileList/*=NULL*/, unsigned int uiBufferSize/*=MAX_LINE_LENGTH*/)
{
	if(InitalizeCheckCommandSocket()==FALSE)
		return RETURN_FAIL;

	////////////////////////////////////////////////////////////
	// Type 변경..
	if(m_pCommandSocket->SetTransferType(FALSE)<0)
		return RETURN_FAIL;

	////////////////////////////////////////////////////////////
	// Data Connection 연결..
	InitalizeCheckDataSocket(TRUE);

	char szCommand[MAX_COMMAND_BUFFER];
	memset(szCommand, 0x00, sizeof(szCommand));

	sprintf(szCommand, "NLST");
	if(m_pCommandSocket->ProcedureConnectDataSocket(szCommand, m_pDataSocket)<0)
	{
		SafeCloseDataSocket();
		return RETURN_FAIL;
	}

	////////////////////////////////////////////////////////////
	// Fetch the data, list of files..
	char szRecvBuffer[MAX_SOCKET_BUFFER];
	memset(szRecvBuffer, 0x00, sizeof(szRecvBuffer));

	int iTotRecvLength = 0;
	int iRecvLength = 0;
	while(TRUE)
	{
		//데이터가 어디가 끝인지 구분자가 없다.
		//그냥 socket에 있는데로 다 꺼내온다. 없으면, 나간다..
		//단 뭐라도 하나 있어야 한다
		if((iRecvLength==0)&&(iTotRecvLength>0))
			break;

		iRecvLength = m_pDataSocket->RecvInformationData(szRecvBuffer, sizeof(szRecvBuffer)-1);
		//에러상황이 확실하다......
		if(iRecvLength<0)
		{
			SafeCloseDataSocket();
			m_pCommandSocket->ConfirmEndTransfer();
			return RETURN_FAIL;
		}

		iTotRecvLength += iRecvLength;
		/*DEBUG*/
		if(strlen(szRecvBuffer)>0)
			printf("%s", szRecvBuffer);

		if(pszFileList!=NULL)
		{
			//버퍼 사이즈 만큼만 담는다...입력받은 사이즈는 종료문자 제외한 크기임.
			if((strlen(pszFileList)+(unsigned int)iRecvLength)<=uiBufferSize)
			{
				if(strlen(pszFileList)>0)
					strcat(pszFileList, szRecvBuffer);
				else
					sprintf(pszFileList, "%s", szRecvBuffer);
			}
		}
		/**/
		memset(szRecvBuffer, 0x00, sizeof(szRecvBuffer));
	}

	/*DEBUG*/
	printf("TOTAL LENGTH from DATA CONNECTION => %d bytes\n", iTotRecvLength);
	/**/
	// End Fetch the data, list of files..
	////////////////////////////////////////////////////////////

	SafeCloseDataSocket();

	return m_pCommandSocket->ConfirmEndTransfer();
}

int CFtpClient::RemoteChangeDirectory(char *pszPath)
{
	if(InitalizeCheckCommandSocket()==FALSE)
		return RETURN_FAIL;

	return m_pCommandSocket->RemoteChangeDirectory(pszPath);
}

int CFtpClient::DownloadFile(char *pszFileName, char *pszDownloadPath, BOOL bBinMode/*=FALSE*/)
{
	if(InitalizeCheckCommandSocket()==FALSE)
		//return 1;
		return RETURN_FAIL;

	/////////////////////////////////////////////////////////////////
	// Local에서 사용할 이름을 만들기......
	char szDownFullName[MAX_PATH];
	memset(szDownFullName, 0x00, sizeof(szDownFullName));

	if(pszDownloadPath!=NULL)
	{
		CFtpFile ftpFile;
		if(ftpFile.CheckDownloadPath(pszDownloadPath))
			sprintf(szDownFullName, "%s%s", pszDownloadPath, pszFileName);
		else
#ifdef __UNIX__
			sprintf(szDownFullName, "%s/%s", pszDownloadPath, pszFileName);
#else
			sprintf(szDownFullName, "%s\\%s", pszDownloadPath, pszFileName);
#endif
	}
	else
		sprintf(szDownFullName, "%s", pszFileName);
	/////////////////////////////////////////////////////////////////

	FILE* pTestFile = fopen(szDownFullName, "r");
	if(pTestFile!=NULL)
	{
		printf("!!! File is already exsit => %s, errno=>%d, errstr=>%s\n",
			szDownFullName, errno, strerror(errno));
		fclose(pTestFile);
		DeleteFile( szDownFullName );
		//return RETURN_FAIL;
	}

	////////////////////////////////////////////////////////////
	// 파일 이름 만들기..
	char szRemoteDirectory[MAX_PATH];
	memset(szRemoteDirectory, 0x00, sizeof(szRemoteDirectory));
	if(m_pCommandSocket->GetRemotePwd(szRemoteDirectory)<0)
		return RETURN_FAIL;
	//		return 2;

	char szTargetName[MAX_PATH];
	memset(szTargetName, 0x00, sizeof(szTargetName));
	sprintf(szTargetName, "%s%s", szRemoteDirectory, pszFileName);
	// End 파일 이름 만들기..
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	// Type 변경..
	if(m_pCommandSocket->SetTransferType(bBinMode)<0)
		return RETURN_FAIL;
	//		return 3;

	int iFileSize = m_pCommandSocket->GetRemoteFileSize(szTargetName);
	if(iFileSize<=0)
		return RETURN_FAIL;
	//		return 4;

	InitalizeCheckDataSocket(TRUE);

	////////////////////////////////////////////////////////////
	// Data Connection 연결..
	char szCommand[MAX_COMMAND_BUFFER];
	memset(szCommand, 0x00, sizeof(szCommand));
	sprintf(szCommand, "RETR %s", szTargetName);

	if(m_pCommandSocket->ProcedureConnectDataSocket(szCommand, m_pDataSocket)<0)
	{
		SafeCloseDataSocket();
		m_pCommandSocket->ConfirmEndTransfer();
		return RETURN_FAIL;
		//return 5;
	}

	////////////////////////////////////////////////////////////
	// TargetFile 생성..
	//꼭 바이너리 파일로 열어야 한다.
	FILE* pTargetFile = fopen(szDownFullName, "wb");
	if(pTargetFile==NULL)
	{
		printf("!!! File is exsit => %s, errno=>%d, errstr=>%s\n",
			szDownFullName, errno, strerror(errno));
		SafeCloseDataSocket();
		m_pCommandSocket->ConfirmEndTransfer();
		return RETURN_FAIL;
		//return 6;
	}
	// End TargetFile 생성..
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	// Fetch the data, list of files..
	char szRecvBuffer[MAX_SOCKET_BUFFER];
	memset(szRecvBuffer, 0x00, sizeof(szRecvBuffer));

	int iTotRecvLength = 0;
	int iRecvLength = 0;
	while(TRUE)
	{
		//정해진 길이만큼 다 읽어온다.
		if(iTotRecvLength>=iFileSize)
			break;

		iRecvLength = m_pDataSocket->RecvInformationData(szRecvBuffer, sizeof(szRecvBuffer)-1);

		if(iRecvLength<0)
		{
			printf("!!! Recv Error On download Files => %d\n", iRecvLength);
			fclose(pTargetFile);
			remove(szDownFullName);
			SafeCloseDataSocket();
			m_pCommandSocket->ConfirmEndTransfer();
			return RETURN_FAIL;
			//return 7;
		}

		if(iRecvLength>0)
			fwrite(szRecvBuffer, sizeof(char), iRecvLength, pTargetFile);

		iTotRecvLength += iRecvLength;
		memset(szRecvBuffer, 0x00, sizeof(szRecvBuffer));

		if(m_bHashMark)
			printf("#");

		if(m_iUpDownSleepTime>0)
#ifdef __UNIX__
			UnixSleep(m_iUpDownSleepTime);
#else
			Sleep(m_iUpDownSleepTime);
#endif
	}
	printf("\n");

	fclose(pTargetFile);

	/*DEBUG*/
	printf("TOTAL LENGTH from DATA CONNECTION => %d bytes\n", iTotRecvLength);
	/**/
	// End Fetch the data, list of files..
	////////////////////////////////////////////////////////////

	SafeCloseDataSocket();


	return m_pCommandSocket->ConfirmEndTransfer();
}


int CFtpClient::UploadFile(char *pszFileName, BOOL bBinMode/*=FALSE*/)
{
	if(InitalizeCheckCommandSocket()==FALSE)
		return RETURN_FAIL;

	FILE* pTestFile = fopen(pszFileName, "r");
	if(pTestFile==NULL)
	{
		printf("!!! File is not exsit => %s, errno=>%d, errstr=>%s\n",
			pszFileName, errno, strerror(errno));
		return RETURN_FAIL;
	}

	////////////////////////////////////////////////////////////
	// 파일 이름 만들기..
	CFtpFile ftpFile;
	char* pszOnlyFileName = ftpFile.GetPureFileName(pszFileName);

	char szRemoteDirectory[MAX_PATH];
	memset(szRemoteDirectory, 0x00, sizeof(szRemoteDirectory));
	if(m_pCommandSocket->GetRemotePwd(szRemoteDirectory)<0)
		return RETURN_FAIL;

	char szTargetName[MAX_PATH];
	memset(szTargetName, 0x00, sizeof(szTargetName));
	sprintf(szTargetName, "%s%s", szRemoteDirectory, pszOnlyFileName);

	////////////////////////////////////////////////////////////
	// Type 변경..
	if(m_pCommandSocket->SetTransferType(bBinMode)<0)
		return RETURN_FAIL;

	InitalizeCheckDataSocket(TRUE);

	////////////////////////////////////////////////////////////
	// Data Connection 연결..
	char szCommand[MAX_COMMAND_BUFFER];
	memset(szCommand, 0x00, sizeof(szCommand));
	sprintf(szCommand, "STOR %s", szTargetName);

	if(m_pCommandSocket->ProcedureConnectDataSocket(szCommand, m_pDataSocket)<0)
	{
		SafeCloseDataSocket();
		m_pCommandSocket->ConfirmEndTransfer();
		return RETURN_FAIL;
	}

	////////////////////////////////////////////////////////////
	// 파일 오픈....
	//나중에 Rename이 가능하도록 바꿔야 하나?? 필요없을 듯..
	FILE* pTargetFile = fopen(pszFileName, "rb");
	if(pTargetFile==NULL)
	{
		printf("!!! File is not exsit => %s\n", pszFileName);
		SafeCloseDataSocket();
		m_pCommandSocket->ConfirmEndTransfer();
		return RETURN_FAIL;
	}

	////////////////////////////////////////////////////////////
	// Fetch the data, list of files..
	char szSendBuffer[MAX_SOCKET_BUFFER];
	memset(szSendBuffer, 0x00, sizeof(szSendBuffer));

	int iTotSendLength = 0;
	int iSendLength = 0;
	int iReadLength = 0;

	while(!feof(pTargetFile))
	{
		iReadLength = 
			fread(szSendBuffer, sizeof(char), sizeof(szSendBuffer)-1, pTargetFile);

		if(ferror(pTargetFile))
		{
			printf("!!! Send Error On Read File => %d, %s\n", errno, strerror(errno));
			fclose(pTargetFile);
			/*올라간 Remote파일을 지운다*/  //=== 추가해야 함...
			SafeCloseDataSocket();
			m_pCommandSocket->ConfirmEndTransfer();
			return RETURN_FAIL;
		}

		iSendLength = 
			m_pDataSocket->SendInformationData(szSendBuffer, iReadLength);

		if(iSendLength<0)
		{
			printf("!!! Send Error On upload Files => %d\n", iSendLength);
			fclose(pTargetFile);
			/*올라간 Remote파일을 지운다*/  //=== 추가해야 함...
			SafeCloseDataSocket();
			m_pCommandSocket->ConfirmEndTransfer();
			return RETURN_FAIL;
		}

		iTotSendLength += iSendLength;
		memset(szSendBuffer, 0x00, sizeof(szSendBuffer));

		if(m_bHashMark)
			printf("#");

		if(m_iUpDownSleepTime>0)
#ifdef __UNIX__
			UnixSleep(m_iUpDownSleepTime);
#else
			Sleep(m_iUpDownSleepTime);
#endif
	}
	printf("\n");

	fclose(pTargetFile);

	/*DEBUG*/
	printf("TOTAL LENGTH through out DATA CONNECTION => %d bytes\n", iTotSendLength);
	/**/
	// End Fetch the data, list of files..
	////////////////////////////////////////////////////////////

	SafeCloseDataSocket();

	return m_pCommandSocket->ConfirmEndTransfer();
}

int CFtpClient::GetRemoteCurrentDirectory(char *pszDirectoryName)
{
	if(InitalizeCheckCommandSocket()==FALSE)
		return RETURN_FAIL;

	char szRemoteDirectory[MAX_PATH];
	memset(szRemoteDirectory, 0x00, sizeof(szRemoteDirectory));
	if(m_pCommandSocket->GetRemotePwd(szRemoteDirectory)<0)
		return RETURN_FAIL;

	sprintf(pszDirectoryName, "%s", szRemoteDirectory);
	return RETURN_GOOD;
}


int CFtpClient::RemoteDeleteFile(char *pszFileName)
{
	if(InitalizeCheckCommandSocket()==FALSE)
		return RETURN_FAIL;

	return m_pCommandSocket->RemoteDeleteFile(pszFileName);
}

int CFtpClient::RemoteRenameFile(char *pszSourceFileName, char *pszDestFileName)
{
	if(InitalizeCheckCommandSocket()==FALSE)
		return RETURN_FAIL;

	return m_pCommandSocket->RemoteRenameFile(pszSourceFileName, pszDestFileName);
}

//Download전에 준비사항을 미리 해 놓는다.
//DATASOCKET 연결.. 
//TYPE 변경..
//저장할 파일을 local에 만들기..
int CFtpClient::SetupExtendDownload(char *pszFileName, char* pszDownloadPath, BOOL bBinMode/*=FALSE*/)
{
	///////////////////////////////////////////////////////////////////////////
	//아직 다중 스레드를 지원하지 않는다. 1개의 DataSocket만 사용한다.
	//따라서, 외부에서 두개의 Thread가 1개의 DLL이나 Class만 가지고 동시에 접근할 
	//경우 발생하는 버그를 없앤다.
	//나중에 여러개의 데이터 소켓을 지원하도록 변경할 수 있다.
	//DataSocket ID로 관리하며, SetupExtendDownload 호출시마다 1개씩 생성한다.
	if((strlen(m_szUploadFileName)>0)
		||(m_pUploadSaveFile!=NULL)
		||(m_iDownloadFileSize>0)
		||(strlen(m_szDownloadFileName)>0)
		||(m_pDownloadSaveFile!=NULL)
		||(m_iDownloadFileSize>0))
	{
		return RETURN_FAIL;
	}
	//End 아직 다중 스레드를 지원하지 않는다. 1개의 DataSocket만 사용한다.
	//////////////////////////////////////////////////////////////////////////

	//Command Socket을 확인한다.
	if(InitalizeCheckCommandSocket()==FALSE)
		return RETURN_FAIL;

	/////////////////////////////////////////////////////////////////
	// Local에서 사용할 이름을 만들기......
	char szDownFullName[MAX_PATH];
	memset(szDownFullName, 0x00, sizeof(szDownFullName));

	if(pszDownloadPath!=NULL)
	{
		CFtpFile ftpFile;
		if(ftpFile.CheckDownloadPath(pszDownloadPath))
			sprintf(szDownFullName, "%s%s", pszDownloadPath, pszFileName);
		else
#ifdef __UNIX__
			sprintf(szDownFullName, "%s/%s", pszDownloadPath, pszFileName);
#else
			sprintf(szDownFullName, "%s\\%s", pszDownloadPath, pszFileName);
#endif
	}
	else
		sprintf(szDownFullName, "%s", pszFileName);
	/////////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////
	// 파일 이름 만들기..
	char szRemoteDirectory[MAX_PATH];
	memset(szRemoteDirectory, 0x00, sizeof(szRemoteDirectory));
	if(m_pCommandSocket->GetRemotePwd(szRemoteDirectory)<0)
		return RETURN_FAIL;

	char szTargetName[MAX_PATH];
	memset(szTargetName, 0x00, sizeof(szTargetName));
	sprintf(szTargetName, "%s%s%s", szRemoteDirectory, pszFileName, ".zip");
	// End 파일 이름 만들기..
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	// Type 변경..
	if(m_pCommandSocket->SetTransferType(bBinMode)<0)
		return RETURN_FAIL;

	//사이즈 가져오기.....
	int iFileSize = m_pCommandSocket->GetRemoteFileSize(szTargetName);
	if(iFileSize<=0)
		return RETURN_FAIL;

	//데이터 소켓을 생성하고....
	InitalizeCheckDataSocket(TRUE);

	////////////////////////////////////////////////////////////
	// Data Connection 연결..
	char szCommand[MAX_COMMAND_BUFFER];
	memset(szCommand, 0x00, sizeof(szCommand));
	sprintf(szCommand, "RETR %s", szTargetName);

	if(m_pCommandSocket->ProcedureConnectDataSocket(szCommand, m_pDataSocket)<0)
	{
		SafeCloseDataSocket();
		SetExtendDownload(szDownFullName, NULL, iFileSize);
		return RETURN_FAIL;
	}

	////////////////////////////////////////////////////////////
	// TargetFile 생성..
	//꼭 바이너리 파일로 열어야 한다.
	FILE* pTargetFile = fopen( GAME_PATCH, "wb" );
	if( pTargetFile == NULL )
	{
		printf("!!! File is exsit => %s, errno=>%d, errstr=>%s\n",
			szDownFullName, errno, strerror(errno));
		SafeCloseDataSocket();
		SetExtendDownload(szDownFullName, pTargetFile, iFileSize);
		return RETURN_FAIL;
	}
	//	fclose(pTargetFile); ==> 닫지 않는다.. Release에서 닫아준다..
	// End TargetFile 생성..
	////////////////////////////////////////////////////////////

	SetExtendDownload(szDownFullName, pTargetFile, iFileSize);

	//정상일 경우의 리턴 값은 파일사이즈..
	return iFileSize;
}

void CFtpClient::SetExtendDownload(char *pszFileName, FILE *pFile, int iFileSize)
{
	sprintf(m_szDownloadFileName, "%s", pszFileName);
	m_pDownloadSaveFile = pFile;
	m_iDownloadFileSize = iFileSize;
}

void CFtpClient::ReleaseExtendDownload()
{
	//파일사이즈가 있다는 것은 일단 데이터소켓을 붙였었다는 얘기임..
	if(m_iDownloadFileSize>0)
	{
		SafeCloseDataSocket();
		m_pCommandSocket->ConfirmEndTransfer();
	}

	//오류 발생시에 로컬에 있는 파일은 지움....
	//삭제하기 전에 핸들을 닫아 줌..
	if(m_pDownloadSaveFile!=NULL)
	{
		fclose(m_pDownloadSaveFile);
		m_pDownloadSaveFile = NULL;
	}

	//오류 발생시에 로컬에 있는 파일은 지움....
	if(m_iLastDownloadError<RETURN_GOOD)
		remove(m_szDownloadFileName);

	//변수들 초기화..
	m_iDownloadFileSize = 0;
	memset(m_szDownloadFileName, 0x00, sizeof(m_szDownloadFileName));
	m_iLastDownloadError = 0;
	m_iTotRecvSize = 0;

}

//버퍼 사이즈는 실제 버퍼의 사이즈보다 1바이트 적게 입력받아야 한다.
//즉, iBufferSize는 실제 pszDownloadBuffer의 사이즈보다 1바이트 적은 값을 입력받아야 함.
int CFtpClient::WriteExtendDownloadFile()
{
	if((strlen(m_szDownloadFileName)<=0)
		||(m_pDownloadSaveFile==NULL)
		||(m_iDownloadFileSize<=0))
	{
		m_iLastDownloadError = RETURN_FAIL;
		return RETURN_FAIL;
	}

	char szRecvBuffer[MAX_SOCKET_BUFFER];
	memset(szRecvBuffer, 0x00, sizeof(szRecvBuffer));

	int iRecvLength = m_pDataSocket->RecvInformationData(szRecvBuffer, sizeof(szRecvBuffer)-1);
	if(iRecvLength<0)
	{
		m_iLastDownloadError = RETURN_FAIL;
		printf("!!! Recv Error On download Files => %d\n", iRecvLength);
		return RETURN_FAIL;
	}

	if((iRecvLength>0)&&(m_pDownloadSaveFile!=NULL))
		fwrite(szRecvBuffer, sizeof(char), iRecvLength, m_pDownloadSaveFile);

	//정해진 길이만큼 다 읽어온다.
	m_iLastDownloadError = RETURN_GOOD;
	m_iTotRecvSize += iRecvLength;

	return m_iTotRecvSize;
}


void CFtpClient::SetExtendUpload(char *pszFileName, FILE *pFile, int iFileSize)
{
	sprintf(m_szUploadFileName, "%s", pszFileName);
	m_pUploadSaveFile = pFile;
	m_iUploadFileSize = iFileSize;
}

void CFtpClient::ReleaseExtendUpload()
{
	//파일사이즈가 있다는 것은 일단 데이터소켓을 붙였었다는 얘기임..
	//Upload일 경우에도 동일하게 작동하게 하기 위해서 소스를 맞추었음...
	if(m_iUploadFileSize>0)
	{
		SafeCloseDataSocket();
		m_pCommandSocket->ConfirmEndTransfer();
	}

	//오류 발생시에 로컬에 있는 파일은 지움....
	//삭제하기 전에 핸들을 닫아 줌..
	if(m_pUploadSaveFile!=NULL)
	{
		fclose(m_pUploadSaveFile);
		m_pUploadSaveFile = NULL;
	}

	//오류 발생시에 리모트에 있는 파일은 지움 ==> 나중에.. 혹은 Client에서 지움...
	/*
	if(m_iLastUploadError<RETURN_GOOD)
		remove(m_szDownloadFileName);
	*/

		//변수들 초기화..
		m_iUploadFileSize = 0;
	memset(m_szUploadFileName, 0x00, sizeof(m_szUploadFileName));
	m_iLastUploadError = 0;
	m_iTotSendSize = 0;
}

//Upload전에  준비사항을 미리 해 놓는다.
//DATASOCKET 연결.. 
//TYPE 변경..
//올릴 파일 확인하기..
int CFtpClient::SetupExtendUpload(char *pszFileName, BOOL bBinMode/*=FALSE*/)
{
	///////////////////////////////////////////////////////////////////////////
	//아직 다중 스레드를 지원하지 않는다. 1개의 DataSocket만 사용한다.
	//따라서, 외부에서 두개의 Thread가 1개의 DLL이나 Class만 가지고 동시에 접근할 
	//경우 발생하는 버그를 없앤다.
	//나중에 여러개의 데이터 소켓을 지원하도록 변경할 수 있다.
	//DataSocket ID로 관리하며, SetupExtendUpload 호출시마다 1개씩 생성한다.
	if((strlen(m_szUploadFileName)>0)
		||(m_pUploadSaveFile!=NULL)
		||(m_iDownloadFileSize>0)
		||(strlen(m_szDownloadFileName)>0)
		||(m_pDownloadSaveFile!=NULL)
		||(m_iDownloadFileSize>0))
	{
		return RETURN_FAIL;
	}
	//End 아직 다중 스레드를 지원하지 않는다. 1개의 DataSocket만 사용한다.
	//////////////////////////////////////////////////////////////////////////

	//Command Socket을 확인한다.
	if(InitalizeCheckCommandSocket()==FALSE)
		return RETURN_FAIL;

	//파일이 있는지 확인한다.
	FILE* pTestFile = fopen(pszFileName, "r");
	if(pTestFile==NULL)
	{
		printf("!!! File is not exsit => %s, errno=>%d, errstr=>%s\n",
			pszFileName, errno, strerror(errno));
		return RETURN_FAIL;
	}

	////////////////////////////////////////////////////////////
	// 파일 이름 만들기..
	CFtpFile ftpFile;
	char* pszOnlyFileName = ftpFile.GetPureFileName(pszFileName);

	char szRemoteDirectory[MAX_PATH];
	memset(szRemoteDirectory, 0x00, sizeof(szRemoteDirectory));
	if(m_pCommandSocket->GetRemotePwd(szRemoteDirectory)<0)
		return RETURN_FAIL;

	char szTargetName[MAX_PATH];
	memset(szTargetName, 0x00, sizeof(szTargetName));
	sprintf(szTargetName, "%s%s", szRemoteDirectory, pszOnlyFileName);
	// End 파일 이름 만들기..
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	// Type 변경..
	if(m_pCommandSocket->SetTransferType(bBinMode)<0)
		return RETURN_FAIL;

	//사이즈 가져오기.....
	int iFileSize = 0;
#ifdef __UNIX__
	struct stat statFileSize;
	if(stat(pszFileName, &statFileSize)==0)
#else
	struct _stat statFileSize;
	if(_stat(pszFileName, &statFileSize)==0)
#endif
		iFileSize = statFileSize.st_size;

	if(iFileSize<=0)
		return RETURN_FAIL;
	//End 사이즈 가져오기.....

	//데이터 소켓을 생성하고....
	InitalizeCheckDataSocket(TRUE);

	////////////////////////////////////////////////////////////
	// Data Connection 연결..
	char szCommand[MAX_COMMAND_BUFFER];
	memset(szCommand, 0x00, sizeof(szCommand));
	sprintf(szCommand, "STOR %s", szTargetName);

	if(m_pCommandSocket->ProcedureConnectDataSocket(szCommand, m_pDataSocket)<0)
	{
		SafeCloseDataSocket();
		SetExtendUpload(pszFileName, NULL, iFileSize);
		return RETURN_FAIL;
	}

	////////////////////////////////////////////////////////////
	// SourceFile 열기...
	//꼭 바이너리 파일로 열어야 한다.
	FILE* pSourceFile = fopen(pszFileName, "rb");
	if(pSourceFile==NULL)
	{
		printf("!!! File is exsit => %s, errno=>%d, errstr=>%s\n",
			pszFileName, errno, strerror(errno));
		SafeCloseDataSocket();
		SetExtendUpload(pszFileName, pSourceFile, iFileSize);
		return RETURN_FAIL;
	}
	// End TargetFile 생성..
	////////////////////////////////////////////////////////////

	SetExtendUpload(pszFileName, pSourceFile, iFileSize);

	//정상일 경우의 리턴 값은 파일사이즈..
	return iFileSize;
}

//버퍼 사이즈는 실제 버퍼의 사이즈보다 1바이트 적게 입력받아야 한다.
//즉, iBufferSize는 실제 pszDownloadBuffer의 사이즈보다 1바이트 적은 값을 입력받아야 함.
int CFtpClient::SendExtendUploadFile()
{
	if((strlen(m_szUploadFileName)<=0)
		||(m_pUploadSaveFile==NULL)
		||(m_iUploadFileSize<=0))
	{
		m_iLastUploadError = RETURN_FAIL;
		return RETURN_FAIL;
	}

	//파일의 끝을 만나는 경우에도, 실패를 보낸다.
	//클라이언트에서 사이즈를 체크해서 읽을 것인지 말 것인지를 확인해야 한다.
	if(feof(m_pUploadSaveFile))
	{
		m_iLastUploadError = RETURN_FAIL;
		return RETURN_FAIL;
	}

	char szSendBuffer[MAX_SOCKET_BUFFER];
	memset(szSendBuffer, 0x00, sizeof(szSendBuffer));
	int iReadLength = 
		fread(szSendBuffer, sizeof(char), sizeof(szSendBuffer)-1, m_pUploadSaveFile);

	if(ferror(m_pUploadSaveFile))
	{
		printf("!!! Send Error On Read File => %d, %s\n", errno, strerror(errno));
		m_iLastUploadError = RETURN_FAIL;
		return RETURN_FAIL;
	}
	// End 파일에서 데이터 읽어오기......

	int iSendLength = 
		m_pDataSocket->SendInformationData(szSendBuffer, sizeof(szSendBuffer)-1);
	if(iSendLength<0)
	{
		m_iLastDownloadError = RETURN_FAIL;
		printf("!!! Recv Error On download Files => %d\n", iSendLength);
		return RETURN_FAIL;
	}

	//정해진 길이만큼 다 읽어온다.
	m_iLastDownloadError = RETURN_GOOD;
	m_iTotSendSize += iSendLength;

	return m_iTotSendSize;
}

void CFtpClient::SetHashMark(BOOL bHashMark/*=TRUE*/)
{
	m_bHashMark = bHashMark;
}

void CFtpClient::SetCommandResponseTimeOut(int iWaitLimit)
{
	if(InitalizeCheckCommandSocket()==FALSE)
		return;

	m_pCommandSocket->SetResponseTimeOut(iWaitLimit);
}

void CFtpClient::LogOut()
{
	SafeCloseDataSocket();
	//루나테스트
	//			FILE* pFile = fopen( "launcherdebug.txt", "at" );
	//			if( pFile )
	//			{
	//				fprintf( pFile, "****After SafeCloseDataSocket() ***\n" );
	//				fclose(pFile);
	//			}
	//---------
	SafeCloseCommandSocket(); //접속까지 종료시킴.....
	//루나테스트
	//			pFile = fopen( "launcherdebug.txt", "at" );
	//			if( pFile )
	//			{
	//				fprintf( pFile, "****After SafeCloseCommandSocket() ***\n" );
	//				fclose(pFile);
	//			}
	//---------
	//꼭 마지막에 호출해야 함..
	InitVariables();
}

void CFtpClient::InitVariables()
{
	m_iTotRecvSize = 0;
	m_iLastDownloadError = 0;
	m_iDownloadFileSize = 0;
	m_pDownloadSaveFile = NULL;
	memset(m_szDownloadFileName, 0x00, sizeof(m_szDownloadFileName));

	m_iTotSendSize = 0;
	m_iLastUploadError = 0;
	m_iUploadFileSize = 0;
	m_pUploadSaveFile = NULL;
	memset(m_szUploadFileName, 0x00, sizeof(m_szUploadFileName));

	m_iUpDownSleepTime = 0;
	m_bHashMark = FALSE;
	memset(m_szServerName, 0x00, sizeof(m_szServerName));
	m_uiServerPort = 0;
	memset(m_szUserId, 0x00, sizeof(m_szUserId));
	memset(m_szPassword, 0x00, sizeof(m_szPassword));

	m_pCommandSocket = NULL;
	m_pDataSocket = NULL;
}

void CFtpClient::SetTransferDataSpeed(int iSleepTime)
{
	m_iUpDownSleepTime = iSleepTime;
}
