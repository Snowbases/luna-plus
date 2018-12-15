#pragma once

class CObject;

class cSkillEffect
{
	CObject* mpOperator;
	MAINTARGET* mpMainTarget;
	int mEffectNum;
	HEFFPROC m_hEff;

	DWORD mSkillObjectID;

public:
	cSkillEffect(void);
	virtual ~cSkillEffect(void);

	void Init( int EffectNum, CObject* pOperator, MAINTARGET* pMainTarget );
	void StartEffect( BOOL bFatal, float rate = 1.0f );
	void EndEffect();
	void StartTargetEffect( DWORD id );
	void EndTargetEffect();
	void SetTargetArray(CObject* pOperator,CTargetList* pTList,MAINTARGET* pMainTarget);
};
