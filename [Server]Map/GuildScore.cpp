#include "StdAfx.h"
#include "GuildScore.h"
#include "UserTable.h"
#include "Object.h"


CGuildScore::CGuildScore( DWORD unitTick, float tickScore ) :
mUnitTick( unitTick ),
mTickScore( tickScore ),
mUnitTickScore( tickScore * unitTick )
{
	Reset( GetTickCount() );
}


void CGuildScore::AddPlayer( DWORD playerIndex )
{
	mScore.mPrepareMember[ playerIndex ] = GetTickCount();
}


float CGuildScore::RemovePlayer( DWORD playerIndex )
{
	Score::ReadyMember&		readyMember		= mScore.mReadyMember;
	Score::PrepareMember&	prepareMember	= mScore.mPrepareMember;

	DWORD beginTick;
	
	// 만약 대기 목록에 있는 경우 진입 시점의 시간을 감안하여 계산한다	
	if( prepareMember.end() != prepareMember.find( playerIndex ) )
	{
		beginTick = prepareMember[ playerIndex ];

		prepareMember.erase( playerIndex );
	}
	// 일괄 처리 멤버에 있을 경우 일괄처리된 시각을 사용한다.
	else if( readyMember.end() != readyMember.find( playerIndex ) )
	{
		beginTick = mScore.mBeginTick;

		readyMember.erase( playerIndex );
	}
	else
	{
		ASSERT( 0 && "It means class has fault in AddMemeber()" );
		return 0.0f;
	}

	DWORD		tick;
	const DWORD currentTick = GetTickCount();

	// 오버 플로된 경우 최대 구간을 감안하여 차이를 계산해야 한다.
	if( currentTick < beginTick )
	{
		tick = UINT_MAX - beginTick + currentTick;
	}
	else
	{
		tick = currentTick - beginTick;
	}

	const float score = float( tick ) / mUnitTick;
	ASSERT( 0 < score );

	return score;
}


BOOL CGuildScore::IsEnableScore( DWORD currentTick ) const
{
	// 틱카운트가 오버플로된 상태인 경우 시작값보다는 작고 끝값보다 틱카운트가 클 경우 점수를 계산할 수 있다
	if( mScore.mEndTick < mScore.mBeginTick )
	{
		if(	mScore.mBeginTick > currentTick	&&
			mScore.mEndTick	< currentTick )
		{			
			return TRUE;
		}
	}
	// 오버플로된 상태가 아니면 끝값보다 현재 틱카운트가 크면 계산할 수 있다.
	else if( mScore.mEndTick < currentTick )
	{
		return TRUE;
	}

	return FALSE;
}


float CGuildScore::GetScore( DWORD currentTick ) const
{
	// 여기에는 사냥, 탈퇴, 추방 등으로 인해 변경된 점수가 더해져 있다.
	float score = mScore.mScore;

	// 멤버마다 단위 점수를 부여
	{
		score += mScore.mReadyMember.size() * mUnitTickScore;
	}

	// 1시간 내 가입/로그인한 멤버 점수 계산
	for(	Score::PrepareMember::const_iterator it = mScore.mPrepareMember.begin();
			mScore.mPrepareMember.end() != it;
			++it )
	{
		const DWORD addedTick	= it->second;
		DWORD		tick;

		/*
		유한 범위를 갖는 두 값의 차이를 구하는 방법은 두 가지가 있다.

		(1)		  A    B			(2)	   B   A
				+----+----+				+----+----+

		A 다음에 B가 기록된다고 해보자. (1)의 경우는 쉽게 계산된다.
		(2)의 경우는 최대값에서 A를 뺀 차이에 B를 더해서 차이를 얻어야한다.
		*/
		if( currentTick < addedTick )
		{
			tick = UINT_MAX - addedTick + currentTick;
		}
		else
		{
			tick = currentTick - addedTick;
		}

		const float memberScore = mTickScore * tick;
		ASSERT( 0 < memberScore );

		score += memberScore;
	}

	return score;
}


void CGuildScore::Reset( DWORD currentTick )
{
	// 개별 처리 멤버를 일괄 처리 멤버로 이동시킨다.
	for(	Score::PrepareMember::const_iterator it = mScore.mPrepareMember.begin();
			mScore.mPrepareMember.end() != it;
			++it )
	{
		const DWORD playerIndex = it->first;

		mScore.mReadyMember.insert( playerIndex );
	}

	// 초기화: 이벤트를 지우고 점수도 초기화한다.
	// 시간 유닛을 1시간 뒤로 변경해놓고, 리스트 맨 뒤에 놓는다. 왜? 1시간이 지나면 다시 계산해야하니까..
	{
		mScore.mScore		= 0;
		mScore.mBeginTick	= currentTick;
		mScore.mEndTick		= currentTick + mUnitTick;

		mScore.mPrepareMember.clear();
	}
}


void CGuildScore::AddScore( float score )
{
	mScore.mScore += score;
}


DWORD CGuildScore::GetPlayerSize() const
{
	return mScore.mPrepareMember.size() + mScore.mReadyMember.size();
}

void CGuildScore::Send( MSGBASE* message, DWORD size ) const
{
	// 개별 처리 멤버 데이터 세팅
	for(	Score::PrepareMember::const_iterator it = mScore.mPrepareMember.begin();
			mScore.mPrepareMember.end() != it;
			++it )
	{
		const DWORD playerIndex = it->first;
		CObject*	object		= g_pUserTable->FindUser( playerIndex );

		if( object )
		{
			object->SendMsg( message, size );
		}
	}

	// 일괄 처리 멤버 데이터 세팅
	// 개별 처리 멤버 데이터 세팅
	for(	Score::ReadyMember::const_iterator it = mScore.mReadyMember.begin();
			mScore.mReadyMember.end() != it;
			++it )
	{
		const DWORD playerIndex = *it;
		CObject*	object		= g_pUserTable->FindUser( playerIndex );

		if( object )
		{
			object->SendMsg( message, size );
		}
	}	
}