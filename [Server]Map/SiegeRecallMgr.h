#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeRecallMgr.h
//	DESC		: The class to manage recall part for siege war.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include <map>

class CSiege_AddObj ;
class CSiegeCommand ;
class CSiegeTerm ;





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define SIEGERECALLMGR	CSiegeRecallMgr::GetInstance()

//#define _USE_ERRBOX_							// If use this macro, you can see the error messagebox.
												// else print error message to console dialog.

#define _ACTIVE_CHEAT_							// If use this macro, you can see notice about exe cheat.

// DB 프로시저 명 정의( 테스트 모드일 경우와 진짜 모드 구분 )
#ifdef _TEST_DB_
// 081103 LYW --- SiegeRecallMgr : 공성 관련 프로시저명 소문자 -> 대문자 수정 및 사용하지 않는 프로시저명은 주석처리한다.
#define MP_SIEGERECALL_INSERT			"dbo.MP_TEST_SIEGERECALL_INSERT"
#define MP_SIEGERECALL_REMOVE			"dbo.MP_TEST_SIEGERECALL_REMOVE"
//#define MP_SIEGERECALL_LOAD				"dbo.MP_TEST_SIEGERECALL_LOAD"
//#define MP_SIEGERECALL_UPDATE			"dbo.MP_TEST_SIEGERECALL_UPDATE"
#define MP_SIEGERECALL_RESET			"dbo.MP_TEST_SIEGERECALL_RESET"

// 081222 LYW --- SiegeRecallMgr : 공성 관련 프로시저명 추가.
#define MP_SIEGERECALL_KILLCOUNT		"dbo.MP_TEST_SIEGERECALL_KILLCOUNT"

#else

#define MP_SIEGERECALL_INSERT			"dbo.MP_SIEGERECALL_INSERT"
#define MP_SIEGERECALL_REMOVE			"dbo.MP_SIEGERECALL_REMOVE"
//#define MP_SIEGERECALL_LOAD				"dbo.MP_SIEGERECALL_LOAD"
//#define MP_SIEGERECALL_UPDATE			"dbo.MP_SIEGERECALL_UPDATE"
#define MP_SIEGERECALL_RESET			"dbo.MP_SIEGERECALL_RESET"
// 081222 LYW --- SiegeRecallMgr : 공성 관련 프로시저명 추가.
#define MP_SIEGERECALL_KILLCOUNT		"dbo.MP_SIEGERECALL_KILLCOUNT"

#endif // _TEST_DB_







//-------------------------------------------------------------------------------------------------
//		The class CSiegeRecallMgr.
//-------------------------------------------------------------------------------------------------
class CSiegeRecallMgr
{
	//typedef std::map< BYTE ,CSiegeTheme >	M_THEME ;
	//M_THEME	m_Theme ;

	// 
	typedef std::map< BYTE, CSiege_AddObj >		M_MADDOBJ ;
	M_MADDOBJ	m_mAddObj ;

	// 081006 LYW --- SiegeRecallMgr : 공성 후 소환물이 소환되었는지 여부를 담을 플래그 추가.
	WORD		m_wRecallCount ;
	BYTE		m_byReadyToUseWaterSeed ;

	// 081226 LYW --- SiegeRecallMgr : 유니크 인덱스 변수 추가.
	// 캐터펄트를 추가할 때, 캐터펄트의 고유 아이디로 추가 및 삭제 처리를 하려 하였으나, 
	// 캐터펄트의 추가 및 조건 명령이 Agent로 부터 넘어오기 때문에, Agent에서 현재 맵서버에서
	// 추가되는 캐터펄트의 고유 아이디를 알지 못하므로, 이는 안된다.
	// 스크립트에서 지정한 고유한 순서대로의 아이디를 사용하면 이를 해결할 수 있다.
	// 애초에 Agent에서만 스크립트 정보를 가지고 있도록 설계한 것이 문제.
	BYTE		m_byUniqueIdx ;

public:
	GETINSTANCE(CSiegeRecallMgr) ;

	CSiegeRecallMgr(void) ;												// Constructor.
	~CSiegeRecallMgr(void) ;											// Destructor.

	void Initialieze() ;												// The function to initialize recall manager.

	void Reset_SiegeObjInfo() ;											// The function to resetting.

	//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정
	CSiege_AddObj* Get_AddObjIdx(DWORD dwObjIdx) ;						// The function to return add object info.


	// Network messaeg part.
	void NetworkMsgParser(void* pMsg ) ;								// The functions to parsing network message.

	void Add_Object(void* pMsg) ;										// The function to add object.
	void Add_CheckObjDie(void* pMsg) ;									// The function to add check routine for object die.
	void Add_CheckUseItem(void* pMsg) ;									// The function to add check routine for use item.
	void Add_Command(void* pMsg) ;										// The function to add command.


