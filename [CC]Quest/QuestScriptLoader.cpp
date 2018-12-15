// QuestScriptLoader.cpp: implementation of the CQuestScriptLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																	// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestScriptLoader.h"														// 퀘스트 스크립트 로더 헤더를 불러온다.

#include "MHFile.h"																	// 묵향 파일 헤더를 불러온다.
#include "QuestInfo.h"																// 퀘스트 정보 헤더를 불러온다.
#include "SubQuestInfo.h"															// 서브 퀘스트 정보 헤더를 불러온다.
#include "QuestString.h"															// 퀘스트 스트링 헤더를 불러온다.
#include "QuestEvent.h"																// 퀘스트 이벤트 헤더를 불러온다.

#include "QuestLimit.h"																// 퀘스트 리미트 헤더를 불러온다.
#include "QuestLimitKind.h"															// 퀘스트 리미트 종류 헤더를 불러온다.
#include "QuestLimitKind_Level.h"													// 퀘스트 리미트 종류 중 레벨 헤더를 불러온다.
#include "QuestLimitKind_Quest.h"													// 퀘스트 리미트 종류 중 퀘스트 헤더를 불러온다.

#include "QuestLimitKind_Stage.h"													// 퀘스트 리미지 종류 중 스테이지 헤더를 불러온다.

#include "QuestTrigger.h"															// 퀘스트 트리거 헤더를 불러온다.
#include "QuestCondition.h"															// 퀘스트 상태 헤더를 불러온다.
#include "QuestExecute.h"															// 퀘스트 실행 헤더를 불러온다.
#include "QuestExecute_Quest.h"														// 퀘스트 실행 퀘스트 헤더를 불러온다.
#include "QuestExecute_Count.h"														// 퀘스트 실행 카운트 헤더를 불러온다.
#include "QuestExecute_Item.h"														// 퀘스트 실행 아이템 헤더를 불러온다.
#include "QuestExecute_RandomItem.h"												// 퀘스트 실행 랜덤 아이템 헤더를 불러온다.
#include "QuestExecute_Time.h"														// 퀘스트 실행 시간 헤더를 불러온다.
#include "QuestExecute_SelectItem.h"												// 100414 ONS 퀘스트 실행 선택보상 아이템 헤더를 불러온다.

#include "QuestNpcScript.h"															// 퀘스트 NPC 스크립트 헤더를 불러온다.
#include "QuestNpc.h"																// 퀘스트 NPC 헤더를 불러온다.
#include "QuestNpcData.h"															// 퀘스트 NPC 데이터 헤더를 불러온다.

//#include "interface/cWindowDef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define LOADUNIT(ekind,strkind,classname,questidx,subquestidx)	\
		if( strcmp(strKind,strkind) == 0 )						\
			return new classname(ekind,pTokens,questidx,subquestidx);

CQuestScriptLoader::CQuestScriptLoader()											// 생성자 함수.
{
}

CQuestScriptLoader::~CQuestScriptLoader()											// 소멸자 함수.
{
}

