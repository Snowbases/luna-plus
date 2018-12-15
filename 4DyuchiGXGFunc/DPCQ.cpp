// DPCQ.cpp: implementation of the CDPCQ class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif

#include "global.h"
#include "DPCQ.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLOBAL_FUNC_DLL DPCQ_HANDLE __stdcall DPCQCreate()
{
	CDPCQ*	pDPCQ = new CDPCQ;
	return pDPCQ;
}
GLOBAL_FUNC_DLL BOOL __stdcall DPCQInitialize(DPCQ_HANDLE pDPCQHandle,DWORD dwMaxBufferSize)
{
	return ((CDPCQ*)pDPCQHandle)->Initialize(dwMaxBufferSize);
	
}
GLOBAL_FUNC_DLL BOOL __stdcall DPCQPushDPC(DPCQ_HANDLE pDPCQHandle,void* pStdcallFunc,WORD wArgNum,DWORD* pArgList,int* pRetAddr,int iOrder)
{
	return ((CDPCQ*)pDPCQHandle)->PushDPC(pStdcallFunc,wArgNum,pArgList,pRetAddr,iOrder);
}

GLOBAL_FUNC_DLL void __stdcall DPCQProcess(DPCQ_HANDLE pDPCQHandle)
{
	((CDPCQ*)pDPCQHandle)->Process();

}
GLOBAL_FUNC_DLL void __stdcall DPCQProcessWithAscSort(DPCQ_HANDLE pDPCQHandle)
{
	((CDPCQ*)pDPCQHandle)->ProcessWithAscSort();
}
GLOBAL_FUNC_DLL void __stdcall DPCQClear(DPCQ_HANDLE pDPCQHandle)
{
	((CDPCQ*)pDPCQHandle)->Clear();
}

GLOBAL_FUNC_DLL void __stdcall DPCQRelease(DPCQ_HANDLE pDPCQHandle)
{
	delete (CDPCQ*)pDPCQHandle;
}





