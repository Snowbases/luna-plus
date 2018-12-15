#include "stdafx.h"
#include "WindowIDEnum.h"
#include "Interface/cScriptManager.h"
#include "ItemManager.h"
#include "interface/cResourceManager.h"
#include "Interface/cIconDialog.h"
#include "Interface/cIconGridDialog.h"
#include "interface/cPushupButton.h"
#include "InventoryExDialog.h"
#include "ChatManager.h"
#include "cWindowManager.h"
#include "Item.h"
#include "ObjectManager.h"
#include "input/Mouse.h"
#include "DissolveDialog.h"
#include "FishingDialog.h"
#include "FishingPointDialog.h"
#include "GameIn.h"

typedef DWORD ItemIndex;
typedef int ItemQuantity;
typedef std::map< ItemIndex, ItemQuantity > ResultContainer;
typedef std::set< ItemIndex > RandomResultContainer;
RandomResultContainer randomAllResultContainer;

void GetDissolvedResult(const ITEMBASE& sourceItem, ResultContainer& resultContainer)
{
	const DissolveScript* const script = ITEMMGR->GetDissolveScript(
		sourceItem.wIconIdx);

	if(0 == script)
	{
		return;
	}

	const size_t sourceSize = (ITEMMGR->IsDupItem(sourceItem.wIconIdx) ? sourceItem.Durability : 1);

	for(DissolveScript::StaticResult::const_iterator iterator = script->mStaticResult.begin();
		script->mStaticResult.end() != iterator;
		++iterator)
	{
		const DissolveScript::Result& result = *iterator;

		resultContainer[result.mItemIndex] += result.mQuantity * sourceSize;
	}
}

void GetDissolvedRandomResult(const ITEMBASE& sourceItem, ResultContainer& resultContainer)
{
	const DissolveScript* const script = ITEMMGR->GetDissolveScript(
		sourceItem.wIconIdx);

	if(0 == script)
	{
		return;
	}

	for(DissolveScript::DynamicResult::const_iterator iterator = script->mDynamicResult.begin();
		script->mDynamicResult.end() != iterator;
		++iterator)
	{
		const DissolveScript::Result& result = iterator->second;

		resultContainer[result.mItemIndex] += result.mQuantity;
	}
}

void OnActionEventDissolveDialog(LONG windowIndex, LPVOID eventThrower, DWORD windowEvent)
{
	cDialog* const dialog = WINDOWMGR->GetWindowForID(
		DIS_DISSOLUTIONDLG);

	if(0 == dialog)
	{
		return;
	}

	dialog->OnActionEvent(
		windowIndex,
		eventThrower,
		windowEvent);
}

CDissolveDialog::CDissolveDialog() :
mSourceDialog(0),
mRandomResultDialog(0),
mIsEffectStart(FALSE)
{}

CDissolveDialog::~CDissolveDialog()
{}

void CDissolveDialog::Clear()
{
	if(mSourceDialog)
	{
		for(WORD cellIndex = 0; mSourceDialog->GetCellNum() > cellIndex; ++cellIndex)
		{
			CItem* const item = (CItem*)mSourceDialog->GetIconForIdx(
				cellIndex);
			CItem* const inventoryItem = ITEMMGR->GetItem(
				item->GetDBIdx());

			if(inventoryItem)
			{
				inventoryItem->SetLock(
					FALSE);
			}

			Clear(
				*item);
		}
	}

	ClearResult();
	mIsEffectStart = FALSE;
}

void CDissolveDialog::Clear(CItem& item) const
{
	cImage image;
	ITEMBASE itemBase;
	ZeroMemory(
		&itemBase,
		sizeof(itemBase));
	item.SetItemBaseInfo(
		itemBase);
	item.SetBasicImage(
		&image);
	item.SetToolTip(
		"");
	item.SetLock(
		FALSE);
	item.SetMovable(
		FALSE);
	item.SetAlpha(
		255);
	item.ClearStateImage();
}

void CDissolveDialog::ClearResult()
{
	for(BYTE tabIndex = 0; tabIndex < GetTabNum(); ++tabIndex)
	{
		cPushupButton* const button = GetTabBtn(
			tabIndex);
		button->SetActive(
			FALSE);

		cIconGridDialog* const iconGridDialog = (cIconGridDialog*)GetTabSheet(
			tabIndex);

		for(WORD cellIndex = 0; cellIndex < iconGridDialog->GetCellNum(); ++cellIndex)
		{
			CItem* const item = (CItem*)iconGridDialog->GetIconForIdx(
				cellIndex);

			if(0 == item)
			{
				continue;
			}

			Clear(
				*item);
		}
	}

	for(WORD cellIndex = 0; mRandomResultDialog->GetCellNum() > cellIndex; ++cellIndex)
	{
		CItem* const item = (CItem*)mRandomResultDialog->GetIconForIdx(
			cellIndex);

		Clear(
			*item);
	}
}

