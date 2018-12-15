#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif

#include "global.h"

/*
struct PERFORMANCE_CONTEXT
{
	CLOCK			dwClock;			// 0
	CLOCK			dwUsagedClock;		// 8
	DWORD			dwPrvTick;			// 16
	DWORD			dwFrameCount;		// 20
	DWORD			dwAvgFrame;			// 24

};*/

GLOBAL_FUNC_DLL void BeginAnalyze(PERFORMANCE_CONTEXT* pCtx)
{
	__asm
	{
		rdtsc
		mov			esi,dword ptr[pCtx]
		mov			dword ptr[esi],eax
		mov			dword ptr[esi+4],edx
	}

	
}
GLOBAL_FUNC_DLL BOOL EndAnalyze(PERFORMANCE_CONTEXT* pCtx)
{

//	DWORD			dwPrvTick;			// 16
//	DWORD			dwFrameCount;		// 20
//	DWORD			dwAvgFrame;			// 24

	BOOL			bResult = FALSE;
	__asm
	{
		rdtsc		
		mov			esi,dword ptr[pCtx]
		sub			eax,dword ptr[esi]
		sub			edx,dword ptr[esi+4]
		mov			dword ptr[esi+8],eax
		mov			dword ptr[esi+12],edx
		
		push		esi
		call		dword ptr[GetTickCount]
		pop			esi
		
		mov			edx,eax						; backup tick count
		sub			eax,dword ptr[esi+16]
		cmp			eax,1000
		jl			lb_inc_frame

		mov			dword ptr[esi+16],edx		; dwPrvTick = GetTickCount();
		mov			eax,dword ptr[esi+20]		
		xor			edx,edx
		mov			dword ptr[esi+24],eax
		mov			dword ptr[esi+20],edx
		inc			dword ptr[bResult];
		

		jmp			lb_return

lb_inc_frame:
		inc			dword ptr[esi+20]

lb_return:
	}
	return bResult;
}