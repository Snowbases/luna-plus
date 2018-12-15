#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif

#include "global.h"

#include "ItemIndexTable.h"


CItemIndexTable::CItemIndexTable()
{
	memset(this,0,sizeof(CItemIndexTable));
}

GLOBAL_FUNC_DLL ITEMTABLE_HANDLE __stdcall ITCreate()
{
	CItemIndexTable*	pITable = new CItemIndexTable;
	return pITable;
}
GLOBAL_FUNC_DLL void __stdcall	ITRelease(ITEMTABLE_HANDLE pIT)
{
	CItemIndexTable*	pITable = (CItemIndexTable*)pIT;
	delete pITable;
}

GLOBAL_FUNC_DLL BOOL __stdcall ITInitialize(ITEMTABLE_HANDLE pIT,DWORD dwMaxItemNum)
{
	CItemIndexTable*	pITable = (CItemIndexTable*)pIT;

	if (dwMaxItemNum)
	{


		pITable->m_dwMaxItemNum = dwMaxItemNum;
//		dwMaxItemNum++;	2003/07/12 주석처리 유영천
		
		pITable->m_pdwIndexList = new DWORD[dwMaxItemNum];
		memset(pITable->m_pdwIndexList,0,sizeof(DWORD)*dwMaxItemNum);
		
		pITable->m_pItemList = new ITEM_HANDLE[dwMaxItemNum];
		memset(pITable->m_pItemList,0,sizeof(ITEM_HANDLE)*dwMaxItemNum);

		pITable->m_pIC = ICCreate();
		ICInitialize(pITable->m_pIC,dwMaxItemNum);
	}

	return TRUE;
}
GLOBAL_FUNC_DLL DWORD __stdcall ITAddItem(ITEMTABLE_HANDLE pIT,void* pVoid)
{
	CItemIndexTable*	pITable = (CItemIndexTable*)pIT;

	DWORD	dwIndex = 0xffffffff;

	if (pITable->m_dwItemNum >= pITable->m_dwMaxItemNum)
		goto lb_return;
	
	dwIndex = ICAllocIndex(pITable->m_pIC);
	dwIndex--;

	if (dwIndex == 0xffffffff)
	{
		goto lb_return;
	}

	pITable->m_pItemList[dwIndex].pItem = pVoid;
	pITable->m_pItemList[dwIndex].dwLinearIndex = pITable->m_dwItemNum;
	pITable->m_pdwIndexList[pITable->m_dwItemNum] = dwIndex;

	pITable->m_dwItemNum++;
	
lb_return:
	return dwIndex;
}

GLOBAL_FUNC_DLL BOOL __stdcall ITDeleteItem(ITEMTABLE_HANDLE pIT,DWORD dwItemIndex)
{
	CItemIndexTable*	pITable = (CItemIndexTable*)pIT;

	BOOL	bResult = FALSE;

	if (dwItemIndex >= pITable->m_dwMaxItemNum)
	{
		char	txt[128];
		wsprintf(txt,"Fail to ITDeleteItem,%u is invalid dwItemIndex");
		OutputDebugString(txt);
		__asm int 3
		goto lb_return;

	}

	pITable->m_pItemList[dwItemIndex].pItem = NULL;
	
	pITable->m_pdwIndexList[pITable->m_pItemList[dwItemIndex].dwLinearIndex] = pITable->m_pdwIndexList[pITable->m_dwItemNum-1];
	pITable->m_pItemList[pITable->m_pdwIndexList[pITable->m_dwItemNum-1]].dwLinearIndex = pITable->m_pItemList[dwItemIndex].dwLinearIndex;
	
	pITable->m_pdwIndexList[pITable->m_dwItemNum-1] = 0;

	dwItemIndex++;
	ICFreeIndex(pITable->m_pIC,dwItemIndex);
	pITable->m_dwItemNum--;
	

	bResult = TRUE;
lb_return:
	return bResult;

}
GLOBAL_FUNC_DLL DWORD __stdcall ITGetItemNum(ITEMTABLE_HANDLE pIT)
{
	CItemIndexTable*	pITable = (CItemIndexTable*)pIT;
	return pITable->m_dwItemNum;
}
GLOBAL_FUNC_DLL void* __stdcall ITGetItem(ITEMTABLE_HANDLE pIT,DWORD dwItemIndex)
{
	void*	pItem = NULL;
	CItemIndexTable*	pITable = (CItemIndexTable*)pIT;
	if (dwItemIndex >= pITable->m_dwMaxItemNum)
	{
		goto lb_return;
	}
	pItem = pITable->m_pItemList[dwItemIndex].pItem;
lb_return:
	return pItem ;
	
}
GLOBAL_FUNC_DLL void* __stdcall ITGetItemSequential(ITEMTABLE_HANDLE pIT,DWORD dwSeqIndex)
{
	void*		pItem = NULL;
	CItemIndexTable*	pITable = (CItemIndexTable*)pIT;
	if (dwSeqIndex >= pITable->m_dwItemNum)
		goto lb_return;

	pItem = pITable->m_pItemList[pITable->m_pdwIndexList[dwSeqIndex]].pItem;

lb_return:
	return pItem;
}
CItemIndexTable::~CItemIndexTable()
{
	if (m_pIC)
	{
		ICRelease(m_pIC);
		m_pIC = NULL;
	}
	if (m_pdwIndexList)
	{
		delete [] m_pdwIndexList;
		m_pdwIndexList = NULL;
	}
	if (m_pItemList)
	{
		delete [] m_pItemList;
		m_pItemList = NULL;
	}
}