// DirectoryName.h: interface for the CDirectoryName class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTORYNAME_H__D3740CAD_3F86_4191_B878_7DB72CC63ABE__INCLUDED_)
#define AFX_DIRECTORYNAME_H__D3740CAD_3F86_4191_B878_7DB72CC63ABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileName.h"
#include <list>

using namespace std;

#define GETNEXTFILENAME_END			0
#define GETNEXTFILENAME_FILE		1
#define GETNEXTFILENAME_DIRECTORY	2

#define LCO_FILE		1
#define LCO_DIRECTORY	2
#define LCO_ALL			LCO_FILE|LCO_DIRECTORY

class CDirectoryName : public CFileName
{
	list<CFileName*> m_ChildList;

	// Position °ü·Ã
	BOOL m_bReturnSelf;
	list<CFileName*>::iterator m_Iter;

public:
	CDirectoryName(CFileName* pParent,CYHFileFind* pFileFind);
	virtual ~CDirectoryName();

	void LoadChild( DWORD flag = LCO_ALL );

	void SetPositionHead();
	DWORD GetNextFileName(CStrClass* pFileNameOut);

	void Release();
};

#endif // !defined(AFX_DIRECTORYNAME_H__D3740CAD_3F86_4191_B878_7DB72CC63ABE__INCLUDED_)