void CDissolveDialog::Linking()
{
	if(mSourceDialog)
	{
		return;
	}

	SetcbFunc(
		::OnActionEventDissolveDialog);

	cDialog* const dialog = WINDOWMGR->GetWindowForID(
		IN_INVENTORYDLG);

	if(dialog)
	{
		dialog->Stick(GetID());
	}

	mSourceDialog = (cIconDialog*)GetWindowForID(
		DIS_BASEICONDLG);

	if(0 == mSourceDialog)
	{
		return;
	}

	for(WORD cellIndex = 0; mSourceDialog->GetCellNum() > cellIndex; ++cellIndex)
	{
		if(FALSE == mSourceDialog->IsAddable(cellIndex))
		{
			continue;
		}

		ITEMBASE itemBase;
		ZeroMemory(
			&itemBase,
			sizeof(itemBase));
		CItem* const item = new CItem(
			&itemBase);

		if(0 == item)
		{
			continue;
		}

		cImage image;
		item->Init(
			0,
			0,
			DEFAULT_ICONSIZE,
			DEFAULT_ICONSIZE,
			&image);
		item->SetData(
			cellIndex);
		
		mSourceDialog->AddIcon(
			cellIndex,
			item);
		WINDOWMGR->AddWindow(
			item);
	}

	mRandomResultDialog = (cIconDialog*)GetWindowForID(
		TB_STATE_AG);

	if(0 == mRandomResultDialog)
	{
		return;
	}

	for(WORD cellIndex = 0; mRandomResultDialog->GetCellNum() > cellIndex; ++cellIndex)
	{
		if(FALSE == mRandomResultDialog->IsAddable(cellIndex))
		{
			continue;
		}

		ITEMBASE itemBase;
		ZeroMemory(
			&itemBase,
			sizeof(itemBase));
		CItem* const item = new CItem(
			&itemBase);

		if(0 == item)
		{
			continue;
		}

		cImage image;
		item->Init(
			0,
			0,
			DEFAULT_ICONSIZE,
			DEFAULT_ICONSIZE,
			&image);

		mRandomResultDialog->AddIcon(
			cellIndex,
			item);
		WINDOWMGR->AddWindow(
			item);
	}

	for(BYTE tabIndex = 0; GetTabNum() > tabIndex; ++tabIndex)
	{
		cPushupButton* const button = GetTabBtn(
			tabIndex);
		button->SetActive(
			FALSE);

		cIconGridDialog* const iconGridDialog = (cIconGridDialog*)GetTabSheet(
			tabIndex);

		for(WORD cellIndex = 0; cellIndex < iconGridDialog->GetCellNum(); ++cellIndex)
		{
			if(FALSE == iconGridDialog->IsAddable(cellIndex))
			{
				continue;
			}

			ITEMBASE itemBase;
			ZeroMemory(
				&itemBase,
				sizeof(itemBase));
			CItem* const item = new CItem(
				&itemBase);

			if(0 == item)
			{
				continue;
			}

			cImage image;
			item->Init(
				0,
				0,
				DEFAULT_ICONSIZE,
				DEFAULT_ICONSIZE,
				&image);

			iconGridDialog->AddIcon(
				cellIndex,
				item);
			WINDOWMGR->AddWindow(
				item);
		}
	}

	mSubmitButton = GetWindowForID(
		DIS_OKBTN);
	mSubmitButton->SetcbFunc(
		::OnActionEventDissolveDialog);

	cWindow* const cancelWidow = GetWindowForID(
		DIS_CANCELBTN);

	if(cancelWidow)
	{
		cancelWidow->SetcbFunc(
			::OnActionEventDissolveDialog);
	}
}

void CDissolveDialog::Render()
{
	cTabDialog::RenderWindow();
	cTabDialog::RenderComponent();
	cTabDialog::RenderTabComponent();

	PutEffect();
}

DWORD CDissolveDialog::ActionEvent(CMouse* mouse)
{
	const DWORD windowEvent = cTabDialog::ActionEvent(
		mouse);

	if(FALSE == (WE_LBTNCLICK & windowEvent))
	{
		return windowEvent;
	}

	const POINT point = {
		mouse->GetMouseX(),
		mouse->GetMouseY()
	};
	CItem* const item = GetSourceItem(
		point);

	if(0 == item)
	{
		return windowEvent;
	}
	else if(item->IsLocked())
	{
		FakeMoveIcon(
			0,
			0,
			0);
	}

	CItem* const inventoryItem = ITEMMGR->GetItem(
		item->GetDBIdx());

	if(inventoryItem)
	{
		inventoryItem->SetLock(
			FALSE);
	}

	RemoveSource(
		item->GetItemBaseInfo(),
		POSTYPE(item->GetData()));
	RemoveResult(
		item->GetItemBaseInfo());
	Clear(
		*item);

	return windowEvent;
}

void CDissolveDialog::OnActionEvent(LONG id, void * p, DWORD we)
{
	switch(id)
	{
	case DIS_OKBTN:
		{
			PutError(
				Submit(),
				0);
			break;
		}
	case DIS_CANCELBTN:
		{
			Clear();
			break;
		}
	}
}

