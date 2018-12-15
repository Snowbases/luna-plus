#pragma once

#include "afxwin.h"
#include "afxcmn.h"

class CclientApp;

// CTournamentDialog 대화 상자입니다.

class CTournamentDialog : public CDialog
{
	DECLARE_DYNAMIC(CTournamentDialog)

public:
	CTournamentDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTournamentDialog();

	virtual void OnFinalRelease();
	void Parse( DWORD serverIndex, const MSGROOT* );
	void Request( const CString& serverName );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TOURNAMENTDIALOG };

private:
	CclientApp&	mApplication;
	DWORD		mServerIndex;
	DWORD		mMaxTournamentCount;
	DWORD		mSelectedGuildIndex;
	DWORD		mSelectedPlayerIndex;

	// 081027 LUJ, 길드 별 참가 정보
	struct Unit
	{
		typedef MSG_RM_TOURNAMENT_GUILD::Guild	Guild;
		Guild									mGuild;
		// 081027 LUJ, 값: 플레이어 번호
		typedef std::list< DWORD >				PlayerList;
		PlayerList								mPlayerList;
	};	
	// 081027 LUJ, 키: 길드 번호, 값: 길드에 속한 플레이어
	typedef stdext::hash_map< DWORD, Unit >			UnitMap;
	// 081027 LUJ, 길드 토너먼트의 참가 길드와 플레이어 정보가 담긴다. 키: 회차
	typedef stdext::hash_map< DWORD, UnitMap >		TournamentMap;
	TournamentMap									mTournamentMap;
	// 081027 LUJ, 길드 이름이 담긴다
	// 081027 LUJ, 키: 인덱스. 값: 이름
	typedef stdext::hash_map< DWORD, CString >		NameMap;
	NameMap											mGuildNameMap;
	NameMap											mPlayerNameMap;
	// 081027 LUJ, 토너먼트 참가 길드 정보가 표시된다
	CTreeCtrl	mGuildTreeCtrl;
	// 081027 LUJ, 대진표 정보가 표시된다
	CTreeCtrl	mTableTreeCtrl;
	CComboBox	mServerCombo;
	CComboBox	mTournamentCountCombo;
	CStatic		mStatusStatic;
private:
	void RefreshGuildTree( CTreeCtrl&, const UnitMap& );
	void RefreshTableTree( CTreeCtrl&, const UnitMap& );
	CString	GetServerName()	const;
	void MakeHeader( CString& ) const;
	CString GetName( const NameMap&, DWORD index ) const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCbnSelchangeTournamentCountCombo();
	afx_msg void OnTvnSelchangedTournamentGuildTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeTournamentServerCombo();
};