#ifndef _HSEOSTESTDLG_H
#define _HSEOSTESTDLG_H

#include "./Interface/cDialog.h"
#include "ImageNumber.h"
#include "../hseos/Common/SHProgressBar.h"

class cStatic;

class CSHMonstermeterDlg : public cDialog
{
protected:
	CImageNumber	m_ImageNumber;
	CImageNumber	m_GiftEventTimeNumber;

	cStatic*		m_PlayTimeText;
	cStatic*		m_KillMonNumText;
	cStatic*		m_GiftTimeText1;
	cStatic*		m_GiftTimeText2;
	cStatic*		m_GiftTimeText3;

	DWORD			m_GiftEventTime;

	CSHProgressBar	m_csFarmRePlantTimeBar;
	CSHProgressBar	m_csFarmReManureTimeBar;
	BOOL			m_bGiftEvent;
	cDialog*		m_GiftDlg;

public:
	CSHMonstermeterDlg() ;
	~CSHMonstermeterDlg() ;

	void		 	Linking();
	virtual void 	SetActive( BOOL val );
	virtual void 	Render();

	CSHProgressBar*	GetFarmRePlantTimeBar()		{ return &m_csFarmRePlantTimeBar; }
	CSHProgressBar*	GetFarmReManureTimeBar()	{ return &m_csFarmReManureTimeBar; }

	//090703 pdy Gift이벤트 유무에따라 활성화/비활성화 할 수 있도록 기능 개선  
	void			SetGiftEventTime( DWORD time )	{ m_GiftEventTime = time; }
	void			SetGiftEvent(BOOL bGift) ;
};



#endif