CDissolveDialog::Error CDissolveDialog::Submit()
{
	CInventoryExDialog* const inventoryDialog = (CInventoryExDialog*)WINDOWMGR->GetWindowForID(
		IN_INVENTORYDLG);

	if(0 == inventoryDialog)
	{
		return ErrorIsNoInventory;
	}

	MSG_ITEM_DISSOLVE_SYN message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_ITEM;
	message.Protocol = MP_ITEM_DISSOLVE_SYN;
	message.dwObjectID = gHeroID;

	typedef std::set< POSTYPE > ReservedPositionContainer;
	ReservedPositionContainer reservedPositionContainer;

	// 재료 취합
	for(WORD cellIndex = 0; mSourceDialog->GetCellNum() > cellIndex; ++cellIndex)
	{
		CItem* const sourceItem = (CItem*)mSourceDialog->GetIconForIdx(
			cellIndex);
		CItem* const inventoryItem = ITEMMGR->GetItem(
			sourceItem->GetDBIdx());

		if(0 == inventoryItem)
		{
			continue;
		}
		else if(ITEMMGR->IsDupItem(inventoryItem->GetItemIdx()))
		{
			if(sourceItem->GetDurability() != inventoryItem->GetDurability())
			{
				const ITEMBASE sourceItemBase = inventoryItem->GetItemBaseInfo();

				RemoveSource(
					sourceItemBase,
					cellIndex);
				RemoveResult(
					sourceItemBase);
				return ErrorInInvalidSource;
			}
		}

		MSG_ITEM_DISSOLVE_SYN::Item item;
		ZeroMemory(
			&item,
			sizeof(item));
		item.mType = MSG_ITEM_DISSOLVE_SYN::Item::TypeSource;
		item.mIndex = inventoryItem->GetItemIdx();
		item.mDbIndex = inventoryItem->GetDBIdx();
		item.mQuantity = (ITEMMGR->IsDupItem(inventoryItem->GetItemIdx()) ? inventoryItem->GetDurability() : 1);
		item.mPosition = inventoryItem->GetPosition();

		if(FALSE == message.Add(item))
		{
			return ErrorInsufficientInventory;
		}

		reservedPositionContainer.insert(
			inventoryItem->GetPosition());
	}

	ResultContainer resultContainer;

	// 결과 취합
	for(BYTE tabIndex = 0; tabIndex < GetTabNum(); ++tabIndex)
	{
		cPushupButton* const button = GetTabBtn(
			tabIndex);

		if(FALSE == button->IsActive())
		{
			break;
		}

		cIconGridDialog* const iconGridDialog = (cIconGridDialog*)GetTabSheet(
			tabIndex);

		for(WORD cellIndex = 0; iconGridDialog->GetCellNum() > cellIndex; ++cellIndex)
		{
			CItem* const item = (CItem*)iconGridDialog->GetIconForIdx(
				cellIndex);

			if(0 == item->GetItemIdx())
			{
				break;
			}

			resultContainer[item->GetItemIdx()] += item->GetDurability();
		}
	}

	// 랜덤 결과 취합
	for(WORD cellIndex = 0; mRandomResultDialog->GetCellNum() > cellIndex; ++cellIndex)
	{
		CItem* const item = (CItem*)mRandomResultDialog->GetIconForIdx(
			cellIndex);

		if(0 == item->GetItemIdx())
		{
			break;
		}

		resultContainer[item->GetItemIdx()] += item->GetDurability();
	}

	// 중첩 처리 가능한 경우 최대한 시도한다
	for(ResultContainer::iterator iterator = resultContainer.begin();
		resultContainer.end() != iterator;
		++iterator)
	{
		const ItemIndex resultItemIndex = iterator->first;
		ItemQuantity& resultItemQuantity = iterator->second;

		if(FALSE == ITEMMGR->IsDupItem(resultItemIndex))
		{
			continue;
		}
		else if(0 >= resultItemQuantity)
		{
			continue;
		}

		for(POSTYPE inventoryCellIndex = TP_INVENTORY_START;
			inventoryCellIndex < TP_INVENTORY_END + TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount();
			++inventoryCellIndex)
		{
			if(0 >= resultItemQuantity)
			{
				break;
			}
			else if(reservedPositionContainer.end() != reservedPositionContainer.find(inventoryCellIndex))
			{
				continue;
			}

			CItem* const item = inventoryDialog->GetItemForPos(
				inventoryCellIndex);
			const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(
				item ? item->GetItemIdx() : 0);

			if(0 == itemInfo)
			{
				continue;
			}
			else if(itemInfo->ItemIdx != resultItemIndex)
			{
				continue;
			}
			else if(itemInfo->Stack <= item->GetDurability())
			{
				continue;
			}
			else if(itemInfo->wSeal != item->GetItemBaseInfo().nSealed)
			{
				continue;
			}

			MSG_ITEM_DISSOLVE_SYN::Item resultItem;
			ZeroMemory(
				&resultItem,
				sizeof(resultItem));
			resultItem.mType = MSG_ITEM_DISSOLVE_SYN::Item::TypeResult;
			resultItem.mPosition = item->GetPosition();
			resultItem.mIndex = item->GetItemIdx();
			resultItem.mDbIndex = item->GetDBIdx();
			resultItem.mQuantity = min(
				itemInfo->Stack - item->GetDurability(),
				DURTYPE(resultItemQuantity));

			if(FALSE == message.Add(resultItem))
			{
				return ErrorInsufficientInventory;
			}

			resultItemQuantity -= resultItem.mQuantity;
		}
	}

	// 결과를 인벤토리 빈 칸에 채워나간다
	for(POSTYPE inventoryCellIndex = TP_INVENTORY_START;
		inventoryCellIndex < TP_INVENTORY_END + TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount();)
	{
		if(resultContainer.empty())
		{
			break;
		}

		const ResultContainer::iterator resultIterator = resultContainer.begin();
		const ItemIndex resultItemIndex = resultIterator->first;
		ItemQuantity& resultItemQuantity = resultIterator->second;

		if(0 >= resultItemQuantity)
		{
			resultContainer.erase(
				resultIterator);
			continue;
		}

		CItem* const item = inventoryDialog->GetItemForPos(
			inventoryCellIndex);

		if(0 < item)
		{
			// 재료가 위치한 슬롯은 사용 가능하므로 검사한다
			if(reservedPositionContainer.end() == reservedPositionContainer.find(item->GetPosition()))
			{
				++inventoryCellIndex;
				continue;
			}
		}

		const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(
			resultItemIndex);

		if(0 == itemInfo)
		{
			++inventoryCellIndex;
			continue;
		}

		MSG_ITEM_DISSOLVE_SYN::Item resultItem;
		ZeroMemory(
			&resultItem,
			sizeof(resultItem));
		resultItem.mType = MSG_ITEM_DISSOLVE_SYN::Item::TypeResult;
		resultItem.mPosition = inventoryCellIndex;
		resultItem.mIndex = resultItemIndex;
		resultItem.mQuantity = min(
			itemInfo->Stack ? itemInfo->Stack : 1,
			resultItemQuantity);

		if(FALSE == message.Add(resultItem))
		{
			return ErrorInsufficientInventory;
		}

        resultItemQuantity -= resultItem.mQuantity;
		++inventoryCellIndex;
	}

	NETWORK->Send(
		&message,
		message.GetSize());
	return ErrorNone;
}

BOOL CDissolveDialog::FakeMoveIcon(LONG x, LONG y, cIcon* movedIcon)
{
	// 완료된 결과가 있을 경우 초기화시킨다
	{
		CItem* const item = (CItem*)mSourceDialog->GetIconForIdx(
			0);

		if(item->IsLocked())
		{
			Clear();
		}
	}

	CItem* const movedItem = (CItem*)movedIcon;

	if(0 == movedItem)
	{
		return FALSE;
	}
	else if(WT_ITEM != movedItem->GetType())
	{
		return FALSE;
	}
	else if(movedItem->IsLocked())
	{
		return FALSE;
	}
	else if(FALSE == ITEMMGR->IsEqualTableIdxForPos(eItemTable_Inventory, movedItem->GetPosition()))
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(787));
		return FALSE;
	}

	// 기존 아이템이 위치한 장소에 놓았을 경우 교체한다
	const POINT point = {x, y};
	CItem* const item = GetSourceItem(
		point);

	if(0 == item)
	{
		return FALSE;
	}
	else if(0 < item->GetItemIdx())
	{
		RemoveSource(
			item->GetItemBaseInfo(),
			POSTYPE(item->GetData()));
		RemoveResult(
			item->GetItemBaseInfo());
		Clear(
			*item);
	}

	const Error error = IsEnableUpdate(movedItem->GetItemBaseInfo());

	if(ErrorNone != error)
	{
		PutError(
			error,
			movedItem->GetItemIdx());
		return FALSE;
	}

	AddSource(
		movedItem->GetItemBaseInfo(),
		POSTYPE(item->GetData()));
	AddResult(
		movedItem->GetItemBaseInfo());

	mSubmitButton->SetActive(
		TRUE);
	return FALSE;
}

