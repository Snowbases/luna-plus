#ifndef BINSEARCHAE_H
#define BINSEARCHAE_H


#include "../4DyuchiGXGFunc/global.h"


//#define NODE_HEADER_SIZE	16


class CBinSearchAE
{	
	BSAENODE*					m_pRootNode;

	BSAENODE*					m_pNodePool;
	char*					m_pItemContainerPool;

	BSAENODE**					m_ppNodeTable;
	ITEM_CONTAINER_AE**		m_ppItemContainerTable;
	
	DWORD					m_dwReservedNodeNum;
	DWORD					m_dwReservedItemNum;

	DWORD					m_dwItemNum;
	DWORD					m_dwMaxItemNum;
	DWORD					m_dwItemContainerSize;

	BSAENODE*				AllocNode();
	void					FreeNode(BSAENODE* pNode);
	ITEM_CONTAINER_AE*		AllocItemContainer();
	void					FreeItemContaiener(ITEM_CONTAINER_AE* pContainer);
	BSAENODE*				SearchNode(DWORD dwKey);
	void					DeleteNode(BSAENODE* pDel);
	
public:
//	BOOL					IsExist(void* pItem);
	DWORD					GetItemNum() {return m_dwItemNum;}
	BOOL					Initialize(DWORD dwMaxItemNum);
	ITEM_CONTAINER_AE*		InsertItem(DWORD dwKey,void* pItem);
	BOOL					DeleteItem(ITEM_CONTAINER_AE*	pItemContainer);
	void*					SearchItem(DWORD dwKey);
	ITEM_CONTAINER_AE*		SearchItemContainer(DWORD dwKey);
	
	CBinSearchAE();
	~CBinSearchAE();
	
};


#endif

