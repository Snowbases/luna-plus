#include "StdAfx.h"
#include "cDialogueList.h"
#include "MHFile.h"
#include "./Interface/cFont.h"
#include "GameIn.h"
#include "NpcScriptDialog.h"
#include "cPage.h"

cDialogueList::cDialogueList() :
m_dwDefaultColor(RGB(255,255,255)),
m_dwStressColor(RGB(0,255,255))
{
	LoadScript();
}

cDialogueList::~cDialogueList()
{}

void cDialogueList::LoadScript()
{
	CMHFile file;
	file.Init(
		"Data/Script/Npc/Npc_Msg.bin",
		"rb");
	DWORD messageIndex = 0;
	WORD wLineIdx = 0;
	
	while(FALSE == file.IsEOF())
	{
		// 091207 LUJ, 1K 이하로 선언하면 메모리 오류가 발생한다... 그런데 파싱 코드가 겁나게 복잡하다.
		TCHAR buffer[1024] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));
		TCHAR textLine[1024] = {0};
		SafeStrCpy(
			textLine,
			buffer,
			sizeof(textLine) / sizeof(*textLine));

		LPCTSTR seperator = " \t#";
		LPCTSTR token = _tcstok(buffer, seperator);
		LPCTSTR comment = "@";
		LPCTSTR openMark = "{";
		LPCTSTR closeMark = "}";

		if(0 == token)
		{
			continue;
		}
		else if(0 == _tcsnicmp(comment, token, _tcslen(comment)))
		{
			continue;
		}
		else if(0 == _tcsnicmp(openMark, token, _tcslen(openMark)))
		{
			continue;
		}
		else if(0 == _tcsnicmp(closeMark, token, _tcslen(closeMark)))
		{
			continue;
		}
		else if(0 == _tcsicmp("msg", token))
		{
			LPCTSTR textMessageIndex = _tcstok(0, seperator);
			messageIndex = _ttoi(textMessageIndex ? textMessageIndex : "");
			wLineIdx = 0;
			continue;
		}

		// 091216 ShinJS --- Npc_Msg.bin의 Data를 저장(인덱스,라인,메세지)
		NpcMsgDataKey dataKey( messageIndex, wLineIdx );
		m_NpcMsgData.insert( std::make_pair( dataKey, textLine ) );
		++wLineIdx;
	}
}

#define NPCDIALOG_TEXTLEN	100														// NPC 다이얼로그의 텍스트 길이를 36으로 디파인한다.

#ifdef _TL_LOCAL_
#define NPCDIALOG_TEXTWIDTH	305
#endif

