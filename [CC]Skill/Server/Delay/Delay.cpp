#include "stdafx.h"
#include ".\delay.h"

cDelay::cDelay(void)
{
	mStart = 0;
	mDelay = 0;
	mRemain = 0;
	mRate = 0.0f;
}

cDelay::~cDelay(void)
{
}

void cDelay::Init( DWORD time )
{
	mDelay = time;
	mStart = 0;
	mRemain = 0;
	mRate = 0.0f;
}

void cDelay::Start()
{
	mStart = gCurTime;

	mRemain = mDelay;
	mRate = 100.0f;

	mbStart = TRUE;
}

DWORD cDelay::Check()
{
	/// 딜레이 카운트가 시작되었다면
	if( mbStart )
	{
		/// 시작부터 현재까지 경과시간
		DWORD spend = gCurTime - mStart;

		/// 설정된 시간보다 크거나 같다면
		if( mDelay <= spend )
		{
			/// 카운트 종료
			End();
			return 0;
		}
		else
		{
			/// 남은 시간 계산
			mRemain = mDelay - spend;

			/// 남은 시간 비율 계산
			mRate = ( float )( mDelay / mRemain );

			/// 남은시간을 돌려준다
			return mRemain;
		}
	}
	else
	{
		return 0;	
	}
}

void cDelay::End()
{
	mbStart = FALSE;

	mStart = 0;
	mRemain = 0;
	mRate = 0.0f;
}

BOOL cDelay::IsStart()
{
	return mbStart;
}

DWORD cDelay::GetStartTime()
{
	return mStart;
}

DWORD cDelay::GetDelayTime()
{
	return mDelay;
}

DWORD cDelay::GetRemainTime()
{
	return mRemain;
}

float cDelay::GetRemainRate()
{
	return mRate;
}