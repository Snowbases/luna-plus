/*********************************************************************

	 파일		: SHFamilyManager.h
	 작성자		: hseos
	 작성일		: 2007/07/03

	 파일설명	: CSHFamilyManager 클래스의 헤더

 *********************************************************************/

#pragma once

class CPlayer;

#if !defined(_AGENTSERVER)
struct USERINFO;
#endif

#include "SHFamily.h"
#include <queue>

class CSHFamilyManager
{
public:
	//----------------------------------------------------------------------------------------------------------------
	enum FAMILY_CREATE_STATE																// 패밀리 생성 진행 상태
	{	
		FCS_NAME_CHECK1 = 0,																// ..DB에서 이름 체크
		FCS_NAME_ERR,																		// ..DB에 같은 이름 있음
		FCS_NAME_CHECK2,																	// ..서버 테이블에서 이름 체크
		FCS_MONEY_ERR,																		// ..필요 돈 부족
		FCS_CREATE,																			// ..패밀리 생성
		FCS_COMPLETE,																		// ..패밀리 생성 완료
	};

	enum FAMILY_INVITE_RESULT																// 패밀리 초대 결과
	{
		FIR_ACCEPT_MASTER = 0,																// ..초대 승락 (마스터에게)
		FIR_ACCEPT_PLAYER,																	// ..초대 승락 (플레이어에게)
		FIR_DENY_MASTER,																	// ..초대 거절 (마스터에게)
		FIR_DENY_PLAYER,																	// ..초대 거절 (플레이어에게)
	};

	enum FAMILY_LEAVE_KIND																	// 패밀리 탈퇴 종류
	{
		FLK_LEAVE = 0,																		// ..탈퇴
		FLK_EXPEL,																			// ..추방
		FLK_BREAKUP,																		// ..해체
		FLK_MAX,
	};

	//----------------------------------------------------------------------------------------------------------------
	static const int			MAX_FAMILY_UNIT_NUM				=	100;					// 패밀리 유닛의 최대 개수
	static const int			MAX_FAMILY_1U_NUM				=	100;					// 패밀리(1유닛당) 최대 개수

	static const int			EMBLEM_WIDTH					=	16;						// 문장 이미지 가로
	static const int			EMBLEM_HEIGHT					=	16;						// 문장 이미지 세로
	static const int			EMBLEM_BPP						=	2;						// 문장 이미지 BPP
	static const int			EMBLEM_BUFSIZE					=	EMBLEM_WIDTH*			// 문장 이미지 사이즈
																	EMBLEM_HEIGHT*
																	EMBLEM_BPP;

	static DWORD				HONOR_POINT_CHECK_TIME;										// 명예 포인트 지급 체크 시간
	static int					HONOR_POINT_CHECK_MEMBER_NUM;								// 명예 포인트 지급 체크 멤버 수
	static int					HONOR_POINT_CHECK_POINT;									// 명예 포인트 지급 포인트
	static float				MONKILL_HONOR_POINT_EXP_RATE;								// 명예 포인트로 지급되는 몬스터 킬 경험치 비율

	static float				LEAVE_PENALTY_EXP_DOWN_RATE[FLK_MAX];						// 패밀리 탈퇴 페널티 경험치 다운 비율
	static int					LEAVE_PENALTY_REJOIN_TIME[FLK_MAX];							// 패밀리 탈퇴 후 가입/창설에 필요한 시간


	//----------------------------------------------------------------------------------------------------------------
	struct PACKET_ADD_MEMBER: public MSGBASE												// 멤버 추가 패킷 구조
	{
		DWORD		nMasterID;																// ..마스터ID
		int			nRace;																	// ..종족
		int			nSex;																	// ..성별
		int			nJobFirst;																// ..직업(처음)
		int			nJobCur;																// ..직업(현재)
		int			nJobGrade;																// ..직업등급
		int			nLV;																	// ..레벨
		char		szName[MAX_NAME_LENGTH+1];												// ..이름
		BOOL		nResult;																// ..응답 결과
	};

