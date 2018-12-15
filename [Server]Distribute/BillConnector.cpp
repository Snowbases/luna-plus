// 선언을 하지 않으면 링크 오류가 발생한다
#define BUILDING_LIBCURL

#include "StdAfx.h"
#include "BillConnector.h"
#include "Network.h"
#include "ServerTable.h"
#include "ServerListManager.h"
#include "DistributeDBMsgParser.h"
#include <WinSock2.h>
#include "..\[CC]Header\curl\curl.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")

#define pgGame_Start	0x00000001	// 게임 시작
#define pgGame_End		0x00000002	// 게임 종료
#define pgServer_Alive	0x00000003	// 서버 확인
#define pgServer_Reset	0x00000004	// 서버 초기화
#define pgUser_Alert	0x00000005	// 사용자 알림
#define pgUser_Sync		0x00000006	// 사용자 확인
#define pgBilling_Auth	0x00000008	// 과금인증. 사용 불필요: 인증 목적으로 0x00000001만 쓰면 된다고 함(애드빌 소프트, 양병문 과장)
#define pgClient_Move	0x0000000A	// 사용자 게임서버 이동
#define pgServer_Conn	0x0000000C	// 서버연결

extern int g_nServerSetNum;

// curl/docs/examples/sendrecv.c
int wait_on_socket(SOCKET sockfd, int for_recv, long timeout_ms)
{
	struct timeval tv;
	fd_set infd, outfd, errfd;
	int res;

	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec= (timeout_ms % 1000) * 1000;

	FD_ZERO(&infd);
	FD_ZERO(&outfd);
	FD_ZERO(&errfd);

	FD_SET(sockfd, &errfd); /* always check for error */

	if(for_recv)
	{
		FD_SET(sockfd, &infd);
	}
	else
	{
		FD_SET(sockfd, &outfd);
	}

	/* select() returns the number of signalled sockets or -1 */
	res = select(sockfd + 1, &infd, &outfd, &errfd, &tv);
	return res;
}

CBillConnector::CBillConnector() :
mCurl(curl_easy_init()),
mSocket(0),
mLastConnectTick(0),
mConnectTryCount(0),
mCompanyCode("EYAP"),
mGameNumber(3) // 루나 고유 번호. 빌링 솔루션 제공사(애드빌 소프트)로부터 할당받은 번호.
{
	ZeroMemory(
		mGameServerName,
		sizeof(mGameServerName));
}

CBillConnector::~CBillConnector(void)
{
	curl_easy_cleanup(
		mCurl);
}

void CBillConnector::Connect(DWORD serverIndex)
{
	// 접속 중이므로 재시도 하지 않는다
	if(mSocket)
	{
		return;
	}
	else if(0 == mCurl)
	{
		SetInvalid();
		return;
	}

	char uniformResourceLocator[MAX_PATH] = {0};
	sprintf(
		uniformResourceLocator,
		"%s:%u",
		SERVERLISTMGR->GetBillingInfo().szIPForServer,
		SERVERLISTMGR->GetBillingInfo().wPortForServer);

	char errorBuffer[MAX_PATH] = {0};
	curl_easy_setopt(
		mCurl,
		CURLOPT_ERRORBUFFER,
		errorBuffer);
	curl_easy_setopt(
		mCurl,
		CURLOPT_URL,
		uniformResourceLocator);
	curl_easy_setopt(
		mCurl,
		CURLOPT_CONNECT_ONLY,
		1); 

	CURLcode curlCode = curl_easy_perform(
		mCurl);

	if(CURLE_OK != curlCode)
	{
		return;
	}

	curlCode = curl_easy_getinfo(
		mCurl,
		CURLINFO_LASTSOCKET,
		&mSocket);

	if(FAILED(mSocket))
	{
		return;
	}

	g_Console.LOG(
		0,
		"Connected to the billing server: %s",
		uniformResourceLocator);

	while(false == mSendPacketQueue.empty())
	{
		mSendPacketQueue.pop();
	}

	mUserCheckContainer.clear();

	_stprintf(
		mGameServerName,
		"%s%02d",
		"LUNA",
		g_nServerSetNum);

	{
		_PACKET_GAME packetGame = {0};
		packetGame.Packet_Type = pgServer_Conn;		
		Send(
			packetGame);
	}

	{
		_PACKET_GAME packetGame = {0};
		packetGame.Packet_Type = pgServer_Reset;		
		Send(
			packetGame);
	}

	OutputDebug(
		"Billing: auto authorization process: %u case",
		mUserPacketContainer.size());

	// 기존 접속자가 있을 경우 재인증 요청을 한다
	while(false == mUserPacketContainer.empty())
	{
		const UserPacketContainer::iterator iterator = mUserPacketContainer.begin();
		const _PACKET_GAME& packetGame = iterator->second;

		GameStart(
			_ttoi(packetGame.User_ID),
			packetGame.User_IP);

		mUserPacketContainer.erase(
			iterator);
	}
}

