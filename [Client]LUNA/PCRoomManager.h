#pragma once

#define PCROOMMGR CPCRoomManager::GetInstance()

class CPCRoomManager
{
	DWORD m_dwHeroPoint;			// HERO의 PC방 Point

	cPtrList m_PCRoomMemberList;
	PTRLISTPOS m_pListPos[MAX_PCROOM_MEMBER_NUM/MAX_PCROOM_MEMBER_LIST];

public:
	CPCRoomManager();
	~CPCRoomManager();

	static CPCRoomManager* GetInstance() { static CPCRoomManager pcroommgr; return &pcroommgr; }

	void SetHeroPoint( DWORD dwPoint ) { m_dwHeroPoint = dwPoint; }
	DWORD GetHeroPoint() const { return m_dwHeroPoint; }

	BOOL CanBuyItem( DWORD dwItemIdx, DWORD dwBuyCnt );

	void NetworkMsgParse( BYTE Protocol, void* pMsg );

	// 100118 ONS PC방정보를 설정한다.
	void ClearMemberInfo();
	void SetMemberInfo(PCROOM_MEMBER* pInfo,BYTE count);
	void GetMemberInfoList(PCROOM_MEMBER_LIST* rtInfo, int num);

};