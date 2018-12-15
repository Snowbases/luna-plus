#pragma once
#include "stdafx.h"

struct EVENTINFO;

class cGiftEvent
{
	DWORD		mID;			// 이벤트 ID
	DWORD		mCharacterIdx;	// 대상 캐릭터
		
	EVENTINFO*	mpInfo;			// 이벤트 정보
	
	DWORD		mDelay;			// 남은 시간
	DWORD		mCheckTime;		//

public:
	cGiftEvent(void);
	~cGiftEvent(void);

	BOOL	Init( DWORD CharacterIdx, DWORD ID, EVENTINFO* pInfo );
	BOOL	Excute();			// 실행 함수

	DWORD	GetID() { return mID; }

	void	Release();
};
