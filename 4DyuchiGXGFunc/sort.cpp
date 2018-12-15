#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif

#include "global.h"

GLOBAL_FUNC_DLL void __stdcall QSortIntASC(SORT_ITEM_INT* pArray,DWORD dwNum)
{
	int			pivot;
	DWORD		i,j,l,r;
	SORT_ITEM_INT	temp;
	DWORD		stp;


	l = 0;
	r = dwNum-1;

	__asm
	{
		mov			dword ptr[stp],esp
		
		push		dword ptr[r]
		push		dword ptr[l]
		
	}
	
	
lb_loop:
	__asm 
	{
		pop			eax
		mov			dword ptr[l],eax
		pop			eax
		mov			dword ptr[r],eax
	}

	if (r-l+1 <= 1)
		goto lb_next_part;
		
	pivot = pArray[r].iCmp;
	i = l;
	j = r-1;

	while (1)
	{
		while (pArray[i].iCmp <= pivot)
		{
			if (i == r-1)	// 탈출 조건: i값이 축값까지 가버렸다면 소트가 되어있는 상태이다.그러므로 리턴
			{
				__asm
				{
					push		dword ptr[i]
					push		dword ptr[l]
					jmp			lb_loop	
				}
			}

			i++;
		}
lb_check_j:
		while (pArray[j].iCmp > pivot)
		{
			
			if (i >= j)
				goto lb_switch_pivot;
			j--;
		}
lb_switch:
		temp = pArray[i];
		pArray[i] = pArray[j];
		pArray[j] = temp;
		
	}	

lb_switch_pivot:
	temp = pArray[i];
	pArray[i] = pArray[r];
	pArray[r] = temp;


lb_exit:	

	__asm
	{
		push		dword ptr[r]			; push r
		mov			eax,dword ptr[i]
		inc			eax
		push		eax						; push i+1

		dec			eax
		jz			lb_skip_partition

		dec			eax
		push		eax						; push	i-1
		push		dword ptr[l]
lb_skip_partition:
	}

lb_next_part:

	__asm
	{
		cmp			esp,dword ptr[stp]
		jnz			lb_loop
	}

lb_return:

	return;

}
GLOBAL_FUNC_DLL void __stdcall QSortIntDSC(SORT_ITEM_INT* pArray,DWORD dwNum)
{
	int			pivot;
	DWORD		i,j,l,r;
	SORT_ITEM_INT	temp;
	DWORD		stp;


	l = 0;
	r = dwNum-1;

	__asm
	{
		mov			dword ptr[stp],esp
		
		push		dword ptr[r]
		push		dword ptr[l]
		
	}
	
	
lb_loop:
	__asm 
	{
		pop			eax
		mov			dword ptr[l],eax
		pop			eax
		mov			dword ptr[r],eax
	}

	if (r-l+1 <= 1)
		goto lb_next_part;
		
	pivot = pArray[r].iCmp;
	i = l;
	j = r-1;

	while (1)
	{
		while (pArray[i].iCmp >= pivot)
		{
			if (i == r-1)	// 탈출 조건: i값이 축값까지 가버렸다면 소트가 되어있는 상태이다.그러므로 리턴
			{
				__asm
				{
					push		dword ptr[i]
					push		dword ptr[l]
					jmp			lb_loop	
				}
			}

			i++;
		}
lb_check_j:
		while (pArray[j].iCmp < pivot)
		{
			
			if (i >= j)
				goto lb_switch_pivot;
			j--;
		}
lb_switch:
		temp = pArray[i];
		pArray[i] = pArray[j];
		pArray[j] = temp;
		
	}	

lb_switch_pivot:
	temp = pArray[i];
	pArray[i] = pArray[r];
	pArray[r] = temp;


lb_exit:	

	__asm
	{
		push		dword ptr[r]			; push r
		mov			eax,dword ptr[i]
		inc			eax
		push		eax						; push i+1

		dec			eax
		jz			lb_skip_partition

		dec			eax
		push		eax						; push	i-1
		push		dword ptr[l]
lb_skip_partition:
	}

lb_next_part:

	__asm
	{
		cmp			esp,dword ptr[stp]
		jnz			lb_loop
	}

lb_return:

	return;

}

