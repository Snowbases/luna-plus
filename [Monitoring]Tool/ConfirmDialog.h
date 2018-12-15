/*
080822 LUJ, 서버 닫기 전에 확인을 받는 창
*/
#pragma once


// CConfirmDialog 대화 상자입니다.

class CConfirmDialog : public CDialog
{
	DECLARE_DYNAMIC(CConfirmDialog)

public:
	CConfirmDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CConfirmDialog();

	virtual void OnFinalRelease();
	virtual void OnOK();

	// 080822 LUJ, 입력된 문자열이 일치하는지 비교한다
public:
	inline BOOL IsConfirm() const
	{
		return mInputText == mConfirmText;
	}
private:
	CString mInputText;
	CString mConfirmText;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONFIRMDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	CEdit	mConfirmTextEdit;
	CStatic	mConfirmTextStatic;
};
