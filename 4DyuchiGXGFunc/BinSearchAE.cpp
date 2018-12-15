#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif

#include "../4DyuchiGXGFunc/global.h"
#include "BinSearchAE.h"

	
GLOBAL_FUNC_DLL BSAE_HANDLE __stdcall	BSAECreate()
{
	CBinSearchAE* pBinSearch = new CBinSearchAE;
	return (BSAE_HANDLE)pBinSearch;
	
}
GLOBAL_FUNC_DLL DWORD __stdcall BSAEGetItemNum(BSAE_HANDLE pBSAE)
{
	CBinSearchAE* pBinSearch = (CBinSearchAE*)pBSAE;
	return pBinSearch->GetItemNum();
}
GLOBAL_FUNC_DLL BOOL __stdcall BSAEInitialize(BSAE_HANDLE pBSAE,DWORD dwMaxItemNum)
{
	CBinSearchAE* pBinSearch = (CBinSearchAE*)pBSAE;
	return pBinSearch->Initialize(dwMaxItemNum);

}
GLOBAL_FUNC_DLL ITEM_CONTAINER_AE* __stdcall BSAEInsertItem(BSAE_HANDLE pBSAE,DWORD dwKey,void* pItem)
{
	CBinSearchAE* pBinSearch = (CBinSearchAE*)pBSAE;
	return pBinSearch->InsertItem(dwKey,pItem);
}
GLOBAL_FUNC_DLL BOOL __stdcall BSAEDeleteItem(BSAE_HANDLE pBSAE,ITEM_CONTAINER_AE* pItemContainer)
{
	CBinSearchAE* pBinSearch = (CBinSearchAE*)pBSAE;
	return pBinSearch->DeleteItem(pItemContainer);

}
GLOBAL_FUNC_DLL void* __stdcall BSAESearchItem(BSAE_HANDLE pBSAE,DWORD dwKey)
{
	CBinSearchAE* pBinSearch = (CBinSearchAE*)pBSAE;
	return pBinSearch->SearchItem(dwKey);
}
GLOBAL_FUNC_DLL ITEM_CONTAINER_AE* __stdcall BSAESearchItemContainer(BSAE_HANDLE pBSAE,DWORD dwKey)
{
	CBinSearchAE* pBinSearch = (CBinSearchAE*)pBSAE;
	return pBinSearch->SearchItemContainer(dwKey);
}

GLOBAL_FUNC_DLL void __stdcall BSAERelease(BSAE_HANDLE pBSAE)
{
	delete (CBinSearchAE*)pBSAE;
}



CBinSearchAE::CBinSearchAE()
{
	memset(this,0,sizeof(CBinSearchAE));

}



BOOL CBinSearchAE::Initialize(DWORD dwMaxItemNum)
{

	m_dwMaxItemNum = dwMaxItemNum;
	m_dwItemContainerSize = sizeof(ITEM_CONTAINER_AE) - sizeof(DWORD) + sizeof(DWORD);

	m_dwMaxItemNum = dwMaxItemNum;
	
	m_pNodePool = new BSAENODE[m_dwMaxItemNum];
	memset(m_pNodePool,0,sizeof(BSAENODE)*m_dwMaxItemNum);

	m_pItemContainerPool = new char[m_dwItemContainerSize*m_dwMaxItemNum];
	memset(m_pItemContainerPool,0,m_dwItemContainerSize*m_dwMaxItemNum);
	
	m_ppNodeTable = new BSAENODE*[m_dwMaxItemNum];
	memset(m_ppNodeTable,0,sizeof(BSAENODE*)*m_dwMaxItemNum);

	m_ppItemContainerTable = new ITEM_CONTAINER_AE*[m_dwMaxItemNum];
	memset(m_ppItemContainerTable,0,sizeof(ITEM_CONTAINER_AE*)*m_dwMaxItemNum);

	for (DWORD i=0; i<m_dwMaxItemNum; i++)
	{
		m_ppNodeTable[i] = m_pNodePool+i;
		m_ppItemContainerTable[i] = (ITEM_CONTAINER_AE*)(m_pItemContainerPool + m_dwItemContainerSize*i);
	}
	m_dwReservedNodeNum = m_dwMaxItemNum;
	m_dwReservedItemNum = m_dwMaxItemNum;

	return TRUE;
}
BSAENODE* CBinSearchAE::AllocNode()
{
	BSAENODE*	pNode;

	pNode = m_ppNodeTable[0];
	m_ppNodeTable[0] = m_ppNodeTable[m_dwReservedNodeNum-1];
	m_dwReservedNodeNum--;
	
	pNode->dwRefCount = 0;
	pNode->pParent = NULL;
	pNode->pLeft = NULL;
	pNode->pRight = NULL;
	pNode->dwRefCount = 0;
	pNode->pItemContainer = NULL;
	
	return pNode;

}
void CBinSearchAE::FreeNode(BSAENODE* pNode)
{

	m_ppNodeTable[m_dwReservedNodeNum] = pNode;
	pNode->dwKey = 0xcccccccc;
	m_dwReservedNodeNum++;
}
ITEM_CONTAINER_AE* CBinSearchAE::AllocItemContainer()
{

	ITEM_CONTAINER_AE*	pContainer;

	pContainer = m_ppItemContainerTable[0];
	m_ppItemContainerTable[0] = m_ppItemContainerTable[m_dwReservedItemNum-1];
	m_dwReservedItemNum--;

	pContainer->pOwner = NULL;
	pContainer->pPrv = NULL;
	pContainer->pNext = NULL;
	pContainer->pItem = NULL;
	

	return pContainer;

}
void CBinSearchAE::FreeItemContaiener(ITEM_CONTAINER_AE* pContainer)
{
	m_ppItemContainerTable[m_dwReservedItemNum] = pContainer;
	pContainer->pItem = NULL;
	pContainer->pOwner = NULL;
	m_dwReservedItemNum++;
}


