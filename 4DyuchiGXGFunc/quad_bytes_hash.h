#pragma once

#include "type.h"
#include "look_aside_list.h"

struct QB_BUCKET
{
	DWORD					m_dwKey;
	DWORD					m_dwItem;
	QB_BUCKET*				m_pPrv;
	QB_BUCKET*				m_pNext;
	
};
#define		QB_BUCKET_SIZE	16

class CQuadBytesHashTable
{
public:
	DWORD					m_dwRefCount;
	QB_BUCKET*				m_pBucketTable;		// 4
	DWORD					m_dwMaxBucketNum;	// 8
	STMPOOL_HANDLE			m_pStaticMemoryPool;	// 12
	CQuadBytesHashTable();
};

