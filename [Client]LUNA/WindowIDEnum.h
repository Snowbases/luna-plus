#ifndef _WINDOWIDENUM_H_
#define _WINDOWIDENUM_H_

// UNIT - ¸ÞÀÎ Å¸ÀÌÆ²/////////////////////////////////////////////
#ifdef WINDOW_ID
#undef WINDOW_ID
#endif

#ifdef WINDOW_ID_DEFINE
#undef WINDOW_ID_DEFINE
#endif

#define WINDOW_ID(a)	a
#define WINDOW_ID_DEFINE(a,def)	a = def

#define IDENUM_PIECE_1
enum IDENUM_PIECE_1
{
#include "WindowIDs.h"
};
#undef IDENUM_PIECE_1

#define IDENUM_PIECE_2
enum IDENUM_PIECE_2
{
#include "WindowIDs.h"
};
#undef IDENUM_PIECE_2

#define IDENUM_PIECE_3
enum IDENUM_PIECE_3
{
#include "WindowIDs.h"
};
#undef IDENUM_PIECE_3

#define IDENUM_PIECE_4
enum IDENUM_PIECE_4
{
#include "WindowIDs.h"
};
#undef IDENUM_PIECE_4

//extern char MT_IDNAME[MT_CODENUM][15];
extern int IDSEARCH(char * idName);
extern LPCTSTR NAMESEARCH(DWORD windowIndex);

#endif //_WINDOWIDENUM_H_
