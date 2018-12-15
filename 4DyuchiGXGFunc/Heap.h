#pragma once

#include "../4DyuchiGXGFunc/global.h"
#include "BinSearchAE.h"

enum MEMORY_STATUS
{
	MEMORY_STATUS_ALLOCATED = 0x01,
	MEMORY_STATUS_AVAILABLE = 0x02,
	MEMORY_STATUS_TERMINATOR = 0x04,
	MEMORY_STATUS_DESTROY	= 0x08
};
struct MEMORY_BLOCK_DESC;


#pragma pack(push,1)
struct MEMORY_STATUS_DESC_LINK
{
	char*						pMemAddr;
	MEMORY_BLOCK_DESC*			pMemDesc;
	DWORD						dwSize;

	BYTE						bStatus;
	BYTE						bReserved1;
	BYTE						bReserved2;
	BYTE						bReserved3;
	
	MEMORY_STATUS_DESC_LINK*	pPrv;
	MEMORY_STATUS_DESC_LINK*	pNext;
};
#pragma pack(pop)

struct MEMORY_BLOCK_DESC
{
	DWORD						dwSize;
	char*						pMemory;
	ITEM_CONTAINER_AE*			pSearchHandle;
	BYTE						bStatus;
	BYTE						bReserved1;
	BYTE						bReserved2;
	BYTE						bReserved3;
	MEMORY_STATUS_DESC_LINK*	pStatusDesc;
};

struct ALIGN_SET
{
	MEMORY_STATUS_DESC_LINK*	pHeadTerminator;
	MEMORY_STATUS_DESC_LINK*	pTailTerminator;
	ALIGN_SET*					pNext;
};

class CHeap
{
	ALIGN_SET*					m_pAlignSet;
	DWORD						m_dwAlignSetNum;
//	MEMORY_STATUS_DESC_LINK*	m_pStatusLink;
//	MEMORY_STATUS_DESC_LINK*	m_pHeadTerminator;
//	MEMORY_STATUS_DESC_LINK*	m_pTailTerminator;

	CBinSearchAE		m_blockSearch;
	DWORD				m_dwAllocUnitSize;
	DWORD				m_dwAlignBytes;
	STMPOOL_HANDLE		m_pDescPool;
	STMPOOL_HANDLE		m_pStatusDescLinkPool;
	char*				m_pMassMemory;
	DWORD				m_dwMemorySize;
	
	DWORD				m_dwAllocatedNum;
	DWORD				m_dwAlloctedMemorySize;


	

	DWORD				m_dwMaxBlockNum;
	BOOL				m_bEnableDebug;

	MEMORY_BLOCK_DESC*	AddNewMemBlock(char* pMem,DWORD dwSize);
	void				FreeMemoryStatusDesc(MEMORY_STATUS_DESC_LINK* pDesc);
	MEMORY_BLOCK_DESC*	GetMemortyBlockStatudDesc(void* pMemAddr);
	BOOL				CheckAlign(char* pMem,DWORD dwSize);




public:
	void		LeakCheck();
	void		HeapCheck();
	BOOL		Initialize(void* pMassMemory,DWORD dwMassMemorySize,DWORD dwMaxBlockNum,DWORD dwUnitSize,DWORD dwAlignUnitNum,BOOL bEnableDebug);

	void*		Alloc(DWORD* pdwErrorCode,DWORD dwSize,BOOL bAlign);
	void*		AllocUnit(DWORD* pdwErrorCode,DWORD dwUnitNum,DWORD* pdwSize,BOOL bAlign);
	void		Free(void* pMem);


	void		Release();
	
	CHeap();
	~CHeap();

};