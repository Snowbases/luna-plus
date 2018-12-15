// FtpFile.cpp: implementation of the CFtpFile class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FtpFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFtpFile::CFtpFile(char* pszFileName/*=NULL*/)
{
	memset(m_szFileFullName, 0x00, sizeof(m_szFileFullName));
	memset(m_szFilePath, 0x00, sizeof(m_szFilePath));
	memset(m_szFileName, 0x00, sizeof(m_szFileName));
}

CFtpFile::~CFtpFile()
{

}

char* CFtpFile::GetPureFileName(char *pszFileName)
{
	char szDirSeptor[2];
	memset(szDirSeptor, 0x00, sizeof(szDirSeptor));
#ifdef __UNIX__
	sprintf(szDirSeptor, "/");
#else
	sprintf(szDirSeptor, "\\");
#endif

	char* pResult = NULL;
	char* pReturnPoint = pszFileName;

	pResult = strpbrk(pszFileName, szDirSeptor);

	while(pResult!=NULL)
	{
		pReturnPoint = ++pResult;
		pResult = strpbrk(pResult, szDirSeptor);
	}
	
	return pReturnPoint; 
}

//모르겠다 너무 어렵다.. 아무튼 디렉토리를 깨끗이 가져온다.
BOOL CFtpFile::GetRemoteDirectoryString(char *pszSource, char *pszResult)
{
	int iLoop = strlen(pszSource);
	if(iLoop<=0)
		return FALSE;

	int iFirstSep = -1;
	int iLastSep = -1;
	int iResultIndex = 0;
	
	for(int i=0; i<iLoop; i++)
	{
		if((pszSource[i]=='\"')&&(iFirstSep==-1))
		{
			iFirstSep = i;
			continue;
		}

		if(iFirstSep<0)
			continue;

		if((pszSource[i]=='\"')&&(iLastSep==-1)&&(iFirstSep>=0))
			iLastSep = i;

		if((iFirstSep>0)&&(iLastSep>0))
			break;

		if((iFirstSep>=0)&&(pszSource[i]!='\"'))
		{
			pszResult[iResultIndex] = pszSource[i];
			iResultIndex++;
		}
	}

	if((iFirstSep<0)||(iLastSep<0))
		return FALSE;

	//마지막에 디렉토리 구분자가 없다면, 넣어준다.
	if(pszResult[strlen(pszResult)-1]!='/')
		strcat(pszResult, "/");

	return TRUE;
}

BOOL CFtpFile::CheckDownloadPath(char *pszDownloadPath)
{
#ifdef __UNIX__
     //mkdir(pszDownloadPath, S_IRWXU|S_IRWXG|S_IRWXO);
	 if(pszDownloadPath[strlen(pszDownloadPath)]=='/')
		 return TRUE;
#else
	//::CreateDirectory( pszDownloadPath );
	 if(pszDownloadPath[strlen(pszDownloadPath)]=='\\')
		 return TRUE;
#endif

	 return FALSE;
}
