#pragma once

//#define WINVER 0x0500

//#include <afx.h>
#ifdef _USE_WINSOCK
	#include <winsock2.h>
#endif

#include <windows.h>
#include <stdio.h>
#include <ole2.h>
#include <initguid.h>

#ifdef _USE_WINSOCK
	#include <mstcpip.h>
#endif



#define GUID_SIZE 128
#define MAX_STRING_LENGTH 256
typedef void**	PPVOID;

