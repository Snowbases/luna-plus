#pragma once

#include "Pool.h"

namespace Trigger
{
	enum eProperty;
	enum eCheckType;
	struct stProperty;
	struct ConditionDesc;
	class CMessage;
	class CTrigger;
	
	class CCondition : public CPoolObject
	{
	public:
		struct Parameter
		{
			DWORD mChannelID;
			DWORD mDescIndex;
			eOwner mOwnerType;
			DWORD mOwnerIndex;
			DWORD mTriggerIndex;

			Parameter() :
			mDescIndex(0),
			mChannelID(0),
			mOwnerType(eOwner(0)),
			mOwnerIndex(0),
			mTriggerIndex(0)
			{}
		};
		typedef BOOL (*Func)(const CCondition&, const CMessage&);

	public:
		CCondition( const Parameter& );
		virtual ~CCondition(void);
		BOOL Check(const CMessage&);
		void Reset();
		int GetCheckCount() const { return mCheckCount; }
		const Parameter& GetParameter() const;
		int GetValue(eProperty) const;
		const stProperty& GetProperty(eProperty) const;
		const CTrigger& GetTrigger() const;
		DWORD GetChannelID() const;
		DWORD GetSuccessObjectIndex() const {return mSuccessObjectIndex;}

	private:
		// 설정된 체크 카운트에 도달했는지 검사한다
		BOOL IsValidCheckCount(DWORD checkCount) const;
		BOOL IsInvalidFlag(const stProperty&) const;
		BOOL IsInvalid(const CMessage&) const;		

	private:
		Parameter mParameter;
		eCheckType mType;
		// 참일때 증가
		int	mCheckCount;
		DWORD mSuccessObjectIndex;
	};
}