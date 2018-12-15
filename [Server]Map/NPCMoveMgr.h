// 090227 ShinJS --- 이동NPC관리
// NPCMoveMgr.h: interface for the CNPCMoveMgr class.
//				 이동하는 NPC를 관리하는 Manager class
//////////////////////////////////////////////////////////////////////
#ifndef __NPCMOVEMGR_H__
#define __NPCMOVEMGR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MOVENPCRES CMoveNPCRes::GetInstance()
#define NPCMOVEMGR CNPCMoveMgr::GetInstance()

#define MAX_NPC_MOVEINFO	10									// NPC의 이동정보 최대 개수

// NPC의 이동정보(맵번호, 이동좌표)
struct stNPC_MoveInfo{
	WORD wMapNum;			// 이동맵 번호
	WORD wDir;				// 방향
	COMPRESSEDPOS cPos;		// 이동좌표(x, y)
};

// 이동 NPC 정보를 담은 class
class CMoveNPC{
	WORD							m_wNPCIdx;					// NPC Index
	WORD							m_wNPCKindIdx;				// NPC 종류
	WORD							m_wStartMapNum;				// 처음 생성될 맵 번호
	DWORD							m_dwMoveTime;				// 이동시간 간격(초)
	DWORD							m_dwReadyToTime;			// 이동전 준비시간(작업을 못하게 막는다)
	BOOL							m_bSendReadyToMove;			// 이동전 메세지 전송 여부	
	DWORD							m_dwLastMoveTime;			// 마지막 이동시간
	WORD							m_wCurrentMoveIdx;			// 현재 이동정보 Index

	CYHHashTable<stNPC_MoveInfo>	m_htMoveInfo;				// NPC의 이동정보(최대갯수=MAX_NPC_MOVEINFO)
public:
	CMoveNPC( WORD wNPCIdx, WORD wNPCKindIdx, WORD wStartMapNum, DWORD dwMoveTime, DWORD dwReadyToTime ){
		m_wNPCIdx			= wNPCIdx;
		m_wNPCKindIdx		= wNPCKindIdx;
		m_wStartMapNum		= wStartMapNum;
		m_dwMoveTime		= dwMoveTime;
		m_dwReadyToTime		= dwReadyToTime;
		m_bSendReadyToMove	= FALSE;
		m_dwLastMoveTime	= 0;
		m_wCurrentMoveIdx	= 0;

		// 이동정보 테이블 초기화
		m_htMoveInfo.Initialize( MAX_NPC_MOVEINFO );
	}
	~CMoveNPC()	{
		// 이동정보 테이블 제거
		stNPC_MoveInfo* pData = NULL;
		// 090707 ShinJS --- 메모리 해제 오류 수정
		m_htMoveInfo.SetPositionHead();
		while( (pData = m_htMoveInfo.GetData()) != NULL )
		{
			delete pData;
			pData = NULL;
		}
		m_htMoveInfo.RemoveAll();
	}

	// NPC 이동정보 추가
	void AddMoveInfo( stNPC_MoveInfo* pMoveInfo, WORD wIndex )	{ m_htMoveInfo.Add( pMoveInfo, wIndex ); }

	BOOL IsSendReadyToMove()	const { return m_bSendReadyToMove; }

	WORD GetNPCIdx()			const { return m_wNPCIdx; }
	WORD GetNPCKindIdx()		const { return m_wNPCKindIdx; }
	WORD GetStartMapNum()		const { return m_wStartMapNum; }
	DWORD GetMoveTime()			const { return m_dwMoveTime; }
	DWORD GetReadyToTime()		const { return m_dwReadyToTime; }
	DWORD GetLastMoveTime()		const { return m_dwLastMoveTime; }
	WORD GetCurMoveIdx()		const { return m_wCurrentMoveIdx; }

	void SetSendReadyToMove( BOOL bSendReadyToMove ) { m_bSendReadyToMove = bSendReadyToMove; }
	void SetLastMoveTime( DWORD dwLastMoveTime ) { m_dwLastMoveTime = dwLastMoveTime; }	

	// NPC 이동 (이동정보Index를 증가시킨다)
	void Move()	{ if( m_htMoveInfo.GetDataNum() > 0 )	m_wCurrentMoveIdx = (m_wCurrentMoveIdx + 1) % WORD(m_htMoveInfo.GetDataNum()); }
	void Move( WORD wMoveIdx ) { if( m_htMoveInfo.GetDataNum() > 0 )	m_wCurrentMoveIdx = (wMoveIdx + 1) % WORD(m_htMoveInfo.GetDataNum()); }
	stNPC_MoveInfo* GetCurMoveInfo() { return (stNPC_MoveInfo*)m_htMoveInfo.GetData( m_wCurrentMoveIdx ); }
	stNPC_MoveInfo* GetNextMoveInfo() { return (stNPC_MoveInfo*)m_htMoveInfo.GetData( (m_wCurrentMoveIdx + 1) % m_htMoveInfo.GetDataNum() ); }
};

// 이동NPC 정보를 모두 가지고 있는 ResourceClass
class CMoveNPCRes
{
	CYHHashTable<CMoveNPC>			 m_htMoveNpc;				// 이동NPC 정보를 담을 테이블
public:
	MAKESINGLETON( CMoveNPCRes );

	CMoveNPCRes();
	~CMoveNPCRes();

	CYHHashTable<CMoveNPC>* GetMoveNpcTable() { return &m_htMoveNpc; }
	CMoveNPC* GetMoveNpc( WORD wNpcIdx ) { return (CMoveNPC*)m_htMoveNpc.GetData( wNpcIdx ); }
};


// 이동NPC 정보를 관리하는 Manager
class CNPCMoveMgr
{
	CYHHashTable<CMoveNPC>			 m_htMoveNpc;				// 이동NPC 정보를 담을 테이블
public:
	MAKESINGLETON( CNPCMoveMgr );

	CNPCMoveMgr();
	~CNPCMoveMgr();

	void Init();
	void Process();												// 이동시간 체크

	BOOL AddMoveNpc( WORD wNpcIdx, WORD wMoveIdx );				// 이동NPC 추가
	void RemoveMoveNpc( WORD wNpcIdx );							// 이동NPC 제거

	BOOL IsReadyToMove( WORD wNpcIdx );							// 이동NPC 이동준비 상태인지 확인
};


#endif // #ifndef __NPCMOVEMGR_H__