CQuestInfo* CQuestScriptLoader::LoadQuestInfo( CMHFile* pFile, DWORD dwQuestIdx )	// 퀘스트 정보를 로딩하는 함수.
{
	CQuestInfo* pQuestInfo = new CQuestInfo( dwQuestIdx );							// 퀘스트 정보를 받을 포인터를 선언하고, 퀘스트 정보 클래스 만큼 메모리를 할당해 받는다.

	CSubQuestInfo* pSub;															// 서브 퀘스트 정보를 받을 포인터를 선언한다.

	char tok[256];																	// 임시 버퍼를 선언한다.

	DWORD dwSubQuestIdx = 0;														// 서브 퀘스트 인덱스를 받을 변수를 선언하고 0으로 세팅한다.

	while( !pFile->IsEOF() )														// 파일 포인터가 파일을 끝에 도달하지 않을 동안	while문을 돌린다.
	{
		pFile->GetString( tok );													// 임시 버퍼에 스트링을 읽어들인다.

		if( tok[0] == '}' )															// }와 같으면,
		{
			break;																	// while문을 탈출한다.
		}		
		else if( stricmp( tok, "$REPEAT" ) == 0 )										
		{
			pQuestInfo->SetRepeat();
		}
		// 100401 ShinJS --- 퀘스트 시간제한 설정.
		else if( stricmp( tok, "$DATELIMIT" ) == 0 )
		{
			char buf[MAX_PATH]={0,};
			pFile->GetLine( buf, MAX_PATH );

			typedef std::pair< struct tm, struct tm > QuestDateLimitData;
			QuestDateLimitData dateLimit;

			struct tm& timeStart = dateLimit.first;
			struct tm& timeEnd = dateLimit.second;

			BOOL bDaily = FALSE, bHasEndTime = FALSE;
			ParseDateInfo( buf, timeStart, timeEnd, bDaily, bHasEndTime );

			// 일일 퀘스트인 경우
			if( bDaily )
			{
				// 종료시각이 주어지지 않은 경우 다음날짜의 1분전에 종료시킨다.
				if( !bHasEndTime )
				{
					timeEnd = timeStart;
					timeEnd.tm_hour = ( timeEnd.tm_hour + 23 ) % 24;
					timeEnd.tm_min = (timeEnd.tm_min + 59 ) % 60;
				}

				timeStart.tm_wday = 0;		timeEnd.tm_wday = 1;		// 일~월
				pQuestInfo->AddDateLimit( dateLimit );

				timeStart.tm_wday = 1;		timeEnd.tm_wday = 2;		// 월~화
				pQuestInfo->AddDateLimit( dateLimit );

				timeStart.tm_wday = 2;		timeEnd.tm_wday = 3;		// 화~수
				pQuestInfo->AddDateLimit( dateLimit );

				timeStart.tm_wday = 3;		timeEnd.tm_wday = 4;		// 수~목
				pQuestInfo->AddDateLimit( dateLimit );

				timeStart.tm_wday = 4;		timeEnd.tm_wday = 5;		// 목~금
				pQuestInfo->AddDateLimit( dateLimit );

				timeStart.tm_wday = 5;		timeEnd.tm_wday = 6;		// 금~토
				pQuestInfo->AddDateLimit( dateLimit );

				timeStart.tm_wday = 6;		timeEnd.tm_wday = 0;		// 토~일
				pQuestInfo->AddDateLimit( dateLimit );
			}
			else
			{
				// 결과 저장
				pQuestInfo->AddDateLimit( dateLimit );
			}

			

		}
		else if( stricmp( tok, "$SUBQUEST" ) == 0 )										// $SUBQUEST와 같으면,
		{
			dwSubQuestIdx = pFile->GetDword();										// 서브 퀘스트 인덱스를 읽어들인다.

			pSub = LoadSubQuestInfo( pFile, dwQuestIdx, dwSubQuestIdx, pQuestInfo );// 서브 퀘스트 정보를 로딩하여, 포인터로 받는다.

			ASSERT(pSub);															// ASSERT 체크를 한다.

			pQuestInfo->AddSubQuestInfo( dwSubQuestIdx, pSub );						// 퀘스트 정보의 서브 퀘스트 정보로 추가한다.
		}
	}

	return pQuestInfo;																// 퀘스트 정보를 리턴한다.
}

