
#include "stdafx.h"
#include "WindowIDEnum.h"

#ifdef WINDOW_ID
#undef WINDOW_ID
#endif

#ifdef WINDOW_ID_DEFINE
#undef WINDOW_ID_DEFINE
#endif

#define WINDOW_ID(a)	#a
#define WINDOW_ID_DEFINE(a,def)	#a

#define IDENUM_PIECE_ALL
char IDNAME[MAX_IDENUM][64] = 
{
#include "WindowIDs.h"
};
#undef IDENUM_PIECE_ALL

int IDSEARCH(char * idName)
{
	for(int i = 0 ; i < MAX_IDENUM ; i++)
	{
		if(strcmp(idName, IDNAME[i])==0)
		{
			return i;
		}
	}
	return -1;
}

LPCTSTR NAMESEARCH(DWORD windowIndex)
{
	const size_t maxWindowIndex = sizeof(IDNAME) / sizeof(*IDNAME);

	if(maxWindowIndex <= windowIndex)
	{
		return _T("?");
	}

	return IDNAME[windowIndex];
}