// ObjectStateManager.h: interface for the CObjectStateManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTSTATEMANAGER_H__3E274A5D_DF34_4010_A57C_6310F5041EEF__INCLUDED_)
#define AFX_OBJECTSTATEMANAGER_H__3E274A5D_DF34_4010_A57C_6310F5041EEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define OBJECTSTATEMGR_OBJ CObjectStateManager::GetInstance()

class CObject;

class CObjectStateManager  
{
	//STATE_INFO m_StateInfo;//taiyo
public:
	GETINSTANCE(CObjectStateManager);
	CObjectStateManager();
	virtual ~CObjectStateManager();
	
	void InitObjectState(CObject* pObject);
	BOOL StartObjectState(CObject* pObject, EObjectState, DWORD dwParam);
	void EndObjectState(CObject* pObject, EObjectState, DWORD EndStateCount = 0);
	EObjectState GetObjectState(CObject*) const;
	void StateProcess(CObject*);
};

#endif // !defined(AFX_OBJECTSTATEMANAGER_H__3E274A5D_DF34_4010_A57C_6310F5041EEF__INCLUDED_)
