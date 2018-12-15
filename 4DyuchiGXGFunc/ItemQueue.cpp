#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif


#include "global.h"
#include "ItemQueue.h"


CItemQueue::CItemQueue()
{
	memset(this,0,sizeof(CItemQueue));
}


CItemQueue::~CItemQueue()
{
	if (m_pItemBuffer)
	{
		delete [] m_pItemBuffer;
		m_pItemBuffer = NULL;
	}
	for (DWORD i=0; i<m_dwMaxTypeNum; i++)
	{
		if (m_pIndexBufferPerItem[i].pIndexBuffer)
		{
			delete [] m_pIndexBufferPerItem[i].pIndexBuffer;
		}
	}
	delete [] m_pIndexBufferPerItem;
}

GLOBAL_FUNC_DLL ITEMQ_HANDLE __stdcall CreateItemQueue()
{
	CItemQueue*		pQ = new CItemQueue;
	return pQ;
}
GLOBAL_FUNC_DLL void __stdcall ReleaseItemQueue(ITEMQ_HANDLE pQ)
{
	if (pQ)
	{
		delete ((CItemQueue*)pQ);
	}
}

GLOBAL_FUNC_DLL DWORD __stdcall GetItemNum(ITEMQ_HANDLE pQ)
{
	return ((CItemQueue*)pQ)->m_dwWritedItemNum;
}
GLOBAL_FUNC_DLL DWORD __stdcall GetItemNumWithType(ITEMQ_HANDLE pQ,DWORD dwTypeID)	
{
	return ((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].dwIndexNum;
}
GLOBAL_FUNC_DLL void __stdcall ResetReadPosition(ITEMQ_HANDLE pQ)	
{
	((CItemQueue*)pQ)->m_dwReadedItemNum = 0;
	((CItemQueue*)pQ)->m_dwCurrentReadOffset = 0;
}

GLOBAL_FUNC_DLL BOOL __stdcall BeginInitialize(ITEMQ_HANDLE pQ,DWORD dwMaxTypeNum)
{
	((CItemQueue*)pQ)->m_dwMaxTypeNum = dwMaxTypeNum;
	((CItemQueue*)pQ)->m_pIndexBufferPerItem = new ITEM_INDEX_BUFFER[dwMaxTypeNum];
	memset(((CItemQueue*)pQ)->m_pIndexBufferPerItem,0,sizeof(ITEM_INDEX_BUFFER)*dwMaxTypeNum);
	
	return TRUE;
}

GLOBAL_FUNC_DLL BOOL __stdcall SetType(ITEMQ_HANDLE pQ,DWORD dwTypeID,DWORD dwItemSize,void* pFunc,DWORD dwMaxNum)
{

	// dwTypeID 는 컬럼 인덱스이므로 type종류갯수보다 작은 수여야 한다.
	BOOL		bResult = FALSE;

	if (dwTypeID >= ((CItemQueue*)pQ)->m_dwMaxTypeNum)
		goto lb_return;

	((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].dwTypeID = dwTypeID;
	((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].pIndexBuffer = new DWORD[dwMaxNum];
	((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].dwMaxIndexNum = dwMaxNum;
	((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].dwItemSize = dwItemSize;
	((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].pFunc = pFunc;

	bResult = TRUE;
lb_return:
	return bResult;

}
GLOBAL_FUNC_DLL void __stdcall EndInitialize(ITEMQ_HANDLE pQ)
{
	for (DWORD i=0; i<((CItemQueue*)pQ)->m_dwMaxTypeNum; i++)
	{
		((CItemQueue*)pQ)->m_dwMaxBufferSize += ( (((CItemQueue*)pQ)->m_pIndexBufferPerItem[i].dwItemSize + sizeof(DWORD)) * ((CItemQueue*)pQ)->m_pIndexBufferPerItem[i].dwMaxIndexNum);
	}
	((CItemQueue*)pQ)->m_pItemBuffer = new char[((CItemQueue*)pQ)->m_dwMaxBufferSize];
}
GLOBAL_FUNC_DLL BOOL __stdcall PushItem(ITEMQ_HANDLE pQ,DWORD dwTypeID,char* pItem)
{
	BOOL	bResult = FALSE;
	DWORD	dwSize;
	char*	pBuffer;
	
	if (((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].dwIndexNum >= ((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].dwMaxIndexNum)
		goto lb_return;

	dwSize = ((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].dwItemSize;
	pBuffer = ((CItemQueue*)pQ)->m_pItemBuffer + ((CItemQueue*)pQ)->m_dwCurrentWriteOffset;

	*(DWORD*)pBuffer = dwTypeID;
	pBuffer += sizeof(DWORD);
	memcpy(pBuffer,pItem,dwSize);
	((CItemQueue*)pQ)->m_dwCurrentWriteOffset += (dwSize + sizeof(DWORD));

	((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].pIndexBuffer[((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].dwIndexNum] = (DWORD)pBuffer;
	((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].dwIndexNum++;
	((CItemQueue*)pQ)->m_dwWritedItemNum++;

	bResult = TRUE;

lb_return:
	return bResult;
}

GLOBAL_FUNC_DLL char* __stdcall GetItemSeq(ITEMQ_HANDLE pQ,DWORD* pdwTypeID,void** ppFunc)
{
	char*	pItem = NULL;
	DWORD	dwTypeID;

	if (((CItemQueue*)pQ)->m_dwReadedItemNum >= ((CItemQueue*)pQ)->m_dwWritedItemNum)
		goto lb_return;

	dwTypeID = *pdwTypeID = *(DWORD*)(((CItemQueue*)pQ)->m_pItemBuffer+((CItemQueue*)pQ)->m_dwCurrentReadOffset);
	pItem = ((CItemQueue*)pQ)->m_pItemBuffer + ((CItemQueue*)pQ)->m_dwCurrentReadOffset + sizeof(DWORD);
	((CItemQueue*)pQ)->m_dwCurrentReadOffset += (((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].dwItemSize + sizeof(DWORD));
	*ppFunc = ((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].pFunc;

	((CItemQueue*)pQ)->m_dwReadedItemNum++;

lb_return:
	return pItem;
}
GLOBAL_FUNC_DLL char* __stdcall GetItem(ITEMQ_HANDLE pQ,void** ppFunc,DWORD dwTypeID,DWORD dwSeqIndex)
{
	*ppFunc = ((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].pFunc;
	return (char*)((CItemQueue*)pQ)->m_pIndexBufferPerItem[dwTypeID].pIndexBuffer[dwSeqIndex];
}

GLOBAL_FUNC_DLL void __stdcall Clear(ITEMQ_HANDLE pQ)
{
	((CItemQueue*)pQ)->m_dwCurrentReadOffset = 0;
	((CItemQueue*)pQ)->m_dwCurrentWriteOffset = 0;
	((CItemQueue*)pQ)->m_dwWritedItemNum = 0;
	((CItemQueue*)pQ)->m_dwReadedItemNum = 0;
	
	for (DWORD i=0; i<((CItemQueue*)pQ)->m_dwMaxTypeNum; i++)
		((CItemQueue*)pQ)->m_pIndexBufferPerItem[i].dwIndexNum = 0;
}
