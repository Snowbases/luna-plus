// ServerSystem.h: interface for the CServerSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERSYSTEM_H__FD3EBFC3_EE3D_4505_A5A1_24DA471D20AB__INCLUDED_)
#define AFX_SERVERSYSTEM_H__FD3EBFC3_EE3D_4505_A5A1_24DA471D20AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ServerTable.h"
#include "UserTable.h"
#include "..\4dyuchigxgfunc/global.h"

//--- max user&server define ------
#define MAX_USER_IN_A_AGENT		4000
#define MAX_SERVER_CONNECTION	100
//---------------------------------

//-----------------------------------------------------------------------
// 맵전환 좌표기억 구조체
//-----------------------------------------------------------------------
typedef struct _MAPCHANGE_INFO
{
	DWORD Kind;
	char CurMapName[MAX_NAME_LENGTH];
	char ObjectName[MAX_NAME_LENGTH];
	WORD CurMapNum;
	WORD MoveMapNum;
	VECTOR3 CurPoint;
	VECTOR3 MovePoint;
	WORD chx_num;
	
	// 방향설정
}MAPCHANGE_INFO;


typedef struct _LOGINPOINT_INFO
{
	WORD Kind;
	char MapName[MAX_NAME_LENGTH+1];
	WORD MapNum;
	BYTE MapLoginPointNum;
	VECTOR3 CurPoint[10];
	WORD chx_num;

	// 방향설정
}LOGINPOINT_INFO;

enum eNATION
{
	eNATION_KOREA,
	eNATION_CHINA,
};


class CServerSystem  
{
	WORD			m_MapChannelNum[MAX_MAP_NUM];		// 각 맵의 채널 수
	MAPCHANGE_INFO	m_MapChange[MAX_POINT_NUM];
	LOGINPOINT_INFO m_Login[MAX_POINT_NUM];

//	INDEXCR_HANDLE	m_IdxCreater;
	WORD			m_wServerNum;
	
	BOOL			m_bUseCrypt;

//국가
	int		m_Nation;

//이벤트공지용
	char	m_strEventNotifyTitle[32];
	char	m_strEventNotifyContext[128];
	BOOL	m_bUseEventNotify;
	BOOL	m_bEvenList[eEvent_Max];
//

	BOOL	m_bTestServer;

	// 100301 ShinJS, 캐릭터들의 초기 스탯 정보
	typedef std::pair<RaceType, BYTE> CharBaseStatusInfoKey;
	typedef std::map<CharBaseStatusInfoKey, PlayerStat>	CharBaseStatusInfoMap;
	CharBaseStatusInfoMap m_CharBaseStatusInfoMap;

	// 맵 서버가 재시작될 경우를 대비해서 디스트리뷰트 서버에서 받은 빌링 정보를 저장해둔다
	typedef DWORD UserIndex;
	typedef std::map< UserIndex, _PACKET_GAME > UserPacketContainer;
	UserPacketContainer mUserPacketContainer;

public:
	CServerSystem();
	virtual ~CServerSystem();

	WORD GetServerNum() { return m_wServerNum; }
	WORD GetChannelNum(int nMapNum) {return m_MapChannelNum[nMapNum]-1;}
	void ConnectionCheck();
	void SendConnectionCheckMsg(USERINFO* pInfo);
	void SendSpeedHackCheckMsg(USERINFO* pInfo);

	void Process();
	void Start(WORD ServerNum);
	void End();		

	// PJH가 추가함
	void SetMapChangeInfo();
	MAPCHANGE_INFO* GetMapChangeInfo(DWORD Index);

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.29
	UINT GetMapChangeIndex(UINT nMapNum);
	// E 데이트 존 추가 added by hseos 2007.11.29

	void SetLoginInfo();
	LOGINPOINT_INFO* GetLoginInfo(DWORD Index);
	
	// KES
	void SendMsgMapServerDown( WORD wServerNum );
	void SetEnableCrypt( BOOL bEnable )		{ m_bUseCrypt = bEnable; }
	BOOL IsEnableCrypt()					{ return m_bUseCrypt; }

	void SetNation();
	int GetNation() { return m_Nation; }

//이벤트공지용
	void SetEventNotifyStr( char* pStrTitle, char* pStrContext );
	void SetUseEventNotify( BOOL bUse );
	BOOL IsUseEventNotify()			{ return m_bUseEventNotify; }
	char* GetEventNotifyTitle()		{ return m_strEventNotifyTitle; }
	char* GetEventNotifyContext()	{ return m_strEventNotifyContext; }
	void ResetApplyEvent()			{	memset( m_bEvenList, 0, sizeof(BOOL)*eEvent_Max );	}
	void SetApplyEvent( DWORD EventIdx )	{	m_bEvenList[EventIdx] = TRUE;	}
//
	
	//---KES 테섭인지 아닌지
	BOOL IsTestServer() { return m_bTestServer; }

//---	
	DWORD MakeAuthKey();
	void ReleaseAuthKey(DWORD key);

	// 100301 ShinJS --- 캐릭터들의 초기 스탯 정보	
	void LoadCharBaseStatusInfo();
	const PlayerStat& GetBaseStatus( RaceType race, BYTE byClass );
	const _PACKET_GAME& GetPacketGame(UserIndex) const;
	void SetPacketGame(UserIndex, const _PACKET_GAME&);
	void RemovePacketGame(UserIndex);
};


extern CServerSystem * g_pServerSystem;
// 080813 LUJ, 수정된 inetwork 호출 형식에 따라 변경
void __stdcall OnConnectServerSuccess(DWORD dwIndex, void* pVoid);
void __stdcall OnConnectServerFail(void* pVoid);
void __stdcall ReceivedMsgFromServer(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength);
void __stdcall ReceivedMsgFromUser(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength);
void __stdcall OnAcceptServer(DWORD dwConnectionIndex);
void __stdcall OnDisconnectServer(DWORD dwConnectionIndex);
void __stdcall OnAcceptUser(DWORD dwConnectionIndex);
void __stdcall OnDisconnectUser(DWORD dwConnectionIndex);
void ButtonProc1();
void ButtonProc2();
void ButtonProc3();
void OnCommand(char* szCommand);


#endif // !defined(AFX_SERVERSYSTEM_H__FD3EBFC3_EE3D_4505_A5A1_24DA471D20AB__INCLUDED_)
