#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif

#include "../4DyuchiGXGFunc/global.h"
//#include "global.h"
#include "Heap.h"

GLOBAL_FUNC_DLL	VHEAP_HANDLE	__stdcall	VHPCreateHeap()
{
	CHeap*	pHeap = new CHeap;
	return (VHEAP_HANDLE)pHeap;
}

GLOBAL_FUNC_DLL	void	__stdcall	VHPHeapCheck(VHEAP_HANDLE pVHeap)
{
	CHeap*	pHeap = (CHeap*)pVHeap;
	pHeap->HeapCheck();

}
GLOBAL_FUNC_DLL	void	__stdcall	VHPLeakCheck(VHEAP_HANDLE pVHeap)
{
	CHeap*	pHeap = (CHeap*)pVHeap;
	pHeap->LeakCheck();
}

GLOBAL_FUNC_DLL	BOOL	__stdcall	VHPInitialize(VHEAP_HANDLE pVHeap,void* pMassMemory,DWORD dwMassMemorySize,DWORD dwMaxBlockNum,DWORD dwUnitSize,DWORD dwAlignUnitNum,BOOL bEnableDebug){
	CHeap*	pHeap = (CHeap*)pVHeap;
	return pHeap->Initialize(pMassMemory,dwMassMemorySize,dwMaxBlockNum,dwUnitSize,dwAlignUnitNum,bEnableDebug);
}

GLOBAL_FUNC_DLL	void*	__stdcall	VHPAlloc(VHEAP_HANDLE pVHeap,DWORD* pdwErrorCode,DWORD dwSize,BOOL bAlign)
{
	CHeap*	pHeap = (CHeap*)pVHeap;
	return pHeap->Alloc(pdwErrorCode,dwSize,bAlign);
}
GLOBAL_FUNC_DLL void* 	__stdcall	VHPAllocUnit(VHEAP_HANDLE pVHeap,DWORD* pdwErrorCode,DWORD dwUnitNum,DWORD* pdwSize,BOOL bAlign)
{
	CHeap*	pHeap = (CHeap*)pVHeap;
	return pHeap->AllocUnit(pdwErrorCode,dwUnitNum,pdwSize,bAlign);

}
GLOBAL_FUNC_DLL	void	__stdcall	VHPFree(VHEAP_HANDLE pVHeap,void* pMem)
{
	CHeap*	pHeap = (CHeap*)pVHeap;
	pHeap->Free(pMem);
}

GLOBAL_FUNC_DLL	void*	__stdcall	VHPDBGAlloc(VHEAP_HANDLE pVHeap,DWORD* pdwErrorCode,DWORD dwSize,BOOL bAlign)
{
	CHeap*	pHeap = (CHeap*)pVHeap;
	return pHeap->Alloc(pdwErrorCode,dwSize,bAlign);
}
GLOBAL_FUNC_DLL void* 	__stdcall	VHPDBGAllocUnit(VHEAP_HANDLE pVHeap,DWORD* pdwErrorCode,DWORD dwUnitNum,DWORD* pdwSize,BOOL bAlign)
{
	CHeap*	pHeap = (CHeap*)pVHeap;
	return pHeap->AllocUnit(pdwErrorCode,dwUnitNum,pdwSize,bAlign);

}

GLOBAL_FUNC_DLL	void	__stdcall 	VHPDBGFree(VHEAP_HANDLE pVHeap,void* pMem)
{
	CHeap*	pHeap = (CHeap*)pVHeap;
	pHeap->Free(pMem);
}

GLOBAL_FUNC_DLL	void	__stdcall	VHPReleaseHeap(VHEAP_HANDLE pVHeap)
{
	delete (CHeap*)pVHeap;
}


void CHeap::FreeMemoryStatusDesc(MEMORY_STATUS_DESC_LINK* pDesc)
{

	MEMORY_STATUS_DESC_LINK*	pPrv = pDesc->pPrv;
	MEMORY_STATUS_DESC_LINK*	pNext = pDesc->pNext;

	pPrv->pNext = pNext;
	pNext->pPrv = pPrv;

	LALFree(m_pStatusDescLinkPool,pDesc);
}

CHeap::CHeap()
{
	memset(this,0,sizeof(CHeap));
}

