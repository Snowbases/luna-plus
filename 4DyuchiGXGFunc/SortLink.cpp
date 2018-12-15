#ifndef GLOBAL_FUNC_DLL
#define GLOBAL_FUNC_DLL extern "C" __declspec(dllexport)
#endif


#include "global.h"

GLOBAL_FUNC_DLL void __stdcall LinkToSortLink(SORT_LINK** ppHead,SORT_LINK** ppTail,SORT_LINK* pNew)
{
	if (!*ppHead)
	{
		*ppTail = *ppHead = pNew;
		(*ppHead)->pNext = NULL;
		(*ppHead)->pPrv = NULL;

	}
	else
	{
		pNew->pNext = (*ppHead);
		(*ppHead)->pPrv = pNew;
		(*ppHead) = pNew;
		pNew->pPrv		=	NULL;				// 이진영 추가 2004/03/39.
	}
}

GLOBAL_FUNC_DLL void __stdcall LinkToSortLinkFIFO(SORT_LINK** ppHead,SORT_LINK** ppTail,SORT_LINK* pNew)
{
	if (!*ppHead)
	{
		*ppTail = *ppHead = pNew;
		(*ppHead)->pNext = NULL;
		(*ppHead)->pPrv = NULL;

	}
	else
	{
		pNew->pPrv = (*ppTail);
		(*ppTail)->pNext = pNew;
		(*ppTail) = pNew;
		pNew->pNext = NULL;

	//	pNew->pNext = (*ppHead);
	//	(*ppHead)->pPrv = pNew;
	//	(*ppHead) = pNew;
	//	pNew->pPrv		=	NULL;				// 이진영 추가 2004/03/39.
	}
}
GLOBAL_FUNC_DLL void __stdcall UnLinkFromSortLink(SORT_LINK** ppHead,SORT_LINK** ppTail,SORT_LINK* pDel)
{
	SORT_LINK*	pPrv = pDel->pPrv;
	SORT_LINK*	pNext = pDel->pNext;

#ifdef	_DEBUG
	if( pDel->pPrv)
	{
		if( pDel->pPrv->pNext != pDel)
			_asm int 3;
	}
#endif

	if (pDel->pPrv)
		pDel->pPrv->pNext = pDel->pNext;
	else
	{
#ifdef _DEBUG
		if (pDel != (*ppHead))
			__asm int 3
#endif
		(*ppHead) = pNext;
	}

	if (pDel->pNext)
		pDel->pNext->pPrv = pDel->pPrv;
	else
	{
#ifdef _DEBUG
		if (pDel != (*ppTail))
			__asm int 3
#endif
		(*ppTail) = pPrv;
	}
	pDel->pPrv = NULL;
	pDel->pNext = NULL;

}