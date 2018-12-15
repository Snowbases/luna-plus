#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif

#include "global.h"
#include "look_aside_list.h"


void InitializeMemoryBlock(void* pBlockEntry,MEMORY_DESC* pDescEntry,DWORD dwUnitSize,DWORD dwNum);
BOOL ReInitialize(CLookAsideList* pList);

CLookAsideList::CLookAsideList()
{
	m_pBaseDesc = NULL;
	m_dwCommitedBlockNum = 0;	
	m_dwUnitSize = 0;
	m_dwMaxBlockNum = 0;
	m_pLinearMemoryPool = NULL;
	m_pLinearDescPool = NULL;
	m_dwAllocatedNum = 0;
	m_bBugCheck = FALSE;
}
GLOBAL_FUNC_DLL STMPOOL_HANDLE __stdcall CreateStaticMemoryPool()
{
	CLookAsideList* pPool = new CLookAsideList;
	return pPool;
}

GLOBAL_FUNC_DLL DWORD __stdcall	LALGetAllocatedNum(STMPOOL_HANDLE pool)
{
	CLookAsideList* pList = (CLookAsideList*)pool;
	return pList->m_dwAllocatedNum;
}

GLOBAL_FUNC_DLL BOOL __stdcall InitializeStaticMemoryPool(STMPOOL_HANDLE pool,DWORD dwUnitSize,DWORD dwDefaultCommitNum,DWORD dwMaxNum)
{
	BOOL			bResult = FALSE;

	CLookAsideList* pList = (CLookAsideList*)pool;
	
	

	if (!dwDefaultCommitNum)
		pList->m_dwDefaultCommitBlockNum = 1;
	else 
		pList->m_dwDefaultCommitBlockNum = dwDefaultCommitNum;

	if (dwDefaultCommitNum > dwMaxNum)
		dwDefaultCommitNum = dwMaxNum;

	pList->m_dwMaxBlockNum = dwMaxNum;
	pList->m_dwUnitSize = dwUnitSize;

	if (!(pList->m_pLinearMemoryPool = VirtualAlloc(NULL,dwMaxNum*(pList->m_dwUnitSize+MEMORY_BLOCK_HEADER_SIZE),MEM_RESERVE,PAGE_READWRITE)))
		goto lb_return;
	
	if (!(pList->m_pLinearDescPool = VirtualAlloc(NULL,dwMaxNum*sizeof(MEMORY_DESC),MEM_RESERVE,PAGE_READWRITE)))
		goto lb_return;

	bResult = TRUE;
		
	if (dwDefaultCommitNum)
	{
		if (!VirtualAlloc(pList->m_pLinearMemoryPool,dwDefaultCommitNum*(pList->m_dwUnitSize+MEMORY_BLOCK_HEADER_SIZE),MEM_COMMIT,PAGE_READWRITE))
		{
			bResult = FALSE;
			goto lb_return;
		}	
		if (!VirtualAlloc(pList->m_pLinearDescPool,dwDefaultCommitNum*sizeof(MEMORY_DESC),MEM_COMMIT,PAGE_READWRITE))
		{
			bResult = FALSE;
			goto lb_return;
		}	
		pList->m_dwCommitedBlockNum = dwDefaultCommitNum;
		InitializeMemoryBlock(
			pList->m_pLinearMemoryPool,
			(MEMORY_DESC*)pList->m_pLinearDescPool,
			pList->m_dwUnitSize,
			pList->m_dwCommitedBlockNum);
		pList->m_pBaseDesc = (MEMORY_DESC*)pList->m_pLinearDescPool;

	}

lb_return:
	return bResult;
}


GLOBAL_FUNC_DLL void __stdcall ReleaseStaticMemoryPool(STMPOOL_HANDLE pool)
{
	CLookAsideList* pList = (CLookAsideList*)pool;

	VirtualFree(
		pList->m_pLinearMemoryPool,
		pList->m_dwCommitedBlockNum*pList->m_dwUnitSize,
		MEM_DECOMMIT);
	VirtualFree(pList->m_pLinearMemoryPool,0,MEM_RELEASE);
	
	VirtualFree(
		pList->m_pLinearDescPool,
		pList->m_dwCommitedBlockNum*sizeof(MEMORY_DESC),MEM_DECOMMIT);
	VirtualFree(pList->m_pLinearDescPool,0,MEM_RELEASE);

	delete pList;

}

