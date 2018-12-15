#pragma once
#include "skillinfo.h"

class CPlayer;
enum SetSkillStatusType;

class cBuffSkillInfo :
	public cSkillInfo
{
protected:
	BUFF_SKILL_INFO mInfo;
	// 081203 LUJ, 스크립트로 수치 보너스를 적용한다
	void SetStatus( CObject&, Status& status, const SkillScript::Buff&, SetSkillStatusType ) const;
	// 081203 LUJ, 스크립트로 버프 상태를 적용한다
	void SetBuff( CObject&, const SkillScript&, SetSkillStatusType ) const;

public:
	cBuffSkillInfo(void);
	virtual ~cBuffSkillInfo(void);
	void Parse(LPCTSTR);
	BUFF_SKILL_INFO* GetSkillInfo() { return &mInfo; }
	void AddPassiveStatus();
	void RemovePassiveStatus();

	void AddPetBuffStatus();
	void RemovePetBuffStatus();
	void AddBuffStatus();
	void RemoveBuffStatus();
	void SetObjectBattleState( CObject* pOperator, CObject* pTarget );
	virtual SKILLKIND GetKind() const { return mInfo.Kind; }
	virtual DWORD GetIndex() const { return mInfo.Index; }
	virtual LPCTSTR GetName() const { return mInfo.Name; }
	virtual DWORD GetTooltip() const { return mInfo.Tooltip; }
	virtual int GetImage() const { return mInfo.Image; }
	virtual LEVELTYPE GetLevel() const { return mInfo.Level; }
	virtual DWORD GetCoolTime() const { return 0; }
};
