#pragma once

class CPlayer;
class CMonster;

// taiyo
// 회복 내력, 회복 생명력 시간 계산 --------------------------------------------------
struct RECOVER_TIME
{
	RECOVER_TIME():lastCheckTime(0),bStart(TRUE){}
	BOOL bStart;
	DWORD lastCheckTime;
};
// 약물에 의한 회복
struct YYRECOVER_TIME
{
	YYRECOVER_TIME():bStart(FALSE){}
	BOOL bStart;
	BYTE count;
	DWORD recoverUnitAmout;
	DWORD lastCheckTime;
	DWORD recoverDelayTime;
};

// 100104 LUJ, 몬스터 상태 처리 인자
struct StateParameter
{
	DWORD AttackStartTime;
	DWORD SearchLastTime;
	DWORD CollSearchLastTime;
	DWORD CurAttackKind;
	DWORD PursuitForgiveStartTime;
	DWORD prePursuitForgiveTime;
	CMonster* pHelperMonster;
	CPlayer* pTarget;
	eMONSTER_ACTION stateNew;
	eMONSTER_ACTION stateCur;
	eMONSTER_ACTION stateOld;
	DWORD nextTime;
	BOOL bStateUpdate;
};

#pragma pack(push, 1)

struct SERVERINFO
{
	SERVERINFO()	{	memset(this,0,sizeof(SERVERINFO));	}
	SERVERINFO(const SERVERINFO* pInfo)	{	memcpy(this,pInfo,sizeof(SERVERINFO));	}
	SERVERINFO(WORD kind, WORD num, LPCTSTR ipsvr, WORD portsvr, LPCTSTR ipusr,WORD portusr)
		: wServerKind(kind),wServerNum(num),wPortForServer(portsvr),wPortForUser(portusr)
	{
		_tcscpy(szIPForServer,ipsvr);
		_tcscpy(szIPForUser,ipusr);
		dwConnectionIndex = 0;
		wAgentUserCnt = 0;
	}
	WORD	wServerKind;						// 서버 종류 : agent, map, distribute
	TCHAR	szIPForServer[MAX_IPADDRESS_SIZE];
	TCHAR	szIPForUser[MAX_IPADDRESS_SIZE];	
	WORD	wPortForServer;						// 서버 포트
	WORD	wPortForUser;						// 유저 포트
	WORD	wServerNum;							// 0 ~ (멥번호 혹은 서버번호)

	DWORD	dwConnectionIndex;					// 커넥션 인덱스 : 고유인덱스

	//--------agent field----------------
	WORD	wAgentUserCnt;
};
// powerup 관련 구조체 /////////////////////////////////////////////
struct MSG_PWRUP_BOOTUP : public MSGROOT
{
	DWORD dwProcessID;
	SERVERINFO BootInfo;
};

struct MSG_PWRUP_BOOTLIST : public MSGROOT
{
	void	AddList(SERVERINFO * pInfo)
	{
		BootList[Num++] = *pInfo;
	}
	DWORD GetSize() const { return sizeof( MSG_PWRUP_BOOTLIST ) - ( sizeof( BootList ) / sizeof( *BootList ) - Num ) * sizeof( *BootList ); }
	WORD		MapServerPort;
	WORD		Num;
	SERVERINFO BootList[ MAX_MAP_NUM ];
};
struct MSG_PWRUP_CONNECT_ACK : public MSGROOT
{
	SERVERINFO BootList;
	//WORD Key;
};
struct MSG_PWRUP_CONNECT_SYN : public MSGROOT
{
	SERVERINFO BootList;
};

struct MSGMC :public MSGROOT
{
	DWORD dwConnectionIdxMC;
};
struct MSGUSERCOUNT :public MSGMC
{
	WORD ServerPort;
	DWORD dwUserCount;
};
struct MSGEXECUTESERVER : public MSGMC
{
	WORD ServerType;
	WORD ServerNum;
	WORD ServerPort;
	char ServerIP[MAX_IPADDRESS_SIZE];
};
struct MSGSHUTDOWNSERVER : public MSGEXECUTESERVER
{
};

struct MSGNOTICE : public MSGMC
{
	char Msg[256];
};

struct MSERVERINFO
{
	MSERVERINFO()
	{
		*szIPForServer = 0;
		wPortForServer = 0;
		dwConnectionIndex = 0;
	}
	char	szIPForServer[MAX_IPADDRESS_SIZE];
	WORD	wPortForServer;						// 서버 포트
	char	szIPForUser[MAX_IPADDRESS_SIZE];
	WORD	wPortForUser;
	WORD	wServerType;
	WORD	wServerNum;

	DWORD	dwConnectionIndex;					// 커넥션 인덱스 : 고유인덱스