BOOL CHeap::Initialize(void* pMassMemory,DWORD dwMassMemorySize,DWORD dwMaxBlockNum,DWORD dwUnitSize,DWORD dwAlignUnitNum,BOOL bEnableDebug)
{
	BOOL	bResult = FALSE;

	m_dwAllocatedNum = 0;
	m_dwAlloctedMemorySize = 0;

	if (!dwUnitSize)
		dwUnitSize = 1;
	
	m_pMassMemory = (char*)pMassMemory;
	if (!m_pMassMemory)
		m_pMassMemory = (char*)dwUnitSize;



	m_dwMaxBlockNum = dwMaxBlockNum;

	m_pDescPool = CreateStaticMemoryPool();
	InitializeStaticMemoryPool(m_pDescPool,sizeof(MEMORY_BLOCK_DESC),dwMaxBlockNum/4,m_dwMaxBlockNum);
	
	m_pStatusDescLinkPool = CreateStaticMemoryPool();
	InitializeStaticMemoryPool(m_pStatusDescLinkPool,sizeof(MEMORY_STATUS_DESC_LINK),(m_dwMaxBlockNum*3)/4,m_dwMaxBlockNum*3);


	m_dwMemorySize = dwMassMemorySize;

	// 메모리 할당 단위
	m_dwAllocUnitSize = dwUnitSize;
	
	if (!dwAlignUnitNum)
	{
		dwAlignUnitNum = dwMassMemorySize;
		m_dwAlignBytes = dwMassMemorySize;
	}
	else
		m_dwAlignBytes = m_dwAllocUnitSize * dwAlignUnitNum;

	if (m_dwAlignBytes > m_dwMemorySize)
	{
		MessageBox(NULL,"CHeap::Initialize, dwAlignBytes > dwMassMemorySize","Error",MB_OK);
		__asm int 3
	}




	m_bEnableDebug = bEnableDebug;
	

	m_blockSearch.Initialize(m_dwMaxBlockNum);

	int		iTotalSize = (int)m_dwMemorySize;
	
	
	MEMORY_STATUS_DESC_LINK*	pStatusDesc;
	MEMORY_BLOCK_DESC*			pMemDesc;

	char* pMem = m_pMassMemory;
	
	ALIGN_SET*	pAlignSet;
	iTotalSize -= m_dwAlignBytes;
	
//	while ( (DWORD)pMemDesc->pMemory + pMemDesc->dwSize > dwMassMemorySize + (DWORD)m_pMassMemory )
	while(iTotalSize >= 0)
	{
		
		pAlignSet = new ALIGN_SET;

		pAlignSet->pHeadTerminator = (MEMORY_STATUS_DESC_LINK*)LALAlloc(m_pStatusDescLinkPool);
		pAlignSet->pTailTerminator = (MEMORY_STATUS_DESC_LINK*)LALAlloc(m_pStatusDescLinkPool);

		pAlignSet->pHeadTerminator->pPrv = NULL;
		pAlignSet->pHeadTerminator->bStatus = MEMORY_STATUS_TERMINATOR;

		pAlignSet->pTailTerminator->bStatus = MEMORY_STATUS_TERMINATOR;
		pAlignSet->pTailTerminator->pNext = NULL;



		pMemDesc = (MEMORY_BLOCK_DESC*)LALAlloc(m_pDescPool);
		pStatusDesc = (MEMORY_STATUS_DESC_LINK*)LALAlloc(m_pStatusDescLinkPool);

		pAlignSet->pHeadTerminator->pNext = pStatusDesc;
		pStatusDesc->pPrv = pAlignSet->pHeadTerminator;

		pAlignSet->pTailTerminator->pPrv = pStatusDesc;
		pStatusDesc->pNext = pAlignSet->pTailTerminator;

		
		pStatusDesc->dwSize = m_dwAlignBytes;
		pStatusDesc->pMemAddr = pMem;
		pStatusDesc->bStatus = MEMORY_STATUS_AVAILABLE;
		pStatusDesc->pMemDesc = pMemDesc;

		pMemDesc->dwSize = m_dwAlignBytes;
		pMemDesc->pStatusDesc = pStatusDesc;
		pMemDesc->bStatus = MEMORY_STATUS_AVAILABLE;
		pMemDesc->pMemory = pMem;

#ifdef _DEBUG
		if ((DWORD)pMemDesc->pMemory + pMemDesc->dwSize > dwMassMemorySize + (DWORD)m_pMassMemory)
			__asm int 3
#endif
		pMemDesc->pSearchHandle = m_blockSearch.InsertItem(pMemDesc->dwSize,pMemDesc);


		iTotalSize -= (int)m_dwAlignBytes;
		pMem += m_dwAlignBytes;

		pAlignSet->pNext = m_pAlignSet;
		m_pAlignSet = pAlignSet;
		m_dwAlignSetNum++;
	}
	// 짜투리 처리


	/////////////////
	
	DWORD dwNum = LALGetAllocatedNum(m_pDescPool);
	dwNum = LALGetAllocatedNum(m_pStatusDescLinkPool);

	
	bResult = TRUE;
lb_return:
	return bResult;
}
void* CHeap::AllocUnit(DWORD* pdwErrorCode,DWORD dwUnitNum,DWORD* pdwSize,BOOL bAlign)
{
	*pdwSize = dwUnitNum*m_dwAllocUnitSize;
	return Alloc(pdwErrorCode,*pdwSize,bAlign);
}

