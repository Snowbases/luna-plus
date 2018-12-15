#pragma once

#include "type.h"
#include "../4DyuchiGRX_Common/stdafx.h"

struct MEMORY_DESC
{
	void*				m_pAddr;
	MEMORY_DESC*		m_pNext;

};
#define		MEMORY_DESC_SIZE	8

struct MEMORY_BLOCK
{
	MEMORY_DESC*		m_pDesc;
	char				m_pMemory[1];
};
#define		MEMORY_BLOCK_HEADER_SIZE	4


class CLookAsideList
{
public:
	MEMORY_DESC*		m_pBaseDesc;
	DWORD				m_dwCommitedBlockNum;	
	DWORD				m_dwDefaultCommitBlockNum;
	DWORD				m_dwUnitSize;
	DWORD				m_dwMaxBlockNum;
	DWORD				m_dwAllocatedNum;
		
	

	void*				m_pLinearMemoryPool;
	void*				m_pLinearDescPool;
	BOOL				m_bBugCheck;
	
public:
	CLookAsideList();
};

