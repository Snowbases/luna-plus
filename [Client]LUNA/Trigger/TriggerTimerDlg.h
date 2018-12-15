#pragma once

#include "stdAfx.h"
#include "interface/cWindowHeader.h"
#include "interface/cDialog.h"
#include "CommonGameDefine.h"

class cStatic;
class CImageNumber;

class CTriggerTimerDlg :
	public cDialog
{
private:
	CImageNumber*	m_csTimeMin;
	CImageNumber*	m_csTimeSec;
	cStatic*		m_csTime;

	eTimerType		m_Type;
	eTimerState		m_State;
	DWORD			m_dwStartTime;
	DWORD			m_dwDurationTime;

public:
	CTriggerTimerDlg(void);
	virtual ~CTriggerTimerDlg(void);

	void		 	Linking();
	virtual void 	Render();

	void			Start(eTimerType type, DWORD dwDuration, DWORD dwElapsedTime);
	void			SetState(eTimerState state);
};