void* CHeap::Alloc(DWORD* pdwErrorCode,DWORD dwSize,BOOL bAlign)
{

	DWORD	dwErrorCode = 0;
	void*	pMem = NULL;

	if (bAlign)
	{
		// 얼라인 옵션이 들어오면 얼라인 사이즈보다 큰 블럭은 할당불가
		if (dwSize > m_dwAlignBytes)
		{
			dwErrorCode = HEAP_ALLOC_FAIL_ERROR_GREATOR_THAN_ALIGNSIZE;
			goto lb_return;
		}
	}
	MEMORY_BLOCK_DESC*	pMemDesc;

	pMemDesc = (MEMORY_BLOCK_DESC*)m_blockSearch.SearchItem(dwSize);


	if (!pMemDesc)
	{
		dwErrorCode = HEAP_ALLOC_FAIL_ERROR_NOT_AVAILABLE_MEM_BLOCK;
		goto lb_return;
	}

	m_blockSearch.DeleteItem(pMemDesc->pSearchHandle);
	pMemDesc->pSearchHandle = NULL;

	MEMORY_STATUS	oldStatus;
	oldStatus = (MEMORY_STATUS)pMemDesc->bStatus;

	pMemDesc->bStatus = MEMORY_STATUS_ALLOCATED;
	

//	FreeMemoryStatusDesc(pMemDesc->pStatusDesc);
//	pMemDesc->pStatusDesc = NULL;
	
	
	int	iAvlSize;
	iAvlSize = (int)(pMemDesc->dwSize) - (int)dwSize;;
	
	if (iAvlSize > 0)
	{
		// 메모리 블럭의 남는 부분이 새로운 메모리 블럭으로 사용 가능할 경우 새로운 디스크립터에 연결해서 링크시킨다.
		MEMORY_BLOCK_DESC*	pNewMemDesc = (MEMORY_BLOCK_DESC*)LALAlloc(m_pDescPool);
		if (!pNewMemDesc)
		{
			// 새로운 블럭을 할당할 수 없으므로 원상복구 
			pMemDesc->bStatus = oldStatus;
			pMemDesc->pSearchHandle = m_blockSearch.InsertItem(pMemDesc->dwSize,pMemDesc);
			dwErrorCode = HEAP_ALLOC_FAIL_ERROR_NOT_AVAILABLE_BLOCK_DESC;
			goto lb_return;
		}

		pNewMemDesc->dwSize = (DWORD)iAvlSize;
		pNewMemDesc->pMemory = pMemDesc->pMemory + dwSize;

#ifdef _DEBUG
		if ((DWORD)pMemDesc->pMemory + dwSize > m_dwMemorySize + (DWORD)m_pMassMemory)
			__asm int 3

		if ((DWORD)pNewMemDesc->pMemory + (DWORD)iAvlSize > m_dwMemorySize + (DWORD)m_pMassMemory)
			__asm int 3


#endif
		pNewMemDesc->bStatus = MEMORY_STATUS_AVAILABLE;

		MEMORY_STATUS_DESC_LINK*	pNewStatusDesc;
		pNewMemDesc->pStatusDesc = pNewStatusDesc = (MEMORY_STATUS_DESC_LINK*)LALAlloc(m_pStatusDescLinkPool);
		
		// 새 블럭디스크립터 링크에 메모리의 기본정보를 세팅
		pNewStatusDesc->pMemDesc = pNewMemDesc;
		pNewStatusDesc->pMemAddr = pNewMemDesc->pMemory;
		pNewStatusDesc->dwSize = pNewMemDesc->dwSize;
		pNewStatusDesc->bStatus = MEMORY_STATUS_AVAILABLE;
		
		// 새 블럭을 기존 블럭링크에 삽입
		pNewStatusDesc->pPrv = pMemDesc->pStatusDesc;
		pNewStatusDesc->pNext = pMemDesc->pStatusDesc->pNext;
		pMemDesc->pStatusDesc->pNext = pNewStatusDesc;
		pNewStatusDesc->pNext->pPrv = pNewStatusDesc;

		pNewMemDesc->pSearchHandle = m_blockSearch.InsertItem(pNewMemDesc->dwSize,pNewMemDesc);
	}
	
	
		

	// 기존 블럭의 사이즈를 세로 세팅 
	pMemDesc->dwSize = dwSize;
	pMemDesc->bStatus = MEMORY_STATUS_ALLOCATED;
	pMemDesc->pStatusDesc->dwSize = pMemDesc->dwSize;
	pMemDesc->pStatusDesc->pMemAddr = pMemDesc->pMemory;
	pMemDesc->pStatusDesc->bStatus = MEMORY_STATUS_ALLOCATED;
	pMem = pMemDesc->pMemory;
#ifdef _DEBUG
	if (bAlign)
	{
		if (!CheckAlign((char*)pMem,dwSize))
			__asm int 3
	}
#endif

	m_dwAllocatedNum++;
	m_dwAlloctedMemorySize += dwSize;

lb_return:

	if (pdwErrorCode)
		*pdwErrorCode = dwErrorCode;

	return pMem;
}



