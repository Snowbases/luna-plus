#include "stdafx.h"
#include "SimpleLua.h"

#pragma comment(lib, "./Lua/LuaLibrary/LuaLib.lib")

extern "C" int LUA_GLUE_FUNCTION( lua_State* pLuaState )
{
	// 기본 스택의 개수를 구한다.
	const int nTotalStackNum = lua_gettop( pLuaState );
	const int nBaseStackNum = (int)lua_tonumber( pLuaState,  -nTotalStackNum );

	// 인자 개수를 구한다.
	const int nParameterCnt = nTotalStackNum - nBaseStackNum;

	return 0;
}

CSimpleLua::CSimpleLua(void) : m_LuaState( NULL ),
	m_LuaErrorFuncName( _T("ERROR_FUNC") )
{
}

CSimpleLua::CSimpleLua(LPCTSTR filename) : m_LuaState( NULL ),
	m_LuaErrorFuncName( _T("ERROR_FUNC") )
{
	Load( filename );
}

CSimpleLua::~CSimpleLua(void)
{
	Close();
}

BOOL CSimpleLua::InitLua()
{
	Close();

	try{
		m_LuaState = luaL_newstate();
		luaL_openlibs( m_LuaState );

		//m_LuaState = lua_open();
		//luaopen_base( m_LuaState );             /* opens the basic library */
		//luaopen_io( m_LuaState );               /* opens the I/O library */
		//luaopen_table( m_LuaState );            /* opens the table library */
		//luaopen_string( m_LuaState );           /* opens the string library. */
		//luaopen_math( m_LuaState );             /* opens the math library. */

	}
	catch(...){
		return FALSE;
	}

	return TRUE;
}

void CSimpleLua::Close()
{
	if( m_LuaState )
	{
		lua_close( m_LuaState );
		m_LuaState = NULL;
	}

	m_bInit = FALSE;
	m_bHasLuaErrorFunc = FALSE;

	ZeroMemory( m_FileName, MAX_PATH );

	m_nBaseStackNum = 0;
	m_nResultStackNum = 0;
	m_nParameterCnt = 0;

	m_mapLuaFuncList.clear();
}

BOOL CSimpleLua::Load( LPCTSTR filename )
{
	m_bInit = InitLua();

	if( !m_bInit )		return FALSE;

	if( luaL_dofile( m_LuaState, filename ) != 0 )
	{
		Close();
		return FALSE;
	}

	/*if( ExtractLuaFunc( filename ) == FALSE )
	{
		Close();
		return FALSE;
	}*/

	_tcsncpy( m_FileName, filename, MAX_PATH );
	m_nBaseStackNum = lua_gettop( m_LuaState );

	m_bHasLuaErrorFunc = IsLuaFuuc( m_LuaErrorFuncName );

	return TRUE;
}

//BOOL CSimpleLua::ExtractLuaFunc( LPCTSTR filename )
//{
//	CShinFileManager file;
//	if( ! file.Init( filename, "r" ) )
//		return FALSE;
//	
//	while( ! file.IsEOF() )
//	{
//		char txt[MAX_PATH]={0,};
//		_tcsncpy( txt, file.GetLine(), MAX_PATH );
//
//		const char* delimit = "\n\t ()";
//		char* token = _tcstok( txt, delimit );
//		if( ! token )
//		{
//			continue;
//		}
//		else if( ! _tcsicmp( token, "function" ) )
//		{
//			token = _tcstok( 0, delimit );
//			if( !token )	continue;
//			DWORD dwHashCode = GetHashCodeFromTxt( token );
//			std::string funcname = token;
//
//			m_mapLuaFuncList.insert( std::make_pair( dwHashCode, funcname ) );
//		}
//	}
//
//	return TRUE;
//}

BOOL CSimpleLua::GetGlobalBoolean( LPCTSTR val, BOOL& boolean )
{
	if( !m_bInit )		return FALSE;

	lua_getglobal( m_LuaState, val );
	if( lua_type( m_LuaState, -1 ) == LUA_TNIL )
	{
		ClearStack();
		return FALSE;
	}

	const BOOL bIsValidType = ( lua_type( m_LuaState, -1 ) == LUA_TBOOLEAN );
    boolean = (BOOL)lua_toboolean( m_LuaState, -1 );
	lua_pop( m_LuaState, 1 );

	return bIsValidType;
}

