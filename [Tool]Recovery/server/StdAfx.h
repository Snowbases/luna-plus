#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


// Windows Header Files:
#include <windows.h>
#include <winsock2.h>
#include <ole2.h>
#include <assert.h>
// 081021 LUJ, 엑셀 자동화를 위해 참조
#include <ComDef.h>
// C RunTime Header Files
#include <cstdlib>
#include <cstdio>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include <hash_map>
#include <hash_set>
#include <iostream>
#include <conio.h>
#include <fstream>
#include <map>
#include <set>

struct VECTOR3	{float x,y,z;};
extern DWORD gCurTime;
struct USERINFO;

#define _CRYPTCHECK_
// 090608 LUJ, yhlibrary에 있는 StaticString이 컴파일이 안되므로 위에 선언된 것으로 대체하기 위해 선언한다.
#define _STATICSTRING_H_
typedef std::string StaticString;
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

#include "..\..\[lib]yhlibrary\HSEL.h"
#include "..\..\[CC]Header\Protocol.h"
#include "..\..\[CC]Header\CommonDefine.h"
#include "DataBase.h"
#include "..\..\[CC]Header\CommonGameDefine.h"
#include "..\..\[CC]Header\CommonStruct.h"
#include "..\..\[CC]Header\ServerGameStruct.h"
#include "..\..\[cc]header\GameResourceStruct.h"
#include "..\common\common.h"
#include "..\..\[CC]ServerModule\MHFile.h"

extern void WriteAssertMsg(TCHAR* pStrFileName,int Line, TCHAR* pMsg);

void PutLog( const TCHAR*, ... );