#include "stdafx.h"																// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestString.h"														// 퀘스트 스트링 헤더를 불러온다.

#include "cWindowDef.h"

#define TEXT_DELIMITER			'^'												// 디리미터를 디파인한다. (구획문자. 데이터의 시작을 나타냄.)
#define TEXT_NEWLINECHAR		'n'												// 뉴 라인을 디파인한다.
#define TEXT_FLAG				'$'												// 플래그를 디파인한다.
#define TEXT_TAB				'	'											// 탭을 디파인 한다.
#define MAX_STR_LENTH			40												// 최대 스트링 길이를 디파인한다.




CQuestString::CQuestString()													// 생성자 함수.
{
	memset(m_TitleBuf, 0, 128) ;

	m_bComplete = FALSE ;

	m_nCount = 0 ;
	m_nTotalCount = 0 ;
}

CQuestString::~CQuestString()													// 소멸자 함수.
{
	ITEM* pDeleteItem ;

	PTRLISTPOS pos = NULL ;

	pos = m_TitleList.GetHeadPosition() ;
	while(pos)
	{
		pDeleteItem = NULL ;
		pDeleteItem = (ITEM*)m_TitleList.GetNext(pos) ;

		if( pDeleteItem )
		{
			m_TitleList.Remove(pDeleteItem) ;

			delete pDeleteItem ;
			pDeleteItem = NULL ;
		}
	}

	m_TitleList.RemoveAll() ;


	pos = NULL ;
	pos = m_DescList.GetHeadPosition() ;
	while(pos)
	{
		pDeleteItem = NULL ;
		pDeleteItem = (ITEM*)m_DescList.GetNext(pos) ;

		if( pDeleteItem )
		{
			m_DescList.Remove(pDeleteItem) ;
			delete pDeleteItem ;
			pDeleteItem = NULL ;
		}
	}

	m_DescList.RemoveAll() ;
}

