/*********************************************************************

	 파일		: SHResidentRegist.h
	 작성자		: hseos
	 작성일		: 2007/06/04

	 파일설명	: 주민등록 클래스의 헤더

 *********************************************************************/

#pragma once

class CPlayer;

#if !defined(_AGENTSERVER)
struct USERINFO;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//
class CSHResidentRegistManager
{
public:
	//----------------------------------------------------------------------------------------------------------------
	static const int				MAX_SERCH_NUM_PER_AGENT;									// 에이전트 하나당 데이트 매칭 검색 최대 개수
	static const int				MAX_SERCH_NUM_CLIENT;										// 데이트 매칭 검색 최대 개수(클라이언트용)
	static const int				DATE_MATCHING_RESERCH_TIME;									// 데이트 매칭 검색 딜레이 시간

	//----------------------------------------------------------------------------------------------------------------
	typedef enum																				// 성향 종류
	{
		PROPENSITY_KIND_HAIR_BLACK = 0,
		PROPENSITY_KIND_HAIR_DYEING,
		PROPENSITY_KIND_CLOTHES_SUIT,
		PROPENSITY_KIND_CLOTHES_CASUAL,
		PROPENSITY_KIND_MAKEUP,
		PROPENSITY_KIND_PERFUME,
		PROPENSITY_KIND_BODY_THIN,
		PROPENSITY_KIND_BODY_FAT,
		PROPENSITY_KIND_BEARD,
		PROPENSITY_KIND_GLASSES,
		PROPENSITY_KIND_HAT,
		PROPENSITY_KIND_ACCESSORY,
		PROPENSITY_KIND_SUMMER,
		PROPENSITY_KIND_WINTER,
		PROPENSITY_KIND_BLOOD_A,
		PROPENSITY_KIND_BLOOD_B,
		PROPENSITY_KIND_BLOOD_O,
		PROPENSITY_KIND_BLOOD_AB,
		PROPENSITY_KIND_BOTTLE_STAR,
		PROPENSITY_KIND_FISH_STAR,
		PROPENSITY_KIND_SHEEP_STAR,
		PROPENSITY_KIND_OX_STAR,
		PROPENSITY_KIND_TWINS_STAR,
		PROPENSITY_KIND_CRAB_STAR,
		PROPENSITY_KIND_LION_STAR,
		PROPENSITY_KIND_VIRGIN_STAR,
		PROPENSITY_KIND_WEIGHT_STAR,
		PROPENSITY_KIND_SCORPION_STAR,
		PROPENSITY_KIND_ARCHER_STAR,
		PROPENSITY_KIND_GOAT_STAR,
		PROPENSITY_KIND_CLEAN,
		PROPENSITY_KIND_COOKING,
		PROPENSITY_KIND_PARTY,
		PROPENSITY_KIND_LONELY,
		PROPENSITY_KIND_SPORTS,
		PROPENSITY_KIND_MUSIC,
	} PROPENSITY_KIND;

	//----------------------------------------------------------------------------------------------------------------
	typedef enum																				 // 데이트 매칭 검색 나이
	{
		DMSAR_UNDER_10 = 0,
		DMSAR_11_TO_14,
		DMSAR_15_TO_19,
		DMSAR_20_TO_24,
		DMSAR_25_TO_29,
		DMSAR_30_TO_34,
		DMSAR_40_TO_49,
		DMSAR_OVER_50,
		DMSAR_MAX,
	} DATE_MATCHING_SERCH_AGE_RANGE;

	//----------------------------------------------------------------------------------------------------------------
	typedef enum																				// 채팅 결과
	{
		DATE_MATCHING_CHAT_RESULT_REQUEST_AND_WAIT = 0,											// ..신청자에게 신청처리와 대기를 알림
		DATE_MATCHING_CHAT_RESULT_REQUESTED,													// ..대상자에게 누군가의 채팅 신청을 알림
		DATE_MATCHING_CHAT_RESULT_CANCEL,														// ..신청자기 신청 취소
		DATE_MATCHING_CHAT_RESULT_BUSY,															// ..대상자가 이미 채팅 중 or 채팅 신청중 or 채팅 신청받는 중 
		DATE_MATCHING_CHAT_RESULT_LOGOFF,														// ..대상자가 현재 접속 중이지 않음
		DATE_MATCHING_CHAT_RESULT_TIMEOUT,														// ..시간 초과
		DATE_MATCHING_CHAT_RESULT_REFUSE,														// ..거절함
		DATE_MATCHING_CHAT_RESULT_START_CHAT,													// ..승락하여 채팅 시작
		DATE_MATCHING_CHAT_RESULT_END_CHAT,														// ..채팅 끝
		DATE_MATCHING_CHAT_RESULT_REJECTION,													// ..데이트매칭거부
	} DATE_MATCHING_CHAT_RESULT;

