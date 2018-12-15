#pragma once
#include "stdafx.h"

struct EVENTINFO
{
	DWORD	Index;
	DWORD	Delay;
	DWORD	Next;
	DWORD	Item;
	DWORD	Count;
};

class cGiftEvent;

#define GIFTMGR	USINGTON( cGiftManager )

class cGiftManager
{
	// 이벤트 정보 테이블
	CYHHashTable<EVENTINFO>			mEventInfoTable;
	
	// 이벤트 객체 관리
	// 090923 ONS 메모리풀 교체
	CPool<cGiftEvent>*				mpGiftEventPool;		// 이벤트 객체 메모리풀
	CYHHashTable<cGiftEvent>		mGiftEventTable;		// 이벤트 객체 테이블
	CIndexGenerator					mIDCreater;				// 이벤트 객체 ID 생성기

	DWORD							mStartEventCount;		// 시작 이벤트 갯수
	DWORD*							mStartEventIndex;		// 시작 이벤트

public:
	cGiftManager(void);
	~cGiftManager(void);

	void	Init();											// 초기화
	void	Release();										// 

	void	AddEvent( DWORD CharacterIdx, DWORD EventIdx );	// 이벤트 추가
	void	RemoveEvent( DWORD EventID );					// 이벤트 삭제
	void	ExcuteEvent( DWORD CharacterIdx, DWORD EventIdx );	// 이벤트 실행

	void	Process();										// 이벤트 프로세스

	DWORD	GetStartEventCount() { return mStartEventCount; }
	DWORD*	GetStartEventIndex() { return mStartEventIndex; }
};

EXTERNGLOBALTON( cGiftManager )
