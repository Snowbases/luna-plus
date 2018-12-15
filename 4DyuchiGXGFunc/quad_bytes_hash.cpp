#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif


#include "global.h"
#include "quad_bytes_hash.h"

/*
GLOBAL_FUNC_DLL DWORD			__stdcall	QBHSelect(QBHASH_HANDLE pHash,DWORD OUT* pItems,DWORD dwMaxItemNum,void* pKeyData,DWORD dwSize);
GLOBAL_FUNC_DLL void*			__stdcall	QBHInsert(QBHASH_HANDLE pHash,DWORD dwItem,void* pKeyData,DWORD dwSize);


GLOBAL_FUNC_DLL	QBHASH_HANDLE	__stdcall	QBHCreate();
GLOBAL_FUNC_DLL BOOL			__stdcall	QBHInitialize(QBHASH_HANDLE pHash,DWORD dwMaxBucketNum,DWORD dwMaxItemNum);

GLOBAL_FUNC_DLL	DWORD			__stdcall	QBHGetMaxBucketNum(QBHASH_HANDLE pHash);
GLOBAL_FUNC_DLL	DWORD			__stdcall	QBDeleteAll(QBHASH_HANDLE pHash);
*/
CQuadBytesHashTable::CQuadBytesHashTable()
{
	m_dwRefCount = 0;
	m_pBucketTable = NULL;
	m_dwMaxBucketNum = 0;
	m_pStaticMemoryPool = NULL;
}

GLOBAL_FUNC_DLL	QBHASH_HANDLE __stdcall	QBHCreate()
{
	CQuadBytesHashTable*	pQBHash = new CQuadBytesHashTable;
	return (QBHASH_HANDLE)pQBHash;
}

GLOBAL_FUNC_DLL BOOL __stdcall QBHInitialize(QBHASH_HANDLE pHash,DWORD dwMaxBucketNum,DWORD dwMaxItemNum)
{
	CQuadBytesHashTable* pQBHash = (CQuadBytesHashTable*)pHash;


	pQBHash->m_dwMaxBucketNum = dwMaxBucketNum;
	pQBHash->m_pBucketTable = new QB_BUCKET[dwMaxBucketNum];
	memset(pQBHash->m_pBucketTable,0,sizeof(QB_BUCKET)*dwMaxBucketNum);

	pQBHash->m_pStaticMemoryPool = CreateStaticMemoryPool();
	return InitializeStaticMemoryPool(pQBHash->m_pStaticMemoryPool,sizeof(QB_BUCKET),dwMaxItemNum/4,dwMaxItemNum);
}
GLOBAL_FUNC_DLL __declspec(naked) DWORD __stdcall QBHSelect(QBHASH_HANDLE pHash,void** ppHashHandle,DWORD OUT* pItems,DWORD dwMaxItemNum,DWORD dwKeyData)
{
	__asm
	{
		enter		0,0

		push		esi
		push		edi
		push		ebx
		push		ecx
		push		edx

		
		
		
		mov			ebx,dword ptr[pHash]							; this

		mov			edx,dword ptr[dwMaxItemNum]
		xor			eax,eax
	
		or			edx,edx
		jz			lb_return

		movd		mm1,edx
		
		xor			edx,edx
		mov			eax,dword ptr[dwKeyData]		; dwKeyData 
		movd		mm0,eax							; backup pKeyData

		mov			edi,dword ptr[ebx+8]			; m_dwMaxBucketNum
		div			edi
		mov			eax,edx
		mov			edi,QB_BUCKET_SIZE
		xor			edx,edx
		mul			edi

		mov			edi,dword ptr[ebx+4]			; m_pBucketTable
		add			edi,eax							; m_pBucketTable + dwIndex

		mov			edx,dword ptr[edi+12]			; pBaseBucket->m_pNext

		xor			eax,eax
		movd		ebx,mm0							; dwKey
		mov			edi,dword ptr[pItems]			; out buffer
		movd		esi,mm1							; dwMaxItemNum

lb_loop_search:
		or			edx,edx
		jz			lb_return

		cmp			ebx,dword ptr[edx]				
		jnz			lb_back

		inc			eax

		; pCurBucket 
		push		esi
		mov			esi,dword ptr[ppHashHandle]
		mov			dword ptr[esi],edx
		pop			esi

		movd		mm2,dword ptr[edx+4]			; pCurBucket->m_dwItem
		movd		dword ptr[edi],mm2
		add			edi,4

		cmp			eax,esi
		jae			lb_return						

lb_back:
	
		mov			edx,dword ptr[edx+12]			; pCurBucket = pCurBucket->m_pNext
		jmp			lb_loop_search
lb_return:
		emms

		pop			edx
		pop			ecx
		pop			ebx
		pop			edi
		pop			esi
		
		leave
		ret			4*5
	}
}

