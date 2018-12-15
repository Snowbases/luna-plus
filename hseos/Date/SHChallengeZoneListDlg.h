#ifndef _SHCHALLENGEZONELISTDLG
#define _SHCHALLENGEZONELISTDLG

#include "./Interface/cDialog.h"
#include "./Interface/cListCtrl.h"

class cGuageBar;
class cStatic;
class CSHChallengeZoneListDlg : public cDialog
{
private:
	cListCtrl*		m_pcsChallengeZoneList;
	int				m_nSelChallengeZone;

public:
	CSHChallengeZoneListDlg() ;
	~CSHChallengeZoneListDlg() ;

	void		 	Linking();
	virtual void 	SetActive( BOOL val );
	virtual void 	Render();
	virtual DWORD	ActionEvent(CMouse* mouseInfo);
	void			OnActionEvent( LONG lId, void* p, DWORD we );

	void			SelectChallengeZone(int nRowIndex);

// 091120 ONS 경험치 분배 버튼/게이지바 설정
private:
	cButton*		m_GuageRightBtn;
	cButton*		m_GuageLeftBtn;
	cButton*		m_MoveBtn;
	cButton*		m_ConfirmBtn;
	cButton*		m_CancleBtn;
	cGuageBar*		m_GuageBar;
	cStatic*		m_Confirm;
	cStatic*		m_GuageManRate;
	cStatic*		m_GuageWomanRate;
	BOOL			PushedGuageBtn;

	BOOL			m_bDiffAgent;
	DWORD			m_dwZoneIndex;
	DWORD			m_dwExpRate;
	DWORD			m_dwDiffAgentID;
	DWORD			m_dwPartnerIndex;
public:
	void			SetDistributeRateByBtn(DWORD dwWindowID);
	void			InitControls();
	void			SetGuestMode(DWORD dwZoneIndex, DWORD dwExpRate, BOOL bDiffAgent=FALSE, DWORD dwDiffAgentID=0, DWORD dwPartnerIndex=0);
};



#endif