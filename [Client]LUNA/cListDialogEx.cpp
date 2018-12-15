#include "stdafx.h"
#include "cListDialogEx.h"
#include "Interface/cButton.h"
#include "Interface/cGuageBar.h"
#include "Interface/cFont.h"
#include "cPage.h"

cListDialogEx::cListDialogEx()																// 생성자 함수.
{
}

cListDialogEx::~cListDialogEx()																// 소멸자 함수.
{
}


void cListDialogEx::ListMouseCheck( LONG x, LONG y, DWORD we )								// 리스트 마우스 체크 함수.
{
	int selIdx = PtIdxInRow(x, y);															// 마우스를 체크해서 선택 된 라인을 받는다.

	if( selIdx < m_lLineNum )																// 선택 된 라인이 라인 수 보다 작으면,
	{
		mSelectedRowIdx = selIdx;															// 선택 된 라인 인덱스를 세팅한다.

		if( we & WE_LBTNCLICK )																// 마우스 왼쪽 클릭을 했다면,
		{
			PTRLISTPOS pos = m_ListItem.FindIndex(m_lTopListItemIdx+mSelectedRowIdx);		// 아이템 리스트에서 선택 된 위치 정보를 받는다.

			if( pos == NULL ) return;														// 위치 정보가 유효하지 않으면, 리턴 처리를 한다.

			LINKITEM* item = NULL;															// 아이템 정보를 받을 포인터 변수를 선언하고 NULL 처리를 한다.

			item = (LINKITEM*)m_ListItem.GetAt(pos);										// 위치 정보에 따른 아이템 정보를 리스트로 부터 받는다.

			if( item && item->dwType  > emLink_Null )										// 아이템 정보가 유효하고, 링크 타입이 NULL 이 아니면,
			{
				cbWindowFunc(m_ID, m_pParent, WE_ROWCLICK);									// 클릭 이벤트로 윈도우 함수를 실행 시킨다.

				we |= WE_ROWCLICK;															// 윈도우 이벤트는 라인 클릭으로 세팅한다.
			}
		}
	}
	else																					// 선택 된 라인이 라인 수를 오버 했다면,
	{
		mSelectedRowIdx = -1;																// 선택 된 라인을 -1로 세팅한다.
	}
}

