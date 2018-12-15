#include "StdAfx.h"
#include "interface/cCheckBox.h"
#include "interface/cEditBox.h"
#include "cIMEex.h"
#include "WindowIDEnum.h"
#include "cWindowManager.h"
#include "RideOptionDlg.h"
#include "OptionManager.h"

void OnActionEventRideOptionDialog(LONG windowIndex, LPVOID eventThrower, DWORD windowEvent)
{
	cDialog* const dialog = WINDOWMGR->GetWindowForID(
		TB_STATE_PUSHUP2);

	if(0 == dialog)
	{
		return;
	}

	dialog->OnActionEvent(
		windowIndex,
		eventThrower,
		windowEvent);
}

CRideOptionDlg::CRideOptionDlg() :
mAllowCheckBox(0),
mMoneyEditBox(0),
mIsUpdateComma(FALSE)
{}

CRideOptionDlg::~CRideOptionDlg()
{}

void CRideOptionDlg::SetActive(BOOL isActive)
{
	if(isActive)
	{
		Linking();

		SHORT_RECT rect = {0};
		WORD value = 0;
		g_pExecutive->GetRenderer()->GetClientRect(
			&rect,
			&value,
			&value);
		SetAbsXY(
			(rect.right - GetWidth()) / 2,
			(rect.bottom - GetHeight()) / 2);
	}

	cDialog::SetActive(
		isActive);
}

void CRideOptionDlg::Render()
{
	cDialog::Render();

	if(FALSE == IsActive())
	{
		return;
	}
	else if(mMoneyEditBox->IsFocus())
	{
		mIsUpdateComma = TRUE;

		const DWORD commaSize = (mMoneyEditBox->GetIME()->GetBufferSize() - 1) / 3;
		const DWORD enableSize = mMoneyEditBox->GetIME()->GetBufferSize() - commaSize;

		if(enableSize < _tcslen(mMoneyEditBox->GetEditText()))
		{
			TCHAR text[MAX_PATH] = {0};
			_tcsncpy(
				text,
				mMoneyEditBox->GetEditText(),
				enableSize);
			mMoneyEditBox->SetEditText(
				text);
		}
	}
	else if(mIsUpdateComma)
	{
		const DWORD money = min(
			_ttoi(mMoneyEditBox->GetEditText()),
			MAX_INVENTORY_MONEY);
		mMoneyEditBox->SetEditText(
			0 == money ? "0" : AddComma(money));
		mIsUpdateComma = FALSE;
	}
}

void CRideOptionDlg::OnActionEvent(LONG windowIndex, LPVOID, DWORD event)
{
	switch(windowIndex)
	{
	case TB_STATE_PO:
		{
			LPCTSTR text = mMoneyEditBox->GetEditText();
			mMoneyEditBox->SetEditText(
				0 == _tcslen(text) ? _T("0") : RemoveComma(LPTSTR(text)));
			break;
		}
	case TB_STATE_PUSHUP3:
		{
			const BOOL isNeedAllow = mAllowCheckBox->IsChecked();
			const MONEYTYPE chargedMoney = _ttoi(
				RemoveComma(mMoneyEditBox->GetEditText()));

			sGAMEOPTION* const gameOption = OPTIONMGR->GetGameOption();
			gameOption->mVehicle.mIsNeedAllow = isNeedAllow;
			gameOption->mVehicle.mChargedMoney = chargedMoney;
			OPTIONMGR->SendOptionMsg();
			OPTIONMGR->SaveGameOption();

			SetActive(
				FALSE);
			break;
		}
	}
}

void CRideOptionDlg::Linking()
{
	if(0 < mAllowCheckBox)
	{
		return;
	}

	SetcbFunc(
		::OnActionEventRideOptionDialog);

	const sGAMEOPTION* const gameOption = OPTIONMGR->GetGameOption();

	mAllowCheckBox = (cCheckBox*)GetWindowForID(
		TB_STATE_PUSHUP4);

	if(WT_CHECKBOX == mAllowCheckBox->GetType())
	{
		mAllowCheckBox->SetChecked(
			gameOption->mVehicle.mIsNeedAllow);
	}

	mMoneyEditBox = (cEditBox*)GetWindowForID(
		TB_STATE_PO);

	if(WT_EDITBOX == mMoneyEditBox->GetType())
	{
		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			_T("%s"),
			AddComma(gameOption->mVehicle.mChargedMoney));
		mMoneyEditBox->SetEditText(
			text);
		mMoneyEditBox->SetcbFunc(
			::OnActionEventRideOptionDialog);
	}

	cWindow* const window = GetWindowForID(
		TB_STATE_PUSHUP3);

	if(window)
	{
		window->SetcbFunc(
			::OnActionEventRideOptionDialog);
	}
}