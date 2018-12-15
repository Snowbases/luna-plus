#include "StdAfx.h"
#include "curl\curl.h"
#include ".\network.h"
#include "../[Lib]ZipArchive/ZipArchive.h"
#include "Application.h"

CNetwork::CNetwork(void) :
mCallBackFunctionPursuit( 0 ),
mLibraryModule( 0 ),
mCurl( 0 )
{
	if( ! Initialize() )
	{
		ASSERT( 0 );
	}
}

CNetwork::~CNetwork(void)
{
	Release();
}

bool CNetwork::Initialize()
{
	Release();

	mLibraryModule = LoadLibrary( _T( "libcurl.dll" ) );

	if( ! mLibraryModule )
	{
		return false;
	}

	// 080625 LUJ, curl을 초기화하기 위해 curl_global_init() 호출
	{
		typedef void ( __cdecl *Function )( long );
		Function curl_global_init = ( Function )GetProcAddress( mLibraryModule, "curl_global_init" );

		if( ! curl_global_init )
		{
			return false;
		}

		curl_global_init( CURL_GLOBAL_WIN32 );
	}

	return true;
}

void CNetwork::Release()
{
	if( ! mLibraryModule )
	{
		return;
	}

	// 080625 LUJ, 함수를 런타임 중에 링킹하여 호출하도록 함
	{
		typedef void ( __cdecl *Function )();
		Function function = ( Function )GetProcAddress( mLibraryModule, "curl_global_cleanup" );

		if( ! function )
		{
			return;
		}

		function();
	}

	FreeLibrary( mLibraryModule );

	mLibraryModule	= 0;
	mCurl			= 0;
}

CNetwork& CNetwork::GetInstance()
{
	static CNetwork instance;

	return instance;
}

int CNetwork::WriteToMemory( char* data, size_t size, size_t nmemb, void* file )
{
	CZipMemFile* memoryFile = ( CZipMemFile* )file;

	if( ! CNetwork::GetInstance().mCurl ||
		! memoryFile )
	{
		return 0;
	}

	const UINT dataSize = UINT( size * nmemb );

	memoryFile->Write( data, dataSize );

	return dataSize;
}

int CNetwork::WriteToDisk( char* data, size_t size, size_t nmemb, void* file )
{
	CFile* diskFile = ( CFile* )file;

	if( ! CNetwork::GetInstance().mCurl ||
		! diskFile )
	{
		return 0;
	}

	const UINT dataSize = UINT( size * nmemb );

	diskFile->Write( data, dataSize );




	return dataSize;
}

int CNetwork::Pursuit( LPVOID /*p*/, double doubleTotal, double doubleNow, double /*unsignedLongTotal*/, double /*unsignedLongNow*/ )
{
	// 090819 ONS 다운로드 중인 파일의 전송속도와 남은시간을 구한다.
	double dCurrTime;
	double dDownSpeed;
	{
		CNetwork& 		instance 		= GetInstance();
		CURL*&			curl			= instance.mCurl;
		HMODULE			libraryModule	= instance.mLibraryModule;

		typedef CURLcode ( __cdecl *Function )(CURL*,CURLINFO , ...);
		Function function = ( Function )GetProcAddress( libraryModule, "curl_easy_getinfo" );
		if( ! function )
		{
			ASSERT( 0 );
			return 0;
		}
		function(curl,CURLINFO_TOTAL_TIME, &dCurrTime );
		function(curl,CURLINFO_SPEED_DOWNLOAD , &dDownSpeed );

	}

	double dRemainTime = (doubleTotal / dDownSpeed) - dCurrTime;

	PursuitFunction function = CNetwork::GetInstance().mCallBackFunctionPursuit;

	if( function )
	{
		( *function )( doubleNow / doubleTotal, dRemainTime, dDownSpeed );
	}

	return 0;
}

