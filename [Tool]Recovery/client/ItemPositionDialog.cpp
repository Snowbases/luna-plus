// ItemPositionDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "ItemPositionDialog.h"
#include ".\itempositiondialog.h"


// CItemPositionDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CItemPositionDialog, CDialog)
CItemPositionDialog::CItemPositionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CItemPositionDialog::IDD, pParent)
	, mAreaType( Item::AreaNone )
{
	EnableAutomation();
}

CItemPositionDialog::~CItemPositionDialog()
{
}

void CItemPositionDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CItemPositionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ITEM_POSITION_INVENTORY_RADIO, mInventoryRadioButton );
	DDX_Control(pDX, IDC_ITEM_POSITION_PRIVATE_STORAGE_RADIO, mPrivateStorageRadioButton );
	DDX_Control(pDX, IDC_ITEM_POSITION_CASH_STORAGE_RADIO, mCashStorageRadioButton );
}


BEGIN_MESSAGE_MAP(CItemPositionDialog, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CItemPositionDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IItemPositionDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {2D0B3A3A-534F-4ACF-9F87-33ECE4809188}
static const IID IID_IItemPositionDialog =
{ 0x2D0B3A3A, 0x534F, 0x4ACF, { 0x9F, 0x87, 0x33, 0xEC, 0xE4, 0x80, 0x91, 0x88 } };

BEGIN_INTERFACE_MAP(CItemPositionDialog, CDialog)
	INTERFACE_PART(CItemPositionDialog, IID_IItemPositionDialog, Dispatch)
END_INTERFACE_MAP()


// CItemPositionDialog 메시지 처리기입니다.

void CItemPositionDialog::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( mInventoryRadioButton.GetCheck() )
	{
		mAreaType = Item::AreaInventory;
	}
	else if( mPrivateStorageRadioButton.GetCheck() )
	{
		mAreaType = Item::AreaPrivateStorage;
	}	
	else if( mCashStorageRadioButton.GetCheck() )
	{
		mAreaType = Item::AreaCashStorage;
	}
	
	OnOK();
}


void CItemPositionDialog::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	mAreaType = Item::AreaNone;

	OnCancel();
}


INT_PTR CItemPositionDialog::DoModal( POSTYPE InventoryPosition, POSTYPE privateStoragePosition, POSTYPE cashStoragePosition )
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	mIsInventoryPosition		= ( USHRT_MAX != InventoryPosition );
	mIsPrivateStoragePosition	= ( USHRT_MAX != privateStoragePosition );
	mIsCashStoragePosition		= ( USHRT_MAX != cashStoragePosition );

	return CDialog::DoModal();
}


Item::Area CItemPositionDialog::GetAreaType() const
{
	return mAreaType;
}


BOOL CItemPositionDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	mInventoryRadioButton.EnableWindow		( mIsInventoryPosition );
	mPrivateStorageRadioButton.EnableWindow	( mIsPrivateStoragePosition );
	mCashStorageRadioButton.EnableWindow	( mIsCashStoragePosition );

	if( mIsInventoryPosition )
	{
		mInventoryRadioButton.SetCheck( 1 );
	}
	else if( mIsPrivateStoragePosition )
	{
		mPrivateStorageRadioButton.SetCheck( 1 );
	}
	else if( mIsCashStoragePosition )
	{
		mCashStorageRadioButton.SetCheck( 1 );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