	struct PACKET_MEMBER_INFO: public MSGBASE												// 멤버 정보 패킷 구조
	{
		int							nIndex;													// ..멤버 인덱스
		CSHFamilyMember::stINFO		stInfo;													// ..기본 정보
		CSHFamilyMember::stINFO_EX	stInfoEx;												// ..추가 정보
	};

	struct PACKET_FAMILY: public MSGBASE													// 패밀리 패킷 구조
	{
		CSHFamily::stINFO		stInfo;														// ..기본 정보
		CSHFamily::stINFO_EX	stInfoEx;													// ..추가 정보
	};

	struct stFAMILY																			// 패밀리 구조
	{
		CSHFamily::stINFO		stInfo;														// ..패밀리 기본 정보
		CSHFamily::stINFO_EX	stInfoEx;													// ..패밀리 추가 정보
	};

	struct stFAMILYMEMBER																	// 멤버 구조
	{
		int							nIndex;													// ..멤버 인덱스
		CSHFamilyMember::stINFO		stInfo;													// ..멤버 기본 정보
		CSHFamilyMember::stINFO_EX	stInfoEx;												// ..멤버 추가 정보
	};

	struct PACKET_FAMILY_TOTAL: public MSGBASE												// 패밀리 모든 정보 패킷 구조
	{
		stFAMILY		stFamily;															// ..패밀리 정보
		stFAMILYMEMBER	stMember[CSHFamily::MAX_MEMBER_NUM];								// ..멤버 정보
	};

	struct PACKET_FAMILY_EMBLEM : public MSGBASE											// 문장 패킷 구조
	{
		DWORD nFamilyID;																	// ..패밀리 ID
		DWORD nTargetPlayerID;																// ..대상 플레이어 ID
		DWORD nChangedFreq;																	// ..변경 회수
		char cImgData[EMBLEM_BUFSIZE];														// ..이미지 데이터
		// 091113 ONS 패밀리 문장의 Register/Load 구분
		BOOL  bIsLoad;					
	};

private:
	//----------------------------------------------------------------------------------------------------------------
	CSHFamily*					m_pcsFamily[MAX_FAMILY_UNIT_NUM];							// 패밀리 테이블
	int							m_nFamilyNum;												// 패밀리 개수

	DWORD						m_nInviteMasterID;											// 초대한 패밀리 마스터ID

#ifdef _AGENTSERVER
	typedef DWORD TickCount;
	typedef DWORD FamilyIndex;
	typedef std::pair< TickCount, FamilyIndex > TransferredFamily;
	std::queue< TransferredFamily > mReservedTransferContainer;
#endif

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHFamilyManager();
	~CSHFamilyManager();


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 공용
	//
	//----------------------------------------------------------------------------------------------------------------
	//							테이블에 패밀리 추가
	CSHFamily*					AddFamilyToTbl(CSHFamily* pcsFamily);
	//							테이블에서 패밀리 삭제
	BOOL						DelFamilyToTbl(DWORD nPlayerID, CSHFamily* pcsFamily);
	BOOL						DelFamilyToTbl(CSHFamily* pcsFamily);
	//							패밀리 설정
	VOID						SetFamily(CSHFamily* pcsFamily);
	//							패밀리 얻기
	CSHFamily*					GetFamily(DWORD nFamilyID);
	CSHFamily*					GetFamily(char* pszName);
	CSHFamily*					GetFamilyByMasterID(DWORD nMasterID);
	
	VOID						Process();

