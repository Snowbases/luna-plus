#pragma once

class cSkillObject;

class cSkillInfo
{
public:
	enum Type
	{
		TypeNone,
		TypeActive,
		TypeBuff,
	};
	Type GetType() const { return mType; }

private:
	Type mType;

public:
	cSkillInfo(Type type) : mType(type) {}
	virtual ~cSkillInfo(void) {}
	virtual cSkillObject* GetSkillObject() const = 0;
	virtual SKILLKIND GetKind() const = 0;
	virtual DWORD GetIndex() const = 0;
	virtual LEVELTYPE GetLevel() const = 0;
};