CDissolveDialog::Error CDissolveDialog::IsEnableUpdate(const ITEMBASE& sourceItem)
{
	const DissolveScript* const script = ITEMMGR->GetDissolveScript(
		sourceItem.wIconIdx);

	if(0 == script)
	{
		return ErrorNoScript;
	}

	CHero* const hero = OBJECTMGR->GetHero();

	if(0 == hero)
	{
		return ErrorNoHero;
	}
	else if(script->mLevel > hero->GetLevel())
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(819),
			script->mLevel);
		return ErrorInvalidLevel;
	}

	// 결과 창에 표시 가능한 재료가 모두 등록될 수 있는지 살펴본다
	{
		ResultContainer resultContainer;
		GetDissolvedRandomResult(
			sourceItem,
			resultContainer);
		
		for(WORD cellIndex = 0; mRandomResultDialog->GetCellNum() > cellIndex; ++cellIndex)
		{
			if(resultContainer.empty())
			{
				break;
			}

			const ResultContainer::const_iterator iterator = resultContainer.begin();
			const ItemIndex itemIndex = iterator->first;

			CItem* const item = (CItem*)mRandomResultDialog->GetIconForIdx(
				cellIndex);

			if(0 == item->GetItemIdx())
			{
				resultContainer.erase(
					itemIndex);
			}
			else if(item->GetItemIdx() == itemIndex)
			{
				resultContainer.erase(
					itemIndex);
			}
		}

		if(false == resultContainer.empty())
		{
			return ErrorInsufficientDynamicResult;
		}

		GetDissolvedResult(
			sourceItem,
			resultContainer);

		for(BYTE tabIndex = 0; tabIndex < GetTabNum(); ++tabIndex)
		{
			cIconGridDialog* const iconGridDialog = (cIconGridDialog*)GetTabSheet(
				tabIndex);

			for(WORD cellIndex = 0; cellIndex < iconGridDialog->GetCellNum(); ++cellIndex)
			{
				if(resultContainer.empty())
				{
					break;
				}

				const ResultContainer::const_iterator iterator = resultContainer.begin();
				const ItemIndex itemIndex = iterator->first;

				CItem* const item = (CItem*)iconGridDialog->GetIconForIdx(
					cellIndex);

				if(0 == item->GetItemIdx())
				{
					resultContainer.erase(
						itemIndex);
				}
				else if(item->GetItemIdx() == itemIndex)
				{
					resultContainer.erase(
						itemIndex);
				}
			}
		}

		if(false == resultContainer.empty())
		{
			return ErrorInsufficientResult;
		}
	}

	// 인벤토리에 결과가 들어갈 공간이 적당한지도 검사한다
	{
		ResultContainer resultContainer;
		GetDissolvedResult(
			sourceItem,
			resultContainer);
		ResultContainer randomResultContainer;
		GetDissolvedRandomResult(
			sourceItem,
			randomResultContainer);

		for(ResultContainer::const_iterator iterator = randomResultContainer.begin();
			randomResultContainer.end() != iterator;
			++iterator)
		{
			const ItemIndex itemIndex = iterator->first;
			const ItemQuantity itemQuantity = iterator->second;

			resultContainer[itemIndex] += itemQuantity;
		}

		CInventoryExDialog* const inventoryDialog = (CInventoryExDialog*)WINDOWMGR->GetWindowForID(
			IN_INVENTORYDLG);

		if(0 == inventoryDialog)
		{
			return ErrorIsNoInventory;
		}

		size_t inventorySlot = 0;

		for(POSTYPE position = TP_INVENTORY_START;
			position > TP_INVENTORY_END + TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount();
			++position)
		{
			CItem* const item = inventoryDialog->GetItemForPos(
				position);

			if(0 == item)
			{
				++inventorySlot;
				continue;
			}

			const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(
				item->GetItemIdx());

			if(0 == itemInfo)
			{
				continue;
			}
			else if(0 == itemInfo->Stack)
			{
				continue;
			}
			else if(resultContainer.end() == resultContainer.find(item->GetItemIdx()))
			{
				continue;
			}

			resultContainer[item->GetItemIdx()] -= (itemInfo->Stack - item->GetDurability());
		}

		size_t needSlot = 0;

		for(ResultContainer::const_iterator iterator = resultContainer.begin();
			resultContainer.end() != iterator;
			++iterator)
		{
			const ItemQuantity itemQuantity = iterator->second;

			if(0 >= itemQuantity)
			{
				continue;
			}

			const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(
				iterator->first);

			if(0 == itemInfo)
			{
				continue;
			}
			else if(0 == itemInfo->Stack)
			{
				needSlot += itemQuantity;
				continue;
			}

            needSlot += size_t(ceil(float(itemQuantity) / itemInfo->Stack));
		}

		if(inventorySlot > needSlot)
		{
			return ErrorInsufficientInventory;
		}
	}

	return ErrorNone;
}

void CDissolveDialog::AddSource(const ITEMBASE& itemBase, POSTYPE position)
{
	CItem* const item = (CItem*)mSourceDialog->GetIconForIdx(
		position);
	CItem* const inventoryItem = ITEMMGR->GetItem(
		itemBase.dwDBIdx);

	if(inventoryItem)
	{
		inventoryItem->SetLock(
			TRUE);
	}

	cImage image;
	item->SetBasicImage(
		ITEMMGR->GetIconImage(itemBase.wIconIdx, &image));
	item->SetItemBaseInfo(
		itemBase);
	ITEMMGR->AddToolTip(
		item);
}

