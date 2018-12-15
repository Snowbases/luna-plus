#include "StdAfx.h"
#include "CristalEmpireBonusProcess.h"
#include "..\UserTable.h"
#include "..\Player.h"
#include "..\ItemManager.h"
#include "..\MapDBMsgParser.h"

namespace Trigger
{
	CCristalEmpireBonusProcess::CCristalEmpireBonusProcess()
	{}

	CCristalEmpireBonusProcess::~CCristalEmpireBonusProcess()
	{}

	void CCristalEmpireBonusProcess::Idle()
	{
		for(PlayerIndexContainer::const_iterator playerIterator = mRequestPlayerContainer.begin();
			mRequestPlayerContainer.end() != playerIterator;
			++playerIterator)
		{
			const PlayerIndex playerIndex = *playerIterator;
			CObject* const object = g_pUserTable->FindUser(
				playerIndex);

			if(0 == object)
			{
				mPlayerBonusContainer.erase(playerIndex);
				continue;
			}
			else if(eObjectKind_Player != object->GetObjectKind())
			{
				continue;
			}
			
			const PlayerBonus& playerBonus = mPlayerBonusContainer[playerIndex];

			MSG_ITEM_OPTION message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_TRIGGER;
			message.Protocol = MP_TRIGGER_ITEM_OPTION_REQUEST_ACK;
			message.mOption[0].mItemDbIndex = playerBonus.mSize;

			if(playerBonus.mItemContainer.size() > sizeof(message.mOption) / sizeof(*message.mOption))
			{
				continue;
			}

			for(PlayerBonus::ItemContainer::const_iterator itemIterator = playerBonus.mItemContainer.begin();
				itemIterator != playerBonus.mItemContainer.end();
				++itemIterator)
			{
				const DWORD itemIndex = itemIterator->first;
				const PlayerBonus::ItemBonus& itemBonus = itemIterator->second;
				ITEM_OPTION& itemOption = message.mOption[message.mSize++];
				itemOption.mEnchant.mIndex = itemIndex;
				itemOption.mEnchant.mLevel = BYTE(itemBonus.mLevel);

				const size_t dropOptionSize = sizeof(itemOption.mDrop.mValue) / sizeof(*itemOption.mDrop.mValue);

				for(size_t i = 0; i < dropOptionSize; ++i)
				{
					PlayerBonus::ItemBonus::BonusContainer::const_iterator bonusIterator = itemBonus.mBonusContainer.begin();

					std::advance(
						bonusIterator,
						i);

					if(itemBonus.mBonusContainer.end() == bonusIterator)
					{
						break;
					}

					const PlayerBonus::ItemBonus::Bonus& bonus = *bonusIterator;
					ITEM_OPTION::Drop::Value& value = itemOption.mDrop.mValue[i];
					value.mKey = bonus.first;
					value.mValue = bonus.second;
				}
			}
			
			object->SendMsg(
				&message,
				message.GetSize());
		}

		mRequestPlayerContainer.clear();
	}

	void CCristalEmpireBonusProcess::Batch(PlayerIndex playerIndex, DWORD itemIndex, ITEM_OPTION::Drop::Key key, float power, LEVELTYPE level, size_t size)
	{
		PlayerBonus& playerBonus = mPlayerBonusContainer[playerIndex];
		playerBonus.mSize = max(
			playerBonus.mSize,
			size);		

		PlayerBonus::ItemBonus& itemBonus = playerBonus.mItemContainer[itemIndex];
		itemBonus.mLevel = max(
			itemBonus.mLevel,
			level);
		itemBonus.mBonusContainer.push_back(
			PlayerBonus::ItemBonus::Bonus(key, power));

		// 100111 LUJ, 한 아이템에 여러 종류의 옵션을 적용시킨다. 이에 Add()요청이 종류 개수에
		//			따라 호출된다. 따라서 셋 컨테이너에 넣어야 전송이 중복되지 않는다
		mRequestPlayerContainer.insert(
			playerIndex);
	}

