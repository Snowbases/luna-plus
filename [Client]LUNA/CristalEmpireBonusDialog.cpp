#include "StdAfx.h"
#include "CristalEmpireBonusDialog.h"
#include "WindowIDEnum.h"
#include "interface/cIconDialog.h"
#include "interface/cWindowManager.h"
#include "interface/cStatic.h"
#include "interface/cImage.h"
#include "ItemManager.h"
#include "Item.h"
#include "input/Mouse.h"
#include "cMsgBox.h"
#include "cScriptManager.h"
#include "ChatManager.h"
#include "ProgressDialog.h"
#include "CommonCalcFunc.h"

const int backgroundIconImageIndex = 1441;

void OnActionEvent(LONG windowIndex, LPVOID eventThrower, DWORD windowEvent)
{
	CCristalEmpireBonusDialog* const dialog = (CCristalEmpireBonusDialog*)WINDOWMGR->GetWindowForID(
		CI_BGSTATIC_0);

	if(0 == dialog)
	{
		return;
	}
	
	dialog->OnActionEvent(
		windowIndex,
		eventThrower,
		windowEvent);
}

void PressYesInMessageBox(LONG, LPVOID, DWORD windowEvent)
{
	CCristalEmpireBonusDialog* const dialog = (CCristalEmpireBonusDialog*)WINDOWMGR->GetWindowForID(
		CI_BGSTATIC_0);

	if(0 == dialog)
	{
		return;
	}
	else if(MBI_NO == windowEvent)
	{
		dialog->SetDisable(
			FALSE);
		return;
	}

	CProgressDialog* const progressDialog = (CProgressDialog*)WINDOWMGR->GetWindowForID(
		PROGRESS_DIALOG);

	if(0 == progressDialog)
	{
		return;
	}
	
	const DWORD waitSecond = 5;

	progressDialog->Wait(
		waitSecond,
		CHATMGR->GetChatMsg(1074),
		::OnActionEvent);
	progressDialog->SetActive(
		TRUE);
}

void PressNoInMessageBox(LONG, LPVOID, DWORD windowEvent)
{
	CCristalEmpireBonusDialog* const dialog = (CCristalEmpireBonusDialog*)WINDOWMGR->GetWindowForID(
		CI_BGSTATIC_0);

	if(0 == dialog)
	{
		return;
	}

	dialog->SetDisable(
		FALSE);

	if(MBI_NO == windowEvent)
	{
		return;
	}

	dialog->SetActive(
		FALSE);
}

CCristalEmpireBonusDialog::CCristalEmpireBonusDialog() :
mTargetIconDialog(0),
mListIconDialog(0),
mResultStatic(0),
mSubmitButton(0)
{}

CCristalEmpireBonusDialog::~CCristalEmpireBonusDialog()
{
	Clear();
}

void CCristalEmpireBonusDialog::Clear()
{
	for(WORD i = (mTargetIconDialog ? mTargetIconDialog->GetCellNum() : 0);
		0 < i--;)
	{
		cIcon* icon = 0;
		mTargetIconDialog->DeleteIcon(
			i,
			&icon);

		if(0 == icon)
		{
			continue;
		}
		else if(WT_ITEM == icon->GetType())
		{
			CItem* const item = ITEMMGR->GetItem(
				((CItem*)icon)->GetDBIdx());

			if(item)
			{
				item->SetLock(
					FALSE);
			}
		}

		SAFE_DELETE(icon);
	}

	for(WORD i = (mListIconDialog ? mListIconDialog->GetCellNum() : 0);
		0 < i--;)
	{
		cIcon* icon = 0;
		mListIconDialog->DeleteIcon(
			i,
			&icon);
		SAFE_DELETE(icon);
	}
}

void CCristalEmpireBonusDialog::SetActive(BOOL isActive)
{
	if(FALSE == isActive)
	{
		Clear();
	}
	else
	{
		mSubmitButton->SetActive(
			FALSE);
	}

	cDialog::SetActive(
		isActive);
}