CSubQuestInfo* CQuestScriptLoader::LoadSubQuestInfo( 
	CMHFile* pFile, DWORD dwQuestIdx, DWORD dwSubQuestIdx, CQuestInfo* pQuestInfo ) // 서브 퀘스트를 로딩하는 함수.
{
	CSubQuestInfo* pSub = new CSubQuestInfo( dwQuestIdx, dwSubQuestIdx );			// 서브 퀘스트 만큼 메모리를 할당하고 포인터로 받는다.
	
	char buf[1024];																	// 임시 버퍼를 선언한다.

	char Token[1024];																// 토큰 버퍼를 선언한다.

	while( !pFile->IsEOF() )														// 파일 포인터가 파일의 끝에 도달하지 않을 동안 while문을 돌린다.
	{
		pFile->GetString( Token );													// 토큰 버퍼에 스트링을 읽어들인다.

		if( Token[0] == '}' )														// }와 같으면,
		{
			break;																	// while문을 탈출한다.
		}

		if( strcmp(Token,"#LIMIT") == 0 )											// #LIMIT와 같으면,
		{
			pFile->GetLine( buf, 1024 );											// 임시버퍼로 라인을 읽어들인다.

			CStrTokenizer tok( buf, " \t\n" );										// 탭과 뉴라인으로 토큰 세팅하여 선언한다.

			//pSub->AddQuestLimit(new CQuestLimit(&tok,dwQuestIdx,dwSubQuestIdx));	// 서브 퀘스트에 퀘스트 리미트를 추가한다.
			
			CQuestLimit* pQuestLimit = new CQuestLimit(&tok,dwQuestIdx,dwSubQuestIdx);
			if(pQuestLimit)
				pSub->AddQuestLimit(pQuestLimit);
		}

//#ifdef _MAPSERVER_																	// 맵서버가 선언되었다면,

		else if( strcmp(Token,"#TRIGGER") == 0 )									// 읽어들인 토큰이 #TRIGGER와 같으면,
		{
			pFile->GetLine( buf, 1024 );											// 임시 버퍼로 라인을 읽어들인다.

			CStrTokenizer tok( buf, " \t\n" );										// 탭과 뉴라인으로 토큰 세팅하여 선언한다.
			// 반복퀘스트의 확인을 위해서 추가 RaMa - 04.10.26
			CQuestTrigger* pQuestTrigger = new CQuestTrigger( &tok, dwQuestIdx, dwSubQuestIdx );	// 트리거 포인터로 메모리를 할당해 받는다.

			if(pQuestTrigger)														// 트리거 정보가 유효한지 체크한다.
			{
				pQuestInfo->SetEndParam( pQuestTrigger->GetEndParam() );			// 종료 파라메터를 세팅한다.
			}

			pSub->AddQuestTrigger( pQuestTrigger );									// 서브 퀘스트의 트리거로 추가한다.
		}		
//#else																				// 맵 서버가 선언되지 않았다면,

		else if( strcmp(Token,"#NPCSCRIPT") == 0 )									// #NPCSCRIPT 와 같다면,
		{
			pFile->GetLine( buf, 1024 );											// 임시버퍼로 라인을 읽어들인다.

			CStrTokenizer tok( buf, " \t\n" );										// 탭과 뉴라인으로 토큰 세팅하여 선언한다.

			//pSub->AddQuestNpcScipt( new CQuestNpcScript( &tok, dwQuestIdx, dwSubQuestIdx ) );	// 서브 퀘스트에 NPC 스크립트를 추가한다.

			CQuestNpcScript* pQuestNpcScript = new CQuestNpcScript( &tok, dwQuestIdx, dwSubQuestIdx );
			if(pQuestNpcScript)
				pSub->AddQuestNpcScipt(pQuestNpcScript);
		}
		else if( strcmp(Token,"#NPCADD") == 0 )										// #NPCADD와 같다면,
		{
			pFile->GetLine( buf, 1024 );											// 임시버퍼로 라인을 읽어들인다.

			CStrTokenizer tok( buf, " \t\n" );										// 탭과 뉴라인으로 토큰 세팅하여 선언한다.

			//pSub->AddQuestNpc( new CQuestNpc( &tok, dwQuestIdx, dwSubQuestIdx ) );	// 서브 퀘스트 NPC를 추가한다.
			CQuestNpc* pQuestNpc = new CQuestNpc( &tok, dwQuestIdx, dwSubQuestIdx );
			if(pQuestNpc)
				pSub->AddQuestNpc(pQuestNpc);
		}		
		else if( strcmp(Token, "#MAXCOUNT") == 0 )									// #MAXCOUNT와 같다면,
		{
			pSub->SetMaxCount( pFile->GetDword() );									// 서브 퀘스트의 최고 카운트를 세팅한다.
		}
//#endif
	}

	return pSub;																	// 서브 퀘스트 정보를 리턴한다.
}

CQuestString* CQuestScriptLoader::LoadQuestString(CMHFile* pFile)					// 퀘스트 스트링을 로딩하여 반환하는 함수.
{
	CQuestString* pQStr = new CQuestString;											// 퀘스트 스트링 만큼 메모리를 할당하여 포인터로 받는다.

	char* pChekString = NULL;														// 체크 스트링 포인터를 선언하고 NULL 처리를 한다.

	char buf[1024];																	// 임시 버퍼를 선언한다.
	char Token[1024];																// 토큰 버퍼를 선언한다.

	int	tline=0;																	// 타이틀 라인길이들 담을 변수를 선언하고 0으로 세팅한다.
	int dline=0;																	// 설명 길이를 담을 변수를 선언하고 0으로 세팅한다.

	while( !pFile->IsEOF() )														// 파일 포인터가 파일 끝이 아닐동안 while문을 돌린다.
	{
		pFile->GetString(Token);													// 토큰 버퍼에 스트링을 읽어들인다.

		if( ( pChekString = strstr( Token, "}" ) ) != NULL)									// 토큰이 }와 같으면,
		{
			break;																	// while문을 탈출한다.
		}
		
		if(strcmp(Token,"#TITLE") == 0)												// 토큰이 #TITLE 과 같다면,
		{
			pFile->GetLine(buf, 1024);												// 임시 버퍼에 라인을 읽어들인다.

			pQStr->AddLine(buf, tline, TRUE);										// 퀘스트 스트링에 라인을 추가한다.
		}
		if(strcmp(Token,"#DESC") == 0)												// 토큰이 #DESC와 같다면,
		{
			while( !pFile->IsEOF() )												// 파일 포인터가 파일 끝이 아닐동안 while문을 돌린다.
			{
				pFile->GetLine(buf, 1024);											// 임시 버퍼에 라인을 읽어들인다.

				char * p = buf;														// 임시 버퍼를 포인터로 받는다.

				int nRt = 0;														// 결과 값을 담을 변수를 선언하고 0으로 세팅한다.

				while( *p )															// 포인터가 유효할 동안 while문을 돌린다.
				{
					if( IsDBCSLeadByte( *p ) )										// 리드 바이트 라면,
					{
						++p;														// 포인터를 증가시킨다.
					}
					else															// 리드 바이트가 아니라면,
					{
						if( *p == '{' )												// {와 같다면,
						{
							nRt = 1;												// 결과 값을 1로 세팅한다.
							break;													// while문을 탈출한다.
						}
						else if(  *p == '}' )										// }와 같다면,
						{
							nRt = 2;												// 결과 값을 2로 세팅한다.
							break;													// while문을 탈출한다.
						}
					}

					++p;															// 포인터를 증가시킨다.
				}

				if( nRt == 1 )														// 결과 값이 1과 같다면, 
				{
					continue;														// 계속 진행한다.
				}
				else if( nRt == 2 )													// 결과 값이 2와 같다면,
				{
					break;															// while문을 탈출한다.
				}

				pQStr->AddLine(buf, dline);											// 스트링 포인터에 라인을 추가한다.

				++dline;															// 라인 값을 증가시킨다.
			}
		}
	}

	return pQStr;																	// 스트링 포인터를 리턴한다.
}

