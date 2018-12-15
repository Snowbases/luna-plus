#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif

#include "global.h"
#include "various_bytes_hash.h"

DWORD ConvertString(char* szString);
DWORD CreateKey(void* pData,DWORD dwSampleNum,DWORD dwBucketNum);

GLOBAL_FUNC_DLL	VBHASH_HANDLE	__stdcall	VBHCreate()
{
	CVariousBytesHashTable* pHash = new CVariousBytesHashTable;
	return pHash;

}

CVariousBytesHashTable::CVariousBytesHashTable()
{
	m_pBucketTable = NULL;
	m_dwMaxBucketNum = 0;

}

GLOBAL_FUNC_DLL BOOL __stdcall VBHInitialize(VBHASH_HANDLE pHash,DWORD dwMaxBucketNum,DWORD dwMaxSize,DWORD dwMaxItemNum)
{
	CVariousBytesHashTable* pVBHash = (CVariousBytesHashTable*)pHash;

	pVBHash->m_dwMaxKeyDataSize = dwMaxSize;
	pVBHash->m_dwMaxBucketNum = dwMaxBucketNum;
	
	pVBHash->m_pBucketTable = new VB_BUCKET[dwMaxBucketNum];
	memset(pVBHash->m_pBucketTable,0,sizeof(VB_BUCKET)*dwMaxBucketNum);
	
	pVBHash->m_pStaticMemoryPool = CreateStaticMemoryPool();
	return InitializeStaticMemoryPool(pVBHash->m_pStaticMemoryPool,VB_BUCKET_DEFAULT_SIZE + dwMaxSize,dwMaxItemNum/4,dwMaxItemNum);
}
GLOBAL_FUNC_DLL DWORD __stdcall VBHSelect(VBHASH_HANDLE pHash,DWORD OUT* pItems,DWORD dwMaxItemNum,void* pKeyData,DWORD dwSize)
{
	VB_BUCKET*		pCurBucket;
	DWORD			dwResult;
	

	__asm
	{
		
		xor			eax,eax
		mov			ecx,dword ptr[dwMaxItemNum]	; dwMaxItemNum
		
		or			ecx,ecx
		jz			lb_return

		mov			ebx,pHash						; this
		mov			edx,dword ptr[ebx+8]		; pBase->m_dwMaxBucketNum
		mov			ecx,dword ptr[dwSize]		; dwSize
		mov			esi,dword ptr[pKeyData]		; pKeyData

		push		edx
		push		ecx
		push		esi

		call		CreateKey

//		mov			ebx,dword ptr[pHash]		; pBase
		mov			edx,VB_BUCKET_DEFAULT_SIZE
		mov			edi,dword ptr[ebx+4]		; pBase->m_pBucketTable
		mul			edx
		add			edi,eax						; pBase->m_pBucketTable+dwIndex;
		mov			edi,dword ptr[edi+8]
		xor			eax,eax
		mov			ebx,dword ptr[pItems]		; out buffer

lb_loop_search:
		or			edi,edi
		jz			lb_return
		
		mov			ecx,dword ptr[dwSize]
		mov			dword ptr[pCurBucket],edi	; backup pCurBucket
		
		cmp			ecx,dword ptr[edi+12]		; if (pCurBucket->m_dwSize != dwSize) 
		jnz			lb_back						;	  goto lb_back

		add			edi,VB_BUCKET_DEFAULT_SIZE
		mov			esi,dword ptr[pKeyData]

lb_cmp_1:
		test		ecx,1
		jz			lb_cmp_2
		cmpsb
		jnz			lb_back
		dec			ecx
		jz			lb_match

lb_cmp_2:
		test		ecx,2
		jz			lb_cmp_4
		cmpsw
		jnz			lb_back
		sub			ecx,2
		jz			lb_match

lb_cmp_4:
		shr			ecx,2
lb_loop_4:
		cmpsd
		jnz			lb_back
		loop		lb_loop_4
		
lb_match:
		mov			edi,dword ptr[pCurBucket] ; restore pCurBucket
		mov			ecx,dword ptr[edi]		; pCurBucket->m_dwItem	
		mov			dword ptr[ebx],ecx		; pItems[dwResult] = pCurBucket->m_dwItem
		inc			eax						; dwResult++
		add			ebx,4

		cmp			eax,dword ptr[dwMaxItemNum]
		jae			lb_return

lb_back:
		mov			edi,dword ptr[pCurBucket]; restore pCurBucket
		mov			edi,dword ptr[edi+8]
		jmp			lb_loop_search

lb_return:
		mov			dword ptr[dwResult],eax
	}

	return dwResult;
}
GLOBAL_FUNC_DLL void* __stdcall VBHInsert(VBHASH_HANDLE pHash,DWORD dwItem,void* pKeyData,DWORD dwSize)
{
	void*	pSearchHandle;
	__asm
	{
		xor			eax,eax
		mov			ebx,pHash					; this
		mov			edx,dword ptr[ebx+12]		; pBase->m_dwMaxKeyDataSize
		mov			ecx,dword ptr[dwSize]
		mov			esi,dword ptr[pKeyData]
		movd		mm2,ecx
		movd		mm3,esi

		cmp			ecx,edx
		ja			lb_return

		movd		mm0,ebx						; backup pBase
		mov			edx,dword ptr[ebx+8]		; pBase->m_dwMaxBucketNum
		
		push		edx
		push		ecx
		push		esi
		
		call		CreateKey
		
		movd		mm1,eax						; dwIndex
		movd		ebx,mm0						; pBase

		mov			edx,dword ptr[ebx+16]		; m_pLookAsideList
		push		edx
		call		LALAlloc
		
		
		or			eax,eax
		jz			lb_return

		mov			edi,eax						; pCurBucket
		movd		ebx,mm0						; pBase
		movd		mm4,eax						; bakcup pCurBucket

		mov			esi,dword ptr[ebx+4]		; pBase->m_pBucketTable
		movd		eax,mm1						; dwIndex
		mov			ecx,VB_BUCKET_DEFAULT_SIZE
		
		mul			ecx
		add			esi,eax						; pBase->m_pBucketTable+dwIndex

		mov			edx,dword ptr[esi+8]		; pBaseBucket->m_pNext
		or			edx,edx						; if (pBaseBucket->m_pNext)
		jz			lb_step_1					;    pBaseBucket->m_pNext->m_pPrv = pCurBucket;

		mov			dword ptr[edx+4],edi		; 

lb_step_1:
		mov			dword ptr[edi+8],edx		; link setting
		mov			dword ptr[edi+4],esi
		mov			dword ptr[esi+8],edi

		mov			ebx,dword ptr[dwItem]		; dwItem
		mov			dword ptr[edi],ebx

		movd		dword ptr[edi+12],mm2		; dwSize
		
		movd		ecx,mm2						; restore dwSize

		add			edi,VB_BUCKET_DEFAULT_SIZE
		movd		esi,mm3						; restore pKeyData

lb_cpy_1:
		test		ecx,1
		jz			lb_cpy_2
		movsb
		dec			ecx
		jz			lb_end

lb_cpy_2:
		test		ecx,2
		jz			lb_cpy_4
		movsw
		sub			ecx,2
		jz			lb_end

lb_cpy_4:
		shr			ecx,2
		rep			movsd
lb_end:
		movd		eax,mm4						; pCurBucket
	
lb_return:
		emms
		mov			dword ptr[pSearchHandle],eax
	}
	return pSearchHandle;



}
GLOBAL_FUNC_DLL BOOL __stdcall VBHDelete(VBHASH_HANDLE pHash,void* pBucket)
{
	CVariousBytesHashTable* pVBHash = (CVariousBytesHashTable*)pHash;

	VB_BUCKET*		pCurBucket = (VB_BUCKET*)pBucket;
	
	pCurBucket->m_pPrv->m_pNext = pCurBucket->m_pNext;
	if (pCurBucket->m_pNext)
		pCurBucket->m_pNext->m_pPrv = pCurBucket->m_pPrv;
	
	LALFree(pVBHash->m_pStaticMemoryPool,pCurBucket);
		
	return TRUE;
}
GLOBAL_FUNC_DLL DWORD __stdcall	VBHDeleteWithKey(VBHASH_HANDLE pHash,void* pKeyData,DWORD dwSize,DelFunc pDelFunc)
{

	VB_BUCKET*	pBucket;
	DWORD	dwDelCount = 0;
	while(pBucket = (VB_BUCKET*)VBHGetSearchHandle(pHash,pKeyData,dwSize))
	{
		if (pDelFunc)
			pDelFunc( (void*)pBucket->m_dwItem);
		
		VBHDelete(pHash,pBucket);
		dwDelCount++;
	}
	return dwDelCount;
}