void CCristalEmpireBonusDialog::Linking()
{
	SetcbFunc(
		::OnActionEvent);

	mTargetIconDialog = (cIconDialog*)GetWindowForID(
		CI_BGSTATIC_2);
	mTargetIconDialog->SetcbFunc(
		::OnActionEvent);

	mListIconDialog = (cIconDialog*)GetWindowForID(
		CI_CIRCLE_IMAGE_0);
	mListIconDialog->SetcbFunc(
		::OnActionEvent);

	mResultStatic = (cStatic*)GetWindowForID(
		CI_BGSTATIC_1);

	mSubmitButton = GetWindowForID(
		CI_BGSTATIC_3);
	mSubmitButton->SetcbFunc(
		::OnActionEvent);

	cWindow* const closeButton = GetWindowForID(
		CI_BGSTATIC_5);

	if(closeButton)
	{
		closeButton->SetcbFunc(
			::OnActionEvent);
	}
}

void CCristalEmpireBonusDialog::Ready(const MSG_ITEM_OPTION& message)
{
	if(0 == mTargetIconDialog)
	{
		Linking();
	}

	mResultStatic->SetStaticText(
		CHATMGR->GetChatMsg(2026));

	if(message.mSize > mListIconDialog->GetCellNum())
	{
		return;
	}

	for(WORD i = 0; i < message.mSize; ++i)
	{
		const ITEM_OPTION& itemOption = message.mOption[i];
		const DWORD itemIndex = itemOption.mEnchant.mIndex;
		const float itemLevel = itemOption.mEnchant.mLevel;
		
		cIcon* const icon = new cIcon;
		icon->SetData(
			itemIndex);
		icon->SetMovable(
			FALSE);

		cImage image;
		icon->Init(
			0,
			0,
			DEFAULT_ICONSIZE,
			DEFAULT_ICONSIZE,
			ITEMMGR->GetIconImage(itemIndex, &image));
		// 100111 LUJ, 최대 가능 회수를 설정한다
		icon->SetIconType(
			DWORD(itemLevel));
		mListIconDialog->AddIcon(
			i,
			icon);
		ITEMMGR->AddToolTip(
			icon);
		icon->AddToolTipLine(
			_T(""));
		icon->AddToolTipLine(
			CHATMGR->GetChatMsg(2027));

		const size_t dropOptionSize = sizeof(itemOption.mDrop.mValue) / sizeof(*itemOption.mDrop.mValue);

		for(size_t i = 0; i < dropOptionSize; ++i)
		{
			const ITEM_OPTION::Drop::Value& value = itemOption.mDrop.mValue[i];

			if(0 == value.mValue)
			{
				continue;
			}

			switch(value.mKey)
			{
			case ITEM_OPTION::Drop::KeyPercentStrength:	
			case ITEM_OPTION::Drop::KeyPercentIntelligence:	
			case ITEM_OPTION::Drop::KeyPercentDexterity:	
			case ITEM_OPTION::Drop::KeyPercentWisdom:	
			case ITEM_OPTION::Drop::KeyPercentVitality:	
			case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:	
			case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:	
			case ITEM_OPTION::Drop::KeyPercentMagicalAttack:	
			case ITEM_OPTION::Drop::KeyPercentMagicalDefence:	
			case ITEM_OPTION::Drop::KeyPercentCriticalRate:	
			case ITEM_OPTION::Drop::KeyPercentCriticalDamage:	
			case ITEM_OPTION::Drop::KeyPercentAccuracy:	
			case ITEM_OPTION::Drop::KeyPercentEvade:	
			case ITEM_OPTION::Drop::KeyPercentMoveSpeed:	
			case ITEM_OPTION::Drop::KeyPercentLife:	
			case ITEM_OPTION::Drop::KeyPercentMana:	
			case ITEM_OPTION::Drop::KeyPercentLifeRecovery:	
			case ITEM_OPTION::Drop::KeyPercentManaRecovery:	
				{
					TCHAR buffer[MAX_PATH] = {0};
					_stprintf(
						buffer,
						_T("   %s (max: %0.0f%%) "),
						ITEMMGR->GetName(value.mKey),
						GetItemDungeonBonus(itemLevel, itemLevel, value.mValue) * 100);
					icon->AddToolTipLine(
						buffer,
						RGB_HALF(57, 187, 225));
					break;
				}	
			case ITEM_OPTION::Drop::KeyPlusStrength:	
			case ITEM_OPTION::Drop::KeyPlusIntelligence:	
			case ITEM_OPTION::Drop::KeyPlusDexterity:	
			case ITEM_OPTION::Drop::KeyPlusWisdom:	
			case ITEM_OPTION::Drop::KeyPlusVitality:	
			case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:	
			case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:	
			case ITEM_OPTION::Drop::KeyPlusMagicalAttack:	
			case ITEM_OPTION::Drop::KeyPlusMagicalDefence:	
			case ITEM_OPTION::Drop::KeyPlusCriticalRate:	
			case ITEM_OPTION::Drop::KeyPlusCriticalDamage:	
			case ITEM_OPTION::Drop::KeyPlusAccuracy:	
			case ITEM_OPTION::Drop::KeyPlusEvade:	
			case ITEM_OPTION::Drop::KeyPlusMoveSpeed:	
			case ITEM_OPTION::Drop::KeyPlusLife:	
			case ITEM_OPTION::Drop::KeyPlusMana:	
			case ITEM_OPTION::Drop::KeyPlusLifeRecovery:	
			case ITEM_OPTION::Drop::KeyPlusManaRecovery:	
				{
					TCHAR buffer[MAX_PATH] = {0};
					_stprintf(
						buffer,
						_T("   %s (max: %0.0f) "),
						ITEMMGR->GetName(value.mKey),
						GetItemDungeonBonus(itemLevel, itemLevel, value.mValue));
					icon->AddToolTipLine(
						buffer,
						RGB_HALF(57, 187, 225));
					break;	
				}
			}
		}
	}

	const WORD itemSize = WORD(message.mOption[0].mItemDbIndex);

	for(WORD i = min(itemSize, mTargetIconDialog->GetCellNum());
		0 < i--;)
	{
		cImage image;
		SCRIPTMGR->GetImage(
			backgroundIconImageIndex,
			&image,
			PFT_ITEMPATH);

		ITEMBASE itemBase;
		ZeroMemory(
			&itemBase,
			sizeof(itemBase));
		CItem* const item = new CItem(&itemBase);

		if(0 == item)
		{
			continue;
		}

		item->Init(
			0,
			0,
			DEFAULT_ICONSIZE,
			DEFAULT_ICONSIZE,
			&image);
		item->SetMovable(
			FALSE);

		SCRIPTMGR->GetImage(
			0,
			&image,
			PFT_HARDPATH);
		item->SetToolTip(
			CHATMGR->GetChatMsg(523),
			RGBA_MAKE(255, 255, 255, 255),
			&image,
			TTCLR_ITEM_CANEQUIP);

		mTargetIconDialog->AddIcon(
			i,
			item);
	}

	Stick(
		IN_INVENTORYDLG);
}