GLOBAL_FUNC_DLL __declspec(naked) void* __stdcall QBHInsert(QBHASH_HANDLE pHash,DWORD dwItem,DWORD dwKeyData)
{
	__asm
	{
		enter		0,0
		push		esi
		push		edi
		push		ebx
		push		ecx
		push		edx
		
		mov			ebx,dword ptr[pHash]		; this
		mov			edx,dword ptr[ebx+12]		; m_pLookAsideList
		movd		mm0,ebx

		push		edx
		call		LALAlloc
				
		or			eax,eax
		jz			lb_return

		mov			edi,eax						; pCurDesc
	
		movd		ebx,mm0
		
		xor			edx,edx
		mov			eax,dword ptr[dwKeyData]	; dwKeyData
		mov			ecx,dword ptr[ebx+8]		; pTable->m_pBucketTable+dwIndex;
		movd		mm1,eax						; backup dwKey
				
		div			ecx
		mov			eax,edx
		mov			ecx,QB_BUCKET_SIZE
		mul			ecx

		; eax = offset
		mov			esi,dword ptr[ebx+4]		; pTable->m_pBucketTable+dwIndex
		add			esi,eax
		mov			edx,dword ptr[esi+12]		; pBaseBucket->m_pNext
		
		; esi = pBaseBucket
		; edx = pBaseBucket->m_pNext
		; edi = pCurBucket

		or		edx,edx							; 	if (pBaseBucket->m_pNext)
		jz		lb_step_1						;      	pBaseBucket->m_pNext->m_pPrv = pCurBucket;

		mov		dword ptr[edx+8],edi
lb_step_1:
		
		mov		dword ptr[edi+12],edx
		mov		dword ptr[edi+8],esi
		mov		dword ptr[esi+12],edi

		mov		ebx,dword ptr[dwItem]
		
		movd	dword ptr[edi],mm1				; dwKey
		mov		dword ptr[edi+4],ebx			; dwItem

		mov		eax,edi

lb_return:
		emms

		pop		edx
		pop		ecx
		pop		ebx
		pop		edi
		pop		esi
		
		leave
		ret		4*3
	}
}
GLOBAL_FUNC_DLL BOOL __stdcall 	QBHDelete(QBHASH_HANDLE pHash,void* pBucket)
{
	CQuadBytesHashTable*	pQBHash = (CQuadBytesHashTable*)pHash;

	QB_BUCKET*		pCurBucket = (QB_BUCKET*)pBucket;
		
	pCurBucket->m_pPrv->m_pNext = pCurBucket->m_pNext;
	if (pCurBucket->m_pNext)
		pCurBucket->m_pNext->m_pPrv = pCurBucket->m_pPrv;
	
	LALFree(pQBHash->m_pStaticMemoryPool,pBucket);
	
	return TRUE;
}
GLOBAL_FUNC_DLL	void __stdcall QBHRelease(QBHASH_HANDLE pHash)
{
	CQuadBytesHashTable*	pQBHash = (CQuadBytesHashTable*)pHash;

	QBDeleteAll(pQBHash);
	if (pQBHash->m_pBucketTable)
	{
		delete [] pQBHash->m_pBucketTable;
		pQBHash->m_pBucketTable = NULL;
	}
	if (pQBHash->m_pStaticMemoryPool)
	{
		ReleaseStaticMemoryPool(pQBHash->m_pStaticMemoryPool);
		pQBHash->m_pStaticMemoryPool = NULL;
	}
	delete pQBHash;


}
GLOBAL_FUNC_DLL	DWORD __stdcall QBHGetMaxBucketNum(QBHASH_HANDLE pHash)
{
	CQuadBytesHashTable*	pQBHash = (CQuadBytesHashTable*)pHash;

	return pQBHash->m_dwMaxBucketNum;
}
GLOBAL_FUNC_DLL DWORD __stdcall	QBHGetAllItem(VBHASH_HANDLE pHash,DWORD* pdwItemList,DWORD dwMaxItemNum)
{
	CQuadBytesHashTable* pQBHash = (CQuadBytesHashTable*)pHash;

	QB_BUCKET*		pCurBucket;

	DWORD			dwItemNum = 0;

	for (DWORD i=0; i<pQBHash->m_dwMaxBucketNum; i++)
	{
		pCurBucket = (pQBHash->m_pBucketTable + i)->m_pNext;
		while (pCurBucket)
		{
						
			if (dwItemNum >= dwMaxItemNum)
				goto lb_return;

			pdwItemList[dwItemNum] = pCurBucket->m_dwItem;
			dwItemNum++;
			pCurBucket = pCurBucket->m_pNext;
		}
	}
lb_return:
	return dwItemNum;
}

