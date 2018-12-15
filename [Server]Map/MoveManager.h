/*
081031 LUJ, 플레이어 리콜 기능을 작업하면 이동 처리를 매니저로 분리함
*/
#pragma once

#define MOVEMGR	( &( CMoveManager::GetInstance() ) )

class CMoveManager  
{
public:
	CMoveManager();
	virtual ~CMoveManager();
	static CMoveManager& GetInstance();
	void NetworkMsgParse( MSGBASE* const, DWORD dwLength );

	// 081031 LUJ, 리콜 처리
public:
	// 081031 LUJ, 플레이어 소환을 요청한다
	void RequestToRecall( const MSG_RECALL_SYN& );
	// 081031 LUJ, 플레이어를 소환한다
	void Recall( DWORD key, DWORD guildScore = 0, DWORD familyPoint = 0 );
private:
	// 081031 LUJ, 처리할 리콜 정보를 저장한다
	void SetRecallMessage( DWORD key, const MSG_RECALL_SYN& );
	// 081031 LUJ, 저장된 리콜 정보를 반환한다
	const MSG_RECALL_SYN& GetRecallMessage( DWORD key ) const;
	// 081031 LUJ, 리콜 정보가 있다면 반환후 컨테이너에서 삭제한다
	MSG_RECALL_SYN PopRecallMessage( DWORD key );
	// 081031 LUJ, 리콜 신청한 정보를 저장한다. 키: 소환 시점의 틱카운트
	typedef stdext::hash_map< DWORD, MSG_RECALL_SYN >	RecallMessageMap;
	RecallMessageMap									mRecallMessageMap;
};