	void CCristalEmpireBonusProcess::Run(PlayerIndex playerIndex, DWORD itemIndex, DWORD itemDbIndex, POSTYPE itemPosition, ITEM_OPTION& itemOptionResult)
	{
		CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(
			playerIndex);

		if(0 == player)
		{
			return;
		}
		else if(eObjectKind_Player != player->GetObjectKind())
		{
			return;
		}

		PlayerBonusContainer::iterator playerBonusIterator = mPlayerBonusContainer.find(playerIndex);

		// 100111 LUJ, 아이템을 강화할 수 없는 플레이어. 비정상 진행.
		if(mPlayerBonusContainer.end() == playerBonusIterator)
		{
			return;
		}

		CItemSlot* const itemSlot = player->GetSlot(itemPosition);

		if(0 == itemSlot)
		{
			return;
		}

		ITEMBASE* const itemBase = itemSlot->GetItemInfoAbs(itemPosition);

		if(0 == itemBase)
		{
			return;
		}
		else if(itemBase->dwDBIdx != itemDbIndex)
		{
			return;
		}
		else if(itemBase->wIconIdx != itemIndex)
		{
			return;
		}

		PlayerBonus& playerBonus = playerBonusIterator->second;

		if(0 == playerBonus.mSize)
		{
			return;
		}
		else if(playerBonus.mDbIndexContainer.end() != playerBonus.mDbIndexContainer.find(itemDbIndex))
		{
			return;
		}

		const PlayerBonus::ItemContainer::const_iterator itemContainer = playerBonus.mItemContainer.find(itemBase->wIconIdx);

		// 100111 LUJ, 보너스를 부여할 수 없는 아이템
		if(playerBonus.mItemContainer.end() == itemContainer)
		{
			return;
		}

		const PlayerBonus::ItemBonus& itemBonus = itemContainer->second;

		if(itemBonus.mLevel <= itemBase->ItemParam)
		{
			return;
		}

		ITEM_OPTION::Drop dropOption = ITEM_OPTION::Drop();
		const size_t dropOptionSize = sizeof(dropOption.mValue) / sizeof(*dropOption.mValue);

		for(size_t i = 0; i < dropOptionSize; ++i)
		{
			PlayerBonus::ItemBonus::BonusContainer::const_iterator iterator = itemBonus.mBonusContainer.begin();
			std::advance(
				iterator,
				i);

			if(itemBonus.mBonusContainer.end() == iterator)
			{
				break;
			}

			const PlayerBonus::ItemBonus::Bonus& bonus = *iterator;
			ITEM_OPTION::Drop::Value& value = dropOption.mValue[i];
			const float level = float(itemBase->ItemParam + 1);
			// 100111 LUJ, [제국 아이템 강화 시뮬레이션.xls] 참조
			//			sin 함수를 사용하므로, power 값을 넘어서는 결과가 나오지 않는다
			value.mKey = bonus.first;
			value.mValue = GetItemDungeonBonus(
				level,
				itemBonus.mLevel,
				bonus.second);			
		}

		ITEM_OPTION itemOption = ITEMMGR->GetOption(*itemBase);

		// 100111 LUJ, 이미 보너스가 최대인 아이템
		if(0 == memcmp(
			&itemOption.mDrop,
			&dropOption,
			sizeof(dropOption)))
		{
			// TODO: 이미 최대인데 뭘 한다구
			return;
		}

		// 100111 LUJ, 회수를 감소시켜주지 않으면, 클라이언트 조작으로 대량 처리될 수 있다
		--playerBonus.mSize;

		// 100111 LUJ, 레벨을 증가시킨다
		++(itemBase->ItemParam);
		itemOption.mItemDbIndex = itemBase->dwDBIdx;
		itemOption.mDrop = dropOption;
		
		ITEMMGR->AddOption(
			itemOption);
		ItemUpdateToDB(
			playerIndex,
			itemDbIndex,
			itemIndex,
			itemBase->Durability,
			itemBase->Position,
			itemBase->QuickPosition,
			itemBase->ItemParam);
		ItemDropOptionInsert(
			itemOption,
			playerIndex);
		LogItemDropOption(
			itemOption,
			eLog_AddItemByTrigger);

		itemOptionResult = itemOption;

		playerBonus.mDbIndexContainer.insert(
			itemDbIndex);
	}

	void CCristalEmpireBonusProcess::Remove(DWORD playerIndex)
	{
		mPlayerBonusContainer.erase(
			playerIndex);
	}
}