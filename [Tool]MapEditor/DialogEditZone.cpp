// DialogEditZone.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "4DyuchiGXMapEditor.h"
#include "DialogEditZone.h"
#include ".\dialogeditzone.h"


// CDialogEditZone 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogEditZone, CDialog)
CDialogEditZone::CDialogEditZone(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogEditZone::IDD, pParent)
{
	m_bShow = FALSE;
	m_pZoneDesc = NULL;

	Create(CDialogEditZone::IDD,pParent);
}

CDialogEditZone::~CDialogEditZone()
{
}

void CDialogEditZone::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogEditZone, CDialog)
END_MESSAGE_MAP()


// CDialogEditZone 메시지 처리기입니다.

BOOL CDialogEditZone::OnInitDialog()
{
	CDialog::OnInitDialog();

	RECT rect;
	GetClientRect(&rect);
	rect.right -= 2;
	rect.bottom -= 55;

	int width = (rect.right - rect.left - 20)/3;


	m_cGridZone.Create(rect, this->GetSafeHwnd(), 3);
	m_cGridZone.SetColumnInfo(0, " Idx ", width, DGTA_CENTER);
	m_cGridZone.SetColumnInfo(1, " CharMove ", width, DGTA_CENTER);
	m_cGridZone.SetColumnInfo(2, " MonsterMove ", width, DGTA_CENTER);
	//m_cGridZone.EnableEdit(TRUE);
	//m_cGridZone.EnableGrid(TRUE);
	

	// @@@
	char str[10];
	for (int row=0; row<MAX_ZONE_NUM; ++row)
	{
		sprintf( str, "%d", row );
		m_cGridZone.InsertItem(str, DGTA_CENTER);
		m_cGridZone.SetItemInfo( row, 1, "1", DGTA_CENTER, false );
		m_cGridZone.SetItemInfo( row, 2, "1", DGTA_CENTER, false );
	}

	LOGFONT lf;
	m_cGridZone.GetColumnFont(&lf);
	lf.lfWeight = FW_BOLD;
	m_cGridZone.SetColumnFont(&lf);

	m_cGridZone.Update();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void CDialogEditZone::OnShow() 
{
	m_bShow = !m_bShow;
	//if(m_bShow)
	//	m_ShowBtn.SetWindowText("Hide");
	//else
	//	m_ShowBtn.SetWindowText("Show");

	//m_pPaletteDlg->RenderPalette();
}

void CDialogEditZone::OnOK() 
{
	if(m_bShow)
		OnShow();

	UpdateData();

	//CDialog::OnOK();
}

void CDialogEditZone::OnCancel() 
{
	if(m_bShow)
		OnShow();

	UpdateGrid();

	CDialog::OnCancel();
}

void CDialogEditZone::EnableView()
{
	ShowWindow(SW_SHOWDEFAULT);
}

void CDialogEditZone::DisableView()
{
	ShowWindow(SW_HIDE);
}

void CDialogEditZone::SetZoneDesc(GXZONE_DESC* pZoneDesc)
{
	m_pZoneDesc = pZoneDesc;

	Update(FALSE);
}

void CDialogEditZone::Update(BOOL bSave)
{
	if (bSave)
		UpdateData();
	else
		UpdateGrid();
}

void CDialogEditZone::UpdateData()
{
	if (!m_pZoneDesc)
		return;

	char buf[10];
	for (DWORD row=0; row<MAX_ZONE_NUM; ++row)
	{
		m_cGridZone.GetItemText(row, 1, buf, 10);
		m_pZoneDesc[row].dwMoveChar = atoi(buf);
		m_cGridZone.GetItemText(row, 2, buf, 10);
		m_pZoneDesc[row].dwMoveMon = atoi(buf);
	}
}

void CDialogEditZone::UpdateGrid()
{
	if (!m_pZoneDesc)
		return;

	char str[10];
	for (DWORD row=0; row<MAX_ZONE_NUM; ++row)
	{
		sprintf( str, "%u", m_pZoneDesc[row].dwMoveChar);
		m_cGridZone.SetItemInfo( row, 1, str, DGTA_CENTER, false );
		sprintf( str, "%u", m_pZoneDesc[row].dwMoveMon);
		m_cGridZone.SetItemInfo( row, 2, str, DGTA_CENTER, false );
	}
}
