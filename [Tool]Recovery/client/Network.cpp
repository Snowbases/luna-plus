#include "stdafx.h"
#include "StdioFileEx.h"
#include "Network.h"
#include "client.h"


CNetwork::CNetwork() :
mInterface( 0 ),
mModule( 0 )
{
	//ZeroMemory( mConnectionIndex, sizeof( mConnectionIndex ) );
	//ZeroMemory( m_CheckSum, sizeof( m_CheckSum ) );

	LoadScript();
}


CNetwork::~CNetwork()
{
	Release();
}


void CNetwork::Release()
{
	AllDisconnect();

	if( mInterface )
	{
		mInterface->Release();
		mInterface = 0;
	}	
	
	if( mModule )
	{
		FreeLibrary( mModule );
		mModule = 0;
	}
}



static HWND mHandle;;

BOOL CNetwork::Initialise( HWND handle )
{
	Release();

	if( ! handle )
	{
		return FALSE;
	}

	mModule = LoadLibrary( _T( "Network.dll" ) );

	DllGetClassObject_BaseNetwork	pNetFunc = (DllGetClassObject_BaseNetwork)GetProcAddress(mModule,"DllGetClassObject");

	if( ! pNetFunc )
	{
		MessageBox( 0, _T( "Network.dll is not found. Run this program on Luna Online installed folder" ), _T( "" ), MB_ICONQUESTION | MB_OK );
		return FALSE;
	}

	HRESULT hr = pNetFunc(CLSID_SC_BASENETWORK_DLL, IID_SC_BASENETWORK_DLL, (void**)&mInterface);

	if( hr != S_OK )
	{
		MessageBox( 0, _T( "Network.dll is not found. Run this program on Luna Online installed folder" ), _T( "" ), MB_ICONQUESTION | MB_OK );
		return FALSE;
	}

	if( ! mInterface )
	{
		MessageBox( 0, _T( "COM interface binding is failed. Request to developer" ), _T( "" ), MB_ICONERROR | MB_OK );
		return FALSE;
	}

	{
		DESC_BASENETWORK desc	= { 0 };
		desc.wSockEventWinMsgID	= WM_SOCKEVENTMSG;	
		desc.OnConnect			= CNetwork::OnConnect;
		desc.OnDisconnect		= CNetwork::OnDisconnect;
		desc.ReceivedMsg		= CNetwork::OnReceive;

		mInterface->InitNetwork( handle, MAX_SERVERSET, &desc );
	}

	mHandle = handle;

	return TRUE;
}


BOOL CNetwork::Connect()
{	
	ASSERT( mInterface );

	if( ! mInterface )
	{
		return FALSE;
	}

	// 접속 불가능한 서버 빼놓고는 재접속을 시도해줘야한다. 접속 불가능한 서버에 접속 시도할 경우,
	// 기존 접속이 서버로 전송은 되나 서버에서 수신은 할 수어 없게 된다. 네트워크 모듈 내의 코드에 원인이 있는 것으로 추측된다.
	for( ServerMap::const_iterator it = mServerMap.begin(); mServerMap.end() != it; ++it )
	{
		const DWORD		serverIndex	= it->first;
		const Server&	server		= it->second;

		char ip[ MAX_PATH ] = { 0 };

		StringCopySafe( ip, CW2AEX< MAX_PATH >( server.mIP ), sizeof( ip ) );

		const DWORD	connectionIndex	= mInterface->ConnectToServer( ip, server.mPort );

		if( connectionIndex )
		{
			mConnectionMap[ serverIndex ]	= connectionIndex;
		}
	}

	return 0 < mConnectionMap.size();
}


void CNetwork::Disconnect( DWORD serverIndex )
{
	ConnectionMap::iterator it = mConnectionMap.find( serverIndex );

	if( mConnectionMap.end() != it )
	{
		const DWORD connectionIndex = it->second;

		mInterface->CompulsiveDisconnect( connectionIndex );

		mConnectionMap.erase( it );
	}
}