BOOL CSimpleLua::GetGlobalBoolean( LPCTSTR val )
{
	if( !m_bInit )		return 0;

	lua_getglobal( m_LuaState, val );
	if( lua_type( m_LuaState, -1 ) == LUA_TNIL )
	{
		lua_pop( m_LuaState, 1 );
		return 0;
	}

    BOOL boolean = (BOOL)lua_toboolean( m_LuaState, -1 );
	lua_pop( m_LuaState, 1 );

	return boolean;
}

BOOL CSimpleLua::GetGlobalNumber( LPCTSTR val, double& num )
{
	if( !m_bInit )		return FALSE;

	lua_getglobal( m_LuaState, val );
	if( lua_type( m_LuaState, -1 ) == LUA_TNIL )
	{
		ClearStack();
		return FALSE;
	}

	const BOOL bIsValidType = ( lua_type( m_LuaState, -1 ) == LUA_TNUMBER );
    num = lua_tonumber( m_LuaState, -1 );
	lua_pop( m_LuaState, 1 );

	return bIsValidType;
}

double CSimpleLua::GetGlobalNumber( LPCTSTR val )
{
	if( !m_bInit )		return 0;

	lua_getglobal( m_LuaState, val );
	if( lua_type( m_LuaState, -1 ) == LUA_TNIL )
	{
		lua_pop( m_LuaState, 1 );
		return 0;
	}

    double num = lua_tonumber( m_LuaState, -1 );
	lua_pop( m_LuaState, 1 );

	return num;
}

LPCTSTR CSimpleLua::GetGlobalString( LPCTSTR val )
{
	static TCHAR tmpString[MAX_PATH];
	ZeroMemory( tmpString, sizeof(tmpString) );
	return GetGlobalString( val, tmpString, sizeof(tmpString)/sizeof(*tmpString) );
}

std::string CSimpleLua::GetGlobalStringWithSTLString( LPCTSTR val )
{
	return GetGlobalString( val );
}

LPCTSTR CSimpleLua::GetGlobalString( LPCTSTR val, LPTSTR str, size_t strLen )
{
	if( !m_bInit )		return "";

	lua_getglobal( m_LuaState, val );
	if( lua_type( m_LuaState, -1 ) == LUA_TNIL )
	{
		lua_pop( m_LuaState, 1 );
		return "";
	}

	const BOOL bIsValidType = ( lua_type( m_LuaState, -1 ) == LUA_TSTRING );
    const char* strVal = lua_tostring( m_LuaState, -1 );
	if( bIsValidType )
		_tcsncpy( str, strVal, strLen );

	lua_pop( m_LuaState, 1 );

	return (bIsValidType == TRUE ? str : "");
}

BOOL CSimpleLua::GetGlobalTableFieldNumber( LPCTSTR table, LPCTSTR field, double& num )
{
	if( !m_bInit )		return FALSE;

	lua_getglobal( m_LuaState, table );
	if( lua_type( m_LuaState, -1 ) == LUA_TNIL )
	{
		lua_pop( m_LuaState, 1 );
		return 0;
	}

	lua_getfield( m_LuaState, -1, field );

	const BOOL bIsValidType = ( lua_type( m_LuaState, -1 ) == LUA_TNUMBER );
    num = lua_tonumber( m_LuaState, -1 );
	lua_pop( m_LuaState, 2 );

	return bIsValidType;
}

double CSimpleLua::GetGlobalTableFieldNumber( LPCTSTR table, LPCTSTR field )
{
	if( !m_bInit )		return 0;

	lua_getglobal( m_LuaState, table );
	if( lua_type( m_LuaState, -1 ) == LUA_TNIL )
	{
		lua_pop( m_LuaState, 1 );
		return 0;
	}

	if( !lua_istable( m_LuaState, -1 ) )
	{
		lua_pop( m_LuaState, 1 );
		return 0;
	}

	lua_getfield( m_LuaState, -1, field );

    double num = lua_tonumber( m_LuaState, -1 );
	lua_pop( m_LuaState, 2 );

	return num;
}