	// The function to add object info to database.
	//void Add_ObjToDB(WORD wTheme, WORD wMapNum, BYTE byStepIdx, WORD wAddObjIdx, BYTE IsParent, BYTE byChildIdx) ;
	void Add_ObjToDB(WORD wTheme, WORD wMapNum, WORD wRecallMap, BYTE byStepIdx, BYTE byAddObjIdx, BYTE byComKind, BYTE byComIdx, BYTE byRecallStep ) ;
	// The function to remove object info from database.
	//void Remove_ObjFromDB(WORD wTheme, WORD wMapNum, BYTE byStepIdx, WORD wAddObjIdx, BYTE IsParent, BYTE byChildIdx) ;
	void Remove_ObjFromDB(WORD wTheme, WORD wMapNum, WORD wRecallMap, BYTE byStepIdx,  BYTE byAddObjIdx, BYTE byComKind, BYTE byComIndex, BYTE byRecallStep ) ;
	// The function to load object info from database.
	void Request_ObjFromDB() ;


	// The function to check terms.
	//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
	BYTE Check_MyAddObjInfo(BYTE byStepNum, DWORD dwObjIdx)	;				// The function to check addObjInfo.
	//void Check_ObjectDie(DWORD dwObjectID, BYTE IsCheatMode) ;			// The function to check object die.
	// 081028 LYW --- SiegeRecall : 킬러 아이디를 남기기 위해 함수를 수정함.
	void Check_ObjectDie(CObject* pKiller, DWORD dwObjectID, BYTE IsCheatMode) ;			// The function to check object die.


	// The function to send start term info.
	void Send_StartTermInfo(BYTE byParentStepIdx, CSiegeTerm* pTerm, BYTE byIdx) ;
	// The function to send end term info.
	void Send_EndTermInfo(BYTE byParentStepIdx, CSiegeTerm* pTerm, BYTE byIdx) ;
	// The function to send command info.
	void Send_CommandInfo(BYTE byParentStepIdx, CSiegeCommand* pCommand, BYTE byIdx) ;


	// The function to remove object info from database.
	void Remove_AllMapAllObj( void* pMsg ) ;							// 모든 맵 / 모든 오브젝트를 삭제해야 하는 함수.
	void Remove_AllMapSpecifyObj( void* pMsg ) ;						// 모든 맵 / 지정 된 오브젝트를 삭제해야 하는 함수.
	void Remove_SpecifyMapAllObj( void* pMsg ) ;						// 지정 된 맵 / 모든 오브젝트를 삭제해야 하는 함수.
	void Remove_SpecifyMapSpecifyObj( void* pMsg ) ;					// 지정 된 맵 / 지정 된 오브젝트를 삭제해야 하는 함수.
	void Set_LoadSummonFlag( void* pMsg ) ;								// 몬스터 로딩 여부를 설정하는 함수.

	BYTE Get_RecalledObjectCount() { return BYTE( m_mAddObj.size() ) ; }

	void EndSiegeWar_ClearRecallInfo() ;

	BYTE IsRecallMonster(DWORD dwID) ;									// 인자로 넘어온 아이디의 몬스터가 소환 된 몬스터인지 반환하는 함수.

	WORD GetRecallCount()	{ return m_wRecallCount ; }					// 소환 할 몬스터 수를 반환하는 함수.

	// 081007 LYW --- 소환해야 할 소환 카운트를 받는다.
	void Set_RecallCount( void* pMsg ) ;

	// 081015 LYW --- SiegeRecallMgr : 공성 소환물의 완전한 킬 상태를 로딩하여 정보가 넘어왔을 때 처리하는 작업 추가.
	void Load_RecallCount( void* pMsg ) ;

	// 081008 LYW --- SiegeRecallMgr : 워터시드 사용울 위한 준비가 되었는지 여부를 반환하는 함수.
	BYTE IsReadyToUseWaterSeed() ;
	void SetReadyToUseWaterSeed(BYTE byValue) { m_byReadyToUseWaterSeed = byValue ; }


	// The functions to print error message.
	void Throw_Error(char* szErr, char* szCaption) ;					// The function to process error message.
	void WriteLog(char* pMsg) ;											// The function to write error log.
	// 081011 LYW --- Protocol : 공성 소환물의 소환 상태를 다음 단계로 넘기는 치트 관련 추가.
	void Cheat_ExcuteNextStep( void* pMsg ) ;

	// 081012 LYW --- SiegeRecallMgr : 소환물 초기화 함수 추가.
	void Clear_RecallInfo() ;

	// 081028 LYW --- SiegeRecall : 캐터펄트 로그를 남기는 함수 추가.
	void WriteCatapultLog( char* pMsg ) ;

	// 081222 LYW --- SiegeRecall : 캐터펄트를 모두 잡았는지 여부를 확인하는 함수 추가.
	void RequestKillCountToDB() ;

	// 081222 LYW --- SiegeRecall : 캐터펄트를 모두 잡았는지 결과 값을 받아서 설정하는 함수 추가.
	void CheckKillCount(int nKillCount) ;

private :
	// 081226 LYW --- SiegeRecallMgr : 유니크 인덱스를 설정 및 반환하는 함수 추가.
	void SetUniqueIdx( BYTE byIdx )	{ m_byUniqueIdx = byIdx ; } ;
	BYTE GetUniqueIdx()				{ return m_byUniqueIdx ; }
};
