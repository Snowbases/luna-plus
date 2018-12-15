#pragma once

#include "YHFileFind.h"

class CFileName  
{
protected:
	CFileName*	m_pParent;
	CString		m_FileName;
	BOOL		m_bIsDirectory;
	
public:
	CFileName( CFileName*, CYHFileFind* );
	virtual ~CFileName();

	inline BOOL IsDirectory() const	{ return m_bIsDirectory; }

	inline CString GetFileName()	const
	{
		return m_FileName;
	}

	CString GetFullPathName() const;
};