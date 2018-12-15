/*********************************************************************

	 파일		: SHDateManager.h
	 작성자		: hseos
	 작성일		: 2007/11/16

	 파일설명	: CSHDateManager 클래스의 헤더

 *********************************************************************/

#pragma once

class CPlayer;

#if !defined(_AGENTSERVER)
struct USERINFO;
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//
class CSHDateManager
{
public:
	//----------------------------------------------------------------------------------------------------------------
	static UINT					NOTICE_HEART_MATCHING_POING;							// 하트 알리미 매칭 포인트
	static UINT					ENTER_CHALLENGE_ZONE_MATCHING_POINT;					// 챌린지 존 입장 가능 매칭 포인트
	static UINT					ENTER_CHALLENGE_ZONE_FREQ_PER_DAY;						// 챌린지 존 입장 가능 회수(하루)
	static UINT					ENTER_CHALLENGE_ZONE_SECTION_NUM;						// 챌린지 존 구간 개수
	static DWORD				CHALLENGE_ZONE_START_DELAY_TIME;						// 챌린지 존 시작 대기 시간
	static DWORD				CHALLENGE_ZONE_END_DELAY_TIME;							// 챌린지 존 종료 대기 시간

	static UINT					CHALLENGE_ZONE_MOTION_NUM_START;						// 챌린지 존 시작 모션 번호
	static UINT					CHALLENGE_ZONE_MOTION_NUM_SUCCESS;						// 챌린지 존 성공 모션 번호
	static UINT					CHALLENGE_ZONE_MOTION_NUM_SUCCESS_LEAST_CLEAR_TIME;		// 챌린지 존 최단 시간 클리어 성공 모션 번호
	static UINT					CHALLENGE_ZONE_MOTION_NUM_FAIL;							// 챌린지 존 실패 모션 번호

	enum CHALLENGEZONE_STATE
	{
		CHALLENGEZONE_START				= 1,
		CHALLENGEZONE_END				= 10000000,										// == 가 아니라 > 일 때 END 임
		CHALLENGEZONE_END_TIMEOUT,
		CHALLENGEZONE_END_PARTNER_OUT,
		CHALLENGEZONE_END_ALL_DIE,
		CHALLENGEZONE_END_SUCCESS,
		CHALLENGEZONE_END_START_COUNTDOWN,
	};

	struct stCHALLENGEZONE_MONSTER
	{
		UINT	nMonsterKind;
		UINT	nMonsterNum;
		float	nPosX;
		float	nPosZ;
		DWORD	nDelayTime;
	};

	struct stCHALLENGEZONE_MONSTER_GROUP
	{
		stCHALLENGEZONE_MONSTER*		pstMonster;
		UINT							nMonsterNum;
	};

	struct stCHALLENGEZONE_MONSTER_GROUP_SECTION
	{
		stCHALLENGEZONE_MONSTER_GROUP*	pstGroup;
		UINT							nGroupNum;
		UINT							nTotalMonsterNum;
	};

	struct stCHALLENGEZONE_MAPINFO
	{
		UINT							nMapNum;
		UINT							nMoveIndex;
	};

	struct stCHALLENGEZONE_SECTION_MONLEVEL
	{
		DWORD							nStart;
		DWORD							nEnd;
		UINT							nLimitTime;
		UINT							nLeastClearTime;
	};

private:
	//----------------------------------------------------------------------------------------------------------------
	BOOL									m_bIsChallengeZone;

	UINT*									m_pnDateZoneMoveIndexList;
	UINT									m_nDateZoneMoveIndexNum;

	stCHALLENGEZONE_MAPINFO*				m_pstChallengeZoneMoveIndexList;
	UINT									m_nChallengeZoneMoveIndexNum;

	UINT									m_nChallengeZoneEnterNum;

	stCHALLENGEZONE_MONSTER_GROUP_SECTION*	m_pstChallengeZoneMonsterGroupSection;

	stCHALLENGEZONE_SECTION_MONLEVEL*		m_pstChallengeZoneSectionMonLevel;
	UINT									m_nChallengeZoneSectionNum;

	UINT									m_nChallengeZoneState;
	DWORD									m_nChallengeZoneTime;
	DWORD									m_nChallengeZoneTimeTick;

	BOOL									m_bChallengeZoneStart;
	DWORD									m_nChallengeZoneFirstEnterPlayerID;

	DWORD									m_nChallengeZoneLeastClearTime;

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHDateManager();
	~CSHDateManager();


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 공용
	//
	//----------------------------------------------------------------------------------------------------------------
	//								초기화
	VOID							Init();
	//								데이터 읽기
	BOOL 							LoadDateInfo();
	//								챌린지 존 몬스터 데이터 읽기
	BOOL 							LoadChallengeMonsterInfo(int nMapNum);
	//								챌린지 존 인가? 
	//								..현재 맵에 접속 중임이 확실할 때는 IsChallengeZoneHere 를 사용
	//								..맵 번호로 체크하거나 체크시점이 애매할 경우 IsChallengeZone 사용
	BOOL							IsChallengeZone(int nMapNum);
	BOOL							IsChallengeZoneHere()					{ return m_bIsChallengeZone; }

