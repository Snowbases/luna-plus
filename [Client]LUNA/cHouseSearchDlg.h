#pragma once

#include "stdAfx.h"
#include ".\interface\cWindowHeader.h"
#include "interface/cDialog.h"

struct stVisitInfo
{
	DWORD m_dwVisitKind;
	DWORD m_dwData1;
	DWORD m_dwData2;

	stVisitInfo(){ Clear(); }
	void Clear(){ memset(this, 0, sizeof(stVisitInfo)); }
};

struct stVisitName
{
	char name[MAX_NAME_LENGTH];

	stVisitName() {memset(this, 0, sizeof(stVisitName));}
};

//090409 pdy 하우징 하우스 검색 UI추가
class cHouseSearchDlg : public cDialog  
{
	cStatic * m_pSearchName;
	cEditBox * m_pSearchNameEdit;
	cButton * m_pSearchBtn;
	cButton * m_pSearchRandomBtn;
	cComboBox* m_pVisitList;

	stVisitInfo m_VisitInfo;
	std::list<stVisitName> m_VisitList;
	
public:
	cHouseSearchDlg();
	virtual ~cHouseSearchDlg();
	void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	void Linking();
	virtual void SetActive(BOOL val);
	void SetName(char* Name);
	char* GetName();
	void OnActionEvent(LONG lId, void* p, DWORD we);

	void SetVisitInfo(DWORD dwVisitKind , DWORD dwData1 ,DWORD dwData2)
	{
		m_VisitInfo.m_dwVisitKind = dwVisitKind ; 
		m_VisitInfo.m_dwData1		  = dwData1 ; 
		m_VisitInfo.m_dwData2		  = dwData2 ; 
	};

	void SetDisableState(BOOL val);

	stVisitInfo* GetVisitInfo() {return &m_VisitInfo;}
	void LoadVisitList();
	void AddVisitList();
};
