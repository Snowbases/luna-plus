#pragma once
#include "common.h"

namespace Trigger
{	
	struct PropertyText;
	struct ValueText;
	class CCondition;
	class CMessage;
	class CAction;
	typedef BOOL (*CheckFunc)(const CCondition&, const CMessage&);
	typedef CAction* (*ActionFunc)();

	class CParser
	{
		friend class CManager;
	public:
		BOOL Update();
		const TriggerDesc& GetTriggerDesc(DWORD hashCode) const;
		const ActionDesc& GetActionDesc(DWORD actionDescIndex) const;
		const ConditionDesc& GetConditionDesc(DWORD conditionDescIndex) const;
		CheckFunc GetCheckFunc(eProperty) const;
		CAction* CreateAction(eAction) const;

	protected:
		CParser(MAPTYPE);
		virtual ~CParser();
		void Load(LPCTSTR fileName);
		void Release();
		eProperty ParseKeyword(LPCTSTR);
		int ParseValue(eProperty, LPCTSTR);
		eOperation ParseOperation(LPCTSTR) const;
		LPCTSTR GetAliasName(DWORD hashCode) const;
		DWORD GetHashCode(LPCTSTR);
		LPCTSTR GetDescName() const;
		// 중복된 값이 등록되어 있는지 검사한다
		void CheckData(const PropertyText*, size_t);
		void CheckData(const ValueText*, size_t);
		void UpdateFuncMap();
		
	private:
		// 트리거 스크립트
		typedef stdext::hash_map<DWORD, TriggerDesc> TriggerDescMap;
		TriggerDescMap mTriggerDescMap;
		// 컨디션 스크립트
		typedef stdext::hash_map<DWORD, ConditionDesc>	ConditionDescMap;
		ConditionDescMap mConditionDescMap;
		// 액션 스크립트
		typedef stdext::hash_map<DWORD, ActionDesc> ActionDescMap;
		ActionDescMap mActionDescMap;
		// 별칭 테이블. 키: 별칭으로 얻어진 해쉬코드. GetHashCode() 참조
		typedef stdext::hash_map< DWORD,std::string > AliasMap;
		AliasMap mAliasMap;
		// 100111 LUJ, 검사했던 값은 더 이상 계산하지 않기 위한 저장소 컨테이너
		typedef stdext::hash_map< std::string, DWORD > HashCodeMap;
		HashCodeMap mHashCodeMap;
		struct stat mFileStatus;
		MAPTYPE mMapType;
		// 컨디션에서 검사하는 속성이 있을 경우 저장되어 있다
		typedef stdext::hash_map< eProperty, CheckFunc > CheckFuncMap;
		CheckFuncMap mCheckFuncMap;
		// 속성별로 실행할 액션이 저장되어 있다
		typedef stdext::hash_map< eAction, ActionFunc > ActionFuncMap;
		ActionFuncMap mActionFuncMap;
	};
}