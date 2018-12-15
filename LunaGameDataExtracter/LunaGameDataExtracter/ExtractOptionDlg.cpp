// ExtractOptionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LunaGameDataExtracter.h"
#include "ExtractOptionDlg.h"
#include ".\extractoptiondlg.h"


// CExtractOptionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CExtractOptionDlg, CDialog)
CExtractOptionDlg::CExtractOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExtractOptionDlg::IDD, pParent)
	, m_dw2DItemIconImageRangeMin(0)
	, m_dw2DItemIconImageRangeMax(0)
	, m_dw3DMonsterImageRangeMin(0)
	, m_dw3DMonsterImageRangeMax(0)
	, m_b2DItemIconImageCheckBox(FALSE)
	, m_b2DItemIconImageALLCheckBox(FALSE)
	, m_b3DMonsterImageCheckBox(FALSE)
	, m_b3DMonsterImageCheckBoxALL(FALSE)
	, m_b2DActiveSkillIconImageCheckBox(FALSE)
	, m_b2DActiveIconImageALLCheckBox(FALSE)
	, m_dw2DActiveIconImageRangeMin(0)
	, m_dw2DActiveIconImageRangeMax(0)
	, m_b2DBuffSkillIconImageCheckBox(FALSE)
	, m_b2DBuffIconImageALLCheckBox(FALSE)
	, m_dw2DBuffIconImageRangeMin(0)
	, m_dw2DBuffIconImageRangeMax(0)
	, m_bExtractItemInfo(FALSE)
	, m_bExtractFileText(FALSE)
	, m_bExtractFileExcel(FALSE)
	, m_bExtractMonsterInfo(FALSE)
	, m_bExtractSkillInfo(FALSE)
	, m_bExtractQuestInfo(FALSE)
{
}

CExtractOptionDlg::~CExtractOptionDlg()
{
}

void CExtractOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_2DITEM_ICON_IMAGE, m_b2DItemIconImageCheckBox);

	DDX_Check(pDX, IDC_CHECK_3DMONSTER_IMAGE, m_b3DMonsterImageCheckBox);
	DDX_Control(pDX, IDC_CHECK_2DITEM_ICON_IMAGE_ALL, m_2DItemIconImageALLCheckBox);
	DDX_Control(pDX, IDC_EDIT_2DITEM_ICON_IMAGE_RANGE_MIN, m_2DItemIconImageRangeMin);
	DDX_Control(pDX, IDC_EDIT_2DITEM_ICON_IMAGE_RANGE_MAX, m_2DItemIconImageRangeMax);
	DDX_Control(pDX, IDC_CHECK_3DMONSTER_IMAGE_ALL, m_3DMonsterImageCheckBoxALL);
	DDX_Control(pDX, IDC_EDIT_3DMONSTER_IMAGE_RANGE_MIN, m_3DMonsterImageRangeMin);
	DDX_Control(pDX, IDC_EDIT_3DMONSTER_IMAGE_RANGE_MAX, m_3DMonsterImageRangeMax);
	DDX_Check(pDX, IDC_CHECK_2DACTIVESKILL_IMAGE, m_b2DActiveSkillIconImageCheckBox);
	DDX_Control(pDX, IDC_CHECK_2DACTIVESKILL_IMAGE_ALL, m_2DActiveIconImageALLCheckBox);
	DDX_Control(pDX, IDC_EDIT_2DACTIVESKILL_IMAGE_RANGE_MIN, m_2DActiveIconImageRangeMin);
	DDX_Control(pDX, IDC_EDIT_2DACTIVESKILL_IMAGE_RANGE_MAX, m_2DActiveIconImageRangeMax);
	DDX_Check(pDX, IDC_CHECK_IDC_EDIT_2DBUFFSKILL_IMAGE, m_b2DBuffSkillIconImageCheckBox);
	DDX_Control(pDX, IDC_CHECK_2DBUFFSKILL_IMAGE_ALL, m_2DBuffIconImageALLCheckBox);
	DDX_Control(pDX, IDC_EDIT_2DBUFFSKILL_IMAGE_RANGE_MIN, m_2DBuffIconImageRangeMin);
	DDX_Control(pDX, IDC_EDIT_2DBUFFSKILL_IMAGE_RANGE_MAX, m_2DBuffIconImageRangeMax);
	DDX_Check(pDX, IDC_CHECK_ITEM_DATA, m_bExtractItemInfo);
	DDX_Check(pDX, IDC_CHECK_EXTRACT_FILE_TEXT, m_bExtractFileText);
	DDX_Check(pDX, IDC_CHECK_EXTRACT_FILE_EXCEL, m_bExtractFileExcel);
	DDX_Check(pDX, IDC_CHECK_MONSTER_DATA, m_bExtractMonsterInfo);
	DDX_Check(pDX, IDC_CHECK_SKILL_DATA, m_bExtractSkillInfo);
	DDX_Check(pDX, IDC_CHECK_QUEST_DATA, m_bExtractQuestInfo);
}


