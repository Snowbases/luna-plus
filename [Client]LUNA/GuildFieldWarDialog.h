#pragma once


#include "INTERFACE\cDialog.h"

class cTextArea;
class cEditBox;
class cButton;
class cStatic;
class cListDialog;
class cCheckBox;

class CGFWarDeclareDlg : public cDialog  
{
protected:	
	cTextArea*	m_pMsg;
	cEditBox*	m_pEditBox;
	cTextArea*	m_pMsg1;
	cTextArea*	m_pMsg2;
	cTextArea*	m_pMsg3;
	cCheckBox*	m_pMoneyCheck;
	cEditBox*	m_pMoneyEdit;
	cStatic*	m_pMoneyStatic;
	cButton*	m_pOkBtn;
	cButton*	m_pCancelBtn;
	
	BOOL		m_bRender;
	cImage		m_LockImage;

	// 070209 LYW --- Add VECTOR2 for render part.
	VECTOR2 m_vPos;
	VECTOR2 m_vScale;

public:
	CGFWarDeclareDlg();
	virtual ~CGFWarDeclareDlg();

	virtual void	SetActive( BOOL val );
	void			Linking();
	void			ShowMoneyEdit();
	BOOL			IsChecked();
};

class CGFWarResultDlg : public cDialog  
{
private:	
	cStatic*	m_pTitle;
	cTextArea*	m_pMsg;
	cStatic*	m_pStatic0;
	cStatic*	m_pStatic1;
	cStatic*	m_pStatic2;
	cStatic*	m_pStatic3;
	cStatic*	m_pStatic4;
	cStatic*	m_pStatic5;

	cStatic*	m_myGuildName;
	cStatic*	m_myGuildLevel;
	cStatic*	m_myGuildMaster;
	cStatic*	m_myGuildMoney;

	cButton*	m_pOkBtn;
	cButton*	m_pCancelBtn;
	cButton*	m_pConfirmBtn;	

public:
	CGFWarResultDlg();
	virtual ~CGFWarResultDlg();

	void	Linking();
	//void	ShowDeclare( GUILDINFO* pInfo );
	void	ShowDeclare( const MSG_GUILD_LIST::Data& );
	void	ShowResult( DWORD dwKind );
};

class CGFWarInfoDlg : public cDialog  
{
private:	
	cStatic*	m_pTitle;
	cStatic*	m_pMsg;
	cListDialog*	m_pGuildListDlg;
	cTextArea*	m_pText;
	cStatic*	m_pStatic0;
	cStatic*	m_pStatic1;
	cStatic*	m_pStatic2;
	cButton*	m_pSuggestOkBtn;
	cButton*	m_pSurrendOkBtn;
	cButton*	m_pGuildUnionRemoveOkBtn;
	cButton*	m_pCancelBtn;

	int			m_nSelectedIdx;

public:
	CGFWarInfoDlg();
	virtual ~CGFWarInfoDlg();

	virtual DWORD	ActionEvent( CMouse* mouseInfo );
	virtual void	SetActive( BOOL );
	
	void	Linking();
	void	ShowSuggest();
	void	ShowSurrend();
	void	ShowGuildUnion();
	void	AddGuildInfoToList();
	void	AddGuildUnionInfoToList();
	int		GetSelectedListIdx()		{ return m_nSelectedIdx; }
};

class CGuildWarInfoDlg : public cDialog
{
protected:
	cListDialog*	m_pBlackGuildListDlg;
	cListDialog*	m_pWhiteGuildListDlg;
	cStatic*		m_pStatic;
	cButton*		m_pCloseBtn;
	
public:
	CGuildWarInfoDlg();
	virtual ~CGuildWarInfoDlg();

	virtual void	SetActive( BOOL val );
	
	void	Linking();
	void	AddListData();
};