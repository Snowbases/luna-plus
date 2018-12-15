#pragma once

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

#include <InitGuid.h>
#include <Unknwn.h>
#include <WinSock2.h>
#include <crtdbg.h>
#include <process.h>
#include <MSTcpIP.h>

#if !defined(_DLLEXPORT_)

// If _DLLEXPORT_ is NOT defined then the default is to import.
#if defined(__cplusplus)
#define DLLENTRY extern "C" __declspec(dllimport)
#else
#define DLLENTRY extern __declspec(dllimport)
#endif
#define STDENTRY DLLENTRY HRESULT WINAPI
#define STDENTRY_(type) DLLENTRY type WINAPI

// Here is the list of server APIs offered by the DLL (using the
// appropriate entry API declaration macros just #defined above).

STDENTRY DllRegisterServer(void);
STDENTRY DllUnregisterServer(void);

#else  // _DLLEXPORT_

// Else if _DLLEXPORT_ is indeed defined then we've been told to export.
#if defined(__cplusplus)
#define DLLENTRY extern "C" __declspec(dllexport)
#else
#define DLLENTRY __declspec(dllexport)
#endif
#define STDENTRY DLLENTRY HRESULT WINAPI
#define STDENTRY_(type) DLLENTRY type WINAPI

#endif // _DLLEXPORT_

#define GUID_SIZE 128
#define MAX_STRING_LENGTH 256
typedef void**	PPVOID;