//void CQuestScriptLoader::LoadQuestString(CMHFile* pFile, CQuestString* pQStr)		// 퀘스트 스트링을 로딩하여 반환하는 함수.
//{
//	char* pChekString = NULL;														// 체크 스트링 포인터를 선언하고 NULL 처리를 한다.
//
//	char buf[1024];																	// 임시 버퍼를 선언한다.
//	char Token[1024];																// 토큰 버퍼를 선언한다.
//
//	int	tline=0;																	// 타이틀 라인길이들 담을 변수를 선언하고 0으로 세팅한다.
//	int dline=0;																	// 설명 길이를 담을 변수를 선언하고 0으로 세팅한다.
//
//	while( !pFile->IsEOF() )														// 파일 포인터가 파일 끝이 아닐동안 while문을 돌린다.
//	{
//		pFile->GetString(Token);													// 토큰 버퍼에 스트링을 읽어들인다.
//
//		if( pChekString = strstr( Token, "}" ) )									// 토큰이 }와 같으면,
//		{
//			break;																	// while문을 탈출한다.
//		}
//		
//		if(strcmp(Token,"#TITLE") == 0)												// 토큰이 #TITLE 과 같다면,
//		{
//			pFile->GetLine(buf, 1024);												// 임시 버퍼에 라인을 읽어들인다.
//
//			//pQStr->AddLine(buf, tline, TRUE);										// 퀘스트 스트링에 라인을 추가한다.
//
//			ITEM* pItem = new ITEM ;
//
//			strcpy(pItem->string, buf) ;
//			pItem->line = tline ;
//			pItem->rgb = RGB(0, 0, 64) ;
//
//			pQStr->GetTitle()->AddTail(pItem) ;
//		}
//		if(strcmp(Token,"#DESC") == 0)												// 토큰이 #DESC와 같다면,
//		{
//			while( !pFile->IsEOF() )												// 파일 포인터가 파일 끝이 아닐동안 while문을 돌린다.
//			{
//				pFile->GetLine(buf, 1024);											// 임시 버퍼에 라인을 읽어들인다.
//
//				char * p = buf;														// 임시 버퍼를 포인터로 받는다.
//
//				int nRt = 0;														// 결과 값을 담을 변수를 선언하고 0으로 세팅한다.
//
//				while( *p )															// 포인터가 유효할 동안 while문을 돌린다.
//				{
//					if( IsDBCSLeadByte( *p ) )										// 리드 바이트 라면,
//					{
//						++p;														// 포인터를 증가시킨다.
//					}
//					else															// 리드 바이트가 아니라면,
//					{
//						if( *p == '{' )												// {와 같다면,
//						{
//							nRt = 1;												// 결과 값을 1로 세팅한다.
//							break;													// while문을 탈출한다.
//						}
//						else if(  *p == '}' )										// }와 같다면,
//						{
//							nRt = 2;												// 결과 값을 2로 세팅한다.
//							break;													// while문을 탈출한다.
//						}
//					}
//
//					++p;															// 포인터를 증가시킨다.
//				}
//
//				if( nRt == 1 )														// 결과 값이 1과 같다면, 
//				{
//					continue;														// 계속 진행한다.
//				}
//				else if( nRt == 2 )													// 결과 값이 2와 같다면,
//				{
//					break;															// while문을 탈출한다.
//				}
//
//				pQStr->AddLine(buf, dline);											// 스트링 포인터에 라인을 추가한다.
//
//				++dline;															// 라인 값을 증가시킨다.
//			}
//		}
//	}
//}

