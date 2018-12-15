#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif

// TempImageMapManager.cpp: implementation of the CTempImageMapManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StaticResourcePool.h"

GLOBAL_FUNC_DLL STRCPOOL_HANDLE __stdcall CreateResourcePool()
{
	CStaticResourcePool*	pRCPool = new CStaticResourcePool;
	return (STRCPOOL_HANDLE)pRCPool;

}
GLOBAL_FUNC_DLL BOOL __stdcall RCPInitialize(STRCPOOL_HANDLE pHandle,DWORD dwMaxItemNum,RESOURCR_ALLOC_FUNC pAllocFunc,RESOURCR_FREE_FUNC pFreeFunc)
{
	CStaticResourcePool*	pRCPool = (CStaticResourcePool*)pHandle;
	return pRCPool->Initialize(dwMaxItemNum,pAllocFunc,pFreeFunc);
}

GLOBAL_FUNC_DLL void* __stdcall RCPAlloc(STRCPOOL_HANDLE pHandle,BOOL* pbEnable,DWORD* pdwResourceHandle,void* pArg1,void* pArg2,void* pArg3,DWORD dwSize)
{
	CStaticResourcePool*	pRCPool = (CStaticResourcePool*)pHandle;
	return pRCPool->Alloc(pbEnable,pdwResourceHandle,pArg1,pArg2,pArg3,dwSize);
}
GLOBAL_FUNC_DLL void __stdcall RCPFree(STRCPOOL_HANDLE pHandle,DWORD dwResourceHandle)
{
	CStaticResourcePool*	pRCPool = (CStaticResourcePool*)pHandle;
	pRCPool->Free(dwResourceHandle);
}
GLOBAL_FUNC_DLL void __stdcall RCPFreeAll(STRCPOOL_HANDLE pHandle)
{
	CStaticResourcePool*	pRCPool = (CStaticResourcePool*)pHandle;
	pRCPool->FreeAll();
}
GLOBAL_FUNC_DLL void __stdcall RCPRelease(STRCPOOL_HANDLE pHandle)
{
	delete ((CStaticResourcePool*)pHandle);
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CStaticResourcePool::ClearMember()
{
	m_pItemPool = NULL;

	m_pSortLinkHead = NULL;
	m_pSortLinkTail = NULL;
	
	m_pItemPool = NULL;
	m_pAllocFunc = NULL;
	m_pFreeFunc = NULL;

}
CStaticResourcePool::CStaticResourcePool()
{
	ClearMember();

}


BOOL CStaticResourcePool::Initialize(DWORD dwMaxItemNum,RESOURCR_ALLOC_FUNC pAllocFunc,RESOURCR_FREE_FUNC pFreeFunc)
{
	BOOL	bResult = FALSE;

	m_pAllocFunc = pAllocFunc;
	m_pFreeFunc = pFreeFunc;
	m_pItemPool = CreateStaticMemoryPool();
	InitializeStaticMemoryPool(m_pItemPool,sizeof(STATIC_RESOURCE_ITEM),dwMaxItemNum/4+1,dwMaxItemNum); 
	

	bResult = TRUE;
lb_return:
	return bResult;
}
void* CStaticResourcePool::Alloc(BOOL* pbEnable,DWORD* pdwResourceHandle,void* pArg1,void* pArg2,void* pArg3,DWORD dwSize)
{
lb_alloc:
	STATIC_RESOURCE_ITEM*	pItem= (STATIC_RESOURCE_ITEM*)LALAlloc(m_pItemPool);

	if (!pItem)
	{
		if (!m_pSortLinkHead)
			goto lb_return;

		Free((DWORD)m_pSortLinkHead->pItem);
		goto lb_alloc;
	}



	pItem->pVoid = m_pAllocFunc(pArg1,pArg2,pArg3,dwSize);
	pItem->sortLink.pItem = (void*)pItem;
	LinkToSortLink(&m_pSortLinkHead,&m_pSortLinkTail,&pItem->sortLink);
	pItem->pbEnable = pbEnable;

	if (pdwResourceHandle)
		*pdwResourceHandle = (DWORD)pItem;
	
lb_return:
	return pItem;
}

void CStaticResourcePool::FreeAll()
{
	SORT_LINK*	pCur = m_pSortLinkHead;
	SORT_LINK*	pNext;
	
	while (pCur)
	{
		pNext = pCur;
		Free((DWORD)pCur->pItem);
		pCur = pNext;
	}
}
void CStaticResourcePool::Free(DWORD dwResourceHandle)
{
	STATIC_RESOURCE_ITEM* pItem = (STATIC_RESOURCE_ITEM*)dwResourceHandle;
	*pItem->pbEnable = FALSE;
	m_pFreeFunc(pItem->pVoid);
	UnLinkFromSortLink(&m_pSortLinkHead,&m_pSortLinkTail,&pItem->sortLink);
	LALFree(m_pItemPool,pItem);
}

void CStaticResourcePool::Cleanup()
{
	FreeAll();
	if (m_pItemPool)
	{
		ReleaseStaticMemoryPool(m_pItemPool);
	}
	ClearMember();

}

CStaticResourcePool::~CStaticResourcePool()
{
	Cleanup();

}
