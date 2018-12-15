/*
목적: 애드빌 소프트에서 제공하는 빌링 서버와 통신하여, 유료 유저인지 확인한다
작업자: 이웅주
도움: 곽철중 팀장

게임에서 사용하는 inetwork.dll은 헤더를 사용해서 전송을 한다. 이러면 빌링 서버는
잘못된 패킷을 수신한다. 이를 피하기 위해 curl 7.20.1 라이브러리를 이용해서 사용
하여 구현했다.
*/
#pragma once
#include <queue>

class CBillConnector
{
private:
	const int mGameNumber;
	DWORD mLastConnectTick;
	DWORD mConnectTryCount;
	LPCTSTR mCompanyCode;
	char mGameServerName[32];
	LPVOID mCurl;
	SOCKET mSocket;

	typedef DWORD UserIndex;
	typedef std::map< UserIndex, _PACKET_GAME > UserPacketContainer;
	UserPacketContainer mUserPacketContainer;

	// 유저가 로그인 중인 검사하기 위해 사용한다. 해당 에이전트에 유저가 없다고
	// 에이전트에서 통보가 올 때마다 UserCheckFailedCount를 1씩 감소시킨다.
	// 에이전트 개수가 여러 개일 수 있다. 따라서 실패가 되더라도 UserCheckFailedCount = 0일
	// 때까지는 빌링 서버에 전송하지 않는다
	typedef size_t AgentSize;
	typedef std::map< UserIndex, AgentSize > UserCheckContainer;
	UserCheckContainer mUserCheckContainer;

	std::queue< _PACKET_GAME > mSendPacketQueue;

private:
	void PacketResultAuth(const _PACKET_GAME&) const;
	void PacketResultAlert(const _PACKET_GAME&) const;
	void Send(_PACKET_GAME&);
	const _PACKET_GAME& GetPacket(UserIndex) const;
	void SetGUID(_PACKET_GAME&) const;
	void Connect(DWORD serverIndex);
	void Receive(const _PACKET_GAME&);
	BOOL IsInvalid() const { return UINT_MAX == mSocket; }
	void SetInvalid() { mSocket = UINT_MAX; }

public:
	CBillConnector();
	virtual ~CBillConnector();
	void GameStart(UserIndex, LPCTSTR userIpAddress);
	// 사용자 접속 종료 후 호출된다
	void GameEnd(UserIndex);
	void Process();
	void SucceedToCheck(UserIndex);
	void FailToCheck(UserIndex);
	void Reset();
};