//CQuestString* CQuestScriptLoader::LoadQuestString(CMHFile* pFile)					// 퀘스트 스트링을 로딩하여 반환하는 함수.
//{
//	CQuestString* pQStr = new CQuestString;											// 퀘스트 스트링 만큼 메모리를 할당하여 포인터로 받는다.
//
//	char* pChekString = NULL;														// 체크 스트링 포인터를 선언하고 NULL 처리를 한다.
//
//	char buf[1024];																	// 임시 버퍼를 선언한다.
//	char Token[1024];																// 토큰 버퍼를 선언한다.
//
//	int	tline=0;																	// 타이틀 라인길이들 담을 변수를 선언하고 0으로 세팅한다.
//	int dline=0;																	// 설명 길이를 담을 변수를 선언하고 0으로 세팅한다.
//
//	while( !pFile->IsEOF() )														// 파일 포인터가 파일 끝이 아닐동안 while문을 돌린다.
//	{
//		pFile->GetString(Token);													// 토큰 버퍼에 스트링을 읽어들인다.
//
//		if( pChekString = strstr( Token, "}" ) )									// 토큰이 }와 같으면,
//		{
//			break;																	// while문을 탈출한다.
//		}
//		
//		if(strcmp(Token,"#TITLE") == 0)												// 토큰이 #TITLE 과 같다면,
//		{
//			pFile->GetLine(buf, 1024);												// 임시 버퍼에 라인을 읽어들인다.
//
//			pQStr->AddLine(buf, tline, TRUE);										// 퀘스트 스트링에 라인을 추가한다.
//		}
//		if(strcmp(Token,"#DESC") == 0)												// 토큰이 #DESC와 같다면,
//		{
//			while( !pFile->IsEOF() )												// 파일 포인터가 파일 끝이 아닐동안 while문을 돌린다.
//			{
//				pFile->GetLine(buf, 1024);											// 임시 버퍼에 라인을 읽어들인다.
//
//				char * p = buf;														// 임시 버퍼를 포인터로 받는다.
//
//				int nRt = 0;														// 결과 값을 담을 변수를 선언하고 0으로 세팅한다.
//
//				while( *p )															// 포인터가 유효할 동안 while문을 돌린다.
//				{
//					if( IsDBCSLeadByte( *p ) )										// 리드 바이트 라면,
//					{
//						++p;														// 포인터를 증가시킨다.
//					}
//					else															// 리드 바이트가 아니라면,
//					{
//						if( *p == '{' )												// {와 같다면,
//						{
//							nRt = 1;												// 결과 값을 1로 세팅한다.
//							break;													// while문을 탈출한다.
//						}
//						else if(  *p == '}' )										// }와 같다면,
//						{
//							nRt = 2;												// 결과 값을 2로 세팅한다.
//							break;													// while문을 탈출한다.
//						}
//					}
//
//					++p;															// 포인터를 증가시킨다.
//				}
//
//				if( nRt == 1 )														// 결과 값이 1과 같다면, 
//				{
//					continue;														// 계속 진행한다.
//				}
//				else if( nRt == 2 )													// 결과 값이 2와 같다면,
//				{
//					break;															// while문을 탈출한다.
//				}
//
//				pQStr->AddLine(buf, dline);											// 스트링 포인터에 라인을 추가한다.
//
//				++dline;															// 라인 값을 증가시킨다.
//			}
//		}
//	}
//
//	return pQStr;																	// 스트링 포인터를 리턴한다.
//}

// 퀘스트 리미트 종류를 로딩하는 함수.
CQuestLimitKind* CQuestScriptLoader::LoadQuestLimitKind( char* strKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	LOADUNIT( eQuestLimitKind_Level,	"&LEVEL",		CQuestLimitKind_Level, dwQuestIdx, dwSubQuestIdx );	// 레벨 제한 정보를 로딩하여 리턴한다.
	LOADUNIT( eQuestLimitKind_Money,	"&MONEY",		CQuestLimitKind_Level, dwQuestIdx, dwSubQuestIdx );	// 머니 제한 정보를 로딩하여 리턴한다.
	LOADUNIT( eQuestLimitKind_Quest,	"&QUEST",		CQuestLimitKind_Quest, dwQuestIdx, dwSubQuestIdx );	// 퀘스트 제한 정보를 로딩하여 리턴한다.
	LOADUNIT( eQuestLimitKind_SubQuest, "&SUBQUEST",	CQuestLimitKind_Quest, dwQuestIdx, dwSubQuestIdx );	// 서브 퀘스트 제한 정보를 로딩하여 리턴한다.
	LOADUNIT( eQuestLimitKind_Stage,	"&STAGE",		CQuestLimitKind_Stage, dwQuestIdx, dwSubQuestIdx );	// 스테이지 제한 정보를 로딩하여 리턴한다.
	// 071011 LYW --- QuestScriptLoader : Add quest limit to check running quest.
	LOADUNIT( eQuestLimitKind_RunningQuest, "&RUNNING_QUEST", CQuestLimitKind_Quest, dwQuestIdx, dwSubQuestIdx ) ; // 진행중인 퀘스트 제한 정보를 로딩하여 리턴한다.

	return NULL;																							// NULL을 리턴한다.
}