ITEM_CONTAINER_AE*	CBinSearchAE::InsertItem(DWORD dwKey,void* pItem)
{

	
	BSAENODE*			pParent;
	BSAENODE*			pCur = NULL;
	ITEM_CONTAINER_AE*	pNewContainer = NULL;
	
//	IsExist(pItem);

	if (!m_pRootNode)
	{
		m_pRootNode = pCur = AllocNode();
		goto lb_true;
	}
	pParent = m_pRootNode;

lb_check_left:

	if (dwKey == pParent->dwKey)
	{
		pCur = pParent;

		goto lb_true;
	}
	if (dwKey < pParent->dwKey)
	{
		if (!pParent->pLeft)
		{
			pCur = AllocNode();
			pParent->pLeft = pCur;
			pCur->pParent = pParent;
			goto lb_true;
		}
		else
		{
			pParent = pParent->pLeft;
			goto lb_check_left;
		}
	}
lb_check_right:
	if (dwKey > pParent->dwKey)
	{
		if (!pParent->pRight)
		{
			pCur = AllocNode();
			pParent->pRight = pCur;
			pCur->pParent = pParent;
		}
		else
		{
			pParent = pParent->pRight;
			goto lb_check_left;
		}
	}
	
lb_true:


	pCur->dwKey = dwKey;
	pNewContainer = AllocItemContainer();
	if (!pNewContainer)
		goto lb_false;

	pNewContainer->dwKey = dwKey;
	
	pNewContainer->pItem = pItem;	// 아이템
	pNewContainer->pOwner = pCur;	// 이 아이템을 소유하고 있는 노드 포인터.

	if (pCur->pItemContainer)
	{
		pNewContainer->pNext = pCur->pItemContainer;
		pCur->pItemContainer->pPrv = pNewContainer;
		pCur->pItemContainer = pNewContainer;
	}
	else
	{
		pCur->pItemContainer = pNewContainer;
	}
	pCur->dwRefCount++;

		
	m_dwItemNum++;
	
	
lb_false:
	if (!pNewContainer)
		__asm int 3
	return pNewContainer;
}


void* CBinSearchAE::SearchItem(DWORD dwKey)
{
	void*	pItem = NULL;
	
	BSAENODE*	pNode = SearchNode(dwKey);
	if (!pNode)
		goto lb_return;

	pItem = pNode->pItemContainer->pItem;

	/*__asm
	{
		mov		esi,dword ptr[pItem]
		mov		eax,dword ptr[esi];
		mov		dword ptr[dwSize],eax
	}
	if (dwSize != pNode->pItemContainer->dwKey)
		__asm int 3*/
lb_return:
	return pItem;

}
ITEM_CONTAINER_AE*	CBinSearchAE::SearchItemContainer(DWORD dwKey)
{
	ITEM_CONTAINER_AE*	pItem = NULL;
	
	BSAENODE*	pNode = SearchNode(dwKey);
	if (!pNode)
		goto lb_return;

	pItem = pNode->pItemContainer;

lb_return:
	return pItem;
}
BSAENODE* CBinSearchAE::SearchNode(DWORD dwKey)
{
	
	BSAENODE*	pParent = m_pRootNode;
	BSAENODE*	pResult = NULL;
	

	// 같거나 크면 ok
lb_search_left:
	
	if (!pParent)
		goto lb_return;

	

	if (pParent->dwKey == dwKey)
	{
		
		pResult = pParent;
		goto lb_return;
	}
	if (dwKey < pParent->dwKey)
	{
	
		pResult = pParent;
		pParent = pParent->pLeft;
		goto lb_search_left;
	}

	if (dwKey > pParent->dwKey)
	{
		// 이전에 왼쪽으로 진행했었고 현재 비교값보다 값이 클 경우는 
		// 이전의 노드를 얻어야한다.
		
		pParent = pParent->pRight;
		goto lb_search_left;
	}
	
lb_return:
	return pResult;
}

