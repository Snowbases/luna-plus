#include "StdAfx.h"
#include "FilteringTable.h"
#include "MHFile.h"

extern int g_nServerSetNum;

BOOL FindPartially(const CFilteringTable::Text& lhs, const CFilteringTable::Text& rhs)
{
	return 0 < _tcsstr(
		lhs.c_str(),
		rhs.c_str());
}

CFilteringTable::CFilteringTable() :
m_eFilterExceptionType(FET_NONE)
{
	LoadScript();
}

CFilteringTable::~CFilteringTable()
{}

CFilteringTable* CFilteringTable::GetInstance()
{
	static CFilteringTable instance;

	return &instance;
}

void CFilteringTable::LoadScript()
{
	CMHFile file;
	file.Init(
		_T("system\\resource\\filterWord.bin"),
		_T("rb"));

	Type type = TypeNone;

	while(FALSE == file.IsEOF())
	{
		TCHAR buffer[MAX_PATH] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));
		
		if(0 == _tcslen(buffer))
		{
			continue;
		}

		LPCTSTR commentMark = _T("@");

		if(0 == _tcsnicmp(buffer, commentMark, _tcslen(commentMark)))
		{
			continue;
		}
		else if(0 == _tcsicmp(buffer, _T("#GM")))
		{
			type = TypePartiallyBlockedName;
			continue;
		}
		else if(0 == _tcsicmp(buffer, _T("#SLANG")))
		{
			type = TypePartiallyBlockedKeyword;
			continue;
		}
		else if(0 == _tcsicmp(buffer, _T("#SYSTEM")))
		{
			type = TypeEntirelyBlockedName;
			continue;
		}

		TCHAR text[MAX_PATH] = {0};
		RemoveSpace(
			buffer,
			text,
			sizeof(text) / sizeof(*text));
		_tcsupr(
			text);

		KeywordContainer& keywordContainer = mFilterContainer[type];
		keywordContainer.insert(
			text);
	}
}

void CFilteringTable::RemoveSpace(LPCTSTR textInput, LPTSTR textOutput, size_t size) const
{
	TCHAR buffer[MAX_PATH] = {0};	

	for(PUCHAR inputPointer = PUCHAR(textInput);
		0 < *inputPointer;
		inputPointer = _mbsinc((const PUCHAR)inputPointer))
	{
		LPCTSTR removedSpeicalCharacter = _T("\t\n\r ~`!@#$%^&*()-_=+\\|<,>.?/");

		if(_tcschr(removedSpeicalCharacter, *inputPointer))
		{
			continue;
		}

		_tcsncat(
			buffer,
			LPCTSTR(inputPointer),
			IsDBCSLeadByte(*inputPointer) ? 2 : 1);
	}

	SafeStrCpy(
		textOutput,
		buffer,
		size);
}