	//----------------------------------------------------------------------------------------------------------------
	typedef struct																				// 주민등록 구조
	{
		char	szNickName[MAX_NAME_LENGTH+1];
		WORD	nRace;
		WORD	nSex;
		WORD	nAge;
		WORD	nLocation;
		WORD	nFavor;
		WORD	nClass;																			// 100113 ONS 직업정보 추가
		WORD	nPropensityLike01;
		WORD	nPropensityLike02;
		WORD	nPropensityLike03;
		WORD	nPropensityDisLike01;
		WORD	nPropensityDisLike02;
	} stRegistInfo;

	//----------------------------------------------------------------------------------------------------------------
	typedef struct																				// 접속지역 구조
	{
		WORD	nID;
		char	szName[MAX_PATH];
	} stLocation;

	//----------------------------------------------------------------------------------------------------------------
	typedef struct																				// 데이트 매칭 검색 나이 범위
	{
		WORD	nMin;
		WORD	nMax;
	} stSerchAgeRange;

	//----------------------------------------------------------------------------------------------------------------
	typedef struct																				// 데이트 매칭 검색결과 구조
	{
		stRegistInfo	stRegistInfo;															// ..등록 정보
		char			szIntroduction[MAX_INTRODUCE_LEN+1];									// ..소갯말
		WORD			nMatchingPoint;															// ..매칭점수
		DWORD			nID;																	// ..ID
		DWORD			nAgentID;																// ..에이전트ID
		DWORD			nClass;																	// 100113 ONS 직업정보 추가
	} stSerchResult;

	//----------------------------------------------------------------------------------------------------------------
	typedef struct																				// 부가정보 구조
	{
		WORD					m_nMaxPropensity;												// ..성향의 최대값
		WORD					m_nMaxLocation;													// ..접속 지역의 최대값
		WORD					m_nMaxSerchAgeRange;											// ..데이트 매칭 검색시 나이범위

		WORD*					m_pTblPropensity;												// ..성향 테이블
		stLocation*				m_pTblLocation;													// ..지역 테이블
		stSerchAgeRange*		m_pTblSerchAgeRange;											// ..검색 나이범위 테이블
	} stADDInfo;

	//----------------------------------------------------------------------------------------------------------------
	typedef struct : public MSGBASE																// 주민등록 패킷 구조
	{
		stRegistInfo		stInfo;																// ..등록 정보
	} PACKET_RESIDENTREGIST_REGIST;

	//----------------------------------------------------------------------------------------------------------------
	typedef struct : public MSGBASE																// 자기소개 패킷 구조
	{
		char		szIntroduction[MAX_INTRODUCE_LEN+1];										// ..자기소갯말
	} PACKET_RESIDENTREGIST_UPDATE_INTRODUCTION;

	//----------------------------------------------------------------------------------------------------------------
	typedef struct : public MSGBASE																// 데이트 매칭 검색 패킷 구조
	{
		stSerchResult	stSerchResult;															// ..검색결과
		int				nGroupIndex;															// ..그룹 인덱스 (클라이언트에서 한 화면에 보여지는 결과물을 그룹으로 칭함)
		int				nGroupMax;																// ..총 그룹 개수
		DWORD			nRequestPlayerID;														// ..검색 요청 플레이어ID(다른 에이전트에 검색 요청 시 결과를 받을 때 필요)
		BOOL			bMaster;
	} PACKET_SERCH_DATE_MATCHING;

	//----------------------------------------------------------------------------------------------------------------
	struct PACKET_SERCH_DATE_MATCHING_TO_OTHER_AGENT : public MSGBASE							// 다른 에이전트에 검색 요청
	{
		DWORD 			nAgeMin;
		DWORD 			nAgeMax;
		DWORD 			nLocation;
		DWORD 			nSex;
		DWORD 			nGroupIndex;
		DWORD			nRequestPlayerID;

		stRegistInfo	stRegistInfo;
	};

private:
	stADDInfo						m_stAddInfo;
	stSerchResult*					m_pstSerchResult;											// 검색결과
	int								m_nSerchResultNum;											// 검색결과 개수
	stSerchResult					m_stChatPartner;											// 채팅 대상 정보

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHResidentRegistManager();
	~CSHResidentRegistManager();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 공용
	//----------------------------------------------------------------------------------------------------------------
	//						매칭 포인트 계산
	UINT					CalcMatchingPoint(DATE_MATCHING_INFO* pstInfo, stRegistInfo* pstTargetInfo);
	UINT					CalcMatchingPoint(DATE_MATCHING_INFO* pstInfo, DATE_MATCHING_INFO* pstTargetInfo);