BOOL CCristalEmpireBonusDialog::FakeMoveIcon(LONG x, LONG y, cIcon* draggedIcon)
{
	CItem* const draggedItem = (CItem*)draggedIcon;

	if(0 == draggedItem)
	{
		return FALSE;
	}
	else if(WT_ITEM != draggedItem->GetType())
	{
		return FALSE;
	}
	else if(FALSE == ITEMMGR->IsEqualTableIdxForPos(
		eItemTable_Inventory,
		draggedItem->GetPosition()))
	{
		return FALSE;
	}

	CItem* item = 0;

	enum Error
	{
		ErrorNone,
		ErrorNotFound,
		ErrorLevelIsFull,		
	}
	error = ErrorNone;
	DWORD itemMaxLevel = 0;

	for(WORD listIconIndex = 0; listIconIndex < mListIconDialog->GetCellNum(); ++listIconIndex)
	{
		cIcon* const icon = mListIconDialog->GetIconForIdx(
			listIconIndex);

		if(0 == icon)
		{
			continue;
		}
		else if(draggedItem->GetItemIdx() != icon->GetData())
		{
			error = ErrorNotFound;
			continue;
		}
		else if(draggedItem->GetItemParam() >= icon->GetIconType())
		{
			error = ErrorLevelIsFull;
			break;
		}

		item = (CItem*)GetIcon(
			*mTargetIconDialog,
			x,
			y);
		itemMaxLevel = icon->GetIconType();
		break;
	}

	switch(error)
	{
	case ErrorLevelIsFull:
		{
			mResultStatic->SetStaticText(
				CHATMGR->GetChatMsg(2024));
			break;
		}
	case ErrorNotFound:
		{
			mResultStatic->SetStaticText(
				CHATMGR->GetChatMsg(2023));
			break;
		}
	}

	if(0 == item)
	{	
		return FALSE;
	}
	else if(WT_ITEM != item->GetType())
	{
		return FALSE;
	}

	CItem* const previousItem = ITEMMGR->GetItem(
		item->GetDBIdx());

	if(previousItem)
	{
		previousItem->SetLock(
			FALSE);
	}

	cImage image;
	item->Init(
		LONG(item->GetAbsX()),
		LONG(item->GetAbsY()),
		DEFAULT_ICONSIZE,
		DEFAULT_ICONSIZE,
		ITEMMGR->GetIconImage(draggedItem->GetItemIdx(), &image));
	item->SetItemBaseInfo(
		draggedItem->GetItemBaseInfo());
	item->SetData(
		itemMaxLevel);
	
	draggedItem->SetLock(
		TRUE);
	ITEMMGR->AddToolTip(
		item);
	
	TCHAR buffer[MAX_PATH] = {0};
	_stprintf(
		buffer,
		_T("%s %d/%d"),
		CHATMGR->GetChatMsg(2025),
		item->GetItemParam(),
		itemMaxLevel);
	item->AddToolTipLine(
		_T(""));
	item->AddToolTipLine(
		buffer);

	mSubmitButton->SetActive(
		TRUE);
	mResultStatic->SetStaticText(
		CHATMGR->GetChatMsg(2026));
	return FALSE;
}

