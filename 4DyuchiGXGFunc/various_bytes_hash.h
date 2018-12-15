#pragma once

#include "type.h"
#include "look_aside_list.h"

struct VB_BUCKET
{
	DWORD				m_dwItem;
	VB_BUCKET*			m_pPrv;
	VB_BUCKET*			m_pNext;
	DWORD				m_dwSize;
};
#define VB_BUCKET_DEFAULT_SIZE		16



class CVariousBytesHashTable
{
public:
	DWORD				m_dwRefCount;
	VB_BUCKET*			m_pBucketTable;
	DWORD				m_dwMaxBucketNum;
	DWORD				m_dwMaxKeyDataSize;
	STMPOOL_HANDLE		m_pStaticMemoryPool;
	CVariousBytesHashTable();
};


