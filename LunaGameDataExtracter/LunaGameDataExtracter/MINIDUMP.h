/* MINIDUMP.H */
#pragma once

#include <windows.h>
#include <DbgHelp.h>

class CMiniDump
{
public:
	static BOOL Begin(VOID);
	static BOOL End(VOID);
};