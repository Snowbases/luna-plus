#include "stdafx.h"

BOOL g_bUsingEnginePack = FALSE;

// √‚√≥: http://www.gpgstudy.com/forum/viewtopic.php?t=795&highlight=%C7%D8%BD%AC+%B9%AE%C0%DA%BF%AD
DWORD GetHashCodeFromText(LPCTSTR text)
{
	const DWORD length = strlen(text);
	DWORD hashCode = 5381;

	for(DWORD i = 0; i < length; ++i)
	{
		DWORD ch = DWORD(text[ i ]);
		hashCode = ((hashCode << 5) + hashCode) + ch;
	}

	return hashCode;
}