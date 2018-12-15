#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMgr.h
//	DESC		: NPC를 소환하는 관리를 하는 매니져 클래스.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include <map>

#include "MHFile.h"
#include "../[CC]RecallInfo/NpcRecall/NpcRecallSelf.h" 
#include "../[CC]RecallInfo/NpcRecall/NpcRecallRemainTime.h"
#include "../[CC]RecallInfo/NpcRecall/NpcRecallMoveable.h"
//class CNpcRecallSelf ;
//class CNpcRecallRemainTime ;
//class CNpcRecallMoveable ;





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define NPCRECALLMGR	CNpcRecallMgr::GetInstance()

//#define _USE_NPCRECALL_ERRBOX_					// 이 매크로가 활성화 되어 있으면, 에러 발생시 메시지 박스를 띄우고,
												// 비활성화 시키면, Sworking 폴더 내, 에러 로그를 남긴다.

#define SCHEDULE_CHECK_TIME		60000			// 스케쥴을 체크할 시간 단위 정의.

// DB 프로시저 명 정의( 테스트 모드일 경우와 진짜 모드 구분 )


#define MP_NPCREMAINTIME_INSERT			"dbo.MP_NPCREMAINTIME_INSERT"
#define MP_NPCREMAINTIME_DELETE			"dbo.MP_NPCREMAINTIME_DELETE"
#define MP_NPCREMAINTIME_UPDATE			"dbo.MP_NPCREMAINTIME_UPDATE"
#define MP_NPCREMAINTIME_LOAD			"dbo.MP_NPCREMAINTIME_LOAD"


enum RECALL_NPC_COMMAND							// 스크립트에서 NPC 소환 정보를 로딩하기 위한 명령어 분류 정의.
{
	e_RNC_SELF_RECALL_NPC = 0,					// 스스로 소환/소멸하는 npc 추가 명령어.
	e_RNC_REMAIN_TIME_NPC,						// 일정시간 동안 소환/소멸 되는 npc 추가 명령어.
	e_RNC_MOVEABLE_NPC,							// 이동이 가능한 npc 추가 명령어.
	e_RNC_NPCRECALLID,							// 현재 로딩중인 npc의 소환 ID를 설정하는 명렁어.
	e_RNC_NPCID,								// 현재 로딩중인 npc의 아이디를 설정하는 명렁어.
	e_RNC_NPCKIND,								// 현재 로딩중인 npc의 종류를 설정하는 명령어.
	e_RNC_RECALLMAP,							// 현재 로딩중인 npc의 소환 맵 번호를 설정하는 명령어.
	e_RNC_XPOS,									// 현재 로딩중인 npc의 소환 좌표 x 를 설정하는 명령어.
	e_RNC_ZPOS,									// 현재 로딩중인 npc의 소환 좌표 z 를 설정하는 명령어.
	e_RNC_DIR,									// 현재 로딩중인 npc의 소환 방향을 설정하는 명령어.
	e_RNC_NAME,									// 현재 로딩중인 npc의 이름을 설정하는 명령어.
	e_RNC_TIME_TABLE,							// 현재 로딩중인 npc의 소환/소멸 시간 테이블을 설정하는 명령어.
	e_RNC_REMAIN_TIME,							// 현재 로딩중인 npc의 소환 유효 시간을 설정하는 명령어.
	e_RNC_MOVE_TIME,							// 현재 로딩중인 npc의 이동이 시작되는 시간을 설정하는 명령어.
	e_RNC_MOVE_XPOS,							// 현재 로딩중인 npc의 이동 할 좌표 x 를 설정하는 명령어.
	e_RNC_MOVE_ZPOS,							// 현재 로딩중인 npc의 이동 할 좌표 z 를 설정하는 명령어.
	e_RNC_CHANGE_MAPNUM,						// 현재 로딩중인 NPC를 통해 맵이동을 할 맵 번호를 설정하는 명령어.
	e_RNC_CHANGE_XPOS,							// 현재 로딩중인 NPC를 통해 맵이동을 할 맵 X좌표를 설정하는 명령어.
	e_RNC_CHANGE_ZPOS,							// 현재 로딩중인 NPC를 통해 맵이동을 할 맵 Z좌표를 설정하는 명령어.
	e_RNC_PARENT_NPCID,							// 현재 로딩중인 NPC의 부모 NPCID를 설정하는 명령어. 
	e_RNC_PARENT_TYPE,							// 현재 로딩중인 NPC의 부모 NPC의 타입을 설정하는 명령어. ( 1 : STATIC NPC , 2: RECALL NPC ) 
	e_RNC_MAX,									// 명령어 제한 체크용.
} ;


