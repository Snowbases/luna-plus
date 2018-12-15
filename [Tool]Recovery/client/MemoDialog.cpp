// MemoDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "MemoDialog.h"
#include ".\memodialog.h"


// CMemoDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMemoDialog, CDialog)
CMemoDialog::CMemoDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMemoDialog::IDD, pParent)
{
	EnableAutomation();

}

CMemoDialog::~CMemoDialog()
{
}

void CMemoDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CMemoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMO_EDIT_CTRL, mMemoEditCtrl);
}

void CMemoDialog::SetText(LPCTSTR text)
{
	mMemoEditCtrl.SetWindowText(
		text);
}


BEGIN_MESSAGE_MAP(CMemoDialog, CDialog)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMemoDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IMemoDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {060057AD-D423-423D-A75C-469ED798C544}
static const IID IID_IMemoDialog =
{ 0x60057AD, 0xD423, 0x423D, { 0xA7, 0x5C, 0x46, 0x9E, 0xD7, 0x98, 0xC5, 0x44 } };

BEGIN_INTERFACE_MAP(CMemoDialog, CDialog)
	INTERFACE_PART(CMemoDialog, IID_IMemoDialog, Dispatch)
END_INTERFACE_MAP()