CDPCQ::CDPCQ()
{
	memset(this,0,sizeof(CDPCQ));
}
BOOL CDPCQ::Initialize(DWORD dwMaxBufferSize)
{
	m_pBuffer = new char[dwMaxBufferSize];
	memset(m_pBuffer,0,dwMaxBufferSize);
	m_dwMaxBufferSize = dwMaxBufferSize;

	m_dwMaxItemNum = dwMaxBufferSize / DEFAULT_DPC_ITEM_SIZE;
	m_pItemPtrList = new SORT_ITEM_INT[m_dwMaxItemNum];
	memset(m_pItemPtrList,0,sizeof(SORT_ITEM_INT)*m_dwMaxItemNum);
	
	return TRUE;
}
BOOL CDPCQ::PushDPC(void* pStdcallFunc,WORD wArgNum,DWORD* pArgList,int* pRetAddr,int iOrder)
{
	
	BOOL	bResult = FALSE;

	WORD	wSize = sizeof(DPC_ITEM) - sizeof(void*) + sizeof(void*)*wArgNum;

	if (m_dwItemNum >= m_dwMaxItemNum)
		goto lb_return;

	if (m_dwCurrentOffset+(DWORD)wSize > m_dwMaxBufferSize)
		goto lb_return;

	DPC_ITEM*	pDPC;
	pDPC = (DPC_ITEM*)(m_pBuffer+m_dwCurrentOffset);
	
	pDPC->wSize = wSize;
	pDPC->pStdcallFunc = pStdcallFunc;
	pDPC->pRetAddr = pRetAddr;
	pDPC->wArgNum = wArgNum;

	WORD	i;
	for (i=0; i<wArgNum; i++)
	{
		pDPC->pArg[i] = pArgList[i];
	}
	m_pItemPtrList[m_dwItemNum].pItem = (void*)pDPC;
	m_pItemPtrList[m_dwItemNum].iCmp = iOrder;

	m_dwItemNum++;
	m_dwCurrentOffset += (DWORD)wSize;

	bResult = TRUE;

lb_return:
	return bResult;
	
}
void CDPCQ::Process()
{
	/*
	#define					W_SIZE_OFFSET		0
	#define					W_ARG_NUM_OFFSET	2
	#define					FUNCPTR_OFFSET		4
	#define					RETADDR_OFFSET		8
	#define					ARG_LIST_OFFSET		12
	*/
	DWORD		dwSize;
	char* pEntry = m_pBuffer;
	DWORD		dwItemNum = m_dwItemNum;

	__asm 
	{
lb_begin:
		mov			ecx,dword ptr[dwItemNum]				; 아이템 갯수
		or			ecx,ecx
		jz			lb_return

		xor			eax,eax
		xor			ecx,ecx
		mov			esi,dword ptr[pEntry]					; pDPC
		mov			ax,word ptr[esi]						; wSize
		mov			dword ptr[dwSize],eax					; backup size
		
		mov			cx,word ptr[esi+W_ARG_NUM_OFFSET]		; wArgNum

lb_push_arg:
		or			cx,cx
		jz			lb_call						; 인자가 없으면 바로 호출..

		dec			cx							; 갯수-1 인덱스부터 내림차순으로 푸쉬한다.
		mov			ax,cx
		shl			ax,2						; 인덱스*4 바이트 오프셋 
		mov			edx,dword ptr[esi+ARG_LIST_OFFSET+eax]	; load arg
		push		edx
		
		jmp			lb_push_arg

lb_call:
		call		dword ptr[esi+FUNCPTR_OFFSET]
		mov			esi,dword ptr[pEntry]					; pDPC
		mov			ebx,dword ptr[esi+RETADDR_OFFSET]		; 리턴값을 넣을 메모리가 유효한지 체크 
		or			ebx,ebx									;
		jz			lb_next_item
		
		mov			dword ptr[ebx],eax						; 리턴값을 써넣는다.
lb_next_item:
		mov			eax,dword ptr[dwSize]					; restore dwSize
		add			dword ptr[pEntry],eax
		dec			dword ptr[dwItemNum]
		jmp			lb_begin

lb_return:
	}
}
void CDPCQ::ProcessWithAscSort()
{
	/*
	#define					W_SIZE_OFFSET		0
	#define					W_ARG_NUM_OFFSET	2
	#define					FUNCPTR_OFFSET		4
	#define					RETADDR_OFFSET		8
	#define					ARG_LIST_OFFSET		12
	*/
	char*		pEntry = (char*)m_pItemPtrList;
	DWORD		dwItemNum = m_dwItemNum;

	if (!dwItemNum)
		return;

	QSortIntASC(m_pItemPtrList,m_dwItemNum);

	__asm 
	{
		mov			edi,dword ptr[pEntry]					; ppDPC
lb_begin:
		mov			esi,dword ptr[edi+4]						; pDPC
		movzx		ecx,word ptr[esi+W_ARG_NUM_OFFSET]		; wArgNum
		push		esi
lb_push_arg:
		or			cx,cx
		jz			lb_call						; 인자가 없으면 바로 호출..

		dec			cx							; 갯수-1 인덱스부터 내림차순으로 푸쉬한다.
		mov			ax,cx
		shl			ax,2						; 인덱스*4 바이트 오프셋 
		mov			edx,dword ptr[esi+ARG_LIST_OFFSET+eax]	; load arg
		push		edx
		
		jmp			lb_push_arg

lb_call:
		call		dword ptr[esi+FUNCPTR_OFFSET]
		pop			esi
		xor			eax,eax


		mov			ebx,dword ptr[esi+RETADDR_OFFSET]		; 리턴값을 넣을 메모리가 유효한지 체크 
		or			ebx,ebx									;
		jz			lb_next_item
		
		mov			dword ptr[ebx],eax						; 리턴값을 써넣는다.
lb_next_item:

		
		add			edi,SORT_ITEM_SIZE
		dec			dword ptr[dwItemNum]					; m_dwItemNum--
		jnz			lb_begin

lb_return:
		nop
	}
}
void CDPCQ::Clear()
{
	m_dwCurrentOffset = 0;
	m_dwItemNum = 0;

}

CDPCQ::~CDPCQ()
{
	if (m_pItemPtrList)
	{
		delete [] m_pItemPtrList;
		m_pItemPtrList = NULL;
	}
	if (m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

}
