#pragma once
#include "questlimitkind.h"										// 퀘스트 리미트 종류 헤더를 불러온다.

class CQuestLimitKind_Stage : public CQuestLimitKind			// 퀘스트 리미트 종류 클래스를 상속받은 퀘스트 리미트 종류 스테이지 클래스.
{
	BYTE byClass ;												// 클래스를 받을 변수.
	BYTE byRacial ;												// 종족을 받을 변수.
	BYTE byJobGrade ;											// 잡 그레이드를 받을 변수.
	BYTE byJobIndex ;											// 잡 인덱스를 받을 변수.

	DWORD dwClassIndex ;										// 클래스 인덱스를 담을 변수.

	DWORD* m_pClassIndex ;										// 클래스 인덱스 포인터 변수.

	BYTE   m_byCount ;											// 카운트 변수.

public:
	CQuestLimitKind_Stage(DWORD dwLimitKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx);		// 생성자 함수.
	virtual ~CQuestLimitKind_Stage(void);																			// 소멸자 함수.

	virtual BOOL	CheckLimit( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );					// 리미트 체크 함수.(서버)
	virtual BOOL	CheckLimit( DWORD dwQuestIdx, DWORD dwSubQuestIdx );											// 리미트 체크 함수.(클라이언트)

	BYTE	GetClassIndexCount() { return m_byCount; }
	DWORD*	GetClassIndexs() { return m_pClassIndex; }
};
