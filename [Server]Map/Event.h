/*
080616 LUJ, 버프 중에 발생하는 다양한 이벤트를 처리하기 위한 추상 클래스
*/
#pragma once

class CObject;

class CEvent
{
public:
	enum Type
	{
		TypeNone,
		TypeTakeDamage,
		TypeGiveDamage,
		TypeDie,
		TypeKill,
	};

	inline Type GetType() const	{ return mType; }
	inline eStatusKind GetTargetStatus() const { return mTargetStatus; }

protected:
	// 080616 LUJ, 직접 생성하지 못하도록 생성자/소멸자를 protected로 선언한다
	CEvent( Type, eStatusKind );
	virtual ~CEvent(void) {};

	Type mType;
	// 100310 ShinJS --- 특정 eStatusKind 만 처리할수 있도록 변수 추가
	eStatusKind			mTargetStatus;
};

// 080616 LUJ, 다른 오브젝트에게 피해를 줬을 때 발동되는 이벤트
class CGiveDamageEvent : public CEvent
{
public:
	CGiveDamageEvent( CObject* attacker, const RESULTINFO& );
	CGiveDamageEvent( CObject* attacker, const RESULTINFO&, eStatusKind );

	inline const RESULTINFO& GetResult() const	{ return mResult; }
	inline CObject* GetTarget() const			{ return mTarget; }

private:
	const RESULTINFO	mResult;
	CObject*			mTarget;
};

// 080616 LUJ, 다른 오브젝트가 피해를 받았을 때 발동되는 이벤트
// 100219 ShinJS --- RESULTINFO 를 수정할수 있도록 변경
class CTakeDamageEvent : public CEvent
{
public:
	CTakeDamageEvent( CObject* attacker, RESULTINFO& );
	CTakeDamageEvent( CObject* attacker, RESULTINFO&, eStatusKind );

	inline RESULTINFO& GetResult() const	{ return mResult; }
	inline CObject* GetAttacker() const			{ return mAttacker; }

private:
	RESULTINFO&			mResult;
	CObject*			mAttacker;
};

// 080616 LUJ, 다른 오브젝트에게 피살될 때 발생하는 이벤트
// 080708 LUJ, 피해자를 인자로 받도록 함
class CDieEvent : public CEvent
{
public:
	CDieEvent( CObject* killer, CObject* victim );

	inline CObject* GetKiller() const	{ return mKiller; }
	inline CObject* GetVictim() const	{ return mVictim; }

protected:
	CObject* mVictim;
	CObject* mKiller;
};

// 080616 LUJ, 다른 오브젝트를 살해할 때 발생하는 이벤트
// 080708 LUJ, CDieEvent와 같은 구조를 쓰도록 함
class CKillEvent : public CDieEvent
{
public:
	CKillEvent( CObject* killer, CObject* victim );
};