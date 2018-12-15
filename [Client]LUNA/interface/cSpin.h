//////////////////////////////////////////////////////////////////////
// class name : cSpin 
// Lastest update : 2002.10.29. by taiyo
//////////////////////////////////////////////////////////////////////

#ifndef _cSPIN_H_
#define _cSPIN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cEditBox.h"


class cButton;

extern void SpinUpBtnProc(LONG lId, void * p, DWORD we);
extern void SpinDownBtnProc(LONG lId, void * p, DWORD we);

typedef		DWORD	SPINUNIT;

class cSpin : public cEditBox  
{
public:
	cSpin();
	virtual ~cSpin();
	
	virtual void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, cbFUNC Func, LONG ID=0);
	virtual DWORD ActionEvent(CMouse * mouseInfo);
	virtual void Render();
	virtual void SetAbsXY(LONG x, LONG y);
	virtual void Add(cWindow * wnd);
	void InitSpin(WORD spinStrSize, WORD strSize);
	LONG GetValue();
	void SetValue(SPINUNIT value);
	void IncUnit();
	void DecUnit();



	LONG GetMin(){ return m_minValue; }
	LONG GetMax(){ return m_maxValue; }
	void SetUnit(SPINUNIT unit){ m_Unit = unit; }
	// 091125 ONS 아이템취급 최대갯수 100 -> 1000으로 수정
	void SetMinMax(SPINUNIT min=0, SPINUNIT max=1000){ m_minValue = min; m_maxValue = max; }
	void SetMin(SPINUNIT min=0){ m_minValue = min; }
	void SetMax(SPINUNIT max=1000){ m_maxValue = max; }

protected:
	cButton * m_upBtn;
	cButton * m_downBtn;
	
	SPINUNIT m_Unit;
	SPINUNIT m_minValue;
	SPINUNIT m_maxValue;
};

#endif // _cSPIN_H_
