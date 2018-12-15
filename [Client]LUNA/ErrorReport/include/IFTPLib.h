#pragma once
/*
	간단한 ftp 라이브러리.

	만든자 : 이정훈
	고친자 : 이진영
	대충 고친 날짜 : 2010-03-08

	lib로 가져다쓸 경우 
	이정훈님에게 감사의 기도를 올린후,
	해당 라이브러리를 링크시키고,
	CreateFTPInterface() 로 인터페이스를 얻어온다.
*/


enum eRETURN_CODE
{
	eRETURN_CODE_INVALID,
	eRETURN_CODE_OK,
	eRETURN_CODE_USERNAME_
};

// 콜백.
typedef void (_stdcall *FileCallBack)(int iNow, int iMax, char* pszFileName);

// ftp 오리지날.
struct IFtpLib
{
	void	virtual Initialize() = 0;
	void	virtual	Cleanup() = 0;							// delete this.

	BOOL	virtual Connect(char* pszIP, WORD wPort) = 0;
	void	virtual Disconnect() = 0;
	BOOL	virtual GetIP_FromDNS(char* pszDNS, char* pszOutIP) = 0;

	BOOL	virtual CD_Down(char* pszName) = 0;				// change directory.
	BOOL	virtual CD_Up() = 0;

	BOOL	virtual Login(char* pszUser, char* pszPass) = 0;

	BOOL	virtual Download_File(char* pszDownloadRootPath, char* pszFileName, BOOL bActiveMode, FileCallBack pcbPart) = 0;
	BOOL	virtual Upload_File(char* pszUploadRootPath, char* pszFileName, BOOL bActiveMode, FileCallBack pcbPart) = 0;
	//bActiveMode (0:Passive, 1:Active)
};




HRESULT __stdcall CreateFTPInterface(IFtpLib** ppv);




// CreateFTPInterface 용.
HRESULT typedef (__stdcall *CREATE_FTP_INSTANCE_FUNC)(IFtpLib** ppv);

// dll 로 땡겨쓸 때 아래 코드를 쓰면 됨.
inline IFtpLib* CreateFTPLib(void)
{
	IFtpLib*	pFtp = NULL;
	HMODULE		hDLL = NULL;

#ifdef _WIN64
	#ifdef _DEBUG
		hDLL = LoadLibraryW(L"FTPLib_x64d.dll");
	#else
		hDLL = LoadLibraryW(L"FTPLib_x64.dll");
	#endif
#else
	#ifdef _DEBUG
		hDLL = LoadLibraryW(L"FTPLib_x86d.dll");
	#else
		hDLL = LoadLibraryW(L"FTPLib_x86.dll");
	#endif
#endif

	if( hDLL)
	{
		CREATE_FTP_INSTANCE_FUNC pFunc = (CREATE_FTP_INSTANCE_FUNC)GetProcAddress( hDLL, "CreateFTPInterface");
		if (pFunc)
		{
			HRESULT hr = pFunc((IFtpLib**)&pFtp);
			if (hr == S_OK)
			{
				pFtp->Initialize();
			}
		}	
	}

	return pFtp;
}


// 라이브러리로 슬 경우.
// 아래 코드를 긁어가서 쓴다.
//#include "../include/IErrorReportLib.h"
//#ifdef _DEBUG
//#ifdef _WIN64
//#pragma comment( lib, "../lib/ErrorReportLib_x64d.lib")
//#else
//#pragma comment( lib, "../lib/ErrorReportLib_x86d.lib")
//#endif
//#else
//#ifdef _WIN64
//#pragma comment( lib, "../lib/ErrorReportLib_x64.lib")
//#else
//#pragma comment( lib, "../lib/ErrorReportLib_x86.lib")
//#endif
//#endif
