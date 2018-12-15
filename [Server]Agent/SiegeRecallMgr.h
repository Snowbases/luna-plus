#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeRecallMgr.h
//	DESC		: 공성전을 위한 소환물을 관리하는 클래스.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include <map>

#include "MHFile.h"

class CSiegeTheme ;
class CSiege_AddObj ;
class CSiegeStep ;





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define SIEGERECALLMGR	CSiegeRecallMgr::GetInstance()

//#define _USE_ERRBOX_							// 이 매크로가 활성화 되어 있으면, 에러 발생시 메시지 박스를 띄우고,
												// 비활성화 시키면, Sworking 폴더 내, 에러 로그를 남긴다.

#define MAX_THEME_COUNT					20		// 최대 테마 카운트.
#define MAX_SIEGEMAP_COUNT_PER_THEME	10		// 한 테마 내, 최대 맵 카운트.
#define MAX_STEP_COUNT_PER_MAP			30		// 한 맵 당, 최대 스텝 카운트.

#define ERR_INVALID_COMMAND				255		// 잘못 된 스크립트 명령어 에러 정의.

// DB 프로시저 명 정의( 테스트 모드일 경우와 진짜 모드 구분 )
#ifdef _TEST_DB_
// 081103 LYW --- SiegeRecallMgr : 공성 관련 프로시저명 소문자 -> 대문자 수정 및 사용하지 않는 프로시저명은 주석처리한다.
//#define MP_SIEGERECALL_INSERT			"dbo.MP_TEST_SIEGERECALL_INSERT"
//#define MP_SIEGERECALL_REMOVE			"dbo.MP_TEST_SIEGERECALL_REMOVE"
#define MP_SIEGERECALL_LOAD				"dbo.MP_TEST_SIEGERECALL_LOAD"
//#define MP_SIEGERECALL_UPDATE			"dbo.MP_TEST_SIEGERECALL_UPDATE"

#else

//#define MP_SIEGERECALL_INSERT			"dbo.MP_SIEGERECALL_INSERT"
//#define MP_SIEGERECALL_REMOVE			"dbo.MP_SIEGERECALL_REMOVE"
#define MP_SIEGERECALL_LOAD				"dbo.MP_SIEGERECALL_LOAD"
//#define MP_SIEGERECALL_UPDATE			"dbo.MP_SIEGERECALL_UPDATE"

#endif // _TEST_DB_


// 081011 LYW --- Protocol : 공성 소환물의 소환 상태를 다음 단계로 넘기는 치트 관련 추가.
enum CHEAT_ERRORTYPE
{
	eCE_InvalidThemeIdx = 0,
	eCE_InvalidMapIdx,
	eCE_InvalidStepIdx,
	eCE_InvalidObjIdx,
	eCE_InvalidObjInfo,
} ;





//-------------------------------------------------------------------------------------------------
//		The class CSiegeRecallMgr.
//-------------------------------------------------------------------------------------------------
class CSiegeRecallMgr
{
	typedef std::map< BYTE, CSiegeTheme >	M_THEME ;							// 테마 정보를 담을 Map 컨테이너.
	M_THEME	m_Theme ;

public:
	GETINSTANCE(CSiegeRecallMgr) ;

	CSiegeRecallMgr(void) ;														// 생성자 함수.
	~CSiegeRecallMgr(void) ;													// 소멸자 함수.

	void Initialize() ;															// 초기화 함수.


	/////////////////////////////
	// [ 스크립트 로딩 파트. ] //
	/////////////////////////////
	void Load_RecallInfo() ;													// 소환 정보를 로딩하는 함수.

	void Command_Process(BYTE byCommandKind, CMHFile* pFile) ;					// 스크립트 명령을 실행하는 함수.
	BYTE Get_CommandKind(char* pString) ;										// 스크립트 명령어 타입을 반환하는 함수.

	void Add_Theme(CMHFile* pFile) ;											// 테마 정보를 추가하는 함수.
	CSiegeTheme* Get_ThemeInfo(BYTE byIdx) ;									// Map 컨테이너에 담기는 인덱스로, 테마 정보를 반환하는 함수.
	CSiegeTheme* Get_ThemeInfoByThemeIdx(WORD wIdx) ;							// 테마 인덱스로, 테마 정보를 반환하는 함수.

	void Add_Theme_Idx(CMHFile* pFile) ;										// 테마 인덱스를 추가하는 함수.

	void Add_Map(CMHFile* pFile) ;												// 맵 정보를 추가하는 함수.
	void Add_Step(CMHFile* pFile) ;												// 스텝 정보를 추가하는 함수.
	void Add_Obj(CMHFile* pFile) ;												// 오브젝트 정보를 추가하는 함수.