GLOBAL_FUNC_DLL void* __stdcall VBHGetSearchHandle(VBHASH_HANDLE pHash,void* pKeyData,DWORD dwSize)
{
	VB_BUCKET*		pCurBucket;
	void*			pSearchHandle;
	

	__asm
	{
		
		xor			eax,eax
		
		mov			ebx,pHash						; this
		mov			edx,dword ptr[ebx+8]		; pBase->m_dwMaxBucketNum
		mov			ecx,dword ptr[dwSize]		; dwSize
		mov			esi,dword ptr[pKeyData]		; pKeyData

		push		edx
		push		ecx
		push		esi

		call		CreateKey

		mov			edx,VB_BUCKET_DEFAULT_SIZE
		mov			edi,dword ptr[ebx+4]		; pBase->m_pBucketTable
		mul			edx
		add			edi,eax						; pBase->m_pBucketTable+dwIndex;
		mov			edi,dword ptr[edi+8]
		xor			eax,eax
		

lb_loop_search:
		or			edi,edi
		jz			lb_return
		
		mov			ecx,dword ptr[dwSize]
		mov			dword ptr[pCurBucket],edi	; backup pCurBucket
		
		cmp			ecx,dword ptr[edi+12]		; if (pCurBucket->m_dwSize != dwSize) 
		jnz			lb_back						;	  goto lb_back

		add			edi,VB_BUCKET_DEFAULT_SIZE
		mov			esi,dword ptr[pKeyData]

lb_cmp_1:
		test		ecx,1
		jz			lb_cmp_2
		cmpsb
		jnz			lb_back
		dec			ecx
		jz			lb_match

lb_cmp_2:
		test		ecx,2
		jz			lb_cmp_4
		cmpsw
		jnz			lb_back
		sub			ecx,2
		jz			lb_match

lb_cmp_4:
		shr			ecx,2
lb_loop_4:
		cmpsd
		jnz			lb_back
		loop		lb_loop_4
		
lb_match:
		mov			eax,dword ptr[pCurBucket] ; restore pCurBucket
		jae			lb_return

lb_back:
		mov			edi,dword ptr[pCurBucket]; restore pCurBucket
		mov			edi,dword ptr[edi+8]
		jmp			lb_loop_search

lb_return:
		mov			dword ptr[pSearchHandle],eax
	}

	return pSearchHandle;
}
GLOBAL_FUNC_DLL	void __stdcall VBHRelease(VBHASH_HANDLE pHash)
{
	CVariousBytesHashTable* pVBHash = (CVariousBytesHashTable*)pHash;

	VBHDeleteAll(pVBHash);
	if (pVBHash->m_pBucketTable)
	{
		delete [] pVBHash->m_pBucketTable;
		pVBHash->m_pBucketTable = NULL;
	}
	if (pVBHash->m_pStaticMemoryPool)
	{
		ReleaseStaticMemoryPool(pVBHash->m_pStaticMemoryPool);
		pVBHash->m_pStaticMemoryPool = NULL;
	}
	delete pVBHash;

}
GLOBAL_FUNC_DLL	DWORD __stdcall	VBHGetMaxBucketNum(VBHASH_HANDLE pHash)
{
	CVariousBytesHashTable* pVBHash = (CVariousBytesHashTable*)pHash;
	
	return pVBHash->m_dwMaxBucketNum;
}