// 퀘스트 상태를 로딩하여 리턴하는 함수.
CQuestCondition* CQuestScriptLoader::LoadQuestCondition( char* strKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	CQuestEvent* pEvent = LoadQuestEvent( strKind, pTokens, dwQuestIdx, dwSubQuestIdx );					// 퀘스트 이벤트 정보를 로딩하여 받는다.

	ASSERT(pEvent);																							// ASSERT 체크를 추가하여 넣는다.

	CQuestCondition* pCondition = new CQuestCondition( pEvent, dwQuestIdx, dwSubQuestIdx );					// 퀘스트 상태 메모리를 할당하여 포인터로 받는다.

	return pCondition;																						// 포인터를 리턴한다.
}

// 퀘스트 이벤트를 로딩하여 리턴한다.
CQuestEvent* CQuestScriptLoader::LoadQuestEvent( char* strKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	LOADUNIT( eQuestEvent_NpcTalk,		"@TALKTONPC",	CQuestEvent, dwQuestIdx, dwSubQuestIdx );			// NPC 대화 이벤트를 로딩하여 리턴한다.
	LOADUNIT( eQuestEvent_Hunt,			"@HUNT",		CQuestEvent, dwQuestIdx, dwSubQuestIdx );			// 사냥 이벤트를 로딩하여 리턴한다.
	LOADUNIT( eQuestEvent_Count,		"@COUNT",		CQuestEvent, dwQuestIdx, dwSubQuestIdx );			// 카운트 이벤트를 로딩하여 리턴한다.
	LOADUNIT( eQuestEvent_GameEnter,	"@GAMEENTER",	CQuestEvent, dwQuestIdx, dwSubQuestIdx );			// 게임 엔터? 이벤트를 로딩하여 리턴한다.
	LOADUNIT( eQuestEvent_Level,		"@LEVEL",		CQuestEvent, dwQuestIdx, dwSubQuestIdx );			// 레벨 이벤트를 로딩하여 리턴한다.
	LOADUNIT( eQuestEvent_UseItem,		"@USEITEM",		CQuestEvent, dwQuestIdx, dwSubQuestIdx );			// 아이템 사용 이벤트를 로딩하여 리턴한다.
	LOADUNIT( eQuestEvent_MapChange,	"@MAPCHANGE",	CQuestEvent, dwQuestIdx, dwSubQuestIdx );			// 맵 체인지 이벤트를 로딩하여 리턴한다.
	LOADUNIT( eQuestEvent_Die,			"@DIE",			CQuestEvent, dwQuestIdx, dwSubQuestIdx );			// 다이 이벤트를 로딩하여 리턴한다.
	LOADUNIT( eQuestEvent_HuntAll,		"@HUNTALL",		CQuestEvent, dwQuestIdx, dwSubQuestIdx );			// 사냥 올~ 이벤트를 로딩하여 리턴한다.

	return NULL;																							// NULL을 리턴한다.
}

