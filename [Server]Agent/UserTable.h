// UserTable.h: interface for the CUserTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERTABLE_H__5BBB3509_10AF_4E68_9B3A_C07E9F23D2A9__INCLUDED_)
#define AFX_USERTABLE_H__5BBB3509_10AF_4E68_9B3A_C07E9F23D2A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

BOOL DisconnectUser(DWORD dwConnectionIndex);

#include <AntiCpSvrFunc.h>
#include "Crypt.h"


#ifdef _NPROTECT_
class CCSAuth2;
#endif

struct aGAMEOPTION
{
	BYTE bNoFriend;
	BYTE bNoWhisper;
	BYTE bNoDateMatching;

	aGAMEOPTION():bNoWhisper(0), bNoFriend(0), bNoDateMatching(0) {}
};

struct CHARSELECTINFO
{
	DWORD dwCharacterID;
	LEVELTYPE Level;
	MAPTYPE MapNum;
	BYTE Gender;

	// 080331 LYW --- UserTable : Add some variable for chatroom system.
	//WORD wClassIndex ;
	char name[MAX_NAME_LENGTH+1] ;
};

typedef struct tagUSERINFO
{
	BOOL	bForceMove;
	// 091106 LUJ, 채널을 강제로 지정해준다
	BOOL	bForceChannel;
	DWORD	dwConnectionIndex;
	DWORD	dwCharacterID;
	DWORD	dwUserID;
	BYTE	UserLevel;
	DWORD	dwMapServerConnectionIndex;
	WORD	wUserMapNum;
	CHARSELECTINFO SelectInfoArray[MAX_CHARACTER_NUM];

	DWORD	DistAuthKey;			// Distribute 서버에서 보내온 AuthKey 보관
//KES 연속 채팅 금지
	DWORD	dwLastChatTime;	

//---KES CheckSum 071003
	BYTE	cbCheckSum;
//----------------------

	WORD	wChannel;
	DWORD	dwUniqueConnectIdx;	// 서버에서의 고유 연결 번호
								// dwConnectionIdx와는 다름. 계속 증가함

	CCrypt crypto;
	aGAMEOPTION GameOption;
	DWORD dwLastConnectionCheckTime;
	BOOL bConnectionCheckFailed;
	DATE_MATCHING_INFO DateMatchingInfo;
	DWORD mFamilyIndex;
	DWORD nFamilyLeaveDate;
	int	nFamilyLeaveKind;
	int	nSexKind;
	UINT nChallengeZoneEnterID;
	UINT nChallengeZoneEnterSection;
	UINT nChallengeZoneReturnMapNum;
	BYTE byAddedChatSystem;
	DWORD dwInvitePartyByNameLastTime; // 마지막 초대요청 시각 저장
	DWORD dwInvitePartyByNameDelayTime; // 제한횟수 초과에 대한 딜레이 시간
	BYTE nInvitePartyByNameCnt; // 초대요청 누적 횟수 (일정시간이내 재요청시 누적, 일정횟수 누적시 행동제한)
	BOOL bIsSearchingByName; // 이름검색 실행중 여부

#ifdef _HACK_SHIELD_
	unsigned char m_GUIDInfo[SIZEOF_GUIDREQINFO];
	unsigned char m_Info[SIZEOF_REQINFO];
	PHSHIELD_CLIENT_CONTEXT m_pCrcInfo;
	BOOL m_bHSCheck;
#endif
#ifdef _NPROTECT_
	CCSAuth2	*m_pCSA;
	BOOL		m_bCSA;
	int			m_nCSAInit;
	DWORD		dwLastNProtectCheck;
#endif

	// CBT 이벤트
	BOOL	mLoginCheck;
	DWORD	mLoginCheckTime;
} USERINFO;

typedef void (__stdcall *USERDisconnectProc)(USERINFO* pInfo);					// 캐릭터가 disconnect되서 RemoveUser()가 호출될 때 호출 되는 콜백.

class CUserTable : public CYHHashTable<USERINFO>
{
	DWORD m_MaxUserCount;
	DWORD m_addCount;
	DWORD m_removeCount;
	USERDisconnectProc m_pOnDisconnectProc;

public:
	CUserTable();
	virtual ~CUserTable();
	void SetPositionUserHead() { SetPositionHead(); }
	USERINFO * GetUserData() { return GetData(); }
	void Init(DWORD dwBucket, USERDisconnectProc);
	USERINFO * FindUser(DWORD dwKey) { return GetData(dwKey); }
	BOOL AddUser(USERINFO*, DWORD dwKey);
	USERINFO* RemoveUser(DWORD dwKey);
	DWORD GetUserMaxCount() const { return m_MaxUserCount; }
	void SetCalcMaxCount(DWORD CurCount);
	
	BOOL SendToUser(DWORD dwKey,DWORD dwUniqueConnectIdx,MSGBASE* pmsg,DWORD size);
	BOOL OnDisconnectUser(DWORD dwKey);

	DWORD GetAddCount() { return m_addCount; }
	DWORD GetRemoveCount() { return m_removeCount; }
};
  


extern CUserTable * g_pUserTable;							// key : connectionidx
extern CUserTable * g_pUserTableForUserID;					// key : UserID
extern CUserTable * g_pUserTableForObjectID;				// key : dwObjectID

#endif // !defined(AFX_USERTABLE_H__5BBB3509_10AF_4E68_9B3A_C07E9F23D2A9__INCLUDED_)
