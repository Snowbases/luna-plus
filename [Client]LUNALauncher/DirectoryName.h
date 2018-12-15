#pragma once

#include "FileName.h"

#define GETNEXTFILENAME_END			0
#define GETNEXTFILENAME_FILE		1
#define GETNEXTFILENAME_DIRECTORY	2

#define LCO_FILE		1
#define LCO_DIRECTORY	2
#define LCO_ALL			LCO_FILE|LCO_DIRECTORY

class CDirectoryName : public CFileName
{
	CList< CFileName*, CFileName*& > mChildList;

	BOOL		mResult;
	POSITION	mPosition;

public:
	CDirectoryName( CFileName* = 0,CYHFileFind* = 0 );
	virtual ~CDirectoryName();

	void LoadChild( DWORD flag = LCO_ALL );

	void SetPositionHead();
	DWORD GetNextFileName( CString& );

	void Release();
	inline size_t GetSize() const
	{
		return mChildList.GetSize();
	}
};