BOOL CBinSearchAE::DeleteItem(ITEM_CONTAINER_AE*	pItemContainer)
{
#ifdef _DEBUG
	if (!m_dwItemNum)
		__asm int 3
#endif

	void*	pItem = pItemContainer->pItem;

	BOOL	bResult = FALSE;
	BSAENODE*	pNode = pItemContainer->pOwner;
	
	ITEM_CONTAINER_AE*		pCur = pItemContainer;

lb_find:
	ITEM_CONTAINER_AE*		pNext;
	ITEM_CONTAINER_AE*		pPrv;
	pNext = pCur->pNext;
	pPrv = pCur->pPrv;

	if (pPrv)
		pPrv->pNext = pNext;
	else
	{
		// 이게 맨 처음이다.
		pNode->pItemContainer = pNext;
	}

	if (pNext)
		pNext->pPrv = pPrv;
	

	FreeItemContaiener(pCur);
//	if (!pNode->dwRefCount)
//		__asm int 3
		
	pNode->dwRefCount--;
	if(!pNode->dwRefCount)
	{
		DeleteNode(pNode);
		
	}
	m_dwItemNum--;
	bResult = TRUE;
//	IsExist(pItem);
	
lb_return:
	return bResult;
}

void CBinSearchAE::DeleteNode(BSAENODE* pDel)
{

	if (!m_pRootNode)
		return;

	
	BSAENODE*		pTemp;
	BSAENODE*		pNewChild;
	BSAENODE*		pNewParent;
	BSAENODE*		pParent;

	pParent = pDel->pParent;
	DWORD	dwKey = pDel->dwKey;

	if (!pDel->pRight)
	{
		// 삭제할 노드의 오른쪽 자식이 없는 경우.
		// pDel->pLeft로 pDel의 위치를 대치한다.
		pNewChild = pNewParent = pDel->pLeft; 
		if (pNewChild)
			pNewChild->pParent = pParent;	


		goto lb_return;
	}

	if (!pDel->pRight->pLeft)
	{
		// 삭제할 노드의 오른쪽 자식의 왼쪽 자식이 없는 경우
		pNewChild = pNewParent = pDel->pRight;
		pNewParent->pLeft = pDel->pLeft;	// 새로운 부모노드의 왼쪽 자식 노드
	
		if (pNewParent->pLeft)
			pNewParent->pLeft->pParent = pNewParent;	

		pNewChild->pParent = pParent;

		goto lb_return;
	}

	// 그 외..
	pTemp = pDel->pRight;

lb_seek_left_end:
	if (pTemp->pLeft)
	{
		pTemp = pTemp->pLeft;
		goto lb_seek_left_end;
	}
	pNewParent = pTemp;					// 삭제할 노드의 왼쪽 자식에 대해 새로운 부모가 될 노드.
	
	pNewChild = pDel->pRight;						// 새로운 자식 노드가 될 노드 
	pNewChild->pParent = pParent;
	
	pNewParent->pLeft = pDel->pLeft;	// 새로운 부모 노드의 왼쪽 자식 노드 세팅
	
	if (pNewParent->pLeft)
		pNewParent->pLeft->pParent = pNewParent;	// 삭제할 노드의 왼쪽 자식의 새로운 부모노드를 세팅 
	

lb_return:
	if (pParent)
	{
		if (dwKey < pParent->dwKey)
			pParent->pLeft = pNewChild;
		else
			pParent->pRight = pNewChild;
	}

	if (pDel == m_pRootNode)
		m_pRootNode = pNewChild;

	FreeNode(pDel);
}/*
BOOL CBinSearchAE::IsExist(void* pItem)
{
	BOOL	bResult = FALSE;
	
	
	for (DWORD i=0; i<m_dwMaxItemNum; i++)
	{
		if (m_ppItemContainerTable[i]->pItem == pItem)
		{
			__asm int 3
			bResult = TRUE;
			goto lb_return;
		}

	}
	
lb_return:
	return bResult;
}*/
CBinSearchAE::~CBinSearchAE()
{
	if (m_pNodePool)
	{
		delete [] m_pNodePool;
		m_pNodePool = NULL;
	}
	if (m_pItemContainerPool)
	{
		delete [] m_pItemContainerPool;
		m_pItemContainerPool = NULL;
	}
	if (m_ppNodeTable)
	{
		delete [] m_ppNodeTable;
		m_ppNodeTable = NULL;
	}
	if (m_ppItemContainerTable)
	{
		delete [] m_ppItemContainerTable;
		m_ppItemContainerTable = NULL;
	}
}
