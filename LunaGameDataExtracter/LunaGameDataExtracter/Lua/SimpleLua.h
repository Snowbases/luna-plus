#pragma once

#include <map>
#include <string>

extern "C"{
#include "./Lua/LuaLibrary/lua.h"
#include "./Lua/LuaLibrary/lualib.h"
#include "./Lua/LuaLibrary/lauxlib.h"
}

typedef int (*LuaGlueFuncType)(lua_State* pLuaState);

#define LuaGlue					extern "C" int
#define LuaGlueRegistType		std::map< std::string, LuaGlueFuncType >

class CSimpleLua
{
	const TCHAR* const m_LuaErrorFuncName;
	TCHAR m_FileName[MAX_PATH];
	BOOL m_bInit;
	BOOL m_bHasLuaErrorFunc;

	lua_State* m_LuaState;
    int m_nBaseStackNum;
	int m_nResultStackNum;
	int m_nParameterCnt;

	std::map< DWORD, std::string > m_mapLuaFuncList;
public:
	CSimpleLua(void);
	CSimpleLua(LPCTSTR filename);
	virtual ~CSimpleLua(void);

	BOOL InitLua();
	void Close();

	const lua_State* const GetLuaState() const { return m_LuaState; }

	// Lua Script Load
	BOOL Load( LPCTSTR filename );
	// Lua 함수들 등록
	BOOL ExtractLuaFunc( LPCTSTR filename );

	LPCTSTR GetLuaScriptname() const { return m_FileName; }

    // 전역변수 얻어오기
	BOOL GetGlobalBoolean( LPCTSTR val, BOOL& boolean );
	BOOL GetGlobalBoolean( LPCTSTR val );
	BOOL GetGlobalNumber( LPCTSTR val, double& num );
	double GetGlobalNumber( LPCTSTR val );
	LPCTSTR GetGlobalString( LPCTSTR val );
	std::string GetGlobalStringWithSTLString( LPCTSTR val );
	LPCTSTR GetGlobalString( LPCTSTR val, LPTSTR str, size_t strLen );
	BOOL GetGlobalTableFieldNumber( LPCTSTR table, LPCTSTR field, double& num );
	double GetGlobalTableFieldNumber( LPCTSTR table, LPCTSTR field );
	LPCTSTR GetGlobalTableFieldString( LPCTSTR table, LPCTSTR field, LPTSTR str, size_t strLen );

    // Stack 관련 함수
	inline BOOL CheckStackToAdd( int nAddCnt=1 ) const;
	void ClearStack();
	BOOL AddNumberInStack( double num );
	BOOL AddNumberInStack( int num );
	BOOL AddStringInStack( LPCTSTR str );
	double GetNumberInStack( int nPos );
	LPCTSTR GetStringInStack( int nPos );
	BOOL IsNumberInStack( int nResultPos ) const;
	BOOL IsStringInStack( int nResultPos ) const;
	
	// lua_pcall 실행(리턴개수 전달)
	BOOL Execute( int nReturnCnt );

	// 함수이름, 인자유형"%d%f%s", 결과값 개수, 결과값 유형"%d%f%s", 인자/결과....
	// 결과값 %s인 경우 결과값 저장소외 저장소의크기를 주어야함!
	BOOL ExecuteFunc( LPCTSTR func, LPCTSTR parameter, int nReturnCnt, LPCTSTR returnParameter, ... );

	// 루아 스크립트 내에 존재하는 함수인지 판단
	BOOL IsLuaFuuc( LPCTSTR funcname ) const;
	BOOL HasResult() const { return m_nResultStackNum > 0; }

	// 루아글루함수 등록
	BOOL AddLuaGlueFunc( LuaGlueRegistType glueType );
	BOOL AddLuaGlueFunc( LPCTSTR glueFuncname, LuaGlueFuncType glueFunc );

	// LuaStack 정보를 디버그창에 출력
	void OutputStackInfo();
};



#define SIMPLELUAMGR	CSimpleLuaManager::GetInstance()
class CSimpleLuaManager
{
	std::map< DWORD, CSimpleLua >	m_mapSimpleLua;
public:
	CSimpleLuaManager();
	~CSimpleLuaManager();
	
	static CSimpleLuaManager* GetInstance() { static CSimpleLuaManager mgr;		return &mgr; }

	BOOL CreateSimpleLua( LPCTSTR scriptname );
	BOOL RemoveSimpleLua( LPCTSTR scriptname );
	BOOL RemoveSimpleLua( DWORD dwHashCode );

	const size_t GetSimpleLuaCount() const { return m_mapSimpleLua.size(); }
	BOOL IsAdded( DWORD dwHashCode ) const;
	BOOL IsAdded( LPCTSTR scriptname ) const;
	CSimpleLua& GetSimpleLua( LPCTSTR name );
	CSimpleLua& GetSimpleLua( DWORD dwHashCode );

	DWORD GetSimpleLuaIndex( LPCTSTR scriptname ) const { return GetHashCodeFromTxt( scriptname ); }
	LPCTSTR GetSimpleLuaScriptname( DWORD dwHashCode );
};