void CNetwork::AllDisconnect()
{
	for( ConnectionMap::const_iterator it = mConnectionMap.begin(); mConnectionMap.end() != it; ++it )
	{
		const DWORD connectionIndex	= it->second;

		mInterface->CompulsiveDisconnect( connectionIndex );
	}

	mConnectionMap.clear();
}

void CNetwork::Send( DWORD serverIndex, const MSGROOT& message, size_t size )
{
	ConnectionMap::const_iterator it = mConnectionMap.find( serverIndex );

	if( mConnectionMap.end() != it )
	{
		const DWORD connectionIndex = it->second;

		mInterface->Send( connectionIndex, ( char* )( &message ), DWORD( size ) );
	}
}

void CNetwork::SendAll( const MSGROOT& message, size_t size )
{
	for( ConnectionMap::const_iterator it = mConnectionMap.begin(); mConnectionMap.end() != it; ++it )
	{
		const DWORD serverIndex	= it->first;

		Send( serverIndex, message, size );
	}
}


void CNetwork::OnConnect( DWORD dwConIndex )
{}


void CNetwork::OnDisconnect( DWORD dwConIndex )
{
	CNetwork& network = CNetwork::GetInstance();

	ConnectionMap& connectionMap = network.mConnectionMap;

	typedef std::set< DWORD >	DisconnectedServerSet;
	DisconnectedServerSet		disconnectedServerSet;

	// 080702 LUJ, 서버 목록 중 접속되지 않는 서버가 있을 경우, 네트워크 dll은 접속된 기존 서버에 대해서도 
	//				접속을 종료하도록 한다. 이를 막기 위해, 접속 종료 요청이 올 경우 재접속을 시도한다
	for( ConnectionMap::iterator it = connectionMap.begin(); connectionMap.end() != it; ++it )
	{
		{
			const DWORD connectionIndex = it->second;

			if( connectionIndex != dwConIndex )
			{
				continue;
			}
		}

		const DWORD serverIndex = it->first;

		CNetwork::ServerMap&				serverMap	= network.mServerMap;
		CNetwork::ServerMap::const_iterator server_it	= serverMap.find( serverIndex );

		if( serverMap.end() == server_it )
		{
			break;
		}

		const CNetwork::Server& server = server_it->second;

		char ip[ MAX_PATH ] = { 0 };
		strncpy( ip, CW2AEX< MAX_PATH >( server.mIP ), sizeof( ip ) );

		{
			const DWORD connectionIndex = network.mInterface->ConnectToServer( ip, server.mPort );

			if( connectionIndex )
			{
				it->second = connectionIndex;
			}
			else
			{
				disconnectedServerSet.insert( serverIndex );
			}
		}
		
		break;
	}

	for(
		DisconnectedServerSet::const_iterator it = disconnectedServerSet.begin();
		disconnectedServerSet.end() != it;
		++it )
	{
		const DWORD serverIndex = *it;
		connectionMap.erase( serverIndex );
	}

	// 080808 LUJ, 접속된 곳이 없으면 강제 종료하자
	if( connectionMap.empty() )
	{
		CString textMessage;
		textMessage.LoadString( IDS_STRING360 );
		CString textExit;
		textExit.LoadString( IDS_STRING292 );
		MessageBox(
			0,
			textMessage,
			textExit,
			MB_OK | MB_ICONSTOP );

		CWnd* window = AfxGetMainWnd();

		if( window )
		{
			window->PostMessage( WM_CLOSE, 0, 0 );
		}
	}
}

void CNetwork::OnReceive( DWORD connectionIndex, char* pMsg, DWORD size )
{
	const MSGROOT*	message		= ( MSGROOT* )pMsg;
	BOOL			isReceive	= FALSE;

	CNetwork&				network			= CNetwork::GetInstance();
	const ConnectionMap&	connectionMap	= network.mConnectionMap;
	DWORD					serverIndex		= 0;

	for( ConnectionMap::const_iterator it = connectionMap.begin(); connectionMap.end() != it; ++it )
	{
		if( it->second == connectionIndex )
		{
			isReceive	= TRUE;
			serverIndex	= it->first;

			break;
		}
	}

	/*
	for( POSITION position = network.mConnection.GetStartPosition(); position; )
	{
		DWORD serverIndexInMap;
		DWORD connectionIndexInMap;

		network.mConnection.GetNextAssoc( position, serverIndexInMap, connectionIndexInMap );

		if( connectionIndex == connectionIndexInMap )
		{
			isReceive = TRUE;
			break;
		}
	}
	*/
	
	if( ! isReceive )
	{
		return;
	}

	CclientApp* application = ( CclientApp* )AfxGetApp();
	ASSERT( application );

	application->Parse( serverIndex, message );	
}