// 080828 LYW --- GameResourceManager : npc 버프 정보를 담을 구조체를 정의한다.
struct stNpcBuffInfo
{
	DWORD dwBuffIdx ;
	DWORD dwMoney ;
} ;

struct stNPCBuffData
{
	DWORD dwNpcIdx ;

	typedef std::list< stNpcBuffInfo >	L_BUFFLIST ;
	L_BUFFLIST							buffList ;
} ;

// 080828 LYW --- GameResourceManager : npc 버프 스킬 요청 실패 처리 분류 정의.
enum SIEGEWARFARE_REQUEST_BUFFSKILL_ERR
{
	e_REQUEST_BUFFSKILL_SUCCESS = 0,						// 버프 스킬 요청 성공.		
	e_REQUEST_BUFFSKILL_FAILED_RECEIVE_NPCINFO,				// npc 정보 받기 실패.
	e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFDATA,			// 버프 데이터 받기 실패.
	e_REQUEST_BUFFSKILL_FAILED_RECEIVE_PLAYERINFO,			// Player 정보 받기 실패.
	e_REQUEST_BUFFSKILL_TOO_FAR_DISTANCE,					// Player와 Npc의 거리가 너무 멀어서 실패.
	e_REQUEST_BUFFSKILL_FAILED_FIND_BUFF,					// Npc의 버프 찾기 실패.
	e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFSKILLINFO,		// 버프 스킬 정보 받기 실패.
	e_REQUEST_BUFFSKILL_NOTENOUGH_PAY,						// 버프 스킬 시전 지불 금액 부족.
	e_REQUEST_BUFFSKILL_INVALID_PLAYER,						// 유저 정보 받기 실패.
	e_REQUEST_BUFFSKILL_INVALID_GUILD,						// 성을 소유한 길드가 아니라면 실패.
	e_REQUEST_BUFFSKILL_INVALID_SIEGWZONE,					// 공성 지역이 아니면 실패.
	e_REQUEST_BUFFSKILL_INVALID_TIME,						// 공성전이 시작이후라면 실패.

	e_REQUEST_BUFFSKILL_MAX,								// 에러 제한 체크용.
} ;


//-------------------------------------------------------------------------------------------------
//		The class CNpcRecallMgr.
//-------------------------------------------------------------------------------------------------
class CNpcRecallMgr
{
	typedef std::map< DWORD, stNPCBuffData>		M_NPCBUFFMAP ;
	M_NPCBUFFMAP								m_MNpcBuffMap ;
	///////////////////////////////////////
	// [ 스크립트 정보를 담을 컨테이너 ] //
	///////////////////////////////////////

	typedef std::map< WORD, CNpcRecallSelf >			MAP_RSELF_NPC ;			// 스스로 소환/소멸되는 NPC를 담을 맵 컨테이너 정의.
	MAP_RSELF_NPC										m_mRSelf_Npc ;

	typedef std::map< WORD, CNpcRecallRemainTime >		MAP_RTIME_NPC ;			// 일정 시간동안 소환/소멸되는 NPC를 담을 맵 컨테이너 정의.
	MAP_RTIME_NPC										m_mRTime_Npc ;

	typedef std::map< WORD, CNpcRecallMoveable >		MAP_RMOVE_NPC ;			// 특정 포인트로 워프 가능한 NPC를 담을 맵 컨테이너 정의.
	MAP_RMOVE_NPC										m_mRMove_Npc ;


	///////////////////////////////////////
	// [ 활성화 된 npc를 담을 컨테이너 ] //
	///////////////////////////////////////

	typedef std::map< DWORD, CNpcRecallSelf >			MAP_ARSELF_NPC ;
	MAP_ARSELF_NPC										m_mARSelf_Npc ;			// 활성화 된 RS npc를 담을 컨테이너.

	typedef std::map< DWORD, CNpcRecallRemainTime >		MAP_ARTIME_NPC ;
	MAP_ARTIME_NPC										m_mARTime_Npc ;			// 활성화 된 RT npc를 담을 컨테이너.