DWORD CCristalEmpireBonusDialog::ActionEvent(CMouse* mouse)
{
	const DWORD windowEvent = cDialog::ActionEvent(
		mouse);

	if(WE_LBTNCLICK & windowEvent)
	{
		CItem* const item = (CItem*)GetIcon(
			*mTargetIconDialog,
			mouse->GetMouseX(),
			mouse->GetMouseY());

		if(0 == item)
		{
			return windowEvent;
		}
		else if(WT_ITEM != item->GetType())
		{
			return windowEvent;
		}

		CItem* const previousItem = ITEMMGR->GetItem(
			item->GetDBIdx());

		if(previousItem)
		{
			previousItem->SetLock(
				FALSE);
		}
		
		cImage image;
		SCRIPTMGR->GetImage(
			backgroundIconImageIndex,
			&image,
			PFT_ITEMPATH);
		item->Init(
			LONG(item->GetAbsX()),
			LONG(item->GetAbsY()),
			DEFAULT_ICONSIZE,
			DEFAULT_ICONSIZE,
			&image);
		ITEMBASE itemBase;
		ZeroMemory(
			&itemBase,
			sizeof(itemBase));
		item->SetItemBaseInfo(
			itemBase);
		
		SCRIPTMGR->GetImage(
			0,
			&image,
			PFT_HARDPATH);
		item->SetToolTip(
			CHATMGR->GetChatMsg(523),
			RGBA_MAKE(255, 255, 255, 255),
			&image,
			TTCLR_ITEM_CANEQUIP);

		BOOL isNoItem = TRUE;

		for(WORD i = 0; i < mTargetIconDialog->GetCellNum(); ++i)
		{
			CItem* const targetItem = (CItem*)mTargetIconDialog->GetIconForIdx(i);

			if(0 == targetItem)
			{
				continue;
			}
			else if(WT_ITEM != targetItem->GetType())
			{
				continue;
			}
			else if(0 < targetItem->GetItemIdx())
			{
				isNoItem = FALSE;
				break;
			}
		}

		if(isNoItem)
		{
			mSubmitButton->SetActive(
				FALSE);
		}
	}

	return windowEvent;
}

void CCristalEmpireBonusDialog::OnActionEvent(LONG windowIndex, LPVOID windowControl, DWORD windowEvent)
{
	switch(windowIndex)
	{
	case PROGRESS_TEXT_CANCELBTN:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID(
				PROGRESS_DIALOG);

			if(0 == dialog)
			{
				break;
			}

			dialog->SetActive(
				FALSE);
			SetDisable(
				FALSE);
			break;
		}
	case PROGRESS_DIALOG:
		{
			Request();
			SetDisable(
				FALSE);
			break;
		}
	case CI_BGSTATIC_3:
		{
			cMsgBox* const messageBox = WINDOWMGR->MsgBox(
				MBI_NOTICE,
				MBT_YESNO,
				CHATMGR->GetChatMsg(2029));

			if(0 == messageBox)
			{
				break;
			}

			messageBox->SetMBFunc(
				PressYesInMessageBox);
			SetDisable(
				TRUE);
			break;
		}
	case CI_BGSTATIC_5:
		{
			if(mTargetIconDialog->IsDisable())
			{
				SetActive(
					FALSE);
				break;
			}

			cMsgBox* const messageBox = WINDOWMGR->MsgBox(
				MBI_NOTICE,
				MBT_YESNO,
				CHATMGR->GetChatMsg(2028));

			if(0 == messageBox)
			{
				break;
			}

			messageBox->SetMBFunc(
				PressNoInMessageBox);
			SetDisable(
				TRUE);
			break;
		}
	}
}