// 081201 LUJ, 다운로드 결과를 반환하도록 한다
// 081202 LUJ, 다운로드를 메모리에 받을 수 있도록 한다
BOOL CNetwork::Download( const TCHAR* path, const TCHAR* fileName, PursuitFunction callBackFunctionPursuit, bool isMemoryDownload )
{
	CNetwork& 		instance 		= GetInstance();
	CURL*&			curl			= instance.mCurl;
	HMODULE			libraryModule	= instance.mLibraryModule;
	CFile&			diskFile		= instance.GetDiskFile();
	CZipMemFile&	memoryFile		= instance.GetMemoryFile();
	memoryFile.SetLength( 0 );

	// 081202 LUJ, 플래그에 따라 다른 파일 형태를 포인터에 할당한다
	if( ! isMemoryDownload &&
		! diskFile.Open( fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ) )
	{
		ASSERT( 0 );
		return FALSE;
	}

	TCHAR url[ MAX_PATH ] = { 0 };
	_stprintf( url, _T( "%s%s" ), path, fileName );

	instance.mCallBackFunctionPursuit	= callBackFunctionPursuit;

	// 080625 LUJ, curl_easy_init()를 런타임 중에 링킹하여 호출하도록 함
	{
		typedef CURL* ( __cdecl *Function )();
		Function function = ( Function )GetProcAddress( libraryModule, "curl_easy_init" );

		if( ! function )
		{
			ASSERT( 0 );
			return FALSE;
		}

		curl = function();
	}

	// 080625 LUJ, curl_easy_setopt()를 런타임 중에 링킹하여 호출하도록 함
	{
		typedef void ( __cdecl *Function )( CURL*, CURLoption, ... );
		Function function = ( Function )GetProcAddress( libraryModule, "curl_easy_setopt" );

		if( ! function )
		{
			ASSERT( 0 );
			return FALSE;
		}

		char errorCode[ CURL_ERROR_SIZE ] = { 0 };
		function( curl, CURLOPT_ERRORBUFFER,		errorCode );	
		function( curl, CURLOPT_HEADER,				0 );
		function( curl, CURLOPT_FOLLOWLOCATION,		1 );
		function( curl, CURLOPT_NOPROGRESS,			0 );
		function( curl, CURLOPT_WRITEDATA,			isMemoryDownload ? ( void* )( &memoryFile ) : ( void* )( &diskFile ) );
		function( curl, CURLOPT_WRITEFUNCTION,		isMemoryDownload ? CNetwork::WriteToMemory : CNetwork::WriteToDisk );
		function( curl, CURLOPT_PROGRESSDATA,		0 );
		function( curl, CURLOPT_PROGRESSFUNCTION,	CNetwork::Pursuit );
		function( curl, CURLOPT_URL,				CW2AEX< sizeof( url ) >( url ) );
	}

	// 080625 LUJ, curl_easy_perform()를 런타임 중에 링킹하여 호출하도록 함
	{
		typedef CURLcode ( __cdecl *Function )( CURL* );
		Function fucntion = ( Function )GetProcAddress( libraryModule, "curl_easy_perform" );

		if( ! fucntion )
		{
			ASSERT( 0 );
			return FALSE;
		}

		try
		{
			const CURLcode result = fucntion( curl );

			// 080625 LUJ, curl_easy_init()를 런타임 중에 링킹하여 호출하도록 함
			{
				typedef void ( __cdecl *Function )( CURL* );
				Function function = ( Function )GetProcAddress( libraryModule, "curl_easy_cleanup" );

				if( ! function )
				{
					ASSERT( 0 );
					return FALSE;
				}

				function( curl );
			}

			curl = 0;

			if( isMemoryDownload )
			{
				// 081202 LUJ, 파일 포인터를 처음으로 이동시켜야 읽을 수 있다
				memoryFile.SeekToBegin();
			}
			else
			{
				// 081201 LUJ, 파일을 반드시 수동으로 닫아줘야 한다. 그렇지 않을 경우 내려받은 파일을 열 수 없다
				diskFile.Close();
			}

			// TODO: write error code by case
			switch( result )
			{
			case CURLE_OK:
			case CURLE_OPERATION_TIMEDOUT:
				{
					CLog::Put( _T( "Download is done" ) );
					break;
				}
			default:
				{
					// 081202 LUJ, 잘못 받은 파일을 삭제한다
					DeleteFile( fileName );
					CLog::Put(
						_T( "Download is failed. %s is deleted" ),
						fileName );

					// 080702 LUJ, 정확한 에러를 표시할 수 있도록 오류 메시지를 표시한다
					typedef const char* ( __cdecl *Function )( CURLcode );
					Function function = ( Function )GetProcAddress( libraryModule, "curl_easy_strerror" );

					if( ! function )
					{
						break;
					}

					CLog::Put( CString( function( result ) ) );
					return FALSE;
				}
			}
		}
		catch( ... )
		{
		}
	}
	return TRUE;
}

CZipMemFile& CNetwork::GetMemoryFile() const
{
	static CZipMemFile file;

	return file;
}

CFile& CNetwork::GetDiskFile() const
{
	static CFile file;

	return file;
}