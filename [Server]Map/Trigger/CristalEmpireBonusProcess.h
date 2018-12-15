#pragma once

namespace Trigger
{
	class CCristalEmpireBonusProcess
	{
	private:
		struct PlayerBonus
		{
			struct ItemBonus
			{
				typedef float Power;
				typedef std::pair< ITEM_OPTION::Drop::Key, Power > Bonus;
				typedef std::list< Bonus > BonusContainer;
				BonusContainer mBonusContainer;
				LEVELTYPE mLevel;

				ItemBonus() :
				mLevel(1)
				{}
			};
			typedef DWORD ItemIndex;
			typedef std::map< ItemIndex, ItemBonus > ItemContainer;
			ItemContainer mItemContainer;
			typedef DWORD ItemDbIndex;
			typedef std::set< ItemDbIndex > DbIndexContainer;
			DbIndexContainer mDbIndexContainer;
			size_t mSize;

			PlayerBonus() :
			mSize(1)
			{}
		};
		typedef DWORD PlayerIndex;
		typedef std::map< PlayerIndex, PlayerBonus > PlayerBonusContainer;
		PlayerBonusContainer mPlayerBonusContainer;
		typedef std::set< PlayerIndex > PlayerIndexContainer;
		PlayerIndexContainer mRequestPlayerContainer;

	public:
		CCristalEmpireBonusProcess();
		virtual ~CCristalEmpireBonusProcess();
		// 100111 LUJ, 트리거의 액션이 요청한 옵션 처리를 일괄적으로 실행한다
		void Idle();
		void Batch(DWORD playerIndex, DWORD itemIndex, ITEM_OPTION::Drop::Key, float power, LEVELTYPE, size_t size);
		void Run(DWORD playerIndex, DWORD itemIndex, DWORD itemDbIndex, POSTYPE, ITEM_OPTION&);
		void Remove(DWORD playerIndex);
	};
}