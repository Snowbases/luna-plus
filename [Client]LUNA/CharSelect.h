// CharSelect.h: interface for the CCharSelect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHARSELECT_H__DE3122B4_F68B_4CDE_8F3E_C718A7BBCDA8__INCLUDED_)
#define AFX_CHARSELECT_H__DE3122B4_F68B_4CDE_8F3E_C718A7BBCDA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GameState.h"

#define CHARSELECT USINGTON(CCharSelect)

class CPlayer;
class cDialog;
class CProfileDlg;
class CCertificateDlg;
class CCameraConfiguration;
class CChannelDialog;
class cImageSelf;

class CCharSelect : public CGameState  
{
	DWORD m_CurSelectedPlayer;
	CPlayer* m_pPlayer[MAX_CHARACTER_NUM];

//	BOOL m_bDoublcClicked;
	CPlayer* m_pDoublcClickedPlayer;
	CCameraConfiguration* mCameraConfiguration;
	void SetCurSelctedPlayer(DWORD);
	void PlaySelectedMotion(DWORD);
	void PlayDeselectedMotion(DWORD);

	BOOL m_bDiablePick;
	BOOL mIsNoResourceLoaded;
	CChannelDialog* m_pChannelDlg;
	
//KES
	BOOL m_bBackToMainTitle;

	cImageSelf* mImageBar;
	VECTOR2 mPositionBar;
	VECTOR2 mScaleBar;
	cImageSelf* mImageLogo;	
	VECTOR2 mPositionLogo;
	VECTOR2	mScaleLogo;

	BOOL m_bEnterGame;

	CProfileDlg* m_pProfile;
	CCertificateDlg* m_pCertificateDlg;
	// 최대 서버셋 개수만큼 있어야한다
	int m_MaxChannel[10];

public:	

	void SetMaxChannel( int set, int num ) { m_MaxChannel[set] = num; }
	int GetMaxChannel( int set ) { return m_MaxChannel[set]; }

	CCharSelect();
	virtual ~CCharSelect();
	
	////MAKESINGLETON(CCharSelect)

	BOOL Init(void* pInitParam);
	void Release(CGameState* pNextGameState);

	void Process();
	void BeforeRender();
	void AfterRender();
	
	void NetworkMsgParse(BYTE Category,BYTE Protocol,void* pMsg, DWORD dwMsgSize);

	void SelectPlayer(int num);	// 0 <= num  <= 3  서있는 자리 위치번호
	CPlayer* GetCurSelectedPlayer();
	int GetCurSelectedPlayerNum() { return m_CurSelectedPlayer; }

	BOOL IsFull();
	
	void SendMsgGetChannelInfo();
	BOOL EnterGame();
	void DeleteCharacter();

	void DisplayNotice(int MsgNum);
	
	void SetDisablePick( BOOL val );
	void BackToMainTitle();

	void OnDisconnect();
	
	void SetChannelDialog( CChannelDialog* pDlg ) { m_pChannelDlg = pDlg; }
	CChannelDialog* GetChannelDialog() { return m_pChannelDlg; }
	// 061215 LYW --- Add function to return and setting profile dialog.
	void SetProfileDlg( CProfileDlg* pDlg ) { m_pProfile = pDlg ; }
	CProfileDlg* GetProfileDlg() { return m_pProfile ; }

	// 061218 LYW --- Add function to return and setting certificate dialog.
	void SetCertificateDlg( CCertificateDlg* pDlg ) { m_pCertificateDlg = pDlg ; }
	CCertificateDlg* GetCertificateDlg() { return m_pCertificateDlg ; }
	// 090424 ONS 신규종족 생성 조건(레벨:50이상)을 체크하는 함수
	BOOL HasLevelOfChar( const LEVELTYPE level ) const;

private:
	void UserConn_CharacterList_Ack( void* pMsg ) ;
	void UserConn_Character_Remove_Ack() ;
	void UserConn_Character_Remove_Nack( void* pMsg ) ;
	void UserConn_DisConnect_Ack() ;
	void UserConn_ChannelInfo_Ack( void* pMsg ) ;
	void UserConn_ChannelInfo_Nack( void* pMsg ) ;
	void Cheat_EventNotify_On( void* pMsg ) ;
	void Cheat_EventNotify_Off( void* pMsg ) ;
	// desc_hseos_성별선택01
	// S 성별선택 추가 added by hseos 2007.06.17
	void UserConn_Character_SexKind(void *pMsg);
	void AdjustBar();
};
EXTERNGLOBALTON(CCharSelect)
#endif // !defined(AFX_CHARSELECT_H__DE3122B4_F68B_4CDE_8F3E_C718A7BBCDA8__INCLUDED_)
