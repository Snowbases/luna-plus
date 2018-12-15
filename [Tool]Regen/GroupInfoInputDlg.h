#pragma once


// CGroupInfoInputDlg 대화 상자입니다.

class CGroupInfoInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CGroupInfoInputDlg)

public:
	CGroupInfoInputDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CGroupInfoInputDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_GROUP_INPUT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_GroupName;
	CEdit m_TargetGroup;
	CEdit m_Ratio;
	CEdit m_Delay;
	CEdit m_IsRegen;
	CEdit m_RegenRange;

	CString m_strTitle;

	CString m_strGroupName;
	DWORD m_dwTargetGroup;
	float m_fRatio;
	DWORD m_dwDelay;
	int m_nIsRegen;
	DWORD m_dwRegenRange;

	BOOL m_bReadOnlyGroupName;
	BOOL m_bReadOnlyTargetGroup;
	BOOL m_bReadOnlyRatio;
	BOOL m_bReadOnlyDelay;
	BOOL m_bReadOnlyIsRegen;
	BOOL m_bReadOnlyRegenRange;

	void SetTitleName( const CString& str ) { m_strTitle = str; }

	void SetGroupName( CString& str )				{ m_strGroupName = str; }
	void SetTargetGroup( DWORD dwTargetGroup)		{ m_dwTargetGroup = dwTargetGroup; }
	void SetRatio( float fRatio )					{ m_fRatio = fRatio; }
	void SetDelay( DWORD dwDelay )					{ m_dwDelay = dwDelay; }
	void SetIsRegen( int nIsRegen )					{ m_nIsRegen = nIsRegen > 0 ? 1 : 0; }
	void SetRegenRange( DWORD dwRange )				{ m_dwRegenRange = dwRange; }

	void SetReadyOnlyGroupName( BOOL bReadyOnly )	{ m_bReadOnlyGroupName = bReadyOnly; }
	void SetReadyOnlyTargetGroup( BOOL bReadyOnly ) { m_bReadOnlyTargetGroup = bReadyOnly; }
	void SetReadyOnlyRatio( BOOL bReadyOnly )		{ m_bReadOnlyRatio = bReadyOnly; }
	void SetReadyOnlyDelay( BOOL bReadyOnly )		{ m_bReadOnlyDelay = bReadyOnly; }
	void SetReadyOnlyIsRegen( BOOL bReadyOnly )		{ m_bReadOnlyIsRegen = bReadyOnly; }
	void SetReadyOnlyRegenRange( BOOL bReadyOnly )	{ m_bReadOnlyRegenRange = bReadyOnly; }

	CString GetGroupName() const	{ return m_strGroupName; }
	DWORD GetTargetGroup() const	{ return m_dwTargetGroup; }
	float GetRatio() const			{ return m_fRatio; }
	DWORD GetDelay() const			{ return m_dwDelay; }
	BOOL GetIsRegen() const			{ return (m_nIsRegen > 0 ? TRUE : FALSE); }
	DWORD GetRegenRange() const		{ return m_dwRegenRange; }
	virtual BOOL OnInitDialog();
};