void CHeap::Free(void* pMem)
{
	DWORD	dwSize;

	MEMORY_BLOCK_DESC*			pMemDesc = GetMemortyBlockStatudDesc(pMem);
	MEMORY_STATUS_DESC_LINK*	pCurStatus = pMemDesc->pStatusDesc;
	MEMORY_STATUS_DESC_LINK*	pFrontStatus = pCurStatus->pPrv;
	MEMORY_STATUS_DESC_LINK*	pRearStatus = pCurStatus->pNext;


	MEMORY_BLOCK_DESC*			pMemDescFront = pFrontStatus->pMemDesc;
	MEMORY_BLOCK_DESC*			pMemDescRear = pRearStatus->pMemDesc;

	dwSize = pMemDesc->dwSize;

	if (pFrontStatus->bStatus == MEMORY_STATUS_AVAILABLE)
	{
		// 앞 블럭과 병합 가능
		// 앞 블럭을 트리에서 제거 
		m_blockSearch.DeleteItem(pMemDescFront->pSearchHandle);
		pMemDescFront->bStatus = MEMORY_STATUS_DESTROY;
		pMemDescFront->pStatusDesc = NULL;

		FreeMemoryStatusDesc(pFrontStatus);
		
		pMemDesc->dwSize += (pMemDescFront->dwSize);
		pMemDesc->pMemory = pMemDescFront->pMemory;

#ifdef _DEBUG
		if ((DWORD)pMemDesc->pMemory + pMemDesc->dwSize > m_dwMemorySize + (DWORD)m_pMassMemory)
			__asm int 3
#endif
			
		LALFree(m_pDescPool,pMemDescFront);
	}
	if (pRearStatus->bStatus == MEMORY_STATUS_AVAILABLE)
	{
		// 뒷 블럭과 병합 가능 
		// 뒷블럭을 트리에서 제거
		m_blockSearch.DeleteItem(pMemDescRear->pSearchHandle);
		pMemDescRear->bStatus = MEMORY_STATUS_DESTROY;

		pMemDesc->dwSize += (pMemDescRear->dwSize);
		
		FreeMemoryStatusDesc(pRearStatus);

		LALFree(m_pDescPool,pMemDescRear);
	}
	pMemDesc->bStatus = MEMORY_STATUS_AVAILABLE;
	// 상태 블럭 디스크립터 재설정 
	pCurStatus->dwSize = pMemDesc->dwSize;
	pCurStatus->pMemAddr = pMemDesc->pMemory;
	pCurStatus->bStatus = MEMORY_STATUS_AVAILABLE;

	pMemDesc->pSearchHandle = m_blockSearch.InsertItem(pMemDesc->dwSize,pMemDesc);


	m_dwAllocatedNum--;
	m_dwAlloctedMemorySize += dwSize;

}

BOOL CHeap::CheckAlign(char* pMem,DWORD dwSize)
{
	BOOL	bResult = TRUE;
	if (!pMem)
		goto lb_return;


	DWORD	dwFrontBoundaryIndex,dwRearBoundaryIndex,dwRelativeMemAddr;

	dwRelativeMemAddr = (DWORD)pMem - (DWORD)m_pMassMemory;

	dwFrontBoundaryIndex = dwRelativeMemAddr / m_dwAlignBytes;
	dwRearBoundaryIndex = (dwRelativeMemAddr + dwSize - 1) / m_dwAlignBytes;

	if ( dwFrontBoundaryIndex == dwRearBoundaryIndex )
		goto lb_return;

	bResult = FALSE;

lb_return:
	return bResult;
}



