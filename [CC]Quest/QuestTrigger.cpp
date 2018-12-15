// QuestTrigger.cpp: implementation of the CQuestTrigger class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"														// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestTrigger.h"												// 퀘스트 트리거 헤더를 불러온다.

#include "QuestScriptLoader.h"											// 퀘스트 스크립트 로더 헤더를 불러온다.
#include "QuestCondition.h"												// 퀘스트 상태 헤더를 불러온다.

#include "QuestExecute.h"												// 퀘스트 실행 헤더를 불러온다.
#include "QuestEvent.h"													// 퀘스트 이벤트 헤더를 불러온다.

#ifdef _MAPSERVER_														// 맵 서버가 선언되어 있다면,

#include "Player.h"														// 플레이어 헤더를 선언한다.

#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// 생성자 함수.
CQuestTrigger::CQuestTrigger( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	ReadTrigger( pTokens, dwQuestIdx, dwSubQuestIdx );					// 트리거를 읽어들인다.
}

CQuestTrigger::~CQuestTrigger()											// 소멸자 함수.
{
	Release();															// 해제 함수를 호출한다.
}

void CQuestTrigger::Release()											// 해제 함수.
{
	if( m_pQuestCondition )												// 퀘스트 상태 정보가 유효하면,
	{
		delete m_pQuestCondition;										// 퀘스트 상태 정보를 삭제한다.
		m_pQuestCondition = NULL;										// 퀘스트 상태 정보를 나타내는 포인터를 null 처리를 한다.
	}

	CQuestExecute* pQuestExecute = NULL;								// 퀘스트 실행 포인터를 NULL 처리를 한다.

	PTRLISTPOS pos = m_QuestExeList.GetHeadPosition();					// 퀘스트 실행 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while( pos )														// 위치 정보가 유효한동안 while문을 돌린다.
	{
		pQuestExecute = (CQuestExecute*)m_QuestExeList.GetNext( pos );	// 퀘스트 실행 정보를 받는다.

		if( pQuestExecute )												// 정보가 유효하다면,
		{
			delete pQuestExecute;										// 퀘스트 실행 정보를 삭제한다.
		}
	}
	m_QuestExeList.RemoveAll();											// 퀘스트 실행 리스트를 모두 비운다.
}

#include "QuestExecute_Quest.h"											// 퀘스트 실행 퀘스트 헤더를 불러온다.

// 트리거를 읽어들이는 함수.
BOOL CQuestTrigger::ReadTrigger( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	char* OneToken;														// 토큰 포인터를 선언한다.

	m_dwEndParam = 0;													// 종료 파라메터를 0으로 세팅한다.

	while( (OneToken = pTokens->GetNextTokenUpper()) != NULL )			// 토큰 정보가 유효한 동안 while문을 돌린다.
	{
		if( OneToken[0] == '@' )										// 토큰이 @와 같다면,
		{
			m_pQuestCondition = CQuestScriptLoader::LoadQuestCondition(	// 퀘스트 상태 정보를 로딩하여, 
				OneToken, pTokens, dwQuestIdx, dwSubQuestIdx );			// 포인터 변수로 받는다.
		}
		else if( OneToken[0] == '*' )									// 토큰이 *와 같다면,
		{
			// 반복퀘스트의 확인을 위해서 추가 RaMa - 04.10.26
			CQuestExecute* pQuestExecute = NULL ;						// 퀘스트 실행 정보를 받을 포인터 변수를 선언한다.

			pQuestExecute = CQuestScriptLoader::LoadQuestExecute(		// 퀘스트 실행 정보를 로딩하여,
				OneToken, pTokens, dwQuestIdx, dwSubQuestIdx );			// 퀘스트 실행 정보를 받을 포인터 변수에 받는다.

			// 퀘스트 실행이 퀘스트 종료와 같으면,
			if(pQuestExecute && pQuestExecute->GetQuestExecuteKind() == eQuestExecute_EndQuest)
			{
				// 퀘스트 실행 정보를 받는다.
				CQuestExecute_Quest* pQExe_Quest = (CQuestExecute_Quest*)pQuestExecute;

				if( pQExe_Quest )										// 퀘스트 실행 정보가 유효하면,
				{
					m_dwEndParam = pQExe_Quest->GetQuestExeIdx();		// 퀘스트 실행 인덱스를 받는다.
				}
			}
			m_QuestExeList.AddTail( pQuestExecute );					// 퀘스트 실행 리스트에 추가한다.
		}
	}
	return TRUE;														// TRUE를 리턴한다.
}