	//								데이트 존 이동 인덱스 얻기
	BOOL 							GetDateZoneMoveIndex(UINT nSelIndex, int* pnIndex);
	UINT 							GetDateZoneMoveIndexNum() const { return m_nDateZoneMoveIndexNum; }
	MAPTYPE							GetMapType(UINT index);
	BOOL 							GetChallengeZoneMoveIndex(UINT nSelIndex, int* pnIndex);
	//								챌린지 존 개수 얻기
	UINT 							GetChallengeZoneMoveIndexNum()			{ return m_nChallengeZoneMoveIndexNum; }

	//								챌린지 존 입장 회수 얻기
	UINT							GetChallengeZoneEnterNum()				{ return m_nChallengeZoneEnterNum; }
	//								챌린지 존 몬스터 정보 얻기
	stCHALLENGEZONE_MONSTER_GROUP*	GetChallengeZoneMonsterGroup(int nSection, int nIndex)	
									{ return &m_pstChallengeZoneMonsterGroupSection[nSection].pstGroup[nIndex]; }

	//								챌린지 존 시작 설정
	VOID							SetChallengeZoneStart(BOOL bStart)					{ m_bChallengeZoneStart = TRUE; }
	VOID							SetChallengeZoneFirstEnterPlayerID(DWORD nPlayerID)	{ m_nChallengeZoneFirstEnterPlayerID = nPlayerID; }

	//								메인 루프
	VOID							MainLoop();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 서버					
	//
	//----------------------------------------------------------------------------------------------------------------
	//	에이전트
	//								클라이언트 요청 분석/처리
	VOID							ASRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength);
	VOID							ASRV_ParseRequestFromServer(DWORD dwConnectionID, char* pMsg, DWORD dwLength);

	//----------------------------------------------------------------------------------------------------------------
	//	맵
	//								클라이언트 요청 분석/처리
	VOID							SRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength);
	//----------------------------------------------------------------------------------------------------------------

	//								데이트 존 입장
	VOID							SRV_EnterDateZone(CPlayer* pPlayer, DWORD nTargetPlayerID, int nZoneIndex);
	//								..에이전트가 다른 플레이어일 경우
	VOID							SRV_EnterDateZone2(CPlayer* pPlayer, MSG_DWORD4* pPacket);
	//								챌린지 존 입장
	VOID							SRV_EnterChallengeZone(CPlayer* pPlayer, DWORD nTargetPlayerID, int nZoneIndex, DWORD dwExpRate, int nKind = 0);
	//								..에이전트가 다른 플레이어일 경우
	VOID							SRV_EnterChallengeZone2(CPlayer* pPlayer, MSG_DWORD5* pPacket, int nKind = 0);

	//								챌린지 존 메인 프로세스
	VOID							SRV_Process(CPlayer* pPlayer);
	//								챌린지 존 시작
	VOID							SRV_StartChallengeZone(CPlayer* pPlayer, UINT nChannelNumFromAgent, UINT nSection);
	//								챌린지 존 시작 알림
	VOID							SRV_NotifyStartChallengeZone(CPlayer* pPlayer, char* pszName1, char* pszName2, DWORD dwExpRate1, DWORD dwExpRate2, UINT nSection);	

	//								챌린지 존 입장 회수 처리
	VOID							SRV_ProcChallengeZoneEnterFreq(CPlayer* pPlayer);
	//								챌린지 존 입장 회수 전송
	VOID							SRV_SendChallengeZoneEnterFreq(CPlayer* pPlayer);
	//								챌린지 존 몬스터 등장 처리
	VOID							SRV_ProcRegenMonster(CPlayer* pPlayer);
	//								챌린지 존 몬스터 죽음 처리
	VOID							SRV_ProcMonsterDie(CPlayer* pPlayer);
	//								챌린지 존 입장 보너스 회수 증가
	VOID							SRV_SetChallengeZoneEnterBonusFreq(CPlayer* pPlayer, UINT nFreq);

	//								챌린지 존을 끝냄
	VOID							SRV_EndChallengeZone(CPlayer* pPlayer, CHALLENGEZONE_STATE eEndState);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 클라이언트
	//
	//----------------------------------------------------------------------------------------------------------------
	//								데이트 존 입장이 가능한가?
	BOOL							CLI_IsPossibleEnterDateZone();
	//								챌린지 존 입장이 가능한가?
	BOOL							CLI_IsPossibleEnterChallengeZone();

	//								데이트 존 입장 요청
	VOID							CLI_RequestDateMatchingEnterDateZone(int nZoneIndex);
	//								챌린지 존 입장 요청
	VOID							CLI_RequestDateMatchingEnterChallengeZone(int nZoneIndex, DWORD wExpRate);

	//----------------------------------------------------------------------------------------------------------------
	//								서버로부터의 응답 분석/처리
	VOID							CLI_ParseAnswerFromSrv(void* pMsg);
};

extern CSHDateManager g_csDateManager;
