// FtpFile.h: interface for the CFtpFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTPFILE_H__0F2E7AFE_AD76_4DAA_A75D_1DE6267ACF31__INCLUDED_)
#define AFX_FTPFILE_H__0F2E7AFE_AD76_4DAA_A75D_1DE6267ACF31__INCLUDED_

#include "StdAfx.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFtpFile  
{
public:
	BOOL CheckDownloadPath(char* pszDownloadPath);
	BOOL GetRemoteDirectoryString(char* pszSource, char* pszResult);
	char* GetPureFileName(char* pszFileName);
	CFtpFile(char* pszFileName=NULL);
	virtual ~CFtpFile();

private:
	char m_szFilePath[MAX_PATH];
	char m_szFileName[MAX_PATH];
	char m_szFileFullName[MAX_PATH];
};

#endif // !defined(AFX_FTPFILE_H__0F2E7AFE_AD76_4DAA_A75D_1DE6267ACF31__INCLUDED_)
