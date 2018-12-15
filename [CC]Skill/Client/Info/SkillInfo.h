#pragma once

class CObject;
class CActionTarget;

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
	cSkillInfo(Type);
	virtual ~cSkillInfo();
	virtual SKILLKIND GetKind() const = 0;
	virtual DWORD GetIndex() const = 0;
	virtual LPCTSTR GetName() const = 0;
	virtual DWORD GetTooltip() const = 0;
	virtual int GetImage() const = 0;
	virtual LEVELTYPE GetLevel() const = 0;
	virtual DWORD GetCoolTime() const = 0;
	void SendMsgToServer(CObject*, CActionTarget*, const ITEMBASE&);
};
