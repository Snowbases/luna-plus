// TempImageMapManager.h: interface for the CTempImageMapManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMPIMAGEMAPMANAGER_H__71695307_6E80_4BF2_A2C3_0575738E475D__INCLUDED_)
#define AFX_TEMPIMAGEMAPMANAGER_H__71695307_6E80_4BF2_A2C3_0575738E475D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../4DyuchiGXGFunc/global.h"


struct STATIC_RESOURCE_ITEM
{
	BOOL*						pbEnable;				
	SORT_LINK					sortLink;
	void*						pVoid;
};
class CStaticResourcePool
{
	SORT_LINK*					m_pSortLinkHead;
	SORT_LINK*					m_pSortLinkTail;
	
	STMPOOL_HANDLE				m_pItemPool;
	RESOURCR_ALLOC_FUNC			m_pAllocFunc;
	RESOURCR_FREE_FUNC			m_pFreeFunc;

	void						ClearMember();
	void						Cleanup();	
public:
	
	BOOL						Initialize(DWORD dwMaxItemNum,RESOURCR_ALLOC_FUNC pAllocFunc,RESOURCR_FREE_FUNC pFreeFunc);
	void*						Alloc(BOOL* pbEnable,DWORD* pdwResourceHandle,void* pArg1,void* pArg2,void* pArg3,DWORD dwSize);
	void						Free(DWORD dwResoruceHandle);
	void						FreeAll();

	CStaticResourcePool();
	~CStaticResourcePool();

};

#endif // !defined(AFX_TEMPIMAGEMAPMANAGER_H__71695307_6E80_4BF2_A2C3_0575738E475D__INCLUDED_)
