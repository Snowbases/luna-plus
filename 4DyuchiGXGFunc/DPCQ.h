// DPCQ.h: interface for the CDPCQ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DPCQ_H__1A684EBE_B012_4EDC_AC0E_FDF03B601A5C__INCLUDED_)
#define AFX_DPCQ_H__1A684EBE_B012_4EDC_AC0E_FDF03B601A5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// __stdcall Àü¿ë
#include "../4DyuchiGRX_Common/stdafx.h"

struct DPC_ITEM
{
	WORD				wSize;				// 0
	WORD				wArgNum;			// 2
	void*				pStdcallFunc;		// 4
	int*				pRetAddr;			// 8
	DWORD				pArg[1];			// 12
};
#define					W_SIZE_OFFSET		0
#define					W_ARG_NUM_OFFSET	2
#define					FUNCPTR_OFFSET		4
#define					RETADDR_OFFSET		8
#define					ARG_LIST_OFFSET		12

#define					DEFAULT_DPC_ITEM_SIZE	32
class CDPCQ  
{
	SORT_ITEM_INT*		m_pItemPtrList;
	DWORD				m_dwMaxItemNum;
	DWORD				m_dwMaxBufferSize;
	char*				m_pBuffer;
	DWORD				m_dwCurrentOffset;
	DWORD				m_dwItemNum;

public:
	BOOL				Initialize(DWORD dwMaxBufferSize);
	BOOL				PushDPC(void* pStdcallFunc,WORD wArgNum,DWORD* pArgList,int* pRetAddr,int iOrder);
	void				Process();
	void				ProcessWithAscSort();
	void				Clear();

	CDPCQ();
	~CDPCQ();

};

#endif // !defined(AFX_DPCQ_H__1A684EBE_B012_4EDC_AC0E_FDF03B601A5C__INCLUDED_)