//퀘스트 이벤트를 처리하는 함수.
BOOL CQuestTrigger::OnQuestEvent( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup,	// 퀘스트 이벤트를 처리하는 함수.
								  CQuest* pQuest, CQuestEvent* pQuestEvent )
{	
	if( !m_pQuestCondition->CheckCondition( pQuestEvent ) )							// 이벤트 정보에 따른 퀘스트 상태 정보를 체크한다.
	{
		return FALSE;																// FALSE를 리턴한다.
	}

	int nErrorCode = -1 ;															// 에러 코드를 담을 변수를 선언하고 -1로 세팅한다.

	CQuestExecute* pQuestExecute ;													// 퀘스트 실행 포인터를 선언한다.

	PTRLISTPOS pos = m_QuestExeList.GetHeadPosition();								// 퀘스트 실행 리스트를 헤드로 세팅하고, 위치 정보를 받는다.

	while( pos )																	// 퀘스트 위치 정보가 유효한 동안 while문을 돌린다.
	{
		pQuestExecute = NULL ;														// 퀘스트 실행 정보를 담을 포인터를 null 처리를 한다.
		pQuestExecute = (CQuestExecute*)m_QuestExeList.GetNext( pos );				// 퀘스트 실행 정보를 받는다.

		if( !pQuestExecute ) continue ;												// 퀘스트 실행 정보가 유효하지 않으면, continue 처리를 한다.

		nErrorCode = pQuestExecute->CheckCondition( pPlayer, pQuestGroup, pQuest ) ;// 퀘스트를 실행 할 조건을 갖추고 있는지 체크하여 결과를 받는다.

		if( nErrorCode != -1 ) break ;												// 에러 코드가 -1이 아니면 while문을 탈출한다.
	}

#ifdef _MAPSERVER_																	// 맵 서버가 선언되어 있다면,

	if( nErrorCode != -1 )															// 실행 결과가 FALSE와 같다면,
	{
		MSG_INT Msg ;																// 메시지 구조체를 선언한다.
		Msg.Category = MP_QUEST;													// 카테고리를 퀘스트로 세팅한다.
		Msg.Protocol = MP_QUEST_EXECUTE_ERROR;										// 프로토콜을 퀘스트 실행 에러로 세팅한다.
		Msg.dwObjectID = pPlayer->GetID();											// 플레이어 아이디를 세팅한다.

		Msg.nData = nErrorCode ;													// 퀘스트 이벤트 처리 에러 코드를 세팅한다.

		pPlayer->SendMsg( &Msg, sizeof(Msg) );										// 플레이어에게 에러 메시지를 전송한다.

		return FALSE ;																// false return 처리를 한다.
	}

#endif

	pos = m_QuestExeList.GetHeadPosition();											// 퀘스트 실행 리스트를 헤드로 세팅하고, 위치 정보를 받는다.

	while( pos )																	// 퀘스트 위치 정보가 유효한 동안 while문을 돌린다.
	{
		pQuestExecute = NULL ;														// 퀘스트 실행 정보를 담을 포인터를 null 처리를 한다.
		pQuestExecute = (CQuestExecute*)m_QuestExeList.GetNext( pos );				// 퀘스트 실행 정보를 받는다.

		if( !pQuestExecute ) continue ;												// 퀘스트 실행 정보가 유효하지 않으면, continue 처리를 한다.

		if( pQuestExecute->Execute( pPlayer, pQuestGroup, pQuest ) == FALSE )		// 퀘스트 이벤트를 실행한다.
			return FALSE;
	}

	return TRUE;
}

//// 퀘스트 이벤트를 처리하는 함수.
//BOOL CQuestTrigger::OnQuestEvent( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest, CQuestEvent* pQuestEvent )
//{
//	// condition
//	if( !m_pQuestCondition->CheckCondition( pQuestEvent ) )				// 이벤트 정보에 따른 퀘스트 상태 정보를 체크한다.
//	{
//		return FALSE;													// FALSE를 리턴한다.
//	}
//
//	// execute
//	BOOL bError = TRUE;													// 에러 여부를 담는 변수를 TRUE로 세팅한다.
//
//	PTRLISTPOS pos = m_QuestExeList.GetHeadPosition();					// 퀘스트 실행 리스트를 헤드로 세팅하고, 위치 정보를 받는다.
//
//	CQuestExecute* pQuestExecute = NULL;								// 퀘스트 실행 포인터를 선언하고 null 처리를 한다.
//
//	while( pos )														// 퀘스트 위치 정보가 유효한 동안 while문을 돌린다.
//	{
//		pQuestExecute = (CQuestExecute*)m_QuestExeList.GetNext( pos );	// 퀘스트 실행 정보를 받는다.
//
//		if( pQuestExecute )												// 퀘스트 실행 정보가 유효하면,
//		{
//			bError = pQuestExecute->Execute( pPlayer, pQuestGroup, pQuest );	// 퀘스트를 실행하여 결과를 받는다.
//
//			if( bError == FALSE )	
//			{
//				break;								// 결과가 FALSE와 같으면, while문을 탈출한다.
//			}
//		}
//	}
//
//#ifdef _MAPSERVER_														// 맵 서버가 선언되어 있다면,
//
//	if( bError == FALSE )												// 실행 결과가 FALSE와 같다면,
//	{
//		// network send
//		MSGBASE Msg;													// 메시지 구조체를 선언한다.
//		Msg.Category = MP_QUEST;										// 카테고리를 퀘스트로 세팅한다.
//		Msg.Protocol = MP_QUEST_EXECUTE_ERROR;							// 프로토콜을 퀘스트 실행 에러로 세팅한다.
//		Msg.dwObjectID = pPlayer->GetID();								// 플레이어 아이디를 세팅한다.
//		pPlayer->SendMsg( &Msg, sizeof(Msg) );							// 플레이어에게 에러 메시지를 전송한다.
//	}
//
//#endif
//
//	return TRUE;
//}
