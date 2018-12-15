#pragma once

#include "./interface/cDialog.h"

#define MB_MAX_BTN_COUNT	2												// 메시지 박스에서 사용가능한 최대 버튼 수.
#define MAX_MB_MSG			128												// 메시지 박스의 최대 길이.

class cStatic ;
class cTextArea ;

enum eMBType
{
	MBT_NOBTN = 0,
	MBT_OK,
	MBT_YESNO,
	MBT_CANCEL,
	MBT_OK_EXIT_PROGRAM,
	MBT_YESNO_EXIT_PROGRAM,
	MBT_CANCEL_EXIT_PROGRAM,
};

enum eMB_BTN_ID
{
	MB_BTN_OK = 0,
	MB_BTN_YES,
	MB_BTN_NO,
	MB_BTN_CANCEL,
	MB_BTN_COUNT,
};

class cMsgBox : public cDialog
{
protected :
	static cImage	m_DlgImg ;
	static cImage	m_BtnImgBasic ;
	static cImage	m_BtnImgOver ;
	static cImage	m_BtnImgPress ;
	static char		m_BtnText[MB_BTN_COUNT][32] ;

	static DWORD	m_dwColor[3] ;									//basic, over, press
	static BOOL		m_bInit ;
	cStatic* mCounterStatic;

protected:
	cTextArea* m_pMsg;
	DWORD m_dwMsgColor;
	cButton* m_pButton[2];
	void (*m_cbMBFunc)(LONG lId, LPVOID, DWORD we);	
	int	m_nDefaultBtn;
	int	m_nMBType;
	DWORD m_dwParam;
	DWORD m_dwVisibleTime;

public:
	cMsgBox();
	virtual ~cMsgBox();
	virtual void Render();
	void SetVisibleTime(DWORD dwVisibleTime);

	virtual DWORD ActionEvent( CMouse * mouseInfo ) ;
	virtual DWORD ActionKeyboardEvent( CKeyboard * keyInfo ) ;

	void SetMBFunc( cbFUNC cbMBFunc ) { m_cbMBFunc = cbMBFunc; }

	static void InitMsgBox() ;
	static BOOL IsInit() { return m_bInit; }
	void MsgBox(LONG lId, eMBType, LPCTSTR strMsg, cbFUNC cbMBFunc = NULL);	
	void SetDefaultBtn( int nDefaultBtn ) { m_nDefaultBtn = nDefaultBtn; }

	BOOL PressDefaultBtn() ;
	BOOL ForcePressButton( LONG lBtnID ) ;
	void ForceClose() ;

	DWORD GetParam() { return m_dwParam; }
	void SetParam( DWORD dwParam )  { m_dwParam = dwParam; }

private:
	void PutCounter();
};