GLOBAL_FUNC_DLL void __stdcall	VBHDeleteAll(VBHASH_HANDLE pHash)
{
	CVariousBytesHashTable* pVBHash = (CVariousBytesHashTable*)pHash;

	VB_BUCKET*		pCurBucket;
	VB_BUCKET*		pNext;

	for (DWORD i=0; i<pVBHash->m_dwMaxBucketNum; i++)
	{
		pCurBucket = (pVBHash->m_pBucketTable + i)->m_pNext;
		while (pCurBucket)
		{
			pNext = pCurBucket->m_pNext;
			LALFree(pVBHash->m_pStaticMemoryPool,pCurBucket);
			pCurBucket = pNext;
		}
	}
}
GLOBAL_FUNC_DLL DWORD __stdcall	VBHGetAllItem(VBHASH_HANDLE pHash,DWORD* pdwItemList,DWORD dwMaxItemNum)
{
	CVariousBytesHashTable* pVBHash = (CVariousBytesHashTable*)pHash;

	VB_BUCKET*		pCurBucket;

	DWORD			dwItemNum = 0;

	for (DWORD i=0; i<pVBHash->m_dwMaxBucketNum; i++)
	{
		pCurBucket = (pVBHash->m_pBucketTable + i)->m_pNext;
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
GLOBAL_FUNC_DLL void* __stdcall VBHGetKeyPtr(void* pSearchHandle)
{
	return (void*)( (char*)pSearchHandle+VB_BUCKET_DEFAULT_SIZE);
}

DWORD __declspec(naked) CreateKey(void* pData,DWORD dwSize,DWORD dwBucketNum)
{
	__asm
	{
//		enter		0,0
//		push		esi
		push		ebx
		push		ecx
		push		edx
		push		esi


		xor			eax,eax
//		mov			ecx,dword ptr[dwSize]
//		mov			esi,dword ptr[pData]
		
		mov			esi,dword ptr[esp+4+4*4]	; pData
		mov			ecx,dword ptr[esp+8+4*4]	; dwSize
lb_1:
		test		ecx,1
		jz			lb_2

		mov			dl,byte ptr[esi]
		or			al,dl
		inc			esi
		dec			ecx
		jz			lb_end

lb_2:
		test		ecx,2
		jz			lb_4

		mov			dx,word ptr[esi]
		or			ax,dx
		add			esi,2
		sub			ecx,2
		jz			lb_end

lb_4:
		shr			ecx,2
lb_4_loop:
		mov			edx,dword ptr[esi]
		or			eax,edx
		loop		lb_4_loop

lb_end:
		mov			ebx,dword ptr[esp+12+4*4]	; dwMaxBucketNum
		xor			edx,edx
		div			ebx

		mov			eax,edx

		pop			esi
		pop			edx
		pop			ecx
		pop			ebx
		
		ret			4*3
	}
}

/*
DWORD CVariousBytesHashTable::Select(DWORD OUT* pItems,DWORD dwMaxItemNum,void* pKeyData,DWORD dwSize)
{
	DWORD	dwResult = 0;
	VB_BUCKET*	pBucket = VBGetBucket(this,pKeyData,dwSize);
	if (!pBucket)
		goto lb_return;

	*pItems = pBucket->m_dwItem;
	dwResult = 1;


lb_return:
	return dwResult;

}
*/

/*
VB_BUCKET*	VBInsert(CVariousBytesHashTable* pBase,DWORD dwItem,void* pKeyData,DWORD dwSize)
{
	VB_BUCKET*		pCurBucket = NULL;
	VB_BUCKET*		pBaseBucket;
	DWORD			dwIndex;

  	dwIndex = CreateKey(pKeyData,dwSize,pBase->m_dwMaxBucketNum);

	if (dwSize > pBase->m_dwMaxKeyDataSize)
		goto lb_return;
		
	pCurBucket = (VB_BUCKET*)LALAlloc(pBase->m_pLookAsideList);
	
	if (!pCurBucket)
		goto lb_return;

	
	pBaseBucket = pBase->m_pBucketTable+dwIndex;
	
	
	if (pBaseBucket->m_pNext)
		pBaseBucket->m_pNext->m_pPrv = pCurBucket;

	pCurBucket->m_pNext = pBaseBucket->m_pNext;
	pCurBucket->m_pPrv = pBaseBucket;
	pBaseBucket->m_pNext = pCurBucket;
	pCurBucket->m_dwItem = dwItem;
	pCurBucket->m_dwSize = dwSize;
	

//	memcpy(pCurBucket->m_pKeyData,pKeyData,dwSize);
	memcpy(((char*)pCurBucket)+16,pKeyData,dwSize);

	
lb_return:
	return pCurBucket;
}*/