BEGIN_MESSAGE_MAP(CExtractOptionDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_2DITEM_ICON_IMAGE, OnBnClickedCheck2ditemIconImage)
	ON_BN_CLICKED(IDC_CHECK_2DITEM_ICON_IMAGE_ALL, OnBnClickedCheck2ditemIconImageAll)
	ON_BN_CLICKED(IDC_CHECK_3DMONSTER_IMAGE, OnBnClickedCheck3dmonsterImage)
	ON_BN_CLICKED(IDC_CHECK_3DMONSTER_IMAGE_ALL, OnBnClickedCheck3dmonsterImageAll)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_2DACTIVESKILL_IMAGE, OnBnClickedCheck2dactiveskillImage)
	ON_BN_CLICKED(IDC_CHECK_2DACTIVESKILL_IMAGE_ALL, OnBnClickedCheck2dactiveskillImageAll)
	ON_BN_CLICKED(IDC_CHECK_IDC_EDIT_2DBUFFSKILL_IMAGE, OnBnClickedCheckIdcEdit2dbuffskillImage)
	ON_BN_CLICKED(IDC_CHECK_2DBUFFSKILL_IMAGE_ALL, OnBnClickedCheck2dbuffskillImageAll)
END_MESSAGE_MAP()

BOOL CExtractOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_2DItemIconImageALLCheckBox.SetCheck( 0 );
	m_3DMonsterImageCheckBoxALL.SetCheck( 0 );
	m_2DActiveIconImageALLCheckBox.SetCheck( 0 );
	m_2DBuffIconImageALLCheckBox.SetCheck( 0 );

	m_2DItemIconImageALLCheckBox.EnableWindow( FALSE );
	m_3DMonsterImageCheckBoxALL.EnableWindow( FALSE );
	m_2DActiveIconImageALLCheckBox.EnableWindow( FALSE );
	m_2DBuffIconImageALLCheckBox.EnableWindow( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

int CExtractOptionDlg::GetTotalExtractDataCnt() const
{
	int cnt = 0;
	if( m_b2DItemIconImageCheckBox )
		++cnt;

	if( m_b3DMonsterImageCheckBox )
		++cnt;

	if( m_b2DActiveSkillIconImageCheckBox )
		++cnt;

	if( m_b2DBuffSkillIconImageCheckBox )
		++cnt;

	if( m_bExtractItemInfo )
		++cnt;

	if( m_bExtractMonsterInfo )
		++cnt;

	if( m_bExtractSkillInfo )
		++cnt;

	if( m_bExtractQuestInfo )
		++cnt;

	return cnt;
}

void CExtractOptionDlg::ClickedImageBtn( BOOL& bBtn, BOOL& bAllBtn, DWORD& dwMin, DWORD& dwMax, CButton& allBtn, CEdit& min, CEdit& max )
{
	UpdateData();

	if( bBtn )
	{
		allBtn.SetCheck( TRUE );
		allBtn.EnableWindow( TRUE );
		min.EnableWindow( FALSE );
		max.EnableWindow( FALSE );
	}
	else
	{
		allBtn.SetCheck( FALSE );
		allBtn.EnableWindow( FALSE );
		min.EnableWindow( FALSE );
		max.EnableWindow( FALSE );
	}

	dwMin = dwMax = 0;
	min.SetWindowText( "" );
	max.SetWindowText( "" );
}

void CExtractOptionDlg::ClickedImageALLBtn( BOOL& bBtn, BOOL& bAllBtn, DWORD& dwMin, DWORD& dwMax, CButton& allBtn, CEdit& min, CEdit& max )
{
	min.EnableWindow( allBtn.GetCheck() == 0 );
	max.EnableWindow( allBtn.GetCheck() == 0 );

	dwMin = dwMax = 0;
	min.SetWindowText( "" );
	max.SetWindowText( "" );
}


// CExtractOptionDlg 메시지 처리기입니다.
void CExtractOptionDlg::OnBnClickedCheck2ditemIconImage()
{
	ClickedImageBtn( m_b2DItemIconImageCheckBox, 
		m_b2DItemIconImageALLCheckBox,
		m_dw2DItemIconImageRangeMin,
		m_dw2DItemIconImageRangeMax,
		m_2DItemIconImageALLCheckBox,
		m_2DItemIconImageRangeMin,
		m_2DItemIconImageRangeMax );
}

void CExtractOptionDlg::OnBnClickedCheck2ditemIconImageAll()
{
	ClickedImageALLBtn( m_b2DItemIconImageCheckBox, 
		m_b2DItemIconImageALLCheckBox,
		m_dw2DItemIconImageRangeMin,
		m_dw2DItemIconImageRangeMax,
		m_2DItemIconImageALLCheckBox,
		m_2DItemIconImageRangeMin,
		m_2DItemIconImageRangeMax );
}

void CExtractOptionDlg::OnBnClickedCheck3dmonsterImage()
{
	ClickedImageBtn( m_b3DMonsterImageCheckBox, 
		m_b3DMonsterImageCheckBoxALL,
		m_dw3DMonsterImageRangeMin,
		m_dw3DMonsterImageRangeMax,
		m_3DMonsterImageCheckBoxALL,
		m_3DMonsterImageRangeMin,
		m_3DMonsterImageRangeMax );
}

void CExtractOptionDlg::OnBnClickedCheck3dmonsterImageAll()
{
	ClickedImageALLBtn( m_b3DMonsterImageCheckBox, 
		m_b3DMonsterImageCheckBoxALL,
		m_dw3DMonsterImageRangeMin,
		m_dw3DMonsterImageRangeMax,
		m_3DMonsterImageCheckBoxALL,
		m_3DMonsterImageRangeMin,
		m_3DMonsterImageRangeMax );
}

void CExtractOptionDlg::OnBnClickedCheck2dactiveskillImage()
{
	ClickedImageBtn( m_b2DActiveSkillIconImageCheckBox, 
		m_b2DActiveIconImageALLCheckBox,
		m_dw2DActiveIconImageRangeMin,
		m_dw2DActiveIconImageRangeMax,
		m_2DActiveIconImageALLCheckBox,
		m_2DActiveIconImageRangeMin,
		m_2DActiveIconImageRangeMax );
}

void CExtractOptionDlg::OnBnClickedCheck2dactiveskillImageAll()
{
	ClickedImageALLBtn( m_b2DActiveSkillIconImageCheckBox, 
		m_b2DActiveIconImageALLCheckBox,
		m_dw2DActiveIconImageRangeMin,
		m_dw2DActiveIconImageRangeMax,
		m_2DActiveIconImageALLCheckBox,
		m_2DActiveIconImageRangeMin,
		m_2DActiveIconImageRangeMax );
}

void CExtractOptionDlg::OnBnClickedCheckIdcEdit2dbuffskillImage()
{
	ClickedImageBtn( m_b2DBuffSkillIconImageCheckBox, 
		m_b2DBuffIconImageALLCheckBox,
		m_dw2DBuffIconImageRangeMin,
		m_dw2DBuffIconImageRangeMax,
		m_2DBuffIconImageALLCheckBox,
		m_2DBuffIconImageRangeMin,
		m_2DBuffIconImageRangeMax );
}

void CExtractOptionDlg::OnBnClickedCheck2dbuffskillImageAll()
{
	ClickedImageALLBtn( m_b2DBuffSkillIconImageCheckBox, 
		m_b2DBuffIconImageALLCheckBox,
		m_dw2DBuffIconImageRangeMin,
		m_dw2DBuffIconImageRangeMax,
		m_2DBuffIconImageALLCheckBox,
		m_2DBuffIconImageRangeMin,
		m_2DBuffIconImageRangeMax );
}


void CExtractOptionDlg::OnBnClickedOk()
{
	CString txt;

	// Update Item icon image info
	m_b2DItemIconImageALLCheckBox = m_2DItemIconImageALLCheckBox.GetCheck() > 0;
	m_2DItemIconImageRangeMin.GetWindowText( txt );
	m_dw2DItemIconImageRangeMin = (DWORD)_ttoi( txt );
	m_2DItemIconImageRangeMax.GetWindowText( txt );
	m_dw2DItemIconImageRangeMax = (DWORD)_ttoi( txt );

	// Update Monster image info
	m_b3DMonsterImageCheckBoxALL = m_3DMonsterImageCheckBoxALL.GetCheck() > 0;
	m_3DMonsterImageRangeMin.GetWindowText( txt );
	m_dw3DMonsterImageRangeMin= (DWORD)_ttoi( txt );
	m_3DMonsterImageRangeMax.GetWindowText( txt );
	m_dw3DMonsterImageRangeMax = (DWORD)_ttoi( txt );

	// Update Active Skill image info
	m_b2DActiveIconImageALLCheckBox = m_2DActiveIconImageALLCheckBox.GetCheck() > 0;
	m_2DActiveIconImageRangeMin.GetWindowText( txt );
	m_dw2DActiveIconImageRangeMin= (DWORD)_ttoi( txt );
	m_2DActiveIconImageRangeMax.GetWindowText( txt );
	m_dw2DActiveIconImageRangeMax = (DWORD)_ttoi( txt );

	// Update Buff Skill image info
	m_b2DBuffIconImageALLCheckBox = m_2DBuffIconImageALLCheckBox.GetCheck() > 0;
	m_2DBuffIconImageRangeMin.GetWindowText( txt );
	m_dw2DBuffIconImageRangeMin= (DWORD)_ttoi( txt );
	m_2DBuffIconImageRangeMax.GetWindowText( txt );
	m_dw2DBuffIconImageRangeMax= (DWORD)_ttoi( txt );


	OnOK();
}