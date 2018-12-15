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
#define _WIN32_WINNT	0x0500 
#define _CRYPTCHECK_

#pragma warning(disable:4512)
#pragma warning(disable:4100) // 참조하지 않는 매개변수
#pragma warning(disable:4127) // 조건식이 상수인 경우
#pragma warning(disable:4201) // 비표준 확장을 사용한경우 내부에 Struct 만들때 이름을 적지 않았음.

#include <windows.h>
#include <winsock2.h>
#include <ole2.h>
#include <initguid.h>
#include <stdio.h>
#include <assert.h>
// TODO: reference additional headers your program requires here

#include "DataBase.h"
#include "Console.h"
 
#include <yhlibrary.h>
//#include "CommonHeader.h"
#include "..\[CC]Header\vector.h"
#include "..\[CC]Header\protocol.h"
#include "..\[CC]Header\CommonDefine.h"
#include "..\[CC]Header\CommonGameDefine.h"
#include "..\[CC]Header\ServerGameDefine.h"
#include "..\[CC]Header\CommonStruct.h"
#include "..\[CC]Header\ServerGameStruct.h"
#include "..\[CC]Header\CommonGameFunc.h"


#include ".\ServerSystem.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
