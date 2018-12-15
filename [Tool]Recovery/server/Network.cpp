#include "stdafx.h"
#include "Network.h"
#include "MSSystem.h"
#include "..\..\[CC]ServerModule\network_guid.h"


GLOBALTON(CNetwork)

CNetwork::CNetwork()
{
	CoInitialize(NULL);
	m_pINet = NULL;

	ZeroMemory( &mAddress, sizeof( mAddress ) );
}

CNetwork::~CNetwork()
{
	CoUninitialize();
}
void CNetwork::Release()
{
	if(m_pINet)
	{
		m_pINet->Release();
		m_pINet = NULL;
	}
}


bool CNetwork::Init(DESC_NETWORK * desc)
{
	HRESULT hr = CoCreateInstance( CLSID_4DyuchiNET, NULL, CLSCTX_INPROC_SERVER, IID_4DyuchiNET, (void**)&m_pINet);
	if (FAILED(hr))
		return false;
	if(!m_pINet->CreateNetwork( desc, 0, 0, 0 ) )
		return false;

	return true;
}


bool CNetwork::StartServer()
{
	printf( "\n" );

	const WORD	port					= 23900;
	char		address[ MAX_PATH ]		= { 0 };
	char		hostName[ MAX_PATH ]	= { 0 };
	gethostname( hostName, sizeof( hostName ) );
	HOSTENT FAR* lphostent = ::gethostbyname( hostName );

	// 081009 LUJ, 설정 파일을 읽어들여 선택된 IP를 얻어낸다
	{
		std::ifstream infile( "RecoveryServer.cfg" );

		if( infile )
		{
			printf( "... loading RecoveryServer.cfg\n" );

			while( ! infile.eof() )
			{
				TCHAR line[ MAX_PATH ] = { 0 };
				infile.getline( line, sizeof( line ) / sizeof( *line ) );

				const TCHAR* separator	= "\t= ";
				const TCHAR* token		= _tcstok( line, separator );

				if( ! token )
				{
					continue;
				}
				else if( ! _tcsicmp( token, "server" ) )
				{
					token = _tcstok( 0, separator );
					StringCopySafe( address, token ? token : _T( "" ), sizeof( address ) / sizeof( *address ) );
					_tprintf(
						"\tIP: %s:%u\t",
						address,
						port);
				}
			}

			if( m_pINet->StartServerWithUserSide( address, port ) )
			{
				StringCopySafe( mAddress.mIp, address, sizeof( address ) );
				mAddress.mPort	= port;
				return true;
			}
			else
			{
				_tprintf( "... Select IP manually\n\n" );
			}
		}
	}

	typedef std::list< std::string >	AddressList;
	AddressList							addressList;

	for( int i = 0; ; ++i )
	{
		const in_addr* ip = ( in_addr* )lphostent->h_addr_list[ i ];

		if( ! ip )
		{
			break;
		}

		char text[ MAX_PATH ] = { 0 };
		StringCopySafe( text, inet_ntoa( *ip ), sizeof( text ) );
		printf(
			"\t IP #%d: %s\t\n",
			i,
			text);
		addressList.push_back( text );
	}

	
	if( 1 == addressList.size() )
	{
		const std::string& text = *( addressList.begin() );
		StringCopySafe( address, text.c_str(), sizeof( address ) / sizeof( *address ) );
	}
	// 081009 LUJ, 외부 네트워크 주소가 두 개이상 있을 경우 관리자가 선택하도록 한다
	else
	{
		while( true )
		{
			int count = 0;

			printf( "\nSelect address to begin server: " );

			const char	character	= tolower( getch() );
			const DWORD	input		= character - '0';

			if( input > addressList.size() )
			{
				printf( "Input range was over. Retry again" );
				continue;
			}

			AddressList::iterator it = addressList.begin();
			std::advance( it, input );

			if( addressList.end() == it )
			{
				printf( "Input range was over. Retry again" );
				continue;
			}

			StringCopySafe( address, it->c_str(), sizeof( address ) / sizeof( *address ) );
			break;
		}
	}

	if( ! strlen( address ) )
	{
		printf( "No available address is. Check network environment on this computer.\n" );
		return false;
	}

	WORD userPort = port;

	while(false == m_pINet->StartServerWithUserSide( address, userPort ))
	{
		_tprintf(
			_T( "\n...Trying %u port" ),
			++userPort);

		const DWORD maxTriedPortSize = 10;

		if((userPort - port) > maxTriedPortSize)
		{
			return false;
		}
	}
	
	StringCopySafe( mAddress.mIp, address, sizeof( address ) );
	mAddress.mPort	= userPort;

	// 복구툴 서버가 이미 떠 있는 경우 포트 값이 일치하지 않는다. 수동 선택해야하므로 저장하지 않는다
	if(port == userPort)
	{
		std::ofstream outFile( "RecoveryServer.cfg" );
		outFile << _T( "server = " ) << mAddress.mIp << std::endl;
		printf( "... saving RecoveryServer.cfg\n" );
	}

	// 제목줄에 네트워크 정보를 함께 표시한다
	{
		TCHAR consoleTitle[MAX_PATH] = {0};
		GetConsoleTitle(consoleTitle, sizeof(consoleTitle) / sizeof(*consoleTitle));

		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			"%s, %s:%u",
			consoleTitle,
			mAddress.mIp,
			mAddress.mPort);
		SetConsoleTitle(text);
	}

	return true;
}


void CNetwork::Send(DWORD connectionIndex, const MSGROOT& message, DWORD size)
{
	m_pINet->SendToUser(connectionIndex, ( char* )&message, size, FLAG_SEND_NOT_ENCRYPTION);
}


void CNetwork::GetUserAddress(DWORD dwConnectionIndex, char* ip, WORD * port)
{
	m_pINet->GetUserAddress(dwConnectionIndex, ip, port);
}


void CNetwork::DisconnectUser(DWORD connectionIndex )
{
	m_pINet->CompulsiveDisconnectUser( connectionIndex );
}


const CNetwork::Address& CNetwork::GetAddress() const
{
	return mAddress;
}