void CDissolveDialog::RemoveSource(const ITEMBASE& itemBase, POSTYPE position)
{
	CItem* const item = (CItem*)mSourceDialog->GetIconForIdx(
		position);
	CItem* const inventoryItem = ITEMMGR->GetItem(
		itemBase.dwDBIdx);

	if(inventoryItem)
	{
		inventoryItem->SetLock(
			FALSE);
	}

	Clear(
		*item);
	
	{
		BOOL isEmptySource = TRUE;

		for(WORD cellIndex = 0; mSourceDialog->GetCellNum() > cellIndex; ++cellIndex)
		{
			CItem* const item = (CItem*)mSourceDialog->GetIconForIdx(
				cellIndex);

			if(0 < item->GetItemIdx())
			{
				isEmptySource = FALSE;
				break;
			}
		}

		if(isEmptySource)
		{
			mSubmitButton->SetActive(
				FALSE);
		}
	}
}

void CDissolveDialog::AddResult(const ITEMBASE& sourceItem)
{
	ResultContainer resultContainer;
	GetDissolvedResult(
		sourceItem,
		resultContainer);

	for(BYTE tabIndex = 0; tabIndex < GetTabNum(); ++tabIndex)
	{
		cIconGridDialog* const iconGridDialog = (cIconGridDialog*)GetTabSheet(
			tabIndex);

		for(WORD cellIndex = 0; iconGridDialog->GetCellNum() > cellIndex; ++cellIndex)
		{
			CItem* const item = (CItem*)iconGridDialog->GetIconForIdx(
				cellIndex);
			const ResultContainer::const_iterator iterator = resultContainer.find(
				item->GetItemIdx());

			if(resultContainer.end() != iterator)
			{
				const ItemIndex itemIndex = iterator->first;
				const ItemQuantity itemQuantity = iterator->second;

				item->SetDurability(
					item->GetDurability() + itemQuantity);

				resultContainer.erase(
					itemIndex);
			}
			else if(0 == item->GetItemIdx())
			{
				if(resultContainer.empty())
				{
					break;
				}

				const ResultContainer::const_iterator iterator = resultContainer.begin();
				const ItemIndex itemIndex = iterator->first;
				const ItemQuantity itemQuantity = iterator->second;

				cImage image;
				ITEMBASE itemBase;
				ZeroMemory(
					&itemBase,
					sizeof(itemBase));
				itemBase.wIconIdx = itemIndex;
				itemBase.Durability = itemQuantity;
				item->SetItemBaseInfo(
					itemBase);
				item->SetBasicImage(
					ITEMMGR->GetIconImage(itemBase.wIconIdx, &image));
				
				resultContainer.erase(
					itemIndex);
			}

			cPushupButton* const button = GetTabBtn(
				tabIndex);
			button->SetActive(
				TRUE);
		}
	}

	GetDissolvedRandomResult(
		sourceItem,
		resultContainer);

	for(WORD cellIndex = 0; mRandomResultDialog->GetCellNum() > cellIndex; ++cellIndex)
	{
		CItem* const item = (CItem*)mRandomResultDialog->GetIconForIdx(
			cellIndex);
		const ResultContainer::const_iterator iterator = resultContainer.find(
			item->GetItemIdx());

		if(resultContainer.end() != iterator)
		{
			const ItemIndex itemIndex = iterator->first;
			const ItemQuantity itemQuantity = iterator->second;

			item->SetDurability(
				item->GetDurability() + itemQuantity);
			
			resultContainer.erase(
				itemIndex);
		}
		else if(0 == item->GetItemIdx())
		{
			if(resultContainer.empty())
			{
				break;
			}

			const ResultContainer::const_iterator iterator = resultContainer.begin();
			const ItemIndex itemIndex = iterator->first;
			const ItemQuantity itemQuantity = iterator->second;

			cImage image;
			ITEMBASE itemBase;
			ZeroMemory(
				&itemBase,
				sizeof(itemBase));
			itemBase.wIconIdx = itemIndex;
			itemBase.Durability = itemQuantity;
			item->SetItemBaseInfo(
				itemBase);
			item->SetBasicImage(
				ITEMMGR->GetIconImage(itemBase.wIconIdx, &image));
			
			resultContainer.erase(
				itemIndex);
		}
	}

	PutTip();
}

void CDissolveDialog::RemoveResult(const ITEMBASE& sourceItem)
{
	ResultContainer resultContainer;
	GetDissolvedResult(
		sourceItem,
		resultContainer);

	ResultContainer remainedResultContainer;

	for(BYTE tabIndex = 0; tabIndex < GetTabNum(); ++tabIndex)
	{
		cPushupButton* const button = GetTabBtn(
			tabIndex);

		if(FALSE == button->IsActive())
		{
			break;
		}

		cIconGridDialog* const iconGridDialog = (cIconGridDialog*)GetTabSheet(
			tabIndex);

		for(WORD cellIndex = 0; cellIndex < iconGridDialog->GetCellNum(); ++cellIndex)
		{
			CItem* const item = (CItem*)iconGridDialog->GetIconForIdx(
				cellIndex);
			const ResultContainer::iterator iterator = resultContainer.find(
				item->GetItemIdx());

			if(resultContainer.end() == iterator)
			{
				continue;
			}

			const DURTYPE quantity = iterator->second;

			if(item->GetDurability() <= quantity)
			{
				Clear(
					*item);
			}
			else
			{
				item->SetDurability(
					item->GetDurability() - quantity);
			}
		}
	}

	ResultContainer randomResultContainer;
	GetDissolvedResult(
		sourceItem,
		randomResultContainer);

	// 랜덤 결과를 제거한다
	for(ResultContainer::const_iterator iterator = randomResultContainer.begin();
		randomResultContainer.end() != iterator;
		++iterator)
	{
		const ItemIndex itemIndex = iterator->first;
		const ItemQuantity itemQuantity = iterator->second;
		
		for(WORD cellIndex = 0; mRandomResultDialog->GetCellNum() > cellIndex; ++cellIndex)
		{
			CItem* const item = (CItem*)mRandomResultDialog->GetIconForIdx(
				cellIndex);

			if(item->GetItemIdx() != itemIndex)
			{
				continue;
			}
			else if(item->GetDurability() <= DURTYPE(itemQuantity))
			{
				Clear(
					*item);
				break;
			}

			item->SetDurability(
				item->GetDurability() - itemQuantity);
			break;
		}
	}

	// 결과를 재배치한다
	{
		ClearResult();

		for(WORD cellIndex = 0; mSourceDialog->GetCellNum() > cellIndex; ++cellIndex)
		{
			CItem* const item = (CItem*)mSourceDialog->GetIconForIdx(
				cellIndex);

			if(item->GetDBIdx() == sourceItem.dwDBIdx)
			{
				continue;
			}

			AddResult(
				item->GetItemBaseInfo());
		}
	}

	// 현재 탭이 숨겨질 경우, 그 이전 탭으로 변경해야한다
	{
		cPushupButton* const button = GetTabBtn(
			GetCurTabNum());

		if(FALSE == button->IsActive())
		{
			SelectTab(
				BYTE(max(int(GetCurTabNum()) - 1, 0)));
		}
	}
}