	BOOL						LoadFamilyInfo();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 서버					
	//
	//----------------------------------------------------------------------------------------------------------------
	//	에이전트
	//							클라이언트 요청 분석/처리
	VOID						ASRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength);
	VOID						ASRV_ParseRequestFromServer(DWORD dwConnectionID, char* pMsg, DWORD dwLength);
	//							패밀리 생성
	VOID						ASRV_CreateFamily(USERINFO* pUserInfo, char* szFamilyName, int nCheckState, CSHFamily* pcsFamily = NULL);
	//							패밀리로 초대
	VOID						ASRV_Invite(USERINFO* pUserInfo, DWORD nTargetPlayerID);
	//							패밀리로 초대 결과
	VOID						ASRV_InviteResult(USERINFO* pUserInfo, PACKET_ADD_MEMBER* pstInfo);
	//							호칭 변경
	VOID						ASRV_ChangeNickname(USERINFO* pUserInfo, DWORD nTargetPlayerID, char* pszNickname);
	//							명예 포인트 처리 (몬스터 사냥시 경험치에 의한)
	VOID						ASRV_UpdateHonorPoint(USERINFO* pUserInfo, DWORD nHonorPoint);
	//							맵의 플레이어 정보 업데이트
	VOID						ASRV_UpdateMapPlayerInfo(USERINFO* pUserInfo, DWORD nLevel, char* pszGuildName);
    //							호칭을 맵서버에 보내기
	VOID						ASRV_SendNicknameToMap(USERINFO* pUserInfo, char* pszNickname);
	//							문장 등록
	VOID						ASRV_RegistEmblem(USERINFO* pUserInfo, DWORD nFamilyID, char* pImgData, int nState = 0);
	//	091111 ONS ------------ 문장 삭제
	VOID						ASRV_DeleteEmblem(USERINFO* pUserInfo, DWORD nFamilyID);
	//							클라이언트에 패밀리 정보 보내기
	VOID						ASRV_SendInfoToClient(CSHFamily* pcsFamily, int nType = 0);
	//							다른 에이전트에 멤버 접속 알리기
	VOID						ASRV_SendMemberConToOtherAgent(CSHFamily* pcsFamily, DWORD nMemberID, int nConState, int nType = 0);
	//							탈퇴
	VOID						ASRV_Leave(USERINFO* pUserInfo);
	//							추방
	VOID						ASRV_Expel(USERINFO* pUserInfo, DWORD nMemberID);
	//							해체
	VOID						ASRV_BreakUp(USERINFO* pUserInfo);
	// 091126 ONS 패밀리장 이양 기능 추가
	VOID						ASRV_Transfer(USERINFO* pUserInfo, DWORD nMemberID);
	// 100809 ONS 클래스초기화 처리시 패밀리 정보를 변경처리 추가
	VOID						ASRV_ChangeClassInfo( DWORD CharacterID, DWORD FamilyIdx, DWORD Job, DWORD Grade );

	//							다른 에이전트로부터의 멤버 접속 처리
	VOID						ASRV_MemberConFromOtherAgent(DWORD nFamilyID, DWORD nMemberID, int nConState, int nType = 0);
	//							다른 에이전트로부터의 호칭 변경 처리
	VOID						ASRV_ChangeNicknameFromOtherAgent(DWORD nFamilyID, DWORD nBehavePlayerID, DWORD nTargetPlayerID, char* pszNickname);
	//							다른 에이전트로부터의 맵의 플레이어 정보 업데이트
	VOID						ASRV_UpdateMapPlayerInfoFromOtherAgent(DWORD nFamilyID, DWORD nPlayerID, DWORD nLevel, char* pszGuildName);
	//							다른 에이전트로부터의 명예 포인트 처리
	VOID						ASRV_UpdateHonorPointFromOtherAgent(DWORD nFamilyID, DWORD nHonorPoint, int nType);
	//							다른 에이전트로부터의 패밀리로 초대
	VOID						ASRV_InviteFromOtherAgent(USERINFO* pUserInfo, DWORD nMasterID, char* pszFamilyName);
	//							다른 에이전트로부터의 패밀리로 초대 결과
	VOID						ASRV_InviteResultFromOtherAgent(PACKET_ADD_MEMBER* pstInfo);
	//							다른 에이전트로부터의 패밀리로 초대 결과
	VOID						ASRV_InviteResultFromOtherAgent2(PACKET_FAMILY_TOTAL* pstInfo);
	//							다른 에이전트로부터의 문장 등록
	VOID						ASRV_RegistEmblemFromOtherAgent(DWORD nFamilyID, char* pImgData);
	// 091111 ONS --------------다른 에이전트로부터의 문장 삭제
	VOID						ASRV_DeleteEmblemFromOtherAgent(DWORD nFamilyID);
	//							다른 에이전트로부터의 탈퇴
	VOID						ASRV_LeaveFromOtherAgent(DWORD nFamilyID, DWORD nMemberID);
	//							다른 에이전트로부터의 추방
	VOID						ASRV_ExpelFromOtherAgent(DWORD nFamilyID, DWORD nMemberID);
	//							다른 에이전트로부터의 해체
	VOID						ASRV_BreakUpFromOtherAgent(DWORD nFamilyID); 
	// 091126 ONS 패밀리장 이양 기능 추가 : 다른 에이전트로부터의 패밀리장 이양
	VOID						ASRV_TransferFromOtherAgent(void* pMsg);

	//
	// 090219 LYW --- SHFamilyManager : 패밀리 멤버 접속 수에 따른 기능 추가 처리 (AgentServer)
	//
	//							다른 에이전트로 부터의 패밀리 정보 새로고침.
	VOID						ASRV_RefreshFamilyInfo(void* pMsg) ;
	//							Agent서버와 연결 된 맵 서버에 캐릭터의 패밀리 인덱스를 전송하는 함수. 추가.
	VOID						ASRV_SetFamilyIdxToMapServer( USERINFO* pUserInfo, CSHFamily* pFamily ) ;
	VOID						ASRV_SendFamilyInfoToMapServer(DWORD familyIndex);
	VOID						ASRV_SendFamilyInfoToMapServerReal( CSHFamily* pFamily ) ;

	//----------------------------------------------------------------------------------------------------------------
	//	맵
	//							클라이언트 요청 분석/처리
	VOID						SRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength);
	//							패밀리 생성
	VOID						SRV_CreateFamily(CPlayer* pPlayer, char* szFamilyName);
	//							패밀리로 초대 결과
	VOID						SRV_InviteResult(CPlayer* pPlayer, DWORD nMasterID, DWORD nResult);
	//							명예 포인트 처리 (몬스터 사냥시 경험치에 의한)
	VOID						SRV_ProcessHonorPoint(CPlayer* pPlayer, DWORD nExp);
	//							명예 포인트 처리
	VOID						SRV_ProcessHonorPointAdd(CPlayer* pPlayer, DWORD nExp);
	//							맵의 플레이어 정보 업데이트
	VOID						SRV_UpdateMapPlayerInfo(CPlayer* pPlayer, char* pszGuildName = NULL);
	VOID						SRV_RegistEmblem(CPlayer* pPlayer, DWORD nFamilyID, char* pImgData, DWORD nChangedNum);
	// 091111 ONS ------------- 문장 삭제
	VOID						SRV_DeleteEmblem(CPlayer* pPlayer, DWORD nFamilyID, DWORD nChangedNum);
	//							문장 표시
	VOID						SRV_ShowEmblem(CPlayer* pPlayer, DWORD nFamilyID, char* pImgData, DWORD nChangedNum);
	//							문장 표시2
	VOID						SRV_ShowEmblem2(CPlayer* pPlayer, DWORD nTargetPlayerID);
	//							탈퇴
	VOID						SRV_Leave(CPlayer* pPlayer, DWORD nPlayerID, int nKInd = 0);
	//							추방
	VOID						SRV_Expel(CPlayer* pPlayer, DWORD nMemberID, int nKInd = 0);
	//							해체
	VOID						SRV_BreakUp(CPlayer* pPlayer, DWORD nPlayerID, int nKInd = 0);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 클라이언트
	//
	//----------------------------------------------------------------------------------------------------------------
	//							패밀리 생성 요청
	VOID						CLI_RequestCreateFamily(char* szFamilyName);
	//							패밀리원으로 초대 요청
	VOID						CLI_RequestInvite(CPlayer* pPlayer);
	//							패밀리원 초대에 응답 요청
	VOID						CLI_RequestAnswerToInvite(FAMILY_INVITE_RESULT eResult);
	//							문장 등록 요청
	BOOL						CLI_RequestRegistEmblem(char* pszBmpName);
	//							문장 표시 요청 (그리드안의 다른 플레이어들에게)
	VOID						CLI_RequestShowEmblem(DWORD nPlayerID, DWORD nTargetPlayerID);
	//							문장 표시
	VOID						CLI_ShowEmblem(CPlayer* pPlayer, DWORD nFamilyID, DWORD nChangedFreq);

	// 091111 ONS 문장 삭제 
	VOID						CLI_RequestDeleteEmblem();

	//							호칭변경 요청
	VOID						CLI_RequestChangeNickname(DWORD nTargetPlayerID, char* szNickname);
	//							호칭ON 요청
	VOID						CLI_RequestChangeNicknameON();

	//							탈퇴 요청
	VOID						CLI_RequestLeave();
	//							추방 요청
	VOID						CLI_RequestExpel(int nMemberID);
	//							해체 요청
	VOID						CLI_RequestBreakUp();

	// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 추가.
	VOID						CLI_RequestPayTax( int nKind = 0);

	// 091126 ONS 패밀리장 이양 기능 추가
	VOID						CLI_RequestTransfer( DWORD dwIndex );

	//----------------------------------------------------------------------------------------------------------------
	//							서버로부터의 응답 분석/처리
	VOID						CLI_ParseAnswerFromSrv(void* pMsg);



	//----------------------------------------------------------------------------------------------------------------
	// 090212 LYW --- SHFamilyManager : 패밀리 맴버 접속 수에 따른 추가 기능 처리. 
	// 프로그램상에서 사용할 컨텐츠 명을 MemberRewards로 정의 한다.
	//
	// 맵 서버에서만 필요한 기능을 추가한다.
	// 위에서 보면 스태틱 상수들을 선언하였다. 
	// 규칙을 따르고 싶지만, 이렇게 하면 기획팀에서 수치를 수정하고 싶을 때 다시 빌드를 해야만 한다.
	// 추가 기능에 대한 수치는 리소스 수정에 따라 적용할 수 있도록 처리한다.
	//
#ifdef _MAPSERVER_	
private :
	//
	// 추가 경험치/버프를 적용시킬 최소 멤버 수를 담을 변수. ( 최소 접속 수 이상 되어야 경험치와 버프를 적용한다. )
	BYTE						m_byMinMemberCnt ;
	// 추가 경험치 계산시 사용될 경험치확률 변수. (현재 맥스 1000% 이하 예상:송가람)
	WORD						m_wRewardExprate ;
	//
	// 추가 경험치 계산시 사용될 경험치수치 변수. (현재 맥스 10만 미만 예상 : 송가람 )
	DWORD						m_dwRewardExpValue ;
	//
	// 패밀리원 접속 수에 따른 추가 버프 인덱스들.
	DWORD						m_dwRewardBuffIdxs[ CSHFamily::MAX_MEMBER_NUM ] ;
	
	void SRV_SetFamilyExp(CPlayer&);
	void SRV_ActiveFamilyBuff(CPlayer&);

public:
	BOOL IsFamilyRewardBuff(DWORD dwIndex);

#endif
};

extern CSHFamilyManager g_csFamilyManager;
