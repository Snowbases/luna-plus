// FtpClient.h: interface for the CFtpClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTPCLIENT_H__9E0F785A_7A79_48D4_872E_16A23EE50601__INCLUDED_)
#define AFX_FTPCLIENT_H__9E0F785A_7A79_48D4_872E_16A23EE50601__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommandSocket.h"
#include "DataSocket.h"
#include "StdAfx.h"	// Added by ClassView

class CFtpClient  
{
public:
	void SetTransferDataSpeed(int iSleepTime);
	void LogOut(); //접속까지 종료시킴.....
	void SetCommandResponseTimeOut(int iWaitLimit);
	void SetHashMark(BOOL bHashMark=TRUE);
/*외부에서 Progressive bar등을 통해서 흐름을 관찰할 수 있는 기능을 제공함.*/

	//Download
	int SetupExtendDownload(char* pszFileName, char* pszDownloadPath, BOOL bBinMode=FALSE);
	int WriteExtendDownloadFile();
	void ReleaseExtendDownload();

	//Upload
	int SetupExtendUpload(char* pszFileName, BOOL bBinMode=FALSE);
	int SendExtendUploadFile();
	void ReleaseExtendUpload();
/*End 외부에서 Progressive bar등을 통해서 흐름을 관찰할 수 있는 기능을 제공함.*/

	int RemoteRenameFile(char* pszSourceFileName, char* pszDestFileName);
	int RemoteDeleteFile(char* pszFileName);
	int GetRemoteCurrentDirectory(char* pszDirectoryName);
	int UploadFile(char* pszFileName, BOOL bBinMode=FALSE);
	int DownloadFile(char* pszFileName,  char* pszDownloadPath, BOOL bBinMode=FALSE);
	int ConnectFtp(const char* pszServerName, unsigned int uiPort, int iTimeOut=0);
	int LogIn(const char* pszUserId, const char* pszPassword);
	int GetRemoteFileList(char* pszFileList=NULL, unsigned int uiBufferSize=MAX_LINE_LENGTH);
	int RemoteChangeDirectory(char* pszPath);

	CFtpClient();
	virtual ~CFtpClient();

private:
	void InitVariables();
	void SetExtendDownload(char* pszFileName, FILE* pFile, int iFileSize);
	void SetExtendUpload(char* pszFileName, FILE* pFile, int iFileSize);

	void SafeCloseDataSocket();
	BOOL InitalizeCheckDataSocket(BOOL bCreate=FALSE);
	void SafeCloseCommandSocket();
	BOOL InitalizeCheckCommandSocket(BOOL bCreate=FALSE);

	int m_iUpDownSleepTime;
	BOOL m_bHashMark;
	char m_szPassword[64];
	char m_szUserId[64];
	unsigned int m_uiServerPort;
	char m_szServerName[64];

	CCommandSocket* m_pCommandSocket;
	CDataSocket* m_pDataSocket;

//Exdtend Download 전용...
	FILE* m_pDownloadSaveFile;
	int m_iDownloadFileSize;
	int m_iTotRecvSize;
	char m_szDownloadFileName[MAX_PATH];
	int m_iLastDownloadError;
//End Exdtend Download 전용...

//Exdtend Upload 전용...
	FILE* m_pUploadSaveFile;
	int m_iUploadFileSize;
	int m_iTotSendSize;
	char m_szUploadFileName[MAX_PATH];
	int m_iLastUploadError;
//End Exdtend Upload 전용...
};

#endif // !defined(AFX_FTPCLIENT_H__9E0F785A_7A79_48D4_872E_16A23EE50601__INCLUDED_)
