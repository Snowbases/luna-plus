#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif

#include <math.h>
#include <stdio.h>
#include "global.h"

GLOBAL_FUNC_DLL BOOL __stdcall IsValidTri(VECTOR3* pv3Point_0,VECTOR3* pv3Point_1,VECTOR3* pv3Point_2)
{
	BOOL	bResult = FALSE;
	if (*pv3Point_0 == *pv3Point_1)
		goto lb_return;

	if (*pv3Point_0 == *pv3Point_2)
		goto lb_return;
	
	if (*pv3Point_1 == *pv3Point_2)
		goto lb_return;

	bResult = TRUE;

lb_return:
	return bResult;
}

GLOBAL_FUNC_DLL BOOL __stdcall IsValidFloat(float f)
{
	BOOL		bResult;
	__asm 
	{
		xor		eax,eax
		mov		edx,0xffc00000
		mov		ecx,dword ptr[f]
		cmp		edx,ecx
		jz		lb_return;

		inc		eax;
		
lb_return:	
		mov		dword ptr[bResult],eax
	}
	return bResult;

}
GLOBAL_FUNC_DLL BOOL __stdcall IsValidVector3(VECTOR3* pv3,DWORD dwNum)
{
	BOOL		bResult;
	__asm 
	{
		
		xor		edx,edx
		mov		eax,dword ptr[dwNum]
		mov		ebx,3
		mul		ebx
		mov		ecx,eax
		xor		eax,eax
		mov		edx,0xffc00000

lb_loop:
		mov		esi,dword ptr[pv3]
		mov		ebx,dword ptr[esi]			; load float
		cmp		ebx,edx
		
		jz		lb_return

		add		esi,4
		loop	lb_loop
		inc		eax
		
lb_return:
		mov		dword ptr[bResult],eax
	}
	return bResult;

}	
			