LPCTSTR CSimpleLua::GetGlobalTableFieldString( LPCTSTR table, LPCTSTR field, LPTSTR str, size_t strLen )
{
	if( !m_bInit )		return "";

	lua_getglobal( m_LuaState, table );
	if( lua_type( m_LuaState, -1 ) == LUA_TNIL )
	{
		lua_pop( m_LuaState, 1 );
		return 0;
	}

	lua_getfield( m_LuaState, -1, field );

	const BOOL bIsValidType = ( lua_type( m_LuaState, -1 ) == LUA_TSTRING );
    const char* strVal = lua_tostring( m_LuaState, -1 );
	if( bIsValidType )
		_tcsncpy( str, strVal, strLen );

	lua_pop( m_LuaState, 2 );

	return (bIsValidType == TRUE ? str : "");
}

BOOL CSimpleLua::CheckStackToAdd( int nAddCnt ) const
{
	if( !m_bInit )		return FALSE;

	return lua_checkstack( m_LuaState, nAddCnt );
}

void CSimpleLua::ClearStack()
{
	if( !m_bInit )		return;
	
	int nPopCnt = lua_gettop( m_LuaState ) - m_nBaseStackNum;
	if( nPopCnt > 0 )
		lua_pop( m_LuaState, nPopCnt );

	m_nResultStackNum = 0;
	m_nParameterCnt = 0;
}

BOOL CSimpleLua::AddNumberInStack( double num )
{
	if( !m_bInit )		return FALSE;

	if( CheckStackToAdd() == FALSE )
	{
		OutputDebug( "Error %s - lua_checkstack() error\n", __FUNCTION__ );
		return FALSE;
	}

	lua_pushnumber( m_LuaState, num );

	return TRUE;
}

BOOL CSimpleLua::AddNumberInStack( int num )
{
	if( !m_bInit )		return FALSE;

	if( CheckStackToAdd() == FALSE )
	{
		OutputDebug( "Error %s - lua_checkstack() error\n", __FUNCTION__ );
		return FALSE;
	}

	lua_pushinteger( m_LuaState, num );

	return TRUE;
}

BOOL CSimpleLua::AddStringInStack( LPCTSTR str )
{
	if( !m_bInit )		return FALSE;

	if( CheckStackToAdd() == FALSE )
	{
		OutputDebug( "Error %s - lua_checkstack() error\n", __FUNCTION__ );
		return FALSE;
	}

	size_t strLen = _tcslen( str );

	lua_pushlstring( m_LuaState, str, strLen );

	return TRUE;
}

double CSimpleLua::GetNumberInStack( int nPos )
{
	if( ! IsNumberInStack( nPos ) )
		return -1;

	const int nStackCnt = lua_gettop( m_LuaState ) - m_nBaseStackNum;
	nPos = -(nStackCnt - nPos + 1);

	return lua_tonumber( m_LuaState, nPos );
}

LPCTSTR CSimpleLua::GetStringInStack( int nPos )
{
	if( ! IsStringInStack( nPos ) )
		return _T("");

	const int nStackCnt = lua_gettop( m_LuaState ) - m_nBaseStackNum;
	nPos = -(nStackCnt - nPos + 1);

	return lua_tostring( m_LuaState, nPos );
}

BOOL CSimpleLua::Execute( int nReturnCnt )
{
	if( !m_bInit )		return FALSE;

	// Error시 Lua 실행 함수
	if( m_bHasLuaErrorFunc )
	{
		lua_getglobal( m_LuaState, m_LuaErrorFuncName );
		lua_insert( m_LuaState, (m_nBaseStackNum > 0 ? m_nBaseStackNum : 1) );
	}

	if( lua_pcall( m_LuaState, m_nParameterCnt, nReturnCnt, m_nBaseStackNum ) != 0 )
	{
		ClearStack();
		return FALSE;
	}

	m_nResultStackNum = lua_gettop( m_LuaState ) - m_nBaseStackNum;

	if( m_bHasLuaErrorFunc )
		m_nResultStackNum -= 1;

	m_nParameterCnt = 0;

	if( m_nResultStackNum != nReturnCnt )
	{
		OutputDebug( "Error %s - 결과값의 개수가 지정개수와 다릅니다.\n", __FUNCTION__ );
	}

	return TRUE;
}