// 퀘스트 실행을 로딩하여 리턴한다.
CQuestExecute* CQuestScriptLoader::LoadQuestExecute( char* strKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	LOADUNIT( eQuestExecute_EndQuest,			"*ENDQUEST",			CQuestExecute_Quest, dwQuestIdx, dwSubQuestIdx );	// 퀘스트 종료.
	LOADUNIT( eQuestExecute_StartSub,			"*STARTSUB",			CQuestExecute_Quest, dwQuestIdx, dwSubQuestIdx );	// 서브 퀘스트 시작.
	LOADUNIT( eQuestExecute_EndSub,				"*ENDSUB",				CQuestExecute_Quest, dwQuestIdx, dwSubQuestIdx );	// 서브 퀘스트 종료.
	LOADUNIT( eQuestExecute_EndOtherSub,		"*ENDOTHERSUB",			CQuestExecute_Quest, dwQuestIdx, dwSubQuestIdx );	// 다른 서브 퀘스트 종료.
	
	LOADUNIT( eQuestExecute_AddCount,			"*ADDCOUNT",			CQuestExecute_Count, dwQuestIdx, dwSubQuestIdx );	// 카운트 추가.
	LOADUNIT( eQuestExecute_AddCountFQW,		"*ADDCOUNTFQW",			CQuestExecute_Count, dwQuestIdx, dwSubQuestIdx );	// 무기 카운트 추가.
	LOADUNIT( eQuestExecute_AddCountFW,			"*ADDCOUNTFW",			CQuestExecute_Count, dwQuestIdx, dwSubQuestIdx );	// 무기 카운트 추가.
	LOADUNIT( eQuestExecute_LevelGap,			"*ADDCOUNTLEVELGAP",	CQuestExecute_Count, dwQuestIdx, dwSubQuestIdx );	// 레벨 차이 카운트 추가.
	LOADUNIT( eQuestExecute_MonLevel,			"*ADDCOUNTMONLEVEL",	CQuestExecute_Count, dwQuestIdx, dwSubQuestIdx );	// 몬스터 레벨 카운트 추가.

	LOADUNIT( eQuestExecute_GiveQuestItem,		"*GIVEQUESTITEM",		CQuestExecute_Item, dwQuestIdx, dwSubQuestIdx );	// 퀘스트 아이템 반납.
	LOADUNIT( eQuestExecute_TakeQuestItem,		"*TAKEQUESTITEM",		CQuestExecute_Item, dwQuestIdx, dwSubQuestIdx );	// 퀘스트 아이템 획득.
	LOADUNIT( eQuestExecute_GiveItem,			"*GIVEITEM",			CQuestExecute_Item, dwQuestIdx, dwSubQuestIdx );	// 아이템 반납.
	LOADUNIT( eQuestExecute_GiveMoney,			"*GIVEMONEY",			CQuestExecute_Item, dwQuestIdx, dwSubQuestIdx );	// 머니 반납.
	LOADUNIT( eQuestExecute_TakeItem,			"*TAKEITEM",			CQuestExecute_Item, dwQuestIdx, dwSubQuestIdx );	// 아이템 획득.
	LOADUNIT( eQuestExecute_TakeMoney,			"*TAKEMONEY",			CQuestExecute_Item, dwQuestIdx, dwSubQuestIdx );	// 머니 획득.
	LOADUNIT( eQuestExecute_TakeExp,			"*TAKEEXP",				CQuestExecute_Item, dwQuestIdx, dwSubQuestIdx );	// 경험치 획득.
	LOADUNIT( eQuestExecute_TakeSExp,			"*TAKESEXP",			CQuestExecute_Item, dwQuestIdx, dwSubQuestIdx );	// SP 획득.
	LOADUNIT( eQuestExecute_TakeQuestItemFQW,	"*TAKEQUESTITEMFQW",	CQuestExecute_Item, dwQuestIdx, dwSubQuestIdx );	// 퀘스트 아이템 무기 획득.
	LOADUNIT( eQuestExecute_TakeQuestItemFW,	"*TAKEQUESTITEMFW",		CQuestExecute_Item, dwQuestIdx, dwSubQuestIdx );	// 퀘스트 아이템 무기 획득.
	LOADUNIT( eQuestExecute_TakeMoneyPerCount,	"*TAKEMONEYPERCOUNT",	CQuestExecute_Item, dwQuestIdx, dwSubQuestIdx );	// 카운트 당 머니 획득.
	LOADUNIT( eQuestExecute_TakeSelectItem,		"*SELECTITEM",			CQuestExecute_SelectItem, dwQuestIdx, dwSubQuestIdx );	// 100414 ONS 선택보상 등록.
	
	LOADUNIT( eQuestExecute_RandomTakeItem,		"*RANDOMTAKEITEM",		CQuestExecute_RandomItem, dwQuestIdx, dwSubQuestIdx );	// 랜덤 획득 아이템.

	LOADUNIT( eQuestExecute_RegenMonster,		"*REGENMONSTER",		CQuestExecute_Quest, dwQuestIdx, dwSubQuestIdx );		// 몬스터 리젠.
	LOADUNIT( eQuestExecute_MapChange,			"*MAPCHANGE",			CQuestExecute_Quest, dwQuestIdx, dwSubQuestIdx );		// 맵 체인지.
	
	LOADUNIT( eQuestExecute_ChangeStage,		"*CHANGESTAGE",			CQuestExecute_Quest, dwQuestIdx, dwSubQuestIdx );		// 스테이지 체인지.
	
	LOADUNIT( eQuestExecute_RegistTime,			"*REGISTTIME",			CQuestExecute_Time, dwQuestIdx, dwSubQuestIdx );		// 시간 등록.

	return NULL;	
}

