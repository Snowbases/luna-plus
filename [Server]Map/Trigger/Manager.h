#pragma once
#include "common.h"
#include "message.h"

#define TRIGGERMGR (&Trigger::CManager::GetInstance())

enum EObjectKind;
class CPlayer;

namespace Trigger
{
	class CTrigger;
	class CMessage;
	class CParser;
	class CAction;
	class CCondition;
	class CFlagManager;
	class CTimer;
	class CCristalEmpireBonusProcess;
	typedef BOOL (*CheckFunc)(const CCondition&, const CMessage&);
	typedef CAction* (*ActionFunc)();
	
	class CManager
	{
	public:
		static CManager& GetInstance();
		static void NetworkMsgParser(DWORD connectionIndex, LPSTR packet, DWORD length);
		CMessage* const AllocateMessage(DWORD channelIndex);
		void Release();
		void Process();
		void NetworkMsgParser(DWORD dwConnectionIndex, BYTE Protocol, LPVOID pMsg, DWORD dwLength);
		void LoadTrigger(CPlayer&);
		CTrigger* CreateTrigger(eOwner, DWORD ownerIndex, DWORD dwChannelID, DWORD hashCode, DWORD repeatCount, struct tm& regTime);
		void CreateTrigger(eOwner, LPDBMESSAGE, LPQUERY);
		const TriggerDesc& GetTriggerDesc(DWORD hashCode) const;
		const ConditionDesc& GetConditionDesc(DWORD conditionDescIndex) const;
		const ActionDesc& GetActionDesc(DWORD actionDescIndex) const;
		DWORD GetHashCode(LPCTSTR);
		LPCTSTR GetAliasName(DWORD hashCode) const;
		CTrigger& GetTrigger(DWORD triggerIndex);
		void RestoreRepeatCount(DWORD triggerIndex, DWORD repeatCount, const struct tm&);
		void StopServerTrigger(DWORD channelIndex);
		void StopObjectTrigger(EObjectKind, DWORD objectIndex);
		CheckFunc GetCheckFunc(eProperty) const;
		CAction* CreateAction(eAction) const;
		void ToggleStatistic();
		void TogglePeriodicMessage();
		void SetFlag(eOwner, DWORD ownerIndex, DWORD alias, int value);
		int GetFlag(eOwner, DWORD ownerIndex, DWORD alias) const;
		bool IsFlag(eOwner, DWORD ownerIndex, DWORD alias) const;
		void StartTimer(DWORD alias, DWORD dwChannel, eTimerType type, DWORD dwDuration);
		BOOL SetTimerState(DWORD alias, eTimerState state);
		CTimer* GetTimer(DWORD alias);
		void BatchItemOption(DWORD playerIndex, DWORD itemIndex, ITEM_OPTION::Drop::Key, float power, LEVELTYPE, size_t size);

	private:
		// LUJ, 싱글턴 객체이므로 생성자/소멸자를 private으로 선언한다
		CManager(void);
		virtual ~CManager(void);
		int GetMessageProceedCount();
		void UpdateDesc();
		void UpdateDescGroup();
		void StopTrigger(eOwner, DWORD ownerIndex);
		void ShowStatistic();
		BOOL IsNoOwner(eOwner, DWORD ownerIndex);
		DWORD GetChannelIndex(eOwner, DWORD ownerIndex);
		void CreateDefaultTrigger(eOwner, DWORD ownerIndex, DWORD channelIndex);
		
	private:
		// 키: 소유주 종류, 값: 소유주 번호
		typedef std::pair< eOwner, DWORD > OwnerKey;
		// LUJ, 트리거 매니저가 최초에 실행시킬 시스템 트리거 번호를 저장하는 컨테이너
		typedef DWORD TriggerIndex;
		typedef std::list< TriggerIndex > TriggerIndexList;
		stdext::hash_map< DWORD, TriggerIndexList > mTriggerDescByGroup;
		// 메시지 큐
		typedef std::map< DWORD, CMessage > MessageMap;
		MessageMap mMessageMap;
		// 삭제할 트리거 인덱스 큐
		std::queue< TriggerIndex > mDeleteTriggerQueue;
		// 트리거 인스턴스가 보관되는 컨테이너
		typedef stdext::hash_map< DWORD, CTrigger > TriggerMap;
		TriggerMap mTriggerMap;
		// 091116 LUJ, 채널별로 분류된 트리거 번호가 저장된다
		typedef DWORD ChannelIndex;
		typedef stdext::hash_set< ChannelIndex > TriggerIndexSet;
		typedef stdext::hash_map< ChannelIndex, TriggerIndexSet > ChannelTriggerMap;
		ChannelTriggerMap mChannelTriggerMap;
		std::queue< TriggerIndex > mReservedTriggerQueue;
		// 091116 LUJ, 한 프로세스 타임에 처리할 메시지 개수
		int mMessageProceedCount;
		CParser* mParser;
		struct Statistic
		{
			DWORD mProceededTriggerCount;
			DWORD mRemovedTriggerCount;
			DWORD mAllocatedTriggerCount;
			DWORD mProceededMessageCount;
			DWORD mAllocatedMessageCount;
			DWORD mRemovedMessageCount;

			Statistic()
			{
				ZeroMemory(this, sizeof(*this));
			}
		}
		mStatistic;
		typedef stdext::hash_set< TriggerIndex > TriggerIndexSet;
		// 트리거 소유주에 따라 생성된 트리거 번호를 저장한다
		// 런타임으로 체크할 때 삭제 타이밍을 놓치고 다시 생성되는 경우가 있다.
		// 이를 막기 위해 이벤트 단위로 트리거를 생성/삭제할 수 있도록 컨테이너를 관리한다
		typedef std::map< OwnerKey, TriggerIndexSet > OwnerCreatedTriggerMap;
		OwnerCreatedTriggerMap mOwnerCreatedTriggerMap;
		BOOL mIsHideStatistic;
		BOOL mIsUnablePeriodicMessage;
		CFlagManager* mFlagManager;
		// 타이머
		typedef stdext::hash_map< DWORD, CTimer* > TimerMap;
		TimerMap mTimerMap;
		CCristalEmpireBonusProcess* mCristalEmpireBonusProcess;		
	};
}
