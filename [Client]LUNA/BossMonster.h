// BossMonster.h: interface for the CBossMonster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOSSMONSTER_H__F4B18B29_BDC0_4858_B5AD_BEC837D5583B__INCLUDED_)
#define AFX_BOSSMONSTER_H__F4B18B29_BDC0_4858_B5AD_BEC837D5583B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Monster.h"
#include "../[cc]skill/client/delay/timedelay.h"

class CBossMonster : public CMonster  
{
	
public:
	CBossMonster();
	virtual ~CBossMonster();

	// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
	virtual void Damage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO&);
	virtual void SetMotionInState(EObjectState);
	cTimeDelay* GetOpenningTime() { return &m_OpenningTime ; }

	void		SetOpenningTime(DWORD delay)	
	{	
		m_OpenningTime.End();
		m_OpenningTime.Init(delay);
		m_OpenningTime.Start();
		SetOpenningStart();
	}

	void		SetOpenningEnd()	{	m_bOpenning = FALSE;	}
	void		SetOpenningStart()	{	m_bOpenning	= TRUE;		}
	BOOL		IsOpenning()		{	return m_bOpenning;		}

private:
	cTimeDelay				m_OpenningTime ;
	BOOL					m_bOpenning;
	//------------------------------------------------------------------------------
//	virtual void AddStatus(CStatus* pStatus);
};

#endif // !defined(AFX_BOSSMONSTER_H__F4B18B29_BDC0_4858_B5AD_BEC837D5583B__INCLUDED_)
