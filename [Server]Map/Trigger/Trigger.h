#pragma once

#include "Pool.h"

namespace Trigger
{
	enum eState;
	enum eOwner;
	enum eProperty;
	class CCondition;
	class CMessage;

	class CTrigger : public CPoolObject
	{
		friend class CManager;
	public:
		struct Parameter
		{
			DWORD mIndex;
			eOwner mOwnerType;
			DWORD mOwnerIndex;
			DWORD mChannelID;
			DWORD mHashCode;
			DWORD mRepeatCount;
			TCHAR mName[MAX_PATH];
			struct tm mRegTime;
		};

	public:
		CTrigger();
		virtual ~CTrigger();
		eState GetState() const	{ return mState; }
		DWORD GetIndex() const { return mParameter.mIndex; }
		// DB로 부터 상태를 받아올 때 사용 (생성시 호출)
		void Process(const CMessage&);
		int GetActionValue(DWORD descIndex, eProperty) const;
		// 트리거의 원래 이름을 가져온다
		DWORD GetHashCode() const;
		// 주어진 해쉬 코드를 트리거의 소유주 번호, 형태에 따라 변형해서 반환한다
		DWORD GetHashCode(DWORD aliasHashCode) const;
		DWORD GetRepeatCount() const { return mParameter.mRepeatCount; }
		DWORD GetSuccessObjectIndex(DWORD conditionHashcode) const;
		DWORD GetChannel() const { return mParameter.mChannelID; }
		void RestoreRepeatCount(DWORD repeatCount);
		LPCTSTR GetName() const { return mParameter.mName; }
		const struct tm& GetRegTime() const { return mParameter.mRegTime; }
		void SetRegTime(const struct tm&);
		inline eOwner GetOwnerType() const { return mParameter.mOwnerType; }
		inline DWORD GetOwnerIndex() const { return mParameter.mOwnerIndex; }
		void UpdateParamter(const Parameter&);
		void NetworkMsgParser(DWORD connectionIndex, BYTE protocol, LPVOID packet, DWORD size);

	private:
		void SetState(eState state)	{ mState = state; }
		void DoAction() const;
		void UpdateCondition();
		void ReleaseCondition();
		BOOL FailCondition(const CMessage&) const;
		void SetDescTimeStamp();
		void SaveToDb();
		void LoadFromDb();
		BOOL IsLoop() const;
		void ResetCondition();

	private:
		Parameter mParameter;
		// 값: 해쉬코드
		typedef stdext::hash_map< DWORD, CCondition* > ConditionMap;
		ConditionMap mConditionMap;
		eState mState;
	};
}