void CDissolveDialog::Add(cWindow* window)
{
	if(window->GetType() == WT_PUSHUPBUTTON)
		AddTabBtn(curIdx1++, (cPushupButton *)window);
	else if(window->GetType() == WT_ICONGRIDDIALOG)
	{
		AddTabSheet(curIdx2++, window);
		((cIconGridDialog*)window)->SetDragOverIconType(WT_ITEM);
	}
	else 
		cTabDialog::Add(window);
}

void CDissolveDialog::SetActive(BOOL isActive)
{
	cTabDialog::SetActive(
		isActive);

	Linking();

	// 다른 작업을 처리 중인 경우에는 창을 띄울 수 없다
	{
		cDialog* dialog = WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
		ASSERT( dialog );

		if( dialog->IsDisable() &&
			dialog->IsActive() )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
			return;
		}
	}

	if(isActive)
	{
		// NPC 상점을 연 상태에서도 불가. 
		// 판매 상점을 연 상태에서 열 수 없음: 아이템이 겹쳐지나, 아이템의 잠금 처리가 클라이언트에 저장되지 않으므로 원천적으로 아이템에 대한
		// 동시 접근 시도를 차단해야 한다.
		// 거래 중/창고 사용일 때는 불가함. 위와 마찬가지 이유
		{
			cDialog* npcScriptDialog			= WINDOWMGR->GetWindowForID( NSI_SCRIPTDLG );
			cDialog* npcShopDialog				= WINDOWMGR->GetWindowForID( DE_DEALERDLG );
			cDialog* stallShopDialog			= WINDOWMGR->GetWindowForID( SSI_STALLDLG );
			cDialog* exchangeDialog				= WINDOWMGR->GetWindowForID( XCI_DIALOG );
			cDialog* privateWarehouseDialog		= WINDOWMGR->GetWindowForID( PYO_STORAGEDLG );
			cDialog* guildWarehouseDialog		= WINDOWMGR->GetWindowForID( GDW_WAREHOUSEDLG );
			cDialog* itemMallWarehouseDialog	= WINDOWMGR->GetWindowForID( ITMALL_BASEDLG );

			ASSERT( npcScriptDialog && npcShopDialog && stallShopDialog && exchangeDialog );
			ASSERT( privateWarehouseDialog && guildWarehouseDialog && itemMallWarehouseDialog );

			if( npcScriptDialog->IsActive() )
			{
				return;
			}
			else if( npcShopDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1077 ) );
				return;
			}
			else if( stallShopDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1078 ) );
				return;
			}
			else if( exchangeDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1079 ) );
				return;
			}
			// 071211 LUJ 창고가 표시된 경우 사용 불가
			else if(	privateWarehouseDialog->IsActive()	||
				guildWarehouseDialog->IsActive()	||
				itemMallWarehouseDialog->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
			// 080429 NYJ --- 낚시중 사용불가
			else if(GAMEIN->GetFishingDlg()->IsPushedStartBtn())
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
			// 080429 NYJ --- 낚시포인트교환중 사용불가
			else if(GAMEIN->GetFishingPointDlg()->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
		}

		// 강화/인챈트/조합/분해/노점판매 창이 동시에 뜨는 것을 막는다
		{
			cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
			cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
			cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );

			ASSERT( mixDialog && enchantDialog && reinforceDialog );

			mixDialog->SetActive( FALSE );
			enchantDialog->SetActive( FALSE );
			reinforceDialog->SetActive( FALSE );
		}


		cWindow* const window = WINDOWMGR->GetWindowForID(
			IN_INVENTORYDLG);

		if(window)
		{
			window->SetActive(
				TRUE);
		}
		
		mSubmitButton->SetActive(
			FALSE);
	}

	Clear();
}

CItem* CDissolveDialog::GetSourceItem(const POINT& point)
{
	for(WORD i = 0; i < mSourceDialog->GetCellNum(); ++i)
	{
		CItem* const item = (CItem*)mSourceDialog->GetIconForIdx(i);

		if(0 == item)
		{
			continue;
		}

		RECT rect = {0};
		rect.left = LONG(item->GetAbsX());
		rect.top = LONG(item->GetAbsY());
		rect.right = LONG(item->GetAbsX()) + item->GetWidth();
		rect.bottom = LONG(item->GetAbsY()) + item->GetHeight();

		if(PtInRect(
			&rect,
			point))
		{
			return item;
		}
	}

	return 0;
}