GLOBAL_FUNC_DLL void __stdcall QBDeleteAll(QBHASH_HANDLE pHash)
{

	CQuadBytesHashTable*	pQBHash = (CQuadBytesHashTable*)pHash;

	QB_BUCKET*		pCurBucket;
	QB_BUCKET*		pNext;

	for (DWORD i=0; i<pQBHash->m_dwMaxBucketNum; i++)
	{
		pCurBucket = (pQBHash->m_pBucketTable + i)->m_pNext;
		while (pCurBucket)
		{
			pNext = pCurBucket->m_pNext;
			LALFree(pQBHash->m_pStaticMemoryPool,pCurBucket);
			pCurBucket = pNext;
		}
	}
}

/*
DWORD CQuadBytesHashTable::Select(DWORD OUT* pItems,DWORD dwMaxItemNum,void* pKeyData,DWORD dwSize)
{

	while(pCurBucket)
	{

		if (pCurBucket->m_dwKey == (*(DWORD*)pKeyData))
		{
			pItems[dwResult] = pCurBucket->m_dwItem;
			dwResult++;
			
			if (dwResult >= dwMaxItemNum)
				goto	lb_return;
		}
		pCurBucket = pCurBucket->m_pNext;
	}
lb_return:
	return dwResult;
}	
*/
/*
QB_BUCKET* QBInsert(CQuadBytesHashTable* pTable,DWORD dwItem,DWORD dwKey)
{
	
	DWORD	dwIndex = dwKey%pTable->m_dwMaxBucketNum;
	
	QB_BUCKET*		pBaseBucket = pTable->m_pBucketTable+dwIndex;
	QB_BUCKET*		pCurBucket = NULL;

	pCurBucket = (QB_BUCKET*)LALAlloc(pTable->m_pLookAsideList);
		
	if (!pCurBucket)
		goto lb_return;
	
	if (pBaseBucket->m_pNext)
		pBaseBucket->m_pNext->m_pPrv = pCurBucket;

	pCurBucket->m_pNext = pBaseBucket->m_pNext;
	pCurBucket->m_pPrv = pBaseBucket;
	pBaseBucket->m_pNext = pCurBucket;
	pCurBucket->m_dwItem = dwItem;
	pCurBucket->m_dwKey = dwKey;

lb_return:
	return pCurBucket;
}
*/