GLOBAL_FUNC_DLL void __stdcall QSortFloatASC(SORT_ITEM_FLOAT* pArray,DWORD dwNum)
{
	float			pivot;
	DWORD			i,j,l,r;
	SORT_ITEM_FLOAT	temp;
	DWORD			stp;


	l = 0;
	r = dwNum-1;

	__asm
	{
		mov			dword ptr[stp],esp
		
		push		dword ptr[r]
		push		dword ptr[l]
		
	}
	
	
lb_loop:
	__asm 
	{
		pop			eax
		mov			dword ptr[l],eax
		pop			eax
		mov			dword ptr[r],eax
	}

	if (r-l+1 <= 1)
		goto lb_next_part;
		
	pivot = pArray[r].fCmp;
	i = l;
	j = r-1;

	while (1)
	{
		while (pArray[i].fCmp <= pivot)
		{
			if (i == r-1)	// 탈출 조건: i값이 축값까지 가버렸다면 소트가 되어있는 상태이다.그러므로 리턴
			{
				__asm
				{
					push		dword ptr[i]
					push		dword ptr[l]
					jmp			lb_loop	
				}
			}

			i++;
		}
lb_check_j:
		while (pArray[j].fCmp > pivot)
		{
			
			if (i >= j)
				goto lb_switch_pivot;
			j--;
		}
lb_switch:
		temp = pArray[i];
		pArray[i] = pArray[j];
		pArray[j] = temp;
		
	}	

lb_switch_pivot:
	temp = pArray[i];
	pArray[i] = pArray[r];
	pArray[r] = temp;


lb_exit:	

	__asm
	{
		push		dword ptr[r]			; push r
		mov			eax,dword ptr[i]
		inc			eax
		push		eax						; push i+1

		dec			eax
		jz			lb_skip_partition

		dec			eax
		push		eax						; push	i-1
		push		dword ptr[l]
lb_skip_partition:
	}

lb_next_part:

	__asm
	{
		cmp			esp,dword ptr[stp]
		jnz			lb_loop
	}

lb_return:

	return;

}
GLOBAL_FUNC_DLL void __stdcall QSortFloatDSC(SORT_ITEM_FLOAT* pArray,DWORD dwNum)
{
	float			pivot;
	DWORD			i,j,l,r;
	SORT_ITEM_FLOAT	temp;
	DWORD			stp;


	l = 0;
	r = dwNum-1;

	__asm
	{
		mov			dword ptr[stp],esp
		
		push		dword ptr[r]
		push		dword ptr[l]
		
	}
	
	
lb_loop:
	__asm 
	{
		pop			eax
		mov			dword ptr[l],eax
		pop			eax
		mov			dword ptr[r],eax
	}

	if (r-l+1 <= 1)
		goto lb_next_part;
		
	pivot = pArray[r].fCmp;
	i = l;
	j = r-1;

	while (1)
	{
		while (pArray[i].fCmp >= pivot)
		{
			if (i == r-1)	// 탈출 조건: i값이 축값까지 가버렸다면 소트가 되어있는 상태이다.그러므로 리턴
			{
				__asm
				{
					push		dword ptr[i]
					push		dword ptr[l]
					jmp			lb_loop	
				}
			}

			i++;
		}
lb_check_j:
		while (pArray[j].fCmp < pivot)
		{
			
			if (i >= j)
				goto lb_switch_pivot;
			j--;
		}
lb_switch:
		temp = pArray[i];
		pArray[i] = pArray[j];
		pArray[j] = temp;
		
	}	

lb_switch_pivot:
	temp = pArray[i];
	pArray[i] = pArray[r];
	pArray[r] = temp;


lb_exit:	

	__asm
	{
		push		dword ptr[r]			; push r
		mov			eax,dword ptr[i]
		inc			eax
		push		eax						; push i+1

		dec			eax
		jz			lb_skip_partition

		dec			eax
		push		eax						; push	i-1
		push		dword ptr[l]
lb_skip_partition:
	}

lb_next_part:

	__asm
	{
		cmp			esp,dword ptr[stp]
		jnz			lb_loop
	}

lb_return:

	return;

}