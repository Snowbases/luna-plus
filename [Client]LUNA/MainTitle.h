// MainTitle.h: interface for the CMainTitle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINTITLE_H__82C6A42B_0EEA_4F49_BA0C_33F88B3FFEF3__INCLUDED_)
#define AFX_MAINTITLE_H__82C6A42B_0EEA_4F49_BA0C_33F88B3FFEF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GameState.h"

#define TITLE USINGTON(CMainTitle)

class cImageSelf;
class CCameraConfiguration;
class cDialog;
class CServerListDialog;


class CMainTitle : public CGameState  
{
	CCameraConfiguration* mCameraConfiguration;
	BOOL	m_bDisconntinToDist;
	DWORD	m_DistAuthKey;
	cDialog*	m_pLogoWindow;
	DWORD		m_dwStartTime;
	BOOL		m_bInit;
	CServerListDialog*	m_pServerListDlg;
	char				m_DistributeAddr[16];
	WORD				m_DistributePort;
	BOOL				m_bServerList;
	DWORD		m_dwWaitTime;	//for agent connection
	BOOL		m_bWaitConnectToAgent;
	BOOL		m_bNoDiconMsg;

	DWORD		m_ConnectionServerNo;

	// 070208 LYW --- MainTitle : Add variables for position and scale of image.
	VECTOR2		m_scale ;
// 090929 ONS 2D로그인 이미지를 추가한다.
private:
	cImageSelf*		m_2DLoginImage;
	cImageSelf*		m_2DLoginLoadingImage;
	VECTOR2			m_tr;
	VECTOR2			mPositionLoadingImage;
	VECTOR2			mScaleLoadingImage;
	
	cImageSelf* mImageBar;
	VECTOR2 mPositionBar;
	VECTOR2 mScaleBar;
	cImageSelf* mImageLogo;	
	VECTOR2 mPositionLogo;
	VECTOR2	mScaleLogo;

	std::string mLoginKey;
	BOOL m_bDynamicDlg;

public:
	CMainTitle();
	virtual ~CMainTitle();
	BOOL	Init(void* pInitParam);
	void	Release(CGameState* pNextGameState);

	void	Process();
	void	BeforeRender();
	void	AfterRender();	
	void	NetworkMsgParse(BYTE Category,BYTE Protocol, LPVOID, DWORD dwMsgSize);
	DWORD	GetDistAuthKey() const { return m_DistAuthKey; }
	DWORD	GetUserIdx() const { return gUserID; }
	void	OnLoginError(DWORD errorcode,DWORD dwParam);
	void	OnDisconnect();
	
	void ConnectToServer(int index);
	void ShowServerList();
	void SetServerList() { m_bServerList = TRUE; }
	CServerListDialog*	GetServerListDialog() { return m_pServerListDlg; }
	void SetLoginKey(LPCTSTR loginKey) { mLoginKey = loginKey; }
	const std::string& GetLoginKey() const { return mLoginKey; }
	void StartWaitConnectToAgent(BOOL bStart);
	void NoDisconMsg() { m_bNoDiconMsg = TRUE; }

private:
	void UserConn_Use_Dynamic_Ack( void* pMsg ) ;
	void UserConn_Dist_ConnectSuccess( void* pMsg ) ;
	void UserConn_Login_Ack( void* pMsg ) ;
	void UserConn_Login_Nack( void* pMsg ) ;
	void UserConn_Server_NotReady() ;
	void UserConn_CharacterList_Ack( void* pMsg ) ;
	void UserConn_CharacterList_Nack() ;
	void UserConn_Agent_ConnectSuccess( void* pMsg ) ;
	void Cheat_EventNotify_On( void* pMsg ) ;
	void Cheat_EventNotify_Off( void* pMsg ) ;
	// desc_hseos_성별선택01
	// S 성별선택 추가 added by hseos 2007.06.16
	void UserConn_Character_SexKind(void *pMsg);
	// E 성별선택 추가 added by hseos 2007.06.16

	/// ON LOGIN ERROR PART.
	void Login_Error_Invalid_Version() ;
	void Login_Error_OverLappedLogin() ;
	void Login_Error_OverLappedLoginiNotherServer( DWORD dwParam ) ;
	void Login_Error_NoAgentServer() ;
	void Login_Error_NoDistributeServer() ;
	void Login_Error_InvalidUserLevel() ;
	void Login_Error_DistServerIsBusy() ;
	void Login_Error_WrongIdPw() ;
	void Login_Error_BlockUserLevel( DWORD dwRemainTime ) ;
	void Login_Error_Invalid_Ip() ;
	void Login_Error_DistConnet_Error() ;
	void Login_Error_MaxUser() ;
	void Login_Error_Minor_InadultServer() ;
	void Login_Error_SecededAccount() ;
	void Login_Error_Not_CloseBetaWinner() ;
	void Login_Error_NoreMainTime() ;
	void Login_Error_NoIpRegen() ;
	void Login_Error_Invalid_Sec_Pass() ;
	void Login_Error_Change_Sec_Pass() ;
	void Login_Error_File_CRC();
	void Login_Error_LoginSession_Invalid();

	void AdjustBar();
	void SetPositionScale();
	void AlignWindow();
};

EXTERNGLOBALTON(CMainTitle)

#endif // !defined(AFX_MAINTITLE_H__82C6A42B_0EEA_4F49_BA0C_33F88B3FFEF3__INCLUDED_)