BOOL ReInitialize(CLookAsideList* pool)
{
	CLookAsideList* pList = (CLookAsideList*)pool;

	BOOL	bResult = FALSE;
	char*	pMemoryPoolEntry;
	char*	pDescPoolEntry;

	if ( pList->m_dwCommitedBlockNum + pList->m_dwDefaultCommitBlockNum 	> pList->m_dwMaxBlockNum)
		goto lb_return;
		

	pMemoryPoolEntry = (char*)pList->m_pLinearMemoryPool + pList->m_dwCommitedBlockNum*(pList->m_dwUnitSize+MEMORY_BLOCK_HEADER_SIZE);
	pDescPoolEntry = (char*)pList->m_pLinearDescPool + pList->m_dwCommitedBlockNum*sizeof(MEMORY_DESC);

	if (!VirtualAlloc(pMemoryPoolEntry,pList->m_dwDefaultCommitBlockNum*(pList->m_dwUnitSize+MEMORY_BLOCK_HEADER_SIZE),MEM_COMMIT,PAGE_READWRITE))
		goto lb_return;
	

	if (!VirtualAlloc(pDescPoolEntry,pList->m_dwDefaultCommitBlockNum*sizeof(MEMORY_DESC),MEM_COMMIT,PAGE_READWRITE))
		goto lb_return;

	pList->m_dwCommitedBlockNum += pList->m_dwDefaultCommitBlockNum;

	InitializeMemoryBlock(pMemoryPoolEntry,(MEMORY_DESC*)pDescPoolEntry,pList->m_dwUnitSize,pList->m_dwDefaultCommitBlockNum);
	
	pList->m_pBaseDesc = (MEMORY_DESC*)pDescPoolEntry;
	bResult = TRUE;

lb_return:
	return bResult;

}
GLOBAL_FUNC_DLL void* __stdcall LALAlloc(STMPOOL_HANDLE pool)
{
	void*		pMem;
	__asm
	{
		
lb_begin:
		mov			ebx,dword ptr[pool]
		mov			edx,dword ptr[ebx]			; m_pBaseDesc

		or			edx,edx						; 여분 메모리 블럭이 없는 경우 추가할당.
		jnz			lb_step_1
		
		; reinitialize memory pool since nothing avaliable memory block
		push		ebx
		call		ReInitialize
		pop			ebx

		or			eax,eax
		jnz			lb_begin
		
		xor			eax,eax
		jmp			lb_return

lb_step_1:
		inc			dword ptr[ebx+20]
		mov			eax,dword ptr[edx]		; m_pBaseDesc->m_pAddr
		mov			esi,dword ptr[edx+4]	; m_pBaseDesc->m_pNext
		add			eax,4					; result
		mov			dword ptr[ebx],esi

lb_return:

		mov			dword ptr[pMem],eax
	}

	return pMem;
}
void BlockCheck(CLookAsideList* pList,char* pMemory)
{
	pMemory-=4;


	MEMORY_DESC* pCur = pList->m_pBaseDesc;
	while (pCur)
	{
		if (pCur->m_pAddr == pMemory)
		{
			char	txt[128];
			wsprintf(txt,"Fail to LALFree,%x is invalid block",pMemory);
			OutputDebugString(txt);
			__asm int 3

		}
		pCur = pCur->m_pNext;
	}
	
}
GLOBAL_FUNC_DLL void __stdcall LALFree(STMPOOL_HANDLE pool,void* pMemory)
{
	if (((CLookAsideList*)pool)->m_bBugCheck)
		BlockCheck((CLookAsideList*)pool,(char*)pMemory);


	__asm 
	{	
		mov			ebx,dword ptr[pool]
		mov			eax,dword ptr[pMemory]


		or			eax,eax
		jz			lb_return;

		mov			edx,dword ptr[eax-4]	; pDesc
		mov			eax,dword ptr[ebx]		; m_pBaseDesc
		
		dec			dword ptr[ebx+20]
		mov			dword ptr[edx+4],eax	; pDesc->m_pNext = m_pBaseDesc
		mov			dword ptr[ebx],edx
lb_return:
	}
}

GLOBAL_FUNC_DLL void __stdcall LALEnableBlockCheck(STMPOOL_HANDLE pool,BOOL bSwitch)
{
	((CLookAsideList*)pool)->m_bBugCheck  = bSwitch;
}
GLOBAL_FUNC_DLL void __stdcall LALFreeWithCheck(STMPOOL_HANDLE pool,void* pMemory)
{

	BlockCheck((CLookAsideList*)pool,(char*)pMemory);
	LALFree(pool,pMemory);
}


void __declspec(naked) InitializeMemoryBlock(void* pBlockEntry,MEMORY_DESC* pDescEntry,DWORD dwUnitSize,DWORD dwNum)
{

	__asm
	{
		enter		0,0
		push		esi
		push		edi
		push		ebx
		push		edx
		push		ecx


		xor			ebx,ebx
		mov			esi,dword ptr[pBlockEntry]
		mov			edi,dword ptr[pDescEntry]
		mov			edx,dword ptr[dwUnitSize]
		mov			ecx,dword ptr[dwNum]

		mov			dword ptr[edi+4],ebx
		mov			dword ptr[edi],esi
		mov			dword ptr[esi],edi
		
		add			esi,edx
		add			edi,MEMORY_DESC_SIZE
		add			esi,4

		dec			ecx
		jz			lb_end
		
		
lb_loop:
		mov			dword ptr[edi-4],edi
		mov			dword ptr[edi],esi
		mov			dword ptr[esi],edi
		add			esi,edx
		add			edi,MEMORY_DESC_SIZE
		add			esi,4

		loop		lb_loop
lb_end:		
		mov			dword ptr[edi-4],ebx

		pop			ecx
		pop			edx
		pop			ebx
		pop			edi
		pop			esi

		
		leave
		ret
	}
}