void CCristalEmpireBonusDialog::Request()
{
	MSG_ITEM_MIX_ACK message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_TRIGGER;
	message.Protocol = MP_TRIGGER_ITEM_OPTION_APPLY_SYN;
	message.dwObjectID = gHeroID;

	for(WORD i = 0; i < mTargetIconDialog->GetCellNum(); ++i)
	{
		CItem* const item = (CItem*)mTargetIconDialog->GetIconForIdx(i);

		if(0 == item)
		{
			continue;
		}
		else if(WT_ITEM != item->GetType())
		{
			continue;
		}
		else if(0 == item->GetDBIdx())
		{
			continue;
		}
		else if(message.mUpdateResult.mSize > _countof(message.mUpdateResult.mItem))
		{
			break;
		}

		ITEMBASE& itemBase = message.mUpdateResult.mItem[message.mUpdateResult.mSize++];
		itemBase.wIconIdx = item->GetItemIdx();
		itemBase.Position = item->GetPosition();
		itemBase.dwDBIdx = item->GetDBIdx();		
	}

	if(0 == message.mUpdateResult.mSize)
	{
		mResultStatic->SetStaticText(
			_T("You must put an item on slots"));
		return;
	}

	NETWORK->Send(
		&message,
		message.GetSize());
}

cIcon* CCristalEmpireBonusDialog::GetIcon(cIconDialog& dialog, LONG x, LONG y) const
{
	for(WORD i = 0; i < dialog.GetCellNum(); ++i)
	{
		cIcon* const icon = dialog.GetIconForIdx(i);

		if(0 == icon)
		{
			continue;
		}
		else if(icon->IsDisable())
		{
			continue;
		}
		else if(icon->GetAbsX() > x)
		{
			continue;
		}
		else if(icon->GetAbsY() > y)
		{
			continue;
		}
		else if(icon->GetAbsX() + icon->GetWidth() < x)
		{
			continue;
		}
		else if(icon->GetAbsY() + icon->GetHeight() < y)
		{
			continue;
		}

		return icon;        
	}

	return 0;
}

void CCristalEmpireBonusDialog::Finish(const MSG_ITEM_OPTION& message)
{
	for(const ITEM_OPTION* itemOption = message.mOption + message.mSize;
		message.mOption != itemOption--;)
	{
		ITEMMGR->AddOption(
			*itemOption);

		CItem* const item = ITEMMGR->GetItem(
			itemOption->mItemDbIndex);

		if(0 == item)
		{
			continue;
		}

		item->SetItemParam(
			item->GetItemParam() + 1);
		ITEMMGR->AddToolTip(
			item);
	}

	for(WORD i = 0; i < mTargetIconDialog->GetCellNum(); ++i)
	{
		CItem* const item = (CItem*)mTargetIconDialog->GetIconForIdx(i);

		if(0 == item)
		{
			continue;
		}
		else if(WT_ITEM != item->GetType())
		{
			continue;
		}
		else if(0 == item->GetDBIdx())
		{
			item->SetLock(
				TRUE);
			item->SetToolTip(
				_T(""));
			continue;
		}

		ITEMMGR->AddToolTip(
			item);
		item->SetDisable(
			TRUE);

		TCHAR buffer[MAX_PATH] = {0};
		_stprintf(
			buffer,
			_T("%s %d/%d"),
			CHATMGR->GetChatMsg(2025),
			item->GetItemParam() + 1,
			item->GetData());
		item->AddToolTipLine(
			_T(""));
		item->AddToolTipLine(
			buffer);
	}

	mSubmitButton->SetActive(
		FALSE);
	mTargetIconDialog->SetDisable(
		TRUE);
	mResultStatic->SetStaticText(
		CHATMGR->GetChatMsg(2022));
}