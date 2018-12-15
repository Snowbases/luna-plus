/*
080303	LUJ, 아이템을 저장할 위치를 선택하는 창
*/
#pragma once


// CItemPositionDialog 대화 상자입니다.

class CItemPositionDialog : public CDialog
{
	DECLARE_DYNAMIC(CItemPositionDialog)

public:
	CItemPositionDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CItemPositionDialog();

	virtual void OnFinalRelease();

	// 080303 LUJ, 아이템 저장 위치를 반환
	Item::Area GetAreaType() const;

	
// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEMPOSITIONDIALOG };

	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()


private:
	Item::Area	mAreaType;
	
	CButton mInventoryRadioButton;
	CButton mPrivateStorageRadioButton;
	CButton mCashStorageRadioButton;

	bool mIsInventoryPosition;
	bool mIsPrivateStoragePosition;
	bool mIsCashStoragePosition;

	
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual INT_PTR DoModal( POSTYPE InventoryPosition, POSTYPE privateStoragePosition, POSTYPE cashStoragePosition );
	virtual BOOL OnInitDialog();
};
