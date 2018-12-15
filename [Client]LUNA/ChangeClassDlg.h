// 100511 ONS 전직초기화 다이얼로그 추가
//
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "interface/cDialog.h"

class cStatic;
class cComboBox;

class CChangeClassDlg : public cDialog
{
private:
	cStatic*			m_pClassKindStatic[MAX_CLASS_LEVEL];
	cComboBox*			m_pClassKindCombo[MAX_CLASS_LEVEL - 1];
	cStatic*			m_pFirstClassKind;
	
public:
	CChangeClassDlg();
	virtual ~CChangeClassDlg();

	virtual void OnActionEvent(LONG lId, void* p, DWORD we);
	virtual void SetActive( BOOL val );
	
	void Linking();
	void SettingClassList();
	BOOL CheckExclusiveClass( WORD Index , BYTE Race );

	int GetCurSelectedIdx( WORD wStage );
	ITEM* GetItem( WORD wStage, int nIndex );

};