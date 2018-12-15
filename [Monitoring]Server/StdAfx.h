// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


// Windows Header Files:
#include <windows.h>
#include <winsock2.h>
#include <ole2.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>

struct VECTOR3	{float x,y,z;};
extern DWORD gCurTime;

#define _CRYPTCHECK_

// Local Header Files
#include "../[Lib]YhLibrary/YHLibrary.h"
#include "Protocol.h"
#include "CommonDefine.h"
#include "MSDefine.h"
#include "CommonGameDefine.h"
#include "CommonStruct.h"
#include "ServerGameStruct.h"
#include "CommonStructMS.h"
#include "..\4DYUCHINET_COMMON\INetwork_GUID.h"

extern void LogConsole(char * buff,...);
extern void WriteAssertMsg(char* pStrFileName,int Line,char* pMsg);
extern void LOG( char* msg, ... );
inline void StringCopySafe( LPTSTR const lhs, LPCTSTR const rhs, size_t size )
{
	strncpy( lhs, rhs, size );
	lhs[ size - 1 ] = 0;
}

#ifndef SafeStrCpy
#define SafeStrCpy(lhs, rhs, size) StringCopySafe(lhs, rhs, size)
#endif

#ifndef _countof
#define _countof(array) (sizeof(array) / sizeof(*array))
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