void CBillConnector::PacketResultAuth(const _PACKET_GAME& packetGame) const
{
	switch(packetGame.Packet_Result)
	{
	case 0:		// 인증성공
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Success. remained seconds: %u. user: %s",
			packetGame.Packet_Result,
			packetGame.Bill_Remain,
			packetGame.User_ID);
		break;
	case 2:		// DB에 접근 할 수 없음.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 3:		// 네트워크에 오류 발생.
		OutputDebug(
			"Billing: Packet_Result(%d)-Network Error has occurred. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 10:	// 인증정보 없음. 결제 정보가 DB에 없음.
		OutputDebug(
			"Billing: Packet_Result(%d)-You have insufficient points to play. Please top up.. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 11:	// 잔액 부족.
		OutputDebug(
			"Billing: Packet_Result(%d)-You have insufficient points to play. Please top up.. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 12:	// 세션이 없슴.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 17:	// 중복게임 사용자.
		OutputDebug(
			"Billing: Packet_Result(%d)-You have already connected another game.. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 18:	// 불법 사용자.
		OutputDebug(
			"Billing: Packet_Result(%d)-You are illegal user.. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 20:	// 정의 되지 않은 패킷.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 25:	// COM 에러
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 26:	// 지정된 시간동안(30초) 응답이 없음.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 27:	// 빌링 DB서버로의 Connect 실패.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 30:	// 패킷에 있는 게임 번호와 빌링 서버에 등록된 게임 번호가 다름.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 31:	// 빌링 어카운트의 서비스 모듈 에러.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 33:	// 해당게임의 구매정보 없음.
		OutputDebug(
			"Billing: Packet_Result(%d)-You have insufficient points to play. Please top up.. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 35:	// 과도한 인증 처리 요구에 의해 지금은 인증 처리 불가.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 100:	// 정의 되지 않은 오류.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 200:	// IP를 확인 하는 중 COM 에러가 발생.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 201:	// 해당 IP를 보유한 공급자의 정보가 부족.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 210:	// 인증 키를 확인하는 중 COM 에러가 발생.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 211:	// 인증 키를 찾지 못함.
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 301:	// 무료 이용자
		OutputDebug(
			"Billing: Packet_Result(%d)-Free user. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case 401:	// 서버 시작 메시지가 전달되지 않은 상태에서 다른 처리가 요청되었음
		OutputDebug(
			"Billing: Packet_Result(%d)-Billing Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	default:	// 알수없는 오류
		OutputDebug(
			"Billing: Packet_Result(%d)-Unknown Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	}
}

void CBillConnector::PacketResultAlert(const _PACKET_GAME& packetGame) const
{
	switch(packetGame.Packet_Result)
	{
	case 1:		// 사용자의 남은 시간(초 단위)
		OutputDebug(
			"Billing: remain seconds: %u. user: %s",
			packetGame.Bill_Remain);
		break;
	case 0:		// 남은시간 없음
		OutputDebug(
			"Billing: game stop. user: %s",
			packetGame.User_ID);
		break;
	case -1:	// 환불
		OutputDebug(
			"Billing:Packet_Result(%d). user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case -2:	// 중복 로그인
		OutputDebug(
			"Billing:Packet_Result(%d)-You are disconnected due to double login from other PC. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case -3:	// 예약되어 있던 결제 정보 사용 시작
		OutputDebug(
			"Billing:Packet_Result(%d). user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	case -4:	// 기타 강제 종료
		OutputDebug(
			"Billing:Packet_Result(%d). user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	default:	// 알수없는 오류
		OutputDebug(
			"Billing:Packet_Result(%d)-Unknown Error. user: %s",
			packetGame.Packet_Result,
			packetGame.User_ID);
		break;
	}
}

void CBillConnector::Receive(const _PACKET_GAME& packetGame)
{
	switch (packetGame.Packet_Type)
	{
		/*-- 설명
		사용자가 게임 서버에 접속하였을 때 게임을 시작해도 되는지 빌링서버에 확인
		인증 결과가 성공이면 빌링서버에서 사용량 계산을 시작 (종료 이벤트까지)
		*/
		/*-- 게임 서버 작업
		결과값이 성공이면 게임을 시작, 실패인 경우 클라이언트에게 결과값 전송후 게임 종료
		*/
	case pgGame_Start:
		{
			PacketResultAuth(
				packetGame);

			const UserIndex userIndex = _ttoi(
				packetGame.User_ID);
            const GameRoomType gameRoomType = GetGameRoomType(
				packetGame);

			if(GameRoomIsInvalid == gameRoomType)
			{
				mUserCheckContainer.erase(
					userIndex);

				// 접속했던 정보를 지우지 않으면 다른 IP에 접속해도 혜택을 받게 된다
				MSGBASE message;
				ZeroMemory(
					&message,
					sizeof(message));
				message.Category = MP_USERCONN;
				message.Protocol = MP_USERCONN_BILLING_START_NACK;
				message.dwObjectID = userIndex;

				g_Network.Broadcast2AgentServer(
					(char*)&message,
					sizeof(message));
				break;
			}

			OutputDebug(
				"User(%u) logged in at %s game room(%u)",
				userIndex,
				GameRoomIsPremium == gameRoomType ? "premium" : "free",
				packetGame.User_Age);

			mUserPacketContainer[userIndex] = packetGame;

			MSG_PACKET_GAME message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_USERCONN;
			message.Protocol = MP_USERCONN_BILLING_START_ACK;
			message.dwObjectID = userIndex;
			message.mPacketGame = packetGame;

			g_Network.Broadcast2AgentServer(
				(char*)&message,
				sizeof(message));
			break;
		}
	case pgServer_Alive:
		{
			_PACKET_GAME sendMsg = {0};
			sendMsg.Packet_Type = pgServer_Alive;
			Send(
				sendMsg);

			OutputDebug(
				"RT->ACCT::Packet_Type=Server_Alive\n");
			break;
		}
		/*-- 설명
		Bill_Method = FM/FW/FD/TX 인 경우 게임종료 24시간 전부터 매 1시간마다 전송 종료 1시간 전부터 매 10분마다 전송, 종료 10분전부터 매 1분마다 전송
		Bill_Method = DB/HB 인 경우 게임이 종료되기 5분전부터 1분에 한번씩 전송
		예약 구매를 하여도 메세징은 계속되며, 예약구매 여부를 확인한 시점에서 새로 구매된 결제내역을 메세징
		*/
		/*-- 게임 서버 작업
		빌링서버에서 받은 정보를 바탕으로 사용자에게 알림
		게임서버에서 Packet_Result가 1과 -3이 아닌 경우 사용자를 강제종료(0x00000002 종료 패킷 전송 안함)
		*/
	case pgUser_Alert:			// 수신전용
		{
			PacketResultAlert(
				packetGame);

			MSG_PACKET_GAME message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_USERCONN;
			message.Protocol = MP_USERCONN_BILLING_ALERT_ACK;
			message.mPacketGame = packetGame;
			message.dwObjectID = _ttoi(
				packetGame.User_ID);

			g_Network.Broadcast2AgentServer(
				(char*)&message,
				sizeof(message));
			break;
		}
		/*-- 설명
		네트워크 장애와 같은 문제로 게임종료 이벤트를 빌링 시스템이 처리 하지 못한경우 사용자가 게임을 아직 진행중인지 확인
		오래 접속되어 있는 사용자 여부 확인
		빌링서버는 사용자가 "OFF" 이면 종료 시간을 기준으로 과금종료
		종료시간을 알지 못하면 검사 주기만큼 시간을 빼고 계산
		*/
		/*-- 게임 서버 작업
		같은 세션값을 가진 사용자가 있는지 확인해서, 결과 리턴
		같은 세션값의 사용자가 있으면 Packet_Result에 1, 없으면 0 반환
		*/
	case pgUser_Sync:
		{
			if(0 != packetGame.Packet_Result)
			{
				break;
			}

			const UserIndex userIndex = _ttoi(
				packetGame.User_ID);

			// 이미 접속을 확인 중에 있다
			if(mUserCheckContainer.end() != mUserCheckContainer.find(userIndex))
			{
				break;
			}

			const AgentSize agentSize = g_pServerTable->GetDataNum();
			
			mUserCheckContainer.insert(
				std::make_pair(userIndex, agentSize));

			MSGBASE message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_USERCONN;
			message.Protocol = MP_USERCONN_BILLING_CHECK_SYN;
			message.dwObjectID = userIndex;

			g_Network.Send2AgentServer(
				(char*)&message,
				sizeof(message));

			OutputDebug(
				"RT->ACCT::Packet_Type=User_Sync(%s, %s)\n",
				packetGame.User_ID,
				packetGame.User_Status);
			break;
		}
	default:
		{
			OutputDebug(
				"RT->ACCT::Packet_Type=Unknown(%d)\n",
				packetGame.Packet_Type);
			break;
		}        
	}
}

// GameStart Method - 게임 시작 즉 과금 시작 정보를 알림.
void CBillConnector::GameStart(DWORD userIndex, LPCTSTR User_IP)
{
	if(0 == _tcsicmp(SERVERLISTMGR->GetBillingInfo().szIPForServer, "database"))
	{
		g_DB.LoginMiddleQuery(
			RGameRoomData,
			userIndex,
			"EXEC dbo.MP_GAMEROOM_SELECT \'%s\'",
			User_IP);
		return;
	}
	else if(IsInvalid())
	{
		return;
	}

	_PACKET_GAME packetGame = {0};
	ZeroMemory(
		&packetGame,
		sizeof(packetGame));
	packetGame.Packet_Type = pgGame_Start;

	SetGUID(
		packetGame);
	_stprintf(
		packetGame.User_ID,
		"%d",
		userIndex);
	SafeStrCpy(
		packetGame.User_IP,
		User_IP,
		_countof(packetGame.User_IP));
	
	Send(
		packetGame);
}

// GameEnd Method - 게임 종료 즉 과금 종료 요청 정보.
void CBillConnector::GameEnd(DWORD userIndex)
{
	if(mUserPacketContainer.end() == mUserPacketContainer.find(userIndex))
	{
		return;
	}

	_PACKET_GAME packet = mUserPacketContainer[userIndex];
	packet.Packet_Type = pgGame_End;
	Send(
		packet);

	mUserPacketContainer.erase(
		userIndex);
	mUserCheckContainer.erase(
		userIndex);
}

void CBillConnector::Send(_PACKET_GAME& packetGame)
{
	memcpy(
		packetGame.User_CC,
		mCompanyCode,
		_tcslen(mCompanyCode));
	SafeStrCpy(
		packetGame.Game_Server,
		mGameServerName,
		_countof(packetGame.Game_Server));

	{
		TCHAR uniqueIndex[MAX_PATH] = {0};
		memcpy(
			uniqueIndex,
			packetGame.Session,
			32);
	}

	packetGame.Packet_Type = htonl(packetGame.Packet_Type);
	packetGame.Packet_Result = htonl(packetGame.Packet_Result);
	packetGame.User_Age = htonl(packetGame.User_Age);
	packetGame.Game_No = htonl(mGameNumber);
	packetGame.Bill_Remain = htonl(packetGame.Bill_Remain);

	mSendPacketQueue.push(
		packetGame);
}

const _PACKET_GAME& CBillConnector::GetPacket(UserIndex userIndex) const
{
	static const _PACKET_GAME emptyPacket = {0};
	const UserPacketContainer::const_iterator iterator = mUserPacketContainer.find(
		userIndex);	

	return mUserPacketContainer.end() == iterator ? emptyPacket : iterator->second;
}

void CBillConnector::Process()
{
	if(IsInvalid())
	{
		return;
	}
	else if(0 == mSocket)
	{
		if(gCurTime < mLastConnectTick)
		{
			return;
		}

		Connect(
			g_nServerSetNum);

		if(0 == mSocket)
		{
			OutputDebug(
				"Billing server don't response(%d)",
				mConnectTryCount);
		}

		const DWORD stepTick = 5000;
		mLastConnectTick = gCurTime + stepTick;
		const DWORD maxTryCount = 10;

		if(maxTryCount < ++mConnectTryCount)
		{
			OutputDebug(
				"Distribute server don't try to connect any more");
			SetInvalid();
		}

		return;
	}
	else if(false == mSendPacketQueue.empty())
	{
		if(false == wait_on_socket(mSocket, 0, 1))
		{
			return;
		}

		_PACKET_GAME& packet = mSendPacketQueue.front();
		size_t sendLength = 0;

		const CURLcode curlCode = curl_easy_send(
			mCurl,
			LPVOID(&packet),
			sizeof(packet),
			&sendLength);

		if(CURLE_OK != curlCode)
		{
			mSocket = 0;

			g_Console.LOG(
				0,
				"Billing Server is disconnected");
			return;
		}

		mSendPacketQueue.pop();
	}

	wait_on_socket(
		mSocket,
		1,
		1);

	char buffer[1024] = {0};
	size_t receiveLength = 0;
	const CURLcode curlCode = curl_easy_recv(
		mCurl,
		buffer,
		_countof(buffer), 
		&receiveLength);

	if(CURLE_OK != curlCode)
	{
		return;
	}

	_PACKET_GAME* const packetGame = (_PACKET_GAME*)buffer;
	packetGame->Packet_Type = ntohl(packetGame->Packet_Type);
	packetGame->Packet_Result = ntohl(packetGame->Packet_Result);
	packetGame->User_Age = ntohl(packetGame->User_Age);
	packetGame->Game_No = ntohl(packetGame->Game_No);
	packetGame->Bill_Remain = ntohl(packetGame->Bill_Remain);

	Receive(
		*packetGame);
}

void CBillConnector::SucceedToCheck(UserIndex userIndex)
{
	if(mUserCheckContainer.end() == mUserCheckContainer.find(userIndex))
	{
		return;
	}

	mUserCheckContainer.erase(
		userIndex);

	const int isExist = 1;

	_PACKET_GAME packetGame = GetPacket(
		userIndex);
	packetGame.Packet_Type = pgUser_Sync;
	packetGame.Packet_Result = isExist;
	
	Send(
		packetGame);
}

void CBillConnector::FailToCheck(UserIndex userIndex)
{
	if(mUserCheckContainer.end() == mUserCheckContainer.find(userIndex))
	{
		return;
	}

	AgentSize& agentSize = mUserCheckContainer[userIndex];

	// 0 값이 들어가면 오류이다
	if(0 == agentSize--)
	{
		mUserCheckContainer.erase(
			userIndex);
		return;
	}
	// 다른 에이전트로부터 응답을 기다려야한다.
	// 에이전트가 다운될 경우 체크를 다시 하도록 컨테이너에 든 값을 일괄 1씩 줄인다
	else if(0 < agentSize)
	{
		return;
	}

	mUserCheckContainer.erase(
		userIndex);

	const int isNotExist = 0;

	_PACKET_GAME packetGame = GetPacket(
		userIndex);
	packetGame.Packet_Type = pgUser_Sync;
	packetGame.Packet_Result = isNotExist;

	Send(
		packetGame);
}

void CBillConnector::SetGUID(_PACKET_GAME& packetGame) const
{
	char uniqueIndex[MAX_PATH] = {0};
	GUID guid;
	CoCreateGuid(
		&guid);
	sprintf(
		uniqueIndex,
		"%04X%04X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		HIWORD(guid.Data1),
		LOWORD(guid.Data1),
		guid.Data2,
		guid.Data3,
		guid.Data4[0],
		guid.Data4[1],
		guid.Data4[2],
		guid.Data4[3],
		guid.Data4[4],
		guid.Data4[5],
		guid.Data4[6],
		guid.Data4[7]);
	memcpy(
		packetGame.Session,
		uniqueIndex,
		_countof(packetGame.Session));
}

void CBillConnector::Reset()
{
	mConnectTryCount = 0;
	mSocket = 0;
	mLastConnectTick = 0;
}