void CQuestString::AddLine(char* pStr, int& line, BOOL bTitle)					// ?쇱씤 異붽? ?⑥닔.
{
	int len=0;																	// 湲몄씠瑜??댁쓣 蹂?섎? ?좎뼵?섍퀬 0?쇰줈 ?명똿?쒕떎.
	int linelen = 0;															// ?쇱씤 湲몄씠瑜??댁쓣 蹂?섎? ?좎뼵?섍퀬 0?쇰줈 ?명똿?쒕떎.

	char tb[128] = { 0, };														// ?꾩떆 踰꾪띁瑜??좎뼵?쒕떎.

	DWORD Color = 0;															// ?됱긽???댁쓣 蹂?섎? ?좎뼵?섍퀬 0?쇰줈 ?명똿?쒕떎.
	BOOL bHighlight = FALSE;													// ?섏씠?쇱씠???щ?瑜?FALSE濡??명똿?쒕떎.

	while(*pStr)																// ?ㅼ뼱??臾몄옄???뺣낫媛 ?좏슚???숈븞 while臾몄쓣 ?뚮┛??
	{
		tb[len]=*pStr;															// ?꾩떆 踰꾪띁??臾몄옄瑜??대뒗??

		++linelen;																// ?쇱씤 湲몄씠瑜?利앷??쒗궓??

		LPSTR pPrevStr = CharPrev( pStr, pStr + 1 ) ;							// A?먯꽌 ?쒖옉?섎뒗 臾몄옄?댁뿉??B媛 媛由ы궎??臾몄옄??諛붾줈 ??臾몄옄???ъ씤?곕? ?살쓬.
		LPSTR pNextStr = CharNext( pPrevStr ) ;									// A媛 媛由ы궎??臾몄옄?댁뿉???ㅼ쓬 臾몄옄 ?ъ씤???살쓬.

		if( pStr + 1 != pNextStr )												// ???꾩튂媛 ?섎굹?대뒗 怨녹씠 ?ㅻⅤ硫?
		{
			tb[++len] = *(++pStr);												// 臾몄옄 ?섎굹瑜???異붽??섍퀬, 
			++linelen;															// ?쇱씤 湲몄씠瑜?利앷??쒗궓??
		}

		if(tb[0]==TEXT_TAB || tb[0] == ' ')										// ?꾩떆 踰꾪띁 泥?臾몄옄媛, ??씠嫄곕굹, 釉붾옲?щ씪硫? 
		{
			len = -1;															// 湲몄씠瑜??섎굹 媛먯냼 ?쒗궎怨?

			--linelen;															// ?쇱씤 湲몄씠???섎굹 媛먯냼 ?쒗궓??
		}	

		if(tb[len] == TEXT_FLAG)												// ?꾩떆 踰꾪띁???댁슜???뚮옒洹몄? 媛숈쑝硫?
		{
			if(*(pStr+=2) == 's')												// ?ъ씤?곕? ?먭컻 利앷? ?쒗궓 媛믪씠 s? 媛숈쑝硫?
			{
				Color = QUEST_DESC_COLOR;										// ?섏뒪???됱긽???명똿?쒕떎.
				bHighlight = TRUE;												// ?섏씠?쇱씠??紐⑤뱶瑜?TRUE濡??명똿?쒕떎.
			}
			else if(*(pStr) == 'e')												// ?ъ씤?곕? 利앷? ?쒗궓 媛믪씠 e? 媛숈쑝硫?
			{
				Color = QUEST_DESC_HIGHLIGHT;									// ?섏뒪???됱긽???섏씠?쇱씠???됱긽?쇰줈 ?명똿?쒕떎.
				bHighlight = FALSE;												// ?섏씠?쇱씠??紐⑤뱶瑜?FALSE 濡??명똿?쒕떎.
			}

			++pStr;																// ?ъ씤?곕? 利앷??쒗궓??
			
			if(len>1)															// 湲몄씠媛 1蹂대떎 ?щ㈃,
			{
				tb[len] = 0;													// ?꾩떆 踰꾪띁瑜?0?쇰줈 ?명똿?쒕떎.
				
				//QString* pStc = new QString;									// ?ㅽ듃留?硫붾え由щ? ?좊떦?섏뿬 ?ъ씤?곕줈 ?섍릿??
				//*pStc = tb;														// ?꾩떆 踰꾪띁???댁슜???ъ씤?곕줈 ?섍릿??
				//pStc->Line = line;												// ?쇱씤???명똿?쒕떎.
				//pStc->Color = Color;											// ?됱긽???명똿?쒕떎.

				ITEM* pItem = new ITEM ;
				strcpy(pItem->string, tb) ;
				pItem->line = line ;
				pItem->rgb = Color ;

				if(bTitle)														// ??댄? 紐⑤뱶?쇰㈃,
				{
					m_TitleList.AddTail(pItem);									// ??댄? 由ъ뒪?몄뿉 ?ㅽ듃留곸쓣 異붽??쒕떎.
				}
				else															// ??댄? 紐⑤뱶媛 ?꾨땲?쇰㈃,
				{
					m_DescList.AddTail(pItem);									// ?ㅻ챸 由ъ뒪?몄뿉 ?ㅽ듃留곸쓣 異붽??쒕떎.
				}
				
				memset(tb, 0, sizeof(tb));										// ?꾩떆 踰꾪띁瑜?硫붾え由????쒕떎.

				len=0;															// 湲몄씠瑜?0?쇰줈 ?명똿?쒕떎.
			}
		}
		else																	// ?꾩떆 踰꾪띁???댁슜???뚮옒洹몄? 媛숈? ?딆쑝硫?
		{
			++len;																// 湲몄씠瑜?利앷? ?쒗궓??
			++pStr;																// ?ъ씤?곕? 利앷? ?쒗궓??
		}

		if(linelen > MAX_STR_LENTH)												// ?쇱씤 湲몄씠媛 理쒕? 臾몄옄??湲몄씠瑜??섏쑝硫?
		{
			//QString* pStc = new QString;										// ?ㅽ듃留?硫붾え由щ? ?덈줈 ?좊떦?섑빐 ?ъ씤?곕줈 ?섍꺼 諛쏅뒗??
			//*pStc = tb;															// ?덈줈???ъ씤?곗뿉???꾩떆 踰꾪띁???댁슜???좊떦?쒕떎.
			//pStc->Line = line;													// ?쇱씤 ?뺣낫瑜??명똿?쒕떎.

			ITEM* pItem = new ITEM ;
			strcpy(pItem->string, tb) ;
			pItem->line = line ;

			if(bHighlight==FALSE)												// ?섏씠?쇱씠?멸? ?곸슜?섏? ?딅뒗 ?곹깭?쇰㈃,
			{
				pItem->rgb=RGBA_MAKE(255, 255, 255, 255);									// ?섏뒪???ㅻ챸?됱긽?쇰줈 ?명똿?쒕떎.
			}
			else																// ?섏씠?쇱씠?멸? ?곸슜?쒕떎硫?
			{
				pItem->rgb=QUEST_DESC_HIGHLIGHT;								// ?섏뒪???ㅻ챸 ?섏씠?쇱씠???됱긽?쇰줈 ?명똿?쒕떎.
			}

			if(bTitle)															// ?쒕ぉ?대씪硫?
			{
				m_TitleList.AddTail(pItem);										// ?쒕ぉ 由ъ뒪?몄뿉 異붽?瑜??쒕떎.
			}
			else																// ?쒕ぉ???꾨땲?쇰㈃,
			{
				m_DescList.AddTail(pItem);										// ?ㅻ챸 由ъ뒪?몄뿉 異붽?瑜??쒕떎.
			}

			++line;																// ?쇱씤??利앷? ?쒗궓??
			
			memset(tb, 0, sizeof(tb));											// ?꾩떆 踰꾪띁瑜?硫붾え由????쒕떎.

			len=0;																// 湲몄씠瑜?0?쇰줈 ?명똿?쒕떎.

			linelen=0;															// ?쇱씤 湲몄씠瑜?0?쇰줈 ?명똿?쒕떎.
		}
	}

	if(len)																		// 湲몄씠媛 ?덈떎硫?
	{
		//QString* pStc = new QString;											// ?ㅽ듃留?硫붾え由щ? ?덈줈 ?좊떦???ъ씤?곕줈 ?섍꺼 諛쏅뒗??
		//*pStc = tb;																// ?덈줈???ъ씤?곗뿉???꾩떆 踰꾪띁???댁슜???좊떦?쒕떎.
		//pStc->Line = line;														// ?쇱씤 ?뺣낫瑜??명똿?쒕떎.
		//pStc->Color = QUEST_DESC_COLOR;											// ?됱긽???명똿?쒕떎.

		ITEM* pItem = new ITEM ;
		strcpy(pItem->string, tb) ;
		pItem->line = line ;
		pItem->rgb = RGBA_MAKE(255, 255, 255, 255) ;

		if(bTitle)																// ?쒕ぉ?대씪硫?
		{
			m_TitleList.AddTail(pItem);											// ?쒕ぉ 由ъ뒪?몄뿉 異붽?瑜??쒕떎.
		}
		else																	// ?쒕ぉ???꾨땲?쇰㈃,
		{
			m_DescList.AddTail(pItem);											// ?ㅻ챸 由ъ뒪?몄뿉 異붽?瑜??쒕떎.
		}
	}	

}

