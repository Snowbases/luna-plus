#pragma once

#include "EffectUnitDesc.h"
#include "MemoryPoolTempl.h"
#include "MoveEffectUnit.h"

class CMHFile;

class CMoveEffectUnitDesc : public CEffectUnitDesc  
{
	static CMemoryPoolTempl<CMoveEffectUnit>* m_pUnitPool;

	DWORD m_Duration;
	BOOL m_bApplyWeight;
	BOOL m_bYMove;
	BOOL m_bTurnChar;
public:
	CMoveEffectUnitDesc(BOOL bDangledToOperator);
	virtual ~CMoveEffectUnitDesc();

	void DoParseScript(char* szKey,CMHFile* pFile);
	virtual CEffectUnit* GetEffectUnit(EFFECTPARAM* pEffectParam);

	friend class CMoveEffectUnit;
	
	static void ReleaseUnit(CMoveEffectUnit* pUnit)
	{
		if(m_pUnitPool)
			m_pUnitPool->Free(pUnit);
	}
};