	DWORD	ProcessID;							//mapserver
	__time64_t	ExpiredTime;							//mapserver
	WORD	wServerState;							//mapserver

	// 090709 --- ServerTool 에서 Off되는 경우 TRUE 셋팅
	BOOL	bOffByServerTool;
};

struct GUILDMEMBERINFO_ID
{
	GUILDMEMBERINFO MemberInfo;
	DWORD GuildIdx;
};

struct MSGMSINFO : public MSGROOT
{
	MSERVERINFO MSInfo;
};

enum
{
	SERVERSTATE_NONE =0,
	SERVERSTATE_OFF=1,
	SERVERSTATE_ON=2,
	SERVERSTATE_ABNORMAL=3,
	SERVERSTATE_RECOVER=4
};

enum
{
	AUTOPATCHSTATE_START = 0,
	AUTOPATCHSTATE_PROC,
	AUTOPATCHSTATE_END,
};

struct SERVERSTATE
{
	WORD ServerType;
	WORD ServerNum;
	WORD ServerPort;
	char ServerIP[MAX_IPADDRESS_SIZE];
	WORD ServerState;
	int	 UserLevel;
	DWORD UserCount;
};

#define MAX_SERVERSTATE_NUM	128
struct MSGSERVERSTATE : public MSGMC
{
	SERVERSTATE serverState;

	// 090709 --- ServerTool 에서 Off되는 경우 TRUE 셋팅
	BOOL bOffByServerTool;
};

struct MSGMONITORTOOLALLSTATE : public MSGROOT
{

	void AddServerState(SERVERSTATE * pState)
	{
		serverState[num++] = *pState;
	}
	int GetSize()
	{
		return sizeof(MSGMONITORTOOLALLSTATE) - (MAX_SERVERSTATE_NUM - num)*sizeof(SERVERSTATE);
	}
	int num;
	SERVERSTATE serverState[MAX_SERVERSTATE_NUM];
};

struct MSSTATE
{
	char	sServerIP[MAX_IPADDRESS_SIZE];
	WORD	wServerPort;
	WORD	wAutoPatchState;
	char	sAutoPatchVersion[13];
};

struct MSGMSSTATE : public MSGROOT
{
	MSSTATE	MSState;
	MSGMSSTATE()
	{
		memset( &MSState, 0, sizeof(MSSTATE) );
	}
};

struct MSGMSSTATEALL : public MSGROOT
{
	int		nNum;
	MSSTATE	MSState[MAX_SERVERSTATE_NUM];	

	MSGMSSTATEALL()
	{
		memset( MSState, 0, sizeof(MSSTATE)*MAX_SERVERSTATE_NUM );
		nNum = 0;
	}
	void	AddMSServerState( MSSTATE* pState )
	{
		MSState[nNum++] = *pState;
	}
	int		GetSize()
	{
		return sizeof(MSGMSSTATEALL) - (MAX_SERVERSTATE_NUM-nNum)*sizeof(MSSTATE);
	}
};

// 06. 05 HIDE NPC - 이영준
struct HIDE_NPC_INFO
{
	WORD UniqueIdx;
	BOOL ChannelInfo[MAX_CHANNEL_NUM];
};

struct _PACKET_GAME
{
	int Packet_Type; // 패킷 타입
	int Packet_Result; // 처리 결과 값
	char Game_Server[32]; // 게임 서버와의 연결 ID
	char Session[32]; // GUID, user index를 사용해도 됨
	char User_CC[4]; // 구분 코드(company code)
	char User_No[20]; // 사용자 번호나 주민번호
	char User_ID[40]; // 사용자 아이디
	char User_IP[24]; // 사용자 접속 아이피
	char User_Gender[1]; // 사용자 성별. M: 남성, W/F: 여성, C: 법인
	char User_Status[3]; // 사용자 상태. HO: 개인, PC: 게임방, SP: ISP, PM: 프리미엄
	char User_PayType[4]; // 결제 수단
	int User_Age; // 사용자 나이
	int Game_No; // 게임 번호(과금 DB에 등록된 번호)
	char Bill_PayType[2]; // 지불 방식 A0: 후불, D0: 직불, P0: 선불
	char Bill_Method[2]; // 과금 방식. FM/FW: 월정액/주말 월정액, FD/DB/HB: 일 정액/기간 정액, TX: 코인 정액, TB: 종량
	char Bill_Expire[12]; // 과금 종료일 YYMMDD
	int Bill_Remain; // 남은 시간 및 포인트
};

struct MSG_PACKET_GAME : public MSGBASE
{
	_PACKET_GAME mPacketGame;
};

#pragma pack(pop)