//void CQuestString::AddLine(char* pStr, int& line, BOOL bTitle)
//{
//	int len=0;																	// 길이를 담을 변수를 선언하고 0으로 세팅한다.
//	int linelen = 0;															// 라인 길이를 담을 변수를 선언하고 0으로 세팅한다.
//
//	char tb[128] = { 0, };														// 임시 버퍼를 선언한다.
//
//	DWORD Color = 0;															// 색상을 담을 변수를 선언하고 0으로 세팅한다.
//	BOOL bHighlight = FALSE;													// 하이라이트 여부를 FALSE로 세팅한다.
//
//	while(*pStr)																// 들어온 문자열 정보가 유효한 동안 while문을 돌린다.
//	{
//		tb[len]=*pStr;															// 임시 버퍼에 문자를 담는다.
//
//		++linelen;																// 라인 길이를 증가시킨다.
//
//		LPSTR pPrevStr = CharPrev( pStr, pStr + 1 ) ;							// A에서 시작하는 문자열에서 B가 가리키는 문자의 바로 앞 문자의 포인터를 얻음.
//		LPSTR pNextStr = CharNext( pPrevStr ) ;									// A가 가리키는 문자열에서 다음 문자 포인터 얻음.
//
//		if( pStr + 1 != pNextStr )												// 두 위치가 나나내는 곳이 다르면,
//		{
//			tb[++len] = *(++pStr);												// 문자 하나를 더 추가하고, 
//			++linelen;															// 라인 길이를 증가시킨다.
//		}
//
//		if(tb[0]==TEXT_TAB || tb[0] == ' ')										// 임시 버퍼 첫 문자가, 탭이거나, 블랭크라면, 
//		{
//			len = -1;															// 길이를 하나 감소 시키고,
//
//			--linelen;															// 라인 길이도 하나 감소 시킨다.
//		}	
//
//		if(tb[len] == TEXT_FLAG)												// 임시 버퍼의 내용이 플래그와 같으면,
//		{
//			if(*(pStr+=2) == 's')												// 포인터를 두개 증가 시킨 값이 s와 같으면,
//			{
//				Color = QUEST_DESC_COLOR;										// 퀘스트 색상을 세팅한다.
//				bHighlight = TRUE;												// 하이라이트 모드를 TRUE로 세팅한다.
//			}
//			else if(*(pStr) == 'e')												// 포인터를 증가 시킨 값이 e와 같으면,
//			{
//				Color = QUEST_DESC_HIGHLIGHT;									// 퀘스트 색상을 하이라이트 색상으로 세팅한다.
//				bHighlight = FALSE;												// 하이라이트 모드를 FALSE 로 세팅한다.
//			}
//
//			++pStr;																// 포인터를 증가시킨다.
//			
//			if(len>1)															// 길이가 1보다 크면,
//			{
//				tb[len] = 0;													// 임시 버퍼를 0으로 세팅한다.
//
//				ITEM* pItem = new ITEM ;
//				strcpy(pItem->string, tb) ;
//				pItem->line = line ;
//				pItem->rgb = Color ;
//
//				if(bTitle)														// 타이틀 모드라면,
//				{
//					m_TitleList.AddTail(pItem);									// 타이틀 리스트에 스트링을 추가한다.
//				}
//				else															// 타이틀 모드가 아니라면,
//				{
//					m_DescList.AddTail(pItem);									// 설명 리스트에 스트링을 추가한다.
//				}
//				
//				memset(tb, 0, sizeof(tb));										// 임시 버퍼를 메모리 셋 한다.
//
//				len=0;															// 길이를 0으로 세팅한다.
//			}
//		}
//		else																	// 임시 버퍼의 내용이 플래그와 같지 않으면,
//		{
//			++len;																// 길이를 증가 시킨다.
//			++pStr;																// 포인터를 증가 시킨다.
//		}
//
//		if(linelen > MAX_STR_LENTH)												// 라인 길이가 최대 문자열 길이를 넘으면,
//		{
//			ITEM* pItem = new ITEM ;
//			strcpy(pItem->string, tb) ;
//			pItem->line = line ;
//			pItem->rgb = Color ;
//
//			if(bHighlight==FALSE)												// 하이라이트가 적용되지 않는 상태라면,
//			{
//				pItem->rgb=QUEST_DESC_COLOR;									// 퀘스트 설명색상으로 세팅한다.
//			}
//			else																// 하이라이트가 적용된다면,
//			{
//				pItem->rgb=QUEST_DESC_HIGHLIGHT;								// 퀘스트 설명 하이라이트 색상으로 세팅한다.
//			}
//
//			if(bTitle)															// 제목이라면,
//			{
//				m_TitleList.AddTail(pItem);										// 제목 리스트에 추가를 한다.
//			}
//			else																// 제목이 아니라면,
//			{
//				m_DescList.AddTail(pItem);										// 설명 리스트에 추가를 한다.
//			}
//
//			++line;																// 라인을 증가 시킨다.
//			
//			memset(tb, 0, sizeof(tb));											// 임시 버퍼를 메모리 셋 한다.
//
//			len=0;																// 길이를 0으로 세팅한다.
//
//			linelen=0;															// 라인 길이를 0으로 세팅한다.
//		}
//	}
//
//	if(len)																		// 길이가 있다면,
//	{
//		ITEM* pItem = new ITEM ;
//		strcpy(pItem->string, tb) ;
//		pItem->line = line ;
//		pItem->rgb = Color ;
//
//		if(bTitle)																// 제목이라면,
//		{
//			m_TitleList.AddTail(pItem);											// 제목 리스트에 추가를 한다.
//		}
//		else																	// 제목이 아니라면,
//		{
//			m_DescList.AddTail(pItem);											// 설명 리스트에 추가를 한다.
//		}
//	}	
//}