void cDialogueList::ParsingLine(DWORD dwId, LPCTSTR buf)
{
	DWORD dwColor = m_dwDefaultColor;
	WORD wLine = WORD(mMessageMap[dwId].size());
	char wBuff[512] = {0};
	int	nCut = 0;
	int nStrLen = 0;

#ifdef _TL_LOCAL_
	BYTE brk[512];
	int nBrkNum = 0;
	int nTextExtent = 0;
#endif

	while( *buf )																	// 버퍼의 정보가 유효하면, while문을 돌린다.
	{
		if( *buf == '$' )															// 버퍼가 '$'와 같으면,
		{
			if( nStrLen != 0 )														// 문자열 길이가 0과 같지 않으면,
			{
				AddLine( dwId, wBuff, dwColor, wLine, emLink_Null );
				ZeroMemory(
					wBuff,
					sizeof(wBuff));
				nStrLen = 0;
			}

			buf += 2;																// 버퍼 포인터를 2증가한다.

			if( *buf == 's' || *buf == 'S' )										// 버퍼의 글자가 s또는 S이면,
			{
				dwColor = m_dwStressColor;											// 색상을 스트레스 색상으로 세팅한다.
			}
			else																	// 그렇지 않을 경우,
			{
				dwColor = m_dwDefaultColor;											// 일반 색상으로 세팅한다.
			}

			++buf;																	// 버퍼 포인터를 증가한다.

			continue;																// while 계속 진행.
		}

#ifdef _TL_LOCAL_	//for WB
		//---
		wBuff[nStrLen] = *buf;
		if( IsDBCSLeadByte( *buf ) )
		{
			++buf;
			++nStrLen;
			++nCut;

			wBuff[nStrLen] = *buf;
		}

		++buf;
		++nStrLen;
		++nCut;

		nTextExtent = CFONT_OBJ->GetTextExtentEx( 0, wBuff, nCut );

		if( nTextExtent > NPCDIALOG_TEXTWIDTH )
		{
			nBrkNum = g_TBreak.FindThaiWordBreak( wBuff, nStrLen, brk, 512, FTWB_SEPARATESYMBOL );

			if( nBrkNum > 1 )
			{
				buf -= brk[nBrkNum-1];
				wBuff[nStrLen-brk[nBrkNum-1]] = 0;
			}
			else
			{
				wBuff[nStrLen] = 0;
			}

			AddLine( dwId, wBuff, dwColor, wLine, emLink_Null );
			memset( wBuff, 0, 512 );

			nStrLen = 0;
			nCut = 0;
			++wLine;
			if( *buf == ' ' )
			{
				++buf;
			}
		}

		//---
#else
		wBuff[nStrLen] = *buf;														// 임시 버퍼의 문자열 길이의 위치에 버퍼의 값을 세팅한다.

		if( IsDBCSLeadByte( *buf ) )												// 버퍼의 값이 한글 바이트의 첫 바이트라면,
		{
			++buf;																	// 버퍼 포인터를 증가한다.
			++nStrLen;																// 문자열 길이를 증가한다.
			++nCut;																	// 컷팅 길이를 증가한다.
			wBuff[nStrLen] = *buf;													// 임시 버퍼의 문자열 길이에 버퍼의 값을 세팅한다.
		}

		++nCut;																		// 컷팅 정보를 증가한다.
		++buf;																		// 버퍼 포인터를 증가한다.
		++nStrLen;																	// 문자열 길이를 증가한다.

		// 091202 ShinJS --- NPC Script Dialog 의 대화 List Dialog의 Text Width와 문자열의 Width를 비교한다.
		cNpcScriptDialog* pNpcScriptDlg	= GAMEIN->GetNpcScriptDialog();
		WORD wListFontIdx	= (pNpcScriptDlg != NULL ? pNpcScriptDlg->GetConversationListFontIdx() : 0);
		LONG textExtent		= (pNpcScriptDlg != NULL ? CFONT_OBJ->GetTextExtentEx( wListFontIdx, wBuff, nCut ) : nCut);
		LONG listWidth		= (pNpcScriptDlg != NULL ? pNpcScriptDlg->GetConversationListTextWidth() : NPCDIALOG_TEXTLEN);

		// 리스트의 문자열이 Text Width 보다 긴 경우 (NPC Script Dialog 를 못찾은 경우 기존방법 사용)
		if( textExtent > listWidth )
		{
			if( nStrLen != 0 )														// 문자열 길이가 0이 아이념,
			{	
				AddLine( dwId, wBuff, dwColor, wLine, emLink_Null );				// 라인을 추가한다.
				nStrLen = 0;
				ZeroMemory(
					wBuff,
					sizeof(wBuff));
			}

			nCut = 0;
			++wLine;
		}
#endif

	}

	if( nStrLen != 0 )																// 문자열 길이가 0과 같지 않으면,
	{
		wBuff[nStrLen] = ' ';
		AddLine( dwId, wBuff, dwColor, wLine, emLink_Null );
	}
}

LPCTSTR cDialogueList::GetDialogue(DWORD dwMsgId)
{
	const FullMessageMap::iterator fullMessageIterator = mFullMessageMap.find(
		dwMsgId);

	if(mFullMessageMap.end() != fullMessageIterator)
	{
		return fullMessageIterator->second.c_str();
	}

	std::string& text = mFullMessageMap[dwMsgId];
	WORD line = 0;

	while(DIALOGUE* const dialogue = GetDialogue(dwMsgId, line++))
	{
		text = text + dialogue->str;
	}

	return text.c_str();
}

DIALOGUE* cDialogueList::GetDialogue( DWORD dwMsgId, WORD wLine )					// 다이얼로그 정보를 반환하는 함수.
{
	MessageMap::iterator messageIterator = mMessageMap.find(dwMsgId);

	if(mMessageMap.end() == messageIterator)
	{
		// 091216 ShinJS --- Pasring 되어 있지 않은 경우 저장된 NpcMsgData를 Parsing 한다.		
		NpcMsgDataKey dataKey( dwMsgId, 0 );
		NpcMsgData::const_iterator iter = m_NpcMsgData.find( dataKey );
		while( iter != m_NpcMsgData.end() )
		{
			const std::string &npcMsg = iter->second;
			ParsingLine(
				dwMsgId,
				npcMsg.c_str());

			dataKey.second = dataKey.second + 1;
			iter = m_NpcMsgData.find( dataKey );
		}

		messageIterator = mMessageMap.find( dwMsgId );
		if(mMessageMap.end() == messageIterator)
			return 0;
	}

	DialogMap& dialogMap = messageIterator->second;

	DialogMap::iterator dialogIteartor = dialogMap.find(wLine);

	if(dialogMap.end() == dialogIteartor)
	{
		return 0;
	}

	return &(dialogIteartor->second);
}

void cDialogueList::AddLine( DWORD dwId, LPCTSTR str, DWORD color, WORD Line, WORD type)
{
	DialogMap& dialogMap = mMessageMap[dwId];
	DIALOGUE& dialog = dialogMap[dialogMap.size()];

	ZeroMemory(
		&dialog,
		sizeof(dialog));
	dialog.dwColor = color;
	dialog.wLine = Line;
	dialog.wType = type;
	SafeStrCpy(
		dialog.str,
		str,
		sizeof(dialog.str) / sizeof(*dialog.str));
}