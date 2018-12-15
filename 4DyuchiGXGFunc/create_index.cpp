#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif

#include "global.h"

#include "create_index.h"

void InitializeIndexDesc(void* pEntry,DWORD dwNum);

CIndexCreator::CIndexCreator()
{
	memset(this,0,sizeof(CIndexCreator));
}
GLOBAL_FUNC_DLL INDEXCR_HANDLE __stdcall ICCreate()
{
	CIndexCreator*	pIC = new CIndexCreator;
	return pIC;
}

	
GLOBAL_FUNC_DLL BOOL __stdcall ICInitialize(INDEXCR_HANDLE pIC,DWORD dwMaxNum)
{
	CIndexCreator*	pIndexCR = (CIndexCreator*)pIC;

	pIndexCR->m_dwMaxIndexNum = dwMaxNum;
	pIndexCR->m_pIndexList = new INDEX_DESC[dwMaxNum];
	InitializeIndexDesc(pIndexCR->m_pIndexList,dwMaxNum);

	pIndexCR->m_pBaseDesc = pIndexCR->m_pIndexList;
	pIndexCR->m_pTailDesc = pIndexCR->m_pIndexList+dwMaxNum-1;

	pIndexCR->m_ppIndexDescTable = new INDEX_DESC*[dwMaxNum+1];
	memset(pIndexCR->m_ppIndexDescTable,0,sizeof(INDEX_DESC*)*(dwMaxNum+1));

	return TRUE;
}
GLOBAL_FUNC_DLL DWORD __stdcall ICGetIndexNum(INDEXCR_HANDLE pIC)
{
	CIndexCreator*	pIndexCR = (CIndexCreator*)pIC;

	return pIndexCR->m_dwIndexNum;
}
GLOBAL_FUNC_DLL DWORD __stdcall ICAllocIndex(INDEXCR_HANDLE pIC)
{
	DWORD	dwResult;
	__asm
	{
		xor			eax,eax
		mov			ebx,dword ptr[pIC]
		mov			esi,dword ptr[ebx+4]		; m_pBaseDesc
		or			esi,esi
		jz			lb_return

		mov			edx,dword ptr[esi]			; result dwIndex
		mov			edi,dword ptr[ebx+8]		; m_ppIndexDescTable
		mov			eax,dword ptr[esi+4]		; pDesc->m_pNext
		mov			dword ptr[ebx+4],eax		; m_pBaseDesc = pDesc->m_pNext
		
		
		; if (m_pBaseDesc == NULL)
		or			eax,eax
		jnz			lb_skip

		; m_pTailDesc = m_pBaseDesc
		mov			dword ptr[ebx+20],eax
lb_skip:



		mov			eax,edx						; backup result
		shl			edx,2						; * 4
		add			edi,edx
		mov			dword ptr[edi],esi			; m_ppIndexDescTable[dwIndex] = pDesc(result)
		inc			dword ptr[ebx+12]
lb_return:
		mov			dword ptr[dwResult],eax
	}
#ifdef _DEBUG
	if (dwResult > ((CIndexCreator*)pIC)->m_dwMaxIndexNum)
		__asm int 3
#endif

	return dwResult;
}

GLOBAL_FUNC_DLL void __stdcall ICFreeIndex(INDEXCR_HANDLE  pIC,DWORD dwIndex)
{
#ifdef _DEBUG
	if (dwIndex > ((CIndexCreator*)pIC)->m_dwMaxIndexNum)
		__asm int 3
#endif
	__asm
	{

		xor			edx,edx
		mov			ebx,dword ptr[pIC]
		mov			eax,dword ptr[dwIndex]
		mov			edi,dword ptr[ebx+8]		; m_ppIndexDescTable
		shl			eax,2
		add			edi,eax
		mov			esi,dword ptr[edi]			; pDesc
		
		or			esi,esi
		jz			lb_return

		mov			dword ptr[edi],edx			; m_ppIndexDescTable[dwIndex] = NULL
		mov			dword ptr[esi+4],edx		; pDesc->m_pNext = NULL

		mov			edi,dword ptr[ebx+20]		; m_pTailDesc

		; if (m_pTailDesc == NULL)
		or			edi,edi
		jnz			lb_not_null

		; m_pTailDesc = m_pBaseDesc = pDesc
		mov			dword ptr[ebx+4],esi
		mov			dword ptr[ebx+20],esi
		jmp			lb_end

lb_not_null:
		mov			dword ptr[edi+4],esi		; m_pTailDesc->pNext = pDesc
		mov			dword ptr[ebx+20],esi		; m_pTailDesc = pDesc
		
lb_end:

		dec			dword ptr[ebx+12]
lb_return:

	}
}
GLOBAL_FUNC_DLL void __stdcall ICRelease(INDEXCR_HANDLE pIC)
{
	CIndexCreator*	pIndexCR = (CIndexCreator*)pIC;
	delete pIndexCR;
}

CIndexCreator::~CIndexCreator()
{
	if (m_pIndexList)
	{
		delete [] m_pIndexList;
		m_pIndexList = NULL;
	}
	if (m_ppIndexDescTable)
	{
		delete [] m_ppIndexDescTable;
		m_ppIndexDescTable = NULL;
	}

}

void __declspec(naked) InitializeIndexDesc(void* pEntry,DWORD dwNum)
{
	__asm
	{
		enter		0,0
		
		push		esi
		push		edi
		push		eax
		push		edx
		push		ecx

		xor			eax,eax
		xor			edx,edx
		inc			eax
		mov			edi,dword ptr[pEntry]
		mov			ecx,dword ptr[dwNum]
		mov			esi,edi
		add			edi,INDEX_DESC_SIZE
		dec			ecx

lb_loop:
		mov			dword ptr[edi-8],eax
		mov			dword ptr[edi-4],edi
		inc			eax
		add			edi,INDEX_DESC_SIZE

		loop		lb_loop
		
		mov			dword ptr[edi-4],edx
		mov			dword ptr[edi-8],eax

		pop			ecx
		pop			edx
		pop			eax
		pop			edi
		pop			esi

		leave
		ret 
	}

}