CNetwork& CNetwork::GetInstance()
{
	static CNetwork instance;

	return instance;
}


void CNetwork::LoadScript()
{
	const CString name( _T( "RecoveryClient.cfg" ) );

	CStdioFileEx file;

	if( ! file.Open( name, CFile::modeRead  ) )
	{
		CString textFileIsNotFound;
		textFileIsNotFound.LoadString( IDS_STRING308 );

		CString text;
		text.Format( textFileIsNotFound, name );

		MessageBox( 0, text, _T( "" ), MB_ICONERROR | MB_OK );

		AfxGetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );
		return;
	}

	enum
	{
		eModeServer,
		eModeNone,
	}
	mode = eModeNone;

	CString line;

	DWORD serverIndex = 0;

	while( file.ReadString( line ) )
	{
		int i = 0;

		const CString	seperator( _T( "\"\n= " ) );
		CString			token = line.Tokenize( seperator, i );

		if( token.IsEmpty() )
		{
			continue;
		}
		else if( _T( "[server]" ) == token  )
		{
			mode = eModeServer;
			continue;
		}
		else if( _T( "//" ) == token.Left( 2 ) )
		{
			continue;
		}

		switch( mode )
		{
		case eModeServer:
			{
				Server& data = mServerMap[ ++serverIndex ];
				SetConnectData(
					token,
					data);
				data.mName	= line.Tokenize( seperator, i );

				do 
				{
					token = line.Tokenize( seperator, i );

					data.mName += ' ' + token;
				}
				while( ! token.IsEmpty() );

				break;
			}
		}
	}
}

void CNetwork::SetConnectData(const CString& text, CNetwork::Server& server) const
{
	TCHAR buffer[MAX_PATH] = {0};
	_tcsncpy(
		buffer,
		text,
		sizeof( buffer ) / sizeof( *buffer ) );

	LPCTSTR seperator = _T( "\t: " );
	// 091012 LUJ, IP 정보를 얻는다
	server.mIP = _tcstok(
		buffer,
		seperator);
	// 포트 정보가 있다면 얻어온다
	LPCTSTR tokenPort = _tcstok(
		0,
		seperator);

	if( tokenPort )
	{
		server.mPort = WORD( _ttoi( tokenPort ) );
	}
	else
	{
		const WORD defaultPort( 23900 );
		server.mPort = defaultPort;
	}
}

DWORD CNetwork::GetServerIndex( const CString& name ) const
{
	for( ServerMap::const_iterator it = mServerMap.begin(); mServerMap.end() != it; ++it )
	{
		const Server& data = it->second;

		if( name == data.mName )
		{
			return it->first;
		}
	}

	return 0;
}


const TCHAR* CNetwork::GetServerName( DWORD serverIndex ) const
{
	ServerMap::const_iterator it = mServerMap.find( serverIndex );

	return mServerMap.end() == it ? _T( "" ) : it->second.mName;
}

// 081021 LUJ, 유효한 서버 인덱스 하나를 반환한다
DWORD CNetwork::GetServerIndex() const
{
	CNetwork::ConnectionMap::const_iterator it = mConnectionMap.begin();

	if( mConnectionMap.end() == it )
	{
		CString textAnyServerNotFoundToRequest;
		textAnyServerNotFoundToRequest.LoadString( IDS_STRING227 );

		MessageBox( 0, textAnyServerNotFoundToRequest, _T( "" ),MB_ICONERROR | MB_OK );
		return 0;
	}

	return it->first;
}