GLOBAL_FUNC_DLL void __stdcall SET_VECTOR3(VECTOR3* pv3,float fVal)
{
	pv3->x = fVal;
	pv3->y = fVal;
	pv3->z = fVal;
}
#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_ADD_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1,VECTOR3* pv3Arg2)
{
	__asm
	{
		mov			edi,dword ptr[pv3Arg1]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2
		
		mov			edi,dword ptr[pv3Arg2]

		; LOAD VECTOR
		movlps		xmm1,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm1,xmm2

		mov			edi,dword ptr[pv3Result]
		addps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_ADD_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1,VECTOR3* pv3Arg2)
{
	pv3Result->x = pv3Arg1->x + pv3Arg2->x;
	pv3Result->y = pv3Arg1->y + pv3Arg2->y;
	pv3Result->z = pv3Arg1->z + pv3Arg2->z;
}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_SUB_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1,VECTOR3* pv3Arg2)
{
//	RDTSC_BEGIN
	__asm
	{
		mov			edi,dword ptr[pv3Arg1]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2

		mov			edi,dword ptr[pv3Arg2]
			
		; LOAD VECTOR
		movlps		xmm1,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm1,xmm2

		mov			edi,dword ptr[pv3Result]
		subps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
//	RDTSC_END
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_SUB_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1,VECTOR3* pv3Arg2)
{
	pv3Result->x = pv3Arg1->x - pv3Arg2->x;
	pv3Result->y = pv3Arg1->y - pv3Arg2->y;
	pv3Result->z = pv3Arg1->z - pv3Arg2->z;
}
#endif
#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_MUL_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1,VECTOR3* pv3Arg2)
{
	__asm
	{
		mov			edi,dword ptr[pv3Arg1]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2


		mov			edi,dword ptr[pv3Arg2]
			
		; LOAD VECTOR
		movlps		xmm1,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm1,xmm2


		mov			edi,dword ptr[pv3Result]
		mulps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_MUL_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1,VECTOR3* pv3Arg2)
{
	pv3Result->x = pv3Arg1->x * pv3Arg2->x;
	pv3Result->y = pv3Arg1->y * pv3Arg2->y;
	pv3Result->z = pv3Arg1->z * pv3Arg2->z;
}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_DIV_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1,VECTOR3* pv3Arg2)
{
	__asm
	{
		mov			edi,dword ptr[pv3Arg1]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2

	
		mov			edi,dword ptr[pv3Arg2]
			
		; LOAD VECTOR
		movlps		xmm1,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm1,xmm2
	
		
		mov			edi,dword ptr[pv3Result]
		divps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_DIV_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1,VECTOR3* pv3Arg2)
{
	pv3Result->x = pv3Arg1->x / pv3Arg2->x;
	pv3Result->y = pv3Arg1->y / pv3Arg2->y;
	pv3Result->z = pv3Arg1->z / pv3Arg2->z;
}
#endif
#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_ADDEQU_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1)
{
	__asm
	{
		mov			edi,dword ptr[pv3Arg1]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2

		mov			edi,dword ptr[pv3Result]

		; LOAD VECTOR
		movlps		xmm1,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm1,xmm2

		addps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_ADDEQU_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1)
{
	pv3Result->x += pv3Arg1->x;
	pv3Result->y += pv3Arg1->y;
	pv3Result->z += pv3Arg1->z;
}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_SUBEQU_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1)
{
	__asm
	{
		mov			edi,dword ptr[pv3Arg1]

		; LOAD VECTOR
		movlps		xmm1,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm1,xmm2

		mov			edi,dword ptr[pv3Result]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2
		
		subps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_SUBEQU_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1)
{
	pv3Result->x -= pv3Arg1->x;
	pv3Result->y -= pv3Arg1->y;
	pv3Result->z -= pv3Arg1->z;
}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_MULEQU_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1)
{
	__asm
	{
		mov			edi,dword ptr[pv3Arg1]

		; LOAD VECTOR
		movlps		xmm1,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm1,xmm2

		mov			edi,dword ptr[pv3Result]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2
	
		mulps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_MULEQU_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1)
{
	pv3Result->x *= pv3Arg1->x;
	pv3Result->y *= pv3Arg1->y;
	pv3Result->z *= pv3Arg1->z;
}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_DIVEQU_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1)
{
	__asm
	{
		mov			edi,dword ptr[pv3Arg1]

		; LOAD VECTOR
		movlps		xmm1,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm1,xmm2

		mov			edi,dword ptr[pv3Result]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2
		
		
		
		divps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_DIVEQU_VECTOR3(VECTOR3* pv3Result,VECTOR3* pv3Arg1)
{
	pv3Result->x /= pv3Arg1->x;
	pv3Result->y /= pv3Arg1->y;
	pv3Result->z /= pv3Arg1->z;
}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_ADDEQU_FLOAT(VECTOR3* pv3Result,float fVal)
{
	__asm
	{
		mov			edi,dword ptr[pv3Result]
		movss		xmm1,dword ptr[fVal]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2

		shufps		xmm1,xmm1,0
		addps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_ADDEQU_FLOAT(VECTOR3* pv3Result,float fVal)
{
	pv3Result->x += fVal;
	pv3Result->y += fVal;
	pv3Result->z += fVal;
}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_SUBEQU_FLOAT(VECTOR3* pv3Result,float fVal)
{	
	__asm
	{
		mov			edi,dword ptr[pv3Result]
		movss		xmm1,dword ptr[fVal]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2

		shufps		xmm1,xmm1,0
		subps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_SUBEQU_FLOAT(VECTOR3* pv3Result,float fVal)
{
	pv3Result->x -= fVal;
	pv3Result->y -= fVal;
	pv3Result->z -= fVal;
}
#endif


#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_MULEQU_FLOAT(VECTOR3* pv3Result,float fVal)
{
	__asm
	{
		mov			edi,dword ptr[pv3Result]
		movss		xmm1,dword ptr[fVal]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2

		shufps		xmm1,xmm1,0
		mulps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_MULEQU_FLOAT(VECTOR3* pv3Result,float fVal)
{
	pv3Result->x *= fVal;
	pv3Result->y *= fVal;
	pv3Result->z *= fVal;
}
#endif


#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_DIVEQU_FLOAT(VECTOR3* pv3Result,float fVal)
{
	__asm
	{
		mov			edi,dword ptr[pv3Result]
		movss		xmm1,dword ptr[fVal]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2
		
		shufps		xmm1,xmm1,0
		divps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_DIVEQU_FLOAT(VECTOR3* pv3Result,float fVal)
{
	pv3Result->x /= fVal;
	pv3Result->y /= fVal;
	pv3Result->z /= fVal;
}
#endif


#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_ADD_FLOAT(VECTOR3* pv3Result,VECTOR3* pv3Arg1,float fVal)
{	
	__asm
	{
		mov			edi,dword ptr[pv3Arg1]
		movss		xmm1,dword ptr[fVal]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2

		
		shufps		xmm1,xmm1,0
		mov			edi,dword ptr[pv3Result]
		addps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_ADD_FLOAT(VECTOR3* pv3Result,VECTOR3* pv3Arg1,float fVal)
{
	pv3Result->x = pv3Arg1->x + fVal;
	pv3Result->y = pv3Arg1->y + fVal;
	pv3Result->z = pv3Arg1->z + fVal;
}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_SUB_FLOAT(VECTOR3* pv3Result,VECTOR3* pv3Arg1,float fVal)
{
	__asm
	{
		mov			edi,dword ptr[pv3Arg1]
		movss		xmm1,dword ptr[fVal]
		
		
		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2

		
		shufps		xmm1,xmm1,0
		mov			edi,dword ptr[pv3Result]
		subps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_SUB_FLOAT(VECTOR3* pv3Result,VECTOR3* pv3Arg1,float fVal)
{
	pv3Result->x = pv3Arg1->x - fVal;
	pv3Result->y = pv3Arg1->y - fVal;
	pv3Result->z = pv3Arg1->z - fVal;
}
#endif


#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_MUL_FLOAT(VECTOR3* pv3Result,VECTOR3* pv3Arg1,float fVal)
{	
	__asm
	{
		mov			edi,dword ptr[pv3Arg1]
		movss		xmm1,dword ptr[fVal]

		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2


		shufps		xmm1,xmm1,0
		mov			edi,dword ptr[pv3Result]
		mulps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_MUL_FLOAT(VECTOR3* pv3Result,VECTOR3* pv3Arg1,float fVal)
{
	pv3Result->x = pv3Arg1->x * fVal;
	pv3Result->y = pv3Arg1->y * fVal;
	pv3Result->z = pv3Arg1->z * fVal;
	
}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall VECTOR3_DIV_FLOAT(VECTOR3* pv3Result,VECTOR3* pv3Arg1,float fVal)
{	
	__asm
	{
		mov			edi,dword ptr[pv3Arg1]
		movss		xmm1,dword ptr[fVal]
		
		; LOAD VECTOR
		movlps		xmm0,[edi]
		movss		xmm2,[edi+8]
		movlhps		xmm0,xmm2

		shufps		xmm1,xmm1,0
		mov			edi,dword ptr[pv3Result]
		divps		xmm0,xmm1
		movhlps		xmm1,xmm0					; z
		movlps		[edi],xmm0					; write xy
		movss		[edi+8],xmm1				; write z
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall VECTOR3_DIV_FLOAT(VECTOR3* pv3Result,VECTOR3* pv3Arg1,float fVal)
{
	pv3Result->x = pv3Arg1->x / fVal;
	pv3Result->y = pv3Arg1->y / fVal;
	pv3Result->z = pv3Arg1->z / fVal;
}
#endif


#ifdef	_USE_SSE
GLOBAL_FUNC_DLL float __stdcall Sin(float x)
{
	int	dv = (int)(x / PI);
	if (abs(dv) >= 1)
	{
		x -= (PI_MUL_2*(float)dv);
	}
	
	float temp[4] = {0.008333f,0.166666f,1.0f,1.0f};
	
	float result;
	__asm
	{
		
		movss	xmm1,x
		
		movups	xmm0,temp		; xmm0 = 1		1		1/6		1/120
		shufps	xmm1,xmm1,0		; xmm1 = x		x		x		x
		
		movss	xmm4,xmm1		; xmm4 = ?		?		?		x
		movaps	xmm3,xmm1		; xmm3 = x		x		x		x
		mulss	xmm3,xmm1		; xmm3 = x		x		x		x^2
		mulps	xmm1,xmm3		; xmm1 = x^2	x^2		x^2		x^3
		mulps	xmm1,xmm3		; xmm1 = x^3	x^3		x^3		x^5
		
		mulps	xmm1,xmm0		; xmm1 = x^3	x^3		x^3/6	x^5/120
		addss	xmm4,xmm1		; xmm4 = ?		?		x^3/6	x^5/120+x
		shufps	xmm1,xmm1,85	; xmm1 = ?		?		?		x^3/6 
		subss	xmm4,xmm1		; xmm4 = ?		?		?		result

		movss	result,xmm4
	}
	return result;
}

GLOBAL_FUNC_DLL float __stdcall Cos(float x)
{
	int	dv = (int)(x / PI);
	if (abs(dv) >= 1)
	{
		x -= (PI_MUL_2*(float)dv);
	}
	
	float temp[4] = {0.001388f,0.041666f,0.5f,1.0f};
	float result;
	
	__asm
	{
		movss	xmm1,x			; load x
		
		movups	xmm0,temp		; xmm0 = 1			1/2			1/24		1/720

		shufps	xmm1,xmm0,240	; xmm1 = 1			1			x			x
		movaps	xmm4,xmm1		; xmm4 = 1			1			x			x
		mulps	xmm1,xmm4		; xmm1 = 1			1			x^2			x^2
		movaps	xmm5,xmm1		; xmm5 = 1			1			x^2			x^2
		shufps	xmm1,xmm1,128	; xmm1 = 1			x^2			x^2			x^2
		mulps	xmm5,xmm1		; xmm5 = 1			x^2			x^4			x^4
		mulss	xmm5,xmm1		; xmm5 = 1			x^2			x^4			x^6
		mulps	xmm5,xmm0		; xmm5 = 1			x^2/2		x^4/24		x^6/720
		movhlps	xmm6,xmm5		; xmm6 = ?			?			1			x^2/2
		addps	xmm5,xmm6		; xmm5 = ?			?			1+x^4/24	x^2/2+x^6/720
		movlhps	xmm3,xmm5		; xmm3 = 1+x^4/24	x^2/2+x^6/720	?		?
		shufps	xmm3,xmm3,3		; xmm3 = ?			?			?			1+x^4/24
		subss	xmm3,xmm5
		movss	result,xmm3
	}
	return result;
}


GLOBAL_FUNC_DLL float __stdcall ASin(float x)
{
	float temp[4] = {0.075f,0.166666f,1.0f,1.0f};
	float result;
	
	__asm
	{
		movss	xmm1,x			; load x
		
		movups	xmm0,temp		; xmm0 = 1		1		1/6		3/40
		shufps	xmm1,xmm1,0		; xmm1 = x		x		x		x
		
		movss	xmm4,xmm1		; xmm4 = ?		?		?		x
		movaps	xmm3,xmm1		; xmm3 = x		x		x		x
		mulss	xmm3,xmm1		; xmm3 = x		x		x		x^2
		mulps	xmm1,xmm3		; xmm1 = x^2	x^2		x^2		x^3
		mulps	xmm1,xmm3		; xmm1 = x^3	x^3		x^3		x^5
		
		mulps	xmm1,xmm0		; xmm1 = x^3	x^3		x^3/6	x^5/120
		addss	xmm4,xmm1		; xmm4 = ?		?		x^3/6	x^5/120+x
		shufps	xmm1,xmm1,85	; xmm1 = ?		?		?		x^3/6 
		addss	xmm4,xmm1		; xmm4 = ?		?		?		result

		
		movss	result,xmm4

	}
	return result;
}
GLOBAL_FUNC_DLL float __stdcall ACos(float x)
{
	//float temp[4] = {120.0f,6.0f,1.0f,1.0f};

	float temp[4] = {0.075f,0.166666f,1.0f,PI_DIV_2};
	float result;
	
	__asm
	{
		movss	xmm1,x			; load x
		
		movups	xmm0,temp		; xmm0 = PI/2	1		1/6		3/40
		shufps	xmm1,xmm1,0		; xmm1 = x		x		x		x
		movaps	xmm3,xmm1;		; xmm3 = x		x		x		x
		movss	xmm4,xmm1		; xmm4 = ?		?		?		x
		mulss	xmm1,xmm4		; xmm1 = x		x		x		x^2
		mulps	xmm3,xmm1		; xmm3 = x^2	x^2 	x^2		x^3
		mulps	xmm3,xmm1		; xmm3 = x^3	x^3 	x^3		x^5
		mulps	xmm3,xmm0		; xmm3 = ?		?		x^3/6	x^5*3/40
		addss	xmm4,xmm3		; xmm4 = ?		?		?		x^5*3/40+x
		shufps	xmm3,xmm3,1		; xmm3 = ?		?		?		x^3*1/6 
		addss	xmm4,xmm3		; xmm4 = ?		?		?		x + x^5*3/40 + x^3*1/6

		shufps	xmm0,xmm0,3		; xmm0 = PI/2	PI/2	PI/2	PI/2
		subss	xmm0,xmm4		; xmm0 = ?		?		?		result

		movss	result,xmm0

	}
//	float result = g_PI_DIV_2 - ( x + x*x*x/6.0f + 9.0f/120.0f*x*x*x*x*x);
	return result;
}
#else
GLOBAL_FUNC_DLL float __stdcall Sin(float x)
{
	int	dv = (int)(x / PI);
	if (abs(dv) >= 1)
	{
		x -= (PI_MUL_2*(float)dv);
	}

	float result = x - (x*x*x /6.0f) + (x*x*x*x*x / 120.0f);// - (x*x*x*x*x*x*x / 5040.0f);
	return result;
}
GLOBAL_FUNC_DLL float __stdcall Cos(float x)
{
	int	dv = (int)(x / PI);

	if (abs(dv) >= 1)
	{
		x -= (PI_MUL_2*(float)dv);
	}
	float result = 1.0f - (x*x/2.0f) + (x*x*x*x/24.0f) - (x*x*x*x*x*x/720.0f);
	return result;
}

GLOBAL_FUNC_DLL float __stdcall ASin(float x)
{
	float result = x + x*x*x/6.0f + 3.0f/40.0f*x*x*x*x*x;
	return result;
}

GLOBAL_FUNC_DLL float __stdcall ACos(float x)
{
	float result = PI_DIV_2 - x - x*x*x/6.0f - 9.0f/120.0f*x*x*x*x*x;
	return result;
}
#endif



#ifdef _USE_SSE
GLOBAL_FUNC_DLL float __stdcall DotProduct(VECTOR3* pv3_0,VECTOR3* pv3_1)
{
	float	result;

	__asm
	{
		mov		esi,dword ptr[pv3_0]
		mov		edi,dword ptr[pv3_1]
		
//		movups	xmm0,[esi]
		; LOAD VECTOR
		movlps	xmm0,[esi]			
		movss	xmm2,[esi+8]
		movlhps	xmm0,xmm2

//		movups	xmm1,[edi]
		; LOAD VECTOR
		movlps	xmm1,[edi]			
		movss	xmm2,[edi+8]
		movlhps	xmm1,xmm2
		
		mulps	xmm0,xmm1
		
		movhlps xmm3,xmm0	
		addss	xmm3,xmm0	// xmm[0-31] = x+z

		movaps	xmm2,xmm0
		shufps	xmm2,xmm2,1	// xmm [0-31] = y
		
		
		addss	xmm3,xmm2
	
		movss	[result],xmm3	; return value 
	}

	return result;
}

#else
GLOBAL_FUNC_DLL float __stdcall DotProduct(VECTOR3* pv3_0,VECTOR3* pv3_1)
{
	float result;

	result = pv3_0->x * pv3_1->x + pv3_0->y * pv3_1->y + pv3_0->z * pv3_1->z;


	return result;
}
#endif


#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall TransformVector3_VPTR1(VECTOR3* pv3SrcDest, MATRIX4* pMat,DWORD dwNum)
{
//	RDTSC_BEGIN

	__asm 
	{
		mov		ecx,dword ptr[dwNum]	; number of vertices
		mov  	esi, pv3SrcDest			; src vertices entry
		mov		ebx, pMat				; matrix

		movups	xmm4,[ebx]		; load matrix 1 line
		movups	xmm5,[ebx+16]	; load matrix 2 line
		movups	xmm6,[ebx+32]	; load matrix 3 line
		movups	xmm7,[ebx+48]	; load matrix 4 line

lb_loop:
//		movups	xmm0,[esi]		; load vector
		; LOAD VECTOR
		movlps	xmm0,[esi]			
		movss	xmm1,[esi+8]
		movlhps	xmm0,xmm1
		
		movaps	xmm1,xmm0		; src vector
		shufps	xmm1,xmm1,0		; x x x x
		mulps	xmm1,xmm4		; 1 line ok


		movaps	xmm2,xmm0		; src vector
		shufps	xmm2,xmm2,85	; y y y y
		mulps	xmm2,xmm5		; 2 line ok

		addps	xmm1,xmm2		

		movaps	xmm2,xmm0		; src vector
		shufps	xmm2,xmm2,170	; z z z z 
		mulps	xmm2,xmm6		; 3 line ok
		
		addps	xmm1,xmm2
		addps	xmm1,xmm7

		; xmm = ?	|	z	|	y	|	x
		movhlps		xmm0,xmm1		; z

		movlps	[esi],xmm1			; write x,y
		movss	[esi+8],xmm0		; write z

		add		esi,VECTOR3_SIZE
		loop		lb_loop
	}
//	RDTSC_END
}
GLOBAL_FUNC_DLL void __stdcall TransformNSizeVector3_VPTR1(char* pv3SrcDest,DWORD dwSize,MATRIX4* pMat,DWORD dwNum)
{
		__asm 
	{
		mov		ecx,dword ptr[dwNum]	; number of vertices
		mov  	esi, pv3SrcDest			; src vertices entry
		mov		ebx, pMat				; matrix

		movups	xmm4,[ebx]		; load matrix 1 line
		movups	xmm5,[ebx+16]	; load matrix 2 line
		movups	xmm6,[ebx+32]	; load matrix 3 line
		movups	xmm7,[ebx+48]	; load matrix 4 line

lb_loop:
//		movups	xmm0,[esi]		; load vector
		; LOAD VECTOR
		movlps	xmm0,[esi]			
		movss	xmm1,[esi+8]
		movlhps	xmm0,xmm1

		
		movaps	xmm1,xmm0		; src vector
		shufps	xmm1,xmm1,0		; x x x x
		mulps	xmm1,xmm4		; 1 line ok


		movaps	xmm2,xmm0		; src vector
		shufps	xmm2,xmm2,85	; y y y y
		mulps	xmm2,xmm5		; 2 line ok

		addps	xmm1,xmm2		

		movaps	xmm2,xmm0		; src vector
		shufps	xmm2,xmm2,170	; z z z z 
		mulps	xmm2,xmm6		; 3 line ok
		
		addps	xmm1,xmm2
		addps	xmm1,xmm7

		; xmm = ?	|	z	|	y	|	x
		movhlps		xmm0,xmm1		; z

		movlps	[esi],xmm1			; write x,y
		movss	[esi+8],xmm0		; write z

		add		esi,dword ptr[dwSize]
		loop		lb_loop
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall TransformVector3_VPTR1(VECTOR3* pv3SrcDest, MATRIX4* pMat,DWORD dwNum)
{
	VECTOR3	v3;
	
//	RDTSC_BEGIN
		
	for (DWORD i=0; i<dwNum; i++)
	{
		v3.x = pv3SrcDest->x*pMat->_11 + pv3SrcDest->y*pMat->_21 + pv3SrcDest->z*pMat->_31 + pMat->_41;
		v3.y = pv3SrcDest->x*pMat->_12 + pv3SrcDest->y*pMat->_22 + pv3SrcDest->z*pMat->_32 + pMat->_42;
		v3.z = pv3SrcDest->x*pMat->_13 + pv3SrcDest->y*pMat->_23 + pv3SrcDest->z*pMat->_33 + pMat->_43;
		
		*pv3SrcDest = v3;

		pv3SrcDest++;
	}

//	RDTSC_END
}
GLOBAL_FUNC_DLL void __stdcall TransformNSizeVector3_VPTR1(char* pv3SrcDest,DWORD dwSize,MATRIX4* pMat,DWORD dwNum)
{
	VECTOR3	v3;
	
	for (DWORD i=0; i<dwNum; i++)
	{
		v3.x = ((VECTOR3*)pv3SrcDest)->x*pMat->_11 + ((VECTOR3*)pv3SrcDest)->y*pMat->_21 + ((VECTOR3*)pv3SrcDest)->z*pMat->_31 + pMat->_41;
		v3.y = ((VECTOR3*)pv3SrcDest)->x*pMat->_12 + ((VECTOR3*)pv3SrcDest)->y*pMat->_22 + ((VECTOR3*)pv3SrcDest)->z*pMat->_32 + pMat->_42;
		v3.z = ((VECTOR3*)pv3SrcDest)->x*pMat->_13 + ((VECTOR3*)pv3SrcDest)->y*pMat->_23 + ((VECTOR3*)pv3SrcDest)->z*pMat->_33 + pMat->_43;
		
		*(VECTOR3*)pv3SrcDest = v3;

		pv3SrcDest += dwSize;
	}

}
#endif

GLOBAL_FUNC_DLL void __stdcall TransformV3TOV4(VECTOR4* pv3Dest,VECTOR3* pv3Src, MATRIX4* pMat,DWORD dwNum)
{
	
	for (DWORD i=0; i<dwNum; i++)
	{
		pv3Dest->x = pv3Src->x*pMat->_11 + pv3Src->y*pMat->_21 + pv3Src->z*pMat->_31 + pMat->_41;
		pv3Dest->y = pv3Src->x*pMat->_12 + pv3Src->y*pMat->_22 + pv3Src->z*pMat->_32 + pMat->_42;
		pv3Dest->z = pv3Src->x*pMat->_13 + pv3Src->y*pMat->_23 + pv3Src->z*pMat->_33 + pMat->_43;
		pv3Dest->w = pv3Src->x*pMat->_14 + pv3Src->y*pMat->_24 + pv3Src->z*pMat->_34 + pMat->_44;
		
		pv3Src++;
		pv3Dest++;
	}
}

#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall TransformVector3_VPTR2(VECTOR3* pv3Dest,VECTOR3* pv3Src, MATRIX4* pMat,DWORD dwNum)
{
//	VECTOR4	v4;
	__asm 
	{
		mov		ecx,dword ptr[dwNum]	; number of vertices
		mov  	esi, pv3Src				; src vertices entry
		mov  	edi, pv3Dest			; dest vertices entry
		mov		ebx, pMat				; matrix

		movups	xmm4,[ebx]		; load matrix 1 line
		movups	xmm5,[ebx+16]	; load matrix 2 line
		movups	xmm6,[ebx+32]	; load matrix 3 line
		movups	xmm7,[ebx+48]	; load matrix 4 line

lb_loop:
		// LOAD VECTOR
		movlps	xmm0,[esi]
		movss	xmm1,[esi+8]
		movlhps	xmm0,xmm1
		
		movaps	xmm1,xmm0		; src vector
		shufps	xmm1,xmm1,0		; x x x x
		mulps	xmm1,xmm4		; 1 line ok


		movaps	xmm2,xmm0		; src vector
		shufps	xmm2,xmm2,85	; y y y y
		mulps	xmm2,xmm5		; 2 line ok

		addps	xmm1,xmm2		

		movaps	xmm2,xmm0		; src vector
		shufps	xmm2,xmm2,170	; z z z z 
		mulps	xmm2,xmm6		; 3 line ok
		
		addps	xmm1,xmm2
		addps	xmm1,xmm7

		; xmm = ?	|	z	|	y	|	x
		movhlps		xmm0,xmm1		; z

		movlps	[edi],xmm1			; write x,y
		movss	[edi+8],xmm0		; write z

		add		edi,VECTOR3_SIZE
		add		esi,VECTOR3_SIZE

		loop		lb_loop
	}
}

GLOBAL_FUNC_DLL void __stdcall TransformVector4_VPTR2(VECTOR4* pv3Dest,VECTOR4* pv3Src, MATRIX4* pMat,DWORD dwNum)
{
	__asm 
	{
		mov		ecx,dword ptr[dwNum]	; number of vertices
		mov  	esi, pv3Src				; src vertices entry
		mov  	edi, pv3Dest			; dest vertices entry
		mov		ebx, pMat				; matrix

		movups	xmm4,[ebx]		; load matrix 1 line
		movups	xmm5,[ebx+16]	; load matrix 2 line
		movups	xmm6,[ebx+32]	; load matrix 3 line
		movups	xmm7,[ebx+48]	; load matrix 4 line

lb_loop:
		movups	xmm0,[esi]		; load vector
		
		movaps	xmm1,xmm0		; src vector
		shufps	xmm1,xmm1,0		; x x x x
		mulps	xmm1,xmm4		; 1 line ok


		movaps	xmm2,xmm0		; src vector
		shufps	xmm2,xmm2,85	; y y y y
		mulps	xmm2,xmm5		; 2 line ok

		addps	xmm1,xmm2		

		movaps	xmm2,xmm0		; src vector
		shufps	xmm2,xmm2,170	; z z z z 
		mulps	xmm2,xmm6		; 3 line ok
		
		addps	xmm1,xmm2

		movaps	xmm2,xmm0		; src vector
		shufps	xmm2,xmm2,255	; w w w w 
		mulps	xmm2,xmm7		; 4 line ok


		addps	xmm1,xmm2

		movups	[edi],xmm1

		add		edi,VECTOR4_SIZE
		add		esi,VECTOR4_SIZE

		loop		lb_loop
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall TransformVector3_VPTR2(VECTOR3* pv3Dest,VECTOR3* pv3Src, MATRIX4* pMat,DWORD dwNum)
{
	
	for (DWORD i=0; i<dwNum; i++)
	{
		pv3Dest->x = pv3Src->x*pMat->_11 + pv3Src->y*pMat->_21 + pv3Src->z*pMat->_31 + pMat->_41;
		pv3Dest->y = pv3Src->x*pMat->_12 + pv3Src->y*pMat->_22 + pv3Src->z*pMat->_32 + pMat->_42;
		pv3Dest->z = pv3Src->x*pMat->_13 + pv3Src->y*pMat->_23 + pv3Src->z*pMat->_33 + pMat->_43;
		
		pv3Src++;
		pv3Dest++;
	}
}

GLOBAL_FUNC_DLL void __stdcall TransformVector4_VPTR2(VECTOR4* pv3Dest,VECTOR4* pv3Src, MATRIX4* pMat,DWORD dwNum)
{
	
	for (DWORD i=0; i<dwNum; i++)
	{
		pv3Dest->x = pv3Src->x*pMat->_11 + pv3Src->y*pMat->_21 + pv3Src->z*pMat->_31 + pv3Src->w*pMat->_41;
		pv3Dest->y = pv3Src->x*pMat->_12 + pv3Src->y*pMat->_22 + pv3Src->z*pMat->_32 + pv3Src->w*pMat->_42;
		pv3Dest->z = pv3Src->x*pMat->_13 + pv3Src->y*pMat->_23 + pv3Src->z*pMat->_33 + pv3Src->w*pMat->_43;
		pv3Dest->w = pv3Src->x*pMat->_14 + pv3Src->y*pMat->_24 + pv3Src->z*pMat->_34 + pv3Src->w*pMat->_44;
		
		pv3Src++;
		pv3Dest++;
	}
}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall CrossProduct(VECTOR3* r,VECTOR3* u,VECTOR3* v)
{
	
	__asm
	{
		mov			esi,dword ptr[u]
		mov			edi,dword ptr[v]

//		movups		xmm0,[esi]		;	?	|	u->z	|	u->y	|	u->x
//		movups		xmm1,[edi]		;	?	|	v->z	|	v->y	|	v->x

		; LOAD VECTOR
		movlps	xmm0,[esi]			
		movss	xmm2,[esi+8]
		movlhps	xmm0,xmm2

		

		; LOAD VECTOR
		movlps	xmm1,[edi]			
		movss	xmm2,[edi+8]
		movlhps	xmm1,xmm2

		

		
		xorps		xmm2,xmm2		; 0으로 초기화
		movaps		xmm6,xmm0		; backup	xmm0

		; x,y성분
		; 	r->x = u->y*v->z - u->z*v->y;
		;	r->y = u->z*v->x - u->x*v->z;

		shufps		xmm0,xmm0,41	;	u->x	|	u->z	|	u->z	|	u->y
		shufps		xmm1,xmm1,146	;	v->z	|	v->y	|	v->x	|	v->z

		
		mulps		xmm0,xmm1		;	u->x*v->z |	u->z*v->y | u->z*v->x |	u->y*v->z	
		movhlps		xmm2,xmm0		;			  |			  |	u->x*v->z |	u->z*v->y
		subps		xmm0,xmm2		;		?	  |		?	  |	u->z*v->x - u->x*v->z |	u->y*v->z - u->z*v->y

		; z성분
		;	r->z = u->x*v->y - u->y*v->x;
									
		shufps		xmm6,xmm6,16	;			  |	u->y	  |			  |	u->x	
		shufps		xmm1,xmm1,18	;			  |	v->x	  |			  |	v->y	
		mulps		xmm6,xmm1		;			  |	u->y*v->x |			  | u->x*v->y 
		movhlps		xmm2,xmm6		;			  |			  | 		  | u->y*v->x
		subss		xmm6,xmm2		;			  |           |           | u->x*v->y - u->y*v->x				

		mov			edi,dword ptr[r]
		movlps		[edi],xmm0		; write x,y
		movss		[edi+8],xmm6
	}
}
#else*/
GLOBAL_FUNC_DLL void __stdcall CrossProduct(VECTOR3* r,VECTOR3* u,VECTOR3* v)
{
	VECTOR3	result;

	result.x = u->y*v->z - u->z*v->y;
	result.y = u->z*v->x - u->x*v->z;
	result.z = u->x*v->y - u->y*v->x;

	*r = result;

}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall Normalize(VECTOR3* OUT vn,VECTOR3* IN v)
{
	__asm
	{
		mov				esi,v
		mov				edi,vn

		xorps			xmm1,xmm1
		
		; LOAD VECTOR
		movlps		xmm0,[esi]
		movss		xmm2,[esi+8]
		movlhps		xmm0,xmm2

		movaps			xmm2,xmm0			; backup 
	
		mulps			xmm0,xmm0			;	?	|	zz	|	yy	|	xx
		movhlps			xmm1,xmm0			;	?	|	?	|	?	|	zz
		addss			xmm1,xmm0			;	?	|	?	|	?	| xx+zz
		shufps			xmm0,xmm0,1			;	?	|	?	|	?	|	yy
		addss			xmm1,xmm0			;	?	|	?	|	?	| xx+yy+zz

		sqrtss			xmm0,xmm1
		
		shufps			xmm0,xmm0,0			; distance
		divps			xmm2,xmm0

		movhlps			xmm0,xmm2			; z
		
		movlps			[edi],xmm2
		movss			[edi+8],xmm0
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall Normalize(VECTOR3* OUT vn,VECTOR3* IN v)
{
	float		sqt = (float)sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
	if (sqt == 0.0f)
	{
		*vn = *v;
		return;
	}

	vn->x = v->x /sqt;
	vn->y = v->y /sqt;
	vn->z = v->z /sqt;

}
#endif

#ifdef _USE_SSE

GLOBAL_FUNC_DLL float __stdcall CalcDistance(VECTOR3* pv3Start,VECTOR3* pv3End)
{
	
	float	result;

	__asm
	{
		mov				esi,dword ptr[pv3Start]
		mov				edi,dword ptr[pv3End]

		; LOAD VECTOR
		movlps		xmm0,[esi]		; start
		movss		xmm2,[esi+8]
		movlhps		xmm0,xmm2

		; LOAD VECTOR
		movlps		xmm1,[edi]		; end
		movss		xmm2,[edi+8]
		movlhps		xmm1,xmm2

//		movups			xmm0,[esi]			; start
//		movups			xmm1,[edi]			; end
		
		subps			xmm0,xmm1
		mulps			xmm0,xmm0			;	?	|	zz	|	yy	|	xx
		movhlps			xmm1,xmm0			;	?	|	?	|	?	|	zz
		addss			xmm1,xmm0			;	?	|	?	|	?	| xx+zz
		shufps			xmm0,xmm0,1			;	?	|	?	|	?	|	yy
		addss			xmm1,xmm0			;	?	|	?	|	?	| xx+yy+zz
		sqrtss			xmm0,xmm1
		
		movss			[result],xmm0
	}
	return result;
}
#else
GLOBAL_FUNC_DLL float __stdcall CalcDistance(VECTOR3* pv3Start,VECTOR3* pv3End)
{
	float	result;

	VECTOR3 v3dist;
	//= *pv3End - *pv3Start;
	VECTOR3_SUB_VECTOR3(&v3dist,pv3End,pv3Start);
	result = (float)sqrt(v3dist.x * v3dist.x + v3dist.y * v3dist.y + v3dist.z * v3dist.z);

	return result;

}
#endif


#ifdef _USE_SSE
GLOBAL_FUNC_DLL void __stdcall MatrixMultiply2(MATRIX4* pResult, MATRIX4* mat1, MATRIX4* mat2)
{
	__asm 
	{
		mov  	edi, [pResult]		; result
		mov  	esi, [mat2]		; matrix 2
		
		movups	xmm4,[esi]		; matrix line 1
		movups	xmm5,[esi+16]	; matrix line 2
		movups	xmm6,[esi+32]	; matrix line 3
		movups	xmm7,[esi+48]	; matrix line 4


		mov		esi, [mat1]		; matrix 1
		mov		ecx,4
lp:
		movups	xmm0,[esi]		; load matrix (n) line

		
		movaps	xmm1,xmm0		; src vector
		shufps	xmm1,xmm1,0		; x x x x
		mulps	xmm1,xmm4		; 1 line ok


		movaps	xmm2,xmm0		; src vector
		shufps	xmm2,xmm2,85	; y y y y
		mulps	xmm2,xmm5		; 2 line ok


		addps	xmm1,xmm2		


		movaps	xmm2,xmm0		; src vector
		shufps	xmm2,xmm2,170	; z z z z 
		mulps	xmm2,xmm6		; 3 line ok
		
		addps	xmm1,xmm2

		movaps	xmm2,xmm0		; src vector
		shufps	xmm2,xmm2,255	; w w w w 
		mulps	xmm2,xmm7		; 4 line ok

		addps	xmm1,xmm2

		movups	[edi],xmm1
		
		add		esi,16
		add		edi,16

		loop lp
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall MatrixMultiply2(MATRIX4* pResult, MATRIX4* mat1, MATRIX4* mat2)
{
	MATRIX4 result;

	result._11 =
		(mat1->_11 * mat2->_11) + (mat1->_12 * mat2->_21) + (mat1->_13 * mat2->_31) + (mat1->_14 * mat2->_41);
	result._12 = 
		(mat1->_11 * mat2->_12) + (mat1->_12 * mat2->_22) + (mat1->_13 * mat2->_32) + (mat1->_14 * mat2->_42);
	result._13 = 
		(mat1->_11 * mat2->_13) + (mat1->_12 * mat2->_23) + (mat1->_13 * mat2->_33) + (mat1->_14 * mat2->_43);
	result._14 = 
		(mat1->_11 * mat2->_14) + (mat1->_12 * mat2->_24) + (mat1->_13 * mat2->_34) + (mat1->_14 * mat2->_44);

	result._21 =
		(mat1->_21 * mat2->_11) + (mat1->_22 * mat2->_21) + (mat1->_23 * mat2->_31) + (mat1->_24 * mat2->_41);
	result._22 = 
		(mat1->_21 * mat2->_12) + (mat1->_22 * mat2->_22) + (mat1->_23 * mat2->_32) + (mat1->_24 * mat2->_42);
	result._23 = 
		(mat1->_21 * mat2->_13) + (mat1->_22 * mat2->_23) + (mat1->_23 * mat2->_33) + (mat1->_24 * mat2->_43);
	result._24 = 
		(mat1->_21 * mat2->_14) + (mat1->_22 * mat2->_24) + (mat1->_23 * mat2->_34) + (mat1->_24 * mat2->_44);

	result._31 =
		(mat1->_31 * mat2->_11) + (mat1->_32 * mat2->_21) + (mat1->_33 * mat2->_31) + (mat1->_34 * mat2->_41);
	result._32 = 
		(mat1->_31 * mat2->_12) + (mat1->_32 * mat2->_22) + (mat1->_33 * mat2->_32) + (mat1->_34 * mat2->_42);
	result._33 = 
		(mat1->_31 * mat2->_13) + (mat1->_32 * mat2->_23) + (mat1->_33 * mat2->_33) + (mat1->_34 * mat2->_43);
	result._34 = 
		(mat1->_31 * mat2->_14) + (mat1->_32 * mat2->_24) + (mat1->_33 * mat2->_34) + (mat1->_34 * mat2->_44);
	
	result._41 =
		(mat1->_41 * mat2->_11) + (mat1->_42 * mat2->_21) + (mat1->_43 * mat2->_31) + (mat1->_44 * mat2->_41);
	result._42 = 
		(mat1->_41 * mat2->_12) + (mat1->_42 * mat2->_22) + (mat1->_43 * mat2->_32) + (mat1->_44 * mat2->_42);
	result._43 = 
		(mat1->_41 * mat2->_13) + (mat1->_42 * mat2->_23) + (mat1->_43 * mat2->_33) + (mat1->_44 * mat2->_43);
	result._44 = 
		(mat1->_41 * mat2->_14) + (mat1->_42 * mat2->_24) + (mat1->_43 * mat2->_34) + (mat1->_44 * mat2->_44);

	*pResult = result;
}
#endif

GLOBAL_FUNC_DLL float __stdcall CalcCosAngle(VECTOR3* vec1, VECTOR3* vec2)
{
	float cosang = (float)
		( (vec1->x*vec2->x + vec1->y*vec2->y + vec1->z*vec2->z) / 
		  ( (float)sqrt(vec1->x*vec1->x + vec1->y*vec1->y + vec1->z*vec1->z)*
		    (float)sqrt(vec2->x*vec2->x + vec2->y*vec2->y + vec2->z*vec2->z) )
		);
	return cosang;
}

GLOBAL_FUNC_DLL BOOL __stdcall IsCollisionSpherAndSphere(VECTOR3* pv3PointTarget,float fRsTarget,VECTOR3* pv3PointSrc,float fRsSrc)
{
	BOOL		bResult = FALSE;
/*
	VECTOR3		v;
//	= *pv3PointSrc - *pv3PointTarget;
	VECTOR3_SUB_VECTOR3(&v,pv3PointSrc,pv3PointTarget);

	float		distance;
	distance = (float)sqrt(v.x * v.x + v.y * v.y + v.z* v.z);
	if (distance > fRsTarget+fRsSrc)
		goto lb_return;

	bResult = TRUE;
lb_return:
*/
	float distance = CalcDistance(pv3PointTarget,pv3PointSrc);
	if (distance < fRsTarget + fRsSrc )
		bResult = TRUE;

	return bResult;
}




GLOBAL_FUNC_DLL BOOL __stdcall SetIdentityMatrix(MATRIX4* pMat)
{
    pMat->_12 = pMat->_13 = pMat->_14 = pMat->_21 = pMat->_23 = pMat->_24 = 0.0f;
    pMat->_31 = pMat->_32 = pMat->_34 = pMat->_41 = pMat->_42 = pMat->_43 = 0.0f;
    pMat->_11 = pMat->_22 = pMat->_33 = pMat->_44 = 1.0f;
	return TRUE;
}


GLOBAL_FUNC_DLL BOOL __stdcall SetInverseMatrix(MATRIX4* q, MATRIX4* a )

{
	BOOL		bResult = FALSE;
	FLOAT		fDetInv;

    if( fabs(a->_44 - 1.0f) > .001f)
		goto lb_return;

    if( fabs(a->_14) > .001f || fabs(a->_24) > .001f || fabs(a->_34) > .001f )
		goto lb_return;

    fDetInv = 1.0f / ( a->_11 * ( a->_22 * a->_33 - a->_23 * a->_32 ) -
                             a->_12 * ( a->_21 * a->_33 - a->_23 * a->_31 ) +
                             a->_13 * ( a->_21 * a->_32 - a->_22 * a->_31 ) );

    q->_11 =  fDetInv * ( a->_22 * a->_33 - a->_23 * a->_32 );
    q->_12 = -fDetInv * ( a->_12 * a->_33 - a->_13 * a->_32 );
    q->_13 =  fDetInv * ( a->_12 * a->_23 - a->_13 * a->_22 );
    q->_14 = 0.0f;

    q->_21 = -fDetInv * ( a->_21 * a->_33 - a->_23 * a->_31 );
    q->_22 =  fDetInv * ( a->_11 * a->_33 - a->_13 * a->_31 );
    q->_23 = -fDetInv * ( a->_11 * a->_23 - a->_13 * a->_21 );
    q->_24 = 0.0f;

    q->_31 =  fDetInv * ( a->_21 * a->_32 - a->_22 * a->_31 );
    q->_32 = -fDetInv * ( a->_11 * a->_32 - a->_12 * a->_31 );
    q->_33 =  fDetInv * ( a->_11 * a->_22 - a->_12 * a->_21 );
    q->_34 = 0.0f;

    q->_41 = -( a->_41 * q->_11 + a->_42 * q->_21 + a->_43 * q->_31 );
    q->_42 = -( a->_41 * q->_12 + a->_42 * q->_22 + a->_43 * q->_32 );
    q->_43 = -( a->_41 * q->_13 + a->_42 * q->_23 + a->_43 * q->_33 );
    q->_44 = 1.0f;

	bResult = TRUE;
lb_return:
	return bResult;
}

GLOBAL_FUNC_DLL void __stdcall SetRotationMatrix(MATRIX4* mat, VECTOR3* vDir, FLOAT fRads )
{
    FLOAT     fCos = (FLOAT)cos( fRads );
    FLOAT     fSin = (FLOAT)sin( fRads );

	VECTOR3		v;

	v = *vDir;


    mat->_11 = ( v.x * v.x ) * ( 1.0f - fCos ) + fCos;
    mat->_12 = ( v.x * v.y ) * ( 1.0f - fCos ) - (v.z * fSin);
    mat->_13 = ( v.x * v.z ) * ( 1.0f - fCos ) + (v.y * fSin);

    mat->_21 = ( v.y * v.x ) * ( 1.0f - fCos ) + (v.z * fSin);
    mat->_22 = ( v.y * v.y ) * ( 1.0f - fCos ) + fCos ;
    mat->_23 = ( v.y * v.z ) * ( 1.0f - fCos ) - (v.x * fSin);

    mat->_31 = ( v.z * v.x ) * ( 1.0f - fCos ) - (v.y * fSin);
    mat->_32 = ( v.z * v.y ) * ( 1.0f - fCos ) + (v.x * fSin);
    mat->_33 = ( v.z * v.z ) * ( 1.0f - fCos ) + fCos;
    
    mat->_14 = mat->_24 = mat->_34 = 0.0f;
    mat->_41 = mat->_42 = mat->_43 = 0.0f;
    mat->_44 = 1.0f;
}
GLOBAL_FUNC_DLL void __stdcall MatrixMultiply3(MATRIX4* pResult,MATRIX4* mat1, MATRIX4* mat2, MATRIX4* mat3)
{
	MatrixMultiply2(pResult,mat1,mat2);
	MatrixMultiply2(pResult,pResult,mat3);

} 
GLOBAL_FUNC_DLL VOID __stdcall MatrixFromQuaternion(MATRIX4* mat, QUARTERNION* q)
{
    FLOAT xx = q->x*q->x; FLOAT yy = q->y*q->y; FLOAT zz = q->z*q->z;
    FLOAT xy = q->x*q->y; FLOAT xz = q->x*q->z; FLOAT yz = q->y*q->z;
    FLOAT wx = q->w*q->x; FLOAT wy = q->w*q->y; FLOAT wz = q->w*q->z;
    
    mat->_11 = 1 - 2 * ( yy + zz ); 
    mat->_12 =     2 * ( xy - wz );
    mat->_13 =     2 * ( xz + wy );

    mat->_21 =     2 * ( xy + wz );
    mat->_22 = 1 - 2 * ( xx + zz );
    mat->_23 =     2 * ( yz - wx );

    mat->_31 =     2 * ( xz - wy );
    mat->_32 =     2 * ( yz + wx );
    mat->_33 = 1 - 2 * ( xx + yy );

    mat->_14 = mat->_24 = mat->_34 = 0.0f;
    mat->_41 = mat->_42 = mat->_43 = 0.0f;
    mat->_44 = 1.0f;
}
GLOBAL_FUNC_DLL VOID __stdcall QuaternionSlerp(QUARTERNION* qQ,QUARTERNION* aQ,QUARTERNION* bQ,float fAlpha)
{
    FLOAT fScale1;
    FLOAT fScale2;

    // Compute dot product, aka cos(theta):
    FLOAT fCosTheta = aQ->x*bQ->x + aQ->y*bQ->y + aQ->z*bQ->z + aQ->w*bQ->w;

    if( fCosTheta < 0.0f )
    {
        // Flip start quaternion
		aQ->x = -aQ->x;
		aQ->y = -aQ->y;
		aQ->z = -aQ->z;
		aQ->w = -aQ->w;
		
		

        fCosTheta = -fCosTheta;
    }

    if( fCosTheta + 1.0f > 0.05f )
    {
        // If the quaternions are close, use linear interploation
        if( 1.0f - fCosTheta < 0.05f )
        {
            fScale1 = 1.0f - fAlpha;
            fScale2 = fAlpha;
        }
        else // Otherwise, do spherical interpolation
        {
            FLOAT fTheta    = (FLOAT)ACos( fCosTheta );
            FLOAT fSinTheta = (FLOAT)Sin( fTheta );
            
            fScale1 = (FLOAT)Sin( fTheta * (1.0f-fAlpha) ) / fSinTheta;
            fScale2 = (FLOAT)Sin( fTheta * fAlpha ) / fSinTheta;
        }
    }
    else
    {
		bQ->x = -aQ->y;
		bQ->y = aQ->x;
		bQ->z = -aQ->w;
		bQ->w = aQ->z;

        fScale1 = (FLOAT)Sin( PI_DIV_2 * (1.0f - fAlpha) );
        fScale2 = (FLOAT)Sin( PI_DIV_2 * fAlpha );
    }

	qQ->x = fScale1*aQ->x + fScale2*bQ->x;
	qQ->y = fScale1*aQ->y + fScale2*bQ->y;
	qQ->z = fScale1*aQ->z + fScale2*bQ->z;
	qQ->w = fScale1*aQ->w + fScale2*bQ->w;
}
GLOBAL_FUNC_DLL void __stdcall TranslateMatrix(MATRIX4* m, VECTOR3* vec)
{
    m->_12 = m->_13 = m->_14 = m->_21 = m->_23 = m->_24 = m->_31 = m->_32 = m->_34 = 0.0f;
    m->_11 = m->_22 = m->_33 = m->_44 = 1.0f;
	m->_41 = vec->x; 
	m->_42 = vec->y; 
	m->_43 = vec->z; 
}

GLOBAL_FUNC_DLL void __stdcall SetScaleMatrix(MATRIX4* m, VECTOR3* v )
{
    m->_12 = m->_13 = m->_14 = m->_21 = m->_23 = m->_24 = 0.0f;
    m->_31 = m->_32 = m->_34 = m->_41 = m->_42 = m->_43 = 0.0f;
    m->_11 = m->_22 = m->_33 = m->_44 = 1.0f;
	m->_11 = v->x; m->_22 = v->y; m->_33 = v->z;
}

#ifdef _USE_SSE
GLOBAL_FUNC_DLL float __stdcall VECTOR3Length(VECTOR3* pv3)
{
	float r;

	__asm
	{
		mov		esi,dword ptr[pv3]
		
		
		; LOAD VECTOR
		movlps		xmm0,[esi]		; start
		movss		xmm2,[esi+8]
		movlhps		xmm0,xmm2

		movaps	xmm1,xmm0
		
		mulps	xmm0,xmm1
		
		movhlps xmm3,xmm0	
		addps	xmm3,xmm0	// xmm[0-31] = x+z

		movaps	xmm2,xmm0
		shufps	xmm2,xmm2,1	// xmm [0-31] = y
		
		
		addps	xmm3,xmm2
		sqrtss	xmm3,xmm3
		movss	[r],xmm3

	}

	return (float)r;
}
#else
GLOBAL_FUNC_DLL float __stdcall VECTOR3Length(VECTOR3* pv3)
{
	float r = DotProduct(pv3,pv3);
	r = (float)sqrt( r);
	return (float)r;
}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL BOOL __stdcall CalcPlaneEquation(PLANE* pPlane,VECTOR3* pv3Tri)
{
//	VECTOR4	v4;
	__asm
	{
		mov			esi,dword ptr[pv3Tri]
		mov			edi,dword ptr[pPlane]
		movups		xmm5,[esi]							; 0
		movups		xmm0,[esi+VECTOR3_SIZE]				; 1

		; LOAD VECTOR
		movlps		xmm1,[esi+VECTOR3_SIZE+VECTOR3_SIZE]		; 2
		movss		xmm2,[esi+VECTOR3_SIZE+VECTOR3_SIZE+8]
		movlhps		xmm1,xmm2

//		movups		[v4],xmm1


//		movups		xmm1,[esi+VECTOR3_SIZE+VECTOR3_SIZE]; 2
//		movups		[v4],xmm1




		; cross product			r = u*v

		subps		xmm0,xmm5		; u
		subps		xmm1,xmm5		; v

		xorps		xmm2,xmm2		; 0으로 초기화
		movaps		xmm6,xmm0		; backup	xmm0

		; x,y성분
		; 	r->x = u->y*v->z - u->z*v->y;
		;	r->y = u->z*v->x - u->x*v->z;

		shufps		xmm0,xmm0,41	;	u->x	|	u->z	|	u->z	|	u->y
		shufps		xmm1,xmm1,146	;	v->z	|	v->y	|	v->x	|	v->z

		
		mulps		xmm0,xmm1		;	u->x*v->z |	u->z*v->y | u->z*v->x |	u->y*v->z	
		movhlps		xmm2,xmm0		;			  |			  |	u->x*v->z |	u->z*v->y
		subps		xmm0,xmm2		;		?	  |		?	  |	u->z*v->x - u->x*v->z |	u->y*v->z - u->z*v->y

		; z성분
		;	r->z = u->x*v->y - u->y*v->x;
									
		shufps		xmm6,xmm6,16	;			  |	u->y	  |			  |	u->x	
		shufps		xmm1,xmm1,18	;			  |	v->x	  |			  |	v->y	
		mulps		xmm6,xmm1		;			  |	u->y*v->x |			  | u->x*v->y 
		movhlps		xmm2,xmm6		;			  |			  | 		  | u->y*v->x
		subss		xmm6,xmm2		;			  |           |           | u->x*v->y - u->y*v->x	
		movlhps		xmm0,xmm6		; r

		; normalize r 
		xorps			xmm1,xmm1
		movaps			xmm2,xmm0			; backup 
	
		mulps			xmm0,xmm0			;	?	|	zz	|	yy	|	xx
		movhlps			xmm1,xmm0			;	?	|	?	|	?	|	zz
		addss			xmm1,xmm0			;	?	|	?	|	?	| xx+zz
		shufps			xmm0,xmm0,1			;	?	|	?	|	?	|	yy
		addss			xmm1,xmm0			;	?	|	?	|	?	| xx+yy+zz

		sqrtss			xmm0,xmm1
		
		shufps			xmm0,xmm0,0			; distance
		divps			xmm2,xmm0

		; 	pPlane->v3Up = r;
		;	VECTOR3		v3Up;
		;	float		D;
		;	float		fAreaSize;	

		movups			[edi],xmm2			; pPlane->v3Up


		;	pPlane->D = -(pv3Tri->x * r.x + pv3Tri->y * r.y + pv3Tri->z * r.z);
		mulps			xmm5,xmm2
		movhlps			xmm3,xmm5	
		addps			xmm3,xmm5	// xmm[0-31] = x+z

		shufps			xmm5,xmm5,1	// xmm [0-31] = y
		addss			xmm3,xmm5

		xorps			xmm0,xmm0
		subss			xmm0,xmm3

		movss			[edi+PLANE_D_OFFSET],xmm0
	}

	return TRUE;
}
#else
GLOBAL_FUNC_DLL BOOL __stdcall CalcPlaneEquation(PLANE* pPlane,VECTOR3* pv3Tri)
{
	VECTOR3 u,v,r;
	
	u.x = pv3Tri[1].x - pv3Tri[0].x;
	u.y = pv3Tri[1].y - pv3Tri[0].y;
	u.z = pv3Tri[1].z - pv3Tri[0].z;

	v.x = pv3Tri[2].x - pv3Tri[0].x;
	v.y = pv3Tri[2].y - pv3Tri[0].y;
	v.z = pv3Tri[2].z - pv3Tri[0].z;

	CrossProduct(&r,&u,&v);
	Normalize(&r,&r);
	pPlane->v3Up = r;
	pPlane->D = -(pv3Tri->x * r.x + pv3Tri->y * r.y + pv3Tri->z * r.z);

	return TRUE;
}
#endif

GLOBAL_FUNC_DLL BOOL __stdcall IsCollisionSphereAndPlane(VECTOR3* pv3Point,float fRs,PLANE* pPlane)
{	
	BOOL		bResult = FALSE;
	float D = (pv3Point->x * pPlane->v3Up.x + pv3Point->y * pPlane->v3Up.y + pv3Point->z * pPlane->v3Up.z + pPlane->D);
	
	if (D < 0)
		D *= -1.0f;
	
	if (D < fRs)
		bResult = TRUE;

	return bResult;
}
GLOBAL_FUNC_DLL BOOL __stdcall ComparePlane(PLANE* pTarget,PLANE* pSrc)
{
	BOOL	bResult = FALSE;
	float	min,cosang;

//	VECTOR3	r;
//	CrossProduct(&r,&pTarget->v3Up,&pSrc->v3Up);

#ifdef _DEBUG
	float len = VECTOR3Length(&pTarget->v3Up);
	if (len < 0.99f)
		__asm int 3

	if (len > 1.01f)
		__asm int 3
#endif

	cosang = DotProduct(&pTarget->v3Up,&pSrc->v3Up);
	if (cosang < 0.95f)
		goto lb_return;

	min = pTarget->D - pSrc->D;
	
	if (min < 0.0f)
		min *= -1.0f;

	if (min > 1.0f)
		goto lb_return;

	bResult = TRUE;

lb_return:
	return bResult;
}

GLOBAL_FUNC_DLL BOOL __stdcall CalcBoundingMesh(VECTOR3* pv3,DWORD dwVertexNum,COLLISION_MESH_OBJECT_DESC* pDesc)
{
	
	float	max_x = -1000000.0f;
	float	max_y = -1000000.0f;
	float	max_z = -1000000.0f;
	float	min_x = 1000000.0f;
	float	min_y = 1000000.0f;
	float	min_z = 1000000.0f;
	
	VECTOR3*	pos = pv3;
	
	for (DWORD i=0; i<dwVertexNum; i++)
	{
		if (max_x < pos[i].x)
			max_x = pos[i].x;
		
		if (max_y < pos[i].y)
			max_y = pos[i].y;
		
		if (max_z < pos[i].z)
			max_z = pos[i].z;
		
		if (min_x > pos[i].x)
			min_x = pos[i].x;
		
		if (min_y > pos[i].y)
			min_y = pos[i].y;
		
		if (min_z > pos[i].z)
			min_z = pos[i].z;
	}

	VECTOR3		point;

	float		len[3];
	len[0] = (max_x - min_x) / 2.0f;
	len[1] = (max_y - min_y) / 2.0f;
	len[2] = (max_z - min_z) / 2.0f;


	point.x = min_x + len[0];
	point.y = min_y + len[1];
	point.z = min_z + len[2];

	float	rs = -1000000.0f;

	for (i=0; i<3; i++)
	{
		if (rs < len[i])
			rs = len[i];
	}
	rs = (float)sqrt(rs*rs + rs*rs + rs*rs);

	pDesc->boundingSphere.fRs = rs;
	pDesc->boundingSphere.v3Point = point;


	pDesc->boundingBox.v3Oct[0].x = min_x;
	pDesc->boundingBox.v3Oct[0].y = max_y;
	pDesc->boundingBox.v3Oct[0].z = max_z;

	pDesc->boundingBox.v3Oct[1].x = min_x;
	pDesc->boundingBox.v3Oct[1].y = min_y;
	pDesc->boundingBox.v3Oct[1].z = max_z;

	pDesc->boundingBox.v3Oct[2].x = max_x;
	pDesc->boundingBox.v3Oct[2].y = min_y;
	pDesc->boundingBox.v3Oct[2].z = max_z;

	pDesc->boundingBox.v3Oct[3].x = max_x;
	pDesc->boundingBox.v3Oct[3].y = max_y;
	pDesc->boundingBox.v3Oct[3].z = max_z;

	pDesc->boundingBox.v3Oct[4].x = min_x;
	pDesc->boundingBox.v3Oct[4].y = max_y;
	pDesc->boundingBox.v3Oct[4].z = min_z;

	pDesc->boundingBox.v3Oct[5].x = min_x;
	pDesc->boundingBox.v3Oct[5].y = min_y;
	pDesc->boundingBox.v3Oct[5].z = min_z;

	pDesc->boundingBox.v3Oct[6].x = max_x;
	pDesc->boundingBox.v3Oct[6].y = min_y;
	pDesc->boundingBox.v3Oct[6].z = min_z;

	pDesc->boundingBox.v3Oct[7].x = max_x;
	pDesc->boundingBox.v3Oct[7].y = max_y;
	pDesc->boundingBox.v3Oct[7].z = min_z;

	return TRUE;
}
GLOBAL_FUNC_DLL float __stdcall CalcAngle(VECTOR3* vec1, VECTOR3* vec2)
{
	float ang = (float)CalcCosAngle(vec1,vec2);

	ang = (float)acos(ang);
	
	return ang;
}

GLOBAL_FUNC_DLL void __stdcall MakeViewVolumeWithOrtho(VIEW_VOLUME* pViewVolume,VECTOR3* pv3From,VECTOR3* pv3To,VECTOR3* pv3Up, float fWidth, float fHVRatio, float fFar)
{
	VECTOR3		v3Right, v3Up;
	VECTOR3		v0v1;
	float		mag;


	//v0v1 = *pv3To - *pv3From;
	VECTOR3_SUB_VECTOR3(&v0v1,pv3To,pv3From);
	
	CrossProduct(&v3Right, pv3Up,&v0v1);
	Normalize(&v3Right, &v3Right);
	
//	v3Right = v3Right * ( VECTOR3Length(&v0v1) * (float)tan( double(fov/2)) );
	mag = fWidth/2.0f;
	VECTOR3_MUL_FLOAT(&v3Right,&v3Right,mag);

	CrossProduct( &v3Up, &v0v1, &v3Right);
	Normalize( &v3Up, &v3Up);
	
//	v3Up	=	v3Up * ( VECTOR3Length(&v3Right) * (1/fHVRatio));
	mag =  VECTOR3Length(&v3Right) * (1/fHVRatio);
	VECTOR3_MUL_FLOAT(&v3Up,&v3Up,mag);


	VECTOR3		T1, T2, T3, T4;	// 각각 뷰볼륨의 왼쪽, 오른쪽, 아래, 위.

//	T1 = v3From - v3Right;
//	T2 = v3From + v3Right;
//	T3 = v3From - v3Up;
//	T4 = v3From + v3Up;
	VECTOR3_SUB_VECTOR3(&T1, pv3From, &v3Right);	//왼쪽.
	VECTOR3_ADD_VECTOR3(&T2, pv3From, &v3Right);	// 오른쪽.
	VECTOR3_SUB_VECTOR3(&T3, pv3From, &v3Up);		// 아래.
	VECTOR3_ADD_VECTOR3(&T4, pv3From, &v3Up);		// 위.
	
	// 왼쪽
	VECTOR3_MUL_FLOAT( &(pViewVolume->Plane[0].v3Up), &v3Right, -1.0f);
	Normalize( &(pViewVolume->Plane[0].v3Up), &(pViewVolume->Plane[0].v3Up));
	pViewVolume->Plane[0].D			=	-1.0f * DotProduct(&(pViewVolume->Plane[0].v3Up), &T1);

	// 오른쪽
	pViewVolume->Plane[1].v3Up		=	v3Right;
	Normalize( &(pViewVolume->Plane[1].v3Up), &(pViewVolume->Plane[1].v3Up));
	pViewVolume->Plane[1].D			=	-1.0f * DotProduct(&(pViewVolume->Plane[1].v3Up), &T2);

	// 아래
	VECTOR3_MUL_FLOAT( &(pViewVolume->Plane[2].v3Up), &v3Up, -1.0f);
	Normalize( &(pViewVolume->Plane[2].v3Up), &(pViewVolume->Plane[2].v3Up));
	pViewVolume->Plane[2].D			=	-1.0f * DotProduct(&(pViewVolume->Plane[2].v3Up), &T3);

	// 위 
	pViewVolume->Plane[3].v3Up		=	v3Up;
	Normalize( &(pViewVolume->Plane[3].v3Up), &(pViewVolume->Plane[3].v3Up));
	pViewVolume->Plane[3].D			=	-1.0f * DotProduct(&(pViewVolume->Plane[3].v3Up), &T4);

	// 뚜껑
	//
	//
	//

	pViewVolume->From		=	*pv3From;
	pViewVolume->fFar		=	fFar;
	pViewVolume->bIsOrtho	=	TRUE;
	pViewVolume->fWidth = fWidth;
}

GLOBAL_FUNC_DLL void __stdcall MakeViewVolume(VIEW_VOLUME* pViewVolume,VECTOR3* pv3From,VECTOR3* pv3To,VECTOR3* pv3Up, float fov, float fHVRatio, float fFar)
{
	VECTOR3		v3Right, v3Up;
	VECTOR3		v0v1;
	float		mag;


	// To를 Far까지 스케일 시킨다.
	VECTOR3		ToDir;
//	ToDir	=	*pv3To	-	*pv3From;
	VECTOR3_SUB_VECTOR3( &ToDir, pv3To, pv3From);
	Normalize( &ToDir, &ToDir);

	VECTOR3		To	=	*pv3From + ToDir * fFar;
	//v0v1 = To - *pv3From;
	VECTOR3_SUB_VECTOR3(&v0v1,&To,pv3From);
	
	CrossProduct(&v3Right, pv3Up,&v0v1);
	Normalize(&v3Right, &v3Right);
	
//	v3Right = v3Right * ( VECTOR3Length(&v0v1) * (float)tan( double(fov/2)) );
	mag = VECTOR3Length(&v0v1) * (float)tan( double(fov/2));
	VECTOR3_MUL_FLOAT(&v3Right,&v3Right,mag);

	CrossProduct( &v3Up, &v0v1, &v3Right);
	Normalize( &v3Up, &v3Up);
	// 이시점에서의 v3Up은 단위벡터길이이다.

//	v3Up	=	v3Up * ( VECTOR3Length(&v3Right) * (1/fHVRatio));
	mag =  VECTOR3Length(&v3Right) * (1/fHVRatio);
	VECTOR3_MUL_FLOAT(&v3Up,&v3Up,mag);


	VECTOR3		T1, T2, T3, T4;	// 각각 From 입장에서 far를 볼 때, 뷰볼륨의 오른쪽위, 왼쪽위, 왼쪽아래, 오른쪽아래.


//	VECTOR3	tpr = *pv3To + v3Right;
//	VECTOR3	tmr = *pv3To - v3Right;
	VECTOR3	tpr,tmr;
	
	VECTOR3_ADD_VECTOR3(&tpr,&To,&v3Right);	// 오른쪽점.
	VECTOR3_SUB_VECTOR3(&tmr,&To,&v3Right);	// 왼쪽점.

//	T1 = tpr + v3Up;
//	T2 = tmr + v3Up;
//	T3 = tmr - v3Up;
//	T4 = tpr - v3Up;
	VECTOR3_ADD_VECTOR3(&T1,&tpr,&v3Up);
	VECTOR3_ADD_VECTOR3(&T2,&tmr,&v3Up);
	VECTOR3_SUB_VECTOR3(&T3,&tmr,&v3Up);
	VECTOR3_SUB_VECTOR3(&T4,&tpr,&v3Up);
	
	pViewVolume->Points[0]	=	T1;
	pViewVolume->Points[1]	=	T2;
	pViewVolume->Points[2]	=	T3;
	pViewVolume->Points[3]	=	T4;


	VECTOR3		v3[15];

	// 왼쪽
	v3[0] = *pv3From;
	v3[1] = T3;
	v3[2] = T2;

	// 오른쪽
	v3[3] = *pv3From;
	v3[4] = T1;
	v3[5] = T4;

	// 아래
	v3[6] = *pv3From;
	v3[7] = T4;
	v3[8] = T3;

	// 위 
	v3[9] = *pv3From;
	v3[10] = T2;
	v3[11] = T1;

	//	VECTOR3		T1, T2, T3, T4;	// 각각 From 입장에서 far를 볼 때, 뷰볼륨의 오른쪽위, 왼쪽위, 왼쪽아래, 오른쪽아래.

	// 뚜껑 far 평면.
	v3[12] = T1;
	v3[13] = T2;
	v3[14] = T3;

	VECTOR3*	pv3PlaneUp = v3;
	for (DWORD i=0; i<5; i++)
	{
		CalcPlaneEquation(pViewVolume->Plane+i,pv3PlaneUp);	// 왼쪽.
		pv3PlaneUp += 3;
	}

	// near 평면.
	pViewVolume->Plane[5].v3Up.x		=	-1.0f	*	pViewVolume->Plane[4].v3Up.x;
	pViewVolume->Plane[5].v3Up.y		=	-1.0f	*	pViewVolume->Plane[4].v3Up.y;
	pViewVolume->Plane[5].v3Up.z		=	-1.0f	*	pViewVolume->Plane[4].v3Up.z;
	pViewVolume->Plane[5].D			=	-1.0f * DotProduct(	&(pViewVolume->Plane[5].v3Up), &(pViewVolume->From));
	// 바깥쪽을 향해 뒤집다.

	pViewVolume->From		=	*pv3From;
	pViewVolume->fFar		=	fFar;
}
GLOBAL_FUNC_DLL void __stdcall CreateVertexListWithBox(char* pv3Array36,DWORD dwVertexSize,VECTOR3* pv3Oct)
{
	BYTE		bIndex[36] = 
	{
		0,1,2,
		0,2,3,
	
		4,6,5,
		4,7,6,

		0,4,1,
		4,5,1,
		
		2,7,3,
		7,2,6,
		
		0,3,7,
		0,7,4,
		
		1,6,2,
		5,6,1
	};
	for (DWORD i=0; i<36; i++)
	{
		*(VECTOR3*)pv3Array36 = pv3Oct[bIndex[i]];
		pv3Array36 += dwVertexSize;
	}/*
	// 뒷쪽.z방향
	*(VECTOR3*)&v3Oct[0] = pv3Oct[0];
	*(VECTOR3*)&v3Oct[1] = pv3Oct[1];
	*(VECTOR3*)&v3Oct[2] = pv3Oct[2];

	*(VECTOR3*)&v3Oct[3] = pv3Oct[0];
	*(VECTOR3*)&v3Oct[4] = pv3Oct[2];
	*(VECTOR3*)&v3Oct[5] = pv3Oct[3];

	// 앞쪽.-z방향
	*(VECTOR3*)&v3Oct[6] = pv3Oct[4];
	*(VECTOR3*)&v3Oct[7] = pv3Oct[6];
	*(VECTOR3*)&v3Oct[8] = pv3Oct[5];

	*(VECTOR3*)&v3Oct[9] = pv3Oct[4];
	*(VECTOR3*)&v3Oct[10] = pv3Oct[7];
	*(VECTOR3*)&v3Oct[11] = pv3Oct[6];

	// 왼쪽 - x방향 
	*(VECTOR3*)&v3Oct[12] = pv3Oct[0];
	*(VECTOR3*)&v3Oct[13] = pv3Oct[4];
	*(VECTOR3*)&v3Oct[14] = pv3Oct[1];

	*(VECTOR3*)&v3Oct[15] = pv3Oct[4];
	*(VECTOR3*)&v3Oct[16] = pv3Oct[5];
	*(VECTOR3*)&v3Oct[17] = pv3Oct[1];

	// 오른쪽 x방향
	*(VECTOR3*)&v3Oct[18] = pv3Oct[2];
	*(VECTOR3*)&v3Oct[19] = pv3Oct[7];
	*(VECTOR3*)&v3Oct[20] = pv3Oct[3];

	*(VECTOR3*)&v3Oct[21] = pv3Oct[7];
	*(VECTOR3*)&v3Oct[22] = pv3Oct[2];
	*(VECTOR3*)&v3Oct[23] = pv3Oct[6];

	// 윗쪽 y방향 
	*(VECTOR3*)&v3Oct[24] = pv3Oct[0];
	*(VECTOR3*)&v3Oct[25] = pv3Oct[3];
	*(VECTOR3*)&v3Oct[26] = pv3Oct[7];

	*(VECTOR3*)&v3Oct[27] = pv3Oct[0];
	*(VECTOR3*)&v3Oct[28] = pv3Oct[7];
	*(VECTOR3*)&v3Oct[29] = pv3Oct[4];

	// 아랫쪽 
	*(VECTOR3*)&v3Oct[30] = pv3Oct[1];
	*(VECTOR3*)&v3Oct[31] = pv3Oct[6];
	*(VECTOR3*)&v3Oct[32] = pv3Oct[2];

	*(VECTOR3*)&v3Oct[33] = pv3Oct[5];
	*(VECTOR3*)&v3Oct[34] = pv3Oct[6];
	*(VECTOR3*)&v3Oct[35] = pv3Oct[1];*/
}

#ifdef _USE_SSE
GLOBAL_FUNC_DLL	void __stdcall PhysiqueTransform(BYTE* pVertexResult,PHYSIQUE_VERTEX* pPhyVertex,DWORD dwVertexNum,DWORD dwSize,MATRIX4* pMatrixEntry)
{
	__asm
	{
		mov			ecx,dword ptr[dwVertexNum]
		or			ecx,ecx
		jz			lb_return

		mov			esi,pPhyVertex
		
//		mov			edi,dword ptr[pMatrixEntry]	; matrix entry
		mov			edx,dword ptr[pVertexResult]; pVertexResult

lb_loop_physique_num:
		xorps		xmm3,xmm3					; v3Result
		movzx		ecx,byte ptr[esi]			; pPhyVertex->bBonesNum
		mov			eax,dword ptr[esi+1]		; bone entry


lb_loop_bones_num:
		mov			ebx,dword ptr[eax]			; matrix index
		movups		xmm0,[eax+8]				; bone->v3Offset
		shl			ebx,6						; matrix size = 64bytes
		add			ebx,dword ptr[pMatrixEntry] ; matrix entry
		

		movups		xmm4,[ebx]		; load matrix 1 line
		movups		xmm5,[ebx+16]	; load matrix 2 line
		movups		xmm6,[ebx+32]	; load matrix 3 line
		movups		xmm7,[ebx+48]	; load matrix 4 line

		movaps		xmm1,xmm0		; src vector
		shufps		xmm1,xmm1,0		; x x x x
		mulps		xmm1,xmm4		; 1 line ok


		movaps		xmm2,xmm0		; src vector
		shufps		xmm2,xmm2,85	; y y y y
		mulps		xmm2,xmm5		; 2 line ok

		addps		xmm1,xmm2		

		movaps		xmm2,xmm0		; src vector
		shufps		xmm2,xmm2,170	; z z z z 
		mulps		xmm2,xmm6		; 3 line ok
		
		addps		xmm1,xmm2
		addps		xmm1,xmm7

		; xmm1 = ?	|	z	|	y	|	x

		movss		xmm0,[eax+4]	; bone->fWeight
		shufps		xmm0,xmm0,0		; xmm0 = fWeight | fWeight | fWeight | fWeight
		
		mulps		xmm1,xmm0		; v3Result = (v3Offeset[0] * fWeight) + (v3Offeset[1] * fWeight) + (v3Offeset[2] * fWeight) ...
		addps		xmm3,xmm1

		add			eax,BONE_SIZE
		loop		lb_loop_bones_num

		movhlps		xmm0,xmm3		; z

		movlps		[edx],xmm3			; write x,y
		movss		[edx+8],xmm0		; write z

		dec			dword ptr[dwVertexNum]
		jz			lb_return

		add			esi,PHYSIQUE_VERTEX_SIZE
		add			edx,dword ptr[dwSize]

		jmp			lb_loop_physique_num
lb_return:
	}
}

GLOBAL_FUNC_DLL	void __stdcall PhysiqueTransformPosAndWriteUV(BYTE* pVertexResult,PHYSIQUE_VERTEX* pPhyVertex,TVERTEX* ptv,DWORD dwVertexNum,DWORD dwSize,MATRIX4* pMatrixEntry)
{
	__asm
	{
		mov			ecx,dword ptr[dwVertexNum]
		or			ecx,ecx
		jz			lb_return

		mov			esi,pPhyVertex
		mov			edi,ptv
		
//		mov			edi,dword ptr[pMatrixEntry]	; matrix entry
		mov			edx,dword ptr[pVertexResult]; pVertexResult

lb_loop_physique_num:
		xorps		xmm3,xmm3					; v3Result
		movzx		ecx,byte ptr[esi]			; pPhyVertex->bBonesNum
		mov			eax,dword ptr[esi+1]		; bone entry


lb_loop_bones_num:
		mov			ebx,dword ptr[eax]			; matrix index
		movups		xmm0,[eax+8]				; bone->v3Offset
		shl			ebx,6						; matrix size = 64bytes
		add			ebx,dword ptr[pMatrixEntry] ; matrix entry
		

		movups		xmm4,[ebx]		; load matrix 1 line
		movups		xmm5,[ebx+16]	; load matrix 2 line
		movups		xmm6,[ebx+32]	; load matrix 3 line
		movups		xmm7,[ebx+48]	; load matrix 4 line

		movaps		xmm1,xmm0		; src vector
		shufps		xmm1,xmm1,0		; x x x x
		mulps		xmm1,xmm4		; 1 line ok


		movaps		xmm2,xmm0		; src vector
		shufps		xmm2,xmm2,85	; y y y y
		mulps		xmm2,xmm5		; 2 line ok

		addps		xmm1,xmm2		

		movaps		xmm2,xmm0		; src vector
		shufps		xmm2,xmm2,170	; z z z z 
		mulps		xmm2,xmm6		; 3 line ok
		
		addps		xmm1,xmm2
		addps		xmm1,xmm7

		; xmm1 = ?	|	z	|	y	|	x

		movss		xmm0,[eax+4]	; bone->fWeight
		shufps		xmm0,xmm0,0		; xmm0 = fWeight | fWeight | fWeight | fWeight
		
		mulps		xmm1,xmm0		; v3Result = (v3Offeset[0] * fWeight) + (v3Offeset[1] * fWeight) + (v3Offeset[2] * fWeight) ...
		addps		xmm3,xmm1

		add			eax,BONE_SIZE
		loop		lb_loop_bones_num

		movhlps		xmm0,xmm3		; z

		movlps		[edx],xmm3			; write x,y
		movss		[edx+8],xmm0		; write z


		mov			eax,dword ptr[edi]
		mov			[edx+12+12],eax				; u
		mov			eax,dword ptr[edi+4]
		mov			[edx+12+12+4],eax			; v

		dec			dword ptr[dwVertexNum]
		jz			lb_return

		add			edi,8
		add			esi,PHYSIQUE_VERTEX_SIZE
		add			edx,dword ptr[dwSize]

		jmp			lb_loop_physique_num
lb_return:
	}
}
void __stdcall PhysiqueTransformPosAndNormal(BYTE* pVertexResult,PHYSIQUE_VERTEX* pPhyVertex,DWORD dwVertexNum,DWORD dwSize,MATRIX4* pMatrixEntry)
{
	VECTOR4			v3Normal;
	VECTOR4			v3Tangent;

	__asm
	{
		mov			ecx,dword ptr[dwVertexNum]
		or			ecx,ecx
		jz			lb_return

		mov			esi,dword ptr[pPhyVertex]
		mov			edx,dword ptr[pVertexResult]; pVertexResult

lb_loop_physique_num:
		xorps		xmm3,xmm3					; v3Result
		movups		dword ptr[v3Normal],xmm3	; v3Normal
		movups		dword ptr[v3Tangent],xmm3	; v3Tangent
		movzx		ecx,byte ptr[esi]			; pPhyVertex->bBonesNum
		mov			eax,dword ptr[esi+1]		; bone entry


lb_loop_bones_num:
		mov			ebx,dword ptr[eax]			; matrix index
		movups		xmm0,[eax+POS_OFFSET_IN_BONE]	; bone->v3Offset
		shl			ebx,6						; matrix size = 64bytes
		add			ebx,dword ptr[pMatrixEntry]	; matrix entry
		

		movups		xmm4,[ebx]		; load matrix 1 line
		movups		xmm5,[ebx+16]	; load matrix 2 line
		movups		xmm6,[ebx+32]	; load matrix 3 line
		movups		xmm7,[ebx+48]	; load matrix 4 line

		movaps		xmm1,xmm0		; src vector
		shufps		xmm1,xmm1,0		; x x x x
		mulps		xmm1,xmm4		; 1 line ok


		movaps		xmm2,xmm0		; src vector
		shufps		xmm2,xmm2,85	; y y y y
		mulps		xmm2,xmm5		; 2 line ok

		addps		xmm1,xmm2		

		movaps		xmm2,xmm0		; src vector
		shufps		xmm2,xmm2,170	; z z z z 
		mulps		xmm2,xmm6		; 3 line ok
		
		addps		xmm1,xmm2
		addps		xmm1,xmm7

		; 여기서부터 탄젠트 벡터 계산
		; LOAD VECTOR
		movlps	xmm0,[eax+TANGENT_OFFSET_IN_BONE]			; src normal
		movss	xmm2,[eax+TANGENT_OFFSET_IN_BONE+8]
		movlhps	xmm0,xmm2

		movaps		xmm2,xmm0		; src normal
		shufps		xmm2,xmm2,0		; x x x x
		mulps		xmm2,xmm4		; 1 line ok

		movaps		xmm7,xmm0		; src normal
		shufps		xmm7,xmm7,85	; y y y y
		mulps		xmm7,xmm5		; 2 line ok

		addps		xmm2,xmm7

		movaps		xmm7,xmm0		; src normal
		shufps		xmm7,xmm7,170	; z z z z
		mulps		xmm7,xmm6		; 3 line ok

		addps		xmm2,xmm7


		movss		xmm0,[eax+4]	; bone->fWeight
		shufps		xmm0,xmm0,0		; xmm0 = fWeight | fWeight | fWeight | fWeight
		
		; xmm2 = ?  |	tz	|	ty	|	tx
		movups		xmm7,dword ptr[v3Tangent]
		mulps		xmm2,xmm0		; v3Tangent = (v3Offeset[0] * fWeight) + (v3Offeset[1] * fWeight) + (v3Offeset[2] * fWeight) ...
		addps		xmm7,xmm2
		movups		dword ptr[v3Tangent],xmm7


		; 여기서부터 노멀계산..
		; LOAD VECTOR
		movlps	xmm0,[eax+NORMAL_OFFSET_IN_BONE]			; src normal
		movss	xmm2,[eax+NORMAL_OFFSET_IN_BONE+8]
		movlhps	xmm0,xmm2
		
		movaps		xmm2,xmm0		; src normal
		shufps		xmm2,xmm2,0		; x x x x
		mulps		xmm2,xmm4		; 1 line ok

		movaps		xmm7,xmm0		; src normal
		shufps		xmm7,xmm7,85	; y y y y
		mulps		xmm7,xmm5		; 2 line ok

		addps		xmm2,xmm7

		movaps		xmm7,xmm0		; src normal
		shufps		xmm7,xmm7,170	; z z z z
		mulps		xmm7,xmm6		; 3 line ok

		addps		xmm2,xmm7


		; xmm1 = ?	|	z	|	y	|	x
		
		movss		xmm0,[eax+4]	; bone->fWeight
		shufps		xmm0,xmm0,0		; xmm0 = fWeight | fWeight | fWeight | fWeight
		
		mulps		xmm1,xmm0		; v3Result = (v3Offeset[0] * fWeight) + (v3Offeset[1] * fWeight) + (v3Offeset[2] * fWeight) ...
		addps		xmm3,xmm1

		; xmm2 = ?  |	nz	|	ny	|	nx
		movups		xmm7,dword ptr[v3Normal]
		mulps		xmm2,xmm0		; v3Normal = (v3Offeset[0] * fWeight) + (v3Offeset[1] * fWeight) + (v3Offeset[2] * fWeight) ...
		addps		xmm7,xmm2
		movups		dword ptr[v3Normal],xmm7



		add			eax,BONE_SIZE
		dec			ecx
		jnz			lb_loop_bones_num

		movhlps		xmm0,xmm3			; z
		movlps		[edx],xmm3			; write x,y
		movss		[edx+8],xmm0		; write z

		; 노멀 써넣기
		mov			eax,dword ptr[v3Normal.x]
		mov			[edx+12],eax
		mov			eax,dword ptr[v3Normal.y]
		mov			[edx+12+4],eax
		mov			eax,dword ptr[v3Normal.z]
		mov			[edx+12+8],eax

		; tangent 써넣기
		mov			eax,dword ptr[v3Tangent.x]
		mov			[edx+12+8+8+4],eax
		mov			eax,dword ptr[v3Tangent.y]
		mov			[edx+12+8+8+8],eax
		mov			eax,dword ptr[v3Tangent.z]
		mov			[edx+12+8+8+12],eax


		dec			dword ptr[dwVertexNum]
		jz			lb_return

		add			esi,PHYSIQUE_VERTEX_SIZE
		add			edx,dword ptr[dwSize]
		jmp			lb_loop_physique_num
lb_return:
	}
}

void __stdcall PhysiqueTransformPosAndNormalAndUV(BYTE* pVertexResult,PHYSIQUE_VERTEX* pPhyVertex,TVERTEX* ptv,DWORD dwVertexNum,DWORD dwSize,MATRIX4* pMatrixEntry)
{
	VECTOR4			v3Normal;
	VECTOR4			v3Tangent;

	__asm
	{
		mov			ecx,dword ptr[dwVertexNum]
		or			ecx,ecx
		jz			lb_return

		mov			esi,dword ptr[pPhyVertex]
		mov			edx,dword ptr[pVertexResult]; pVertexResult
		mov			edi,dword ptr[ptv];			; uv

lb_loop_physique_num:
		xorps		xmm3,xmm3					; v3Result
		movups		dword ptr[v3Normal],xmm3	; v3Normal
		movups		dword ptr[v3Tangent],xmm3	; v3Tangent
		movzx		ecx,byte ptr[esi]			; pPhyVertex->bBonesNum
		mov			eax,dword ptr[esi+1]		; bone entry


lb_loop_bones_num:
		mov			ebx,dword ptr[eax]			; matrix index
		movups		xmm0,[eax+POS_OFFSET_IN_BONE]	; bone->v3Offset
		shl			ebx,6						; matrix size = 64bytes
		add			ebx,dword ptr[pMatrixEntry]	; matrix entry
		

		movups		xmm4,[ebx]		; load matrix 1 line
		movups		xmm5,[ebx+16]	; load matrix 2 line
		movups		xmm6,[ebx+32]	; load matrix 3 line
		movups		xmm7,[ebx+48]	; load matrix 4 line

		movaps		xmm1,xmm0		; src vector
		shufps		xmm1,xmm1,0		; x x x x
		mulps		xmm1,xmm4		; 1 line ok


		movaps		xmm2,xmm0		; src vector
		shufps		xmm2,xmm2,85	; y y y y
		mulps		xmm2,xmm5		; 2 line ok

		addps		xmm1,xmm2		

		movaps		xmm2,xmm0		; src vector
		shufps		xmm2,xmm2,170	; z z z z 
		mulps		xmm2,xmm6		; 3 line ok
		
		addps		xmm1,xmm2
		addps		xmm1,xmm7


		; 여기서부터 탄젠트 벡터 계산
		; LOAD VECTOR
		movlps	xmm0,[eax+TANGENT_OFFSET_IN_BONE]			; src normal
		movss	xmm2,[eax+TANGENT_OFFSET_IN_BONE+8]
		movlhps	xmm0,xmm2

		movaps		xmm2,xmm0		; src normal
		shufps		xmm2,xmm2,0		; x x x x
		mulps		xmm2,xmm4		; 1 line ok

		movaps		xmm7,xmm0		; src normal
		shufps		xmm7,xmm7,85	; y y y y
		mulps		xmm7,xmm5		; 2 line ok

		addps		xmm2,xmm7

		movaps		xmm7,xmm0		; src normal
		shufps		xmm7,xmm7,170	; z z z z
		mulps		xmm7,xmm6		; 3 line ok

		addps		xmm2,xmm7

		movss		xmm0,[eax+4]	; bone->fWeight
		shufps		xmm0,xmm0,0		; xmm0 = fWeight | fWeight | fWeight | fWeight
		
		; xmm2 = ?  |	tz	|	ty	|	tx
		movups		xmm7,dword ptr[v3Tangent]
		mulps		xmm2,xmm0		; v3Tangent = (v3Offeset[0] * fWeight) + (v3Offeset[1] * fWeight) + (v3Offeset[2] * fWeight) ...
		addps		xmm7,xmm2

		movups		dword ptr[v3Tangent],xmm7



		; 여기서부터 노멀계산..
		; LOAD VECTOR
		movlps	xmm0,[eax+NORMAL_OFFSET_IN_BONE]			; src normal
		movss	xmm2,[eax+NORMAL_OFFSET_IN_BONE+8]
		movlhps	xmm0,xmm2

		movaps		xmm2,xmm0		; src normal
		shufps		xmm2,xmm2,0		; x x x x
		mulps		xmm2,xmm4		; 1 line ok

		movaps		xmm7,xmm0		; src normal
		shufps		xmm7,xmm7,85	; y y y y
		mulps		xmm7,xmm5		; 2 line ok

		addps		xmm2,xmm7

		movaps		xmm7,xmm0		; src normal
		shufps		xmm7,xmm7,170	; z z z z
		mulps		xmm7,xmm6		; 3 line ok

		addps		xmm2,xmm7


		; xmm1 = ?	|	z	|	y	|	x
		
		movss		xmm0,[eax+4]	; bone->fWeight
		shufps		xmm0,xmm0,0		; xmm0 = fWeight | fWeight | fWeight | fWeight
		
		mulps		xmm1,xmm0		; v3Result = (v3Offeset[0] * fWeight) + (v3Offeset[1] * fWeight) + (v3Offeset[2] * fWeight) ...
		addps		xmm3,xmm1

		; xmm2 = ?  |	nz	|	ny	|	nx
		movups		xmm7,dword ptr[v3Normal]
		mulps		xmm2,xmm0		; v3Normal = (v3Offeset[0] * fWeight) + (v3Offeset[1] * fWeight) + (v3Offeset[2] * fWeight) ...
		addps		xmm7,xmm2
		movups		dword ptr[v3Normal],xmm7



		add			eax,BONE_SIZE
		dec			ecx
		jnz			lb_loop_bones_num

		movhlps		xmm0,xmm3			; z
		movlps		[edx],xmm3			; write x,y
		movss		[edx+8],xmm0		; write z

		; 노멀 써넣기
		mov			eax,dword ptr[v3Normal.x]
		mov			[edx+12],eax
		mov			eax,dword ptr[v3Normal.y]
		mov			[edx+12+4],eax
		mov			eax,dword ptr[v3Normal.z]
		mov			[edx+12+8],eax

		; uv 써넣기
		mov			eax,dword ptr[edi]
		mov			[edx+12+8+4],eax			; u
		mov			eax,dword ptr[edi+4]
		mov			[edx+12+8+8],eax			; v

		; tangent 써넣기
		mov			eax,dword ptr[v3Tangent.x]
		mov			[edx+12+8+8+4],eax
		mov			eax,dword ptr[v3Tangent.y]
		mov			[edx+12+8+8+8],eax
		mov			eax,dword ptr[v3Tangent.z]
		mov			[edx+12+8+8+12],eax


		dec			dword ptr[dwVertexNum]
		jz			lb_return

		add			edi,8
		add			esi,PHYSIQUE_VERTEX_SIZE
		add			edx,dword ptr[dwSize]
		jmp			lb_loop_physique_num
lb_return:
	}
}
#else 
GLOBAL_FUNC_DLL	void __stdcall PhysiqueTransform(BYTE* pVertexResult,PHYSIQUE_VERTEX* pPhyVertex,DWORD dwVertexNum,DWORD dwSize,MATRIX4* pMatrixEntry)
{
	VECTOR3	v3,v3Result;
	MATRIX4*	pBoneMatrix;
	DWORD	i,j;

	for (i=0; i<dwVertexNum; i++)
	{
		v3Result.x = 0.0f;
		v3Result.y = 0.0f;
		v3Result.z = 0.0f;

		for (j=0; j<pPhyVertex[i].bBonesNum; j++)
		{

			pBoneMatrix = pMatrixEntry + (DWORD)pPhyVertex[i].pBoneList[j].pBone;
			
			v3.x = 
				pPhyVertex[i].pBoneList[j].v3Offset.x*pBoneMatrix->_11 + 
				pPhyVertex[i].pBoneList[j].v3Offset.y*pBoneMatrix->_21 + 
				pPhyVertex[i].pBoneList[j].v3Offset.z*pBoneMatrix->_31 + pBoneMatrix->_41;

			v3.y = 
				pPhyVertex[i].pBoneList[j].v3Offset.x*pBoneMatrix->_12 + 
				pPhyVertex[i].pBoneList[j].v3Offset.y*pBoneMatrix->_22 + 
				pPhyVertex[i].pBoneList[j].v3Offset.z*pBoneMatrix->_32 + pBoneMatrix->_42;

			v3.z = 
				pPhyVertex[i].pBoneList[j].v3Offset.x*pBoneMatrix->_13 + 
				pPhyVertex[i].pBoneList[j].v3Offset.y*pBoneMatrix->_23 + 
				pPhyVertex[i].pBoneList[j].v3Offset.z*pBoneMatrix->_33 + pBoneMatrix->_43;

			v3.x *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.y *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.z *= pPhyVertex[i].pBoneList[j].fWeight;

			v3Result.x += v3.x;
			v3Result.y += v3.y;
			v3Result.z += v3.z;
		}
		*(VECTOR3*)pVertexResult = v3Result;

		pVertexResult += dwSize;
	}
}
GLOBAL_FUNC_DLL	void __stdcall PhysiqueTransformPosAndWriteUV(BYTE* pVertexResult,PHYSIQUE_VERTEX* pPhyVertex,TVERTEX* ptv,DWORD dwVertexNum,DWORD dwSize,MATRIX4* pMatrixEntry)
{
		VECTOR3	v3,v3Result;
	MATRIX4*	pBoneMatrix;
	DWORD	i,j;

	for (i=0; i<dwVertexNum; i++)
	{
		v3Result.x = 0.0f;
		v3Result.y = 0.0f;
		v3Result.z = 0.0f;

		for (j=0; j<pPhyVertex[i].bBonesNum; j++)
		{

			pBoneMatrix = pMatrixEntry + (DWORD)pPhyVertex[i].pBoneList[j].pBone;
			
			v3.x = 
				pPhyVertex[i].pBoneList[j].v3Offset.x*pBoneMatrix->_11 + 
				pPhyVertex[i].pBoneList[j].v3Offset.y*pBoneMatrix->_21 + 
				pPhyVertex[i].pBoneList[j].v3Offset.z*pBoneMatrix->_31 + pBoneMatrix->_41;

			v3.y = 
				pPhyVertex[i].pBoneList[j].v3Offset.x*pBoneMatrix->_12 + 
				pPhyVertex[i].pBoneList[j].v3Offset.y*pBoneMatrix->_22 + 
				pPhyVertex[i].pBoneList[j].v3Offset.z*pBoneMatrix->_32 + pBoneMatrix->_42;

			v3.z = 
				pPhyVertex[i].pBoneList[j].v3Offset.x*pBoneMatrix->_13 + 
				pPhyVertex[i].pBoneList[j].v3Offset.y*pBoneMatrix->_23 + 
				pPhyVertex[i].pBoneList[j].v3Offset.z*pBoneMatrix->_33 + pBoneMatrix->_43;

			v3.x *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.y *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.z *= pPhyVertex[i].pBoneList[j].fWeight;

			v3Result.x += v3.x;
			v3Result.y += v3.y;
			v3Result.z += v3.z;
		}
		*(VECTOR3*)pVertexResult = v3Result;
		*(TVERTEX*)((char*)pVertexResult + sizeof(VECTOR3) + sizeof(VECTOR3)) = *ptv;

		ptv++;
		pVertexResult += dwSize;
	}
}
GLOBAL_FUNC_DLL	void __stdcall PhysiqueTransformPosAndNormal(BYTE* pVertexResult,PHYSIQUE_VERTEX* pPhyVertex,DWORD dwVertexNum,DWORD dwSize,MATRIX4* pMatrixEntry)
{
	VECTOR3	v3,v3PosResult,v3NormalResult,v3TangentResult;

	DWORD	i,j;
	MATRIX4*	pBoneMatrix;
	for (i=0; i<dwVertexNum; i++)
	{
		
		v3NormalResult.x = 0.0f;
		v3NormalResult.y = 0.0f;
		v3NormalResult.z = 0.0f;

		v3PosResult.x = 0.0f;
		v3PosResult.y = 0.0f;
		v3PosResult.z = 0.0f;

		v3TangentResult.x = 0.0f;
		v3TangentResult.y = 0.0f;
		v3TangentResult.z = 0.0f;


		for (j=0; j<pPhyVertex[i].bBonesNum; j++)
		{
			pBoneMatrix = pMatrixEntry + (DWORD)pPhyVertex[i].pBoneList[j].pBone;

			
			v3.x = 
				pPhyVertex[i].pBoneList[j].v3Offset.x*pBoneMatrix->_11 + 
				pPhyVertex[i].pBoneList[j].v3Offset.y*pBoneMatrix->_21 + 
				pPhyVertex[i].pBoneList[j].v3Offset.z*pBoneMatrix->_31 + pBoneMatrix->_41;

			v3.y = 
				pPhyVertex[i].pBoneList[j].v3Offset.x*pBoneMatrix->_12 + 
				pPhyVertex[i].pBoneList[j].v3Offset.y*pBoneMatrix->_22 + 
				pPhyVertex[i].pBoneList[j].v3Offset.z*pBoneMatrix->_32 + pBoneMatrix->_42;

			v3.z = 
				pPhyVertex[i].pBoneList[j].v3Offset.x*pBoneMatrix->_13 + 
				pPhyVertex[i].pBoneList[j].v3Offset.y*pBoneMatrix->_23 + 
				pPhyVertex[i].pBoneList[j].v3Offset.z*pBoneMatrix->_33 + pBoneMatrix->_43;

			v3.x *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.y *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.z *= pPhyVertex[i].pBoneList[j].fWeight;

			v3PosResult.x += v3.x;
			v3PosResult.y += v3.y;
			v3PosResult.z += v3.z;


			v3.x = 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.x*pBoneMatrix->_11 + 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.y*pBoneMatrix->_21 + 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.z*pBoneMatrix->_31;

			v3.y = 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.x*pBoneMatrix->_12 + 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.y*pBoneMatrix->_22 + 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.z*pBoneMatrix->_32;

			v3.z = 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.x*pBoneMatrix->_13 + 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.y*pBoneMatrix->_23 + 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.z*pBoneMatrix->_33;

			v3.x *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.y *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.z *= pPhyVertex[i].pBoneList[j].fWeight;

			v3NormalResult.x += v3.x;
			v3NormalResult.y += v3.y;
			v3NormalResult.z += v3.z;


			v3.x = 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.x*pBoneMatrix->_11 + 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.y*pBoneMatrix->_21 + 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.z*pBoneMatrix->_31;

			v3.y = 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.x*pBoneMatrix->_12 + 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.y*pBoneMatrix->_22 + 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.z*pBoneMatrix->_32;

			v3.z = 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.x*pBoneMatrix->_13 + 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.y*pBoneMatrix->_23 + 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.z*pBoneMatrix->_33;

			v3.x *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.y *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.z *= pPhyVertex[i].pBoneList[j].fWeight;

			v3TangentResult.x += v3.x;
			v3TangentResult.y += v3.y;
			v3TangentResult.z += v3.z;


		}
		*(VECTOR3*)pVertexResult = v3PosResult;
		*(VECTOR3*)((char*)pVertexResult + sizeof(VECTOR3)) = v3NormalResult;
		*(VECTOR3*)((char*)pVertexResult + sizeof(VECTOR3) + sizeof(VECTOR3) + sizeof(TVERTEX)) = v3TangentResult;
		pVertexResult += dwSize;
	}
}
void __stdcall PhysiqueTransformPosAndNormalAndUV(BYTE* pVertexResult,PHYSIQUE_VERTEX* pPhyVertex,TVERTEX* ptv,DWORD dwVertexNum,DWORD dwSize,MATRIX4* pMatrixEntry)
{
	VECTOR3	v3,v3PosResult,v3NormalResult,v3TangentResult;

	DWORD	i,j;
	MATRIX4*	pBoneMatrix;
	for (i=0; i<dwVertexNum; i++)
	{
		
		v3NormalResult.x = 0.0f;
		v3NormalResult.y = 0.0f;
		v3NormalResult.z = 0.0f;

		v3PosResult.x = 0.0f;
		v3PosResult.y = 0.0f;
		v3PosResult.z = 0.0f;

		v3TangentResult.x = 0.0f;
		v3TangentResult.y = 0.0f;
		v3TangentResult.z = 0.0f;

		for (j=0; j<pPhyVertex[i].bBonesNum; j++)
		{
			pBoneMatrix = pMatrixEntry + (DWORD)pPhyVertex[i].pBoneList[j].pBone;

			
			v3.x = 
				pPhyVertex[i].pBoneList[j].v3Offset.x*pBoneMatrix->_11 + 
				pPhyVertex[i].pBoneList[j].v3Offset.y*pBoneMatrix->_21 + 
				pPhyVertex[i].pBoneList[j].v3Offset.z*pBoneMatrix->_31 + pBoneMatrix->_41;

			v3.y = 
				pPhyVertex[i].pBoneList[j].v3Offset.x*pBoneMatrix->_12 + 
				pPhyVertex[i].pBoneList[j].v3Offset.y*pBoneMatrix->_22 + 
				pPhyVertex[i].pBoneList[j].v3Offset.z*pBoneMatrix->_32 + pBoneMatrix->_42;

			v3.z = 
				pPhyVertex[i].pBoneList[j].v3Offset.x*pBoneMatrix->_13 + 
				pPhyVertex[i].pBoneList[j].v3Offset.y*pBoneMatrix->_23 + 
				pPhyVertex[i].pBoneList[j].v3Offset.z*pBoneMatrix->_33 + pBoneMatrix->_43;

			v3.x *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.y *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.z *= pPhyVertex[i].pBoneList[j].fWeight;

			v3PosResult.x += v3.x;
			v3PosResult.y += v3.y;
			v3PosResult.z += v3.z;


			v3.x = 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.x*pBoneMatrix->_11 + 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.y*pBoneMatrix->_21 + 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.z*pBoneMatrix->_31;

			v3.y = 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.x*pBoneMatrix->_12 + 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.y*pBoneMatrix->_22 + 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.z*pBoneMatrix->_32;

			v3.z = 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.x*pBoneMatrix->_13 + 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.y*pBoneMatrix->_23 + 
				pPhyVertex[i].pBoneList[j].v3NormalOffset.z*pBoneMatrix->_33;

			v3.x *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.y *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.z *= pPhyVertex[i].pBoneList[j].fWeight;

			v3NormalResult.x += v3.x;
			v3NormalResult.y += v3.y;
			v3NormalResult.z += v3.z;


			v3.x = 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.x*pBoneMatrix->_11 + 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.y*pBoneMatrix->_21 + 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.z*pBoneMatrix->_31;

			v3.y = 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.x*pBoneMatrix->_12 + 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.y*pBoneMatrix->_22 + 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.z*pBoneMatrix->_32;

			v3.z = 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.x*pBoneMatrix->_13 + 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.y*pBoneMatrix->_23 + 
				pPhyVertex[i].pBoneList[j].v3TangentOffset.z*pBoneMatrix->_33;

			v3.x *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.y *= pPhyVertex[i].pBoneList[j].fWeight;
			v3.z *= pPhyVertex[i].pBoneList[j].fWeight;

			v3TangentResult.x += v3.x;
			v3TangentResult.y += v3.y;
			v3TangentResult.z += v3.z;

		}
		*(VECTOR3*)pVertexResult = v3PosResult;
		*(VECTOR3*)((char*)pVertexResult + sizeof(VECTOR3)) = v3NormalResult;
		*(TVERTEX*)((char*)pVertexResult + sizeof(VECTOR3) + sizeof(VECTOR3)) = *ptv;
		*(VECTOR3*)((char*)pVertexResult + sizeof(VECTOR3) + sizeof(VECTOR3) + sizeof(TVERTEX)) = v3TangentResult;
		ptv++;
		pVertexResult += dwSize;
	}
}
#endif


GLOBAL_FUNC_DLL BOOL __stdcall IsCollisionMeshWithScreenCoord(VECTOR3* pv3IntersectPoint,float* pfDist,COLLISION_MESH_OBJECT_DESC* pColMeshDesc,MATRIX4* pMatView,MATRIX4* pMatProj,POINT* ptCursor,SHORT_RECT* pClipRect)
{
	BOOL		bResult = FALSE;

	VECTOR3		v3Pos,v3Dir;

	WORD wWidth = pClipRect->right - pClipRect->left;
	WORD wHeight = pClipRect->bottom - pClipRect->top;

	POINT p;
	if (ptCursor->x > (long)pClipRect->right || ptCursor->y > (long)pClipRect->bottom)
		goto lb_return;

	p.x = ptCursor->x - (long)pClipRect->left;
	p.y = ptCursor->y - (long)pClipRect->top;

	if (p.x < 0 || p.y < 0)
		goto lb_return;

	CalcRay(&v3Pos,&v3Dir,p.x,p.y,(DWORD)wWidth,(DWORD)wHeight,pMatProj,pMatView);
	bResult = IsCollisionMeshAndRay(pv3IntersectPoint,pfDist,pColMeshDesc,&v3Pos,&v3Dir);
	
lb_return:
	return bResult;

}
GLOBAL_FUNC_DLL BOOL __stdcall IsCollisionMeshAndRay(VECTOR3* pv3IntersectPoint,float* pfDist,COLLISION_MESH_OBJECT_DESC* pColMeshDesc,VECTOR3* pv3Pos,VECTOR3* pv3Dir)
{

	VECTOR3			v3Face[36];
	
	BOOL			bResult = FALSE;




	float	t = 
		( DotProduct(pv3Dir,&pColMeshDesc->boundingSphere.v3Point) - DotProduct(pv3Pos,pv3Dir) ) /
		DotProduct(pv3Dir,pv3Dir);

	
	VECTOR3		v3PosX;
	VECTOR3_MUL_FLOAT(&v3PosX,pv3Dir,t);
	VECTOR3_ADDEQU_VECTOR3(&v3PosX,pv3Pos);

	if ( CalcDistance(&v3PosX,&pColMeshDesc->boundingSphere.v3Point) > pColMeshDesc->boundingSphere.fRs)
		goto lb_return;

	CreateVertexListWithBox((char*)v3Face,sizeof(VECTOR3),pColMeshDesc->boundingBox.v3Oct);

	VECTOR3*		pv3Tri;
	pv3Tri= v3Face;

	float		fBary1,fBary2;
	float		fMinDist;
	float		fDist;

	fMinDist = 900000.0f;
	

	DWORD	i;
	for (i=0; i<12; i++)
	{
		if (IntersectTriangle(
			pv3IntersectPoint,
			pv3Pos,
			pv3Dir,
			pv3Tri+0,
			pv3Tri+1,
			pv3Tri+2,
			&fDist,
			&fBary1,
			&fBary2,
			TRUE))
		{
			if (fDist < fMinDist)
			{
				fMinDist = fDist;

				*pfDist = fDist;
				bResult = TRUE;
			}
		}
		pv3Tri += 3;

	}

lb_return:
	return bResult;
}

GLOBAL_FUNC_DLL void __stdcall SetFaceNormalToVertex(VECTOR3* pv3NormalResult,VECTOR3* pv3List,DWORD dwVertexNum,WORD* pIndex,DWORD dwFacesNum)
{
	VECTOR3			u,v,c;
	DWORD			i,k;
	
	for (i=0; i<dwFacesNum; i++)
	{
		
		VECTOR3_SUB_VECTOR3(&u,&pv3List[pIndex[i*3+1]],&pv3List[pIndex[i*3+0]]);
		VECTOR3_SUB_VECTOR3(&v,&pv3List[pIndex[i*3+2]],&pv3List[pIndex[i*3+0]]);
		CrossProduct(&c,&u,&v);
		for (k=0; k<3; k++)
		{
			VECTOR3_ADDEQU_VECTOR3(&pv3NormalResult[pIndex[i*3+k]],&c);
		
			
		}
	}
}

GLOBAL_FUNC_DLL void __stdcall SetFaceTangentToVertex(VECTOR3* pv3TangentResult,VECTOR3* pv3List,TVERTEX* ptvList,DWORD dwVertexNum,WORD* pIndex,DWORD dwFacesNum)
{

	DWORD			i,k;
	
	for (i=0; i<dwFacesNum; i++)
	{
		VECTOR3	P,Q;
		VECTOR3_SUB_VECTOR3(&P,&pv3List[pIndex[i*3+1]],&pv3List[pIndex[i*3+0]]);
		VECTOR3_SUB_VECTOR3(&Q,&pv3List[pIndex[i*3+2]],&pv3List[pIndex[i*3+0]]);

		float	s1,s2,t1,t2;
		s1 = ptvList[pIndex[i*3+1]].u - ptvList[pIndex[i*3+0]].u;
		t1 = ptvList[pIndex[i*3+1]].v - ptvList[pIndex[i*3+0]].v;

		s2 = ptvList[pIndex[i*3+2]].u - ptvList[pIndex[i*3+0]].u;
		t2 = ptvList[pIndex[i*3+2]].v - ptvList[pIndex[i*3+0]].v;

		float	A = 1.0f / (s1*t2 - s2*t1);


		VECTOR3	v3Tangent;
		
		v3Tangent.x = 1.0f * A*(t2*P.x - t1*Q.x);
		v3Tangent.y = 1.0f * A*(t2*P.y - t1*Q.y);
		v3Tangent.z = 1.0f * A*(t2*P.z - t1*Q.z);

//		v3Tangent.x = A*(s1*P.x - s2*Q.x);
//		v3Tangent.y = A*(s1*P.y - s2*Q.y);
//		v3Tangent.z = A*(s1*P.z - s2*Q.z);



		for (k=0; k<3; k++)
		{
			VECTOR3_ADDEQU_VECTOR3(&pv3TangentResult[pIndex[i*3+k]],&v3Tangent);
					
		}
	}
}


/*
GLOBAL_FUNC_DLL BOOL __stdcall CalcIntersectPointRayAndTri(VECTOR3* pv3IntersectPoint,VECTOR3* pv3Orig,VECTOR3* pv3Dir,VECTOR3* pv3Tri)
{
	PLANE	plane;
	VECTOR3	v3To;
	float	t;

	VECTOR3_ADD_VECTOR3(&v3To,pv3Orig,pv3Dir);
	CalcPlaneEquation(&plane,pv3Tri);
	return CalcIntersectPointLineAndPlane(pv3IntersectPoint,&plane,pv3Orig,&v3To,&t);
}*/
GLOBAL_FUNC_DLL void __stdcall SetTransformBoundingMesh(COLLISION_MESH_OBJECT_DESC* pDest,COLLISION_MESH_OBJECT_DESC* pSrc,MATRIX4* pMat)
{
	TransformVector3_VPTR2(
		&pDest->boundingSphere.v3Point,
		&pSrc->boundingSphere.v3Point,
		pMat,1);

	TransformVector3_VPTR2(
		pDest->boundingBox.v3Oct,
		pSrc->boundingBox.v3Oct,
		pMat,8);

}
GLOBAL_FUNC_DLL void __stdcall CalcRay(VECTOR3* pv3Pos,VECTOR3* pv3Dir,DWORD dwPosX,DWORD dwPosY,DWORD dwWidth,DWORD dwHeight,MATRIX4* pProj,MATRIX4* pView)
{
	// Compute the vector of the pick ray in screen space
	VECTOR3		v;

	v.x =  ( ( ( 2.0f * (float)dwPosX ) / (float)dwWidth) - 1.0f ) / pProj->_11;
    v.y = -( ( ( 2.0f * (float)dwPosY ) / (float)dwHeight) - 1.0f ) / pProj->_22;
    v.z =  1.0f;

	// Get the inverse view matrix
	MATRIX4		matViewInverser;
	SetInverseMatrix(&matViewInverser,pView);

	
	// Transform the screen space pick ray into 3D space
    pv3Dir->x  = v.x*matViewInverser._11 + v.y*matViewInverser._21 + v.z*matViewInverser._31;
    pv3Dir->y  = v.x*matViewInverser._12 + v.y*matViewInverser._22 + v.z*matViewInverser._32;
    pv3Dir->z  = v.x*matViewInverser._13 + v.y*matViewInverser._23 + v.z*matViewInverser._33;
    pv3Pos->x = matViewInverser._41;
    pv3Pos->y = matViewInverser._42;
    pv3Pos->z = matViewInverser._43;
}
GLOBAL_FUNC_DLL void __stdcall ResetTM(NODE_TM* pTM)
{
	memset(pTM,0,sizeof(NODE_TM));
	pTM->fScaleX = 1.0f;
	pTM->fScaleY = 1.0f;
	pTM->fScaleZ = 1.0f;
	SetIdentityMatrix(&pTM->mat4);
	pTM->mat4Inverse = pTM->mat4;
}

GLOBAL_FUNC_DLL BOOL __stdcall IntersectTriangle(VECTOR3* pv3IntersectPoint,VECTOR3* orig,VECTOR3* dir,VECTOR3* v0,VECTOR3* v1, VECTOR3* v2,float* t, float* u, float* v,BOOL bCullBackface)
{
    // Find vectors for two edges sharing vert0
//  VECTOR3 edge1 = *v1 - *v0;
//  VECTOR3 edge2 = *v2 - *v0;
	
	VECTOR3 edge1,edge2;
	VECTOR3_SUB_VECTOR3(&edge1,v1,v0);
	VECTOR3_SUB_VECTOR3(&edge2,v2,v0);
	

    // Begin calculating determinant - also used to calculate U parameter
    VECTOR3 pvec;
    CrossProduct(&pvec,dir,&edge2 );

    // If determinant is near zero, ray lies in plane of triangle

//	float det = edge1.x * pvec.x + edge1.y * pvec.y + edge1.z * pvec.z;
	float det = DotProduct(&edge1,&pvec);
  
	VECTOR3 tvec;
    if( det > 0 )
    {
//        tvec = *orig - *v0;
		VECTOR3_SUB_VECTOR3(&tvec,orig,v0);
    }
    else
	{
		if (bCullBackface)
			return FALSE;

		VECTOR3_SUB_VECTOR3(&tvec,v0,orig);
		det = -det;
	}




    if( det < 0.0001f )
        return FALSE;



    // Calculate U parameter and test bounds

//	*u = tvec.x*pvec.x + tvec.y*pvec.y + tvec.z*pvec.z;
	*u = DotProduct(&tvec,&pvec);

    if( *u < 0.0f || *u > det )
        return FALSE;

    // Prepare to test V parameter
    VECTOR3 qvec;
    CrossProduct( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
//	*v = dir->x*qvec.x + dir->y*qvec.y + dir->z*qvec.z;
	*v = DotProduct(dir,&qvec);

    if( *v < 0.0f || *u + *v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
//	*t = edge2.x*qvec.x + edge2.y*qvec.y + edge2.z*qvec.z;
	*t = DotProduct(&edge2,&qvec);

	
    FLOAT fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

	if (pv3IntersectPoint)
	{
	//	VECTOR3 edge1,edge2;
	//	VECTOR3_SUB_VECTOR3(&edge1,v1,v0);
	//	VECTOR3_SUB_VECTOR3(&edge2,v2,v0);

		VECTOR3	uoffset,voffset;
		VECTOR3_MUL_FLOAT(&uoffset,&edge1,*u);
		VECTOR3_MUL_FLOAT(&voffset,&edge2,*v);

		VECTOR3_ADD_VECTOR3(pv3IntersectPoint,v0,&uoffset);
		VECTOR3_ADDEQU_VECTOR3(pv3IntersectPoint,&voffset);
/*

		PLANE	plane;
		VECTOR3	v3To;
		float	t;
		
		VECTOR3	v3Tri[3];
		v3Tri[0] = *v0;
		v3Tri[1] = *v1;
		v3Tri[2] = *v2;
		
		VECTOR3_ADD_VECTOR3(&v3To,orig,dir);
		CalcPlaneEquation(&plane,v3Tri);
		CalcIntersectPointLineAndPlane(pv3IntersectPoint,&plane,orig,&v3To,&t);*/
	//	if (pv3IntersectPoint->y == 0.0f)
	//		__asm nop
	}

    return TRUE;
}
GLOBAL_FUNC_DLL void __stdcall MakeViewVolumeWithSelectRange(VIEW_VOLUME* pViewVolume,VECTOR3* pv3From, VECTOR3* pv3LeftUp, VECTOR3* pv3RightUp, VECTOR3* pv3RightDown, VECTOR3* pv3LeftDown)
{
	pViewVolume->bIsOrtho	=	FALSE;
	pViewVolume->From		=	*pv3From;
	VECTOR3			v3Tri[3];

	// 왼쪽.
	v3Tri[0] = *pv3From;
	v3Tri[1] = *pv3LeftDown;
	v3Tri[2] = *pv3LeftUp;
	CalcPlaneEquation(&(pViewVolume->Plane[0]),v3Tri);
//	MakePLANE( &(pViewVolume->Plane[0]), pv3From, pv3LeftDown, pv3LeftUp);

	// 오른쪽.
	v3Tri[0] = *pv3From;
	v3Tri[1] = *pv3RightUp;
	v3Tri[2] = *pv3RightDown;
	CalcPlaneEquation(&(pViewVolume->Plane[1]),v3Tri);
//	MakePLANE( &(pViewVolume->Plane[1]), pv3From, pv3RightUp, pv3RightDown);
	
	// 아래.
	v3Tri[0] = *pv3From;
	v3Tri[1] = *pv3RightDown;
	v3Tri[2] = *pv3LeftDown;
	CalcPlaneEquation(&(pViewVolume->Plane[2]),v3Tri);
//	MakePLANE( &(pViewVolume->Plane[2]), pv3From, pv3RightDown, pv3LeftDown);

	// 위.
	v3Tri[0] = *pv3From;
	v3Tri[1] = *pv3LeftUp;
	v3Tri[2] = *pv3RightUp;
	CalcPlaneEquation(&(pViewVolume->Plane[3]),v3Tri);
//	MakePLANE( &(pViewVolume->Plane[3]), pv3From, pv3LeftUp, pv3RightUp);

	// far.
	PLANE	p;
	v3Tri[0] = *pv3LeftUp;
	v3Tri[1] = *pv3RightUp;
	v3Tri[2] = *pv3RightDown;
	CalcPlaneEquation(&p,v3Tri);
//	MakePLANE( &p, pv3LeftUp, pv3RightUp, pv3RightDown);

	pViewVolume->fFar	=	DotProduct( &(p.v3Up), pv3From) + p.D;
	

}


GLOBAL_FUNC_DLL void __stdcall FindNearestVertexOnLine(VECTOR3* OUT pv3Result ,float* OUT pT, VECTOR3* IN pv3From, VECTOR3* IN pv3To, VECTOR3* IN pv3Point)
{
	//	직선 From-To위의 한 점을 점 Result라고 할때, 직선 From-To와  직선 Result-v의 내적은 0이다.
	//	직선은 벡터Result = 벡터From + t*벡터FromTo
	//	t를 먼저 구하고, 그 t를 직선방정식에 대입.
	//	t = ((T-F)*(V-F)) / ((T-F)*(T-F))

//	*pT			=	((To.x-From.x)*(v.x-From.x) + (To.y-From.y)*(v.y-From.y) + (To.z-From.z)*(v.z-From.z)) / ( (To.x-From.x)*(To.x-From.x) + (To.y-From.y)*(To.y-From.y) + (To.z-From.z)*(To.z-From.z) );


//	(*pOut).x	=	From.x + (*pT) * (To.x - From.x);
//	(*pOut).y	=	From.y + (*pT) * (To.y - From.y);
//	(*pOut).z	=	From.z + (*pT) * (To.z - From.z);
//	VECTOR3_SUB_VECTOR3( pOut, &To, &From);
//	VECTOR3_MULEQU_FLOAT( pOut, *pT);
//	VECTOR3_ADDEQU_VECTOR3( pOut, &From);

	VECTOR3		Velocity;
	VECTOR3_SUB_VECTOR3( &Velocity,pv3To,pv3From);
	VECTOR3		FV;
	VECTOR3_SUB_VECTOR3( &FV,pv3Point,pv3From);

	*pT		=	DotProduct( &Velocity, &FV) / DotProduct( &Velocity, &Velocity);

	*pv3Result	=	Velocity;
	VECTOR3_MULEQU_FLOAT( pv3Result, *pT);
	VECTOR3_ADDEQU_VECTOR3( pv3Result,pv3From);

	/*
		*pT		=	
		((pv3To->x-pv3From->x)*(pv3Point->x-pv3From->x) + (pv3To->y-pv3From->y)*(pv3Point->y-pv3From->y) + 
		(pv3To->z-pv3From->z)*(pv3Point->z-pv3From->z)) / 
		((pv3To->x-pv3From->x)*(pv3To->x-pv3From->x) + 
		(pv3To->y-pv3From->y)*(pv3To->y-pv3From->y) + 
		(pv3To->z-pv3From->z)*(pv3To->z-pv3From->z) );

	pv3Result->x = pv3From->x + *pT * (pv3To->x - pv3From->x);
	pv3Result->y = pv3From->y + *pT * (pv3To->y - pv3From->y);
	pv3Result->z = pv3From->z + *pT * (pv3To->z - pv3From->z);
	*/
}
GLOBAL_FUNC_DLL void __stdcall Set2PowValueLess(DWORD* pdwOut,DWORD dwIn)
{

	DWORD	size = 2;

lb_pow_width:
	size *= 2;
	if (size <= dwIn)
		goto lb_pow_width;

	*pdwOut = size / 2;
}
GLOBAL_FUNC_DLL BOOL __stdcall RemoveCRLF(char* pStr,DWORD dwLen)
{	
	BOOL	bResult = FALSE;
	for (DWORD i=0; i<dwLen; i++)
	{
		if (pStr[i] == 0x0d)
		{
			pStr[i] = 0x20;
			bResult = TRUE;
		}
		else if (pStr[i] == 0x0a)
		{
			pStr[i] = 0x20;
			bResult = TRUE;
		}
		else if (pStr[i] == 0x09)
		{
			pStr[i] = 0x20;
			bResult = TRUE;
		}
	}
	return bResult;

}
GLOBAL_FUNC_DLL DWORD __stdcall COLORtoDWORD(float r,float g,float b,float a)
{
	DWORD R = (DWORD)(r*255);
	DWORD G = (DWORD)(g*255);
	DWORD B = (DWORD)(b*255);
	DWORD A = (DWORD)(a*255);

	return ( 
		( (A<<24) & 0xff000000 ) | 
		( (R<<16) & 0x00ff0000 ) |
		( (G<<8) & 0x0000ff00 ) |
		( B & 0x000000ff )
		);
}
GLOBAL_FUNC_DLL void __stdcall SetBitmapFromTexturePlane(char* pBits,DWORD dwPitch,TEXTURE_PLANE* pTexPlane,DWORD dwTexPlaneNum)
{
	PATCH*			pPatch;
	for (DWORD m=0; m<dwTexPlaneNum; m++)
	{

		pPatch = pTexPlane[m].pPatch;
		for (DWORD i=0; i<pTexPlane[m].dwPatchNum; i++)
		{
			*(WORD*)((char*)pBits + ((pPatch[i].sx)<<1) + (pPatch[i].sy)*dwPitch) = 
				(WORD)(

				((pPatch[i].dwColor & 0x00ff0000)>>16>>3<<11) |
				((pPatch[i].dwColor & 0x0000ff00)>>8>>2<<5) |
				(pPatch[i].dwColor & 0x000000ff)>>3);
		}
	}
}

#ifdef _USE_SSE
GLOBAL_FUNC_DLL DWORD __stdcall CullBackFace(WORD* pFaceIndexList,TRI_FACE* pFaceEntry,VECTOR3* pv3Eye,DWORD dwFacesNum)
{
	DWORD	dwCount;

	__asm
	{
		xor		ebx,ebx
		xor		eax,eax
		mov		dword ptr[dwCount],eax
		xorps	xmm6,xmm6
		mov		esi,pv3Eye

		; LOAD VECTOR
		movlps	xmm7,[esi]			; camera position
		movss	xmm2,[esi+8]
		movlhps	xmm7,xmm2

//		movups	xmm7,[esi]						; camera position
		
		mov		ecx,dword ptr[dwFacesNum]

		mov		edi,dword ptr[pFaceIndexList]
		mov		esi,dword ptr[pFaceEntry]

lb_loop:
		movaps	xmm2,xmm7							; restore camera position
		movups	xmm5,[esi+TRI_FACE_NORMAL_OFFSET]	; load face normal
		

		mulps	xmm2,xmm5
		
		movhlps xmm3,xmm2	
		addps	xmm3,xmm2	// xmm[0-31] = x+z

		movaps	xmm4,xmm2
		shufps	xmm4,xmm4,1	// xmm [0-31] = y
				
		addss	xmm3,xmm4
		addss	xmm3,dword ptr[esi+TRI_FACE_D_OFFSET]

		comiss	xmm3,xmm6					; if cosang <= 0.0f skip
		jbe		lb_next

		mov		word ptr[edi],bx
		inc		eax
		add		edi,2
lb_next:
		inc		ebx
		add		esi,TRI_FACE_SIZE

		loop	lb_loop
		
		mov		dword ptr[dwCount],eax
		
	}
	return dwCount;

}
#else
GLOBAL_FUNC_DLL DWORD __stdcall CullBackFace(WORD* pFaceIndexList,TRI_FACE* pFaceEntry,VECTOR3* pv3Eye,DWORD dwFacesNum)
{
	DWORD	dwCount;

	dwCount = 0;
	for (DWORD i=0; i<dwFacesNum; i++)
	{
		if (0 > (DotProduct(&pFaceEntry[i].plane.v3Up,pv3Eye) + pFaceEntry[i].plane.D))
			continue;

		pFaceIndexList[dwCount] = i;
		dwCount++;
	}
	return dwCount;
}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL DWORD __stdcall BuildIndexBufferWithFaceIndexListForShadow(WORD* pIndexBufferEntry,TRI_FACE* pFaceEntry,WORD* pwFaceIndexEntry,VECTOR3* pv3Eye,BOUNDING_SPHERE* pSphere,DWORD dwFacesNum)
{
	DWORD	dwCount;
	float	fRs = pSphere->fRs;

	__asm
	{
		xor		eax,eax
		mov		dword ptr[dwCount],eax
		xorps	xmm6,xmm6
		mov		esi,dword ptr[pv3Eye]

//		movups	xmm7,[esi]						; camera position
		; LOAD VECTOR
		movlps	xmm7,[esi]			; camera position
		movss	xmm2,[esi+8]
		movlhps	xmm7,xmm2


		mov		esi,dword ptr[pSphere]
//		movups	xmm0,[esi]						; sender position
		; LOAD VECTOR
		movlps	xmm0,[esi]			; sender position
		movss	xmm2,[esi+8]
		movlhps	xmm0,xmm2

		mov		ecx,dword ptr[esi+VECTOR3_SIZE]	; sender rs
		mov		dword ptr[fRs],ecx
		
			
		mov		ecx,dword ptr[dwFacesNum]

		mov		edi,dword ptr[pIndexBufferEntry]
		mov		esi,dword ptr[pwFaceIndexEntry]
		mov		ebx,dword ptr[pFaceEntry]
		

lb_loop:
		push	TRI_FACE_SIZE
		pop		edx

		
		movzx	eax,word ptr[esi]
		mul		edx

		add		eax,ebx									; pFaceEntry[pwFaceIndexEntry[i]]
		
		; compare light or camera position
		movaps	xmm2,xmm7								; restore camera position
		movups	xmm5,[eax+TRI_FACE_NORMAL_OFFSET]		; load face normal
		movss	xmm1,dword ptr[eax+TRI_FACE_D_OFFSET]	; load D
		

		mulps	xmm2,xmm5
		
		movhlps xmm3,xmm2	
		addps	xmm3,xmm2	// xmm[0-31] = x+z

		movaps	xmm4,xmm2
		shufps	xmm4,xmm4,1	// xmm [0-31] = y
				
		addss	xmm3,xmm4
		addss	xmm3,xmm1

		comiss	xmm3,xmm6							; if cosang <= 0.0f skip
		jb		lb_next

		
		; compare sender position
		; xmm1 = D , xmm5 = normal , xmm0 = sender point

		movaps	xmm2,xmm0							; restore sender position
		mulps	xmm2,xmm5
		
		movhlps xmm3,xmm2	
		addss	xmm3,xmm2	// xmm[0-31] = x+z

		movaps	xmm4,xmm2
		shufps	xmm4,xmm4,1	// xmm [0-31] = y
				
		addss	xmm3,xmm4
		addss	xmm3,xmm1

		comiss	xmm3,xmm6
		jae		lb_write
		//jb		lb_next

		; 반지름을 더해서 다시 계산...
		addss	xmm3,dword ptr[fRs]
		comiss	xmm3,xmm6
		jb		lb_next

lb_write:
		mov		edx,dword ptr[eax+TRI_FACE_INDEX_OFFSET]	; read index 0,1
		mov		dword ptr[edi],edx							; write index 0,1
		mov		dx,word ptr[eax+TRI_FACE_INDEX_OFFSET+4]	; read index 2
		mov		word ptr[edi+4],dx							; write index 2
		
		inc		dword ptr[dwCount]
		add		edi,2*3

lb_next:
		add		esi,2
		loop	lb_loop
	}
	return dwCount;
}
#else
GLOBAL_FUNC_DLL DWORD __stdcall BuildIndexBufferWithFaceIndexListForShadow(WORD* pIndexBufferEntry,TRI_FACE* pFaceEntry,WORD* pwFaceIndexEntry,VECTOR3* pv3Eye,BOUNDING_SPHERE* pSphere,DWORD dwFacesNum)
{
	DWORD dwCount;
	dwCount = 0;
	for (DWORD i=0; i<dwFacesNum; i++)
	{

		if (0.0f > (DotProduct(&pFaceEntry[pwFaceIndexEntry[i]].plane.v3Up,pv3Eye) + pFaceEntry[pwFaceIndexEntry[i]].plane.D))
			continue;

		if (0.0f > (DotProduct(&pFaceEntry[pwFaceIndexEntry[i]].plane.v3Up,&pSphere->v3Point) + pFaceEntry[pwFaceIndexEntry[i]].plane.D))
		{
			if (0.0f > DotProduct(&pFaceEntry[pwFaceIndexEntry[i]].plane.v3Up,&pSphere->v3Point) + pFaceEntry[pwFaceIndexEntry[i]].plane.D+pSphere->fRs)
				continue;
		}

		pIndexBufferEntry[dwCount*3+0] = pFaceEntry[pwFaceIndexEntry[i]].wIndex[0];
		pIndexBufferEntry[dwCount*3+1] = pFaceEntry[pwFaceIndexEntry[i]].wIndex[1];
		pIndexBufferEntry[dwCount*3+2] = pFaceEntry[pwFaceIndexEntry[i]].wIndex[2];
		dwCount++;
	}
	return dwCount;
}
#endif

#ifdef _USE_SSE
GLOBAL_FUNC_DLL DWORD __stdcall BuildIndexBufferWithFaceIndexList(WORD* pIndexBufferEntry,TRI_FACE* pFaceEntry,WORD* pwFaceIndexEntry,VECTOR3* pv3Eye,DWORD dwFacesNum)
{
	DWORD	dwCount;

	__asm
	{
		xor		eax,eax
		mov		dword ptr[dwCount],eax
		xorps	xmm6,xmm6
		mov		esi,pv3Eye

//		movups	xmm7,[esi]						; camera position
		; LOAD VECTOR
		movlps	xmm7,[esi]			; camera position
		movss	xmm2,[esi+8]
		movlhps	xmm7,xmm2


		mov		ecx,dword ptr[dwFacesNum]

		mov		edi,dword ptr[pIndexBufferEntry]
		mov		esi,dword ptr[pwFaceIndexEntry]
		mov		ebx,dword ptr[pFaceEntry]
		

lb_loop:
		push	TRI_FACE_SIZE
		pop		edx

		
		movzx	eax,word ptr[esi]
		mul		edx

		add		eax,ebx								; pFaceEntry[pwFaceIndexEntry[i]]
		
		movaps	xmm2,xmm7							; restore camera position
		movups	xmm5,[eax+TRI_FACE_NORMAL_OFFSET]	; load face normal
		

		mulps	xmm2,xmm5
		
		movhlps xmm3,xmm2	
		addss	xmm3,xmm2	// xmm[0-31] = x+z

		movaps	xmm4,xmm2
		shufps	xmm4,xmm4,1	// xmm [0-31] = y
				
		addss	xmm3,xmm4
		addss	xmm3,dword ptr[eax+TRI_FACE_D_OFFSET]

		comiss	xmm3,xmm6					; if cosang <= 0.0f skip
		jb		lb_next

		mov		edx,dword ptr[eax+TRI_FACE_INDEX_OFFSET]	; read index 0,1
		mov		dword ptr[edi],edx							; write index 0,1
		mov		dx,word ptr[eax+TRI_FACE_INDEX_OFFSET+4]	; read index 2
		mov		word ptr[edi+4],dx							; write index 2
		
		inc		dword ptr[dwCount]
		add		edi,2*3

lb_next:
		add		esi,2
		loop	lb_loop
	}
	return dwCount;
}
#else
GLOBAL_FUNC_DLL DWORD __stdcall BuildIndexBufferWithFaceIndexList(WORD* pIndexBufferEntry,TRI_FACE* pFaceEntry,WORD* pwFaceIndexEntry,VECTOR3* pv3Eye,DWORD dwFacesNum)
{
	DWORD dwCount;
	dwCount = 0;
	for (DWORD i=0; i<dwFacesNum; i++)
	{
		if (0 > (DotProduct(&pFaceEntry[pwFaceIndexEntry[i]].plane.v3Up,pv3Eye) + pFaceEntry[pwFaceIndexEntry[i]].plane.D))
			continue;

		pIndexBufferEntry[dwCount*3+0] = pFaceEntry[pwFaceIndexEntry[i]].wIndex[0];
		pIndexBufferEntry[dwCount*3+1] = pFaceEntry[pwFaceIndexEntry[i]].wIndex[1];
		pIndexBufferEntry[dwCount*3+2] = pFaceEntry[pwFaceIndexEntry[i]].wIndex[2];
		dwCount++;
	}
	return dwCount;
}
#endif

GLOBAL_FUNC_DLL void __stdcall BuildFaceList(TRI_FACE* pFacesEntry,VECTOR3* pv3,WORD* pIndex,DWORD dwFacesNum)
{
	VECTOR3	center;

	VECTOR3	v3Tri[3];
	for (DWORD i=0; i<dwFacesNum; i++)
	{
		v3Tri[0] = pv3[pIndex[i*3+0]];
		v3Tri[1] = pv3[pIndex[i*3+1]];
		v3Tri[2] = pv3[pIndex[i*3+2]];

		CalcPlaneEquation(&pFacesEntry[i].plane,v3Tri);
		
//		if (!IsValidTri(&v3Tri[0],&v3Tri[1],&v3Tri[2]))
//			__asm int 3
		VECTOR3_ADD_VECTOR3(&center,v3Tri+0,v3Tri+1);
		VECTOR3_ADDEQU_VECTOR3(&center,v3Tri+2);
		VECTOR3_DIV_FLOAT(&pFacesEntry[i].v3Point,&center,3.0f);
		
		pFacesEntry[i].wIndex[0] = pIndex[i*3+0];
		pFacesEntry[i].wIndex[1] = pIndex[i*3+1];
		pFacesEntry[i].wIndex[2] = pIndex[i*3+2];
		pFacesEntry[i].wIndex[3] = 0xffff;
	}
}


GLOBAL_FUNC_DLL BOOL __stdcall MakeViewVolumeIncludingSphere(VIEW_VOLUME* pViewVolume,VECTOR3* pv3From, BOUNDING_SPHERE* pBS, float fFar)
{
	VECTOR3		To, Right;
	VECTOR3_SUB_VECTOR3( &To, &(pBS->v3Point), pv3From);
	float		fTo		=	VECTOR3Length( &To);

	if( fTo <= pBS->fRs)
	{
		return	FALSE;
	}

	VECTOR3		Up;
	VECTOR3_DIV_FLOAT( &To, &To, fTo);
	if( To.y > 0.9f || To.y < -0.9f)			
	{
		Up.x	=	1.0f;
		Up.y	=	0.0f;
	}
	else
	{
		Up.x	=	0.0f;
		Up.y	=	1.0f;
	}
	Up.z	=	0.0f;
	CrossProduct( &Right, &Up, &To);
	CrossProduct( &Up, &To, &Right);

	float	fFov	=	float(asin( (pBS->fRs)/fTo));

	MakeViewVolume( pViewVolume, pv3From, &(pBS->v3Point), &Up, fFov, 1.0f, fFar);
	return	TRUE;
}

GLOBAL_FUNC_DLL void __stdcall SetRotationXMatrix(MATRIX4* pMat, float fRad)
{

    SetIdentityMatrix(pMat);
    pMat->_22 =  (float)cos( fRad );
    pMat->_23 =  (float)sin( fRad );
    pMat->_32 = -(float)sin( fRad );
    pMat->_33 =  (float)cos( fRad );
}


GLOBAL_FUNC_DLL void __stdcall SetRotationYMatrix(MATRIX4* pMat, float fRad)
{

    SetIdentityMatrix(pMat);
    pMat->_11 =  (float)cos( fRad );
    pMat->_13 = -(float)sin( fRad );
    pMat->_31 =  (float)sin( fRad );
    pMat->_33 =  (float)cos( fRad );
}

GLOBAL_FUNC_DLL void __stdcall SetRotationZMatrix(MATRIX4* pMat, float fRad)
{

    SetIdentityMatrix(pMat);
    pMat->_11  =  (float)cos( fRad );
    pMat->_12  =  (float)sin( fRad );
    pMat->_21  = -(float)sin( fRad );
    pMat->_22  =  (float)cos( fRad );

}
GLOBAL_FUNC_DLL void __stdcall QuaternionFromRotation(QUARTERNION* pQ,VECTOR3* v, float fTheta )
{
	float sinValue = (float)sin(fTheta/2);
    pQ->x = sinValue * v->x;
    pQ->y = sinValue * v->y;
    pQ->z = sinValue * v->z;
    pQ->w = (float)cos(fTheta/2);
}
GLOBAL_FUNC_DLL void __stdcall QuaternionMultiply(QUARTERNION* pQResult,QUARTERNION* pQA,QUARTERNION* pQB)
{
	float Dx,Dy,Dz,Dw;
	
    Dx = pQB->w*pQA->x + pQB->x*pQA->w + pQB->y*pQA->z - pQB->z*pQA->y;
    Dy = pQB->w*pQA->y + pQB->y*pQA->w + pQB->z*pQA->x - pQB->x*pQA->z;
    Dz = pQB->w*pQA->z + pQB->z*pQA->w + pQB->x*pQA->y - pQB->y*pQA->x;
    Dw = pQB->w*pQA->w - pQB->x*pQA->x - pQB->y*pQA->y - pQB->z*pQA->z;

	pQResult->x = Dx;
	pQResult->y = Dy;
	pQResult->z = Dz;
	pQResult->w = Dw;

}
GLOBAL_FUNC_DLL void __stdcall DWORDtoCOLOR(DWORD color,float* r,float* g,float* b,float* a)
{

	DWORD	A = color>>24 & 0x000000ff;
	DWORD	R = color>>16 & 0x000000ff;
	DWORD	G = color>>8 & 0x000000ff;
	DWORD	B = color & 0x000000ff;
	
	*a =  (float)A / 255.0f;
	*r =  (float)R / 255.0f;
	*g =  (float)G / 255.0f;
	*b =  (float)B / 255.0f;
}
/*
GLOBAL_FUNC_DLL DWORD __stdcall ClipLightWithRS(LIGHT_DESC* pLightDescResult,COLLISION_MESH_OBJECT_DESC* pColMeshDesc,LIGHT_DESC* pLightDescIn,DWORD dwLightNum)
{
	DWORD		dwLightNumResult = 0;
	float		distance;

	for (DWORD i=0; i<dwLightNum; i++)
	{
		distance = CalcDistance(&pColMeshDesc->boundingSphere.v3Point,&pLightDescIn[i].v3Point);
		if (distance < pColMeshDesc->boundingSphere.fRs + pLightDescIn[i].fRs )
		{
			pLightDescResult[dwLightNumResult] = pLightDescIn[i];
			dwLightNumResult++;
		}
	}
	return dwLightNumResult;

}
*/	
GLOBAL_FUNC_DLL DWORD __stdcall ClipLightWithRS(LIGHT_INDEX_DESC* pbLightIndexArrayResult,COLLISION_MESH_OBJECT_DESC* pColMeshDesc,LIGHT_DESC* pLightDescIn,DWORD dwLightNum,BYTE bStartIndex)
{
	DWORD		dwLightNumResult = 0;
	float		distance;

	for (DWORD i=0; i<dwLightNum; i++)
	{
		distance = CalcDistance(&pColMeshDesc->boundingSphere.v3Point,&pLightDescIn[i].v3Point);
		if (distance < pColMeshDesc->boundingSphere.fRs + pLightDescIn[i].fRs )
		{
			pbLightIndexArrayResult[dwLightNumResult].bLightIndex = (BYTE)i+bStartIndex;
			pbLightIndexArrayResult[dwLightNumResult].pMtlHandle = pLightDescIn[i+bStartIndex].pMtlHandle;
			
			dwLightNumResult++;
		}
	}
	return dwLightNumResult;

}


// 면:선분.
// 평면 p와 만나는 선분 lv1, lv2를 주면, 
// 리턴값은 COLLISIONTYPE이고, 만나는 선분rv1,rv2를 리턴값으로 하고 그때의 t값이.... 복잡하군.
GLOBAL_FUNC_DLL BOOL __stdcall CalcIntersectPointLineAndPlane(VECTOR3* pv3Result,PLANE* pPlane,VECTOR3* pv3From,VECTOR3* pv3To, float* pft)
{
	// 선분과 평면이 교차하는 경우.
	// 직선 방정식과 평면방정식을 대입해 t를 찾아내고 실제 위치 버텍스를 찾아낸다.
	VECTOR3		v3Line;
	float		t,fUnder;
	BOOL		bResult = FALSE;

	
	
	VECTOR3_SUB_VECTOR3(&v3Line,pv3To,pv3From);
	fUnder = DotProduct(&pPlane->v3Up,&v3Line);

	if (fUnder == 0.0f)
		goto lb_return;

	t	=	-1.0f * (DotProduct(&pPlane->v3Up,pv3From)+ pPlane->D   ) / fUnder;		// 리턴값 t

	pv3Result->x = pv3From->x + t * v3Line.x;
	pv3Result->y = pv3From->y + t * v3Line.y;
	pv3Result->z = pv3From->z + t * v3Line.z;

	

	*pft = t;
	bResult = TRUE;

lb_return:
	return bResult;

}


GLOBAL_FUNC_DLL BOOL __stdcall ResizeImage(char* pDest,DWORD dwDestWidth,DWORD dwDestHeight,char* pSrc,DWORD dwSrcWidth,DWORD dwSrcHeight,DWORD dwBytesPerPixel)
{
	DWORD	px1,py1;
	float	dx = (float)dwDestWidth / (float)dwSrcWidth;
	float	dy = (float)dwDestHeight / (float)dwSrcHeight;
	
	char*	pDestTemp;
	char*	pSrcTemp;

	for (DWORD y=0; y<(DWORD)dwSrcHeight; y++)
	{		
		py1 = (DWORD)(dy * (float)y);
		
		for (DWORD x=0; x<(DWORD)dwSrcWidth; x++)
		{
			px1 = (DWORD)(dx * (float)x);
			
			pDestTemp = pDest + (px1 + dwDestWidth*py1)*dwBytesPerPixel;
			pSrcTemp = pSrc + (x + y*dwSrcWidth)*dwBytesPerPixel;

		//	*(WORD*)(p+px1*2 + dwWidth*2*py1) = *(WORD*)(m_pRawImage + x*2 + y*m_dwWidth*2);
		
			for (DWORD i=0; i<dwBytesPerPixel; i++)
				*(pDestTemp+i) = *(pSrcTemp+i);
				
		}
	}
	return TRUE;
}
GLOBAL_FUNC_DLL BOOL __stdcall ClipRenderObjectWithViewVolume(VIEW_VOLUME* pVolume,COLLISION_MESH_OBJECT_DESC* pColMeshObjDesc,DWORD dwFlag)
{
	//m_bViewVolumeClip = FALSE;

	BOOL		bResult = FALSE;
	float		D = 0,fDist;
	VECTOR3		v3Dist;

	if (!pColMeshObjDesc)
	{
		bResult = TRUE;
		goto lb_return;
	}


	//뷰볼륨 클리핑을 한다.
	/////////////////////////////////////////////////////////////////////////////
	//	Code by myself97
	/////////////////////////////////////////////////////////////////////////////
	
	// 뷰볼륨 거리 클리핑.
	VECTOR3_SUB_VECTOR3(&v3Dist,&pVolume->From,&pColMeshObjDesc->boundingSphere.v3Point);

	fDist = VECTOR3Length(&v3Dist);
	if( pColMeshObjDesc->boundingSphere.fRs + pVolume->fFar < fDist )	
		goto lb_return;


	DWORD	j,i;
	// 뷰볼륨 클리핑.
	for( j = 0; j < 4; j++)
	{
		// 바운딩 스피어.
		D = DotProduct(&pVolume->Plane[j].v3Up,&pColMeshObjDesc->boundingSphere.v3Point) + pVolume->Plane[j].D;

		if( D > pColMeshObjDesc->boundingSphere.fRs)	
			goto lb_return;
			
			if (!CalcDistance(&pColMeshObjDesc->boundingBox.v3Oct[0],&pColMeshObjDesc->boundingBox.v3Oct[4]))
				goto Label1;

			// 바운딩 박스.
			for( i = 0; i < 8; i++)
			{
				D = DotProduct(&pVolume->Plane[j].v3Up,&pColMeshObjDesc->boundingBox.v3Oct[i]) + pVolume->Plane[j].D;

				// 하나라도 plane내에 들어가면 pass 아니면 return FALSE;
				if( D < 0)	
					goto Label1;
			}
			goto lb_return;
	Label1:
			_asm nop
	}
lb_exit:


	bResult = TRUE;

lb_return:
	return bResult;
}
GLOBAL_FUNC_DLL BOOL __stdcall ClipVertexWithViewVolume(VIEW_VOLUME* pVolume,VECTOR3* pv3Point)
{
	//m_bViewVolumeClip = FALSE;

	BOOL		bResult = FALSE;
	float		D = 0;
	
	DWORD	j;
	
	// 뷰볼륨 클리핑.
	for( j = 0; j < 4; j++)
	{
		D = DotProduct(&pVolume->Plane[j].v3Up,pv3Point) + pVolume->Plane[j].D;

		if( D > 0)	
			goto lb_return;
	}
	bResult = TRUE;

lb_return:
	return bResult;
}
#ifdef _USE_SSE
// 주의사항...반드시 BYTE*갯수+4만큼의 메모리를 넣어줄것.
GLOBAL_FUNC_DLL void __stdcall CreateViewPortFlag(BYTE* pbFlag,VECTOR4* pv4TransformedList,DWORD dwVerticesNum)
{
	float	f0 = 0.0f;
	float	f1 = 1.0f;
	
	DWORD	dwTemp[8];

	__asm
	{
		xorps			xmm6,xmm6

		movss			xmm5,dword ptr[f1]
		movss			xmm4,dword ptr[f0]
		shufps			xmm5,xmm5,0
		movhlps			xmm4,xmm5
		subps			xmm6,xmm4

		lea				ebx,dword ptr[dwTemp]
		mov				ecx,dword ptr[dwVerticesNum]
		mov				esi,dword ptr[pv4TransformedList]

lb_loop_vertices:

		mov				edi,ebx						; dwTemp
		movups			xmm0,[esi]					; src vector4 

		; w로 나눈다
		movaps			xmm2,xmm0				
		shufps			xmm2,xmm2,255
		shufps			xmm0,xmm5,164				;   1 |  1  |  y  |  x
		divps			xmm0,xmm2					; 1/w | z/w | y/w | x/w
		shufps			xmm0,xmm0,52				; xmm0 = ? | w | y | x
        movaps			xmm7,xmm0					; backup

		; 비교부분
		movaps			xmm4,xmm5

		cmpps			xmm0,xmm4,2		; 상수보다 크지 않아야 한다.조건을 만족하면 true, 아니면 false
		movups			[edi],xmm0
		
		add				edi,12
	
		movaps			xmm0,xmm6
		movaps			xmm4,xmm7

		cmpps			xmm0,xmm4,2		; 상수보다 커야한다.조건을 만족하면 true, 아니면 false
		movups			[edi],xmm0

		; 

		; dwTemp		| ? | x | x | x | ? | x | x | x |
		mov				edi,ebx			; dwTemp

		; 결과를 저장할 edx레지스터
		xor				edx,edx

		mov				eax,dword ptr[edi]
		inc				eax
		or				edx,eax
		add				edi,4
		shl				edx,1
		

		mov				eax,dword ptr[edi]
		inc				eax
		or				edx,eax
		add				edi,4
		shl				edx,1
		

		mov				eax,dword ptr[edi]
		inc				eax
		or				edx,eax
		add				edi,4
		shl				edx,1
		

		mov				eax,dword ptr[edi]
		inc				eax
		or				edx,eax
		add				edi,4
		shl				edx,1
		

		mov				eax,dword ptr[edi]
		inc				eax
		or				edx,eax
		add				edi,4
		shl				edx,1
		

		mov				eax,dword ptr[edi]
		inc				eax
		or				edx,eax
		add				edi,4
		shl				edx,1
		
//		RDTSC_BEGIN

		; 버텍스당 flag 저장 
		mov				edi,dword ptr[pbFlag]
		mov				dword ptr[edi],edx
		inc				edi
		mov				dword ptr[pbFlag],edi

//		RDTSC_END


		add				esi,16
		sub				ecx,1
		jnz				lb_loop_vertices
	}
}
#else
GLOBAL_FUNC_DLL void __stdcall CreateViewPortFlag(BYTE* pbFlag,VECTOR4* pv4TransformedList,DWORD dwVerticesNum)
{
	VECTOR4			v4Point;
		
	for (DWORD i=0; i<8; i++)
	{
		v4Point.x = pv4TransformedList[i].x / pv4TransformedList[i].w;
		v4Point.y = pv4TransformedList[i].y / pv4TransformedList[i].w;
		v4Point.z = pv4TransformedList[i].z / pv4TransformedList[i].w;
		v4Point.w = 1.0f / pv4TransformedList[i].w;


		if (v4Point.x > 1.0f )
			pbFlag[i] |= 0x01;
			
		if (v4Point.y > 1.0f )
			pbFlag[i] |= 0x02;

		if (v4Point.x < -1.0f )
			pbFlag[i] |= 0x04;

		if (v4Point.y < -1.0f )
			pbFlag[i] |= 0x08;

		if (v4Point.w < 0.0f )
			pbFlag[i] |= 0x10;
			
		if (v4Point.w > 1.0f )
			pbFlag[i] |= 0x20;
	}
}
#endif

GLOBAL_FUNC_DLL BOOL __stdcall ClipBoundingBoxWithViewProjMatrix(MATRIX4* pMatViewProj,COLLISION_MESH_OBJECT_DESC* pColMeshObjDesc,DWORD dwFlag)
{
	BOOL		bResult = FALSE;
	VECTOR4		v4PointOct[8];
	BYTE		bFlag[8 + 4];

	*(DWORD*)&bFlag[0] = 0x00000000;
	*(DWORD*)&bFlag[4] = 0x00000000;

	TransformV3TOV4(v4PointOct,pColMeshObjDesc->boundingBox.v3Oct,pMatViewProj,8);
	CreateViewPortFlag(bFlag,v4PointOct,8);
	

	DWORD	dwLineIndexList[12][2] = {
		// x축 정렬
		0,3,
		1,2,
		5,6,
		4,7,

		// y축 정렬
		1,0,
		2,3,
		6,7,
		5,4,
				
		// z축 정렬
		4,0,
		5,1,
		6,2,
		7,3
	};
	BYTE		bEdgeFlag[12];
	*(DWORD*)&bEdgeFlag[0] = 0x00000000;
	*(DWORD*)&bEdgeFlag[4] = 0x00000000;
	*(DWORD*)&bEdgeFlag[8] = 0x00000000;

	DWORD		dwCount = 0;
	bool		bTest;
	DWORD	i;
	for (i=0; i<12; i++)
	{
		bTest = (bool)(	bEdgeFlag[i] = bFlag[dwLineIndexList[i][0]] & bFlag[dwLineIndexList[i][1]] );
		dwCount += 	(DWORD)bTest;
	}
	if (dwCount != 12)
	{
		bResult = TRUE;
	}
	else
	{
		__asm nop
	}
	return bResult;
}





GLOBAL_FUNC_DLL void __stdcall CalcXZ(float* px,float* pz,DWORD dwPosX,DWORD dwPosY,float top,float left,float bottom,float right,float width,float height,DWORD dwFacesNumX,DWORD dwFacesNumY)
{
	DWORD		dwIndex;
	
	dwIndex = dwPosY*(dwFacesNumX+1)+dwPosX;
	*px = (float)dwPosX/(float)dwFacesNumX * width + left;
	*pz = (float)dwPosY/(float)dwFacesNumY * height + top;
}

GLOBAL_FUNC_DLL void __stdcall TransposeMatrix(MATRIX4* pMat)
{
	float t_12,t_13,t_14,t_23,t_24,t_34;

	t_12=pMat->_12;
	t_13=pMat->_13;
	t_14=pMat->_14;
	t_23=pMat->_23;
	t_24=pMat->_24;
	t_34=pMat->_34;

	pMat->_12=pMat->_21;
	pMat->_13=pMat->_31;
	pMat->_14=pMat->_41;
	pMat->_23=pMat->_32;
	pMat->_24=pMat->_42;
	pMat->_34=pMat->_43;	
	
	pMat->_21=t_12;
	pMat->_31=t_13;
	pMat->_41=t_14;
	pMat->_32=t_23;
	pMat->_42=t_24;
	pMat->_43=t_34;	
}
GLOBAL_FUNC_DLL void __stdcall SetLightTexMatrix(MATRIX4* pMat,VECTOR3* pv3Pos,float fRs)
{
	float		one_div_2LRs = 1.0f/(2.0f*fRs);

	SetIdentityMatrix(pMat);
	pMat->_11 = one_div_2LRs;
	pMat->_22 = one_div_2LRs;
	pMat->_33 = one_div_2LRs;
	pMat->_41 = (0.5f) - (pv3Pos->x*one_div_2LRs);
	pMat->_42 = (0.5f) - (pv3Pos->y*one_div_2LRs);
	pMat->_43 = (0.5f) - (pv3Pos->z*one_div_2LRs);
}

GLOBAL_FUNC_DLL BOOL __stdcall SetViewMatrix(MATRIX4* pMat,VECTOR3* pv3From,VECTOR3* pv3To,VECTOR3* pv3Up)
{
    // Get the z basis vector, which points straight ahead. This is the
    // difference from the eyepoint to the lookat point.
	
	BOOL	bResult = FALSE;

	VECTOR3	v3View;
	VECTOR3	v3Right;
	VECTOR3	v3Up;
	VECTOR3	v3Temp;

	FLOAT fDotProduct;

//  D3DVECTOR vView = vAt - vFrom;
	VECTOR3_SUB_VECTOR3(&v3View,pv3To,pv3From);

	float	fLength = VECTOR3Length(&v3View);
    if( fLength < 1e-6f )
		goto lb_return;
	
//	FLOAT fLength = Magnitude( vView );
//	Normalize the z basis vector
//  vView /= fLength;

	VECTOR3_DIVEQU_FLOAT(&v3View,fLength);

    // Get the dot product, and calculate the projection of the z basis
    // vector onto the up vector. The projection is the y basis vector.
    fDotProduct = DotProduct(pv3Up,&v3View);

 
//	D3DVECTOR vUp = vWorldUp - fDotProduct * vView;
	
	VECTOR3_MUL_FLOAT(&v3Temp,&v3View,fDotProduct);
	VECTOR3_SUB_VECTOR3(&v3Up,pv3Up,&v3Temp);


    // If this vector has near-zero length because the input specified a
    // bogus up vector, let's try a default up vector

	fLength = VECTOR3Length(&v3Up);

    if( 1e-6f > fLength)
    {
		//vUp = D3DVECTOR( 0.0f, 1.0f, 0.0f ) - vView.y * vView;
		VECTOR3_MUL_FLOAT(&v3Temp,&v3View,v3View.y);
		v3Up.x = 0.0f;
		v3Up.y = 1.0f;
		v3Up.z = 0.0f;
		VECTOR3_SUBEQU_VECTOR3(&v3Up,&v3Temp);

        // If we still have near-zero length, resort to a different axis.
		fLength = VECTOR3Length(&v3Up);
        if( 1e-6f > fLength)
        {
			//vUp = D3DVECTOR( 0.0f, 0.0f, 1.0f ) - vView.z * vView;
			VECTOR3_MUL_FLOAT(&v3Temp,&v3View,v3View.z);
			v3Up.x = 0.0f;
			v3Up.y = 0.0f;
			v3Up.z = 1.0f;
			VECTOR3_SUBEQU_VECTOR3(&v3Up,&v3Temp);

			fLength = VECTOR3Length(&v3Up);
			
			if( 1e-6f > fLength)
				goto lb_return;
        }
    }

    // Normalize the y basis vector
//    vUp /= fLength;
	VECTOR3_DIVEQU_FLOAT(&v3Up,fLength);
	
    // The x basis vector is found simply with the cross product of the y
    // and z basis vectors
//    D3DVECTOR vRight = CrossProduct( vUp, vView );
	
	CrossProduct(&v3Right,&v3Up,&v3View);


    // Start building the matrix. The first three rows contains the basis
    // vectors used to rotate the view to point at the lookat point
	SetIdentityMatrix(pMat);
//    D3DUtil_SetIdentityMatrix( mat );
    pMat->_11 = v3Right.x;    pMat->_12 = v3Up.x;    pMat->_13 = v3View.x;
    pMat->_21 = v3Right.y;    pMat->_22 = v3Up.y;    pMat->_23 = v3View.y;
    pMat->_31 = v3Right.z;    pMat->_32 = v3Up.z;    pMat->_33 = v3View.z;

    // Do the translation values (rotations are still about the eyepoint)
    pMat->_41 = - DotProduct(pv3From,&v3Right );
    pMat->_42 = - DotProduct(pv3From,&v3Up );
    pMat->_43 = - DotProduct(pv3From,&v3View);
	bResult = TRUE;

lb_return:
	return bResult;
}
GLOBAL_FUNC_DLL BOOL __stdcall SetProjectionMatrix(MATRIX4* pMat,float fFOV,float fAspect,float fNearPlane,float fFarPlane)
{
	BOOL	bResult = FALSE;

	float cos_fov_div_2;
	float sin_fov_div_2;
	float w,h,Q;
	
    if( fabs(fFarPlane-fNearPlane) < 0.01f )
		goto lb_return;

    if( fabs(sin(fFOV/2.0f)) < 0.01f )
		goto lb_return;

	cos_fov_div_2 = cosf(fFOV/2.0f);
	sin_fov_div_2 = sinf(fFOV/2.0f);

	w = fAspect * ( cos_fov_div_2/sin_fov_div_2 );
    h = 1.0f  * ( cos_fov_div_2/sin_fov_div_2 );
	Q = fFarPlane / ( fFarPlane - fNearPlane );

   // FLOAT w = fAspect * ( cosf(fFOV/2)/sinf(fFOV/2) );
  //  FLOAT h =   1.0f  * ( cosf(fFOV/2)/sinf(fFOV/2) );
  //  FLOAT Q = fFarPlane / ( fFarPlane - fNearPlane );

    ZeroMemory(pMat,sizeof(MATRIX4));
    pMat->_11 = w;
    pMat->_22 = h;
    pMat->_33 = Q;
    pMat->_34 = 1.0f;
    pMat->_43 = -Q*fNearPlane;

	bResult = TRUE;

lb_return:
	return bResult;
}


/*
	From To로 들어온 시점을 라디안 단위의 Yaw,Pitch,Roll 각으로 바꾼다.
	FromTo는 '절대로' 완전히 위를 보고있거나(y=1.0f), 완전히 아래를 보고있지(y=-1.0f) 않다고 가정한다.
	Up은 위의 가정 하에서 언제나 (0,1,0)이다. 즉 Roll은 0이다(pOutYPR->z = 0.0f).
*/
GLOBAL_FUNC_DLL void __stdcall ConvertFromTo2YPR(VECTOR3* pVEC, VECTOR3* pOutYPR)
{
	VECTOR3		To;
	Normalize( &To, pVEC);
#ifdef	_DEBUG
	// 완전히 위를 보고 있거나 완전히 아래를 보고 있는 예외상황을 집어낸다.
	if( To.y > 0.999f || To.y < -0.999f)
	{
		_asm int 3;		// 절대 생겨선 안된다.
	}
#endif
	if( To.y > 0.999f)
	{
		pOutYPR->x	=	0.0f;
		pOutYPR->y	=	-1.0f * PI/2.0f;
		pOutYPR->z	=	0.0f;
		return	;
	}
	if( To.y < -0.999f)
	{
		pOutYPR->x	=	0.0f;
		pOutYPR->y	=	PI/2.0f;
		pOutYPR->z	=	0.0f;
		return	;
	}

	// 요
	pOutYPR->x	=	(float)atan2( To.x, To.z) ;
	if( pOutYPR->x >= 2.0f*PI)	pOutYPR->x -= 2.0f*PI;
	if( pOutYPR->x < 0.0f)		pOutYPR->x += 2.0f*PI;


	// 피치.
	float	fSin	=	To.y;		// To의 길이는 1이다. 
	pOutYPR->y		=	-1.0f * (float)asin( fSin);		// -1을 곱해준건 수학함수가 오른손 좌표계로 값을 내기때문에 왼손좌표계로 보정한것이다.

	// 롤.
	pOutYPR->z		=	0.0f;
}


// 위치만.
GLOBAL_FUNC_DLL	void __stdcall RotatePositionWithPivot( VECTOR3* pOutPos, VECTOR3* pPivot, VECTOR3* pPosition, VECTOR3* pCameraAnglePYR, float fRotateYaw, float fRotatePitch)
{
	
	// 일단 피봇을 원점으로 옮기고,
	VECTOR3		ToFrom;
	VECTOR3_SUB_VECTOR3( &ToFrom, pPosition, pPivot);
	float		fToFrom	=	VECTOR3Length( &ToFrom);

	MATRIX4	mRotate;
	// 먼저 pitch
	// 카메라의 요나 롤을 이용해 회전축을 먼저 구한다.
	//pCameraAngleYPR->x);
	VECTOR3	Camera2DView;
	Camera2DView.x	=	0.0f;
	Camera2DView.y	=	0.0f;
	Camera2DView.z	=	1.0f;
//	if( pCameraAnglePYR->x >= (PI-0.01)/2.0f)	// 카메라가 아래를 보고 있을 경우.
	if( pCameraAnglePYR->x >= (PI)/2.0f)	// 카메라가 아래를 보고 있을 경우.
	{
		goto	lbRotateYaw;
	}
//	else if( pCameraAnglePYR->x <= -1.0f * (PI-0.01)/2.0f)	// 카메라가 아래를 보고 있을 경우.
	else if( pCameraAnglePYR->x <= -1.0f * (PI)/2.0f)	// 카메라가 아래를 보고 있을 경우.
	{
		goto	lbRotateYaw;
	}
	else // 일반적인 경우.
	{
		SetRotationYMatrix( &mRotate, pCameraAnglePYR->y);
	}

	TransformVector3_VPTR1( &Camera2DView, &mRotate, 1);
//	if( pCameraAngleYPR->x <= 0.0001f && pCameraAngleYPR->x >= -0.0001f) // 거의 앞을 보고 있을 경우.

	VECTOR3	RotateAxis;
//	VECTOR3_MUL_FLOAT( &RotateAxis, &Camera2DView, -1.0f);
	RotateAxis.x	=	Camera2DView.z;
	RotateAxis.y	=	0.0f;
	RotateAxis.z	=	-1.0f * Camera2DView.x;
	
	SetRotationMatrix( &mRotate, &RotateAxis, fRotatePitch);
	TransformVector3_VPTR1( &ToFrom, &mRotate, 1);

lbRotateYaw:
	// 그다음 yaw.
	SetRotationYMatrix( &mRotate, fRotateYaw);
	TransformVector3_VPTR1( &ToFrom, &mRotate, 1);


	// 다시 피봇을 원위치 시킨뒤,
	VECTOR3_ADD_VECTOR3( pOutPos, pPivot, &ToFrom);

	// 함수 끝내다.
	_asm nop;
}
GLOBAL_FUNC_DLL	float	__stdcall DistanceFromSphereAndPlane( BOUNDING_SPHERE* pSphere, PLANE* pPlane)
{
	float	fDist	=	DotProduct( &(pPlane->v3Up), &(pSphere->v3Point)) + pPlane->D;
	return	fDist;
}

GLOBAL_FUNC_DLL	BOOL	__stdcall IsCollisionSphereAndBox( BOUNDING_SPHERE* pSphere, BOUNDING_BOX* pBox)
{
	BOOL	bResult	=	FALSE;
	float	fDistance;
	int		i;
	PLANE	p;
	VECTOR3		v[3];
	BYTE		bIndex[18] = 
	{
		7,3,2,
		0,4,1,
		0,3,7,
		1,5,2,
		0,1,3,
		4,7,6
	};
	for( i = 0; i < 6; i++)
	{
		v[0]	=	pBox->v3Oct[bIndex[i*3+0]];
		v[1]	=	pBox->v3Oct[bIndex[i*3+1]];
		v[2]	=	pBox->v3Oct[bIndex[i*3+2]];
		CalcPlaneEquation( &p, v);
		fDistance	=	DistanceFromSphereAndPlane( pSphere, &p);
		if( fDistance > pSphere->fRs)
			return	FALSE;
	}
	return	TRUE;
	



/*

	// 어느 위치에 있는가.?
	unsigned char where	=	0;			
	// x축 기준으로, +, 0, -방향.
	for( i = 0; i < 3; i++)
	{
		fDistance	=	DistanceFromSphereAndPlane( pSphere, &(p[i]));
		if( fDistance > 0.0f) where	=	where | ((unsigned char)1 << i);
	}
	switch( where)
	{
		case	0x20;
			break;
		case	
	}
*/

	return	bResult;
}

GLOBAL_FUNC_DLL DWORD __stdcall	RemoveExt(char* szResultName,char* szFileName)
{
	DWORD	dwLen = lstrlen(szFileName);
	DWORD	dwCount = 0;

	while (szFileName[dwCount] != '.')
	{
		if (dwCount >= dwLen)
			goto lb_return;

		szResultName[dwCount] = szFileName[dwCount];
		dwCount++;
	}
	szResultName[dwCount] = 0;

lb_return:
	return dwCount;
}
GLOBAL_FUNC_DLL DWORD __stdcall	GetExt(char* szResultName,char* szFileName)
{
	DWORD	dwLen = lstrlen(szFileName);
	DWORD	dwCount = 0;

	while (szFileName[dwCount] != '.')
	{
		if (dwCount >= dwLen)
			goto lb_return;

		dwCount++;
	}
	szFileName += dwCount;
	dwCount = 0;
	while (szFileName[dwCount] != 0)
	{
		szResultName[dwCount] = szFileName[dwCount];
		dwCount++;
	}
	szResultName[dwCount] = 0;

lb_return:
	return dwCount;

}
GLOBAL_FUNC_DLL DWORD __stdcall GetNamePath(char* szResultPath,char* szFileName)
{
	DWORD	dwLen;
	
	DWORD len_src = lstrlen(szFileName);
	DWORD count = 0;
	DWORD i=0;

	for (i=len_src; i>0; i--)
	{
		if (szFileName[i-1] == '\\' || szFileName[i-1] == '/')
		{
			memcpy(szResultPath,szFileName,i);
			dwLen = i;
			goto lb_return;
		}
		
	}
	dwLen = 0;
	

lb_return:
	szResultPath[dwLen] = 0;
	return dwLen;

}
GLOBAL_FUNC_DLL DWORD __stdcall GetNameRemovePath(char* dest,char* src)
{
	
	if (!src || !dest)
		return 0;

	DWORD	dwLen;
	

	DWORD len_src = lstrlen(src);
	DWORD count = 0;
	DWORD i=0;

	for (i=len_src; i>0; i--)
	{
		if (src[i-1] == '\\' || src[i-1] == '/')
		{
			memcpy(dest,&src[i],len_src-i);
			dwLen = len_src-i;
			goto lb_return;
		}
		
	}
	memcpy(dest,src,len_src);
	dwLen = len_src;

lb_return:
	dest[dwLen] = 0;
	return dwLen;

}
GLOBAL_FUNC_DLL BOOL __stdcall WriteTGA(char* szFileName,char* pSrc,DWORD dwWidth,DWORD dwHeight,DWORD dwPitch,DWORD bpp)
{
	FILE*	fp = fopen(szFileName,"wb");
	if (!fp)
		return FALSE;

//	static DWORD count=0;
//	count++;
//	char txt[255];
//	wsprintf(txt,"hoho%d.%s",count,"tga");
	TGA_HEADER	header;
	memset(&header,0,sizeof(header));
	
	
	header.width = (WORD)dwWidth;
	header.height = (WORD)dwHeight;
	header.Bits = 24;
	header.ImageType = 2;
	
	BYTE		r;
	BYTE		g;
	BYTE		b;
	fwrite(&header,sizeof(header),1,fp);
	
	DWORD	dwSize = dwWidth*dwHeight*3;
	char* pBits = new char[dwSize];
	char* pDest = pBits;
	
	int	x,y;

	if (bpp == 16)
	{
		WORD		wPixel;
		for (y=dwHeight-1; y>=0; y--)
		{
			for (x=0; x<dwWidth; x++)
			{
				wPixel = *(WORD*)(pSrc + (x<<1) + dwPitch*y);
				r = (wPixel & 0x001f) << 3;
				g = (wPixel & 0x07e0) >> 5 << 2; 
				b = (wPixel & 0xf800) >> 11 << 3;

				*(pDest+0) = r;
				*(pDest+1) = g;
				*(pDest+2) = b;
				/*
				fwrite(&r,1,1,fp);
				fwrite(&g,1,1,fp);
				fwrite(&b,1,1,fp);
				*/

				pDest += 3;
				
			}
		}
	}
	else if (bpp == 32)
	{
		for (y=dwHeight-1; y>=0; y--)
		{
			for (x=0; x<dwWidth; x++)
			{
				
				*(pDest+0) = *((pSrc + (x<<2) + dwPitch*y)+0);
				*(pDest+1) = *((pSrc + (x<<2) + dwPitch*y)+1);
				*(pDest+2) = *((pSrc + (x<<2) + dwPitch*y)+2);

				pDest += 3;
			}
		}
	}
	else if (bpp == 24)
	{
		for (y=dwHeight-1; y>=0; y--)
		{
			for (x=0; x<dwWidth; x++)
			{
				
				*(pDest+0) = *((pSrc + (x*3) + dwPitch*y)+0);
				*(pDest+1) = *((pSrc + (x*3) + dwPitch*y)+1);
				*(pDest+2) = *((pSrc + (x*3) + dwPitch*y)+2);

				pDest += 3;
			}
		}
	

	}
	fwrite(pBits,sizeof(char),dwSize,fp);

	delete [] pBits;

	fclose(fp);

	return TRUE;
}

GLOBAL_FUNC_DLL __declspec(naked) void __stdcall GetEIP(DWORD* pdwEIP)
{
	__asm
	{
		push	eax
		push	edi

		mov		eax,dword ptr[esp+8]
		mov		edi,dword ptr[esp+4+8]
		mov		dword ptr[edi],eax
		pop		edi
		pop		eax
		ret		4

	}

}


GLOBAL_FUNC_DLL void __stdcall CharToSmallASCII(char* szDest,char* szStr,DWORD dwLen)
{
	__asm
	{
		mov		esi,dword ptr[szStr]	
		mov		edi,dword ptr[szDest]

		mov		ecx,dword ptr[dwLen]
		or		ecx,ecx
		jz		lb_exit

lb_replace:
		; 한 바이트 읽는다.
		mov		al,byte ptr[esi]
		
		; A와 Z사이에 있는지 검사..
		cmp		al,'A'
		jl		lb_skip

		cmp		al,'Z'
		ja		lb_skip

		; 소문자로 변환
		add		al,32
		mov		byte ptr[edi],al

lb_skip:
		inc		edi
		inc		esi

		loop	lb_replace
lb_exit:
	}

}
GLOBAL_FUNC_DLL void __stdcall CharToSmallASCIILen(char* szDest,char* szStr)
{
	__asm
	{
		mov		esi,dword ptr[szStr]	
		mov		edi,dword ptr[szDest]

lb_replace:
		; 한 바이트 읽는다.
		mov		al,byte ptr[esi]

		or		al,al
		jz		lb_exit
		
		; A와 Z사이에 있는지 검사..
		cmp		al,'A'
		jl		lb_skip

		cmp		al,'Z'
		ja		lb_skip

		; 소문자로 변환
		add		al,32
		mov		byte ptr[edi],al

lb_skip:
		inc		edi
		inc		esi
		jmp		lb_replace
lb_exit:

		
	}

}

#define DIV_64_MASK	0xC0C0C0C0

GLOBAL_FUNC_DLL void __stdcall Convert8BitTo2BitImage(char* p2Bits,char* p8Bits,DWORD dwWidth,DWORD dwHeight)
{
	__asm
	{
		mov		eax,dword ptr[dwWidth]
		mov		ebx,dword ptr[dwHeight]
		mov		esi,dword ptr[p8Bits]
		mov		edi,dword ptr[p2Bits]

		mul		ebx
		shr		eax,2					; width*height /4
		mov		ecx,eax

lb_loop:
		mov		eax,dword ptr[esi]
		xor		ebx,ebx					; 결과를 담을 레지스터
		and		eax,DIV_64_MASK			; 64로 나눈다.
		shr		eax,6
		
		or		bl,al
		ror		bl,2
		shr		eax,8

		or		bl,al
		ror		bl,2
		shr		eax,8

		or		bl,al
		ror		bl,2
		shr		eax,8

		or		bl,al
		ror		bl,2
		
		mov		byte ptr[edi],bl
		add		esi,4
		inc		edi

		sub		ecx,1
		jnz		lb_loop
	}
}

#define PIXEL_MASK_0	0x00000003

GLOBAL_FUNC_DLL void __stdcall Convert2BitTo8BitImage(char* p8Bits,char* p2Bits,DWORD dwWidth,DWORD dwHeight)
{
	// 00000011

	// 
	__asm
	{
		mov		eax,dword ptr[dwWidth]
		mov		ebx,dword ptr[dwHeight]
		mov		esi,dword ptr[p2Bits]
		mov		edi,dword ptr[p8Bits]

		mul		ebx
		shr		eax,2					; width*height /4
		mov		ecx,eax
		xor		edx,edx

lb_loop:	
		xor		ebx,ebx

		mov		dl,byte ptr[esi]
		mov		eax,edx

		and		eax,PIXEL_MASK_0
		or		ebx,eax
		shl		ebx,8
		shr		edx,2		

		mov		eax,edx
		and		eax,PIXEL_MASK_0
		or		ebx,eax
		shl		ebx,8
		shr		edx,2

		mov		eax,edx
		and		eax,PIXEL_MASK_0
		or		ebx,eax
		shl		ebx,8
		shr		edx,2

		mov		eax,edx
		and		eax,PIXEL_MASK_0
		or		ebx,eax

		shl		ebx,6

		mov		dword ptr[edi],ebx

		add		edi,4
		inc		esi

		sub		ecx,1
		jnz		lb_loop
	}
}

//#define WHITE_MASK	0x3fff3fff;
//#define WHITE_MASK	0xffffffff;
GLOBAL_FUNC_DLL void __stdcall Convert2BitTo16BitImageWithPitch(char* p16Bits,char* p2Bits,DWORD dwWidth,DWORD dwHeight,DWORD dwPitch)
{

	// 00000011

	// 
	__asm
	{
		mov		ebx,dword ptr[dwHeight]
		mov		esi,dword ptr[p2Bits]
		mov		edi,dword ptr[p16Bits]
				
		xor		edx,edx

lb_loop_height:
		mov		ecx,dword ptr[dwWidth]
		shr		ecx,2					; width /4


lb_loop_width:	
		mov		dl,byte ptr[esi]

		; 처음 두 픽셀 4바이트 
		xor		ebx,ebx
		mov		eax,edx

		and		eax,PIXEL_MASK_0
		or		ebx,eax
		shr		edx,2		

		mov		eax,edx
		and		eax,PIXEL_MASK_0
		shl		eax,16				; 16bits 쉬프트 
		or		ebx,eax
		shl		ebx,14				; 4*3 bits+ 2bits 이동
		shr		edx,2		

		mov		eax,ebx
		shr		eax,2
		or		ebx,eax
		or		ebx,0x0FFF0FFF

//		or		ebx,dword ptr[dwOrMask]

		mov		dword ptr[edi],ebx
		; 여기까지 2바이트 완료

		; 두 번째 픽셀 4바이트
		xor		ebx,ebx
		mov		eax,edx

		and		eax,PIXEL_MASK_0
		or		ebx,eax
		shr		edx,2		

		mov		eax,edx
		and		eax,PIXEL_MASK_0
		shl		eax,16				; 16bits 쉬프트 
		or		ebx,eax
		shl		ebx,14				; 4*3 bits+ 2bits 이동

		mov		eax,ebx
		shr		eax,2
		or		ebx,eax
		or		ebx,0x0FFF0FFF
//		or		ebx,dword ptr[dwOrMask]

		mov		dword ptr[edi+4],ebx

		add		edi,8
		inc		esi

		sub		ecx,1
		jnz		lb_loop_width

		add		edi,dword ptr[dwPitch]
		mov		ebx,dword ptr[dwWidth]
		shl		ebx,1
		sub		edi,ebx
		
		sub		dword ptr[dwHeight],1
		jnz		lb_loop_height

	}
}



GLOBAL_FUNC_DLL void __stdcall SetPixel2BitImage(char* p2Bits,DWORD dwPx,DWORD dwPy,DWORD dwWidth,DWORD dwHeight,DWORD dwColor)
{
	// dwColor 는 0부터 3 사이의 값이어야 한다
	__asm
	{
		mov		esi,dword ptr[p2Bits]

		; 몇번째 바이트에 속하는지 먼저 찾는다
		xor		edx,edx
		mov		eax,dword ptr[dwPy]
		mov		ebx,dword ptr[dwWidth]
		mul		ebx

		mov		edx,dword ptr[dwPx]
		add		eax,edx

		shr		eax,2

		add		esi,eax					; 대충의 위치
		
		; 비트 위치
		and		edx,0x00000003			; 4로 나눈 나머지
		mov		ecx,edx
		shl		ecx,1					; 곱하기 2.이만큼 쉬프트할거다

		; and 마스크 만들기.픽셀을 추출
		mov		dl,0xFC
		rol		dl,cl

		mov		bl,byte ptr[esi]
		and		bl,dl

		mov		edx,dword ptr[dwColor]
		and		dl,0x03
		shl		dl,cl

		or		bl,dl

		mov		byte ptr[esi],bl
	}
}

GLOBAL_FUNC_DLL void __stdcall SubPixel2BitImage(char* p2Bits,DWORD dwPx,DWORD dwPy,DWORD dwWidth,DWORD dwHeight,DWORD dwColor)
{
	// dwColor 는 0부터 3 사이의 값이어야 한다
	__asm
	{
		mov		esi,dword ptr[p2Bits]

		; 몇번째 바이트에 속하는지 먼저 찾는다
		xor		edx,edx
		mov		eax,dword ptr[dwPy]
		mov		ebx,dword ptr[dwWidth]
		mul		ebx

		mov		edx,dword ptr[dwPx]
		add		eax,edx

		shr		eax,2

		add		esi,eax					; 대충의 위치
		
		; 비트 위치
		and		edx,0x00000003			; 4로 나눈 나머지
		mov		ecx,edx
		shl		ecx,1					; 곱하기 2.이만큼 쉬프트할거다

		; and 마스크 만들기.픽셀을 추출
		mov		dl,0xFC
		rol		dl,cl

		mov		bl,byte ptr[esi]
		mov		al,bl
		and		bl,dl		; 원 컬러값을 마스킹한 바이트

		not		dl
		and		al,dl		
		ror		al,cl		; 원 컬러값

		mov		dh,byte ptr[dwColor]
		and		dh,0x03

		sub		al,dh
		jnb		lb_not_below

		xor		al,al

lb_not_below:
		rol		al,cl
		or		bl,al
		mov		byte ptr[esi],bl
	}
}

GLOBAL_FUNC_DLL void __stdcall AddPixel2BitImage(char* p2Bits,DWORD dwPx,DWORD dwPy,DWORD dwWidth,DWORD dwHeight,DWORD dwColor)
{
	// dwColor 는 0부터 3 사이의 값이어야 한다
	__asm
	{
		mov		esi,dword ptr[p2Bits]

		; 몇번째 바이트에 속하는지 먼저 찾는다
		xor		edx,edx
		mov		eax,dword ptr[dwPy]
		mov		ebx,dword ptr[dwWidth]
		mul		ebx

		mov		edx,dword ptr[dwPx]
		add		eax,edx

		shr		eax,2

		add		esi,eax					; 대충의 위치
		
		; 비트 위치
		and		edx,0x00000003			; 4로 나눈 나머지
		mov		ecx,edx
		shl		ecx,1					; 곱하기 2.이만큼 쉬프트할거다

		; and 마스크 만들기.픽셀을 추출
		mov		dl,0xFC
		rol		dl,cl

		mov		bl,byte ptr[esi]
		mov		al,bl
		and		bl,dl		; 원 컬러값을 마스킹한 바이트

		not		dl
		and		al,dl		
		ror		al,cl		; 원 컬러값

		mov		dh,byte ptr[dwColor]
		and		dh,0x03

		add		al,dh
		test	al,0x04
		jz		lb_not_overflow

		mov		al,0x03

lb_not_overflow:
		rol		al,cl
		or		bl,al
		mov		byte ptr[esi],bl
	}
}

GLOBAL_FUNC_DLL void __stdcall Create8BitsAttenuationMap(char* p8Bits,DWORD dwWidth,DWORD dwHeight,DWORD dwRS)
{
	BYTE*			p;
	DWORD			x,y;

	int			iRSPow = (int)(dwRS*dwRS);
	int			iCenterX = (int)(dwWidth/2);
	int			iCenterY = (int)(dwHeight/2);
	int			iColor;

	for (y=1; y<dwHeight-1; y++)
	{
		for (x=1; x<dwWidth-1; x++)
		{
			int	iDistPow = (iCenterX-(int)x)*(iCenterX-(int)x) + (iCenterY-(int)y)*(iCenterY-(int)y);
			
			p = (BYTE*)( p8Bits + x + y*dwWidth );
			
			if (iDistPow < iRSPow)
			{

				float	fFallOff = (float)(iRSPow - iDistPow) / (float)iRSPow;
			
				iColor = (int)( 255.0f * fFallOff );
				*p = (BYTE)iColor;
			}
			else
			{
				*p = 0;
			}
		}
	}
}





GLOBAL_FUNC_DLL void __stdcall SetOutLineBorder4Bytes(char* pBits,DWORD dwWidth,DWORD dwHeight,DWORD dwPitch,DWORD dwBorderColor)
{
	DWORD		y;
	char*		p = pBits;


	memset(pBits,dwBorderColor,4*dwWidth);
	memset(pBits + dwPitch*(dwHeight-1),dwBorderColor,4*dwWidth);


	p = pBits;
	for (y=0; y<dwHeight; y++)
	{
		*(DWORD*)p = dwBorderColor;
		p += dwPitch;
	}
	
	p = pBits + (dwWidth-1)*4;
	for (y=0; y<dwHeight; y++)
	{
		*(DWORD*)p = dwBorderColor;
		p += dwPitch;
	}
}

GLOBAL_FUNC_DLL void __stdcall SetOutLineBorder2Bytes(char* pBits,DWORD dwWidth,DWORD dwHeight,DWORD dwPitch,DWORD wBorderColor)
{
	DWORD		x,y;
	char*		p = pBits;
	for (x=0; x<dwWidth; x++)
	{
		*(WORD*)p = wBorderColor;
		p += 2;
	}

	p = pBits + dwPitch*(dwHeight-1);
	for (x=0; x<dwWidth; x++)
	{
		*(WORD*)p = wBorderColor;
		p += 2;
	}

	p = pBits;
	for (y=0; y<dwHeight; y++)
	{
		*(WORD*)p = wBorderColor;
		p += dwPitch;
	}
	
	p = pBits + (dwWidth-1)*2;
	for (y=0; y<dwHeight; y++)
	{
		*(WORD*)p = wBorderColor;
		p += dwPitch;
	}
}


/*
// 쿼드트리를 사용한다는 전제로 사용할 함수이다. 일단은 포인터 테이블 카피해서 호출해야겠지.
// 쿼드트리로 걸러낸 룸 후보들들 넣어준다.
GLOBAL_FUNC_DLL DWORD __stdcall SearchRoomWithCollisionMesh(PORTAL_ROOM** ppResultRoomList,PORTAL_ROOM** ppRoomList,DWORD dwRoomNum,COLLISION_MESH_OBJECT_DESC* pColMeshDesc,DWORD dwMaxResultNum,BOOL* pbInsufficient)
{
	// room이 서로 겹치게 되면 문제가 된다.모델링할때 각별히 신경써야한다.
	// 테스트하는 프로그램이 필요할지도..
	// 이 함수는 이후에 쿼드트리를 타는 방식으로 바꾼다.
	// 1.쿼드트리에서 룸들을 걸러낸다.
	// 2.걸러낸 룸들에 뷰볼륨을 포함하지 않는 경우 익스테리어 룸이 뷰볼륨을 포함하는 것이다.

	*pbInsufficient = FALSE;

	DWORD		dwResultNum = 0;
	float		fDistance;

	DWORD	i,j,k;
	float	D;
	for (i=0; i<dwRoomNum; i++)
	{
		// 먼저 스피어로 테스트
		fDistance = CalcDistance(&ppRoomList[i]->colMeshDesc.boundingSphere.v3Point,&pColMeshDesc->boundingSphere.v3Point);
		if (fDistance >= pColMeshDesc->boundingSphere.fRs + ppRoomList[i]->colMeshDesc.boundingSphere.fRs )
			goto lb_next_room;
		

		for (j=0; j<8; j++)
		{
			for (k=0; k<6; k++)
			{
				D = DotProduct(&ppRoomList[i]->plane[k].v3Up,&pColMeshDesc->boundingBox.v3Oct[j]) + ppRoomList[i]->plane[k].D;
				if (D < 0.0f)
					goto lb_next_room;
			}
			if (dwResultNum >= dwMaxResultNum)
			{
				*pbInsufficient = TRUE;
				goto lb_return;
			}
			ppResultRoomList[dwResultNum] = ppRoomList[i];
			dwResultNum++;
			goto lb_next_room;
		}
		

lb_next_room:
		__asm nop
	}
lb_return:
	return dwResultNum;
}
*/

GLOBAL_FUNC_DLL DWORD __stdcall CollisionTestBoxToBox(VECTOR3* pv3Quad,DWORD* pdwAxisMask,BOUNDING_BOX* pBoxDest,BOUNDING_BOX* pBoxSrcA,BOUNDING_BOX* pBoxSrcB)
{
	*pBoxDest = *pBoxSrcA;
	struct LINE_COLLISION_TEST
	{
		DWORD	dwVertexIndex[2];	// 라인...
		BOOL	bCollisionResult;	// 충돌여부..
		VECTOR3	v3IntersectPoint;	// 충돌점.
		DWORD	dwDir;
	};
	DWORD	dwLineIndexList[24][2] = {
		// x축 정렬
		0,3,
		1,2,
		5,6,
		4,7,

		// y축 정렬
		1,0,
		2,3,
		6,7,
		5,4,
				
		// z축 정렬
		4,0,
		5,1,
		6,2,
		7,3,	
		

		// -x축 정렬
		7,4,
		6,5,
		2,1,
		3,0,

		// -y축 정렬
		4,5,
		7,6,
		3,2,
		0,1,
		
		// -z축 정렬
		3,7,
		2,6,
		1,5,
		0,4
		
	};
	LINE_COLLISION_TEST	testLine[24];
	DWORD	i,j;
	for (i=0; i<24; i++)
	{
		testLine[i].dwVertexIndex[0] = dwLineIndexList[i][0];
		testLine[i].dwVertexIndex[1] = dwLineIndexList[i][1];
		testLine[i].bCollisionResult = FALSE;
		testLine[i].v3IntersectPoint.x = 999999.0f;
		testLine[i].v3IntersectPoint.y = 999999.0f;
		testLine[i].v3IntersectPoint.z = 999999.0f;
		testLine[i].dwDir = 3;
		
	}
	DWORD	dwTriIndexList[12][3] = {
		4,6,5,
		4,7,6,
		
		3,1,2,
		3,0,1,

		7,2,6,
		7,3,2,

		0,5,1,
		0,4,5,

		0,7,4,
		0,3,7,

		5,2,1,
		5,6,2,
	};

	DWORD	dwCollisionPointNum = 0;
	VECTOR3	v3IntersectPoint;
	float	t,u,v;
	DWORD	dwLineIndex = 0;

	
	for (DWORD k=0; k<2; k++)
	{
		for (i=0; i<12; i++)
		{
			for (j=0; j<12; j++)
			{
				VECTOR3*	pv3RayFrom;
				VECTOR3*	pv3RayTo;
				
				VECTOR3		v3RayDir;
				
				pv3RayFrom = &pBoxSrcA->v3Oct[testLine[dwLineIndex].dwVertexIndex[0]];
				pv3RayTo = &pBoxSrcA->v3Oct[testLine[dwLineIndex].dwVertexIndex[1]];

				VECTOR3_SUB_VECTOR3(&v3RayDir,pv3RayTo,pv3RayFrom);

				if (IntersectTriangle(
						&v3IntersectPoint,
						pv3RayFrom,
						&v3RayDir,
						&pBoxSrcB->v3Oct[dwTriIndexList[j][0]],
						&pBoxSrcB->v3Oct[dwTriIndexList[j][1]],
						&pBoxSrcB->v3Oct[dwTriIndexList[j][2]],
						&t,&u,&v,TRUE))
				{

					if ( t >= 0.0f && t <= 1.0f )
					{
			//			g_v3IntersectPoint[g_dwIntersectPointNum] = v3IntersectPoint;
			//			g_dwIntersectPointNum++;

						testLine[dwLineIndex].bCollisionResult = TRUE;
						testLine[dwLineIndex].v3IntersectPoint = v3IntersectPoint;
						testLine[dwLineIndex].dwDir = k;
						
						if (dwCollisionPointNum < 4)
						{
							pv3Quad[dwCollisionPointNum] = v3IntersectPoint;
						}
						dwCollisionPointNum++;
					}
				}
			}
			dwLineIndex++;
		}
	}
	
	for (i=0; i<24; i++)
	{

		if (testLine[i].bCollisionResult)
		{
			pBoxDest->v3Oct[testLine[i].dwVertexIndex[1]] = testLine[i].v3IntersectPoint;					
			
		}
	}
	BOOL	bAxis[6];

	bAxis[0] = FALSE;
	bAxis[1] = FALSE;
	bAxis[2] = FALSE;
	bAxis[3] = FALSE;
	bAxis[4] = FALSE;
	bAxis[5] = FALSE;
	
	DWORD		dwAxisMask = 0;

//	if (dwCollsionPointNum == 4)
//		__asm nop

	// 방향은 0 아님 1이다. 즉 합은 0아니면 4가 나와야 하고 4에 대한 나머지 값은 0이다.
	DWORD	dwDirSum;
	dwLineIndex = 0;
	for (i=0; i<6; i++)
	{
		dwDirSum = 0;
		for (j=0; j<4; j++)
		{
			bAxis[i] =  testLine[dwLineIndex].bCollisionResult;
			dwDirSum += testLine[dwLineIndex].dwDir; 
			dwLineIndex++;
		}
		if (dwCollisionPointNum == 4)
			__asm nop

		bAxis[i] &= ( ~(dwDirSum % 4) );

		dwAxisMask |= (bAxis[i]<<i);
	}
	*pdwAxisMask = dwAxisMask;

	if (dwCollisionPointNum == 4 && !dwAxisMask)
		__asm nop
lb_return:
	return dwCollisionPointNum;
}
/*
GLOBAL_FUNC_DLL BOOL __stdcall IsIncludePortal(VIEW_VOLUME* pVV,PORTAL* pPortal)
{
	BOOL	bResult = FALSE;
	float	D;
	DWORD	i,j;
	

	// 일단 거리 클리핑
	D = CalcDistance(&pVV->From,&pPortal->v3Point);
	if (D > pVV->fFar)
		goto lb_return;


	// 뷰볼륨에 포탈이 들어가는 것은 물론 포탈면이 뷰를 향하고 있어야한다.
	D = DotProduct(&pPortal->plane.v3Up,&pVV->From) + pPortal->plane.D;
	if (  D < 0.0f)
		goto lb_return;


	for (i=0; i<4; i++)
	{
		for (j=0; j<4; j++)
		{
			D = DotProduct(&pVV->Plane[i].v3Up,&pPortal->v3QuadPoint[j]) + pVV->Plane[i].D;
			
			if (D <= 0.0f) 
			{
				// 면 위에 있으면...
				goto lb_next_plane;
			}
		}
		// 여까지 왔으면 실패.모든 평면은 적어도 한개의 점을 위에 얹어야한다.
		goto lb_return;

lb_next_plane:
		__asm nop
	}
	bResult = TRUE;

lb_return:
	return bResult;

}
GLOBAL_FUNC_DLL void __stdcall AdjustViewVolume(VIEW_VOLUME* pVVDest,VIEW_VOLUME* pVVSrc,CAMERA_DESC* pCameraDesc,PORTAL* pPortal)
{
	VECTOR4		v4QuadTransformed[4];
	VECTOR3		v3NewQuadIndexList[4];
	VECTOR3		v3From,v3To;

	MATRIX4	matView,matViewInverse,matProj,matResult;

	::SetViewMatrix(
		&matView,
		&pCameraDesc->v3From,
		&pCameraDesc->v3To,
		&pCameraDesc->v3Up);

	SetInverseMatrix(&matViewInverse,&matView);

	::SetProjectionMatrix(
		&matProj,
		pCameraDesc->fFovY,
		1.0f / pCameraDesc->fAspect,
		DEFAULT_NEAR,
		pCameraDesc->fFar);


	MatrixMultiply2(&matResult,&matView,&matProj);

	TransformV3TOV4(v4QuadTransformed,pPortal->v3QuadPoint,&matResult,4);

	VECTOR3	v3Dir[4];
	for (DWORD i=0; i<4; i++)
	{
		v4QuadTransformed[i].x = v4QuadTransformed[i].x / v4QuadTransformed[i].w;
		v4QuadTransformed[i].y = v4QuadTransformed[i].y / v4QuadTransformed[i].w;
		v4QuadTransformed[i].z = v4QuadTransformed[i].z / v4QuadTransformed[i].w;
		v4QuadTransformed[i].w = 1.0f / v4QuadTransformed[i].w;


		if (v4QuadTransformed[i].x > 1.0f )
			v4QuadTransformed[i].x = 1.0f;

		if (v4QuadTransformed[i].y > 1.0f )
			v4QuadTransformed[i].y = 1.0f;

		if (v4QuadTransformed[i].x < -1.0f )
			v4QuadTransformed[i].x = -1.0f;

		if (v4QuadTransformed[i].y < -1.0f )
			v4QuadTransformed[i].y = -1.0f;

		

		v3NewQuadIndexList[i].x = v4QuadTransformed[i].x / matProj._11;
		v3NewQuadIndexList[i].y = v4QuadTransformed[i].y / matProj._22;
		v3NewQuadIndexList[i].z = 1.0f;


		v3Dir[i].x  = 
			v3NewQuadIndexList[i].x * matViewInverse._11 + 
			v3NewQuadIndexList[i].y * matViewInverse._21 + 
			v3NewQuadIndexList[i].z * matViewInverse._31;

		v3Dir[i].y  = 
			v3NewQuadIndexList[i].x * matViewInverse._12 + 
			v3NewQuadIndexList[i].y * matViewInverse._22 + 
			v3NewQuadIndexList[i].z * matViewInverse._32;

		v3Dir[i].z  = 
			v3NewQuadIndexList[i].x * matViewInverse._13 + 
			v3NewQuadIndexList[i].y * matViewInverse._23 + 
			v3NewQuadIndexList[i].z * matViewInverse._33;
			

 
	}
	v3From.x = matViewInverse._41;
    v3From.y = matViewInverse._42;
    v3From.z = matViewInverse._43;

//	VECTOR3		Points[4];	// 각각 From 입장에서 far를 볼 때, 뷰볼륨의 오른쪽위, 왼쪽위, 왼쪽아래, 오른쪽아래.
	VECTOR3	v3Tri[3];
	v3Tri[0] = pVVSrc->Points[0];
	v3Tri[1] = pVVSrc->Points[3];
	v3Tri[2] = pVVSrc->Points[2];
	
	
	PLANE	farPlane;
	CalcPlaneEquation(&farPlane,v3Tri);
	float	t;
	
	for (i=0; i<4; i++)
	{
		VECTOR3_ADD_VECTOR3(&v3To,&v3From,&v3Dir[i]);
		CalcIntersectPointLineAndPlane(&v3NewQuadIndexList[i],&farPlane,&v3From,&v3To,&t);
	}

	VIEW_VOLUME		vvNew;
	vvNew = *pVVSrc;

	// 왼쪽 
	v3Tri[0] = v3NewQuadIndexList[3];
	v3Tri[1] = v3From;
	v3Tri[2] = v3NewQuadIndexList[2];
	CalcPlaneEquation(&vvNew.Plane[0],v3Tri);

	// 오른쪽
	v3Tri[0] = v3NewQuadIndexList[1];
	v3Tri[1] = v3From;
	v3Tri[2] = v3NewQuadIndexList[0];
	CalcPlaneEquation(&vvNew.Plane[1],v3Tri);
	
	// 아래
	v3Tri[0] = v3NewQuadIndexList[2];
	v3Tri[1] = v3From;
	v3Tri[2] = v3NewQuadIndexList[1];
	CalcPlaneEquation(&vvNew.Plane[2],v3Tri);

	// 위
	v3Tri[0] = v3NewQuadIndexList[0];
	v3Tri[1] = v3From;
	v3Tri[2] = v3NewQuadIndexList[3];
	CalcPlaneEquation(&vvNew.Plane[3],v3Tri);

	vvNew.Points[0] = v3NewQuadIndexList[0];
	vvNew.Points[1] = v3NewQuadIndexList[1];
	vvNew.Points[2] = v3NewQuadIndexList[2];
	vvNew.Points[3] = v3NewQuadIndexList[3];

	*pVVDest = vvNew;
}

*/

/*HRESULT D3DUtil_SetViewMatrix( D3DMATRIX& mat, D3DVECTOR& vFrom,
                               D3DVECTOR& vAt, D3DVECTOR& vWorldUp )
{
    // Get the z basis vector, which points straight ahead. This is the
    // difference from the eyepoint to the lookat point.
    D3DVECTOR vView = vAt - vFrom;

    FLOAT fLength = Magnitude( vView );
    if( fLength < 1e-6f )
        return E_INVALIDARG;

    // Normalize the z basis vector
    vView /= fLength;

    // Get the dot product, and calculate the projection of the z basis
    // vector onto the up vector. The projection is the y basis vector.
    FLOAT fDotProduct = DotProduct( vWorldUp, vView );

    D3DVECTOR vUp = vWorldUp - fDotProduct * vView;

    // If this vector has near-zero length because the input specified a
    // bogus up vector, let's try a default up vector
    if( 1e-6f > ( fLength = Magnitude( vUp ) ) )
    {
        vUp = D3DVECTOR( 0.0f, 1.0f, 0.0f ) - vView.y * vView;

        // If we still have near-zero length, resort to a different axis.
        if( 1e-6f > ( fLength = Magnitude( vUp ) ) )
        {
            vUp = D3DVECTOR( 0.0f, 0.0f, 1.0f ) - vView.z * vView;

            if( 1e-6f > ( fLength = Magnitude( vUp ) ) )
                return E_INVALIDARG;
        }
    }

    // Normalize the y basis vector
    vUp /= fLength;

    // The x basis vector is found simply with the cross product of the y
    // and z basis vectors
    D3DVECTOR vRight = CrossProduct( vUp, vView );

    // Start building the matrix. The first three rows contains the basis
    // vectors used to rotate the view to point at the lookat point
    D3DUtil_SetIdentityMatrix( mat );
    mat._11 = vRight.x;    mat._12 = vUp.x;    mat._13 = vView.x;
    mat._21 = vRight.y;    mat._22 = vUp.y;    mat._23 = vView.y;
    mat._31 = vRight.z;    mat._32 = vUp.z;    mat._33 = vView.z;

    // Do the translation values (rotations are still about the eyepoint)
    mat._41 = - DotProduct( vFrom, vRight );
    mat._42 = - DotProduct( vFrom, vUp );
    mat._43 = - DotProduct( vFrom, vView );

    return S_OK;
}*/




/*
BOOL AddBoundingMesh(COLLISION_MESH_OBJECT_DESC* pDesc,COLLISION_MESH_OBJECT_DESC** ppDesc,DWORD dwNum)
{
	// 바운딩 박스가 유효해야만 사용 가능.
	
	float	max_x = -10000000.0f;
	float	max_y = -10000000.0f;
	float	max_z = -10000000.0f;
	float	min_x = 10000000.0f;
	float	min_y = 10000000.0f;
	float	min_z = 10000000.0f;
	
	DWORD		i=0,j=0;
	VECTOR3*	pos;
	
	for (j=0; j<dwNum; j++)
	{
		pos = ppDesc[j]->boundingBox.v3Oct;

		for (i=0; i<8; i++)
		{
			if (max_x < pos[i].x)
				max_x = pos[i].x;
			
			if (max_y < pos[i].y)
				max_y = pos[i].y;
			
			if (max_z < pos[i].z)
				max_z = pos[i].z;
			
			if (min_x > pos[i].x)
				min_x = pos[i].x;
			
			if (min_y > pos[i].y)
				min_y = pos[i].y;
			
			if (min_z > pos[i].z)
				min_z = pos[i].z;
		}
	}

	VECTOR3		point;

	float		len[3];
	len[0] = (max_x - min_x) / 2.0f;
	len[1] = (max_y - min_y) / 2.0f;
	len[2] = (max_z - min_z) / 2.0f;

	point.x = min_x + len[0];
	point.y = min_y + len[1];
	point.z = min_z + len[2];

	float	rs = -100000.0f;

	for (i=0; i<3; i++)
	{
		if (rs < len[i])
			rs = len[i];
	}
	pDesc->boundingSphere.fRs = rs;
	pDesc->boundingSphere.v3Point = point;


	pDesc->boundingBox.v3Oct[0].x = min_x;
	pDesc->boundingBox.v3Oct[0].y = max_y;
	pDesc->boundingBox.v3Oct[0].z = max_z;

	pDesc->boundingBox.v3Oct[1].x = min_x;
	pDesc->boundingBox.v3Oct[1].y = min_y;
	pDesc->boundingBox.v3Oct[1].z = max_z;

	pDesc->boundingBox.v3Oct[2].x = max_x;
	pDesc->boundingBox.v3Oct[2].y = min_y;
	pDesc->boundingBox.v3Oct[2].z = max_z;

	pDesc->boundingBox.v3Oct[3].x = max_x;
	pDesc->boundingBox.v3Oct[3].y = max_y;
	pDesc->boundingBox.v3Oct[3].z = max_z;

	pDesc->boundingBox.v3Oct[4].x = min_x;
	pDesc->boundingBox.v3Oct[4].y = max_y;
	pDesc->boundingBox.v3Oct[4].z = min_z;

	pDesc->boundingBox.v3Oct[5].x = min_x;
	pDesc->boundingBox.v3Oct[5].y = min_y;
	pDesc->boundingBox.v3Oct[5].z = min_z;

	pDesc->boundingBox.v3Oct[6].x = max_x;
	pDesc->boundingBox.v3Oct[6].y = min_y;
	pDesc->boundingBox.v3Oct[6].z = min_z;

	pDesc->boundingBox.v3Oct[7].x = max_x;
	pDesc->boundingBox.v3Oct[7].y = max_y;
	pDesc->boundingBox.v3Oct[7].z = min_z;

	return TRUE;
}*/