BOOL CFilteringTable::IsInvalidText(Type type, LPCTSTR text) const
{
	const FilterContainer::const_iterator filterIterator = mFilterContainer.find(
		type);

	if(mFilterContainer.end() == filterIterator)
	{
		return FALSE;
	}

	const KeywordContainer& keywordContainer = filterIterator->second;

	TCHAR buffer[MAX_PATH] = {0};
	RemoveSpace(
		text,
		buffer,
		sizeof(buffer) / sizeof(*buffer));
	_tcsupr(
		buffer);

	if(TypeEntirelyBlockedName == type)
	{
		return keywordContainer.end() != keywordContainer.find(
			buffer);
	}

	const Text checkedText(buffer);
	
	for(KeywordContainer::const_iterator iterator = keywordContainer.begin();
		keywordContainer.end() != iterator;
		++iterator)
	{
		if(FindPartially(checkedText, *iterator))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFilteringTable::FilterChat(LPCTSTR pChat )
{
	return IsInvalidText(
		TypePartiallyBlockedKeyword,
		pChat);
}

BOOL CFilteringTable::IsUsableName(LPCTSTR text)
{
	if(IsInvalidText(TypeEntirelyBlockedName, text))
	{
		return FALSE;
	}
	else if(IsInvalidText(TypePartiallyBlockedName, text))
	{
		return FALSE;
	}
	else if(IsInvalidText(TypePartiallyBlockedKeyword, text))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CFilteringTable::IsInvalidCharInclude( unsigned char* pStr, BOOL AllowAllDBC /*= FALSE*/ )
{
	BOOL bOk = FALSE;
#ifdef _TL_LOCAL_
	enum eKind
	{
		NONE,
		CONSO,	//consonant
		V_UP,	//vowel in upper
		SV_UP,	//special vowel in upper
		V_SIDE,	//vowel in side
		V_UN,	//vowel in under
		V_UPSI,	//vowel in upper and side
		SOU,	//special in upper
		ENG,	//english and number
	}
	LastKind = NONE;

	LPCTSTR thai[255] = { 
		NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,
		NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,
		NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,
		ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,
		NONE,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,
		ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	NONE,	NONE,	NONE,	NONE,	NONE,
		NONE,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,
		ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	ENG,	NONE,	NONE,	NONE,	NONE,	NONE,
		NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,
		NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,

		NONE,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,
		CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,
		CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,	CONSO,

		V_SIDE,	V_UP,	V_SIDE,	V_UPSI,	V_UP,	V_UP,	V_UP,	V_UP,	V_UN,	V_UN,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,
		V_SIDE,	V_SIDE,	V_SIDE,	V_SIDE,	V_SIDE,	V_SIDE,	V_SIDE,	SV_UP,	SOU,	SOU,	SOU,	SOU,	SOU,	NONE,	NONE,	NONE,
		NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE
	};
#endif
	while( *pStr )
	{
		bOk = FALSE;

		if( IsDBCSLeadByte( *pStr ) )
		{
			if( AllowAllDBC )
			{
				bOk = TRUE;
			}
			else
			{
#ifdef _JAPAN_LOCAL_
				if( IsInDBCRange( MAKEWORD(pStr[1],pStr[0]) ) )
					bOk = TRUE;
#elif defined _ML_LOCAL_

				if( g_nServerSetNum != 1 || g_pServerSystem->IsTestServer() )
				{
					//big5 only chinese character (http://www.microsoft.com/globaldev/reference/dbcs/950.mspx)
					if( ( pStr[0] >= 0xa4 && pStr[0] <= 0xc5 ) || ( pStr[0] >= 0xc9 && pStr[0] <= 0xf8 ) )
					{
						if( ( pStr[1] >= 0x40 && pStr[1] < 0x7f ) || ( pStr[1] > 0xa0 && pStr[1] < 0xff ) )
							bOk = TRUE;
					}
					else if( pStr[0] == 0xc6 )
					{
						if( pStr[1] >= 0x40 && pStr[1] < 0x7f )
							bOk = TRUE;
					}
					else if( pStr[0] == 0xf9 )
					{
						if( ( pStr[1] >= 0x40 && pStr[1] < 0x7f ) || ( pStr[1] > 0xa0 && pStr[1] <= 0xdc ) )
							bOk = TRUE;
					}
				}
#elif defined _TW_LOCAL_
				{
					//big5 only chinese character (http://www.microsoft.com/globaldev/reference/dbcs/950.mspx)
					if( ( pStr[0] >= 0xa4 && pStr[0] <= 0xc5 ) || ( pStr[0] >= 0xc9 && pStr[0] <= 0xf8 ) )
					{
						if( ( pStr[1] >= 0x40 && pStr[1] < 0x7f ) || ( pStr[1] > 0xa0 && pStr[1] < 0xff ) )
							bOk = TRUE;
					}
					else if( pStr[0] == 0xc6 )
					{
						if( pStr[1] >= 0x40 && pStr[1] < 0x7f )
							bOk = TRUE;
					}
					else if( pStr[0] == 0xf9 )
					{
						if( ( pStr[1] >= 0x40 && pStr[1] < 0x7f ) || ( pStr[1] > 0xa0 && pStr[1] <= 0xdc ) )
							bOk = TRUE;
					}
					/*
					if( pStr[0] >= 0xa1 && pStr[0] <=0xf9 &&
					( pStr[1] >= 0x40 && pStr[1] <= 0x7e ||
					pStr[1] >= 0xa1 && pStr[1] <= 0xfe ) )
					{
					bOk = TRUE;
					}
					*/
				}
				/*			if( pStr[0] >= 0xa4 && pStr[0] <=0xc6 && pStr[1] >= 0x40 && pStr[1] <= 0x7e )//0xA440-0xC67E
				{
				bOk = TRUE;
				}
				else if( pStr[0] >= 0xc9 && pStr[0] <= 0xf9 && pStr[1] >= 0x40 && pStr[1] <= 0xd5 )//0xC940-0xF9D5
				{
				bOk = TRUE;
				}
				*/
				///////////
#elif defined _TL_LOCAL_

				bOk = FALSE;
#else
				if( pStr[0] >= 0xb0 && pStr[0] <=0xc8 && pStr[1] >= 0xa1 && pStr[1] <= 0xfe )//0xB0A1~0xC8FE
				{
					bOk = TRUE;
				}
				else if( pStr[0] >= 0x81 && pStr[0] <= 0xc6 && pStr[1] >= 0x41 && pStr[1] <= 0xfe )
				{
					bOk = TRUE;
				}

				if( pStr[0] >= 0xa1 && pStr[0] <= 0xac && pStr[1] >= 0x80 && pStr[1] <= 0xfe )
				{
					bOk = FALSE;
				}
#endif
			}

			++pStr;
		}
		else
		{
			//영문
			if( ( *pStr >= 'A' && *pStr <= 'Z' ) || ( *pStr >= 'a' && *pStr <= 'z' ) )
				bOk = TRUE;
			//숫자
			else if( *pStr >= '0' && *pStr <= '9' )
				bOk = TRUE;
#ifdef _JAPAN_LOCAL_
			else if( *pStr >= 0xa6 && *pStr <= 0xdf )	//일본 반각문자 //// Halfwidth Katakana Letter Wo ~ Halfwidth Katakana Semi-Voiced Sound Mark
				bOk = TRUE;
#endif
#ifdef _TL_LOCAL_
			else if( thai[*pStr] == CONSO || thai[*pStr] == V_SIDE )
			{
				bOk = TRUE;
			}
			else if( thai[*pStr] == V_UP || thai[*pStr] == SV_UP || thai[*pStr] == V_UN || thai[*pStr] == V_UPSI )
			{
				if( LastKind == CONSO )
					bOk = TRUE;
			}
			else if( thai[*pStr] == SOU )
			{
				if( LastKind == CONSO || LastKind == V_UP || LastKind == V_UN )
					bOk = TRUE;
			}

			LastKind = thai[*pStr];

#endif
			if (IsExceptionInvalidCharInclude(pStr))
			{
				bOk = TRUE;
			}
		}

		++pStr;

		if( bOk == FALSE )
		{
			SetExceptionInvalidCharIncludeType(FET_NONE);
			return TRUE;
		}
	}

	SetExceptionInvalidCharIncludeType(FET_NONE);
	return FALSE;
}

// desc_hseos_문자필터링01
// S 문자필터링 추가 added by hseos 2007.06.25
// ..필터링에서 제외할 문자 처리
BOOL CFilteringTable::IsExceptionInvalidCharInclude(unsigned char* pszText)
{
	switch(m_eFilterExceptionType)
	{
	case FET_WHISPER_CHARNAME:
		if (*pszText == '[' || *pszText == ']')
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFilteringTable::IsCharInString( const char* pStr, const char* pNoChar )
{
	while( *pStr )
	{
		if( IsDBCSLeadByte( *pStr ) )
		{
			++pStr;			
		}
		else
		{
			const char* p = pNoChar;
			while( *p )
			{
				if( *pStr == *p ) return TRUE;
				++p;
			}
		}
		++pStr;
	}

	return FALSE;
}

BOOL CFilteringTable::IsInDBCRange(WORD character) const
{
	struct DoubleByteCharacter
	{
		WORD Start;
		WORD End;
	};

	const DoubleByteCharacter doubleByteCharacterArray[] = 
	{
		// 2 BYTE Characters.
		{ 0x815b, 0x815b }, // Katakana-Hiragana Prolonged Sound Mark
		{ 0x8199, 0x81a5 }, // White Star ~ Black Down-pointing Ttriangle
		{ 0x81f2, 0x81f5 }, // Music Sharp Sign ~ Dagger
		{ 0x824F, 0x8258 }, // Fullwidth Digit Zero ~ Fullwidth Digit Nine
		{ 0x8260, 0x8279 }, // Fullwidth Latin Capital Letter A ~ Fullwidth Latin Capital Letter Z
		{ 0x8281, 0x829a }, // Fullwidth Latin Small Letter A ~ Fullwidth Latin Small Letter Z
		{ 0x829f, 0x82f1 }, // Hiragana Letter Small A ~ Hiragana Letter N
		{ 0x8340, 0x837e },	// Katakana Letter Small A ~ Katakana Letter Small Me 
		{ 0x8380, 0x8396 }, // Katakana Letter Small Mu ~ Katakana Letter Small Ke
		{ 0x8794, 0x8794 }, // N-Ary Summation
		{ 0x8740, 0x875d }, // Circled Digit One ~ Roman Numeral Ten
		{ 0xfa40, 0xfa49 }, // Small Roman Numeral One ~ Small Roman Numeral Ten
		// Kanji.. !!!!! TT
		{ 0x889f, 0x88fc },	{ 0x8940, 0x897e },	{ 0x8980, 0x89fc },	{ 0x8a40, 0x8a7e },	{ 0x8a80, 0x8afc },	{ 0x8b40, 0x8b7e },	
		{ 0x8b80, 0x8bfc },	{ 0x8c40, 0x8c7e },	{ 0x8c80, 0x8cfc },	{ 0x8d40, 0x8d7e },	{ 0x8d80, 0x8dfc },	{ 0x8e40, 0x8e7e },
		{ 0x8e80, 0x8efc },	{ 0x8f40, 0x8f7e },	{ 0x8f80, 0x8ffc },	{ 0x9040, 0x907e },	{ 0x9080, 0x90fc },	{ 0x9140, 0x917e },
		{ 0x9180, 0x91fc },	{ 0x9240, 0x927e },	{ 0x9280, 0x92fc },	{ 0x9340, 0x937e },	{ 0x9380, 0x93fc },	{ 0x9440, 0x947e },
		{ 0x9480, 0x94fc },	{ 0x9540, 0x957e },	{ 0x9580, 0x95fc },	{ 0x9640, 0x967e },	{ 0x9680, 0x96fc },	{ 0x9740, 0x977e }, 
		{ 0x9780, 0x97fc },	{ 0x9840, 0x9872 },	{ 0x989f, 0x98fc },	{ 0x9940, 0x997e },	{ 0x9980, 0x99fc },	{ 0x9a40, 0x9a7e },
		{ 0x9a80, 0x9afc },	{ 0x9b40, 0x9b7e },	{ 0x9b80, 0x9bfc },	{ 0x9c40, 0x9c7e },	{ 0x9c80, 0x9cfc },	{ 0x9d40, 0x9d7e },	
		{ 0x9d80, 0x9dfc },	{ 0x9e40, 0x9e7e },	{ 0x9e80, 0x9efc },	{ 0x9f40, 0x9f7e },	{ 0x9f80, 0x9ffc },	{ 0xe040, 0xe07e },
		{ 0xe080, 0xe0fc },	{ 0xe140, 0xe17e },	{ 0xe180, 0xe1fc },	{ 0xe240, 0xe27e },	{ 0xe280, 0xe2fc },	{ 0xe340, 0xe37e },
		{ 0xe380, 0xe3fc },	{ 0xe440, 0xe47e },	{ 0xe480, 0xe4fc },	{ 0xe540, 0xe57e },	{ 0xe580, 0xe5fc },	{ 0xe640, 0xe67e },
		{ 0xe680, 0xe6fc },	{ 0xe740, 0xe77e },	{ 0xe780, 0xe7fc },	{ 0xe840, 0xe87e },	{ 0xe880, 0xe8fc },	{ 0xe940, 0xe97e },	
		{ 0xe980, 0xe9fc },	{ 0xea40, 0xea7e },	{ 0xea80, 0xeaa4 },	{ 0xed40, 0xed7e },	{ 0xed80, 0xedfc },	{ 0xee40, 0xee7e },
		{ 0xee80, 0xeeec },	{ 0xfa5c, 0xfa7e },	{ 0xfa80, 0xfafc },	{ 0xfb40, 0xfb7e },	{ 0xfb80, 0xfbfc },	{ 0xfc40, 0xfc4b }
	};

	for(size_t i = sizeof(doubleByteCharacterArray) / sizeof(*doubleByteCharacterArray);
		0 < i--;)
	{
		const DoubleByteCharacter& doubleByteCharacter = doubleByteCharacterArray[i];

		if(doubleByteCharacter.Start > character)
		{
			continue;
		}
		else if(doubleByteCharacter.End < character)
		{
			continue;
		}

		return TRUE;
	}

	return FALSE;
}