#pragma once
#include "stdAfx.h"
#include "./Interface/cEditBox.h"
#include "./Interface/ctabdialog.h"
#include "./Interface/cPushupButton.h"

class cStatic;
class cListDialog;
class cEditBox;
class cPushupButton;

class CCookDlg :
	public cTabDialog
{
public:
	CCookDlg(void);
	virtual ~CCookDlg(void);

	virtual void Add( cWindow* window ) ;

	void	Linking();
	virtual void SetActive( BOOL val );
	virtual void OnActionEvent(LONG lId, void * p, DWORD we);
	DWORD ActionEvent(CMouse * mouseInfo);

	void UpdateDlg();

	void ClearRecipe(WORD wLevel);
	void AddRecipe(WORD wLevel, DWORD dwIndex, char* pRecipeName);
	void DelRecipe(WORD wLevel, DWORD dwPos, bool bDeselect=FALSE);
	int GetRecipePos(WORD wLevel, DWORD dwRecipeIdx);
	void Send();

	WORD CheckCookUtil();
	bool CheckIngredients();

	void SetDlgLock(BOOL bVal);

protected:
	stRecipeInfo*	m_pSelectedRecipe;
	DWORD			m_dwSelectedRecipe;
	DWORD			m_dwSelectedFire;
	WORD			m_wMakeNum;

	cStatic*		m_pTitle;
	cStatic*		m_pLevel;
	cStatic*		m_pCookCount;
	cStatic*		m_pEatCount;
	cStatic*		m_pFireCount;

	cEditBox*		m_pMakeNum;
	cButton*		m_pBtnStart;
	cButton*		m_pBtnClose;
	cPushupButton*	m_pTabBtn[MAX_COOKING_LEVEL];

	std::list<DWORD> m_lstRecipeIndex[MAX_COOKING_LEVEL];
	cListDialog*	m_pRecipeList[MAX_COOKING_LEVEL];
	cListDialog*	m_pIngredientList[MAX_COOKING_LEVEL];
};
