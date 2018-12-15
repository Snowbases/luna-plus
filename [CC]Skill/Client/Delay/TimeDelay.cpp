#include "stdafx.h"
#include ".\timedelay.h"

cTimeDelay::cTimeDelay(void)
{
	mStart = 0;
	mDelay = 0;
	mRemain = 0;
	mRate = 0.0f;
}

cTimeDelay::~cTimeDelay(void)
{
}

void cTimeDelay::Start()
{
	mStart = gCurTime;
	mbStart = TRUE;
	mRemain = mDelay;
	mRate = 1.0f;
}

DWORD cTimeDelay::Check()
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
			mRate = (float)((double)mRemain / (double)mDelay);

			/// 남은시간을 돌려준다
			return mRemain;
		}
	}
	else
	{
		return 0;	
	}
}