MEMORY_BLOCK_DESC* CHeap::GetMemortyBlockStatudDesc(void* pMemAddr)
{
	MEMORY_BLOCK_DESC*	pMemDesc = NULL;

	ALIGN_SET*	pAlignSetCur = m_pAlignSet;
	while (pAlignSetCur)
	{
		MEMORY_STATUS_DESC_LINK*	pCur = pAlignSetCur->pHeadTerminator->pNext;
		while (pCur)
		{

			if (pCur->pMemAddr == pMemAddr)
			{
				pMemDesc = pCur->pMemDesc;
				goto lb_return;
			}
			pCur = pCur->pNext;
		}
		pAlignSetCur = pAlignSetCur->pNext;
	}
lb_return:
#ifdef _DEBUG
	if (!pMemDesc)
		__asm int 3
#endif

	return pMemDesc;

}
void CHeap::LeakCheck()
{
	BOOL	bCheck = FALSE;;

	DWORD dwNum = LALGetAllocatedNum(m_pDescPool);
	dwNum = LALGetAllocatedNum(m_pStatusDescLinkPool);

	
	char txt[512];
	memset(txt,0,512);

	OutputDebugString("Begin LeakCheck\n");

	ALIGN_SET*	pAlignSetCur = m_pAlignSet;
	while (pAlignSetCur)
	{
		MEMORY_STATUS_DESC_LINK*	pStatusCur = pAlignSetCur->pHeadTerminator;
		while (pStatusCur)
		{
			if (!pStatusCur->pMemDesc)
				goto lb_skip;

			if (pStatusCur->pMemDesc->dwSize == m_dwAlignBytes)
				goto lb_skip;
			
			wsprintf(txt,"heap block not freed!!! address: %X , size : %d \n",pStatusCur->pMemDesc->pMemory,pStatusCur->pMemDesc->dwSize);
			OutputDebugString(txt);
			bCheck = TRUE;
	lb_skip:
			pStatusCur = pStatusCur->pNext;
		}
		pAlignSetCur = pAlignSetCur->pNext;
	}

lb_end:
	OutputDebugString("End LeakCheck\n");
	if (bCheck)
		__asm int 3
}

void CHeap::HeapCheck()
{
	/*
	if (!m_pItemTable)
		return;
	

	MEMORY_BLOCK_DESC*	pMemDesc;
	char txt[512];
	
	OutputDebugString("Begin HeapCheck\n");

	DWORD	i;
	DWORD dwNum;

	dwNum = ITGetItemNum(m_pItemTable);
	
	for (i=0; i<dwNum; i++)
	{
		pMemDesc = (MEMORY_BLOCK_DESC*)ITGetItemSequential(m_pItemTable,i);

		MEM_BLOCK_HEAD*		pMemHead;
		MEM_BLOCK_HEAD*		pMemTail;

		pMemHead = (MEM_BLOCK_HEAD*)( (char*)pMemDesc->pMemory - m_dwAlignBytes);
		pMemTail = (MEM_BLOCK_HEAD*)( (char*)pMemDesc->pMemory + pMemHead->dwSize );

		if (pMemDesc != pMemHead->pMemDesc)
			goto lb_crash;

		if (pMemDesc != pMemTail->pMemDesc)
			goto lb_crash;
		
		if (pMemDesc->dwSize != pMemHead->dwSize)
			goto lb_crash;

		if (pMemDesc->dwSize != pMemTail->dwSize)
			goto lb_crash;

		continue;

lb_crash:
		memset(txt,0,512);
		wsprintf(txt,"HeapBlock damaged address: %X , size : %d \n",pMemDesc->pMemory,pMemDesc->dwSize);
		OutputDebugString(txt);

	}
	
lb_end:
	OutputDebugString("End HeapCheck\n");*/
}

void CHeap::Release()
{

	ALIGN_SET*	pNext;
	ALIGN_SET*	pAlignSetCur = m_pAlignSet;
	DWORD	dwNum = 0;
	while (pAlignSetCur)
	{
		pNext = pAlignSetCur->pNext;
		delete pAlignSetCur;
		pAlignSetCur = pNext;
		dwNum++;
		
	}
	m_pAlignSet = NULL;

	if (m_pDescPool)
	{
		
		ReleaseStaticMemoryPool(m_pDescPool);
		m_pDescPool = NULL;
	}

	if (m_pStatusDescLinkPool)
	{
		ReleaseStaticMemoryPool(m_pStatusDescLinkPool);
		m_pStatusDescLinkPool = NULL;
	}
	


}

CHeap::~CHeap()
{
	Release();

}
