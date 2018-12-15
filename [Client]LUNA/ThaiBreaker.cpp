#include "stdafx.h"
#include "ThaiBreaker.h"

#ifdef _TL_LOCAL_

ThaiBreakIterator g_TBreak((unsigned char*)"./Image/ftwrk.lex" );

int ThaiBreak( char *msg, BYTE *brk )
{
	return g_TBreak.FindThaiWordBreak( msg, strlen(msg), brk, 256, 0 );
}

#endif