BOOL CSimpleLua::ExecuteFunc( LPCTSTR func, LPCTSTR parameter, int nReturnCnt, LPCTSTR returnParameter, ... )
{
	ClearStack();

	try{
		// 함수 등록
		lua_getglobal( m_LuaState, func );
		if( lua_type( m_LuaState, -1 ) == LUA_TNIL )
		{
			OutputDebug( "Error %s\n", __FUNCTION__ );
			ClearStack();
			return FALSE;
		}

		va_list vl;
		va_start( vl, returnParameter );
		for( ; *parameter ; ++parameter )
		{
			switch( *parameter )
			{
			case 'd':
				{
					if( AddNumberInStack( va_arg( vl, int ) ) == FALSE )
					{
						OutputDebug( "Error %s\n", __FUNCTION__ );
						ClearStack();
					}

					++m_nParameterCnt;
				}
				break;
			case 'f':
				{
					if( AddNumberInStack( va_arg( vl, double ) ) == FALSE )
					{
						OutputDebug( "Error %s\n", __FUNCTION__ );
						ClearStack();
					}

					++m_nParameterCnt;
				}
				break;
			case 's':
				{
					if( AddStringInStack( va_arg( vl, char* ) ) == FALSE )
					{
						OutputDebug( "Error %s\n", __FUNCTION__ );
						ClearStack();
					}

					++m_nParameterCnt;
				}
				break;
			case '%':
			case ' ':
				break;
			default:
				{
					OutputDebug( "Error %s - 잘못된 Parameter Argment '%s' \n", __FUNCTION__, *parameter );
				}
				break;
			}
		}

		if( Execute( nReturnCnt ) == FALSE )
		{
			OutputDebug( "Error %s\n", __FUNCTION__ );
			ClearStack();
			return FALSE;
		}

		int nCnt = 0;
		for( int nPos = 0 ; *returnParameter && nPos < m_nResultStackNum ; ++returnParameter )
		{
			switch( *returnParameter )
			{
			case 'd':
				{
					*va_arg( vl, int* ) = (int)GetNumberInStack( ++nPos );
				}
				break;
			case 'f':
				{
					*va_arg( vl, double* ) = GetNumberInStack( ++nPos );
				}
				break;
			case 's':
				{
					char* pDest = *va_arg( vl, char** );
					const int nLen = va_arg( vl, int );
					_tcsncpy( pDest, GetStringInStack( ++nPos ), nLen );
				}
				break;
			case '%':
			case ' ':
				break;
			default:
				{
					OutputDebug( "Error %s - 잘못된 Return Parameter Argment '%s' \n", __FUNCTION__, *returnParameter );
				}
				break;
			}
		}

		va_end( vl );
	}
	catch(...)
	{
		OutputDebug( "Error %s\n", __FUNCTION__ );
		ClearStack();
		return FALSE;
	}

	return TRUE;
}

BOOL CSimpleLua::IsLuaFuuc( LPCTSTR funcname ) const
{
	const DWORD dwHashCode = GetHashCodeFromTxt( funcname );
	return m_mapLuaFuncList.find( dwHashCode ) != m_mapLuaFuncList.end();
}

BOOL CSimpleLua::IsNumberInStack( int nPos ) const
{
	const int nStackCnt = lua_gettop( m_LuaState ) - m_nBaseStackNum;
	if( nStackCnt < nPos )
		return FALSE;

	nPos = -(nStackCnt - nPos + 1);

	return ( lua_type( m_LuaState, nPos ) == LUA_TNUMBER );
}

BOOL CSimpleLua::IsStringInStack( int nPos ) const
{
	const int nStackCnt = lua_gettop( m_LuaState ) - m_nBaseStackNum;
	if( nStackCnt < nPos )
		return FALSE;

	nPos = -(nStackCnt - nPos + 1);

	return ( lua_type( m_LuaState, nPos ) == LUA_TSTRING );
}

BOOL CSimpleLua::AddLuaGlueFunc( LuaGlueRegistType glueType )
{
	for( LuaGlueRegistType::const_iterator iter = glueType.begin() ; iter != glueType.end() ; ++iter )
	{
		if( AddLuaGlueFunc( iter->first.c_str(), iter->second ) == FALSE )
			return FALSE;
	}

	return TRUE;
}

BOOL CSimpleLua::AddLuaGlueFunc( LPCTSTR glueFuncname, LuaGlueFuncType glueFunc )
{
	if( !m_bInit )		return FALSE;

	lua_register( m_LuaState, glueFuncname, glueFunc );

	return TRUE;
}

