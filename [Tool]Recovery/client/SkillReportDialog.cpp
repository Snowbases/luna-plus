// SkillReportDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "SkillReportDialog.h"
#include "SkillDialog.h"


// CSkillReportDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSkillReportDialog, CDialog)
CSkillReportDialog::CSkillReportDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CSkillReportDialog::IDD, pParent)
	, mApplication( application )
{
	EnableAutomation();
}


CSkillReportDialog::~CSkillReportDialog()
{}


void CSkillReportDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}


void CSkillReportDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SKILL_REPORT_LIST_CTRL, mReportListCtrl);
}


BEGIN_MESSAGE_MAP(CSkillReportDialog, CDialog)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CSkillReportDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_ISkillReportDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {4D618123-CCB0-461D-8637-EFCB2E430741}
static const IID IID_ISkillReportDialog =
{ 0x4D618123, 0xCCB0, 0x461D, { 0x86, 0x37, 0xEF, 0xCB, 0x2E, 0x43, 0x7, 0x41 } };

BEGIN_INTERFACE_MAP(CSkillReportDialog, CDialog)
	INTERFACE_PART(CSkillReportDialog, IID_ISkillReportDialog, Dispatch)
END_INTERFACE_MAP()



BOOL CSkillReportDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// 리스트 컨트롤 초기화
	{
		const TCHAR* separator = _T( "................................................" );

		{
			const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
			CRect rect;

			{
				mReportListCtrl.GetWindowRect( rect );

				mReportListCtrl.InsertColumn( 0, _T( "" ),	LVCFMT_LEFT, int( rect.Width() * 0.4f ) );
				mReportListCtrl.InsertColumn( 1, _T( "" ),	LVCFMT_LEFT, int( rect.Width() * 0.13f ) );
				mReportListCtrl.InsertColumn( 2, _T( "" ),	LVCFMT_LEFT, int( rect.Width() * 0.17f ) );
				mReportListCtrl.InsertColumn( 3, _T( "" ),	LVCFMT_LEFT, int( rect.Width() * 0.3f ) );
				mReportListCtrl.SetExtendedStyle( mReportListCtrl.GetExtendedStyle() | addStyle );

				// 헤더
				{
					const DWORD row = mReportListCtrl.GetItemCount();

					CString textName;
					textName.LoadString( IDS_STRING20 );
					mReportListCtrl.InsertItem( row, textName, 0 );

					CString textLevel;
					textLevel.LoadString( IDS_STRING5 );
					mReportListCtrl.SetItemText( row, 1, textLevel );

					CString textSkillPoint;
					textSkillPoint.LoadString( IDS_STRING96 );
					mReportListCtrl.SetItemText( row, 2, textSkillPoint );

					CString textMoney;
					textMoney.LoadString( IDS_STRING199 );
					mReportListCtrl.SetItemText( row, 3, textMoney );
				}
				
				// 구분선
				{
					const DWORD row = mReportListCtrl.GetItemCount();

					mReportListCtrl.InsertItem( row, separator, 0 );
					mReportListCtrl.SetItemText( row, 1, separator );
					mReportListCtrl.SetItemText( row, 2, separator );
					mReportListCtrl.SetItemText( row, 3, separator );
				}
			}
		}

		const CSkillDialog* dialog = mApplication.GetSkillDialog();
		ASSERT( dialog );

		const CSkillDialog::SkillMap& skillMap = dialog->mSkillMap;

		
		DWORD usedMoney = 0;
		DWORD usedPoint = 0;

		for(
			CSkillDialog::SkillMap::const_iterator it = skillMap.begin();
			skillMap.end() != it;
			++it )
		{
			const SKILL_BASE& skill = it->second;

			for( int level = 1; level <= skill.Level; ++level )
			{
				const ActiveSkillScript* script = mApplication.GetActiveSkillScript( skill.wSkillIdx / 100 * 100 + level );

				if( ! script )
				{
					continue;
				}

				{
					const DWORD row = mReportListCtrl.GetItemCount();

					mReportListCtrl.InsertItem( row, script->mName, 0 );

					CString text;
					text.Format( _T( "%d" ), level );
					mReportListCtrl.SetItemText( row, 1, text );

					text.Format( _T( "%d" ), script->mPoint );
					mReportListCtrl.SetItemText( row, 2, text );

					text.Format( _T( "%d" ), script->mMoney );
					mReportListCtrl.SetItemText( row, 3, text );
				}

				{
					usedMoney += script->mMoney;
					usedPoint += script->mPoint;
				}
			}
		}

		if( skillMap.empty() )
		{
			const DWORD row = mReportListCtrl.GetItemCount();

			CString textEmpty;
			textEmpty.LoadString( IDS_STRING203 );

			mReportListCtrl.InsertItem( row, textEmpty, 0 );
		}

		// 구분선
		{
			const DWORD row = mReportListCtrl.GetItemCount();

			mReportListCtrl.InsertItem( row, separator, 0 );
			mReportListCtrl.SetItemText( row, 1, separator );
			mReportListCtrl.SetItemText( row, 2, separator );
			mReportListCtrl.SetItemText( row, 3, separator );
		}

		// 결과
		{
			const DWORD row = mReportListCtrl.GetItemCount();

			CString textSum;
			textSum.LoadString( IDS_STRING204 );

			mReportListCtrl.InsertItem( row, textSum, 0 );

			CString text;
			text.Format( _T( "%d" ), usedPoint );
			mReportListCtrl.SetItemText( row, 2, text );

			text.Format( _T( "%d" ), usedMoney );
			mReportListCtrl.SetItemText( row, 3, text );
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}