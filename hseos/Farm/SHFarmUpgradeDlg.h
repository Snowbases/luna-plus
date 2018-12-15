#ifndef _FARMUPGRADE_H
#define _FARMUPGRADE_H

#include "./Interface/cDialog.h"
#include "ImageNumber.h"

class cStatic;

class CSHFarmUpgradeDlg : public cDialog
{
protected:
	cStatic*		m_CurGrade;
	cStatic*		m_NextGrade;
	cStatic*		m_NeedGold;
	cStatic*		m_NeedFamilyHonor;

	int				m_nFarmZone;
	int				m_nFarmID;
	int				m_nUpgradeKind;

public:
	CSHFarmUpgradeDlg() ;
	~CSHFarmUpgradeDlg() ;

	void		 	Linking();
	virtual void 	SetActive( BOOL val );
	virtual void 	Render();

	void			SetUpgradeTarget(int nFarmZone, int nFarmID, int nUpgradeKind) 
					{
						m_nFarmZone = nFarmZone;
						m_nFarmID = nFarmID;
						m_nUpgradeKind = nUpgradeKind;
					}

	int				GetFarmZone()			{ return m_nFarmZone; }
	int				GetFarmID()				{ return m_nFarmID; }
	int				GetFarmUpgradeKind()	{ return m_nUpgradeKind; }
};



#endif