	void Add_StartTerm(CMHFile* pFile) ;										// 시작 조건을 추가하는 함수.
	void Add_EndTerm(CMHFile* pFile) ;											// 종료 조건을 추가하는 함수.
	void Add_Command(CMHFile* pFile) ;											// 종료 시 실행할 명령을 추가하는 함수.

	void Die_RecallObj(CMHFile* pFile) ;										// 오브젝트가 죽을경우, 다른 소환물을 소환하는 정보를 추가하는 함수.

	void Check_Die_Obj(CMHFile* pFile) ;										// 오브젝트가 소멸되었는지 여부를 체크하는, 조건을 추가하는 함수.
	void Check_Use_Item(CMHFile* pFile) ;										// 아이템을 사용해야 하는 조건을 추가하는 함수.


	//////////////////////////////////////////
	// [ 네트워크 메시지를 파싱하는 파트. ] //
	//////////////////////////////////////////
	void UserMsgParser(DWORD dwIndex, char* pMsg, DWORD dwLength) ;				// 클라이언트로 부터의 메시지를 파싱하는 함수.
	void ServerMsgPaser(DWORD dwIndex, char* pMsg, DWORD dwLength) ;			// 서버로 부터의 메시지를 파싱하는 함수.

	void Execute_Step(DWORD dwIndex, char* pMsg, DWORD dwLength) ;				// 지정 된 스텝을 실행하는 함수.
	void Execute_AddObj(DWORD dwIndex, CSiegeStep* pStep, BYTE byChildIdx) ;	// 오브젝트 추가를 실행하는 함수.
	void Execute_TermsAndCommand(CSiegeStep* pStep, CSiege_AddObj* pAddObj) ;	// 시작조건 / 종료 조건 / 종료 명령어를 실행하는 함수.

	void Remove_Object(char* pMsg) ;											// 모든 맵 서버의 오브젝트들을, 모두/오브젝트 타입에 따라 삭제하는 함수.
	void Remove_Object_SpecifyMap(char* pMsg) ;									// 지정 된 맵 서버의 오브젝트들을, 모두/오브젝트 타입에 따라 삭제하는 함수.

	void Add_Obj_Ack(DWORD dwIndex, char* pMsg) ;								// 오브젝트 추가 성공 메시지 처리를 하는 함수.
	void Add_Obj_Nack(DWORD dwIndex, char* pMsg) ;								// 오브젝트 추가 실패 메시지 처리를 하는 함수.

	void Ready_Execute_Command(DWORD dwIndex, char* pMsg) ;						// The function to execute command

	void Request_ObjInfo_Syn(DWORD dwIndex, char* pMsg) ;						// 맵 서버로 부터, 오브젝트 로딩 정보가 오면 처리하는 함수.
	void Result_ObjInfo_Syn( LPQUERY pData, LPDBMESSAGE pMessage ) ;			// DB에 기록 된 오브젝트 정보를 참조하여, 오브젝트 리로딩 처리를 한다.

	void RSiegeRecallUpdate( LPQUERY pData, LPDBMESSAGE pMessage ) ;			// DB에 기록 된 오브젝트 정보를 참조하여, 오브젝트 업데이트 결과 처리를 한다.
	void RSiegeRecallInsert( LPQUERY pData, LPDBMESSAGE pMessage ) ;			// DB에 소환 오브젝트 정보 추가 후, 맵 서버에 소환 명령어를 보내는 처리를 한다.
	//void RSiegeRecallRemove( LPQUERY pData, LPDBMESSAGE pMessage ) ;			// DB에 소환 오브젝트 정보 삭제 후, 맵 서버에 소환 정보 삭제 명령을 보내는 처리를 한다.

	// 081007 LYW --- SiegeRecallMgr : 맵 서버에서, 해당맵의 소환물이 몇개인지 요청하는 작업 추가.
	void RequestRecallCount_Syn( DWORD dwIndex, void* pMsg ) ;

	// 081011 LYW --- Protocol : 공성 소환물의 소환 상태를 다음 단계로 넘기는 치트 관련 추가.
	void Cheat_Execute_NextStep( DWORD dwIndex, char* pMsg ) ;
	void Cheat_Execute_EndStep( DWORD dwIndex, char* pMsg ) ;


	////////////////////////////////
	// [ 에러 메시지 처리 파트. ] //
	////////////////////////////////
	void Throw_Error(char* szErr, char* szCaption) ;							// 에러 메시지 처리를 하는 함수.
	void WriteLog(char* pMsg) ;													// 로그를 남기는 처리를 하는 함수.


	/////////////////////////
	// [ 치트 처리 파트. ] //
	/////////////////////////
	void Cheat_Execute_Step(DWORD dwIndex, char* pMsg) ;						// The function to execute step by cheat code.
	//void Cheat_AddObj(DWORD dwIndex, void* pMsg) ;						// The function to add object by cheat code.
};
