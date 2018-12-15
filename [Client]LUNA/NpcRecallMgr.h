#pragma once

#include "../[CC]RecallInfo/NpcRecall/NpcRecallSelf.h"
#include "../[CC]RecallInfo/NpcRecall/NpcRecallRemainTime.h"
#include "../[CC]RecallInfo/NpcRecall/NpcRecallMoveable.h"

class CNpc;
class CMHFile;

#define NPCRECALLMGR	CNpcRecallMgr::GetInstance()

#ifdef _GMTOOL_
//#define _USE_NPCRECALL_ERRBOX_					// 이 매크로가 활성화 되어 있으면, 에러 발생시 메시지 박스를 띄우고,
//												// 비활성화 시키면, Sworking 폴더 내, 에러 로그를 남긴다.
#endif //_GMTOOL_

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
	e_RNC_RECALL_FAILED_MSG,					// 이미 소환된 npc를 소환하려는 경우, 실패 메시지를 담을 명령어.
	e_RNC_PARENT_NPCID,							// 현재 로딩중인 NPC의 부모 NPCID를 설정하는 명령어. 
	e_RNC_PARENT_TYPE,							// 현재 로딩중인 NPC의 부모 NPC의 타입을 설정하는 명령어. ( 1 : STATIC NPC , 2: RECALL NPC ) 

	e_RNC_MAX,									// 명령어 제한 체크용.
} ;

// 080828 LYW --- GameResourceManager : npc 버프 정보를 담을 구조체를 정의한다.
struct stNpcBuffInfo
{
	DWORD dwBuffIdx ;
	DWORD dwMoney ;
};

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
	///////////////////////////////////////
	// [ 스크립트 정보를 담을 컨테이너 ] //
	///////////////////////////////////////

	typedef std::map< WORD, CNpcRecallSelf >			MAP_RSELF_NPC ;			// 스스로 소환/소멸되는 NPC를 담을 맵 컨테이너 정의.
	MAP_RSELF_NPC										m_mRSelf_Npc ;

	typedef std::map< WORD, CNpcRecallRemainTime >		MAP_RTIME_NPC ;			// 일정 시간동안 소환/소멸되는 NPC를 담을 맵 컨테이너 정의.
	MAP_RTIME_NPC										m_mRTime_Npc ;

	typedef std::map< WORD, CNpcRecallMoveable >		MAP_RMOVE_NPC ;			// 특정 포인트로 워프 가능한 NPC를 담을 맵 컨테이너 정의.
	MAP_RMOVE_NPC										m_mRMove_Npc ;

	DWORD		m_CurSelectedNpcID ;											// 현재 선택 된 npc id를 담을 변수.


	////////////////////////////////////////////////////////////////////////////////////
	// 081023 LYW --- NpcRecallMgr : 메모리 문제로 인해, new delete를 따로 안하는 구조로 수정한다.
	//// 080826 KTH -- NPC Buff List
	//// 지금은 서버에서만 사용될 확률이 높지만, 나중을 위해 공유 리소스에 배치하였음.
	//CYHHashTable<stNPCBuffData> m_NPCBuff;
	typedef std::map< DWORD, stNPCBuffData>		M_NPCBUFFMAP ;
	M_NPCBUFFMAP								m_MNpcBuffMap ;
	////////////////////////////////////////////////////////////////////////////////////

public :
	GETINSTANCE(CNpcRecallMgr) ;

	CNpcRecallMgr(void) ;														// 생성자 함수.
	~CNpcRecallMgr(void) ;														// 소멸자 함수.


	//////////////////////
	//  [초기화 파트.]  //
	//////////////////////

	void Initialize() ;															// 초기화 함수.

	void Load_RecallNpc() ;														// 소환용 npc 정보를 로드하는 함수.

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

	void Load_SetRecallFailedMsg(CMHFile* pFile) ;								// 이미 소환 된, npc 소환 실패 에러 메시지 번호를 로딩하는 함수.

	void Load_SetParentNpcID(CMHFile* pFile) ;									// 부모 NPC의 ID를 셋팅 (서버 인증을 위하여)

	void Load_SetParentNpcType(CMHFile* pFile) ;									// 부모 NPC의 Type을 셋팅 (서버 인증을 위하여)
	

	///////////////////////////
	// [ PROCESS & PARSER. ] //
	///////////////////////////

	void NetworkMsgParser( BYTE Protocol, void* pMsg ) ;						// 서버로 부터의 네트워크 메시지를 파싱하는 함수.

	void RecallNpc_Nack(void* pMsg) ;											// NPC 소환 실패 처리를 하는 함수.

	void AlreadyRecallNpc_Nack(void* pMsg) ;									// 이미 소환 된 npc이기 때문에 소환 실패처리를 하는 함수.

	void ChangeMap_Nack(void* pMsg) ;											// 맵 이동 실패 메시지 처리를 하는 함수 추가.

	
	///////////////////////////////////////////
	// [ 스크립트 정보 추가 & 반환용 ]       //
	///////////////////////////////////////////

	WORD Get_CurTotalNpcCount() ;												// 스크립트 로딩 중, 지금까지 로드 된 총 npc 수를 반환하는 함수.

	CNpcRecallBase* Get_CurNpcBase() ;											// 현재 로드 한, npc의 기본 정보를 반환하는 함수.

	BYTE Get_NpcType(DWORD dwIndex) ;											// npc 인덱스로 npc의 타입을 반환하는 함수.

	CNpcRecallSelf* Get_SelfNpc(DWORD dwIndex) ;								// 스스로 소환/소멸되는 npc 정보를 반환하는 함수.

	CNpcRecallRemainTime* Get_TimeNpc(DWORD dwIndex) ;							// 일정시간 소환/소멸되는 npc 정보를 반환하는 함수.

	CNpcRecallMoveable* Get_MoveNpc(DWORD dwIndex) ;							// 이동가능한 npc 정보를 반환하는 함수.

	CNpcRecallBase* Get_RecallBase(DWORD dwIndex);								// 소환 기본정보를 반환하는 함수.
	CNpcRecallBase* Get_RecallBaseByNpcId(DWORD dwNpcIndex);

	void Set_CurSelectNpcId(DWORD dwNpcid) { m_CurSelectedNpcID = dwNpcid ; }	// 현재 선택 된 npc 아이디를 설정하는 함수.
	DWORD Get_CurSelectNpcId() { return m_CurSelectedNpcID ; }					// 현재 선택 된 npc 아이디를 반환하는 함수.

	BYTE Check_RecallNpc(DWORD dwRecallIdx)  ;									// 소환 된 npc인지 체크하여, 다음 이벤트 처리를 하는 함수.


	void PrintError(char* szErr, char* szCaption) ;								// 에러 메시지를 출력하는 함수.


	// 080826 KTH Load NPC Buff List
	void LoadNPCBuffList();
	stNPCBuffData* GetNPCBuffData(DWORD dwNPCIdx);

	void OnAddRecallNpc(CNpc* pNpc );
	void OnRemoveRecallNpc(CNpc* pNpc );
};