	//----------------------------------------------------------------------------------------------------------------
	//						스크립트 파일 로드
	BOOL					LoadScriptFileData();														
	//----------------------------------------------------------------------------------------------------------------
	//						부가정보 얻기
	stADDInfo*				GetADDInfo()				{ return &m_stAddInfo; }

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 서버					
	//
	//----------------------------------------------------------------------------------------------------------------
	//	에이전트
	//						클라이언트 요청 분석/처리
	VOID					ASRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength);
	VOID					ASRV_ParseRequestFromServer(DWORD dwConnectionID, char* pMsg, DWORD dwLength);
	//						로그아웃 등등의 상황에 따른 채팅 종료
	VOID					ASRV_EndDateMatchingChat(USERINFO* pUserInfo);
	//						데이트 매칭 채팅 상태 초기화
	VOID					ASRV_InitDateMatchingChatState(USERINFO* pUserInfo);
	//						매칭 포인트 계산
//	UINT					ASRV_CalcMatchingPoint(USERINFO* pUserInfo, USERINFO* pSerchInfo);

	//----------------------------------------------------------------------------------------------------------------
	//	맵
	//						클라이언트 요청 분석/처리
	VOID					SRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength);
	//----------------------------------------------------------------------------------------------------------------
	//						주민등록 처리
	VOID					SRV_RegistResident(CPlayer* pPlayer, stRegistInfo* pInfo);
	//----------------------------------------------------------------------------------------------------------------
	//						자기소개 업데이트 처리
	VOID					SRV_UpdateIntroduction(CPlayer* pPlayer, char* szIntroduction);
	//----------------------------------------------------------------------------------------------------------------
	//						데이트 매칭 검색
	VOID					SRV_SerchDateMatching(CPlayer* pPlayer, WORD nAgeRange, WORD nLocation, WORD nSex, WORD nGroupIndex);
	//----------------------------------------------------------------------------------------------------------------
	//						데이트 매칭 채팅 신청 처리
	VOID					SRV_RequestDateMatchingChat(CPlayer* pPlayer, MSG_DWORD2* pPacket);
	//----------------------------------------------------------------------------------------------------------------
	//						데이트 매칭 채팅 결과 처리
	VOID					SRV_ResultRequestDateMatchingChat(CPlayer* pPlayer, MSG_DWORD2* pPacket);
	//----------------------------------------------------------------------------------------------------------------
	//						데이트 매칭 채팅 시간 초과 처리
	VOID					SRV_ProcDateMatchingChatTimeOut(CPlayer* pPlayer);
	
	// 클라이언트
	//
	//----------------------------------------------------------------------------------------------------------------
	//						서버에 주민등록 요청
	VOID					CLI_RequestRegist(stRegistInfo* pInfo);
	//						서버에 자기소개 업데이트 요청
	VOID					CLI_RequestUpdateIntroduction(char* szIntroduction);
	//						서버에 데이트 매칭 검색 요청
	VOID					CLI_RequestSerchDateMatching(WORD nAgeRange, WORD nLocation, WORD nSex, int nGroupIndexDelta = 0);
	//						서버에 다른 캐릭터 추천하기 요청
	VOID					CLI_RequestRecommendOtherChar(DWORD nTargetPlayerID);
	//						서버에 데이트 매칭 채팅 신청
	VOID					CLI_RequestDateMatchingChat(stSerchResult* pSerchTarget);
	//						서버에 데이트 매칭 채팅 신청 취소
	VOID					CLI_CancelDateMatchingChat();
	//						서버에 데이트 매칭 채팅 신청 거절
	VOID					CLI_RefuseDateMatchingChat();
	//						서버에 데이트 매칭 채팅 신청 승락
	VOID					CLI_StartDateMatchingChat();
	//						서버에 데이트 매칭 채팅 끝 요청
	VOID					CLI_EndDateMatchingChat();

	//						서버에 데이트 매칭 채팅 내용 보내기
	VOID					CLI_SendDateMatchingChatText(char* szText);

	//						데이트 매칭 검색결과 최대값 얻기
	int						CLI_GetSerchDateMatchingMax()				{ return m_nSerchResultNum; }
	//						데이트 매칭 검색결과 얻기
	stSerchResult*			CLI_GetSerchDateMatchingResult(int nIndex)	{ return &m_pstSerchResult[nIndex]; }
	//						채팅 대상 정보 
	stSerchResult			CLI_GetChatPartner()						{ return m_stChatPartner; }

	//----------------------------------------------------------------------------------------------------------------
	//						서버로부터의 응답 분석/처리
	VOID					CLI_ParseAnswerFromSrv(void* pMsg);
};

extern CSHResidentRegistManager g_csResidentRegistManager;
