#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallBase.h
//	DESC		: NPC를 소환하는 기본 정보를 담고 있는 클래스.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
//#define _USE_NPCRECALL_ERRBOX_					// 이 매크로가 활성화 되어 있으면, 에러 발생시 메시지 박스를 띄우고,
												// 비활성화 시키면, Sworking 폴더 내, 에러 로그를 남긴다.





//-------------------------------------------------------------------------------------------------
//		The class CNpcRecallBase.
//-------------------------------------------------------------------------------------------------
class CNpcRecallBase
{
	BYTE		m_byRecallCodition ;									// 소환 조건.

	DWORD		m_dwNpcRecallIdx ;										// NpcRecall 인덱스 
	DWORD		m_dwNpcIdx ;											// Npc 인덱스. StaticNpcList.bin의 Unique index를 사용한다.
	WORD		m_wNpcKind ;											// NpcList.bin에서 참조할 Kind 인덱스.

	MAPTYPE		m_RecallMapNum ;										// 소환 맵 번호.

	float		m_fXpos ;												// 좌표 X.
	float		m_fZpos ;												// 좌표 Z.
	WORD		m_wDir;													// 방향.

	BYTE		m_byActive ;											// 활성화 여부를 담는 변수.

	DWORD		m_dwCreatedIdx ;										// 맵 서버에서 생성 된 npc 생성 인덱스.

	BYTE		m_byReadyToDelete ;										// NPC를 삭제할 준비가 되었는지 여부를 담을 변수.

	DWORD		m_dwRecallTime ;										// Npc가 소환 된 시간.

	WORD		m_wRecallFailedMsg ;									// 소환 실패 시 출력할 에러 메시지 번호.

	char		m_NpcName[MAX_NPC_NAME_LENGTH+1];						// Npc이름 

	WORD		m_wRecalledChenel;										// 소환된 체널

	DWORD		m_dwParentNpcIdx;										// 부모 NPC이름 

	BYTE		m_byParentType;											// 부모 NPC Type

public:
	enum		eParentNpcType	{	eParentNpcType_UnKnown ,			// 부모 NPC Type 열거형 
									eParentNpcType_Static,				
									eParentNpcType_Recall,	
									eParentNpcType_OnlySelf,
									eParentNpcType_Max		} ;


public:
	CNpcRecallBase();
	virtual ~CNpcRecallBase();
	void	Set_RecallCondition(BYTE byCondition) ;						// 소환 조건을 설정하는 함수.
	BYTE	Get_RecallCondition() { return m_byRecallCodition ; }		// 소환 조건을 반환하는 함수.

	void	Set_NpcRecallIndex(DWORD m_dwNpcRecallIdx) ;						// NpcRecall 인덱스를 설정하는 함수.
	DWORD	Get_NpcRecallIndex() { return m_dwNpcRecallIdx ; }				// NpcRecall 인덱스를 반환하는 함수.

	void	Set_NpcIndex(DWORD dwNpcIdx) ;								// Npc 인덱스를 설정하는 함수.
	DWORD	Get_NpcIndex() { return m_dwNpcIdx ; }						// Npc 인덱스를 반환하는 함수.

	void	Set_NpcKind(WORD wNpcKind) ;								// Npc 종류를 설정하는 함수.
	WORD	Get_NpcKind() { return m_wNpcKind ; }						// Npc 종류를 반환하는 함수.

	void	Set_RecallMap(MAPTYPE mapNum) ;								// Npc의 소환 맵 번호를 설정하는 함수.
	MAPTYPE Get_RecallMap() { return m_RecallMapNum ; }					// Npc의 소환 맵 번호를 반환하는 함수.

	void	Set_RecallPosX(float fXpos) ;								// Npc의 소환 X좌표를 설정하는 함수.
	void	Set_RecallPosZ(float fZpos) ;								// Npc의 소환 Z좌표를 설정하는 함수.
	float	Get_RecallposX() { return m_fXpos ; }						// Npc의 소환 좌표 X를 반환하는 함수.
	float	Get_RecallposZ() { return m_fZpos ; }						// Npc의 소환 좌표 Z를 반환하는 함수.
	void Get_RecallPos(VECTOR3* vOutPut);							// Npc의 소환 좌표를 반환하는 함수.

	void	Set_RecallDir(WORD wDir) ;									// Npc의 소환 방향을 설정하는 함수.
	WORD	Get_RecallDir() { return m_wDir ; }							// Npc의 소환 방향을 반환하는 함수.

	// 맵 서버에서 생성된 npc 아이디를 설정/반환하는 함수.
	void	Set_CreatedIdx(DWORD dwCreatedIdx) { m_dwCreatedIdx = dwCreatedIdx ; }
	DWORD	Get_CreatedIdx() { return m_dwCreatedIdx ; }

	// npc를 삭제할 준비가 되었는지 여부를 설정/반환하는 함수.
	void	Set_ReadyToDelete(BYTE byReady) { m_byReadyToDelete = byReady ; }
	BYTE	Is_ReadyToDelete() { return m_byReadyToDelete ; }

	// Npc가 소환 된 시간을 설정/반환 할 함수.
	void	Set_RecallTime(DWORD dwRecallTime) { m_dwRecallTime = dwRecallTime ; }
	DWORD	Get_RecallTime() { return m_dwRecallTime ; }

	// Npc 소환 실패 시 출력할 에러 메시지를 설정/반환하는 함수.
	void	Set_RecallFailedMsg(WORD wMsgNum) { m_wRecallFailedMsg = wMsgNum ; }
	WORD	Get_RecallFailedMsg() { return m_wRecallFailedMsg ; }

	void	Set_NpcName(char* pNpcName) ;
	char*	Get_NpcName() {return m_NpcName;}	

	void	Set_RecalledChenel(WORD wChenel );
	WORD	Get_RecalledChenel(){return m_wRecalledChenel;}

	void	Set_ParentNpcIndex(DWORD dwIndex);
	DWORD	Get_ParentNpcIndex(){return m_dwParentNpcIdx;}

	void	Set_ParentNpcType(BYTE byType);
	DWORD	Get_ParentNpcType(){return m_byParentType;}



	
	////////////////////////////////
	// [ 에러 메시지 처리 파트. ] //
	////////////////////////////////

	void Throw_Error(char* szErr, char* szCaption) ;					// 에러 메시지 처리를 하는 함수.
	void WriteLog(char* pMsg) ;											// 로그를 남기는 처리를 하는 함수.
};
