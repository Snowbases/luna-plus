// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__74ACF483_F8C6_4E6F_A7D9_A4335A96C6B9__INCLUDED_)
#define AFX_STDAFX_H__74ACF483_F8C6_4E6F_A7D9_A4335A96C6B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning (disable:4786)

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// 080820 LUJ, 유니코드 적용위해 작업
#include <mmsystem.h>
#include <winsock2.h>
#include <hash_map>
#include <list>

#define _CRYPTCHECK_

#define SAFE_DELETE(a)			if((a))	{ delete (a); (a)=NULL; }
#define SAFE_DELETE_ARRAY(a)	if((a))	{ delete [] (a); (a)=NULL; }
#define SAFE_RELEASE(a)			if((a))	{ (a)->Release(); (a)=NULL; }

struct StaticString
{
	StaticString()	{	Str = NULL;	}
	~StaticString()	{	SAFE_DELETE_ARRAY( Str ); }

	TCHAR* Str;

	inline void operator = ( const TCHAR* pchar )
	{
		Str = new TCHAR[ _tcslen(pchar)+1 ];
		_tcscpy( Str, pchar );
	}

	inline operator const TCHAR*() const
	{
		return Str;
	}

	inline operator TCHAR*() const
	{
		return Str;
	}
};

// 090608 LUJ, CommonStruct.h에 사용된 HashTable 클래스를 컴파일하기 위해 빈 템플릿 클래스를 선언했다.
#define HASHTABLE_H
template< typename T >
class CYHHashTable
{
public:
	inline void Initialize( DWORD ) {}
	inline void RemoveAll() {}
	inline T* GetData() const {}
	inline void SetPositionHead() {}
};

extern DWORD gCurTime;

#include "..\[CC]Header\vector.h"
#include "..\[CC]Header\protocol.h"
#include "..\[lib]yhlibrary\HSEL.h"
#include "..\[CC]Header\CommonDefine.h"
#include "..\[CC]Header\CommonGameDefine.h"
#include "..\[CC]Header\CommonStruct.h"
#include "..\[CC]Header\ServerGameDefine.h"
#include "..\[CC]Header\ServerGameStruct.h"
#include "..\[CC]Header\CommonStructMS.h"
#include "..\[CC]Header\CommonGameFunc.h"
#include "MHFile.h"
#include "ServerToolCommon.h"
#include "Resource.h"

CString GetDataDirectory();
CString GetUserCountDirectory();

void LOG( LPCTSTR, ... );

#endif // !defined(AFX_STDAFX_H__74ACF483_F8C6_4E6F_A7D9_A4335A96C6B9__INCLUDED_)
