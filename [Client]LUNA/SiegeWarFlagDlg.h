#pragma once
// 080929 LYW --- SiegeWarFlagDlg : 공성 깃발 정보 다이얼로그 추가.
#include "stdafx.h"
#include "./Interface/cDialog.h"

#include "./Interface/cStatic.h"
//class cStatic ;

class CSiegeWarFlagDlg : public cDialog
{
	cStatic*	m_pFlagName ;
	cStatic*	m_pMasterName ;
	cStatic*	m_pGuildName ;

public:
	CSiegeWarFlagDlg(void);
	virtual ~CSiegeWarFlagDlg(void);

	void Linking() ;

	void SetFlagName(char* pFlagName) ;
	const char* GetFlagName() { return m_pFlagName->GetStaticText() ; }

	void SetMasterName(char* pMasterName) ;
	const char* GetMasterName() { return m_pMasterName->GetStaticText() ; }

	void SetGuildName(char* pGuildName) ;
	const char* GetGuildName() { return m_pGuildName->GetStaticText() ; }
};
