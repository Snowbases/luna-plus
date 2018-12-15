#pragma once


#include "./Interface/cDialog.h"
#include "./Interface/cImage.h"

class cIconGridDialog;
class cEditBox;
class cPushupButton;
class cButton;
class cStatic;

class CExchangeDialog  : public cDialog
{
protected:

//for linking
	BOOL				m_bInit;
	cIconGridDialog*	m_pMyIconGridDlg;
	cIconGridDialog*	m_pOpIconGridDlg;
	cEditBox*			m_pMyMoneyEditBox;
	cEditBox*			m_pOpMoneyEditBox;
	cPushupButton*		m_pLockBtn;
	cPushupButton*		m_pExchangeBtn;
	cButton*			m_pInputMoneyBtn;

	cStatic*			m_pMyNameStc;
	cStatic*			m_pOpNameStc;

	cImage				m_LockImage;
	
	BOOL				m_bMyLock;
	BOOL				m_bOpLock;
	
	DWORD				m_dwMoney;
	DWORD				m_dwOpMoney;
	
	BOOL				m_bExchangable;

	int					m_nMyItemNum;		//교환할 내 아이템 수
	int					m_nOpItemNum;		//교환할 상대 아이템 수

public:
	CExchangeDialog();
	virtual ~CExchangeDialog();
	
	void Linking();
	virtual void SetActive( BOOL val );
//	virtual DWORD ActionEvent( CMouse* mouseInfo, CKeyboard* keyInfo );
	virtual void Render();
	void OnActionEvent(LONG lId, void * p, DWORD we);

	virtual BOOL FakeMoveIcon( LONG x, LONG y, cIcon* icon );
	void AddItem( int nType, POSTYPE pos, cIcon* pItem );
	void DelItem( int nType, POSTYPE pos );
	void DelItemOptionInfo( POSTYPE pos );
	void DelItemOptionInfoAll();
	//void DelItemRareOptionInfo( POSTYPE pos );
	//void DelItemRareOptionInfoAll();

	DWORD InputMoney( int nType, DWORD dwMoney );

	void OpLock( BOOL bLock );	//상대 락걸기
	void MyLock( BOOL bLock );

	DWORD GetMoney() { return m_dwMoney; }
	BOOL IsMyLock()	{ return m_bMyLock; }
	BOOL IsOpLock() { return m_bOpLock; }
	
	void SetPlayerName( char* pMyName, char* pOpName );

	void MoneyInputButtonEnable( BOOL bEnable );

	static void OnInputMoney( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 );
	static void OnInputCancel( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 );
};