	typedef std::map< DWORD, CNpcRecallMoveable >		MAP_ARMOVE_NPC ;
	MAP_ARMOVE_NPC										m_mARMove_Npc ;			// 활성화 된 RM npc를 담을 컨테이너.

public :
	GETINSTANCE(CNpcRecallMgr) ;

	CNpcRecallMgr(void) ;														// 생성자 함수.
	~CNpcRecallMgr(void) ;														// 소멸자 함수.


	//////////////////////
	//  [초기화 파트.]  //
	//////////////////////

	void Initialize() ;															// 초기화 함수.

	void Load_RecallNpc() ;														// 소환용 npc 정보를 로드하는 함수.

	void Load_NpcFromDB();

	void OnLoadNpc_FromDB( DWORD dwRecallidx , DWORD dwChenel , DWORD dwRemainTime ) ;

	BYTE Get_CommandKind(char* pString) ;										// 스크립트 명령어 타입을 반환하는 함수.

	void Command_Process(BYTE byCommandKind, CMHFile* pFile) ;					// 스크립트 명령어 타입에 따른 처리를 하는 함수.

	void Load_SelfRecallNpc() ;													// 스스로 소환/소멸되는 npc 정보를 로드하는 함수.

	void Load_RemainTimeNpc() ;													// 일정 시간 소환/소멸되는 npc 정보를 로드하는 함수.

	void Load_MoveableNpc() ;													// 이동 가능한 npc 정보를 로드하는 함수.

	void Load_SetNpcRecallId(CMHFile* pFile) ;									// 현재 로딩중인 npc의 소환인덱스를 세팅하는 함수.

	void Load_SetNpcId(CMHFile* pFile) ;										// 현재 로딩중인 npc의 인덱스를 세팅하는 함수.

	void Load_SetNpcKind(CMHFile* pFile) ;										// 현재 로딩중인 npc의 종류를 세팅하는 함수.

	void Load_SetRecallMap(CMHFile* pFile) ;									// 소환 될 맵 번호를 세팅하는 함수.

	void Load_SetXpos(CMHFile* pFile) ;											// 소환 될 X좌표를 세팅하는 함수.

	void Load_SetZpos(CMHFile* pFile) ;											// 소환 될 Z좌표를 세팅하는 함수.

	void Load_SetDir(CMHFile* pFile ) ;											// 소환 될 방향을 세팅하는 함수.

	void Load_SetName(CMHFile* pFile ) ;										// 현재 로딩중인 npc 이름을 설정

	void Load_TimeTable(CMHFile* pFile) ;										// 스스로 소환될 시간 테이블을 로딩하는 함수.

	void Load_SetRemainTime(CMHFile* pFile) ;									// 소환 유지 시간을 로딩하는 함수.

	void Load_SetMoveTime(CMHFile* pFile) ;										// 이동을 시작 할 시간을 로딩하는 함수.

	void Load_SetMoveXpos(CMHFile* pFile) ;										// 이동 할 X좌표를 로딩하는 함수.

	void Load_SetMoveZpos(CMHFile* pFile) ;										// 이동 할 Z좌표를 로딩하는 함수.

	void Load_SetChangeMapNum(CMHFile* pFile) ;									// 포탈을 통해 이동할 맵 번호를 로딩하는 함수.

	void Load_SetChangeMapXPos(CMHFile* pFile) ;								// 포탈을 통해 이동할 맵의 X좌표를 로딩하는 함수.

	void Load_SetChangeMapZPos(CMHFile* pFile) ;								// 포탈을 통해 이동할 맵의 Z좌표를 로딩하는 함수.

	void Load_SetParentNpcID(CMHFile* pFile) ;									// 부모 NPC의 ID를 셋팅 (서버 인증을 위하여)

	void Load_SetParentNpcType(CMHFile* pFile) ;									// 부모 NPC의 Type을 셋팅 (서버 인증을 위하여)

	
	///////////////////////////
	// [ PROCESS & PARSER. ] //
	///////////////////////////

	void Process() ;															// 프로세싱 함수.

	void NetworkMsgParser(DWORD dwIndex, void* pMsg, DWORD dwLength) ;			// 메시지 파서 함수.

	void CheckSchedule_SelfRecallNpc() ;										// 스스로 소환/소멸되는 npc의 스케쥴을 체크한다.

	void CheckSchedule_RemainTimeNpc() ;										// 일정 시간동안 소환/소멸되는 npc의 스케쥴을 체크한다.