// 퀘스트 NPC 데이터를 로딩하여 리턴하는 함수.
CQuestNpcData* CQuestScriptLoader::LoadQuestNpcData( char* strKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	LOADUNIT( 0, "@NPC", CQuestNpcData, dwQuestIdx, dwSubQuestIdx );		// NPC 데이터를 로딩하여 리턴한다.
	return NULL;															// NULL을 리턴한다.
}

void CQuestScriptLoader::ParseDateInfo( const char* buf, struct tm& timeStart, struct tm& timeEnd, BOOL& bDaily, BOOL& bHasEndTime )
{
	struct _StrDateParse{
		int GetValueAndDelete( std::string& str, const char* findStr )
		{
			const size_t findStrSize = strlen( findStr );
			size_t nPos = str.find( findStr );
			if( nPos == std::string::npos )
			{
				int val = atoi( str.c_str() );
				str = "";
				return val;
			}

			std::string substr = str.substr( 0, nPos );
			str = str.substr( nPos + findStrSize, str.length() );
			if( stricmp( "*", substr.c_str() ) == 0 )
				return 0;

			return atoi( substr.c_str() );
		}
		std::string& GetCutBlank( std::string& str )
		{			size_t blankPos = std::string::npos;
			while( (blankPos = str.find( " " ) ) != std::string::npos )
			{
				str.replace( blankPos, blankPos+1, "" );
			}

			return str;
		}
		int GetWeekDay( std::string& str )
		{
			str = GetCutBlank( str );
			if( stricmp( "Sun", str.c_str() ) == 0 )
				return 0;
			else if( stricmp( "Mon", str.c_str() ) == 0 )
				return 1;
			else if( stricmp( "Tus", str.c_str() ) == 0 )
				return 2;
			else if( stricmp( "Wed", str.c_str() ) == 0 )
				return 3;
			else if( stricmp( "Thu", str.c_str() ) == 0 )
				return 4;
			else if( stricmp( "Fri", str.c_str() ) == 0 )
				return 5;
			else if( stricmp( "Sat", str.c_str() ) == 0 )
				return 6;

			return -1;
		}

		void MakeUpperStr( std::string& str )
		{
			std::transform( str.begin(), str.end(), str.begin(), toupper );
		}

	}StrDateParse;

	try
	{
		timeEnd.tm_hour = 23;
		timeEnd.tm_min = 59;

		std::string strBuf( buf );
		StrDateParse.MakeUpperStr( strBuf );

		size_t afterPos = strBuf.find( "~", 0 );
		std::string strBefore = strBuf.substr( 0, afterPos );
		std::string strAfter = ( afterPos == std::string::npos ? "" : strBuf.substr( afterPos + 1, strBuf.length() ) );

		const char* monthDayChar = "/";
		const char* hourMinChar = ":";
		const char* nextDataChar = " ";

		timeStart.tm_mon = timeEnd.tm_mon = -1;

		// 일일 퀘스트
		if( strBefore.find( "DAILY", 0 ) != std::string::npos )
		{
			StrDateParse.GetValueAndDelete( strBefore, "DAILY" );
			bDaily = TRUE;
		}

		// 월/일 Parse
		if( strBefore.find( monthDayChar ) != std::string::npos )
		{
			timeStart.tm_mon	= StrDateParse.GetValueAndDelete( strBefore, monthDayChar ) - 1;		// Month (0 - 11, January = 0).
			timeStart.tm_mday	= StrDateParse.GetValueAndDelete( strBefore, nextDataChar );
		}

		// 시:분 Parse
		if( strBefore.find( hourMinChar ) != std::string::npos )
		{
			timeStart.tm_hour	= StrDateParse.GetValueAndDelete( strBefore, hourMinChar );
			timeStart.tm_min	= StrDateParse.GetValueAndDelete( strBefore, nextDataChar );
		}

		// 요일 Parse
		timeStart.tm_wday	= StrDateParse.GetWeekDay( strBefore );


		// 월/일 Parse
		if( strAfter.find( monthDayChar ) != std::string::npos )
		{
			timeEnd.tm_mon	= StrDateParse.GetValueAndDelete( strAfter, monthDayChar ) - 1;				// Month (0 - 11, January = 0).
			timeEnd.tm_mday	= StrDateParse.GetValueAndDelete( strAfter, nextDataChar );
		}

		// 시:분 Parse
		if( strAfter.find( hourMinChar ) != std::string::npos )
		{
			timeEnd.tm_hour	= StrDateParse.GetValueAndDelete( strAfter, hourMinChar );
			timeEnd.tm_min	= StrDateParse.GetValueAndDelete( strAfter, nextDataChar );
			bHasEndTime = TRUE;
		}

		// 요일 Parse
		timeEnd.tm_wday	= StrDateParse.GetWeekDay( strAfter );
	}
	catch(...)
	{
	}
}