void CDissolveDialog::Succeed(const MSG_ITEM_DISSOLVE_ACK& message)
{
	for(WORD cellIndex = 0; mSourceDialog->GetCellNum() > cellIndex; ++cellIndex)
	{
		CItem* const item = (CItem*)mSourceDialog->GetIconForIdx(
			cellIndex);

		item->SetLock(
			TRUE);
		item->SetMovable(
			FALSE);
		ITEMMGR->RemoveOption(
			item->GetDBIdx());
	}

	ResultContainer resultContainer;

	for(DWORD i = 0; i < message.mUpdateResult.mSize; ++i)
	{
		const ITEMBASE& itemBase = message.mUpdateResult.mItem[i];

		if(0 == itemBase.wIconIdx)
		{
			continue;
		}

		resultContainer[itemBase.wIconIdx] += (ITEMMGR->IsDupItem(itemBase.wIconIdx) ? itemBase.Durability : 1);
	}

	DWORD lockedTime = gCurTime;
	const DWORD delayedTime = 30;

	for(BYTE tablIndex = 0; GetTabNum() > tablIndex; ++tablIndex)
	{
		const BOOL isInstantLocked = (tablIndex != GetCurTabNum());
		cIconGridDialog* const iconGridDialog = (cIconGridDialog*)GetTabSheet(
			tablIndex);

		if(0 == iconGridDialog)
		{
			break;
		}

		for(WORD cellIndex = 0; iconGridDialog->GetCellNum() > cellIndex; ++cellIndex)
		{
			CItem* const item = (CItem*)iconGridDialog->GetIconForIdx(
				cellIndex);
			item->SetVisibleDurability(
				FALSE);
			item->SetItemParam(
				lockedTime += delayedTime);

			const ResultContainer::iterator iterator = resultContainer.find(
				item->GetItemIdx());

			if(resultContainer.end() == iterator)
			{
				item->SetDurability(
					0);
				item->SetLock(
					isInstantLocked);
				continue;
			}
			
			const ItemQuantity itemQuantity = iterator->second;

			if(0 >= itemQuantity)
			{
				item->SetDurability(
					0);
				item->SetLock(
					isInstantLocked);
				continue;
			}

			item->SetDurability(
				min(DURTYPE(itemQuantity), item->GetDurability()));

			resultContainer[item->GetItemIdx()] -= item->GetDurability();
		}
	}

	for(WORD cellIndex = 0; mRandomResultDialog->GetCellNum() > cellIndex; ++cellIndex)
	{
		CItem* const item = (CItem*)mRandomResultDialog->GetIconForIdx(
			cellIndex);
		item->SetVisibleDurability(
			FALSE);
		item->SetItemParam(
			lockedTime += delayedTime);

		const ResultContainer::iterator iterator = resultContainer.find(
			item->GetItemIdx());

		if(resultContainer.end() == iterator)
		{
			item->SetDurability(
				0);
			continue;
		}

		const ItemQuantity itemQuantity = iterator->second;

		if(0 >= itemQuantity)
		{
			item->SetDurability(
				0);
			continue;
		}

		item->SetDurability(
			min(DURTYPE(itemQuantity), item->GetDurability()));

		resultContainer[item->GetItemIdx()] -= item->GetDurability();
	}

	mSubmitButton->SetActive(
		FALSE);

	mIsEffectStart = TRUE;

	CHATMGR->AddMsg(
		CTC_SYSMSG,
		CHATMGR->GetChatMsg(1072));
}

void CDissolveDialog::PutEffect()
{
	if(FALSE == mIsEffectStart)
	{
		return;
	}

	cImage emptyImage;
	cImage coverImage;
	SCRIPTMGR->GetImage(
		141,
		&coverImage);

	BOOL isNoProceed = TRUE;

	for(BYTE tablIndex = 0; GetTabNum() > tablIndex; ++tablIndex)
	{
		if(tablIndex != GetCurTabNum())
		{
			continue;
		}

		cIconGridDialog* const iconGridDialog = (cIconGridDialog*)GetTabSheet(
			tablIndex);

		if(0 == iconGridDialog)
		{
			break;
		}

		for(WORD cellIndex = 0; iconGridDialog->GetCellNum() > cellIndex; ++cellIndex)
		{
			CItem* const item = (CItem*)iconGridDialog->GetIconForIdx(
				cellIndex);

			if(0 == item->GetItemParam())
			{
				continue;
			}
			else if(gCurTime < item->GetItemParam())
			{
				isNoProceed = FALSE;
				continue;
			}

			item->SetVisibleDurability(
				0 < item->GetDurability());
			item->SetLock(
				0 == item->GetDurability());
			item->SetMovable(
				FALSE);
			item->SetItemParam(
				0);
			item->SetStateImage(
				0 == item->GetDurability() && item->GetItemIdx() ? &coverImage : &emptyImage);
			
			isNoProceed = FALSE;
		}
	}

	for(WORD cellIndex = 0; mRandomResultDialog->GetCellNum() > cellIndex; ++cellIndex)
	{
		CItem* const item = (CItem*)mRandomResultDialog->GetIconForIdx(
			cellIndex);

		if(0 == item->GetItemParam())
		{
			continue;
		}
		else if(gCurTime < item->GetItemParam())
		{
			isNoProceed = FALSE;
			continue;
		}

		item->SetVisibleDurability(
			0 < item->GetDurability());
		item->SetLock(
			0 == item->GetDurability());
		item->SetMovable(
			FALSE);
		item->SetItemParam(
			0);
		item->SetStateImage(
			0 == item->GetDurability() && item->GetItemIdx() ? &coverImage : &emptyImage);

		isNoProceed = FALSE;
	}

	if(isNoProceed)
	{
		mIsEffectStart = FALSE;
	}
}

