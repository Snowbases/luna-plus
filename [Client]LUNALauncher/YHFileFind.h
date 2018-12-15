#pragma once

class CYHFileFind  
{
	HANDLE			m_hFileFind;
	WIN32_FIND_DATA file;
public:
	CYHFileFind();
	virtual ~CYHFileFind();

	BOOL FindFile( const TCHAR* filter = 0 );

	inline BOOL FindNextFile()
	{
		return m_hFileFind ? ::FindNextFile(m_hFileFind,&file) : FALSE;
	}

	inline BOOL IsDirectory() const
	{
		return file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	}

	inline CString GetFileName() const
	{
		return m_hFileFind ? file.cFileName : _T( "" );
	}
};