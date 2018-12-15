#pragma once

#define PCROOMMGR CPCRoomManager::GetInstance()
#define MAX_PCROOM_PROVIDE_ITEM_NUM		5		// PC방 지급 아이템 최대 개수


// PC방 지급 아이템 정보
struct stPCRoomItemInfo{
	DWORD dwItemIdx;
	WORD wItemCnt;
};

// PC방 버프 정보
struct stPCRoomBuffInfo{
	stPCRoomBuffInfo()
	{
		minLv = maxLv = 0;
	}

	LEVELTYPE minLv;					// 최소 레벨
	LEVELTYPE maxLv;					// 최대 레벨
	std::vector<DWORD> vecBuffIdx;
};

// 같은 PC방 버프 정보
struct stPCRoomPartyBuffInfo{
	stPCRoomPartyBuffInfo()
	{
		wConnectedNumMin = wConnectedNumMax = 0;
	}

	WORD wConnectedNumMin;				// 접속자 최소 인원
	WORD wConnectedNumMax;				// 접속자 최대 인원
	std::vector<DWORD> vecBuffIdx;
};

struct stPCRoomUserInfo{
	DWORD dwPlayerIdx;
	DWORD dwPCRoomIdx;					// PC방 Index
	DWORD dwLastProvidePointTime;		// 마지막 PC방 포인트 지급 시각
	DWORD dwPoint;						// PC방 현재포인트
	DWORD dwLoginPoint;					// 로그인시 PC방 포인트
	BOOL bCanProvideItem;				// PC방 아이템 지급 가능 여부
	BOOL bOpenMall;						// PC방 Mall Open 여부
	DWORD dwUserIdx;
	// 100105 ONS PC방 버프지급관련 변수추가
	BOOL  bPCRoomBuffProvided;									// PC방버프 지급 여부
	BYTE  byDay;												// 날짜에 따른 PC방 버프
	WORD  wBuffLevel;											// 레벨에 따른 PC방 버프
	WORD  wBuffStage;											// 파티원수에 따른 PC방 버프
	// 100812 ONS 유료/무료 가맹점정보
	GameRoomType eGameRoomType;					
};

class CPCRoomManager
{
	// Script 관련
	CYHHashTable<stPCRoomItemInfo>	m_htPCRoomItem[GameRoomTypeMaxCount];						// 지급 아이템 정보
	CYHHashTable<stPCRoomBuffInfo>	m_htPCRoomBuff[GameRoomTypeMaxCount];						// 버프 정보
	cPtrList						m_PCRoomPartyBuffList[GameRoomTypeMaxCount];				// 같은 PC방 버프 정보

	// 100310 ONS PC방 파티버프를 실시간의 적용을 위해 각PC방의 멤버수를 관리하도록 한다.
	typedef std::map< DWORD, WORD >	PCRoomMemberMap;
	PCRoomMemberMap					m_PCRoomMemberMap;					// < PC방 인덱스, PC방 멤버수 >

	WORD							m_wMallNPCIdx;						// PC방 상점 NPC Index

	DWORD							m_dwUpdatePointTime[GameRoomTypeMaxCount];				// PC방 포인트 적립 시간
	DWORD							m_dwUpdatePointValue[GameRoomTypeMaxCount];				// PC방 포인트 적립 수치
	
	struct stat						m_ScriptFileStat;


	// User Data 관련
	CYHHashTable<stPCRoomUserInfo>	m_PCRoomUser;						// PC방 유저 정보

	// 100305 ONS PC방버프정보 업데이트 상태를 정의한다.
	enum eBuffInfoState
	{
		eBuffInfoState_Init = 0,
		eBuffInfoState_Update,
		eBuffInfoState_End
	};

	typedef DWORD PlayerIndex;
	typedef std::map< PlayerIndex, _PACKET_GAME > PlayerPacketContainer;
	PlayerPacketContainer mPlayerPacketContainer;

public:
	CPCRoomManager();
	virtual ~CPCRoomManager();
	static CPCRoomManager* GetInstance();
	void SetPacketGame(PlayerIndex, const _PACKET_GAME&);
	void RemovePacketGame(PlayerIndex);
	DWORD GetGameRoomIndex(PlayerIndex) const;
	void Init();
	void Release();
    void ReleaseScript();
	void LoadScript();

	void Process();
	void ProvideItem(DWORD dwPlayerIdx);
	void AddPlayer(DWORD dwPlayerIdx, DWORD userIndex, DWORD dwPoint, WORD wMemberCount, BOOL bCanProvideItem, BOOL bPrivededBuff, BYTE byDay);
	void RemovePlayer(DWORD dwPlayerIdx);

	DWORD GetPoint( DWORD dwPlayerIdx );
	void SetPoint( DWORD dwPlayerIdx, DWORD dwPoint );
	DWORD UpdatePoint( DWORD dwPlayerIdx );													// 현재 시각으로 Point Update 후 반환

	WORD GetMallNPCIdx() const { return m_wMallNPCIdx; }

	void NetworkMsgParse( const MSGBASE* const pMsg, DWORD dwLength );

	// 100105 ONS 버프 부여 관련 함수.
	BOOL LoadBuffSkill( DWORD dwPlayerIdx );						
	void BuffSkillStart( DWORD dwPlayerIdx, std::vector<DWORD> BuffIndex );			
	void RemovePCRoomBuff( DWORD dwObjectIdx );
	void RemoveBuff( DWORD dwPlayerIdx, DWORD dwBuffIndex );
	void PCRoomBuffLoad(CPlayer&);
	// 100312 ONS 파티버프 및 파티원리스트 갱신관련 함수추가
	void SetPartyMemberCount( DWORD dwPartyIdx, WORD wMemberCount );
	void SetPartyMemberCountToOtherMap( DWORD dwPartyIdx, WORD wMemberCount );
	WORD GetPartyBuffStage( WORD wCurCount, GameRoomType GameRoomType );
	WORD GetMinimumOfStage(WORD wPartyStage, GameRoomType GameRoomType );
	WORD GetPartyMemberCount( DWORD dwPartyIdx );
	BOOL GetPartyBuffIndex( DWORD dwPCRoomIdx, GameRoomType GameRoomType, std::vector<DWORD>& pBuffIndex );
	BOOL GetDailyBuffIndex( LEVELTYPE PlayerLevel, std::vector<DWORD>& pBuffIndex, BYTE byDay, GameRoomType GameRoomType );
	BOOL GetPartyBuffIndexByStage( WORD wPartyStage, std::vector<DWORD>& pBuffIndex, GameRoomType GameRoomType );
	BYTE GetDayInfo();

	void UpdatePartyBuff( DWORD dwPlayerIdx, DWORD dwPCRoomIdx, GameRoomType GameRoomType);
	void UpdatePartyBuffToOtherMap( DWORD dwPlayerIdx, DWORD dwPCRoomIdx, GameRoomType GameRoomType );
	void UpdateRCRoomMemberList( MSG_PCROOM_MEMBER_LIST* pMsg );
	GameRoomType GetUserGameRoomType( DWORD playerIndex );

};