void CDissolveDialog::PutTip()
{
	typedef ItemIndex SourceItemIndex;
	typedef ItemQuantity SourceItemQuantity;
	typedef ItemQuantity ResultItemQuantity;
	typedef std::pair< SourceItemQuantity, ResultItemQuantity > Quantity;

	typedef std::map< SourceItemIndex, Quantity > SourceItemContainer;
	typedef ItemIndex ResultItemIndex;

	typedef std::map< ResultItemIndex, SourceItemContainer > ResultItemContainer;
	ResultItemContainer resultItemContainer;
	ResultItemContainer randomResultItemContainer;

	for(WORD cellIndex = 0; mSourceDialog->GetCellNum() > cellIndex; ++cellIndex)
	{
		CItem* const item = (CItem*)mSourceDialog->GetIconForIdx(
			cellIndex);

		ResultContainer resultContainer;
		GetDissolvedResult(
			item->GetItemBaseInfo(),
			resultContainer);

		while(false == resultContainer.empty())
		{
			const ResultContainer::const_iterator iterator = resultContainer.begin();
			const ItemIndex resultItemIndex = iterator->first;
			const ItemQuantity resultItemQuantity = iterator->second;
			resultContainer.erase(
				resultItemIndex);

			SourceItemContainer& sourceItemContainer = resultItemContainer[resultItemIndex];
			Quantity& quantity = sourceItemContainer[item->GetItemIdx()];
			quantity.first += (ITEMMGR->IsDupItem(item->GetItemIdx()) ? item->GetDurability() : 1);
			quantity.second += resultItemQuantity;
		}

		GetDissolvedRandomResult(
			item->GetItemBaseInfo(),
			resultContainer);

		while(false == resultContainer.empty())
		{
			const ResultContainer::const_iterator iterator = resultContainer.begin();
			const ItemIndex resultItemIndex = iterator->first;
			const ItemQuantity resultItemQuantity = iterator->second;
			resultContainer.erase(
				resultItemIndex);

			SourceItemContainer& sourceItemContainer = randomResultItemContainer[resultItemIndex];
			Quantity& quantity = sourceItemContainer[item->GetItemIdx()];
			quantity.first += (ITEMMGR->IsDupItem(item->GetItemIdx()) ? item->GetDurability() : 1);
			quantity.second += resultItemQuantity;
		}
	}

	LPCTSTR textDissolve = RESRCMGR->GetMsg(
		617);

	for(BYTE tabIndex = 0; GetTabNum() > tabIndex; ++tabIndex)
	{
		cPushupButton* const button = GetTabBtn(
			tabIndex);

		if(FALSE == button->IsActive())
		{
			break;
		}

		cIconGridDialog* const iconGridDialog = (cIconGridDialog*)GetTabSheet(
			tabIndex);

		for(WORD cellIndex = 0; iconGridDialog->GetCellNum() > cellIndex; ++cellIndex)
		{
			CItem* const item = (CItem*)iconGridDialog->GetIconForIdx(
				cellIndex);

			if(resultItemContainer.end() == resultItemContainer.find(item->GetItemIdx()))
			{
				continue;
			}

			ITEMMGR->AddToolTip(
				item);
			item->AddToolTipLine(
				"");
			item->AddToolTipLine(
				textDissolve);

			const SourceItemContainer& sourceItemContainer = resultItemContainer[item->GetItemIdx()];

			for(SourceItemContainer::const_iterator iterator = sourceItemContainer.begin();
				sourceItemContainer.end() != iterator;
				++iterator)
			{
				const SourceItemIndex sourceItemIndex = iterator->first;
				const Quantity& quantity = iterator->second;

				PutTip(
					*item,
					sourceItemIndex,
					quantity.first,
					quantity.second);
			}
		}
	}

	for(WORD cellIndex = 0; mRandomResultDialog->GetCellNum() > cellIndex; ++cellIndex)
	{
		CItem* const item = (CItem*)mRandomResultDialog->GetIconForIdx(
			cellIndex);

		if(randomResultItemContainer.end() == randomResultItemContainer.find(item->GetItemIdx()))
		{
			continue;
		}

		ITEMMGR->AddToolTip(
			item);
		item->AddToolTipLine(
			"");
		item->AddToolTipLine(
			textDissolve);

		const SourceItemContainer& sourceItemContainer = randomResultItemContainer[item->GetItemIdx()];

		for(SourceItemContainer::const_iterator iterator = sourceItemContainer.begin();
			sourceItemContainer.end() != iterator;
			++iterator)
		{
			const SourceItemIndex sourceItemIndex = iterator->first;
			const Quantity& quantity = iterator->second;

			PutTip(
				*item,
				sourceItemIndex,
				quantity.first,
				quantity.second);
		}
	}
}

void CDissolveDialog::PutTip(CItem& item, DWORD sourceItemIndex, DURTYPE sourceItemSize, DURTYPE resultItemSize) const
{
	const ITEM_INFO* const sourceItemInfo = ITEMMGR->GetItemInfo(
		sourceItemIndex);

	if(0 == sourceItemInfo)
	{
		return;
	}

	LPCTSTR textQuantity = RESRCMGR->GetMsg(
		630);
	const DWORD sourceItemColor = ITEMMGR->GetItemNameColor(
		*sourceItemInfo);

	TCHAR text[MAX_PATH] = {0};
	_stprintf(
		text,
		"%s %u%s %+d",
		sourceItemInfo->ItemName,
		sourceItemSize,
		textQuantity,
		resultItemSize);
	item.AddToolTipLine(
		text,
		sourceItemColor);
}

void CDissolveDialog::PutError(CDissolveDialog::Error error, DWORD itemIndex) const
{
	switch(error)
	{
	case ErrorInsufficientInventory:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(839));
			break;
		}
	case ErrorInsufficientResult:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(817));
			break;
		}
	case ErrorInsufficientDynamicResult:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(815));
			break;
		}
	case ErrorNoScript:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(816));
			break;
		}
	case ErrorInvalidLevel:
		{
			const DissolveScript* const script = ITEMMGR->GetDissolveScript(
				itemIndex);

			if(0 == script)
			{
				break;
			}

			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(819),
				script->mLevel);
			break;
		}
	case ErrorInInvalidSource:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(1084));
			break;
		}
	}
}

// 100625 ONS 분해시 우클릭으로 아이템이동 가능하도록 수정.
void CDissolveDialog::MoveIcon( CItem* movedItem )
{
	// 완료된 결과가 있을 경우 초기화시킨다
	{
		CItem* const item = (CItem*)mSourceDialog->GetIconForIdx(
			0);

		if(item->IsLocked())
		{
			Clear();
		}
	}

	if( 0 == movedItem )
	{
		return;
	}
	else if(WT_ITEM != movedItem->GetType())
	{
		return;
	}
	else if(movedItem->IsLocked())
	{
		return;
	}
	else if(FALSE == ITEMMGR->IsEqualTableIdxForPos(eItemTable_Inventory, movedItem->GetPosition()))
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(787));
		return;
	}

	const Error error = IsEnableUpdate(movedItem->GetItemBaseInfo());
	if(ErrorNone != error)
	{
		PutError(
			error,
			movedItem->GetItemIdx());
		return;
	}

	WORD wEmptyPos = 0;
	for(WORD cellIndex = 0; mSourceDialog->GetCellNum() > cellIndex; ++cellIndex)
	{
		CItem* const item = (CItem*)mSourceDialog->GetIconForIdx(
			cellIndex);

		if(0 == item->GetItemIdx())
		{
			wEmptyPos = cellIndex;
			break;
		}
	}

	AddSource(
		movedItem->GetItemBaseInfo(),
		POSTYPE(wEmptyPos));
	AddResult(
		movedItem->GetItemBaseInfo());

	mSubmitButton->SetActive(
		TRUE);
}