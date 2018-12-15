#pragma once

/*
070730 웅주. 길드  점수 계산 클래스

길드 점수 처리는 브로드캐스팅이나 반복문 제거를 염두에 두고 설계되었다. 따라서, 다른 맵과 동기를 하거나 하지 않는다.
길드 총 점수 또한 각각의 맵은 알지 못한다. 총 점수를 검증하려면 DB 프로시저를 거쳐야한다. 



처리 개요:	각 맵은 별도로 접속자 수를 집계하고, 추가할 점수를 계산한다. 1시간 단위로 이 점수는 DB에 업데이트되고 0으로 초기화된다.
			점수가 업데이트될 때마다 브로드캐스팅한다.

			프로세스 타임이 올때마다 리스트의 맨 처음에 있는 길드 정보를 꺼내 계산할 시각이 되었는지 체크한다. 
			되었으면 처리하고, 다음 처리시각을 현재 시각을 1시간 뒤로 해놓는다.

처리 경우:
(1) 길드 회원 로그인, 가입
	- 일단 대기 목록에 넣는다. 

(2) 길드 회원 로그아웃, 탈퇴, 추방
	- 길드 회원수를 1 감소시키고, 경과된 시간만큼 실수 값으로 포인트에 추가해둔다.

(3) 사냥 성공
	- 리얼 타임 요구로 인해 즉각 처리한다

(4) 1시간 경과
	- ( 길드 회원수 * 1 ) + ( 대기목록의 각각의 회원의 포인트) + 적립 포인트 = 총합을 DB에 업데이트한다.
	`업데이트가 성공하면 결과값을 전서버의 해당 길드에 브로드캐스팅한다.

*/


class CGuildScore
{
	friend class CGuildManager;

private:
	CGuildScore( DWORD unitTime, float tickScore );
	virtual ~CGuildScore()	{};

public:
	// 로그인, 가입.
	void	AddPlayer	( DWORD playerIndex );

	// , 로그아웃, 탈퇴, 추방시 호출. 회원이 그때까지 누적시킨 점수를 반환한다
	float	RemovePlayer( DWORD playerIndex );

	// 점수를 증가시킨다
	void AddScore( float score );

	// 점수를 계산 가능한 시점인지 검사한다
	BOOL IsEnableScore( DWORD currentTick ) const;

	// 누적된 점수를 계산한다
	float GetScore( DWORD currentTick ) const;

	// 다시 계산 가능한 상태로 만든다
	void Reset( DWORD currentTick );

	// 현재 회원수를 반환
	DWORD GetPlayerSize() const;
	void Send( MSGBASE*, DWORD size ) const;

private:
	struct Score
	{
		float	mScore;		// DB에 누적시킬 점수. 현재 점수가 아님!

		DWORD	mBeginTick;	// 시작한 시점의 틱카운트
		DWORD	mEndTick;	// 끝나는 시점의 틱카운트

		// 일괄 처리되는 회원
		// 키: 플레이어 인덱스
		typedef std::set< DWORD >	ReadyMember;
		ReadyMember					mReadyMember;

		// 개별 처리되는 회원. 입장 시각이 다르므로 개별적으로 처리되어야 한다
		// 키: 플레이어 인덱스, 값: 입장된 시점의 틱카운트
		typedef std::pair< DWORD, DWORD >			EventUnit;
		typedef stdext::hash_map< DWORD, DWORD >	PrepareMember;
		PrepareMember								mPrepareMember;
	}
	mScore;

	// 갱신 간격
	const DWORD mUnitTick;

	// 1 틱카운트당 점수
	const float mTickScore;

	// 갱신 간격당 얻는 점수
	const float mUnitTickScore;
};