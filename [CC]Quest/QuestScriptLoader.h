// QuestScriptLoader.h: interface for the CQuestScriptLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTSCRIPTLOADER_H__19DDC2D1_0947_4501_B70B_C636D9250099__INCLUDED_)
#define AFX_QUESTSCRIPTLOADER_H__19DDC2D1_0947_4501_B70B_C636D9250099__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StrTokenizer.h"						// 스트링 토크 나이져 헤더를 불러온다.

class CMHFile;									// 묵향 파일 클래스를 선언한다.
class CQuestInfo;								// 퀘스트 정보 클래스를 선언한다.
class CSubQuestInfo;							// 서브 퀘스트 정보 클래스를 선언한다.
class CQuestString;								// 퀘스트 스트링 클래스를 선언한다.

class CQuestLimitKind;							// 퀘스트 제한 종류 클래스를 선언한다.
class CQuestCondition;							// 퀘스트 상태 클래스를 선언한다.
class CQuestEvent;								// 퀘스트 이벤트 클래스를 선언한다.
class CQuestExecute;							// 퀘스트 실행 클래스를 선언한다.

class CQuestNpcScriptData;						// 퀘스트 NPC 스크립트 데이터 클래스를 선언한다.
class CQuestNpcData;							// 퀘스트 NPC 데이터 클래스를 선언한다.

class CQuestScriptLoader						// 퀘스트 스크립트 로더 클래스.
{
public:
	CQuestScriptLoader();						// 생성자 함수.
	virtual ~CQuestScriptLoader();				// 소멸자 함수.

	// 퀘스트 정보를 리턴하는 함수.
	static CQuestInfo*		LoadQuestInfo( CMHFile* pFile, DWORD dwQuestIdx );
	// 서브 퀘스트 정보를 리턴하는 함수.
	static CSubQuestInfo*	LoadSubQuestInfo( CMHFile* pFile, DWORD dwQuestIdx, DWORD dwSubQuestIdx, CQuestInfo* pQuestInfo  );
	// 퀘스트 스트링 정보를 리턴하는 함수.
	static CQuestString*	LoadQuestString(CMHFile* pFile);
	//static void LoadQuestString(CMHFile* pFile, CQuestString* pQStr) ;

	// 퀘스트 제한 종류 정보를 리턴하는 함수.
	static CQuestLimitKind*	LoadQuestLimitKind( char* strKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	// 퀘스트 상태 정보를 리턴하는 함수.
	static CQuestCondition*	LoadQuestCondition( char* strKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	// 퀘스트 이벤트 정보를 리턴하는 함수.
	static CQuestEvent*		LoadQuestEvent( char* strKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );	
	// 퀘스트 실행 정보를 리턴하는 함수.
	static CQuestExecute*	LoadQuestExecute( char* strKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	
	// 퀘스트 NPC 데이터 정보를 리턴하는 함수.
	static CQuestNpcData*		LoadQuestNpcData( char* strKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );

	static void ParseDateInfo( const char* buf, struct tm& timeStart, struct tm& timeEnd, BOOL& bDaily, BOOL& bHasEndTime );
};

// 데이터 아웃에 읽어들인 토큰 값을 변환해 세팅하는 함수.
template<class T> void GetScriptParam( T& dataOut, CStrTokenizer* pTokens )	
{
	char* pOneToken = pTokens->GetNextTokenUpper();		// 토큰의 GetNextTokenUpper() 함수를 호출한다.
	dataOut = (T)atol(pOneToken);						// 토큰 값을 숫자로 변환해 데이터 아웃에 담는다.
}

#endif // !defined(AFX_QUESTSCRIPTLOADER_H__19DDC2D1_0947_4501_B70B_C636D9250099__INCLUDED_)