	void CheckSchedule_MoveableNpc() ;											// 이동 가능한 npc의 스케쥴을 체크한다.

	void CTOM_Recall_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) ;			// 클라이언트로 부터 npc 소환 요청이 들어왔을 때, 처리하는 함수.

	void CTOAM_ChangeMap_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) ;		// 클라이언트로 부터, 이동 가능한 npc를 통한 맵 이동 요청이 들어왔을 때, 처리하는 함수.

	void Cheat_CTOA_Recall_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) ;		// 테스트용 치트 함수.


	///////////////////////////////////////////
	// [ 스크립트 정보 추가 & 반환용 ]       //
	///////////////////////////////////////////

	WORD Get_CurTotalNpcCount() ;												// 스크립트 로딩 중, 지금까지 로드 된 총 npc 수를 반환하는 함수.

	CNpcRecallBase* Get_CurNpcBase() ;											// 현재 로드 한, npc의 기본 정보를 반환하는 함수.
	BYTE Get_NpcType(DWORD dwIndex) ;											// npc 인덱스로 npc의 타입을 반환하는 함수.

	CNpcRecallSelf* Get_SelfNpc(DWORD dwIndex) ;								// 스스로 소환/소멸되는 npc 정보를 반환하는 함수.

	CNpcRecallRemainTime* Get_TimeNpc(DWORD dwIndex) ;							// 일정시간 소환/소멸되는 npc 정보를 반환하는 함수.

	CNpcRecallMoveable* Get_MoveNpc(DWORD dwIndex) ;							// 이동가능한 npc 정보를 반환하는 함수.

	CNpcRecallBase* Get_RecallBase(DWORD dwIndex) ;

	CNpcRecallSelf* Get_ActiveSelfNpc(DWORD dwIndex , WORD wChenel) ;			// 스스로 소환/소멸되는 npc 정보를 반환하는 함수.

	CNpcRecallRemainTime* Get_ActiveTimeNpc(DWORD dwIndex) ;					// 일정시간 소환/소멸되는 npc 정보를 반환하는 함수.

	CNpcRecallMoveable* Get_ActiveMoveNpc(DWORD dwIndex) ;						// 이동가능한 npc 정보를 반환하는 함수.

	CNpcRecallMoveable* Get_ActiveMoveNpcByCreateIdx(DWORD dwIndex) ;			// 생성 아이디로, 소환중인 이동 npc 정보를 검색하는 함수.

	CNpcRecallBase* Get_ActiveRecallBase(DWORD dwIndex , WORD wchenel );

	BOOL CheckHackRecallNpc( CPlayer* pPlayer , DWORD dwRecallIndex ) ;

	////////////////////////////////
	// [ 에러 메시지 처리 파트. ] //
	////////////////////////////////

	void Throw_Error(char* szErr, char* szCaption) ;							// 에러 메시지 처리를 하는 함수.
	void WriteLog(char* pMsg) ;													// 로그를 남기는 처리를 하는 함수.

	void Recall_Npc( CNpcRecallBase* pNpcRecallBase );
	void Recall_success( CNpcRecallBase* pNpcRecallBase ) ;
	void Recall_Failed( CNpcRecallBase* pNpcRecallBase , BYTE byError ) ;

	void Delete_Npc( CNpcRecallBase* pNpcRecallBase );
	void Delete_Success( CNpcRecallBase* pNpcRecallBase ) ;
	void Delete_Failed( CNpcRecallBase* pNpcRecallBase , BYTE byError ) ;

	void Move_Npc( CNpcRecallBase* pNpcRecallBase );
	void Move_Npc_Success( CNpcRecallBase* pNpcRecallBase ) ;
	void Move_Npc_Failed( CNpcRecallBase* pNpcRecallBase , BYTE byError ) ;

	void ATOM_ChangeMap_Ack(DWORD dwIndex, void* pMsg, DWORD dwLength) ;

	// 080826 KTH Load NPC Buff List
	void LoadNPCBuffList();
	stNPCBuffData* GetNPCBuffData(DWORD dwNPCIdx);

	// 080826 KTH -- NPC Buff Excute Fuction
	BYTE ExcuteNPCBuff(CPlayer* pPlayer, WORD wNPCIndex, DWORD dwSelectBuff = 0);

	// 081028 LYW --- NpcRecallMgr : npc 소환 삭제 정보를 남길 함수 추가.
	void WriteRecallNpcLog( char* pMsg ) ;

};