void cListDialogEx::Render()																// 렌더 함수.
{
	if(!m_bActive) return;																	// 활성화 상태가 아니라면, 리턴 처리 한다.

	cDialog::RenderWindow();																// 윈도우를 렌더한다.

	static VECTOR2 m_tmpPos;																// 임시 위치 벡터를 선언한다.

	DWORD dwMergeColor = RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100) ;		// 머지 색상을 세팅한다.


	m_TopImage.RenderSprite(NULL,NULL,0,&m_absPos,dwMergeColor);							// 탑 이미지를 렌더한다.

	m_tmpPos.x = m_absPos.x;																// 임시 위치 벡터를 세팅한다.
	m_tmpPos.y = m_absPos.y+m_topHeight;

	for(int j = 0 ; j < m_middleNum ; j++)													// 미들 개수 만큼 for문을 돌린다.
	{
		m_MiddleImage.RenderSprite(NULL,NULL,0,&m_tmpPos,dwMergeColor);						// 미들 이미지를 렌더한다.
		m_tmpPos.y += m_middleHeight;														// 임시 위치 높이 값을 세팅한다.
	}

	m_DownImage.RenderSprite(NULL,NULL,0,&m_tmpPos,dwMergeColor);							// 다운 이미지를 렌더한다.

	LINKITEM* item = 0;
	LONG i = 0;
	RECT rect = {0};
	rect.left   = (LONG)m_absPos.x+m_textRelRect.left ;										// 레프트를 텍스트 렉트의 레프트로 세팅한다.
	rect.top    = (LONG)m_absPos.y+m_textRelRect.top + (m_lLineHeight*i) ;					// 탑을 텍스트 렉트의 탑 + 라인높이*카운트 로 세팅한다.
	rect.right  = 1 ;																		// 라이트를 1로 세팅한다.
	rect.bottom = 1 ;																		// 버텀을 1로 세팅한다.

	PTRLISTPOS pos = m_ListItem.FindIndex( m_lTopListItemIdx );								// 탑 리스트 아이템 인덱스로 아이템 리스트에서 위치 정보를 받는다.

	while(pos)																				// 위치 정보가 유효할 동안 while문을 돌린다.
	{
		item = (LINKITEM*)m_ListItem.GetNext(pos);											// 위치 정보에 해당하는 아이템 정보를 받는다.

		if(i < m_lLineNum)																	// 카운트가 라인 수 보다 작다면,
		{
			if(CFONT_OBJ->IsCreateFont(m_wFontIdx))											// 주어진 인덱스의 폰트가 생성되었는지 확인하고,
			{
				rect.top    = (LONG)m_absPos.y+m_textRelRect.top + (m_lLineHeight*i) ;		// 렉트의 탑을 세팅한다.

				if( item->dwType  > emLink_Null )											// 아이템 타입이 링크 NULL 보타 크다면,
				{
					if( mSelectedRowIdx == i )												// 선택 된 라인 인덱스가 카운트와 같다면,
					{
						m_tmpPos.x = m_absPos.x + 2;										// 임시 위치 X좌표를 세팅한다.
						m_tmpPos.y = m_absPos.y + m_textRelRect.top + 
							(((float)mSelectedRowIdx)*m_lLineHeight) - m_fGap;				// 임시 위치 Y좌표를 세팅한다.

						m_OverImage.RenderSprite(&m_OverImageScale,NULL,0,&m_tmpPos,dwMergeColor);	// 오버 이미지를 렌더 한다.
					}

					if(CFONT_OBJ->IsCreateFont(4))											// 4번 폰트가 생성 되었다면,
					{
						if( mSelectedRowIdx == i )											// 마우스 오버의 경우,
						{
							// 4번 폰트, 아이템 오버 색상으로 글자를 출력한다.
							//CFONT_OBJ->RenderFont(4,item->string,lstrlen(item->string),&rect,RGBA_MERGE(item->dwOverRGB, m_alpha * m_dwOptionAlpha / 100));
							CFONT_OBJ->RenderFontShadow( 4, 1, item->string, lstrlen(item->string), &rect, RGBA_MERGE( item->dwOverRGB, m_alpha * m_dwOptionAlpha / 100 ) );
						}
						else																// 일반적인 경우
						{
							// 4번 폰트, 아이템 색상으로 글자를 출력한다.
							//CFONT_OBJ->RenderFont(4,item->string,lstrlen(item->string),&rect,RGBA_MERGE(item->rgb, m_alpha * m_dwOptionAlpha / 100));
							CFONT_OBJ->RenderFontShadow( 4, 1, item->string, lstrlen(item->string), &rect, RGBA_MERGE( item->rgb, m_alpha * m_dwOptionAlpha / 100 ) );
						}
					}
					else																	// 4번 폰트가 생성 되지 않았다면,
					{
						if( mSelectedRowIdx == i )											// 마우스 오버의 경우,
						{
							// 현재 폰트, 아이템 오버 색상으로 글자 출력을 한다.
							//CFONT_OBJ->RenderFont(m_wFontIdx,item->string,lstrlen(item->string),&rect,RGBA_MERGE(item->dwOverRGB, m_alpha * m_dwOptionAlpha / 100));
							CFONT_OBJ->RenderFontShadow( m_wFontIdx, 1, item->string, lstrlen(item->string), &rect, RGBA_MERGE( item->dwOverRGB, m_alpha * m_dwOptionAlpha / 100 ) );
						}
						else																// 일반적인 경우
						{	// 현재 폰트, 아이템 색상으로 글자를 출력한다.
							//CFONT_OBJ->RenderFont(m_wFontIdx,item->string,lstrlen(item->string),&rect,RGBA_MERGE(item->rgb, m_alpha * m_dwOptionAlpha / 100));
							CFONT_OBJ->RenderFontShadow( m_wFontIdx, 1, item->string, lstrlen(item->string), &rect, RGBA_MERGE( item->rgb, m_alpha * m_dwOptionAlpha / 100 ) );
						}
					}
				}
				else																		// 아이템 타입이 링크 NULL 과 같거나 작으면,
				{
					// 현재 폰트, 아이템 색상으로 글자를 출력한다.
					//CFONT_OBJ->RenderFont(m_wFontIdx,item->string,lstrlen(item->string),&rect,RGBA_MERGE(item->rgb, m_alpha * m_dwOptionAlpha / 100));
					CFONT_OBJ->RenderFontShadow( m_wFontIdx, 1, item->string, lstrlen(item->string), &rect, RGBA_MERGE( item->rgb, m_alpha * m_dwOptionAlpha / 100 ) );

					// 현재 폰트, 스트링의 가로 영역을 받는다.
					LONG lWid = CFONT_OBJ->GetTextExtentEx( m_wFontIdx, item->string, lstrlen(item->string) );

					// 다음 아이템 정보를 받는다.
					LINKITEM* pNext = (LINKITEM*)item->NextItem;						

					// 동일 라인에 다음 아이템이 존재하면..

					int nTempLeft = rect.left ;
					while(pNext)
					{
						// 렉트의 레프트를 다시 세팅한다.
						rect.left += lWid;	

						// 현재 폰트, 아이템 색상으로 다음 아이템 스트링을 출력한다.
						//CFONT_OBJ->RenderFont(m_wFontIdx,pNext->string,lstrlen(pNext->string),&rect,RGBA_MERGE(pNext->rgb, m_alpha * m_dwOptionAlpha / 100));
						CFONT_OBJ->RenderFontShadow( m_wFontIdx, 1, pNext->string, lstrlen(pNext->string), &rect, RGBA_MERGE( pNext->rgb, m_alpha * m_dwOptionAlpha / 100 ) );

						// 다음 아이템 스트링의 가로 길이를 받는다.
						lWid = CFONT_OBJ->GetTextExtent( m_wFontIdx, pNext->string, lstrlen(pNext->string) );

						// 다음 아이템 정보를 받는다.
						pNext = (LINKITEM*)pNext->NextItem;
					}

					rect.left = nTempLeft ;
				}
			}

			i++;																			// 카운트 변수를 증가 시킨다.
		}
		else		
			break;																			// 카운트가 라인수와 같거나 크면,
	}


	if(IsMaxLineOver())																		// 최대 라인을 오버 했을 경우,
	{	
		if(m_pUpwardBtn)																	// 업 버튼 정보가 유효할 경우,
		{
			m_pUpwardBtn->Render();															// 업 버튼을 렌더 한다.
		}

		if(m_pDownwardBtn)																	// 다운 버튼 정보가 유효할 경우,
		{
			m_pDownwardBtn->Render();														// 다운 버튼을 렌더 한다.
		}

		if(m_pGuageBar)																		// 게이지바 정보가 유효할 경우,
		{
			m_pGuageBar->Render();															// 게이지바를 렌던한다.
		}
	}	

	cDialog::RenderComponent();																// 윈도우 컴포넌트들을 렌더 한다.
}