void CSimpleLua::OutputStackInfo()
{
	if( !m_bInit )		return;

	OutputDebug( "\n----------------------------------------\n" );
	OutputDebug( "Lua Stack\n" );

    int top = lua_gettop( m_LuaState );
    for ( int i=top ; i>=1 ; --i )
    {
        OutputDebug( "%02d | ", i );
        switch ( lua_type( m_LuaState, i ) )
        {
        case LUA_TNIL:
            OutputDebug( "nil      | \n" );
            break;
        case LUA_TBOOLEAN:
            OutputDebug( "boolean  | %d\n", (int)lua_toboolean( m_LuaState, i ) );
            break;
		case LUA_TNUMBER:
            OutputDebug( "number   | %f\n", lua_tonumber( m_LuaState, i ) );
            break;
        case LUA_TSTRING:
            OutputDebug( "string   | %s\n", lua_tostring( m_LuaState, i ) );
            break;
        case LUA_TTABLE:
            OutputDebug( "table    | 0x%08x\n", lua_topointer( m_LuaState, i ) );
            break;
        case LUA_TFUNCTION:
            OutputDebug( "function | 0x%08x\n", lua_topointer( m_LuaState, i ) );
            break;
        case LUA_TUSERDATA:
            OutputDebug( "userdata | 0x%08x\n", lua_topointer( m_LuaState, i ) );
            break;
        case LUA_TTHREAD:
            OutputDebug( "thread   | 0x%08x\n", lua_topointer( m_LuaState, i ) );
            break;
        default:
            OutputDebug( "unknown  | \n" );
            break;
        }
    }

	OutputDebug("----------------------------------------\n");
}



// class SimpleLuaManager
CSimpleLuaManager::CSimpleLuaManager()
{
}

CSimpleLuaManager::~CSimpleLuaManager()
{
}

BOOL CSimpleLuaManager::CreateSimpleLua( LPCTSTR scriptname )
{
	DWORD dwHashCode = GetHashCodeFromTxt( scriptname );
	if( IsAdded( dwHashCode ) )
	{
		OutputDebug( "Error %s - 이미 생성된 있는 Script 입니다.", __FUNCTION__ );
		return FALSE;
	}

	CSimpleLua& simpleLua = m_mapSimpleLua[ dwHashCode ];
	if( simpleLua.Load( scriptname ) == FALSE )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CSimpleLuaManager::RemoveSimpleLua( LPCTSTR scriptname )
{
	DWORD dwHashCode = GetHashCodeFromTxt( scriptname );
	
	return RemoveSimpleLua( dwHashCode );
}

BOOL CSimpleLuaManager::RemoveSimpleLua( DWORD dwHashCode )
{
	std::map< DWORD, CSimpleLua >::iterator iterSimpleLua =  m_mapSimpleLua.find( dwHashCode );
	if( iterSimpleLua == m_mapSimpleLua.end() )
	{
		return FALSE;
	}

	m_mapSimpleLua.erase( iterSimpleLua );
	return TRUE;
}

BOOL CSimpleLuaManager::IsAdded( DWORD dwHashCode ) const 
{
	std::map< DWORD, CSimpleLua >::const_iterator iterSimpleLua =  m_mapSimpleLua.find( dwHashCode );
	return ( iterSimpleLua != m_mapSimpleLua.end() );
}

BOOL CSimpleLuaManager::IsAdded( LPCTSTR scriptname ) const 
{
	DWORD dwHashCode = GetHashCodeFromTxt( scriptname );
	return IsAdded( dwHashCode );
}

CSimpleLua& CSimpleLuaManager::GetSimpleLua( LPCTSTR name )
{
	DWORD dwHashCode = GetHashCodeFromTxt( name );

	return GetSimpleLua( dwHashCode );
}

CSimpleLua& CSimpleLuaManager::GetSimpleLua( DWORD dwHashCode )
{
	static CSimpleLua emptySimpleLua;
	std::map< DWORD, CSimpleLua >::iterator iterSimpleLua =  m_mapSimpleLua.find( dwHashCode );
	if( iterSimpleLua == m_mapSimpleLua.end() )
	{
		return emptySimpleLua;
	}

	return iterSimpleLua->second;
}

LPCTSTR CSimpleLuaManager::GetSimpleLuaScriptname( DWORD dwHashCode )
{ 
	CSimpleLua& simpleLua = GetSimpleLua( dwHashCode );
	return simpleLua.GetLuaScriptname();
}