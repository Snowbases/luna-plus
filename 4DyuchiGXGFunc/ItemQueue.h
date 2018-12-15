#ifndef ITEM_QUEUE_H
#define ITEM_QUEUE_H

#include "../4DyuchiGRX_common/typedef.h"


struct ITEM_INDEX_BUFFER
{
	DWORD		dwTypeID;
	DWORD		dwIndexNum;
	DWORD		dwMaxIndexNum;
	DWORD		dwItemSize;
	void*		pFunc;
	DWORD*		pIndexBuffer;

};
struct QUEUE_ITEM
{
	DWORD		dwTypeID;
	char		pItem[1];
};

class CItemQueue
{
public:
	DWORD				m_dwMaxTypeNum;
	ITEM_INDEX_BUFFER*	m_pIndexBufferPerItem;
	char*				m_pItemBuffer;
	DWORD				m_dwMaxBufferSize;
	
	DWORD				m_dwCurrentWriteOffset;
	DWORD				m_dwCurrentReadOffset;
	DWORD				m_dwWritedItemNum;
	DWORD				m_dwReadedItemNum;
	CItemQueue();
	~CItemQueue();
};

#endif
