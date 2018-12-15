#pragma once
#include ".\interface\cdialog.h"

#define MAX_TEXTLEN			58

#ifdef _TL_LOCAL_
#define CHATLIST_TEXTEXTENT	348
#endif

enum CHATTINGDLG_TYPE
{
	e_CHATTINGDLG_CHAT,
	e_CHATTINGDLG_ASSIST,
	e_CHATTINGDLG_GENERAL_SHOUT,
	e_CHATTINGDLG_MAX
};

enum CONVERSATION
{
	e_CONVERSATION_ALL, 
	e_CONVERSATION_PARTY, 
	e_CONVERSATION_FAMILY, 
	e_CONVERSATION_GUILD, 
	e_CONVERSATION_ALLIANCE, 
	e_CONVERSATION_TRADE, 
	e_CONVERSATION_SYSTEM, 
	e_CONVERSATION_ASSISTANCE,
	e_CONVERSATION_MAX,
} ;

enum SEND_MSG_TYPE
{
	e_SMT_ALL, 
	e_SMT_PARTY, 
	e_SMT_FAMILY, 
	e_SMT_GUILD, 
	e_SMT_ALLIANCE, 
	e_SMT_TRADE, 
	e_SMT_WHOLE, 
	e_SMT_WHISPER, 
	e_SMT_WHISPER2, // /id MSG.
	e_SMT_MAX,
} ;

class cButton ;
class cEditBox ;
class cListDialog ;
class cPushupButton ;
class cStatic ;

class CChattingDlg : public cDialog
{
	cListDialog*	m_pChatList ;
	cListDialog*	m_pAssistList ;
	cListDialog*	m_pShoutList;

	cEditBox*		m_pInputBox ;

	cButton*		m_pExtendBtn ;
	cButton*		m_pReductionBtn ;
	cButton*		m_pCloseBtn ;
	cPushupButton*	m_pTabButton[e_CONVERSATION_MAX] ;
	CONVERSATION	m_byCurAssistance ;
	SEND_MSG_TYPE	m_byCurCommand ;
	int				m_nNameIndex;
	char			m_szWhisperName[128] ;
	char			m_szSendMsg[128] ;

public:
	CChattingDlg(void);
	virtual ~CChattingDlg(void);

	void Linking() ;

	virtual DWORD ActionEvent( CMouse* mouseInfo ) ;
	virtual DWORD ActionKeyboardEvent( CKeyboard* keyInfo ) ;

	BOOL Event_LButtonClick(CMouse* mouseInfo) ;
	void Event_RButtonClick(CMouse* mouseInfo) ;
	void Event_PushDown(CMouse* mouseInfo) ;

	cEditBox* GetInputBox() { return m_pInputBox ; }

	void SetCommandToInputBox() ;

	// CONVERSATION STRING PART.
	void InspectMsg(char* str) ;
	void FilteringMsg(char* str) ;
	void InspectCommand( char* str ) ;
	void FindUserName( char* name, char* msg ) ;

	BOOL Check_FaceCommand( char* str ) ;
	BOOL Check_LifeAction( char* str ) ;
	BOOL Check_EmotionCommand( char* str ) ;
	BOOL Check_EmotionWord( char* str ) ;
	void SendMsg() ;
	
	void Msg_All() ;
	void Msg_Party() ;
	void Msg_Family() ;
	void Msg_Guild() ;
	void Msg_Alliance() ;
	void Msg_Trade() ;
	void Msg_Whole() ;	
	void Msg_Whisper() ;
	int GetEmoticonNum(char* str);
	int GetCommand();
	void AddMsg( BYTE byMessageType, BYTE byTabBtnType, char* msg, DWORD dwColor ) ;
	void AddShoutMsg(char* msg, DWORD dwColor);

	void ExtendDlg() ;
	void ReductionDlg() ;
	void ResetAssist() ;
	void CallAssistanceDlg(CONVERSATION);
	void CallShoutDlg();

	DWORD GetCurMsgColor(BYTE msgType) ;

	cListDialog* GetChatListDlg() { return  m_pChatList ; }
	cListDialog* GetAssistListDlg() { return m_pAssistList ; }
	cListDialog* GetShoutListDlg() {return m_pShoutList;}
	void SetRaceActive(WORD wRace, BOOL bActive);
} ;
